





/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/

#include "NasEmmLppMsgProc.h"

#include  "NasLmmPubMOsa.h"
#include  "NasEmmPubUGlobal.h"

#include  "NasEmmMrrcPubInterface.h"
#include  "NasLmmPubMOm.h"
#include  "NasEmmAttDetInclude.h"


/*lint -e767*/
#define    PS_FILE_ID_NASEMMLPPMSGPROC_C   3001
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMLPPMSGPROC_C
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

/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_EMM_CompUplinkGenericNasTransportMsg(VOS_UINT8 *pMsgBuf,
                                                  VOS_UINT32 *pulIndex,
                                                  const NAS_EMM_UPLINK_GENERIC_NAS_TRANSPORT_STRU *pMsgStruct)
{
    VOS_UINT32 ulIndex = 0;

    NAS_EMM_PUBU_LOG_NORM("NAS_EMM_CompUplinkGenericNasTransportMsg enter!");

    if ((NAS_LMM_NULL == pMsgBuf) || (NAS_LMM_NULL == pMsgStruct))
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_CompUplinkGenericNasTransportMsg: invalid input!");
        return;
    }

    /* encode PD and Security header type */
    pMsgBuf[ulIndex++] = EMM_CN_MSG_PD_EMM;

    /* encode message type */
    pMsgBuf[ulIndex++] = NAS_EMM_CN_MT_UPLINK_GENERIC_NAS_TRANSPORT;

    /* encode Generic message container type */
    pMsgBuf[ulIndex++] = pMsgStruct->ucContainerType;

    /* encode Generic message container */
    pMsgBuf[ulIndex++] = (pMsgStruct->stContainer.usMsgLen >> NAS_EMM_MOVEMENT_8_BITS) & NAS_EMM_FIRST_BYTE_F;
    pMsgBuf[ulIndex++] = (pMsgStruct->stContainer.usMsgLen) & NAS_EMM_FIRST_BYTE_F;
    NAS_LMM_MEM_CPY(pMsgBuf + ulIndex, pMsgStruct->stContainer.aucMsg, pMsgStruct->stContainer.usMsgLen);
    ulIndex += pMsgStruct->stContainer.usMsgLen;

    /* encode Additional Information if necessary */
    if (pMsgStruct->bitOpAdditionalInfo)
    {
        /* encode IEI of Additional Information */
        pMsgBuf[ulIndex++] = NAS_EMM_ADDITIONAL_INFO_IEI;

        /* encode length of Additional Information */
        pMsgBuf[ulIndex++] = pMsgStruct->stAdditionalInfo.ucMsgLen;

        /* copy content of Additional Information */
        NAS_LMM_MEM_CPY(pMsgBuf + ulIndex, pMsgStruct->stAdditionalInfo.aucMsg, pMsgStruct->stAdditionalInfo.ucMsgLen);
        ulIndex += pMsgStruct->stAdditionalInfo.ucMsgLen;
    }

    *pulIndex = ulIndex;
}


VOS_VOID NAS_EMM_SendMrrcDataReq_UplinkGenericNasTransport(const NAS_EMM_UPLINK_GENERIC_NAS_TRANSPORT_STRU *pstUplinkMsg)
{
    NAS_EMM_MRRC_DATA_REQ_STRU *pIntraMsg = NAS_LMM_NULL;
    VOS_UINT32 ulDataReqMsgLenNoHeader = 0;

    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_SendMrrcDataReq_LppData enter!");

    if (NAS_LMM_NULL == pstUplinkMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_SendMrrcDataReq_LppData: invalid input");
        return;
    }

    pIntraMsg = (VOS_VOID *)NAS_LMM_MEM_ALLOC(NAS_EMM_INTRA_MSG_MAX_SIZE);
    if (NAS_LMM_NULL == pIntraMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_SendMrrcDataReq_LppData: alloc msg fail!");
        return;
    }

    NAS_EMM_CompUplinkGenericNasTransportMsg(pIntraMsg->stNasMsg.aucNasMsg,
                                             &(pIntraMsg->stNasMsg.ulNasMsgSize),
                                             pstUplinkMsg);

    ulDataReqMsgLenNoHeader = NAS_EMM_CountMrrcDataReqLen(pIntraMsg->stNasMsg.ulNasMsgSize);
    if (NAS_EMM_INTRA_MSG_MAX_SIZE < ulDataReqMsgLenNoHeader)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_SendMrrcDataReq_LppData: size too long!");
        NAS_LMM_MEM_FREE(pIntraMsg);
        return;
    }

    EMM_COMP_MM_MSG_HEADER(pIntraMsg, ulDataReqMsgLenNoHeader);

    pIntraMsg->ulMsgId = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;

    pIntraMsg->enEstCaue    = LRRC_LNAS_EST_CAUSE_MO_DATA;
    pIntraMsg->enCallType   = LRRC_LNAS_CALL_TYPE_ORIGINATING_CALL;
    pIntraMsg->enEmmMsgType = NAS_EMM_NAS_UPLINK_GENERIC_NAS_TRANSPORT;
    pIntraMsg->enDataCnf    = LRRC_LMM_DATA_CNF_NOT_NEED;
    pIntraMsg->ulEsmMmOpId  = NAS_LMM_OPID;

    /*report air message UPLINK GENERIC NAS TRANSPORT*/
    NAS_LMM_SendOmtAirMsg(NAS_EMM_OMT_AIR_MSG_UP, NAS_EMM_UPLINK_GENERIC_NAS_TRANSPORT,(NAS_MSG_STRU *)&(pIntraMsg->stNasMsg));

    /*report key event UPLINK GENERIC NAS TRANSPORT*/
    NAS_LMM_SendOmtKeyEvent(EMM_OMT_KE_EMM_UPLINK_GENERIC_NAS_TRANSPORT);

    NAS_EMM_SndUplinkNasMsg(pIntraMsg);

    NAS_LMM_MEM_FREE(pIntraMsg);

}
/*lint +e961*/
/*lint +e960*/
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

