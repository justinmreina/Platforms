/* ======================================================================== */
/* DSP_maxidx_cn.c -- Maximum Vector Value                                  */
/*                    Natural C Implementation                              */
/*                                                                          */
/* Rev 0.0.1                                                                */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*          int DSP_maxidx_cn (                                             */
/*              const short * Input,      // Input array     //             */
/*              short         nInputs     // Length of input //             */
/*          );                                                              */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The "maxidx" routine finds the largest element in an array,         */
/*      returning the index to that element within the array.               */
/*                                                                          */
/*      The input array is treated as 16 separate "columns" that are        */
/*      interleaved throughout the array.  If values in different columns   */
/*      are equal to the maximum value, then the element in the leftmost    */
/*      column is returned.  If two values within a column are equal to     */
/*      the maximum, then the one with the lower index is returned.         */
/*      Column takes precedence over index within the column.               */
/*                                                                          */
/*      The function returns the index of the maximum value.                */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      The input must be at least 48 elements long, and be a multiple      */
/*      of 16 elements long.                                                */
/*                                                                          */
/*      The code requires at least 48 bytes of stack, to make room for a    */
/*      32-byte temporary working buffer.                                   */
/*                                                                          */
/*      The optimized implementations assume that the Input array is        */
/*      double-word aligned, and are optimized for LITTLE ENDIAN.           */
/*                                                                          */
/*  SOURCE                                                                  */
/*      This is taken from the BDTI 99 spec.                                */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_maxidx_cn, ".text:ansi");

int DSP_maxidx_cn (
    const short * Input, 
    int           nInputs 
) 
{
	int    i, j;
    int    maxLocation, maxIndex;
    short  maxValue;
    short  Max[16];

    #ifndef NOASSUME
    _nassert((int) nInputs >= 32);
    _nassert((int) nInputs % 16 == 0);
    _nassert((int) Input   % 8  == 0);
    #endif

    /* -------------------------------------------------------------------- */
    /*  Initialize our "max" values from the input array.                   */
    /* -------------------------------------------------------------------- */
    for (i = 0; i < 16; i++)
        Max[i] = Input[i];

    /* -------------------------------------------------------------------- */
    /*  Find the maximums for each of 16 "columns" of the input.            */
    /* -------------------------------------------------------------------- */

    #ifndef NOASSUME
    #pragma MUST_ITERATE(1,,1);
    #endif
    for (i = 16; i < nInputs; i += 16)
        for (j = 0; j < 16; j++)
            if (Input[i + j] > Max[j]) Max[j] = Input[i + j];

    /* -------------------------------------------------------------------- */
    /*  Find the leftmost "column" with the largest value.                  */
    /* -------------------------------------------------------------------- */
    maxValue = Max[0];
    maxIndex = 0;
    for (i = 0; i < 16; i++) {
        if (Max[i] > maxValue) {
            maxValue = Max[i];
            maxIndex = i;
        }
    }

    /* -------------------------------------------------------------------- */
    /*  Find the earliest value in the selected column with the max value.  */
    /* -------------------------------------------------------------------- */
    maxLocation = maxIndex;

    #ifndef NOASSUME
    #pragma MUST_ITERATE(1,,1);
    #endif
    for (i = nInputs - 16 + maxIndex; i >= 0; i -= 16)
        if (Input[i] == maxValue) maxLocation = i;

    /* -------------------------------------------------------------------- */
    /*  Return the the location                                             */
    /* -------------------------------------------------------------------- */

    return maxLocation;
}

/* ======================================================================== */
/*  End of file:  DSP_maxidx_cn.c                                           */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

