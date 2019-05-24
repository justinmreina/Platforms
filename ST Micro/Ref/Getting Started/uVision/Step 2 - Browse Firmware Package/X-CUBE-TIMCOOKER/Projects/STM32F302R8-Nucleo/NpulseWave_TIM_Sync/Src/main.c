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

/* USER CODE BEGIN Includes */
#include "main.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint16_t Tim1Prescaler = 0;
uint16_t Tim2Prescaler = 0;
uint16_t Period = 0 ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
static void RCC_Configuration(void);
static void GPIO_Configuration(void);
void Nvic_Configuration(void);
void TIM1_Part1_Configuration(void);
void TIM2_Part1_Configuration(void);
void TIM1_Part2_Configuration(void);
void TIM2_Part2_Configuration(void);
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

  /* USER CODE BEGIN 2 */
	
  /* Enable GPIOA, TIM1, TIM2 peripherals*/
  RCC_Configuration();
	
  /* Configure used GPIOs */
  GPIO_Configuration();
	
#ifdef PERIODIC_NPULSES_GENERATION 

  /* Configure TIM2 peripheral */
  TIM2_Part1_Configuration();

  /* Configure TIM1 peripheral */
  TIM1_Part1_Configuration();
	
#elif defined ONESHOT_COMPLYMENTARY_NPULSES_GENERATION /* not PERIODIC_NPULSES_GENERATION*/

  /* Configure nvic to handel TIM1 Trigger and Commutation */
  Nvic_Configuration();

  /* Configure TIM2 peripheral */
  TIM2_Part2_Configuration();
	
	/* Configure TIM1 peripheral */
  TIM1_Part2_Configuration();
	
#endif /* PERIODIC_NPULSES_GENERATION */

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
  *      System Clock source            = PLL (HSE)
  *      SYSCLK(Hz)                     = 64000000
  *      HCLK(Hz)                       = 64000000
  *      AHB Prescaler                  = 1
  *      APB1 Prescaler                 = 2
  *      APB2 Prescaler                 = 1
  *      HSE Frequency(Hz)              = 8000000
  *      Flash Latency(WS)              = 2
  *      TIM1 Clock source              = HCLK
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

/* USER CODE BEGIN 4 */

/**
  * @brief  Enable used peripherals  
	*			    Enable TIM1, TIM2 and GPIOA peripherals
  * @param  None 
  * @retval None
  */
static void RCC_Configuration(void)
{
  /* TIM1 clock enable */
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
  /* TIM2 clock enable */
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
  /* GPIOA clock enable */
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
}

/**
  * @brief  Configures GPIOs 
  *         Enable only PA8 for first sub-example
  *         Enable both PA8 and PA7 for second sub-example
  *         Push-Pull mode         = activated
  *         Pull definition        = Pull Down
  *         Alternate function     = AF6
  *         Speed                  = High
  * @param  None 
  * @retval None
  */
static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  
  /* Configure TIM1_CHANNEL1 in output, push-pull & alternate function mode */
	
#ifdef PERIODIC_NPULSES_GENERATION 
  GPIO_InitStruct.Pin = GPIO_PIN_8;
#elif defined ONESHOT_COMPLYMENTARY_NPULSES_GENERATION /* not PERIODIC_NPULSES_GENERATION */
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_7;
#endif /* PERIODIC_NPULSES_GENERATION */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN; 
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF6_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  Configures NVIC register
  *         Set TIM1 Trigger and Commutation handler to High priority
  *         Enable NVIC
  * @param  None 
  * @retval None
  */
void Nvic_Configuration(void)
{	
/* Enable the TIM1 global Interrupt & set priority */
HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, 0, 0);
HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
}

/**
  * @brief  Configure the TIM1 peripheral for PERIODIC_NPULSES_GENERATION Part 
  *         Initialize TIM1 peripheral as follows:
  *    + Prescaler = (SystemCoreClock/1000000) - 1  // get 1MHz as Timer clock
  *    + Period = 1000000 / 20000 = 500
  *    + Pulse = Period /2 = 50%
  *    + Repetition counter (RCR) = PULSE_NUMBER -1 (defined in main.h file)
  *    + PWM mode = mode1
  *    + Master Output Mode = one pulse mode (OPM)
  *    + Slave input mode = trigger mode
  *    + Trigger input selection = ITR1
  *    + Output polarity level = active low
  *    + Output Compare Preload = Enabled
  * @param  None
  * @retval None
  */
void TIM1_Part1_Configuration(void)
{	
  /* set the Timer prescaler to get 1MHz as counter clock */
  Tim1Prescaler= (uint16_t) (SystemCoreClock / 1000000) - 1; 
	
  /* Initialize the PWM period to get 20 KHz as frequency from 1MHz */
  Period = 1000000 / 20000; 
	
  /* Configure the Timer prescaler */
  TIM1->PSC = Tim1Prescaler;
	
  /* Configure the period */
  TIM1->ARR = Period-1;
	
  /* Configure the repetition counter */
  TIM1->RCR = PULSES_NUMBER-1; 
		
  /*Configure pulse width */
  TIM1->CCR1 = Period / 2; 
  
  /* Select the Clock Division to 1*/
  /* Reset clock Division bit field */
  TIM1->CR1 &= ~ TIM_CR1_CKD;
  /* Select DIV1 as clock division*/
  TIM1->CR1 |= TIM_CLOCKDIVISION_DIV1;
	
  /* Select the Up-counting for  TIM1 counter */
  /* Reset mode selection bit fields*/
   TIM1->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
  /* select Up-counting mode */
  TIM1->CR1 |=  TIM_COUNTERMODE_UP;
	
  /* SET PWM1 mode */
  /* Reset the Output Compare Mode Bits */
  TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
  TIM1->CCMR1 &= ~TIM_CCMR1_CC1S;
  /* Select the output compare mode 1*/
  TIM1->CCMR1 |= TIM_OCMODE_PWM1;
	
/**************  One pulse mode configuration *************/	

  /* One Pulse Mode selection */	
  TIM1->CR1 |= TIM_CR1_OPM;
	
/**********************************************************/
/******* Slave mode configuration : Trigger mode *********/

 /* Select the TIM_TS_ITR1 signal as Input trigger for the TIM */
  TIM1->SMCR &= ~TIM_SMCR_TS;
  TIM1->SMCR |= TIM_TS_ITR1;
	
  /* Select the Slave Mode */ 
  TIM1->SMCR &= ~TIM_SMCR_SMS;
  TIM1->SMCR |= TIM_SLAVEMODE_TRIGGER;
/**********************************************************/	

  /* Enable the output compare 1 Preload */
  TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
	
  /* Set the UG bit to enable UEV */
  TIM1->EGR |= TIM_EGR_UG;
	
  /* Enable the TIM1 Main Output */
  TIM1->BDTR |= TIM_BDTR_MOE;

  /* Select active low as output polarity level */
  /* Reset the Output Polarity level */
  TIM1->CCER &= ~TIM_CCER_CC1P;
  /* Set the Low ourput */
  TIM1->CCER |= TIM_OCPOLARITY_LOW;

  /* Enable CC1 output on High level*/
  TIM1->CCER |= TIM_CCER_CC1E;
	
  /* Enable the TIM Counter */
  TIM1->CR1 |= TIM_CR1_CEN;
}

/**
  * @brief  Configure the TIM2 peripheral for PERIODIC_NPULSES_GENERATION Part 
  *    Initialize TIM1 peripheral as follows:
  *    + Prescaler = (SystemCoreClock/1000000) - 1  
  *    + Period = 1000000 / 50 = 500 us
  *    + Pulse = Periode/2 = 50%
  *    + Master Output Mode = trigger update mode
  * @param  None
  * @retval None
  */
void TIM2_Part1_Configuration(void)
{	
  /* set the Timer prescaler to get 1MHz as counter clock */
  Tim2Prescaler= (uint16_t) ((SystemCoreClock ) / 1000000) - 1; 
	
  /* Initialize the PWM period to get 50 Hz as frequency from 1MHz */
  Period = 1000000 / 50; 
	
  /* Configure the period */
  TIM2->ARR = Period-1;
	 
  /* Configure the Timer prescaler */
  TIM2->PSC = Tim2Prescaler;
  
  /* Select the Clock Divison to 1*/
  /* Reset clock Division bit field */
  TIM2->CR1 &= ~ TIM_CR1_CKD;
  /* Select DIV1 as clock division*/
  TIM2->CR1 |= TIM_CLOCKDIVISION_DIV1;
	
  /* Select the Up-counting for  TIM1 counter */
  /* Reset mode selection bit fields */
  TIM2->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
  /* select Up-counting mode */
  TIM2->CR1 |= TIM_COUNTERMODE_UP;

/****** Master mode configuration: Trigger update mode *******/		
	
  /* Trigger of TIM2 Update into TIM1 Slave */
  TIM1->CR2 &= ~ TIM_CR2_MMS;
  TIM2->CR2 |= TIM_TRGO_UPDATE;
	
/*************************************************************/	

  /* Enable the TIM Counter */
  TIM2->CR1 |= TIM_CR1_CEN;
}

/**
  * @brief  Configure the TIM1 peripheral for PERIODIC_NPULSES_GENERATION Part 
       Initialize TIM1 peripheral as follows:
       + Prescaler = (SystemCoreClock/1000000) - 1 
       + Period = 1000000 / 2000 = 5000 -> Frequency = 2 KHz
       + Pulse = Period /2 = 50%
       + PWM mode = mode1
       + Master Output Mode = Trigger Reset mode
       + Slave input mode = trigger mode 
       + trigger input selection = ITR1
       + Channel1 Output polarity level  = active High
       + Channel1N Output polarity level = active High
       + Capture Compare Preload = Enabled
       + Commitation event = Enabled
       + COM control update = TRGI
       + Update event generation UG = Enabled
COM control update
  * @param  None
  * @retval None
  */
void TIM1_Part2_Configuration(void)
{
  /* set the Timer prescaler to get 1MHz as counter clock */
  Tim1Prescaler= (uint16_t) (SystemCoreClock / 1000000) - 1; 

  /* Initialize the PWM periode to get 2 KHz as frequency 10000*/	
  Period = 1000000 / 2000; 
	
  /* Configure the Timer prescaler */
  TIM1->PSC = Tim1Prescaler;
  
  /* Configure the period */
  TIM1->ARR = Period-1;
	
  /*Configure pulse width */
  TIM1->CCR1 = Period / 2; 
  
  /* Select the Clock Divison to 1*/
  /* Reset clock Division bit field */
  TIM1->CR1 &= ~ TIM_CR1_CKD;
  /* Select DIV1 as clock division*/
  TIM1->CR1 |= TIM_CLOCKDIVISION_DIV1;
	
  /* Select the Up-counting for TIM1 counter */
  /* Reset mode selection bit fields */
  TIM1->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
  /* select Up-counting mode */
  TIM1->CR1 |= TIM_COUNTERMODE_UP;
	
  /* SET PWM1 mode */
  /* Reset the Output Compare Mode Bits */
  TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
  TIM1->CCMR1 &= ~TIM_CCMR1_CC1S;
  /* Select the output compare mode 1*/
  TIM1->CCMR1 |= TIM_OCMODE_PWM1;
	
  /* Select active High as output polarity level */
  /* Reset the Output Polarity level */
  TIM1->CCER &= ~TIM_CCER_CC1P;
  /* Set the high Output Compare Polarity */
  TIM1->CCER |= TIM_OCPOLARITY_HIGH;

  /* Enable CC1 output on High level*/
  TIM1->CCER |= TIM_CCER_CC1E;
		
 /* Select active High as output Complementary polarity level */
 /* Reset the Output N State */
  TIM1->CCER &= ~TIM_CCER_CC1NP;
 /* Set the Output N Polarity to high level */
  TIM1->CCER |= TIM_OCNPOLARITY_HIGH;
	
  /* Enable CC1 output on High level*/
  TIM1->CCER |= TIM_CCER_CC1NE;
  
/******************* COM Control update configuration *********************/

  /* Set the capture Compare Preload */
  TIM1->CR2 |= TIM_CR2_CCPC;
	
  /* Set CCUS bit to select the COM control update to trigger input TRGI*/
  TIM1->CR2 |= TIM_CR2_CCUS;

  /* Enable the Commutation Interrupt sources */
  TIM1->DIER |= TIM_IT_COM;
/******************************************************************/
  
/********** Master mode configuration: Trigger Reset mode *********/

  /* Configure TIM1's trigger output Update to trig TIM2 */
  TIM1->CR2 &= ~ TIM_CR2_MMS;
  TIM1->CR2 |= TIM_TRGO_RESET;
/******************************************************************/	
        
/************ Slave mode configuration : Trigger mode ************/
	
 /* Select the TIM_TS_ITR1 signal as Input trigger for the TIM */
  TIM1->SMCR &= ~TIM_SMCR_TS;
  TIM1->SMCR |= TIM_TS_ITR1;
	
  /* Select the Slave Mode */ 
  TIM1->SMCR &= ~TIM_SMCR_SMS;
  TIM1->SMCR |= TIM_SLAVEMODE_TRIGGER;
/******************************************************************/	

  /* Set the UG bit to enable UEV */
  TIM1->EGR |= TIM_EGR_UG;
	
  /* Enable the TIM1 Main Output */
  TIM1->BDTR |= TIM_BDTR_MOE;
	
  /* Enable the TIM Counter */
  TIM1->CR1 |= TIM_CR1_CEN;
}
 
/**
  * @brief  Configure the TIM2 peripheral for PERIODIC_NPULSES_GENERATION Part 
  *    Initialize TIM1 peripheral as follows:
  *
  *    + Prescaler = (SystemCoreClock/1000000) - 1  
  *    + Period = 1000000 / 1000 = 10000  // Frequency = 100 Hz
  *    + Master Output Mode = trigger update mode
  *    + Slave input mode = trigger Reset mode 
  *    + trigger input selection = ITR0
  * @param  None
  * @retval None
  */
void TIM2_Part2_Configuration(void)
{		
  /* set the Timer prescaler to get 1MHz as counter clock */
  Tim2Prescaler= (uint16_t) ((SystemCoreClock ) / 1000000) - 1; 
	
  /* Initialize the PWM periode to get 100 Hz as frequency from 1MHz */
  Period = 1000000 / 100; 
	
  /* Configure the period */
  TIM2->ARR = Period-1;
	 
  /* Configure the Timer prescaler */
  TIM2->PSC = Tim2Prescaler;
  
  /* Select the Clock Divison to 1*/
  /* Reset clock Division bit field */
  TIM2->CR1 &= ~ TIM_CR1_CKD;
  /* Select DIV1 as clock division*/
  TIM2->CR1 |= TIM_CLOCKDIVISION_DIV1;
	
  /* Select the Up-counting for TIM1 counter */
  /* Reset mode selection bit fields*/
  TIM2->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
  /* select Up-counting mode */
  TIM2->CR1 |= TIM_COUNTERMODE_UP;

/********** Master mode configuration: trigger update ***********/	
	
  /* Trigger of TIM2 Update into TIM1 Slave */
  TIM2->CR2 &= ~TIM_CR2_MMS;
  TIM2->CR2 |= TIM_TRGO_UPDATE;
	
/****************************************************************/	
/********** Slave mode configuration: Trigger mode **************/
	
 /* Select the TIM_TS_ITR0 signal as Input trigger for the TIM */
  TIM2->SMCR &= ~TIM_SMCR_TS;
  TIM2->SMCR |= TIM_TS_ITR0;
	
  /* Slave Mode selection: Trigger reset Mode */
  TIM2->SMCR &= ~TIM_SMCR_SMS;
  TIM2->SMCR |= TIM_SLAVEMODE_RESET;
/****************************************************************/	
	
  /* Enable the TIM1 Counter */
  TIM2->CR1 |= TIM_CR1_CEN;
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
