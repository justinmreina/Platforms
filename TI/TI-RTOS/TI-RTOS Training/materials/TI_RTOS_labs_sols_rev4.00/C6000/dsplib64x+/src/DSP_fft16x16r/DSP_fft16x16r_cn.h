/* ======================================================================= */
/* DSP_fft16x16r_cn.h -- 16x16r Mixed Radix FFT                            */
/*                       Natural C Implementation                          */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_FFT16X16R_CN_H_
#define DSP_FFT16X16R_CN_H_ 1

void DSP_fft16x16r_cn (
    int n,
    short * ptr_x,
    const short * ptr_w,
    short * ptr_y,
    int radix,
    int offset,
    int nmax
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_fft16x16r_cn.h                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

