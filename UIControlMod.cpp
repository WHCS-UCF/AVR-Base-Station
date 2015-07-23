#include "UIControlMod.h"

UIControlMod::UIControlMod(WHCSGfx * gfx, ControlModule * ctrl)
  :UIView(gfx), m_ctrl(ctrl), m_vAction(gfx), m_drawFrame(true)
{
  m_hasUpdate = false;
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

bool UIControlMod::touchEvent(TouchEvent * ev)
{
  if(m_vAction.within(ev))
  {
    queueDraw();

    if(m_vAction.touchEvent(ev))
    {
      doAction();
      return true;
    }
  }
  else
  {
    this->noEvent();
  }

  return false;
}

void UIControlMod::noEvent()
{
  queueDraw();
  m_vAction.noEvent();
}

void UIControlMod::doAction()
{
  printf_P(PSTR("%s action: "), m_ctrl->getName());

  uint8_t state;

  if(m_ctrl->getRole() == ROLE_AC_SWITCH)
  {
    state = !m_ctrl->getACState();
    m_ctrl->setACState(state, PUB_LCD);
    printf_P(PSTR("AC %d\n"), state);
  }
  else if(m_ctrl->getRole() == ROLE_DC_SWITCH)
  {
    state = !m_ctrl->getDCState();
    m_ctrl->setDCState(state, PUB_LCD);
    printf_P(PSTR("DC %d\n"), state);
  }
}

void UIControlMod::draw()
{
  if(m_drawFrame)
  {
    m_gfx->cursor(m_rect.x+10, m_rect.y+m_rect.h/2-3);
    m_gfx->textSize(2);
    m_gfx->textColor(COLOR_BLACK);
    m_gfx->puts(m_ctrl->getName());
    m_gfx->line({m_rect.x, m_rect.y+m_rect.h},
        {m_rect.x+m_rect.w, m_rect.y+m_rect.h}, COLOR_BLACK);

    m_drawFrame = false;
  }

  uint8_t state;
  color_t color;
  cm_role_t role = m_ctrl->getRole();


  if(role == ROLE_AC_SWITCH)
    state = m_ctrl->getACState();
  else if(role == ROLE_DC_SWITCH)
    state = m_ctrl->getDCState();

  if(role == ROLE_DC_SWITCH || role == ROLE_AC_SWITCH)
  {
    if(state)
      color = COLOR_GREEN;
    else
      color = COLOR_RED;

    m_gfx->fillRect(m_rect.x+120, m_rect.y+m_rect.h/2-3, 10, 10, color);
  }

  if(m_vAction.needsDraw())
    m_vAction.draw();

  m_drawn = true;
  m_dirty = false;
}

void UIControlMod::invalidate()
{

}

void UIControlMod::gotUpdate()
{
  m_hasUpdate = true;
}

bool UIControlMod::hasUpdate()
{
  return m_hasUpdate;
}

void UIControlMod::clearUpdate()
{
  m_hasUpdate = false;
}

ControlModule * UIControlMod::getModule()
{
  return m_ctrl;
}
