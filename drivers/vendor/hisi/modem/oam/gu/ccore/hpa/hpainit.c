/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: HPAInit.c                                                       */
/*                                                                           */
/* Author: Xu cheng                                                          */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-02                                                             */
/*                                                                           */
/* Description: HPA Init                                                     */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2008-02                                                          */
/*    Author: Xu cheng                                                       */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#include "hpacomm.h"
#include "DspInterface.h"
#include "DrvInterface.h"
#include "TtfOamInterface.h"
#include "NVIM_Interface.h"
#include "apminterface.h"
#include "sleepsleep.h"
#include "om.h"
#include "omnosig.h"
#include "OamSpecTaskDef.h"
#include "psregrpt.h"
#include "omnvinterface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/* Macro of log */
#define    THIS_FILE_ID        PS_FILE_ID_HPA_INIT_C


/*****************************************************************************
 Function   : HPA_Init
 Description: HPA Init
 Input      : void
 Return     : OK or Error
 Other      :
 *****************************************************************************/
VOS_UINT32 HPA_Init(VOS_VOID)
{
    /* set SFN & CFN */
    g_ucHpaCfnRead = 0;
    g_usHpaSfnRead = 0;

    /* set to 0 */
    atomic_set(&g_stDspMailBoxTransferCount, 0);
    atomic_set(&g_stGDspMailBoxTransferCount, 0);
    atomic_set(&g_stGDsp1MailBoxTransferCount, 0);

    /*Hpa Error Count global Var. Init*/
    VOS_MemSet((VOS_VOID *)(g_astHpaErrorCount), (VOS_CHAR)0x00,
        sizeof(g_astHpaErrorCount));

    /*Hpa Interrupt Count global Var. Init*/
    VOS_MemSet((VOS_VOID *)(&g_stHpaIntCount), (VOS_CHAR)0x00,
        sizeof(HPA_INT_COUNT_STRU));

    if ( VOS_OK != VOS_SmCCreate( "TRAN", 0, VOS_SEMA4_FIFO, &g_ulHpaTransferSem))
    {
        PS_LOG(UEPS_PID_WHPA, 0, PS_PRINT_WARNING,
            "HPA: creat g_ulHpaTransferSem Err");

        return VOS_ERR;
    }

#ifdef HPA_ITT
    g_pfnHpaDspIsrStub = VOS_NULL_PTR;

    g_pfnHpaIttMsgFilterFunc = VOS_NULL_PTR;
#endif

    if(NV_OK != NV_Read(en_NV_Item_Max_Gsm_Reg_Cnt,&g_stGhpaRegCnt,sizeof(g_stGhpaRegCnt)))
    {
        PS_LOG(UEPS_PID_APM, 0, PS_PRINT_WARNING,
            "HPA: Read en_NV_Item_Max_Gsm_Reg_Cnt Err");
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   : atomic_dec
 Description: decrease atomic counter.comes from linux
 Input      : atomic counter
 Return     : void
 Other      :
 *****************************************************************************/
void atomic_dec( hpa_atomic_t *v )
{
    int flags;

    flags = VOS_SplIMP();

    (v->counter)--;

    VOS_Splx(flags);

    return;
}


VOS_VOID HPA_TransferTaskEntry(VOS_VOID)
{
    for ( ; ; )
    {
        if (VOS_OK != VOS_SmP(g_ulHpaTransferSem, 0))
        {
            LogPrint("HPA_TransferTaskEntry: VOS_SmP Fail.\r\n");
#if (VOS_WIN32 != VOS_OS_VER)
            continue;
#else
            break;
#endif
        }

        if ( 0 < (atomic_read(&g_stDspMailBoxTransferCount)) )
        {
            atomic_dec(&g_stDspMailBoxTransferCount);

            PsRegCapture(0, g_usHpaSfnRead, (VOS_UINT32)g_ucHpaCfnRead, PS_REG_SYS_MODE_WCDMA);

            if ( MAIL_BOX_PROTECTWORD_SND == g_ulOmNosigEnable )
            {
                OM_LoopTestProc();
            }

#if (VOS_WIN32 != VOS_OS_VER)
            continue;
#else
            break;
#endif
        }

        if ( 0 < (atomic_read(&g_stGDspMailBoxTransferCount)) )
        {
            atomic_dec(&g_stGDspMailBoxTransferCount);

            PsRegCapture(0, 0, GHPA_GetRealFN(MODEM_ID_0), PS_REG_SYS_MODE_GSM);

#if (VOS_WIN32 != VOS_OS_VER)
            continue;
#else
            break;
#endif
        }

#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )
        if ( 0 < (atomic_read(&g_stGDsp1MailBoxTransferCount)) )
        {
            atomic_dec(&g_stGDsp1MailBoxTransferCount);

            PsRegCapture(0, 0, GHPA_GetRealFN(MODEM_ID_1), PS_REG_SYS_MODE_GSM1);

#if (VOS_WIN32 != VOS_OS_VER)
            continue;
#else
            break;
#endif
        }
#endif
        LogPrint("HPA_TransferTaskEntry: should not.\r\n");
    }
}

/*****************************************************************************
 Function   : HPA_FIDInit
 Description: HPA FID Init
 Input      : Ip
 Return     : void
 Other      :
 *****************************************************************************/
VOS_UINT32 HPA_FIDInit( enum VOS_INIT_PHASE_DEFINE ip )
{
    /* Add for L Only version */
    if (BOARD_TYPE_LTE_ONLY == BSP_OM_GetBoardType())
    {
        return 0;
    }
    /* Add for L Only version */

    switch ( ip )
    {
        case VOS_IP_LOAD_CONFIG:
            if ( VOS_OK != VOS_RegisterPIDInfo(UEPS_PID_SHPA,
                APM_PIDInit, (Msg_Fun_Type)HPA_ApmMsgPIDProc))
            {
                PS_LOG(  UEPS_PID_GHPA, 0, PS_PRINT_ERROR,
                    "VOS_RegisterPIDInfo UEPS_PID_SHPA failure !\n");

                return VOS_ERR;
            }

            if ( VOS_OK
                != VOS_RegisterTaskPrio(WUEPS_FID_HPA, COMM_HPA_TASK_PRIO) )
            {
                PS_LOG(  UEPS_PID_GHPA, 0, PS_PRINT_ERROR,
                    "HPA_FIDInit:VOS_RegisterMsgTaskPrio failure !\n");

                return VOS_ERR;
            }

            if ( VOS_NULL_BYTE == VOS_RegisterSelfTaskPrio(WUEPS_FID_HPA,
                (VOS_TASK_ENTRY_TYPE)HPA_TransferTaskEntry,
                COMM_HPA_SELFTASK_PRIO, HPA_TRANSFER_TASK_STACK_SIZE ) )
            {
                PS_LOG(  UEPS_PID_GHPA, 0, PS_PRINT_ERROR,
                    "VOS_RegisterSelfTask:HPA_TransferTaskEntry failure !\n");

                return VOS_ERR;
            }

            break;

        case VOS_IP_FARMALLOC:
            if(VOS_OK != HPA_Init())
            {
                PS_LOG(  UEPS_PID_GHPA, 0, PS_PRINT_ERROR,
                    "HPA Init failure !\n");
                return VOS_ERR;
            }
            break;

        default:
            break;
    }

    return VOS_OK;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


