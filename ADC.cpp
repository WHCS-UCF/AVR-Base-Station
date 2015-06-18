#include "ADC.h"

#include "base_station_pins.h"

void WHCSADC::init()
{
  PIN_MODE_INPUT(ADC); // all inputs (to engage pull ups)

  // select Vref=AVcc
  ADMUX |= (1<<REFS0);

  // set prescaller to 128 and enable ADC 
  ADCSRA |= _BV(ADPS2) | _BV(ADPS1) |
    _BV(ADPS0) | _BV(ADEN);
}

uint16_t WHCSADC::read(uint8_t pin)
{
  // select ADC channel with safety mask
  ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);

  // single conversion mode
  ADCSRA |= _BV(ADSC);

  // wait for ADC conversion
  while (ADCSRA & _BV(ADSC));

  return ADC;
}
