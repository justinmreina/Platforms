;*-----------------------------------------------------------------------------
;* Source Location: OMAPL1x/support/src/
;* File: ARM9_handler.asm
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
;*	handler.asm
;*
;*		This file includes functions that can handle the ARM exceptions. It is
;*		important to note that the SWI handler needs further implementation to
;*		handle other software exceptions besides #327680 which is used to
;*		switch from a user mode to a privileged mode. Also, please review the
;*		comments at the end of this file for an example of how to use the AINTC
;*		in conjunction with the IRQ and FIQ handlers.
;*
;*		This module performs the following actions:
;*			1) Defines ARM exception handlers
;*
;*-----------------------------------------------------------------------------

; Global symbols defined here
	.global SWI_handler
	.global IRQ_handler
	.global FIQ_handler
	.global NOPRIV

__AINTC_GPVR_ADDR:.set 0xFFFEE084		; Address of AINTC GPVR register


;****************************************************************************
;* FUNCTION DEF: SWI_handler
;****************************************************************************
SWI_handler:.asmfunc
		STMFD	R13!, {R0-R12, R14}		; Store registers on SVC stack
		SUB		R13, R13, #0x4			; Adjust
		LDR		R0, [R14, #-4]			; Load SWI instruction into R0
		BIC		R0, R0, #0xFF000000		; R0 contains SWI #
		CMP		R0, #327680				; Compare to exit user mode
		BNE		NOPRIV					; General SWI handling, not available
		MRS		R0, SPSR				; Now will
		BIC		R0, R0, #0x1F			;	return from
		ORR		R0, R0, #0x1F			;	SVC mode
		MSR		SPSR_CF, R0				; 	with privileges
NOPRIV:	ADD		R13, R13, #0x4			; Adjust
		LDMFD	R13!, {R0-R12, R14}		; Restore registers from SVC stack
		MOVS	PC, R14					; Return to program with privileges
	.endasmfunc


;****************************************************************************
;* FUNCTION DEF: IRQ_handler
;****************************************************************************	
IRQ_handler:.asmfunc
		STMFD	R13!, {R0-R12, R14}		; Store registers on IRQ stack
		SUB		R13, R13, #0x4			; Adjust
		LDR		R0, _AINTC_GPVR_ADDR	; R0 contains address of GPVR
										; 	(which contains ISR address)
		LDR		R1, [R0]				; R1 contains address of highest 
										; 	prioritized ISR
		ADD		R14, PC, #0				; Save return address in LR, 
										; 	PC = PC + 8 bytes,see documentation
		LDR		PC, [R1]				; Go to ISR (still in IRQ mode), 
										; 	will return to following line
		ADD		R13, R13, #0x4			; Adjust
		LDMFD	R13!, {R0-R12, R14}		; Restore registers from IRQ stack
		SUBS	PC, R14, #0x4			; Return to program current status
										;	before IRQ exception
	.endasmfunc


;****************************************************************************
;* FUNCTION DEF: FIQ_handler (same as IRQ_handler)
;****************************************************************************
FIQ_handler:.asmfunc
		STMFD	R13!, {R0-R12, R14}		; Store registers on FIQ stack
		SUB		R13, R13, #0x4			; Adjust
		LDR		R0, _AINTC_GPVR_ADDR	; R0 contains address of GPVR
										;	(which contains ISR address)
		LDR		R1, [R0]				; R1 contains address of highest
										;	prioritized ISR
		ADD		R14, PC, #0				; Save return address in LR,
										;	PC = PC + 8 bytes,see documentation
		LDR		PC, [R1]				; Go to ISR (still in FIQ mode),
										;	will return to following line
		ADD		R13, R13, #0x4			; Adjust
		LDMFD	R13!, {R0-R12, R14}		; Restore registers from FIQ stack
		SUBS	PC, R14, #0x4			; Return to program current status
										;	before FIQ exception
	.endasmfunc	


;****************************************************************************
;* CONSTANTS USED BY THIS MODULE
;****************************************************************************
_AINTC_GPVR_ADDR .long __AINTC_GPVR_ADDR


;****************************************************************************
;* SAMPLE AINTC SETUP AND USE - THIS IS NOT EXECUTABLE CODE
;****************************************************************************
;			/*** Pointer to AINTC Registers ***/
;	CSL_AintcRegsOvly aintcRegs = (CSL_AintcRegsOvly)(CSL_AINTC_0_REGS);
;			/*** ISR Table ***/
;	ISRpointer sysISRtbl[MAX_ISR_ASSIGNMENTS] = {NULL};
;			/*** Sample ISR Declaration	***/
;	void GPIO_input_isr (void);
;
;			/*** Assign ISR to GPIOB0 ***/
;	sysISRtbl[AINTC_GPIO_B0INT] = &GPIO_input_isr;
;			/*** VBR Points to ISR Table ***/
;	aintcRegs->VBR = (unsigned int) sysISRtbl;
;			/*** Pointers Equal 4 Bytes ***/
;	aintcRegs->VSR = 0;
;			/*** GPIOB0 Ints Map to Ch 2 ***/
;	CSL_FINS(aintcRegs->CMR10, AINTC_CMR10_CHNL_NPLUS2, 2);
;			/*** Enable GPIOB0 Ints ***/
;	CSL_FINS(aintcRegs->EISR, AINTC_EISR_INDEX, AINTC_GPIO_B0INT);
;			/*** Enable IRQ Ints ***/
;	CSL_FINS(aintcRegs->HIEISR, AINTC_HIEISR_INDEX, 1);
;			/*** Enable Host Ints ***/
;	CSL_FINS(aintcRegs->GER, AINTC_GER_ENABLE, 1);
