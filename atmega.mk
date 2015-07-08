####
#
# Erico Nunes <nunes dot erico * gmail> @ 2012
# Grant Hernandez - WHCS
#
# resources:
# http://www.instructables.com/files/orig/FK6/1AKF/ANLEUN334H7/FK61AKFANLEUN334H7.txt
# https://www.mainframe.cx/~ckuethe/avr-c-tutorial/
# http://paul.graysonfamily.org/thoughts/avrlinux/
#
####

####################
# Define the following on the application:
#
# PROGRAM := file.c
# MCU := atmega328p
# AVRDUDE_MCU := m328p
# AVRDUDE_DEV := /dev/ttyS0
# F_CPU=16000000UL
# # optionally define LIBNAME in order to build as library
#
# CFLAGS += -Wall -Wextra
# INCLUDE += ../lib
#
# include atmega.mk
#
####################

####################
####################
####################

ifndef F_CPU
 $(error Project F_CPU has not been set)
endif
export F_CPU

ifndef MCU
 $(error Project MCU has not been set)
endif
export MCU

# Process any additional include directories
ifdef INCLUDE
INCLUDE := $(foreach i, $(INCLUDE),-I$(i))
endif

ifdef LIB_INCLUDE
LIB_INCLUDE := $(foreach i, $(LIB_INCLUDE),-L$(i))
endif

ifndef BUILD_DIR
BUILD_DIR=build
endif

ifdef AVRDUDE_DEV
AVRDUDE_DEV := -P $(AVRDUDE_DEV)
endif

# Tooling
ifndef PREFIX
PREFIX := avr-
endif
OBJDUMP := $(PREFIX)objdump
OBJCOPY := $(PREFIX)objcopy
SIZE := $(PREFIX)size
AS := $(PREFIX)as
CC := $(PREFIX)gcc
CXX := $(PREFIX)g++
AVRDUDE := avrdude

# Flags
CFLAGS   += -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -ffunction-sections -fdata-sections -w -fno-exceptions $(INCLUDE)
CXXFLAGS += $(CFLAGS)
LDFLAGS  := -Wl,-Os,--relax,--gc-sections -mmcu=$(MCU) $(LIB_INCLUDE) $(LDFLAGS)
DEPFLAGS += -mmcu=$(MCU) -DF_CPU=$(F_CPU) -MM $(INCLUDE)

# Objects and their dependencies
OBJS := $(SOURCES:.cpp=.o)
OBJS := $(OBJS:.c=.o)
OBJS := $(addprefix $(BUILD_DIR)/, $(notdir $(OBJS)))

DEPS =  $(OBJS:.o=.d)
DEPS := $(addprefix $(BUILD_DIR)/, $(notdir $(DEPS)))

.SECONDARY:
.PHONY: all libraries build-directory $(LIBRARIES) status clean size upload upload_spi

ifdef LIBNAME
all : build-directory lib$(LIBNAME).a
else
all : build-directory libraries $(PROGRAM).hex
endif

build-directory : $(BUILD_DIR)

$(BUILD_DIR) :
	@mkdir -vp $(BUILD_DIR)

libraries : $(LIBRARIES)

$(LIBRARIES) :
	@echo "building library $@..."
	@$(MAKE) -C $@ all
	@echo "library built"

$(PROGRAM).elf : $(OBJS)
	@echo "LD $@"
	@$(CXX) -o $@ $(OBJS) $(LDFLAGS)
	@$(SIZE) $@

# only used when building a library
lib$(LIBNAME).a : $(OBJS)
	@echo "AR $@ $(OBJS)"
	@$(AR) cr $@ $(OBJS)

$(BUILD_DIR)/%.o: %.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	@echo "CXX $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

%.hex: %.elf
	@$(OBJCOPY) -j .text -j .data -O ihex $< $@
	@echo "design ready to program with $@"

$(BUILD_DIR)/%.d : %.c
	@echo "generating dependencies for $<"
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(DEPFLAGS) $< -MF"$@" -MT"$@ $(<:.c=.o)" ### generate dep for $<

$(BUILD_DIR)/%.d : %.cpp
	@echo "generating dependencies for $<"
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(DEPFLAGS) $< -MF"$@" -MT"$@ $(<:.cpp=.o)" ### generate dep for $<

upload : upload_spi

upload_spi: all $(PROGRAM).hex
	$(AVRDUDE) -p $(AVRDUDE_MCU) -c usbtiny $(AVRDUDE_DEV) -B 1 -V -U flash:w:$(PROGRAM).hex:i

upload_spi_slow: all $(PROGRAM).hex
	$(AVRDUDE) -p $(AVRDUDE_MCU) -c usbtiny $(AVRDUDE_DEV) -V -U flash:w:$(PROGRAM).hex:i

status:
	$(AVRDUDE) -p $(AVRDUDE_MCU) -c usbtiny $(AVRDUDE_DEV)

clean:
	@rm -fv $(PROGRAM).elf
	@rm -fv $(PROGRAM).hex
	@rm -fv $(BUILD_DIR)/*.o
	@rm -fv $(BUILD_DIR)/*.i
	@rm -fv $(BUILD_DIR)/*.d
	@-rmdir -v $(BUILD_DIR)
	@rm -fv *.a
ifdef LIBRARIES
	@for i in $(LIBRARIES); do \
	   echo "cleaning library $$i..."; \
	   $(MAKE) -s -C $$i clean; \
	   echo "library clean"; \
	done
endif

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(DEPS)
endif
