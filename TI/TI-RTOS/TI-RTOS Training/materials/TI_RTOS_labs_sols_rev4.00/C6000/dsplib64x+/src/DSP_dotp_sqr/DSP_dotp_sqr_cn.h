/* ======================================================================= */
/* DSP_dotp_sqr_cn.h - Vector Dot Product and Square implementation        */
/*                     Natural C Implementation interface header file      */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_DOTP_SQR_CN_H_
#define DSP_DOTP_SQR_CN_H_

int DSP_dotp_sqr_cn (
    int G,            /* Sum-of-y-squared initial value */
    const short  *x,  /* First input array              */
    const short  *y,  /* Second input array.            */
    int *restrict d,  /* Accumulation of x[] * y[].     */
    int nx            /* Length of the input vector.    */
);

#endif /* DSP_DOTP_SQR_CN_H_ */

/* ======================================================================== */
/*  End of file:  DSP_dotp_sqr_cn.h                                         */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

