void drawPlot(byte pos, byte row, byte width, byte height, int min_val, int max_val, int *plot_array) {
#if (USE_PLOTS == 1)
  int max_value = -32000;
  int min_value = 32000;

  for (byte i = 0; i < 15; i++) {
    if (plot_array[i] > max_value) max_value = plot_array[i];
    if (plot_array[i] < min_value) min_value = plot_array[i];
  }
  lcd.setCursor(0, 1); lcd.print(max_value); lcd.print(F("  "));
  //lcd.setCursor(0, 1); lcd.print(label);
  lcd.setCursor(0, 2); lcd.print(F(">")); lcd.print(plot_array[14]); lcd.print(F("  "));
  lcd.setCursor(0, 3); lcd.print(min_value); lcd.print(F("  "));

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
#endif
}

void redrawPlot() {
#if (USE_PLOTS == 1)
  lcd.setCursor(1, 0);
  switch (debugPage) {
    case 1: lcd.print(F("TEMP"));
      drawPlot(5, 3, 15, 4, 0, 45, (int*)tempDay);
      break;
    case 2: lcd.print(F("HUM"));
      drawPlot(5, 3, 15, 4, 0, 100, (int*)humDay);
      break;
    case 3: lcd.print(F("SEN1"));
#if (THERM1 == 1)
      drawPlot(5, 3, 15, 4, 0, 100, (int*)sensDay_0);
#else
      drawPlot(5, 3, 15, 4, 0, 255, (int*)sensDay_0);
#endif
      break;
    case 4: lcd.print(F("SEN2"));
#if (THERM2 == 1)
      drawPlot(5, 3, 15, 4, 0, 100, (int*)sensDay_1);
#else
      drawPlot(5, 3, 15, 4, 0, 255, (int*)sensDay_1);
#endif
      break;
    case 5: lcd.print(F("SEN3"));
#if (THERM3 == 1)
      drawPlot(5, 3, 15, 4, 0, 100, (int*)sensDay_2);
#else
      drawPlot(5, 3, 15, 4, 0, 255, (int*)sensDay_2);
#endif
      break;
    case 6: lcd.print(F("SEN4"));
#if (THERM4 == 1)
      drawPlot(5, 3, 15, 4, 0, 100, (int*)sensDay_3);
#else
      drawPlot(5, 3, 15, 4, 0, 255, (int*)sensDay_3);
#endif
      break;
  }
#endif
}

void redrawChannels() {
  // вывод иmени канала
  if (currentLine == 0 || currentLine == 4) {
    lcd.setCursor(1, 0);
    /*// костыльная конструкция ради сохранения SRAM паmяти
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
    if (currentChannel == 2 || currentChannel == 3) lcd.write(42);

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
        if (channels[currentChannel].global && channels[currentChannel].mode != 2) lcd.print(F("+W"));
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
        if (channels[currentChannel].mode < 4)
          lcd.print(relayNames[channels[currentChannel].relayType]);
        else
          lcd.print("---");
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
  byte curPWMchannel = channelToPWM(currentChannel);

  if (currentLine == 4) {
    lcd.setCursor(1, 0);
    lcd.print(F("<"));
    lcd.print(modeNames[channels[currentChannel].mode]);
    if (channels[currentChannel].global && channels[currentChannel].mode != 2) lcd.print(F("+W"));
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
      /*case 2: lcd.print(modeSettingsNames[6]); spaceColon();
        break;*/
      case 3: lcd.print(modeSettingsNames[8]); spaceColon();
        break;
    }

    switch (curSetMode) {
      case 0:
        lcd.setCursor(8, 1);
        if (thisH[0] < 10) lcd.print(F("00"));
        if (thisH[0] >= 10 && thisH[0] < 100) lcd.print(F("0"));
        lcd.print((byte)(thisH[0]));
        lcd.print(F(":"));
        if (thisM[0] < 10) lcd.print(0);
        lcd.print((byte)(thisM[0]));
        lcd.print(F(":"));
        if (thisS[0] < 10) lcd.print(0);
        lcd.print((byte)(thisS[0]));
        break;
      case 1: lcd.print((int)(impulsePrds[channels[currentChannel].impulsePrd]));
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
      case 3:
        if (channels[currentChannel].sensPeriod < 60) {
          lcd.print(channels[currentChannel].sensPeriod);
          lcd.print(F("s "));
        } else {
          lcd.print((float)channels[currentChannel].sensPeriod / 60, 1);
          lcd.print(F("m "));
        }
        break;
      case 4:
#if (USE_PID == 1)
        lcd.setCursor(0, 1);
        lcd.print(F("P:")); lcd.print(PID[curPWMchannel].kP, 1);
        lcd.setCursor(7, 1);
        lcd.print(F("I:")); lcd.print(PID[curPWMchannel].kI, 1);
        lcd.setCursor(14, 1);
        lcd.print(F("D:")); lcd.print(PID[curPWMchannel].kD, 1);
#endif
        break;
      case 5:
#if (USE_DAWN == 1)
        lcd.setCursor(0, 1);
        lcd.print("Start:"); lcd.print(dawn[curPWMchannel].start); lcd.print("h ");
        lcd.setCursor(10, 1);
        lcd.print("Dur:"); lcd.print(dawn[curPWMchannel].dur1); lcd.print("m ");
#endif
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
      case 3: lcd.print(modeSettingsNames[9]); spaceColon();
        break;
    }

    switch (curSetMode) {
      case 0:
        lcd.setCursor(9, 2);
        if (thisH[1] < 10) lcd.print(0);
        lcd.print((byte)(thisH[1]));
        lcd.print(F(":"));
        if (thisM[1] < 10) lcd.print(0);
        lcd.print((byte)(thisM[1]));
        lcd.print(F(":"));
        if (thisS[1] < 10) lcd.print(0);
        lcd.print((byte)(thisS[1]));
        break;
      case 1: if (channels[currentChannel].work < 60) {
          lcd.print(channels[currentChannel].work);
          lcd.print(F("s "));
        } else {
          lcd.print(channels[currentChannel].work / 60);
          lcd.print(F("m "));
        }
        break;
      case 2:
        lcd.setCursor(0, 2);
        lcd.print(F("On"));
        lcd.setCursor(4, 2);
        if (thisH[0] < 10) lcd.print(" ");
        lcd.print((byte)(thisH[0]));
        lcd.print(F(":"));
        if (thisM[0] < 10) lcd.print(0);
        lcd.print((byte)(thisM[0]));
        lcd.print(F(":"));
        if (thisS[0] < 10) lcd.print(0);
        lcd.print((byte)(thisS[0]));
        break;
      case 3: lcd.print(sensorNames[channels[currentChannel].sensor]);
        lcd.print(F(" "));
        lcd.print(sensorVals[channels[currentChannel].sensor]);
        break;
      case 4:
#if (USE_PID == 1)
        lcd.setCursor(0, 2);
        lcd.print("Sens:"); lcd.print(sensorNames[PID[curPWMchannel].sensor]);
        lcd.setCursor(13, 2);
        lcd.print("set:"); lcd.print(PID[curPWMchannel].setpoint);
#endif
        break;
      case 5:
#if (USE_DAWN == 1)
        lcd.setCursor(0, 2);
        lcd.print("Stop:"); lcd.print(dawn[curPWMchannel].stop); lcd.print("h ");
        lcd.setCursor(10, 2);
        lcd.print("Dur:"); lcd.print(dawn[curPWMchannel].dur2); lcd.print("m ");
#endif
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
          lcd.print(":");
          lcd.print(leftM);
          lcd.print(":");
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
        lcd.print(F("Off"));
        lcd.setCursor(4, 3);
        if (thisH[1] < 10) lcd.print(" ");
        lcd.print((byte)(thisH[1]));
        lcd.print(F(":"));
        if (thisM[1] < 10) lcd.print(0);
        lcd.print((byte)(thisM[1]));
        lcd.print(F(":"));
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
      case 4:
#if (USE_PID == 1)
        lcd.setCursor(0, 3);
        lcd.print(F("T:")); lcd.print(PID[curPWMchannel].dT);
        if (PID[curPWMchannel].dT < 10) lcd.print("  ");
        else if (PID[curPWMchannel].dT < 100) lcd.print(" ");
        lcd.setCursor(5, 3);
        if (currentChannel == 9) {
          lcd.print(F("min:")); lcd.print(((float)PID[curPWMchannel].minSignal / 100), 1); lcd.print("  ");
          lcd.setCursor(13, 3);
          lcd.print(F("max:")); lcd.print(((float)PID[curPWMchannel].maxSignal / 100), 1);
        } else {
          lcd.print(F("min:")); lcd.print(PID[curPWMchannel].minSignal); lcd.print("  ");
          lcd.setCursor(13, 3);
          lcd.print(F("max:")); lcd.print(PID[curPWMchannel].maxSignal);
          if (PID[curPWMchannel].maxSignal < 100) lcd.print(" ");
        }
#endif
        break;
      case 5:
#if (USE_DAWN == 1)
        lcd.setCursor(0, 3);
        lcd.print(F("min:")); lcd.print(dawn[curPWMchannel].minV);
        lcd.print("  ");
        lcd.setCursor(8, 3);
        lcd.print(F("max:")); lcd.print(dawn[curPWMchannel].maxV);
        if (dawn[curPWMchannel].maxV < 100) lcd.print(" ");
#endif
        break;
    }
  }
}

void redrawDebug() {
  //lcd.clear();
  lcd.setCursor(1, 0); lcd.print(F("DEBUG"));
  lcd.setCursor(7, 0); lcd.print(F("S1:"));
  if (channels[7].state) {
    //lcd.print(/*string*/(servoPos[0]));
    //lcd.print(F(" "));
#if (SERVO1_RELAY == 0)
    if (channels[7].mode == 4 || channels[7].mode == 5) {
      lcd.print(servoPos[0]);lcd.print(' ');
      //byte thisHeight = round(servoPos[0] / 22); // перевод 0-180 в 0-8
      //if (thisHeight == 0) lcd.write(32);   // пустой
      //else if (thisHeight < 8) lcd.write(thisHeight + 1); // половинки
      //else lcd.write(0);          // полный
    } else {
      if (channelStates[7]) lcd.print(F("Max"));
      else lcd.print(F("Min"));
    }    
#else
    lcd.print(channelStates[7]);
#endif

  }
  else lcd.print(F("-"));

  lcd.setCursor(14, 0); lcd.print(F("S2:"));
  if (channels[8].state) {
    //lcd.print(/*string*/(servoPos[1]));
    //if (servoPos[0] < 100) lcd.print(F(" "));
#if (SERVO2_RELAY == 0)
    if (channels[8].mode == 4 || channels[8].mode == 5) {
      lcd.print(servoPos[1]);lcd.print(' ');
      //byte thisHeight = round(servoPos[1] / 22); // перевод 0-180 в 0-8
      //if (thisHeight == 0) lcd.write(32);   // пустой
      //else if (thisHeight < 8) lcd.write(thisHeight + 1); // половинки
      //else lcd.write(0);          // полный
    } else {
      if (channelStates[8]) lcd.print(F("Max"));
      else lcd.print(F("Min"));
    }
#else
    lcd.print(channelStates[8]);
#endif
  }
  else lcd.print(F("-"));

  lcd.setCursor(0, 1); lcd.print("H:"); lcd.print(sensorVals[1]);
  lcd.setCursor(5, 1); lcd.print(sensorVals[2]);
  if (THERM1 || DALLAS_SENS1) {
    lcd.write(223);
    lcd.print(" ");
  }
  else lcd.print(F("  "));

  lcd.setCursor(9, 1);
  if (DHT_SENS2) lcd.print("-");
  else lcd.print(sensorVals[3]);

  if (THERM2) {
    lcd.write(223);
    lcd.print(" ");
  }
  else lcd.print(F("  "));

  lcd.setCursor(13, 1); lcd.print(sensorVals[4]);
  if (THERM3) {
    lcd.write(223);
    lcd.print(" ");
  }
  else lcd.print(F("  "));

  lcd.setCursor(17, 1); lcd.print(sensorVals[5]);
  if (THERM4) {
    lcd.write(223);
    lcd.print(" ");
  } else {
    if (sensorVals[4] < 10) lcd.print(F("  "));
    else if (sensorVals[4] < 100) lcd.print(F(" "));
  }

  lcd.setCursor(0, 2); lcd.print(F("T:")); lcd.print((int)(sensorVals[0])); lcd.write(223);
  lcd.setCursor(5, 2); lcd.print(F("R:"));
  for (byte i = 0; i < 7; i++) {
    if (channels[i].state) {
      if (channels[i].mode == 4 || channels[i].mode == 5) {
        byte thisHeight;
        if (i == 2) thisHeight = round(pwmVal[0] / 32); // перевод 0-255 в 0-8
        if (i == 3) thisHeight = round(pwmVal[1] / 32); // перевод 0-255 в 0-8
        if (thisHeight == 0) lcd.write(32);   // пустой
        else if (thisHeight < 8) lcd.write(thisHeight + 1); // половинки
        else lcd.write(0);          // полный
      } else lcd.print(channelStates[i]);
    }
    else lcd.print(F("-"));
  }

  lcd.setCursor(15, 2); lcd.print(F("D:"));
  if (channels[9].state) {
    if (channels[9].mode != 4) lcd.print(lastDriveState);
    else lcd.print(driveStep);    // выводим текущую величину задержки по ПИД
  }
  else lcd.print(F("-"));

  lcd.setCursor(0, 3);
  if (realTime[0] < 10) lcd.print(F(" "));
  lcd.print((byte)(realTime[0])); lcd.print(F(":"));
  if (realTime[1] < 10) lcd.print(0);
  lcd.print((byte)(realTime[1])); lcd.print(F(":"));
  if (realTime[2] < 10) lcd.print(0);
  lcd.print((byte)(realTime[2]));
  lcd.setCursor(9, 3);
  lcd.print( rtc.getDay() );
  lcd.setCursor(12, 3); lcd.print(F("U:")); lcd.print(uptime);
}

void redrawMainSettings() {
  if (currentLine == 4) lcd.clear();

  if (currentLine == 0 || currentLine == 4) {
    clearLine2(0);
    lcd.setCursor(1, 0); lcd.print(F("SETTINGS"));
    lcd.setCursor(11, 0); lcd.print(F("A-BKL:"));
    if (settings.backlight) lcd.print(F("On"));
    else lcd.print(F("Off"));
  }
  if (currentLine == 1 || currentLine == 4) {
    clearLine2(1);
    lcd.setCursor(0, 1); lcd.print(F("BKL-TOUT:")); lcd.print(settings.backlTime);
    lcd.setCursor(13, 1); lcd.print(F("DRV:")); lcd.print(settings.drvSpeed);
  }
  if (currentLine == 2 || currentLine == 4) {
    clearLine2(2);
    lcd.setCursor(0, 2); lcd.print(F("S1_SP:")); lcd.print(settings.srv1_Speed);
    lcd.setCursor(10, 2); lcd.print(F("S1_ACC:")); lcd.print(settings.srv1_Acc, 1);
  }
  if (currentLine == 3 || currentLine == 4) {
    clearLine2(3);
    lcd.setCursor(0, 3); lcd.print(F("S2_SP:")); lcd.print(settings.srv2_Speed);
    lcd.setCursor(10, 3); lcd.print(F("S2_ACC:")); lcd.print(settings.srv2_Acc, 1);
  }
}

/*
  void redrawMainSettings() {
  if (currentLine == 4) lcd.clear();

  if (currentLine == 0 || currentLine == 4) {
    clearLine2(0);
    lcd.setCursor(1, 0); lcd.print(F("SETTINGS"));
    lcd.setCursor(11, 0); lcd.print(F("A-BKL:"));
    if (settings.backlight) lcd.print(F("On"));
    else lcd.print(F("Off"));
  }
  if (currentLine == 1 || currentLine == 4) {
    clearLine2(1);
    lcd.setCursor(0, 1); lcd.print(F("BKL-TOUT:")); lcd.print(settings.backlTime);
    lcd.setCursor(13, 1); lcd.print(F("DRV:")); lcd.print(settings.drvSpeed);
  }
  if (currentLine == 2 || currentLine == 4) {
    clearLine2(2);
    lcd.setCursor(0, 2); lcd.print(F("S1_SP:")); lcd.print(settings.srv1_Speed);
    lcd.setCursor(10, 2); lcd.print(F("S1_ACC:")); lcd.print(settings.srv1_Acc, 1);
  }
  if (currentLine == 3 || currentLine == 4) {
    clearLine2(3);
    lcd.setCursor(0, 3); lcd.print(F("S2_SP:")); lcd.print(settings.srv2_Speed);
    lcd.setCursor(10, 3); lcd.print(F("S2_ACC:")); lcd.print(settings.srv2_Acc, 1);
  }
  }
*/
void redrawService() {
  if (currentLine == 4) lcd.clear();

  if (currentLine == 0 || currentLine == 4) {
    clearLine2(0);
    lcd.setCursor(1, 0); lcd.print(F("SERVICE"));
    lcd.setCursor(12, 0);
    if (realTime[0] < 10) lcd.print(F(" "));
    lcd.print((byte)(realTime[0])); lcd.print(F(":"));
    if (realTime[1] < 10) lcd.print(0);
    lcd.print((byte)(realTime[1])); lcd.print(F(":"));
    if (realTime[2] < 10) lcd.print(0);
    lcd.print((byte)(realTime[2]));
  }

  if (currentLine == 1 || currentLine == 4) {
    clearLine2(1);
    lcd.setCursor(0, 1); lcd.print(F("R1 2 3 4 5 6 7"));
    lcd.setCursor(14, 1); lcd.print(F("S1:"));

    if (SERVO1_RELAY) {   // если реле
      lcd.print(channelStatesServ[7]);
    } else {
      lcd.print((byte)(servoPosServ[0]));
    }
  }
  if (currentLine == 2 || currentLine == 4) {
    clearLine2(2);
    lcd.setCursor(1, 2); for (byte i = 0; i < 7; i++) {
      if (channels[i].state) lcd.print(channelStatesServ[i]);
      else lcd.print('-');
      lcd.print(' ');
    }
    lcd.setCursor(14, 2); lcd.print(F("S2:"));

    if (SERVO2_RELAY) {   // если реле
      lcd.print(channelStatesServ[8]);
    } else {
      lcd.print((byte)(servoPosServ[1]));
    }

  }
  if (currentLine == 3 || currentLine == 4) {
    clearLine2(3);
    lcd.setCursor(0, 3); lcd.print(F("D:"));
    lcd.setCursor(2, 3);
    if (channelStatesServ[9]) lcd.print(F("OPEN"));
    else lcd.print(F("CLOS"));

    lcd.setCursor(7, 3); lcd.print(F("SP:"));
    lcd.print(settings.comSensPeriod);

    lcd.setCursor(14, 3); lcd.print(F("PP:"));
    switch (settings.plotMode) {
      case 0: lcd.print("DAY");
        break;
      case 1: lcd.print("HR ");
        break;
      case 2: lcd.print("MIN");
        break;
    }
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
    lcd.print((' '));
  }
}
void clearLine2(byte row) {
  lcd.setCursor(0, row);
  for (byte i = 0; i < 20; i++) {
    lcd.print((' '));
  }
}
