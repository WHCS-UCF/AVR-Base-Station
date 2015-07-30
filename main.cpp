#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#ifdef abs
#undef abs
#endif
#include <stdlib.h>

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
#include "UIMain.h"
#include "eeprom.h"
#include "ControlModule.h"
#include "util.h"

///////////////////////////////////////
// Function prototypes
///////////////////////////////////////

int lcd_putchar(char c, FILE *stream);
void early_init();
int main();

///////////////////////////////////////
// Global variables
///////////////////////////////////////

// Misc
#define MAIN_LOOP_WARNING 400 // 300ms maximum main loop time until warning
// STDIO file handles for printf related functions
static FILE gLocalStdout;
static FILE gLocalLCDOut;


// Subsystems and associated resources
RingBuffer uartRxBuffer, uartTxBuffer;
RF24 rf24(NRF_CE_NUMBER, NRF_CS_NUMBER); // pins on PORTB ONLY
Radio radio(&rf24, 0xde);
Adafruit_TFTLCD tft(&gLocalLCDOut);
WHCSLCD lcd(&tft, 3);
WHCSGfx gfx(&tft);
TouchScreen touch(300, 20); // rxplate, pressure threshold
UIManager ui(&touch, &lcd);
BlueTooth bluetooth(&uartRxBuffer, &uartTxBuffer);

// Control Modules
// door 0, light 1, sensor 2
extern UIControlMod uiControl[];

ControlModule cmDoor(&bluetooth, &uiControl[0], &radio,
    "Door", 0, ROLE_DC_SWITCH);
ControlModule cmLight(&bluetooth, &uiControl[1], &radio,
    "Light", 1, ROLE_AC_SWITCH);
ControlModule cmTemp(&bluetooth, &uiControl[2], &radio,
    "Temperature", 2, ROLE_TEMPERATURE);
ControlModule cmOutlet(&bluetooth, &uiControl[3], &radio,
    "Outlet", 3, ROLE_AC_SWITCH);

UIControlMod uiControl[] = {
  UIControlMod(&gfx, &cmDoor),
  UIControlMod(&gfx, &cmLight),
  UIControlMod(&gfx, &cmTemp),
  UIControlMod(&gfx, &cmOutlet)
};

ControlModule * controlModules[] = {
  &cmDoor, &cmLight, &cmTemp, &cmOutlet
};
#define NUM_CONTROL_MODULES 4

// UI scenes
TouchCalibrate uiCalibrate(&gfx, &touch);
UIMain uiMain(&gfx, &ui);

///////////////////////////////////////
// Interrupt handlers
///////////////////////////////////////

// ISR to queue read bytes from the UART
ISR(USART_RXC_vect)
{
  uartRxBuffer.put(UDR);
}

EMPTY_INTERRUPT(USART_TXC_vect);

// ISR to pump the TX out buffer
ISR(USART_UDRE_vect)
{
  if(uartTxBuffer.available())
    UDR = uartTxBuffer.get();
  else
    disableTXInterrupts();
}

// read incoming radio packets
//ISR(INT0_vect)
//{
//  radio_pkt pkt;
//  if(rf24.available())
//  {
//    if(rf24.read(&pkt, MAX_PACKET_SIZE))
//      if(!radio.queuePacket(&pkt));
//          //printf("\n");
//  }
//}

// called instead of RESET (vect0) for a bad ISR
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

///////////////////////////////////////
// Function definitions
///////////////////////////////////////

int lcd_putchar(char c, FILE *stream) {
  tft.write(c);
  return 1;
}

void early_init()
{
  // initialize timing (millis) as the first call 
  timing_init();
  WHCSADC::init(); // warning: this sets the ADC port as all inputs

  initUart(); // spin up the hardware USART
  enableRXInterrupts(); // enable receive interrupts
  // this is causing an interrupt loop
  // you MUST have UDR filled
  // enableTXInterrupts(); // enable TX interrupts

  // start the software serial library and interrupt timer
  soft_serial_init();

  // set LIBC's primary stdout and stderr to soft serial
  fdev_setup_stream(&gLocalStdout, soft_serial_putc, NULL, _FDEV_SETUP_WRITE);
  stdout = &gLocalStdout;
  stderr = &gLocalStdout;

  // Hack to allow the Adafruit_GFX class to use printf
  fdev_setup_stream(&gLocalLCDOut, lcd_putchar, NULL, _FDEV_SETUP_WRITE);

  // STDIN disabled due to software serial not having RX on an interrupt pin
  //fdev_setup_stream(&mystdin, NULL, soft_serial_getc, _FDEV_SETUP_READ);
  //stdin = &mystdin;
}

int main()
{
  early_init(); // bring up basic subsystems and timing
  sei(); // start interrupts

  // seed the rand num gen
  srand(WHCSADC::read(1));

  printf_P(PSTR("\n[W]ireless [H]ome [C]ontrol [S]ystem Base Station\n"));
  printf_P(PSTR("Starting subsystems...\n"));

  // show that we're powered up
  PIN_MODE_OUTPUT(STATUS_LED);
  PIN_HIGH(STATUS_LED);

  // start up the components
  radio.begin();
  //radio.enableInterrupt();
  bluetooth.begin();
  bluetooth.setModules(controlModules, NUM_CONTROL_MODULES);
  ui.begin();

  uiMain.setModules(uiControl, NUM_CONTROL_MODULES);

  bool forceCalibration = false;
  if(!EEPROM::hasCalibration() || forceCalibration)
  {
    ui.pushUI(&uiMain);
    ui.pushUI(&uiCalibrate);
  }
  else
  {
    int16_t xMin, xMax, yMin, yMax;
    EEPROM::loadCalibration(&xMin, &xMax, &yMin, &yMax);
    uiCalibrate.setCalibration(xMin, xMax, yMin, yMax);

    ui.pushUI(&uiMain);
  }

  printf_P(PSTR("WHCS main loop starting\n"));
#ifdef ENABLE_MEMORY_DEBUGGING
  printf_P(PSTR("Free RAM %d/%d bytes\n"), FreeRAM(), TotalRAM());
  printf_P(PSTR("Stack watermark: %u\n\n"), StackCount());
#endif

  uint8_t pktBuf[10];
  uint8_t ptr = 0;

  uint8_t pktOutBuf[10];
  uint8_t pktOutSize = 0;
  bool inPacket = false;

  // keep track of the worst case loop performance
  time_t maxLoopTime = 0;
  Timer loopTimeReset;
  loopTimeReset.once(5000);

  ///////////////////////////////////////
  // BEGIN MAIN EVENT LOOP
  ///////////////////////////////////////

  while(1)
  {
    unsigned long mainStart = millis();

    ///////////////////////////////////////
    // BEGIN MAIN LOOP TIMING
    ///////////////////////////////////////

    // Tick the UI "thread"
    // This handles animations, UI scene lifecycle and rendering
    ui.tick();
    bluetooth.tick();
    bluetooth.receiveCommand();

    radio.readPackets();

    while(radio.available())
    {
      radio_pkt pkt;
      radio.recv(&pkt);

      // figure out who this is for and dispatch it
      if(pkt.from >= 0 && pkt.from < NUM_CONTROL_MODULES)
        controlModules[pkt.from]->handlePacket(&pkt);
      else
        printf_P(PSTR("Ignoring packet to CM %d\n"), pkt.from);
    }

    // tick all the control modules
    for(int i = 0; i < NUM_CONTROL_MODULES; i++)
      controlModules[i]->tick();

    ///////////////////////////////////////
    // END MAIN LOOP TIMING
    ///////////////////////////////////////

    time_t delta = millis() - mainStart;

    if(loopTimeReset.update())
    {
      maxLoopTime = 0;
      rf24.startListening();
    }

    if(delta > maxLoopTime) {
      loopTimeReset.once(5000);

      if(delta > MAIN_LOOP_WARNING)
        printf_P(PSTR("WARNING LIMIT REACHED: "));

      printf_P(PSTR("Main loop max: %lums"), delta);

      if(maxLoopTime > 0)
        printf_P(PSTR(" (+%lums)\n"), delta-maxLoopTime);
      else
        printf_P(PSTR("\n"));

#ifdef ENABLE_MEMORY_DEBUGGING
  printf_P(PSTR("Free RAM %d/%d bytes\n"), FreeRAM(), TotalRAM());
  printf_P(PSTR("Stack watermark: %u\n"), StackCount());
#endif

      //printf_P(PSTR("Stack watermark: %u\n"), StackCount());
      maxLoopTime = delta;
    }
  }

  // main must not return
  for(;;);
}
