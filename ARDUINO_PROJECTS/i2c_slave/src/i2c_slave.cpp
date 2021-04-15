/* sudo avrdude -c  usbasp  -p m328p -P /dev/ttyS0 -b 19200 -U
 * flash:w:Ping_M4_T4.hex:i */

// D3 - реле 1 - M4
// D10 - реле 2 - T4
#include <LiquidCrystal.h>
#include <OneWire.h>
//#include <Vector.h>
#include <Wire.h>
#include <string.h>

using namespace std;
#include "TM1637.h"
// #include <LiquidCrystal_I2C.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_PCF8574.h>
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(OLED_RESET);

//****** ПАРАМЕТРЫ *********
// #define WAIT_PING 60000          // Время ожидания пинга (ms)

#define WAIT_PING 60000  //  (ms)

#define NUMBER_RESTART 3  // Колл попыток перезапуска при отсутствии пинга
#define WAIT_POWER_ON \
  180000  // Ждем 3 мин прогрузки вычислителя после перезагрузки
#define WAIT_PING_RESTART \
  1800000  // Ждем 30 мин потом пробуем опять ловить пинг

//#define TEMP_VERY_COLD 30
#define TEMP_COLD 15  // Температура включения подогрева (минус)
#define DELTA 5  // Дельта
// Подогрев -15гр. и ниже - включается  -10гр. - отключается
#define TEMP_TEST 80  // Температура тестового подогрева

#define TEMP_START 20  // Температура старта (минус)

// Старт платы -20 гр. и выше  -25 гр и ниже - останов платы

//#define TEMP_HOT 2
#define TEMP_VERY_HOT 110
//**************************
const int TEMP_UPDATE_TIME = 1000;  // время ожидания ds1820
int number_restart = NUMBER_RESTART;
int restart = 0;  //номер попытки рестарта (не менять)
const int wait_ping = WAIT_PING;

//Распределяем пины данных ARDUINO
// Use analog pins as digital pins. A0 to A5 are D14 to D19.

// pins definitions for TM1637
#define CLK 15  // A1
#define DIO 14  // A0

// pins definitions for TM1602_I2C   Arduino Nano: A4-SDA A5-SCL (
//это зашито в Nano на уровне железа

const int ping2 = 13;
const int ping1 = 12;
#define ONE_WIRE_BUS 11
const int relay_board_1 = 7;  // 7 - заглушка
const int relay_board_2 = 7;  // 7 - заглушка
const int relay_ping_1 = 3;
const int relay_ping_2 = 10;

// pins definitions for LCD
//#define RS 9
//#define E 8
//#define DB4 7
//#define DB5 6
//#define DB6 5
//#define DB7 4

const int relay_heater_cable = 7;  // 7 - заглушка

// Создаем объекты для вывода на 4-значный дисплей и LCD
TM1637 tm1637(CLK, DIO);
// LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7);

// Создаем объект OneWire для шины 1-Wire, с помощью которого будет
// осуществляться работа с датчиком
OneWire ds1820(ONE_WIRE_BUS);
DallasTemperature sensor(
    &ds1820);  // Pass our oneWire reference to Dallas Temperature.

// LiquidCrystal_I2C lcd1(0x27,16,2); // Указываем I2C адрес (наиболее
// распространенное значение), а также параметры экрана (в случае LCD 1602 - 2
// строки по 16 символов в каждой

LiquidCrystal_PCF8574 lcd1(0x27);

//Адрес на шине I2C: 0x3F или 0x27 (зависит от модификации чипа)
// LiquidCrystal_PCF8574 lcd1(0x3F);

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
bool ping_status1 = false;
bool ping_status2 = false;
bool ping_change1 = false;
bool ping_change2 = false;
bool power_board1_on;
bool power_board2_on;
unsigned long timechangePing1;
unsigned long timechangePing2;
unsigned long timerestart1;
unsigned long timerestart2;
unsigned long currtime;

float tempSensor = 0;
int temp_minus;
byte data[9]{B00000000, B00000000, B00000000, B00000000, B00000000,
             B00000000, B00000000, B11111100, B10010000};  // это -55 градусов
// byte
// data[9]{B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000101,B01010000};
// // это +85 градусов

byte replay[8]{1, 2, 3, 4, 5, 6, 7, 8};  //Ответ Хадасу

String stringToKhadas{"Hello Khadas!!! "};
String stringFromKhadas;
int numReceiveByte = 0;
// Vector<char> dataFromKhadas;
char dataFromKhadas[128]{41, 42, 43, 44, 45, 46, 47, 48,
                         49, 50, 51, 52, 53, 54, 55, 56};

DeviceAddress insideThermometer,
    outsideThermometer;  // arrays to hold device addresses

void restart_1() {
  digitalWrite(relay_ping_1, 1);
  auto t = millis();
  while ((millis() - 3000) < t) {
  }
  digitalWrite(relay_ping_1, 0);
  timerestart1 = millis();
}
void restart_2() {
  digitalWrite(relay_ping_2, 1);
  auto t = millis();
  while ((millis() - 3000) < t) {
  }
  digitalWrite(relay_ping_2, 0);
  timerestart2 = millis();
}

bool checkHotAlarm(DeviceAddress deviceAddress) {
  bool hot = false;
  if (sensor.hasAlarm(deviceAddress)) {
    //  lcd.setCursor(0, 0);
    lcd1.setCursor(0, 0);
    if (HOT) {
      //  lcd.print("Hot!");
      lcd1.print("Hot!");
      hot = true;
    }
    if (COLD) hot = false;
  }
  return hot;
}

bool checkColdAlarm(DeviceAddress deviceAddress) {
  bool cold = false;
  if (sensor.hasAlarm(deviceAddress)) {
    //  lcd.setCursor(0, 0);
    lcd1.setCursor(0, 0);
    if (COLD) {
      //   lcd.print("Cold!");
      lcd1.print("Cold!");
      cold = true;
    }
    if (HOT) cold = false;
  }

  return cold;
}

float receive_temp() {
  float temperat = 0;
  long lastUpdateTime =
      0;  // Переменная для хранения времени последнего считывания с датчика
  ds1820.reset();  // Начинаем взаимодействие со сброса всех предыдущих команд и
                   // параметров
  ds1820.write(0xCC);  // Даем датчику DS18b20 команду пропустить поиск по
                       // адресу. В нашем случае только одно устрйоство
  ds1820.write(0x44);  // Даем датчику DS18b20 команду измерить температуру.
                       // Само значение температуры мы еще не получаем - датчик
                       // его положит во внутреннюю память
  if (millis() - lastUpdateTime >
      TEMP_UPDATE_TIME) {  //Если прошло установленное время - забираем
                           //результат
    lastUpdateTime = millis();
    ds1820
        .reset();  // Теперь готовимся получить значение измеренной температуры
    ds1820.write(0xCC);
    ds1820.write(0xBE);  // Просим передать нам значение регистров со значением
                         // температуры
    // Получаем и считываем ответ
    data[8] = ds1820.read();  // Читаем младший байт значения температуры
    data[7] = ds1820.read();  // Читаем старший байт значения температуры
    // Формируем температуру для вывода: "склеиваем" значения,затем умножаем на
    // коэффициент, (для 12 бит по умолчанию - это 0,0625)
    temp_minus = 0;
    temp_minus = data[7] & B10000000;

    if (temp_minus != 128) {
      HOT = true;
      COLD = false;
      temperat = ((data[7] << 8) | data[8]) * 0.0625;
    }
    if (temp_minus == 128) {
      HOT = false;
      COLD = true;
      temperat = ((~((data[7] << 8) | data[8])) + 1) * 0.0625;
    }
  }
  return temperat;
}

void print_temperature_1637(float temper) {
  //  int8_t Digits[] = {0x00, 0x00, 0x00, 0x00};
  int8_t Digits[] = {0x01, 0x01, 0x02, 0x03};
  int KL1 = temper;
  int KL2 = (temper - KL1) * 100;
  if (KL1 > 99) (KL1 = temper - 100);
  //  Digits[0] = (KL1 / 10);  // раскидываем 4-значное число на цифры
  //  Digits[1] = (KL1 % 10);
  //  Digits[2] = (KL2 / 10);
  //  Digits[3] = (KL2 % 10);
  tm1637.display(0, Digits[0]);

  tm1637.display(1, Digits[1]);
  tm1637.display(2, Digits[2]);
  tm1637.display(3, Digits[3]);
  tm1637.point(ping1_A);  // ping
}

void print_ping_1637() {
  int8_t Digits[] = {0x00, 0x00, 0x00, 0x00};
  int KL1 = millis() / 1000;

  if (KL1 > 59) (KL1 = millis() / 1000 - 60);
  int KL2 = KL1 / 60;
  Digits[0] = (KL1 / 10);  // раскидываем 4-значное число на цифры
  Digits[1] = (KL1 % 10);
  //  Digits[3] = (KL2 / 10);
  //  Digits[3] = (KL2 % 10);
  //  Digits[2] = stringFromKhadas.length();
  //  Digits[2] = numReceiveByte;
  Digits[2] = dataFromKhadas[0];
  Digits[3] = dataFromKhadas[1];
  //  Digits[0] = ping1_B;
  //  Digits[1] = 0;
  //  Digits[2] = 0;
  //  Digits[3] = ping2_B;
  //  tm1637.init();
  tm1637.display(2, Digits[0]);
  tm1637.display(3, Digits[1]);
  tm1637.display(0, Digits[2]);
  tm1637.display(1, Digits[3]);
  tm1637.point((millis() / 1000) % 2);
}

void print_temperature_1602(float temper) {
  //      lcd.setCursor(0, 1);
  lcd1.setCursor(0, 1);
  if (HOT) {
    //          lcd.print("+");
    lcd1.print("+");
  }
  if (COLD) {
    //          lcd.print("-");
    lcd1.print("-");
  }
  //      lcd.print(temper);
  lcd1.print(temper);
}

void printTime_lcd1() {
  lcd1.setCursor(5, 0);
  lcd1.print(millis() / 1000);
}

void printKhadasData() {
  lcd1.setCursor(0, 1);
  for (int i = 0; i < 16; ++i) {
    lcd1.print(dataFromKhadas[i]);
  }
}

bool changePing1() {  //проверяем изменение состояния 1 пинга
  ping1_A = digitalRead(ping1);
  if (ping1_A == ping1_B) return false;
  if (ping1_A != ping1_B) {
    ping1_B = ping1_A;
    timechangePing1 = millis();
    return true;
  }
}

bool changePing2() {  //проверяем изменение состояния 2 пинга
  ping2_A = digitalRead(ping2);
  if (ping2_A == ping2_B) return false;
  if (ping2_A != ping2_B) {
    ping2_B = ping2_A;
    timechangePing2 = millis();
    return true;
  }
}

void powerCable(bool action) {
  digitalWrite(relay_heater_cable, action);
  state_relay_heater_cable = action;

  // lcd1.setCursor(0, 0);
  // if(action){
  //     lcd1.print("ON    ");
  // }
  // if(!action){
  //     lcd1.print("OFF  ");
  // }
}

void powerBoard1(bool action) {
  digitalWrite(relay_board_1, !action);
  state_relay_board_1 = action;
}

void powerBoard2(bool action) {
  digitalWrite(relay_board_2, !action);
  state_relay_board_2 = action;
}

void OLED_print() {
  display.clearDisplay();
  display.setTextSize(1);  // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 0);
  if (state_relay_heater_cable) display.print("ON");
  if (!state_relay_heater_cable) display.print("OFF");

  display.setCursor(110, 0);
  if (state_relay_board_1) display.print("ON");
  if (!state_relay_board_1) display.print("OFF");

  display.setCursor(110, 12);
  if (state_relay_board_2) display.print("ON");
  if (!state_relay_board_2) display.print("OFF");

  display.setCursor(50, 0);
  display.print(millis() / 1000);

  display.setTextSize(2);
  display.setCursor(10, 12);
  display.print(ping1_A);
  display.print("  ");
  display.print(ping2_A);

  display.setTextSize(1);
  display.setCursor(80, 24);
  if (HOT) {
    display.print("+");
  }
  if (COLD) {
    display.print("-");
  }
  display.print(tempSensor);

  display.display();  // Show initial text
}

void lcd1_print() {
  printTime_lcd1();
  lcd1.setCursor(0, 0);
  if (power_board1_on) lcd1.print("OFF");
  if (!power_board1_on) lcd1.print("ON ");

  lcd1.setCursor(13, 0);
  if (!power_board1_on) lcd1.print("ON    ");
  if (power_board1_on) lcd1.print("OFF  ");

  //  lcd1.setCursor(0, 1);
  //  if (ping1_B == 1) lcd1.print("1");
  //  if (ping1_B == 0) lcd1.print("0");

  //  lcd1.setCursor(13, 1);
  //  if (ping2_B == 1) lcd1.print("1");
  //  if (ping2_B == 0) lcd1.print("0");
}

//   Регистрирует функцию, которая будет вызываться, когда мастер запрашивает
//   данные от ведомого устройства
void requestEvent() {
  //  char c;
  //  Wire.write(c);  // ответить сообщением
  String rep = String(replay[0]) + String(replay[1]) + String(replay[2]) +
               String(replay[3]) + String(replay[4]) + String(replay[5]) +
               String(replay[6]) + String(replay[7]) + " hellow ";

  //  Wire.write(rep.c_str());  // ответ в Хадас
  Wire.write(stringToKhadas.c_str());  // 16 байт ответа в Хадас

  //  auto rep = " ";
  //  auto rr = (char*)rep;
  //  Wire.write(data[7]);
}

//   функция, которая будет выполняться всякий раз, когда от мастера принимаются
//   данные данная функция регистрируется как обработчик события
void receiveEvent(int howMany) {
  char c;
  numReceiveByte = 0;
  //  stringFromKhadas = Wire.readString();
  int i = 0;
  while (Wire.available()) {
    c = Wire.read();        // принять байт как символ
                            //    dataFromKhadas.push_back(c);
    dataFromKhadas[i] = c;  //в массиве данные от Хадаса
    ++numReceiveByte;
    ++i;
  }
}

void setup() {
  delay(3000);
  lcd1.begin(16, 2);
  lcd1.setBacklight(255);
  lcd1.home();
  lcd1.clear();

  Wire.begin(0x0c);

  Wire.onRequest(requestEvent);  // обработчик события запрос данных от master
  Wire.onReceive(receiveEvent);  // обработчик события прием данных от master

  tm1637.init();
  tm1637.set(BRIGHT_DARKEST);
  // BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  //  lcd.begin(16, 2);           // Задаем размерность экрана

  sensor.begin();
  sensor.getAddress(insideThermometer, 0);

  //  sensor.setHighAlarmTemp(insideThermometer, TEMP_VERY_HOT);
  //  sensor.setLowAlarmTemp(insideThermometer, TEMP_VERY_COLD*(-1));

  pinMode(LED_BUILTIN,
          OUTPUT);  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ping1, INPUT);
  pinMode(ping2, INPUT);
  pinMode(relay_board_1, OUTPUT);
  pinMode(relay_board_2, OUTPUT);
  pinMode(relay_ping_1, OUTPUT);
  pinMode(relay_ping_2, OUTPUT);
  pinMode(relay_heater_cable, OUTPUT);

  powerCable(1);
  powerBoard1(0);
  powerBoard2(0);
  power_board1_on = false;
  power_board2_on = false;

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);

  timechangePing1 = millis();
  timechangePing2 = millis();
  digitalWrite(relay_ping_1, 0);
  digitalWrite(relay_ping_2, 0);
  timerestart1 = millis();
  timerestart2 = millis();
}

void setupMaster() {
  currtime = millis();
  Wire.begin();  // Мастер

  //  lcd1.begin(16, 2);
  //  lcd1.setBacklight(255);
  //  lcd1.home();
  //  lcd1.clear();

  //  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //  display.clearDisplay();
  //  display.setTextSize(2);
}

void setupSlave() {
  Wire.begin(0x0c);  // Slave
  Wire.onRequest(requestEvent);  // обработчик события запрос данных от master
  Wire.onReceive(receiveEvent);  // обработчик события прием данных от master
}

void loop() {
  auto miliss = millis();
  setupSlave();
  //  print_ping_1637();

  if ((millis() - currtime) > 1000) {
    setupMaster();
    print_ping_1637();
    printKhadasData();
    lcd1_print();
  }
}

void receive_temp_dallas(DeviceAddress deviceAddress) {
  //На будущее - если подключать еще термодатчики
  sensor.requestTemperatures();
  tempSensor = sensor.getTempC(deviceAddress);
}
