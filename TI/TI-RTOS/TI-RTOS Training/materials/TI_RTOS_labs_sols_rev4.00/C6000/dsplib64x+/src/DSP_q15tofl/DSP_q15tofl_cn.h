/* ======================================================================= */
/* DSP_q15tofl_cn.h -- Q15 to Float conversion                             */
/*                     Optimized C Implementation (w/ Intrinsics)          */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_Q15TOFL_CN_H_
#define _DSP_Q15TOFL_CN_H_ 1

void DSP_q15tofl_cn (
    const short *restrict q15,  /* Input Q15 array      */
    float *restrict flt,        /* Output float array   */
    short nx                    /* Number of elements   */
);

#endif /* _DSP_Q15TOFL_CN_H_ */

/* ======================================================================= */
/*  End of file:  DSP_q15tofl_cn.h                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

