#ifndef UICONTROLMOD_H
#define UICONTROLMOD_H

#include "ControlModule.h"
#include "UIView.h"
#include "UIButton.h"

class UIControlMod : public UIView
{
  public:
    UIControlMod(WHCSGfx * gfx, ControlModule * ctrl);

    void onCreate();
    void touchEvent(TouchEvent * ev);
    void draw();
    void invalidate();

    ControlModule * getModule();
  private:
    ControlModule * m_ctrl;
    UIButton m_vAction;
};

#endif // UICONTROLMOD_H
