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
#include "RF24.h"
#include "whcslcd.h"
#include "whcsgfx.h"
#include "base_station_pins.h"

#include "img/grant.h"
#include "img/jimmy.h"

int uart_putchar(char c, FILE *stream) {
  if (c == '\n')
    uart_putchar('\r', stream);

  USART_SendByte(c);

  return 1;
}

int uart_getchar(FILE *stream) {
  //loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
  //return UDR0;
  //return 0;
  return USART_ReceiveByte();
}

RF24 radio(NRF_CE_NUMBER, NRF_CS_NUMBER); // pins on PORTB ONLY
Adafruit_TFTLCD tft;
WHCSLCD lcd(&tft, 3);
WHCSGfx gfx(&tft);
TouchScreen touch(300);

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define MAIN_LOOP_WARNING 300 // 300ms maximum main loop time until warning
static FILE mystdout;
static FILE mystdin;

int main()
{
  // initialize timing (millis()) as the first call 
  timing_init();
  WHCSADC::init(); // this sets the ADC port as all inputs
  initUart();

  // setup STDIN/STDOUT for printf
  fdev_setup_stream(&mystdout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
  fdev_setup_stream(&mystdin, NULL, uart_getchar, _FDEV_SETUP_READ);
  stdout = &mystdout;
  stdin = &mystdin;

  printf("[W]ireless [H]ome [C]ontrol [S]ystem Base Station\n");

  // show that we're powered up
  PIN_MODE_OUTPUT(STATUS_LED);
  PIN_HIGH(STATUS_LED);

  radio.begin();

  lcd.begin();
  tft.fillScreen(0x0ff0);

  PIN_MODE_OUTPUT(HC05_ENABLE);
  PIN_HIGH(HC05_ENABLE);

  printf_P(PSTR("WHCS main loop starting\n"));

  // Enable interrupts before entering main event loop
  sei();

  lcd.screenOn();

  while(1) {
    tft.fillScreen(0x0);
    gfx.drawAsciiArt(0, 0, gImageGrant, sizeof(gImageGrant), 0);
    _delay_ms(2000);

    tft.fillScreen(0x0);
    gfx.drawAsciiArt(0, 0, gImageJimmy, sizeof(gImageJimmy), 0);
    _delay_ms(2000);
  }

  // allows Jimmy to test 
  putchar(0x1b);
  putchar(0x42);
  putchar(0x50);
  putchar(0x43);

  /*time_t maxLoopTime = 0;

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
      if(delta > MAIN_LOOP_WARNING)
        printf("WARNING LIMIT: ");

      printf("Main loop max: %lums\n", delta);

      maxLoopTime = delta;
    }
  }*/

  // main must not return
  for(;;);
}
