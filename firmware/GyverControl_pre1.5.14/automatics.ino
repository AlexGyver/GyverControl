void timersTick() {   // каждую секунду
  // получаем время
  realTime[0] = rtc.getHours();
  realTime[1] = rtc.getMinutes();
  realTime[2] = rtc.getSeconds();
  getDay();   // получить номер дня
  uptime += (float)0.0000115741;   // аптайм у нас в сутках! 1/(24*60*60)

  for (byte curChannel = 0; curChannel < 10; curChannel++) {
    if (channels[curChannel].state                                                             // если канал активен (on/off)
        && (curChannel >= 7 || ((curChannel < 7 || !channels[curChannel].type) && channels[curChannel].relayType != 2) ) ) {   // и для всех кроме ОБЩИХ реле
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
        case 2:   // ---------------------- если неделька ----------------------
          if (checkDay(curChannel)) channelStates[curChannel] = channels[curChannel].direction;     // включились до stop hour
          else channelStates[curChannel] = !channels[curChannel].direction;    // выключились до следующего start hour
          break;
        case 3:   // ---------------------- если датчик ----------------------
          if (millis() - timerMillis[curChannel] >= channels[curChannel].sensPeriod * 1000L) {
            timerMillis[curChannel] = millis();

            /*
              флаг и laststate на каждый канал, +20 байт
              если (laststate != checkHysteresis) {
                если (!флаг) {
                  сброс таймера
                  флаг = 1
                } иначе {
                  laststate = checkHysteresis
                  стейт меняем
                  флаг = 0
                }
              }
            */

            if (checkHysteresis(curChannel) == 1) channelStates[curChannel] = channels[curChannel].direction;
            else if (checkHysteresis(curChannel) == 2) channelStates[curChannel] = !channels[curChannel].direction;
          }
          break;
        case 4:   // ---------------------- если ПИД ----------------------
          break;
        case 5:   // -------------------- если рассвет --------------------
#if (USE_DAWN == 1)
          if ((realTime[2] % 10) == 0 || startFlagDawn) {    // проверка на рассвет каждые 10 секунд
            checkDawn(curChannel);          
          }
#endif
          break;
      }
      // если week глобальный, выключить канал в нерабочее время!
      if (channels[curChannel].global && !checkDay(curChannel)) channelStates[curChannel] = !channels[curChannel].direction;
    }
  }
  startFlagDawn = false;
  // --- применяем ---

  // реле
  boolean atLeastOneValve = false;                              // флаг "хотя бы одного" клапана
  for (byte relay = 0; relay < 10; relay++) {
    if (!channels[relay].type && channels[relay].state && channels[relay].mode != 4 && channels[relay].mode != 5) {
      if (channels[relay].relayType == 1                          // если реле - КЛАПАН
          && channelStates[relay] == channels[relay].direction)   // и он ВКЛЮЧЕН
        atLeastOneValve = true;                                   // запоминаем, что нужно включить общий канал
      digitalWrite(relayPins[relay], channelStates[relay]);       // включаем/выключаем все реле типов РЕЛЕ и КЛАПАН
    }
  }

  for (byte relay = 0; relay < 10; relay++) {
    if (!channels[relay].type && channels[relay].state && channels[relay].relayType == 2
        && channels[relay].mode != 4 && channels[relay].mode != 5) {    // только для ОБЩИХ каналов реле, кроме ПИД и рассвета
      if (atLeastOneValve) channelStates[relay] = channels[relay].direction;  // включить общий
      else channelStates[relay] = !channels[relay].direction;                 // выключить общий
      digitalWrite(relayPins[relay], channelStates[relay]);                   // применить
    }
  }

#if (USE_PID_RELAY == 1)
  // шим-реле - пид
  if (channels[0].mode == 4) {
    PWMactive[0] = (long)pwmVal[0] * PWMperiod / 255;
  }

  if (channels[1].mode == 4) {
    PWMactive[1] = (long)pwmVal[1] * PWMperiod / 255;
  }
#endif

  // шим - рассвет или пид
  if (channels[2].mode >= 4) {
    analogWrite(RELAY_2, pwmVal[2]);
  }
  if (channels[3].mode >= 4) {
    analogWrite(RELAY_3, pwmVal[3]);
  }

  // серво
#if (SERVO1_RELAY == 1)   // если реле
  digitalWrite(SERVO_0, channelStates[7]);
#else   // если серво
  if (channels[7].mode != 4 && channels[7].mode != 5) {   // не пид и не рассвет
    pwmVal[4] = channelStates[7] ? settings.minAngle[0] : settings.maxAngle[0];
  }

#if (SMOOTH_SERVO == 1)
  if (channels[7].state) servo1.setTargetDeg(pwmVal[4]);
#else
  if (channels[7].state) servo1.write(pwmVal[4]);
#endif

#endif


#if (SERVO2_RELAY == 1)   // если реле
  digitalWrite(SERVO_1, channelStates[8]);
#else
  if (channels[8].mode != 4 && channels[8].mode != 5) {   // не пид и не рассвет
    pwmVal[5] = channelStates[8] ? settings.minAngle[1] : settings.maxAngle[1];
  }

#if (SMOOTH_SERVO == 1)
  if (channels[8].state) servo2.setTargetDeg(pwmVal[5]);
#else
  if (channels[8].state) servo2.write(pwmVal[5]);
#endif

#endif


#if (USE_DRIVE == 1)
  // привод
  if (lastDriveState != channelStates[9]) {
    lastDriveState = channelStates[9];
    driveState = 1;
  }
#endif

#if (SCHEDULE_NUM > 0)
  checkShedule();   // проверяем расписание
#endif
}

boolean checkDay(byte channel) {
  byte today = rtc.getDay();
  if (today == 0) today = 7;
  long thisTime = ((long)realTime[0] * 3600 + realTime[1] * 60 + realTime[2]);
  if (bitRead(channels[channel].week, today - 1) &&
      ((channels[channel].weekOff > channels[channel].weekOn) ?
       (thisTime > channels[channel].weekOn && thisTime < channels[channel].weekOff) :
       (thisTime > channels[channel].weekOn || thisTime < channels[channel].weekOff))) return true;
  else return false;
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
  if (curChannel == 2 || curChannel == 3 || curChannel == 7 || curChannel == 8) {
    byte curPWMchannel = channelToPWM[curChannel];
    long nowTime = realTime[0] * 60 * 60L + realTime[1] * 60L + realTime[2];       // секунды с начала суток
    long timeStart = dawn[curPWMchannel].start * 60 * 60L;
    long timeStop = dawn[curPWMchannel].stop * 60 * 60L;
    byte thisSignal;
    if (nowTime < (timeStart)) {   // до рассвета (ночь)
      if (channels[curChannel].direction) thisSignal = dawn[curPWMchannel].minV;
      else thisSignal = dawn[curPWMchannel].maxV;
    } else if (nowTime < (timeStart + dawn[curPWMchannel].dur1 * 60)) { // рассвет
      if (channels[curChannel].direction)
        thisSignal = map((nowTime - timeStart), 0, dawn[curPWMchannel].dur1 * 60, dawn[curPWMchannel].minV, dawn[curPWMchannel].maxV);
      else
        thisSignal = map((nowTime - timeStart), 0, dawn[curPWMchannel].dur1 * 60, dawn[curPWMchannel].maxV, dawn[curPWMchannel].minV);
    } else if (nowTime < (timeStop)) {  // день
      if (channels[curChannel].direction) thisSignal = dawn[curPWMchannel].maxV;
      else thisSignal = dawn[curPWMchannel].minV;
    } else if (nowTime < (timeStop + dawn[curPWMchannel].dur2 * 60)) { // закат
      if (channels[curChannel].direction)
        thisSignal = map((nowTime - timeStop), 0, dawn[curPWMchannel].dur2 * 60, dawn[curPWMchannel].maxV, dawn[curPWMchannel].minV);
      else
        thisSignal = map((nowTime - timeStop), 0, dawn[curPWMchannel].dur2 * 60, dawn[curPWMchannel].minV, dawn[curPWMchannel].maxV);
    } else {  // ночь
      if (channels[curChannel].direction) thisSignal = dawn[curPWMchannel].minV;
      else thisSignal = dawn[curPWMchannel].maxV;
    }
    pwmVal[curPWMchannel] = thisSignal;
  }
#endif
}

void getDay() {
  uint16_t days = rtc.getDate();
  for (byte i = 0; i < rtc.getMonth() - 1; i++)
    days += daysMonth[i];

  if (rtc.getMonth() > 2 && rtc.getYear() % 4 == 0) // високосный
    ++days;
  thisDay = days;
}

#if (SCHEDULE_NUM > 0)
void checkShedule() {
  int thisDayFromZero = thisDay - 1;
  for (byte i = 0; i < SCHEDULE_NUM; i++) {
    if (schedule[i].pidChannel > 0) {  // если не off
      byte idx;
      if (schedule[i].startDay > 0) {   // в заданный период
        if (schedule[i].startDay < schedule[i].endDay) {  // без перехода через новый год
          if (thisDayFromZero < schedule[i].startDay) idx = 0;
          else if (thisDayFromZero >= schedule[i].endDay) idx = schedule[i].pointAmount - 1;
          else idx = schedule[i].pointAmount * (thisDayFromZero - schedule[i].startDay) / (schedule[i].endDay - schedule[i].startDay);
        } else {          // переход через новый год
          byte dayNum = (thisDayFromZero < 365 && thisDayFromZero > schedule[i].endDay) ? (thisDayFromZero - schedule[i].startDay) : (365 - schedule[i].startDay + thisDayFromZero);
          if (thisDayFromZero >= schedule[i].endDay && thisDayFromZero < schedule[i].startDay) idx = 0;
          else idx = schedule[i].pointAmount * dayNum / (schedule[i].endDay + 365 - schedule[i].startDay);
        }
      } else {  // ежедневно
        idx = schedule[i].pointAmount * realTime[0] / 24;
      }
      PID[schedule[i].pidChannel - 1].setpoint = schedule[i].setpoints[idx] / 10.0f;
    }
  }
}
#endif
