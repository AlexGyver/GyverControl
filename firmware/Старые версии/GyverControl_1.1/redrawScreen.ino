void drawPlot(byte pos, byte row, byte width, byte height, int min_val, int max_val, int *plot_array/*, String label*/) {
  int max_value = -32000;
  int min_value = 32000;

  for (byte i = 0; i < 15; i++) {
    if (plot_array[i] > max_value) max_value = plot_array[i];
    if (plot_array[i] < min_value) min_value = plot_array[i];
  }
  lcd.setCursor(0, 1); lcd.print(max_value);
  //lcd.setCursor(0, 1); lcd.print(label);
  lcd.setCursor(0, 2); lcd.print(F(">")); lcd.print(plot_array[14]);
  lcd.setCursor(0, 3); lcd.print(min_value);

  for (byte i = 0; i < width; i++) {                  // каждый столбец параметров
    int fill_val = plot_array[i];
    fill_val = constrain(fill_val, min_val, max_val);
    byte infill, fract;
    // найти количество целых блоков с учётом минимума и максимума для отображения на графике
    if (plot_array[i] > min_val)
      infill = floor((float)(plot_array[i] - min_val) / (max_val - min_val) * height * 10);
    else infill = 0;
    fract = (float)(infill % 10) * 8 / 10;                   // найти количество оставшихся полосок
    infill = infill / 10;

    for (byte n = 0; n < height; n++) {     // для всех строк графика
      if (n < infill && infill > 0) {       // пока мы ниже уровня
        lcd.setCursor(pos + i, (row - n));      // заполняем полными ячейками
        lcd.write(0);
      }
      if (n >= infill) {                    // если достигли уровня
        lcd.setCursor(pos + i, (row - n));
        if (fract > 0) lcd.write(fract);          // заполняем дробные ячейки
        else lcd.write(16);                       // если дробные == 0, заливаем пустой
        for (byte k = n + 1; k < height; k++) {   // всё что сверху заливаем пустыми
          lcd.setCursor(pos + i, (row - k));
          lcd.write(16);
        }
        break;
      }
    }
  }
}

void redrawPlot() {
  loadPlot();
  lcd.setCursor(1, 0);
  switch (debugPage) {
    case 1: lcd.print(F("TEMP"));
      drawPlot(5, 3, 15, 4, 0, 45, (int*)tempDay);
      break;
    case 2: lcd.print(F("HUM"));
      drawPlot(5, 3, 15, 4, 0, 100, (int*)humDay);
      break;
    case 3: lcd.print(F("SEN1"));
      drawPlot(5, 3, 15, 4, 0, 1023, (int*)sensDay_0);
      break;
    case 4: lcd.print(F("SEN2"));
      drawPlot(5, 3, 15, 4, 0, 1023, (int*)sensDay_1);
      break;
    case 5: lcd.print(F("SEN3"));
      drawPlot(5, 3, 15, 4, 0, 1023, (int*)sensDay_2);
      break;
    case 6: lcd.print(F("SEN4"));
      drawPlot(5, 3, 15, 4, 0, 1023, (int*)sensDay_3);
      break;
  }
}

void redrawChannels() {
  // вывод иmени канала
  if (currentLine == 0 || currentLine == 4) {
    lcd.setCursor(1, 0);
    /*// костыльная конструкция ради сохранения паmяти
    switch (currentChannel) {
      case 0: lcd.print(F("Channel 0"));
        break;
      case 1: lcd.print(F("Channel 1"));
        break;
      case 2: lcd.print(F("Channel 2"));
        break;
      case 3: lcd.print(F("Channel 3"));
        break;
      case 4: lcd.print(F("Channel 4"));
        break;
      case 5: lcd.print(F("Channel 5"));
        break;
      case 6: lcd.print(F("Channel 6"));
        break;
      case 7: lcd.print(F("Servo 1"));
        break;
      case 8: lcd.print(F("Servo 2"));
        break;
      case 9: lcd.print(F("Drive"));
        break;
    }*/
    lcd.print(channelNames[currentChannel]);

    // вывод состояния
    lcd.setCursor(15, 0);
    if (channels[currentChannel].state) lcd.print(F("On"));
    else lcd.print(F("Off"));
  }

  // вывод настроек
  if (channels[currentChannel].state) {
    byte curMode;
    if (currentChannel < 7) curMode = 0;
    else if (currentChannel < 9) curMode = 1;
    else curMode = 2;

    if (currentLine == 1 || currentLine == 4) {
      lcd.setCursor(1, 1);
      clearLine();
      lcd.setCursor(1, 1);
      if (channels[currentChannel].relayType != 2) {
        lcd.print(settingsNames[curMode * 3 + 0]); spaceColon();
        lcd.print(F(" <"));
        lcd.print(modeNames[channels[currentChannel].mode]);
        lcd.print(F(">"));
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
        lcd.print(relayNames[channels[currentChannel].relayType]);
      }
      if (curMode == 1) {
        lcd.setCursor(11, 3);
        if (currentChannel == 7) {
          lcd.print(minAngle[0]);
          lcd.setCursor(16, 3);
          lcd.print(maxAngle[0]);
        } else if (currentChannel == 8) {
          lcd.print(minAngle[1]);
          lcd.setCursor(16, 3);
          lcd.print(maxAngle[1]);
        }
      }
      if (curMode == 2) {
        //lcd.setCursor(11, 3);
        lcd.print(driveTimeout);
      }
    }
  }
}

void redrawSettings() {
  byte curSetMode = channels[currentChannel].mode;

  if (currentLine == 4) {
    lcd.setCursor(1, 0);
    lcd.print(F("<"));
    lcd.print(modeNames[channels[currentChannel].mode]);
    lcd.print(F(">"));
    lcd.setCursor(15, 0); lcd.print(F("Back"));
  }

  if (currentLine == 1 || currentLine == 4) {
    lcd.setCursor(1, 1);
    clearLine();
    lcd.setCursor(1, 1);

    switch (curSetMode) {
      case 0: lcd.print(modeSettingsNames[0]); spaceColon();
        break;
      case 1: lcd.print(modeSettingsNames[3]); spaceColon();
        break;
      case 2: lcd.print(modeSettingsNames[6]); spaceColon();
        break;
      case 3: lcd.print(modeSettingsNames[8]); spaceColon();
        break;
    }

    switch (curSetMode) {
      case 0:
        lcd.setCursor(8, 1);
        if (thisH[0] < 10) lcd.print(F("00"));
        if (thisH[0] >= 10 && thisH[0] < 100) lcd.print(F("0"));
        lcd.print(String(thisH[0]));
        lcd.print(F(":"));
        if (thisM[0] < 10) lcd.print(0);
        lcd.print(String(thisM[0]));
        lcd.print(F(":"));
        if (thisS[0] < 10) lcd.print(0);
        lcd.print(String(thisS[0]));
        break;
      case 1: lcd.print(String(impulsePrds[channels[currentChannel].impulsePrd]));
        if (channels[currentChannel].impulsePrd < 6)
          lcd.print(F(" m "));
        else
          lcd.print(F(" h  "));
        break;
      case 2: lcd.print(String(channels[currentChannel].hour1));
        lcd.print(F(" h "));
        break;
      case 3:
        if (channels[currentChannel].sensPeriod < 60) {
          lcd.print(channels[currentChannel].sensPeriod);
          lcd.print(F(" s "));
        } else {
          lcd.print((float)channels[currentChannel].sensPeriod / 60, 1);
          lcd.print(F(" m "));
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
        break;
      case 1: lcd.print(modeSettingsNames[4]); spaceColon();
        break;
      case 2: lcd.print(modeSettingsNames[7]); spaceColon();
        break;
      case 3: lcd.print(modeSettingsNames[9]); spaceColon();
        break;
    }

    switch (curSetMode) {
      case 0:
        lcd.setCursor(9, 2);
        if (thisH[1] < 10) lcd.print(0);
        lcd.print(String(thisH[1]));
        lcd.print(F(":"));
        if (thisM[1] < 10) lcd.print(0);
        lcd.print(String(thisM[1]));
        lcd.print(F(":"));
        if (thisS[1] < 10) lcd.print(0);
        lcd.print(String(thisS[1]));
        break;
      case 1: lcd.print(channels[currentChannel].work);
        lcd.print(F(" s "));
        break;
      case 2: lcd.print(String(channels[currentChannel].hour2));
        lcd.print(F(" h "));
        break;
      case 3: lcd.print(sensorNames[channels[currentChannel].sensor]);
        lcd.print(F(" "));
        lcd.print(sensorVals[channels[currentChannel].sensor]);
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
          lcd.print(String(leftH) + ":" + String(leftM) + ":" + String(leftS));
        }
        break;
      case 1:
        lcd.print(modeSettingsNames[5]); spaceColon();
        lcd.print(channels[currentChannel].startHour);
        lcd.print(F(" h "));
        break;
      case 2:
        break;
      case 3:
        lcd.print(modeSettingsNames[10]); spaceColon();
        lcd.print(channels[currentChannel].threshold);
        break;
    }
  }
}

void redrawDebug() {
  //lcd.clear();
  lcd.setCursor(1, 0); lcd.print(F("DEBUG"));
  lcd.setCursor(7, 0); lcd.print(F("S1:"));
  if (channels[7].state) {
    //lcd.print(String(servoPos[0]));
    //lcd.print(F(" "));
    if (channelStates[7]) lcd.print(F("Max"));
    else lcd.print(F("Min"));
  }
  else lcd.print(F("-"));

  lcd.setCursor(14, 0); lcd.print(F("S2:"));
  if (channels[8].state) {
    //lcd.print(String(servoPos[1]));
    //if (servoPos[0] < 100) lcd.print(F(" "));
    if (channelStates[8]) lcd.print(F("Max"));
    else lcd.print(F("Min"));
  }
  else lcd.print(F("-"));

  lcd.setCursor(0, 1); lcd.print("H:"); lcd.print(sensorVals[1]);
  lcd.setCursor(5, 1); lcd.print(sensorVals[2]); lcd.print(F("  "));
  lcd.setCursor(9, 1); lcd.print(sensorVals[3]); lcd.print(F("  "));
  lcd.setCursor(13, 1); lcd.print(sensorVals[4]); lcd.print(F("  "));
  lcd.setCursor(17, 1); lcd.print(sensorVals[5]);
  if (sensorVals[4] < 10) lcd.print(F("  "));
  else if (sensorVals[4] < 100) lcd.print(F(" "));

  lcd.setCursor(0, 2); lcd.print(F("T:")); lcd.print(String(sensorVals[0]));
  lcd.setCursor(5, 2); lcd.print(F("R:"));
  for (byte i = 0; i < 7; i++) {
    if (channels[i].state) lcd.print(channelStates[i]);
    else lcd.print(F("-"));
  }
  lcd.setCursor(15, 2); lcd.print(F("D:"));
  if (channels[9].state) lcd.print(lastDriveState);
  else lcd.print(F("-"));

  lcd.setCursor(0, 3);
  if (realTime[0] < 10) lcd.print(F(" "));
  lcd.print(String(realTime[0])); lcd.print(F(":"));
  if (realTime[1] < 10) lcd.print(0);
  lcd.print(String(realTime[1])); lcd.print(F(":"));
  if (realTime[2] < 10) lcd.print(0);
  lcd.print(String(realTime[2]));
  lcd.setCursor(12, 3); lcd.print(F("U:")); lcd.print(uptime);
}

void redrawMainSettings() {
  if (currentLine == 4) lcd.clear();

  if (currentLine == 0 || currentLine == 4) {
    clearLine2(0);
    lcd.setCursor(1, 0); lcd.print(F("SETTINGS"));
    lcd.setCursor(11, 0); lcd.print(F("A-BKL:"));
    if (LCD_BACKL) lcd.print(F("On"));
    else lcd.print(F("Off"));
  }
  if (currentLine == 1 || currentLine == 4) {
    clearLine2(1);
    lcd.setCursor(0, 1); lcd.print(F("BKL-TOUT:")); lcd.print(BACKL_TOUT);
    lcd.setCursor(13, 1); lcd.print(F("DRV:")); lcd.print(DRIVE_SPEED);
  }
  if (currentLine == 2 || currentLine == 4) {
    clearLine2(2);
    lcd.setCursor(0, 2); lcd.print(F("S1_SP:")); lcd.print(SERVO1_SPEED);
    lcd.setCursor(10, 2); lcd.print(F("S1_ACC:")); lcd.print(SERVO1_ACC, 1);
  }
  if (currentLine == 3 || currentLine == 4) {
    clearLine2(3);
    lcd.setCursor(0, 3); lcd.print(F("S2_SP:")); lcd.print(SERVO2_SPEED);
    lcd.setCursor(10, 3); lcd.print(F("S2_ACC:")); lcd.print(SERVO2_ACC, 1);
  }
}

void redrawService() {
  if (currentLine == 4) lcd.clear();

  if (currentLine == 0 || currentLine == 4) {
    clearLine2(0);
    lcd.setCursor(1, 0); lcd.print(F("SERVICE"));
    lcd.setCursor(12, 0);
    if (realTime[0] < 10) lcd.print(F(" "));
    lcd.print(String(realTime[0])); lcd.print(F(":"));
    if (realTime[1] < 10) lcd.print(0);
    lcd.print(String(realTime[1])); lcd.print(F(":"));
    if (realTime[2] < 10) lcd.print(0);
    lcd.print(String(realTime[2]));
  }

  if (currentLine == 1 || currentLine == 4) {
    clearLine2(1);
    lcd.setCursor(0, 1); lcd.print(F("R1 2 3 4 5 6 7"));
    lcd.setCursor(14, 1); lcd.print(F("S1:")); lcd.print(String(servoPosServ[0]));
  }
  if (currentLine == 2 || currentLine == 4) {
    clearLine2(2);
    lcd.setCursor(1, 2); for (byte i = 0; i < 7; i++) {
      lcd.print(channelStatesServ[i]);
      lcd.print(" ");
    }
    lcd.setCursor(14, 2); lcd.print(F("S2:")); lcd.print(String(servoPosServ[1]));
  }
  if (currentLine == 3 || currentLine == 4) {
    clearLine2(3);
    lcd.setCursor(0, 3); lcd.print(F("D:"));
    lcd.setCursor(2, 3);
    if (channelStatesServ[9]) lcd.print(F("OPEN"));
    else lcd.print(F("CLOS"));

    lcd.setCursor(7, 3); lcd.print(F("SP:"));
    lcd.print(comSensPeriod);
  }
}

void redrawScreen() {
  if (currentLine == 4) lcd.clear();

  if (navDepth == 0) {            // корень mеню
    if (currentChannel >= 0) {    // для всех кроме DEBUG
      redrawChannels();           // вывод настроек каналов
    } else if (currentChannel == -1) {
      redrawDebug();
    } else if (currentChannel == -2) {
      redrawMainSettings();
    } else if (currentChannel == -3) {
      redrawService();
    }
  } else {
    redrawSettings();
  }
  drawArrow();
}

void spaceColon() {
  lcd.print(F(": "));
}

void clearLine() {
  for (byte i = 0; i < 19; i++) {
    lcd.print(F(" "));
  }
}
void clearLine2(byte row) {
  lcd.setCursor(0, row);
  for (byte i = 0; i < 20; i++) {
    lcd.print(F(" "));
  }
}
