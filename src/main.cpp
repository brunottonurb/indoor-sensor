#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "secrets.h"
#include "display.h"
#include "measurement_type.h"
#include "db.h"
#include "sensor.h"
#include "circular-buffer.h"

const int maxMeasurements = GRAPH_DURATION / MEASUREMENT_INTERVAL; // Calculate maximum number of measurements based on graph duration and measurement interval
CircularBuffer<measurementType> measurements(maxMeasurements); // Create a circular buffer to store measurements

const unsigned long debounceDelay = 50;     // the debounce time; increase if the output flickers

int prev_button_state = HIGH; // the previous state from the input pin
int button_state;    // the current reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the button input was sampled
int displayState = 0; // 0 == off, 1 == text, 2 == graph humidity, 3 == graph temperature
unsigned long displayTimeoutStartTime = 0; // To store the start time of display, it should turn off after displayDuration*1000 seconds

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN, INPUT_PULLUP); // use internal pullup resistor

  try {
    setup_display();
  } catch (const char *error) {
    Serial.println(error);
  }

  try {
    setup_sensor();
  } catch (const char *error) {
    Serial.println(error);
    drawText(error);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    const char *connecting_message = "Connecting to WiFi...";
    Serial.println(connecting_message);
    drawText(connecting_message);
  }
  const char *success_message = "Connected to WiFi";
  Serial.println(success_message);
  drawText(success_message);

  delay(2000);  // Pause for 2 seconds

  // Clear the buffer and the display
  display.clearDisplay();
  display.display();
}

void printMeasurementBufferToSerial() {
  Serial.print("MeasurementsSize: ");
  Serial.println(measurements.getSize());
  // print header
  Serial.println("Index\tTemperature\tHumidity\tPressure\tGas Resistance\tTimestamp");
  for (int i = 0; i < (int)measurements.getSize(); i++) {
    Serial.print(i);
    Serial.print("\t");
    Serial.print(measurements.get(i).temperature);
    Serial.print("\t\t");
    Serial.print(measurements.get(i).humidity);
    Serial.print("\t\t");
    Serial.print(measurements.get(i).pressure);
    Serial.print("\t\t");
    Serial.print(measurements.get(i).gas_resistance);
    Serial.print("\t\t");
    Serial.println(measurements.get(i).timestamp);
  }
}

void loop() {
  unsigned long currentTime = millis();

  bool shouldTakeMeasurement = false;

  // Check if it's time to take a measurement
  if (measurements.getSize() == 0) {
    Serial.println("No measurements yet, taking one now");
    shouldTakeMeasurement = true;
  } else if (currentTime - measurements.get(-1).timestamp >= MEASUREMENT_INTERVAL) {
    Serial.println("Time to take a measurement");
    shouldTakeMeasurement = true;
  }

  if (shouldTakeMeasurement) {
    // If there are no measurements, take one
    measurementType measurement;
    try {
      measurement = takeMeasurement();
      measurements.put(measurement);
    } catch (const char *error) {
      Serial.println(error);
      drawText(error);
    }
    try {
      sendToInfluxDB(measurement.temperature, measurement.humidity, measurement.pressure, measurement.gas_resistance);
    } catch (const char *error) {
      Serial.println(error);
      drawText(error);
    }
  }

  int reading = digitalRead(BUTTON_PIN);  // read the state of the button
  if (reading != prev_button_state) {      // if the input state has changed
    lastDebounceTime = millis();           // reset the debouncing timer
  }

  if (millis() - lastDebounceTime > debounceDelay) { // if enough time has passed
    // determine if the button state has changed
    if (reading != button_state) {
      button_state = reading;

      if (button_state == HIGH) {  // button has been pressed
        if (displayState == 0) { // If display is not active, display to first screen
          displayState = 1;

          // printMeasurementBufferToSerial();

          // display last measurement
          if (measurements.getSize() > 0) {
            measurementType mostRecentMeasurement = measurements.getLast();
            displaySensorValues(mostRecentMeasurement);
          } else {
            Serial.println("No measurements to display");
          }
          displayTimeoutStartTime = millis(); // Start the display timeout
          Serial.println("Displaying sensor values");
        } else if (displayState == 1) { // if the data display is on, go to the graph display and reset the timeout
          displayState = 2;
          if (measurements.getSize() == 0) {
            Serial.println("No measurements to display");
            drawText("No measurements to display");
          }
          plotHumidityGraph(measurements.toArray(), measurements.getSize(), GRAPH_DURATION);
          displayTimeoutStartTime = millis(); // restart the display timeout
          Serial.println("Displaying humidity graph");
        } else if (displayState == 2) { // if the graph display is on, switch to temperature graph display and reset the timeout
          displayState = 3;
          if (measurements.getSize() == 0) {
            Serial.println("No measurements to display");
            drawText("No measurements to display");
          }
          plotTemperatureGraph(measurements.toArray(), measurements.getSize(), GRAPH_DURATION);
          displayTimeoutStartTime = millis(); // restart the display timeout
          Serial.println("Displaying temperature graph");
        } else if (displayState == 3) { // if the graph display is on, turn off the display and remove the timeout
          displayState = 0;
          displayTimeoutStartTime = 0;
          display.clearDisplay();
          display.display();
          Serial.println("Turning off display with button press");
        }
      }
    }
  }

  if (displayTimeoutStartTime > 0 && displayState > 0 && (millis() - displayTimeoutStartTime >= DISPLAY_DURATION)) { // If display is active and display duration has passed, deactivate display
    displayState = 0;
    displayTimeoutStartTime = 0;
    display.clearDisplay();
    display.display();
  }

  // save the current state as the last state, for next time through the loop
  prev_button_state = reading;
}
