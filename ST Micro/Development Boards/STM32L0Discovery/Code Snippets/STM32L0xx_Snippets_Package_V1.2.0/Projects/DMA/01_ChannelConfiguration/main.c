/**
  ******************************************************************************
  * File     01_ChannelConfiguration/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the DMA 
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
   - ADC
   - SYSCFG
   - GPIO PA0 as ExtInt
   - GPIO PB4, PA5 for leds
   - EXTI0
   - DMA
   - Low power (WFI)
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
    - This example configures the ADC to convert 3 channels (CH6, 7 and 17)
    - The data are stored thanks to the DMA.
    - The end of sequence (EOSEQ) is managed by interrupt.
    - The code launches a continuous conversion.
    - The conversion is stopped if the USER push-button is pressed and start again
      in case of a new push. While the conversion is paused both leds are on.
    - The green led toggles but blinking is not visible.
      In case of failure, the red led blinks coding the error type
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

/* Delay values */
#define LONG_DELAY 500
#define SHORT_DELAY 100

/* Error codes used to make the red led blinking */
#define ERROR_DMA_XFER 0x01
#define ERROR_UNEXPECTED_DMA_IT 0x02
#define ERROR_UNEXPECTED_ADC_IT 0x04
#define ERROR_UNEXPECTED_EXT_IT 0x08

#define ERROR_HSI_TIMEOUT 0x10
#define ERROR_PLL_TIMEOUT 0x11
#define ERROR_CLKSWITCH_TIMEOUT 0x12

/* Internal voltage reference calibration value address */
#define VREFINT_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FF80078))

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0xFF;  //use to report errors 

uint16_t ADC_array[3]; //Array to store the values coming from the ADC and copied by DMA
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void ConfigureGPIO(void);
void  ConfigureGPIOforADC(void);
void ConfigureExternalIT(void);
void SetClockForADC(void);
void CalibrateADC(void);
void ConfigureADC(void);
void ConfigureDMA(void);
void EnableADC(void);
void DisableADC(void);
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
  ConfigureExternalIT();
  ConfigureGPIOforADC();
  SetClockForADC();  
  ConfigureADC();
  CalibrateADC(); 
  EnableADC();
  ConfigureDMA();
  ADC1->CR |= ADC_CR_ADSTART; /* Start the ADC conversions */
  if (error == 0xFF)
  {
    error = 0;
  }	
  while (error < ERROR_UNEXPECTED_DMA_IT) /* loop till no unrecoverable error */
  {
    __WFI();
  }
  SysTick_Config(16000); /* 1ms config */
  DisableADC();
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
  * Brief   This function enables the peripheral clocks on GPIO ports A
  *         configures PA6 and PA7 in Analog mode.
  *         For portability, some GPIO are again enabled.
  * Param   None
  * Retval  None
  */
__INLINE void  ConfigureGPIOforADC(void)
{
  /* (1) Enable the peripheral clock of GPIOA */
  /* (2) Select analog mode for PA6 and PA7 (reset state) */
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN; /* (1) */  
  //GPIOA->MODER |= GPIO_MODER_MODE6 | GPIO_MODER_MODE7; /* (2) */
}


/**
  * Brief   This function enables the peripheral clocks on GPIO port A,
  *         configures the EXTI register and NVIC IRQ.
  *         PA0 is set in input,no pull-up, no pull-down
  *         SYSCFG_EXTICR1 is kept at its reset value to select Port A for EXTI0 
  * Param   None
  * Retval  None
  */
__INLINE void  ConfigureExternalIT(void)
{  
  /* (1) Enable the peripheral clock of GPIOA */ 
  /* (2) Select input mode (00) on GPIOA pin 0 */  
  /* (3) Select Port A for pin 0 external interrupt by writing 0000 
         in EXTI0 (reset value)*/
  /* (4) Configure the corresponding mask bit in the EXTI_IMR register */
  /* (5) Configure the Trigger Selection bits of the Interrupt line 
         on rising edge*/
  /* (6) Configure the Trigger Selection bits of the Interrupt line 
         on falling edge*/
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN; /* (1) */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE0)); /* (2) */  
  //SYSCFG->EXTICR[0] &= (uint16_t)~SYSCFG_EXTICR1_EXTI0_PA; /* (3) */
  EXTI->IMR |= 0x0001; /* (4) */ 
  EXTI->RTSR |= 0x0001; /* (5) */
  //EXTI->FTSR |= 0x0001; /* (6) */
  
  /* Configure NVIC for External Interrupt */
  /* (6) Enable Interrupt on EXTI0_1 */
  /* (7) Set priority for EXTI0_1 */
  NVIC_EnableIRQ(EXTI0_1_IRQn); /* (6) */
  NVIC_SetPriority(EXTI0_1_IRQn,0); /* (7) */
}


/**
  * Brief   This function enables the clock in the RCC for the ADC
  *        and for the System configuration (mandatory to enable VREFINT)
  * Param   None
  * Retval  None
  */
__INLINE void SetClockForADC(void)
{
  /* (1) Enable the peripheral clock of the ADC and SYSCFG */
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_SYSCFGEN; /* (1) */
}


/**
  * Brief   This function performs a self-calibration of the ADC
  * Param   None
  * Retval  None
  */
__INLINE void  CalibrateADC(void)
{
  /* (1) Ensure that ADEN = 0 */
  /* (2) Clear ADEN */
  /* (3) Set ADCAL=1 */
  /* (4) Wait until EOCAL=1 */
  /* (5) Clear EOCAL */
  if ((ADC1->CR & ADC_CR_ADEN) != 0) /* (1) */
  {
    ADC1->CR &= (uint32_t)(~ADC_CR_ADEN);  /* (2) */  
  }
  ADC1->CR |= ADC_CR_ADCAL; /* (3) */
  while ((ADC1->ISR & ADC_ISR_EOCAL) == 0) /* (4) */
  {
    /* For robust implementation, add here time-out management */
  }
  ADC1->ISR |= ADC_ISR_EOCAL; /* (5) */
}


/**
  * Brief   This function configures the ADC to convert sequentially 3 channels
            in continuous mode.
  *         The conversion frequency is 16MHz i.e. PCLK
  *         The interrupt on overrun is enabled and the NVIC is configured
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureADC(void)
{
  /* (1) Select PCLK by writing 11 in CKMODE */
  /* (2) Select the continuous mode */
  /* (3) Select CHSEL6, CHSEL7 and CHSEL17 */
  /* (4) Select a sampling mode of 111 i.e. 239.5 ADC clk to be greater than 5 us */
  /* (5) Enable interrupt on overrrun */
  /* (6) Wake-up the VREFINT (only for VBAT, Temp sensor and VRefInt) */
  /* (7) Enable VREFINT buffer  for ADC by setting EN_VREFINT and 
         ENBUF_VREFINT_ADC bit in SYSCFG_CFGR3 */
  /* (8) Wait for VREFINT ADC buffer ready */
  ADC1->CFGR2 |= ADC_CFGR2_CKMODE; /* (1) */ 
  ADC1->CFGR1 |= ADC_CFGR1_CONT; /* (2) */
  ADC1->CHSELR = ADC_CHSELR_CHSEL6 | ADC_CHSELR_CHSEL7 | ADC_CHSELR_CHSEL17; /* (3) */
  ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; /* (4) */
  ADC1->IER = ADC_IER_OVRIE; /* (5) */
  ADC->CCR |= ADC_CCR_VREFEN; /* (6) */
  SYSCFG->CFGR3 |= SYSCFG_CFGR3_EN_VREFINT 
                 | SYSCFG_CFGR3_ENBUF_VREFINT_ADC; /* (7) */
  while ((SYSCFG->CFGR3 & SYSCFG_CFGR3_VREFINT_ADC_RDYF) == 0) /* (8) */
  {
    /* For robust implementation, add here time-out management */
  }
  
  /* Configure NVIC for ADC */
  /* (1) Enable Interrupt on ADC */
  /* (2) Set priority for ADC */
  NVIC_EnableIRQ(ADC1_COMP_IRQn); /* (1) */
  NVIC_SetPriority(ADC1_COMP_IRQn,0); /* (2) */
}


/**
  * Brief   This function configures the DMA to store the result of an ADC sequence.
  *         The conversion results are stored in 3-items array.
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureDMA(void)
{
  /* (1) Enable the peripheral clock on DMA */
  /* (2) Remap DMA channel1 on ADC (reset value) */
  /* (3) Enable DMA transfer on ADC */
  /* (4) Configure the peripheral data register address */
  /* (5) Configure the memory address */
  /* (6) Configure the number of DMA tranfer to be performs on channel 1 */
  /* (7) Configure increment, size and interrupts */
  /* (8) Enable DMA Channel 1 */
  RCC->AHBENR |= RCC_AHBENR_DMA1EN; /* (1) */
  //DMA1_CSELR->CSELR &= (uint32_t)(~DMA_CSELR_C1S); /* (2) */
  ADC1->CFGR1 |= ADC_CFGR1_DMAEN; /* (3) */
  DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR)); /* (4) */
  DMA1_Channel1->CMAR = (uint32_t)(ADC_array); /* (5) */
  DMA1_Channel1->CNDTR = 3; /* (6) */
  DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 \
                      | DMA_CCR_TEIE | DMA_CCR_TCIE ; /* (7) */  
  DMA1_Channel1->CCR |= DMA_CCR_EN; /* (8) */
  
  /* Configure NVIC for DMA */
  /* (1) Enable Interrupt on DMA Channel 1  */
  /* (2) Set priority for DMA Channel 1 */
  NVIC_EnableIRQ(DMA1_Channel1_IRQn); /* (1) */
  NVIC_SetPriority(DMA1_Channel1_IRQn,0); /* (2) */
}


/**
  * Brief   This function enables the ADC
  * Param   None
  * Retval  None
  */
__INLINE void EnableADC(void)
{
  /* (1) Enable the ADC */
  /* (2) Wait until ADC ready */
  ADC1->CR |= ADC_CR_ADEN; /* (1) */
  if ((ADC1->CFGR1 &  ADC_CFGR1_AUTOFF) == 0)
  {
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) /* (2) */
    {
      /* For robust implementation, add here time-out management */
    }  
  }
}


/**
  * Brief   This function disables the ADC
  * Param   None
  * Retval  None
  */
__INLINE void DisableADC(void)
{
  /* (1) Ensure that no conversion on going */
  /* (2) Stop any ongoing conversion */
  /* (3) Wait until ADSTP is reset by hardware i.e. conversion is stopped */
  /* (4) Disable the ADC */
  /* (5) Wait until the ADC is fully disabled */
  if ((ADC1->CR & ADC_CR_ADSTART) != 0) /* (1) */
  {
    ADC1->CR |= ADC_CR_ADSTP; /* (2) */
  }
  while ((ADC1->CR & ADC_CR_ADSTP) != 0) /* (3) */
  {
     /* For robust implementation, add here time-out management */
  }
  ADC1->CR |= ADC_CR_ADDIS; /* (4) */
  while ((ADC1->CR & ADC_CR_ADEN) != 0) /* (5) */
  {
    /* For robust implementation, add here time-out management */
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
  * Brief   This function handles DMA Channel1 interrupt request.
  *         It manages the ADC and DMA 
  * Param   None
  * Retval  None
  */
void DMA1_Channel1_IRQHandler(void)
{
  if ((DMA1->ISR & DMA_ISR_TCIF1) != 0) /* Test if transfer completed on DMA channel 1 */
  {
    DMA1_Channel1->CCR &= (uint32_t)(~DMA_CCR_EN); /* Disable DMA Channel 1 to write in CNDTR*/
    DMA1_Channel1->CNDTR = 3; /* Reload the number of DMA tranfer to be performs on channel 1 */
    DMA1_Channel1->CCR |= DMA_CCR_EN; /* Enable again DMA Channel 1 */
    DMA1->IFCR |= DMA_IFCR_CTCIF1; /* Clear the flag */
    GPIOB->ODR ^= (1<<4); /* Toggle green led on PB4 */
    ADC1->CR |= ADC_CR_ADSTART; /* Restart the sequence conversion */
  }
  else if ((DMA1->ISR & DMA_ISR_TEIF1) != 0) /* Test if transfer error on DMA channel 1 */
  {
    error |= ERROR_DMA_XFER; /* Report an error */
    DMA1->IFCR |= DMA_IFCR_CTEIF1; /* Clear the flag */
  }
  else
  {
    error |= ERROR_UNEXPECTED_DMA_IT; /* Report unexpected DMA interrupt occurrence */
  }
}


/**
  * Brief   This function handles ADC interrupt request.
  *         It manages the ADC and DMA in case of overrun
  *         the ADC is stopped but not disabled,
  *         the DMA is reinitialized,
  *         The AD conversion is reume till the USER button is pressed
  * Param   None
  * Retval  None
  */
void ADC1_COMP_IRQHandler(void)
{
  if ((ADC1->ISR & ADC_ISR_OVR) != 0)  /* Check OVR has triggered the IT */
  {
    GPIOA->BSRR = (1 << 5); /* Switch on red led to report a resume of the conversion  */
    GPIOB->BSRR = (1<<(4+16)); /* Switch off green led to reportit is due to overrun  */
    ADC1->ISR |= ADC_ISR_OVR; /* Clear the pending bit */
    ADC1->CR |= ADC_CR_ADSTP; /* Stop the sequence conversion */
    DMA1_Channel1->CCR &= (uint32_t)(~DMA_CCR_EN); /* Disable DMA Channel 1 to write in CNDTR*/
    DMA1_Channel1->CNDTR = 3; /* Reload the number of DMA tranfer to be performs on channel 1 */
    DMA1_Channel1->CCR |= DMA_CCR_EN; /* Enable again DMA Channel 1 */    
  }
  else
  {
    error |= ERROR_UNEXPECTED_ADC_IT; /* Report unexpected ADC interrupt occurrence */
  }
}


/**
  * Brief   This function handles EXTI0_1 interrupt request.
  *         It toggles the conversion sequence if PA0 is pressed
  * Param   None
  * Retval  None
  */
void EXTI0_1_IRQHandler(void)
{
  if ((EXTI->PR & EXTI_PR_PR0) != 0)  /* Check line 0 has triggered the IT */
  {
    EXTI->PR = EXTI_PR_PR0; /* Clear the pending bit */
    if ((ADC1->CR & ADC_CR_ADSTART) != 0) /* Check if conversion on going */
    {
      ADC1->CR |= ADC_CR_ADSTP; /* Stop the sequence conversion */
      GPIOA->BSRR = (1 << 5); /* Switch on red led to report a resume of the conversion  */
      GPIOB->BSRR = (1 << 4); /* Switch on the green led */
    }
    else
    {
      ADC1->CR |= ADC_CR_ADSTART; /* Restart the sequence conversion */
      GPIOA->BRR = (1 << 5); /* Switch off red led to stop reporting the resume */
    }   
  }
  else
  {
    error |= ERROR_UNEXPECTED_EXT_IT; /* Report unexpected External interrupt occurrence */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
