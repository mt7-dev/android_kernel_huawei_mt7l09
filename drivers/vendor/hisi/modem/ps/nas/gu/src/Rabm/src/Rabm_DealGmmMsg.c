

/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "Rabm2GInclude.h"
#include "NasRabmMain.h"
#include "NasRabmMsgProc.h"


#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_RABM_DEALGMMMSG_C
/*****************************************************************************
   2 全局变量定义
*****************************************************************************/

/*****************************************************************************
   3 函数实现
*****************************************************************************/
extern VOS_VOID Tc_TaskInit(VOS_VOID);


VOS_VOID  NAS_RabmGmmRoutingAreaUpdateIndMsg( VOS_VOID *pMsg )
{
    GMMRABM_ROUTING_AREA_UPDATE_IND_MSG     *pstRAMsg;
    VOS_UINT8                                ucLoop;
    VOS_UINT8                                ucPdpActFlag = 0;

    pstRAMsg = ( GMMRABM_ROUTING_AREA_UPDATE_IND_MSG * )pMsg;

    if (NAS_MML_NET_RAT_TYPE_WCDMA == NAS_RABM_GetCurrentSysMode())
    {
        if ( GMM_RABM_RAU_UNDER_HANDOVER == pstRAMsg->ucGmmRauCause )
        {
            /*打印流程信息---收到2G向3G切换中的路由区更新消息:*/
            RABM_LOG_NORMAL( "NAS_RabmGmmRoutingAreaUpdateIndMsg:NORMAL:RECEIVE RA Update Msg while 2G changes to 3G" );

            if ( GMM_RABM_RAU_SUCCESS == pstRAMsg->ucResult )
            {
                /*寻找是否有激活的PDP:*/
                for ( ucLoop = 0; ucLoop < RABM_3G_PS_MAX_ENT_NUM ; ucLoop++ )
                {
                    if( RABM_NSAPI_ACTIVE_NO_RAB == g_aRabmPsEnt[ucLoop].ucState )
                    {
                        ucPdpActFlag = 1;
                        break;
                    }
                }

                if ( 1 == ucPdpActFlag )
                {
                    gRabm2GTo3GDataResume = VOS_TRUE;
                }
            }
            else
            {
                 /*打印警告信息---收到2G向3G切换过后的路由区更新失败消息:*/
                 RABM_LOG_NORMAL( "NAS_RabmGmmRoutingAreaUpdateIndMsg:NORMAL:RECEIVE RA Update Msg(2G->3G),but RAU Fail!" );
                 NAS_Rabm2GTo3GRAUFail();
            }
        }
        else
        {
            /*打印流程信息---收到3G下的NORMAL RAU消息:*/
            RABM_LOG_NORMAL( "NAS_RabmGmmRoutingAreaUpdateIndMsg:NORMAL:RECEIVE RA Update Msg(GMM_RABM_RAU_UNDER_NORMAL) in 3G mode!" );
            return;
        }
    }
    else     /*若当前运行模式为2G*/
    {
        /*正常的路由区更新:*/
        if ( GMM_RABM_RAU_UNDER_NORMAL == pstRAMsg->ucGmmRauCause )
        {
            /*打印流程信息---收到2G中的路由区更新消息:*/
            RABM_LOG_NORMAL( "NAS_RabmGmmRoutingAreaUpdateIndMsg:NORMAL:RECEIVE RA Update Msg(in 2G Mode)" );

            if ( GMM_RABM_RAU_SUCCESS == pstRAMsg->ucResult )
            {
                NAS_Rabm2GRoutingAreaUpdate( pstRAMsg );
            }
            else
            {
                /*打印警告信息---收到2G中的路由区更新失败消息:*/
                RABM_LOG_WARNING( "NAS_RabmGmmRoutingAreaUpdateIndMsg:WARNING:RECEIVE RA Update Msg(in 2G Mode),but RAU Fail!" );
            }
        }
        else  /*切换中的路由区更新*/
        {
            if ( GMM_RABM_RAU_SUCCESS == pstRAMsg->ucResult )
            {
                /*打印流程信息---收到3G向2G切换中的路由区更新消息:*/
                RABM_LOG_NORMAL( "NAS_RabmGmmRoutingAreaUpdateIndMsg:NORMAL:RECEIVE RA Update Msg(3G->2G)" );

                /*进行3G到2G中的路由区更新流程:*/
                NAS_Rabm3GTo2GRoutingAreaUpdate( pstRAMsg );
            }
            else
            {
                 /*打印警告信息---收到3G向2G切换中的路由区更新失败消息:*/
                 RABM_LOG_WARNING( "NAS_RabmGmmRoutingAreaUpdateIndMsg:WARNING:RECEIVE RA Update Msg(3G->2G),but RAU Fail!" );
                 NAS_Rabm3GTo2GRAUFail();
            }
        }
    }
}



VOS_VOID  NAS_Rabm2GTo3GRAUFail( VOS_VOID )
{
    /*释放在2G向3G切换开始时SNDCP缓存的数据:*/
    SN_RabmClear2G3Share();

    return;
}
VOS_VOID  NAS_Rabm3GTo2GRAUFail( VOS_VOID )
{
    VOS_UINT32                              ulLength;
    GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG    *pstRAUpdateRsp;

    /*发送GMMRABM_ROUT_AREA_UPDATE_RSP消息:*/
    ulLength       = sizeof( GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG ) - VOS_MSG_HEAD_LENGTH;
    pstRAUpdateRsp = ( GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG * )PS_ALLOC_MSG( WUEPS_PID_RABM, ulLength );
    if ( VOS_NULL_PTR == pstRAUpdateRsp )
    {
        /*打印出错信息---申请消息包失败:*/
        RABM_LOG_ERROR( "NAS_Rabm3GTo2GRAUFail:ERROR:Allocates a message packet for ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP msg FAIL!" );

        return;
    }
    /*填写消息头:*/
    pstRAUpdateRsp->MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstRAUpdateRsp->MsgHeader.ulSenderPid     = WUEPS_PID_RABM;
    pstRAUpdateRsp->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstRAUpdateRsp->MsgHeader.ulReceiverPid   = WUEPS_PID_GMM;
    pstRAUpdateRsp->MsgHeader.ulLength        = ulLength;

    pstRAUpdateRsp->MsgHeader.ulMsgName       = ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP;
    /*填写消息体:*/
    PS_NAS_MEM_SET( pstRAUpdateRsp->aucRecvNPDUNumListIE, 0, RABM_RECV_N_PDU_NUM_LIST_IE_MAXLEN );
    /*置N-PDU标识为无效:*/
    pstRAUpdateRsp->ucNpduValidFlg            = GMM_RABM_NPDU_INVALID;
    /*发送该消息:*/
    if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_RABM, pstRAUpdateRsp ) )
    {
        /*打印警告信息---发送消息失败:*/
        RABM_LOG_WARNING( "NAS_Rabm3GTo2GRAUFail:WARNING:SEND ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP msg FAIL!" );
    }
    else
    {
        /*打印流程信息---发送了消息:*/
        RABM_LOG_NORMAL( "NAS_Rabm3GTo2GRAUFail:NORMAL:SEND ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP Msg" );
    }

    /*释放在3G向2G切换开始时PDCP缓存的数据:*/
    SN_RabmClear3G2Share();

    return;
}
VOS_VOID  NAS_Rabm2GRoutingAreaUpdate( GMMRABM_ROUTING_AREA_UPDATE_IND_MSG   *pMsg )
{
    VOS_UINT8                               ucLoop;
    VOS_UINT8                               ucFlag = VOS_FALSE;
    VOS_UINT32                              ulLength;
    RABM_SN_SEQUENCE_IND_MSG                *pstSnSequenceInd;
    GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG     *pstRAUpdateRsp;

    for ( ucLoop = 0; ucLoop < RABM_2G_MAX_ENT_NUM; ucLoop++ )   /*遍历2G中所有NSAPI*/
    {
        switch ( gastRabm2GEntity[ucLoop].State )    /*该NSAPI所对应的状态*/
        {
            case RABM_2G_NULL:
            case RABM_NSAPI_OK_TRANSMODE_NO:
            case RABM_ROUT_AREA_UPDATE:             /*这三种状态的处理方式一样*/
                break;
            case RABM_DATA_TRANS_STOP:
            case RABM_DATA_TRANS_READY:
                if ( VOS_FALSE == ucFlag )
                {
                    /*清标记:*/
                    ucFlag = VOS_TRUE;

                    /*获取新的TLLI值:*/
                    gulRabmTLLI = pMsg->ulTLLI;

                    /*启动Rabm.routing.area.update.T1定时器:*/
                    if ( RABM_SUCCESS != NAS_RabmStartTimer( RABM_TIMER_NAME_COMMON, RABM_TIMER_PARA_ROUT_AREA_UPDATE_T1 ) )
                    {
                        /*打印出错信息---启动Rabm.routing.area.update.T1定时器失败:*/
                        RABM_LOG_ERROR( "NAS_Rabm2GRoutingAreaUpdate:ERROR:Start Rabm.routing.area.update.T1 Timer FAIL!" );
                        return;
                    }
                    /*打印流程信息---启动了Rabm.routing.area.update.T1定时器:*/
                    RABM_LOG_NORMAL( "NAS_Rabm2GRoutingAreaUpdate:NORMAL:Start Rabm.routing.area.update.T1 Timer SUCCESS" );

                    /*发送SNRABM_SEQUENCE_IND消息:*/
                    ulLength         = sizeof( RABM_SN_SEQUENCE_IND_MSG ) - VOS_MSG_HEAD_LENGTH;
                    pstSnSequenceInd = ( RABM_SN_SEQUENCE_IND_MSG * )PS_ALLOC_MSG( WUEPS_PID_RABM, ulLength );
                    if ( VOS_NULL_PTR == pstSnSequenceInd )
                    {
                        /*打印出错信息---申请消息包失败:*/
                        RABM_LOG_ERROR( "NAS_Rabm2GRoutingAreaUpdate:ERROR:Allocates a message packet for SNRABM_SEQUENCE_IND msg FAIL!" );

                        /*关闭Rabm.routing.area.update.T1定时器:*/
                        if ( RABM_SUCCESS != NAS_RabmStopTimer( RABM_TIMER_NAME_COMMON, RABM_TIMER_PARA_ROUT_AREA_UPDATE_T1 ) )
                        {
                            /*打印警告信息---关闭Rabm.routing.area.update.T1定时器失败:*/
                            RABM_LOG_WARNING( "NAS_Rabm2GRoutingAreaUpdate:WARNING:Stop Rabm.routing.area.update.T1 Timer FAIL!" );
                        }
                        else
                        {
                            /*打印流程信息---关闭了Rabm.routing.area.update.T1定时器:*/
                            RABM_LOG_NORMAL( "NAS_Rabm2GRoutingAreaUpdate:NORMAL:Stop Rabm.routing.area.update.T1 Timer SUCCESS" );
                        }
                        return;
                    }
                    /*填写消息头:*/
                    pstSnSequenceInd->ulSenderCpuId   = VOS_LOCAL_CPUID;
                    pstSnSequenceInd->ulSenderPid     = WUEPS_PID_RABM;
                    pstSnSequenceInd->ulReceiverCpuId = VOS_LOCAL_CPUID;
                    pstSnSequenceInd->ulReceiverPid   = UEPS_PID_SN;
                    pstSnSequenceInd->ulLength        = ulLength;
                    /*填写消息体:*/
                    pstSnSequenceInd->usMsgType                = ID_SN_RABM_SEQUENCE_IND;
                    pstSnSequenceInd->usPId                    = 5;
                    pstSnSequenceInd->RabmSnSequenceInd.ulTLLI = pMsg->ulTLLI;
                    /*填入N-PDU数组:*/
                    NAS_RabmDecodeNPdu( pMsg, &pstSnSequenceInd->RabmSnSequenceInd.RecvNPduNumList );
                    /*发送该消息:*/
                    if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_RABM, pstSnSequenceInd ) )
                    {
                        /*打印警告信息---发送消息失败:*/
                        RABM_LOG_WARNING( "NAS_Rabm2GRoutingAreaUpdate:WARNING:SEND SNRABM_SEQUENCE_IND msg FAIL!" );
                    }
                    else
                    {
                        /*打印流程信息---发送了消息:*/
                        RABM_LOG_NORMAL( "NAS_Rabm2GRoutingAreaUpdate:NORMAL:SEND SNRABM_SEQUENCE_IND Msg" );
                    }
                }

                /*打印流程信息---状态切换:*/
                if ( RABM_DATA_TRANS_STOP == gastRabm2GEntity[ucLoop].State )
                {
                    RABM_LOG1_NORMAL( "STATE RABM_DATA_TRANS_STOP state CHANGE TO RABM_ROUT_AREA_UPDATE state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                }
                else
                {
                    RABM_LOG1_NORMAL( "STATE RABM_DATA_TRANS_READY state CHANGE TO RABM_ROUT_AREA_UPDATE state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                }

                /*将该NSAPI的状态置为RABM_ROUT_AREA_UPDATE状态:*/
                RABM_SetGState((RABM_NSAPI_ENUM)ucLoop, RABM_ROUT_AREA_UPDATE);

                /*填入路由区发生的原因值:*/
                gastRabm2GEntity[ucLoop].RAUpdateCause = RABM_RA_UPDATE_CAUSE_NORMAL;
                break;
            default:
                /*打印出错信息---该NSAPI所处状态的字段出错:*/
                RABM_LOG1_ERROR( "NAS_Rabm2GRoutingAreaUpdate:ERROR:This NSAPI's state is WRONG!: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
        }
    }

    if ( VOS_FALSE == ucFlag )
    {
        /*获取新的TLLI值:*/
        gulRabmTLLI = pMsg->ulTLLI;

        /*发送将Rx N-PDU numbers填为0的GMMRABM_ROUTING_AREA_UPDATE_RSP消息:*/
        ulLength       = sizeof( GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG ) - VOS_MSG_HEAD_LENGTH;
        pstRAUpdateRsp = ( GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG * )PS_ALLOC_MSG( WUEPS_PID_RABM, ulLength );
        if ( VOS_NULL_PTR == pstRAUpdateRsp )
        {
            /*打印出错信息---申请消息包失败:*/
            RABM_LOG_ERROR( "NAS_Rabm2GRoutingAreaUpdate:ERROR:Allocates a message packet for ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP msg FAIL!" );
            return;
        }
        /*填写消息头:*/
        pstRAUpdateRsp->MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
        pstRAUpdateRsp->MsgHeader.ulSenderPid     = WUEPS_PID_RABM;
        pstRAUpdateRsp->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
        pstRAUpdateRsp->MsgHeader.ulReceiverPid   = WUEPS_PID_GMM;
        pstRAUpdateRsp->MsgHeader.ulLength        = ulLength;
        pstRAUpdateRsp->MsgHeader.ulMsgName       = ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP;
        /*填写消息体:*/
        PS_NAS_MEM_SET( pstRAUpdateRsp->aucRecvNPDUNumListIE, 0, RABM_RECV_N_PDU_NUM_LIST_IE_MAXLEN );
        /*填写N-PDU有效标志为无效:*/
        pstRAUpdateRsp->ucNpduValidFlg = GMM_RABM_NPDU_INVALID;
        /*发送该消息:*/
        if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_RABM, pstRAUpdateRsp ) )
        {
            /*打印警告信息---发送消息失败:*/
            RABM_LOG_WARNING( "NAS_Rabm2GRoutingAreaUpdate:WARNING:SEND ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP(N-PDU numbers is Invalid) msg FAIL!" );
        }
        else
        {
            /*打印流程信息---发送了消息:*/
            RABM_LOG_NORMAL( "NAS_Rabm2GRoutingAreaUpdate:NORMAL:SEND ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP(N-PDU numbers is Invalid) Msg" );
        }
    }
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_VOID NAS_RABM_GetLPdcpNpduNumList(
    RABM_PDCP_GET_N_PDU_NUM_LIST_ST    *pstPdcpNpduNumList
)
{
    VOS_UINT8                           ucEntId;
    VOS_UINT8                           ucState;

    for (ucEntId = 0; ucEntId < RABM_2G_MAX_ENT_NUM; ucEntId++)
    {
        ucState = NAS_RABM_GetGPsEntState(ucEntId);
        if (RABM_2G_NULL != ucState)
        {
            pstPdcpNpduNumList->aucNPDUNumList[ucEntId + RABM_2G_NSAPI_OFFSET][0] = 1;
            pstPdcpNpduNumList->aucNPDUNumList[ucEntId + RABM_2G_NSAPI_OFFSET][1] = 0;
        }
    }
}
#endif


VOS_VOID  NAS_Rabm3GTo2GRoutingAreaUpdate( GMMRABM_ROUTING_AREA_UPDATE_IND_MSG   *pMsg )
{
    VOS_UINT8                               ucLoop;
    VOS_UINT8                               ucFlag = VOS_FALSE;
    VOS_UINT32                              ulLength;
    RABM_PDCP_GET_N_PDU_NUM_LIST_ST         stPdcpNpduNumList;
    GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG    *pstRAUpdateRsp;
    RABM_SN_SEQUENCE_IND_MSG               *pstSnSequenceInd;

    /* 初始化N-PDU Number List */
    PS_MEM_SET(&stPdcpNpduNumList, 0x00, sizeof(RABM_PDCP_GET_N_PDU_NUM_LIST_ST));

    for ( ucLoop = 0; ucLoop < RABM_2G_MAX_ENT_NUM; ucLoop++ )   /*遍历所有NSAPI*/
    {
        switch ( gastRabm2GEntity[ucLoop].State )     /*该NSAPI所对应的状态*/
        {
            case RABM_2G_NULL :
                break;
            case RABM_NSAPI_OK_TRANSMODE_NO :
            case RABM_ROUT_AREA_UPDATE :
            case RABM_DATA_TRANS_STOP :                     /*这四种状态的处理方式一样*/
                /*打印警告信息---该NSAPI所处的状态不合逻辑:*/
                if ( RABM_NSAPI_OK_TRANSMODE_NO == gastRabm2GEntity[ucLoop].State )
                {
                    RABM_LOG1_WARNING( "NAS_Rabm3GTo2GRoutingAreaUpdate:WARNING:This NSAPI's state RABM_NSAPI_OK_TRANSMODE_NO is Abnormal!: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                }
                else if ( RABM_ROUT_AREA_UPDATE == gastRabm2GEntity[ucLoop].State )
                {
                    RABM_LOG1_WARNING( "NAS_Rabm3GTo2GRoutingAreaUpdate:WARNING:This NSAPI's state RABM_ROUT_AREA_UPDATE is Abnormal!: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                }
                else
                {
                    RABM_LOG1_WARNING( "NAS_Rabm3GTo2GRoutingAreaUpdate:WARNING:This NSAPI's state RABM_DATA_TRANS_STOP is Abnormal!: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                }
                break;
            case RABM_DATA_TRANS_READY :
                if ( VOS_FALSE == ucFlag )
                {
                    /*清标记:*/
                    ucFlag = VOS_TRUE;

                    /*获取新的TLLI值:*/
                    gulRabmTLLI = pMsg->ulTLLI;

                    /*启动Rabm.Sys.3To2.T3定时器:*/
                    if ( RABM_SUCCESS != NAS_RabmStartTimer( RABM_TIMER_NAME_COMMON, RABM_TIMER_PARA_3_TO_2_T3 ) )
                    {
                        /*打印出错信息---启动Rabm.Sys.3To2.T3定时器失败:*/
                        RABM_LOG_ERROR( "NAS_Rabm3GTo2GRoutingAreaUpdate:ERROR:Start Rabm.Sys.3To2.T3 Timer FAIL!" );
                        return;
                    }
                    /*打印流程信息---启动了Rabm.Sys.3To2.T3定时器:*/
                    RABM_LOG_NORMAL( "NAS_Rabm3GTo2GRoutingAreaUpdate:NORMAL:Start Rabm.Sys.3To2.T3 Timer SUCCESS" );

                    /*发送SNRABM_SEQUENCE_IND消息:*/
                    ulLength         = sizeof( RABM_SN_SEQUENCE_IND_MSG ) - VOS_MSG_HEAD_LENGTH;
                    pstSnSequenceInd = ( RABM_SN_SEQUENCE_IND_MSG * )PS_ALLOC_MSG( WUEPS_PID_RABM, ulLength );
                    if ( VOS_NULL_PTR == pstSnSequenceInd )
                    {
                        /*打印出错信息---申请消息包失败:*/
                        RABM_LOG_ERROR( "NAS_Rabm3GTo2GRoutingAreaUpdate:ERROR:Allocates a message packet for SNRABM_SEQUENCE_IND msg FAIL!" );

                        /*关闭Rabm.Sys.3To2.T3定时器:*/
                        if ( RABM_SUCCESS != NAS_RabmStopTimer( RABM_TIMER_NAME_COMMON, RABM_TIMER_PARA_3_TO_2_T3 ) )
                        {
                            /*打印出错信息---关闭Rabm.Sys.3To2.T3定时器失败:*/
                            RABM_LOG_WARNING( "NAS_Rabm3GTo2GRoutingAreaUpdate:WARNING:Stop Rabm.Sys.3To2.T3 Timer FAIL!" );
                        }
                        else
                        {
                            /*打印出错信息---关闭Rabm.Sys.3To2.T3定时器成功:*/
                            RABM_LOG_NORMAL( "NAS_Rabm3GTo2GRoutingAreaUpdate:NORMAL:Stop Rabm.Sys.3To2.T3 Timer SUCCESS!" );
                        }
                        return;
                    }
                    /*填写消息头:*/
                    pstSnSequenceInd->ulSenderCpuId   = VOS_LOCAL_CPUID;
                    pstSnSequenceInd->ulSenderPid     = WUEPS_PID_RABM;
                    pstSnSequenceInd->ulReceiverCpuId = VOS_LOCAL_CPUID;
                    pstSnSequenceInd->ulReceiverPid   = UEPS_PID_SN;
                    pstSnSequenceInd->ulLength        = ulLength;
                    /*填写消息体:*/
                    pstSnSequenceInd->usMsgType                = ID_SN_RABM_SEQUENCE_IND;
                    pstSnSequenceInd->usPId                    = 5;
                    pstSnSequenceInd->RabmSnSequenceInd.ulTLLI = pMsg->ulTLLI;
                    /*填入N-PDU数组:*/
                    NAS_RabmDecodeNPdu( pMsg, &pstSnSequenceInd->RabmSnSequenceInd.RecvNPduNumList );
                    /*发送该消息:*/
                    if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_RABM, pstSnSequenceInd ) )
                    {
                        /*打印警告信息---发送消息失败:*/
                        RABM_LOG_WARNING( "NAS_Rabm3GTo2GRoutingAreaUpdate:WARNING:SEND SNRABM_SEQUENCE_IND msg FAIL!" );
                    }
                    else
                    {
                        /*打印流程信息---发送了消息:*/
                        RABM_LOG_NORMAL( "NAS_Rabm3GTo2GRoutingAreaUpdate:NORMAL:SEND SNRABM_SEQUENCE_IND Msg" );
                    }

                    /*发送GMMRABM_ROUT_AREA_UPDATE_RSP消息:*/
                    ulLength       = sizeof( GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG ) - VOS_MSG_HEAD_LENGTH;
                    pstRAUpdateRsp = ( GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG * )PS_ALLOC_MSG( WUEPS_PID_RABM, ulLength );
                    if ( VOS_NULL_PTR == pstRAUpdateRsp )
                    {
                        /*打印出错信息---申请消息包失败:*/
                        RABM_LOG_ERROR( "NAS_Rabm3GTo2GRoutingAreaUpdate:ERROR:Allocates a message packet for ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP msg FAIL!" );

                        /*关闭Rabm.Sys.3To2.T3定时器:*/
                        if ( RABM_SUCCESS != NAS_RabmStopTimer( RABM_TIMER_NAME_COMMON, RABM_TIMER_PARA_3_TO_2_T3 ) )
                        {
                            /*打印出错信息---关闭Rabm.Sys.3To2.T3定时器失败:*/
                            RABM_LOG_WARNING( "NAS_Rabm3GTo2GRoutingAreaUpdate:WARNING:Stop Rabm.Sys.3To2.T3 Timer FAIL!" );
                        }
                        else
                        {
                            /*打印出错信息---关闭Rabm.Sys.3To2.T3定时器成功:*/
                            RABM_LOG_NORMAL( "NAS_Rabm3GTo2GRoutingAreaUpdate:NORMAL:Stop Rabm.Sys.3To2.T3 Timer SUCCESS!" );
                        }
                        return;
                    }
                    /*填写消息头:*/
                    pstRAUpdateRsp->MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
                    pstRAUpdateRsp->MsgHeader.ulSenderPid     = WUEPS_PID_RABM;
                    pstRAUpdateRsp->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
                    pstRAUpdateRsp->MsgHeader.ulReceiverPid   = WUEPS_PID_GMM;
                    pstRAUpdateRsp->MsgHeader.ulLength        = ulLength;

                    pstRAUpdateRsp->MsgHeader.ulMsgName       = ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP;
                    /*置N-PDU为有效:*/
                    pstRAUpdateRsp->ucNpduValidFlg            = GMM_RABM_NPDU_VALID;

                    /* 获取N-PDU Number List */
#if (FEATURE_ON == FEATURE_LTE)
                    if (VOS_TRUE == NAS_RABM_GetL2GFlg())
                    {
                        NAS_RABM_GetLPdcpNpduNumList(&stPdcpNpduNumList);
                        NAS_RABM_ClearL2GFlg();
                    }
                    else
                    {
                        stPdcpNpduNumList = gPdcpRcvNpduNum;
                    }
#else
                    stPdcpNpduNumList = gPdcpRcvNpduNum;
#endif
                    /*填入N-PDU码流:*/
                    NAS_RabmEncodeNPdu(pstRAUpdateRsp, (RABM_SNDCP_RECV_N_PDU_NUM_LIST_ST*)&stPdcpNpduNumList);

                    /*发送该消息:*/
                    if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_RABM, pstRAUpdateRsp ) )
                    {
                        /*打印警告信息---发送消息失败:*/
                        RABM_LOG_WARNING( "NAS_Rabm3GTo2GRoutingAreaUpdate:WARNING:SEND ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP msg FAIL!" );
                    }
                    else
                    {
                        /*打印流程信息---发送了消息:*/
                        RABM_LOG_NORMAL( "NAS_Rabm3GTo2GRoutingAreaUpdate:NORMAL:SEND ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP Msg" );
                    }
                }

                /*将该NSAPI的状态置为RABM_ROUT_AREA_UPDATE状态:*/
                RABM_SetGState((RABM_NSAPI_ENUM)ucLoop, RABM_ROUT_AREA_UPDATE);
                /*打印流程信息---状态切换:*/
                RABM_LOG1_NORMAL( "STATE RABM_DATA_TRANS_READY state CHANGE TO RABM_ROUT_AREA_UPDATE state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                /*填入路由区发生的原因值:*/
                gastRabm2GEntity[ucLoop].RAUpdateCause = RABM_RA_UPDATE_CAUSE_3G_TO_2G;
                break;
            default:
                /*打印出错信息---该NSAPI所处状态的字段出错:*/
                RABM_LOG1_ERROR( "NAS_Rabm3GTo2GRoutingAreaUpdate:ERROR:This NSAPI's state is WRONG!: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
        }
    }

    if ( VOS_FALSE == ucFlag )
    {
        /*获取新的TLLI值:*/
        gulRabmTLLI = pMsg->ulTLLI;

        /*发送将Rx N-PDU numbers填为0的GMMRABM_ROUTING_AREA_UPDATE_RSP消息:*/
        ulLength       = sizeof( GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG ) - VOS_MSG_HEAD_LENGTH;
        pstRAUpdateRsp = ( GMMRABM_ROUTING_AREA_UPDATE_RSP_MSG * )PS_ALLOC_MSG( WUEPS_PID_RABM, ulLength );
        if ( VOS_NULL_PTR == pstRAUpdateRsp )
        {
            /*打印出错信息---申请消息包失败:*/
            RABM_LOG_ERROR( "NAS_Rabm3GTo2GRoutingAreaUpdate:ERROR:Allocates a message packet for ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP msg FAIL!" );
            return;
        }
        /*填写消息头:*/
        pstRAUpdateRsp->MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
        pstRAUpdateRsp->MsgHeader.ulSenderPid     = WUEPS_PID_RABM;
        pstRAUpdateRsp->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
        pstRAUpdateRsp->MsgHeader.ulReceiverPid   = WUEPS_PID_GMM;
        pstRAUpdateRsp->MsgHeader.ulLength        = ulLength;

        pstRAUpdateRsp->MsgHeader.ulMsgName       = ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP;
        /*填写消息体:*/
        PS_NAS_MEM_SET( pstRAUpdateRsp->aucRecvNPDUNumListIE, 0, RABM_RECV_N_PDU_NUM_LIST_IE_MAXLEN );
        /*填写N-PDU有效标志为无效:*/
        pstRAUpdateRsp->ucNpduValidFlg = GMM_RABM_NPDU_INVALID;
        /*发送该消息:*/
        if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_RABM, pstRAUpdateRsp ) )
        {
            /*打印警告信息---发送消息失败:*/
            RABM_LOG_WARNING( "NAS_Rabm3GTo2GRoutingAreaUpdate:WARNING:SEND ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP(N-PDU numbers is Invalid) msg FAIL!" );
        }
        else
        {
            /*打印流程信息---发送了消息:*/
            RABM_LOG_NORMAL( "NAS_Rabm3GTo2GRoutingAreaUpdate:NORMAL:SEND ID_RABM_GMM_ROUTING_AREA_UPDATE_RSP(N-PDU numbers is Invalid) Msg" );
        }
    }
}


VOS_VOID  NAS_RabmDeal2GTo3GSuspendIndMsg( VOS_VOID )
{
    VOS_UINT8               ucLoop;
    VOS_UINT8               ucFlag1 = VOS_FALSE;
    ST_APP_RABM_STATUS      stReportStatus;

    for ( ucLoop = 0; ucLoop < RABM_2G_MAX_ENT_NUM; ucLoop++ )   /*遍历2G的每个NSAPI*/
    {
        switch ( gastRabm2GEntity[ucLoop].State )    /*该NSAPI所对应的状态*/
        {
            case RABM_2G_NULL:
                /*打印流程信息---状态不变:*/
                RABM_LOG1_NORMAL( "STATE RABM_2G_NULL state keep the same: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;
            case RABM_NSAPI_OK_TRANSMODE_NO:
                if ( RABM_ACT_REQ_T1_ON == gaRabmActReqT1Switch[ucLoop] )   /*若打开了Rabm.act.req.T1定时器*/
                {
                    /*关闭Rabm.act.req.T1定时器:*/
                    if ( RABM_SUCCESS != NAS_RabmStopTimer( ( RABM_TIMER_NAME_ENUM )ucLoop, RABM_TIMER_PARA_ACT_REQ_T1 ) )
                    {
                        /*打印警告信息---关闭Rabm.act.req.T1定时器失败:*/
                        RABM_LOG1_WARNING( "NAS_RabmDeal2GTo3GSuspendIndMsg:WARNING:Stop Rabm.act.req.T1 Timer FAIL!: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                    }
                    else
                    {
                        /*打印流程信息---关闭了Rabm.act.req.T1定时器:*/
                        RABM_LOG1_NORMAL( "NAS_RabmDeal2GTo3GSuspendIndMsg:NORMAL:Stop Rabm.act.req.T1 Timer SUCCESS!: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                    }
                }

                /*向APP上报TAFRABM_STARTSTOP_IND(STOP)消息:*/
                stReportStatus.ucDomain     = D_RABM_PS_DOMAIN;
                stReportStatus.ucRabId      = ucLoop + RABM_NSAPI_OFFSET;
                stReportStatus.ucStatus     = TAF_RABM_STOP_DATA;
                stReportStatus.ucStartCause = TAF_RABM_STOP_CAUSE_NORMAL;
                Api_AppRabmStatusInd( &stReportStatus );             /*通知APP停止数据传输*/

                /*打印流程信息---上报了消息:*/
                RABM_LOG1_NORMAL( "STATE RABM_NSAPI_OK_TRANSMODE_NO state report TAFRABM_STARTSTOP_IND(STOP) Msg to APP: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );

                /*将状态设置为RABM_DATA_TRANS_STOP状态:*/
                RABM_SetGState((RABM_NSAPI_ENUM)ucLoop, RABM_DATA_TRANS_STOP);
                /*打印流程信息---状态切换:*/
                RABM_LOG1_NORMAL( "STATE RABM_NSAPI_OK_TRANSMODE_NO state CHANGE TO RABM_DATA_TRANS_STOP state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;
            case RABM_DATA_TRANS_READY:
                /*向APP上报TAFRABM_STARTSTOP_IND(STOP)消息:*/
                stReportStatus.ucDomain     = D_RABM_PS_DOMAIN;
                stReportStatus.ucRabId      = ucLoop + RABM_NSAPI_OFFSET;
                stReportStatus.ucStatus     = TAF_RABM_STOP_DATA;
                stReportStatus.ucStartCause = TAF_RABM_STOP_CAUSE_NORMAL;
                Api_AppRabmStatusInd( &stReportStatus );             /*通知APP停止数据传输*/

                /*打印流程信息---上报了消息:*/
                RABM_LOG1_NORMAL( "STATE RABM_DATA_TRANS_READY state report TAFRABM_STARTSTOP_IND(STOP) Msg to APP: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );

                /*将状态设置为RABM_DATA_TRANS_STOP状态:*/
                RABM_SetGState((RABM_NSAPI_ENUM)ucLoop, RABM_DATA_TRANS_STOP);
                /*打印流程信息---状态切换:*/
                RABM_LOG1_NORMAL( "STATE RABM_DATA_TRANS_READY state CHANGE TO RABM_DATA_TRANS_STOP state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;
            case RABM_ROUT_AREA_UPDATE:
                if ( VOS_FALSE == ucFlag1 )
                {
                    /*清标志:*/
                    ucFlag1 = VOS_TRUE;

                    if ( RABM_ROUTING_AREA_UPDATE_T1_ON == gRabmRAT1Switch )   /*若打开了rout.area.update.T1定时器*/
                    {
                        /*关闭rout.area.update.T1定时器:*/
                        if ( RABM_SUCCESS != NAS_RabmStopTimer( RABM_TIMER_NAME_COMMON, RABM_TIMER_PARA_ROUT_AREA_UPDATE_T1 ) )
                        {
                            /*打印警告信息---关闭rout.area.update.T1定时器失败:*/
                            RABM_LOG_WARNING( "NAS_RabmDeal2GTo3GSuspendIndMsg:WARNING:Stop rout.area.update.T1 Timer FAIL!" );
                        }
                        else
                        {
                            /*打印流程信息---关闭了rout.area.update.T1定时器:*/
                            RABM_LOG_NORMAL( "NAS_RabmDeal2GTo3GSuspendIndMsg:NORMAL:Stop rout.area.update.T1 Timer SUCCESS!" );
                        }
                    }
                    else
                    {
                        if ( RABM_3G_TO_2G_T3_ON == gRabm3GTo2GT3Switch )    /*若打开了Rabm.sys.3To2.T3定时器*/
                        {
                            /*关闭Rabm.sys.3To2.T3定时器:*/
                            if ( RABM_SUCCESS != NAS_RabmStopTimer( RABM_TIMER_NAME_COMMON, RABM_TIMER_PARA_3_TO_2_T3 ) )
                            {
                                /*打印警告信息---关闭Rabm.sys.3To2.T3定时器失败:*/
                                RABM_LOG_WARNING( "NAS_RabmDeal2GTo3GSuspendIndMsg:WARNING:Stop Rabm.sys.3To2.T3 Timer FAIL!" );
                            }
                            else
                            {
                                /*打印流程信息---关闭了Rabm.sys.3To2.T3定时器:*/
                                RABM_LOG_NORMAL( "NAS_RabmDeal2GTo3GSuspendIndMsg:NORMAL:Stop Rabm.sys.3To2.T3 Timer SUCCESS!" );
                            }
                        }
                    }
                }

                if ( RABM_RA_UPDATE_CAUSE_NORMAL == gastRabm2GEntity[ucLoop].RAUpdateCause )
                {
                    /*向APP上报TAFRABM_STARTSTOP_IND(STOP)消息:*/
                    stReportStatus.ucDomain     = D_RABM_PS_DOMAIN;
                    stReportStatus.ucRabId      = ucLoop + RABM_NSAPI_OFFSET;
                    stReportStatus.ucStatus     = TAF_RABM_STOP_DATA;
                    stReportStatus.ucStartCause = TAF_RABM_STOP_CAUSE_NORMAL;
                    Api_AppRabmStatusInd( &stReportStatus );             /*通知APP停止数据传输*/

                    /*打印流程信息---上报了消息:*/
                    RABM_LOG1_NORMAL( "STATE RABM_DATA_TRANS_READY state report TAFRABM_STARTSTOP_IND(STOP) Msg to APP: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                }

                /*将状态设置为RABM_DATA_TRANS_STOP状态:*/
                RABM_SetGState((RABM_NSAPI_ENUM)ucLoop, RABM_DATA_TRANS_STOP);
                /*打印流程信息---状态切换:*/
                RABM_LOG1_NORMAL( "STATE RABM_ROUT_AREA_UPDATE state CHANGE TO RABM_DATA_TRANS_STOP state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;
            case RABM_DATA_TRANS_STOP:
                /*打印流程信息---状态不变:*/
                RABM_LOG1_NORMAL( "STATE RABM_DATA_TRANS_STOP state keep the same: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;
            default:
                /*打印出错信息--- 该NSAPI所对应的状态出错:*/
                RABM_LOG1_ERROR( "NAS_RabmDeal2GTo3GSuspendIndMsg:ERROR:This NSAPI's state is WRONG!: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;
        }
    }

}
VOS_VOID  NAS_RabmDeal3GTo2GSuspendIndMsg( VOS_VOID )
{
    VOS_UINT8               ucLoop;
    ST_APP_RABM_STATUS      stReportStatus;

    /* 停止RAB重建保护定时器 */
    if ( RABM_TRUE == g_ucReestTimerFlg )
    {
        RABM_TimerStop(0);
        g_ucReestTimerFlg = RABM_FALSE;
    }

    for ( ucLoop = 0; ucLoop < RABM_3G_PS_MAX_ENT_NUM; ucLoop++ )   /*遍历3G的每个NSAPI*/
    {
        /*若开了Rabm.ReestTimer,则关闭它，并复位此定时器标志:*/
        if ( RABM_TRUE == g_aRabmPsEnt[ucLoop].ucReEstFlg )
        {
            NAS_RABM_StopReestRabPendingTmr(ucLoop + RABM_NSAPI_OFFSET);
            g_aRabmPsEnt[ucLoop].ucReEstFlg = RABM_FALSE;
        }

        switch ( g_aRabmPsEnt[ucLoop].ucState )    /*该NSAPI所对应的状态*/
        {
            case RABM_NULL:
                break;

            case RABM_NSAPI_ACTIVE_PENDING:
            case RABM_ACT_PENDING_WITH_RAB:             /*这两种状态的处理方式一样*/
                /*打印流程信息---状态切换:*/
                if ( RABM_NSAPI_ACTIVE_PENDING == g_aRabmPsEnt[ucLoop].ucState )
                {
                    RABM_LOG1_NORMAL( "STATE RABM_NSAPI_ACTIVE_PENDING state CHANGE TO RABM_NULL state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                }
                else
                {
                    RABM_LOG1_NORMAL( "STATE RABM_ACT_PENDING_WITH_RAB state CHANGE TO RABM_NULL state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                }
                RABM_SetWState(ucLoop, RABM_NULL);                      /*状态清空 */

                RABM_RelSaveData(&g_aRabmPsEnt[ucLoop].DlDataSave, RABM_DOWNLINK);       /*释放存储的下行数据*/
                RABM_RelSaveData(&g_aRabmPsEnt[ucLoop].UlDataSave, RABM_UPLINK);         /*释放存储的上行数据*/
                break;

            case RABM_NSAPI_ACTIVE_WITH_RAB:
                /*向APP上报TAFRABM_STARTSTOP_IND(STOP)消息:*/
                stReportStatus.ucDomain     = D_RABM_PS_DOMAIN;
                stReportStatus.ucRabId      = ucLoop + RABM_NSAPI_OFFSET;
                stReportStatus.ucStatus     = TAF_RABM_STOP_DATA;
                stReportStatus.ucStartCause = TAF_RABM_STOP_CAUSE_NORMAL;
                Api_AppRabmStatusInd( &stReportStatus );             /*通知APP停止数据传输*/
                /*打印流程信息---上报了消息:*/
                RABM_LOG1_NORMAL( "STATE RABM_NSAPI_ACTIVE_WITH_RAB state report TAFRABM_STARTSTOP_IND(STOP) Msg to APP: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );

                /*置位是否通知TAF停止数据传输的标志位:*/
                g_aRabmPsEnt[ucLoop].ucStpFlg = RABM_TRUE;

                /*将状态设置为RABM_DATA_TRANSFER_STOP:*/
                RABM_SetWState(ucLoop, RABM_DATA_TRANSFER_STOP);
                /*打印流程信息---状态切换:*/
                RABM_LOG1_NORMAL( "STATE RABM_NSAPI_ACTIVE_WITH_RAB state CHANGE TO RABM_DATA_TRANSFER_STOP state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;

            case RABM_DATA_TRANSFER_STOP:
                /*打印流程信息---状态不变:*/
                RABM_LOG1_NORMAL( "STATE RABM_NSAPI_ACTIVE_NO_RAB state keep the same: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;

            case RABM_NSAPI_ACTIVE_NO_RAB:
                /*将状态设置为RABM_DATA_TRANSFER_STOP:*/
                RABM_SetWState(ucLoop, RABM_DATA_TRANSFER_STOP);
                /*打印流程信息---状态切换:*/
                RABM_LOG1_NORMAL( "STATE RABM_NSAPI_ACTIVE_NO_RAB state CHANGE TO RABM_DATA_TRANSFER_STOP state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;

            case RABM_TC_STATE_NULL:
                Tc_TaskInit();

                /*将状态设置为RABM_NULL:*/
                RABM_SetWState(ucLoop, RABM_NULL);
                /*打印流程信息---状态切换:*/
                RABM_LOG1_NORMAL( "STATE RABM_TC_STATE_NULL state CHANGE TO RABM_NULL state: NSAPI:", ucLoop+ RABM_NSAPI_OFFSET );
                break;

            case RABM_TC_STATE_WITH_RAB:
                Tc_TaskInit();
                /*将状态设置为RABM_NULL:*/
                RABM_SetWState(ucLoop, RABM_NULL);
                /*打印流程信息---状态切换:*/
                RABM_LOG1_NORMAL( "STATE RABM_TC_STATE_WITH_RAB state CHANGE TO RABM_NULL state: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;

            default:
                /*打印出错信息--- 该NSAPI所对应的状态出错:*/
                RABM_LOG1_ERROR( "NAS_RabmDeal3GTo2GSuspendIndMsg:ERROR:This NSAPI's state is WRONG!: NSAPI:", ucLoop + RABM_NSAPI_OFFSET );
                break;
        }
    }

}


VOS_VOID  NAS_RabmSndSnSaveDataInd(VOS_VOID)
{
    VOS_UINT32                      ulLength;
    RABM_SNDCP_SAVE_DATA_IND_MSG    *pSaveDataInd;

    ulLength     = sizeof( RABM_SNDCP_SAVE_DATA_IND_MSG ) - VOS_MSG_HEAD_LENGTH;
    pSaveDataInd = ( RABM_SNDCP_SAVE_DATA_IND_MSG *)PS_ALLOC_MSG( WUEPS_PID_RABM, ulLength );

    if ( VOS_NULL_PTR == pSaveDataInd )
    {
        /*打印出错信息---申请消息包失败:*/
        RABM_LOG_ERROR( "NAS_RabmSndSnSaveDataInd:ERROR:Allocates a message packet for RABM_SNDCP_SAVE_DATA_IND_MSG FAIL!" );

        return;
    }

    /*填写消息头:*/
    pSaveDataInd->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pSaveDataInd->ulSenderPid     = WUEPS_PID_RABM;
    pSaveDataInd->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSaveDataInd->ulReceiverPid   = UEPS_PID_SN;
    pSaveDataInd->ulLength        = ulLength;
    /*填写消息体:*/
    pSaveDataInd->usMsgType       = ID_RABM_SNDCP_SAVE_DATA_IND;
    pSaveDataInd->usPId           = 5;

    /*发送该消息:*/
    if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_RABM, pSaveDataInd ) )
    {
        /*打印警告信息---发送消息失败:*/
        RABM_LOG_WARNING( "NAS_RabmSndSnSaveDataInd:WARNING:SEND RABM_SNDCP_SAVE_DATA_IND_MSG msg FAIL!" );
    }
    else
    {
        /*打印流程信息---发送了消息:*/
        RABM_LOG_NORMAL( "NAS_RabmSndSnSaveDataInd:NORMAL:SEND RABM_SNDCP_SAVE_DATA_IND_MSG Msg" );
    }

    return;
}




VOS_VOID  NAS_RabmSndSnDataResumeInd(VOS_VOID)
{
    VOS_UINT32                      ulLength;
    RABM_SNDCP_RESUME_DATA_IND_MSG  *pResumeDataInd;

    ulLength       = sizeof( RABM_SNDCP_RESUME_DATA_IND_MSG ) - VOS_MSG_HEAD_LENGTH;
    pResumeDataInd = ( RABM_SNDCP_RESUME_DATA_IND_MSG *)PS_ALLOC_MSG( WUEPS_PID_RABM, ulLength );

    if ( VOS_NULL_PTR == pResumeDataInd )
    {
        /*打印出错信息---申请消息包失败:*/
        RABM_LOG_ERROR( "NAS_RabmSndSnDataResumeInd:ERROR:Allocates a message packet for RABM_SNDCP_RESUME_DATA_IND_MSG FAIL!" );

        return;
    }

    /*填写消息头:*/
    pResumeDataInd->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pResumeDataInd->ulSenderPid     = WUEPS_PID_RABM;
    pResumeDataInd->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pResumeDataInd->ulReceiverPid   = UEPS_PID_SN;
    pResumeDataInd->ulLength        = ulLength;
    /*填写消息体:*/
    pResumeDataInd->usMsgType       = ID_RABM_SNDCP_RESUME_DATA_IND;
    pResumeDataInd->usPId           = 5;
    /*填写NSAPI和RB的映射关系:*/
    PS_MEM_CPY( (VOS_VOID *)pResumeDataInd->astNsapiMapRb, (VOS_VOID *)gastNsapiMapRb, sizeof(SNDCP_NSAPI_MAP_RB_INFO) * SNDCP_RABM_MAX_NSAPI_NUM );

    /*发送该消息:*/
    if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_RABM, pResumeDataInd ) )
    {
        /*打印警告信息---发送消息失败:*/
        RABM_LOG_WARNING( "NAS_RabmSndSnDataResumeInd:WARNING:SEND RABM_SNDCP_RESUME_DATA_IND_MSG msg FAIL!" );
    }
    else
    {
        /*打印流程信息---发送了消息:*/
        RABM_LOG_NORMAL( "NAS_RabmSndSnDataResumeInd:NORMAL:SEND RABM_SNDCP_RESUME_DATA_IND_MSG Msg" );
    }

    return;
}
VOS_VOID  NAS_RabmSndPdcpDataResumeInd(RRRABM_RAB_IND_STRU  *pMsg)
{
    VOS_UINT8                       ucLoop;
    VOS_UINT32                      ulLoop;
    VOS_UINT8                       ucEntId;
    VOS_UINT8                       ucRabId;
    VOS_UINT32                      ulLength;
    RABM_PDCP_DATA_RESUME_IND_STRU  *pDataResumeInd;

    ulLength       = sizeof( RABM_PDCP_DATA_RESUME_IND_STRU ) - VOS_MSG_HEAD_LENGTH;
    pDataResumeInd = (RABM_PDCP_DATA_RESUME_IND_STRU *)PS_ALLOC_MSG( WUEPS_PID_RABM, ulLength );

    if ( VOS_NULL_PTR == pDataResumeInd )
    {
        /*打印出错信息---申请消息包失败:*/
        RABM_LOG_ERROR( "NAS_RabmSndPdcpDataResumeInd:ERROR:Allocates a message packet for RABM_PDCP_DATA_RESUME_IND_STRU FAIL!" );

        return;
    }

    /*填写消息头:*/
    pDataResumeInd->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pDataResumeInd->ulSenderPid     = WUEPS_PID_RABM;
    pDataResumeInd->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pDataResumeInd->ulReceiverPid   = WUEPS_PID_PDCP;
    pDataResumeInd->ulLength        = ulLength;
    pDataResumeInd->enMsgName       = ID_RABM_PDCP_DATA_RESUME_IND;

    /*初始化FLAG:*/
    for ( ucLoop = 0; ucLoop < PS_NSAPI_NUM; ucLoop++ )
    {
        /*初始化FLAG:*/
        pDataResumeInd->astNsapiMapRb[ucLoop].enValidInd = PS_FALSE;
    }

    /*填写RB信息*/
    for(ulLoop = 0; ulLoop < pMsg->ulRabCnt; ulLoop++)
    {
        if(RABM_CN_PS == pMsg->aRabInfo[ulLoop].ulCnDomainId)
        {
            ucEntId = (VOS_UINT8)((pMsg->aRabInfo[ulLoop].ulRabId & 0x0f) - RABM_NSAPI_OFFSET);
            ucRabId = (VOS_UINT8)(pMsg->aRabInfo[ulLoop].ulRabId);
            if( RABM_NSAPI_ACTIVE_NO_RAB == g_aRabmPsEnt[ucEntId].ucState )
            {
                pDataResumeInd->astNsapiMapRb[ucRabId].enValidInd = PS_TRUE;
                pDataResumeInd->astNsapiMapRb[ucRabId].ucRbId = (VOS_UINT8)(pMsg->aRabInfo[ulLoop].aulRbId[0]);
            }
        }
    }


    /*发送该消息:*/
    if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_RABM, pDataResumeInd ) )
    {
        /*打印警告信息---发送消息失败:*/
        RABM_LOG_WARNING( "NAS_RabmSndPdcpDataResumeInd:WARNING:SEND RABM_PDCP_DATA_RESUME_IND msg FAIL!" );
    }
    else
    {
        /*打印流程信息---发送了消息:*/
        RABM_LOG_NORMAL( "NAS_RabmSndPdcpDataResumeInd:NORMAL:SEND RABM_PDCP_DATA_RESUME_IND Msg" );
    }

    return;
}





VOS_VOID  NAS_RabmRcvPdcpDataResumeRsp(VOS_VOID *pMsg)
{
    /*打印流程信息--收到PDCP_RABM_DATA_RESUME_RSP_STRU消息:*/
    RABM_LOG_NORMAL("NAS_RabmRcvPdcpDataResumeRsp:NORMAL:Recv PDCP_RABM_DATA_RESUME_RSP_STRU msg");

    return;
}
VOS_UINT32  NAS_RabmSndPdcpSaveDataInd(VOS_VOID)
{
    VOS_UINT32                      ulLength;
    VOS_UINT32                      ulResult;
    RABM_PDCP_SAVE_DATA_IND_STRU    *pSaveDataInd;

    ulLength     = sizeof( RABM_PDCP_SAVE_DATA_IND_STRU ) - VOS_MSG_HEAD_LENGTH;
    pSaveDataInd = (RABM_PDCP_SAVE_DATA_IND_STRU *)PS_ALLOC_MSG( WUEPS_PID_RABM, ulLength );
    if ( VOS_NULL_PTR == pSaveDataInd )
    {
        /*打印出错信息---申请消息包失败:*/
        RABM_LOG_ERROR( "NAS_RabmSndPdcpSaveDataInd:ERROR:Allocates a message packet for RABM_PDCP_SAVE_DATA_IND_STRU msg FAIL!" );

        return RABM_FAILURE;
    }

    /*填写消息头:*/
    pSaveDataInd->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pSaveDataInd->ulSenderPid     = WUEPS_PID_RABM;
    pSaveDataInd->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSaveDataInd->ulReceiverPid   = WUEPS_PID_PDCP;
    pSaveDataInd->ulLength        = ulLength;
    pSaveDataInd->enMsgName       = ID_RABM_PDCP_SAVE_DATA_IND;

    /*发送该消息:*/
    if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_RABM, pSaveDataInd ) )
    {
        /*打印警告信息---发送消息失败:*/
        RABM_LOG_WARNING( "NAS_RabmSndPdcpSaveDataInd:WARNING:SEND RABM_PDCP_SAVE_DATA_IND_STRU msg FAIL!" );

        ulResult = RABM_SUCCESS;
    }
    else
    {
        /*打印流程信息---发送了消息:*/
        RABM_LOG_NORMAL( "NAS_RabmSndPdcpSaveDataInd:NORMAL:SEND RABM_PDCP_SAVE_DATA_IND_STRU Msg" );

        ulResult = RABM_FAILURE;
    }

    return ulResult;
}
VOS_VOID  NAS_RabmRcvPdcpSaveDataRsp(VOS_VOID *pMsg)
{
    VOS_UINT8                       ucLoop,i,ucFlag;
    PDCP_RABM_SAVE_DATA_RSP_STRU    *pSaveDataRsp;

    pSaveDataRsp = (PDCP_RABM_SAVE_DATA_RSP_STRU *)pMsg;

    if ( pSaveDataRsp->ucRbCnt > PS_WUE_MAX_PS_RB_NUM )
    {
        /*打印警告信息---消息中指示的RB个数超过PDCP_RABM_MAX_ENT_NUM:*/
        RABM_LOG_WARNING( "NAS_RabmRcvPdcpSaveDataRsp:WARNING:RB count in the msg is beyond PDCP_RABM_MAX_ENT_NUM!" );
        return;
    }

    /*初始化gPdcpRcvNpduNum全局变量:*/
    PS_MEM_SET( (VOS_VOID *)(&gPdcpRcvNpduNum), 0, sizeof(RABM_PDCP_GET_N_PDU_NUM_LIST_ST) );

    for ( ucLoop = 0; ucLoop < pSaveDataRsp->ucRbCnt; ucLoop++ )
    {
        ucFlag = VOS_FALSE;

        for ( i = 0; i < RABM_3G_PS_MAX_ENT_NUM; i++ )
        {
            if ( g_aRabmPsEnt[i].RabInfo.ucRbNum != 0 )
            {
                if ( g_aRabmPsEnt[i].RabInfo.aucRbId[0]
                     == pSaveDataRsp->astPdcpNumInfo[ucLoop].ucRbId )
                {
                    gPdcpRcvNpduNum.aucNPDUNumList[i + RABM_2G_NSAPI_OFFSET][0] = 1;
                    gPdcpRcvNpduNum.aucNPDUNumList[i + RABM_2G_NSAPI_OFFSET][1] = pSaveDataRsp->astPdcpNumInfo[ucLoop].ucRcvNPduNum;

                    ucFlag = VOS_TRUE;
                    break;
                }
            }
        }

        if ( VOS_TRUE != ucFlag )
        {
            /*打印警告信息---消息中指示的RB未找到对应的NSAPI:*/
            RABM_LOG1_WARNING( "NAS_RabmRcvPdcpSaveDataRsp:WARNING:Can't find NSAPI match to RbId in the msg ! RbId = ",
                               pSaveDataRsp->astPdcpNumInfo[ucLoop].ucRbId );
        }
    }

    return;
}
VOS_VOID  NAS_RabmSaveNsapiMapRbInfo(VOS_VOID)
{
    VOS_UINT8       ucLoop;

    for  ( ucLoop = 0; ucLoop < SNDCP_RABM_MAX_NSAPI_NUM; ucLoop++ )
    {
        if ( ucLoop < RABM_MIN_NSAPI_NUMBER )
        {
            gastNsapiMapRb[ucLoop].ucFlag = PS_FALSE;
            continue;
        }


        if ( RABM_NSAPI_ACTIVE_WITH_RAB == g_aRabmPsEnt[ucLoop - RABM_2G_NSAPI_OFFSET].ucState )
        {
            gastNsapiMapRb[ucLoop].ucFlag = PS_TRUE;
            gastNsapiMapRb[ucLoop].ucRbId = g_aRabmPsEnt[ucLoop - RABM_2G_NSAPI_OFFSET].RabInfo.aucRbId[0];
        }
        else
        {
            gastNsapiMapRb[ucLoop].ucFlag = PS_FALSE;
        }

    }

    return;
}


VOS_VOID NAS_RABM_RcvGmmFastDormMmlProcStatusQryCnf(
    struct MsgCB                       *pstMsg
)
{
    GMM_RABM_MML_PROC_STATUS_QRY_CNF_STRU    *pstProcStatus;

    pstProcStatus = (GMM_RABM_MML_PROC_STATUS_QRY_CNF_STRU*)pstMsg;

    NAS_RabmStopTimer( RABM_TIMER_NAME_COMMON, RABM_TIMER_FASTDORM_WAIT_GMM_QRY_RESULT );

    /*if (状态在NAS_RABM_FASTDORM_WAIT_GMM_QRY_RESULT) */
    if (NAS_RABM_FASTDORM_WAIT_GMM_QRY_RESULT == NAS_RABM_GetCurrFastDormStatus())
    {
        /*if (当前可以给WAS发送FAST DORM)*/
        if ( (VOS_TRUE != pstProcStatus->stMmlProcStatus.bitOpGmmSpecificProcedure)
          && (VOS_TRUE != pstProcStatus->stMmlProcStatus.bitOpPsSmsService)
          && (VOS_TRUE != pstProcStatus->stMmlProcStatus.bitOpCsSignal) )
        {
            /* 向WAS发送RRRABM_FASTDORM_START_REQ */
            NAS_RABM_SndWasFastDormStartReq(RABM_FASTDORM_ENUM);
            /* 设置为NAS_RABM_FASTDORM_RUNNING状态 */
            NAS_RABM_SetCurrFastDormStatus(NAS_RABM_FASTDORM_RUNNING);
        }
        else
        {
            /* 设置为NAS_RABM_FASTDORM_RETRY状态 */
            NAS_RABM_SetCurrFastDormStatus(NAS_RABM_FASTDORM_RETRY);
            /* 启动定时器RABM_TIMER_FASTDORM_RETRY */
            NAS_RabmStartTimer( RABM_TIMER_NAME_FD_RETRY, RABM_TIMER_FASTDORM_RETRY );
        }
    }
    NAS_RABM_SndOmFastdormStatus();
}
VOS_VOID NAS_RABM_RcvGmmMmlProcStatusQryCnf(
    struct MsgCB                       *pstMsg
)
{
    GMM_RABM_MML_PROC_STATUS_QRY_CNF_STRU    *pstProcStatus;

    pstProcStatus = (GMM_RABM_MML_PROC_STATUS_QRY_CNF_STRU*)pstMsg;

    if (RABM_RELEASE_RRC_ENUM == pstProcStatus->stMmlProcStatus.enOptFlag)
    {
        NAS_RABM_RcvGmmReleaseRrcMmlProcStatusQryCnf(pstMsg);
    }

    if (RABM_FASTDORM_ENUM == pstProcStatus->stMmlProcStatus.enOptFlag)
    {
        NAS_RABM_RcvGmmFastDormMmlProcStatusQryCnf(pstMsg);
    }

    return;
}


VOS_VOID NAS_RABM_RcvGmmReleaseRrcMmlProcStatusQryCnf(
    struct MsgCB                       *pstMsg
)
{
    GMM_RABM_MML_PROC_STATUS_QRY_CNF_STRU    *pstProcStatus;

    pstProcStatus = (GMM_RABM_MML_PROC_STATUS_QRY_CNF_STRU*)pstMsg;

    if ( (VOS_TRUE != pstProcStatus->stMmlProcStatus.bitOpGmmSpecificProcedure)
      && (VOS_TRUE != pstProcStatus->stMmlProcStatus.bitOpPsSmsService)
      && (VOS_TRUE != pstProcStatus->stMmlProcStatus.bitOpCsSignal) )
    {
        /* 向WAS发送RRRABM_FASTDORM_START_REQ */
        NAS_RABM_SndWasFastDormStartReq(RABM_RELEASE_RRC_ENUM);
    }

    return;
}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif
