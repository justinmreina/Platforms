//-----------------------------------------------------------------------------
// \file    evmomapl138_i2c.h
// \brief   OMAP-L138 i2c registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_I2C_H
#define EVMOMAPL138_I2C_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
   volatile uint32_t ICOAR;      // 0x0000
   volatile uint32_t ICIMR;      // 0x0004
   volatile uint32_t ICSTR;      // 0x0008
   volatile uint32_t ICCLKL;     // 0x000C
   volatile uint32_t ICCLKH;     // 0x0010
   volatile uint32_t ICCNT;      // 0x0014
   volatile uint32_t ICDRR;      // 0x0018
   volatile uint32_t ICSAR;      // 0x001C
   volatile uint32_t ICDXR;      // 0x0020
   volatile uint32_t ICMDR;      // 0x0024
   volatile uint32_t ICIVR;      // 0x0028
   volatile uint32_t ICEMDR;     // 0x002C
   volatile uint32_t ICPSC;      // 0x0030
   volatile uint32_t REVID1;     // 0x0034
   volatile uint32_t REVID2;     // 0x0038
   volatile uint32_t RSVD0[3];   // 0x003C
   volatile uint32_t ICPFUNC;    // 0x0048
   volatile uint32_t ICPDIR;     // 0x004C
   volatile uint32_t ICPDIN;     // 0x0050
   volatile uint32_t ICPDOUT;    // 0x0054
   volatile uint32_t ICPDSET;    // 0x0058
   volatile uint32_t ICPDCLR;    // 0x005C
} i2c_regs_t;

#define I2C0         ((i2c_regs_t *)I2C0_REG_BASE)
#define I2C1         ((i2c_regs_t *)I2C1_REG_BASE)

// defines for ICSTR.
#define BB           (0x00001000)   // bit 12
#define ICXRDY       (0x00000010)   // bit 4
#define ICRRDY       (0x00000008)   // bit 3

// defines for ICMDR.
#define NACKMOD      (0x00008000)   // bit 15
#define ICMDR_FREE   (0x00004000)   // bit 14
#define STT          (0x00002000)   // bit 13
#define STP          (0x00000800)   // bit 11
#define MST          (0x00000400)   // bit 10
#define TRX          (0x00000200)   // bit 9
#define XA           (0x00000100)   // bit 8
#define RM           (0x00000080)   // bit 7
#define DLB          (0x00000040)   // bit 6
#define IRS          (0x00000020)   // bit 5
#define ICMDR_STB    (0x00000010)   // bit 4
#define FDF          (0x00000008)   // bit 3
#define MASK_BC      (0x00000007)   // bit 0-2

// defines to pass whether we should set stop bit after a write
// and check busy bit before a read.
#define CHECK_BUSY_BEFORE_READ      (1)
#define SKIP_BUSY_BIT_CHECK         (0)
#define SET_STOP_BIT_AFTER_WRITE    (1)
#define SKIP_STOP_BIT_AFTER_WRITE   (0)

// i2c addresses for OMAP-L138 EVM.
#define I2C_ADDR_AIC3106      (0x0018)
#define I2C_ADDR_GPIO_UI      (0x0020)
#define I2C_ADDR_GPIO_EX      (0x0021)
#define I2C_ADDR_PMIC         (0x0048)
#define I2C_ADDR_SVIDEODEC    (0x005C)
#define I2C_ADDR_CVIDEODEC    (0x005D)
#define I2C_ADDR_CDCE913      (0x0065)
#define I2C_ADDR_CVIDEOENC    (0x002A)

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------
typedef enum
{
   I2C_CLK_100K = 0,
   I2C_CLK_400K
} i2c_clk_e;

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t I2C_init(i2c_regs_t *i2c, i2c_clk_e in_clock_rate);
uint32_t I2C_read(i2c_regs_t *i2c, uint16_t in_addr, uint8_t *dest_buffer, uint16_t in_length, uint8_t chk_busy);
uint32_t I2C_write(i2c_regs_t *i2c, uint16_t in_addr, uint8_t *src_buffer, uint16_t in_length, uint8_t set_stop);

#endif

