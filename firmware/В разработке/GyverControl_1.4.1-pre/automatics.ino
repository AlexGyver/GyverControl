void timersTick() {   // каждую секунду
  // получаем время
  now = rtc.now();
  realTime[0] = now.hour();
  realTime[1] = now.minute();
  realTime[2] = now.second();
  if (realTime[0] == 0 && realTime[1] == 0 && realTime[0] == 0) getDay();   // получить номер дня
  uptime += (float)0.0000115741;   // аптайм у нас в сутках! 1/(24*60*60)

  for (byte curChannel = 0; curChannel < 10; curChannel++) {
    if (channels[curChannel].state       // если канал активен (on/off)
        && (curChannel >= 7 || (curChannel < 7 && channels[curChannel].relayType != 2) ) ) {   // для всех кроме ОБЩИХ реле
      switch (channels[curChannel].mode) {
        case 0:   // ---------------------- если таймер ----------------------
          if (millis() - timerMillis[curChannel] >= channels[curChannel].period * 1000
              && channelStates[curChannel] != channels[curChannel].direction) {
            timerMillis[curChannel] = millis();
            channelStates[curChannel] = channels[curChannel].direction;
          }
          if (millis() - timerMillis[curChannel] >= channels[curChannel].work * 1000
              && channelStates[curChannel] == channels[curChannel].direction) {
            channelStates[curChannel] = !channels[curChannel].direction;
          }
          break;
        case 1:   // ---------------------- если таймер RTC ----------------------
          if (realTime[2] == 0 || realTime[2] == 1) {                               // проверка каждую минуту (в первые две секунды, на всякий случай)!
            if (channels[curChannel].impulsePrd < 6) {                              // если минутные периоды
              if (channelStates[curChannel] != channels[curChannel].direction) {    // если канал ВЫКЛЮЧЕН
                byte waterTime = 0;                                                 // начало проверки времени с 0 минуты
                for (byte j = 0; j < 60 / impulsePrds[channels[curChannel].impulsePrd]; j++) {
                  if (realTime[1] == waterTime) {
                    channelStates[curChannel] = channels[curChannel].direction;     // ВКЛЮЧАЕМ
                    timerMillis[curChannel] = millis();                             // взводим таймер
                  }
                  waterTime += impulsePrds[channels[curChannel].impulsePrd];
                }
              }
            } else if (channels[curChannel].impulsePrd < 13) {                        // если часовые периоды
              if (realTime[1] == 0 && (realTime[2] == 0 || realTime[2] == 1)) {       // проверка в первую минуту часа!
                if (channelStates[curChannel] != channels[curChannel].direction) {    // если ВЫКЛЮЧЕН
                  byte waterTime = channels[curChannel].startHour;                    // начало проверки времени со стартового часа
                  for (byte j = 0; j < 24 / impulsePrds[channels[curChannel].impulsePrd]; j++) {
                    if (waterTime < 24) {
                      if (realTime[0] == waterTime) {
                        channelStates[curChannel] = channels[curChannel].direction;   // ВКЛЮЧАЕМ
                        timerMillis[curChannel] = millis();                           // взводим таймер
                      }
                    } else {
                      if (realTime[0] == waterTime - 24) {
                        channelStates[curChannel] = channels[curChannel].direction;   // ВКЛЮЧАЕМ
                        timerMillis[curChannel] = millis();                           // взводим таймер
                      }
                    }
                    waterTime += impulsePrds[channels[curChannel].impulsePrd];
                  }
                }
              }
            } else {        // суточные периоды
              if (realTime[1] == 0 && (realTime[2] == 0 || realTime[2] == 1)) {       // проверка в первую минуту часа!
                if (channelStates[curChannel] != channels[curChannel].direction) {    // если ВЫКЛЮЧЕН
                  if (thisDay % impulsePrds[channels[curChannel].impulsePrd] == 0  // если попали в день
                      && realTime[0] == channels[curChannel].startHour) {             // и час
                    channelStates[curChannel] = channels[curChannel].direction;   // ВКЛЮЧАЕМ
                    timerMillis[curChannel] = millis();                           // взводим таймер
                  }
                }
              }
            }
          }

          if (channelStates[curChannel] == channels[curChannel].direction                     // если ВКЛЮЧЕН
              && millis() - timerMillis[curChannel] >= channels[curChannel].work * 1000) {    // И отработал время РАБОТА
            channelStates[curChannel] = !channels[curChannel].direction;                      // ВЫКЛЮЧАЕМ
          }
          break;
        case 2:   // ---------------------- если сутки ----------------------
          if (checkDay(curChannel)) channelStates[curChannel] = channels[curChannel].direction;     // включились до stop hour
          else channelStates[curChannel] = !channels[curChannel].direction;    // выключились до следующего start hour
          /*
            if ((realTime[0] >= channels[curChannel].hour1 && realTime[0] < channels[curChannel].hour2) ) {

              if (channels[curChannel].global) {                             // если работаем в паре с сенсором
                if (checkHysteresis(curChannel) == 1) channelStates[curChannel] = channels[curChannel].direction;
                else if (checkHysteresis(curChannel) == 2) channelStates[curChannel] = !channels[curChannel].direction;
              } else {                                                          // работаем чисто по таймеру
                channelStates[curChannel] = channels[curChannel].direction;     // включились до stop hour
              }
            } else if ((realTime[0] < channels[curChannel].hour1 || realTime[0] >= channels[curChannel].hour2)) {

              channelStates[curChannel] = !channels[curChannel].direction;    // выключились до следующего start hour
            }
          */
          break;
        case 3:   // ---------------------- если датчик ----------------------
          if (millis() - timerMillis[curChannel] >= channels[curChannel].sensPeriod * 1000L) {
            timerMillis[curChannel] = millis();

            if (checkHysteresis(curChannel) == 1) channelStates[curChannel] = channels[curChannel].direction;
            else if (checkHysteresis(curChannel) == 2) channelStates[curChannel] = !channels[curChannel].direction;
          }
          break;
        case 4:   // ---------------------- если ПИД ----------------------
          break;
        case 5:   // -------------------- если рассвет --------------------
#if (USE_DAWN == 1)
          if (realTime[2] == 0 || realTime[2] == 1 || startFlagDawn) {    // проверка каждую минуту (в первые две секунды, на всякий случай)!
            checkDawn(curChannel);
          }
#endif
          break;
      }
      // если Day глобальный, выключить канал в нерабочее время!
      if (channels[curChannel].global && !checkDay(curChannel)) channelStates[curChannel] = !channels[curChannel].direction;
    }
  }
  startFlagDawn = false;
  // --- применяем ---

  // реле
  boolean atLeastOneValve = false;                              // флаг "хотя бы одного" клапана
  for (byte relay = 0; relay < 7; relay++) {
    if (channels[relay].mode != 4 && channels[relay].mode != 5) {
      if (channels[relay].relayType == 1                          // если реле - КЛАПАН
          && channelStates[relay] == channels[relay].direction)   // и он ВКЛЮЧЕН
        atLeastOneValve = true;                                   // запоминаем, что нужно включить общий канал
      digitalWrite(relayPins[relay], channelStates[relay]);       // включаем/выключаем все реле типов РЕЛЕ и КЛАПАН
    }
  }

  for (byte relay = 0; relay < 7; relay++) {
    if (channels[relay].relayType == 2
        && channels[relay].mode != 4 && channels[relay].mode != 5) {    // только для ОБЩИХ каналов реле, кроме ПИД и рассвета
      if (atLeastOneValve) channelStates[relay] = channels[relay].direction;  // включить общий
      else channelStates[relay] = !channels[relay].direction;                 // выключить общий
      digitalWrite(relayPins[relay], channelStates[relay]);                   // применить
    }
  }

  // шим - рассвет или пид
  if (channels[2].mode >= 4) {
    analogWrite(RELAY_2, pwmVal[0]);
  }
  if (channels[3].mode >= 4) {
    analogWrite(RELAY_3, pwmVal[1]);
  }

  // серво
  if (SERVO1_RELAY) {   // если реле
    digitalWrite(SERVO_0, channelStates[7]);
  } else {
#if (SERVO1_RELAY == 0)
    if (channels[7].mode != 4 && channels[7].mode != 5) {
      if (channelStates[7]) servoPos[0] = minAngle[0];
      else servoPos[0] = maxAngle[0];
    }
    if (channels[7].state) servo1.setTargetDeg(servoPos[0]);
#endif
  }

  if (SERVO2_RELAY) {   // если реле
    digitalWrite(SERVO_1, channelStates[8]);
  } else {
#if (SERVO2_RELAY == 0)
    if (channels[8].mode != 4 && channels[8].mode != 5) {
      if (channelStates[8]) servoPos[1] = minAngle[1];
      else servoPos[1] = maxAngle[1];
    }
    if (channels[8].state) servo2.setTargetDeg(servoPos[1]);
#endif
  }

  // привод
  if (lastDriveState != channelStates[9]) {
    lastDriveState = channelStates[9];
    driveState = 1;
  }
}

boolean checkDay(byte channel) {
  if ((realTime[0] >= channels[channel].hour1 && realTime[0] < channels[channel].hour2) ) {
    return true;
  } else if ((realTime[0] < channels[channel].hour1 || realTime[0] >= channels[channel].hour2)) {
    return false;
  }
}

byte checkHysteresis(byte channel) {
  byte state = 0;
  if (channels[channel].thresholdMax > channels[channel].threshold) {
    // больше максимума - включить
    if (sensorVals[channels[channel].sensor] > channels[channel].thresholdMax)
      state = 1;

    // меньше минимума - выключить
    if (sensorVals[channels[channel].sensor] < channels[channel].threshold)
      state = 2;
  } else {
    // больше максимума - выключить
    if (sensorVals[channels[channel].sensor] > channels[channel].threshold)
      state = 2;

    // меньше минимума - включить
    if (sensorVals[channels[channel].sensor] < channels[channel].thresholdMax)
      state = 1;
  }
  return state;
}

void checkDawn(byte curChannel) {
#if (USE_DAWN == 1)
  byte curPWMchannel;
  switch (curChannel) {
    case 2: curPWMchannel = 0;  // канал 2
      break;
    case 3: curPWMchannel = 1;  // канал 3
      break;
    case 7: curPWMchannel = 2;  // серво 1
      break;
    case 8: curPWMchannel = 3;  // серво 2
      break;
  }
  int nowTime = realTime[0] * 60 + realTime[1];       // минуты с начала суток
  byte thisSignal;
  if (nowTime < (dawn[curPWMchannel].start * 60)) {   // до рассвета (ночь)
    if (channels[curChannel].direction) thisSignal = dawn[curPWMchannel].minV;
    else thisSignal = dawn[curPWMchannel].maxV;
  } else if (nowTime < (dawn[curPWMchannel].start * 60 + dawn[curPWMchannel].dur1)) { // рассвет
    if (channels[curChannel].direction)
      thisSignal = map((nowTime - dawn[curPWMchannel].start * 60), 0, dawn[curPWMchannel].dur1, dawn[curPWMchannel].minV, dawn[curPWMchannel].maxV);
    else
      thisSignal = map((nowTime - dawn[curPWMchannel].start * 60), 0, dawn[curPWMchannel].dur1, dawn[curPWMchannel].maxV, dawn[curPWMchannel].minV);
  } else if (nowTime < (dawn[curPWMchannel].stop * 60)) {  // день
    if (channels[curChannel].direction) thisSignal = dawn[curPWMchannel].maxV;
    else thisSignal = dawn[curPWMchannel].minV;
  } else if (nowTime < (dawn[curPWMchannel].stop * 60 + dawn[curPWMchannel].dur2)) {  // закат
    if (channels[curChannel].direction)
      thisSignal = map((nowTime - dawn[curPWMchannel].stop * 60), 0, dawn[curPWMchannel].dur2, dawn[curPWMchannel].maxV, dawn[curPWMchannel].minV);
    else
      thisSignal = map((nowTime - dawn[curPWMchannel].stop * 60), 0, dawn[curPWMchannel].dur2, dawn[curPWMchannel].minV, dawn[curPWMchannel].maxV);
  } else {  // ночь
    if (channels[curChannel].direction) thisSignal = dawn[curPWMchannel].minV;
    else thisSignal = dawn[curPWMchannel].maxV;
  }

  switch (curChannel) {
    case 2: pwmVal[0] = thisSignal;
      break;
    case 3: pwmVal[1] = thisSignal;
      break;
    case 7: servoPos[0] = thisSignal;
      break;
    case 8: servoPos[1] = thisSignal;
      break;
  }
#endif
}

void driveTick() {
  if (channels[9].state || serviceFlag || manualControl) {
    if (driveState == 1) {
      driveState = 2;
      driveTimer = millis();
      boolean thisDirection;
      if (channels[9].mode != 4) {
        thisDirection = channelStates[9];
      } else {
        if (driveStep > 0) {
          thisDirection = channels[9].direction;
        } else {
          thisDirection = !channels[9].direction;
        }
      }
      if (serviceFlag) thisDirection = channelStatesServ[9];
      if (manualControl) thisDirection = manualPos;

      if (thisDirection) {
        digitalWrite(DRV_SIGNAL1, !DRIVER_LEVEL);
        digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL);
      } else {
        digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL);
        digitalWrite(DRV_SIGNAL2, !DRIVER_LEVEL);
      }
    }

    long thisTimeout;
    if (channels[9].mode == 4) thisTimeout = abs(driveStep);
    else thisTimeout = (long)driveTimeout * 1000;
    if (driveState == 2 && millis() - driveTimer >= thisTimeout) {
      driveState = 0;
      digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL);
      digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL);
      manualControl = false;
    }
  }
}

void getDay() {
  uint16_t days = now.day();
  for (byte i = 0; i < now.month() - 1; i++)
    days += daysMonth[i];

  if (now.month() > 2 && now.year() % 4 == 0) // високосный
    ++days;
  thisDay = days;
}
