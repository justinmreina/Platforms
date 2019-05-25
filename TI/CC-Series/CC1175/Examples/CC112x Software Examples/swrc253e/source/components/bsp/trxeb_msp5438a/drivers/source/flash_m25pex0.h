//*****************************************************************************
//! @file       flash_m25pe20.h
//! @brief      Device driver header file for M25PEx0 SPI flash (x=1,2).
//!             This header file contains defines for functionality that
//!             is only available in the T9HX process.
//!
//!             The M25PEx0 SPI flash defines the following sizes:
//!             +-------------+---------+-------+-------+
//!             | Name        | Pages   | Bytes | Kbit  |
//!             +-------------+---------+-------+-------+
//!             | Page        |   1     |  256  |   2   |
//!             | Sub-sector  |  16     | 4069  |  32   |
//!             | Sector      | 256     |65536  | 512   |
//!             +-------------+---------+-------+-------+
//!
//!             Datasheet:
//!
//! Revised     $Date: 2013-05-18 16:26:35 +0200 (fr, 18 mai 2013) $
//! Revision    $Revision: 7595 $
//
//  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//****************************************************************************/
#ifndef __FLASH_M25PEX0_H__
#define __FLASH_M25PEX0_H__


/******************************************************************************
* If building with a C++ compiler, make all of the definitions in this header
* have a C binding.
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************
* INCLUDES
*/


/******************************************************************************
* MACROS
*/
#define FLASH_ADDR_TO_PAGE(addr)        ((uint16_t)(addr >> 8))
#define FLASH_PAGE_TO_ADDR(page)        ((((uint32_t)page) << 8))

#define FLASH_ADDR_TO_SUBSECTOR(addr)   ((uint8_t)(addr >> 12))
#define FLASH_SUBSECTOR_TO_ADDR(sect)   ((((uint32_t)sect) << 12))

#define FLASH_ADDR_TO_SECTOR(addr)      ((uint8_t)(addr >> 16))
#define FLASH_SECTOR_TO_ADDR(sect)      ((((uint32_t)sect) << 16))

#define FLASH_PAGE_TO_SUBSECTOR(sect)   ((uint16_t)(sect >> 4))
#define FLASH_PAGE_TO_SECTOR(sect)      ((uint16_t)(sect >> 8))

/******************************************************************************
* DEFINES
*/
// Instructions
#define FLASH_INSTR_WREN                0x06    //!< Write Enable
#define FLASH_INSTR_WRDI                0x04    //!< Write Disable
#define FLASH_INSTR_RDID                0x9F    //!< Read Identification
#define FLASH_INSTR_RDSR                0x05    //!< Read Status Register
#define FLASH_INSTR_WRLR                0xE5    //!< Write Lock Register
#define FLASH_INSTR_WRSR                0x01    //!< Write Status Register
#define FLASH_INSTR_RDLR                0xE8    //!< Read Lock Register
#define FLASH_INSTR_READ                0x03    //!< Read Data Bytes
#define FLASH_INSTR_FAST_READ           0x0B    //!< Read Data Bytes (hi speed)
#define FLASH_INSTR_PW                  0x0A    //!< Page Write
#define FLASH_INSTR_PP                  0x02    //!< Page Program
#define FLASH_INSTR_PE                  0xDB    //!< Page Erase
#define FLASH_INSTR_SSE                 0x20    //!< SubSector Erase
#define FLASH_INSTR_SE                  0xD8    //!< Sector Erase
#define FLASH_INSTR_BE                  0xC7    //!< Bulk Erase
#define FLASH_INSTR_DP                  0xB9    //!< Deep Power-down
#define FLASH_INSTR_RDP                 0xAB    //!< Release from Deep Power-down

// Bitmasks of the status register
#define FLASH_STATUS_SRWD_BM            0x80
#define FLASH_STATUS_BP_BM              0x0C
#define FLASH_STATUS_WEL_BM             0x02    //!< Write Enable bitmask
#define FLASH_STATUS_WIP_BM             0x01    //!< Write in Progress bitmask

// Flash sizes
#define FLASH_PAGE_SIZE                 256     //!< Number of bytes in a page
#define FLASH_SUBSECTOR_SIZE            (16 * FLASH_PAGE_SIZE)
#define FLASH_SECTOR_SIZE               (256* FLASH_PAGE_SIZE)

// Sector defines
#define FLASH_SECTOR_0                  0
#define FLASH_SECTOR_1                  1
#define FLASH_SECTOR_2                  2
#define FLASH_SECTOR_3                  3


/******************************************************************************
* FUNCTION PROTOTYPES
*/
void flashInit(void);
void flashUninit(void);
uint32_t flashSpiInit(void);
uint8_t flashStatusGet(void);
uint32_t flashIdGet(void);

uint32_t flashRead(uint32_t ui32Addr, uint8_t *pui8Data,
                        uint32_t ui32Bytes);
uint32_t flashPageWrite(uint16_t ui16Page, uint8_t *pui8Data,
                             uint16_t ui16Bytes);

uint8_t flashPageErase(uint16_t ui16Page);
uint8_t flashSubSectorErase(uint8_t ui16SubSector);
uint8_t flashSectorErase(uint8_t ui8Sector);
uint8_t flashBulkErase(void);

void flashDeepPowerDownEnable(void);
void flashDeepPowerDownDisable(void);


/******************************************************************************
* Mark the end of the C bindings section for C++ compilers.
******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif /* #ifndef __FLASH_M25PEX0_H__ */
