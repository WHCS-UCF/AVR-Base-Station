#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdint.h>

class WHCSADC
{
public:
    static void init();
    static uint16_t read(uint8_t pin);
};

#endif // ADC_H
