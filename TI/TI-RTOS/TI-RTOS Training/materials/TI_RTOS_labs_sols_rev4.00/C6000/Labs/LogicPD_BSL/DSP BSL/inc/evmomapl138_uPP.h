//-----------------------------------------------------------------------------
// \file    evmomapl138_uPP.h
// \brief   OMAP-L138 uPP registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_UPP_H
#define EVMOMAPL138_UPP_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------

//bits UPPCR
#define  UPP_UPPCR_SWRST    0x00000010


#ifdef FREE
	#undef FREE
#endif 

#ifdef SOFT
	#undef SOFT
#endif 

//registers
typedef struct
{
  volatile uint32_t UPPID;        // 0x0000
  volatile uint32_t UPPCR;        // 0x0004
  volatile uint32_t UPDLB;        // 0x0008
  volatile uint32_t NA1;           // 0x000c not accessable
  volatile uint32_t UPCTL;        // 0x0010
  volatile uint32_t UPICR;        // 0x0014
  volatile uint32_t UPIVR;        // 0x0018
  volatile uint32_t UPTCR;        // 0x001c
  volatile uint32_t UPISR;        // 0x0020
  volatile uint32_t UPIER;        // 0x0024
  volatile uint32_t UPIES;        // 0x0028
  volatile uint32_t UPIEC;        // 0x002c
  volatile uint32_t UPEOI;        // 0x0030
  volatile uint32_t NA2;           // 0x0034 not accessable
  volatile uint32_t NA3;           // 0x0038 not accessable
  volatile uint32_t NA4;          // 0x003c not accessable
  volatile uint32_t UPID0;        // 0x0040
  volatile uint32_t UPID1;        // 0x0044
  volatile uint32_t UPID2;        // 0x0048
  volatile uint32_t NA5;           // 0x004c not accessable
  volatile uint32_t UPIS0;        // 0x0050
  volatile uint32_t UPIS1;        // 0x0054
  volatile uint32_t UPIS2;        // 0x0058
  volatile uint32_t NA6;           // 0x005c not accessable
  volatile uint32_t UPQD0;        // 0x0060
  volatile uint32_t UPQD1;        // 0x0064
  volatile uint32_t UPQD2;        // 0x0068
  volatile uint32_t NA7;           // 0x007c not accessable
  volatile uint32_t UPQS0;        // 0x0070
  volatile uint32_t UPQS1;        // 0x0074
  volatile uint32_t UPQS2;        // 0x0078
} uPP_regs_t;

typedef struct
{
  //UPCTL
  union
  {
    struct 
    {
      unsigned MODE       :2;
      unsigned CHN        :1;
      unsigned SDRTXIL    :1;
      unsigned DDRDEMUX   :1;
      unsigned RESERVED3  :11;
      unsigned DRA        :1;
      unsigned IWA        :1;
      unsigned DPWA       :3;
      unsigned DPFA       :2;
      unsigned RESERVED2  :1;
      unsigned DRB        :1;
      unsigned IWB        :1;
      unsigned DPWB       :3;
      unsigned DPFB       :2;
      unsigned RESERVED1  :1;
    }bits; 
    uint32_t value;
  }UPCTL;
  
  //UPPCR
  union
  {
      struct
      {
      
        unsigned FREE         :1;
        unsigned SOFT         :1;
        unsigned RTEMU        :1;
        unsigned EN           :1;
        unsigned SWRST        :1;
        unsigned RESERVED2    :2;
        unsigned DB           :1;
        unsigned RESERVED1    :24;        
      }bits;  
      uint32_t value;
  }UPPCR;
  
  //UPDLB
  union 
  {
    struct
    {
      unsigned RESERVED2      :12;
      unsigned AB             :1;
      unsigned BA             :1;
      unsigned RESERVED1      :18;
    }bits;
    uint32_t value;   
  }UPDLB;
  
  //UPICR
  union
  {
    struct
    {
      unsigned STARTPOLA      :1;
      unsigned ENAPOLA        :1;
      unsigned WAITPOLA       :1;
      unsigned STARTA         :1;
      unsigned ENAA           :1;
      unsigned WAITA          :1;
      unsigned RESERVED4      :2;
      unsigned CLKDIVA        :4;
      unsigned CLKINVA        :1;
      unsigned TRISA          :1;
      unsigned RESERVED3      :2;
      unsigned STARTPOLB       :1;
      unsigned ENAPOLB         :1;
      unsigned WAITPOLB       :1;
      unsigned STARTB         :1;
      unsigned ENAB           :1;
      unsigned WAITB          :1;
      unsigned RESERVED2      :2;
      unsigned CLKDIVB        :4;
      unsigned CLKINVB        :1;
      unsigned TRISB          :1;
      unsigned RESERVED1      :2;
    }bits;
    uint32_t value;
  }UPICR;
  
  //UPIVR
  union
  {
    struct
    {
      unsigned VALA            :16;
      unsigned VALB            :16;
    }bits;
    uint32_t value;    
  }UPIVR;

  //UPTCR
  union
  {
    struct
    {
      unsigned RDSIZEI        :2;
      unsigned RESERVED4      :6;
      unsigned RDSIZEQ        :2;
      unsigned RESERVED3      :6;
      unsigned TXSIZEA        :2;
      unsigned RESERVED2      :6;
      unsigned TXSIZEB        :2;
      unsigned RESERVED1      :6;
    }bits;
    uint32_t value;
  }UPTCR;
  
  //UPIES
  union
  {
    struct
    {
      unsigned DPEI           :1;
      unsigned UORI           :1;
      unsigned ERRI           :1;
      unsigned EOWI           :1;
      unsigned EOLI           :1;
      unsigned RESERVED2      :3;
      unsigned DPEQ           :1;
      unsigned UORQ           :1;
      unsigned ERRQ           :1;
      unsigned EOWQ           :1; 
      unsigned EOLQ           :1;
      unsigned RESERVED1      :19;
    } bits;
    uint32_t value;
  }UPIES;
} upp_config_t;

//UPISR_t
typedef union
{
  struct
  {
    unsigned DPEI           :1;
    unsigned UORI           :1;
    unsigned ERRI           :1;
    unsigned EOWI           :1;
    unsigned EOLI           :1;
    unsigned RESERVED2      :3;
    unsigned DPEQ           :1;
    unsigned UORQ           :1;
    unsigned ERRQ           :1;
    unsigned EOWQ           :1; 
    unsigned EOLQ           :1;
    unsigned RESERVED1      :19;
  } bits;
  uint32_t value;
}UPISR_t;

typedef union
{
	struct
	{
		unsigned ACT		:1;		
		unsigned PEND		:1;
		unsigned RESERVED2	:2;
		unsigned WM			:4;
		unsigned RESERVED1	:24;
	}bits;
	uint32_t value;
}UPXS2_t;

#define UPP  ((uPP_regs_t *)UPP_REG_BASE)

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t UPP_init(upp_config_t * config);


#endif

