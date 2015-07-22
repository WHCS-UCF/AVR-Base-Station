# Makefile for WHCS Base Station
PROGRAM=base-station
SOURCES=main.cpp \
	SoftSerial.cpp \
	compat.cpp \
	fun.cpp \
	whcslcd.cpp \
	whcsgfx.cpp \
	touchscreen.cpp \
	TouchCalibrate.cpp \
	BlueTooth.cpp \
	RingBuffer.cpp \
	ControlModule.cpp \
	UIManager.cpp \
	UIScene.cpp \
	UIView.cpp \
	UIButton.cpp \
	UIMain.cpp \
	UIControlMod.cpp \
	eeprom.cpp

LIBRARIES=lib/MEGA32A_UART_LIBRARY/MEGA32A_UART_LIBRARY/libUART.a \
	  lib/MEGA32A_SPI_LIBRARY/MEGA32A_SPI_LIBRARY/libSPI.a \
	  lib/AVR-gfx-lib/libgfx.a \
	  lib/WHCS_RF24/WHCS_RF24/libRF24.a \
	  common/libcommon.a
CFLAGS=-Wall -Wextra -Werror

# chip and CPU speed information
MCU=atmega32
F_CPU=16000000L
AVRDUDE_MCU=m32

# define hardware baud rate
#export USART_BAUD=38400
export USART_BAUD=9600

include atmega.mk
