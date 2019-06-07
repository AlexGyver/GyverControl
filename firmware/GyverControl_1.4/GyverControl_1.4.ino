/*
  Скетч к проекту "Универсальный контроллер"
  Страница проекта (схемы, описания): https://alexgyver.ru/gyvercontrol/
  Исходники на GitHub: https://github.com/AlexGyver/gyvercontrol
  Нравится, как написан и закомментирован код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2019
  http://AlexGyver.ru/
*/

/*
   ===== Версия 1.4 =====
  - Данные с аналоговых датчиков усредняются средним арифметическим
  - Температура в градусах (с бортового термометра, термистора или далласа) выводится со значком градуса
  - Добавлен режим ПИД для каналов реле 3 и 4, обоих каналов серво, канала привода
  - Добавлен режим РАССВЕТ для каналов реле 3 и 4 и обоих каналов серво
  - Каналы ПИД и рассвета помечены * (звёздочкой)
  - На экране отладки каналы ПИД и рассвета выводятся графиком (0-255 ШИМ, 0-180 серво)
  - Добавлена настройка, позволяющая убрать из кода датчик BME280
  - К режиму Day добавлена настройка Global, задающая время работы для всех режимов
  - В режиме Timer RTC появились периоды 1, 2.. 7 суток
  - Добавлена поддержка датчиков DHT11/DHT22. ВЫВОД ЗНАЧЕНИЙ ВМЕСТО BME280!!!
  - Значения датчиков сведены к 8 битам (0 - минимум, 255 - максимум)
  - В режиме Sensor можно установить максимум меньше минимума, это инвертирует работу режима
  - Графики можно отключить, это экономит память
  - На экране debug после времени отображается день недели (цифра 1 - 7)
  - Ещё больше оптимизации
  - И ещё больше оптимизации
  - Поправлены баги
  - Режим Day заменён на режим Week - "Неделька"
*/

// -------------------- НАСТРОЙКИ ---------------------
// ======= ЭНКОДЕР =======
#define ENCODER_TYPE 1      // тип энкодера (0 или 1). Если энкодер работает некорректно (пропуск шагов/2 шага), смените тип
#define ENC_REVERSE 0       // 1 - инвертировать направление энкодера, 0 - нет
#define CONTROL_TYPE 1      // тип управления энкодером:
// 0 - удерживание и поворот для изменения значения
// 1 - клик для входа в изменение, повторный клик для выхода (стрелочка меняется на галочку)

// ======= КАНАЛЫ =======
#define DRIVER_LEVEL 1      // 1 или 0 - уровень сигнала на драйвер/реле для привода
#define SERVO1_RELAY 1      // 1 - заменить серво 1 на реле. 0 - ничего не делать
#define SERVO2_RELAY 1      // 1 - заменить серво 2 на реле. 0 - ничего не делать
// названия каналов (только английские буквы)
const char *channelNames[] = {
  "Channel 1",
  "Channel 2",
  "Channel 3",
  "Channel 4",
  "Channel 5",
  "Channel 6",
  "Channel 7",
  "Servo 1",
  "Servo 2",
  "Drive",
};

// ======== СИСТЕМА =======
#define WDT_ENABLE 1        // 1 - включить, 0 - отключить watchdog (только для optiboot)
#define USE_PLOTS 0         // 1 - включить, 0 - отключить вывод графиков
#define USE_PID 0           // включает/отключает поддержку ПИД регулятора на каналах 2, 3, серво и привода
#define USE_DAWN 0          // включает/отключает поддержку режима РАССВЕТ на каналах 2, 3, серво
#define SETT_TIMEOUT 100    // таймаут неактивности (секунд) после которого автоматически откроется DEBUG и сохранятся настройки
#define LCD_ADDR 0x3f       // адрес дисплея - 0x27 или 0x3f . Смени если не работает!!
#define BME_ADDR 0x76       // адрес BME280 - 0x76 или 0x77. Смени если не работает!! (добавлено в v1.1.1)

// ======== ДАТЧИКИ =======
// цифровой датчик температуры и влажности bme280 (шина i2c)
#define USE_BME 0           // 1 - использовать BME280, 0 - не использовать

// цифровой датчик температуры ds18b20 (вход SENS1)
#define DALLAS_SENS1 0      // 1 - ко входу SENS1 подключен ds18b20, 0 - подключен обычный аналоговый датчик

// цифровой датчик температуры и влажности DHT11/DHT22 (вход SENS2) - вместо BME280
#define DHT_SENS2 0         // 1 - ко входу SENS2 подключен DHT11/DHT22, 0 - подключен обычный аналоговый датчик
#define DHT_TYPE DHT22       // тип DHT датчика: DHT11 или DHT22

// термисторы
#define THERM1 0            // 1 - ко входу SENS1 подключен термистор, 0 - подключен другой аналоговый датчик
#define BETA_COEF1 3435     // температурный коэффициент термистора 1 (см. даташит)

#define THERM2 0            // 1 - ко входу SENS2 подключен термистор, 0 - подключен другой аналоговый датчик
#define BETA_COEF2 3435     // температурный коэффициент термистора 2 (см. даташит)

#define THERM3 0            // 1 - ко входу SENS3 подключен термистор, 0 - подключен другой аналоговый датчик
#define BETA_COEF3 3435     // температурный коэффициент термистора 3 (см. даташит)

#define THERM4 0            // 1 - ко входу SENS4 подключен термистор, 0 - подключен другой аналоговый датчик
#define BETA_COEF4 3435     // температурный коэффициент термистора 4 (см. даташит)

// -------------------- ПИНЫ ---------------------
#define SW        0
#define RELAY_0   1
#define DT        2
#define CLK       3
#define RELAY_1   4
#define RELAY_2   5
#define RELAY_3   6
#define RELAY_4   7
#define RELAY_5   8
#define RELAY_6   9
#define DRV_SIGNAL1 10
#define DRV_PWM     11
#define DRV_SIGNAL2 12
#define SERVO_0   13
#define SERVO_1   A0
#define SENS_VCC  A1
#define SENS_1    A2
#define SENS_2    A3
#define SENS_3    A6
#define SENS_4    A7

// -------------------- БИБЛИОТЕКИ ---------------------
#include "encMinim.h"
encMinim enc(CLK, DT, SW, ENC_REVERSE, ENCODER_TYPE);

#if (SERVO1_RELAY == 0 || SERVO2_RELAY == 0)
#include <ServoSmooth.h>
#endif
#if (SERVO1_RELAY == 0)
ServoSmooth servo1;
#endif
#if (SERVO2_RELAY == 0)
ServoSmooth servo2;
#endif

//#include "LCD_1602_RUS.h"
//LCD_1602_RUS lcd(LCD_ADDR, 20, 4);
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);

#include <EEPROM.h>

#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

// adafr
#if (USE_BME == 1)
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
#endif

#if (DALLAS_SENS1 == 1)
#include <OneWire.h>            // библиотека протокола датчиков
#include <DallasTemperature.h>  // библиотека датчика

OneWire oneWire(SENS_1);
DallasTemperature sensors(&oneWire);
#endif

#if (DHT_SENS2 == 1)
#include <DHT.h>
DHT dht(SENS_2, DHT_TYPE);
#endif

#if (WDT_ENABLE == 1)
#include <avr/wdt.h>
#endif

// -------------------- ПЕРЕМЕННЫЕ ---------------------
// dawn
struct {
  int8_t start = 0;
  int8_t stop = 0;
  byte dur1 = 60;
  byte dur2 = 60;
  byte minV = 0;
  byte maxV = 255;
} dawn[4];

// PID
struct {
  int8_t sensor = 0;      // канал датчика
  float kP = 0.0;         // P
  float kI = 0.0;         // I
  float kD = 0.0;         // D
  byte dT = 1;            // период итерации, с
  byte minSignal = 0;     // мин. сигнал
  byte maxSignal = 100;   // макс. сигнал
  int setpoint = 20;      // установка
} PID[5];

float integralSum[5];
int prevInput[5];
int input[5];
int output[5];
uint32_t PIDtimers[5];

// Settings
struct {
  boolean backlight = 1;    // автоотключение подсветки дисплея по таймауту бездействия (1 - разрешить)
  byte backlTime = 60;      // таймаут отключения дисплея, секунды
  byte drvSpeed = 255;      // скорость привода, 0-255
  byte srv1_Speed = 40;     // макс. скорость серво 1, 0-255
  byte srv2_Speed = 40;     // макс. скорость серво 2, 0-255
  float srv1_Acc = 0.2;     // ускорение серво 1, 0.0-1.0
  float srv2_Acc = 0.2;     // ускорение серво 2, 0.0-1.0
  int16_t comSensPeriod = 10;
  int8_t plotMode = 0;
} settings;

// Channels
struct channelsStruct {
  boolean state = 0;          // состояние канала (вкл/выкл)
  boolean direction = true;   // направление работы
  boolean global = false;     // режим глобальных суток
  int8_t week = 0;            // неделька
  int8_t sensor;              // тип датчика (air temp, air hum, mois1...)
  int8_t relayType;           // тип реле (помпа, клапан, реле)
  int8_t mode;                // режим работы (таймер, rtc, сутки, датчик)
  int8_t startHour = 0;       // начальный час для таймера RTC
  int8_t impulsePrd = 1;      // период импульса
  int16_t threshold = 30;     // мин. порог срабатывания
  int16_t thresholdMax = 30;  // макс. порог срабатывания
  int16_t sensPeriod = 2;     // период опроса датчика (секунды)
  uint32_t period = 100;      // период вызова
  uint32_t work = 0;          // период работы
};
channelsStruct channels[10];
uint32_t timerMillis[10];       // счётчик миллис

uint32_t driveTimer;
byte driveState;
boolean lastDriveState;
byte driveTimeout = 5;          // таймаут движения привода
boolean manualControl;
boolean manualPos;
boolean controlState;

byte minAngle[] = {0, 0};         // мин. угол
byte maxAngle[] = {180, 180};     // макс. угол

//byte eeprAdr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 17};
byte impulsePrds[] = {1, 5, 10, 15, 20, 30, 1, 2, 3, 4, 6, 8, 12, 1, 2, 3, 4, 5, 6, 7};
byte relayPins[] = {RELAY_0, RELAY_1, RELAY_2, RELAY_3, RELAY_4, RELAY_5, RELAY_6};

int sensorVals[6];    // темп. воздв, влажн в, почв1, почв2, почв3, почв4
int8_t realTime[3];
float uptime = 0;
byte servoPos[2];
byte servoPosServ[2];
byte pwmVal[2];

int driveStep;   // время движения привода для ПИД
boolean drivePidFlag; // флаг на движение по ПИДу

boolean channelStates[10];
boolean channelStatesServ[10];
int8_t debugPage;

int8_t arrowPos;  // 0-3
int8_t navDepth;  // 0-2
int8_t currentChannel = 0;  // -3 - 14
int8_t currentMode; // 0-3
int8_t thisH[2], thisM[2], thisS[2];
byte currentLine;
uint32_t commonTimer, backlTimer, plotTimer;
int16_t plotTimeout = 5760;
boolean backlState = true;
int tempDay[15], humDay[15];
int sensDay_0[15], sensDay_1[15], sensDay_2[15], sensDay_3[15];

boolean serviceFlag;
boolean startPID = false;
boolean timeChanged;
boolean startFlagDawn;
boolean settingsChanged;
uint32_t settingsTimer;

const byte daysMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0};
uint16_t thisDay;
boolean EEPROM_updFlag = false;

// символы
// график
byte row8[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
byte row7[8] = {0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
byte row6[8] = {0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
byte row5[8] = {0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
byte row4[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111};
byte row3[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
byte row2[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
byte row1[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111};

const char *settingsNames[]  = {
  "Mode",
  "Direction",
  "Type",

  "Mode",
  "Direction",
  "Limits",

  "Mode",
  "Direct.",
  "Timeout",
};

const char *modeNames[]  = {
  "Timer",
  "Timer RTC",
  "Week",
  "Sensor",
  "PID",
  "Dawn",
};

const char *relayNames[]  = {
  "Relay",
  "Valve",
  "Common",
};

const char *modeSettingsNames[]  = {
  "Period",   // 0
  "Work",     // 1
  "Left",     // 2

  "Period",   // 3
  "Work",     // 4
  "Start hour", // 5

  "",    // 6
  "",     // 7

  "Period",   // 8
  "Sensor",   // 9
  //"Threshold",  // 10
};

const char *sensorNames[]  = {
  "Air t.",
  "Air h.",

  "Sens1",
  "Sens2",
  "Sens3",
  "Sens4",
};

const char *directionNames[]  = {
  "Off-On",
  "On-Off",
  "Min-Max",
  "Max-Min",
  "Close-Open",
  "Open-Close",
};

// -------------------- SETUP ---------------------
void setup() {
  /*Serial.begin(9600);
    delay(50);
    Serial.println("start");*/
#if (WDT_ENABLE == 1)
  wdt_disable();
  delay(3000); // Задержка, чтобы было время перепрошить устройство в случае bootloop
  wdt_enable (WDTO_8S); // Для тестов не рекомендуется устанавливать значение менее 8 сек.
#endif

  // ----- дисплей -----
  lcd.init();
  lcd.backlight();
  lcd.clear();
  loadPlot();

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
}

void enISR1() {
  enc.tick();
}
void enISR2() {
  enc.tick();
}

// -------------------- LOOP ---------------------
void loop() {
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

// -------------------- SERVICE ---------------------
void serviceTick() {
  if (millis() - commonTimer > 1000) {
    commonTimer = millis();
    // управляем реле
    for (byte i = 0; i < 7; i++)
      digitalWrite(relayPins[i], channelStatesServ[i]);

    if (SERVO1_RELAY) {
      digitalWrite(SERVO_0, channelStatesServ[7]);
    }
    if (SERVO2_RELAY) {
      digitalWrite(SERVO_1, channelStatesServ[8]);
    }

    // управляем серво
#if (SERVO1_RELAY == 0)
    servo1.setTargetDeg(servoPosServ[0]);

#endif
#if (SERVO2_RELAY == 0)
    servo2.setTargetDeg(servoPosServ[1]);
#endif
  }
}

void serviceIN() {    // выполняем при входе в сервис
#if (SERVO1_RELAY == 0)
  servo1.start();
#endif
#if (SERVO2_RELAY == 0)
  servo2.start();
#endif

  servoPosServ[0] = servoPos[0];
  servoPosServ[1] = servoPos[1];

  serviceFlag = true;
  for (byte i = 0; i < 10; i++) channelStatesServ[i] = channelStates[i];
  realTime[0] = now.hour();
  realTime[1] = now.minute();
  realTime[2] = now.second();
}

void serviceOUT() {   // выполняем при выходе с сервиса
  serviceFlag = false;
  startFlagDawn = true;
  for (byte i = 0; i < 7; i++) digitalWrite(relayPins[i], channelStates[i]);    // вернуть реле на места
  if (SERVO1_RELAY) {
    digitalWrite(SERVO_0, channelStates[7]);
  }
  if (SERVO2_RELAY) {
    digitalWrite(SERVO_1, channelStates[8]);
  }

  // шим - рассвет
  if (channels[2].mode == 5) {
    analogWrite(RELAY_2, pwmVal[0]);
  }
  if (channels[3].mode == 5) {
    analogWrite(RELAY_3, pwmVal[1]);
  }

  // вернуть серво на места
#if (SERVO1_RELAY == 0)
  if (!channels[7].state) servo1.stop();
  servo1.setTargetDeg(servoPos[0]);
#endif
#if (SERVO2_RELAY == 0)
  if (!channels[8].state) servo2.stop();
  servo2.setTargetDeg(servoPos[1]);
#endif
  // вернуть привод
  driveState = 1;

  if (timeChanged) {
    timeChanged = false;
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), realTime[0], realTime[1], realTime[2]));
  }

  updateSettings();
}
// -------------------- SERVICE ---------------------

void backlTick() {
  if (settings.backlight && backlState && millis() - backlTimer >= (long)settings.backlTime * 1000) {
    backlState = false;
    lcd.noBacklight();
  }
}

void debTick() {
  if ( (currentChannel >= 0 || currentChannel == -2) && millis() - settingsTimer > SETT_TIMEOUT * 1000L) {   // если не дебаг и сработал таймер
    settingsTimer = millis();
    EEPROM_updFlag = true;
    if (currentChannel == -2) updateSettings();
    if (currentChannel >= 0) updateEEPROM(currentChannel);
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

void loadPlot() {
#if (USE_PLOTS == 1)
  lcd.createChar(0, row8);
  lcd.createChar(1, row1);
  lcd.createChar(2, row2);
  lcd.createChar(3, row3);
  lcd.createChar(4, row4);
  lcd.createChar(5, row5);
  lcd.createChar(6, row6);
  lcd.createChar(7, row7);
#endif
}
