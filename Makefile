# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, you can obtain one at http://mozilla.org/MPL/2.0/

##########################################################################
# User configuration and firmware specific object files
##########################################################################
NAME = btle_und_snes

TARGET := NRF51
BOARD := BOARD_BTLE_SNES

SHELL = /bin/bash

# Debugging stuff
TERMINAL ?= xterm -e
JLINKEXE := JLinkExe
JLINKGDB := JLinkGDBServer
JLINK_OPTIONS = -device nrf51822 -if swd -speed 1000

# Flash start address
FLASH_START_ADDRESS := 0x00000000

# Cross-compiler
TARGET_TRIPLE := arm-none-eabi

# Build Directory
BUILD := build

# Nordic stuff
#USE_SOFTDEVICE = s110
USE_SOFTDEVICE = blank

SOURCES = \
	source \
	linker

INCLUDES := \
	source \
	linker \
	include \
	include/gcc \
	include/esb \
	include/boards \
	kiwitest

LIBRARIES :=

LD_SCRIPT := gcc_nrf51_$(USE_SOFTDEVICE).ld

##########################################################################
# Compiler prefix and location
##########################################################################
CCACHE = ccache
HOST_AS = $(CCACHE) $(HOST_COMPILE)gcc
HOST_CC = $(CCACHE) $(HOST_COMPILE)gcc
HOST_LD = $(CCACHE) $(HOST_COMPILE)gcc
HOST_SIZE = $(HOST_COMPILE)size
HOST_OBJCOPY = $(HOST_COMPILE)objcopy
HOST_OBJDUMP = $(HOST_COMPILE)objdump

TARGET_PREFIX = $(TARGET_TRIPLE)-
TARGET_AS = $(CCACHE) $(TARGET_PREFIX)gcc
TARGET_CC = $(CCACHE) $(TARGET_PREFIX)gcc
TARGET_GDB = $(TARGET_PREFIX)gdb
TARGET_LD = $(CCACHE) $(TARGET_PREFIX)gcc
TARGET_SIZE = $(TARGET_PREFIX)size
TARGET_OBJCOPY = $(TARGET_PREFIX)objcopy
TARGET_OBJDUMP = $(TARGET_PREFIX)objdump

##########################################################################
# Misc. stuff
##########################################################################
UNAME := $(shell uname -s)

PROGRAM_VERSION := \
	$(shell git describe --always --tags --abbrev=7 2>/dev/null | \
	sed s/^v//)
ifeq ("$(PROGRAM_VERSION)","")
    PROGRAM_VERSION:='unknown'
endif

##########################################################################
# Compiler settings
##########################################################################

COMMON_FLAGS := -g -c -Wall -Werror -ffunction-sections -fdata-sections \
	-fmessage-length=0 -std=gnu99 \
	-DTARGET=$(TARGET) -D$(BOARD) -D$(TARGET) \
	-DPROGRAM_VERSION=\"$(PROGRAM_VERSION)\" \
	-DSEMIHOSTED \
	-DDEBUG_RESET_REASON=0 \

COMMON_ASFLAGS := -D__ASSEMBLY__ -x assembler-with-cpp

HOST_FLAGS := $(COMMON_FLAGS) $(INCLUDE) -DUSE_NATIVE_STDLIB=1
CLANG_FLAGS := -fcolor-diagnostics -Qunused-arguments
ifneq (,$(findstring clang,$(HOST_CC)))
	HOST_FLAGS += $(CLANG_FLAGS)
endif

# On MinGW hosts, we have to define this to get %z for printf.
ifneq (,$(findstring windows32,$(UNAME)))
	HOST_FLAGS += -D__USE_MINGW_ANSI_STDIO=1
endif

HOST_CFLAGS := $(HOST_FLAGS) -O0 --coverage 
HOST_ASFLAGS := $(HOST_FLAGS) $(COMMON_ASFLAGS)
HOST_LDFLAGS := --coverage
HOST_LDLIBS  = -lm -pthread -lrt 

TARGET_ARCHFLAGS := -march=armv6-m -mthumb -mcpu=cortex-m0
TARGET_FLAGS := $(COMMON_FLAGS) -O2 -fmerge-constants $(TARGET_ARCHFLAGS) $(INCLUDE)
TARGET_CFLAGS := $(TARGET_FLAGS)
TARGET_ASFLAGS := $(TARGET_FLAGS) $(COMMON_ASFLAGS)

TARGET_LDFLAGS = $(TARGET_ARCHFLAGS) -Wl,--gc-sections
TARGET_LDLIBS  =

##########################################################################
# Top-level Makefile
##########################################################################

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUTDIR := $(CURDIR)
export OUTPUT := $(OUTPUTDIR)/$(NAME)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) 
export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
HFILES := $(foreach dir,$(INCLUDES),$(notdir $(wildcard $(dir)/*.h)))
export LIBFILES := $(addprefix $(CURDIR)/,$(foreach dir,$(LIBRARIES),$(wildcard $(dir)/*.a)))

export OFILES := $(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))/ \
                  -I$(CURDIR)/$(BUILD)/

##########################################################################
# Targets
##########################################################################

.PHONY: $(BUILD) clean ctags test dotest flash debug

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -rf $(BUILD) $(NAME)_test $(NAME)_test.exe $(NAME)_test.xml \
		$(OUTPUT).bin tags alice_test.info test_coverage

ctags:
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile ctags

flash: $(BUILD)
	echo -e "r\nloadbin $(NAME).bin, $(FLASH_START_ADDRESS)\nr\ng\nexit\n" > .jlinkflash
	-$(JLINKEXE) $(JLINK_OPTIONS) .jlinkflash

debug: flash
	echo -e "file $(BUILD)/$(NAME).elf\n target remote localhost:2331\nbreak main\nmon semihosting enable\nmon semihosting ThumbSWI 0xAB\n" > .gdbinit
	$(TERMINAL) "$(JLINKGDB) $(JLINK_OPTIONS) -port 2331" &
	sleep 1
	$(TERMINAL) "telnet localhost 2333" &
	$(TERMINAL) "$(TARGET_GDB) $(BUILD)/$(NAME).elf"

##########################################################################
# Build-level Makefile
##########################################################################
else

DEPENDS := $(OFILES:.o=.d)

# Targets
.PHONY: ctags

all: $(OUTPUTDIR)/tags $(OUTPUT).bin \
	$(NAME).objdump 

ctags: $(OUTPUTDIR)/tags

##########################################################################
# Project-specific Build Rules
##########################################################################
$(OUTPUT).bin: $(NAME).bin
	@cp $< $@

$(NAME).elf: $(OFILES)
	@echo $(@F)
	@$(TARGET_LD) $(TARGET_LDFLAGS) -Wl,-Map=$(@F).map \
		-L $(CURDIR)/../linker -T $(LD_SCRIPT) -o $@ $(OFILES) \
		$(TARGET_LDLIBS) $(LIBFILES)

	-@( cd $(OUTPUTDIR); )

$(NAME).objdump: $(NAME).elf

$(OUTPUTDIR)/tags: $(CFILES) $(HFILES)
	@ctags -f $@ --totals -R \
		$(OUTPUTDIR)/source \
		$(OUTPUTDIR)/include \
		$(OUTPUTDIR)/Makefile

##########################################################################
# General Build Rules
##########################################################################
%.host.o : %.c
	@echo $(@F)
	@$(HOST_CC) -MMD -MP -MF $(DEPSDIR)/$*.host.d $(HOST_CFLAGS) -o $@ $<

%.host.o : %.s
	@echo $(@F)
	@$(HOST_AS) -MMD -MP -MF $(DEPSDIR)/$*.host.d $(HOST_ASFLAGS) -o $@ $<

%.o : %.c
	@echo $(@F)
	@$(TARGET_CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(TARGET_CFLAGS) -o $@ $<

%.o : %.s
	@echo $(@F)
	@$(TARGET_AS) -MMD -MP -MF $(DEPSDIR)/$*.d $(TARGET_ASFLAGS) -o $@ $<

%.objdump: %.elf
	@echo $(@F)
	@$(TARGET_OBJDUMP) -dSl -Mforce-thumb $< > $@

%.hex: %.elf
	@echo $(@F)
	@$(TARGET_OBJCOPY) $(TARGET_OCFLAGS) -O ihex $< $@

%.bin: %.elf
	@echo $(@F)
	@$(TARGET_OBJCOPY) $(TARGET_OCFLAGS) -O binary $< $@

-include $(DEPENDS)

endif
