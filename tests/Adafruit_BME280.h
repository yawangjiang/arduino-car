#pragma once
#include <cstdint>
extern bool testBmeReady;
extern float testTemperature, testHumidity, testPressure;
class Adafruit_BME280 {
 public:
  bool begin(uint8_t = 0x76) { return testBmeReady; }
  float readTemperature() { return testTemperature; }
  float readHumidity() { return testHumidity; }
  float readPressure() { return testPressure; }
};
