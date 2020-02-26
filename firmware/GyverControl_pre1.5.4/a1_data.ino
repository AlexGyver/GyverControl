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
#include <microWire.h>
#include <microLiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);

#include <EEPROM.h>

#include <microDS3231.h>
MicroDS3231 rtc;

// bme
#if (USE_BME == 1)
#include <GyverBME280.h>
GyverBME280 bme;
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>
//#define SEALEVELPRESSURE_HPA (1013.25)
//Adafruit_BME280 bme;
#endif

#if (DALLAS_SENS1 == 1)
#include <microDS18B20.h>
MicroDS18B20 dallas(SENS_1);
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
  uint8_t dur1 = 60;
  uint8_t dur2 = 60;
  uint8_t minV = 0;
  uint8_t maxV = 255;
  // 6
} dawn[4];

// PID
#define PID_CH_AMOUNT 5
struct {
  int8_t sensor = 0;      // канал датчика
  float kP = 0.0;         // P
  float kI = 0.0;         // I
  float kD = 0.0;         // D
  byte dT = 1;            // период итерации, с
  byte minSignal = 0;     // мин. сигнал
  byte maxSignal = 100;   // макс. сигнал
  float setpoint = 20;    // установка
  // 20
} PID[PID_CH_AMOUNT];

float integralSum[PID_CH_AMOUNT];
float prevInput[PID_CH_AMOUNT];
float input[PID_CH_AMOUNT];
int output[PID_CH_AMOUNT];
uint32_t PIDtimers[PID_CH_AMOUNT];

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
  uint32_t periodW = 100;     // период вызова
  uint32_t workW = 0;         // период работы
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
const byte impulsePrds[] = {1, 5, 10, 15, 20, 30, 1, 2, 3, 4, 6, 8, 12, 1, 2, 3, 4, 5, 6, 7};
byte relayPins[] = {RELAY_0, RELAY_1, RELAY_2, RELAY_3, RELAY_4, RELAY_5, RELAY_6};

float sensorVals[6];    // темп. воздв, влажн в, сенс1, сенс2, сенс3, сенс4
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
  "Air t",
  "Air h",

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
