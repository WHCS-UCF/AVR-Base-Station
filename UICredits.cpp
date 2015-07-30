#include "UICredits.h"

#include <avr/pgmspace.h>

#include "img/grant.h"
#include "img/jimmy.h"
#include "img/ucf.h"
#include "img/joesph.h"

UICredits::UICredits(WHCSGfx * gfx)
  :UIScene(gfx), m_drawState(0)
{

}

void UICredits::touchEvent(TouchEvent * ev)
{
  if(ev->event == TouchEvent::TOUCH_DOWN)
  {
    end();
    return;
  }
}

void UICredits::onCreate()
{
  m_gfx->clearScreen(COLOR_BLACK);
  m_drawState = 0;

  UIScene::onCreate();
}

void UICredits::onResume()
{
}

void UICredits::onDestroy()
{

}

void UICredits::tick()
{
  if(m_tSlideshow.update())
  {
    m_drawState = (m_drawState + 1) % 3;
    queueRedraw();
  }
}

void UICredits::draw()
{
  m_gfx->clearScreen(COLOR_BLACK);
  const char * names[] = {"Joseph Love", "Jimmy Campbell", "Grant Hernandez"};
  uint8_t adjusts[] = {m_gfx->width()/2-6*11, m_gfx->width()/2-6*14, m_gfx->width()/2-6*15};


  if(m_drawState == 0)
  {
    m_gfx->drawAsciiArt(20, 0, gImageJoesph, sizeof(gImageJoesph), COLOR_GREEN);
  }
  else if(m_drawState == 1)
  {
    m_gfx->drawAsciiArt(0, 0, gImageJimmy, sizeof(gImageJimmy), COLOR_GREEN);
  }
  else if(m_drawState == 2)
  {
    m_gfx->drawAsciiArt(0, 0, gImageGrant, sizeof(gImageGrant), COLOR_GREEN);
  }

  m_gfx->fillRect(0, m_gfx->height()-20, m_gfx->width()-1, 20, COLOR_BLACK);
  m_gfx->cursor(adjusts[m_drawState], m_gfx->height()-15);
  m_gfx->textSize(2);
  m_gfx->textColor(COLOR_GREEN);
  m_gfx->puts(names[m_drawState]);

  m_tSlideshow.once(5000);

  UIScene::draw();
}
