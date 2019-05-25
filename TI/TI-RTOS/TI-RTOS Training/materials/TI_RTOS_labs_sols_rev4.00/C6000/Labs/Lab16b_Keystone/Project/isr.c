//-----------------------------------------------------------------------------
// Title:   Technical Training Organization (TTO) SYS/BIOS 1.5-DAY Workshop 0.80
// File:	isr.c 
// Rev:		0.8
// Date:	07-25-2011
// Author:	Eric Wilbur (ericw@ti.com)
// Ref:		C6748 BSL, Experimenter Test Example (audio, mcasp)
//
// Brief:	Interrupt Service Routine (INT5), Triggered when RDATA/XDATA ready
//          One ISR for both RCV/XMT, Read & Write 32 bits EVERY INTERRUPT
//          McASP combines interrupts (RCV/XMT), Datasheet = EVT SRC #61
//			With a combined interrupt, we must check to see WHICH side (RCV or XMT)
//			caused the interrupt. Otherwise, we are reading or writing twice
//			the necessary data (and it causes a slight static noise).
//
//
// Notes:	1. Refer to main.h for key definitions
//			2. Refer to .CFG (INT5) properties for more info
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "main.h"


//-----------------------------------------------------------------------------
// isrAudio()
//-----------------------------------------------------------------------------
void isrAudio(void)
{
  	static int32_t 		dataIn32, dataOut32;			//store McASP audio data
	static int16_t 		blkCnt = 0;						//blk counter
	static int16_t 		*pInBuf_local, *pOutBuf_local;	//local pointers to global buffers


//Init local pointers for ping/pong buffering

	if (blkCnt == 0){									//If new buffer, swap ping/pong
   		if (pingPong == PING){							//PING Buffers
			pInBuf_local = rcvPing;		 
			pOutBuf_local  = xmtPing;
   		}
		else {											//PONG Buffers
			pInBuf_local = rcvPong;		 
			pOutBuf_local  = xmtPong;
		}
	}
	
	// McASP set to 16-bit slot size - always read 32, data in upper half, ROR 16-bit ENABLED	
	// ROR 16 shifts the upper half to the lower half of dataIn32

	// This ISR is executed for either a RCV (RRDY) or XMT (XRDY) interrupt from the McASP.
	// Therefore, we must check to see if it was RRDY or XRDY that triggered this ISR and
	// read or write accordingly.


	// check for receive ready
 	if(CHKBIT(MCASP->SRCTL12, RRDY)) {				//if ISR triggered by RRDY...
 		
		//read 32 bits from McASP (ROR 16, so lower 16 bits valid) and store in InBuf	
		dataIn32 = MCASP->XBUF12;
		pInBuf_local[blkCnt] = (int16_t)(dataIn32);
		
	}
	
	
	// check for transmit ready
   	if(CHKBIT(MCASP->SRCTL11, XRDY)) {				//if ISR triggered by XRDY...
   		
		//read 16 bits from XMT buffer and send to McASP XMT 
		dataOut32 = pOutBuf_local[blkCnt];
		MCASP->XBUF11 = dataOut32;
	
  		blkCnt++;	
  											
	}

	//IF end of buffer, copy rcv-to-xmt, zero blkCnt, swap PING-PONG boolean
	if (blkCnt >= BUFFSIZE){

//		FIR_process(pInBuf_local, pOutBuf_local, BUFFSIZE);			//copy RCV to XMT buffer (ALGO)

//		Swi_post (firProcessSwi);

		Semaphore_post (mcaspReady);

		blkCnt = 0;

		pingPong ^=1;

	}
	
}

  
   



