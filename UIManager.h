#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "touchscreen.h"
#include "whcslcd.h"
#include "UIScene.h"
#include "TouchEvent.h"

class UIManager
{
public:
  UIManager(TouchScreen * touch, WHCSLCD * lcd);
  void begin();

  void tick();
  bool needsCalibration();
  void pushUI(UIScene * scene);
  void popUI();
private:
  void tickTouch();

  bool getTouchEvent(TouchEvent * ev);

  // power management
  void activityTick();
  bool activityTimedOut();
  void powerDown(bool fast);
  void powerUp(bool fast);

  // components
  TouchScreen * m_touch;
  WHCSLCD * m_lcd;

  // state
  UIScene * m_curScene;
  UIScene * m_sceneStack[4];
  int8_t m_curScenePos;
#define MAX_SCENES (sizeof(m_sceneStack)/sizeof(*m_sceneStack))

  uint8_t m_touchState;
  TouchEvent m_lastTouchEvent;
  bool m_powerMgmtEnabled;
  bool m_ignoringEvents;
  Timer m_tActivity;
};

#endif // UIMANAGER_H
