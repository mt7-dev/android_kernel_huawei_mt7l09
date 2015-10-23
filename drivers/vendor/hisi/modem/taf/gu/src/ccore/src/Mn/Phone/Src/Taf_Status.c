
#include "om.h"
#include "MmaAppLocal.h"
#include "rfa.h"
#include "NasOmInterface.h"
#include "TTFMemInterface.h"
#include "NVIM_Interface.h"
#include "DrvInterface.h"
/* 删除ExtAppMmcInterface.h*/

#include "TafAppMma.h"
#include "TafMmcInterface.h"
#if ( FEATURE_ON == FEATURE_LTE )
#include "msp_nvim.h"
#endif
#include "NasUtranCtrlInterface.h"

#include "TafSdcLib.h"
#include "Taf_Status.h"

#include "NasUsimmApi.h"
#include "TafLog.h"
#include "TafMmaCtx.h"
#include "TafMmaSndImsa.h"

#include "NasMtaInterface.h"

#include "NasStkInterface.h"

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
#include "MtcMmaInterface.h"
#endif

#include "TafMmaSndTaf.h"
#include "TafMmaProcNvim.h"

#include "TafMmaSndApp.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_TAF_STATUS_C

/* STATUS 全局共享变量 */
STATUS_CONTEXT_STRU                     g_StatusContext;
extern TAF_MMA_GLOBAL_VALUE_ST          gstMmaValue;



extern MMA_ME_PERSONALISATION_STATUS_STRU  g_stMmaMePersonalisationStatus;

MN_PH_REG_TIME_INFO_STRU            g_stRegTimeInfo = {0, 0, 0, 0};

extern MMA_TIMER_ST                            g_stSyscfgWaitDetachCnfTimer;

extern MMA_TIMER_ST                            g_stPowerDownDelayTimer;



extern VOS_UINT32                       g_ucUsimHotOutFlag;




VOS_UINT32                              aStaTimerLen[STA_TIMER_BUTT] = {0};

extern TAF_MMA_LOCATION_STATUS_EVENT_INFO_STRU g_stMmsStkLocStaSysInfo;

VOS_UINT8 g_MMA_IccStatus = 0xFE;

extern MMA_DEACT_SIM_WHEN_POWEROFF_ENUM_UINT8  g_ucMmaDeactSimWhenPoweroff;

#define STA_INFO_PRINT     PS_PRINT_INFO
#define STA_NORMAL_PRINT   PS_PRINT_NORMAL
#define STA_WARNING_PRINT  PS_PRINT_WARNING
#define STA_ERROR_PRINT    PS_PRINT_ERROR


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*----------- STATUS 给TAFM提供的API接口函数 ------------*/
/*=======================================================*/




VOS_UINT32 Sta_MsgReqAbort ( VOS_UINT32 ulAbortType )
{
    VOS_UINT32      ulEvent;        /* 事件类型 */

    /* 未完成请求过程类型 */
    switch ( g_StatusContext.ulFsmState )
    {
        case STA_FSM_RESTART:
            ulEvent = TAF_PH_EVT_OPER_MODE_CNF;
            break;
        case STA_FSM_STOP:
            ulEvent = TAF_PH_EVT_OPER_MODE_CNF;
            break;
        case STA_FSM_PLMN_LIST:
            ulEvent = TAF_PH_EVT_PLMN_LIST_CNF;
            break;
        case STA_FSM_PLMN_SEL:
            ulEvent = TAF_PH_EVT_PLMN_SEL_CNF;
            break;
        case STA_FSM_PLMN_RESEL:
            ulEvent = TAF_PH_EVT_PLMN_RESEL_CNF;
            break;
        case STA_FSM_SYS_CFG_SET:
            ulEvent = TAF_PH_EVT_SYS_CFG_CNF;
            break;

        default:
            /* 未指定的请求过程，返回错误 */
            return STA_ERROR;
    }

    /* 确定返回事件错误类型 */
    if ( STA_MSGABORT_OK != ulAbortType )
    {
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;
    }

    Sta_EventReport ( g_StatusContext.ulCurOpId, ulEvent );

    return STA_SUCCESS;
}


VOS_UINT32 Sta_Attach (VOS_UINT32 ulOpID, VOS_UINT32 ulAttachType, TAF_MMA_EPS_ATTACH_REASON_ENUM_UINT8 enAttachReason)
{
    VOS_UINT8               ucSubNum;   /* 可用的子状态标号*/
    VOS_UINT32               ulRet;

    ucSubNum    = 0;

    /* 入口参数合法性检查 */
    if ( Sta_IsInvalidOpId (ulOpID) )
    {
        STA_TRACE (STA_WARNING_PRINT,"Sta_Attach():WARNING:parameter invalid ");
        return STA_ERROR;
    }

    /* 检查ATTACH请求类型合法性 */
    if ( ulAttachType > STA_ATTACH_TYPE_GPRS_IMSI )
    {
        g_StatusContext.PhoneError    = TAF_ERR_PARA_ERROR;
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

        /* 上报APP当前状态事件 */
        Sta_EventReport (ulOpID, TAF_PH_EVT_ATTACH_CNF);

        /* 打印调试跟踪信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_Attach():WARNING:parameter is wrong");
        return STA_ERROR;
    }

    /* ATTACH状态输入条件检查异常*/
    if (VOS_TRUE != TAF_MMA_IsEnableDeAttach())
    {/* 如果是NULL状态，返回错误 */
        g_StatusContext.PhoneError    = TAF_ERR_PHONE_MSG_UNMATCH;

        /* 事件上报信息的输出类型 */
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

        /* 上报APP当前状态事件 */
        Sta_EventReport(ulOpID, TAF_PH_EVT_ATTACH_CNF);

        /* 打印调试跟踪信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_Attach():WARNING:system FSM state is wrong");
        return STA_ERROR;     /* 错误返回*/
    }/* ATTACH状态输入条件检查异常*/

    /* 检查当前系统激活域状态 */
    /* 判断是否存在下述情况*/
    /* 同样的命令请求是否已经得到响应，一条命令请求不响应的话不允许*/
    /* 不允许进行下条同样的请求，PS和CS域上的ATTACH算不同的请求*/
    switch (ulAttachType)
    {
        case STA_ATTACH_TYPE_GPRS: /* PS*/
            ucSubNum = (VOS_UINT8)Sta_FindIdelFsmSub (STA_FSMSUB_ATTACH_PS);
            break;

        case STA_ATTACH_TYPE_IMSI: /* CS*/
            ucSubNum = (VOS_UINT8)Sta_FindIdelFsmSub (STA_FSMSUB_ATTACH_CS);
            break;

        case STA_ATTACH_TYPE_GPRS_IMSI: /* PS&CS*/
            ucSubNum = (VOS_UINT8)Sta_FindIdelFsmSub (STA_FSMSUB_ATTACH_CSPS);
            break;

        default:    /* 参数错误 */
            g_StatusContext.PhoneError    = TAF_ERR_PARA_ERROR;
            g_StatusContext.ucOperateType = STA_OP_PHONEERROR;
            /* 上报APP当前状态事件 */
            Sta_EventReport (ulOpID, TAF_PH_EVT_ATTACH_CNF);

            /* 输出跟踪调试信息 */
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_Attach():WARNING:parameter is wrong");
            return STA_ERROR;
    }

    /* 无效FSM状态处理 */
    if ( STA_FSMSUB_INDEX_NULL == ucSubNum )
    {
        g_StatusContext.PhoneError    = TAF_ERR_PHONE_MSG_UNMATCH;
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

        /* 上报APP当前状态事件 */
        Sta_EventReport (ulOpID, TAF_PH_EVT_ATTACH_CNF);

        /* 输出跟踪调试信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_Attach():WARNING:system FSM state is wrong");
        return STA_ERROR;
    }

    ulRet = TAF_MMA_SndMmcAttachReq(ulOpID, ulAttachType, enAttachReason);

    if ( VOS_ERR == ulRet )
    {
        STA_TRACE (STA_ERROR_PRINT,
            "Sta_Attach():  failed  ");
        return STA_ERROR;       /* 错误返回 */
    }

    /* 记录当前的操作过程标识号 */
    g_StatusContext.ulCurOpId = ulOpID;
    /*modified by liuyang id:48197 date:2005-10-9 for V100R001 Begin*/
    /*更改主状态*/
    g_StatusContext.ulFsmState = STA_FSM_DE_ATTACH;
    /*modified by liuyang id:48197 date:2005-10-9 for V100R001 End*/

    /* 上报APP当前状态事件 */
    /* 预留的函数接口，暂不使用 */
    /* ulRet = Sta_EventReport (ulOpID, TAF_PH_EVT_ATTACH);*/

    /* 设置子状态标志量，启动保护定时器 */
    if ( STA_ATTACH_TYPE_GPRS == ulAttachType )
    {
        TAF_SDC_SetPsAttachAllowFlg(VOS_TRUE);

        /* PS域ATTACH */
        g_StatusContext.aucFsmSubFlag[ucSubNum]
            = STA_FSMSUB_ATTACH_PS;
        /*设置PS域ATTACH标志;*/
        g_StatusContext.aFsmSub[ucSubNum].ucFsmStateSub
            = STA_FSMSUB_ATTACH_PS;
        g_StatusContext.aFsmSub[ucSubNum].TimerName
            = STA_TIMER_ATTACH_PS;

        /*启动PS域ATTACH定时器;*/
        ulRet = NAS_StartRelTimer(&g_StatusContext.ahStaTimer[ucSubNum],
                                  WUEPS_PID_MMA,
                                  aStaTimerLen[STA_TIMER_ATTACH_PS],
                                  TAF_MMA,
                                  MMA_MAKELPARAM(ucSubNum, STA_TIMER_ATTACH_PS),
                                  VOS_RELTIMER_NOLOOP
                                     );
        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_Attach():ERROR: timer start failed");

            /* 恢复原来的子状态标志 */
            if ( STA_ERROR
                == Sta_UpdateFsmFlag(STA_FSMSUB_ATTACH_PS,
                        0, STA_UPDATE_SINGL_CLEAR) )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_Attach():WARNING:timer clear failed");
            }

            return STA_ERROR;       /* 错误返回 */
        }
        /* 更新子状态命令操作标识号 */
        g_StatusContext.aFsmSub[ucSubNum].ulOpId = ulOpID;
        g_StatusContext.ucAttachReq++;
        /* BEGIN: Added by liuyang, id:48197, 2005/10/12 */
        g_StatusContext.aucStaPhoneOperator[ucSubNum] = TAF_PH_EVT_PS_ATTACH_CNF;
        /* END:   Added by liuyang, id:48197, 2005/10/12 */
    }
    else if ( STA_ATTACH_TYPE_IMSI == ulAttachType )
    {
        TAF_SDC_SetCsAttachAllowFlg(VOS_TRUE);

        /* CS域ATTACH */
        g_StatusContext.aucFsmSubFlag[ucSubNum]
            = STA_FSMSUB_ATTACH_CS;
        /* 设置CS域ATTACH标志;*/
        g_StatusContext.aFsmSub[ucSubNum].ucFsmStateSub
            = STA_FSMSUB_ATTACH_CS;
        g_StatusContext.aFsmSub[ucSubNum].TimerName
            = STA_TIMER_ATTACH_CS;

        /*启动CS域ATTACH定时器;*/
        ulRet = NAS_StartRelTimer (
            &g_StatusContext.ahStaTimer[ucSubNum],
            WUEPS_PID_MMA,
            aStaTimerLen[STA_TIMER_ATTACH_CS],
            TAF_MMA,
            MMA_MAKELPARAM(ucSubNum, STA_TIMER_ATTACH_CS),
            VOS_RELTIMER_NOLOOP
             );
        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_Attach():ERROR:timer start failed");

            /* 恢复原来的子状态标志 */
            if ( STA_ERROR
                == Sta_UpdateFsmFlag(STA_FSMSUB_ATTACH_CS,
                        0, STA_UPDATE_SINGL_CLEAR) )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_Attach():WARNING:timer clear failed");
            }

            return STA_ERROR;       /* 错误返回 */
        }
        /* 更新子状态命令操作标识号 */
        g_StatusContext.aFsmSub[ucSubNum].ulOpId = ulOpID;
        g_StatusContext.ucAttachReq++;
        /* BEGIN: Added by liuyang, id:48197, 2005/10/12 */
        g_StatusContext.aucStaPhoneOperator[ucSubNum] = TAF_PH_EVT_CS_ATTACH_CNF;
        /* END:   Added by liuyang, id:48197, 2005/10/12 */
    }
    else
    {
        /* CS&PS ATTATCH */
        TAF_SDC_SetPsAttachAllowFlg(VOS_TRUE);

        TAF_SDC_SetCsAttachAllowFlg(VOS_TRUE);

        g_StatusContext.aucFsmSubFlag[0]
            = STA_FSMSUB_ATTACH_CSPS;
        /*设置PS域ATTACH标志;*/
        g_StatusContext.aFsmSub[0].ucFsmStateSub
            = STA_FSMSUB_ATTACH_PS;
        g_StatusContext.aFsmSub[0].TimerName
            = STA_TIMER_ATTACH_PS;

        /*启动PS域ATTACH定时器;*/
        ulRet = NAS_StartRelTimer (
            &g_StatusContext.ahStaTimer[0],
            WUEPS_PID_MMA,
            aStaTimerLen[STA_TIMER_ATTACH_PS],
            TAF_MMA,
            MMA_MAKELPARAM(0, STA_TIMER_ATTACH_PS),
            VOS_RELTIMER_NOLOOP
            );
        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_Attach():ERROR:timer start failed");

            /* 恢复原来的子状态标志 */
            if ( STA_ERROR
                == Sta_UpdateFsmFlag(STA_FSMSUB_ATTACH_PS,
                        0, STA_UPDATE_SINGL_CLEAR) )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_Attach():WARNING:timer clear failed");
            }
            return STA_ERROR;       /* 错误返回 */
        }
        /* 更新子状态命令操作标识号 */
        g_StatusContext.aFsmSub[0].ulOpId = ulOpID;
        g_StatusContext.ucAttachReq++;
        /* BEGIN: Added by liuyang, id:48197, 2005/10/12 */
        g_StatusContext.aucStaPhoneOperator[0] = TAF_PH_EVT_ATTACH_CNF;
        /* END:   Added by liuyang, id:48197, 2005/10/12 */

        /* CS域ATTACH */
        g_StatusContext.aucFsmSubFlag[1]
            = STA_FSMSUB_ATTACH_CSPS;
        /* 设置CS域ATTACH标志;*/
        g_StatusContext.aFsmSub[1].ucFsmStateSub
            = STA_FSMSUB_ATTACH_CS;
        g_StatusContext.aFsmSub[1].TimerName
            = STA_TIMER_ATTACH_CS;

        /*启动CS域ATTACH定时器;*/
        ulRet = NAS_StartRelTimer (
            &g_StatusContext.ahStaTimer[1],
            WUEPS_PID_MMA,
            aStaTimerLen[STA_TIMER_ATTACH_CS],
            TAF_MMA,
            MMA_MAKELPARAM(1, STA_TIMER_ATTACH_CS),
            VOS_RELTIMER_NOLOOP
            );
        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_Attach():ERROR:timer start failed");

            /* 恢复原来的子状态标志 */
            if ( STA_ERROR
                == Sta_UpdateFsmFlag(STA_FSMSUB_ATTACH_CS,
                        0, STA_UPDATE_SINGL_CLEAR) )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_Attach():WARNING:timer clear failed");
            }
            return STA_ERROR;       /* 错误返回 */
        }
        /* 更新子状态命令操作标识号 */
        g_StatusContext.aFsmSub[1].ulOpId = ulOpID;
        g_StatusContext.ucAttachReq++;
        /* BEGIN: Added by liuyang, id:48197, 2005/10/12 */
        g_StatusContext.aucStaPhoneOperator[1] = TAF_PH_EVT_ATTACH_CNF;
        /* END:   Added by liuyang, id:48197, 2005/10/12 */
    }
     /*
    清空当前的PLMNid
    modified by liuyang id:48197 date:2005-9-5 for V200R001
    */
    /* Sta_ClearCurPlmnId( &(g_StatusContext.CurPlmnId) ); */
    return STA_SUCCESS;             /* 正常返回 */
}




VOS_UINT32 TAF_MMA_GetDetachFsmSubNum(
    VOS_UINT32                          ulDetachType,
    VOS_UINT8                          *pucFsmSubNum

)
{
    /* 根据DETACH TYPE找到对应的字状态机 */
    switch (ulDetachType)
    {
        case STA_PS_DETACH:

            *pucFsmSubNum = (VOS_UINT8)Sta_FindIdelFsmSub (STA_FSMSUB_DETACH_PS);
            break;

        case STA_CS_DETACH:

            *pucFsmSubNum = (VOS_UINT8)Sta_FindIdelFsmSub (STA_FSMSUB_DETACH_CS);
            break;

        case STA_PS_CS_DETACH:

            *pucFsmSubNum = (VOS_UINT8)Sta_FindIdelFsmSub (STA_FSMSUB_DETACH_CSPS);
            break;

        default:

            /* 错误返回 */
            return VOS_FALSE;
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_MMA_IsDetachCnfEnable(VOS_VOID)
{    
    /* DETACHCNF状态输入条件检查异常*/
    if ((g_StatusContext.ulFsmState != STA_FSM_DE_ATTACH)
     && (g_StatusContext.ulFsmState != STA_FSM_ENABLE)
     && (g_StatusContext.ulFsmState != STA_FSM_SYS_CFG_SET))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_IsDetachCnfEnable ERROR! FSM STATE NOT OK!!!");

        /* 如果不是DE_ATTACH状态，ENABLE状态，返回错误 */
        return VOS_FALSE;
    }

    return VOS_TRUE;
}



VOS_UINT32 Sta_Detach (
    VOS_UINT32                          ulOpID,
    VOS_UINT32                          ulDetachType,
    TAF_MMA_DETACH_CAUSE_ENUM_UINT8     enDetachCause)
{

    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    VOS_UINT8                           ucCtxIndex;

    VOS_UINT8       ucSubNum;   /* 可用的子状态标号*/
    VOS_UINT32      ulRet;
    /* void        *pRet; */
    VOS_UINT32      ulTimerIndex;


    ucSubNum    = 0;

    if (VOS_TRUE != TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_DETACH_REQ, &ucCtxIndex))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "Sta_Detach ERROR! NOT FIND DETACH REQ");

        return STA_ERROR;
    }

    pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();

    /* 入口参数合法性检查 */
    if ( Sta_IsInvalidOpId (ulOpID) )
    {
        STA_TRACE(STA_WARNING_PRINT,"Sta_Detach():WARNING:parameter invalid");
        return STA_ERROR;
    }

    /* 检查DETACH请求类型合法性 */
    if ( ulDetachType > STA_PS_CS_DETACH )
    {

        /* 给APP发送DETACH CNF */
        TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                             TAF_MMA_APP_OPER_RESULT_FAILURE,
                             TAF_ERR_PARA_ERROR);

        TAF_MMA_ClearOperCtx(ucCtxIndex);

        /* 打印调试跟踪信息 */
        STA_TRACE (STA_WARNING_PRINT, "Sta_Detach():WARNING:parameter wrong");

        return STA_ERROR;
    }

    /* DETACH状态输入条件检查异常*/
    if (VOS_TRUE != TAF_MMA_IsEnableDeAttach())

    {

        /* 给APP发送DETACH CNF */
        TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                             TAF_MMA_APP_OPER_RESULT_FAILURE,
                             TAF_ERR_PHONE_MSG_UNMATCH);

        TAF_MMA_ClearOperCtx(ucCtxIndex);

        /* 输出跟踪调试信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_Detach():WARNING:system FSM state is wrong");

        return STA_ERROR;       /* 错误返回*/
    }

    /* 检查当前系统激活域状态 */
    /* 判断是否存在下述情况*/
    /* 同样的命令请求是否已经得到响应，一条命令请求不响应的话不允许*/
    /* 不允许进行下条同样的请求，PS和CS域上的ATTACH算不同的请求*/

    if ( VOS_FALSE == TAF_MMA_GetDetachFsmSubNum(ulDetachType, &ucSubNum) )
    {
        /* 输出跟踪调试信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_Detach():WARNING:parameters is wrong");


        /* 给APP发送DETACH CNF */
        TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                             TAF_MMA_APP_OPER_RESULT_FAILURE,
                             TAF_ERR_PARA_ERROR);

        TAF_MMA_ClearOperCtx(ucCtxIndex);

        return STA_ERROR;   /* 错误返回 */
    }

    /* 无效FSM状态处理 */
    if ( STA_FSMSUB_INDEX_NULL == ucSubNum )
    {

        /* 给APP发送DETACH CNF */
        TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                             TAF_MMA_APP_OPER_RESULT_FAILURE,
                             TAF_ERR_PHONE_MSG_UNMATCH);

        TAF_MMA_ClearOperCtx(ucCtxIndex);

        /* 输出跟踪调试信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_Detach():WARNING:system FSM state is wrong");

        return STA_ERROR;
    }

    ulRet = TAF_MMA_SndMmcDetachReq(ulOpID, ulDetachType, enDetachCause);

    if ( VOS_ERR == ulRet )
    {
        STA_TRACE (STA_ERROR_PRINT,
            "Sta_Detach():ERROR: failed");
        return STA_ERROR;       /* 错误返回 */
    }

    /* 记录当前的操作过程标识号 */
    g_StatusContext.ulCurOpId = ulOpID;

    /*modified by liuyang id:48197 date:2005-10-9 for V100R001 Begin*/
    /*更改主状态*/
    g_StatusContext.ulFsmState = STA_FSM_DE_ATTACH;
    /*modified by liuyang id:48197 date:2005-10-9 for V100R001 End*/

    /* 上报APP当前状态事件 */
    /* 预留的函数接口，暂不使用 */
    /*ulRet = Sta_EventReport (ulOpID, TAF_PH_EVT_DETACH);*/

    /* 设置DETACH子状态标志量，启动状态保护定时器 */
    if ( ulDetachType == STA_PS_DETACH )
    {/* PS域DETACH */
        g_StatusContext.aucFsmSubFlag[ucSubNum]
            = STA_FSMSUB_DETACH_PS;

        /*设置PS域DETACH标志;*/
        g_StatusContext.aFsmSub[ucSubNum].ucFsmStateSub
            = STA_FSMSUB_DETACH_PS;
        g_StatusContext.aFsmSub[ucSubNum].TimerName
            = STA_TIMER_DETACH_PS;

        /*启动PS域DETACH定时器;*/
        ulRet = NAS_StartRelTimer (
            &g_StatusContext.ahStaTimer[ucSubNum],
            WUEPS_PID_MMA,
            aStaTimerLen[STA_TIMER_DETACH_PS],
            TAF_MMA,
            MMA_MAKELPARAM(ucSubNum, STA_TIMER_DETACH_PS),
            VOS_RELTIMER_NOLOOP
             );
        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_Detach():ERROR:timer start failed");

            /* 恢复原来的子状态标志 */
            if ( STA_ERROR
                == Sta_UpdateFsmFlag(STA_FSMSUB_DETACH_PS,
                        0, STA_UPDATE_SINGL_CLEAR) )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_Detach():WARNING:timer clear failed");
            }
            return STA_ERROR;
        }
        /* 记录当前的操作过程标识号 */
        g_StatusContext.aFsmSub[ucSubNum].ulOpId = ulOpID;
        g_StatusContext.ucDetachReq++;

        /* BEGIN: Added by liuyang, id:48197, 2005/10/12 */
        g_StatusContext.aucStaPhoneOperator[ucSubNum] = TAF_PH_EVT_PS_DETACH_CNF;
        /* END:   Added by liuyang, id:48197, 2005/10/12 */

    }
    else if ( ulDetachType == STA_CS_DETACH )
    {/* CS域DETACH */
        g_StatusContext.aucFsmSubFlag[ucSubNum]
            = STA_FSMSUB_DETACH_CS;

        /* 设置CS域DETACH标志;*/
        g_StatusContext.aFsmSub[ucSubNum].ucFsmStateSub
            = STA_FSMSUB_DETACH_CS;
        g_StatusContext.aFsmSub[ucSubNum].TimerName
            = STA_TIMER_DETACH_CS;

        /*启动CS域ATTACH定时器;*/
        ulRet = NAS_StartRelTimer (
                    &g_StatusContext.ahStaTimer[ucSubNum],
                    WUEPS_PID_MMA,
                    aStaTimerLen[STA_TIMER_DETACH_CS],
                    TAF_MMA,
                    MMA_MAKELPARAM(ucSubNum, STA_TIMER_DETACH_CS),
                    VOS_RELTIMER_NOLOOP
                    );
        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_Detach():ERROR:timer start failed");

            /* 恢复原来的子状态标志 */
            if ( STA_ERROR
                == Sta_UpdateFsmFlag(STA_FSMSUB_DETACH_CS,
                        0, STA_UPDATE_SINGL_CLEAR) )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_Detach():WARNING:timer clear failed");
            }
            return STA_ERROR;
        }
        /* 记录当前的操作过程标识号 */
        g_StatusContext.aFsmSub[ucSubNum].ulOpId = ulOpID;
        g_StatusContext.ucDetachReq++;
        /* BEGIN: Added by liuyang, id:48197, 2005/10/12 */
        g_StatusContext.aucStaPhoneOperator[ucSubNum] = TAF_PH_EVT_CS_DETACH_CNF;
        /* END:   Added by liuyang, id:48197, 2005/10/12 */
    }
    else
    {/* CS&PS DETACH */
        ulTimerIndex = 0;
        /* PS域DETACH */
        g_StatusContext.aucFsmSubFlag[0]
            = STA_FSMSUB_DETACH_CSPS;

        /*设置PS域DETACH标志*/
        g_StatusContext.aFsmSub[0].ucFsmStateSub
            = STA_FSMSUB_DETACH_PS;
        g_StatusContext.aFsmSub[0].TimerName
            = STA_TIMER_DETACH_PS;

        /*启动PS域ATTACH定时器*/
        ulRet = NAS_StartRelTimer (
                    &g_StatusContext.ahStaTimer[0],
                    WUEPS_PID_MMA,
                    aStaTimerLen[STA_TIMER_DETACH_PS],
                    TAF_MMA,
                    MMA_MAKELPARAM(ulTimerIndex, STA_TIMER_DETACH_PS),
                    VOS_RELTIMER_NOLOOP
                    );
        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_Detach():ERROR:timer start failed");

            /* 恢复原来的子状态标志 */
            if ( STA_ERROR
                == Sta_UpdateFsmFlag(STA_FSMSUB_DETACH_PS,
                        0, STA_UPDATE_SINGL_CLEAR) )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_Detach():WARNING:timer clear failed");
            }
            return STA_ERROR;
        }
        /* 记录当前的操作过程标识号 */
        g_StatusContext.aFsmSub[0].ulOpId = ulOpID;
        g_StatusContext.ucDetachReq++;
        /* BEGIN: Added by liuyang, id:48197, 2005/10/12 */
        g_StatusContext.aucStaPhoneOperator[0] = TAF_PH_EVT_DETACH_CNF;
        /* END:   Added by liuyang, id:48197, 2005/10/12 */

        /* CS域DETACH */
    /*modified by liuyang id:48197 date:2005-9-29 for V100R001*/
    /*此时子状态应该是PS,CS都DETACH过程*/
       /* g_StatusContext.aucFsmSubFlag[1]
            = STA_FSMSUB_DETACH_CS;*/
        g_StatusContext.aucFsmSubFlag[1]
            = STA_FSMSUB_DETACH_CSPS;
        ulTimerIndex = 1;
        /* 设置CS域DETACH标志;*/
        g_StatusContext.aFsmSub[1].ucFsmStateSub
            = STA_FSMSUB_DETACH_CS;
        g_StatusContext.aFsmSub[1].TimerName
            = STA_TIMER_DETACH_CS;
    /*modified by liuyang id:48197 date:2005-9-29 for V100R001*/
        /*启动CS域ATTACH定时器;*/
        ulRet = NAS_StartRelTimer (
            &g_StatusContext.ahStaTimer[1],
            WUEPS_PID_MMA,
            aStaTimerLen[STA_TIMER_DETACH_CS],
            TAF_MMA,
            MMA_MAKELPARAM(ulTimerIndex, STA_TIMER_DETACH_CS),
            VOS_RELTIMER_NOLOOP
            );
        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_Detach():ERROR:timer start failed");

            /* 恢复原来的子状态标志 */
            /*modified by liuyang id:48197 date:2005-9-29 for V100R001*/
            if ( STA_ERROR
                == Sta_UpdateFsmFlag(STA_FSMSUB_DETACH_CS,0, STA_UPDATE_SINGL_CLEAR) )
            /*modified by liuyang id:48197 date:2005-9-29 for V100R001*/
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_Detach():WARNING:timer clear failed");
            }
            return STA_ERROR;
        }
        /* 记录当前的操作过程标识号 */
        g_StatusContext.aFsmSub[1].ulOpId = ulOpID;
        g_StatusContext.ucDetachReq++;
        /* BEGIN: Added by liuyang, id:48197, 2005/10/12 */
        g_StatusContext.aucStaPhoneOperator[1] = TAF_PH_EVT_DETACH_CNF;
        /* END:   Added by liuyang, id:48197, 2005/10/12 */
    }

    /* 用户下发AT^CGCATT=1,3,at返回ERROR,此时结果会被保存，在detach时候清空该信息 */
    g_StatusContext.ucOperateType = STA_OP_NULL;

    /*
    清空当前的PLMNid
    modified by liuyang id:48197 date:2005-9-5 for V200R001
    */
    /* Sta_ClearCurPlmnId( &(g_StatusContext.CurPlmnId) ); */
    return STA_SUCCESS;    /* 正常返回 */
}


VOS_VOID MN_PH_UpdateBeginRegTime( VOS_VOID  )
{
    VOS_UINT8                           ucSdcSimStatus;
    VOS_UINT8                           ucIsNeedPin;

    ucSdcSimStatus          = TAF_SDC_GetSimStatus();

    /* 卡状态为需要PIN或PUK时，认为需要PIN码 */
    if ( (TAF_SDC_USIM_STATUS_SIM_PIN   == ucSdcSimStatus)
      || (TAF_SDC_USIM_STATUS_SIM_PUK   == ucSdcSimStatus) )
    {
        ucIsNeedPin = VOS_TRUE;
    }
    else
    {
        ucIsNeedPin = VOS_FALSE;
    }
    
    if ( ((TAF_MMA_FSM_PHONE_MODE        == TAF_MMA_GetCurrFsmId())
       || (MN_PH_REG_TIME_STATE_PINVALID == g_stRegTimeInfo.enRegTimeState))
      && (VOS_FALSE == ucIsNeedPin) )    
    {
        g_stRegTimeInfo.ulBeginTick = OM_GetSlice();
        MN_INFO_LOG1("MN_PH_UpdateBeginRegTime:first time",(VOS_INT16)g_stRegTimeInfo.ulBeginTick);
    }
    
    if (VOS_FALSE == ucIsNeedPin)
    {
        MN_INFO_LOG1("MN_PH_UpdateBeginRegTime:second time,no need pin",(VOS_INT16)g_stRegTimeInfo.ulBeginTick);
        g_stRegTimeInfo.enRegTimeState = MN_PH_REG_TIME_STATE_BEGIN;
        g_stRegTimeInfo.ulCostTime     = 0;
    }
    else
    {
        MN_INFO_LOG("MN_PH_UpdateBeginRegTime:need pin");
        g_stRegTimeInfo.enRegTimeState = MN_PH_REG_TIME_STATE_PINVALID;
    }
}
VOS_VOID  MN_PH_UpdateEndRegTime( VOS_VOID )
{
    if ( MN_PH_REG_TIME_STATE_BEGIN != g_stRegTimeInfo.enRegTimeState )
    {
        return;
    }

    g_stRegTimeInfo.ulEndTick = OM_GetSlice();
    MN_INFO_LOG2("MN_PH_UpdateEndRegTime beginTick = %0x,endtick = %0x\r\n",
                 (VOS_INT16)g_stRegTimeInfo.ulBeginTick,(VOS_INT16)g_stRegTimeInfo.ulEndTick);

    /* 时间戳是由0开始递增，所以当前的时间戳应该大于上次。另在时间戳
       到0xFFFFFFFF时会重置到0，所以else分支处理这种情况 */
    /* MMA报给AT时，按slice上报，(32 * 1024)个slice是1S
       如果slice为0，表示没有注册成功；如果slice小于1S,AT按1S上报 */
    if (g_stRegTimeInfo.ulEndTick >= g_stRegTimeInfo.ulBeginTick)
    {
        g_stRegTimeInfo.ulCostTime
            = (g_stRegTimeInfo.ulEndTick - g_stRegTimeInfo.ulBeginTick);
    }
    else
    {
        g_stRegTimeInfo.ulCostTime
            = (0xFFFFFFFF - g_stRegTimeInfo.ulBeginTick + g_stRegTimeInfo.ulEndTick);
    }

    MN_INFO_LOG1("Into MN_PH_UpdateEndRegTime ulCostTime = %d\r\n",(VOS_INT16)g_stRegTimeInfo.ulCostTime);
    g_stRegTimeInfo.enRegTimeState  = MN_PH_REG_TIME_STATE_END;
    g_stRegTimeInfo.ulEndTick       = 0;

}





VOS_UINT32 Sta_PlmnList ( VOS_UINT32 ulOpID )
{
    VOS_UINT32          ulRet;
    VOS_UINT16          ClientId;
    VOS_UINT8           AppOpId;
    VOS_UINT8           ucPhoneEvent;

    /* 入口参数合法性检查 */
    if ( Sta_IsInvalidOpId (ulOpID) )
    {
        STA_TRACE(STA_WARNING_PRINT, "Sta_PlmnList():WARNING:parameter invalid  ");
        return STA_ERROR;
    }

    /* PLMN_LIST状态输入条件检查异常*/
    /* 只有在ENABLE状态，或者REFRESH触发的自动搜网，才能下发list请求，否则返回错误*/
    if (VOS_TRUE != TAF_MMA_IsEnablePlmnList())
    {
        g_StatusContext.PhoneError    = TAF_ERR_PHONE_MSG_UNMATCH;
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

        if (ulOpID <= TAF_MAX_STATUS_TI)
        {
            /*通过Ti获取ClientId,CallId*/
            if (TAF_SUCCESS != MMA_GetIdByTi(TAF_MMA, (VOS_UINT8)ulOpID, &ClientId, &AppOpId, &ucPhoneEvent))
            {
                MMA_WARNINGLOG("Taf_PhoneEvent():WARNING:Invoke Taf_GetIdByTi failed");
                return STA_ERROR;
            }
        }
        else
        {   /*是STATUS主动发起的事件*/
            AppOpId = MMA_OP_ID_INTERNAL;
            /*ClientId置为广播值:高字节的低4bit类型是MUX_PORT_BUTT*/
            ClientId = MMA_CLIENTID_BROADCAST;
        }

        /* 上报APP当前状态事件 */
        MMA_HandleEventError(ClientId, AppOpId, TAF_ERR_PHONE_MSG_UNMATCH, TAF_PH_EVT_ERR);

        /* 输出跟踪调试信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_PlmnList():WARNING:system FSM state is wrong\n");

        return STA_ERROR;       /* 错误返回 */
    }


    ulRet = TAF_MMA_SndMmcPlmnListReq();

    if ( VOS_ERR == ulRet )
    {
        STA_TRACE (STA_ERROR_PRINT,
            "Sta_PlmnList():ERROR: failed");
        return STA_ERROR;       /* 错误返回 */
    }
    /* 记录当前的操作过程标识号 */
    g_StatusContext.ulCurOpId = ulOpID;

    /* 设置PLMN_LIST状态标志量 */
    g_StatusContext.ulFsmState = STA_FSM_PLMN_LIST;

    /* 启动PLMN_LIST状态定时器 */
    g_StatusContext.aucFsmSubFlag[0]         = STA_FSMSUB_MONO;
    g_StatusContext.aFsmSub[0].ucFsmStateSub = STA_FSMSUB_MONO;
    g_StatusContext.aFsmSub[0].TimerName     = STA_TIMER_PLMN_LIST;
    ulRet = NAS_StartRelTimer (
            &g_StatusContext.ahStaTimer[0],
            WUEPS_PID_MMA,
            aStaTimerLen[STA_TIMER_PLMN_LIST],
            TAF_MMA,
            MMA_MAKELPARAM(0, STA_TIMER_PLMN_LIST),
            VOS_RELTIMER_NOLOOP
            );
    if ( VOS_OK != ulRet )
    {
        /* 恢复原来的子状态标志 */
        if ( STA_ERROR
            == Sta_UpdateFsmFlag(STA_FSMSUB_NULL,
                STA_FSM_NULL, STA_UPDATE_MONO_CLEAR) )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_PlmnList():ERROR:timer clear failed\n");
        }
        return STA_ERROR;
    }

    /* 更新子状态的操作过程标识号 */
    g_StatusContext.aFsmSub[0].ulOpId = ulOpID;

    return STA_SUCCESS;         /* 正常返回 */
}


VOS_UINT32 Sta_PlmnReselectAuto ( VOS_UINT32 ulOpID )
{
    VOS_UINT32                            ulRet;
    TAF_MMA_AUTO_PLMN_SEL_USER_ENUM_UINT8 enAutoPlmnSelUser;

    enAutoPlmnSelUser = TAF_MMA_GetAutoPlmnSelUser();

    /* 入口参数合法性检查 */
    if ( Sta_IsInvalidOpId (ulOpID) )
    {
        STA_TRACE(STA_WARNING_PRINT,
            "Sta_PlmnReselectAuto():WARNING:parameter invalid");
        return STA_ERROR;
    }

    /* svlte nv开启允许关机状态设置cops */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        return TAF_MMA_PlmnReselectAutoReq_PowerOff(ulOpID);
    }

    /* PLMN_RESEL状态输入条件检查异常 */
    /* 当前正在进行6F15文件触发的自动搜网 */
    if ((STA_FSM_PLMN_RESEL == g_StatusContext.ulFsmState)
     && (TAF_MMA_AUTO_PLMN_SEL_USER_USIM_REFRESH == enAutoPlmnSelUser))
    {
        /* 记录为AT发起的自动搜网，收到CNF之后需要给用户回复结果 */
        TAF_MMA_SetAutoPlmnSelUser(TAF_MMA_AUTO_PLMN_SEL_USER_AT);

        /* 记录当前的操作过程标识号 */
        g_StatusContext.ulCurOpId = ulOpID;

        return STA_SUCCESS;
    }

    if ( !(g_StatusContext.ulFsmState & STA_FSM_ENABLE) )
    {/* 如果不是ENABLE状态，返回错误 */
        g_StatusContext.PhoneError    = TAF_ERR_PHONE_MSG_UNMATCH;
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;
        /* 上报APP当前状态事件 */
        Sta_EventReport (ulOpID, TAF_PH_EVT_PLMN_RESEL_CNF);

        /* 输出跟踪调试信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_PlmnReselectAuto():WARNING:system FSM state  is wrong ");

        return STA_ERROR;           /* 错误返回 */
    }

    ulRet = TAF_MMA_SndMmcPlmnUserReselReq(MMA_MMC_PLMN_SEL_MODE_AUTO);

    if ( VOS_ERR == ulRet )
    {
        STA_TRACE (STA_ERROR_PRINT,
            "Sta_PlmnReselectAuto():ERROR: failed");
        return STA_ERROR;           /* 错误返回 */
    }

    /* 记录AT发起的自动搜网请求 */
    TAF_MMA_SetAutoPlmnSelUser(TAF_MMA_AUTO_PLMN_SEL_USER_AT);

    /* 记录当前的操作过程标识号 */
    g_StatusContext.ulCurOpId = ulOpID;

    /* 记录PLMN重新类型 */
    g_StatusContext.ucReselMode = MMA_MMC_PLMN_SEL_MODE_AUTO;


    /* 设置PLMN_RESEL状态标志量 */
    g_StatusContext.ulFsmState = STA_FSM_PLMN_RESEL;

    STA_TRACE1(STA_NORMAL_PRINT, "@@@@Sta_PlmnReselectAuto:FsmState=",STA_FSM_PLMN_RESEL);
    /* 启动PLMN_RESEL状态定时器 */
    g_StatusContext.aucFsmSubFlag[0]         = STA_FSMSUB_PLMN_RESEL_AUTO;
    g_StatusContext.aFsmSub[0].ucFsmStateSub = STA_FSMSUB_PLMN_RESEL_AUTO;
    g_StatusContext.aFsmSub[0].TimerName     = STA_TIMER_PLMN_RESEL;
    ulRet = NAS_StartRelTimer (
            &g_StatusContext.ahStaTimer[0],
            WUEPS_PID_MMA,
            aStaTimerLen[STA_TIMER_PLMN_RESEL],
            TAF_MMA,
            MMA_MAKELPARAM(0, STA_TIMER_PLMN_RESEL),
            VOS_RELTIMER_NOLOOP
            );
    if ( VOS_OK != ulRet )
    {
        /* 恢复原来的子状态标志 */
        if ( STA_ERROR
            == Sta_UpdateFsmFlag(STA_FSMSUB_NULL,
                STA_FSM_NULL, STA_UPDATE_MONO_CLEAR) )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_PlmnReselectAuto():ERROR:timer clear failed");
        }
        return STA_ERROR;
    }
    /* 更新子状态的操作过程标识号 */
    g_StatusContext.aFsmSub[0].ulOpId = ulOpID;

    return STA_SUCCESS;         /* 正常返回 */
}
VOS_UINT32 Sta_PlmnReselectManual ( VOS_UINT32 ulOpID )
{
    VOS_UINT32                      ulRet;
    /* void                        *pRet; */

    /* 入口参数合法性检查 */
    if ( Sta_IsInvalidOpId (ulOpID) )
    {
        STA_TRACE(STA_WARNING_PRINT,
            "Sta_PlmnReselectManual():WARNING:parameter invalid");
        return STA_ERROR;
    }

    /* PLMN_RESEL状态输入条件检查异常*/
    if ( !(g_StatusContext.ulFsmState & STA_FSM_ENABLE) )
    {/* 如果不是ENABLE状态，返回错误 */
        g_StatusContext.PhoneError    = TAF_ERR_PHONE_MSG_UNMATCH;
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;
        /* 上报APP当前状态事件 */
        Sta_EventReport (ulOpID, TAF_PH_EVT_PLMN_RESEL_CNF);

        /* 输出跟踪调试信息 */
       STA_TRACE (STA_WARNING_PRINT,
            "Sta_PlmnReselectManual():WARNING:system FSM state is wrong\n ");

        return STA_ERROR;    /* 错误返回*/
    }

    ulRet = TAF_MMA_SndMmcPlmnUserReselReq(MMA_MMC_PLMN_SEL_MODE_MANUAL);

    if ( VOS_ERR == ulRet )
    {
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_PlmnReselectManual():WARNING: failed");
        return STA_ERROR;       /* 错误返回 */
    }
    /* 记录当前的操作过程标识号 */
    g_StatusContext.ulCurOpId = ulOpID;

    /* 记录PLMN重新类型 */
    g_StatusContext.ucReselMode = MMA_MMC_PLMN_SEL_MODE_MANUAL;

    /* 上报APP当前状态事件，手动PLMN搜索 */
    /*Sta_EventReport (ulOpID, TAF_PH_EVT_PLMN_RESEL);*/

    /* 设置PLMN_RESEL状态标志量 */
    g_StatusContext.ulFsmState = STA_FSM_PLMN_RESEL;

    STA_TRACE1(STA_NORMAL_PRINT, "@@@@Sta_PlmnReselectManual:FsmState=",STA_FSM_PLMN_RESEL);
    /* 启动PLMN_RESEL状态定时器 */
    g_StatusContext.aucFsmSubFlag[0]
        = STA_FSMSUB_PLMN_RESEL_MANUAL;
    g_StatusContext.aFsmSub[0].ucFsmStateSub
        = STA_FSMSUB_PLMN_RESEL_MANUAL;
    g_StatusContext.aFsmSub[0].TimerName
        = STA_TIMER_PLMN_RESEL;
    ulRet = NAS_StartRelTimer (
            &g_StatusContext.ahStaTimer[0],
            WUEPS_PID_MMA,
            aStaTimerLen[STA_TIMER_PLMN_RESEL],
            TAF_MMA,
            MMA_MAKELPARAM(0, STA_TIMER_PLMN_RESEL),
            VOS_RELTIMER_NOLOOP
            );
    if ( VOS_OK != ulRet )
    {
        /* 恢复原来的子状态标志 */
        if ( STA_ERROR
            == Sta_UpdateFsmFlag(STA_FSMSUB_NULL,
                STA_FSM_NULL, STA_UPDATE_MONO_CLEAR) )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_PlmnReselectManual():WARNING:timer clear failed");
        }
        return STA_ERROR;       /* 错误返回 */
    }
    /* 更新子状态的操作过程标识号 */
    g_StatusContext.aFsmSub[0].ulOpId = ulOpID;
    /*
    清空当前的PLMNid
    modified by liuyang id:48197 date:2005-9-5 for V200R001
    */
    /* Sta_ClearCurPlmnId( &(g_StatusContext.CurPlmnId) ); */
    return STA_SUCCESS;         /* 正常返回 */
}


VOS_UINT32 Sta_PlmnSelect ( VOS_UINT32 ulOpID, TAF_PLMN_ID_STRU PlmnId,
                        TAF_PH_RA_MODE AccessMode, VOS_UINT8 ucReselMode)
{
    VOS_UINT32                      ulRet;

    /* 入口参数合法性检查 */
    if ( Sta_IsInvalidOpId (ulOpID) )
    {
        STA_TRACE(STA_WARNING_PRINT,
            "Sta_PlmnSelect():WARNING:parameter invalid");
        return STA_ERROR;
    }
    
    /* svlte nv开启允许关机状态设置cops */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        return TAF_MMA_RcvPlmnSelectionReq_PowerOff(ulOpID, PlmnId, AccessMode, ucReselMode);
    }


    /* PLMN_SEL状态输入条件检查异常*/
    if (VOS_TRUE != TAF_MMA_IsEnablePlmnSelect())
    {
        /* 如果不是ENABLE状态，返回错误 */
        if (TAF_MMA_PLMN_MODE_ENUM_DISABLE == TAF_MMA_GetPlmnMode_Refresh())
        {
            g_StatusContext.PhoneError = TAF_ERR_NET_SEL_MENU_DISABLE;
        }
        else
        {
            g_StatusContext.PhoneError = TAF_ERR_PHONE_MSG_UNMATCH;
        }

        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;
        /* 上报APP当前状态事件 */
        Sta_EventReport (ulOpID, TAF_PH_EVT_PLMN_SEL_CNF);

        /* 输出跟踪调试信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_PlmnSelect():WARNING:system FSM state is wrong");

        return STA_ERROR;    /* 错误返回 */
    }

    /* USIM卡禁止GSM接入并且是G单模时，不允许用户指定搜
       SIM卡并且是L单模时，也不允许用户指定搜 */
    if (VOS_FALSE == TAF_MMA_IsExistPlmnRatAllowAccess())
    {
        g_StatusContext.PhoneError    = TAF_ERR_NO_NETWORK_SERVICE;
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

        /* 上报APP当前状态事件 */
        Sta_EventReport (ulOpID, TAF_PH_EVT_PLMN_SEL_CNF);

        /* 输出跟踪调试信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_PlmnSelect():WARNING:system FSM state is wrong");

        return STA_ERROR;
    }

    /* 记录输入参数 */
    g_StatusContext.ulCurOpId     = ulOpID;
    g_StatusContext.StaPlmnId.Mcc = PlmnId.Mcc;
    g_StatusContext.StaPlmnId.Mnc = PlmnId.Mnc;

    ulRet = TAF_MMA_SndMmcPlmnSpecialReq((MMA_MMC_PLMN_ID_STRU *)&PlmnId, AccessMode);

    if ( VOS_ERR == ulRet )
    {
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_PlmnSelect():WARNING: failed");
        return STA_ERROR;           /* 错误返回 */
    }
    /* 记录当前的操作过程标识号 */
    g_StatusContext.ulCurOpId = ulOpID;

    /* 上报APP当前状态事件 */
    /*Sta_EventReport (ulOpID, TAF_PH_EVT_PLMN_SEL);*/

    /* 设置PLMN_SEL状态标志量 */
    g_StatusContext.ulFsmState = STA_FSM_PLMN_SEL;

    STA_TRACE1(STA_NORMAL_PRINT, "@@@@Sta_PlmnSelect:FsmState=",STA_FSM_PLMN_SEL);
    /* 启动PLMN_SEL状态定时器 */
    g_StatusContext.aucFsmSubFlag[0]         = STA_FSMSUB_MONO;
    g_StatusContext.aFsmSub[0].ucFsmStateSub = STA_FSMSUB_MONO;
    g_StatusContext.aFsmSub[0].TimerName     = STA_TIMER_PLMN_SEL;

    ulRet = NAS_StartRelTimer (
            &g_StatusContext.ahStaTimer[0],
            WUEPS_PID_MMA,
            aStaTimerLen[STA_TIMER_PLMN_SEL],
            TAF_MMA,
            MMA_MAKELPARAM(0, STA_TIMER_PLMN_SEL),
            VOS_RELTIMER_NOLOOP
            );

    if ( VOS_OK != ulRet )
    {
        /* 恢复原来的子状态标志 */
        if (STA_ERROR
            == Sta_UpdateFsmFlag(STA_FSMSUB_NULL,
                STA_FSM_NULL, STA_UPDATE_MONO_CLEAR) )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_PlmnSelect():WARNING:timer clear failed\n");
        }
        return STA_ERROR;       /* 错误返回 */
    }
    /* 记录当前的操作过程标识号 */
    g_StatusContext.aFsmSub[0].ulOpId = ulOpID;
    /*
    清空当前的PLMNid
    modified by liuyang id:48197 date:2005-9-5 for V200R001
    */
    /* Sta_ClearCurPlmnId( &(g_StatusContext.CurPlmnId) ); */
    return STA_SUCCESS;         /* 正常返回 */
}


VOS_VOID  Sta_AttachTimeoutProc(REL_TIMER_MSG * pMsg)
{
    STA_TIMER_NAME_E    TimerId;    /* 定时器状态名称*/
    VOS_UINT16              TimerIndex; /* 定时器序号 */
    /* HTIMER              hTimer; */    /* 定时器变量*/
    VOS_UINT32          ulRet;
    VOS_UINT8           ucOperate;


    /*读取系统定时器消息,得到定时器ID和定时器索引*/
    TimerId     = (STA_TIMER_NAME_E)MMA_LOWORD(pMsg->ulPara);
    TimerIndex  = MMA_HIWORD(pMsg->ulPara);
    /* hTimer      = pMsg->hTm; */
    g_StatusContext.PhoneError    = TAF_ERR_TIME_OUT;
    g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

    switch ( TimerId )
    {
        case STA_TIMER_ATTACH_CS:
            ucOperate = TAF_PH_EVT_CS_ATTACH_CNF;
            g_StatusContext.ucOperateType |= STA_OP_CSSERVICESTATUS;
            break;
        case STA_TIMER_ATTACH_PS:
            ucOperate = TAF_PH_EVT_PS_ATTACH_CNF;
            g_StatusContext.ucOperateType |= STA_OP_PSSERVICESTATUS;
            break;
        default:
            MMA_WARNINGLOG("Sta_AttachTimeoutProc():WARNING:UNKNOW TIMERID!");
            return;
    }
    /*如果是发起两个attach过程，判断是否是同时两个域的attach过程，若不是，
      单独处理*/
    if (  STA_FSMSUB_ATTACH_CSPS == g_StatusContext.aucFsmSubFlag[TimerIndex])
    {
        ucOperate = TAF_PH_EVT_ATTACH_CNF;
        g_StatusContext.ucOperateType |= (STA_OP_CSSERVICESTATUS
                                         |STA_OP_PSSERVICESTATUS);
    }

   g_StatusContext.ucAttachReq--;

   /* 上报APP当前状态事件 */

   /*判断这次ATTACH是否不是启动过程*/
   if (( STA_OPID_ATTATCH != g_StatusContext.aFsmSub[TimerIndex].ulOpId )
       && (0 == g_StatusContext.ucAttachReq))
   {
        if ( MMA_SET_MSCLASSTYPE_NO ==
                gstMmaValue.stSetMsClass.enSetMsClassTypeflag)
        {
              Sta_EventReport (g_StatusContext.ulCurOpId, ucOperate);
        }
        else if(MMA_SET_MSCLASSTYPE_YES ==
                    gstMmaValue.stSetMsClass.enSetMsClassTypeflag)
        {
          /*若是设置参数过程，判断是否完成所有的ATTACH请求，若是，同步全局
            变量，报告设置参数成功*/
            Sta_SyncMsClassType(g_StatusContext.aFsmSub[TimerIndex].ulOpId ,
                                gstMmaValue.stSetMsClass.NewMsClassType);
        }
        else
        {
            STA_TRACE (STA_WARNING_PRINT,
                       "Sta_AttachTimeoutProc():WARNING:SET MSClass Type Flag Error failed");
        }
   }
   else
   {
       STA_TRACE(STA_NORMAL_PRINT, "Sta_AttachTimeoutProc():NORMAL:Internal Attach TimeOut");
   }
   /*清除当前状态标志，回到事件请求状态*/

   /*  清除子状态描述信息 */
   /* BEGIN: Modified by liuyang, 2005/10/22 */
   /*此时应将OpId置为无效值*/
   /* g_StatusContext.aFsmSub[TimerIndex].ulOpId = 0; */
   g_StatusContext.aFsmSub[TimerIndex].ulOpId = STA_OPID_INVALID;
   /* END:   Modified by liuyang, 2005/10/22 */
   g_StatusContext.aFsmSub[TimerIndex].ucFsmStateSub
                          = STA_FSMSUB_NULL;
   g_StatusContext.aFsmSub[TimerIndex].TimerName
                          = STA_TIMER_NULL;
   g_StatusContext.aucFsmSubFlag[TimerIndex]
                          = STA_FSMSUB_NULL;
   /* 如果不存在其他域的DE_ATTACH，回到ENABLE态*/
   if (g_StatusContext.ucAttachReq <= 0)
   {
        ulRet = Sta_UpdateFsmFlag ( STA_FSMSUB_NULL,
                                    STA_FSM_ENABLE,
                                    STA_UPDATE_DUAL_CLEAR );
        if (STA_ERROR == ulRet)
        {
            STA_TRACE(STA_WARNING_PRINT, "Sta_AttachTimeoutProc():WARNING:Sta_UpdateFsmFlag failed\n");
        }
        g_StatusContext.ucAttachReq = 0;
        Sta_EventReport (g_StatusContext.ulCurOpId, ucOperate);
   }
   else
   {
       g_StatusContext.ulFsmState = STA_FSM_DE_ATTACH;
   }
   return;
}
VOS_VOID  Sta_DetachTimeoutProc(REL_TIMER_MSG * pMsg)
{
    STA_TIMER_NAME_E    TimerId;    /* 定时器状态名称*/
    VOS_UINT16              TimerIndex; /* 定时器序号 */
    VOS_UINT8           ucOperate;
    VOS_UINT32          ulRet;

    /*读取系统定时器消息,得到定时器ID和定时器索引*/
    TimerId     = (STA_TIMER_NAME_E)MMA_LOWORD(pMsg->ulPara);
    TimerIndex  = MMA_HIWORD(pMsg->ulPara);
    /* hTimer      = pMsg->hTm; */


    g_StatusContext.PhoneError    = TAF_ERR_TIME_OUT;
    g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

    switch ( TimerId )
    {
        case STA_TIMER_DETACH_CS:
            ucOperate = TAF_PH_EVT_CS_DETACH_CNF;
            g_StatusContext.ucOperateType |= STA_OP_CSSERVICESTATUS;
            break;
        case STA_TIMER_DETACH_PS:
            ucOperate = TAF_PH_EVT_PS_DETACH_CNF;
            g_StatusContext.ucOperateType |= STA_OP_PSSERVICESTATUS;
            break;
        default:
            MMA_WARNINGLOG("Sta_DetachTimeoutProc():WARNING:UNKNOW TIMERID!");
            return;
    }
    /*判断是否此次是两个过程同时DETACH*/
    if (  STA_FSMSUB_DETACH_CSPS == g_StatusContext.aucFsmSubFlag[TimerIndex])
    {
        ucOperate = TAF_PH_EVT_DETACH_CNF;
        g_StatusContext.ucOperateType |= (STA_OP_CSSERVICESTATUS
                                         |STA_OP_PSSERVICESTATUS);
    }

    g_StatusContext.ucDetachReq--;


    /* 上报APP当前状态事件 */
    /* 预留的函数接口，暂不使用 */
    /*ulRet = Sta_EventReport (g_StatusContext.ulCurOpId,
        TAF_PH_EVT_DETACH_CNF);
    if ( STA_ERROR == ulRet )
    {
        return STA_ERROR;
    }*/
    /*判断此次DETACH发起的来源*/
    if (STA_OPID_INSIDE == g_StatusContext.aFsmSub[TimerIndex].ulOpId)
    {
       /*清除当前状态标志，回到事件请求状态*/
       /*  清除子状态描述信息 */
       /* BEGIN: Modified by liuyang, 2005/10/22 */
       /*此时应将OpId置为无效值*/
       /* g_StatusContext.aFsmSub[TimerIndex].ulOpId = 0; */
       g_StatusContext.aFsmSub[TimerIndex].ulOpId = STA_OPID_INVALID;
       /* END:   Modified by liuyang, 2005/10/22 */
       g_StatusContext.aFsmSub[TimerIndex].ucFsmStateSub
           = STA_FSMSUB_NULL;
       g_StatusContext.aFsmSub[TimerIndex].TimerName
           = STA_TIMER_NULL;
       g_StatusContext.aucFsmSubFlag[TimerIndex]
           = STA_FSMSUB_NULL;
       /* 如果不存在其他域的DE_ATTACH，回到ENABLE态*/
       if (g_StatusContext.ucDetachReq <= 0)
       {
           ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL,
                                     STA_FSM_ENABLE,
                                     STA_UPDATE_DUAL_CLEAR);
           if (STA_ERROR == ulRet)
           {
               STA_TRACE(STA_WARNING_PRINT, "Sta_DetachTimeoutProc():WARNING:Sta_UpdateFsmFlag failed\n");
           }
           g_StatusContext.ucDetachReq = 0;
       }
       else
       {
           g_StatusContext.ulFsmState = STA_FSM_DE_ATTACH;
       }
       return;
    }
    /*是APP或AT发起的直接DETACH命令*/
    if( MMA_SET_MSCLASSTYPE_NO ==
         gstMmaValue.stSetMsClass.enSetMsClassTypeflag )
    {
        Sta_DetachTimeoutProc4App(TimerId,TimerIndex, ucOperate);
    }
    else if(MMA_SET_MSCLASSTYPE_YES ==
               gstMmaValue.stSetMsClass.enSetMsClassTypeflag )
    {
        Sta_DetachTimeoutProc4DefMsClassType(TimerId, TimerIndex);
    }
    else
    {
        ;
    }
    return;
}
VOS_VOID Sta_DetachTimeoutProc4App(STA_TIMER_NAME_E    TimerId,
                                   VOS_UINT16          TimerIndex,
                                   VOS_UINT8           ucOperate)
{
    VOS_UINT32      ulRet;

    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    VOS_UINT8                           ucCtxIndex;

     /*DETACH完成*/
    if( 0 ==  g_StatusContext.ucDetachReq )
    {
        if (VOS_TRUE == TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_DETACH_REQ, &ucCtxIndex))
        {
            pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();

            TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                                 TAF_MMA_APP_OPER_RESULT_FAILURE,
                                 TAF_ERR_ERROR);

            TAF_MMA_ClearOperCtx(ucCtxIndex);
        }

        ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL,
                                 STA_FSM_ENABLE,
                                 STA_UPDATE_DUAL_CLEAR);
       if (STA_ERROR == ulRet)
       {
           STA_TRACE(STA_WARNING_PRINT, "Sta_DetachTimeoutProc4App():WARNING:Sta_UpdateFsmFlag failed\n");
       }
       g_StatusContext.ucDetachReq = 0;
       return;
    }
    /*清除当前状态标志，回到事件请求状态*/
    /*  清除子状态描述信息 */
    /* BEGIN: Modified by liuyang, 2005/10/22 */
    /*此时应将OpId置为无效值*/
    /* g_StatusContext.aFsmSub[TimerIndex].ulOpId = 0; */
    g_StatusContext.aFsmSub[TimerIndex].ulOpId          = STA_OPID_INVALID;
    /* END:   Modified by liuyang, 2005/10/22 */
    g_StatusContext.aFsmSub[TimerIndex].ucFsmStateSub   = STA_FSMSUB_NULL;
    g_StatusContext.aFsmSub[TimerIndex].TimerName       = STA_TIMER_NULL;
    g_StatusContext.aucFsmSubFlag[TimerIndex]           = STA_FSMSUB_NULL;

    g_StatusContext.ulFsmState = STA_FSM_DE_ATTACH;
    return;
}
VOS_VOID  Sta_DetachTimeoutProc4DefMsClassType( STA_TIMER_NAME_E    TimerId,
                                                VOS_UINT16          TimerIndex)
{
    VOS_UINT32      ulRet;

    if (TimerIndex > STA_TIMER_MAX -1)
    {
        return ;
    }

    /*若当前发起了两个DETACH过程，则要关闭另一个DETACH定时器，
    同时复位状态*/
    if ( 0 < g_StatusContext.ucDetachReq )
    {
         /*释放另一个定时器的资源*/

        ulRet = NAS_StopRelTimer(WUEPS_PID_MMA, g_StatusContext.aFsmSub[1 - TimerIndex].TimerName,
                                 &g_StatusContext.ahStaTimer[1 - TimerIndex]);

        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_WARNING_PRINT,
                    "Sta_DetachTimeoutProc4DefMsClassType(): WARNING:\
                    NAS_StopRelTimer failed");
        }
        g_StatusContext.ahStaTimer[1 - TimerIndex] = 0;
    }

    /*发送MODE_CHANGE_REQ至MMC，控制其状态回滚*/
    /* gstMmaValue.stSetMsClass.enSetMsClassTypeflag现在没有置成YES的地方，
        所以Sta_DetachTimeoutProc4DefMsClassType现在已经走不到了,不需要通知STK/MSG CS域能力变更 */
    TAF_MMA_SndMmcModeChangeReq((MMA_MMC_MS_MODE_ENUM_UINT32)gstMmaValue.stSetMsClass.MsClassType);

    /*同步手机状态为为更改前状态*/
    Sta_SyncMsClassType(g_StatusContext.ulCurOpId, gstMmaValue.stSetMsClass.MsClassType);

    /*清空当前STATUS模块子状态*/
    ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL,
                              STA_FSM_ENABLE,
                              STA_UPDATE_DUAL_CLEAR);
    if (STA_ERROR == ulRet)
    {
        STA_TRACE(STA_WARNING_PRINT, "Sta_DetachTimeoutProc4DefMsClassType():WARNING:Sta_UpdateFsmFlag failed\n");
    }

    /* 清除DETACH请求 */
    g_StatusContext.ucDetachReq = 0;
    return;
}



VOS_UINT32 Sta_TimeoutProc (REL_TIMER_MSG * pMsg)
{
    STA_TIMER_NAME_E                    TimerId;    /* 定时器状态名称*/
    VOS_UINT16                          TimerIndex; /* 定时器序号 */
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulOpId;

    ulOpId  = STA_OPID_INVALID;

    /* 参数检查 */
    if ( VOS_NULL_PTR == pMsg )
    {
        STA_TRACE (STA_ERROR_PRINT,
            "Sta_TimeoutProc():ERROR:parameter invaild null PTR");
        return STA_ERROR;
    }

    /*读取系统定时器消息,得到定时器ID和定时器索引*/
    TimerId     = (STA_TIMER_NAME_E)MMA_LOWORD(pMsg->ulPara);
    TimerIndex  = MMA_HIWORD(pMsg->ulPara);
    STA_TRACE1 (STA_WARNING_PRINT,
            "Sta_TimeoutProc():WARNING:Status Time Out, TimerId is ",TimerId);

    if (TimerIndex >= STA_TIMER_MAX)
    {
        STA_TRACE (STA_ERROR_PRINT,
                "Sta_TimeoutProc():TimerIndex over run.");
        return STA_ERROR;
    }

    /* 系统定时器消息分发处理*/
    switch ( TimerId )
    {


        case STA_TIMER_ATTACH_CS :
        case STA_TIMER_ATTACH_PS :
            Sta_AttachTimeoutProc(pMsg);
            break;

        case STA_TIMER_DETACH_CS:
        case STA_TIMER_DETACH_PS:
            Sta_DetachTimeoutProc(pMsg);
            break;

        case STA_TIMER_PLMN_LIST :


            /* 暂时不向AT回复结果，收到ID_MMC_MMA_PLMN_LIST_ABORT_CNF 后向
               AT回复*/

            /*清除当前状态标志，回到ENABLE状态*/
            /*  清除子状态描述信息 */
            /* BEGIN: Modified by liuyang, 2005/10/22 */
            /*此时应将OpId置为无效值*/
            ulOpId                                     = g_StatusContext.aFsmSub[TimerIndex].ulOpId;
            g_StatusContext.aFsmSub[TimerIndex].ulOpId = STA_OPID_INVALID;
            /* END:   Modified by liuyang, 2005/10/22 */

            g_StatusContext.aFsmSub[TimerIndex].ucFsmStateSub
                = STA_FSMSUB_NULL;
            g_StatusContext.aFsmSub[TimerIndex].TimerName
                = STA_TIMER_NULL;
            g_StatusContext.aucFsmSubFlag[TimerIndex]
                = STA_FSMSUB_NULL;

            /* 调用处理函数 */
            TAF_MMA_RcvTimerPlmnListCnfExpired(ulOpId);

            break;

        case STA_TIMER_PLMN_SEL :
            ulOpId                                            = g_StatusContext.aFsmSub[TimerIndex].ulOpId;
            g_StatusContext.aFsmSub[TimerIndex].ulOpId        = STA_OPID_INVALID;
            g_StatusContext.aFsmSub[TimerIndex].ucFsmStateSub = STA_FSMSUB_NULL;
            g_StatusContext.aFsmSub[TimerIndex].TimerName     = STA_TIMER_NULL;
            g_StatusContext.aucFsmSubFlag[TimerIndex]         = STA_FSMSUB_NULL;

            /* MMA的指定搜网保护定时器超时则主动向MMC发送停止指定搜网请求
               收到MMC的回复后再向AT回复 */
            ulRet = TAF_MMA_SndMmcSpecPlmnSearchAbortReq();

            if (VOS_ERR == ulRet)
            {
                STA_TRACE(STA_ERROR_PRINT, "Sta_TimeoutProc():ERROR: failed");
            }

            /* 启动STA_TIMER_SPEC_PLMN_ABORT 10s定时器，如果超时未收到MMC的回复则上报AT失败 */
            g_StatusContext.aucFsmSubFlag[0]         = STA_FSMSUB_MONO;
            g_StatusContext.aFsmSub[0].ucFsmStateSub = STA_FSMSUB_MONO;
            g_StatusContext.aFsmSub[0].TimerName     = STA_TIMER_SPEC_PLMN_ABORT;
            g_StatusContext.aFsmSub[0].ulOpId        = ulOpId;

            if (VOS_OK != NAS_StartRelTimer(&g_StatusContext.ahStaTimer[0],
                                            WUEPS_PID_MMA,
                                            aStaTimerLen[STA_TIMER_SPEC_PLMN_ABORT],
                                            TAF_MMA,
                                            MMA_MAKELPARAM(0, STA_TIMER_SPEC_PLMN_ABORT),
                                            VOS_RELTIMER_NOLOOP))
            {
                STA_TRACE(STA_ERROR_PRINT, "Sta_TimeoutProc():ERROR: VOS_StartRelTimer runs failed");
            }

            break;

        case STA_TIMER_PLMN_RESEL:
            g_StatusContext.PhoneError    = TAF_ERR_TIME_OUT;
            g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

            /* 6F15文件refresh触发的自动搜网，不需要上报APP */
            if (TAF_MMA_AUTO_PLMN_SEL_USER_AT == TAF_MMA_GetAutoPlmnSelUser())
            {
                /* 上报APP当前状态事件 */
                Sta_EventReport (g_StatusContext.ulCurOpId,
                    TAF_PH_EVT_PLMN_RESEL_CNF);
            }

            /* 清除当前状态标志，回到ENABLE状态 */
            /*  清除子状态描述信息 */
            /*此时应将OpId置为无效值*/
            g_StatusContext.aFsmSub[TimerIndex].ulOpId = STA_OPID_INVALID;
            g_StatusContext.aFsmSub[TimerIndex].ucFsmStateSub
                = STA_FSMSUB_NULL;
            g_StatusContext.aFsmSub[TimerIndex].TimerName
                = STA_TIMER_NULL;
            g_StatusContext.aucFsmSubFlag[TimerIndex]
                = STA_FSMSUB_NULL;
            g_StatusContext.ulFsmState = STA_FSM_ENABLE;
            break;

        case STA_TIMER_SPEC_PLMN_ABORT:
            g_StatusContext.aFsmSub[TimerIndex].ulOpId        = STA_OPID_INVALID;
            g_StatusContext.aFsmSub[TimerIndex].ucFsmStateSub = STA_FSMSUB_NULL;
            g_StatusContext.aFsmSub[TimerIndex].TimerName     = STA_TIMER_NULL;
            g_StatusContext.aucFsmSubFlag[TimerIndex]         = STA_FSMSUB_NULL;

            /* 向AT回复指定搜超时结果 */
            g_StatusContext.PhoneError    = TAF_ERR_TIME_OUT;
            g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

            /* 上报APP当前状态事件 */
            Sta_EventReport (g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_SEL_CNF);

            /* 迁移状态到ENABLE状态 */
            g_StatusContext.ulFsmState = STA_FSM_ENABLE;
            break;

        case STA_TIMER_PLMN_LIST_ABORT:

            TAF_MMA_RcvTimerPlmnListAbortCnfExpired(TimerIndex);

            break;

        default :
            /* do nothing */
            /* 输出跟踪调试信息 */
            STA_TRACE (STA_NORMAL_PRINT,
                "Sta_TimeoutProc():NORMAL:do nothing");
            return STA_ERROR;
    }

    return STA_SUCCESS;     /* 正常返回 */
}

/* 删除 Sta_SoftReset ( VOS_UINT8 ucResetMode )*/



VOS_UINT32 Sta_PlmnListAbort ( VOS_UINT32 ulOpID )
{
    VOS_UINT32                      ulRet;
    /* void                        *pRet; */
    /* ST_MMC_PLMN_LIST_ABORT_REQ  *pReq; */    /* 请求过程参数指针 */

    /* 入口参数合法性检查 */
    if ( Sta_IsInvalidOpId (ulOpID) )
    {
        STA_TRACE(STA_WARNING_PRINT, "Sta_PlmnListAbort():WARNING: parameter invalid");
        return STA_ERROR;
    }

    /* 正常状态PLMN_LIST下的响应 */
    if ( g_StatusContext.ulFsmState & STA_FSM_PLMN_LIST )
    {
        ulRet = TAF_MMA_SndMmcPlmnListAbortReq();

        if ( VOS_ERR == ulRet )
        {
            STA_TRACE (STA_ERROR_PRINT,
                "Sta_PlmnListAbort():ERROR: failed");
            return STA_ERROR;     /* 错误返回 */
        }

        /*清除子状态*/
        ulRet = Sta_UpdateFsmFlag (STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);
        if (STA_ERROR == ulRet)
        {
            STA_TRACE(STA_WARNING_PRINT, "Sta_PlmnListAbort():WARNING:Sta_UpdateFsmFlag failed\n");
        }

        /* 上报APP当前状态事件 */
        Sta_EventReport (ulOpID, TAF_PH_EVT_PLMN_LIST_ABORT_CNF);

        return STA_SUCCESS;       /* 正常返回 */
    }

    /* PLMN_LIST_ABORT 输入条件异常 */
    g_StatusContext.PhoneError    = TAF_ERR_PHONE_MSG_UNMATCH;
    g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

    /* 向APP报告错误事件 */
    Sta_EventReport (ulOpID, TAF_PH_EVT_PLMN_LIST_ABORT_CNF);

    /* 输出跟踪调试信息 */
    STA_TRACE (STA_WARNING_PRINT,
        "Sta_PlmnListAbort():WARNING:system FSM state is wrong");

    return STA_ERROR;            /* 错误返回 */
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*---------- 协议栈响应消息处理函数(内部使用) -----------*/
/*=======================================================*/



/* Sta_AttachCnf_SysCfgSet*/




VOS_UINT32 Sta_AttachCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    STA_PHONE_DATA_S                    Update; /* 需要更新的全局数据 */
    VOS_UINT8                           ucSub;          /* 子状态索引号 */
    VOS_UINT32                          ulOpId;         /* 命令操作数标识 */
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucFsmSubSta;    /*标示当前进行操作的域*/
    VOS_UINT8                           ucOperate;      /*用于记录引起当前-cnf的REQ*/

    MMC_MMA_ATTACH_CNF_STRU            *pAttachCnf;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    pAttachCnf = (MMC_MMA_ATTACH_CNF_STRU*)pstMsg;

    ulOpId  =   pAttachCnf->ulOpID;

    /* ATTACHCNF状态输入条件检查异常*/
    if ((g_StatusContext.ulFsmState != STA_FSM_DE_ATTACH)
        && (g_StatusContext.ulFsmState != STA_FSM_ENABLE)
        && (g_StatusContext.ulFsmState != STA_FSM_PLMN_SEL)
        && (g_StatusContext.ulFsmState != STA_FSM_SYS_CFG_SET)
       )
    {
        /* 如果不是DE_ATTACH状态，ENABLE状态，返回错误 */
        /* 打印调试跟踪信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_AttachCnf ():WARNING: system FSM state is wrong");
        return VOS_TRUE;     /* 错误返回*/
    }/* ATTACHCNF状态输入条件检查异常*/

    

    /*增加在ENABLE状态下ATTACH_CNF的接受*/
    if ( STA_FSM_DE_ATTACH == g_StatusContext.ulFsmState)
    {
        /*使用当前操作返回域确定上次操作进行的索引值*/
        if (  MMA_MMC_SRVDOMAIN_CS == pAttachCnf->enCnDomainId )
        {
            ucFsmSubSta = STA_FSMSUB_ATTACH_CS;
        }
        else if (MMA_MMC_SRVDOMAIN_PS == pAttachCnf->enCnDomainId )
        {
            ucFsmSubSta = STA_FSMSUB_ATTACH_PS;
        }
        else
        {
            ucFsmSubSta = STA_FSMSUB_NULL;
        }
        ucSub   = (VOS_UINT8)Sta_FindFsmSubBySubSta (ucFsmSubSta);

        if ( STA_FSMSUB_INDEX_NULL == ucSub )
        {
            /* 输出跟踪调试信息 */
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_AttachCnf():WARNING:wrong message parameter");

            return VOS_TRUE;   /* 错误返回 */
        }

        /* 关闭ATTACH定时器 */
        ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                 g_StatusContext.aFsmSub[ucSub].TimerName,
                                 &g_StatusContext.ahStaTimer[ucSub]);

        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_AttachCnf():WARNING:NAS_StopRelTimer failed");
        }
        g_StatusContext.ahStaTimer[ucSub] = 0;


        /* 清除请求过程个数 */
        if ((MMA_MMC_SRVDOMAIN_CS == pAttachCnf->enCnDomainId)
         || (MMA_MMC_SRVDOMAIN_PS == pAttachCnf->enCnDomainId))
        {
            g_StatusContext.ucAttachReq--;
        }
        else
        {
            g_StatusContext.ucAttachReq = 0;
        }



        /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */
        
        /* 修改：Attach CS+PS时，CS和PS域有一个域正常服务，就认为ATTACH成功，
           只有两个域都不能正常服务时，才给AT上报ERROR */
        if (MMA_MMC_SRVDOMAIN_CS == pAttachCnf->enCnDomainId)
        {
            /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */
            if (MMA_MMC_SERVICE_STATUS_NO_CHANGE != pAttachCnf->enServiceStatus)
            {
                enCsServiceStatus = (VOS_UINT8)pAttachCnf->enServiceStatus;
            }
            g_StatusContext.ucOperateType     = STA_OP_CSSERVICESTATUS;
        }
        if (MMA_MMC_SRVDOMAIN_PS == pAttachCnf->enCnDomainId)
        {
            /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */

            if (MMA_MMC_SERVICE_STATUS_NO_CHANGE != pAttachCnf->enServiceStatus)
            {
                enPsServiceStatus = (VOS_UINT8)pAttachCnf->enServiceStatus;
            }

            g_StatusContext.ucOperateType     = STA_OP_PSSERVICESTATUS;
        }

        /*上报ATTACH结果*/
        if ( STA_FSMSUB_ATTACH_CS == g_StatusContext.aucFsmSubFlag[ucSub] )
        {
            ucOperate = TAF_PH_EVT_CS_ATTACH_CNF;

            TAF_SDC_SetCsAttachAllowFlg(VOS_TRUE);

            /* 增加判断CS ATTACH结果的原因值,如果不是正常服务上报error */
            if (MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE != enCsServiceStatus)
            {
                g_StatusContext.ucOperateType |= STA_OP_PHONEERROR;
                g_StatusContext.PhoneError = TAF_ERR_PHONE_ATTACH_FORBIDDEN;
            }

        }
        else if ( STA_FSMSUB_ATTACH_PS == g_StatusContext.aucFsmSubFlag[ucSub] )
        {
            ucOperate = TAF_PH_EVT_PS_ATTACH_CNF;

            TAF_SDC_SetPsAttachAllowFlg(VOS_TRUE);
            
            /* 增加判断PS ATTACH结果的原因值,如果不是正常服务上报error */
            if (MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE != enPsServiceStatus)
            {
                g_StatusContext.ucOperateType |= STA_OP_PHONEERROR;
                g_StatusContext.PhoneError     = TAF_ERR_PHONE_ATTACH_FORBIDDEN;
            }
        }

        else if ( STA_FSMSUB_ATTACH_CSPS == g_StatusContext.aucFsmSubFlag[ucSub])
        {
            ucOperate = TAF_PH_EVT_ATTACH_CNF;

            TAF_SDC_SetCsAttachAllowFlg(VOS_TRUE);
            TAF_SDC_SetPsAttachAllowFlg(VOS_TRUE);

            /*增加判断CS+PS ATTACH结果的原因值：只有两个域都不能正常服务时，才给AT上报ERROR */
          if ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != enCsServiceStatus )
           && (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != enPsServiceStatus) )
            {
                g_StatusContext.ucOperateType |= STA_OP_PHONEERROR;
                g_StatusContext.PhoneError = TAF_ERR_PHONE_ATTACH_FORBIDDEN;
            }
        }
        else
        {
            ucOperate = TAF_PH_EVT_ERR;
        }

        /* 更新系统状态变量 */
        Update.ucDomainFlag      = (VOS_UINT8)(pAttachCnf->enCnDomainId);
        Update.ulCsServiceStatus = TAF_SDC_GetCsServiceStatus();
        Update.ulPsServiceStatus = TAF_SDC_GetPsServiceStatus();
        Update.ulCsCause         = 0;
        Update.ulPsCause         = 0;
        Update.PlmnId.Mcc        = pAttachCnf->stPlmnId.ulMcc;
        Update.PlmnId.Mnc        = pAttachCnf->stPlmnId.ulMnc;
        /*
          系统集成测试中发现如下问题:
          1、缺少子状态输入值；
          2、导致Sta_UpdateFsmFlag()误将Update.ucFsmSubOperate
             作为子状态输入值；
        */

        /* 增加系统子状态的输入值，*/

        Update.ucFsmSubOperate   = STA_FSMSUB_FINISH;
        Update.ucFsmUpdateFlag   = STA_UPDATE_SINGL_CLEAR;
        Update.ucFsmStateSub     = g_StatusContext.aFsmSub[ucSub].ucFsmStateSub;

        /* 修改系统状态机的状态主值 */
        if (g_StatusContext.ucAttachReq <= 0)
        {
            /* ATTACH过程全部完成，状态主值置位 */
            if (0 == g_StatusContext.ucDetachReq)
            {
                Update.ulFsmState           = STA_FSM_ENABLE;
            }
            else
            {
                Update.ulFsmState           = STA_FSM_NO_UPDATE;
            }
            g_StatusContext.ucAttachReq = 0;
            /*
            V200R001版本需要在客户发起的客户
            ATTACH的结果上报，作如下改动：
            */
            if (STA_OPID_INSIDE != ulOpId)
            {
                /*如果不是设置参数发起的ATTACH过程*/
                if( MMA_SET_MSCLASSTYPE_NO ==
                    gstMmaValue.stSetMsClass.enSetMsClassTypeflag)
                {
                     Sta_EventReport (ulOpId, ucOperate);
                }
                else
                {
                    /*
                    如果是设置手机模式过程的ATTACH回应
                    则说明设置参数过程已经完成，需更
                    改全局变量与向上报告完成。
                    */
                    Sta_SyncMsClassType(ulOpId,
                              gstMmaValue.stSetMsClass.NewMsClassType);
                }
            }
        }
        else
        {/* ATTACH过程未全部完成，状态主值不变，
            继续等待ATTACH的结果
         */
            Update.ulFsmState    = STA_FSM_NO_UPDATE;
        }
        Update.ucFsmSubOperate   = STA_FSMSUB_FINISH;

        ulRet                    = Sta_UpdateData (&Update);
        if ( STA_ERROR == ulRet )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_AttachCnf():WARNING:Sta_UpdateData failed");
            return STA_ERROR;       /* 错误返回 */
        }
    }
    /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */

    return VOS_TRUE;         /* 正常返回 */
}


VOS_UINT32   Sta_SyncMsClassType(VOS_UINT32 ulOpId,
                                 TAF_PH_MS_CLASS_TYPE ucMsClass)
{
    VOS_UINT16   ClientId;
    VOS_UINT8          Id;
    VOS_UINT32      ulRet;
    VOS_UINT8       ucPhoneEvent;


    Id       = 0;
    ClientId = 0;

    ulRet = MMA_GetIdByTi(TAF_MMA, (VOS_UINT8)ulOpId, &ClientId, &Id, &ucPhoneEvent);
    if ( MMA_SUCCESS != ulRet )
    {
        MMA_WARNINGLOG("Sta_SyncMsClassType():WARING:MMA_GetIdByTi failed");
    }

    /*
    若获取TI失败，则应该继续同步全局变量，
    但无法上报结果，保证流程可以继续走下去
    */
    MMA_SyncMsClassType(ClientId, Id, ucMsClass);
    return MMA_SUCCESS;
}


VOS_UINT32 Sta_DetachCnf_SysCfgSet(MMC_MMA_DETACH_CNF_STRU *pDetachCnf )
{
    STA_PHONE_DATA_S                    Update;
    VOS_UINT32                          ulRst;
    MMA_SYS_CFG_SET_STRU               *pstSysCfgSet;
    TAF_PH_MS_CLASS_TYPE                ucClassType;

    MMA_MMC_ATTACH_TYPE_ENUM_UINT32     enAttachType;

    TAF_CS_SERVICE_ENUM_UINT32          enCsState;

    pstSysCfgSet = &gstMmaValue.stSysCfgSet;


    /*CS DETACH结果上报*/
    if (MMA_MMC_SRVDOMAIN_CS == pDetachCnf->enCnDomainId)
    {
        /*清除CS DETACH标志*/
        pstSysCfgSet->usSetFlag &= ~MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH;
    }

    /*PS DETACH结果上报*/
    if (MMA_MMC_SRVDOMAIN_PS == pDetachCnf->enCnDomainId)
    {
        /*清除PS DETACH标志*/
        pstSysCfgSet->usSetFlag &= ~MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH;
    }


    if ((!MMA_BIT_ISVALID(pstSysCfgSet->usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH))
        && (!MMA_BIT_ISVALID(pstSysCfgSet->usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH)))
    {
        /* 停止保护定时器 */
        if ( MMA_TIMER_RUN == g_stSyscfgWaitDetachCnfTimer.ucTimerStatus )
        {
            ulRst = NAS_StopRelTimer(WUEPS_PID_MMA,
                                     TI_MN_MMA_SYSCFG_WAIT_DETACH_CNF,
                                     &(g_stSyscfgWaitDetachCnfTimer.MmaHTimer));
            if (VOS_OK != ulRst)
            {
                STA_TRACE (STA_WARNING_PRINT, "Sta_DetachCnf_SysCfgSet():WARNING:NAS_StopRelTimer failed");
            }

            /*设置停止定时器标志*/
            g_stSyscfgWaitDetachCnfTimer.ucTimerStatus = MMA_TIMER_STOP;
        }
    }
    else
    {
        /* 有Detach消息标志时继续等待  */
        return STA_SUCCESS;
    }

    /*ATTACH,DETACH标志已经清除,则清除服务域设置标志*/
    if (MMA_SYS_CFG_NONE_SET == (pstSysCfgSet->usSetFlag & MMA_SYS_CFG_SRV_DOMAIN_NEED_ATTACH_DETACH))
    {
        pstSysCfgSet->usSetFlag &= ~MMA_SYS_CFG_SRV_DOMAIN_SET;
    }

    STA_TRACE1 (STA_INFO_PRINT,
                "Sta_DetachCnf_SysCfgSet:INFO:SYSCFG setflag:",
                pstSysCfgSet->usSetFlag);

    /*DETACH标志已经清除,但还有除服务域设置以外的其他设置*/
    if ((!MMA_BIT_ISVALID(pstSysCfgSet->usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH))
        && (!MMA_BIT_ISVALID(pstSysCfgSet->usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH))
        && (MMA_SYS_CFG_NONE_SET != (pstSysCfgSet->usSetFlag&MMA_SYS_CFG_SND_TO_MMC)))
    {
        if (!((MMA_BIT_ISVALID(gstMmaValue.stSysCfgSet.usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_ATTACH))
            || (MMA_BIT_ISVALID(gstMmaValue.stSysCfgSet.usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_ATTACH))))
        {
            MMA_ChangeSrv2Class(gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucSrvDomain, &ucClassType);

            /* 此处不需要通知STK和MSG模块CS域能力，因为该分支是要SYSCFG设置的，等MMA收到SYSCFG设置回复时再通知 */
            TAF_MMA_SndMmcModeChangeReq((MMA_MMC_MS_MODE_ENUM_UINT32)ucClassType);

            enCsState = TAF_MMA_GetCsStateFromSyscfgSetFlag(pstSysCfgSet->usSetFlag);
            if (TAF_CS_SERVICE_BUTT != enCsState)
            {
                TAF_MMA_SndStkCsServiceChangeNotify(enCsState);
                TAF_MMA_SndMsgCsServiceChangeNotify(enCsState);
            }

            /* 清除标识:置标识时没有判断是不是支持SVLTE，所以不支持SVLTE时，也需要统一清除 */
            TAF_MMA_ClearSyscfgCsServiceChangeFlag(pstSysCfgSet->usSetFlag);
        }

        TAF_MMA_SndMmcSysCfgReq(&(pstSysCfgSet->stSysCfgSetPara), pstSysCfgSet->usSetFlag);
    }
    /*DETACH标志已经清除,没有除服务域设置以外的其他设置,有ATTACH标志存在*/
    else if ((!MMA_BIT_ISVALID(pstSysCfgSet->usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH))
             && (!MMA_BIT_ISVALID(pstSysCfgSet->usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH))
             && (MMA_BIT_ISVALID(pstSysCfgSet->usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET)))
    {
        if (VOS_OK == TAF_MMA_ConvertAttachTypeToMmc(gstMmaValue.stSysCfgSet.ucAttachType, &enAttachType))
        {
            /*ATTACH消息需要先发送MODE CHANGE消息*/
            MMA_ChangeSrv2Class(gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucSrvDomain, &ucClassType);
            TAF_MMA_SndMmcModeChangeReq((MMA_MMC_MS_MODE_ENUM_UINT32)ucClassType);

            enCsState = TAF_MMA_GetCsStateFromSyscfgSetFlag(pstSysCfgSet->usSetFlag);
            if (TAF_CS_SERVICE_BUTT != enCsState)
            {
                TAF_MMA_SndStkCsServiceChangeNotify(enCsState);
                TAF_MMA_SndMsgCsServiceChangeNotify(enCsState);
            }

            /* 清除标识:置标识时没有判断是不是支持SVLTE，所以不支持SVLTE时，也需要统一清除 */
            TAF_MMA_ClearSyscfgCsServiceChangeFlag(pstSysCfgSet->usSetFlag);

            TAF_MMA_SndMmcAttachReq(STA_OPID_INSIDE, enAttachType, TAF_MMA_ATTACH_REASON_INITIAL);
        }

        pstSysCfgSet->usSetFlag = MMA_SYS_CFG_NONE_SET;

        TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_NO_ERROR);
    }
    else
    {

        /*上报设置结果 */
        TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_NO_ERROR);

        MMA_ChangeSrv2Class(gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucSrvDomain, &ucClassType);
        TAF_MMA_SndMmcModeChangeReq((MMA_MMC_MS_MODE_ENUM_UINT32)ucClassType);

        enCsState = TAF_MMA_GetCsStateFromSyscfgSetFlag(pstSysCfgSet->usSetFlag);
        if (TAF_CS_SERVICE_BUTT != enCsState)
        {
            TAF_MMA_SndStkCsServiceChangeNotify(enCsState);
            TAF_MMA_SndMsgCsServiceChangeNotify(enCsState);
        }

        /* 清除标识:置标识时没有判断是不是支持SVLTE，所以不支持SVLTE时，也需要统一清除 */
        TAF_MMA_ClearSyscfgCsServiceChangeFlag(pstSysCfgSet->usSetFlag);

        gstMmaValue.stSysCfgSet.usSetFlag = MMA_SYS_CFG_NONE_SET;

    }


    /* 更新系统状态变量 */
    Update.ucDomainFlag = (VOS_UINT8)pDetachCnf->enCnDomainId;

    Update.ulCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    Update.ulPsServiceStatus = TAF_SDC_GetPsServiceStatus();


    Update.ulCsCause = 0;
    Update.ulPsCause = 0;
    Update.ucFsmStateSub = STA_FSMSUB_NULL;

    if (MMA_SYS_CFG_NONE_SET == pstSysCfgSet->usSetFlag)
    {
        Update.ulFsmState = STA_FSM_ENABLE;
    }
    else
    {
        Update.ulFsmState = STA_FSM_NO_UPDATE;
    }

    Update.ucFsmSubOperate = STA_FSMSUB_FINISH;
    Update.ucFsmUpdateFlag = STA_UPDATE_DUAL_CLEAR;

    ulRst = Sta_UpdateData (&Update);
    if (STA_ERROR == ulRst)
    {
        STA_TRACE (STA_WARNING_PRINT,
                   "Sta_DetachCnf():WARNING:Sta_UpdateData failed  ");
    }
    return ulRst;
}
VOS_UINT32 Sta_DetachCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    STA_PHONE_DATA_S                    Update;                                 /* 需要更新的全局数据 */
    VOS_UINT32                          opId;                                   /* 命令操作数标识 */
    VOS_UINT8                           ucSub;                                  /* 子状态索引号 */
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucFsmSubSta;                            /*标示当前进行操作的域*/

    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    VOS_UINT8                           ucCtxIndex;

    /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */

    MMC_MMA_DETACH_CNF_STRU            *pDetachCnf;

    pDetachCnf = (MMC_MMA_DETACH_CNF_STRU*)pstMsg;

    pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();


    /* DETACH CNF是否准许被处理，若不准许处理，直接返回 */
    if ( VOS_FALSE == TAF_MMA_IsDetachCnfEnable())
    {
        return VOS_TRUE;
    }

    /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */

    /*增加在ENABLE状态下ATTACH_CNF的接受*/
    if ( STA_FSM_DE_ATTACH == g_StatusContext.ulFsmState)
    {
        /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */


        /* 没有DETACH REQ,进行异常保护 */
        if (VOS_TRUE != TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_DETACH_REQ, &ucCtxIndex))
        {
            return VOS_TRUE;
        } 

        /* 查找对应的子状态*/
        opId  = pDetachCnf->ulOpID;
        /*使用当前操作返回域确定上次操作进行的索引值*/
        if (  MMA_MMC_SRVDOMAIN_CS == pDetachCnf->enCnDomainId )
        {
            /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */
            ucFsmSubSta = STA_FSMSUB_DETACH_CS;
        }
        else if (MMA_MMC_SRVDOMAIN_PS == pDetachCnf->enCnDomainId )
        {
            /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */
            ucFsmSubSta = STA_FSMSUB_DETACH_PS;
        }
        else
        {
            ucFsmSubSta = STA_FSMSUB_NULL;
        }


        ucSub   = (VOS_UINT8)Sta_FindFsmSubBySubSta (ucFsmSubSta);
        if ( STA_FSMSUB_INDEX_NULL == ucSub )
        {/* 没有匹配的命令过程 */


            /* 给APP发送DETACH CNF */
            TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                                 TAF_MMA_APP_OPER_RESULT_FAILURE,
                                 TAF_ERR_PHONE_MSG_UNMATCH);

            TAF_MMA_ClearOperCtx(ucCtxIndex);

            /* 输出跟踪调试信息 */
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_DetachCnf():WARNING:wrong message parameter");

            return VOS_TRUE;
        }

        /*关闭DETACH定时器*/
        ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                 g_StatusContext.aFsmSub[ucSub].TimerName,
                                 &g_StatusContext.ahStaTimer[ucSub]);


        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_DetachCnf():WARNING:NAS_StopRelTimer failed  ");
        }
        g_StatusContext.ahStaTimer[ucSub] = 0;

        /* 清除请求过程个数 */
        if ((MMA_MMC_SRVDOMAIN_CS == pDetachCnf->enCnDomainId)
         || (MMA_MMC_SRVDOMAIN_PS == pDetachCnf->enCnDomainId))
        {
            g_StatusContext.ucDetachReq--;
        }

        /*上报detach结果*/
        /*只上报当前有效的域状态*/
        if (MMA_MMC_SRVDOMAIN_CS == pDetachCnf->enCnDomainId)
        {
            g_StatusContext.ucOperateType |= STA_OP_CSSERVICESTATUS;

            /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */
            
            /*增加判断DETACH结果的原因值*/
            if (MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE == pDetachCnf->enServiceStatus)
            {
                g_StatusContext.ucOperateType |= STA_OP_PHONEERROR;
                g_StatusContext.PhoneError     = TAF_ERR_PHONE_DETACH_FORBIDDEN;
            }
        }
        if (MMA_MMC_SRVDOMAIN_PS == pDetachCnf->enCnDomainId)
        {
            g_StatusContext.ucOperateType |= STA_OP_PSSERVICESTATUS;
            
            /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */
            
            /*增加判断DETACH结果的原因值*/
            if (MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE == pDetachCnf->enServiceStatus)
            {
                g_StatusContext.ucOperateType |= STA_OP_PHONEERROR;
                g_StatusContext.PhoneError     = TAF_ERR_PHONE_DETACH_FORBIDDEN;
            }
        }


        /*上报detach结果*/
        if ( STA_FSMSUB_DETACH_CS == g_StatusContext.aucFsmSubFlag[ucSub] )
        {
            TAF_SDC_SetCsAttachAllowFlg(VOS_FALSE);
        }
        else if ( STA_FSMSUB_DETACH_PS == g_StatusContext.aucFsmSubFlag[ucSub] )
        {
            TAF_SDC_SetPsAttachAllowFlg(VOS_FALSE);
        }
        else if ( STA_FSMSUB_DETACH_CSPS == g_StatusContext.aucFsmSubFlag[ucSub] )

        {
           /*g_StatusContext.ucOperateType = STA_OP_CSSERVICESTATUS|STA_OP_PSSERVICESTATUS;*/

           TAF_SDC_SetCsAttachAllowFlg(VOS_FALSE);
           TAF_SDC_SetPsAttachAllowFlg(VOS_FALSE);
        }
        else
        {
        }

        /* 更新系统状态变量 */
        Update.ucDomainFlag      = (VOS_UINT8)pDetachCnf->enCnDomainId;
        Update.ulCsServiceStatus = TAF_SDC_GetCsServiceStatus();
        Update.ulPsServiceStatus = TAF_SDC_GetPsServiceStatus();
        Update.ulCsCause         = 0;
        Update.ulPsCause         = 0;

        /*在确定子状态索引的时候，应使用当前的子状态值，而不能使用子状态标志，子状态标志
        在同时发起两个域的操作时，与子状态值不相等，其余情况相等。*/
        Update.ucFsmStateSub     = g_StatusContext.aFsmSub[ucSub].ucFsmStateSub;

        if ( g_StatusContext.ucDetachReq == 0 )
        {
            if ( 0 == g_StatusContext.ucAttachReq )
            {
                if (STA_FSM_RESTART == g_StatusContext.ulPreFsmState)
                {
                    Update.ulFsmState             = g_StatusContext.ulPreFsmState;

                    g_StatusContext.ulPreFsmState = STA_FSM_NULL;
                }
                else
                {
                    Update.ulFsmState           = STA_FSM_ENABLE;

                }
            }
            else
            {
                Update.ulFsmState           = STA_FSM_NO_UPDATE;
            }
            g_StatusContext.ucDetachReq = 0;
        }
        else
        {
            Update.ulFsmState        = STA_FSM_NO_UPDATE;
        }
        Update.ucFsmSubOperate   = STA_FSMSUB_FINISH;
        Update.ucFsmUpdateFlag   = STA_UPDATE_SINGL_CLEAR;

        ulRet = Sta_UpdateData (&Update);
        if ( STA_ERROR == ulRet )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_DetachCnf():WARNING:Sta_UpdateData failed  ");
        }
        /*
        当DETACH发生的过程不是启动过程，
        且已经完成DETACH请求的任务
        */
        if ( (STA_OPID_INSIDE != opId) && (0 == g_StatusContext.ucDetachReq))
        {
            /*不是设置参数发生的DETACH过程*/
            if (MMA_SET_MSCLASSTYPE_NO == gstMmaValue.stSetMsClass.enSetMsClassTypeflag )
            {
                if (STA_OP_PHONEERROR == (g_StatusContext.ucOperateType & STA_OP_PHONEERROR))
                {
                    TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                                         TAF_MMA_APP_OPER_RESULT_FAILURE,
                                         g_StatusContext.PhoneError);

                    TAF_MMA_ClearOperCtx(ucCtxIndex);
                }
                else
                {
                    TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                                         TAF_MMA_APP_OPER_RESULT_SUCCESS,
                                         TAF_ERR_NO_ERROR);

                    TAF_MMA_ClearOperCtx(ucCtxIndex);
                }

            }
            /*手机类型更改发生的DETACH过程*/
            else if (MMA_SET_MSCLASSTYPE_YES == gstMmaValue.stSetMsClass.enSetMsClassTypeflag )
            {
               ulRet = Sta_ProcSetMsClassTypeforDetach(pDetachCnf);
            }
            else
            {
                /*异常打印*/
                MMA_WARNINGLOG("Sta_DetachCnf():WARNING:ERROR Ms Class Type Flag!");
            }
        }
    }
    else if (STA_FSM_SYS_CFG_SET == g_StatusContext.ulFsmState)
    {
        /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */
        ulRet = Sta_DetachCnf_SysCfgSet(pDetachCnf);

        if ( STA_ERROR == ulRet )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_DetachCnf():WARNING:Sta_DetachCnf_SysCfgSet failed  ");
            return VOS_TRUE;
        }
    }
    else
    {   
          /* 服务状态上报，统一放到Sta_ServiceStatusInd()处理,attach cnf 和 detach cnf不再做处理 */
    }

    return VOS_TRUE;
}
VOS_UINT32 Sta_SysCfgCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PH_ERR_CODE                     usErrorCode;
    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU *pstLastSyscfgSet = VOS_NULL_PTR;

    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode;
    MMA_MMC_ATTACH_TYPE_ENUM_UINT32     enAttachType;
    TAF_PH_MS_CLASS_TYPE                ucClassType;
    TAF_CS_SERVICE_ENUM_UINT32          enCsState;
    MMC_MMA_SYS_CFG_SET_CNF_STRU       *pstSysCfgCnf;

    pstSysCfgCnf = (MMC_MMA_SYS_CFG_SET_CNF_STRU*)pstMsg;

    enSysMode = TAF_SDC_GetSysMode();

    pstLastSyscfgSet     = MN_MMA_GetLastSyscfgSetAddr();

    if (STA_FSM_SYS_CFG_SET == g_StatusContext.ulFsmState)
    {
        if ((MMA_BIT_ISVALID(gstMmaValue.stSysCfgSet.usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_ATTACH))
         || (MMA_BIT_ISVALID(gstMmaValue.stSysCfgSet.usSetFlag, MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_ATTACH)))
        {
            if (VOS_OK == TAF_MMA_ConvertAttachTypeToMmc(gstMmaValue.stSysCfgSet.ucAttachType, &enAttachType))
            {
                /*ATTACH消息需要先发送MODE CHANGE消息*/
                MMA_ChangeSrv2Class(gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucSrvDomain, &ucClassType);
                TAF_MMA_SndMmcModeChangeReq((MMA_MMC_MS_MODE_ENUM_UINT32)ucClassType);

                enCsState = TAF_MMA_GetCsStateFromSyscfgSetFlag(gstMmaValue.stSysCfgSet.usSetFlag);
                if (TAF_CS_SERVICE_BUTT != enCsState)
                {
                    TAF_MMA_SndStkCsServiceChangeNotify(enCsState);
                    TAF_MMA_SndMsgCsServiceChangeNotify(enCsState);
                }

                /* 清除标识:置标识时没有判断是不是支持SVLTE，所以不支持SVLTE时，也需要统一清除 */
                TAF_MMA_ClearSyscfgCsServiceChangeFlag(gstMmaValue.stSysCfgSet.usSetFlag);

                TAF_MMA_SndMmcAttachReq(STA_OPID_INSIDE, enAttachType, TAF_MMA_ATTACH_REASON_INITIAL);
            }
        }

        g_StatusContext.ulFsmState = g_StatusContext.ulPreFsmState;

        if (MMA_MMC_SYS_CFG_SET_SUCCESS == pstSysCfgCnf->enRst)
        {
            usErrorCode = TAF_ERR_NO_ERROR;


            if (TAF_MMA_SYSCFG_USER_AT == TAF_MMA_GetSyscfgUser())
            {
                 /* 更新当前设置成功的接入技术 */
                 MMA_PhSysCfgWriteNvim();
            }
            else
            {
                /* 只更新全局变量 */
                pstLastSyscfgSet->stRatPrioList = gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder;
            }


            /* 当前的接入技术，是否在新的接入技术列表中, 不在RAT列表中时，上报 Mode:0,0*/
            if ((VOS_FALSE == MN_MMA_IsSpecRatSupported(enSysMode))
             && (TAF_SDC_SYS_MODE_BUTT != enSysMode))
            {
                TAF_SDC_SetSysMode(TAF_SDC_SYS_MODE_BUTT);
                TAF_SDC_SetSysSubMode(TAF_SDC_SYS_SUBMODE_NONE);

                Sta_ModeChangeEventReport(TAF_PH_INFO_NONE_RAT, TAF_SYS_SUBMODE_NONE);

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
                /* 给MTC模块上报当前接入模式 */
                TAF_MMA_SndMtcRatModeInd(TAF_SDC_SYS_MODE_BUTT, NAS_UTRANCTRL_GetCurrUtranMode());
#endif

            }
        }
        else if (MMA_MMC_SYS_CFG_SET_FAILURE_CS_SERV_EXIST == pstSysCfgCnf->enRst)
        {
            usErrorCode = TAF_ERR_SYSCFG_CS_SERV_EXIST;
            /* 如果是4F36refresh触发的SYSCFG失败，需要启动TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING定时器 */
            TAF_MMA_RcvSyscfgCnfFail();
        }
        else
        {
            usErrorCode = TAF_ERR_ERROR;

            /* 如果是4F36refresh触发的SYSCFG失败，需要启动TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING定时器 */
            TAF_MMA_RcvSyscfgCnfFail();
        }

        /* 清除SYSCFG设置标志 */
        gstMmaValue.stSysCfgSet.usSetFlag = MMA_SYS_CFG_NONE_SET;

        if (TAF_MMA_SYSCFG_USER_AT == TAF_MMA_GetSyscfgUser())
        {
            TAF_MMA_ReportSysCfgSetCnf(usErrorCode);
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 Sta_ProcSetMsClassTypeforDetach(MMC_MMA_DETACH_CNF_STRU *pDetachCnf )
{
    VOS_UINT32          opId;   /* 命令操作数标识 */
    VOS_UINT32          ulRet = STA_SUCCESS;
    VOS_UINT16       ClientId;
    VOS_UINT8              Id;
    TAF_PHONE_EVENT     PhoneEvt;

     /* 查找对应的子状态*/
    opId  = pDetachCnf->ulOpID;

    /*先将允许发起attach,detach过程置位无效*/
    gstMmaValue.stSetMsClass.enSetMsClassTypeDeAttFlg = MMA_SET_CLASSTYPE_DE_ATTACH_DISABLE;

    /*判断Detach过程是否成功*/
    if(STA_SUCCESS == Sta_SuccessDetach(pDetachCnf) )
    {
        /* BEGIN: Added by liuyang id:48197, 2006/3/2   PN:A32D02310*/
        /*向MMC层发送模式切换命令*/
        /* gstMmaValue.stSetMsClass.enSetMsClassTypeflag现在没有置成YES的地方，
            所以Sta_ProcSetMsClassTypeforDetach现在已经走不到了，不需要通知STK/MSG CS域能力变更 */
        TAF_MMA_SndMmcModeChangeReq((MMA_MMC_MS_MODE_ENUM_UINT32)gstMmaValue.stSetMsClass.NewMsClassType);
        /* END:   Added by liuyang id:48197, 2006/3/2 */

        /*参数设置判断过程是否完成*/
        if ( STA_SUCCESS == Sta_CompleteDefMsClassType(pDetachCnf) )
        {
            /*调用Sta_SyncMsClassType()完成同步与结果上报*/
            Sta_SyncMsClassType(opId,
                           gstMmaValue.stSetMsClass.NewMsClassType);
           ulRet = STA_SUCCESS;
        }
        /*当过程为CG->CC或CC->CG时，继续进行ATTACH过程*/
        else
        {
            if (TAF_SUCCESS !=  MMA_GetIdByTi(TAF_MMA, (VOS_UINT8)opId, &ClientId, &Id, &PhoneEvt))
            {
                MMA_WARNINGLOG("Sta_ProcSetMsClassTypeforDetach():WARNING:Invoke Taf_GetIdByTi failed");
            }

            /* BEGIN: Added by liuyang id:48197, 2006/3/2   PN:A32D02310*/
            ulRet = MMA_StartInternalTimer(1, MMA_INTERNAL_OP_CLASS_CHANGE, ClientId, Id);
            /* END:   Added by liuyang id:48197, 2006/3/2 */
            /* BEGIN: Deleted by liuyang id:48197, 2006/3/2   PN:A32D02310*/
               /*判断最终要设定的手机类型*/
            /*if ( TAF_PH_MS_CLASS_CC ==   */
            /*     gstMmaValue.stSetMsClass.NewMsClassType)  */
            /*{  */
                   /*NULL -> CC*/
                   /* BEGIN: Added by liuyang, 2005/10/26 */
               /*gstMmaValue.stSetMsClass.enSetMsClassTypeDeAttFlg = MMA_SET_CLASSTYPE_ATTACH_ENABLE;  */
               /*MMA_PhoneAttach(ClientId, Id, TAF_PH_CS_OPERATE);  */
               /*Sta_Attach(opId, TAF_CS_ATTACH);  */
                   /* END:   Added by liuyang, 2005/10/26 */
            /*   ulRet = STA_SUCCESS;  */
            /*}  */
            /*else if(TAF_PH_MS_CLASS_CG ==  */
            /*        gstMmaValue.stSetMsClass.NewMsClassType)  */
            /*{  */
                    /*NULL -> CG*/
                   /* BEGIN: Added by liuyang, 2005/10/26 */
               /*gstMmaValue.stSetMsClass.enSetMsClassTypeDeAttFlg = MMA_SET_CLASSTYPE_ATTACH_ENABLE;  */
               /*MMA_PhoneAttach(ClientId, Id, TAF_PH_PS_OPERATE);  */
               /*Sta_Attach(opId, TAF_PS_ATTACH);  */
                   /* END:   Added by liuyang, 2005/10/26 */
                /*ulRet = STA_SUCCESS;  */
            /*}  */
            /*else  */
            /*{  */
                   /*打印错误消息*/
               /*STA_TRACE (STA_WARNING_PRINT,  */
               /*             "Sta_ProcSetMsClassTypeforDetach():WARNING: Error MS class attach type  input");  */
               /*ulRet  =  STA_ERROR;  */
            /*}  */
            /* END:   Deleted by liuyang id:48197, 2006/3/2 */
          }
    }
    else
    {
        /*
        向MMC发送MODE_CHANGE_REQ将状态回滚回未设置状态
        上报参数更改失败事件
        */
        /* gstMmaValue.stSetMsClass.enSetMsClassTypeflag现在没有置成YES的地方，
            所以Sta_ProcSetMsClassTypeforDetach现在已经走不到了，不需要通知STK/MSG CS域能力变更 */
        TAF_MMA_SndMmcModeChangeReq((MMA_MMC_MS_MODE_ENUM_UINT32)gstMmaValue.stSetMsClass.MsClassType);

        ulRet = Sta_SyncMsClassType(opId, gstMmaValue.stSetMsClass.MsClassType);
    }
    return ulRet;
}


VOS_UINT32 Sta_SuccessDetach(MMC_MMA_DETACH_CNF_STRU *pDetachCnf )
{
    VOS_UINT32                          ulAttachType;
    VOS_UINT32                          ulDetachType;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    Sta_ProcMsClassTypeNewToOld(gstMmaValue.stSetMsClass.MsClassType,
                                gstMmaValue.stSetMsClass.NewMsClassType,
                                &ulAttachType,
                                &ulDetachType);

    if ((STA_PS_DETACH   == ulDetachType)
     && (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != enPsServiceStatus))
    {
        return STA_SUCCESS;
    }

    if (( STA_CS_DETACH  == ulDetachType )
     && (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != enCsServiceStatus))
    {
        return STA_SUCCESS;
    }

    if ((STA_PS_CS_DETACH == ulDetachType)
     && ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE  != enPsServiceStatus)
      && (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != enCsServiceStatus)))
    {
        return STA_SUCCESS;
    }

    return STA_ERROR;
}

/*****************************************************************************
 函 数 名  : Sta_ProcMsClassTypeNewToOld
 功能描述  : 通过当前的手机类型与目标类型，返回要附着的和去附着的类型
 输入参数  : TAF_PH_MS_CLASS_TYPE   CurMsClassType
             TAF_PH_MS_CLASS_TYPE   NewMsClassType
             VOS_UINT32            *ulAttachType
             VOS_UINT32            *ulDetachtype
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年9月8日
    作    者   : liuyang
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 Sta_ProcMsClassTypeNewToOld(TAF_PH_MS_CLASS_TYPE   CurMsClassType,
                                       TAF_PH_MS_CLASS_TYPE   NewMsClassType,
                                       VOS_UINT32            *pulAttachType,
                                       VOS_UINT32            *pulDetachType)
{
   MMA_CLASS_CHANGE_INDEX_ENUM      enMsClassIndex;

  *pulAttachType =  STA_ATTACH_TYPE_NULL;
  *pulDetachType =  STA_DETACH_NULL;

   if (CurMsClassType == NewMsClassType)
   {
        return STA_SUCCESS;
   }

   enMsClassIndex  = (MMA_CLASS_CHANGE_INDEX_ENUM)(((CurMsClassType << 4) & 0xf0)
                                                    |(NewMsClassType & 0x0f));
   switch ( enMsClassIndex )
   {
        /*NULL -> A*/
        case MMA_NULL2A :
            *pulAttachType = STA_ATTACH_TYPE_GPRS_IMSI;
            break;
         /*A -> NULL*/
        case MMA_A2NULL :
            *pulDetachType = STA_PS_CS_DETACH;
            break;

        /*NULL -> CG*/
        case MMA_NULL2CG :
        /*CC -> A*/
        case MMA_CC2A:
            *pulAttachType = STA_ATTACH_TYPE_GPRS;
            break;
        /*NULL -> CC*/
        case MMA_NULL2CC :
        /*CG -> A*/
        case MMA_CG2A :
            *pulAttachType = STA_ATTACH_TYPE_IMSI;
            break;

        /*A -> CG*/
        case MMA_A2CG:
        /*CC ->NULL*/
        case MMA_CC2NULL :
            *pulDetachType = STA_CS_DETACH;
            break;

        /*A -> CC*/
        case MMA_A2CC:
        /*CG -> NULL*/
        case MMA_CG2NULL :
            *pulDetachType = STA_PS_DETACH;
            break;

       /*CG-> CC*/
        case MMA_CG2CC :
            *pulAttachType = STA_ATTACH_TYPE_IMSI;
            *pulDetachType = STA_PS_DETACH;
            break;

        /*CC -> CG*/
        case MMA_CC2CG :
            *pulAttachType = STA_ATTACH_TYPE_GPRS;
            *pulDetachType = STA_CS_DETACH;
            break;

        default:
             STA_TRACE (STA_WARNING_PRINT,
            "Sta_ProcMsClassTypeNewToOld():WARNING:Sta_ProcMsClassType New To Old failed");
        return STA_ERROR;   /* 错误返回 */
   }
   return STA_SUCCESS;
}
VOS_UINT32 Sta_CompleteDefMsClassType(MMC_MMA_DETACH_CNF_STRU *pDetachCnf )
{
    VOS_UINT32 ulAttachType, ulDetachType;

    /*获取当前需要ATTACH,DETACH的类型*/
    Sta_ProcMsClassTypeNewToOld(gstMmaValue.stSetMsClass.MsClassType,
                                gstMmaValue.stSetMsClass.NewMsClassType,
                                &ulAttachType,
                                &ulDetachType);

    /*若只有DETACH过程，则说明此过程到这已经完成*/
    if (STA_ATTACH_TYPE_NULL == ulAttachType)
    {
        return STA_SUCCESS;
    }
    return STA_ERROR;
}





VOS_UINT32 Sta_SelPlmnSpecializeCnf( VOS_UINT32 ulResult )
{
    VOS_UINT32 ulRet;

    STA_TRACE1(STA_NORMAL_PRINT, "@@@@Sta_SelPlmnSpecializeCnf:FsmState=",(VOS_INT32)g_StatusContext.ulFsmState);

    if ((STA_FSM_PLMN_RESEL == g_StatusContext.ulFsmState)
     && (MMA_MMC_USER_PLMN_SEARCH_RESULT_REJ == ulResult))
    {
        /*关闭状态定时器*/
        ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                 g_StatusContext.aFsmSub[0].TimerName,
                                 &g_StatusContext.ahStaTimer[0]);


        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_ServiceStatusInd():WARNING:NAS_StopRelTimer failed");
        }

        /*清理子状态*/
        ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);
        if (STA_ERROR == ulRet)
        {
            STA_TRACE(STA_WARNING_PRINT, "Sta_SelPlmnSpecializeCnf():WARNING:Sta_UpdateFsmFlag failed\n");
        }

         /* 上报当前指定搜网失败 */
        g_StatusContext.PhoneError    = TAF_ERR_ERROR;
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;
        MMA_WARNINGLOG("Sta_SelPlmnSpecializeCnf():WARING:REJECT PLMN USER SEL");
        /* 上报APP当前状态事件 */
        Sta_EventReport (g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_RESEL_CNF);

        return MMA_SUCCESS;
    }


    /*关闭状态定时器*/

    ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                             g_StatusContext.aFsmSub[0].TimerName,
                             &g_StatusContext.ahStaTimer[0]);


    if ( VOS_OK != ulRet )
    {
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_ServiceStatusInd():WARNING:NAS_StopRelTimer failed");
    }

    /* BEGIN: Modified by liuyang id:48197, 2006/3/27   PN:A32D02372*/
    /*内部发起的指定搜网过程,只有在手动重选的过程中,完成PLMNList后会发起*/

    STA_TRACE1(STA_NORMAL_PRINT, "Sta_SelPlmnSpecializeCnf:g_StatusContext.ulCurOpId=", (VOS_INT32)g_StatusContext.ulCurOpId);
    if ( STA_OPID_INSIDE == g_StatusContext.ulCurOpId )
    {
        g_StatusContext.ulCurOpId = 0;
        Sta_EventReport (g_StatusContext.ulCurOpId,
                         TAF_PH_EVT_PLMN_RESEL_CNF);
        STA_TRACE(STA_NORMAL_PRINT,"Sta_SelPlmnSpecializeCnf: 8 reported to AT!");
        return STA_SUCCESS;
    }
    /* END:   Modified by liuyang id:48197, 2006/3/27 */

    NAS_MN_ReportPlmnSrchResult(ulResult);

    ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);
    if (STA_ERROR == ulRet)
    {
        STA_TRACE(STA_WARNING_PRINT, "Sta_SelPlmnSpecializeCnf():WARNING:Sta_UpdateFsmFlag failed\n");
    }
    return MMA_SUCCESS;
}


VOS_UINT8 Sta_GetSysSubMode_W(STA_DATA_TRANSFER_STATUS_UINT8      ucDataTranStatus ,
                                       STA_DATA_TRANSFER_MODE_UINT8        ucDataTranMode
                                       )
{
    VOS_UINT8                          ucSysSubMode;

    if (STA_DATA_TRANSFER_PDP_ACT == ucDataTranStatus)
    {
        switch  ( ucDataTranMode )
        {
            case STA_DATA_TRANSFER_HSDPA :
                ucSysSubMode = TAF_SYS_SUBMODE_HSDPA;
                break;
            case STA_DATA_TRANSFER_HSUPA :
                ucSysSubMode = TAF_SYS_SUBMODE_HSUPA;
                break;
            case STA_DATA_TRANSFER_HSDPA_AND_HSUPA :
                ucSysSubMode = TAF_SYS_SUBMODE_HSDPA_HSUPA;
                break;
            case STA_DATA_TRANSFER_HSPA_PLUS :
                ucSysSubMode = TAF_SYS_SUBMODE_HSPA_PLUS;
                break;

            case STA_DATA_TRANSFER_DC_HSPA_PLUS:
                ucSysSubMode = TAF_SYS_SUBMODE_DC_HSPA_PLUS;
                break;

            case STA_DATA_TRANSFER_DC_MIMO:
                ucSysSubMode = TAF_SYS_SUBMODE_DC_MIMO;
                break;



            default:
                ucSysSubMode = TAF_SYS_SUBMODE_WCDMA;
                break;
        }
    }
    else
    {
        ucSysSubMode = TAF_SYS_SUBMODE_WCDMA;
    }

    return ucSysSubMode;
}


VOS_UINT8 Sta_GetSysSubMode_Td(
    STA_DATA_TRANSFER_STATUS_UINT8      ucDataTranStatus,
    STA_DATA_TRANSFER_MODE_UINT8        ucDataTranMode
)
{
    VOS_UINT8                          ucSysSubMode;

    if (STA_DATA_TRANSFER_PDP_ACT == ucDataTranStatus)
    {
        switch  ( ucDataTranMode )
        {
            case STA_DATA_TRANSFER_HSDPA :
                ucSysSubMode = TAF_SYS_SUBMODE_HSDPA;
                break;

            case STA_DATA_TRANSFER_HSUPA :
                ucSysSubMode = TAF_SYS_SUBMODE_HSUPA;
                break;

            case STA_DATA_TRANSFER_HSDPA_AND_HSUPA :
                ucSysSubMode = TAF_SYS_SUBMODE_HSDPA_HSUPA;
                break;

            case STA_DATA_TRANSFER_HSPA_PLUS :
                ucSysSubMode = TAF_SYS_SUBMODE_HSPA_PLUS;
                break;

            default:
                ucSysSubMode = TAF_SYS_SUBMODE_TD_SCDMA;
                break;
        }
    }
    else
    {
        ucSysSubMode = TAF_SYS_SUBMODE_TD_SCDMA;
    }

    return ucSysSubMode;
}

VOS_VOID Sta_ModeChangeEventReport(VOS_UINT8 ucNewSysMode, VOS_UINT8 ucNewSysSubMode)
{
    TAF_PHONE_EVENT_INFO_STRU *pstPhoneEvent;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("Sta_ModeChangeEventReport:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    pstPhoneEvent->OP_Mode      = MMA_TRUE;
    pstPhoneEvent->PhoneEvent   = TAF_PH_EVT_MODE_CHANGE_IND;
    pstPhoneEvent->RatType      = ucNewSysMode;
    pstPhoneEvent->ucSysSubMode = ucNewSysSubMode;

    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return;
}



VOS_VOID Sta_UpdateDataTranStatusAndMode(MMA_MMC_DATATRAN_ATTRI_ENUM_UINT8 enDataTranAttri)
{

    switch (enDataTranAttri)
    {
        case MMA_MMC_DATATRAN_ATTRI_PDP_ACT:
            g_StatusContext.ucDataTranStatus = STA_DATA_TRANSFER_PDP_ACT;

            if ((TAF_SDC_SYS_MODE_WCDMA == TAF_SDC_GetSysMode())
             && (STA_DATA_TRANSFER_MODE_BUTT == g_StatusContext.ucDataTranMode))
            {
                g_StatusContext.ucDataTranMode = STA_DATA_TRANSFER_R99_ONLY;
            }
            break;

        case MMA_MMC_DATATRAN_ATTRI_PDP_DEACT:
            g_StatusContext.ucDataTranStatus = STA_DATA_TRANSFER_PDP_DEACT;
            break;

        case MMA_MMC_DATATRAN_ATTRI_R99_ONLY:
            g_StatusContext.ucDataTranMode   = STA_DATA_TRANSFER_R99_ONLY;
            break;

        case MMA_MMC_DATATRAN_ATTRI_HSDPA:
            g_StatusContext.ucDataTranMode   = STA_DATA_TRANSFER_HSDPA;
            break;

        case MMA_MMC_DATATRAN_ATTRI_HSUPA:
            g_StatusContext.ucDataTranMode   = STA_DATA_TRANSFER_HSUPA;
            break;
        case MMA_MMC_DATATRAN_ATTRI_HSDPA_AND_HSUPA:
            g_StatusContext.ucDataTranMode   = STA_DATA_TRANSFER_HSDPA_AND_HSUPA;
            break;

        case MMA_MMC_DATATRAN_ATTRI_HSPA_PLUS:
            g_StatusContext.ucDataTranMode = STA_DATA_TRANSFER_HSPA_PLUS;
            break;

        case MMA_MMC_DATATRAN_ATTRI_LTE_DATA:
            g_StatusContext.ucDataTranStatus = STA_DATA_TRANSFER_PDP_ACT;
            g_StatusContext.ucDataTranMode = STA_DATA_TRANSFER_LTE_DATA;
            break;

        case MMA_MMC_DATATRAN_ATTRI_LTE_IDLE:
            g_StatusContext.ucDataTranMode = STA_DATA_TRANSFER_LTE_IDLE;
            break;

        case MMA_MMC_DATATRAN_ATTRI_DC_HSPA_PLUS:
            g_StatusContext.ucDataTranMode = STA_DATA_TRANSFER_DC_HSPA_PLUS;
            break;

        case MMA_MMC_DATATRAN_ATTRI_DC_MIMO:
            g_StatusContext.ucDataTranMode = STA_DATA_TRANSFER_DC_MIMO;
            break;


        default:
            STA_TRACE(STA_WARNING_PRINT, "Sta_DataTranAttri:Attri incorrect.");
            break;
    }

}


VOS_UINT32 Sta_DataTranAttri(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8   ucOldWSysSubMode = TAF_SYS_SUBMODE_WCDMA;
    VOS_UINT8   ucNewWSysSubMode = TAF_SYS_SUBMODE_WCDMA;

    VOS_UINT8   ucOldTdSysSubMode = TAF_SYS_SUBMODE_TD_SCDMA;
    VOS_UINT8   ucNewTdSysSubMode = TAF_SYS_SUBMODE_TD_SCDMA;

    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode;
    MMC_MMA_DATATRAN_ATTRI_IND_STRU    *pstDataTranAttri;

    pstDataTranAttri = (MMC_MMA_DATATRAN_ATTRI_IND_STRU*)pstMsg;


    enSysMode = TAF_SDC_GetSysMode();

    STA_TRACE1(STA_NORMAL_PRINT, "Sta_DataTranAttri:ucDataTranAttri=",(VOS_INT32)pstDataTranAttri->enDataTranAttri);

    if (TAF_SDC_SYS_MODE_WCDMA == enSysMode)
    {
        if (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
        {
            ucOldWSysSubMode = Sta_GetSysSubMode_W(g_StatusContext.ucDataTranStatus,
                                                   g_StatusContext.ucDataTranMode);
        }
        else
        {
            ucOldTdSysSubMode = Sta_GetSysSubMode_Td(g_StatusContext.ucDataTranStatus,
                                                     g_StatusContext.ucDataTranMode);
        }
    }

    /* 更新全局变量中的DataTranStatus 和 DataTranStatus */
    Sta_UpdateDataTranStatusAndMode(pstDataTranAttri->enDataTranAttri);


    /* 上报^MODE事件，如果系统子模式有改变 */
    if (TAF_SDC_SYS_MODE_WCDMA == enSysMode)
    {
        if (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
        {
            /* 当前UTRAN_MODE为FDD时，上报W的系统子模式 */
            ucNewWSysSubMode = Sta_GetSysSubMode_W(g_StatusContext.ucDataTranStatus,
                                                   g_StatusContext.ucDataTranMode);

            if(ucOldWSysSubMode != ucNewWSysSubMode)
            {

                Sta_ModeChangeEventReport(TAF_PH_INFO_WCDMA_RAT, ucNewWSysSubMode);
            }
        }
        else
        {
            /* 当前UTRAN_MODE为TDD时，上报TD的系统子模式 */
            ucNewTdSysSubMode = Sta_GetSysSubMode_Td(g_StatusContext.ucDataTranStatus,
                                                     g_StatusContext.ucDataTranMode);

            if(ucOldTdSysSubMode != ucNewTdSysSubMode)
            {

                Sta_ModeChangeEventReport(TAF_PH_INFO_TD_SCDMA_RAT, ucNewTdSysSubMode);
            }
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 Sta_GetServiceStatus()
{
    return TAF_SDC_GetServiceStatus();
}


VOS_VOID Sta_IccStatusReport(VOS_UINT32 ulCsServiceStatus, VOS_UINT32 ulPsServiceStatus)
{
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;
    VOS_UINT8                           ucSimStatus;
    VOS_UINT8                           ucSimLockStatus;
    
    ucSimStatus                         = MMA_GetUsimStatus(VOS_FALSE);
     
    /* 获取锁卡状态 */
    if ( MMA_SIM_IS_LOCK == MMA_GetMeLockStatus() )
    {
        ucSimLockStatus                 = VOS_TRUE;
    }
    else
    {
        ucSimLockStatus                 = VOS_FALSE;
    }
    

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    if (((enCsServiceStatus != ulCsServiceStatus)
      && (TAF_SDC_SERVICE_STATUS_NO_IMSI == enCsServiceStatus))
     || ((enPsServiceStatus != ulPsServiceStatus)
      && (TAF_SDC_SERVICE_STATUS_NO_IMSI == enPsServiceStatus)) )
    {
        if (TAF_PH_ME_SIM_UNLOCK == MMA_GetMeLockStatus())
        {
            TAF_MMA_SndAtIccStatusInd(ucSimStatus, ucSimLockStatus);
        }
    }

    return;
}
VOS_VOID Sta_ReportServiceEvent(VOS_UINT32 ulCsServiceStatus, VOS_UINT32 ulPsServiceStatus)
{
    static VOS_UINT8   ucLimitServiceSentFlg = VOS_FALSE;/* 标志limit service是否已经上报 */

    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    if ((enCsServiceStatus != ulCsServiceStatus)
     || (enPsServiceStatus != ulPsServiceStatus))
    {
        if ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enCsServiceStatus)
         && (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enPsServiceStatus))
        {
            NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_CS_PS_SERVICE, VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);
            ucLimitServiceSentFlg = VOS_FALSE;
        }
        else if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enCsServiceStatus)
        {
            /*当前仅CS处于normal service，则之前CS不为normal service或PS之前为normal service均需要上报CS_SERVICE*/
            if ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != ulCsServiceStatus)
             || (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == ulPsServiceStatus))
            {
                NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_CS_SERVICE, VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);
                ucLimitServiceSentFlg = VOS_FALSE;
            }
        }
        else if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enPsServiceStatus)
        {
            if ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != ulPsServiceStatus)
             || (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == ulCsServiceStatus))
            {
                NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PS_SERVICE, VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);
                ucLimitServiceSentFlg = VOS_FALSE;
            }
        }
        else
        {
            /*当前处于limit service，则之前CS为normal service或PS之前为normal service均需要上报LIMIT_SERVICE*/
            if (((TAF_SDC_SERVICE_STATUS_NO_SERVICE != enPsServiceStatus)
             || (TAF_SDC_SERVICE_STATUS_NO_SERVICE != enCsServiceStatus))
             && (VOS_FALSE == ucLimitServiceSentFlg))
            {
                NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_LIMITED_SERVICE, VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);
                ucLimitServiceSentFlg = VOS_TRUE;
            }
        }
    }

}

VOS_VOID TAF_MMA_ProcCsServiceStatusChanged(VOS_UINT8 ucOldCsServiceStatus)
{
    if(VOS_TRUE == TAF_MMA_IsCsServiceStatusChanged(ucOldCsServiceStatus))
    {
        TAF_MMA_SndStkLocationInfoInd();
    }
}



VOS_UINT8 TAF_MMA_IsCsServiceStatusChanged(VOS_UINT8 ucOldCsServiceStatus)
{
    if(ucOldCsServiceStatus != TAF_SDC_GetCsServiceStatus())
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}



VOS_UINT8 TAF_MMA_IsPsServiceStatusChanged(VOS_UINT8 ucOldPsServiceStatus)
{
    if(ucOldPsServiceStatus != TAF_SDC_GetPsServiceStatus())
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}


VOS_VOID Sta_UpdateServiceStatus(VOS_UINT32 ulServiceStatus, VOS_UINT32 ulCnDomainId)
{
    VOS_UINT32 ulPsServiceStatus;
    VOS_UINT32 ulCsServiceStatus;

    STA_TRACE1 (STA_INFO_PRINT,
        "Sta_UpdateServiceStatus():INFO:ulCnDomainId is: ", (VOS_INT32)ulCnDomainId);
    STA_TRACE1 (STA_INFO_PRINT,
        "Sta_UpdateServiceStatus():INFO:ulServiceStatus is: ", (VOS_INT32)ulServiceStatus);

    ulCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    ulPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    /* 更新CS/PS服务状态 */
    switch (ulCnDomainId)
    {
        case MMA_MMC_SRVDOMAIN_CS:
            if(MMA_MMC_SERVICE_STATUS_NO_CHANGE != ulServiceStatus)
            {
                TAF_SDC_SetCsServiceStatus((VOS_UINT8)ulServiceStatus);
            }
            break;
        case MMA_MMC_SRVDOMAIN_PS:
            if(MMA_MMC_SERVICE_STATUS_NO_CHANGE != ulServiceStatus)
            {
                TAF_SDC_SetPsServiceStatus((VOS_UINT8)ulServiceStatus);
            }
            break;
        case MMA_MMC_SRVDOMAIN_NO_CHANGE:
        case MMA_MMC_SRVDOMAIN_CS_PS:
        case MMA_MMC_SRVDOMAIN_NOT_REG_PLMN_SEARCHING:
            /* 这两种服务域只有MMC才会上报，因此PS/CS都需要更新 */
            if(MMA_MMC_SERVICE_STATUS_NO_CHANGE != ulServiceStatus)
            {
                TAF_SDC_SetCsServiceStatus((VOS_UINT8)ulServiceStatus);
                TAF_SDC_SetPsServiceStatus((VOS_UINT8)ulServiceStatus);
            }
            break;
        default:
            break;
    }

    Sta_IccStatusReport(ulCsServiceStatus, ulPsServiceStatus);
    Sta_ReportServiceEvent(ulCsServiceStatus, ulPsServiceStatus);

    ulCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    ulPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    /* 更新总的服务状态 */
    if (  (MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE == ulCsServiceStatus)
       && (MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE == ulPsServiceStatus) )
    {
        TAF_SDC_SetServiceStatus(TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE);
        TAF_SDC_SetServiceDomain(TAF_SDC_SERVICE_DOMAIN_CS_PS);
    }
    else if (MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE == ulCsServiceStatus)
    {
        TAF_SDC_SetServiceStatus(TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE);
        TAF_SDC_SetServiceDomain(TAF_SDC_SERVICE_DOMAIN_CS);
    }
    else if (MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE == ulPsServiceStatus)
    {
        TAF_SDC_SetServiceStatus(TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE);
        TAF_SDC_SetServiceDomain(TAF_SDC_SERVICE_DOMAIN_PS);
    }
    else if ((MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE_REGION == ulCsServiceStatus)
          || (MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE_REGION == ulPsServiceStatus) )
    {
        TAF_SDC_SetServiceStatus(TAF_SDC_REPORT_SRVSTA_REGIONAL_LIMITED_SERVICE);
    }
    else if ((MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE == ulCsServiceStatus)
          || (MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE == ulPsServiceStatus) )
    {
        TAF_SDC_SetServiceStatus(TAF_SDC_REPORT_SRVSTA_LIMITED_SERVICE);
    }
    else if ((MMA_MMC_SERVICE_STATUS_NO_IMSI == ulCsServiceStatus)
          || (MMA_MMC_SERVICE_STATUS_NO_IMSI == ulPsServiceStatus) )
    {
        TAF_SDC_SetServiceStatus(TAF_SDC_REPORT_SRVSTA_LIMITED_SERVICE);
    }
    else
    {
        TAF_SDC_SetServiceStatus(TAF_SDC_REPORT_SRVSTA_NO_SERVICE);


        if (VOS_TRUE == gstMmaValue.pg_StatusContext->ulTimMaxFlg )
        {
            TAF_SDC_SetServiceStatus(TAF_SDC_REPORT_SRVSTA_DEEP_SLEEP);
        }

    }

    /* 更新总的服务域 */
    if (TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE != TAF_SDC_GetServiceStatus() )
    {
        if(MMA_MMC_SRVDOMAIN_NOT_REG_PLMN_SEARCHING == ulCnDomainId)
        {
            TAF_SDC_SetServiceDomain(TAF_SDC_SERVICE_DOMAIN_SEARCHING);
        }
        else
        {
            switch(gstMmaValue.stSetMsClass.NewMsClassType)
            {
                case TAF_PH_MS_CLASS_A:
                case TAF_PH_MS_CLASS_B:
                    TAF_SDC_SetServiceDomain(TAF_SDC_SERVICE_DOMAIN_CS_PS);
                    break;
                case TAF_PH_MS_CLASS_CC:
                    TAF_SDC_SetServiceDomain(TAF_SDC_SERVICE_DOMAIN_CS);
                    break;
                case TAF_PH_MS_CLASS_CG:
                    TAF_SDC_SetServiceDomain(TAF_SDC_SERVICE_DOMAIN_PS);
                    break;
                case TAF_PH_MS_CLASS_NULL:
                    TAF_SDC_SetServiceDomain(TAF_SDC_SERVICE_DOMAIN_NO_DOMAIN);
                    break;
                default:
                    TAF_SDC_SetServiceDomain(TAF_SDC_SERVICE_DOMAIN_NO_DOMAIN);
                    break;
            }
        }
    }
    else
    {
        g_StatusContext.ulTimMaxFlg = VOS_FALSE;
    }

    if ( TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE == TAF_SDC_GetServiceStatus() )
    {
        MN_PH_UpdateEndRegTime();
    }

}



VOS_VOID Sta_UpdateLocalState(
    MMC_MMA_SERVICE_STATUS_IND_STRU    *pServiceStatus,
    VOS_UINT8                           ucFsmSubOperate
)
{
    STA_PHONE_DATA_S                    Update;

    TAF_SDC_PLMN_ID_STRU               *pstPlmnId = VOS_NULL_PTR;

    pstPlmnId = TAF_SDC_GetCurrCampPlmnId();
    Update.ulCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    Update.ulPsServiceStatus = TAF_SDC_GetPsServiceStatus();
    Update.ucDomainFlag      = MMA_MMC_SRVDOMAIN_CS_PS;
    Update.ulCsCause         = 0;
    Update.ulPsCause         = 0;

    Update.PlmnId.Mcc        = pstPlmnId->ulMcc;
    Update.PlmnId.Mnc        = pstPlmnId->ulMnc;
    Update.ucFsmSubOperate   = ucFsmSubOperate;
    Update.ulFsmState        = STA_FSM_ENABLE;
    Update.ucFsmStateSub     = STA_FSMSUB_NULL;
    Update.ucFsmUpdateFlag   = STA_UPDATE_MONO_CLEAR;

    if ( STA_ERROR == Sta_UpdateData (&Update) )
    {
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_UpdateLocalState():WARNING:Sta_UpdateData failed");
    }
}


VOS_UINT32 TAF_MMA_IsPsSrvStatusChanged(
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enCnDomainId,
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32  enServiceStatus
)
{
    if ((MMA_MMC_SRVDOMAIN_PS    == enCnDomainId)
     || (MMA_MMC_SRVDOMAIN_CS_PS == enCnDomainId))
    {
        if (TAF_SDC_GetPsServiceStatus() != enServiceStatus)
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}



VOS_VOID TAF_MMA_ProcMmcServiceStatusInd(
    MMC_MMA_SERVICE_STATUS_IND_STRU    *pstServiceStatus
)
{
    VOS_UINT8                           ucOldCsSrvSta;
    VOS_UINT8                           ulOldSrvSta;
    
#if (FEATURE_IMS == FEATURE_ON)    
    if (VOS_TRUE  == TAF_SDC_GetImsSupportFlag())
    {
        /* PS服务状态发生变化通知IMSA或SPM模块 */       
        if (VOS_TRUE == TAF_MMA_IsPsSrvStatusChanged(pstServiceStatus->enCnDomainId, pstServiceStatus->enServiceStatus))
        {
            /* 通知SPM服务状态变更，触发SPM处理缓存的消息，消息中不带服务域和服务状态，由SPM取SDC全局变量中的 */
            TAF_MMA_SndSpmServiceStatusChangeNotify();

            /* 修改通知IMSA的SERVICE CHANGE IND逻辑,收到服务状态地方变化后通知,另外收到PS注册完成后通知一次 */
            if ((MMA_MMC_SRVDOMAIN_PS    == pstServiceStatus->enCnDomainId)
             || (MMA_MMC_SRVDOMAIN_CS_PS == pstServiceStatus->enCnDomainId))
            { 
                TAF_MMA_SndImsaSrvInfoNotify(pstServiceStatus->enServiceStatus);             
            }
        }
    }
#endif

    ucOldCsSrvSta   = TAF_SDC_GetCsServiceStatus();
    ulOldSrvSta     = TAF_SDC_GetServiceStatus();

    Sta_UpdateServiceStatus(pstServiceStatus->enServiceStatus, pstServiceStatus->enCnDomainId);

    TAF_MMA_ProcCsServiceStatusChanged(ucOldCsSrvSta);
    TAF_MMA_ProcLociStatusEvent();

    if (ulOldSrvSta != TAF_SDC_GetServiceStatus())
    {
        g_StatusContext.ucOperateType = STA_OP_PHONESERVICESTATUS;
    
        Sta_EventReport(g_StatusContext.ulCurOpId, TAF_PH_EVT_SERVICE_STATUS_IND);
    }

    return;
}



VOS_UINT32 Sta_ServiceStatusInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    STA_PHONE_DATA_S                    Update;         /* 需要更新的全局数据 */
    VOS_UINT32                          ulRet;
    TAF_SDC_PLMN_ID_STRU               *pstPlmnId = VOS_NULL_PTR;

#if ((FEATURE_MULTI_MODEM == FEATURE_ON) || (FEATURE_CL_INTERWORK == FEATURE_ON))
    VOS_UINT32                          ulOldUsimValidStatus;
    VOS_UINT32                          ulNewUsimValidStatus;
#endif

#if (FEATURE_CL_INTERWORK == FEATURE_ON)
    VOS_UINT8                           ulOldSrvSta;
#endif

    MMC_MMA_SERVICE_STATUS_IND_STRU    *pServiceStatus;

    pServiceStatus = (MMC_MMA_SERVICE_STATUS_IND_STRU*)pstMsg;

    pstPlmnId = TAF_SDC_GetCurrCampPlmnId();

#if (FEATURE_CL_INTERWORK == FEATURE_ON)
    ulOldSrvSta     = TAF_SDC_GetServiceStatus();
#endif

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    ulOldUsimValidStatus = TAF_SDC_IsUsimStausValid();
#endif

    /* 更新SDC中的SIM卡的CS/PS的注册状态 */
    TAF_SDC_SetSimCsRegStatus(pServiceStatus->ucSimCsRegStatus);
    TAF_SDC_SetSimPsRegStatus(pServiceStatus->ucSimPsRegStatus);

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    ulNewUsimValidStatus = TAF_SDC_IsUsimStausValid();

    /* 如果cs ps卡状态发生改变需要通知mtc */
    if (ulOldUsimValidStatus != ulNewUsimValidStatus)
    {
        TAF_MMA_SndMtcRegStatusInd((VOS_UINT8)ulNewUsimValidStatus);
    }
#endif

    /* 更新本地的服务状态 */
    if (VOS_TRUE == pServiceStatus->bitOpSrvSta)
    {
        TAF_MMA_ProcMmcServiceStatusInd(pServiceStatus); 
    }

    if (VOS_TRUE == pServiceStatus->bitOpRegSta)
    {
        TAF_MMA_ReportRegStatus(pServiceStatus->enRegState, pServiceStatus->enCnDomainId);
    }

#if (FEATURE_CL_INTERWORK == FEATURE_ON)
    if ((ulOldSrvSta != TAF_SDC_GetServiceStatus())
     || (ulOldUsimValidStatus != ulNewUsimValidStatus))
    {
        if (VOS_TRUE == TAF_MMA_IsPowerOnCLInterWork())
        {
            TAF_MMA_SndCmmcaServiceInd();
        }
    }
#endif

    switch ( g_StatusContext.ulFsmState )
    {/* 系统状态 */
        case  STA_FSM_PLMN_SEL:

            Sta_UpdateLocalState(pServiceStatus, STA_FSMSUB_UPDATE);

            break;

        case  STA_FSM_ENABLE:

            /*取得PS和CS域的服务信息*/
            Update.ulCsServiceStatus = TAF_SDC_GetCsServiceStatus();
            Update.ulPsServiceStatus = TAF_SDC_GetPsServiceStatus();
            Update.ucDomainFlag      = (VOS_UINT8)pServiceStatus->enCnDomainId;
            Update.ulCsCause         = 0;
            Update.ulPsCause         = 0;

            Update.PlmnId.Mcc        = pstPlmnId->ulMcc;
            Update.PlmnId.Mnc        = pstPlmnId->ulMnc;


            Update.ucFsmSubOperate   = STA_FSMSUB_FINISH;
            Update.ulFsmState        = STA_FSM_ENABLE;
            Update.ucFsmStateSub     = STA_FSMSUB_NULL;
            Update.ucFsmUpdateFlag   = STA_UPDATE_DUAL_CLEAR;

            g_StatusContext.ucAttachReq = 0;
            g_StatusContext.ucDetachReq = 0;

            ulRet = Sta_UpdateData (&Update);
            if ( STA_ERROR == ulRet )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_ServiceStatusInd():WARNING:Sta_UpdateData failed");
            }

            break;

        default:
            
            break;
    }

    return VOS_TRUE;         /* 正常返回 */
}


VOS_UINT32 Sta_DetachInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    STA_PHONE_DATA_S                    Update; /* 需要更新的全局数据 */
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulOldSrvSta;
    VOS_UINT8                           ucOldCsSrvSta; 

    MMC_MMA_DETACH_IND_STRU            *pDetachInd;

    pDetachInd = (MMC_MMA_DETACH_IND_STRU*)pstMsg;


    /* ENABLE状态*/
    if ( g_StatusContext.ulFsmState & STA_FSM_ENABLE )
    {
        ulOldSrvSta = TAF_SDC_GetServiceStatus();

        ucOldCsSrvSta = TAF_SDC_GetCsServiceStatus();

        Sta_UpdateServiceStatus(pDetachInd->enServiceStatus, pDetachInd->enCnDomainId);

        TAF_MMA_ProcCsServiceStatusChanged(ucOldCsSrvSta);

        /*取得PS和CS域的服务信息*/
        Update.ulCsServiceStatus = TAF_SDC_GetCsServiceStatus();
        Update.ulPsServiceStatus = TAF_SDC_GetPsServiceStatus();
        Update.ulCsCause         = 0;
        Update.ulPsCause         = 0;
        Update.ucDomainFlag      = (VOS_UINT8)pDetachInd->enCnDomainId;

        Update.ucFsmSubOperate   = STA_FSMSUB_FINISH;
        Update.ulFsmState        = STA_FSM_ENABLE;
        Update.ucFsmStateSub     = STA_FSMSUB_NULL;
        Update.ucFsmUpdateFlag   = STA_UPDATE_DUAL_CLEAR;
        ulRet = Sta_UpdateData (&Update);
        if ( STA_ERROR == ulRet )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_DetachInd():WARNING:Sta_UpdateData failed");
        }

        /* 向APP上报事件*/
        g_StatusContext.ucOperateType = STA_OP_CSSERVICESTATUS
                                       |STA_OP_PSSERVICESTATUS;
        if (ulOldSrvSta != TAF_SDC_GetServiceStatus())
        {
            g_StatusContext.ucOperateType |= STA_OP_PHONESERVICESTATUS;
        }
        Sta_EventReport (g_StatusContext.ulCurOpId,
            TAF_PH_EVT_SERVICE_STATUS_IND);

        return VOS_TRUE;
    }


    /* 没有做处理 */
    STA_TRACE (STA_WARNING_PRINT,
        "Sta_DetachInd():WARNING:invalid state, do nothing");

    return VOS_TRUE;
}
VOS_UINT32 Sta_ReOrderPlmnList (TAF_PLMN_LIST_STRU* pPlmnList)
{
    VOS_UINT32   ulRegPlmnIndx = TAF_MAX_PLMN_NUM;
    VOS_UINT32   ulSamePlmnIndx = TAF_MAX_PLMN_NUM;
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder = VOS_NULL_PTR;
    VOS_UINT32   ulIndx = 0;

    TAF_PLMN_ID_STRU    stPlmnId;
    TAF_PLMN_STATUS_STRU    stStatus;
    PS_MEM_SET(&stPlmnId, 0, sizeof(stPlmnId));
    PS_MEM_SET(&stStatus, 0, sizeof(stStatus));

    pstRatOrder     = MN_MMA_GetRatPrioListAddr();

    /* 上报列表数小于2或当前模式非双模,可直接返回 */
    if( (pPlmnList->ucPlmnNum < 2 )
     || (pstRatOrder->ucRatOrderNum <= 1) )
    {
        return STA_SUCCESS;
    }
    /* 确定当前是否有注册的PLMN */
    for( ulIndx = 0; ulIndx < pPlmnList->ucPlmnNum; ++ulIndx)
    {
        if( TAF_PH_NETWORK_STATUS_CURRENT == pPlmnList->PlmnInfo[ulIndx].PlmnStatus)
        {
            ulRegPlmnIndx = ulIndx;
        }
    }
    if(TAF_MAX_PLMN_NUM == ulRegPlmnIndx)
    {
        return STA_ERROR; /* 上报列表中没有注册的PLMN 返回错误*/
    }
    stPlmnId.Mcc = pPlmnList->Plmn[ulRegPlmnIndx].Mcc;
    stPlmnId.Mnc = pPlmnList->Plmn[ulRegPlmnIndx].Mnc;
    /* 当前是双模且有注册的PLMN,确定列表中是否有其它列表项与当前注册的PLMN相同 */
    for( ulIndx = 0; ulIndx < pPlmnList->ucPlmnNum; ++ulIndx)
    {
        if( (stPlmnId.Mnc == pPlmnList->Plmn[ulIndx].Mnc)
            && (stPlmnId.Mcc == pPlmnList->Plmn[ulIndx].Mcc)
            && (ulIndx != ulRegPlmnIndx) )
        {
            ulSamePlmnIndx = ulIndx;
        }
    }
    if(TAF_MAX_PLMN_NUM == ulSamePlmnIndx)
    {
       return STA_SUCCESS; /* 上报列表中没有与注册PLMN相同的列表项 */
    }
    if(ulRegPlmnIndx < ulSamePlmnIndx)
    {
       return STA_SUCCESS; /* 上报列表中的注册PLMN的顺序已正常 ,返回*/
    }
    /* 调整注册PLMN项到前面 返回*/
    PS_MEM_CPY(&stStatus, &(pPlmnList->PlmnInfo[ulSamePlmnIndx]), sizeof(TAF_PLMN_STATUS_STRU));
    PS_MEM_CPY(&(pPlmnList->PlmnInfo[ulSamePlmnIndx]), &(pPlmnList->PlmnInfo[ulRegPlmnIndx]), sizeof(TAF_PLMN_STATUS_STRU));
    PS_MEM_CPY(&(pPlmnList->PlmnInfo[ulRegPlmnIndx]), &stStatus, sizeof(TAF_PLMN_STATUS_STRU));
    return STA_SUCCESS; /* 上报列表中的注册PLMN的顺序调整完毕,返回*/
}


VOS_VOID Sta_PlmnListEventReport (VOS_UINT32 ulOpID)
{
    TAF_PHONE_EVENT_PLMN_LIST_STRU         *pstPhoneEvent;
    VOS_UINT8                               n;          /* 循环次数 */
    VOS_UINT16                              ClientId;
    VOS_UINT8                               AppOpId;
    TAF_PHONE_EVENT                         ucPhoneEvent;
    TAF_PH_OPERATOR_NAME_FORMAT_OP_STRU     AtOperName;

    if ( STA_OP_PLMNLIST & g_StatusContext.ucOperateType )
    {
        pstPhoneEvent = (TAF_PHONE_EVENT_PLMN_LIST_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_PLMN_LIST_STRU));

        if (VOS_NULL_PTR == pstPhoneEvent)
        {
            STA_TRACE(PS_PRINT_ERROR,"Sta_PlmnListEventReport:ERROR:ALLOC MEMORY FAIL.");
            return;
        }

        PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_PLMN_LIST_STRU));

        pstPhoneEvent->PhoneEvent   = TAF_PH_EVT_PLMN_LIST_CNF;
        pstPhoneEvent->ulPlmnNum    = 0;
        for ( n = 0; n < g_StatusContext.PlmnList.ucPlmnNum; n++ )
        {
            if (n >= MAX_PLMN_NAME_LIST )
            {
                break;
            }

            pstPhoneEvent->astPlmnName[n].PlmnId.Mcc = g_StatusContext.PlmnList.Plmn[n].Mcc;
            pstPhoneEvent->astPlmnName[n].PlmnId.Mnc = g_StatusContext.PlmnList.Plmn[n].Mnc;
            MMA_PlmnId2Bcd(&(pstPhoneEvent->astPlmnName[n].PlmnId));
            pstPhoneEvent->astPlmnInfo[n].PlmnStatus = g_StatusContext.PlmnList.PlmnInfo[n].PlmnStatus;
            pstPhoneEvent->astPlmnInfo[n].RaMode     = g_StatusContext.PlmnList.PlmnInfo[n].RaMode;

            PS_MEM_SET(&AtOperName,0,sizeof(AtOperName));
            AtOperName.OperName.PlmnId.Mcc  = pstPhoneEvent->astPlmnName[n].PlmnId.Mcc;
            AtOperName.OperName.PlmnId.Mnc  = pstPhoneEvent->astPlmnName[n].PlmnId.Mnc;
            AtOperName.ListOp               = TAF_PH_OPER_NAME_PLMN2ALPHANUMERICNAME;

            /* 获取优先网络的长短名 */
            if (MMA_SUCCESS == Taf_PhoneGetNetworkNameForListPlmn(&AtOperName))
            {
                PS_MEM_CPY(pstPhoneEvent->astPlmnName[n].aucOperatorNameLong,AtOperName.OperName.aucOperatorNameLong,TAF_PH_OPER_NAME_LONG);
                PS_MEM_CPY(pstPhoneEvent->astPlmnName[n].aucOperatorNameShort,AtOperName.OperName.aucOperatorNameShort,TAF_PH_OPER_NAME_SHORT);
            }
            else
            {
                pstPhoneEvent->astPlmnName[n].aucOperatorNameLong[0]  = '\0';
                pstPhoneEvent->astPlmnName[n].aucOperatorNameShort[0] = '\0';
            }

            pstPhoneEvent->ulPlmnNum++;
        }

        /*APP发起的过程最大Ti是1*/
        if (ulOpID <= TAF_MAX_STATUS_TI)
        {
            /*通过Ti获取ClientId,CallId*/
            if (TAF_SUCCESS != MMA_GetIdByTi(TAF_MMA,(VOS_UINT8)ulOpID, &ClientId, &AppOpId, &ucPhoneEvent))
            {
                PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
                MMA_WARNINGLOG("Taf_PhoneEvent():WARNING:Invoke Taf_GetIdByTi failed");
                return;
            }
        }
        else
        {   /*是STATUS主动发起的事件*/
            AppOpId = MMA_OP_ID_INTERNAL;
            /*ClientId置为广播值:高字节的低4bit类型是MUX_PORT_BUTT*/
            ClientId = MMA_CLIENTID_BROADCAST;
        }

        pstPhoneEvent->ClientId  = ClientId;
        pstPhoneEvent->OpId      = AppOpId;

        /* 调用电话管理上报函数 */
        MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_PLMN_LIST_STRU));

        PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    }

    return ;    /* 正常返回 */
}


VOS_UINT32 Sta_PlmnListInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_PLMN_LIST_CNF_STRU         *pPlmnListInd = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           n;          /* 循环次数 */

    pPlmnListInd = (MMC_MMA_PLMN_LIST_CNF_STRU*)pstMsg;

    /* 系统状态 */
    switch ( g_StatusContext.ulFsmState )
    {
        case  STA_FSM_PLMN_LIST:
            /* 关闭状态定时器 */

            ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                     g_StatusContext.aFsmSub[0].TimerName,
                                     &g_StatusContext.ahStaTimer[0]);

            if ( VOS_OK != ulRet )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_PlmnListInd():WARNING:NAS_StopRelTimer failed");
            }
            g_StatusContext.ahStaTimer[0] = 0;

            /* 保存参数结果 */
            for ( n = 0; n < pPlmnListInd->ulCnt; n++ )
            {
                g_StatusContext.PlmnList.Plmn[n].Mcc
                    = pPlmnListInd->aPlmnList[n].ulMcc;
                g_StatusContext.PlmnList.Plmn[n].Mnc
                    = pPlmnListInd->aPlmnList[n].ulMnc;
                /* BEGIN: Added by liuyang, id:48197, 2005/10/16 */
                /*增加每个PLMN的状态和接入模式*/
                g_StatusContext.PlmnList.PlmnInfo[n].PlmnStatus
                    = pPlmnListInd->aucPlmnStatus[n];
                g_StatusContext.PlmnList.PlmnInfo[n].RaMode
                    = pPlmnListInd->aucRaMode[n];
                /* END:   Added by liuyang, id:48197, 2005/10/16 */
            }
            g_StatusContext.PlmnList.ucPlmnNum = (VOS_UINT8)pPlmnListInd->ulCnt;

            /*对上报的PLMN列表进行调整 */
            Sta_ReOrderPlmnList(&g_StatusContext.PlmnList);
            /* 向APP上报事件*/
            g_StatusContext.ucOperateType = STA_OP_PLMNLIST;

            /* 列表搜回复结果通过专有的事件回复 */
            Sta_PlmnListEventReport(g_StatusContext.ulCurOpId);
            break;

        case  STA_FSM_PLMN_RESEL:
            /* 不是手动重选子状态 */
            if ( STA_FSMSUB_PLMN_RESEL_MANUAL
                != g_StatusContext.aFsmSub[0].ucFsmStateSub )
            {
                /* 输出跟踪调试信息 */
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_PlmnListInd():WARNING:is not manual PLMN_RESEL");

                return VOS_TRUE;
            }
            /* 关闭状态定时器 */
            ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                     g_StatusContext.aFsmSub[0].TimerName,
                                     &g_StatusContext.ahStaTimer[0]);


            if ( VOS_OK != ulRet )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_PlmnListInd():WARNING:NAS_StopRelTimer failed");
            }
            g_StatusContext.ahStaTimer[0] = 0;
            /* BEGIN: Added by liuyang, 2005/10/26 */
            /* 保存参数结果 */
            for ( n = 0; n < pPlmnListInd->ulCnt; n++ )
            {
                g_StatusContext.PlmnList.Plmn[n].Mcc
                    = pPlmnListInd->aPlmnList[n].ulMcc;
                g_StatusContext.PlmnList.Plmn[n].Mnc
                    = pPlmnListInd->aPlmnList[n].ulMnc;
                /* BEGIN: Added by liuyang, id:48197, 2005/10/16 */
                /*增加每个PLMN的状态和接入模式*/
                g_StatusContext.PlmnList.PlmnInfo[n].PlmnStatus
                    = pPlmnListInd->aucPlmnStatus[n];
                g_StatusContext.PlmnList.PlmnInfo[n].RaMode
                    = pPlmnListInd->aucRaMode[n];
                /* END:   Added by liuyang, id:48197, 2005/10/16 */
            }
            g_StatusContext.PlmnList.ucPlmnNum = (VOS_UINT8)pPlmnListInd->ulCnt;
            /* END:   Added by liuyang, 2005/10/26 */
            /* BEGIN: Modified by liuyang, 2005/10/26 */
            /*应该上报plmn列表*/
            /* 向APP上报事件*/
            /*g_StatusContext.ucOperateType = STA_OP_CSSERVICESTATUS
                                           |STA_OP_PSSERVICESTATUS
                                           |STA_OP_PLMNRESELMODE;*/
            g_StatusContext.ucOperateType = STA_OP_PLMNLIST
                                           |STA_OP_PLMNRESELMODE;
            /* END:   Modified by liuyang, 2005/10/26 */

            /* BEGIN: Added by liuyang id:48197, 2006/3/27   PN:A32D02372*/


            Sta_EventReport (g_StatusContext.ulCurOpId,
                             TAF_PH_EVT_PLMN_RESEL_CNF);    /*<==A32D12559*/

            if ( (MMA_PLMN_MCC_NONE != g_StatusContext.StaPlmnId.Mcc)
               &&(MMA_PLMN_MNC_NONE != g_StatusContext.StaPlmnId.Mnc)
               )
            {
                /*返回注册状态,等待用户发起指定PLMN搜索*/
                ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);
                if (STA_ERROR == ulRet)
                {
                    STA_TRACE(STA_WARNING_PRINT, "Sta_PlmnListInd():WARNING:Sta_UpdateFsmFlag failed\n");
                }
                /*启动注册内部Rplmn过程*/
                Sta_PlmnSelect(STA_OPID_INSIDE,
                               g_StatusContext.StaPlmnId,
                               TAF_PH_RA_MODE_DEFAULT,  /*<==A32D12536*/
                               TAF_PLMN_RESELETION_MANUAL);
            }
            else
            {
                /*
                Sta_EventReport (g_StatusContext.ulCurOpId,
                                 TAF_PH_EVT_PLMN_RESEL_CNF);
                */
                /*返回注册状态,等待用户发起指定PLMN搜索*/
                ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);
                if (STA_ERROR == ulRet)
                {
                    STA_TRACE(STA_WARNING_PRINT, "Sta_PlmnListInd():WARNING:Sta_UpdateFsmFlag failed\n");
                }
            }

            return VOS_TRUE;
        case STA_FSM_ENABLE:
            /*直接上报列表*/
            for ( n = 0; n < pPlmnListInd->ulCnt; n++ )
            {
                g_StatusContext.PlmnList.Plmn[n].Mcc
                    = pPlmnListInd->aPlmnList[n].ulMcc;
                g_StatusContext.PlmnList.Plmn[n].Mnc
                    = pPlmnListInd->aPlmnList[n].ulMnc;
                /* BEGIN: Added by liuyang, id:48197, 2005/10/16 */
                /*增加每个PLMN的状态和接入模式*/
                g_StatusContext.PlmnList.PlmnInfo[n].PlmnStatus
                    = pPlmnListInd->aucPlmnStatus[n];
                g_StatusContext.PlmnList.PlmnInfo[n].RaMode
                    = pPlmnListInd->aucRaMode[n];
                /* END:   Added by liuyang, id:48197, 2005/10/16 */
            }
            g_StatusContext.PlmnList.ucPlmnNum = (VOS_UINT8)pPlmnListInd->ulCnt;
            /* END:   Added by liuyang, 2005/10/26 */
            /* BEGIN: Modified by liuyang, 2005/10/26 */
            /*应该上报plmn列表*/
            g_StatusContext.ucOperateType = STA_OP_PLMNLIST
                                           |STA_OP_PLMNRESELMODE;
            Sta_EventReport (g_StatusContext.ulCurOpId,
                             TAF_PH_EVT_PLMN_LIST_IND);

            return VOS_TRUE;
        /* END:   Added by liuyang id:48197, 2005/12/14 */
        default :
            /* do nothing */
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_PlmnListInd():WARNING:invalid state, do nothing");

            return VOS_TRUE;
    }

    /*清除子状态*/
    ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);
    if (STA_ERROR == ulRet)
    {
        STA_TRACE(STA_WARNING_PRINT, "Sta_PlmnListInd():WARNING:Sta_UpdateFsmFlag failed\n");
    }
    return VOS_TRUE;
}
VOS_UINT32 Sta_CoverageAreaInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    STA_PHONE_DATA_S                                        Update;         /* 需要更新的全局数据 */
    VOS_UINT32                                              ulRet;
    MMC_MMA_COVERAGE_AREA_IND_STRU                         *pCoverageAreaInd;

    TAF_SDC_SYS_MODE_ENUM_UINT8                             ucSysMode;
    TAF_SDC_SYS_SUBMODE_ENUM_UINT8                          ucSysSubmode;

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    MMA_MTC_CURR_CAMP_PLMN_INFO_IND_STRU                    stSndMtcPlmnInfo;
#endif
    
    pCoverageAreaInd = (MMC_MMA_COVERAGE_AREA_IND_STRU*)pstMsg;

    g_StatusContext.ulTimMaxFlg = pCoverageAreaInd->ulTimMaxFlg;

    if (MMA_MMC_COVERAGE_AREA_CHANGE_MODE_LOST == pCoverageAreaInd->enCoverageChgMode)
    {
        TAF_SDC_SetCampOnFlag(VOS_FALSE);
        
        TAF_MMA_SndMtaAreaLostInd();
        
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
        if (VOS_TRUE == TAF_SDC_GetUtranSkipWPlmnSearchFlag())
        {
            PS_MEM_SET(&stSndMtcPlmnInfo, 0, sizeof(stSndMtcPlmnInfo));
            stSndMtcPlmnInfo.enRatMode             = MTC_RATMODE_BUTT;
            stSndMtcPlmnInfo.ucIsForbiddenPlmnFlag = VOS_FALSE;
            stSndMtcPlmnInfo.ucPsAttachAllowFlag   = TAF_SDC_GetPsAttachAllowFlg();
            stSndMtcPlmnInfo.stPlmnId.ulMcc        = TAF_SDC_INVALID_MCC;
            stSndMtcPlmnInfo.stPlmnId.ulMnc        = TAF_SDC_INVALID_MCC;
            TAF_MMA_SndMtcCurrCampPlmnInfoInd(&stSndMtcPlmnInfo);
        }
#endif
        
        ucSysMode = TAF_SDC_GetSysMode();
        ucSysSubmode = TAF_SDC_GetSysSubMode();
        if ((TAF_SDC_SYS_MODE_BUTT    != ucSysMode)
         || (TAF_SDC_SYS_SUBMODE_NONE != ucSysSubmode)){
            Mma_ModeChgReport(TAF_SDC_SYS_MODE_BUTT, TAF_SYS_SUBMODE_NONE);
            TAF_SDC_SetSysMode(TAF_SDC_SYS_MODE_BUTT);
            TAF_SDC_SetSysSubMode(TAF_SYS_SUBMODE_NONE); 
        }
    }

    switch ( g_StatusContext.ulFsmState )
    {
        case  STA_FSM_ENABLE :
            /*进出服务区时，MMC均会向MMA发送service status ind，指示CS/PS服务状态
              以及PLMNID，因此删除下面代码*/
            return VOS_TRUE;

        case  STA_FSM_DE_ATTACH:
            /* 参数为出覆盖区*/
            /* Detach过程中，进出服务区时，MMC均会向MMA发送detach cnf和service status ind，指示CS/PS服务状态
              以及PLMNID，因此删除下面代码 */
            return VOS_TRUE;

        case  STA_FSM_PLMN_RESEL: /* PLMN_RESEL状态 */
            /* 子状态为自动重选模式*/
            if ( STA_FSMSUB_PLMN_RESEL_AUTO
                == g_StatusContext.aFsmSub[0].ucFsmStateSub )
            {
                /* 参数为进入覆盖区*/
                if ( MMA_MMC_COVERAGE_AREA_CHANGE_MODE_ENTER == pCoverageAreaInd->enCoverageChgMode )
                { /* PLMN驻留区 */
                    /* do nothing */
                    STA_TRACE (STA_NORMAL_PRINT,
                                "Sta_CoverageAreaInd():NORMAL:resident PLMN indicator,\
                                do nothing");
                    return VOS_TRUE;
                }
                else /* 出覆盖区*/
                {
                    /* 关闭PLMN_RESEL状态定时器*/
                    if ( 0 != g_StatusContext.ahStaTimer[0] )
                    {
                        ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                                 g_StatusContext.aFsmSub[0].TimerName,
                                                 &g_StatusContext.ahStaTimer[0]);


                        if ( VOS_OK != ulRet )
                        {
                            STA_TRACE (STA_WARNING_PRINT,
                                        "Sta_CoverageAreaInd():WARNING:\
                                        NAS_StopRelTimer failed");
                        }
                        g_StatusContext.ahStaTimer[0] = 0;
                    }

                    /* 保存参数结果 */
                    Update.ulCsServiceStatus = pCoverageAreaInd->enCsSvcSta;
                    Update.ulPsServiceStatus = pCoverageAreaInd->enPsSvcSta;

                    Update.ulCsCause         = MMA_MMC_CAUSE_NULL;
                    Update.ulPsCause         = MMA_MMC_CAUSE_NULL;

                    Update.ucFsmSubOperate   = STA_FSMSUB_FINISH;
                    Update.ulFsmState        = STA_FSM_ENABLE;
                    Update.ucFsmStateSub     = STA_FSMSUB_NULL;
                    Update.ucFsmUpdateFlag   = STA_UPDATE_MONO_CLEAR;
                    ulRet = Sta_UpdateData (&Update);
                    if ( STA_ERROR == ulRet )
                    {
                        STA_TRACE (STA_WARNING_PRINT,
                                    "Sta_CoverageAreaInd():WARNING:\
                                    Sta_UpdateData failed");
                    }

                    /* 向APP上报事件*/
                    g_StatusContext.ucOperateType = STA_OP_CSSERVICESTATUS
                        |STA_OP_PSSERVICESTATUS|STA_OP_PLMNRESELMODE;
                    Sta_EventReport (g_StatusContext.ulCurOpId,
                        TAF_PH_EVT_PLMN_RESEL_CNF);
                }
                return VOS_TRUE;
            }/* 结束 子状态为自动重选模式 */

            /* 子状态为手动重选模式 */
            if ( STA_FSMSUB_PLMN_RESEL_MANUAL
                == g_StatusContext.aFsmSub[0].ucFsmStateSub )
            {
                /* 参数为出覆盖区*/
                if ( MMA_MMC_COVERAGE_AREA_CHANGE_MODE_LOST == pCoverageAreaInd->enCoverageChgMode )
                {
                    /* 关闭PLMN_RESEL状态定时器*/
                    if ( 0 != g_StatusContext.ahStaTimer[0] )
                    {
                        ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                                 g_StatusContext.aFsmSub[0].TimerName,
                                                 &g_StatusContext.ahStaTimer[0]);


                        if ( VOS_OK != ulRet )
                        {
                            STA_TRACE (STA_WARNING_PRINT,
                                "Sta_CoverageAreaInd():WARNING:\
                                NAS_StopRelTimer failed  ");
                        }
                        g_StatusContext.ahStaTimer[0] = 0;
                    }

                    /* 保存参数结果 */
                    Update.ulCsServiceStatus = pCoverageAreaInd->enCsSvcSta;
                    Update.ulPsServiceStatus = pCoverageAreaInd->enPsSvcSta;

                    Update.ulCsCause         = MMA_MMC_CAUSE_NULL;
                    Update.ulPsCause         = MMA_MMC_CAUSE_NULL;

                    Update.ucFsmSubOperate   = STA_FSMSUB_FINISH;
                    Update.ulFsmState        = STA_FSM_ENABLE;
                    Update.ucFsmStateSub     = STA_FSMSUB_NULL;
                    Update.ucFsmUpdateFlag   = STA_UPDATE_MONO_CLEAR;
                    ulRet = Sta_UpdateData (&Update);
                    if ( STA_ERROR == ulRet )
                    {
                        STA_TRACE (STA_WARNING_PRINT,
                            "Sta_CoverageAreaInd():WARNING:Sta_UpdateData failed  ");
                    }

                    /* 向APP上报事件*/
                    g_StatusContext.ucOperateType = STA_OP_CSSERVICESTATUS
                        |STA_OP_PSSERVICESTATUS|STA_OP_PLMNRESELMODE;
                    Sta_EventReport (g_StatusContext.ulCurOpId,
                        TAF_PH_EVT_PLMN_RESEL_CNF);
                }
                return VOS_TRUE;
            }
            return VOS_TRUE;

        case  STA_FSM_PLMN_SEL:  /* PLMN_SEL状态 */
            /* 参数为进入覆盖区*/
            /* 删除之前在这里的保护处理,目前用户指定搜网时,MMC都会上报指定搜网结果
               无需此处进行保护 */
            return VOS_TRUE;

        default:
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_CoverageAreaInd():WARNING:invalid state, do nothing  ");
            return VOS_TRUE;
    }
}


VOS_VOID NAS_MN_ReportPlmnSrchResult(
    VOS_UINT32                  ulResult
)
{
    if ( MMA_MMC_USER_PLMN_SEARCH_RESULT_SUCCESS == ulResult )
    {
        /* 上报当前指定搜网成功 */
        g_StatusContext.PhoneError    = TAF_ERR_NO_ERROR;
        if (STA_FSM_PLMN_RESEL == g_StatusContext.ulFsmState)
        {
            g_StatusContext.ucOperateType = STA_OP_PLMNRESELMODE;
            /* 上报APP当前状态事件 */
            Sta_EventReport (g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_RESEL_CNF);
            STA_TRACE(STA_NORMAL_PRINT, "NAS_MN_ReportPlmnSrchResult: TAF_PH_EVT_PLMN_RESEL_CNF reported to AT!");
        }
        else if(STA_FSM_PLMN_SEL == g_StatusContext.ulFsmState)
        {
            /* 设置上报事件类型，并上报APP搜网成功 */
            g_StatusContext.ucOperateType = STA_OP_REGPLMN;
            Sta_EventReport (g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_SEL_CNF);
        }
        else
        {
        }
    }
    else
    {
        /* 上报当前指定搜网失败 */
        g_StatusContext.PhoneError    = TAF_ERR_NO_NETWORK_SERVICE;

        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;
        MMA_NORMAILLOG("NAS_MN_ReportPlmnSrchResult():NORMAIL:SPECIALIZE THE PLMN FAILURE!");
        /* 上报APP当前状态事件 */
        if (STA_FSM_PLMN_RESEL == g_StatusContext.ulFsmState)
        {
            /* 上报APP当前状态事件 */
            Sta_EventReport (g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_RESEL_CNF);
            STA_TRACE(STA_NORMAL_PRINT, "NAS_MN_ReportPlmnSrchResult: TAF_PH_EVT_PLMN_RESEL_CNF reported to AT!");
        }
        else if(STA_FSM_PLMN_SEL == g_StatusContext.ulFsmState)
        {
            /* 上报APP当前状态事件 */
            Sta_EventReport (g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_SEL_CNF);
            STA_TRACE(STA_NORMAL_PRINT, "NAS_MN_ReportPlmnSrchResult: TAF_PH_EVT_PLMN_SEL_CNF reported to AT!");
        }
        else
        {
        }
    }
}
VOS_UINT32 Sta_PlmnListRej(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32          ulRet;

    /* 不是PLMN_LIST状态 */
    if ( !(g_StatusContext.ulFsmState & STA_FSM_PLMN_LIST) )
    {
        g_StatusContext.PhoneError = TAF_ERR_PHONE_MSG_UNMATCH;

        /* 输出跟踪调试信息 */
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_PlmnListInd():WARNING:unmatched system FSM state\n" );

        return VOS_TRUE;
    }

    /* 关闭状态定时器*/

    ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                             g_StatusContext.aFsmSub[0].TimerName,
                             &g_StatusContext.ahStaTimer[0]);


    if ( VOS_OK != ulRet )
    {
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_PlmnListRej():WARNING:NAS_StopRelTimer failed  ");
    }
    g_StatusContext.ahStaTimer[0]    = 0;
    /* 空闲标志 */
    g_StatusContext.aucFsmSubFlag[0] = STA_FSMSUB_NULL;

    /*清除子状态*/
    ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);
    if (STA_ERROR == ulRet)
    {
        STA_TRACE(STA_WARNING_PRINT, "Sta_PlmnListRej():WARNING:Sta_UpdateFsmFlag failed\n");
    }

    /* 协议栈拒绝PLMN搜索*/
    /* 向APP上报事件*/
    Sta_EventReport (g_StatusContext.ulCurOpId,
        TAF_PH_EVT_PLMN_LIST_REJ);

    return VOS_TRUE;
}

VOS_UINT32 Sta_PlmnListAortCnf (MMC_MMA_PLMN_LIST_ABORT_CNF_STRU *pPlmnListAbortCnf)
{
    VOS_UINT16                          ClientId;
    VOS_UINT8                           AppOpId;
    TAF_PHONE_EVENT                     PhoneEvent;

    VOS_UINT32                          ulIndex;

    VOS_UINT32                          ulRet;

    /* stop cnf和search cnf对冲时有可能带上来的列表不为空，需要上报*/
    if (pPlmnListAbortCnf->ulCnt > 0)
    {
        for ( ulIndex = 0; ulIndex < pPlmnListAbortCnf->ulCnt; ulIndex++ )
        {
            g_StatusContext.PlmnList.Plmn[ulIndex].Mcc
                = pPlmnListAbortCnf->aPlmnList[ulIndex].ulMcc;
            g_StatusContext.PlmnList.Plmn[ulIndex].Mnc
                = pPlmnListAbortCnf->aPlmnList[ulIndex].ulMnc;

            /*增加每个PLMN的状态和接入模式*/
            g_StatusContext.PlmnList.PlmnInfo[ulIndex].PlmnStatus
                = pPlmnListAbortCnf->aucPlmnStatus[ulIndex];
            g_StatusContext.PlmnList.PlmnInfo[ulIndex].RaMode
                = pPlmnListAbortCnf->aucRaMode[ulIndex];

        }
        g_StatusContext.PlmnList.ucPlmnNum = (VOS_UINT8)pPlmnListAbortCnf->ulCnt;

        /*对上报的PLMN列表进行调整 */
        Sta_ReOrderPlmnList(&g_StatusContext.PlmnList);

        /* 向APP上报事件*/
        g_StatusContext.ucOperateType = STA_OP_PLMNLIST;

        /* 列表搜回复结果通过专有的事件回复 */
        Sta_PlmnListEventReport(g_StatusContext.ulCurOpId);
    }
    else
    {
        /* 向AT回复列表搜超时结果 */
        g_StatusContext.PhoneError    = TAF_ERR_TIME_OUT;
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

        if (g_StatusContext.ulCurOpId <= TAF_MAX_STATUS_TI)
        {
            /*通过Ti获取ClientId,CallId*/
            if (TAF_SUCCESS != MMA_GetIdByTi(TAF_MMA,(VOS_UINT8)g_StatusContext.ulCurOpId, &ClientId, &AppOpId, &PhoneEvent))
            {
                MMA_WARNINGLOG("Taf_PhoneEvent():WARNING:Invoke Taf_GetIdByTi failed");
                return STA_ERROR;
            }
        }
        else
        {   /*是STATUS主动发起的事件*/
            AppOpId = MMA_OP_ID_INTERNAL;
            /*ClientId置为广播值:高字节的低4bit类型是MUX_PORT_BUTT*/
            ClientId = MMA_CLIENTID_BROADCAST;
        }

        /* 上报APP当前状态事件 */
        MMA_HandleEventError(ClientId, AppOpId, TAF_ERR_TIME_OUT, TAF_PH_EVT_ERR);
    }


    /* 更新状态标志 */
    ulRet = Sta_UpdateFsmFlag (STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);

    if (STA_ERROR == ulRet)
    {
        STA_TRACE(STA_WARNING_PRINT, "Sta_PlmnListAortCnf():WARNING:Sta_UpdateFsmFlag failed\n");
    }

    /* 设置当前没有处理用户列表搜打断过程 */
    g_StatusContext.ucPlmnListAbortProc = TAF_MMA_PLMN_LIST_ABORT_BUTT;

    return STA_SUCCESS;
}



VOS_UINT32 TAF_MMA_PlmnListAbortCnfUserAbort (MMC_MMA_PLMN_LIST_ABORT_CNF_STRU *pPlmnListAbortCnf)
{

    VOS_UINT32                      ulRet;

    g_StatusContext.ucOperateType = STA_OP_NULL;

    /* 向AT回复列表搜打断 */
    Sta_EventReport (g_StatusContext.ulCurOpId,
            TAF_PH_EVT_PLMN_LIST_ABORT_CNF);

    /* 更新状态标志 */
    ulRet = Sta_UpdateFsmFlag (STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);

    if (STA_ERROR == ulRet)
    {
        STA_TRACE(STA_WARNING_PRINT, "TAF_MMA_PlmnListAbortCnfUserAbort():WARNING:Sta_UpdateFsmFlag failed\n");
    }

    /* 设置当前没有处理用户列表搜打断过程 */
    g_StatusContext.ucPlmnListAbortProc = TAF_MMA_PLMN_LIST_ABORT_BUTT;

    return STA_SUCCESS;
}
VOS_UINT32 TAF_MMA_RcvPlmnListAbortCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_PLMN_LIST_ABORT_CNF_STRU   *pPlmnListAbortCnf;

    pPlmnListAbortCnf = (MMC_MMA_PLMN_LIST_ABORT_CNF_STRU*)pstMsg;

    /* 当前不是PLMN LIST状态，直接返回 */
    if (STA_FSM_PLMN_LIST != g_StatusContext.ulFsmState)
    {
        STA_TRACE(STA_WARNING_PRINT, "TAF_MMA_RcvPlmnListAbortCnf():WARNING:Sta_UpdateFsmFlag failed\n");
        return VOS_TRUE;
    }

    /* 异常保护：当前没有正在处理用户列表搜打断，直接返回 */
    if (TAF_MMA_PLMN_LIST_ABORT_BUTT == g_StatusContext.ucPlmnListAbortProc)
    {
        STA_TRACE(STA_WARNING_PRINT, "TAF_MMA_RcvPlmnListAbortCnf():WARNING:Sta_UpdateFsmFlag failed\n");
        return VOS_TRUE;
    }

    /* 停定时器 */
    if (VOS_OK != NAS_StopRelTimer(WUEPS_PID_MMA,
                     STA_TIMER_PLMN_LIST_ABORT,
                     &g_StatusContext.ahStaTimer[0]))
    {
        STA_TRACE(STA_WARNING_PRINT, "TAF_MMA_RcvPlmnListAbortCnf():WARNING:NAS_StopRelTimer failed!");
    }

    g_StatusContext.ahStaTimer[0] = VOS_NULL_PTR;

    /* 定时器超时导致的PLMN LIST ABORT的处理 */
    if ( TAF_MMA_PLMN_LIST_ABORT_PROCESSING_TIMEOUT == g_StatusContext.ucPlmnListAbortProc)
    {
        Sta_PlmnListAortCnf(pPlmnListAbortCnf);
    }
    else
    {
        TAF_MMA_PlmnListAbortCnfUserAbort(pPlmnListAbortCnf);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_RcvSpecPlmnSearchAbortCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (VOS_OK != NAS_StopRelTimer(WUEPS_PID_MMA,
                     STA_TIMER_SPEC_PLMN_ABORT,
                     &g_StatusContext.ahStaTimer[0]))
    {
        STA_TRACE(STA_WARNING_PRINT, "TAF_MMA_RcvSpecPlmnSearchAortCnf():WARNING:NAS_StopRelTimer failed!");
    }
    /* 向AT回复指定搜超时结果 */
    g_StatusContext.PhoneError    = TAF_ERR_TIME_OUT;
    g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

    /* 上报APP当前状态事件 */
    Sta_EventReport (g_StatusContext.ulCurOpId,
        TAF_PH_EVT_PLMN_SEL_CNF);

    /* 迁移状态到ENABLE状态 */
    g_StatusContext.ulFsmState = STA_FSM_ENABLE;

    return VOS_TRUE;
}

/* 函数Sta_MsgInd删除 */



TAF_SDC_REPORT_SRVSTA_ENUM_UINT8 TAF_MMA_ConvertTafSrvStaToStkType(
                                    TAF_SDC_REPORT_SRVSTA_ENUM_UINT8  enTafSrvType
                                    )
{
    TAF_SDC_REPORT_SRVSTA_ENUM_UINT8 enRtType;

    enRtType = enTafSrvType;

    if(TAF_SDC_REPORT_SRVSTA_REGIONAL_LIMITED_SERVICE == enTafSrvType)
    {
        enRtType = TAF_SDC_REPORT_SRVSTA_LIMITED_SERVICE;
    }

    if(TAF_SDC_REPORT_SRVSTA_DEEP_SLEEP == enTafSrvType)
    {
        enRtType = TAF_SDC_REPORT_SRVSTA_NO_SERVICE;
    }

    return enRtType;
}



TAF_SDC_SERVICE_STATUS_ENUM_UINT8 TAF_MMA_ConvertSrvTypeForStk(
                                    TAF_SDC_SERVICE_STATUS_ENUM_UINT8  enTafSrvType
                                    )
{
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8 enRtType;

    enRtType = enTafSrvType;

    if (TAF_SDC_SERVICE_STATUS_LIMITED_SERVICE_REGION == enTafSrvType
     || TAF_SDC_SERVICE_STATUS_NO_IMSI == enTafSrvType)
    {
        enRtType = TAF_SDC_SERVICE_STATUS_LIMITED_SERVICE;
    }

    if (TAF_SDC_SERVICE_STATUS_DEEP_SLEEP == enTafSrvType)
    {
        enRtType = TAF_SDC_SERVICE_STATUS_NO_SERVICE;
    }

    return enRtType;
}




TAF_SDC_REPORT_SRVSTA_ENUM_UINT8 TAF_MMA_GetSrvTypeForStk(VOS_VOID)
{
    TAF_SDC_REPORT_SRVSTA_ENUM_UINT8    enRtType;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;
    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU *pstSysCfg;

    pstSysCfg         = MN_MMA_GetLastSyscfgSetAddr();

    enCsServiceStatus = TAF_MMA_ConvertSrvTypeForStk(TAF_SDC_GetCsServiceStatus());
    enPsServiceStatus = TAF_MMA_ConvertSrvTypeForStk(TAF_SDC_GetPsServiceStatus());

    if ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enCsServiceStatus)
     || (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enPsServiceStatus))
    {
        enRtType = TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE;
    }
    else if ((TAF_SDC_SERVICE_STATUS_NO_SERVICE == enCsServiceStatus)
          || (TAF_SDC_SERVICE_STATUS_NO_SERVICE == enPsServiceStatus))
    {
        if ((TAF_PH_SERVICE_CS == pstSysCfg->ucSrvDomain)
         && (TAF_SDC_SERVICE_STATUS_LIMITED_SERVICE == enCsServiceStatus))
        {
            enRtType = TAF_SDC_REPORT_SRVSTA_LIMITED_SERVICE;
        }
        else
        {
            enRtType = TAF_SDC_REPORT_SRVSTA_NO_SERVICE;
        }
    }
    else
    {
        enRtType = TAF_SDC_REPORT_SRVSTA_LIMITED_SERVICE;
    }

    return enRtType;
}



VOS_VOID TAF_MMA_ProcLociStatusEvent(VOS_VOID)
{
   TAF_SDC_REPORT_SRVSTA_ENUM_UINT8    enStkServiceType;

   enStkServiceType = TAF_MMA_GetSrvTypeForStk();

   if (enStkServiceType != g_stMmsStkLocStaSysInfo.enServiceStatus)
   {
       if (VOS_FALSE == MN_MMA_GetRoamingBrokerFlg())
       {
          NAS_MMA_ReportLociStatus();
       }
       else
       {
           if (TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE == enStkServiceType)
           {
               NAS_MMA_ReportLociStatus();
           }
       }
   }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*--------------- STATUS内部数据处理函数  ---------------*/
/*=======================================================*/


VOS_VOID Sta_EventReport ( VOS_UINT32 ulOpID, VOS_UINT32 ulEventType )
{
    TAF_PHONE_EVENT_INFO_STRU      *pstPhoneEvent;
    VOS_UINT8                       n;          /* 循环次数 */
    VOS_UINT8                       ucNum;      /* PLMN有效数目*/
    TAF_PHONE_EVENT_PLMN_LIST_STRU         *pstPlmnListPhoneEvent;
    TAF_PHONE_EVENT                         ucPhoneEvent;
    VOS_UINT16                              ClientId;
    VOS_UINT8                               AppOpId;
    
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;
    TAF_SDC_REPORT_SRVSTA_ENUM_UINT8    enServiceStatus;

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();
    enServiceStatus   = TAF_SDC_GetServiceStatus();


    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        STA_TRACE(PS_PRINT_ERROR,"Sta_EventReport:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));
    /* 清除结构变量标志 */
    pstPhoneEvent->OP_CsServiceStatus    = 0;
    pstPhoneEvent->OP_PhoneError         = 0;
    pstPhoneEvent->OP_PlmnList           = 0;
    pstPhoneEvent->OP_PlmnReselMode      = 0;
    pstPhoneEvent->OP_PsServiceStatus    = 0;
    pstPhoneEvent->OP_RegPlmn            = 0;
    pstPhoneEvent->OP_Spare              = 0;
    /*增加上报事件选项初始化*/
    pstPhoneEvent->OP_CurRegSta          = 0;
    pstPhoneEvent->OP_Rssi               = 0;
    pstPhoneEvent->OP_BatteryPower       = 0;
    pstPhoneEvent->OP_Pin                = 0;
    pstPhoneEvent->OP_Srvst              = 0;

    /* 填写上报事件参数 */
    /* 必填参数 */
    pstPhoneEvent->OpId = (VOS_UINT8)ulOpID;         /*写入ulOpId*/
    pstPhoneEvent->PhoneEvent = (TAF_PHONE_EVENT)ulEventType;
    pstPhoneEvent->PhoneError = TAF_ERR_NO_ERROR;
    /* 可选参数设置 */
    if ( STA_OP_REGPLMN & g_StatusContext.ucOperateType )
    {
        pstPhoneEvent->OP_RegPlmn = 1;
        pstPhoneEvent->RegPlmn.Mcc= g_StatusContext.StaPlmnId.Mcc;
        pstPhoneEvent->RegPlmn.Mnc= g_StatusContext.StaPlmnId.Mnc;
        MMA_PlmnId2Bcd(&(pstPhoneEvent->RegPlmn));
    }

    if ( STA_OP_PLMNLIST & g_StatusContext.ucOperateType )
    {
        ucNum = g_StatusContext.PlmnList.ucPlmnNum;
        if (ucNum <= TAF_MAX_PLMN_NUM)
        {
            pstPhoneEvent->OP_PlmnList = 1;
            for ( n = 0; n < ucNum; n++ )
            {
                pstPhoneEvent->PlmnList.Plmn[n].Mcc
                    = g_StatusContext.PlmnList.Plmn[n].Mcc;
                pstPhoneEvent->PlmnList.Plmn[n].Mnc
                    = g_StatusContext.PlmnList.Plmn[n].Mnc;
                MMA_PlmnId2Bcd(&(pstPhoneEvent->PlmnList.Plmn[n]));
                pstPhoneEvent->PlmnList.PlmnInfo[n].PlmnStatus
                    = g_StatusContext.PlmnList.PlmnInfo[n].PlmnStatus;
                pstPhoneEvent->PlmnList.PlmnInfo[n].RaMode
                    = g_StatusContext.PlmnList.PlmnInfo[n].RaMode;
            }
            pstPhoneEvent->PlmnList.ucPlmnNum = g_StatusContext.PlmnList.ucPlmnNum;
        }
        else
        {
            pstPhoneEvent->OP_PhoneError = 1;
            pstPhoneEvent->PhoneError    = TAF_ERR_UNSPECIFIED_ERROR;
        }
    }

    if ( STA_OP_PLMNRESELMODE & g_StatusContext.ucOperateType )
    {
        pstPhoneEvent->OP_PlmnReselMode = 1;
        pstPhoneEvent->PlmnReselMode
            = (TAF_PLMN_RESEL_MODE_TYPE)g_StatusContext.ucReselMode;
    }

    if ( STA_OP_CSSERVICESTATUS & g_StatusContext.ucOperateType )
    {
        pstPhoneEvent->OP_CsServiceStatus = 1;

        /* CS域服务状态值 */
        pstPhoneEvent->CsServiceStatus
            = (TAF_PHONE_SERVICE_STATUS)enCsServiceStatus;
    }

    if ( STA_OP_PSSERVICESTATUS & g_StatusContext.ucOperateType )
    {
        pstPhoneEvent->OP_PsServiceStatus = 1;

        /* PS域服务状态值 */
        pstPhoneEvent->PsServiceStatus
            = (TAF_PHONE_SERVICE_STATUS)enPsServiceStatus;
    }

    if ( STA_OP_PHONEERROR & g_StatusContext.ucOperateType )
    {
        pstPhoneEvent->OP_PhoneError = 1;
        pstPhoneEvent->PhoneError    = g_StatusContext.PhoneError;
    }


    if ( STA_OP_PHONESERVICESTATUS & g_StatusContext.ucOperateType )
    {
        pstPhoneEvent->OP_Srvst      = 1;
        pstPhoneEvent->ServiceStatus = enServiceStatus;


    }

    /* LIST搜网事件被打断的情况在有卡回复空列表,无卡回复ERROR */
    if ((STA_OP_PHONEERROR == g_StatusContext.ucOperateType)
     && (TAF_PH_EVT_PLMN_LIST_CNF == ulEventType))
    {
        pstPlmnListPhoneEvent = (TAF_PHONE_EVENT_PLMN_LIST_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_PLMN_LIST_STRU));

        if (VOS_NULL_PTR == pstPlmnListPhoneEvent)
        {
            PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
            STA_TRACE(PS_PRINT_ERROR,"Sta_EventReport:ERROR:ALLOC MEMORY FAIL.");
            return;
        }

        PS_MEM_SET(pstPlmnListPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_PLMN_LIST_STRU));

        /* 检查SIM卡状态 */
        if (TAF_ERR_NO_ERROR == MMA_CheckUsimStatusForPlmnSel())
        {
            /*APP发起的过程最大Ti是1*/
            if (ulOpID <= TAF_MAX_STATUS_TI)
            {
                /*通过Ti获取ClientId,CallId*/
                if (TAF_SUCCESS != MMA_GetIdByTi(TAF_MMA, (VOS_UINT8)ulOpID, &ClientId, &AppOpId, &ucPhoneEvent))
                {
                    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
                    PS_MEM_FREE(WUEPS_PID_MMA, pstPlmnListPhoneEvent);

                    MMA_WARNINGLOG("Sta_EventReport():WARNING:Invoke Taf_GetIdByTi failed");
                    return;
                }
            }
            else
            {   /*是STATUS主动发起的事件*/
                AppOpId = MMA_OP_ID_INTERNAL;
                /*ClientId置为广播值:高字节的低4bit类型是MUX_PORT_BUTT*/
                ClientId = MMA_CLIENTID_BROADCAST;
            }

            /* 构造LIST搜网回复事件 */
            pstPlmnListPhoneEvent->PhoneEvent   = (TAF_PHONE_EVENT)ulEventType;
            pstPlmnListPhoneEvent->ulPlmnNum    = 0;
            pstPlmnListPhoneEvent->OpId         = (VOS_UINT8)ulOpID;         /*写入ulOpId*/
            pstPlmnListPhoneEvent->ClientId     = ClientId;
            pstPlmnListPhoneEvent->OpId         = AppOpId;

            /* 调用电话管理上报函数 */
            MN_PH_SendMsg(pstPlmnListPhoneEvent->ClientId,(VOS_UINT8*)pstPlmnListPhoneEvent,sizeof(TAF_PHONE_EVENT_PLMN_LIST_STRU));

            PS_MEM_FREE(WUEPS_PID_MMA, pstPlmnListPhoneEvent);
            PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

            return;
        }

        pstPhoneEvent->OP_PhoneError = MMA_TRUE;

        pstPhoneEvent->PhoneError = TAF_ERR_USIM_SIM_CARD_NOTEXIST;

        /* 调用电话管理事件上报函数 向APP发送应答事件 */
        Taf_PhoneEvent (pstPhoneEvent);

        PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

        PS_MEM_FREE(WUEPS_PID_MMA, pstPlmnListPhoneEvent);

        return;
    }

    /* 调用电话管理事件上报函数 向APP发送应答事件*/
    Taf_PhoneEvent (pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    
    
    return ;    /* 正常返回 */
}

/*************************************************
  Function:
    Sta_IsFsmSubValid
  Description:
     判断子状态是否匹配FSM主状态
  Calls:                 被本函数调用的函数清单
  Called By:             调用本函数的函数清单
  Input:
    VOS_UINT8   ucFsmSub;   子状态值
    VOS_UINT32  ulFsm;      FSM状态值
  Output:
    无
  Return:
    STA_TRUE,  有效的子状态值，匹配；
    STA_FALSE, 无效的子状态值，不匹配；
  Others:
  1.日    期   : 2007年07月24日
    作    者   : l60022475
    修改内容   : A32D12535
*************************************************/
VOS_BOOL Sta_IsFsmSubValid ( VOS_UINT8 ucFsmSub, VOS_UINT32 ulFsm )
{
    switch ( ulFsm )
    {
        case STA_FSM_RESTART:
        case STA_FSM_STOP:
        case STA_FSM_PLMN_LIST:
        case STA_FSM_PLMN_SEL:
            /* ==>A32D12535 */
            if ( (STA_FSMSUB_MONO == ucFsmSub)
               ||(STA_FSM_RESTART == ucFsmSub)
                )
            /* <==A32D12535 */
            {
                return STA_TRUE;
            }
            return STA_FALSE;

        /*
          增加CS&PS同时DETACH或ATTACH状态的合法性；
          modified by roger in 2005-07-11
        */
        case STA_FSM_DE_ATTACH:
            if ( (STA_FSMSUB_ATTACH_CS == ucFsmSub)
              || (STA_FSMSUB_ATTACH_PS == ucFsmSub)
              || (STA_FSMSUB_DETACH_CS == ucFsmSub)
              || (STA_FSMSUB_DETACH_PS == ucFsmSub)
              || (STA_FSMSUB_ATTACH_CSPS == ucFsmSub)
              || (STA_FSMSUB_DETACH_CSPS == ucFsmSub) )
            {
                return STA_TRUE;
            }
            return STA_FALSE;

        case STA_FSM_PLMN_RESEL:
            if ( (STA_FSMSUB_PLMN_RESEL_AUTO == ucFsmSub)
              || (STA_FSMSUB_PLMN_RESEL_MANUAL == ucFsmSub) )
            {
                return STA_TRUE;
            }
            return STA_FALSE;

        /* 内部函数调用时使用的中间状态，允许为有效状态 */
        case STA_FSM_NO_UPDATE:
        /* FSM的稳态，对子状态值没有匹配要求 */
        case STA_FSM_ENABLE:
        case STA_FSM_NULL:
            return STA_TRUE;

        default :
            return STA_FALSE;
    }
}

/*************************************************
  Function:
    Sta_IsInvalidOpId
  Description:
    是否是无效的OPID
  Calls:                  被本函数调用的函数清单
  Called By:              调用本函数的函数清单
  Input:
    VOS_UINT32  ulOpId;      命令标识
  Output:
    无
  Return:
    STA_TRUE,   指定的ulOpId无效；
    STA_FALSE,  指定的ulOpId有效；
  Others:
*************************************************/
VOS_BOOL Sta_IsInvalidOpId ( VOS_UINT32 ulOpId )
{
    if ( STA_OPID_INSIDE == ulOpId )
    {
        return STA_FALSE;
    }

    if ( ulOpId <= STA_OPID_MAX )
    {
        return STA_FALSE;
    }

    return STA_TRUE;
}


/* BEGIN: Deleted by liuyang, 2005/10/26 */
/*此函数已经作废*/
/*************************************************
  Function:
    Sta_IsDualFsmSub
  Description:
    是否存在两个子状态同时有效，针对同一个OPID
  Calls:                  被本函数调用的函数清单
  Called By:              调用本函数的函数清单
  Input:
    VOS_UINT32  ulOpId;      命令标识
  Output:
    无
  Return:
    STA_TRUE,   存在双重有效子状态；
    STA_FALSE,  不存在双重有效子状态；
  Others:
*************************************************/
VOS_UINT32 Sta_IsDualFsmSub ( VOS_VOID )
{
    /*modified by liuyang id:48197 date:2005-10-5 for V100R001*/
    /*当前所有的双重子状态的OpId都是一样的，
    因此判断当子状态标志位都为相同时,此状态为双重子状态*/
    if  ( g_StatusContext.aucFsmSubFlag[0]
          != g_StatusContext.aucFsmSubFlag[1])
    {
        return STA_FALSE;
    }
    /* 子状态OPID不相同，不是双重子状态 */
    /*if ( g_StatusContext.aFsmSub[0].ulOpId
        != g_StatusContext.aFsmSub[1].ulOpId )
    {
        return STA_FALSE;
    }
    */
    /*modified by liuyang id:48197 date:2005-10-5 for V100R001*/
    return STA_TRUE;
}
/* END:   Deleted by liuyang, 2005/10/26 */

/*************************************************
  Function:
    Sta_FindFsmSubBySubSta
  Description:
     查找指定命令操作标识的对应子状态索引号
  Calls:                  被本函数调用的函数清单
  Called By:              调用本函数的函数清单
  Input:
    VOS_UINT32  ulOpId;       命令标识
  Output:
    无
  Return:
    STA_FSMSUB_INDEX_NULL, 无效的子状态索引值；
    0< ret <STA_TIMER_MAX, 允许的索引值ret；
  Others:
*************************************************/
/*
VOS_UINT32 Sta_FindFsmSubBySubSta ( VOS_UINT32 ulOpId )
{
    VOS_UINT8       ucIndex;
    VOS_UINT8       n;

    ucIndex = STA_FSMSUB_INDEX_NULL;
    for ( n = 0; n < STA_TIMER_MAX; n++ )
    {
        if ( ulOpId == g_StatusContext.aFsmSub[n].ulOpId )
        {
            ucIndex = n;
            break;
        }
    }

    return ucIndex;
}
*/
/*************************************************
  Function:
    Sta_FindFsmSubBySubSta
  Description:
     查找指定指定子状态的对应子状态索引号
  Calls:                  被本函数调用的函数清单
  Called By:              调用本函数的函数清单
  Input:
    VOS_UINT8  ucFsmSubSta;       命令标识
  Output:
    无
  Return:
    STA_FSMSUB_INDEX_NULL, 无效的子状态索引值；
    0< ret <STA_TIMER_MAX, 允许的索引值ret；
  Others:
*************************************************/
VOS_UINT8 Sta_FindFsmSubBySubSta ( VOS_UINT8 ucFsmSubSta )
{
    VOS_UINT8       ucIndex;
    VOS_UINT8       n;

    ucIndex = STA_FSMSUB_INDEX_NULL;

    for ( n = 0; n < STA_TIMER_MAX; n++ )
    {
        if (ucFsmSubSta == g_StatusContext.aFsmSub[n].ucFsmStateSub)
        {
            ucIndex = n;
            break;
        }
    }

    return ucIndex;
}

/*************************************************
  Function:
    Sta_FindFsmSub
  Description:
     查找对应子状态的索引号
  Calls:                  被本函数调用的函数清单
  Called By:              调用本函数的函数清单
  Input:
    VOS_UINT8  ucFsmSub;      命令标识
  Output:
    无
  Return:
    STA_FSMSUB_INDEX_NULL, 无效的子状态索引值；
    0< ret <STA_TIMER_MAX, 允许的索引值ret；
  Others:
*************************************************/
VOS_UINT32 Sta_FindFsmSub ( VOS_UINT8 ucFsmSub )
{
    VOS_UINT8       ucIndex;
    VOS_UINT8       n;

    ucIndex = STA_FSMSUB_INDEX_NULL;    /* 无效索引号 */
    for ( n = 0; n < STA_TIMER_MAX; n++ )
    {
        /* 单种子状态属性查找 */
        if ( ucFsmSub <= STA_FSMSUB_DETACH_PS )
        {
            if ( ucFsmSub == g_StatusContext.aFsmSub[n].ucFsmStateSub )
            {
                ucIndex = n;
                break;
            }
        }

        /* CS&PS ATTACH子状态属性查找 */
        if ( STA_FSMSUB_ATTACH_CSPS == ucFsmSub )
        {
            if ( (STA_FSMSUB_ATTACH_CS
                == g_StatusContext.aFsmSub[n].ucFsmStateSub)
              || (STA_FSMSUB_ATTACH_PS
                == g_StatusContext.aFsmSub[n].ucFsmStateSub) )
            {
                ucIndex = n;
                break;
            }
        }

        /* CS&PS DETACH子状态属性查找 */
        if ( STA_FSMSUB_DETACH_CSPS == ucFsmSub )
        {
            if ( (STA_FSMSUB_DETACH_CS
                == g_StatusContext.aFsmSub[n].ucFsmStateSub)
              || (STA_FSMSUB_DETACH_PS
                == g_StatusContext.aFsmSub[n].ucFsmStateSub) )
            {
                ucIndex = n;
                break;
            }
        }
    }

    return ucIndex;
}

/*************************************************
  Function:
    Sta_FindIdleFsmSub
  Description:
     查找指定子状态值的对应的空闲资源索引号
  Calls:                  被本函数调用的函数清单
  Called By:              调用本函数的函数清单
  Input:
    VOS_UINT32  ulOpId;      命令标识
  Output:
    无
  Return:
    STA_FSMSUB_INDEX_NULL, 无效的子状态索引值；
    0< ret <STA_TIMER_MAX, 允许的索引值ret；
  Others:
*************************************************/
VOS_UINT32 Sta_FindIdelFsmSub ( VOS_UINT8 ucSubState )
{
    VOS_UINT8       ucRetIndex;
    VOS_UINT8       n;                  /* 循环次数 */

    ucRetIndex = STA_FSMSUB_INDEX_NULL;

    /* 查找空闲子状态 */
    for ( n = 0; n < STA_TIMER_MAX; n++ )
    {
        if ( STA_FSMSUB_NULL == g_StatusContext.aucFsmSubFlag[n] )
        {
            ucRetIndex = n;
            break;
        }
    }

    /* 无可用的子状态资源，直接返回无效索引值 */
    if ( STA_FSMSUB_INDEX_NULL == ucRetIndex )
    {
        return STA_FSMSUB_INDEX_NULL;      /* 返回无效值 */
    }

    /* 可用的子状态资源不足，直接返回无效索引值 */
    if ( (STA_FSMSUB_ATTACH_CSPS == ucSubState)
      || (STA_FSMSUB_DETACH_CSPS == ucSubState) )
    {
        /*modified by liuyang id:48197 date:2005-10-5 for V100R001*/
        /*if ( 0 != ucRetIndex )*/
        if ((STA_FSMSUB_NULL != g_StatusContext.aucFsmSubFlag[0])
          ||(STA_FSMSUB_NULL != g_StatusContext.aucFsmSubFlag[1]))
        /*modified by liuyang id:48197 date:2005-10-5 for V100R001*/
        {
            return STA_FSMSUB_INDEX_NULL;   /* 返回无效值 */
        }
        else
        {
            return ucRetIndex;              /* 返回一个有效的索引值 */
        }
    }

    /* 特殊状态直接返回空闲子状态索引 */
    if ( (STA_FSMSUB_NULL == ucSubState)
        || (STA_FSMSUB_OK == ucSubState)
        || (STA_FSMSUB_MONO == ucSubState) )
    {
        return ucRetIndex;
    }

    /* 非空闲的子状态只允许存在一个 */
    for ( n = 0; n < STA_TIMER_MAX; n++ )
    {
        if ( ucSubState == g_StatusContext.aucFsmSubFlag[n] )
        {
            return STA_FSMSUB_INDEX_NULL;   /* 返回无效值 */
        }
    }

    return ucRetIndex;
}
VOS_UINT32 Sta_UpdateFsmFlag ( VOS_UINT8 ucFsmSub,
                           VOS_UINT32 ulFsm,
                           VOS_UINT32 ulType )
{
    VOS_UINT8                           n;                  /* 循环次数*/
    VOS_UINT8                           ucIndex;            /* 子状态索引*/

    ucIndex = STA_FSMSUB_INDEX_NULL;

    /* 按照更新类型进行操作 */
    switch ( ulType )
    {
        case STA_UPDATE_SETNEW:
            if (  STA_FSM_NO_UPDATE == ulFsm )
            {
               return STA_SUCCESS;
            }
            /* 双重子状态同时更新 */
            if ((STA_FSMSUB_ATTACH_CSPS == ucFsmSub)
              ||(STA_FSMSUB_DETACH_CSPS == ucFsmSub))
            {
                for ( n = 0; n < STA_TIMER_MAX; n++ )
                {
                    g_StatusContext.aFsmSub[n].ucFsmStateSub
                        = STA_FSMSUB_NULL;
                    g_StatusContext.aucFsmSubFlag[n]
                        = STA_FSMSUB_NULL;
                    /*将当前所有的OpId设置为无效*/
                    g_StatusContext.aFsmSub[n].ulOpId = STA_OPID_INVALID;
                    /*清除当前操作标志*/
                    g_StatusContext.aucStaPhoneOperator[n] = STA_FSMSUB_NULL;
                }
                g_StatusContext.ulFsmState = ulFsm;
                return STA_SUCCESS;
            }
            /* 双重子状态的单一更新 */
            if ( STA_FSM_DE_ATTACH == ulFsm )
            {
                /*替换查找子状态索引函数*/
                for ( n = 0; n < STA_TIMER_MAX; n++ )
                {
                    if ( g_StatusContext.aFsmSub[n].ulOpId != STA_OPID_INVALID )
                    {
                        ucIndex = n;
                        break;
                    }
                    else
                    {
                        ucIndex = STA_FSMSUB_INDEX_NULL;
                    }
                }

                if ( STA_FSMSUB_INDEX_NULL == ucIndex )
                {/* 无效的子状态 */
                    STA_TRACE (STA_WARNING_PRINT,
                        "Sta_UpdateFsmFlag():WARNING:invalid FSM substate  ");
                    return STA_ERROR;
                }
                g_StatusContext.aFsmSub[ucIndex].ucFsmStateSub = ucFsmSub;
                g_StatusContext.aucFsmSubFlag[ucIndex]         = ucFsmSub;
                /*设置OpID无效值*/
                g_StatusContext.aFsmSub[ucIndex].ulOpId        = STA_OPID_INVALID;
                /*清除当前操作标志*/
                g_StatusContext.aucStaPhoneOperator[ucIndex]   = STA_FSMSUB_NULL;
                return STA_SUCCESS;
            }
            /* 单重子状态的更新 */
            g_StatusContext.aFsmSub[0].ucFsmStateSub  = ucFsmSub;
            g_StatusContext.aucFsmSubFlag[0]          = ucFsmSub;
            break;

        /* 单重子状态的更新,并清除定时器等资源 */
        case STA_UPDATE_MONO_CLEAR:

            if (VOS_NULL_PTR != g_StatusContext.ahStaTimer[0])
            {
                if (VOS_OK != NAS_StopRelTimer(WUEPS_PID_MMA,
                                 g_StatusContext.aFsmSub[0].TimerName,
                                &g_StatusContext.ahStaTimer[0]))
                {
                    STA_TRACE(STA_WARNING_PRINT, "Sta_UpdateFsmFlag():WARNING:NAS_StopRelTimer failed!");
                }
            }
            g_StatusContext.aFsmSub[0].ucFsmStateSub  = ucFsmSub;
            g_StatusContext.aucFsmSubFlag[0]          = ucFsmSub;
            g_StatusContext.aFsmSub[0].TimerName      = STA_TIMER_NULL;
            g_StatusContext.ahStaTimer[0]             = VOS_NULL_PTR;



            if ( STA_FSM_INVALID != ulFsm )
            {
                g_StatusContext.ulFsmState = ulFsm;
            }

            /*设置OpID无效值*/
            g_StatusContext.aFsmSub[0].ulOpId        = STA_OPID_INVALID;
            /*清除当前操作标志*/
            g_StatusContext.aucStaPhoneOperator[0]   = STA_FSMSUB_NULL;
            break;

        /* 双重子状态同时更新, 并清除相关资源，如定时器 */
        case STA_UPDATE_DUAL_CLEAR:

            for ( n = 0; n < STA_TIMER_MAX; n++ )
            {
                if (VOS_NULL_PTR != g_StatusContext.ahStaTimer[n])
                {
                    if (VOS_OK != NAS_StopRelTimer(WUEPS_PID_MMA,
                                     g_StatusContext.aFsmSub[n].TimerName,
                                    &g_StatusContext.ahStaTimer[n]))
                    {
                        STA_TRACE(STA_WARNING_PRINT, "Sta_UpdateFsmFlag():WARNING:NAS_StopRelTimer failed!");
                    }
                }
                g_StatusContext.aFsmSub[n].ucFsmStateSub  = ucFsmSub;
                g_StatusContext.aucFsmSubFlag[n]          = ucFsmSub;
                g_StatusContext.aFsmSub[n].TimerName      = STA_TIMER_NULL;
                g_StatusContext.ahStaTimer[n]             = VOS_NULL_PTR;


                /*设置OpID无效值*/
                g_StatusContext.aFsmSub[n].ulOpId        = STA_OPID_INVALID;

                /*清除当前操作标志*/
                g_StatusContext.aucStaPhoneOperator[n]   = STA_FSMSUB_NULL;
            }
            if ( (STA_FSM_INVALID != ulFsm)
                && (STA_FSM_NO_UPDATE != ulFsm) )
            {
                g_StatusContext.ulFsmState = ulFsm;
            }
            break;

       /* 双重子状态的单一更新，并清除定时器等资源 */
        case STA_UPDATE_SINGL_CLEAR:
            ucIndex = (VOS_UINT8)Sta_FindFsmSub (ucFsmSub);

            if ( STA_FSMSUB_INDEX_NULL == ucIndex )
            {/* 无效的子状态 */
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_UpdateFsmFlag():WARNING:invalid FSM substate");
                return STA_ERROR;
            }

            if (VOS_NULL_PTR != g_StatusContext.ahStaTimer[ucIndex])
            {
                if (VOS_OK != NAS_StopRelTimer(WUEPS_PID_MMA,
                                 g_StatusContext.aFsmSub[ucIndex].TimerName,
                                &g_StatusContext.ahStaTimer[ucIndex]))
                {
                    STA_TRACE(STA_WARNING_PRINT, "Sta_UpdateFsmFlag():WARNING:NAS_StopRelTimer failed!");
                }
            }

            g_StatusContext.aFsmSub[ucIndex].ucFsmStateSub  = STA_FSMSUB_NULL;
            g_StatusContext.aucFsmSubFlag[ucIndex]          = STA_FSMSUB_NULL;

            g_StatusContext.aFsmSub[ucIndex].TimerName      = STA_TIMER_NULL;
            g_StatusContext.ahStaTimer[ucIndex]             = VOS_NULL_PTR;


            /*清除当前操作标志*/
            g_StatusContext.aucStaPhoneOperator[ucIndex]   = STA_FSMSUB_NULL;

            /*
              增加在STA_FSM_NO_UPDATE时不更新状态机主状态值；
            */
            if ( (STA_FSM_INVALID != ulFsm)
                && (STA_FSM_NO_UPDATE != ulFsm) )
            {
                g_StatusContext.ulFsmState = ulFsm;
            }

            /*设置OpID无效值*/
            g_StatusContext.aFsmSub[ucIndex].ulOpId        = STA_OPID_INVALID;
            break;

        /* 未使用的状态，返回错误 */
        default:
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_UpdateFsmFlag():WARNING:unused state, do nothing  ");
            return STA_ERROR;
    }

    return STA_SUCCESS;
}


VOS_UINT32 Sta_UpdateData ( STA_PHONE_DATA_S *pData )
{
    VOS_UINT32  ulRet;
    VOS_UINT8   ucFsmSub;
    VOS_UINT32  ulFsm;
    VOS_UINT8   ucFsmFlag;

    /* 参数指针有效性由调用者保证，默认有效 */
    ucFsmSub  = pData->ucFsmStateSub;
    ulFsm     = pData->ulFsmState;
    ucFsmFlag = pData->ucFsmUpdateFlag;

    /* 判断子状态值是否有效 */
    if ( !Sta_IsFsmSubValid (ucFsmSub,ulFsm) )
    {
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_UpdateData():WARNING:unmatched FSM state  ");

        return STA_ERROR;     /* 错误返回 */
    }

    /* 按照子状态的更新模式来处理 */
    switch ( pData->ucFsmSubOperate )
    {
        /* 设置FSM状态标志量，开始新的子状态 */
        case STA_FSMSUB_NEW:
            g_StatusContext.ulCurOpId = pData->ulOpId;  /*记录命令标识号*/
            ulRet = Sta_UpdateFsmFlag (ucFsmSub, ulFsm, STA_UPDATE_SETNEW);
            if ( STA_ERROR == ulRet )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_UpdateData():WARNING:Sta_UpdateFsmFlag failed  ");
            }
            break;

        /* 刷新子状态描述信息 */
        case STA_FSMSUB_UPDATE:
            g_StatusContext.ulNewOpId = pData->ulOpId;  /* 本次更新的标识号*/
            break;

        /* 子状态操作完成，清除子状态标志 */
        case STA_FSMSUB_FINISH:
            ulRet = Sta_UpdateFsmFlag (ucFsmSub, ulFsm, ucFsmFlag);
            if ( STA_ERROR == ulRet )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_UpdateData():WARNING:Sta_UpdateFsmFlag failed  ");
            }
            break;

        /* 其他模式不支持，直接返回 */
        default:
            STA_TRACE (STA_WARNING_PRINT,
                "Sta_UpdateData():WARNING:invalid operate ");
            return STA_ERROR;
    }

    /* 更新STATUS业务信息记录 */
    g_StatusContext.StaPlmnId.Mcc       = pData->PlmnId.Mcc;
    g_StatusContext.StaPlmnId.Mnc       = pData->PlmnId.Mnc;

    g_StatusContext.ulMmProc            = pData->ulMmProc ;
    g_StatusContext.ucDomainFlag        = pData->ucDomainFlag;
    /*保证每次只对有效的域返回的服务状态更新全局变量*/
    switch ( pData->ucDomainFlag )
    {
        case MMA_MMC_SRVDOMAIN_CS :
            TAF_SDC_SetCsServiceStatus((VOS_UINT8)pData->ulCsServiceStatus);
            g_StatusContext.ulCsCause = pData->ulCsCause;
            break;
        case MMA_MMC_SRVDOMAIN_PS:
            TAF_SDC_SetPsServiceStatus((VOS_UINT8)pData->ulPsServiceStatus);
            g_StatusContext.ulPsCause = pData->ulPsCause;
            break;
        case MMA_MMC_SRVDOMAIN_CS_PS:
            TAF_SDC_SetCsServiceStatus((VOS_UINT8)pData->ulCsServiceStatus);
            TAF_SDC_SetPsServiceStatus((VOS_UINT8)pData->ulPsServiceStatus);
            g_StatusContext.ulCsCause = pData->ulCsCause;
            g_StatusContext.ulPsCause = pData->ulPsCause;
            break;
        default :
            break;
    }

    return STA_SUCCESS;       /* 正常返回 */
}




/*
    在进入PLMN的稳态之后，通过MMC的上报信息更新
    增添两个函数
    modified by liuyang id:48197 date:2005-9-5 for V200R001
*/
/*****************************************************************************
 函 数 名  : ClearCurPlmnId
 功能描述  : 将PLMNID置零，即为无效PLMN
 输出参数  : 无
 返 回 值  : VOS_UINT8
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年9月5日
    作    者   : liuyang
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID   Sta_ClearCurPlmnId(TAF_PLMN_ID_STRU *pstNewPlmnId)
{
    pstNewPlmnId->Mcc  = 0;
    pstNewPlmnId->Mnc  = 0;
}

/*****************************************************************************
 函 数 名  : Sta_InitCurPlmnId
 功能描述  : 将当前PLMNid装入当前全局变量
 输入参数  : TAF_PLMN_ID_STRU NewPlmnId
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年9月5日
    作    者   : liuyang
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID   Sta_InitCurPlmnId(TAF_PLMN_ID_STRU    stSrcPlmnId,
                             TAF_PLMN_ID_STRU   *pstDstPlmnId)
{
    *pstDstPlmnId = stSrcPlmnId;
}



/*****************************************************************************
 函 数 名  : Sta_StopAllRunningTimer
 功能描述  : 停止所有运行的定时器，并清除定时器状态变量名相关的变量
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年1月19日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 Sta_StopAllRunningTimer()
{
    VOS_UINT32      ulI;
    VOS_UINT32      ulRet = VOS_OK;

    for ( ulI = 0; ulI < STA_TIMER_MAX; ulI++ )
    {
        if (STA_TIMER_NULL != g_StatusContext.aFsmSub[ulI].TimerName )
        {
            ulRet = VOS_StopRelTimer (&g_StatusContext.ahStaTimer[ulI]);
            if ( VOS_OK != ulRet )
            {
                STA_TRACE (STA_WARNING_PRINT,
                    "Sta_StopAllRunningTimer():WARNING:VOS_StopRelTimer failed ");
            }
            g_StatusContext.aFsmSub[ulI].TimerName = STA_TIMER_NULL;
        }
    }
    return ulRet;
}

/* Sta_ResetFsmFlg */


/* BEGIN: Added by liuyang id:48197, 2006/3/13   PN:A32D02500*/

VOS_UINT32 Sta_AttachByModeService(VOS_UINT32  ulOpId)
{
    VOS_UINT32                          ulAttachType;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucAutoAttachFlg;
    NAS_NVIM_AUTOATTACH_STRU            stAutoattachFlag;
    VOS_UINT32                          ulLength;

    ulLength = 0;
    PS_MEM_SET(&stAutoattachFlag, 0x00, sizeof(NAS_NVIM_AUTOATTACH_STRU));
    NV_GetLength(en_NV_Item_Autoattach, &ulLength);
    if (NV_OK == NV_Read(en_NV_Item_Autoattach,
                         (VOS_VOID*)(&stAutoattachFlag),
                         ulLength))
    {
        STA_TRACE1(STA_INFO_PRINT,
            "Sta_AttachByModeService:INFO:Read Auto Attach",(VOS_INT32)stAutoattachFlag.usAutoattachFlag);

        if (1 == stAutoattachFlag.usAutoattachFlag)
        {
            ucAutoAttachFlg = MMA_AUTO_ATTACH_ENABLE;
        }
        else
        {
            ucAutoAttachFlg = MMA_AUTO_ATTACH_DISABLE;
        }
    }
    else
    {
        ucAutoAttachFlg = MMA_AUTO_ATTACH_ENABLE;
    }
    switch (g_StatusContext.ucModeService)
    {
        case TAF_PH_MS_CLASS_A:
            ulAttachType = STA_ATTACH_TYPE_GPRS_IMSI;
            if (MMA_AUTO_ATTACH_DISABLE == ucAutoAttachFlg)
            {
                 ulAttachType = STA_ATTACH_TYPE_IMSI;
            }
            break;

        case TAF_PH_MS_CLASS_CG:
            ulAttachType = STA_ATTACH_TYPE_GPRS;
            if (MMA_AUTO_ATTACH_DISABLE == ucAutoAttachFlg)
            {
                 STA_TRACE(STA_NORMAL_PRINT,
                           "Sta_AttachByModeService:WARNING:Auto Attach Not Allowed!");
                 return STA_ERROR;
            }
            break;

        case TAF_PH_MS_CLASS_CC:
            ulAttachType = STA_ATTACH_TYPE_IMSI;
            break;

        case TAF_PH_MS_CLASS_NULL:
            g_StatusContext.ulFsmState = STA_FSM_ENABLE;
            return STA_SUCCESS;

        default:
            STA_TRACE (STA_WARNING_PRINT,
            "Sta_AttachByModeService():WARNING:g_StatusContext.ucModeService error");
            return STA_ERROR;
    }
    ulRet      = Sta_Attach (ulOpId, ulAttachType, TAF_MMA_ATTACH_REASON_INITIAL);
    if ( STA_ERROR == ulRet )
    {
        STA_TRACE (STA_WARNING_PRINT,
            "Sta_AttachByModeService():WARNING:Sta_Attach failed");
    }
    return ulRet;
}




VOS_UINT8 Sta_GetStatusForSat(VOS_UINT32 ulServiceStatus)
{
    VOS_UINT8 ucSatServiceStatus;

    switch (ulServiceStatus)
    {
        case MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE:
            ucSatServiceStatus = STA_SAT_LOCATION_NORMAL_SERVICE;
            break;

        case MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE :
        case MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE_REGION :
        case MMA_MMC_SERVICE_STATUS_NO_IMSI :
            ucSatServiceStatus = STA_SAT_LOCATION_LIMIT_SERVICE;
            break;

        case MMA_MMC_SERVICE_STATUS_NO_SERVICE:
            ucSatServiceStatus = STA_SAT_LOCATION_NO_SERVICE;
            break;

        default :
            ucSatServiceStatus = STA_SAT_LOCATION_NO_SERVICE;
            break;
    }

    return ucSatServiceStatus;
}


VOS_VOID Sta_GetLocInfoForSat(
    MMA_MMC_PLMN_ID_STRU                stPlmnId,
    VOS_UINT16                          usLac,
    VOS_UINT16                          usCellId,
    VOS_UINT8                          *pucLocInfo
)
{
    VOS_UINT8 ucSn = 0;

    pucLocInfo[ucSn]    = (VOS_UINT8)(stPlmnId.ulMcc & 0x0000000f);
    pucLocInfo[ucSn++] |= (VOS_UINT8)((stPlmnId.ulMcc & 0x00000f00) >> 4);
    pucLocInfo[ucSn]    = (VOS_UINT8)((stPlmnId.ulMcc & 0x000f0000) >> 16);
    pucLocInfo[ucSn++] |= (VOS_UINT8)((stPlmnId.ulMnc & 0x000f0000) >> 12);
    pucLocInfo[ucSn]    = (VOS_UINT8)(stPlmnId.ulMnc & 0x0000000f);
    pucLocInfo[ucSn++] |= (VOS_UINT8)((stPlmnId.ulMnc & 0x00000f00) >> 4);

    pucLocInfo[ucSn++]  = (VOS_UINT8)(usLac >> 8);
    pucLocInfo[ucSn++]  = (VOS_UINT8)(usLac & 0x000000ff);

    pucLocInfo[ucSn++]  = (VOS_UINT8)(usCellId >> 8);
    pucLocInfo[ucSn++]  = (VOS_UINT8)(usCellId & 0x000000ff);

    for (ucSn = 0; ucSn < STA_SAT_LOCATION_INFORMATON_LEN; ucSn ++)
    {
        STA_TRACE1(STA_INFO_PRINT,
                   "Sta_GetLocInfoForSat():INFO:Locinfo:",pucLocInfo[ucSn]);
    }

    return;
}




VOS_VOID Mma_ModeChgReport(
    TAF_SDC_SYS_MODE_ENUM_UINT8         enCurNetWork,
    VOS_UINT8                           ucSysSubMode
)
{

    VOS_UINT8                            ucModeChangeFlg;

    ucModeChangeFlg = TAF_MMA_IsModeChange(enCurNetWork, ucSysSubMode);

    if (VOS_TRUE == ucModeChangeFlg)
    {
        if (TAF_SDC_SYS_MODE_WCDMA == enCurNetWork)
        {
            if (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
            {
                /* G->W下，先上报缺省模式: ^MODE:5,4,后续上报时根据RRMM_AT_MSG_IND更新 */
                Sta_ModeChangeEventReport(TAF_PH_INFO_WCDMA_RAT, TAF_SYS_SUBMODE_WCDMA);
            }
            else
            {
                /* G->TD下，先上报缺省模式: ^MODE:15,8,后续上报时根据RRMM_AT_MSG_IND更新 */
                Sta_ModeChangeEventReport(TAF_PH_INFO_TD_SCDMA_RAT, TAF_SYS_SUBMODE_TD_SCDMA);
            }

            /* 更新全局变量中保存的UtranMode */
            g_StatusContext.ucPreUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();
        }
        else if (TAF_SDC_SYS_MODE_GSM == enCurNetWork)
        {
            Sta_ModeChangeEventReport(TAF_PH_INFO_GSM_RAT, ucSysSubMode);
        }
        else if (TAF_SDC_SYS_MODE_LTE == enCurNetWork)
        {
            Sta_ModeChangeEventReport(TAF_PH_INFO_LTE_RAT, ucSysSubMode);
        }
        else
        {
            Sta_ModeChangeEventReport(TAF_PH_INFO_NONE_RAT, TAF_SYS_SUBMODE_NONE);
        }
    }

    return;
}

TAF_PH_RAT_TYPE_ENUM_UINT8 TAF_MMA_ConvertMmaRatToStkRat(
                                         TAF_SDC_SYS_MODE_ENUM_UINT8 enMmaRatType
                                         )
{
    switch (enMmaRatType)
    {
        case TAF_SDC_SYS_MODE_GSM:
             return TAF_PH_RAT_GSM;

        case TAF_SDC_SYS_MODE_WCDMA:
             return TAF_PH_RAT_WCDMA;

        case TAF_SDC_SYS_MODE_LTE:
             return TAF_PH_RAT_LTE;

        default:
            MN_ERR_LOG("TAF_MMA_ConvertMmaRatToStkRat: The input is invalid");
            return TAF_PH_RAT_BUTT;
    }
}


/* 删除TAF_MMA_SndStkAccTechChgEvt */

VOS_UINT32 Sta_MmInfo(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent;
    MMC_MMA_MM_INFO_IND_STRU           *pstMmInfo;

    pstMmInfo = (MMC_MMA_MM_INFO_IND_STRU*)pstMsg;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("Sta_MmInfo:ERROR:ALLOC MEMORY FAIL.");
        return VOS_TRUE;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    pstPhoneEvent->OP_MmInfo    = MMA_TRUE;
    pstPhoneEvent->PhoneEvent   = TAF_PH_EVT_MM_INFO_IND;

    PS_MEM_CPY(&gstMmaValue.stOperatorNameInfo, &pstMmInfo->stName, sizeof(gstMmaValue.stOperatorNameInfo));

    PS_MEM_CPY(pstPhoneEvent->stMmInfo.ucLSAID, pstMmInfo->aucLSAID, sizeof(pstPhoneEvent->stMmInfo.ucLSAID));
    pstPhoneEvent->stMmInfo.stUniversalTimeandLocalTimeZone  = pstMmInfo->stUniversalTimeandLocalTimeZone;
    pstPhoneEvent->stMmInfo.cLocalTimeZone                   = pstMmInfo->cLocalTimeZone;
    pstPhoneEvent->stMmInfo.ucDST                            = pstMmInfo->ucDST;
    pstPhoneEvent->stMmInfo.ucIeFlg                          = pstMmInfo->ucIeFlg;


    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return VOS_TRUE;
}



TAF_PH_PLMN_SELECTION_RESULT_ENUM_UINT32 TAF_MMA_ConvertServiceTypeToAtType(
    MMA_MMC_PLMN_SELECTION_RESULT_ENUM_UINT32 enMmaServiceType
)
{
    switch (enMmaServiceType)
    {
        case MMA_MMC_PLMN_SELECTION_RESULT_NORMAL_SERVICE:
             return TAF_PH_PLMN_SELECTION_RESULT_NORMAL_SERVICE;

        case MMA_MMC_PLMN_SELECTION_RESULT_LIMITED_SERVICE:
             return TAF_PH_PLMN_SELECTION_RESULT_LIMITED_SERVICE;

        case MMA_MMC_PLMN_SELECTION_RESULT_NO_SERVICE:
             return TAF_PH_PLMN_SELECTION_RESULT_NO_SERVICE;
             
        case MMA_MMC_PLMN_SELECTION_RESULT_INTERRUPT:
             return TAF_PH_PLMN_SELECTION_RESULT_INTERRUPT;

        default:
            MN_ERR_LOG("TAF_MMA_ConvertServiceTypeToAtType: The input is invalid");
            return TAF_PH_PLMN_SELECTION_RESULT_BUTT;
    }
}


VOS_UINT32 TAF_MMA_ProcPlmnSelectStartInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_PLMN_SELECTION_START_IND_STRU                  *pstPlmnStartInd    = VOS_NULL_PTR;  
    TAF_PH_PLMN_SELECTION_INFO_STRU                         stTafPlmnSelectInfo;   
    
    pstPlmnStartInd = (MMC_MMA_PLMN_SELECTION_START_IND_STRU *)pstMsg;
    PS_MEM_SET(&stTafPlmnSelectInfo, 0, sizeof(TAF_PH_PLMN_SELECTION_INFO_STRU));

    if (MMA_MMC_PLMN_SELECTION_START != pstPlmnStartInd->enPlmnSeleStartFlag)
    {
        MN_ERR_LOG("TAF_MMA_ProcPlmnSelectStartInd: The input is invalid");
        return VOS_TRUE;
    }
  
    /* 填充上报事件 */
    stTafPlmnSelectInfo.ulPlmnSelectRlst = TAF_PH_PLMN_SELECTION_RESULT_BUTT;
    stTafPlmnSelectInfo.ulPlmnSelectFlag = TAF_PH_PLMN_SELECTION_START;

    /* 填充消息头 */
    stTafPlmnSelectInfo.PhoneEvent       = TAF_MMA_EVT_PLMN_SELECTION_INFO_IND;
    stTafPlmnSelectInfo.OpId             = MMA_OP_ID_INTERNAL;
    stTafPlmnSelectInfo.ClientId         = MMA_CLIENTID_BROADCAST;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(stTafPlmnSelectInfo.ClientId,(VOS_UINT8*)&stTafPlmnSelectInfo,sizeof(TAF_PH_PLMN_SELECTION_INFO_STRU));
    
    return VOS_TRUE;
}




VOS_UINT32 MN_MMA_ProcPlmnSelectionRlstInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_PLMN_SELECTION_RLST_IND_STRU                    *pstPlmnSelRslt;

    TAF_PH_PLMN_SELECTION_INFO_STRU     stTafPlmnSelectInfo;   

    pstPlmnSelRslt = (MMC_MMA_PLMN_SELECTION_RLST_IND_STRU*)pstMsg; 

    PS_MEM_SET(&stTafPlmnSelectInfo, 0, sizeof(TAF_PH_PLMN_SELECTION_INFO_STRU));


    /* RoamingBroker功能使能则向SIM卡发送Location事件 */
    if (VOS_TRUE == MN_MMA_GetRoamingBrokerFlg())
    {
        if ((MMA_MMC_PLMN_SELECTION_RESULT_LIMITED_SERVICE == pstPlmnSelRslt->enResult)
         || (MMA_MMC_PLMN_SELECTION_RESULT_NO_SERVICE == pstPlmnSelRslt->enResult))
        {
            NAS_MMA_ReportLociStatus();
        }
    }
    /* 开机搜网，一个网络都没有搜索到后，UE处于无服务，而由于开机初始化时
       服务状态也是无服务，因此通过服务状态改变无法报location status事件，
       需要在此处报一次,有AT&T认证用例验证此场景 */
    else
    {
        if ((VOS_FALSE == g_StatusContext.ucIsReportedLocStatus)
         && (MMA_MMC_PLMN_SELECTION_RESULT_NO_SERVICE == pstPlmnSelRslt->enResult))
        {
            NAS_MMA_ReportLociStatus();
        }
    }

    /* 填充上报事件 */
    stTafPlmnSelectInfo.ulPlmnSelectRlst = TAF_MMA_ConvertServiceTypeToAtType(pstPlmnSelRslt->enResult);

    if (TAF_PH_PLMN_SELECTION_RESULT_INTERRUPT == stTafPlmnSelectInfo.ulPlmnSelectRlst)
    {
        stTafPlmnSelectInfo.ulPlmnSelectFlag = TAF_PH_PLMN_SELECTION_INTERRUPT;
    }
    else
    {
        stTafPlmnSelectInfo.ulPlmnSelectFlag = TAF_PH_PLMN_SELECTION_END;
    }

    /* 填充消息头 */
    stTafPlmnSelectInfo.PhoneEvent       = TAF_MMA_EVT_PLMN_SELECTION_INFO_IND;
    stTafPlmnSelectInfo.OpId             = MMA_OP_ID_INTERNAL;
    stTafPlmnSelectInfo.ClientId         = MMA_CLIENTID_BROADCAST;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(stTafPlmnSelectInfo.ClientId,(VOS_UINT8*)&stTafPlmnSelectInfo,sizeof(TAF_PH_PLMN_SELECTION_INFO_STRU));
    
    return VOS_TRUE;
}

/*****************************************************************************
 函 数 名  : MN_PH_GetRegCostTime
 功能描述  : 返回当前开机搜网成功注册花费的时间
 输入参数  : 无
 输出参数  : VOS_UINT32:开机搜网注册花费的时间,单位为s
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年9月8日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  MN_PH_GetRegCostTime( VOS_VOID )
{
    return g_stRegTimeInfo.ulCostTime;
}



TAF_PH_MODE MMA_GetCurrPhMode(VOS_VOID)
{
    return TAF_SDC_GetCurPhoneMode();
}




VOS_VOID TAF_MMA_ReportPlmnModeEvent(VOS_UINT8 ucPlmnMode)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent = VOS_NULL_PTR;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("TAF_MMA_ReportPlmnModeEvent:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    pstPhoneEvent->OP_PlmnMode  = VOS_TRUE;
    pstPhoneEvent->PhoneEvent   = TAF_PH_EVT_NSM_STATUS_IND;
    pstPhoneEvent->ucPlmnMode   = ucPlmnMode;

    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return;
}


VOS_UINT32 TAF_MMA_IsEnablePlmnList(VOS_VOID)
{
    TAF_MMA_AUTO_PLMN_SEL_USER_ENUM_UINT8 enAutoPlmnSelUser;

    enAutoPlmnSelUser = TAF_MMA_GetAutoPlmnSelUser();

    if (STA_FSM_ENABLE == g_StatusContext.ulFsmState)
    {
        return VOS_TRUE;
    }

    /* refresh触发的自动搜网 */
    if ((STA_FSM_PLMN_RESEL == g_StatusContext.ulFsmState)
     && (TAF_MMA_AUTO_PLMN_SEL_USER_USIM_REFRESH == enAutoPlmnSelUser))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 TAF_MMA_IsEnablePlmnSelect(VOS_VOID)
{
    TAF_MMA_PLMN_MODE_ENUM_UINT8        enPlmnMode;

    enPlmnMode = TAF_MMA_GetPlmnMode_Refresh();

    /* 只有在ENABLE状态，并且网络没有disable掉网络选择菜单，才能进行指定搜网 */
    if ((STA_FSM_ENABLE == g_StatusContext.ulFsmState)
     && (TAF_MMA_PLMN_MODE_ENUM_ENABLE == enPlmnMode))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 TAF_MMA_IsEnableDeAttach(VOS_VOID)
{
    TAF_MMA_AUTO_PLMN_SEL_USER_ENUM_UINT8 enAutoPlmnSelUser;

    enAutoPlmnSelUser = TAF_MMA_GetAutoPlmnSelUser();

    if (STA_FSM_ENABLE == g_StatusContext.ulFsmState)
    {
        return VOS_TRUE;
    }

    if (STA_FSM_DE_ATTACH == g_StatusContext.ulFsmState)
    {
        return VOS_TRUE;
    }

    /* refresh触发的自动搜网 */
    if ((STA_FSM_PLMN_RESEL == g_StatusContext.ulFsmState)
     && (TAF_MMA_AUTO_PLMN_SEL_USER_USIM_REFRESH == enAutoPlmnSelUser))
    {
        return VOS_TRUE;
    }

    /* refresh触发的SYSCFG，在收到CNF之后可能会触发attach。
       此时用户下发attach或者detach，不能打断SYSCFG。
       而且MMA目前没有缓存机制，所以直接给用户返回ERROR。
    */

    return VOS_FALSE;
}


VOS_UINT32 TAF_MMA_SndAutoPlmnReselect_Refresh(VOS_VOID)
{
    VOS_UINT32                          ulRet;

    ulRet = TAF_MMA_SndMmcPlmnUserReselReq(MMA_MMC_PLMN_SEL_MODE_AUTO);

    if (VOS_ERR == ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndAutoPlmnReselect_Refresh(): send msg fail");
        return VOS_ERR;
    }

    /* 记录PLMN重新类型 */
    g_StatusContext.ucReselMode   = MMA_MMC_PLMN_SEL_MODE_AUTO;

    /* 记录当前的主状态 */
    g_StatusContext.ulPreFsmState = g_StatusContext.ulFsmState;

    /* 设置主状态 */
    g_StatusContext.ulFsmState    = STA_FSM_PLMN_RESEL;

    /* 设置子状态 */
    g_StatusContext.aucFsmSubFlag[0]         = STA_FSMSUB_PLMN_RESEL_AUTO;
    g_StatusContext.aFsmSub[0].ucFsmStateSub = STA_FSMSUB_PLMN_RESEL_AUTO;
    g_StatusContext.aFsmSub[0].TimerName     = STA_TIMER_PLMN_RESEL;

    /* 启动PLMN_RESEL状态定时器 */
    ulRet = NAS_StartRelTimer(&g_StatusContext.ahStaTimer[0],
                              WUEPS_PID_MMA,
                              aStaTimerLen[STA_TIMER_PLMN_RESEL],
                              TAF_MMA,
                              MMA_MAKELPARAM(0, STA_TIMER_PLMN_RESEL),
                              VOS_RELTIMER_NOLOOP
                              );
    if (VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndAutoPlmnReselect_Refresh(): start timer fail");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MMA_RcvMmcPlmnReselCnf_Refresh(MMC_MMA_PLMN_RESEL_CNF_STRU *pstMsg)
{
    VOS_UINT32                          ulRet;

    /*关闭状态定时器*/
    ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                             g_StatusContext.aFsmSub[0].TimerName,
                             &g_StatusContext.ahStaTimer[0]);

    if ( VOS_OK != ulRet )
    {
        STA_TRACE (STA_WARNING_PRINT,
            "TAF_MMA_RcvMmcPlmnReselCnf_Refresh():WARNING:NAS_StopRelTimer failed");
    }

    ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);
    if (STA_ERROR == ulRet)
    {
        STA_TRACE(STA_WARNING_PRINT, "TAF_MMA_RcvMmcPlmnReselCnf_Refresh():WARNING:Sta_UpdateFsmFlag failed\n");
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_RcvMmcPlmnReselCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_PLMN_RESEL_CNF_STRU        *pstPlmnReselCnfMsg;

    pstPlmnReselCnfMsg = (MMC_MMA_PLMN_RESEL_CNF_STRU*)pstMsg;

    TAF_MMA_SetPlmnSelectionMode(TAF_PH_PLMN_SEL_MODE_AUTO);

    /* 如果是6F15文件refresh触发的自动搜网 */
    if (TAF_MMA_AUTO_PLMN_SEL_USER_USIM_REFRESH == TAF_MMA_GetAutoPlmnSelUser())
    {
        /* MMC给MMA回复指定搜和自动搜都是D_MMC_PLMN_SEL_CNF消息，refresh只会触发自动搜网 */
        if (STA_FSM_PLMN_RESEL != g_StatusContext.ulFsmState)
        {
            return VOS_TRUE;
        }

        TAF_MMA_RcvMmcPlmnReselCnf_Refresh(pstPlmnReselCnfMsg);
    }
    /* AT发起的自动搜网或指定搜网，按原有流程 */
    else
    {
        Sta_SelPlmnSpecializeCnf(pstPlmnReselCnfMsg->enResult);
    }


    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvMmcPlmnSpecialSelCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_PLMN_SPECIAL_SEL_CNF_STRU  *pstPlmnSpecSelCnfMsg;

    pstPlmnSpecSelCnfMsg = (MMC_MMA_PLMN_SPECIAL_SEL_CNF_STRU*)pstMsg;

    /* svlte nv开启允许关机状态设置cops */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        TAF_MMA_SetPlmnSelectionMode(MMA_MMC_PLMN_SEL_MODE_MANUAL);
        
        g_StatusContext.ucOperateType = STA_OP_REGPLMN;
        Sta_EventReport(g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_SEL_CNF);

        return VOS_TRUE;
    }

    TAF_MMA_SetPlmnSelectionMode(TAF_PH_PLMN_SEL_MODE_MANUAL);

    Sta_SelPlmnSpecializeCnf(pstPlmnSpecSelCnfMsg->enResult);


    return VOS_TRUE;
}


VOS_VOID TAF_MMA_StopStatusCtxTimer(VOS_VOID)
{
    VOS_UINT32                            ulRet;

    /* 关闭当前状态的定时器保护*/
    if (VOS_NULL != g_StatusContext.ahStaTimer[0])
    {
        ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                 g_StatusContext.aFsmSub[0].TimerName,
                                 &g_StatusContext.ahStaTimer[0]);
        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "TAT_MMA_ProcPhoneStop_NotEnableStatus():WARNING:NAS_StopRelTimer failed  ");
        }
        g_StatusContext.ahStaTimer[0] = VOS_NULL;
    }

    if ( VOS_NULL != g_StatusContext.ahStaTimer[1] )
    {
        ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                 g_StatusContext.aFsmSub[1].TimerName,
                                 &g_StatusContext.ahStaTimer[1]);


        if ( VOS_OK != ulRet )
        {
            STA_TRACE (STA_WARNING_PRINT,
                "TAT_MMA_ProcPhoneStop_NotEnableStatus():WARNING:NAS_StopRelTimer failed  ");
        }
        g_StatusContext.ahStaTimer[1] = VOS_NULL;
    }
}
VOS_UINT32 TAF_MMA_ProcPhoneStop_NotEnableStatus(VOS_VOID)
{
    VOS_UINT32                            ulRet;
    VOS_UINT32                            ulIsRefreshAutoPlmnSel;
    VOS_UINT32                            ulIsRefreshSyscfg;
    TAF_MMA_AUTO_PLMN_SEL_USER_ENUM_UINT8 enAutoPlmnSelUser;
    TAF_MMA_SYSCFG_USER_ENUM_UINT8        enSyscfgUser;

    enAutoPlmnSelUser = TAF_MMA_GetAutoPlmnSelUser();
    enSyscfgUser      = TAF_MMA_GetSyscfgUser();

    ulIsRefreshAutoPlmnSel = VOS_FALSE;
    ulIsRefreshSyscfg      = VOS_FALSE;

    /* refresh触发的自动搜网 */
    if ((STA_FSM_PLMN_RESEL == g_StatusContext.ulFsmState)
     && (TAF_MMA_AUTO_PLMN_SEL_USER_USIM_REFRESH == enAutoPlmnSelUser))
    {
        ulIsRefreshAutoPlmnSel = VOS_TRUE;
    }

    /* refresh触发的SYSCFG*/
    if ((STA_FSM_SYS_CFG_SET == g_StatusContext.ulFsmState)
     && (TAF_MMA_SYSCFG_USER_USIM_REFRESH == enSyscfgUser))
    {
        ulIsRefreshSyscfg = VOS_TRUE;
    }

    /* 如果是Refresh触发的自动搜网或SYSCFG，不需要上报响应事件通知APP*/
    if ((VOS_TRUE != ulIsRefreshAutoPlmnSel)
     && (VOS_TRUE != ulIsRefreshSyscfg))
    {
        /* 终止当前的请求过程，上报响应事件通知APP */
        ulRet = Sta_MsgReqAbort (STA_MSGABORT_ERR);
        if (STA_ERROR == ulRet)
        {
            NAS_WARNING_LOG(WUEPS_PID_MMA,
                "TAF_MMA_ProcPhoneStop_NotEnableStatus(): different request abort failed  ");
        }
    }

    TAF_MMA_StopStatusCtxTimer();

    return VOS_OK;
}


VOS_VOID TAF_MMA_DeleteRatType(
    TAF_PH_RAT_TYPE_ENUM_UINT8           ucRatType,
    MMA_MMC_PLMN_RAT_PRIO_STRU          *pstPlmnRatPrioList
)
{
    VOS_UINT32                          i;

    MMA_MMC_PLMN_RAT_PRIO_STRU          stOldPlmnRatPrioList;

    /* 记录输入的接入技术列表 */
    PS_MEM_CPY(&stOldPlmnRatPrioList, pstPlmnRatPrioList, sizeof(MMA_MMC_PLMN_RAT_PRIO_STRU));


    pstPlmnRatPrioList->ucRatNum = 0;

    /* 接入技术初始化为BUTT */
    for (i = 0 ; i < MMA_MMC_MAX_RAT_NUM; i++)
    {
        pstPlmnRatPrioList->aucRatPrio[i] = TAF_SDC_SYS_MODE_BUTT;
    }


    if (stOldPlmnRatPrioList.ucRatNum > MMA_MMC_MAX_RAT_NUM)
    {
        stOldPlmnRatPrioList.ucRatNum = MMA_MMC_MAX_RAT_NUM;
    }

    for (i = 0 ; i < stOldPlmnRatPrioList.ucRatNum; i++)
    {
        if (ucRatType == stOldPlmnRatPrioList.aucRatPrio[i])
        {
            continue;
        }
        pstPlmnRatPrioList->aucRatPrio[pstPlmnRatPrioList->ucRatNum] = stOldPlmnRatPrioList.aucRatPrio[i];
        pstPlmnRatPrioList->ucRatNum++;
    }

    return;
}

VOS_VOID TAF_MMA_GetAvailablePlmnRatPrio(MMA_MMC_PLMN_RAT_PRIO_STRU *pstPlmnRatPrioList)
{

    VOS_UINT32                          ulGsmAccessForbidden;
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT8                           ucCardType;
    VOS_UINT32                          ulGsmForbidden;
    VOS_UINT8                           i;

    VOS_UINT8                           ucRatBalancingEnableFlg;

    ucRatBalancingEnableFlg = TAF_MMA_GetRatBalancingEnableFlg();
#endif


    ulGsmAccessForbidden = TAF_MMA_IsGsmAccessForbidden();

    /* 如果UE不支持GSM接入，当设置非G单模去掉G模，如果是G单模，由MMC控制提供限制服务 */
    if ((VOS_FALSE == MN_MMA_IsGOnlyMode((TAF_PH_RAT_ORDER_STRU *)pstPlmnRatPrioList))
     && (VOS_TRUE == ulGsmAccessForbidden))
    {
        TAF_MMA_DeleteRatType(TAF_PH_RAT_GSM, pstPlmnRatPrioList);
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* SIM卡校正去掉L模 */
    NAS_USIMMAPI_GetCardType(VOS_NULL_PTR, &ucCardType) ;

    /* SIM卡时，如果是LTE only，不删除L模 */
    if ( (USIMM_CARD_SIM == ucCardType)
      && (VOS_TRUE == ucRatBalancingEnableFlg) )
    {
        TAF_MMA_DeleteRatType(TAF_PH_RAT_LTE, pstPlmnRatPrioList);
    }

    /* 如果个数为0,即当前仅存在L单模也接入禁止时,默认为GW模 */
    if (0 == pstPlmnRatPrioList->ucRatNum)
    {
        i = 0;

        if (VOS_TRUE == TAF_SDC_IsPlatformSupportUtran())
        {
            pstPlmnRatPrioList->aucRatPrio[i] = TAF_PH_RAT_WCDMA;
            i ++;
        }

        ulGsmForbidden = MMA_IsGsmForbidden();
        if ((VOS_TRUE == TAF_SDC_IsPlatformSupportGsm())
         && ((MMA_FALSE == ulGsmForbidden)))
        {
            pstPlmnRatPrioList->aucRatPrio[i] = TAF_PH_RAT_GSM;
            i ++;
        }
        pstPlmnRatPrioList->ucRatNum      = i;
    }
#endif

    return;

}


VOS_VOID TAF_MMA_RcvSyscfgCnfFail(VOS_VOID)
{
    VOS_UINT32                          ulResult;

    if (TAF_MMA_SYSCFG_USER_USIM_REFRESH != TAF_MMA_GetSyscfgUser())
    {
        return;
    }

    /* TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING定时器正在运行，返回 */
    if (MMA_TIMER_RUN == TAF_MMA_GetTiTryingPeriodRatBalancingStatus())
    {
        return;
    }

    /* 如果是4F36refresh触发的SYSCFG失败，需要启动TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING定时器 */
    ulResult = NAS_StartRelTimer(&(g_StatusContext.stRatBalancingCtx.stTiPeriodTryingRatBalancing.MmaHTimer),
                                 WUEPS_PID_MMA,
                                 TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING_LEN,
                                 TAF_MMA,
                                 TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING,
                                 VOS_RELTIMER_NOLOOP );

    if (VOS_OK != ulResult)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_RcvSyscfgCnfFail(): Start Timer Fail");
    }
    else
    {
        TAF_MMA_SetTiTryingPeriodRatBalancingStatus(MMA_TIMER_RUN);
    }

    return;
}



VOS_UINT8 TAF_MMA_IsModeChange(
    TAF_SDC_SYS_MODE_ENUM_UINT8         enCurNetWork,
    VOS_UINT8                           ucSysSubMode
)
{
    VOS_UINT8                           ucModeChangeFlg;
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enCurrUtranMode;

    enCurrUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();

    ucModeChangeFlg = VOS_FALSE;

    /* 如果接入技术不相同，需要上报Mode */
    if (enCurNetWork != TAF_SDC_GetSysMode())
    {
        ucModeChangeFlg = VOS_TRUE;
#ifndef __PS_WIN32_RECUR__
#endif
    }
    /* 接入技术为G时，系统子模式不同时，需要上报Mode */
    else if (TAF_SDC_SYS_MODE_GSM == enCurNetWork)
    {
        if (ucSysSubMode != TAF_SDC_GetSysSubMode())
        {
            ucModeChangeFlg = VOS_TRUE;
        }
    }

    /* 当前网络为WCDMA时，有可能TD->W或W->TD,需要进一步判断UtranMode
       如果UtranMode发生变化，需要上报Mode */
    else if (TAF_SDC_SYS_MODE_WCDMA == enCurNetWork)
    {
        if (enCurrUtranMode != g_StatusContext.ucPreUtranMode)
        {
            ucModeChangeFlg             = VOS_TRUE;
        }
    }
    else
    {
    }


    return ucModeChangeFlg;

}


VOS_UINT32 TAF_MMA_RcvMmcUserPlmnRej(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;

    /*关闭状态定时器*/
    ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                             g_StatusContext.aFsmSub[0].TimerName,
                             &g_StatusContext.ahStaTimer[0]);


    if ( VOS_OK != ulRet )
    {
        STA_TRACE (STA_WARNING_PRINT,
            "TAF_MMA_RcvMmcUserPlmnRej():WARNING:NAS_StopRelTimer failed");
    }


    /* 上报当前指定搜网失败 */
    g_StatusContext.PhoneError    = TAF_ERR_CMD_TYPE_ERROR;

    g_StatusContext.ucOperateType = STA_OP_PHONEERROR;
    MMA_NORMAILLOG("NAS_MN_ReportPlmnSrchResult():NORMAIL:SPECIALIZE THE PLMN FAILURE!");

    /* 上报APP当前状态事件 */
    Sta_EventReport (g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_SEL_CNF);
    STA_TRACE(STA_NORMAL_PRINT, "NAS_MN_ReportPlmnSrchResult: TAF_PH_EVT_PLMN_SEL_CNF reported to AT!");

    ulRet = Sta_UpdateFsmFlag(STA_FSMSUB_NULL, STA_FSM_ENABLE, STA_UPDATE_MONO_CLEAR);
    if (STA_ERROR == ulRet)
    {
        STA_TRACE(STA_WARNING_PRINT, "Sta_SelPlmnSpecializeCnf():WARNING:Sta_UpdateFsmFlag failed\n");
    }

    return VOS_TRUE;

}
VOS_VOID  TAF_MMA_RcvTimerPlmnListCnfExpired( VOS_UINT32 ulOpId )
{
    VOS_UINT32                          ulRet;

    /* 当前状态不是PLMN LIST状态，直接返回 */
    if ( STA_FSM_PLMN_LIST != g_StatusContext.ulFsmState )
    {
        STA_TRACE(STA_WARNING_PRINT, "TAF_MMA_RcvUserAbortPlmnList():WARNING: no plmn list searching");
        return;
    }

    /* MMA的LIST搜网保护定时器超时则主动向MMC发送停止LIST搜网请求
       收到MMC的回复后再向AT回复 */
    ulRet = TAF_MMA_SndMmcPlmnListAbortReq();

    if (VOS_ERR == ulRet)
    {
        STA_TRACE(STA_ERROR_PRINT, "TAF_MMA_AbortPlmnList():ERROR: failed");
    }


    /* 启动STA_TIMER_PLMN_LIST_ABORT 10s定时器，如果超时未收到MMC的回复则上报AT失败 */
    g_StatusContext.aucFsmSubFlag[0]         = STA_FSMSUB_MONO;
    g_StatusContext.aFsmSub[0].ucFsmStateSub = STA_FSMSUB_MONO;
    g_StatusContext.aFsmSub[0].TimerName     = STA_TIMER_PLMN_LIST_ABORT;
    g_StatusContext.aFsmSub[0].ulOpId        = ulOpId;
    if (VOS_OK != NAS_StartRelTimer(&g_StatusContext.ahStaTimer[0],
                                     WUEPS_PID_MMA,
                                     aStaTimerLen[STA_TIMER_PLMN_LIST_ABORT],
                                     TAF_MMA,
                                     MMA_MAKELPARAM(0, STA_TIMER_PLMN_LIST_ABORT),
                                     VOS_RELTIMER_NOLOOP))
    {
        STA_TRACE(STA_ERROR_PRINT, "TAF_MMA_AbortPlmnList():ERROR: VOS_StartRelTimer runs failed");
    }


    /* 设置当前正在处理定时器超时的列表搜打断过程 */
    g_StatusContext.ucPlmnListAbortProc = TAF_MMA_PLMN_LIST_ABORT_PROCESSING_TIMEOUT;

    return;
}



VOS_VOID  TAF_MMA_RcvUserAbortPlmnList( VOS_UINT32 ulOpId )
{
    VOS_UINT32                          ulRet;

    /* MMA上报PLMN LIST CNF和列表搜打断对冲时，当前状态已不是PLMN LIST状态，
       直接给AT回复TAF_PH_EVT_PLMN_LIST_ABORT_CNF */
    if ( STA_FSM_PLMN_LIST != g_StatusContext.ulFsmState )
    {
        STA_TRACE(STA_WARNING_PRINT, "TAF_MMA_RcvUserAbortPlmnList():WARNING: no plmn list searching");

        g_StatusContext.ucOperateType = STA_OP_NULL;

        /* 上报AT当前状态事件 */
        Sta_EventReport (g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_LIST_ABORT_CNF);
        return;
    }

    /* 定时器超时打断列表搜和用户打断列表搜对冲时，以用户打断为准 */
    if ( TAF_MMA_PLMN_LIST_ABORT_PROCESSING_TIMEOUT == g_StatusContext.ucPlmnListAbortProc )
    {
        STA_TRACE(STA_WARNING_PRINT, "TAF_MMA_RcvUserAbortPlmnList():WARNING: plmn list abort processing");

        /* 设置当前正在处理用户打断列表搜过程 */
        g_StatusContext.ucPlmnListAbortProc = TAF_MMA_PLMN_LIST_ABORT_PROCESSING_USER_ABORT;

        return;
    }

    /* 停列表搜定时器 */
    if (VOS_OK != NAS_StopRelTimer(WUEPS_PID_MMA,
             STA_TIMER_PLMN_LIST,
             &g_StatusContext.ahStaTimer[0]))
    {
        STA_TRACE(STA_ERROR_PRINT, "TAF_MMA_RcvUserAbortPlmnList():WARNING: NAS_StopRelTimer failed");
    }

    /* MMA的LIST搜网保护定时器超时则主动向MMC发送停止LIST搜网请求
       收到MMC的回复后再向AT回复 */

    ulRet = TAF_MMA_SndMmcPlmnListAbortReq();
    if (VOS_ERR == ulRet)
    {
        STA_TRACE(STA_ERROR_PRINT, "TAF_MMA_RcvUserAbortPlmnList():ERROR: failed");
    }


    /* 启动STA_TIMER_PLMN_LIST_ABORT 10s定时器，如果超时未收到MMC的回复则上报AT失败 */
    g_StatusContext.aucFsmSubFlag[0]         = STA_FSMSUB_MONO;
    g_StatusContext.aFsmSub[0].ucFsmStateSub = STA_FSMSUB_MONO;
    g_StatusContext.aFsmSub[0].TimerName     = STA_TIMER_PLMN_LIST_ABORT;
    g_StatusContext.aFsmSub[0].ulOpId        = ulOpId;
    if (VOS_OK != NAS_StartRelTimer(&g_StatusContext.ahStaTimer[0],
                                     WUEPS_PID_MMA,
                                     aStaTimerLen[STA_TIMER_PLMN_LIST_ABORT],
                                     TAF_MMA,
                                     MMA_MAKELPARAM(0, STA_TIMER_PLMN_LIST_ABORT),
                                     VOS_RELTIMER_NOLOOP))
    {
        STA_TRACE(STA_ERROR_PRINT, "TAF_MMA_RcvUserAbortPlmnList():ERROR: VOS_StartRelTimer runs failed");
    }


    /* 设置当前正在进行用户打断列表搜过程 */
    g_StatusContext.ucPlmnListAbortProc = TAF_MMA_PLMN_LIST_ABORT_PROCESSING_USER_ABORT;

    return;
}



VOS_VOID  TAF_MMA_RcvTimerPlmnListAbortCnfExpired(
    VOS_UINT16                          usTimerIndex
)
{
    if (TAF_MMA_PLMN_LIST_ABORT_BUTT == g_StatusContext.ucPlmnListAbortProc)
    {
        STA_TRACE(STA_WARNING_PRINT, "TAF_MMA_RcvTimerPlmnListAbortCnfExpired():WARNING: no plmn list abort");
        return;
    }

    g_StatusContext.aFsmSub[usTimerIndex].ulOpId        = STA_OPID_INVALID;
    g_StatusContext.aFsmSub[usTimerIndex].ucFsmStateSub = STA_FSMSUB_NULL;
    g_StatusContext.aFsmSub[usTimerIndex].TimerName     = STA_TIMER_NULL;
    g_StatusContext.aucFsmSubFlag[usTimerIndex]         = STA_FSMSUB_NULL;

    /* PLMN LIST ABORT原因值为定时器超时 */
    if ( TAF_MMA_PLMN_LIST_ABORT_PROCESSING_TIMEOUT == g_StatusContext.ucPlmnListAbortProc )
    {
        /* 向AT回复列表搜超时结果 */
        g_StatusContext.PhoneError    = TAF_ERR_TIME_OUT;
        g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

        /* 上报AT当前状态事件 */
        Sta_EventReport (g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_LIST_CNF);
    }
    else
    {
        g_StatusContext.ucOperateType = STA_OP_NULL;

        /* 上报AT当前状态事件 */
        Sta_EventReport (g_StatusContext.ulCurOpId, TAF_PH_EVT_PLMN_LIST_ABORT_CNF);
    }

    /* 迁移状态到ENABLE状态 */
    g_StatusContext.ulFsmState = STA_FSM_ENABLE;

    /* 设置当前没有处理用户列表搜打断过程 */
    g_StatusContext.ucPlmnListAbortProc = TAF_MMA_PLMN_LIST_ABORT_BUTT;

    return;
}



VOS_UINT32 TAF_MMA_IsGsmAccessForbidden(VOS_VOID)
{
    VOS_UINT32   ulEfustGsmAccess;
    VOS_UINT32   ulEfustGsmSecContext;
    VOS_UINT8    ucSimStatus;
    VOS_UINT8    ucSimType;

    if (MMA_QUICK_START_ENABLE == gstMmaValue.ulQuickStartFlg)
    {
        return VOS_FALSE;
    }

    if (VOS_TRUE == gstMmaValue.stEfustServiceCfg.ucForbidReg2GNetWork)
    {
        /*获取卡状态和卡类型*/
        MMA_PhoneGetUsimStatus(&ucSimStatus);
        MMA_PhoneGetUsimType(&ucSimType);

        if ( ( MMA_USIM_PRESENT == ucSimStatus )
            && (MMA_USIM_TYPE_USIM == ucSimType) )
        {
            /* 根据USIM卡文件中EFust，第27和38项判断是否允许GSM接入 */
            ulEfustGsmAccess     = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_GSM_ACCESS_IN_USIM);
            ulEfustGsmSecContext = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_GSM_SECURITY_CONTEXT);            

            if ((0 == ulEfustGsmAccess) && (0 == ulEfustGsmSecContext))
            {
                return VOS_TRUE;
            }
        }

    }

    return VOS_FALSE;
}

/* TAF_MMA_InitTimerLen移动位置 */

VOS_UINT32 TAF_MMA_IsExistPlmnRatAllowAccess(VOS_VOID)
{
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList = VOS_NULL_PTR;
    VOS_UINT32                          ulGOnlyModeFlg;
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT32                          ulLOnlyModeFlg;
    VOS_UINT8                           ucCardType;
#endif

    ulGOnlyModeFlg = VOS_FALSE;
    pstRatPrioList = MN_MMA_GetRatPrioListAddr();
    ulGOnlyModeFlg = MN_MMA_IsGOnlyMode(pstRatPrioList);

#if (FEATURE_ON == FEATURE_LTE)
    ulLOnlyModeFlg = VOS_FALSE;
    NAS_USIMMAPI_GetCardType(VOS_NULL_PTR, &ucCardType);
    ulLOnlyModeFlg = MN_MMA_IsLOnlyMode(pstRatPrioList);
#endif

    /* USIM卡禁止GSM接入并且是G单模时，不允许用户指定搜 */
    if ((VOS_TRUE == TAF_MMA_IsGsmAccessForbidden())
     && (VOS_TRUE == ulGOnlyModeFlg))
    {
        return VOS_FALSE;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* SIM卡并且是L单模时，也不允许用户指定搜 */
    if ((USIMM_CARD_SIM == ucCardType)
     && (VOS_TRUE == ulLOnlyModeFlg))
    {
        return VOS_FALSE;
    }
#endif

    return VOS_TRUE;

}


VOS_VOID TAF_MMA_ReportRegStatus(
    MMA_MMC_REG_STATE_ENUM_UINT8        enRegState,
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enCnDomainId
)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent = VOS_NULL_PTR;
    TAF_SDC_PLMN_ID_STRU               *pstPlmnId     = VOS_NULL_PTR;

    pstPlmnId     = TAF_SDC_GetCurrCampPlmnId();
    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                        sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("TAF_MMA_ReportRegStatus:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));
    MMA_InitEventInfoOP(pstPhoneEvent);
    pstPhoneEvent->OP_CurRegSta            = MMA_FALSE;

    if (MMA_MMC_SRVDOMAIN_CS == enCnDomainId)
    {
        pstPhoneEvent->CurRegSta.OP_CsRegState = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_Lac        = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_CellId     = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_ucAct      = MMA_TRUE;

        TAF_SDC_SetCsRegStatus(enRegState);
        pstPhoneEvent->CurRegSta.RegState = enRegState;
    }
    else if (MMA_MMC_SRVDOMAIN_PS == enCnDomainId)
    {
        pstPhoneEvent->CurRegSta.OP_PsRegState = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_Lac        = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_Rac        = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_CellId     = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_ucAct      = MMA_TRUE;

        TAF_SDC_SetPsRegStatus(enRegState);
        pstPhoneEvent->CurRegSta.ucPsRegState = enRegState;
    }
    else
    {
    }


    pstPhoneEvent->CurRegSta.ucRac               = TAF_SDC_GetCurrCampRac();
    pstPhoneEvent->CurRegSta.CellId.ucCellNum    = 1;
    pstPhoneEvent->CurRegSta.CellId.aulCellId[0] = TAF_SDC_GetCurrCampCellId();
    pstPhoneEvent->CurRegSta.usLac               = TAF_SDC_GetCurrCampLac();

    PS_MEM_CPY(&pstPhoneEvent->CurRegSta.Plmn, pstPlmnId, sizeof(pstPhoneEvent->CurRegSta.Plmn));

    pstPhoneEvent->PhoneEvent              = TAF_PH_EVT_SYSTEM_INFO_IND;
    pstPhoneEvent->OP_CurRegSta            = MMA_TRUE;

    if (TAF_PH_USIM_SPN == gstMmaValue.stUsimInfo.UsimSpnInfo.SpnType)
    {
        pstPhoneEvent->OP_SpnInfo      = MMA_TRUE;
        pstPhoneEvent->PhSpnInfo.DispRplmnMode   = gstMmaValue.stUsimInfo.UsimSpnInfo.DispRplmnMode;
        pstPhoneEvent->PhSpnInfo.PhRoam          = MMA_IsRoam(pstPlmnId);
        PS_MEM_CPY(pstPhoneEvent->PhSpnInfo.aucSpnName, gstMmaValue.stUsimInfo.UsimSpnInfo.aucSpnName,
                   TAF_PH_SPN_NAME_MAXLEN);
    }
    else if (TAF_PH_GSM_SPN == gstMmaValue.stUsimInfo.SimSpnInfo.SpnType)
    {
        pstPhoneEvent->OP_SpnInfo      = MMA_TRUE;
        pstPhoneEvent->PhSpnInfo.DispRplmnMode   = gstMmaValue.stUsimInfo.SimSpnInfo.DispRplmnMode;
        pstPhoneEvent->PhSpnInfo.PhRoam          = MMA_IsRoam(pstPlmnId);
        PS_MEM_CPY(pstPhoneEvent->PhSpnInfo.aucSpnName, gstMmaValue.stUsimInfo.SimSpnInfo.aucSpnName,
                   TAF_PH_SPN_NAME_MAXLEN);
    }
    else
    {
    }

    /* 将MMC上报的系统子模式转为+CREG/+CGREG命令参数[ACT]中协议规定的接入技术值 */
    NAS_MMA_TransferSysSubMode2ActForAtCmdCreg(TAF_SDC_GetSysSubMode(), &(pstPhoneEvent->CurRegSta.ucAct));

    switch (TAF_SDC_GetCampSysMode())
    {
#if (FEATURE_ON == FEATURE_LTE)
        case TAF_SDC_SYS_MODE_LTE:
            pstPhoneEvent->RatType = TAF_PH_INFO_LTE_RAT;
            break;
#endif

        case TAF_SDC_SYS_MODE_WCDMA:
            pstPhoneEvent->RatType = TAF_PH_INFO_WCDMA_RAT;
            break;
            
        case TAF_SDC_SYS_MODE_GSM:
            pstPhoneEvent->RatType = TAF_PH_INFO_GSM_RAT;
            break;

        default:
            pstPhoneEvent->RatType = TAF_PH_INFO_NONE_RAT;
            break;
    }

    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
    return;
}
VOS_UINT32 TAF_MMA_IsCsServDomainAvail(
    TAF_PH_MS_CLASS_TYPE                enMsClassType
)
{
    /* 手机模式是A模式或者CC模式，则认为CS域可用，其他情况认为CS域不可用 */
    if ( (TAF_PH_MS_CLASS_A  == enMsClassType)
      || (TAF_PH_MS_CLASS_CC == enMsClassType) )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 TAF_MMA_IsPsServDomainAvail(
    TAF_PH_MS_CLASS_TYPE                enMsClassType
)
{
    /* 手机模式是A模式或者CG模式，则认为PS域可用，其他情况认为PS域不可用 */
    if ( (TAF_PH_MS_CLASS_A  == enMsClassType)
      || (TAF_PH_MS_CLASS_CG == enMsClassType) )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
TAF_CS_SERVICE_ENUM_UINT32  TAF_MMA_GetCsStateFromSyscfgSetFlag(
    VOS_UINT16                          usSyscfgSetFlag
)
{
    /* 不支持SVLTE，不需要通知MSG和STK,认为无变化 */
    if (VOS_FALSE == TAF_SDC_GetSvlteSupportFlag())
    {
        return TAF_CS_SERVICE_BUTT;
    }

    /* syscfg设置标志新的CS域能力不可用 */
    if (MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_OFF ==
        (usSyscfgSetFlag & MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_OFF))
    {
        return TAF_CS_OFF;
    }

    /* syscfg设置标志新的CS域能力可用 */
    if (MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_ON ==
        (usSyscfgSetFlag & MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_ON))
    {
        return TAF_CS_ON;
    }

    /* 其他情况默认不需要发送 */
    return TAF_CS_SERVICE_BUTT;

}


VOS_VOID  TAF_MMA_ClearSyscfgCsServiceChangeFlag(
    VOS_UINT16                          usSyscfgSetFlag
)
{
    if (MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_OFF ==
        (usSyscfgSetFlag & MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_OFF))
    {
        /* 之前置过发送CS_OFF的标志，把CS_OFF的标志清除 */
        gstMmaValue.stSysCfgSet.usSetFlag &= ~MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_OFF;
    }
    else if (MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_ON ==
        (usSyscfgSetFlag & MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_ON))
    {
        /* 之前置过发送CS_ON的标志，把CS_ON的标志清除 */
        gstMmaValue.stSysCfgSet.usSetFlag &= ~MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_ON;
    }
    else
    {
    }

    return;

}


VOS_UINT32 TAF_MMA_SndStkCsServiceChangeNotify(
    TAF_CS_SERVICE_ENUM_UINT32          enCsState
)
{
    VOS_UINT32                          ulRet;
    TAF_STK_CS_SERVICE_IND             *pstMsg = VOS_NULL_PTR;

    pstMsg = (TAF_STK_CS_SERVICE_IND *)PS_ALLOC_MSG(WUEPS_PID_TAF,
                           sizeof(TAF_STK_CS_SERVICE_IND) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_TAF, "TAF_MMA_SndStkCsServiceChangeNotify:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(TAF_STK_CS_SERVICE_IND) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = NAS_GetOamReceivePid(MAPS_STK_PID);
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_TAF;
    pstMsg->ulLength         = sizeof(TAF_STK_CS_SERVICE_IND) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgName        = TAF_STK_CS_STATE_IND;
    pstMsg->enCSState        = enCsState;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_TAF, pstMsg );

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_TAF, "TAF_MMA_SndStkCsServiceChangeNotify:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;

}
VOS_UINT32 TAF_MMA_IsNeedSndStkEvt(VOS_VOID)
{
    if (VOS_TRUE == TAF_SDC_GetSvlteSupportFlag())
    {
       if (VOS_TRUE == TAF_MMA_IsCsServDomainAvail(gstMmaValue.stSetMsClass.MsClassType))
       {
           return VOS_TRUE;
       }
       return VOS_FALSE;
    }
    else
    {
        return VOS_TRUE;
    }
}



VOS_VOID TAF_MMA_SndMtaAreaLostInd(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    NAS_MTA_AREA_LOST_IND_STRU         *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (NAS_MTA_AREA_LOST_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(NAS_MTA_AREA_LOST_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMtaAreaLostInd:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(NAS_MTA_AREA_LOST_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->stMsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid    = UEPS_PID_MTA;
    pstMsg->stMsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength         = sizeof(NAS_MTA_AREA_LOST_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName        = ID_NAS_MTA_AREA_LOST_IND;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMsgCsServiceChangeNotify:ERROR:PS_SEND_MSG FAILURE");
        return;
    }

    return;
}


VOS_UINT32  TAF_MMA_PlmnReselectAutoReq_PowerOff(VOS_UINT32 ulOpID)
{
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo = VOS_NULL_PTR;

    pstMsCapInfo = TAF_SDC_GetMsCapInfo();
    TAF_SDC_InitSvlteSupportFlag(TAF_SDC_INIT_CTX_STARTUP, pstMsCapInfo);

    if (VOS_TRUE == TAF_SDC_GetSvlteSupportFlag())
    {
        /* 直接写8214 nv项*/
        TAF_MMA_WritePlmnSelectionModeNvim(MMA_MMC_PLMN_SEL_MODE_AUTO);

        TAF_MMA_SetPlmnSelectionMode(MMA_MMC_PLMN_SEL_MODE_AUTO);
            
        /* 直接上报结果 */
        g_StatusContext.PhoneError    = TAF_ERR_NO_ERROR;
        g_StatusContext.ucOperateType = STA_OP_PLMNRESELMODE;
        Sta_EventReport(ulOpID, TAF_PH_EVT_PLMN_RESEL_CNF);
        return STA_SUCCESS;
    }

    /* svlte nv关闭上报error */
    g_StatusContext.PhoneError    = TAF_ERR_PHONE_MSG_UNMATCH;
    g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

    Sta_EventReport (ulOpID, TAF_PH_EVT_PLMN_RESEL_CNF);
    return STA_ERROR;
}
VOS_UINT32  TAF_MMA_RcvPlmnSelectionReq_PowerOff(
    VOS_UINT32                          ulOpID,
    TAF_PLMN_ID_STRU                    stPlmnId,
    TAF_PH_RA_MODE                      ucAccessMode,
    VOS_UINT8                           ucReselMode
)
{
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo = VOS_NULL_PTR;

    pstMsCapInfo = TAF_SDC_GetMsCapInfo();
    TAF_SDC_InitSvlteSupportFlag(TAF_SDC_INIT_CTX_STARTUP, pstMsCapInfo);

    if (VOS_TRUE == TAF_SDC_GetSvlteSupportFlag())
    {
        TAF_MMA_SndMmcPlmnSpecialReq((MMA_MMC_PLMN_ID_STRU *)&stPlmnId, ucAccessMode);
        g_StatusContext.ulCurOpId = ulOpID;
        g_StatusContext.StaPlmnId.Mcc = stPlmnId.Mcc;
        g_StatusContext.StaPlmnId.Mnc = stPlmnId.Mnc;

        return STA_SUCCESS;
    }

    /* svlte nv关闭上报error */
    g_StatusContext.ucOperateType = STA_OP_PHONEERROR;

    Sta_EventReport (ulOpID, TAF_PH_EVT_PLMN_SEL_CNF);
    return STA_ERROR;

}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

