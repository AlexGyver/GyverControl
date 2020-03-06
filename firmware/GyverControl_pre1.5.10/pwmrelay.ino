#if (USE_PID_RELAY == 1)
void tickPWM(byte ch, bool &flag, uint32_t &tmr) {
  if (channels[ch].mode == 4 && !serviceFlag) {
    if (millis() - tmr >= (flag ? PWMactive[ch] : (PWMperiod - PWMactive[ch]))) {
      tmr = millis();
      flag = !flag;
      digitalWrite(relayPins[ch], flag ^ channels[ch].direction);
    }
  }
}
#endif
