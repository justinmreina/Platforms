//-----------------------------------------------------------------------------
// \file    evmomapl138_psc.h
// \brief   OMAP-L138 power and sleep config registers & bit definitions.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_PSC_H
#define EVMOMAPL138_PSC_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
  volatile uint32_t REVID;          // 0x0000
  volatile uint32_t RSVD0[5];       // 0x0004
  volatile uint32_t INTEVAL;        // 0x0018
  volatile uint32_t RSVD1[9];       // 0x001C
  volatile uint32_t MERRPR0;        // 0x0040
  volatile uint32_t RSVD2[3];       // 0x0044
  volatile uint32_t MERRCR0;        // 0x0050
  volatile uint32_t RSVD3[3];       // 0x0054
  volatile uint32_t PERRPR;         // 0x0060
  volatile uint32_t RSVD4;          // 0x0064
  volatile uint32_t PERRCR;         // 0x0068
  volatile uint32_t RSVD5[45];      // 0x006C
  volatile uint32_t PTCMD;          // 0x0120
  volatile uint32_t RSVD6;          // 0x0124
  volatile uint32_t PTSTAT;         // 0x0128
  volatile uint32_t RSVD7[53];      // 0x012C
  volatile uint32_t PDSTAT0;        // 0x0200
  volatile uint32_t PDSTAT1;        // 0x0204
  volatile uint32_t RSVD8[62];      // 0x0208
  volatile uint32_t PDCTL0;         // 0x0300
  volatile uint32_t PDCTL1;         // 0x0304
  volatile uint32_t RSVD9[62];      // 0x0308
  volatile uint32_t PDCFG0;         // 0x0400
  volatile uint32_t PDCFG1;         // 0x0404
  volatile uint32_t RSVD10[254];    // 0x0408
  volatile uint32_t MDSTAT[32];     // 0x0800
  volatile uint32_t RSVD11[96];     // 0x0880
  volatile uint32_t MDCTL[32];      // 0x0A00
} psc_regs_t;

// define the power and sleep config modules.
#define PSC0            ((psc_regs_t *)PSC0_REG_BASE)
#define PSC1            ((psc_regs_t *)PSC1_REG_BASE)

// domain defines.
#define DOMAIN0         (0x00000001)
#define DOMAIN1         (0x00000002)

// defines for ptcmd.
#define GO_0            (0x00000001)
#define GO_1            (0x00000002)

// defines for ptstat.
#define GOSTAT_0        (0x00000001)
#define GOSTAT_1        (0x00000002)

// psc0 lpsc defines.
#define LPSC_TPCC          (0)
#define LPSC_TPTC0         (1)
#define LPSC_TPTC1         (2)
#define LPSC_EMIFA         (3)
#define LPSC_SPI0          (4)
#define LPSC_MMCSD0        (5)
#define LPSC_AINTC         (6)
#define LPSC_ARM_RAMROM    (7)
// 8 not used.
#define LPSC_UART0         (9)
#define LPSC_SCR0          (10)
#define LPSC_SCR1          (11)
#define LPSC_SCR2          (12)
#define LPSC_DMAX          (13)
#define LPSC_ARM           (14)
#define LPSC_DSP           (15)

// psc1 lpsc defines.
// 0 not used.
#define LPSC_USB0                (1)
#define LPSC_USB1                (2)
#define LPSC_GPIO                (3)
#define LPSC_HPI                 (4)
#define LPSC_EMAC                (5)
#define LPSC_EMIF3A              (6)
#define LPSC_MCASP0              (7)
#define LPSC_SATA                (8)
#define LPSC_VPIF                (9)
#define LPSC_SPI1                (10)
#define LPSC_I2C1                (11)
#define LPSC_UART1               (12)
#define LPSC_UART2               (13)
#define LPSC_MCBSP0              (14)
#define LPSC_MCBSP1              (15)
#define LPSC_LCDC                (16)
#define LPSC_PWM                 (17)
#define LPSC_MMCSD1              (18)
#define LPSC_RPI                 (19)
#define LPSC_ECAP                (20)
#define LPSC_TPTC2               (21)
// 22-23 not used.
#define LPSC_SCR8                (24)
#define LPSC_SCR7                (25)
#define LPSC_SCR12               (26)
// 27-30 not used.
#define LPSC_SHRAM               (31)

// psc module status register defines.
#define MASK_STATE               (0x0000003F)

// psc module control register defines.
#define FORCE                    (0x80000000)   // bit 31.
#define EMUIHBIE                 (0x00000400)   // bit 10.
#define EMURSTIE                 (0x00000200)   // bit 9.
#define LRST                     (0x00000100)   // bit 8.
#define NEXT                     (0x00000007)   // bits 0-2.

// psc module next states.
#define PSC_ENABLE               (0x00000003)
#define PSC_DISABLE              (0x00000002)
#define PSC_SYNCRESET            (0x00000001)
#define PSC_SWRSTDISABLE         (0x00000000)

#endif
