#include "ControlModule.h"

#include <stdio.h>

ControlModule::ControlModule(BlueTooth * bt, UIControlMod * uiCtrl, const char * name, cm_id_t id, cm_role_t role)
  :m_name(name), m_id(id), m_role(role), m_acState(STATE_OFF), m_dcState(STATE_OFF),
  m_temperature(0), m_bt(bt), m_uiCtrl(uiCtrl)
{

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

int16_t ControlModule::getTemperature()
{
  return m_temperature;
}

cm_id_t ControlModule::getId()
{
  return m_id;
}

const char * ControlModule::getName()
{
  return m_name;
}

void ControlModule::setACState(uint8_t state, publisher_t who)
{
  m_acState = state;
  handlePublish(who);

}

void ControlModule::setDCState(uint8_t state, publisher_t who)
{
  m_dcState = state;
  handlePublish(who);
}

// pub sub

void ControlModule::handlePublish(publisher_t who)
{
  if(who == PUB_LCD)
  {
    // TODO: notify radio
    notifyBTUpdate();
  }
  else if(who == PUB_BT)
  {
    // TODO: notify radio
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
