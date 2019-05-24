/**
  ******************************************************************************
  * File     02_EepromEraseAndProg/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to erase and program the Data eeprom 
  *          memory
  *
 ===============================================================================
                    #####       MCU Resources     #####
 ===============================================================================
   - Data Eeprom memory
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
    - This example erases the target first data Eeprom word and program it 
      with a defined value.
      The programming is performed writng a byte, then a 16-bit word and 
      finally a 32-bit word.
      This test is done twice in case the erase value (OxFFFF) was already 
      programmed at the first address of the page before the erasing sequence.
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
/* NVM key definitions */
#define FLASH_PDKEY1               ((uint32_t)0x04152637) /*!< Flash power down key1 */
#define FLASH_PDKEY2               ((uint32_t)0xFAFBFCFD) /*!< Flash power down key2: used with FLASH_PDKEY1 
                                                              to unlock the RUN_PD bit in FLASH_ACR */

#define FLASH_PEKEY1               ((uint32_t)0x89ABCDEF) /*!< Flash program erase key1 */
#define FLASH_PEKEY2               ((uint32_t)0x02030405) /*!< Flash program erase key: used with FLASH_PEKEY2
                                                               to unlock the write access to the FLASH_PECR register and
                                                               data EEPROM */

#define FLASH_PRGKEY1              ((uint32_t)0x8C9DAEBF) /*!< Flash program memory key1 */
#define FLASH_PRGKEY2              ((uint32_t)0x13141516) /*!< Flash program memory key2: used with FLASH_PRGKEY2
                                                               to unlock the program memory */

#define FLASH_OPTKEY1              ((uint32_t)0xFBEAD9C8) /*!< Flash option key1 */
#define FLASH_OPTKEY2              ((uint32_t)0x24252627) /*!< Flash option key2: used with FLASH_OPTKEY1 to
                                                              unlock the write access to the option byte block */


/* Time-out values */
#define HSI_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
#define PLL_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
#define CLOCKSWITCH_TIMEOUT_VALUE  ((uint32_t)5000) /* 5 s    */


#define DATA_E2_ADDR   ((uint32_t)0x08080000)  /* Data EEPROM address */
#define DATA_BYTE            ((uint8_t)0x12)  /* 8-bits value to be programmed */
#define DATA_16B_WORD            ((uint16_t)0x3456)  /* 16-bits value to be programmed */
#define DATA_32B_WORD            ((uint32_t)0x89ABCDEF)  /* 32-bits value to be programmed */

/* Error codes used to make the red led blinking */
#define ERROR_ERASE 0x01
#define ERROR_PROG_BYTE  0x02
#define ERROR_PROG_16B_WORD 0x04
#define ERROR_PROG_32B_WORD 0x08
#define ERROR_WRITE_PROTECTION 0x10
#define ERROR_READOUT_PROTECTION 0x20
#define ERROR_FETCH_DURING_PROG 0x40
#define ERROR_SIZE 0x80
#define ERROR_ALIGNMENT 0x100
#define ERROR_NOT_ZERO 0x200
#define ERROR_OPTION_NOT_VALID 0x400
#define ERROR_UNKNOWN 0x800

#define ERROR_HSI_TIMEOUT 0x55
#define ERROR_PLL_TIMEOUT 0xAA
#define ERROR_CLKSWITCH_TIMEOUT 0xBB

#define FLASH_SR_FWWERR ((uint32_t)0x00020000)

/* Delay value : short one is used for the error coding, long one in case of no error
   or between two bursts */
#define SHORT_DELAY 100
#define LONG_DELAY 1000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error = 0;
uint32_t test_to_be_performed_twice = 1; //this variable is set to 2 if the first address of the page to erase is yet erased
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void ConfigureGPIO(void);
void UnlockPELOCK(void);
void LockNVM(void);
void EepromErase(uint32_t addr);
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
  if (error != 0)
  {
    while(1) /* endless loop */
    {
    }
  }
  SysTick_Config(16000); /* 1ms config */
  UnlockPELOCK();
  FLASH->PECR |= FLASH_PECR_ERRIE | FLASH_PECR_EOPIE; /* enable flash interrupts */
  /* Check if the first address of the page is yet erased,this is only for this example */
  if (*(uint32_t *)(DATA_E2_ADDR) == (uint32_t)0) 
  {
    test_to_be_performed_twice = 2;
  }
  
  while (test_to_be_performed_twice-- > 0)
  {
    EepromErase(DATA_E2_ADDR);
    __WFI();
    if (*(uint32_t *)(DATA_E2_ADDR) != (uint32_t)0)
    {
      error |= ERROR_ERASE;
    }
    /* Perform data programming */
    /* (1) Write in data EEPROM */
    *(uint8_t *)(DATA_E2_ADDR+1) = DATA_BYTE; /* (1) */
    __WFI();
    if  (*(uint8_t *)(DATA_E2_ADDR+1) != DATA_BYTE) 
    {
      error |= ERROR_PROG_BYTE;
    }
    *(uint16_t *)(DATA_E2_ADDR+2) = DATA_16B_WORD; /* (1) */
    __WFI();
    if  (*(uint32_t *)(DATA_E2_ADDR) != ((DATA_BYTE << 8) + (DATA_16B_WORD << 16) ))
    {
      error |= ERROR_PROG_16B_WORD;
    }
    *(uint32_t *)(DATA_E2_ADDR) = DATA_32B_WORD; /* (1) */
    __WFI();
    if  (*(uint32_t *)(DATA_E2_ADDR) != DATA_32B_WORD)
    {
      error |= ERROR_PROG_32B_WORD;
    }
  }
  LockNVM();
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
  while ((RCC->CFGR & RCC_CFGR_SWS_PLL)  != RCC_CFGR_SWS_PLL) /* (9) */
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
  * Brief   This function unlocks the data EEPROM and the FLASH_PECR. 
  *         The data EEPROM will be ready to be erased or programmed
  *         but the program memory will be still locked till PRGLOCK is set.
  *         It first checks no flash operation is on going,
  *         then unlocks PELOCK if it is locked.
  * Param   None
  * Retval  None
  */
__INLINE void UnlockPELOCK(void)
{  
  /* (1) Wait till no operation is on going */
  /* (2) Check if the PELOCK is unlocked */
  /* (3) Perform unlock sequence */
  while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (1) */  
  {
    /* For robust implementation, add here time-out management */
  }  
  if ((FLASH->PECR & FLASH_PECR_PELOCK) != 0) /* (2) */
  {    
    FLASH->PEKEYR = FLASH_PEKEY1; /* (3) */
    FLASH->PEKEYR = FLASH_PEKEY2;
  }
}


/**
  * Brief   This function locks the NVM.
  *         It first checks no flash operation is on going,
  *         then locks the flash.
  * Param   None
  * Retval  None
  */
__INLINE void LockNVM(void)
{  
  /* (1) Wait till no operation is on going */
  /* (2) Locks the NVM by setting PELOCK in PECR */
  while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (1) */  
  {
    /* For robust implementation, add here time-out management */
  }  
  FLASH->PECR |= FLASH_PECR_PELOCK; /* (2) */
}

/**
  * Brief   This function erases a word of data EEPROM.
  *         The ERASE bit and DATA bit are set in PECR at the beginning 
  *         and reset at the endof the function. In case of successive erase, 
  *         these two operations could be performed outside the function.
  *         The flash interrupts must have been enabled prior to call 
  *         this function.
  * Param   addr is the 32-bt word address to erase
  * Retval  None
  */
__INLINE void EepromErase(uint32_t addr)
{   
  /* (1) Set the ERASE and DATA bits in the FLASH_PECR register 
         to enable page erasing */
  /* (2) Write a 32-bit word value at the desired address 
         to start the erase sequence */
  /* (3) Enter in wait for interrupt. The EOP check is done in the Flash ISR */
  /* (6) Reset the ERASE and DATA bits in the FLASH_PECR register 
         to disable the page erase */
  FLASH->PECR |= FLASH_PECR_ERASE | FLASH_PECR_DATA; /* (1) */    
  *(__IO uint32_t *)addr = (uint32_t)0; /* (2) */
  __WFI(); /* (3) */
  FLASH->PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_DATA); /* (4) */
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
  * Brief   This function handles FLASH interrupt request.
  *         It handles any kind of error even if not used in this example.
  * Param   None
  * Retval  None
  */
void FLASH_IRQHandler(void)
{
  if ((FLASH->SR & FLASH_SR_EOP) != 0)  /* (3) */
  {
    FLASH->SR = FLASH_SR_EOP; /* (4) */
  }
  /* Manage the error cases */
  else if ((FLASH->SR & FLASH_SR_FWWERR) != 0) /* Check Fetch while Write error */
  {
    error |= ERROR_FETCH_DURING_PROG; /* Report the error to the main progran */
    FLASH->SR = FLASH_SR_FWWERR; /* Clear the flag by software by writing it at 1*/
  }
  else if ((FLASH->SR & FLASH_SR_NOTZEROERR) != 0) /* Check Not Zero error */
  /* This error occurs if the address content was not cleared/erased 
     before the programming */
  {
    error |= ERROR_NOT_ZERO; /* Report the error to the main progran */
    FLASH->SR = FLASH_SR_NOTZEROERR; /* Clear the flag by software by writing it at 1*/
  }
  else if ((FLASH->SR & FLASH_SR_SIZERR) != 0) /* Check Size error */
  {
    error |= ERROR_SIZE; /* Report the error to the main progran */
    FLASH->SR = FLASH_SR_SIZERR; /* Clear the flag by software by writing it at 1*/
  }
  else if ((FLASH->SR & FLASH_SR_WRPERR) != 0) /* Check Write protection error */
  {
    error |= ERROR_WRITE_PROTECTION; /* Report the error to the main progran */
    FLASH->SR = FLASH_SR_WRPERR; /* Clear the flag by software by writing it at 1*/
  }
  else if ((FLASH->SR & FLASH_SR_RDERR) != 0) /* Check Read-out protection error */
  {
    error |= ERROR_READOUT_PROTECTION; /* Report the error to the main progran */
    FLASH->SR = FLASH_SR_RDERR; /* Clear the flag by software by writing it at 1*/
  }
  else if ((FLASH->SR & FLASH_SR_OPTVERR) != 0) /* Check Option valid error */
  {
    error |= ERROR_OPTION_NOT_VALID; /* Report the error to the main progran */
    FLASH->SR = FLASH_SR_OPTVERR; /* Clear the flag by software by writing it at 1*/
  }
  else if ((FLASH->SR & FLASH_SR_PGAERR) != 0) /* Check alignment error */
  {
    error |= ERROR_ALIGNMENT; /* Report the error to the main progran */
    FLASH->SR = FLASH_SR_PGAERR; /* Clear the flag by software by writing it at 1*/
  } 
  else
  {
    error |= ERROR_UNKNOWN; 
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
