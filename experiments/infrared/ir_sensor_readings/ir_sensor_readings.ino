#include <Arduino.h>
// Lab 3 三路采样，恢复 PDF 代码排版符号；左 A2 / 中 A1 / 右 A0。
void setup() { Serial.begin(9600); }

void loop() {
  int L = analogRead(A2); delay(2);
  int C = analogRead(A1); delay(2);
  int R = analogRead(A0); delay(2);
  Serial.print(F("LEFT: ")); Serial.print(L);
  Serial.print(F("\tMIDDLE: ")); Serial.print(C);
  Serial.print(F("\tRIGHT: ")); Serial.println(R);
  delay(50);
}
