;* ======================================================================== *;
;*  TEXAS INSTRUMENTS, INC.                                                 *;
;*                                                                          *;
;*  C64x+DSPLIB  DSP Signal Processing Library                              *;
;*                                                                          *;
;*      Release:        Version 2.2.00.00                                        *;
;*      Snapshot date:  10-October-2010                                         *;
;*                                                                          *;
;*  This library contains proprietary intellectual property of Texas        *;
;*  Instruments, Inc.  The library and its source code are protected by     *;
;*  various copyrights, and portions may also be protected by patents or    *;
;*  other legal protections.                                                *;
;*                                                                          *;
;*  This software is licensed for use with Texas Instruments TMS320         *;
;*  family DSPs.  This license was provided to you prior to installing      *;
;*  the software.  You may review this license by consulting the file       *;
;*  TI_license.PDF which accompanies the files in this library.             *;
;* ------------------------------------------------------------------------ *;
;*          Copyright (C) 2010 Texas Instruments, Incorporated.             *;
;*                          All Rights Reserved.                            *;
;* ======================================================================== *;


This file contains a brief description of the DSP Library and how
to use it.

--------------------------------------------------------------------------------
Release Directory Structure
c64plus
 |
 +--dsplib
     |
     +--docs                Library documentation
     |
     +--example             Example to show DSPLIB usage 
     |
     +--src                 Source code with CCS project examples
     |   |
     |   +--[Kernels]
     |
     |--include
     |    dsplib64plus.h      Header file containing kernels
     |
     |--lib
     |   dsplib64plus.lib       Precompiled COFF library
     |   dsplib64plus_elf.lib  Precompiled ELF library
     |--build
     |   dsplib64plus.pjt    Provided project to rebuild library
     |
     |--README.txt          Top-level README file

------------------------------------------------------------------------
Whats New in release 2.2.0.0
------------------------------------------------------------------------
ELF support
Linux build support
CCSv4 build support 

--------------------------------------------------------------------------------
Using the DSP Library

There are really two ways the DSP Library can be used, one is from
within Code Composer Studio and the other is by using the command line
tools. In both cases, it's a matter of including the library header
file, linking to the correct build of the library, and calling the
APIs.

-------------------------------------------------
Using the DSP Library in Code Composer Studio

1. First you have to specify to the linker to link the DSP Library
   into your application.  You do this by going to
   "Project->Options->Linker->Include Libraries" and then specify the
   DSP Library, dsplib64plus.lib.

2. For each of your C files that you plan to use a DSP Library
   kernel in, you must include the appropriate header file for each
   kernel. For instance, to use the kernel "DSP_fir_gen()", you must
   #include <dsplib64plus.h> in that file.

3. Now you may use the DSP Library API at will.


-----------------------------------------------------
Using the DSP Library with the Command Line Tools

1. You have to tell the linker to link your application with the
   DSP library.  You can do this by using the linker -l option in
   your linker command file or at the linker command line, i.e
   -l dsplib64plus.lib

2. For each of your C files that you plan to use a DSP Library
   kernel in, you must include the appropriate header file for each
   kernel. For instance, to use the kernel "DSP_fir_gen()", you must
   #include <dsplib64plus.h> in that file.


--------------------------------------------------------------------------------
How do I Re-Build the Library?

Please see the documentation in the docs directory for instructions
on how to re-build the library in CCS environment

Linux build:
Set Path to depenedencies in Rules.make file in the root directory and execute  a make in the root directory using 

make all

Note: Execute 'make help' in root directory to view other options.
Make based build allows you to required project directory and build individual project by executing the make instruction.

--------------------------------------------------------------------------------
Changes Since release version:dsplib_v200 

Bug Fix
-------
- DSP_fft16x16r: Function had bug introduced when the function was originally migrated from 64x to 64x+. The bug caused incorrect output for N that is power 4. (64, 256, 1024 etc). The output was correct for other cases (128, 512 etc). The bug has been fixed.
- DSP_ifft16x16: SA/ASM version had bug when handling radix-2 case. The bug has been fixed. 

Enhancement
-----------
- Replaced the intrinsic C version of all FFT routines with SA version as the performance is significantly improved. The intrinsic C version of code is included in the kernel test project but the LIB is built with SA version.
- Document updates in the UG


********************************************************************************
* End of README.TXT
********************************************************************************
