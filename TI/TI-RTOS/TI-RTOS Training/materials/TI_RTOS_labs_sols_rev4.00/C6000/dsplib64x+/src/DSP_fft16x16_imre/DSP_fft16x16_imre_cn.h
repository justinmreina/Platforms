/* ======================================================================= */
/* DSP_fft16x16_imre_cn.h -- 16x16 Mixed Radix FFT                         */
/*                           Natural C Implementation                      */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef FFT16X16_IMRE_CN_H_
#define FFT16X16_IMRE_CN_H_ 1

void DSP_fft16x16_imre_cn (
    const short * ptr_w,
    int npoints,
    short * ptr_x,
    short * ptr_y
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_fft16x16_imre_cn.h                                    */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

