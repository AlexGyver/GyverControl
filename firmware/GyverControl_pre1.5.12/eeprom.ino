void updateEEPROM(byte channel) {
  if (EEPROM_updFlag) {
    EEPROM.put(channel * 40, channels[channel]);
    if (channels[channel].mode == 4 || channels[channel].mode == 5) {
      byte curPWMchannel = channelToPWM[channel];
#if (USE_PID == 1)
      if (channels[channel].mode == 4) EEPROM.put(curPWMchannel * 20 + EEPR_PID, PID[curPWMchannel]);
#endif
#if (USE_DAWN == 1)
      if (channels[channel].mode == 5 && curPWMchannel != 6) EEPROM.put(curPWMchannel * 6 + EEPR_DAWN, dawn[curPWMchannel]);
#endif
    }
#if (SCHEDULE_NUM > 0)
    for (byte i = 0; i < SCHEDULE_NUM; i++) {
      EEPROM.put((SCHEDULE_MAX * 2 + 10) * i + EEPR_SHED, schedule[i]);
    }
#endif
    updateSettings();
    EEPROM_updFlag = false;
  }
}

void updateSettings() {
  EEPROM.put(EEPR_SETTINGS, settings);
}

void clearEEPROM() {
  EEPROM.write(EEPR_KEY_ADDR, EEPR_KEY);
  for (byte i = 0; i < 10; i++) {
    EEPROM.put(i * 40, channels[i]);
  }
  EEPROM.put(EEPR_SETTINGS, settings);

  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
#if (USE_PID == 1)
    EEPROM.put(i * 20 + EEPR_PID, PID[i]);
#endif
#if (USE_DAWN == 1)
    if (i < 4) EEPROM.put(i * 6 + EEPR_DAWN, dawn[i]);
#endif
#if (SCHEDULE_NUM > 0)
    for (byte i = 0; i < 5; i++) {     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      EEPROM.put((SCHEDULE_MAX * 2 + 10) * i + EEPR_SHED, schedule[i]);
    }
#endif
  }
}

void readEEPROM() {
  for (byte i = 0; i < 10; i++) {
    EEPROM.get(i * 40, channels[i]);
  }
  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
#if (USE_PID == 1)
    EEPROM.get(i * 20 + EEPR_PID, PID[i]);
#endif
#if (USE_DAWN == 1)
    if (i < 4) EEPROM.get(i * 6 + EEPR_DAWN, dawn[i]);
#endif
  }
  EEPROM.get(EEPR_SETTINGS, settings);
#if (SCHEDULE_NUM > 0)
  for (byte i = 0; i < SCHEDULE_NUM; i++) {
    EEPROM.get((SCHEDULE_MAX * 2 + 10) * i + EEPR_SHED, schedule[i]);
  }
#endif
}
