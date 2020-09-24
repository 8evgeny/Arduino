 #include "TM1637.h"
 #include <OneWire.h>
 #include <LiquidCrystal.h>

 #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
 #include <LiquidCrystal_PCF8574.h>
 #include <DallasTemperature.h>

//****** ПАРАМЕТРЫ *********
// #define WAIT_PING 60000          // Время ожидания пинга (ms)

 #define WAIT_PING 6000000000       // Для тестов (ms)

 #define NUMBER_RESTART 3           // Колл попыток перезапуска при отсутствии пинга
 #define WAIT_POWER_ON 180000       // Ждем 3 мин прогрузки вычислителя после перезагрузки
 #define WAIT_PING_RESTART 1800000  // Ждем 30 мин потом пробуем опять ловить пинг

 #define TEMP_VERY_COLD -10         // Отключаем питание и греем
 #define TEMP_COLD 5                // (это минус 5 )Включаем подогрев и питание платы
 #define TEMP_HOT 5                 // Отключаем подогрев
 #define TEMP_VERY_HOT 80           // Отключаем питание и ждем
//**************************
 const int TEMP_UPDATE_TIME = 1000; // время ожидания ds1820
 int number_restart = NUMBER_RESTART;
 int restart = 0; //номер попытки рестарта (не менять)
 const int wait_ping = WAIT_PING;

//Распределяем пины данных ARDUINO
// Use analog pins as digital pins. A0 to A5 are D14 to D19.

//pins definitions for TM1637
 #define CLK 15 //A1
 #define DIO 14 //A0

//pins definitions for TM1602_I2C   Arduino Nano: A4-SDA A5-SCL (
//это зашито в Nano на уровне железа

 const int ping2 = 13;
 const int ping1 = 12;
 #define ONE_WIRE_BUS 11
 const int relay_board_1 = 10;

//pins definitions for LCD
 #define RS 9
 #define E 8
 #define DB4 7
 #define DB5 6
 #define DB6 5
 #define DB7 4

 const int relay_heater_cable = 3;
 const int relay_board_2 = 2;

// Создаем объекты для вывода на 4-значный дисплей и LCD
  TM1637 tm1637(CLK,DIO);
  LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7);

// Создаем объект OneWire для шины 1-Wire, с помощью которого будет осуществляться работа с датчиком
 OneWire ds1820(ONE_WIRE_BUS);
 DallasTemperature sensor(&ds1820);// Pass our oneWire reference to Dallas Temperature.

// LiquidCrystal_I2C lcd1(0x27,16,2); // Указываем I2C адрес (наиболее распространенное значение), а также параметры экрана (в случае LCD 1602 - 2 строки по 16 символов в каждой
 LiquidCrystal_PCF8574 lcd1(0x27);

 int ping1_A = 0;
 int ping1_B = 0;
 int ping2_A = 0;
 int ping2_B = 0;
 bool state_relay_board_1 = true;
 bool state_relay_board_2 = true;
 bool state_relay_heater_cable = true;
 bool very_cold;
 bool very_hot;
 bool COLD;
 bool HOT;
 bool ping_status = false;
 bool power_board1_on;
 unsigned long timeping;
 float tempSensor =0;
 int temp_minus;
 byte data[9]{B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B11111100,B10010000}; // это -55 градусов
// byte data[9]{B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000101,B01010000}; // это +85 градусов

 DeviceAddress insideThermometer, outsideThermometer; // arrays to hold device addresses

 bool checkHotAlarm(DeviceAddress deviceAddress){
  bool hot = false ;
  if (sensor.hasAlarm(deviceAddress)){
   lcd.setCursor(0, 0);
   lcd1.setCursor(0, 0);
   if(HOT){
   lcd.print("Hot!");
   lcd1.print("Hot!");
   hot = true;
   }
   if (COLD) hot = false;
  }
  return hot;
 }

 bool checkColdAlarm(DeviceAddress deviceAddress){
  bool cold = false;
  if (sensor.hasAlarm(deviceAddress)){
   lcd.setCursor(0, 0);
   lcd1.setCursor(0, 0);
   if(COLD){
    lcd.print("Cold!");
    lcd1.print("Cold!");
    cold = true;
   }
  if (HOT) cold = false;
  }

  return cold;
 }

 float receive_temp(){
  float temperat = 0;
  long lastUpdateTime = 0; // Переменная для хранения времени последнего считывания с датчика
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
    HOT = true;
    COLD = false;
    temperat =  ((data[7] << 8) | data[8]) * 0.0625;
   }
   if (temp_minus == 128){
    HOT = false;
    COLD = true;
    temperat =  ((~((data[7] << 8) | data[8]))+1) * 0.0625;
   }
  }
  return temperat;
 }

 void print_temperature_1637(float temper) {
   int8_t Digits[] = {0x00, 0x00, 0x00, 0x00};
   int KL1 = temper;
   int KL2 = (temper - KL1) * 100;
   if (KL1 > 99)(KL1 = temper - 100);
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

 void print_temperature_1602(float temper) {
       lcd.setCursor(0, 1);
       lcd1.setCursor(0, 1);
       if(HOT) {
           lcd.print("+");
           lcd1.print("+");
       }
       if(COLD) {
           lcd.print("-");
           lcd1.print("-");
       }
       lcd.print(temper);
       lcd1.print(temper);
   }

 bool checkPing(){
  bool pingstate;
  ping1_A = digitalRead(ping1);
  ping1_B = ping1_A;
  timeping = millis();
  pingstate = true;
  while(ping1_A == ping1_B) {
   ping1_A = digitalRead(ping1);
   lcd.setCursor(6, 0);
   lcd1.setCursor(6, 0);
   lcd.print(millis()/1000);
   lcd1.print(millis()/1000);
   if (millis() - timeping > wait_ping) {
    pingstate = false;
    break;
   }
  }
 return pingstate;
 }

 void powerCable(bool action){
  digitalWrite(relay_heater_cable, action);
  state_relay_heater_cable = action;
  lcd.setCursor(0, 0);
  lcd1.setCursor(0, 0);
  if(action){
      lcd.print("ON    ");
      lcd1.print("ON    ");
  }
  if(!action){
      lcd.print("OFF  ");
      lcd1.print("OFF  ");
  }
 }

 void powerBoard1(bool action){
     digitalWrite(relay_board_1, !action);
     state_relay_board_1 = action;
     lcd.setCursor(13, 0);
     lcd1.setCursor(13, 0);
     if(!action){
         power_board1_on = false;
         lcd.print("OFF");
         lcd1.print("OFF");
     }
     if(action){
         power_board1_on = true;
         lcd.print("ON ");
         lcd1.print("ON ");
     }
 }

 void powerBoard2(bool action){
//     digitalWrite(relay_board_2, !action);
//     state_relay_board_2 = action;
 }

 void setup()
  {
   delay(3000);
   lcd1.begin(16,2);
   lcd1.setBacklight(255);
   lcd1.home();
   lcd1.clear();

   tm1637.init();
   tm1637.set(BRIGHT_DARKEST); //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
   lcd.begin(16, 2);           // Задаем размерность экрана
   sensor.begin();
   sensor.getAddress(insideThermometer, 0);
   sensor.setHighAlarmTemp(insideThermometer, TEMP_VERY_HOT);
   sensor.setLowAlarmTemp(insideThermometer, TEMP_VERY_COLD);

   pinMode(LED_BUILTIN, OUTPUT);// initialize digital pin LED_BUILTIN as an output.
   pinMode(ping1, INPUT);
   pinMode(ping2, INPUT);
   pinMode(relay_board_1, OUTPUT);
   pinMode(relay_board_2, OUTPUT);
   pinMode(relay_heater_cable, OUTPUT);

   powerCable(1);
   powerBoard1(0);
   power_board1_on = false;
  }

 void loop()
 {

//Рабочий режим - на реле 2 светодиода горят - кабель выкл  плата - вкл
  tempSensor = receive_temp();
  print_temperature_1637(tempSensor);
  print_temperature_1602(tempSensor);
  very_cold = checkColdAlarm(insideThermometer);
  if(very_cold){ //действия при переохлаждении - греем и ждем
   powerCable(1); //светодиод на реле не горит - подогрев включен по умолчанию
   powerBoard1(0); //светодиод на реле не горит
   while(very_cold){
    tempSensor = receive_temp();
    print_temperature_1602(tempSensor);
    print_temperature_1637(tempSensor);
    very_cold = checkColdAlarm(insideThermometer);
   }
  }

  very_hot = checkHotAlarm(insideThermometer);
  if(very_hot){ //действия при перегреве
  powerCable(0);
  powerBoard1(0);
   while(very_cold){
    tempSensor = receive_temp();
    print_temperature_1602(tempSensor);
    print_temperature_1637(tempSensor);
    very_hot = checkHotAlarm(insideThermometer);
   }
  }

  if(COLD && (tempSensor < TEMP_COLD) && !very_cold){ //включаем питание платы
   powerBoard1(1);
  }

  if(COLD && (tempSensor > TEMP_COLD) && !very_cold){ //включаем подогрев и питание платы
   powerCable(1);
   powerBoard1(1);
  }

  if(HOT && (tempSensor < TEMP_HOT) && !very_hot){//включаем питание платы включаем кабель
   powerBoard1(1);
   powerCable(1);
  }

  if(HOT && (tempSensor > TEMP_HOT) && !very_hot){//выключаем подогрев и включаем питание платы
   powerCable(0);
   powerBoard1(1);
  }

// if (power_board1_on){

//  ping_status = checkPing();

//  if(ping_status ){ //пинг ok
//  lcd.setCursor(7, 1);
//  lcd.print("PingOK  ");
//  lcd1.setCursor(7, 1);
//  lcd1.print("PingOK  ");
//  }

//  digitalWrite(LED_BUILTIN, ping1_A); //светодиод на ардуине моргает по пингу

//  if(!ping_status){ //действия если нет пинга
//   lcd.setCursor(7, 1);
//   lcd.print("NoPing  ");
//   lcd1.setCursor(7, 1);
//   lcd1.print("NoPing  ");
//   tempSensor = receive_temp();
//   print_temperature_1602(tempSensor);
//   print_temperature_1637(tempSensor);
//   powerBoard1(0);
//   delay(5000);
//   ++restart;
//   if(number_restart == restart-1){
//      lcd.setCursor(7, 1);
//      lcd.print("wait ....");
//      lcd.print(restart);
//      lcd1.setCursor(7, 1);
//      lcd1.print("wait ....");
//      lcd1.print(restart);
//   delay(WAIT_PING_RESTART);

//  restart = 0;
//  }
//   lcd.setCursor(7, 1);
//   lcd.print("Restart");
//   lcd.print(restart);
//   lcd1.setCursor(7, 1);
//   lcd1.print("Restart");
//   lcd1.print(restart);
//  powerBoard1(1);
//  delay(WAIT_POWER_ON);//ждем прогрузки платы
//  }

//  if(ping_status && (restart!=0)){ //пинг появился
//  restart = 0;
//  lcd.setCursor(7, 1);
//  lcd.print("        ");
//  lcd1.setCursor(7, 1);
//  lcd1.print("        ");
//  }

// }

}

 void receive_temp_dallas(DeviceAddress deviceAddress){
    //На будущее - если подключать еще термодатчики
    sensor.requestTemperatures();
    tempSensor = sensor.getTempC(deviceAddress);
}

