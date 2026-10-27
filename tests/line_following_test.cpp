#include "host_runtime.h"
#include "../firmware/line_following_pd/line_following_pd.ino"
#include <cmath>

int main() {
  try {
    testAdsReady = false; setup(); loop();
    expect(pwm[9] == 0 && pwm[10] == 0, "ADC init failure stays stopped");
    testAdsReady = true; setup();
    testAdc[0] = 5999; testAdc[1] = 6000; testAdc[2] = 5999;
    loop(); expect(irSensors == 0b010 && pwm[9] == 100 && pwm[10] == 100, "threshold boundary centered");
    testAdc[0] = 6000; testAdc[1] = 0; testAdc[2] = 0; loop();
    expect(irSensors == 0b100 && pwm[9] == 100 && pwm[10] == 25, "left sensor correction and clamp");
    testAdc[0] = 0; testAdc[1] = 0; testAdc[2] = 6000; loop();
    expect(irSensors == 0b001 && pwm[9] == 25 && pwm[10] == 100, "right correction and clamp");
    testAdc[2] = 0; loop();
    expect(reverseMode && pwm[9] == 35 && levels[6] == LOW, "lost line reverses");
    testMillis += 20; loop(); expect(lockSpeed && !reverseMode, "reverse duration 20 ms");
    loop(); expect(pwm[9] == 0 && pwm[10] == 60, "fixed search direction");
    testAdc[1] = 6000; testAdc[2] = 6000; loop();
    expect(tempSpeedMode && !lockSpeed, "011 starts transition");
    loop(); expect(pwm[9] == 60 && pwm[10] == 30, "transition speeds");
    testMillis += 130; loop(); expect(!tempSpeedMode, "transition completes");
    irSensors = 0b111; getError(); expect(std::abs(error + 2.9f) < 0.001f, "all-black legacy rule");
    irSensors = 0b101; getError(); expect(std::abs(error + 2.9f) < 0.001f, "101 keeps previous error");
    std::cout << "PASS: line thresholds and lost-line recovery (" << assertions << " assertions)\n";
  } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
