;*-----------------------------------------------------------------------------
;* Source Location: OMAPL1x/support/src/
;* File: ARM9_initstack.asm
;*
;* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
;*
;*  Redistribution and use in source and binary forms, with or without
;*  modification, are permitted provided that the following conditions
;*  are met:
;*
;*    Redistributions of source code must retain the above copyright
;*    notice, this list of conditions and the following disclaimer.
;*
;*    Redistributions in binary form must reproduce the above copyright
;*    notice, this list of conditions and the following disclaimer in the
;*    documentation and/or other materials provided with the
;*    distribution.
;*
;*    Neither the name of Texas Instruments Incorporated nor the names of
;*    its contributors may be used to endorse or promote products derived
;*    from this software without specific prior written permission.
;*
;*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
;*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;* 
;*-----------------------------------------------------------------------------
;* 
;*	initstack.asm
;*
;*		If using the RTS library from the TMS470 Code Generation Tools 4.6.3 or
;*		earlier, include this file (use function _init_stack as an entry point)
;*		to initialize the privileged mode stacks.
;*
;*		This module performs the following actions:
;*			1) Allocates the stack and initializes the stack pointer for SVC,
;*			   IRQ, & FIQ mode
;*			2) Calls the normal initial boot routine (_c_int00)
;*
;*-----------------------------------------------------------------------------

; Global symbols defined here
   .global _c_int00
   .global _init_stack


;****************************************************************************
;* DEFINE PRIVILEGED MODE STACKS: SVC, IRQ, & FIQ
;****************************************************************************
__SVC_STACK_SIZE:.set 0x1000
__IRQ_STACK_SIZE:.set 0x1000
__FIQ_STACK_SIZE:.set 0x1000
__svc_stack:.usect	".svcstack", __SVC_STACK_SIZE, 4
__irq_stack:.usect	".irqstack", __IRQ_STACK_SIZE, 4
__fiq_stack:.usect	".fiqstack", __FIQ_STACK_SIZE, 4


;****************************************************************************
;* FUNCTION DEF: _init_stack
;****************************************************************************
_init_stack:.asmfunc
	; SET TO SVC MODE
	MRS		R0, CPSR
	BIC		R0, R0, #0x1F
	ORR		R0, R0, #0x13
	MSR		CPSR_CF, R0
	
	; INITIALIZE THE SVC MODE STACK
	LDR		SP, c_svc_stack
	LDR		R0, c_SVC_STACK_SIZE
	ADD		SP, SP, R0
	
	; SET TO IRQ MODE
	MRS		R0, CPSR
	BIC		R0, R0, #0x1F
	ORR		R0, R0, #0x12
	MSR		CPSR_CF, R0
	
	; INITIALIZE THE IRQ MODE STACK
	LDR		SP, c_irq_stack
	LDR		R0, c_IRQ_STACK_SIZE
	ADD		SP, SP, R0
	
	; SET TO FIQ MODE
	MRS		R0, CPSR
	BIC		R0, R0, #0x1F
	ORR		R0, R0, #0x11
	MSR		CPSR_CF, R0
	
	; INITIALIZE THE FIQ MODE STACK
	LDR		SP, c_fiq_stack
	LDR		R0, c_FIQ_STACK_SIZE
	ADD		SP, SP, R0
	
	; Continue to _c_int00
	LDR		PC, c_int00
	.endasmfunc


;****************************************************************************
;* CONSTANTS USED BY THIS MODULE
;****************************************************************************
c_int00 .long _c_int00
c_svc_stack .long __svc_stack
c_irq_stack .long __irq_stack
c_fiq_stack .long __fiq_stack
c_SVC_STACK_SIZE .long __SVC_STACK_SIZE
c_IRQ_STACK_SIZE .long __IRQ_STACK_SIZE
c_FIQ_STACK_SIZE .long __FIQ_STACK_SIZE
