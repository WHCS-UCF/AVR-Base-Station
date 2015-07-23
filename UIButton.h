#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "UIView.h"

class UIButton : public UIView
{
  public:
    UIButton(WHCSGfx * gfx);

    void setLabel(const char * label);
    void useBorders(bool which);

    void setForegroundColor(color_t fg);

    bool touchEvent(TouchEvent * ev);
    void noEvent();
    void draw();
    void invalidate();
  private:
    const char * m_label;
    bool m_borders;
    color_t m_fgColor;

    typedef uint8_t state_t;
    enum button_state_t { STATE_DOWN, STATE_UP };
    state_t m_state;
};

#endif // UIBUTTON_H
