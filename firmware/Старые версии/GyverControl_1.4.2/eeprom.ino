void updateEEPROM(byte channel) {
  if (EEPROM_updFlag) {
    EEPROM.put(channel * 30, channels[channel]);
    if (channels[channel].mode == 4 || channels[channel].mode == 5) {
      byte curPWMchannel;
      switch (channel) {
        case 2: curPWMchannel = 0;  // канал 2
          break;
        case 3: curPWMchannel = 1;  // канал 3
          break;
        case 7: curPWMchannel = 2;  // серво 1
          break;
        case 8: curPWMchannel = 3;  // серво 2
          break;
        case 9: curPWMchannel = 4;  // привод
          break;
      }
      if (channels[channel].mode == 4) EEPROM.put(curPWMchannel * 25 + 400, PID[curPWMchannel]);
      if (channels[channel].mode == 5) EEPROM.put(curPWMchannel * 7 + 500, dawn[curPWMchannel]);
    }
    updByte(300, minAngle[0]);
    updByte(301, minAngle[1]);
    updByte(302, maxAngle[0]);
    updByte(303, maxAngle[1]);
    updByte(304, driveTimeout);
    EEPROM_updFlag = false;
  }
}

void updateSettings() {
  if (settingsChanged) {
    EEPROM.put(900, settings);
    settingsChanged = false;
  }
}

void updByte(int addr, byte val) {
  byte buf = EEPROM.read(addr);
  if (val != buf) EEPROM.write(addr, val);
}
