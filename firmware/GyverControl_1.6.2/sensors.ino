// заполнение массива значений с датчиков
// здесь можно прописать свой датчик!
void getAllData() {
  sensorVals[0] = 0;
  sensorVals[1] = 0;

#if (USE_BME == 1)
  sensorVals[0] = bme.readTemperature();
  sensorVals[1] = bme.readHumidity();
#endif


#if (DHT_SENS2 == 1)
  sensorVals[0] = dht.readTemperature();
  sensorVals[1] = dht.readHumidity();
#endif

#if (USE_HTU21D == 1)
  sensorVals[1] = myHTU21D.readHumidity();
  sensorVals[0] = myHTU21D.readTemperature();
#endif

  sensorVals[1] = constrain(sensorVals[1], 0, 99);

  sensorVals[2] = analogReadAverage(SENS_1) / 4;
  sensorVals[3] = analogReadAverage(SENS_2) / 4;
  sensorVals[4] = analogReadAverage(SENS_3) / 4;
  sensorVals[5] = analogReadAverage(SENS_4) / 4;

#if (DALLAS_SENS1 == 1)
#if (DALLAS_AMOUNT > 1)
  for (byte i = 0; i < DALLAS_AMOUNT; i++) {
    float thisDal = dallas[i].getTemp();
    if (thisDal != 0) dallasBuf[i] = thisDal;
    dallas[i].requestTemp();
  }
  float thisMin = 200.0, thisMax = -200.0, thisSum = 0.0;
  for (byte i = 0; i < DALLAS_AMOUNT; i++) {
    thisSum += dallasBuf[i];
    if (dallasBuf[i] > thisMax) thisMax = dallasBuf[i];
    if (dallasBuf[i] < thisMin) thisMin = dallasBuf[i];
  }

#if (DALLAS_MODE == 0)
  sensorVals[2] = thisSum / DALLAS_AMOUNT;
#elif (DALLAS_MODE == 1)
  sensorVals[2] = thisMax;
#elif (DALLAS_MODE == 2)
  sensorVals[2] = thisMin;
#endif

#else
  sensorVals[2] = dallas.getTemp();
  dallas.requestTemp();
#endif
#endif

#if (USE_CO2 == 1)
#if (CO2_PIN == 1)
  sensorVals[2] = CO2ppm;
#elif (CO2_PIN == 2)
  sensorVals[3] = CO2ppm;
#endif
#endif

#if (THERM1 == 1)
  sensorVals[2] = getThermTemp(analogReadAverage(SENS_1), BETA_COEF1);
#endif
#if (THERM2 == 1)
  sensorVals[3] = getThermTemp(analogReadAverage(SENS_2), BETA_COEF2);
#endif
#if (THERM3 == 1)
  sensorVals[4] = getThermTemp(analogReadAverage(SENS_3), BETA_COEF3);
#endif
#if (THERM4 == 1)
  sensorVals[5] = getThermTemp(analogReadAverage(SENS_4), BETA_COEF4);
#endif
}

#if (THERM1 == 1 || THERM2 == 1 || THERM3 == 1 || THERM4 == 1)
#define RESIST_BASE 10000   // сопротивление при TEMP_BASE градусах по Цельсию (Ом)
#define TEMP_BASE 25        // температура, при которой измерено RESIST_BASE (градусов Цельсия)
#define RESIST_10K 10000    // точное сопротивление 10к резистора (Ом)

float getThermTemp(int resistance, int B_COEF) {
  float thermistor;
  thermistor = RESIST_10K / ((float)1024 / resistance - 1);
  thermistor /= RESIST_BASE;                        // (R/Ro)
  thermistor = log(thermistor) / B_COEF;            // 1/B * ln(R/Ro)
  thermistor += (float)1.0 / (TEMP_BASE + 273.15);  // + (1/To)
  thermistor = (float)1.0 / thermistor - 273.15;    // инвертируем и конвертируем в градусы по Цельсию
  return thermistor;
}
#endif

int analogReadAverage(byte pin) {
  // пропускаем первые 10 измерений
  for (byte i = 0; i < 10; i++)
    analogRead(pin);

  // суммируем
  int sum = 0;
  for (byte i = 0; i < 10; i++)
    sum += analogRead(pin);
  return (sum / 10);
}

uint32_t sensorTimer = 0;
uint32_t period = 1000;
byte sensorMode = 0;
void readAllSensors() {
  if (millis() - sensorTimer >= period) {
    sensorTimer = millis();
    switch (sensorMode) {
      case 0:   // вкл питание
        sensorMode = 1;
        period = 100;
        digitalWrite(SENS_VCC, 1);
        break;
      case 1:   // измеряем
        sensorMode = 2;
        period = 25;
        getAllData();
        break;
      case 2:   // выключаем
        sensorMode = 0;
        period = (long)settings.comSensPeriod * 1000;
        digitalWrite(SENS_VCC, 0);
        break;
    }
  }
}

#if (USE_PLOTS == 1)
void plotTick() {
  if (millis() - plotTimer >= 60000L) {     // таймер каждую минуту
    plotTimer += 60000L;
    static uint16_t counter = 0;
    static int averH[6] = {0, 0, 0, 0, 0, 0};
    static int averD[6] = {0, 0, 0, 0, 0, 0};
    int tempPlot[6][15];
    counter++;                              // минутный счётчик

    int ints[6];
    // массив измерений, переведённый в int с коэффициентом 7
    for (byte i = 0; i < 6; i++) ints[i] = (float)sensorVals[i] * 7;

    scrollPlot(sensMinute, ints);           // каждую минуту сдвигаем минутный график

    if (counter % 10 == 0) {                // каждые 10 минут суммируем в часовое усреднение
      for (byte i = 0; i < 6; i++) averH[i] += ints[i];
    }
    if (counter % 240 == 0) {               // каждые 4 часа суммируем в суточное усреднение
      for (byte i = 0; i < 6; i++) averD[i] += ints[i];
    }
    if (counter % 60 == 0) {                      // каждый час
      for (byte i = 0; i < 6; i++) averH[i] /= 6; // усредняем
      EEPROM.get(EEPR_PLOT_H, tempPlot);          // загружаем часовой график
      scrollPlot(tempPlot, averH);                // двигаем и добавляем
      EEPROM.put(EEPR_PLOT_H, tempPlot);          // сохраняем
      for (byte i = 0; i < 6; i++) averH[i] = 0;  // обнуляем среднее
    }

    if (counter % 1440L == 0) {                   // каждые сутки
      for (byte i = 0; i < 6; i++) averD[i] /= 6; // усредняем
      EEPROM.get(EEPR_PLOT_D, tempPlot);          // загружаем дневной график
      scrollPlot(tempPlot, averD);                // двигаем и добавляем
      EEPROM.put(EEPR_PLOT_D, tempPlot);          // сохраняем
      for (byte i = 0; i < 6; i++) averD[i] = 0;  // обнуляем среднее
      counter = 0;
    }
  }

}
void scrollPlot(int vals[6][15], int newVal[6]) {
  for (byte i = 0; i < 6; i++) {
    for (byte j = 0; j < 14; j++)
      vals[i][j] = vals[i][j + 1];

    // обновляем крайний элемент
    vals[i][14] = newVal[i];
  }
}
#endif
