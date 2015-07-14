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
ifdef LIBRARIES
INCLUDE += $(foreach i, $(LIBRARIES), $(dir $(i)))
LIB_INCLUDE += $(foreach i, $(LIBRARIES), $(dir $(i)))
LIBS = $(LIBRARIES)
endif

ifdef INCLUDE
INCLUDE := $(foreach i, $(INCLUDE),-I$(i))
endif

ifdef LIB_INCLUDE
LIB_INCLUDE := $(foreach i, $(LIB_INCLUDE),-L$(i))
endif

#ifdef LINK_LIBS
#LDFLAGS := $(foreach i, $(LINK_LIBS),-l$(i))
#LIBS = $(foreach i, $(LINK_LIBS),lib$(i).a)
#endif

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
LD := $(PREFIX)g++
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
all : build-directory $(LIBRARIES) lib$(LIBNAME).a
else
all : build-directory $(LIBRARIES) $(PROGRAM).hex
endif

build-directory : $(BUILD_DIR)

$(BUILD_DIR) :
	@mkdir -vp $(BUILD_DIR)

ifdef LIBRARIES
define BUILD_LIB
$T : $(dir $T)
	@echo "building library $(basename $(notdir $T))..."
	@$(MAKE) --no-print-directory -C $$< all
endef

$(foreach T,$(LIBRARIES),$(eval $(BUILD_LIB)))
endif

##########################

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(DEPS)
endif

# C files
$(BUILD_DIR)/%.o: %.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# C++ files
$(BUILD_DIR)/%.o: %.cpp
	@echo "CXX $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# linking step
$(PROGRAM).elf : $(DEPS) $(OBJS) $(LIBS)
	@echo "LD $@"
	@$(LD) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	@$(SIZE) $@

# Build static library
#
# NOTE: there is "ar magic" going on here
# This command makes use of thin archive, which will
# flatten any added AR archive in to the newly created one.
# This allows lower libraries to correctly order their dependencies
# for building and to have that propagate up the tree
lib$(LIBNAME).a : $(DEPS) $(OBJS) $(LIBRARIES)
	@echo "AR $@ $(OBJS) $(foreach i,$(LIBRARIES),$(notdir $(i)))"
	@$(AR) cTr $@ $(OBJS) $(LIBRARIES)


# elf -> hex for programming
%.hex: %.elf
	@$(OBJCOPY) -j .text -j .data -O ihex $< $@
	@echo "design ready to program with $@"

# generate deps
$(BUILD_DIR)/%.d : %.c
	@echo "generating dependencies for $<"
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(DEPFLAGS) $< -MF"$@" -MT"$@ $(<:.c=.o)" ### generate dep for $<

$(BUILD_DIR)/%.d : %.cpp
	@echo "generating dependencies for $<"
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(DEPFLAGS) $< -MF"$@" -MT"$@ $(<:.cpp=.o)" ### generate dep for $<

##########################

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
	@-rmdir $(BUILD_DIR) 2>/dev/null
	@rm -fv *.a
ifdef LIBRARIES
	@for i in $(LIBRARIES); do \
	   echo "cleaning library $$(basename $$i)..."; \
	   $(MAKE) -s -C $$(dirname $$i) clean; \
	done
endif
