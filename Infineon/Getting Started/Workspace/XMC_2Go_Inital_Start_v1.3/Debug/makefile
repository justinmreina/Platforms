################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Libraries/XMCLib/src/subdir.mk
-include Libraries/Newlib/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
XMC_2Go_Inital_Start_v1.3.hex \

SECONDARY_LIST += \
XMC_2Go_Inital_Start_v1.3.lst \

SECONDARY_SIZE += \
XMC_2Go_Inital_Start_v1.3.siz \


LIST_FILES = $(addsuffix .o.lst, $(basename $(OBJS))) 
MAP_FILE = XMC_2Go_Inital_Start_v1.3.map

# All Target
all: XMC_2Go_Inital_Start_v1.3.elf secondary-outputs

# Tool invocations
XMC_2Go_Inital_Start_v1.3.elf: $(OBJS) ../linker_script.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM-GCC C Linker'
	$(file >objects.rsp,$(OBJS))
	"C:\DAVEv4-64Bit\DAVE-4.2.6\eclipse\ARM-GCC-49/bin/arm-none-eabi-gcc" -T"../linker_script.ld" -nostartfiles -Xlinker --gc-sections -specs=nano.specs -Wl,-Map,$(MAP_FILE) -mcpu=cortex-m0 -mthumb -g -gdwarf-2 -o "XMC_2Go_Inital_Start_v1.3.elf" "@objects.rsp" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo.
XMC_2Go_Inital_Start_v1.3.hex: XMC_2Go_Inital_Start_v1.3.elf
	@echo 'Invoking: ARM-GCC Create Flash Image'
	"C:\DAVEv4-64Bit\DAVE-4.2.6\eclipse\ARM-GCC-49/bin/arm-none-eabi-objcopy" -O ihex "XMC_2Go_Inital_Start_v1.3.elf"  "XMC_2Go_Inital_Start_v1.3.hex"
	@echo 'Finished building: $@'
	@echo.
XMC_2Go_Inital_Start_v1.3.lst: XMC_2Go_Inital_Start_v1.3.elf
	@echo 'Invoking: ARM-GCC Create Listing'
	"C:\DAVEv4-64Bit\DAVE-4.2.6\eclipse\ARM-GCC-49/bin/arm-none-eabi-objdump" -h -S "XMC_2Go_Inital_Start_v1.3.elf" > "XMC_2Go_Inital_Start_v1.3.lst"
	@echo 'Finished building: $@'
	@echo.
XMC_2Go_Inital_Start_v1.3.siz: XMC_2Go_Inital_Start_v1.3.elf
	@echo 'Invoking: ARM-GCC Print Size'
	"C:\DAVEv4-64Bit\DAVE-4.2.6\eclipse\ARM-GCC-49/bin/arm-none-eabi-size"  --format=berkeley "XMC_2Go_Inital_Start_v1.3.elf"
	@echo 'Finished building: $@'
	@echo.
# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ARM_GCC_FOR_XMC_LINKER_OUTPUT_TYPE_OUTPUTS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) XMC_2Go_Inital_Start_v1.3.elf $(LIST_FILES) $(MAP_FILE) objects.rsp
	-@echo.
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
