/**
  @page ArbWaveform_DMAburst example
  
  @verbatim
  ******************** (C) COPYRIGHT 2015 STMicroelectronics *******************
  * @file    STM32F302R8-Nucleo/ArbWaveform_DMAburst/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    27-10-2015
  * @brief   Description of the ArbWaveform_DMAburst.
  ******************************************************************************
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
  @endverbatim

@par Example Description

This example aims to show that arbitrary waveforms can be generated thanks to the use 
of Timer’s DMA burst without CPU intervention/overhead.

This is done by updating, on-the-fly, Timer parameters by updating the relevant registers
from a pre-initialized memory buffer. 

In our example, the objective is to generate waveform that contains a sequence of 
3 categories of pulses with 2 frequencies (F1,F2) and 3 duty cycles (D1,D2,D3):
* 2 pulses (F1,D1)
* 1 pulse  (F2,D2)
* 3 pulses (F1,D3)
With:
    . F1 = 8,0 KHz    
    . F2 = 3,2 KHz      
    . D1 = 25 %       
    . D2 = 85 %      
    . D3 = 50 %       
	
In this example, we use TIM1 with it DMA1 burst.
The parameters and relevant TIM1 registers to be updated are: 
    
    - Frequency            -> update of TIM1_ARR  register
    - Pulse width          -> update of TIM1_CCR1 register
    - Repetition counter   -> update of TIM1_RCR  register
	
The "SystemClock_Config()" function is used to configure the system clock
(SYSCLK) to run at 64 MHz.

TIM1CLK = HCLK = SystemCoreClock
TIM1 counter clock = 32 MHz
    
To get TIM1 counter clock at 32 MHz, the prescaler is computed as follows:
  Prescaler = (TIM1CLK / TIM1 counter clock) - 1
  Prescaler = (SystemCoreClock /32 MHz) - 1
            = (64 MHz /32 MHz) - 1 
            = 1
				
* TIM1 Frequency (Fx) = TIM1 counter clock/(TIM1_ARR + 1)

   -> TIM1_ARR [F1] = (TIM1 counter clock / F2) -1 = (32 MHz / 8 KHz) -1   = 4000-1
   -> TIM1_ARR {F2] = (TIM1 counter clock / F1) -1 = (32 MHz / 3,2 KHz) -1 = 10000-1
  
* TIM1 Channel1 duty cycle (Dx) = (TIM1_CCR1/ TIM1_ARR)* 100 

   -> TIM1_CCR1 [D1]= (D1(%) * TIM1_ARR[F1])/100 = (20 * 4000)/100  = 800 
   -> TIM1_CCR1 [D2]= (D2(%) * TIM1_ARR[F2])/100 = (85 * 10000)/100 = 8500
   -> TIM1_CCR1 [D3]= (D3(%) * TIM1_ARR[F1])/100 = (50 * 4000)/100  = 2000
 
* TIM1_RCR = number of pulses -1
  
So after calculation, we need to update this sequence:
 + 2 pulses (F1,D1) : TIM1_ARR = 4000-1  ; TIM1_RCR = 1  ;  TIM1_CCR1 = 800
 + 1 pulses (F2,D2) : TIM1_ARR = 10000-1 ; TIM1_RCR = 0  ;  TIM1_CCR1 = 8500
 + 3 pulses (F1,D3) : TIM1_ARR = 4000-1  ; TIM1_RCR = 2  ;  TIM1_CCR1 = 2000

So, we consider the following memory buffer DataRegBuffer:
  DataRegBuffer[9] = {1/F1,1,D1,1/F2,0,D2,1/F1,2,D3}
	           = {4000,1,800,10000,0,8500,4000,2,2000}
					 
Each update event, 3 data will be transferred and preloaded to the three Timer registers 
(ARR, RCR, CCR1) using the Timer DMA burst. 

To achieve correctly the TIMER DMA burst transfer, we need to do the following main 
configurations on both Timer and DMA1:

- Configure TIM1 data control register as follow:
   + Enable update DMA
   + DMA base address     = ARR base address : To start update from ARR register 
   + TIM DMA Burst Length = 3                : To update 3 registers starting from base address.
   + Enable the auto-reload preload          : To preload data in the shadow auto-reload register
   + Enable the output compare 1 Preload     : To preload data in the shadow output compare 1 register
   + Enable PWM1 output (OCMODE = 1, MOE = 1, CC1E = 1)
   
- Configure DAM1 channel 5 as follow:	
   +     Data Transfer direction  = Memory to peripheral  
   +     Peripheral Increment     = Disable 
   +     Memory Increment         = Enable  
   +     Peripheral Data Size     = Word 
   +     Memory Data Size         = Word 
   +     Data Transfer mode       = circular mode 
   +     Priority level           = very high priority 
   +     Peripheral address       = TIM DMAR for burst access (see "main.c")
   +     Peripheral address       = DataRegBuffer 
   +     Number of data register  = DataRegBuffer data number (9)
  
  The TIM1 waveforms can be displayed using an oscilloscope.
  You need to stop the displaying by pushing the Run/Stop bottun 
  to make measurement of waveform parameters.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents 

  - STM32F302R8-Nucleo/ArbWaveform_DMAburst/Src/main.c                 Main program
  - STM32F302R8-Nucleo/ArbWaveform_DMAburst/Src/stm32f3xx_it.c         Interrupt handlers 
  - STM32F302R8-Nucleo/ArbWaveform_DMAburst/Src/stm32f3xx_hal_msp.c    HAL MSP module
  - STM32F302R8-Nucleo/ArbWaveform_DMAburst/Inc/main.h                 Main program header file  
  - STM32F302R8-Nucleo/ArbWaveform_DMAburst/Inc/stm32f3xx_hal_conf.h   HAL Configuration file
  - STM32F302R8-Nucleo/ArbWaveform_DMAburst/Inc/stm32f3xx_it.h         Interrupt handlers header file

        
@par Hardware and Software environment  

  - This example runs on STM32F302R8 device.
    
  - This example has been tested with STMicroelectronics STM32F302R8-Nucleo RevC
    boards and can be easily tailored to any other supported device 
    and development board.


  - STM32F302R8-Nucleo RevC Set-up
    - Connect the TIM1 pins to an oscilloscope to monitor the different waveforms:
      - TIM1_CH1 : pin PA.08 : pin 8 in CN9


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
