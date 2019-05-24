/**
  ******************************************************************************
  * File     03_CPU_FrequencyChange/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to increase and decrease 
  *          the CPU frequency
  *
 ===============================================================================
                    #####       MCU Resources     #####
 ===============================================================================
   - Flash memory
   - GPIO PB4, PA5 for leds
   - SYSTICK (to manage led blinking)
   - RCC
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
    - This example increases the CPU frequency and then decreases it.
      The flash latency is increased while the CPU frequency is increased but 
      the flash latency is decreased while the CPU frequency is also decreased.
      If this example is successful, the green led blinks regularly. 
      In case of failure, the red led blinks many times according to the error
      then is off for a longer period.
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


/* Error codes used to make the red led blinking */
#define ERROR_HSI_TIMEOUT 0x1
#define ERROR_PLL_TIMEOUT 0x2
#define ERROR_CLKSWITCH_TIMEOUT 0x3

/* Delay value : short one is used for the error coding, long one in case of no error
   or between two bursts */
#define SHORT_DELAY 100
#define LONG_DELAY 1000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0;  //initialized at 0 and modified by the functions 
uint32_t test_to_be_performed_twice = 1; //this variable is set to 2 if the first address of the page to erase is yet erased

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void  ConfigureGPIO(void);
void  IncreaseCPU_Clock(void);
void  DecreaseCPU_Clock(void);
void  SwitchFromPLLtoHSI(void);
void  SwitchOnPLL(void);
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
  if (error == 0)
  {
    SysTick_Config(16000); /* 1ms config */ 
    IncreaseCPU_Clock();
    if (error == 0)
    {
      SysTick_Config(32000); /* 1ms config */
      DecreaseCPU_Clock();
      if (error == 0)
      {
        SysTick_Config(8000); /* 1ms config */  
      }
    }
  }
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
  while ((RCC->CR & (RCC_CR_HSIRDY | RCC_CR_HSIDIVF)) != (RCC_CR_HSIRDY | RCC_CR_HSIDIVF)) /* (4) */
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
  while ((RCC->CFGR & RCC_CFGR_SWS)  != RCC_CFGR_SWS_PLL) /* (9) */
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
__INLINE void ConfigureGPIO(void)
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
  * Brief   This function increases the frequency from 16MHz to 32MHz.
  * Param   None
  * Retval  None
  */
__INLINE void  IncreaseCPU_Clock(void)
{
  /* (1) Set one wait state in Latency bit of FLASH_ACR */
  /* (2) Check the latency is set */
  /* (3) Switch the clock on HSI16/4 and disable PLL  */
  /* (4) Set PLLMUL to 16 to get 32MHz on CPU clock  */
  /* (5) Enable and switch on PLL */
  FLASH->ACR |= FLASH_ACR_LATENCY; /* (1) */    
  while ((FLASH->ACR & FLASH_ACR_LATENCY) == 0); /* (2) */    
  SwitchFromPLLtoHSI(); /* (3) */
  RCC->CFGR = (RCC->CFGR & (~(uint32_t)RCC_CFGR_PLLMUL)) 
            | RCC_CFGR_PLLMUL16; /* (4) */
  SwitchOnPLL(); /* (5) */
}


/**
  * Brief   This function decreases the frequency from 32MHz to 8MHz.
  * Param   None
  * Retval  None
  */
__INLINE void  DecreaseCPU_Clock(void)
{
  /* (1) Switch the clock on HSI16/4 and disable PLL */
  /* (2) Set PLLMUL to 4 to get 8MHz on CPU clock  */
  /* (3) Enable and switch on PLL */
  /* (4) Set one wait state in Latency bit of FLASH_ACR */
  /* (5) Check the latency is set */
  SwitchFromPLLtoHSI(); /* (1) */
  RCC->CFGR = (RCC->CFGR & (~(uint32_t)RCC_CFGR_PLLMUL)) 
            | RCC_CFGR_PLLMUL4; /* (2) */
  SwitchOnPLL(); /* (3) */
  FLASH->ACR &= ~FLASH_ACR_LATENCY; /* (4) */    
  while ((FLASH->ACR & FLASH_ACR_LATENCY) != 0); /* (5) */     
}


/**
  * Brief   This function switches on HSI and disables the PLL.
  * Param   None
  * Retval  None
  */
__INLINE void  SwitchFromPLLtoHSI(void)
{
  uint32_t tickstart;  
  /* (1) Switch the clock on HSI16/4 */
  /* (2) Wait for clock switched on HSI16/4 */
  /* (3) Disable the PLL by resetting PLLON */
  /* (4) Wait until PLLRDY is cleared */
  RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_SW)) |  RCC_CFGR_SW_HSI; /* (1) */
  tickstart = Tick;
  while ((RCC->CFGR & RCC_CFGR_SWS)  != RCC_CFGR_SWS_HSI) /* (2) */
  {
    if ((Tick - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
    {
      error = ERROR_CLKSWITCH_TIMEOUT; /* Report an error */
      return;
    }      
  }
  RCC->CR &= ~RCC_CR_PLLON; /* (3) */
  tickstart = Tick;
  while ((RCC->CR & RCC_CR_PLLRDY)  != 0) /* (4) */
  {
    if ((Tick - tickstart ) > PLL_TIMEOUT_VALUE)
    {
      error = ERROR_PLL_TIMEOUT; /* Report an error */
      return;
    }      
  }
}

/**
  * Brief   This function enables the PLL and switches the clock to the PLL.
  * Param   None
  * Retval  None
  */
__INLINE void  SwitchOnPLL(void)
{
  uint32_t tickstart;  
  /* (1) Switch on the PLL */
  /* (2) Wait for PLL ready */
  /* (3) Switch the clock to the PLL */
  /* (4) Wait until the clock is switched to the PLL */

  RCC->CR |= RCC_CR_PLLON; /* (1) */
  tickstart = Tick;
  while ((RCC->CR & RCC_CR_PLLRDY)  == 0) /* (2) */
  {
    if ((Tick - tickstart ) > PLL_TIMEOUT_VALUE)
    {
      error = ERROR_PLL_TIMEOUT; /* Report an error */
      return;
    }      
  }
  RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_SW)) | RCC_CFGR_SW_PLL; /* (3) */
  tickstart = Tick;
  while ((RCC->CFGR & RCC_CFGR_SWS)  != RCC_CFGR_SWS_PLL) /* (4) */
  {
    if ((Tick - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
    {
      error = ERROR_CLKSWITCH_TIMEOUT; /* Report an error */
      return;
    }      
  }
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
  
  Tick++;
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
  * Brief   This function handles PPP interrupt request.
  * Param   None
  * Retval  None
  */
/*void PPP_IRQHandler(void)
{
}*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
