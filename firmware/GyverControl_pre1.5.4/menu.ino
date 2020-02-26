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
  *val += incr;
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
    case 14: incr(&settings.comSensPeriod, dir, 1000);
      settingsChanged = true;
      currentLine = 3;
      break;
    case 15: incr(&settings.plotMode, dir, 2);
      settingsChanged = true;
      switch (settings.plotMode) {
        case 0: plotTimeout = 5760;
          break;
        case 1: plotTimeout = 240;
          break;
        case 2: plotTimeout = 4;
          break;
      }
      currentLine = 3;
      break;
  }
}

void settingsSett(int dir) {
  if (arrowPos > 0) settingsChanged = true;
  switch (arrowPos) {
    case 0:
      currentChannel += 1 * dir;
      currentChannel = constrain(currentChannel, -3, 9);
      if (!serviceFlag && currentChannel == -3) serviceIN();
      if (serviceFlag && currentChannel > -3) serviceOUT();
      if (settingsChanged) {
        applySettings();
        updateSettings();
      }
      currentLine = 4;
      break;
    case 1: incr(&settings.backlight, dir);
      currentLine = 0;
      break;
    case 2: incr(&settings.backlTime, dir * 5, 255);
      backlTimer = millis();
      currentLine = 1;
      break;
    case 3: incr(&settings.drvSpeed, dir * 5, 255);
      currentLine = 1;
      break;
    case 4: incr(&settings.srv1_Speed, dir * 5, 255);
      currentLine = 2;
      break;
    case 5: incr(&settings.srv1_Acc, dir * 0.1, 1.0);
      currentLine = 2;
      break;
    case 6: incr(&settings.srv2_Speed, dir * 5, 255);
      currentLine = 3;
      break;
    case 7: incr(&settings.srv2_Acc, dir * 0.1, 1.0);
      currentLine = 3;
      break;
  }
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
          incr(&minAngle[0], 10 * dir, 180);
        } else if (currentChannel == 8) {
          incr(&minAngle[1], 10 * dir, 180);
        }
        currentLine = 3;

      } else {    // привод
        incr(&driveTimeout, dir, 255);
        currentLine = 3;
      }
      break;
    case 5:
      if (curMode == 1) {
        if (currentChannel == 7) {
          incr(&maxAngle[0], 10 * dir, 180);
        } else if (currentChannel == 8) {
          incr(&maxAngle[1], 10 * dir, 180);
        }
      }
      currentLine = 3;
      break;
  }
}

void chSettingsSett(int dir) {
  byte curPWMchannel = channelToPWM(currentChannel);
  if (arrowPos > 0) EEPROM_updFlag = true;
  switch (arrowPos) {
    case 0:
      hms_to_s(); // сохранить время для режимов 0 и 2
      if (dir == 1) {
        if (currentChannel == 2 || currentChannel == 3
            || currentChannel == 7 || currentChannel == 8) {  // для ШИМ и СЕРВО каналов
          incr(&channels[currentChannel].mode, 1, 5);
        } else if (currentChannel == 9) {                     // для привода
          incr(&channels[currentChannel].mode, 1, 4);
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
        case 4: incr(&PID[curPWMchannel].kP, dir * 0.1, 100);
          break;
        case 5: incr(&dawn[curPWMchannel].start, dir, 23);
          break;
      }
      break;
    case 3:
      switch (thisMode) {
        case 0:
          thisM[0] += dir;
          currentLine = 1;
          break;
        case 1:
          if (channels[currentChannel].work < 60)
            incr(&channels[currentChannel].work, 2 * dir, 10000);
          else
            incr(&channels[currentChannel].work, 60 * dir, 10000);
          currentLine = 2;
          break;
        case 2:  // неделька
          bitWrite(channels[currentChannel].week, 1, ( (dir == 1) ? 1 : 0) );
          break;
        case 3:
          incr(&channels[currentChannel].sensor, dir, 5);
          currentLine = 2;
          break;
        case 4: incr(&PID[curPWMchannel].kI, dir * 0.1, 100);
          currentLine = 1;
          break;
        case 5: incr(&dawn[curPWMchannel].dur1, dir * 10, 255);
          currentLine = 1;
          break;
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
          incrInt(&channels[currentChannel].threshold, dir, 255);
          currentLine = 3;
          break;
        case 4: incr(&PID[curPWMchannel].kD, dir * 0.1, 100);
          currentLine = 1;
          break;
        case 5:
          incr(&dawn[curPWMchannel].stop, dir, 23);
          currentLine = 2;
          break;
      }
      break;
    case 5:
      switch (thisMode) {
        case 0: thisH[1] += dir;
          currentLine = 2;
          break;
        case 2:  // неделька
          bitWrite(channels[currentChannel].week, 3, ( (dir == 1) ? 1 : 0) );
          break;
        case 3:
          incrInt(&channels[currentChannel].thresholdMax, dir, 255);
          currentLine = 3;
          break;
        case 4: incr(&PID[curPWMchannel].sensor, dir, 5);
          currentLine = 2;
          break;
        case 5: incr(&dawn[curPWMchannel].dur2, dir * 10, 255);
          currentLine = 2;
          break;
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
        case 4:
          incr(&PID[curPWMchannel].setpoint, dir / 10.0, 255);
          currentLine = 2;
          break;
        case 5: incr(&dawn[curPWMchannel].minV, dir * 10, 255);
          currentLine = 3;
          break;
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
        case 4: incr(&PID[curPWMchannel].dT, dir, 255);
          currentLine = 3;
          break;
        case 5: incr(&dawn[curPWMchannel].maxV, dir * 10, 255);
          currentLine = 3;
          break;
      }
      break;
    case 8:
      switch (thisMode) {
        case 2: // неделька
          bitWrite(channels[currentChannel].week, 6, ( (dir == 1) ? 1 : 0) );
          break;
        case 4: incr(&PID[curPWMchannel].minSignal, dir * 5, 255);
          currentLine = 3;
          break;
      }
      break;
    case 9:
      switch (thisMode) {
        case 2: // неделька
          thisH[0] += dir;
          currentLine = 2;
          break;
        case 4: incr(&PID[curPWMchannel].maxSignal, dir * 5, 255);
          currentLine = 3;
          break;
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
    if (backlState) {
      backlTimer = millis();      // сбросить таймаут дисплея
      if (currentChannel >= 0) {
        if (arrowPos == 2 && navDepth == 0) { // заходим в настройки режима
          navDepth = 1;
          arrowPos = 0;
          s_to_hms();
        }
        if (arrowPos == 1 && navDepth == 1) { // нажатие на Back
          //updateEEPROM(currentChannel);
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
  }

  if (enc.isTurn()) {
    settingsTimer = millis();
    if (backlState) {
      backlTimer = millis();      // сбросить таймаут дисплея
      // позиции стрелки для каналов и режимов
      if (!controlState && enc.isRight()) {
        if (currentChannel >= 0) {
          debugPage = 0;
          if (navDepth == 0) {
            if (channels[currentChannel].state) {
              if (currentChannel > 6 && currentChannel < 9) {
                if (++arrowPos > 5) arrowPos = 5;
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
          if (++arrowPos > 7) arrowPos = 7;

        } else if (currentChannel == -3) {
          // позиции стрелки для сервиса
          if (++arrowPos > 15) arrowPos = 15;
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

      thisMode = channels[currentChannel].mode;

#if (CONTROL_TYPE == 0)
      if (enc.isRightH()) {
#elif (CONTROL_TYPE == 1)
      if ((controlState && enc.isRight()) || enc.isRightH()) {
#endif
        debugPage = 0;
        if (currentChannel < 0) {
          if (currentChannel == -2) settingsSett(1);
          else serviceSett(1);
        } else {
          if (navDepth == 0) {
            channelSett(1);
          } else {
            chSettingsSett(1);
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
          if (currentChannel == -2) settingsSett(-1);
          else serviceSett(-1);
        } else {
          if (navDepth == 0) {
            channelSett(-1);
          } else {
            chSettingsSett(-1);
          }
          if (thisMode == 0 || thisMode == 2) recalculateTime();
        }
        redrawScreen();
      }
    } else {
      backlOn();      // включить дисплей
    }
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

void applySettings() {
  analogWrite(DRV_PWM, settings.drvSpeed);
#if (SERVO1_RELAY == 0)
  servo1.setSpeed(settings.srv1_Speed);    // ограничить скорость
  servo1.setAccel(settings.srv1_Acc);      // установить ускорение (разгон и торможение)
#endif
#if (SERVO2_RELAY == 0)
  servo2.setSpeed(settings.srv2_Speed);    // ограничить скорость
  servo2.setAccel(settings.srv2_Acc);      // установить ускорение (разгон и торможение)
#endif
}

// перевод секунд в ЧЧ:ММ:СС
void s_to_hms() {
  uint32_t period = 0;
  if (channels[currentChannel].mode == 0)
    period = channels[currentChannel].period;
  else if (channels[currentChannel].mode == 2)
    period = channels[currentChannel].periodW;

  thisH[0] = floor((long)period / 3600);    // секунды в часы
  thisM[0] = floor((period - (long)thisH[0] * 3600) / 60);
  thisS[0] = period - (long)thisH[0] * 3600 - thisM[0] * 60;

  if (channels[currentChannel].mode == 0)
    period = channels[currentChannel].work;
  else if (channels[currentChannel].mode == 2)
    period = channels[currentChannel].workW;
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
    channels[currentChannel].periodW = ((long)thisH[0] * 3600 + thisM[0] * 60 + thisS[0]);
    channels[currentChannel].workW = ((long)thisH[1] * 3600 + thisM[1] * 60 + thisS[1]);
  }
}
