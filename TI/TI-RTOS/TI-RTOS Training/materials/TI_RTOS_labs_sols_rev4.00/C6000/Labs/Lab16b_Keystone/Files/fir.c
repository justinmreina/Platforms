//-----------------------------------------------------------------------------
// Title:   Technical Training Organization (TTO) SYS/BIOS 1.5-DAY Workshop 0.80
// File:	fir.c 
// Rev:		0.80
// Date:	07-25-2011
// Author:	Eric Wilbur (ericw@ti.com)
// Ref:		C6748 BSL, Experimenter Test Example (audio, mcasp)
//
// Brief:	placeholder (memcpy) for real FIR routine
//			contains TWO versions. One is a standard fxn that is called by isrAudio
//			The second is called via a SWI object (SWI_firProcess)
//
// Notes:	1. Refer to main.h for key definitions
//			2. Refer to .CFG (INT5) properties for more info
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "main.h"

//---------------------------------------
// Swi and Task version of FIR_process
//---------------------------------------

void FIR_process(void){

	while(1)
	{
		Semaphore_pend (mcaspReady, BIOS_WAIT_FOREVER);
		
		if (pingPong == PING){
			memcpy (xmtPong, rcvPong, BUFFSIZE * sizeof(int16_t));}     //copy PONG
		else {
			memcpy (xmtPing, rcvPing, BUFFSIZE * sizeof(int16_t));}		//copy PING
	}
}


//---------------------------------------
// Hwi version of FIR_process
//---------------------------------------

//void FIR_process(int16_t * Src, int16_t * Dst, uint16_t len){

//		memcpy (Dst, Src, len * sizeof(uint16_t));       //copy RCV to XMT

//}




