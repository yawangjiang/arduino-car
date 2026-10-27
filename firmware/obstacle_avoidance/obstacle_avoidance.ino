/*
 * 根据期末报告 3.3 整理。被裁切的 enterState() 根据状态名称及已有函数补全。
 * 修正扫描方向 byte 回绕、停车后继续前进；无效测距时停车等待有效读数。
 * 角度、PWM 与运动时长沿用报告参数，实车仍需标定。
 */
#include <Arduino.h>
#include <Servo.h>

const byte enablePin1 = 5;
const byte enablePin2 = 6;
const byte in1Pin = 11;
const byte in2Pin = 10;
const byte in3Pin = 8;
const byte in4Pin = 9;
const byte servoPin = 3;
const byte inputPin = 13;
const byte outputPin = 12;

const int leftCompensate = -3;
const int rightCompensate = 3;
const byte baseSpeed = 120;
const byte backSpeed = 150;
const byte pivotSpeed = 160;
const byte backArcInner = 100;
const byte backArcOuter = 160;
const byte centerAngle = 87;
const byte sweepLeft = 138;
const byte sweepRight = 42;
const byte wideLeft = 15;
const byte wideRight = 165;
const byte nearDist = 40;
const byte veryClose = 12;
const unsigned int maxValid = 400;
const unsigned long sonicTimeout = 8000;
const unsigned long servoMoveInt = 7;
const unsigned long checkInt = 40;
const unsigned long settleTime = 300;
const unsigned long backTime = 720;
const unsigned long longBack = 850;
const unsigned long pivot60Time = 175;
const unsigned long recoverTime = 55;

enum State : byte {
  FORWARD, STOPPING, LOOK_LEFT, LOOK_RIGHT, BACK_LEFT, BACK_RIGHT,
  TURN_LEFT, TURN_RIGHT, RECOVER, WAIT_FOR_ECHO
};

Servo myservo;
State state = FORWARD;
unsigned long stateStart = 0;
unsigned long lastServoMove = 0;
unsigned long lastCheck = 0;
int8_t sweepDir = 1;
byte sweepAngle = centerAngle;
unsigned int Fdist = 0;
unsigned int Ldist = 0;
unsigned int Rdist = 0;

void enterState(State newState);
unsigned int readDistance();
void stopMotors();
void advance();
void back();
void backArcLeft();
void backArcRight();
void pivotLeft();
void pivotRight();

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0) ^ micros());
  pinMode(enablePin1, OUTPUT);
  pinMode(enablePin2, OUTPUT);
  pinMode(in1Pin, OUTPUT); pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT); pinMode(in4Pin, OUTPUT);
  pinMode(inputPin, INPUT); pinMode(outputPin, OUTPUT);
  myservo.attach(servoPin);
  myservo.write(centerAngle);
  stopMotors();
  delay(250);
  // 首次有效测距前保持停车。
  enterState(WAIT_FOR_ECHO);
}

void loop() {
  unsigned long now = millis();
  switch (state) {
    case FORWARD:
      if (now - lastServoMove >= servoMoveInt) {
        lastServoMove = now;
        sweepAngle += sweepDir * 2;
        if (sweepAngle >= sweepLeft) { sweepAngle = sweepLeft; sweepDir = -1; }
        else if (sweepAngle <= sweepRight) { sweepAngle = sweepRight; sweepDir = 1; }
        myservo.write(sweepAngle);
      }
      if (now - lastCheck >= checkInt) {
        lastCheck = now;
        Fdist = readDistance();
        if (Fdist == 0) { enterState(WAIT_FOR_ECHO); break; }
        if (Fdist <= nearDist) { enterState(STOPPING); break; }
      }
      advance();
      break;
    case STOPPING:
      if (now - stateStart >= 20) enterState(LOOK_LEFT);
      break;
    case LOOK_LEFT:
      if (now - stateStart >= settleTime) {
        Ldist = readDistance();
        enterState(Ldist == 0 ? WAIT_FOR_ECHO : LOOK_RIGHT);
      }
      break;
    case LOOK_RIGHT:
      if (now - stateStart >= settleTime) {
        Rdist = readDistance();
        if (Rdist == 0) enterState(WAIT_FOR_ECHO);
        else enterState(Ldist >= Rdist ? BACK_LEFT : BACK_RIGHT);
      }
      break;
    case BACK_LEFT:
    case BACK_RIGHT:
      if (now - stateStart >= (Fdist <= veryClose ? longBack : backTime)) {
        stopMotors();
        enterState(random(0, 2) ? TURN_LEFT : TURN_RIGHT);
      }
      break;
    case TURN_LEFT:
    case TURN_RIGHT:
      if (now - stateStart >= pivot60Time) {
        stopMotors();
        enterState(RECOVER);
      }
      break;
    case RECOVER:
      // 沿用报告的 55 ms 过渡前行，之后回到等待有效测距状态。
      if (now - stateStart >= recoverTime) enterState(WAIT_FOR_ECHO);
      else advance();
      break;
    case WAIT_FOR_ECHO:
      // 转回中位后先等待舵机稳定；无回波不等于前方畅通。
      stopMotors();
      if (now - stateStart >= settleTime && now - lastCheck >= checkInt) {
        lastCheck = now;
        Fdist = readDistance();
        if (Fdist != 0) enterState(Fdist <= nearDist ? STOPPING : FORWARD);
      }
      break;
  }
}

void enterState(State newState) {
  state = newState;
  stateStart = millis();
  Serial.print(F("state=")); Serial.println(static_cast<int>(state));
  switch (state) {
    case FORWARD:
      sweepAngle = centerAngle; sweepDir = 1;
      myservo.write(centerAngle); advance(); break;
    case STOPPING: stopMotors(); break;
    case LOOK_LEFT: stopMotors(); myservo.write(wideLeft); break;
    case LOOK_RIGHT: stopMotors(); myservo.write(wideRight); break;
    case BACK_LEFT: myservo.write(centerAngle); backArcLeft(); break;
    case BACK_RIGHT: myservo.write(centerAngle); backArcRight(); break;
    case TURN_LEFT: myservo.write(centerAngle); pivotLeft(); break;
    case TURN_RIGHT: myservo.write(centerAngle); pivotRight(); break;
    case RECOVER: myservo.write(centerAngle); advance(); break;
    case WAIT_FOR_ECHO: stopMotors(); myservo.write(centerAngle); break;
  }
}

unsigned int readDistance() {
  digitalWrite(outputPin, LOW); delayMicroseconds(2);
  digitalWrite(outputPin, HIGH); delayMicroseconds(10);
  digitalWrite(outputPin, LOW);
  unsigned long duration = pulseIn(inputPin, HIGH, sonicTimeout);
  if (duration == 0) return 0;
  unsigned long distance = duration / 58;
  return distance > 0 && distance <= maxValid ? static_cast<unsigned int>(distance) : 0;
}

void stopMotors() {
  digitalWrite(in1Pin, LOW); digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, LOW); digitalWrite(in4Pin, LOW);
  analogWrite(enablePin1, 0); analogWrite(enablePin2, 0);
}

void advance() {
  int leftSpeed = constrain(baseSpeed + leftCompensate, 0, 255);
  int rightSpeed = constrain(baseSpeed + rightCompensate, 0, 255);
  digitalWrite(in1Pin, LOW); digitalWrite(in2Pin, HIGH);
  digitalWrite(in3Pin, HIGH); digitalWrite(in4Pin, LOW);
  // 原程序通道 1 对应右轮，通道 2 对应左轮。
  analogWrite(enablePin1, rightSpeed); analogWrite(enablePin2, leftSpeed);
}

void back() {
  digitalWrite(in1Pin, HIGH); digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, LOW); digitalWrite(in4Pin, HIGH);
  analogWrite(enablePin1, constrain(backSpeed + rightCompensate, 0, 255));
  analogWrite(enablePin2, constrain(backSpeed + leftCompensate, 0, 255));
}

void backArcLeft() {
  digitalWrite(in1Pin, HIGH); digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, LOW); digitalWrite(in4Pin, HIGH);
  analogWrite(enablePin1, backArcInner); analogWrite(enablePin2, backArcOuter);
}

void backArcRight() {
  digitalWrite(in1Pin, HIGH); digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, LOW); digitalWrite(in4Pin, HIGH);
  analogWrite(enablePin1, backArcOuter); analogWrite(enablePin2, backArcInner);
}

void pivotLeft() {
  digitalWrite(in1Pin, LOW); digitalWrite(in2Pin, HIGH);
  digitalWrite(in3Pin, LOW); digitalWrite(in4Pin, HIGH);
  analogWrite(enablePin1, pivotSpeed); analogWrite(enablePin2, pivotSpeed);
}

void pivotRight() {
  digitalWrite(in1Pin, HIGH); digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, HIGH); digitalWrite(in4Pin, LOW);
  analogWrite(enablePin1, pivotSpeed); analogWrite(enablePin2, pivotSpeed);
}
