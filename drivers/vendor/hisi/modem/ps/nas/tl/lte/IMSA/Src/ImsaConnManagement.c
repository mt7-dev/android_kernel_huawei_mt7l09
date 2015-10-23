/******************************************************************************

   Copyright(C)2013,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : ImsaConnManagement.c
  Description     : 该C文件实现连接管理模块的初始化，内部消息的处理和发送，
                    提供外部调用的API
  History           :
     1.sunbing 49683      2013-06-19  Draft Enact

******************************************************************************/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "ImsaConnManagement.h"
#include "ImsaPublic.h"
#include "ImsaEntity.h"
#include "ImsNicInterface.h"
#include "ImsaRegManagement.h"

#include "ImsaProcApsMsg.h"


/*lint -e766*/
#if (VOS_OS_VER != VOS_WIN32)

#include <arpa/inet.h>

#endif
/*lint +e766*/

/*lint -e767*/
#define    THIS_FILE_ID  PS_FILE_ID_IMSACONNMANAGEMENT_C
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
VOS_CHAR                                g_acIpStringForSt_ipv4[IMSA_IPV4_ADDR_STRING_LEN + 1] = {0};
VOS_CHAR                                g_acIpStringForSt_ipv6[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

extern VOS_UINT32 IMSA_CONN_GetTimerLen
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_TIMER_ID_ENUM_UINT16           enTimerId
);
extern VOS_UINT32 IMSA_CONN_IsImsSdf
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
);
extern VOS_UINT32 IMSA_CONN_IsPndTypeValid
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
);
extern VOS_UINT32 IMSA_CONN_IsEmcSdf
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
);
extern VOS_UINT32 IMSA_CONN_FindSipSignalDialPara
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT32                          ulSdfNum,
    const TAF_SDF_PARA_STRU            *pstSdfPara,
    VOS_UINT32                         *pulIndex
);
extern VOS_VOID IMSA_CONN_SaveSelectedSdfPara
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const TAF_SDF_PARA_STRU            *pstTafSdfPara
);
extern VOS_VOID IMSA_CONN_DeletePdpInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
);
extern VOS_UINT32 IMSA_CONN_IsPdpIpvxInfoIncludeRegPara
(
    IMSA_IP_TYPE_ENUM_UINT8                 enIpType,
    VOS_CHAR                               *pcRegUeAddr,
    VOS_CHAR                               *pcRegPcscfAddr,
    const IMSA_PDP_CNTXT_INFO_STRU         *pstPdpInfo
);
extern VOS_UINT32 IMSA_CONN_IsPdpInfoIncludeRegPara
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU         *pstPdpInfo
);
extern IMSA_FSM_PROC IMSA_FsmGetProcItem(VOS_UINT32 ulState, VOS_UINT32 ulEvt);
extern VOS_UINT32 IMSA_FsmRun(IMSA_REG_ENTITY_STRU *pstEntity, VOS_UINT32 ulEvt, VOS_VOID *pData);
extern VOS_VOID IMSA_RegResetRetryTimes
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType
);
extern VOS_VOID IMSA_RegConverterReg2Ims
(
    IMSA_REG_TYPE_ENUM_UINT8 enRegType,
    const VOS_CHAR *pucIpAddr,
    const VOS_CHAR *pacPcscfAddr,
    IMSA_IMS_INPUT_EVENT_STRU *pstImsaImsInputEvt
);
extern VOS_VOID IMSA_RegConfigIpAddress2Ims
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType,
    const VOS_CHAR                     *pcUeAddr,
    const VOS_CHAR                     *pacPcscfAddr
);
extern VOS_VOID IMSA_RegSendImsMsgDereg
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType,
    VOS_UINT32                          ulIsLocal
);
extern VOS_UINT32 IMSA_RegSendImsMsgReg
(
    IMSA_REG_TYPE_ENUM_UINT8 enRegType,
    const VOS_CHAR *pcUeAddr,
    const VOS_CHAR *pcPcscfAddr
);
extern VOS_UINT32 IMSA_RegSendIntraMsgRegInd
(
    IMSA_REG_TYPE_ENUM_UINT8 enRegType,
    IMSA_REG_RESULT_ENUM_UINT32 enResult,
    IMSA_RESULT_ACTION_ENUM_UINT32 enAction
);
extern VOS_UINT32 IMSA_RegSendIntraMsgDeregInd
(
    IMSA_REG_TYPE_ENUM_UINT8 enRegType,
    IMSA_REG_RESULT_ENUM_UINT32 enResult,
    IMSA_RESULT_ACTION_ENUM_UINT32 enAction
);
extern VOS_UINT32 IMSA_RegGetAddrByParamType
(
    IMSA_REG_TYPE_ENUM_UINT8 enRegType,
    IMSA_REG_ADDR_PARAM_ENUM_UINT32 enParamType,
    VOS_CHAR *pucUeAddr,
    VOS_CHAR *pucPcscfAddr
);
extern VOS_UINT32 IMSA_RegParseImsError
(
    const IMSA_IMS_OUTPUT_SERVICE_EVENT_STRU  *pstOutputService,
    IMSA_IMS_OUTPUT_ERROR_STRU          *pstImsErrorInfo
);
extern VOS_UINT32 IMSA_RegCalculateRetryDelayTime
(
    VOS_UINT32                          ulMaxTime,
    VOS_UINT32                          ulBaseTime,
    VOS_UINT32                          ulConsecutiveFailures
);
extern VOS_UINT32 IMSA_RegCalculateRegFailureRetryIntervelTime
(
    VOS_UINT32                          ulRetryTimes,
    VOS_UINT32                          ulRetryAfter,
    VOS_UINT32                          ulIsReRegisteration
);
extern VOS_VOID IMSA_RegFsmProcRegisteringImsRegFailure305
(
    IMSA_REG_ENTITY_STRU               *pstEntity
);
extern VOS_VOID IMSA_RegWaitForRetryCommonProc
(
    IMSA_REG_ENTITY_STRU               *pstEntity,
    VOS_UINT32                          ulRetryAfter,
    VOS_UINT32                          ulIsReRegisteration
);
extern VOS_UINT32 IMSA_RegProcImsMsgState(const IMSA_IMS_OUTPUT_SERVICE_EVENT_STRU  *pstOutputService);
extern IMSA_REG_ADDR_PAIR_STRU* IMSA_RegAddrPairCreate
(
    IMSA_IP_TYPE_ENUM_UINT8 enIpType,
    const VOS_CHAR *pacUeAddr,
    const VOS_CHAR *pacPcscfAddr
);
extern VOS_VOID IMSA_RegAddrPairDestroy(IMSA_REG_ADDR_PAIR_STRU *pstAddrPair);
extern IMSA_REG_ADDR_PAIR_STRU* IMSA_RegAddrPairListAdd
(
    IMSA_REG_ADDR_PAIR_STRU **ppstListHead,
    IMSA_REG_ADDR_PAIR_STRU *pstNode
);
extern VOS_VOID IMSA_RegAddrPairListDestroy(IMSA_REG_ADDR_PAIR_STRU *pstListHead);
extern VOS_UINT32 IMSA_RegAddrPairMgrUpdateWithNewPcscfAddr
(
    IMSA_REG_ADDR_PAIR_MGR_STRU *pstAddrPairMgr,
    IMSA_IP_TYPE_ENUM_UINT8 enIpType,
    const VOS_CHAR *pacPcscfAddr
);
extern VOS_UINT32 IMSA_RegAddrPairMgrUpdateWithNewUeAddr
(
    IMSA_REG_ADDR_PAIR_MGR_STRU *pstAddrPairMgr,
    IMSA_IP_TYPE_ENUM_UINT8 enIpType,
    const VOS_CHAR *pacUeAddr
);
extern VOS_VOID IMSA_RegGetAddrPairListHead
(
    IMSA_REG_TYPE_ENUM_UINT8            ulRegType,
    IMSA_IP_TYPE_ENUM_UINT8             enIpType,
    IMSA_REG_ADDR_PAIR_STRU          ***pppstListHead
);
extern VOS_VOID IMSA_RegAddrPairListClearAllUsedFlag(IMSA_REG_ADDR_PAIR_STRU *pstList);

extern VOS_UINT32 IMSA_CONN_PdnInfoNicConfig (IMS_NIC_PDN_INFO_CONFIG_STRU *pstConfigInfo);

extern VOS_VOID IMSA_CONN_DeleteEmcPdpInfo
(
    VOS_UINT8                               ucCid
);


extern VOS_VOID IMSA_CONN_PdnInfoNicDelete
(
    VOS_UINT8                           ucPdpId
);


extern VOS_VOID IMSA_CONN_ProcReEstablishConnSetupReq
(
    const IMSA_CONN_SETUP_REQ_STRU     *pstConnSetupReqMsg
);


/*lint -e960*/
/*lint -e961*/
/*****************************************************************************
  3 Function
*****************************************************************************/


VOS_VOID IMSA_CONN_ClearNicPdpInfo(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();

    IMSA_INFO_LOG("IMSA_CONN_InitNicPdpInfo is entered!");

    IMSA_MEM_SET(pstConnManager->astNicPdpInfoArray, 0, sizeof(IMSA_CONN_NIC_PDP_INFO_STRU)*IMSA_CONN_MAX_NIC_PDP_NUM);
}


VOS_VOID IMSA_CONN_DeletePdpId2Nic(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32              i                = 0;

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        IMSA_CONN_PdnInfoNicDelete(pstConnManager->astNicPdpInfoArray[i].ucPdpId);
    }
}



VOS_VOID IMSA_CONN_Init( VOS_VOID )
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstEmcConn      = IMSA_CONN_GetEmcConnAddr();

    IMSA_MEM_SET(   pstEmcConn, 0x0, sizeof(IMSA_EMC_CONN_STRU));
    pstEmcConn->enImsaConnStatus            = IMSA_CONN_STATUS_IDLE;
    pstEmcConn->enFirstReqPdnType           = IMSA_IP_TYPE_BUTT;
    pstEmcConn->enCurReqPdnType             = IMSA_IP_TYPE_BUTT;
    pstEmcConn->stProtectTimer.phTimer      = VOS_NULL_PTR;
    pstEmcConn->ucCurMaxOpid                = IMSA_CONN_EMC_CONN_MIN_OPID_VALUE;

    /* 初始化普通连接 */
    pstNormalConn   = IMSA_CONN_GetNormalConnAddr();
    IMSA_MEM_SET(   pstNormalConn, 0x0, sizeof(IMSA_NORMAL_CONN_STRU));
    pstNormalConn->enImsaConnStatus         = IMSA_CONN_STATUS_IDLE;
    pstNormalConn->enFirstReqPdnType        = IMSA_IP_TYPE_BUTT;
    pstNormalConn->enCurReqPdnType          = IMSA_IP_TYPE_BUTT;
    pstNormalConn->stProtectTimer.phTimer   = VOS_NULL_PTR;
    pstNormalConn->ucCurMaxOpid             = IMSA_CONN_NORMAL_CONN_MIN_OPID_VALUE;

    /* 初始化存储配置给NIC的PDP信息 */
    IMSA_CONN_ClearNicPdpInfo();

    return ;
}

/* lihong00150010 volte phaseiii begin */

VOS_VOID IMSA_CONN_PdnInfoNicDelete
(
    VOS_UINT8                           ucPdpId
)
{
    IMSA_INFO_LOG("before IMS_NIC_PdnInfoDel");

    (VOS_VOID)IMS_NIC_PdnInfoDel(ucPdpId, MODEM_ID_0);

    IMSA_INFO_LOG("after IMS_NIC_PdnInfoDel");
}
/* lihong00150010 volte phaseiii end */

VOS_VOID IMSA_CONN_ClearResource( VOS_VOID )
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    /* VOS_UINT32                          i               = 0; */

    /* 初始化紧急连接 */
    pstEmcConn      = IMSA_CONN_GetEmcConnAddr();

    /* 停止保护定时器 */
    IMSA_StopTimer(&pstEmcConn->stProtectTimer);

    /* lihong00150010 volte phaseiii begin */
    /* 通知NIC删除承载信息 */
    #if 0
    if (IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
    {
        IMSA_CONN_PdnInfoNicDelete(pstEmcConn->stSipSignalPdp.ucPdpId);
    }
    #endif
    /* lihong00150010 volte phaseiii end */

    pstEmcConn->enImsaConnStatus            = IMSA_CONN_STATUS_IDLE;
    pstEmcConn->enFirstReqPdnType           = IMSA_IP_TYPE_BUTT;
    pstEmcConn->enCurReqPdnType             = IMSA_IP_TYPE_BUTT;
    pstEmcConn->stProtectTimer.phTimer      = VOS_NULL_PTR;
    pstEmcConn->ucCurMaxOpid                = IMSA_CONN_EMC_CONN_MIN_OPID_VALUE;
    pstEmcConn->ucOpid                      = IMSA_NULL;
    IMSA_MEM_SET(   &pstEmcConn->stSelSdfPara,
                    0x0,
                    sizeof(IMSA_SEL_SDF_PARA_STRU));

    IMSA_MEM_SET(   &pstEmcConn->stSipSignalPdp,
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU));

    pstEmcConn->ulSipMediaPdpNum        = IMSA_NULL;
    IMSA_MEM_SET(   pstEmcConn->astSipMediaPdpArray,
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM);

    /* 初始化普通连接 */
    pstNormalConn   = IMSA_CONN_GetNormalConnAddr();
    IMSA_StopTimer(&pstNormalConn->stProtectTimer);

    /* lihong00150010 volte phaseiii begin */
    /* 通知NIC删除承载信息 */
    #if 0
    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState)
        {
            IMSA_CONN_PdnInfoNicDelete(pstNormalConn->astSipSignalPdpArray[i].ucPdpId);
        }
    }
    #endif
    /* lihong00150010 volte phaseiii end */

    pstNormalConn->enImsaConnStatus         = IMSA_CONN_STATUS_IDLE;
    pstNormalConn->enFirstReqPdnType        = IMSA_IP_TYPE_BUTT;
    pstNormalConn->enCurReqPdnType          = IMSA_IP_TYPE_BUTT;
    pstNormalConn->stProtectTimer.phTimer   = VOS_NULL_PTR;
    pstNormalConn->ucCurMaxOpid             = IMSA_CONN_NORMAL_CONN_MIN_OPID_VALUE;
    pstNormalConn->ucOpid                   = IMSA_NULL;
    pstNormalConn->ulSipSignalPdpNum        = IMSA_NULL;
    IMSA_MEM_SET(   pstNormalConn->astSipSignalPdpArray,
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM);

    pstNormalConn->ulSipMediaPdpNum        = IMSA_NULL;
    IMSA_MEM_SET(   pstNormalConn->astSipMediaPdpArray,
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM);

    /* 将NIC PDP INFO中存储的承载号通知TTF删除 */
    IMSA_CONN_DeletePdpId2Nic();

    /* 清除存储配置给NIC的PDP信息*/
    IMSA_CONN_ClearNicPdpInfo();

    return ;
}



VOS_VOID IMSA_CONN_AssignOpid
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT8                          *pucOpid
)
{
    VOS_UINT8                           ucOpid = IMSA_NULL;

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        /*获取当前最大OPID*/
        ucOpid = IMSA_CONN_GetNormalConnCurMaxOpid();

        /*如果还未到最大值，直接加1*/
        if( ucOpid < IMSA_CONN_NORMAL_CONN_MAX_OPID_VALUE)
        {
            *pucOpid = ucOpid + 1;
        }
        else/*如果已经是最大值，从初值开始分配*/
        {
            (*pucOpid) = IMSA_CONN_NORMAL_CONN_MIN_OPID_VALUE;
        }

        IMSA_CONN_SetNormalConnCurMaxOpid(*pucOpid);

        IMSA_INFO_LOG1("IMSA_CONN_AssignOpid,normal,assin opid:", (*pucOpid));
    }
    else
    {
        /*获取当前最大OPID*/
        ucOpid = IMSA_CONN_GetEmcConnCurMaxOpid();

        /*如果还未到最大值，直接加1*/
        if( ucOpid < IMSA_CONN_EMC_CONN_MAX_OPID_VALUE)
        {
            *pucOpid = ucOpid + 1;
        }
        else/*如果已经是最大值，从初值开始分配*/
        {
            (*pucOpid) = IMSA_CONN_EMC_CONN_MIN_OPID_VALUE;
        }

        IMSA_CONN_SetEmcConnCurMaxOpid(*pucOpid);

        IMSA_INFO_LOG1("IMSA_CONN_AssignOpid,emc,assin opid:", (*pucOpid));
    }
}



VOS_UINT32 IMSA_CONN_GetConnTypeByOpid
(
    VOS_UINT8                           ucOpid,
    IMSA_CONN_TYPE_ENUM_UINT32         *penConnType
)
{
    if (ucOpid == IMSA_CONN_GetNormalConnOpid())
    {
        *penConnType = IMSA_CONN_TYPE_NORMAL;
        return IMSA_SUCC;
    }

    if (ucOpid == IMSA_CONN_GetEmcConnOpid())
    {
        *penConnType = IMSA_CONN_TYPE_EMC;
        return IMSA_SUCC;
    }

    return IMSA_FAIL;
}


IMSA_PDP_STATE_ENUM_UINT8 IMSA_CONN_GetSipSignalPdpState
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IP_TYPE_ENUM_UINT8             enIpType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT32                          i               = IMSA_NULL;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn  = IMSA_CONN_GetEmcConnAddr();

        if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
            && (enIpType == pstEmcConn->stSipSignalPdp.stPdpAddr.enIpType))
        {
            return IMSA_PDP_STATE_ACTIVE;
        }

        return IMSA_PDP_STATE_INACTIVE;
    }

    pstNormalConn   = IMSA_CONN_GetNormalConnAddr();
    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ((IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState)
            && (enIpType == pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.enIpType))
        {
            return IMSA_PDP_STATE_ACTIVE;
        }
    }

    return IMSA_PDP_STATE_INACTIVE;
}


VOS_UINT32 IMSA_CONN_IsConnStatusEqual
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_CONN_STATUS_ENUM_UINT8         enImsaConnStatus
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enImsaConnStatusTmp = IMSA_CONN_STATUS_BUTT;

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enImsaConnStatusTmp = IMSA_CONN_GetNormalConnStatus();
    }
    else
    {
        enImsaConnStatusTmp = IMSA_CONN_GetEmcConnStatus();
    }

    if (enImsaConnStatus == enImsaConnStatusTmp)
    {
        return IMSA_TRUE;
    }
    else
    {
        return IMSA_FALSE;
    }
}


VOS_VOID IMSA_CONN_ClearConnResource
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();

        IMSA_StopTimer(&pstEmcConn->stProtectTimer);
        pstEmcConn->enCurReqPdnType     = IMSA_IP_TYPE_BUTT;
        pstEmcConn->enFirstReqPdnType   = IMSA_IP_TYPE_BUTT;
        pstEmcConn->enImsaConnStatus    = IMSA_CONN_STATUS_IDLE;
        pstEmcConn->ucOpid              = IMSA_CONN_ILLEGAL_OPID;
        IMSA_MEM_SET(                   &pstEmcConn->stSelSdfPara,
                                        0x0,
                                        sizeof(IMSA_SEL_SDF_PARA_STRU));

        IMSA_MEM_SET(                   &pstEmcConn->stSipSignalPdp,
                                        0x0,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU));

        pstEmcConn->ulSipMediaPdpNum    = 0;
        IMSA_MEM_SET(                   pstEmcConn->astSipMediaPdpArray,
                                        0x0,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM);
    }
    else
    {
        pstNormalConn = IMSA_CONN_GetNormalConnAddr();
        IMSA_StopTimer(&pstNormalConn->stProtectTimer);
        pstNormalConn->enCurReqPdnType  = IMSA_IP_TYPE_BUTT;
        pstNormalConn->enFirstReqPdnType= IMSA_IP_TYPE_BUTT;
        pstNormalConn->enImsaConnStatus = IMSA_CONN_STATUS_IDLE;
        pstNormalConn->ucOpid           = IMSA_CONN_ILLEGAL_OPID;
        IMSA_MEM_SET(                   &pstNormalConn->stSelSdfPara,
                                        0x0,
                                        sizeof(IMSA_SEL_SDF_PARA_STRU));
        pstNormalConn->ulSipSignalPdpNum= 0;
        IMSA_MEM_SET(                   pstNormalConn->astSipSignalPdpArray,
                                        0x0,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM);
        pstNormalConn->ulSipMediaPdpNum = 0;
        IMSA_MEM_SET(                   pstNormalConn->astSipMediaPdpArray,
                                        0x0,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM);
    }
}


VOS_UINT32 IMSA_CONN_HasActiveSipSignalPdp
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn  = IMSA_CONN_GetEmcConnAddr();

        if (IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
        {
            return IMSA_TRUE;
        }
        else
        {
            return IMSA_FALSE;
        }
    }
    else
    {
        pstNormalConn = IMSA_CONN_GetNormalConnAddr();

        if (0 != pstNormalConn->ulSipSignalPdpNum)
        {
            return IMSA_TRUE;
        }
        else
        {
            return IMSA_FALSE;
        }
    }
}


VOS_UINT32 IMSA_CONN_GetPdpContextByPdpId
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT8                           ucPdpId,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32 *penSipPdpType,
    IMSA_PDP_CNTXT_INFO_STRU          **ppstPdpContext
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT32                          i               = IMSA_NULL;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn          = IMSA_CONN_GetEmcConnAddr();

        if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
            && ((ucPdpId == pstEmcConn->stSipSignalPdp.ucPdpId)))
        {
            *ppstPdpContext = &pstEmcConn->stSipSignalPdp;
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;

            return IMSA_SUCC;
        }

        for (i = 0; i < pstEmcConn->ulSipMediaPdpNum; i++)
        {
            if ((ucPdpId == pstEmcConn->astSipMediaPdpArray[i].ucPdpId)
                && (IMSA_PDP_STATE_ACTIVE == pstEmcConn->astSipMediaPdpArray[i].enPdpState))
            {
                *ppstPdpContext = &pstEmcConn->astSipMediaPdpArray[i];
                *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_MEDIA;
                return IMSA_SUCC;
            }
        }

        return IMSA_FAIL;
    }

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ((ucPdpId == pstNormalConn->astSipSignalPdpArray[i].ucPdpId)
            && (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState))
        {
            *ppstPdpContext = &pstNormalConn->astSipSignalPdpArray[i];
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;
            return IMSA_SUCC;
        }
    }

    for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
    {
        if ((ucPdpId == pstNormalConn->astSipMediaPdpArray[i].ucPdpId)
            && (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState))
        {
            *ppstPdpContext = &pstNormalConn->astSipMediaPdpArray[i];
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_MEDIA;
            return IMSA_SUCC;
        }
    }

    return IMSA_FAIL;
}


VOS_UINT32 IMSA_CONN_GetPdpContextByCid
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT8                           ucCid,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32 *penSipPdpType,
    IMSA_PDP_CNTXT_INFO_STRU          **ppstPdpContext
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT32                          i               = IMSA_NULL;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn          = IMSA_CONN_GetEmcConnAddr();

        if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
            && ((ucCid == pstEmcConn->stSipSignalPdp.ucCid)))
        {
            *ppstPdpContext = &pstEmcConn->stSipSignalPdp;
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;

            return IMSA_SUCC;
        }

        for (i = 0; i < pstEmcConn->ulSipMediaPdpNum; i++)
        {
            if ((ucCid == pstEmcConn->astSipMediaPdpArray[i].ucCid)
                && (IMSA_PDP_STATE_ACTIVE == pstEmcConn->astSipMediaPdpArray[i].enPdpState))
            {
                *ppstPdpContext = &pstEmcConn->astSipMediaPdpArray[i];
                *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_MEDIA;
                return IMSA_SUCC;
            }
        }

        return IMSA_FAIL;
    }

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ((ucCid == pstNormalConn->astSipSignalPdpArray[i].ucCid)
            && (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState))
        {
            *ppstPdpContext = &pstNormalConn->astSipSignalPdpArray[i];
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;
            return IMSA_SUCC;
        }
    }

    for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
    {
        if ((ucCid == pstNormalConn->astSipMediaPdpArray[i].ucCid)
            && (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState))
        {
            *ppstPdpContext = &pstNormalConn->astSipMediaPdpArray[i];
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_MEDIA;
            return IMSA_SUCC;
        }
    }

    return IMSA_FAIL;
}


VOS_UINT32 IMSA_CONN_GetTimerLen
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_TIMER_ID_ENUM_UINT16           enTimerId
)
{
    if (TI_IMSA_SIP_SIGAL_PDP_ORIG == enTimerId)
    {
        if (IMSA_CONN_TYPE_NORMAL == enConnType)
        {
            return TI_IMSA_NRM_SIP_SIGAL_PDP_ORIG_TIMER_LEN;
        }
        else
        {
            return TI_IMSA_EMC_SIP_SIGAL_PDP_ORIG_TIMER_LEN;
        }
    }

    if (TI_IMSA_SIP_SIGAL_PDP_END == enTimerId)
    {
        return TI_IMSA_SIP_SIGAL_PDP_END_TIMER_LEN;
    }

    if (TI_IMSA_WAIT_IPV6_INFO == enTimerId)
    {
        return TI_IMSA_WAIT_IPV6_INFO_TIMER_LEN;
    }

    return 0;
}


VOS_VOID IMSA_CONN_StartTimer
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_TIMER_ID_ENUM_UINT16           enTimerId
)
{
    VOS_UINT32                          ulTimerLen      = IMSA_NULL;
    IMSA_TIMER_STRU                    *pstProtectImer  = VOS_NULL_PTR;

    /* 获取定时器时长 */
    ulTimerLen                  = IMSA_CONN_GetTimerLen(enConnType, enTimerId);

    IMSA_INFO_LOG2("IMSA_CONN_StartTimer:enConnType:Timer Len:", enConnType, ulTimerLen);

    if (IMSA_CONN_TYPE_NORMAL   == enConnType)
    {
        pstProtectImer          = IMSA_CONN_GetNormalConnProtectTimerAddr();

    }
    else
    {
        pstProtectImer          = IMSA_CONN_GetEmcConnProtectTimerAddr();
    }

    /* 赋值定时器参数 */
    pstProtectImer->ulTimerLen  = ulTimerLen;
    pstProtectImer->usName      = enTimerId;
    pstProtectImer->usPara      = (VOS_UINT16)enConnType;
    pstProtectImer->ucMode      = VOS_RELTIMER_NOLOOP;

    IMSA_StartTimer(pstProtectImer);
}


VOS_VOID IMSA_CONN_StopTimer
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_TIMER_ID_ENUM_UINT16           enTimerId
)
{
    IMSA_TIMER_STRU                    *pstProtectImer  = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_NORMAL   == enConnType)
    {
        pstProtectImer          = IMSA_CONN_GetNormalConnProtectTimerAddr();

    }
    else
    {
        pstProtectImer          = IMSA_CONN_GetEmcConnProtectTimerAddr();
    }

    if (pstProtectImer->usName == enTimerId)
    {
        IMSA_StopTimer(pstProtectImer);
    }
}


VOS_VOID IMSA_CONN_SetConnStatus
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus
)
{
    /* 获取连接原状态 */

    /* 打印连接类型 */

    /* 打印连接状态变更新信息 */

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        IMSA_CONN_GetNormalConnStatus() = enConnStatus;
    }
    else
    {
        IMSA_CONN_GetEmcConnStatus()    = enConnStatus;
    }
}
VOS_VOID IMSA_CONN_SaveFirstReqPdnType
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IP_TYPE_ENUM_UINT8             enIpType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn       = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn          = VOS_NULL_PTR;

    /* 获取PRIM PDP参数的格式 */
    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        pstNormalConn                   = IMSA_CONN_GetNormalConnAddr();
        pstNormalConn->enFirstReqPdnType= enIpType;
    }
    else
    {
        pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();
        pstEmcConn->enFirstReqPdnType   = enIpType;
    }
}
VOS_VOID IMSA_CONN_SaveCurReqPdnType
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IP_TYPE_ENUM_UINT8             enIpType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn       = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn          = VOS_NULL_PTR;

    /* 获取PRIM PDP参数的格式 */
    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        pstNormalConn                   = IMSA_CONN_GetNormalConnAddr();
        pstNormalConn->enCurReqPdnType  = enIpType;
    }
    else
    {
        pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();
        pstEmcConn->enCurReqPdnType     = enIpType;
    }
}
VOS_UINT32 IMSA_CONN_IsImsSdf
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
)
{
    if ((IMSA_OP_TRUE == pstSdfPara->bitOpImCnSignalFlg)
       && (TAF_PDP_FOR_IM_CN_SIG_ONLY == pstSdfPara->enImCnSignalFlg))
    {
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_IsPndTypeValid
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
)
{
    if ((IMSA_OP_TRUE == pstSdfPara->bitOpPdnType)
       && (TAF_PDP_IPV4 <= pstSdfPara->enPdnType)
       && (TAF_PDP_IPV4V6 >= pstSdfPara->enPdnType))
    {
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_IsEmcSdf
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
)
{
    if ((IMSA_OP_TRUE == pstSdfPara->bitOpEmergencyInd)
       && (TAF_PDP_FOR_EMC == pstSdfPara->enEmergencyInd))
    {
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_FindSipSignalReStablishDialPara
(
    VOS_UINT8                           ucCid,
    VOS_UINT32                          ulSdfNum,
    const TAF_SDF_PARA_STRU            *pstSdfPara,
    VOS_UINT32                         *pulIndex
)
{
    VOS_UINT32                          i = IMSA_NULL;

    if (ulSdfNum == 0)
    {
        IMSA_ERR_LOG("IMSA_CONN_FindSipSignalReStablishDialPara:No Sdf!");
        return IMSA_FAIL;
    }

    for (i = 0; i < ulSdfNum; i++)
    {
        if (ucCid == pstSdfPara[i].ucCid)
        {
            *pulIndex = i;

            return IMSA_SUCC;
        }
    }
    return IMSA_FAIL;
}



VOS_UINT32 IMSA_CONN_FindSipSignalDialPara
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT32                          ulSdfNum,
    const TAF_SDF_PARA_STRU            *pstSdfPara,
    VOS_UINT32                         *pulIndex
)
{
    VOS_UINT32                          i = IMSA_NULL;

    if (ulSdfNum == 0)
    {
        IMSA_ERR_LOG("IMSA_CONN_FindSipSignalDialPara:No Sdf!");
        return IMSA_FAIL;
    }

    /* 普通连接选择具有TAF_PDP_FOR_IMS_CN_SIG_ONLY标识，PDN类型和APN参数，
       且参数合法有效的CID；紧急连接选择具有TAF_PDP_FOR_IMS_CN_SIG_ONLY标识
       和TAF_PDP_FOR_EMC，PDN类型和APN参数，且参数合法有效的CID */
    for (i = 0; i < ulSdfNum; i++)
    {
        if (IMSA_OP_TRUE == pstSdfPara[i].bitOpLinkdCid)
        {
            continue;
        }

        if (IMSA_TRUE != IMSA_CONN_IsImsSdf(&pstSdfPara[i]))
        {
            continue;
        }

        if (IMSA_TRUE != IMSA_CONN_IsPndTypeValid(&pstSdfPara[i]))
        {
            continue;
        }

        if (IMSA_CONN_TYPE_EMC == enConnType)
        {
            if (IMSA_TRUE != IMSA_CONN_IsEmcSdf(&pstSdfPara[i]))
            {
                continue;
            }
        }
        else/* 如果是普通类型，则不选择有紧急标识的CID */
        {
            if (IMSA_OP_TRUE != pstSdfPara[i].bitOpApn)
            {
                continue;
            }

            if (IMSA_TRUE == IMSA_CONN_IsEmcSdf(&pstSdfPara[i]))
            {
                continue;
            }
        }

        *pulIndex = i;

        return IMSA_SUCC;
    }

    return IMSA_FAIL;
}
VOS_VOID IMSA_CONN_SaveSelectedSdfPara
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const TAF_SDF_PARA_STRU            *pstTafSdfPara
)
{
    IMSA_SEL_SDF_PARA_STRU             *pstSelSdfPara = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstSelSdfPara = IMSA_CONN_GetEmcConnSelSdfParaAddr();
    }
    else
    {
        pstSelSdfPara = IMSA_CONN_GetNormalConnSelSdfParaAddr();
    }

    IMSA_MEM_SET(pstSelSdfPara, 0x0, sizeof(IMSA_SEL_SDF_PARA_STRU));

    pstSelSdfPara->ucCid                    = pstTafSdfPara->ucCid;

    pstSelSdfPara->bitOpLinkdCid            = pstTafSdfPara->bitOpLinkdCid;
    pstSelSdfPara->ucLinkdCid               = pstTafSdfPara->ucLinkdCid;

    pstSelSdfPara->bitOpPdnType             = pstTafSdfPara->bitOpPdnType;
    pstSelSdfPara->enPdnType                = pstTafSdfPara->enPdnType;

    pstSelSdfPara->bitOpApn = pstTafSdfPara->bitOpApn;
    IMSA_MEM_CPY(   &pstSelSdfPara->stApnInfo,
                    &pstTafSdfPara->stApnInfo,
                    sizeof(TAF_PDP_APN_STRU));

    pstSelSdfPara->bitOpGwAuthInfo = pstTafSdfPara->bitOpGwAuthInfo;
    IMSA_MEM_CPY(   &pstSelSdfPara->stGwAuthInfo,
                    &pstTafSdfPara->stGwAuthInfo,
                    sizeof(TAF_GW_AUTH_STRU));

    pstSelSdfPara->bitOpEmergencyInd        = pstTafSdfPara->bitOpEmergencyInd;
    pstSelSdfPara->enEmergencyInd           = pstTafSdfPara->enEmergencyInd;

    pstSelSdfPara->bitOpIpv4AddrAllocType   = pstTafSdfPara->bitOpIpv4AddrAllocType;
    pstSelSdfPara->enIpv4AddrAllocType      = pstTafSdfPara->enIpv4AddrAllocType;

    pstSelSdfPara->bitOpPcscfDiscovery      = pstTafSdfPara->bitOpPcscfDiscovery;
    pstSelSdfPara->enPcscfDiscovery         = pstTafSdfPara->enPcscfDiscovery;

    pstSelSdfPara->bitOpImCnSignalFlg       = pstTafSdfPara->bitOpImCnSignalFlg;
    pstSelSdfPara->enImCnSignalFlg          = pstTafSdfPara->enImCnSignalFlg;
}


VOS_VOID IMSA_CONN_GetPrimPdpCntFromSelSdfPara
(
    const IMSA_SEL_SDF_PARA_STRU       *pstSelectedSdfPara,
    TAF_PDP_PRIM_CONTEXT_EXT_STRU      *pstPdpPrimContextExt
)
{
    IMSA_MEM_SET(                       (VOS_VOID *)pstPdpPrimContextExt,
                                        0x00,
                                        sizeof(TAF_PDP_PRIM_CONTEXT_EXT_STRU));

    pstPdpPrimContextExt->ucCid         = pstSelectedSdfPara->ucCid;
    pstPdpPrimContextExt->ucDefined     = 1;

    /* 赋值PDN TYPE */
    pstPdpPrimContextExt->bitOpPdpType  = pstSelectedSdfPara->bitOpPdnType;
    pstPdpPrimContextExt->enPdpType     = pstSelectedSdfPara->enPdnType;

    /* 赋值APN */
    pstPdpPrimContextExt->bitOpApn      = pstSelectedSdfPara->bitOpApn;
    IMSA_MEM_CPY(                       (VOS_CHAR*)pstPdpPrimContextExt->aucApn,
                                        (VOS_CHAR*)pstSelectedSdfPara->stApnInfo.aucValue,
                                        pstSelectedSdfPara->stApnInfo.ucLength);

    /* 赋值Ipv4AddrAlloc */
    pstPdpPrimContextExt->bitOpIpv4AddrAlloc    = pstSelectedSdfPara->bitOpIpv4AddrAllocType;
    pstPdpPrimContextExt->enIpv4AddrAlloc       = pstSelectedSdfPara->enIpv4AddrAllocType;

    /* 赋值紧急标识 */
    pstPdpPrimContextExt->bitOpEmergencyInd     = pstSelectedSdfPara->bitOpEmergencyInd;
    pstPdpPrimContextExt->enEmergencyFlg        = pstSelectedSdfPara->enEmergencyInd;

    /* 赋值P-CSCF发现方式 */
    pstPdpPrimContextExt->bitOpPcscfDiscovery   = pstSelectedSdfPara->bitOpPcscfDiscovery;
    pstPdpPrimContextExt->enPcscfDiscovery      = pstSelectedSdfPara->enPcscfDiscovery;

    /* 赋值IMS标识 */
    pstPdpPrimContextExt->bitOpImCnSignalFlg    = pstSelectedSdfPara->bitOpImCnSignalFlg;
    pstPdpPrimContextExt->enImCnSignalFlg       = pstSelectedSdfPara->enImCnSignalFlg;
}



VOS_VOID IMSA_CONN_GetImsDailParaFromSelSdfPara
(
    const IMSA_SEL_SDF_PARA_STRU       *pstSelectedSdfPara,
    TAF_PS_DIAL_PARA_STRU              *pstDialParaInfo
)
{
    IMSA_MEM_SET(pstDialParaInfo, 0, sizeof(TAF_PS_DIAL_PARA_STRU));

    pstDialParaInfo->ucCid                  = pstSelectedSdfPara->ucCid;
    pstDialParaInfo->enPdpType              = pstSelectedSdfPara->enPdnType;

    /* 赋值APN信息 */
    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpApn)
    {
        pstDialParaInfo->bitOpApn           = IMSA_OP_TRUE;

        /* 拨号APN参数没有长度字段，是因为是字符串格式，已在结尾加上'\0' */
        IMSA_MEM_CPY(                       pstDialParaInfo->aucApn,
                                            pstSelectedSdfPara->stApnInfo.aucValue,
                                            pstSelectedSdfPara->stApnInfo.ucLength);
    }

    /* 赋值鉴权参数 */
    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpGwAuthInfo)
    {
        pstDialParaInfo->bitOpAuthType      = IMSA_OP_TRUE;
        pstDialParaInfo->enAuthType         = pstSelectedSdfPara->stGwAuthInfo.enAuthType;

        if (pstSelectedSdfPara->stGwAuthInfo.ucUserNameLen != 0)
        {
            pstDialParaInfo->bitOpUserName  = IMSA_OP_TRUE;

            /* 拨号用户名参数没有长度字段，是因为是字符串格式，已在结尾加上'\0' */
            IMSA_MEM_CPY(                   pstDialParaInfo->aucUserName,
                                            pstSelectedSdfPara->stGwAuthInfo.aucUserName,
                                            pstSelectedSdfPara->stGwAuthInfo.ucUserNameLen);
        }

        if (pstSelectedSdfPara->stGwAuthInfo.ucPwdLen       != 0)
        {
            pstDialParaInfo->bitOpPassWord  = IMSA_OP_TRUE;

            /* 拨号密码参数没有长度字段，是因为是字符串格式，已在结尾加上'\0' */
            IMSA_MEM_CPY(                   pstDialParaInfo->aucPassWord,
                                            pstSelectedSdfPara->stGwAuthInfo.aucPwd,
                                            pstSelectedSdfPara->stGwAuthInfo.ucPwdLen);
        }
    }

    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpIpv4AddrAllocType)
    {
        pstDialParaInfo->bitOpIpv4AddrAlloc = IMSA_OP_TRUE;
        pstDialParaInfo->enIpv4AddrAlloc = pstSelectedSdfPara->enIpv4AddrAllocType;
    }

    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpEmergencyInd)
    {
        pstDialParaInfo->bitOpEmergencyInd = IMSA_OP_TRUE;
        pstDialParaInfo->enEmergencyInd = pstSelectedSdfPara->enEmergencyInd;
    }

    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpPcscfDiscovery)
    {
        pstDialParaInfo->bitOpPcscfDiscovery = IMSA_OP_TRUE;
        pstDialParaInfo->enPcscfDiscovery = pstSelectedSdfPara->enPcscfDiscovery;
    }

    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpImCnSignalFlg)
    {
        pstDialParaInfo->bitOpImCnSignalFlg = IMSA_OP_TRUE;
        pstDialParaInfo->enImCnSignalFlg = pstSelectedSdfPara->enImCnSignalFlg;
    }

    pstDialParaInfo->bitOpReqType = IMSA_OP_FALSE;
    pstDialParaInfo->bitOpPdpDcomp = IMSA_OP_FALSE;
    pstDialParaInfo->bitOpPdpHcomp = IMSA_OP_FALSE;

}
VOS_VOID IMSA_CONN_ConvertIpAddress2String
(
    IMSA_IP_TYPE_ENUM_UINT8             enIpType,
    const VOS_UINT8                    *pucSrc,
    VOS_CHAR                           *pcDst
)
{
#if (VOS_OS_VER != VOS_WIN32)
/*lint -e718 -e746 -e40 */
    if (IMSA_IP_TYPE_IPV4 == enIpType)
    {
        inet_ntop(AF_INET, pucSrc , pcDst, 16);
    }
    else
    {
        inet_ntop(AF_INET6, pucSrc , pcDst, 46);
    }
/*lint +e718 +e746 +e40 */
#else
    VOS_UINT8                           i = 0;

    if (IMSA_IP_TYPE_IPV4 == enIpType)
    {
        /* ST中的IPV6地址和IPV6类型的P-CSCF地址的最后一个字节必须是[0,9] */
        IMSA_MEM_CPY(pcDst, g_acIpStringForSt_ipv4, (IMSA_IPV4_ADDR_STRING_LEN + 1));
        i = strlen(g_acIpStringForSt_ipv4) - 1;
        pcDst[i] = pucSrc[3] + 0x30;
        /*g_acIpStringForSt_ipv4[i] ++ ;*/
    }
    else
    {
        /* ST中的IPV6地址和IPV6类型的P-CSCF地址的最后一个字节必须是大于等于16 */
        IMSA_MEM_CPY(pcDst, g_acIpStringForSt_ipv6, (IMSA_IPV6_ADDR_STRING_LEN + 1));
        i = strlen(g_acIpStringForSt_ipv6) - 1;
        pcDst[i] = (pucSrc[15] - 16) + 0x30;
        /*g_acIpStringForSt_ipv6[i] ++ ;*/
    }

#endif
}


VOS_VOID IMSA_CONN_ConfigPdpInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    VOS_CHAR                    acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_UINT8                   aucTemp[IMSA_IPV6_PREFIX_LEN]           = {0};

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpAddr.aucIpV4Addr,
                                            acUeAddr);

        (VOS_VOID)IMSA_RegAddrPairMgrAddUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                IMSA_IP_TYPE_IPV4,
                                                acUeAddr);

        /* 配置IPV4 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV6地址 */
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpAddr.aucIpV6Addr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                    IMSA_IP_TYPE_IPV6,
                                                    acUeAddr);
        }

        /* 配置IPV6 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }
    }
}


VOS_VOID IMSA_CONN_DeletePdpInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    VOS_CHAR                    acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 删除IPV4地址 */
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpAddr.aucIpV4Addr,
                                            acUeAddr);

        (VOS_VOID)IMSA_RegAddrPairMgrRmvUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                IMSA_IP_TYPE_IPV4,
                                                acUeAddr);

        /* 删除IPV4 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 删除IPV6地址 */
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                                            acUeAddr);

        (VOS_VOID)IMSA_RegAddrPairMgrRmvUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                IMSA_IP_TYPE_IPV6,
                                                acUeAddr);

        /* 删除IPV6 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }
    }
}


VOS_UINT32 IMSA_CONN_PdnInfoNicConfig (IMS_NIC_PDN_INFO_CONFIG_STRU *pstConfigInfo)
{
    VOS_UINT32                          ulRslt                          = IMSA_FAIL;

    IMSA_INFO_LOG("IMSA_CONN_PdnInfoNicConfig enter");
    IMSA_INFO_LOG1("IMSA_CONN_PdnInfoNicConfig ,usSockMaxPort=:", pstConfigInfo->stSockPortInfo.usSockMaxPort);
    IMSA_INFO_LOG1("IMSA_CONN_PdnInfoNicConfig ,usSockMinPort=:", pstConfigInfo->stSockPortInfo.usSockMinPort);

    vos_printf("IMSA_CONN_PdnInfoNicConfig enter");
    vos_printf("IMSA_CONN_PdnInfoNicConfig ,usSockMaxPort=: %d", pstConfigInfo->stSockPortInfo.usSockMaxPort);
    vos_printf("IMSA_CONN_PdnInfoNicConfig ,usSockMinPort=: %d", pstConfigInfo->stSockPortInfo.usSockMinPort);

    ulRslt = IMS_NIC_PdnInfoConfig(pstConfigInfo);

    IMSA_INFO_LOG("IMSA_CONN_PdnInfoNicConfig leave");
    vos_printf("IMSA_CONN_PdnInfoNicConfig leave");

    return ulRslt;
}


VOS_VOID IMSA_CONN_ConfigPdpIPv6Info2Bsp
(
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    IMS_NIC_PDN_INFO_CONFIG_STRU        stConfigInfo                    = {0};
    VOS_UINT32                          ulRslt                          = IMSA_FAIL;
    VOS_UINT8                           aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager               = VOS_NULL_PTR;

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            IMSA_MEM_SET(&stConfigInfo, 0, sizeof(IMS_NIC_PDN_INFO_CONFIG_STRU));
            stConfigInfo.ucRabId     = pstPdpInfo->ucPdpId;
            stConfigInfo.enModemId   = MODEM_ID_0;

            /* 配置IPV6地址 */
            stConfigInfo.bitOpIpv6PdnInfo    = IMSA_OP_TRUE;
            stConfigInfo.stIpv6PdnInfo.ulBitPrefixLen = IMSA_IPV6_PREFIX_BIT_LEN;
            IMSA_MEM_CPY(   stConfigInfo.stIpv6PdnInfo.aucIpV6Addr,
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IPV6 DNS地址 */
            stConfigInfo.stIpv6PdnInfo.bitOpDnsPrim = pstPdpInfo->stPdpIpv6Dns.bitOpPriDns;
            IMSA_MEM_CPY(   stConfigInfo.stIpv6PdnInfo.aucDnsPrimAddr,
                            pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                            IMSA_IPV6_ADDR_LEN);

            stConfigInfo.stIpv6PdnInfo.bitOpDnsSec = pstPdpInfo->stPdpIpv6Dns.bitOpSecDns;
            IMSA_MEM_CPY(   stConfigInfo.stIpv6PdnInfo.aucDnsSecAddr,
                            pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IP栈端口号范围 */
            pstControlManager = IMSA_GetControlManagerAddress();
            stConfigInfo.bitOpSockPortInfo = IMSA_OP_TRUE;
            stConfigInfo.stSockPortInfo.usSockMinPort
                    = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMinPort;
            stConfigInfo.stSockPortInfo.usSockMaxPort
                    = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMaxPort;

            ulRslt = IMSA_CONN_PdnInfoNicConfig(&stConfigInfo);
            if (IMS_NIC_SUCC != ulRslt)
            {
                IMSA_ERR_LOG1("IMS_NIC_PdnInfoConfig failed,result:", ulRslt);
            }
        }
    }
}
VOS_VOID IMSA_CONN_ConfigPdpInfo2Bsp
(
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    IMS_NIC_PDN_INFO_CONFIG_STRU        stConigInfo                     = {0};
    VOS_UINT32                          ulRslt                          = IMSA_FAIL;
    VOS_UINT8                           aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager               = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_ConfigPdpInfo2Bsp is entered!");

    IMSA_MEM_SET(&stConigInfo, 0, sizeof(IMS_NIC_PDN_INFO_CONFIG_STRU));

    stConigInfo.ucRabId     = pstPdpInfo->ucPdpId;
    stConigInfo.enModemId   = MODEM_ID_0;

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        stConigInfo.bitOpIpv4PdnInfo    = IMSA_OP_TRUE;
        IMSA_MEM_CPY(   stConigInfo.stIpv4PdnInfo.aucIpV4Addr,
                        pstPdpInfo->stPdpAddr.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN);

        /* 配置IPV4 DNS地址 */
        stConigInfo.stIpv4PdnInfo.bitOpDnsPrim = pstPdpInfo->stPdpIpv4Dns.bitOpPriDns;
        IMSA_MEM_CPY(   stConigInfo.stIpv4PdnInfo.aucDnsPrimAddr,
                        pstPdpInfo->stPdpIpv4Dns.aucPriDns,
                        IMSA_IPV4_ADDR_LEN);

        stConigInfo.stIpv4PdnInfo.bitOpDnsSec = pstPdpInfo->stPdpIpv4Dns.bitOpSecDns;
        IMSA_MEM_CPY(   stConigInfo.stIpv4PdnInfo.aucDnsSecAddr,
                        pstPdpInfo->stPdpIpv4Dns.aucSecDns,
                        IMSA_IPV4_ADDR_LEN);
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            /* 配置IPV6地址 */
            stConigInfo.bitOpIpv6PdnInfo    = IMSA_OP_TRUE;
            stConigInfo.stIpv6PdnInfo.ulBitPrefixLen = IMSA_IPV6_PREFIX_BIT_LEN;
            IMSA_MEM_CPY(   stConigInfo.stIpv6PdnInfo.aucIpV6Addr,
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IPV6 DNS地址 */
            stConigInfo.stIpv6PdnInfo.bitOpDnsPrim = pstPdpInfo->stPdpIpv6Dns.bitOpPriDns;
            IMSA_MEM_CPY(   stConigInfo.stIpv6PdnInfo.aucDnsPrimAddr,
                            pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                            IMSA_IPV6_ADDR_LEN);

            stConigInfo.stIpv6PdnInfo.bitOpDnsSec = pstPdpInfo->stPdpIpv6Dns.bitOpSecDns;
            IMSA_MEM_CPY(   stConigInfo.stIpv6PdnInfo.aucDnsSecAddr,
                            pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                            IMSA_IPV6_ADDR_LEN);
        }
    }

    if ((IMSA_OP_TRUE == stConigInfo.bitOpIpv4PdnInfo)
        || (IMSA_OP_TRUE == stConigInfo.bitOpIpv6PdnInfo))
    {
        pstControlManager = IMSA_GetControlManagerAddress();
        stConigInfo.bitOpSockPortInfo = IMSA_OP_TRUE;
        stConigInfo.stSockPortInfo.usSockMinPort
                = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMinPort;
        stConigInfo.stSockPortInfo.usSockMaxPort
                = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMaxPort;

        ulRslt = IMSA_CONN_PdnInfoNicConfig(&stConigInfo);
        if (IMS_NIC_SUCC != ulRslt)
        {
            IMSA_ERR_LOG1("IMS_NIC_PdnInfoConfig failed,result:", ulRslt);
        }
    }
}


VOS_VOID IMSA_CONN_SaveSipSignalPdpInfo
(
    IMSA_PDP_CNTXT_INFO_STRU                   *pstPdpInfo,
    const TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU    *pstPdpActivateCnf
)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;
    pstControlManager               = IMSA_GetControlManagerAddress();

    pstPdpInfo->ucPdpId             = pstPdpActivateCnf->ucRabId;
    pstPdpInfo->enPdpState          = IMSA_PDP_STATE_ACTIVE;
    if (IMSA_OP_TRUE == pstPdpActivateCnf->bitOpLinkdRabId)
    {
        pstPdpInfo->enPdpType = IMSA_PDP_TYPE_DEDICATED;
    }
    else if ((IMSA_OP_TRUE == pstPdpActivateCnf->bitOpEmergencyInd) && (TAF_PDP_FOR_EMC == pstPdpActivateCnf->enEmergencyInd))
    {
        pstPdpInfo->enPdpType = IMSA_PDP_TYPE_EMERGENCY;
    }
    else
    {
        pstPdpInfo->enPdpType = IMSA_PDP_TYPE_DEFAULT;
    }
    pstPdpInfo->ucCid               = pstPdpActivateCnf->ucCid;

    /* 存储IP地址 */
    pstPdpInfo->bitOpPdpAddr        = pstPdpActivateCnf->bitOpPdpAddr;
    IMSA_MEM_CPY(                   &pstPdpInfo->stPdpAddr,
                                    &pstPdpActivateCnf->stPdpAddr,
                                    sizeof(IMSA_IP_ADDRESS_STRU));

    switch(pstControlManager->stPcscfDiscoveryPolicyInfo.enPcscfDiscoverPolicy)
    {
    case IMSA_PCSCF_DISCOVERY_POLICY_PCO:

        /* 通过PCO获取IPV4 P-CSCF */
        IMSA_MEM_CPY(                   &pstPdpInfo->stPdpIpv4Pcscf,
                                        &pstPdpActivateCnf->stPcscf,
                                        sizeof(IMSA_PDP_IPV4_PCSCF_STRU));

        /* 通过PCO获取IPV6 P-CSCF */
        IMSA_MEM_CPY(                   &pstPdpInfo->stPdpIpv6Pcscf,
                                        &pstPdpActivateCnf->stIpv6Pcscf,
                                        sizeof(IMSA_PDP_IPV6_PCSCF_STRU));

        IMSA_INFO_LOG("IMSA_CONN_SaveSipSignalPdpInfo: IMSA_PCSCF_DISCOVERY_POLICY_PCO!");
    	break;
    case IMSA_PCSCF_DISCOVERY_POLICY_NV:

        /* 通过NV配置获取IPV4 P-CSCF */
        IMSA_MEM_CPY(                   &pstPdpInfo->stPdpIpv4Pcscf,
                                        &pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv4Pcscf,
                                        sizeof(IMSA_PDP_IPV4_PCSCF_STRU));

        /* 通过NV配置获取IPV6 P-CSCF */
        IMSA_MEM_CPY(                   &pstPdpInfo->stPdpIpv6Pcscf,
                                        &pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv6Pcscf,
                                        sizeof(IMSA_PDP_IPV6_PCSCF_STRU));

        IMSA_INFO_LOG("IMSA_CONN_SaveSipSignalPdpInfo: IMSA_PCSCF_DISCOVERY_POLICY_NV!");
    	break;
    default:
        IMSA_INFO_LOG("IMSA_CONN_SaveSipSignalPdpInfo:P-CSCF policy error!");
        break;
    }

    /* 存储IPV4 DNS */
    IMSA_MEM_CPY(                   &pstPdpInfo->stPdpIpv4Dns,
                                    &pstPdpActivateCnf->stDns,
                                    sizeof(IMSA_PDP_IPV4_DNS_STRU));

    /* 存储IPV6 DNS */
    IMSA_MEM_CPY(                   &pstPdpInfo->stPdpIpv6Dns,
                                    &pstPdpActivateCnf->stIpv6Dns,
                                    sizeof(IMSA_PDP_IPV6_DNS_STRU));

    #if 0
    /* 存储APN */
    pstPdpInfo->bitOpApn            = pstPdpActivateCnf->bitOpApn;
    IMSA_MEM_CPY(                   &pstPdpInfo->stApn,
                                    &pstPdpActivateCnf->stApn,
                                    sizeof(IMSA_PDP_APN_STRU));

    /* 存储UMTS QOS */
    IMSA_MEM_CPY(                   &pstPdpInfo->stUmtsQos,
                                    &pstPdpActivateCnf->stUmtsQos,
                                    sizeof(IMSA_PDP_UMTS_QOS_STRU));

    /* 存储EPS QOS */
    IMSA_MEM_CPY(                   &pstPdpInfo->stEpsQos,
                                    &pstPdpActivateCnf->stEpsQos,
                                    sizeof(IMSA_PDP_EPS_QOS_STRU));
    #endif
}


VOS_VOID IMSA_CONN_SaveSipMediaPdpInfo
(
    IMSA_CONN_TYPE_ENUM_UINT32                  enConnType,
    const TAF_PS_CALL_PDP_ACTIVATE_IND_STRU    *pstPdpActivateInd
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn       = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn          = VOS_NULL_PTR;
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpInfo          = VOS_NULL_PTR;

    pstNormalConn                   = IMSA_CONN_GetNormalConnAddr();
    pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();

    if (IMSA_CONN_TYPE_NORMAL       == enConnType)
    {
        pstPdpInfo                  = &pstNormalConn->astSipMediaPdpArray[pstNormalConn->ulSipMediaPdpNum];
        pstNormalConn->ulSipMediaPdpNum++;
    }
    else
    {
        pstPdpInfo                  = &pstEmcConn->astSipMediaPdpArray[pstEmcConn->ulSipMediaPdpNum];
        pstEmcConn->ulSipMediaPdpNum++;
    }

    pstPdpInfo->ucPdpId             = pstPdpActivateInd->ucRabId;
    pstPdpInfo->enPdpState          = IMSA_PDP_STATE_ACTIVE;
    pstPdpInfo->enPdpType           = IMSA_PDP_TYPE_DEDICATED;
    pstPdpInfo->ucCid               = pstPdpActivateInd->ucCid;

    /* 存储关联的信令承载号 */
    pstPdpInfo->bitOpLinkedPdpId    = pstPdpActivateInd->bitOpLinkdRabId;
    pstPdpInfo->ucLinkedPdpId       = pstPdpActivateInd->ucLinkdRabId;
    #if 0
    /* 存储UMTS QOS */
    IMSA_MEM_CPY(                   &pstPdpInfo->stUmtsQos,
                                    &pstPdpActivateInd->stUmtsQos,
                                    sizeof(IMSA_PDP_UMTS_QOS_STRU));

    /* 存储EPS QOS */
    IMSA_MEM_CPY(                   &pstPdpInfo->stEpsQos,
                                    &pstPdpActivateInd->stEpsQos,
                                    sizeof(IMSA_PDP_EPS_QOS_STRU));
    #endif
}
VOS_VOID IMSA_CONN_ModifySipPdpInfo
(
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32          enSipPdpType,
    IMSA_PDP_CNTXT_INFO_STRU                   *pstPdpInfo,
    const TAF_PS_CALL_PDP_MODIFY_IND_STRU      *pstPdpModifyInd
)
{
    #if 0
    /* 存储UMTS QOS */
    IMSA_MEM_CPY(       &pstPdpInfo->stUmtsQos,
                        &pstPdpModifyInd->stUmtsQos,
                        sizeof(IMSA_PDP_UMTS_QOS_STRU));

    /* 存储EPS QOS */
    IMSA_MEM_CPY(       &pstPdpInfo->stEpsQos,
                        &pstPdpModifyInd->stEpsQos,
                        sizeof(IMSA_PDP_EPS_QOS_STRU));
    #endif
    if (IMSA_CONN_SIP_PDP_TYPE_SIGNAL == enSipPdpType)
    {
        /* 存储IPV4 P-CSCF */
        IMSA_MEM_CPY(   &pstPdpInfo->stPdpIpv4Pcscf,
                        &pstPdpModifyInd->stPcscf,
                        sizeof(IMSA_PDP_IPV4_PCSCF_STRU));

        /* 存储IPV4 DNS */
        IMSA_MEM_CPY(   &pstPdpInfo->stPdpIpv4Dns,
                        &pstPdpModifyInd->stDns,
                        sizeof(IMSA_PDP_IPV4_DNS_STRU));

        /* 存储IPV6 P-CSCF */
        IMSA_MEM_CPY(   &pstPdpInfo->stPdpIpv6Pcscf,
                        &pstPdpModifyInd->stIpv6Pcscf,
                        sizeof(IMSA_PDP_IPV6_PCSCF_STRU));

        /* 存储IPV6 DNS */
        IMSA_MEM_CPY(   &pstPdpInfo->stPdpIpv6Dns,
                        &pstPdpModifyInd->stIpv6Dns,
                        sizeof(IMSA_PDP_IPV6_DNS_STRU));
    }
}


VOS_VOID IMSA_CONN_SaveIpv6Info
(
    IMSA_PDP_CNTXT_INFO_STRU                   *pstPdpInfo,
    const TAF_PS_IPV6_INFO_IND_STRU            *pstIpv6InfoInd
)
{
    /* 23.401要求3GPP的IPV6前缀必须是64位
       IPv6 Stateless Address autoconfiguration specified in RFC 4862 [18] is
       the basic mechanism to allocate /64 IPv6 prefix to the UE. */

    /* 29.061要求网侧分配的IPV6前缀是无限长的，因此无需考虑IPV6前缀的有效期
       AdvValidLifetime
           Shall have a value giving Prefixes infinite lifetime, i.e. 0xFFFFFFFF.
           The assigned prefix remains Preferred until PDP Context/Bearer Deactivation.
       AdvPreferredLifetime
           Shall have a value giving Prefixes infinite lifetime, i.e. 0xFFFFFFFF.
           The assigned prefix remains Preferred until PDP Context/Bearer Deactivation.
       */
    IMSA_MEM_CPY(   pstPdpInfo->stPdpAddr.aucIpV6Addr,
                    pstIpv6InfoInd->stIpv6RaInfo.astPrefixList[0].aucPrefix,
                    IMSA_IPV6_PREFIX_LEN);
}


VOS_UINT32 IMSA_CONN_IsPdpIpvxInfoIncludeRegPara
(
    IMSA_IP_TYPE_ENUM_UINT8                 enIpType,
    VOS_CHAR                               *pcRegUeAddr,
    VOS_CHAR                               *pcRegPcscfAddr,
    const IMSA_PDP_CNTXT_INFO_STRU         *pstPdpInfo
)
{
    VOS_CHAR                            acPdpUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acPdpPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    if (IMSA_IP_TYPE_IPV4 == enIpType)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpAddr.aucIpV4Addr,
                                            acPdpUeAddr);

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                                acPdpPcscfAddr);

            /* 比较地址对 */
            if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
                && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
            {
                return IMSA_TRUE;
            }
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                                acPdpPcscfAddr);

            /* 比较地址对 */
            if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
                && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
            {
                return IMSA_TRUE;
            }
        }
    }
    else
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                                            acPdpUeAddr);

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                                acPdpPcscfAddr);

            /* 比较地址对 */
            if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
                && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
            {
                return IMSA_TRUE;
            }
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                                acPdpPcscfAddr);

            /* 比较地址对 */
            if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
                && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
            {
                return IMSA_TRUE;
            }
        }
    }

    return IMSA_FALSE;
}



VOS_UINT32 IMSA_CONN_IsPdpInfoIncludeRegPara
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU         *pstPdpInfo
)
{
    VOS_CHAR                            acRegUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acRegPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_UINT32                          ulRslt          = IMSA_FAIL;

    ulRslt = IMSA_RegGetRegedPara(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                    acRegUeAddr,
                                    acRegPcscfAddr);

    /* 如果未获取到注册上的参数，则直接返回不包含 */
    if (IMSA_FAIL == ulRslt)
    {
        return IMSA_FALSE;
    }

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (IMSA_TRUE == IMSA_CONN_IsPdpIpvxInfoIncludeRegPara( IMSA_IP_TYPE_IPV4,
                                                                acRegUeAddr,
                                                                acRegPcscfAddr,
                                                                pstPdpInfo))
        {
            return IMSA_TRUE;
        }
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (IMSA_TRUE == IMSA_CONN_IsPdpIpvxInfoIncludeRegPara( IMSA_IP_TYPE_IPV6,
                                                                acRegUeAddr,
                                                                acRegPcscfAddr,
                                                                pstPdpInfo))
        {
            return IMSA_TRUE;
        }
    }

    return IMSA_FALSE;
}
VOS_VOID IMSA_CONN_DeleteEmcPdpInfo
(
    VOS_UINT8                               ucCid
)
{
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT32                          i               = IMSA_NULL;

    pstEmcConn  = IMSA_CONN_GetEmcConnAddr();

    if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
        &&(ucCid == pstEmcConn->stSipSignalPdp.ucCid))
    {
        /* 去激活虚拟网卡或者配置IP地址无效,配置DNS地址无效 */
        /* lihong00150010 volte phaseiii begin */
        /* IMSA_CONN_PdnInfoNicDelete(pstEmcConn->stSipSignalPdp.ucPdpId); */
        /* lihong00150010 volte phaseiii end */

        /* 调用REG模块删除IP地址和P-CSCF地址 */
        IMSA_CONN_DeletePdpInfo2Reg(IMSA_CONN_TYPE_EMC, &pstEmcConn->stSipSignalPdp);

        IMSA_MEM_SET(       &pstEmcConn->stSipSignalPdp,
                            0x0,
                            sizeof(IMSA_PDP_CNTXT_INFO_STRU));

        /* 通知CDS下行过滤承载 */
        IMSA_CONN_SndCdsSetImsBearerReq();

        return ;
    }

    /* 查找普通媒体承载，并删除 */
    for (i = 0; i < pstEmcConn->ulSipMediaPdpNum; i++)
    {
        if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->astSipMediaPdpArray[i].enPdpState)
            &&(ucCid == pstEmcConn->astSipMediaPdpArray[i].ucCid))
        {
            if (i < (IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM - 1))
            {
                IMSA_MEM_CPY(   &pstEmcConn->astSipMediaPdpArray[i],
                                &pstEmcConn->astSipMediaPdpArray[i+1],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1)-i));
            }

            IMSA_MEM_SET(       &pstEmcConn->astSipMediaPdpArray[IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1],
                                0x0,
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU));

            pstEmcConn->ulSipMediaPdpNum--;

            /* 通知CDS下行过滤承载 */
            IMSA_CONN_SndCdsSetImsBearerReq();

            return ;
        }
    }

    return ;
}


VOS_VOID IMSA_CONN_DeletePdpInfo
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    VOS_UINT8                               ucCid,
    VOS_UINT32                             *pulRegParaValid
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    /* IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR; */
    VOS_UINT32                          i               = IMSA_NULL;

    *pulRegParaValid = IMSA_CONN_REG_PARA_VALID;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        IMSA_INFO_LOG("IMSA_CONN_DeletePdpInfo:delete emc pdp!");

        IMSA_CONN_DeleteEmcPdpInfo(ucCid);

        return ;
    }

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    /* 查找普通信令承载，并删除 */
    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ((IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState)
            &&(ucCid == pstNormalConn->astSipSignalPdpArray[i].ucCid))
        {
            /* 去激活虚拟网卡或者配置IP地址无效,配置DNS地址无效 */
            /* lihong00150010 volte phaseiii begin */
            /* IMSA_CONN_PdnInfoNicDelete(pstNormalConn->astSipSignalPdpArray[i].ucPdpId); */
            /* lihong00150010 volte phaseiii end */

            /* 识别注册参数是否失效 */
            if (IMSA_TRUE == IMSA_CONN_IsPdpInfoIncludeRegPara( IMSA_CONN_TYPE_NORMAL,
                                                                &pstNormalConn->astSipSignalPdpArray[i]))
            {
                *pulRegParaValid = IMSA_CONN_REG_PARA_INVALID;
            }

            /* 调用REG模块删除IP地址和P-CSCF地址 */
            IMSA_CONN_DeletePdpInfo2Reg(IMSA_CONN_TYPE_NORMAL, &pstNormalConn->astSipSignalPdpArray[i]);

            if (i < (IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM - 1))
            {
                IMSA_MEM_CPY(   &pstNormalConn->astSipSignalPdpArray[i],
                                &pstNormalConn->astSipSignalPdpArray[i+1],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM-1)-i));
            }

            IMSA_MEM_SET(       &pstNormalConn->astSipSignalPdpArray[IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM-1],
                                0x0,
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU));

            pstNormalConn->ulSipSignalPdpNum--;

            /* 通知CDS下行过滤承载 */
            IMSA_CONN_SndCdsSetImsBearerReq();

            return ;
        }
    }

    /* 查找普通媒体承载，并删除 */
    for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
    {
        if ((IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState)
            &&(ucCid == pstNormalConn->astSipMediaPdpArray[i].ucCid))
        {
            if (i < (IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM - 1))
            {
                IMSA_MEM_CPY(   &pstNormalConn->astSipMediaPdpArray[i],
                                &pstNormalConn->astSipMediaPdpArray[i+1],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1)-i));
            }

            IMSA_MEM_SET(       &pstNormalConn->astSipMediaPdpArray[IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1],
                                0x0,
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU));

            pstNormalConn->ulSipMediaPdpNum--;

            /* 通知CDS下行过滤承载 */
            IMSA_CONN_SndCdsSetImsBearerReq();

            return ;
        }
    }

    return ;
}
VOS_VOID IMSA_CONN_SndConnSetupInd
(
    IMSA_CONN_RESULT_ENUM_UINT32            enResult,
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32      enSipPdpType
)
{
    IMSA_CONN_SETUP_IND_STRU           *pstConnSetupInd  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnSetupInd is entered!");

    /*分配空间并检验分配是否成功*/
    pstConnSetupInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_SETUP_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnSetupInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnSetupInd:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstConnSetupInd), 0, IMSA_GET_MSG_LENGTH(pstConnSetupInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnSetupInd,
                                ID_IMSA_CONN_SETUP_IND,
                                sizeof(IMSA_CONN_SETUP_IND_STRU));

    /*填写响应结果*/
    pstConnSetupInd->enResult       = enResult;
    pstConnSetupInd->enConnType     = enConnType;
    pstConnSetupInd->enSipPdpType   = enSipPdpType;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnSetupInd);
}
VOS_VOID IMSA_CONN_SndConnMediaPdpModifyInd
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType
)
{
    IMSA_CONN_MODIFY_IND_STRU           *pstConnModifyInd  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnMediaPdpModifyInd is entered!");

    /*分配空间并检验分配是否成功*/
    pstConnModifyInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_MODIFY_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnModifyInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnMediaPdpModifyInd:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstConnModifyInd), 0, IMSA_GET_MSG_LENGTH(pstConnModifyInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnModifyInd,
                                ID_IMSA_CONN_MODIFY_IND,
                                sizeof(IMSA_CONN_MODIFY_IND_STRU));

    /*填写响应结果*/
    pstConnModifyInd->enConnType    = enConnType;
    pstConnModifyInd->enSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_MEDIA;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnModifyInd);
}
VOS_VOID IMSA_CONN_SndConnRegParaInvalid( VOS_VOID )
{
    IMSA_CONN_MODIFY_IND_STRU           *pstConnModifyInd  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnRegParaInvalid is entered!");

    /*分配空间并检验分配是否成功*/
    pstConnModifyInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_MODIFY_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnModifyInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnMediaPdpModifyInd:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstConnModifyInd), 0, IMSA_GET_MSG_LENGTH(pstConnModifyInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnModifyInd,
                                ID_IMSA_CONN_MODIFY_IND,
                                sizeof(IMSA_CONN_MODIFY_IND_STRU));

    /*填写响应结果*/
    pstConnModifyInd->enConnType            = IMSA_CONN_TYPE_NORMAL;
    pstConnModifyInd->enSipPdpType          = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;
    pstConnModifyInd->bitOpRegParaValidFlag = IMSA_OP_TRUE;
    pstConnModifyInd->ulRegParaValidFlag    = IMSA_CONN_REG_PARA_INVALID;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnModifyInd);
}
VOS_VOID IMSA_CONN_SndConnRelInd
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32      enSipPdpType
)
{
    IMSA_CONN_REL_IND_STRU             *pstConnRelInd  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnRelInd is entered!");

    /*分配空间并检验分配是否成功*/
    pstConnRelInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_REL_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnRelInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnRelInd:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstConnRelInd), 0, IMSA_GET_MSG_LENGTH(pstConnRelInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnRelInd,
                                ID_IMSA_CONN_REL_IND,
                                sizeof(IMSA_CONN_REL_IND_STRU));

    /*填写响应结果*/
    pstConnRelInd->enConnType     = enConnType;
    pstConnRelInd->enSipPdpType   = enSipPdpType;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnRelInd);
}
VOS_VOID IMSA_CONN_ProcReEstablishConnSetupReq
(
    const IMSA_CONN_SETUP_REQ_STRU     *pstConnSetupReqMsg
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus        = IMSA_CONN_STATUS_BUTT;
    TAF_SDF_PARA_QUERY_INFO_STRU       *pstSdfQueryInfo     = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt              = IMSA_FAIL;
    /* TAF_PS_DIAL_PARA_STRU              *pstDialParaInfo     = VOS_NULL_PTR; */
    VOS_UINT8                           ucOpid              = IMSA_NULL;
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType          = pstConnSetupReqMsg->enConnType;
    VOS_UINT32                          ulFoundIndex        = IMSA_NULL;
    /* IMSA_SEL_SDF_PARA_STRU             *pstSelectedSdfPara  = VOS_NULL_PTR; */
    IMSA_CONTROL_MANAGER_STRU          *pstImsaControlManager   = VOS_NULL_PTR;
    VOS_UINT8                           ucCid               = IMSA_FAIL;
    IMSA_IP_TYPE_ENUM_UINT8             enIpType            = IMSA_FAIL;

    IMSA_INFO_LOG("IMSA_CONN_ProcReEstablishConnSetupReq is entered!");

    pstImsaControlManager   = IMSA_GetControlManagerAddress();

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enConnStatus = IMSA_CONN_GetNormalConnStatus();

        ucCid= pstImsaControlManager->stNrmRegParaInfo.ucCid;

        enIpType = pstImsaControlManager->stNrmRegParaInfo.enIpType;
    }
    else
    {
        enConnStatus = IMSA_CONN_GetEmcConnStatus();

        ucCid= pstImsaControlManager->stEmcRegParaInfo.ucCid;

        enIpType = pstImsaControlManager->stEmcRegParaInfo.enIpType;
    }

    /* 如果在CONNING态，则丢弃 */
    if (IMSA_CONN_STATUS_CONNING == enConnStatus)
    {
        return ;
    }

    /* 如果在RELEASING态，则直接回复连接建立失败，原因值为IMSA_CONN_RESULT_FAIL_CONN_RELEASING */
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_CONN_RELEASING,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return ;
    }

    pstSdfQueryInfo = IMSA_MEM_ALLOC(sizeof(TAF_SDF_PARA_QUERY_INFO_STRU));

    if (VOS_NULL_PTR == pstSdfQueryInfo)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcReEstablishConnSetupReq:ERROR: Mem alloc fail!");

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return;
    }

    /* 获取所有NV项中的SDF配置信息 */
    IMSA_CONN_AssignOpid(enConnType, &ucOpid);
    ulRslt = TAF_PS_GetCidSdfParaInfo(PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid, pstSdfQueryInfo);
    if (VOS_OK != ulRslt)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcReEstablishConnSetupReq:TAF_PS_GetCidSdfParaInfo fail!");

        IMSA_MEM_FREE(pstSdfQueryInfo);

        /* 如果获取SDF信息失败，且存在其他PDN的情况下，直接返回成功，SERVICE模块可以判断
        出IP地址发生变化，会本地去注册，然后再重新发起注册 */
        if (IMSA_TRUE == IMSA_CONN_HasActiveSipSignalPdp(enConnType))
        {
            IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_SUCCESS,
                                        enConnType,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        }
        else
        {
            IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                        enConnType,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL);
        }
        return;
    }

    /* 查找备份CID的SDF信息  */
    ulRslt = IMSA_CONN_FindSipSignalReStablishDialPara( ucCid,
                                                        pstSdfQueryInfo->ulSdfNum,
                                                        pstSdfQueryInfo->astSdfPara,
                                                        &ulFoundIndex);
    if (IMSA_FAIL == ulRslt)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcReEstablishConnSetupReq:ERROR: Err Para!");

        IMSA_MEM_FREE(pstSdfQueryInfo);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_PARA_ERR,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return;
    }



    /* 将PDN类型修改为备份的PDN类型 */
    pstSdfQueryInfo->astSdfPara[ulFoundIndex].enPdnType = enIpType;

    /* 存储选择的SDF参数 */
    IMSA_CONN_SaveSelectedSdfPara(  enConnType,
                                    &pstSdfQueryInfo->astSdfPara[ulFoundIndex]);

    IMSA_MEM_FREE(pstSdfQueryInfo);

    /* pstSelectedSdfPara = IMSA_CONN_GetNormalConnSelSdfParaAddr(); */


    ulRslt = IMSA_CONN_SipSignalPdpActOrig(enConnType,
                                           ucCid,
                                           enIpType);

    if (IMSA_FAIL== ulRslt)
    {
        /* 如果拨号失败，且存在其他PDN的情况下，直接返回成功，SERVICE模块可以判断
        出IP地址发生变化，会本地去注册，然后再重新发起注册 */
        if (IMSA_TRUE == IMSA_CONN_HasActiveSipSignalPdp(enConnType))
        {
            IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_SUCCESS,
                                        enConnType,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        }
        else
        {
            IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                        enConnType,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL);
        }
        return;
    }

    /* 连接状态转到CONNING状态 */
    IMSA_CONN_SetConnStatus(enConnType, IMSA_CONN_STATUS_CONNING);

    /* 存储最初请求的PDN类型为备份PDN类型 */
    IMSA_CONN_SaveFirstReqPdnType(enConnType, enIpType);

}


VOS_VOID IMSA_CONN_ProcConnSetupReq
(
    const IMSA_CONN_SETUP_REQ_STRU     *pstConnSetupReqMsg
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus        = IMSA_CONN_STATUS_BUTT;
    TAF_SDF_PARA_QUERY_INFO_STRU       *pstSdfQueryInfo     = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt              = IMSA_FAIL;
    TAF_PS_DIAL_PARA_STRU              *pstDialParaInfo     = VOS_NULL_PTR;
    VOS_UINT8                           ucOpid              = IMSA_NULL;
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType          = pstConnSetupReqMsg->enConnType;
    VOS_UINT32                          ulFoundIndex        = IMSA_NULL;
    IMSA_SEL_SDF_PARA_STRU             *pstSelectedSdfPara  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_ProcConnSetupReq is entered!");

    if (IMSA_TRUE == pstConnSetupReqMsg->ulReEstablishFlag)
    {
        IMSA_CONN_ProcReEstablishConnSetupReq(pstConnSetupReqMsg);
        return;
    }

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enConnStatus = IMSA_CONN_GetNormalConnStatus();
    }
    else
    {
        enConnStatus = IMSA_CONN_GetEmcConnStatus();
    }

    /* 如果在CONN态，则直接返回连接建立成功 */
    if (IMSA_CONN_STATUS_CONN == enConnStatus)
    {
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_SUCCESS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return ;
    }

    /* 如果在CONNING态，则丢弃 */
    if (IMSA_CONN_STATUS_CONNING == enConnStatus)
    {
        return ;
    }

    /* 如果在RELEASING态，则直接回复连接建立失败，原因值为IMSA_CONN_RESULT_FAIL_CONN_RELEASING */
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_CONN_RELEASING,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return ;
    }

    /* 如果在IDLE态，则发起连接建立流程 */

    pstSdfQueryInfo = IMSA_MEM_ALLOC(sizeof(TAF_SDF_PARA_QUERY_INFO_STRU));

    if (VOS_NULL_PTR == pstSdfQueryInfo)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:ERROR: Mem alloc fail!");

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return;
    }

    /* 获取所有NV项中的SDF配置信息 */
    IMSA_CONN_AssignOpid(enConnType, &ucOpid);
    ulRslt = TAF_PS_GetCidSdfParaInfo(PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid, pstSdfQueryInfo);
    if (VOS_OK != ulRslt)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:TAF_PS_GetCidSdfParaInfo fail!");

        IMSA_MEM_FREE(pstSdfQueryInfo);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return;
    }

    ulRslt = IMSA_CONN_FindSipSignalDialPara(   enConnType,
                                                pstSdfQueryInfo->ulSdfNum,
                                                pstSdfQueryInfo->astSdfPara,
                                                &ulFoundIndex);

    if (IMSA_FAIL == ulRslt)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:ERROR: Err Para!");

        IMSA_MEM_FREE(pstSdfQueryInfo);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_PARA_ERR,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return;
    }

    /* 存储选择的SDF参数 */
    IMSA_CONN_SaveSelectedSdfPara(  enConnType,
                                    &pstSdfQueryInfo->astSdfPara[ulFoundIndex]);

    IMSA_MEM_FREE(pstSdfQueryInfo);

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        pstSelectedSdfPara = IMSA_CONN_GetNormalConnSelSdfParaAddr();
    }
    else
    {
        pstSelectedSdfPara = IMSA_CONN_GetEmcConnSelSdfParaAddr();
    }

    pstDialParaInfo = IMSA_MEM_ALLOC(sizeof(TAF_PS_DIAL_PARA_STRU));

    if (VOS_NULL_PTR == pstDialParaInfo)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:TAF_PS_DIAL_PARA_STRU ERROR: Mem alloc fail!");

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return;
    }

    /* 将选择的SDF参数转化成拨号参数格式 */
    IMSA_CONN_GetImsDailParaFromSelSdfPara( pstSelectedSdfPara,
                                            pstDialParaInfo);


    /* 产生OPID并存储 */
    IMSA_CONN_AssignOpid(enConnType, &ucOpid);
    IMSA_CONN_SetOpid(enConnType, ucOpid);

    /* 调用API接口TAF_PS_CallOrig请求APS建立IMS信令承载 */
    if (VOS_OK == TAF_PS_CallOrig(PS_PID_IMSA,IMSA_CLIENT_ID, ucOpid, pstDialParaInfo))
    {
        /* 启动IMS拨号定时器 */
        IMSA_CONN_StartTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_ORIG);

        /* 连接状态转到CONNING状态 */
        IMSA_CONN_SetConnStatus(enConnType, IMSA_CONN_STATUS_CONNING);

        /* 存储最初拨号请求的PDN类型 */
        IMSA_CONN_SaveFirstReqPdnType(enConnType, pstDialParaInfo->enPdpType);

        /* 存储当前拨号请求的PDN类型 */
        IMSA_CONN_SaveCurReqPdnType(enConnType, pstDialParaInfo->enPdpType);

        IMSA_INFO_LOG1("global enFirstReqPdnType:", IMSA_CONN_GetNormalConnFirstReqPdnType());
    }
    else
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:TAF_PS_CallOrig fail!");

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL);
    }

    IMSA_MEM_FREE(pstDialParaInfo);
}


VOS_VOID IMSA_CONN_RequestApsRelConn
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT8                           ucOpid          = IMSA_NULL;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn      = IMSA_CONN_GetEmcConnAddr();

        /* 如果在CONNING态，则请求释放正在建立的信令承载和已经激活的信令承载 */
        if (IMSA_CONN_STATUS_CONNING == pstEmcConn->enImsaConnStatus)
        {
            /* 产生OPID并存储 */
            IMSA_CONN_AssignOpid(enConnType, &ucOpid);

            if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                            pstEmcConn->stSelSdfPara.ucCid))
            {
                IMSA_WARN_LOG("IMSA_CONN_RequestApsRelConn:EMC,conninig,CallEnd failed!");
            }

            IMSA_CONN_ClearConnResource(IMSA_CONN_TYPE_EMC);

            /* 通知SERVICE模块连接释放 */
            IMSA_CONN_SndConnRelInd(IMSA_CONN_TYPE_EMC, IMSA_CONN_SIP_PDP_TYPE_SIGNAL);
            return ;
        }

        /* 产生OPID并存储 */
        IMSA_CONN_AssignOpid(enConnType, &ucOpid);
        IMSA_CONN_SetOpid(enConnType, ucOpid);



        if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                        (VOS_UINT8)pstEmcConn->stSipSignalPdp.ucCid))
        {
            IMSA_WARN_LOG("IMSA_CONN_RequestApsRelConn:EMC,conn,CallEnd failed!");

            IMSA_CONN_ClearConnResource(IMSA_CONN_TYPE_EMC);

            /* 通知SERVICE模块连接释放 */
            IMSA_CONN_SndConnRelInd(IMSA_CONN_TYPE_EMC, IMSA_CONN_SIP_PDP_TYPE_SIGNAL);
            return ;
        }

        /* 启动IMSA去拨号定时器 */
        IMSA_CONN_StartTimer(IMSA_CONN_TYPE_EMC, TI_IMSA_SIP_SIGAL_PDP_END);

        /* 设置连接状态为去连接中 */
        IMSA_CONN_SetConnStatus(IMSA_CONN_TYPE_EMC, IMSA_CONN_STATUS_RELEASING);
        return ;
    }

    pstNormalConn   = IMSA_CONN_GetNormalConnAddr();

    /* 如果在CONNING态，则请求释放正在建立的信令承载和已经激活的信令承载 */
    if (IMSA_CONN_STATUS_CONNING == pstNormalConn->enImsaConnStatus)
    {
        /* 产生OPID并存储 */
        IMSA_CONN_AssignOpid(enConnType, &ucOpid);

        if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                        pstNormalConn->stSelSdfPara.ucCid))
        {
            IMSA_WARN_LOG("IMSA_CONN_RequestApsRelConn:normal,conninig,CallEnd failed!");
        }
    }

    if (0 == pstNormalConn->ulSipSignalPdpNum)
    {
        IMSA_WARN_LOG("IMSA_CONN_RequestApsRelConn:normal,conning,no active pdp!");

        IMSA_CONN_ClearConnResource(IMSA_CONN_TYPE_NORMAL);

        /* 通知SERVICE模块连接释放 */
        IMSA_CONN_SndConnRelInd(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL);
        return ;
    }

    /* 产生OPID并存储 */
    IMSA_CONN_AssignOpid(enConnType, &ucOpid);
    IMSA_CONN_SetNormalConnOpid(ucOpid);

    if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                    pstNormalConn->astSipSignalPdpArray[0].ucCid))
    {
        IMSA_WARN_LOG("IMSA_CONN_RequestApsRelConn:normal,CallEnd failed!");

        IMSA_CONN_ClearConnResource(IMSA_CONN_TYPE_NORMAL);

        /* 通知SERVICE模块连接释放 */
        IMSA_CONN_SndConnRelInd(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return ;
    }

    /* 启动IMSA去拨号定时器 */
    IMSA_CONN_StartTimer(IMSA_CONN_TYPE_NORMAL, TI_IMSA_SIP_SIGAL_PDP_END);

    /* 设置连接状态为去连接中 */
    IMSA_CONN_SetConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_RELEASING);

    return ;
}



VOS_VOID IMSA_CONN_ProcConnRelReq
(
    const IMSA_CONN_REL_REQ_STRU       *pstConnRelReqMsg
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus        = IMSA_CONN_STATUS_BUTT;
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType          = pstConnRelReqMsg->enConnType;

    IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq is entered!");

    if (IMSA_CONN_SIP_PDP_TYPE_MEDIA == pstConnRelReqMsg->enSipPdpType)
    {
        IMSA_ERR_LOG("IMSA_CONN_ProcConnRelReq:Not support release media type pdp!");
        return ;
    }

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enConnStatus = IMSA_CONN_GetNormalConnStatus();
    }
    else
    {
        enConnStatus = IMSA_CONN_GetEmcConnStatus();
    }

    /* 如果在IDLE态，则直接返回连接释放成功 */
    if (IMSA_CONN_STATUS_IDLE == enConnStatus)
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq: idle!");

        IMSA_CONN_SndConnRelInd(  enConnType,
                                  IMSA_CONN_SIP_PDP_TYPE_SIGNAL);

        return ;
    }

    /* 如果在RELEASING态，则丢弃 */
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq: releasing!");
        return ;
    }

    /* 停止IMS拨号定时器 */
    IMSA_CONN_StopTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_ORIG);

    /* 停止等待IPV6参数定时器 */
    IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_IPV6_INFO);

    /* 启动IMS去拨号定时器 */
    IMSA_CONN_StartTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_END);

    /* 请求APS释放连接 */
    IMSA_CONN_RequestApsRelConn(enConnType);

    return ;
}


VOS_UINT32 IMSA_CONN_IsEmcPdpDefined( VOS_VOID )
{
    VOS_UINT32                          ulRslt              = IMSA_FAIL;
    TAF_SDF_PARA_QUERY_INFO_STRU       *pstSdfQueryInfo     = VOS_NULL_PTR;
    VOS_UINT8                           ucOpid              = IMSA_NULL;
    VOS_UINT32                          ulFoundIndex        = IMSA_NULL;

    pstSdfQueryInfo = IMSA_MEM_ALLOC(sizeof(TAF_SDF_PARA_QUERY_INFO_STRU));

    if (VOS_NULL_PTR == pstSdfQueryInfo)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_IsEmcPdpDefined:ERROR: Mem alloc fail!");

        return IMSA_FALSE;
    }

    /* 获取所有NV项中的SDF配置信息 */
    IMSA_CONN_AssignOpid(IMSA_CONN_TYPE_EMC, &ucOpid);
    ulRslt = TAF_PS_GetCidSdfParaInfo(PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid, pstSdfQueryInfo);
    if (VOS_OK != ulRslt)
    {
        IMSA_INFO_LOG("IMSA_CONN_IsEmcPdpDefined:TAF_PS_GetCidSdfParaInfo failed!");

        IMSA_MEM_FREE(pstSdfQueryInfo);

        return IMSA_FALSE;
    }

    ulRslt = IMSA_CONN_FindSipSignalDialPara(   IMSA_CONN_TYPE_EMC,
                                                pstSdfQueryInfo->ulSdfNum,
                                                pstSdfQueryInfo->astSdfPara,
                                                &ulFoundIndex);

    IMSA_MEM_FREE(pstSdfQueryInfo);

    if (IMSA_FAIL == ulRslt)
    {
        IMSA_INFO_LOG("IMSA_CONN_IsEmcPdpDefined:not define emc pdp!");

        return IMSA_FALSE;
    }

    return IMSA_TRUE;
}


VOS_UINT32 IMSA_CONN_IsEqualToSelectedCid
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT8                           ucCid
)
{
    IMSA_SEL_SDF_PARA_STRU             *pstSelSdfPara = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        pstSelSdfPara = IMSA_CONN_GetNormalConnSelSdfParaAddr();
    }
    else
    {
        pstSelSdfPara = IMSA_CONN_GetEmcConnSelSdfParaAddr();
    }

    if (ucCid == pstSelSdfPara->ucCid)
    {
        return IMSA_TRUE;
    }
    else
    {
        return IMSA_FALSE;
    }
}

/*****************************************************************************
 Function Name   : IMSA_CONN_PrintNrmConnState
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_INT32

 History         :
    1.lihong00150010    2013-08-29  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_PrintNrmConnState( VOS_VOID )
{
    switch (IMSA_CONN_GetNormalConnStatus())
    {
        case IMSA_CONN_STATUS_IDLE:
            IMSA_INFO_LOG("Current Normal Conn State: IDLE");
            break;
        case IMSA_CONN_STATUS_CONNING:
            IMSA_INFO_LOG("Current Normal Conn State: CONNING");
            break;
        case IMSA_CONN_STATUS_RELEASING:
            IMSA_INFO_LOG("Current Normal Conn State: RELEASING");
            break;
        case IMSA_CONN_STATUS_CONN:
            IMSA_INFO_LOG("Current Normal Conn State: CONN");
            break;
        default:
            IMSA_INFO_LOG("Current Normal Conn State: UNKNOWN");
            break;
    }
}

/*****************************************************************************
 Function Name   : IMSA_CONN_PrintEmcConnState
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_INT32

 History         :
    1.lihong00150010    2013-08-29  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_PrintEmcConnState( VOS_VOID )
{
    switch (IMSA_CONN_GetEmcConnStatus())
    {
        case IMSA_CONN_STATUS_IDLE:
            IMSA_INFO_LOG("Current Emc Conn State: IDLE");
            break;
        case IMSA_CONN_STATUS_CONNING:
            IMSA_INFO_LOG("Current Emc Conn State: CONNING");
            break;
        case IMSA_CONN_STATUS_RELEASING:
            IMSA_INFO_LOG("Current Emc Conn State: RELEASING");
            break;
        case IMSA_CONN_STATUS_CONN:
            IMSA_INFO_LOG("Current Emc Conn State: CONN");
            break;
        default:
            IMSA_INFO_LOG("Current Emc Conn State: UNKNOWN");
            break;
    }
}

/*****************************************************************************
 Function Name   : IMSA_CONN_PrintNicPdpInfo
 Description     : 打印NIC PDP INFO存储结构中的信息
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_PrintNicPdpInfo(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    IMSA_PRINT_NIC_PDP_INFO_STRU     *pstMsgNicInfo;

    pstMsgNicInfo = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_PRINT_NIC_PDP_INFO_STRU));
    if (VOS_NULL_PTR == pstMsgNicInfo)
    {
        IMSA_ERR_LOG("IMSA_CONN_PrintNicPdpInfo: mem alloc fail!.");
        return;
    }

    IMSA_MEM_SET(IMSA_GET_MSG_ENTITY(pstMsgNicInfo), 0, \
                 IMSA_GET_MSG_LENGTH(pstMsgNicInfo));

    IMSA_WRITE_INTRA_MSG_HEAD(  pstMsgNicInfo,
                                ID_IMSA_NIC_PDP_INFO_IND,
                                sizeof(IMSA_PRINT_NIC_PDP_INFO_STRU));

    IMSA_MEM_CPY(pstMsgNicInfo->astNicPdpInfoArray, \
                 pstConnManager->astNicPdpInfoArray, \
                 sizeof(IMSA_CONN_NIC_PDP_INFO_STRU)*IMSA_CONN_MAX_NIC_PDP_NUM);

    IMSA_INFO_LOG("IMSA_CONN_PrintNicPdpInfo: show nic pdp info.");

    (VOS_VOID)LTE_MsgHook(pstMsgNicInfo);

    IMSA_FREE_MSG(pstMsgNicInfo);
}


/*****************************************************************************
 Function Name   : IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo
 Description     : 判断存在于NIC PDP INFO中的IPV4，是否存在于最新承载信息中的IP地址

 Input           : pstNicPdpInfo-----------配置给虚拟网卡的承载信息
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_UINT32 IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo(const IMSA_CONN_NIC_PDP_INFO_STRU *pstNicPdpInfo)
{
    IMSA_EMC_CONN_STRU       *pstEmcConn      = VOS_NULL_PTR;
    IMSA_NORMAL_CONN_STRU    *pstNormalConn   = VOS_NULL_PTR;
    VOS_UINT32               i                = 0;

    IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo is entered!");

    pstEmcConn = IMSA_CONN_GetEmcConnAddr();

    if (0 == IMSA_MEM_CMP(pstNicPdpInfo->stPdpAddr.aucIpV4Addr, pstEmcConn->stSipSignalPdp.stPdpAddr.aucIpV4Addr, IMSA_IPV4_ADDR_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo: IPV4 exist in EmcPdpInfo!");

        return IMSA_TRUE;
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo: IPV4 not exist in EmcPdpInfo!");

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ( 0 == IMSA_MEM_CMP(pstNicPdpInfo->stPdpAddr.aucIpV4Addr, pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.aucIpV4Addr, IMSA_IPV4_ADDR_LEN))
        {
            IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo: IPV4 exist in NormalPdpInfo!");

            return IMSA_TRUE;
        }
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo: IPV4 not exist in NormalPdpInfo!");

    return IMSA_FALSE;
}

/*****************************************************************************
 Function Name   : IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo
 Description     : 判断存在于NIC PDP INFO中的IPV6，是否存在于最新承载信息中的IP地址

 Input           : pstNicPdpInfo-----------配置给虚拟网卡的承载信息
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_UINT32 IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo(const IMSA_CONN_NIC_PDP_INFO_STRU *pstNicPdpInfo)
{
    IMSA_EMC_CONN_STRU       *pstEmcConn      = VOS_NULL_PTR;
    IMSA_NORMAL_CONN_STRU    *pstNormalConn   = VOS_NULL_PTR;
    VOS_UINT32               i                = 0;

    IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo is entered!");

    pstEmcConn = IMSA_CONN_GetEmcConnAddr();

    if (0 == IMSA_MEM_CMP(pstNicPdpInfo->stPdpAddr.aucIpV6Addr, pstEmcConn->stSipSignalPdp.stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo: IPV6 exist in EmcPdpInfo!");

        return IMSA_TRUE;
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo: IPV6 not exist in EmcPdpInfo!");

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ( 0 == IMSA_MEM_CMP(pstNicPdpInfo->stPdpAddr.aucIpV6Addr, pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_LEN))
        {
            IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo: IPV6 exist in NormalPdpInfo!");

            return IMSA_TRUE;
        }
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo: IPV6 not exist in NormalPdpInfo!");

    return IMSA_FALSE;
}


/*****************************************************************************
 Function Name   : IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo
 Description     : 用于识别存在于NIC PDP INFO中，但不存在于最新承载信息中的IP地址，
                   将其从NIC PDP INFO中删除，并通知TTF删除
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               ulRst           = IMSA_TRUE;
    VOS_UINT32               i               = 0;

    IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo is entered!");

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        switch(pstConnManager->astNicPdpInfoArray[i].stPdpAddr.enIpType)
        {
        case IMSA_IP_TYPE_IPV4:
            IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IMSA_IP_TYPE_IPV4!");

            if (IMSA_FALSE == IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo(&pstConnManager->astNicPdpInfoArray[i]))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IPV4 not exist in PdpInfo!");

                ulRst = IMSA_FALSE;
            }

            break;
        case IMSA_IP_TYPE_IPV6:
            IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IMSA_IP_TYPE_IPV6!");

            if (IMSA_FALSE == IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo(&pstConnManager->astNicPdpInfoArray[i]))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IPV6 not exist in PdpInfo!");

                ulRst = IMSA_FALSE;
            }

            break;
        case IMSA_IP_TYPE_IPV4V6:
            IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IMSA_IP_TYPE_IPV4V6!");

            if (IMSA_FALSE == IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo(&pstConnManager->astNicPdpInfoArray[i]) ||
                IMSA_FALSE == IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo(&pstConnManager->astNicPdpInfoArray[i]))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IPV4V6 not exist in PdpInfo!");

                ulRst = IMSA_FALSE;
            }

            break;
        default:
            IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IP tpye error!");
            break;
        }

        if (IMSA_FALSE == ulRst)
        {
            /* 防止紧急注册时，普通连接正在重建的场景或者普通注册时，
               紧急连接正在重建的场景，因此在CONNING+REG状态不能删*/
            if (IMSA_FALSE == IMSA_SRV_IsConningRegState(pstConnManager->astNicPdpInfoArray[i].ucIsEmc))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: NIC delete pdp info!");
                IMSA_CONN_PdnInfoNicDelete(pstConnManager->astNicPdpInfoArray[i].ucPdpId);

                IMSA_MEM_SET(&pstConnManager->astNicPdpInfoArray[i], 0, sizeof(IMSA_CONN_NIC_PDP_INFO_STRU ));
                pstConnManager->astNicPdpInfoArray[i].ucIsUsed = 0;
            }

            ulRst = IMSA_TRUE;
        }
    }
}

/*****************************************************************************
 Function Name   : IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo
 Description     : 判断承载信息中的IPV4，是否存在于NIC PDP INFO存储结构中

 Input           : pstPdpAddr---------------承载信息中的IP信息
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_UINT32 IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo(const IMSA_IP_ADDRESS_STRU *pstPdpAddr)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               i               = 0;

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        if ( 0 == IMSA_MEM_CMP(pstPdpAddr->aucIpV4Addr, pstConnManager->astNicPdpInfoArray[i].stPdpAddr.aucIpV4Addr, IMSA_IPV4_ADDR_LEN))
        {
            IMSA_INFO_LOG("IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo: IPV4 exist in NicPdpInfo!");
            return IMSA_TRUE;
        }
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo: IPV4 not exist in NicPdpInfo!");

    return IMSA_FALSE;
}

/*****************************************************************************
 Function Name   : IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo
 Description     : 判断承载信息中的IPV6，是否存在于NIC PDP INFO存储结构中

 Input           : pstPdpAddr---------------承载信息中的IP信息
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_UINT32 IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo(const IMSA_IP_ADDRESS_STRU *pstPdpAddr)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager    = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               i                 = 0;

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        if ( 0 == IMSA_MEM_CMP(pstPdpAddr->aucIpV6Addr, pstConnManager->astNicPdpInfoArray[i].stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_LEN))
        {
            IMSA_INFO_LOG("IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo: IPV6 exist in NicPdpInfo!");
            return IMSA_TRUE;
        }
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo: IPV6 not exist in NicPdpInfo!");

    return IMSA_FALSE;
}

/*****************************************************************************
 Function Name   : IMSA_CONN_PdpIdIsExistInNicPdpInfo
 Description     : 判断承载信息中的PDP ID，是否存在于NIC PDP INFO存储结构中

 Input           : pstPdpAddr-----------承载信息中的IP信息
 Output          : pulArrayIndex--------PDP ID存在于astNicPdpInfoArray结构中的索引
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_UINT32 IMSA_CONN_PdpIdIsExistInNicPdpInfo(VOS_UINT8 ucPdpId, VOS_UINT32 *pulArrayIndex)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager    = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               i                 = 0;

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        if (ucPdpId == pstConnManager->astNicPdpInfoArray[i].ucPdpId)
        {
            IMSA_INFO_LOG("IMSA_CONN_PdpIdIsExistInNicPdpInfo: PdpId exist in NicPdpInfo!");
            *pulArrayIndex = i;
            return IMSA_TRUE;
        }
    }

    IMSA_INFO_LOG("IMSA_CONN_PdpIdIsExistInNicPdpInfo: PdpId not exist in NicPdpInfo!");

    return IMSA_FALSE;
}


/*****************************************************************************
 Function Name   : IMSA_CONN_InquireNotUsedInNicPdpInfo
 Description     : 查询ucIsUsed为0的NIC PDP记录
 Input           : None
 Output          : pulArrayIndex------------ucIsUsed为0的astNicPdpInfoArray的索引
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_UINT32 IMSA_CONN_InquireNotUsedInNicPdpInfo(VOS_UINT32 *pulArrayIndex)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               i               = 0;

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            IMSA_INFO_LOG("IMSA_CONN_InquireNotUsedInNicPdpInfo: inquire succ!");
            *pulArrayIndex = i;
            return  IMSA_TRUE;
        }
    }
    IMSA_INFO_LOG("IMSA_CONN_InquireNotUsedInNicPdpInfo: inquire fail!");

    return IMSA_FALSE;
}

/*****************************************************************************
 Function Name   : IMSA_CONN_SaveNicPdpInfo
 Description     : 承载信息保存到NIC PDP INFO存储结构中
 Input           : ucPdpId--------------------被保存的PDP ID
                   ucIsEmc--------------------是否为紧急
                   pulArrayIndex--------------保存到NIC PDP INFO结构中的索引
                   pstPdpAddr-----------------被保存的PDP INFO
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_SaveNicPdpInfo(VOS_UINT8 ucPdpId,        VOS_UINT8 ucIsEmc, \
                                            VOS_UINT32 ulArrayIndex, const IMSA_IP_ADDRESS_STRU *pstPdpAddr)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();

    IMSA_INFO_LOG("IMSA_CONN_SaveNicPdpInfo is entered!");

    IMSA_MEM_CPY(&pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr, pstPdpAddr, sizeof(IMSA_IP_ADDRESS_STRU));
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucIsUsed = 1;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucIsEmc = ucIsEmc;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucPdpId = ucPdpId;
}

/*****************************************************************************
 Function Name   : IMSA_CONN_AddNormalPdpInfo2NicPdpInfo
 Description     : 判断IPV4信令承载信息是否存在于NIC PDP INFO中，
                   如果不存在，添加到NIC PDP INFO中，并配置给TTF
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_ProcIPV4PdpInfoForNic(VOS_UINT8 ucIsEmc, IMSA_PDP_CNTXT_INFO_STRU *pstPdpCntxtInfo)
{
    IMSA_IP_ADDRESS_STRU     *pstImsaIpAddr   = VOS_NULL_PTR;
    VOS_UINT32               ulArrayIndex     = 0;
    VOS_UINT8                ucPdpId          = 0;

    IMSA_INFO_LOG("IMSA_CONN_ProcIPV4PdpInfoForNic is entered!");

    pstImsaIpAddr = &pstPdpCntxtInfo->stPdpAddr;
    ucPdpId       = pstPdpCntxtInfo->ucPdpId;

    if (IMSA_FALSE == IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo(pstImsaIpAddr))
    {
        if (IMSA_TRUE == IMSA_CONN_InquireNotUsedInNicPdpInfo(&ulArrayIndex))
        {
            IMSA_CONN_SaveNicPdpInfo(ucPdpId, ucIsEmc, ulArrayIndex, pstImsaIpAddr);

            IMSA_CONN_ConfigPdpInfo2Bsp(pstPdpCntxtInfo);
        }
        else
        {
            IMSA_INFO_LOG("IMSA_CONN_ProcIPV4PdpInfoForNic: Inquire notUsed for ipv4 fail!");
        }
    }
}

/*****************************************************************************
 Function Name   : IMSA_CONN_ProcIPV6PdpInfoForNic
 Description     : 判断IPV6信令承载信息是否存在于NIC PDP INFO中，
                   如果不存在，添加到NIC PDP INFO中，并配置给TTF
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_ProcIPV6PdpInfoForNic(VOS_UINT8 ucIsEmc, IMSA_PDP_CNTXT_INFO_STRU *pstPdpCntxtInfo)
{
    IMSA_IP_ADDRESS_STRU     *pstImsaIpAddr   = VOS_NULL_PTR;
    VOS_UINT32               ulArrayIndex     = 0;
    VOS_UINT8                ucPdpId          = 0;
    VOS_UINT8                aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};

    IMSA_INFO_LOG("IMSA_CONN_ProcIPV6PdpInfoForNic is entered!");

    pstImsaIpAddr = &pstPdpCntxtInfo->stPdpAddr;
    ucPdpId       = pstPdpCntxtInfo->ucPdpId;

    if (0 == IMSA_MEM_CMP(aucTemp, pstImsaIpAddr->aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcIPV6PdpInfoForNic: IPV6 prefix is 0!");
    }
    else
    {
        if (IMSA_FALSE == IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo(pstImsaIpAddr))
        {
            if (IMSA_TRUE == IMSA_CONN_InquireNotUsedInNicPdpInfo(&ulArrayIndex))
            {
                IMSA_CONN_SaveNicPdpInfo(ucPdpId, ucIsEmc, ulArrayIndex, pstImsaIpAddr);

                IMSA_CONN_ConfigPdpInfo2Bsp(pstPdpCntxtInfo);
            }
            else
            {
                IMSA_INFO_LOG("IMSA_CONN_ProcIPV6PdpInfoForNic: Inquire notUsed for ipv6 fail!");
            }
        }
    }
}

/*****************************************************************************
 Function Name   : IMSA_CONN_ProcIPV4V6PdpInfoForNic
 Description     : 判断IPV4V6信令承载信息是否存在于NIC PDP INFO中，
                   如果不存在，添加到NIC PDP INFO中，并配置给TTF
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_ProcIPV4V6PdpInfoForNic(VOS_UINT8 ucIsEmc, IMSA_PDP_CNTXT_INFO_STRU *pstPdpCntxtInfo)
{
    IMSA_CONN_MANAGER_STRU   *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    IMSA_IP_ADDRESS_STRU     *pstImsaIpAddr   = VOS_NULL_PTR;
    VOS_UINT32               ulArrayIndex     = 0;
    VOS_UINT8                ucPdpId          = 0;
    VOS_UINT8                aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};

    IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic is entered!");

    pstImsaIpAddr = &pstPdpCntxtInfo->stPdpAddr;
    ucPdpId       = pstPdpCntxtInfo->ucPdpId;

    if (0 == IMSA_MEM_CMP(aucTemp, pstImsaIpAddr->aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic: IPV6 prefix is 0, !");
        return;
    }

    if ((IMSA_TRUE == IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo(pstImsaIpAddr)) &&
        (IMSA_TRUE == IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo(pstImsaIpAddr)))
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic: IPV4V6 exist in PdpInfo!");
        return;
    }

    if (IMSA_TRUE == IMSA_CONN_PdpIdIsExistInNicPdpInfo(ucPdpId, &ulArrayIndex))
    {
        IMSA_CONN_PdnInfoNicDelete(ucPdpId);

        IMSA_CONN_SaveNicPdpInfo(ucPdpId, ucIsEmc, ulArrayIndex, pstImsaIpAddr);

        if (0 == IMSA_MEM_CMP(aucTemp, pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.enIpType = IMSA_IP_TYPE_IPV4;
        }

        IMSA_CONN_ConfigPdpInfo2Bsp(pstPdpCntxtInfo);
    }
    else
    {
        if (IMSA_TRUE == IMSA_CONN_InquireNotUsedInNicPdpInfo(&ulArrayIndex))
        {
            IMSA_CONN_SaveNicPdpInfo(ucPdpId, ucIsEmc, ulArrayIndex, pstImsaIpAddr);

            if (0 == IMSA_MEM_CMP(aucTemp, pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
            {
                pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.enIpType = IMSA_IP_TYPE_IPV4;
            }

            IMSA_CONN_ConfigPdpInfo2Bsp(pstPdpCntxtInfo);
        }
        else
        {
            IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic: Inquire notUsed for ipv6 fail!");
        }
    }

    return;
}


/*****************************************************************************
 Function Name   : IMSA_CONN_AddNormalPdpInfo2NicPdpInfo
 Description     : 存在于最新普通信令承载信息中，但不存在于NIC PDP INFO中的IP地址，
                   添加到NIC PDP INFO中，并配置给TTF
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_AddNormalPdpInfo2NicPdpInfo(VOS_VOID)
{
    IMSA_NORMAL_CONN_STRU    *pstNormalConn   = VOS_NULL_PTR;
    VOS_UINT32               i                = 0;

    IMSA_INFO_LOG("IMSA_CONN_AddNormalPdpInfo2NicPdpInfo is entered!");

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i ++ )
    {
        switch(pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.enIpType)
        {
        case IMSA_IP_TYPE_IPV4:
            IMSA_INFO_LOG("IMSA_CONN_AddNormalPdpInfo2NicPdpInfo: IMSA_IP_TYPE_IPV4!");

            IMSA_CONN_ProcIPV4PdpInfoForNic(0, &pstNormalConn->astSipSignalPdpArray[i]);
            break;
        case IMSA_IP_TYPE_IPV6:
            IMSA_INFO_LOG("IMSA_CONN_AddNormalPdpInfo2NicPdpInfo: IMSA_IP_TYPE_IPV6!");

            IMSA_CONN_ProcIPV6PdpInfoForNic(0, &pstNormalConn->astSipSignalPdpArray[i]);
            break;
        case IMSA_IP_TYPE_IPV4V6:
            IMSA_INFO_LOG("IMSA_CONN_AddNormalPdpInfo2NicPdpInfo: IMSA_IP_TYPE_IPV4V6!");

            IMSA_CONN_ProcIPV4V6PdpInfoForNic(0, &pstNormalConn->astSipSignalPdpArray[i]);
            break;
        default:
            IMSA_INFO_LOG("IMSA_CONN_AddNormalPdpInfo2NicPdpInfo: enIpType error!");
            break;
        }
    }
}

/*****************************************************************************
 Function Name   : IMSA_CONN_AddEmcPdpInfo2NicPdpInfo
 Description     : 存在于最新紧急信令承载信息中，但不存在于NIC PDP INFO中的IP地址，
                   添加到NIC PDP INFO中，并配置给TTF
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_AddEmcPdpInfo2NicPdpInfo(VOS_VOID)
{
    IMSA_EMC_CONN_STRU       *pstEmcConn      = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_AddEmcPdpInfo2NicPdpInfo is entered!");

    pstEmcConn = IMSA_CONN_GetEmcConnAddr();

    switch(pstEmcConn->stSipSignalPdp.stPdpAddr.enIpType)
    {
    case IMSA_IP_TYPE_IPV4:
        IMSA_INFO_LOG("IMSA_CONN_AddEmcPdpInfo2NicPdpInfo: IMSA_IP_TYPE_IPV4!");

        IMSA_CONN_ProcIPV4PdpInfoForNic(1, &pstEmcConn->stSipSignalPdp);
        break;
    case IMSA_IP_TYPE_IPV6:
        IMSA_INFO_LOG("IMSA_CONN_AddEmcPdpInfo2NicPdpInfo: IMSA_IP_TYPE_IPV6!");

        IMSA_CONN_ProcIPV6PdpInfoForNic(1, &pstEmcConn->stSipSignalPdp);
        break;
    case IMSA_IP_TYPE_IPV4V6:
        IMSA_INFO_LOG("IMSA_CONN_AddEmcPdpInfo2NicPdpInfo: IMSA_IP_TYPE_IPV4V6!");

        IMSA_CONN_ProcIPV4V6PdpInfoForNic(1, &pstEmcConn->stSipSignalPdp);
        break;
    default:
        IMSA_INFO_LOG("IMSA_CONN_AddEmcPdpInfo2NicPdpInfo: enIpType error!");
        break;
    }
}

/*****************************************************************************
 Function Name   : IMSA_CONN_AddPdpInfo2NicPdpInfo
 Description     : 用于识别存在于最新承载信息中，但不存在于NIC PDP INFO中的IP地址，
                   添加到NIC PDP INFO中，并配置给TTF
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_AddPdpInfo2NicPdpInfo(VOS_VOID)
{
    IMSA_INFO_LOG("IMSA_CONN_AddPdpInfo2NicPdpInfo is entered!");

    /* 识别存在于最新普通信令承载信息中，但不存在于NIC PDP INFO中的IP地址，
       添加到NIC PDP INFO中，并配置给TTF */
    IMSA_CONN_AddNormalPdpInfo2NicPdpInfo();

    /* 识别存在于最新紧急信令承载信息中，但不存在于NIC PDP INFO中的IP地址，
       添加到NIC PDP INFO中，并配置给TTF */
    IMSA_CONN_AddEmcPdpInfo2NicPdpInfo();
}


/*****************************************************************************
 Function Name   : IMSA_CONN_UpdateNicPdpInfo
 Description     : 更新存储配置给NIC的PDP信息
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.xiongxianghui00253310    2014-04-16  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_CONN_UpdateNicPdpInfo( VOS_VOID )
{

    IMSA_INFO_LOG("IMSA_CONN_UpdateNicPdpInfo is entered!");
    /* 将更新前的NIC PDP信息上传到HIDS上显示 */
    IMSA_CONN_PrintNicPdpInfo();

    /* 识别存在于NIC PDP INFO中，但不存在于最新承载信息中的IP地址，
       将其从NIC PDP INFO中删除，并通知TTF删除 */
    IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo();

    /* 识别存在于最新承载信息中，但不存在于NIC PDP INFO中的IP地址，
       添加到NIC PDP INFO中，并配置给TTF */
    IMSA_CONN_AddPdpInfo2NicPdpInfo ();

    /* 将更新后的NIC PDP信息上传到HIDS上显示 */
    IMSA_CONN_PrintNicPdpInfo();
}

/*lint +e961*/
/*lint +e960*/

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaConnManagement.c */



