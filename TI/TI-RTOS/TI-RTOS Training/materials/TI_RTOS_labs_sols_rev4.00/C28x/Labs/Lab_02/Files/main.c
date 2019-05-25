//---------------------------------------------------------------------------
// Project: Blink C28x CCS (STARTER)
// Author: Eric Wilbur
// Date: June 2014
//
// EWare is a subset of the header files for F28069. For this lab, the files
// are not modified. However, for future BIOS labs, some files use ifndef
// to exclude some pieces of the header file code that conflicts with BIOS.
//---------------------------------------------------------------------------


//-----------------------------------------
// ControlSuite Header Files
//-----------------------------------------
#include "DSP28x_Project.h"
#include "stdint.h"


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
	
// Clear all ints, init PIE vector table - see F2806x_PieCtrl.c file
	DINT;

	InitPieCtrl();

	IER = 0x0000;
	IFR = 0x0000;

// Init table, populate table including unused INTs
// The shell ISR routines are found in F2806x_DefaultIsr.c.
// This function is found in F2806x_PieVect.c.
	InitPieVectTable();

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
// Note: DELAY_US(time) does work in this lab, but conflicts w/BIOS in the
//       next lab, so we're just using a dumb for() loop  ;-)
//-----------------------------------------------------------------------------
void delay(void)							//called by main
{
	uint32_t u32i;

	for (u32i=0; u32i<3200000; u32i++);		//delay ~1/2sec

}






