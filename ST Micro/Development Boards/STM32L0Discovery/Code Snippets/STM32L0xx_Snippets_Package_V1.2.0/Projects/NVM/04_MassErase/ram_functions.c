/**
  ******************************************************************************
  * File     01_FlashEraseAndProg/ram_functions.c 
  * Author   MCD Application Team
  * Version  V1.2.0
  * Date     05-February-2016
  * Brief    This file contains the code to be executed from RAM
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
/* Private function prototypes -----------------------------------------------*/
__RAM_FUNC void OptionByteErase(uint8_t index);
__RAM_FUNC void OptionByteProg(uint8_t index, uint16_t data);
__RAM_FUNC void LockNVM(void);
/* Private functions ---------------------------------------------------------*/
 
/**
  * Brief   This function performs a mass erase of the flash.
  *         This function is loaded in RAM.
  * Param   None
  * Retval  While successful, the function never returns except if executed from RAM
  */
__RAM_FUNC void FlashMassErase(void)
{   
  /* (1) Check if the read out protection is level 2 */
  /* (2) Check if the read out protection is level 0 */
  /* (3) Set read out protection to level 1 */
  /* (4) Reload the Option bytes */
  /* (5) Program read out protection to level 0 by writing 0xAA 
         to start the mass erase */
  /* (6) Lock the NVM by setting the PELOCK bit */
  if ((FLASH->OPTR & 0x000000FF) == 0xCC) /* (1) */
  {
    error |=  ERROR_RDPROT_L2;
    return;
  }
  else if ((FLASH->OPTR & 0x000000FF) == 0xAA) /* (2) */
  {
    OptionByteProg(FLASH_OPTR0, 0x00); /* (3) */
    FLASH->PECR |= FLASH_PECR_OBL_LAUNCH; /* (4) */
    /* The MCU will reset while executing the option bytes reloading */
  }
  OptionByteProg(FLASH_OPTR0, 0xAA); /* (5) */
  if (*(uint32_t *)(FLASH_MAIN_ADDR ) != (uint32_t)0) /* Check the erasing of the page by reading all the page value */
  {
    error |= ERROR_ERASE; /* Report the error */
  }
  LockNVM(); /* (6) */
  if (error != 0)
  {  
    GPIOB->BRR |= (1<<4); /* Light off green led on PB4   */
    GPIOA->BSRR |= (1 << 5); /* Light on red led on PA5   */
  }
  while (1) /* Infinite loop */
  {
  } 
}


/**
  * Brief   This function erases a 16-bit option byte and its complement word.
  * Param   None
  * Retval  None
  */
__INLINE __RAM_FUNC void OptionByteErase(uint8_t index)
{
  /* (1) Set the ERASE bit in the FLASH_PECR register 
         to enable option byte erasing */
  /* (2) Write a 32-bit word value at the option byte address to be erased 
         to start the erase sequence */
  /* (3) Wait until the BSY bit is reset in the FLASH_SR register */
  /* (4) Check the EOP flag in the FLASH_SR register */
  /* (5) Clear EOP flag by software by writing EOP at 1 */
  /* (6) Reset the ERASE and PROG bits in the FLASH_PECR register 
         to disable the page erase */
  FLASH->PECR |= FLASH_PECR_ERASE; /* (1) */
  *(__IO uint32_t *)(OB_BASE + index) = 0; /* (2) */
  while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (3) */  
  {
    /* For robust implementation, add here time-out management */
  }
  if ((FLASH->SR & FLASH_SR_EOP) != 0)  /* (4) */
  {  
    FLASH->SR = FLASH_SR_EOP; /* (5)*/
  }    
  /* Manage the error cases */
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
  FLASH->PECR &= ~(FLASH_PECR_ERASE); /* (6) */
}


/**
  * Brief   This function programs a 16-bit option byte and its complement word.
  * Param   None
  * Retval  None
  */
__INLINE __RAM_FUNC void OptionByteProg(uint8_t index, uint16_t data)
{
  /* (1) Write a 32-bit word value at the option byte address,   
         the 16-bit data is extended with its compemented value */
  /* (3) Wait until the BSY bit is reset in the FLASH_SR register */
  /* (4) Check the EOP flag in the FLASH_SR register */
  /* (5) Clear EOP flag by software by writing EOP at 1 */
  *(__IO uint32_t *)(OB_BASE + index) = (uint32_t)((~data << 16) | data); /* (1) */
  while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (2) */  
  {
    /* For robust implementation, add here time-out management */
  }
  if ((FLASH->SR & FLASH_SR_EOP) != 0)  /* (3) */
  {  
    FLASH->SR = FLASH_SR_EOP; /* (4)*/
  }    
  /* Manage the error cases */
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
}


/**
  * Brief   This function locks the NVM.
  *         It first checks no flash operation is on going,
  *         then locks the flash.
  * Param   None
  * Retval  None
  */
__INLINE __RAM_FUNC void LockNVM(void)
{  
  /* (1) Wait till no operation is on going */
  /* (2) Locks the NVM by setting PELOCK in PECR */
  while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (1) */  
  {
    /* For robust implementation, add here time-out management */
  }  
  FLASH->PECR |= FLASH_PECR_PELOCK; /* (2) */
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
