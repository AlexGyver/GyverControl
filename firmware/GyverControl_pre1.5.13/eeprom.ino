void updateEEPROM(byte channel) {
  if (EEPROM_updFlag) {
    EEPROM.put(channel * 32, channels[channel]);
    if (channels[channel].mode == 4 || channels[channel].mode == 5) {

#if (USE_PID == 1)
      if (channels[channel].mode == 4) EEPROM.put(channelToPWM[channel] * 20 + EEPR_PID, PID[channelToPWM[channel]]);
#endif

#if (USE_DAWN == 1)
      if (channels[channel].mode == 5 && channelToPWM[channel] != 6) EEPROM.put(channelToPWM[channel] * 6 + EEPR_DAWN, dawn[channelToPWM[channel]]);
#endif

    }

#if (SCHEDULE_NUM > 0)
    for (byte i = 0; i < SCHEDULE_NUM; i++) {
      EEPROM.put((SCHEDULE_MAX * 2 + 6) * i + EEPR_SHED, schedule[i]);
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
    EEPROM.put(i * 32, channels[i]);
  }
  EEPROM.put(EEPR_SETTINGS, settings);

#if (USE_PID == 1)
  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
    EEPROM.put(i * 20 + EEPR_PID, PID[i]);
#if (USE_DAWN == 1)
    EEPROM.put(i * 6 + EEPR_DAWN, dawn[i]);
#endif
  }
#endif

#if (SCHEDULE_NUM > 0)
  for (byte i = 0; i < 5; i++) {     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    EEPROM.put((SCHEDULE_MAX * 2 + 6) * i + EEPR_SHED, schedule[i]);
  }
#endif
}

void readEEPROM() {
  for (byte i = 0; i < 10; i++) {
    EEPROM.get(i * 32, channels[i]);
  }
  EEPROM.get(EEPR_SETTINGS, settings);
  
#if (USE_PID == 1)
  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
    EEPROM.get(i * 20 + EEPR_PID, PID[i]);
#if (USE_DAWN == 1)
    EEPROM.get(i * 6 + EEPR_DAWN, dawn[i]);
#endif
  }
#endif

#if (SCHEDULE_NUM > 0)
  for (byte i = 0; i < 5; i++) {     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    EEPROM.get((SCHEDULE_MAX * 2 + 6) * i + EEPR_SHED, schedule[i]);
  }
#endif
}
