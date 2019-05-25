//-----------------------------------------------------------------------------
// \file    ethernet_smsc.h
// \brief   SMSC LAN 87x bit definitions.
//
//-----------------------------------------------------------------------------

#ifndef ETHERNET_SMSC_H
#define ETHERNET_SMSC_H

//-----------------------------------------------------------------------------
// Public Defines
//-----------------------------------------------------------------------------

// SMSC LAN87x register defines.
#define SMSC_REG_BASIC_CTRL         (0)
#define SMSC_REG_BASIC_STAT         (1)
#define SMSC_REG_PHYID1             (2)
#define SMSC_REG_PHYID2             (3)

// basic control bitmask defines.
#define BASIC_CTRL_SOFTRESET     (0x8000)   // bit 15
#define BASIC_CTRL_LOOPBACK      (0x4000)   // bit 14
#define BASIC_CTRL_FORCE_100     (0x2000)   // bit 13
#define BASIC_CTRL_AN_ENABLE     (0x1000)   // bit 12
#define BASIC_CTRL_POWER_DOWN    (0x0800)   // bit 11
#define BASIC_CTRL_ISOLATE       (0x0400)   // bit 10
#define BASIC_CTRL_RESTART_AN    (0x0200)   // bit 9
#define BASIC_CTRL_FORCE_FULL    (0x0100)   // bit 8
#define BASIC_CTRL_COLL_TEST     (0x0080)   // bit 7

// basic status bitmask defines.
#define BASIC_STAT_100T4         (0x8000)   // bit 15
#define BASIC_STAT_100TX_FULL    (0x4000)   // bit 14
#define BASIC_STAT_100TX_HALF    (0x2000)   // bit 13
#define BASIC_STAT_10T_FULL      (0x1000)   // bit 12
#define BASIC_STAT_10T_HALF      (0x0800)   // bit 11
#define BASIC_STAT_AUTO_COMP     (0x0020)   // bit 5
#define BASIC_STAT_REMOTE_FAULT  (0x0010)   // bit 4
#define BASIC_STAT_AUTO_ABLE     (0x0008)   // bit 3
#define BASIC_STAT_LINK_STAT     (0x0004)   // bit 2
#define BASIC_STAT_JABBER        (0x0002)   // bit 1
#define BASIC_STAT_EXT           (0x0001)   // bit 0

#endif


