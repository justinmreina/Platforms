/******************************************************************************
*  Filename: hal_f5_timerA0.c
*
*  Description: Implementation file for basic timer operations needed for 
*               frequency hopping and frequency scanning algorithms
*
*  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/

/******************************************************************************
 * INCLUDES
 */
#include "msp430.h"
#include "hal_defs.h"
#include "hal_timer.h"

/******************************************************************************
 * LOCAL FUNCTIONS
 */
unsigned char volatile timer_event;

/* variable used to check if ISR has been assigned */
static uint8 timer_has_isr;

/* Function pointer */
static void (*timer_isr)(void);

/******************************************************************************
 * LOCAL DEFINES
 */
#define TIMER_LOW_POWER_MODE LPM3_bits

/******************************************************************************
 * @fn          hal_timer_init
 *
 * @brief       Function to initialize the timer for frequency hopping operation
 *              This function uses Timer A0
 *
 * input parameters
 *
 * @param       master_count : this defines the "system tick frequency"
 *
 * output parameters
 *
 * @return      void
 */
void hal_timer_init(uint16 master_count) {

  /* Start Timer 0 using the ACLK as a source (this enables the use of    */
  /* various low power modes). Timer 0 will be used to keep RF burst time */
  
  /* Seting for system tick frequency */
  TA0CCR0  = master_count - 1;              
  
  /* Used for burst alignnment */
  TA0CCR1  = 0;                             
  
  /* Used for one shot type events */
  TA0CCR2  = 0;                             
  
  /* This configures for : ACLK, Up to CCR0, clear TA0R, div/1 */
  TA0CTL   = TASSEL_1 + MC_1 + TACLR + ID_0;
  
  /* interrupt isr has not been attached */
  timer_has_isr = FALSE;

   /* interrupt enabled */
  TA0CCTL1 = CCIE;
  
  return;
}

/******************************************************************************
 * @fn          hal_timer_adjust
 *
 * @brief       Function used to make adjustments to the timing of the master
 *              schedule during run time. This is used by the slave node to en-
 *              sure that its master system tick is running at the same cadence
 *              of the masters (typically TX = master, RX = slave)
 *
 * input parameters
 *
 * @param       adjust: performs an offset of the timer counts values in runtime
 *
 * output parameters
 *
 * @return      void
 */
void hal_timer_adjust(uint16 adjust) {

  /* loads the new values into the timer registers */
  TA0R = adjust;      

  return;
}


/******************************************************************************
 * @fn          hal_timer_get_current
 *
 * @brief       This function returns the current timer count value
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      This function returns the current timer count value
 */
uint16 hal_timer_get_current(void) {      

  return (TA0R);
}

/******************************************************************************
 * @fn          hal_timer_stop
 *
 * @brief       This function stops the timer imidiately
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 */
void hal_timer_stop(void) {

  /* disable the timer imidiately */
  TA0CTL = 0;
  
  return;
}

/******************************************************************************
 * @fn          hal_timer_expire
 *
 * @brief       This function waits in low power mode for the timer to expire
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 */
void hal_timer_expire(void) {
  
  /* enable interrupt */
  TA0CCTL1 = CCIE;   
  
  timer_event = 0;
  
  while (timer_event == 0 ) {
    /* enter low power mode and wait for interupt */
    _BIS_SR(TIMER_LOW_POWER_MODE + GIE);
  }
  
  /* disable interrupt */
  TA0CCTL1 = 0;
  
  return;
}

/******************************************************************************
 * @fn          hal_timer_wait
 *
 * @brief       This function waits in low power mode for the timer to expire or
 *              a GPIO trigger, which ever comes first.
 * 
 *              Any value of "time" up to 65536 is valid, even it that value
 *              is greater than the master clock value, as the function 
 *              implements a loop. Furthermore if a value of 0 is inputted the 
 *              timer is disabled and the function waits for GPIO trigger only.
 *
 * input parameters
 *
 * @param       time : 2 options exist for time
 *                     - time = 0, then the function will wait until GPIO event
 *                     - time > 0, GPIO or timer event, which ever comes first.
 *
 * output parameters
 *
 * @return      void
 */
void hal_timer_wait(uint16 time) {

  uint16 wait_count;
  uint16 end_of_loop;
  uint16 current_TA_value;

  /* store the current value of the timer register */
  current_TA_value = TA0R;   
  wait_count = current_TA_value + time;
  
  /* if the requested waittime exceeds the TA0CCR0 (max) then make a loop */
  if(wait_count > TA0CCR0) {
    end_of_loop = 1;
    while(end_of_loop) {
       if(wait_count > TA0CCR0) {
  
         /* set the timer expiration target */
         TA0CCR2  = TA0CCR0-1;
          
          /* calculate the remaining wait time remaining */
          wait_count = wait_count - (TA0CCR2 - current_TA_value);
          
          /* do not count the initial timer values more that once, zero it out */
          current_TA_value = 0;
          
          /* interrupt enabled */
          TA0CCTL2 = CCIE;                       
          timer_event = 0;
          
          /* Enter Low Power mode and wait */
          _BIS_SR(TIMER_LOW_POWER_MODE + GIE);             
          
          /* check if interupt came from timer if not then it came from the radio
            ISR and we need to exit imidately to be able to align to its time. */
          if (timer_event == 0) 
            return;    
       } else {
         end_of_loop = 0;
       }
    }
  }
  
  /* set the timer expiration target */
  TA0CCR2  = wait_count;                 
  
  /* interrupt enabled */
  TA0CCTL2 = CCIE;
  
  /* Enter Low Power mode and wait */
  _BIS_SR(TIMER_LOW_POWER_MODE + GIE);

  /* disable interupts on CCR2 and exit */
  TA0CCTL2 = 0;                        
  
  return;
}

/******************************************************************************
 * @fn          timer_isr_register
 *
 * @brief       Register the interrupts service routine
 *
 * input parameters
 *
 * @param       void (*pfnIntHandler)(void) - pointer to the isr to call
 *
 * output parameters
 *
 * @return      void
 */
void timer_isr_register(void (*pfnIntHandler)(void)) {

  /* attach the interrupt isr */
  timer_isr = pfnIntHandler;

  /* interrupt isr has been attached */
  timer_has_isr = TRUE;
  
  return;
}

/******************************************************************************
 * @fn          Timer A0 interrupt service routine
 *
 * @brief       This ISR does not do anything in the current implemetation
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 */
void timer_isr_deregister(void (*pfnIntHandler)(void)) {

   /* de-attach the interrupt isr */
  timer_isr = NULL;

  /* interrupt isr has not been attached */
  timer_has_isr = FALSE;
  
  return;
  
}



/******************************************************************************
 * @fn          Timer A0 interrupt service routine
 *
 * @brief       This ISR does not do anything in the current implemetation
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMERB0_ISR (void) {
  
   /* Clear LPM bits from SR to stay awake after ISR terminates */
   /* _BIC_SR_IRQ(TIMER_LOW_POWER_MODE); */
  
}

/******************************************************************************
 * @fn          Timer A1 interrupt service routine
 *
 * @brief       This ISR termines what caused the ISR to fire and takes action
 *              Any access, read or write, of the TxIV register automatically 
 *              resets the highest "pending" interrupt flag.           
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 */
// Timer_B7 Interrupt Vector (TBIV) handler
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMERA1_ISR(void) {
 
  /* switch based on which interrupt flag has been set */
  switch( __even_in_range(TA0IV,14) ) {
      /* no interrupt */
    case  TA0IV_NONE: 
      break;                 
      /* this flag is set when the timer expires, wake up MCU */
    case  TA0IV_TA0CCR1:                     
      timer_event = TA0IV_TA0CCR1;
      if(timer_has_isr == TRUE) {
        /* if a timer ISR has been registers, then call it */
        timer_isr();
      } else {
        /* Clear LPM bits from SR to stay awake after ISR terminates */
        _BIC_SR_IRQ(TIMER_LOW_POWER_MODE);
      }
      break;       
       /* this flag is set for one shot timer events, wake up MCU */
    case  TA0IV_TA0CCR2:                     
       timer_event = TA0IV_TA0CCR2;
       /* Clear LPM bits from SR to stay awake after ISR terminates */
      _BIC_SR_IRQ(TIMER_LOW_POWER_MODE);
      break;       
    case  TA0IV_TA0CCR3: 
      break;             /* CCR3 not used */
    case  TA0IV_TA0CCR4: 
      break;             /* CCR4 not used */
    case  TA0IV_5: 
      break;             /* CCR5 not used */
    case  TA0IV_6: 
      break;             /* CCR6 not used */
    case  TA0IV_TA0IFG: 
      break;             /* IFG not used  */
    default: 
      break;
  }
}
