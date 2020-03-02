// -------------------- SETUP ---------------------
void setup() {
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

  // --------- dallas ---------
#if (DALLAS_SENS1 == 1)

#endif

  // ---------- DHT ----------
#if (DHT_SENS2 == 1)
  dht.begin();
#endif

  // ----- сброс настроек -----
  boolean resetSettings = false;
  if (!digitalRead(SW)) {
    resetSettings = true;
    lcd.setCursor(0, 0);
    lcd.print(F("Reset settings OK"));
  }
  while (!digitalRead(SW));

  // ----- первый запуск или сброс -----
  if (EEPROM.read(1022) != EEPR_KEY || resetSettings) {
    EEPROM.write(1022, EEPR_KEY);
    for (byte i = 0; i < 10; i++) {
      EEPROM.put(i * 40, channels[i]);
    }
    EEPROM.put(EEPR_SETTINGS, settings);

    for (byte i = 0; i < PID_CH_AMOUNT; i++) {
      EEPROM.put(i * 20 + EEPR_PID, PID[i]);
      if (i < 4) EEPROM.put(i * 6 + EEPR_DAWN, dawn[i]);
    }
  }

  // ----- чтение настроек -----
  for (byte i = 0; i < 10; i++) {
    EEPROM.get(i * 40, channels[i]);
  }
  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
    EEPROM.get(i * 20 + EEPR_PID, PID[i]);
    if (i < 4) EEPROM.get(i * 6 + EEPR_DAWN, dawn[i]);
  }
  EEPROM.get(EEPR_SETTINGS, settings);


  /*
    24 часа - каждые 1,6 часа - 5 760 000
    1 час - каждые 4 минуты - 240 000
    1 минута - каждые 4 секунды - 4 000
  */
  switch (settings.plotMode) {
    case 0: plotTimeout = 5760;
      break;
    case 1: plotTimeout = 240;
      break;
    case 2: plotTimeout = 4;
      break;
  }

  // ----- серво -----
  // настройки серво
  channelStates[7] = !channels[7].direction;
  channelStates[8] = !channels[8].direction;

#if (SERVO1_RELAY == 0)
  servo1.setSpeed(settings.srv1_Speed);    // ограничить скорость
  servo1.setAccel(settings.srv1_Acc);      // установить ускорение (разгон и торможение)
  if (!channels[7].direction) servoPos[0] = settings.minAngle[0];
  else servoPos[0] = settings.maxAngle[0];
  // подключение серво
  // сервы при запуске знают, где они стоят и не двигаются
  servo1.attach(SERVO_0, 600, 2400, servoPos[0]); // 600 и 2400 - длины импульсов, при которых серво крутит на 0 и 180. Также указываем стартовый угол
  if (!channels[7].state) servo1.stop();
  servo1.setCurrentDeg(servoPos[0]);
  servo1.setTargetDeg(servoPos[0]);
#endif

#if (SERVO2_RELAY == 0)
  servo2.setSpeed(settings.srv2_Speed);    // ограничить скорость
  servo2.setAccel(settings.srv2_Acc);      // установить ускорение (разгон и торможение)
  if (!channels[8].direction) servoPos[1] = settings.minAngle[1];
  else servoPos[1] = settings.maxAngle[1];
  servo2.attach(SERVO_1, 600, 2400, servoPos[1]); // аналогично
  if (!channels[8].state) servo2.stop();
  servo2.setCurrentDeg(servoPos[1]);
  servo2.setTargetDeg(servoPos[1]);
#endif

  // ----- реле -----
  for (byte i = 0; i < 7; i++) {
    pinMode(relayPins[i], OUTPUT);
    channelStates[i] = !channels[i].direction;        // вернуть реле на места
    if (channels[i].mode < 4) digitalWrite(relayPins[i], channelStates[i]);     // вернуть реле на места
  }
  pinMode(SENS_VCC, OUTPUT);
  if (SERVO1_RELAY) {
    pinMode(SERVO_0, OUTPUT);
    digitalWrite(SERVO_0, channelStates[7]);
  }
  if (SERVO2_RELAY) {
    pinMode(SERVO_1, OUTPUT);
    digitalWrite(SERVO_1, channelStates[8]);
  }

  // привод
  // частота на пинах 3 и 11 - 31.4 кГц (влияет на tone() )
  TCCR2A |= _BV(WGM20);
  TCCR2B = TCCR2B & 0b11111000 | 0x01;

  /*// частота на пинах 3 и 11 - 7.8 кГц (влияет на tone() )
    TCCR2A |= _BV(WGM20) | _BV(WGM21);
    TCCR2B = TCCR2B & 0b11111000 | 0x02;*/
  pinMode(DRV_PWM, OUTPUT);
  pinMode(DRV_SIGNAL1, OUTPUT);
  pinMode(DRV_SIGNAL2, OUTPUT);
  digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL);
  digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL);
  analogWrite(DRV_PWM, settings.drvSpeed);
  lastDriveState = !channels[9].direction;
  channelStates[9] = lastDriveState;
  manualPos = !lastDriveState;

  // прерывание на энкодер
  if (ENC_REVERSE) attachInterrupt(0, enISR1, CHANGE);
  else attachInterrupt(1, enISR2, CHANGE);

  startFlagDawn = true;

  // костыли
  channels[9].type = true;  // привод
#if (SERVO1_RELAY == 0)
  channels[7].type = true;  // серво
#endif
#if (SERVO2_RELAY == 0)
  channels[8].type = true;  // серво
#endif

  // ----- понеслась -----
  currentChannel = -1;  // окно дебаг
  currentLine = 4;
  drawArrow();
  redrawScreen();

  customSetup();        // вызов кастомного блока инициализации (вкладка custom)
}

void enISR1() {
  enc.tick();
}
void enISR2() {
  enc.tick();
}
