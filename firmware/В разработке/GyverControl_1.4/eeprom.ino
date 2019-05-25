void updateEEPROM(byte channel) {
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
  /*channelsStruct chBuffer;
    EEPROM.get(channel * 30, chBuffer);
    byte counter = 0;
    if (chBuffer.state != channels[channel].state) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].state);
    counter++;
    if (chBuffer.direction != channels[channel].direction) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].direction);
    counter++;
    if (chBuffer.hour1 != channels[channel].hour1) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].hour1);
    counter++;
    if (chBuffer.hour2 != channels[channel].hour2) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].hour2);
    counter++;
    if (chBuffer.sensor != channels[channel].sensor) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].sensor);
    counter++;
    if (chBuffer.relayType != channels[channel].relayType) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].relayType);
    counter++;
    if (chBuffer.mode != channels[channel].mode) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].mode);
    counter++;
    if (chBuffer.startHour != channels[channel].startHour) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].startHour);
    counter++;
    if (chBuffer.impulsePrd != channels[channel].impulsePrd) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].impulsePrd);
    counter++;
    if (chBuffer.threshold != channels[channel].threshold) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].threshold);
    counter++;
    if (chBuffer.sensPeriod != channels[channel].sensPeriod) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].sensPeriod);
    counter++;
    if (chBuffer.period != channels[channel].period) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].period);
    counter++;
    if (chBuffer.work != channels[channel].work) EEPROM.put(channel * 30 + eeprAdr[counter], channels[channel].work);
  */
  updByte(300, minAngle[0]);
  updByte(301, minAngle[1]);
  updByte(302, maxAngle[0]);
  updByte(303, maxAngle[1]);
  updByte(304, driveTimeout);
}

void updateSettings() {
  EEPROM.put(310, settings);
  settingsChanged = false;
}

void updByte(int addr, byte val) {
  byte buf = EEPROM.get(addr, buf);
  if (val != buf) EEPROM.put(addr, val);
}
