#include <Adafruit_BME680.h>
#include "measurement_type.h"

Adafruit_BME680 bme; // Define the sensor object

void setup_sensor() {
  if (!bme.begin()) {
    const char *no_sensor_message = "Could not find a valid BME680 sensor, check wiring!";
    throw no_sensor_message;
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

measurementType takeMeasurement() {
  measurementType measurement;
  // this is a blocking way to do this, there may be a way to do it async
  if (! bme.performReading()) {
    throw "Failed to perform reading :(";
  }
  measurement.temperature = bme.temperature;
  measurement.humidity = bme.humidity;
  measurement.pressure = bme.pressure / 100.0;
  measurement.gas_resistance = bme.gas_resistance / 1000.0;
  measurement.timestamp = millis();
  return measurement;
}

// unsigned long beginMeasurement() {
//   unsigned long endTime = bme.beginReading();
//   if (endTime == 0) {
//     throw "Failed to begin reading :(";
//   }
//   return endTime;
// }

// void endMeasurement(measurementType &measurement, unsigned long endTime) {
//   if (!bme.endReading()) {
//     throw "Failed to end reading :(";
//   }
//   measurement.temperature = bme.temperature;
//   measurement.humidity = bme.humidity;
//   measurement.pressure = bme.pressure / 100.0;
//   measurement.gas_resistance = bme.gas_resistance / 1000.0;
//   measurement.timestamp = endTime;
// }

