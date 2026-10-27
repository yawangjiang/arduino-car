/* 从期末报告 3.4 拆出的六轴观测程序；输出原始三轴加速度与角速度，不控制电机。 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

Adafruit_MPU6050 mpu;
bool sensorReady = false;
unsigned long printTimer = 0;
const unsigned long printInterval = 100;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sensorReady = mpu.begin(0x68);
  if (!sensorReady) {
    Serial.println(F("MPU6050 init failed; check wiring and address"));
    return;
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  Serial.println(F("time_ms,ax_m_s2,ay_m_s2,az_m_s2,gx_rad_s,gy_rad_s,gz_rad_s,accel_norm_m_s2"));
}

void loop() {
  if (!sensorReady) return;
  unsigned long now = millis();
  if (now - printTimer < printInterval) return;
  printTimer = now;
  sensors_event_t accel, gyro, temperature;
  if (!mpu.getEvent(&accel, &gyro, &temperature)) {
    Serial.println(F("MPU6050 read failed"));
    return;
  }
  float x = accel.acceleration.x;
  float y = accel.acceleration.y;
  float z = accel.acceleration.z;
  if (!isfinite(x) || !isfinite(y) || !isfinite(z) ||
      !isfinite(gyro.gyro.x) || !isfinite(gyro.gyro.y) || !isfinite(gyro.gyro.z)) {
    Serial.println(F("MPU6050 invalid sample"));
    return;
  }
  float norm = sqrtf(x * x + y * y + z * z);
  Serial.print(now); Serial.print(',');
  Serial.print(x, 3); Serial.print(',');
  Serial.print(y, 3); Serial.print(',');
  Serial.print(z, 3); Serial.print(',');
  Serial.print(gyro.gyro.x, 3); Serial.print(',');
  Serial.print(gyro.gyro.y, 3); Serial.print(',');
  Serial.print(gyro.gyro.z, 3); Serial.print(',');
  Serial.println(norm, 3);
}
