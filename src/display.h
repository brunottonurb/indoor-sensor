#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "measurement_type.h"
#include <cstddef> // for std::size_t

extern Adafruit_SSD1306 display;
void setup_display();
void drawText(const char *text);
void displaySensorValues(measurementType measurement);
void plotHumidityGraph(measurementType measurements[], std::size_t measurementsSize, unsigned long graphLength);
void plotTemperatureGraph(measurementType measurements[], std::size_t measurementsSize, unsigned long graphLength);

#endif
