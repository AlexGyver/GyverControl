void updateEEPROM(byte channel) {
  if (EEPROM_updFlag) {
    EEPROM.put(channel * 30, channels[channel]);
    if (channels[channel].mode == 4 || channels[channel].mode == 5) {
      byte curPWMchannel = channelToPWM(channel);
      if (channels[channel].mode == 4) EEPROM.put(curPWMchannel * 25 + 400, PID[curPWMchannel]);
      if (channels[channel].mode == 5) EEPROM.put(curPWMchannel * 7 + 500, dawn[curPWMchannel]);
    }
    EEPROM.update(300, minAngle[0]);
    EEPROM.update(301, minAngle[1]);
    EEPROM.update(302, maxAngle[0]);
    EEPROM.update(303, maxAngle[1]);
    EEPROM.update(304, driveTimeout);
    EEPROM_updFlag = false;
  }
}

void updateSettings() {
  if (settingsChanged) {
    EEPROM.put(900, settings);
    settingsChanged = false;
  }
}
