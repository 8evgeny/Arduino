#include "TM1637.h"
#include <OneWire.h>
#include <LiquidCrystal.h>
//#include <DallasTemperature.h>

#define ONE_WIRE_BUS 11
OneWire ds1820(ONE_WIRE_BUS);// Создаем объект OneWire для шины 1-Wire, с помощью которого будет осуществляться работа с датчиком
// Pass our oneWire reference to Dallas Temperature.
//DallasTemperature sensors(&ds1820);

#define RS 9 //pins definitions for 1602
#define E 8
#define DB4 7
#define DB5 6
#define DB6 5
#define DB7 4
LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7);

#define CLK 3 //pins definitions for TM1637
#define DIO 2
TM1637 tm1637(CLK,DIO);

const int ping1 = 12;
//const int ping2 = 11;
const int relay_mikrocomputer_1 = 10;
//const int relay_mikrocomputer_2 = 9;
//const int relay_heater_cable = 8;

int ping1State = 0;
int ping1State_read = 0;
int ping1Count = 0;
int ping2State = 0;
int ping2State_read = 0;
int ping2Count = 0;

bool state_relay_mikrocomputer_1=true;
bool state_relay_mikrocomputer_2=true;
bool state_relay_heater_cable=true;
//int8_t DispMSG[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
float tempSensor ;

 void receive_temp_no_delay(){
//#include <OneWire.h>
//     OneWire ds(8); // Объект OneWire
//     int temperature = 0; // Глобальная переменная для хранения значение температуры с датчика DS18B20
//     long lastUpdateTime = 0; // Переменная для хранения времени последнего считывания с датчика
//     const int TEMP_UPDATE_TIME = 1000; // Определяем периодичность проверок
//     void setup(){
//         Serial.begin(9600);
//     }
//     void loop(){
//         detectTemperature(); // Определяем температуру от датчика DS18b20
//         Serial.println(temperature); // Выводим полученное значение температуры
//         // Т.к. переменная temperature имеет тип int, дробная часть будет просто отбрасываться
//     }
//     int detectTemperature(){
//         byte data[2];
//         ds.reset();
//         ds.write(0xCC);
//         ds.write(0x44);
//         if (millis() - lastUpdateTime > TEMP_UPDATE_TIME)
//         {
//             lastUpdateTime = millis();
//             ds.reset();
//             ds.write(0xCC);
//             ds.write(0xBE);
//             data[0] = ds.read();
//             data[1] = ds.read();
//             // Формируем значение
//             temperature = (data[1] << 8) + data[0]; temperature = temperature >> 4;
//         }
//     }

 }

 int temp_minus=0;
// byte data[2] = {B11111100,B10010000}; // это -55 Место для значения температуры
 byte data[9]{B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B11111100,B10010000};

 void receive_temperature(){
     // Определяем температуру от датчика DS18b20
     ds1820.reset(); // Начинаем взаимодействие со сброса всех предыдущих команд и параметров
     ds1820.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство
     ds1820.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
     delay(1000); // Микросхема измеряет температуру, а мы ждем.
     ds1820.reset(); // Теперь готовимся получить значение измеренной температуры
     ds1820.write(0xCC);
     ds1820.write(0xBE); // Просим передать нам значение регистров со значением температуры
     // Получаем и считываем ответ
     data[8] = ds1820.read(); // Читаем младший байт значения температуры
     data[7] = ds1820.read(); // старший
     // Формируем итоговое : "склеиваем" значение,затем умножаем его на коэффициент, соответсвующий разрешающей способности (для 12 бит по умолчанию - это 0,0625)
     temp_minus = 0;
     temp_minus = data[7] & B10000000;
     if (temp_minus != 128){
     tempSensor =  ((data[7] << 8) | data[8]) * 0.0625;
     }
     if (temp_minus == 128){
         tempSensor =  ((~((data[7] << 8) | data[8]))+1) * 0.0625;
     }

  }

 void print_temperature() {
      int8_t Digits[] = {0x00, 0x00, 0x00, 0x00};
      int KL1 = tempSensor;
      int KL2 = (tempSensor - KL1) * 100;
      if (KL1 > 99)(KL1 = tempSensor - 100);
      //tm1637.point(POINT_ON);
      Digits[0] = (KL1 / 10); // раскидываем 4-значное число на цифры
      Digits[1] = (KL1 % 10);
//      Digits[2] = (KL2 / 10);
//      Digits[3] = (KL2 % 10);
      tm1637.display(2,Digits[0]);
      tm1637.display(3,Digits[1]);
   }

 void print_lcd_setup(){
     lcd.begin(16, 2);                  // Задаем размерность экрана
//     lcd.setCursor(0, 0);              // Устанавливаем курсор в начало 1 строки
//     lcd.print("Temperature:");       // Выводим текст
//     lcd.setCursor(0, 1);              // Устанавливаем курсор в начало 2 строки

  }

 void setup()
  {
  print_lcd_setup();


    tm1637.init();
    tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  //    sensors.begin();

   pinMode(LED_BUILTIN, OUTPUT);// initialize digital pin LED_BUILTIN as an output.
   pinMode(ping1, INPUT);
//   pinMode(ping2, INPUT);
   pinMode(relay_mikrocomputer_1, OUTPUT);
//   pinMode(relay_mikrocomputer_2, OUTPUT);
//   pinMode(relay_heater_cable, OUTPUT);
   tm1637.init();
   tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  }



 void loop()
 {
  receive_temperature();
  print_temperature();

  lcd.setCursor(0, 1);
  if(temp_minus != 128) lcd.print("+");
  if(temp_minus == 128) lcd.print("-");
  lcd.print(tempSensor);
//    lcd.setCursor(0, 0);
//    lcd.print(data[0]);
//    lcd.setCursor(0, 1);
//    lcd.print(data[1]);

  ping1State = digitalRead(ping1);
  if (ping1State_read != ping1State) {
      ping1State_read = ping1State;
      ++ping1Count;
  }
  if(ping1Count == 50)   {
  digitalWrite(relay_mikrocomputer_1, state_relay_mikrocomputer_1);
//  digitalWrite(relay_mikrocomputer_2, state_relay_mikrocomputer_2);
  state_relay_mikrocomputer_1?state_relay_mikrocomputer_1 = 0:state_relay_mikrocomputer_1 = 1;
//  state_relay_mikrocomputer_2?state_relay_mikrocomputer_2 = 0:state_relay_mikrocomputer_2 = 1;
  ping1Count = 0;
  }

//  if(state_relay_mikrocomputer_1) tm1637.display(0, 1);
//  if(!state_relay_mikrocomputer_1) tm1637.display(0, 0);
//  if(state_relay_mikrocomputer_2) tm1637.display(3, 1);
//  if(!state_relay_mikrocomputer_2) tm1637.display(3, 0);

  digitalWrite(LED_BUILTIN, ping1State);
  if(ping1State){
//   tm1637.point(POINT_OFF);
//  tm1637.point(true);
//  tm1637.display(0, 1);
  }
  if(!ping1State){
//   tm1637.point(POINT_ON);
//  tm1637.point(false);
//  tm1637.display(0, 0);
  }
  delay(100);


}






