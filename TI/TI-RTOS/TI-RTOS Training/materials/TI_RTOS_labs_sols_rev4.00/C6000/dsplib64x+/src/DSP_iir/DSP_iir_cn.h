/* ======================================================================= */
/* DSP_irr_cn.h -- IIR Filter                                              */
/*                 Natural C Implementation                                */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_IIR_CN_H_
#define _DSP_IIR_CN_H_

short DSP_iir_cn (
    short         Input,
    const short * Coefs,
    int           nCoefs,
    short       * State
);

#endif /* _DSP_IIR_CN_H_ */

/* ======================================================================= */
/*  End of file:  DSP_iir_cn.h                                             */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

