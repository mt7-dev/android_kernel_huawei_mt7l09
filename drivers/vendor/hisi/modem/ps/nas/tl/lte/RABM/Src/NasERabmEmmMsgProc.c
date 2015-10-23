

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "NasERabmEmmMsgProc.h"
#include  "NasERabmRrcMsgProc.h"
#include  "NasERabmIpFilter.h"
#include  "NasERabmETcMsgProc.h"
#include  "LUPAppItf.h"
/*#include  "R_ITF_FlowCtrl.h"*/
#include  "NasERabmCdsMsgProc.h"
#include  "CdsInterface.h"



/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASRABMEMMMSGPROC_C
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


VOS_VOID NAS_ERABM_EmmMsgDistr( VOS_VOID *pRcvMsg )
{
    PS_MSG_HEADER_STRU         *pEmmMsg = VOS_NULL_PTR;

    pEmmMsg = (PS_MSG_HEADER_STRU*)pRcvMsg;

    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_EmmMsgDistr is entered.");

    /*根据消息名，调用相应的消息处理函数*/
    switch (pEmmMsg->ulMsgName)
    {
        /*如果收到的是EMM_ERABM_REEST_IND消息，调用NAS_ERABM_RcvRabmEmmReest_Ind函数*/
        case ID_EMM_ERABM_REEST_IND:
            NAS_ERABM_RcvRabmEmmReestInd((EMM_ERABM_REEST_IND_STRU*)pRcvMsg);
            break;

        /*如果收到的是EMM_ERABM_REL_IND消息，调用NAS_ERABM_RcvRabmEmmRelInd函数*/
        case ID_EMM_ERABM_REL_IND:
            NAS_ERABM_RcvRabmEmmRelInd();
            break;

        case ID_EMM_ERABM_RRC_CON_REL_IND:
            NAS_ERABM_RcvRabmEmmRrcConRelInd();
            break;

        case ID_EMM_ERABM_SUSPEND_IND:
            NAS_ERABM_RcvRabmEmmSuspendInd( (EMM_ERABM_SUSPEND_IND_STRU *) pRcvMsg );
            break;

        case ID_EMM_ERABM_RESUME_IND:
            NAS_ERABM_RcvRabmEmmResumeInd( (EMM_ERABM_RESUME_IND_STRU *) pRcvMsg );
            break;

        default:
            NAS_ERABM_WARN_LOG("NAS_ERABM_EmmMsgDistr:WARNING:EMM->RABM Message name non-existent!");
            break;
    }
}


/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_ERABM_RcvRabmEmmReestInd(const EMM_ERABM_REEST_IND_STRU *pRcvMsg )
{
    if (NAS_ERABM_L_MODE_STATUS_NORMAL != NAS_ERABM_GetLModeStatus())
    {
        NAS_ERABM_WARN_LOG("NAS_ERABM_RcvRabmEmmReestInd:Rabm is already suspended!");
        return ;
    }

    switch(pRcvMsg->enReEstState)
    {
        /*case EMM_ERABM_REEST_STATE_SUCC:*/
        case EMM_ERABM_REEST_STATE_FAIL:

            /*打印进入该函数*/
            NAS_ERABM_LOG1("NAS_ERABM_RcvRabmEmmReestInd: receive Emm Reset result: .",
                          pRcvMsg->enReEstState);
            /*SERVICE流程结束*,停止定时器*/
            NAS_ERABM_TimerStop(NAS_ERABM_WAIT_RB_REEST_TIMER);
            NAS_ERABM_SetEmmSrState(NAS_ERABM_SERVICE_STATE_TERMIN);
            /* 如果在正常模式下 */
            if (NAS_ERABM_MODE_TYPE_NORMAL == NAS_ERABM_GetMode())
            {
                /* 通知L2释放缓存的数据包 */
                /*APPITF_FreeAllofRabmBuf();*/

                /* 通知CDS释放缓存的数据包 */
                NAS_ERABM_SndErabmCdsFreeBuffDataInd();

                /* 清除上行数据阻塞标志*/
                NAS_ERABM_SetUpDataPending(EMM_ERABM_NO_UP_DATA_PENDING);
            }
            else if (NAS_ERABM_TIMER_STATE_STOPED == NAS_ERABM_IsTimerRunning(NAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER))
            /*环回模式下不清理缓存，保持原上行数据Pending状态*/
            {
                NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRabmEmmReestInd: NAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER start.");
                /* lihong00150010 emergency tau&service begin */
                NAS_ERABM_TimerStart(   NAS_ERABM_WAIT_TC_FREE_BUFFER_LENGTH,
                                        NAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER,
                                        NAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER);
            }   /* lihong00150010 emergency tau&service end */
            else
            {
                NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRabmEmmReestInd: NAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER is running.");
            }
            break;

        case EMM_ERABM_REEST_STATE_INITIATE:

            /*打印进入该函数*/
            NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRabmEmmReestInd: receive EMM_ERABM_REEST_STATE_INITIATE .");
            /*SERVICE流程启动,启动定时器*/
            if (NAS_ERABM_SERVICE_STATE_TERMIN == NAS_ERABM_GetEmmSrState())
            {/* lihong00150010 emergency tau&service begin */
                NAS_ERABM_TimerStart(   NAS_ERABM_WAIT_RB_REEST_LENGTH,
                                        NAS_ERABM_WAIT_RB_REEST_TIMER,
                                        VOS_FALSE);
                NAS_ERABM_SetEmmSrState(NAS_ERABM_SERVICE_STATE_INIT);
            }/* lihong00150010 emergency tau&service end */
            break;

        default:
            NAS_ERABM_WARN_LOG("NAS_ERABM_RcvRabmEmmReestInd:WARNNING: EstState illegal!");
            break;
    }
}
VOS_VOID  NAS_ERABM_RcvRabmEmmRelInd( VOS_VOID )
{
    VOS_UINT32 ulLoop = NAS_ERABM_NULL;

    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRabmEmmRelInd is entered.");

    /* 通知CDS RAB RELEASE */
    for (ulLoop = NAS_ERABM_MIN_EPSB_ID; ulLoop <= NAS_ERABM_MAX_EPSB_ID; ulLoop++)
    {
        if (NAS_ERABM_EPSB_ACTIVE == NAS_ERABM_GetEpsbStateInfo(ulLoop))
        {
            /* 给CDS发送CDS_ERABM_RAB_RELEASE_IND */
            NAS_ERABM_SndErabmCdsRabReleaseInd(ulLoop);
        }
    }

    /*释放所有资源,回到开机初始化后的状态*/
    NAS_ERABM_ClearRabmResource();

    /*根据TFT和承载信息更新IP Filter到硬件加速器*/
    if(PS_FAIL == NAS_ERABM_IpfConfigUlFilter())
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_EsmMsgDistr: IPF Update IP Filter Failed.");
    }

    /*根据TFT和承载信息更新IP Filter到CDS*/
    if(PS_FAIL == NAS_ERABM_CdsConfigUlFilter())
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_EsmMsgDistr: CDS Update IP Filter Failed.");
    }
    #if 0
    /* 配置下行TFT信息 */
    if(PS_FAIL == NAS_ERABM_IpfConfigDlFilter())
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_RcvRabmEmmResumeInd: Update DL IP Filter Failed.");
    }
    #endif
}
VOS_VOID  NAS_ERABM_RcvRabmEmmRrcConRelInd( VOS_VOID )
{
    VOS_UINT32                          ulEpsbId = NAS_ERABM_NULL;

    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRabmEmmRrcConRelInd is entered.");

    NAS_ERABM_TimerStop( NAS_ERABM_WAIT_RB_RESUME_TIMER);

    NAS_ERABM_TimerStop( NAS_ERABM_WAIT_RB_REEST_TIMER);

    NAS_ERABM_SetEmmSrState(NAS_ERABM_SERVICE_STATE_TERMIN);

    /*停止建立等待定时器*/
    NAS_ERABM_TimerStop(NAS_ERABM_WAIT_EPSB_ACT_TIMER);

    /*清除等待承载建立标识 */
    NAS_ERABM_SetWaitEpsBActSign(NAS_ERABM_NOT_WAIT_EPSB_ACT_MSG);


    /* 如果在正常模式下 */
    if (NAS_ERABM_MODE_TYPE_NORMAL == NAS_ERABM_GetMode())
    {
        /* 通知L2释放缓存的数据包 */
        /*APPITF_FreeAllofRabmBuf();*/

        /* 异系统切换成功后，RRC会上报链路释放清DRB，但此场景不能通知CDS清缓存 */
        if (NAS_ERABM_L_MODE_STATUS_NORMAL == NAS_ERABM_GetLModeStatus())
        {
            /* (1)如果当前有上行Pending 标识，即有数据缓存,则给CDS发释放缓存 */
            /* (2)存在已激活DRB,则给CDS发释放缓存 */
            if((EMM_ERABM_UP_DATA_PENDING == NAS_ERABM_GetUpDataPending())
               || ((NAS_ERABM_FAILURE == NAS_ERABM_IsAllActtiveBearerWithoutDrb())
                    && (NAS_ERABM_NULL != NAS_ERABM_GetActiveEpsBearerNum())))
            {
                /* 通知CDS释放缓存的数据包 */
                NAS_ERABM_SndErabmCdsFreeBuffDataInd();

                /* 清除上行数据阻塞标志 */
                NAS_ERABM_SetUpDataPending(EMM_ERABM_NO_UP_DATA_PENDING);
            }
        }

    }
    else  /*环回模式下不清理缓存，保持原上行数据Pending状态*/
    {
        NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRabmEmmRrcConRelInd: Do not release buffer .");
    }

    /* 释放所有RB */
    for (ulEpsbId = NAS_ERABM_MIN_EPSB_ID; ulEpsbId<= NAS_ERABM_MAX_EPSB_ID; ulEpsbId++)
    {
        NAS_ERABM_RcvRbRelease(ulEpsbId);
    }

    return;
}
VOS_VOID NAS_ERABM_SndRabmEmmReestReq
(
    VOS_UINT32                          ulIsEmcType
)
{
    EMM_ERABM_REEST_REQ_STRU    *pstReestReq = VOS_NULL_PTR;

    /*分配空间和检测是否分配成功*/
    pstReestReq = (VOS_VOID*)NAS_ERABM_ALLOC_MSG(
                                        sizeof(EMM_ERABM_REEST_REQ_STRU));
    if ( VOS_NULL_PTR == pstReestReq )
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_SndRabmEmmReestReq:ERROR:Alloc msg fail!" );
        return;
    }

    /*清空*/
    NAS_ERABM_MEM_SET(NAS_ERABM_GET_MSG_ENTITY(pstReestReq), NAS_ERABM_NULL,\
                     NAS_ERABM_GET_MSG_LENGTH(pstReestReq));

    /*填写消息头*/
    NAS_ERABM_WRITE_EMM_MSG_HEAD(pstReestReq, ID_EMM_ERABM_REEST_REQ);
	/* lihong00150010 emergency tau&service begin */
    pstReestReq->ulIsEmcType = ulIsEmcType;
	/* lihong00150010 emergency tau&service end */
    /* 调用消息发送函数 */
    NAS_ERABM_SND_MSG(pstReestReq);

}


VOS_VOID NAS_ERABM_SndRabmEmmDrbSetupInd( VOS_VOID )
{
    EMM_ERABM_DRB_SETUP_IND_STRU    *pstDrbSetInd = VOS_NULL_PTR;

    /*分配空间和检测是否分配成功*/
    pstDrbSetInd = (VOS_VOID*)NAS_ERABM_ALLOC_MSG(
                                        sizeof(EMM_ERABM_DRB_SETUP_IND_STRU));
    if ( VOS_NULL_PTR == pstDrbSetInd )
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_SndRabmEmmDrbSetupInd:ERROR:Alloc msg fail!" );
        return;
    }

    /*清空*/
    NAS_ERABM_MEM_SET(NAS_ERABM_GET_MSG_ENTITY(pstDrbSetInd), NAS_ERABM_NULL,\
                     NAS_ERABM_GET_MSG_LENGTH(pstDrbSetInd));

    /*填写消息头*/
    NAS_ERABM_WRITE_EMM_MSG_HEAD(pstDrbSetInd, ID_EMM_ERABM_DRB_SETUP_IND);

    /* 调用消息发送函数*/
    NAS_ERABM_SND_MSG(pstDrbSetInd);

}


VOS_VOID NAS_ERABM_SndRabmEmmRelReq( VOS_VOID)
{
    EMM_ERABM_REL_REQ_STRU    *pstRelReq  = VOS_NULL_PTR;

    /*分配空间和检测是否分配成功*/
    pstRelReq = (VOS_VOID*)NAS_ERABM_ALLOC_MSG(sizeof(EMM_ERABM_REL_REQ_STRU));
    if ( VOS_NULL_PTR == pstRelReq )
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_SndRabmEmmRelReq:ERROR:Alloc msg fail!" );
        return;
    }

    NAS_ERABM_MEM_SET(NAS_ERABM_GET_MSG_ENTITY(pstRelReq), NAS_ERABM_NULL,\
                     NAS_ERABM_GET_MSG_LENGTH(pstRelReq));

    /*填写消息头*/
    NAS_ERABM_WRITE_EMM_MSG_HEAD(pstRelReq, ID_EMM_ERABM_REL_REQ);

    /* 调用消息发送函数*/
    NAS_ERABM_SND_MSG(pstRelReq);

}

/*****************************************************************************
 Function Name   : NAS_ERABM_SndRabmEmmSuspendRsp
 Description     : ERABM模块回复ID_EMM_ERABM_SUSPEND_RSP消息
 Input           : EMM_ERABM_RSLT_TYPE_ENUM_UINT32       enRslt
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-05-03  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ERABM_SndRabmEmmSuspendRsp
(
    EMM_ERABM_RSLT_TYPE_ENUM_UINT32      enRslt
)
{
    EMM_ERABM_SUSPEND_RSP_STRU          *pstRabmEmmSuspendRsp  = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    pstRabmEmmSuspendRsp = (VOS_VOID*)NAS_ERABM_ALLOC_MSG(sizeof(EMM_ERABM_SUSPEND_RSP_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRabmEmmSuspendRsp)
    {
        /*打印异常信息*/
        NAS_ERABM_ERR_LOG("NAS_ERABM_SndRabmEmmSuspendRsp:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ERABM_MEM_SET( NAS_ERABM_GET_MSG_ENTITY(pstRabmEmmSuspendRsp), 0, NAS_ERABM_GET_MSG_LENGTH(pstRabmEmmSuspendRsp));

    /*填写消息头*/
    NAS_ERABM_WRITE_EMM_MSG_HEAD(pstRabmEmmSuspendRsp, ID_EMM_ERABM_SUSPEND_RSP);

    /*填写响应结果*/
    pstRabmEmmSuspendRsp->enRslt= enRslt;

    /*调用消息发送函数 */
    NAS_ERABM_SND_MSG(pstRabmEmmSuspendRsp);
}

/*****************************************************************************
 Function Name   : NAS_ERABM_SndRabmEmmResumeRsp
 Description     : ESM模块回复ID_EMM_ERABM_RESUME_RSP消息
 Input           : EMM_ERABM_RSLT_TYPE_ENUM_UINT32       enRslt
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-05-03  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ERABM_SndRabmEmmResumeRsp
(
    EMM_ERABM_RSLT_TYPE_ENUM_UINT32      enRslt
)
{
    EMM_ERABM_RESUME_RSP_STRU            *pstRabmEmmResumeRsp  = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    pstRabmEmmResumeRsp = (VOS_VOID*)NAS_ERABM_ALLOC_MSG(sizeof(EMM_ERABM_RESUME_RSP_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRabmEmmResumeRsp)
    {
        /*打印异常信息*/
        NAS_ERABM_ERR_LOG("NAS_ERABM_SndRabmEmmResumeRsp:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ERABM_MEM_SET( NAS_ERABM_GET_MSG_ENTITY(pstRabmEmmResumeRsp), 0, NAS_ERABM_GET_MSG_LENGTH(pstRabmEmmResumeRsp));

    /*填写消息头*/
    NAS_ERABM_WRITE_EMM_MSG_HEAD(pstRabmEmmResumeRsp, ID_EMM_ERABM_RESUME_RSP);

    /*填写响应结果*/
    pstRabmEmmResumeRsp->enRslt= enRslt;

    /*调用消息发送函数 */
    NAS_ERABM_SND_MSG(pstRabmEmmResumeRsp);
}
EMM_ERABM_UP_DATA_PENDING_ENUM_UINT32 NAS_ERABM_IsDataPending( VOS_VOID )
{
    /* 根据CDS记录是否有上行缓存数据，以及RABM记录是否有上行数据PENDING共同决定 */
    return (CDS_IsPsDataAvail()||NAS_ERABM_GetUpDataPending());
}

/*****************************************************************************
 Function Name   : NAS_ERABM_RcvRabmEmmSuspendInd
 Description     : ERABM模块收到ID_EMM_ERABM_SUSPEND_IND处理函数
 Input           : EMM_ERABM_SUSPEND_IND_STRU *pRcvMsg
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010            2011-05-03      Draft Enact
*****************************************************************************/
VOS_VOID  NAS_ERABM_RcvRabmEmmSuspendInd(const EMM_ERABM_SUSPEND_IND_STRU *pRcvMsg )
{
    VOS_UINT32                          ulEpsbId        = NAS_ERABM_NULL;
    VOS_UINT8                           ucTimerIndex    = NAS_ERABM_NULL;

    /* 设置状态为挂起态 */
    NAS_ERABM_SetLModeStatus(NAS_ERABM_L_MODE_STATUS_SUSPENDED);

    /* 停止定时器 */
    for (ucTimerIndex = 0; ucTimerIndex < NAS_NAS_ERABM_TIMER_NAME_BUTT; ucTimerIndex++)
    {
        NAS_ERABM_TimerStop(ucTimerIndex);
    }
    /* 如果是切换和CCO类型的挂起，则不删除DRB信息，只将DRB的状态设为NAS_ERABM_RB_SUSPENDED，
       因为L-GU切换或者CCO失败回退成功时，RRC并不上报LRRC_LRABM_RAB_IND消息，而
       只上报LRRC_LRABM_STATUS_IND消息恢复数传；其他类型的挂起，则删除DRB信息*/
    if ((EMM_ERABM_SYS_CHNG_TYPE_HO == pRcvMsg->enSysChngType)
         || (EMM_ERABM_SYS_CHNG_TYPE_CCO == pRcvMsg->enSysChngType))
    {
        /* 将DRB状态设为NAS_ERABM_RB_SUSPENDED */
        for (ulEpsbId = NAS_ERABM_MIN_EPSB_ID; ulEpsbId<= NAS_ERABM_MAX_EPSB_ID; ulEpsbId++)
        {
            NAS_ERABM_SetRbStateInfo(ulEpsbId, NAS_ERABM_RB_SUSPENDED);
        }
    }

    #if 0
    /* 设置用户面模式为NULL */
    if (PS_SUCC != UP_SetRanMode(RAN_MODE_NULL))
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_RcvRabmEmmSuspendInd: Update Ran Mode Failed.");
    }
    #endif

    /*清除等待EPS承载激活标识*/
    NAS_ERABM_SetWaitEpsBActSign(NAS_ERABM_NOT_WAIT_EPSB_ACT_MSG);

    /*SERVICE流程停止*/
    NAS_ERABM_SetEmmSrState(NAS_ERABM_SERVICE_STATE_TERMIN);

    /* 通知L2释放缓存的数据包 */
    /*APPITF_FreeAllofRabmBuf();*/

    /* 清除上行数据阻塞标志*/
    NAS_ERABM_SetUpDataPending(EMM_ERABM_NO_UP_DATA_PENDING);

    /* 通知ETC挂起 */
    NAS_ERABM_SndRabmTcSuspendInd();

    /* 回复EMM挂起成功 */
    NAS_ERABM_SndRabmEmmSuspendRsp(EMM_ERABM_RSLT_TYPE_SUCC);
}

/*****************************************************************************
 Function Name   : NAS_ERABM_RcvRabmEmmResumeInd
 Description     : SM模块收到ID_EMM_ERABM_RESUME_IND处理函数
 Input           : EMM_ERABM_RESUME_IND_STRU *pRcvMsg
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010            2011-05-03      Draft Enact
*****************************************************************************/
VOS_VOID  NAS_ERABM_RcvRabmEmmResumeInd(const EMM_ERABM_RESUME_IND_STRU *pRcvMsg )
{
    (VOS_VOID)pRcvMsg;

    /* 设置状态为正常态 */
    NAS_ERABM_SetLModeStatus(NAS_ERABM_L_MODE_STATUS_NORMAL);

    #if 0
    /* 设置用户面模式为NULL */
    if (PS_SUCC != UP_SetRanMode(RAN_MODE_LTE))
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_RcvRabmEmmResumeInd: Update Ran Mode Failed.");
    }

    /*设置LTE模流控参数*/
    R_ITF_SetLFlowLev();
    #endif

    /*根据TFT和承载信息更新IP Filter到硬件加速器*/
    if(PS_FAIL == NAS_ERABM_IpfConfigUlFilter())
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_EsmMsgDistr: IPF Update IP Filter Failed.");
    }

    /*根据TFT和承载信息更新IP Filter到CDS*/
    if(PS_FAIL == NAS_ERABM_CdsConfigUlFilter())
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_EsmMsgDistr: CDS Update IP Filter Failed.");
    }
    #if 0
    /* 配置下行TFT信息 */
    if(PS_FAIL == NAS_ERABM_IpfConfigDlFilter())
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_RcvRabmEmmResumeInd: Update DL IP Filter Failed.");
    }
    #endif

    /* 通知ETC解挂 */
    NAS_ERABM_SndRabmTcResumeInd();

    /* 回复EMM解挂成功 */
    NAS_ERABM_SndRabmEmmResumeRsp(EMM_ERABM_RSLT_TYPE_SUCC);
}
/*lint +e961*/
/*lint +e960*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

