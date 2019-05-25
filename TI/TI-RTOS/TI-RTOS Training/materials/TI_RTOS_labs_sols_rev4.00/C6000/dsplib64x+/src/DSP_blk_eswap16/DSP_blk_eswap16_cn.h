/* ======================================================================= */
/* DSP_blk_eswap16_cn.h -- Endian-swap a block of 16-bit values            */
/*                         Natural C Implementation                        */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_BLK_ESWAP16_CN_H_
#define DSP_BLK_ESWAP16_CN_H_ 1

void DSP_blk_eswap16_cn (
    void *restrict src,
    void *restrict dst,
    int  n_hwords
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_blk_eswap16_cn.h                                      */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

