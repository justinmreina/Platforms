/*
 * cslr_version.h
 *
 * This file contains the macros for Register Chip Support Library (CSL) which 
 * can be used for operations on the respective underlying hardware/peripheral
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/** ============================================================================
 *   @file  csl_version.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  This file contains the version number of different chips
 *
 */
/* =============================================================================
 *  Revision History
 *  ===============
 *  25-Aug-2006 NG  Updated for new chip c6454 for the release v03_00_10_02
 * =============================================================================
 */

#ifndef _CSL_VERSION_H_
#define _CSL_VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/pspiom/cslr/tistdtypes.h>

/* 0xAABBCCDD -> Arch (AA); API Changes (BB); Major (CC); Minor (DD) */
#define CSL_VERSION_ID          (0x03000400)
#define CSL_VERSION_STR         "@(#) CSL Revision: 3.00.04.00;"

#ifdef C6452
#define CSL_CHIP_ID             (0x6452)
#define CSL_CHIP_STR            "TMS320C6452 PG 1.0 (C6452)"
#endif

#ifdef DM648
#define CSL_CHIP_ID             (0x648)
#define CSL_CHIP_STR            "DM648 PG 1.0"
#endif


/**
  @brief    Retrieves the version ID
*/
Uint32 CSL_versionGetID (
    void
);


/**
  @brief    Retrieves the version string
*/
const char *
    CSL_versionGetStr (
        void
);


#ifdef __cplusplus
}
#endif

#endif /* _CSL_VERSION_H_ */

