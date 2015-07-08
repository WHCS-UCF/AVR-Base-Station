# Makefile for WHCS Base Station
PROGRAM=base-station
SOURCES=main.cpp \
	SoftSerial.cpp \
	compat.cpp \
	whcslcd.cpp \
	whcsgfx.cpp \
	touchscreen.cpp \
	TouchCalibrate.cpp \
	BlueTooth.cpp \
	RingBuffer.cpp \
	UIManager.cpp \
	UIScene.cpp \
	ControlModule.cpp \
	fun.cpp

LIBRARIES=lib/MEGA32A_UART_LIBRARY/MEGA32A_UART_LIBRARY/ \
	  lib/AVR-gfx-lib/ \
	  lib/MEGA32A_SPI_LIBRARY/MEGA32A_SPI_LIBRARY/ \
	  lib/WHCS_RF24/WHCS_RF24/ \
	  common/

INCLUDE=$(LIBRARIES)
LIB_INCLUDE=$(LIBRARIES)

# include any libraries
LDFLAGS=-lcommon -lUART -lgfx -lRF24 -lSPI

# chip and CPU speed information
MCU=atmega32
F_CPU=16000000L
AVRDUDE_MCU=m32

# define hardware baud rate
export USART_BAUD=9600

include atmega.mk
