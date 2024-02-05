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
#include "measurementType.h"
#include "db.h"
#include "sensor.h"

const int maxMeasurements = GRAPH_DURATION / MEASUREMENT_INTERVAL; // Calculate maximum number of measurements based on graph duration and measurement interval
measurementType measurements[maxMeasurements];
int measurementIndex = 0;

const unsigned long debounceDelay = 50;     // the debounce time; increase if the output flickers
unsigned long lastMeasurementTime = 0;
int prev_button_state = HIGH; // the previous state from the input pin
int button_state;    // the current reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the button input was sampled
int displayState = 0; // 0 == off, 1 == text, 2 == graph
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

  lastMeasurementTime = millis();
}

void loop() {
  unsigned long currentTime = millis();

  // Check if it's time to take a measurement
  if (currentTime - lastMeasurementTime >= MEASUREMENT_INTERVAL) {

    try {
      takeMeasurement(measurements[measurementIndex]);
    } catch (const char *error) {
      Serial.println(error);
      drawText(error);
    }

    lastMeasurementTime = currentTime;
    try {
      sendToInfluxDB(measurements[measurementIndex].temperature, measurements[measurementIndex].humidity, measurements[measurementIndex].pressure, measurements[measurementIndex].gas_resistance);
    } catch (const char *error) {
      Serial.println(error);
      drawText(error);
    }

    // Update timestamp of the measurement
    measurements[measurementIndex].timestamp = currentTime;

    // Update measurement index
    measurementIndex = (measurementIndex + 1) % maxMeasurements;
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
          displaySensorValues(measurements[measurementIndex]);
          displayTimeoutStartTime = millis(); // Start the display timeout
        } else if (displayState == 1) { // if the data display is on, go to the graph display and reset the timeout
          displayState = 2;
          plotGraphHumidityGraph(measurements, maxMeasurements, MEASUREMENT_INTERVAL);
          displayTimeoutStartTime = millis(); // restart the display timeout
        } else { //if (displayState == 2) { // clear the display
          displayState = 0;
          displayTimeoutStartTime = 0;
          display.clearDisplay();
          display.display();
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
