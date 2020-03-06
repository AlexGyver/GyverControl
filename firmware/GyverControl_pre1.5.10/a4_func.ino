// channel to pwm
const byte channelToPWM[] = {0, 1, 2, 3, 0, 0, 4, 5, 6};

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
#if (SMOOTH_SERVO == 1)
    servo1.setTargetDeg(servoPosServ[0]);
#else
    servo1.write(servoPosServ[0]);
#endif
#endif

#if (SERVO2_RELAY == 0)
#if (SMOOTH_SERVO == 1)
    servo2.setTargetDeg(servoPosServ[1]);
#else
    servo2.write(servoPosServ[1]);
#endif
#endif
  }
}

void serviceIN() {    // выполняем при входе в сервис
#if (SERVO1_RELAY == 0)
#if (SMOOTH_SERVO == 1)
  servo1.start();
#else
  servo1.attach(SERVO_0, 600, 2400);
#endif

#endif
#if (SERVO2_RELAY == 0)
#if (SMOOTH_SERVO == 1)
  servo2.start();
#else
  servo2.attach(SERVO_1, 600, 2400);
#endif

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
    analogWrite(RELAY_2, pwmVal[2]);
  }
  if (channels[3].mode == 5) {
    analogWrite(RELAY_3, pwmVal[3]);
  }

  // вернуть серво на места
#if (SERVO1_RELAY == 0)
#if (SMOOTH_SERVO == 1)
  if (!channels[7].state) servo1.stop();
  servo1.setTargetDeg(servoPos[0]);
#else
  servo1.write(servoPos[0]);
  if (!channels[7].state) servo1.detach();
#endif

#endif
#if (SERVO2_RELAY == 0)
#if (SMOOTH_SERVO == 1)
  if (!channels[8].state) servo2.stop();
  servo2.setTargetDeg(servoPos[1]);
#else
  servo1.write(servoPos[1]);
  if (!channels[8].state) servo2.detach();
#endif
#endif

  // вернуть привод
  driveState = 1;

  if (timeChanged) {
    timeChanged = false;
    rtc.setTime(realTime[2], realTime[1], realTime[0], rtc.getDate(), rtc.getMonth(), rtc.getYear());
  }
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
    updateEEPROM(currentChannel);
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
