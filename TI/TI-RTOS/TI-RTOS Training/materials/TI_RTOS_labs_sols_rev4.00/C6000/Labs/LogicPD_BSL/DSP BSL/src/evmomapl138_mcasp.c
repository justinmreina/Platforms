//-----------------------------------------------------------------------------
// \file    evmomapl138_mcasp.c
// \brief   implementation of a mcasp driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_mcasp.h"

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
// Private Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

uint32_t MCASP_init(void)
{
   // enable the psc and config pinmux for mcasp.
   EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_MCASP0, PSC_ENABLE);
   EVMOMAPL138_pinmuxConfig(PINMUX_MCASP_REG_0, PINMUX_MCASP_MASK_0, PINMUX_MCASP_VAL_0);
   EVMOMAPL138_pinmuxConfig(PINMUX_MCASP_REG_1, PINMUX_MCASP_MASK_1, PINMUX_MCASP_VAL_1);
   
   // reset mcasp.
   MCASP->GBLCTL  = 0;

   // configure receive registers.
   MCASP->RMASK      = 0xFFFFFFFF;
   MCASP->RFMT       = 0x00008078;
   MCASP->AFSRCTL    = 0x00000112;
   MCASP->ACLKRCTL   = 0x000000AF;
   MCASP->AHCLKRCTL  = 0x00000000;
   MCASP->RTDM       = 0x00000003;
   MCASP->RINTCTL    = 0x00000000;
   MCASP->RCLKCHK    = 0x00FF0008;

   // configure transmit registers.
   MCASP->XMASK      = 0xFFFFFFFF;
   MCASP->XFMT       = 0x00008078;
   MCASP->AFSXCTL    = 0x00000112;
   MCASP->ACLKXCTL   = 0x000000AF;
   MCASP->AHCLKXCTL  = 0x00000000;
   MCASP->XTDM       = 0x00000003;
   MCASP->XINTCTL    = 0x00000000;
   MCASP->XCLKCHK    = 0x00FF0008;

   // config serializers (11 = xmit, 12 = rcv).
   MCASP->SRCTL11    = 0x000D;
   MCASP->SRCTL12    = 0x000E;

   // config pin function and direction.
   MCASP->PFUNC      = 0;
   MCASP->PDIR       = 0x14000800;

   //
   MCASP->DITCTL     = 0x00000000;
   MCASP->DLBCTL     = 0x00000000;
   MCASP->AMUTE      = 0x00000000;

   MCASP->XSTAT = 0x0000FFFF;        // Clear all
   MCASP->RSTAT = 0x0000FFFF;        // Clear all
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------
