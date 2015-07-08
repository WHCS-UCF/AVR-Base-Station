#ifndef TOUCHEVENT_H
#define TOUCHEVENT_H

#include "touchscreen.h"

class TouchEvent
{
public:
  enum event_t {TOUCH_DOWN, TOUCH_UP, TOUCH_MOVE};
  uint8_t event;
  TSPoint point;
};

#endif // TOUCHEVENT_H
