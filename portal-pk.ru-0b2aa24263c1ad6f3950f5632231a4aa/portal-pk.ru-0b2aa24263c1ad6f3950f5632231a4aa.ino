#include "TM1637.h"
int8_t DispMSG[] = {1, 2, 3, 4};
//Определяем пины для подключения к плате Arduino
#define CLK 3
#define DIO 2
//Создаём объект класса TM1637, в качестве параметров
//передаём номера пинов подключения
TM1637 tm1637(CLK, DIO);

void setup()
{
  //Инициализация модуля
  tm1637.init();
  //Установка яркости горения сегментов
  /*
     BRIGHT_TYPICAL = 2 Средний
     BRIGHT_DARKEST = 0 Тёмный
     BRIGHTEST = 7      Яркий
  */
  tm1637.set(BRIGHT_TYPICAL);
}

void loop() 
{
  //Задание на включение разделителя
  tm1637.point(true);
  //Выводим массива на дисплей порязрядно
  tm1637.display(0, DispMSG[0]);
  tm1637.display(1, DispMSG[1]);
  tm1637.display(2, DispMSG[2]);
  tm1637.display(3, DispMSG[3]);
  //Задержка
  delay(1000);
  //Задание на выключение разделителя
  tm1637.point(false);
  //Выводим массива на дисплей порязрядно
  tm1637.display(0, DispMSG[0]);
  tm1637.display(1, DispMSG[1]);
  tm1637.display(2, DispMSG[2]);
  tm1637.display(3, DispMSG[3]);
  //Задержка
  delay(1000);
}
