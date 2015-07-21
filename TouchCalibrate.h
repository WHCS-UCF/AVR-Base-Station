#ifndef TOUCHCALIBRATE_H
#define TOUCHCALIBRATE_H

#include <stdint.h>

#include <Adafruit_TFTLCD.h>
#include "touchscreen.h"
#include "TouchEvent.h"
#include "Timer.h"
#include "UIScene.h"
#include "UIButton.h"

class TouchCalibrate : public UIScene
{
  public:
    TouchCalibrate(WHCSGfx * gfx, TouchScreen * touch);
    void reset();

    void touchEvent(TouchEvent * ev); // called on a touch event
    void onCreate(); // called when scene first created
    void onDestroy();
    void tick(); 
    void draw();
    void setCalibration(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax);
  private:
    bool checkCal();

    TouchScreen * m_touch;
    UIButton m_vAcceptButton;
    UIButton m_vResetButton;

    Timer m_tReset;
    uint8_t m_secondsLeft;
    uint8_t m_state;
    bool m_newState;
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
