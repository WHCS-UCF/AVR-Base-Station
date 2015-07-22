#include <Adafruit_TFTLCD.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "whcsgfx.h"

WHCSGfx::WHCSGfx(Adafruit_TFTLCD * lcd)
  :m_tft(lcd)
{
}

void WHCSGfx::drawAsciiArt(int16_t x, int16_t y, const PROGMEM uint8_t * array, size_t size, uint16_t baseColor)
{
  m_tft->setCursor(x, y);
  m_tft->setTextWrap(false);
  m_tft->setTextSize(1);
  
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
        m_tft->write(chr);
    }
    else
    {
      if(byte == '\n')
      {
        int16_t x, y;
        m_tft->getCursor(&x, &y);
        m_tft->setCursor(0, y+8);
      }
      else if(byte == 0x1) // normal color
        m_tft->setTextColor(0x0560);
      else if(byte == 0x2)
        m_tft->setTextColor(0x07c0);
      else if(byte == 0x3)
        m_tft->setTextColor(0x07e0);
      else
        m_tft->write(byte);
    }
  }

  m_tft->setTextWrap(true);
}

void WHCSGfx::drawBorder(rect bounds, color_t color)
{
  m_tft->drawRect(bounds.x, bounds.y, bounds.w, bounds.h, color);
}

void WHCSGfx::fillRect(coord_t x, coord_t y, coord_t w, coord_t h, color_t color)
{
  m_tft->fillRect(x, y, w, h, color);
}

void WHCSGfx::fillRect(rect bounds, color_t color)
{
  m_tft->fillRect(bounds.x, bounds.y, bounds.w, bounds.h, color);
}

void WHCSGfx::clearScreen(color_t color)
{
  m_tft->fillScreen(color);
}

coord_t WHCSGfx::width()
{
  m_tft->width();
}

coord_t WHCSGfx::height()
{
  m_tft->height();
}

void WHCSGfx::pixel(coord_t x, coord_t y, color_t color)
{
  m_tft->drawPixel(x, y, color);
}

void WHCSGfx::pixel(point xy, color_t color)
{
  m_tft->drawPixel(xy.x, xy.y, color);
}

void WHCSGfx::line(point start, point end, color_t color)
{
  m_tft->drawLine(start.x, start.y, end.x, end.y, color);
}

void WHCSGfx::clearRect(rect bounds, color_t color)
{
  m_tft->fillRect(bounds.x, bounds.y, bounds.w, bounds.h, color);
}

void WHCSGfx::cursor(coord_t x, coord_t y)
{
  m_tft->setCursor(x, y);
}

uint8_t WHCSGfx::getRotation()
{
  return m_tft->getRotation();
}

void WHCSGfx::textColor(color_t color)
{
  m_tft->setTextColor(color);
}

void WHCSGfx::textSize(uint8_t s)
{
  m_tft->setTextSize(s);
}

void WHCSGfx::puts(const char * str)
{
  m_tft->println((char *)str);
}
