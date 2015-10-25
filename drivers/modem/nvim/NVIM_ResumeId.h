/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: NVIM_ResumeId.h                                                 */
/*                                                                           */
/* Author: Jiang kaibo                                                       */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-06                                                             */
/*                                                                           */
/* Description: Def NV Resume ID num.                                        */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2008-06                                                          */
/*    Author: Jiang kaibo                                                    */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef _NVIM_RESUMEID_H
#define _NVIM_RESUMEID_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "product_config.h"

extern unsigned short   g_ausNvResumeManufactureIdList[];

extern unsigned short   g_ausNvResumeUserIdList[];

extern unsigned short   g_ausNvResumeSecureIdList[];


extern unsigned long bsp_nvm_getRevertNum(unsigned long enNvItem);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



#endif

