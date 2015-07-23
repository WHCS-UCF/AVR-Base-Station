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
#include "UIMain.h"
#include "eeprom.h"
#include "ControlModule.h"
#include "util.h"

#include "img/grant.h"
#include "img/jimmy.h"
#include "img/ucf.h"

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
#define MAIN_LOOP_WARNING 300 // 300ms maximum main loop time until warning
// STDIO file handles for printf related functions
static FILE gLocalStdout;
static FILE gLocalLCDOut;


// Subsystems and associated resources
RingBuffer uartRxBuffer, uartTxBuffer;
RF24 rf24(NRF_CE_NUMBER, NRF_CS_NUMBER); // pins on PORTB ONLY
Radio radio(&rf24, 0x41);
Adafruit_TFTLCD tft(&gLocalLCDOut);
WHCSLCD lcd(&tft, 3);
WHCSGfx gfx(&tft);
TouchScreen touch(300, 20); // rxplate, pressure threshold
UIManager ui(&touch, &lcd);
BlueTooth bluetooth(&uartRxBuffer, &uartTxBuffer);

// Control Modules
// door 0, light 1, sensor 2
extern UIControlMod uiControl[];

ControlModule cmDoor(&bluetooth, &uiControl[0],
    "Door", 0, ROLE_DC_SWITCH);
ControlModule cmLight(&bluetooth, &uiControl[1],
    "Light", 1, ROLE_AC_SWITCH);
ControlModule cmTemp(&bluetooth, &uiControl[2],
    "Temperature", 2, ROLE_TEMPERATURE);
ControlModule cmOutlet(&bluetooth, &uiControl[3],
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
UIMain uiMain(&gfx);

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

static inline int FreeRAM() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

static inline int TotalRAM() {
  extern char *__data_start; 
  return (int)(RAMEND - (int)&__data_start + 1);
}

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

  printf_P(PSTR("\n[W]ireless [H]ome [C]ontrol [S]ystem Base Station\n"));
  printf_P(PSTR("Starting subsystems...\n"));

  // show that we're powered up
  PIN_MODE_OUTPUT(STATUS_LED);
  PIN_HIGH(STATUS_LED);

  // start up the components
  radio.begin();
  bluetooth.begin();
  bluetooth.setModules(controlModules, NUM_CONTROL_MODULES);
  ui.begin();

  /*coord_t x = 0, y = 0;
  bool first = true;

  lcd.screenOn();
  //setAddrWindow(int x1, int y1, int x2, int y2);
  //pushColors(uint16_t *data, uint8_t len, boolean first);

  //tft.setAddrWindow(0,0, gfx.width()-1, gfx.height()-1);
  for(uint16_t i = 0x0; i < 0xffff;)
  {
    y = x/gfx.width();

    tft.fillScreen(i);
    tft.setCursor(0, 200);
    tft.setTextSize(2);
    tft.setTextColor(~i);
    tft.printf("Color 0x%04x", i);

    
    //tft.pushColors(&i, 1, first);
    first = false;
    //gfx.pixel(x % gfx.width(), y, i);
    x++;
    i += ;
  }
  while(1)
  {
  }*/

  rf24.printDetails();

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
  printf_P(PSTR("Free RAM %d/%d bytes\n\n"), FreeRAM(), TotalRAM());
  //printf_P(PSTR("Stack watermark: %u\n"), StackCount());

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

    ///////////////////////////////////////
    // END MAIN LOOP TIMING
    ///////////////////////////////////////

    time_t delta = millis() - mainStart;

    if(loopTimeReset.update())
      maxLoopTime = 0;

    if(delta > maxLoopTime) {
      loopTimeReset.once(5000);

      if(delta > MAIN_LOOP_WARNING)
        printf_P(PSTR("WARNING LIMIT REACHED: "));

      printf_P(PSTR("Main loop max: %lums"), delta);

      if(maxLoopTime > 0)
        printf_P(PSTR(" (+%lums)\n"), delta-maxLoopTime);
      else
        printf_P(PSTR("\n"));

      //printf_P(PSTR("Stack watermark: %u\n"), StackCount());
      maxLoopTime = delta;
    }
  }

  // main must not return
  for(;;);
}
