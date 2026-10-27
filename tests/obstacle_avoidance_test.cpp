#include "host_runtime.h"
#include "../firmware/obstacle_avoidance/obstacle_avoidance.ino"

int main() {
  try {
    setup();
    expect(state == WAIT_FOR_ECHO && pwm[5] == 0 && pwm[6] == 0, "startup waits stopped");
    testMillis += 300; testPulse = 0; loop();
    expect(state == WAIT_FOR_ECHO && pwm[5] == 0, "no echo stays stopped");
    testPulse = 5800; testMillis += 40; loop();
    expect(state == FORWARD && pwm[5] == 123 && pwm[6] == 117, "valid clear echo resumes");

    bool reversed = false;
    for (int i = 0; i < 120; ++i) {
      testMillis += 7; loop();
      expect(sweepAngle >= sweepRight && sweepAngle <= sweepLeft, "scan within limits");
      if (sweepDir < 0) reversed = true;
    }
    expect(reversed, "signed sweep actually reverses");

    testPulse = 40 * 58; testMillis += 40; loop();
    expect(state == STOPPING && pwm[5] == 0 && pwm[6] == 0, "40 cm stops without advance override");
    testMillis += 20; loop();
    expect(state == LOOK_LEFT && myservo.angle == wideLeft, "left look");
    testPulse = 80 * 58; testMillis += 300; loop();
    expect(state == LOOK_RIGHT && myservo.angle == wideRight, "right look");
    testPulse = 30 * 58; testMillis += 300; loop();
    expect(state == BACK_LEFT && pwm[5] == 100 && pwm[6] == 160, "back arc chooses larger left space");
    testMillis += 719; loop(); expect(state == BACK_LEFT, "back duration not shortened");
    testMillis += 1; loop(); expect(state == TURN_RIGHT, "random branch retained");
    testMillis += 175; loop(); expect(state == RECOVER, "turn completes");
    testMillis += 55; loop(); expect(state == WAIT_FOR_ECHO && pwm[5] == 0, "recover waits for echo");

    enterState(FORWARD); testPulse = 0; testMillis += 40; loop();
    expect(state == WAIT_FOR_ECHO && pwm[6] == 0, "lost echo while moving stops");
    enterState(LOOK_LEFT); testMillis += 300; loop();
    expect(state == WAIT_FOR_ECHO && pwm[5] == 0, "invalid side reading stops");
    Fdist = 12; enterState(BACK_RIGHT); testMillis += 720; loop();
    expect(state == BACK_RIGHT, "very close uses longer back time");
    testMillis += 130; testRandom = 1; loop(); expect(state == TURN_LEFT, "850 ms then left random branch");

    testPulse = 57; expect(readDistance() == 0, "sub-centimeter result invalid");
    testPulse = 58; expect(readDistance() == 1, "one centimeter valid");
    testPulse = 9000; expect(readDistance() == 0, "timeout invalid");
    std::cout << "PASS: obstacle state machine, scan and failure handling (" << assertions << " assertions)\n";
  } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
