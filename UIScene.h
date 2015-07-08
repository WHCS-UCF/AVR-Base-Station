#ifndef UISCENE_H
#define UISCENE_H

#include "TouchEvent.h"
#include <Adafruit_TFTLCD.h>

class UIScene
{
  public:
    UIScene(Adafruit_TFTLCD * tft);

    virtual void touchEvent(TouchEvent * ev); // called on a touch event
    virtual void onCreate() = 0; // called when scene first created
    //virtual void onPause() = 0; // called when scene resuming
    //virtual void onResume() = 0; // called when scene resuming
    virtual void onDestroy() = 0; // called when scene ending
    virtual void tick();
    virtual void draw();

    void end(); // ends the scene
    bool isDirty();
    bool queueRedraw();
    bool done();
  protected:
    Adafruit_TFTLCD * m_tft;
  private:
    bool m_dirty;
    bool m_done;
};

#endif // UISCENE_H
