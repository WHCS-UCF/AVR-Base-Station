#include "UIScene.h"

UIScene::UIScene(WHCSGfx * gfx)
  :m_gfx(gfx), m_dirty(true), m_done(false)
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
