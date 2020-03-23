void updateEEPROM(byte channel) {
  if (EEPROM_updFlag) {
    EEPROM.put(channel * EEPR_CH_STEP, channels[channel]);

#if (USE_PID == 1)
    if (channels[channel].mode == 4)
      EEPROM.put(channelToPWM[channel] * EEPR_PID_STEP + EEPR_PID, PID[channelToPWM[channel]]);
#endif

#if (USE_DAWN == 1)
    if (channels[channel].mode == 5 && channelToPWM[channel] != 6)
      EEPROM.put(channelToPWM[channel] * EEPR_DAWN_STEP + EEPR_DAWN, dawn[channelToPWM[channel]]);
#endif

#if (SCHEDULE_NUM > 0)
    for (byte i = 0; i < SCHEDULE_NUM; i++) {
      EEPROM.put(EEPR_SHED_STEP * i + EEPR_SHED, schedule[i]);
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
  EEPROM.put(EEPR_KEY_ADDR, EEPR_KEY);
  for (byte i = 0; i < 10; i++) {
    EEPROM.put(i * EEPR_CH_STEP, channels[i]);
  }

  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
#if (USE_PID == 1)
    EEPROM.put(i * EEPR_PID_STEP + EEPR_PID, PID[i]);
#endif
#if (USE_DAWN == 1)
    EEPROM.put(i * EEPR_DAWN_STEP + EEPR_DAWN, dawn[i]);
#endif
  }

  EEPROM.put(EEPR_SETTINGS, settings);

#if (SCHEDULE_NUM > 0)
  for (byte i = 0; i < 5; i++) {     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    EEPROM.put(EEPR_SHED_STEP * i + EEPR_SHED, schedule[i]);
  }
#endif
}

void readEEPROM() {
  for (byte i = 0; i < 10; i++) {
    EEPROM.get(i * EEPR_CH_STEP, channels[i]);
  }
  EEPROM.get(EEPR_SETTINGS, settings);

  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
#if (USE_PID == 1)
    EEPROM.get(i * EEPR_PID_STEP + EEPR_PID, PID[i]);
#endif
#if (USE_DAWN == 1)
    EEPROM.get(i * EEPR_DAWN_STEP + EEPR_DAWN, dawn[i]);
#endif
  }


#if (SCHEDULE_NUM > 0)
  for (byte i = 0; i < 5; i++) {     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    EEPROM.get(EEPR_SHED_STEP * i + EEPR_SHED, schedule[i]);
  }
#endif
}
