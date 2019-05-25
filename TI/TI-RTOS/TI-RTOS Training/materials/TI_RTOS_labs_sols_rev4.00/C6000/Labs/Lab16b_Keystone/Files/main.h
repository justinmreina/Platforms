//-----------------------------------------------------------------------------
// Title:   Technical Training Organization (TTO) SYS/BIOS 1.5-DAY Workshop 0.80
// File:	main.h 
// Rev:		0.80
// Date:	07-25-2011
// Author:	Eric Wilbur (ericw@ti.com)
// Ref:		C6748 BSL, Experimenter Test Example (audio, mcasp)
//
// Brief:	Definitions, prototypes and externs
//
// Notes:	1. Refer to main.h for key definitions
//-----------------------------------------------------------------------------
#ifndef MAIN_H_
#define MAIN_H_

//-------------------------------------------------------
// SYS/BIOS HEADER FILES
//-------------------------------------------------------
//these header files are MUSTS for the BIOS workshop labs (and most other systems)

#include <xdc/std.h>  								//mandatory - have to include first, for BIOS types
#include <ti/sysbios/BIOS.h> 						//mandatory - if you call APIs like BIOS_start()
#include <xdc/cfg/global.h> 						//header file for statically defined objects/handles


// sysbios kernel header files only needed if object is created dynamically.
// static sysbios objects are taken care of by the global.h file listed above.
// however, we include them here for full default coverage - in case we create dynamic objects later
// xdc runtime modules require a header file and are therefore included below


#include <ti/sysbios/knl/Clock.h> 					//when using Clock module (dynamically)
#include <ti/sysbios/knl/Idle.h>					//when using Idle (dynamically)
#include <ti/sysbios/knl/Task.h>					//when using Tasks (dynamically)
#include <ti/sysbios/knl/Semaphore.h>				//when using Semaphores (dynamically)
#include <ti/sysbios/knl/Swi.h>						//when using Idle (dyanmically)
#include <ti/sysbios/utils/Load.h>					//when using load_getCPULoad()

#include <xdc/runtime/Timestamp.h> 					//when using Timestamp APIs (TSCL/H), 32bit, 64bit
#include <xdc/runtime/System.h> 					//for runtime system calls (e.g. system_printf)
#include <xdc/runtime/Log.h>						//for Log_info() calls



//-----------------------------------------------------------------------------
// LogicPD BSL Includes
//-----------------------------------------------------------------------------
#include "stdio.h"

#include "evmomapl138.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_i2c.h"
#include "evmomapl138_mcasp.h"
#include "evmomapl138_aic3106.h"
#include "evmomapl138_led.h"

//-----------------------------------------------------------------------------
// Definitions - BUFFSIZE, PING/PONG, FILTER ORDER
//-----------------------------------------------------------------------------
#define BUFFSIZE 256
#define PING     0
#define PONG     1


//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------

// Note: for the first part of the lab, uncomment FIR_process (src, dst, len)
//       and comment out FIR_process(void). When the SWI is employed, do just
//       the opposite as shown below...

//void FIR_process(int16_t * Src, int16_t * Dst, uint16_t len);
void FIR_process(void);

void isrAudio(void);
void init_buffers(void);
void McASP_Init_TTO();									// init McASP
void AIC3106_Init_TTO();								// init AIC3106
void McASP_Start_TTO();
void USTIMER_delay(uint32_t time);
void ledToggle(void);
void ledTogglePost (void);


//-----------------------------------------------------------------------------
// Externs
//-----------------------------------------------------------------------------
extern int16_t rcvPing[BUFFSIZE];
extern int16_t rcvPong[BUFFSIZE];
extern int16_t xmtPing[BUFFSIZE];
extern int16_t xmtPong[BUFFSIZE];

extern uint16_t pingPong;

extern	cregister volatile unsigned int	CSR;	// control status register
extern	cregister volatile unsigned int	ICR;	// interrupt clear register
extern	cregister volatile unsigned int	IER;	// interrupt enable reg.



#endif



