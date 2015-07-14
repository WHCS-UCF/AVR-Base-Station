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
  void setTopLevelUI(UIScene * scene);
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
  uint8_t m_touchState;
  bool m_powerMgmtEnabled;
  bool m_ignoringEvents;
  Timer m_tActivity;
};

#endif // UIMANAGER_H
