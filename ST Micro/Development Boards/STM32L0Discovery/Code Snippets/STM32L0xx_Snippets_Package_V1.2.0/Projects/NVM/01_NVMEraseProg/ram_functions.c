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
uint32_t data_for_hpage[FLASH_PAGE_SIZE/2] = { 0x11223344, 0x55667788, 0x99aabbcc, 0xddeeff00,
                                               0x12131415, 0x16171819, 0x1a1b1c1d, 0x1e1f1011,
                                               0x23242526, 0x2728292a, 0x2b2c2d2e, 0x2f202122,
                                               0x34353637, 0x38393a3b, 0x3c3d3e3f, 0x30313233};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * Brief   This function programs a half page. It is executed from the RAM.
  *         The Programming bit (PROG) and half-page programming bit (FPRG)
  *         is set at the beginning and reset at the end of the function,
  *         in case of successive programming, these two operations
  *         could be performed outside the function.
  *         This function waits the end of programming, clears the appropriate  
  *         bit in the Status register and eventually reports an error. 
  * Param   flash_addr is the first address of the half-page to be programmed
  *         data is the 32-bit word array to program
  * Retval  None
  */

__RAM_FUNC void FlashHalfPageProg(uint32_t flash_addr, uint32_t *data)
{    
  uint8_t i;
  /* (1) Set the PROG and FPRG bits in the FLASH_PECR register 
         to enable a half page programming */
  /* (2) Perform the data write (half-word) at the desired address */
  /* (3) Wait until the BSY bit is reset in the FLASH_SR register */
  /* (4) Check the EOP flag in the FLASH_SR register */
  /* (5) clear it by software by writing it at 1 */
  /* (6) Reset the PROG and FPRG bits to disable programming */
  FLASH->PECR |= FLASH_PECR_PROG | FLASH_PECR_FPRG; /* (1) */
  for (i = 0; i < ((FLASH_PAGE_SIZE/2) * 4); i+=4, data++)
  {
    *(__IO uint32_t*)(flash_addr + i) = *data; /* (2) */
  }
  while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (3) */
  {
    /* For robust implementation, add here time-out management */
  }  
  if ((FLASH->SR & FLASH_SR_EOP) != 0)  /* (4) */
  {
    FLASH->SR = FLASH_SR_EOP; /* (5) */
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
  else if ((FLASH->SR & FLASH_SR_PGAERR) != 0) /* Check half-page alignment error */
  {      
    error |= ERROR_ALIGNMENT; 
    FLASH->SR = FLASH_SR_PGAERR; /* Clear it by software by writing it at 1*/
  }
  else
  {
    error |= ERROR_UNKNOWN; 
  }
  FLASH->PECR &= ~(FLASH_PECR_PROG | FLASH_PECR_FPRG); /* (6) */
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
