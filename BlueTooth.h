#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "common.h"
#include <stdint.h>

#include "RingBuffer.h"
#include "ControlModule.h"

class ControlModule;

class BlueTooth
{
public:
  BlueTooth(RingBuffer * rxBuffer, RingBuffer * txBuffer);
  void setModules(ControlModule ** controls, uint8_t numCtrl);
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
  void receiveCommand();

  // Action
  void enterProgrammingMode();
  static inline void powerUp();
  static inline void powerDown();

  void sendNotifyUpdate();
  void respQueryIfBaseStation();
  void respSuccess(uint8_t cm);
  void respError(uint8_t cm);
  void respErrorResult(uint8_t cm, uint8_t result);
  void respSuccessResult(uint8_t cm, uint8_t result);
private:
  ControlModule * getControlById(uint8_t id);
  void buildPkt(uint8_t * buf, uint8_t op, uint8_t cm);
  void resetPacketState();

  RingBuffer * m_rxBuffer;
  RingBuffer * m_txBuffer;
  bool m_lastConnectStatus;
  uint8_t m_lastRefId;
  ControlModule ** m_ctrl;
  uint8_t m_numCtrl;

  //packet state
  uint8_t m_pktPtr;
  bool m_inPacket;
  uint8_t m_pktBuf[4];
};

#endif // BLUETOOTH_H
