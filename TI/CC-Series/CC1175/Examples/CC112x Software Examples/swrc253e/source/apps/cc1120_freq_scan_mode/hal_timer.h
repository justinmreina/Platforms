/******************************************************************************
*  Filename: hal_f5_timer.h
*
*  Description: Implementation file for basic timer operations needed for 
*               frequency hopping and frequency scanning algorithms
*
*  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/
#include "hal_types.h"

/******************************************************************************
 * PROTOTYPES - SETUP
 */
void hal_timer_init(uint16 master_count);
void hal_timer_stop(void);

/******************************************************************************
 * PROTOTYPES - ADJUSTMENTS
 */
void hal_timer_adjust(uint16 adjust);
uint16 hal_timer_get_current(void);

/******************************************************************************
 * PROTOTYPES - WAIT FUNCTIONS
 */
void hal_timer_expire(void);
void hal_timer_wait(uint16 time);

/******************************************************************************
 * PROTOTYPES - ISR CONNECTOR FUNCTIONS
 */
void timer_isr_register(void (*pfnIntHandler)(void));
void timer_isr_deregister(void (*pfnIntHandler)(void));
