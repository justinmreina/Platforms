/* ======================================================================= */
/* DSP_ifft32x32_cn.h -- 32x32 Mixed Radix Inverse FFT                     */
/*                       Natural C Implementation                          */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_IFFT32X32_CN_H_
#define DSP_IFFT32X32_CN_H_ 1

void DSP_ifft32x32_cn (
    const int * restrict ptr_w,
    int npoints,
    int * restrict ptr_x,
    int * restrict ptr_y
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_ifft32x32_cn.h                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

