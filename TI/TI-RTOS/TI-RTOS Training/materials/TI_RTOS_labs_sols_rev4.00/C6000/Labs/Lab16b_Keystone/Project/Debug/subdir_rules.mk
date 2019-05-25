################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
aic3106_TTO.obj: ../aic3106_TTO.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -g --include_path="C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/include" --include_path="C:/SYSBIOSv4/Labs/DSP_BSL/bsl/inc" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="aic3106_TTO.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

fir.obj: ../fir.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -g --include_path="C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/include" --include_path="C:/SYSBIOSv4/Labs/DSP_BSL/bsl/inc" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="fir.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

isr.obj: ../isr.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -g --include_path="C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/include" --include_path="C:/SYSBIOSv4/Labs/DSP_BSL/bsl/inc" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="isr.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/compiler.opt: ../keystone_flash.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/CCStudio_v5.1.0/xdctools_3_22_04_46/xs" --xdcpath="C:/CCStudio_v5.1.0/bios_6_32_05_54/packages;C:/CCStudio_v5.1.0/ipc_1_23_05_40/packages;C:/SYSBIOSv4/Labs/SYSBIOS_Platforms;" xdc.tools.configuro -o configPkg -t ti.targets.C674 -p evmc6748_FLASH -r release -c "C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000" --compileOptions "-g --optimize_with_debug" "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: configPkg/compiler.opt
configPkg/: configPkg/compiler.opt

led.obj: ../led.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -g --include_path="C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/include" --include_path="C:/SYSBIOSv4/Labs/DSP_BSL/bsl/inc" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="led.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -g --include_path="C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/include" --include_path="C:/SYSBIOSv4/Labs/DSP_BSL/bsl/inc" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

mcasp_TTO.obj: ../mcasp_TTO.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -g --include_path="C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000/include" --include_path="C:/SYSBIOSv4/Labs/DSP_BSL/bsl/inc" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="mcasp_TTO.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


