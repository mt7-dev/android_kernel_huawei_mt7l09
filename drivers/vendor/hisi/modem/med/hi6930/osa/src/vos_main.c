/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: vos_main.c                                                      */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement root function                                      */
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
/* 3. Date: 2007-03-10                                                       */
/*    Author: Xu Cheng                                                       */
/*    Modification: A32D07254                                                */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_typdef.h"
#include "vos_config.h"
#include "v_io.h"
#include "v_blkmem.h"
#include "v_queue.h"
#include "vos_id.h"
#include "v_int.h"
#include "v_lib.h"

#if (VOS_CPU_TYPE ==  VOS_HIFI)
#include <xtensa/config/specreg.h>
#include <xtensa/config/core.h>
#include <xtensa/simcall.h>
#endif

#ifdef VOS_VENUS_TEST_STUB
#include "stdio.h"
#include "venus_stub.h"
#endif


#define    THIS_FILE_ID        PS_FILE_ID_VOS_MAIN_C

#if (VOS_WIN32 == VOS_OS_VER)
extern VOS_UINT32 VOS_SemCtrlBlkInit(VOS_UINT32 ulSemCtrlBlkNumber);

extern VOS_UINT32 VOS_TaskCtrlBlkInit(VOS_UINT32 ulTaskCtrlBlkNumber);
#endif


VOS_UINT32 vos_StartUpStage = 0;
VOS_UINT32 vos_MainStartCnt = 0;

/*****************************************************************************
 Function   : VOS_Startup
 Description: startup function
 Calls      :
 Called By  : root
 Input      : None
 Return     : VOS_OK or VOS_ERROR
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_Startup( VOS_VOID )
{
    VOS_UINT32      ulStartUpFailStage = 0;

    if ( VOS_OK != VOS_MemInit() )
    {
        ulStartUpFailStage |= 0x0001;
    }

#if (VOS_WIN32 == VOS_OS_VER)
    if ( VOS_OK != VOS_SemCtrlBlkInit(VOS_MAX_SEM_NUMBER) )
    {
        ulStartUpFailStage |= 0x0002;
    }
#endif

    if ( VOS_OK != VOS_QueueCtrlBlkInit(VOS_MAX_QUEUE_NUMBER) )
    {
        ulStartUpFailStage |= 0x0004;
    }

#if (VOS_WIN32 == VOS_OS_VER)
    if ( VOS_OK != VOS_TaskCtrlBlkInit(VOS_MAX_TASKS-1) )
    {
        ulStartUpFailStage |= 0x0008;
    }
#endif

    if ( VOS_OK != VOS_PidCtrlBlkInit() )
    {
        ulStartUpFailStage |= 0x0010;
    }

    if ( VOS_OK != VOS_FidCtrlBlkInit() )
    {
        ulStartUpFailStage |= 0x0020;
    }

    if ( VOS_OK != CreateFidsQueque() )
    {
        ulStartUpFailStage |= 0x0040;
    }

    if ( VOS_OK != VOS_FidsInit() )
    {
        ulStartUpFailStage |= 0x0080;
    }

    /* create FID task & selftask task */
    if ( VOS_OK != CreateFidsTask() )
    {
        ulStartUpFailStage |= 0x0100;
    }

    if ( VOS_OK != VOS_PidsInit() )
    {
        ulStartUpFailStage |= 0x0200;
    }

    /* calculate return value */
    if( 0 != ulStartUpFailStage )
    {
        vos_StartUpStage = ulStartUpFailStage;

        Print1("startup retuen value is %ld.\r\n",ulStartUpFailStage);

        return(ulStartUpFailStage);
    }

    return(VOS_OK);
}

/*****************************************************************************
 Function   : root
 Description: main function
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID root( VOS_VOID)
{
    Print("\r\n!!!!! VOS_Startup Begin !!!!!\r\n");

#if (VOS_WIN32 == VOS_OS_VER)
    VOS_SplInit();
#endif

    VOS_Startup();

    Print("\r\n!!!!! VOS_Startup End !!!!!\r\n");

    return;
}

#if (VOS_ZOS == VOS_OS_VER)

extern VOS_VOID VOSInit(VOS_VOID);
extern VOS_VOID VOSStart(VOS_VOID);

/*****************************************************************************
 Function   : main
 Description: main function
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
#if (VOS_HIFI == VOS_CPU_TYPE)
int main(VOS_VOID)
{
#endif

#if (VOS_ARM == VOS_CPU_TYPE)
VOS_VOID vosmain(VOS_VOID)
{
#endif

    /* 进入main函数的次数统计 */
    vos_MainStartCnt++;

    #ifdef ZOS_SIM
    VOS_DEBUG_LOG_CPU_INFO(0x00000006);
    VOS_InitTest();
    #endif

    VOSInit();

    root();

    VOSStart();

    return 0;
}

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

