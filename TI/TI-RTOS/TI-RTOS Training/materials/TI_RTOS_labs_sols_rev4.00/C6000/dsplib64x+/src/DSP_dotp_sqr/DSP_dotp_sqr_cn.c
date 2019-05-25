/* ======================================================================= */
/* DSP_dotp_sqr_cn.c - Vector Dot Product and Square implementation        */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_dotp_sqr_cn, ".text:ansi");

int DSP_dotp_sqr_cn
(
    int G,
    const short *x,
    const short *y,
    int *d,
    int nx
)
{
    int i;

    for (i = 0; i < nx; i++) {
        *d += x[i] * y[i];
        G  += y[i] * y[i];
    }

    return G;
}

/* ======================================================================== */
/*  End of file:  DSP_dotp_sqr_cn.c                                         */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

