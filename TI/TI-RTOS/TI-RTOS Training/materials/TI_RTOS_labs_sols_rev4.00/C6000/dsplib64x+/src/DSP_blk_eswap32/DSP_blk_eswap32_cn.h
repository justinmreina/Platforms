/* ======================================================================= */
/* DSP_blk_eswap32_cn.h -- Endian-swap a block of 32-bit values            */
/*                         Natural C Implementation                        */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_BLK_ESWAP32_CN_H_
#define DSP_BLK_ESWAP32_CN_H_ 1

void DSP_blk_eswap32_cn (
    void *restrict src,
    void *restrict dst,
    int  n_words
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_blk_eswap32_cn.h                                      */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

