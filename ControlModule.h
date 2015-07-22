#ifndef CONTROLMODULE_H
#define CONTROLMODULE_H

#include <stdint.h>

enum cm_role_t {
  ROLE_NONE = 0,
  ROLE_DC_SWITCH = 0x1,
  ROLE_AC_SWITCH = 0x2,
  ROLE_TEMPERATURE = 0x4,
};

typedef uint8_t cm_id_t;

enum cm_state_t
{
  STATE_ON,
  STATE_OFF
};

class ControlModule
{
  public:
    ControlModule(const char * name, cm_id_t id, cm_role_t role);
    void printDetails();

    uint8_t getACState();
    uint8_t getDCState();
    int16_t getTemperature();
    cm_role_t getRole();
    const char * getName();

    void setACState(uint8_t state);
    void setDCState(uint8_t state);
  private:
    const char * m_name;
    cm_id_t m_id;
    cm_role_t m_role;

    uint8_t m_acState;
    uint8_t m_dcState;
    int16_t m_temperature;
};

#endif // CONTROLMODULE_H
