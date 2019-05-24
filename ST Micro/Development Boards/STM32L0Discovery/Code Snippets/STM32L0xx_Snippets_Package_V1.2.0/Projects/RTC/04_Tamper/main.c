/**
  ******************************************************************************
  * File     04_Tamper/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to configure the RTC
  *          in order to have a tamper detection.
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
   - GPIO  PA9(USART1_TX),PA10(USART1_RX), PA0 (tamper mode), PA5, PB4,
   - RTC
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
   - Plug cable " USB to TTL 3V3 " (from FTDIChip)
   - Connect FTDI Rx to USART1 Tx(PA9)and FTDI Tx to USART1 Rx(PA10)
   - Launch serial communication SW
   - PC interface is configured as follows:
      - Data Length = 8 Bits
      - One Stop Bit
      - None parity
      - BaudRate = 9600 baud
      - Flow control: None
   - Launch the program
   - The green LED is blinking Green to indicate that RTC is well configured 
     and time is displayed on PC interface
   - Press the user button (tamper detection) to enter in the RTC init mode
   - Follow the step on the interface to change the RTC time
   - note: there is no LSE on board so the LSI is used instead, please keep in mind
     to have a precise time the LSE has to be used in a normal application
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
#include "string.h"

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
#define ERROR_RTC 0x01
#define ERROR_HSI_TIMEOUT 0x02
#define ERROR_PLL_TIMEOUT 0x03
#define ERROR_CLKSWITCH_TIMEOUT 0x04


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0;  //initialized at 0 and modified by the functions 

uint8_t send = 0;
uint8_t stringtosend[20] = "\n00 : 00 : 00 ";
uint8_t RTC_InitializationMode = 0;
uint8_t CharToReceive = 0;
uint8_t CharReceived = 0;
uint8_t Alarm = 1; /* set to 1 for the first print */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Configure_GPIO_LED(void);
void Configure_GPIO_USART1(void);
void Configure_USART1(void);
void Configure_RTC(void);
void Init_RTC(uint32_t Time);
void Process(void);

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
  
  Configure_GPIO_USART1();
  Configure_USART1();
  Configure_RTC();
  Init_RTC(0);
	
  /* Infinite loop */
  while(1)
  {
    Process();
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
  
  /* lit green LED */
  GPIOB->BSRR = GPIO_BSRR_BS_4;
}

/**
  * Brief   This function :
             - Enables GPIO clock
             - Configures the USART1 pins on GPIO PA9 PA10
  * Param   None
  * Retval  None
  */
__INLINE void Configure_GPIO_USART1(void)
{
  /* Enable the peripheral clock of GPIOA */
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	
  /* GPIO configuration for USART1 signals */
  /* (1) Select AF mode (10) on PA9 and PA10 */
  /* (2) AF4 for USART1 signals */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10))\
                 | (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1); /* (1) */
  GPIOA->AFR[1] = (GPIOA->AFR[1] &~ ((0xF << (1 * 4)) | (0xF << (2 * 4))))\
                  | (4 << (1 * 4)) | (4 << (2 * 4)); /* (2) */
}

/**
  * Brief   This function configures USART1.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_USART1(void)
{
  /* Enable the peripheral clock USART1 */
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  /* Configure USART1 */
  /* (1) oversampling by 16, 9600 baud */
  /* (2) 8 data bit, 1 start bit, 1 stop bit, no parity */
  USART1->BRR = 160000 / 96; /* (1) */
  USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; /* (2) */
  
  /* polling idle frame Transmission */
  while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC)
  { 
    /* add time out here for a robust application */
  }
  USART1->ICR = USART_ICR_TCCF; /* clear TC flag */
  USART1->CR1 |= USART_CR1_TCIE; /* enable TC interrupt */
  
  /* Configure IT */
  /* (3) Set priority for USART1_IRQn */
  /* (4) Enable USART1_IRQn */
  NVIC_SetPriority(USART1_IRQn, 0); /* (3) */
  NVIC_EnableIRQ(USART1_IRQn); /* (4) */
}

/**
  * Brief   This function configures RTC.
  * Param   None
  * Retval  None
  */
__INLINE void Configure_RTC(void)
{
  /* Enable the peripheral clock RTC */
  /* (1) Enable the LSI */
  /* (2) Wait while it is not ready */
  /* (3) Enable PWR clock */
  /* (4) Enable write in RTC domain control register */
  /* (5) LSI for RTC clock */
  /* (6) Disable PWR clock */
  RCC->CSR |= RCC_CSR_LSION; /* (1) */
  while((RCC->CSR & RCC_CSR_LSIRDY)!=RCC_CSR_LSIRDY) /* (2) */
  { 
    /* add time out here for a robust application */
  }
  RCC->APB1ENR |= RCC_APB1ENR_PWREN; /* (3) */
  PWR->CR |= PWR_CR_DBP; /* (4) */
  RCC->CSR = (RCC->CSR & ~RCC_CSR_RTCSEL) | RCC_CSR_RTCEN | RCC_CSR_RTCSEL_1; /* (5) */
  RCC->APB1ENR &=~ RCC_APB1ENR_PWREN; /* (7) */

  /* Configure RTC */
  /* (7) Write access for RTC regsiters */
  /* (8) Disable alarm A to modify it */
  /* (9) Wait until it is allow to modify alarm A value */
  /* (10) Modify alarm A mask to have an interrupt each 1Hz */
  /* (11) Enable alarm A and alarm A interrupt */
  /* (12) Disable write access */
  /* (13) Tamper configuration:
          - Disable precharge (PU)
          - RTCCLK/256 tamper sampling frequency
          - Activate time stamp on tamper detection
          - input rising edge trigger detection on RTC_TAMP2 (PA0)
          - Tamper interrupt enable */
  RTC->WPR = 0xCA; /* (7) */
  RTC->WPR = 0x53; /* (7) */
  RTC->CR &=~ RTC_CR_ALRAE; /* (8) */
  while((RTC->ISR & RTC_ISR_ALRAWF) != RTC_ISR_ALRAWF) /* (9) */
  { 
    /* add time out here for a robust application */
  }
  RTC->ALRMAR = RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK1; /* (10) */
  RTC->CR = RTC_CR_ALRAIE | RTC_CR_ALRAE; /* (11) */ 
  RTC->WPR = 0xFE; /* (12) */
  RTC->WPR = 0x64; /* (12) */
  RTC->TAMPCR = RTC_TAMPCR_TAMPPUDIS | RTC_TAMPCR_TAMPFREQ | RTC_TAMPCR_TAMPTS | RTC_TAMPCR_TAMP2E | RTC_TAMPCR_TAMPIE; /* (13) */
  
  /* Configure exti and nvic for RTC IT */
  /* (14) unmask line 17 */
  /* (15) Rising edge for line 17 */
  /* (16) unmask line 19 */
  /* (17) Rising edge for line 19 */
  /* (18) Set priority */
  /* (19) Enable RTC_IRQn */
  EXTI->IMR |= EXTI_IMR_IM17; /* (14) */ 
  EXTI->RTSR |= EXTI_RTSR_TR17; /* (15) */ 
  EXTI->IMR |= EXTI_IMR_IM19; /* (16) */ 
  EXTI->RTSR |= EXTI_RTSR_TR19; /* (17) */ 
  NVIC_SetPriority(RTC_IRQn, 0); /* (18) */ 
  NVIC_EnableIRQ(RTC_IRQn); /* (19) */
}

/**
  * Brief   This function configures RTC.
  * Param   uint32_t New time
  * Retval  None
  */
__INLINE void Init_RTC(uint32_t Time)
{
  /* RTC init mode */
  /* Configure RTC */
  /* (1) Write access for RTC registers */
  /* (2) Enable init phase */
  /* (3) Wait until it is allow to modify RTC register values */
  /* (4) set prescaler, 40kHz/64 => 625 Hz, 625Hz/625 => 1Hz */
  /* (5) New time in TR */
  /* (6) Disable init phase */
  /* (7) Disable write access for RTC registers */
  RTC->WPR = 0xCA; /* (1) */ 
  RTC->WPR = 0x53; /* (1) */
  RTC->ISR = RTC_ISR_INIT; /* (2) */
  while((RTC->ISR & RTC_ISR_INITF)!=RTC_ISR_INITF) /* (3) */
  { 
    /* add time out here for a robust application */
  }
  RTC->PRER = 0x003F0270; /* (4) */
  RTC->TR = RTC_TR_PM | Time; /* (5) */
  RTC->ISR =~ RTC_ISR_INIT; /* (6) */
  RTC->WPR = 0xFE; /* (7) */
  RTC->WPR = 0x64; /* (7) */
}

/**
  * Brief   This function processes RTC with USART.
  * Param   None
  * Retval  None
  */
void Process(void)
{
  volatile uint32_t TimeToCompute = 0;
  static uint32_t NewTime=0;
  
  switch(RTC_InitializationMode)
  {
  case 0:
    /* check alarm and synchronisation flag */
    if((Alarm)&&((RTC->ISR & RTC_ISR_RSF) == RTC_ISR_RSF))
    {
      Alarm=0;
      
      TimeToCompute = RTC->TR; /* get time */
      RTC->DR; /* need to read date also */
      
      stringtosend[1] = (uint8_t)(((TimeToCompute & RTC_TR_HT)>>20) + 48);/* hour tens */
      stringtosend[2] = (uint8_t)(((TimeToCompute & RTC_TR_HU)>>16) + 48);/* hour units */
      stringtosend[6] = (uint8_t)(((TimeToCompute & RTC_TR_MNT)>>12) + 48);/* minute tens */
      stringtosend[7] = (uint8_t)(((TimeToCompute & RTC_TR_MNU)>>8) + 48);/* minute units */
      stringtosend[11] = (uint8_t)(((TimeToCompute & RTC_TR_ST)>>4) + 48);/* second tens */
      stringtosend[12] = (uint8_t)((TimeToCompute & RTC_TR_SU) + 48);/* second units */
      
      /* start USART transmission */
      USART1->TDR = stringtosend[send++]; /* Will inititiate TC if TXE */
    }
  break;
  case 1:      
    {
      if(!send)
      {
        strcpy((char *)stringtosend,"\nSend hour tens\n");
        USART1->TDR = stringtosend[send++]; /* Will inititiate TC if TXE */
        RTC_InitializationMode=2;
      }
    }
    break;
  case 2: /* Hour tens */
    {
      if(CharReceived)
      {
        CharReceived=0;
        CharToReceive -= 48;
        if(CharToReceive<3)
        {
          NewTime = CharToReceive << 20;
          RTC_InitializationMode=3;
        }
        else RTC_InitializationMode=1;
      }
    }
    break;
  case 3:      
    {
      if(!send)
      {
        strcpy((char *)stringtosend,"\nSend hour units\n");
        USART1->TDR = stringtosend[send++]; /* Will inititiate TC if TXE */
        RTC_InitializationMode=4;
      }
    }
    break;
  case 4: /* Hour units */
    {
       if(CharReceived)
      {
        CharReceived=0;
        CharToReceive -= 48;
        if((((NewTime>>20) == 2) && (CharToReceive<4)) ||
            (((NewTime>>20) < 2) && (CharToReceive<10)))
        {
          NewTime |= CharToReceive << 16;
          RTC_InitializationMode=5;
        }
        else RTC_InitializationMode=3;
      }
    }
    break;
  case 5:      
    {
      if(!send)
      {
        strcpy((char *)stringtosend,"\nSend minute tens\n");
        USART1->TDR = stringtosend[send++]; /* Will inititiate TC if TXE */
        RTC_InitializationMode=6;
      }
    }
    break;
  case 6: /* Minute tens */
    {
      if(CharReceived)
      {
        CharReceived=0;
        CharToReceive -= 48;
        if(CharToReceive<6)
        {
          NewTime |= CharToReceive << 12;
          RTC_InitializationMode=7;
        }
        else RTC_InitializationMode=5;
      }
    }
    break;
  case 7:      
    {
      if(!send)
      {
        strcpy((char *)stringtosend,"\nSend minute units\n");
        USART1->TDR = stringtosend[send++]; /* Will inititiate TC if TXE */
        RTC_InitializationMode=8;
      }
    }
    break;
  case 8: /* Minute units */
    {
      if(CharReceived)
      {
        CharReceived=0;
        CharToReceive -= 48;
        if(CharToReceive<10)
        {
          NewTime |= CharToReceive << 8;
          RTC_InitializationMode=9;
        }
        else RTC_InitializationMode=7;
      }
    }
    break;
  case 9:      
    {
      if(!send)
      {
        strcpy((char *)stringtosend,"\nSend second tens\n");
        USART1->TDR = stringtosend[send++]; /* Will inititiate TC if TXE */
        RTC_InitializationMode=10;
      }
    }
    break;
  case 10: /* Second tens */
    {
       if(CharReceived)
      {
        CharReceived=0;
        CharToReceive -= 48;
        if(CharToReceive<6)
        {
          NewTime |= CharToReceive << 4;
          RTC_InitializationMode=11;
        }
        else RTC_InitializationMode=9;
      }
    }
    break;
  case 11:      
    {
      if(!send)
      {
        strcpy((char *)stringtosend,"\nSend second units\n");
        USART1->TDR = stringtosend[send++]; /* Will inititiate TC if TXE */
        RTC_InitializationMode=12;
      }
    }
    break;
  case 12: /* Second Units */
    {
      if(CharReceived)
      {
        CharReceived=0;
        CharToReceive -= 48;
        if(CharToReceive<10)
        {
          NewTime |= CharToReceive;
          RTC_InitializationMode=13;
        }
        else RTC_InitializationMode=11;
      }
    }
    break;
  case 13:
    {
      Init_RTC(NewTime);
      strcpy((char *)stringtosend,"\n-- : -- : --      ");
      RTC_InitializationMode=0;
    }
    break;
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
	
    RTC_InitializationMode = 1;
  }
}

/**
  * Brief   This function handles USART1 interrupt request.
  * Param   None
  * Retval  None
  */
void USART1_IRQHandler(void)
{
  if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
  {
    CharToReceive = (uint8_t)(USART1->RDR); /* Receive data, clear flag */
    CharReceived = 1;      
  }
  else if((USART1->ISR & USART_ISR_TC) == USART_ISR_TC)
  {
    if(send == sizeof(stringtosend))
    {
      send=0;
      USART1->ICR = USART_ICR_TCCF; /* Clear transfer complete flag */
    }
    else
    {
      /* clear transfer complete flag and fill TDR with a new char */
      USART1->TDR = stringtosend[send++];
    }
  }
  else
  {
      NVIC_DisableIRQ(USART1_IRQn); /* Disable USART1_IRQn */
  }
	
}

/**
  * Brief   This function handles RTC interrupt request.
  * Param   None
  * Retval  None
  */
void RTC_IRQHandler(void)
{
  /* Check alarm A flag */
  if((RTC->ISR & (RTC_ISR_ALRAF)) == (RTC_ISR_ALRAF))
  {
    RTC->ISR =~ RTC_ISR_ALRAF; /* clear flag */
    EXTI->PR = EXTI_PR_PR17; /* clear exti line 17 flag */
    GPIOB->ODR ^= (1 << 4) ; /* Toggle Green LED */
    Alarm = 1;
  }
  /* Check tamper and timestamp flag */
  else if(((RTC->ISR & (RTC_ISR_TAMP2F)) == (RTC_ISR_TAMP2F)) && ((RTC->ISR & (RTC_ISR_TSF)) == (RTC_ISR_TSF))) 
  {
    RTC->ISR =~ (RTC_ISR_TAMP2F); /* clear tamper flag */
    EXTI->PR = EXTI_PR_PR19; /* clear exti line 17 flag */
    RTC_InitializationMode = 1;
  }
  else
  {
    error = ERROR_RTC;
    NVIC_DisableIRQ(RTC_IRQn);/* Disable RTC_IRQn */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
