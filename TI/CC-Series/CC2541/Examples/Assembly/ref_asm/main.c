/************************************************************************************************************************************/
/** @file       main.c
 *  @brief      x
 *  @details	x
 *
 *  @target     Texas Instruments CC2541F128
 *  @board      all
 *
 *  @author     Justin Reina, Firmware Engineer
 *  @created	6/22/17
 *  @last rev	6/22/17
 *
 *
 *  @section 	BLE Stack Vocabulary
 *      ARC - Advanced Remote Control
 *      ST - Sensor Tag
 *
 *  @section 	Locations of HAL reference (in prog)
 *      BLE-Stack 1.4.2.2/Components/hal/target/CC2541ST/hal_board_cfg.h
 *      BLE-Stack 1.4.2.2/Components/hal/target/CC2541ST/hal_led.c
 *
 *  @section	Opens
 *      Assembly inline
 *      Assembly file
 *      GPIO Read
 *      GPIO ISR
 *      PWM out
 *
 *  @section	Legal Disclaimer
 *      All contents of this source file and/or any other related source files are the explicit property of Justin Reina. Do not 
 *      distribute. Do not copy.
 */
/************************************************************************************************************************************/
#include "globals.h"


/************************************************************************************************************************************/
/** @fcn        int main(void)
 *  @brief      x
 *  @details    x
 *
 *  @ref        https://www.iar.com/support/tech-notes/compiler/mixing-c-and-c/
 *  @note       the symbols ("P1DIR, P1_0, etc.) are not easily found here, using direct addresses instead (e.g. 0xFE, 0x90)
 *  @note       '\n' is required to end lines in multi-line statements like this
 *  @note       labels like 'loop' are visible only within the asm("") statement call
 *  @note       external ASM files in IAR are almost impossible, avoid if you can
 */
/************************************************************************************************************************************/
int main(void) {

 
//        P1DIR |= BIT0;
        asm(" ORL 0xFE, #0x01");
        
//        for(;;) {
//          P1_0 ^= BIT0;
//        }
asm ("loop:   NOP\n"                                                        
     "        XRL 0x90, #0x01\n"                                            
     "        SJMP loop");                                                  
}                     

