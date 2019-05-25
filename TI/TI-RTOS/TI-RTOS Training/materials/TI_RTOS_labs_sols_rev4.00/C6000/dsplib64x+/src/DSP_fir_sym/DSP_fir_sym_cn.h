/* ======================================================================= */
/* DSP_fir_sym_cn.h -- Symmetric FIR Filter                                */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_FIR_SYM_CN_H_
#define _DSP_FIR_SYM_CN_H_ 1

void DSP_fir_sym_cn
(
    const short *restrict x,    /* Input samples                   */
    const short *restrict h,    /* Filter taps                     */
    short       *restrict r,    /* Output samples                  */
    int nh,                     /* Number of symmetric filter taps */
    int nr,                     /* Number of output samples        */
    int s                       /* Final output shift              */
);

#endif /* _DSP_FIR_SYM_CN_H_ */

/* ======================================================================= */
/*  End of file:  DSP_fir_sym_cn.h                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

