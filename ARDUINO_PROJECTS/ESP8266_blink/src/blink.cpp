//sudo avrdude -c  usbasp  -p m328p -P /dev/ttyS0 -b 19200 -U flash:w:blink.hex:i

#include    "blink.h"

#define LED_STAND_PIN 13

unsigned long time = 0;
//unsigned long DELAY = 1000000;
unsigned long DELAY = 300000;
bool on = false;

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void setup()
{
    pinMode(LED_STAND_PIN, OUTPUT);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void loop()
{
    if ( micros() >= time + DELAY )
    {
        time = micros();
        on = !on;
    }

    uint8_t state = on ? HIGH : LOW;

    digitalWrite(LED_STAND_PIN, state);
}
