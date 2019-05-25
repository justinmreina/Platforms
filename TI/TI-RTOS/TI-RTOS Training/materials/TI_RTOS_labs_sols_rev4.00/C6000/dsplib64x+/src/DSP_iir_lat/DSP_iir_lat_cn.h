/* ======================================================================= */
/* DSP_irr_lat_cn.h -- IIR Lattice Filter                                  */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_IIR_LAT_CN_H_
#define _DSP_IIR_LAT_CN_H_ 1

void DSP_iir_lat_cn (
    short       *x,
    int         nx,
    const short *restrict k,
    int         nk,
    int         *restrict b,
    short       *r
);

#endif /* _DSP_IIR_LAT_CN_H_ */

/* ======================================================================= */
/*  End of file:  DSP_iir_lat_cn.h                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

