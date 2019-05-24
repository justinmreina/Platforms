/**
  ******************************************************************************
  * File     02_CommunicationUsingDMA/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the GPIOs and SPI1(master)
  *          and SPI2 (slave) in full duplex communication in order to send and 
  *          receive bytes in DMA mode.
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
   - GPIO  PA0, PC6, PC7, PC8, PC9,
           PA4(SPI1_NSS), PB3(SPI1_SCK), PA6(SPI1_MISO), PA7(SPI1_MOSI), 
           PB12(SPI2_NSS), PB13(SPI2_SCK), PB14(SPI2_MISO), PB15(SPI2_MOSI)
   - SPI1, SPI2
   - DMA1
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
   - Solder SB31 and SB32 on the board
   - Unplug the electronic paper display
   - Connect PA4/PB12, PB3/PB13, PA6/PB14, PA7/PB15.
   - Launch the program
   - Press the button
   - The green LED toggles (message transmit and receive)
   - Else the red LED blink
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
#include "string.h" /* for strcmp */
#include "stdlib.h" /* for malloc */


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
#define ERROR_SPI 0x01
#define ERROR_HSI_TIMEOUT 0x02
#define ERROR_PLL_TIMEOUT 0x03
#define ERROR_CLKSWITCH_TIMEOUT 0x04


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0;  //initialized at 0 and modified by the functions 

const uint8_t stringtosendwSPI1[] = "DMA with SPI 1"; /* Both strings have same length */
const uint8_t stringtosendwSPI2[] = "And with SPI 2"; /* Receive and transmit are done in the same transaction */
uint8_t * stringtoreceivewSPI1;
uint8_t * stringtoreceivewSPI2;
volatile uint8_t SPI1_StringReceived = 0;
volatile uint8_t SPI2_StringReceived = 0;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Configure_GPIO_LED(void);
void Configure_GPIO_SPI1(void);
void Configure_DMA1_SPI1(void);
void Configure_SPI1(void);
void Configure_GPIO_SPI2(void);
void Configure_DMA1_SPI2(void);
void Configure_SPI2(void);
void Configure_GPIO_Button(void);
void Configure_EXTI(void);

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
  
  Configure_GPIO_SPI1();
  Configure_DMA1_SPI1();
  Configure_SPI1();
  Configure_GPIO_SPI2();
  Configure_DMA1_SPI2();
  Configure_SPI2();
  Configure_GPIO_Button();
  Configure_EXTI();
	
  /* Start transmission in button IRQ handler */
	
  /* Infinite loop */
  while(1)
  {
    if(SPI1_StringReceived && SPI2_StringReceived)
    {
      SPI1_StringReceived=0;
      SPI2_StringReceived=0;
      
      if ((strcmp((const char *)stringtosendwSPI2,(const char *)stringtoreceivewSPI1) == 0)
       && (strcmp((const char *)stringtosendwSPI1,(const char *)stringtoreceivewSPI2) == 0))
      {
        GPIOB->ODR ^= (1 << 4); /* toggle green LED */
      }
      else
      {
        error = ERROR_SPI; /* Report an error */
      }
    }
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
             - Configures the SPI1 pins on GPIO PA4 PB3 PA6 PA7
  * Param   None
  * Retval  None
  */
__INLINE void Configure_GPIO_SPI1(void)
{
  /* Enable the peripheral clock of GPIOA and GPIOB */
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
  RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
	
  /* (1) Select AF mode (10) on PA4, PA6, PA7 */
  /* (2) AF0 for SPI1 signals */
  /* (3) Select AF mode (10) on PB3 */
  /* (4) AF0 for SPI1 signals */
  GPIOA->MODER = (GPIOA->MODER 
                  & ~(GPIO_MODER_MODE4 | \
                      GPIO_MODER_MODE6 | GPIO_MODER_MODE7))\
                  | (GPIO_MODER_MODE4_1 | \
                     GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1); /* (1) */
  GPIOA->AFR[0] = (GPIOA->AFR[0] & \
                   ~((0xF<<(4*4)) | \
                     (0xF<<(4*6)) | ((uint32_t)0xF<<(4*7)))); /* (2) */
   GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE3)) | GPIO_MODER_MODE3_1; /* (3) */
   GPIOB->AFR[0] = (GPIOB->AFR[0] & ~((0xF<<(4*3)))); /* (4) */
}

/**
  * Brief   This function configures DMA for SPI1.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_DMA1_SPI1(void)
{
  /* Enable the peripheral clock DMA11 */
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  
  /* DMA1 Channel2 SPI1_RX config */
  /* (1)  Map SPI1_RX DMA channel */
  /* (2) Peripheral address */
  /* (3) Memory address */
  /* (4) Data size */
  /* (5) Memory increment */
  /*     Peripheral to memory */
  /*     8-bit transfer */
  /*     Transfer complete IT */
  DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~DMA_CSELR_C2S) | (1 << (1 * 4)); /* (1) */
  DMA1_Channel2->CPAR = (uint32_t)&(SPI1->DR); /* (2) */
  stringtoreceivewSPI1 = malloc(sizeof(stringtosendwSPI2));
  DMA1_Channel2->CMAR = (uint32_t)stringtoreceivewSPI1; /* (3) */
  DMA1_Channel2->CNDTR = sizeof(stringtosendwSPI2); /* (4) */
  DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE  | DMA_CCR_EN; /* (5) */
  
  /* DMA1 Channel3 SPI1_TX config */
  /* (6)  Map SPI1_TX DMA channel */
  /* (7) Peripheral address */
  /* (8) Memory address */
  /* (9) Memory increment */
  /*     Memory to peripheral*/
  /*     8-bit transfer */
  DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~DMA_CSELR_C3S) | (1 << (2 * 4)); /* (6) */
  DMA1_Channel3->CPAR = (uint32_t)&(SPI1->DR); /* (7) */
  DMA1_Channel3->CMAR = (uint32_t)stringtosendwSPI1; /* (8) */
  DMA1_Channel3->CCR |= DMA_CCR_MINC | DMA_CCR_DIR; /* (9) */
  
  /* Configure IT */
  /* (10) Set priority for DMA1_Channel2_3_IRQn */
  /* (11) Enable DMA1_Channel2_3_IRQn */
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0); /* (10) */
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn); /* (11) */
}

/**
  * Brief   This function configures SPI1.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_SPI1(void)
{
  /* Enable the peripheral clock SPI1 */
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  /* Configure SPI1 in master */
  /* (1) Master selection, BR: Fpclk/256 (due to C13 on the board, SPI_CLK is set to the minimum)
         CPOL and CPHA at zero (rising first edge), 8-bit data frame */
  /* (2) Slave select output enabled, DMA in reception and transmission */
  /* (3) Enable SPI1 */
  SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR; /* (1) */
  SPI1->CR2 = SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN | SPI_CR2_SSOE; /* (2) */
  SPI1->CR1 |= SPI_CR1_SPE; /* (3) */
}

/**
  * Brief   This function :
             - Enables GPIO clock
             - Configures the SPI2 pins on GPIO PB12 PB13 PB14 PB15
  * Param   None
  * Retval  None
  */
__INLINE void Configure_GPIO_SPI2(void)
{
  /* Enable the peripheral clock of GPIOB */
  RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
	
  /* (1) Select AF mode (10) on PB12, PB13, PB14, PB15 */
  /* (2) AF0 for SPI2 signals */
  GPIOB->MODER = (GPIOB->MODER 
                 & ~(GPIO_MODER_MODE12 | GPIO_MODER_MODE13 | \
                     GPIO_MODER_MODE14 | GPIO_MODER_MODE15))\
                 | (GPIO_MODER_MODE12_1 | GPIO_MODER_MODE13_1|\
                    GPIO_MODER_MODE14_1 | GPIO_MODER_MODE15_1); /* (1) */
  GPIOB->AFR[1] = (GPIOB->AFR[1] & \
                   ~((0xF<<(4*4)) | (0xF<<(4*5)) |\
                     (0xF<<(4*6)) | ((uint32_t)0xF<<(4*7)))); /* (2) */
}

/**
  * Brief   This function configures DMA for SPI2.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_DMA1_SPI2(void)
{
  /* Enable the peripheral clock DMA11 */
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  
  /* DMA1 Channel4 SPI2_RX config */
  /* (1)  Map SPI2_RX DMA channel */
  /* (2) Peripheral address */
  /* (3) Memory address */
  /* (4) Data size */
  /* (5) Memory increment */
  /*     Peripheral to memory */
  /*     8-bit transfer */
  /*     Transfer complete IT */
  DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~DMA_CSELR_C4S) | (2 << (3 * 4)); /* (1) */
  DMA1_Channel4->CPAR = (uint32_t)&(SPI2->DR); /* (2) */
  stringtoreceivewSPI2 = malloc(sizeof(stringtosendwSPI1));
  DMA1_Channel4->CMAR = (uint32_t)stringtoreceivewSPI2; /* (3) */
  DMA1_Channel4->CNDTR = sizeof(stringtosendwSPI1); /* (4) */
  DMA1_Channel4->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE  | DMA_CCR_EN; /* (5) */
  
  /* DMA1 Channel5 SPI2_TX config */
  /* (6)  Map SPI2_TX DMA channel */
  /* (7) Peripheral address */
  /* (8) Memory address */
  /* (9) Memory increment */
  /*     Memory to peripheral*/
  /*     8-bit transfer */
  DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~DMA_CSELR_C5S) | (2 << (4 * 4)); /* (6) */
  DMA1_Channel5->CPAR = (uint32_t)&(SPI2->DR); /* (7) */
  DMA1_Channel5->CMAR = (uint32_t)stringtosendwSPI2; /* (8) */
  DMA1_Channel5->CCR |= DMA_CCR_MINC | DMA_CCR_DIR; /* (9) */
  
  /* Configure IT */
  /* (10) Set priority for DMA1_Channel4_5_6_7_IRQn */
  /* (11) Enable DMA1_Channel4_5_6_7_IRQn */
  NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0); /* (10) */
  NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn); /* (11) */

}

/**
  * Brief   This function configures SPI2.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_SPI2(void)
{
   /* Enable the peripheral clock SPI2 */
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

  /* Configure SPI2 in slave */
  /* nSS hard, slave, CPOL and CPHA at zero (rising first edge), 8-bit */
  /* (1)  TX and RX with DMA */
  /* (2) Enable SPI2 */
  SPI2->CR2 = SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN; /* (1) */
  SPI2->CR1 |= SPI_CR1_SPE; /* (2) */
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
	
    /* start 8-bit transmission with DMA*/
    /* Prepare Slave */
    DMA1_Channel5->CCR &=~ DMA_CCR_EN;
    DMA1_Channel5->CNDTR = sizeof(stringtosendwSPI2); /* Data size */
    DMA1_Channel5->CCR |= DMA_CCR_EN;
    /* Prepare master */
    DMA1_Channel3->CCR &=~ DMA_CCR_EN;
    DMA1_Channel3->CNDTR = sizeof(stringtosendwSPI1); /* Data size */
    DMA1_Channel3->CCR |= DMA_CCR_EN;
  }
}

/**
  * Brief   This function handles DMA1 channel 2 TC interrupt request.
  * Param   None
  * Retval  None
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  if((DMA1->ISR & DMA_ISR_TCIF2) == DMA_ISR_TCIF2)
  {
    DMA1->IFCR |= DMA_IFCR_CTCIF2; /* Clear TC flag */

    SPI1_StringReceived = 1;
    
    DMA1_Channel2->CCR &=~ DMA_CCR_EN;
    DMA1_Channel2->CNDTR = sizeof(stringtosendwSPI2); /* Data size */
    DMA1_Channel2->CCR |= DMA_CCR_EN;
  }
  else
  {
    error = ERROR_SPI; /* Report an error */
    NVIC_DisableIRQ(DMA1_Channel2_3_IRQn); /* Disable DMA1_Channel2_3_IRQn */
  }
}

/**
  * Brief   This function handles DMA1 channel 4 TC interrupt request.
  * Param   None
  * Retval  None
  */
void DMA1_Channel4_5_6_7_IRQHandler(void)
{
  if((DMA1->ISR & DMA_ISR_TCIF4) == DMA_ISR_TCIF4)
  {
    DMA1->IFCR |= DMA_IFCR_CTCIF4; /* Clear TC flag */

    SPI2_StringReceived = 1;
    
    DMA1_Channel4->CCR &=~ DMA_CCR_EN;
    DMA1_Channel4->CNDTR = sizeof(stringtosendwSPI1); /* Data size */
    DMA1_Channel4->CCR |= DMA_CCR_EN;
  }
  else
  {
    error = ERROR_SPI; /* Report an error */
    NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn); /* Disable DMA1_Channel4_5_IRQn */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
