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

void drawDebug() {
  lcd.clear();
  if (debugPage == 0) redrawDebug();
#if (PID_AUTOTUNE == 1)
  else if (debugPage == 1) redrawTuner();
#endif
  else redrawPlot();
}

#define MAX_CH_POS (9+SCHEDULE_NUM)

#define MIN_CH_POS -3
#if (START_MENU == 1)
#define MIN_CH_POS -2
#endif

void changeChannel(int dir) {
  static int8_t lastChannel = -1;
#if (START_MENU == 1)
  if (!startService)
#endif
    currentChannel += dir;
  currentChannel = constrain(currentChannel, MIN_CH_POS, MAX_CH_POS);

  if (lastChannel >= 0 && lastChannel < 10) {
    EEPROM.put(lastChannel * EEPR_CH_STEP, setChannel);
#if (USE_PID == 1)
    if (setChannel.mode == 4) EEPROM.put(channelToPWM[lastChannel] * EEPR_PID_STEP + EEPR_PID, setPID);
#endif
#if (USE_DAWN == 1)
    if (setChannel.mode == 5) EEPROM.put(channelToPWM[lastChannel] * EEPR_DAWN_STEP + EEPR_DAWN, setDawn);
#endif
  }

  if (currentChannel >= 0 && currentChannel < 10) {
    EEPROM.get(currentChannel * EEPR_CH_STEP, setChannel);
#if (USE_PID == 1)
    if (setChannel.mode == 4) EEPROM.get(channelToPWM[currentChannel] * EEPR_PID_STEP + EEPR_PID, setPID);
#endif
#if (USE_DAWN == 1)
    if (setChannel.mode == 5) EEPROM.get(channelToPWM[currentChannel] * EEPR_DAWN_STEP + EEPR_DAWN, setDawn);
#endif
  }

#if (SCHEDULE_NUM > 0)
  if (lastChannel >= 10) EEPROM.put((lastChannel - 10) * EEPR_SHED_STEP + EEPR_SHED, setSchedule);
  if (currentChannel >= 10) EEPROM.get((currentChannel - 10) * EEPR_SHED_STEP + EEPR_SHED, setSchedule);
#endif

  currentLine = 4;
  lastChannel = currentChannel;
}

#if (SCHEDULE_NUM > 0)
void scheduleSett(int dir) {
  bool flag = 0;
  switch (arrowPos) {
    case 0:
      changeChannel(dir);
      break;
    case 1: incr(&setSchedule.pidChannel, dir, 7);
      flag = 1;
      break;
    case 2: incr(&setSchedule.startDay, dir, 365); break;
    case 3: incr(&setSchedule.endDay, dir, 365); break;
    case 4: incr(&setSchedule.pointAmount, dir, SCHEDULE_MAX); break;
  }
  if (arrowPos > 4) {
    incr(&setSchedule.setpoints[arrowPos - 5], dir, 2550);
  }
  if (flag) {
    int thisVal = sensorVals[loadPID(setSchedule.pidChannel - 1).sensor] * 10;
    for (byte i = 0; i < SCHEDULE_MAX; i++) setSchedule.setpoints[i] = thisVal;
  }
}
#endif


void serviceSett(int dir) {
  switch (arrowPos) {
    case 0: changeChannel(dir);
      if (serviceFlag && currentChannel > -3) serviceOUT();
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
      changeChannel(dir);
      EEPROM.put(EEPR_SETTINGS, settings);
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
      break;
#if (SMOOTH_SERVO == 1)
    case 9: incr(&settings.srv1_Speed, dir * 5, 255); break;
    case 10: incr(&settings.srv1_Acc, dir * 0.1, 1.0); break;
    case 11: incr(&settings.srv2_Speed, dir * 5, 255); break;
    case 12: incr(&settings.srv2_Acc, dir * 0.1, 1.0); break;
#endif
#if (PID_AUTOTUNE == 1)
    case 9: incr(&tunerSettings.tuner, dir);
      tunerSettings.restart = true;
      break;
    case 10: incr(&tunerSettings.result, dir);
      tuner.P = 0;
      tuner.I = 0;
      tuner.D = 0;
      break;
    case 11: incr(&tunerSettings.channel, dir, 7); break;
    case 12: incr(&tunerSettings.sensor, dir, 6); break;
    case 13: incr(&tunerSettings.manual, dir); break;
    case 14: incr(&tunerSettings.steady, dir, 255); break;
    case 15: incr(&tunerSettings.step, dir, 255); break;
    case 16: incr(&tunerSettings.window, dir * 0.01, 5); break;
    case 17: incr(&tunerSettings.kickTime, dir, 60); break;
    case 18: incr(&tunerSettings.delay, dir, 60); break;
    case 19: incr(&tunerSettings.period, dir, 60); break;
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
  switch (arrowPos) {
    case 0:
      changeChannel(dir);
      break;
    case 1:
      incr(&setChannel.state, dir);
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
      incr(&setChannel.direction, dir);
      currentLine = 2;
      break;
    case 4:
      if (curMode == 0) {   // реле
        incr(&setChannel.relayType, dir, 2);
        currentLine = 4;

      } else if (curMode == 1) {  // серво
        if (currentChannel == 7) {
          incr(&settings.minAngle[0], 5 * dir, 180);
        } else if (currentChannel == 8) {
          incr(&settings.minAngle[1], 5 * dir, 180);
        }
        currentLine = 3;

      } else {    // привод
        incr(&settings.driveTimeout, dir, 30000);
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
  //if (arrowPos > 0) EEPROM_updFlag = true;
  switch (arrowPos) {
    case 0:
      hms_to_s(); // сохранить время для режимов 0 и 2
      if (dir == 1) {
        if ((currentChannel <= 1 && USE_PID_RELAY == 1) || currentChannel == 9) {  // для ШИМ-реле и привода
          incr(&setChannel.mode, 1, 4);
        } else if (currentChannel == 2 || currentChannel == 3 ||
                   (currentChannel == 7 && SERVO1_RELAY == 0) ||
                   (currentChannel == 8 && SERVO2_RELAY == 0)) { // для ШИМ и СЕРВО каналов
          incr(&setChannel.mode, 1, 5);
        } else {                                              // для обычных реле
          incr(&setChannel.mode, 1, 3);
        }
      } else {
        incr(&setChannel.mode, -1, 5);
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
        case 1: incr(&setChannel.impulsePrd, dir, 19);
          break;
        case 2: // неделька
          bitWrite(setChannel.week, 0, ( (dir == 1) ? 1 : 0) );
          break;
        case 3:
          if (setChannel.sensPeriod < 60) {
            incr(&setChannel.sensPeriod, dir * 2, 32000);
          } else {
            incr(&setChannel.sensPeriod, dir * 60, 32000);
          }
          break;
#if (USE_PID == 1)
        case 4: setPID.kP += dir * 0.1;
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&setDawn.start, dir, 23);
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
          if (setChannel.work < 10) {
            dir = (dir > 0) ? 1 : -1;
          }
          else if (setChannel.work > 60) dir = dir * 60;

          incr(&setChannel.work, dir, 10000);
          if (setChannel.work == 0) setChannel.work = 1;
          currentLine = 2;
          break;
        case 2:  // неделька
          bitWrite(setChannel.week, 1, ( (dir == 1) ? 1 : 0) );
          break;
        case 3:
          incr(&setChannel.sensor, dir, 5);
          setChannel.threshold = sensorVals[setChannel.sensor];
          setChannel.thresholdMax = sensorVals[setChannel.sensor];
          currentLine = 4;
          break;
#if (USE_PID == 1)
        case 4: setPID.kI += dir * 0.01;
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&setDawn.dur1, dir * 10, 255);
          break;
#endif
      }
      break;
    case 4:
      switch (thisMode) {
        case 0: thisS[0] += dir;
          currentLine = 1;
          break;
        case 1: incr(&setChannel.startHour, dir, 23);
          currentLine = 3;
          break;
        case 2: // неделька
          bitWrite(setChannel.week, 2, ( (dir == 1) ? 1 : 0) );
          break;
        case 3:
          if (setChannel.threshold > 255) dir *= 10;
          incrInt(&setChannel.threshold, dir, 5000);
          currentLine = 3;
          break;
#if (USE_PID == 1)
        case 4: setPID.kD += dir * 0.5;
          break;
#endif
#if (USE_DAWN == 1)
        case 5:
          incr(&setDawn.stop, dir, 23);
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
          bitWrite(setChannel.week, 3, ( (dir == 1) ? 1 : 0) );
          break;
        case 3:
          if (setChannel.thresholdMax > 255) dir *= 10;
          incrInt(&setChannel.thresholdMax, dir, 5000);
          currentLine = 3;
          break;
#if (USE_PID == 1)
        case 4: incr(&setPID.sensor, dir, 5);
          setPID.setpoint = sensorVals[setPID.sensor];
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&setDawn.dur2, dir * 10, 255);
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
          bitWrite(setChannel.week, 4, ( (dir == 1) ? 1 : 0) );
          break;
#if (USE_PID == 1)
        case 4:
          incr(&setPID.setpoint, dir * 0.1, 255);
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&setDawn.minV, dir * 10, 255);
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
          bitWrite(setChannel.week, 5, ( (dir == 1) ? 1 : 0) );
          break;
#if (USE_PID == 1)
        case 4: incr(&setPID.dT, dir, 255);
          break;
#endif
#if (USE_DAWN == 1)
        case 5: incr(&setDawn.maxV, dir * 10, 255);
          break;
#endif
      }
      break;
    case 8:
      switch (thisMode) {
        case 2: // неделька
          bitWrite(setChannel.week, 6, ( (dir == 1) ? 1 : 0) );
          break;
#if (USE_PID == 1)
        case 4: incr(&setPID.minSignal, dir * 5, 255);
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
        case 4: incr(&setPID.maxSignal, dir * 5, 255);
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
      incr(&setChannel.global, dir);
      currentLine = 3;
      break;
  }
}

void controlTick() {
  enc.tick(controlState);  // отработка энкодера

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
    byte fastTurn = enc.isFast() ? (arrowPos == 0 ? 1 : FAST_TURN_STEP) : 1;
#else
    byte fastTurn = 1;
#endif

    // позиции стрелки для каналов и режимов
    if (enc.getState() == 1) {
      if (currentChannel >= 0 && currentChannel < 10) {
        debugPage = 0;
        if (navDepth == 0) {  // настройки канала
          if (setChannel.state) {
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
          switch (setChannel.mode) {
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
#if (USE_PLOTS == 1 || PID_AUTOTUNE == 1)
        debugPage++;
#endif
#if (PID_AUTOTUNE == 1 && USE_PLOTS == 0)
        if (debugPage > 1) debugPage = 1;
#endif
#if (USE_PLOTS == 1)
        if (debugPage > 6 + PID_AUTOTUNE) debugPage = 6 + PID_AUTOTUNE;
#endif
        drawDebug();
      } else if (currentChannel == -2) {  // позиции стрелки для настроек
        incr(&arrowPos, 1, SETTINGS_AMOUNT);

      } else if (currentChannel == -3) {  // позиции стрелки для сервиса
        incr(&arrowPos, 1, 13);

      } else if (currentChannel >= 10) {
#if (SCHEDULE_NUM > 0)
        incr(&arrowPos, 1, (4 + setSchedule.pointAmount));
#endif
      }
      drawArrow();

    }
    if (enc.getState() == 2) {
      if (--arrowPos < 0) arrowPos = 0;
      if (currentChannel == -1) {
        if (--debugPage < 0) debugPage = 0;
        drawDebug();
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
    thisMode = setChannel.mode;

    // ============ изменение
    int8_t changed = 0;

#if (CONTROL_TYPE == 0)
    if (enc.getState() == 3) {
#elif (CONTROL_TYPE == 1)
    if (enc.getState() == 3) {
#endif
      changed = 1;

#if (CONTROL_TYPE == 0)
    } else if (enc.getState() == 4) {
#elif (CONTROL_TYPE == 1)
    } else if (enc.getState() == 4) {
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
    enc.rst();
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
  if (enc.isHolded()) {
    manualControl = true;
    driveState = 1;
    manualPos = !manualPos;
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

  if (setChannel.mode == 0)
    period = setChannel.period;
  else if (setChannel.mode == 2)
    period = setChannel.weekOn;

  thisH[0] = floor((long)period / 3600);    // секунды в часы
  thisM[0] = floor((period - (long)thisH[0] * 3600) / 60);
  thisS[0] = period - (long)thisH[0] * 3600 - thisM[0] * 60;

  if (setChannel.mode == 0)
    period = setChannel.work;
  else if (setChannel.mode == 2)
    period = setChannel.weekOff;

  thisH[1] = floor((long)period / 3600);    // секунды в часы
  thisM[1] = floor((period - (long)thisH[1] * 3600) / 60);
  thisS[1] = period - (long)thisH[1] * 3600 - thisM[1] * 60;
}

// перевод ЧЧ:ММ:СС в секунды
void hms_to_s() {
  if (setChannel.mode == 0) {
    setChannel.period = ((long)thisH[0] * 3600 + thisM[0] * 60 + thisS[0]);
    setChannel.work = ((long)thisH[1] * 3600 + thisM[1] * 60 + thisS[1]);
  }
  if (setChannel.mode == 2) {
    setChannel.weekOn = ((long)thisH[0] * 3600 + thisM[0] * 60 + thisS[0]);
    setChannel.weekOff = ((long)thisH[1] * 3600 + thisM[1] * 60 + thisS[1]);
  }
}
