/******************************************************************************/
/* This file was taken from STLport <www.stlport.org> and modified by         */
/* Texas Instruments.                                                         */
/******************************************************************************/

/*
 * Copyright (c) 1999
 * Boris Fomitchev
 *
 * Copyright (c) 2014-2014 Texas Instruments Incorporated
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

#ifndef _STLP_INTERNAL_CSTDIO
#define _STLP_INTERNAL_CSTDIO


#if defined (_STLP_USE_NEW_C_HEADERS)
#  if defined (_STLP_HAS_INCLUDE_NEXT)
#    include_next <cstdio>
#  else
#    include _STLP_NATIVE_CPP_C_HEADER(cstdio)
#  endif
#else
#  define _CPP_STYLE_HEADER /* Place functions in std:: namespace */
#  include <stdio.h>
#  undef _CPP_STYLE_HEADER
#endif


#if defined (_STLP_MSVC_LIB) && (_STLP_MSVC_LIB < 1400) || defined (_STLP_USING_PLATFORM_SDK_COMPILER)
inline int vsnprintf(char *s1, size_t n, const char *s2, va_list v)
{ return _STLP_VENDOR_CSTD::_vsnprintf(s1, n, s2, v); }
#endif

#if defined (_STLP_IMPORT_VENDOR_CSTD )
_STLP_BEGIN_NAMESPACE
using _STLP_VENDOR_CSTD::FILE;
using _STLP_VENDOR_CSTD::fpos_t;
using _STLP_VENDOR_CSTD::size_t;

// undef obsolete macros
#  undef putc
#  undef getc
#  undef getchar
#  undef putchar
#  undef feof
#  undef ferror

#  if !defined (_STLP_NO_CSTD_FUNCTION_IMPORTS)
using _STLP_VENDOR_CSTD::clearerr;
using _STLP_VENDOR_CSTD::fclose;
using _STLP_VENDOR_CSTD::feof;
using _STLP_VENDOR_CSTD::ferror;
using _STLP_VENDOR_CSTD::fflush;
using _STLP_VENDOR_CSTD::fgetc;
using _STLP_VENDOR_CSTD::fgetpos;
using _STLP_VENDOR_CSTD::fgets;
using _STLP_VENDOR_CSTD::fopen;
using _STLP_VENDOR_CSTD::fprintf;
using _STLP_VENDOR_CSTD::fputc;
using _STLP_VENDOR_CSTD::fputs;
using _STLP_VENDOR_CSTD::fread;
using _STLP_VENDOR_CSTD::freopen;
using _STLP_VENDOR_CSTD::fscanf;
using _STLP_VENDOR_CSTD::fseek;
using _STLP_VENDOR_CSTD::fsetpos;
using _STLP_VENDOR_CSTD::ftell;
using _STLP_VENDOR_CSTD::fwrite;

 using _STLP_VENDOR_CSTD::getc;
 using _STLP_VENDOR_CSTD::putc;
 using _STLP_VENDOR_CSTD::getchar;
 using _STLP_VENDOR_CSTD::putchar;

using _STLP_VENDOR_CSTD::gets;
using _STLP_VENDOR_CSTD::perror;
using _STLP_VENDOR_CSTD::printf;
using _STLP_VENDOR_CSTD::puts;
using _STLP_VENDOR_CSTD::remove;
using _STLP_VENDOR_CSTD::rename;
using _STLP_VENDOR_CSTD::rewind;
using _STLP_VENDOR_CSTD::setbuf;
using _STLP_VENDOR_CSTD::tmpfile;
using _STLP_VENDOR_CSTD::tmpnam;
using _STLP_VENDOR_CSTD::scanf;
using _STLP_VENDOR_CSTD::setvbuf;
using _STLP_VENDOR_CSTD::sprintf;
using _STLP_VENDOR_CSTD::sscanf;
using _STLP_VENDOR_CSTD::ungetc;
using _STLP_VENDOR_CSTD::vfprintf;
using _STLP_VENDOR_CSTD::vprintf;
using _STLP_VENDOR_CSTD::vsprintf;
#    if (defined (__MWERKS__) || (defined (_STLP_MSVC_LIB) && (_STLP_MSVC_LIB < 1400)) || \
        (defined (__BORLANDC__)))
using _STLP_VENDOR_CSTD::vsnprintf;
#    endif
#  endif /* _STLP_NO_CSTD_FUNCTION_IMPORTS */
_STLP_END_NAMESPACE
#endif /* _STLP_IMPORT_VENDOR_CSTD */

#endif /* _STLP_INTERNAL_CSTDIO */
