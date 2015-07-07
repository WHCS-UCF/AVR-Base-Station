#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include <MEGA32A_UART_LIBRARY.h>
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <RF24.h>

#include "fun.h"
#include "timing.h"
#include "ADC.h"
#include "touchscreen.h"
#include "Radio.h"
#include "whcslcd.h"
#include "whcsgfx.h"
#include "pinout.h"
#include "TouchCalibrate.h"

#include "img/grant.h"
#include "img/jimmy.h"
#include "img/ucf.h"

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

RF24 rf24(NRF_CE_NUMBER, NRF_CS_NUMBER); // pins on PORTB ONLY
Radio radio(&rf24, 0x41);
Adafruit_TFTLCD tft;
WHCSLCD lcd(&tft, 3);
WHCSGfx gfx(&tft);
TouchScreen touch(300, 20); // rxplate, pressure threshold

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
  lcd.clearScreen();
  lcd.screenOn();

  // enable the HC-05
  PIN_MODE_OUTPUT(HC05_ENABLE);
  PIN_HIGH(HC05_ENABLE);

  printf_P(PSTR("WHCS main loop starting\n"));

  // Enable interrupts before entering main event loop
  sei();

  time_t maxLoopTime = 0;

  // Main event loop
  Timer evt;
  evt.periodic(3000);

  int down = 0;
  TSPoint pLast;
  TouchCalibrate uiCalibrate(&tft, &touch);
  uiCalibrate.onCreate();

  while(1)
  {
    unsigned long mainStart = millis();

    ///////////////////////////////////////

    lcd.tick();
    uiCalibrate.tick();

    if(uiCalibrate.isDirty())
      uiCalibrate.draw();

    TSPoint p = touch.getPoint();
    TouchEvent te;

    if(p.valid && p.z != 0)
    {
      if(!down) {
        //printf("TOUCH_DOWN (%d, %d)\n", p.x, p.y);
        te.point = p;
        te.event = TouchEvent::TOUCH_DOWN;
        uiCalibrate.touchEvent(&te);
        down = 1;
      }

      //printf("z = %d\n", p.z);

      //if(p != pLast)
        //printf("TOUCH_MOVE (%d, %d)\n", p.x, p.y);

      pLast = p;
    }
    else if(p.valid && p.z == 0)
    {
      if(down) {
        //printf("TOUCH_UP (%d, %d)\n", pLast.x, pLast.y);
        te.point = p;
        te.event = TouchEvent::TOUCH_UP;
        uiCalibrate.touchEvent(&te);
        down = 0;
      }
    }

    ///////////////////////////////////////

    time_t delta = millis() - mainStart;

    if(delta > maxLoopTime) {
      if(delta > MAIN_LOOP_WARNING)
        printf("WARNING LIMIT: ");

      printf("Main loop max: %lums\n", delta);

      maxLoopTime = delta;
    }
  }

  // main must not return
  for(;;);
}
