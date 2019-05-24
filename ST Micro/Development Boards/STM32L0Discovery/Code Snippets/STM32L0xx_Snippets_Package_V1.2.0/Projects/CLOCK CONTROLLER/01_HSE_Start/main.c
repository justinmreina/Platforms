/**
  ******************************************************************************
  * File     01_HSE_Start/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to start the HSE 
  *          to use it as system clock.
  *
 ===============================================================================
                    #####       MCU Resources     #####
 ===============================================================================
   - RCC
   - GPIO PA5 and PB4
   - SYSTICK (to manage led blinking)

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
    - On the discovery board ensure that SB20 is closed and SB21 is open.
    - The HSE on OSC_IN is provided by the MCO of the STM32F1 
      and is clocked at 8MHz.
    - This examples starts the HSE and use it as system clock
      The green LED blinks once HSE is started.

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


/* Delay value : short one is used for the error coding, long one in case of no error
   or between two bursts */
#define SHORT_DELAY 200
#define LONG_DELAY 1000

/* Error codes used to make the red led blinking */
#define ERROR_CSS 0x01
#define ERROR_HSE_LOST 0x02
#define ERROR_UNEXPECTED_RCC_IRQ 0x04

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint16_t error = 0xFF;  //initialized at 0xFF and modified by the functions 
/* Private function prototypes -----------------------------------------------*/
void  ConfigureGPIO(void);
void StartHSE(void);
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
  ConfigureGPIO();
  SysTick_Config(8000);/* 1ms config with HSE 8MHz*/
  StartHSE();
  while (1) /* Infinite loop */
  {
    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSE)
    {
      if (error == 0xFF)
      {
        error = 0;
      }
    }
    else
    {
      if (error == 0)
      {
        error = ERROR_HSE_LOST;
      }
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
  * Brief   This function enables the interrupton HSE ready,
  *         and start the HSE as external clock.
  * Param   None
  * Retval  None
  */
__INLINE void StartHSE(void)
{
  /* Configure NVIC for RCC */
  /* (1) Enable Interrupt on RCC */
  /* (2) Set priority for RCC */
  NVIC_EnableIRQ(RCC_CRS_IRQn); /* (1)*/
  NVIC_SetPriority(RCC_CRS_IRQn,0); /* (2) */
  
  /* (1) Enable interrupt on HSE ready */
  /* (2) Enable the CSS 
         Enable the HSE and set HSEBYP to use the external clock 
         instead of an oscillator 
         Enable HSE */
  /* Note : the clock is switched to HSE in the RCC_CRS_IRQHandler ISR */
  RCC->CIER |= RCC_CIER_HSERDYIE; /* (1) */  
  RCC->CR |= RCC_CR_CSSHSEON | RCC_CR_HSEBYP | RCC_CR_HSEON; /* (2) */  
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
  if ((RCC->CIFR & RCC_CIFR_CSSF) != 0)
  {
    error = ERROR_CSS; /* Report the error */
    RCC->CICR |= RCC_CICR_CSSC; /* Clear the flag */
  }
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
  *         It toggles the green led if the action has been performed correctly
  *         and toggles the red led coding the error number
  * Param   None
  * Retval  None
  */
void SysTick_Handler(void)
{
  static uint32_t long_counter = LONG_DELAY;
  static uint32_t short_counter = SHORT_DELAY;  
  static uint16_t error_temp = 0;
  
  if (long_counter-- == 0) 
  {
    if(error == 0)
    {
      /* the following instruction can only be used if no ISR modifies GPIOC ODR
         either by writing directly it or by using GPIOC BSRR or BRR 
         else a toggle mechanism must be implemented using GPIOC BSRR and/or BRR
      */
      GPIOB->ODR ^= (1 << 4);//toggle green led on PB4
      long_counter = LONG_DELAY;
    }
    else if (error != 0xFF)
    {
      /* red led blinks according to the code error value */
      error_temp = (error << 1) - 1;
      short_counter = SHORT_DELAY;
      long_counter = LONG_DELAY << 1;
      GPIOA->BSRR = (1 << 5); //set red led on PA5
      GPIOB->BRR = (1 << 4); //switch off green led on PB4
    }
  }
  if (error_temp > 0)
  {
    if (short_counter-- == 0) 
    {
      GPIOA->ODR ^= (1 << 5); //toggle red led
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
  * Brief   This function handles RCC interrupt request 
  *         and switch the system clock to HSE.
  * Param   None
  * Retval  None
  */
void RCC_CRS_IRQHandler(void)
{
  /* (1) Check the flag HSE ready */
  /* (2) Clear the flag HSE ready */
  /* (3) Switch the system clock to HSE */
  
  if ((RCC->CIFR & RCC_CIFR_HSERDYF) != 0) /* (1) */
  {
    RCC->CICR |= RCC_CICR_HSERDYC; /* (2) */    
    RCC->CFGR = ((RCC->CFGR & (~RCC_CFGR_SW)) | RCC_CFGR_SW_HSE); /* (3) */
  }
  else
  {
    error = ERROR_UNEXPECTED_RCC_IRQ; /* Report an error */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
