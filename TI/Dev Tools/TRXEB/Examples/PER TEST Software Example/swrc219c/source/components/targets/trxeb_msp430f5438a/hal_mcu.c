/***********************************************************************************
  Filename:     hal_mcu.c

  Description:  hal mcu implementation file for trxeb

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/

#include <msp430.h>
#include <hal_types.h>
#include "hal_mcu.h"


/***********************************************************************************
 * CONSTANTS AND DEFINES
 */
#define DCO_MULT_1MHZ           30
#define DCO_MULT_4MHZ           122
#define DCO_MULT_8MHZ           244
#define DCO_MULT_12MHZ          366
#define DCO_MULT_16MHZ          488
#define DCO_MULT_20MHZ          610
#define DCO_MULT_25MHZ          763

#define DCORSEL_1MHZ            DCORSEL_2
#define DCORSEL_4MHZ            DCORSEL_4
#define DCORSEL_8MHZ            DCORSEL_4
#define DCORSEL_12MHZ           DCORSEL_5
#define DCORSEL_16MHZ           DCORSEL_5
#define DCORSEL_20MHZ           DCORSEL_6
#define DCORSEL_25MHZ           DCORSEL_7

#define VCORE_1MHZ              PMMCOREV_0
#define VCORE_4MHZ              PMMCOREV_0
#define VCORE_8MHZ              PMMCOREV_0
#define VCORE_12MHZ             PMMCOREV_1
#define VCORE_16MHZ             PMMCOREV_1
#define VCORE_20MHZ             PMMCOREV_2
#define VCORE_25MHZ             PMMCOREV_3

#define VCORE_1_35V             PMMCOREV_0
#define VCORE_1_55V             PMMCOREV_1
#define VCORE_1_75V             PMMCOREV_2
#define VCORE_1_85V             PMMCOREV_3

/***********************************************************************************
 * LOCAL FUNCTIONS
 */

static void halMcuSetVCoreUp(unsigned char level);
static void halMcuSetVCoreDown(unsigned char level);
static void halMcuGetSystemClockSettings(unsigned char systemClockSpeed,
                                         unsigned char *setDcoRange,
                                         unsigned char *setVCore,
                                         unsigned int  *setMultiplier);
static void halMcuSetVCore(unsigned char level);
static void halMcuDisableSVS(void);
static void halMcuEnableSVS(void);
static void halMcuOutputSystemClock(void);
static void halMcuStopOutputSystemClock(void);

/***********************************************************************************
 * LOCAL VARIABLES
 */
/* NOTE: variable holds the syctem clock speed set by a call to halMcuSetSystemClock */
static uint8 systemClock;

/*****************************************************************************
 * @fn          halMcuSetVCoreUp
 *
 * @brief       Increments the VCore setting.
 *
 * input parameters
 *
 * @param       level  - The target VCore setting
 *
 * output parameters
 *
 * @return      none
 */
static void halMcuSetVCoreUp (unsigned char level)
{
    /* Open PMM module registers for write access */
    PMMCTL0_H = 0xA5;

    /* Set SVS/M high side to new level */
    SVSMHCTL = (SVSMHCTL & ~(SVSHRVL0*3 + SVSMHRRL0)) |
        (SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level);

    /* Set SVM new Level */
    SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
    /* Set SVS/M low side to new level */
    SVSMLCTL = (SVSMLCTL & ~(SVSMLRRL_3)) | (SVMLE + SVSMLRRL0 * level);

    while ((PMMIFG & SVSMLDLYIFG) == 0);      /* Wait till SVM is settled (Delay) */
    PMMCTL0_L = PMMCOREV0 * level;            /* Set VCore to x                   */
    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);        /* Clear already set flags          */

    if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);       /* Wait till level is reached       */

    /* Set SVS/M Low side to new level  */
    SVSMLCTL = (SVSMLCTL & ~(SVSLRVL0*3 + SVSMLRRL_3)) |
        (SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level);

    /* Lock PMM module registers from write access  */
    PMMCTL0_H = 0x00;
}

/*****************************************************************************
 * @fn          halMcuSetVCoreDown
 *
 * @brief       Decrements the VCore setting.
 *
 * input parameters
 *
 * @param       level  - The target VCore.
 *
 * ouput parameters
 *
 * @return      none
 */
static void halMcuSetVCoreDown(unsigned char level)
{
/* Open PMM module registers for write access */
PMMCTL0_H = 0xA5;

/* Set SVS/M low side to new level            */
SVSMLCTL = (SVSMLCTL & ~(SVSLRVL0*3 + SVSMLRRL_3)) |
    (SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level);

while ((PMMIFG & SVSMLDLYIFG) == 0);      /* Wait till SVM is settled (Delay) */
PMMCTL0_L = (level * PMMCOREV0);          /* Set VCore to new level           */
/* Lock PMM module registers for write access  */

PMMCTL0_H = 0x00;
}

/*****************************************************************************
 * @fn          halMcuGetSystemClockSettings
 *
 * @brief       Get function for the DCORSEL, VCORE, and DCO multiplier
 *              settings that map to a given clock speed.
 *
 * input parameters
 *
 * @param       systemClockSpeed  - Target DCO frequency - HAL_MCU_SYSCLK_xxMHZ
 * @param       setDcoRange       - Pointer to the DCO range select bits.
 * @param       setVCore          - Pointer to the VCore level bits.
 * @param       setMultiplier     - Pointer to the DCO multiplier bits.
 *
 * output parameters
 *
 * @return      none
 */
static void halMcuGetSystemClockSettings(unsigned char systemClockSpeed,
                                           unsigned char *setDcoRange,
                                           unsigned char *setVCore,
                                           unsigned int  *setMultiplier)
{
    switch (systemClockSpeed)
    {
        case HAL_MCU_SYSCLK_1MHZ:
        *setDcoRange = DCORSEL_1MHZ;
        *setVCore = VCORE_1MHZ;
        *setMultiplier = DCO_MULT_1MHZ;
        break;
        case HAL_MCU_SYSCLK_4MHZ:
        *setDcoRange = DCORSEL_4MHZ;
        *setVCore = VCORE_4MHZ;
        *setMultiplier = DCO_MULT_4MHZ;
        break;
        case HAL_MCU_SYSCLK_8MHZ:
        *setDcoRange = DCORSEL_8MHZ;
        *setVCore = VCORE_8MHZ;
        *setMultiplier = DCO_MULT_8MHZ;
        break;
        case HAL_MCU_SYSCLK_12MHZ:
        *setDcoRange = DCORSEL_12MHZ;
        *setVCore = VCORE_12MHZ;
        *setMultiplier = DCO_MULT_12MHZ;
        break;
        case HAL_MCU_SYSCLK_16MHZ:
        *setDcoRange = DCORSEL_16MHZ;
        *setVCore = VCORE_16MHZ;
        *setMultiplier = DCO_MULT_16MHZ;
        break;
        case HAL_MCU_SYSCLK_20MHZ:
        *setDcoRange = DCORSEL_20MHZ;
        *setVCore = VCORE_20MHZ;
        *setMultiplier = DCO_MULT_20MHZ;
        break;
        case HAL_MCU_SYSCLK_25MHZ:
        *setDcoRange = DCORSEL_25MHZ;
        *setVCore = VCORE_25MHZ;
        *setMultiplier = DCO_MULT_25MHZ;
        break;
    }
}


/*****************************************************************************
 * @fn          halMcuSetVCore
 *
 * @brief       Set function for the PMM core voltage (PMMCOREV) setting
 *
 * input parameters
 *
 * @param       level  - Target VCore setting
 *
 * output parameters
 *
 * @return      none
 */
static void halMcuSetVCore(unsigned char level)
{
 unsigned char actLevel;
 do {
   actLevel = PMMCTL0_L & PMMCOREV_3;
   if (actLevel < level)
   halMcuSetVCoreUp(++actLevel);       /* Set VCore (step by step) */
   if (actLevel > level)
   halMcuSetVCoreDown(--actLevel);     /* Set VCore (step by step) */
 }while (actLevel != level);
}

/*****************************************************************************
 * @fn          halMcuDisableSVS
 *
 * @brief       Disables all supply voltage supervision and monitoring.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
static void halMcuDisableSVS(void)
{
    /* Open PMM module registers for write access   */
    PMMCTL0_H = 0xA5;

    SVSMLCTL &= ~( SVMLE + SVSLE + SVSLFP + SVMLFP ); /* Disable Low side SVM   */
    SVSMHCTL &= ~( SVMHE + SVSHE + SVSHFP + SVMHFP ); /* Disable High side SVM  */
    PMMCTL1 = PMMREFMD;

    /* Lock PMM module registers for write access  */
    PMMCTL0_H = 0x00;
}

/*****************************************************************************
 * @fn          halMcuEnableSVS
 *
 * @brief       Enables all supply voltage supervision and monitoring
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
static void halMcuEnableSVS(void)
{
    /* Open PMM module registers for write access  */
    PMMCTL0_H = 0xA5;

    /*
     * NOTE: To attain the expected < 6 us wakeup from LPM modes, the following
     * two lines must be commented out due to the fact that the PMM will hold
     * the CPU until the reference is fully settled.
     */
    SVSMHCTL &= ~(SVSHFP+SVMHFP);             /* Disable full-performance mode  */
    SVSMLCTL &= ~(SVSLFP+SVMLFP);             /* Disable full-performance mode  */
    SVSMLCTL |= ( SVMLE + SVSLE);             /* Enable Low side SVM            */
    SVSMHCTL |= ( SVMHE + SVSHE);             /* Enable High side SVM           */
    PMMCTL1 &= ~PMMREFMD;

    /* Lock PMM module registers for write access */
    PMMCTL0_H = 0x00;
}

/*****************************************************************************
 * @fn          halMcuStartXT1
 *
 * @brief       Initialization routine for XT1. Sets the necessary internal
 *              capacitor values and loops until all ocillator fault flags
 *              remain cleared. XT1 is in Low Power Mode
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halMcuStartXT1(void)
{
    /* Set up XT1 Pins to analog function, and to lowest drive */
    P7SEL |= 0x03;
    UCSCTL6 |= XCAP_3 ;                       /* Set internal cap values */

    while(SFRIFG1 & OFIFG) {                  /* Check OFIFG fault flag  */
        while ( (SFRIFG1 & OFIFG))            /* Check OFIFG fault flag  */
        {
            /* Clear OSC fault flags  */
            UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT1HFOFFG + XT2OFFG);
            SFRIFG1 &= ~OFIFG;                    /* Clear OFIFG fault flag  */
        }
        UCSCTL6 &= ~(XT1DRIVE1_L+XT1DRIVE0);    /* Reduce the drive strength */
    }
}

/*****************************************************************************
 * @fn          halMcuSetSystemClock
 *
 * @brief       Set function for MCLK frequency. Also selects XT1 as ACLK
 *              source with no divison in Low Power mode
 *
 * input parameters
 *
 * @param       systemClockSpeed  - Intended frequency of operation
 *
 * output parameters
 *
 * @return      none
 */
void halMcuSetSystemClock(unsigned char systemClockSpeed)
{
    unsigned char setDcoRange, setVCore;
    unsigned int  setMultiplier;

    /* Storing the wanted system clock speed */
    systemClock =  systemClockSpeed;
    halMcuGetSystemClockSettings( systemClockSpeed, &setDcoRange, &setVCore, &setMultiplier);

    halMcuSetVCore( setVCore );

    __bis_SR_register(SCG0);                  /* Disable the FLL control loop   */
    UCSCTL0 = 0x00;                           /* Set lowest possible DCOx, MODx */
    UCSCTL1 = setDcoRange;                    /* Select suitable range          */

    UCSCTL2 = setMultiplier + FLLD_1;         /* Set DCO Multiplier */
    UCSCTL4 = SELA__XT1CLK | SELS__DCOCLKDIV  |  SELM__DCOCLKDIV ;
    __bic_SR_register(SCG0);                  /* Enable the FLL control loop */

    /* Loop until XT1,XT2 & DCO fault flag is cleared    */
    do
    {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
        /* Clear XT2,XT1,DCO fault flags  */
        SFRIFG1 &= ~OFIFG;                    /* Clear fault flags           */
    }while (SFRIFG1&OFIFG);                   /* Test oscillator fault flag  */

    /* Worst-case settling time for the DCO when the DCO range bits have been
     * changed is n x 32 x 32 x f_FLL_reference. See UCS chapter in 5xx UG
     * for optimization.
     * 32 x 32 x / f_FLL_reference (32,768 Hz) = .03125 = t_DCO_settle
     * t_DCO_settle / (1 / 25 MHz) = 781250 = counts_DCO_settle
     */
    __delay_cycles(781250);
}

/*****************************************************************************
 * @fn          halMcuOutputSystemClock
 *
 * @brief       Initializes ACLK, MCLK, SMCLK outputs on P11.0, P11.1,
 *              and P11.2, respectively.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
static void halMcuOutputSystemClock(void)
{
    P11DIR |= 0x07;
    P11SEL |= 0x07;
}

/*****************************************************************************
 * @fn          halMcuStopOutputSystemClock
 *
 * @brief       Stops the output of ACLK, MCLK, SMCLK on P11.0, P11.1, and
 *              P11.2.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
static void halMcuStopOutputSystemClock(void)
{
    P11OUT &= ~0x07;
    P11DIR |= 0x07;
    P11SEL &= ~0x07;
}

/*****************************************************************************
 * @fn          halMcuDisablePeripheralClockRequest
 *
 * @brief       Disables peripheral clock request according to bitMask passed
 *
 * input parameters
 *
 * @param       bitMask  - according to UCSCTL8
 *
 * output parameters
 *
 * @return      none
 */
void halMcuDisablePeripheralClockRequest(uint16 bitMask)
{
  UCSCTL8 &= ~bitMask;
}

/*****************************************************************************
 * @fn          halMcuGetSystemClock
 *
 * @brief       Returns the current system clock frequency
 *
 * input parameters
 *
 * output parameters
 *
 * @return      system clock frequency
 */
uint8 halMcuGetSystemClock(void)
{
	return systemClock;
}


/***********************************************************************************
* @fn          halMcuWaitUs
*
* @brief       Busy wait function. Waits the specified number of microseconds. Use
*              assumptions about number of clock cycles needed for the various
*              instructions. The duration of one cycle depends on MCLK. In this HAL
*              , it is set to 8 MHz, thus 8 cycles per usec.
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint16 usec - number of microseconds delay
*
* @return      none
*/
#pragma optimize=none
void halMcuWaitUs(uint16 usec) // 5 cycles for calling
{
    // The least we can wait is 3 usec:
    // ~1 usec for call, 1 for first compare and 1 for return
    while(usec > 3)       // 2 cycles for compare
    {                // 2 cycles for jump
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        usec -= 2;        // 1 cycles for optimized decrement
    }
}                         // 4 cycles for returning


/***********************************************************************************
* @fn          halMcuWaitMs
*
* @brief       Busy wait function. Waits the specified number of milliseconds. Use
*              assumptions about number of clock cycles needed for the various
*              instructions.
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint16 millisec - number of milliseconds delay
*
* @return      none
*/
#pragma optimize=none
void halMcuWaitMs(uint16 msec)
{
    while(msec-- > 0)
    {
        halMcuWaitUs(1000);
    }
}


/***********************************************************************************
* @fn          halMcuSetLowPowerMode
*
* @brief      Sets the MCU in a low power mode. Will turn global interrupts on at
*             the same time as entering the LPM mode. The MCU must be waken from
*             an interrupt (status register on stack must be modified).
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint8 mode - power mode
*
* @return      none
*/
void halMcuSetLowPowerMode(uint8 mode)
{
    switch (mode)
    {
    case HAL_MCU_LPM_0:
        __low_power_mode_0();
        break;
    case HAL_MCU_LPM_1:
        __low_power_mode_1();
        break;
    case HAL_MCU_LPM_2:
        __low_power_mode_2();
        break;
    case HAL_MCU_LPM_3:
        __low_power_mode_3();
        break;
    case HAL_MCU_LPM_4:
        __low_power_mode_4();
        break;
    }
}

/***********************************************************************************
* @fn          halMcuReset
*
* @brief       MCU soft reset.
*
* @param       none
*
* @return      none
*/
void halMcuReset(void)
{
    void (*pf)(void)= 0;
    (*pf)();
}


/***********************************************************************************
  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/
