#pragma once
struct sensors_event_t {
  struct { float x = 0, y = 0, z = 0; } acceleration;
  struct { float x = 0, y = 0, z = 0; } gyro;
  float temperature = 0;
};
