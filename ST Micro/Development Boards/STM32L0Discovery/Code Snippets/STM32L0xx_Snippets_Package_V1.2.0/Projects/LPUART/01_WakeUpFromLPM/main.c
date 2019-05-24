/**
  ******************************************************************************
  * File     01_WakeUpFromLPM/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the GPIOs and USART1 
  *          in order to receive bytes. 
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
   - GPIO PB10(LPUART_TX),PB11(LPUART_RX),PA0,PA5,PB4
   - LPUART
   - EXTI

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
   - Close SB18 and SB19
   - Fit 32K crystal and associated components
   - Plug cable " USB to TTL 3V3 " (from FTDIChip), to GND, PB10 and PB11.
   - Launch serial communication SW on PC
   - PC interface configuration : 
      - Data Length = 8 Bits
      - One Stop Bit
      - None parity
      - BaudRate = 9600 baud
      - Flow control: None 
   - Launch the program
   - Send with PC 'g' or 'G' to lit green LED on the board and wake up it
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

/* Error codes used to make the orange led blinking */
#define ERROR_NONE 0x00
#define ERROR_LPUART 0x01
#define ERROR_HSI_TIMEOUT 0x02
#define ERROR_PLL_TIMEOUT 0x03
#define ERROR_CLKSWITCH_TIMEOUT 0x04

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = ERROR_NONE;  //initialized at 0 and modified by the functions 

/* Private function prototypes -----------------------------------------------*/
void Configure_GPIO_LED(void);
void Configure_GPIO_LPUART(void);
void Configure_LPUART(void);
void Configure_LPM_Stop(void);


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
  Configure_GPIO_LED();
  Configure_GPIO_LPUART();
  Configure_LPUART();
  Configure_LPM_Stop();
  
  /* Reception done in LPUART IRQ handler */
  while (!error) /* Infinite loop */
  {
    /* Check LPUART is ready for reception before entering STOP mode */
    if((LPUART1->ISR & USART_ISR_REACK) == USART_ISR_REACK)
    {
      error = ERROR_LPUART;
      __WFI();
    }
  }
  
  /* An error occur */
  SysTick_Config(2000); /* 1ms config */
  while(1);
}

/**
  * Brief   This function enables the peripheral clocks on GPIO port A and B,
  *         configures GPIO PB4 in output mode for the Green LED pin,
  *         configures GPIO PA5 in output mode for the Red LED pin,
  * Param   None
  * Retval  None
  */
__INLINE void Configure_GPIO_LED(void)
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
  * Brief   This function :
             - Enables GPIO clock
             - Configures the LPUART pins on GPIO PB10 PB11
  * Param   None
  * Retval  None
  */
__INLINE void Configure_GPIO_LPUART(void)
{
  /* Enable the peripheral clock of GPIOB */
  RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
	
  /* GPIO configuration for LPUART signals */
  /* (1) Select AF mode (10) on PB10 and PB11 */
  /* (2) AF4 for LPUART signals */
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE10|GPIO_MODER_MODE11))\
                 | (GPIO_MODER_MODE10_1 | GPIO_MODER_MODE11_1); /* (1) */
  GPIOB->AFR[1] = (GPIOB->AFR[1] &~ (0x0000FF00))\
                  | (4 << (2 * 4)) | (4 << (3 * 4)); /* (2) */
}

/**
  * Brief   This function configures LPUART.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_LPUART(void)
{
	/* (1) Enable power interface clock */
  /* (2) Disable back up protection register to allow the access to the RTC clock domain */
  /* (3) LSE on */
  /* (4) Wait LSE ready */
  /* (5) Enable back up protection register to allow the access to the RTC clock domain */
  /* (6) LSE mapped on LPUART */
  /* (7) Enable the peripheral clock LPUART */
  /* Configure LPUART */
  /* (8) oversampling by 16, 9600 baud */
  /* (9) 8 data bit, 1 start bit, 1 stop bit, no parity, reception mode, stop mode */
  /* (10) Set priority for LPUART1_IRQn */
  /* (11) Enable LPUART1_IRQn */
  RCC->APB1ENR |= (RCC_APB1ENR_PWREN); /* (1) */
  PWR->CR |= PWR_CR_DBP; /* (2) */
  RCC->CSR |= RCC_CSR_LSEON; /* (3) */
  while ((RCC->CSR & (RCC_CSR_LSERDY)) != (RCC_CSR_LSERDY)) /* (4) */
  {
    /* add time out here for a robust application */
  }
  PWR->CR &=~ PWR_CR_DBP; /* (5) */
  RCC->CCIPR |= RCC_CCIPR_LPUART1SEL; /* (6) */
	RCC->APB1ENR |= RCC_APB1ENR_LPUART1EN; /* (7) */
  LPUART1->BRR = 0x369; /* (8) */
	LPUART1->CR1 = USART_CR1_UESM | USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE; /* (9) */
  NVIC_SetPriority(LPUART1_IRQn, 0); /* (10) */
  NVIC_EnableIRQ(LPUART1_IRQn); /* (11) */
}

/**
  * Brief   This function :
             - Configures the low power mode
  * Param   None
  * Retval  None
  */
__INLINE void Configure_LPM_Stop(void)
{
  /* (1) To enter deep sleep when __WFI() */
  /* (2) Select STOP mode */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; /* (1) */
  PWR->CR &=~ PWR_CR_PDDS; /* (2) */
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
  * Brief   This function handles LPUART interrupt request.
  * Param   None
  * Retval  None
  */
void LPUART1_IRQHandler(void)
{
  uint8_t chartoreceive = 0;
  
  /* The system is waked up by an UART frame */
  /* at this point the system is clocked by MSI */
  
  if((LPUART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
  {
    chartoreceive = (uint8_t)(LPUART1->RDR); /* Receive data, clear flag */
    error = ERROR_NONE;  
    switch(chartoreceive)
    {
    case 'g':
    case 'G': GPIOB->ODR ^= (1 << 4); /* toggle green led on PB4 */
              break;
    default: break;
    }
  }
  else
  {
    error = ERROR_LPUART; /* Report an error */
    NVIC_DisableIRQ(LPUART1_IRQn); /* Disable LPUART1_IRQn */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
