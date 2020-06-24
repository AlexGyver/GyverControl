/***************************************************************************************************/
/* 
  This is an Arduino example for SHT21, HTU21D Digital Humidity & Temperature Sensor shows how to
  reassign default SDA/SCL pins for ESP8266-01

  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/

  This sketch uses I2C bus to communicate, specials pins are required to interface
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

  GNU GPL license, all text above must be included in any redistribution, see link below for details
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#include <Wire.h>              //use bug free i2c driver https://github.com/enjoyneering/ESP8266-I2C-Driver
#include <HTU21D.h>
#include <LiquidCrystal_I2C.h> //https://github.com/enjoyneering/LiquidCrystal_I2C
#include <ESP8266WiFi.h>

#undef SDA                     //delete dafault SDA pin number
#undef SCL                     //delete dafault SCL pin number

#define SDA           1        //assign new SDA pin to GPIO1/D2/0TX for all slaves on i2c bus
#define SCL           3        //assign new SCL pin to GPIO3/D7/0RX for all slaves on i2c bus

#define LCD_ROWS      4        //qnt. of lcd rows
#define LCD_COLUMNS   20       //qnt. of lcd columns
#define DEGREE_SYMBOL 0xDF     //degree symbol from the LCD ROM
#define SPACE_SYMBOL  0x20     //space  symbol from lcd ROM

const uint8_t temperature_icon[8] PROGMEM = {0x04, 0x0A, 0x0A, 0x0A, 0x0A, 0x1F, 0x1F, 0x0E}; //PROGMEM saves variable to flash & keeps dynamic memory free
const uint8_t humidity_icon[8]    PROGMEM = {0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x00};

float   temperature = 0;
float   humidity    = 0;


/*
HTU21D(resolution)
resolution:
HTU21D_RES_RH12_TEMP14 - RH: 12Bit, Temperature: 14Bit, by default
HTU21D_RES_RH8_TEMP12  - RH: 8Bit,  Temperature: 12Bit
HTU21D_RES_RH10_TEMP13 - RH: 10Bit, Temperature: 13Bit
HTU21D_RES_RH11_TEMP11 - RH: 11Bit, Temperature: 11Bit
*/ 
HTU21D            myHTU21D(HTU21D_RES_RH12_TEMP14);
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);


void setup()
{
  WiFi.persistent(false);                                       //disable saving wifi config into SDK flash area
  WiFi.forceSleepBegin();                                       //disable AP & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep

  Serial.begin(115200);
  Serial.swap();                                                //remap serial pins GPIO1/D2/UART0TXD & GPIO3/D7/UART0RXD to GPIO15/UART0RTS & GPIO13/UART0CTS
  
  /* LCD connection check */  
  while (lcd.begin(LCD_COLUMNS, LCD_ROWS, LCD_5x8DOTS) != true) //20 colums, 4 rows, 5x8 pixels char size
  {
    digitalWrite(LED, LOW);
    delay(500);
    digitalWrite(LED, HIGH);
    delay(500);
  }

  lcd.print(F("PCF8574 is OK"));
  delay(1000);

  lcd.clear();

  /* HTU21D connection check */
  while (myHTU21D.begin() != true)
  {
    lcd.setCursor(0, 0);
    lcd.print(F("HTU21D error"));
    delay(5000);
  }

  lcd.clear();

  lcd.print(F("HTU21D OK"));
  delay(1000);

  lcd.clear();

  /* load custom symbol to CGRAM */
  lcd.createChar(0, temperature_icon);                          //variable stored in flash
  lcd.createChar(1, humidity_icon);

  /* prints static text */
  lcd.setCursor(0, 0);                                          //set 1-st colum & 1-st row
  lcd.write(0);                                                 //print custom tempereture symbol

  lcd.setCursor(0, 1);                              
  lcd.write(1);
}

void loop()
{
  humidity    = myHTU21D.readCompensatedHumidity();
  temperature = myHTU21D.readTemperature();

  /* prints dynamic temperature data */
  lcd.setCursor(1, 0);

  if   (temperature != HTU21D_ERROR) lcd.print(temperature);
  else                               lcd.print(F("xxx"));
  lcd.write(DEGREE_SYMBOL);
  lcd.write(SPACE_SYMBOL);

  /* prints dynamic humidity data */
  lcd.setCursor(1, 1);

  if   (humidity != HTU21D_ERROR) lcd.print(humidity);
  else                            lcd.print(F("xxx"));
  lcd.print(F("%"));
  lcd.write(SPACE_SYMBOL);

  delay(10000);
}
