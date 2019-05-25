//-----------------------------------------------------------------------------
// \file    evmomapl138_spiflash.c
// \brief   implementation of a spi flash driver for OMAP-L138 EVM. Implements
//          functions to init, read, and write SPI flash.
//
//-----------------------------------------------------------------------------

#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_spi.h"
#include "spiflash_numonyx.h"
#include "evmomapl138_spiflash.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
// place the ethernet MAC address at the beginning of the last sector.
#define MACADDR_ADDR          (SPIFLASH_CHIP_SIZE - SPIFLASH_SECTOR_SIZE)
#define MACADDR_NUM_BYTES     (6)
// max writable address...do not allow user to overwrite MAC address!
#define MAX_WRITE_ADDR        (MACADDR_ADDR - 1)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static uint32_t localWrite(uint32_t in_addr, uint8_t *in_data, uint32_t in_length, uint8_t wait_for_completion);
static uint32_t localErase(uint32_t in_addr, uint32_t in_length, uint8_t wait_for_completion);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the spi flash.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...spi flash ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t SPIFLASH_init(void)
{
   uint32_t rtn = ERR_NO_ERROR;

   EVMOMAPL138_pinmuxConfig(PINMUX_GPIO_BUFF_OE_REG, PINMUX_GPIO_BUFF_OE_MASK, PINMUX_GPIO_BUFF_OE_VAL);
   GPIO_setDir(GPIO_BUFF_OE_BANK, GPIO_BUFF_OE_PIN, GPIO_OUTPUT);
   GPIO_setOutput(GPIO_BUFF_OE_BANK, GPIO_BUFF_OE_PIN, OUTPUT_LOW);
   
   rtn = NUMONYX_initSpiFlashPort();
   if (rtn != ERR_NO_ERROR)
   {
      #ifdef DEBUG
      printf("spi flash, spi init error:\t%d\r\n", rtn);
      #endif
      return (ERR_INIT_FAIL);
   }
   
   // make sure the spi flash is not busy.
   rtn = NUMONYX_waitFlashReady(TIMEOUT_MAX);
   if (rtn != ERR_NO_ERROR)
   {
      #ifdef DEBUG
      printf("spi flash init wait flash error:\t%d\r\n", rtn);
      #endif
      return (ERR_INIT_FAIL);
   }
   
   rtn = NUMONYX_verifyDeviceID();
   if (rtn != ERR_NO_ERROR)
   {
      #ifdef DEBUG
      printf("spi flash init verify id error:\t%d\r\n", rtn);
      #endif
      return (ERR_INIT_FAIL);
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   read data from spi flash.
//
// \param   uint32_t in_addr - flash address to read from.
//
// \param   uint8_t *dest_buffer - pointer to memory where data is to be copied.
//
// \param   uint32_t in_length - number of bytes to read.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, data returned in dest_buffer.
//    ERR_INVALID_PARAMETER - null pointer or invalid address.
//-----------------------------------------------------------------------------
uint32_t SPIFLASH_read(uint32_t in_addr, uint8_t *dest_buffer, uint32_t in_length)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;
   
   // verify the input parameters.
   if (((in_addr + in_length) < (SPIFLASH_CHIP_SIZE - 1)) &&
         (dest_buffer != NULL))
   {
      // make sure the spi flash is not busy.
      rtn = NUMONYX_waitFlashReady(TIMEOUT_MAX);
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }
      
      // send the fast read cmd and addr.
      rtn = NUMONYX_sendCmdFastRead(in_addr);
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }
      
      // clock out the data.
      rtn = SPI_xfer(SPIFLASH_SPI, NULL, dest_buffer, in_length, SPI_HOLD_CLR);
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   write data to spi flash.
//
// \param   uint32_t in_addr - flash address to read from.
//
// \param   uint8_t *in_data - pointer to data to write.
//
// \param   uint32_t in_length - number of bytes to write.
//
// \param   uint8_t wait_for_completion - should we wait for the write to
//             complete before returning.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, data written.
//    ERR_INVALID_PARAMETER - null pointer or invalid address.
//-----------------------------------------------------------------------------
uint32_t SPIFLASH_write(uint32_t in_addr, uint8_t *in_data, uint32_t in_length, uint8_t wait_for_completion)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   // verify the input parameters.
   if (((in_addr + in_length) < MAX_WRITE_ADDR) &&
         (in_data != NULL))
   {
      rtn = localWrite(in_addr, in_data, in_length, wait_for_completion);
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   erase the spi flash sector that contains the given address through
//          the sector that contains address + length.
//
// \param   uint32_t in_addr - address to erase.
//
// \param   uint32_t in_length - number of bytes to erase.
//
// \param   uint8_t wait_for_completion - should we wait for the write to
//             complete before returning.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, data written.
//    ERR_INVALID_PARAMETER - invalid address.
//-----------------------------------------------------------------------------
uint32_t SPIFLASH_erase(uint32_t in_addr, uint32_t in_length, uint8_t wait_for_completion)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   // verify the input parameters.
   if ((in_addr + in_length) < MAX_WRITE_ADDR)
   {
      rtn = localErase(in_addr, in_length, wait_for_completion);
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   read mac address from spi flash.
//
// \param   uint8_t *mac_addr - pointer to memory where mac address is to be copied.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, data returned in mac_addr.
//    ERR_INVALID_PARAMETER - null pointer or invalid address.
//-----------------------------------------------------------------------------
uint32_t SPIFLASH_readMACAddr(uint8_t *mac_addr)
{
   return (SPIFLASH_read(MACADDR_ADDR, mac_addr, MACADDR_NUM_BYTES));
}

//-----------------------------------------------------------------------------
// \brief   write mac address to first 6 bytes of last sector in spi flash.
//
// \param   uint8_t *mac_addr - pointer to address to be written.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, data returned in mac_addr.
//    ERR_INVALID_PARAMETER - null pointer or invalid address.
//-----------------------------------------------------------------------------
uint32_t SPIFLASH_writeMACAddr(uint8_t *mac_addr)
{
   uint32_t rtn;

   rtn = localErase(MACADDR_ADDR, MACADDR_NUM_BYTES, 1);
   
   if (rtn != ERR_NO_ERROR)
   {
      return (rtn);
   }
   
   return (localWrite(MACADDR_ADDR, mac_addr, MACADDR_NUM_BYTES, 1));
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// performs the actual write to spi flash...no bounds checking.
//-----------------------------------------------------------------------------
uint32_t localWrite(uint32_t in_addr, uint8_t *in_data, uint32_t in_length, uint8_t wait_for_completion)
{
   uint32_t rtn;
   uint8_t first_write = 1;
   uint32_t bytes_to_page_end;
   uint32_t bytes_remaining = in_length;
   uint32_t data_idx = 0;
   uint32_t write_addr;
   uint32_t write_length;

   // compute vars for first write.
   write_addr = in_addr;
   bytes_to_page_end = SPIFLASH_PAGE_SIZE - (in_addr % SPIFLASH_PAGE_SIZE);
   if (in_length > bytes_to_page_end)
   {
      write_length = bytes_to_page_end;
   }
   else
   {
      write_length = in_length;
   }

   // make sure the spi flash is not busy.
   rtn = NUMONYX_waitFlashReady(TIMEOUT_MAX);
   if (rtn != ERR_NO_ERROR)
   {
      return (rtn);
   }

   // write pages until we go past the address + length.
   while (bytes_remaining > 0)
   {
      if (!first_write)
      {
         // wait for the last write to complete.
         rtn = NUMONYX_waitFlashReady(TIMEOUT_POST_PAGE_PROG);
         if (rtn != ERR_NO_ERROR)
         {
            return (rtn);
         }

         // compute the write length for the current write.
         if (bytes_remaining > SPIFLASH_PAGE_SIZE)
         {
            write_length = SPIFLASH_PAGE_SIZE;
         }
         else
         {
            write_length = bytes_remaining;
         }
      }

      // send the write enable and sector erase commands.
      rtn = NUMONYX_sendCmdWriteEnable();
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }

      rtn = NUMONYX_sendCmdPageProgram(write_addr);
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }

      // clock out the data.
      rtn = SPI_xfer(SPIFLASH_SPI, in_data + data_idx, NULL, write_length, SPI_HOLD_CLR);
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }

      // setup variables for next loop.
      first_write = 0;
      write_addr += write_length;
      data_idx += write_length;
      bytes_remaining -= write_length;
   }

   // wait for the final write to complete before returning (if desired).
   if (wait_for_completion)
   {
      rtn = NUMONYX_waitFlashReady(TIMEOUT_POST_PAGE_PROG);
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }
   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// performs the actual spi flash erase...no bounds checking.
//-----------------------------------------------------------------------------
uint32_t localErase(uint32_t in_addr, uint32_t in_length, uint8_t wait_for_completion)
{
   uint32_t rtn;
   uint32_t erase_addr = in_addr;

   // make sure the spi flash is not busy.
   rtn = NUMONYX_waitFlashReady(TIMEOUT_MAX);
   if (rtn != ERR_NO_ERROR)
   {
      return (rtn);
   }

   // erase sectors until we go past the address + length.
   while (erase_addr < (in_addr + in_length))
   {
      rtn = NUMONYX_waitFlashReady(TIMEOUT_POST_SEC_ERASE);
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }

      // send the write enable and sector erase commands.
      rtn = NUMONYX_sendCmdWriteEnable();
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }

      rtn = NUMONYX_sendCmdSectorErase(erase_addr);
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }

      erase_addr += SPIFLASH_SECTOR_SIZE;
   }

   // wait for the final erase to complete before returning (if desired).
   if (wait_for_completion)
   {
      rtn = NUMONYX_waitFlashReady(TIMEOUT_POST_SEC_ERASE);
      if (rtn != ERR_NO_ERROR)
      {
         return (rtn);
      }
   }
   
   return (rtn);
}
