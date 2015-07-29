#ifndef UICONTROLMOD_H
#define UICONTROLMOD_H

#include "UIView.h"
#include "UIButton.h"
#include "ControlModule.h"

class ControlModule;

class UIControlMod : public UIView
{
  public:
    UIControlMod(WHCSGfx * gfx, ControlModule * ctrl);

    void onCreate();
    bool touchEvent(TouchEvent * ev);
    void redrawAll();
    void noEvent();
    void doAction();
    void draw();
    void invalidate();

    // pub sub
    void gotUpdate();
    bool hasUpdate();
    void clearUpdate();

    ControlModule * getModule();
  private:
    ControlModule * m_ctrl;
    UIButton m_vAction;
    bool m_drawFrame;
    bool m_hasUpdate;
};

#endif // UICONTROLMOD_H
