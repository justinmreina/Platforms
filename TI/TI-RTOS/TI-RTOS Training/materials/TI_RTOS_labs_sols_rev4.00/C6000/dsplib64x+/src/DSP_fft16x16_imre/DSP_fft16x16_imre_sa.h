/* ======================================================================= */
/* DSP_fft16x16_imre_sa.h -- Serial ASM Implementation                     */
/*                                                                         */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_FFT16X16_IMRE_SA_H_
#define DSP_FFT16X16_IMRE_SA_H_ 1

void DSP_fft16x16_imre (
    const short * restrict ptr_w,
    int npoints,
    short * restrict ptr_x,
    short * restrict ptr_y
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_fft16x16_imre_sa.h                                    */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

