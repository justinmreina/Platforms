/******************************************************************************/
/* This file was taken from STLport <www.stlport.org> and modified by         */
/* Texas Instruments.                                                         */
/******************************************************************************/

/*
 * Copyright (c) 2005
 * Francois Dumont
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
 */

/* This header contains npos definition used in basic_string and rope
 * implementation. It do not have to be guarded as files including it
 * are already guarded and it has sometimes to be included several times.
 */

#if defined (_STLP_STATIC_CONST_INIT_BUG)
  enum { npos = -1 };
#else
  static const size_t npos = ~(size_t)0;
#endif
