// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// (c) ladyada / adafruit
// Code under MIT License

#ifndef _ADAFRUIT_TOUCHSCREEN_H_
#define _ADAFRUIT_TOUCHSCREEN_H_
#include <stdint.h>

class TSPoint {
 public:
  TSPoint(void);
  TSPoint(int16_t x, int16_t y, int16_t z);

  bool operator==(TSPoint);
  bool operator!=(TSPoint);

  int16_t x, y, z;
  bool valid;
};

class TouchScreen {
 public:
  TouchScreen();
  TouchScreen(uint16_t rx, int16_t threshold);

  TSPoint getPoint();
  void setCalibration(int16_t screenWidth, int16_t screenHeight,
      int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax);
  bool usingRawCoords(); // getter for screen coords bool
  void useScreenCoords(); // ADC -> screen values
  void useRawCoords(); // direct ADC values
private:
  // wrapper for creating a point with respect to screen coordinates
  TSPoint createPoint(int16_t x, int16_t y, int16_t z);
  int16_t _screenWidth, _screenHeight;
  int16_t _xMin, _xMax, _yMin, _yMax;
  bool _rawCoord;

  uint16_t _rxplate;
  int16_t _pressureThreshold;
};

#endif
