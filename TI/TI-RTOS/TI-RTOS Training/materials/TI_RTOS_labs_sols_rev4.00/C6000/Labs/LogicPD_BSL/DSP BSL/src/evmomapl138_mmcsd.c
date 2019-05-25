//-----------------------------------------------------------------------------
// \file    evmomapl138_mmcsd.c
// \brief   implementation of the mmc/sd driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------

#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_mmcsd.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
// pinmux defines.
#define PINMUX_MMCSD0_REG_0      (10)
#define PINMUX_MMCSD0_MASK_0     (0xFFFFFFFF)
#define PINMUX_MMCSD0_VAL_0      (0x22222222)
#define PINMUX_MMCSD0_REG_1      (11)
#define PINMUX_MMCSD0_MASK_1     (0x000000FF)
#define PINMUX_MMCSD0_VAL_1      (0x00000022)

#define PINMUX_MMCSD1_REG_0      (18)
#define PINMUX_MMCSD1_MASK_0     (0xFF0FFF00)
#define PINMUX_MMCSD1_VAL_0      (0x11022200)
#define PINMUX_MMCSD1_REG_1      (19)
#define PINMUX_MMCSD1_MASK_1     (0x0FFFFFFF)
#define PINMUX_MMCSD1_VAL_1      (0x02221111)

// mmc/sd commands.
#define MMCSD_CMD_GO_IDLE              (0)
#define MMCSD_CMD_ALL_SEND_CID         (2 | RSPFMT_136BIT_CRC)
#define MMCSD_CMD_SELECT_CARD          (7 | RSPFMT_48BIT_CRC)
#define MMCSD_CMD_DESELECT_CARD        (7)
#define MMCSD_CMD_SEND_CSD             (9 | RSPFMT_136BIT_CRC)
#define MMCSD_CMD_SEND_CID             (10 | RSPFMT_136BIT_CRC)
#define MMCSD_CMD_SEND_STATUS          (13 | RSPFMT_48BIT_CRC)
#define MMCSD_CMD_GO_INACTIVE          (15)
#define MMCSD_CMD_SET_BLK_LEN          (16 | RSPFMT_48BIT_CRC)
#define MMCSD_CMD_READ_SINGLE_BLOCK    (17 | RSPFMT_48BIT_CRC)
#define MMCSD_CMD_READ_MULT_BLOCK      (18 | RSPFMT_48BIT_CRC)
#define MMCSD_CMD_WRITE_SINGLE_BLOCK   (24 | RSPFMT_48BIT_CRC)
#define MMCSD_CMD_WRITE_MULT_BLOCK     (25 | RSPFMT_48BIT_CRC)
#define MMCSD_CMD_APP                  (55 | RSPFMT_48BIT_CRC)

#define SD_CMD_SEND_REL_ADDR           (03 | RSPFMT_48BIT_CRC | PPLEN )
#define SD_CMD_APP_OP_COND             (41 | RSPFMT_48BIT_NOCRC)
#define SD_CMD_SET_BUS_WIDTH           (06 | RSPFMT_48BIT_CRC | PPLEN )
#define MMC_CMD_SEND_OP_COND           (01 | RSPFMT_48BIT_NOCRC)

#define MMCSD_DEFAULT_NBLK             (1)
#define MMCSD_DEFAULT_BLKLEN           (512)
#define MMCSD_OP_TIMEOUT               (2048)

#define RW_TIMEOUT                     (100000)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

static void getCardResp(mmcsd_regs_t *mmcsd, uint16_t *card_resp);
static uint32_t sendRcaSD(mmcsd_regs_t *mmcsd, uint32_t *rca);
static uint32_t setBusWidth(mmcsd_regs_t *mmcsd, uint32_t rca, uint32_t in_bus_width);
static uint32_t sendCmdOpCondSD(mmcsd_regs_t *mmcsd, uint32_t in_volt_win, uint32_t in_timeout);
static uint32_t sendCmd(mmcsd_regs_t *mmcsd, uint32_t in_cmd, uint32_t in_arg, uint8_t check_status);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the mmc/sd controller for use.
//
// \param   mmcsd_regs_t *mmcsd - pointer to register struct for the desired mmcsd.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...mmc/sd ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t MMCSD_init(mmcsd_regs_t *mmcsd)
{
   // need to set sd/nor select pin to use sd interface.
   EVMOMAPL138_pinmuxConfig(PINMUX_GPIO_SD_NOR_SELECT_REG,
                            PINMUX_GPIO_SD_NOR_SELECT_MASK,
                            PINMUX_GPIO_SD_NOR_SELECT_VAL);
   GPIO_setDir(GPIO_SD_NOR_SELECT_BANK, GPIO_SD_NOR_SELECT_PIN, GPIO_OUTPUT);
   GPIO_setOutput(GPIO_SD_NOR_SELECT_BANK, GPIO_SD_NOR_SELECT_PIN, OUTPUT_HIGH);

   // enable the psc and config pinmux for desired mmc/sd controller.
   switch ((uint32_t)mmcsd)
   {
      case MMCSD0_REG_BASE:
         EVMOMAPL138_lpscTransition(PSC0, DOMAIN0, LPSC_MMCSD0, PSC_ENABLE);
         EVMOMAPL138_pinmuxConfig(PINMUX_MMCSD0_REG_0, PINMUX_MMCSD0_MASK_0, PINMUX_MMCSD0_VAL_0);
         EVMOMAPL138_pinmuxConfig(PINMUX_MMCSD0_REG_1, PINMUX_MMCSD0_MASK_1, PINMUX_MMCSD0_VAL_1);
         break;
      
      case MMCSD1_REG_BASE:
         EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_MMCSD1, PSC_ENABLE);
         EVMOMAPL138_pinmuxConfig(PINMUX_MMCSD1_REG_0, PINMUX_MMCSD1_MASK_0, PINMUX_MMCSD1_VAL_0);
         EVMOMAPL138_pinmuxConfig(PINMUX_MMCSD1_REG_1, PINMUX_MMCSD1_MASK_1, PINMUX_MMCSD1_VAL_1);
         break;
      
      default:
         return (ERR_INIT_FAIL);
   }

   // place mmc/sd into reset.
   mmcsd->MMCCTL = CMDRST | DATRST;
   
   // enable the memory clock.
   mmcsd->MMCCLK = 0x0301;
   USTIMER_delay(100);
   
   mmcsd->MMCTOR = 0x3FFF;
   mmcsd->MMCTOD = 0xFFFF;

   // release mmc/sd from reset.
   mmcsd->MMCCTL = 0;
   
   // configure the fifo to 64 byte level.
   SETBIT(mmcsd->MMCFIFOCTL, FIFOLEV);
   
   mmcsd->MMCNBLK = MMCSD_DEFAULT_NBLK;
   mmcsd->MMCBLEN = MMCSD_DEFAULT_BLKLEN;

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   perform the identify card procedure to enumerate all cards on the bus.
//
// \param   mmcsd_regs_t *mmcsd - pointer to register struct for the desired mmcsd.
//
// \param   uint32_t *rca.
//
// \param   uint32_t in_bus_width.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...mmc/sd ready to use.
//-----------------------------------------------------------------------------
uint32_t MMCSD_identifyCard(mmcsd_regs_t *mmcsd, uint32_t *rca, uint32_t in_bus_width)
{
   uint32_t rtn;
   uint16_t card_reg[8];

   // put cards in idle state.
   rtn = sendCmd(mmcsd, DCLR | MMCSD_CMD_GO_IDLE, 0, 1);
   if (rtn != ERR_NO_ERROR)
      return (rtn);
   
   // add short delay and send voltage range.
   USTIMER_delay(100000);
   sendCmd(mmcsd, MMCSD_CMD_APP, 0, 1);
   
   // TODO: add mmc identify card.
   rtn = sendCmdOpCondSD(mmcsd, 0x00ff8000, MMCSD_OP_TIMEOUT);
   
   // request all the cid info.
   rtn = sendCmd(mmcsd, MMCSD_CMD_ALL_SEND_CID, 0, 1);
   getCardResp(mmcsd, card_reg);

   // ask for suggested relative address.
   rtn = sendRcaSD(mmcsd, rca);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // select the card using the selected rca.
   rtn = sendCmd(mmcsd, MMCSD_CMD_SELECT_CARD, *rca << 16, 1);
   if (rtn != ERR_NO_ERROR)
      return (rtn);
   
   // set bus width to 4-bit mode.
   rtn = setBusWidth(mmcsd, *rca, in_bus_width);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // config the desired block length.
   rtn = sendCmd(mmcsd, MMCSD_CMD_SET_BLK_LEN, MMCSD_DEFAULT_BLKLEN, 0);
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   reads a block out of the sd card.
//
// \param   mmcsd_regs_t *mmcsd.
//
// \param   uint32_t in_block.
//
// \param   uint8_t *dest_buffer.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...data copied to dest_buffer.

//-----------------------------------------------------------------------------
uint32_t MMCSD_readBlock(mmcsd_regs_t *mmcsd, uint32_t in_block, uint32_t *dest_buffer)
{
   uint32_t rtn;
   uint32_t i, j, cnt;

   // reset fifo and config for receive.
   SETBIT(mmcsd->MMCFIFOCTL, FIFORST);
   USTIMER_delay(100);
   CLRBIT(mmcsd->MMCFIFOCTL, FIFODIR);
   
   // send the single block read command.
   rtn = sendCmd(mmcsd, 0x12000 | MMCSD_CMD_READ_SINGLE_BLOCK, in_block * MMCSD_DEFAULT_BLKLEN, 1);
   if (rtn != ERR_NO_ERROR)
      return (rtn);
   
   // read data from fifo.   
   for (i = 0; i < 8; i++)
   {
      // wait for data.
      cnt = 0;
      while (!CHKBIT(mmcsd->MMCST0, DRRDY))
      {
         if (cnt++ > RW_TIMEOUT)
         {
            // timed out waiting for data.
            return (ERR_TIMEOUT);
         }
      }
      
      // copy fifo data from reg...64 byte fifo -> 16 32-bit transfers.
      for (j = 0; j < 16; j++)
         *dest_buffer++ = mmcsd->MMCDRR;
   }
   
   // confirm the data transfer is complete.
   cnt = 0;
   while (!CHKBIT(mmcsd->MMCST0, DATDNE))
   {
      if (cnt++ > RW_TIMEOUT)
      {
         // timed out waiting for data.
         return (ERR_TIMEOUT);
      }
   }

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   writes a block the an sd card.
//
// \param   mmcsd_regs_t *mmcsd.
//
// \param   uint32_t in_block.
//
// \param   uint8_t *src_buffer.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...data written to sd card.

//-----------------------------------------------------------------------------
uint32_t MMCSD_writeBlock(mmcsd_regs_t *mmcsd, uint32_t in_block, uint32_t *src_buffer)
{
   uint32_t rtn;
   uint32_t i, j, cnt;
   
   // reset fifo and config for transmit.
   SETBIT(mmcsd->MMCFIFOCTL, FIFORST);
   USTIMER_delay(10);
   SETBIT(mmcsd->MMCFIFOCTL, FIFODIR);   
   
   rtn = sendCmd(mmcsd, 0x12800 | MMCSD_CMD_WRITE_SINGLE_BLOCK, in_block * MMCSD_DEFAULT_BLKLEN, 0);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // wait for transmit buffer to be ready.
   cnt = 0;
   while (!CHKBIT(mmcsd->MMCST0, DXRDY))
   {
      if (cnt++ > RW_TIMEOUT)
      {
         // timed out waiting for data.
         return (ERR_TIMEOUT);
      }
   }
   
   // write the data to the fifo.
   for (i = 0; i < 8; i++)
   {
      // copy data to fifo.
      for (j = 0; j < 16; j++)
         mmcsd->MMCDXR = *src_buffer++;
   }

   // confirm the data transfer is complete.
   cnt = 0;
   while (!CHKBIT(mmcsd->MMCST0, DATDNE))
   {
      if (cnt++ > RW_TIMEOUT)
      {
         // timed out waiting for data.
         return (ERR_TIMEOUT);
      }
   }
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// reads the sd response registers and returns them in an array.
//-----------------------------------------------------------------------------
void getCardResp(mmcsd_regs_t *mmcsd, uint16_t *card_resp)
{   
   card_resp[0] = (uint16_t)(mmcsd->MMCRSP01 & 0xFFFF0000) >> 16;
   card_resp[1] = (uint16_t)(mmcsd->MMCRSP01 & 0x0000FFFF);
   card_resp[2] = (uint16_t)(mmcsd->MMCRSP23 & 0xFFFF0000) >> 16;
   card_resp[3] = (uint16_t)(mmcsd->MMCRSP23 & 0x0000FFFF);
   card_resp[4] = (uint16_t)(mmcsd->MMCRSP45 & 0xFFFF0000) >> 16;
   card_resp[5] = (uint16_t)(mmcsd->MMCRSP45 & 0x0000FFFF);
   card_resp[6] = (uint16_t)(mmcsd->MMCRSP67 & 0xFFFF0000) >> 16;
   card_resp[7] = (uint16_t)(mmcsd->MMCRSP67 & 0x0000FFFF);
}

//-----------------------------------------------------------------------------
// set the sd card rca.
//-----------------------------------------------------------------------------
uint32_t sendRcaSD(mmcsd_regs_t *mmcsd, uint32_t *rca)
{
   uint32_t rtn;
   
   rtn = sendCmd(mmcsd, SD_CMD_SEND_REL_ADDR, 0, 1);
   if (rtn != ERR_NO_ERROR)
      return (rtn);
      
   *rca = mmcsd->MMCRSP67 >> 16;
   
   return (ERR_NO_ERROR);      
}

//-----------------------------------------------------------------------------
// sets the bus width for the sd interface.
//-----------------------------------------------------------------------------
uint32_t setBusWidth(mmcsd_regs_t *mmcsd, uint32_t rca, uint32_t in_bus_width)
{
   uint32_t rtn;
   uint32_t timeout;
   
   // verify input.
   if (BUSWIDTH_4BIT == in_bus_width)
   {
      in_bus_width = 2;
   }
   else if (BUSWIDTH_1BIT == in_bus_width)
   {
      in_bus_width = 1;
   }
   else
   {
      return (ERR_INVALID_PARAMETER);
   }
   
   for (timeout = 0; timeout < MMCSD_OP_TIMEOUT; timeout++)
   {
      rtn = sendCmd(mmcsd, MMCSD_CMD_APP, rca << 16, 1);
      if (rtn != ERR_NO_ERROR)
         return (rtn);

      rtn = sendCmd(mmcsd, SD_CMD_SET_BUS_WIDTH, in_bus_width, 1);
      if (rtn == ERR_NO_ERROR)
         break;
   }

   // set width to 1, for a 4-bit data bus - sd card.
   if (ERR_NO_ERROR == rtn)
   {
      if (0 == in_bus_width)
      {
         mmcsd->MMCCTL &= 0xFFFB;
      }
      else
      {
         mmcsd->MMCCTL |= 0x0004;
      }
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// send the operating conditions to sd card (voltage window).
//-----------------------------------------------------------------------------
uint32_t sendCmdOpCondSD(mmcsd_regs_t *mmcsd, uint32_t in_volt_win, uint32_t in_timeout)
{
   uint32_t rtn;
   
   while (in_timeout > 0)
   {
      rtn = sendCmd(mmcsd, SD_CMD_APP_OP_COND, in_volt_win, 1);
      if (rtn != ERR_NO_ERROR)
         return (rtn);
      
      if (CHKBIT(mmcsd->MMCRSP67, 0x80000000))
         return (ERR_NO_ERROR);
      
      rtn = sendCmd(mmcsd, MMCSD_CMD_APP, 0, 1);
      if (rtn != ERR_NO_ERROR)
         return (rtn);
      
      in_timeout--;
   }
   
   return (ERR_FAIL);
}

//-----------------------------------------------------------------------------
// sends a command to the card via mmc/sd controller.
//-----------------------------------------------------------------------------
uint32_t sendCmd(mmcsd_regs_t *mmcsd, uint32_t in_cmd, uint32_t in_arg, uint8_t check_status)
{
   uint32_t rtn = ERR_NO_ERROR;
   uint32_t stat_reg;
   uint32_t timeout_cntr = 0;
   
   // wait for command line to be idle.
   while (CHKBIT(mmcsd->MMCST1, MMCST1_BUSY)) {}
   
   // clear status regs.
   mmcsd->MMCRSP01 = 0;
   mmcsd->MMCRSP23 = 0;
   mmcsd->MMCRSP45 = 0;
   mmcsd->MMCRSP67 = 0;
   CLRBIT(mmcsd->MMCCIDX, CIDX_MASK);
   
   // set arg and cmd to send command to card.
   mmcsd->MMCARGHL = in_arg;
   mmcsd->MMCCMD = in_cmd;
   
   // delay a bit to give the card a chance to respond.
   USTIMER_delay(25);
   
   if (check_status)
   {
      while (1)
      {
         stat_reg = mmcsd->MMCST0;
         
         //
         if (CHKBIT(stat_reg, RSPDNE))
         {
            rtn = ERR_NO_ERROR;
            break;
         }
         else if (CHKBIT(stat_reg, TOUTRS))
         {
            rtn = ERR_MMCSD_TIMEOUT;
            break;
         }
         else
         {
            USTIMER_delay(25);
            timeout_cntr++;
         }
         
         if (timeout_cntr > RW_TIMEOUT)
         {
            rtn = ERR_MMCSD_TIMEOUT;
            break;
         }
      }
   }
   
   return (rtn);
}
