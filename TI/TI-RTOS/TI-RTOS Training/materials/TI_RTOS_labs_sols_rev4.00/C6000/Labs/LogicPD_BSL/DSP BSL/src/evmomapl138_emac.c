//-----------------------------------------------------------------------------
// \file    evmomapl138_emac.c
// \brief   implementation of the emac/mdio driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------

#include "stdio.h"
#include "string.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "ethernet_smsc.h"
#include "evmomapl138_emac.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_i2c_gpio.h"
#include "evmomapl138_cdce913.h"


//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

// mdio clock divide down value.
#define MDIO_CLK_FREQ_HZ   (2000000)
#define MDIO_CLK_DIVISOR   ((SYSCLOCK4_HZ / MDIO_CLK_FREQ_HZ) - 1)
// packet defines.
#define MAX_RX_BUFFERS     (10)
#define MIN_PACKET_SIZE    (46)
#define MAX_PACKET_SIZE    (1518)
#define PACKET_ALIGN       (18)
// rx / tx desriptor memory offsets.
#define RX_DESC_OFFSET     (0)
#define TX_DESC_OFFSET     (0x1000)
//MII pinmux
#define PINMUX_MII_REG_0         (2)
#define PINMUX_MII_MASK_0        (0xFFFFFFF0)
#define PINMUX_MII_VAL_0         (0x88888880)
#define PINMUX_MII_REG_1         (3)
#define PINMUX_MII_MASK_1        (0xFFFFFFFF)
#define PINMUX_MII_VAL_1         (0x88888888)
//RMII pinmux
#define PINMUX_RMII_REG_0        (14)
#define PINMUX_RMII_MASK_0       (0xFFFFFF00)
#define PINMUX_RMII_VAL_0        (0x88888800)
#define PINMUX_RMII_REG_1        (15)
#define PINMUX_RMII_MASK_1       (0x000000FF)
#define PINMUX_RMII_VAL_1        (0x00000080)
//MDIO pinmux
#define PINMUX_MDIO_REG          (4)
#define PINMUX_MDIO_MASK         (0x000000FF)
#define PINMUX_MDIO_VAL          (0x00000088)
//GPIO pinmux
#define PINMUX_MII_MDIO_EN_REG	 (6)
#define PINMUX_MII_MDIO_EN_MASK	 (0x000000F0)
#define PINMUX_MII_MDIO_EN_VAL	 (0x00000080)
//
#define EMAC_RMII_SPEED_100		 (0x00008000)
//-----------------------------------------------------------------------------
// Private Static Variables
//-----------------------------------------------------------------------------
static uint8_t g_active_phy_id = 0xFF;

static volatile emac_descriptor_t *g_rx_desc = (emac_descriptor_t *)(EMAC_RAM_BASE + RX_DESC_OFFSET);
static volatile emac_descriptor_t *g_tx_desc = (emac_descriptor_t *)(EMAC_RAM_BASE + TX_DESC_OFFSET);
static volatile emac_descriptor_t *g_rx_active_head = 0;
static volatile emac_descriptor_t *g_rx_active_tail = 0;
static uint8_t g_rx_queue_active = 0;
static uint8_t	g_rx_buffers[MAX_RX_BUFFERS * (MAX_PACKET_SIZE + PACKET_ALIGN)];

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static uint32_t initMdioPhy(void);
static uint8_t isLinkActive(uint8_t in_phy);
static uint16_t phyRegRead(uint8_t in_phy, uint8_t in_reg);
static void phyRegWrite(uint8_t in_phy, uint8_t in_reg, uint16_t in_data);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the EMAC and MDIO for use.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...emac ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t EMAC_init(emac_interface_e emac_interface)
{
   uint32_t rtn = 0;
   uint32_t i;
   volatile emac_descriptor_t *tmp_rx_desc;
   
   // reset emac module.
   EMAC->SOFTRESET = 1;
	while (EMAC->SOFTRESET != 0) {}
	
	// init emac module.
   //------------------
   
	// make sure emac control interrupts are disabled.
   EMAC_CTRL->C0RXTHRESHEN = 0;
   EMAC_CTRL->C1RXTHRESHEN = 0;
   EMAC_CTRL->C0RXEN = 0;
   EMAC_CTRL->C1RXEN = 0;
   EMAC_CTRL->C0TXEN = 0;
   EMAC_CTRL->C1TXEN = 0;
   EMAC_CTRL->C0MISCEN = 0;
   EMAC_CTRL->C1MISCEN = 0;
   

   // enable the psc and config pinmux for the desired interface.
   EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_EMAC, PSC_ENABLE); 
   EVMOMAPL138_pinmuxConfig(PINMUX_MDIO_REG, PINMUX_MDIO_MASK, PINMUX_MDIO_VAL); 					  //MDIO shared by both RMII and MII
   EVMOMAPL138_pinmuxConfig(PINMUX_MII_MDIO_EN_REG, PINMUX_MII_MDIO_EN_MASK, PINMUX_MII_MDIO_EN_VAL); //pinmux to select gpio bank2 pin6
   GPIO_setDir(GPIO_BANK2, GPIO_PIN6, GPIO_OUTPUT);     //needed to control the MII mido clock enable 
    	
   if (EMAC_INTERFACE_MII == emac_interface)
   {
   	  //PINMUXING
      EVMOMAPL138_pinmuxConfig(PINMUX_MII_REG_0, PINMUX_MII_MASK_0, PINMUX_MII_VAL_0);
      EVMOMAPL138_pinmuxConfig(PINMUX_MII_REG_1, PINMUX_MII_MASK_1, PINMUX_MII_VAL_1);
      
      SYSCONFIG->KICKR[0] = KICK0R_UNLOCK;
      SYSCONFIG->KICKR[1] = KICK1R_UNLOCK;
      CLRBIT(SYSCONFIG->CFGCHIP[3], 0x00000100);

	   //Enable the MII MDIO clock line
      GPIO_setOutput(GPIO_BANK2, GPIO_PIN6, OUTPUT_LOW);
   }
   else
   {
    	//PINMUXING
    	EVMOMAPL138_pinmuxConfig(PINMUX_RMII_REG_0, PINMUX_RMII_MASK_0, PINMUX_RMII_VAL_0);
    	EVMOMAPL138_pinmuxConfig(PINMUX_RMII_REG_1, PINMUX_RMII_MASK_1, PINMUX_RMII_VAL_1);

		SYSCONFIG->KICKR[0] = KICK0R_UNLOCK;
      	SYSCONFIG->KICKR[1] = KICK1R_UNLOCK;
      	SETBIT(SYSCONFIG->CFGCHIP[3], 0x00000100);
      	
		//BOARD CONFIGURATION
    		//disable camera, video, adc buffers to they dont interfere
    	I2CGPIO_setOutput(I2C_ADDR_GPIO_UI, I2C_GPIO_UI_SELA, 0);
    	I2CGPIO_setOutput(I2C_ADDR_GPIO_UI, I2C_GPIO_UI_SELB, 1);  
    	I2CGPIO_setOutput(I2C_ADDR_GPIO_UI, I2C_GPIO_UI_SELC, 1);  

		//Disable the MII MDIO clock line
    	GPIO_setOutput(GPIO_BANK2, GPIO_PIN6, OUTPUT_HIGH);
   }
   
   // setup local MAC address, only channel 0 is valid.
   // program all 8, only need to set MACADDRHI for index = 0.
   // use duplicate address for all unused channels.
   // TODO: read MAC address from SPI flash.
   EMAC->MACINDEX = 0;
   EMAC->MACADDRHI = 0x01020304;
   EMAC->MACADDRLO = 0x00000506 | MACADDRLO_VALID | MACADDRLO_MATCHFILT;
   for (i = 1; i < 8; i++)
   {
      EMAC->MACINDEX = i;
      EMAC->MACADDRLO = 0x00000506;
   }
   EMAC->MACSRCADDRHI = 0x01020304;
   EMAC->MACSRCADDRLO = 0x00000506;

	// init header descriptor pointer regs to 0.
   for (i = 0; i < 8; i++)
   {
      EMAC->TXHDP[i] = 0;
      EMAC->RXHDP[i] = 0;
   }
   
   // clear all network statistics registers.
#ifndef BOOT
   memset((uint8_t *)NET_STAT_REG_BASE, 0, NET_STAT_REG_NUM_BYTES);
#endif
   // initialize receive channel free buffer count regs, if buffer flow
   // control is to be enabled.
   // NOTE: this example does not use buffer flow control.
   
   // no multicast addresses, clear MAC address hash registers.
   EMAC->MACHASH1 = 0;
   EMAC->MACHASH2 = 0;
   
   // enable receive and transmit channel interrupts.
   EMAC->RXINTMASKSET = 1;
   EMAC->TXINTMASKSET = 1;
   
   // init receive buffer offset and max length.
   EMAC->RXBUFFEROFFSET = 0;
   EMAC->RXMAXLEN = MAX_PACKET_SIZE;
   
   // enable unicast chan 0 only.
   EMAC->RXUNICASTSET = 0x01;
   
   // config receive multicast/broadcast/promiscuous channel enable.
   EMAC->RXMBPENABLE = 0;
   SETBIT(EMAC->RXMBPENABLE, RXCAFEN);
   EMAC->RXMBPENABLE = 0x01E02020;		//enable reception of almost all frames inc error
   

   
   // initialize receive/transmit descriptor list queues.
   g_rx_active_head = tmp_rx_desc = g_rx_desc;
   for (i = 0; i < MAX_RX_BUFFERS; i++)
   {
      tmp_rx_desc->buffer = &g_rx_buffers[i * (MAX_PACKET_SIZE + PACKET_ALIGN)];
      tmp_rx_desc->buff_offset_length = MAX_PACKET_SIZE;
      tmp_rx_desc->flags_pkt_length = EMAC_DSC_FLAG_OWNER;
      
      if (i == (MAX_RX_BUFFERS - 1))
      {
         tmp_rx_desc->next = 0;
         g_rx_active_tail = tmp_rx_desc;
         g_rx_queue_active = 1;
      }
      else
      {
         tmp_rx_desc->next = (uint32_t)(tmp_rx_desc + 1);
         tmp_rx_desc++;
      }
   }
   
   // clear MAC, receive, and transmit control registers.
   EMAC->MACCONTROL |= EMAC_RMII_SPEED_100;
   EMAC->RXCONTROL = 0;
   EMAC->TXCONTROL = 0;

   // prepare for receive by writing pointer to head of receive buffer
   // desriptor list.
   EMAC->RXHDP[0] = (uint32_t)g_rx_desc;
   
   // enable receive / transmit DMA controllers...set GMIIEN.
   EMAC->RXCONTROL = 1;
   EMAC->TXCONTROL = 1;
   SETBIT(EMAC->MACCONTROL, GMIIEN | FULLDUPLEX);
   
   SETBIT(EMAC->EMCONTROL, SOFT);
   
   // init mdio / phy.
   //-----------------
   rtn = initMdioPhy();
   if (rtn != ERR_NO_ERROR)
      return (rtn);
   
   
   if (rtn)
      return (ERR_INIT_FAIL);
   else
      return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   receive a packet from the network.
//
// \param   uint8_t *dest_buffer - pointer to memory where data is to be copied.
//
// \param   uint32_t *length - number of bytes actually received.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...packet copied to dest_buffer.
//    ERR_INVALID_PARAMETER - null pointer.
//    ERR_ENET_Q_EMPTY - no new data to receive.
//    ERR_ENET_RX_ERROR - error found in packet.
//-----------------------------------------------------------------------------
uint32_t EMAC_rxPacket(uint8_t *dest_buffer, uint32_t *length)
{
   uint32_t rtn = ERR_NO_ERROR;
   uint32_t tmp_rx_flags;
   volatile emac_descriptor_t *tmp_rx_desc;
   volatile emac_descriptor_t *curr_desc;
   volatile emac_descriptor_t *tail_desc;

   if ((dest_buffer == NULL) || (length == NULL) || (g_rx_active_head == NULL))
   {
      return (ERR_INVALID_PARAMETER);
   }

   tmp_rx_desc = g_rx_active_head;
   tmp_rx_flags = tmp_rx_desc->flags_pkt_length;

      // check to see if we have packet wating for us
   if ((!CHKBIT(EMAC->RXINTSTATRAW, RX0PEND)) ||
         (CHKBIT(tmp_rx_flags, EMAC_DSC_FLAG_OWNER)))
   {
      return (ERR_ENET_Q_EMPTY);
   }
   
   if (CHKBIT(tmp_rx_flags, EMAC_DSC_FLAG_MASK_RX_ERROR))
   {
      // error in packet - discard it and requeue desc.
      rtn = ERR_ENET_RX_ERROR;
   }
   else
   {
      // packet is good...copy to input buffer.
      *length = tmp_rx_desc->buff_offset_length & 0xFFFF;
#ifndef BOOT
      memcpy(dest_buffer, tmp_rx_desc->buffer, *length);
#endif
   }
   
   // acknowledge to emac that we received the packet.
   EMAC->RXCP[0] = (uint32_t)tmp_rx_desc;
   curr_desc = tmp_rx_desc;
   g_rx_active_head = (volatile emac_descriptor_t *)tmp_rx_desc->next;

   if (CHKBIT(tmp_rx_flags, EMAC_DSC_FLAG_EOQ))
   {
      if (g_rx_active_head)
      {
         EMAC->RXHDP[0] = (uint32_t)g_rx_active_head;
      }
      else
      {
         g_rx_queue_active = 0;
      }
   }
   
   // recycle RX descriptor
   tmp_rx_desc->buff_offset_length = MAX_PACKET_SIZE;
   tmp_rx_desc->flags_pkt_length = EMAC_DSC_FLAG_OWNER;
   tmp_rx_desc->next = 0;

   if (g_rx_active_head == 0)
   {
      g_rx_active_head = curr_desc;
      g_rx_active_tail = curr_desc;
      if (g_rx_queue_active != 0)
      {
         EMAC->RXHDP[0] = (uint32_t)g_rx_active_head;
         g_rx_queue_active = 1;
      }
   }
   else
   {
      tail_desc = g_rx_active_tail;
      g_rx_active_tail = curr_desc;
      tail_desc->next = (uint32_t)curr_desc;
      tmp_rx_flags = tail_desc->flags_pkt_length;
      if (CHKBIT(tmp_rx_flags, EMAC_DSC_FLAG_EOQ))
      {
         EMAC->RXHDP[0] = (uint32_t)curr_desc;
         CLRBIT(tmp_rx_flags, EMAC_DSC_FLAG_EOQ);
         tail_desc->flags_pkt_length = tmp_rx_flags;
      }
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   transmit a packet on the network.
//
// \param   uint8_t *src_buffer - data to be transmitted.
//
// \param   uint32_t length - number of bytes to be transmitted.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...packet transmitted.
//    ERR_ENET_LINK_INACTIVE - link for active phy is not active.
//    ERR_ENET_PKT_TOO_LARGE - input length too large for buffer.
//-----------------------------------------------------------------------------
uint32_t EMAC_txPacket(uint8_t *src_buffer, uint32_t length)
{

   // check link before sending.
   if (!isLinkActive(g_active_phy_id))
   {
      return (ERR_ENET_LINK_INACTIVE);
   }
   
   // check the length of the packet.
   if (length > MAX_PACKET_SIZE)
   {
      // NOTE: in pratice, if a packet is too large for the buffer, you
      // should break it into multiple ethernet frames and transmit.
      return (ERR_ENET_PKT_TOO_LARGE);
   }
   else if (length < MIN_PACKET_SIZE)
   {
      // bump the length up to the min packet size.
      length = MIN_PACKET_SIZE;
   }
   
   // populate the tx descriptor.
   g_tx_desc->next = 0;
	g_tx_desc->buffer = src_buffer;
	g_tx_desc->buff_offset_length = (length & 0xFFFF);
	g_tx_desc->flags_pkt_length = ((length & 0xFFFF) |
                           			EMAC_DSC_FLAG_SOP |
                           			EMAC_DSC_FLAG_EOP |
                           			EMAC_DSC_FLAG_OWNER);

   // kick off the transmit
	EMAC->TXHDP[0] = (uint32_t)g_tx_desc;
	// wait for packet to complete.
	while (!CHKBIT(EMAC->TXINTSTATRAW, TX0PEND))
	{
	  // make sure the link is not broken.
     if (!isLinkActive(g_active_phy_id))
	  {
	     return (ERR_ENET_LINK_INACTIVE);
     }
   }
   
   // if we make it here, the transmit succeeded!
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   power on the phy.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...phy is on.
//    ERR_FAIL - something happened and could not power on.
//-----------------------------------------------------------------------------
uint32_t EMAC_phyPowerOn(void)
{
   uint32_t rtn;
   uint16_t ctrl_reg;

   ctrl_reg = phyRegRead(g_active_phy_id, SMSC_REG_BASIC_CTRL);

   if (!CHKBIT(ctrl_reg, BASIC_CTRL_POWER_DOWN))
   {
      // phy is already on...nothing left to do.
      #if DEBUG
      printf("phy powered, basic ctrl reg: %04x\n", ctrl_reg);
      #endif
      rtn = ERR_NO_ERROR;
   }
   else
   {
      // clear power down bit and write back to phy.
      CLRBIT(ctrl_reg, BASIC_CTRL_POWER_DOWN);
      phyRegWrite(g_active_phy_id, SMSC_REG_BASIC_CTRL, ctrl_reg);

      // short delay, then read the reg back to verify loopback is enabled.
      USTIMER_delay(500);
      ctrl_reg = phyRegRead(g_active_phy_id, SMSC_REG_BASIC_CTRL);
      if (!CHKBIT(ctrl_reg, BASIC_CTRL_POWER_DOWN))
      {
         // phy is powered...return success.
         #if DEBUG
         printf("phy powered, basic ctrl reg: %04x\n", ctrl_reg);
         #endif
         rtn = ERR_NO_ERROR;
      }
      else
      {
         // power down bit did not clear...return error.
         #if DEBUG
         printf("power down did not clear: %04x\n", ctrl_reg);
         #endif
         rtn = ERR_FAIL;
      }
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   power down the phy.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...phy is powered down.
//    ERR_FAIL - something happened and could not power down.
//-----------------------------------------------------------------------------
uint32_t EMAC_phyPowerDown(void)
{
   uint32_t rtn;
   uint16_t ctrl_reg;

   ctrl_reg = phyRegRead(g_active_phy_id, SMSC_REG_BASIC_CTRL);

   if(CHKBIT(ctrl_reg, BASIC_CTRL_ISOLATE) && CHKBIT(ctrl_reg, BASIC_CTRL_POWER_DOWN)) 
   {
      // phy is already powered down...nothing left to do.
      #if DEBUG
      printf("phy powered down, basic ctrl reg: %04x\n", ctrl_reg);
      #endif
      rtn = ERR_NO_ERROR;
   }
   else
   {
      // set power down bit and write back to phy.
      SETBIT(ctrl_reg, BASIC_CTRL_ISOLATE | BASIC_CTRL_POWER_DOWN);
      phyRegWrite(g_active_phy_id, SMSC_REG_BASIC_CTRL, ctrl_reg);

      // short delay, then read the reg back to verify loopback is disabled.
      USTIMER_delay(500);
      ctrl_reg = phyRegRead(g_active_phy_id, SMSC_REG_BASIC_CTRL);

      if(CHKBIT(ctrl_reg, BASIC_CTRL_ISOLATE) && CHKBIT(ctrl_reg, BASIC_CTRL_POWER_DOWN)) 
      {
         // phy is powered down...return success.
         #if DEBUG
         printf("phy powered down, basic ctrl reg: %04x\n", ctrl_reg);
         #endif
         rtn = ERR_NO_ERROR;
      }
      else
      {
         // power down bit did not set...return error.
         #if DEBUG
         printf("power down bit did not set: %04x\n", ctrl_reg);
         #endif
         rtn = ERR_FAIL;
      }
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   put the phy into loopback mode.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...phy is in loopback mode.
//    ERR_FAIL - something happened and could not enter loopback.
//-----------------------------------------------------------------------------
uint32_t EMAC_phyEnterLoopback(void)
{
   uint32_t rtn;
   uint16_t ctrl_reg;
 

   ctrl_reg = phyRegRead(g_active_phy_id, SMSC_REG_BASIC_CTRL);
   


   if (ctrl_reg & BASIC_CTRL_LOOPBACK)
   {
      // loopback is already enabled...nothing left to do.
      #if DEBUG
      printf("loopback enabled, basic ctrl reg: %04x\n", ctrl_reg);
      #endif
      rtn = ERR_NO_ERROR;
   }
   else
   {
      // set loopback bit and write back to phy.
      ctrl_reg |= BASIC_CTRL_LOOPBACK;
      phyRegWrite(g_active_phy_id, SMSC_REG_BASIC_CTRL, ctrl_reg);

      // short delay, then read the reg back to verify loopback is enabled.
      USTIMER_delay(500);
      ctrl_reg = phyRegRead(g_active_phy_id, SMSC_REG_BASIC_CTRL);
      if (ctrl_reg & BASIC_CTRL_LOOPBACK)
      {
         // loopback is enabled...return success.
         #if DEBUG
         printf("loopback enabled , basic ctrl reg: %04x\n", ctrl_reg);
         #endif
         rtn = ERR_NO_ERROR;
      }
      else
      {
         // loopback bit did not get set...return error.
         #if DEBUG
         printf("loopback did not set: %04x\n", ctrl_reg);
         #endif
         rtn = ERR_FAIL;
      }
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   remove the phy from loopback mode.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...phy is out of loopback mode.
//    ERR_FAIL - something happened and could not disable loopback.
//-----------------------------------------------------------------------------
uint32_t EMAC_phyExitLoopback(void)
{
   uint32_t rtn;
   uint16_t ctrl_reg;

   ctrl_reg = phyRegRead(g_active_phy_id, SMSC_REG_BASIC_CTRL);

   if (!(ctrl_reg & BASIC_CTRL_LOOPBACK))
   {
      // loopback is already disabled...nothing left to do.
      #if DEBUG
      printf("loopback disabled, basic ctrl reg: %04x\n", ctrl_reg);
      #endif
      rtn = ERR_NO_ERROR;
   }
   else
   {
      // clear loopback bit and write back to phy.
      ctrl_reg &= ~BASIC_CTRL_LOOPBACK;
      phyRegWrite(g_active_phy_id, SMSC_REG_BASIC_CTRL, ctrl_reg);

      // short delay, then read the reg back to verify loopback is disabled.
      USTIMER_delay(5);
      ctrl_reg = phyRegRead(g_active_phy_id, SMSC_REG_BASIC_CTRL);
      if (!(ctrl_reg & BASIC_CTRL_LOOPBACK))
      {
         // loopback is disabled...return success.
         #if DEBUG
         printf("loopback disabled , basic ctrl reg: %04x\n", ctrl_reg);
         #endif
         rtn = ERR_NO_ERROR;
      }
      else
      {
         // loopback bit did not clear...return error.
         #if DEBUG
         printf("loopback did not clear: %04x\n", ctrl_reg);
         #endif
         rtn = ERR_FAIL;
      }
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// initialize the mdio module and identify the active phy.
//-----------------------------------------------------------------------------
uint32_t initMdioPhy(void)
{
   uint32_t i;
   uint16_t phy_reg;

   // init the mdio regs.
   MDIO->CONTROL = MDIO_CTRL_ENABLE |
                     MDIO_CTRL_FAULT |
                     MDIO_CTRL_FAULTENB |
                     MDIO_CLK_DIVISOR;
   while (CHKBIT(MDIO->CONTROL, MDIO_CTRL_IDLE)) {}

   // look for an active phy...takes up to 50 us for each phy to be checked.
   for (i = 0; i < 32; i++)
   {
      if (MDIO->ALIVE)
      {
         // at least one phy has acknowledged us...break the loop.
         break;
      }
      USTIMER_delay(50);
   }

   if (MDIO->ALIVE)
   {
      #if DEBUG
      printf("found an active phy: 0x%08X\n\n", MDIO->ALIVE);
      #endif
   }
   else
   {
      #if DEBUG
      printf("error connecting to phy!\n");
      #endif
      return (ERR_INIT_FAIL);
   }

   // identify which phy is active.
   for (i = 0; i < 32; i++)
   {
      if (CHKBIT(MDIO->ALIVE, bitval_u32[i]))
      {
         if (CHKBIT(MDIO->ALIVE, ~bitval_u32[i]) && CHKBIT(MDIO->ALIVE, ~bitval_u32[3]))
         {
            // more than one phy connected!
            #if DEBUG
            printf("more than one phy connected: 0x%08X\r\n", MDIO->LINK);
            #endif
            g_active_phy_id = 0xFF;

            return (ERR_INIT_FAIL);
         }
         else
         {
            g_active_phy_id = i;
            break;
         }
      }
   }

   MDIO->USERPHYSEL0 = 0;
   
   // disable all auto functionality for loopback.
   phy_reg = phyRegRead(g_active_phy_id, 27);
   SETBIT(phy_reg, bitval_u32[15]);
   phyRegWrite(g_active_phy_id, 27, phy_reg);
   
   phy_reg = 0x2100;
   phyRegWrite(g_active_phy_id, 0, phy_reg);
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// returns if the link is currently active...1 -> active, 0 -> not active.
//-----------------------------------------------------------------------------
uint8_t isLinkActive(uint8_t in_phy)
{
   uint16_t status;

   status = phyRegRead(in_phy, SMSC_REG_BASIC_STAT);
   
   if (CHKBIT(status, BASIC_STAT_LINK_STAT))
      return (1);
   else
      return (0);
}

//-----------------------------------------------------------------------------
// read a phy register using the MDIO.
//-----------------------------------------------------------------------------
uint16_t phyRegRead(uint8_t in_phy, uint8_t in_reg)
{
   // make sure mdio is not busy.
   while (CHKBIT(MDIO->USERACCESS0, USERACC_GO)) {}

   MDIO->USERACCESS0 = USERACC_GO |
                        (in_reg << USERACC_SHIFT_REG) |
                        (in_phy << USERACC_SHIFT_PHY);

   while (CHKBIT(MDIO->USERACCESS0, USERACC_GO)) {}

   return ((uint16_t)(MDIO->USERACCESS0 & USERACC_MASK_DATA));
}

//-----------------------------------------------------------------------------
// write a phy register using the MDIO.
//-----------------------------------------------------------------------------
void phyRegWrite(uint8_t in_phy, uint8_t in_reg, uint16_t in_data)
{
   // make sure mdio is not busy.
   while (CHKBIT(MDIO->USERACCESS0, USERACC_GO)) {}

   MDIO->USERACCESS0 = USERACC_GO |
                        USERACC_WRITE |
                        (in_reg << USERACC_SHIFT_REG) |
                        (in_phy << USERACC_SHIFT_PHY) |
                        in_data;
}
