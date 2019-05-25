//----------------------------------------------------------------------------------
// Project: Blink C28x Using BIOS (STARTER)
// Author: Eric Wilbur
// Date: June 2014
//
// Note: The following files are NOT needed when using BIOS:
//		 F2806x_PieCtrl.c, F2806x_PieVect.c, F2806x_DefaultIsr.c
//
//       BIOS takes care of all vector and ISR management
//       Read the EWareReadme.txt file for more details on this.
//
// Follow these steps to create this project in CCSv6.0:
// 1. Project -> New CCS Project
// 2. Select Template:
//    - TI-RTOS for C2000 -> Kernel Examples -> TI Target Ex -> Minimal ->
//    - this template does NOT contain ControlSuite or UIA/RTOS Analyzer support
// 3. Add UIA from available products
// 4. Add EWare_F28069_BIOS to project (subset of all C files)
// 5. Add F2806x_Headers_BIOS.cmd
// 6. Add main.c from TI-RTOS Workshop Solution file for this lab
// 7. Edit as needed (to add/subtract) BIOS services, delete given Task
// 8. Add include search paths (two of them from _headers and _common)
// 9. Build, load, run...
//------------------------------------------------------------------------------------


//----------------------------------------
// BIOS header files
//----------------------------------------
#include <xdc/std.h>  						//mandatory - have to include first, for BIOS types
#include <ti/sysbios/BIOS.h> 				//mandatory - if you call APIs like BIOS_start()
#include <xdc/runtime/Log.h>				//for Log_info() calls when UIA is added
#include <xdc/cfg/global.h> 				//header file for statically defined objects/handles


//-----------------------------------------
// ControlSuite Header Files
//-----------------------------------------
#include "DSP28x_Project.h"


//-----------------------------------------
// Prototypes
//-----------------------------------------
void hardware_init(void);
void ledToggle(void);
void delay(void);


//-----------------------------------------
// Globals
//-----------------------------------------
volatile int16_t i16ToggleCount = 0;


//---------------------------------------------------------------------------
// main()
//---------------------------------------------------------------------------
void main(void)
{

   hardware_init();							// init hardware via Xware

   while(1)									// forever loop
   {
	   ledToggle();							// toggle LED

	   delay();								// create a delay of ~1/2sec

	   i16ToggleCount += 1;					// keep track of #toggles
   }

}


//-----------------------------------------------------------------------------
// hardware_init()
//-----------------------------------------------------------------------------
void hardware_init(void)					//called by main
{

// Init PLL, watchdog, periph clocks - see F2806x_SysCtrl.c file
// Clock frequency set to 90 MHz - see F2806x_Examples.h
	InitSysCtrl();

// Copy InitFlash fxn to RAM and run it - sets flash wait states for 90MHz
	memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(unsigned long)&RamfuncsLoadSize);
	InitFlash();
	
// Configure GPIO34 (LD2 LED2 of Control Stick) as a GPIO output pin
	EALLOW;
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
	EDIS;

}


//---------------------------------------------------------------------------
// ledToggle()
//
// Toggle LED via GPIO pin (LD2 on 28069 Control Stick)
//---------------------------------------------------------------------------
void ledToggle(void)
{
	GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1; 		// Toggle GPIO34 (LD2) of Control Stick
}


//-----------------------------------------------------------------------------
// delay()
//
// Note: DELAY_US(time) does work in this lab, but conflicts w/BIOS. So, we're
//       just using a dumb for() loop  ;-)
//-----------------------------------------------------------------------------
void delay(void)							//called by main
{
	uint32_t u32i;

	for (u32i=0; u32i<700000; u32i++);		//delay ~1/2sec

}




