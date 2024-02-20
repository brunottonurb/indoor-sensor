#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "measurement_type.h"
#include <cstddef> // for std::size_t

// hack to get a degree symbol
const char ds[2] = {static_cast<char>(248), '\0'};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup_display() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    throw "SSD1306 Display setup failed";
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
}

void drawText(const char *text) {
  display.clearDisplay();
  display.setTextSize(1);       // Normal 1:1 pixel scale
  display.setTextColor(WHITE);  // Draw white text
  display.setCursor(0, 0);      // Start at top-left corner
  display.cp437(true);          // Use full 256 char 'Code Page 437' font

  display.println(text);  // Print the text to the screen
  display.display();      // Display the content
}

void displaySensorValues(measurementType measurement) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.cp437(true);

  display.print(F("Temperature: "));
  display.print(measurement.temperature);
  display.print(F(" "));
  display.print(ds);
  display.println(F("C"));

  display.print(F("Pressure: "));
  display.print(measurement.pressure);
  display.println(F(" hPa"));

  display.print(F("Humidity: "));
  display.print(measurement.humidity);
  display.println(F(" %"));

  display.print(F("Gas Resistance: "));
  display.print(measurement.gas_resistance);
  display.println(F(" KOhms"));

  display.display();
}

void plotHumidityGraph(
  measurementType measurements[],
  std::size_t measurementsSize,
  unsigned long graphLength
) {
  // // generate some random data for testing
  // int a = 50;
  // measurementType measurements[100];
  // for (int i = 0; i < 100; i++) {
  //   measurements[i].timestamp = i;
  //   measurements[i].humidity = a + random(-5, 5);
  // }
  // int measurementsSize = 100;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.cp437(true);

  display.println(F("Humidity (%)"));

  const int graphWidth = SCREEN_WIDTH;
  const int graphHeight = SCREEN_HEIGHT - 10; // Leave a gap at the top for the title
  const int yOffset = 10; // Start drawing below the gap

  // the graph goes for 4 hours (or graphLength)
  // left is 4 hours before the newest measurement
  const unsigned long endTime = measurements[measurementsSize -  1].timestamp;
  const unsigned long startTime = measurements[0].timestamp;

  // keep track of max and min values
  // so we can add a label to these points
  measurementType minHumidity = measurements[0];
  measurementType maxHumidity = measurements[0];

  // draw the dots for the measurements
  int x;
  int y;
  for (int i = 0; i < (int)measurementsSize; i++) {
    x = map(measurements[i].timestamp, startTime, endTime, 0, graphWidth);
    y = map(measurements[i].humidity, 0, 100, yOffset + graphHeight, yOffset);
  
    Serial.print("i: ");
    Serial.print(i);
    Serial.print(" timestamp: ");
    Serial.print(measurements[i].timestamp);
    Serial.print(" startTime: ");
    Serial.print(startTime);
    Serial.print(" endTime: ");
    Serial.print(endTime);
    Serial.print(" graphWidth: ");
    Serial.print(graphWidth);
    Serial.print(" x: ");
    Serial.print(x);
    Serial.print(" y: ");
    Serial.println(y);

    display.drawPixel(x, y, WHITE);

    if (i > 0) {
      if (measurements[i].humidity < minHumidity.humidity) {
        minHumidity = measurements[i];
      }
      if (measurements[i].humidity > maxHumidity.humidity) {
        maxHumidity = measurements[i];
      }
    }
  }

  // add labels for max and min values
  display.setCursor(map(maxHumidity.timestamp, startTime, endTime, 0, graphWidth), map(maxHumidity.humidity, 0, 100, yOffset + graphHeight, yOffset));
  display.print(maxHumidity.humidity);
  display.println("%");

  const int minX = map(minHumidity.timestamp, startTime, endTime, 0, graphWidth);
  const int maxX = map(maxHumidity.timestamp, startTime, endTime, 0, graphWidth);
  const int minY = map(minHumidity.humidity, 0, 100, yOffset + graphHeight, yOffset);
  const int maxY = map(maxHumidity.humidity, 0, 100, yOffset + graphHeight, yOffset);

  if (minX != maxX && minY != maxY) { // do not print min if it's the same as max
    display.setCursor(minX, minY);
    display.print(minHumidity.humidity);
    display.println("%");
  }

  display.display();
}

void plotTemperatureGraph(
  measurementType measurements[],
  std::size_t measurementsSize,
  unsigned long graphLength
) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.cp437(true);

  display.print(F("Temperature ("));
  display.print(ds);
  display.println(F("C)"));

  const int graphWidth = SCREEN_WIDTH;
  const int graphHeight = SCREEN_HEIGHT - 10; // Leave a gap at the top for the title
  const int yOffset = 10; // Start drawing below the gap

  // the graph goes for 4 hours (or graphLength)
  // left is 4 hours before the newest measurement
  const unsigned long endTime = measurements[measurementsSize -  1].timestamp;
  const unsigned long startTime = measurements[0].timestamp;

  // keep track of max and min values
  // so we can add a label to these points
  measurementType minTemperature = measurements[0];
  measurementType maxTemperature = measurements[0];

  // draw the dots for the measurements
  int x;
  int y;
  for (int i = 0; i < (int)measurementsSize; i++) {
    // disregard temps below 0 and above 45, this is an indoor sensor
    x = map(measurements[i].timestamp, startTime, endTime, 0, graphWidth);
    y = map(measurements[i].temperature, 0, 45, yOffset + graphHeight, yOffset);
    display.drawPixel(x, y, WHITE);

    if (i > 0) {
      if (measurements[i].temperature < minTemperature.temperature) {
        minTemperature = measurements[i];
      }
      if (measurements[i].temperature > maxTemperature.temperature) {
        maxTemperature = measurements[i];
      }
    }
  }

  // add labels for max and min values
  display.setCursor(map(maxTemperature.timestamp, startTime, endTime, 0, graphWidth), map(maxTemperature.temperature, -20, 40, yOffset + graphHeight, yOffset));
  display.print(maxTemperature.temperature);
  display.print(ds);
  display.println("C");
  
  const int minX = map(minTemperature.timestamp, startTime, endTime, 0, graphWidth);
  const int maxX = map(maxTemperature.timestamp, startTime, endTime, 0, graphWidth);
  const int minY = map(minTemperature.temperature, -20, 40, yOffset + graphHeight, yOffset);
  const int maxY = map(maxTemperature.temperature, -20, 40, yOffset + graphHeight, yOffset);

  if (minX != maxX && minY != maxY) { // do not print min if it's the same as max
    display.setCursor(minX, minY);
    display.print(minTemperature.temperature);
    display.print(ds);
    display.println("C");
  }

  display.display();
}
