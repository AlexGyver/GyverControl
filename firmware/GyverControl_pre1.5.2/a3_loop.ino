// -------------------- LOOP ---------------------
void loop() {
  customLoop();       // вызов блока своих функций (вкладка custom)

  checkPID();         // пересчёт регулятора
  backlTick();        // таймер неактивности подсветки
  debTick();          // таймер неактивности дисплея
  controlTick();      // управление
  plotTick();         // суточные графики
  readAllSensors();   // опрос датчиков

  driveTick();        // работа привода

#if (SERVO1_RELAY == 0)
  servo1.tick();      // здесь происходит движение серво по встроенному таймеру!
#endif
#if (SERVO2_RELAY == 0)
  servo2.tick();      // здесь происходит движение серво по встроенному таймеру!
#endif

  if (currentChannel == -3) {         // если СЕРВИС
    serviceTick();
  } else {  // если ОТЛАДКА или НАСТРОЙКИ
    if (millis() - commonTimer > 1000) {
      commonTimer = millis();
      timersTick();
      if (currentChannel == -1) {
        if (debugPage == 0) redrawDebug();
        else redrawPlot();
      }
    }
  }

#if (WDT_ENABLE == 1)
  wdt_reset();        // пинаем собаку
#endif
}
// -------------------- LOOP ---------------------
