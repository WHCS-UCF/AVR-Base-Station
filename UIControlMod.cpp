#include "UIControlMod.h"

UIControlMod::UIControlMod(WHCSGfx * gfx, ControlModule * ctrl)
  :UIView(gfx), m_ctrl(ctrl), m_vAction(gfx)
{
}

void UIControlMod::onCreate()
{
  m_vAction.setVisible(true);
  if(m_ctrl->getRole() == ROLE_AC_SWITCH)
  {
    m_vAction.setVisible(true);
    m_vAction.useBorders(true);
    m_vAction.setLabel("Toggle");
    m_vAction.setBounds(m_rect.x+m_rect.w-100,
        m_rect.y+10, 90, 32);
  }
  else if(m_ctrl->getRole() == ROLE_DC_SWITCH)
  {
    m_vAction.setVisible(true);
    m_vAction.useBorders(true);
    m_vAction.setLabel("Toggle");
    m_vAction.setBounds(m_rect.x+m_rect.w-100,
        m_rect.y+10, 90, 32);
  }
  else
    m_vAction.setVisible(false);
}

void UIControlMod::touchEvent(TouchEvent * ev)
{
  if(m_vAction.within(ev))
  {
    printf("%s action\n", m_ctrl->getName());
    m_vAction.touchEvent(ev);
    queueDraw();
    return;
  }
}

void UIControlMod::draw()
{
  m_gfx->cursor(m_rect.x+10, m_rect.y+m_rect.h/2-3);
  m_gfx->textSize(2);
  m_gfx->textColor(COLOR_BLACK);
  m_gfx->puts(m_ctrl->getName());
  m_gfx->line({m_rect.x, m_rect.y+m_rect.h},
      {m_rect.x+m_rect.w, m_rect.y+m_rect.h}, COLOR_BLACK);

  if(m_vAction.needsDraw())
    m_vAction.draw();

  m_drawn = true;
  m_dirty = false;
}

void UIControlMod::invalidate()
{

}

ControlModule * UIControlMod::getModule()
{
  return m_ctrl;
}
