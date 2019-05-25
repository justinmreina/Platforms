//-----------------------------------------------------------------------------
// \file    evmomapl138_mmcsd.h
// \brief   OMAP-L138 mmc/sd registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_MMCSD_H
#define EVMOMAPL138_MMCSD_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
   volatile uint32_t MMCCTL;              // 0x0000
   volatile uint32_t MMCCLK;              // 0x0004
   volatile uint32_t MMCST0;              // 0x0008
   volatile uint32_t MMCST1;              // 0x000C
   volatile uint32_t MMCIM;               // 0x0010
   volatile uint32_t MMCTOR;              // 0x0014
   volatile uint32_t MMCTOD;              // 0x0018
   volatile uint32_t MMCBLEN;             // 0x001C
   volatile uint32_t MMCNBLK;             // 0x0020
   volatile uint32_t MMCNBLC;             // 0x0024
   volatile uint32_t MMCDRR;              // 0x0028
   volatile uint32_t MMCDXR;              // 0x002C
   volatile uint32_t MMCCMD;              // 0x0030
   volatile uint32_t MMCARGHL;            // 0x0034
   volatile uint32_t MMCRSP01;            // 0x0038
   volatile uint32_t MMCRSP23;            // 0x003C
   volatile uint32_t MMCRSP45;            // 0x0040
   volatile uint32_t MMCRSP67;            // 0x0044
   volatile uint32_t MMCDRSP;             // 0x0048
   volatile uint32_t RSDV0;               // 0x004C
   volatile uint32_t MMCCIDX;             // 0x0050
   volatile uint32_t RSVD1[4];            // 0x0054
   volatile uint32_t SDIOCTL;             // 0x0064
   volatile uint32_t SDIOST0;             // 0x0068
   volatile uint32_t SDIOIEN;             // 0x006C
   volatile uint32_t SDIOIST;             // 0x0070
   volatile uint32_t MMCFIFOCTL;          // 0x0074
} mmcsd_regs_t;

#define MMCSD0       ((mmcsd_regs_t *)MMCSD0_REG_BASE)
#define MMCSD1       ((mmcsd_regs_t *)MMCSD1_REG_BASE)

// bitmask defines for MMCCTL.
#define PERMDX          (0x00000400)   // bit 10
#define PERMDR          (0x00000200)   // bit 9
#define CMDRST          (0x00000002)   // bit 1
#define DATRST          (0x00000001)   // bit 0
#define DATEG_SHIFT     (6)
#define WIDTH0_SHIFT    (2)

#define BUSWIDTH_1BIT   (0)
#define BUSWIDTH_4BIT   (1)

// bitmask defines for MMCST0.
#define CCS             (0x00002000)   // bit 13
#define TRNDNE          (0x00001000)   // bit 12
#define DATED           (0x00000800)   // bit 11
#define DRRDY           (0x00000400)   // bit 10
#define DXRDY           (0x00000200)   // bit 9
#define CRCRS           (0x00000080)   // bit 7
#define CRCRD           (0x00000040)   // bit 6
#define CRCWR           (0x00000020)   // bit 5
#define TOUTRS          (0x00000010)   // bit 4
#define TOUTRD          (0x00000008)   // bit 3
#define RSPDNE          (0x00000004)   // bit 2
#define BSYDNE          (0x00000002)   // bit 1
#define DATDNE          (0x00000001)   // bit 0

// bitmask defines for MMCST1.
#define MMCST1_BUSY     (0x00000001)   // bit 0

// bitmask defines for MMCCMD.
#define DMATRIG               (0x00010000)   // bit 16
#define DCLR                  (0x00008000)   // bit 15
#define INITCK                (0x00004000)   // bit 14
#define WDATX                 (0x00002000)   // bit 13
#define STRMTP                (0x00001000)   // bit 12
#define DTRW                  (0x00000800)   // bit 11
#define RSPFMT_48BIT_NOCRC    (0x00000600)   // bits 9-10
#define RSPFMT_136BIT_CRC     (0x00000400)   // bits 9-10
#define RSPFMT_48BIT_CRC      (0x00000200)   // bits 9-10
#define RSPFMT_NORSP          (0x00000000)   // bits 9-10
#define BSYEXP                (0x00000100)   // bit 8
#define PPLEN                 (0x00000080)   // bit 7

// bitmask defines for MMCCIDX.
#define CIDX_MASK       (0x0000003F)   // bits 0-5

// bitmask defines for MMCFIFOCTL.
#define FIFOLEV         (0x00000004)   // bit 2
#define FIFODIR         (0x00000002)   // bit 1
#define FIFORST         (0x00000001)   // bit 0

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t MMCSD_init(mmcsd_regs_t *mmcsd);
uint32_t MMCSD_identifyCard(mmcsd_regs_t *mmcsd, uint32_t *rca, uint32_t in_bus_width);
uint32_t MMCSD_readBlock(mmcsd_regs_t *mmcsd, uint32_t in_block, uint32_t *dest_buffer);
uint32_t MMCSD_writeBlock(mmcsd_regs_t *mmcsd, uint32_t in_block, uint32_t *src_buffer);

#endif


