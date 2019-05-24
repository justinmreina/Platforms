/**
  @page TIM_ExtCLK_ETR Timer clocking using external input ETR example
  
  @verbatim
  ******************** (C) COPYRIGHT 2015 STMicroelectronics *******************
  * @file    STM32F302R8-Nucleo/TIM_ExtCLK_ETR/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    28-10-2015
  * @brief   Description of the TIM_ExtCLK_ETR example.
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

This example provides a short descripton of how to synchronize the TIM2 by an 
external source clock (mode 2) and the internal source clock (HSI)

To choose between the external or the internal clock source you should comment or
uncomment the macro for conditional compilation, for example:

to select the internal source clock you must comment the USE_ETR macro:
#define USE_ETR

This example will measure the same input signal connected in TIM2 CH2 pin (PA.01)
in two synchronization mode (ETR mode and internal source clock mode) and compare 
the results.

----TIM2 Config---------------------------------------------

ETR mode config:
- HSI = 8MHz
- A calibated external source clock from a waveform generator that 
  frequency = 10MHz connected to the TIM2_CH1 (PA.00) configured as ETR pin
- Asynchronous Divider = 4 => ETRP = 10000000/4 = 2.5 MHz < (HSI/3)
we choose the Asynchronous Divider = 4 to apply the Synchronization condition :
ETRP <= (APBx_CLK / 3) and in this example the APBx_CLK is the HSI_CLK

Internal source clock mode config:
- HSI = 8MHz

Input capture config:
- TIM2 CH2 (PA.01) configured as input capture.

---- DMA Config ------------------------------------------

The DMA is used to store the CCR2 value at each capture interrupt in a buffer to 
calculate the frequency of the input signal

To calculate the period of the incoming signal that is the difference between 
two successive values of the CCR2 register multiplied by the increment period
TIM2 counter.

To calculate the PPM (Part per Million), we apply the following equation:
fm: measured frequency
f:nominal frequency
PPM = (df*10^6)/f with df= fm-f and f = 1kHz
    = ((fm - f) * 10^6 )/ f
    = ((1/Tm - 1/T) * 10^6) / (1/T) with Tm = 1/fm and T= 1/f = 10^(-3)
    = (( T - Tm ) / T ) * 10^6
    = ( T - Tm ) * ( 10^6/ T )
    = ((TREF - Pm) * (10^6 / TREF) with TREF is the ideal number of count between
    two input capture and Pm is the number of count measured between two onput
    capture
	
In this example we will measure a signal that the frequency is 1kHz
In the internal source clock mode :
TREF = INTERN_CLK / MEASURED_SIGNAL
     = 8000000 / 1000
     = 8000
	 
In the ETR mode:
TREF = ETRP_CLK / MEASURED_SIGNAL 
ETRP_CLK is external source Clock frequency after the divider in Hz
TREF = 2500000 / 1000
     = 2500

Note: if you want to change the frequecy of the internal, external Clock or 
the measured signal frequency you just put the new frequency in the correct macro
in main.c

 The result of the PPm will be stocked in others buffer to visualize the difference
 between to two synchronization mode.
  
@note Care must be taken when using HAL_Delay(), this function provides accurate
delay (in milliseconds)
based on variable incremented in SysTick ISR. This implies that if HAL_Delay()
is called from a peripheral ISR process, then the SysTick interrupt must have 
higher priority (numerically lower)than the peripheral interrupt. Otherwise the
caller ISR process will be blocked.
To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority()
function.
      
@note The application need to ensure that the SysTick time base is always set to
1 millisecond to have correct HAL operation.

@par Directory contents 

  - STM32F302R8-Nucleo/TIM_ExtCLK_ETR/Inc/stm32f3xx_hal_conf.h    HAL configuration file
  - STM32F302R8-Nucleo/TIM_ExtCLK_ETR/Src/stm32f3xx_it.c          interrupt handlers
  - STM32F302R8-Nucleo/TIM_ExtCLK_ETR/Src/main.c                  Main program
  - STM32F302R8-Nucleo/TIM_ExtCLK_ETR/Src/stm32f3xx_hal_msp.c     HAL MSP file
  

@par Hardware and Software environment  
  - This example runs on STM32F302R8 devices.
    
  - This example has been tested with STM32F302R8-Nucleo board and can be
    easily tailored to any other supported device and development board.
  - STM32F302R8-Nucleo RevC Set-up	
  - Connect The PA.00 (ETR pin) : pin 28 in CN7 to the external source clock 
    from a waveform generator. 
  - Connect the PA.01 (TIM2_CH2) : pin 30 in CN7 to the external signal from 
  a waveform generator to measure the frequency.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

