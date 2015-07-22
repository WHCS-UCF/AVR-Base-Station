#ifndef UIMAIN_H
#define UIMAIN_H

#include "UIScene.h"
#include "UIButton.h"
#include "UIControlMod.h"

class UIMain : public UIScene
{
  public:
    UIMain(WHCSGfx * gfx);

    void touchEvent(TouchEvent * ev);
    void onCreate();
    void onDestroy();
    void tick(); 
    void draw();

    void setModules(UIControlMod * mods, int num);
  private:
    UIControlMod * m_vCtrl;
    int m_numCtrl;
    UIButton m_vSettingsButton;
    bool m_drawFrame;
};

#endif // UIMAIN_H

