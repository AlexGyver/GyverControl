void setup() {
#ifdef DEBUG_ENABLE
  Serial.begin(9600);
#endif
  boolean startupPress = false;
  initHardware();

  // просто сброс настроек
#if (START_MENU == 0)
  // сброс настроек
  if (!digitalRead(SW)) {
    startupPress = true;
    lcd.setCursor(0, 0);
    lcd.print(F("Reset settings OK"));
  }
  while (!digitalRead(SW));
#else
  // стартовое меню
  readEEPROM();
  if (!digitalRead(SW)) {
    drawStartMenu(0);
    while (!digitalRead(SW));
    startMenu();
  }

#endif

  // ----- первый запуск или сброс -----
  if (EEPROM.read(EEPR_KEY_ADDR) != EEPR_KEY || startupPress) {
    clearEEPROM();  // сброс настроек
  }
  readEEPROM();     // чтение настроек
  applySettings();  // применение настроек

  // ----- понеслась -----
  currentChannel = -1;  // окно дебаг
  currentLine = 4;
  drawArrow();
  redrawScreen();
  disableABC();
  customSetup();        // вызов кастомного блока инициализации (вкладка custom)
}

#if (ENC_REVERSE == 1)
void enISR1() {
  enc.tick();
}
#else
void enISR2() {
  enc.tick();
}
#endif
