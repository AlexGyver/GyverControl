void setup() {
#if (DEBUG_ENABLE == 1)
  uart.begin(9600);
#endif

#if (DEBUG_PID > 0)
#if (PID_AUTOTUNE == 1)
  uart.println("input , min , max");
#else
#if (SHOW_INTEGRAL == 1)
  uart.println("set , input , integral/3 , out");
#else
  uart.println("set , input , out");
#endif
#endif

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
  EEPROM.get(EEPR_SETTINGS, settings);
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
  EEPROM.get(EEPR_SETTINGS, settings);     // чтение настроек
  applySettings();  // применение настроек

  // ----- понеслась -----
  currentChannel = -1;  // окно дебаг
  currentLine = 4;
  drawArrow();
  redrawScreen();
  disableABC();
  customSetup();        // вызов кастомного блока инициализации (вкладка custom)
}

ISR(INT0_vect) {        // External interrupt vectors
  enc.tick(controlState);
}

ISR(INT1_vect) {
  enc.tick(controlState);
}
