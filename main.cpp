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
#include "SoftSerial.h"
#include "BlueTooth.h"

#include "img/grant.h"
#include "img/jimmy.h"
#include "img/ucf.h"

RingBuffer uartRxBuffer, uartTxBuffer;

/*int uart_putchar(char c, FILE *stream) {
  if (c == '\n')
    uart_putchar('\r', stream);

  //USART_SendByte(c);
  if(UCSRA & _BV(UDRE))
    UDR = c;
  else {
    while(uartTxBuffer.full());
    uartTxBuffer.put(c);
  }

  return 1;
}*/

//*int uart_getchar(FILE *stream) {
  //loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
  //return UDR0;
  //return 0;
  //return USART_ReceiveByte();
//}

static FILE mylcdout;
RF24 rf24(NRF_CE_NUMBER, NRF_CS_NUMBER); // pins on PORTB ONLY
Radio radio(&rf24, 0x41);
Adafruit_TFTLCD tft(&mylcdout);
WHCSLCD lcd(&tft, 3);
WHCSGfx gfx(&tft);
TouchScreen touch(300, 20); // rxplate, pressure threshold

int lcd_putchar(char c, FILE *stream) {
  tft.write(c);
  return 1;
}

#define MAIN_LOOP_WARNING 300 // 300ms maximum main loop time until warning
static FILE mystdout;
static FILE mystdin;

BlueTooth bluetooth(&uartRxBuffer, &uartTxBuffer);

ISR(USART_RXC_vect)
{
  uartRxBuffer.put(UDR);
}

EMPTY_INTERRUPT(USART_TXC_vect);
ISR(USART_UDRE_vect)
{
  if(uartTxBuffer.available())
    UDR = uartTxBuffer.get();
  else
    disableTXInterrupts();
}

ISR(BADISR_vect)
{
  PIN_MODE_OUTPUT(STATUS_LED);
  while(1) {
    PIN_HIGH(STATUS_LED);
    _delay_ms(500);
    PIN_LOW(STATUS_LED);
    _delay_ms(500);
  }
}

int main()
{
  // initialize timing (millis) as the first call 
  timing_init();
  WHCSADC::init(); // this sets the ADC port as all inputs
  initUart();
  enableRXInterrupts();
  // this is causing an interrupt loop
  // you MUST have UDR filled
  enableTXInterrupts();

  soft_serial_init();
  // Enable interrupts here for the software serial
  sei();

  // setup STDIN/STDOUT for printf
  fdev_setup_stream(&mystdout, soft_serial_putc, NULL, _FDEV_SETUP_WRITE);
  fdev_setup_stream(&mylcdout, lcd_putchar, NULL, _FDEV_SETUP_WRITE);
  //fdev_setup_stream(&mystdin, NULL, uart_getchar, _FDEV_SETUP_READ);
  stdout = &mystdout;
  //stdin = &mystdin;

  printf_P(PSTR("[W]ireless [H]ome [C]ontrol [S]ystem Base Station\n"));

  // show that we're powered up
  PIN_MODE_OUTPUT(STATUS_LED);
  PIN_HIGH(STATUS_LED);

  //radio.begin();

  lcd.begin();
  lcd.clearScreen();
  lcd.fadeUp();

  // enable the HC-05
  PIN_MODE_OUTPUT(HC05_ENABLE);
  PIN_HIGH(HC05_ENABLE);

  printf_P(PSTR("WHCS main loop starting\n"));

  time_t maxLoopTime = 0;

  // Main event loop
  Timer evt;
  evt.periodic(3000);

  int down = 0;
  TSPoint pLast;
  TouchCalibrate uiCalibrate(&tft, &touch);
  uiCalibrate.onCreate();

  bool done = false;

  while(1)
  {
    unsigned long mainStart = millis();

    ///////////////////////////////////////

    // pump the BlueTooth TX buffer
    if(uartTxBuffer.available()) {
      if(UCSRA & _BV(UDRE)) {
        int c = uartTxBuffer.get();
        UDR = c;
        enableTXInterrupts();
      }
    }

    uint8_t buf[10];
    if(bluetooth.available()) {
      size_t amt = bluetooth.read(buf, 10);
      for(size_t i = 0; i < amt; i++) {
        printf("%x ", buf[i]);
      }
    }

    lcd.tick();
    uiCalibrate.tick();

    if(uiCalibrate.isDirty())
      uiCalibrate.draw();
    else if(uiCalibrate.done() && !done) {
      lcd.fadeDown();
      done = true;
    }

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

      //if(abs(p.x-pLast.x) > 2 || abs(p.y-pLast.y) > 2);
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
        printf_P(PSTR("WARNING LIMIT: "));

      printf_P(PSTR("Main loop max: %lums\n"), delta);

      maxLoopTime = delta;
    }
  }

  // main must not return
  for(;;);
}
