/**
  ******************************************************************************
  * File     03_GenerateSinewaveWithDMA/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the DAC in order to 
  *          generate a sinewave signal using the DMA and synchronized by 
  *          the Timer 6.
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
   - GPIO PA4
   - DAC
   - DMA
   - TIM6
   - NVIC
   - WFI
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
    - The code performs the DAC configuration and provides data in order 
      to generate a sinus wave on the DAC output PA4 centered on VDD/2.
    - The data are first computed and stored in an array. Then the DMA
      is configured and data are automatically transfered from the array 
      to the DAC by the DMA.
    - The frequency depends on INCREMENT definition, which defines the step
      between 2 DAConversions. The greater INCREMENT, the smoother the wave 
      but the lower the frequency.
    - The Timer6 is configured to generate an external trigger
      on TRGO each us. This value is limited by the computation of the sinewave.
    - The sinewave period is 72us i.e. the frequency is ~13.9kHz.
    - The signal can be monitored with an oscilloscope on PA4.
    - In case of eror, the red blinks coding theerror code.

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

/* following define is the step to generate the sinewave, must be a divider of 90.
   The lower INCREMENT, the lower the sinewave frequency. */
#define INCREMENT 5
#define SIN_ARRAY_SIZE 360/INCREMENT

#define TIM6_DAC_IRQn (IRQn_Type)17
/* Delay value : short one is used for the error coding, long one (~1s) in case 
   of no error or between two bursts */
#define SHORT_DELAY 100
#define LONG_DELAY 1000

/* Error codes used to make the red led blinking */
#define ERROR_DAC_DMA_UNDERRUN 0x01
#define ERROR_DMA_XFER 0x02
#define ERROR_UNEXPECTED_DMA_IT 0x04
#define ERROR_UNEXPECTED_DAC_IT 0x08
     
#define ERROR_HSI_TIMEOUT 0x10
#define ERROR_PLL_TIMEOUT 0x11
#define ERROR_CLKSWITCH_TIMEOUT 0x12

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0;  //initialized at 0 and modified by the functions 
uint16_t sin_data[SIN_ARRAY_SIZE]; //table containing the data to generate the sinewave 
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void ConfigureGPIO(void);
void ConfigureGPIOasAnalog(void);
void ConfigureDAC(void);
void ConfigureTIM6(void);
void ConfigureDMA(void);
uint16_t ComputeSinusPolynomialApprox(uint32_t x);
uint16_t GenerateWave(void);
/* Private functions ---------------------------------------------------------*/

/**
  * Brief   Main program.
  * Param   None
  * Retval  None
  */
int main(void)
{
  uint32_t x;
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
  /* Initialization of the table values for DAC signal generation */
  for (x = 0; x < SIN_ARRAY_SIZE; x++)
  {
    sin_data[x] = GenerateWave();
  }
  ConfigureGPIOasAnalog();
  ConfigureDAC();
  ConfigureDMA();
  ConfigureTIM6();
  __WFI(); /* If an eror occurs or the execution is stopped by debugger the wait mode will be exited */
  SysTick_Config(16000); /* 1ms config */
  while (1) /* Infinite loop only reach in case of error */
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
  * Brief   This function enables the peripheral clocks on GPIO port A
  *         and configures PA4 in Analog mode.
  * Param   None
  * Retval  None
  */
__INLINE void  ConfigureGPIOasAnalog(void)
{
  /* (1) Enable the peripheral clock of GPIOA */
  /* (2) Select analog mode for PA4 (reset value) */
  //RCC->IOPENR |= RCC_IOPENR_GPIOAEN; /* (1) */
  //GPIOA->MODER |= GPIO_MODER_MODER4; /* (2) */
}


/**
  * Brief   This function enables the peripheral clocks on DAC
  *         and configures the DAC to be ready to generate a signal on DAC1_OUT
  *         synchronized by TIM6 HW trigger and loaded by a DMA transfer.
  *         The output buffer is disabled to reach the closer VDD and VSS 
  *         with DAC output.
  * Param   None
  * Retval  None
  */
__INLINE void  ConfigureDAC(void)
{
  /* (1) Enable the peripheral clock of the DAC */
  /* (2) Enable DMA transfer on DAC ch1, 
         enable interrupt on DMA underrun DAC, 
         enable the DAC ch1, enable the trigger on ch 1,
         disable the buffer on ch1, 
         and select TIM6 as trigger by keeping 000 in TSEL1 */
  RCC->APB1ENR |= RCC_APB1ENR_DACEN; /* (1) */
  DAC->CR |= DAC_CR_DMAUDRIE1 | DAC_CR_DMAEN1 | DAC_CR_BOFF1 | DAC_CR_TEN1 | DAC_CR_EN1; /* (2) */  

  /* Configure NVIC for DAC */
  /* (3) Enable Interrupt on DAC Channel1 and Channel2 */
  /* (4) Set priority for DAC Channel1 and Channel2 */
  NVIC_EnableIRQ(TIM6_DAC_IRQn); /* (3)*/
  NVIC_SetPriority(TIM6_DAC_IRQn,0); /* (4) */

  DAC->DHR12R1 = 2048; /* Initialize the DAC value to middle point */
}


/**
  * Brief   This function configures the Timer6 to generate an external trigger
  *         on TRGO each microsecond.
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureTIM6(void)
{
  /* (1) Enable the peripheral clock of the TIM6 */ 
  /* (2) Configure MMS=010 to output a rising edge at each update event */
  /* (3) Select PCLK i.e. 16MHz/1=16MHz (reset value)*/
  /* (4) Set one update event each 1 microsecond */
  /* (5) Enable TIM6 */
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; /* (1) */ 
  TIM6->CR2 |= TIM_CR2_MMS_1; /* (2) */
  //TIM6->PSC = 0; /* (3) */
  TIM6->ARR = (uint16_t)15; /* (4) */
  TIM6->CR1 |= TIM_CR1_CEN; /* (5) */
}


/**
  * Brief   This function configures the DMA to load a value from sin_data[] 
  *         to the DAC channel 1.
  *         The load is performed at DAC request.
  *         The DMA is configured in circular mode and read from memory to peripheral. 
  *         Only the transfer error can trigger an interrupt.
  * Param   None
  * Retval  None
  */
__INLINE void ConfigureDMA(void)
{
  /* (1) Enable the peripheral clock on DMA */ 
  /* (2) Remap DAC on DMA channel 2 */
  /* (3) Configure the peripheral data register address */
  /* (4) Configure the memory address */
  /* (5) Configure the number of DMA tranfer to be performs on DMA channel x */
  /* (6) Configure increment, size (16-bits), interrupts, transfer from memory to peripheral and circular mode */
  /* (7) Enable DMA Channel x */
  RCC->AHBENR |= RCC_AHBENR_DMA1EN; /* (1) */
  DMA1_CSELR->CSELR |= (uint32_t)(9 << 4); /* (2) */
  DMA1_Channel2->CPAR = (uint32_t) (&(DAC->DHR12R1)); /* (3) */
  DMA1_Channel2->CMAR = (uint32_t)(sin_data); /* (4) */
  DMA1_Channel2->CNDTR = SIN_ARRAY_SIZE; /* (5) */
  DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 \
                      | DMA_CCR_DIR | DMA_CCR_TEIE | DMA_CCR_CIRC; /* (6) */   
  DMA1_Channel2->CCR |= DMA_CCR_EN; /* (7) */

  /* Configure NVIC for DMA */
  /* (1) Enable Interrupt on DMA Channels x */
  /* (2) Set priority for DMA Channels x */  
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn); /* (1) */
  NVIC_SetPriority(DMA1_Channel2_3_IRQn,3); /* (2) */
}


/**
  * Brief   This function computes the polynomial approximation of the sinus function.
  *         As this function computes for an angle between 0 and PI/2, the calling function
  *         must manage the symetry to get the correct value for the other values
  *         from PI/2 to 2PI.
  *         This function uses a polynom of 7-order
  *         P7(x) = x-x^3/3!+x^5/5!-x^7/7!
  *         This function is not optimized.
  * Param   x is an integer which corresponds to the angle in degree 
  *         It must be between 0 and 360       
  * Retval  the return value is the one translated on 12-bit i.e. between 0 and 2048.
  */
uint16_t ComputeSinusPolynomialApprox(uint32_t x)
{
float sin_p7;
float angle, angle_p;

  angle = ((float)x)*3.14/180;
  sin_p7 = angle; 
  angle_p = angle*angle*angle; /* angle_p = angle^3 */
  sin_p7 -= angle_p/6;
  angle_p = angle_p*angle*angle; /* angle_p = angle^5 */
  sin_p7 += angle_p/120;
  angle_p = angle_p*angle*angle; /* angle_p = angle^7 */
  sin_p7 -= angle_p/5040;
  sin_p7 *= 2048;
  return((uint16_t)sin_p7);   
}

/**
  * Brief   This function generates a sinusoidal wave centered on VDD/2.
  *         It runs an abscissa from 0 to 2PI.
  *         It uses the symetry from 0-PI/2 in order to get the other value :
  *         sin(PI-x) = sin(x) and sin(PI+x) = -sin(x)
  * Param   None       
  * Retval  the return value is the current sinus point on 12-bit i.e. between 0 and 4095.
  */
uint16_t GenerateWave(void)
{
static uint16_t i = 0;
uint16_t data = 0;

  if (i < 90)
  {
    data = 0x800 + ComputeSinusPolynomialApprox(i);
    i+=INCREMENT;
  }
  else if (i < 180)  /* PI/2 < i < PI */
  {
    data = 0x800 + ComputeSinusPolynomialApprox(180-i);
    i+=INCREMENT;
  }
  else if (i < (180 + 90))  /* PI < i < 3PI/2 */
  {
    data = 0x800 - ComputeSinusPolynomialApprox(i-180);
    i+=INCREMENT;
  }
  else if (i < 360)  /* 3PI/2 < i < 2PI */
  {
    data = 0x800 - ComputeSinusPolynomialApprox(360-i);
    i+=INCREMENT;
    if (i >= 360)
    {
      i=0;
    }
  }
  return(data);
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
  * Brief   This function handles DMA Channel2 and 3 interrupt request.
  *         It only manages DMA error on channel 2
  * Param   None
  * Retval  None
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  
  if ((DMA1->ISR & DMA_ISR_TEIF2) != 0) /* Test if transfer error on DMA channel 2 */
  {
    error |= ERROR_DMA_XFER; /* Report an error on DMA transfer */
    DMA1->IFCR |= DMA_IFCR_CTEIF2; /* Clear the flag */
  }
  else
  {
    error |= ERROR_UNEXPECTED_DMA_IT; /* Report unexpected DMA interrupt occurrence */
  }
}


/**
  * Brief   This function handles DMA Channel1 interrupt request.
  *         It only manages DMA error
  * Param   None
  * Retval  None
  */
void TIM6_DAC_IRQHandler(void)
{
  
  if ((DAC->SR & DAC_SR_DMAUDR1) != 0) /* Test if transfer error on DMA and DAC channel 1 */
  {
    error |= ERROR_DAC_DMA_UNDERRUN; /* Report an error on DMA underrun */
    DAC->SR |= DAC_SR_DMAUDR1;
  }
  else
  {
    error |= ERROR_UNEXPECTED_DAC_IT; /* Report unexpected DMA interrupt occurrence */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
