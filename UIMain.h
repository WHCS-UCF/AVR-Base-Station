#ifndef UIMAIN_H
#define UIMAIN_H

#include "UIScene.h"

class UIMain : public UIScene
{
  public:
    UIMain(WHCSGfx * gfx);

    void touchEvent(TouchEvent * ev);
    void onCreate();
    void onDestroy();
    void tick(); 
    void draw();
};

#endif // UIMAIN_H

