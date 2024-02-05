#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "measurementType.h"

extern Adafruit_SSD1306 display;
void setup_display();
void drawText(const char *text);
void displaySensorValues(measurementType measurement);
void plotGraphHumidityGraph(measurementType measurements[], int maxMeasurements, int measurementInterval);

#endif
