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
#define TIM1_DMAR_ADDRESS ((uint32_t)0x40012C4C) /* TIM DMAR address for burst access*/
#define BUFFER_DATA_NUMBER ((uint32_t)9)
/* Private variables ---------------------------------------------------------*/

uint32_t aSRC_Buffer[BUFFER_DATA_NUMBER] = {4000-1,1,800,10000-1,0,8500,4000-1,2,2000};
uint16_t Tim1Prescaler = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void RCC_Configuration(void);
static void TIM1_Configuration(void);
static void DMABurst_TIM1RegUpdate_Configuration(void);

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
	
  /* Enable GPIOA, TIM1, TIM2 peripherals*/
  RCC_Configuration();
	
  /* Configure DMA-Burst to update TIM1 registers with memory data*/
  DMABurst_TIM1RegUpdate_Configuration();  
	
  /* Configure TIM1 peripheral */
  TIM1_Configuration();

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
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *     System Clock source            = PLL (HSE)
  *     SYSCLK(Hz)                     = 64000000
  *     HCLK(Hz)                       = 64000000
  *     AHB Prescaler                  = 1
  *     APB1 Prescaler                 = 2
  *     APB2 Prescaler                 = 1
  *     HSE Frequency(Hz)              = 8000000
  *     Flash Latency(WS)              = 2
  *     TIM1 Clock source              = HCLK
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

/** Configure pins
     PA8   ------> S_TIM1_CH1
*/
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF6_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**
  * @brief  Enable used peripherals  
	*			    Enable TIM1 and DMA1  peripherals
  * @param  None 
  * @retval None
  */
static void RCC_Configuration(void)
{
  /* TIM1 clock enable */
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
  /* DMA1 clock enable */
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}

/**
  * @brief  Configure the TIM1 peripheral 
  *         Initialize TIM1 peripheral as follows:
  *     + Prescaler = (SystemCoreClock / 32000000) - 1;  
  *     + Period                     = 0xFFFF;
  *     + Pulse                      = 0xFFF;
  *     + PWM mode                   = mode1
  *     + DMA base address           = TIM1_ARR
  *     + DMA burst transfer length  = 3
  *     + Update generation UG       = Enabled
  *     + DMA request update         = Enabled
  *     + Auto-reload Preload        = Enabled
  *     + Output compare Preload     = Enabled
  * @param  None
  * @retval None
  */
static void TIM1_Configuration(void)
{
  /* set the Timer prescaler */
  Tim1Prescaler= (uint16_t) (SystemCoreClock / 32000000) - 1;   
	
  /* Configure the period */
  TIM1->ARR = 0xFFFF;
	
  /* Configure the Timer prescaler */
  TIM1->PSC = Tim1Prescaler;
	
  /* Configure pulse width */
  TIM1->CCR1 = 0xFFF;	
  
  /* Select the Clock Divison to 1*/
  /* Reset clock Division bit field */
  TIM1->CR1 &= ~ TIM_CR1_CKD;
  /* Select DIV1 as clock division*/
  TIM1->CR1 |= TIM_CLOCKDIVISION_DIV1;
	
  /* Select the Up-counting for  TIM1 counter */
  /* Reset mode selection bit fiels*/
  TIM1->CR1 &= ~( TIM_CR1_DIR | TIM_CR1_CMS);
  /* selct Up-counting mode */
  TIM1->CR1 |=  TIM_COUNTERMODE_UP;
	
  /* SET PWM1 mode */
  /* Reset the Output Compare Mode Bits */
  TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
  TIM1->CCMR1 &= ~TIM_CCMR1_CC1S;
  /* Select the output compare mode 1*/
  TIM1->CCMR1 |= TIM_OCMODE_PWM1;
	
  /* Enable the output compare 1 Preload */
  TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
	
  /* Enable auto-reload Preload  */
  TIM1->CR1 |= TIM_CR1_ARPE;
	
  /* TIM1 DMA Update enable */
  TIM1->DIER |= TIM_DMA_UPDATE; 
	
  /* Configure of the DMA Base register and the DMA Burst Length */
  /* Reset DBA and DBL bit fields */
  TIM1->DCR &= ~TIM_DCR_DBA;
  TIM1->DCR &= ~TIM_DCR_DBL;	
  /* Select the DMA base register and DMA burst length */
  TIM1->DCR = TIM_DMABase_ARR | TIM_DMABurstLength_3Transfers;	
	
  /* Enable UEV by setting UG bit to Load buffer data into preload registers */
  TIM1->EGR |= TIM_EGR_UG;
	
  /* Wait until the RESET of UG bit*/
  while((TIM1->EGR & TIM_EGR_UG) == SET){}
		
 /* Enable UEV by setting UG bit to load data from preload to active registers */
  TIM1->EGR |= TIM_EGR_UG;
	
  /* Enable the TIM1 Main Output */
  TIM1->BDTR |= TIM_BDTR_MOE;

  /* Enable CC1 output*/
  TIM1->CCER |= TIM_CCER_CC1E;
	
  /* Enable the TIM Counter */
  TIM1->CR1 |= TIM_CR1_CEN;
}

/**
  * @brief  Configure the DMA1 peripheral in burst mode
  *         Initialize DMA1 channel 5
  *     Data Transfer direction  = Memory to peripheral  
  *     Peripheral Increment     = Disable 
  *     Memory Increment         = Enable  
  *     Peripheral Data Size     = Word 
  *     Memory Data Size         = Word 
  *     Data Transfer mode       = circular mode 
  *     Priority level           = very high priority 
  *     Peripheral address       = TIM DMAR for burst access
  *     Peripheral address       = aSRC_Buffer
  *     Number of data register  = aSRC_Buffer data number
  * @param  None
  * @retval None
  */
static void DMABurst_TIM1RegUpdate_Configuration(void)
{
/* Configure DMA1 Channel5 CR register */
  /* Reset DMA1 Channel5 control register */
  DMA1_Channel5->CCR  = 0;
  /* Set CHSEL bits according to DMA Channel 5 */
  /* Set DIR bits according to Memory to peripheral direction */
  /* Set PINC bit according to DMA Peripheral Increment Disable */
  /* Set MINC bit according to DMA Memory Increment Enable */
  /* Set PSIZE bits according to Peripheral DataSize = Word */
  /* Set MSIZE bits according to Memory DataSize Word */
  /* Set CIRC bit according to circular mode */
  /* Set PL bits according to very high priority */
  /* Set MBURST bits according to single memory burst */
  /* Set PBURST bits according to single peripheral burst */
  DMA1_Channel5->CCR |= DMA_MEMORY_TO_PERIPH |
                        DMA_PINC_DISABLE| DMA_MINC_ENABLE|
                        DMA_PDATAALIGN_WORD| DMA_MDATAALIGN_WORD |
                        DMA_CIRCULAR | DMA_PRIORITY_HIGH; 

  /* Write to DMA1 Channel5 number of data register register */
  DMA1_Channel5->CNDTR = BUFFER_DATA_NUMBER;

  /* Write to DMA1 Channel5 peripheral address */
  DMA1_Channel5->CPAR = (uint32_t)TIM1_DMAR_ADDRESS;

  /* Write to DMA1 Channel5 Memory address register */
  DMA1_Channel5->CMAR = (uint32_t)aSRC_Buffer;
	
	/* Enable DMA1 Channel5 */
  DMA1_Channel5->CCR |= (uint32_t)DMA_CCR_EN;
}
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
