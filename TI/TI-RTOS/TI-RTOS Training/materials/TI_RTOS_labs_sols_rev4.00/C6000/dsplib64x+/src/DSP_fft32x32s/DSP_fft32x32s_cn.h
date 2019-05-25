/* ======================================================================= */
/* DSP_fft32x32s_cn.h -- 32x32s Mixed Radix FFT                            */
/*                       Natural C Implementation                          */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_FFT32X32S_CN_H_
#define DSP_FFT32X32S_CN_H_ 1

void DSP_fft32x32s_cn (
    const int * ptr_w,
    int npoints,
    int * ptr_x,
    int * ptr_y
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_fft32x32s_cn.h                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

