# Indoor sensor firmware

## Hardware
- ESP8266 NodeMCU v3
- BME680 connected via I2C
- 0.96" OLED screen connected via I2C
- One button connected to D7 using internal pull-up resistor

## Features
- Display data in different ways when button is pressed
- Send data to influxDB

## Instructions
- Use VSCode with Platformio extension
- Add `src/secrets.h` with:
```cpp
#define WIFI_SSID <your_wifi_ssid>
#define WIFI_PASSWORD <your_wifi_password>
#define INFLUXDB_URL http://<url>/write?db=<db_name>
```
- Have a look at `src/config.h`, especially at `LOCATION_NAME`
