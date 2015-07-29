#include "ControlModule.h"

#include <stdio.h>

ControlModule::ControlModule(BlueTooth * bt, UIControlMod * uiCtrl, Radio * radio,
    const char * name, cm_id_t id, cm_role_t role)
  :m_name(name), m_id(id), m_role(role), m_acState(STATE_OFF), m_dcState(STATE_OFF),
  m_temperature(0), m_bt(bt), m_uiCtrl(uiCtrl), m_radio(radio), m_alive(false)
{
  //m_tHeartbeat.once(CONTROL_MOD_TIMEOUT);
  m_tPing.periodic(CONTROL_MOD_PING);
}

void ControlModule::tick()
{
  if(m_tPing.update())
  {
    if(!m_radio->sendTo(m_id, RADIO_OP_PING, NULL, 0))
      printf_P(PSTR("[%s] PING lost\n"), m_name);
    else
      printf_P(PSTR("[%s] PING OKAY\n"), m_name);
  }

  if(m_alive)
  {
    if(m_tHeartbeat.update())
    {
      printf_P(PSTR("*** LOST CONTROL MODULE %s ***\n"), m_name);
      m_alive = false;
      m_uiCtrl->gotUpdate();
    }
  }
}

void ControlModule::printDetails()
{
  printf("Control module '%s' (id %hu), role ", m_name, m_id);

  if(m_role == ROLE_AC_SWITCH)
    printf("AC");
  else if(m_role == ROLE_DC_SWITCH)
    printf("DC");
  else if(m_role == ROLE_TEMPERATURE)
    printf("TEMP");
  else
    printf("NONE");

  printf("\n");
}

cm_role_t ControlModule::getRole()
{
  return m_role;
}

uint8_t ControlModule::getACState()
{
  return m_acState;
}

uint8_t ControlModule::getDCState()
{
  return m_dcState;
}

int8_t ControlModule::getTemperature()
{
  return m_temperature;
}

uint8_t ControlModule::getState()
{
  if(m_role == ROLE_AC_SWITCH)
    return getACState();
  else if(m_role == ROLE_DC_SWITCH)
    return getDCState();
  else if(m_role == ROLE_TEMPERATURE)
    return getTemperature();
  else
    return 0;
}

cm_id_t ControlModule::getId()
{
  return m_id;
}

const char * ControlModule::getName()
{
  return m_name;
}

bool ControlModule::isAlive()
{
  return m_alive;
}

void ControlModule::handlePacket(radio_pkt * pkt)
{
  if(!pkt)
    return;

  if(pkt->from != m_id)
    return;

  switch(pkt->opcode)
  {
  case RADIO_OP_PONG:
    printf("%s::handle() PONG\n", m_name);
    m_tHeartbeat.once(CONTROL_MOD_TIMEOUT);

    if(!m_alive)
    {
      printf_P(PSTR("*** CONTROL MOD %s ALIVE ***\n"), m_name);
      m_alive = true;
      m_uiCtrl->gotUpdate();
    }
    break;
  case RADIO_OP_NEWDATA:
  case RADIO_OP_SUCCESS_RESULT:
    printf("%s::handle() success result/new data: state %d\n", m_name, pkt->data[0]);
    setState(pkt->data[0], PUB_RADIO);
    break;
  default:
    printf("%s::handle() unk opcode %hu\n", m_name, pkt->opcode);
    break;
  }
}

bool ControlModule::setACState(uint8_t state, publisher_t who)
{
  if(who == PUB_RADIO)
  {
    m_acState = state;
    handlePublish(who);
    return true;
  }

  uint8_t status = state;
  if(!m_radio->sendTo(m_id, RADIO_OP_SET_STATUS, &status, sizeof(status)))
  {
    printf_P(PSTR("Module %s set status failed\n"), m_name);
    return false;
  }

  m_acState = state;
  handlePublish(who);

  return true;
}

bool ControlModule::setDCState(uint8_t state, publisher_t who)
{
  if(who == PUB_RADIO)
  {
    m_dcState = state;
    handlePublish(who);
    return true;
  }

  uint8_t status = state;
  if(!m_radio->sendTo(m_id, RADIO_OP_SET_STATUS, &status, sizeof(status)))
  {
    printf_P(PSTR("ControlModule %s set status failed\n"), m_name);
    return false;
  }

  m_dcState = state;
  handlePublish(who);

  return true;
}

bool ControlModule::setTemperature(uint8_t state, publisher_t who)
{
  if(who == PUB_RADIO)
  {
    m_temperature = state;
    handlePublish(who);
    return true;
  }

  return false;
}

bool ControlModule::setState(uint8_t state, publisher_t who)
{
  if(m_role == ROLE_AC_SWITCH)
    return setACState(state, who);
  else if(m_role == ROLE_DC_SWITCH)
    return setDCState(state, who);
  else if(m_role == ROLE_TEMPERATURE)
    return setTemperature(state, who);
  else
    return false;
}

// pub sub

void ControlModule::handlePublish(publisher_t who)
{
  if(who == PUB_LCD)
  {
    notifyBTUpdate();
  }
  else if(who == PUB_BT)
  {
    m_uiCtrl->gotUpdate();
  }
  else if(who == PUB_RADIO)
  {
    m_uiCtrl->gotUpdate();
    notifyBTUpdate();
  }
}

void ControlModule::notifyBTUpdate()
{
  m_bt->sendNotifyUpdate();
}
