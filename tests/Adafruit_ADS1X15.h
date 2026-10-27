#pragma once
#include <cstdint>
extern bool testAdsReady;
extern int16_t testAdc[3];
class Adafruit_ADS1115 {
 public:
  bool begin(uint8_t = 0x48) { return testAdsReady; }
  int16_t readADC_SingleEnded(int channel) { return testAdc[channel]; }
};
