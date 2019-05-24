/**
  ******************************************************************************
  * File     05_PWM_InputWithDMA/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the timer in PWM input mode
  *          using the DMA to get the result without CPU load. 
  *
    ==============================================================================
                      ##### RCC specific features #####
  ==============================================================================
    [..] After reset the device is running from MSI (2 MHz) with Flash 0 WS,
         and voltage scaling range is 2 (1.5V)
         all peripherals are off except internal SRAM, Flash and SW-DP.
         (+) There is no prescaler on High speed (AHB) and Low speed (APB) busses;
             all peripherals mapped on these busses are running at MSI speed.
         (+) The clock for all peripherals is switched off, except the SRAM and 
             FLASH.
         (+) All GPIOs are in analog state, except the SW-DP pins which
             are assigned to be used for debug purpose.
    [..] Once the device started from reset, the user application has to:
         (+) Configure the clock source to be used to drive the System clock
             (if the application needs higher frequency/performance)
         (+) Configure the System clock frequency and Flash settings
         (+) Configure the AHB and APB busses prescalers
         (+) Enable the clock for the peripheral(s) to be used
         (+) Configure the clock source(s) for peripherals whose clocks are not
             derived from the System clock (ADC, RTC/LCD, RNG and IWDG)
 ===============================================================================
                    #####       MCU Resources     #####
 ===============================================================================
   - RCC
  - TIM2
  - GPIO PA0 for TIM2_CH1
  - GPIO PA5 and PB4 for LEDs
  - DMA
  - SYSTICK for led management   

 ===============================================================================
                    ##### How to use this example #####
 ===============================================================================
    - this file must be inserted in a project containing  the following files :
      o system_stm32l0xx.c, startup_stm32l053xx.s
      o stm32l0xx.h to get the register definitions
      o CMSIS files
 ===============================================================================
                    ##### How to test this example #####
 ===============================================================================
    - This example configures the TIM2 in order to compute the period and the  
      duty cycle of a signal applied on its TI1 (channel 1).
      This channel is used as trigger input.
      The period and duty cycle of the signal on TIMx CH1 are downloaded 
      by the DMA from respectively the CCR1 and CCR2.
      The GPIO PA0, corresponding to TIM2_CH1, is configured as alternate function 
      and the AFR2 is selected.
    - The green led is switched ON to indicate that TIMERS is correctly configured
    - To get an accurate measure of a square signal, the solder bridge SB22  
      must be unsoldered in order to disconnect the pull-down of the push-button,
      else the signal will get low rising slope and the duty cycle will be modified.
    - To test this example, the user must applied a periodic signal on PA0.
      The period and duty cycle are loaded in global variables named Period and
      DutyCycle which can be monitored in live.
    - This example can be easily ported on any other timer by modifying TIMx 
      definition. The corresponding GPIO must be also configured as alternate 
      function according to the datasheet.
    - The green LED toggles if the period is not null and the duty cycle
      less than the period and till no fatal error is detected.
      In case of capture overflow, the red led is switched on each time 
      an error is reported.

  *    
  ******************************************************************************
  * Attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx.h"

/**  STM32L0_Snippets
  * 
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Time-out values */
#define HSI_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
#define PLL_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
#define CLOCKSWITCH_TIMEOUT_VALUE  ((uint32_t)5000) /* 5 s    */

/* Delay value : short one is used for the error coding, long one (~1s) in case 
   of no error or between two bursts */
#define SHORT_DELAY 200
#define LONG_DELAY 1000

/* Error codes used to make the red led blinking */
#define ERROR_DMA_XFER 0x01
#define ERROR_UNEXPECTED_DMA_IT 0x02

#define ERROR_HSI_TIMEOUT 0x04
#define ERROR_PLL_TIMEOUT 0x05
#define ERROR_CLKSWITCH_TIMEOUT 0x06

/* Define the Timer to be configured */
#define TIMx_BASE       TIM2_BASE
#define TIMx            ((TIM_TypeDef *) TIMx_BASE)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0;  // Initialized at 0 and modified by the functions 

uint16_t Period = 0; // Period of the signal applied on TI1 based on 48MHz clock
uint16_t DutyCycle = 0; // High signal time based on 48MHz clock
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void ConfigureGPIO(void);
void ConfigureTIMxAsPWM_Input(void);
void ConfigureDMA(void);
/* Private functions ---------------------------------------------------------*/
/**
  * Brief   Main program.
  * Param   None
  * Retval  None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l0xx.c file
     */
  SysTick_Config(2000); /* 1ms config */
  SystemClock_Config();
  ConfigureGPIO();
  if (error != 0)
  {
    while(1) /* endless loop */
    {
    }
  }
  error = 0;
  SysTick->CTRL = 0; /* Disable SysTick */
  ConfigureTIMxAsPWM_Input();
  ConfigureDMA();
  GPIOB->BSRR = 1<<4; /* switch on green led */
  while (error < ERROR_UNEXPECTED_DMA_IT)  
  {  
    __WFI();
  }
  GPIOB->BRR = 1<<4; /* switch off green led */  
  SysTick_Config(16000); /* 1ms config */
  while (1) /* Infinite loop */
  {
  }    
}


/**
  * Brief   This function configures the system clock @16MHz and voltage scale 1
  *         assuming the registers have their reset value before the call.
  *         POWER SCALE   = RANGE 1
  *         SYSTEM CLOCK  = PLL MUL8 DIV2
  *         PLL SOURCE    = HSI/4
  *         FLASH LATENCY = 0
  * Param   None
  * Retval  None
  */
__INLINE void SystemClock_Config(void)
{
  uint32_t tickstart;
  /* (1) Enable power interface clock */
  /* (2) Select voltage scale 1 (1.65V - 1.95V) 
         i.e. (01)  for VOS bits in PWR_CR */
  /* (3) Enable HSI divided by 4 in RCC-> CR */
  /* (4) Wait for HSI ready flag and HSIDIV flag */
  /* (5) Set PLL on HSI, multiply by 8 and divided by 2 */
  /* (6) Enable the PLL in RCC_CR register */
  /* (7) Wait for PLL ready flag */
  /* (8) Select PLL as system clock */
  /* (9) Wait for clock switched on PLL */
  RCC->APB1ENR |= (RCC_APB1ENR_PWREN); /* (1) */
  PWR->CR = (PWR->CR & ~(PWR_CR_VOS)) | PWR_CR_VOS_0; /* (2) */
  
  RCC->CR |= RCC_CR_HSION | RCC_CR_HSIDIVEN; /* (3) */
  tickstart = Tick;
  while ((RCC->CR & (RCC_CR_HSIRDY |RCC_CR_HSIDIVF)) != (RCC_CR_HSIRDY |RCC_CR_HSIDIVF)) /* (4) */
  {
    if ((Tick - tickstart ) > HSI_TIMEOUT_VALUE)
    {
      error = ERROR_HSI_TIMEOUT; /* Report an error */
      return;
    }      
  }
  RCC->CFGR |= RCC_CFGR_PLLSRC_HSI | RCC_CFGR_PLLMUL8 | RCC_CFGR_PLLDIV2; /* (5) */
  RCC->CR |= RCC_CR_PLLON; /* (6) */
  tickstart = Tick;
  while ((RCC->CR & RCC_CR_PLLRDY)  == 0) /* (7) */
  {
    if ((Tick - tickstart ) > PLL_TIMEOUT_VALUE)
    {
      error = ERROR_PLL_TIMEOUT; /* Report an error */
      return;
    }      
  }
  RCC->CFGR |= RCC_CFGR_SW_PLL; /* (8) */
  tickstart = Tick;
  while ((RCC->CFGR & RCC_CFGR_SWS_PLL)  == 0) /* (9) */
  {
    if ((Tick - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
    {
      error = ERROR_CLKSWITCH_TIMEOUT; /* Report an error */
      return;
    }      
  }
}


/**
  * Brief   This function enables the peripheral clocks on GPIO port A and B,
  *         configures GPIO PB4 in output mode for the Green LED pin,
  *         configures GPIO PA5 in output mode for the Red LED pin,
  * Param   None
  * Retval  None
  */
__INLINE void  ConfigureGPIO(void)
{  
  /* (1) Enable the peripheral clock of GPIOA and GPIOB */
  /* (2) Select output mode (01) on GPIOA pin 5 */
  /* (3) Select output mode (01) on GPIOB pin 4 */
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN; /* (1) */  
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE5)) 
               | (GPIO_MODER_MODE5_0); /* (2) */  
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE4)) 
               | (GPIO_MODER_MODE4_0); /* (3) */  
}


/**
  * Brief   This function configures the TIMx as PWM input with a DMA transfer.
  *         It also enables the peripheral clock on TIMx and on GPIOA.
  *         It configures GPIO PA0 as Alternate function for TIM2_CH1
  *         To use another timer, channel or GPIO, the RCC and GPIO configuration 
  *         must be adapted according to the datasheet.
  *         In case of other timer, the interrupt sub-routine must also be renamed
  *         with the right handler and the NVIC configured correctly.
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureTIMxAsPWM_Input(void)
{
  /* (1) Enable the peripheral clock of Timer x */
  /* (2) Enable the peripheral clock of GPIOB */
  /* (3) Select alternate function mode on GPIOB pin 13 */
  /* (4) Select AF2 on PA0 in AFRL for TIM2_CH1 */
  
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; /* (1) */
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN; /* (2) */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE0)) \
               | (GPIO_MODER_MODE0_1); /* (3) */  
  GPIOA->AFR[0] |= 0x02; /* (4) */
  
  /* (1) Select the active input TI1 for TIMx_CCR1 (CC1S = 01), 
         select the active input TI1 for TIMx_CCR2 (CC2S = 10) */ 
  /* (2) Select TI1FP1 as valid trigger input (TS = 101)
         configure the slave mode in reset mode (SMS = 100) */
  /* (3) Enable capture by setting CC1E and CC2E 
         select the rising edge on CC1 and CC1N (CC1P = 0 and CC1NP = 0, reset value),
         select the falling edge on CC2 (CC2P = 1). */
  /* (4) Enable DMA request on Capture/Compare 1 and 2 */
  /* (5) Enable counter */  
  
  TIMx->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_1; /* (1)*/
  TIMx->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_0 \
              | TIM_SMCR_SMS_2; /* (2) */
  TIMx->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC2P; /* (3) */  
  TIMx->DIER |= TIM_DIER_CC1DE | TIM_DIER_CC2DE; /* (4) */
  TIMx->CR1 |= TIM_CR1_CEN; /* (5) */
}


/**
  * Brief   This function configures the DMA to load values from TIM2 CCR1 and CCR2 
  *         to the memory.
  *         The load is performed at TIM2 request.
  *         TIM2 CH1 (CCR1) is connected to DMA channel 5 and TIM2 CH2 (CCR2) to
  *         DMA channel 7.
  *         The DMA is configured in circular mode and read from memory to peripheral. 
  *         Only the transfer error can trigger an interrupt.
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureDMA(void)
{
  /* (1) Enable the peripheral clock on DMA */ 
  /* (2) Remap DMA channel5 and 7 on TIM2_CH1 and TIM2_CH2  
         by writing 1000 in DMA_CSELR_C5S and DMA_CSELR_C7S */
  /* (3) Configure the peripheral data register address for DMA channel x */
  /* (4) Configure the memory address for DMA channel x */
  /* (5) Configure the number of DMA tranfer to be performed on DMA channel x */
  /* (6) Configure no increment (reset value), size (16-bits), interrupts,  
         transfer from peripheral to memory and circular mode  for DMA channel x */
  /* (7) Enable DMA Channel x */
  RCC->AHBENR |= RCC_AHBENR_DMA1EN; /* (1) */
  DMA1_CSELR->CSELR |= 8 << (4 * (5-1)) | 8 << (4 * (7-1)); /* (2) */
  DMA1_Channel5->CPAR = (uint32_t) (&(TIMx->CCR1)); /* (3) */
  DMA1_Channel5->CMAR = (uint32_t)(&Period); /* (4) */
  DMA1_Channel5->CNDTR = 1; /* (5) */
  DMA1_Channel5->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 \
                      | DMA_CCR_TEIE | DMA_CCR_CIRC; /* (6) */   
  DMA1_Channel5->CCR |= DMA_CCR_EN; /* (7) */
  
  /* repeat (3) to (6) for channel 6 */
  DMA1_Channel7->CPAR = (uint32_t) (&(TIMx->CCR2)); /* (2) */
  DMA1_Channel7->CMAR = (uint32_t)(&DutyCycle); /* (3) */
  DMA1_Channel7->CNDTR = 1; /* (4) */
  DMA1_Channel7->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 \
                      | DMA_CCR_TEIE | DMA_CCR_CIRC; /* (5) */   
  DMA1_Channel7->CCR |= DMA_CCR_EN; /* (6) */

  /* Configure NVIC for DMA */
  /* (7) Enable Interrupt on DMA Channels x */
  /* (8) Set priority for DMA Channels x */  
  NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn); /* (7) */
  NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn,3); /* (8) */
}


/******************************************************************************/
/*            Cortex-M0 Plus Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * Brief   This function handles NMI exception.
  * Param   None
  * Retval  None
  */
void NMI_Handler(void)
{
}

/**
  * Brief   This function handles Hard Fault exception.
  * Param   None
  * Retval  None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * Brief   This function handles SVCall exception.
  * Param   None
  * Retval  None
  */
void SVC_Handler(void)
{
}

/**
  * Brief   This function handles PendSVC exception.
  * Param   None
  * Retval  None
  */
void PendSV_Handler(void)
{
}

/**
  * Brief   This function handles SysTick Handler.
  *         It only toggles the red led coding the error number
  * Param   None
  * Retval  None
  */
void SysTick_Handler(void)
{
  static uint32_t long_counter = LONG_DELAY;
  static uint32_t short_counter = SHORT_DELAY;  
  static uint16_t error_temp = 0;
  
  Tick++;
  if (long_counter-- == 0) 
  {
    if ((error == 0) && (Period != 0) && (DutyCycle <= Period) )
    {
      /* the following instruction can only be used if no ISR modifies GPIOB ODR
         either by writing directly it or by using GPIOB BSRR or BRR 
         else a toggle mechanism must be implemented using GPIOB BSRR and/or BRR
      */
      GPIOB->ODR ^= (1<<4);//toggle green led on PC9
      long_counter = LONG_DELAY;
    }
    else if (error != 0xFF)
    {
      /* red led blinks according to the code error value */
      error_temp = (error << 1) - 1;
      error &= ~ERROR_DMA_XFER; /* Reset error on DMA transfer */
      short_counter = SHORT_DELAY;
      long_counter = LONG_DELAY << 1;
      GPIOA->BSRR = (1<<5); /* Set red led on PA5 */
      GPIOB->BRR = (1<<4); /* Switch off green led on PB4 */
    }
  }
  if (error_temp > 0)
  {
    if (short_counter-- == 0) 
    {
      GPIOA->ODR ^= (1 << 5); /* Toggle red led */
      short_counter = SHORT_DELAY;
      error_temp--;
    }  
  }
}


/******************************************************************************/
/*                 STM32L0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l0xx.s).                                               */
/******************************************************************************/

/**
  * Brief   This function handles DMA Channel2 and 3 interrupt request.
  *         It only manages DMA error on channel 3
  * Param   None
  * Retval  None
  */
void DMA1_Channel4_5_6_7_IRQHandler(void)
{
  
  if ((DMA1->ISR & (DMA_ISR_TEIF5 | DMA_ISR_TEIF6)) != 0) /* Test if transfer error on DMA channel 5 or 6 */
  {
    error |= ERROR_DMA_XFER; /* Report an error on DMA transfer */
    DMA1->IFCR |= DMA_ISR_TEIF5 | DMA_ISR_TEIF6; /* Clear both flags */
  }
  else
  {
    error |= ERROR_UNEXPECTED_DMA_IT; /* Report unexpected DMA interrupt occurrence */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
