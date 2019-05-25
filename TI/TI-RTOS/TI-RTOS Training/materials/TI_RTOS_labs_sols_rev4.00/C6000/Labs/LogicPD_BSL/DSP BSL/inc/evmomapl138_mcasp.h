//-----------------------------------------------------------------------------
// \file    evmomapl138_mcasp.h
// \brief   OMAP-L138 mcasp registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_MCASP_H
#define EVMOMAPL138_MCASP_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
   volatile uint32_t REVID;         // 0x0000
   volatile uint32_t RSVD0[3];      // 0x0004
   volatile uint32_t PFUNC;         // 0x0010
   volatile uint32_t PDIR;          // 0x0014
   volatile uint32_t PDOUT;         // 0x0018
   volatile uint32_t PDIN;          // 0x001C
   //volatile uint32_t PDSET;         // 0x001C
   volatile uint32_t PDCLR;         // 0x0020
   volatile uint32_t RSVD1[8];      // 0x0024
   volatile uint32_t GBLCTL;        // 0x0044
   volatile uint32_t AMUTE;         // 0x0048
   volatile uint32_t DLBCTL;        // 0x004C
   volatile uint32_t DITCTL;        // 0x0050
   volatile uint32_t RSVD2[3];      // 0x0054
   volatile uint32_t RGBLCTL;       // 0x0060
   volatile uint32_t RMASK;         // 0x0064
   volatile uint32_t RFMT;          // 0x0068
   volatile uint32_t AFSRCTL;       // 0x006C
   volatile uint32_t ACLKRCTL;      // 0x0070
   volatile uint32_t AHCLKRCTL;     // 0x0074
   volatile uint32_t RTDM;          // 0x0078
   volatile uint32_t RINTCTL;       // 0x007C
   volatile uint32_t RSTAT;         // 0x0080
   volatile uint32_t RSLOT;         // 0x0084
   volatile uint32_t RCLKCHK;       // 0x0088
   volatile uint32_t REVTCTL;       // 0x008C
   volatile uint32_t RSVD3[4];      // 0x0090
   volatile uint32_t XGBLCTL;       // 0x00A0
   volatile uint32_t XMASK;         // 0x00A4
   volatile uint32_t XFMT;          // 0x00A8
   volatile uint32_t AFSXCTL;       // 0x00AC
   volatile uint32_t ACLKXCTL;      // 0x00B0
   volatile uint32_t AHCLKXCTL;     // 0x00B4
   volatile uint32_t XTDM;          // 0x00B8
   volatile uint32_t XINTCTL;       // 0x00BC
   volatile uint32_t XSTAT;         // 0x00C0
   volatile uint32_t XSLOT;         // 0x00C4
   volatile uint32_t XCLKCHK;       // 0x00C8
   volatile uint32_t XEVTCTL;       // 0x00CC
   volatile uint32_t RSVD4[12];     // 0x00D0
   volatile uint32_t DITCSRA0;      // 0x0100
   volatile uint32_t DITCSRA1;      // 0x0104
   volatile uint32_t DITCSRA2;      // 0x0108
   volatile uint32_t DITCSRA3;      // 0x010C
   volatile uint32_t DITCSRA4;      // 0x0110
   volatile uint32_t DITCSRA5;      // 0x0114
   volatile uint32_t DITCSRB0;      // 0x0118
   volatile uint32_t DITCSRB1;      // 0x011C
   volatile uint32_t DITCSRB2;      // 0x0120
   volatile uint32_t DITCSRB3;      // 0x0124
   volatile uint32_t DITCSRB4;      // 0x0128
   volatile uint32_t DITCSRB5;      // 0x012C
   volatile uint32_t DITUDRA0;      // 0x0130
   volatile uint32_t DITUDRA1;      // 0x0134
   volatile uint32_t DITUDRA2;      // 0x0138
   volatile uint32_t DITUDRA3;      // 0x013C
   volatile uint32_t DITUDRA4;      // 0x0140
   volatile uint32_t DITUDRA5;      // 0x0144
   volatile uint32_t DITUDRB0;      // 0x0148
   volatile uint32_t DITUDRB1;      // 0x014C
   volatile uint32_t DITUDRB2;      // 0x0150
   volatile uint32_t DITUDRB3;      // 0x0154
   volatile uint32_t DITUDRB4;      // 0x0158
   volatile uint32_t DITUDRB5;      // 0x015C
   volatile uint32_t RSVD5[8];      // 0x0160
   volatile uint32_t SRCTL0;        // 0x0180
   volatile uint32_t SRCTL1;        // 0x0184
   volatile uint32_t SRCTL2;        // 0x0188
   volatile uint32_t SRCTL3;        // 0x018C
   volatile uint32_t SRCTL4;        // 0x0190
   volatile uint32_t SRCTL5;        // 0x0194
   volatile uint32_t SRCTL6;        // 0x0198
   volatile uint32_t SRCTL7;        // 0x019C
   volatile uint32_t SRCTL8;        // 0x01A0
   volatile uint32_t SRCTL9;        // 0x01A4
   volatile uint32_t SRCTL10;       // 0x01A8
   volatile uint32_t SRCTL11;       // 0x01AC
   volatile uint32_t SRCTL12;       // 0x01B0
   volatile uint32_t SRCTL13;       // 0x01B4
   volatile uint32_t SRCTL14;       // 0x01B8
   volatile uint32_t SRCTL15;       // 0x01BC
   volatile uint32_t RSVD6[16];     // 0x01C0
   volatile uint32_t XBUF0;         // 0x0200
   volatile uint32_t XBUF1;         // 0x0204
   volatile uint32_t XBUF2;         // 0x0208
   volatile uint32_t XBUF3;         // 0x020C
   volatile uint32_t XBUF4;         // 0x0210
   volatile uint32_t XBUF5;         // 0x0214
   volatile uint32_t XBUF6;         // 0x0218
   volatile uint32_t XBUF7;         // 0x021C
   volatile uint32_t XBUF8;         // 0x0220
   volatile uint32_t XBUF9;         // 0x0224
   volatile uint32_t XBUF10;        // 0x0228
   volatile uint32_t XBUF11;        // 0x022C
   volatile uint32_t XBUF12;        // 0x0230
   volatile uint32_t XBUF13;        // 0x0234
   volatile uint32_t XBUF14;        // 0x0238
   volatile uint32_t XBUF15;        // 0x023C
   volatile uint32_t RSVD7[16];     // 0x0240
   volatile uint32_t RBUF0;         // 0x0280
   volatile uint32_t RBUF1;         // 0x0284
   volatile uint32_t RBUF2;         // 0x0288
   volatile uint32_t RBUF3;         // 0x028C
   volatile uint32_t RBUF4;         // 0x0290
   volatile uint32_t RBUF5;         // 0x0294
   volatile uint32_t RBUF6;         // 0x0298
   volatile uint32_t RBUF7;         // 0x029C
   volatile uint32_t RBUF8;         // 0x02A0
   volatile uint32_t RBUF9;         // 0x02A4
   volatile uint32_t RBUF10;        // 0x02A8
   volatile uint32_t RBUF11;        // 0x02AC
   volatile uint32_t RBUF12;        // 0x02B0
   volatile uint32_t RBUF13;        // 0x02B4
   volatile uint32_t RBUF14;        // 0x02B8
   volatile uint32_t RBUF15;        // 0x02BC
} mcasp_regs_t;

#define MCASP        ((mcasp_regs_t *)MCASP_REG_BASE)

// bitmask defines for GBLCTL.
#define XFRST        (0x00001000)   // bit 12
#define XSMRST       (0x00000800)   // bit 11
#define XSRCLR       (0x00000400)   // bit 10
#define XHCLKRST     (0x00000200)   // bit 9
#define XCLKRST      (0x00000100)   // bit 8
#define RFRST        (0x00000010)   // bit 4
#define RSMRST       (0x00000008)   // bit 3
#define RSRCLR       (0x00000004)   // bit 2
#define RHCLKRST     (0x00000002)   // bit 1
#define RCLKRST      (0x00000001)   // bit 0

// bitmask defines for SRCTL.
#define RRDY         (0x00000020)   // bit 5
#define XRDY         (0x00000010)   // bit 4

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t MCASP_init(void);

#endif

