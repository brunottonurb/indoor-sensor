#ifndef SENSOR_H
#define SENSOR_H

#include <Adafruit_BME680.h> // Include the library for the sensor
#include "measurementType.h" // Include the measurementType header file

extern Adafruit_BME680 bme; // Declare the sensor object as an extern

void setup_sensor(); // Declare the setup function

void takeMeasurement(measurementType &measurement); // Declare the function to take a measurement

#endif // SENSOR_H