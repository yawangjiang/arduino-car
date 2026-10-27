#include <Arduino.h>
// Lab 4 基础循迹；补全被 PDF 裁切的函数声明，保留原左右轮通道映射。
void Scan();
void UpdateDirection();
void Drive(int leftSpeed, int rightSpeed);
void setMotor(int leftservoSpeed, int rightservospeed, boolean reversel, boolean reverser);
// Pin definitions for two-way motors
int enablePin1 = 9;
int enablePin2 = 10;
int in1Pin = 2;
int in2Pin = 3;
int in3Pin = 4;
int in4Pin = 5;

boolean reverseL = 0;
boolean reverseR = 0;
const int maxSpeed = 180;

int leftServoSpeed = 0;
int rightServoSpeed = 0;

const int irPins[3] = {A5, A4, A3};
int irSensorDigital[3] = {0, 0, 0};
int threshold = 500;
int irSensors = 0b000;
int error = 0;
int errorLast = 0;

void setup() {
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  pinMode(enablePin2, OUTPUT);

    Serial.begin(9600);
}

void loop() {
  Scan();
  UpdateDirection();
  Drive(leftServoSpeed, rightServoSpeed);
}

void Scan() {
  irSensors = 0b000;
  for (int i = 0; i < 3; i++) {
    int sensorValue = analogRead(irPins[i]);
    if (sensorValue >= threshold) {
      irSensorDigital[i] = 1;
    } else {
      irSensorDigital[i] = 0;
    }
    int b = 2 - i;
    irSensors = irSensors + (irSensorDigital[i] << b);
  }
}

void UpdateDirection() {
  errorLast = error;
  switch (irSensors) {
    case 0b000: error = (errorLast < 0) ? -130 : 130; break;
    case 0b100: error = -80; break;
    case 0b110: error = -40; break;
    case 0b010: error = 0; break;
    case 0b011: error = 40; break;
    case 0b001: error = 80; break;
    case 0b111: error = 1; break;
    default: error = errorLast;
  }

    if (error >= 0) {
      leftServoSpeed = maxSpeed;
      rightServoSpeed = maxSpeed - error;
    } else {
      leftServoSpeed = maxSpeed + error;
      rightServoSpeed = maxSpeed;
    }
}

void Drive(int leftSpeed, int rightSpeed) {
  int right = rightSpeed;
  int left = leftSpeed;

    reverseL = 0;
    reverseR = 0;

    if (left < 0) { left = -left; reverseL = 1; }
    else if (left > maxSpeed) left = maxSpeed;

    if (right < 0) { right = -right; reverseR = 1; }
    else if (right > maxSpeed) right = maxSpeed;

    setMotor(left, right, reverseL, reverseR);
}

void setMotor(int leftservoSpeed, int rightservospeed, boolean reversel, boolean reverser) {
  analogWrite(enablePin1, rightservospeed);
  digitalWrite(in1Pin, !reverser);
  digitalWrite(in2Pin, reverser);

    analogWrite(enablePin2, leftservoSpeed);
    digitalWrite(in3Pin, !reversel);
    digitalWrite(in4Pin, reversel);
}
