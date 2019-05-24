/**
  ******************************************************************************
  * File     01_UpcounterOnRisingEdgeOnTI2/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the TIMx Channel 2   
  *          to use it as external clock
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
   - TIMx
   - GPIO PA0 for TIM2_CH1
   - GPIO PA5 and PB4 for LEDs
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
    - This example configures the TIM2 in order to increment the counter
      while a rising edge occurs on its channel 1.
      This channel is used as external clock.
      The GPIO PA0, corresponding to TIM2_CH1, is configured as alternate function 
      and the AFR2 is selected.
      To test this example, the user button is kept connected to PA0.
      The TIM2 counter is loaded in a global variable named Counter which can be 
      monitored in live.
    - This example can be easily ported on another timer by modifying TIMx 
      definition.
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

/* Delay value : short one is used for the error coding, long one (~2s) in case 
   of no error or between two bursts */
#define SHORT_DELAY 100
#define LONG_DELAY 2000

#define ERROR_HSI_TIMEOUT 0x10
#define ERROR_PLL_TIMEOUT 0x11
#define ERROR_CLKSWITCH_TIMEOUT 0x12

/* Define the Timer to be configured */
#define TIMx_BASE       TIM2_BASE
#define TIMx            ((TIM_TypeDef *) TIMx_BASE)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0;  //initialized at 0 and modified by the functions 

volatile uint16_t Counter = 0; // Contains the last value of the captured counter
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void ConfigureGPIO(void);
void ConfigureTIMxAsUpcounter(void);
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
  SysTick->CTRL = 0; /* Disable SysTick */
  ConfigureTIMxAsUpcounter();
  while (error == 0) /* Infinite loop */
  {
    Counter = TIM2->CNT; /* only to visualize the effect */
  }
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
  * Brief   This function configures the TIMx to perform the count of rising 
  *         edges on TI1.
  *         To use another timer, channel or GPIO, the 4 first code lines must 
  *         be adapted according to the datasheet.
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureTIMxAsUpcounter(void)
{
  /* (1) Enable the peripheral clock of Timer 2 */
  /* (2) Enable the peripheral clock of GPIOA */
  /* (3) Select Alternate function mode (10) on GPIOA pin 0 */  
  /* (4) Select TIM2_CH1 on PA0 by enabling AF2 for pin 0 in GPIOA AFRL register */ 

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; /* (1) */
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN; /* (2) */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE0)) \
               | (GPIO_MODER_MODE0_1); /* (3) */  
  GPIOA->AFR[0] |= 0x2; /* (4) */
  
  /* (1) Configure channel 1 to detect rising edges on the TI1 input 
         by writing CC1S = ‘01’, 
         and configure the input filter duration by writing the IC1F[3:0] bits 
         in the TIMx_CCMR1 register (if no filter is needed, keep IC2F=0000).*/
  /* (2) Select rising edge polarity by writing CC1P=0 in the TIMx_CCER register 
         Not necessary as it keeps the reset value. */
  /* (3) Configure the timer in external clock mode 1 by writing SMS=111 
         Select TI1 as the trigger input source by writing TS=101 
         in the TIMx_SMCR register.*/
  /* (4) Enable the counter by writing CEN=1 in the TIMx_CR1 register. */  
  TIMx->CCMR1 |= TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1 | TIM_CCMR1_CC1S_0; /* (1)*/  
  //TIMx->CCER &= (uint16_t)(~TIM_CCER_CC1P); /* (2) */
  TIMx->SMCR |= TIM_SMCR_SMS | TIM_SMCR_TS_2 | TIM_SMCR_TS_0; /* (3) */
  TIMx->CR1 |= TIM_CR1_CEN; /* (4) */
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
    if ((error != 0) && (error != 0xFF))
      {
      /* red led blinks according to the code error value */
      error_temp = (error << 1) - 1;
      short_counter = SHORT_DELAY;
      long_counter = LONG_DELAY << 1;
      GPIOA->BSRR = (1<<5); /* Set red led on PA5 */
      GPIOB->BRR = (1<<4); /* Switch off green led on PB4 */
      }
      else
      {
      long_counter = LONG_DELAY;
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
  * Brief   This function handles PPP interrupt request.
  * Param   None
  * Retval  None
  */
/*
void PPP_IRQHandler(void)
{
}
*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
