


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEsmInclude.h"
#include    "NasEsmIpMsgProc.h"
/*#include    "IpDhcpv4Server.h"*/
#include    "LRabmPdcpInterface.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASESMNWMSGPDNCONPROC_C
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
/*extern VOS_UINT32  APPITF_SendRequestArp(const VOS_UINT8* pucReqIPAddr);
extern VOS_UINT32 APPITF_ConfigBoardIPAddr(const VOS_UINT8* pucCfgIP);*/



/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_ESM_ProcNwMsgEsmCause54
(
    VOS_UINT32                          ulStateTblIndex,
    const EMM_ESM_INTRA_DATA_IND_STRU  *pRcvMsg
)
{

    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr    = VOS_NULL_PTR;

    (VOS_VOID)pRcvMsg;
    /*打印进入该函数*/
    NAS_ESM_NORM_LOG("NAS_ESM_ProcNwMsgEsmCause54 is entered.");

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    /* 已经注册，由APP拨号触发PDN连接请求的场景，不回复APP结果直接重新发起PDN连接请求，
       请求类型设置为initial request */
    if((NAS_ESM_PS_REGISTER_STATUS_ATTACHED == NAS_ESM_GetEmmStatus())
        &&(ID_APP_ESM_NDISCONN_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType))
    {
        /* 修改状态表中记录的PDN请求类型，重新发起PDN连接请求，由于之前的状态表并未释放，
           仅需重新编码即可 */
        pstStateAddr->enPdnReqType = NAS_ESM_PDN_REQ_TYPE_INITIAL;

        /*填充NAS_ESM_ENCODE_INFO_STRU*/
        NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
        stEncodeInfo.bitOpESMCau        = NAS_ESM_OP_FALSE;
        stEncodeInfo.ulCid              = pstStateAddr->ulCid;
        stEncodeInfo.ucPti              = pstStateAddr->stNwMsgRecord.ucPti;
        stEncodeInfo.ulEpsbId           = pstStateAddr->ulEpsbId;
        stEncodeInfo.enPdnReqType       = pstStateAddr->enPdnReqType;

        /*组装PDN_Connect_Req消息*/
        if(NAS_ESM_FAILURE == NAS_ESM_EncodePdnConReqMsg(stEncodeInfo, \
                                                         pstStateAddr->stNwMsgRecord.aucMsgBuff,\
                                                         &pstStateAddr->stNwMsgRecord.ulMsgLength))
        {
            NAS_ESM_WARN_LOG("NAS_ESM_ProcNwMsgEsmCause54:WARNING:EncodePdnConReqMsg fail!");

            /*向APP回复APP_ESM_BEARER_SETUP_CNF(失败)消息*/
            NAS_ESM_SndEsmAppNdisConnCnfFailMsg(ulStateTblIndex, APP_ERR_UNSPECIFIED_ERROR);
            NAS_ESM_RelStateTblResource(ulStateTblIndex);
            return ;
        }

        /*调用函数，SM->EMM发送消息:ID_EMM_ESM_DATA_REQ(NAS_ESMCN_MSG_TYPE_PDN_CONNECT_REQ)*/
        NAS_ESM_SndEsmEmmDataReqMsg(pstStateAddr->stNwMsgRecord.ulOpId,
                                    pstStateAddr->stNwMsgRecord.ucIsEmcPdnReq,
                                    pstStateAddr->stNwMsgRecord.ulMsgLength,
                                    pstStateAddr->stNwMsgRecord.aucMsgBuff);

        /* 记录空口消息 */
        NAS_ESM_SndAirMsgReportInd(pstStateAddr->stNwMsgRecord.aucMsgBuff,
                                   pstStateAddr->stNwMsgRecord.ulMsgLength,
                                   NAS_ESM_AIR_MSG_DIR_ENUM_UP,
                                   ESM_PDN_CONNECT_REQ);

        /*启动T3482定时器*/
        NAS_ESM_TimerStart(ulStateTblIndex,TI_NAS_ESM_T3482);

        /*状态转换*/
        NAS_ESM_SetProcTransState(ulStateTblIndex,NAS_ESM_BEARER_PROC_TRANS_STATE_PENDING);
        return;

    }

    return;

}

/*****************************************************************************
 Function Name   : NAS_ESM_ProcValidNwMsgPdnConnRej
 Description     : 处理PDN连接拒绝请求消息
 Input           : ulStateTblIndex -- 动态表索引
                   pRcvMsg         -- 收到的空口消息
 Output          : None
 Return          : VOS_VOID

 History         :
    1.sunbing49683      2009-4-16   Draft Enact
    2.lihong00150010    2010-08-12  Modify
    3.liuwenyu00143951  2010-11-11  Modify
*****************************************************************************/
/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_ESM_ProcValidNwMsgPdnConnRej
(
    VOS_UINT32                          ulStateTblIndex,
    const EMM_ESM_INTRA_DATA_IND_STRU  *pRcvMsg
)
{
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr    = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_NORM_LOG("NAS_ESM_ProcValidNwMsgPdnConnRej is entered.");

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    /* 如果发起的是handover的PDN连接请求，收到原因值54 */
    if((NAS_ESM_OP_TRUE == pRcvMsg->stEsmNwMsgIE.bitOpEsmCause)
        &&(NAS_ESM_CAUSE_PDN_CONNECTION_DOES_NOT_EXIST == pRcvMsg->stEsmNwMsgIE.enEsmCau)
        &&(NAS_ESM_PDN_REQ_TYPE_HANDOVER == pstStateAddr->enPdnReqType))
    {
        NAS_ESM_ProcNwMsgEsmCause54(ulStateTblIndex, pRcvMsg);
        return;
    }

    /*向APP回复消息*/
    if(ID_APP_ESM_PDP_SETUP_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType )
    {
        /*向APP发送ID_APP_ESM_PDP_SETUP_CNF(失败)消息*/
        NAS_ESM_SndEsmAppSdfSetupCnfFailMsg(ulStateTblIndex, \
                  NAS_ESM_GetAppErrorByEsmCause(pRcvMsg->stEsmNwMsgIE.enEsmCau));
    }
    /*向APP回复消息*/
    else if(ID_APP_ESM_NDISCONN_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType )
    {
        /*向APP发送ID_APP_ESM_NDISCONN_CNF(失败)消息*/
        NAS_ESM_LOG1("NAS_ESM_ProcValidNwMsgPdnConnRej:ERROR =", NAS_ESM_GetAppErrorByEsmCause(pRcvMsg->stEsmNwMsgIE.enEsmCau));
        NAS_ESM_SndEsmAppNdisConnCnfFailMsg(ulStateTblIndex, \
                                      NAS_ESM_GetAppErrorByEsmCause(pRcvMsg->stEsmNwMsgIE.enEsmCau));
    }
    else
    {
        /*向APP发送ID_APP_ESM_PDP_SETUP_IND(失败)消息*/
        NAS_ESM_SndEsmAppSdfSetupIndMsg(ulStateTblIndex, \
                  NAS_ESM_GetAppErrorByEsmCause(pRcvMsg->stEsmNwMsgIE.enEsmCau));
    }

    /*释放状态表资源*/
    NAS_ESM_RelStateTblResource(ulStateTblIndex);

}



VOS_VOID  NAS_ESM_ProcValidNwMsgActDefltEpsbReq
(
    VOS_UINT32                                  ulStateTblIndex,
    const EMM_ESM_INTRA_DATA_IND_STRU          *pRcvMsg
)
{
    VOS_UINT32                          ulEpsbId         = NAS_ESM_NULL;
    EMM_ESM_INTRA_DATA_IND_STRU        *pMsg             = VOS_NULL_PTR;
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo = VOS_NULL_PTR;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr     = VOS_NULL_PTR;
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;

    /*打印进入该函数*/
    NAS_ESM_NORM_LOG("NAS_ESM_ProcValidNwMsgActDefltEpsbReq is entered.");

    pMsg = (EMM_ESM_INTRA_DATA_IND_STRU *) pRcvMsg;

    /*根据PTI找到状态表的ulStateTblIndex，进入状态处理的PTI能够找到对应状态表*/
    if(NAS_ESM_FAILURE == NAS_ESM_QueryStateTblIndexByPti(pMsg->stEsmNwMsgIE.ucPti,&ulStateTblIndex))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcValidNwMsgActDefltEpsbReq:WARNING: Tbl index Query Fail!");
        return;
    }

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    /*取出承载信息*/
    ulEpsbId = (VOS_UINT32)(pMsg->stEsmNwMsgIE.ucEpsbId);
    pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);
    pstEpsbCntxtInfo->ulLinkedEpsbId = 0;

    /*初始化，并填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.bitOpESMCau = NAS_ESM_OP_FALSE;
    stEncodeInfo.ucPti       = pstStateAddr->stNwMsgRecord.ucPti;
    stEncodeInfo.ulEpsbId    = ulEpsbId;

    /*停止定时器*/
    NAS_ESM_TimerStop(ulStateTblIndex, TI_NAS_ESM_T3482);

    /* 自动接受缺省承载激活的处理 */
    NAS_ESM_DefltBearActMsgAutoAccept(ulStateTblIndex,
                                      stEncodeInfo,
                                      &(pMsg->stEsmNwMsgIE));
}


VOS_VOID NAS_ESM_SaveNwActDefltEpsbReqInfo
(
    const  NAS_ESM_NW_MSG_STRU          *pstMsgIE,
    VOS_UINT32                           ulStateTblIndex
)
{
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr      = VOS_NULL_PTR;
    NAS_ESM_SDF_CNTXT_INFO_STRU        *pstSdfCntxtInfo   = VOS_NULL_PTR;
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo  = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_SaveNwActDefltEpsbReqInfo is entered.");

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    pstSdfCntxtInfo = NAS_ESM_GetSdfCntxtInfo(pstStateAddr->ulCid);

    /* 设置SDF的QOS内容 */
    NAS_ESM_MEM_CPY(&pstSdfCntxtInfo->stSdfQosInfo,
                    &pstMsgIE->stSdfQosInfo,
                    sizeof(NAS_ESM_CONTEXT_LTE_QOS_STRU));

    /* 设置SDF上下文中的PF信息和关联承载信息 */
    pstSdfCntxtInfo->ulSdfPfNum = NAS_ESM_NULL;
    pstSdfCntxtInfo->ulEpsbId = pstMsgIE->ucEpsbId;

    pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(pstMsgIE->ucEpsbId);

    /* 设置EPS QOS标识位和EPS QOS内容 */
    pstEpsbCntxtInfo->bitOpEpsQos = NAS_ESM_OP_TRUE;
    NAS_ESM_MEM_CPY(&(pstEpsbCntxtInfo->stEpsQoSInfo), \
                    &(pstSdfCntxtInfo->stSdfQosInfo), \
                    sizeof(NAS_ESM_CONTEXT_LTE_QOS_STRU));

    /*存储PDN信息，设置PDN标识位*/
    pstEpsbCntxtInfo->bitOpPdnAddr = NAS_ESM_OP_TRUE;
    NAS_ESM_MEM_CPY(&pstEpsbCntxtInfo->stPdnAddrInfo,
                    &pstMsgIE->stPdnAddrInfo,
                    sizeof(NAS_ESM_CONTEXT_IP_ADDR_STRU));
    if ((NAS_ESM_PDN_IPV4 == pstEpsbCntxtInfo->stPdnAddrInfo.ucIpType)
        || (NAS_ESM_PDN_IPV4_IPV6 == pstEpsbCntxtInfo->stPdnAddrInfo.ucIpType))
    {
        #if (VOS_OS_VER != VOS_WIN32)
        /*根据网络分配的IP地址生成CPE的IP地址，目前只适用于IPV4*/
        pstEpsbCntxtInfo->bitOpGateWayAddrInfo = NAS_ESM_OP_TRUE;

        /* 根据IP地址获得子网掩码 */
        pstEpsbCntxtInfo->stSubnetMask.ucIpType = NAS_ESM_PDN_IPV4;
        NAS_ESM_DHCPGetIPv4Mask(pstEpsbCntxtInfo->stPdnAddrInfo.aucIpV4Addr,
                                        pstEpsbCntxtInfo->stSubnetMask.aucIpV4Addr);

        /* 根据IP地址获得网关地址 */
        pstEpsbCntxtInfo->stGateWayAddrInfo.ucIpType = NAS_ESM_PDN_IPV4;
        NAS_ESM_DHCPGetIPv4GateWay(pstEpsbCntxtInfo->stPdnAddrInfo.aucIpV4Addr,
                                        pstEpsbCntxtInfo->stSubnetMask.aucIpV4Addr,
                                        pstEpsbCntxtInfo->stGateWayAddrInfo.aucIpV4Addr);
        #else
        /*根据网络分配的IP地址生成CPE的IP地址，目前只适用于IPV4*/
        pstEpsbCntxtInfo->bitOpGateWayAddrInfo = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(&pstEpsbCntxtInfo->stGateWayAddrInfo,
                        &pstEpsbCntxtInfo->stPdnAddrInfo,
                        sizeof(NAS_ESM_CONTEXT_IP_ADDR_STRU));
        if(pstEpsbCntxtInfo->stGateWayAddrInfo.aucIpV4Addr[APP_MAX_IPV4_ADDR_LEN-1] == NAS_ESM_GATEWAYE_IP1)
        {
            pstEpsbCntxtInfo->stGateWayAddrInfo.aucIpV4Addr[APP_MAX_IPV4_ADDR_LEN-1] = NAS_ESM_GATEWAYE_IP2;
        }
        else
        {
            pstEpsbCntxtInfo->stGateWayAddrInfo.aucIpV4Addr[APP_MAX_IPV4_ADDR_LEN-1] = NAS_ESM_GATEWAYE_IP1;
        }

        NAS_ESM_SetSubnetMask(pstEpsbCntxtInfo->stSubnetMask);
        #endif
    }
    /*设置APN标识位*/
    pstEpsbCntxtInfo->bitOpApn= NAS_ESM_OP_TRUE;
    NAS_ESM_MEM_CPY(&pstEpsbCntxtInfo->stApnInfo,
                    &pstMsgIE->stApnInfo,
                    sizeof(APP_ESM_APN_INFO_STRU));

    /*检查TI(如果存在)，直接存储*/
    pstEpsbCntxtInfo->bitOpTransId = pstMsgIE->bitOpTransId;
    NAS_ESM_MEM_CPY(    &pstEpsbCntxtInfo->stTransId,
                        &pstMsgIE->stTransId,
                        sizeof(NAS_ESM_CONTEXT_TRANS_ID_STRU));

    /*检查PDP QOS(如果存在)，直接存储*/
    pstEpsbCntxtInfo->bitOpNegQos = pstMsgIE->bitOpNegQos;
    NAS_ESM_MEM_CPY(    &pstEpsbCntxtInfo->stCntxtQosInfo,
                        &pstMsgIE->stCntxtQosInfo,
                        sizeof(NAS_ESM_CONTEXT_QOS_STRU));

    /*检查LLC SAPI(如果存在)，直接存储*/
    pstEpsbCntxtInfo->bitOpLlcSapi = pstMsgIE->bitOpLlcSapi;
    pstEpsbCntxtInfo->ucLlcSapi = pstMsgIE->ucLlcSapi;

    /*检查Radio Priority(如果存在)，直接存储*/
    pstEpsbCntxtInfo->bitOpRadioPriority = pstMsgIE->bitOpRadioPriority;
    pstEpsbCntxtInfo->ucRadioPriority = pstMsgIE->ucRadioPriority;

    /*检查Packet Flow Identifer(如果存在)，直接存储*/
    pstEpsbCntxtInfo->bitOpPacketFlowId = pstMsgIE->bitOpPacketFlowId;
    pstEpsbCntxtInfo->ucPacketFlowId = pstMsgIE->ucPacketFlowId;

    /*检查APN-AMBR(如果存在)，直接存储*/
    pstEpsbCntxtInfo->bitOpApnAmbr = pstMsgIE->bitOpApnAmbr;
    NAS_ESM_MEM_CPY(&pstEpsbCntxtInfo->stApnAmbrInfo,
                    &pstMsgIE->stApnAmbrInfo,
                    sizeof(NAS_ESM_CONTEXT_APN_AMBR_STRU));

    /*检查ESM Cause(如果存在)，直接存储*/
    pstEpsbCntxtInfo->bitOpEsmCause = pstMsgIE->bitOpEsmCause;
    pstEpsbCntxtInfo->enEsmCau = pstMsgIE->enEsmCau;

    /*检查ESM PCO(如果存在)，直接存储*/
    pstEpsbCntxtInfo->bitOpPco = pstMsgIE->bitOpPco;
    NAS_ESM_MEM_CPY(&pstEpsbCntxtInfo->stPcoInfo,
                    &pstMsgIE->stPcoInfo,
                    sizeof(NAS_ESM_CONTEXT_PCO_STRU));
}
/*****************************************************************************
 Function Name   : NAS_ESM_SndEsmAppPdpManageIndMsg
 Description     : 向APP发送
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2009-12-3  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_SndEsmAppPdpManageIndMsg
(
    const APP_ESM_PDP_MANAGER_IND_STRU       *pstPdpManInd
)
{
    APP_ESM_PDP_MANAGER_IND_STRU    *pstPdpManIndTmp = VOS_NULL_PTR;

    /*分配空间和检测是否分配成功*/
    pstPdpManIndTmp = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(APP_ESM_PDP_MANAGER_IND_STRU));
    if (VOS_NULL_PTR == pstPdpManIndTmp)
    {
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmAppPdpManageIndMsg:ERROR:Alloc msg fail!" );
        return;
    }

    /*清空*/
    NAS_ESM_MEM_SET(NAS_ESM_GET_MSG_ENTITY(pstPdpManIndTmp), NAS_ESM_NULL, NAS_ESM_GET_MSG_LENGTH(pstPdpManIndTmp));

    /*填写消息头*/
    NAS_ESM_WRITE_APP_MSG_HEAD(pstPdpManIndTmp,
                               ID_APP_ESM_PDP_MANAGER_IND,
                               pstPdpManInd->usOriginalId,
                               pstPdpManInd->usTerminalId,
                               pstPdpManInd->ulSN);

    /* 填写消息内容 */
    pstPdpManIndTmp->ulCid           = pstPdpManInd->ulCid;
    pstPdpManIndTmp->ulOpId          = pstPdpManInd->ulOpId;
    pstPdpManIndTmp->enOperateType   = pstPdpManInd->enOperateType;

    pstPdpManIndTmp->bitOpLinkCid    = pstPdpManInd->bitOpLinkCid;
    pstPdpManIndTmp->ulLinkCid       = pstPdpManInd->ulLinkCid;

    pstPdpManIndTmp->bitOpModifyTpye = pstPdpManInd->bitOpModifyTpye;
    pstPdpManIndTmp->enBearerModifyType = pstPdpManInd->enBearerModifyType;

    /* 调用消息发送函数*/
    NAS_ESM_SND_MSG(pstPdpManIndTmp);

}

/*****************************************************************************
 Function Name   : NAS_ESM_IsDhcpProcedureNeeded
 Description     : 判定是否需要开启DHCP流程获取网络参数
 Input           : ucEpsbId-----------------承载号
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-11-30  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_IsDhcpProcedureNeeded
(
    VOS_UINT32                          ucEpsbId
)
{
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo = VOS_NULL_PTR;

    pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ucEpsbId);

    /* 如果PDN类型为IPV6，则不需要发起DHCP流程 */
    if (NAS_ESM_PDN_IPV6 == pstEpsbCntxtInfo->stPdnAddrInfo.ucIpType)
    {
        NAS_ESM_NORM_LOG("NAS_ESM_IsDhcpProcedureNeeded:PDN type is ipv6!");
        return PS_FALSE;
    }

    /* 如果IPv4地址为0.0.0.0，则需要发起DHCP流程 */
    if (NAS_ESM_IS_IPV4_ADDR_INVALID(pstEpsbCntxtInfo->stPdnAddrInfo.aucIpV4Addr))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_IsDhcpProcedureNeeded:PDN address is 0.0.0.0!");
        return PS_TRUE;
    }

    /* 如果不存在DNS服务器，则需要发起DHCP流程 */
    if ((NAS_ESM_OP_TRUE != pstEpsbCntxtInfo->bitOpPco)
        || (NAS_ESM_NULL == pstEpsbCntxtInfo->stPcoInfo.ucIpv4DnsSerNum))
    {
        NAS_ESM_NORM_LOG("NAS_ESM_IsDhcpProcedureNeeded:DNS address is not exist!");
        return PS_TRUE;
    }

    return PS_FALSE;
}

/*****************************************************************************
 Function Name   : NAS_ESM_SndEsmAppSdfSetupSuccMsg
 Description     : 向APP回复成功消息
 Input           : ucEpsbId-----------------承载号
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-11-30  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_SndEsmAppSdfSetupSuccMsg
(
    VOS_UINT32                          ulStateTblIndex
)
{
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr     = VOS_NULL_PTR;

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);
    if((NAS_ESM_OP_TRUE == pstStateAddr->bitOpAppMsgRecord)
        && (ID_APP_ESM_PDP_SETUP_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType))
    {
        /*向APP发送ID_APP_ESM_PDP_SETUP_CNF(成功)消息*/
        NAS_ESM_SndEsmAppSdfSetupCnfSuccMsg(ulStateTblIndex);
    }
    else if((NAS_ESM_OP_TRUE == pstStateAddr->bitOpAppMsgRecord)
            && (ID_APP_ESM_NDISCONN_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType))
    {
        /*向APP发送ID_APP_ESM_NDISCONN_CNF(成功)消息*/
        NAS_ESM_SndEsmAppNdisConnCnfSuccMsg(ulStateTblIndex);
    }
    else
    {
        NAS_ESM_SndEsmAppSdfSetupIndMsg(ulStateTblIndex, APP_SUCCESS);
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_SetEpsBearTftInfo
 Description     : 赋值承载TFT信息
 Input           : ulBitCId-----------------承载关联CID
                   pstEpsBearInfoInd--------ESM与SM接口的承载信息指针
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-04-30  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_SetEpsBearTftInfo
(
    VOS_UINT32                          ulBitCId,
    SM_ESM_EPS_BEARER_INFO_IND_STRU    *pstEpsBearInfoInd
)
{
    VOS_UINT32                          ulCnt1          = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt2          = NAS_ESM_NULL;
    NAS_ESM_SDF_CNTXT_INFO_STRU        *pstSdfCntxtInfo = VOS_NULL_PTR;

    pstEpsBearInfoInd->bitOpTft         = NAS_ESM_OP_TRUE;
    pstEpsBearInfoInd->stTft.ulPfNum    = NAS_ESM_NULL;

    for ( ulCnt1 = NAS_ESM_MIN_CID; ulCnt1 <= NAS_ESM_MAX_CID; ulCnt1++ )
    {
        if(NAS_ESM_OP_FALSE == ((ulBitCId >> ulCnt1) & NAS_ESM_OP_TRUE))
        {
            continue;
        }

        pstSdfCntxtInfo = NAS_ESM_GetSdfCntxtInfo(ulCnt1);
        for ( ulCnt2 = NAS_ESM_NULL; ulCnt2 < pstSdfCntxtInfo->ulSdfPfNum; ulCnt2++ )
        {
            pstEpsBearInfoInd->stTft.astCidPf[pstEpsBearInfoInd->stTft.ulPfNum].ulCid = ulCnt1;
            NAS_ESM_MEM_CPY(    &pstEpsBearInfoInd->stTft.astCidPf[pstEpsBearInfoInd->stTft.ulPfNum].stPf,
                                &pstSdfCntxtInfo->astSdfPfInfo[ulCnt2],
                                sizeof(NAS_ESM_CONTEXT_TFT_STRU));
            pstEpsBearInfoInd->stTft.ulPfNum++;
        }
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_SetEpsBearPcoIpv4Item
 Description     : 赋值承载PCO IPV4项
 Input           : pstEpsbCntxtInfo-----------------ESM承载信息指针
                   pstEpsBearInfoInd----------------ESM与SM接口中的承载信息指针
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-04-30  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_SetEpsBearPcoIpv4Item
(
    const NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsbCntxtInfo,
    SM_ESM_EPS_BEARER_INFO_IND_STRU    *pstEpsBearInfoInd
)
{
    if (pstEpsbCntxtInfo->stPcoInfo.ucIpv4DnsSerNum == 1)
    {
        pstEpsBearInfoInd->bitOpPcoIpv4Item = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpPriDns = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv4Item.aucPriDns,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv4DnsServer[0].aucIpV4Addr,
                            APP_MAX_IPV4_ADDR_LEN);
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpSecDns = NAS_ESM_OP_FALSE;
    }

    if (pstEpsbCntxtInfo->stPcoInfo.ucIpv4DnsSerNum == 2)
    {
        pstEpsBearInfoInd->bitOpPcoIpv4Item = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpPriDns = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv4Item.aucPriDns,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv4DnsServer[0].aucIpV4Addr,
                            APP_MAX_IPV4_ADDR_LEN);
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpSecDns = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv4Item.aucSecDns,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv4DnsServer[1].aucIpV4Addr,
                            APP_MAX_IPV4_ADDR_LEN);
    }

    if (pstEpsbCntxtInfo->stPcoInfo.ucIpv4PcscfNum == 1)
    {
        pstEpsBearInfoInd->bitOpPcoIpv4Item = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpPriPcscf = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv4Item.aucPriPcscf,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv4Pcscf[0].aucIpV4Addr,
                            APP_MAX_IPV4_ADDR_LEN);
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpSecPcscf = NAS_ESM_OP_FALSE;
    }

    if (pstEpsbCntxtInfo->stPcoInfo.ucIpv4PcscfNum == 2)
    {
        pstEpsBearInfoInd->bitOpPcoIpv4Item = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpPriPcscf = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv4Item.aucPriPcscf,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv4Pcscf[0].aucIpV4Addr,
                            APP_MAX_IPV4_ADDR_LEN);
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpSecPcscf = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv4Item.aucSecPcscf,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv4Pcscf[1].aucIpV4Addr,
                            APP_MAX_IPV4_ADDR_LEN);
    }

    if (pstEpsbCntxtInfo->stPcoInfo.ucIpv4NbnsNum == 1)
    {
        pstEpsBearInfoInd->bitOpPcoIpv4Item = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpPriNbns = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv4Item.aucPriNbns,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv4Nbns[0].aucIpV4Addr,
                            APP_MAX_IPV4_ADDR_LEN);
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpSecNbns = NAS_ESM_OP_FALSE;
    }

    if (pstEpsbCntxtInfo->stPcoInfo.ucIpv4NbnsNum == 2)
    {
        pstEpsBearInfoInd->bitOpPcoIpv4Item = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpPriNbns = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv4Item.aucPriNbns,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv4Nbns[0].aucIpV4Addr,
                            APP_MAX_IPV4_ADDR_LEN);
        pstEpsBearInfoInd->stPcoIpv4Item.bitOpSecNbns = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv4Item.aucSecNbns,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv4Nbns[1].aucIpV4Addr,
                            APP_MAX_IPV4_ADDR_LEN);
    }
}

VOS_VOID NAS_ESM_SetEpsBearPcoIpv6Item
(
    const NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsbCntxtInfo,
    SM_ESM_EPS_BEARER_INFO_IND_STRU    *pstEpsBearInfoInd
)
{
    if (pstEpsbCntxtInfo->stPcoInfo.ucIpv6DnsSerNum == 1)
    {
        pstEpsBearInfoInd->bitOpPcoIpv6Item = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->stPcoIpv6Item.bitOpPriDns = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv6Item.aucPriDns,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv6DnsServer[0].aucIpV6Addr,
                            APP_MAX_IPV6_ADDR_LEN);
        pstEpsBearInfoInd->stPcoIpv6Item.bitOpSecDns = NAS_ESM_OP_FALSE;
    }

    if (pstEpsbCntxtInfo->stPcoInfo.ucIpv6DnsSerNum == 2)
    {
        pstEpsBearInfoInd->bitOpPcoIpv6Item = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->stPcoIpv6Item.bitOpPriDns = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv6Item.aucPriDns,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv6DnsServer[0].aucIpV6Addr,
                            APP_MAX_IPV6_ADDR_LEN);
        pstEpsBearInfoInd->stPcoIpv6Item.bitOpSecDns = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv6Item.aucSecDns,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv6DnsServer[1].aucIpV6Addr,
                            APP_MAX_IPV6_ADDR_LEN);
    }

    if (pstEpsbCntxtInfo->stPcoInfo.ucIpv6PcscfNum == 1)
    {
        pstEpsBearInfoInd->bitOpPcoIpv6Item = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->stPcoIpv6Item.bitOpPriPcscf = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv6Item.aucPriPcscf,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv6Pcscf[0].aucIpV6Addr,
                            APP_MAX_IPV6_ADDR_LEN);
        pstEpsBearInfoInd->stPcoIpv6Item.bitOpSecPcscf = NAS_ESM_OP_FALSE;
    }

    if (pstEpsbCntxtInfo->stPcoInfo.ucIpv6PcscfNum == 2)
    {
        pstEpsBearInfoInd->bitOpPcoIpv6Item = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->stPcoIpv6Item.bitOpPriPcscf = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv6Item.aucPriPcscf,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv6Pcscf[0].aucIpV6Addr,
                            APP_MAX_IPV6_ADDR_LEN);
        pstEpsBearInfoInd->stPcoIpv6Item.bitOpSecPcscf = NAS_ESM_OP_TRUE;
        NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stPcoIpv6Item.aucSecPcscf,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv6Pcscf[1].aucIpV6Addr,
                            APP_MAX_IPV6_ADDR_LEN);
    }

}

/*****************************************************************************
 Function Name   : NAS_ESM_SetEpsBearInfo
 Description     : 赋值承载信息
 Input           : pstEpsbCntxtInfo-----------------ESM承载信息指针
                   pstEpsBearInfoInd----------------ESM与SM接口中的承载信息指针
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-04-30  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_SetEpsBearInfo
(
    const NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsbCntxtInfo,
    SM_ESM_EPS_BEARER_INFO_IND_STRU    *pstEpsBearInfoInd
)
{
    NAS_ESM_CONTEXT_APN_STRU            stTmpApnInfo    = {NAS_ESM_NULL};
    VOS_UINT32                          ulRslt          = NAS_ESM_NULL;

    /* 赋值EPS QOS */
    pstEpsBearInfoInd->bitOpEpsQos = NAS_ESM_OP_TRUE;
    NAS_ESM_MEM_CPY(    &pstEpsBearInfoInd->stEpsQos,
                        &pstEpsbCntxtInfo->stEpsQoSInfo.stQosInfo,
                        sizeof(APP_ESM_EPS_QOS_INFO_STRU));

    /* 赋值关联CID */
    pstEpsBearInfoInd->ulBitCid = pstEpsbCntxtInfo->ulBitCId;

    /* 赋值TFT信息 */
    NAS_ESM_SetEpsBearTftInfo(pstEpsbCntxtInfo->ulBitCId, pstEpsBearInfoInd);

    /* 赋值LLC SAPI */
    pstEpsBearInfoInd->bitOpLlcSapi = pstEpsbCntxtInfo->bitOpLlcSapi;
    pstEpsBearInfoInd->ucLlcSapi = pstEpsbCntxtInfo->ucLlcSapi;

    /* 赋值PDP QOS */
    pstEpsBearInfoInd->bitOpNegQos = pstEpsbCntxtInfo->bitOpNegQos;
    NAS_ESM_MEM_CPY(    &pstEpsBearInfoInd->stNegQos,
                        &pstEpsbCntxtInfo->stCntxtQosInfo,
                        sizeof(NAS_ESM_CONTEXT_QOS_STRU));

    /* 赋值packet flow id */
    pstEpsBearInfoInd->bitOpPacketFlowId = pstEpsbCntxtInfo->bitOpPacketFlowId;
    pstEpsBearInfoInd->ucPacketFlowId = pstEpsbCntxtInfo->ucPacketFlowId;

    /* 赋值radio priority */
    pstEpsBearInfoInd->bitOpRadioPriority = pstEpsbCntxtInfo->bitOpRadioPriority;
    pstEpsBearInfoInd->ucRadioPriority = pstEpsbCntxtInfo->ucRadioPriority;

    /* 赋值transaction id */
    pstEpsBearInfoInd->bitOpTransId = pstEpsbCntxtInfo->bitOpTransId;
    NAS_ESM_MEM_CPY(    &pstEpsBearInfoInd->stTransId,
                        &pstEpsbCntxtInfo->stTransId,
                        sizeof(NAS_ESM_CONTEXT_TRANS_ID_STRU));

    if (PS_TRUE == NAS_ESM_IsDefaultEpsBearerType(pstEpsbCntxtInfo->enBearerCntxtType))
    {
        /* 将译码的APN信息转成未译码的APN信息 */
        ulRslt = NAS_ESM_ApnTransformaton(&stTmpApnInfo,
                                          &pstEpsbCntxtInfo->stApnInfo);
        if(APP_SUCCESS != ulRslt)
        {
            pstEpsBearInfoInd->bitOpApn = NAS_ESM_OP_FALSE;
        }
        else
        {
            /* 赋值APN */
            pstEpsBearInfoInd->bitOpApn = NAS_ESM_OP_TRUE;
            pstEpsBearInfoInd->stApn.ucLength = stTmpApnInfo.ucApnLen;
            NAS_ESM_MEM_CPY(    pstEpsBearInfoInd->stApn.aucValue,
                                stTmpApnInfo.aucApnName,
                                APP_MAX_APN_LEN);
        }

        /* 赋值PDN ADDRESS */
        NAS_ESM_MEM_CPY( &pstEpsBearInfoInd->stPdnAddr,
                         &pstEpsbCntxtInfo->stPdnAddrInfo,
                         sizeof(NAS_ESM_CONTEXT_IP_ADDR_STRU));

        /* 赋值APN-AMBR */
        pstEpsBearInfoInd->bitOpApnAmbr = pstEpsbCntxtInfo->bitOpApnAmbr;
        NAS_ESM_MEM_CPY(    &pstEpsBearInfoInd->stEpsApnAmbr,
                            &pstEpsbCntxtInfo->stApnAmbrInfo,
                            sizeof(NAS_ESM_CONTEXT_APN_AMBR_STRU));

        if (NAS_ESM_OP_TRUE == pstEpsbCntxtInfo->bitOpPco)
        {
            /* 赋值BCM */
            pstEpsBearInfoInd->bitOpBcm = pstEpsbCntxtInfo->stPcoInfo.bitOpBcm;
            pstEpsBearInfoInd->enBcm = pstEpsbCntxtInfo->stPcoInfo.enBcm;

            /* 赋值承载PCO IPV4项 */
            NAS_ESM_SetEpsBearPcoIpv4Item(pstEpsbCntxtInfo, pstEpsBearInfoInd);

            /* 赋值承载PCO IPV6项 */
            NAS_ESM_SetEpsBearPcoIpv6Item(pstEpsbCntxtInfo, pstEpsBearInfoInd);
            /* lihong00150010 ims begin */
            pstEpsBearInfoInd->bitOpImCnSigalFlag   = pstEpsbCntxtInfo->stPcoInfo.bitOpImsCnSignalFlag;
            pstEpsBearInfoInd->enImCnSignalFlag     = (VOS_UINT8)pstEpsbCntxtInfo->stPcoInfo.enImsCnSignalFlag;
        }


        /* 如果是紧急承载，则设置紧急标识 */
        if (NAS_ESM_BEARER_TYPE_EMERGENCY == pstEpsbCntxtInfo->enBearerCntxtType)
        {
            pstEpsBearInfoInd->bitOpEmcInd = NAS_ESM_OP_TRUE;
            pstEpsBearInfoInd->enEmergencyInd = SM_ESM_PDP_FOR_EMC;
        }
        /* lihong00150010 ims end */
    }
    else
    {
        /* 赋值Link EpabId */
        pstEpsBearInfoInd->bitOpLinkedEpsbId = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->ulLinkedEpsbId = pstEpsbCntxtInfo->ulLinkedEpsbId;

        /* 赋值PDN ADDRESS */
        pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(pstEpsbCntxtInfo->ulLinkedEpsbId);
        NAS_ESM_MEM_CPY( &pstEpsBearInfoInd->stPdnAddr,
                         &pstEpsbCntxtInfo->stPdnAddrInfo,
                         sizeof(NAS_ESM_CONTEXT_IP_ADDR_STRU));
    }
}
/* lihong00150010 ims end */

/*****************************************************************************
 Function Name   : NAS_ESM_SndSmEspBearerActIndMsg
 Description     : 向SM发送承载激活消息
 Input           : ucEpsbId-----------------承载号
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-04-30  Draft Enact

*****************************************************************************/
/*lint -specific(-e433)*/

VOS_VOID NAS_ESM_SndSmEspBearerActIndMsg
(
    VOS_UINT32                          ulEpsbId
)
{
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo   = VOS_NULL_PTR;
    SM_ESM_EPS_BEARER_INFO_IND_STRU    *pstEpsBearInfoInd  = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    /*lint -e433 -e826*/
    pstEpsBearInfoInd = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(SM_ESM_EPS_BEARER_INFO_IND_STRU));
    /*lint +e433 +e826*/

    if ( VOS_NULL_PTR == pstEpsBearInfoInd )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndSmEspBearerActIndMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(NAS_ESM_GET_MSG_ENTITY(pstEpsBearInfoInd), 0, NAS_ESM_GET_MSG_LENGTH(pstEpsBearInfoInd));

    /*获取对应EpsbCntxtTbl表地址*/
    pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);

    pstEpsBearInfoInd->enPdpOption  = SM_ESM_PDP_OPT_ACTIVATE;
    pstEpsBearInfoInd->ulEpsbId     = ulEpsbId;

    /* 赋值承载信息 */
    NAS_ESM_SetEpsBearInfo(pstEpsbCntxtInfo, pstEpsBearInfoInd);
    
    if( NAS_ESM_OP_TRUE == pstEpsbCntxtInfo->bitOpEsmCause)
    {
        /*如果承载信息中有原因码，则通知SM*/
        pstEpsBearInfoInd->bitOpErrorCode = NAS_ESM_OP_TRUE;
        pstEpsBearInfoInd->ulErrorCode    = NAS_ESM_GetAppErrorByEsmCause(pstEpsbCntxtInfo->enEsmCau);
    }
    else
    {
        pstEpsBearInfoInd->bitOpErrorCode = NAS_ESM_OP_FALSE;
    }

    /*填写消息头*/
    NAS_ESM_WRITE_SM_MSG_HEAD(pstEpsBearInfoInd, ID_ESM_SM_EPS_BEARER_INFO_IND);

    /* 调用消息发送函数 */
    NAS_ESM_SND_MSG(pstEpsBearInfoInd);
}
/*lint -specific(+e433)*/


VOS_VOID NAS_ESM_DefltBearActMsgAutoAccept
(
    VOS_UINT32                          ulStateTblIndex,
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo,
    const NAS_ESM_NW_MSG_STRU          *pstDecodedNwMsg
)
{
    VOS_UINT32                          ulSndNwMsgLen    = NAS_ESM_NULL;
    VOS_UINT32                          ulCurPdnNum      = NAS_ESM_NULL;
    VOS_UINT32                          ulRslt           = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr     = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_DefltBearActMsgAutoAccept is entered.");

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    /* 保存缺省承载激活的上下文信息 */
    NAS_ESM_SaveNwActDefltEpsbReqInfo(pstDecodedNwMsg,ulStateTblIndex);

    /*向网侧回复Activate default bearer context accept*/
    NAS_ESM_EncodeActDefltEpsbCntxtAccMsg(stEncodeInfo, \
                                  NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                 &ulSndNwMsgLen);
    NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                               ulSndNwMsgLen, NAS_ESM_AIR_MSG_DIR_ENUM_UP,\
                               ESM_ACT_DEFLT_EPS_BEARER_CNTXT_ACP);

    /*如果当前过程处于ATTACH过程中，则通知EMM PDN Conn SUCC；否者通知EMM Data Req*/
    if(NAS_ESM_PS_REGISTER_STATUS_ATTACHED != NAS_ESM_GetEmmStatus())
    {
        NAS_ESM_SndEsmEmmPdnConSuccReqMsg(ulSndNwMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
    }
    else
    {
        NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID, PS_FALSE, ulSndNwMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
    }

    /* 如果支持双APN，则记录注册承载 */
    if (NAS_ESM_OP_TRUE == NAS_ESM_AttachBearerReestEnable())
    {
        if (NAS_ESM_ATTACH_CID == pstStateAddr->ulCid)
        {
            NAS_ESM_SetAttachBearerId(stEncodeInfo.ulEpsbId);
        }
    }

    NAS_ESM_UpdateEsmSndNwMsgLen(ulSndNwMsgLen);

    /* 缓存PTI和回复消息 */
    ulRslt = NAS_ESM_SavePtiBuffItem(stEncodeInfo.ucPti,
                                     (VOS_UINT8)stEncodeInfo.ulEpsbId,
                                     ulSndNwMsgLen,
                                     NAS_ESM_GetCurEsmSndNwMsgAddr());
    if (ulRslt != NAS_ESM_SUCCESS)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_DefltBearActMsgAutoAccept:WARN:Save pti buffer item failed!");
    }

    /*转换状态*/
    NAS_ESM_SetBearCntxtState(stEncodeInfo.ulEpsbId, NAS_ESM_BEARER_STATE_ACTIVE);
    if (NAS_ESM_BEARER_TYPE_EMERGENCY == pstStateAddr->enBearerCntxtType)
    {
        NAS_ESM_SetBearCntxtType(stEncodeInfo.ulEpsbId, NAS_ESM_BEARER_TYPE_EMERGENCY);
    }
    else
    {
        NAS_ESM_SetBearCntxtType(stEncodeInfo.ulEpsbId, NAS_ESM_BEARER_TYPE_DEFAULT);
    }
    NAS_ESM_SetBearCntxtLinkCid(stEncodeInfo.ulEpsbId,pstStateAddr->ulCid);

    /*当前PDN连接数增加1条*/
    ulCurPdnNum = NAS_ESM_GetCurPdnNum()+1;

    /*保存当前PDN连接数*/
    NAS_ESM_SetCurPdnNum(ulCurPdnNum);

    /*关键事件上报*/
    NAS_ESM_SndKeyEventReportInd(NAS_ESM_EVENT_BEARER_SETUP(stEncodeInfo.ulEpsbId));

    /*向APP回复成功消息*/
    NAS_ESM_SndEsmAppSdfSetupSuccMsg(ulStateTblIndex);
#if 0
    if (IP_DHCPV4SERVER_ON == IP_DHCPV4SERVER_GET_DHCPV4SERVER_FLAG())
    {
        if (1 == NAS_ESM_GetCurPdnNum())
        {
            /* 将获取到的网络参数通知IP模块 */
            NAS_ESM_SndEsmIpNwParaIndMsg(pstDecodedNwMsg->ucEpsbId);
        }
    }
#endif

    /*向EMM发送ID_EMM_ESM_BEARER_STATUS_REQ*/
    NAS_ESM_SndEsmEmmBearerStatusReqMsg(EMM_ESM_BEARER_CNTXT_MOD_MUTUAL);

    /*向RABM发送ID_ESM_ERABM_ACT_IND*/
    NAS_ESM_SndEsmRabmActIndMsg(stEncodeInfo.ulEpsbId);

    NAS_ESM_PrintEpsbInfo(stEncodeInfo.ulEpsbId);

    /*释放状态表资源*/
    NAS_ESM_RelStateTblResource(ulStateTblIndex);

    /* 通知SM承载激活 */
    NAS_ESM_SndSmEspBearerActIndMsg(stEncodeInfo.ulEpsbId);
}

/*****************************************************************************
 Function Name   : NAS_ESM_DefltBearActMsgManualProc
 Description     : 缺省承载激活手动处理
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2009-12-3  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_DefltBearActMsgManualProc
(
    VOS_UINT32                          ulStateTblIndex,
    const NAS_ESM_NW_MSG_STRU          *pstDecodedNwMsg
)
{
    APP_ESM_PDP_MANAGER_IND_STRU        stPdpManInd;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr     = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_DefltBearActMsgManualProc is entered.");

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    /* 开辟空间，用于缓存译码后的网络信息 */
    pstStateAddr->pstDecodedNwMsg = (VOS_VOID *)NAS_ESM_MEM_ALLOC(sizeof(NAS_ESM_NW_MSG_STRU));

    if (VOS_NULL_PTR == pstStateAddr->pstDecodedNwMsg)
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_DefltBInactPPendMsgNwActDefltEpsbReq:ERROR:SM->APP,Memory Alloc FAIL!");

        /*释放状态表资源*/
        NAS_ESM_RelStateTblResource(ulStateTblIndex);

        return ;
    }

     /* 将译码后的网络信息拷贝至缓存区 */
    NAS_ESM_MEM_CPY(pstStateAddr->pstDecodedNwMsg,
                    pstDecodedNwMsg,
                    sizeof(NAS_ESM_NW_MSG_STRU));

    /* 清空临时变量stPdpManInd */
    NAS_ESM_MEM_SET(&stPdpManInd,
                     NAS_ESM_NULL,
                     sizeof(APP_ESM_PDP_MANAGER_IND_STRU));

    stPdpManInd.bitOpLinkCid = NAS_ESM_OP_FALSE;
    stPdpManInd.bitOpModifyTpye = NAS_ESM_OP_FALSE;

    /* 填写CID */
    stPdpManInd.ulCid = pstStateAddr->ulCid;

    /* 填写Opid, OriginalId和TerminalId*/
    if((NAS_ESM_OP_TRUE == pstStateAddr->bitOpAppMsgRecord)
        &&(ID_APP_ESM_PDP_SETUP_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType))
    {
        stPdpManInd.ulOpId = pstStateAddr->stAppMsgRecord.ulAppMsgOpId;
        stPdpManInd.usTerminalId = pstStateAddr->stAppMsgRecord.usOriginalId;
        stPdpManInd.usOriginalId = pstStateAddr->stAppMsgRecord.usTerminalId;
    }
    else
    {
        stPdpManInd.ulOpId = NAS_ESM_OPID_TYPE_MANAGER_IND;

        /* 分配本次过程APP_MSG_HEADER*/
        /* 因为在NAS_ESM_SndEsmAppPdpManageIndMsg中还要交换OriginalId和uTerminalId */
        /* 所以这里传入的参数是反的 */
        NAS_ESM_AssignMidHeader(&stPdpManInd.usOriginalId,&stPdpManInd.usTerminalId);
    }

    /* 填写操作类型 */
    stPdpManInd.enOperateType = APP_ESM_BEARER_OPERATE_TYPE_DEF_ACT;

    /* 向APP发送ID_APP_ESM_PDP_MANAGER_IND消息 */
    NAS_ESM_SndEsmAppPdpManageIndMsg(&stPdpManInd);

    /* 启动定时器等待APP回复消息 */
    NAS_ESM_TimerStart(ulStateTblIndex, TI_NAS_ESM_WAIT_APP_CNF);

    /* 设置等待App回复标志位 */
    pstStateAddr->ucIsWaitForUserAnswer = PS_TRUE;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ActDefltBearFailProc
 Description     : 激活缺省承载失败处理
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2009-12-4  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_ActDefltBearFailProc
(
    const NAS_ESM_ENCODE_INFO_STRU     *pstEncodeInfo,
    VOS_UINT32                          ulStateTblIndex
)
{
    VOS_UINT32                          ulSndNwMsgLen    = NAS_ESM_NULL;
    VOS_UINT32                          ulRslt           = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr     = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_ActDefltBearFailProc is entered.");

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    /*向网侧回复Activate default bearer context reject*/
    NAS_ESM_EncodeActDefltEpsbCntxtRejMsg(*pstEncodeInfo, \
                                  NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                 &ulSndNwMsgLen);

    NAS_ESM_SndAirMsgReportInd(NAS_ESM_GetCurEsmSndNwMsgAddr(),\
                                   ulSndNwMsgLen, NAS_ESM_AIR_MSG_DIR_ENUM_UP,\
                                   ESM_ACT_DEFLT_EPS_BEARER_CNTXT_REJ);
    NAS_ESM_SndEsmEmmDataReqMsg(NAS_ESM_ILLEGAL_OPID, PS_FALSE, ulSndNwMsgLen, NAS_ESM_GetCurEsmSndNwMsgAddr());
    NAS_ESM_UpdateEsmSndNwMsgLen(ulSndNwMsgLen);

    /* 缓存PTI和回复消息 */
    ulRslt =  NAS_ESM_SavePtiBuffItem(pstEncodeInfo->ucPti,
                                      (VOS_UINT8)pstEncodeInfo->ulEpsbId,
                                      ulSndNwMsgLen,
                                      NAS_ESM_GetCurEsmSndNwMsgAddr());
    if (ulRslt != NAS_ESM_SUCCESS)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ActDefltBearFailProc:WARN:Save pti buffer item failed!");
    }

    /*向APP回复消息*/
    if(ID_APP_ESM_PDP_SETUP_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType )
    {
        /*向APP发送ID_APP_ESM_PDP_SETUP_CNF(失败)消息*/
        NAS_ESM_SndEsmAppSdfSetupCnfFailMsg(ulStateTblIndex, \
                                      NAS_ESM_GetAppErrorByEsmCause(pstEncodeInfo->ucESMCau));
    }

    /*向APP回复消息*/
    if(ID_APP_ESM_NDISCONN_REQ == pstStateAddr->stAppMsgRecord.enAppMsgType )
    {
        /*向APP发送ID_APP_ESM_NDISCONN_CNF(失败)消息*/
        NAS_ESM_LOG1("NAS_ESM_ActDefltBearFailProc:ERROR =", NAS_ESM_GetAppErrorByEsmCause(pstEncodeInfo->ucESMCau));
        NAS_ESM_SndEsmAppNdisConnCnfFailMsg(ulStateTblIndex, \
                                      NAS_ESM_GetAppErrorByEsmCause(pstEncodeInfo->ucESMCau));
    }


   /*释放状态表资源*/
    NAS_ESM_RelStateTblResource(ulStateTblIndex);
}
/*lint +e961*/
/*lint +e960*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of NasEsmNwMsgPdnConProc.c */
