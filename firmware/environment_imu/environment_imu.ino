#include <Arduino.h>
#include <math.h>
// 期末报告 3.4；Z 轴阈值保持，补充初始化/读数检查与输出单位。
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MPU6050.h>

int enablePin1 = 9;
int enablePin2 = 10;
int in1Pin = 6;
int in2Pin = 7;
int in3Pin = 4;
int in4Pin = 5;

Adafruit_BME280 bme;
Adafruit_MPU6050 mpu;

const int fullSpeed = 255;
const float LIFT_TRIGGER = -8.0f;
const float DROP_STOP = -15.0f;

//
unsigned long printTimer = 0;
const unsigned long printInterval = 1200;
//
bool isRunning = false;
bool sensorsReady = false;

void setMotor(int left, int right, bool revL, bool revR) {

    analogWrite(enablePin1, left);
    digitalWrite(in1Pin, !revL);
    digitalWrite(in2Pin, revL);
    analogWrite(enablePin2, right);
    digitalWrite(in3Pin, !revR);
    digitalWrite(in4Pin, revR);
}
void carForward() { setMotor(fullSpeed, fullSpeed, false, false); }
void carStop() { setMotor(0, 0, false, false); }

void setup() {
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);
    pinMode(in3Pin, OUTPUT);
    pinMode(in4Pin, OUTPUT);
    pinMode(enablePin1, OUTPUT);
    pinMode(enablePin2, OUTPUT);
    carStop();

    Serial.begin(9600);

    Wire.begin();
    bool bmeReady = bme.begin(0x76);
    bool mpuReady = mpu.begin(0x68);
    sensorsReady = bmeReady && mpuReady;
    if (!sensorsReady) { Serial.println(F("Sensor init failed; motors stopped")); return; }
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
}

void loop() {
    if (!sensorsReady) { carStop(); return; }
    unsigned long now = millis();
    sensors_event_t accel, gyro, mpuTemp;
    if (!mpu.getEvent(&accel, &gyro, &mpuTemp) || !isfinite(accel.acceleration.z)) {
        isRunning = false;
        carStop();
        return;
    }
    float zRaw = accel.acceleration.z;
    float Z = zRaw + 1.6;

    //
    if(zRaw > LIFT_TRIGGER)
    {
         isRunning = true;

    }
    if(zRaw < DROP_STOP)
    {
         isRunning = false;
    }

    //
    if(isRunning)
         carForward();
    else
         carStop();

    // 1.2s
    if (now - printTimer >= printInterval)
    {
         printTimer = now;
         // BME280
         float temp = bme.readTemperature();
         float hum = bme.readHumidity();
         float press = bme.readPressure() / 100.0f;
         Serial.print(F("temperature_C="));
         Serial.print(temp, 1);
         Serial.print(F(" humidity_pct="));
         Serial.print(hum, 1);
         Serial.print(F(" pressure_hPa="));
         Serial.print(press, 1);
         Serial.println();

         // MPU
         Serial.print(F("accel_m_s2(X/Y/Z_display): "));
         Serial.print(accel.acceleration.x, 2); Serial.print(" / ");
         Serial.print(accel.acceleration.y, 2); Serial.print(" / ");
         Serial.println(Z, 2);

         Serial.print(F("gyro_rad_s(X/Y/Z): "));
         Serial.print(gyro.gyro.x, 2); Serial.print(" / ");
         Serial.print(gyro.gyro.y, 2); Serial.print(" / ");
         Serial.println(gyro.gyro.z, 2);

         Serial.println("------------------------------");
    }
}
