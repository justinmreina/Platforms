/**
  @page EPAPER_Display  Description of the EPAPER_Display example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    EPAPER_Display/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-June-2014
  * @brief   Description of the EPAPER_Display example.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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

This directory provides a reference  project that can be used to build any firmware application for
STM32L0xx devices using STM32CubeL0 HAL and running on STM32L0xx_Discovery board from STMicroelectronics
to display pictures on the embedded EPAPER module. 

One workspace is provided for STM32L0xx_Discovery board.
This workspace includes the right set of peripherals, linker and startup files.


@par Directory contents 

  - EPAPER_Display/Src/main.c                 Main program
  - EPAPER_Display/Src/system_STM32L0xx.c     STM32L0xx system clock configuration file
  - EPAPER_Display/Src/STM32L0xx_it.c         Interrupt handlers 
  - EPAPER_Display/Src/STM32L0xx_hal_msp.c    HAL MSP module
  - EPAPER_Display/Inc/main.h                 Main program header file  
  - EPAPER_Display/Inc/STM32L0xx_hal_conf.h   HAL Configuration file
  - EPAPER_Display/Inc/STM32L0xx_it.h         Interrupt handlers header file
  - EPAPER_Display/Inc/picture.h              Constant coding each picture to display
  

        
@par Hardware and Software environment  

  - This example runs on STM32L0xx devices.
    
  - This example has been tested with STMicroelectronics STM32L0xx_Discovery board  
    and can be easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
