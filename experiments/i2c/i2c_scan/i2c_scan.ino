#include <Arduino.h>
#include <Wire.h>
// 整理时补充的接线诊断工具；报告原文未附此草图。
void setup() {
  Serial.begin(115200);
  Wire.begin();
}

void loop() {
  Serial.println(F("I2C addresses:"));
  for (uint8_t address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print(F("0x"));
      if (address < 16) Serial.print('0');
      Serial.println(address, HEX);
    }
  }
  delay(2000);
}
