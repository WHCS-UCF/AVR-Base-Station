#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include <MEGA32A_UART_LIBRARY.h>
#include <Adafruit_TFTLCD.h> // Hardware-specific library

#include "fun.h"
#include "timing.h"
#include "ADC.h"
#include "touchscreen.h"
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
WHCSLCD lcd(&tft, 3);
TouchScreen touch(300);

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main()
{
  timing_init(); // initialize millis()

  initUart();
  fdevopen(uart_putchar, NULL);
  fdevopen(NULL, uart_getchar);

  printf("[W]ireless [H]ome [C]ontrol [S]ystem Base Station\n");

  WHCSADC::init();

  PIN_MODE_OUTPUT(STATUS_LED);
  PIN_HIGH(STATUS_LED);

  lcd.begin();
  tft.fillScreen(0x0ff0);


  //PIN_MODE_OUTPUT(HC05_ENABLE);
  //PIN_HIGH(HC05_ENABLE);

  // Enable interrupts before entering main event loop
  sei();
  printf("Interrupts enabled - starting main loop\n");

  time_t maxLoopTime = 0;

  // Main event loop
  Timer updown;
  updown.periodic(3000);
  int dir = 0;
  while(1)
  {
    unsigned long mainStart = millis();

    lcd.tick();

    if(updown.update())
    {
      if(dir)
      {
        lcd.fadeUp();
        dir = 0;
      }
      else
      {
        lcd.fadeDown();
        dir = 1;
      }
    }

    time_t delta = millis() - mainStart;

    if(delta > maxLoopTime) {
      printf("Main loop max: %lums\n", delta);
      maxLoopTime = delta;
    }
  }

  // main must not return
  for(;;);
}
