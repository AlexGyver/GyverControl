byte thisMode;
byte curMode;

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

void serviceSett(int dir) {
  switch (arrowPos) {
    case 0:
      currentChannel += 1 * dir;
      currentChannel = constrain(currentChannel, -3, 9);
      if (serviceFlag && currentChannel > -3) serviceOUT();
      if (!serviceFlag && currentChannel == -3) serviceIN();
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
    case 11:
      if (SERVO1_RELAY) {   // если реле
        channelStatesServ[7] = (dir == 1) ? 1 : 0;
      } else {
        incr(&servoPosServ[0], dir * 10, 180);
      }
      currentLine = 1;
      break;
    case 12:
      if (SERVO2_RELAY) {   // если реле
        channelStatesServ[8] = (dir == 1) ? 1 : 0;
      } else {
        incr(&servoPosServ[1], dir * 10, 180);
      }
      currentLine = 2;
      break;
    case 13: incr(&channelStatesServ[9], dir);
      driveState = 1;
      currentLine = 3;
      break;
  }
}

void settingsSett(int dir) {
  switch (arrowPos) {
    case 0:
      currentChannel += dir;
      updateSettings();
      currentChannel = constrain(currentChannel, -3, 9);
      if (!serviceFlag && currentChannel == -3) serviceIN();
      if (serviceFlag && currentChannel > -3) serviceOUT();

      analogWrite(DRV_PWM, settings.drvSpeed);
#if (SERVO1_RELAY == 0)
      servo1.setSpeed(settings.srv1_Speed);    // ограничить скорость
      servo1.setAccel(settings.srv1_Acc);      // установить ускорение (разгон и торможение)
#endif
#if (SERVO2_RELAY == 0)
      servo2.setSpeed(settings.srv2_Speed);    // ограничить скорость
      servo2.setAccel(settings.srv2_Acc);      // установить ускорение (разгон и торможение)
#endif

      currentLine = 4;
      break;
    case 1: incr(&settings.backlight, dir);
      //currentLine = 0;
      break;
    case 2: incr(&settings.backlTime, dir * 5, 255);
      if (settings.backlTime == 0) settings.backlTime = 5;
      backlTimer = millis();
      //currentLine = 1;
      break;
    case 3: incr(&settings.drvSpeed, dir * 5, 255);
      //currentLine = 1;
      break;
    case 4: incr(&settings.srv1_Speed, dir * 5, 255);
      //currentLine = 2;
      break;
    case 5: incr(&settings.srv1_Acc, dir * 0.1, 1.0);
      //currentLine = 2;
      break;
    case 6: incr(&settings.srv2_Speed, dir * 5, 255);
      //currentLine = 3;
      break;
    case 7: incr(&settings.srv2_Acc, dir * 0.1, 1.0);
      //currentLine = 3;
      break;
    case 8: setDMY(0, dir); break;
    case 9: setDMY(1, dir); break;
    case 10: setDMY(2, dir); break;
    case 11: incr(&settings.comSensPeriod, dir, 1000); break;
    case 12: incr(&settings.plotMode, dir, 2);
      switch (settings.plotMode) {
        case 0: plotTimeout = 5760;
          break;
        case 1: plotTimeout = 240;
          break;
        case 2: plotTimeout = 4;
          break;
      }
      break;
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

void channelSett(int dir) {
  if (arrowPos > 0) EEPROM_updFlag = true;

  switch (arrowPos) {
    case 0:
      if (currentChannel >= 0) updateEEPROM(currentChannel);
      currentChannel += 1 * dir;
      currentChannel = constrain(currentChannel, -3, 9);
      if (!serviceFlag && currentChannel == -3) serviceIN();
      if (serviceFlag && currentChannel > -3) serviceOUT();
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
        if (currentChannel <= 1 || currentChannel == 9) {  // для ШИМ-реле и привода
          incr(&channels[currentChannel].mode, 1, 4);
        } else if (currentChannel == 2 || currentChannel == 3 ||
                   (currentChannel == 7 && SERVO1_RELAY == 0) ||
                   (currentChannel == 8 && SERVO2_RELAY == 0)) { // для ШИМ и СЕРВО каналов
          incr(&channels[currentChannel].mode, 1, 5);
        } else {                                              // для реле
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
        case 4: PID[curPWMchannel].kI += dir * 0.1;
          currentLine = 1;
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&dawn[curPWMchannel].dur1, dir * 10, 255);
          currentLine = 1;
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
        case 4: PID[curPWMchannel].kD += dir * 0.1;
          currentLine = 1;
          break;
#endif
#if (USE_DAWN == 1)
        case 5:
          incr(&dawn[curPWMchannel].stop, dir, 23);
          currentLine = 2;
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
          currentLine = 2;
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&dawn[curPWMchannel].dur2, dir * 10, 255);
          currentLine = 2;
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
          currentLine = 2;
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&dawn[curPWMchannel].minV, dir * 10, 255);
          currentLine = 3;
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
          currentLine = 3;
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&dawn[curPWMchannel].maxV, dir * 10, 255);
          currentLine = 3;
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
          currentLine = 3;
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
          currentLine = 3;
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
    if (CONTROL_TYPE == 1
        && !(arrowPos == 1 && navDepth == 1)
        && !(arrowPos == 2 && navDepth == 0 && currentChannel >= 0)) {
      controlState = !controlState;
      drawArrow();
    }
    settingsTimer = millis();
#if (USE_CO2 == 1)
    CO2_rst = true;
#endif
    if (backlState) {
      backlTimer = millis();      // сбросить таймаут дисплея
      if (currentChannel >= 0) {
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
    } else {
      backlOn();      // включить дисплей
    }
    if (currentChannel == -2) redrawMainSettings();
  }

  if (enc.isTurn()) {
    settingsTimer = millis();
#if (USE_CO2 == 1)
    CO2_rst = true;
#endif

#if (FAST_TURN == 1)
    byte fastTurn = enc.isFast() ? FAST_TURN_STEP : 1;
#else
    byte fastTurn = 1;
#endif

    if (backlState) {
      backlTimer = millis();      // сбросить таймаут дисплея
      // позиции стрелки для каналов и режимов
      if (!controlState && enc.isRight()) {
        if (currentChannel >= 0) {
          debugPage = 0;
          if (navDepth == 0) {
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
          } else {
            switch (channels[currentChannel].mode) {
              case 0: if (++arrowPos > 7) arrowPos = 7;
                break;
              case 1: if (++arrowPos > 4) arrowPos = 4;
                break;
              case 2: if (++arrowPos > 15) arrowPos = 15;
                break;
              case 3: if (++arrowPos > 5) arrowPos = 5;
                break;
              case 4: if (++arrowPos > 9) arrowPos = 9;
                break;
              case 5: if (++arrowPos > 7) arrowPos = 7;
                break;
            }
          }
        } else if (currentChannel == -1) {
          // позиции стрелки для дебаг
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
        } else if (currentChannel == -2) {
          // позиции стрелки для настроек
          if (++arrowPos > SETTINGS_AMOUNT) arrowPos = SETTINGS_AMOUNT;

        } else if (currentChannel == -3) {
          // позиции стрелки для сервиса
          if (++arrowPos > 13) arrowPos = 13;
          //redrawScreen();
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

#if (CONTROL_TYPE == 0)
      if (enc.isRightH()) {
#elif (CONTROL_TYPE == 1)
      if ((controlState && enc.isRight()) || enc.isRightH()) {
#endif
        debugPage = 0;
        if (currentChannel < 0) {
          if (currentChannel == -2) settingsSett(fastTurn);
          else serviceSett(1);
        } else {
          if (navDepth == 0) {
            channelSett(fastTurn);
          } else {
            chSettingsSett(fastTurn);
          }
          if (thisMode == 0 || thisMode == 2) recalculateTime();
        }
        redrawScreen();

#if (CONTROL_TYPE == 0)
      } else if (enc.isLeftH()) {
#elif (CONTROL_TYPE == 1)
      } else if ((controlState && enc.isLeft()) || enc.isLeftH()) {
#endif
        debugPage = 0;
        if (currentChannel < 0) {
          if (currentChannel == -2) settingsSett(-fastTurn);
          else serviceSett(-1);
        } else {
          if (navDepth == 0) {
            channelSett(-fastTurn);
          } else {
            chSettingsSett(-fastTurn);
          }
          if (thisMode == 0 || thisMode == 2) recalculateTime();
        }
        redrawScreen();
      }
    } else {
      backlOn();      // включить дисплей
    }
    if (currentChannel == -2) redrawMainSettings();
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