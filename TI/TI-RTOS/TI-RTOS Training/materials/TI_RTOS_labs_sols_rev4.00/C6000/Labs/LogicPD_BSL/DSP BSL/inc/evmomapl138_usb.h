//-----------------------------------------------------------------------------
// \file    evmomapl138_usb.h
// \brief   OMAP-L138 usb host and otg registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_USB_H
#define EVMOMAPL138_USB_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------

// usb host register structure
typedef struct
{
   volatile uint32_t HCREVISION;             // 0x0000
   volatile uint32_t HCCONTROL;              // 0x0004
   volatile uint32_t HCCOMMANDSTATUS;        // 0x0008
   volatile uint32_t HCINTERRUPTSTATUS;      // 0x000C
   volatile uint32_t HCINTERRUPTENABLE;      // 0x0010
   volatile uint32_t HCINTERRUPTDISABLE;     // 0x0014
   volatile uint32_t HCHCCA;                 // 0x0018
   volatile uint32_t HCPERIODCURRENTED;      // 0x001C
   volatile uint32_t HCCONTROLHEADED;        // 0x0020
   volatile uint32_t HCCONTROLCURRENTED;     // 0x0024
   volatile uint32_t HCBULKHEADED;           // 0x0028
   volatile uint32_t HCBULKCURRENTED;        // 0x002C
   volatile uint32_t HCDONEHEAD;             // 0x0030
   volatile uint32_t HCFMINTERVAL;           // 0x0034
   volatile uint32_t HCFMREMAINING;          // 0x0038
   volatile uint32_t HCFMNUMBER;             // 0x003C
   volatile uint32_t HCPERIODICSTART;        // 0x0040
   volatile uint32_t HCLSTHRESHOLD;          // 0x0044
   volatile uint32_t HCRHDESCRIPTORA;        // 0x0048
   volatile uint32_t HCRHDESCRIPTORB;        // 0x004C
   volatile uint32_t HCRHSTATUS;             // 0x0050
   volatile uint32_t HCRHPORTSTATUS1;        // 0x0054
   volatile uint32_t HCRHPORTSTATUS2;        // 0x0058
} usb_host_regs_t;

#define USB_HOST           ((usb_host_regs_t *) USB_HOST_REG_BASE)

// usb otg registers
typedef struct
{
   volatile uint32_t REVID;            // 0x0000
   volatile uint32_t CTRLR;            // 0x0004
   volatile uint32_t STATR;            // 0x0008
   volatile uint32_t EMUR;             // 0x000C
   volatile uint32_t MODE;             // 0x0010
   volatile uint32_t AUTOREQ;          // 0x0014
   volatile uint32_t SRPFIXTIME;       // 0x0018
   volatile uint32_t TEARDOWN;         // 0x001C
   volatile uint32_t INTSRCR;          // 0x0020
   volatile uint32_t INTSETR;          // 0x0024
   volatile uint32_t INTCLRR;          // 0x0028
   volatile uint32_t INTMSKR;          // 0x002C
   volatile uint32_t INTMSKSETR;       // 0x0030
   volatile uint32_t INTMSKCLRR;       // 0x0034
   volatile uint32_t INTMASKEDR;       // 0x0038
   volatile uint32_t EOIR;             // 0x003C
   volatile uint32_t INTVECTR;         // 0x0040
   volatile uint32_t RSVD0[3];         // 0x0044
   volatile uint32_t GENRNDISSZ1;      // 0x0050
   volatile uint32_t GENRNDISSZ2;      // 0x0054
   volatile uint32_t GENRNDISSZ3;      // 0x0058
   volatile uint32_t GENRNDISSZ4;      // 0x005C
   volatile uint32_t RSVD1[232];       // 0x0060
   volatile uint8_t  FADDR;            // 0x0400
   volatile uint8_t  POWER;            // 0x0401
   volatile uint16_t INTRTX;           // 0x0402
   volatile uint16_t INTRRX;           // 0x0404
   volatile uint16_t INTRTXE;          // 0x0406
   volatile uint16_t INTRRXE;          // 0x0408
   volatile uint8_t  INTRUSB;          // 0x040A
   volatile uint8_t  INTRUSBE;         // 0x040B
   volatile uint16_t FRAME;            // 0x040C
   volatile uint8_t  INDEX;            // 0x040E
   volatile uint8_t  TESTMODE;         // 0x040F
   volatile uint16_t TXMAXP;           // 0x0410
   volatile uint16_t CSR;              // 0x0412
   volatile uint16_t RXMAXP;           // 0x0414
   volatile uint16_t RXCSR;            // 0x0416
   volatile uint16_t COUNT0;           // 0x0418
   volatile uint8_t  HOST_TYPE0;       // 0x041A
   volatile uint8_t  HOST_NAKLIM0;     // 0x041B
   volatile uint8_t  HOST_RXTYPE;      // 0x041C
   volatile uint16_t HOST_RXINTERVAL;  // 0x041D
   volatile uint8_t  CONFIGDATA;       // 0x041F
   volatile uint32_t FIFO0;            // 0x0420
   volatile uint32_t FIFO1;            // 0x0424
   volatile uint32_t FIFO2;            // 0x0428
   volatile uint32_t FIFO3;            // 0x042C
   volatile uint32_t FIFO4;            // 0x0430
   volatile uint32_t RSVD2[10];        // 0x0434
   volatile uint8_t DEVCTL;			   // 0x0460
} usb_otg_regs_t;

#define USB_OTG_DEVCTL    				*( volatile uint8_t* )( 0x01e00460 )

#define USB_OTG           				((usb_otg_regs_t *) USB_OTG_REG_BASE)

#define USB_OTG_POWER_HSEN				0x20
#define USB_OTG_POWER_ENSUSPM			0x01
#define USB_OTG_POWER_SOFTCONN			0x40

#define USB_OTG_DEVCTL_SESSION			0x01

#define INTRUSB_SESSREQ					0x40
#define INTSRCR_SESSREQ					(INTRUSB_SESSREQ << 16)

#define DEVCTL_DEV_TYPE					0x80
#define DEVCTL_FS_DEV					0x40
#define DEVCTL_LS_DEV					0x20
#define DEVCTL_SESSION					0x01

#define VBUS_ABOVE_VBUS_VALID_MASK		0x18

#define POST_SESSION_SET_VALUE			0x81

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t USB_init(void);
uint8_t USB_getHostRevision(void);
void USB_OTG_init();

#endif
