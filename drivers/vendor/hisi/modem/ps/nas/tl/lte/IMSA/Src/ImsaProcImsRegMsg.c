/******************************************************************************

   Copyright(C)2013,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : ImsaProcImsRegMsg.c
  Description     : 该C文件实现IMS注册消息处理和IMS注册消息发送
  History           :
     1.sunbing 49683      2013-06-19  Draft Enact

******************************************************************************/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "ImsaProcImsRegMsg.h"
#include "ImsaEntity.h"
#include "ImsaPublic.h"
#include "ImsaImsInterface.h"
#include "UsimPsInterface.h"
#include "ImsaImsInterface.h"
#include "ImsaProcMmaMsg.h"
#include "ImsaImsAdaption.h"
#include "ImsaRegManagement.h"

/*lint -e767*/
#define    THIS_FILE_ID   PS_FILE_ID_IMSAPROCIMSREGMSG_C
/*lint +e767*/

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#if (FEATURE_ON == FEATURE_IMS)
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/


/*****************************************************************************
  3 Function
*****************************************************************************/
extern VOS_UINT32 IMSA_RegProcImsMsgState(const IMSA_IMS_OUTPUT_SERVICE_EVENT_STRU *pstOutputService);

/*lint -e960*/
/*lint -e961*/
/*****************************************************************************
 Function Name  : IMSA_RegProcImsMsgAuth()
 Description    : IMS鉴权消息处理
 Input          : CSM_OutputService* pstCsmOutputService
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-07-01  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_RegProcImsMsgAuth(IMSA_IMS_OUTPUT_SERVICE_EVENT_STRU* pstOutputService)
{
    USIMM_AUTH_DATA_STRU     stUsimAuthData ={0};

    if(pstOutputService->bitOpAka == 0)
    {
        IMSA_WARN_LOG("IMSA_RegProcImsMsgAuth: AKA bit is empty!");
        return;
    }

    /*保存IMS消息对应的OPID*/
    IMSA_SaveRcvImsOpid(pstOutputService->ulOpId);

    stUsimAuthData.enAuthType = USIMM_IMS_AUTH;
    stUsimAuthData.unAuthPara.stImsAuth.stAutn.ulDataLen = 16;
    stUsimAuthData.unAuthPara.stImsAuth.stAutn.pucData = (VOS_UINT8*)&pstOutputService->stAkaPara.ucAkaAutn[0];

    stUsimAuthData.unAuthPara.stImsAuth.stRand.ulDataLen = 16;
    stUsimAuthData.unAuthPara.stImsAuth.stRand.pucData = (VOS_UINT8*)&pstOutputService->stAkaPara.ucAkaRand[0];

    /*输入鉴权数据*/
    (VOS_VOID)USIMM_AuthReq(PS_PID_IMSA,  \
                  USIMM_ISIM_APP,\
                  &stUsimAuthData);

}


/*****************************************************************************
 Function Name  : IMSA_ImsMsgServiceEvent()
 Description    : D2IMS服务类型的事件处理函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-20  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ImsMsgServiceEvent(VOS_VOID *pSeriveEvent)
{
    IMSA_IMS_OUTPUT_SERVICE_EVENT_STRU *pstOutSerive = VOS_NULL_PTR;

    pstOutSerive = (IMSA_IMS_OUTPUT_SERVICE_EVENT_STRU*)pSeriveEvent;

    /* 注册相关消息处理 */
    /*lint -e788*/
    switch (pstOutSerive->enOutputServiceReason)
    {
    case IMSA_IMS_OUTPUT_SEVICE_REASON_STATE:
        (VOS_VOID)IMSA_RegProcImsMsgState(pstOutSerive);
        break;

    case IMSA_IMS_OUTPUT_SEVICE_REASON_AUTH_CHALLENGE:
        IMSA_RegProcImsMsgAuth(pstOutSerive);
        break;

    case IMSA_IMS_OUTPUT_SEVICE_REASON_IPSEC_RELEASE:
        IMSA_INFO_LOG("IMSA_ImsMsgServiceEvent: IPSEC_RELEASE.");
        break;

    case IMSA_IMS_OUTPUT_SEVICE_REASON_IPSEC_SETUP:
        IMSA_INFO_LOG("IMSA_ImsMsgServiceEvent: IPSEC_SETUP.");
        break;

    case IMSA_IMS_OUTPUT_SEVICE_REASON_OK:
        IMSA_INFO_LOG("IMSA_ImsMsgServiceEvent: REASON_OK.");
        break;


    default:
        IMSA_WARN_LOG("IMSA_ImsMsgServiceEvent: err sevice reason!");
        break;
    }/*lint +e788*/


}
VOS_VOID IMSA_ImsMsgSystemEvent(VOS_VOID *pSystemEvent)
{
    IMSA_IMS_OUTPUT_SYSTEM_EVENT_STRU       *pstOutSystem;

    pstOutSystem = (IMSA_IMS_OUTPUT_SYSTEM_EVENT_STRU*)pSystemEvent;

    if(IMSA_IMS_OUTPUT_SYSTM_REASON_OK == pstOutSystem->enOutputSysReason)
    {
        IMSA_ProcImsMsgStartOrStopCnf();
    }
    else
    {
        IMSA_ERR_LOG("IMSA_ImsMsgSystemEvent: err System reason!");
    }

}


VOS_VOID IMSA_ImsMsgParaEvent(VOS_VOID *pParaEvent)
{
    IMSA_IMS_OUTPUT_PARA_EVENT_STRU       *pstOutPara;
    IMSA_ENTITY_STRU                      *pstImsaEntity = IMSA_CtxGet();

    pstOutPara = (IMSA_IMS_OUTPUT_PARA_EVENT_STRU*)pParaEvent;

    IMSA_INFO_LOG2("IMSA_ImsMsgParaEvent: reason:,opid:", pstOutPara->enOutputParaReason,pstOutPara->ulOpId);

    switch (pstImsaEntity->stImsaControlManager.stImsMsg.ulCsmReason)
    {
        case IMAS_IMS_INPUT_PARA_REASON_QUERY_IMPU:

            if (pstImsaEntity->stImsaControlManager.stImsMsg.ulCsmId == pstOutPara->ulOpId)
            {
                IMSA_ProcImsMsgQryVolteImpuCnf(pstOutPara);

                IMSA_ImsInputParaMsgClear();
            }
            break;

        case IMAS_IMS_INPUT_PARA_REASON_SET_CALL_WAITING:

            if (pstImsaEntity->stImsaControlManager.stImsMsg.ulCsmId == pstOutPara->ulOpId)
            {
                IMSA_ProcImsMsgCcwaiSetCnf(pstOutPara);

                IMSA_ImsInputParaMsgClear();
            }
            break;

        default:
            break;
    }

}


VOS_VOID IMSA_ImsMsgNvInfoEvent(VOS_VOID *pParaEvent)
{
    IMSA_IMS_OUTPUT_NV_INFO_EVENT_STRU       *pstOutNvInfo;

    pstOutNvInfo = (IMSA_IMS_OUTPUT_NV_INFO_EVENT_STRU*)pParaEvent;

    IMSA_INFO_LOG2("IMSA_ImsMsgParaEvent: reason:,opid:", pstOutNvInfo->enOutputNvInfoReason, pstOutNvInfo->ulOpId);

}

/*lint +e961*/
/*lint +e960*/
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaProcImsRegMsg.c */










