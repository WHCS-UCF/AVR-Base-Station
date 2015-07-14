#include "UIScene.h"

UIScene::UIScene(Adafruit_TFTLCD * tft)
  :m_tft(tft), m_dirty(true), m_done(false)
{

}

void UIScene::touchEvent(TouchEvent * ev)
{

}

void UIScene::tick()
{

}

void UIScene::draw()
{
  m_dirty = false;
}

void UIScene::end()
{
  m_done = true;
}

bool UIScene::queueRedraw()
{
  m_dirty = true;
}

bool UIScene::isDirty()
{
  return m_dirty;
}

bool UIScene::isDone()
{
  return m_done;
}
