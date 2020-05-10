#include <microDS18B20.h>

MicroDS18B20 sensor_1(A2);
uint8_t address[8];

void setup() {
  Serial.begin(9600);
}

void loop() {
  sensor_1.readAddress(address);    // Читаем адрес термометра в наш массив
  Serial.print('{');
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("0x");
    Serial.print(address[i], HEX);  // Выводим уникальный адрес для последующего копирования
    if (i < 7) Serial.print(',');
  }
  Serial.println("},");
  delay(3000);
}
