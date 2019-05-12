byte thisMode;
byte curMode;

// меняем при повороте вправо, настройки каналов
void rightHdepth0() {
  switch (arrowPos) {
    case 0:
      if (currentChannel >= 0) updateEEPROM(currentChannel);
      if (++currentChannel > 9) currentChannel = 9;
      if (serviceFlag && currentChannel > -3) serviceOUT();
      currentLine = 4;
      break;
    case 1: channels[currentChannel].state = true;
      if (currentChannel == 7)
        servo1.attach(SERVO_0, 600, 2400);
      if (currentChannel == 8)
        servo2.attach(SERVO_1, 600, 2400);
      currentLine = 4;
      break;
    case 2: currentLine = 1;
      break;
    case 3: channels[currentChannel].direction = false;
      currentLine = 2;
      break;
    case 4:
      if (curMode == 0) {
        if (++channels[currentChannel].relayType > 2) channels[currentChannel].relayType = 2;
        currentLine = 4;
      } else if (curMode == 1) {  // серво
        if (currentChannel == 7) {
          minAngle[0] += 10;
          if (minAngle[0] > 180) minAngle[0] = 180;
        } else if (currentChannel == 8) {
          minAngle[1] += 10;
          if (minAngle[1] > 180) minAngle[1] = 180;
        }
        currentLine = 3;
      } else {
        driveTimeout += 1;
        if (driveTimeout > 254) driveTimeout = 255;
        currentLine = 3;
      }
      break;
    case 5:
      if (curMode == 1) {
        if (currentChannel == 7) {
          maxAngle[0] += 10;
          if (maxAngle[0] > 180) maxAngle[0] = 180;
        } else if (currentChannel == 8) {
          maxAngle[1] += 10;
          if (maxAngle[1] > 180) maxAngle[1] = 180;
        }
      }
      currentLine = 3;
      break;
  }
}

// меняем при повороте вправо, настройки режимов
void rightHdepth1() {
  switch (arrowPos) {
    case 0: if (++channels[currentChannel].mode > 3) channels[currentChannel].mode = 3;
      currentLine = 4;
      break;
    case 1: currentLine = 0;
      currentLine = 4;
      break;
    case 2:
      if (thisMode == 0) {   // период
        thisH[0]++;
        if (thisH[0] > 999) thisH[0] = 999;
      } else if (thisMode == 1) {  // импульс
        channels[currentChannel].impulsePrd++;
        if (channels[currentChannel].impulsePrd > 13) channels[currentChannel].impulsePrd = 13;
      } else if (thisMode == 2) {  // сутки
        if (++channels[currentChannel].hour1 > 23) channels[currentChannel].hour1 = 23;
      } else {    // датчик
        if (channels[currentChannel].sensPeriod < 60) {
          channels[currentChannel].sensPeriod += 2;
        } else {
          channels[currentChannel].sensPeriod += 60;
        }
      }
      currentLine = 1;
      break;
    case 3:
      if (thisMode == 0) {   // период
        thisM[0]++;
        currentLine = 1;
      } else if (thisMode == 1) {  // импульс
        channels[currentChannel].work++;
        if (channels[currentChannel].work > 100) channels[currentChannel].work = 100;
        currentLine = 2;
      } else if (thisMode == 2) {  // сутки
        if (++channels[currentChannel].hour2 > 23) channels[currentChannel].hour2 = 23;
        currentLine = 2;
      } else {      // датчик
        if (++channels[currentChannel].sensor > 5) channels[currentChannel].sensor = 5;
        currentLine = 2;
      }
      break;
    case 4:
      if (thisMode == 0) {   // период
        thisS[0]++;
        currentLine = 1;
      } else if (thisMode == 1) {
        if (++channels[currentChannel].startHour > 23) channels[currentChannel].startHour = 23;
        currentLine = 3;
      } else if (thisMode == 3) {
        if (channels[currentChannel].threshold >= 50)
          channels[currentChannel].threshold += 10;
        else
          channels[currentChannel].threshold++;
        if (channels[currentChannel].threshold > 1023) channels[currentChannel].threshold = 1023;
        currentLine = 3;
      }
      break;
    case 5:
      if (thisMode == 0) {   // период
        thisH[1]++;
        currentLine = 2;
      } else if (thisMode == 3) {
        if (channels[currentChannel].thresholdMax >= 50)
          channels[currentChannel].thresholdMax += 10;
        else
          channels[currentChannel].thresholdMax++;
        if (channels[currentChannel].thresholdMax > 1023) channels[currentChannel].thresholdMax = 1023;
        currentLine = 3;
      }
      break;
    case 6:
      if (thisMode == 0) {   // период
        thisM[1]++;
        currentLine = 2;
      }
      break;
    case 7:
      if (thisMode == 0) {   // период
        thisS[1]++;
        currentLine = 2;
      }
      break;
  }
}

// меняем при повороте влево, настройки каналов
void leftHdepth0() {
  switch (arrowPos) {
    case 0:
      if (currentChannel >= 0) updateEEPROM(currentChannel);
      if (--currentChannel < -3) currentChannel = -3;
      if (!serviceFlag && currentChannel == -3) serviceIN();
      if (currentChannel == 7)
        servo1.detach();
      if (currentChannel == 8)
        servo2.detach();
      currentLine = 4;
      break;
    case 1: channels[currentChannel].state = false;
      currentLine = 4;
      break;
    case 2:
      currentLine = 1;
      break;
    case 3: channels[currentChannel].direction = true;
      currentLine = 2;
      break;
    case 4:
      if (curMode == 0) {
        if (--channels[currentChannel].relayType < 0) channels[currentChannel].relayType = 0;
        currentLine = 4;
      } else if (curMode == 1) {  // серво
        if (currentChannel == 7) {
          minAngle[0] -= 10;
          if (minAngle[0] > 180) minAngle[0] = 0;
        } else if (currentChannel == 8) {
          minAngle[1] -= 10;
          if (minAngle[1] > 180) minAngle[1] = 0;
        }
        currentLine = 3;
      } else {
        driveTimeout -= 1;
        if (driveTimeout < 1) driveTimeout = 1;
        currentLine = 3;
      }
      break;
    case 5:
      if (curMode == 1) {
        if (currentChannel == 7) {
          maxAngle[0] -= 10;
          if (maxAngle[0] > 180) maxAngle[0] = 0;
        } else if (currentChannel == 8) {
          maxAngle[1] -= 10;
          if (maxAngle[1] > 180) maxAngle[1] = 0;
        }
      }
      currentLine = 3;
      break;
  }
}

// меняем при повороте влево, настройки режимов
void leftHdepth1() {
  switch (arrowPos) {
    case 0: if (--channels[currentChannel].mode < 0) channels[currentChannel].mode = 0;
      currentLine = 4;
      break;
    case 1: currentLine = 4;
      break;
    case 2:
      if (thisMode == 0) {   // период
        thisH[0]--;
      } else if (thisMode == 1) {  // импульс
        if (--channels[currentChannel].impulsePrd < 0) channels[currentChannel].impulsePrd = 0;
      } else if (thisMode == 2) {      // сутки
        if (--channels[currentChannel].hour1 < 0) channels[currentChannel].hour1 = 0;
      } else {    // датчик
        if (channels[currentChannel].sensPeriod < 60) {
          channels[currentChannel].sensPeriod -= 2;
        } else {
          channels[currentChannel].sensPeriod -= 60;
        }
        if (channels[currentChannel].sensPeriod < 2) channels[currentChannel].sensPeriod = 2;
      }
      currentLine = 1;
      break;
    case 3:
      if (thisMode == 0) {   // период
        thisM[0]--;
        currentLine = 1;
      } else if (thisMode == 1) {  // импульс
        if (--channels[currentChannel].work < 1) channels[currentChannel].work = 1;
        currentLine = 2;
      } else if (thisMode == 2) {      // сутки
        if (--channels[currentChannel].hour2 < 0) channels[currentChannel].hour2 = 0;
        currentLine = 2;
      } else {      // датчик
        if (--channels[currentChannel].sensor < 0) channels[currentChannel].sensor = 0;
        currentLine = 2;
      }
      break;
    case 4:
      if (thisMode == 0) {   // период
        thisS[0]--;
        currentLine = 1;
      } else if (thisMode == 1) {
        if (--channels[currentChannel].startHour < 0) channels[currentChannel].startHour = 0;
        currentLine = 3;
      } else if (thisMode == 3) {
        if (channels[currentChannel].threshold >= 50)
          channels[currentChannel].threshold -= 10;
        else
          channels[currentChannel].threshold--;
        if (channels[currentChannel].threshold < 0) channels[currentChannel].threshold = 0;
        currentLine = 3;
      }
      break;
    case 5:
      if (thisMode == 0) {   // период
        thisH[1]--;
        currentLine = 2;
      } else if (thisMode == 3) {
        if (channels[currentChannel].thresholdMax >= 50)
          channels[currentChannel].thresholdMax -= 10;
        else
          channels[currentChannel].thresholdMax--;
        if (channels[currentChannel].thresholdMax < 0) channels[currentChannel].thresholdMax = 0;
        currentLine = 3;
      }
      break;
    case 6:
      if (thisMode == 0) {   // период
        thisM[1]--;
        currentLine = 2;
      }
      break;
    case 7:
      if (thisMode == 0) {   // период
        thisS[1]--;
        currentLine = 2;
      }
      break;
  }
}

void rightHservice() {
  switch (arrowPos) {
    case 0:
      if (++currentChannel > 9) currentChannel = 9;
      if (serviceFlag && currentChannel > -3) serviceOUT();
      currentLine = 4;
      break;
    case 1: realTime[0]++;
      correctTime();
      currentLine = 0;
      break;
    case 2: realTime[1]++;
      correctTime();
      currentLine = 0;
      break;
    case 3: realTime[2]++;
      correctTime();
      currentLine = 0;
      break;
    case 4: channelStatesServ[0] = true;
      currentLine = 2;
      break;
    case 5: channelStatesServ[1] = true;
      currentLine = 2;
      break;
    case 6: channelStatesServ[2] = true;
      currentLine = 2;
      break;
    case 7: channelStatesServ[3] = true;
      currentLine = 2;
      break;
    case 8: channelStatesServ[4] = true;
      currentLine = 2;
      break;
    case 9: channelStatesServ[5] = true;
      currentLine = 2;
      break;
    case 10: channelStatesServ[6] = true;
      currentLine = 2;
      break;
    case 11: servoPosServ[0] += 10;
      if (servoPosServ[0] > 180) servoPosServ[0] = 180;
      currentLine = 1;
      break;
    case 12: servoPosServ[1] += 10;
      if (servoPosServ[1] > 180) servoPosServ[1] = 180;
      currentLine = 2;
      break;
    case 13: channelStatesServ[9] = true;
      driveState = 1;
      currentLine = 3;
      break;
    case 14: settings.comSensPeriod += 1;
      currentLine = 3;
      break;
    case 15: if (++settings.plotMode >= 2) settings.plotMode = 2;
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

void leftHservice() {
  switch (arrowPos) {
    case 0:
      if (--currentChannel < -3) currentChannel = -3;
      if (!serviceFlag && currentChannel == -3) serviceIN();
      currentLine = 4;
      break;
    case 1: realTime[0]--;
      correctTime();
      currentLine = 0;
      break;
    case 2: realTime[1]--;
      correctTime();
      currentLine = 0;
      break;
    case 3: realTime[2]--;
      correctTime();
      currentLine = 0;
      break;
    case 4: channelStatesServ[0] = false;
      currentLine = 2;
      break;
    case 5: channelStatesServ[1] = false;
      currentLine = 2;
      break;
    case 6: channelStatesServ[2] = false;
      currentLine = 2;
      break;
    case 7: channelStatesServ[3] = false;
      currentLine = 2;
      break;
    case 8: channelStatesServ[4] = false;
      currentLine = 2;
      break;
    case 9: channelStatesServ[5] = false;
      currentLine = 2;
      break;
    case 10: channelStatesServ[6] = false;
      currentLine = 2;
      break;
    case 11:
      if (servoPosServ[0] >= 10) servoPosServ[0] -= 10;
      else servoPosServ[0] = 0;
      currentLine = 1;
      break;
    case 12:
      if (servoPosServ[1] >= 10) servoPosServ[1] -= 10;
      else servoPosServ[1] = 0;
      currentLine = 2;
      break;
    case 13: channelStatesServ[9] = false;
      driveState = 1;
      currentLine = 3;
      break;
    case 14: settings.comSensPeriod -= 1;
      if (settings.comSensPeriod < 1) settings.comSensPeriod = 1;
      currentLine = 3;
      break;
    case 15: if (--settings.plotMode < 0) settings.plotMode = 0;
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

void rightHsettings() {
  if (arrowPos > 0) settingsChanged = true;
  switch (arrowPos) {
    case 0:
      if (++currentChannel > 9) currentChannel = 9;
      if (serviceFlag && currentChannel > -3) serviceOUT();
      if (settingsChanged) {
        applySettings();
        updateSettings();
      }
      currentLine = 4;
      break;
    case 1: settings.backlight = true;
      currentLine = 0;
      break;
    case 2:
      if (settings.backlTime < 250) settings.backlTime += 5;
      else settings.backlTime = 255;
      backlTimer = millis();
      currentLine = 1;
      break;
    case 3:
      if (settings.drvSpeed < 250) settings.drvSpeed += 10;
      else settings.drvSpeed = 255;
      currentLine = 1;
      break;
    case 4:
      if (settings.srv1_Speed < 250) settings.srv1_Speed += 5;
      else settings.srv1_Speed = 255;
      currentLine = 2;
      break;
    case 5:
      settings.srv1_Acc += (float)0.1;
      if (settings.srv1_Acc >= 1) settings.srv1_Acc = 1;
      currentLine = 2;
      break;
    case 6:
      if (settings.srv2_Speed < 250) settings.srv2_Speed += 5;
      else settings.srv2_Speed = 255;
      currentLine = 3;
      break;
    case 7:
      settings.srv2_Acc += (float)0.1;
      if (settings.srv2_Acc >= 1) settings.srv2_Acc = 1;
      currentLine = 3;
      break;
  }
}
void leftHsettings() {
  if (arrowPos > 0) settingsChanged = true;
  switch (arrowPos) {
    case 0:
      if (--currentChannel < -3) currentChannel = -3;
      if (!serviceFlag && currentChannel == -3) serviceIN();
      if (settingsChanged) {
        applySettings();
        updateSettings();
      }
      currentLine = 4;
      break;
    case 1: settings.backlight = false;
      currentLine = 0;
      break;
    case 2:
      if (settings.backlTime > 10) settings.backlTime -= 5;
      else settings.backlTime = 5;
      backlTimer = millis();
      currentLine = 1;
      break;
    case 3:
      if (settings.drvSpeed > 10) settings.drvSpeed -= 10;
      else settings.drvSpeed = 5;
      currentLine = 1;
      break;
    case 4:
      if (settings.srv1_Speed > 10) settings.srv1_Speed -= 5;
      else settings.srv1_Speed = 5;
      currentLine = 2;
      break;
    case 5:
      settings.srv1_Acc -= (float)0.1;
      if (settings.srv1_Acc < 0.1) settings.srv1_Acc = 0.1;
      currentLine = 2;
      break;
    case 6:
      if (settings.srv2_Speed > 10) settings.srv2_Speed -= 5;
      else settings.srv2_Speed = 5;
      currentLine = 3;
      break;
    case 7:
      settings.srv2_Acc -= (float)0.1;
      if (settings.srv2_Acc < 0.1) settings.srv2_Acc = 0.1;
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
    if (CONTROL_TYPE == 1 && !(arrowPos == 1 && navDepth == 1)) {
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
          updateEEPROM(currentChannel);
          navDepth = 0;
          arrowPos = 2;
          if (channels[currentChannel].mode == 0) hms_to_s();
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
              case 2: if (++arrowPos > 3) arrowPos = 3;
                break;
              case 3: if (++arrowPos > 5) arrowPos = 5;
                break;
            }
          }
        } else if (currentChannel == -1) {
          // позиции стрелки для дебаг
          arrowPos = 0;
          if (++debugPage > 6) debugPage = 6;
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
      if (controlState && enc.isRight()) {
#endif
        debugPage = 0;
        if (currentChannel < 0) {
          if (currentChannel == -2) rightHsettings();
          else rightHservice();
        } else {
          if (navDepth == 0) {
            rightHdepth0();
          } else {
            rightHdepth1();
          }
          if (thisMode == 0) recalculateTime();
        }
        redrawScreen();
        
#if (CONTROL_TYPE == 0)
      } else if (enc.isLeftH()) {
#elif (CONTROL_TYPE == 1)
      } else if (controlState && enc.isLeft()) {
#endif
        debugPage = 0;
        if (currentChannel < 0) {
          if (currentChannel == -2) leftHsettings();
          else leftHservice();
        } else {
          if (navDepth == 0) {
            leftHdepth0();
          } else {
            leftHdepth1();
          }
          if (thisMode == 0) recalculateTime();
        }
        redrawScreen();
      }
    } else {
      backlOn();      // включить дисплей
    }
  }
}

void recalculateTime() {
  for (byte i = 0; i < 1; i++) {
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
  servo1.setSpeed(settings.srv1_Speed);    // ограничить скорость
  servo1.setAccel(settings.srv1_Acc);      // установить ускорение (разгон и торможение)
  servo2.setSpeed(settings.srv2_Speed);    // ограничить скорость
  servo2.setAccel(settings.srv2_Acc);      // установить ускорение (разгон и торможение)
}

// перевод секунд в ЧЧ:ММ:СС
void s_to_hms() {
  uint32_t period = channels[currentChannel].period;
  thisH[0] = floor((long)period / 3600);    // секунды в часы
  thisM[0] = floor((period - (long)thisH[0] * 3600) / 60);
  thisS[0] = period - (long)thisH[0] * 3600 - thisM[0] * 60;

  period = channels[currentChannel].work;
  thisH[1] = floor((long)period / 3600);    // секунды в часы
  thisM[1] = floor((period - (long)thisH[1] * 3600) / 60);
  thisS[1] = period - (long)thisH[1] * 3600 - thisM[1] * 60;
}

// перевод ЧЧ:ММ:СС в секунды
void hms_to_s() {
  channels[currentChannel].period = ((long)thisH[0] * 3600 + thisM[0] * 60 + thisS[0]);
  channels[currentChannel].work = ((long)thisH[1] * 3600 + thisM[1] * 60 + thisS[1]);
}
