#include "TouchCalibrate.h"

#include <stdio.h>
#include "whcsgfx.h"
#include "common.h"

#define TOLERANCE 10
//#define TOUCH_DEBUG

TouchCalibrate::TouchCalibrate(Adafruit_TFTLCD * tft, TouchScreen * touch)
  :UIScene(tft), m_touch(touch), m_state(0)
{
}

void TouchCalibrate::reset()
{
  queueRedraw();
  m_state = 0;
  m_touch->useRawCoords();
  m_tft->fillScreen(COLOR_BLACK);

  printf_P(PSTR("TouchCalibrate::reset()\n"));
}

void TouchCalibrate::touchEvent(TouchEvent * ev)
{
  if(ev->event != TouchEvent::TOUCH_DOWN)
    return;

  // based off of the rotation of the LCD, we need to adjust
  // our orientation
  if(m_tft->getRotation() % 2 == 1) {
    int16_t tmp = ev->point.x;
    ev->point.x = ev->point.y;
    ev->point.y = tmp;
  }

#ifdef TOUCH_DEBUG
  printf("TOUCH_DOWN (%d, %d)\n", ev->point.x, ev->point.y);
#endif

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
    queueRedraw();
  }
  else
  {
    bool hit = true;

    // super hax to detect touch in buttons
    if(ev->point.y >= m_tft->height()-60 && ev->point.y <= m_tft->height()-35)
    {
      if(ev->point.x >= 50 && ev->point.x <= 150)
        end();
      else if(ev->point.x >= 160 && ev->point.x <= 260) {
        reset();
      }
      else
        hit = false;
    }
    else
      hit = false;

    if(!hit)
      m_tft->drawPixel(ev->point.x, ev->point.y, COLOR_WHITE);
  }
}

void TouchCalibrate::onCreate()
{
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
      int xMin = (m_calXMin[0]+m_calXMin[1])/2;
      int xMax = (m_calXMax[0]+m_calXMax[1])/2;

      int yMin = (m_calYMin[0]+m_calYMin[1])/2;
      int yMax = (m_calYMax[0]+m_calYMax[1])/2;
#ifdef TOUCH_DEBUG
      printf("Display calibrated: Xmin %d, XMax %d, Ymin %d, Ymax %d\n",
          xMin, xMax, yMin, yMax);
#endif

      int16_t w = m_tft->width();
      int16_t h = m_tft->height();

      if(m_tft->getRotation() % 2 == 1) {
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

      // start a timer to reset the calibration if the user doesn't
      // accept after a certain amount of time
      // this is assuming that the user is unable to accept
      m_tReset.periodic(1000);

      m_state = CAL_ACCEPT;
      m_secondsLeft = 16;
      queueRedraw();
    }
    else
    {
#ifdef TOUCH_DEBUG
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

  int16_t squareTableX[4] = {0, m_tft->width()-10, m_tft->width()-10, 0};
  int16_t squareTableY[4] = {0, 0, m_tft->height()-10, m_tft->height()-10};

  for(int i = 0; i < 4; i++) {
    uint16_t color;

    if(i < m_state)
      color = COLOR_GREEN;
    else if(i > m_state)
      color = COLOR_WHITE;
    else
      color = COLOR_RED;

    m_tft->fillRect(squareTableX[i], squareTableY[i], 10, 10, color);
  }

  m_tft->setTextSize(2);

  if(m_state < CAL_DONE)
  {
    m_tft->setCursor(20, m_tft->height()/2-20);
    m_tft->setTextColor(COLOR_WHITE);
    m_tft->println("Touchscreen Calibration\n");

    // clear the text area
    m_tft->fillRect(0, m_tft->height()/2, m_tft->width(), 30, COLOR_BLACK);
    m_tft->setCursor(35, m_tft->height()/2);
    m_tft->println("Touch the ");
    m_tft->setTextColor(COLOR_RED);

    if(m_state == CAL_TL)
      m_tft->println("top-left");
    else if(m_state == CAL_TR)
      m_tft->println("top-right");
    else if(m_state == CAL_BR)
      m_tft->println("bottom-right");
    else if(m_state == CAL_BL)
      m_tft->println("bottom-left");
  }
  else
  {
    m_tft->setCursor(35, m_tft->height()/2-20);
    m_tft->setTextColor(COLOR_GREEN);
    m_tft->fillRect(20, m_tft->height()/2-20, m_tft->width(), 70, COLOR_BLACK);
    m_tft->println("Calibration Complete");

    m_tft->drawRect(50, m_tft->height()-60, 100, 25, COLOR_GREEN);
    m_tft->setCursor(65, m_tft->height()-55);
    m_tft->println("Accept");

    m_tft->drawRect(160, m_tft->height()-60, 100, 25, COLOR_RED);
    m_tft->setCursor(65+100+15, m_tft->height()-55);
    m_tft->setTextColor(COLOR_RED);
    m_tft->println("Reset");
  }

  if(m_state == CAL_ACCEPT)
  {
    //m_secondsLeft
    m_tft->setCursor(55, m_tft->height()-90);
    m_tft->setTextColor(COLOR_WHITE);
    m_tft->println("Reverting in");
    m_tft->printf("   \b\b%d", m_secondsLeft);
  }

  UIScene::draw();
}

///////////////////////////////

bool TouchCalibrate::checkCal()
{
#ifdef TOUCH_DEBUG
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
