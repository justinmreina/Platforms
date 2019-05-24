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
COMP_HandleTypeDef hcomp4;
DAC_HandleTypeDef hdac;

/* USER CODE BEGIN PV */
#define PULSE 600
#define PERIOD 999
#define VREF 2047 /* Will be converted by the DAC to get VREF = 1.65V */
#define DEAD_TIME 0 
/* Private variables ---------------------------------------------------------*/
uint32_t Prescaler = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_COMP4_Init(void);
static void MX_DAC_Init(void);


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
  MX_COMP4_Init();
  MX_DAC_Init();

  /* USER CODE BEGIN 2 */
	
  /****************** Configure the TIM1 ********************************/
  TIM1_Config();
	
  /****************** Set the Iref value ********************************/
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, VREF);
	
  /****************** Convert the Iref value ****************************/
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	
  /****************** Start the comparison ******************************/
  HAL_COMP_Start(&hcomp4);
	
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

/**
  * @brief  Configure the TIM1 peripheral:
  *   Counter mode : UP Counter 
  *   Clock Division : 1
  *   Period : 999
  *   Prescaler : 15
  *   Dead Time : 0
  *   Lock Level : OFF
  *   Output idle mode : Enable
  *   Output Run mode : Disable
  *   Breack input : Enable
  *   Polarity : High
  *   Automatic output: Enable
  *   Output Channel : Channel 1
	*   PWM Mode : mode 1
	*   Polarity : High 
  * @param  None
  * @retval None
  */

static void TIM1_Config(void)
{
	
  /* TIM1 clock enable */
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;	
	
 /* Configure the Time base :
       + Counter mode : UP Counter
       + Clock Division : 1
       + Period : 999
       + Prescaler : 15
 */
  
 /* Set the Timer prescaler to get 500 kHz as counter clock */
 Prescaler = (uint16_t) (SystemCoreClock / 500000) - 1;
 
 /* Select the up counter mode*/
 TIM1->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS); 
 TIM1->CR1 |= TIM_COUNTERMODE_UP;  
			
 /* Set the clock division to 1*/
 TIM1->CR1 &= ~TIM_CR1_CKD;
 TIM1->CR1 |= TIM_CLOCKDIVISION_DIV1;
			
 /* Set the Autoreload value */
 TIM1->ARR = PERIOD;
 
 /* Set the Capture Compare Register value */
 TIM1->CCR1 = PULSE;
		 
 /* Set the Prescaler value */
 TIM1->PSC = Prescaler;
			
 /* Generate an update event to reload the Prescaler and the repetition
 counter value immediatly */
 TIM1->EGR = TIM_EGR_UG;
			
				
 /******Configure the Internal Clock source **********************************/
 /* Disable slave mode to clock the prescaler directly with the internal clock
 if the TIM_SMCR is in the reset value, we can delete the following instruction*/	
 TIM1->SMCR = RESET;       
	
 /* Configure Break function :
       + Dead Time : 0
       + Lock Level : OFF
       + Output idle mode : Enable
       + Output Run mode : Disable
       + Breack input : Enable
       + Polarity : High
       + Automatic output: Enable
 */			
 /* Clear the BDTR bits */
  TIM1->BDTR = RESET;

 /* Set the Dead Time value to 0 */
 TIM1->BDTR |= DEAD_TIME;
	
 /* Disable the Lock Level*/
 TIM1->BDTR |= TIM_LOCKLEVEL_OFF; 

 /* Enable the Output idle mode */
 TIM1->BDTR |= TIM_OSSI_ENABLE; 
			
 /* Disable the Output run mode */			
 TIM1->BDTR |= TIM_OSSR_DISABLE;  
			
 /* Enable the Break input */		
 TIM1->BDTR |= TIM_BREAK_ENABLE; 
			
 /* Set the polarity to High*/
 TIM1->BDTR |= TIM_BREAKPOLARITY_HIGH;
			
 /* Enable the automatic output */
 TIM1->BDTR |= TIM_AUTOMATICOUTPUT_ENABLE; 
       
 /* Configure the PWM mode :
       + Channel : Channel 1
       + PWM Mode : mode 1
       + Polarity : High
 */

 /* Select the Channel 1 Output Compare and the Mode */
 TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
 TIM1->CCMR1 &= ~TIM_CCMR1_CC1S;
 TIM1->CCMR1 |= TIM_OCMODE_PWM1;
			
 /* Set the Output Compare Polarity to High*/
 TIM1->CCER &= ~TIM_CCER_CC1P;
 TIM1->CCER |= TIM_OCPOLARITY_HIGH;
  
 /* Enable the Compare output channel 1*/
 TIM1->CCER |= TIM_CCER_CC1E;
     
 /*Enable the TIM peripheral.*/      
 TIM1->CR1|=TIM_CR1_CEN; 
	
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

/* COMP4 init function */
void MX_COMP4_Init(void)
{

  hcomp4.Instance = COMP4;
  hcomp4.Init.InvertingInput = COMP_INVERTINGINPUT_DAC1_CH1;
  hcomp4.Init.NonInvertingInput = COMP_NONINVERTINGINPUT_IO1;
  hcomp4.Init.Output = COMP_OUTPUT_TIM1BKIN;
  hcomp4.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
  hcomp4.Init.BlankingSrce = COMP_BLANKINGSRCE_NONE;
  hcomp4.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
  HAL_COMP_Init(&hcomp4);

}

/* DAC init function */
void MX_DAC_Init(void)
{

  DAC_ChannelConfTypeDef sConfig;

    /**DAC Initialization 
    */
  hdac.Instance = DAC;
  HAL_DAC_Init(&hdac);

    /**DAC channel OUT1 config 
    */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);

}



/** Configure pins
     PC0   ------> S_TIM1_CH1
*/
void MX_GPIO_Init(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOC_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();

	/*Configure GPIO pin : PC0 */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
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
