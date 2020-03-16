void initHardware() {
#if (WDT_ENABLE == 1)
  wdt_disable();
  delay(3000); // Задержка, чтобы было время перепрошить устройство в случае bootloop
  wdt_enable (WDTO_8S); // Для тестов не рекомендуется устанавливать значение менее 8 сек.
#endif

  // ----- дисплей -----
  lcd.init();
  lcd.backlight();
  lcd.clear();
#if (USE_PLOTS == 1 || USE_PID == 1 || USE_DAWN == 1)
  lcd.createChar(0, row8);
  lcd.createChar(1, row1);
  lcd.createChar(2, row2);
  lcd.createChar(3, row3);
  lcd.createChar(4, row4);
  lcd.createChar(5, row5);
  lcd.createChar(6, row6);
  lcd.createChar(7, row7);
#endif

  // ----- RTC -----
  if (rtc.lostPower()) {  //  при потере питания
    rtc.setTime(COMPILE_TIME);  // установить время компиляции
  }

  // ---------- BME ----------
#if (USE_BME == 1)
  bme.begin(BME_ADDR);
#endif

  // ---------- DHT ----------
#if (DHT_SENS2 == 1)
  dht.begin();
#endif

#if (USE_DRIVE == 1)
  // привод
  // частота на пинах 3 и 11 - 31.4 кГц
  TCCR2A |= _BV(WGM20);
  TCCR2B = TCCR2B & 0b11111000 | 0x01;

  /*// частота на пинах 3 и 11 - 7.8 кГц
    TCCR2A |= _BV(WGM20) | _BV(WGM21);
    TCCR2B = TCCR2B & 0b11111000 | 0x02;*/
  pinMode(DRV_PWM, OUTPUT);
  pinMode(DRV_SIGNAL1, OUTPUT);
  pinMode(DRV_SIGNAL2, OUTPUT);
  digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL);
  digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL);
  analogWrite(DRV_PWM, settings.drvSpeed);
#endif

  // прерывание на энкодер
#if (ENC_REVERSE == 1)
  attachInterrupt(0, enISR1, CHANGE);
#else
  attachInterrupt(1, enISR2, CHANGE);
#endif

  pinMode(SENS_VCC, OUTPUT);

#if (SERVO1_RELAY == 1)
  pinMode(SERVO_0, OUTPUT);
#endif

#if (SERVO2_RELAY == 1)
  pinMode(SERVO_1, OUTPUT);
#endif

  for (byte i = 0; i < 7; i++) pinMode(relayPins[i], OUTPUT);

#if (SERVO1_RELAY == 0)
  servo1.attach(SERVO_0, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
#endif
#if (SERVO2_RELAY == 0)
  servo2.attach(SERVO_1, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
#endif
}

void applySettings() {
  // после чтения настроек!
  plotTimeout = plotTimeouts[settings.plotMode];

  channelStates[7] = !channels[7].direction;
  channelStates[8] = !channels[8].direction;

#if (USE_DRIVE == 1)
  analogWrite(DRV_PWM, settings.drvSpeed);
  lastDriveState = !channels[9].direction;
  channelStates[9] = lastDriveState;
  manualPos = !lastDriveState;
#endif
  startFlagDawn = true;

  // костыли
  channels[9].type = true;  // привод
#if (SERVO1_RELAY == 0)
  channels[7].type = true;  // серво
#endif
#if (SERVO2_RELAY == 0)
  channels[8].type = true;  // серво
#endif

#if (SERVO2_RELAY == 1)
  digitalWrite(SERVO_1, channelStates[8]);
#endif
#if (SERVO1_RELAY == 1)
  digitalWrite(SERVO_0, channelStates[7]);
#endif

  // ----- реле -----
  for (byte i = 0; i < 7; i++) {
    channelStates[i] = !channels[i].direction;        // вернуть реле на места
    if (channels[i].mode < 4) digitalWrite(relayPins[i], channelStates[i]);     // вернуть реле на места
  }

#if (SERVO1_RELAY == 0)
  if (!channels[7].direction) pwmVal[4] = settings.minAngle[0];
  else pwmVal[4] = settings.maxAngle[0];

#if (SMOOTH_SERVO == 1)
  servo1.attach(SERVO_0, SERVO_MIN_PULSE, SERVO_MAX_PULSE, pwmVal[4]); // 600 и 2400 - длины импульсов, при которых серво крутит на 0 и 180. Также указываем стартовый угол
  if (!channels[7].state) servo1.stop();

  servo1.setSpeed(settings.srv1_Speed);    // ограничить скорость
  servo1.setAccel(settings.srv1_Acc);      // установить ускорение (разгон и торможение)
  servo1.setCurrentDeg(pwmVal[4]);
  servo1.setTargetDeg(pwmVal[4]);
#else
  servo1.attach(SERVO_0, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  servo1.write(pwmVal[4]);
#endif
#endif

#if (SERVO2_RELAY == 0)
  if (!channels[8].direction) pwmVal[5] = settings.minAngle[1];
  else pwmVal[5] = settings.maxAngle[1];

#if (SMOOTH_SERVO == 1)
  servo2.attach(SERVO_1, SERVO_MIN_PULSE, SERVO_MAX_PULSE, pwmVal[5]); // аналогично
  if (!channels[8].state) servo2.stop();

  servo2.setSpeed(settings.srv2_Speed);    // ограничить скорость
  servo2.setAccel(settings.srv2_Acc);      // установить ускорение (разгон и торможение)
  servo2.setCurrentDeg(pwmVal[5]);
  servo2.setTargetDeg(pwmVal[5]);
#else
  servo2.attach(SERVO_1, SERVO_MIN_PULSE, SERVO_MAX_PULSE); // аналогично
  servo2.write(pwmVal[5]);
#endif
#endif
}


void debTick() {
  if ( (currentChannel >= 0 || currentChannel == -2) && millis() - settingsTimer > SETT_TIMEOUT * 1000L) {   // если не дебаг и сработал таймер
    settingsTimer = millis();
    EEPROM_updFlag = true;
    updateEEPROM(currentChannel);
    currentChannel = -1;
    navDepth = 0;
    arrowPos = 0;
    debugPage = 0;
    currentLine = 4;
    redrawScreen();
  }
}

void backlOn() {
  backlState = true;
  backlTimer = millis();
  lcd.backlight();
}


void backlTick() {
  if (backlState && settings.backlight && millis() - backlTimer >= (long)settings.backlTime * 1000) {
    backlState = false;
    lcd.noBacklight();
  }
}

// микро-юарт для отключения калибровки MH-Z19
void disableABC() {
#if (USE_CO2 == 1 && CO2_CALIB == 0)
  _tx_delay = 1000000UL / 9600;
  _tx_pin_reg = portOutputRegister(digitalPinToPort(CO2_RX));
  _tx_pin_mask = digitalPinToBitMask(CO2_RX);
  uint8_t command[] = {0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87};

  for (int i = 0; i < 9; i++) {
    uint8_t data = command[i];
    *_tx_pin_reg &= ~ _tx_pin_mask;
    delayMicroseconds(_tx_delay);
    for (uint8_t i = 0; i < 8; i++) {
      if (data & 0x01) {
        *_tx_pin_reg |= _tx_pin_mask;
        delayMicroseconds(_tx_delay);
      } else {
        *_tx_pin_reg &= ~ _tx_pin_mask;
        delayMicroseconds(_tx_delay);
      }
      data >>= 1;
    }
    *_tx_pin_reg |= _tx_pin_mask;
    delayMicroseconds(_tx_delay);
  }
#endif
}
