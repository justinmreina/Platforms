//-----------------------------------------------------------------------------
// \file    spiflash_numonyx.h
// \brief   OMAP-L138 spi flash numonyx function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef SPIFLASH_NUMONYX_H
#define SPIFLASH_NUMONYX_H

//-----------------------------------------------------------------------------
// Public Defines and Macros
//-----------------------------------------------------------------------------
// numonyx spi flash sizes.
#define NUMONYX_M25P64_SIZE         (8388608)

// numonyx memory org defines.
#define SPIFLASH_PAGE_SIZE          (256)
#define SPIFLASH_SECTOR_SIZE        (65536)
#define SPIFLASH_CHIP_SIZE          (NUMONYX_M25P64_SIZE)
#define SPIFLASH_NUM_PAGES          (SPIFLASH_CHIP_SIZE / SPIFLASH_PAGE_SIZE)
#define SPIFLASH_NUM_SECTORS        (SPIFLASH_CHIP_SIZE / SPIFLASH_SECTOR_SIZE)

// max timeouts for various commands...in microseconds.
#define TIMEOUT_POST_PAGE_PROG      (5000)      // 5 ms
#define TIMEOUT_POST_SEC_ERASE      (3000000)   // 3 s
#define TIMEOUT_POST_CHIP_ERASE     (160000000) // 160 s
// max timeout for a cmd that is being used...need to change if sector or
// chip erase commands are implemented.
#define TIMEOUT_MAX                 (TIMEOUT_POST_SEC_ERASE)

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t NUMONYX_initSpiFlashPort(void);
uint32_t NUMONYX_verifyDeviceID(void);
uint32_t NUMONYX_waitFlashReady(uint32_t in_us_timeout);
uint32_t NUMONYX_sendCmdFastRead(uint32_t in_addr);
uint32_t NUMONYX_sendCmdWriteEnable(void);
uint32_t NUMONYX_sendCmdPageProgram(uint32_t in_addr);
uint32_t NUMONYX_sendCmdSectorErase(uint32_t in_addr);

#endif

