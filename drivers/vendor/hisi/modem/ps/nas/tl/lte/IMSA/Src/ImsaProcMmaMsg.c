/******************************************************************************

   Copyright(C)2013,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : ImsaProcMmaMsg.c
  Description     : 该C文件实现MMA消息处理和MMA消息发送
  History           :
     1.sunbing 49683      2013-06-19  Draft Enact

******************************************************************************/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "PsTypeDef.h"
#include "ImsaProcMmaMsg.h"
#include "ImsaEntity.h"
#include "ImsaPublic.h"
#include "ImsaServiceManagement.h"
#include "ImsaImsAdaption.h"
#include "ImsaMain.h"
#include "ImsaImsApi.h"
#include "ImsaProcSpmMsg.h"

/*lint -e767*/
#define    THIS_FILE_ID       PS_FILE_ID_IMSAPROCMMAMSG_C
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


/*lint -e960*/
/*lint -e961*/
/*****************************************************************************
 Function Name  : IMSA_ProcMmaMsg()
 Description    : MMA消息处理函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-21  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcMmaMsg(const VOS_VOID *pRcvMsg )
{
    /* 定义消息头指针*/
    PS_MSG_HEADER_STRU          *pHeader = VOS_NULL_PTR;
    IMSA_CONTROL_MANAGER_STRU   *pstControlManager;

    /* 获取消息头指针*/
    pHeader = (PS_MSG_HEADER_STRU *) pRcvMsg;

    pstControlManager = IMSA_GetControlManagerAddress();

    /* 关机过程中，收到除开关机消息外的消息，都直接丢弃 */
    if ((IMSA_STATUS_STOPING == pstControlManager->enImsaStatus)
        && (ID_MMA_IMSA_START_REQ != pHeader->ulMsgName)
        && (ID_MMA_IMSA_STOP_REQ != pHeader->ulMsgName))
    {
        IMSA_WARN_LOG("IMSA_ProcMmaMsg: Status is Stoping and msg is not start and stop!");
        return;
    }

    switch(pHeader->ulMsgName)
    {
        case ID_MMA_IMSA_START_REQ:
            IMSA_ProcMmaMsgStartReq();
            break;

        case ID_MMA_IMSA_STOP_REQ:
            IMSA_ProcMmaMsgStopReq();
            break;

        case ID_MMA_IMSA_DEREG_REQ:
            IMSA_ProcMmaMsgDeregReq();
            break;

        case ID_MMA_IMSA_SERVICE_CHANGE_IND:
            IMSA_ProcMmaMsgServiceChangeInd(pRcvMsg);
            break;

        case ID_MMA_IMSA_CAMP_INFO_CHANGE_IND:
            IMSA_ProcMmaMsgCampInfoChangeInd(pRcvMsg);
            break;
        default:
            break;
    }
}

/*****************************************************************************
 Function Name  : IMSA_ProcMmaMsgStartReq()
 Description    : MMA开机消息处理函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-21  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcMmaMsgStartReq(VOS_VOID  )
{
    IMSA_CONTROL_MANAGER_STRU  *pstControlManager;
    static VOS_UINT8            ucD2ImsTaskStart = 0;

    pstControlManager = IMSA_GetControlManagerAddress();

    if(ucD2ImsTaskStart == 0)
    {
        /*初始化D2协议栈*/
        (VOS_VOID)IMSA_ImsInit();

        ucD2ImsTaskStart ++;
    }

    /*判断开机状态，如果已开机，则回复开机成功*/
    if(pstControlManager->enImsaStatus == IMSA_STATUS_STARTED)
    {
        IMSA_SndMmaMsgStartCnf();
        return;
    }

    /*已处于开机过程状态，则丢弃*/
    if(pstControlManager->enImsaStatus == IMSA_STATUS_STARTING)
    {
        IMSA_WARN_LOG("IMSA_ProcMmaMsgStartReq: Status is Starting!");
        return;
    }

    IMSA_StopTimer(&pstControlManager->stProtectTimer);

    /*初始化*/

    /*读取NV*/
    IMSA_ReadImsaNV();

    /*通知IMS开机，转状态*/
    pstControlManager->enImsaStatus = IMSA_STATUS_STARTING;

    IMSA_SndImsMsgStartReq();

    IMSA_StartTimer(&pstControlManager->stProtectTimer);

    /*卡在位，则读取卡信息*/
    if(pstControlManager->enImsaIsimStatus == IMSA_ISIM_STATUS_AVAILABLE)
    {
        IMSA_INFO_LOG("IMSA_ProcMmaMsgStartReq: ISIM is available!");
        IMSA_ReadIsimFile();
    }
}

/*****************************************************************************
 Function Name  : IMSA_SndMmaMsgStartCnf()
 Description    : 给MMA发送开机回复消息
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-26  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SndMmaMsgStartCnf(VOS_VOID  )
{
    IMSA_MMA_START_CNF_STRU           *pstImsaMmaStartCnf;

    /*分配空间并检验分配是否成功*/
    pstImsaMmaStartCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_MMA_START_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstImsaMmaStartCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMmaMsgStartCnf:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstImsaMmaStartCnf), 0, IMSA_GET_MSG_LENGTH(pstImsaMmaStartCnf));

    /*填写消息头*/
    IMSA_WRITE_MMA_MSG_HEAD(pstImsaMmaStartCnf, ID_IMSA_MMA_START_CNF);

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstImsaMmaStartCnf);
}




/*****************************************************************************
 Function Name  : IMSA_ProcMmaMsgStopReq()
 Description    : MMA关机消息处理函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-21  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcMmaMsgStopReq(VOS_VOID  )
{
    IMSA_CONTROL_MANAGER_STRU *pstControlManager;

    IMSA_INFO_LOG("IMSA_ProcMmaMsgStopReq: enter!");

    pstControlManager = IMSA_GetControlManagerAddress();

    /* SRVCC异常(例如关机，DEREG REQ，状态迁离CONN+REG)，清缓存 */
    IMSA_SrvccAbormalClearBuff(IMSA_SRVCC_ABNORMAL_STOP_REQ);

    /*判断状态，如果已关机，则回复关机成功*/
    if(pstControlManager->enImsaStatus == IMSA_STATUS_NULL)
    {
        IMSA_SndMmaMsgStopCnf();
        return;
    }

    /*已处于关机状态，则丢弃*/
    if(pstControlManager->enImsaStatus == IMSA_STATUS_STOPING)
    {
        IMSA_WARN_LOG("IMSA_ProcMmaMsgStopReq: Status is Stoping!");
        return;
    }

    /*停止开关机保护定时器*/
    IMSA_StopTimer(&pstControlManager->stProtectTimer);

    /*通知IMS关机，转状态*/
    pstControlManager->enImsaStatus = IMSA_STATUS_STOPING;

    /*关机后，卡在位时，需要将卡状态置为有效，因为可能有卡无效的场景，需要关机后清除状态*/
    if(pstControlManager->enImsaIsimStatus != IMSA_ISIM_STATUS_ABSENT)
    {
        pstControlManager->enImsaIsimStatus = IMSA_ISIM_STATUS_AVAILABLE;
    }

    IMSA_SndImsMsgStopReq();

    /*启动开关机保护定时器*/
    IMSA_StartTimer(&pstControlManager->stProtectTimer);
}

/*****************************************************************************
 Function Name  : IMSA_SndMmaMsgStopCnf()
 Description    : 给MMA发送开机回复消息
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-21  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SndMmaMsgStopCnf(VOS_VOID  )
{
    IMSA_MMA_STOP_CNF_STRU           *pstImsaMmaStopCnf;

    IMSA_INFO_LOG("IMSA_SndMmaMsgStopCnf:enter!");

    /*分配空间并检验分配是否成功*/
    pstImsaMmaStopCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_MMA_STOP_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstImsaMmaStopCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMmaMsgStopCnf:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstImsaMmaStopCnf), 0, IMSA_GET_MSG_LENGTH(pstImsaMmaStopCnf));

    /*填写消息头*/
    IMSA_WRITE_MMA_MSG_HEAD(pstImsaMmaStopCnf, ID_IMSA_MMA_STOP_CNF);

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstImsaMmaStopCnf);
}



/*****************************************************************************
 Function Name  : IMSA_ProcMmaMsgDeregReq()
 Description    : MMA去注册消息处理函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-21  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcMmaMsgDeregReq(VOS_VOID )
{
    /* SRVCC异常(例如关机，DEREG REQ，状态迁离CONN+REG)，清缓存 */
    IMSA_SrvccAbormalClearBuff(IMSA_SRVCC_ABNORMAL_DEREG_REQ);

    IMSA_SRV_ProcDeregReq(IMSA_SRV_DEREG_CAUSE_MMA_DEREG_REQ);

    return ;
}

/*****************************************************************************
 Function Name  : IMSA_ProcMmaMsgDeregReq()
 Description    : MMA SERVICE CHANGE IND消息处理函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-21  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcMmaMsgServiceChangeInd
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_SRV_ProcServiceChangeInd((MMA_IMSA_SERVICE_CHANGE_IND_STRU*)pRcvMsg);

    return ;
}


VOS_VOID IMSA_ProcMmaMsgCampInfoChangeInd
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_SRV_ProcCampInfoChangeInd((MMA_IMSA_CAMP_INFO_CHANGE_IND_STRU*)pRcvMsg);

    return ;
}


/*****************************************************************************
 Function Name  : IMSA_SndMmaMsgDeregCnf()
 Description    : 给MMA发送去注册回复消息
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-26  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SndMmaMsgDeregCnf(VOS_VOID )
{
    IMSA_MMA_DEREGISTER_CNF_STRU           *pstImsaMmaDeregCnf;

    /*分配空间并检验分配是否成功*/
    pstImsaMmaDeregCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_MMA_DEREGISTER_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstImsaMmaDeregCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMmaMsgDeregCnf:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstImsaMmaDeregCnf), 0, IMSA_GET_MSG_LENGTH(pstImsaMmaDeregCnf));

    /*填写消息头*/
    IMSA_WRITE_MMA_MSG_HEAD(pstImsaMmaDeregCnf, ID_IMSA_MMA_DEREG_CNF);

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstImsaMmaDeregCnf);
}

/*****************************************************************************
 Function Name  : IMSA_ProcImsMsgStartOrStopCnf()
 Description    : 处理IMS开机或者关机结果
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-24  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcImsMsgStartOrStopCnf(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU *pstControlManager;

    IMSA_NORM_LOG("IMSA_ProcImsMsgStartOrStopCnf: Enter!");

    pstControlManager = IMSA_GetControlManagerAddress();

    /*停止开关机保护定时器*/
    IMSA_StopTimer(&pstControlManager->stProtectTimer);

    /*关机流程*/
    if(pstControlManager->enImsaStatus == IMSA_STATUS_STOPING)
    {
        /*清除资源*/
        IMSA_ClearResource();

        /*进入等待开机状态*/
        pstControlManager->enImsaStatus = IMSA_STATUS_NULL;

        IMSA_SndMmaMsgStopCnf();
    }
    else if(pstControlManager->enImsaStatus == IMSA_STATUS_STARTING)
    {
        /*进入开机状态*/
        pstControlManager->enImsaStatus = IMSA_STATUS_STARTED;

        IMSA_SndMmaMsgStartCnf();

        /* 给IMS配置UE能力 */
        IMSA_ConfigUeCapabilityInfo2Ims();

        if ((IMSA_OP_TRUE == pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpImpi)
            && (IMSA_OP_TRUE == pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpTImpu)
            && (IMSA_OP_TRUE == pstControlManager->stImsaCommonInfo.bitOpHomeNetDomainName))
        {
            /* config account info to ims */
            IMSA_ConfigAccoutInfo2Ims(IMSA_REG_TYPE_NORMAL);

            /* config account info to ims */
            IMSA_ConfigAccoutInfo2Ims(IMSA_REG_TYPE_EMC);
        }

        /* set imei */
        IMSA_ConfigImei2Ims();

        /* set retry time */
        IMSA_ConfigTimerLength2Ims();

        IMSA_ConfigSipPort2Ims();

        /* 给IMS配置SIP信息 */
        IMSA_ConfigSipInfo2Ims();

        IMSA_ConfigVoipInfo2Ims();

        IMSA_ConfigCodeInfo2Ims();

        IMSA_ConfigSsConfInfo2Ims();

        IMSA_ConfigSecurityInfo2Ims();
    }
    else
    {
        IMSA_WARN_LOG("IMSA_ProcImsMsgStartOrStopCnf: err stat!");
    }


}
 VOS_VOID IMSA_SndMmaMsgImsVoiceCapNotify(MMA_IMSA_IMS_VOICE_CAP_ENUM_UINT8 enImsVoiceCap)
 {
     IMSA_MMA_IMS_VOICE_CAP_NOTIFY_STRU     *pstImsaMmaImsVoiceCap;

     /*分配空间并检验分配是否成功*/
     pstImsaMmaImsVoiceCap = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_MMA_IMS_VOICE_CAP_NOTIFY_STRU));

     /*检测是否分配成功*/
     if (VOS_NULL_PTR == pstImsaMmaImsVoiceCap)
     {
         /*打印异常信息*/
         IMSA_ERR_LOG("IMSA_SndMmaMsgDeregCnf:ERROR:Alloc Msg fail!");
         return ;
     }

     /*清空*/
     IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstImsaMmaImsVoiceCap), 0, IMSA_GET_MSG_LENGTH(pstImsaMmaImsVoiceCap));

     /*填写消息头*/
     IMSA_WRITE_MMA_MSG_HEAD(pstImsaMmaImsVoiceCap, ID_IMSA_MMA_IMS_VOICE_CAP_NOTIFY);


     pstImsaMmaImsVoiceCap->enImsVoiceCap = enImsVoiceCap;

     /*调用消息发送函数 */
     IMSA_SND_MSG(pstImsaMmaImsVoiceCap);
 }
/*lint +e961*/
/*lint +e960*/



#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaProcMmaMsg.c */



