//-----------------------------------------------------------------------------
// \file    evmomapl138_spiflash.h
// \brief   OMAP-L138 spi flash defines and function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_SPI_FLASH_H
#define EVMOMAPL138_SPI_FLASH_H

// include flash chip specific defines.
#include "spiflash_numonyx.h"

//-----------------------------------------------------------------------------
// Public Defines and Macros
//-----------------------------------------------------------------------------
#define SPIFLASH_SPI          (SPI1)

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t SPIFLASH_init(void);
uint32_t SPIFLASH_read(uint32_t in_addr, uint8_t *dest_buffer, uint32_t in_length);
uint32_t SPIFLASH_write(uint32_t in_addr, uint8_t *in_data, uint32_t in_length, uint8_t wait_for_completion);
uint32_t SPIFLASH_erase(uint32_t in_addr, uint32_t in_length, uint8_t wait_for_completion);

uint32_t SPIFLASH_readMACAddr(uint8_t *mac_addr);
uint32_t SPIFLASH_writeMACAddr(uint8_t *mac_addr);

#endif

