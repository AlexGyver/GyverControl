void updateEEPROM(byte channel) {
  if (EEPROM_updFlag) {
    EEPROM.put(channel * 40, channels[channel]);
    if (channels[channel].mode == 4 || channels[channel].mode == 5) {
      byte curPWMchannel = channelToPWM(channel);
      if (channels[channel].mode == 4) EEPROM.put(curPWMchannel * 18 + 500, PID[curPWMchannel]);
      if (channels[channel].mode == 5 && curPWMchannel != 4) EEPROM.put(curPWMchannel * 6 + 600, dawn[curPWMchannel]);
    }
    EEPROM.update(400, minAngle[0]);
    EEPROM.update(401, minAngle[1]);
    EEPROM.update(402, maxAngle[0]);
    EEPROM.update(403, maxAngle[1]);
    EEPROM.update(404, driveTimeout);
    EEPROM_updFlag = false;
  }
}

void updateSettings() {
  if (settingsChanged) {
    EEPROM.put(900, settings);
    settingsChanged = false;
  }
}
