/* ======================================================================= */
/* DSP_recip16_cn.h -- Reciprocal Fucntion                                 */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_recip16_cn (                                               */
/*         const short * x,                                                */
/*         short * rfrac,                                                  */
/*         short * rexp,                                                   */
/*         short nx                                                        */
/*     )                                                                   */
/*                                                                         */
/*  Description                                                            */
/*     This program performs a reciprocal on a vector of Q15 numbers.      */
/*     The result is stored in two parts: a Q15 part and an exponent       */
/*     (power of two) of the fraction.                                     */
/*     First, the input is loaded, then its absolute value is taken,       */
/*     then it is normalized, then divided using a loop of conditional     */
/*     subtracts, and finally it is negated if the original input was      */
/*     negative.                                                           */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays x, rfrac, and rexp do not overlap                            */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_recip16_cn, ".text:ansi");

#include "DSP_recip16_cn.h"

void DSP_recip16_cn (
    const short * x,    /* Input array                       */
    short * rfrac,      /* Output array containg Fractions   */
    short * rexp,       /* Output array containing Exponents */
    short nx            /* Number of elements in arrays      */
)
{
    int i, j, a, b;
    short neg, normal;

    for(i = nx; i > 0; i--) {
        a = *(x++);
        if(a < 0) {                 /* take absolute value */
            a = -a;
            neg = 1;
        }
        else
            neg = 0;

        normal = _norm(a);          /* normalize           */
        a = a << normal;

        *(rexp++) = normal - 15;    /* store exponent      */
        b = 0x80000000;             /* dividend = 1        */

        for(j = 15; j > 0; j--)
            b = _subc(b, a);        /* divide              */

        b = b & 0x7FFF;             /* clear remainder     */

        if(neg) b = -b;             /* if negative, negate */

        *(rfrac++) = b;             /* store fraction      */
   }
}

/* ======================================================================= */
/*  End of file:  DSP_recip16_cn.c                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

