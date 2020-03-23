void printDash() {
  lcd.print('-');
}

void printSpace() {
  lcd.print("  ");
}

void printColon() {
  lcd.print(':');
}

void spaceColon() {
  lcd.print(F(": "));
}

void clearLine() {
  for (byte i = 0; i < 19; i++) {
    lcd.print((' '));
  }
}
void clearLine2(byte row) {
  lcd.setCursor(0, row);
  for (byte i = 0; i < 20; i++) {
    lcd.print((' '));
  }
}
void printOff() {
  lcd.print(F("Off"));
}
void printOn() {
  lcd.print(F("On"));
}

void drawPlot(int *plot_array) {
#if (USE_PLOTS == 1)
  float max_value = -32000.0;
  float min_value = 32000.0;

  for (byte i = 0; i < 15; i++) {
    float thisVal = plot_array[i] / 7.0;
    if (thisVal > max_value) max_value = thisVal;
    if (thisVal < min_value) min_value = thisVal;
  }

  lcd.setCursor(0, 1); lcd.print(max_value, 1);
  lcd.setCursor(0, 2); lcd.print('>'); lcd.print(plot_array[14] / 7.0, 1);
  lcd.setCursor(0, 3); lcd.print(min_value, 1);

  for (byte i = 0; i < 15; i++) {                  // каждый столбец параметров
    float thisVal = plot_array[i] / 7.0;
    int fill_val = thisVal;
    fill_val = constrain(fill_val, min_value, max_value);
    byte infill, fract;
    // найти количество целых блоков с учётом минимума и максимума для отображения на графике
    if (thisVal > min_value)
      infill = floor((float)(thisVal - min_value) / (max_value - min_value) * 4 * 10);
    else infill = 0;
    fract = (float)(infill % 10) * 8 / 10;                   // найти количество оставшихся полосок
    infill = infill / 10;

    for (byte n = 0; n < 4; n++) {            // для всех строк графика
      if (n < infill && infill > 0) {         // пока мы ниже уровня
        lcd.setCursor(5 + i, (3 - n));        // заполняем полными ячейками
        lcd.write(0);
      }
      if (n >= infill) {                      // если достигли уровня
        lcd.setCursor(5 + i, (3 - n));
        if (fract > 0) lcd.write(fract);      // заполняем дробные ячейки
        else lcd.write(16);                   // если дробные == 0, заливаем пустой
        for (byte k = n + 1; k < 4; k++) {    // всё что сверху заливаем пустыми
          lcd.setCursor(5 + i, (3 - k));
          lcd.write(16);
        }
        break;
      }
    }
  }
#endif
}

void redrawPlot() {
#if (USE_PLOTS == 1)
  switch (debugPage) {
    case 1: drawPlot(tempDay); break;
    case 2: drawPlot(humDay); break;
    case 3: drawPlot(sensDay_0); break;
    case 4: drawPlot(sensDay_1); break;
    case 5: drawPlot(sensDay_2); break;
    case 6: drawPlot(sensDay_3); break;
  }
#endif
  lcd.setCursor(1, 0);
  lcd.print(sensorNames[debugPage - 1]);
}

void redrawChannels() {
  // вывод иmени канала
  if (currentLine == 0 || currentLine == 4) {
    lcd.setCursor(1, 0);
    lcd.print(channelNames[currentChannel]);
#if (USE_PID_RELAY == 1)
    if (currentChannel <= 3) lcd.write(42); // звёздочка
#else
    if (currentChannel == 2 || currentChannel == 3) lcd.write(42); // звёздочка
#endif
    // вывод состояния
    lcd.setCursor(15, 0);
    if (channels[currentChannel].state) printOn();
    else printOff();
  }

  // вывод настроек
  if (channels[currentChannel].state) {
    byte curMode;
    if (currentChannel < 7) curMode = 0;
    else if (currentChannel < 9) curMode = 1;
    else curMode = 2;
#if (SERVO1_RELAY == 1)
    if (currentChannel == 7) curMode = 0;
#endif
#if (SERVO2_RELAY == 1)
    if (currentChannel == 8) curMode = 0;
#endif

    if (currentLine == 1 || currentLine == 4) {
      lcd.setCursor(1, 1);
      clearLine();
      lcd.setCursor(1, 1);
      if (channels[currentChannel].relayType != 2) {
        lcd.print(settingsNames[curMode * 3 + 0]); spaceColon();
        lcd.print(modeNames[channels[currentChannel].mode]);
        if (channels[currentChannel].global && channels[currentChannel].mode != 2) lcd.print(F("+W"));
      } else {
        lcd.print("---");
      }
    }

    if (currentLine == 2 || currentLine == 4) {
      lcd.setCursor(1, 2);
      clearLine();
      lcd.setCursor(1, 2);
      lcd.print(settingsNames[curMode * 3 + 1]); spaceColon();
      lcd.print(directionNames[channels[currentChannel].direction + curMode * 2]);
    }
    if (currentLine == 3 || currentLine == 4) {
      lcd.setCursor(1, 3);
      clearLine();
      lcd.setCursor(1, 3);
      lcd.print(settingsNames[curMode * 3 + 2]); spaceColon();

      if (curMode == 0) {
        if (channels[currentChannel].mode < 4)
          lcd.print(relayNames[channels[currentChannel].relayType]);
        else
          lcd.print("---");
      }
#if (SERVO1_RELAY == 0 || SERVO2_RELAY == 0)
      if (curMode == 1) {
        lcd.setCursor(11, 3);
        if (currentChannel == 7) {
#if (SERVO1_RELAY == 0)
          lcd.print(settings.minAngle[0]);
          lcd.setCursor(16, 3);
          lcd.print(settings.maxAngle[0]);
#endif
        } else if (currentChannel == 8) {
#if (SERVO2_RELAY == 0)
          lcd.print(settings.minAngle[1]);
          lcd.setCursor(16, 3);
          lcd.print(settings.maxAngle[1]);
#endif
        }
      }
#endif
      if (curMode == 2) {
        //lcd.setCursor(11, 3);
        lcd.print(settings.driveTimeout);
      }
    }
  }
}

void redrawSettings() {
  byte curSetMode = channels[currentChannel].mode;
  byte curPWMchannel = channelToPWM[currentChannel];
  if (curSetMode < 4) {
    if (currentLine == 4) printName();
    if (currentLine == 1 || currentLine == 4) {
      lcd.setCursor(1, 1);
      clearLine();
      lcd.setCursor(1, 1);

      switch (curSetMode) {
        case 0: lcd.print(modeSettingsNames[0]); spaceColon();
          lcd.setCursor(8, 1);
          if (thisH[0] < 10) lcd.print(F("00"));
          if (thisH[0] >= 10 && thisH[0] < 100) lcd.print(0);
          lcd.print((byte)(thisH[0]));
          printColon();
          if (thisM[0] < 10) lcd.print(0);
          lcd.print((byte)(thisM[0]));
          printColon();
          if (thisS[0] < 10) lcd.print(0);
          lcd.print((byte)(thisS[0]));
          break;
        case 1: lcd.print(modeSettingsNames[3]); spaceColon();
          lcd.print((int)(impulsePrds[channels[currentChannel].impulsePrd]));
          if (channels[currentChannel].impulsePrd < 6)
            lcd.print(F("m "));
          else if (channels[currentChannel].impulsePrd < 13)
            lcd.print(F("h "));
          else
            lcd.print(F("d "));
          break;
        case 2:
          lcd.setCursor(0, 1);
          lcd.print(F("days "));
          for (byte i = 0; i < 7; i++) {
            lcd.print(bitRead(channels[currentChannel].week, i));
            lcd.print(" ");
          }
          break;
        case 3: lcd.print(modeSettingsNames[8]); spaceColon();
          if (channels[currentChannel].sensPeriod < 60) {
            lcd.print(channels[currentChannel].sensPeriod);
            lcd.print(F("s "));
          } else {
            lcd.print((float)channels[currentChannel].sensPeriod / 60, 1);
            lcd.print(F("m "));
          }
          break;
      }
    }

    if (currentLine == 2 || currentLine == 4) {
      lcd.setCursor(1, 2);
      clearLine();
      lcd.setCursor(1, 2);

      switch (curSetMode) {
        case 0: lcd.print(modeSettingsNames[1]); spaceColon();
          lcd.setCursor(9, 2);
          if (thisH[1] < 10) lcd.print(0);
          lcd.print((byte)(thisH[1]));
          printColon();
          if (thisM[1] < 10) lcd.print(0);
          lcd.print((byte)(thisM[1]));
          printColon();
          if (thisS[1] < 10) lcd.print(0);
          lcd.print((byte)(thisS[1]));
          break;
        case 1: lcd.print(modeSettingsNames[4]); spaceColon();
          if (channels[currentChannel].work < 60) {
            lcd.print(channels[currentChannel].work);
            lcd.print(F("s "));
          } else {
            lcd.print(channels[currentChannel].work / 60);
            lcd.print(F("m "));
          }
          break;
        case 2:
          lcd.setCursor(0, 2);
          printOn();
          lcd.setCursor(4, 2);
          if (thisH[0] < 10) lcd.print(" ");
          lcd.print((byte)(thisH[0]));
          printColon();
          if (thisM[0] < 10) lcd.print(0);
          lcd.print((byte)(thisM[0]));
          printColon();
          if (thisS[0] < 10) lcd.print(0);
          lcd.print((byte)(thisS[0]));
          break;
        case 3: lcd.print(modeSettingsNames[9]); spaceColon();
          lcd.print(sensorNames[channels[currentChannel].sensor]);
          lcd.print(F(" "));
          lcd.print((int)(sensorVals[channels[currentChannel].sensor]));
          break;
      }
    }

    if (currentLine == 3 || currentLine == 4) {
      lcd.setCursor(1, 3);
      clearLine();
      lcd.setCursor(1, 3);
      switch (curSetMode) {
        case 0:
          lcd.print(modeSettingsNames[2]); spaceColon();
          if (channels[currentChannel].period > 0) {
            long period = channels[currentChannel].period - (millis() - timerMillis[currentChannel]) / 1000L;
            byte leftH = floor((long)period / 3600);    // секунды в часы
            byte leftM = floor((period - (long)leftH * 3600) / 60);
            byte leftS = period - (long)leftH * 3600 - leftM * 60;

            lcd.print(leftH);
            printColon();
            lcd.print(leftM);
            printColon();
            lcd.print(leftS);
          }
          break;
        case 1:
          lcd.print(modeSettingsNames[5]); spaceColon();
          lcd.print(channels[currentChannel].startHour);
          lcd.print(F(" h "));
          break;
        case 2:
          lcd.setCursor(0, 3);
          printOff();
          lcd.setCursor(4, 3);
          if (thisH[1] < 10) lcd.print(' ');
          lcd.print((byte)(thisH[1]));
          printColon();
          if (thisM[1] < 10) lcd.print(0);
          lcd.print((byte)(thisM[1]));
          printColon();
          if (thisS[1] < 10) lcd.print(0);
          lcd.print((byte)(thisS[1]));

          lcd.setCursor(13, 3);
          lcd.print(F("Glob"));
          lcd.setCursor(18, 3);
          lcd.print(channels[currentChannel].global);
          break;
        case 3:
          lcd.print("minV:");
          lcd.print(channels[currentChannel].threshold);
          lcd.setCursor(11, 3);
          lcd.print("maxV:");
          lcd.print(channels[currentChannel].thresholdMax);
          break;
      }
    }
  } else {    // для пид и рассвет
    currentLine = 0;
    byte thisAmount;  // количество строк
    if (curSetMode == 4) thisAmount = PID_SET_AMOUNT;
    else if (curSetMode == 5) thisAmount = DAWN_SET_AMOUNT;

    int8_t screenPos = (arrowPos - 1) / 4;
    if (lastScreen != screenPos) lcd.clear();
    lastScreen = screenPos;

    if (screenPos < 1) printName();
    if (arrowPos == 0) arrow(0, 0);
    else space(0, 0);
    if (arrowPos == 1) arrow(14, 0);
    else space(14, 0);

    for (byte i = 0; i < 4; i++) {  // для всех строк
      if (arrowPos <= 1 && i == 0) continue;

      lcd.setCursor(0, i);          // курсор в начало
      int8_t index = 4 * screenPos + i - 1; // минус для
      smartArrow(arrowPos == 4 * screenPos + i + 1); // если курсор находится на выбранной строке
      if (index >= thisAmount) break; // если пункты меню закончились, покидаем цикл for

      // выводим имя и значение пункта меню
      if (index >= 0) {
        if (curSetMode == 4) {
#if (USE_PID == 1)
          lcd.print(pidNames[index]); spaceColon();
          byte driveOffset = 1;
          if (currentChannel == 9) driveOffset = 100;
          switch (index) {
            case 0: lcd.print(PID[curPWMchannel].kP, 2); break;
            case 1: lcd.print(PID[curPWMchannel].kI, 2); break;
            case 2: lcd.print(PID[curPWMchannel].kD, 2); break;
            case 3: lcd.print(sensorNames[PID[curPWMchannel].sensor]); break;
            case 4: lcd.print(PID[curPWMchannel].setpoint, 1); break;
            case 5: lcd.print(PID[curPWMchannel].dT); break;
            case 6: lcd.print(((float)PID[curPWMchannel].minSignal / driveOffset), 1); break;
            case 7: lcd.print(((float)PID[curPWMchannel].maxSignal / driveOffset), 1); break;
          }
#endif
        }
        if (curSetMode == 5) {
#if (USE_DAWN == 1)
          lcd.print(dawnNames[index]); spaceColon();
          switch (index) {
            case 0: lcd.print(dawn[curPWMchannel].start); lcd.print('h'); break;
            case 1: lcd.print(dawn[curPWMchannel].dur1); lcd.print('m'); break;
            case 2: lcd.print(dawn[curPWMchannel].stop); lcd.print('h'); break;
            case 3: lcd.print(dawn[curPWMchannel].dur2); lcd.print('m'); break;
            case 4: lcd.print(dawn[curPWMchannel].minV); break;
            case 5: lcd.print(dawn[curPWMchannel].maxV); break;
          }
#endif
        }
        printSpace();
      }
    }
  }
}

void printName() {
  lcd.setCursor(1, 0);
  lcd.print(modeNames[channels[currentChannel].mode]);
  if (channels[currentChannel].global && channels[currentChannel].mode != 2) lcd.print(F("+W"));
  lcd.setCursor(15, 0); lcd.print(F("Back"));
}

// позиции вывода первых двух датчиков
#define POS_1 4
#define POS_2 9

#if (USE_CO2 == 1)
#if (CO2_PIN == 2)
#if (DALLAS_SENS1 == 1)
#define POS_1 3
#define POS_2 8
#else
#define POS_1 4
#define POS_2 8
#endif
#endif
#endif

void redrawDebug() {
  lcd.setCursor(1, 0); lcd.print(F("DEBUG"));
  lcd.setCursor(7, 0); lcd.print(F("S1:"));
  if (channels[7].state) {

#if (SERVO1_RELAY == 0)
    if (channels[7].mode >= 4) {
      lcd.print(pwmVal[4]); printSpace();
    } else {
      if (channelStates[7]) lcd.print(F("Max"));
      else lcd.print(F("Min"));
    }
#else
    lcd.print(channelStates[7]);
#endif

  }
  else printDash();

  lcd.setCursor(14, 0); lcd.print(F("S2:"));
  if (channels[8].state) {

#if (SERVO2_RELAY == 0)
    if (channels[8].mode >= 4) {
      lcd.print(pwmVal[5]); printSpace();
    } else {
      if (channelStates[8]) lcd.print(F("Max"));
      else lcd.print(F("Min"));
    }
#else
    lcd.print(channelStates[8]);
#endif
  }
  else printDash();

  // 1 строка, датчики
  // 1 влажн
  //clearLine2(1);
  lcd.setCursor(0, 1); lcd.print((int)(sensorVals[1])); lcd.write(37); printSpace();

  lcd.setCursor(POS_1, 1);
#if (DALLAS_SENS1 == 1)
  // 2 даллас
  lcd.print(sensorVals[2], 1);
#else
  // 2 аналог
  lcd.print((int)sensorVals[2]);
#endif

#if (THERM1 == 1 || DALLAS_SENS1 == 1)
  lcd.write(223);
#endif
  printSpace();

  // 3 dht
  lcd.setCursor(POS_2, 1);

#if (DHT_SENS2 == 1)
  printDash();
#else
  // 3 аналог или термистор
  lcd.print((int)sensorVals[3]);
#endif

#if (THERM2 == 1)
  lcd.write(223);
#endif
  printSpace();
  // 4 аналог или термистор
  lcd.setCursor(13, 1); lcd.print((int)sensorVals[4]);
#if (THERM3 == 1)
  lcd.write(223);
#endif
  printSpace();
  // 5 аналог или термистор
  lcd.setCursor(17, 1); lcd.print((int)sensorVals[5]);
#if (THERM4 == 1)
  lcd.write(223);
  if ((int)sensorVals[5] < 10) lcd.print(' ');
#else
  if ((int)sensorVals[5] < 10) printSpace();
  else if ((int)sensorVals[5] < 100) lcd.print(' ');
#endif

  lcd.setCursor(0, 2); lcd.print(sensorVals[0], 1); lcd.write(223); printSpace();
  lcd.setCursor(6, 2); lcd.print(F("R:"));
  for (byte i = 0; i < 7; i++) {
    if (channels[i].state) {
#if (USE_PID == 1 || USE_DAWN == 1)
      if (channels[i].mode == 4 || channels[i].mode == 5) {
        byte thisHeight;
        thisHeight = round(pwmVal[i] / 32); // перевод 0-255 в 0-8

        if (thisHeight == 0) lcd.write(32);   // пустой
        else if (thisHeight < 8) lcd.write(thisHeight + 1); // половинки
        else lcd.write(0);          // полный

      } else
#endif
      {
        lcd.print((bool)channelStates[i] ^ !channels[i].direction);
      }
    }
    else printDash();
  }

  lcd.setCursor(15, 2); lcd.print(F("D:"));
  if (channels[9].state) {
    if (channels[9].mode != 4) lcd.print(lastDriveState);
    else lcd.print(pwmVal[6]);    // выводим текущую величину задержки по ПИД
  }
  else printDash();

  lcd.setCursor(0, 3);
  if (realTime[0] < 10) lcd.print(F(" "));
  lcd.print((byte)(realTime[0])); printColon();
  if (realTime[1] < 10) lcd.print(0);
  lcd.print((byte)(realTime[1])); printColon();
  if (realTime[2] < 10) lcd.print(0);
  lcd.print((byte)(realTime[2]));
  lcd.setCursor(9, 3);
  byte today = rtc.getDay();
  if (today == 0) today = 7;
  lcd.print(today);
  lcd.setCursor(12, 3); lcd.print(F("U:")); lcd.print(uptime);
}

void redrawMainSettings() {
  currentLine = 0;
  byte screenPos = arrowPos / 4;
  if (lastScreen != screenPos) lcd.clear();
  lastScreen = screenPos;

  for (byte i = 0; i < 4; i++) {  // для всех строк
    int8_t index = 4 * screenPos + i - 1; // минус для SETTINGS
    lcd.setCursor(0, i);          // курсор в начало
    smartArrow(arrowPos == 4 * screenPos + i); // если курсор находится на выбранной строке

    if (screenPos == 0 && i == 0) lcd.print(F("SETTINGS"));

    // если пункты меню закончились, покидаем цикл for
    if (index >= SETTINGS_AMOUNT) break;

    // выводим имя и значение пункта меню
    if (index >= 0) {
      lcd.print(settingsPageNames[index]);
      spaceColon();
      switch (index) {
        case 0: if (settings.backlight) printOn();
          else printOff(); break;
        case 1: lcd.print(settings.backlTime); break;
        case 2: lcd.print(settings.drvSpeed); break;
        case 3: lcd.print(rtc.getDate()); break;
        case 4: lcd.print(rtc.getMonth()); break;
        case 5: lcd.print(rtc.getYear()); break;
        case 6: lcd.print(settings.comSensPeriod); break;
        case 7: switch (settings.plotMode) {
            case 0: lcd.print("DAY");
              break;
            case 1: lcd.print("HR ");
              break;
            case 2: lcd.print("MIN");
              break;
          } break;
#if (SMOOTH_SERVO == 1)
        case 8: lcd.print(settings.srv1_Speed); break;
        case 9: lcd.print(settings.srv1_Acc, 1); break;
        case 10: lcd.print(settings.srv2_Speed); break;
        case 11: lcd.print(settings.srv2_Acc, 1); break;
#endif
      }
      printSpace();
    }
  }
}

#if (SCHEDULE_NUM > 0)
void redrawSchedule() {
  currentLine = 0;
  int8_t schChannel = currentChannel - 10;
  byte screenPos = arrowPos / 4;
  if (lastScreen != screenPos) lcd.clear();
  lastScreen = screenPos;

  for (byte i = 0; i < 4; i++) {  // для всех строк
    int8_t index = 4 * screenPos + i - 1; // минус
    // если пункты меню закончились, покидаем цикл for

    if (index >= 6 + schedule[schChannel].pointAmount) break;
    lcd.setCursor(0, i);          // курсор в начало
    smartArrow(arrowPos == 4 * screenPos + i); // если курсор находится на выбранной строке
    if (screenPos == 0 && i == 0) {
      lcd.print(F("SCHEDULE "));
      lcd.print(schChannel + 1);
    }

    // выводим имя и значение пункта меню
    if (index >= 0 && index < 4) {
      lcd.print(schedulePageNames[index]);
      spaceColon();
      switch (index) {
        case 0:
          switch (schedule[schChannel].pidChannel) {
            case 0: printOff(); break;
            case 1:
            case 2:
            case 3:
            case 4: lcd.print(schedule[schChannel].pidChannel); break;
            case 5: lcd.print("S1"); break;
            case 6: lcd.print("S2"); break;
            case 7: lcd.print("Dr"); break;
          }
          break;
        case 1: if (schedule[schChannel].startDay > 0) printDate(schedule[schChannel].startDay - 1);
          else lcd.print("every");
          printSpace();
          break;
        case 2: if (schedule[schChannel].startDay > 0) printDate(schedule[schChannel].endDay - 1);
          else printDash();
          break;
        case 3: lcd.print(schedule[schChannel].pointAmount); break;
      }
    } else if (index >= 4) {
      index -= 4;
      if (index < schedule[schChannel].pointAmount) {
        lcd.print(F("Point "));
        lcd.print(index + 1);
        spaceColon();
        lcd.print(schedule[schChannel].setpoints[index] / 10.0f, 1);
      }
    }
    printSpace();
  }
}

void printDate(int day365) {
  int curDay = 0;
  byte month, day;
  for (byte i = 0; i < 12; i++) {
    curDay += daysMonth[i];
    if (curDay > day365) {
      curDay -= daysMonth[i];
      month = i + 1;
      break;
    }
  }
  day = day365 - curDay + 1;
  lcd.print(day);
  lcd.print('.');
  lcd.print(month);
}
#endif

void redrawService() {
  if (currentLine == 4) lcd.clear();

  if (currentLine == 0 || currentLine == 4) {
    clearLine2(0);
    lcd.setCursor(1, 0);
#if (START_MENU == 0)
    lcd.print(F("SERVICE"));
#else
    lcd.print(F("Back"));
#endif

    lcd.setCursor(12, 0);
    if (realTime[0] < 10) lcd.print(' ');
    lcd.print((byte)(realTime[0])); printColon();
    if (realTime[1] < 10) lcd.print(0);
    lcd.print((byte)(realTime[1])); printColon();
    if (realTime[2] < 10) lcd.print(0);
    lcd.print((byte)(realTime[2]));
  }

  if (currentLine == 1 || currentLine == 4) {
    clearLine2(1);
    lcd.setCursor(0, 1); lcd.print(F("R1 2 3 4 5 6 7"));
  }
  if (currentLine == 2 || currentLine == 4) {
    clearLine2(2);
    lcd.setCursor(1, 2); for (byte i = 0; i < 7; i++) {
      lcd.print(channelStatesServ[i] ^ !channels[i].direction);
      lcd.print(' ');
    }
  }
  if (currentLine == 3 || currentLine == 4) {
    clearLine2(3);
    lcd.setCursor(0, 3); lcd.print(F("D:"));
    lcd.setCursor(2, 3);
    if (channelStatesServ[9]) lcd.print(F("OPEN"));
    else lcd.print(F("CLOS"));

    lcd.setCursor(7, 3); lcd.print(F("S1:"));
    if (SERVO1_RELAY) {   // если реле
      lcd.print(channelStatesServ[7]);
    } else {
      lcd.print((byte)(servoPosServ[0]));
    }

    lcd.setCursor(14, 3); lcd.print(F("S2:"));
    if (SERVO2_RELAY) {   // если реле
      lcd.print(channelStatesServ[8]);
    } else {
      lcd.print((byte)(servoPosServ[1]));
    }
  }
}

void redrawScreen() {
  if (currentLine == 4) {
    lcd.clear();
  }

  if (navDepth == 0) {            // корень mеню
    if (currentChannel == -3) redrawService();
    else if (currentChannel == -2) redrawMainSettings();
    else if (currentChannel == -1) redrawDebug();
    else if (currentChannel < 10) redrawChannels();
#if (SCHEDULE_NUM > 0)
    else redrawSchedule();
#endif
  } else {
    redrawSettings();
  }
  drawArrow();
}
