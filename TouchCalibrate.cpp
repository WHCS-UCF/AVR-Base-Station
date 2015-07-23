#include "TouchCalibrate.h"

#include <stdio.h>
#include "whcsgfx.h"
#include "common.h"
#include "eeprom.h"

#define TOLERANCE 10
#undef DEBUG_CAL

TouchCalibrate::TouchCalibrate(WHCSGfx * gfx, TouchScreen * touch)
  :UIScene(gfx), m_newState(false), m_touch(touch), m_state(0), m_vAcceptButton(gfx), m_vResetButton(gfx)
{
}

void TouchCalibrate::reset()
{
  m_state = 0;
  m_newState = true;
  m_touch->useRawCoords();
  m_gfx->clearScreen(COLOR_BLACK);

#ifdef DEBUG_CAL
  printf_P(PSTR("TouchCalibrate::reset()\n"));
#endif

  queueRedraw();
}

void TouchCalibrate::touchEvent(TouchEvent * ev)
{
  if(m_vAcceptButton.isDrawn() && m_vAcceptButton.within(ev->point.x, ev->point.y))
  {
    // save calibration
    EEPROM::saveCalibration(xMin, xMax, yMin, yMax);

    end();
    queueRedraw();
    m_vAcceptButton.touchEvent(ev);
    return;
  }

  if(m_vResetButton.isDrawn() && m_vResetButton.within(ev->point.x, ev->point.y))
  {
    reset();
    queueRedraw();
    m_vResetButton.touchEvent(ev);
    return;
  }

  if(ev->event != TouchEvent::TOUCH_DOWN)
    return;

  if(m_state == CAL_TL) {
    m_calXMin[0] = ev->point.x;
    m_calYMin[0] = ev->point.y;
  }
  else if(m_state == CAL_TR) {
    m_calXMax[0] = ev->point.x;
    m_calYMin[1] = ev->point.y;
  }
  else if(m_state == CAL_BR) {
    m_calXMax[1] = ev->point.x;
    m_calYMax[0] = ev->point.y;
  }
  else if(m_state == CAL_BL) {
    m_calXMin[1] = ev->point.x;
    m_calYMax[1] = ev->point.y;
  }

  if(m_state < CAL_DONE) {
    m_state++;
    m_newState = true;
    queueRedraw();
  }
  else
  {
    m_gfx->pixel(ev->point.x, ev->point.y, COLOR_WHITE);
  }
}

void TouchCalibrate::onCreate()
{
  m_vAcceptButton.setForegroundColor(COLOR_WHITE);
  m_vAcceptButton.useBorders(true);
  m_vAcceptButton.setBounds(50, m_gfx->height()-60, 100, 25);
  m_vAcceptButton.setLabel("Accept");
  m_vAcceptButton.setVisible(true);

  m_vResetButton.setForegroundColor(COLOR_WHITE);
  m_vResetButton.useBorders(true);
  m_vResetButton.setBounds(160, m_gfx->height()-60, 100, 25);
  m_vResetButton.setLabel("Reset");
  m_vResetButton.setVisible(true);

  reset();
}

void TouchCalibrate::onDestroy()
{
}

void TouchCalibrate::tick()
{
  if(m_state == CAL_DONE)
  {
    if(checkCal())
    {
      xMin = (m_calXMin[0]+m_calXMin[1])/2;
      xMax = (m_calXMax[0]+m_calXMax[1])/2;

      yMin = (m_calYMin[0]+m_calYMin[1])/2;
      yMax = (m_calYMax[0]+m_calYMax[1])/2;
#ifdef DEBUG_CAL
      printf("Display calibrated: Xmin %d, XMax %d, Ymin %d, Ymax %d\n",
          xMin, xMax, yMin, yMax);
#endif

      setCalibration(xMin, xMax, yMin, yMax);

      // start a timer to reset the calibration if the user doesn't
      // accept after a certain amount of time
      // this is assuming that the user is unable to accept
      m_tReset.periodic(1000);

      m_newState = true;
      m_state = CAL_ACCEPT;
      m_secondsLeft = 16;

      m_vAcceptButton.queueDraw();
      m_vResetButton.queueDraw();
      queueRedraw();
    }
    else
    {
#ifdef DEBUG_CAL
      printf("Calibration failed\n");
#endif
      reset();
    }
  }
  else if(m_state == CAL_ACCEPT)
  {
    if(m_tReset.update())
    {
      queueRedraw();
      m_secondsLeft--;

      if(m_secondsLeft <= 0)
        reset();
    }
  }
}

void TouchCalibrate::draw()
{
  if(m_vAcceptButton.needsDraw())
    m_vAcceptButton.draw();
  if(m_vResetButton.needsDraw())
    m_vResetButton.draw();

  int16_t squareTableX[4] = {0, m_gfx->width()-10, m_gfx->width()-10, 0};
  int16_t squareTableY[4] = {0, 0, m_gfx->height()-10, m_gfx->height()-10};

  for(int i = 0; i < 4; i++) {
    uint16_t color;

    if(i < m_state)
      color = COLOR_GREEN;
    else if(i > m_state)
      color = COLOR_WHITE;
    else
      color = COLOR_RED;

    m_gfx->fillRect(squareTableX[i], squareTableY[i], 10, 10, color);
  }

  m_gfx->textSize(2);

  if(m_state < CAL_DONE)
  {
    m_gfx->cursor(20, m_gfx->height()/2-20);
    m_gfx->textColor(COLOR_WHITE);
    m_gfx->puts("Touchscreen Calibration\n");

    // clear the text area
    m_gfx->fillRect(0, m_gfx->height()/2, m_gfx->width(), 30, COLOR_BLACK);
    m_gfx->cursor(35, m_gfx->height()/2);
    m_gfx->puts("Touch the ");
    m_gfx->textColor(COLOR_RED);

    if(m_state == CAL_TL)
      m_gfx->puts("top-left");
    else if(m_state == CAL_TR)
      m_gfx->puts("top-right");
    else if(m_state == CAL_BR)
      m_gfx->puts("bottom-right");
    else if(m_state == CAL_BL)
      m_gfx->puts("bottom-left");
  }
  else
  {
    m_gfx->cursor(35, m_gfx->height()/2-20);
    m_gfx->textColor(COLOR_GREEN);
    m_gfx->fillRect(20, m_gfx->height()/2-20, m_gfx->width(), 70, COLOR_BLACK);
    m_gfx->puts("Calibration Complete");

    //m_gfx->drawBorder(50, m_gfx->height()-60, 100, 25, COLOR_GREEN);
    //m_gfx->cursor(65, m_gfx->height()-55);
    //m_gfx->puts("Accept");

    //m_gfx->drawBorder(160, m_gfx->height()-60, 100, 25, COLOR_RED);
    //m_gfx->cursor(65+100+15, m_gfx->height()-55);
    //m_gfx->textColor(COLOR_RED);
    //m_gfx->puts("Reset");
  }

  if(m_state == CAL_ACCEPT)
  {
    //m_secondsLeft
    m_gfx->cursor(55, m_gfx->height()-90);
    m_gfx->textColor(COLOR_WHITE);
    m_gfx->puts("Reverting in");
    //m_gfx->printf("   \b\b%d", m_secondsLeft);
  }

  UIScene::draw();
}

void TouchCalibrate::setCalibration(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax)
{
  int16_t w = m_gfx->width();
  int16_t h = m_gfx->height();

  if(m_gfx->getRotation() % 2 == 1) {
    int16_t tmp = w;
    w = h;
    h = tmp;

    tmp = yMin;
    yMin = xMin;
    xMin = tmp;

    tmp = yMax;
    yMax = xMax;
    xMax = tmp;
  }

  m_touch->setCalibration(w, h,
      xMin, xMax, yMin, yMax);
  m_touch->useScreenCoords();
}

///////////////////////////////

bool TouchCalibrate::checkCal()
{
#ifdef DEBUG_CAL
  printf("%d ", abs(m_calXMin[0] - m_calXMin[1]));
  printf("%d ", abs(m_calXMax[0] - m_calXMax[1]));
  printf("%d ", abs(m_calYMin[0] - m_calYMin[1]));
  printf("%d ", abs(m_calYMax[0] - m_calYMax[1]));
#endif

  if(abs(m_calXMin[0] - m_calXMin[1]) > TOLERANCE)
    return false;
  if(abs(m_calXMax[0] - m_calXMax[1]) > TOLERANCE)
    return false;
  if(abs(m_calYMin[0] - m_calYMin[1]) > TOLERANCE)
    return false;
  if(abs(m_calYMax[0] - m_calYMax[1]) > TOLERANCE)
    return false;

  return true;
}
