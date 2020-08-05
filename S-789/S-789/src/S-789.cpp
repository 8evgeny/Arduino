#include "TM1637.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#define CLK 3//pins definitions for TM1637 and can be changed to other ports
#define DIO 2
const int ping1 = 12;
const int ping2 = 11;
const int relay_mikrocomputer_1 = 10;
const int relay_mikrocomputer_2 = 9;
const int relay_heater_cable = 8;
int ping1State = 0;
int ping1State_read = 0;
int ping1Count = 0;
int ping2State = 0;
int ping2State_read = 0;
int ping2Count = 0;
bool state_relay_mikrocomputer_1=true;
bool state_relay_mikrocomputer_2=true;
bool state_relay_heater_cable=true;
TM1637 tm1637(CLK,DIO);



void setup()
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ping1, INPUT);
  pinMode(ping2, INPUT);
  pinMode(relay_mikrocomputer_1, OUTPUT);
  pinMode(relay_mikrocomputer_2, OUTPUT);
  pinMode(relay_heater_cable, OUTPUT);
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
}

int8_t DispMSG[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
void loop()
{
  ping1State = digitalRead(ping1);

  if (ping1State_read != ping1State) {
      ping1State_read = ping1State;
      ++ping1Count;
  }
  if(ping1Count == 50)   {
  digitalWrite(relay_mikrocomputer_1, state_relay_mikrocomputer_1);
  digitalWrite(relay_mikrocomputer_2, state_relay_mikrocomputer_2);
  state_relay_mikrocomputer_1?state_relay_mikrocomputer_1 = 0:state_relay_mikrocomputer_1 = 1;
  state_relay_mikrocomputer_2?state_relay_mikrocomputer_2 = 0:state_relay_mikrocomputer_2 = 1;
  ping1Count = 0;
  }
//  tm1637.point(state_relay_mikrocomputer_1);//включение разделителя

  if(state_relay_mikrocomputer_1) tm1637.display(2, 1);
  if(!state_relay_mikrocomputer_1) tm1637.display(2, 0);
  if(state_relay_mikrocomputer_2) tm1637.display(3, 1);
  if(!state_relay_mikrocomputer_2) tm1637.display(3, 0);

  digitalWrite(LED_BUILTIN, ping1State);
  if(ping1State){
  tm1637.point(true);
  tm1637.display(0, 1);
  }
  if(!ping1State){
  tm1637.point(false);
  tm1637.display(0, 0);
  }
  delay(100);


}






