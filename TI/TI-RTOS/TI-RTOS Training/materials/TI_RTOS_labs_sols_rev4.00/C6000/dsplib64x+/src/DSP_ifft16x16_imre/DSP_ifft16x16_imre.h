/* ======================================================================= */
/* DSP_ifft16x16_imre.h -- 16x16 Mixed Radix Inverse FFT                   */
/*                         Intrinsic C Implementation                      */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_IFFT16X16_IMRE_H_
#define DSP_IFFT16X16_IMRE_H_ 1

void DSP_ifft16x16_imre_i (
    const short * restrict ptr_w,
    int npoints,
    short * restrict ptr_x,
    short * restrict ptr_y
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_ifft16x16_imre.h                                      */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

