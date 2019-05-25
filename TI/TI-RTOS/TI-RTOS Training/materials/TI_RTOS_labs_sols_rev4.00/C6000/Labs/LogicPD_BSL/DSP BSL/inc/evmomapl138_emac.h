//-----------------------------------------------------------------------------
// \file    evmomapl138_emac.h
// \brief   OMAP-L138 emac, emac control, and mdio registers, bit definitions,
//          and function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_EMAC_H
#define EVMOMAPL138_EMAC_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct 
{
   volatile uint32_t REVID;           // 0x0000
   volatile uint32_t SOFTRESET;       // 0x0004
   volatile uint32_t RSVD0;           // 0x0008
   volatile uint32_t INTCTL;          // 0x000C
   volatile uint32_t C0RXTHRESHEN;    // 0x0010
   volatile uint32_t C0RXEN;          // 0x0014
   volatile uint32_t C0TXEN;          // 0x0018
   volatile uint32_t C0MISCEN;        // 0x001C
   volatile uint32_t C1RXTHRESHEN;    // 0x0020
   volatile uint32_t C1RXEN;          // 0x0024
   volatile uint32_t C1TXEN;          // 0x0028
   volatile uint32_t C1MISCEN;        // 0x002C
   volatile uint32_t C2RXTHRESHEN;    // 0x0030
   volatile uint32_t C2RXEN;          // 0x0034
   volatile uint32_t C2TXEN;          // 0x0038
   volatile uint32_t C2MISCEN;        // 0x003C
   volatile uint32_t C0RXTHRESHSTAT;  // 0x0040
   volatile uint32_t C0RXSTAT;        // 0x0044
   volatile uint32_t C0TXSTAT;        // 0x0048
   volatile uint32_t C0MISCSTAT;      // 0x004C
   volatile uint32_t C1RXTHRESHSTAT;  // 0x0050
   volatile uint32_t C1RXSTAT;        // 0x0054
   volatile uint32_t C1TXSTAT;        // 0x0058
   volatile uint32_t C1MISCSTAT;      // 0x005C
   volatile uint32_t C2RXTHRESHSTAT;  // 0x0060
   volatile uint32_t C2RXSTAT;        // 0x0064
   volatile uint32_t C2TXSTAT;        // 0x0068
   volatile uint32_t C2MISCSTAT;      // 0x006C
   volatile uint32_t C0RXIMAX;        // 0x0070
   volatile uint32_t C0TXIMAX;        // 0x0074
   volatile uint32_t C1RXIMAX;        // 0x0078
   volatile uint32_t C1TXIMAX;        // 0x007C
   volatile uint32_t C2RXIMAX;        // 0x0080
   volatile uint32_t C2TXIMAX;        // 0x0084
} emac_ctrl_regs_t;

#define EMAC_CTRL    ((emac_ctrl_regs_t *)EMAC_CTRL_REG_BASE)

typedef struct
{
   volatile uint32_t TXREVID;            // 0x0000
   volatile uint32_t TXCONTROL;          // 0x0004
   volatile uint32_t TXTEARDOWN;         // 0x0008
   volatile uint32_t RSVD0;              // 0x000C
   volatile uint32_t RXREVID;            // 0x0010
   volatile uint32_t RXCONTROL;          // 0x0014
   volatile uint32_t RXTEARDOWN;         // 0x0018
   volatile uint32_t RSVD1[25];          // 0x001C
   volatile uint32_t TXINTSTATRAW;       // 0x0080
   volatile uint32_t TXINTSTATMASKED;    // 0x0084
   volatile uint32_t TXINTMASKSET;       // 0x0088
   volatile uint32_t TXINTMASKCLEAR;     // 0x008C
   volatile uint32_t MACINVECTOR;        // 0x0090
   volatile uint32_t MACEOIVECTOR;       // 0x0094
   volatile uint32_t RSVD2[2];           // 0x0098
   volatile uint32_t RXINTSTATRAW;       // 0x00A0
   volatile uint32_t RXINTSTATMASKED;    // 0x00A4
   volatile uint32_t RXINTMASKSET;       // 0x00A8
   volatile uint32_t RXINTMASKCLEAR;     // 0x00AC
   volatile uint32_t MACINTSTATRAW;      // 0x00B0
   volatile uint32_t MACINTSTATMASKED;   // 0x00B4
   volatile uint32_t MACINTMASKSET;      // 0x00B8
   volatile uint32_t MACINTMASKCLR;      // 0x00BC
   volatile uint32_t RSVD3[16];          // 0x00C0
   volatile uint32_t RXMBPENABLE;        // 0x0100
   volatile uint32_t RXUNICASTSET;       // 0x0104
   volatile uint32_t RXUNICASTCLR;       // 0x0108
   volatile uint32_t RXMAXLEN;           // 0x010C
   volatile uint32_t RXBUFFEROFFSET;     // 0x0110
   volatile uint32_t RXFILTERLOWTHRESH;  // 0x0114
   volatile uint32_t RSVD4[2];           // 0x0118
   volatile uint32_t RX0FLOWTHRESH;      // 0x0120
   volatile uint32_t RX1FLOWTHRESH;      // 0x0124
   volatile uint32_t RX2FLOWTHRESH;      // 0x0128
   volatile uint32_t RX3FLOWTHRESH;      // 0x012C
   volatile uint32_t RX4FLOWTHRESH;      // 0x0130
   volatile uint32_t RX5FLOWTHRESH;      // 0x0134
   volatile uint32_t RX6FLOWTHRESH;      // 0x0138
   volatile uint32_t RX7FLOWTHRESH;      // 0x013C
   volatile uint32_t RX0FREEBUFFER;      // 0x0140
   volatile uint32_t RX1FREEBUFFER;      // 0x0144
   volatile uint32_t RX2FREEBUFFER;      // 0x0148
   volatile uint32_t RX3FREEBUFFER;      // 0x014C
   volatile uint32_t RX4FREEBUFFER;      // 0x0150
   volatile uint32_t RX5FREEBUFFER;      // 0x0154
   volatile uint32_t RX6FREEBUFFER;      // 0x0158
   volatile uint32_t RX7FREEBUFFER;      // 0x015C
   volatile uint32_t MACCONTROL;         // 0x0160
   volatile uint32_t MACSTATUS;          // 0x0164
   volatile uint32_t EMCONTROL;          // 0x0168
   volatile uint32_t FIFOCONTROL;        // 0x016C
   volatile uint32_t MACCONFIG;          // 0x0170
   volatile uint32_t SOFTRESET;          // 0x0174
   volatile uint32_t RSVD5[22];          // 0x0178
   volatile uint32_t MACSRCADDRLO;       // 0x01D0
   volatile uint32_t MACSRCADDRHI;       // 0x01D4
   volatile uint32_t MACHASH1;           // 0x01D8
   volatile uint32_t MACHASH2;           // 0x01DC
   volatile uint32_t BOFFTEST;           // 0x01E0
   volatile uint32_t TPACETEST;          // 0x01E4
   volatile uint32_t RXPAUSE;            // 0x01E8
   volatile uint32_t TXPAUSE;            // 0x01EC
   volatile uint32_t RSVD6[4];           // 0x01F0
   volatile uint32_t RXGOODFRAMES;       // 0x0200
   volatile uint32_t RXBCASTFRAMES;      // 0x0204
   volatile uint32_t RXMCASTFRAMES;      // 0x0208
   volatile uint32_t RXPAUSEFRAMES;      // 0x020C
   volatile uint32_t RXCRCERRORS;        // 0x0210
   volatile uint32_t RXALIGNCODEERRORS;  // 0x0214
   volatile uint32_t RXOVERSIZED;        // 0x0218
   volatile uint32_t RXJABBER;           // 0x021C
   volatile uint32_t RXUNDERSIZED;       // 0x0220
   volatile uint32_t RXFRAGMENTS;        // 0x0224
   volatile uint32_t RXFILTERED;         // 0x0228
   volatile uint32_t RXQOSFILTERED;      // 0x022C
   volatile uint32_t RXOCTETS;           // 0x0230
   volatile uint32_t TXGOODFRAMES;       // 0x0234
   volatile uint32_t TXBCASTFRAMES;      // 0x0238
   volatile uint32_t TXMCASTFRAMES;      // 0x023C
   volatile uint32_t TXPAUSEFRAMES;      // 0x0240
   volatile uint32_t TXDEFFERRED;        // 0x0244
   volatile uint32_t TXCOLLISION;        // 0x0248
   volatile uint32_t TXSINGLECOLL;       // 0x024C
   volatile uint32_t TXMULTICOLL;        // 0x0250
   volatile uint32_t TXEXCESSIVECOLL;    // 0x0254
   volatile uint32_t TXLATECOLL;         // 0x0258
   volatile uint32_t TXUNDERRUN;         // 0x025C
   volatile uint32_t TXCARRIERSENSE;     // 0x0260
   volatile uint32_t TXOCTETS;           // 0x0264
   volatile uint32_t FRAME64;            // 0x0268
   volatile uint32_t FRAME65T127;        // 0x026C
   volatile uint32_t FRAME128T255;       // 0x0270
   volatile uint32_t FRAME256T511;       // 0x0274
   volatile uint32_t FRAME512T1023;      // 0x0278
   volatile uint32_t FRAME1024TUP;       // 0x027C
   volatile uint32_t NETOCTETS;          // 0x0280
   volatile uint32_t RXSOFOVERRUNS;      // 0x0284
   volatile uint32_t RXMOFOVERRUNS;      // 0x0288
   volatile uint32_t RXDMAOVERRUNS;      // 0x028C
   volatile uint32_t RSVD7[156];         // 0x0290
   volatile uint32_t MACADDRLO;          // 0x0500
   volatile uint32_t MACADDRHI;          // 0x0504
   volatile uint32_t MACINDEX;           // 0x0508
   volatile uint32_t RSVD8[61];          // 0x050C
   volatile uint32_t TXHDP[8];           // 0x0600
   volatile uint32_t RXHDP[8];           // 0x0620
   volatile uint32_t TXCP[8];            // 0x0640
   volatile uint32_t RXCP[8];            // 0x0660
} emac_regs_t;

#define EMAC    ((emac_regs_t *)EMAC_REG_BASE)

// network statitics registers defines.
#define NET_STAT_REG_OFFSET      (0x00000200)
#define NET_STAT_REG_BASE        (EMAC_REG_BASE + NET_STAT_REG_OFFSET)
#define NET_STAT_REG_NUM_BYTES   (36 * 4)

// *XINTSTATRAW bitmask defines.
#define TX0PEND            (0x00000001)   // bit 0
#define RX0PEND            (0x00000001)   // bit 0

// RXMBPENABLE bitmask defines.
#define RXPASSCRC          (0x40000000)   // bit 30
#define RXQOSEN            (0x20000000)   // bit 29
#define RXNOCHAIN          (0x10000000)   // bit 28
#define RXCMFEN            (0x01000000)   // bit 24
#define RXCSFEN            (0x00800000)   // bit 23
#define RXCEFEN            (0x00400000)   // bit 22
#define RXCAFEN            (0x00200000)   // bit 21
#define RXBROADEN          (0x00002000)   // bit 13
#define RXMULTEN           (0x00000020)   // bit 5
#define RXPROMCH_SHIFT     (16)
#define RXPBROADCH_SHIFT   (8)
#define RXMULTCH_SHIFT     (0)

// MACCONTROL bitmask defines.
#define RMIISPEED          (0x00008000)   // bit 15
#define RXOFFLENBLOCK      (0x00004000)   // bit 14
#define RXOWNERSHIP        (0x00002000)   // bit 13
#define CMDIDLE            (0x00000800)   // bit 11
#define TXSHORTGAPEN       (0x00000400)   // bit 10
#define TXPTYPE            (0x00000200)   // bit 9
#define TXPACE             (0x00000040)   // bit 6
#define GMIIEN             (0x00000020)   // bit 5
#define TXFLOWEN           (0x00000010)   // bit 4
#define RXBUFFERFLOWEN     (0x00000008)   // bit 3
#define LOOPBACK           (0x00000002)   // bit 1
#define FULLDUPLEX         (0x00000001)   // bit 0

// MACADDRLO bitmask defines.
#define MACADDRLO_VALID       (0x00100000)   // bit 20
#define MACADDRLO_MATCHFILT   (0x00080000)   // bit 19

typedef struct
{
   volatile uint32_t REVID;               // 0x0000
   volatile uint32_t CONTROL;             // 0x0004
   volatile uint32_t ALIVE;               // 0x0008
   volatile uint32_t LINK;                // 0x000C
   volatile uint32_t LINKINTRAW;          // 0x0010
   volatile uint32_t LINKINTMASKED;       // 0x0014
   volatile uint32_t RSVD0[2];            // 0x0018
   volatile uint32_t USERINTRAW;          // 0x0020
   volatile uint32_t USERINTMASKED;       // 0x0024
   volatile uint32_t USERINTMASKSET;      // 0x0028
   volatile uint32_t USERINTMASKCLR;      // 0x002C
   volatile uint32_t RSVD1[20];           // 0x0030
   volatile uint32_t USERACCESS0;         // 0x0080
   volatile uint32_t USERPHYSEL0;         // 0x0084
   volatile uint32_t USERACCESS1;         // 0x0088
   volatile uint32_t USERPHYSEL1;         // 0x008C
} mdio_regs_t;

#define MDIO    ((mdio_regs_t *)MDIO_REG_BASE)

// CONTROL bitmask defines.
#define MDIO_CTRL_IDLE           (0x80000000)   // bit 31
#define MDIO_CTRL_ENABLE         (0x40000000)   // bit 30
#define MDIO_CTRL_PREAMBLE       (0x00100000)   // bit 20
#define MDIO_CTRL_FAULT          (0x00080000)   // bit 19
#define MDIO_CTRL_FAULTENB       (0x00040000)   // bit 18

// USERACCESS0 bitmask defines.
#define USERACC_GO               (0x80000000)   // bit 31
#define USERACC_WRITE            (0x40000000)   // bit 30
#define USERACC_ACK              (0x20000000)   // bit 29
#define USERACC_MASK_DATA        (0x0000FFFF)   // bit 0-15
#define USERACC_SHIFT_REG        (21)
#define USERACC_SHIFT_PHY        (16)

// descriptor flag defines.
#define EMAC_DSC_FLAG_SOP           (0x80000000)   // bit 31
#define EMAC_DSC_FLAG_EOP           (0x40000000)   // bit 30
#define EMAC_DSC_FLAG_OWNER         (0x20000000)   // bit 29
#define EMAC_DSC_FLAG_EOQ           (0x10000000)   // bit 28
#define EMAC_DSC_FLAG_TDOWNCMPLT    (0x08000000)   // bit 27
#define EMAC_DSC_FLAG_PASSCRC       (0x04000000)   // bit 26
#define EMAC_DSC_FLAG_JABBER        (0x02000000)   // bit 25
#define EMAC_DSC_FLAG_OVERSIZE      (0x01000000)   // bit 24
#define EMAC_DSC_FLAG_FRAGMENT      (0x00800000)   // bit 23
#define EMAC_DSC_FLAG_UNDERSIZED    (0x00400000)   // bit 22
#define EMAC_DSC_FLAG_CONTROL       (0x00200000)   // bit 21
#define EMAC_DSC_FLAG_OVERRUN       (0x00100000)   // bit 20
#define EMAC_DSC_FLAG_CODEERROR     (0x00080000)   // bit 19
#define EMAC_DSC_FLAG_ALIGNERROR    (0x00040000)   // bit 18
#define EMAC_DSC_FLAG_CRCERROR      (0x00020000)   // bit 17
#define EMAC_DSC_FLAG_NOMATCH       (0x00010000)   // bit 16
#define EMAC_DSC_FLAG_MASK_RX_ERROR (0x03FC0000)

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------
typedef struct _emac_descriptor_t
{
   uint32_t next;
   uint8_t *buffer;
   uint32_t buff_offset_length;
   uint32_t flags_pkt_length;
} emac_descriptor_t;

typedef enum
{
   EMAC_INTERFACE_MII = 0,
   EMAC_INTERFACE_RMII
} emac_interface_e;

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t EMAC_init(emac_interface_e emac_interface);
uint32_t EMAC_rxPacket(uint8_t *dest_buffer, uint32_t *length);
uint32_t EMAC_txPacket(uint8_t *src_buffer, uint32_t length);
uint32_t EMAC_phyPowerOn(void);
uint32_t EMAC_phyPowerDown(void);
uint32_t EMAC_phyEnterLoopback(void);
uint32_t EMAC_phyExitLoopback(void);


#endif


