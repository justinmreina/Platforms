/**
  @page NpulseWave_OnePulseMode N-pulse waveform generation using One Pulse mode example
  
  @verbatim
  ******************** (C) COPYRIGHT 2015 STMicroelectronics *******************
  * @file    STM32F302R8-Nucleo/NpulseWave_OnePulseMode/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    28-10-2015
  * @brief   Description of the NpulseWave_OnePulseMode example.
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

This example shows how to use the TIM preipheral to generate a N pulses thankx 
to the use of the One Pulse Mode and the Repeatition counter.

In our example, the objective is to generate waveform that contains 10 pulses
with frequency = 200 Hz and duty Cycle = 60%
We want to get TIM1 counter clock at 1MHz
TIM1CLK = HSI = SystemCoreClock = 8MHz
Prescaler = (TIM1CLK / TIM1 counter clock) - 1
            = (8000000 / 1000000) - 1
            = 7
The autoreload value is 5000 (TIM1->ARR), so the maximum frequency value to
overflow the TIM1 is TIM1 counter clock / (TIM1_ARR + 1) = 1000000/4999 = 200 Hz.

The TIM1 is configured as follows:
- The One pulse mode is enabled
- PWM Mode 2 activated 
- The PWM output external signal is connected to TIM1 CH1 pin (PC.00)

The TIM_Pulses defines the delay value, the delay value is fixed to:
delay = CCR1/TIM1 counter clock = 2000/1000000 = 2 ms.
The (TIM_Period - TIM_Pulse) define the One pulse value, the pulse value is fixed
to:
One Pulse value = (TIM_Period - TIM_Pulse)/TIM1 counter clock
                = (5000 - 2000) / 1000000 = 3 ms

The repetition counter allows for the timer to repeate the one pulse N times if
the REP register = N-1.
In this example the RepeatCounter = 9 so the TIM1 will generate 10 pulses.

The TIM1 generate N pulses and automatically stop when finished.



@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents 

  - STM32F302R8-Nucleo/NpulseWave_OnePulseMode/Inc/stm32f3xx_hal_conf.h     HAL configuration file
  - STM32F302R8-Nucleo/NpulseWave_OnePulseMode/Src/stm32f3xx_it.c           interrupt handlers
  - STM32F302R8-Nucleo/NpulseWave_OnePulseMode/Src/main.c                   Main program
  - STM32F302R8-Nucleo/NpulseWave_OnePulseMode/Src/stm32f3xx_hal_msp.c      HAL MSP file
  

@par Hardware and Software environment  
  - This example runs on STM32F302R8 devices.
  - This example has been tested with STM32F302R8-Nucleo board and can be
    easily tailored to any other supported device and development board.
  - STM32F302R8-Nucleo RevC Set-up	
  - Connect The PC.00 : pin 38 in CN7 to an oscilloscope to visualize the N pulses. 
  

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

