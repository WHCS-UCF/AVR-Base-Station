#include "UIManager.h"

#include <stdio.h>
#include <avr/pgmspace.h>

#define SCREEN_TIMEOUT_MS 60000
#undef DEBUG_POWER_MGMT
#undef DEBUG_TOUCH

UIManager::UIManager(TouchScreen * touch, WHCSLCD * lcd)
  :m_touch(touch), m_lcd(lcd), m_touchState(TouchEvent::TOUCH_UP)
 // start with the touch up
{
  m_powerMgmtEnabled = true;
  m_ignoringEvents = false;
}

void UIManager::begin()
{
  m_lcd->begin();
  m_lcd->clearScreen();
  powerUp(false);

  m_tActivity.once(SCREEN_TIMEOUT_MS);

  printf_P(PSTR("UIManager: ready to display\n"));
}

void UIManager::tick()
{
  if(m_curScene && m_curScene->isDone())
    setTopLevelUI(NULL);

  tickTouch();

  // handle activity timeout
  if(activityTimedOut())
  {
    if(m_touchState == TouchEvent::TOUCH_UP)
      powerDown(false);
    else
    {
      activityTick();

#ifdef DEBUG_POWER_MGMT
      printf_P(PSTR("UIManager: not sleeping due to down finger\n"));
#endif
    }

  }

  m_lcd->tick();

  if(m_curScene) {
    m_curScene->tick();

    if(m_curScene->isDirty())
      m_curScene->draw();
  }
}

void UIManager::tickTouch()
{
  TouchEvent te;

  if(getTouchEvent(&te)) {
    activityTick();

    if(m_lcd->isVisible())
    {
      // stop an in progress power down quickly
      if(m_lcd->isPoweringOff())
        powerUp(true);

      if(!m_ignoringEvents)
      {
        if(m_curScene) // if we are managing a scene, dispatch touch event
          m_curScene->touchEvent(&te);
        else
          printf_P(PSTR("UIManager: lost event (no scene)\n"));
      }
      else
      {
#ifdef DEBUG_POWER_MGMT
        printf_P(PSTR("UIManager: ignoring event as touch down not seen\n"));
#endif
      }
    }
    else
    {
      if(!m_ignoringEvents) 
      {
        powerUp(false);
#ifdef DEBUG_POWER_MGMT
        printf_P(PSTR("UIManager: ignored touch down\n"));
#endif
      }

      m_ignoringEvents = true;
    }

    if(m_ignoringEvents && te.event == TouchEvent::TOUCH_UP)
      m_ignoringEvents = false;
  }
}

void UIManager::activityTick()
{
  if(!m_powerMgmtEnabled)
    return;

  m_tActivity.restart();
}

bool UIManager::activityTimedOut()
{
  if(!m_powerMgmtEnabled)
    return false;

  //printf("Calling activity timed out\n");
  //printf("Timer addr 0x%0x\n", m_tActivity);
  bool test = m_tActivity.update();
  //printf("Done\n");

  return test;
}

void UIManager::powerDown(bool fast)
{
  if(!m_powerMgmtEnabled)
    return;

  if(fast)
    m_lcd->screenOff();
  else
    m_lcd->screenFadeDown();

#ifdef DEBUG_POWER_MGMT
    printf_P(PSTR("UIManager: sleeping\n"));
#endif
}

void UIManager::powerUp(bool fast)
{
  if(!m_powerMgmtEnabled)
    return;

  if(fast)
    m_lcd->screenOn();
  else
    m_lcd->screenFadeUp();

#ifdef DEBUG_POWER_MGMT
    printf_P(PSTR("UIManager: waking up\n"));
#endif
}

bool UIManager::needsCalibration()
{
  return true;
}

bool UIManager::getTouchEvent(TouchEvent * te)
{
  if(!te)
    return false;

  TSPoint p = m_touch->getPoint();
  bool result = false;

  if(p.valid && p.z != 0)
  {
    if(m_touchState == TouchEvent::TOUCH_UP) {
#ifdef DEBUG_TOUCH
      printf_P(PSTR("UIManager: TOUCH_DOWN (%d, %d)\n"), p.x, p.y);
#endif

      te->point = p;
      te->event = TouchEvent::TOUCH_DOWN;
      m_touchState = te->event;
      result = true;
      //m_lastTouchEvent = *ev;
    }

    //printf("z = %d\n", p.z);

    //if(abs(p.x-pLast.x) > 2 || abs(p.y-pLast.y) > 2);
      //printf("TOUCH_MOVE (%d, %d)\n", p.x, p.y);
  }
  else if(p.valid && p.z == 0)
  {
    if(m_touchState == TouchEvent::TOUCH_DOWN) {
#ifdef DEBUG_TOUCH
      printf_P(PSTR("UIManager: TOUCH_UP (%d, %d)\n"), p.x, p.y);
#endif

      te->point = p;
      te->event = TouchEvent::TOUCH_UP;
      m_touchState = te->event;
      result = true;
      //m_lastTouchEvent = *ev;
    }
  }

  return result;
}

void UIManager::setTopLevelUI(UIScene * scene)
{
  if(m_curScene != NULL) {
    if(!m_curScene->isDone()) {
      printf_P(PSTR("UIManager: WARN scene transition without end\n"));
    }

    printf_P(PSTR("UIManager: calling onDestroy()\n"));
    m_curScene->onDestroy();

    // XXX: remove this
    m_lcd->clearScreen();
  }

  if(scene)
  {
    printf_P(PSTR("UIManager: calling onCreate()\n"));
    scene->onCreate();
  }

  m_curScene = scene;
}
