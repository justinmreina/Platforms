//-----------------------------------------------------------------------------
// \file    evmomapl138_pll.h
// \brief   OMAP-L138 phase locked loop controller registers & bit definitions.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_PLL_H
#define EVMOMAPL138_PLL_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
   volatile uint32_t REVID;         // 0x0000
   volatile uint32_t RSVD0[56];     // 0x0004
   volatile uint32_t RSTYPE;        // 0x00E4
   volatile uint32_t RSVD1[6];      // 0x00E8
   volatile uint32_t PLLCTL;        // 0x0100
   volatile uint32_t OCSEL;         // 0x0104
   volatile uint32_t RSVD2[2];      // 0x0108
   volatile uint32_t PLLM;          // 0x0110
   volatile uint32_t PREDIV;        // 0x0114
   volatile uint32_t PLLDIV1;       // 0x0118
   volatile uint32_t PLLDIV2;       // 0x011C
   volatile uint32_t PLLDIV3;       // 0x0120
   volatile uint32_t OSCDIV;        // 0x0124
   volatile uint32_t POSTDIV;       // 0x0128
   volatile uint32_t RSVD3[3];      // 0x012C
   volatile uint32_t PLLCMD;        // 0x0138
   volatile uint32_t PLLSTAT;       // 0x013C
   volatile uint32_t ALNCTL;        // 0x0140
   volatile uint32_t DCHANGE;       // 0x0144
   volatile uint32_t CKEN;          // 0x0148
   volatile uint32_t CKSTAT;        // 0x014C
   volatile uint32_t SYSTAT;        // 0x0150
   volatile uint32_t RSVD4[3];      // 0x0154
   volatile uint32_t PLLDIV4;       // 0x0160
   volatile uint32_t PLLDIV5;       // 0x0164
   volatile uint32_t PLLDIV6;       // 0x0168
   volatile uint32_t PLLDIV7;       // 0x016C
} pll_regs_t;

#define PLL0         ((pll_regs_t*)PLL0_REG_BASE)
#define PLL1         ((pll_regs_t*)PLL1_REG_BASE)

// bitmask defines for PLLCTL.
#define EXTCLKSRC       (0x00000200)   // bit 9
#define CLKMODE         (0x00000100)   // bit 8
#define PLLENSRC        (0x00000020)   // bit 5
#define PLLDIS          (0x00000010)   // bit 4
#define PLLRST          (0x00000008)   // bit 3
#define PLLPWRDN        (0x00000002)   // bit 1
#define PLLEN           (0x00000001)   // bit 0
#define CLKMODE_SHIFT   (8)

// bitmask defines for PLLCMD and PLLSTAT.
#define GOSET        (0x00000001)
#define GOSTAT       (0x00000001)

// defines for divisors.
#define DIV_ENABLE         (0x00008000)
#define MULTIPLIER         (24)
#define POSTDIV_RATIO      (1)
#define PLLDIV2_RATIO      (2)
#define PLLDIV3_RATIO      (12)
#define PLLDIV4_RATIO      (4)
#define PLLDIV5_RATIO      (5)
#define PLLDIV6_RATIO      (1)
#define PLLDIV7_RATIO      (6)
#define PLL_LOCK_CYCLES          (2400)
#define PLL_STABILIZATION_TIME   (2000)
#define PLL_RESET_TIME_CNT       (200)

// system clock frequency defines.
#define AUXCLOCK_HZ        (24)
#define SYSCLOCK1_HZ       (300000000)
#define SYSCLOCK2_HZ       (SYSCLOCK1_HZ / PLLDIV2_RATIO)
#define SYSCLOCK3_HZ       (SYSCLOCK1_HZ / PLLDIV3_RATIO)
#define SYSCLOCK4_HZ       (SYSCLOCK1_HZ / PLLDIV4_RATIO)
#define SYSCLOCK5_HZ       (SYSCLOCK1_HZ / PLLDIV5_RATIO)
#define SYSCLOCK6_HZ       (SYSCLOCK1_HZ / PLLDIV6_RATIO)
#define SYSCLOCK7_HZ       (SYSCLOCK1_HZ / PLLDIV7_RATIO)

// bitmask defines for OCSEL (OBSCLK pin).
#define OBSCLK_CLKIN       (0x14)
#define OBSCLK_SYSCLK1     (0x17)
#define OBSCLK_SYSCLK2     (0x18)
#define OBSCLK_SYSCLK3     (0x19)
#define OBSCLK_SYSCLK4     (0x1A)
#define OBSCLK_SYSCLK5     (0x1B)
#define OBSCLK_SYSCLK6     (0x1C)
#define OBSCLK_SYSCLK7     (0x1D)

#endif
