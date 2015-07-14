#include "BlueTooth.h"

#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <MEGA32A_UART_LIBRARY.h>

#include "pinout.h"

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
  :m_rxBuffer(rxBuffer), m_txBuffer(txBuffer), m_lastConnectStatus(false)
{
}

void BlueTooth::begin()
{
  PIN_MODE_INPUT(HC05_STATUS);
  PIN_MODE_OUTPUT(HC05_ENABLE);
  PIN_MODE_OUTPUT(HC05_KEY);

  // no programming needed
  PIN_LOW(HC05_KEY);

  // enable the HC-05
  powerUp();

  bool connected = isConnected();

  printf_P(PSTR("BlueTooth: "));

  if(!connected)
    printf_P(PSTR("no "));

  printf_P(PSTR("client connected\n"));

  m_lastConnectStatus = connected;
}

void BlueTooth::tick()
{
  bool connected = isConnected();

  if(connected && !m_lastConnectStatus) {
    printf_P(PSTR("BlueTooth: "));
    printf_P(PSTR("client connected\n"));
  }
  else if(!connected && m_lastConnectStatus) {
    printf_P(PSTR("BlueTooth: "));
    printf_P(PSTR("client disconnected\n"));
  }

  m_lastConnectStatus = connected;

  // pump the BlueTooth TX buffer
  if(m_txBuffer->available()) {
    // is the queue empty?
    // if so, TX interrupts must be disabled
    // start the queue off with data
    // and enable the TX interrupt to stream it
    if(UCSRA & _BV(UDRE)) {
      int c = m_txBuffer->get();
      UDR = c;
      enableTXInterrupts();
    }
  }
}

bool BlueTooth::isConnected()
{
  return PIN_READ(HC05_STATUS);
}

size_t BlueTooth::available()
{
  return m_rxBuffer->available();
}

size_t BlueTooth::read(uint8_t * buf, size_t amount)
{
  size_t i = 0;

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

size_t BlueTooth::write(const char * string)
{
  return write((uint8_t *)string, strlen(string));
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

// currently does not work
void BlueTooth::enterProgrammingMode()
{
  powerDown();
  PIN_HIGH(HC05_KEY);
  printf("Powering off - key HIGH\n");
  _delay_ms(2000);

  powerUp();
  printf("Powering on\n");
  _delay_ms(3000);

  printf("key LOW\n");
  PIN_LOW(HC05_KEY);

  write("AT\n\r");

  // enter in to AT mode
  while(1) {
    if(available()) {
      printf("Available\n");

      uint8_t buf[10];
      size_t amt = read(buf, 10);

      for(size_t i = 0; i < amt; i++)
        printf("%c", buf[i]);
    }
  }
}

void BlueTooth::powerUp()
{
  PIN_HIGH(HC05_ENABLE);
}

void BlueTooth::powerDown()
{
  PIN_LOW(HC05_ENABLE);
}
