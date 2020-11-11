#include "TM1637.h"
#include <OneWire.h>
#include <LiquidCrystal.h>

#include <Wire.h>
// #include <LiquidCrystal_I2C.h>

#include <LiquidCrystal_PCF8574.h>
#include <DallasTemperature.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

//****** ПАРАМЕТРЫ *********
// #define WAIT_PING 60000          // Время ожидания пинга (ms)

#define WAIT_PING 6000000000       // Для тестов (ms)

#define NUMBER_RESTART 3           // Колл попыток перезапуска при отсутствии пинга
#define WAIT_POWER_ON 180000       // Ждем 3 мин прогрузки вычислителя после перезагрузки
#define WAIT_PING_RESTART 1800000  // Ждем 30 мин потом пробуем опять ловить пинг

//#define TEMP_VERY_COLD 30
#define TEMP_COLD 15               // Температура включения подогрева (минус)
#define DELTA 5                    // Дельта
// Подогрев -15гр. и ниже - включается  -10гр. - отключается
#define TEMP_TEST 80               // Температура тестового подогрева

#define TEMP_START 20              // Температура старта (минус)

// Старт платы -20 гр. и выше  -25 гр и ниже - останов платы

//#define TEMP_HOT 2
#define TEMP_VERY_HOT 95
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
LiquidCrystal_PCF8574 lcd1(0x27); //Адрес на шине I2C: 0x3F или 0x27 (зависит от модификации чипа)
//LiquidCrystal_PCF8574 lcd1(0x3F);

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

// Принудительно включаем подогрев
// digitalWrite(relay_heater_cable, 1);
// state_relay_heater_cable = 1;

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


void testdrawline() {
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for(i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000); // Pause for 2 seconds
}

void testdrawrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testfillrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testdrawcircle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillcircle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(2000);
}

void testdrawroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawtriangle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfilltriangle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(2000);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
//    Serial.print(F("x: "));
//    Serial.print(icons[f][XPOS], DEC);
//    Serial.print(F(" y: "));
//    Serial.print(icons[f][YPOS], DEC);
//    Serial.print(F(" dy: "));
//    Serial.println(icons[f][DELTAY], DEC);
  }

  for(;;) { // Loop forever...
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display(); // Show the display buffer on the screen
    delay(200);        // Pause for 1/10 second

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }


}

void OLED_print(){
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 0);
  if(state_relay_heater_cable) display.print("ON");
  if(!state_relay_heater_cable) display.print("OFF");

  display.setCursor(110, 0);
  if(state_relay_board_1) display.print("ON");
  if(!state_relay_board_1) display.print("OFF");

  display.setCursor(40, 0);
  display.print(millis()/1000);
  display.setTextSize(2);
  display.setCursor(10, 15);
  if(HOT) {
   display.print("+");
  }
  if(COLD) {
   display.print("-");
  }
  display.print(tempSensor);

  display.display();      // Show initial text

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
//  sensor.setHighAlarmTemp(insideThermometer, TEMP_VERY_HOT);
//  sensor.setLowAlarmTemp(insideThermometer, TEMP_VERY_COLD*(-1));

  pinMode(LED_BUILTIN, OUTPUT);// initialize digital pin LED_BUILTIN as an output.
  pinMode(ping1, INPUT);
  pinMode(ping2, INPUT);
  pinMode(relay_board_1, OUTPUT);
  pinMode(relay_board_2, OUTPUT);
  pinMode(relay_heater_cable, OUTPUT);

  powerCable(1);
  powerBoard1(0);
  power_board1_on = false;

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
//  display.display();
//  delay(2000); // Pause for 2 seconds
  // Clear the buffer
//  display.clearDisplay();
//  display.drawPixel(10, 10, SSD1306_WHITE);
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
//  display.display();
//  delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...
//  testdrawline();      // Draw many lines
//  testdrawrect();      // Draw rectangles (outlines)
//  testfillrect();      // Draw rectangles (filled)
//  testdrawcircle();    // Draw circles (outlines)
//  testfillcircle();    // Draw circles (filled)
//  testdrawroundrect(); // Draw rounded rectangles (outlines)
//  testfillroundrect(); // Draw rounded rectangles (filled)
//  testdrawtriangle();  // Draw triangles (outlines)
//  testfilltriangle();  // Draw triangles (filled)
//  testdrawchar();      // Draw characters of the default font
//  testdrawstyles();    // Draw 'stylized' characters
//  testscrolltext();    // Draw scrolling text
//  testdrawbitmap();    // Draw a small bitmap image
  // Invert and restore display, pausing in-between
//  display.invertDisplay(true);
//  delay(1000);
//  display.invertDisplay(false);
//  delay(1000);
//  testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
  display.clearDisplay();
  display.setTextSize(2);
 }

void loop()
{

//Рабочий режим - на реле 2 светодиода горят - кабель выкл  плата - вкл
 tempSensor = receive_temp();
 print_temperature_1637(tempSensor);
 print_temperature_1602(tempSensor);

//if( COLD && tempSensor > TEMP_VERY_COLD) very_cold = true;
//if( COLD && tempSensor <= TEMP_VERY_COLD) very_cold = false;

//  very_cold = checkColdAlarm(insideThermometer);
//  if(very_cold){ //действия при переохлаждении - греем и ждем
//   powerCable(1); //светодиод на реле не горит - подогрев включен по умолчанию
//   powerBoard1(0); //светодиод на реле не горит
//   while(very_cold){
//    tempSensor = receive_temp();
//    print_temperature_1602(tempSensor);
//    print_temperature_1637(tempSensor);
//    very_cold = checkColdAlarm(insideThermometer);
//   }
//  }

//if( HOT && tempSensor > TEMP_VERY_HOT) very_hot = true;
//if( HOT && tempSensor <= TEMP_VERY_HOT) very_hot = false;

//  very_hot = checkHotAlarm(insideThermometer);
//  if(very_hot){ //действия при перегреве
//  powerCable(0);
//  powerBoard1(0);
//   while(very_hot){
//    tempSensor = receive_temp();
//    print_temperature_1602(tempSensor);
//    print_temperature_1637(tempSensor);
//    very_hot = checkHotAlarm(insideThermometer);
//   }
//  }


  if(COLD && tempSensor > TEMP_COLD) {
    powerCable(1);
  }

  if(COLD && tempSensor < (TEMP_COLD - DELTA)) {
    powerCable(0);
  }

  if(HOT) {
    powerCable(0);
  }

    //тестовый подогрев
    //if(HOT && tempSensor < TEMP_TEST - 1) {
    //    powerCable(1);
    //}
    //if(HOT && tempSensor > TEMP_TEST) {
    //    powerCable(0);
    //}


  if(COLD && tempSensor > TEMP_START + DELTA) {
    powerBoard1(0);
  }

  if(COLD && tempSensor < TEMP_START) {
    powerBoard1(1);
  }

  if(HOT && tempSensor < TEMP_VERY_HOT - DELTA) {
    powerBoard1(1);
  }

  if(HOT && tempSensor > TEMP_VERY_HOT) {
    powerBoard1(0);
  }


// if(very_cold ){
//  powerCable(1);
//  powerBoard1(0);
// }

// if(COLD && (tempSensor > TEMP_COLD) && !very_cold){
//  powerCable(1);
//  powerBoard1(0);
// }

// if(COLD && tempSensor < TEMP_COLD) {
//  powerBoard1(1);
//  powerCable(1);
// }

// if(HOT && tempSensor < TEMP_HOT) {
//  powerCable(1);
//  powerBoard1(1);
// }

// if(HOT && (tempSensor > TEMP_HOT) && !very_hot){//выключаем подогрев и включаем питание платы
//  powerCable(0);
//  powerBoard1(1);
// }

// if(very_hot ){
//  powerCable(0);
//  powerBoard1(0);
// }


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

  lcd.setCursor(6, 0);
  lcd1.setCursor(6, 0);
  lcd.print(millis()/1000);
  lcd1.print(millis()/1000);


 OLED_print();


}

void receive_temp_dallas(DeviceAddress deviceAddress){
   //На будущее - если подключать еще термодатчики
   sensor.requestTemperatures();
   tempSensor = sensor.getTempC(deviceAddress);
}








