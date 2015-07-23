#include "BlueTooth.h"

#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <MEGA32A_UART_LIBRARY.h>

#include "pinout.h"

#undef DEBUG_BT

BlueTooth::BlueTooth(RingBuffer * rxBuffer, RingBuffer * txBuffer)
  :m_rxBuffer(rxBuffer), m_txBuffer(txBuffer), m_lastConnectStatus(false)
{
  m_lastRefId = 0;
  resetPacketState();
  setModules(NULL, 0);
}

void BlueTooth::setModules(ControlModule ** controls, uint8_t numCtrl)
{
  m_ctrl = controls;
  m_numCtrl = numCtrl;
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

    resetPacketState();
    m_txBuffer->clear();
  }
  else if(!connected && m_lastConnectStatus) {
    printf_P(PSTR("BlueTooth: "));
    printf_P(PSTR("client disconnected\n"));
    m_txBuffer->clear();
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

  if(!m_lastConnectStatus)
    return 0;

  for(i = 0; i < amount; i++) {
    putch(buf[i]);
  }

  return amount;
}

// SUCCESS_WITH_RESULT
// ERROR_NO_RESULT
//
// GET_MODULE_STATUS - refid, opcode, cm
//   - response refid, SUCCSS/ERR, cm, RESULT
//   - 4 bytes with 1 byte status
//////////////////////
// SUCCESS_NO_RESULT
// ERROR_NO_RESULT
//
// TURN_ON_MODULE
// TURN_OFF_MODULE
//   - 3 bytes with opcode being status
//   - resp no result
//   public static final byte GET_STATUS_OF_BASE_STATION = 0x00;
#define GET_MODULE_STATUS 0X01
#define TURN_ON_MODULE 0x02
#define TURN_OFF_MODULE 0x03
#define TOGGLE_MODULE 0x04
#define GET_CONTROL_MODULE_TYPE 0x05
#define GET_DATA_COLLECTOR_DATA 0x06
#define GET_NUMBER_OF_MODULES 0x07
#define GET_CONTROL_MODULE_UID 0x08
#define SET_UPDATE_INTERVAL 0x09
#define QUERY_IF_BASE_STATION 0x0A
#define SUCCESS_NO_RESULT 0x50
#define SUCCESS_WITH_RESULT 0x51
#define ERROR_NO_RESULT 0x52
#define ERROR_WITH_RESULT 0x53
#define CONTROL_MODULES_CHANGED 0x70

void BlueTooth::receiveCommand()
{
  uint8_t buf[10];

  if(!m_lastConnectStatus || !available())
    return;

  size_t amt = read(buf, 10);

#ifdef DEBUG_BT
  printf("BT: got %u byte(s): ", amt);
#endif

  for(size_t i = 0; i < amt; i++)
  {
    uint8_t b = buf[i];
#ifdef DEBUG_BT
    printf("%x ", b);
#endif

    if(!m_inPacket) {
      if(buf[i] == '\e') {
#ifdef DEBUG_BT
        printf("Entering packet\n");
#endif
        m_inPacket = true;
        m_pktPtr = 0;
      }
      else
      {
#ifdef DEBUG_BT
        printf("[stray %02x]", b);
#endif
      }
      continue;
    }

    m_pktBuf[m_pktPtr] = b;

    if(m_pktPtr < 2) {
      m_pktPtr++;
      continue;
    }

    resetPacketState();

    if(m_pktPtr >= sizeof(m_pktBuf))
    {
      printf("Packet BOF\n");
      continue;
    }

    uint8_t refId = m_pktBuf[0];
    uint8_t opcode = m_pktBuf[1];
    uint8_t cmTarget = m_pktBuf[2];

#ifdef DEBUG_BT
    printf("\nREF[%d] OP[%d] CM[%d]\n",
        refId, opcode, cmTarget);
#endif

    m_lastRefId = refId;

    if(opcode == QUERY_IF_BASE_STATION)
    {
      printf("Query From BT\n");
      respQueryIfBaseStation();

      /*case QUERY_IF_BASE_STATION:
        printf("Query From BT\n");
        sendRespQueryIfBase()

        pktOutBuf[1] = refId;
        pktOutBuf[2] = QUERY_IF_BASE_STATION;
        pktOutBuf[3] = 4;
        pktOutSize = 4;
        break;*/

      continue;
    }

    ControlModule * cm = getControlById(cmTarget);

    if(!cm) {
      printf("Bad control module %d\n", cmTarget);
      continue;
    }

    switch(opcode)
    {
      case GET_MODULE_STATUS:
      {
        printf("GET_STATUS id %d\n", cmTarget);

        cm->printDetails();

        uint8_t status = 0;
        if(cm->getRole() == ROLE_AC_SWITCH)
          status = cm->getACState();
        else if(cm->getRole() == ROLE_DC_SWITCH)
          status = cm->getDCState();
        else if(cm->getRole() == ROLE_TEMPERATURE)
          status = cm->getTemperature();

        respSuccessResult(cmTarget, status);
        break;
      }
      case TURN_ON_MODULE:
        printf("ON id %d\n", cmTarget);

        if(cm->getRole() == ROLE_AC_SWITCH)
          cm->setACState(STATE_ON, PUB_BT);
        else if(cm->getRole() == ROLE_DC_SWITCH)
          cm->setDCState(STATE_ON, PUB_BT);

        respSuccess(cmTarget);
        break;
      case TURN_OFF_MODULE:
        printf("OFF id %d\n", cmTarget);

        if(cm->getRole() == ROLE_AC_SWITCH)
          cm->setACState(STATE_OFF, PUB_BT);
        else if(cm->getRole() == ROLE_DC_SWITCH)
          cm->setDCState(STATE_OFF, PUB_BT);

        respSuccess(cmTarget);
        break;
      default:
        printf("UnkOP %02x\n", opcode);
    }
  }
}

ControlModule * BlueTooth::getControlById(uint8_t id)
{
  for(int i = 0; i < m_numCtrl; i++)
    if(m_ctrl[i]->getId() == id)
      return m_ctrl[i];

  return NULL;
}

// Messages
void BlueTooth::sendNotifyUpdate()
{
  uint8_t outBuf[4];

  //m_lastRefId++;
  buildPkt(outBuf, CONTROL_MODULES_CHANGED, 0);
  outBuf[1] = 0;
  write(outBuf, sizeof(outBuf));
}

void BlueTooth::respQueryIfBaseStation()
{
  uint8_t outBuf[4];
  buildPkt(outBuf, QUERY_IF_BASE_STATION, 0x4);
  write(outBuf, sizeof(outBuf));
}

void BlueTooth::respSuccess(uint8_t cm)
{
  uint8_t outBuf[4];
  buildPkt(outBuf, SUCCESS_NO_RESULT, cm);
  write(outBuf, sizeof(outBuf));
}

void BlueTooth::respSuccessResult(uint8_t cm, uint8_t result)
{
  uint8_t outBuf[5];
  buildPkt(outBuf, SUCCESS_WITH_RESULT, cm);
  outBuf[4] = result;
  write(outBuf, sizeof(outBuf));
}

void BlueTooth::buildPkt(uint8_t * buf, uint8_t op, uint8_t cm)
{
  buf[0] = '\e';
  buf[1] = m_lastRefId;
  buf[2] = op;
  buf[3] = cm;
}

void BlueTooth::resetPacketState()
{
  m_pktPtr = 0;
  m_inPacket = false;
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
