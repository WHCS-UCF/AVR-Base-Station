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
  void tick();

  // Status
  bool isConnected();

  // Data API
  size_t available();
  size_t read(uint8_t * buf, size_t amount);
  size_t putch(uint8_t c);
  size_t write(uint8_t * buf, size_t amount);
  // expects a NULL terminated string
  size_t write(const char * string);

  // Action
  void enterProgrammingMode();
  static inline void powerUp();
  static inline void powerDown();
private:
  RingBuffer * m_rxBuffer;
  RingBuffer * m_txBuffer;
  bool m_lastConnectStatus;
};

#endif // BLUETOOTH_H
