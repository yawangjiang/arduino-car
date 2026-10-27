#pragma once
class Servo {
 public:
  int angle = 90;
  int pin = -1;
  void attach(int value) { pin = value; }
  void write(int value) { angle = value; }
};
