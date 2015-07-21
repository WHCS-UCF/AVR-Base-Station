#include "UIButton.h"

UIButton::UIButton(WHCSGfx * gfx)
  :UIView(gfx), m_label(NULL), m_borders(false), m_state(STATE_UP)
{

}

void UIButton::setLabel(const char * label)
{
  m_label = label;
}

void UIButton::useBorders(bool which)
{
  m_borders = which;
}

void UIButton::touchEvent(TouchEvent * ev)
{
  state_t newState = m_state;

  if(ev->event == TouchEvent::TOUCH_UP)
    newState = STATE_UP;
  else if(ev->event == TouchEvent::TOUCH_DOWN)
    newState = STATE_DOWN;

  if(newState != m_state) {
    m_dirty = true;
    m_state = newState;
  }
}

void UIButton::draw()
{
  if(m_borders)
  {
    if(m_state == STATE_UP)
      m_gfx->drawBorder(m_rect, COLOR_GREEN);
    else
      m_gfx->drawBorder(m_rect, COLOR_RED);
  }

  if(m_label)
  {
    m_gfx->textSize(2);
    m_gfx->textColor(COLOR_WHITE);
    m_gfx->cursor(x()+5, y()+5);
    m_gfx->puts(m_label);
  }

  m_drawn = true;
  m_dirty = false;
}

void UIButton::invalidate()
{
  m_gfx->clearRect(m_rect, COLOR_BLACK);

  m_drawn = false;
}
