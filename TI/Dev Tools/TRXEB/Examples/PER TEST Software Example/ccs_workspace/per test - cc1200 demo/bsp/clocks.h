#ifndef _CLOCKS_H_
#define _CLOCKS_H_

//Libraries
#include <stdint.h>

//PROCESSOR
#include <msp430.h>


//Defines
#define DCO_MULT_16MHZ          488
#define DCORSEL_16MHZ           DCORSEL_5
#define VCORE_16MHZ             PMMCOREV_1

#define BSP_SYS_CLK_16MHZ       16000000UL

//Globals
extern void clocks_init(void);


//Locals
void jmr_bspMcuSetVCore(uint8_t ui8Level);
void jmr_bspMcuSetVCoreUp(uint8_t ui8Level);
void jmr_bspMcuSetVCoreDown(uint8_t ui8Level);



#endif

