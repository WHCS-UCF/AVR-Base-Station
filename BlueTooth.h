#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "common.h"
#include <stdint.h>

#include "RingBuffer.h"

class BlueTooth
{
public:
  BlueTooth(RingBuffer * rxBuffer, RingBuffer * txBuffer);
  void begin();

  bool isConnected();
  size_t available();
  size_t read(uint8_t * buf, size_t amount);
  size_t putch(uint8_t c);
  size_t write(uint8_t * buf, size_t amount);
private:
  RingBuffer * m_rxBuffer;
  RingBuffer * m_txBuffer;
};

#endif // BLUETOOTH_H
