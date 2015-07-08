#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include <MEGA32A_UART_LIBRARY.h>
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <RF24.h>

#include "fun.h"
#include "timing.h"
#include "ADC.h"
#include "touchscreen.h"
#include "Radio.h"
#include "whcslcd.h"
#include "whcsgfx.h"
#include "pinout.h"
#include "TouchCalibrate.h"
#include "SoftSerial.h"
#include "BlueTooth.h"
#include "UIManager.h"
#include "ControlModule.h"

#include "img/grant.h"
#include "img/jimmy.h"
#include "img/ucf.h"

RingBuffer uartRxBuffer, uartTxBuffer;

/*int uart_putchar(char c, FILE *stream) {
  if (c == '\n')
    uart_putchar('\r', stream);

  //USART_SendByte(c);
  if(UCSRA & _BV(UDRE))
    UDR = c;
  else {
    while(uartTxBuffer.full());
    uartTxBuffer.put(c);
  }

  return 1;
}*/

//*int uart_getchar(FILE *stream) {
  //loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
  //return UDR0;
  //return 0;
  //return USART_ReceiveByte();
//}

// Control Modules
// door 0, light 1, sensor 2
ControlModule cmDoor("Door", 0, ROLE_DC_SWITCH);
ControlModule cmLight("Light", 1, ROLE_AC_SWITCH);
ControlModule cmTemp("Temperature", 2, ROLE_TEMPERATURE);
ControlModule * controlModules[] = {&cmDoor, &cmLight, &cmTemp, NULL};
#define NUM_CONTROL_MODULES 3

static FILE mylcdout;
RF24 rf24(NRF_CE_NUMBER, NRF_CS_NUMBER); // pins on PORTB ONLY
Radio radio(&rf24, 0x41);
Adafruit_TFTLCD tft(&mylcdout);
WHCSLCD lcd(&tft, 3);
WHCSGfx gfx(&tft);
TouchScreen touch(300, 20); // rxplate, pressure threshold
UIManager ui(&touch, &lcd);

int lcd_putchar(char c, FILE *stream) {
  tft.write(c);
  return 1;
}

#define MAIN_LOOP_WARNING 300 // 300ms maximum main loop time until warning
static FILE mystdout;
static FILE mystdin;

BlueTooth bluetooth(&uartRxBuffer, &uartTxBuffer);

ISR(USART_RXC_vect)
{
  uartRxBuffer.put(UDR);
}

EMPTY_INTERRUPT(USART_TXC_vect);
ISR(USART_UDRE_vect)
{
  if(uartTxBuffer.available())
    UDR = uartTxBuffer.get();
  else
    disableTXInterrupts();
}

ISR(BADISR_vect)
{
  PIN_MODE_OUTPUT(STATUS_LED);
  while(1) {
    PIN_HIGH(STATUS_LED);
    _delay_ms(500);
    PIN_LOW(STATUS_LED);
    _delay_ms(500);
  }
}

int main()
{
  // initialize timing (millis) as the first call 
  timing_init();
  WHCSADC::init(); // this sets the ADC port as all inputs
  initUart();
  enableRXInterrupts();
  // this is causing an interrupt loop
  // you MUST have UDR filled
  enableTXInterrupts();

  soft_serial_init();
  // Enable interrupts here for the software serial
  sei();

  // setup STDIN/STDOUT for printf
  fdev_setup_stream(&mystdout, soft_serial_putc, NULL, _FDEV_SETUP_WRITE);
  fdev_setup_stream(&mylcdout, lcd_putchar, NULL, _FDEV_SETUP_WRITE);
  //fdev_setup_stream(&mystdin, NULL, uart_getchar, _FDEV_SETUP_READ);
  stdout = &mystdout;
  //stdin = &mystdin;

  printf_P(PSTR("[W]ireless [H]ome [C]ontrol [S]ystem Base Station\n"));

  // show that we're powered up
  PIN_MODE_OUTPUT(STATUS_LED);
  PIN_HIGH(STATUS_LED);

  // enable the HC-05
  PIN_MODE_OUTPUT(HC05_ENABLE);
  PIN_HIGH(HC05_ENABLE);
  PIN_MODE_INPUT(HC05_STATUS);

  // start up the components
  radio.begin();
  bluetooth.begin();
  ui.begin();

  for(int i = 0; i < NUM_CONTROL_MODULES; i++) {
    controlModules[i]->printDetails();
  }

  // start with calibration
  TouchCalibrate uiCalibrate(&tft, &touch);
  ui.setTopLevelUI(&uiCalibrate);

  printf_P(PSTR("WHCS main loop starting\n"));

  time_t maxLoopTime = 0;

  // Main event loop
  int down = 0;
  TSPoint pLast;

  bool done = false;
  bool firstLoop = true;
  bool btConnected = false;

  uint8_t pktBuf[10];
  uint8_t ptr = 0;

  uint8_t pktOutBuf[10];
  uint8_t pktOutSize = 0;
  bool inPacket = false;

  while(1)
  {
    unsigned long mainStart = millis();

    ///////////////////////////////////////

    if(bluetooth.isConnected()) {
      if(!btConnected || firstLoop) {
        printf("BlueTooth client connected!\n");
        btConnected = true;
      }
    }
    else {
      if(btConnected || firstLoop) {
        printf("BlueTooth client disconnected!\n");
        btConnected = false;
      }
    }

    // pump the BlueTooth TX buffer
    if(uartTxBuffer.available()) {
      if(UCSRA & _BV(UDRE)) {
        int c = uartTxBuffer.get();
        UDR = c;
        enableTXInterrupts();
      }
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

    uint8_t buf[10];

    if(bluetooth.available()) {
      size_t amt = bluetooth.read(buf, 3);

      printf("Recv %u byte(s): ", amt);
      for(size_t i = 0; i < amt; i++) {
        uint8_t b = buf[i];
        printf("%x ", b);

        if(buf[i] == '\e') {
          printf("Entering packet\n");
          inPacket = true;
          ptr = 0;
        }
        else
        {
          if(inPacket) {
            radio_pkt radioPkt;
            pktBuf[ptr] = b;

            if(ptr == 2) {
              uint8_t refId = pktBuf[0];
              uint8_t opcode = pktBuf[1];
              uint8_t cmTarget = pktBuf[2];

              printf("\nREF[%d] OP[%d] CM[%d]\n",
                  refId, opcode, cmTarget);

              if(cmTarget > 2) {
                printf("Bad control module %d\n", cmTarget);
              }

              ControlModule * cm = controlModules[cmTarget];
              pktOutBuf[0] = '\e';

              switch(opcode)
              {
                case QUERY_IF_BASE_STATION:
                  printf("Query From BT\n");

                  pktOutBuf[1] = refId;
                  pktOutBuf[2] = QUERY_IF_BASE_STATION;
                  pktOutBuf[3] = 4;
                  pktOutSize = 4;
                  break;
                case GET_MODULE_STATUS:
                  printf("GET_STATUS id %d\n", cmTarget);

                  cm->printDetails();
                  pktOutBuf[1] = refId;
                  pktOutBuf[2] = SUCCESS_WITH_RESULT;
                  pktOutBuf[3] = cmTarget;

                  if(cm->getRole() == ROLE_AC_SWITCH)
                    pktOutBuf[4] = cm->getACState();
                  else if(cm->getRole() == ROLE_DC_SWITCH)
                    pktOutBuf[4] = cm->getDCState();
                  else if(cm->getRole() == ROLE_TEMPERATURE)
                    pktOutBuf[4] = cm->getTemperature();

                  pktOutSize = 5;
                  break;
                case TURN_ON_MODULE:
                  printf("ON id %d\n", cmTarget);
                  pktOutBuf[1] = refId;
                  pktOutBuf[2] = SUCCESS_NO_RESULT;
                  pktOutBuf[3] = cmTarget;
                  pktOutSize = 4;

                  radioPkt.data[1] = 'O';
                  radioPkt.size = 1;

                  if(radio.sendTo(cmTarget, &radioPkt)) {
                    printf("Send ON command to CM\n");
                    if(cm->getRole() == ROLE_AC_SWITCH)
                      cm->setACState(STATE_ON);
                    else if(cm->getRole() == ROLE_DC_SWITCH)
                      cm->setDCState(STATE_ON);
                  }
                  else {
                    printf("ON command failed\n");
                  }

                  for(int i = 0; i < radioPkt.size+1; i++)
                    printf("%02x ", radioPkt.data[i]);
                  break;
                case TURN_OFF_MODULE:
                  printf("OFF id %d\n", cmTarget);
                  pktOutBuf[1] = refId;
                  pktOutBuf[2] = SUCCESS_NO_RESULT;
                  pktOutBuf[3] = cmTarget;
                  pktOutSize = 4;

                  radioPkt.data[1] = 'A';
                  radioPkt.size = 1;

                  if(radio.sendTo(cmTarget, &radioPkt)) {
                    printf("Send OFF command to CM\n");

                    if(cm->getRole() == ROLE_AC_SWITCH)
                      cm->setACState(STATE_OFF);
                    else if(cm->getRole() == ROLE_DC_SWITCH)
                      cm->setDCState(STATE_OFF);
                  }
                  else {
                    printf("OFF command failed\n");
                  }
                  break;
                default:
                  printf("Unrecognized opcode %02x\n", opcode);
                  pktOutSize = 0;
              }

              if(pktOutSize > 0)
                bluetooth.write(pktOutBuf, pktOutSize);

              inPacket = false;
            }

            ptr++;
          }
          else
          {
            printf("IGN %02x\n", b);
          }
        }
      }

      //bluetooth.write(buf, amt);
      printf("\n");
    }



    ui.tick();

    ///////////////////////////////////////

    time_t delta = millis() - mainStart;

    if(delta > maxLoopTime) {
      if(delta > MAIN_LOOP_WARNING)
        printf_P(PSTR("WARNING LIMIT: "));

      printf_P(PSTR("Main loop max: %lums\n"), delta);

      maxLoopTime = delta;
    }

    firstLoop = false;
  }

  // main must not return
  for(;;);
}
