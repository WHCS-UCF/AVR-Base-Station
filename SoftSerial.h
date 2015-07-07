#ifndef SOFTSERIAL_H
#define SOFTSERIAL_H

#include <stdio.h>

// software serial library that uses TIMER1
void soft_serial_init();
int soft_serial_putc(char c, FILE *stream);

#endif // SOFTSERIAL_H
