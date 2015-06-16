# Makefile for WHCS Base Station
PROGRAM=base-station
SOURCES=main.cpp

LIBRARIES=lib/MEGA32A_UART_LIBRARY/MEGA32A_UART_LIBRARY/ \
	  lib/AVR-gfx-lib/
INCLUDE=$(LIBRARIES)
LIB_INCLUDE=$(LIBRARIES)

# include any libraries
LDFLAGS=-lUART -lgfx

# chip and CPU speed information
MCU=atmega32
F_CPU=16000000L
AVRDUDE_MCU=m32

include atmega.mk
