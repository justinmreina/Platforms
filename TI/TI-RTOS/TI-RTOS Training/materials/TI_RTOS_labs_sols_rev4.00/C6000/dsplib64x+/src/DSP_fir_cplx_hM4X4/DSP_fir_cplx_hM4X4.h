/* ======================================================================= */
/* DSP_fir_cplx_hM4X4.h -- Complex FIR Filter                              */
/*                         Optimized C Implementation (w/ Intrinsics)      */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_FIR_CPLX_HM4X4_H_
#define _DSP_FIR_CPLX_HM4X4_H_

void DSP_fir_cplx_hM4X4 (
    const short *restrict x,    /* Input array [nr+nh-1 elements] */
    const short *restrict h,    /* Coeff array [nh elements]      */
    short       *restrict r,    /* Output array [nr elements]     */
    int nh,                     /* Number of coefficients         */
    int nr                      /* Number of output samples       */
);

#endif /* _DSP_FIR_CPLX_HM4X4_H_ */

/* ======================================================================= */
/*  End of file:  DSP_fir_cplx_hM4X4.h                                     */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

