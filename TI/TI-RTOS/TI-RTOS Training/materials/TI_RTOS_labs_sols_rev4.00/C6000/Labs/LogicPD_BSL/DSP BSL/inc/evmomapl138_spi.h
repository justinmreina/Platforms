//-----------------------------------------------------------------------------
// \file    evmomapl138_spi.h
// \brief   OMAP-L138 spi registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_SPI_H
#define EVMOMAPL138_SPI_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
  volatile uint32_t SPIGCR0;        // 0x0000
  volatile uint32_t SPIGCR1;        // 0x0004
  volatile uint32_t SPIINT;         // 0x0008
  volatile uint32_t SPILVL;         // 0x000C
  volatile uint32_t SPIFLG;         // 0x0010
  volatile uint32_t SPIPC0;         // 0x0014
  volatile uint32_t SPIPC1;         // 0x0018
  volatile uint32_t SPIPC2;         // 0x001C
  volatile uint32_t SPIPC3;         // 0x0020
  volatile uint32_t SPIPC4;         // 0x0024
  volatile uint32_t SPIPC5;         // 0x0028
  volatile uint32_t RSVD0[3];       // 0x002C
  volatile uint32_t SPIDAT0;        // 0x0038
  volatile uint32_t SPIDAT1;        // 0x003C
  volatile uint32_t SPIBUF;         // 0x0040
  volatile uint32_t SPIEMU;         // 0x0044
  volatile uint32_t SPIDELAY;       // 0x0048
  volatile uint32_t SPIDEF;         // 0x004C
  volatile uint32_t SPIFMT0;        // 0x0050
  volatile uint32_t SPIFMT1;        // 0x0054
  volatile uint32_t SPIFMT2;        // 0x0058
  volatile uint32_t SPIFMT3;        // 0x005C
  volatile uint32_t INTVEC0;        // 0x0060
  volatile uint32_t INTVEC1;        // 0x0064
} spi_regs_t;

#define SPI0         ((spi_regs_t *)SPI0_REG_BASE)
#define SPI1         ((spi_regs_t *)SPI1_REG_BASE)

// defines for SPIGCR0.
#define RESET        (0x00000001)   // bit 0

// defines for SPIGCR1.
#define ENABLE       (0x01000000)   // bit 24
#define LOOPBACK     (0x00010000)   // bit 16
#define POWERDOWN    (0x00000100)   // bit 8
#define CLKMOD       (0x00000002)   // bit 1
#define MASTER       (0x00000001)   // bit 0

// defines for SPIPC0, 1, 2, 3, 4, 5.
#define SOMI         (0x00000800)   // bit 11
#define SIMO         (0x00000400)   // bit 10
#define CLK          (0x00000200)   // bit 9
#define ENA          (0x00000100)   // bit 8
#define SCS0         (0x00000001)   // bit 0

// defines for SPIDAT1.
#define CSHOLD       (0x10000000)   // bit 28
#define CSNR         (0x00010000)   // bit 16
#define MASK_TXDATA  (0x0000FFFF)

// defines for SPIBUF.
#define RXEMPTY      (0x80000000)   // bit 31
#define TXFULL       (0x20000000)   // bit 29

// defines for SPIDEF.
#define CSDEF0       (0x00000001)   // bit 0

// defines for SPIFMT0.
#define SHIFTDIR           (0x00100000)   // bit 20
#define POLARITY           (0x00020000)   // bit 17
#define PHASE              (0x00010000)   // bit 16
#define SHIFT_PRESCALE     (8)   // bit 8
#define SHIFT_CHARLEN      (0)   // bit 0

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------

typedef enum
{
   SPI_MODE_SLAVE = 0,
   SPI_MODE_MASTER
} spi_mode_e;

typedef enum
{
   SPI_3PIN = 0,
   SPI_4PIN_CS,
   SPI_4PIN_EN,
   SPI_5PIN
} spi_pin_option_e;

typedef enum
{
   SPI_CS_ACTIVE_LOW = 0,
   SPI_CS_ACTIVE_HIGH
} spi_cs_active_e;

typedef enum
{
   SPI_SHIFT_MSB = 0,
   SPI_SHIFT_LSB
} spi_shift_dir_e;

typedef enum
{
   SPI_HOLD_NONE = 0,
   SPI_HOLD_ACTIVE,
   SPI_HOLD_CLR
} spi_cs_hold_e;

typedef struct
{
   spi_mode_e mode;
   spi_pin_option_e pin_option;
   spi_cs_active_e cs_active;
   spi_shift_dir_e shift_dir;
   uint8_t polarity;
   uint8_t phase;
   uint32_t freq;
} spi_config_t;

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t SPI_init(spi_regs_t *spi, spi_config_t *in_config);
uint32_t SPI_xfer(spi_regs_t *spi, uint8_t *src_buffer, uint8_t *dest_buffer, uint32_t in_length, spi_cs_hold_e in_cs_hold);

#endif

