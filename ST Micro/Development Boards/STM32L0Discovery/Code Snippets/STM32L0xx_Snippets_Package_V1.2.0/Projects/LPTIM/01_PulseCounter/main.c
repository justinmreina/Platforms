/**
  ******************************************************************************
  * File     01_PulseCounter/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the LPTIM    
  *          to use it to count pulses in low power mode
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
   - LPTIMx
   - GPIO PB5 as LPTIM1_IN1
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
WARNING : To test this example disconnect the e-paper

    - This example configures the LPTIM in order to count pulses in response 
      to an edge on IN1 input while the device is in stop mode.
      The GPIO PB5, corresponding to LPTIM1_IN1, is configured as alternate function 
      and the AFR2 is selected.
    - To test this example, the user button must be connected to PB5, this is 
      easily done by wiring PA0 to PB5 thru the connector pins.
    - The counter will be incremented once pushing the user button, 
      no glitch filter is enabled in order to run without any clock.
      The device exits low power mode while the counter reaches the ARR value, 
      the green led blinks once before the device returns in low power.

TIPS : While this code is loaded in the device, keep the RESET button pressed 
       while starting or stopping the debug session.
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
#define TIMx_BASE       TIM21_BASE
#define TIMx            ((TIM_TypeDef *) TIMx_BASE)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0xFF;  //initialized at 0 and modified by the functions 
volatile uint16_t Counter = 0; // Contains the last value of the captured counter
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void ConfigureGPIO(void);
void ConfigureLPTIMxInCounterMode(void);
void ConfigureLowPowerMode(void);
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
  ConfigureLPTIMxInCounterMode();
  ConfigureLowPowerMode();
  __WFI();
  LPTIM1->ARR = 9; /* Reload ARR to get an interrupt each 10 pulses */
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
  * Brief   This function configures the LPTIMx in Counter mode to count once the 
  *         a rising edge occurs on TI2. The timer counter is enabled by HW.
  *         To use another timer, channel or GPIO, the RCC and GPIO configuration 
  *         must be adapted according to the datasheet. 
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureLPTIMxInCounterMode(void)
{
  /* (1) Enable the peripheral clock of LPTimer x */
  /* (2) Enable the peripheral clock of GPIOB (already done) */
  /* (3) Select Alternate function mode (10) on GPIOB pin 5 */  
  /* (4) Select LPTIM1_CH1 on PB5 by enabling AF2 for pin 5 in GPIOB AFRL register */ 

  RCC->APB1ENR |= RCC_APB1ENR_LPTIM1EN; /* (1) */
  //RCC->IOPENR |= RCC_IOPENR_GPIOBEN; /* (2) */
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE5)) \
               | (GPIO_MODER_MODE5_1); /* (3) */  
  GPIOB->AFR[0] |= 0x2  << (5 * 4); /* (4) */
  
  /* (1) Configure LPTimer in Counter on External Input1.*/
  /* (2) Enable interrupt on Autoreload match */
  /* (3) Enable LPTimer  */
  /* (4) Set Autoreload to 4 in order to get an interrupt after 10 pulses
         because the 5 first pulses don't increment the counter */
  LPTIM1->CFGR |= LPTIM_CFGR_COUNTMODE | LPTIM_CFGR_CKSEL; /* (1)*/  
  LPTIM1->IER |= LPTIM_IER_ARRMIE; /* (2) */
  LPTIM1->CR |= LPTIM_CR_ENABLE; /* (3) */
  LPTIM1->ARR = 4; /* (4) */
  LPTIM1->CR |= LPTIM_CR_CNTSTRT; /* start the counter in continuous */      

  /* Configure EXTI and NVIC for LPTIM1 */
  /* (1) Configure extended interrupt for LPTIM1 */
  /* (2) Enable Interrupt on LPTIM1 */
  /* (3) Set priority for LPTIM1 */  
  EXTI->IMR |= EXTI_IMR_IM29; /* (1) */
  NVIC_EnableIRQ(LPTIM1_IRQn); /* (2) */
  NVIC_SetPriority(LPTIM1_IRQn,3); /* (3) */  
}


/**
  * Brief   This function configures the Low power mode in order to 
  *         enter in stop mode with voltage regulator in low power mode 
  *         while wfi() instruction is executed.
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureLowPowerMode(void)
{
  /* (1) Set SLEEPDEEP bit of Cortex System Control Register */
  /* (2) Clear PDDS bit and set LPDS bit in PWR_CR 
         clear WUF in PWR_CSR by setting CWUF in PWR_CR */
  /* (3) Select HSI/4 as clock while exiting stop mode */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; /* (1) */   
  PWR->CR = (PWR->CR & (uint32_t)(~(PWR_CR_PDDS)))
          | PWR_CR_LPSDSR | PWR_CR_CWUF; /* (2) */
  RCC->CFGR |= RCC_CFGR_STOPWUCK; /* (3) */
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
  *         It only toggles the red led coding either the error number
  *         or the counter value
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
    if(error == 0)
    {
      Counter = TIMx->CNT;
      if (Counter > 0x1000)
      {
        error_temp = ((Counter / 0x1000) * 2) - 1;
        GPIOA->BSRR = (1<<5); //set red led on PA5
      }
      long_counter = LONG_DELAY;
    }
    else if (error != 0xFF)
    {
      /* red led blinks according to the code error value */
      error_temp = (error << 1) - 1;
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
  * Brief   This function handles LPTIM1 interrupt request.
  *         It toggles the green led while the autoreload occurs.
  * Param   None
  * Retval  None
  */
void LPTIM1_IRQHandler(void)
{
  if ((LPTIM1->ISR & LPTIM_ISR_ARRM) != 0) /* Check ARR match */ 
  {
    LPTIM1->ICR |= LPTIM_ICR_ARRMCF; /* Clear ARR match flag */
    GPIOB->ODR ^= (1 << 4); /* Toggle green led */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
