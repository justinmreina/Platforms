/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/


/* USER CODE BEGIN PV */
#define PERIOD 4999
#define PULSE_NUMBER 10
#define PULSE 2000
/* Private variables ---------------------------------------------------------*/
uint32_t Prescaler = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);


/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void TIM1_Config(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
	
  /* USER CODE BEGIN 2 */
	
  /************************* Configure the TIM1 **************************/
  TIM1_Config();	

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}




/**
  * @brief  Configure the TIM1 peripheral:
  *   Counter mode : UP Counter 
  *   Clock Division : 1
  *   Period : 5000
  *   Prescaler : 7
  *   Repetition counter : PULSE_NUMBER - 1
  *   One Pulse Mode : Enable
  *   output channel : Channel 1
	*   PWM Mode : mode 1
	*   Polarity : High 
  * @param  None
  * @retval None
  */

static void TIM1_Config(void)
{
	
  /* Peripheral clock enable */
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
 /* Configure the Time base :
       + Counter mode : UP Counter
       + Clock Division : 1
       + Period : 5000
       + Pulse : 2000  
       + Prescaler : 7
       + Repetition counter : PULSE_NUMBER - 1
 */

 /* Set the Timer prescaler to get 1MHz as counter clock */
 Prescaler = (uint16_t) (SystemCoreClock / 1000000) - 1;
 
 /* Select the up counter mode */
 TIM1->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
 TIM1->CR1 |= TIM_COUNTERMODE_UP; 
  
 /* Set the clock division to 1*/
 TIM1->CR1 &= ~TIM_CR1_CKD;         
 TIM1->CR1 |= TIM_CLOCKDIVISION_DIV1;
	
 /* Set the Autoreload value */
 TIM1->ARR = PERIOD;       

 /* Set the Pulse value */
 TIM1->CCR1 = PULSE; 

 /* Set the Prescaler value */
 TIM1->PSC = Prescaler;

 /* Set the Repetition counter value */
 TIM1->RCR = PULSE_NUMBER - 1;   

/* Generate an update event to reload the Prescaler and the repetition counter 
  value immediately */	
 TIM1->EGR = TIM_EGR_UG;            
	
/******** Configure the Internal Clock source *********************************/
 /* Disable slave mode to clock the prescaler directly with the internal clock
 if the TIM_SMCR is in the reset value, we can delete the following instruction*/
 TIM1->SMCR =  RESET; 
	
/****************** Configure the One Pulse Mode ******************************/

 /* Select the OPM Mode */	
 TIM1->CR1 |= TIM_CR1_OPM;  
	
 /* Configure the PWM mode:
       + Channel : Channel 1
       + PWM Mode : mode 2
       + Polarity : High
 */
 /* Select the Channel 1 Output Compare and the Mode */ 
 TIM1->CCMR1 &= (uint16_t)~TIM_CCMR1_OC1M;    
 TIM1->CCMR1 &= (uint16_t)~TIM_CCMR1_CC1S;
 TIM1->CCMR1 |=  TIM_OCMODE_PWM2;  
	
 /* Set the Output Compare Polarity to High */
 TIM1->CCER &= (uint16_t)~TIM_CCER_CC1P;     
 TIM1->CCER |= TIM_OCPOLARITY_HIGH; 
           
 /* Enable the Compare output channel 1*/
 TIM1->CCER = TIM_CCER_CC1E;
 	
 /*Enable the TIM main Output*/
 TIM1->BDTR |= TIM_BDTR_MOE; 

 /*Enable the TIM peripheral*/
 TIM1->CR1 |= TIM_CR1_CEN; 
	
}




/** Configure pins
     PC0   ------> S_TIM1_CH1
*/
void MX_GPIO_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStruct;

 /* GPIO Ports Clock Enable */
 __GPIOC_CLK_ENABLE();
	
 /*Configure GPIO pin : PC0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
