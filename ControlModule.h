#ifndef CONTROLMODULE_H
#define CONTROLMODULE_H

#include <stdint.h>
#include "BlueTooth.h"
#include "UIControlMod.h"
#include "Radio.h"
#include "Timer.h"

#define CONTROL_MOD_TIMEOUT 15000
#define CONTROL_MOD_PING 4000

enum cm_role_t {
  ROLE_NONE = 0,
  ROLE_DC_SWITCH = 0x1,
  ROLE_AC_SWITCH = 0x2,
  ROLE_TEMPERATURE = 0x4,
};

enum publisher_t
{
  PUB_RADIO,
  PUB_LCD,
  PUB_BT
};

typedef uint8_t cm_id_t;

enum cm_state_t
{
  STATE_OFF,
  STATE_ON
};

class BlueTooth;
class UIControlMod;

class ControlModule
{
  public:
    ControlModule(BlueTooth * bt, UIControlMod * uiCtrl, Radio * radio,
        const char * name, cm_id_t id, cm_role_t role);
    void tick();
    void printDetails();

    uint8_t getACState();
    uint8_t getDCState();
    int8_t getTemperature();
    uint8_t getState();
    bool isAlive();

    cm_role_t getRole();
    cm_id_t getId();
    const char * getName();

    void handlePacket(radio_pkt * pkt);
    bool setACState(uint8_t state, publisher_t who);
    bool setDCState(uint8_t state, publisher_t who);
    bool setTemperature(uint8_t state, publisher_t who);
    bool setState(uint8_t state, publisher_t who);
  private:
    void handlePublish(publisher_t who);
    void notifyBTUpdate();

    // subscribers and publishers
    BlueTooth * m_bt;
    UIControlMod * m_uiCtrl;
    Radio * m_radio;

    const char * m_name;
    cm_id_t m_id;
    cm_role_t m_role;

    uint8_t m_acState;
    uint8_t m_dcState;
    int8_t m_temperature;

    // state tracking
    bool m_alive;
    Timer m_tHeartbeat;
    Timer m_tPing;
};

#endif // CONTROLMODULE_H
