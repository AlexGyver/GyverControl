/*
  struct {
  byte pidChannel = 0;    // выбор канала
  int startDay = 1;  // день
  int endDay = 1;    // месяц
  byte pointAmount = 15;  // количество точек
  int setpoints[30];  // массив расписания
  } schedule[3];
  struct {
  int8_t start = 0;
  int8_t stop = 0;
  uint8_t dur1 = 30;
  uint8_t dur2 = 30;
  uint8_t minV = 0;
  uint8_t maxV = 255;
  // 6
  } dawn[7];

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
  } PID[7];
  struct {
  boolean backlight = 1;    // автоотключение подсветки дисплея по таймауту бездействия (1 - разрешить)
  byte backlTime = 60;      // таймаут отключения дисплея, секунды
  byte drvSpeed = 125;      // скорость привода, 0-255
  byte srv1_Speed = 40;     // макс. скорость серво 1, 0-255
  byte srv2_Speed = 40;     // макс. скорость серво 2, 0-255
  float srv1_Acc = 0.2;     // ускорение серво 1, 0.0-1.0
  float srv2_Acc = 0.2;     // ускорение серво 2, 0.0-1.0
  int16_t comSensPeriod = 5;
  int8_t plotMode = 0;
  byte minAngle[2] = {0, 0};
  byte maxAngle[2] = {180, 180};
  byte driveTimeout = 5;
  } settings; //21
  struct channelsStruct {
  boolean type = 0;
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
  uint32_t weekOn = 100;      // неделька вкл
  uint32_t weekOff = 0;       // неделька выкл
  };
  channelsStruct channels[10];
  void setup() {
  Serial.begin(9600);
  Serial.println(sizeof(channels) / 10);
  Serial.println(sizeof(PID) / 7);
  Serial.println(sizeof(dawn) / 7);
  Serial.println(sizeof(settings) / 1);
  Serial.println(sizeof(schedule) / 3);
  Serial.println();

  int count = 0;
  count += sizeof(channels);
  Serial.println(count);
  count += sizeof(PID);
  Serial.println(count);
  count += sizeof(dawn);
  Serial.println(count);
  count += sizeof(settings);
  Serial.println(count);
  count += sizeof(schedule);
  Serial.println(count);

  }

  void loop() {
  // put your main code here, to run repeatedly:

  }
*/
