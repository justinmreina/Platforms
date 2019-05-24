/**
  @page NpulseWave_TIM_Sync example
  
  @verbatim
  ******************** (C) COPYRIGHT 2015 STMicroelectronics *******************
  * @file    STM32F302R8-Nucleo/NpulseWave_TIM_Sync/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    27-10-2015
  * @brief   Description of the NpulseWave_TIM_Sync example.
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

This example shows how to interconnect two timers to generate N pulses waveform
The example is split into two sub-examples:
- PERIODIC_NPULSES_GENERATION
- ONESHOT_COMPLYMENTARY_NPULSES_GENERATION

To run one of the sub-example, you should uncomment of the relevant macros in “main.h”
For example:
#define PERIODIC_NPULSES_GENERATION
//#define ONESHOT_COMPLYMENTARY_NPULSES_GENERATION

Then the "SystemClock_Config()" function is used to configure the system clock
(SYSCLK) to run at 64 MHz.

Now, We describe sub-examples:

-------------------------------------------------------------
Part 1: PERIODIC_NPULSES_GENERATION sub-example description:
-------------------------------------------------------------

   This example aims to generate a Periodic N pulse waveform.
   The targeted waveform is N pulses at 20 KHz generated periodically at 50Hz.

   To achieve this, we use both TIM1 and TIM2 interconnected as follow:
     - TIM2: As Master trigger mode, it trigs the timer 1 with frequency of 50Hz.
     - TIM1: As Slave trigger mode/one pulse mode, it generates N pulses each 
             trig-event of TIM2.  
   TIM1CLK and TIM2CLK is fixed to SystemCoreClock, the TIM1 and TIM2 Prescaler 
   are set to have: TIM1 counter clock = TIM2 counter clock = 1 MHz 
   
   TIM1 prescaler = TIM2 prescaler = (uint16_t) ((SystemCoreClock ) / 1000000) - 1 
   
   * The Main Configuration of TIM1:
      - TIM1 Period = Timer counter clock / Frequency = 1000000 / 20000 = 50 us
      - TIM1 duty cycle = TIM1 Period / 2 = 25us = 50%
      - TIM1 repetition counter = N-1 = PULSES_NUMBER -1  
	  
   * The main Configuration of TIM2:
      - TIM2 Period = Timer counter clock / Frequency = 1000000 / 50 = 20000 us
	  
   Each period, N pusles is generated (by default N = 6). To modify the number of
   pulses you should use PULSES_NUMBER macro in the "main.h" file.
	  
   The TIM1 waveform can be displayed using an oscilloscope on TIM1_CH1 : pin PA.08.

--------------------------------------------------------------------------
Part 2: ONESHOT_COMPLYMENTARY_NPULSES_GENERATION sub-example description:
--------------------------------------------------------------------------   
   This sub-example aims to generate two complementary N pulses' waveforms, for only one shot.
   
   The targeted waveforms is 2 KHz PWMs which contain only 20 pulses each period; 500us as  
   pulse period and 50% as pulse duty cycle. Both waveforms are complementary.
	  -> the total time of 20 pulses is Tt = 500*20 = 10000 us.
	
   To achieve this, we use both TIM1 and TIM2 interconnected as follow:
   
    * TIM2: As Master trigger update/ Salve trigger reset mode.
	  -> The slave TIM2 will be reset by TIM1 when enabled to be synchronized 
	     and start counting at the same time.
	  .- TIM2 period = Tt = 10000 us.
	  -> TIM2 will trig the TIM1 after a programmed period equal to Tt.
			 
    * TIM1: As Master trigger reset mode/ Slave trigger mode.
	. TIM1 channel1 is configured to generate two complementary outputs OC1 and OC1N.
	. The polarity of both outputs is set to active High.
	. The commutation (COM) event is enabled to manage the switching-off complementary
          outputs.
	 -> When Master TIM1 is enabled, it will reset its slave TIM2.         
	 -> After Tt, the slave TIM1 will be triggered by TIM2 and the COM event occurs to 
	    stop the pulses' generation with two possible level-state after switching-off.

   When TIM1 is enabled, it reset the TIM2 and thus both timer are synchronized and their 
   counters start counting at the same time. After a time equal to Tt (TIM2 period), TIM2 
   trigs the TIM1 and a commutation event occurs on TIM1. The commutation event will stop 
   both complementary channel generation. There is two possible switching-off configuration 
   can be selected by user:
   
     . DISABLING_CHANNELS_OUTPUTS : will disable both complementary channels and we get
       a low level for both channels at the end.
       
     . SET_CHANNEL_TO_INACTIVE_STATE: will set the channel1 to the inactive state. At the end,
       we get a low level in OC1 and High level in OC1N at the end.
     
  User can select on oh both possible level-states after switching-off by uncommenting  
  one of the relevant  macros in "main.h"
  For example:
  //#define DISABLING_CHANNELS_OUTPUTS
  #define SET_CHANNEL_TO_INACTIVE_STATE  
  
  To be able to visualize correctly the generated waveforms proceed as follow:
    - Download the example into flash memory.
    - Connect the oscilloscope probes to the TIM_CH1 and TIM1_CH1N outputs 
    - Activate the trigger mode and select “Edge “ as trigger type for the TIM1_CH1 source.
    - Adjust the level button of the Trigger control to 3V level.
    - Push the single mode of the Run control.
    - Push the reset button of Nucleo board to restart the example and detect the one shot waveforms.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents 

  - STM32F302R8-Nucleo/NpulseWave_TIM_Sync/Src/main.c                 Main program
  - STM32F302R8-Nucleo/NpulseWave_TIM_Sync/Src/stm32f3xx_it.c         Interrupt handlers 
  - STM32F302R8-Nucleo/NpulseWave_TIM_Sync/Src/stm32f3xx_hal_msp.c    HAL MSP module
  - STM32F302R8-Nucleo/NpulseWave_TIM_Sync/Inc/main.h                 Main program header file  
  - STM32F302R8-Nucleo/NpulseWave_TIM_Sync/Inc/stm32f3xx_hal_conf.h   HAL Configuration file
  - STM32F302R8-Nucleo/NpulseWave_TIM_Sync/Inc/stm32f3xx_it.h         Interrupt handlers header file

        
@par Hardware and Software environment  

  - This example runs on STM32F302R8 device.
    
  - This example has been tested with STMicroelectronics STM32F302R8-Nucleo RevC
    boards and can be easily tailored to any other supported device 
    and development board.


  - STM32F302R8-Nucleo RevC Set-up
    - Connect the TIM1 pins to an oscilloscope to monitor the different waveforms:
      - TIM1_CH1 : pin PA.08 : pin 8 in CN9  
      - TIM1_CH1N: pin PA.07 : pin 26 in cn10  


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Download and Run the example
 - Use the osciloscope and applying the recommendation mentionned to visuale waveforms

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
