################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me -Ooff --include_path="D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/source" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/driverlib/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/inc/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/example/common/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink_extlib/provisioninglib" -g --define=ccs --define=USER_INPUT_ENABLE --define=cc3200 --diag_wrap=off --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

network_common.obj: D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/example/common/network_common.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me -Ooff --include_path="D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/source" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/driverlib/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/inc/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/example/common/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink_extlib/provisioninglib" -g --define=ccs --define=USER_INPUT_ENABLE --define=cc3200 --diag_wrap=off --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="network_common.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

pinmux.obj: ../pinmux.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me -Ooff --include_path="D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/source" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/driverlib/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/inc/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/example/common/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink_extlib/provisioninglib" -g --define=ccs --define=USER_INPUT_ENABLE --define=cc3200 --diag_wrap=off --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="pinmux.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup_ccs.obj: D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/example/common/startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me -Ooff --include_path="D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/source" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/driverlib/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/inc/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/example/common/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink_extlib/provisioninglib" -g --define=ccs --define=USER_INPUT_ENABLE --define=cc3200 --diag_wrap=off --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

uart_if.obj: D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/example/common/uart_if.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me -Ooff --include_path="D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/source" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/driverlib/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/inc/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/example/common/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink_extlib/provisioninglib" -g --define=ccs --define=USER_INPUT_ENABLE --define=cc3200 --diag_wrap=off --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="uart_if.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

udma_if.obj: D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/example/common/udma_if.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me -Ooff --include_path="D:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.7/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/include" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink/source" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/driverlib/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/inc/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/example/common/" --include_path="D:/Documents/MyWorkspaces/CCS-Suppl/CC3200-WIFI-Example/cc3200-sdk/simplelink_extlib/provisioninglib" -g --define=ccs --define=USER_INPUT_ENABLE --define=cc3200 --diag_wrap=off --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="udma_if.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


