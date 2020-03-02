#if (USE_PID_RELAY == 1)
void tickPWM_0() {
  if (channels[0].mode == 4 && !serviceFlag) {
    static uint32_t tmr;
    static bool flag;
    if (millis() - tmr >= (flag ? PWMactive[0] : (PWMperiod - PWMactive[0]))) {
      tmr = millis();
      flag = !flag;
      digitalWrite(relayPins[0], flag ^ channels[0].direction);
    }
  }
}

void tickPWM_1() {
  if (channels[1].mode == 4 && !serviceFlag) {
    static uint32_t tmr;
    static bool flag;
    if (millis() - tmr >= (flag ? PWMactive[1] : (PWMperiod - PWMactive[1]))) {
      tmr = millis();
      flag = !flag;
      digitalWrite(relayPins[1], flag ^ channels[1].direction);
    }
  }
}
#endif
