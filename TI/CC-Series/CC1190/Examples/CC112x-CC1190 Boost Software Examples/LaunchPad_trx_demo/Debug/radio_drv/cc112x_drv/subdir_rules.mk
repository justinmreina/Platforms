################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
radio_drv/cc112x_drv/cc112x_drv.obj: ../radio_drv/cc112x_drv/cc112x_drv.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.1.LTS/bin/cl430" -vmspx --abi=eabi --data_model=restricted -O2 --opt_for_speed=0 --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.1.LTS/include" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/hal_mcu" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/radio_drv" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/uart_drv" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/radio_drv/cc1101_drv" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/radio_drv/cc112x_drv" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/radio_drv/cc1190_drv" --advice:power="all" -g --define=__MSP430F5529__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="radio_drv/cc112x_drv/cc112x_drv.d" --obj_directory="radio_drv/cc112x_drv" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

radio_drv/cc112x_drv/cc112x_utils.obj: ../radio_drv/cc112x_drv/cc112x_utils.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.1.LTS/bin/cl430" -vmspx --abi=eabi --data_model=restricted -O2 --opt_for_speed=0 --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.1.LTS/include" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/hal_mcu" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/radio_drv" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/uart_drv" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/radio_drv/cc1101_drv" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/radio_drv/cc112x_drv" --include_path="D:/Documents/Ref/Platforms/TI/CC-Series/CC1190/Examples/CC112x-CC1190 Boost Software Examples/LaunchPad_trx_demo/radio_drv/cc1190_drv" --advice:power="all" -g --define=__MSP430F5529__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="radio_drv/cc112x_drv/cc112x_utils.d" --obj_directory="radio_drv/cc112x_drv" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


