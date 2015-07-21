#include "UIMain.h"

UIMain::UIMain(WHCSGfx * gfx)
  :UIScene(gfx)
{

}

void UIMain::touchEvent(TouchEvent * ev)
{
  //end();
  m_gfx->pixel(ev->point.x, ev->point.y, COLOR_BLACK);
}

void UIMain::onCreate()
{
  queueRedraw();
}

void UIMain::onDestroy()
{

}

void UIMain::tick()
{

}

void UIMain::draw()
{
  printf("Draw\n");
  m_gfx->clearScreen(COLOR_RED);

  UIScene::draw();
}
