


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "NasLmmPubMTimer.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASMMPUBMTMER_C
/*lint +e767*/

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/


/*****************************************************************************
  3 Function
*****************************************************************************/
/*****************************************************************************
  3.1 状态定时器 Function
*****************************************************************************/


/*lint -e960*/
/*lint -e961*/
VOS_VOID    NAS_LMM_StartStateTimer(
                    NAS_LMM_STATE_TI_ENUM_UINT16             enStateTimerId )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimer;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulParam;            /* 定时器类型标识 */

    /*获取该状态定时器控制块地址*/
    pstStateTimer = NAS_LMM_GetStateTimerAddr(enStateTimerId);
    /*无法获取该定时器控制块地址，说明定时器ID非法*/
    if ( VOS_NULL_PTR == pstStateTimer )
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_StartStateTimer, pstStateTimer = 0");
        return;
    }

    /*检查该状态定时器是否在运行，如果正在运行，先停止该定时器。
      停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
    if ( VOS_NULL_PTR != pstStateTimer->psthTimer )
    {
        ulRslt = PS_STOP_REL_TIMER(&(pstStateTimer->psthTimer));
        /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
        if ( VOS_OK != ulRslt )
        {
            NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StartStateTimer, Start State Timer Failure!",
                                 enStateTimerId);
            return;
        }
    }

    /*如果时长为零，直接返回，不启动定时器*/
    if(0 == pstStateTimer->ulTimerLen)
    {
        NAS_LMM_PUBM_LOG1_WARN("NAS_LMM_StartStateTimer:TimerLen = 0; enStaTimerId = ",
                               enStateTimerId);
        return;
    }

    pstStateTimer->ucTimerType          = NAS_LMM_STATE_TIMER;
    ulParam                             = pstStateTimer->ucTimerType;

    /*
    */
    /*NAS-MM状态定时器无参数信息，启动定时器时将定时器参数设置为ulParam
      状态定时器都是非循环定时器，定时器模式为VOS_RELTIMER_NOLOOP*/
    ulRslt = PS_START_REL_TIMER(&(pstStateTimer->psthTimer),
                                        PS_PID_MM,
                                        pstStateTimer->ulTimerLen,
                                        enStateTimerId,
                                        ulParam,
                                        VOS_RELTIMER_NOLOOP);
    /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
    if ( VOS_OK != ulRslt )
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StartStateTimer, Start State Timer Failure!",
                             enStateTimerId);
        return;
    }

    pstStateTimer->ucTimerRs            = NAS_LMM_TIMER_RS_RUNNING;
    pstStateTimer->ucTimerSs            = NAS_LMM_TIMER_SS_NOT_SUSPENDING;

    /*
    NAS_LMM_PUBM_LOG2_NORM("NAS_LMM_StartStateTimer OK. StateTimerId: Timer Len:",
                           enStateTimerId,pstStateTimer->ulTimerLen);
    */


    /* 定时器状态勾包出来 */
    NAS_LMM_SndOmEmmTimerStatus(NAS_LMM_TIMER_RUNNING, (VOS_UINT16)enStateTimerId
                                , pstStateTimer->ulTimerLen);

    return;
}
VOS_VOID    NAS_LMM_StopStateTimer(
                    NAS_LMM_STATE_TI_ENUM_UINT16             enStateTimerId )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimer;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTimerRemainLen;

    /*获取该状态定时器控制块地址*/
    pstStateTimer = NAS_LMM_GetStateTimerAddr(enStateTimerId);
    /*无法获取该定时器控制块，说明定时器ID非法*/
    if ( VOS_NULL_PTR == pstStateTimer )
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopStateTimer, pstStateTimer = 0.enStateTimerId: ",
                             enStateTimerId);
        return;
    }

    /*检查该状态定时器是否在运行，如果正在运行，停止该定时器。
      停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
    if ( VOS_NULL_PTR != pstStateTimer->psthTimer )
    {
        /* 获取当前定时器的剩余时间 */
        if (VOS_OK != VOS_GetRelTmRemainTime(&(pstStateTimer->psthTimer), &ulTimerRemainLen ))
        {
            ulTimerRemainLen = 0;
        }
        ulRslt = PS_STOP_REL_TIMER(&(pstStateTimer->psthTimer));
        /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
        if ( VOS_OK != ulRslt )
        {
            NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopStateTimer, Stop State Timer Failure!",
                                 enStateTimerId);
            return;
        }

        pstStateTimer->ucTimerRs        = NAS_LMM_TIMER_RS_NOT_RUNNING;
        pstStateTimer->ucTimerSs        = NAS_LMM_TIMER_SS_NOT_SUSPENDING;

        /* 定时器状态勾包出来 */
        NAS_LMM_SndOmEmmTimerStatus(NAS_LMM_TIMER_STOPED, (VOS_UINT16)enStateTimerId
                                    , ulTimerRemainLen);

    }
    return;
}
VOS_VOID    NAS_LMM_SuspendStateTimer(
                    NAS_LMM_STATE_TI_ENUM_UINT16             enStateTimerId )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimer;
    VOS_UINT32                          ulTick;
    VOS_UINT32                          ulRslt;

    /*获取该状态定时器控制块地址*/
    pstStateTimer = NAS_LMM_GetStateTimerAddr(enStateTimerId);
    /*无法获取该定时器控制块地址，说明定时器ID非法*/
    if ((VOS_NULL_PTR == pstStateTimer))
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_SuspendStateTimer, pstStateTimer = 0");
        return;
    }

    /*检查该状态定时器是否在运行，如果正在运行，先获取计时器的剩余时长。
      停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
    if ( VOS_NULL_PTR != pstStateTimer->psthTimer )
    {
        ulRslt = VOS_GetRelTmRemainTime(&(pstStateTimer->psthTimer),
                                        &ulTick);
        /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
        if ( VOS_OK != ulRslt )
        {
            NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_SuspendStateTimer,VOS_GetRelTmRemainTime.enStateTimerId: ",
                                 enStateTimerId);
            return;
        }

        ulRslt = PS_STOP_REL_TIMER(&(pstStateTimer->psthTimer));
        /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
        if ( VOS_OK != ulRslt )
        {
            NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_SuspendStateTimer,VOS_StopRelTimer.enStateTimerId: ",
                                 enStateTimerId);
            return;
        }

        pstStateTimer->ucTimerType      = NAS_LMM_STATE_TIMER;
        pstStateTimer->ucTimerSs        = NAS_LMM_TIMER_SS_SUSPENDING;
        pstStateTimer->ulTimerRemainLen = VOS_TmTickToMillSec(ulTick);
    }

    return;
}


VOS_VOID    NAS_LMM_ResumeStateTimer(
                    NAS_LMM_STATE_TI_ENUM_UINT16             enStateTimerId )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimer;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulParam;            /* 定时器类型标识 */

    /*获取该状态定时器控制块地址*/
    pstStateTimer = NAS_LMM_GetStateTimerAddr(enStateTimerId);
    /*无法获取该定时器控制块地址，说明定时器ID非法*/
    if ( VOS_NULL_PTR == pstStateTimer )
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_ResumeStateTimer, pstStateTimer = 0");
        return;
    }

    /*检查该状态定时器是否被挂起*/
    if (    (VOS_NULL_PTR               == pstStateTimer->psthTimer)
         && (NAS_LMM_TIMER_RS_NOT_RUNNING == pstStateTimer->ucTimerRs)
         && (NAS_LMM_TIMER_SS_SUSPENDING == pstStateTimer->ucTimerSs) )
    {
        pstStateTimer->ucTimerType      = NAS_LMM_STATE_TIMER;
        ulParam                         = pstStateTimer->ucTimerType;

        /*NAS-MM状态定时器无参数信息，启动定时器时将定时器参数设置为ulParam
          状态定时器都是非循环定时器，定时器模式为VOS_RELTIMER_NOLOOP*/
        ulRslt = PS_START_REL_TIMER(&(pstStateTimer->psthTimer),
                                        PS_PID_MM,
                                        pstStateTimer->ulTimerRemainLen,
                                        enStateTimerId,
                                        ulParam,
                                        VOS_RELTIMER_NOLOOP );
        /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
        if ( VOS_OK != ulRslt )
        {
            NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_ResumeStateTimer, Start State Timer Failure!",
                                 enStateTimerId);
            return;
        }

         /* 同时修改g_stMmMainContext中对应定时器的运行标识,即
        g_stMmMainContext.astMmTimerSta */
        pstStateTimer->ucTimerRs              = NAS_LMM_TIMER_RS_RUNNING;
    }
    else
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_ResumeStateTimer, State Timer is not suspended!",
                             enStateTimerId);
        return;
    }

    pstStateTimer->ucTimerSs            = NAS_LMM_TIMER_SS_NOT_SUSPENDING;
    pstStateTimer->ulTimerRemainLen     = NAS_LMM_TIMER_ZERO_VALUE;

    return;
}


VOS_VOID    NAS_LMM_ModifyStateTimer(
                    NAS_LMM_STATE_TI_ENUM_UINT16             enStateTimerId,
                    VOS_UINT32                              ulTimerLen )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimer;

    pstStateTimer = NAS_LMM_GetStateTimerAddr(enStateTimerId);
    /*无法获取该定时器控制块，说明定时器ID非法*/
    if ( VOS_NULL_PTR == pstStateTimer )
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_ModifyStateTimer, pstStateTimer = 0.enStateTimerId: ",
                             enStateTimerId);
        return;
    }

    pstStateTimer->ulTimerLen           = ulTimerLen;
    pstStateTimer->ucTimerType          = NAS_LMM_STATE_TIMER;

    NAS_LMM_PUBM_LOG2_NORM("NAS_LMM_ModifyStateTimer OK. StateTimerId, StateTimerLength",
                          enStateTimerId,
                          pstStateTimer->ulTimerLen );

    return;
}
VOS_VOID    NAS_LMM_StopAllStateTimer( VOS_VOID )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimerList;
    VOS_UINT32                          ulTcbIdxLoop;
    VOS_UINT32                          ulRslt;

    /*获取状态定时器列表首地址*/
    pstStateTimerList = NAS_LMM_GetStateTimerListAddr();

    /*停止所有处于运行状态的协议定时器*/
    for ( ulTcbIdxLoop = TI_NAS_EMM_STATE_NO_TIMER + 1; ulTcbIdxLoop < NAS_LMM_STATE_TI_BUTT; ulTcbIdxLoop++ )
    {
        /*检查该状态定时器是否在运行，如果正在运行，停止该定时器。
          停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
        if ( VOS_NULL_PTR != pstStateTimerList[ulTcbIdxLoop].psthTimer )
        {
            ulRslt = PS_STOP_REL_TIMER(&(pstStateTimerList[ulTcbIdxLoop].psthTimer));
            /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
            if ( VOS_OK != ulRslt )
            {
                /* 启动失败，打印失败信息 */
                NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopAllStateTimer, Stop pstStateTimerList[%d] Failure!",
                                     ulTcbIdxLoop);
            }

            pstStateTimerList[ulTcbIdxLoop].ucTimerRs = NAS_LMM_TIMER_RS_NOT_RUNNING;
            pstStateTimerList[ulTcbIdxLoop].ucTimerSs = NAS_LMM_TIMER_SS_NOT_SUSPENDING;
        }
    }

    return;
}
/*****************************************************************************
 Function Name   : NAS_LMM_SuspendInitStopPtlTimer
 Description     : 在挂起时停止除3412,3402,3423外的所有协议定时器
 Input           : VOS_VOID
 Output          : None
 Return          : None

 History         :
    1.FTY          2012-02-15  Draft Enact

*****************************************************************************/
VOS_VOID    NAS_LMM_SuspendInitStopPtlTimer (VOS_VOID )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstPtlTimerList;
    VOS_UINT32                          ulTcbIdxLoop;
    VOS_UINT32                          ulRslt;

    /*获取状态定时器列表首地址*/
    pstPtlTimerList = NAS_LMM_GetPtlTimerListAddr();


    NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_SuspendInitStopPtlTimer. 3412",
                                      TI_NAS_EMM_PTL_T3412);
   NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_SuspendInitStopPtlTimer. 3402",
                                      TI_NAS_EMM_PTL_T3402);

    /*停止所有处于运行状态的协议定时器*/
    for (   ulTcbIdxLoop = TI_NAS_LMM_PTL_TI_PUB_BOUNDARY_START +1;
            ulTcbIdxLoop < NAS_LMM_PTL_TI_BUTT;
            ulTcbIdxLoop++ )
    {
        /*检查该协议定时器是否在运行，如果正在运行，停止该定时器。
         停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
        if ( VOS_NULL_PTR != pstPtlTimerList[ulTcbIdxLoop].psthTimer )
        {
            /* 不停止T3411,T3412,T3423,T3402, T3416*/
            if(  (TI_NAS_EMM_PTL_T3411 == (pstPtlTimerList[ulTcbIdxLoop].ulName))
               ||(TI_NAS_EMM_PTL_T3412 == (pstPtlTimerList[ulTcbIdxLoop].ulName))
               ||(TI_NAS_EMM_PTL_T3402 == (pstPtlTimerList[ulTcbIdxLoop].ulName))
               ||(TI_NAS_EMM_PTL_T3423 == (pstPtlTimerList[ulTcbIdxLoop].ulName))
               ||(TI_NAS_EMM_PTL_T3416 == (pstPtlTimerList[ulTcbIdxLoop].ulName)))
            {
                NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_SuspendInitStopPtlTimer. ",
                                      pstPtlTimerList[ulTcbIdxLoop].ulName);

                continue;
            }

            ulRslt = PS_STOP_REL_TIMER(&(pstPtlTimerList[ulTcbIdxLoop].psthTimer));

            /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
            if ( VOS_OK != ulRslt )
            {
                NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopAllPtlTimer, Stop pstPtlTimerList[%d] Failure!",
                                     ulTcbIdxLoop);
            }

            pstPtlTimerList[ulTcbIdxLoop].ucTimerRs = NAS_LMM_TIMER_RS_NOT_RUNNING;
            pstPtlTimerList[ulTcbIdxLoop].ucTimerSs = NAS_LMM_TIMER_SS_NOT_SUSPENDING;
        }
    }

    return;
}

/*****************************************************************************
 Function Name   : NAS_LMM_StopAllStateTimer
 Description     : 在挂起时停止除3411外的所有状态定时器
 Input           : VOS_VOID
 Output          : None
 Return          : None

 History         :
    1.FTY          2012-02-15  Draft Enact

*****************************************************************************/
VOS_VOID    NAS_LMM_SuspendInitStopStateTimer (VOS_VOID )
{
    NAS_LMM_TIMER_CTRL_STRU            *pstStateTimerList;
    VOS_UINT32                          ulTcbIdxLoop;
    VOS_UINT32                          ulRslt;

    /*获取状态定时器列表首地址*/
    pstStateTimerList = NAS_LMM_GetStateTimerListAddr();

    /*停止除STATE_SERVICE_T3442以及STATE_DEL_FORB_TA_PROID外所有处于运行状态的协议定时器*/
    for ( ulTcbIdxLoop = TI_NAS_EMM_STATE_NO_TIMER + 1; ulTcbIdxLoop < NAS_LMM_STATE_TI_BUTT; ulTcbIdxLoop++ )
    {
        /*检查该状态定时器是否在运行，如果正在运行，停止该定时器。
          停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
        if ( VOS_NULL_PTR != pstStateTimerList[ulTcbIdxLoop].psthTimer )
        {
            /* 若是STATE_SERVICE_T3442以及STATE_DEL_FORB_TA_PROID，不停定时器，继续判断剩余定时器 */
            if( (TI_NAS_EMM_STATE_SERVICE_T3442 == (pstStateTimerList[ulTcbIdxLoop].ulName))
                || (TI_NAS_EMM_STATE_DEL_FORB_TA_PROID == (pstStateTimerList[ulTcbIdxLoop].ulName)))
            {
                NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_StopAllStateTimerExcept3411. ",
                                      pstStateTimerList[ulTcbIdxLoop].ulName);
                continue;
            }

            ulRslt = PS_STOP_REL_TIMER(&(pstStateTimerList[ulTcbIdxLoop].psthTimer));
            /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
            if ( VOS_OK != ulRslt )
            {
                /* 启动失败，打印失败信息 */
                NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopAllStateTimer, Stop pstStateTimerList[%d] Failure!",
                                     ulTcbIdxLoop);
            }

            pstStateTimerList[ulTcbIdxLoop].ucTimerRs = NAS_LMM_TIMER_RS_NOT_RUNNING;
            pstStateTimerList[ulTcbIdxLoop].ucTimerSs = NAS_LMM_TIMER_SS_NOT_SUSPENDING;
        }
    }

    return;
}
/*****************************************************************************
  3.2 协议定时器 Function
*****************************************************************************/

VOS_VOID    NAS_LMM_StartPtlTimer(
                    NAS_LMM_PTL_TI_ENUM_UINT16               enPtlTimerId )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstPtlTimer;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulParam;            /* 定时器类型标识 */

    /*获取该协议定时器控制块地址*/
    pstPtlTimer = NAS_LMM_GetPtlTimerAddr(enPtlTimerId);
    /*无法获取该协议定时器控制块地址，说明该协议定时器ID非法*/
    if ( VOS_NULL_PTR == pstPtlTimer )
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_StartPtlTimer, pstPtlTimer = 0");
        return;
    }

    /*检查该协议定时器是否在运行，如果正在运行，先停止该协议定时器。
      停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
    if ( VOS_NULL_PTR != pstPtlTimer->psthTimer )
    {
        ulRslt = PS_STOP_REL_TIMER(&(pstPtlTimer->psthTimer));
        /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
        if ( VOS_OK != ulRslt )
        {
            NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StartPtlTimer, Start Ptl Timer Failure!",
                                 enPtlTimerId);
            return;
        }
    }

    /*定时器时长不能超过18小时或者为零*/
    if((TI_NAS_LMM_TIMER_MAX_LEN < pstPtlTimer->ulTimerLen)
    ||(0 == pstPtlTimer->ulTimerLen))
    {
        NAS_LMM_PUBM_LOG2_WARN("NAS_LMM_StartPtlTimer:TimerLen ERR; enPtlTimerId, TimerLength",
                              enPtlTimerId,
                              pstPtlTimer->ulTimerLen);
        return;
    }

    pstPtlTimer->ucTimerType            = NAS_LMM_PTL_TIMER;
    ulParam                             = pstPtlTimer->ucTimerType;

    /*NAS-MM协议定时器无参数信息，启动定时器时将定时器参数设置为0*/
    ulRslt = PS_START_REL_TIMER(&(pstPtlTimer->psthTimer),
                                        PS_PID_MM,
                                        pstPtlTimer->ulTimerLen,
                                        enPtlTimerId,
                                        ulParam,
                                        pstPtlTimer->ucMode);
    /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
    if ( VOS_OK != ulRslt )
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StartPtlTimer, Start State Timer Failure!",
                             enPtlTimerId);
        return;
    }

    pstPtlTimer->ucTimerRs            = NAS_LMM_TIMER_RS_RUNNING;
    pstPtlTimer->ucTimerSs            = NAS_LMM_TIMER_SS_NOT_SUSPENDING;

    NAS_LMM_PUBM_LOG2_NORM("NAS_LMM_StartPtlTimer OK. PtlTimerId: Timer Len:",
                           enPtlTimerId,pstPtlTimer->ulTimerLen);

    /* 定时器状态勾包出来 */
    NAS_LMM_SndOmEmmTimerStatus(NAS_LMM_TIMER_RUNNING, (VOS_UINT16)enPtlTimerId
                                , pstPtlTimer->ulTimerLen);



    return;
}
VOS_VOID    NAS_LMM_StopPtlTimer(
                    NAS_LMM_PTL_TI_ENUM_UINT16               enPtlTimerId )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstPtlTimer;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTimerRemainLen;

    /*获取该协议定时器控制块地址*/
    pstPtlTimer = NAS_LMM_GetPtlTimerAddr(enPtlTimerId);
    /*无法获取该协议定时器控制块，说明该协议定时器ID非法*/
    if ( VOS_NULL_PTR == pstPtlTimer )
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopPtlTimer, pstPtlTimer = 0.enPtlTimerId: ",
                             enPtlTimerId);
        return;
    }

    /*检查该协议定时器是否在运行，如果正在运行，停止该定时器。
      停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
    if ( VOS_NULL_PTR != pstPtlTimer->psthTimer )
    {
        /* 获取当前定时器的剩余时间 */
        if (VOS_OK != VOS_GetRelTmRemainTime(&(pstPtlTimer->psthTimer), &ulTimerRemainLen ))
        {
            ulTimerRemainLen = 0;
        }

        ulRslt = PS_STOP_REL_TIMER(&(pstPtlTimer->psthTimer));
        /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
        if ( VOS_OK != ulRslt )
        {
            NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopPtlTimer, Stop Ptl Timer Failure!",
                                 enPtlTimerId);
            return;
        }

        pstPtlTimer->ucTimerRs = NAS_LMM_TIMER_RS_NOT_RUNNING;
        pstPtlTimer->ucTimerSs = NAS_LMM_TIMER_SS_NOT_SUSPENDING;

        /* 定时器状态勾包出来 */
        NAS_LMM_SndOmEmmTimerStatus(NAS_LMM_TIMER_STOPED, (VOS_UINT16)enPtlTimerId
                                    , ulTimerRemainLen);


    }

    return;
}
VOS_VOID  NAS_LMM_StartInactivePtlTimer( NAS_LMM_PTL_TI_ENUM_UINT16           enPtlTimerId)
{
    NAS_LMM_TIMER_CTRL_STRU              *pstPtlTimer;

    /*获取该协议定时器控制块地址*/
    pstPtlTimer = NAS_LMM_GetPtlTimerAddr(enPtlTimerId);

    /*无法获取该协议定时器控制块，说明该协议定时器ID非法*/
    if ( VOS_NULL_PTR == pstPtlTimer )
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StartInactivePtlTimer, pstPtlTimer = 0.enPtlTimerId: ",
                             enPtlTimerId);
        return;
    }

    /*检查该协议定时器是否运行，如果不运行则启动*/
    if ( VOS_NULL_PTR == pstPtlTimer->psthTimer )
    {
        NAS_LMM_StartPtlTimer(enPtlTimerId);
    }

    return;
}
VOS_VOID  NAS_LMM_StartInactiveStateTimer( NAS_LMM_STATE_TI_ENUM_UINT16 enStateTimerId )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimer;

    /*获取该状态定时器控制块地址*/
    pstStateTimer = NAS_LMM_GetStateTimerAddr(enStateTimerId);

    /*无法获取该定时器控制块地址，说明定时器ID非法*/
    if ( VOS_NULL_PTR == pstStateTimer )
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_StartInactiveStateTimer, pstStateTimer = 0");
        return;
    }

    /*检查状态定时器是否运行，如果不运行则启动*/
    if ( VOS_NULL_PTR == pstStateTimer->psthTimer )
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_StartInactiveStateTimer: timer");
        NAS_LMM_StartStateTimer(enStateTimerId);
    }

    return;
}
VOS_VOID    NAS_LMM_ModifyPtlTimer(
                    NAS_LMM_PTL_TI_ENUM_UINT16               enPtlTimerId,
                    VOS_UINT32                              ulTimerLen )
{
    NAS_LMM_TIMER_CTRL_STRU             *pstPtlTimer;

    pstPtlTimer = NAS_LMM_GetPtlTimerAddr(enPtlTimerId);

    /*无法获取该协议定时器控制块，说明该协议定时器ID非法*/
    if ( VOS_NULL_PTR == pstPtlTimer )
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_ModifyPtlTimer Error: pstPtlTimer = 0.enPtlTimerId: ",
                             enPtlTimerId);
        return;
    }

    /*修改 NVIM 时长成功，修改全局变量*/
    pstPtlTimer->ulTimerLen           = ulTimerLen;
    pstPtlTimer->ucTimerType          = NAS_LMM_PTL_TIMER;

    /*
    NAS_LMM_PUBM_LOG2_NORM("NAS_LMM_ModifyPtlTimer OK. PtlTimerId, PtlTimerLength",
                          enPtlTimerId,
                          pstPtlTimer->ulTimerLen );
    */

    return;
}

VOS_VOID    NAS_LMM_StopAllPtlTimer( VOS_VOID )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstPtlTimerList;
    VOS_UINT32                          ulTcbIdxLoop;
    VOS_UINT32                          ulRslt;

    /*获取状态定时器列表首地址*/
    pstPtlTimerList = NAS_LMM_GetPtlTimerListAddr();

    /*停止所有处于运行状态的协议定时器*/
    for ( ulTcbIdxLoop = TI_NAS_LMM_PTL_TI_PUB_BOUNDARY_START +1; ulTcbIdxLoop < NAS_LMM_PTL_TI_BUTT; ulTcbIdxLoop++ )
    {
        /*检查该协议定时器是否在运行，如果正在运行，停止该定时器。
          停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
        if ( VOS_NULL_PTR != pstPtlTimerList[ulTcbIdxLoop].psthTimer )
        {
            ulRslt = PS_STOP_REL_TIMER(&(pstPtlTimerList[ulTcbIdxLoop].psthTimer));

            /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
            if ( VOS_OK != ulRslt )
            {
                NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopAllPtlTimer, Stop pstPtlTimerList[%d] Failure!",
                                     ulTcbIdxLoop);
            }

            pstPtlTimerList[ulTcbIdxLoop].ucTimerRs = NAS_LMM_TIMER_RS_NOT_RUNNING;
            pstPtlTimerList[ulTcbIdxLoop].ucTimerSs = NAS_LMM_TIMER_SS_NOT_SUSPENDING;
        }
    }

   return;
}



/*****************************************************************************
  3.3 定时器 Function
*****************************************************************************/

VOS_VOID    NAS_LMM_InitAllStateTimer( VOS_VOID )
{

    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_InitAllStateTimer          START INIT...");

    /*T3440*/
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_STATE_T3440,
                                        NAS_LMM_TIMER_T3440_LEN);
    /*PUBM*/
    /*USIM*/
    NAS_LMM_InitStateTimer(              TI_NAS_LMM_TIMER_WAIT_USIM_CNF,
                                        NAS_LMM_TIMER_WAIT_USIM_CNF_LEN);
    NAS_LMM_InitStateTimer(              TI_NAS_LMM_TIMER_WAIT_USIM_READY_START,
                                        NAS_LMM_TIMER_WAIT_USIM_READY_START_LEN);
    /*MRRC*/
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_MRRC_WAIT_RRC_CONN_CNF,
                                        NAS_LMM_TIMER_MRRC_WAIT_RRC_CONN_CNF_LEN);
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF,
                                        NAS_LMM_TIMER_MRRC_WAIT_RRC_REL_CNF_LEN);
    /*ATTACH*/
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_T3410,
                                        NAS_LMM_TIMER_ATTACH_T3410_LEN);
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_WAIT_ESM_PDN_RSP,
                                        NAS_LMM_TIMER_ATTACH_WAIT_ESM_PDN_RSP_LEN);
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_WAIT_ESM_BEARER_CNF,
                                        NAS_LMM_TIMER_ATTACH_WAIT_ESM_BEARER_CNF_LEN);
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_WAIT_RRC_DATA_CNF,
                                        NAS_LMM_TIMER_WAIT_RRC_DATA_CNF_LEN);
    /*DETACH*/
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_T3421,
                                        NAS_LMM_TIMER_DETACH_T3421_LEN);

    /*TAU*/
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_STATE_TAU_T3430,
                                        NAS_LMM_TIMER_TAU_T3430);

    /*SER*/
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_STATE_SERVICE_T3417,
                                        NAS_LMM_TIMER_SER_T3417_LEN);

    NAS_LMM_InitStateTimer(              TI_NAS_EMM_STATE_SERVICE_T3442,
                                        NAS_EMM_NULL);

    NAS_LMM_InitStateTimer(              TI_NAS_EMM_STATE_SERVICE_T3417_EXT,
                                        NAS_LMM_TIMER_SER_T3417_EXT_LEN);

    /*PLMN*/
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_WAIT_MMC_START_CNF_TIMER,
                                        NAS_LMM_TIMER_PLMN_WAIT_MMC_START_CNF_LEN);
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_WAIT_RRC_START_CNF_TIMER,
                                        NAS_LMM_TIMER_PLMN_WAIT_RRC_START_CNF_LEN);
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_WAIT_MMC_STOP_CNF_TIMER,
                                        NAS_LMM_TIMER_PLMN_WAIT_MMC_STOP_CNF_LEN);
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_WAIT_RRC_STOP_CNF_TIMER,
                                        NAS_LMM_TIMER_PLMN_WAIT_RRC_STOP_CNF_LEN);

    /*AUTH*/
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_T3418,
                                        NAS_LMM_TIMER_AUTH_T3418_LEN);
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_T3420,
                                        NAS_LMM_TIMER_AUTH_T3420_LEN);
    /*MMC*/
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_STATE_DEL_FORB_TA_PROID,
                                        NAS_LMM_TIMER_EMM_DEL_FORB_TA_PRIOD_LEN);
    /*SUSPEND RESUME */
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_RRCORI_WAIT_OTHER_SUSPEND_RSP_TIMER,
                                        NAS_LMM_TIMER_SUSPEND_RESUME_WAIT_LEN);

    NAS_LMM_InitStateTimer(              TI_NAS_EMM_WAIT_SUSPEND_END_TIMER,
                                        NAS_LMM_TIMER_SUSPEND_WAIT_END_LEN);

    NAS_LMM_InitStateTimer(              TI_NAS_EMM_MMCORI_WAIT_OTHER_SUSPEND_RSP_TIMER,
                                        NAS_LMM_TIMER_SUSPEND_RESUME_WAIT_LEN);

    NAS_LMM_InitStateTimer(              TI_NAS_EMM_SYSCFGORI_WAIT_OTHER_SUSPEND_RSP_TIMER,
                                        NAS_LMM_TIMER_SUSPEND_RESUME_WAIT_LEN);

    NAS_LMM_InitStateTimer(              TI_NAS_EMM_RRCRSM_WAIT_OTHER_RESUME_RSP_TIMER,
                                        NAS_LMM_TIMER_SUSPEND_RESUME_WAIT_LEN);

    NAS_LMM_InitStateTimer(              TI_NAS_EMM_SYSCFGRSM_WAIT_OTHER_RESUME_RSP_TIMER,
                                        NAS_LMM_TIMER_SUSPEND_RESUME_WAIT_LEN);

    /*当系统变换的时候，L->GU->L这样的场景，回到L的时候需要搜网的有时会长达15s,
      但是LNAS等待系统的时间超时的定时器当前只有5s，所以延长等待时间到20s*/
    NAS_LMM_InitStateTimer(              TI_NAS_EMM_WAIT_SYS_INFO_IND_TIMER,
                                        NAS_LMM_TIMER_WAIT_SYS_INFO_IND_LEN);

    NAS_LMM_InitStateTimer(              TI_NAS_EMM_STATE_WAIT_SYSCFG_CNF_TIMER,
                                        NAS_LMM_TIMER_SYSCFG_CNF_WAIT_LEN);

    NAS_LMM_InitStateTimer(              TI_NAS_EMMC_STATE_WAIT_PLMN_SRCH_CNF_TIMER,
                                        NAS_LMM_TIMER_PLMN_SRCH_CNF_WAIT_LEN);

   #if 0
    /*从 SIM读取 HPLMN 搜网周期*/
    usDataLen = sizeof(LNAS_LMM_NV_HPLMN_PERI_STRU);
    ulRslt = NAS_LMM_NvimRead(EN_NV_ID_HPLMN_PERI_FILE, &stNvHplmnPeri, &usDataLen);
    if((EN_NV_OK == ulRslt) && (NAS_EMM_BIT_SLCT == stNvHplmnPeri.bitOpHplmnPeri))
    {
        /*NAS_LMM_InitStateTimer(          TI_NAS_EMMC_STATE_HPLMN_SRCH,stNvHplmnPeri.ulHplmnPeriTimerLen);*/
        NAS_EMMC_GetMmcHplmnPeriod()     = stNvHplmnPeri.ulHplmnPeriTimerLen;
    }
    else
    {
       /* NAS_LMM_InitStateTimer(          TI_NAS_EMMC_STATE_HPLMN_SRCH,
                                        NAS_LMM_TIMER_MMC_DEFAULT_HPLMN_SRCH_PRIOD_LEN);*/
        NAS_EMMC_GetMmcHplmnPeriod()     = NAS_LMM_TIMER_MMC_DEFAULT_HPLMN_SRCH_PRIOD_LEN;
    }
    #endif
    return;
}


VOS_VOID    NAS_LMM_InitStateTimer(
                    NAS_LMM_STATE_TI_ENUM_UINT16             enStateTimerId,
                    VOS_UINT32                              ulTimerLen )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimer;

    pstStateTimer = NAS_LMM_GetStateTimerAddr(enStateTimerId);

    /*无法获取该定时器控制块，说明定时器ID非法*/
    if ( VOS_NULL_PTR == pstStateTimer )
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_InitStateTimer, pstStateTimer = 0.enStateTimerId: ",
                             enStateTimerId);
        return;
    }

    pstStateTimer->ulName               = enStateTimerId;
    pstStateTimer->ulTimerLen           = ulTimerLen;
    pstStateTimer->psthTimer            = VOS_NULL_PTR;
    pstStateTimer->ulParam              = NAS_LMM_TIMER_NOT_CARE;
    pstStateTimer->ulTimerRemainLen     = NAS_LMM_TIMER_ZERO_VALUE;
    pstStateTimer->ucMaxNum             = NAS_LMM_TIMER_NOT_CARE;
    pstStateTimer->ucExpedNum           = NAS_LMM_TIMER_ZERO_VALUE;
    pstStateTimer->ucMode               = VOS_RELTIMER_NOLOOP;
    pstStateTimer->ucTimerType          = NAS_LMM_STATE_TIMER;
    pstStateTimer->ucTimerRs            = NAS_LMM_TIMER_RS_NOT_RUNNING;
    pstStateTimer->ucTimerSs            = NAS_LMM_TIMER_SS_NOT_SUSPENDING;
    pstStateTimer->ucTimerVs            = NAS_LMM_TIMER_VS_NOT_NEED_TO_UPDATE;

    return;

}
VOS_VOID    NAS_LMM_InitAllPtlTimer( VOS_VOID )
{

    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_InitAllPtlTimer            START INIT...");

    NAS_LMM_InitPtlTimer(            TI_NAS_EMM_PTL_T3412,
                                    NAS_LMM_TIMER_T3412_LEN);
    NAS_LMM_InitPtlTimer(            TI_NAS_EMM_PTL_T3402,
                                    NAS_LMM_TIMER_T3402_LEN);
    NAS_LMM_InitPtlTimer(            TI_NAS_EMM_PTL_T3423,
                                    NAS_LMM_TIMER_T3423_LEN);
    NAS_LMM_InitPtlTimer(            TI_NAS_EMM_PTL_T3416,
                                    NAS_LMM_TIMER_AUTH_T3416_LEN);
    NAS_LMM_InitPtlTimer(            TI_NAS_EMM_PTL_SWITCH_OFF_TIMER,
                                    NAS_LMM_TIMER_DETACH_SWITCH_OFF_LEN);
    NAS_LMM_InitPtlTimer(            TI_NAS_EMM_PTL_REATTACH_DELAY,
                                     NAS_LMM_TIMER_REATTACH_DELAY_LEN);
    NAS_LMM_InitPtlTimer(            TI_NAS_EMM_PTL_T3411,
                                     NAS_LMM_TIMER_T3411_LEN);
	
    NAS_LMM_InitPtlTimer(            TI_NAS_EMM_PTL_CSFB_DELAY,
                                     NAS_LMM_TIMER_CSFB_DELAY_LEN);

    return;
}


VOS_VOID    NAS_LMM_InitPtlTimer(
                    NAS_LMM_PTL_TI_ENUM_UINT16             enPtlTimerId,
                    VOS_UINT32                            ulTimerLen )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstPtlTimer;

    pstPtlTimer = NAS_LMM_GetPtlTimerAddr(enPtlTimerId);

    /*无法获取该定时器控制块，说明定时器ID非法*/
    if ( VOS_NULL_PTR == pstPtlTimer )
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_InitPtlimer, pstPtlTimer = 0.enPtlTimerId: ",
                             enPtlTimerId);
        return;
    }

    pstPtlTimer->ulName               = enPtlTimerId;
    pstPtlTimer->ulTimerLen           = ulTimerLen;
    pstPtlTimer->psthTimer            = VOS_NULL_PTR;
    pstPtlTimer->ulParam              = NAS_LMM_TIMER_NOT_CARE;
    pstPtlTimer->ulTimerRemainLen     = NAS_LMM_TIMER_ZERO_VALUE;
    pstPtlTimer->ucMaxNum             = NAS_LMM_TIMER_NOT_CARE;
    pstPtlTimer->ucExpedNum           = NAS_LMM_TIMER_ZERO_VALUE;

    pstPtlTimer->ucTimerType          = NAS_LMM_PTL_TIMER;
    pstPtlTimer->ucTimerRs            = NAS_LMM_TIMER_RS_NOT_RUNNING;
    pstPtlTimer->ucTimerSs            = NAS_LMM_TIMER_SS_NOT_SUSPENDING;
    pstPtlTimer->ucTimerVs            = NAS_LMM_TIMER_VS_NOT_NEED_TO_UPDATE;
    pstPtlTimer->ucMode               = VOS_RELTIMER_NOLOOP;
    return;

}
VOS_VOID  NAS_LMM_StopAllEmmStateTimer(VOS_VOID)
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimerList;
    VOS_UINT32                          ulTcbIdxLoop;
    VOS_UINT32                          ulRslt;
    VOS_BOOL                            bFlag               = VOS_TRUE;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_StopAllEmmStateTimer");

    /*获取状态定时器列表首地址*/
    pstStateTimerList = NAS_LMM_GetStateTimerListAddr();

    /*停止所有处于运行状态的协议定时器*/
    for ( ulTcbIdxLoop = TI_NAS_EMM_STATE_NO_TIMER + 1; ulTcbIdxLoop < TI_NAS_EMM_STATE_TI_BUTT; ulTcbIdxLoop++ )
    {
        /*检查该状态定时器是否在运行，如果正在运行，停止该定时器。
          停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
        if ( VOS_NULL_PTR != pstStateTimerList[ulTcbIdxLoop].psthTimer )
        {
            ulRslt = PS_STOP_REL_TIMER(&(pstStateTimerList[ulTcbIdxLoop].psthTimer));
            /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
            if ( VOS_OK != ulRslt )
            {
                bFlag = VOS_FALSE;
                /* 启动失败，打印失败信息 */
                NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopAllEmmStateTimer, Stop pstStateTimerList[%d] Failure!",
                                     ulTcbIdxLoop);
            }

            pstStateTimerList[ulTcbIdxLoop].ucTimerRs = NAS_LMM_TIMER_RS_NOT_RUNNING;
            pstStateTimerList[ulTcbIdxLoop].ucTimerSs = NAS_LMM_TIMER_SS_NOT_SUSPENDING;
        }
    }

    NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_StopAllEmmStateTimer: bFlag =", bFlag);

    return;

}



VOS_VOID    NAS_LMM_StopAllEmmPtlTimer( VOS_VOID )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstPtlTimerList;
    VOS_UINT32                          ulTcbIdxLoop;
    VOS_UINT32                          ulRslt;

    /*获取状态定时器列表首地址*/
    pstPtlTimerList = NAS_LMM_GetPtlTimerListAddr();

    /*停止所有处于运行状态的协议定时器*/
    for (   ulTcbIdxLoop = TI_NAS_LMM_PTL_TI_PUB_BOUNDARY_START +1;
            ulTcbIdxLoop < TI_NAS_EMM_PTL_TI_BUTT;
            ulTcbIdxLoop++ )
    {
        /*检查该协议定时器是否在运行，如果正在运行，停止该定时器。
          停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
        if ( VOS_NULL_PTR != pstPtlTimerList[ulTcbIdxLoop].psthTimer )
        {
            ulRslt = PS_STOP_REL_TIMER(&(pstPtlTimerList[ulTcbIdxLoop].psthTimer));

            /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
            if ( VOS_OK != ulRslt )
            {
                NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopAllEmmPtlTimer, Stop pstPtlTimerList[%d] Failure!",
                                     ulTcbIdxLoop);
            }

            pstPtlTimerList[ulTcbIdxLoop].ucTimerRs = NAS_LMM_TIMER_RS_NOT_RUNNING;
            pstPtlTimerList[ulTcbIdxLoop].ucTimerSs = NAS_LMM_TIMER_SS_NOT_SUSPENDING;
        }
    }

   return;
}


/*****************************************************************************
 Function Name   : NAS_LMM_StopAllEmmPtlTimer
 Description     : 停止EMM所有的协议定时器
 Input           : VOS_VOID
 Output          : None
 Return          : NAS_LMM_SUCC   -- 成功
                   NAS_LMM_FAIL   -- 失败

 History         :
    1.Hanlufeng 41410      2011-04-28  Draft Enact

*****************************************************************************/
VOS_VOID    NAS_LMM_StopAllLmmTimerExcept3412_3423( VOS_VOID )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstPtlTimerList;
    VOS_UINT32                          ulTcbIdxLoop;
    VOS_UINT32                          ulRslt;

    /* 停止所有状态定时器 */
    NAS_LMM_StopAllStateTimer();


    /* 停止所有协议定时器 */

    /*获取状态定时器列表首地址*/
    pstPtlTimerList = NAS_LMM_GetPtlTimerListAddr();

    /*停止所有处于运行状态的协议定时器*/
    for (   ulTcbIdxLoop = TI_NAS_LMM_PTL_TI_PUB_BOUNDARY_START +1;
            ulTcbIdxLoop < NAS_LMM_PTL_TI_BUTT;
            ulTcbIdxLoop++ )
    {
        /*检查该协议定时器是否在运行，如果正在运行，停止该定时器。
          停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
        if ( VOS_NULL_PTR != pstPtlTimerList[ulTcbIdxLoop].psthTimer )
        {

            /* 若是T3412或3423，不停此2定时器，继续判断剩余定时器 */
            if(   (TI_NAS_EMM_PTL_T3412 == (pstPtlTimerList[ulTcbIdxLoop].ulName))
                ||(TI_NAS_EMM_PTL_T3423 == (pstPtlTimerList[ulTcbIdxLoop].ulName)))
            {
                NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_StopAllEmmPtlTimerExcept3412_3423. ",
                                      pstPtlTimerList[ulTcbIdxLoop].ulName);
                continue;
            }

            ulRslt = PS_STOP_REL_TIMER(&(pstPtlTimerList[ulTcbIdxLoop].psthTimer));

            /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
            if ( VOS_OK != ulRslt )
            {
                NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StopAllPtlTimer, Stop pstPtlTimerList[%d] Failure!",
                                     ulTcbIdxLoop);
            }

            pstPtlTimerList[ulTcbIdxLoop].ucTimerRs = NAS_LMM_TIMER_RS_NOT_RUNNING;
            pstPtlTimerList[ulTcbIdxLoop].ucTimerSs = NAS_LMM_TIMER_SS_NOT_SUSPENDING;

        }
    }




   return;
}
/*****************************************************************************
 Function Name   : NAS_LMM_StopAllEmmPtlTimer
 Description     : 在挂起时停止EMM定时器
 Input           : VOS_VOID
 Output          : None
 Return          : None

 History         :
    1.FTY         2012-02-21 Draft Enact
*****************************************************************************/
VOS_VOID    NAS_LMM_SuspendInitStopLmmTimer( VOS_VOID )
{
    /* 停止状态定时器 */
    NAS_LMM_SuspendInitStopStateTimer();

    /* 停止协议定时器 */
    NAS_LMM_SuspendInitStopPtlTimer();
}


VOS_VOID  NAS_LMM_GetStateTimerLen( NAS_LMM_STATE_TI_ENUM_UINT16   enStateTimerId,
                                   VOS_UINT32   *pulTimerLen)
{
    NAS_LMM_TIMER_CTRL_STRU             *pstStateTimer = VOS_NULL_PTR;


    /*获取该状态定时器控制块地址*/
    pstStateTimer = NAS_LMM_GetStateTimerAddr(enStateTimerId);
    /*无法获取该定时器控制块地址，说明定时器ID非法*/
    if ( VOS_NULL_PTR == pstStateTimer )
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_GetStateTimerLen, pstStateTimer无效");
        *pulTimerLen = 0;
        return;
    }

    /*获取定时器时长*/
    *pulTimerLen = pstStateTimer->ulTimerLen;
}
VOS_VOID  NAS_LMM_GetPtlTimerLen( NAS_LMM_PTL_TI_ENUM_UINT16   enPtlTimerId,
                                   VOS_UINT32   *pulTimerLen)
{
    NAS_LMM_TIMER_CTRL_STRU             *pstPtlTimer = VOS_NULL_PTR;


    /*获取该协议定时器控制块地址*/
    pstPtlTimer = NAS_LMM_GetPtlTimerAddr(enPtlTimerId);
    /*无法获取该定时器控制块地址，说明定时器ID非法*/
    if ( VOS_NULL_PTR == pstPtlTimer )
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_GetPtlTimerLen, pstPtlTimer无效");
        *pulTimerLen = 0;
        return;
    }

    /*获取定时器时长*/
    *pulTimerLen = pstPtlTimer->ulTimerLen;
}
NAS_LMM_TIMER_RUN_STA_ENUM_UINT32  NAS_LMM_IsStaTimerRunning(
                                    NAS_LMM_STATE_TI_ENUM_UINT16 enStateTimerId )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimer = VOS_NULL_PTR;

    /*获取该状态定时器控制块地址*/
    pstStateTimer = NAS_LMM_GetStateTimerAddr(enStateTimerId);
    if ( VOS_NULL_PTR == pstStateTimer )
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_IsStaTimerRunning: pstStateTimer NULL.");
        return NAS_LMM_TIMER_INVALID;
    }

    /*检查该状态定时器是否在运行*/
    if ( VOS_NULL_PTR != pstStateTimer->psthTimer )
    {
        return NAS_LMM_TIMER_RUNNING;
    }
    else
    {
        return NAS_LMM_TIMER_STOPED;
    }
}


NAS_LMM_TIMER_RUN_STA_ENUM_UINT32  NAS_LMM_IsPtlTimerRunning(
                                    NAS_LMM_PTL_TI_ENUM_UINT16 enPtlTimerId )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstPtlTimer = VOS_NULL_PTR;

    /*获取该协议定时器控制块地址*/
    pstPtlTimer = NAS_LMM_GetPtlTimerAddr(enPtlTimerId);
    if ( VOS_NULL_PTR == pstPtlTimer )
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_IsPtlTimerRunning: pstPtlTimer NULL.");
        return NAS_LMM_TIMER_INVALID;
    }

    /*检查该状态定时器是否在运行*/
    if ( VOS_NULL_PTR != pstPtlTimer->psthTimer )
    {
        return NAS_LMM_TIMER_RUNNING;
    }
    else
    {
        return NAS_LMM_TIMER_STOPED;
    }
}

VOS_VOID    NAS_LMM_SuspendAllPtlTimer( VOS_VOID )
{
    VOS_UINT32                          ulTcbIdxLoop;

    /*停止所有处于运行状态的协议定时器*/
    for ( ulTcbIdxLoop = TI_NAS_EMM_STATE_NO_TIMER + 1; ulTcbIdxLoop < TI_NAS_EMM_STATE_TI_BUTT; ulTcbIdxLoop++ )
    {
        /*检查该协议定时器是否在运行，如果正在运行，挂起该定时器*/
        NAS_LMM_SuspendStateTimer((NAS_LMM_STATE_TI_ENUM_UINT16) ulTcbIdxLoop);
    }
    for (   ulTcbIdxLoop = TI_NAS_LMM_PTL_TI_PUB_BOUNDARY_START +1;
            ulTcbIdxLoop < TI_NAS_EMM_PTL_TI_BUTT;
            ulTcbIdxLoop++ )
    {
        /*检查该协议定时器是否在运行，如果正在运行，挂起该定时器*/
        NAS_LMM_SuspendStateTimer((NAS_LMM_STATE_TI_ENUM_UINT16) ulTcbIdxLoop);
    }

   return;
}
VOS_VOID    NAS_LMM_SuspendAllPtlTimerExpT3412( VOS_VOID )
{
    VOS_UINT32                          ulTcbIdxLoop;

    /*停止所有处于运行状态的协议定时器*/
    for ( ulTcbIdxLoop = TI_NAS_EMM_STATE_NO_TIMER + 1; ulTcbIdxLoop < TI_NAS_EMM_STATE_TI_BUTT; ulTcbIdxLoop++ )
    {
        /*检查该协议定时器是否在运行，如果正在运行，挂起该定时器*/
        NAS_LMM_SuspendStateTimer((NAS_LMM_STATE_TI_ENUM_UINT16) ulTcbIdxLoop);
    }
    for (   ulTcbIdxLoop = TI_NAS_LMM_PTL_TI_PUB_BOUNDARY_START +1;
            ulTcbIdxLoop < TI_NAS_EMM_PTL_TI_BUTT;
            ulTcbIdxLoop++ )
    {
        if(ulTcbIdxLoop == TI_NAS_EMM_PTL_T3412)
        {
            continue;
        }
        /*检查该协议定时器是否在运行，如果正在运行，挂起该定时器*/
        NAS_LMM_SuspendStateTimer( (NAS_LMM_STATE_TI_ENUM_UINT16)ulTcbIdxLoop);
    }

   return;
}
VOS_VOID    NAS_LMM_ResumeAllPtlTimer( VOS_VOID )
{
    VOS_UINT32                          ulTcbIdxLoop;

    /*停止所有处于运行状态的协议定时器*/
    for ( ulTcbIdxLoop = TI_NAS_EMM_STATE_NO_TIMER + 1; ulTcbIdxLoop < TI_NAS_EMM_STATE_TI_BUTT; ulTcbIdxLoop++ )
    {
        /*检查该协议定时器是否挂起，如果正在挂起，恢复该定时器*/
        NAS_LMM_ResumeStateTimer((NAS_LMM_STATE_TI_ENUM_UINT16) ulTcbIdxLoop);
    }
    for (   ulTcbIdxLoop = TI_NAS_LMM_PTL_TI_PUB_BOUNDARY_START +1;
            ulTcbIdxLoop < TI_NAS_EMM_PTL_TI_BUTT;
            ulTcbIdxLoop++ )
    {
        /*检查该协议定时器是否挂起，如果正在挂起，恢复该定时器*/
        NAS_LMM_ResumeStateTimer((NAS_LMM_STATE_TI_ENUM_UINT16)ulTcbIdxLoop);
    }

   return;
}
VOS_VOID    NAS_LMM_StopAllAttachTimer( VOS_VOID )
{
    NAS_LMM_StopStateTimer( TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    NAS_LMM_StopStateTimer( TI_NAS_EMM_T3410);

    NAS_LMM_StopStateTimer( TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

    NAS_LMM_StopStateTimer( TI_NAS_EMM_WAIT_RRC_DATA_CNF);
}


VOS_VOID  NAS_LMM_SndOmEmmTimerStatus(
    NAS_LMM_TIMER_RUN_STA_ENUM_UINT32          enTimerStatus,
    VOS_UINT16                          enTimerId,
    VOS_UINT32                          ulTimerRemainLen
)
{
    NAS_EMM_TIMER_INFO_STRU            *pstMsg = VOS_NULL_PTR;

    pstMsg = (NAS_EMM_TIMER_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_EMM_TIMER_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_SndOmEmmTimerStatus: mem alloc fail!.");
        return;
    }

    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = PS_PID_MM;
    pstMsg->stMsgHeader.ulReceiverPid   = PS_PID_MM;
    pstMsg->stMsgHeader.ulLength        = sizeof(NAS_EMM_TIMER_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->stMsgHeader.ulMsgName       = enTimerId + PS_MSG_ID_EMM_TO_EMM_OM_BASE;
    pstMsg->enTimerStatus               = enTimerStatus;
    pstMsg->usTimerId                   = enTimerId;
    pstMsg->usRsv                       = 0;

    pstMsg->ulTimerRemainLen            = ulTimerRemainLen;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

}
/*lint +e961*/
/*lint +e960*/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

