#include "SoftSerial.h"

#include <avr/interrupt.h>
#include <stdint.h>

#include "pinout.h"

enum soft_state_t
{
  STATE_IDLE = 0,
  STATE_TX,
  STATE_TX_STOP,
  STATE_TX_STOP2
};

static volatile uint8_t softState = STATE_IDLE;
static volatile uint8_t txBuf = 0;
static volatile uint8_t bitCount = 0;

ISR(TIMER1_COMPA_vect)
{
  if(softState == STATE_TX) { // 8 bits
    if(bitCount == 8) {
      softState = STATE_TX_STOP;
      PIN_LOW(TX_SOFT); // start a stop bit
      return;
    }

    if(txBuf & 1)
      PIN_HIGH(TX_SOFT);
    else
      PIN_LOW(TX_SOFT);

    txBuf >>= 1;
    bitCount++;
  } else if(softState == STATE_TX_STOP) {
    softState = STATE_TX_STOP2;
    PIN_HIGH(TX_SOFT);
  } else {
    softState = STATE_IDLE;
    TCCR1B = 0; // timer off
  }
}

void soft_serial_init()
{
  PIN_MODE_OUTPUT(TX_SOFT);
  PIN_HIGH(TX_SOFT);

  OCR1A = 138;
  TIMSK |= _BV(OCIE1A);
  TCCR1A = 0;
  TCCR1B = 0;
}

int soft_serial_putc(char c, FILE *stream)
{
  if (c == '\n')
    soft_serial_putc('\r', stream);

  while(softState != STATE_IDLE);

  txBuf = c;
  bitCount = 0;
  TCNT1 = 0;

  PIN_LOW(TX_SOFT);
  // turn timer on
  TCCR1B = _BV(CS10) | _BV(WGM12);
  softState = STATE_TX;
}
