//-----------------------------------------------------------------------------
// Title:   Technical Training Organization (TTO) BIOS Workshop 5.50
// File:	main.h, Lab3 - Intro to BIOS 
// Rev:		1.0
// Date:	08-31-2010
// Author:	Eric Wilbur (ericw@ti.com)
// Ref:		C6748 BSL, Experimenter Test Example (DIP_LED)
//
// Brief:	header file for prototypes
//
// Notes:	1. Refer to main.h for key definitions
//			2. If NO GEL file present, uncomment #define in main.h
//-----------------------------------------------------------------------------

// uncomment this define if running without gel initialization.
// #define NO_GEL    (1)


//-------------------------------------------------------
// HEADER FILES
//-------------------------------------------------------
								//bios include file
#include "stdio.h"
#include "types.h"
#include "evmc6748.h"
#include "evmc6748_timer.h"
#include "evmc6748_i2c.h"
#include "evmc6748_led.h"
#include "evmc6748_gpio.h"
#include <clk.h>


//-------------------------------------------------------
// PROTOTYPES
//-------------------------------------------------------
void ledToggle (void);
void USTIMER_delay(uint32_t time);


