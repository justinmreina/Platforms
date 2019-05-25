//-----------------------------------------------------------------------------
// Title:   Technical Training Organization (TTO) SYSBIOS 1.5-DAY Workshop 0.80
// File:	led.c, Lab2B - Intro to SYSBIOS 
// Rev:		0.80
// Date:	07-25-2011
// Author:	Eric Wilbur (ericw@ti.com)
// Ref:		C6748 BSL, LED Fxns
//
// Brief:	Toggle LED_1 every half second (called via Clock Fxn - ledToggleClock)
//
// Notes:	1. Refer to main.h for key definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// INCLUDES (BSL, main.h)
//-----------------------------------------------------------------------------

#include "main.h"


//-----------------------------------------------------------------------------
// ledToggle()
//-----------------------------------------------------------------------------
void ledToggle(void)								//called by Clock Fxn
{
	static int16_t count=0;

	uint32_t start, finish, result;					// variables for benchmarking
    uint32_t cpu_load;

	while(1)
	{
		Semaphore_pend(ledToggleSem, BIOS_WAIT_FOREVER);
		
		count += 1;

		start = Timestamp_get32();
		LED_toggle(LED_1);								//toggle LED_1 on C6748 EVM
		finish = Timestamp_get32();
		
		result = finish - start;

		Log_info1("BENCHMARK = [%u] cycles", result);	//send benchmark to Log message
		
		cpu_load = Load_getCPULoad();					//get realtime CPU load

		Log_info1("CPU LOAD = [%u]", cpu_load); 			//send Log RTA a message (debug)			

		Log_info1("TOGGLED LED [%u] times", count); 	//send Log RTA a message (debug)			
	}
}
   
void ledTogglePost (void)
{
	Semaphore_post (ledToggleSem);
}


