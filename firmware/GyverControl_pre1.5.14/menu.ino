void incr(boolean* val, int incr) {
  if (incr == 1) *val = true;
  else *val = false;
}
void incr(int* val, int incr, int limit) {
  *val += incr;
  if (*val > limit) *val = limit;
  if (*val < 0) *val = 0;
}
void incr(uint32_t* val, int incr, int limit) {
  if (incr < 0 && *val < -incr) *val = 0;
  else *val += incr;
  if (*val > limit) *val = limit;
  if (*val < 0) *val = 0;
}
void incr(int8_t* val, int incr, int limit) {
  *val += incr;
  if (*val > limit) *val = limit;
  if (*val < 0) *val = 0;
}
void incr(float* val, float incr, float limit) {
  *val += incr;
  if (*val > limit) *val = limit;
  if (*val < 0) *val = 0;
}
void incr(byte* val, int incr, int limit) {
  int value = (int) * val;
  value += incr;
  if (value > limit) value = limit;
  if (value < 0) value = 0;
  *val = (byte)value;
}
void incrInt(int* val, int incr, int limit) {
  *val += incr;
  if (*val > limit) *val = limit;
  if (*val < -100) *val = -100;
}

#define MAX_CH_POS (9+SCHEDULE_NUM)

#define MIN_CH_POS -3
#if (START_MENU == 1)
#define MIN_CH_POS -2
#endif

#if (SCHEDULE_NUM > 0)
void scheduleSett(int dir) {
  EEPROM_updFlag = true;
  int8_t schChannel = currentChannel - 10;
  switch (arrowPos) {
    case 0:
      currentChannel += dir;
      if (currentChannel > MAX_CH_POS) currentChannel = MAX_CH_POS;
      currentLine = 4;
      break;
    case 1: incr(&schedule[schChannel].pidChannel, dir, 7); break;
    case 2: incr(&schedule[schChannel].startDay, dir, 365); break;
    case 3: incr(&schedule[schChannel].endDay, dir, 365); break;
    case 4: incr(&schedule[schChannel].pointAmount, dir, SCHEDULE_MAX); break;
  }
  if (arrowPos > 4) {
    incr(&schedule[schChannel].setpoints[arrowPos - 5], dir, 500);
  }
}
#endif


void serviceSett(int dir) {
  switch (arrowPos) {
    case 0:
#if (START_MENU == 1)
      if (!startService)
#endif
        currentChannel += 1 * dir;
      if (currentChannel < MIN_CH_POS) currentChannel = MIN_CH_POS;
      if (serviceFlag && currentChannel > -3) serviceOUT();
      currentLine = 4;

      break;
    case 1: realTime[0] += dir;
      if (realTime[0] > 23) realTime[0] = 0;
      if (realTime[0] < 0) realTime[0] = 23;
      correctTime();
      currentLine = 0;
      break;
    case 2: realTime[1] += dir;
      correctTime();
      currentLine = 0;
      break;
    case 3: realTime[2] += dir;
      correctTime();
      currentLine = 0;
      break;
    case 4: incr(&channelStatesServ[0], dir);
      currentLine = 2;
      break;
    case 5: incr(&channelStatesServ[1], dir);
      currentLine = 2;
      break;
    case 6: incr(&channelStatesServ[2], dir);
      currentLine = 2;
      break;
    case 7: incr(&channelStatesServ[3], dir);
      currentLine = 2;
      break;
    case 8: incr(&channelStatesServ[4], dir);
      currentLine = 2;
      break;
    case 9: incr(&channelStatesServ[5], dir);
      currentLine = 2;
      break;
    case 10: incr(&channelStatesServ[6], dir);
      currentLine = 2;
      break;
    case 11: incr(&channelStatesServ[9], dir);
      driveState = 1;
      currentLine = 3;
      break;
    case 12:
#if (SERVO1_RELAY == 1)    // если реле
      channelStatesServ[7] = (dir == 1) ? 1 : 0;
#else
      incr(&servoPosServ[0], dir * 10, 180);
#endif
      currentLine = 3;
      break;
    case 13:
#if (SERVO2_RELAY == 1)    // если реле
      channelStatesServ[8] = (dir == 1) ? 1 : 0;
#else
      incr(&servoPosServ[1], dir * 10, 180);
#endif
      currentLine = 3;
      break;
  }
}

void settingsSett(int dir) {
  switch (arrowPos) {
    case 0:
      updateSettings();
      currentChannel += dir;
      if (currentChannel < MIN_CH_POS) currentChannel = MIN_CH_POS;
      currentLine = 4;
#if (START_MENU == 0)
      if (!serviceFlag && currentChannel == -3) serviceIN();
#endif
      analogWrite(DRV_PWM, settings.drvSpeed);
#if (SERVO1_RELAY == 0 && SMOOTH_SERVO == 1)
      servo1.setSpeed(settings.srv1_Speed);    // ограничить скорость
      servo1.setAccel(settings.srv1_Acc);      // установить ускорение (разгон и торможение)
#endif
#if (SERVO2_RELAY == 0 && SMOOTH_SERVO == 1)
      servo2.setSpeed(settings.srv2_Speed);    // ограничить скорость
      servo2.setAccel(settings.srv2_Acc);      // установить ускорение (разгон и торможение)
#endif
      break;
    case 1: incr(&settings.backlight, dir); break;
    case 2: incr(&settings.backlTime, dir * 5, 255);
      if (settings.backlTime == 0) settings.backlTime = 5;
      backlTimer = millis();
      break;
    case 3: incr(&settings.drvSpeed, dir * 5, 255); break;
    case 4: setDMY(0, dir); break;
    case 5: setDMY(1, dir); break;
    case 6: setDMY(2, dir); break;
    case 7: incr(&settings.comSensPeriod, dir, 1000); break;
    case 8: incr(&settings.plotMode, dir, 2);
      plotTimeout = plotTimeouts[settings.plotMode];
      break;
#if (SMOOTH_SERVO == 1)
    case 9: incr(&settings.srv1_Speed, dir * 5, 255); break;
    case 10: incr(&settings.srv1_Acc, dir * 0.1, 1.0); break;
    case 11: incr(&settings.srv2_Speed, dir * 5, 255); break;
    case 12: incr(&settings.srv2_Acc, dir * 0.1, 1.0); break;
#endif
  }
}

void setDMY(byte set, int dir) {
  int thisD = rtc.getDate();
  int thisM = rtc.getMonth();
  int thisY = rtc.getYear();
  switch (set) {
    case 0: thisD += dir; if (thisD < 1) thisD = 1; break;
    case 1: thisM += dir; if (thisM < 1) thisM = 1; break;
    case 2: thisY += dir; break;
  }
  rtc.setTime(rtc.getSeconds(), rtc.getMinutes(), rtc.getHours(), thisD, thisM, thisY);
}

// выбор канала
void channelSett(int dir) {
  if (arrowPos > 0) EEPROM_updFlag = true;

  switch (arrowPos) {
    case 0:
      if (currentChannel >= 0) updateEEPROM(currentChannel);
      currentChannel += 1 * dir;
      if (currentChannel > MAX_CH_POS) currentChannel = MAX_CH_POS;
      currentLine = 4;
      break;
    case 1:
      incr(&channels[currentChannel].state, dir);
#if (SERVO1_RELAY == 0)
      if (currentChannel == 7) {
        if (dir == 1) servo1.attach(SERVO_0, 600, 2400);
        else servo1.detach();
      }
#endif
#if (SERVO2_RELAY == 0)
      if (currentChannel == 8) {
        if (dir == 1) servo2.attach(SERVO_1, 600, 2400);
        else servo2.detach();
      }
#endif
      currentLine = 4;
      break;
    case 2:
      currentLine = 1;
      break;
    case 3:
      incr(&channels[currentChannel].direction, dir);
      currentLine = 2;
      break;
    case 4:
      if (curMode == 0) {   // реле
        incr(&channels[currentChannel].relayType, dir, 2);
        currentLine = 4;

      } else if (curMode == 1) {  // серво
        if (currentChannel == 7) {
          incr(&settings.minAngle[0], 5 * dir, 180);
        } else if (currentChannel == 8) {
          incr(&settings.minAngle[1], 5 * dir, 180);
        }
        currentLine = 3;

      } else {    // привод
        incr(&settings.driveTimeout, dir, 255);
        currentLine = 3;
      }
      break;
    case 5:
      if (curMode == 1) {
        if (currentChannel == 7) {
          incr(&settings.maxAngle[0], 5 * dir, 180);
        } else if (currentChannel == 8) {
          incr(&settings.maxAngle[1], 5 * dir, 180);
        }
      }
      currentLine = 3;
      break;
  }
}

void chSettingsSett(int dir) {
  byte curPWMchannel = channelToPWM[currentChannel];
  if (arrowPos > 0) EEPROM_updFlag = true;
  switch (arrowPos) {
    case 0:
      hms_to_s(); // сохранить время для режимов 0 и 2
      if (dir == 1) {
        if ((currentChannel <= 1 && USE_PID_RELAY == 1) || currentChannel == 9) {  // для ШИМ-реле и привода
          incr(&channels[currentChannel].mode, 1, 4);
        } else if (currentChannel == 2 || currentChannel == 3 ||
                   (currentChannel == 7 && SERVO1_RELAY == 0) ||
                   (currentChannel == 8 && SERVO2_RELAY == 0)) { // для ШИМ и СЕРВО каналов
          incr(&channels[currentChannel].mode, 1, 5);
        } else {                                              // для обычных реле
          incr(&channels[currentChannel].mode, 1, 3);
        }
      } else {
        incr(&channels[currentChannel].mode, -1, 5);
      }
      s_to_hms(); // загрузить новое время для режимов 0 и 2
      currentLine = 4;
      break;
    case 1:
      currentLine = 4;
      break;
    case 2:
      currentLine = 1;
      switch (thisMode) {
        case 0: incr(&thisH[0], dir, 999);
          break;
        case 1: incr(&channels[currentChannel].impulsePrd, dir, 19);
          break;
        case 2: // неделька
          bitWrite(channels[currentChannel].week, 0, ( (dir == 1) ? 1 : 0) );
          break;
        case 3:
          if (channels[currentChannel].sensPeriod < 60) {
            incr(&channels[currentChannel].sensPeriod, dir * 2, 32000);
          } else {
            incr(&channels[currentChannel].sensPeriod, dir * 60, 32000);
          }
          break;
#if (USE_PID == 1)
        case 4: PID[curPWMchannel].kP += dir * 0.1;
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&dawn[curPWMchannel].start, dir, 23);
          break;
#endif
      }
      break;
    case 3:
      switch (thisMode) {
        case 0:
          thisM[0] += dir;
          currentLine = 1;
          break;
        case 1:
          if (channels[currentChannel].work < 10) {
            dir = (dir > 0) ? 1 : -1;
          }
          else if (channels[currentChannel].work > 60) dir = dir * 60;

          incr(&channels[currentChannel].work, dir, 10000);
          if (channels[currentChannel].work == 0) channels[currentChannel].work = 1;
          currentLine = 2;
          break;
        case 2:  // неделька
          bitWrite(channels[currentChannel].week, 1, ( (dir == 1) ? 1 : 0) );
          break;
        case 3:
          incr(&channels[currentChannel].sensor, dir, 5);
          currentLine = 2;
          break;
#if (USE_PID == 1)
        case 4: PID[curPWMchannel].kI += dir * 0.05;
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&dawn[curPWMchannel].dur1, dir * 10, 255);
          break;
#endif
      }
      break;
    case 4:
      switch (thisMode) {
        case 0: thisS[0] += dir;
          currentLine = 1;
          break;
        case 1: incr(&channels[currentChannel].startHour, dir, 23);
          currentLine = 3;
          break;
        case 2: // неделька
          bitWrite(channels[currentChannel].week, 2, ( (dir == 1) ? 1 : 0) );
          break;
        case 3:
          if (channels[currentChannel].threshold > 255) dir *= 10;
          incrInt(&channels[currentChannel].threshold, dir, 5000);
          currentLine = 3;
          break;
#if (USE_PID == 1)
        case 4: PID[curPWMchannel].kD += dir * 0.05;
          break;
#endif
#if (USE_DAWN == 1)
        case 5:
          incr(&dawn[curPWMchannel].stop, dir, 23);
          break;
#endif
      }
      break;
    case 5:
      switch (thisMode) {
        case 0: incr(&thisH[1], dir, 23);
          currentLine = 2;
          break;
        case 2:  // неделька
          bitWrite(channels[currentChannel].week, 3, ( (dir == 1) ? 1 : 0) );
          break;
        case 3:
          if (channels[currentChannel].threshold > 255) dir *= 10;
          incrInt(&channels[currentChannel].thresholdMax, dir, 5000);
          currentLine = 3;
          break;
#if (USE_PID == 1)
        case 4: incr(&PID[curPWMchannel].sensor, dir, 5);
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&dawn[curPWMchannel].dur2, dir * 10, 255);
          break;
#endif
      }
      break;
    case 6:
      switch (thisMode) {
        case 0: thisM[1] += dir;
          currentLine = 2;
          break;
        case 2: // неделька
          bitWrite(channels[currentChannel].week, 4, ( (dir == 1) ? 1 : 0) );
          break;
#if (USE_PID == 1)
        case 4:
          incr(&PID[curPWMchannel].setpoint, dir * 0.1, 255);
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&dawn[curPWMchannel].minV, dir * 10, 255);
          break;
#endif
      }
      break;
    case 7:
      switch (thisMode) {
        case 0: thisS[1] += dir;
          currentLine = 2;
          break;
        case 2: // неделька
          bitWrite(channels[currentChannel].week, 5, ( (dir == 1) ? 1 : 0) );
          break;
#if (USE_PID == 1)
        case 4: incr(&PID[curPWMchannel].dT, dir, 255);
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&dawn[curPWMchannel].maxV, dir * 10, 255);
          break;
#endif
      }
      break;
    case 8:
      switch (thisMode) {
        case 2: // неделька
          bitWrite(channels[currentChannel].week, 6, ( (dir == 1) ? 1 : 0) );
          break;
#if (USE_PID == 1)
        case 4: incr(&PID[curPWMchannel].minSignal, dir * 5, 255);
          break;
#endif
      }
      break;
    case 9:
      switch (thisMode) {
        case 2: // неделька
          thisH[0] += dir;
          currentLine = 2;
          break;
#if (USE_PID == 1)
        case 4: incr(&PID[curPWMchannel].maxSignal, dir * 5, 255);
          break;
#endif
      }
      break;
    case 10:
      thisM[0] += dir;
      currentLine = 2;
      break;
    case 11:
      thisS[0] += dir;
      currentLine = 2;
      break;
    case 12:
      thisH[1] += dir;
      currentLine = 3;
      break;
    case 13:
      thisM[1] += dir;
      currentLine = 3;
      break;
    case 14:
      thisS[1] += dir;
      currentLine = 3;
      break;
    case 15:
      incr(&channels[currentChannel].global, dir);
      currentLine = 3;
      break;
  }
}

void controlTick() {
  enc.tick();  // отработка энкодера
  if (enc.isHolded()) {
    manualControl = true;
    driveState = 1;
    manualPos = !manualPos;
  }

  if (enc.isClick()) {
    settingsTimer = millis();
    backlTimer = millis();
    if (!backlState) {
      backlOn();
      return;
    }
    
    if (CONTROL_TYPE == 1 && currentChannel < 10 &&
#if (START_MENU == 1)
        !startService &&
#endif
        !(arrowPos == 1 && navDepth == 1) &&  // back
        !(arrowPos == 2 && navDepth == 0 && currentChannel >= 0)) { // настройки канала
      controlState = !controlState;
      drawArrow();
    }
#if (SCHEDULE_NUM > 0)
    if (currentChannel >= 10) controlState = !controlState;
#endif

#if (USE_CO2 == 1)
    CO2_rst = true;
#endif

    if (currentChannel >= 0 && currentChannel < 10) {
      if (arrowPos == 2 && navDepth == 0) { // заходим в настройки режима
        navDepth = 1;
        arrowPos = 0;
        s_to_hms();
      }
      if (arrowPos == 1 && navDepth == 1) { // нажатие на Back
        navDepth = 0;
        arrowPos = 2;
        hms_to_s();
      }
      currentLine = 4;
      redrawScreen();
    }

    if (currentChannel == -2) redrawMainSettings();
#if (SCHEDULE_NUM > 0)
    if (currentChannel >= 10) redrawSchedule();
#endif
#if (START_MENU == 1)
    if (arrowPos == 0 && startService) startService = false;
#endif
  }

  if (enc.isTurn()) {
    settingsTimer = millis();
    backlTimer = millis();
    if (!backlState) {
      backlOn();
      return;
    }

#if (USE_CO2 == 1)
    CO2_rst = true;
#endif

#if (FAST_TURN == 1)
    byte fastTurn = enc.isFast() ? FAST_TURN_STEP : 1;
#else
    byte fastTurn = 1;
#endif

    // позиции стрелки для каналов и режимов
    if (!controlState && enc.isRight()) {
      if (currentChannel >= 0 && currentChannel < 10) {
        debugPage = 0;
        if (navDepth == 0) {  // настройки канала
          if (channels[currentChannel].state) {
            if (currentChannel > 6 && currentChannel < 9) {
              byte thisMax = 5;
              if ( (currentChannel == 7 && SERVO1_RELAY == 1) ||
                   (currentChannel == 8 && SERVO2_RELAY == 1) ) thisMax = 4;
              if (++arrowPos > thisMax) arrowPos = thisMax;
            } else {
              if (++arrowPos > 4) arrowPos = 4;
            }
          } else {
            if (++arrowPos > 1) arrowPos = 1;
          }
        } else {        // настройки режима
          switch (channels[currentChannel].mode) {
            case 0: incr(&arrowPos, 1, 7); break; 
            case 1: incr(&arrowPos, 1, 4); break;
            case 2: incr(&arrowPos, 1, 15); break;
            case 3: incr(&arrowPos, 1, 5); break;
            case 4: incr(&arrowPos, 1, 9); break;
            case 5: incr(&arrowPos, 1, 7); break;
          }
        }
      } else if (currentChannel == -1) {  // позиции стрелки для дебаг
        arrowPos = 0;
#if (USE_PLOTS == 1)
        if (++debugPage > 6) debugPage = 6;
#endif
        if (debugPage == 0) {
          lcd.clear();
          redrawDebug();
        } else {
          lcd.clear();
          redrawPlot();
        }
      } else if (currentChannel == -2) {  // позиции стрелки для настроек
        incr(&arrowPos, 1, SETTINGS_AMOUNT);

      } else if (currentChannel == -3) {  // позиции стрелки для сервиса
        incr(&arrowPos, 1, 13);
        
      } else if (currentChannel >= 10) {
#if (SCHEDULE_NUM > 0)
        incr(&arrowPos, 1, (4 + schedule[currentChannel - 10].pointAmount));
#endif
      }
      drawArrow();

    } else if (!controlState && enc.isLeft()) {
      if (--arrowPos < 0) arrowPos = 0;
      if (currentChannel == -1) {
        if (--debugPage < 0) debugPage = 0;
        if (debugPage == 0) {
          lcd.clear();
          redrawDebug();
        } else {
          lcd.clear();
          redrawPlot();
        }
      }
      drawArrow();
    }

    if (currentChannel < 7 && currentChannel >= 0) curMode = 0;
    else if (currentChannel < 9) curMode = 1;
    else curMode = 2;
#if (SERVO1_RELAY == 1)
    if (currentChannel == 7) curMode = 0;
#endif
#if (SERVO2_RELAY == 1)
    if (currentChannel == 8) curMode = 0;
#endif
    thisMode = channels[currentChannel].mode;

    // ============ изменение
    int8_t changed = 0;

#if (CONTROL_TYPE == 0)
    if (enc.isRightH()) {
#elif (CONTROL_TYPE == 1)
    if ((controlState && enc.isRight()) || enc.isRightH()) {
#endif
      changed = 1;

#if (CONTROL_TYPE == 0)
    } else if (enc.isLeftH()) {
#elif (CONTROL_TYPE == 1)
    } else if ((controlState && enc.isLeft()) || enc.isLeftH()) {
#endif
      changed = -1;
    }
    if (changed != 0) {
      debugPage = 0;
      if (currentChannel < 0) {
        if (currentChannel == -2) settingsSett(fastTurn * changed);
        else serviceSett(changed);
      } else if (currentChannel < 10) {
        if (navDepth == 0) {
          channelSett(fastTurn * changed);
        } else {
          chSettingsSett(fastTurn * changed);
        }
        if (thisMode == 0 || thisMode == 2) recalculateTime();
      } else {
#if (SCHEDULE_NUM > 0)
        scheduleSett(fastTurn * changed);
#endif
      }
      redrawScreen();
    }

    if ( (currentChannel == -2) ||
         (currentChannel >= 0 && navDepth == 1 && thisMode >= 4) ||
         currentChannel >= 10)
      redrawScreen();  // костыль для пида и рассвета и меню настроек
  }
}

void recalculateTime() {
  for (byte i = 0; i < 2; i++) {
    if (thisS[i] > 59) {
      thisS[i] = 0;
      thisM[i]++;
    }
    if (thisM[i] > 59) {
      thisM[i] = 0;
      thisH[i]++;
    }
    if (thisS[i] < 0) {
      if (thisM[i] > 0) {
        thisS[i] = 59;
        thisM[i]--;
      } else thisS[i] = 0;
    }
    if (thisM[i] < 0) {
      if (thisH[i] > 0) {
        thisM[i] = 59;
        thisH[i]--;
      } else thisM[i] = 0;
    }
    if (thisH[i] < 0) thisH[i] = 0;
    if (thisMode == 2 && thisH[i] > 23) thisH[i] = 23;
  }
}

void correctTime() {
  timeChanged = true;
  for (byte i = 0; i < 1; i++) {
    if (realTime[2] > 59) {
      realTime[2] = 0;
      realTime[1]++;
    }
    if (realTime[1] > 59) {
      realTime[1] = 0;
      realTime[0]++;
    }
    if (realTime[2] < 0) {
      if (realTime[1] > 0) {
        realTime[2] = 59;
        realTime[1]--;
      } else realTime[2] = 0;
    }
    if (realTime[1] < 0) {
      if (realTime[0] > 0) {
        realTime[1] = 59;
        realTime[0]--;
      } else realTime[1] = 0;
    }
    if (realTime[0] < 0) realTime[0] = 0;
  }
}


// перевод секунд в ЧЧ:ММ:СС
void s_to_hms() {
  uint32_t period = 0;

  if (channels[currentChannel].mode == 0)
    period = channels[currentChannel].period;
  else if (channels[currentChannel].mode == 2)
    period = channels[currentChannel].weekOn;

  thisH[0] = floor((long)period / 3600);    // секунды в часы
  thisM[0] = floor((period - (long)thisH[0] * 3600) / 60);
  thisS[0] = period - (long)thisH[0] * 3600 - thisM[0] * 60;

  if (channels[currentChannel].mode == 0)
    period = channels[currentChannel].work;
  else if (channels[currentChannel].mode == 2)
    period = channels[currentChannel].weekOff;

  thisH[1] = floor((long)period / 3600);    // секунды в часы
  thisM[1] = floor((period - (long)thisH[1] * 3600) / 60);
  thisS[1] = period - (long)thisH[1] * 3600 - thisM[1] * 60;
}

// перевод ЧЧ:ММ:СС в секунды
void hms_to_s() {
  if (channels[currentChannel].mode == 0) {
    channels[currentChannel].period = ((long)thisH[0] * 3600 + thisM[0] * 60 + thisS[0]);
    channels[currentChannel].work = ((long)thisH[1] * 3600 + thisM[1] * 60 + thisS[1]);
  }
  if (channels[currentChannel].mode == 2) {
    channels[currentChannel].weekOn = ((long)thisH[0] * 3600 + thisM[0] * 60 + thisS[0]);
    channels[currentChannel].weekOff = ((long)thisH[1] * 3600 + thisM[1] * 60 + thisS[1]);
  }
}
