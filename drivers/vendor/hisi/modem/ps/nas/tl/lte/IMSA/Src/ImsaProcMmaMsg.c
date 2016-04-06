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

    /* 获取消息头指针*/
    pHeader = (PS_MSG_HEADER_STRU *) pRcvMsg;

    switch(pHeader->ulMsgName)
    {
        case ID_MMA_IMSA_START_REQ:
            IMSA_ProcMmaMsgStartReq();
            break;

        case ID_MMA_IMSA_STOP_REQ:
            IMSA_ProcMmaMsgStopReq(pRcvMsg);
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
        case ID_MMA_IMSA_MODEM1_INFO_IND:
            IMSA_ProcMmaMsgModem1InfoInd(pRcvMsg);
            break;
        case ID_MMA_IMSA_VOICE_DOMAIN_CHANGE_IND:
            IMSA_ProcMmaMsgVoiceDomainChangeInd(pRcvMsg);
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
VOS_VOID IMSA_ProcMmaMsgStopReq
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_CONTROL_MANAGER_STRU *pstControlManager;

    MMA_IMSA_STOP_REQ_STRU            *pstMmaImsaStopReq = VOS_NULL_PTR;

    pstMmaImsaStopReq = (MMA_IMSA_START_REQ_STRU*) pRcvMsg;

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

    /* 记录关机请求类型。如果关机请求类型是ims动态关闭，则需要在收到IMS关机结果后释放现有承载 */
    pstControlManager->enStopType = pstMmaImsaStopReq->enStopType;

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

VOS_VOID IMSA_ProcMmaMsgModem1InfoInd
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_SRV_ProcModem1InfoInd((MMA_IMSA_MODEM1_INFO_IND_STRU*)pRcvMsg);
    return;
}

VOS_VOID IMSA_ProcMmaMsgVoiceDomainChangeInd
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_SRV_ProcVoiceDomainChangeInd((MMA_IMSA_VOICE_DOMAIN_CHANGE_IND_STRU*)pRcvMsg);

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
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager;
    IMSA_NORMAL_CONN_STRU              *pstNormalConn      = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn         = VOS_NULL_PTR;
    VOS_UINT8                           ucOpid             = IMSA_NULL;
    VOS_UINT32                          ulLoop             = IMSA_NULL;

    IMSA_NORM_LOG("IMSA_ProcImsMsgStartOrStopCnf: Enter!");

    pstControlManager = IMSA_GetControlManagerAddress();

    /*停止开关机保护定时器*/
    IMSA_StopTimer(&pstControlManager->stProtectTimer);

    /*关机流程*/
    if(pstControlManager->enImsaStatus == IMSA_STATUS_STOPING)
    {
        /* 如果是IMS动态关闭类型的关机，则释放链接 */
        if (pstControlManager->enStopType == MMA_IMSA_STOP_TYPE_IMS_SWITCH_OFF)
        {
            pstEmcConn      = IMSA_CONN_GetEmcConnAddr();
            if (IMSA_CONN_STATUS_IDLE != pstEmcConn->enImsaConnStatus)
            {
                /* 请求APS释放连接 */
                IMSA_WARN_LOG("IMSA_ProcImsMsgStartOrStopCnf:releasing emc conn!");
                /* 如果在CONNING态，则请求释放正在建立的信令承载和已经激活的信令承载 */
                if (IMSA_CONN_STATUS_CONNING == pstEmcConn->enImsaConnStatus)
                {
                    /* 产生OPID并存储 */
                    IMSA_CONN_AssignOpid(IMSA_CONN_TYPE_EMC, &ucOpid);
                    if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                                    pstEmcConn->stSelSdfPara.ucCid))
                    {
                        IMSA_WARN_LOG("IMSA_ProcImsMsgStartOrStopCnf:EMC,conninig,CallEnd failed!");
                    }
                }

                else
                {
                    /* 产生OPID并存储 */
                    IMSA_CONN_AssignOpid(IMSA_CONN_TYPE_EMC, &ucOpid);

                    if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                                    (VOS_UINT8)pstEmcConn->stSipSignalPdp.ucCid))
                    {
                        IMSA_WARN_LOG("IMSA_ProcImsMsgStartOrStopCnf:IMS Swtich off,conn,CallEnd failed!");
                    }
                    IMSA_CONN_SndCdsSetImsBearerReq();
                }
            }
            pstNormalConn      = IMSA_CONN_GetNormalConnAddr();
            if (IMSA_CONN_STATUS_IDLE != pstNormalConn->enImsaConnStatus)
            {
                /* 请求APS释放连接 */
                IMSA_WARN_LOG("IMSA_ProcImsMsgStartOrStopCnf:releasing normal conn!");
                /* 如果在CONNING态，则请求释放正在建立的信令承载和已经激活的信令承载 */
                if (IMSA_CONN_STATUS_CONNING == pstNormalConn->enImsaConnStatus)
                {
                    /* 产生OPID并存储 */
                    IMSA_CONN_AssignOpid(IMSA_CONN_TYPE_NORMAL, &ucOpid);

                    if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                                    pstNormalConn->stSelSdfPara.ucCid))
                    {
                        IMSA_WARN_LOG("IMSA_ProcImsMsgStartOrStopCnf:IMS Swtich off,normal,conninig,CallEnd failed!");
                    }
                }

                if (0 == pstNormalConn->ulSipSignalPdpNum)
                {
                    IMSA_WARN_LOG("IMSA_ProcImsMsgStartOrStopCnf:IMS Swtich off,normal,conning,no active pdp!");
                }
                else
                {
                    /* 如果建立了多个承载，需要逐个释放 */
                    for (ulLoop = 0 ; ulLoop < pstNormalConn->ulSipSignalPdpNum ; ulLoop ++)
                    {
                        /* 产生OPID并存储 */
                        IMSA_CONN_AssignOpid(IMSA_CONN_TYPE_NORMAL, &ucOpid);
                        if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                                    pstNormalConn->astSipSignalPdpArray[ulLoop].ucCid))
                        {
                            IMSA_WARN_LOG("IMSA_ProcImsMsgStartOrStopCnf:IMS Swtich off,normal,CallEnd failed!");
                        }
                    }
                    IMSA_CONN_SndCdsSetImsBearerReq();
               }
            }
        }
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

        IMSA_ConfigMediaParmInfo2Ims();

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_ConfigErrlogCtrlInfo2Ims();
        #endif
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



