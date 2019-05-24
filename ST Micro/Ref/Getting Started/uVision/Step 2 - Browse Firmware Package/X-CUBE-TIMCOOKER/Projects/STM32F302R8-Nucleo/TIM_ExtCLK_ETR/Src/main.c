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
/* Comment the macro to select the internal source clock */
//#define USE_ETR
#define CAPTURE_BUFFER_SIZE ((uint32_t)1001) /* The Buffer size */
#define PPM_BUFFER_SIZE ((CAPTURE_BUFFER_SIZE) - 1)
#define PERIOD 0xFFFFFFFF 
#define MEASURED_SIGNAL 1000 /* Measured signal frequency in Hz*/

#ifdef USE_ETR 
#define ETRP_CLK 2500000 /* external source Clock frequency after the divider 
                            in Hz*/
#define TREF ((ETRP_CLK) / (MEASURED_SIGNAL)) /* the ideal number of count between
                                                 two input capture */
#else /* Internal clock source*/
#define INTERN_CLK 8000000 /* internal source Clock frequency in Hz*/
#define TREF ((INTERN_CLK) / (MEASURED_SIGNAL)) /* ideal number of count between 
                                                   two input capture */
#endif /* USE_ETR */
#define CONSTANT ((1000000) / (TREF)) /* constant used to calculate the PPM*/

/* Private variables ---------------------------------------------------------*/
uint32_t Prescaler = 0;
uint32_t aCaptureBuffer[CAPTURE_BUFFER_SIZE];
uint32_t aPPMBuffer[PPM_BUFFER_SIZE];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void GPIO_Config(void);
static void TIM2_Config(void);
static void DMA1_Config(void);
static void TIM2_DMA1_Capture(void);
static void PPM_Calculate(void);

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
 MX_DMA_Init();

/* USER CODE BEGIN 2 */
 /****************** Configure the TIM2 ***************************************/
 GPIO_Config();
 DMA1_Config();
	 
 /****************** Configure the TIM2 ***************************************/
 TIM2_Config();
	
 /*********** Start the TIM2 Input Capture measurement on in DMA mode **********/
 TIM2_DMA1_Capture();
 
 /*********** Calculate the PPM of the mesured signal **********/
 PPM_Calculate();

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

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


/**
  * @brief  Configure the TIM2 peripheral:
  * + ETR MODE : - HSI : 8MHz 
  *              - Divider : 4
  *              - Polarity : High
  *              - ETR pin : Channel 1
  * + Internal source clock mode: - HSI : 8MHz
  *
  *   Counter mode : UP Counter 
  *   Clock Division : 1
  *   Period : 0xFFFFFFFF
  *   Prescaler : 0 
  *   input Channel : Channel 2
  * @param  None
  * @retval None
  */

static void TIM2_Config(void)
{
 /* TIM2 clock enable */
 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 
 
 /* Set the Timer prescaler to get 8MHz as counter clock */
 Prescaler = (uint16_t) (SystemCoreClock / 8000000) - 1;
	
 /* Reset the SMCR register */
 TIM2->SMCR = RESET; 
   
#ifdef USE_ETR 	

 /* Configure the ETR Clock source:
       + Asychronous divider : 4
       + Polarity : Rising Edge
 */	
  TIM2->SMCR |= (TIM_ETRPRESCALER_DIV4 | TIM_ETRPOLARITY_NONINVERTED);                   
  TIM2->SMCR |= TIM_SMCR_ECE;                       
	
 /******** Configure the Internal Clock source ********************************/
#else	/* Internal clock source*/
  
 /* Disable slave mode to clock the prescaler directly with the internal clock 
  if the TIM_SMCR is in the reset value, we can delete the following instruction*/
 TIM2->SMCR &= ~TIM_SMCR_SMS;  
	
#endif 	/* USE_ETR*/

 /* Configure the Time base:
       + Counter mode : UP Counter
       + Clock Division : 1
       + Period : 0xFFFFFFFF
       + Prescaler : 0
 */	
 
 /* Select the up counter mode */
 TIM2->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
 TIM2->CR1 |= TIM_COUNTERMODE_UP;          
	
 /* Set the clock division to 1 */
 TIM2->CR1 &= ~TIM_CR1_CKD;
 TIM2->CR1 |= TIM_CLOCKDIVISION_DIV1;      
	
 /* Set the Autoreload value */
 TIM2->ARR = PERIOD ;
	
 /* Set the Prescaler value */
 TIM2->PSC = Prescaler;      
	
 /* Generate an update event to reload the Prescaler value immediatly */
 TIM2->EGR = TIM_EGR_UG;
	
 /* Configure the Timer Input Capture Channels:
       + Channel : Channel 2
 */	
  /* Connect the Timer input to IC2 */
  TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
  TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;
	
}

/**
  * @brief  Configure the DMA1 peripheral:
  * Data Transfer Direction : Peripheral to Memory
  * Peripheral Increment : Disable
  * Memory Increment : Enable
  * Peripheral Data Size : Word
  * Memory Data Size : Word
  * Data Transfer Mode : Normal
  * Priority Level : High
  * Peripheral Address : TIM2_CCR2 register
  * Memory Address : aBuffer
  * The length of transfered data : BUFFER_SIZE
  * @param  None
  * @retval None
  */
static void DMA1_Config(void)
{
 /* Reset the DMA Channel 7 Control register*/ 
 DMA1_Channel7->CCR = ((uint32_t)~(DMA_CCR_PL | DMA_CCR_MSIZE | DMA_CCR_PSIZE | \
                      DMA_CCR_MINC | DMA_CCR_PINC | DMA_CCR_CIRC | \
                      DMA_CCR_DIR));
	
 /* Configure the DMA1 Channel 7 */
 /* Set the direction bits : Peripheral to Memory */
 DMA1_Channel7->CCR |= DMA_PERIPH_TO_MEMORY;
	
 /* Disable the Peripheral increment */
 DMA1_Channel7->CCR |= DMA_PINC_DISABLE;
	
 /* Enable the Memory increment */
 DMA1_Channel7->CCR |= DMA_MINC_ENABLE;
	
 /* Set the Peripheral Data size to Word*/
 DMA1_Channel7->CCR |= DMA_PDATAALIGN_WORD;
	
 /* Set the Memory Data size to Word*/	
 DMA1_Channel7->CCR |= DMA_MDATAALIGN_WORD;
	
 /* Set the transfer mode to Normal*/	
 DMA1_Channel7->CCR |= DMA_NORMAL;
	
 /* Set the Priority to High*/	
 DMA1_Channel7->CCR |= DMA_PRIORITY_HIGH;
	
 /* Configure the source, destination address and the data length */
 DMA1_Channel7->CNDTR = CAPTURE_BUFFER_SIZE;

 /* Configure DMA Channel destination address */
 DMA1_Channel7->CPAR = (uint32_t)&(TIM2->CCR2);
 
 /* Configure DMA Channel source address */
 DMA1_Channel7->CMAR = (uint32_t)aCaptureBuffer;
 
 /* Enable the transfer Error interrupt */
 DMA1_Channel7->CCR |= DMA_IT_TE;
	
}
/**
  * @brief  Starts the TIM2 Input Capture measurement on DMA mode :
  * Channel : TIM_CHANNEL_2
  * @param  None
  * @retval None
  */

static void TIM2_DMA1_Capture(void)
{
 /* Enable the DMA1 */
 DMA1_Channel7->CCR |=  DMA_CCR_EN;
 
 /* Enable the TIM Capture/Compare 2 DMA request */
 TIM2->DIER |= TIM_DMA_CC2;
 
 /*Enables the TIM Capture Compare Channel 2.*/
 TIM2->CCER |= TIM_CCER_CC2E;

 /*Enable the TIM2*/
 TIM2->CR1 |= TIM_CR1_CEN;
	
 /* wait until the transfer complete*/
 while ((DMA1->ISR & DMA_ISR_TCIF7) == RESET) 
 {
 }
}

/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
  * @brief  Calculate the PPM :
  * @param  None
  * @retval None
  */

static void PPM_Calculate(void)
{
 uint16_t i= 0;
  for (i = 0; i < PPM_BUFFER_SIZE ; i++)
  {
   aPPMBuffer[i] = (TREF - ((aCaptureBuffer[i+1] - aCaptureBuffer[i]))) * CONSTANT;
  }
}

/** Pinout Configuration
*/
void GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();

  /**TIM2 GPIO Configuration    
   PA0     ------> TIM2_ETR
   PA1     ------> TIM2_CH2 
   */
#ifdef USE_ETR 
   GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
#else
   GPIO_InitStruct.Pin = GPIO_PIN_1;
#endif /* USE_ETR*/
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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
