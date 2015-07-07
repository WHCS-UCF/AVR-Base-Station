#ifndef TOUCHCALIBRATE_H
#define TOUCHCALIBRATE_H

#include <stdint.h>

#include <Adafruit_TFTLCD.h>
#include "touchscreen.h"
#include "TouchEvent.h"
#include "Timer.h"

class TouchCalibrate
{
  public:
    TouchCalibrate(Adafruit_TFTLCD * tft, TouchScreen * touch);
    void reset();

    void touchEvent(TouchEvent * ev); // called on a touch event
    void onCreate(); // called when scene first created
    void end(); // ends the scene
    void tick(); 
    void draw();
    bool isDirty();
    bool done();
  private:
    bool checkCal();

    TouchScreen * m_touch;
    Adafruit_TFTLCD * m_tft;
    bool m_dirty;
    bool m_done;

    Timer m_tReset;
    uint8_t m_secondsLeft;
    uint8_t m_state;
    enum cal_state {
      CAL_TL = 0,
      CAL_TR,
      CAL_BR,
      CAL_BL,
      CAL_DONE,
      CAL_ACCEPT
    };

    int16_t m_calXMin[2];
    int16_t m_calXMax[2];
    int16_t m_calYMin[2];
    int16_t m_calYMax[2];
};

#endif // TOUCHCALIBRATE_H
