#include "host_runtime.h"
#include "../firmware/environment/bme280_monitor/bme280_monitor.ino"
#include <limits>

int main() {
  try {
    setup();
    expect(Serial.output.str().find("humidity_pct,pressure_hPa") != std::string::npos, "CSV units");
    clearSerial(); testMillis = 1199; loop(); expect(Serial.output.str().empty(), "1200 ms interval");
    testMillis = 1200; loop();
    expect(Serial.output.str().find("1013.25") != std::string::npos, "Pa converted to hPa");
    testHumidity = std::numeric_limits<float>::quiet_NaN();
    clearSerial(); testMillis += 1200; loop();
    expect(Serial.output.str().find("invalid sample") != std::string::npos, "invalid humidity rejected");
    testBmeReady = false; setup(); clearSerial(); testMillis += 1200; loop();
    expect(Serial.output.str().empty(), "init failure prevents readings");
    std::cout << "PASS: BME280 standalone logging (" << assertions << " assertions)\n";
  } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
