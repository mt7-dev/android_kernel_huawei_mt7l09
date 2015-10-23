/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_lib.c                                                         */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement general function                                   */
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

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_lib.h"
#include "v_blkmem.h"
#include "v_io.h"
#if (VOS_ZOS == VOS_OS_VER)
#include "kernel.h"
#endif

#if (VOS_DEBUG == VOS_DOPRA_VER)
#include "stdarg.h"
#endif

#ifdef VOS_VENUS_TEST_STUB
#include "stdio.h"
#endif

#include "med_drv_timer_hifi.h"

#define    THIS_FILE_ID        FILE_ID_V_LIB_C

VOS_UINT32        g_ulErrorNo = 0;/* global error number */


/******************************************************************************
 Function    : V_SetErrorNo
 Description : Set global error number.
 Input       : ulErrorNo -- Error number
 Return      : VOS_OK on Success and VOS_ERROR on Failure
 *****************************************************************************/
VOS_UINT32 V_SetErrorNo( VOS_UINT32 ulErrorNo,
                         VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    VOS_UINT32 ulTemp = 0;

    /* the definition of bit
       31 30 29 28 27 26 25 24 -> file id
       23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 -> line number
       7  6  5  4  3  2  1  0 ->error numer
    */

    ulTemp = ulFileID;

    ulTemp <<= 16;

    ulTemp |= (0xffff&usLineNo);

    ulTemp <<= 8;

    ulTemp |= (0xffff&ulErrorNo);

    g_ulErrorNo = ulTemp;

    LogPrint3("# VOS_SetErrorNo, F:%ld, L:%ld, ErrNo:%ld.\r\n",
        (VOS_INT)ulFileID, usLineNo, (VOS_INT)ulErrorNo);

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_GetTick
 Description: Get the system time
 Input      : None
 Output     : pSysTime -- If this function is returned successfully,
                          pSysTime will be the system date/time/week day
 Return     : VOS_OK on success, Error code on failure
 *****************************************************************************/
VOS_UINT32 VOS_GetTick( VOS_VOID )
{
#if (VOS_HIFI == VOS_CPU_TYPE)
    #ifdef VOS_VENUS_TEST_STUB
        return 0;
    #else
        return DRV_TIMER_ReadSysTimeStamp();
    #endif
#else
    return GetTickCount();
#endif
}

#if (VOS_DEBUG == VOS_DOPRA_VER)

#if ( ( VOS_ARM == VOS_CPU_TYPE ) || ( VOS_X86 == VOS_CPU_TYPE ) )

#if (VOS_WIN32 == VOS_OS_VER)
/* which comes from MFC of WIN32 */
//extern VOS_VOID zprint(VOS_CHAR *str);
#define zprint printf
#endif

#if (VOS_ARM == VOS_CPU_TYPE )
//#include <fioLib.h>
#endif

#define VOS_MAX_PRINT_LEN           1024

/*****************************************************************************
 Function   : vos_printf
 Description: Print function
 Input      : format -- Format string to print
 Output     : None
 Return     : VOS_OK on success and VOS_ERROR on error
 *****************************************************************************/
VOS_INT32 vos_printf( VOS_CHAR * format, ... )
{
    VOS_INT32  rc;
    VOS_UINT32 ulReturn = VOS_OK;
    va_list    argument;
    VOS_CHAR   output_info[VOS_MAX_PRINT_LEN + 4];
    VOS_CHAR   *pcWarning = "\r\n# vos printf err: unknown internal error.\r\n";

    va_start( argument, format );
#if (VOS_WIN32 == VOS_OS_VER)
    rc = _vsnprintf( output_info, VOS_MAX_PRINT_LEN, format, argument );
#endif

#if (VOS_ARM == VOS_CPU_TYPE )
    //rc = vnprintf( output_info, format, argument );
#endif
    va_end( argument );

    output_info[VOS_MAX_PRINT_LEN - 1] = '\0';

    if( rc >= VOS_MAX_PRINT_LEN - 1 )
    {
        VOS_UINT32  ulTempLen;
        VOS_CHAR *  pcWarning = " [!!!Warning: Print too long!!!]\r\n";
        ulTempLen = strlen( pcWarning );
        strncpy( output_info + ( VOS_MAX_PRINT_LEN - ulTempLen - 1 ), pcWarning,
            ulTempLen );
        rc = VOS_MAX_PRINT_LEN - 1;

        return VOS_ERR;
    }
    else if( rc < 0 )
    {
        strcpy( output_info, pcWarning );
        rc = strlen( pcWarning );

        return VOS_ERR;
    }

#if (VOS_ARM == VOS_CPU_TYPE)

#endif

#if (VOS_WIN32 == VOS_OS_VER)
    zprint(output_info );
#endif

    return (VOS_INT32)ulReturn;
}

#endif /* VOS_CPU_TYPE */

#endif /* VOS_DOPRA_VER */


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


