#include "BlueTooth.h"

/*struct pkt_bs_ident {
  uint8_t id[4];
};

struct bt_pkt
{
  uint8_t refId;
  uint8_t opcode;
  uint8_t length;
  union {
    struct pkt_bs_ident;
    uint8_t data[0];
  } data;
};

#define MAX_BLUETOOTH_MSG_SIZE sizeof(struct bt_pkt)
*/

BlueTooth::BlueTooth(RingBuffer * rxBuffer, RingBuffer * txBuffer)
  :m_rxBuffer(rxBuffer), m_txBuffer(txBuffer)
{
}

size_t BlueTooth::available()
{
  return m_rxBuffer->available();
}

size_t BlueTooth::read(uint8_t * buf, size_t amount)
{
  size_t i;

  for(i = 0; i < amount; i++) {
    if(!m_rxBuffer->available())
      break;

    buf[i] = m_rxBuffer->get();
  }

  return i;
}

size_t BlueTooth::putch(uint8_t c)
{
  while(m_txBuffer->full()); // wait for some space
  m_txBuffer->put(c);

  return 1;
}

size_t BlueTooth::write(uint8_t * buf, size_t amount)
{
  size_t i;

  for(i = 0; i < amount; i++) {
    while(m_txBuffer->full()); // wait for some space
    m_txBuffer->put(buf[i]);
  }

  return amount;
}
