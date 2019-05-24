/**
  ******************************************************************************
  * File     18_MasterSlaveSynchronized/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to synchronize two timers   
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
   - TIMx
   - TIMy
   - GPIO PA5 for TIM2_CH1 (TIMy_CH1)
   - GPIO PB13 for TIM21_CH1 (TIMx_CH1)  
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
    - This example configures the TIM1 and TIM2 in order to start them 
      synchronously.
      Timer 21 is in master mode with CEN used as TRGO.
      Timer 2 must be configured in slave mode using ITR1 as internal trigger.     
      Timer2 is set in slave mode in external clock mode 1, so
      Timer 2 is in gated mode.
      To visualize the behaviour, TIM21_CH1 is configured in PWM mode 1 and 
      TIM2_CH1 also.
      TIM21_CH1 and TIM2_CH1 are configured with the same PWM.
      To monitor the signals, use an oscilloscope on PB13 for TIM21_CH1 and PA5
      for TIM2_CH1.
      Due to PA5 connected to the LED, the falling edge slopes are not as sharp 
      as on PB13.
    - This example can be easily ported on other timers getting slave 
      and/or master features.
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

/* Error codes used to make the red led blinking */
#define ERROR_HSI_TIMEOUT 0x01
#define ERROR_PLL_TIMEOUT 0x02
#define ERROR_CLKSWITCH_TIMEOUT 0x03

/* Define the Timers to be configured */
#define TIMx_BASE      TIM21_BASE
#define TIMy_BASE      TIM2_BASE
#define TIMx           ((TIM_TypeDef *) TIMx_BASE)
#define TIMy           ((TIM_TypeDef *) TIMy_BASE)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0xFF;  //initialized at 0 and modified by the functions 
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void ConfigureGPIO(void);
void ConfigureTIMxSynchroWithTIMy(void);
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
  if (error != 0xFF)
  {
    while(1) /* endless loop */
    {
    }
  }
  SysTick->CTRL = 0; /* Disable SysTick */
  error = 0;
  ConfigureTIMxSynchroWithTIMy();
  while (1) /* Infinite loop */
  {
    __WFI();   
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
  * Brief   This function configures the TIMx and TIMy to be started 
  *         synchronously.
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureTIMxSynchroWithTIMy(void)
{
  /* (1) Enable the peripheral clock of Timer x */
  /* (2) Enable the peripheral clock of GPIOB (already enabled) */
  /* (3) Select alternate function mode on GPIOB pin 13 */
  /* (4) Select AF6 on PB13 in AFRH for TIM21_CH1 */
  RCC->APB2ENR |= RCC_APB2ENR_TIM21EN; /* (1) */
  //RCC->IOPENR |= RCC_IOPENR_GPIOBEN; /* (2) */
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE13)) \
               | (GPIO_MODER_MODE13_1); /* (3) */  
  GPIOB->AFR[1] |= 0x6 << ((13 - 8) * 4); /* (4) */

  /* (1) Enable the peripheral clock of Timer y */
  /* (2) Enable the peripheral clock of GPIOA (yet enabled) */
  /* (3) Select alternate function mode on GPIOA pin 5 */
  /* (4) Select AF5 on PA5 in AFRL for TIM2_CH1 */
  
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; /* (1) */
  //RCC->IOPENR |= RCC_IOPENR_GPIOAEN; /* (2) */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE5)) 
               | GPIO_MODER_MODE5_1; /* (3) */
  GPIOA->AFR[0] |= (0x05 << (5*4)); /* (4) */
  
  /* (1) Configure Timer x in master mode to send its enable signal 
         as trigger output (MMS=001 in the TIMx_CR2 register). */
  /* (2) Configure the Timer x Channel 1 waveform (TIMx_CCMR1 register)
         is in PWM mode 1 (write OC1M = 110) */
  /* (3) Configure TIMy in slave mode using ITR1 as internal trigger 
         by writing TS = 000 in TIMy_SMCR (reset value)
         Configure TIMy in gated mode, by writing SMS=101 in the
         TIMy_SMCR register. */
  /* (4) Set TIMx prescaler to 2 */
  /* (5) Set TIMy prescaler to 2 */
  /* (6) Set TIMx Autoreload to 99 in order to get an overflow (so an UEV) 
         each 10ms */
  /* (7) Set capture compare register to a value between 0 and 99 */
  TIMx->CR2 |= TIM_CR2_MMS_0; /* (1)*/  
  TIMx->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; /* (2) */
  TIMy->SMCR |= TIM_SMCR_SMS_2 | TIM_SMCR_SMS_0; /* (3) */
  TIMx->PSC = 2; /* (4) */
  TIMy->PSC = 2; /* (5) */  
  TIMx->ARR = 99; /* (6) */
  TIMx-> CCR1 = 25; /* (7) */
  /* Configure the slave timer Channel 1 as PWM as Timer to show synchronicity  */
  /* (1) Configure the Timer y in PWM mode 1 (write OC1M = 110) */
  /* (2) Set TIMx Autoreload to 99  */
  /* (3) Set capture compare register to 25 */
  TIMy->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; /* (1) */
  TIMy->ARR = 99; /* (2) */
  TIMy-> CCR1 = 25; /* (3) */
  /* Enable the output of TIMx OC1 */
  /* Select active high polarity on OC1 (CC1P = 0, reset value),
     enable the output on OC1 (CC1E = 1) */
  TIMx->CCER |= TIM_CCER_CC1E;
  /* Enable the output of TIMy OC1 */
  /* Select active high polarity on OC1 (CC1P = 0, reset value),
     enable the output on OC1 (CC1E = 1) */
  TIMy->CCER |= TIM_CCER_CC1E;
  /* (1) Reset Timer x by writing ‘1 in UG bit (TIMx_EGR register) */
  /* (2) Reset Timer y by writing ‘1 in UG bit (TIMy_EGR register) */
  TIMx->EGR |= TIM_EGR_UG; /* (1) */ 
  TIMy->EGR |= TIM_EGR_UG; /* (2) */
  /* (1) Enable the slave counter first by writing CEN=1 in the TIMy_CR1 register. 
         TIMy will start synchronously with the master timer*/  
  /* (2) Start the master counter by writing CEN=1 in the TIMx_CR1 register. */
  TIMy->CR1 |= TIM_CR1_CEN; /* (1) */  
  TIMx->CR1 |= TIM_CR1_CEN; /* (2) */
}


/******************************************************************************/
/*            Cortex-M0 Plus Processor Exceptions Handlers                    */
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
