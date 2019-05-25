#############################################################################
# Makefile_src                                                              #
#                                                                           #
#  Common make file invoked by fft example project                          #
#############################################################################
#
#
#############################################################################
#                                                                           #
#   Copyright (C) 2010 Texas Instruments Incorporated                       #
#     http://www.ti.com/                                                    #
#                                                                           #
#############################################################################

# Use the main DMAI Rules.make for building apps in this directory
ifndef DSPLIBC64P_INSTALL_DIR
    DSPLIBC64P_INSTALL_DIR = ../..
endif

include $(DSPLIBC64P_INSTALL_DIR)/Rules.make

#Set Target name 
TARGET = $(notdir $(CURDIR))
CUR_DIR = "."
# Comment this out if you want to see full compiler and linker output.
VERBOSE = @

export CODEGEN_INSTALL_DIR	

#Linker command file used
LFILE = lnk.cmd
INCLUDES = -i$(CODEGEN_INSTALL_DIR)/include  \
           -i$(DSPLIBC64P_INSTALL_DIR)/lib  \
           -i$(DSPLIBC64P_INSTALL_DIR)/include  \
           -i$(DSPLIBC64P_INSTALL_DIR)/src/DSP_fft16x16  \
           -i$(DSPLIBC64P_INSTALL_DIR)/src/DSP_fft32x32  \
           -i$(DSPLIBC64P_INSTALL_DIR)/src/DSP_fft16x32  \
                               
MAP_FILE = $(BUILD_TYPE)/$(TARGET).map

EXE = $(BUILD_TYPE)/$(TARGET).out

#Set Compiler and Linker flags based on Configuration specified
ifeq ($(BUILD_TYPE), Release)
    LD_FLAGS 	+= -z -c -m $(MAP_FILE) -o$(EXE) -w -x -i $(CODEGEN_INSTALL_DIR)/lib -i $(DSPLIBC64P_INSTALL_DIR)/lib -ldsplib64plus.lib
    C_FLAGS	+= -o3 $(INCLUDES) -mw -mv6400+ -fr"$(BUILD_TYPE)" -fs"$(BUILD_TYPE)" -ft"$(BUILD_TYPE)" -d"__TI_EABI__=0"
endif
ifeq ($(BUILD_TYPE), Release_elf)
    LD_FLAGS 	+= --abi=eabi -z -c -m $(MAP_FILE) -o$(EXE) -w -x -i $(CODEGEN_INSTALL_DIR)/lib  -i $(DSPLIBC64P_INSTALL_DIR)/lib -ldsplib64plus_elf.lib
    C_FLAGS	+= --abi=eabi -o3 $(INCLUDES) -mw -mv6400+ -fr"$(BUILD_TYPE)" -fs"$(BUILD_TYPE)" -ft"$(BUILD_TYPE)" 
endif

#Set Compiler and Linker paths and flags for the build
COMPILE.c = $(VERBOSE) $(CODEGEN_INSTALL_DIR)/bin/cl6x $(C_FLAGS)
LINK.c = $(VERBOSE) $(CODEGEN_INSTALL_DIR)/bin/cl6x $(LD_FLAGS)

# C Sources to be built
SOURCES = $(wildcard *.c) 
GENTW16x16_CSRC = ../../src/DSP_fft16x16/gen_twiddle_fft16x16.c
GENTW32x32_CSRC = ../../src/DSP_fft32x32/gen_twiddle_fft32x32.c
GENTW16x32_CSRC = ../../src/DSP_fft16x32/gen_twiddle_fft16x32.c

#Assembly source files to be built
ASMSRC  = $(wildcard *.sa)

# Header to be included
HEADERS = $(wildcard *.h) \
          $(wildcard $(CODEGEN_INSTALL_DIR)/include/*.h) \
          $(wildcard ../../src/DSP_fft16x16/*.h) \
          $(wildcard ../../src/DSP_fft32x32/*.h) \
          $(wildcard ../../src/DSP_fft16x32/*.h) \
          $(wildcard ../../include/*.h)

#Object files to be built
OBJFILES = $(SOURCES:%.c=$(BUILD_TYPE)/%.obj) $(ASMSRC:%.sa=$(BUILD_TYPE)/%.obj)
GENTW16x16_OBJ  = $(GENTW16x16_CSRC:../../src/DSP_fft16x16/%.c=$(BUILD_TYPE)/%.obj)
GENTW32x32_OBJ  = $(GENTW32x32_CSRC:../../src/DSP_fft32x32/%.c=$(BUILD_TYPE)/%.obj)
GENTW16x32_OBJ  = $(GENTW16x32_CSRC:../../src/DSP_fft16x32/%.c=$(BUILD_TYPE)/%.obj)  

.PHONY: clean install 

all:	TARGET

TARGET: $(BUILD_TYPE)/$(TARGET).out

install:
	@mkdir -p $(EXEC_DIR)
	@echo Copying outfile $(TARGET).out from $(BUILD_TYPE)/
	@install -m 755 $(BUILD_TYPE)/$(TARGET).out $(EXEC_DIR)/
	@chmod -x $(EXEC_DIR)/*.out 

$(BUILD_TYPE)/$(TARGET).out: $(OBJFILES) $(GENTW16x16_OBJ) $(GENTW16x32_OBJ) $(GENTW32x32_OBJ) $(LFILE)
	@echo
	@echo Linking $@ from $^..
	@echo $(LINK.c) -o $@ $^
	$(LINK.c) -o $@ $^ 

$(BUILD_TYPE)/%.obj: %.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) $(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) $(INCLUDES) $<

$(BUILD_TYPE)/%.obj: ../../src/DSP_fft16x16/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) $(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) $(INCLUDES) $<

$(BUILD_TYPE)/%.obj: ../../src/DSP_fft16x32/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) $(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) $(INCLUDES) $<

$(BUILD_TYPE)/%.obj: ../../src/DSP_fft32x32/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) $(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) $(INCLUDES) $<


clean:
	@echo Removing generated files..
	$(VERBOSE) -$(RM) -r $(BUILD_TYPE) *~ *.d .dep
