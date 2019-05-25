/* ======================================================================= */
/* DSP_recip16_cn.h -- Reciprocal Fucntion                                 */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_RECIP16_CN_H_
#define _DSP_RECIP16_CN_H_ 1

void DSP_recip16_cn (
    const short * x,    /* Input array                       */
    short * rfrac,      /* Output array containg Fractions   */
    short * rexp,       /* Output array containing Exponents */
    short nx            /* Number of elements in arrays      */
);

#endif /* _DSP_RECIP16_CN_H_ */

/* ======================================================================= */
/*  End of file:  DSP_recip16_cn.h                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

