#pragma once

#include <cstdlib>
#include <cstdint>
#include <sstream>

// Host-side API stub. This header is not used by Arduino builds.
constexpr int A0 = 14;
constexpr int A1 = 15;
constexpr int A2 = 16;
constexpr int INPUT = 0;
constexpr int OUTPUT = 1;
constexpr int HIGH = 1;
constexpr int LOW = 0;
constexpr int BIN = 2;
constexpr int HEX = 16;
using byte = uint8_t;
using boolean = bool;
#define F(value) value

using std::abs;

struct SerialStub {
  long baud = 0;
  std::ostringstream output;
  void begin(long value) { baud = value; }
  template <typename T> void print(const T& value) { output << value; }
  template <typename T> void println(const T& value) { output << value << '\n'; }
  template <typename T> void print(const T& value, int) { output << value; }
  template <typename T> void println(const T& value, int) { output << value << '\n'; }
  void println() { output << '\n'; }
  explicit operator bool() const { return true; }
};

extern SerialStub Serial;
void pinMode(int pin, int mode);
int analogRead(int pin);
void analogWrite(int pin, int value);
void digitalWrite(int pin, int value);
unsigned long millis();
unsigned long micros();
void delay(unsigned long value);
void delayMicroseconds(unsigned int value);
unsigned long pulseIn(int pin, int level, unsigned long timeout);
void randomSeed(unsigned long value);
long random(long low, long high);

inline int constrain(int value, int low, int high) {
  return value < low ? low : (value > high ? high : value);
}
