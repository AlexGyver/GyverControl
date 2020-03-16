// заполнение массива значений с датчиков
// здесь можно прописать свой датчик!
void getAllData() {
  sensorVals[0] = 0;
  sensorVals[1] = 0;

#if (USE_BME == 1)
  sensorVals[0] = bme.readTemperature();
  sensorVals[1] = bme.readHumidity();
  sensorVals[1] = constrain(sensorVals[1], 0, 99);
#endif

#if (DHT_SENS2 == 1)
  sensorVals[0] = (dht.readTemperature());
  sensorVals[1] = (dht.readHumidity());
  sensorVals[1] = constrain(sensorVals[1], 0, 99);
#endif

  sensorVals[2] = analogReadAverage(SENS_1) / 4;
  sensorVals[3] = analogReadAverage(SENS_2) / 4;
  sensorVals[4] = analogReadAverage(SENS_3) / 4;
  sensorVals[5] = analogReadAverage(SENS_4) / 4;

#if (DALLAS_SENS1 == 1)
  sensorVals[2] = dallas.getTemp();
  dallas.requestTemp();
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
  thermistor = RESIST_10K / ((float)1023 / resistance - 1);
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

uint32_t sensorTimer;
uint32_t period;
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

void plotTick() {
#if (USE_PLOTS == 1)
  if (millis() - plotTimer >= plotTimeout * 1000L) {
    plotTimer = millis();
    // сдвигаем массивы
    for (byte i = 0; i < 14; i++) {
      tempDay[i] = tempDay[i + 1];
      humDay[i] = humDay[i + 1];
      sensDay_0[i] = sensDay_0[i + 1];
      sensDay_1[i] = sensDay_1[i + 1];
      sensDay_2[i] = sensDay_2[i + 1];
      sensDay_3[i] = sensDay_3[i + 1];
    }

    // обновляем крайний элемент
    tempDay[14] = sensorVals[0] * 7;
    humDay[14] = sensorVals[1] * 7;
    sensDay_0[14] = sensorVals[2] * 7;
    sensDay_1[14] = sensorVals[3] * 7;
    sensDay_2[14] = sensorVals[4] * 7;
    sensDay_3[14] = sensorVals[5] * 7;
    // почему на 7? Чтобы при использовании датчика СО2 у нас не перепоолнился int...
  }
#endif
}
