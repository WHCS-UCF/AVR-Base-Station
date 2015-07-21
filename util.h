#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define ENABLE_MEMORY_DEBUGGING
#ifdef ENABLE_MEMORY_DEBUGGING
void StackPaint(void) __attribute__ ((naked)) __attribute__ ((section (".init1")));
uint16_t StackCount(void);
#endif

#endif // UTIL_H
