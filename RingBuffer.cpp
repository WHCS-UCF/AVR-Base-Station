#include "RingBuffer.h"

#include <util/atomic.h>

RingBuffer::RingBuffer()
  :m_head(0), m_tail(0), m_available(0)
{

}

bool RingBuffer::available()
{
  return m_available > 0;
}

bool RingBuffer::full()
{
  return m_available == sizeof(m_buf);
}

bool RingBuffer::empty()
{
  return m_available == 0;
}

uint8_t RingBuffer::get()
{
  uint8_t d;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if(m_available == 0)
      return 0;

    d = m_buf[m_head];
    m_head = (m_head + 1) % sizeof(m_buf);
    m_available--;
  }

  return d;
}

void RingBuffer::put(uint8_t data)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    m_buf[m_tail] = data;
    m_tail = (m_tail + 1) % sizeof(m_buf);
    m_available = min(m_available + 1, sizeof(m_buf));
  }
}
