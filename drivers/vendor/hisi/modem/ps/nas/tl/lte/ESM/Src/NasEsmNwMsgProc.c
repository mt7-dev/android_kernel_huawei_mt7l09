


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "NasEsmNwMsgProc.h"
#include  "NasCommBuffer.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASESMNWMSGPROC_C
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
 Function Name   : NAS_ESM_ValidateTftOpCreateTftRsltProc
 Description     : 对承载修改的TAD码为Create TFT 检查结果的处理
 Input           : enRslt          -- 检查结果
                   pstEsmNwMsgIE   -- 译码后的空口消息指针
                   ulStateTblIndex -- 动态表索引
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.liuwenyu00143951      2010-5-14  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_ValidateTftOpCreateTftRsltProc
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE,
    NAS_ESM_ENCODE_INFO_STRU           *pstEncodeInfo,
    NAS_ESM_QUERY_CID_RSLT_ENUM_UINT8   enRslt,
    VOS_UINT32                          ulStateTblIndex
)
{
    (VOS_VOID)enRslt;

    /* Create TFT时,对应的专有承载的TFT不能存在 */
    if (NAS_ESM_BEARER_TYPE_DEDICATED == NAS_ESM_GetBearCntxtType(pstEsmNwMsgIE->ucEpsbId))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ValidateTftOpCreateTftRsltProc:The created Ttf already exist!");
        pstEncodeInfo->ucESMCau = NAS_ESM_CAUSE_SEMANTIC_ERR_IN_TFT;
        NAS_ESM_BearModFailProc(pstEncodeInfo, ulStateTblIndex);
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateTftOpNoOpRsltProc
 Description     : 对承载修改的TAD码为No operation 检查结果的处理
 Input           : enRslt          -- 检查结果
                   pstEsmNwMsgIE   -- 译码后的空口消息指针
                   ulStateTblIndex -- 动态表索引
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.liuwenyu00143951      2010-5-14  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_ValidateTftOpNoOpRsltProc
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE,
    NAS_ESM_ENCODE_INFO_STRU           *pstEncodeInfo,
    NAS_ESM_QUERY_CID_RSLT_ENUM_UINT8   enRslt,
    VOS_UINT32                          ulStateTblIndex
)
{
    (VOS_VOID)pstEsmNwMsgIE;

    /* 修改Qos时,对应的filter必须全部找到或者部分找到，且属于同一CID下 */
    if (NAS_ESM_QUERY_CID_RSLT_ALL_UNFOUND == enRslt)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ValidateTftOpNoOpRsltProc:Tad code error!");
        pstEncodeInfo->ucESMCau = NAS_ESM_CAUSE_SEMANTIC_ERR_IN_TFT;
        NAS_ESM_BearModFailProc(pstEncodeInfo, ulStateTblIndex);
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_CreatePfPrecedenceValidateList
 Description     : 获取某承载下packet filter优先级验证列表
 Input           : ucEpsbId------------------承载号
 Output          : pucPfNum------------------packet filter数
                   pstPfPVList---------------优先级验证列表首地址
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-5-27  Draft Enact

*****************************************************************************/
/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_ESM_GetPfPrecedenceValidateListInBearer
(
    VOS_UINT8                           ucEpsbId,
    VOS_UINT8                          *pucPfNum,
    NAS_ESM_PF_PRCDNC_VLDT_STRU        *pstPfPVList
)
{
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEspbCntxtInfo    = VOS_NULL_PTR;
    NAS_ESM_CONTEXT_TFT_STRU           *pstPacketFilter     = VOS_NULL_PTR;
    NAS_ESM_SDF_CNTXT_INFO_STRU        *pstSdfCntxtInfo     = VOS_NULL_PTR;
    VOS_UINT32                          ulCnt1              = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt2              = NAS_ESM_NULL;

    pstEspbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ucEpsbId);

    NAS_ESM_INFO_LOG1("Show all PF of specefic EPSB ID below:",ucEpsbId);

    for(ulCnt1 = NAS_ESM_MIN_CID; ulCnt1 <= NAS_ESM_MAX_CID; ulCnt1++)
    {
        if(NAS_ESM_OP_TRUE !=((pstEspbCntxtInfo->ulBitCId >> ulCnt1 )& NAS_ESM_BIT_0))
        {
            continue;
        }

        pstSdfCntxtInfo = NAS_ESM_GetSdfCntxtInfo(ulCnt1);

        for (ulCnt2 = NAS_ESM_NULL; ulCnt2 < pstSdfCntxtInfo->ulSdfPfNum; ulCnt2++)
        {
            pstPacketFilter = &pstSdfCntxtInfo->astSdfPfInfo[ulCnt2];
            pstPfPVList[*pucPfNum].ucEpsbId = (VOS_UINT8)pstSdfCntxtInfo->ulEpsbId;
            pstPfPVList[*pucPfNum].ucPfId = pstPacketFilter->ucNwPacketFilterId;
            pstPfPVList[*pucPfNum].ucPrecedence = pstPacketFilter->ucPrecedence;
            /* 设置TFT的时候验证优先级会比较Cid */
            pstPfPVList[*pucPfNum].ucCid = (VOS_UINT8)ulCnt1;
            NAS_ESM_INFO_LOG2("CID: Precedence:", ulCnt1, pstPacketFilter->ucPrecedence);
            (*pucPfNum)++;
        }
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateDelPfRenderEmpty
 Description     : 验证delete packet filter是否会导致专有承载的TFT为空
 Input           : pstNwMsg-------------------------译码后的空口消息
 Output          : None
 Return          : NAS_ESM_CAUSE_ENUM_UINT8

 History         :
    1.lihong00150010      2010-5-28  Draft Enact

*****************************************************************************/
NAS_ESM_CAUSE_ENUM_UINT8  NAS_ESM_ValidateDelPfRenderEmpty
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg
)
{
    VOS_UINT8                           ucPfPVNum       = NAS_ESM_NULL;
    VOS_UINT8                           ucCnt1          = NAS_ESM_NULL;
    VOS_UINT8                           ucCnt2          = NAS_ESM_NULL;
    VOS_UINT8                           ucNumTmp        = NAS_ESM_NULL;
    NAS_ESM_PF_PRCDNC_VLDT_STRU         astPfPVListTmp[NAS_ESM_MAX_SDF_PF_NUM]= {0};
    const NAS_ESM_CONTEXT_TFT_STRU     *pstPacketFilter = VOS_NULL_PTR;

    if (PS_TRUE == NAS_ESM_IsDefaultEpsBearerType(NAS_ESM_GetBearCntxtType(pstNwMsg->ucEpsbId)))
    {
        return NAS_ESM_CAUSE_SUCCESS;
    }

    if (NAS_ESM_TFT_OP_TYPE_DELETE_FILTER != pstNwMsg->stTadInfo.enTftOpType)
    {
        return NAS_ESM_CAUSE_SUCCESS;
    }

    /* 获取此承载下packet filter优先级验证列表 */
    NAS_ESM_GetPfPrecedenceValidateListInBearer(pstNwMsg->ucEpsbId,
                                                &ucPfPVNum,
                                                astPfPVListTmp);

    /* 空口消息中携带的packet filter数目小于承载下已拥有的packet filter数，则
       不可能将承载下的所有packet filter全部删除 */
    if (pstNwMsg->stTadInfo.ucSdfPfNum < ucPfPVNum)
    {
        return NAS_ESM_CAUSE_SUCCESS;
    }

    /* 判断承载下已拥有的所有packet filter是否会被全部删除 */
    for (ucCnt1 = NAS_ESM_NULL; ucCnt1 < pstNwMsg->stTadInfo.ucSdfPfNum; ucCnt1++)
    {
        pstPacketFilter = &pstNwMsg->stTadInfo.astSdfPfInfo[ucCnt1];
        for (ucCnt2 = NAS_ESM_NULL; ucCnt2 < ucPfPVNum; ucCnt2++)
        {
            if (pstPacketFilter->ucNwPacketFilterId == astPfPVListTmp[ucCnt2].ucPfId)
            {
                ucNumTmp++;
            }
        }
    }

    if (ucNumTmp == ucPfPVNum)
    {
        return NAS_ESM_CAUSE_SEMANTIC_ERR_IN_TFT;
    }

    return NAS_ESM_CAUSE_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateTftOpDelTftOrDelPfRsltProc
 Description     : 对承载修改的TAD码为Delete filter,Delete TFT
                   检查结果的处理
 Input           : enRslt          -- 检查结果
                   pstEsmNwMsgIE   -- 译码后的空口消息指针
                   ulStateTblIndex -- 动态表索引
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.liuwenyu00143951      2010-5-14  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_ValidateTftOpDelTftOrDelPfRsltProc
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE,
    NAS_ESM_ENCODE_INFO_STRU           *pstEncodeInfo,
    NAS_ESM_QUERY_CID_RSLT_ENUM_UINT8   enRslt,
    VOS_UINT32                          ulStateTblIndex
)
{
    VOS_UINT32                          ulSndNwMsgLen   = NAS_ESM_NULL;
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause      = NAS_ESM_NULL;


    /* 如果没找到CID，且当前TAD操作码为DELETE_FILTER或者DELETE TFT，则回复修改承载成功 */
    if (NAS_ESM_QUERY_CID_RSLT_ALL_UNFOUND == enRslt)
    {
        NAS_ESM_INFO_LOG("NAS_ESM_ValidateTftOpDelTftOrDelPfRsltProc: accept delete not exist filter !");

        /*向网侧回复Modify EPS bearer context accept*/
        NAS_ESM_EncodeModEpsbCntxtAccMsg(pstEncodeInfo, \
                                 NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                &ulSndNwMsgLen);
        NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                   ulSndNwMsgLen, NAS_ESM_AIR_MSG_DIR_ENUM_UP,\
                                   ESM_MOD_EPS_BEARER_CNTXT_ACP);
        NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID, PS_FALSE, ulSndNwMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
        NAS_ESM_UpdateEsmSndNwMsgLen(ulSndNwMsgLen);

        return NAS_ESM_MSG_HANDLED;
    }

    /* 专有承载不能删除TFT */
    if ((NAS_ESM_TFT_OP_TYPE_DELETE_TFT == pstEsmNwMsgIE->stTadInfo.enTftOpType)
        && (NAS_ESM_BEARER_TYPE_DEDICATED == NAS_ESM_GetBearCntxtType(pstEsmNwMsgIE->ucEpsbId)))
    {
        /*打印信息*/
        NAS_ESM_WARN_LOG("NAS_ESM_ValidateTftOpDelTftOrDelPfRsltProc:Illegal TAD Operation !");

        pstEncodeInfo->ucESMCau = NAS_ESM_CAUSE_SEMANTIC_ERR_IN_TFT;
        NAS_ESM_BearModFailProc(pstEncodeInfo, ulStateTblIndex);
        return NAS_ESM_MSG_HANDLED;
    }

    /* 验证delete packet filter是否会导致专有承载的TFT为空，若是回复拒绝消息，
       原因值为NAS_ESM_CAUSE_SEMANTIC_ERR_IN_TFT */
    enEsmCause = NAS_ESM_ValidateDelPfRenderEmpty(pstEsmNwMsgIE);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        /*打印信息*/
        NAS_ESM_WARN_LOG("NAS_ESM_ValidateTftOpDelTftOrDelPfRsltProc:NAS_ESM_ValidateDelPfRenderEmpty failed!");

        pstEncodeInfo->ucESMCau = enEsmCause;
        NAS_ESM_BearModFailProc(pstEncodeInfo, ulStateTblIndex);
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateTftOpRsltProc
 Description     : 对承载修改的TAD码检查结果的处理
 Input           : enRslt          -- 检查结果
                   pstEsmNwMsgIE   -- 译码后的空口消息指针
                   ulStateTblIndex -- 动态表索引
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.liuwenyu00143951      2010-5-14  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_ValidateTftOpRsltProc
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE,
    NAS_ESM_QUERY_CID_RSLT_ENUM_UINT8   enRslt,
    NAS_ESM_ENCODE_INFO_STRU           *pstEncodeInfo,
    VOS_UINT32                          ulStateTblIndex
)
{
    /* 修改承载,修改的filter必须在同一个Sdf上*/
    if ((NAS_ESM_QUERY_CID_RSLT_ALL_FOUND_UNEQUAL_CID == enRslt)
     || (NAS_ESM_QUERY_CID_RSLT_PART_FOUND_UNEQUAL_CID == enRslt))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ValidateTftOpRsltProc:Mod filter not in Same Cid!");
        pstEncodeInfo->ucESMCau = NAS_ESM_CAUSE_SEMANTIC_ERR_IN_TFT;
        NAS_ESM_BearModFailProc(pstEncodeInfo, ulStateTblIndex);
        return NAS_ESM_MSG_HANDLED;
    }

    if (NAS_ESM_OP_TRUE != pstEsmNwMsgIE->bitOpTadInfo)
    {
        return NAS_ESM_SUCCESS;
    }

    switch(pstEsmNwMsgIE->stTadInfo.enTftOpType)
    {
        case NAS_ESM_TFT_OP_TYPE_CREATE_TFT:
            return NAS_ESM_ValidateTftOpCreateTftRsltProc(pstEsmNwMsgIE,
                                                          pstEncodeInfo,
                                                          enRslt,
                                                          ulStateTblIndex);
        case NAS_ESM_TFT_OP_TYPE_DELETE_TFT:
        case NAS_ESM_TFT_OP_TYPE_DELETE_FILTER:
            return NAS_ESM_ValidateTftOpDelTftOrDelPfRsltProc(pstEsmNwMsgIE,
                                                              pstEncodeInfo,
                                                              enRslt,
                                                              ulStateTblIndex);

        case NAS_ESM_TFT_OP_TYPE_NO_OP:
            return NAS_ESM_ValidateTftOpNoOpRsltProc(pstEsmNwMsgIE,
                                                     pstEncodeInfo,
                                                     enRslt,
                                                     ulStateTblIndex);
        default:
             break;
    }

    return NAS_ESM_SUCCESS;
}
VOS_UINT32 NAS_ESM_ValidateTftOpInModMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE,
    NAS_ESM_ENCODE_INFO_STRU           *pstEncodeInfo,
    VOS_UINT32                          ulStateTblIndex
)
{
    VOS_UINT32                          ulCId               = NAS_ESM_NULL;
    NAS_ESM_QUERY_CID_RSLT_ENUM_UINT8   enRslt              = NAS_ESM_QUERY_CID_RSLT_BUTT;

    enRslt = NAS_ESM_QuerySdfId(pstEsmNwMsgIE,&ulCId);


    return NAS_ESM_ValidateTftOpRsltProc(pstEsmNwMsgIE,
                                         enRslt,
                                         pstEncodeInfo,
                                         ulStateTblIndex);
}

VOS_VOID NAS_ESM_ProcEsmStatusCauseInvalidPtiOrNonExistMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE
)
{
    VOS_UINT32                          ulStateTblId        = NAS_ESM_NULL;

    if (pstEsmNwMsgIE->ucPti == NAS_ESM_PTI_UNASSIGNED_VALUE)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcEsmStatusCauseInvalidPtiOrNonExistMsg:WARN:PTI is unassigned!");
        return ;
    }

    /*根据PTI，找到动态表Index*/
    if(NAS_ESM_SUCCESS == NAS_ESM_QueryStateTblIndexByPti(  pstEsmNwMsgIE->ucPti,
                                                            &ulStateTblId))
    {
        switch(NAS_ESM_GetStateTblAppMsgType(ulStateTblId))
        {
            case ID_APP_ESM_PDP_SETUP_REQ:
                NAS_ESM_SndEsmAppSdfSetupCnfFailMsg(ulStateTblId,
                        NAS_ESM_GetAppErrorByEsmCause(pstEsmNwMsgIE->enEsmCau));
                break;
            case ID_APP_ESM_PDP_RELEASE_REQ:
                NAS_ESM_SndEsmAppSdfRelCnfFailMsg(  ulStateTblId,
                        NAS_ESM_GetAppErrorByEsmCause(pstEsmNwMsgIE->enEsmCau));
                break;
            case ID_APP_ESM_PDP_MODIFY_REQ:
                NAS_ESM_SndEsmAppSdfModCnfFailMsg(  ulStateTblId,
                        NAS_ESM_GetAppErrorByEsmCause(pstEsmNwMsgIE->enEsmCau));
                break;
            case ID_APP_ESM_NDISCONN_REQ:
                NAS_ESM_LOG1("NAS_ESM_ProcEsmStatusCauseInvalidPtiOrNonExistMsg:ERROR =", NAS_ESM_GetAppErrorByEsmCause(pstEsmNwMsgIE->enEsmCau));
                NAS_ESM_SndEsmAppNdisConnCnfFailMsg(ulStateTblId,
                        NAS_ESM_GetAppErrorByEsmCause(pstEsmNwMsgIE->enEsmCau));
                break;
            default:
                NAS_ESM_NORM_LOG("NAS_ESM_ProcEsmStatusCauseInvalidPtiOrNonExistMsg: Error: Illegal App Msg Type!");
                break;
        }

        /*释放状态表资源*/
        NAS_ESM_RelStateTblResource(ulStateTblId);
    }
}


VOS_VOID NAS_ESM_ProcEsmStatusCauseInvalidEpsbId
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE
)
{
    VOS_UINT32                          ulStateTblId        = NAS_ESM_NULL;

    /* 若PTI已存在于状态表，则先停止流程 */
    NAS_ESM_ProcEsmStatusCauseInvalidPtiOrNonExistMsg(pstEsmNwMsgIE);

    if((pstEsmNwMsgIE->ucEpsbId < NAS_ESM_MIN_EPSB_ID)
        ||(pstEsmNwMsgIE->ucEpsbId > NAS_ESM_MAX_EPSB_ID))
    {
        return ;
    }

    /*通知APP释放缺省承载及其关联承载*/
    NAS_ESM_DeactBearerAndInformApp(pstEsmNwMsgIE->ucEpsbId);

    /*通知EMM当前承载状态信息，发送ID_EMM_ESM_BEARER_STATUS_REQ*/
    NAS_ESM_SndEsmEmmBearerStatusReqMsg(EMM_ESM_BEARER_CNTXT_MOD_MUTUAL);

    /*根据ulEpsbId查找StatTbl表ID*/
    if(NAS_ESM_SUCCESS == NAS_ESM_QueryStateTblIndexByEpsbId(pstEsmNwMsgIE->ucEpsbId,&ulStateTblId))
    {
        /*释放状态表资源*/
        NAS_ESM_RelStateTblResource(ulStateTblId);
    }
}
VOS_VOID  NAS_ESM_ProcValidNwsMsgEsmStatus
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE
)
{
    NAS_ESM_NORM_LOG("NAS_ESM_ProcValidNwsMsgEsmStatus is entered!");

    /* 若不带ESM CAUSE信元，则直接返回 */
    if(NAS_ESM_OP_TRUE != pstEsmNwMsgIE->bitOpEsmCause)
    {
        return;
    }

    switch (pstEsmNwMsgIE->enEsmCau)
    {
        case NAS_ESM_CAUSE_MESSAGE_TYPE_NON_EXIST_OR_NOT_IMPLEMENTED:
        case NAS_ESM_CAUSE_INVALID_PTI_VALUE:
            NAS_ESM_ProcEsmStatusCauseInvalidPtiOrNonExistMsg(pstEsmNwMsgIE);
            break;
        case NAS_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY:
            NAS_ESM_ProcEsmStatusCauseInvalidEpsbId(pstEsmNwMsgIE);
            break;
        default:
            break;
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_ProcNwMsgDeactProcedureCollision
 Description     : 针对同一承载，网络发起的流程与UE发起的流程产生冲突，
                   本函数实现流程冲突处理功能，在状态机函数中不再处理冲突流程
 Input           : EMM_ESM_INTRA_DATA_IND_STRU *pRcvMsg
 Output          : None
 Return          : VOS_VOID

 History         :
    1.sunbing49683      2009-2-20  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_ProcNwMsgDeactProcedureCollision
(
    VOS_UINT32                          ulStateTblId,
    const NAS_ESM_NW_MSG_STRU           *pRcvMsg
)
{
    VOS_UINT32                          ulEpsbId            = NAS_ESM_NULL;
    VOS_UINT32                          ulSndNwMsgLen       = NAS_ESM_NULL;
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_ProcNwMsgDeactProcedureCollision is entered.");

    ulEpsbId = (VOS_UINT32)pRcvMsg->ucEpsbId;

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblId);

    /*如果当前承载处于PDN去连接过程中，则处理去激活流程，终止PDN去连接流程*/
    if(NAS_ESMCN_MSG_TYPE_PDN_DISCONNECT_REQ == \
                                    pstStateAddr->stNwMsgRecord.enEsmCnMsgType)

    {
        /*通知APP释放缺省承载及其关联承载*/
        NAS_ESM_DeactBearerAndInformApp(ulEpsbId);

        /*通知EMM当前承载状态信息，发送ID_EMM_ESM_BEARER_STATUS_REQ*/
        NAS_ESM_SndEsmEmmBearerStatusReqMsg(EMM_ESM_BEARER_CNTXT_MOD_MUTUAL);

        /*释放状态表资源*/
        NAS_ESM_RelStateTblResource(ulStateTblId);
    }
    /*如果当前承载处于承载修改请求过程中(专有承载释放)，则处理去激活流程，终止承载资源修改流程*/
    else if(NAS_ESMCN_MSG_TYPE_BEARER_RES_MOD_REQ == \
                                pstStateAddr->stNwMsgRecord.enEsmCnMsgType)
    {
        /*通知APP释放缺省承载及其关联承载*/
        NAS_ESM_DeactBearerAndInformApp(ulEpsbId);

        /*通知EMM当前承载状态信息，发送ID_EMM_ESM_BEARER_STATUS_REQ*/
        NAS_ESM_SndEsmEmmBearerStatusReqMsg(EMM_ESM_BEARER_CNTXT_MOD_MUTUAL);

        /*释放状态表资源*/
        NAS_ESM_RelStateTblResource(ulStateTblId);
    }
    else
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgDeactProcedureCollision:WARNNING:Undefine Collision Process!");
        return;
    }

    /*初始化，并填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.bitOpESMCau = NAS_ESM_OP_FALSE;
    stEncodeInfo.ucPti       = pRcvMsg->ucPti;
    stEncodeInfo.ulEpsbId    = ulEpsbId;

    /*向网侧回复Deactivate EPS bearer context accept*/
    NAS_ESM_EncodeDeactEpsbCntxtAccMsg(stEncodeInfo, \
                               NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                              &ulSndNwMsgLen);
    /*空口消息上报*/
    NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                       ulSndNwMsgLen, NAS_ESM_AIR_MSG_DIR_ENUM_UP,\
                                       ESM_DEACT_EPS_BEARER_CNTXT_ACP);
    NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID, PS_FALSE, ulSndNwMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
    NAS_ESM_UpdateEsmSndNwMsgLen(ulSndNwMsgLen);

}

/*****************************************************************************
 Function Name   : NAS_ESM_SndRejAirMsg
 Description     : 发送各种REJ空口消息
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-4-12  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_SndRejAirMsg
(
    NAS_ESMCN_MSG_TYPE_ENUM_UINT8       enEsmCnMsgType,
    const NAS_ESM_ENCODE_INFO_STRU     *pstEncodeInfo
)
{
    VOS_UINT32                          ulSndNwMsgLen       = NAS_ESM_NULL;

    switch(enEsmCnMsgType)
    {
        case NAS_ESMCN_MSG_TYPE_ACT_DEDICTD_EPS_BEARER_CNTXT_REQ:
            NAS_ESM_EncodeActDedictdEpsbCntxtRejMsg(    *pstEncodeInfo,\
                                                        NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                                        &ulSndNwMsgLen);
            NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),ulSndNwMsgLen,
                                       NAS_ESM_AIR_MSG_DIR_ENUM_UP,
                                       ESM_ACT_DEDICT_EPS_BEARER_CNTXT_REJ);
            break;

        case NAS_ESMCN_MSG_TYPE_ACT_DEFLT_EPS_BEARER_CNTXT_REQ:
            NAS_ESM_EncodeActDefltEpsbCntxtRejMsg(  *pstEncodeInfo,\
                                                    NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                                    &ulSndNwMsgLen);
            NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),ulSndNwMsgLen,
                                       NAS_ESM_AIR_MSG_DIR_ENUM_UP,
                                       ESM_ACT_DEFLT_EPS_BEARER_CNTXT_REJ);
            break;

        case NAS_ESMCN_MSG_TYPE_MOD_EPS_BEARER_CNTXT_REQ:
            NAS_ESM_EncodeModEpsbCntxtRejMsg(   *pstEncodeInfo, \
                                                NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                                &ulSndNwMsgLen);
            NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),ulSndNwMsgLen,
                                       NAS_ESM_AIR_MSG_DIR_ENUM_UP,
                                       ESM_MOD_EPS_BEARER_CNTXT_REJ);
            break;
        case NAS_ESMCN_MSG_TYPE_ESM_INFORMATION_REQ:
            NAS_ESM_EncodeEsmStatus(    *pstEncodeInfo, \
                                        NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                        &ulSndNwMsgLen);
            NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),ulSndNwMsgLen,
                                       NAS_ESM_AIR_MSG_DIR_ENUM_UP,
                                       ESM_STUS_MO);
            break;
        default:
            NAS_ESM_NORM_LOG("NAS_ESM_SndRejAirMsg:NORM: Ignore NW msg");
            return;
    }

    NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILL_CID, PS_FALSE, ulSndNwMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
    NAS_ESM_UpdateEsmSndNwMsgLen(ulSndNwMsgLen);
}

/*sunbing 49683 2013-10-14 VoLTE begin*/
/*****************************************************************************
 Function Name   : NAS_ESM_SndApsEsmNotificationIndMsg
 Description     : 向APS发送ESM Notification ind消息
 Input           :
 Output          : None
 Return          : VOS_VOID

 History         :
    1.sunbing 49683      2013-10-14  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_SndApsEsmNotificationIndMsg
(
    ESM_APS_NOTIFICATION_IND_ENUM_UINT32   enNotificationIndValue
)
{
    ESM_APS_NOTIFICATION_IND_STRU    *pstNotificationInd  = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    pstNotificationInd = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(ESM_APS_NOTIFICATION_IND_STRU));

    if ( VOS_NULL_PTR == pstNotificationInd )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndApsEsmNotificationIndMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(NAS_ESM_GET_MSG_ENTITY(pstNotificationInd), 0, NAS_ESM_GET_MSG_LENGTH(pstNotificationInd));

    pstNotificationInd->enNotificationIndicator = enNotificationIndValue;

    /*填写消息头*/
    NAS_ESM_WRITE_SM_MSG_HEAD(pstNotificationInd, ID_ESM_APS_ESM_NOTIFICATION_IND);

    /* 调用消息发送函数 */
    NAS_ESM_SND_MSG(pstNotificationInd);
}
/*sunbing 49683 2013-10-14 VoLTE begin*/


/*****************************************************************************
 Function Name   : NAS_ESM_ProcNwMsgForIllegalPti
 Description     : 由于网络携带的PTI不合法，UE根据网络消息内容发拒绝消息或直
                   接忽略
 Input           : NAS_ESM_NW_MSG_STRU stEsmNwMsgIE
 Output          : None
 Return          : VOS_VOID

 History         :
    1.sunbing49683      2009-2-19  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_ProcNwMsgForIllegalPti
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE,
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause
)
{
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;
    VOS_UINT32                          ulEsmBuffIndex      = NAS_ESM_NULL;
    VOS_UINT8                           ucPti               = pstEsmNwMsgIE->ucPti;
    VOS_UINT32                          ulMsgId             = NAS_ESM_NULL;
    NAS_ESM_PTI_BUFF_ITEM_STRU         *pstPtiBuffItem      = VOS_NULL_PTR;

    /*初始化，并填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.bitOpESMCau = NAS_ESM_OP_TRUE;
    stEncodeInfo.ucESMCau    = enEsmCause;
    stEncodeInfo.ucPti       = pstEsmNwMsgIE->ucPti;
    stEncodeInfo.ulEpsbId    = (VOS_UINT32)pstEsmNwMsgIE->ucEpsbId;

    if (enEsmCause != NAS_ESM_CAUSE_PTI_MISMATCH)
    {
        /*返回REJ消息*/
        NAS_ESM_SndRejAirMsg(pstEsmNwMsgIE->enEsmCnMsgType, &stEncodeInfo);

        return ;
    }

    /* 根据PTI查找ESM缓存记录 */
    if (NAS_ESM_SUCCESS != NAS_ESM_QueryPtiBuffItemByPti(   ucPti,
                                                            &ulEsmBuffIndex))
    {
        NAS_ESM_LOG1("NAS_ESM_ProcNwMsgForIllegalPti:WARNING:PTI not matched!",ucPti);
    
        /*返回REJ消息*/
        NAS_ESM_SndRejAirMsg(pstEsmNwMsgIE->enEsmCnMsgType, &stEncodeInfo);

        return ;
    }

    NAS_ESM_INFO_LOG2("NAS_ESM_ProcNwMsgForIllegalPti:NORM:PTI is found in Esm Buffer!",
                 ucPti, ulEsmBuffIndex);

    /* 获取缓存记录 */
    pstPtiBuffItem = (NAS_ESM_PTI_BUFF_ITEM_STRU*)
                            NAS_ESM_GetBuffItemAddr(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulEsmBuffIndex);

    if (pstPtiBuffItem == VOS_NULL_PTR)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgForIllegalPti:WARN:Get buffer item failed!");
        return ;
    }

    /* 原来的消息检测失败，且原因值为NAS_ESM_CAUSE_ERROR时，所以没有回复消息 */
    if (pstPtiBuffItem->stEsmMsg.ulEsmMsgSize == NAS_ESM_NULL)
    {
        return ;
    }

    /* 获取消息ID */
    ulMsgId = pstPtiBuffItem->stEsmMsg.aucEsmMsg[NAS_ESM_MSGID_OFFSET];

    /* 如果当前状态为ATTACHING态，且回复消息为缺省承载激活成功消息，则给
       MM发送EMM_ESM_PDN_CON_SUCC_REQ消息，否则发送EMM_ESM_DATA_REQ消息*/
    if ((ulMsgId == NAS_ESMCN_MSG_TYPE_ACT_DEFLT_EPS_BEARER_CNTXT_ACC)
        && (NAS_ESM_PS_REGISTER_STATUS_ATTACHING == NAS_ESM_GetEmmStatus()))
    {
        NAS_ESM_SndEsmEmmPdnConSuccReqMsg(pstPtiBuffItem->stEsmMsg.ulEsmMsgSize,
                                          pstPtiBuffItem->stEsmMsg.aucEsmMsg);
    }
    else
    {
        NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID,
                                    PS_FALSE,
                                    pstPtiBuffItem->stEsmMsg.ulEsmMsgSize,
                                    pstPtiBuffItem->stEsmMsg.aucEsmMsg);
    }
}


VOS_VOID  NAS_ESM_ProcNwMsgForIllegalEpsbId
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE
)
{
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;
    VOS_UINT32                          ulSndNwMsgLen = NAS_ESM_NULL;

    /*初始化，并填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.bitOpESMCau = NAS_ESM_OP_TRUE;
    stEncodeInfo.ucPti       = pstEsmNwMsgIE->ucPti;
    stEncodeInfo.ulEpsbId    = (VOS_UINT32)pstEsmNwMsgIE->ucEpsbId;
    stEncodeInfo.ucESMCau    = NAS_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY;

    if (pstEsmNwMsgIE->enEsmCnMsgType == NAS_ESMCN_MSG_TYPE_DEACT_EPS_BEARER_CNTXT_REQ)
    {
        /*向网侧回复Deact EPS bearer context accept*/
        NAS_ESM_EncodeDeactEpsbCntxtAccMsg(stEncodeInfo, \
                                 NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                &ulSndNwMsgLen);
        NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                   ulSndNwMsgLen, NAS_ESM_AIR_MSG_DIR_ENUM_UP,\
                                   ESM_DEACT_EPS_BEARER_CNTXT_ACP);
        NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID, PS_FALSE, ulSndNwMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
        NAS_ESM_UpdateEsmSndNwMsgLen(ulSndNwMsgLen);
    }
    else
    {
        /*给网侧发拒绝消息*/
        NAS_ESM_SndRejAirMsg(pstEsmNwMsgIE->enEsmCnMsgType, &stEncodeInfo);
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_ProcNwMsgForDecodeError
 Description     : 空口消息译码错误处理
 Input           : NAS_ESM_NW_MSG_STRU *pstMsgIE
                   NAS_ESM_CAUSE_ENUM_UINT8  enEsmCause
 Output          : None
 Return          : VOS_VOID

 History         :
    1.sunbing49683      2009-2-24  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_ProcNwMsgForDecodeError(const NAS_ESM_NW_MSG_STRU *pstEsmNwMsgIE,
                                                     NAS_ESM_CAUSE_ENUM_UINT8  enEsmCause)
{
    VOS_UINT32                          ulSndNwMsgLen       = NAS_ESM_NULL;
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;

    NAS_ESM_INFO_LOG("NAS_ESM_ProcNwMsgForDecodeError is entered.");

    /* 若原因值为消息太短，则丢弃此消息 */
    if (NAS_ESM_CAUSE_MSG_TOO_SHORT == enEsmCause)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgForDecodeError:Msg too short!");
        return ;
    }

    /*初始化NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));

    /*填充NAS_ESM_ENCODE_INFO_STRU*/
    stEncodeInfo.bitOpESMCau = NAS_ESM_OP_TRUE;
    stEncodeInfo.ulEpsbId    = (VOS_UINT32)pstEsmNwMsgIE->ucEpsbId;
    stEncodeInfo.ucPti       = pstEsmNwMsgIE->ucPti;
    stEncodeInfo.ucESMCau    = enEsmCause;

    /*根据接收到的空口消息类型处理*/
    switch(pstEsmNwMsgIE->enEsmCnMsgType)
    {
        /*如果网侧消息类型为ACT_DEFLT_EPS_BEARER_CNTXT_REQ*/
        case NAS_ESMCN_MSG_TYPE_ACT_DEFLT_EPS_BEARER_CNTXT_REQ:
            /*回复拒绝*/
            NAS_ESM_EncodeActDefltEpsbCntxtRejMsg(stEncodeInfo,\
                                         NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                        &ulSndNwMsgLen);

            NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),ulSndNwMsgLen,\
                                   NAS_ESM_AIR_MSG_DIR_ENUM_UP,\
                                   ESM_ACT_DEFLT_EPS_BEARER_CNTXT_REJ);

            break;

        /*如果网侧消息类型为ACT_DEDICTD_EPS_BEARER_CNTXT_REQ*/
        case NAS_ESMCN_MSG_TYPE_ACT_DEDICTD_EPS_BEARER_CNTXT_REQ:
            /*回复拒绝*/
            NAS_ESM_EncodeActDedictdEpsbCntxtRejMsg(stEncodeInfo,\
                                           NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                          &ulSndNwMsgLen);

            NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),ulSndNwMsgLen,\
                                   NAS_ESM_AIR_MSG_DIR_ENUM_UP,\
                                   ESM_ACT_DEDICT_EPS_BEARER_CNTXT_REJ);

            break;

        /*如果网侧消息类型为MOD_EPS_BEARER_CNTXT_REQ*/
        case NAS_ESMCN_MSG_TYPE_MOD_EPS_BEARER_CNTXT_REQ:
            /*回复拒绝*/
            NAS_ESM_EncodeModEpsbCntxtRejMsg(stEncodeInfo,\
                                    NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                   &ulSndNwMsgLen);

            NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),ulSndNwMsgLen,\
                                   NAS_ESM_AIR_MSG_DIR_ENUM_UP,\
                                   ESM_MOD_EPS_BEARER_CNTXT_REJ);

            break;

        default:
            /*网络消息解码错误，返回ESM STATUS*/
            NAS_ESM_EncodeEsmStatus(stEncodeInfo,\
                                    NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                   &ulSndNwMsgLen);

            NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),ulSndNwMsgLen,\
                                   NAS_ESM_AIR_MSG_DIR_ENUM_UP,\
                                   ESM_STUS_MO);


            /*打印异常信息*/
            NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmDataInd:WARNING: NW Message Protocol Error!");
            break;
    }

    NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID, PS_FALSE, ulSndNwMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
    NAS_ESM_UpdateEsmSndNwMsgLen(ulSndNwMsgLen);
}


VOS_VOID  NAS_ESM_ProcValidNwMsgEsmInfoReq
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE
)
{
    VOS_UINT32                          ulSndNwMsgLen       = NAS_ESM_NULL;
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;

    NAS_ESM_INFO_LOG("NAS_ESM_ProcValidNwMsgEsmInfoReq is entered.");

    /*填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.ucPti = pstEsmNwMsgIE->ucPti;/*bsun 应该根据PDN连接请求中的PTI处理*/

    /*编码ESM INFORMATION RESPONSE消息*/
    if(NAS_ESM_FAILURE == NAS_ESM_EncodeEsmInformationRspMsg(stEncodeInfo, \
                                  NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                 &ulSndNwMsgLen))
    {
        /*打印错误消息，ESM INFORMATION RESPONSE消息编码错误*/
        NAS_ESM_WARN_LOG("NAS_ESM_ProcValidNwMsgEsmInfoReq:WARNING:EncodeEsmInformationRsp fail!");
        return;
    }

    /*空口消息上报*/
    NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                               ulSndNwMsgLen, NAS_ESM_AIR_MSG_DIR_ENUM_UP,\
                               ESM_INFO_RSP);

    /*发送ESM INFORMATION RESPONSE消息*/
    NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID, PS_FALSE, ulSndNwMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
    NAS_ESM_UpdateEsmSndNwMsgLen(ulSndNwMsgLen);
}


/*****************************************************************************
 Function Name   : NAS_ESM_ValidatePtiInActDefltOrInfoOrRejMsg
 Description     : 对缺省承载激活请求或ESM INFO或REJ消息中的PTI进行合法性检测
 Input           : ulPti-------------------------PTI
 Output          : None
 Return          : NAS_ESM_CAUSE_ENUM_UINT8

 History         :
    1.lihong00150010      2010-5-13  Draft Enact

*****************************************************************************/
NAS_ESM_CAUSE_ENUM_UINT8  NAS_ESM_ValidatePtiInActDefltOrInfoOrRejMsg
(
    VOS_UINT8                           ucPti
)
{
    VOS_UINT32                          ulStateTblId        = NAS_ESM_NULL;

    NAS_ESM_LOG1("NAS_ESM_ValidatePtiInActDefltOrInfoOrRejMsg, Pti : ", ucPti);

    if ((ucPti == NAS_ESM_PTI_UNASSIGNED_VALUE)
         || (ucPti == NAS_ESM_PTI_RESERVED_VALUE))
    {
        return NAS_ESM_CAUSE_INVALID_PTI_VALUE;
    }

    /* 根据PTI查找到动态表Index */
    if(NAS_ESM_SUCCESS != NAS_ESM_QueryStateTblIndexByPti(ucPti, &ulStateTblId))
    {
        return NAS_ESM_CAUSE_PTI_MISMATCH;
    }

    return NAS_ESM_CAUSE_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidatePtiInActDedictOrModOrDeactOrStatusMsg
 Description     : 对专有承载激活请求或承载修改或承载去激活或ESM STATUS消息
                   中的PTI进行合法性检测
 Input           : ulPti-------------------------PTI
 Output          : None
 Return          : NAS_ESM_CAUSE_ENUM_UINT8

 History         :
    1.lihong00150010      2010-5-13  Draft Enact

*****************************************************************************/
NAS_ESM_CAUSE_ENUM_UINT8  NAS_ESM_ValidatePtiInActDedictOrModOrDeactOrStatusMsg
(
    VOS_UINT32                          ulPti,
    VOS_UINT32                         *pulStateTblId
)
{
    VOS_UINT32                          ulStateTblId        = NAS_ESM_NULL;

    *pulStateTblId = NAS_ESM_ILL_STATETAL_INDEX;

    if (ulPti == NAS_ESM_PTI_RESERVED_VALUE)
    {
        return NAS_ESM_CAUSE_INVALID_PTI_VALUE;
    }

    if (ulPti == NAS_ESM_PTI_UNASSIGNED_VALUE)
    {
        return NAS_ESM_CAUSE_SUCCESS;
    }

    /* 根据PTI查找到动态表Index */
    if(NAS_ESM_SUCCESS != NAS_ESM_QueryStateTblIndexByPti((VOS_UINT8)ulPti, &ulStateTblId))
    {
        return NAS_ESM_CAUSE_PTI_MISMATCH;
    }

    *pulStateTblId = ulStateTblId;
    return NAS_ESM_CAUSE_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateEpsbIdInActMsg
 Description     : 对缺省承载激活请求或专有承载激活请求消息
                   中的承载号进行合法性检测
 Input           : ulEpsbId-------------------------承载号
 Output          : None
 Return          : NAS_ESM_CAUSE_ENUM_UINT8

 History         :
    1.lihong00150010      2010-5-14  Draft Enact

*****************************************************************************/
NAS_ESM_CAUSE_ENUM_UINT8  NAS_ESM_ValidateEpsbIdInActMsg
(
    VOS_UINT8                           ucEpsbId
)
{
    if((ucEpsbId < NAS_ESM_MIN_EPSB_ID)||(ucEpsbId > NAS_ESM_MAX_EPSB_ID))
    {
        return NAS_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY;
    }

    return NAS_ESM_CAUSE_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateEpsbIdInRejOrInfoMsg
 Description     : 对网侧拒绝或ESM INFO消息中的承载号进行合法性检测
 Input           : ulEpsbId-------------------------承载号
 Output          : None
 Return          : NAS_ESM_CAUSE_ENUM_UINT8

 History         :
    1.lihong00150010      2010-5-14  Draft Enact

*****************************************************************************/
NAS_ESM_CAUSE_ENUM_UINT8  NAS_ESM_ValidateEpsbIdInRejOrInfoMsg
(
    VOS_UINT8                           ucEpsbId
)
{
    if(ucEpsbId != NAS_ESM_UNASSIGNED_EPSB_ID)
    {
        return NAS_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY;
    }

    return NAS_ESM_CAUSE_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateEpsbIdInModOrDeactMsg
 Description     : 对承载修改或承载去激活消息中的承载号进行合法性检测
 Input           : ulEpsbId-------------------------承载号
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-5-14  Draft Enact

*****************************************************************************/
NAS_ESM_CAUSE_ENUM_UINT8  NAS_ESM_ValidateEpsbIdInModOrDeactMsg
(
    VOS_UINT8                           ucEpsbId
)
{
    if(NAS_ESM_BEARER_STATE_ACTIVE != NAS_ESM_GetBearCntxtState(ucEpsbId))
    {
        return NAS_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY;
    }

    return NAS_ESM_CAUSE_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateLinkedEpsbId
 Description     : 对关联承载号进行合法性检测
 Input           : ucLinkedEpsbId-------------------------关联承载号
 Output          : None
 Return          : NAS_ESM_CAUSE_ENUM_UINT8

 History         :
    1.lihong00150010      2010-5-14  Draft Enact

*****************************************************************************/
NAS_ESM_CAUSE_ENUM_UINT8  NAS_ESM_ValidateLinkedEpsbId
(
    VOS_UINT8                           ucLinkedEpsbId
)
{
    VOS_UINT32                          ulStateTblIndex  = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr     = VOS_NULL_PTR;

    if((NAS_ESM_BEARER_STATE_ACTIVE != NAS_ESM_GetBearCntxtState(ucLinkedEpsbId))
        || (PS_FALSE == NAS_ESM_IsDefaultEpsBearerType(NAS_ESM_GetBearCntxtType(ucLinkedEpsbId))))
    {
        return NAS_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY;
    }

    /*Linked EPS Bearer正处于PDN DISCONNECT过程中，直接返回ERROR，此条消息不处理*/
    if(NAS_ESM_SUCCESS == NAS_ESM_QueryStateTblIndexByEpsbId(ucLinkedEpsbId,&ulStateTblIndex))
    {
        pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);
        if(NAS_ESMCN_MSG_TYPE_PDN_DISCONNECT_REQ == pstStateAddr->stNwMsgRecord.enEsmCnMsgType)
        {
            /*打印信息*/
            NAS_ESM_NORM_LOG("NAS_ESM_ValidateLinkedEpsbId:NORMAL:Link Bearer In Procedure of PDN DISCONNECT !");

            return NAS_ESM_CAUSE_ERROR;
        }
    }

    return NAS_ESM_CAUSE_SUCCESS;
}



VOS_VOID  NAS_ESM_CreatePfPVListInBearCreateTft
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg,
    VOS_UINT8                          *pucPfNum,
    NAS_ESM_PF_PRCDNC_VLDT_STRU        *pstPfPVList
)
{
    VOS_UINT32                          ulCnt           = NAS_ESM_NULL;
    const NAS_ESM_CONTEXT_TFT_STRU     *pstPacketFilter = VOS_NULL_PTR;

    for (ulCnt = NAS_ESM_NULL; ulCnt < pstNwMsg->stTadInfo.ucSdfPfNum; ulCnt++)
    {
        pstPacketFilter = &pstNwMsg->stTadInfo.astSdfPfInfo[ulCnt];
        pstPfPVList[*pucPfNum].ucEpsbId = pstNwMsg->ucEpsbId;
        pstPfPVList[*pucPfNum].ucPfId = pstPacketFilter->ucNwPacketFilterId;
        pstPfPVList[*pucPfNum].ucPrecedence = pstPacketFilter->ucPrecedence;
        (*pucPfNum)++;
    }
}


VOS_VOID  NAS_ESM_CreatePfPVListInBearAddOrReplaceFilter
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg,
    VOS_UINT8                          *pucPfNum,
    NAS_ESM_PF_PRCDNC_VLDT_STRU        *pstPfPVList
)
{
    VOS_UINT32                          ulCnt1          = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt2          = NAS_ESM_NULL;
    VOS_UINT8                           ucPfPVNumTmp    = NAS_ESM_NULL;
    VOS_UINT8                           ucPfPVNum       = NAS_ESM_NULL;
    NAS_ESM_PF_PRCDNC_VLDT_STRU         astPfPVListTmp[NAS_ESM_MAX_SDF_PF_NUM]= {0};
    const NAS_ESM_CONTEXT_TFT_STRU     *pstPacketFilter = VOS_NULL_PTR;

    /* 获取此承载下packet filter优先级验证列表 */
    NAS_ESM_GetPfPrecedenceValidateListInBearer(pstNwMsg->ucEpsbId,
                                                &ucPfPVNum,
                                                astPfPVListTmp);

    ucPfPVNumTmp = ucPfPVNum;

    for (ulCnt1 = NAS_ESM_NULL; ulCnt1 < pstNwMsg->stTadInfo.ucSdfPfNum; ulCnt1++)
    {
        pstPacketFilter = &pstNwMsg->stTadInfo.astSdfPfInfo[ulCnt1];

        for (ulCnt2 = NAS_ESM_NULL; ulCnt2 < ucPfPVNumTmp; ulCnt2++)
        {
            if (pstPacketFilter->ucNwPacketFilterId == astPfPVListTmp[ulCnt2].ucPfId)
            {
                astPfPVListTmp[ulCnt2].ucPrecedence = pstPacketFilter->ucPrecedence;
                break;
            }
        }

        if (ulCnt2 >= ucPfPVNumTmp)
        {
            astPfPVListTmp[ucPfPVNum].ucEpsbId = pstNwMsg->ucEpsbId;
            astPfPVListTmp[ucPfPVNum].ucPfId = pstPacketFilter->ucNwPacketFilterId;
            astPfPVListTmp[ucPfPVNum].ucPrecedence = pstPacketFilter->ucPrecedence;
            ucPfPVNum++;
        }
    }

    NAS_ESM_MEM_CPY(&pstPfPVList[*pucPfNum],
                    astPfPVListTmp,
                    ucPfPVNum * sizeof(NAS_ESM_PF_PRCDNC_VLDT_STRU));

    (*pucPfNum) += ucPfPVNum;

}
VOS_UINT32  NAS_ESM_CreatePfPrecedenceValidateListInPdn
(
    VOS_UINT32                          ulEpsbId,
    VOS_UINT8                          *pucPfNum,
    NAS_ESM_PF_PRCDNC_VLDT_STRU        *pstPfPVList
)
{
    VOS_UINT32                          ulCnt           = NAS_ESM_NULL;
    VOS_UINT8                           aucEpsbId[NAS_ESM_MAX_EPSB_NUM] = {0};
    VOS_UINT8                           ucEpsBearNum    = NAS_ESM_NULL;

    *pucPfNum = NAS_ESM_NULL;

    /* 获取某PDN下的所有承载号 */
    if (NAS_ESM_SUCCESS != NAS_ESM_GetAllEpsBearerInPdn((VOS_UINT8)ulEpsbId,
                                                        &ucEpsBearNum,
                                                        aucEpsbId))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_CreatePfPrecedenceValidateListInPdn:NAS_ESM_GetAllEpsBearerInPdn failed!");
        return NAS_ESM_FAILURE;
    }

    /* 获得同一PDN下的其他承载的packet filter优先级列表 */
    for (ulCnt= NAS_ESM_NULL; ulCnt < ucEpsBearNum; ulCnt++)
    {

        /* 获取此承载下packet filter优先级验证列表 */
        NAS_ESM_GetPfPrecedenceValidateListInBearer(aucEpsbId[ulCnt],
                                                    pucPfNum,
                                                    pstPfPVList);
    }

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateTftInActDedictMsg
 Description     : 对专有承载激活请求消息中的TFT进行合法性检测
 Input           : pstTft-------------------------TFT指针
 Output          : None
 Return          : NAS_ESM_CAUSE_ENUM_UINT8

 History         :
    1.lihong00150010      2010-5-14  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_ESM_ValidateTftInActDedictMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg,
    VOS_UINT32                          ulStateTblIndex
)
{
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo = { NAS_ESM_NULL };

    /*初始化，并填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.bitOpESMCau = NAS_ESM_OP_TRUE;
    stEncodeInfo.ucPti       = pstNwMsg->ucPti;
    stEncodeInfo.ulEpsbId    = (VOS_UINT32)pstNwMsg->ucEpsbId;

    if (NAS_ESM_TFT_OP_TYPE_CREATE_TFT != pstNwMsg->stTadInfo.enTftOpType)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ValidateTftInActDedictMsg:Tad op is not create new tft!");

        stEncodeInfo.ucESMCau = NAS_ESM_CAUSE_SEMANTIC_ERR_IN_TFT;
        NAS_ESM_ActDediBearFailProc(&stEncodeInfo, ulStateTblIndex);
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_CAUSE_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateTftInActDedictMsg
 Description     : 对承载修改请求消息中的TFT进行合法性检测
 Input           : pstNwMsg-------------------------译码后的空口消息指针
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-5-14  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_ESM_ValidateTftInModMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg,
    VOS_UINT32                          ulStateTblIndex
)
{
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;

    /*初始化，并填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.bitOpESMCau = NAS_ESM_OP_TRUE;
    stEncodeInfo.ucPti       = pstNwMsg->ucPti;
    stEncodeInfo.ulEpsbId    = (VOS_UINT32)pstNwMsg->ucEpsbId;

    if (NAS_ESM_SUCCESS != NAS_ESM_ValidateTftOpInModMsg(pstNwMsg,&stEncodeInfo,ulStateTblIndex))
    {
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}
NAS_ESM_COMPARE_RESULT_ENUM_UINT8 NAS_ESM_ComPareQosRate
(
    VOS_UINT32 ulData1,
    VOS_UINT32 ulData2
)
{
   if(ulData1 > ulData2)
   {
        return NAS_ESM_COMPARE_RESULT_BIG;
   }
   else if(ulData1 == ulData2)
   {
        return NAS_ESM_COMPARE_RESULT_EQUAL;
   }
   else
   {
        return NAS_ESM_COMPARE_RESULT_SMALL;
   }
}


VOS_UINT32 NAS_ESM_IsEpsQosEqual
(
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enUlMaxRateComRslt,
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enDlMaxRateComRslt,
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enULGMaxRateComRslt,
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enDLGMaxRateComRslt
)
{
    /* 如果都等，则返回PS_TRUE */
    if((NAS_ESM_COMPARE_RESULT_EQUAL == enUlMaxRateComRslt)
        && (NAS_ESM_COMPARE_RESULT_EQUAL == enDlMaxRateComRslt)
        && (NAS_ESM_COMPARE_RESULT_EQUAL == enULGMaxRateComRslt)
        && (NAS_ESM_COMPARE_RESULT_EQUAL == enDLGMaxRateComRslt))
    {
        return PS_TRUE;
    }

    return PS_FALSE;
}
VOS_UINT32 NAS_ESM_IsEpsQosBiggerOrEqual
(
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enUlMaxRateComRslt,
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enDlMaxRateComRslt,
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enULGMaxRateComRslt,
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enDLGMaxRateComRslt
)
{
    /* 如果都大或者等于，则返回PS_TRUE */
    if(((NAS_ESM_COMPARE_RESULT_BIG == enUlMaxRateComRslt) || (NAS_ESM_COMPARE_RESULT_EQUAL == enUlMaxRateComRslt))
        && ((NAS_ESM_COMPARE_RESULT_BIG == enDlMaxRateComRslt) || (NAS_ESM_COMPARE_RESULT_EQUAL == enDlMaxRateComRslt))
        && ((NAS_ESM_COMPARE_RESULT_BIG == enULGMaxRateComRslt) || (NAS_ESM_COMPARE_RESULT_EQUAL == enULGMaxRateComRslt))
        && ((NAS_ESM_COMPARE_RESULT_BIG == enDLGMaxRateComRslt) || (NAS_ESM_COMPARE_RESULT_EQUAL == enDLGMaxRateComRslt)))
    {
        return PS_TRUE;
    }

    return PS_FALSE;
}
VOS_UINT32 NAS_ESM_IsEpsQosSmallerOrEqual
(
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enUlMaxRateComRslt,
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enDlMaxRateComRslt,
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enULGMaxRateComRslt,
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enDLGMaxRateComRslt
)
{
    /* 如果都小，则返回PS_TRUE */
    if(((NAS_ESM_COMPARE_RESULT_SMALL == enUlMaxRateComRslt) || (NAS_ESM_COMPARE_RESULT_EQUAL == enUlMaxRateComRslt))
        && ((NAS_ESM_COMPARE_RESULT_SMALL == enDlMaxRateComRslt) || (NAS_ESM_COMPARE_RESULT_EQUAL == enDlMaxRateComRslt))
        && ((NAS_ESM_COMPARE_RESULT_SMALL == enULGMaxRateComRslt) || (NAS_ESM_COMPARE_RESULT_EQUAL == enULGMaxRateComRslt))
        && ((NAS_ESM_COMPARE_RESULT_SMALL == enDLGMaxRateComRslt) || (NAS_ESM_COMPARE_RESULT_EQUAL == enDLGMaxRateComRslt)))
    {
        return PS_TRUE;
    }

    return PS_FALSE;
}
NAS_ESM_COMPARE_RESULT_ENUM_UINT8 NAS_ESM_CompareEpsQos
(
    const APP_ESM_EPS_QOS_INFO_STRU                      *pstEpsQoSInfo1,
    const APP_ESM_EPS_QOS_INFO_STRU                      *pstEpsQoSInfo2
)
{
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enUlMaxRateComRslt;
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enDlMaxRateComRslt;
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enULGMaxRateComRslt;
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8   enDLGMaxRateComRslt;

    /* 比较UlMaxRate */
    enUlMaxRateComRslt = NAS_ESM_ComPareQosRate(pstEpsQoSInfo1->ulULMaxRate,
                                                pstEpsQoSInfo2->ulULMaxRate);

    /* 比较DlMaxRate */
    enDlMaxRateComRslt = NAS_ESM_ComPareQosRate(pstEpsQoSInfo1->ulDLMaxRate,
                                                pstEpsQoSInfo2->ulDLMaxRate);

    /* 比较UlGMaxRate */
    enULGMaxRateComRslt = NAS_ESM_ComPareQosRate(pstEpsQoSInfo1->ulULGMaxRate,
                                                pstEpsQoSInfo2->ulULGMaxRate);

    /* 比较DlGMaxRate */
    enDLGMaxRateComRslt = NAS_ESM_ComPareQosRate(pstEpsQoSInfo1->ulDLGMaxRate,
                                                pstEpsQoSInfo2->ulDLGMaxRate);

    /* 如果都等，则返回NAS_ESM_COMPARE_RESULT_EQUAL */
    if(PS_TRUE == NAS_ESM_IsEpsQosEqual(enUlMaxRateComRslt, enDlMaxRateComRslt,
                                        enULGMaxRateComRslt, enDLGMaxRateComRslt))
    {
        return NAS_ESM_COMPARE_RESULT_EQUAL;
    }

    /* 如果各项速率都是小于或者等于，则返回NAS_ESM_COMPARE_RESULT_SMALL */
    if(PS_TRUE == NAS_ESM_IsEpsQosSmallerOrEqual(enUlMaxRateComRslt, enDlMaxRateComRslt,
                                                 enULGMaxRateComRslt, enDLGMaxRateComRslt))
    {
        return NAS_ESM_COMPARE_RESULT_SMALL;
    }

    /* 如果各项速率都是大于或者等于，则返回NAS_ESM_COMPARE_RESULT_BIG */
    if(PS_TRUE == NAS_ESM_IsEpsQosBiggerOrEqual(enUlMaxRateComRslt, enDlMaxRateComRslt,
                                                enULGMaxRateComRslt, enDLGMaxRateComRslt))
    {
        return NAS_ESM_COMPARE_RESULT_BIG;
    }

    return NAS_ESM_COMPARE_RESULT_OTHER;
}
NAS_ESM_CAUSE_ENUM_UINT8 NAS_ESM_ValidateEpsQos
(
    NAS_ESM_TFT_OP_TYPE_ENUM_UINT8                           enTftOpType,
    const NAS_ESM_CONTEXT_LTE_QOS_STRU                      *pstEpsQoSInfoOld,
    const NAS_ESM_CONTEXT_LTE_QOS_STRU                      *pstEpsQoSInfoNew
)
{
    NAS_ESM_COMPARE_RESULT_ENUM_UINT8                   enQosCompareResult;

    enQosCompareResult = NAS_ESM_CompareEpsQos(&pstEpsQoSInfoOld->stQosInfo,
                                               &pstEpsQoSInfoNew->stQosInfo);

    if(NAS_ESM_TFT_OP_TYPE_ADD_FILTER == enTftOpType)
    {
        /* 若是添加SDF，则当前保存的QOS应小于或者等于收到消息中的QOS */
        if ((NAS_ESM_COMPARE_RESULT_SMALL == enQosCompareResult)
          ||(NAS_ESM_COMPARE_RESULT_EQUAL == enQosCompareResult))
        {
            return NAS_ESM_CAUSE_SUCCESS;
        }
        else
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ValidateEpsQos:WARNING:Qos is not increased !");
            return NAS_ESM_CAUSE_PROTOCOL_ERROR;
        }
    }
    else if (NAS_ESM_TFT_OP_TYPE_DELETE_FILTER == enTftOpType)
    {
        /* 若是删除SDF，则当前保存的QOS应大于或者等于收到消息中的QOS */
        if ((NAS_ESM_COMPARE_RESULT_BIG == enQosCompareResult)
          ||(NAS_ESM_COMPARE_RESULT_EQUAL == enQosCompareResult))
        {
            return NAS_ESM_CAUSE_SUCCESS;
        }
        else
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ValidateEpsQos:WARNING:Qos is not decreased !");
            return NAS_ESM_CAUSE_PROTOCOL_ERROR;
        }
    }
    else
    {
        return NAS_ESM_CAUSE_SUCCESS;
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateQosInModMsg
 Description     : 对承载修改请求中的QOS信息进行合法性检测
 Input           : pstMsgIE -- 译码后的网络消息
 Output          : None
 Return          : NAS_ESM_CAUSE_ENUM_UINT8

 History         :
    1.lihong00150010      2009-12-2  Draft Enact

*****************************************************************************/
NAS_ESM_CAUSE_ENUM_UINT8  NAS_ESM_ValidateQosInModMsg
(
    const NAS_ESM_NW_MSG_STRU               *pstMsgIE,
    const NAS_ESM_EPSB_CNTXT_INFO_STRU      *pstEpsbCntxtInfo
)
{
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause       = NAS_ESM_CAUSE_SUCCESS;

    /* 如果不带QOS */
    if (NAS_ESM_OP_FALSE == pstMsgIE->bitOpEpsQos)
    {
        /* 缺省承载可以不带Qos */
        if(PS_TRUE == NAS_ESM_IsDefaultEpsBearerType(NAS_ESM_GetBearCntxtType(pstMsgIE->ucEpsbId)))
        {
            NAS_ESM_INFO_LOG("NAS_ESM_ValidateQosInModMsg:INFO:Default Bearer No QoS Information !");
            return NAS_ESM_CAUSE_SUCCESS;
        }

        /* 当TAD码为NO OPERATION时，必须携带QOS；当承载为GBR承载，且TAD码为ADD SDF,
           则必须携带QOS */
        if (((NAS_ESM_OP_TRUE == pstMsgIE->bitOpTadInfo)
          && (NAS_ESM_TFT_OP_TYPE_NO_OP == pstMsgIE->stTadInfo.enTftOpType)))

        {
            NAS_ESM_WARN_LOG("NAS_ESM_ValidateQosInModMsg:WARNING:No QoS Information !");
            return NAS_ESM_CAUSE_PROTOCOL_ERROR;
        }
        else
        {
            return NAS_ESM_CAUSE_SUCCESS;
        }
    }

    /* 判断修改后的Qos是否合理 */
    if (PS_TRUE == NAS_ESM_IsGbrBearer(pstEpsbCntxtInfo->stEpsQoSInfo.stQosInfo.ucQCI))

    {
        enEsmCause = NAS_ESM_ValidateEpsQos(pstMsgIE->stTadInfo.enTftOpType,
                                            &pstEpsbCntxtInfo->stEpsQoSInfo,
                                            &pstMsgIE->stSdfQosInfo);
        if (NAS_ESM_CAUSE_SUCCESS != enEsmCause)
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ValidateQosInModMsg:WARNING: Qos invalid!");
            return enEsmCause;
        }
    }

    return NAS_ESM_CAUSE_SUCCESS;
}

#if 0
/*****************************************************************************
 Function Name   : NAS_ESM_ValidateTftInActDedictMsg
 Description     : 对承载修改请求消息中的APN-AMBR进行合法性检测
 Input           : pstNwMsg-------------------------译码后的空口消息指针
 Output          : None
 Return          : NAS_ESM_CAUSE_ENUM_UINT8

 History         :
    1.lihong00150010      2010-5-14  Draft Enact

*****************************************************************************/
NAS_ESM_CAUSE_ENUM_UINT8  NAS_ESM_ValidateApnAmbrInModMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg
)
{
    if( (NAS_ESM_OP_TRUE == pstNwMsg->bitOpApnAmbr)
        && (NAS_ESM_BEARER_TYPE_DEFAULT != NAS_ESM_GetBearCntxtType(pstNwMsg->ucEpsbId)))
    {
        /*打印信息*/
        NAS_ESM_NORM_LOG("NAS_ESM_ValidateApnAmbrInModMsg:NORMAL:Dedicated bearer can not have APN AMBR !");
        return NAS_ESM_CAUSE_PROTOCOL_ERROR;
    }

    return NAS_ESM_CAUSE_SUCCESS;
}
#endif

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateActDefltEpsbReqMsg
 Description     : 对激活缺省承载请求消息进行合法性检测
 Input           : pstNwMsg-------------------------译码后的空口消息指针
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-5-12  Draft Enact

*****************************************************************************/
static VOS_UINT32  NAS_ESM_ValidateActDefltEpsbReqMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg
)
{
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause = NAS_ESM_NULL;

    /* 检测PTI，PTI不为0，不为RESERVED，且通过此PTI能检索到状态表
       中的记录，否则回复拒绝消息 */
    enEsmCause = NAS_ESM_ValidatePtiInActDefltOrInfoOrRejMsg(pstNwMsg->ucPti);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateActDefltEpsbReqMsg:WARN:Validate Pti failed, cause : ", enEsmCause);
        NAS_ESM_ProcNwMsgForIllegalPti(pstNwMsg, enEsmCause);
        return NAS_ESM_MSG_HANDLED;
    }

    /* 检测承载号范围，必须位于5-15之间，否则回复拒绝消息 */
    enEsmCause = NAS_ESM_ValidateEpsbIdInActMsg(pstNwMsg->ucEpsbId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateActDefltEpsbReqMsg:WARN:Validate EpsbId failed, cause : ", enEsmCause);
        NAS_ESM_ProcNwMsgForIllegalEpsbId(pstNwMsg);
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateActDedictEpsbReqMsg
 Description     : 对激活专有承载请求消息进行合法性检测
 Input           : pstNwMsg-------------------------译码后的空口消息指针
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-5-12  Draft Enact

*****************************************************************************/
static VOS_UINT32  NAS_ESM_ValidateActDedictEpsbReqMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg
)
{
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause      = NAS_ESM_NULL;
    VOS_UINT32                          ulStateTblId    = NAS_ESM_NULL;
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;

    /* 检测PTI，PTI不为RESERVED，当PTI不为0时，通过此PTI能检索到状态表
       中的记录，否则回复拒绝消息*/
    enEsmCause = NAS_ESM_ValidatePtiInActDedictOrModOrDeactOrStatusMsg(pstNwMsg->ucPti,
                                                                       &ulStateTblId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateActDedictEpsbReqMsg:WARN:Validate Pti failed, cause : ", enEsmCause);
        NAS_ESM_ProcNwMsgForIllegalPti(pstNwMsg, enEsmCause);
        return NAS_ESM_MSG_HANDLED;
    }

    /* 检测承载号范围，必须位于5-15之间，否则回复拒绝消息 */
    enEsmCause = NAS_ESM_ValidateEpsbIdInActMsg(pstNwMsg->ucEpsbId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateActDedictEpsbReqMsg:WARN:Validate EpsbId failed, cause : ", enEsmCause);
        NAS_ESM_ProcNwMsgForIllegalEpsbId(pstNwMsg);
        return NAS_ESM_MSG_HANDLED;
    }

    /*初始化，并填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.bitOpESMCau = NAS_ESM_OP_TRUE;
    stEncodeInfo.ucPti       = pstNwMsg->ucPti;
    stEncodeInfo.ulEpsbId    = (VOS_UINT32)pstNwMsg->ucEpsbId;

    /* 检测关联承载号范围，必须位于5-15之间，必须为激活的缺省承载，否则
       回复拒绝消息；若此缺省承载处于PDN DISCONNECT流程中，则丢弃此消息 */
    enEsmCause = NAS_ESM_ValidateLinkedEpsbId(pstNwMsg->ucLinkedEpsbId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateActDedictEpsbReqMsg:WARN:Validate Linked EpsbId failed, cause : ", enEsmCause);
        stEncodeInfo.ucESMCau = enEsmCause;
        NAS_ESM_ActDediBearFailProc(&stEncodeInfo, ulStateTblId);
        return NAS_ESM_MSG_HANDLED;
    }

    /* 检测TFT */
    if (NAS_ESM_SUCCESS != NAS_ESM_ValidateTftInActDedictMsg(pstNwMsg,
                                                             ulStateTblId))
    {
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateModEpsbReqMsg
 Description     : 对承载修改请求消息进行合法性检测
 Input           : pstNwMsg-------------------------译码后的空口消息指针
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-5-12  Draft Enact

*****************************************************************************/
static VOS_UINT32  NAS_ESM_ValidateModEpsbReqMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg
)
{
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause          = NAS_ESM_NULL;
    VOS_UINT32                          ulStateTblId        = NAS_ESM_NULL;
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstTmpEpsbCntxtInfo = VOS_NULL_PTR;


    /* 检测PTI，PTI不为RESERVED，当PTI不为0时，通过此PTI能检索到状态表
          中的记录，否则回复拒绝消息*/
    enEsmCause = NAS_ESM_ValidatePtiInActDedictOrModOrDeactOrStatusMsg(pstNwMsg->ucPti,
                                                                       &ulStateTblId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateModEpsbReqMsg:WARN:Validate Pti failed, cause : ", enEsmCause);
        NAS_ESM_ProcNwMsgForIllegalPti(pstNwMsg, enEsmCause);
        return NAS_ESM_MSG_HANDLED;
    }

    /* 检测承载号范围，必须位于5-15之间，必须为激活态，否则回复拒绝消息 */
    enEsmCause = NAS_ESM_ValidateEpsbIdInModOrDeactMsg(pstNwMsg->ucEpsbId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateModEpsbReqMsg:WARN:Validate EpsbId failed, cause : ", enEsmCause);
        NAS_ESM_ProcNwMsgForIllegalEpsbId(pstNwMsg);
        return NAS_ESM_MSG_HANDLED;
    }

    /*初始化，并填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.bitOpESMCau = NAS_ESM_OP_TRUE;
    stEncodeInfo.ucPti       = pstNwMsg->ucPti;
    stEncodeInfo.ulEpsbId    = (VOS_UINT32)pstNwMsg->ucEpsbId;

    /* 检测此消息是否与PDN DISCONNECT流程冲突，若是则丢弃此消息 */
    pstTmpEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(pstNwMsg->ucEpsbId);
    if(NAS_ESM_BEARER_TYPE_DEDICATED == pstTmpEpsbCntxtInfo->enBearerCntxtType)
    {
        enEsmCause = NAS_ESM_ValidateLinkedEpsbId((VOS_UINT8)pstTmpEpsbCntxtInfo->ulLinkedEpsbId);
    }
    else
    {
        enEsmCause = NAS_ESM_ValidateLinkedEpsbId(pstNwMsg->ucEpsbId);
    }

    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateActDedictEpsbReqMsg:WARN:Validate Linked EpsbId failed, cause : ", enEsmCause);
        stEncodeInfo.ucESMCau = enEsmCause;
        NAS_ESM_BearModFailProc(&stEncodeInfo, ulStateTblId);
        return NAS_ESM_MSG_HANDLED;
    }

    /* 检测TFT */
    if (NAS_ESM_SUCCESS != NAS_ESM_ValidateTftInModMsg(pstNwMsg, ulStateTblId))
    {
        return NAS_ESM_MSG_HANDLED;
    }

    /* 检测QOS */
    enEsmCause = NAS_ESM_ValidateQosInModMsg(   pstNwMsg,
                                                NAS_ESM_GetEpsbCntxtInfoAddr(pstNwMsg->ucEpsbId));
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateActDedictEpsbReqMsg:WARN:NAS_ESM_ValidateQosInModMsg failed! ", enEsmCause);
        stEncodeInfo.ucESMCau = enEsmCause;
        NAS_ESM_BearModFailProc(&stEncodeInfo, ulStateTblId);
        return NAS_ESM_MSG_HANDLED;
    }

    #if 0
    /* 检测APN-AMBR */
    enEsmCause = NAS_ESM_ValidateApnAmbrInModMsg(pstNwMsg);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateActDedictEpsbReqMsg:WARN:Validate Linked EpsbId failed, cause : ", enEsmCause);
        stEncodeInfo.ucESMCau = enEsmCause;
        NAS_ESM_BearModFailProc(&stEncodeInfo, ulStateTblId);
        return NAS_ESM_MSG_HANDLED;
    }
    #endif

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateDeactEpsbReqMsg
 Description     : 对承载去激活请求消息进行合法性检测
 Input           : pstNwMsg-------------------------译码后的空口消息指针
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-5-12  Draft Enact

*****************************************************************************/
static VOS_UINT32  NAS_ESM_ValidateDeactEpsbReqMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg
)
{
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause      = NAS_ESM_NULL;
    VOS_UINT32                          ulStateTblId    = NAS_ESM_NULL;

    /* 检测PTI，当PTI不为0时，且通过此PTI不能检索到状态表记录，或者若PTI为
       RESERVED，则丢弃此消息*/
    enEsmCause = NAS_ESM_ValidatePtiInActDedictOrModOrDeactOrStatusMsg( pstNwMsg->ucPti,
                                                                        &ulStateTblId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateDeactEpsbReqMsg:WARN:Validate Pti failed, cause : ", enEsmCause);
        return NAS_ESM_MSG_HANDLED;
    }

    /* 检测承载号范围，必须位于5-15之间，必须为激活态，否则直接回复ACCEPT消息 */
    enEsmCause = NAS_ESM_ValidateEpsbIdInModOrDeactMsg(pstNwMsg->ucEpsbId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateDeactEpsbReqMsg:WARN:Validate EpsbId failed, cause : ", enEsmCause);
        NAS_ESM_ProcNwMsgForIllegalEpsbId(pstNwMsg);
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateNwRejMsg
 Description     : 对网侧拒绝消息进行合法性检测
 Input           : pstNwMsg-------------------------译码后的空口消息指针
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-5-12  Draft Enact

*****************************************************************************/
static VOS_UINT32  NAS_ESM_ValidateNwRejMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg
)
{
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause = NAS_ESM_NULL;

    /* 检测PTI，PTI不为0，不为RESERVED，通过此PTI能检索到状态表
       中的记录，否则丢弃此消息 */
    enEsmCause = NAS_ESM_ValidatePtiInActDefltOrInfoOrRejMsg(pstNwMsg->ucPti);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateNwRejMsg:WARN:Validate Pti failed, cause : ", enEsmCause);
        return NAS_ESM_MSG_HANDLED;
    }

    /* 检测承载号，必须为0，否则丢弃此消息 */
    enEsmCause = NAS_ESM_ValidateEpsbIdInRejOrInfoMsg(pstNwMsg->ucEpsbId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateNwRejMsg:WARN:Validate EpsbId failed, cause : ", enEsmCause);
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateEsmInfoReqMsg
 Description     : 对ESM INFO请求消息进行合法性检测
 Input           : pstNwMsg-------------------------译码后的空口消息指针
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-5-12  Draft Enact

*****************************************************************************/
static VOS_UINT32  NAS_ESM_ValidateEsmInfoReqMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg
)
{
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause = NAS_ESM_NULL;

    /* 检测PTI，PTI若为0，或者为RESERVED，则直接丢弃此消息；若通过此PTI不能
       检索到状态表中的记录，则回复ESM STATUS消息 */
    enEsmCause = NAS_ESM_ValidatePtiInActDefltOrInfoOrRejMsg(pstNwMsg->ucPti);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateEsmInfoReqMsg:WARN:Validate Pti failed, cause : ", enEsmCause);
        if (enEsmCause == NAS_ESM_CAUSE_PTI_MISMATCH)
        {
            NAS_ESM_ProcNwMsgForIllegalPti(pstNwMsg, enEsmCause);
        }
        return NAS_ESM_MSG_HANDLED;
    }

    /* 检测承载号，必须为0，否则回复ESM STATUS消息 */
    enEsmCause = NAS_ESM_ValidateEpsbIdInRejOrInfoMsg(pstNwMsg->ucEpsbId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateEsmInfoReqMsg:WARN:Validate EpsbId failed, cause : ", enEsmCause);
        NAS_ESM_ProcNwMsgForIllegalEpsbId(pstNwMsg);
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ValidateEsmStatusMsg
 Description     : 对ESM STATUS消息进行合法性检测
 Input           : pstNwMsg-------------------------译码后的空口消息指针
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-5-12  Draft Enact

*****************************************************************************/
static VOS_UINT32  NAS_ESM_ValidateEsmStatusMsg
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg
)
{
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause      = NAS_ESM_NULL;
    VOS_UINT32                          ulStateTblId    = NAS_ESM_NULL;

    /* 检测PTI，PTI为RESERVED，或者当PTI不为0，且通过此PTI不能检索到状态表
       中的记录，则丢弃此消息 */
    enEsmCause = NAS_ESM_ValidatePtiInActDedictOrModOrDeactOrStatusMsg( pstNwMsg->ucPti,
                                                                        &ulStateTblId);
    if (enEsmCause != NAS_ESM_CAUSE_SUCCESS)
    {
        NAS_ESM_LOG1("NAS_ESM_ValidateEsmStatusMsg:WARN:Validate Pti failed, cause : ", enEsmCause);
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}


VOS_UINT32  NAS_ESM_UpdateStatetblNwInitProcedure
(
    const NAS_ESM_NW_MSG_STRU           *pRcvMsg,
    VOS_UINT32                          *pulStateTblId
)
{
    VOS_UINT32                          ulRslt              = NAS_ESM_NULL;
    VOS_UINT32                          ulCId               = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    NAS_ESM_INFO_LOG("NAS_ESM_UpdateStatetblNwInitProcedure is entered.");

    ulRslt = NAS_ESM_QuerySdfId(pRcvMsg,&ulCId);

    if ((NAS_ESM_QUERY_CID_RSLT_ALL_FOUND_EQUAL_CID != ulRslt)
     && (NAS_ESM_QUERY_CID_RSLT_PART_FOUND_EQUAL_CID != ulRslt))
    {
        ulCId = NAS_ESM_GetCid(pRcvMsg->ucEpsbId);
    }

    if (NAS_ESM_SUCCESS == NAS_ESM_GetStateTblIndexByCid(ulCId,pulStateTblId))
    {
        pstStateAddr = NAS_ESM_GetStateTblAddr(*pulStateTblId);

        /*填写状态表信息，包括承载状态，承载Id，承载类型*/
        pstStateAddr->ulEpsbId = pRcvMsg->ucEpsbId;
        pstStateAddr->enBearerCntxtState = NAS_ESM_GetBearCntxtState(pstStateAddr->ulEpsbId);
        pstStateAddr->enBearerCntxtType  = NAS_ESM_GetBearCntxtType(pstStateAddr->ulEpsbId);
        pstStateAddr->ulCid = ulCId;
    }
    else
    {
        NAS_ESM_WARN_LOG("NAS_ESM_UpdateStatetblNwInitProcedure:WARNING:insufficient resource!");

        /*根据消息内容，回复拒绝或忽略*/
        return NAS_ESM_MSG_HANDLED;
    }

    return NAS_ESM_SUCCESS;
}



VOS_VOID  NAS_ESM_ProcNwMsgActDedictdEpsbReq
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    VOS_UINT32                          ulCId               = NAS_ESM_NULL;
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    /* 验证空口消息,不验证TFT 优先级冲突的情况 */
    if (NAS_ESM_MSG_HANDLED == NAS_ESM_ValidateActDedictEpsbReqMsg(&pRcvMsg->stEsmNwMsgIE))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgActDedictdEpsbReq: Msg is Validated and return!");
        return;
    }

    /* 如果承载已经激活，先本地去激活 */
    if(NAS_ESM_BEARER_STATE_ACTIVE == NAS_ESM_GetBearCntxtState(pRcvMsg->stEsmNwMsgIE.ucEpsbId))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgActDedictdEpsbReq : release already active bearer!");
        NAS_ESM_DeactBearerAndInformApp(pRcvMsg->stEsmNwMsgIE.ucEpsbId);
    }


    /* 处理动态表和Cid */
    if (NAS_ESM_PTI_UNASSIGNED_VALUE == pRcvMsg->stEsmNwMsgIE.ucPti )
    {
        ulCId = NAS_ESM_GetCid(pRcvMsg->stEsmNwMsgIE.ucEpsbId);

        if (NAS_ESM_SUCCESS == NAS_ESM_GetStateTblIndexByCid(ulCId,&ulStateTblIndex))
        {
            pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

            /*填写状态表信息，包括承载状态，承载Id，承载类型*/
            pstStateAddr->ulEpsbId = pRcvMsg->stEsmNwMsgIE.ucEpsbId;
            pstStateAddr->enBearerCntxtState = NAS_ESM_GetBearCntxtState(pstStateAddr->ulEpsbId);
            pstStateAddr->enBearerCntxtType  = NAS_ESM_GetBearCntxtType(pstStateAddr->ulEpsbId);
            pstStateAddr->ulCid = ulCId;
        }
        else
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgActDedictdEpsbReq:state table not enough!");
            return;
        }
    }
    else
    {
        if(NAS_ESM_FAILURE == NAS_ESM_QueryStateTblIndexByPti(pRcvMsg->stEsmNwMsgIE.ucPti,
                                                             &ulStateTblIndex))
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgActDedictdEpsbReq:WARN:PTI mismatach!");
            return ;
        }

        pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

        /* 处理网侧下发的PTI已分配，但是承载号不同的情况 */
        NAS_ESM_ProcNwMsgActDediBearSamePtiWithDiffEBI(pRcvMsg,ulStateTblIndex,pstStateAddr);

    }

    /* 处理已经验证没有问题的空口消息 */
    NAS_ESM_ProcValidNwMsgActDedictdEpsbReq(ulStateTblIndex,pRcvMsg);
}


VOS_VOID  NAS_ESM_ProcNwMsgModEpsbReq
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;
    VOS_UINT32                          ulRslt              = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    /* 验证空口消息,不验证TFT 优先级冲突的情况 */
    if (NAS_ESM_MSG_HANDLED == NAS_ESM_ValidateModEpsbReqMsg(&pRcvMsg->stEsmNwMsgIE))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgModEpsbReq: Msg is Validated and return!");
        return;
    }

    /*根据承载ID，检查网络发起的流程与UE发起的流程是否有冲突*/
    if(NAS_ESM_SUCCESS == NAS_ESM_QueryStateTblIndexByEpsbId(pRcvMsg->stEsmNwMsgIE.ucEpsbId,
                                                            &ulStateTblIndex))
    {
        /*网络和UE同时对同一承载进行操作，UE忽略网络的承载修改请求*/
        if(NAS_ESM_PTI_UNASSIGNED_VALUE == pRcvMsg->stEsmNwMsgIE.ucPti )
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgModEpsbReq:Collision of NW orig procedure!");
            return;
        }
    }

    /* 处理动态表和Cid */
    if (NAS_ESM_PTI_UNASSIGNED_VALUE == pRcvMsg->stEsmNwMsgIE.ucPti )
    {
        ulRslt = NAS_ESM_UpdateStatetblNwInitProcedure(&pRcvMsg->stEsmNwMsgIE,
                                                       &ulStateTblIndex);
        if (NAS_ESM_MSG_HANDLED == ulRslt)
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgModEpsbReq:Update state table error!");
            return ;
        }
    }
    else
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgModEpsbReq:NORM:Ue Init procedure!");

        if(NAS_ESM_FAILURE == NAS_ESM_QueryStateTblIndexByPti(pRcvMsg->stEsmNwMsgIE.ucPti,
                                                              &ulStateTblIndex))
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgModEpsbReq:WARN:PTI mismatach!");
            return ;
        }

        pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

        /* 用户发起的承载修改和去激活，不更新动态表信息 */

        NAS_ESM_ProcNwMsgModDediBearSamePtiWithDiffEBI(pRcvMsg,ulStateTblIndex,pstStateAddr);

    }

    /* 处理已经验证没有问题的空口消息 */
    NAS_ESM_ProcValidNwMsgModEpsbReq(ulStateTblIndex,pRcvMsg);
}


VOS_VOID  NAS_ESM_ProcNwMsgActDefltEpsbReq
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    /* 验证空口消息合法性 */
    if (NAS_ESM_MSG_HANDLED == NAS_ESM_ValidateActDefltEpsbReqMsg(&pRcvMsg->stEsmNwMsgIE))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgActDefltEpsbReq: Msg is Validated and return!");
        return;
    }

    /* 如果承载已经激活，先本地去激活 */
    if(NAS_ESM_BEARER_STATE_ACTIVE == NAS_ESM_GetBearCntxtState(pRcvMsg->stEsmNwMsgIE.ucEpsbId))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgActDefltEpsbReq : release already active bearer!");
        NAS_ESM_DeactBearerAndInformApp(pRcvMsg->stEsmNwMsgIE.ucEpsbId);
    }

    /*根据PTI查找到动态表Index，若没查询到则进行异常处理*/
    if(NAS_ESM_FAILURE == NAS_ESM_QueryStateTblIndexByPti(pRcvMsg->stEsmNwMsgIE.ucPti, &ulStateTblIndex))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgActDefltEpsbReq:WARN:PTI mismatach!");
        return ;
    }

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    pstStateAddr->ulEpsbId = pRcvMsg->stEsmNwMsgIE.ucEpsbId;

    /* 处理已经验证没有问题的空口消息 */
    NAS_ESM_ProcValidNwMsgActDefltEpsbReq(ulStateTblIndex,pRcvMsg);
}


VOS_VOID  NAS_ESM_ProcNwMsgDeactEpsbReq
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;
    VOS_UINT32                          ulCId               = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    /* 验证空口消息 */
    if (NAS_ESM_MSG_HANDLED == NAS_ESM_ValidateDeactEpsbReqMsg(&pRcvMsg->stEsmNwMsgIE))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgDeactEpsbReq: Msg is Validated and return!");
        return;
    }

    /*根据承载ID，检查网络发起的流程与UE发起的流程是否有冲突*/
    if(NAS_ESM_SUCCESS == NAS_ESM_QueryStateTblIndexByEpsbId(pRcvMsg->stEsmNwMsgIE.ucEpsbId,
                                                            &ulStateTblIndex))
    {
        if(NAS_ESM_PTI_UNASSIGNED_VALUE == pRcvMsg->stEsmNwMsgIE.ucPti )
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgDeactEpsbReq:Collision of NW orig procedure!");

            NAS_ESM_ProcNwMsgDeactProcedureCollision(ulStateTblIndex,&pRcvMsg->stEsmNwMsgIE);
            return;
        }
    }


    /* 处理动态表和Cid */
    if (NAS_ESM_PTI_UNASSIGNED_VALUE == pRcvMsg->stEsmNwMsgIE.ucPti )
    {
        (VOS_VOID)NAS_ESM_QuerySdfId(&pRcvMsg->stEsmNwMsgIE,&ulCId);
        if (NAS_ESM_SUCCESS == NAS_ESM_GetStateTblIndexByCid(ulCId,&ulStateTblIndex))
        {
            pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

            /*填写状态表信息，包括承载状态，承载Id，承载类型*/
            pstStateAddr->ulEpsbId = pRcvMsg->stEsmNwMsgIE.ucEpsbId;
            pstStateAddr->enBearerCntxtState = NAS_ESM_GetBearCntxtState(pstStateAddr->ulEpsbId);
            pstStateAddr->enBearerCntxtType  = NAS_ESM_GetBearCntxtType(pstStateAddr->ulEpsbId);
            pstStateAddr->ulCid = ulCId;
        }
        else
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgDeactEpsbReq:state table not enough!");
            return;
        }
    }
    else
    {
        if(NAS_ESM_FAILURE == NAS_ESM_QueryStateTblIndexByPti(pRcvMsg->stEsmNwMsgIE.ucPti,
                                                             &ulStateTblIndex))
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgDeactEpsbReq:WARN:PTI mismatach!");
            return ;
        }
        pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);
        NAS_ESM_ProcNwMsgDeaDediBearSamePtiWithDiffEBI(pRcvMsg,ulStateTblIndex,pstStateAddr);
    }


    /* 处理已经验证没有问题的空口消息 */
    NAS_ESM_ProcValidNwMsgDeactEpsbReq(ulStateTblIndex,pRcvMsg);
}
VOS_VOID  NAS_ESM_ProcNwMsgPdnConnRej
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;

    /* 验证空口消息,不验证TFT 优先级冲突的情况 */
    if (NAS_ESM_MSG_HANDLED == NAS_ESM_ValidateNwRejMsg(&pRcvMsg->stEsmNwMsgIE))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgPdnConnRej: Msg is Validated and return!");
        return;
    }

    /*根据PTI查找到动态表Index，若没查询到则丢弃*/
    if(NAS_ESM_FAILURE == NAS_ESM_QueryStateTblIndexByPti(pRcvMsg->stEsmNwMsgIE.ucPti, &ulStateTblIndex))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgPdnConnRej:WARN:PTI mismatach!");
        return ;
    }

    /* xiongxianghui00253310 modify for ftmerrlog begin */
    #if (FEATURE_PTM == FEATURE_ON)
    NAS_ESM_ErrlogInfoProc(pRcvMsg->stEsmNwMsgIE.enEsmCau);
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog begin */

    /* 处理已经验证没有问题的空口消息 */
    NAS_ESM_ProcValidNwMsgPdnConnRej(ulStateTblIndex,pRcvMsg);
}
VOS_VOID  NAS_ESM_ProcNwMsgPdnDisconRej
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;

    /* 验证空口消息,不验证TFT 优先级冲突的情况 */
    if (NAS_ESM_MSG_HANDLED == NAS_ESM_ValidateNwRejMsg(&pRcvMsg->stEsmNwMsgIE))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgPdnDisconRej: Msg is Validated and return!");
        return;
    }

    /*根据PTI查找到动态表Index，若没查询到则丢弃*/
    if(NAS_ESM_FAILURE == NAS_ESM_QueryStateTblIndexByPti(pRcvMsg->stEsmNwMsgIE.ucPti, &ulStateTblIndex))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgPdnDisconRej:WARN:PTI mismatach!");
        return ;
    }

    /* xiongxianghui00253310 modify for ftmerrlog begin */
    #if (FEATURE_PTM == FEATURE_ON)
    NAS_ESM_ErrlogInfoProc(pRcvMsg->stEsmNwMsgIE.enEsmCau);
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog begin */

    /* 处理已经验证没有问题的空口消息 */
    NAS_ESM_ProcValidNwMsgPdnDisconRej(ulStateTblIndex,pRcvMsg);
}
VOS_VOID  NAS_ESM_ProcNwMsgResModRej
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;

    /* 验证空口消息,不验证TFT 优先级冲突的情况 */
    if (NAS_ESM_MSG_HANDLED == NAS_ESM_ValidateNwRejMsg(&pRcvMsg->stEsmNwMsgIE))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgResModRej: Msg is Validated and return!");
        return;
    }

    /*根据PTI查找到动态表Index，若没查询到则丢弃*/
    if(NAS_ESM_FAILURE == NAS_ESM_QueryStateTblIndexByPti(pRcvMsg->stEsmNwMsgIE.ucPti, &ulStateTblIndex))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgResModRej:WARN:PTI mismatach!");
        return ;
    }

    /* xiongxianghui00253310 modify for ftmerrlog begin */
    #if (FEATURE_PTM == FEATURE_ON)
    NAS_ESM_ErrlogInfoProc(pRcvMsg->stEsmNwMsgIE.enEsmCau);
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog begin */

    /* 处理已经验证没有问题的空口消息 */
    NAS_ESM_ProcValidNwMsgResModRej(ulStateTblIndex,pRcvMsg);
}
VOS_VOID  NAS_ESM_ProcNwMsgResAllocRej
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;

    /* 验证空口消息,不验证TFT 优先级冲突的情况 */
    if (NAS_ESM_MSG_HANDLED == NAS_ESM_ValidateNwRejMsg(&pRcvMsg->stEsmNwMsgIE))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgResAllocRej: Msg is Validated and return!");
        return;
    }

    /*根据PTI查找到动态表Index，若没查询到则丢弃*/
    if(NAS_ESM_FAILURE == NAS_ESM_QueryStateTblIndexByPti(pRcvMsg->stEsmNwMsgIE.ucPti, &ulStateTblIndex))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgResAllocRej:WARN:PTI mismatach!");
        return ;
    }

    /* xiongxianghui00253310 modify for ftmerrlog begin */
    #if (FEATURE_PTM == FEATURE_ON)
    NAS_ESM_ErrlogInfoProc(pRcvMsg->stEsmNwMsgIE.enEsmCau);
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog begin */

    /* 处理已经验证没有问题的空口消息 */
    NAS_ESM_ProcValidNwMsgResAllocRej(ulStateTblIndex,pRcvMsg);
}
VOS_VOID  NAS_ESM_ProcNwMsgEsmInfoReq
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    /* 验证空口消息,不验证TFT 优先级冲突的情况 */
    if (NAS_ESM_MSG_HANDLED == NAS_ESM_ValidateEsmInfoReqMsg(&pRcvMsg->stEsmNwMsgIE))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgEsmInfoReq: Msg is Validated and return!");
        return;
    }

    /* 处理已经验证没有问题的空口消息 */
    NAS_ESM_ProcValidNwMsgEsmInfoReq(&pRcvMsg->stEsmNwMsgIE);
}



VOS_VOID  NAS_ESM_ProcNwsMsgEsmStatus
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    /* 验证空口消息,不验证TFT 优先级冲突的情况 */
    if (NAS_ESM_MSG_HANDLED == NAS_ESM_ValidateEsmStatusMsg(&pRcvMsg->stEsmNwMsgIE))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ProcNwsMsgEsmStatus: Msg is Validated and return!");
        return;
    }

    /* 处理已经验证没有问题的空口消息 */
    NAS_ESM_ProcValidNwsMsgEsmStatus(&pRcvMsg->stEsmNwMsgIE);
}


VOS_VOID NAS_ESM_ProcNwMsgEsmNotification
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg
)
{
    VOS_UINT32 ulStateId;
    VOS_UINT8 ucRcvPti;
    VOS_UINT8 ucRcvEbi;
    NAS_ESM_ENCODE_INFO_STRU stEncodeInfo = {0};
    VOS_UINT32 ulMsgLen = 0;

    NAS_ESM_INFO_LOG("NAS_ESM_ProcNwMsgEsmNotification: entered");

    if (NAS_ESM_NULL == pRcvMsg)
    {
        NAS_ESM_ERR_LOG("NAS_ESM_ProcNwMsgEsmNotification: invalid parameter");
        return;
    }

    ucRcvPti = pRcvMsg->stEsmNwMsgIE.ucPti;
    ucRcvEbi = pRcvMsg->stEsmNwMsgIE.ucEpsbId;

    stEncodeInfo.bitOpESMCau = NAS_ESM_OP_TRUE;
    stEncodeInfo.ucPti       = ucRcvPti;
    stEncodeInfo.ulEpsbId    = ucRcvEbi;

    if (NAS_ESM_PTI_RESERVED_VALUE == ucRcvPti)
    {
        /* If the PTI is a reserved value, the UE shall respond with an ESM
           STATUS message including ESM cause #81 "invalid PTI value" */
        stEncodeInfo.ucESMCau = NAS_ESM_CAUSE_INVALID_PTI_VALUE;

        NAS_ESM_EncodeEsmStatus(stEncodeInfo, NAS_ESM_GetCurEsmSndNwMsgAddr(), &ulMsgLen);
        NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID, PS_FALSE, ulMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
    }
    else if ((NAS_ESM_PTI_UNASSIGNED_VALUE != ucRcvPti) &&
             (NAS_ESM_SUCCESS != NAS_ESM_QueryStateTblIndexByPti(ucRcvPti, &ulStateId)))
    {
        /* If the PTI is an assigned value that does not match any PTI in use,
           the UE shall respond with an ESM STATUS message including ESM cause
           #47 "PTI mismatch" */
        stEncodeInfo.ucESMCau = NAS_ESM_CAUSE_PTI_MISMATCH;

        NAS_ESM_EncodeEsmStatus(stEncodeInfo, NAS_ESM_GetCurEsmSndNwMsgAddr(), &ulMsgLen);
        NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID, PS_FALSE, ulMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
    }
    else if (((ucRcvEbi > NAS_ESM_UNASSIGNED_EPSB_ID) && (ucRcvEbi < NAS_ESM_MIN_EPSB_ID)) ||
             (NAS_ESM_SUCCESS != NAS_ESM_QueryStateTblIndexByEpsbId(ucRcvEbi, &ulStateId)) ||
             ((NAS_ESM_UNASSIGNED_EPSB_ID == ucRcvEbi) && (NAS_ESM_PTI_UNASSIGNED_VALUE == ucRcvPti)))
    {
        /* 1. includes a reserved EPS bearer identity value
           2. includes a assigned EPS bearer identity value that does not match
              an existing EPS bearer context
           3. includes the combination of an unassigned PTI value and an
              unassigned EPS bearer identity value  */
        stEncodeInfo.ucESMCau = NAS_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY;

        NAS_ESM_EncodeEsmStatus(stEncodeInfo, NAS_ESM_GetCurEsmSndNwMsgAddr(), &ulMsgLen);
        NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID, PS_FALSE, ulMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
    }
    else
    {
        /*sunbing 49683 2013-10-14 VoLTE begin*/
        if(pRcvMsg->stEsmNwMsgIE.stNotificationInd.ucValue == NAS_ESM_NOTIFICATION_INDICATOR_SRVCC_HO_CANCEL)
        {
            NAS_ESM_SndApsEsmNotificationIndMsg(ESM_APS_NOTIFICATION_IND_SRVCC_HO_CANCELLED);
        }
        else
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgEsmNotification: Err para!");
        }
        /*sunbing 49683 2013-10-14 VoLTE end*/
        /* TODO: Notify ESM user */
    }
}
/*lint +e961*/
/*lint +e960*/
/*****************************************************************************
 Function Name  : NAS_ESM_NwMsgProcess
 Description    : ESM模块对初步译码后的网侧空口消息进行处理
 Input          : EMM_ESM_INTRA_DATA_IND_STRU *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2009-02-05  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_NwMsgProcess
(
    const  EMM_ESM_INTRA_DATA_IND_STRU      *pRcvMsg
)
{
    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_NwMsgProcess is entered.");

    switch(pRcvMsg->stEsmNwMsgIE.enEsmCnMsgType)
    {
       case NAS_ESMCN_MSG_TYPE_ACT_DEFLT_EPS_BEARER_CNTXT_REQ:
           NAS_ESM_ProcNwMsgActDefltEpsbReq(pRcvMsg);
           break;
       case NAS_ESMCN_MSG_TYPE_ACT_DEDICTD_EPS_BEARER_CNTXT_REQ:
           NAS_ESM_ProcNwMsgActDedictdEpsbReq(pRcvMsg);
           break;
       case NAS_ESMCN_MSG_TYPE_MOD_EPS_BEARER_CNTXT_REQ:
           NAS_ESM_ProcNwMsgModEpsbReq(pRcvMsg);
           break;
       case NAS_ESMCN_MSG_TYPE_DEACT_EPS_BEARER_CNTXT_REQ:
           NAS_ESM_ProcNwMsgDeactEpsbReq(pRcvMsg);
           break;
       case NAS_ESMCN_MSG_TYPE_PDN_CONNECT_REJ:
           NAS_ESM_ProcNwMsgPdnConnRej(pRcvMsg);
           break;
       case NAS_ESMCN_MSG_TYPE_PDN_DISCONNECT_REJ:
           NAS_ESM_ProcNwMsgPdnDisconRej(pRcvMsg);
           break;
       case NAS_ESMCN_MSG_TYPE_BEARER_RES_MOD_REJ:
           NAS_ESM_ProcNwMsgResModRej(pRcvMsg);
           break;
       case NAS_ESMCN_MSG_TYPE_BEARER_RES_ALLOC_REJ:
           NAS_ESM_ProcNwMsgResAllocRej(pRcvMsg);
           break;
       case NAS_ESMCN_MSG_TYPE_ESM_INFORMATION_REQ:
           NAS_ESM_ProcNwMsgEsmInfoReq(pRcvMsg);
           break;
       case NAS_ESMCN_MSG_TYPE_ESM_STATUS:
           NAS_ESM_ProcNwsMsgEsmStatus(pRcvMsg);
           break;
       case NAS_ESMCN_MSG_TYPE_ESM_NOTIFICATION:
           NAS_ESM_ProcNwMsgEsmNotification(pRcvMsg);
           break;
       default:
           NAS_ESM_WARN_LOG("NAS_ESM_NwMsgDistr:WARNNING: Illegal Msg Type!");
           break;
    }
}
VOS_VOID NAS_ESM_SndEsmAppSdfProcCnfSucc
(
    VOS_UINT32                          ulStateTblIndex,
    APP_ESM_BEARER_MODIFY_ENUM_UINT32   enModfyType
)
{
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr = VOS_NULL_PTR;

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    /* 回复App 修改成功 *//* 由于没有修改请求，无法得知修改的内容，统一回复TFT&QOS
    根据stAppMsgRecord中的enAppMsgType判断 */
    if (ID_APP_ESM_PDP_MODIFY_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType)
    {
        NAS_ESM_SndEsmAppSdfModCnfSuccMsg(ulStateTblIndex, enModfyType);
    }
    else if (ID_APP_ESM_PDP_SETUP_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType)
    {
        NAS_ESM_SndEsmAppSdfSetupCnfSuccMsg(ulStateTblIndex);
    }
    else if (ID_APP_ESM_PDP_RELEASE_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType)
    {
        NAS_ESM_SndEsmAppSdfRelCnfSuccMsg(ulStateTblIndex);
    }
    else
    {
        NAS_ESM_NORM_LOG("NAS_ESM_SndEsmAppSdfProcCnfSucc: Modfy Cnf Type IS Error!");
    }

    return;
}


VOS_VOID NAS_ESM_ProcNwMsgActDediBearSamePtiWithDiffEBI
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg,
    VOS_UINT32                                ulStateTblIndex,
    NAS_ESM_STATE_INFO_STRU                  *pstStateAddr

)
{
    VOS_UINT32                                ulCId = NAS_ESM_NULL;

    if (NAS_ESM_UNASSIGNED_EPSB_ID == pstStateAddr->ulEpsbId)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgActDediBearSamePtiWithDiffEBI:WARNING:Local Eps Id is Unassigned!");

        pstStateAddr->ulEpsbId = pRcvMsg->stEsmNwMsgIE.ucEpsbId;

        return;
    }

    /* 查询相同的PTI对应的EBI是否相同 */
    if (pstStateAddr->ulEpsbId != pRcvMsg->stEsmNwMsgIE.ucEpsbId)
    {
        /* 回复App 修改成功 *//* 由于没有修改请求，无法得知修改的内容，统一回复TFT&QOS */
        NAS_ESM_SndEsmAppSdfProcCnfSucc(ulStateTblIndex, APP_ESM_BEARER_MODIFY_TFT_QOS);

        /* 释放状态表 */
        NAS_ESM_RelStateTblResource(ulStateTblIndex);

        /* 生成1个CID */
        ulCId = NAS_ESM_GetCid(pRcvMsg->stEsmNwMsgIE.ucEpsbId);

        if (NAS_ESM_SUCCESS == NAS_ESM_GetStateTblIndexByCid(ulCId,&ulStateTblIndex))
        {
            NAS_ESM_INFO_LOG("NAS_ESM_ProcNwMsgActDediBearSamePtiWithDiffEBI:Get Cid Succ");

            pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

            /*填写状态表信息，包括承载状态，承载Id，承载类型*/
            pstStateAddr->ulEpsbId = pRcvMsg->stEsmNwMsgIE.ucEpsbId;
            pstStateAddr->enBearerCntxtState = NAS_ESM_GetBearCntxtState(pstStateAddr->ulEpsbId);
            pstStateAddr->enBearerCntxtType  = NAS_ESM_GetBearCntxtType(pstStateAddr->ulEpsbId);
            pstStateAddr->ulCid = ulCId;
        }
        else
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgActDediBearSamePtiWithDeffEBI: state table not enough!");
        }
    }

    return;
}
VOS_VOID NAS_ESM_ProcNwMsgModDediBearSamePtiWithDiffEBI
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg,
    VOS_UINT32                                ulStateTblIndex,
    NAS_ESM_STATE_INFO_STRU                  *pstStateAddr

)
{
    VOS_UINT32                                ulCId  = NAS_ESM_NULL;
    VOS_UINT32                                ulRslt = NAS_ESM_NULL;

    if (NAS_ESM_UNASSIGNED_EPSB_ID == pstStateAddr->ulEpsbId)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgModDediBearSamePtiWithDiffEBI:WARNING:Local Eps Id is Unassigned!");

        pstStateAddr->ulEpsbId = pRcvMsg->stEsmNwMsgIE.ucEpsbId;

        return;
    }

    /* 查询相同的PTI对应的EBI是否相同 */
    if (pstStateAddr->ulEpsbId != pRcvMsg->stEsmNwMsgIE.ucEpsbId)
    {
        /* 回复App 修改成功 *//* 由于没有修改请求，无法得知修改的内容，统一回复TFT&QOS */
        NAS_ESM_SndEsmAppSdfProcCnfSucc(ulStateTblIndex, APP_ESM_BEARER_MODIFY_TFT_QOS);

        /* 释放状态表 */
        NAS_ESM_RelStateTblResource(ulStateTblIndex);

        /* 查找CID */
        ulRslt = NAS_ESM_QuerySdfId(&(pRcvMsg->stEsmNwMsgIE),&ulCId);

        if ((NAS_ESM_QUERY_CID_RSLT_ALL_FOUND_EQUAL_CID != ulRslt)
         && (NAS_ESM_QUERY_CID_RSLT_PART_FOUND_EQUAL_CID != ulRslt))
        {
            NAS_ESM_INFO_LOG("NAS_ESM_ProcNwMsgModDediBearSamePtiWithDiffEBI:Not Found!");

            /* 是否会出现重复?承载是否有检查? */
            ulCId = NAS_ESM_GetCid(pRcvMsg->stEsmNwMsgIE.ucEpsbId);
        }

        if (NAS_ESM_SUCCESS == NAS_ESM_GetStateTblIndexByCid(ulCId,&ulStateTblIndex))
        {
            NAS_ESM_INFO_LOG("NAS_ESM_ProcNwMsgModDediBearSamePtiWithDiffEBI:Get Cid Succ!");

            pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

            /*填写状态表信息，包括承载状态，承载Id，承载类型*/
            pstStateAddr->ulEpsbId = pRcvMsg->stEsmNwMsgIE.ucEpsbId;
            pstStateAddr->enBearerCntxtState = NAS_ESM_GetBearCntxtState(pstStateAddr->ulEpsbId);
            pstStateAddr->enBearerCntxtType  = NAS_ESM_GetBearCntxtType(pstStateAddr->ulEpsbId);
            pstStateAddr->ulCid = ulCId;
        }
        else
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgModDediBearSamePtiWithDeffEBI:WARNING:insufficient resource!");
        }
    }

    return;
}


VOS_VOID NAS_ESM_ProcNwMsgDeaDediBearSamePtiWithDiffEBI
(
    const EMM_ESM_INTRA_DATA_IND_STRU        *pRcvMsg,
    VOS_UINT32                                ulStateTblIndex,
    NAS_ESM_STATE_INFO_STRU                  *pstStateAddr

)
{
    VOS_UINT32                                ulCId  = NAS_ESM_NULL;

    if (NAS_ESM_UNASSIGNED_EPSB_ID == pstStateAddr->ulEpsbId)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgDeaDediBearSamePtiWithDiffEBI:WARNING:Local Eps Id is Unassigned!");

        pstStateAddr->ulEpsbId = pRcvMsg->stEsmNwMsgIE.ucEpsbId;

        return;
    }

    /* 查询相同的PTI对应的EBI是否相同 */
    if (pstStateAddr->ulEpsbId != pRcvMsg->stEsmNwMsgIE.ucEpsbId)
    {
        /* 回复App 修改成功 *//* 由于没有修改请求，无法得知修改的内容，统一回复TFT&QOS */
        NAS_ESM_SndEsmAppSdfProcCnfSucc(ulStateTblIndex, APP_ESM_BEARER_MODIFY_TFT_QOS);

        /* 释放状态表 */
        NAS_ESM_RelStateTblResource(ulStateTblIndex);

        (VOS_VOID)NAS_ESM_QuerySdfId(&(pRcvMsg->stEsmNwMsgIE),&ulCId);
        if (NAS_ESM_SUCCESS == NAS_ESM_GetStateTblIndexByCid(ulCId,&ulStateTblIndex))
        {
            NAS_ESM_INFO_LOG("NAS_ESM_ProcNwMsgDeaDediBearSamePtiWithDiffEBI:Get Cid Succ!");

            pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

            /*填写状态表信息，包括承载状态，承载Id，承载类型*/
            pstStateAddr->ulEpsbId = pRcvMsg->stEsmNwMsgIE.ucEpsbId;
            pstStateAddr->enBearerCntxtState = NAS_ESM_GetBearCntxtState(pstStateAddr->ulEpsbId);
            pstStateAddr->enBearerCntxtType  = NAS_ESM_GetBearCntxtType(pstStateAddr->ulEpsbId);
            pstStateAddr->ulCid = ulCId;
        }
        else
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgDeaDediBearSamePtiWithDeffEBI:state table not enough!");
        }
    }

    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

