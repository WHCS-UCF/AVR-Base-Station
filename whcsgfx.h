#ifndef WHCSGFX_H
#define WHCSGFX_H

#include <stdint.h>
#include <avr/pgmspace.h>

// Color definitions
#define COLOR_BLACK       0x0000      /*   0,   0,   0 */
#define COLOR_RED         0xF800      /* 255,   0,   0 */
#define COLOR_GREEN       0x07E0      /*   0, 255,   0 */
#define COLOR_BLUE        0x001F      /*   0,   0, 255 */
#define COLOR_WHITE       0xFFFF      /* 255, 255, 255 */

#define COLOR_NAVY        0x000F      /*   0,   0, 128 */
#define COLOR_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define COLOR_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define COLOR_MAROON      0x7800      /* 128,   0,   0 */
#define COLOR_PURPLE      0x780F      /* 128,   0, 128 */
#define COLOR_OLIVE       0x7BE0      /* 128, 128,   0 */
#define COLOR_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define COLOR_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define COLOR_CYAN        0x07FF      /*   0, 255, 255 */
#define COLOR_MAGENTA     0xF81F      /* 255,   0, 255 */
#define COLOR_YELLOW      0xFFE0      /* 255, 255,   0 */
#define COLOR_ORANGE      0xFD20      /* 255, 165,   0 */
#define COLOR_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define COLOR_PINK        0xF81F

class WHCSGfx
{
  public:
    WHCSGfx(Adafruit_TFTLCD * lcd);
    void drawAsciiArt(int16_t x, int16_t y, const PROGMEM uint8_t * array, size_t size, uint16_t baseColor);
  private:
    Adafruit_TFTLCD * m_lcd;
};

#endif // WHCSGFX_H
