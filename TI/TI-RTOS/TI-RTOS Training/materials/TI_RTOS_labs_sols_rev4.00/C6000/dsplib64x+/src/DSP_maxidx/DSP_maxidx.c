/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  DSPLIB  DSP Signal Processing Library                                   */
/*                                                                          */
/*  This library contains proprietary intellectual property of Texas        */
/*  Instruments, Inc.  The library and its source code are protected by     */
/*  various copyrights, and portions may also be protected by patents or    */
/*  other legal protections.                                                */
/*                                                                          */
/*  This software is licensed for use with Texas Instruments TMS320         */
/*  family DSPs.  This license was provided to you prior to installing      */
/*  the software.  You may review this license by consulting the file       */
/*  TI_license.PDF which accompanies the files in this library.             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* DSP_maxidx.c -- Maximum Vector Value                                     */
/*                 Intrinsic C Implementation                               */
/*                                                                          */
/* Rev 0.0.1                                                                */
/*                                                                          */
/*    USAGE                                                                 */
/*        This routine is C callable, and has the following C prototype:    */
/*                                                                          */
/*        int DSP_maxidx (                                                  */
/*            const short *x,                                               */
/*            int nx                                                        */
/*        );                                                                */
/*                                                                          */
/*        x       = pointer to input data                                   */
/*        nx      = number of samples                                       */
/*        return  = max index                                               */
/*                                                                          */
/*    DESCRIPTION                                                           */
/*       This routine finds the maximum value of the vector x[ ] and        */
/*       returns the index of that value.                                   */
/*       The input array is treated as 16 separate "columns" that are       */
/*       interleaved throughout the array.  If values in different columns  */
/*       are equal to the maximum value, then the element in the leftmost   */
/*       column is returned.  If two values within a column are equal to    */
/*       the maximum, then the one with the lower index is returned.        */
/*       Column takes precedence over index.                                */
/*                                                                          */
/*       int maxidx(short *x, int nInputs);                                 */
/*       {                                                                  */
/*        int    i;                                                         */
/*        int    maxLocation;                                               */
/*        short  maxValue;                                                  */
/*        maxLocation = nInputs - 1;                                        */
/*        maxValue = Input[maxLocation];                                    */
/*        for (i = nInputs-2; i >= 0; i--) {                                */
/*                if (maxValue < Input[i]) {                                */
/*                        maxValue = Input[i];                              */
/*                        maxLocation = i;                                  */
/*                }                                                         */
/*        }                                                                 */
/*        return  maxLocation;                                              */
/*       }                                                                  */
/*                                                                          */
/*       The above C code is a general implementation without  restrictions.*/
/*       The assembly code may have some restrictions, as noted below.      */
/*                                                                          */
/*    TECHNIQUES                                                            */
/*       The code is unrolled 16 times to enable the full bandwidth of LDDW */
/*       and MAX2 instructions to be utilized. This splits the search into  */
/*       16 subranges. The global maximum is then found from the list of    */
/*       maximums of the subsranges.                                        */
/*       Then using this offset from the subranges, the global maximum and  */
/*       the index of it are found using a simple match.                    */
/*       For common maximums in multiple ranges, the index will be          */
/*       different to the above C code.                                     */
/*                                                                          */
/*    ASSUMPTIONS                                                           */
/*       The input data are stored on double-word aligned boundaries.       */
/*       nx must be a multiple of 16 and >= 32                              */
/*                                                                          */
/* ======================================================================== */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_maxidx, ".text:intrinsic");

#include "DSP_maxidx.h"

int DSP_maxidx (
    const short * Input,
    int           nInputs
)
{
	short Max[16];

    int i;
    int d3d2, d1d0, d7d6, d5d4, dbda, d9d8, dfde, dddc;
    int m3m2, m1m0, m7m6, m5m4, mbma, m9m8, mfme, mdmc;
    int maxLocation, maxIndex, maxValue;

    double input_dword0, input_dword1;
    double input_dword2, input_dword3;

    /*--------------------------------------------------------------------*/
    /* Infrom the compiler that the "Input", "Max" arrays are double word */
    /* aligned.                                                           */
    /*--------------------------------------------------------------------*/
    _nassert((int) Input % 16 == 0);
    _nassert((int) Max   % 16 == 0);

    /*--------------------------------------------------------------------*/
    /* Load the data from the first 16 values, to initialize the maximums */
    /* for 16 parallel searches. The individual words within a double     */
    /* word are accessed using the _lo and _hi intrinsics.                */
    /*--------------------------------------------------------------------*/
    input_dword0 = _amemd8_const(&Input[0]);
    input_dword1 = _amemd8_const(&Input[4]);
    input_dword2 = _amemd8_const(&Input[8]);
    input_dword3 = _amemd8_const(&Input[12]);

    m1m0 = _lo(input_dword0);
    m3m2 = _hi(input_dword0);
    m5m4 = _lo(input_dword1);
    m7m6 = _hi(input_dword1);
    m9m8 = _lo(input_dword2);
    mbma = _hi(input_dword2);
    mdmc = _lo(input_dword3);
    mfme = _hi(input_dword3);

    /*--------------------------------------------------------------------*/
    /* For all inputs past the first 16, perform parallel searches for    */
    /* the maximums. Each iteration of the loop loads 16 new values and   */
    /* updates the maximum within each of the 16 columns. Inform the co-  */
    /* mpiler that the following loop iterates at least twice.            */
    /*--------------------------------------------------------------------*/

    #pragma MUST_ITERATE(1,,1);
    for (i = 16; i < nInputs; i += 16) {
        /*----------------------------------------------------------------*/
        /* Load input data as double words and access low and high words  */
        /* using the _lo and _hi intrinsic.                               */
        /*----------------------------------------------------------------*/

        input_dword0 = _amemd8_const(&Input[i +  0]);
        input_dword1 = _amemd8_const(&Input[i +  4]);
        input_dword2 = _amemd8_const(&Input[i +  8]);
        input_dword3 = _amemd8_const(&Input[i + 12]);

        d1d0 = _lo(input_dword0);
        d3d2 = _hi(input_dword0);
        d5d4 = _lo(input_dword1);
        d7d6 = _hi(input_dword1);
        d9d8 = _lo(input_dword2);
        dbda = _hi(input_dword2);
        dddc = _lo(input_dword3);
        dfde = _hi(input_dword3);

        /*---------------------------------------------------------------*/
        /* Update current set of maximums using the max2 instruction     */
        /*---------------------------------------------------------------*/
        m1m0 = _max2(m1m0, d1d0);
        m3m2 = _max2(m3m2, d3d2);
        m5m4 = _max2(m5m4, d5d4);
        m7m6 = _max2(m7m6, d7d6);
        m9m8 = _max2(m9m8, d9d8);
        mbma = _max2(mbma, dbda);
        mdmc = _max2(mdmc, dddc);
        mfme = _max2(mfme, dfde);
    }

    /*-------------------------------------------------------------------*/
    /* Store local maximums within each of the 16 columns to the Max     */
    /* array.                                                            */
    /*-------------------------------------------------------------------*/
    _amemd8(&Max[0])  = _itod(m3m2, m1m0);
    _amemd8(&Max[4])  = _itod(m7m6, m5m4);
    _amemd8(&Max[8])  = _itod(mbma, m9m8);
    _amemd8(&Max[12]) = _itod(mfme, mdmc);

    /*-------------------------------------------------------------------*/
    /* Search for largest value, global max and where it occurs, 1 of 16 */
    /*-------------------------------------------------------------------*/
    maxValue = Max[0];
    maxIndex = 0;

//  #pragma UNROLL(16) - this will save a few cycles but will expand code size considerably
    for (i = 0; i < 16; i++) {
        if (Max[i] > maxValue) {
            maxValue = Max[i];
            maxIndex = i;
        }
    }

    /*-------------------------------------------------------------------*/
    /* Use maxIndex to find the column to search within. Search within   */
    /* the column to find the index of the element in the vector.        */
    /*-------------------------------------------------------------------*/
    maxLocation = maxIndex;

    #pragma MUST_ITERATE(1,,1);
    for (i = nInputs - 16 + maxIndex; i >= 0; i -= 16)
        if (Input[i] == maxValue) maxLocation = i;

    /*-------------------------------------------------------------------*/
    /* Return the location where the maximum value was found             */
    /*-------------------------------------------------------------------*/
    return maxLocation;
}

/* ======================================================================== */
/*  End of file:  DSP_maxidx.c                                              */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

