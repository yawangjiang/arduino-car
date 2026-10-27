#include <Arduino.h>
// 从期末避障测距函数拆出，用于单独观测回波，不控制电机或舵机。
const byte trigPin = 12;
const byte echoPin = 13;
const unsigned long timeoutUs = 8000;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println(F("duration_us,distance_cm,valid"));
}

void loop() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long duration = pulseIn(echoPin, HIGH, timeoutUs);
  unsigned long distance = duration / 58;
  bool valid = duration != 0 && distance > 0 && distance <= 400;
  Serial.print(duration); Serial.print(',');
  Serial.print(valid ? distance : 0); Serial.print(',');
  Serial.println(valid ? 1 : 0);
  delay(200);
}
