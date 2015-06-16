#include "fun.h"

void flash_cursor(Adafruit_TFTLCD * tft, int16_t x, int16_t y, uint8_t times)
{
  for(int i = 0; i < times; i++) {
    tft->drawChar(x, y, '_', 0x0ff0, 0, 2);
    _delay_ms(700);
    tft->drawChar(x, y, '_', 0, 0, 2);
    _delay_ms(700);
  }
}

void delay_print(Adafruit_TFTLCD * tft, char * str)
{
  while(*str) {
    tft->write(*str++);
    _delay_ms(60);
  }
}

void hacker_init(Adafruit_TFTLCD * tft)
{
  static char * strings[]
  {
    "WHCS loading",
    "Initializing NRF",
    "Bringing up HC-05",
    "Seeding AES-256 PRNG",
    "Starting main loop...",
    NULL
  };

  tft->setTextColor(0x0ff0);
  tft->setTextSize(2);
  tft->setCursor(20, 20);

  char ** str = strings;
  uint8_t yval = 20;

  while(*str) {
    delay_print(tft, *str);
    flash_cursor(tft, 22 + strlen(*str)*12, yval, 2);

    yval += 20;
    tft->setCursor(20, yval);
    str++;
  }
}
