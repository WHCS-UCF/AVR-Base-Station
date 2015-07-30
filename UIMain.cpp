#include "UIMain.h"

UIMain::UIMain(WHCSGfx * gfx, UIManager * mgr)
  :UIScene(gfx), m_vSettingsButton(gfx), m_drawFrame(false), m_mgr(mgr), m_credits(gfx)
{

}

void UIMain::touchEvent(TouchEvent * ev)
{
  if(m_vSettingsButton.within(ev)) {
    if(m_vSettingsButton.touchEvent(ev))
    {
      printf_P(PSTR("Settings button hit\n"));
      m_mgr->pushUI(&m_credits);
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

  //m_gfx->pixel(ev->point.x, ev->point.y, COLOR_BLACK);
}

void UIMain::onResume()
{
  m_drawFrame = true;
  for(int i = 0; i < m_numCtrl; i++)
    m_vCtrl[i].redrawAll();

  queueRedraw();
}

void UIMain::onCreate()
{
  m_vSettingsButton.useBorders(true);
  m_vSettingsButton.setBackgroundColor(COLOR_LIGHTGREY);
  m_vSettingsButton.setForegroundColor(COLOR_BLACK);
  m_vSettingsButton.setBounds(m_gfx->width()-115, 10, 105, 25);
  m_vSettingsButton.setLabel("Credits");
  m_vSettingsButton.setVisible(true);

  onResume();
}

void UIMain::onDestroy()
{

}

void UIMain::tick()
{
  for(int i = 0; i < m_numCtrl; i++) {
    if(m_vCtrl[i].hasUpdate()) {
      m_vCtrl[i].queueDraw();
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
    point p1 = {0, 40};
    point p2 = {m_gfx->width()-1, 40};
    uint16_t whcsColors[] = {COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN};
    for(int i = 0; i < sizeof(whcsColors)/sizeof(*whcsColors); i++)
      for(int j = 0; j < 2; j++, p1.y++, p2.y++)
        m_gfx->line(p1, p2, whcsColors[i]);

    //m_gfx->line({0, 42}, {m_gfx->width()-1, 42}, COLOR_RED);
    //m_gfx->line({0, 43}, {m_gfx->width()-1, 43}, COLOR_ORANGE);
    //m_gfx->line({0, 44}, {m_gfx->width()-1, 44}, COLOR_YELLOW);
    //m_gfx->line({0, 45}, {m_gfx->width()-1, 45}, COLOR_GREEN);

    m_drawFrame = false;
  }

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
  rect bounds = {0, 48, m_gfx->width()-1, 48};
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
