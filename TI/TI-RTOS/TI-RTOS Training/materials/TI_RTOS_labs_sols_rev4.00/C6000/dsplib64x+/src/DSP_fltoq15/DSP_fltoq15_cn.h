/* ======================================================================= */
/* DSP_flttoq15_cn.h -- Float to Q15 conversion                            */
/*                      Natural C Implementation                           */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_FLTOQ15_CN_H_
#define _DSP_FLTOQ15_CN_H_ 1

void DSP_fltoq15_cn (
    const float *restrict flt,  /* Input float array    */
    short *restrict q15,        /* Output Q15 array     */
    short nx                    /* Number of elements   */
);

#endif /* _DSP_FLTOQ15_CN_H_ */

/* ======================================================================= */
/*  End of file:  DSP_fltoq15_cn.h                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

