/**
  ******************************************************************************
  * File     01_NVMEraseAndProg/main.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This code example shows how to erase and program the Flash memory
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
    - This example erases the target page and program the first 32-bits word
      with a defined value.
      The programming is performed in two operations first the 16 less 
      significant bits (LSbits) on the first address then the 16 MSbits 
      on the next address.
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
#include "ram_functions.h"

/**  STM32L0_Snippets
  * 
  */



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Tick;
volatile uint16_t error;
uint32_t test_to_be_performed_twice = 1; //this variable is set to 2 if the first address of the page to erase is yet erased
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void  ConfigureGPIO(void);
void  UnlockPELOCK(void);
void  UnlockPRGLOCK(void);
void  LockNVM(void);
void  FlashErase(uint32_t page_addr);
void CheckFlashErase(uint32_t first_page_addr);
void FlashWord32Prog(uint32_t flash_addr, uint32_t data);
void CheckFlashHalfPageProg(uint32_t flash_addr, uint32_t *data);
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
  error = 0;
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
  UnlockPRGLOCK();  
  /* Check if the first address of the page is yet erased,this is only for this example */
  if (*(uint32_t *)(FLASH_USER_START_ADDR) == (uint32_t)0) 
  {
    test_to_be_performed_twice = 2;
  }
  
  while (test_to_be_performed_twice-- > 0)
  {
    FlashErase(FLASH_USER_START_ADDR);
    CheckFlashErase(FLASH_USER_START_ADDR);
    FlashWord32Prog(FLASH_USER_LAST_ADDR , (uint32_t)DATA_TO_PROG);
    
    /* Check the programming of the address */
    if ((*(uint32_t *)(FLASH_USER_LAST_ADDR)) != DATA_TO_PROG)
    {
      error |= ERROR_PROG;
    }
    FlashHalfPageProg(FLASH_USER_START_ADDR, data_for_hpage);
    CheckFlashHalfPageProg(FLASH_USER_START_ADDR, data_for_hpage);
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
  * Brief   This function prepares the flash to be erased or programmed.
  *         It first checks no flash operation is on going,
  *         checks also PELOCK is reset
  *         then unlocks the flash if it is locked.
  * Param   None
  * Retval  None
  */
__INLINE void UnlockPRGLOCK(void)
{  
  /* (1) Wait till no operation is on going */
  /* (2) Check that the PELOCK is unlocked */
  /* (3) Check if the PRGLOCK is unlocked */  
  /* (4) Perform unlock sequence */
  while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (1) */  
  {
    /* For robust implementation, add here time-out management */
  }  
  if ((FLASH->PECR & FLASH_PECR_PELOCK) == 0) /* (2) */
  {    
    if ((FLASH->PECR & FLASH_PECR_PRGLOCK) != 0) /* (3) */
    {    
      FLASH->PRGKEYR = FLASH_PRGKEY1; /* (4) */
      FLASH->PRGKEYR = FLASH_PRGKEY2;
    }
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
  * Brief   This function erases a page of flash.
  *         The Page Erase bit (PER) is set at the beginning and reset at the end
  *         of the function, in case of successive erase, these two operations
  *         could be performed outside the function.
  * Param   page_addr is an address inside the page to erase
  * Retval  None
  */
__INLINE void FlashErase(uint32_t page_addr)
{   
  /* (1) Set the ERASE and PROG bits in the FLASH_PECR register 
         to enable page erasing */
  /* (2) Write a 32-bit word value in an address of the selected page 
         to start the erase sequence */
  /* (3) Wait until the BSY bit is reset in the FLASH_SR register */
  /* (4) Check the EOP flag in the FLASH_SR register */
  /* (5) Clear EOP flag by software by writing EOP at 1 */
  /* (6) Reset the ERASE and PROG bits in the FLASH_PECR register 
         to disable the page erase */
  FLASH->PECR |= FLASH_PECR_ERASE | FLASH_PECR_PROG; /* (1) */    
  *(__IO uint32_t *)page_addr = (uint32_t)0; /* (2) */    
  while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (3) */ 
  {
    /* For robust implementation, add here time-out management */
  }  
  if ((FLASH->SR & FLASH_SR_EOP) != 0)  /* (4) */
  {  
    FLASH->SR = FLASH_SR_EOP; /* (5)*/
  }    
  /* Manage the error cases */
  else if ((FLASH->SR & FLASH_SR_FWWERR) != 0) /* Check Fetch while Write error */
  {
    error |= ERROR_FETCH_DURING_ERASE; /* Report the error to the main progran */
    FLASH->SR = FLASH_SR_FWWERR; /* Clear the flag by software by writing it at 1*/
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
  else
  {
    error |= ERROR_UNKNOWN; /* Report the error to the main progran */
  }
  FLASH->PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_PROG); /* (6) */
}


/**
  * Brief   This function checks that the whole page has been correctly erased.
  *         A word is erased while all its bits are reset.
  * Param   first_page_addr is the first address of the page to erase
  * Retval  None
  */
__INLINE void CheckFlashErase(uint32_t first_page_addr)
{
uint32_t i;  

  for (i=FLASH_PAGE_SIZE; i > 0;i-=4) /* Check the erasing of the page by reading all the page value */
  {
    if (*(__IO uint32_t *)(first_page_addr + i -4) != (uint32_t)0) /* compare with erased value, all bits at 0 */
    {
      error |= ERROR_ERASE; /* report the error to the main progran */
    }
  }
}


/**
  * Brief   This function programs a 32-bit word.
  *         No need to set the Programming bit (PROG).
  *         This function waits the end of programming, clears the appropriate  
  *         bit in the Status register and eventually reports an error. 
  * Param   flash_addr is the address to be programmed
  *         data is the 32-bit word to program
  * Retval  None
  */
__INLINE void FlashWord32Prog(uint32_t flash_addr, uint32_t data)
{    
  /* (1) Perform the data write (32-bit word) at the desired address */
  /* (2) Wait until the BSY bit is reset in the FLASH_SR register */
  /* (3) Check the EOP flag in the FLASH_SR register */
  /* (4) clear it by software by writing it at 1 */
  *(__IO uint32_t*)(flash_addr) = data; /* (1) */
  while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (2) */
  {
    /* For robust implementation, add here time-out management */
  }  
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
  else
  {
    error |= ERROR_UNKNOWN; 
  }
}


/**
  * Brief   This function checks that the half page has been correctly programmed
  * Param   flash_addr is the first address of the half-page to be verified
  *         data is the 32-bit word array expected content
  * Retval  None
  */
__INLINE void CheckFlashHalfPageProg(uint32_t flash_addr, uint32_t *data)
{
uint32_t i;  

  for (i = 0; i < (FLASH_PAGE_SIZE/2); i+=4, data++)
  {
    if (*(uint32_t *)(flash_addr + i) != *data)
    {
      error |= ERROR_HALF_PROG; /* report the error to the main progran */
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
