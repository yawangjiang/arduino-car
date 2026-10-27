#pragma once
#include "Arduino.h"
#include "Wire.h"
#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

SerialStub Serial;
WireStub Wire;
std::array<int, 20> modes{}, inputs{}, pwm{}, levels{};
unsigned long testMillis = 0;
unsigned long testPulse = 5800;
long testRandom = 0;
bool testAdsReady = true, testBmeReady = true, testMpuReady = true, testMpuReadReady = true;
int16_t testAdc[3] = {0, 6000, 0};
float testTemperature = 25, testHumidity = 50, testPressure = 101325;
float testAx = 0, testAy = 0, testAz = -9.8f, testGx = 0, testGy = 0, testGz = 0;

void pinMode(int pin, int mode) { modes.at(pin) = mode; }
int analogRead(int pin) { return inputs.at(pin); }
void analogWrite(int pin, int value) { pwm.at(pin) = value; }
void digitalWrite(int pin, int value) { levels.at(pin) = value; }
unsigned long millis() { return testMillis; }
unsigned long micros() { return testMillis * 1000; }
void delay(unsigned long value) { testMillis += value; }
void delayMicroseconds(unsigned int) {}
unsigned long pulseIn(int, int, unsigned long timeout) { return testPulse > timeout ? 0 : testPulse; }
void randomSeed(unsigned long) {}
long random(long, long) { return testRandom; }

int assertions = 0;
void expect(bool condition, const std::string& message) {
  ++assertions;
  if (!condition) throw std::runtime_error(message);
}
void clearSerial() { Serial.output.str(""); Serial.output.clear(); }
