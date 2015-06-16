#ifndef WHCSLCD_H
#define WHCSLCD_H

#include <stdint.h>

class WHCSLCD
{
  public:
    WHCSLCD(Adafruit_TFTLCD * lcd, uint8_t rotation);
    void begin();

    void clearScreen();
    void screenOff();
    void screenOn();
  private:
    Adafruit_TFTLCD * m_lcd;
    uint8_t m_rotation;
};

#endif // WHCSLCD_H
