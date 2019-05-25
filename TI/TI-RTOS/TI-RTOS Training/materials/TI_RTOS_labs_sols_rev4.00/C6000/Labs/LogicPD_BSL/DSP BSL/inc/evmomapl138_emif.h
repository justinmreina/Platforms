//-----------------------------------------------------------------------------
// \file    evmomapl138_emif.h
// \brief   OMAP-L138 emif registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_EMIF_H
#define EVMOMAPL138_EMIF_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
   volatile uint32_t REVID;               // 0x0000
   volatile uint32_t AWCC;                // 0x0004
   volatile uint32_t SDCR;                // 0x0008
   volatile uint32_t SDRCR;               // 0x000C
   volatile uint32_t CE2CFG;              // 0x0010
   volatile uint32_t CE3CFG;              // 0x0014
   volatile uint32_t CE4CFG;              // 0x0018
   volatile uint32_t CE5CFG;              // 0x001C
   volatile uint32_t SDTIMR;              // 0x0020
   volatile uint32_t RSVD0[6];            // 0x0024
   volatile uint32_t SDSRETR;             // 0x003C
   volatile uint32_t INTRAW;              // 0x0040
   volatile uint32_t INTMASK;             // 0x0044
   volatile uint32_t INTMASKSET;          // 0x0048
   volatile uint32_t INTMASKCLR;          // 0x004C
   volatile uint32_t RSVD1[4];            // 0x0050
   volatile uint32_t NANDFCR;             // 0x0060
   volatile uint32_t NANDFSR;             // 0x0064
   volatile uint32_t PMCR;                // 0x0068
   volatile uint32_t NANDF1ECC;           // 0x0070
   volatile uint32_t NANDF2ECC;           // 0x0074
   volatile uint32_t NANDF3ECC;           // 0x0078
   volatile uint32_t NANDF4ECC;           // 0x007C
   volatile uint32_t RSVD2[15];           // 0x0080
   volatile uint32_t NAND4BITECCLOAD;     // 0x00BC
   volatile uint32_t NAND4BITECC1;        // 0x00C0
   volatile uint32_t NAND4BITECC2;        // 0x00C4
   volatile uint32_t NAND4BITECC3;        // 0x00C8
   volatile uint32_t NAND4BITECC4;        // 0x00CC
   volatile uint32_t NANDERRADDR1;        // 0x00D0
   volatile uint32_t NANDERRADDR2;        // 0x00D4
   volatile uint32_t NANDERRVAL1;         // 0x00D8
   volatile uint32_t NANDERRVAL2;         // 0x00DC
} emifa_regs_t;

#define EMIFA        ((emifa_regs_t *)EMIFA_REG_BASE)

// bitmask defines for CEnCFG.
#define SS                 (0x80000000)   // bit 31
#define EW                 (0x80000000)   // bit 30

#define ASIZE              (0x00000001)   // bits 0-1

// bitmask defines for NANDFCR.
#define FOURBITECC_START   (0x00001000)   // bit 12
#define CS5NAND            (0x00000008)   // bit 3
#define CS4NAND            (0x00000004)   // bit 2
#define CS3NAND            (0x00000002)   // bit 1
#define CS2NAND            (0x00000001)   // bit 0
#define ECCSEL_MASK        (0x00000030)   // bits 4-5
#define ECCSEL_CS3         (0x00000010)   // bits 4
#define ECCSEL_CS4         (0x00000020)   // bits 5
#define ECCSEL_CS5         (0x00000030)   // bits 4-5

// bitmask defines for NANDFSR.
#define WAITST             (0x00000001)   // bit 0

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t EMIFA_init(void);


#endif


