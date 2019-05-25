//-----------------------------------------------------------------------------
// \file    spiflash_numonyx.c
// \brief   implementation of comands for Numonyx M25P64 SPI flash.
//
//-----------------------------------------------------------------------------

#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_spi.h"
#include "evmomapl138_spiflash.h"
#include "spiflash_numonyx.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

// numonyx spi flash commands.
#define NUMONYX_CMD_WREN            (0x06)
#define NUMONYX_CMD_WRDI            (0x04)
#define NUMONYX_CMD_RDID            (0x9F)
#define NUMONYX_CMD_RDSR            (0x05)
#define NUMONYX_CMD_WRSR            (0x01)
#define NUMONYX_CMD_READ            (0x03)
#define NUMONYX_CMD_FAST_READ       (0x0B)
#define NUMONYX_CMD_PAGE_PROG       (0x02)
#define NUMONYX_CMD_SEC_ERASE       (0xD8)
#define NUMONYX_CMD_BULK_ERASE      (0xC7)
#define NUMONYX_CMD_RD_ELEC_SIG     (0xAB)

// numonyx command / response sizes.
#define CMD_SIZE_WREN               (1)
#define CMD_SIZE_RDSR               (1)
#define RESP_SIZE_RDSR              (1)
#define CMD_SIZE_FAST_READ          (5)
#define CMD_SIZE_PAGE_PROG          (4)
#define CMD_SIZE_SEC_ERASE          (4)

// numonyx status register defines.
#define SR_WRITE_PROTECT            (0x80)
#define SR_BLOCK_PROTECT_2          (0x10)
#define SR_BLOCK_PROTECT_1          (0x08)
#define SR_BLOCK_PROTECT_0          (0x04)
#define SR_WRITE_ENABLE_LATCH       (0x02)
#define SR_BUSY                     (0x01)

// numonyx mfr and device ids.
#define NUMONYX_MFR_ID              (0x20)
#define NUMONYX_DEV_ID_TYPE         (0x20)
#define NUMONYX_DEV_ID_CAP          (0x17)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static void copyAddr(uint32_t addr, uint8_t *buffer);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// initialize spi port for spi flash.
//-----------------------------------------------------------------------------
uint32_t NUMONYX_initSpiFlashPort(void)
{
   uint32_t rtn = ERR_NO_ERROR;
   spi_config_t spi_config;

   spi_config.mode = SPI_MODE_MASTER;
   spi_config.pin_option = SPI_4PIN_CS;
   spi_config.cs_active = SPI_CS_ACTIVE_LOW;
   spi_config.shift_dir = SPI_SHIFT_MSB;
   spi_config.polarity = 0;
   spi_config.phase = 1;
   spi_config.freq = 50000000;

   rtn = SPI_init(SPIFLASH_SPI, &spi_config);
   if (rtn != ERR_NO_ERROR)
   {
      #ifdef DEBUG
      printf("spi flash, spi init error:\t%d\r\n", rtn);
      #endif
      return (ERR_INIT_FAIL);
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// verify device id.
//-----------------------------------------------------------------------------
uint32_t NUMONYX_verifyDeviceID(void)
{
   uint32_t rtn;
   uint8_t spi_data[4];
   
   spi_data[0] = NUMONYX_CMD_RDID;
   spi_data[1] = 0;
   spi_data[2] = 0;
   spi_data[3] = 0;
   #ifdef DEBUG
   printf("spi_data tx:\t%02X %02X %02X %02X\r\n\r\n", spi_data[0], spi_data[1], spi_data[2], spi_data[3]);
   #endif
   
   rtn = SPI_xfer(SPIFLASH_SPI, spi_data, spi_data, 4, SPI_HOLD_CLR);
   if (rtn != ERR_NO_ERROR)
   {
      #ifdef DEBUG
      printf("send read id cmd error:\t%d\r\n", rtn);
      #endif
      return (ERR_FAIL);
   }

   // verify device id.
   #ifdef DEBUG
   printf("spi_data rx:\t%02X %02X %02X %02X\r\n\r\n", spi_data[0], spi_data[1], spi_data[2], spi_data[3]);
   /*printf("spi_data[%u]:\t%02X\r\n", 0, spi_data[0]);
   printf("spi_data[%u]:\t%02X\r\n", 1, spi_data[1]);
   printf("spi_data[%u]:\t%02X\r\n", 2, spi_data[2]);
   printf("spi_data[%u]:\t%02X\r\n", 3, spi_data[3]);
   printf("mfg id:\t%02X\r\n", spi_data[1]);
   printf("dev id mem type:\t%02X\r\n", spi_data[2]);
   printf("dev id mem cap:\t%02X\r\n", spi_data[3]);*/
   #endif

   if ((spi_data[1] != NUMONYX_MFR_ID) ||
         (spi_data[2] != NUMONYX_DEV_ID_TYPE) ||
         (spi_data[3] != NUMONYX_DEV_ID_CAP))
   {
      #ifdef DEBUG
      printf("spi flash id does not match expected id!\r\n");
      #endif
      return (ERR_FAIL);
   }

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// wait until spi flash is not busy.
//-----------------------------------------------------------------------------
uint32_t NUMONYX_waitFlashReady(uint32_t in_us_timeout)
{
   uint32_t rtn;
   uint8_t spi_data[CMD_SIZE_RDSR + RESP_SIZE_RDSR];
   
   // reset the microsecond timer...will be used to watch for timeout.
   USTIMER_reset();
   
   do
   {
      // send the read status reg command.
      spi_data[0] = NUMONYX_CMD_RDSR;
      rtn = SPI_xfer(SPIFLASH_SPI, spi_data, spi_data, CMD_SIZE_RDSR + RESP_SIZE_RDSR, SPI_HOLD_CLR);
      if (rtn != ERR_NO_ERROR)
         return (rtn);
      
      // see if we have gone beyond our timeout.
      if (USTIMER_get() > in_us_timeout)
         return (ERR_TIMEOUT);

   } while (spi_data[1] & SR_BUSY);
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// send the write enable command.
//-----------------------------------------------------------------------------
uint32_t NUMONYX_sendCmdWriteEnable(void)
{
   uint8_t spi_cmd_data[CMD_SIZE_WREN];

   spi_cmd_data[0] = NUMONYX_CMD_WREN;

   return SPI_xfer(SPIFLASH_SPI, spi_cmd_data, NULL, CMD_SIZE_WREN, SPI_HOLD_CLR);
}

//-----------------------------------------------------------------------------
// send the fast read command and address bytes.
//-----------------------------------------------------------------------------
uint32_t NUMONYX_sendCmdFastRead(uint32_t in_addr)
{
   uint8_t spi_cmd_data[CMD_SIZE_FAST_READ];

   spi_cmd_data[0] = NUMONYX_CMD_FAST_READ;
   copyAddr(in_addr, &spi_cmd_data[1]);
   spi_cmd_data[4] = 0;
   
   // tranmit the command, keeping chip select active so the data can be
   // clocked in immediately after command.
   return SPI_xfer(SPIFLASH_SPI, spi_cmd_data, NULL, CMD_SIZE_FAST_READ, SPI_HOLD_ACTIVE);
}

//-----------------------------------------------------------------------------
// send the page program command and address bytes.
//-----------------------------------------------------------------------------
uint32_t NUMONYX_sendCmdPageProgram(uint32_t in_addr)
{
   uint8_t spi_cmd_data[CMD_SIZE_PAGE_PROG];

   spi_cmd_data[0] = NUMONYX_CMD_PAGE_PROG;
   copyAddr(in_addr, &spi_cmd_data[1]);

   // tranmit the command, keeping chip select active so the data can be 
   // clocked out immediately after command.
   return SPI_xfer(SPIFLASH_SPI, spi_cmd_data, NULL, CMD_SIZE_PAGE_PROG, SPI_HOLD_ACTIVE);
}

//-----------------------------------------------------------------------------
// send the sector erase command and address bytes.
//-----------------------------------------------------------------------------
uint32_t NUMONYX_sendCmdSectorErase(uint32_t in_addr)
{
   uint8_t spi_cmd_data[CMD_SIZE_SEC_ERASE];

   spi_cmd_data[0] = NUMONYX_CMD_SEC_ERASE;
   copyAddr(in_addr, &spi_cmd_data[1]);
   
   return SPI_xfer(SPIFLASH_SPI, spi_cmd_data, NULL, CMD_SIZE_SEC_ERASE, SPI_HOLD_CLR);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// helper function to copy the uint32 address into a buffer to send via spi.
//-----------------------------------------------------------------------------
void copyAddr(uint32_t addr, uint8_t *buffer)
{
   buffer[0] = (uint8_t)((addr & 0x00FF0000) >> 16);
   buffer[1] = (uint8_t)((addr & 0x0000FF00) >> 8);
   buffer[2] = (uint8_t)(addr & 0x000000FF);
}
