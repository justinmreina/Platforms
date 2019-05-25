//-----------------------------------------------------------------------------
// \file    evmomapl138_nand.c
// \brief   implementation of the nand driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_emif.h"
#include "evmomapl138_nand.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

// emifa nand pinmux defines.
#define PINMUX_EMIFA_NAND_REG_0        (7)
#define PINMUX_EMIFA_NAND_MASK_0       (0x00FF0FF0)
#define PINMUX_EMIFA_NAND_VAL_0        (0x00110110)
#define PINMUX_EMIFA_NAND_REG_1        (9)
#define PINMUX_EMIFA_NAND_MASK_1       (0xFFFFFFFF)
#define PINMUX_EMIFA_NAND_VAL_1        (0x11111111)
#define PINMUX_EMIFA_NAND_REG_2        (12)
#define PINMUX_EMIFA_NAND_MASK_2       (0x0FF00000)
#define PINMUX_EMIFA_NAND_VAL_2        (0x01100000)

#define NAND_SPARE_BYTE_SIZE           (64)
#define TIMEOUT_NAND                   (750) // in us.

// nand flash
#define NAND_FLASH_BASE                (0x62000000)
#define NAND_OFFSET_DATA               (0x00)
#define NAND_OFFSET_ALE                (0x08)
#define NAND_OFFSET_CLE                (0x10)

// nand flash command defines.
#define NAND_CMD_READ                  (0x00)
#define NAND_CMD_PAGE_PROG_CONFIRM     (0x10)
#define NAND_CMD_READ_CONFIRM          (0x30)
#define NAND_CMD_ERASE                 (0x60)
#define NAND_CMD_READ_STAT             (0x70)
#define NAND_CMD_PAGE_PROG             (0x80)
#define NAND_CMD_READ_ID               (0x90)
#define NAND_CMD_ERASE_CONFIRM         (0xD0)
#define NAND_CMD_RESET                 (0xFF)

// nand flash status reg defines.
#define NAND_STAT_READY                (0x40)
#define NAND_STAT_FAIL                 (0x01)

// nand flash address defines.
#define NAND_ADDR_READ_ID              (0x00)
#define NAND_ADDR_ONFI_ID              (0x20)

// nand flash addressing cycles.
#define ADDR_CYCLES_3                  (3)
#define ADDR_CYCLES_5                  (5)

// expected device id for Micron MT29F4F08AAC.
#define EXPECTED_MFG_ID                (0x2C)
#define EXPECTED_DEV_ID                (0xDC)

//-----------------------------------------------------------------------------
// Static Variable Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static uint32_t readSpareData(uint32_t in_block, uint32_t in_page, uint8_t *spare_data);
static uint32_t waitFlashReady(uint32_t in_timeout);
static uint32_t waitFlashStatus(uint32_t in_timeout);
static void writeAddressBytes(uint8_t in_num_cycles, uint32_t in_block, uint32_t in_page);
static void readData(uint8_t *dest_buffer, uint32_t in_length);
static void writeData(uint8_t *src_buffer, uint32_t in_length);
static void writeLatchEnable(uint32_t in_offset, uint8_t in_data);
static uint32_t verifyDeviceId(uint32_t in_timeout);
static void insertECCIntoSpare(uint8_t *in_spare);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the nand flash for use.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...nand ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t NAND_init(void)
{
   uint32_t rtn = 0;
   
   // make sure the ui board is connected before proceeding.
   if (!UTIL_isUIBoardAttached())
      return (ERR_NO_UI_BOARD);
   
   // enable the psc and config pinmux for nand.
   EVMOMAPL138_lpscTransition(PSC0, DOMAIN0, LPSC_EMIFA, PSC_ENABLE);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NAND_REG_0, PINMUX_EMIFA_NAND_MASK_0, PINMUX_EMIFA_NAND_VAL_0);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NAND_REG_1, PINMUX_EMIFA_NAND_MASK_1, PINMUX_EMIFA_NAND_VAL_1);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NAND_REG_2, PINMUX_EMIFA_NAND_MASK_2, PINMUX_EMIFA_NAND_VAL_2);
   
   // config nand fcr reg.
   CLRBIT(EMIFA->NANDFCR, ECCSEL_MASK);
   SETBIT(EMIFA->NANDFCR, ECCSEL_CS3);
   SETBIT(EMIFA->NANDFCR, CS3NAND);
   
   // send reset cmd to nand.
   writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_RESET);
   rtn = waitFlashReady(TIMEOUT_NAND);
   if (rtn != ERR_NO_ERROR)
   {
      #ifdef DEBUG
      printf("nand wait ready error:\t%d\r\n", rtn);
      #endif
      return (ERR_INIT_FAIL);
   }
   
   // read/verify device id.
   rtn = verifyDeviceId(TIMEOUT_NAND);
   if (rtn != ERR_NO_ERROR)
   {
      #ifdef DEBUG
      printf("nand device id error:\t%d\r\n", rtn);
      #endif
      return (ERR_INIT_FAIL);
   }

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   read a page out of nand flash.
//
// \param   uint32_t in_block - nand block to be read.
//
// \param   uint32_t in_page - nand page to be read.
//
// \param   uint8_t *dest_buffer - pointer to memory where data is to be copied.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...nand data copied to dest buffer.
//    else - something happened during nand read.
//-----------------------------------------------------------------------------
uint32_t NAND_readPage(uint32_t in_block, uint32_t in_page, uint8_t *dest_buffer)
{
   uint32_t rtn;
   
   // setup the nand read.
   writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_READ);
   writeAddressBytes(ADDR_CYCLES_5, in_block, in_page);
   writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_READ_CONFIRM);
   rtn = waitFlashReady(TIMEOUT_NAND);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // copy data to buffer.
   readData(dest_buffer, NAND_PAGE_SIZE);

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   write a page of nand flash.
//
// \param   uint32_t in_block - nand block to be written.
//
// \param   uint32_t in_page - nand page to be written.
//
// \param   uint8_t *src_buffer - pointer to data to write.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...data written to nand.
//    else - something happened during nand write.
//-----------------------------------------------------------------------------
uint32_t NAND_writePage(uint32_t in_block, uint32_t in_page, uint8_t *src_buffer)
{
   uint32_t rtn;
   uint32_t i;
   uint8_t spare_data[NAND_SPARE_BYTE_SIZE];
   
   // set spare to FF's.
   for (i = 0; i < NAND_SPARE_BYTE_SIZE; i++)
   {
      spare_data[i] = 0xFF;
   }
   
   // enable ECC.
   SETBIT(EMIFA->NANDFCR, FOURBITECC_START);

   // setup nand write.
   writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_PAGE_PROG);
   writeAddressBytes(ADDR_CYCLES_5, in_block, in_page);
   rtn = waitFlashReady(TIMEOUT_NAND);
   if (rtn != ERR_NO_ERROR)
      return (rtn);
   
   // copy data to nand.
   writeData(src_buffer, NAND_PAGE_SIZE);
   
   // place the ecc into the spare bytes...this function will read the
   // ecc regs, so ecc will be disabled.
   insertECCIntoSpare(spare_data);

   // write the spare bytes to nand.
   writeData(spare_data, NAND_SPARE_BYTE_SIZE);

   // send confirm command to verify write.
   writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_PAGE_PROG_CONFIRM);

   // check write status.
   return (waitFlashStatus(TIMEOUT_NAND));
}

//-----------------------------------------------------------------------------
// \brief   erase a block of nand flash.
//
// \param   uint32_t in_block - nand block to be erased.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...block erased.
//    else - something happened during nand erase.
//-----------------------------------------------------------------------------
uint32_t NAND_eraseBlock(uint32_t in_block)
{
   uint32_t rtn;

   // setup nand write.
   writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_ERASE);
   writeAddressBytes(ADDR_CYCLES_3, in_block, 0);
   writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_ERASE_CONFIRM);
   rtn = waitFlashReady(TIMEOUT_NAND);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // check erase status.
   return (waitFlashStatus(TIMEOUT_NAND));
}

//-----------------------------------------------------------------------------
// \brief   check if a block of nand flash is bad.
//
// \param   uint32_t in_block - nand block to be checked.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...block is not bad.
//    ERR_NAND_BAD_BLOCK - block is bad.
//    else - something happened during nand read.
//-----------------------------------------------------------------------------
uint32_t NAND_isBadBlock(uint32_t in_block)
{
   uint32_t rtn = ERR_NO_ERROR;
   uint32_t i, j;
   uint8_t spare_data[NAND_SPARE_BYTE_SIZE];
   
   // check bytes 0-5 of spare data of the first two pages of the block
   // for non-0xFF data.
   for (i = 0; i < 2; i++)
   {
      rtn = readSpareData(in_block, i, spare_data);
      if (rtn != ERR_NO_ERROR)
         return (rtn);

      for (j = 0; j < 6; j++)
      {
         if (spare_data[j] != 0xFF)
         {
            rtn = ERR_NAND_BAD_BLOCK;
            break;
         }
      }
   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

uint32_t readSpareData(uint32_t in_block, uint32_t in_page, uint8_t *spare_data)
{
   uint32_t rtn;
   uint32_t i;
   uint8_t dummy_byte;

   // setup the nand read.
   writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_READ);
   writeAddressBytes(ADDR_CYCLES_5, in_block, in_page);
   writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_READ_CONFIRM);
   rtn = waitFlashReady(TIMEOUT_NAND);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // copy data portion of page to dummy byte.
   for (i = 0; i < NAND_PAGE_SIZE; i++)
      readData(&dummy_byte, 1);

   // copy spare portion of page to buffer.
   readData(spare_data, NAND_SPARE_BYTE_SIZE);

   return(ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// waits for flash to not be busy or timeout occurs.
//-----------------------------------------------------------------------------
uint32_t waitFlashReady(uint32_t in_timeout)
{
   uint32_t cnt = 0;
   uint32_t start = 0;
   
   start = USTIMER_get();
   
   while (!CHKBIT(EMIFA->NANDFSR, WAITST))
   {
      cnt = USTIMER_get() - start;
      if (cnt > in_timeout)
      {
         return (ERR_TIMEOUT);
      }
   }

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// waits for flash status to be available and returns pass/fail.
//-----------------------------------------------------------------------------
uint32_t waitFlashStatus(uint32_t in_timeout)
{
   uint32_t cnt = 0;
   uint32_t start = 0;
   uint8_t status;
   
   start = USTIMER_get();
   
   do
   {
      cnt = USTIMER_get() - start;
      if (cnt > in_timeout)
         return (ERR_TIMEOUT);
         
      writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_READ_STAT);
      readData(&status, 1);
      
   } while (!CHKBIT(status, NAND_STAT_READY));
   
   if (CHKBIT(status, NAND_STAT_FAIL))
      return (ERR_FAIL);
   else
      return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// writes the flash address...pass in 3 or 5 cycles.
//-----------------------------------------------------------------------------
void writeAddressBytes(uint8_t in_num_cycles, uint32_t in_block, uint32_t in_page)
{
   if (ADDR_CYCLES_5 == in_num_cycles)
   {
      writeLatchEnable(NAND_OFFSET_ALE, 0);
      writeLatchEnable(NAND_OFFSET_ALE, 0);
   }
   
   writeLatchEnable(NAND_OFFSET_ALE, (in_page & 0x3F) | ((in_block & 0x03) << 6));
   writeLatchEnable(NAND_OFFSET_ALE, ((in_block >> 2) & 0xFF));
   writeLatchEnable(NAND_OFFSET_ALE, ((in_block >> 10) & 0x03));
}

//-----------------------------------------------------------------------------
// reads data out of nand.
//-----------------------------------------------------------------------------
void readData(uint8_t *dest_buffer, uint32_t in_length)
{
   volatile uint8_t *src, *dest;
   uint32_t i;
   
   dest = (volatile uint8_t *)dest_buffer;
   src = (volatile uint8_t *)(NAND_FLASH_BASE + NAND_OFFSET_DATA);
   
   for (i = 0; i < in_length; i++)
   {
      dest[i] = *src;
   }
}

//-----------------------------------------------------------------------------
// writes nand data.
//-----------------------------------------------------------------------------
void writeData(uint8_t *src_buffer, uint32_t in_length)
{
   volatile uint8_t *src, *dest;
   uint32_t i;
   
   dest = (volatile uint8_t *)(NAND_FLASH_BASE + NAND_OFFSET_DATA);
   src = (volatile uint8_t *)src_buffer;
   
   for (i = 0; i < in_length; i++)
   {
      *dest = src[i];
   }
}

//-----------------------------------------------------------------------------
// writes the data to the latch enable specified by the offset.
//-----------------------------------------------------------------------------
void writeLatchEnable(uint32_t in_offset, uint8_t in_data)
{
   volatile uint8_t *latch;
   
   latch = (volatile uint8_t *)(NAND_FLASH_BASE + in_offset);
   
   *latch = in_data;
}

//-----------------------------------------------------------------------------
// sends the read id command and verifies the response against expected.
//-----------------------------------------------------------------------------
uint32_t verifyDeviceId(uint32_t in_timeout)
{
   uint32_t rtn;
   uint8_t dev_id[5];
   #ifdef DEBUG
   uint32_t i;
   #endif
   
   // send the read id cmd and wait for response.
   writeLatchEnable(NAND_OFFSET_CLE, NAND_CMD_READ_ID);
   writeLatchEnable(NAND_OFFSET_ALE, NAND_ADDR_READ_ID);
   
   rtn = waitFlashReady(TIMEOUT_NAND);
   if (rtn != ERR_NO_ERROR)
   {
      return (rtn);
   }
   
   // read data and verify against expected.
   readData(dev_id, 5);
   
   #ifdef DEBUG
   for (i = 0; i < 5; i++)
   {
      printf("dev_id[%u]: 0x%02X\r\n", i, dev_id[i]);
   }
   #endif
   
   if ((dev_id[0] != EXPECTED_MFG_ID) ||
      (dev_id[1] != EXPECTED_DEV_ID))
      return (ERR_FAIL);
      
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// convert the 4 ECC regs into 10 bytes of data to write to spare.
//-----------------------------------------------------------------------------
void insertECCIntoSpare(uint8_t *in_spare)
{
   uint32_t out[3],in[4],i;
   uint32_t ecc_regs[4];

   ecc_regs[0] = (EMIFA->NAND4BITECC1 & 0x03FF03FF);
   ecc_regs[1] = (EMIFA->NAND4BITECC2 & 0x03FF03FF);
   ecc_regs[2] = (EMIFA->NAND4BITECC3 & 0x03FF03FF);
   ecc_regs[3] = (EMIFA->NAND4BITECC4 & 0x03FF03FF);

   // convert the ecc reg data into a 10 byte array.

   for (i = 0; i < 4; i++)
   {
      ((uint8_t *)in)[i] = ((uint8_t *)ecc_regs)[i];
   }

  out[0] = ((in[1]&0x00030000) <<14) | ((in[1]&0x000003FF) <<20) |
           ((in[0]&0x03FF0000) >> 6) | ((in[0]&0x000003FF)     );
  out[1] = ((in[3]&0x0000000F) <<28) | ((in[2]&0x03FF0000) << 2) |
           ((in[2]&0x000003FF) << 8) | ((in[1]&0x03FC0000) >>18);
  out[2] = ((out[2]&0xFFFF0000)    ) | ((in[3]&0x03FF0000) >>10) |
           ((in[3]&0x000003F0) >> 4);

  for (i = 0; i < 10; i++)
  {
    in_spare[i + 6] = ((uint8_t *)out)[i];
  }
  
  for (i = 16; i < 26; i++)
  {
    in_spare[i + 6] = ((uint8_t *)out)[i - 16];
  }
  
  for (i = 32; i < 42; i++)
  {
    in_spare[i + 6] = ((uint8_t *)out)[i - 32];
  }
  
  for (i = 48; i < 58; i++)
  {
    in_spare[i + 6] = ((uint8_t *)out)[i - 48];
  }
}
