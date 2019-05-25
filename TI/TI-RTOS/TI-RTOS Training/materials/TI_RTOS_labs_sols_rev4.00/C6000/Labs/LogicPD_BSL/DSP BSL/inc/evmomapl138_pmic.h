//-----------------------------------------------------------------------------
// \file    evmomapl138_pmic.h
// \brief   OMAP-L138 pmic definitions and function prototypes.
//          driver for TI TPS65070 PMIC.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_PMIC_H
#define EVMOMAPL138_PMIC_H

//-----------------------------------------------------------------------------
// Public Defines and Macros
//-----------------------------------------------------------------------------

// register defines for TI TPS65070.
#define PPATH1                (0x01)
#define INT                   (0x02)
#define CHGCONFIG0            (0x03)
#define CHGCONFIG1            (0x04)
#define CHGCONFIG2            (0x05)
#define CHGCONFIG3            (0x06)
#define ADCONFIG              (0x07)
#define TSCMODE               (0x08)
#define ADRESULT1             (0x09)
#define ADRESULT2             (0x0A)
#define PGOODZ                (0x0B)
#define PGOODMASK             (0x0C)
#define CONCTRL1              (0x0D)
#define CONCTRL2              (0x0E)
#define CONCTRL3              (0x0F)
#define DEFDCDC1              (0x10)
#define DEFDCDC2_LOW          (0x11)
#define DEFDCDC2_HIGH         (0x12)
#define DEFDCDC3_LOW          (0x13)
#define DEFDCDC3_HIGH         (0x14)
#define DEFSLEW               (0x15)
#define LDOCTRL1              (0x16)
#define DEFLD02               (0x17)
#define WLEDCTRL1             (0x18)
#define WLEDCTRL2             (0x19)
//#define EN_3P3_1P8_IO		  (1)	 //uncomment this to give the board ability run i/o at 1.8v
//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t PMIC_init(void);
uint32_t PMIC_readRegister(uint8_t in_reg_addr, uint8_t *dest_buffer);
uint32_t PMIC_writeRegister(uint8_t in_reg_addr, uint8_t in_data);
uint32_t PMIC_writeStartupRegs(void);
uint32_t PMIC_readPrintRegs(void);

#endif
