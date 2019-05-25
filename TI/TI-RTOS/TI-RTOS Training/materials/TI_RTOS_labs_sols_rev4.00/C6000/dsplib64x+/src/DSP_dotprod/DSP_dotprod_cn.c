/* ======================================================================= */
/* DSP_dotprod_cn.c -- Dot product implementation                          */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_dotprod_cn, ".text:ansi");

int DSP_dotprod_cn (
    const short *m,    /* Pointer to first vector  */
    const short *n,    /* Pointer to second vector */
    int count          /* Length of vectors.       */
)
{
    int i, sum = 0;

    for (i = 0; i < count; i++) {
        sum += m[i] * n[i];
    }

    return sum;
}

/* ======================================================================== */
/*  End of file:  DSP_dotprod_cn.c                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

