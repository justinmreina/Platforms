/**
  ******************************************************************************
  * File     01_Acquisition/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the GPIOs and TSC
  *          in order to perform acquisition and lit LEDs if touch detected. 
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
   - Systick
   - GPIO PA0,PA5,PB4
          PA7(Sampling capacitor, G2IO4), PA6(electrode, G2IO3)
   - TSC
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
   - Launch the program
   - The green LED lit (mode "ON", acquisitions are on going on the sensor)
   - Touch the 2nd element of the linear sensor
   - The red LED lit while it is pressed
   - after few time (5s), if no activities on linear sensor, 
     reference adaption is computed each 100ms
   - The board goes to "OFF" when press on the user button
   - Press the user button, to set the mode "ON"
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

/* Error codes used to make the red led blinking */
#define ERROR_TSC 0x01
#define ERROR_HSI_TIMEOUT 0x02
#define ERROR_PLL_TIMEOUT 0x03
#define ERROR_CLKSWITCH_TIMEOUT 0x04

#define THRESHOLD (50)
#define NUMBER_OF_CALIBRATION (10)
#define SOFT_DELAY (30)
#define TIME_BEFORE_ADAPTATION (5000) /* 5s */
#define TIME_BETWEEN_ADAPTATION (100) /* 100ms */
#define REFERENCE_TAB_SIZE (3)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0;  //initialized at 0 and modified by the functions 

uint8_t Standby = 0; /* default Standby = 0 => ON, Standby = 1 => OFF */
uint8_t CalibrationDone = 0; /* By default => calibration ongoing */
uint32_t AcquisitionValue = 0;
uint32_t Reference = 0;
uint8_t Activities = 0;
volatile uint32_t Count = 0;
uint8_t ReferenceAdaptation = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Configure_GPIO_LED(void);
void Configure_GPIO_TSC(void);
void Configure_TSC(void);
void Configure_GPIO_Button(void);
void Configure_EXTI(void);
void Process(void);
void SoftDelay(void);

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
  Configure_GPIO_LED();
  if (error != 0)
  {
    while(1) /* endless loop */
    {
    }
  }
  
  SysTick_Config(16000); /* 1ms config */
  
  Configure_GPIO_TSC();
  Configure_TSC();
  Configure_GPIO_Button();
  Configure_EXTI();

  /* First acquisition */
  TSC->CR |= (1<<1); /* TSC_CR_START = 1 */
  
  /* Infinite loop */
  while (!error) 
  {
    /* Comment this function to perform only one acquisition */
    Process();
  }
  
  /* error occurs */
  while(1);
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
  
  /* lit green LED */
  GPIOB->BSRR = GPIO_BSRR_BS_4;
}

/**
  * Brief   This function :
  *         - Enables GPIO clock
  *         - Configures the TSC pins on GPIO PA6 PA7
  * Param   None
  * Retval  None
  */
__INLINE void Configure_GPIO_TSC(void)
{
  /* Enable the peripheral clock of GPIOA */
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	
  /* (1) Open drain for sampling */
  /* (2) PP for channel */
  /* (3) Select AF mode (10) on PA6, PA7 */
  /* (4) AF3 for TSC signals */
  GPIOA->OTYPER |= GPIO_OTYPER_OT_7; /* (1) */
  GPIOA->OTYPER &=~ GPIO_OTYPER_OT_6; /* (2) */
  GPIOA->MODER = (GPIOA->MODER  & ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7))\
                | (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1); /* (3) */
  GPIOA->AFR[0] = (GPIOA->AFR[0] & ~((0xF<<(4*6)) | ((uint32_t)0xF<<(4*7))))\
                     |(3<<(4*6))|(3<<(4*7)); /* (4) */
}

/**
  * Brief   This function configures TSC.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_TSC(void)
{
  /* Enable the peripheral clock TSC */
  RCC->AHBENR |= RCC_AHBENR_TSCEN;

  /* Configure TSC */
  /* (1) fPGCLK = fHCLK/32, pulse high = 2xtPGCLK,Master, pulse low = 2xtPGCLK */
  /*     Charge transfer cycle will be around 8µs */
  /*     TSC_CR_CTPH_0 = 1, TSC_CR_CTPL_0 = 1, TSC_CR_PGPSC_0 = 1, TSC_CR_PGPSC_2 = 1 */
  /*     Max count value = 16383 pulses, TSC_CR_MCV_1 = 1, TSC_CR_MCV_2 = 1 */
  /*     TSC enabled, TSC_CR_TSCE = 1 */
  /* (2) Disable hysteresis, TSC_IOHCR_G2_IO4 = 0, TSC_IOHCR_G2_IO3 = 0 */
  /* (3) Enable end of acquisition IT, TSC_IER_EOAIE = 1 */
  /* (4) Sampling enabled, G2IO4, TSC_IOSCR_G2_IO4 = 1 */
  /* (5) Channel enabled, G2IO3, TSC_IOCCR_G2_IO3 = 1 */
  /* (6) Enable group, G2, TSC_IOGCSR_G2E = 1 */
  TSC->CR = TSC_CR_PGPSC_2 | TSC_CR_PGPSC_0 | TSC_CR_CTPH_0 | TSC_CR_CTPL_0 |\
            TSC_CR_MCV_2 | TSC_CR_MCV_1 | TSC_CR_TSCE; /* (1) */
  TSC->IOHCR &= (uint32_t)(~(TSC_IOHCR_G2_IO3 | TSC_IOHCR_G2_IO4)); /* (2) */
  TSC->IER = TSC_IER_EOAIE; /* (3) */
  TSC->IOSCR = TSC_IOSCR_G2_IO4; /* (4) */
  TSC->IOCCR = TSC_IOCCR_G2_IO3; /* (5) */
  TSC->IOGCSR |= TSC_IOGCSR_G2E; /* (5) */
  
  /* Configure IT */
  /* (4) Set priority for TSC_IRQn */
  /* (5) Enable TSC_IRQn */
  NVIC_SetPriority(TSC_IRQn, 0); /* (4) */
  NVIC_EnableIRQ(TSC_IRQn); /* (5) */ 
}

/**
  * Brief   This function :
             - Enables GPIO clock
             - Configures the Push Button GPIO PA0
  * Param   None
  * Retval  None
  */
__INLINE void Configure_GPIO_Button(void)
{
  /* Enable the peripheral clock of GPIOA */
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	
  /* Select mode */
  /* Select input mode (00) on PA0 */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE0));
}

/**
  * Brief   This function configures EXTI.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_EXTI(void)
{
  /* Configure Syscfg, exti and nvic for pushbutton PA0 */
  /* (1) PA0 as source input */
  /* (2) Unmask port 0 */
  /* (3) Rising edge */
  /* (4) Set priority */
  /* (5) Enable EXTI0_1_IRQn */
  SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0] & ~SYSCFG_EXTICR1_EXTI0) | SYSCFG_EXTICR1_EXTI0_PA; /* (1) */ 
  EXTI->IMR |= EXTI_IMR_IM0; /* (2) */ 
  EXTI->RTSR |= EXTI_RTSR_TR0; /* (3) */ 
  NVIC_SetPriority(EXTI0_1_IRQn, 0); /* (4) */ 
  NVIC_EnableIRQ(EXTI0_1_IRQn); /* (5) */ 
}

/**
  * Brief   This function processes the TSC value.
  * Param   None
  * Retval  None
  */
void Process(void)
{
  static uint32_t NumberOfCalibration=0;
  uint8_t RefIndex=0;
  static uint8_t RefIndexStatic=0;
  static uint32_t ReferenceTab[REFERENCE_TAB_SIZE];
  
  if(!Standby) /* Acquisition mode */
  {
    if(AcquisitionValue) /* check if there is a new acquisition value */
    {
      if(CalibrationDone) /* check if the calibration is done */
      {
        if((AcquisitionValue + THRESHOLD) < Reference) /* Touch detected */
        {
          GPIOA->BSRR = GPIO_BSRR_BS_5;/* Lit red LED */
          Activities = 1;
        }
        else if(AcquisitionValue > (Reference + THRESHOLD)) /* Need recalibration */
        {
          GPIOA->BSRR = GPIO_BSRR_BR_5;/* Off red LED */
          Activities = 1;
          CalibrationDone = 0;/* restart calibration */
          Reference = 0;/* Reset reference */
        }
        else /* no touch detected */
        {
          GPIOA->BSRR = GPIO_BSRR_BR_5;/*  Off red LED */
          
          /* Reference adaptation */
          if(ReferenceAdaptation)
          {
            ReferenceAdaptation=0;
            RefIndexStatic%=REFERENCE_TAB_SIZE;
            ReferenceTab[RefIndexStatic++] = AcquisitionValue;
            
            for(RefIndex=0;RefIndex<REFERENCE_TAB_SIZE;RefIndex++)
            {
               Reference += ReferenceTab[RefIndex];
            }
            Reference /= (REFERENCE_TAB_SIZE + 1);
          }
        }
      }
      else /* Calibration */
      {
        if(NumberOfCalibration < NUMBER_OF_CALIBRATION)
        {
          Reference += AcquisitionValue;
          NumberOfCalibration++;
        }
        else if(NumberOfCalibration == NUMBER_OF_CALIBRATION)
        {
          Reference += AcquisitionValue;
          Reference /= (NUMBER_OF_CALIBRATION + 1); /* Compute reference */
          NumberOfCalibration = 0; /* Reset number of calibration for nex time */
          CalibrationDone = 1; /* Calibration Completed */
          
          /* Fill reference table */
          for(RefIndex=0;RefIndex<REFERENCE_TAB_SIZE;RefIndex++)
          {
             ReferenceTab[RefIndex] = Reference;
          }
        }
      }
      AcquisitionValue = 0; /* Reset Acquisition value */
      SoftDelay(); /* Wait to discharge sample capacitor before new acquisition */
      TSC->CR |= TSC_CR_START; /* new acquisition, TSC_CR_START = 1 */
    }
  }
}

/**
  * Brief   This function is a delay software.
  * Param   None
  * Retval  None
  */
void SoftDelay(void)
{
  Count = SOFT_DELAY;
  while(Count--);
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
  static uint32_t Counter=0;
  static uint32_t CounterRef=0;
  
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
  

  if((Activities)||(Standby))
  {
    Counter=0;
  }
  else if(Counter < TIME_BEFORE_ADAPTATION)
  {
    Counter++;
  }
  else if(Counter == TIME_BEFORE_ADAPTATION) /* 5s */
  {
    if(CounterRef++ == TIME_BETWEEN_ADAPTATION) /* 100ms */
    {
      CounterRef=0;
      ReferenceAdaptation=1;
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
  * Brief   This function handles EXTI 0 1 interrupt request.
  * Param   None
  * Retval  None
  */
void EXTI0_1_IRQHandler(void)
{
  if((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0)
  {
    /* Clear EXTI 0 flag */
    EXTI->PR = EXTI_PR_PR0;	
	
    Standby = 1 - Standby; /* Toggle stanby mode */
    
    if(Standby)
    {
      GPIOA->BSRR = GPIO_BSRR_BR_5;/* Off LEDs */
      GPIOB->BSRR = GPIO_BSRR_BR_4;/* Off LEDs */
      TSC->CR &= (uint32_t)(~TSC_CR_START);/* Stop acquisition, TSC_CR_START = 0 */
    }
    else
    {
      GPIOB->BSRR = GPIO_BSRR_BS_4; /* Lit green LED */
      Reference = 0; /* Reset reference */
      CalibrationDone = 0; /* To perform calibration */
      TSC->CR |= TSC_CR_START;/* New acquisition, TSC_CR_START = 1 */
    }
  }
}

/**
  * Brief   This function handles TSC interrupt request.
  * Param   None
  * Retval  None
  */
void TSC_IRQHandler(void)
{
  /* End of acquisition flag */
  if((TSC->ISR & TSC_ISR_EOAF) == TSC_ISR_EOAF)
  {
    TSC->ICR = TSC_ICR_EOAIC;/* Clear flag, TSC_ICR_EOAIC = 1 */
    
    AcquisitionValue = TSC->IOGXCR[1];/* Get G2 counter value */
  }
  else
  {
    error = ERROR_TSC;
    NVIC_DisableIRQ(TSC_IRQn);/* Disable TS_IRQn */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
