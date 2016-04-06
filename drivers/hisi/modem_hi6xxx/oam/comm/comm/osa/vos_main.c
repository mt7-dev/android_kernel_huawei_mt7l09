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
/*lint --e{537}*/
#include "v_typdef.h"
#include "v_root.h"
#include "vos_config.h"
#include "v_IO.h"
#include "v_blkMem.h"
#include "v_queue.h"
#include "v_sem.h"
#include "v_timer.h"
#include "vos_Id.h"
#include "v_int.h"
#include "WatchDog.h"
#include "NVIM_Interface.h"
#include "DrvInterface.h"

/* LINUX 不支持 */
#if (VOS_VXWORKS== VOS_OS_VER)
#include "stdio.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_VOS_MAIN_C

typedef struct
{
    VOS_UINT32 ulStartUpStage;
    VOS_UINT32 ulStep;
    VOS_UINT32 ulOutSideStep;
    VOS_UINT16 usFidInitStep;
    VOS_UINT16 usFidInitId;
    VOS_UINT16 usPidInitStep;
    VOS_UINT16 usPidInitId;
}VOS_START_ERR_STEP_STRU;

extern VOS_UINT32       g_ulOmFidInit ;
extern VOS_UINT32       g_ulOmPidInit ;
extern VOS_UINT32       g_ulVosOutsideStep;
extern VOS_UINT16       g_usFidInitStep;
extern VOS_UINT16       g_usFidInitId;
extern VOS_UINT16       g_usPidInitStep;
extern VOS_UINT16       g_usPidInitId;

extern VOS_UINT32 VOS_OutsideInit(VOS_VOID);

extern VOS_VOID OM_RecordMemInit(VOS_VOID);

extern VOS_VOID VOS_RunTask(VOS_VOID);


VOS_UINT32 vos_StartUpStage = 0x00010000;
VOS_UINT32 g_ulVosStartStep = 0;

HTIMER  g_VosProtectInitTimer = VOS_NULL_PTR;


/*****************************************************************************
 Function   : root
 Description: main function
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID root( VOS_VOID)
{
    Print("\n!!!!! VOS_Startup Begin !!!!!\n");

#ifdef BSP_CONFIG_HI3630

#if (VOS_OS_VER == VOS_VXWORKS)
    BSP_SYNC_Wait(SYNC_MODULE_MSP,0);
#elif (VOS_OS_VER == VOS_LINUX)
    BSP_SYNC_Give(SYNC_MODULE_MSP);
#endif

#endif

#if (VOS_WIN32 == VOS_OS_VER)
    VOS_SplInit();
#endif

#ifndef VOS_SUPPORT_APP_MEM
    if ( VOS_OK != VOS_Startup( VOS_STARTUP_INIT_DOPRA_SOFEWARE_RESOURCE ) )
    {
        Print("VOS_Startup Phase 0: Error.\n");
    }
#endif

    if ( VOS_OK != VOS_Startup( VOS_STARTUP_SET_TIME_INTERRUPT ) )
    {
        Print("VOS_Startup Phase 1: Error.\n");
    }

    if ( VOS_OK != VOS_Startup( VOS_STARTUP_CREATE_TICK_TASK ) )
    {
        Print("VOS_Startup Phase2: Error.\n");
    }

    if( VOS_OK != VOS_Startup( VOS_STARTUP_CREATE_ROOT_TASK ) )
    {
        Print("VOS_Startup Phase 3: Error\n");
    }

    if ( VOS_OK != VOS_Startup( VOS_STARTUP_SUSPEND_MAIN_TASK ) )
    {
        Print("VOS_Startup Phase 4: Error\n");
    }

    Print("\n!!!!! VOS_Startup End !!!!!\n");

    return;
}

/*****************************************************************************
 Function   : VOS_ProtectInit
 Description: reboot if OSA can't init
 Calls      :
 Called By  : root
 Input      : None
 Return     : None
 Other      :
 *****************************************************************************/
VOS_VOID VOS_ProtectInit(VOS_UINT32 ulParam1, VOS_UINT32 ulParam2)
{
    VOS_START_ERR_STEP_STRU stStep;

    stStep.ulStartUpStage = vos_StartUpStage;
    stStep.ulStep         = g_ulVosStartStep;
    stStep.ulOutSideStep  = g_ulVosOutsideStep;
    stStep.usFidInitStep  = g_usFidInitStep;
    stStep.usFidInitId    = g_usFidInitId;
    stStep.usPidInitStep  = g_usPidInitStep;
    stStep.usPidInitId    = g_usPidInitId;


    VOS_ProtectionReboot(OSA_EXPIRE_ERROR, 0, 0, (VOS_CHAR *)&stStep, sizeof(VOS_START_ERR_STEP_STRU) );
}


VOS_UINT32 VOS_Startup( enum VOS_STARTUP_PHASE ph )
{
    VOS_UINT32      ulReturnValue;
    VOS_UINT32      ulStartUpFailStage = 0;

    switch(ph)
    {
        case VOS_STARTUP_INIT_DOPRA_SOFEWARE_RESOURCE :
            vos_StartUpStage = 0x00010000;

            if ( VOS_OK != VOS_MemInit() )
            {
                ulStartUpFailStage |= 0x0001;

                break;
            }

            VOS_SemCtrlBlkInit();

            VOS_QueueCtrlBlkInit();

            VOS_TaskCtrlBlkInit();

            VOS_TimerCtrlBlkInit();

            OM_RecordMemInit();

            if ( VOS_OK != RTC_TimerCtrlBlkInit() )
            {
                ulStartUpFailStage |= 0x0100;
            }

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
            if ( VOS_OK != VOS_DrxTimerCtrlBlkInit())
            {
                ulStartUpFailStage |= 0x0200;
            }
#endif

            if ( VOS_OK != VOS_PidCtrlBlkInit() )
            {
                ulStartUpFailStage |= 0x0400;
            }

            if ( VOS_OK != VOS_FidCtrlBlkInit() )
            {
                ulStartUpFailStage |= 0x0800;
            }

            if ( VOS_OK != CreateFidsQueque() )
            {
                ulStartUpFailStage |= 0x1000;
            }
            break;

        case VOS_STARTUP_SET_TIME_INTERRUPT:
            vos_StartUpStage = 0x00020000;
            break;

        case VOS_STARTUP_CREATE_TICK_TASK:
            vos_StartUpStage = 0x00040000;
            /* create soft timer task */
            if ( VOS_OK != VOS_TimerTaskCreat() )
            {
                ulStartUpFailStage |= 0x0001;
            }

            if ( VOS_OK != RTC_TimerTaskCreat() )
            {
                ulStartUpFailStage |= 0x0002;
            }

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
            if ( VOS_OK != VOS_DrxTimerTaskCreat() )
            {
                ulStartUpFailStage |= 0x0004;
            }
#endif

            break;

        case VOS_STARTUP_CREATE_ROOT_TASK:
            vos_StartUpStage = 0x00080000;

            if ( VOS_OK !=
                    VOS_StartCallBackRelTimer(&g_VosProtectInitTimer,
                        DOPRA_PID_TIMER, 20000, 0, 0,
                        VOS_RELTIMER_NOLOOP, VOS_ProtectInit, VOS_TIMER_NO_PRECISION) )
            {
                ulStartUpFailStage |= 0x0001;
            }

            g_ulVosStartStep = 0x0000;

            ulReturnValue = VOS_OutsideInit();

            if(VOS_OK != ulReturnValue)
            {
                ulStartUpFailStage |= ulReturnValue;
            }

            g_ulVosStartStep = 0x0004;

            if ( VOS_OK != WD_TaskCreat() )
            {
                ulStartUpFailStage |= 0x0004;
            }

            g_ulVosStartStep = 0x0008;

            if ( VOS_OK != VOS_FidsInit() )
            {
                ulStartUpFailStage |= 0x0008;
            }

            g_ulVosStartStep = 0x0010;

#if 0
            /* 创建VOS各FID任务之前，锁任务 */
            if ( VOS_OK != VOS_TaskLock() )
            {
                ulStartUpFailStage |= 0x0010;
            }
#endif
            g_ulVosStartStep = 0x0020;

            /* create FID task & selftask task */
            if ( VOS_OK != CreateFidsTask() )
            {
                ulStartUpFailStage |= 0x0020;
            }

#if (VOS_WIN32 != VOS_OS_VER)
            g_ulVosStartStep = 0x0040;

            /* suspend FID task & selftask task */
            if ( VOS_OK != VOS_SuspendFidsTask() )
            {
                ulStartUpFailStage |= 0x0040;
            }
#endif

            g_ulVosStartStep = 0x0080;
#if 0
            /* 创建VOS各FID任务完成后解锁 */
            if ( VOS_OK != VOS_TaskUnlock() )
            {
                ulStartUpFailStage |= 0x0080;
            }
#endif
            g_ulVosStartStep = 0x0100;

            if ( VOS_OK != VOS_PidsInit() )
            {
                ulStartUpFailStage |= 0x0100;
            }
            break;

        case VOS_STARTUP_SUSPEND_MAIN_TASK:
            vos_StartUpStage = 0x00100000;

            g_ulVosStartStep = 0x0001;

            /* Resume任务之前，锁任务 */
            if ( VOS_OK != VOS_TaskLock() )
            {
                ulStartUpFailStage |= 0x0001;
            }

#if (VOS_WIN32 != VOS_OS_VER)

            g_ulVosStartStep = 0x0002;

            /* Resume FID task & selftask task */
            if ( VOS_OK != VOS_ResumeFidsTask() )
            {
                ulStartUpFailStage |= 0x0002;
            }
#endif

            g_ulVosStartStep = 0x0004;

            /* Resume任务之后解锁 */
            if ( VOS_OK != VOS_TaskUnlock() )
            {
                ulStartUpFailStage |= 0x0004;
            }

            g_ulVosStartStep = 0x0008;

            /* stop protect timer */
            VOS_StopRelTimer(&g_VosProtectInitTimer);

 #if (OSA_CPU_CCPU == VOS_OSA_CPU)
            /* OSA初始化完成，需要调用DRV函数通知DRV OSA启动完成 */
            if ( VOS_OK != DRV_CCPU_RESET_OVER() )
            {
                ulStartUpFailStage |= 0x0008;
            }
#endif

#if (VOS_LINUX == VOS_OS_VER)
            VOS_RunTask();
#endif


            break;

        default:
            break;
    }

    /* calculate return value */
    if( 0 != ulStartUpFailStage )
    {
        ulReturnValue = vos_StartUpStage;
        ulReturnValue |= ulStartUpFailStage;
        Print1("startup retuen value is %x.\r\n",ulReturnValue);

        /* reboot */
        VOS_ProtectionReboot(OSA_INIT_ERROR, (VOS_INT)ulReturnValue,
            (VOS_INT)g_ulOmPidInit, (VOS_CHAR *)&g_ulOmFidInit, sizeof(VOS_UINT32));

        return(ulReturnValue);
    }
    else
    {
        return(VOS_OK);
    }
}

#if (VOS_LINUX == VOS_OS_VER)

/*****************************************************************************
 Function   : VOS_ModuleInit
 Description:
 Input      :
 Output     :
 Return     :
 *****************************************************************************/
extern VOS_INT RNIC_InitNetCard(VOS_VOID);


extern VOS_UINT32 APP_VCOM_Init(void);

extern unsigned int is_load_modem(void);

VOS_INT VOS_ModuleInit(VOS_VOID)
{
#ifdef BSP_CONFIG_HI3630
    /* 如果是升级模式，则不启动VOS */
    if(!is_load_modem())

    {
        Print("update or charge mode, will not start vos.\r\n");
        return 0;
    }
#endif
    APP_VCOM_Init();

    root();

    RNIC_InitNetCard();

    return 0;
}

/*****************************************************************************
 Function   : VOS_ModuleExit
 Description:
 Input      :
 Output     :
 Return     :
 *****************************************************************************/
static VOS_VOID VOS_ModuleExit(VOS_VOID)
{
    return;
}

#ifndef BOOT_OPTI_BUILDIN
#ifdef FEATURE_FLASH_LESS
#else

/*when flash less on, VOS_ModuleInit should be called by bsp drv.*/
late_initcall(VOS_ModuleInit);
module_exit(VOS_ModuleExit);


MODULE_AUTHOR("x51137");

MODULE_DESCRIPTION("Hisilicon OSA");

MODULE_LICENSE("GPL");
#endif
#endif

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

