//*****************************************************************************
//! @file       flash_trxeb.c
//! @brief      TrxEB specific implementation for M25PEx0 flash driver.
//!
//! Revised     $Date: 2013-06-07 14:40:10 +0200 (to, 07 jun 2013) $
//! Revision    $Revision: 7626 $
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
#ifndef FLASH_EXCLUDE


/**************************************************************************//**
* @addtogroup flash_api
* @{
******************************************************************************/


/******************************************************************************
* INCLUDES
*/
#include "bsp.h"
#include "flash_m25pex0.h"


/******************************************************************************
* DEFINES
*/
//
//! Dummy byte sent over SPI
//
#define FLASH_SPI_DUMMY         0x00

//
// IO pin register defines
//
#define FLASH_CS_N_DIR          P8DIR
#define FLASH_CS_N_SEL          P8SEL
#define FLASH_CS_N_OUT          P8OUT
#define FLASH_PWR_DIR           P7DIR
#define FLASH_PWR_SEL           P7SEL
#define FLASH_PWR_OUT           P7OUT
#define FLASH_PWR_DS            P7DS
#define FLASH_PWR_REN           P7REN
#define FLASH_RST_DIR           FLASH_PWR_DIR
#define FLASH_RST_SEL           FLASH_PWR_SEL
#define FLASH_RST_OUT           FLASH_PWR_OUT
#define FLASH_SPI_BUS_DIR       P9DIR
#define FLASH_SPI_BUS_SEL       P9SEL
#define FLASH_SPI_BUS_OUT       P9OUT
#define FLASH_SPI_BUS_REN       P9REN


/******************************************************************************
* MACROS
*/
#define FLASH_SPI_BEGIN()           (FLASH_CS_N_OUT &= ~(BSP_FLASH_CS_N))
#define FLASH_SPI_END()             (FLASH_CS_N_OUT |= BSP_FLASH_CS_N)

//
//! Clear RX flag, TX is cleared upon buffer write
//
#define FLASH_SPI_TX(x)             do{ UCB2IFG &= ~UCRXIFG; UCB2TXBUF = (x);}\
                                    while(0)
#define FLASH_SPI_RX()              UCB2RXBUF
#define FLASH_SPI_WAIT_RXRDY()      do{while(!(UCB2IFG & UCRXIFG));} while(0);

#define FLASH_RST_ENABLE()          (FLASH_RST_OUT &= ~(BSP_FLASH_RST))
#define FLASH_RST_DISABLE()         (FLASH_RST_OUT |=  (BSP_FLASH_RST))

#define FLASH_PWR_ENABLE()          (FLASH_PWR_OUT |=  (BSP_FLASH_PWR))
#define FLASH_PWR_DISABLE()         (FLASH_PWR_OUT &= ~(BSP_FLASH_PWR))


/******************************************************************************
* LOCAL VARIABLES AND FUNCTION PROTOTYPES
*/
static uint8_t flashErase(uint8_t ui8Cmd, uint32_t ui32Addr);


/******************************************************************************
* FUNCTIONS
*/
/**************************************************************************//**
* @brief    Initialize flash. This function assumes that the SPI interface to
*           the flash device has already been initialized using e.g.
*           bspIoSpiInit() or flashSpiInit().
*
* @return   Returns the true SPI clock speed, 0 on failure
******************************************************************************/
void
flashInit(void)
{
    //
    // SPI flash CS as GPIO output high
    //
    FLASH_CS_N_SEL &= ~(BSP_FLASH_CS_N);
    FLASH_CS_N_OUT |= (BSP_FLASH_CS_N);
    FLASH_CS_N_DIR |= (BSP_FLASH_CS_N);

    //
    // SPI flash reset as GPIO output low
    //
    FLASH_RST_SEL &= ~(BSP_FLASH_RST);
    FLASH_RST_OUT &= ~(BSP_FLASH_RST);
    FLASH_RST_DIR |= (BSP_FLASH_RST);

    //
    // SPI flash PWR as GPIO output high (full drivestrength)
    //
    FLASH_PWR_DS  |= (BSP_FLASH_PWR);
    FLASH_PWR_SEL &= ~(BSP_FLASH_PWR);
    FLASH_PWR_OUT |= (BSP_FLASH_PWR);
    FLASH_PWR_DIR |= (BSP_FLASH_PWR);

    //
    // Wait >10 ms (minimum time from POR to READ/WRITE instructions)
    //
    switch(bspSysClockSpeedGet())
    {
    case BSP_SYS_CLK_25MHZ:
        __delay_cycles(251000);
        break;
    case BSP_SYS_CLK_20MHZ:
        __delay_cycles(201000);
        break;
    case BSP_SYS_CLK_16MHZ:
        __delay_cycles(161000);
        break;
    case BSP_SYS_CLK_12MHZ:
        __delay_cycles(121000);
        break;
    case BSP_SYS_CLK_8MHZ:
        __delay_cycles(81000);
        break;
    case BSP_SYS_CLK_4MHZ:
        __delay_cycles(40500);
        break;
    case BSP_SYS_CLK_1MHZ:
        __delay_cycles(10250);
        break;
    default:
        bspAssert();
        break;
    }

    //
    // SPI flash reset high
    //
    FLASH_RST_OUT |= (BSP_FLASH_RST);
}


/**************************************************************************//**
* @brief    Initialize SPI interface to external flash. Calling this function
*           is quicker than flashInit() if the external flash has already been
*           initialized. The SPI clock speed will be initialized to the maximum
*           possible based on the system clock speed set by bspInit().
*
* @return   Returns the SPI clock speed set
******************************************************************************/
uint32_t
flashSpiInit(void)
{
    return (bspIoSpiInit(BSP_FLASH_SPI, bspSysClockSpeedGet()));
}


/**************************************************************************//**
* @brief    Uninitialize SPI flash interface. Assert flash reset and release
*           flash power.
*
* @return   None
******************************************************************************/
void
flashUninit(void)
{
    //
    // Deassert CSn
    //
    FLASH_SPI_END();

    //
    // Un-init SPI module and bus (MISO, MOSI, SCLK)
    //
    bspIoSpiUninit(BSP_FLASH_SPI);

    //
    // Assert reset pin before disabling power
    //
    FLASH_RST_ENABLE();
    FLASH_PWR_SEL &= ~(BSP_FLASH_PWR);
    FLASH_PWR_OUT &= ~(BSP_FLASH_PWR);
    FLASH_PWR_REN &= ~(BSP_FLASH_PWR);
}


/**************************************************************************//**
* @brief    Get SPI flash status byte.
*
* @return   Returns SPI flash status byte
******************************************************************************/
uint8_t
flashStatusGet(void)
{
    uint8_t ui8Status;

    //
    // Assert CSn
    //
    FLASH_SPI_BEGIN();

    //
    // Send command byte, wait, send dummy byte, wait, read returned status.
    //
    FLASH_SPI_TX(FLASH_INSTR_RDSR);
    FLASH_SPI_WAIT_RXRDY();
    FLASH_SPI_TX(FLASH_SPI_DUMMY);
    FLASH_SPI_WAIT_RXRDY();
    ui8Status = FLASH_SPI_RX();

    //
    // Deassert CSn and return status.
    //
    FLASH_SPI_END();
    return (ui8Status);
}


/**************************************************************************//**
* @brief    Get SPI flash ID bytes.
*
* @return   Returns SPI flash ID bytes (3 bytes). Bits [7:0] is the memory
*           capacity, bits [8:15] is the memory type and [16:23] is the
*           manufacturer ID.
******************************************************************************/
uint32_t
flashIdGet(void)
{
    uint8_t ui8ManId, ui8Type, ui8Capacity;
    uint32_t ui32Id;

    //
    // Assert CSn
    //
    FLASH_SPI_BEGIN();

    //
    // Send read instruction and wait for RX flag to go high
    //
    FLASH_SPI_TX(FLASH_INSTR_RDID);
    FLASH_SPI_WAIT_RXRDY();

    //
    // Send dummy, wait, and read manufacturer id (0x20 = Numonyx)
    //
    FLASH_SPI_TX(FLASH_SPI_DUMMY);
    FLASH_SPI_WAIT_RXRDY();
    ui8ManId = FLASH_SPI_RX();

    //
    // Send dummy, wait, and read type
    //
    FLASH_SPI_TX(FLASH_SPI_DUMMY);
    FLASH_SPI_WAIT_RXRDY();
    ui8Type = FLASH_SPI_RX();

    //
    // Send dummy, wait, and read capacity (0x12 = M25PE20)
    //
    FLASH_SPI_TX(FLASH_SPI_DUMMY);
    FLASH_SPI_WAIT_RXRDY();
    ui8Capacity = FLASH_SPI_RX();        // Capacity (0x12 = M25PE20)

    //
    // Deassert CSn
    //
    FLASH_SPI_END();

    //
    // Concatenate the three values and return id
    //
    ui32Id  = ((uint32_t)ui8ManId) << 16;
    ui32Id |= ((uint32_t)ui8Type)  << 8;
    ui32Id |= ((uint32_t)ui8Capacity);
    return (ui32Id);
}


/**************************************************************************//**
* @brief    Read bytes from SPI flash. Data are transferred using the CPU and
*           active state waiting.
*
* @param    ui32Addr      SPI flash start address
* @param    pui8Data     Pointer to buffer to put read bytes
* @param    ui32Bytes     Number of bytes to read
*
* @return   Returns number of bytes read
******************************************************************************/
uint32_t
flashRead(uint32_t ui32Addr, uint8_t *pui8Data, uint32_t ui32Bytes)
{
    uint32_t ui32Cnt;

    //
    // Assert CSn
    //
    FLASH_SPI_BEGIN();

    //
    // Send READ instruction
    //
    FLASH_SPI_TX(FLASH_INSTR_READ);
    FLASH_SPI_WAIT_RXRDY();

    //
    // Send sddress
    //
    FLASH_SPI_TX((uint8_t)(ui32Addr >> 16));
    FLASH_SPI_WAIT_RXRDY();
    FLASH_SPI_TX((uint8_t)(ui32Addr >> 8));
    FLASH_SPI_WAIT_RXRDY();
    FLASH_SPI_TX((uint8_t)(ui32Addr));
    FLASH_SPI_WAIT_RXRDY();

    //
    // Read data
    //
    for(ui32Cnt = 0; ui32Cnt < ui32Bytes; ui32Cnt++)
    {
        FLASH_SPI_TX(FLASH_SPI_DUMMY);
        FLASH_SPI_WAIT_RXRDY();
        pui8Data[ui32Cnt] = FLASH_SPI_RX();
    }

    //
    // Deassert CSn
    //
    FLASH_SPI_END();

    //
    // Return number of bytes read
    //
    return (ui32Cnt);
}


/**************************************************************************//**
* @brief    Write bytes to SPI flash page.
*
* @param    ui16Page      SPI flash page to write to [0-1023]
* @param    pui8Data     Pointer to buffer with data
* @param    ui16Bytes     Number of bytes to write [1-256]
*
* @return   Returns number of bytes written to the external flash
******************************************************************************/
uint32_t
flashPageWrite(uint16_t ui16Page, uint8_t *pui8Data, uint16_t ui16Bytes)
{
    uint32_t ui32Cnt;
    uint8_t ui8Status;

    if((ui16Bytes == 0) || (ui16Bytes > 256))
    {
        return (0);
    }

    //
    // Assert CSn and send enable write command
    //
    FLASH_SPI_BEGIN();
    FLASH_SPI_TX(FLASH_INSTR_WREN);
    FLASH_SPI_WAIT_RXRDY();
    FLASH_SPI_END();

    //
    // Start write sequence
    //
    FLASH_SPI_BEGIN();
    FLASH_SPI_TX(FLASH_INSTR_PW);
    FLASH_SPI_WAIT_RXRDY();

    //
    // Address
    //
    FLASH_SPI_TX((uint8_t)(ui16Page >> 8));
    FLASH_SPI_WAIT_RXRDY();
    FLASH_SPI_TX((uint8_t)(ui16Page));
    FLASH_SPI_WAIT_RXRDY();
    FLASH_SPI_TX((uint8_t)(0x00));
    FLASH_SPI_WAIT_RXRDY();

    //
    // Transfer the data
    //
    for(ui32Cnt = 0; ui32Cnt < ui16Bytes; ui32Cnt++)
    {
        FLASH_SPI_TX(pui8Data[ui32Cnt]);
        FLASH_SPI_WAIT_RXRDY();
    }

    //
    // Deassert CSn
    //
    FLASH_SPI_END();

    //
    // Assert CSn and wait for write to finish
    //
    FLASH_SPI_BEGIN();
    FLASH_SPI_TX(FLASH_INSTR_RDSR);
    FLASH_SPI_WAIT_RXRDY();
    do
    {
        FLASH_SPI_TX(FLASH_SPI_DUMMY);
        FLASH_SPI_WAIT_RXRDY();
        ui8Status = FLASH_SPI_RX();
    }
    while(ui8Status & FLASH_STATUS_WIP_BM);

    //
    // Deassert CSn and return number of bytes written
    //
    FLASH_SPI_END();
    return (ui32Cnt);
}


/**************************************************************************//**
* @brief    Function erases the flash page specified by \e ui8Page. The function
*           does not return until the erase process has completed, or the
*           process times out. The timeout time is ~20 ms.
*
* @param    ui16Page          Page to erase [0-1023]
*
* @return   Returns 0 on success
* @return   Returns 1 on timeout
******************************************************************************/
uint8_t
flashPageErase(uint16_t ui16Page)
{
    return (flashErase(FLASH_INSTR_PE, FLASH_PAGE_TO_ADDR(ui16Page)));
}


/**************************************************************************//**
* @brief    Function erases the sub-sector specified by \e ui8SubSector. It is
*           assumed that the specified sub-sector is not write protected. The
*           function does not return until the erase process has completed, or
*           the process times out. The timeout time is ~150 ms. A subsector
*           consists of 4096 bytes (16 pages).
*
* @param    ui8SubSector     Sub-sector to erase [0-63]
*
* @return   Returns 0 on success
* @return   Returns 1 on timeout
******************************************************************************/
uint8_t
flashSubSectorErase(uint8_t ui8SubSector)
{
    return (flashErase(FLASH_INSTR_SSE, FLASH_SUBSECTOR_TO_ADDR(ui8SubSector)));
}


/**************************************************************************//**
* @brief    Function erases the sector specified by \e ui8Sector. It is assumed
*           that the specified sector is not write protected (check BP0, BP1
*           bits in the flash device status register). The function does not
*           return until the erase process has completed, or the process times
*           out. The timeout time is ~5 s. A subsector consists of 65536 bytes
*           (256 pages).
*
* @param    ui8Sector        Sector to erase [0-3]
*
* @return   Returns 0 on success
* @return   Returns 1 on timeout
******************************************************************************/
uint8_t
flashSectorErase(uint8_t ui8Sector)
{
    return (flashErase(FLASH_INSTR_SE, FLASH_SECTOR_TO_ADDR(ui8Sector)));
}


/**************************************************************************//**
* @brief    Function erases the entire flash. It is assumed that the flash
*           device is not write protected. The function does not return
*           until the erase process has completed, or the process times out.
*           The timeout time is 10 seconds.
*
* @return   Returns 0 on success
* @return   Returns 1 on timeout
******************************************************************************/
uint8_t
flashBulkErase(void)
{
    return (flashErase(FLASH_INSTR_BE, 0));
}


/**************************************************************************//**
* @brief    Puts flash device into deep power-down mode. The flash device
*           must be released from deep power-down using flashPowerDownDisable()
*           before it accepts new instructions.
*
* @return   None
******************************************************************************/
void
flashDeepPowerDownEnable(void)
{
    //
    // Assert CSn, enter deep power-down, wait and deassert CSn
    //
    FLASH_SPI_BEGIN();
    FLASH_SPI_TX(FLASH_INSTR_DP);
    FLASH_SPI_WAIT_RXRDY();
    FLASH_SPI_END();
}


/**************************************************************************//**
* @brief    Releases flash device from deep power-down mode and back to normal
*           standby mode.
*
* @see      flashDeepPowerDownEnable()
*
* @return   None
******************************************************************************/
void
flashDeepPowerDownDisable(void)
{
    //
    // Assert CSn, release from deep power-down, wait and deassert CSn
    //
    FLASH_SPI_BEGIN();
    FLASH_SPI_TX(FLASH_INSTR_RDP); // Release from Deep Power-Down
    FLASH_SPI_WAIT_RXRDY();
    FLASH_SPI_END();
}


/******************************************************************************
* LOCAL FUNCTIONS
*/
static uint8_t
flashErase(uint8_t ui8Cmd, uint32_t ui32Addr)
{
    volatile uint_fast8_t ui8Status;
    uint_fast8_t ui8Cycles = 0;
    uint_fast8_t ui8Slow = 0;
    uint32_t ui32CyclesMax;

    //
    // Calculate timeout based on instruction
    //
    switch(ui8Cmd)
    {
    case FLASH_INSTR_PE:
        ui32CyclesMax = 4;
        break;
    case FLASH_INSTR_SSE:
        ui32CyclesMax = 30;
        break;
    case FLASH_INSTR_SE:
        ui32CyclesMax = 50;
        ui8Slow = 1;
        break;
    case FLASH_INSTR_BE:
        ui32CyclesMax = 100;
        ui8Slow = 1;
        break;
    }
    ui32CyclesMax *= bspSysClockSpeedGet();

    //
    // Set Write Enable bit
    //
    FLASH_SPI_BEGIN();
    FLASH_SPI_TX(FLASH_INSTR_WREN);
    FLASH_SPI_WAIT_RXRDY();
    FLASH_SPI_END();

    //
    // Send command
    //
    FLASH_SPI_BEGIN();
    FLASH_SPI_TX(ui8Cmd);
    FLASH_SPI_WAIT_RXRDY();
    if(ui8Cmd != FLASH_INSTR_BE)
    {
        //
        // Send address bytes
        //
        FLASH_SPI_TX((uint8_t)(ui32Addr >> 16));
        FLASH_SPI_WAIT_RXRDY();
        FLASH_SPI_TX((uint8_t)(ui32Addr >> 8));
        FLASH_SPI_WAIT_RXRDY();
        FLASH_SPI_TX((uint8_t)(0x00));
        FLASH_SPI_WAIT_RXRDY();
    }
    FLASH_SPI_END();

    //
    // Poll status for command to complete
    //
    FLASH_SPI_BEGIN();
    FLASH_SPI_TX(FLASH_INSTR_RDSR);
    FLASH_SPI_WAIT_RXRDY();
    do
    {
        FLASH_SPI_TX(FLASH_SPI_DUMMY);
        FLASH_SPI_WAIT_RXRDY();
        ui8Status = FLASH_SPI_RX();
        if(ui8Slow)
        {
            __delay_cycles(95000);
        }
        __delay_cycles(5000);
        ui8Cycles++;
    }
    while((ui8Status & FLASH_STATUS_WIP_BM) && (ui8Cycles < ui32CyclesMax));
    FLASH_SPI_END();

    //
    // Return status (success : fail)
    //
    return (ui8Cycles < ui32CyclesMax) ? 0 : 1;
}


/**************************************************************************//**
* Close the Doxygen group.
* @}
******************************************************************************/
#endif // #ifndef FLASH_EXCLUDE
