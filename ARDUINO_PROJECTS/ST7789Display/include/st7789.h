#ifndef S7789_H
#define S7789_H

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Arduino SPI library

// ST7789 TFT module connections
#define TFT_CS     10
#define TFT_RST    8  // define reset pin, or set to -1 and connect to Arduino RESET pin
#define TFT_DC     9  // define data/command pin

void testdrawtext(char *text, uint16_t color);
void tftPrintTest();
void testlines(uint16_t color);
void testfastlines(uint16_t color1, uint16_t color2);
void testdrawrects(uint16_t color);
void testfillrects(uint16_t color1, uint16_t color2);
void testfillcircles(uint8_t radius, uint16_t color);
void testdrawcircles(uint8_t radius, uint16_t color);
void testtriangles();
void testroundrects();
void tftPrintTest();
void mediabuttons();

#endif
