/***************************************************************************************************/
/* 
  This is an Arduino example for SHT21, HTU21D Digital Humidity & Temperature Sensor

  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/

  This sensor uses I2C bus to communicate, specials pins are required to interface
  Board:                                    SDA                    SCL
  Uno, Mini, Pro, ATmega168, ATmega328..... A4                     A5
  Mega2560, Due............................ 20                     21
  Leonardo, Micro, ATmega32U4.............. 2                      3
  Digistump, Trinket, ATtiny85............. 0/physical pin no.5    2/physical pin no.7
  Blue Pill, STM32F103xxxx boards.......... PB7*                   PB6*
  ESP8266 ESP-01:.......................... GPIO0/D5               GPIO2/D3
  NodeMCU 1.0, WeMos D1 Mini............... GPIO4/D2               GPIO5/D1

                                           *STM32F103xxxx pins B7/B7 are 5v tolerant, but bi-directional
                                            logic level converter is recommended

  Frameworks & Libraries:
  ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
  ESP8266 Core          - https://github.com/esp8266/Arduino
  ESP8266 I2C lib fixed - https://github.com/enjoyneering/ESP8266-I2C-Driver
  STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

  GNU GPL license, all text above must be included in any redistribution, see link below for details:
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#include <Wire.h>
#include <HTU21D.h>
#include <LiquidCrystal_I2C.h> //https://github.com/enjoyneering/LiquidCrystal_I2C

#define LCD_ROWS      4        //qnt. of lcd rows
#define LCD_COLUMNS   20       //qnt. of lcd columns
#define DEGREE_SYMBOL 0xDF     //degree symbol from the LCD ROM
#define MAX_HUMIDITY  100      //max. relative humidity
#define LED           1        //connect led to ATtiny85 pin no.6 in series with 470 Ohm resistor

float   humidity = 0;

const uint8_t humidity_icon[8]    PROGMEM = {0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x00}; //PROGMEM saves variable to flash & keeps dynamic memory free
const uint8_t plus_minus_icon[8]  PROGMEM = {0x00, 0x04, 0x0E, 0x04, 0x00, 0x0E, 0x00, 0x00};
const uint8_t temperature_icon[8] PROGMEM = {0x04, 0x0A, 0x0A, 0x0E, 0x0E, 0x1F, 0x1F, 0x0E};

/*
HTU21D(resolution)

resolution:
HTU21D_RES_RH12_TEMP14 - RH: 12-Bit, Temperature: 14-Bit, by default
HTU21D_RES_RH8_TEMP12  - RH: 8-Bit,  Temperature: 12-Bit
HTU21D_RES_RH10_TEMP13 - RH: 10-Bit, Temperature: 13-Bit
HTU21D_RES_RH11_TEMP11 - RH: 11-Bit, Temperature: 11-Bit
*/
HTU21D            myHTU21D(HTU21D_RES_RH12_TEMP14);
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);


void setup()
{
  pinMode(LED, OUTPUT);

  /* LCD connection check */ 
  while (lcd.begin(LCD_COLUMNS, LCD_ROWS) != true)          //20 colums, 4 rows
  {
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }

  /* HTU21D connection check */
  while (myHTU21D.begin() != true)
  {
    lcd.print(F("HTU21D error"));                           //(F()) saves string to flash & keeps dynamic memory free
    delay(5000);
  }

  lcd.clear();

  lcd.print(F("HTU21D OK"));
  delay(2000);

  lcd.clear();

  /* load custom symbol to CGRAM */
  lcd.createChar(0, humidity_icon);
  lcd.createChar(1, temperature_icon);
  lcd.createChar(2, plus_minus_icon);

  /* prints static text */
  lcd.setCursor(0, 0); 
  lcd.write(1);                                             //print custom tempereture symbol

  lcd.setCursor(0, 1);                                      //set 1-st colum & 2-nd row, first colum & row started at zero
  lcd.write(0);                                             //print custom humidity symbol

  lcd.setCursor(0, 2);
  lcd.print(F("Battery:"));

  lcd.setCursor(11, 2);
  lcd.print(F("FW:"));
}


void loop()
{
  humidity = myHTU21D.readCompensatedHumidity();

  /* prints dynamic text & data */
  lcd.setCursor(1, 0);
  lcd.print(myHTU21D.readTemperature());
  lcd.write(2);                                             //print custom plus/minus symbol
  lcd.print(F("0.3"));
  lcd.write(DEGREE_SYMBOL);                                 //print degree symbol from the LCD ROM
  lcd.print(F("C "));

  lcd.setCursor(1, 1);
  lcd.print(humidity);
  lcd.write(2);
  lcd.print(F("2% "));
  
  lcd.setCursor(7, 2);
  if (myHTU21D.batteryStatus() == true)
  {
    lcd.print(F("OK "));
  }
  else
  {
    lcd.print(F("Low"));
  }

  lcd.setCursor(14, 2);
  lcd.print(myHTU21D.readFirmwareVersion());

  /* prints horizontal graph from 0 to MAX_HUMIDITY */
  lcd.printHorizontalGraph('H', 3, humidity, MAX_HUMIDITY); //name of the bar, row, current value, max. value

  delay(20000);
}
