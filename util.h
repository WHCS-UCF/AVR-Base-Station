#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <avr/io.h>

//#define ENABLE_MEMORY_DEBUGGING
#ifdef ENABLE_MEMORY_DEBUGGING
void StackPaint(void) __attribute__ ((naked)) __attribute__ ((section (".init1")));
uint16_t StackCount(void);

static inline int FreeRAM() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

static inline int TotalRAM() {
  extern char *__data_start; 
  return (int)(RAMEND - (int)&__data_start + 1);
}

#endif

#endif // UTIL_H
