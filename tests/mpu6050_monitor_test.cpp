#include "host_runtime.h"
#include "../firmware/inertial/mpu6050_monitor/mpu6050_monitor.ino"
#include <limits>

int main() {
  try {
    setup(); expect(Serial.output.str().find("gx_rad_s") != std::string::npos, "gyro column units");
    clearSerial(); testMillis = 99; loop(); expect(Serial.output.str().empty(), "100 ms interval");
    testAx = 3; testAy = 4; testAz = 0; testMillis = 100; loop();
    expect(Serial.output.str().find("100,3,4,0,0,0,0,5") != std::string::npos, "raw axes and norm");
    testMpuReadReady = false; testMillis += 100; clearSerial(); loop();
    expect(Serial.output.str().find("read failed") != std::string::npos, "read failure reported");
    testMpuReadReady = true; testGz = std::numeric_limits<float>::quiet_NaN();
    testMillis += 100; clearSerial(); loop();
    expect(Serial.output.str().find("invalid sample") != std::string::npos, "nonfinite gyro rejected");
    testMpuReady = false; setup(); testMillis += 100; clearSerial(); loop();
    expect(Serial.output.str().empty(), "init failure prevents readings");
    std::cout << "PASS: MPU6050 standalone logging (" << assertions << " assertions)\n";
  } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
