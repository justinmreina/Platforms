/* ======================================================================= */
/* DSP_fir_gen_hM17_rA8X8_cn.h -- FIR Filter (Radix 8)                     */
/*                                Natural C Implementation                 */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_FIR_GEN_HM17_RA8X8_CN_H_
#define DSP_FIR_GEN_HM17_RA8X8_CN_H_

void DSP_fir_gen_hM17_rA8X8_cn (
    const short *restrict x,    /* Input array [nr+nh-1 elements] */
    const short *restrict h,    /* Coeff array [nh elements]      */
    short       *restrict r,    /* Output array [nr elements]     */
    int nh,                     /* Number of coefficients         */
    int nr                      /* Number of output samples       */
);

#endif /* DSP_FIR_GEN_HM17_RA8X8_CN_H_ */

/* ======================================================================= */
/*  End of file:  DSP_fir_gen_hM17_rA8X8_cn.h                              */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

