//-----------------------------------------------------------------------------
// \file    evmomapl138_nor.c
// \brief   implementation of the nor driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_emif.h"
#include "evmomapl138_nor.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
// emifa nor pinmux defines.
#define PINMUX_EMIFA_NOR_REG_0      (5)
#define PINMUX_EMIFA_NOR_MASK_0     (0xFF000000)
#define PINMUX_EMIFA_NOR_VAL_0      (0x11000000)
#define PINMUX_EMIFA_NOR_REG_1      (6)
#define PINMUX_EMIFA_NOR_MASK_1     (0x0F00000F)
#define PINMUX_EMIFA_NOR_VAL_1      (0x01000001)
#define PINMUX_EMIFA_NOR_REG_2      (7)
#define PINMUX_EMIFA_NOR_MASK_2     (0x00FF000F)
#define PINMUX_EMIFA_NOR_VAL_2      (0x00110001)
#define PINMUX_EMIFA_NOR_REG_3      (8)
#define PINMUX_EMIFA_NOR_MASK_3     (0xFFFFFFFF)
#define PINMUX_EMIFA_NOR_VAL_3      (0x11111111)
#define PINMUX_EMIFA_NOR_REG_4      (9)
#define PINMUX_EMIFA_NOR_MASK_4     (0xFFFFFFFF)
#define PINMUX_EMIFA_NOR_VAL_4      (0x11111111)
#define PINMUX_EMIFA_NOR_REG_5      (10)
#define PINMUX_EMIFA_NOR_MASK_5     (0xFFFFFFFF)
#define PINMUX_EMIFA_NOR_VAL_5      (0x11111111)
#define PINMUX_EMIFA_NOR_REG_6      (11)
#define PINMUX_EMIFA_NOR_MASK_6     (0xFFFFFFFF)
#define PINMUX_EMIFA_NOR_VAL_6      (0x11111111)
#define PINMUX_EMIFA_NOR_REG_7      (12)
#define PINMUX_EMIFA_NOR_MASK_7     (0xFFFFFFFF)
#define PINMUX_EMIFA_NOR_VAL_7      (0x11111111)

// nor flash defines.
#define NOR_TIMEOUT_MAX             (2000000)       // TODO: check datasheet.

// nor command defines.
#define NOR_CMD_READ_ARRAY          (0xFF)
#define NOR_CMD_READ_ID             (0x90)
#define NOR_CMD_READ_QUERY          (0x98)
#define NOR_CMD_READ_STATUS         (0x70)
#define NOR_CMD_CLEAR_STATUS        (0x50)
#define NOR_CMD_PROGRAM             (0x40)
#define NOR_CMD_WRITE_BUFFER        (0xE8)
#define NOR_CMD_BLOCK_ERASE         (0x20)
#define NOR_CMD_CONFIRM             (0xD0)
#define NOR_CMD_BLOCK_UNLOCK        (0x60)
// nor status defines.
#define NOR_STAT_RDY                (0x80)
// id defines
#define OFFSET_MFG_ID               (0x00)
#define OFFSET_DEV_ID               (0x02)
#define EXPECTED_MFG_ID             (0x0089)
#define EXPECTED_DEV_ID             (0x8817)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static void writeFlash(uint32_t in_addr, uint16_t in_val);
static uint32_t waitFlashReady(uint32_t in_addr, uint32_t in_timeout);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the nor flash for use.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...nand ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t NOR_init(void)
{
   uint32_t rtn = ERR_NO_ERROR;
   uint16_t mfgid, devid;
#ifndef BOOT
   // make sure the ui board is connected before proceeding.
   if (!UTIL_isUIBoardAttached())
      return (ERR_NO_UI_BOARD);
#endif

   // need to clear sd/nor select pin to use nor.
   EVMOMAPL138_pinmuxConfig(PINMUX_GPIO_SD_NOR_SELECT_REG,
                            PINMUX_GPIO_SD_NOR_SELECT_MASK,
                            PINMUX_GPIO_SD_NOR_SELECT_VAL);
   GPIO_setDir(GPIO_SD_NOR_SELECT_BANK, GPIO_SD_NOR_SELECT_PIN, GPIO_OUTPUT);
   GPIO_setOutput(GPIO_SD_NOR_SELECT_BANK, GPIO_SD_NOR_SELECT_PIN, OUTPUT_LOW);
   
   // enable the psc and configure pinmux for nor.
   EVMOMAPL138_lpscTransition(PSC0, DOMAIN0, LPSC_EMIFA, PSC_ENABLE);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NOR_REG_0, PINMUX_EMIFA_NOR_MASK_0, PINMUX_EMIFA_NOR_VAL_0);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NOR_REG_1, PINMUX_EMIFA_NOR_MASK_1, PINMUX_EMIFA_NOR_VAL_1);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NOR_REG_2, PINMUX_EMIFA_NOR_MASK_2, PINMUX_EMIFA_NOR_VAL_2);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NOR_REG_3, PINMUX_EMIFA_NOR_MASK_3, PINMUX_EMIFA_NOR_VAL_3);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NOR_REG_4, PINMUX_EMIFA_NOR_MASK_4, PINMUX_EMIFA_NOR_VAL_4);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NOR_REG_5, PINMUX_EMIFA_NOR_MASK_5, PINMUX_EMIFA_NOR_VAL_5);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NOR_REG_6, PINMUX_EMIFA_NOR_MASK_6, PINMUX_EMIFA_NOR_VAL_6);
   EVMOMAPL138_pinmuxConfig(PINMUX_EMIFA_NOR_REG_7, PINMUX_EMIFA_NOR_MASK_7, PINMUX_EMIFA_NOR_VAL_7);
   
   // config for 16-bit data bus.
   SETBIT(EMIFA->CE2CFG, ASIZE);
   
   // clear flash status.
   writeFlash(NOR_FLASH_BASE, NOR_CMD_CLEAR_STATUS);
   writeFlash(NOR_FLASH_BASE, NOR_CMD_CLEAR_STATUS);
   
   // read id.
   writeFlash(NOR_FLASH_BASE, NOR_CMD_READ_ID);
   writeFlash(NOR_FLASH_BASE, NOR_CMD_READ_ID);
   
   // read cfi query.
   writeFlash(NOR_FLASH_BASE, NOR_CMD_READ_QUERY);
   writeFlash(NOR_FLASH_BASE, NOR_CMD_READ_QUERY);

   mfgid = *(uint16_t *)(NOR_FLASH_BASE + OFFSET_MFG_ID);
   devid = *(uint16_t *)(NOR_FLASH_BASE + OFFSET_DEV_ID);

   // verify the mfg and dev id match expected.
   if ((mfgid != EXPECTED_MFG_ID) ||
         (devid != EXPECTED_DEV_ID))
   {
      rtn = ERR_INIT_FAIL;
   }

   // clear status and put chip back into read mode.
   writeFlash(NOR_FLASH_BASE, NOR_CMD_CLEAR_STATUS);
   writeFlash(NOR_FLASH_BASE, NOR_CMD_READ_ARRAY);

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   read data from nor flash.
//
// \param   .
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...data copied to dest buffer.
//-----------------------------------------------------------------------------
uint32_t NOR_read(uint32_t in_addr, uint16_t *dest_buffer, uint32_t in_length)
{
   uint16_t *src;
   uint32_t i;
   uint32_t word_count = (in_length / 2);

   src = (uint16_t *)in_addr;
   for (i = 0; i < word_count; i++)
   {
     *dest_buffer++ = *src++;
   }
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   write data to nor flash.
//
// \param   uint32_t in_addr - start address of flash to be written.
//
// \param   uint16_t *src_buffer - pointer to data to write.
//
// \param   uint32_t in_length - number of bytes to write.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...data written to nor.
//    else - something happened during write.
//-----------------------------------------------------------------------------
uint32_t NOR_write(uint32_t in_addr, uint16_t *src_buffer, uint32_t in_length)
{
   uint32_t rtn;
   uint32_t i;
   uint16_t word_count = (in_length / 2);
   
   for (i = 0; i < word_count; i++)
   {
      writeFlash(in_addr, NOR_CMD_PROGRAM);
      writeFlash(in_addr + (2 * i), src_buffer[i]);
      
      rtn = waitFlashReady(in_addr, NOR_TIMEOUT_MAX);
      if (rtn != ERR_NO_ERROR)
         break;
   }
   
   writeFlash(in_addr, NOR_CMD_CLEAR_STATUS);
   writeFlash(in_addr, NOR_CMD_READ_ARRAY);

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   erase block of nor flash that contains input address.
//
// \param   uint32_t in_block_addr - address within block to be erased.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...nor flash erased.
//    else - something happened during erase.
//-----------------------------------------------------------------------------
uint32_t NOR_eraseBlock(uint32_t in_block_addr)
{
   uint32_t rtn;
   
   // send erase command, passing desired address.
   writeFlash(in_block_addr, NOR_CMD_BLOCK_ERASE);
   writeFlash(in_block_addr, NOR_CMD_CONFIRM);
   
   // wait for chip to be ready.
   rtn = waitFlashReady(in_block_addr, NOR_TIMEOUT_MAX);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // clear status and put chip back into read mode.
   writeFlash(in_block_addr, NOR_CMD_CLEAR_STATUS);
   writeFlash(in_block_addr, NOR_CMD_READ_ARRAY);
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   unlock a block in nor flash that contains the input address.
//
// \param   uint32_t in_block_addr - address within block to be unlocked.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...nor flash block unlocked.
//    else - something happened during unlock.
//-----------------------------------------------------------------------------
uint32_t NOR_unlockBlock(uint32_t in_block_addr)
{
   uint32_t rtn;
   
   // send erase command, passing desired address.
   writeFlash(in_block_addr, NOR_CMD_BLOCK_UNLOCK);
   writeFlash(in_block_addr, NOR_CMD_CONFIRM);

   // wait for chip to be ready.
   rtn = waitFlashReady(in_block_addr, NOR_TIMEOUT_MAX);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // clear status and put chip back into read mode.
   writeFlash(in_block_addr, NOR_CMD_CLEAR_STATUS);
   writeFlash(in_block_addr, NOR_CMD_READ_ARRAY);

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// write 16 bits of data to flash.
//-----------------------------------------------------------------------------
void writeFlash(uint32_t in_addr, uint16_t in_val)
{
   volatile uint32_t dummy;

   // write the value to flash
   *(volatile uint16_t*)in_addr = in_val;

   // read something outside of flash (stack) to toggle CS
   dummy = (uint32_t)&dummy;
}

//-----------------------------------------------------------------------------
// waits for flash to not be busy or timeout occurs.
//-----------------------------------------------------------------------------
uint32_t waitFlashReady(uint32_t in_addr, uint32_t in_timeout)
{
   uint32_t cnt = 0;
   uint32_t start = 0;
   uint16_t status;
   
   start = USTIMER_get();

   do 
   {
      cnt = USTIMER_get() - start;
      if (cnt > in_timeout)
      {
         return (ERR_TIMEOUT);
      }
      
      status = (uint16_t)*(volatile uint16_t*)in_addr;
   } while (!CHKBIT(status, NOR_STAT_RDY));

   return (ERR_NO_ERROR);
} 
