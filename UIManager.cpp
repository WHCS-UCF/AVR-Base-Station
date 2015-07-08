#include "UIManager.h"

#include <stdio.h>
#include <avr/pgmspace.h>

UIManager::UIManager(TouchScreen * touch, WHCSLCD * lcd)
  :m_touch(touch), m_lcd(lcd), m_touchState(TouchEvent::TOUCH_UP)
 // start with the touch up
{

}

void UIManager::begin()
{
  m_lcd->begin();
  m_lcd->clearScreen();
  m_lcd->fadeUp();

  printf_P(PSTR("UIManger: ready to display\n"));
}

void UIManager::tick()
{
  TouchEvent te;

  if(getTouchEvent(&te)) {
    if(m_curScene) // if we are managing a scene, dispatch touch event
      m_curScene->touchEvent(&te);
    else
      printf_P(PSTR("UIManager: WARN lost touch event\n"));
  }

  m_lcd->tick();

  if(m_curScene) {
    m_curScene->tick();

    if(m_curScene->isDirty())
      m_curScene->draw();
  }
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
      //printf("TOUCH_DOWN (%d, %d)\n", p.x, p.y);
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
      //printf("TOUCH_UP (%d, %d)\n", pLast.x, pLast.y);
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
  if(!scene)
    return;

  if(m_curScene != NULL) {
    // TODO: check to see if the scene had ended yet
    if(!m_curScene->done()) {
      printf_P(PSTR("UIManager: WARN scene transition without end\n"));
    }

    printf_P(PSTR("UIManager: calling onDestroy()\n"));
    m_curScene->onDestroy();
  }


  printf_P(PSTR("UIManager: calling onCreate()\n"));
  scene->onCreate();
  m_curScene = scene;
}
