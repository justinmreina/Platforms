/* ======================================================================= */
/* DSP_ifft16x32_cn.h -- 16x32 Mixed Radix Inverse FFT                     */
/*                       Natural C Implementation                          */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_IFFT16X32_CN_H_
#define DSP_IFFT16X32_CN_H_ 1

void DSP_ifft16x32_cn (
    const short * ptr_w,
    int npoints,
    int * ptr_x,
    int * ptr_y
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_ifft16x32_cn.h                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

