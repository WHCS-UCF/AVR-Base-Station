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
  bool getTouchEvent(TouchEvent * ev);
  void setTopLevelUI(UIScene * scene);
private:
  // components
  TouchScreen * m_touch;
  WHCSLCD * m_lcd;

  // state
  UIScene * m_curScene;
  uint8_t m_touchState;
};

#endif // UIMANAGER_H
