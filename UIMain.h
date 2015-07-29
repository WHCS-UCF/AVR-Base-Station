#ifndef UIMAIN_H
#define UIMAIN_H

#include "UIScene.h"
#include "UIButton.h"
#include "UIControlMod.h"
#include "UICredits.h"
#include "UIManager.h"

class UIMain : public UIScene
{
  public:
    UIMain(WHCSGfx * gfx, UIManager * mgr);

    void touchEvent(TouchEvent * ev);
    void onCreate();
    void onResume();
    void onDestroy();
    void tick(); 
    void draw();

    void setModules(UIControlMod * mods, int num);
  private:
    UIControlMod * m_vCtrl;
    int m_numCtrl;
    UIButton m_vSettingsButton;
    bool m_drawFrame;

    UICredits m_credits;
    UIManager * m_mgr;
};

#endif // UIMAIN_H

