;*-----------------------------------------------------------------------------
;* Source Location: OMAPL1x/support/src/
;* File: ARM9_intvecs.asm
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
;*	intvecs.asm
;*
;*		This file includes a generic setup to map the ARM exception vectors to
;*		the given corresponding handlers. It is necessary that the section
;*		"INTVECS" is mapped to the ARM local RAM (origin 0xFFFF0000) in your
;*		linker command file.
;*
;*		This module performs the following actions:
;*			1) Maps exception handlers to their ARM exception address
;*
;*-----------------------------------------------------------------------------

; Global symbols defined here
   .global _c_int00
   .global SWI_handler
   .global IRQ_handler
   .global FIQ_handler
   .global INT_VECS


;****************************************************************************
;* MAP EXCEPTION HANDLER TO CORRESPONDING EXCEPTION ADDRESS
;****************************************************************************
	.sect ".intvecs"
INT_VECS:					;  EXCEPTION						  ADDRESS
 	LDR PC, __c_int00      	;Reset Interrupt					(0xFFFF0000)
 	.word 0					;Undefined Instructions Interrupt	(0xFFFF0004)
 	LDR PC, __SWI_handler	;Software Interrupt					(0xFFFF0008)
 	.word 0		    		;Prefetch Abort Interrupt			(0xFFFF000C)
 	.word 0			 		;Data Abort Interrupt				(0xFFFF0010)
 	.word 0					;Reserved For Future Expansion		(0xFFFF0014)
 	LDR PC, __IRQ_handler	;IRQ Interrupt						(0xFFFF0018)
 	LDR PC, __FIQ_handler	;FIQ Interrupt						(0xFFFF001C)


;****************************************************************************
;* CONSTANTS USED BY THIS MODULE
;****************************************************************************
__c_int00 .long _c_int00
__SWI_handler .long SWI_handler
__IRQ_handler .long IRQ_handler
__FIQ_handler .long FIQ_handler
