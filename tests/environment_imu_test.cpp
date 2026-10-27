#include "host_runtime.h"
#include "../firmware/environment_imu/environment_imu.ino"
#include <limits>

int main() {
  try {
    testMpuReady = false; setup(); loop();
    expect(pwm[9] == 0 && pwm[10] == 0, "sensor init failure stops");
    testMpuReady = true; setup();
    testAz = -9.8f; loop(); expect(!isRunning, "initial deadband stays stopped");
    testAz = -8.0f; loop(); expect(!isRunning, "start threshold strict");
    testAz = -7.99f; loop(); expect(isRunning && pwm[9] == 255, "above -8 starts");
    testAz = -15.0f; loop(); expect(isRunning, "stop threshold strict");
    testAz = -15.01f; loop(); expect(!isRunning && pwm[10] == 0, "below -15 stops");
    testAz = -9.0f; loop(); expect(!isRunning, "deadband keeps stopped");
    testAz = -7.0f; loop(); testMpuReadReady = false; loop();
    expect(!isRunning && pwm[9] == 0, "read failure clears running state");
    testMpuReadReady = true; testAz = std::numeric_limits<float>::quiet_NaN(); loop();
    expect(pwm[9] == 0, "invalid z stops");
    testAz = -9.8f; testMillis = 1200; clearSerial(); loop();
    expect(Serial.output.str().find("pressure_hPa=1013.25") != std::string::npos, "pressure converted to hPa");
    expect(Serial.output.str().find("gyro_rad_s") != std::string::npos, "gyro units named");
    std::cout << "PASS: inertial threshold state retention and failures (" << assertions << " assertions)\n";
  } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
