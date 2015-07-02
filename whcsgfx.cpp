#include <Adafruit_TFTLCD.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "whcsgfx.h"

WHCSGfx::WHCSGfx(Adafruit_TFTLCD * lcd)
  :m_lcd(lcd)
{
}

void WHCSGfx::drawAsciiArt(int16_t x, int16_t y, const PROGMEM uint8_t * array, size_t size, uint16_t baseColor)
{
  m_lcd->setCursor(x, y);
  m_lcd->setTextWrap(false);
  m_lcd->setTextSize(1);
  
  for(size_t i = 0; i < size; i++)
  {
    uint8_t byte = pgm_read_byte(&array[i]);

    if(byte == 0) // repeat byte
    {
      i++;
      uint8_t amt = pgm_read_byte(&array[i]);
      i++;
      uint8_t chr = pgm_read_byte(&array[i]);

      for(uint8_t j = 0; j < amt; j++)
        m_lcd->write(chr);
    }
    else
    {
      if(byte == '\n')
      {
        int16_t x, y;
        m_lcd->getCursor(&x, &y);
        m_lcd->setCursor(0, y+8);
      }
      else if(byte == 0x1) // normal color
        m_lcd->setTextColor(0x0560);
      else if(byte == 0x2)
        m_lcd->setTextColor(0x07c0);
      else if(byte == 0x3)
        m_lcd->setTextColor(0x07e0);
      else
        m_lcd->write(byte);
    }
  }

  m_lcd->setTextWrap(true);
}
