#ifndef UIVIEW_H
#define UIVIEW_H

#include <stdint.h>
#include "whcsgfx.h"
#include "TouchEvent.h"

class UIView
{
  public:
    UIView();
    UIView(WHCSGfx * gfx);
    UIView(rect bounds);
    UIView(coord_t x, coord_t y, coord_t w, coord_t h);

    void setDrawingCtx(WHCSGfx * gfx);
    virtual void touchEvent(TouchEvent * ev) = 0;
    virtual void draw() = 0;
    virtual void invalidate() = 0;

    bool needsDraw();
    bool isDrawn();
    void queueDraw();
    void setVisible(bool visible);
    bool isVisible();

    bool within(coord_t x, coord_t y);
    bool within(point * pt);
    bool within(TouchEvent * ev);

    void setBackgroundColor(color_t c);

    coord_t x();
    coord_t y();
    coord_t w();
    coord_t h();

    point getXY();
    void setXY(point xy);
    void setXY(coord_t x, coord_t y);

    rect getBounds();
    void setBounds(coord_t x, coord_t y, coord_t w, coord_t h);
    void setBounds(rect bounds);

    void x(coord_t);
    void y(coord_t);
    void w(coord_t);
    void h(coord_t);

  protected:
    WHCSGfx * m_gfx;
    rect m_rect;

    color_t m_bgColor;
    bool m_dirty; // draw state tracking
    bool m_drawn;
    bool m_visible; // user specified visibility
};

#endif // UIVIEW_H
