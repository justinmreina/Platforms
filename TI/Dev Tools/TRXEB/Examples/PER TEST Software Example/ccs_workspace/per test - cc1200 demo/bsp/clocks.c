#include "clocks.h"


void clocks_init(void) {

    //  Set capacitor values for XT1, 32768 Hz */
//----------------------------------------------------------------------------------------------------------------------------------//
// @src     bspMcuStartXT1();
    // Set up XT1 Pins to analog function, and to lowest drive
    P7SEL |= 0x03;

    // Set internal cap values
    UCSCTL6 |= XCAP_3 ;

    // Check OFIFG fault flag
    while(SFRIFG1 & OFIFG) {
        // Check OFIFG fault flag
        while(SFRIFG1 & OFIFG) {
            // Clear OSC fault flags and OFIFG fault flag
            UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT1HFOFFG + XT2OFFG);
            SFRIFG1 &= ~OFIFG;
        }

        // Reduce the drive strength
        UCSCTL6 &= ~(XT1DRIVE1_L + XT1DRIVE0);
    }


//----------------------------------------------------------------------------------------------------------------------------------//
// @src     bspSysClockSpeedSet(BSP_SYS_CLK_16MHZ);
    uint8_t ui8SetDcoRange, ui8SetVCore;
    uint32_t ui32SetMultiplier;

    const uint32_t ui32SystemClockSpeed = BSP_SYS_CLK_16MHZ;

    //
    // Set clocks (doing sanity check)
    // MCLK     = ui32SysClockSpeed;
    // SMCLK    = ui32SysClockSpeed;
    // ACLK     = 32 768 Hz
    //
    if((ui32SystemClockSpeed > 25000000UL) || (ui32SystemClockSpeed < 1000000UL)) {
        for(;;);
    }

    // Get DCO, VCore and multiplier settings for the given clock speed
    ui8SetDcoRange = DCORSEL_16MHZ;                                         /* <expanded>bspMcuGetSystemClockSettings(              */
    ui8SetVCore = VCORE_16MHZ;                                              /* ui32SystemClockSpeed, &ui8SetDcoRange, &ui8SetVCore, */
    ui32SetMultiplier = DCO_MULT_16MHZ;                                     /* &ui32SetMultiplier);                                 */

    // Set VCore setting
    jmr_bspMcuSetVCore(ui8SetVCore);

    // Disable FLL control loop, set lowest possible DCOx, MODx and select a suitable range
    __bis_SR_register(SCG0);
    UCSCTL0 = 0x00;
    UCSCTL1 = ui8SetDcoRange;

    // Set DCO multiplier and re-enable the FLL control loop
    UCSCTL2 = ui32SetMultiplier + FLLD_1;
    UCSCTL4 = SELA__XT1CLK | SELS__DCOCLKDIV  |  SELM__DCOCLKDIV ;
    __bic_SR_register(SCG0);

    // Loop until oscillator fault flags (XT1, XT2 & DCO fault flags) are cleared
    do {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);

        // Clear XT2, XT1, DCO fault flags
        SFRIFG1 &= ~OFIFG;
    }
    while(SFRIFG1 & OFIFG);

    // Worst-case settling time for the DCO
    __delay_cycles(781250);

    return;
}


/*****************************************************************************
* @brief    This function sets the PMM core voltage (PMMCOREV) setting.
*
* @param    ui8Level        is the target VCore setting.
*
* @return   None
******************************************************************************/
void jmr_bspMcuSetVCore(uint8_t ui8Level) {

    uint8_t ui8ActLevel;

    do {
        ui8ActLevel = PMMCTL0_L & PMMCOREV_3;
        if(ui8ActLevel < ui8Level) {

            // Set Vcore (step by step)
            jmr_bspMcuSetVCoreUp(++ui8ActLevel);
        }
        if(ui8ActLevel > ui8Level) {
            // Set VCore step by step
            jmr_bspMcuSetVCoreDown(--ui8ActLevel);
        }
    } while(ui8ActLevel != ui8Level);
}


void jmr_bspMcuSetVCoreUp(uint8_t ui8Level) {
    // Open PMM module registers for write access
    PMMCTL0_H = 0xA5;

    // Set SVS/M high side to new ui8Level
    SVSMHCTL = (SVSMHCTL & ~(SVSHRVL0 * 3 + SVSMHRRL0)) | (SVSHE + SVSHRVL0 * ui8Level + SVMHE + SVSMHRRL0 * ui8Level);

    // Set SVM new Level
    SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * ui8Level;

    // Set SVS/M low side to new level
    SVSMLCTL = (SVSMLCTL & ~(SVSMLRRL_3)) | (SVMLE + SVSMLRRL0 * ui8Level);

    // Wait until SVM is settled
    while((PMMIFG & SVSMLDLYIFG) == 0);

    // Set VCore to 'level' and clear flags
    PMMCTL0_L = PMMCOREV0 * ui8Level;
    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);

    if((PMMIFG & SVMLIFG)) {

        // Wait until level is reached
        while((PMMIFG & SVMLVLRIFG) == 0);
    }

    // Set SVS/M Low side to new level
    SVSMLCTL = (SVSMLCTL & ~(SVSLRVL0 * 3 + SVSMLRRL_3)) | (SVSLE + SVSLRVL0 * ui8Level + SVMLE + SVSMLRRL0 * ui8Level);

    // Lock PMM module registers from write access
    PMMCTL0_H = 0x00;
}


void jmr_bspMcuSetVCoreDown(uint8_t ui8Level) {

    // Open PMM module registers for write access
    PMMCTL0_H = 0xA5;

    // Set SVS/M low side to new level
    SVSMLCTL = (SVSMLCTL & ~(SVSLRVL0 * 3 + SVSMLRRL_3)) | (SVSLRVL0 * ui8Level + SVMLE + SVSMLRRL0 * ui8Level);

    // Wait until SVM is settled
    while((PMMIFG & SVSMLDLYIFG) == 0);

    // Set VCore to new level
    PMMCTL0_L = (ui8Level * PMMCOREV0);

    // Lock PMM module registers for write access
    PMMCTL0_H = 0x00;
}

