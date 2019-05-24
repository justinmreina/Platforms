/**
  ******************************************************************************
  * File     03_InputCaptureOnTI1/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the timer to capture 
  *          the counter value in TIMx_CCR1 when TI1 input rises.
  *
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
    - GPIO PB13 for TIM21_CH1
    - GPIO PA5 and PB4 for LEDs
    - SYSTICK for led management   

 ===============================================================================
                    ##### How to use this example #####
 ===============================================================================
    - this file must be inserted in a project containing the following files:
      o system_stm32l0xx.c, startup_stm32l053xx.s
      o stm32l0xx.h to get the register definitions
      o CMSIS files
 ===============================================================================
                   ##### How to test this example #####
 ===============================================================================
    - This example configures the TIM21 in order to compute the elapsed time 
      between two rising edges occurring on its TI1 (channel 1).
      This channel is used as trigger input.
      The TIMx interrupt subroutine computes the period of the signal on 
      TIMx IC1F.
      The GPIO PB13, corresponding to TIM21_CH1, is configured as alternate function 
      and the AFR6 is selected.
      To test this example, the user button can be connected to PB13, this is 
      easily done by wiring PA0 to PB13 thru the connector pins or by connecting 
      a wave generator to PB13.
      The elapsed time is loaded in a global variable named Counter which can be 
      monitored in live.
      The maximum elapsed time which can be measured between two pulses 
      is 700 ms with these settings RCC and TIMx. To remove this limitation, 
      the Update interrupt must be implemented.
    - This example can be easily ported on any other timer by modifying TIMx 
      definition. The corresponding GPIO must be also adapted according to 
      the datasheet.
    - The green LED lit once a period has been computed.

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

/* Delay value : short one is used for the error coding, long one in case of no error
   or between two bursts */
#define SHORT_DELAY 200
#define LONG_DELAY 1000

/* Error codes used to make the red led blinking */
#define ERROR_WRONG_IT 0x01

#define ERROR_HSI_TIMEOUT 0x02
#define ERROR_PLL_TIMEOUT 0x03
#define ERROR_CLKSWITCH_TIMEOUT 0x04

#define TIMx_BASE       TIM21_BASE
#define TIMx            ((TIM_TypeDef *) TIMx_BASE)
#define TIMx_IRQn TIM21_IRQn
#define TIMx_IRQHandler TIM21_IRQHandler



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0xFF;  //initialized at 0xFF and set in the TIMx interrupt subroutine
uint16_t gap = 0;    //initialized at 0 and used to synchronize the IC laps computation
uint16_t counter0; //used to store the first rising edge IC counter value 
volatile uint16_t Counter;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void ConfigureGPIO(void);
void ConfigureTIMxAsInputCapture(void);
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
  error = 0;
  SysTick->CTRL = 0; /* Disable SysTick */
  ConfigureTIMxAsInputCapture();
    
  while (error == 0)
  {  
     __WFI();
     if (error == 0)
     {
       GPIOB->BSRR = 1<<4; /* switch on green led */
     }
     else
     {
       GPIOB->BRR = 1<<4; /* switch off green led */
     }       
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
  * Brief   This function configures the TIMx as input capture 
  *         and enables the interrupt on TIMx. It also enables the peripheral
  *         clock on TIMx and on GPIOB, set the APB1CLK and AHBCLK to get 
  *         one count each 8us.
  *         It configures GPIO PB13 as Alternate function for TIM21_CH1
  *         To use another timer, channel or GPIO, the RCC and GPIO configuration 
  *         must be adapted according to the datasheet.
  *         In case of other timer, the interrupt sub-routine must also be renamed
  *         with the right handler and the NVIC configured correctly.
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureTIMxAsInputCapture(void)
{
  /* Configure NVIC for TIMx */
  /* (1) Enable Interrupt on TIMx */
  /* (2) Set priority for TIMx*/
  NVIC_EnableIRQ(TIMx_IRQn); /* (1) */
  NVIC_SetPriority(TIMx_IRQn,0); /* (2) */
  
  /* (1) Enable the peripheral clock of Timer x */
  /* (2) Set APB2 clock prescaler to /16 (111)
         set AHB clock prescaler to /16 (1011) */
  /* (3) Enable the peripheral clock of GPIOB */
  /* (4) Select alternate function mode on GPIOB pin 13 */
  /* (5) Select AF6 on PB13 in AFRH for TIM21_CH1 */
  
  RCC->APB2ENR |= RCC_APB2ENR_TIM21EN; /* (1) */
  RCC->CFGR |=  RCC_CFGR_PPRE2 | RCC_CFGR_HPRE_3
             | RCC_CFGR_HPRE_1 | RCC_CFGR_HPRE_0; /* (2) */
  RCC->IOPENR |= RCC_IOPENR_GPIOBEN; /* (3) */
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE13)) \
               | (GPIO_MODER_MODE13_1); /* (4) */  
  GPIOB->AFR[1] |= 0x6 << ((13 - 8) * 4); /* (5) */
  
  /* (1) Select the active input TI1 (CC1S = 01),
         program the input filter for 8 clock cycles (IC1F = 0011), 
         select the rising edge on CC1 (CC1P = 0, reset value)
         and prescaler at each valid transition (IC1PS = 00, reset value) */
  /* (2) Enable capture by setting CC1E */
  /* (3) Enable interrupt on Capture/Compare */
  /* (4) Enable counter */  
  
  TIMx->CCMR1 |= TIM_CCMR1_CC1S_0 \
               | TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1; /* (1)*/
  TIMx->CCER |= TIM_CCER_CC1E; /* (2) */  
  TIMx->DIER |= TIM_DIER_CC1IE; /* (3) */
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
  * Brief   This function handles TIMx interrupt request.
  *         This interrupt subroutine computes the laps between 2 rising edges 
  *         on T1IC. This laps is stored in the "Counter" variable.
  * Param   None
  * Retval  None
  */
void TIMx_IRQHandler(void)
{
uint16_t counter1;
  
/*
  
*/
  if ((TIMx->SR & TIM_SR_CC1IF) != 0)
  {
    if ((TIMx->SR & TIM_SR_CC1OF) != 0)  /* Check the overflow */
    {
      error = 0xFF;
      gap = 0;  /* Reinitialize the laps computing */
      TIMx->SR = ~(TIM_SR_CC1OF | TIM_SR_CC1IF); /* Clear the flags */
      return;
    }
    if (gap == 0) /* Test if it is the first rising edge */
    {
      counter0 = TIMx->CCR1; /* Read the capture counter which clears the CC1ICF */
      gap = 1; /* Indicate that the first rising edge has yet been detected */
    }
    else
    {
      counter1 = TIMx->CCR1; /* Read the capture counter which clears the CC1ICF */
      if (counter1 > counter0) /* Check capture counter overflow */
      {
        Counter = counter1 - counter0;
      }
      else
      {
        Counter = counter1 + 0xFFFF - counter0 + 1;
      }
      counter0 = counter1;
      error = 0;
    }    
  }
  else
  {
    error = ERROR_WRONG_IT; /* Report an error */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
