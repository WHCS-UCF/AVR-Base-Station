#include <Adafruit_TFTLCD.h>
#include "base_station_pins.h"

#include "whcslcd.h"

WHCSLCD::WHCSLCD(Adafruit_TFTLCD * lcd, uint8_t rotation)
{
  m_lcd = lcd;
  m_rotation = rotation;
}

void WHCSLCD::begin()
{
  // initialize LCD auxillary pins
  // set backlight as output
  LCD_BACKLIGHT_DDR |= LCD_BACKLIGHT_MSK;

  screenOff();

  m_lcd->reset();
  _delay_ms(100);

  // TODO: check ID and confirm
  // meant to work with a specific controller
  m_lcd->begin(0x9341);
  clearScreen();

  // orient the screen as requested
  m_lcd->setRotation(m_rotation);

  _delay_ms(10);
  screenOn();
}

void WHCSLCD::clearScreen()
{
  m_lcd->fillScreen(0);
}

void WHCSLCD::screenOff()
{
  LCD_BACKLIGHT_PORT &= ~LCD_BACKLIGHT_MSK;
}

void WHCSLCD::screenOn()
{
  LCD_BACKLIGHT_PORT |= LCD_BACKLIGHT_MSK;
}
