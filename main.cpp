#include <stdio.h>
#include <util/delay.h>
#include "fun.h"
#include "ADC.h"
#include "touchscreen.h"

// libraries
#include <MEGA32A_UART_LIBRARY.h>
#include <Adafruit_TFTLCD.h> // Hardware-specific library

#include "whcslcd.h"
#include "base_station_pins.h"

int uart_putchar(char c, FILE *stream) {
  if (c == '\n')
    uart_putchar('\r', stream);

  USART_SendByte(c);

  return 1;
}

int uart_getchar(FILE *stream) {
  //loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
  //return UDR0;
  return 0;
}

Adafruit_TFTLCD tft;
WHCSLCD lcd(&tft, 1);
TouchScreen touch(300);

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main()
{
  WHCSADC::init();
  initUart();
  fdevopen(uart_putchar, NULL);
  fdevopen(NULL, uart_getchar);

  STATUS_LED_DDR |= STATUS_LED_MSK;
  STATUS_LED_PORT |= STATUS_LED_MSK;

  printf("All init\n");
  lcd.begin();
  tft.fillRect(100, 60, 120, 120, 0xff00);

  tft.setTextColor(0x0ff0);
  tft.setTextSize(3);
  tft.setCursor(110, 65);
  tft.println("LED");

  int dirty = 1;
  int press = 0;

  for(;;)
  {
    TSPoint p = touch.getPoint();
    printf("x = %d, y = %d, z = %d\n", p.x, p.y, p.z);

    if(p.z) {
      /*tft.drawPixel(map(p.x, 146, 877, 0, 319), 
          map(p.y, 114, 880, 0, 239), 0x0ff0);*/
      if(press == 0)
        dirty = 1;

      press = 1;

      STATUS_LED_PORT ^= STATUS_LED_MSK;
    }
    else
    {
      if(press == 1)
        dirty = 1;

      press = 0;
    }

    if(STATUS_LED_PORT & STATUS_LED_MSK)
    tft.setTextColor(0x0ff0);
    else
    tft.setTextColor(0x00ff);

    if(dirty) {
      tft.setTextSize(3);
      tft.setCursor(110, 65);
      tft.println("LED");
      dirty = 0;
    }
  }

  // main must not return
  for(;;);
}
