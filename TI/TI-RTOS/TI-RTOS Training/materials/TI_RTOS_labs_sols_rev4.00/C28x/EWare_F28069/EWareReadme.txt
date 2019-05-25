TI-RTOS workshop EWareReadme.txt for C28x Users of BIOS
Author: Eric Wilbur


Where did EWare come from?

EWare is what the workshop author called his subset of header files needed to create
the labs. EWare is "Extracted" Ware - the author's play on the Wares out there. The original
files came from the standard controlSUITE install at these locations:

C:\TI\controlSUITE\device_support\f2806x\v136\F2806x_common\source
C:\TI\controlSUITE\device_support\f2806x\v136\F2806x_headers\source

All C28x programmers will end up using a subset of header file code depending on what their
application needs. This implementation is no different. These files are simply COPIED into the 
project via IMPORT of the folder in CCS - this keeps them nice and tight inside a folder vs.
copying in each individual source file.

The files contained in the EWare folder are the only files necessary to perform the labs
in this workshop. However, when BIOS is added to the current header file code, there are
conflicts in the interrupts and timers areas. In all BIOS labs, the author excludes parts
of these files with the symbol "#ifndef ti_sysbios_BIOS__include". If BIOS is present in the
program, this symbol is activated and therefore the author used this symbol for exclusions of
code or files that conflict with BIOS.

For non-BIOS programs, all files in EWare are used. 

For BIOS programs, the following files are excluded using the symbol "ti_sysbios_BIOS__include". These
files have code that conflicts with BIOS' implementation and are therefore excluded:
	F2806x_PieCtrl.c
	F2806x_PieVect.c
	F2806x_DefaultIsr.c
	
For BIOS programs, the following FILE has code exclusions using the symbol "ti_sysbios_BIOS__include".
The use of Timer 1 and Timer 2 conflict with BIOS. Timer 0 is left alone:
	F2806x_CpuTimers.c
	
Also note that since we are running the code at 90MHz, flash/OTP wait states would normally be set to
the proper settings based on the datasheet for this device. In table 5-84, you will find the min wait
states at different frequencies. For 90MHz, this results in the settings shown in the InitFlash()
routine in F2806x_SysCtrl.c. We have opted NOT to call this fxn in order to simplify the code and focus
on TI-RTOS (BIOS) concepts only. In YOUR system, you will want to set the proper wait states. If you 
want to learn more about the architecture, consider taking the 3-day C28x workshop. Note that we are 
therefore running at MAX WAIT STATES (15) for Flash and OTP which will drastically affect benchmarks
throughout the workshop. Keep that in mind as you perform any BIOS or code benchmarks in this workshop.

*** END ***
