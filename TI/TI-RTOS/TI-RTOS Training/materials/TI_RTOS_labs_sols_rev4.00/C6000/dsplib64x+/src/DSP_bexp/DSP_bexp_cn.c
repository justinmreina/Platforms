/* ======================================================================= */
/* DSP_bexp_c.c -- Block exponent of a vector                              */
/*                 Natural C Implementation                                */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     int DSP_bexp_cn                                                     */
/*     (                                                                   */
/*         const int *x,     // Input Data                                 */
/*         short      nx     // Number of elements                         */
/*     );                                                                  */
/*                                                                         */
/*  Description                                                            */
/*     The bexp function, performs a determination of the block            */
/*     exponent of the vector of elements and returns the maximum          */
/*     exponent.  This information can then be used to re-normalize        */
/*     the vector.  This operation is useful when auto-scaling is          */
/*     required as in a FFT.  The bexp operation returns the minimum       */
/*     norm of the vector.  The minimum norm corresponds to the            */
/*     maximum exponent.                                                   */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays x and r do not overlap                                       */
/*                                                                         */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_bexp_cn, ".text:ansi");

int DSP_bexp_cn (
    const int *x,   /* Input data           */
    short nx        /* Number of elements   */
)
{
    int i;
    unsigned int mask, mag;

    mask = 0;

    /* -------------------------------------------------------------------- */
    /*  Merge the absolute values of all the elements together.  The        */
    /*  leftmost bit-change on this mask gives us the magnitude we desire.  */
    /* -------------------------------------------------------------------- */
#ifndef NOASSUME
    _nassert((int)x % 4 == 0);
    _nassert(nx % 4 == 0);
    #pragma MUST_ITERATE(4,,4);
#endif
    for (i = 0; i < nx; i++)
        mask |= x[i] ^ (x[i] >> 31);

    /* -------------------------------------------------------------------- */
    /*  Find the magnitude of the mask.                                     */
    /* -------------------------------------------------------------------- */
    for (mag = 0; (1 << mag) < mask; mag++)
        ;

    return 31 - mag;
}

/* ======================================================================= */
/*  End of file:  DSP_bexp_c.c                                             */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

