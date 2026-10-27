#include <Arduino.h>
// 期末报告 3.2；保留原 PD 参数与找线时序，增加 ADC 初始化失败停车。
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;

int enablePin1 = 9;
int enablePin2 = 10;
int in1Pin = 6;
int in2Pin = 7;
int in3Pin = 4;
int in4Pin = 5;

boolean reverseL = 0, reverseR = 0;
const int maxSpeed = 100;
const int minSpeed = 25;
const int leftOffset = 5;

bool lockSpeed = false;
bool tempSpeedMode = false;
unsigned long tempSpeedStart = 0;
const unsigned long tempSpeedTime = 130;

bool reverseMode = false;
unsigned long reverseStart = 0;
const unsigned long reverseTime = 20;
const int reverseSpeed = 35;

int leftServoSpeed = 0, rightServoSpeed = 0;
const int irChannels[3] = {0, 1, 2};
int threshold = 6000;
int irSensors = 0b000;

float error = 0;
float errorLast = 0;

float Kp = 23;
float Ki = 0;
float Kd = 37;

float PID = 0, integral = 0, derivative = 0;

bool adcReady = false;
void Scan();
void getError();
void computePID();
void DriveByPID();
void Drive(int leftSpeed, int rightSpeed);
void setMotor(int left, int right, boolean revL, boolean revR);

void setup() {
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);
    pinMode(in3Pin, OUTPUT);
    pinMode(in4Pin, OUTPUT);
    pinMode(enablePin1, OUTPUT);
    pinMode(enablePin2, OUTPUT);
    Serial.begin(9600);
    Drive(0, 0);
    adcReady = ads.begin();
    if (!adcReady) Serial.println(F("ADS1115 init failed; motors stopped"));
}

void loop() {
    if (!adcReady) { Drive(0, 0); return; }
    Scan();
    getError();
    computePID();
    DriveByPID();

    Serial.print("Sensor State: ");Serial.print(irSensors, BIN);
    Serial.print(" | Error: ");Serial.print(error);
    Serial.print(" | Left Wheel: ");Serial.print(leftServoSpeed);
    Serial.print(" | Right Wheel: ");Serial.println(rightServoSpeed);
}

// Read three-channel infrared ADC value and synthesize state bitmask
void Scan() {
    irSensors = 0b000;
    for (int i = 0; i < 3; i++) {
        int val = ads.readADC_SingleEnded(irChannels[i]);
        bool isBlack = (val >= threshold);
        irSensors |= (isBlack << (2 - i));
    }
}

// Map sensor combination to trajectory deviation error
void getError() {

    errorLast = error;
    switch (irSensors) {
        case 0b100: error = 2.5; break;
        case 0b110: error = 1.2; break;
        case 0b010: error = 0; break;
        case 0b011: error = -1.9; break;
        case 0b001: error = -2.9; break;
        case 0b000: error = (errorLast <= 0) ? -2.9 : 2.0; break;
        case 0b111: error = -2.9; break;
        default: error = errorLast;
    }
}

// Discrete PID calculation, integral anti-windup limit
void computePID() {
    integral += error;
    derivative = error - errorLast;
    PID = Kp * error + Ki * integral + Kd * derivative;

    if (integral > 30) integral = 30;
    if (integral < -30) integral = -30;
}

// State machine: line lost reverse search, transition speed, normal PID differential speed
void DriveByPID() {
    unsigned long currentTime = millis();

    if(irSensors == 0b000 && !lockSpeed && !tempSpeedMode && !reverseMode){
        reverseMode = true;
        reverseStart = currentTime;
    }

    if(reverseMode){
        leftServoSpeed = -reverseSpeed;
        rightServoSpeed = -reverseSpeed;
        if(millis() - reverseStart >= reverseTime){
            reverseMode = false;
            lockSpeed = true;
        }
    }
    else if(lockSpeed == true){
        leftServoSpeed = 0;

        rightServoSpeed = 60;

        if(irSensors == 0b011){
            lockSpeed = false;
            tempSpeedMode = true;
            tempSpeedStart = millis();
        }
    }
    else if(tempSpeedMode == true){
        leftServoSpeed = 60;
        rightServoSpeed = 30;
        if(millis() - tempSpeedStart >= tempSpeedTime){
            tempSpeedMode = false;
        }
    }
    else{
        int runSpeed = maxSpeed;
        leftServoSpeed    = runSpeed + PID + leftOffset;
        rightServoSpeed = runSpeed - PID;

        if (leftServoSpeed < minSpeed)    leftServoSpeed = minSpeed;
        if (rightServoSpeed < minSpeed) rightServoSpeed = minSpeed;
        if (leftServoSpeed > runSpeed) leftServoSpeed = maxSpeed;
        if (rightServoSpeed > runSpeed) rightServoSpeed = maxSpeed;
    }

    Drive(leftServoSpeed, rightServoSpeed);
}

// Judge forward/reverse direction and call hardware setting function
void Drive(int leftSpeed, int rightSpeed) {
    int left = leftSpeed;
    int right = rightSpeed;
    reverseL = 0; reverseR = 0;

    if (left < 0) { left = -left; reverseL = 1; }
    if (right < 0) { right = -right; reverseR = 1; }

    setMotor(left, right, reverseL, reverseR);
}

// Actual L298N IO level configuration and PWM output

void setMotor(int left, int right, boolean revL, boolean revR) {
    analogWrite(enablePin1, left);
    digitalWrite(in1Pin, !revL);
    digitalWrite(in2Pin, revL);

    analogWrite(enablePin2, right);
    digitalWrite(in3Pin, !revR);
    digitalWrite(in4Pin, revR);
}
