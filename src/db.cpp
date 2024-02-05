#include <ESP8266HTTPClient.h>
#include "config.h"

void sendToInfluxDB(float temperature, float humidity, float pressure, float gasResistance) {
  WiFiClient client;
  HTTPClient http;

  String data = "weather,location=" + String(LOCATION_NAME) + " temperature=" + String(temperature) + ",humidity=" + String(humidity) + ",pressure=" + String(pressure) + ",gas_resistance=" + String(gasResistance);

  Serial.println("Sending data to InfluxDB: " + data);

  http.begin(client, INFLUXDB_URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(data);

  if (httpCode > 0) {
    Serial.printf("[HTTP] POST request to InfluxDB returned code %d\n", httpCode);
  } else {
    Serial.printf("[HTTP] POST request to InfluxDB failed, error: %s\n", http.errorToString(httpCode).c_str());
    throw http.errorToString(httpCode).c_str();
  }

  http.end();
}
