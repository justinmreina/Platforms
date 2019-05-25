//-----------------------------------------------------------------------------
// Title:   Technical Training Organization (TTO) BIOS Workshop 5.50
// File:	main.c, Lab3 - Intro to BIOS
// Rev:		1.0
// Date:	08-31-2010
// Author:	Eric Wilbur (ericw@ti.com)
// Ref:		C6748 BSL, Experimenter Test Example (audio, mcasp)
//
// Brief:	Init C6748 Timer/I2C/LEDs via BSL calls, IDL/PRD Fxn toggles LED
//
// Notes:	1. Refer to main.h for key definitions
//			2. If NO GEL file present, uncomment #define in main.h
//-----------------------------------------------------------------------------


//-------------------------------------
// INCLUDES FOR BSL FXNs
//-------------------------------------
#include "main.h"


//--------------------------------------
// main()
//--------------------------------------
void main(void){

#if NO_GEL								//if no GEL, init board/RAM
   EVMC6748_init();
   EVMC6748_initRAM();
#endif

   
   I2C_init(I2C0, I2C_CLK_400K);
   
   // init LEDs   
   LED_init();

   return;								//return to BIOS Scheduler
}


//-----------------------------------------------------------------------------
// USTIMER_delay()
//
// LogicPD BSL fxn - re-written for a few BSL.c files that need it.
// The original USTIMER_init() is not used because it is NOT BIOS compatible
// and took both timers so that BIOS PRDs would not work. This is a 
// workaround.
//
// If you need a "delay" in this app, call this routine with the number
// of usec's of delay you need. It is approximate - not exact.
// value for time<300 is perfect for 1us. We padded it some.
//-----------------------------------------------------------------------------

void USTIMER_delay(uint32_t usec)
{
	volatile int32_t i, start, time, current;

	for (i=0; i<usec; i++)
	{
		start = CLK_gethtime();
		time = 0;
		while (time < 350)
		{
			current = CLK_gethtime();
			time = current - start;
		}
	}
}


