// -------------------- SETUP ---------------------
void setup() {
  /*Serial.begin(9600);
    delay(50);
    Serial.println("start");*/
    //uartBegin();
#if (WDT_ENABLE == 1)
  wdt_disable();
  delay(3000); // Задержка, чтобы было время перепрошить устройство в случае bootloop
  wdt_enable (WDTO_8S); // Для тестов не рекомендуется устанавливать значение менее 8 сек.
#endif

  // ----- дисплей -----
  lcd.init();
  lcd.backlight();
  lcd.clear();
  //#if (USE_PLOTS == 1)
  lcd.createChar(0, row8);
  lcd.createChar(1, row1);
  lcd.createChar(2, row2);
  lcd.createChar(3, row3);
  lcd.createChar(4, row4);
  lcd.createChar(5, row5);
  lcd.createChar(6, row6);
  lcd.createChar(7, row7);
  //#endif

  // ----- RTC -----
  rtc.begin();
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  now = rtc.now();
  getDay();

  // ---------- BME ----------
#if (USE_BME == 1)
  bme.begin(BME_ADDR);
#endif

  // --------- dallas ---------
#if (DALLAS_SENS1 == 1)
  sensors.begin();
  sensors.setWaitForConversion(false);  // асинхронное получение данных
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
  if (EEPROM.read(1022) != 42 || resetSettings) {
    EEPROM.write(1022, 42);
    for (byte i = 0; i < 10; i++) {
      EEPROM.put(i * 30, channels[i]);
    }
    EEPROM.put(300, minAngle[0]);
    EEPROM.put(301, minAngle[1]);
    EEPROM.put(302, maxAngle[0]);
    EEPROM.put(303, maxAngle[1]);
    EEPROM.put(304, driveTimeout);
    EEPROM.put(900, settings);

    for (byte i = 0; i < 5; i++) {
      EEPROM.put(i * 25 + 400, PID[i]);
      EEPROM.put(i * 7 + 500, dawn[i]);
    }
  }

  // ----- чтение настроек -----
  for (byte i = 0; i < 10; i++) {
    EEPROM.get(i * 30, channels[i]);
  }
  minAngle[0] = EEPROM.read(300);
  minAngle[1] = EEPROM.read(301);
  maxAngle[0] = EEPROM.read(302);
  maxAngle[1] = EEPROM.read(303);
  driveTimeout = EEPROM.read(304);

  for (byte i = 0; i < 5; i++) {
    EEPROM.get(i * 25 + 400, PID[i]);
    EEPROM.get(i * 7 + 500, dawn[i]);
  }

  EEPROM.get(900, settings);

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
  if (!channels[7].direction) servoPos[0] = minAngle[0];
  else servoPos[0] = maxAngle[0];
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
  if (!channels[8].direction) servoPos[1] = minAngle[1];
  else servoPos[1] = maxAngle[1];
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
