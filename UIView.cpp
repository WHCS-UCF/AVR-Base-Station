#include "UIView.h"

UIView::UIView()
  :m_gfx(NULL), m_dirty(false), m_drawn(false), m_visible(false)
{
  setBounds(0, 0, 0, 0);
  setBackgroundColor(COLOR_BLACK);
}

UIView::UIView(WHCSGfx * gfx)
  :m_gfx(gfx), m_dirty(false), m_drawn(false), m_visible(false)
{
  setBounds(0, 0, 0, 0);
  setBackgroundColor(COLOR_BLACK);
}

UIView::UIView(rect bounds)
  :m_gfx(NULL), m_dirty(false), m_drawn(false), m_visible(false)
{
  setBounds(bounds);
  setBackgroundColor(COLOR_BLACK);
}

UIView::UIView(coord_t x, coord_t y, coord_t w, coord_t h)
  :m_gfx(NULL), m_dirty(false), m_drawn(false), m_visible(false)
{
  setBounds(x, y, w, y);
  setBackgroundColor(COLOR_BLACK);
}

void UIView::setDrawingCtx(WHCSGfx * gfx)
{
  m_gfx = gfx;
}

bool UIView::isDrawn()
{
  return m_drawn;
}

bool UIView::needsDraw()
{
  return m_dirty;
}

void UIView::queueDraw()
{
  m_dirty = true;
}

void UIView::setVisible(bool visible)
{
  if(visible != m_visible) {
    m_visible = visible;
    queueDraw();
  }
}

bool UIView::isVisible()
{
  return m_visible;
}

bool UIView::within(coord_t x, coord_t y)
{
  point p = {x, y};
  return within(&p);
}

bool UIView::within(TouchEvent * ev)
{
  return within(ev->point.x, ev->point.y);
}

bool UIView::within(struct point * pt)
{
  if(pt->x < m_rect.x) return false;
  if(pt->y < m_rect.y) return false;
  if(pt->x >= m_rect.x + m_rect.w) return false;
  if(pt->y >= m_rect.y + m_rect.h) return false;
  return true;
}

void UIView::setBackgroundColor(color_t c)
{
  m_bgColor = c;
}

coord_t UIView::x()
{
  return m_rect.x;
}

coord_t UIView::y()
{
  return m_rect.y;
}

coord_t UIView::w()
{
  return m_rect.w;
}

coord_t UIView::h()
{
  return m_rect.h;
}

point UIView::getXY()
{
  return {m_rect.x, m_rect.y};
}

void UIView::setXY(point xy)
{
  if(m_drawn)
    return;

  m_rect.x = xy.x;
  m_rect.y = xy.y;
}

void UIView::setXY(coord_t x, coord_t y)
{
  if(m_drawn)
    return;

  m_rect.x = x;
  m_rect.y = y;
  m_dirty = true;
}

rect UIView::getBounds()
{
  return m_rect;
}

void UIView::setBounds(coord_t x, coord_t y, coord_t w, coord_t h)
{
  if(m_drawn)
    return;

  m_rect = {x, y, w, h};
}

void UIView::setBounds(rect bounds)
{
  if(m_drawn)
    return;

  m_rect = bounds;
}

void UIView::x(coord_t x)
{
  if(m_drawn)
    return;

  m_rect.x = x;
  m_dirty = true;
}

void UIView::y(coord_t y)
{
  if(m_drawn)
    return;

  m_rect.y = y;
  m_dirty = true;
}

void UIView::w(coord_t w)
{
  if(m_drawn)
    return;

  m_rect.w = w;
  m_dirty = true;
}

void UIView::h(coord_t h)
{
  if(m_drawn)
    return;

  m_rect.h = h;
  m_dirty = true;
}
