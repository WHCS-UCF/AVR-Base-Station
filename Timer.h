#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "timing.h"

class Timer
{
public:
  enum timer_state { TIMER_STOPPED, TIMER_RUNNING,
                     TIMER_PAUSED,  TIMER_FIRED    };
  enum timer_type  { TIMER_NONE,    TIMER_ONESHOT, TIMER_PERIODIC };
  typedef void (*timerCallback_t)(void);

  Timer();

  void once(time_t length);
  void once(time_t length, timerCallback_t callback);
  void periodic(time_t length);
  void periodic(time_t length, timerCallback_t callback);

  void start();
  void restart();
  void stop();
  void fire(); // used to force the timer to fire
  uint8_t percentComplete(); // how close are we to being done

  // returns true if the timer fired
  bool update();
  bool running();
  bool fired();

  static void registerTimer(Timer * timer);
  static void unregisterTimer(Timer * timer);
  static int  updateTimers();
private:
  uint8_t state_;
  uint8_t type_;
  time_t  start_;
  time_t  length_;
  timerCallback_t callback_;
};

#endif
