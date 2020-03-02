void updateEEPROM(byte channel) {
  if (EEPROM_updFlag) {
    EEPROM.put(channel * 40, channels[channel]);
    if (channels[channel].mode == 4 || channels[channel].mode == 5) {
      byte curPWMchannel = channelToPWM[channel];
      if (channels[channel].mode == 4) EEPROM.put(curPWMchannel * 20 + EEPR_PID, PID[curPWMchannel]);
      if (channels[channel].mode == 5 && curPWMchannel != 6) EEPROM.put(curPWMchannel * 6 + EEPR_DAWN, dawn[curPWMchannel]);
    }
    EEPROM.put(EEPR_SETTINGS, settings);
    EEPROM_updFlag = false;
  }
}

void updateSettings() {
  if (settingsChanged) {
    EEPROM.put(EEPR_SETTINGS, settings);
    settingsChanged = false;
  }
}
