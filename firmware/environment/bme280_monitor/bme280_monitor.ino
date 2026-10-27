/* 从期末报告 3.4 的 BME280 采集部分拆分，不控制电机，不依赖 MPU6050。 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <math.h>

Adafruit_BME280 bme;
const uint8_t sensorAddress = 0x76;
const unsigned long printInterval = 1200;
unsigned long printTimer = 0;
bool sensorReady = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sensorReady = bme.begin(sensorAddress);
  if (!sensorReady) {
    Serial.println(F("BME280 init failed; check wiring, address and sensor model"));
    return;
  }
  Serial.println(F("time_ms,temperature_C,humidity_pct,pressure_hPa"));
}

void loop() {
  if (!sensorReady) return;
  unsigned long now = millis();
  if (now - printTimer < printInterval) return;
  printTimer = now;

  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0f;
  if (!isfinite(temperature) || !isfinite(humidity) || !isfinite(pressure)) {
    Serial.println(F("BME280 invalid sample"));
    return;
  }
  Serial.print(now); Serial.print(',');
  Serial.print(temperature, 2); Serial.print(',');
  Serial.print(humidity, 2); Serial.print(',');
  Serial.println(pressure, 2);
}
