#ifndef SENSOR_H
#define SENSOR_H

#include <Adafruit_BME680.h> // Include the library for the sensor
#include "measurement_type.h" // Include the measurementType header file

extern Adafruit_BME680 bme;

void setup_sensor();

measurementType takeMeasurement();

#endif // SENSOR_H