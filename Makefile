# Makefile for WHCS Base Station
PROGRAM=base-station
SOURCES=main.cpp \
	compat.cpp \
	whcslcd.cpp \
	ADC.cpp \
	touchscreen.cpp \
	Timer.cpp \
	timing.cpp \
	fun.cpp

LIBRARIES=lib/MEGA32A_UART_LIBRARY/MEGA32A_UART_LIBRARY/ \
	  lib/AVR-gfx-lib/ \
	  lib/MEGA32A_SPI_LIBRARY/MEGA32A_SPI_LIBRARY/ \
	  lib/WHCS_RF24/WHCS_RF24/

INCLUDE=$(LIBRARIES)
LIB_INCLUDE=$(LIBRARIES)

# include any libraries
LDFLAGS=-lUART -lgfx

# chip and CPU speed information
MCU=atmega32
F_CPU=16000000L
AVRDUDE_MCU=m32

include atmega.mk
