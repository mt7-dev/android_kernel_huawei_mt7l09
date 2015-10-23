

/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "Rabm2GInclude.h"


#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_RABM_MSGDISPATCH_C

/*****************************************************************************
   2 全局变量定义
*****************************************************************************/

/*****************************************************************************
   3 函数实现
*****************************************************************************/


VOS_VOID  NAS_Rabm2GMsgDispatch(struct MsgCB  *pMsg )
{
    MSG_HEADER_STRU                     *pstHeader;   /*定义消息头指针*/

    if ( VOS_PID_TIMER == pMsg->ulSenderPid )                        /*收到定时器消息*/
    {
        NAS_RabmTimerExpiredMsgDispatch( ( REL_TIMER_MSG * ) pMsg );
        return;
    }

    pstHeader = ( MSG_HEADER_STRU * )pMsg;                       /*获取消息头指针*/

    switch ( pstHeader->ulSenderPid )                            /*根据SenderPid分发消息*/
    {
        case   UEPS_PID_SN:                                      /*收到SNDCP模块发送过来的消息*/
            if ( ID_SN_RABM_SEQUENCE_RSP == pstHeader->ulMsgName )
            {
                NAS_RabmSnSequenceRspMsg( ( VOS_VOID *)pMsg );
            }
            else if ( ID_RABM_SNDCP_SAVE_DATA_RSP == pstHeader->ulMsgName )
            {
                NAS_RabmSnSaveDataRspMsg();
            }
            else if ( ID_RABM_SNDCP_RESUME_DATA_RSP == pstHeader->ulMsgName )
            {
                NAS_RabmSnResumeDataRspMsg();
            }
            else
            {
                /*打印警告信息---MsgName出错:*/
                RABM_LOG_WARNING( "NAS_Rabm2GMsgDispatch:WARNING:The Msg-name of the Msg from UEPS_PID_SN is WRONG!" );
                return;
            }
            break;
        case   WUEPS_PID_TAF:
            if( TAFRABM_PS_DATA_REQ == pstHeader->ulMsgName )
            {
                NAS_RabmRcvTafGprsPsDataReq((TAFRABM_PS_DATA_REQ_STRU *)pMsg);
            }
            else if ( RABM_APS_SET_TRANSMODE_MSG_TYPE == ( ( RABM_APS_SET_TRANSMODE_MSG * )pMsg )->usMsgType )
            {
                NAS_RabmSetTransMode( &( ( ( RABM_APS_SET_TRANSMODE_MSG * )pMsg )->RabmApsSetTransMode ) );
            }
            else
            {
                /*打印警告信息---MsgName出错:*/
                RABM_LOG_WARNING( "NAS_Rabm2GMsgDispatch:WARNING:The Msg-name of the Msg from WUEPS_PID_TAF is WRONG!" );
                return;
            }
            break;
        case   WUEPS_PID_GMM:                                    /*收到GMM模块发送过来的消息*/
            switch ( pstHeader->ulMsgName )
            {
                case ID_GMM_RABM_ROUTING_AREA_UPDATE_IND:
                    NAS_RabmGmmRoutingAreaUpdateIndMsg( ( VOS_VOID * )pMsg );
                    break;
                case ID_GMM_RABM_REESTABLISH_CNF:                    /*收到3G的消息*/
                    /*打印警告信息---收到不合逻辑的消息:*/
                    RABM_LOG_WARNING( "NAS_Rabm2GMsgDispatch:WARNING:RECEIVE ID_GMM_RABM_REESTABLISH_CNF Msg is ILLOGICAL in 2G mode!" );
                    return;
                default:                                         /*收到消息名称出错的消息*/
                    /*打印警告信息---MsgName出错:*/
                    RABM_LOG_WARNING( "NAS_Rabm2GMsgDispatch:WARNING:The Msg-name of the Msg from WUEPS_PID_GMM is WRONG!" );
                    return;
            }
            break;
        case   WUEPS_PID_WRR:
            if ( RRRABM_HANDOVER_RAB_IND == pstHeader->ulMsgName )
            {
                NAS_RabmDealHandoverRabInd( (RRRABM_HANDOVER_RAB_IND_STRU *)pMsg );
            }
            else
            {
                /*打印警告信息---MsgName出错:*/
                RABM_LOG_WARNING( "NAS_Rabm2GMsgDispatch:WARNING:The Msg-name of the Msg from WUEPS_PID_WRR is WRONG!" );
                return;
            }
            break;
        case   WUEPS_PID_PDCP:
            /*打印警告信息---收到不合逻辑的消息:*/
            RABM_LOG_WARNING( "NAS_Rabm2GMsgDispatch:WARNING:RECEIVE Msg from WUEPS_PID_PDCP is ILLOGICAL in 2G mode!" );
            return;
        case   WUEPS_PID_RLC:
            /*打印警告信息---收到不合逻辑的消息:*/
            RABM_LOG_WARNING( "NAS_Rabm2GMsgDispatch:WARNING:RECEIVE Msg from WUEPS_PID_RLC is ILLOGICAL in 2G mode!" );
            return;
        default:                                                 /*收到消息的发送Pid出错的消息*/
            /*打印警告信息---SendPid出错:*/
            RABM_LOG_WARNING( "NAS_Rabm2GMsgDispatch:WARNING:The SendPid of the receive Msg is WRONG!" );
            return;
    }
}


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif
