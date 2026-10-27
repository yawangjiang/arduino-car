#include <Arduino.h>
// Lab 1 原采样程序；Y 开始，N 停止，A0 采样，D4 为原控制输出。

int Pin = 4;
int index = 1;
char key = 'W';

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode (Pin, OUTPUT);
  Serial.println("Y:start  N:stop");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    key = Serial.read();
    delay(3);
    while (Serial.available() > 0) {
      Serial.read();
    }
 }

  if (key == 'Y') {
    digitalWrite (Pin, HIGH);
    int val = analogRead(A0);
    Serial.print(index);
    Serial.print(" : ");
    Serial.println(val);
    delay(3000);
    index++;
  } else if (key == 'N') {
    digitalWrite(Pin, LOW);
    Serial.println("Stopped");
    Serial.println("Y:start  N:stop");
    index = 1;
    key = 'W';
  }
}
