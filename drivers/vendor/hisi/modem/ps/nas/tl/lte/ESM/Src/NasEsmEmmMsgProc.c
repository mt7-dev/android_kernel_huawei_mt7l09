

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEsmEmmMsgProc.h"
#include    "NasEsmNwMsgProc.h"
#include    "NasEsmInclude.h"
#include    "NasEsmIpMsgProc.h"
/*#include	"IpDhcpv4Server.h"*/
#include "NasEsmAppMsgParaProc.h"


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASESMEMMMSGPROC_C
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

VOS_VOID NAS_ESM_EmmMsgDistr( VOS_VOID *pRcvMsg )
{
    PS_MSG_HEADER_STRU         *pEmmMsg  = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_EmmMsgDistr is entered.");

    pEmmMsg = (PS_MSG_HEADER_STRU*)pRcvMsg;

    /*根据消息名，调用相应的消息处理函数*/
    switch(pEmmMsg->ulMsgName)
    {
        case ID_EMM_ESM_SUSPEND_IND:
            NAS_ESM_RcvEsmEmmSuspendInd( (EMM_ESM_SUSPEND_IND_STRU *) pRcvMsg );
            break;

        case ID_EMM_ESM_RESUME_IND:
            NAS_ESM_RcvEsmEmmResumeInd( (EMM_ESM_RESUME_IND_STRU *) pRcvMsg );
            break;
        /*如果收到的是ID_EMM_ESM_PDN_CON_IND消息*/
        case ID_EMM_ESM_PDN_CON_IND:

            /*调用Sm_RcvEsmEmmDataInd函数*/
            NAS_ESM_RcvEsmEmmPdnConInd( (EMM_ESM_PDN_CON_IND_STRU *) pRcvMsg );
            break;

        /*如果收到的是ID_EMM_ESM_DATA_IND消息*/
        case ID_EMM_ESM_DATA_IND:

            /*调用Sm_RcvEsmEmmDataInd函数*/
            NAS_ESM_RcvEsmEmmDataInd( (EMM_ESM_DATA_IND_STRU *) pRcvMsg );
            break;

        /*如果收到的是ID_EMM_ESM_EST_CNF消息*/
        case ID_EMM_ESM_EST_CNF:

            /*调用Sm_RcvEsmEmmDataInd函数*/
            NAS_ESM_RcvEsmEmmEstCnf( (EMM_ESM_EST_CNF_STRU *) pRcvMsg );
            break;

        /*如果收到的是ID_EMM_ESM_STATUS_IND消息*/
        case ID_EMM_ESM_STATUS_IND:

            /*调用Sm_RcvEsmEmmStatusInd函数*/
            NAS_ESM_RcvEsmEmmStatusInd( (EMM_ESM_STATUS_IND_STRU *) pRcvMsg );
            break;

        /*如果收到的是ID_EMM_ESM_BEARER_STATUS_IND消息*/
        case ID_EMM_ESM_BEARER_STATUS_IND:

            /*调用Sm_RcvEsmEmmBearerStatusInd函数*/
            NAS_ESM_RcvEsmEmmBearerStatusInd( (EMM_ESM_BEARER_STATUS_IND_STRU *) pRcvMsg );
            break;

        /*如果收到的是ID_EMM_ESM_REL_IND消息*/
        case ID_EMM_ESM_REL_IND:

            /* 如果ESM处于非注册态，则直接返回 */
            if (NAS_ESM_PS_REGISTER_STATUS_DETACHED == NAS_ESM_GetEmmStatus())
            {
                NAS_ESM_WARN_LOG("NAS_ESM_EmmMsgDistr:WARNING:Msg is discard cause ESM state is detached!");
                return ;
            }

            NAS_ESM_RcvEmmDetachMsg();

            break;
        case ID_EMM_ESM_DEACT_NON_EMC_BEARER_IND:
            NAS_ESM_RcvEsmEmmDeactAllNonEmcBearerInd();
            break;
        /*PC REPLAY MODIFY BY LEILI END*/
        #if(VOS_WIN32 == VOS_OS_VER)
        case ID_EMM_ESM_REPLAY_EXPORT_CTX_IND:
            NAS_ESM_WARN_LOG("NAS_ESM_EmmMsgDistr:WARNING:EMM->SM Message ID_EMM_ESM_REPLAY_EXPORT_CTX_IND!");
            (void)NAS_ESM_ExportContextData();
            (void)NAS_ERABM_ExportContextData();

            break;
        #endif
        /*PC REPLAY MODIFY BY LEILI END*/
        default:
            NAS_ESM_WARN_LOG("NAS_ESM_EmmMsgDistr:WARNING:EMM->SM Message name non-existent!");
            break;
    }
}

/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_ESM_RcvEsmEmmSuspendInd(const EMM_ESM_SUSPEND_IND_STRU *pRcvMsg )
{
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;
    NAS_ESM_BUFF_MANAGE_INFO_STRU      *pstEsmBuffManInfo   = NAS_ESM_GetEsmBufferManageInfoAddr();

    (VOS_VOID)pRcvMsg;

    /* 设置状态为挂起态 */
    NAS_ESM_SetLModeStatus(NAS_ESM_L_MODE_STATUS_SUSPENDED);

    /*如果延迟释放非紧急承载定时器存在，则停止*/
    NAS_ESM_TimerStop(0, TI_NAS_ESM_REL_NON_EMC_BEARER);

    /* 清空动态表，如果有APP消息，则回复APP相应操作失败，原因值为挂起 */
    NAS_ESM_ClearStateTable(APP_ERR_SM_SUSPENDED, PS_FALSE);

    /* 释放ATTACHING类型缓存记录 */
    for (ulCnt = NAS_ESM_NULL; ulCnt < NAS_ESM_MAX_ATTACHING_BUFF_ITEM_NUM; ulCnt++)
    {
        if (pstEsmBuffManInfo->paAttBuffer[ulCnt] != VOS_NULL_PTR)
        {
            NAS_ESM_ClearEsmBuffItem(NAS_ESM_BUFF_ITEM_TYPE_ATTACHING, ulCnt);
        }
    }

    /* 若ESM当前状态为注册中，则将状态转为去注册态，待GU模注册成功后通知LMM，
       再由LMM通知ESM进入注册态； */
    if (NAS_ESM_PS_REGISTER_STATUS_ATTACHING == NAS_ESM_GetEmmStatus())
    {
        NAS_ESM_SetEmmStatus(NAS_ESM_PS_REGISTER_STATUS_DETACHED);
    }

    /* 如果支持双APN，则停止注册承载重建定时器 */
    if (NAS_ESM_OP_TRUE == NAS_ESM_AttachBearerReestEnable())
    {
        NAS_ESM_TimerStop(0, TI_NAS_ESM_ATTACH_BEARER_REEST);
    }

    /* 回复EMM挂起成功 */
    NAS_ESM_SndEsmEmmSuspendRsp(EMM_ESM_RSLT_TYPE_SUCC);
}


VOS_VOID  NAS_ESM_RcvEsmEmmResumeInd(const EMM_ESM_RESUME_IND_STRU *pRcvMsg )
{
    (VOS_VOID)pRcvMsg;

    /* 设置状态为正常态 */
    NAS_ESM_SetLModeStatus(NAS_ESM_L_MODE_STATUS_NORMAL);

    /* 回复EMM解挂成功 */
    NAS_ESM_SndEsmEmmResumeRsp(EMM_ESM_RSLT_TYPE_SUCC);

    /* 如果支持双APN，则判断是否需要启动注册承载重建定时器 */
    if (NAS_ESM_OP_TRUE == NAS_ESM_AttachBearerReestEnable())
    {
        if ((NAS_ESM_GetCurPdnNum() > 0) &&
            (NAS_ESM_UNASSIGNED_EPSB_ID == NAS_ESM_GetAttachBearerId()))
        {
            /* 如果注册承载重建定时器时长不为0，则启动起重建过程 */
            if (0 != NAS_ESM_GetAttachBearerReestTimerLen())
            {
                NAS_ESM_TimerStart(0, TI_NAS_ESM_ATTACH_BEARER_REEST);
            }
        }
    }
}


VOS_VOID  NAS_ESM_RcvEsmEmmPdnConInd(const EMM_ESM_PDN_CON_IND_STRU *pRcvMsg )
{
    VOS_UINT32                          ulPti               = NAS_ESM_NULL;
    VOS_UINT32                          ulRegCId            = NAS_ESM_NULL;
    VOS_UINT32                          ulStateTblId        = NAS_ESM_NULL;
    VOS_UINT32                          ulCur               = NAS_ESM_NULL;
    VOS_UINT32                          ulSdfNum            = NAS_ESM_NULL;
    NAS_ESM_SDF_PARA_STRU              *pstSdfPara          = VOS_NULL_PTR;
    APP_ESM_SDF_PARA_STRU              *pstAppSdfPara       = VOS_NULL_PTR;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;
    NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;
    NAS_ESM_PDP_MANAGE_INFO_STRU       *pEpsbManageInfo     = VOS_NULL_PTR;


    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_RcvEsmEmmPdnConInd is entered.");

    (VOS_VOID)pRcvMsg;

    if (NAS_ESM_L_MODE_STATUS_NORMAL != NAS_ESM_GetLModeStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmPdnConInd: L mode is already suspended!");

        NAS_ESM_SetLModeStatus(NAS_ESM_L_MODE_STATUS_NORMAL);
    }

    if (NAS_ESM_PS_REGISTER_STATUS_DETACHED != NAS_ESM_GetEmmStatus())
    {
        NAS_ESM_NORM_LOG("NAS_ESM_RcvEsmEmmPdnConInd:NORM: received abnormal Pdn Con Ind !");

        if (PS_TRUE == NAS_ESM_HasEmergencyPdn())
        {
            /*如果有紧急承载，则仅本地释放所有非紧急承载*/
            NAS_ESM_ClearEsmResoureWhenEstingEmcPdn();
        }
        else
        {
            /*ESM去注册，释放资源*/
            NAS_ESM_RcvEmmDetachMsg();
        }
    }

    pstAppSdfPara = NAS_ESM_MEM_ALLOC(sizeof(APP_ESM_SDF_PARA_STRU));

    if (VOS_NULL_PTR == pstAppSdfPara)
    {
         /*打印不合法信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_RcvEsmEmmPdnConInd:ERROR: Mem alloc fail!");

        /* 回复EMM ID_EMM_ESM_PDN_CON_RSP（FAIL） */
        NAS_ESM_SndEsmEmmPdnConRspMsg(EMM_ESM_PDN_CON_RSLT_FAIL, 0,0);

        return;
    }

    /* 逐一获取12套NV参数 */
    for (ulCur = NAS_ESM_MIN_CID; ulCur < NAS_ESM_CID_NV_NUM; ulCur++)
    {
        pstAppSdfPara->ulCId = ulCur;
        /*PC REPLAY MODIFY BY LEILI BEGIN*/
        if (APP_FAILURE == NAS_ESM_GetSdfPara(&ulSdfNum, pstAppSdfPara))
        {
            continue;
        }

        pstSdfPara = NAS_ESM_GetSdfParaAddr(ulCur);
        NAS_ESM_MEM_CPY(pstSdfPara, pstAppSdfPara, sizeof(APP_ESM_SDF_PARA_STRU));
    }

    NAS_ESM_MEM_FREE(pstAppSdfPara);

    /* 从APP获取承载管理模式参数，则采取自动接受模式 */
    pEpsbManageInfo = NAS_ESM_GetBearerManageInfoAddr();
    if (APP_FAILURE == NAS_ESM_GetPdpManageInfo(pEpsbManageInfo))
    {
        /*打印警告信息*/
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmPdnConInd:WARN: Fail to get PDP Manage Info from APP!");
        pEpsbManageInfo->enMode = APP_ESM_BEARER_MANAGE_MODE_AUTO;
        pEpsbManageInfo->enType = APP_ESM_BEARER_MANAGE_TYPE_ACCEPT;
    }
    /*PC REPLAY MODIFY BY LEILI END*/
    /* 获取用于注册的CID，优先级顺序由高到低为CID0 > CID1-CID11 > CID12-CID20 */
    /* 除了个别运营商特殊需求, 需要GU NV项CID0初始化为专有类型，CID11为缺省类型*/
    ulRegCId = NAS_ESM_GetRegistCid();

    if (NAS_ESM_ILL_CID == ulRegCId)
    {
        /*打印不合法信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_RcvEsmEmmPdnConInd: There is no default bearer type sdf");

        /* 回复EMM ID_EMM_ESM_PDN_CON_RSP（FAIL） */
        NAS_ESM_SndEsmEmmPdnConRspMsg(EMM_ESM_PDN_CON_RSLT_FAIL, 0,0);

        return ;
    }

    NAS_ESM_LOG1("NAS_ESM_RcvEsmEmmPdnConInd:The cid of the found sdf is :", ulRegCId);

    /*在动态表中，获取过程标识*/
    if(NAS_ESM_SUCCESS != NAS_ESM_GetStateTblIndexByCid(NAS_ESM_ATTACH_CID,&ulStateTblId))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmPdnConInd:WARNING:insufficient resource!");

        /* 回复EMM ID_EMM_ESM_PDN_CON_RSP（FAIL）*/
        NAS_ESM_SndEsmEmmPdnConRspMsg(EMM_ESM_PDN_CON_RSLT_FAIL, 0,0);

        return ;
    }

    /*将查询到的ulRegCId参数拷贝到CID0中，发起注册*/
    pstSdfPara = NAS_ESM_GetSdfParaAddr(NAS_ESM_ATTACH_CID);
    NAS_ESM_MEM_CPY(    pstSdfPara,
                        NAS_ESM_GetSdfParaAddr(ulRegCId),
                        sizeof(NAS_ESM_SDF_PARA_STRU));

    /*设置初始状态*/
    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblId);
    pstStateAddr->enBearerCntxtState = NAS_ESM_BEARER_STATE_INACTIVE;
    pstStateAddr->enBearerCntxtType  = NAS_ESM_BEARER_TYPE_DEFAULT;
    if(NAS_ESM_PDN_REQ_TYPE_HANDOVER == pRcvMsg->enPdnReqType)
    {
        pstStateAddr->enPdnReqType = NAS_ESM_PDN_REQ_TYPE_HANDOVER;
    }
    else
    {
        pstStateAddr->enPdnReqType = NAS_ESM_PDN_REQ_TYPE_INITIAL;
    }

    /*分配PTI,并存存储到承载信息中*/
    NAS_ESM_AssignPTI(&ulPti);
    pstStateAddr->stNwMsgRecord.ucPti = (VOS_UINT8)ulPti;

    /*填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.bitOpESMCau        = NAS_ESM_OP_FALSE;
    stEncodeInfo.ulCid              = pstStateAddr->ulCid;
    stEncodeInfo.ucPti              = (VOS_UINT8)ulPti;
    stEncodeInfo.ulEpsbId           = pstStateAddr->ulEpsbId;
    stEncodeInfo.enPdnReqType       = pstStateAddr->enPdnReqType;

    /*组装PDN_Connect_Req消息*/
    if( NAS_ESM_FAILURE == NAS_ESM_EncodePdnConReqMsg(stEncodeInfo, \
                                                      pstStateAddr->stNwMsgRecord.aucMsgBuff,\
                                                      &pstStateAddr->stNwMsgRecord.ulMsgLength))
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_RcvEsmEmmPdnConInd:ERROR:EncodePdnConReqMsg fail!");

        /* 回复EMM ID_EMM_ESM_PDN_CON_RSP（FAIL）*/
        NAS_ESM_SndEsmEmmPdnConRspMsg(EMM_ESM_PDN_CON_RSLT_FAIL, 0,0);

        NAS_ESM_RelStateTblResource(ulStateTblId);

        return ;
    }

    /*记录空口消息类型*/
    pstStateAddr->bitOpNwMsgRecord = NAS_ESM_OP_TRUE;
    pstStateAddr->stNwMsgRecord.enEsmCnMsgType \
            = NAS_ESMCN_MSG_TYPE_PDN_CONNECT_REQ;

    /*给EMM发送ID_EMM_ESM_PDN_CON_RSP*/
    NAS_ESM_SndEsmEmmPdnConRspMsg(EMM_ESM_PDN_CON_RSLT_SUCC, \
                                  pstStateAddr->stNwMsgRecord.ulMsgLength,\
                                  pstStateAddr->stNwMsgRecord.aucMsgBuff);

    /*转换状态*/
    NAS_ESM_SetProcTransState(ulStateTblId, NAS_ESM_BEARER_PROC_TRANS_STATE_PENDING);
    NAS_ESM_SetEmmStatus(NAS_ESM_PS_REGISTER_STATUS_ATTACHING);
}

/*****************************************************************************
 Function Name   : NAS_ESM_BufferDecodedNwMsg
 Description     : 缓存ESM消息
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-8-19  Draft Enact
    2.lihong00150010      2010-02-09 Modify
    3.lihong00150010      2010-04-22 Modify

*****************************************************************************/
VOS_VOID  NAS_ESM_BufferDecodedNwMsg
(
    const EMM_ESM_INTRA_DATA_IND_STRU  *pstEmmEsmIntraDataIndMsg,
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause
)
{
    NAS_ESM_ATTACHING_BUFF_ITEM_STRU   *pstDecodedNwMsg     = VOS_NULL_PTR;
    NAS_ESM_BUFF_MANAGE_INFO_STRU      *pstEsmBuffManInfo   = NAS_ESM_GetEsmBufferManageInfoAddr();
    VOS_UINT32                          ulBuffItemLen       = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;

    /* 判断是否已到达系统能够存储缓存记录的极限 */
    if (pstEsmBuffManInfo->ucAttBuffItemNum >= NAS_ESM_MAX_ATTACHING_BUFF_ITEM_NUM)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_BufferDecodedNwMsg:WARN:Attaching Buffer is full!");
        return;
    }

    /* 计算缓存记录长度 */
    ulBuffItemLen = sizeof(NAS_ESM_ATTACHING_BUFF_ITEM_STRU);

    /* 获取缓存记录空间 */
    pstDecodedNwMsg = (NAS_ESM_ATTACHING_BUFF_ITEM_STRU *)
                                NAS_COMM_AllocBuffItem( NAS_COMM_BUFF_TYPE_ESM,
                                                        ulBuffItemLen);
    if (VOS_NULL_PTR == pstDecodedNwMsg)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_BufferDecodedNwMsg:WARN:NAS_COMM_AllocBuffItem is failed!");
        return;
    }

    /* 缓存译码以后的网络消息 */
    NAS_ESM_MEM_CPY(&pstDecodedNwMsg->stDecodedNwMsg,
                    pstEmmEsmIntraDataIndMsg,
                    sizeof(EMM_ESM_INTRA_DATA_IND_STRU));

    /* 缓存译码结果 */
    pstDecodedNwMsg->enDecodeRslt = enEsmCause;

    /* 查找用于存储缓存记录首地址的指针 */
    for (ulCnt = NAS_ESM_NULL; ulCnt < NAS_ESM_MAX_ATTACHING_BUFF_ITEM_NUM; ulCnt++)
    {
        if (pstEsmBuffManInfo->paAttBuffer[ulCnt] == VOS_NULL_PTR)
        {
            break;
        }
    }

    if (ulCnt >= NAS_ESM_MAX_ATTACHING_BUFF_ITEM_NUM)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_BufferDecodedNwMsg:Get the valuable which stores buffer item addr failed!");
        return ;
    }

    /* 记录存储缓存记录的首地址 */
    pstEsmBuffManInfo->paAttBuffer[ulCnt] = (VOS_VOID *)pstDecodedNwMsg;

    /* 增加缓存记录个数 */
    pstEsmBuffManInfo->ucAttBuffItemNum++;

    NAS_ESM_INFO_LOG1("NAS_ESM_BufferDecodedNwMsg:INFO:Buffer network msg success!", ulCnt);
}


VOS_VOID NAS_ESM_RcvEsmEmmDataInd( EMM_ESM_DATA_IND_STRU *pRcvMsg )
{
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause          = NAS_ESM_CAUSE_SUCCESS;
    VOS_UINT32                          ulRcvNwMsgLen       = NAS_ESM_NULL;
    EMM_ESM_INTRA_DATA_IND_STRU        *pstEmmEsmIntraDataIndMsg = VOS_NULL_PTR;
    VOS_UINT8                          *pucTmpSmMsg         = VOS_NULL_PTR;
    NAS_ESM_SND_NW_MSG_STRU            *pstEsmSndNwMsg      = VOS_NULL_PTR;
    NAS_ESM_NW_MSG_STRU                *pstEsmNwMsgIE       = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_RcvEsmEmmDataInd is entered.");

    if (NAS_ESM_L_MODE_STATUS_NORMAL != NAS_ESM_GetLModeStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmDataInd: L mode is already suspended!");
        return ;
    }

    /* 如果ESM处于非注册态，则直接返回 */
    if (NAS_ESM_PS_REGISTER_STATUS_DETACHED == NAS_ESM_GetEmmStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmDataInd:WARNING:Msg is discard cause ESM state is detached!");
        return ;
    }

    pstEmmEsmIntraDataIndMsg = NAS_ESM_MEM_ALLOC(sizeof(EMM_ESM_INTRA_DATA_IND_STRU));
    if(VOS_NULL_PTR == pstEmmEsmIntraDataIndMsg)
    {
        NAS_ESM_ERR_LOG("NAS_ESM_RcvEsmEmmDataInd:Alloc Mem fail!");
        return;
    }

    NAS_ESM_MEM_SET(pstEmmEsmIntraDataIndMsg, 0, sizeof(EMM_ESM_INTRA_DATA_IND_STRU));

    /*构造内部的ID_EMM_ESM_DATA_IND消息(携带一条已解析的空口消息)*/
    pstEmmEsmIntraDataIndMsg->ulSenderPid = PS_PID_MM;

    pstEsmNwMsgIE = &(pstEmmEsmIntraDataIndMsg->stEsmNwMsgIE);

    /*取出此消息中所包含SM消息的长度*/
    ulRcvNwMsgLen = pRcvMsg->stEsmMsg.ulEsmMsgSize;
    if( ulRcvNwMsgLen > ESM_MAX_MSG_CONTAINER_CNTNTS_LEN )
    {
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmDataInd:WARNING:EMM->SM Message NW Msg over-long!");
        ulRcvNwMsgLen = ESM_MAX_MSG_CONTAINER_CNTNTS_LEN;
    }

    /*取得SM消息首地址*/
    pucTmpSmMsg = pRcvMsg->stEsmMsg.aucEsmMsg;

    pstEmmEsmIntraDataIndMsg->ulMsgId = ID_EMM_ESM_DATA_IND;

    /*清空NAS_ESM_NW_MSG_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)pstEsmNwMsgIE, 0, sizeof(NAS_ESM_NW_MSG_STRU));

    /*调用空口消息解析函数, 如果解析失败直接返回*/
    enEsmCause = NAS_ESM_DecodeNwMsg( pucTmpSmMsg,
                                   &ulRcvNwMsgLen,
                                    pstEsmNwMsgIE );

    /* 若SM的状态为NAS_ESM_PS_REGISTER_STATUS_ATTACHING，且此消息不为
       ESM INFORMATION REQ消息或者缺省承载激活请求消息，则对消息进行缓存；
       否则直接处理消息 */
    if((NAS_ESM_PS_REGISTER_STATUS_ATTACHING == NAS_ESM_GetEmmStatus())
        && (NAS_ESMCN_MSG_TYPE_ESM_INFORMATION_REQ != pstEsmNwMsgIE->enEsmCnMsgType)
        && (NAS_ESMCN_MSG_TYPE_ACT_DEFLT_EPS_BEARER_CNTXT_REQ != pstEsmNwMsgIE->enEsmCnMsgType)
        && (NAS_ESMCN_MSG_TYPE_PDN_CONNECT_REJ != pstEsmNwMsgIE->enEsmCnMsgType))
    {
        /* 缓存ESM消息 */
        NAS_ESM_BufferDecodedNwMsg(pstEmmEsmIntraDataIndMsg, enEsmCause);

        NAS_ESM_MEM_FREE(pstEmmEsmIntraDataIndMsg);

        return;
    }

    /*分配空间和检测是否分配成功,用于存放SM准备回复给网侧的消息*/
    pstEsmSndNwMsg = NAS_ESM_GetEsmSndNwMsgAddr();
    pstEsmSndNwMsg->pucNwMsg = (VOS_UINT8 *)NAS_ESM_MEM_ALLOC(ESM_MAX_MSG_CONTAINER_CNTNTS_LEN);
    if ( VOS_NULL_PTR == pstEsmSndNwMsg->pucNwMsg )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_RcvEsmEmmDataInd:ERROR:Alloc Snd Nw Msg fail!");

        NAS_ESM_MEM_FREE(pstEmmEsmIntraDataIndMsg);
        return;
    }

    /*如果解析错误，返回拒绝或丢弃*/
    if( NAS_ESM_CAUSE_SUCCESS != enEsmCause )
    {
        NAS_ESM_ProcNwMsgForDecodeError(pstEsmNwMsgIE,enEsmCause);

        /*打印异常信息*/
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmDataInd:WARNING: NW Message Error!");
    }
    else/*如果消息解析成功*/
    {
        /*处理此条空口消息*/
        NAS_ESM_NwMsgProcess(pstEmmEsmIntraDataIndMsg);
    }

    /*释放空口消息占用的空间*/
    NAS_ESM_MEM_FREE(pstEsmSndNwMsg->pucNwMsg);
    pstEsmSndNwMsg->pucNwMsg = VOS_NULL_PTR;
    pstEsmSndNwMsg->ulNwMsgSize = NAS_ESM_NULL;

    NAS_ESM_MEM_FREE(pstEmmEsmIntraDataIndMsg);
}



VOS_VOID NAS_ESM_RcvEsmEmmBearerStatusInd(const EMM_ESM_BEARER_STATUS_IND_STRU *pRcvMsg )
{
    VOS_UINT32                  ulCnt                       = NAS_ESM_NULL;
    VOS_UINT32                  ulQueryRslt                 = NAS_ESM_FAILURE;
    VOS_UINT32                  ulEpsbId                    = NAS_ESM_NULL;
    VOS_UINT32                  ulEpsbIdNum                 = NAS_ESM_NULL;
    VOS_UINT32                  aulEpsbId[NAS_ESM_MAX_EPSB_NUM] = {NAS_ESM_NULL};
    VOS_UINT32                  ulStateTblIndex             =  NAS_ESM_NULL;
    NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsbCntxtInfo          = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_RcvEsmEmmBearerStatusInd is entered.");

    if (NAS_ESM_L_MODE_STATUS_NORMAL != NAS_ESM_GetLModeStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmBearerStatusInd: L mode is already suspended!");
        return ;
    }

    /* 如果ESM处于非注册态或注册中态，则直接返回 */
    if (NAS_ESM_PS_REGISTER_STATUS_ATTACHED != NAS_ESM_GetEmmStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmBearerStatusInd:WARNING:Msg is discard cause ESM state is not attached!");
        return ;
    }

    /*取出ID_EMM_ESM_BEARER_STATUS_IND消息中的EPSID信息*/
    ulEpsbIdNum = pRcvMsg->ulEpsIdNum;
    NAS_ESM_MEM_CPY(aulEpsbId, pRcvMsg->aulEsmEpsId, ulEpsbIdNum*(sizeof(VOS_UINT32)/sizeof(VOS_UINT8)));

    /*轮询每一个承载信息*/
    for( ulEpsbId = NAS_ESM_MIN_EPSB_ID; ulEpsbId <= NAS_ESM_MAX_EPSB_ID; ulEpsbId++ )
    {
        /*取出承载信息*/
        pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);

        /*如果此承载未激活*/
        if( NAS_ESM_BEARER_STATE_INACTIVE == pstEpsbCntxtInfo->enBearerCntxtState )
        {
            continue;
        }

        ulQueryRslt = NAS_ESM_FAILURE;

        /*查询ID_EMM_ESM_BEARER_STATUS_IND消息的EPSID信息中是否包含此承载ID*/
        for( ulCnt = 0; ulCnt < ulEpsbIdNum; ulCnt++ )
        {
            if( ulEpsbId == aulEpsbId[ulCnt] )
            {
                ulQueryRslt = NAS_ESM_SUCCESS;
                break;
            }
        }

        /*如果不包含*/
        if( NAS_ESM_FAILURE == ulQueryRslt )
        {
            /*通知APP释放承载，并释放相关承载和资源*/
            NAS_ESM_DeactBearerAndInformApp(ulEpsbId);

            /*清除和中止与ulEpsbId关联的流程信息*/
            if(NAS_ESM_SUCCESS == NAS_ESM_QueryStateTblIndexByEpsbId(ulEpsbId, &ulStateTblIndex))
            {
                NAS_ESM_RelStateTblResource(ulStateTblIndex);
            }
        }
    }

    /*通知EMM当前承载状态信息，发送ID_EMM_ESM_BEARER_STATUS_REQ*/
    NAS_ESM_SndEsmEmmBearerStatusReqMsg(EMM_ESM_BEARER_CNTXT_MOD_MUTUAL);

    /*如果支持双APN，则检查注册承载，并进行处理*/
    if (NAS_ESM_OP_TRUE == NAS_ESM_AttachBearerReestEnable())
    {
        NAS_ESM_TimerStop(0, TI_NAS_ESM_ATTACH_BEARER_REEST);
        if (NAS_ESM_UNASSIGNED_EPSB_ID == NAS_ESM_GetAttachBearerId())
        {
            NAS_ESM_ReestablishAttachBearer();
        }
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_BufferedNwMsgProc
 Description     : 处理ESM缓存消息
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2009-8-19  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_BufferedNwMsgProc()
{
    NAS_ESM_ATTACHING_BUFF_ITEM_STRU   *pstDecodedNwMsg     = VOS_NULL_PTR;
    NAS_ESM_SND_NW_MSG_STRU            *pstEsmSndNwMsg      = VOS_NULL_PTR;
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;

    /*分配空间和检测是否分配成功,用于存放SM准备回复给网侧的消息*/
    pstEsmSndNwMsg = NAS_ESM_GetEsmSndNwMsgAddr();
    pstEsmSndNwMsg->pucNwMsg = (VOS_UINT8 *)NAS_ESM_MEM_ALLOC(ESM_MAX_MSG_CONTAINER_CNTNTS_LEN);

    if ( VOS_NULL_PTR == pstEsmSndNwMsg->pucNwMsg )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_BufferedNwMsgProc:ERROR:Alloc Snd Nw Msg fail!");
        return;
    }

    for (ulCnt = NAS_ESM_NULL; ulCnt < NAS_ESM_MAX_ATTACHING_BUFF_ITEM_NUM; ulCnt++)
    {
        /* 获取缓存记录 */
        pstDecodedNwMsg = (NAS_ESM_ATTACHING_BUFF_ITEM_STRU*)
                                NAS_ESM_GetBuffItemAddr(NAS_ESM_BUFF_ITEM_TYPE_ATTACHING, ulCnt);

        if (pstDecodedNwMsg == VOS_NULL_PTR)
        {
            continue;
        }

        NAS_ESM_INFO_LOG1("NAS_ESM_BufferedNwMsgProc:NORM:Buffer Itmem Index:", ulCnt);

        pstEsmSndNwMsg->ulNwMsgSize = NAS_ESM_NULL;
        NAS_ESM_MEM_SET(    pstEsmSndNwMsg->pucNwMsg,
                            NAS_ESM_NULL,
                            ESM_MAX_MSG_CONTAINER_CNTNTS_LEN);

        /*如果解析错误，返回拒绝或丢弃*/
        if( NAS_ESM_CAUSE_SUCCESS != pstDecodedNwMsg->enDecodeRslt)
        {
            NAS_ESM_ProcNwMsgForDecodeError(&pstDecodedNwMsg->stDecodedNwMsg.stEsmNwMsgIE,
                                            pstDecodedNwMsg->enDecodeRslt);

            /*打印异常信息*/
            NAS_ESM_WARN_LOG("NAS_ESM_BufferedNwMsgProc:WARNING: NW Message Error!");
        }
        else/*如果消息解析成功*/
        {
            /*处理此条空口消息*/
            NAS_ESM_NwMsgProcess(&pstDecodedNwMsg->stDecodedNwMsg);
        }

        /* 清除缓存记录 */
        NAS_ESM_ClearEsmBuffItem(NAS_ESM_BUFF_ITEM_TYPE_ATTACHING, ulCnt);
    }

    /*释放空口消息占用的空间*/
    NAS_ESM_MEM_FREE(pstEsmSndNwMsg->pucNwMsg);
    pstEsmSndNwMsg->pucNwMsg = VOS_NULL_PTR;
    pstEsmSndNwMsg->ulNwMsgSize = NAS_ESM_NULL;
}
VOS_VOID NAS_ESM_RcvEsmEmmEstCnf(const EMM_ESM_EST_CNF_STRU *pRcvMsg )
{
    EMM_ESM_ATTACH_RESULT_ENUM_UINT32   ulEstRst         = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU             *pstStateTblInfo = NAS_ESM_NULL;
    VOS_UINT32                          ulStateTblIndex  = 0;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG2("NAS_ESM_RcvEsmEmmEstCnf is entered.", NAS_ESM_GetLModeStatus(), pRcvMsg->ulEstRst);

    if (NAS_ESM_L_MODE_STATUS_NORMAL != NAS_ESM_GetLModeStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmEstCnf: L mode is already suspended!");
        return ;
    }

    /*取出消息内容*/
    ulEstRst = ((EMM_ESM_EST_CNF_STRU *)pRcvMsg)->ulEstRst;

    /*判断当前EMM注册状态，如果是Attaching，根据消息内容，保存注册状态；否则丢弃*/
    if( NAS_ESM_PS_REGISTER_STATUS_ATTACHING == NAS_ESM_GetEmmStatus())
    {
        /*注册成功*/
        if( EMM_ESM_ATTACH_RESULT_SUCC == ulEstRst )
        {
            /*保存注册状态*/
            NAS_ESM_SetEmmStatus(NAS_ESM_PS_REGISTER_STATUS_ATTACHED);

            /* 处理ESM缓存消息 */
            NAS_ESM_BufferedNwMsgProc();
        }
        else
        {
            /*EMM注册失败或去注册，释放资源*/
            NAS_ESM_RcvEmmDetachMsg();
        }
    }
    else if (NAS_ESM_PS_REGISTER_STATUS_ATTACHED == NAS_ESM_GetEmmStatus())
    {
        /* 如果紧急PDN连接建立请求和EMM StatusInd对冲时会出现当此分支，
         * 对冲时EMM针对紧急PDN的EstReq将返回失败，ESM后续处理如下:
         *      1. 不改变ESM记录的Emm注册状态
         *      2. 将缓存的紧急PDN连接建立消息使用DataReq进行重发，并启动T3482
         */
        if ((EMM_ESM_ATTACH_RESULT_FAIL == ulEstRst) &&
            (NAS_ESM_SUCCESS == NAS_ESM_QueryStateTblIndexForEmc(&ulStateTblIndex)))
        {
            pstStateTblInfo = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

            NAS_ESM_LOG1("NAS_ESM_RcvEsmEmmEstCnf", pstStateTblInfo->ulCid);

            NAS_ESM_SndEsmEmmDataReqMsg(pstStateTblInfo->stNwMsgRecord.ulOpId,
                                        pstStateTblInfo->stNwMsgRecord.ucIsEmcPdnReq,
                                        pstStateTblInfo->stNwMsgRecord.ulMsgLength,
                                        pstStateTblInfo->stNwMsgRecord.aucMsgBuff);

            /*启动T3482定时器*/
            NAS_ESM_TimerStart(ulStateTblIndex, TI_NAS_ESM_T3482);

            /*状态转换*/
            /*之前已经更改，这里不再设置状态*/
        }
    }
    else
    {
        /*打印异常信息*/
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmEmmEstCnf:Register Status is not attachingl!");
    }
}


VOS_VOID NAS_ESM_RcvEsmEmmStatusInd(const EMM_ESM_STATUS_IND_STRU *pRcvMsg )
{
    EMM_ESM_ATTACH_STATUS_ENUM_UINT32   ulEstRst;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_RcvEsmEmmStatusInd is entered.");

    /*取出消息内容*/
    ulEstRst = ((EMM_ESM_STATUS_IND_STRU *)pRcvMsg)->enEMMStatus;

    /*如果消息内容为ATTACHED，记录注册状态*/
    if( EMM_ESM_ATTACH_STATUS_ATTACHED == ulEstRst )
    {
        /*保存注册状态*/
        NAS_ESM_SetEmmStatus(NAS_ESM_PS_REGISTER_STATUS_ATTACHED);

        /* 处理ESM缓存消息 */
        NAS_ESM_BufferedNwMsgProc();
    }
    /*消息内容为DETACHED，释放资源*/
    else if(EMM_ESM_ATTACH_STATUS_DETACHED == ulEstRst)
    {
        /*如果延迟释放非紧急承载定时器存在，则停止*/
        NAS_ESM_TimerStop(0, TI_NAS_ESM_REL_NON_EMC_BEARER);

        /*EMM注册失败或去注册，释放资源*/
        NAS_ESM_RcvEmmDetachMsg();
    }
    else if (EMM_ESM_ATTACH_STATUS_EMC_ATTACHING == ulEstRst)
    {
        /* 由紧急PDN连接导致EMM的TAU/Service/Attach失败，且后续将要进行紧急注册，
         * ESM清除紧急注册以外的资源
         */
        NAS_ESM_ClearEsmResoureWhenEstingEmcPdn();
    }
    else
    {
        /*bsun 检查动态表，重启流程*/
        NAS_ESM_NORM_LOG("NAS_ESM_RcvEsmEmmStatusInd:NORM:UE re-entry Sevice NW!");
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_RcvEmmDetachMsg
 Description     : SM收到EMM注册失败或去注册信息，回复APP信息和释放本地资源
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_VOID

 History         :
    1.sunbing      2008-10-17  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_RcvEmmDetachMsg( VOS_VOID )
{
    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_RcvEmmDetachMsg is entered.");

    /* 通知RABM清除资源 */
    NAS_ESM_SndEsmRabmRelIndMsg();
#if 0

    if (IP_DHCPV4SERVER_ON == IP_DHCPV4SERVER_GET_DHCPV4SERVER_FLAG())
    {
        /* 通知IP清楚资源 */
        NAS_ESM_SndEsmIpRelInd();
    }
#endif


    /*清资源*/
    NAS_ESM_ClearEsmResource();
}
/*****************************************************************************
 Function Name   : NAS_ESM_SndEsmEmmSuspendRsp
 Description     : ESM模块回复ID_EMM_ESM_SUSPEND_RSP消息
 Input           : EMM_ESM_RSLT_TYPE_ENUM_UINT32       enRslt
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-05-02  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_SndEsmEmmSuspendRsp
(
    EMM_ESM_RSLT_TYPE_ENUM_UINT32       enRslt
)
{
    EMM_ESM_SUSPEND_RSP_STRU           *pstEsmEmmSuspendRsp  = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    pstEsmEmmSuspendRsp = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_SUSPEND_RSP_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstEsmEmmSuspendRsp)
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmSuspendRsp:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET( NAS_ESM_GET_MSG_ENTITY(pstEsmEmmSuspendRsp), 0, NAS_ESM_GET_MSG_LENGTH(pstEsmEmmSuspendRsp));

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pstEsmEmmSuspendRsp, ID_EMM_ESM_SUSPEND_RSP);

    /*填写响应结果*/
    pstEsmEmmSuspendRsp->enRslt= enRslt;

    /*调用消息发送函数 */
    NAS_ESM_SND_MSG(pstEsmEmmSuspendRsp);
}

/*****************************************************************************
 Function Name   : NAS_ESM_SndEsmEmmResumeRsp
 Description     : ESM模块回复ID_EMM_ESM_RESUME_RSP消息
 Input           : EMM_ESM_RSLT_TYPE_ENUM_UINT32       enRslt
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-05-02  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_SndEsmEmmResumeRsp
(
    EMM_ESM_RSLT_TYPE_ENUM_UINT32       enRslt
)
{
    EMM_ESM_RESUME_RSP_STRU            *pstEsmEmmResumeRsp  = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    pstEsmEmmResumeRsp = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_SUSPEND_RSP_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstEsmEmmResumeRsp)
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmResumeRsp:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET( NAS_ESM_GET_MSG_ENTITY(pstEsmEmmResumeRsp), 0, NAS_ESM_GET_MSG_LENGTH(pstEsmEmmResumeRsp));

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pstEsmEmmResumeRsp, ID_EMM_ESM_RESUME_RSP);

    /*填写响应结果*/
    pstEsmEmmResumeRsp->enRslt= enRslt;

    /*调用消息发送函数 */
    NAS_ESM_SND_MSG(pstEsmEmmResumeRsp);
}
VOS_VOID NAS_ESM_SndEsmEmmBearerStatusReqMsg(
                    EMM_ESM_BEARER_CNTXT_MOD_ENUM_UINT32 enBearerCntxtChangeMod)
{
    VOS_UINT32                  ulCnt                       = NAS_ESM_NULL;
    VOS_UINT32                  ulEpsbIdNum                 = NAS_ESM_NULL;
    VOS_UINT32                  aulEpsbId[NAS_ESM_MAX_EPSB_NUM]  = {NAS_ESM_NULL};
    EMM_ESM_BEARER_STATUS_REQ_STRU *pSmEmmBearStatusReqMsg   = VOS_NULL_PTR;

    /*找出已激活承载，并记录其ID*/
    for( ulCnt = NAS_ESM_MIN_EPSB_ID; ulCnt <= NAS_ESM_MAX_EPSB_ID; ulCnt++ )
    {
        /*此承载已激活*/
        if( NAS_ESM_BEARER_STATE_ACTIVE == NAS_ESM_GetBearCntxtState(ulCnt) )
        {
            /*保存已激活承载ID,准备发送给RABM*/
            aulEpsbId[ulEpsbIdNum++] = ulCnt;
        }
    }

    /*分配空间并检验分配是否成功*/
    pSmEmmBearStatusReqMsg = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_BEARER_STATUS_REQ_STRU));

    if ( VOS_NULL_PTR == pSmEmmBearStatusReqMsg )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmBearerStatusReqMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(NAS_ESM_GET_MSG_ENTITY(pSmEmmBearStatusReqMsg), 0, \
               NAS_ESM_GET_MSG_LENGTH(pSmEmmBearStatusReqMsg));

    /*此次修改承载上下文的方式*/
    pSmEmmBearStatusReqMsg->enBearerCntxtMod = enBearerCntxtChangeMod;

    /*将ulEpsbIdNum、pulEpsbId填入pSmEmmBearStatusReqMsg*/
    pSmEmmBearStatusReqMsg->ulEpsIdNum = ulEpsbIdNum;
    NAS_ESM_MEM_CPY((VOS_UINT8*)pSmEmmBearStatusReqMsg->aulEsmEpsId, (VOS_UINT8*)aulEpsbId,
               (pSmEmmBearStatusReqMsg->ulEpsIdNum)*(sizeof(VOS_UINT32)/sizeof(VOS_UINT8)));

    /*填写当前是否有紧急PDN连接*/
    pSmEmmBearStatusReqMsg->ulExistEmcPdnFlag = NAS_ESM_HasExistedEmergencyPdn();

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pSmEmmBearStatusReqMsg,ID_EMM_ESM_BEARER_STATUS_REQ);

    /* 调用消息发送函数 */
    NAS_ESM_SND_MSG(pSmEmmBearStatusReqMsg);

}
VOS_VOID NAS_ESM_SndEsmEmmPdnConRspMsg( EMM_ESM_PDN_CON_RSLT_ENUM_UINT32        ulRst,
                                                          VOS_UINT32 ulLength, const VOS_UINT8 *pucSendMsg)
{
    VOS_UINT32                          ulTmpLength       = NAS_ESM_NULL;
    EMM_ESM_PDN_CON_RSP_STRU           *pSmEmmPdnConRsp   = VOS_NULL_PTR;

    /*根据空口消息的长度分配空间，处于消息头外*/
    /* lint -e831 */
    if(ulLength > ESM_MIN_SND_MSG_LEN)
    {
        ulTmpLength = ulLength - ESM_MIN_SND_MSG_LEN ;
        pSmEmmPdnConRsp = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_PDN_CON_RSP_STRU) + ulTmpLength);
    }
    /* lint +e831 */
    else/*如果长度小于ESM_MIN_SND_MSG_LEN，分配的空间等于ESM_MIN_SND_MSG_LEN*/
    {
        pSmEmmPdnConRsp = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_PDN_CON_RSP_STRU));
    }


    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pSmEmmPdnConRsp)
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmPdnConRspMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET( NAS_ESM_GET_MSG_ENTITY(pSmEmmPdnConRsp), 0, NAS_ESM_GET_MSG_LENGTH(pSmEmmPdnConRsp));

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pSmEmmPdnConRsp,ID_EMM_ESM_PDN_CON_RSP);

    /*填写响应结果*/
    pSmEmmPdnConRsp->ulRst = ulRst;

    /*如果响应结果为成功，填写空口消息*/

    if( EMM_ESM_PDN_CON_RSLT_SUCC == ulRst )
    {
        /*填写空口消息头长度*/
        pSmEmmPdnConRsp->stEsmMsg.ulEsmMsgSize = ulLength;

        /*将空口消息存放到EMM_ESM_PDN_CON_RSP_STRU结构中*/
        /*lint -e669*/
        NAS_ESM_MEM_CPY(pSmEmmPdnConRsp->stEsmMsg.aucEsmMsg, pucSendMsg, ulLength);/*lint !e669*/
        /*lint +e669*/
    }


    /*调用消息发送函数 */
    NAS_ESM_SND_MSG(pSmEmmPdnConRsp);

}

/*****************************************************************************
 Function Name   : NAS_ESM_SndEsmEmmEstReqMsg
 Description     : 发送上行空口消息，触发MM发起注册
 Input           : VOS_UINT32 ulLength
                   VOS_UINT8 *pucSendMsg
 Output          : None
 Return          : VOS_VOID

 History         :
    1.sunbing49683      2009-2-28  Draft Enact

*****************************************************************************/

/*lint -e669*/
/*lint -e831*/
VOS_VOID NAS_ESM_SndEsmEmmEstReqMsg(VOS_UINT32 ulOpId,
                                      VOS_UINT8  ucIsEmcPdnType,
                                      VOS_UINT32 ulLength,
                                      const VOS_UINT8 *pucSendMsg)
{
    VOS_UINT32                          ulTmpLength         = NAS_ESM_NULL;
    EMM_ESM_EST_REQ_STRU               *pSmEmmEstReq        = VOS_NULL_PTR;

    /*根据空口消息的长度分配空间*/
    if( NAS_ESM_NULL == ulLength)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_SndEsmEmmEstReqMsg: WARNING: Msg Length is zero");
        return ;
    }
    if(ulLength > ESM_MIN_SND_MSG_LEN)
    {
        ulTmpLength = ulLength - ESM_MIN_SND_MSG_LEN ;
        pSmEmmEstReq = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_EST_REQ_STRU) + ulTmpLength);
    }
    else/*如果长度小于ESM_MIN_SND_MSG_LEN，分配的空间等于ESM_MIN_SND_MSG_LEN*/
    {
        pSmEmmEstReq = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_EST_REQ_STRU));
    }

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pSmEmmEstReq)
    {
         /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmEstReqMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET( NAS_ESM_GET_MSG_ENTITY(pSmEmmEstReq), 0, NAS_ESM_GET_MSG_LENGTH(pSmEmmEstReq));

    /*填写空口消息头长度*/
    pSmEmmEstReq->stEsmMsg.ulEsmMsgSize    = ulLength;

    /*将空口消息存放到EMM_ESM_EST_REQ_STRU结构中*/
    NAS_ESM_MEM_CPY(pSmEmmEstReq->stEsmMsg.aucEsmMsg, pucSendMsg, ulLength);

    /*填写是否是紧急PDN连接请求类型*/
    pSmEmmEstReq->ulIsEmcType = ucIsEmcPdnType;

    /*填写OpId*/
    pSmEmmEstReq->ulOpId = ulOpId;

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pSmEmmEstReq,ID_EMM_ESM_EST_REQ);

    /*调用消息发送函数 */
    NAS_ESM_SND_MSG(pSmEmmEstReq);

}
/*lint +e831*/
/*lint +e669*/


/*****************************************************************************
 Function Name   : NAS_ESM_SndEsmEmmPdnConSuccReqMsg
 Description     : ATTACH过程中，ESM建立缺省承载成功，
                   向EMM返回EMM_ESM_PDN_CON_SUCC_REQ_STRU消息；
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-8-18  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_SndEsmEmmPdnConSuccReqMsg
(
     VOS_UINT32                         ulLength,
     const VOS_UINT8                   *pucSendMsg
)
{
    VOS_UINT32                          ulTmpLength         = NAS_ESM_NULL;
    EMM_ESM_PDN_CON_SUCC_REQ_STRU      *pSmEmmPdnConSucc    = VOS_NULL_PTR;

    /*根据空口消息的长度分配空间*/
    if( NAS_ESM_NULL == ulLength)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_SndEsmEmmPdnConSuccReqMsg: WARNING: Msg Length is zero");
        return ;
    }
    else if(ulLength > ESM_MIN_SND_MSG_LEN)
    {
        ulTmpLength = ulLength - ESM_MIN_SND_MSG_LEN ;
        pSmEmmPdnConSucc = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_PDN_CON_SUCC_REQ_STRU) + ulTmpLength);
    }
    else/*如果长度小于ESM_MIN_SND_MSG_LEN，分配的空间等于ESM_MIN_SND_MSG_LEN*/
    {
        pSmEmmPdnConSucc = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_PDN_CON_SUCC_REQ_STRU));
    }

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pSmEmmPdnConSucc)
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmPdnConSuccReqMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET( NAS_ESM_GET_MSG_ENTITY(pSmEmmPdnConSucc), 0, NAS_ESM_GET_MSG_LENGTH(pSmEmmPdnConSucc));

    /*填写空口消息头长度*/
    pSmEmmPdnConSucc->stEsmMsg.ulEsmMsgSize = ulLength;

    /*将空口消息存放到EMM_ESM_DATA_REQ_STRU结构中*/
    /*lint -e669 */
    NAS_ESM_MEM_CPY(pSmEmmPdnConSucc->stEsmMsg.aucEsmMsg, pucSendMsg, ulLength);/*lint !e669 !e670*/
    /*lint +e669*/

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pSmEmmPdnConSucc,ID_EMM_ESM_PDN_CON_SUCC_REQ);

    /*调用消息发送函数 */
    NAS_ESM_SND_MSG(pSmEmmPdnConSucc);

}


/*****************************************************************************
 Function Name   : NAS_ESM_SndEsmEmmDataReqMsg
 Description     : 向MM发送DATA_REQ消息，携带上行空口消息
 Input           : VOS_UINT32 ulLength
                   VOS_UINT8 *pucSendMsg
 Output          : None
 Return          : VOS_VOID

 History         :
    1.sunbing49683      2009-2-28  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_SndEsmEmmDataReqMsg( VOS_UINT32 ulOpId,
                                       VOS_UINT8 ulIsEmcPdnType,
                                       VOS_UINT32 ulLength,
                                       const VOS_UINT8 *pucSendMsg)
{
    VOS_UINT32                          ulTmpLength         = NAS_ESM_NULL;
    EMM_ESM_DATA_REQ_STRU              *pSmEmmDataReq       = VOS_NULL_PTR;

    /*根据空口消息的长度分配空间*/
    if( NAS_ESM_NULL == ulLength)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_SndEsmEmmDataReqMsg: WARNING: Msg Length is zero");
        return ;
    }
    else if(ulLength > ESM_MIN_SND_MSG_LEN)
    {
        ulTmpLength = ulLength - ESM_MIN_SND_MSG_LEN ;
        pSmEmmDataReq = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_DATA_REQ_STRU) + ulTmpLength);
    }
    else/*如果长度小于ESM_MIN_SND_MSG_LEN，分配的空间等于ESM_MIN_SND_MSG_LEN*/
    {
        pSmEmmDataReq = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_DATA_REQ_STRU));
    }

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pSmEmmDataReq)
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmDataReqMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET( NAS_ESM_GET_MSG_ENTITY(pSmEmmDataReq), 0, NAS_ESM_GET_MSG_LENGTH(pSmEmmDataReq));

    /*填写空口消息头长度*/
    pSmEmmDataReq->stEsmMsg.ulEsmMsgSize = ulLength;

    /*将空口消息存放到EMM_ESM_DATA_REQ_STRU结构中*/
    /*lint -e669*/
    NAS_ESM_MEM_CPY(pSmEmmDataReq->stEsmMsg.aucEsmMsg, pucSendMsg, ulLength);/*lint !e669 !e670*/
    /*lint +e669*/

    /*填写是否是紧急PDN连接请求类型*/
    pSmEmmDataReq->ulIsEmcType = ulIsEmcPdnType;

    /*填写OpId*/
    pSmEmmDataReq->ulOpId = ulOpId;

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pSmEmmDataReq,ID_EMM_ESM_DATA_REQ);

    /*调用消息发送函数 */
    NAS_ESM_SND_MSG(pSmEmmDataReq);

}

#if 0

VOS_VOID NAS_ESM_SndEsmEmmDataReqMsgMultiNwMsg( VOS_VOID )
{
    VOS_UINT32                          ulNwMsgLength   = NAS_ESM_NULL;
    VOS_UINT32                          ulTmpLength     = NAS_ESM_NULL;
    EMM_ESM_DATA_REQ_STRU              *pSmEmmDataReq   = VOS_NULL_PTR;
    NAS_ESM_SND_NW_MSG_STRU            *pstEsmSndNwMsg  = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_SndEsmEmmDataReqMsgMultiNwMsg is entered.");

    /*取得空口消息存放地址和总长度*/
    pstEsmSndNwMsg = NAS_ESM_GetEsmSndNwMsgAddr();
    ulNwMsgLength = pstEsmSndNwMsg->ulNwMsgSize;

    /*如果没有要发送的空口消息，直接返回*/
    if( NAS_ESM_NULL == ulNwMsgLength )
    {
        /*打印信息*/
        NAS_ESM_NORM_LOG("NAS_ESM_SndEsmEmmDataReqMsgMultiNwMsg:NORM:No Msg!");
        return ;
    }

    /*根据空口消息的长度分配空间，处于消息头外*/
    if( ulNwMsgLength > ESM_MIN_SND_MSG_LEN)
    {
        ulTmpLength = ulNwMsgLength - ESM_MIN_SND_MSG_LEN ;
        pSmEmmDataReq = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_DATA_REQ_STRU) + ulTmpLength);
    }
    else/*如果长度小于ESM_MIN_SND_MSG_LEN，分配的空间等于ESM_MIN_SND_MSG_LEN*/
    {
        pSmEmmDataReq = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_DATA_REQ_STRU));
    }

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pSmEmmDataReq)
    {
         /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmDataReqMsgMultiNwMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET( NAS_ESM_GET_MSG_ENTITY(pSmEmmDataReq), 0, NAS_ESM_GET_MSG_LENGTH(pSmEmmDataReq));

    /*填写空口消息头长度*/
    pSmEmmDataReq->stEsmMsg.ulEsmMsgSize = NAS_ESM_GetEsmSndNwMsgLen();

    /*将空口消息存放到EMM_ESM_DATA_REQ_STRU结构中*/
    /*lint -e669*/
    NAS_ESM_MEM_CPY(pSmEmmDataReq->stEsmMsg.aucEsmMsg, pstEsmSndNwMsg->pucNwMsg, ulNwMsgLength);
    /*lint +e669*/

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pSmEmmDataReq,ID_EMM_ESM_DATA_REQ);

    /*调用消息发送函数 */
    NAS_ESM_SND_MSG(pSmEmmDataReq);

}
#endif

/*leili modify for isr begin*/

VOS_VOID NAS_ESM_SndEsmEmmBearerModifyReq( VOS_UINT32 ulEpsId)
{
    EMM_ESM_BEARER_MODIFY_REQ_STRU  *pSmEmmBearModReq   = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    pSmEmmBearModReq = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_BEARER_MODIFY_REQ_STRU));

    if ( VOS_NULL_PTR == pSmEmmBearModReq )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmRelReqMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(NAS_ESM_GET_MSG_ENTITY(pSmEmmBearModReq), 0, NAS_ESM_GET_MSG_LENGTH(pSmEmmBearModReq));

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pSmEmmBearModReq,ID_EMM_ESM_BEARER_MODIFY_REQ);

    pSmEmmBearModReq->ulEpsId = ulEpsId;

    /*调用消息发送函数 */
    NAS_ESM_SND_MSG(pSmEmmBearModReq);

}
/*leili modify for isr end*/


VOS_VOID NAS_ESM_SndEsmEmmRelReqMsg( VOS_VOID )
{
    EMM_ESM_REL_REQ_STRU  *pSmEmmRelReq   = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    pSmEmmRelReq = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_REL_REQ_STRU));

    if ( VOS_NULL_PTR == pSmEmmRelReq )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmRelReqMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(NAS_ESM_GET_MSG_ENTITY(pSmEmmRelReq), 0, NAS_ESM_GET_MSG_LENGTH(pSmEmmRelReq));

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pSmEmmRelReq,ID_EMM_ESM_REL_REQ);

    /*调用消息发送函数 */
    NAS_ESM_SND_MSG(pSmEmmRelReq);

}
VOS_VOID  NAS_ESM_ClearStateTable( VOS_UINT32 ulAppErrType, VOS_UINT32 ulKeepEmc )
{
    APP_ESM_MSG_TYPE_ENUM_UINT32        enAppMsgType        = NAS_ESM_NULL;
    VOS_UINT32                          ulStateTblId        = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG2("NAS_ESM_ClearStateTable is entered.", ulAppErrType, ulKeepEmc);

    /*遍历状态表，如果有APP消息，则回复APP*/
    for( ulStateTblId = 0; ulStateTblId < NAS_ESM_MAX_STATETBL_NUM; ulStateTblId++ )
    {
        pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblId);

        /*如果允许保留紧急承载对应的动态表项，则跳过该项*/
        if ((NAS_ESM_BEARER_TYPE_EMERGENCY == pstStateAddr->enBearerCntxtType) &&
            (PS_TRUE == ulKeepEmc))
        {
			/* lihong00150010 emergency tau&service begin */
            NAS_ESM_INFO_LOG("NAS_ESM_ClearStateTable:stop 3482");

            /* 如果是紧急PDN引起TAU或者SERVICE，失败后需要发起紧急注册的场景，
               需要停止3482定时器，否则紧急注册过程中3482可能超时 */
            NAS_ESM_TimerStop(ulStateTblId, TI_NAS_ESM_T3482);
			/* lihong00150010 emergency tau&service ends */
            continue;
        }

        /* 如果此状态表存有APP消息记录，根据记录回复消息*/
        if( NAS_ESM_OP_TRUE == pstStateAddr->bitOpAppMsgRecord )
        {
            /*取出此承载中记录的APP消息类型*/
            enAppMsgType = pstStateAddr->stAppMsgRecord.enAppMsgType;

            /*根据所记录的消息类型回复消息*/
            switch( enAppMsgType )
            {
                /*如果是ID_APP_ESM_PDP_SETUP_REQ*/

                case ID_APP_ESM_PDP_SETUP_REQ:
                    /*向APP发送ID_APP_ESM_PDP_SETUP_CNF(失败)消息*/

                    NAS_ESM_SndEsmAppSdfSetupCnfFailMsg(    ulStateTblId, \
                                                            ulAppErrType);
                    break;

                case ID_APP_ESM_PDP_RELEASE_REQ:
                    if (APP_ERR_SM_SUSPENDED == ulAppErrType)
                    {
                        /*向APP发送ID_APP_ESM_PDP_RELEASE_CNF(失败)消息*/
                        NAS_ESM_SndEsmAppSdfRelCnfFailMsg(ulStateTblId, ulAppErrType);
                    }
                    else
                    {
                        /*向APP发送ID_APP_ESM_PDP_RELEASE_CNF(成功)消息*/
                        NAS_ESM_SndEsmAppSdfRelCnfSuccMsg(ulStateTblId);
                    }
                    break;

                case ID_APP_ESM_PDP_MODIFY_REQ:
                    /*向APP发送ID_APP_ESM_PDP_MODIFY_CNF(失败)消息*/
                    NAS_ESM_SndEsmAppSdfModCnfFailMsg(  ulStateTblId, \
                                                        ulAppErrType);
                    break;

                case ID_APP_ESM_NDISCONN_REQ:
                    /*向APP发送ID_APP_ESM_NDISCONN_CNF(失败)消息*/
                    NAS_ESM_LOG1("NAS_ESM_ClearStateTable:ERROR =", ulAppErrType);
                    NAS_ESM_SndEsmAppNdisConnCnfFailMsg(    ulStateTblId, \
                                                            ulAppErrType);
                    break;

                default:
                    break;
            }
        }

        NAS_ESM_RelStateTblResource(ulStateTblId);
    }

}


EMM_ESM_UP_SINGNAL_PENDING_ENUM_UINT32 NAS_ESM_IsUpSingnalPending( VOS_VOID )
{
    VOS_UINT32                          ulStateTblIndex = NAS_ESM_NULL;
    VOS_UINT32                          ulTimerMaxExpTimer = NAS_ESM_NULL;
    NAS_ESM_TIMER_STRU                 *pstTimerInfo = VOS_NULL_PTR;

    NAS_ESM_INFO_LOG("NAS_ESM_IsUpSingnalPending is entered.");

    /* 判断是否有流程在等待网侧响应*/
    for (ulStateTblIndex = 0; ulStateTblIndex < NAS_ESM_MAX_STATETBL_NUM; ulStateTblIndex++)
    {
        if (NAS_ESM_BEARER_PROC_TRANS_STATE_PENDING
                == NAS_ESM_GetProcTransState(ulStateTblIndex))
        {
            /* 判断该流程对应定时的超时次数*/
            pstTimerInfo = NAS_ESM_GetStateTblTimer(ulStateTblIndex);

            /* 判断流程对应定时器没有运行，提示警告*/
            if(VOS_NULL_PTR == pstTimerInfo->hTimer)
            {
                NAS_ESM_WARN_LOG("NAS_ESM_IsUpSingnalPending:warn: Timer not running.");
                continue;
            }

            /* 获取流程对应定时器的最大超时次数*/
            ulTimerMaxExpTimer = NAS_ESM_GetTimerMaxExpNum(pstTimerInfo->enPara);

            /* 如果超时次数小于最大次数减1，返回存在上行信令阻塞*/
            if((pstTimerInfo->ucExpireTimes) < (ulTimerMaxExpTimer-1))
            {
                NAS_ESM_INFO_LOG("NAS_ESM_IsUpSingnalPending:Uplink singnal pending.");
                return EMM_ESM_UP_SINGNAL_PENDING;
            }
            else
            {
                /*定时器超时次数已经超过最大次数减1，不认为上行信令阻塞*/
                NAS_ESM_INFO_LOG("NAS_ESM_IsUpSingnalPending:ExpireTimex >= Max-1");
                continue;
            }
        }
    }

    NAS_ESM_INFO_LOG("NAS_ESM_IsUpSingnalPending:no uplink signal pending.");
    return EMM_ESM_NO_UP_SINGNAL_PENDING;
}

VOS_UINT32 NAS_ESM_GetRegistCid( VOS_VOID )
{
    VOS_UINT32                          ulCur = NAS_ESM_ILL_CID;
    NAS_ESM_SDF_PARA_STRU              *pstSdfPara  = VOS_NULL_PTR;

    /* 首先判断CID0是否为缺省承载 */
    pstSdfPara = NAS_ESM_GetSdfParaAddr(NAS_ESM_NULL);
    if (NAS_ESM_BEARER_TYPE_DEFAULT == pstSdfPara->enBearerCntxtType)
    {
       return NAS_ESM_NULL;
    }

    /* 从CID11-CID1寻找缺省承载类型的CID */
    for (ulCur = 1; ulCur < NAS_ESM_CID_NV_NUM; ulCur++ )
    {
       pstSdfPara = NAS_ESM_GetSdfParaAddr(ulCur);

       if (NAS_ESM_BEARER_TYPE_DEFAULT == pstSdfPara->enBearerCntxtType)
       {
           return ulCur;
       }
    }

    /* 如果之前的所有CID都不满足条件，则从CID12-CID20寻找缺省承载类型的CID */
    for (ulCur = NAS_ESM_CID_NV_NUM; ulCur < NAS_ESM_MAX_UE_CONTRUL_CID_NUM; ulCur++ )
    {
       pstSdfPara = NAS_ESM_GetSdfParaAddr(ulCur);

       if (NAS_ESM_BEARER_TYPE_DEFAULT == pstSdfPara->enBearerCntxtType)
       {
           return ulCur;
       }
    }

    return NAS_ESM_ILL_CID;
}
VOS_UINT32 NAS_ESM_ValidCidAttachBearerReest(VOS_UINT32 ulCid)
{
    VOS_UINT32 ulEpsbId = 0;
    VOS_UINT32 ulStateTblId = 0;
    VOS_UINT32 ulResult = PS_FALSE;

    if (NAS_ESM_FAILURE == NAS_ESM_QueryEpsbCntxtTblByCid(ulCid, &ulEpsbId))
    {
        if (NAS_ESM_FAILURE == NAS_ESM_QueryStateTblIndexByCid(ulCid, &ulStateTblId))
        {
            ulResult = PS_TRUE;
        }
    }
    else
    {
        if (NAS_ESM_BEARER_STATE_INACTIVE == NAS_ESM_GetBearCntxtState(ulEpsbId))
        {
            ulResult = PS_TRUE;
        }
    }

    return ulResult;
}


VOS_VOID NAS_ESM_ReestablishAttachBearer( VOS_VOID )
{
     VOS_UINT32                          ulPti               = NAS_ESM_NULL;
     VOS_UINT32                          ulRegCId            = NAS_ESM_NULL;
     VOS_UINT32                          ulStateTblId        = NAS_ESM_NULL;
     VOS_UINT32                          ulCur               = NAS_ESM_NULL;
     VOS_UINT32                          ulSdfNum            = NAS_ESM_NULL;
     NAS_ESM_SDF_PARA_STRU              *pstSdfPara          = VOS_NULL_PTR;
     APP_ESM_SDF_PARA_STRU              *pstAppSdfPara       = VOS_NULL_PTR;
     NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;
     NAS_ESM_ENCODE_INFO_STRU            stEncodeInfo;
     NAS_ESM_PDP_MANAGE_INFO_STRU       *pEpsbManageInfo     = VOS_NULL_PTR;


    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_ReestablishAttachBearer is entered.");

    if (NAS_ESM_L_MODE_STATUS_NORMAL != NAS_ESM_GetLModeStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ReestablishAttachBearer: L mode is already suspended!");
        return;
    }

    if (NAS_ESM_PS_REGISTER_STATUS_ATTACHED != NAS_ESM_GetEmmStatus())
    {
        NAS_ESM_NORM_LOG("NAS_ESM_ReestablishAttachBearer: ESM status is not ATTACHED !");
        return;
    }

    pstAppSdfPara = NAS_ESM_MEM_ALLOC(sizeof(APP_ESM_SDF_PARA_STRU));

    if (VOS_NULL_PTR == pstAppSdfPara)
    {
         /*打印不合法信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_ReestablishAttachBearer:ERROR: Mem alloc fail!");
        return;
    }

    /* 逐一获取12套NV参数 */
    for (ulCur = NAS_ESM_MIN_CID; ulCur < NAS_ESM_CID_NV_NUM; ulCur++)
    {
        pstAppSdfPara->ulCId = ulCur;
        /*PC REPLAY MODIFY BY LEILI BEGIN*/
        if (APP_FAILURE == NAS_ESM_GetSdfPara(&ulSdfNum, pstAppSdfPara))
        {
            continue;
        }

        pstSdfPara = NAS_ESM_GetSdfParaAddr(ulCur);
        NAS_ESM_MEM_CPY(pstSdfPara, pstAppSdfPara, sizeof(APP_ESM_SDF_PARA_STRU));
    }

    NAS_ESM_MEM_FREE(pstAppSdfPara);

    /* 从APP获取承载管理模式参数，则采取自动接受模式 */
    pEpsbManageInfo = NAS_ESM_GetBearerManageInfoAddr();
    if (APP_FAILURE == NAS_ESM_GetPdpManageInfo(pEpsbManageInfo))
    {
        /*打印警告信息*/
        NAS_ESM_WARN_LOG("NAS_ESM_ReestablishAttachBearer:WARN: Fail to get PDP Manage Info from APP!");
        pEpsbManageInfo->enMode = APP_ESM_BEARER_MANAGE_MODE_AUTO;
        pEpsbManageInfo->enType = APP_ESM_BEARER_MANAGE_TYPE_ACCEPT;
    }
    /*PC REPLAY MODIFY BY LEILI END*/
    /* 获取用于注册的CID，优先级顺序由高到低为CID0 > CID1-CID11 > CID12-CID20 */
    /* 除了个别运营商特殊需求, 需要GU NV项CID0初始化为专有类型，CID11为缺省类型*/
    ulRegCId = NAS_ESM_GetRegistCid();
    if (NAS_ESM_ILL_CID == ulRegCId)
    {
        /*打印不合法信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_ReestablishAttachBearer: There is no default bearer type sdf");
        return ;
    }

    NAS_ESM_LOG1("NAS_ESM_ReestablishAttachBearer:The cid of the found sdf is :", ulRegCId);

    /* 检查CID是否符合注册承载重建的条件 */
    if (PS_FALSE == NAS_ESM_ValidCidAttachBearerReest(ulRegCId))
    {
        NAS_ESM_ERR_LOG("NAS_ESM_ReestablishAttachBearer: Invalid CID");
        return;
    }

    /*在动态表中，获取过程标识*/
    if(NAS_ESM_SUCCESS != NAS_ESM_GetStateTblIndexByCid(NAS_ESM_ATTACH_CID, &ulStateTblId))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ReestablishAttachBearer:WARNING:insufficient resource!");
        return ;
    }

    /*将查询到的ulRegCId参数拷贝到CID0中，发起注册*/
    pstSdfPara = NAS_ESM_GetSdfParaAddr(NAS_ESM_ATTACH_CID);
    NAS_ESM_MEM_CPY(    pstSdfPara,
                        NAS_ESM_GetSdfParaAddr(ulRegCId),
                        sizeof(NAS_ESM_SDF_PARA_STRU));

    /*设置初始状态*/
    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblId);
    pstStateAddr->enBearerCntxtState = NAS_ESM_BEARER_STATE_INACTIVE;
    pstStateAddr->enBearerCntxtType  = NAS_ESM_BEARER_TYPE_DEFAULT;

    /*分配PTI,并存存储到承载信息中*/
    NAS_ESM_AssignPTI(&ulPti);
    pstStateAddr->stNwMsgRecord.ucPti = (VOS_UINT8)ulPti;

    /*填充NAS_ESM_ENCODE_INFO_STRU*/
    NAS_ESM_MEM_SET((VOS_VOID*)&stEncodeInfo, 0, sizeof(NAS_ESM_ENCODE_INFO_STRU));
    stEncodeInfo.bitOpESMCau        = NAS_ESM_OP_FALSE;
    stEncodeInfo.ulCid              = pstStateAddr->ulCid;
    stEncodeInfo.ucPti              = (VOS_UINT8)ulPti;
    stEncodeInfo.ulEpsbId           = pstStateAddr->ulEpsbId;
    stEncodeInfo.enPdnReqType       = pstStateAddr->enPdnReqType;

    /*组装PDN_Connect_Req消息*/
    if( NAS_ESM_FAILURE == NAS_ESM_EncodePdnConReqMsg(stEncodeInfo, \
                                                      pstStateAddr->stNwMsgRecord.aucMsgBuff,\
                                                      &pstStateAddr->stNwMsgRecord.ulMsgLength))
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_ReestablishAttachBearer:ERROR:EncodePdnConReqMsg fail!");

        NAS_ESM_RelStateTblResource(ulStateTblId);
        return ;
    }

    /*记录空口消息类型*/
    pstStateAddr->bitOpNwMsgRecord = NAS_ESM_OP_TRUE;
    pstStateAddr->stNwMsgRecord.enEsmCnMsgType = NAS_ESMCN_MSG_TYPE_PDN_CONNECT_REQ;
    pstStateAddr->stNwMsgRecord.ucIsEmcPdnReq = PS_FALSE;
    pstStateAddr->stNwMsgRecord.ulOpId = pstStateAddr->ulCid;

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
    NAS_ESM_TimerStart(ulStateTblId,TI_NAS_ESM_T3482);

    /*转换状态*/
    NAS_ESM_SetProcTransState(ulStateTblId, NAS_ESM_BEARER_PROC_TRANS_STATE_PENDING);
}
VOS_VOID NAS_ESM_RcvEsmEmmDeactAllNonEmcBearerInd(VOS_VOID)
{
    NAS_ESM_INFO_LOG("NAS_ESM_RcvEsmEmmDeactAllNonEmcBearerInd: enter");

    if (PS_TRUE == NAS_ESM_HasExistedEmergencyPdn())
    {
        NAS_ESM_INFO_LOG("NAS_ESM_RcvEsmEmmDeactAllNonEmcBearerInd: existed emergency PDN");

        /*如果当前有已经建立的紧急PDN连接，则立即释放所有非紧急PDN连接*/
        NAS_ESM_DeactAllNonEmcPdn();
    }
    else if (PS_TRUE == NAS_ESM_HasEstingEmergencyPdn())
    {
        NAS_ESM_INFO_LOG("NAS_ESM_RcvEsmEmmDeactAllNonEmcBearerInd: esting emergency PDN");

        /*如果当前有正在建立的紧急PDN连接，则延迟释放所有非紧急PDN连接*/
        NAS_ESM_TimerStart(0, TI_NAS_ESM_REL_NON_EMC_BEARER);
    }
    else
    {   /*如果当前没有紧急PDN，则不做处理*/
        NAS_ESM_INFO_LOG("NAS_ESM_RcvEsmEmmDeactAllNonEmcBearerInd: no emergency PDN");
    }

    return;
}
VOS_VOID NAS_ESM_SndEsmEmmClrEsmProcResNtyMsg(VOS_UINT32 ulOpId, VOS_UINT8 ucIsEmcPdnType)
{
    EMM_ESM_CLR_ESM_PROC_RES_NOTIFY_STRU *pSmEmmNotify      = VOS_NULL_PTR;

    NAS_ESM_INFO_LOG2("NAS_ESM_SndEsmEmmClrEsmProcResNtyMsg: enter", ulOpId, ucIsEmcPdnType);

    /*根据空口消息的长度分配空间*/
    pSmEmmNotify= (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(EMM_ESM_CLR_ESM_PROC_RES_NOTIFY_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pSmEmmNotify)
    {
         /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmEmmClrEsmProcResNtyMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET( NAS_ESM_GET_MSG_ENTITY(pSmEmmNotify), 0, NAS_ESM_GET_MSG_LENGTH(pSmEmmNotify));

    /*填写是否是紧急PDN连接请求类型*/
    pSmEmmNotify->ulIsEmcType = ucIsEmcPdnType;

    /*填写OpId*/
    pSmEmmNotify->ulOpId = ulOpId;

    /*填写消息头*/
    NAS_ESM_WRITE_EMM_MSG_HEAD(pSmEmmNotify, ID_EMM_ESM_CLR_ESM_PROC_RES_NOTIFY);

    /*调用消息发送函数 */
    NAS_ESM_SND_MSG(pSmEmmNotify);
}
/*lint +e961*/
/*lint +e960*/



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of NasEsmEmmMsgProc.c */
