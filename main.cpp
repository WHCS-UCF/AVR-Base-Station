#include <stdio.h>
#include <util/delay.h>

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
WHCSLCD lcd(&tft, 3); // rotate 270 deg

int main()
{
  initUart();
  fdevopen(uart_putchar, NULL);
  fdevopen(NULL, uart_getchar);

  printf("All init\n");
  lcd.begin();

  for(;;) {
    printf("Things are working!\n");
    _delay_ms(1000);
  }

  // main must not return
}

