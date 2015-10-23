



/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/

#include    "NasEmmAttDetInclude.h"
#include    "NasLmmPubMPrint.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMDETACH_C
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
NAS_EMM_DETACH_CTRL_STRU                g_stEmmDetCtrl;


/*****************************************************************************
  3 Function
*****************************************************************************/


/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_EMM_DetachInit()
{
    NAS_EMM_DETACH_LOG_NORM("NAS_EMM_DetachInit                START INIT...");

    /* 初始化 detach全局变量 */
    NAS_LMM_MEM_SET(&g_stEmmDetCtrl, 0, sizeof(g_stEmmDetCtrl));

    NAS_EMM_GLO_AD_GetDetTypeMo()       = MMC_LMM_MO_DET_PS_ONLY;

    /*初始化 UE 发起 detach 的默认类型*/
    NAS_EMM_GLO_AD_GetDetMode()         = NAS_EMM_DETACH_MODE_NOT_SWITCH_OFF;

    return;
}


VOS_VOID  NAS_EMM_FreeDetDyn( VOS_VOID )
{
    /*暂时无相关操作*/
    return;
}
VOS_VOID NAS_EMM_Detach_ClearResourse(VOS_VOID)
{

    /*动态内存释放*/
    NAS_EMM_FreeDetDyn();

    /* 停止DETACH定时器 */
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3421);

    /*赋初值*/
    NAS_LMM_MEM_SET(NAS_EMM_GLO_AD_GetDetCtrlAddr(), 0, sizeof(NAS_EMM_DETACH_CTRL_STRU));

    NAS_EMM_GLO_AD_GetDetTypeMo()       = MMC_LMM_MO_DET_PS_ONLY;

    /*初始化 UE 发起 detach 的默认类型*/
    NAS_EMM_GLO_AD_GetDetMode()         = NAS_EMM_DETACH_MODE_NOT_SWITCH_OFF;

    return;
}


VOS_UINT32  NAS_EMM_IntraDetReqChk(const NAS_LMM_INTRA_DETACH_REQ_STRU *pMsgMsg)
{
    /*消息内容检查*/
    if (ID_NAS_LMM_INTRA_DETACH_REQ == pMsgMsg->ulMsgId)
    {
        return  NAS_EMM_PARA_VALID;
    }
    else
    {
        return  NAS_EMM_PARA_INVALID;
    }
}
VOS_VOID  NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_ENUM_UINT32 ulAppRslt  )
{
    if (VOS_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_DETACH_REQ))
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_SendDetRslt: Send LMM_MMC_DETACH_CNF");
        NAS_EMM_AppSendDetCnf(ulAppRslt);
    }
    else
    {
        NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);
    }
}
/*****************************************************************************
 Function Name   : NAS_EMM_GetDetachType
 Description     : 编码DETACH REQ消息时，获取DETACH TYPE

         用户DetachType                   UE MODE
                                PS                      CS+PS
         EPS detach             EPS DETACH              EPS DETACH
         IMSI detach            在预处理里已处理        IMSI DETACH
         EPS/IMSI detach        EPS DETACH              EPS DETACH(当注册域为PS);
                                                        EPS/IMSI DETACH(当注册域为其他)

 Input           : 无
 Output          : 无
 Return          : MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32

 History         :
    1.lihong00150010      2011-09-29  Draft Enact

*****************************************************************************/
MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32 NAS_EMM_GetDetachType( VOS_VOID )
{
    MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32  enDetachType = MMC_LMM_MO_DETACH_TYPE_BUTT;

    /* 获取用户请求的DETACH类型 */
    enDetachType = NAS_EMM_GLO_AD_GetDetTypeMo();

    /* 如果UE模式为PS，则返回类型为EPS ONLY */
    if (NAS_EMM_NO == NAS_EMM_IsCsPsUeMode())
    {
        return MMC_LMM_MO_DET_PS_ONLY;
    }

    /* 如果用户请求的DETACH类型不为EPS/IMSI，则返回用户请求的DETACH类型 */
    if (MMC_LMM_MO_DET_CS_PS != enDetachType)
    {
        return enDetachType;
    }

    /*如果用户请求的是EPS/IMSI DETACH，且当前注册域为PS，则返回EPS ONLY;如果当前
      注册域为CS+PS，则返回EPS/IMSI;如果当前注册域为NULL，则暂时暂时先返回EPS/IMSI，
      不再考虑UE是否被原因值#2拒绝过，也不考虑在收到DETACH消息之前用户是否允许在CS域注册 */
    if (NAS_LMM_REG_DOMAIN_PS == NAS_LMM_GetEmmInfoRegDomain())
    {
        return MMC_LMM_MO_DET_PS_ONLY;
    }
    else
    {
        return MMC_LMM_MO_DET_CS_PS;
    }
}

/*****************************************************************************
 Function Name   : NAS_EMM_GetSwitchOffDetachType
 Description     : 当为关机类型的DETACH时，获取DETACH TYPE

 Input           : 无
 Output          : 无
 Return          : MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32

 History         :
    1.lihong00150010      2012-02-02  Draft Enact

*****************************************************************************/
MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32 NAS_EMM_GetSwitchOffDetachType( VOS_VOID )
{
    VOS_UINT32                          ulCurEmmStat;

    /*如果用户请求的是EPS/IMSI DETACH，且当前注册域为PS，则返回EPS ONLY;如果当前
      注册域为CS+PS，则返回EPS/IMSI;如果当前注册域为NULL时，目前只有注册过程中的
      三种状态会发送关机DETACH消息，因此根据ATTACH类型来填写DETACH类型*/
    if (NAS_LMM_REG_DOMAIN_PS == NAS_LMM_GetEmmInfoRegDomain())
    {
        return MMC_LMM_MO_DET_PS_ONLY;
    }

    if (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain())
    {
        return MMC_LMM_MO_DET_CS_PS;
    }

    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(   NAS_EMM_CUR_MAIN_STAT,
                                                NAS_EMM_CUR_SUB_STAT);

    if ((ulCurEmmStat != NAS_LMM_PUB_COMP_EMMSTATE(    EMM_MS_REG_INIT,
                                                        EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
        && (ulCurEmmStat != NAS_LMM_PUB_COMP_EMMSTATE(  EMM_MS_REG_INIT,
                                                        EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF))
        && (ulCurEmmStat != NAS_LMM_PUB_COMP_EMMSTATE(  EMM_MS_REG_INIT,
                                                        EMM_SS_ATTACH_WAIT_RRC_DATA_CNF)))
    {
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_GetSwitchOffDetachType:Illegal state!");

        return MMC_LMM_MO_DET_PS_ONLY;
    }

    /* 如果是ATTACH过程中的关机，则根据ATTACH类型填写DETACH类型 */
    if (MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType())
    {
        return MMC_LMM_MO_DET_CS_PS;
    }
    else
    {
        return MMC_LMM_MO_DET_PS_ONLY;
    }
}
VOS_VOID    NAS_EMM_CompCnDetachReqMoNasMsg(    VOS_UINT8      *pucCnMsg,
                                                VOS_UINT32     *pulIndex)
{
    VOS_UINT32                          ulIndex      = 0;
    VOS_UINT8                           ucKsi;
    MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32  enDetachType = MMC_LMM_MO_DETACH_TYPE_BUTT;
    VOS_UINT32                          ulLen        = 0;

    ulIndex                             = *pulIndex;

    /* 填充 Protocol Discriminator + Security header type*/
    pucCnMsg[ulIndex++]                 = EMM_CN_MSG_PD_EMM;

    /* 填充 Detach request message identity */
    pucCnMsg[ulIndex++]                 = NAS_EMM_CN_MT_DETACH_REQ_MT;

    /* lihong00150010 swtichoff begin */
    /*填充Detach type*/
    if (NAS_EMM_DETACH_MODE_SWITCH_OFF == NAS_EMM_GLO_AD_GetDetMode())
    {
        enDetachType = NAS_EMM_GetSwitchOffDetachType();
    }
    else
    {
        enDetachType = NAS_EMM_GetDetachType();
    }
    pucCnMsg[ulIndex]                   = (VOS_UINT8)(NAS_EMM_GLO_AD_GetDetMode() |
                                                      enDetachType);
    /* lihong00150010 swtichoff end */
    /*填充KSIasme*/
    ucKsi = NAS_EMM_SecuGetKSIValue();
    pucCnMsg[ulIndex++] |= (ucKsi & 0x0F)<<4;

    /*填充GUTI或IMSI*/
    if (NAS_EMM_AD_BIT_SLCT             == NAS_EMM_GLO_AD_OP_GUTI())
    {
        /*填充GUTI*/
        /*============ GUTI ============ begin */
        pucCnMsg[ulIndex++]             = NAS_EMM_GLO_AD_GetLen();
        pucCnMsg[ulIndex++]             =(NAS_EMM_GLO_AD_GetOeToi()|
                                          NAS_EMM_HIGH_HALF_BYTE_F);

        NAS_LMM_MEM_CPY(             &(pucCnMsg[ulIndex]),
                                        NAS_EMM_GLO_AD_GetGutiPlmn(),
                                        NAS_EMM_AD_LEN_PLMN_ID);
        ulIndex                         += NAS_EMM_AD_LEN_PLMN_ID;
        pucCnMsg[ulIndex++]             = NAS_EMM_GLO_AD_GetMmeGroupId();
        pucCnMsg[ulIndex++]             = NAS_EMM_GLO_AD_GetMMeGroupIdCnt();
        pucCnMsg[ulIndex++]             = NAS_EMM_GLO_AD_GetMMeCode();

        NAS_LMM_MEM_CPY(             &(pucCnMsg[ulIndex]),
                                        NAS_EMM_GLO_AD_GetMTmsiAddr(),
                                        sizeof(NAS_EMM_MTMSI_STRU));
        ulIndex                         += sizeof(NAS_EMM_MTMSI_STRU);


        /*============ GUTI ============ end */
    }/* lihong00150010 emergency tau&service begin */
    else if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_IMSI())
    {/* lihong00150010 emergency tau&service end */
        /*填充IMSI*/
        /*============ IMSI ============ begin */

        NAS_LMM_MEM_CPY(             &(pucCnMsg[ulIndex]),
                                        NAS_EMM_GLO_AD_GetImsi(),
                                        NAS_EMM_AD_LEN_CN_MSG_IMSI);

        ulIndex                         += NAS_EMM_AD_LEN_CN_MSG_IMSI;
        /*============ IMSI ============ end */

    }/* lihong00150010 emergency tau&service begin */
    else
    {
        NAS_EMM_CompImei(&(pucCnMsg[ulIndex]),&ulLen);
        ulIndex                         += ulLen;
    }
	/* lihong00150010 emergency tau&service end */
    *pulIndex = ulIndex;

    return;

}
VOS_VOID    NAS_EMM_CompCnDetachReqMo(  NAS_EMM_MRRC_DATA_REQ_STRU *pDetReqMoMsg)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulDataReqLenNoHeader;
    NAS_EMM_MRRC_DATA_REQ_STRU         *pTempDetReqMsg = pDetReqMoMsg;

    if (NAS_EMM_NULL_PTR == pDetReqMoMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_CompCnDetachReqMo: Mem Alloc Err!");
        return;
    }

    ulIndex = 0;
    /*计算DETACH REQ空口消息*/
    NAS_EMM_CompCnDetachReqMoNasMsg(pTempDetReqMsg->stNasMsg.aucNasMsg, &ulIndex);

    /*内部消息长度计算*/
    ulDataReqLenNoHeader             = NAS_EMM_CountMrrcDataReqLen(ulIndex);

    if ( NAS_EMM_INTRA_MSG_MAX_SIZE < ulDataReqLenNoHeader )
    {
        /* 打印错误信息 */
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_CompCnDetachReqMo, Size error");
        return ;
    }

    /*填充消息长度*/
    pTempDetReqMsg->stNasMsg.ulNasMsgSize    = ulIndex;

    /*填充消息头*/
    NAS_EMM_COMP_AD_INTRA_MSG_HEADER(pTempDetReqMsg, ulDataReqLenNoHeader);

    /*填充消息ID*/
    pTempDetReqMsg->ulMsgId                  = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;

    pTempDetReqMsg->enDataCnf                = LRRC_LMM_DATA_CNF_NEED;

    /*填充消息内容*/
    pTempDetReqMsg->enEstCaue                = LRRC_LNAS_EST_CAUSE_MO_SIGNALLING;
    pTempDetReqMsg->enCallType               = LRRC_LNAS_CALL_TYPE_ORIGINATING_SIGNALLING;
    pTempDetReqMsg->enEmmMsgType             = NAS_EMM_MSG_DETACH_REQ;

    return;
}




VOS_VOID    NAS_EMM_MrrcSendDetReqMo()
{
    NAS_EMM_MRRC_DATA_REQ_STRU          *pIntraMsg;
    NAS_MSG_STRU                        *pstNasMsg;
    MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32  enDetachType = MMC_LMM_MO_DETACH_TYPE_BUTT;

    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MrrcSendDetReqMo is entered");

    /*以最小消息长度，申请消息内存,主要是看是否队列有空间*/
    pIntraMsg = (VOS_VOID *) NAS_LMM_MEM_ALLOC(NAS_EMM_INTRA_MSG_MAX_SIZE);

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pIntraMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MrrcSendDetReqMo: MSG ALLOC ERR!");
        return;

    }

    /*构造发向网侧的DETACH REQUEST 消息*/
    NAS_EMM_CompCnDetachReqMo(pIntraMsg);

    /* 如果是联合TAU，则通知MM进入MM IMSI DETACH PENDING状态 */
    enDetachType = NAS_EMM_GetDetachType();
    if ((NAS_EMM_DETACH_MODE_NOT_SWITCH_OFF == NAS_EMM_GLO_AD_GetDetMode())
        && ((MMC_LMM_MO_DET_CS_PS == enDetachType)
            || (MMC_LMM_MO_DET_CS_ONLY == enDetachType)))
    {
        NAS_EMM_SendMmCombinedStartNotifyReq(MM_LMM_COMBINED_DETACH);
    }

    /*向OM发送空口消息DETACH REQUEST*/
    pstNasMsg = (NAS_MSG_STRU *)(&(pIntraMsg->stNasMsg));
    NAS_LMM_SendOmtAirMsg(NAS_EMM_OMT_AIR_MSG_UP, NAS_EMM_DETACH_REQ_MO, pstNasMsg);
    NAS_LMM_SendOmtKeyEvent(             EMM_OMT_KE_DETACH_REQ_MO);

    /*向MRRC发送DETACH REQUEST 消息*/
    NAS_EMM_SndUplinkNasMsg(          	pIntraMsg);

    NAS_LMM_MEM_FREE(pIntraMsg);
    return;

}

VOS_VOID NAS_EMM_SendImsiDetachReqMo( VOS_VOID )
{
    NAS_LMM_FSM_STATE_STRU               stEmmState;

    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_SendImsiDetachReqMo is enter.");

    /*启动定时器T3421*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_T3421);

    /*修改状态：进入主状态EMM_MS_REG子状态EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF*/
    stEmmState.enFsmId                    = NAS_LMM_PARALLEL_FSM_EMM;
    stEmmState.enMainState                = EMM_MS_REG;
    stEmmState.enSubState                 = EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF;
    stEmmState.enStaTId                   = TI_NAS_EMM_T3421;
    NAS_LMM_StaTransProc(stEmmState);

    /*向MRRC发送DETACH REQUEST消息*/
    NAS_EMM_MrrcSendDetReqMo();

    return;
}
VOS_VOID  NAS_EMM_SendDetachReqMo(VOS_VOID)
{

    NAS_LMM_FSM_STATE_STRU               stEmmState;

    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_SendDetachReqMo is enter.");


    /*启动定时器T3421*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_T3421);

    /*修改状态：进入主状态DEREG_INIT子状态DETACH_WAIT_CN_DETACH_CNF*/
    stEmmState.enFsmId                    = NAS_LMM_PARALLEL_FSM_EMM;
    stEmmState.enMainState                = EMM_MS_DEREG_INIT;
    stEmmState.enSubState                 = EMM_SS_DETACH_WAIT_CN_DETACH_CNF;
    stEmmState.enStaTId                   = TI_NAS_EMM_T3421;
    NAS_LMM_StaTransProc(stEmmState);

    /*向MRRC发送DETACH REQUEST消息*/
    NAS_EMM_MrrcSendDetReqMo();
    return;
}
VOS_VOID NAS_EMM_RrcRelCauseCnDetachSrcClrAttemptToAtt(VOS_VOID)
{
    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_RrcRelCauseCnDetachSrcClrAttemptToAtt is entered!");

    /*delete list of equivalent PLMNs, */
    /*NAS_EMM_INFO_DELETE_EPLMN_LIST();*/

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /*shall set the update status to EU2 NOT UPDATED */
    NAS_EMM_MAIN_CONTEXT_SET_AUX_UPDATE_STAE(EMM_US_NOT_UPDATED_EU2);

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

    /*and shall start timer T3402. */
    NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3402);

    /*clear Attach Attempt Counter*/
    /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/

    return;
}


VOS_VOID    NAS_EMM_SendDetachForPowerOffReq(VOS_VOID)
{
    /* 设置DETACH类型为关机DETACH */
    NAS_EMM_GLO_AD_GetDetMode()         = NAS_EMM_DETACH_MODE_SWITCH_OFF;

    /* 向MRRC发送DETACH REQUEST消息 */
    NAS_EMM_MrrcSendDetReqMo();
}


VOS_VOID    NAS_EMM_DetProcEnterForbTA(VOS_VOID)
{
    /* 终止DETACH流程*/
    NAS_LMM_StopStateTimer(           TI_NAS_EMM_T3421);

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
    NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);
    /* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE == NAS_EMM_GLO_AD_GetUsimPullOutFlag())
    {
        /*状态转换，通知MMC卡无效*/
        NAS_EMM_ProcLocalNoUsim();
    }/* lihong00150010 emergency tau&service end */
    else
    {
        /* 状态迁移:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                    EMM_SS_DEREG_LIMITED_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);
        /* 本地DETACH*/
        NAS_LMM_DeregReleaseResource();
    }

    /*向MMC发送LMM_MMC_DETACH_CNF消息*/
    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

    NAS_EMM_RelReq(                  NAS_LMM_NOT_BARRED);

    return;
}
/* lihong00150010 emergency tau&service begin */
/*****************************************************************************
 Function Name   : NAS_EMM_ImsiDetProcEnterForbTA
 Description     : REG.IMSI_DETACH_INIT状态下收到SYS_INFO，且携带禁止信息，终止DETACH
                   流程，释放链路
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010     2011-10-18  Draft Enact
    2.lihong00150010     2012-12-19  Modify:Emergency

*****************************************************************************/
VOS_VOID    NAS_EMM_ImsiDetProcEnterForbTA( VOS_VOID )
{
    /* 终止DETACH流程*/
    NAS_LMM_StopStateTimer(           TI_NAS_EMM_T3421);

    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                EMM_SS_REG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_DETACH_CNF消息*/
    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

    /* IMSI DETACH后释放资源 */
    NAS_LMM_ImsiDetachReleaseResource();

    NAS_EMM_RelReq(                  NAS_LMM_NOT_BARRED);

    return;
}
/* lihong00150010 emergency tau&service end */

VOS_VOID  NAS_EMM_MsDrgInitSsWtCnDetCnfProcMsgRrcRelInd( VOS_UINT32 ulCause)
{
    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsDrgInitSsWtCnDetCnfProcMsgRrcRelInd is entered");

    (VOS_VOID)(                             ulCause);

    /*停止定时器T3421*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3421);

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
     NAS_EMM_EsmSendStatResult(       EMM_ESM_ATTACH_STATUS_DETACHED);
    /* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE == NAS_EMM_GLO_AD_GetUsimPullOutFlag())
    {/* lihong00150010 emergency tau&service end */
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsDrgInitSsWtCnDetCnfProcMsgRrcRelInd: No USIM");

        NAS_EMM_ProcLocalNoUsim();

        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();

        return;
    }

    /* 有卡，不区分释放原因值，完成本地DETACH*/

    /*修改状态：进入主状态DEREG子状态DEREG_NORMAL_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

    /* 本地DETACH释放资源:动态内存、赋初值 */
    NAS_LMM_DeregReleaseResource();

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    return;
}

VOS_UINT32  NAS_EMM_MsDrgInitSsWtCnDetCnfMsgAuthFail(
                                                VOS_UINT32  ulMsgId,
                                                VOS_VOID   *pMsgStru)
{

    NAS_EMM_INTRA_AUTH_FAIL_STRU        *pMsgAuthFail   = (NAS_EMM_INTRA_AUTH_FAIL_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    (VOS_VOID)(ulMsgId);
    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgAuthFail is entered.");

    /*获得原因值*/
    ulCause                                             =   pMsgAuthFail->ulCause;

    /*依据原因值处理*/
    if(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL              ==  ulCause)
    {
        NAS_EMM_ProcDetachAuthRej(                          ulCause);
    }
    else
    {
        NAS_EMM_MsDrgInitSsWtCnDetCnfProcMsgRrcRelInd(      ulCause);
    }

    return   NAS_LMM_MSG_HANDLED;
}

/*****************************************************************************
 Function Name   : NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgAuthFail
 Description     : 主状态EMM_MS_REG+子状态EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF下收到AUTH FAIL消息
                   在AUTH过程中,收到RRC_REL_IND或CN_AUTH_REJ,AUTH模块都会转发AUTH FAIL
                   所以在收到AUTH_FAIL时会处理RRC_REL_IND或CN_AUTH_REJ两种消息
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-09-28  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgAuthFail
(
    VOS_UINT32                          ulMsgId,
    const VOS_VOID                     *pMsgStru
)
{

    NAS_EMM_INTRA_AUTH_FAIL_STRU        *pMsgAuthFail   = (NAS_EMM_INTRA_AUTH_FAIL_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgAuthFail is entered",
                                                ulMsgId,
                                                pMsgStru);

    /*获得原因值*/
    ulCause = pMsgAuthFail->ulCause;

    /*依据原因值处理*/
    if(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL ==  ulCause)
    {
        NAS_EMM_ProcDetachAuthRej(ulCause);
    }
    else
    {
        NAS_EMM_ProcMsRegImsiDetachInitMsgRrcRelInd(ulCause);
    }

    return   NAS_LMM_MSG_HANDLED;
}



VOS_UINT32  NAS_EMM_MsDrgInitSsWtCnDetCnfMsgRrcRelInd(
                                                VOS_UINT32  ulMsgId,
                                                VOS_VOID   *pMsgStru)
{

    LRRC_LMM_REL_IND_STRU                 *pMsgRrcRelInd  = (LRRC_LMM_REL_IND_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    (VOS_VOID)(ulMsgId);
    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgRrcRelInd is entered.");

    /*获得原因值*/
    ulCause                                             =   pMsgRrcRelInd->enRelCause;

    NAS_EMM_MsDrgInitSsWtCnDetCnfProcMsgRrcRelInd(          ulCause);

    return   NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgIntraConnectFailInd
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU         *pMrrcConnectFailInd = NAS_EMM_NULL_PTR;

    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgIntraConnectFailInd is entered",
                                                ulMsgId,
                                                pMsgStru);
    pMrrcConnectFailInd = (NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU *)pMsgStru;

    /*check the input ptr*/
    if (NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgIntraConnectFailInd: NULL PTR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*check current state*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF))
    {
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgIntraConnectFailInd: STATE ERR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*停止定时器T3421*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3421);

    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    /*修改状态：进入主状态REG子状态EMM_SS_REG_NORMAL_SERVICE*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG,
                                        EMM_SS_REG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    if ((LRRC_EST_ACCESS_BARRED_MO_SIGNAL == pMrrcConnectFailInd->enEstResult) ||
        (LRRC_EST_ACCESS_BARRED_ALL == pMrrcConnectFailInd->enEstResult))
    {
        /*向MMC发送LMM_MMC_DETACH_CNF消息*/
        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_ACCESS_BARED);
    }
    else
    {
        /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);
    }

    /* IMSI DETACH后释放资源 */
    NAS_LMM_ImsiDetachReleaseResource();

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    return   NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDrgInitSsWtCnDetCnfMsgIntraConnectFailInd(
                                                VOS_UINT32  ulMsgId,
                                                VOS_VOID   *pMsgStru)
{

    NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU         *pMrrcConnectFailInd = NAS_EMM_NULL_PTR;

    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgIntraConnectFailInd is entered",
                                                ulMsgId,
                                                pMsgStru);
    pMrrcConnectFailInd = (NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU *)pMsgStru;
    /*check the input ptr*/
    if (NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgIntraConnectFailInd: NULL PTR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*check current state*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG_INIT,EMM_SS_DETACH_WAIT_CN_DETACH_CNF))
    {
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgIntraConnectFailInd: STATE ERR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*停止定时器T3421*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3421);

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
    NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);
    /* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE != NAS_EMM_GLO_AD_GetUsimPullOutFlag())
    {/* lihong00150010 emergency tau&service end */
        /*修改状态：进入主状态DEREG子状态DEREG_NORMAL_SERVICE*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_NORMAL_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {
        /*NAS_EMM_ProcLocalNoUsim();*/
        /*修改状态：进入主状态DEREG子状态DEREG_NORMAL_SERVICE*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_NO_IMSI,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /* 给MMC回复LMM_MMC_USIM_STATUS_CNF */
        NAS_EMM_SendMmcUsimStatusCnf();
		/* lihong00150010 emergency tau&service begin */
        /* 清除拔卡标识 */
        NAS_EMM_GLO_AD_GetUsimPullOutFlag() = VOS_FALSE;
		/* lihong00150010 emergency tau&service end */
    }

    if ((LRRC_EST_ACCESS_BARRED_MO_SIGNAL == pMrrcConnectFailInd->enEstResult) ||
        (LRRC_EST_ACCESS_BARRED_ALL == pMrrcConnectFailInd->enEstResult))
    {
        /*向MMC发送LMM_MMC_DETACH_CNF消息*/
        /*NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_ACCESS_BARED);*/
        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_ACCESS_BARED);
    }
    else
    {
        /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
        /*NAS_EMM_MmcSendDetIndLocal();*/
        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);
    }

   /* 本地DETACH释放资源:动态内存、赋初值 */
   NAS_LMM_DeregReleaseResource();

   /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
   NAS_EMM_CommProcConn2Ilde();
   return   NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgT3421Exp
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgT3421Exp",
                                                    ulMsgId,
                                                    pMsgStru);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgT3421Exp: STATE ERR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*Detach attempt counter加1*/
    NAS_EMM_GLO_AD_GetDetAtmpCnt()++;

    /*判断Detach attempt counter，分为小于5和大于等于5处理*/
    if (NAS_EMM_GLO_AD_GetDetAtmpCnt() < 5)
    {
        /*发送DETACH REQUEST消息*/
        NAS_EMM_SendImsiDetachReqMo();

        return NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgT3421Exp 5 times");

    NAS_EMM_GLO_AD_GetDetAtmpCnt()  = 0;

    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    /* 如果当前是数据连接态，则直接转到REG+NORMAL_SERVIC态，否则待释放RRC链路
       后再转到REG+NORMAL_SERVIC态 */
    if (NAS_EMM_CONN_DATA == NAS_EMM_GetConnState())
    {
        NAS_EMM_AdStateConvert(             EMM_MS_REG,
                                            EMM_SS_REG_NORMAL_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

        /* IMSI DETACH后释放资源 */
        NAS_LMM_ImsiDetachReleaseResource();

        return NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_AdStateConvert( EMM_MS_REG,
                            EMM_SS_REG_NORMAL_SERVICE,
                            TI_NAS_EMM_STATE_NO_TIMER);

    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDrgInitSsWtCnDetCnfMsgT3421Exp( VOS_UINT32  ulMsgId,
                                                      VOS_VOID   *pMsgStru )
{
    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgT3421Exp",
                                                    ulMsgId,
                                                    pMsgStru);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG_INIT,EMM_SS_DETACH_WAIT_CN_DETACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgT3421Exp: STATE ERR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*Detach attempt counter加1*/
    NAS_EMM_GLO_AD_GetDetAtmpCnt()++;

    /*判断Detach attempt counter，分为小于5和大于等于5处理*/
    if (NAS_EMM_GLO_AD_GetDetAtmpCnt() < 5)
    {
        /*发送DETACH REQUEST消息*/
        NAS_EMM_SendDetachReqMo();
    }
    else
    {
        NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgT3421Exp 5 times");

        /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
        NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);
        /* lihong00150010 emergency tau&service begin */
        if (VOS_TRUE == NAS_EMM_GLO_AD_GetUsimPullOutFlag())
        {
            /*状态转换，通知MMC卡无效*/
            NAS_EMM_ProcLocalNoUsim();
        }/* lihong00150010 emergency tau&service end */
        else
        {
            NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                    EMM_SS_DEREG_NORMAL_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        }
        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

        /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
        NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    }
    return  NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_EMM_DetachToTAU()
{
    /*停止定时器T3421*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3421);

    /*修改状态：进入主状态REG子状态REG_NORMAL_SERVICE*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG,
                                        EMM_SS_REG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

   return;
}



VOS_UINT32  NAS_EMM_MsRegSsRegNmlSrvMsgIntraDetReq(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru)
{
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt = MMC_LMM_TAU_RSLT_BUTT;
    NAS_LMM_INTRA_DETACH_REQ_STRU              *pRcvEmmMsg;
    pRcvEmmMsg                          = (NAS_LMM_INTRA_DETACH_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG1_INFO("NAS_EMM_MsRegSsRegNmlSrvMsgTauIntraMsgDetReq is entered", ulMsgId);


     /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegSsRegNmlSrvMsgTauIntraMsgDetReq: STATE ERR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_IntraDetReqChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsRegSsRegNmlSrvMsgTauIntraMsgDetReq: MMC_MM_SYS_INFO_IND_STRU para err!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }

    /* 如果是IMSI DETACH类型，则发起IMSI DETACH流程 */
    if (MMC_LMM_MO_DET_CS_ONLY == NAS_EMM_GLO_AD_GetDetTypeMo())
    {
        NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsRegSsRegNmlSrvMsgTauIntraMsgDetReq:Proceed Imsi Detach");

        /* IMSI detach被TAU打断，如果TAU ACCEPT是Ta Updated Only，原因值为#2或者#18
           也会进入NORMAL_SERVICE，发送内部DETACH请求，这时不应该再发起IMSI DETACH流程*/
        if (NAS_LMM_REG_DOMAIN_PS == NAS_LMM_GetEmmInfoRegDomain())
        {
            /*向APP发送DETACH成功*/
            NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);

            /* IMSI DETACH后释放资源 */
            NAS_LMM_ImsiDetachReleaseResource();

            return  NAS_LMM_MSG_HANDLED;
        }

        /* 发起IMSI DETACH流程 */
        NAS_EMM_SendImsiDetachReqMo();
        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsRegSsRegNmlSrvMsgTauIntraMsgDetReq:Proceed EPS Detach or EPS/IMSI Detach");

    /* 如果3411在运行，需要给MMC报TAU结果，以让MMC退出搜网状态机；3402定时器在
       运行无需通知，MMC收到TAU结果尝试次数为5时，已退出搜网状态机 */
    if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
    {
        NAS_EMM_SetTauTypeNoProcedure();

        ulTauRslt = MMC_LMM_TAU_RSLT_MO_DETACH_FAILURE;
        NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);

    }

    /*停止定时器*/
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3411);
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3402);

    /*发送DETACH REQUEST消息*/
    NAS_EMM_SendDetachReqMo();

    return   NAS_LMM_MSG_HANDLED;
}

/*****************************************************************************
 Function Name   : NAS_EMM_DetGuAttachRst
 Description     : 挂起状态下，处理GU发来的DETACH结果
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_VOID

 History         :
    1.Hanlufeng 41410      2011-5-12  Draft Enact

*****************************************************************************/
VOS_VOID NAS_EMM_DetGuAttachRst( VOS_VOID * pMsg )
{
    MMC_LMM_ACTION_RESULT_REQ_STRU     *pMmcActResult;


    pMmcActResult = (MMC_LMM_ACTION_RESULT_REQ_STRU *)pMsg;


    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_AttGuAttachRst: ulActRst =.",
                            pMmcActResult->ulActRst);

    /* 若非PS相关的，暂时丢弃，待联合操作时再考虑 */
    if(MMC_LMM_IMSI_ONLY == pMmcActResult->ulRstDomain)
    {
        NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_AttGuAttachRst: ulRstDomain =.",
                            pMmcActResult->ulRstDomain);
        return;
    }

    /* DETACH 无论成功失败，都认为去注册完成 */
    NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                    EMM_SS_DEREG_NO_CELL_AVAILABLE,
                                    TI_NAS_EMM_STATE_NO_TIMER);


    /* 通知ESM状态改变 */
    NAS_EMM_PUB_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);

    return;
}
VOS_UINT32 NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgEsmDataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    EMM_ESM_DATA_REQ_STRU              *pstEsmDataReq = (EMM_ESM_DATA_REQ_STRU*)pMsgStru;

    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgEsmDataReq",
                                                            ulMsgId,
                                                            pMsgStru);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgEsmDataReq: STATE ERR!");
        return NAS_LMM_MSG_DISCARD;
    }

    if (VOS_TRUE == pstEsmDataReq->ulIsEmcType)
    {
        NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_TRUE);
    }

    NAS_EMM_SER_SendMrrcDataReq_ESMdata(&pstEsmDataReq->stEsmMsg);

    return NAS_LMM_MSG_HANDLED;
}



VOS_UINT32 NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgTcDataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgTcDataReq",
                                                            ulMsgId,
                                                            pMsgStru);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgTcDataReq: STATE ERR!");
        return NAS_LMM_MSG_DISCARD;
    }

    NAS_EMM_SER_SendMrrcDataReq_Tcdata((EMM_ETC_DATA_REQ_STRU *)pMsgStru);

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32 NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgRabmRelReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgRabmRelReq",
                                                            ulMsgId,
                                                            pMsgStru);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgRabmRelReq: STATE ERR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*停止定时器T3421*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3421);

    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                    EMM_SS_REG_NORMAL_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);
    /*向MMC发送LMM_MMC_DETACH_CNF消息*/
    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

    /* IMSI DETACH后释放资源 */
    NAS_LMM_ImsiDetachReleaseResource();

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_SndDetachReqFailProc(VOS_VOID* pMsg,VOS_UINT32 *pulIsDelBuff)
{
    LRRC_LMM_DATA_CNF_STRU              *pstRrcMmDataCnf = VOS_NULL_PTR;

    *pulIsDelBuff = VOS_TRUE;

    pstRrcMmDataCnf = (LRRC_LMM_DATA_CNF_STRU*) pMsg;

    if ((EMM_MS_DEREG_INIT != NAS_LMM_GetEmmCurFsmMS())
        && ((EMM_MS_REG != NAS_LMM_GetEmmCurFsmMS())
            && (EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF != NAS_LMM_GetEmmCurFsmSS())))
    {
        /*打印出错信息*/
        NAS_EMM_SER_LOG_INFO("NAS_EMM_SndExtendedServiceReqFailProc: STATE ERR!");
        return  NAS_EMM_SUCC;
    }
    switch (pstRrcMmDataCnf->enSendRslt)
    {
        case LRRC_LMM_SEND_RSLT_FAILURE_HO:
        case LRRC_LMM_SEND_RSLT_FAILURE_TXN:
        case LRRC_LMM_SEND_RSLT_FAILURE_RLF:
            if (EMM_MS_DEREG_INIT == NAS_LMM_GetEmmCurFsmMS())
            {
                NAS_LMM_StopStateTimer(         TI_NAS_EMM_T3421);

                /*发送DETACH REQUEST消息*/
                NAS_EMM_SendDetachReqMo();
            }
            else
            {
                NAS_LMM_StopStateTimer(         TI_NAS_EMM_T3421);

                /* 发起IMSI DETACH流程 */
                NAS_EMM_SendImsiDetachReqMo();
            }
            break;
        default:
            break;
        }

    return NAS_EMM_SUCC;
}
VOS_UINT32 NAS_EMM_SndMtDetachAccFailProc(VOS_VOID* pMsg,VOS_UINT32 *pulIsDelBuff)
{
    LRRC_LMM_DATA_CNF_STRU              *pstRrcMmDataCnf = VOS_NULL_PTR;
    NAS_EMM_MRRC_DATA_REQ_STRU          *pMrrcDataMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRrcMmDataReqMsgLen;
    NAS_EMM_MRRC_MGMT_DATA_STRU         *pEmmMrrcMgmtData = NAS_EMM_NULL_PTR;

    *pulIsDelBuff = VOS_TRUE;

    pstRrcMmDataCnf = (LRRC_LMM_DATA_CNF_STRU*) pMsg;

    pEmmMrrcMgmtData = NAS_EMM_FindMsgInDataReqBuffer(pstRrcMmDataCnf->ulOpId);

    if (NAS_EMM_NULL_PTR == pEmmMrrcMgmtData)
    {
        return NAS_EMM_FAIL;
    }

    ulRrcMmDataReqMsgLen = sizeof(NAS_EMM_MRRC_DATA_REQ_STRU)+
                           pEmmMrrcMgmtData->ulNasMsgLength -
                           NAS_EMM_4BYTES_LEN;
    /* 申请消息内存*/
    pMrrcDataMsg = (NAS_EMM_MRRC_DATA_REQ_STRU *)((VOS_VOID*)NAS_LMM_MEM_ALLOC(ulRrcMmDataReqMsgLen));

    if(VOS_NULL_PTR == pMrrcDataMsg)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SndMtDetachAccFailProc: Mem Alloc Fail");
        return NAS_EMM_FAIL;
    }

    switch (pstRrcMmDataCnf->enSendRslt)
    {
        case LRRC_LMM_SEND_RSLT_FAILURE_HO:
        case LRRC_LMM_SEND_RSLT_FAILURE_TXN:
        case LRRC_LMM_SEND_RSLT_FAILURE_RLF:
            if (NAS_EMM_MAX_MT_DETACH_ACC_TX_NUM > NAS_EMM_GLO_AD_GetMtDetachAccTxCnt())
            {
                NAS_EMM_GLO_AD_GetMtDetachAccTxCnt() ++;

                pMrrcDataMsg->enDataCnf = LRRC_LMM_DATA_CNF_NEED;

                pMrrcDataMsg->enEmmMsgType = NAS_EMM_MSG_DETACH_ACP;
                pMrrcDataMsg->ulMsgId = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;
                pMrrcDataMsg->ulEsmMmOpId = pEmmMrrcMgmtData->ulEsmMmOpId;

                pMrrcDataMsg->stNasMsg.ulNasMsgSize = pEmmMrrcMgmtData->ulNasMsgLength;

                NAS_LMM_MEM_CPY(pMrrcDataMsg->stNasMsg.aucNasMsg,
                                 pEmmMrrcMgmtData->aucNasMsgContent,
                                 pEmmMrrcMgmtData->ulNasMsgLength);

                NAS_EMM_SndUplinkNasMsg(pMrrcDataMsg);
            }
            else
            {
                NAS_EMM_GLO_AD_GetMtDetachAccTxCnt() = 0;
            }

            break;

        default:
            break;
        }

    /*释放所申请内存 */
    NAS_LMM_MEM_FREE(pMrrcDataMsg);
    return NAS_EMM_SUCC;
}
/*lint +e961*/
/*lint +e960*/
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

