/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_IO.c                                                          */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement I/O                                                */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/* 2. Date: 2006-10                                                          */
/*    Author: Xu Cheng                                                       */
/*    Modification: Standardize code                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef _V_IO_H
#define _V_IO_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "dopra_def.h"
#include "vos_config.h"
#include "v_typdef.h"


#if (VOS_DEBUG == VOS_DOPRA_VER)

#if (VOS_HIFI == VOS_CPU_TYPE )
#define vos_printf printf
#else
VOS_INT32  vos_printf( VOS_CHAR * format, ... );
#endif

#define Print( fmt ) vos_printf((fmt))

#define Print1( fmt, larg1 ) vos_printf((fmt), (larg1))

#define Print2( fmt, larg1, larg2 ) vos_printf((fmt), (larg1), (larg2))

#define Print3( fmt, larg1, larg2, larg3)\
    vos_printf((fmt), (larg1), (larg2), (larg3))

#define Print4( fmt, larg1, larg2, larg3, larg4)\
    vos_printf((fmt), (larg1), (larg2), (larg3), (larg4))

#define Print5( fmt, larg1, larg2, larg3, larg4, larg5)\
    vos_printf((fmt), (larg1), (larg2), (larg3), (larg4), (larg5))

#define logMsg vos_printf

#define LogPrint( fmt ) logMsg(fmt)

#define LogPrint1( fmt, larg1 ) logMsg((fmt), (larg1))

#define LogPrint2( fmt, larg1, larg2 )\
    logMsg((fmt), (larg1), (larg2))

#define LogPrint3( fmt, larg1, larg2, larg3)\
    logMsg((fmt), (larg1), (larg2), (larg3))

#define LogPrint4( fmt, larg1, larg2, larg3, larg4)\
    logMsg((fmt), (larg1), (larg2), (larg3), (larg4))

#define LogPrint5( fmt, larg1, larg2, larg3, larg4, larg5)\
    logMsg((fmt), (larg1), (larg2), (larg3), (larg4), (larg5))

#define LogPrint6( fmt, larg1, larg2, larg3, larg4, larg5, larg6)\
    logMsg((fmt), (larg1), (larg2), (larg3), (larg4), (larg5), (larg6))

#else
#define Print( fmt ) ((VOS_VOID)0)

#define Print1( fmt, larg1 ) ((VOS_VOID)0)

#define Print2( fmt, larg1, larg2 ) ((VOS_VOID)0)

#define Print3( fmt, larg1, larg2, larg3) ((VOS_VOID)0)

#define Print4( fmt, larg1, larg2, larg3, larg4) ((VOS_VOID)0)

#define Print5( fmt, larg1, larg2, larg3, larg4, larg5) ((VOS_VOID)0)

#define LogPrint( fmt ) ((VOS_VOID)0)

#define LogPrint1( fmt, larg1 ) ((VOS_VOID)0)

#define LogPrint2( fmt, larg1, larg2 ) ((VOS_VOID)0)

#define LogPrint3( fmt, larg1, larg2, larg3) ((VOS_VOID)0)

#define LogPrint4( fmt, larg1, larg2, larg3, larg4) ((VOS_VOID)0)

#define LogPrint5( fmt, larg1, larg2, larg3, larg4, larg5) ((VOS_VOID)0)

#define LogPrint6( fmt, larg1, larg2, larg3, larg4, larg5, larg6) ((VOS_VOID)0)
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_IO_H */


