//-----------------------------------------------------------------------------
// Title:   Technical Training Organization (TTO) SYS/BIOS 4-DAY Workshop 6.10
// File:	mcasp_TTO.c 
// Rev:		6.10
// Date:	12-01-2011
// Author:	Eric Wilbur (ericw@ti.com)
// Ref:		C6748 BSL, Experimenter Test Example (audio, mcasp)
//
// Brief:	Init McASP clocks/control & interrupts, dummy writes to XDATA
//
// Notes:	1. Refer to main.h for key definitions
//			2. Underrun is a SERIOUS condition on the McASP. Notice that
//             dummy writes occur at selected points to keep XDATA filled
//			3. For symbol definitions, refer to evmc6748_mcasp.h
//			
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "main.h"

//-----------------------------------------------------------------------------
// DEFINES (enable XDATA/RDATA interrupts in McASP)
//-----------------------------------------------------------------------------
#define RDATA 0x20 							// R/XDATA interrupt mask
#define XDATA 0x20

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
// pinmux defines.
#define PINMUX_MCASP_REG_0       (0)
#define PINMUX_MCASP_MASK_0      (0x00FFFFFF)
#define PINMUX_MCASP_VAL_0       (0x00111111)
#define PINMUX_MCASP_REG_1       (1)
#define PINMUX_MCASP_MASK_1      (0x000FF000)
#define PINMUX_MCASP_VAL_1       (0x00011000)


//-----------------------------------------------------------------------------
// McASP_Init_TTO()
//-----------------------------------------------------------------------------

void McASP_Init_TTO(void)
{
   // enable the psc and config pinmux for mcasp.
   EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_MCASP0, PSC_ENABLE);
   EVMOMAPL138_pinmuxConfig(PINMUX_MCASP_REG_0, PINMUX_MCASP_MASK_0, PINMUX_MCASP_VAL_0);
   EVMOMAPL138_pinmuxConfig(PINMUX_MCASP_REG_1, PINMUX_MCASP_MASK_1, PINMUX_MCASP_VAL_1);
   
   
   
   // reset mcasp.
   MCASP->GBLCTL  = 0;

   // NOTE: ROR 16-bits enabled for both XMT/RCV. SLOT SIZE = 16 bits, 1-bit delay
   //       MCLK = 2.456MHz, CLKRDIV = 16, SO, bitclk = MCLK/CLKRDIV = 1.536MHz
   //       SAMPLE RATE = 1.536MHz/32bits = 48KHz

   // configure receive registers for I2S
   MCASP->RMASK      = 0xFFFFFFFF;		// all 32-bits NOT masked
   MCASP->RFMT       = 0x0001807C;		// MSB first, align left, slot=16bits, 1-bit delay, ROR 16-bits
   MCASP->AFSRCTL    = 0x00000112;		// int'l gen'd, FS/word, 2 SLOT TDM = I2S
   MCASP->ACLKRCTL   = 0x000000AF;		// rising edge, clkrm internal, /16 CLKRDIV
   MCASP->AHCLKRCTL  = 0x00000000;		// HCLKRDIV = 1
   MCASP->RTDM       = 0x00000003;		// SLOT 0 & 1 active I2S
   MCASP->RINTCTL    = 0x00000000;		// ints disabled (enabled later though)
   MCASP->RCLKCHK    = 0x00FF0008;		// RMAX = FF, RPS = /256

   // configure transmit registers for I2S - all same as above
   MCASP->XMASK      = 0xFFFFFFFF;
   MCASP->XFMT       = 0x0001807C;		 
   MCASP->AFSXCTL    = 0x00000112;
   MCASP->ACLKXCTL   = 0x000000AF;
   MCASP->AHCLKXCTL  = 0x00000000;
   MCASP->XTDM       = 0x00000003;
   MCASP->XINTCTL    = 0x00000000;
   MCASP->XCLKCHK    = 0x00FF0008;

   // config serializers (11 = xmit, 12 = rcv)
   MCASP->SRCTL11    = 0x000D;					// XMT
   MCASP->SRCTL12    = 0x000E;					// RCV

   // config pin function and direction.
   MCASP->PFUNC      = 0;
   MCASP->PDIR       = 0x14000800;

   MCASP->DITCTL     = 0x00000000;
   MCASP->DLBCTL     = 0x00000000;
   MCASP->AMUTE      = 0x00000000;

}

//-----------------------------------------------------------------------------
// McASP_Start_TTO()
//-----------------------------------------------------------------------------

void McASP_Start_TTO(void)
{

   // enable the audio clocks, verifying each bit is properly set.
	SETBIT(MCASP->XGBLCTL, XHCLKRST);
    while (!CHKBIT(MCASP->XGBLCTL, XHCLKRST)) {}
    SETBIT(MCASP->RGBLCTL, RHCLKRST);
    while (!CHKBIT(MCASP->RGBLCTL, RHCLKRST)) {}

    SETBIT(MCASP->XGBLCTL, XCLKRST);
    while (!CHKBIT(MCASP->XGBLCTL, XCLKRST)) {}
    SETBIT(MCASP->RGBLCTL, RCLKRST);
    while (!CHKBIT(MCASP->RGBLCTL, RCLKRST)) {}

   	SETBIT(MCASP->RINTCTL, RDATA);					// enable McASP XMT/RCV interrupts
   	while (!CHKBIT(MCASP->RINTCTL, RDATA)) {}		// see #defines at top of file
   	SETBIT(MCASP->XINTCTL, XDATA);
  	while (!CHKBIT(MCASP->XINTCTL, XDATA)) {}

    MCASP->XSTAT = 0x0000FFFF;        // Clear all (see procedure in UG)
    MCASP->RSTAT = 0x0000FFFF;        // Clear all


   SETBIT(MCASP->XGBLCTL, XSRCLR);
   while (!CHKBIT(MCASP->XGBLCTL, XSRCLR)) {}
   SETBIT(MCASP->RGBLCTL, RSRCLR);
   while (!CHKBIT(MCASP->RGBLCTL, RSRCLR)) {}

   /* Write a 0, so that no underrun occurs after releasing the state machine */
   MCASP->XBUF11 = 0;

   SETBIT(MCASP->XGBLCTL, XSMRST);
   while (!CHKBIT(MCASP->XGBLCTL, XSMRST)) {}
   SETBIT(MCASP->RGBLCTL, RSMRST);
   while (!CHKBIT(MCASP->RGBLCTL, RSMRST)) {}

   SETBIT(MCASP->XGBLCTL, XFRST);
   while (!CHKBIT(MCASP->XGBLCTL, XFRST)) {}
   SETBIT(MCASP->RGBLCTL, RFRST);
   while (!CHKBIT(MCASP->RGBLCTL, RFRST)) {}

   // wait for transmit ready and send a dummy byte.
   while(!CHKBIT(MCASP->SRCTL11, XRDY)) {}
   MCASP->XBUF11 = 0;
   
}






