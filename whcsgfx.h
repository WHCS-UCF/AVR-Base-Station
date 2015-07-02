#ifndef WHCSGFX_H
#define WHCSGFX_H

#include <stdint.h>

class WHCSGfx
{
  public:
    WHCSGfx(Adafruit_TFTLCD * lcd);
    void drawAsciiArt(int16_t x, int16_t y, const PROGMEM uint8_t * array, size_t size, uint16_t baseColor);
  private:
    Adafruit_TFTLCD * m_lcd;
};

#endif // WHCSGFX_H
