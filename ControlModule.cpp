#include "ControlModule.h"

#include <stdio.h>

ControlModule::ControlModule(const char * name, cm_id_t id, cm_role_t role)
  :m_name(name), m_id(id), m_role(role), m_acState(STATE_OFF), m_dcState(STATE_OFF),
  m_temperature(0)
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

const char * ControlModule::getName()
{
  return m_name;
}

void ControlModule::setACState(uint8_t state)
{
  m_acState = state;
}

void ControlModule::setDCState(uint8_t state)
{
  m_dcState = state;
}
