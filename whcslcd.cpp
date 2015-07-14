#include <Adafruit_TFTLCD.h>
#include "pinout.h"

#include "whcslcd.h"

#define LCD_VISIBLE_THRESHOLD 30

WHCSLCD::WHCSLCD(Adafruit_TFTLCD * lcd, uint8_t rotation)
  :m_isFading(false)
{
  m_lcd = lcd;
  m_rotation = rotation;
}

void WHCSLCD::begin()
{
  // initialize LCD auxillary pins
  // set backlight as output
  PIN_MODE_OUTPUT(LCD_BACKLIGHT);

  // start with the screen off
  screenOff();

  m_lcd->reset();
  _delay_ms(100);

  // TODO: check ID and confirm
  // meant to work with a specific controller
  m_lcd->begin(0x9341);
  clearScreen();

  // orient the screen as requested
  m_lcd->setRotation(m_rotation);

  // allow the LCD to settle before turning it on
  _delay_ms(10);
}

void WHCSLCD::tick()
{
  if(m_isFading && m_tFade.update())
  {
    if(m_fadeDirection == FADE_UP)
    {
      if(m_brightness == 255)
        screenOn();
      else 
      {
        m_brightness++;
        OCR0 = m_brightness;
      }
    }
    else if(m_fadeDirection == FADE_DOWN)
    {
      if(m_brightness == 0)
        screenOff();
      else 
      {
        m_brightness--;
        OCR0 = m_brightness;
      }
    }
  }
}

void WHCSLCD::clearScreen()
{
  m_lcd->fillScreen(0);
}

void WHCSLCD::screenOff()
{
  m_isFading = false;
  m_brightness = 0;
  disableTimer();
  PIN_LOW(LCD_BACKLIGHT);
}

void WHCSLCD::screenOn()
{
  m_isFading = false;
  m_brightness = 255;
  disableTimer();
  PIN_HIGH(LCD_BACKLIGHT);
}

void WHCSLCD::screenFadeUp()
{
  if(m_brightness == 255)
    return;

  m_isFading = true;
  m_fadeDirection = FADE_UP;
  m_tFade.periodic(10);
  enableTimer();
}

void WHCSLCD::screenFadeDown()
{
  if(m_brightness == 0)
    return;

  m_isFading = true;
  m_fadeDirection = FADE_DOWN;
  m_tFade.periodic(10);
  enableTimer();
}

bool WHCSLCD::isPoweringOn()
{
  return m_isFading && m_fadeDirection == FADE_UP;
}

bool WHCSLCD::isPoweringOff()
{
  return m_isFading && m_fadeDirection == FADE_DOWN;
}


bool WHCSLCD::isVisible()
{
  return m_brightness >= LCD_VISIBLE_THRESHOLD;
}

void WHCSLCD::enableTimer()
{
  // Used to provide PWM output for the screen brightness
  // Timer0 to non-inverted Fast PWM mode
  // Uses 16 MHz PWM
  OCR0 = m_brightness;
  TCCR0 = _BV(WGM01) | _BV(WGM00) | _BV(COM01) | _BV(CS00);
}

void WHCSLCD::disableTimer()
{
  TCCR0 = 0;
}
