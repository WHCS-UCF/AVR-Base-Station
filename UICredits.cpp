#include "UICredits.h"

#include <avr/pgmspace.h>

#include "img/grant.h"
#include "img/jimmy.h"
#include "img/ucf.h"

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
    m_drawState =2;
    queueRedraw();
  }
}

void UICredits::draw()
{
  m_gfx->clearScreen(COLOR_BLACK);

    m_drawState =2;
  if(m_drawState == 0)
    m_gfx->drawAsciiArt(0, 0, gImageGrant, sizeof(gImageGrant), COLOR_RED);
  else if(m_drawState == 1)
    m_gfx->drawAsciiArt(0, 0, gImageJimmy, sizeof(gImageJimmy), COLOR_GREEN);
  else if(m_drawState == 2)
    m_gfx->drawAsciiArt(20, 0, gImageUCF, sizeof(gImageUCF), COLOR_BLUE);

  m_tSlideshow.once(5000);

  UIScene::draw();
}
