

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "NasEmmTauSerInclude.h"
#include  "NasLmmPubMPrint.h"
#include  "EmmTcInterface.h"
#include  "NasLmmPubMOm.h"
/* lihong00150010 emergency tau&service begin */
#include  "NasEmmAttDetInclude.h"
/* lihong00150010 emergency tau&service end */

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMSERVICEPROC_C
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
NAS_EMM_SER_CTRL_STRU                   g_stEmmSERCtrlTbl;
NAS_EMM_SER_ESM_MSG_BUF_STRU            g_stEmmEsmMsgBuf;


/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_EMM_SER_Init(VOS_VOID)
{
    NAS_EMM_SER_LOG_NORM("NAS_EMM_SER_Init                   START INIT...");

    NAS_LMM_MEM_SET(&g_stEmmSERCtrlTbl, 0, sizeof(g_stEmmSERCtrlTbl));

    NAS_LMM_MEM_SET(&g_stEmmEsmMsgBuf, 0, sizeof(g_stEmmEsmMsgBuf));

    return;
}


VOS_VOID  NAS_EMM_SER_ClearResource( VOS_VOID )
{
    NAS_EMM_SER_LOG_NORM("NAS_EMM_SER_ClearResource entered.");

    /* 清空SER全局变量*/
    NAS_LMM_MEM_SET(&g_stEmmSERCtrlTbl, 0, sizeof(g_stEmmSERCtrlTbl));

    /* 释放缓存的ESM消息*/
    NAS_EMM_SerClearEsmDataBuf();

    /* 清空接入Bar信息*/
    NAS_EMM_ClearBarResouce();

    /* 停止Service流程相关的所有定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

    return;
}


VOS_VOID  NAS_EMM_FreeSerDyn( VOS_VOID )
{
    /* 释放SERVICE模块缓存的ESM的消息 */
    NAS_EMM_SerClearEsmDataBuf();

    return;
}
VOS_VOID NAS_EMM_SER_CompCnServiceReq(  LRRC_LNAS_MSG_STRU   *pNasMsg)
{
    VOS_UINT32                          ulIndex     = 0;
    VOS_UINT8                           ucKsiSn;
    VOS_UINT8                           ucKsi;
    VOS_UINT8                           ucSequenceNum;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO("NAS_EMM_SER_CompCnServiceReq is entered.");

    /* 清空将填充CN消息的空间 */
    /* xiongxianghui00253310 delete memset 2013-11-26 begin */
    /* NAS_LMM_MEM_SET(pNasMsg->aucNasMsg, 0, pNasMsg->ulNasMsgSize); */
    /* xiongxianghui00253310 delete memset 2013-11-26 end   */

    /* 填充 Security header type  +  Protocol discriminator */
    pNasMsg->aucNasMsg[ulIndex++]       = EMM_CN_SECURITY_HEADER_TYPE_SER_REQ;

    /* 计算 KsiSn*/
    ucKsi = NAS_EMM_SecuGetKSIValue();
    ucSequenceNum = (VOS_UINT8)NAS_EMM_SecuGetUlNasCountVAlue();
    ucSequenceNum = ucSequenceNum & EMM_TAUSER_LOW5_BITS;
    ucKsiSn = ((VOS_UINT8)(ucKsi << 5)) | ucSequenceNum;


    /* 填充 KSI and sequence number   */
    pNasMsg->aucNasMsg[ulIndex++]       = ucKsiSn;


    /* 填充 Message authentication code */
    /*这里只需要初始化为0，在完整性保护时会填充这两个字节*/
    pNasMsg->aucNasMsg[ulIndex++]       = 0;
    pNasMsg->aucNasMsg[ulIndex]         = 0;
    return;
}

/*lint -e669*/

VOS_VOID    NAS_EMM_SER_SendMrrcDataReq_ServiceReq()
{
    NAS_EMM_MRRC_DATA_REQ_STRU          *pMrrcDataReqMsg    = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulDataReqMsgLenNoHeader;
    VOS_UINT32                          ulCnMsgLen;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_SER_SendMrrcDataReq_ServiceReq is entered.");

    /* 计算NAS_MSG_STRU消息长度 */
    ulCnMsgLen                              = NAS_EMM_COUNT_CN_SER_REQ_LEN;
    ulDataReqMsgLenNoHeader                 = EMM_COUNT_DATA_REQ_LEN(ulCnMsgLen);

    /* 申请DOPRA消息 */
    pMrrcDataReqMsg = (VOS_VOID *)NAS_LMM_MEM_ALLOC(NAS_EMM_INTRA_MSG_MAX_SIZE);

    if(NAS_EMM_NULL_PTR == pMrrcDataReqMsg)
    {
        NAS_EMM_SER_LOG_ERR( "Nas_Emm_SER_SendMrrcDataReq_ServiceReq: MSG ALLOC ERR !!");
        return;
    }

    /* xiongxianghui00253310 add memset 2013-11-26 begin */
    NAS_LMM_MEM_SET(pMrrcDataReqMsg, 0, NAS_EMM_INTRA_MSG_MAX_SIZE);
    /* xiongxianghui00253310 add memset 2013-11-26 end   */

    /* 清除CSFB ABORT标识 */
    NAS_EMM_SER_SaveEmmSerCsfbAbortFlag(NAS_EMM_CSFB_ABORT_FLAG_INVALID);
    /* 填写MRRC_DATA_REQ 的DOPRA消息头 */
    EMM_COMP_MM_MSG_HEADER(pMrrcDataReqMsg, ulDataReqMsgLenNoHeader);

    /* 填写MRRC_DATA_REQ 的消息ID标识 */
    pMrrcDataReqMsg->ulMsgId                = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;

    /*填写建立原因*/
    switch(NAS_EMM_SER_GetEmmSERStartCause())
    {
        case NAS_EMM_SER_START_CAUSE_RRC_PAGING:
            pMrrcDataReqMsg->enEstCaue = LRRC_LNAS_EST_CAUSE_MT_ACCESS;
            pMrrcDataReqMsg->enCallType = LRRC_LNAS_CALL_TYPE_TERMINATING_CALL;
            break;

        case NAS_EMM_SER_START_CAUSE_RABM_REEST:
            pMrrcDataReqMsg->enEstCaue = LRRC_LNAS_EST_CAUSE_MO_DATA;
            pMrrcDataReqMsg->enCallType = LRRC_LNAS_CALL_TYPE_ORIGINATING_CALL;
            break;

        case NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ:
            pMrrcDataReqMsg->enEstCaue = LRRC_LNAS_EST_CAUSE_MO_DATA;
            pMrrcDataReqMsg->enCallType = LRRC_LNAS_CALL_TYPE_ORIGINATING_CALL;
            break;

        case NAS_ESM_SER_START_CAUSE_UPLINK_PENDING:
            pMrrcDataReqMsg->enEstCaue = LRRC_LNAS_EST_CAUSE_MO_DATA;
            pMrrcDataReqMsg->enCallType = LRRC_LNAS_CALL_TYPE_ORIGINATING_CALL;
            break;

        case NAS_EMM_SER_START_CAUSE_SMS_EST_REQ:
            pMrrcDataReqMsg->enEstCaue = LRRC_LNAS_EST_CAUSE_MO_DATA;
            pMrrcDataReqMsg->enCallType = LRRC_LNAS_CALL_TYPE_ORIGINATING_CALL;
            break;

        /* lihong00150010 emergency tau&service begin */
        case NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC:
            pMrrcDataReqMsg->enEstCaue = LRRC_LNAS_EST_CAUSE_EMERGENCY_CALL;
            pMrrcDataReqMsg->enCallType = LRRC_LNAS_CALL_TYPE_EMERGENCY_CALL;
            break;

        case NAS_EMM_SER_START_CAUSE_RABM_REEST_EMC:
            pMrrcDataReqMsg->enEstCaue = LRRC_LNAS_EST_CAUSE_EMERGENCY_CALL;
            pMrrcDataReqMsg->enCallType = LRRC_LNAS_CALL_TYPE_EMERGENCY_CALL;
            break;
        /* lihong00150010 emergency tau&service end */

        default:
            NAS_EMM_SER_LOG_ERR("Nas_Emm_SER_SendMrrcDataReq_ServiceReq:StartCause ERR");
            break;
    }
    pMrrcDataReqMsg->enEmmMsgType       = NAS_EMM_MSG_SERVICE_REQ;

    /* 填写MRRC_DATA_REQ 的NAS消息长度标识 */
    pMrrcDataReqMsg->stNasMsg.ulNasMsgSize  = ulCnMsgLen;

    pMrrcDataReqMsg->enDataCnf              = LRRC_LMM_DATA_CNF_NEED;

    /* 构造MRRC_DATA_REQ 中的 NAS_MSG_STRU,即CN消息(Compose the msg of): SER_REQ */
    NAS_EMM_SER_CompCnServiceReq(               &(pMrrcDataReqMsg->stNasMsg));

    /*空口消息上报SER REQ*/
    NAS_LMM_SendOmtAirMsg(NAS_EMM_OMT_AIR_MSG_UP, NAS_EMM_SER_REQ,  (NAS_MSG_STRU*)&(pMrrcDataReqMsg->stNasMsg));

    /*关键事件上报SER REQ*/
    NAS_LMM_SendOmtKeyEvent(         EMM_OMT_KE_SER_REQ);

    /* 发送消息(Send the msg of) MRRC_DATA_REQ(SER_REQ) */
    NAS_EMM_SndUplinkNasMsg((VOS_VOID*)pMrrcDataReqMsg);

    NAS_LMM_MEM_FREE(pMrrcDataReqMsg);

    return;
}
/*lint +e669*/


/*****************************************************************************
 Function Name   : NAS_EMM_CompCnExtendedSerReqMoNasMsg
 Description     : 组装EXTENDED SERVICE REQ消息
 Input           : pulIndex
 Output          : pucCnMsg
                   pulIndex
 Return          : VOS_VOID

 History         :
    1.lihong      2012-02-23  Draft Enact

*****************************************************************************/
VOS_VOID    NAS_EMM_CompCnExtendedSerReqMoNasMsg
(
    VOS_UINT8                          *pucCnMsg,
    VOS_UINT32                         *pulIndex
)
{
    VOS_UINT32                          ulIndex      = NAS_EMM_NULL;
    VOS_UINT8                           ucKsi        = NAS_EMM_NULL;
    NAS_LMM_PUB_INFO_STRU              *pstEmmInfo   = VOS_NULL_PTR;
    VOS_UINT32                          ulIeLength          = 0;

    ulIndex                 = *pulIndex;
    pstEmmInfo              = NAS_LMM_GetEmmInfoAddr();

    /* 填充 Protocol Discriminator + Security header type*/
    pucCnMsg[ulIndex++]     = EMM_CN_MSG_PD_EMM;

    /* 填充 Detach request message identity */
    pucCnMsg[ulIndex++]     = NAS_EMM_CN_MT_EXT_SER_REQ;

    /* 填充service type */
    if (NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
    {
        pucCnMsg[ulIndex]   = MMC_LMM_SERVICE_MO_CSFB_1XCSFB;
    }
    else if (NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
    {
        pucCnMsg[ulIndex]   = MMC_LMM_SERVICR_MO_CSFB_1XCSFB_EMERGENCY;
    }
    else
    {
        pucCnMsg[ulIndex]   = MMC_LMM_SERVICR_MT_CSFB_1XCSFB;
    }

    /*填充KSIasme*/
    ucKsi = NAS_EMM_SecuGetKSIValue();
    pucCnMsg[ulIndex++]     |= (ucKsi & 0x0F)<<4;

    /*填充M-TSMI*/

    /* 填写LENGTH为5 */
    pucCnMsg[ulIndex++]     = 5;

    /*  填写TYPE OF IDENTITY，和EVEN/ODD标识，根据协议前4BIT为'1111' */
    pucCnMsg[ulIndex]       = NAS_EMM_TYPE_OF_IDENTITY_TMSI_PTMSI_MTMSI;
    pucCnMsg[ulIndex++]     |= NAS_EMM_HIGH_4_BIT;

    NAS_LMM_MEM_CPY(        &(pucCnMsg[ulIndex]),
                            &pstEmmInfo->stMmUeId.stGuti.stMTmsi,
                            sizeof(NAS_EMM_MTMSI_STRU));
    ulIndex                 += sizeof(NAS_EMM_MTMSI_STRU);

    /* 只有MT CSFB才填充CSFB RESPONSE */
    if (NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
    {
        pucCnMsg[ulIndex++] = NAS_EMM_EXTENDED_SER_REQ_CSFB_RSP_IEI | NAS_EMM_SER_GetEmmSerCsfbRsp();
    }

    /* 填充EPS bearer context status */
    /*leili modify for isr begin*/
    NAS_EMM_EncodeEpsBearStatus(        &(pucCnMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;
    /*leili modify for isr end*/
    #if 0
    pucCnMsg[ulIndex++]     = NAS_EMM_TAU_REQ_EPS_STATUS_IEI;
    pucCnMsg[ulIndex++]     = NAS_EMM_CN_EPS_CONTEXT_LEN;

    for(ulLoop = 0; ulLoop < NAS_EMM_GetEpsIdNum(); ulLoop++)
    {
        ucEsmEPSNum         = (VOS_UINT8)(NAS_EMM_GetEpsId(ulLoop));
        if( NAS_EMM_EPS_BEARER_STATUS_NUM_SEVEN < ucEsmEPSNum)
        {
            ucEsmEPSNum     = ucEsmEPSNum - NAS_EMM_EPS_BEARER_STATUS_NUM_EIGNT;
            ucEsmEPS2       = ucEsmEPS2 | NAS_EMM_TAU_ESM_BEARER_LEFT_N(ucEsmEPSNum);
        }
        else
        {
            ucEsmEPS1       = ucEsmEPS1 | NAS_EMM_TAU_ESM_BEARER_LEFT_N(ucEsmEPSNum);
        }
    }
    pucCnMsg[ulIndex++]     = ucEsmEPS1;
    pucCnMsg[ulIndex++]     = ucEsmEPS2;
    #endif
    *pulIndex               = ulIndex;

    return;

}
VOS_VOID    NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq(VOS_VOID)
{
    NAS_EMM_MRRC_DATA_REQ_STRU         *pMrrcDataReqMsg         = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulDataReqMsgLenNoHeader = NAS_EMM_NULL;
    VOS_UINT32                          ulIndex                 = NAS_EMM_NULL;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq is entered.");

    /*以最小消息长度，申请消息内存,主要是看是否队列有空间*/
    pMrrcDataReqMsg = (VOS_VOID *) NAS_LMM_MEM_ALLOC(NAS_EMM_INTRA_MSG_MAX_SIZE);

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pMrrcDataReqMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq: MSG ALLOC ERR!");
        return;

    }

    /* 清除CSFB ABORT标识 */
    NAS_EMM_SER_SaveEmmSerCsfbAbortFlag(NAS_EMM_CSFB_ABORT_FLAG_INVALID);

    /*组装EXTENDED SERVICE REQ消息*/
    NAS_EMM_CompCnExtendedSerReqMoNasMsg(pMrrcDataReqMsg->stNasMsg.aucNasMsg, &ulIndex);

    /*内部消息长度计算*/
    ulDataReqMsgLenNoHeader = NAS_EMM_CountMrrcDataReqLen(ulIndex);

    if ( NAS_EMM_INTRA_MSG_MAX_SIZE < ulDataReqMsgLenNoHeader )
    {
        /* 打印错误信息 */
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq, Size error");
        NAS_LMM_MEM_FREE(pMrrcDataReqMsg);
        return ;
    }

    /*填充消息长度*/
    pMrrcDataReqMsg->stNasMsg.ulNasMsgSize = ulIndex;

    /*填充消息头*/
    EMM_COMP_MM_MSG_HEADER(pMrrcDataReqMsg, ulDataReqMsgLenNoHeader);

    /* 填写MRRC_DATA_REQ 的消息ID标识 */
    pMrrcDataReqMsg->ulMsgId            = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;
    /*填写建立原因*/
    switch(NAS_EMM_SER_GetEmmSERStartCause())
    {
        case NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ:
            pMrrcDataReqMsg->enEstCaue  = LRRC_LNAS_EST_CAUSE_MT_ACCESS;
            pMrrcDataReqMsg->enCallType = LRRC_LNAS_CALL_TYPE_TERMINATING_CALL;
            break;

        case NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ:
            pMrrcDataReqMsg->enEstCaue  = LRRC_LNAS_EST_CAUSE_MO_DATA;
            pMrrcDataReqMsg->enCallType = LRRC_LNAS_CALL_TYPE_MO_CSFB;
            break;

        case NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ:
            pMrrcDataReqMsg->enEstCaue  = LRRC_LNAS_EST_CAUSE_EMERGENCY_CALL;
            pMrrcDataReqMsg->enCallType = LRRC_LNAS_CALL_TYPE_EMERGENCY_CALL;
            break;

        default:
            NAS_EMM_SER_LOG_ERR("Nas_Emm_SER_SendMrrcDataReq_ServiceReq:StartCause ERR");
            break;
    }
    pMrrcDataReqMsg->enEmmMsgType       = NAS_EMM_MSG_EXTENDED_SERVICE_REQ;

    pMrrcDataReqMsg->enDataCnf          = LRRC_LMM_DATA_CNF_NEED;

    /*空口消息上报SER REQ*/
    NAS_LMM_SendOmtAirMsg(NAS_EMM_OMT_AIR_MSG_UP, NAS_EMM_EXTENDED_SER_REQ,  (NAS_MSG_STRU*)&(pMrrcDataReqMsg->stNasMsg));

    /*关键事件上报SER REQ*/
    NAS_LMM_SendOmtKeyEvent(            EMM_OMT_KE_EXTENDED_SER_REQ);

    /* 发送消息(Send the msg of) MRRC_DATA_REQ(SER_REQ) */
    NAS_EMM_SndUplinkNasMsg(             pMrrcDataReqMsg);

    NAS_LMM_MEM_FREE(pMrrcDataReqMsg);

    return;
}


VOS_VOID    NAS_EMM_SER_SendMrrcDataReq_ESMdata
(
    EMM_ESM_MSG_STRU                   *pstEsmMsg
)
{
    VOS_UINT32                          ulMsgLenthNoHeader;
    NAS_EMM_MRRC_DATA_REQ_STRU          *pMrrcDataReqMsg    = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG1_INFO( "NAS_EMM_SER_SendMrrcDataReq_ESMdata is entered.", pstEsmMsg);

    /*申请消息*/
    pMrrcDataReqMsg = (VOS_VOID *)NAS_LMM_MEM_ALLOC(NAS_EMM_INTRA_MSG_MAX_SIZE);
    if ( NAS_EMM_NULL_PTR == pMrrcDataReqMsg )
    {
        NAS_EMM_SER_LOG_ERR( "Nas_Emm_SER_SendMrrcDataReq_ESMdata: NAS_LMM_ALLOC_MSG err !!");
        return;
    }

    ulMsgLenthNoHeader                  = NAS_EMM_CountMrrcDataReqLen(pstEsmMsg->ulEsmMsgSize);

    /* 拷贝消息 */
    pMrrcDataReqMsg->stNasMsg.ulNasMsgSize = pstEsmMsg->ulEsmMsgSize;

    NAS_LMM_MEM_CPY(                    (pMrrcDataReqMsg->stNasMsg.aucNasMsg),
                                        (pstEsmMsg->aucEsmMsg),
                                        (pstEsmMsg->ulEsmMsgSize));
    /* 改写DOPRA消息头 */
    EMM_COMP_MM_MSG_HEADER(             (pMrrcDataReqMsg),
                                        ulMsgLenthNoHeader);

    /*改写消息ID*/
    pMrrcDataReqMsg->ulMsgId            = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;

    /*填写 EST CAUSE*/
    pMrrcDataReqMsg->enEstCaue          = LRRC_LNAS_EST_CAUSE_MO_DATA;
    pMrrcDataReqMsg->enCallType         = LRRC_LNAS_CALL_TYPE_ORIGINATING_CALL;
    pMrrcDataReqMsg->enEmmMsgType       = NAS_EMM_MSG_ESM;

    pMrrcDataReqMsg->enDataCnf          = LRRC_LMM_DATA_CNF_NEED;

    /* 向MRRC转发MMESM_DATA_REQ消息 */
    NAS_EMM_SndUplinkNasMsg(             pMrrcDataReqMsg);

    NAS_LMM_MEM_FREE(pMrrcDataReqMsg);
    return;
}

/*******************************************************************************
  Module   :
  Function : NAS_EMM_SER_SendMrrcDataReq_Tcdata
  Input    : pMsg------------TC消息
  Output   :
  NOTE     : 向RRC转发TC_DATA_REQ消息
  Return   : VOS_VOID
  History  :
      1.lihong00150010      2009-10-16  Draft Enact
*******************************************************************************/
VOS_VOID    NAS_EMM_SER_SendMrrcDataReq_Tcdata
(
    EMM_ETC_DATA_REQ_STRU               *pMsg
)
{
    VOS_UINT32                          ulMsgLenthNoHeader;
    NAS_EMM_MRRC_DATA_REQ_STRU         *pMrrcDataReqMsg    = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG1_INFO( "NAS_EMM_SER_SendMrrcDataReq_Tcdata is entered.", pMsg);


    /*申请消息*/
    pMrrcDataReqMsg = (VOS_VOID *)NAS_LMM_MEM_ALLOC(NAS_EMM_INTRA_MSG_MAX_SIZE);
    if ( NAS_EMM_NULL_PTR == pMrrcDataReqMsg )
    {
        NAS_EMM_SER_LOG_ERR( "NAS_EMM_SER_SendMrrcDataReq_Tcdata: NAS_LMM_ALLOC_MSG err !!");
        return;
    }
    ulMsgLenthNoHeader                  = NAS_EMM_CountMrrcDataReqLen(pMsg->stTcMsg.ulTcMsgSize);

    /* 拷贝消息 */
    pMrrcDataReqMsg->stNasMsg.ulNasMsgSize = pMsg->stTcMsg.ulTcMsgSize;

    NAS_LMM_MEM_CPY(                        (pMrrcDataReqMsg->stNasMsg.aucNasMsg),
                                        (pMsg->stTcMsg.aucTcMsg),
                                        (pMsg->stTcMsg.ulTcMsgSize));
    /* 改写DOPRA消息头 */
    EMM_COMP_MM_MSG_HEADER(             (pMrrcDataReqMsg),
                                        ulMsgLenthNoHeader);

    /*改写消息ID*/
    pMrrcDataReqMsg->ulMsgId            = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;

    /*填写 EST CAUSE*/
    pMrrcDataReqMsg->enEstCaue          = LRRC_LNAS_EST_CAUSE_MO_DATA;
    pMrrcDataReqMsg->enCallType         = LRRC_LNAS_CALL_TYPE_ORIGINATING_CALL;
    pMrrcDataReqMsg->enEmmMsgType       = NAS_EMM_MSG_TC;

    pMrrcDataReqMsg->enDataCnf          = LRRC_LMM_DATA_CNF_NOT_NEED;

    /* 向MRRC转发TC_DATA_REQ消息 */
    NAS_EMM_SndUplinkNasMsg(pMrrcDataReqMsg);

    NAS_LMM_MEM_FREE(pMrrcDataReqMsg);

    return;
}
VOS_VOID    NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_ENUM_UINT32
                                                                enRabmReestState)
{
    EMM_ERABM_REEST_IND_STRU             *pRabmReestIndMsg    = NAS_EMM_NULL_PTR;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_SER_SendRabmReestInd is entered.");

    /* 申请DOPRA消息 */
    pRabmReestIndMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ERABM_REEST_IND_STRU));

    if(NAS_EMM_NULL_PTR == pRabmReestIndMsg)
    {
        NAS_EMM_SER_LOG_ERR( "NAS_EMM_SER_SendRabmReestInd: MSG ALLOC ERR !!");
        return;
    }

    /* 填写EMM_ERABM_REEST_IND 的DOPRA消息头 */
    EMM_COMP_ERABM_MSG_HEADER(pRabmReestIndMsg, sizeof(EMM_ERABM_REEST_IND_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);

    /* 填写EMM_ERABM_REEST_IND 的消息ID标识 */
    pRabmReestIndMsg->ulMsgId           = ID_EMM_ERABM_REEST_IND;

    /* 构造EMM_ERABM_REEST_IND 中的重建状态 */
    pRabmReestIndMsg->enReEstState      = enRabmReestState;

    /* 发送消息(Send the msg of) EMM_ERABM_REEST_IND */
    NAS_LMM_SEND_MSG(                   pRabmReestIndMsg);

    return;

}
VOS_VOID NAS_EMM_SER_AbnormalOver(VOS_VOID)
{
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_AbnormalOver is entered.");

    /*停止T3417定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

    /*停止T3440定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

    /*停止T3417ext定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

    /*如果SR流程是由于SMS触发，需要回复SMS建链失败，并且清除SR的发起原因，*/
    if(NAS_EMM_SER_START_CAUSE_SMS_EST_REQ == NAS_EMM_SER_GetSerStartCause())
    {
        /* SER异常的原因值上报暂时报LMM_SMS_ERR_CAUSE_OTHERS，
           以后可能要根据相应的原因值进行具体细分处理，上报准确的原因值
           此处作为遗留问题 */
        NAS_LMM_SndLmmSmsErrInd(LMM_SMS_ERR_CAUSE_OTHERS);
        NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_BUTT);
    }

    if (VOS_TRUE != NAS_EMM_SER_IsCsfbProcedure())
    {
        /*Inform RABM that SER fail*/
         NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_FAIL);
    }

    /*清空ESM_DATA缓存*/
    NAS_EMM_SerClearEsmDataBuf();

    return;
}


VOS_UINT32    NAS_EMM_SER_CHKFSMStateMsgp(
                                    NAS_EMM_MAIN_STATE_ENUM_UINT16  stMS,
                                    NAS_EMM_SUB_STATE_ENUM_UINT16   stSS,
                                    VOS_VOID   *pMsgStru)
{

    NAS_EMM_SER_LOG1_INFO( "NAS_EMM_SER_CHKFSMStateMsgp is entered.", pMsgStru);

    if(NAS_EMM_CHK_STAT_INVALID(stMS, stSS))
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMState: STATE ERR !!");
        return NAS_EMM_FAIL;
    }

    if ( NAS_EMM_NULL_PTR == pMsgStru )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKMsgp: pMsgStru is NULL_PTR !!");
        return NAS_EMM_FAIL;
    }

    return NAS_EMM_SUCC;
}




VOS_VOID  NAS_EMM_MmcSendSerResultIndRej
(
    NAS_EMM_CN_CAUSE_ENUM_UINT8       ucCnCause
)
{
    LMM_MMC_SERVICE_RESULT_IND_STRU     *pstRsltInd = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO(               "NAS_EMM_MmcSendSerResultIndRej is entered.");

    /* 申请MMC内部消息 */
    pstRsltInd  = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_SERVICE_RESULT_IND_STRU));

    if(NAS_EMM_NULL_PTR                 == pstRsltInd)
    {
        NAS_EMM_SER_LOG_ERR(            "NAS_EMM_SER_SendMmcActionResultReq: MSG ALLOC ERR !!");
        return;
    }

    NAS_LMM_MEM_SET(pstRsltInd, 0, sizeof(LMM_MMC_SERVICE_RESULT_IND_STRU));

    /* 填充给RRC的DOPRA头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER(            pstRsltInd,
                                        sizeof(LMM_MMC_SERVICE_RESULT_IND_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);
    /*填充OPID*/


    /* 填充消息ID */
    pstRsltInd->ulMsgId                 = ID_LMM_MMC_SERVICE_RESULT_IND;

    /* 填充消息内容----SERVCIE结果 */
    pstRsltInd->ulServiceRst            = MMC_LMM_SERVICE_RSLT_CN_REJ;

    /*填充消息内容----拒绝原因值*/
    pstRsltInd->bitOpCnCause            = NAS_EMM_BIT_SLCT;
    pstRsltInd->ucCnCause               = ucCnCause;

    /*填充消息内容----请求类型*/
    /* 如果ARBOT标识无效，则是CSFB流程，则填写请求类型 */
    if (NAS_EMM_CSFB_ABORT_FLAG_VALID   != NAS_EMM_SER_GetEmmSerCsfbAbortFlag())
    {
        if (NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        {
            pstRsltInd->bitOpReqType    = NAS_EMM_BIT_SLCT;
            pstRsltInd->ulReqType       = MMC_LMM_SERVICE_MO_CSFB_1XCSFB;
        }
        else if (NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        {
            pstRsltInd->bitOpReqType    = NAS_EMM_BIT_SLCT;
            pstRsltInd->ulReqType       = MMC_LMM_SERVICR_MT_CSFB_1XCSFB;
        }
        else if (NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        {
            pstRsltInd->bitOpReqType    = NAS_EMM_BIT_SLCT;
            pstRsltInd->ulReqType       = MMC_LMM_SERVICR_MO_CSFB_1XCSFB_EMERGENCY;
        }
        else
        {
            /* 非CSFB类型不填写ulReqType */
        }
    }

    /* 发送LMM_MMC_SERVICE_RESULT_IND消息 */
    NAS_LMM_SendLmmMmcMsg(             pstRsltInd);

    return;
}
VOS_VOID  NAS_EMM_MmcSendSerResultIndOtherType
(
    MMC_LMM_SERVICE_RSLT_ENUM_UINT32    ulSerRslt
)
{
    LMM_MMC_SERVICE_RESULT_IND_STRU     *pstRsltInd = NAS_EMM_NULL_PTR;

    /* 打印进入该函数*/
    NAS_EMM_SER_LOG_INFO("NAS_EMM_MmcSendSerResultIndOtherType is entered.");

    /* 申请MMC内部消息*/
    pstRsltInd  = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_SERVICE_RESULT_IND_STRU));

    if(NAS_EMM_NULL_PTR                 == pstRsltInd)
    {
        NAS_EMM_SER_LOG_ERR("NAS_EMM_MmcSendSerResultIndOtherType: MSG ALLOC ERR !!");
        return;
    }

    NAS_LMM_MEM_SET(pstRsltInd, 0, sizeof(LMM_MMC_SERVICE_RESULT_IND_STRU));

    /* 填充给RRC的DOPRA头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER(       pstRsltInd,
                                        sizeof(LMM_MMC_SERVICE_RESULT_IND_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);
    /*填充OPID*/


    /* 填充消息ID */
    pstRsltInd->ulMsgId                 = ID_LMM_MMC_SERVICE_RESULT_IND;

    /* 填充消息内容----SERVCIE结果 */
    if ((MMC_LMM_SERVICE_RSLT_FAILURE== ulSerRslt) ||
        (MMC_LMM_SERVICE_RSLT_ACCESS_BARED == ulSerRslt) ||
        (MMC_LMM_SERVICE_RSLT_AUTH_REJ == ulSerRslt))
    {

        NAS_EMM_SER_LOG1_NORM("NAS_EMM_MmcSendSerResultIndOtherType: ulSerRslt = ",
                                ulSerRslt);
        pstRsltInd->ulServiceRst            = ulSerRslt;
    }
    else
    {
        NAS_EMM_SER_LOG_WARN("NAS_EMM_MmcSendSerResultIndOtherType: ulSerRslt is err! ");
    }

    /*填充消息内容----请求类型*/
    /* 如果ARBOT标识无效，则是CSFB流程，则填写请求类型 */
    if (NAS_EMM_CSFB_ABORT_FLAG_VALID   != NAS_EMM_SER_GetEmmSerCsfbAbortFlag())
    {
        if (NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        {
            pstRsltInd->bitOpReqType    = NAS_EMM_BIT_SLCT;
            pstRsltInd->ulReqType       = MMC_LMM_SERVICE_MO_CSFB_1XCSFB;
        }
        else if (NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        {
            pstRsltInd->bitOpReqType    = NAS_EMM_BIT_SLCT;
            pstRsltInd->ulReqType       = MMC_LMM_SERVICR_MT_CSFB_1XCSFB;
        }
        else if (NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        {
            pstRsltInd->bitOpReqType    = NAS_EMM_BIT_SLCT;
            pstRsltInd->ulReqType       = MMC_LMM_SERVICR_MO_CSFB_1XCSFB_EMERGENCY;
        }
        else
        {
            /* 非CSFB类型不填写ulReqType */
        }
    }

    /* 发送LMM_MMC_SERVICE_RESULT_IND消息 */
    NAS_LMM_SendLmmMmcMsg(             pstRsltInd);

    return;
}



NAS_EMM_SER_START_CAUSE_ENUM_UINT8  NAS_EMM_SER_GetSerStartCause(
                                        VOS_VOID)
{
    return NAS_EMM_SER_GetEmmSERStartCause();
}


VOS_UINT32 NAS_EMM_SER_IsCsfbProcedure( VOS_VOID )
{
    if ((NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        || (NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        || (NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause()))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_EMM_SER_IsMoCsfbProcedure( VOS_VOID )
{
    if (NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_VOID  NAS_EMM_MmSendCsfbSerPaingInd
(
    NAS_EMM_MT_CSFB_TYPE_ENUM_UINT8     enMtCsfbType,
    const NAS_EMM_CN_CS_SER_NOTIFICAIOTN_STRU *pstCsSerNotification,
    LRRC_LNAS_PAGING_UE_ID_ENUM_UINT32 enPagingUeId
)
{
    LMM_MM_CSFB_SERVICE_PAGING_IND_STRU   *pstMmCsfbSerPaingInd = VOS_NULL_PTR;

    /* 申请DOPRA消息 */
    pstMmCsfbSerPaingInd = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MM_CSFB_SERVICE_PAGING_IND_STRU));
    if (NAS_LMM_NULL_PTR == pstMmCsfbSerPaingInd)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_SER_LOG_ERR("NAS_EMM_MmSendCsfbSerPaingInd: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstMmCsfbSerPaingInd, 0, sizeof(LMM_MM_CSFB_SERVICE_PAGING_IND_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MM_MSG_HEADER((pstMmCsfbSerPaingInd),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MM_CSFB_SERVICE_PAGING_IND_STRU));

    /* 填充消息ID */
    pstMmCsfbSerPaingInd->ulMsgId    = ID_LMM_MM_CSFB_SERVICE_PAGING_IND;

    /* 填充消息体 */
    pstMmCsfbSerPaingInd->ulOpId     = NAS_EMM_OPID_MM;

    if (NAS_EMM_MT_CSFB_TYPE_CS_SER_NOTIFICATION == enMtCsfbType)
    {
        /* 填充CLI */
        pstMmCsfbSerPaingInd->bitOpCli = pstCsSerNotification->bitOpCli;
        pstMmCsfbSerPaingInd->ucCliLength = pstCsSerNotification->ucCliLength;
        NAS_LMM_MEM_CPY(    pstMmCsfbSerPaingInd->aucCliValue,
                            pstCsSerNotification->aucCliValue,
                            pstCsSerNotification->ucCliLength);

        /* 填充SS CODE */
        pstMmCsfbSerPaingInd->bitOpSsCodeValue = pstCsSerNotification->bitOpSsCode;
        pstMmCsfbSerPaingInd->ucSsCodeValue = pstCsSerNotification->ucSsCodeValue;
        if (0 == pstCsSerNotification->ucPagingIdenity)
        {
            pstMmCsfbSerPaingInd->enPagingUeId = LMM_MM_PAGING_IND_UE_ID_IMSI;
        }
        else
        {
            pstMmCsfbSerPaingInd->enPagingUeId = LMM_MM_PAGING_IND_UE_ID_TMSI;
        }
        
    }
    else
    {
        if(LRRC_LNAS_S_TMSI_LTE == enPagingUeId)
        {
            pstMmCsfbSerPaingInd->enPagingUeId = LMM_MM_PAGING_IND_UE_ID_S_TMSI;
        }
        else if(LRRC_LNAS_IMSI_LTE == enPagingUeId)
        {
            pstMmCsfbSerPaingInd->enPagingUeId = LMM_MM_PAGING_IND_UE_ID_IMSI;
        }
        else
        {

        }
    }

    /* 发送DOPRA消息 */
    NAS_LMM_SendLmmMmcMsg((VOS_VOID*)pstMmCsfbSerPaingInd);

    return;
}
VOS_VOID  NAS_EMM_MmSendCsfbSerEndInd
(
    MM_LMM_CSFB_SERVICE_RSLT_ENUM_UINT32     enCsfbSrvRslt
)
{
    LMM_MM_CSFB_SERVICE_END_IND_STRU   *pstMmCsfbSerEndInd = VOS_NULL_PTR;

    /* 如果已经收到ABORT消息，则不用通知MM终止CSFB流程 */
    if (NAS_EMM_CSFB_ABORT_FLAG_VALID == NAS_EMM_SER_GetEmmSerCsfbAbortFlag())
    {
        NAS_EMM_SER_LOG_NORM("NAS_EMM_MmSendCsfbSerEndInd:CSFB is already aborted!");
        return ;
    }

    /* 申请DOPRA消息 */
    pstMmCsfbSerEndInd = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MM_CSFB_SERVICE_END_IND_STRU));
    if (NAS_LMM_NULL_PTR == pstMmCsfbSerEndInd)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_SER_LOG_ERR("NAS_EMM_MmSendCsfbSerEndInd: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstMmCsfbSerEndInd, 0, sizeof(LMM_MM_CSFB_SERVICE_END_IND_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MM_MSG_HEADER((pstMmCsfbSerEndInd),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MM_CSFB_SERVICE_END_IND_STRU));

    /* 填充消息ID */
    pstMmCsfbSerEndInd->ulMsgId    = ID_LMM_MM_CSFB_SERVICE_END_IND;

    /* 填充消息体 */
    pstMmCsfbSerEndInd->ulOpId     = NAS_EMM_OPID_MM;

    pstMmCsfbSerEndInd->enCsfbSrvRslt = enCsfbSrvRslt;

    /* 发送DOPRA消息 */
    NAS_LMM_SendLmmMmcMsg((VOS_VOID*)pstMmCsfbSerEndInd);

    return;
}
/*lint +e961*/
/*lint +e960*/

VOS_VOID NAS_EMM_RestartSerProcedural(VOS_VOID)
{
    /* 重启SERVICE流程 */
    switch (NAS_EMM_SER_GetEmmSERStartCause())
    {
        case NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ:
        case NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ:
        case NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ:

            /*停止T3417定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

            /*停止T3440定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

            /*停止T3417ext定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

            /*启动定时器3417*/
            NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

            /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
            NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

            /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
            NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq();
            break;

        default :

            /*停止T3417定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

            /*停止T3440定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

            /*停止T3417ext定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

            /*启动定时器3417*/
            NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

            /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
            NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417);

            /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
            NAS_EMM_SER_SendMrrcDataReq_ServiceReq();
            break;
    }

    return ;
}


/* lihong00150010 emergency tau&service begin */

VOS_VOID  NAS_EMM_EmcPndReqSerAbnormalCommProc
(
    NAS_EMM_SUB_STATE_ENUM_UINT16       ucSs
)
{
    NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

    /* 本地DETACH释放资源:动态内存、赋初值 */
    NAS_LMM_DeregReleaseResource();

    /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
    NAS_EMM_MmcSendDetIndLocal( MMC_LMM_L_LOCAL_DETACH_OTHERS);

    /* 记录ATTACH触发原因值 */
    NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER;

    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                ucSs,
                                TI_NAS_EMM_STATE_NO_TIMER);
}
/* lihong00150010 emergency tau&service end */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


