


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CDS.h"
#include "CdsDlProc.h"
#include "CdsIpfCtrl.h"
#include "CdsDebug.h"
#include "QosFcOm.h"
#include "CdsImsProc.h"


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_CDS_DL_PROC_C
/*lint +e767*/

/*****************************************************************************
  2 外部函数声明
*****************************************************************************/
extern VOS_BOOL Fc_CdsDlPktDiscard(VOS_UINT8 ucRabId);

/******************************************************************************
   3 私有定义
******************************************************************************/


/******************************************************************************
   4 全局变量定义
******************************************************************************/


/******************************************************************************
   5 函数实现
******************************************************************************/

#if (CDS_FEATURE_OFF == CDS_FEATURE_MULTI_MODEM)

VOS_UINT32 I1_NAS_RABM_GetDefaultRabId(VOS_UINT8 ucRabId, VOS_UINT8 *pucDeftRabId)
{
    if (VOS_NULL_PTR == pucDeftRabId)
    {
        return PS_FAIL;
    }

    *pucDeftRabId = ucRabId;
    return PS_SUCC;
}
#endif


VOS_UINT32 CDS_GUGetDefaultRabId(VOS_UINT8 ucRabId, VOS_UINT8 *pucDeftRabId, MODEM_ID_ENUM_UINT16 enModemId)
{
    if (MODEM_ID_0 == enModemId)
    {
        return NAS_RABM_GetDefaultRabId(ucRabId, pucDeftRabId);
    }
    else if (MODEM_ID_1 == enModemId)
    {
        return I1_NAS_RABM_GetDefaultRabId(ucRabId, pucDeftRabId);
    }
    else
    {
        return PS_FAIL;
    }
}


VOS_UINT32 CDS_RxDlSdu(TTF_MEM_ST *pstIpPkt)
{
    VOS_INT32      lLock;

    lLock = VOS_SplIMP();

    if (PS_SUCC != LUP_EnQue(CDS_GET_IPF_DL_SDU_QUE(),pstIpPkt))
    {
        VOS_Splx(lLock);
        TTF_MemFree(UEPS_PID_CDS, pstIpPkt);
        CDS_DBG_DL_EN_SDU_QUEUE_FAIL_NUM(1);
        CDS_UP_EVENT_RPT(CDS_UP_EVENT_DL_SDU_QUE_FULL);
        return PS_FAIL;
    }

    CDS_DBG_DL_EN_SDU_QUEUE_SUCC_NUM(1);

    VOS_Splx(lLock);

    return PS_SUCC;
}



VOS_UINT32 CDS_GUDlSduParaCheck(CDS_RABM_TRANS_DATA_STRU *pstDlData)
{
    if (VOS_NULL_PTR == pstDlData)
    {
        CDS_ERROR_LOG(UEPS_PID_CDS,"CDS_GUDlSduParaCheck : Input Null Para.");
        return PS_FAIL;
    }

    if (VOS_NULL_PTR == pstDlData->pstSdu)
    {
        CDS_ERROR_LOG(UEPS_PID_CDS,"CDS_GUDlSduParaCheck : Sdu Ptr Is NULL.");
        return PS_FAIL;
    }

    if ((0 == pstDlData->ulSduLen) || (CDS_DL_SDU_MAX_LEN < pstDlData->ulSduLen))
    {
        TTF_MemFree(UEPS_PID_CDS,pstDlData->pstSdu);
        CDS_ERROR_LOG1(UEPS_PID_CDS,"CDS_GUDlSduParaCheck : SDU Too Big.",pstDlData->ulSduLen);
        return PS_FAIL;
    }

    if ((CDS_NAS_MIN_BEARER_ID > pstDlData->ucRabId) || (CDS_NAS_MAX_BEARER_ID < pstDlData->ucRabId))
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS,"CDS_GUDlSduParaCheck : Input RabId Fail.RabID=",pstDlData->ucRabId);
        TTF_MemFree(UEPS_PID_CDS,pstDlData->pstSdu);
        return PS_FAIL;
    }

    return PS_SUCC;
}


VOS_VOID CDS_UMTS_DL_DataInd(CDS_RABM_TRANS_DATA_STRU *pstDlData, MODEM_ID_ENUM_UINT16 enModemId)
{
    TTF_MEM_ST            *pstIpPkt;
    CDS_ENTITY_STRU       *pstCdsEntity;
    CDS_LB_DL_SDU_STRU     stLbSdu;
    VOS_UINT8              ucDeftRabId;
    CDS_IMS_SDU_STRU       stImsSdu;

    /*入参判断*/
    if (PS_SUCC != CDS_GUDlSduParaCheck(pstDlData))
    {
        CDS_ERROR_LOG(UEPS_PID_CDS,"CDS_UMTS_DL_DataInd : Sdu Para Error.");
        return;
    }

    pstCdsEntity = CDS_GetCdsEntity(enModemId);
    if (VOS_NULL_PTR == pstCdsEntity)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS,"CDS_UMTS_DL_DataInd : Input ModemId Fail.RabID=",enModemId);
        TTF_MemFree(UEPS_PID_CDS,pstDlData->pstSdu);
        return;
    }

    CDS_DBG_DL_UMTS_RX_SDU_NUM(1);
    CDS_DBG_MODEM_DL_UMTS_RX_SDU_NUM(enModemId,1);

    /*IMS*/
    if (PS_TRUE == CDS_IsImsBearer(pstDlData->ucRabId, pstCdsEntity))
    {
        stImsSdu.ucRabId  = pstDlData->ucRabId;
        stImsSdu.ucDrbId  = 0;
        stImsSdu.ulSduLen = pstDlData->ulSduLen;
        stImsSdu.pstSdu   = pstDlData->pstSdu;
        CDS_DlProcImsData(&stImsSdu, pstCdsEntity);
        CDS_DBG_DL_UMTS_RX_IMS_PKT_NUM(1);
        CDS_DBG_MODEM_DL_UMTS_RX_IMS_PKT_NUM(enModemId,1);
        return;
    }

    /*环回模式*/
    if ((PS_TRUE == pstCdsEntity->ulTestModeFlg) && (CDS_LB_STATE_START == pstCdsEntity->ulLoopBackState))
    {
        stLbSdu.ulSduLen = pstDlData->ulSduLen;
        stLbSdu.pstSdu   = pstDlData->pstSdu;
        CDS_RxLoopBackPkt(&stLbSdu,pstCdsEntity);

        CDS_DBG_LB_DL_RX_SDU_FROM_UMTS(1);
        CDS_DBG_MODEM_ID_LB_DL_RX_SDU_FROM_UMTS(enModemId,1);
        return;
    }

    /*性能测试，内存直接释放*/
    if (PS_TRUE == pstCdsEntity->ulDLPktDiscardFlg)
    {
        TTF_MemFree(UEPS_PID_CDS, pstDlData->pstSdu);
        CDS_DBG_DL_UMTS_DIRECT_FREE_SDU_NUM(1);
        CDS_DBG_MODEM_DL_UMTS_DIRECT_FREE_SDU_NUM(enModemId,1);
        return;
    }

    /* 流控丢包 */
    if(Fc_CdsDlPktDiscard(pstDlData->ucRabId))
    {
        TTF_MemFree(UEPS_PID_CDS, pstDlData->pstSdu);
        FC_DBG_DLGU_CDS_DISCARD_PKT_STAT(1);
        CDS_DBG_DL_UMTS_FC_FREE_SDU_NUM(1);
        CDS_DBG_MODEM_DL_UMTS_FC_FREE_SDU_NUM(enModemId,1);
        CDS_UP_EVENT_RPT(CDS_UP_EVENT_DL_FLOW_CTRL_DISCARD);
        return;
    }

    /*下行流量统计*/
    CDS_DLDataFlowStats(pstDlData->ucRabId,pstDlData->ulSduLen,pstCdsEntity);

    /*获得缺省PDP承载ID*/
    if (PS_SUCC != CDS_GUGetDefaultRabId(pstDlData->ucRabId,&ucDeftRabId,enModemId))
    {
        TTF_MemFree(UEPS_PID_CDS, pstDlData->pstSdu);
        CDS_DBG_DL_UMTS_GET_DEFT_RABID_FAIL(1);
        return;
    }

    /*保存当前信息*/
    pstIpPkt = pstDlData->pstSdu;
    CDS_DL_SAVE_LEN_MODEMID_RABID_TO_TTF(pstIpPkt,
                                         pstDlData->ulSduLen,
                                         enModemId,
                                         ucDeftRabId);

    /*入队*/
    if (PS_SUCC != CDS_RxDlSdu(pstIpPkt))
    {
        CDS_DBG_DL_UMTS_ENQUE_FAIL_NUM(1);
        CDS_DBG_MODEM_DL_UMTS_ENQUE_FAIL_NUM(enModemId,1);
        return;
    }

    CDS_DBG_DL_UMTS_ENQUE_SUCC_NUM(1);
    CDS_DBG_MODEM_DL_UMTS_ENQUE_SUCC_NUM(enModemId,1);
    return;
}
VOS_VOID CDS_GSM_RxSdu(CDS_RABM_TRANS_DATA_STRU *pstDlData, MODEM_ID_ENUM_UINT16 enModemId)
{
    TTF_MEM_ST            *pstIpPkt;
    CDS_ENTITY_STRU       *pstCdsEntity;
    CDS_LB_DL_SDU_STRU     stLbSdu;
    VOS_UINT8              ucDeftRabId;
    CDS_IMS_SDU_STRU       stImsSdu;

    /*入参判断*/
    if (PS_SUCC != CDS_GUDlSduParaCheck(pstDlData))
    {
        CDS_ERROR_LOG(UEPS_PID_CDS,"CDS_GSM_DL_DataInd : Sdu Para Error.");
        return;
    }

    pstCdsEntity = CDS_GetCdsEntity(enModemId);
    if (VOS_NULL_PTR == pstCdsEntity)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS,"CDS_GSM_DL_DataInd : Input ModemId Error.ModemId=",enModemId);
        TTF_MemFree(UEPS_PID_CDS,pstDlData->pstSdu);
        return;
    }

    CDS_DBG_DL_GSM_RX_SDU_NUM(1);
    CDS_DBG_MODEM_DL_GSM_RX_SDU_NUM(enModemId,1);

    /*IMS*/
    if (PS_TRUE == CDS_IsImsBearer(pstDlData->ucRabId, pstCdsEntity))
    {
        stImsSdu.ucRabId  = pstDlData->ucRabId;
        stImsSdu.ucDrbId  = 0;
        stImsSdu.ulSduLen = pstDlData->ulSduLen;
        stImsSdu.pstSdu   = pstDlData->pstSdu;
        CDS_DlProcImsData(&stImsSdu, pstCdsEntity);
        CDS_DBG_DL_GSM_RX_IMS_PKT_NUM(1);
        CDS_DBG_MODEM_DL_GSM_RX_IMS_PKT_NUM(enModemId,1);
        return;
    }

    /*环回模式*/
    if ((PS_TRUE == pstCdsEntity->ulTestModeFlg) && (CDS_LB_STATE_START == pstCdsEntity->ulLoopBackState))
    {
        stLbSdu.ulSduLen = pstDlData->ulSduLen;
        stLbSdu.pstSdu   = pstDlData->pstSdu;
        CDS_RxLoopBackPkt(&stLbSdu,pstCdsEntity);

        CDS_DBG_LB_DL_RX_SDU_FROM_GSM(1);
        CDS_DBG_MODEM_ID_LB_DL_RX_SDU_FROM_GSM(enModemId,1);
        return;
    }

    /* 流控丢包 */
    if(Fc_CdsDlPktDiscard(pstDlData->ucRabId))
    {
        TTF_MemFree(UEPS_PID_CDS, pstDlData->pstSdu);
        FC_DBG_DLGU_CDS_DISCARD_PKT_STAT(1);
        CDS_DBG_DL_GSM_FC_FREE_SDU_NUM(1);
        CDS_DBG_MODEM_DL_GSM_FC_FREE_SDU_NUM(enModemId,1);
        CDS_UP_EVENT_RPT(CDS_UP_EVENT_DL_FLOW_CTRL_DISCARD);
        return;
    }

    /*下行流量统计*/
    CDS_DLDataFlowStats(pstDlData->ucRabId,pstDlData->ulSduLen,pstCdsEntity);

    /*获得缺省PDP承载ID*/
    if (PS_SUCC != CDS_GUGetDefaultRabId(pstDlData->ucRabId,&ucDeftRabId,enModemId))
    {
        TTF_MemFree(UEPS_PID_CDS, pstDlData->pstSdu);
        CDS_DBG_DL_GSM_GET_DEFT_RABID_FAIL(1);
        return;
    }

    /*保存当前信息*/
    pstIpPkt = pstDlData->pstSdu;
    CDS_DL_SAVE_LEN_MODEMID_RABID_TO_TTF(pstIpPkt,
                                         pstDlData->ulSduLen,
                                         enModemId,
                                         ucDeftRabId);

    /*入队*/
    if (PS_SUCC != CDS_RxDlSdu(pstIpPkt))
    {
        CDS_DBG_DL_GSM_ENQUE_FAIL_NUM(1);
        CDS_DBG_MODEM_DL_GSM_ENQUE_FAIL_NUM(enModemId,1);
        return;
    }

    CDS_DBG_DL_GSM_ENQUE_SUCC_NUM(1);
    CDS_DBG_MODEM_DL_GSM_ENQUE_SUCC_NUM(enModemId,1);

    return;
}


VOS_VOID CDS_GSM_DL_DataInd(CDS_RABM_TRANS_DATA_STRU *pstDlData, MODEM_ID_ENUM_UINT16 enModemId)
{
    CDS_GSM_RxSdu(pstDlData, enModemId);

    if (0 != LUP_QueCnt(CDS_GET_IPF_DL_SDU_QUE()))
    {
        CDS_SendEventToCds(CDS_EVENT_DL_DATA_PROC);
        CDS_DBG_DL_SDU_TRIGGER_EVENT_NUM(1);
    }

    return;
}


VOS_VOID CDS_LTE_RecvDbgData(TTF_MEM_ST *pstIpPkt)
{
    CDS_ENTITY_STRU       *pstCdsEntity;

    if (VOS_NULL_PTR == pstIpPkt)
    {
        return;
    }

    /*LTE默认放在Modem0*/
    pstCdsEntity = CDS_GetCdsEntity(MODEM_ID_0);
    if (VOS_NULL_PTR == pstCdsEntity)
    {
        TTF_MemFree(UEPS_PID_CDS, pstIpPkt);
        return;
    }

    CDS_DBG_DL_LTE_RX_DBG_PKT_NUM(1);

    /*保存必要信息*/
    CDS_DL_SAVE_LEN_MODEMID_RABID_TO_TTF(pstIpPkt,
                                         pstIpPkt->usUsed,
                                         MODEM_ID_0,
                                         pstCdsEntity->ucDbgBearerId);

    /*入队*/
    if (PS_SUCC != CDS_RxDlSdu(pstIpPkt))
    {
        CDS_DBG_DL_LTE_ENQUE_FAIL_NUM(1);
        return;
    }

    CDS_SendEventToCds(CDS_EVENT_DL_DATA_PROC);
    CDS_DBG_DL_SDU_TRIGGER_EVENT_NUM(1);
    CDS_DBG_DL_LTE_ENQUE_SUCC_NUM(1);
    return;
}


#if (CDS_FEATURE_ON == CDS_FEATURE_LTE)
extern VOS_VOID L2_DlAppThrStatProc(VOS_UINT32 ulLength);
VOS_VOID CDS_LTE_DL_DataInd(CDS_LPDCP_DATA_STRU *pstDsLpdcpData)
{
    TTF_MEM_ST            *pstIpPkt;
    VOS_UINT8              ucDeftEpsbID;
    VOS_UINT8              ucEpsbID;
    CDS_ENTITY_STRU       *pstCdsEntity;
    CDS_LB_DL_SDU_STRU     stLbSdu;
    CDS_IMS_SDU_STRU       stImsSdu;

    CDS_ASSERT(VOS_NULL_PTR != pstDsLpdcpData);

    /*LTE默认放在Modem0*/
    pstCdsEntity = CDS_GetCdsEntity(MODEM_ID_0);
    if (VOS_NULL_PTR == pstCdsEntity)
    {
        TTF_MemFree(UEPS_PID_CDS, pstDsLpdcpData->pstSdu);
        return;
    }

    CDS_DBG_DL_LTE_RX_SDU_NUM(1);

    /*性能测试，内存直接释放*/
    if (PS_TRUE == pstCdsEntity->ulDLPktDiscardFlg)
    {
        TTF_MemFree(UEPS_PID_CDS, pstDsLpdcpData->pstSdu);
        CDS_DBG_DL_LTE_DIRECT_FREE_SDU_NUM(1);
        return;
    }

    /*环回模式*/
    if ((PS_TRUE == pstCdsEntity->ulTestModeFlg) && (CDS_LB_STATE_START == pstCdsEntity->ulLoopBackState))
    {
        stLbSdu.ulSduLen = pstDsLpdcpData->ulSduLen;
        stLbSdu.pstSdu   = pstDsLpdcpData->pstSdu;
        CDS_RxLoopBackPkt(&stLbSdu,pstCdsEntity);
        CDS_DBG_LB_DL_RX_SDU_FROM_LTE(1);
        return;
    }

    /*获得DRB对应的EpsbID失败.用于流量统计*/
    if (PS_SUCC != CDS_ERABM_GetEpsbId(pstDsLpdcpData->ucRbId,&ucEpsbID))
    {
        TTF_MemFree(UEPS_PID_CDS, pstDsLpdcpData->pstSdu);
        CDS_DBG_DL_LTE_GET_EPSBID_FAIL(1);
        return;
    }

    /*IMS*/
    if (PS_TRUE == CDS_IsImsBearer(ucEpsbID, pstCdsEntity))
    {
        stImsSdu.ucRabId  = ucEpsbID;
        stImsSdu.ucDrbId  = pstDsLpdcpData->ucRbId;
        stImsSdu.ulSduLen = pstDsLpdcpData->ulSduLen;
        stImsSdu.pstSdu   = pstDsLpdcpData->pstSdu;
        CDS_DlProcImsData(&stImsSdu, pstCdsEntity);
        CDS_DBG_DL_LTE_RX_IMS_PKT_NUM(1);
        return;
    }


    /* 流控丢包 */
    if(Fc_CdsDlPktDiscard(ucEpsbID))
    {
        TTF_MemFree(UEPS_PID_CDS, pstDsLpdcpData->pstSdu);
        FC_DBG_DLLTE_CDS_DISCARD_PKT_STAT(1);
        CDS_DBG_DL_LTE_FC_FREE_SDU_NUM(1);
        CDS_UP_EVENT_RPT(CDS_UP_EVENT_DL_FLOW_CTRL_DISCARD);
        return;
    }

    /*下行流量统计*/
    CDS_DLDataFlowStats(ucEpsbID,pstDsLpdcpData->ulSduLen,pstCdsEntity);

    L2_DlAppThrStatProc(pstDsLpdcpData->ulSduLen);

    /*获得缺省EPS承载ID*/
    if (PS_SUCC != CDS_ERABM_GetDefaultEpsbId(pstDsLpdcpData->ucRbId,&ucDeftEpsbID))
    {
        TTF_MemFree(UEPS_PID_CDS, pstDsLpdcpData->pstSdu);
        CDS_DBG_DL_LTE_GET_DEFT_EPSBID_FAIL(1);
        return;
    }

    /*保存必要信息*/
    pstIpPkt = pstDsLpdcpData->pstSdu;
    CDS_DL_SAVE_LEN_MODEMID_RABID_TO_TTF(pstIpPkt,
                                         pstDsLpdcpData->ulSduLen,
                                         MODEM_ID_0,
                                         ucDeftEpsbID);

    /*入队*/
    if (PS_SUCC != CDS_RxDlSdu(pstIpPkt))
    {
        CDS_DBG_DL_LTE_ENQUE_FAIL_NUM(1);
        return;
    }

    CDS_DBG_DL_LTE_ENQUE_SUCC_NUM(1);
    return;
}


VOS_VOID CDS_MBMS_DL_DataInd(TTF_MEM_ST *pstSdu)
{
    CDS_ENTITY_STRU       *pstCdsEntity;

    if (VOS_NULL_PTR == pstSdu)
    {
        return;
    }

    /*LTE默认放在Modem0*/
    pstCdsEntity = CDS_GetCdsEntity(MODEM_ID_0);
    if (VOS_NULL_PTR == pstCdsEntity)
    {
        TTF_MemFree(UEPS_PID_CDS, pstSdu);
        return;
    }

    CDS_DBG_DL_LTE_RX_MBMS_SDU_NUM(1);

    /*保存必要信息*/
    CDS_DL_SAVE_LEN_MODEMID_RABID_TO_TTF(pstSdu,
                                         pstSdu->usUsed,
                                         MODEM_ID_0,
                                         pstCdsEntity->ucMbmsBearerId);

    /*入队*/
    if (PS_SUCC != CDS_RxDlSdu(pstSdu))
    {
        CDS_DBG_DL_LTE_ENQUE_FAIL_NUM(1);
        return;
    }

    CDS_DBG_DL_LTE_ENQUE_SUCC_NUM(1);
    return;
}


#endif
VOS_VOID CDS_SetDfsMax(VOS_VOID)
{
    TAF_APS_SetDfsMax();
    return;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

