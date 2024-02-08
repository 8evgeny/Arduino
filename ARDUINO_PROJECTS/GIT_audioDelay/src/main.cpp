//sudo avrdude -c  usbasp  -p m328p -P /dev/ttyS0 -b 19200 -U flash:w:blink.hex:i

//#include    "blink.h"
#include    <Arduino.h>
#define LED_STAND_PIN 13

unsigned long timeON = 0;
unsigned long timeOFF = 0;
unsigned long delayON = 500000;
unsigned long delayOFF = 2000000;

bool on = false;
uint8_t state = LOW;
void setup()
{
    pinMode(LED_STAND_PIN, OUTPUT);
    digitalWrite(LED_STAND_PIN, state);
}

void loop()
{
    if (( micros() >= timeON + delayON )&& (state == HIGH)){
        timeOFF = micros();
        state = LOW;
    }
    if (( micros() >= timeOFF + delayOFF ) && (state == LOW)){
        timeON = micros();
        state = HIGH;
    }

    digitalWrite(LED_STAND_PIN, state);
}
