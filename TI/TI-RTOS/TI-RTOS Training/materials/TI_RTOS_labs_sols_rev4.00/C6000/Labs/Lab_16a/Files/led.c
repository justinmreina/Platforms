//-----------------------------------------------------------------------------
// Title:   Technical Training Organization (TTO) BIOS Workshop 5.50
// File:	led.c, Lab3 - Intro to BIOS 
// Rev:		1.0
// Date:	08-31-2010
// Author:	Eric Wilbur (ericw@ti.com)
// Ref:		C6748 BSL, Experimenter Test Example (DIP_LED)
//
// Brief:	Toggle LED_1 every half second (called via IDL or PRD)
//
// Notes:	1. Refer to main.h for key definitions
//			2. If NO GEL file present, uncomment #define in main.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// INCLUDES (BSL, main.h)
//-----------------------------------------------------------------------------

#include "main.h"


//-----------------------------------------------------------------------------
// ledToggle()
//-----------------------------------------------------------------------------
void ledToggle(void)					//called by IDL thread or PRD
{
	LED_toggle(LED_1);					//toggle LED_1 on C6748 EVM
    USTIMER_delay(DELAY_HALF_SEC);		//wait half-second
}
   



