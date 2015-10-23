


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasLmmPubMInclude.h"
#include    "NasLmmPubMPrint.h"
#include    "NasLmmPubMTimer.h"
#include    "Msp_diag_cmd_id.h"
#include    "NasEmmSmsInterface.h"
#include    "NasLmmCore.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASMMPUBMFSM_C
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
NAS_LMM_MAIN_CONTEXT_STRU                g_stMmMainContext;
NAS_LMM_FSM_DESC_STRU                    g_astNasMmFsmDesc[NAS_LMM_PARALLEL_FSM_BUTT];
VOS_UINT32                               g_ulMmcSerRegFlag;

/*****************************************************************************
  3 Function
*****************************************************************************/



/*lint -e960*/
/*lint -e961*/
VOS_UINT32 NAS_LMM_FsmRegisterFsm(       NAS_LMM_PARALLEL_FSM_ENUM_UINT16 enParaFsmId,
                                        VOS_UINT32                      ulSize,
                                        NAS_LMM_STA_STRU                *pStaTable,
                                        NAS_LMM_EXCEP_FUN                pfExceptHandle )
{
    NAS_LMM_FSM_DESC_STRU               *pFsmDesc;

    /* 检查入口参数 */
    if ( ( NAS_LMM_PARALLEL_FSM_BUTT <= enParaFsmId ) || ( NAS_LMM_NULL_PTR == pStaTable ) )
    {
        NAS_LMM_PUBM_LOG2_ERR("NAS_LMM_FsmRegisterFsm, The input is illegal! enParaFsmId, pStaTable",
                            enParaFsmId,
                            pStaTable);
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 指向指定的并行状态机的状态机描述符 */
    pFsmDesc                            = &g_astNasMmFsmDesc[enParaFsmId];


    /* 对状态机描述符结构进行赋值 */
    pFsmDesc->ulSize                    = ulSize;
    pFsmDesc->pStaTable                 = pStaTable;
    pFsmDesc->pfExceptHandle            = pfExceptHandle;
    pFsmDesc->aulMmParaFsmId            = enParaFsmId;


    /* 对状态转移表进行排序, UE 1.0 不进行排序*/
    /*
    NAS_LMM_FSM_StaSort(pStaTable, ulSize);
    */
    return NAS_LMM_SUCC;

}




VOS_UINT32 NAS_LMM_FsmProcessEvent(      NAS_LMM_PARALLEL_FSM_ENUM_UINT16 enParalFsmId,
                                        VOS_UINT32                      ulEventType,
                                        VOS_UINT32                      ulMsgID,
                                        VOS_UINT8                      *pRcvMsg )
{
    NAS_LMM_ACTION_FUN                   pActFun;
    NAS_LMM_FSM_DESC_STRU               *pFsmDesc;
    VOS_UINT32                          ulCurState;
    NAS_LMM_FSM_STATE_STRU              *pstFsmCtx  = NAS_LMM_NULL_PTR;




    /* 找到某并行状态机的描述表地址 */
    pFsmDesc = &(g_astNasMmFsmDesc[enParalFsmId]);

    /* 获取状态机地址,用于取状态 */
    pstFsmCtx = NAS_LMM_GetCurFsmAddr(enParalFsmId);

    /* 生成某并行状态机的当前状态 */
    ulCurState = NAS_LMM_PUB_COMP_EMMSTATE(pstFsmCtx->enMainState,
                                          pstFsmCtx->enSubState);


    /* 查找状态转移表中的相应的动作处理函数 */
    pActFun = NAS_LMM_FsmFindAct(pFsmDesc, ulCurState, ulEventType);
    if (NAS_LMM_NULL_PTR != pActFun)
    {
        return (*pActFun) (ulMsgID, pRcvMsg);
    }
    /* 如果找不到处理函数，并不表示异常，仅仅表示该消息没有被当前状态机处理，
        在这种情况下，需要调用该状态机的异常处理函数，看该消息能够在异常处理函数中处理。 */
    else
    {
        /* 异常函数处理， */
        if (NAS_LMM_NULL_PTR != pFsmDesc->pfExceptHandle)
        {

            return (*pFsmDesc->pfExceptHandle) (ulEventType, pRcvMsg);
        }
        /* 如果没有异常处理函数，表示该消息没有被该状态机处理 */
        else
        {
            return NAS_LMM_MSG_DISCARD;
        }
    }
}




/*****************************************************************************
 Function Name  : NAS_LMM_FsmFindAct
 Discription    : 根据状态和事件类型在排序后的状态转移表中快速查找事件处理函数
 Input          : 状态机描述符 状态 事件类型
 Output         : None
 Return         : 事件处理函数
 History:
      1.  Wang Yangcai 49178  2008-02-19 移植代码

*****************************************************************************/
NAS_LMM_ACTION_FUN   NAS_LMM_FsmFindAct(  const NAS_LMM_FSM_DESC_STRU   *pFsmDesc,
                                        VOS_UINT32              ulState,
                                        VOS_UINT32              ulEventType)
{
    VOS_UINT32                          ulActTblLoc;
    VOS_UINT32                          ulMmStaTblLoc;
    NAS_LMM_ACT_STRU                    *pMmActStru;

    (VOS_VOID)(ulState);
    /* 根据某并行状态机的当前状态(主状态和子状态的组合状态),在该状态机的状态
    转移表中查找状态事件处理表, 输出为状态事件处理表在状态转移表中的索引 */
    NAS_LMM_FsmGetEvtTab(pFsmDesc, &ulMmStaTblLoc);
    if ( NAS_LMM_SEA_TABLE_ERR_LOC == ulMmStaTblLoc)
    {   /* 若没有找到事件处理表, 调用异常处理函数 */

        /* 目前的异常处理就是打印 */
        NAS_LMM_PUBM_LOG_ERR(            "NAS_LMM_FsmFindAct: In ulMmStaTblLoc not fine correct StaTable.");
        return  VOS_NULL_PTR;
    }
    else
    {
        /*
        NAS_LMM_PUBM_LOG2_INFO(          "NAS_LMM_FsmFindAct: aulMmParaFsmId = , ulMmStaTblLoc =  ",
                                        pFsmDesc->aulMmParaFsmId,
                                        ulMmStaTblLoc);
        */
    }



    /* 找到某并行状态机的状态事件处理表 */
    pMmActStru = pFsmDesc->pStaTable[ulMmStaTblLoc].pActTable;


    /* 根据收到的事件类型, 在状态事件处理表中查找事件类型的处理函数  */
    NAS_LMM_FsmGetEvtStatFun(            pFsmDesc,
                                        ulMmStaTblLoc,
                                        ulEventType,
                                        &ulActTblLoc);
    if ( NAS_LMM_SEA_TABLE_ERR_LOC       == ulActTblLoc)
    {   /* 若没有找到事件类型, 调用异常处理函数 */

        /* 目前的异常处理就是打印 */
        return  VOS_NULL_PTR;
    }
    else
    {
        /*
        NAS_LMM_PUBM_LOG2_INFO(          "NAS_LMM_FsmFindAct: aulMmParaFsmId = , ulActTblLoc =  ",
                                        pFsmDesc->aulMmParaFsmId,
                                        ulActTblLoc);
       */
    }

    /* 使用找到的函数处理收到的事件 */
    return  pMmActStru[ulActTblLoc].pfActionFun;
}


/*****************************************************************************
 Function Name  : NAS_LMM_FsmGetEvtTab
 Discription    : 根据当前状态，在状态转移表g_astEmmStaTbl中查找事件处理表，
                  并输出该事件处理表在g_astEmmStaTbl中的数组索引值
 Input          : None
 Output         : pActTable
 Return         : None
 History:
      1.  韩鲁峰 41410  yyyy-mm-dd  Draft Enact
*****************************************************************************/
VOS_VOID    NAS_LMM_FsmGetEvtTab(       const NAS_LMM_FSM_DESC_STRU   *pFsmDesc,
                                        VOS_UINT32             *pulStaTblLoc)
{

    NAS_LMM_STA_STRU*                    pStaTable;                /* 状态转移表的基地址     */
    VOS_UINT32                          ulMmStaTableSize;         /* 状态转移表的大小       */
    VOS_UINT32                          ulCurState;
    NAS_LMM_FSM_STATE_STRU              *pstFsmCtx  = NAS_LMM_NULL_PTR;


    VOS_UINT32                          i;
    VOS_UINT32                          ulMaxSeaCycle;


    pStaTable                           = pFsmDesc->pStaTable;
    ulMmStaTableSize                    = pFsmDesc->ulSize;



    /* 获取状态机地址,用于取状态 */
    pstFsmCtx = NAS_LMM_GetCurFsmAddr(pFsmDesc->aulMmParaFsmId);

    /* 生成某并行状态机的当前状态 */
    ulCurState = NAS_LMM_PUB_COMP_EMMSTATE(pstFsmCtx->enMainState, pstFsmCtx->enSubState);


    /* 获取在某并行状态机的状态转移表中的最大查找循环次数 */
    ulMaxSeaCycle = ulMmStaTableSize;

    /* 在在某并行状态机的状态转移表中查找对应的状态处理表 */
    for(i = 0; i < ulMaxSeaCycle; i++)
    {
        /* 若状态相同,则找到了,给输出参数赋当前在表中的索引,退出循环 */
        if((pStaTable + i)->ulState     == ulCurState)
        {
            *pulStaTblLoc               = i;
            break;
        }
    }

    /* 若没有找到匹配的状态, 给输出参数赋错误值 */
    if(i >= ulMaxSeaCycle)
    {
        *pulStaTblLoc                   = NAS_LMM_SEA_TABLE_ERR_LOC;
        NAS_LMM_PUBM_LOG2_ERR(           "NAS_LMM_GetEvtTab: Not find correct STATE. aulMmParaFsmId = , ulCurState =",
                                        pFsmDesc->aulMmParaFsmId,
                                        ulCurState);
    }

    return;
}


/*****************************************************************************
 Function Name  : NAS_LMM_FsmGetEvtStatFun
 Discription    : 在某状态的事件处理表事件处理表中,查找处理输入消息的函数,并输出
 Input          : ulEmmStaTblLoc,
                  pRcvMsg
 Output         : pulEmmActTblLoc
 Return         : None
 History:
      1.  韩鲁峰 41410  yyyy-mm-dd  Draft Enact
*****************************************************************************/
VOS_VOID    NAS_LMM_FsmGetEvtStatFun(    const NAS_LMM_FSM_DESC_STRU   *pFsmDesc,
                                        VOS_UINT32              ulFsmStaTblLoc,
                                        VOS_UINT32              ulEventType,
                                        VOS_UINT32             *pulEmmActTblLoc)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulMaxSeaCycle;
    NAS_LMM_ACT_STRU                    *pMmActStru;


    /* 从状态转移表中找到某状态的事件处理表 */
    pMmActStru = pFsmDesc->pStaTable[ulFsmStaTblLoc].pActTable;

    /* 在g_astEmmStaTbl[ulEmmStaTblLoc].pActTable中的最大查找循环次数 *//*HLF*/
    ulMaxSeaCycle = pFsmDesc->pStaTable[ulFsmStaTblLoc].ulSize;

    /* 在g_astEmmStaTbl[ulEmmStaTblLoc].pActTable中查找对应事件类型的索引 */
    for(i = 0; i < ulMaxSeaCycle; i++)
    {
        /* 若事件类型相同,则找到了,给输出参数赋当前在表中的索引,退出循环 */
        if(ulEventType == (pMmActStru + i)->ulEventType)
        {
            *pulEmmActTblLoc            = i;
            break;
        }
    }

    /* 若没有找到匹配的状态, 给输出参数赋错误值 */
    if(i >= ulMaxSeaCycle)
    {
        *pulEmmActTblLoc                = NAS_LMM_SEA_TABLE_ERR_LOC;
      /*  NAS_LMM_PUBM_LOG2_ERR("NAS_LMM_FsmGetEvtStatFun: Not find correct FUNC. aulMmParaFsmId = , EVENT TYPE= ",
                                        pFsmDesc->aulMmParaFsmId,
                                        ulEventType);
      */
    }

    return;
}

/* lihong00150010 emergency tau&service begin */

VOS_VOID  NAS_LMM_EnterDeregStateModify
(
    NAS_LMM_FSM_STATE_STRU              *pstDestState
)
{
    NAS_MM_NETWORK_ID_STRU             *pstPresentNwInfo = NAS_EMM_NULL_PTR;

    /* 如果无卡或者卡无效，则进入DEREG+NO_IMSI态 */
    if ((NAS_LMM_SIM_STATUS_UNAVAILABLE == NAS_LMM_GetSimState())
        || (NAS_LMM_SIM_INVALID == NAS_LMM_GetPsSimValidity()))
    {
        pstDestState->enSubState = EMM_SS_DEREG_NO_IMSI;
        return ;
    }

    /* 如果目标状态为DEREG+NORMAL_SERVICE态，且当前小区被禁或者是ANYCELL小区，
       则进入DEREREG+LIMIT_SERVICE */
    pstPresentNwInfo = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_PRESENT_NET_ID);
    if ((EMM_SS_DEREG_NORMAL_SERVICE == pstDestState->enSubState)
        && ((EMMC_EMM_NO_FORBIDDEN != pstPresentNwInfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstPresentNwInfo->ulCellStatus)))
    {
        pstDestState->enSubState = EMM_SS_DEREG_LIMITED_SERVICE;
        return ;
    }

    return ;
}
/* lihong00150010 emergency tau&service end */

VOS_VOID  NAS_LMM_StaTransProc( NAS_LMM_FSM_STATE_STRU  stDestState )
{

    NAS_LMM_FSM_STATE_STRU *             pstCurState;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_StaTransProc enter!");

    /* 入参检查 */
    if ( NAS_LMM_PARALLEL_FSM_BUTT <= stDestState.enFsmId )
    {
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_StaTransProc, The input is illegal! enParaFsmId",
                             stDestState.enFsmId);
        return;
    }

    /* 获取某并行状态机当前状态的地址 */
    pstCurState                         = NAS_LMM_GetCurFsmAddr(stDestState.enFsmId);

    /* 如果当前状态与待转变为的状态相同,则不进行修改 */
    if( (pstCurState->enMainState       == stDestState.enMainState) &&
        (pstCurState->enSubState        == stDestState.enSubState)  &&
        (pstCurState->enStaTId          == stDestState.enStaTId))
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_StaTransProc: EMM STATE REMAINS. ");
        return;
    }

    /*执行状态迁移引起的处理*/
    NAS_LMM_ActionBeforeTransToDestSta(stDestState);
	/* lihong00150010 emergency tau&service begin */
    if (EMM_MS_DEREG == stDestState.enMainState)
    {
        NAS_LMM_EnterDeregStateModify(&stDestState);
    }
	/* lihong00150010 emergency tau&service end */
    /*状态机迁移*/
    NAS_LMM_FsmTranState(stDestState);

    return;
}


/*****************************************************************************
 Function Name  :NAS_LMM_FsmTranState
 Discription    :
 Input          :stDestState:目的状态，UE 1.0填写其中4个成员:
                                            enFsmId
                                            enMainState
                                            enSubState;
                                            enStaTId;
 Output         : None
 Return         : None
 History:
      1.  Name+ID  yyyy-mm-dd  Draft Enact
*****************************************************************************/
VOS_VOID  NAS_LMM_FsmTranState(          NAS_LMM_FSM_STATE_STRU  stDestState)
{
    NAS_LMM_FSM_STATE_STRU *             pstCurState;

    /* 获取某并行状态机当前状态的地址 */
    pstCurState                         = NAS_LMM_GetCurFsmAddr(stDestState.enFsmId);

    /* 打印当前状态和定时器 */
    NAS_LMM_PUBM_LOG_NORM("======================================================================================");
    if(NAS_LMM_PARALLEL_FSM_EMM          == stDestState.enFsmId)
    {
        NAS_LMM_PUBM_LOG_NORM("======NAS_EMM Change Statate To:======");
    }
    #if 0
    else if(NAS_LMM_PARALLEL_FSM_MMC     == stDestState.enFsmId)
    {
        NAS_LMM_PUBM_LOG_NORM("======NAS_LMMC Change Statate To:======");
    }
    #endif
    else
    {
        NAS_LMM_PUBM_LOG_NORM("======NAS_SECU Change Statate To:======");
    }

    /* MM层某并行状态机的状态转换 */
    pstCurState->enMainState            = stDestState.enMainState;
    pstCurState->enSubState             = stDestState.enSubState;
    pstCurState->enStaTId               = stDestState.enStaTId;

    /* 打印转换后的状态和定时器 */
    NAS_LMM_PrintFsmState(pstCurState->enFsmId);

    NAS_LMM_PUBM_LOG_NORM("======================================================================================");
    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_ProcRrcOriResumeMsgMmcSuspendReq
 Description     : RESUME+RRCORI_WAIT_SYS_INFO状态,如果要转到Dereg状态时，对于
                                 是否应该更新安全上下文进行判断
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.FTY　　      2012-04-26  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_ProcRrcOriResumeDestStateDeregWriteEpsSecuContext(VOS_VOID)
{
    NAS_EMM_PUBU_LOG1_INFO(" NAS_EMM_ProcRrcOriResumeDestStateDeregWriteEpsSecuContext:NAS_EMM_GetResumeType()",
                            NAS_EMM_GetResumeType());

    /*RESUME+RRCORI_WAIT_SYS_INFO和RESUME+RRCRSM_WAIT_OTHER_RESUME状态下收到MMC挂起消息的处理*/
    switch (NAS_EMM_GetResumeType())
    {
        case NAS_LMM_SYS_CHNG_TYPE_REVERSE_RSL:
        case NAS_LMM_SYS_CHNG_TYPE_REVERSE_HO:
        case NAS_LMM_SYS_CHNG_TYPE_REVERSE_CCO:
        case NAS_LMM_SYS_CHNG_TYPE_REVERSE_REDIR:
            if(EMM_MS_DEREG != NAS_EMM_GetMsBefSuspend())
            {
                NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
            }
            break;

        default:
            if(EMM_MS_DEREG != NAS_EMM_GetMsBefResume())
            {
                NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
            }
            break;
    }
}
VOS_VOID  NAS_LMM_ClearEmmPara( VOS_VOID )
{
    /*设置 EPS update status为EU3*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

    /*删除GUTI*/
    NAS_EMM_ClearGuti();

    /*删除LVR TAI*/
    NAS_EMM_ClearLVRTai();

    /*删除KSIasme*/
    NAS_EMM_ClearAllSecuCntxt();

    /* 删除TIN值 */
    NAS_EMM_SetTinType(MMC_LMM_TIN_INVALID);
}
VOS_VOID  NAS_LMM_ActionBeforeTransToDestSta( NAS_LMM_FSM_STATE_STRU  stDestState )
{

    NAS_LMM_FSM_STATE_STRU *             pstCurState;


    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ActionBeforeTransToDestSta enter!");

    /*判断主动上报给APP的状态是否改变，如果改变就主动上报APP_MM_REG_STAT_IND消息*/
    NAS_LMM_AppStateChange(stDestState);

    /* L模激活的情况下上报 */

    /*对于服务状态的上报:即使在挂起，也要及时维护本地映射的服务状态,注册状态
      在上报状态前再去判断当前是否为主模*/
    /*if ( (EMM_MS_NULL == stDestState.enMainState)
       ||( (NAS_LMM_CUR_LTE_ACTIVE == NAS_EMM_GetCurLteState())
         &&(EMM_MS_NULL != stDestState.enMainState)))
    {*/
    /*判断主动上报给MMC的服务状态是否改变，如果改变就主动上报ID_LMM_MMC_SERVICE_STATUS_IND消息*/
    NAS_LMM_MmcServiceStateChange(stDestState);

    /*判断主动上报给MMC的注册状态是否改变，如果改变就主动上报ID_LMM_MMC_REGISTER_STATUS_IND消息*/
    NAS_LMM_MmcRegisterStateChange(stDestState);
    /*}*/

    /*如果目标状态是Dereg:
      删除 new mapped security context
      删除 new native security context
      删除 current mapped security context
      如果存在non-current native secu cntxt将其还原为current native secu cntxt
      清空EMM本地维护的承载上下文*/
    if(EMM_MS_DEREG == stDestState.enMainState)
    {
        /* 进入DEREG态，清除UE无线能力变化标识 */
        NAS_EMM_ClearUeRadioAccCapChgFlag();

        /*leili modify for isr begin*/
        /*停止定时器*/
        NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3412);

        /*通知GU模T3412停止*/
        NAS_EMM_SendTimerStateNotify(TI_NAS_EMM_PTL_T3412, LMM_MMC_TIMER_STOP);

        NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3423);

        /*通知GU模T3423停止*/
        NAS_EMM_SendTimerStateNotify(TI_NAS_EMM_PTL_T3423, LMM_MMC_TIMER_STOP);

        /*leili modify for isr end*/
        /*安全上下文*/
        NAS_EMM_SecuDeregClrSecuCntxt();


        /*leili modify for isr begin*/
        /*清空EMM维护的承载上下文*/
        NAS_EMM_PubmInfoBearerContextInit();
#if 0
        NAS_LMM_MEM_SET(                NAS_EMM_GetEpsBearerContextAddr(),
                                        0,
                                        sizeof(NAS_LMM_EPS_BEARER_CONTEXT_STRU));
#endif

        if (NAS_LMM_REG_STATUS_EMC_REGED == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_LMM_ClearEmmPara();
        }

        NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_DEREG);


        /*leili modify for isr end*/
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_NULL);

        /* 清除GU发起过LAU或者联合RAU标识，因为进入DEREG态后接着是ATTACH流程，而不是TAU流程 */
        NAS_LMM_SetEmmInfoLauOrComRauFlag(NAS_EMM_LAU_OR_COMBINED_RAU_NOT_HAPPENED);
        NAS_LMM_SetEmmInfoSrvccFlag(NAS_EMM_SRVCC_NOT_HAPPENED);
        /* 清除第一次TAU标识，因为进入DEREG态后接着是ATTACH流程，而不是TAU流程 */
        NAS_LMM_SetEmmInfoFirstTauFlag(NAS_EMM_NOT_FIRST_TAU);

        /* 清除联合DETACH被TAU打断标识 */
        NAS_EMM_TAU_SetEmmCombinedDetachBlockFlag(NAS_EMM_COM_DET_BLO_NO);
        /* 清除UPDATE_MM标识 */
        /*NAS_LMM_SetEmmInfoUpdateMmFlag(NAS_EMM_UPDATE_MM_FLAG_INVALID);*/

        /*清空T3412超时标志，DrxNetCapability标志*/
        NAS_LMM_SetEmmInfoT3412ExpCtrl(    NAS_EMM_T3412_EXP_NO);
        NAS_LMM_SetEmmInfoDrxNetCapChange(  NAS_EMM_NO);
        /*清空RRC删除原因标识*/
        NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_NO);
        /*清空 EPS 承载上下文变更标志*/
        NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_NOCHANGE);

        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_NO);
        NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3442);

        NAS_EMM_SER_SaveEmmSerCsfbAbortFlag(NAS_EMM_CSFB_ABORT_FLAG_INVALID);

        NAS_LMM_SetEmmInfoPsState(GMM_LMM_GPRS_NOT_SUSPENSION);

        NAS_EMM_ClrAllUlDataReqBufferMsgExceptMtDetachAcc();

        return ;
    }

    /*当目标状态是EMM_MS_REG，并且当前状态是EMM_MS_DEREG，
    删除卡或者NV中保存的安全上下文*/
    /* 33401 CR457中描述UE从脱离去注册态转去注册态时，需要将SIM卡或者NV中的安全
    上下文设置为无效*/
    pstCurState = NAS_LMM_GetEmmCurFsmAddr() ;
    if((EMM_MS_REG == stDestState.enMainState)
        &&(EMM_MS_DEREG == pstCurState->enMainState))
    {
        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_DELETE);
    }

    #if 0
   /* 当需要转到REG+NORMAL_SERVICE态时，如果UPDATE_MM标识有效，则需要发送内部
       TAU消息，类型为NAS_LMM_INTRA_TAU_TYPE_UPDATE_MM，在预处理处理这种类型的
       内部消息，将状态转到REG+ATTEMP_TO_UPDATE_MM状态 */
    if ((EMM_MS_REG == stDestState.enMainState)
         && (EMM_SS_REG_NORMAL_SERVICE == stDestState.enSubState)
         && (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI()))
    {
        NAS_EMM_SendIntraTauReq(    ID_APP_MM_SET_DRX_REQ,
                                    NAS_LMM_INTRA_TAU_TYPE_UPDATE_MM);
    }
    #endif
    return;
}



VOS_UINT32  NAS_LMM_MsgProcessInFsm(     NAS_LMM_PARALLEL_FSM_ENUM_UINT16  enParalFsmId,
                                        MsgBlock                        *pMsg,
                                        VOS_UINT32                       ulMsgId,
                                        VOS_UINT32                       ulEventType )
{
    VOS_UINT32                          ulProcessFlag; /* 标记该消息是否处理 */

    /* 状态机地址越界 */
    if ( enParalFsmId >= NAS_LMM_PARALLEL_FSM_BUTT )
    {
        /* 打印出错信息 */
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_MsgProcessInFsm, FsmId Error, enParalFsm",
                                        enParalFsmId);

        return  NAS_LMM_MSG_DISCARD;
    }

    /* 状态机中消息的处理 */
    ulProcessFlag = NAS_LMM_FsmProcessEvent(     enParalFsmId,
                                                ulEventType,
                                                ulMsgId,
                                                (VOS_UINT8 *)pMsg );
    return ulProcessFlag;
}



NAS_LMM_COMM_ACTION_FUN  NAS_LMM_CommFindFun(
                                        const NAS_LMM_COMM_ACT_STRU   *pastMap,
                                        VOS_UINT32              ulMapNum,
                                        VOS_UINT32              ulMsgId,
                                        VOS_UINT32              ulSenderPid)
{
    VOS_UINT32                          i;
    NAS_LMM_COMM_ACTION_FUN              pActFun = NAS_LMM_NULL_PTR;

    #if 0
    /* 在Map中查找对应的 TYPE*/
    for(i = 0; i < ulMapNum; i++)
    {
        /* 若类型相同,则找到了,返回当前的动作函数，退出循环 */
        if(ulMsgId == pastMap[i].ulMsgId)
        {
            pActFun            = pastMap[i].pfFun;
            break;
        }
    }
    #endif
    /* 在Map中查找对应的 TYPE*/
    for(i = 0; i < ulMapNum; i++)
    {
        /* 若类型相同,则找到了,返回当前的动作函数，退出循环 */
        if((ulMsgId == pastMap[i].ulMsgId)
            && (ulSenderPid == pastMap[i].ulSenderPid))
        {
            pActFun            = pastMap[i].pfFun;
            break;
        }
    }

#if 0
    if (NAS_LMM_NULL_PTR == pActFun)
    {
        /* 如果找不到处理函数，表示当前没有该处理函数 */
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_CommFindFun: Not find action functin");
    }
#endif

    return  pActFun;
}

/*****************************************************************************
 Function Name   : NAS_LMM_CommFindFun
 Description     : 在pMap指向的结构中查找ulMsgId对应的操作函数;
                   pMap指向的结构中共有ulMapNum项;
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.wangchen    2012-05-31
*****************************************************************************/
NAS_EMMC_COMM_ACTION_FUN  NAS_EMMC_CommFindFun(
                                        const NAS_EMMC_COMM_ACT_STRU   *pastMap,
                                        VOS_UINT32              ulMapNum,
                                        VOS_UINT32              ulMsgId,
                                        VOS_UINT32              ulSenderPid)
{
    VOS_UINT32                          i;
    NAS_EMMC_COMM_ACTION_FUN              pActFun = NAS_LMM_NULL_PTR;

    /* 在Map中查找对应的 TYPE*/
    for(i = 0; i < ulMapNum; i++)
    {
        /* 若类型相同,则找到了,返回当前的动作函数，退出循环 */
        if((ulMsgId == pastMap[i].ulMsgId)
            && (ulSenderPid == pastMap[i].ulSenderPid))
        {
            pActFun            = pastMap[i].pfFun;
            break;
        }
    }

#if 0
    if (NAS_LMM_NULL_PTR == pActFun)
    {
        /* 如果找不到处理函数，表示当前没有该处理函数 */
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_CommFindFun: Not find action functin");
    }
#endif

    return  pActFun;
}
VOS_UINT32  NAS_LMM_GetServiceDomain(NAS_LMM_MAIN_STATE_ENUM_UINT16  enMainState)
{
    VOS_UINT32                           ulSrvDomain = APP_MM_SERVICE_DOMAIN_NULL;

    switch( enMainState )
    {
        case      EMM_MS_REG:
        case      EMM_MS_TAU_INIT:
        case      EMM_MS_SER_INIT:
                /* 在注册态下，返回PS */
                ulSrvDomain = APP_MM_SERVICE_DOMAIN_PS;
                break;

        default:
                /*非注册态下返回NULL*/
                ulSrvDomain = APP_MM_SERVICE_DOMAIN_NULL;
                break;
    }

    return ulSrvDomain;
}




VOS_VOID  NAS_LMM_AppStateChange( NAS_EMM_FSM_STATE_STRU  stEmmDestState)
{
    NAS_APP_REG_STAT_ENUM_UINT32       ulNasAppDstState;

    /* 入参检查 */
    if ( NAS_LMM_PARALLEL_FSM_EMM != stEmmDestState.enFsmId )
    {
       /* NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_AppStateChange, The input is illegal! enParaFsmId",
                             stEmmDestState.enFsmId);*/
        return;
    }

    ulNasAppDstState = NAS_LMM_GetNasAppRegState();

    /* 根据EMM的目的状态，生成目的APP注册状态 */
    NAS_LMM_StateMap(&stEmmDestState, &ulNasAppDstState);

    NAS_LMM_PUBM_LOG1_NORM("NAS_LMM_AppStateChange NAS_LMM_GetNasAppRegState()=",NAS_LMM_GetNasAppRegState());

    NAS_LMM_PUBM_LOG1_NORM("NAS_LMM_AppStateChange ulNasAppDstState=",ulNasAppDstState);



    /* 若APP注册状态发生变化，则修改该状态，并上报给APP */
    if(NAS_LMM_GetNasAppRegState() != ulNasAppDstState)
    {
        /*向AT发送APP_MM_STAT_IND消息*/
        NAS_EMM_AppRegStatInd(ulNasAppDstState,stEmmDestState.enMainState);

        NAS_LMM_SetNasAppRegState(ulNasAppDstState);

    }
    return;
}

/*****************************************************************************
 Function Name   : NAS_LMM_IsRegisteredInHplmn
 Description     : 判断是否注册在HPLMN上
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2012-07-10  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_LMM_IsRegisteredInHplmn()
{
    NAS_MM_PLMN_LIST_STRU               stEHplmnList    = {0};
    VOS_UINT32                          ulRslt          = MMC_LMM_FAIL;
    MMC_LMM_PUB_INFO_STRU               stPubInfo;
    NAS_EMM_PLMN_ID_STRU               *pstCurentPlmn   = VOS_NULL_PTR;

    pstCurentPlmn = NAS_LMM_GetEmmInfoPresentPlmnAddr();

    NAS_LMM_MEM_SET(&stPubInfo,0,sizeof(MMC_LMM_PUB_INFO_STRU));
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /* 如果EHPLMN获取成功，且个数不为0，则判断当前PLMN是否在EHPLMN列表中，如果
       存在，则是注册在HPLMN上，否则就是注册到RPLMN上 */
    ulRslt = NAS_EMM_GetGulPubInfo(MMC_LMM_EHPLMN, &stPubInfo);
    /*PC REPLAY MODIFY BY LEILI END*/
    if ((MMC_LMM_SUCC == ulRslt) && (stPubInfo.u.stEHplmnList.ulPlmnNum > 0))
    {
        NAS_LMM_PUBM_LOG1_NORM("NAS_LMM_StateMap HPLMN",stEHplmnList.ulPlmnNum);

        NAS_LMM_MEM_CPY (   &stEHplmnList,
                            &stPubInfo.u.stEHplmnList,
                            sizeof(MMC_LMM_PLMN_LIST_STRU));

        if (NAS_LMM_MATCH_SUCCESS == NAS_LMM_PlmnIDMatchHplmnList(pstCurentPlmn, &stEHplmnList))
        {
            return NAS_EMM_YES;
        }

        return NAS_EMM_NO;
    }

    /* 如果EHPLMN无效，则判断当前PLMN与HPLMN是否相同，如果相同，则是注册在HPLMN
       上，否则就是注册到RPLMN上 */
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    ulRslt = NAS_EMM_GetGulPubInfo(MMC_LMM_HPLMN, &stPubInfo);
    if (MMC_LMM_SUCC != ulRslt)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_IsRegisteredInHplmn:Hplmn cannot be got!");
        return NAS_EMM_NO;
    }
    /*PC REPLAY MODIFY BY LEILI END*/

    if (NAS_LMM_MATCH_SUCCESS == NAS_LMM_HplmnMatch((NAS_MM_PLMN_ID_STRU *)(&stPubInfo.u.stHplmn), pstCurentPlmn))
    {
     	return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}


VOS_VOID  NAS_LMM_StateMap(const NAS_EMM_FSM_STATE_STRU *pstEmmDestState,
                        NAS_APP_REG_STAT_ENUM_UINT32   *pulNasAppDstState)

{
    switch(pstEmmDestState->enMainState)
    {
        case    EMM_MS_NULL:
        case    EMM_MS_DEREG_INIT:
                *pulNasAppDstState = NAS_APP_RS_NOT_REG_AND_NOT_SRCH_OPERATOR;
                break;

        case    EMM_MS_DEREG:
                if ((EMM_SS_DEREG_NO_IMSI == pstEmmDestState->enSubState))
                {
                    *pulNasAppDstState = NAS_APP_RS_NOT_REG_AND_NOT_SRCH_OPERATOR;
                }
                else
                {
                    *pulNasAppDstState = NAS_APP_RS_NOT_REG_AND_SRCH_OPERATOR;
                }
                break;

        case    EMM_MS_REG_INIT:
                *pulNasAppDstState = NAS_APP_RS_NOT_REG_AND_SRCH_OPERATOR;
                break;

        case    EMM_MS_REG:
                if (NAS_EMM_YES == NAS_LMM_IsRegisteredInHplmn())
                {
                    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_StateMap HPLMN");
                    *pulNasAppDstState = NAS_APP_RS_REG_HPLMN;
                }
                else
                {
                    *pulNasAppDstState = NAS_APP_RS_REG_ROAMING;
                }
                break;

        default:
                break;

    }

    return;
}




VOS_VOID  NAS_LMM_RegStatOsa( const NAS_APP_REG_STAT_ENUM_UINT32   *pulNasAppDstState,
                               APP_REG_STAT_ENUM_UINT32       *pulAppRegStat)
{
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_RegStatOsa is entered");

    switch(*pulNasAppDstState)
    {
    case NAS_APP_RS_NOT_REG_AND_NOT_SRCH_OPERATOR:
        *pulAppRegStat                    = APP_REG_STAT_NOT_REG_AND_NOT_SRCH_OPERATOR;
        break;


    case NAS_APP_RS_NOT_REG_AND_SRCH_OPERATOR:
        *pulAppRegStat                    = APP_REG_STAT_NOT_REG_AND_SRCH_OPERATOR;
        break;

    case NAS_APP_RS_REG_HPLMN:
        *pulAppRegStat                    = APP_REG_STAT_REG_HPLMN;
        break;

    case NAS_APP_RS_REG_ROAMING:
        *pulAppRegStat                    = APP_REG_STAT_REG_ROAMING;
        break;

    case NAS_APP_RS_REG_DENIED:
        *pulAppRegStat                    = APP_REG_STAT_REG_DENIED;
        break;

    case NAS_APP_RS_UNKNOWN:
        *pulAppRegStat                    = APP_REG_STAT_UNKNOWN;
        break;
    default:

        break;

    }
    return;
}



VOS_VOID  NAS_EMM_AppRegStatInd( NAS_APP_REG_STAT_ENUM_UINT32   ulNasAppState,
                                           NAS_LMM_MAIN_STATE_ENUM_UINT16  enMainState)
{

    APP_MM_REG_STAT_IND_STRU            *pEmmAppRegStatMsg;
    NAS_MM_NETWORK_ID_STRU              *pstNetIdInfo;
    APP_REG_STAT_ENUM_UINT32            ulAppRegStat = APP_REG_STAT_NOT_REG_AND_NOT_SRCH_OPERATOR;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_AppRegStatInd is entered");

    pstNetIdInfo                        = NAS_LMM_GetEmmInfoNetInfoPresentNetAddr();

    /*申请消息内存*/
    pEmmAppRegStatMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_REG_STAT_IND_STRU));

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pEmmAppRegStatMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppRegStatInd: MSG ALLOC ERR!");
        return;

    }

    NAS_LMM_MEM_SET(pEmmAppRegStatMsg,0,sizeof(APP_MM_REG_STAT_IND_STRU));

    /*构造APP_MM_REG_STAT_IND消息*/
     /* 打包消息头 */
    NAS_LMM_COMP_APP_DT_MSG_HEADER( pEmmAppRegStatMsg,
                                 NAS_LMM_MSG_LENGTH_NO_HEADER(APP_MM_REG_STAT_IND_STRU));
    /*填充消息ID*/
    pEmmAppRegStatMsg->ulMsgId          = ID_APP_MM_REG_STAT_IND;

    /*填充消息内容*/
    pEmmAppRegStatMsg->ulOpId           = DIAG_CMD_MM_REG_STAT_IND;
    /*pEmmAppRegStatMsg->ulRslt           = APP_SUCCESS;*/
    pEmmAppRegStatMsg->enAccessMode     = NAS_LMM_GetEmmInfoRaMod();

    /*获得服务域信息*/
    pEmmAppRegStatMsg->enSrvDomain      = NAS_LMM_GetServiceDomain(enMainState);

    NAS_LMM_RegStatOsa(&ulNasAppState,   &ulAppRegStat);

    pEmmAppRegStatMsg->enRegStat        = ulAppRegStat;

    NAS_LMM_MEM_CPY(                     &pEmmAppRegStatMsg->stPlmnId,
                                        &pstNetIdInfo->stPlmnId,
                                        sizeof(NAS_MM_PLMN_ID_STRU));

    NAS_LMM_MEM_CPY(                     &pEmmAppRegStatMsg->stTac,
                                        &pstNetIdInfo->stTac,
                                        sizeof(NAS_MM_TAC_STRU));

    pEmmAppRegStatMsg->ulCellId         = pstNetIdInfo->ulCellId;

    /*向APP发送APP_MM_REG_STAT_IND消息*/
    NAS_LMM_SEND_MSG(pEmmAppRegStatMsg);

    return;
}


MMC_LMM_SERVICE_STATUS_ENUM_UINT32  NAS_LMM_DeregServiceStateMap(
                                        const NAS_EMM_FSM_STATE_STRU        *pstEmmDestState)
{
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32  ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_TYPE_BUTT;

    switch(pstEmmDestState->enSubState)
    {
        case EMM_SS_DEREG_NORMAL_SERVICE:
        case EMM_SS_DEREG_NO_CELL_AVAILABLE:
            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_NO_SERVICE;
            break;

        case EMM_SS_DEREG_ATTEMPTING_TO_ATTACH:
        case EMM_SS_DEREG_ATTACH_NEEDED:
        case EMM_SS_DEREG_NO_IMSI:
        case EMM_SS_DEREG_LIMITED_SERVICE:
            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_LIMIT_SERVICE;
            break;

        default:
            break;
    }
    /* 注意rej12的标记在离开DEREG LIMIT SERVICE状态时要清0 */
    if ((EMM_SS_DEREG_LIMITED_SERVICE == pstEmmDestState->enSubState)
       &&(NAS_LMM_REJ_CAUSE_IS_12 == NAS_LMM_GetNasRejCause12Flag()))
    {
        ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_LIMIT_SERVICE_REGION;
        NAS_LMM_SetNasRejCause12Flag(NAS_LMM_REJ_CAUSE_NOT_12);

    }
    return ulNasMmcDstServiceState;
}
MMC_LMM_SERVICE_STATUS_ENUM_UINT32  NAS_LMM_RegServiceStateMap(
                                        const NAS_EMM_FSM_STATE_STRU        *pstEmmDestState)
{
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32  ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_TYPE_BUTT;

    switch(pstEmmDestState->enSubState)
    {
        case EMM_SS_REG_NO_CELL_AVAILABLE:

            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_NO_SERVICE;
            break;

        case EMM_SS_REG_ATTEMPTING_TO_UPDATE:
            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_LIMIT_SERVICE;
            break;

        case EMM_SS_REG_LIMITED_SERVICE:
            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_LIMIT_SERVICE;
            break;

        case EMM_SS_REG_WAIT_ACCESS_GRANT_IND:
            if (NAS_EMM_BAR_PROCEDURE_TAU == NAS_EMM_GetBarProcedure())
            {
                ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_LIMIT_SERVICE;
            }
            break;

        case EMM_SS_REG_NORMAL_SERVICE:
        case EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM:
        case EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF:
            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_NORMAL_SERVICE;
            break;

        default:
            break;
    }

    return ulNasMmcDstServiceState;
}


VOS_VOID  NAS_EMM_SendMmcServiceStatInd( VOS_UINT32 ulCsServiceStateChangeFlag,
                                                      VOS_UINT32 ulPsServiceStateChangeFlag)

{
    LMM_MMC_SERVICE_STATUS_IND_STRU    *pEmmMmcSerStatMsg;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_SendMmcServiceStatInd is entered");

    if(NAS_LMM_MMC_SER_REG_STAT_OPEN_FLAG == NAS_Emm_Get_Mmc_Srv_Reg_Flag())
    {
        /*申请消息内存*/
        pEmmMmcSerStatMsg = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_SERVICE_STATUS_IND_STRU));

        /*判断申请结果，若失败打印错误并退出*/
        if (NAS_EMM_NULL_PTR == pEmmMmcSerStatMsg)
        {
            /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_EMM_SendMmcServiceStatInd: MSG ALLOC ERR!");
            return;

        }

        NAS_LMM_MEM_SET(pEmmMmcSerStatMsg,0,sizeof(LMM_MMC_SERVICE_STATUS_IND_STRU));

        /*构造LMM_MMC_SERVICE_STATUS_IND消息*/
        /* 打包消息头 */
        EMM_PUBU_COMP_MMC_MSG_HEADER( pEmmMmcSerStatMsg,
                                     NAS_LMM_MSG_LENGTH_NO_HEADER(LMM_MMC_SERVICE_STATUS_IND_STRU));

        /*填充消息ID*/
        pEmmMmcSerStatMsg->ulMsgId          = ID_LMM_MMC_SERVICE_STATUS_IND;

        /*填充消息内容*/
        pEmmMmcSerStatMsg->ulOpId           = NAS_EMM_OPID_MMC;

        if((NAS_EMM_YES == ulCsServiceStateChangeFlag) && (MMC_LMM_SERVICE_STATUS_TYPE_BUTT != NAS_LMM_GetNasMmcCSServiceState()))
        {
            pEmmMmcSerStatMsg->bitOpCsStatus = NAS_EMM_BIT_SLCT;
            pEmmMmcSerStatMsg->ulCsServiceStatus = NAS_LMM_GetNasMmcCSServiceState();
        }

        if(NAS_EMM_YES == ulPsServiceStateChangeFlag)
        {
            pEmmMmcSerStatMsg->bitOpPsStatus = NAS_EMM_BIT_SLCT;
            pEmmMmcSerStatMsg->ulServiceStatus  = NAS_LMM_GetNasMmcServiceState();
        }

        /*pEmmMmcSerStatMsg->ulServiceStatus  = ulNasMmcSerivceState;*/

        /*向MMC发送LMM_MMC_SERVICE_STATUS_IND消息*/
        NAS_LMM_SendLmmMmcMsg(pEmmMmcSerStatMsg);
    }
    return;
}

/*****************************************************************************
 Function Name   : NAS_LMM_RegCSServiceStateMap
 Description     : 把当前EMM 注册状态映射为EMM内部定义上报给MMC的CS域服务状态
 Input           : 当前EMM状态
 Output          : 内部定义上报给MMC的服务状态
 Return          : VOS_VOID

 History         :
    1.    FTY      2012-01-11  Draft Enact

*****************************************************************************/
MMC_LMM_SERVICE_STATUS_ENUM_UINT32  NAS_LMM_RegCSServiceStateMap(
                                        const NAS_EMM_FSM_STATE_STRU        *pstEmmDestState)
{
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32  ulNasMmcDstServiceState = NAS_LMM_GetNasMmcCSServiceState();

    /*注册域为PS,CS服务状态都为NO SERVICE*/
    if(NAS_LMM_REG_DOMAIN_PS == NAS_LMM_GetEmmInfoRegDomain())
    {
        NAS_LMM_PUBM_LOG1_NORM("NAS_LMM_RegCSServiceStateMap:NAS_LMM_GetEmmInfoRegDomain",NAS_LMM_GetEmmInfoRegDomain());
        ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_NO_SERVICE;

        return ulNasMmcDstServiceState;
    }

    switch(pstEmmDestState->enSubState)
    {
        case EMM_SS_REG_NO_CELL_AVAILABLE:
        /* EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM不可能为CS_PS注册域 ，冗余   */
        /*case EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM:*/
            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_NO_SERVICE;
            break;

        case EMM_SS_REG_ATTEMPTING_TO_UPDATE:
        case EMM_SS_REG_LIMITED_SERVICE:
        case EMM_SS_REG_WAIT_ACCESS_GRANT_IND:
            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_LIMIT_SERVICE;
            break;

        case EMM_SS_REG_NORMAL_SERVICE:
            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_NORMAL_SERVICE;
            break;

        default:
            break;

    }

    return ulNasMmcDstServiceState;
}


/*****************************************************************************
 Function Name   : NAS_LMM_UeModeCsPsCSServiceStateMap
 Description     : UE模式为CS/PS,把当前EMM状态映射为EMM内部定义上报给MMC的CS域服务状态
 Input           : 当前EMM状态
 Output          : 内部定义上报给MMC的服务状态
 Return          : VOS_VOID

 History         :
    1.   FTY       2012-01-11  Draft Enact

*****************************************************************************/
MMC_LMM_SERVICE_STATUS_ENUM_UINT32  NAS_LMM_CsPsModeCSServiceStateMap(
                                        const NAS_EMM_FSM_STATE_STRU        *pstEmmDestState)

{
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32 ulNasMmcDstServiceState = NAS_LMM_GetNasMmcCSServiceState();

    switch(pstEmmDestState->enMainState)
    {
        case EMM_MS_NULL:
        case EMM_MS_DEREG:
            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_NO_SERVICE;
            break;

        case EMM_MS_REG:
            ulNasMmcDstServiceState = NAS_LMM_RegCSServiceStateMap(pstEmmDestState);
            break;

        /*其余状态不做处理*/
        default:
                break;

    }

    return ulNasMmcDstServiceState;
}
VOS_VOID NAS_LMM_CovertMmlRegStatusToLmmRegStatus
(
    NAS_MML_REG_STATUS_ENUM_UINT8 enMmlRegStatus,
    MMC_LMM_REGISTER_STATUS_ENUM_UINT32 *penLmmRegState
)
{
    switch(enMmlRegStatus)
    {
        case  NAS_MML_REG_NOT_REGISTERED_NOT_SEARCH:
              *penLmmRegState = MMC_LMM_REGISTER_STATUS_NOT_REG_NOT_SRCHING;
              break;

        case  NAS_MML_REG_REGISTERED_HOME_NETWORK:
              *penLmmRegState = MMC_LMM_REGISTER_STATUS_REG_HPLMN;
              break;

        case  NAS_MML_REG_NOT_REGISTERED_SEARCHING:
              *penLmmRegState = MMC_LMM_REGISTER_STATUS_NOT_REG_SRCHING;
              break;

        case  NAS_MML_REG_REGISTERED_DENIED:
              *penLmmRegState = MMC_LMM_REGISTER_STATUS_REG_DENINED;
              break;

        case  NAS_MML_REG_UNKNOWN:
              *penLmmRegState = MMC_LMM_REGISTER_STATUS_UNKNOWN;
              break;

        case  NAS_MML_REG_REGISTERED_ROAM:
              *penLmmRegState = MMC_LMM_REGISTER_STATUS_REG_ROAMING;
              break;

        default:
              NAS_LMM_PUBM_LOG_ERR("NAS_LMM_CovertMmlRegStatusToLmmRegStatus: enMmlRegStatus ERR!");
              break;

    }
    return;
}





VOS_VOID  NAS_EMM_SendMmcRegisterStatInd(NAS_EMM_REG_CHANGE_TYPE_ENUM_UINT32 ulChangeType)
{
    NAS_MM_NETWORK_ID_STRU             *pstNetIdInfo;
    LMM_MMC_REGISTER_STATUS_IND_STRU   *pEmmMmcRegStatMsg;
    MMC_LMM_REGISTER_STATUS_ENUM_UINT32 ulNasMmcRegState;


    pstNetIdInfo                        = NAS_LMM_GetEmmInfoNetInfoPresentNetAddr();
    ulNasMmcRegState                    = NAS_LMM_GetNasMmcRegisterState();

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_SendMmcServiceStatInd is entered");

    if(NAS_LMM_MMC_SER_REG_STAT_OPEN_FLAG == NAS_Emm_Get_Mmc_Srv_Reg_Flag())
    {
        /*申请消息内存*/
        pEmmMmcRegStatMsg = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_REGISTER_STATUS_IND_STRU));

        /*判断申请结果，若失败打印错误并退出*/
        if (NAS_EMM_NULL_PTR == pEmmMmcRegStatMsg)
        {
            /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_EMM_SendMmcRegisterStatInd: MSG ALLOC ERR!");
            return;

        }

        NAS_LMM_MEM_SET(pEmmMmcRegStatMsg,0,sizeof(LMM_MMC_REGISTER_STATUS_IND_STRU));

        /*构造LMM_MMC_RESISTER_STATUS_IND消息*/
        /* 打包消息头 */
        EMM_PUBU_COMP_MMC_MSG_HEADER( pEmmMmcRegStatMsg,
                                     NAS_LMM_MSG_LENGTH_NO_HEADER(LMM_MMC_REGISTER_STATUS_IND_STRU));

        /*填充消息ID*/
        pEmmMmcRegStatMsg->ulMsgId          = ID_LMM_MMC_REGISTER_STATUS_IND;

        /*填充消息内容*/
        pEmmMmcRegStatMsg->ulOpId           = NAS_EMM_OPID_MMC;


        if (NAS_EMM_REG_CHANGE_TYPE_REG_STATE == ulChangeType)
        {
            pEmmMmcRegStatMsg->bitOpLocationInfo = NAS_EMM_BIT_NO_SLCT;

            pEmmMmcRegStatMsg->bitOpRegisterStatus = NAS_EMM_BIT_SLCT;

            pEmmMmcRegStatMsg->ulRegisterStatus = ulNasMmcRegState;
        }
        else
        {
            pEmmMmcRegStatMsg->bitOpLocationInfo = NAS_EMM_BIT_SLCT;

            pEmmMmcRegStatMsg->bitOpRegisterStatus = NAS_EMM_BIT_NO_SLCT;

            pEmmMmcRegStatMsg->ulCellId = pstNetIdInfo->ulCellId;

            NAS_LMM_MEM_CPY(                    &pEmmMmcRegStatMsg->stTac,
                                            &pstNetIdInfo->stTac,
                                            sizeof(NAS_MM_TAC_STRU));
        }

        /*向MMC发送LMM_MMC_REGISTER_STATUS_IND消息*/
        NAS_LMM_SendLmmMmcMsg(pEmmMmcRegStatMsg);

    }
    return;

}


MMC_LMM_SERVICE_STATUS_ENUM_UINT32  NAS_LMM_ServiceStateMap(
                                        const NAS_EMM_FSM_STATE_STRU        *pstEmmDestState)

{
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32 ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_TYPE_BUTT;

    switch(pstEmmDestState->enMainState)
    {
        case EMM_MS_NULL:
            ulNasMmcDstServiceState = MMC_LMM_SERVICE_STATUS_NO_SERVICE;
            break;

        case EMM_MS_DEREG:
            ulNasMmcDstServiceState = NAS_LMM_DeregServiceStateMap(pstEmmDestState);
            break;

        case EMM_MS_REG:
            ulNasMmcDstServiceState = NAS_LMM_RegServiceStateMap(pstEmmDestState);
            break;

        /*其余状态不做处理*/
        default:
                break;

    }

    return ulNasMmcDstServiceState;
}

/*****************************************************************************
 Function Name   : NAS_LMM_CSServiceStateMap
 Description     : 把当前EMM状态映射为EMM内部定义上报给MMC的CS域服务状态
 Input           : 当前EMM状态
 Output          : 内部定义上报给MMC的服务状态
 Return          : VOS_VOID

 History         :
    1.    FTY      2012-01-11  Draft Enact

*****************************************************************************/
MMC_LMM_SERVICE_STATUS_ENUM_UINT32  NAS_LMM_CSServiceStateMap(
                                        const NAS_EMM_FSM_STATE_STRU        *pstEmmDestState)

{
      MMC_LMM_SERVICE_STATUS_ENUM_UINT32 ulNasMmcDstCSServiceState = MMC_LMM_SERVICE_STATUS_TYPE_BUTT;
      if(NAS_EMM_YES == NAS_EMM_IsCsPsUeMode())
      {
           ulNasMmcDstCSServiceState = NAS_LMM_CsPsModeCSServiceStateMap(pstEmmDestState);
      }

      return ulNasMmcDstCSServiceState;
}

/*****************************************************************************
 Function Name   : NAS_LMM_IsMmcPSServiceStateChange
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.    FTY      2012-01-13 Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_LMM_IsMmcPSServiceStateChange( const NAS_EMM_FSM_STATE_STRU  *pstEmmDestState,
                                                              MMC_LMM_SERVICE_STATUS_ENUM_UINT32   *pulNasMmcDstServiceState)
{
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32       ulNasMmcDstServiceState;
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32       ulNasMmcOriServiceState;


    /* 根据EMM的目的状态，生成目的MMC注册状态 */
    ulNasMmcDstServiceState = NAS_LMM_ServiceStateMap(pstEmmDestState);
    ulNasMmcOriServiceState = NAS_LMM_GetNasMmcServiceState();

    NAS_LMM_PUBM_LOG1_NORM("NAS_LMM_IsMmcPSServiceStateChange NAS_LMM_GetNasMmcServiceState()=",ulNasMmcOriServiceState);
    NAS_LMM_PUBM_LOG1_NORM("NAS_LMM_IsMmcPSServiceStateChange ulNasMmcDstState=",ulNasMmcDstServiceState);

    if((ulNasMmcOriServiceState != ulNasMmcDstServiceState)
      &&(MMC_LMM_SERVICE_STATUS_TYPE_BUTT != ulNasMmcDstServiceState))
    {
        *pulNasMmcDstServiceState = ulNasMmcDstServiceState;
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;

}

/*****************************************************************************
 Function Name   : NAS_LMM_IsMmcCSServiceStateChange
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.    FTY      2012-01-13 Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_LMM_IsMmcCSServiceStateChange( const NAS_EMM_FSM_STATE_STRU  *pstEmmDestState,
                                               MMC_LMM_SERVICE_STATUS_ENUM_UINT32   *pulNasMmcDstServiceState)
{
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32       ulNasMmcCSDstServiceState;
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32       ulNasMmcCSOriServiceState;


    /* 根据EMM的目的状态，生成目的MMC注册状态 */
    ulNasMmcCSDstServiceState = NAS_LMM_CSServiceStateMap(pstEmmDestState);
    ulNasMmcCSOriServiceState = NAS_LMM_GetNasMmcCSServiceState();

    NAS_LMM_PUBM_LOG1_NORM("NAS_LMM_IsMmcCSServiceStateChange NAS_LMM_GetNasMmcCSServiceState()=",ulNasMmcCSOriServiceState);
    NAS_LMM_PUBM_LOG1_NORM("NAS_LMM_IsMmcCSServiceStateChange ulNasMmcDstState=",ulNasMmcCSDstServiceState);

    if(ulNasMmcCSOriServiceState != ulNasMmcCSDstServiceState)
    {
        *pulNasMmcDstServiceState = ulNasMmcCSDstServiceState;
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;

}
VOS_VOID  NAS_LMM_MmcServiceStateChange( NAS_EMM_FSM_STATE_STRU  stEmmDestState)
{
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32       ulNasMmcDstPSServiceState  = MMC_LMM_SERVICE_STATUS_TYPE_BUTT;
    MMC_LMM_SERVICE_STATUS_ENUM_UINT32       ulNasMmcDstCSServiceState  = MMC_LMM_SERVICE_STATUS_TYPE_BUTT;
    VOS_UINT32                               ulPsServiceStateChangeFlag = NAS_EMM_NO;
    VOS_UINT32                               ulCsServiceStateChangeFlag = NAS_EMM_NO;;

    /* 入参检查 */
    if ( NAS_LMM_PARALLEL_FSM_EMM != stEmmDestState.enFsmId )
    {
       /* NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_AppStateChange, The input is illegal! enParaFsmId",
                             stEmmDestState.enFsmId);*/
        return;
    }

    /*检查CS域状态是否改变*/
    ulCsServiceStateChangeFlag = NAS_LMM_IsMmcCSServiceStateChange(&stEmmDestState,&ulNasMmcDstCSServiceState);
    if(NAS_EMM_YES == ulCsServiceStateChangeFlag)
    {
        NAS_LMM_SetNasMmcCSServiceState(ulNasMmcDstCSServiceState);
    }

    /*检查PS域状态是否改变*/
    ulPsServiceStateChangeFlag = NAS_LMM_IsMmcPSServiceStateChange(&stEmmDestState,&ulNasMmcDstPSServiceState);
    if(NAS_EMM_YES == ulPsServiceStateChangeFlag)
    {
        NAS_LMM_SetNasMmcServiceState(ulNasMmcDstPSServiceState);
    }

    /*只有服务状态改变，并且CS改变时，状态有效时才发送*/
    if((NAS_EMM_NO != ulPsServiceStateChangeFlag)
      || ((NAS_EMM_NO != ulCsServiceStateChangeFlag)
         &&(MMC_LMM_SERVICE_STATUS_TYPE_BUTT != ulNasMmcDstCSServiceState)))
    {
        /*如果L模为从模,不需要发送*/
        if ( (EMM_MS_NULL == stEmmDestState.enMainState)
           ||( (NAS_LMM_CUR_LTE_ACTIVE == NAS_EMM_GetCurLteState())
             &&(EMM_MS_NULL != stEmmDestState.enMainState)))
        {
           NAS_EMM_SendMmcServiceStatInd(ulCsServiceStateChangeFlag, ulPsServiceStateChangeFlag);
        }
    }


    return;
}

MMC_LMM_REGISTER_STATUS_ENUM_UINT32  NAS_LMM_RegRegisterStateMap(
                                        const NAS_EMM_FSM_STATE_STRU        *pstEmmDestState)
{
    MMC_LMM_REGISTER_STATUS_ENUM_UINT32 ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_BUTT;

    switch(pstEmmDestState->enSubState)
    {
        case EMM_SS_REG_WAIT_ACCESS_GRANT_IND:
            ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_NOT_REG_NOT_SRCHING;
            break;

        case EMM_SS_REG_LIMITED_SERVICE:
        case EMM_SS_REG_PLMN_SEARCH:
        case EMM_SS_REG_NO_CELL_AVAILABLE:
            ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_NOT_REG_SRCHING;
            break;

        case EMM_SS_REG_NORMAL_SERVICE:
        case EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM:
        case EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF:
            if(NAS_EMM_YES == NAS_LMM_IsRegisteredInHplmn())
            {
                ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_REG_HPLMN;
            }
            else
            {
                ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_REG_ROAMING;
            }
            break;

        case EMM_SS_REG_ATTEMPTING_TO_UPDATE:
            if(EMM_US_UPDATED_EU1 == NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat)
            {
                if(NAS_EMM_YES == NAS_LMM_IsRegisteredInHplmn())
                {
                    ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_REG_HPLMN;
                }
                else
                {
                    ulNasMmcDstRegisterState= MMC_LMM_REGISTER_STATUS_REG_ROAMING;
                }
            }
            else
            {
                ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_NOT_REG_SRCHING;
            }
            break;
        default:
            break;
    }
    return ulNasMmcDstRegisterState;
}


MMC_LMM_REGISTER_STATUS_ENUM_UINT32  NAS_LMM_DeregRegisterStateMap(
                                        const NAS_EMM_FSM_STATE_STRU         *pstEmmDestState)
{
    MMC_LMM_REGISTER_STATUS_ENUM_UINT32 ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_BUTT;

    switch(pstEmmDestState->enSubState)
    {
        case EMM_SS_DEREG_NORMAL_SERVICE:
        case EMM_SS_DEREG_ATTACH_NEEDED:
            ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_NOT_REG_NOT_SRCHING;
            break;

        case EMM_SS_DEREG_LIMITED_SERVICE:
        case EMM_SS_DEREG_ATTEMPTING_TO_ATTACH:
        case EMM_SS_DEREG_NO_CELL_AVAILABLE:
        case EMM_SS_DEREG_PLMN_SEARCH:
            if (VOS_FALSE == NAS_MML_GetPsAttachAllowFlg())
            {
                ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_NOT_REG_NOT_SRCHING;
            }
            else
            {
                ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_NOT_REG_SRCHING;
            }
            break;

        case EMM_SS_DEREG_NO_IMSI:
            ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_REG_DENINED;
            break;

        default:
            break;
    }
    return ulNasMmcDstRegisterState;
}


MMC_LMM_REGISTER_STATUS_ENUM_UINT32  NAS_LMM_RegisterStateMap(
                                        const NAS_EMM_FSM_STATE_STRU         *pstEmmDestState)
{
    MMC_LMM_REGISTER_STATUS_ENUM_UINT32 ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_BUTT;

    switch(pstEmmDestState->enMainState)
    {

        case EMM_MS_NULL:
        case EMM_MS_DEREG_INIT:
            ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_NOT_REG_NOT_SRCHING;
            break;

        case EMM_MS_REG_INIT:
            ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_NOT_REG_SRCHING;
            break;

        case EMM_MS_SER_INIT:
            if(NAS_EMM_YES == NAS_LMM_IsRegisteredInHplmn())
            {
                ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_REG_HPLMN;
            }
            else
            {
                ulNasMmcDstRegisterState = MMC_LMM_REGISTER_STATUS_REG_ROAMING;
            }
            break;

        case EMM_MS_DEREG:
            ulNasMmcDstRegisterState = NAS_LMM_DeregRegisterStateMap(pstEmmDestState);
            break;

        case EMM_MS_REG:
            ulNasMmcDstRegisterState = NAS_LMM_RegRegisterStateMap(pstEmmDestState);
            break;

        /*其余状态不做处理*/
        default:
            break;
    }
    return ulNasMmcDstRegisterState;
}
VOS_VOID  NAS_LMM_MmcRegisterStateChange( NAS_EMM_FSM_STATE_STRU  stEmmDestState)
{
    MMC_LMM_REGISTER_STATUS_ENUM_UINT32       ulNasMmcDstRegState;

    /* 入参检查 */
    if ( NAS_LMM_PARALLEL_FSM_EMM != stEmmDestState.enFsmId )
    {
       /* NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_AppStateChange, The input is illegal! enParaFsmId",
                             stEmmDestState.enFsmId);*/
        return;
    }

    ulNasMmcDstRegState = NAS_LMM_GetNasMmcRegisterState();

    /* 根据EMM的目的状态，生成目的MMC注册状态 */
    ulNasMmcDstRegState = NAS_LMM_RegisterStateMap(&stEmmDestState);

    NAS_LMM_PUBM_LOG1_NORM("NAS_LMM_MmcRegisterStateChange NAS_LMM_GetNasMmcRegState()=",NAS_LMM_GetNasMmcRegisterState());

    NAS_LMM_PUBM_LOG1_NORM("NAS_LMM_MmcRegisterStateChange ulNasMmcDstState=",ulNasMmcDstRegState);

    /* 若上报给MMC注册状态发生变化，则修改该状态，并上报给MMC */
    if ((NAS_LMM_GetNasMmcRegisterState() != ulNasMmcDstRegState)
       &&(MMC_LMM_REGISTER_STATUS_BUTT != ulNasMmcDstRegState))
    {
        NAS_LMM_SetNasMmcRegisterState(ulNasMmcDstRegState);

        /*在发送之前判断，L模激活的情况下上报 */
        /*add by FTY in 2012-01-11 start:*/
        /*没有激活的情况，如果改变要本地保存注册状态，不上报*/

        if (NAS_LMM_CUR_LTE_ACTIVE == NAS_EMM_GetCurLteState())
        {
            /*向MMC发送ID_LMM_MMC_SERVICE_STATUS_IND消息*/
            NAS_EMM_SendMmcRegisterStatInd(NAS_EMM_REG_CHANGE_TYPE_REG_STATE);
        }
    }
}
/*****************************************************************************/
/*lint +e961*/
/*lint +e960*/
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of 子系统+模块+文件名.c */
