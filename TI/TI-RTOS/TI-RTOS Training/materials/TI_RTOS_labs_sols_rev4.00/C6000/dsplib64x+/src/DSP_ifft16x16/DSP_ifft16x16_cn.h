/* ======================================================================= */
/* DSP_ifft16x16_cn.h -- 16x16 Mixed Radix Inverse FFT                     */
/*                       Natural C Implementation                          */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_IFFT16X16_CN_H_
#define DSP_IFFT16X16_CN_H_ 1

void DSP_ifft16x16_cn (
    const short * ptr_w,
    int npoints,
    short * ptr_x,
    short * ptr_y
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_ifft16x16_cn.h                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

