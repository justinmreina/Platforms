/**
  @page CycleByCycle_Regulation_BKP Cycle-by-Cycle regulation using Break input example
  
  @verbatim
  ******************** (C) COPYRIGHT 2015 STMicroelectronics *******************
  * @file    STM32F302R8-Nucleo/CycleByCycle_Regulation_BKP/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    28-10-2015
  * @brief   Description of the CycleByCycle_Regulation_BKP example.
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

This example show how to use the TIM1 peripheral to realize a cycle by cycle
regulation by using the break input function, and to simulate  motor we was used
an potentiometer and a capacitor.

the potentiometer value = 50kOhm
the capacitor value = 10 µF


        ____________________________
        |                          |
        |                          |
       _|_       _____________     |
       |  |      |            |    |
       |  |      |            |    |
       |  |----->| STM32F302  |----|-----> PWM output
       |  | _|_  |            |    
       |__| ___  |____________|    
        |    |
        |____|
        |
       _|_ GND                   

We want to get TIM1 counter clock at 500 KHz.
TIM1CLK = HSI = SystemCoreClock = 8MHz, 
Prescaler = (TIM1CLK / TIM1 counter clock) -1
          = (8000000 / 500000) - 1
          =  15

The autoreload value is 999 (TIM1->ARR), so the maximum frequency value to update
event is TIM1 counter clock / (TIM1_ARR + 1) = 500000 / 1000 = 500 Hz.

The TIM1 is configured as follows:
The break function is enabled, external signal is connected to TIM1 CH1 pin 
(PC.00).

We consider the initial TIM_Pulses of the PWM mode 1 waveform is defines as follows:
pulse = CCR1 / TIM1 counter clock = 900/500000 = 1,8 ms
The (TIM_Period - TIM_Pulse) define the pulse value, the delay value is fixed to:
delay value = (TIM_Period - TIM_Pulse)/ TIM1 counter clock
            = (1000- 900) / 500000 = 200 us.
            
The VREF is stocked in RAM and it will be compared with the input voltage from the
potentiometer.
We use the DAC and the COMP4 to compare the input voltage (PB.00) with the 
setpoint stored in the RAM.
The COMP4 output is connected to the TIM1_BKIN.

                            |\
                            | \
           Voltage input ---|  \
                            |   \_________ COMP4 Output to the TIM1 BRK
                            |   /
                    VREF ---|  /
                            | /
                            |/

if voltage input >= setpoint -----> COMP4 Output = 1
if voltage input < setpoint -----> COMP4 Output  = 0

If the BRK is activated the Timer PWM output will be disabled, we set the
automatic output enable AOE bit in the TIM_BDTR to enable automatically the PWM 
output at the next update event.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents 

  - STM32F302R8-Nucleo/CycleByCycle_Regulation_BKP/Inc/stm32f3xx_hal_conf.h    HAL configuration file
  - STM32F302R8-Nucleo/CycleByCycle_Regulation_BKP/Src/stm32f3xx_it.c          interrupt handlers
  - STM32F302R8-Nucleo/CycleByCycle_Regulation_BKP/Src/main.c                  Main program
  - STM32F302R8-Nucleo/CycleByCycle_Regulation_BKP/Src/stm32f3xx_hal_msp.c     HAL MSP file
  

@par Hardware and Software environment  
  - This example runs on STM32F302R8 devices.
    
  - This example has been tested with STM32F302R8-Nucleo board and can be
    easily tailored to any other supported device and development board.
  - STM32F302R8-Nucleo RevC Set-up
  - Connect the PC.00 (TIM1_CH1) : pin 38 in CN7 to the potentiometer
  - Connect the potentiometer output to the PB.O0 ( COMP4 input): pin 34 in CN7 	
  - Connect The PC.00 (TIM1_CH1) : pin 38 in CN7 to an oscilloscope to visualize 
  the change of the resistor value on the duty cycle. 
  

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

