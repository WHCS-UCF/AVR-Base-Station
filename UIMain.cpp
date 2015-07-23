#include "UIMain.h"

UIMain::UIMain(WHCSGfx * gfx)
  :UIScene(gfx), m_vSettingsButton(gfx), m_drawFrame(false)
{

}

void UIMain::touchEvent(TouchEvent * ev)
{
  if(m_vSettingsButton.within(ev)) {
    if(m_vSettingsButton.touchEvent(ev))
    {
      printf_P(PSTR("Settings button hit\n"));
      queueRedraw();
      return;
    }
  }
  else
  {
    m_vSettingsButton.noEvent();
  }

  for(int i = 0; i < m_numCtrl; i++) {
      if(m_vCtrl[i].within(ev)) {
        m_vCtrl[i].touchEvent(ev);
        queueRedraw();
      }
      else
      {
        m_vCtrl[i].noEvent();
        queueRedraw();
      }
  }

  m_gfx->pixel(ev->point.x, ev->point.y, COLOR_BLACK);
}

void UIMain::onCreate()
{
  m_vSettingsButton.useBorders(true);
  m_vSettingsButton.setBackgroundColor(COLOR_LIGHTGREY);
  m_vSettingsButton.setForegroundColor(COLOR_BLACK);
  m_vSettingsButton.setBounds(m_gfx->width()-120, 10, 105, 25);
  m_vSettingsButton.setLabel("Settings");
  m_vSettingsButton.setVisible(true);

  m_drawFrame = true;
}

void UIMain::onDestroy()
{

}

void UIMain::tick()
{
  for(int i = 0; i < m_numCtrl; i++) {
    if(m_vCtrl[i].hasUpdate()) {
      m_vCtrl[i].queueDraw();
      m_vCtrl[i].clearUpdate();
      queueRedraw();
      break;
    }
  }
}

void UIMain::draw()
{
  if(m_drawFrame)
  {
    m_gfx->clearScreen(COLOR_WHITE);

    m_gfx->cursor(10,10);
    m_gfx->textSize(3);
    m_gfx->textColor(0xe51a);
    m_gfx->puts("W H C S");
    m_gfx->line({0, 45}, {m_gfx->width()-1, 45}, COLOR_RED);

    m_drawFrame = false;
  }

  /*for(int i = 0; i < m_numControlModules; i++)
  {
    drawControlModule(...);
  }*/

  if(m_vSettingsButton.needsDraw())
    m_vSettingsButton.draw();

  for(int i = 0; i < m_numCtrl; i++) {
    if(m_vCtrl[i].needsDraw())
      m_vCtrl[i].draw();
  }

  UIScene::draw();
}

void UIMain::setModules(UIControlMod * mods, int num)
{
  rect bounds = {0, 45, m_gfx->width()-1, 45};
  for(int i = 0; i < num; i++) {
    mods[i].getModule()->printDetails();
    mods[i].setBounds(bounds);
    mods[i].setVisible(true);
    mods[i].onCreate(); // must be called last
    bounds.y += 45;
  }

  m_vCtrl = mods;
  m_numCtrl = num;
}
