#ifndef UICREDITS_H
#define UICREDITS_H

#include "UIScene.h"
#include "Timer.h"

class UICredits : public UIScene
{
  public:
    UICredits(WHCSGfx * gfx);

    void touchEvent(TouchEvent * ev);
    void onCreate();
    void onResume();
    void onDestroy();
    void tick();
    void draw();
  private:
    Timer m_tSlideshow;
    int m_drawState;
};

#endif // !UICREDITS_H
