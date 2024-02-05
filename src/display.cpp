#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "measurementType.h"

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
  display.println(F(" *C"));

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

void plotGraphHumidityGraph(measurementType measurements[], int maxMeasurements, int measurementInterval) {
  // Clear display buffer
  display.clearDisplay();

  // Define graph dimensions
  const int graphWidth = SCREEN_WIDTH;
  const int graphHeight = SCREEN_HEIGHT - 10; // Leave a gap at the top for the title
  const int yOffset = 10; // Start drawing below the gap

  Serial.println("draw vertical lines");

  // // Draw vertical lines at hour intervals
  // for (int hour = 1; hour < maxMeasurements / (measurementInterval / 3600000); hour++) {
  //   int x = map(hour * (measurementInterval / 3600000), 0, maxMeasurements, 0, graphWidth);
  //   display.drawFastVLine(x, yOffset, graphHeight - yOffset, SSD1306_WHITE);
  // }

  // Find maximum and minimum humidity values
  float maxHumidity = 0;
  float minHumidity = 100; // Initialize with a value higher than any possible humidity
  int count = 0; // Counter for the number of valid measurements
  for (int i = 0; i < maxMeasurements; i++) {
    if (measurements[i].humidity > 0) {
      count++;
      if (measurements[i].humidity > maxHumidity) {
        maxHumidity = measurements[i].humidity;
      }
      if (measurements[i].humidity < minHumidity) {
        minHumidity = measurements[i].humidity;
      }
    }
  }

  Serial.println("plot data points");

  // Plot humidity data points
  for (int i = 0; i < maxMeasurements; i++) {
    int x = map(i, 0, maxMeasurements - 1, 0, graphWidth);
    if (measurements[i].humidity == 0) {
      continue;
    }
    int y = map(measurements[i].humidity, 0, 100, graphHeight, yOffset);
    display.drawPixel(x, y, SSD1306_WHITE);

    // Label maximum value
    if (measurements[i].humidity == maxHumidity) {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.cp437(true);
      display.setCursor(x + 5, y - 5); // Adjust position for label
      display.print((int)maxHumidity); // Print the maximum humidity value
      display.println("%");
    }

    // Label minimum value only if there are more than one measurement and max != min
    if (count > 1 && (int)minHumidity != (int)maxHumidity && measurements[i].humidity == minHumidity) {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.cp437(true);
      display.setCursor(x + 5, y + 5); // Adjust position for label
      display.print((int)minHumidity); // Print the minimum humidity value
      display.println("%");
    }
  }

  // Display title
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.cp437(true);
  display.setCursor(0, 0);
  display.println("Humidity");

  // Update display
  display.display();
}

