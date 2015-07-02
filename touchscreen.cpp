// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// (c) ladyada / adafruit
// Code under MIT License

#include <avr/pgmspace.h>

#include "ADC.h"
#include "touchscreen.h"
#include "pinout.h"

// increase or decrease the touchscreen oversampling. This is a little different than you make think:
// 1 is no oversampling, whatever data we get is immediately returned
// 2 is double-sampling and we only return valid data if both points are the same
// 3+ uses insert sort to get the median value.
// We found 2 is precise yet not too slow so we suggest sticking with it!

#define NUMSAMPLES 2

TSPoint::TSPoint(void) {
  x = y = 0;
}

TSPoint::TSPoint(int16_t x0, int16_t y0, int16_t z0) {
  x = x0;
  y = y0;
  z = z0;
}

bool TSPoint::operator==(TSPoint p1) {
  return  ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

bool TSPoint::operator!=(TSPoint p1) {
  return  ((p1.x != x) || (p1.y != y) || (p1.z != z));
}

#if (NUMSAMPLES > 2)
static void insert_sort(int array[], uint8_t size) {
  uint8_t j;
  int save;
  
  for (int i = 1; i < size; i++) {
    save = array[i];
    for (j = i; j >= 1 && save < array[j - 1]; j--)
      array[j] = array[j - 1];
    array[j] = save; 
  }
}
#endif

TSPoint TouchScreen::getPoint(void) {
  int x, y, z;
  int samples[NUMSAMPLES];
  uint8_t i, valid = 1; 

  PIN_MODE_INPUT(TOUCH_YM);
  PIN_MODE_INPUT(TOUCH_YP);
  PIN_LOW(TOUCH_YP);
  PIN_LOW(TOUCH_YM);
   
  PIN_MODE_OUTPUT(TOUCH_XP);
  PIN_MODE_OUTPUT(TOUCH_XM);
  PIN_HIGH(TOUCH_XP);
  PIN_LOW(TOUCH_XM);
   
   for (i=0; i<NUMSAMPLES; i++) {
     samples[i] = WHCSADC::read(TOUCH_YP_NUMBER);
   }
#if NUMSAMPLES > 2
   insert_sort(samples, NUMSAMPLES);
#endif
#if NUMSAMPLES == 2
   // TODO: optimize this common case
   if (samples[0] != samples[1]) { valid = 0; }
#endif
   x = (1023 - samples[NUMSAMPLES/2]);

   PIN_MODE_INPUT(TOUCH_XP);
   PIN_MODE_INPUT(TOUCH_XM);
   PIN_LOW(TOUCH_XP);
   
   PIN_MODE_OUTPUT(TOUCH_YP);
   PIN_MODE_OUTPUT(TOUCH_YM);
   //pinMode(_yp, OUTPUT);
   //*portOutputRegister(yp_port) |= yp_pin;
   //digitalWrite(_yp, HIGH);
   //pinMode(_ym, OUTPUT);
   PIN_HIGH(TOUCH_YP);
  
   for (i=0; i<NUMSAMPLES; i++) {
     samples[i] = WHCSADC::read(TOUCH_XM_NUMBER); //analogRead(_xm);
   }

#if NUMSAMPLES > 2
   insert_sort(samples, NUMSAMPLES);
#endif
#if NUMSAMPLES == 2
   if (samples[0] != samples[1]) { valid = 0; }
#endif

   y = (samples[NUMSAMPLES/2]);

   PIN_MODE_OUTPUT(TOUCH_XP);
   PIN_LOW(TOUCH_XP);

   PIN_HIGH(TOUCH_YM);

   PIN_MODE_INPUT(TOUCH_YP);

   int z1 = WHCSADC::read(TOUCH_XM_NUMBER);
   int z2 = WHCSADC::read(TOUCH_YP_NUMBER);

   if (_rxplate != 0) {
     // now read the x 
     float rtouch;
     rtouch = z2;
     rtouch /= z1;
     rtouch -= 1;
     rtouch *= x;
     rtouch *= _rxplate;
     rtouch /= 1024;

     z = rtouch;
   } else {
     z = (1023-(z2-z1));
   }

   if (! valid) {
     z = 0;
   }

   return TSPoint(y, x, z);
}

TouchScreen::TouchScreen() {
  _rxplate = 0;
  pressureThreshhold = 10;
}


TouchScreen::TouchScreen(uint16_t rxplate) {
  _rxplate = rxplate;

  pressureThreshhold = 10;
}

int TouchScreen::readTouchX(void) {
  PIN_MODE_INPUT(TOUCH_YM);
  PIN_MODE_INPUT(TOUCH_YP);
  PIN_LOW(TOUCH_YM);
  PIN_LOW(TOUCH_YP);

  PIN_MODE_OUTPUT(TOUCH_XP);
  PIN_MODE_OUTPUT(TOUCH_XM);
  PIN_HIGH(TOUCH_XP);
  PIN_LOW(TOUCH_XM);

  return 1023-WHCSADC::read(TOUCH_YP_NUMBER);
}


int TouchScreen::readTouchY(void) {
   PIN_MODE_INPUT(TOUCH_XP);
   PIN_MODE_INPUT(TOUCH_XM);
   PIN_LOW(TOUCH_XP);
   PIN_LOW(TOUCH_XM);

   PIN_MODE_OUTPUT(TOUCH_YP);
   PIN_MODE_OUTPUT(TOUCH_YM);
   PIN_HIGH(TOUCH_YP);
   PIN_LOW(TOUCH_YM);

   return 1023-WHCSADC::read(TOUCH_XM_NUMBER);
}


uint16_t TouchScreen::pressure(void) {
  PIN_MODE_OUTPUT(TOUCH_XP);
  PIN_LOW(TOUCH_XP);

  PIN_MODE_OUTPUT(TOUCH_YM);
  PIN_HIGH(TOUCH_YM);

  PIN_MODE_INPUT(TOUCH_YP);
  PIN_MODE_INPUT(TOUCH_XM);
  PIN_LOW(TOUCH_YP);
  PIN_LOW(TOUCH_XM);

  int z1 = WHCSADC::read(TOUCH_XM_NUMBER);
  int z2 = WHCSADC::read(TOUCH_YP_NUMBER);

  // Set X+ to ground
  /*pinMode(_xp, OUTPUT);
  digitalWrite(_xp, LOW);
  
  // Set Y- to VCC
  pinMode(_ym, OUTPUT);
  digitalWrite(_ym, HIGH); 
  
  // Hi-Z X- and Y+
  pinMode(_xm, INPUT);
  pinMode(_yp, INPUT);
  digitalWrite(_xm, LOW);
  digitalWrite(_yp, LOW);
  
  int z1 = analogRead(_xm); 
  int z2 = analogRead(_yp);*/

  if (_rxplate != 0) {
    // now read the x 
    float rtouch;
    rtouch = z2;
    rtouch /= z1;
    rtouch -= 1;
    rtouch *= readTouchX();
    rtouch *= _rxplate;
    rtouch /= 1024;
    
    return rtouch;
  } else {
    return (1023-(z2-z1));
  }
}
