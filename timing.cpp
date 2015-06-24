#include "timing.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile time_t gMillis = 0;

ISR(TIMER2_COMP_vect)
{
  gMillis++;
}

void timing_init()
{
  // Timer2 to normal mode with clk/64 (4us)
  // Interrupt on OCR2 match (250)
  // 4us * 250 = 1ms
  // Could be adjusted for the time an interrupt takes
  OCR2 = 250;
  TIMSK |= _BV(OCIE2); // enable interrupt for that timer
  TCCR2 = _BV(CS22) | _BV(WGM21); // enable timer
}
