//-----------------------------------------------------------------------------
// \file    evmomapl138_uPP.c
// \brief   implementation of uPP driver for OMAP-L138.
//
//-----------------------------------------------------------------------------


#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_uPP.h"
#include "evmomapl138_GPIO.h"
#include "evmomapl138_emac.h"

//-----------------------------------------------------------------------------
// Private Defines 
//-----------------------------------------------------------------------------
#define PINMUX_UPP_REG_0	(19)
#define PINMUX_UPP_MASK_0	(0x000F0000)
#define PINMUX_UPP_VAL_0	(0x00010000)

#define PINMUX_UPP_REG_1	(13)
#define PINMUX_UPP_MASK_1	(0xFFFF0000)
#define PINMUX_UPP_VAL_1	(0x48440000)

#define PINMUX_UPP_REG_2	(14)
#define PINMUX_UPP_MASK_2	(0xFFFFFF00)
#define PINMUX_UPP_VAL_2	(0x44444400)

#define PINMUX_UPP_REG_3	(15)
#define PINMUX_UPP_MASK_3	(0xFFFFFFFF)
#define PINMUX_UPP_VAL_3	(0x44444444)

#define PINMUX_UPP_REG_4	(16)
#define PINMUX_UPP_MASK_4	(0xFFFFFFFF)
#define PINMUX_UPP_VAL_4	(0x44444444)

#define PINMUX_UPP_REG_5	(17)
#define PINMUX_UPP_MASK_5	(0xFFFFFFFF)
#define PINMUX_UPP_VAL_5	(0x44444444)

#define PINMUX_UPP_REG_6	(15)
#define PINMUX_UPP_MASK_6	(0x00FFFFFF)
#define PINMUX_UPP_VAL_6	(0x00444444)

#define PINMUX_UPP_REG_7	(18)
#define PINMUX_UPP_MASK_7	(0x00FFFFFF)
#define PINMUX_UPP_VAL_7	(0x00444444)

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the upp port
//
// \param   upp_config_t * config: structure containing nessisary information 
//									to setup upp
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...upp ready to use.
//-----------------------------------------------------------------------------

uint32_t UPP_init(upp_config_t * config)
{
   uint32_t rtn = ERR_INIT_FAIL;
   uint32_t i = 0;
   
   
   EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_RPI, PSC_ENABLE);	//Provide power to uPP


   //take care of pin muxing
   EVMOMAPL138_pinmuxConfig(PINMUX_UPP_REG_0,PINMUX_UPP_MASK_0, PINMUX_UPP_VAL_0);
   EVMOMAPL138_pinmuxConfig(PINMUX_UPP_REG_1,PINMUX_UPP_MASK_1, PINMUX_UPP_VAL_1);
   EVMOMAPL138_pinmuxConfig(PINMUX_UPP_REG_2,PINMUX_UPP_MASK_2, PINMUX_UPP_VAL_2);
   EVMOMAPL138_pinmuxConfig(PINMUX_UPP_REG_3,PINMUX_UPP_MASK_3, PINMUX_UPP_VAL_3);
   EVMOMAPL138_pinmuxConfig(PINMUX_UPP_REG_4,PINMUX_UPP_MASK_4, PINMUX_UPP_VAL_4);
   EVMOMAPL138_pinmuxConfig(PINMUX_UPP_REG_5,PINMUX_UPP_MASK_5, PINMUX_UPP_VAL_5);
   EVMOMAPL138_pinmuxConfig(PINMUX_UPP_REG_6,PINMUX_UPP_MASK_6, PINMUX_UPP_VAL_6);
   EVMOMAPL138_pinmuxConfig(PINMUX_UPP_REG_7,PINMUX_UPP_MASK_7, PINMUX_UPP_VAL_7);
   
   //enable the ADC 
	GPIO_setDir(GPIO_BANK6, GPIO_PIN9, GPIO_OUTPUT); 	
	GPIO_setOutput(GPIO_BANK6, GPIO_PIN9, OUTPUT_LOW);	

   //reset uPP
   SETBIT(UPP->UPPCR, UPP_UPPCR_SWRST);
   for(i = 0; i < 300; i++){};      		//wait 200 clock cycles for reset.
   CLRBIT(UPP->UPPCR, UPP_UPPCR_SWRST);
   
   //setup control registers
   UPP->UPCTL=config->UPCTL.value;
   UPP->UPICR=config->UPICR.value; 
   UPP->UPIVR=config->UPIVR.value; 
   UPP->UPTCR=config->UPTCR.value;
   UPP->UPDLB=config->UPDLB.value;  
   UPP->UPIES=config->UPIES.value;
   UPP->UPPCR=config->UPPCR.value;
    
    
   return (rtn);
}

