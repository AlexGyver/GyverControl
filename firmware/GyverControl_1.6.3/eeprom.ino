void clearEEPROM() {
  for (int i = 0; i < 1023; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.put(EEPR_KEY_ADDR, EEPR_KEY);
  for (byte i = 0; i < 10; i++) {
    /*activeChannel.type = ((i == 9) ||
                          (i == 7 && !SERVO1_RELAY) ||
                          (i == 8 && !SERVO2_RELAY)) ? true : false;*/
    EEPROM.put(i * EEPR_CH_STEP, activeChannel);
  }

  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
#if (USE_PID == 1)
    EEPROM.put(i * EEPR_PID_STEP + EEPR_PID, activePID);
#endif
#if (USE_DAWN == 1)
    EEPROM.put(i * EEPR_DAWN_STEP + EEPR_DAWN, activeDawn);
#endif
  }

  EEPROM.put(EEPR_SETTINGS, settings);
  EEPROM.put(EEPR_PLOT_D, sensMinute);
  EEPROM.put(EEPR_PLOT_H, sensMinute);

#if (SCHEDULE_NUM > 0)
  for (byte i = 0; i < 2; i++) {     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    EEPROM.put(i * EEPR_SHED_STEP + EEPR_SHED, activeSchedule);
  }
#endif
}
