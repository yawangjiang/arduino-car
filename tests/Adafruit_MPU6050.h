#pragma once
#include "Adafruit_Sensor.h"
#include <cstdint>
extern bool testMpuReady, testMpuReadReady;
extern float testAx, testAy, testAz, testGx, testGy, testGz;
constexpr int MPU6050_RANGE_2_G = 0;
constexpr int MPU6050_RANGE_250_DEG = 0;
class Adafruit_MPU6050 {
 public:
  bool begin(uint8_t = 0x68) { return testMpuReady; }
  void setAccelerometerRange(int) {}
  void setGyroRange(int) {}
  bool getEvent(sensors_event_t* accel, sensors_event_t* gyro, sensors_event_t* temp) {
    accel->acceleration.x = testAx; accel->acceleration.y = testAy; accel->acceleration.z = testAz;
    gyro->gyro.x = testGx; gyro->gyro.y = testGy; gyro->gyro.z = testGz;
    temp->temperature = 20;
    return testMpuReadReady;
  }
};
