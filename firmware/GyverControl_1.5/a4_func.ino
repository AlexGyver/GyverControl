// channel to pwm
byte channelToPWM(byte ch) {
  switch (ch) {
    case 2: return 0;  // канал 2
    case 3: return 1;  // канал 3
    case 7: return 2;  // серво 1
    case 8: return 3;  // серво 2
    case 9: return 4;  // привод
  }
}

// -------------------- SERVICE ---------------------
void serviceTick() {
  if (millis() - commonTimer > 1000) {
    commonTimer = millis();
    // управляем реле
    for (byte i = 0; i < 7; i++) {
      if (channels[i].state) digitalWrite(relayPins[i], channelStatesServ[i]);
    }

    if (SERVO1_RELAY) {
      digitalWrite(SERVO_0, channelStatesServ[7]);
    }
    if (SERVO2_RELAY) {
      digitalWrite(SERVO_1, channelStatesServ[8]);
    }

    // управляем серво
#if (SERVO1_RELAY == 0)
    servo1.setTargetDeg(servoPosServ[0]);

#endif
#if (SERVO2_RELAY == 0)
    servo2.setTargetDeg(servoPosServ[1]);
#endif
  }
}

void serviceIN() {    // выполняем при входе в сервис
#if (SERVO1_RELAY == 0)
  servo1.start();
#endif
#if (SERVO2_RELAY == 0)
  servo2.start();
#endif

  servoPosServ[0] = servoPos[0];
  servoPosServ[1] = servoPos[1];

  serviceFlag = true;
  for (byte i = 0; i < 10; i++) channelStatesServ[i] = channelStates[i];
  realTime[0] = rtc.getHours();
  realTime[1] = rtc.getMinutes();
  realTime[2] = rtc.getSeconds();
}

void serviceOUT() {   // выполняем при выходе с сервиса
  serviceFlag = false;
  startFlagDawn = true;
  for (byte i = 0; i < 7; i++) {
    if (channels[i].state) digitalWrite(relayPins[i], channelStates[i]); // вернуть реле на места
  }
  if (SERVO1_RELAY) {
    digitalWrite(SERVO_0, channelStates[7]);
  }
  if (SERVO2_RELAY) {
    digitalWrite(SERVO_1, channelStates[8]);
  }

  // шим - рассвет
  if (channels[2].mode == 5) {
    analogWrite(RELAY_2, pwmVal[0]);
  }
  if (channels[3].mode == 5) {
    analogWrite(RELAY_3, pwmVal[1]);
  }

  // вернуть серво на места
#if (SERVO1_RELAY == 0)
  if (!channels[7].state) servo1.stop();
  servo1.setTargetDeg(servoPos[0]);
#endif
#if (SERVO2_RELAY == 0)
  if (!channels[8].state) servo2.stop();
  servo2.setTargetDeg(servoPos[1]);
#endif
  // вернуть привод
  driveState = 1;

  if (timeChanged) {
    timeChanged = false;
    rtc.setTime(realTime[2], realTime[1], realTime[0], rtc.getDate(), rtc.getMonth(), rtc.getYear());
  }

  updateSettings();
}
// -------------------- SERVICE ---------------------

void backlTick() {
  if (settings.backlight && backlState && millis() - backlTimer >= (long)settings.backlTime * 1000) {
    backlState = false;
    lcd.noBacklight();
  }
}

void debTick() {
  if ( (currentChannel >= 0 || currentChannel == -2) && millis() - settingsTimer > SETT_TIMEOUT * 1000L) {   // если не дебаг и сработал таймер
    settingsTimer = millis();
    EEPROM_updFlag = true;
    if (currentChannel == -2) updateSettings();
    if (currentChannel >= 0) updateEEPROM(currentChannel);
    currentChannel = -1;
    navDepth = 0;
    arrowPos = 0;
    debugPage = 0;
    currentLine = 4;
    redrawScreen();
  }
}

void backlOn() {
  backlState = true;
  backlTimer = millis();
  lcd.backlight();
}
