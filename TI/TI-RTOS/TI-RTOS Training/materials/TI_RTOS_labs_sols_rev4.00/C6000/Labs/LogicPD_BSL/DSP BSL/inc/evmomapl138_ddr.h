//-----------------------------------------------------------------------------
// \file    evmomapl138_ddr.h
// \brief   OMAP-L138 ddr registers & bit definitions.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_DDR_H
#define EVMOMAPL138_DDR_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
  volatile uint32_t REVID;          // 0x0000
  volatile uint32_t SDRSTAT;        // 0x0004
  volatile uint32_t SDCR;           // 0x0008
  volatile uint32_t SDRCR;          // 0x000C
  volatile uint32_t SDTIMR1;        // 0x0010
  volatile uint32_t SDTIMR2;        // 0x0014
  volatile uint32_t RSVD1;          // 0x0018
  volatile uint32_t SDCR2;          // 0x001C
  volatile uint32_t PBBPR;          // 0x0020
  volatile uint32_t RSVD2;          // 0x0024
  volatile uint32_t VBUSMCFG1;      // 0x0028
  volatile uint32_t VBUSMCFG2;      // 0x002C
  volatile uint32_t RSVD3[36];      // 0x0030
  volatile uint32_t IRR;            // 0x00C0
  volatile uint32_t IMR;            // 0x00C4
  volatile uint32_t IMSR;           // 0x00C8
  volatile uint32_t IMCR;           // 0x00CC
  volatile uint32_t RSVD4[4];       // 0x00D0
  volatile uint32_t DDRPHYREV;      // 0x00E0
  volatile uint32_t DDRPHYCTL1;     // 0x00E4
  volatile uint32_t DDRPHYCTL2;     // 0x00E8
} ddr_regs_t;

#define DDR          ((ddr_regs_t*)DDR_REG_BASE)

#define VTPIO_CTL    (*(uint32_t *)(0x1E2C0000))

#endif
