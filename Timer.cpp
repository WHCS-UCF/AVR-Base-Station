#include "Timer.h"

Timer::Timer()
  :state_(TIMER_STOPPED), type_(TIMER_NONE), start_(0),
   length_(0), callback_(NULL)
{

}

void Timer::once(time_t length)
{
  state_    = TIMER_STOPPED;
  type_     = TIMER_ONESHOT;
  start_    = 0;
  length_   = length;
  callback_ = NULL;

  start();
}

void Timer::once(time_t length, timerCallback_t callback)
{
  state_    = TIMER_STOPPED;
  type_     = TIMER_ONESHOT;
  start_    = 0;
  length_   = length;
  callback_ = callback;

  start();
}

void Timer::periodic(time_t length)
{
  state_    = TIMER_STOPPED;
  type_     = TIMER_PERIODIC;
  start_    = 0;
  length_   = length;
  callback_ = NULL;

  start();
}

void Timer::periodic(time_t length, timerCallback_t callback)
{
  state_    = TIMER_STOPPED;
  type_     = TIMER_PERIODIC;
  start_    = 0;
  length_   = length;
  callback_ = callback;

  start();
}

void Timer::start()
{
  if(type_ == TIMER_NONE)
    return;

  state_ = TIMER_RUNNING;
  start_ = millis();
}

void Timer::restart()
{
  start();
}

void Timer::stop()
{
  if(type_ == TIMER_NONE)
    return;

  state_ = TIMER_STOPPED;
  start_ = 0;
}

void Timer::fire()
{
  if(type_ == TIMER_NONE)
    return;

  // dont call reset
  if(callback_)
    callback_();

  // one hit and it's done
  if(type_ == TIMER_ONESHOT)
  {
    start_ = 0;
    state_ = TIMER_FIRED;
  }
  else
  {
    start_ = millis();
  }
}

bool Timer::update()
{
  time_t now = millis(), elapsed;

  // it doesnt make sense to call this otherwise
  if(type_ == TIMER_NONE || state_ != TIMER_RUNNING)
    return false;

  // prevent unsigned overflow
  if(now >= start_)
    elapsed = now - start_;
  else
    return false; // lets hope this doesnt happen

  // timer has fired!
  if(elapsed >= length_)
  {
    //Serial.println(elapsed);
    fire();
    return true;
  }
  else
    return false;
}

uint8_t Timer::percentComplete()
{
  if(type_ == TIMER_NONE)
    return 0;

  if(state_ == TIMER_FIRED)
    return 100;
  if(state_ == TIMER_STOPPED)
    return 0;

  time_t now = millis(), elapsed;

  // prevent unsigned overflow
  if(now >= start_)
    elapsed = now - start_;
  else
    return 0; // lets hope this doesnt happen

  return min(100, elapsed*100/length_);
}

bool Timer::running()
{
  if(type_ == TIMER_NONE)
    return false;

  return state_ == TIMER_RUNNING;
}

bool Timer::fired()
{
  if(type_ == TIMER_NONE)
    return false;

  return state_ == TIMER_FIRED;
}
