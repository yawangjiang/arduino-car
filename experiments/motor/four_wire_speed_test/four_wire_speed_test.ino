#include <Arduino.h>
// Lab 2 四线软件 PWM 实验；保留原参数与顺序输出。
void move(int l, int r);
void pwmMotor(int p, int n, int s);
int in1 = 2;
int in2 = 3;
int in3 = 4;
int in4 = 5;

int l_spd = 245;
int r_spd = 255;

bool run = false;

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

    delay(1000);
    run = true;
}

void loop() {
  if(run){
    move(l_spd, r_spd);
  }
}

void move(int l, int r) {
  pwmMotor(in1, in2, l);
  pwmMotor(in4, in3, r);
}

void pwmMotor(int p, int n, int s) {
  if(s <= 0){
    digitalWrite(p, LOW);
    digitalWrite(n, LOW);
    return;
  }
  if(s >= 255){

     digitalWrite(p, HIGH);
     digitalWrite(n, LOW);
     return;
    }
    digitalWrite(p, HIGH);
    digitalWrite(n, LOW);
    delayMicroseconds(s * 4);
    digitalWrite(p, LOW);
    delayMicroseconds((255 - s) * 4);
}
