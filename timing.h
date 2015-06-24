#ifndef TIMING_H
#define TIMING_H

#include <avr/interrupt.h>

#include "common.h"

// must be called before the first use of millis()
void timing_init();

// returns the time in miliseconds since the processor started
static inline time_t millis()
{
  extern volatile time_t gMillis;

  // must disable interrupts in order to atomically read the millis()
  cli();
  time_t m = gMillis;
  sei();

  return m;
}

#endif // TIMING_H
