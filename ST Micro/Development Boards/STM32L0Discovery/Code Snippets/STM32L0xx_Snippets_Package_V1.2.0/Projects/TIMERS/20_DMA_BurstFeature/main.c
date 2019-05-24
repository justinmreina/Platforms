/**
  ******************************************************************************
  * File     20_DMA_BurstFeature/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the timer 
  *          to generate a PWM center-aligned signal and to modify 
  *          the duty cycle on each Update Event. 
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
  - GPIO PB3, PB10, PB11 for TIM2_CH2, TIM2_CH3 and TIM2_CH4
  - DMA
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
    WARNING : To test this example, first disconnect the e-paper
    - This example configures the TIM2 in order to generate a PWM center-aligned 
      on OC2, OC3 and OC4, with a period of 1 milliseconds and a variable duty 
      cycle.
      The GPIO PB3, PB10 and PB11, corresponding to TIM2_CH2/CH3/CH4, are 
      configured as alternate function and the AF2 is selected for all of them.
    - To test this example, the user must monitor the signal on PB3, PB10 
      and PB11.
    - This example can be easily ported on any other timer by modifying TIMx 
      definition. The corresponding GPIO must be also adapted according to 
      the datasheet.

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

/* Define the Timer to be configured */
#define TIMx_BASE TIM2_BASE
#define TIMx           ((TIM_TypeDef *) TIMx_BASE)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0xFF;  //initialized at 0 and modified by the functions 
uint16_t Duty_Cycle_Table[3*10] = { 19, 29, 39, 69, 79, 89,
                                   119,129,139,169,179,189,
                                   219,229,239,269,279,289,
                                   319,329,339,359,379,389,
                                   419,429,439,469,479,489};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void ConfigureGPIO(void);
void ConfigureTIMxAsPWM_CenterAligned(void);
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
  ConfigureTIMxAsPWM_CenterAligned();
  while (1)  
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
  * Brief   This function configures the TIMx as PWM mode 1 and center-aligned
  *         and enables the peripheral clock on TIMx and on GPIOA.
  *         It configures GPIO PB3, PB10, PB11 as Alternate function 
            for TIM2_CH2/CH3/CH4
  *         To use another timer, channel or GPIO, the RCC and GPIO configuration 
  *         must be adapted according to the datasheet.
  *         In case of other timer, the interrupt sub-routine must also be renamed
  *         with the right handler and the NVIC configured correctly.
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureTIMxAsPWM_CenterAligned(void)
{
  /* (1) Enable the peripheral clocks of Timer x */
  /* (2) Enable the peripheral clock of GPIOB */
  /* (3) Select alternate function mode on GPIOB pin 3, 10 and 11 */
  /* (4) Select AF2 on PB3 in AFRL for TIM2_CH2 */
  /* (5) Select AF2 on PB10 and 11 in AFRH for TIM2_CH3 and TIM2_CH4 */
  
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; /* (1) */
  //RCC->IOPENR |= RCC_IOPENR_GPIOBEN; /* (2) */
  GPIOB->MODER = (GPIOB->MODER 
              & ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE10 | GPIO_MODER_MODE11)) 
               | (GPIO_MODER_MODE3_1 | GPIO_MODER_MODE10_1 | GPIO_MODER_MODE11_1); /* (3) */
  GPIOB->AFR[0] |= 0x02 << (3 * 4); /* (4) */
  GPIOB->AFR[1] |= (0x02 << ((10-8) * 4)) | (0x02 << ((11-8) * 4)); /* (5) */
  
  /* (1) Set prescaler to 15, so APBCLK/16 i.e 1MHz */ 
  /* (2) Set ARR = 499, as timer clock is 1MHz and center-aligned counting,
         the period is 1000 us */
  /* (3) Set CCR2/3/4 = 49, the signal will be high during 14 us */
  /* (4) Select PWM mode 1 on OC2/3/4  (OCxM = 110),
         enable preload register on OC2/3/4 (OCxPE = 1, reset value) */
  /* (5) Select active high polarity on OC2/3/4 (CCxP = 0, reset value),
         enable the output on OC2/3/4 (CCxE = 1)*/
  /* (6) Select center-aligned mode 1 (CMS = 01) */  
  /* (7) Force update generation (UG = 1) */
  
  TIMx->PSC = 15; /* (1) */
  TIMx->ARR = 499; /* (2) */
  TIMx->CCR2 = 20; /* (3) */
  TIMx->CCR3 = 30; /* (3) */
  TIMx->CCR4 = 40; /* (3) */  
  TIMx->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE; /* (4) */
  TIMx->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE
               | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4PE; /* (4) */
  TIMx->CCER |= TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E; /* (5) */
  TIMx->CR1 |= TIM_CR1_CMS_0; /* (6) */
  TIMx->EGR |= TIM_EGR_UG; /* (7) */
  
  /* Configure DMA Burst Feature */
  /* Configure the corresponding DMA channel */
  /* (1) Enable the peripheral clocks of Timer x and DMA*/
  /* (2) Remap DMA channel2 on TIM2_UP  by writing 1000 in DMA_CSELR_C2S */

  /* (3) Set DMA channel peripheral address is the DMAR register address */
  /* (4) Set DMA channel memory address is the address of the buffer in the RAM 
         containing the data to be transferred by DMA into CCRx registers */
  /* (5) Set the number of data transfer to sizeof(Duty_Cycle_Table) */
  /* (6) Configure DMA transfer in CCR register
         enable the circular mode by setting CIRC bit (optional)
         set memory size to 16_bits MSIZE = 01 
         set peripheral size to 32_bits PSIZE = 10
         enable memory increment mode by setting MINC
         set data transfer direction read from memory by setting DIR  */
  /* (7) Configure TIMx_DCR register with DBL = 3 transfers 
         and DBA = (@TIMx->CCR2 - @TIMx->CR1) >> 2 = 0xE */
  /* (8) Enable the TIMx update DMA request by setting UDE bit in DIER register */
  /* (9) Enable TIMx */
  /* (10) Enable DMA channel */
  RCC->AHBENR |= RCC_AHBENR_DMA1EN; /* (1) */
  DMA1_CSELR->CSELR |= 8 << (4 * (2-1)); /* (2) */
  DMA1_Channel2->CPAR = (uint32_t)(&(TIMx->DMAR)); /* (3) */
  DMA1_Channel2->CMAR = (uint32_t)(Duty_Cycle_Table); /* (4) */
  DMA1_Channel2->CNDTR = 10*3; /* (5) */
  DMA1_Channel2->CCR |= DMA_CCR_CIRC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_1
                      | DMA_CCR_MINC | DMA_CCR_DIR; /* (6) */
  TIMx->DCR = (3 << 8) 
            + ((((uint32_t)(&TIM2->CCR2)) - ((uint32_t)(&TIM2->CR1))) >> 2) ; /* (7) */
  TIMx->DIER |= TIM_DIER_UDE; /* (8) */
  TIMx->CR1 |= TIM_CR1_CEN; /* (9) */
  DMA1_Channel2->CCR |= DMA_CCR_EN; /* (10) */
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
