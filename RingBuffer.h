#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "common.h"
#include <stdint.h>

class RingBuffer
{
public:
  RingBuffer();

  bool available();
  bool empty();
  bool full();
  void clear();

  uint8_t get();
  void put(uint8_t data);
private:
  uint8_t m_buf[32];
  uint8_t m_head, m_tail;
  uint8_t m_available;
};

#endif // RINGBUFFER_H
