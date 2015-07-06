// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// (c) ladyada / adafruit
// Code under MIT License

#include <avr/pgmspace.h>
#include <stdio.h>

#include "ADC.h"
#include "touchscreen.h"
#include "common.h"
#include "pinout.h"

// increase or decrease the touchscreen oversampling. This is a little different than you make think:
// 1 is no oversampling, whatever data we get is immediately returned
// 2 is double-sampling and we only return valid data if both points are the same
// 3+ uses insert sort to get the median value.
// We found 2 is precise yet not too slow so we suggest sticking with it!

#define NUMSAMPLES 2
#define TOLERANCE 5 // maximum difference between 2 sample points

// default constructor starts as invalid
TSPoint::TSPoint(void) {
  x = y = z = 0;
  valid = false;
}

TSPoint::TSPoint(int16_t x0, int16_t y0, int16_t z0) {
  x = x0;
  y = y0;
  z = z0;
  valid = true;
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

TouchScreen::TouchScreen() {
  _rxplate = 0;
  _pressureThreshold = 0;
  _rawCoord = true;
  _screenWidth = _screenHeight = 0;
  _xMin = _xMax = _yMin = _yMax = 0;
}


TouchScreen::TouchScreen(uint16_t rxplate, int16_t threshold) {
  _rxplate = rxplate;
  _pressureThreshold = threshold;
  _rawCoord = true;
  _screenWidth = _screenHeight = 0;
  _xMin = _xMax = _yMin = _yMax = 0;
}

TSPoint TouchScreen::getPoint(void) {
  int x, y, z;
  int samples[NUMSAMPLES];
  uint8_t i;

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
   if (abs(samples[0] - samples[1]) > TOLERANCE) { return TSPoint();}
#endif
   x = (1023 - samples[NUMSAMPLES/2]);

   PIN_MODE_INPUT(TOUCH_XP);
   PIN_MODE_INPUT(TOUCH_XM);
   PIN_LOW(TOUCH_XP);

   PIN_MODE_OUTPUT(TOUCH_YP);
   PIN_MODE_OUTPUT(TOUCH_YM);
   PIN_HIGH(TOUCH_YP);

   for (i=0; i<NUMSAMPLES; i++) {
     samples[i] = WHCSADC::read(TOUCH_XM_NUMBER);
   }

#if NUMSAMPLES > 2
   insert_sort(samples, NUMSAMPLES);
#endif
#if NUMSAMPLES == 2
   if (abs(samples[0] - samples[1]) > TOLERANCE) { return TSPoint();}
#endif

   y = (samples[NUMSAMPLES/2]);

   PIN_MODE_OUTPUT(TOUCH_XP);
   PIN_LOW(TOUCH_XP);

   PIN_HIGH(TOUCH_YM);

   PIN_MODE_INPUT(TOUCH_YP);

   int z1 = WHCSADC::read(TOUCH_XM_NUMBER);
   int z2 = WHCSADC::read(TOUCH_YP_NUMBER);

   // the read pressure value is lower than the threshold
   // return a valid point with a value of zero for pressure
   if(z1 < _pressureThreshold)
     return createPoint(x, y, 0);

   if (_rxplate != 0) {
     //(z2/z1 - 1)*x*_rxplate/1024
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

   //printf("z %d, z1 %d, z2 %d, x %d, rx %d\n", z, z1, z2, x, _rxplate);

   return createPoint(x, y, z);
}

void TouchScreen::setCalibration(int16_t screenWidth, int16_t screenHeight,
      int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax)
{
  _screenWidth = screenWidth;
  _screenHeight = screenHeight;
  _xMin = xMin;
  _yMin = yMin;
  _xMax = xMax;
  _yMax = yMax;
}

bool TouchScreen::usingRawCoords()
{
  return _rawCoord;
}

void TouchScreen::useScreenCoords()
{
  _rawCoord = false;
}

void TouchScreen::useRawCoords()
{
  _rawCoord = true;
}

TSPoint TouchScreen::createPoint(int16_t x, int16_t y, int16_t z)
{
  if(_rawCoord)
    return TSPoint(x, y, z);

  TSPoint p;

  //p.x = map(x, _yMin, _yMax, 0, _screenWidth);
  //p.y = map(y, _xMin, _xMax, 0, _screenHeight);
  p.x = map(x, _xMin, _xMax, 0, _screenWidth);
  p.y = map(y, _yMin, _yMax, 0, _screenHeight);
  p.z = z;
  p.valid = true;

  /*if(p.z)
    printf("(%d, %d) -> (%d, %d)\n", x, y, p.x, p.y);*/

  return p;
}
