#include <Arduino.h>
// Lab 2 六线硬件 PWM 实验。
void move(int l, int r, bool dir);
int enA = 9;
int enB = 10;
int in1 = 2;
int in2 = 3;
int in3 = 4;
int in4 = 5;

int l_spd = 249;
int r_spd = 255;

bool run = false;

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

    delay(1000);
    run = true;
}

void loop() {
  bool dir = 0;
  if(run){
    move(l_spd, r_spd, dir);
  }
}

void move(int l, int r, bool dir) {
  analogWrite(enA, l);
  digitalWrite(in1, !dir);
  digitalWrite(in2, dir);

    analogWrite(enB, r);
    digitalWrite(in3, dir);
    digitalWrite(in4, !dir);
}
