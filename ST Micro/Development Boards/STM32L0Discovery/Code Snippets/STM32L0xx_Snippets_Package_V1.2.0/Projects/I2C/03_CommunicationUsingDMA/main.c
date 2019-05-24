/**
  ******************************************************************************
  * File     03_CommunicationUsingDMA/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the GPIOs and I2C 
  *          in order to receive with slave and transmit with master, 
  *          both with DMA. 
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
   - GPIO PB6(I2C1_SCL),PB7(I2C1_SDA),PB13(I2C2_SCL),PB14(I2C2_SDA),PA0,PA5,PB4
   - I2C1 (slave), I2C2 (master)
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
   - Plug wires between PB6/PB13 and PB7/PB14 
   - Launch the program
   - Press the user button to initiate a transmit request by master 
     then slave receives bytes
   - The green LED toggles if everything goes well
   - The red LED lit in other cases
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
#define ERROR_I2C 0x01
#define ERROR_HSI_TIMEOUT 0x02
#define ERROR_PLL_TIMEOUT 0x03
#define ERROR_CLKSWITCH_TIMEOUT 0x04

/* I2C */
#define I2C1_OWN_ADDRESS (0x5A)
#define I2C_CMD_TOGGLE_GREEN (0x81)
#define SIZE_OF_DATA  (10)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0;  //initialized at 0 and modified by the functions 

uint8_t datatosend[SIZE_OF_DATA];
uint8_t datatoreceive[SIZE_OF_DATA];

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Configure_GPIO_LED(void);
void Configure_GPIO_I2C1(void);
void Configure_DMA_I2C1(void);
void Configure_I2C1_Slave(void);
void Configure_GPIO_I2C2(void);
void Configure_DMA_I2C2(void);
void Configure_I2C2_Master(void);
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
  
  Configure_GPIO_I2C1();
  Configure_DMA_I2C1();
  Configure_I2C1_Slave();
  Configure_GPIO_I2C2();
  Configure_DMA_I2C2();
  Configure_I2C2_Master();
  Configure_GPIO_Button();
  Configure_EXTI();

  /* Initiate I2C sequence in button IRQ handler */

  /* Infinite loop */
  while (1) 
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
             - Configures the I2C1 pins on GPIO PB6 PB7
  * Param   None
  * Retval  None
  */
__INLINE void Configure_GPIO_I2C1(void)
{
  /* Enable the peripheral clock of GPIOB */
  RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

  /* (1) PU for I2C signals */
  /* (2) open drain for I2C signals */
  /* (3) AF1 for I2C signals */
  /* (4) Select AF mode (10) on PB6 and PB7 */
  GPIOB->PUPDR = (GPIOB->PUPDR & ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7)) \
                 | (GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0); /* (1) */
  GPIOB->OTYPER |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7; /* (2) */
  GPIOB->AFR[0] = (GPIOB->AFR[0] & ~((0xF << ( 6 * 4 )) | ((uint32_t)0xF << ( 7 * 4 )))) \
                  | (1 << ( 6 * 4 )) | (1 << (7 * 4)); /* (3) */
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7)) \
                 | (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1); /* (4) */

}

/**
  * Brief   This function configures DMA for I2C1.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_DMA_I2C1(void)
{
  /* Enable the peripheral clock DMA1 */
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  
   /* DMA1 Channel3 I2C1_RX config */
  /* (1)  Map I2C1_RX DMA channel */
  /* (2)  Peripheral address */
  /* (3)  Memory address */
  /* (4)  Data size */
  /* (5)  Memory increment */
  /*      Peripheral to memory*/
  /*      8-bit transfer */
  /*      Transfer complete IT */
  DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~DMA_CSELR_C3S) | (6 << (2 * 4)); /* (1) */
  DMA1_Channel3->CPAR = (uint32_t)&(I2C1->RXDR); /* (2) */
  DMA1_Channel3->CMAR = (uint32_t)datatoreceive; /* (3) */
  DMA1_Channel3->CNDTR = SIZE_OF_DATA; /* (4) */
  DMA1_Channel3->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (5) */
  
  /* Configure IT */
  /* (6) Set priority for DMA1_Channel2_3_IRQn */
  /* (7) Enable DMA1_Channel2_3_IRQn */
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0); /* (6) */
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn); /* (7) */
}

/**
  * Brief   This function configures I2C1, slave.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_I2C1_Slave(void)
{
  /* Configure RCC for I2C1 */
  /* (1) Enable the peripheral clock I2C1 */
  /* (2) Use APBCLK for I2C CLK */
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; /* (1) */
  RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL; /* (2) */
  
  /* Configure I2C1, slave */
  /* (3) Timing register value is computed with the AN4235 xls file,
         fast Mode @400kHz with I2CCLK = 16MHz, rise time = 100ns, 
         fall time = 10ns */
  /* (4) Periph enable, address match interrupt enable, receive DMA enable */
  /* (5) 7-bit address = 0x5A */
  /* (6) Enable own address 1 */
  I2C1->TIMINGR = (uint32_t)0x00300619; /* (3) */
  I2C1->CR1 = I2C_CR1_PE | I2C_CR1_RXDMAEN | I2C_CR1_ADDRIE; /* (4) */
  I2C1->OAR1 |= (uint32_t)(I2C1_OWN_ADDRESS << 1); /* (5) */
  I2C1->OAR1 |= I2C_OAR1_OA1EN; /* (6) */
  
  /* Configure IT */
  /* (7) Set priority for I2C1_IRQn */
  /* (8) Enable I2C1_IRQn */
  NVIC_SetPriority(I2C1_IRQn, 0); /* (7) */
  NVIC_EnableIRQ(I2C1_IRQn); /* (8) */
}

/**
  * Brief   This function :
             - Enables GPIO clock
             - Configures the I2C2 pins on GPIO PB13 PB14
  * Param   None
  * Retval  None
  */
__INLINE void Configure_GPIO_I2C2(void)
{
  /* Enable the peripheral clock of GPIOB */
  RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
	
  /* (1) PU for I2C signals */
  /* (2) open drain for I2C signals */
  /* (3) AF5 for I2C signals */
  /* (4) Select AF mode (10) on PB13 and PB14 */
  GPIOB->PUPDR = (GPIOB->PUPDR & ~(GPIO_PUPDR_PUPD13 | GPIO_PUPDR_PUPD14)) \
                 | (GPIO_PUPDR_PUPD13_0 | GPIO_PUPDR_PUPD14_0); /* (1) */
  GPIOB->OTYPER |= GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_14; /* (2) */
  GPIOB->AFR[1] = (GPIOB->AFR[1] & ~((0xF << ( 5 * 4 )) | (0xF << ( 6 * 4 )))) \
                  | (5 << ( 5 * 4 )) | (5 << ( 6 * 4 )); /* (3) */
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE13 | GPIO_MODER_MODE14)) \
                 | (GPIO_MODER_MODE13_1 | GPIO_MODER_MODE14_1); /* (4) */
  
}

/**
  * Brief   This function configures DMA for I2C2.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_DMA_I2C2(void)
{
  /* Enable the peripheral clock DMA1 */
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  
  /* DMA1 Channel4 I2C2_TX config */
  /* (1)  Map I2C2_TX DMA channel */
  /* (2)  Peripheral address */
  /* (3)  Memory address */
  /* (4)  Memory increment */
  /*      Memory to peripheral */
  /*      8-bit transfer */
  DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~DMA_CSELR_C4S) | (7 << (3 * 4)); /* (1) */
  DMA1_Channel4->CPAR = (uint32_t)&(I2C2->TXDR); /* (2) */
  DMA1_Channel4->CMAR = (uint32_t)datatosend; /* (3) */
  DMA1_Channel4->CCR |= DMA_CCR_MINC | DMA_CCR_DIR; /* (4) */
}

/**
  * Brief   This function configures I2C2, master.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_I2C2_Master(void)
{
  /* Enable the peripheral clock I2C2 */
  RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

  /* Configure I2C2, master */
  /* (1) Timing register value is computed with the AN4235 xls file,
         fast Mode @400kHz with I2CCLK = 16MHz, rise time = 100ns, 
         fall time = 10ns */
  /* (2) Periph enable, transmit DMA enable */
  /* (3) Slave address = 0x5A, read transfer, 1 byte to receive, autoend */
  I2C2->TIMINGR = (uint32_t)0x00300619; /* (1) */
  I2C2->CR1 = I2C_CR1_PE | I2C_CR1_TXDMAEN; /* (2) */
  I2C2->CR2 =  I2C_CR2_AUTOEND | (SIZE_OF_DATA << 16) | (I2C1_OWN_ADDRESS<<1); /* (3) */
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
	
    datatosend[SIZE_OF_DATA-1] = I2C_CMD_TOGGLE_GREEN;
    
    /* start I2C master transmission sequence */
    if((I2C2->ISR & I2C_ISR_TXE) == (I2C_ISR_TXE)) /* Check Tx empty */
    {
      DMA1_Channel4->CCR &=~ DMA_CCR_EN;
      DMA1_Channel4->CNDTR = SIZE_OF_DATA;/* Data size */
      DMA1_Channel4->CCR |= DMA_CCR_EN;
      
      I2C2->CR2 |= I2C_CR2_START; /* Go */
    }
  }
}

/**
  * Brief   This function handles I2C1 interrupt request.
  * Param   None
  * Retval  None
  */
void I2C1_IRQHandler(void)
{
  uint32_t I2C_InterruptStatus = I2C1->ISR; /* Get interrupt status */
  
  if((I2C_InterruptStatus & I2C_ISR_ADDR) == I2C_ISR_ADDR)
  {
    I2C1->ICR |= I2C_ICR_ADDRCF; /* Address match event */
  }
  else
  {
    error = ERROR_I2C; /* Report an error */
  }
}

/**
  * Brief   This function handles DMA1 channel 2 and 3 interrupt request.
  * Param   None
  * Retval  None
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  if((DMA1->ISR & DMA_ISR_TCIF3) == DMA_ISR_TCIF3)
  {
    DMA1->IFCR |= DMA_IFCR_CTCIF3;/* Clear TC flag */

    if(datatoreceive[SIZE_OF_DATA-1] == I2C_CMD_TOGGLE_GREEN)
    {
      datatoreceive[SIZE_OF_DATA-1] = 0;
      GPIOB->ODR ^= (1<<4); /* toggle green LED */
    }
    else
    {
      error = ERROR_I2C; /* Report an error */
    }
    
    DMA1_Channel3->CCR &=~ DMA_CCR_EN;
    DMA1_Channel3->CNDTR = SIZE_OF_DATA;/* Data size */
    DMA1_Channel3->CCR |= DMA_CCR_EN;
  }
  else
  {
    error = ERROR_I2C; /* Report an error */
    NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);/* Disable DMA1_Channel2_3_IRQn */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
