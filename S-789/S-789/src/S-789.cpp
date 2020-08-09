#include "TM1637.h"
#include <OneWire.h>
#include <LiquidCrystal.h>
#include <DallasTemperature.h>

const int relay_heater_cable = 13;
const int ping1 = 12;
#define ONE_WIRE_BUS 11
//const int ping2 = 11;
const int relay_board_1 = 10;
//const int relay_mikrocomputer_2 = 9;
#define RS 9 //pins definitions for 1602
#define E 8
#define DB4 7
#define DB5 6
#define DB6 5
#define DB7 4
#define CLK 3 //pins definitions for TM1637
#define DIO 2

TM1637 tm1637(CLK,DIO);
OneWire ds1820(ONE_WIRE_BUS);// Создаем объект OneWire для шины 1-Wire, с помощью которого будет осуществляться работа с датчиком
DallasTemperature sensor(&ds1820);// Pass our oneWire reference to Dallas Temperature.
LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7);

int ping1_A = 0;
int ping1_B = 0;
int ping1_count = 0;

int ping2StateA = 0;
int ping2StateB = 0;

//bool state_relay_mikrocomputer_1 = true;
//bool state_relay_mikrocomputer_2 = true;
//bool state_relay_heater_cable = true;
bool very_cold;
bool very_hot;
bool ping_status;
unsigned long timeping;
float tempSensor =0;
int temp_minus;
byte data[9]{B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B11111100,B10010000}; // это -55 градусов
//byte data[9]{B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000101,B01010000}; // это +85 градусов
DeviceAddress insideThermometer, outsideThermometer; // arrays to hold device addresses

 bool checkHotAlarm(DeviceAddress deviceAddress){
  bool hot;
  if (sensor.hasAlarm(deviceAddress)){
   lcd.setCursor(0, 0);
   if(temp_minus != 128){
   lcd.print("Hot!");
   hot = true;
   }
  }
  if (temp_minus == 128) hot = false;
  if (!sensor.hasAlarm(deviceAddress)){
   lcd.setCursor(0, 0);
   lcd.print("          ");
  }
  return hot;
 }

 bool checkColdAlarm(DeviceAddress deviceAddress){
  bool cold;
  if (sensor.hasAlarm(deviceAddress)){
   lcd.setCursor(0, 0);
   if(temp_minus == 128){
    lcd.print("Cold!");
    cold = true;
   }
  }
  if (temp_minus != 128) cold = false;
  if (!sensor.hasAlarm(deviceAddress)){
   lcd.setCursor(0, 0);
   lcd.print("          ");
  }
  return cold;
 }

 void receive_temp(){
  long lastUpdateTime = 0; // Переменная для хранения времени последнего считывания с датчика
  const int TEMP_UPDATE_TIME = 1000; // Определяем периодичность проверок
  ds1820.reset(); // Начинаем взаимодействие со сброса всех предыдущих команд и параметров
  ds1820.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство
  ds1820.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
  if (millis() - lastUpdateTime > TEMP_UPDATE_TIME){ //Если прошло установленное время - забираем результат
   lastUpdateTime = millis();
   ds1820.reset(); // Теперь готовимся получить значение измеренной температуры
   ds1820.write(0xCC);
   ds1820.write(0xBE); // Просим передать нам значение регистров со значением температуры
   // Получаем и считываем ответ
   data[8] = ds1820.read(); // Читаем младший байт значения температуры
   data[7] = ds1820.read(); // Читаем старший байт значения температуры
   // Формируем температуру для вывода: "склеиваем" значения,затем умножаем на коэффициент, (для 12 бит по умолчанию - это 0,0625)
   temp_minus = 0;
   temp_minus = data[7] & B10000000;
   if (temp_minus != 128){
    tempSensor =  ((data[7] << 8) | data[8]) * 0.0625;
   }
   if (temp_minus == 128){
    tempSensor =  ((~((data[7] << 8) | data[8]))+1) * 0.0625;
   }
  }
 }

 void print_temperature_1637() {
      int8_t Digits[] = {0x00, 0x00, 0x00, 0x00};
      int KL1 = tempSensor;
      int KL2 = (tempSensor - KL1) * 100;
      if (KL1 > 99)(KL1 = tempSensor - 100);

      Digits[0] = (KL1 / 10); // раскидываем 4-значное число на цифры
      Digits[1] = (KL1 % 10);
      Digits[2] = (KL2 / 10);
      Digits[3] = (KL2 % 10);

      tm1637.display(0,Digits[0]);
      tm1637.display(1,Digits[1]);
      tm1637.display(2,Digits[2]);
      tm1637.display(3,Digits[3]);

      tm1637.point(ping1_A); //ping
   }

 void print_temperature_1602() {
       lcd.setCursor(0, 1);
       if(temp_minus != 128) lcd.print("+");
       if(temp_minus == 128) lcd.print("-");
       lcd.print(tempSensor);

       if (ping_status){
        lcd.setCursor(9, 1);
        lcd.print("PingOK");
       }
       else{
        lcd.setCursor(9, 1);
        lcd.print("NoPing");
       }
   }

 bool checkPing(){
  bool pingstate;
  const int MAX_PING_TIME = 3000; // Время ожидания пинга
  ping1_A = digitalRead(ping1);
  ping1_B = ping1_A;
  timeping = millis();
  pingstate = true;
  while(ping1_A == ping1_B) {
   ping1_A = digitalRead(ping1);
   lcd.setCursor(6, 0);
   lcd.print(millis()/1000);
   if (millis() - timeping > MAX_PING_TIME) {
    pingstate = false;
    break;
   }
  }
 return pingstate;
 }

 void heartCable(bool action){
  digitalWrite(relay_heater_cable, action);
 }

 void powerBoard1(bool action){
     digitalWrite(relay_board_1, action);
 }

 void setup()
  {
   tm1637.init();
   tm1637.set(BRIGHT_DARKEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
   lcd.begin(16, 2);          // Задаем размерность экрана
   sensor.begin();
   sensor.getAddress(insideThermometer, 0);
   sensor.setHighAlarmTemp(insideThermometer, 40);
   sensor.setLowAlarmTemp(insideThermometer, -1);

   pinMode(LED_BUILTIN, OUTPUT);// initialize digital pin LED_BUILTIN as an output.
   pinMode(ping1, INPUT);
//   pinMode(ping2, INPUT);
   pinMode(relay_board_1, OUTPUT);
//   pinMode(relay_mikrocomputer_2, OUTPUT);
//   pinMode(relay_heater_cable, OUTPUT);

  }

 void loop()
 {
  receive_temp();
  print_temperature_1637();
  print_temperature_1602();
  very_hot = checkHotAlarm(insideThermometer);
  very_cold = checkColdAlarm(insideThermometer);
  ping_status = checkPing();

  heartCable(1);
  heartCable(0);
  powerBoard1(1);
  powerBoard1(0);

 digitalWrite(LED_BUILTIN, ping1_A); //светодиод на ардуине моргает по пингу

}

 void receive_temp_dallas(DeviceAddress deviceAddress){
    //На будущее - если подключать еще термодатчики
    sensor.requestTemperatures();
    tempSensor = sensor.getTempC(deviceAddress);
}

