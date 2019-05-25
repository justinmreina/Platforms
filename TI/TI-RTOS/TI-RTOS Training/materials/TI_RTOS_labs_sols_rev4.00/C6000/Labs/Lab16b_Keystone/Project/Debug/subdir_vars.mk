################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../keystone_flash.cfg 

LIB_SRCS += \
C:/SYSBIOSv4/Labs/DSP_BSL/bsl/lib/evmomapl138_bsl.lib 

C_SRCS += \
../aic3106_TTO.c \
../fir.c \
../isr.c \
../led.c \
../main.c \
../mcasp_TTO.c 

OBJS += \
./aic3106_TTO.obj \
./fir.obj \
./isr.obj \
./led.obj \
./main.obj \
./mcasp_TTO.obj 

GEN_SRCS += \
./configPkg/compiler.opt \
./configPkg/linker.cmd 

C_DEPS += \
./aic3106_TTO.pp \
./fir.pp \
./isr.pp \
./led.pp \
./main.pp \
./mcasp_TTO.pp 

GEN_MISC_DIRS += \
./configPkg/ 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_OPTS += \
./configPkg/compiler.opt 

GEN_SRCS__QUOTED += \
"configPkg\compiler.opt" \
"configPkg\linker.cmd" 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

C_DEPS__QUOTED += \
"aic3106_TTO.pp" \
"fir.pp" \
"isr.pp" \
"led.pp" \
"main.pp" \
"mcasp_TTO.pp" 

OBJS__QUOTED += \
"aic3106_TTO.obj" \
"fir.obj" \
"isr.obj" \
"led.obj" \
"main.obj" \
"mcasp_TTO.obj" 

C_SRCS__QUOTED += \
"../aic3106_TTO.c" \
"../fir.c" \
"../isr.c" \
"../led.c" \
"../main.c" \
"../mcasp_TTO.c" 

CFG_SRCS__QUOTED += \
"../keystone_flash.cfg" 

GEN_OPTS__FLAG += \
--cmd_file="./configPkg/compiler.opt" 

GEN_CMDS__FLAG += \
-l"./configPkg/linker.cmd" 


