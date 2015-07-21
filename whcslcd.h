#ifndef WHCSLCD_H
#define WHCSLCD_H

#include <stdint.h>

#include "Timer.h"
#include <Adafruit_TFTLCD.h>

class WHCSLCD
{
  public:
    WHCSLCD(Adafruit_TFTLCD * lcd, uint8_t rotation);
    void begin();

    // periodic tick for timers
    void tick();

    // LCD drawing
    void clearScreen();

    // LCD power management
    void setBrightness(uint8_t value);
    void screenOff();
    void screenOn();
    void screenFadeUp();
    void screenFadeDown();
    bool isPoweringOn();
    bool isPoweringOff();
    bool isVisible();

    Adafruit_TFTLCD * getGfx();
  private:
    void enableTimer();
    void disableTimer();

    enum fadeDirection_t { FADE_UP, FADE_DOWN };

    Adafruit_TFTLCD * m_lcd;
    Timer m_tFade;
    uint8_t m_rotation;
    uint8_t m_brightness;

    // brightness fading
    uint8_t m_fadeDirection;
    bool m_isFading;
};

#endif // WHCSLCD_H
