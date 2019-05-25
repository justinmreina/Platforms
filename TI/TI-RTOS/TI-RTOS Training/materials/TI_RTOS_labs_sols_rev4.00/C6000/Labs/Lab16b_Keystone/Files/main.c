//-----------------------------------------------------------------------------
// Title:   Technical Training Organization (TTO) SYS/BIOS 1.5-DAY Workshop 0.80
// File:	main.c 
// Rev:		0.80
// Date:	07-25-2011
// Author:	Eric Wilbur (ericw@ti.com)
// Ref:		C6748 BSL, Experimenter Test Example (audio, mcasp)
//
// Brief:	Definitions, prototypes and externs
//
// Notes:	1. Refer to main.h for key definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "main.h"


//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
int16_t rcvPing[BUFFSIZE];	// ping/pong buffers
int16_t rcvPong[BUFFSIZE];
int16_t xmtPing[BUFFSIZE];
int16_t xmtPong[BUFFSIZE];

uint16_t pingPong = PING;


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------

void main(void)
{
    init_buffers();								// zero buffers					

   	I2C_init(I2C0, I2C_CLK_400K);				// init I2C comm channel

	LED_init();									// init LED
	
   	McASP_Init_TTO();							// init McASP
   	AIC3106_Init_TTO();							// init AIC3106

	McASP_Start_TTO();							// start McASP clocks

	BIOS_start();  								// start BIOS scheduler
}												 


//-----------------------------------------------------------------------------
// init_buffers()
//-----------------------------------------------------------------------------

void init_buffers(void)
{
int16_t i;

	for (i=0; i<BUFFSIZE; i++)				// zero all buffers
	{
		rcvPing[i]=0;
		rcvPong[i]=0;
		xmtPing[i]=0;
		xmtPong[i]=0;
	}
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
		start = Timestamp_get32();
		time = 0;
		while (time < 350)
		{
			current = Timestamp_get32();
			time = current - start;
		}
	}
}



