#ifndef WHCSGFX_H
#define WHCSGFX_H

#include <stdint.h>
#include <avr/pgmspace.h>
#include <Adafruit_TFTLCD.h>

typedef uint16_t color_t;
typedef int16_t coord_t;
struct point { coord_t x, y; };
struct rect { coord_t x, y; coord_t w, h; };

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
    void drawBorder(rect bounds, color_t color);

    coord_t width();
    coord_t height();
    void pixel(coord_t x, coord_t y, color_t color);
    void pixel(point xy, color_t color);
    void line(point start, point end, color_t color);

    void fillRect(coord_t x, coord_t y, coord_t w, coord_t h, color_t color);
    void fillRect(rect bounds, color_t color);
    void clearScreen(color_t color);
    void clearRect(rect bounds, color_t color);
    void cursor(coord_t x, coord_t y);
    uint8_t getRotation();

    // Text operations
    void textColor(color_t color);
    void textSize(uint8_t s);
    void puts(const char * str);

    Adafruit_TFTLCD * getTFT();
  private:
    Adafruit_TFTLCD * m_tft;
};

#endif // WHCSGFX_H
