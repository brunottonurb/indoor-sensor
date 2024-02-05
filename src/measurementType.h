#ifndef MEASUREMENT_TYPE_H
#define MEASUREMENT_TYPE_H

struct measurementType {
  unsigned long timestamp; // Timestamp of the measurement
  float temperature; // c
  float pressure; // hPa
  float humidity; // %
  float gas_resistance; // KOhms 
};

#endif // MEASUREMENT_TYPE_H
