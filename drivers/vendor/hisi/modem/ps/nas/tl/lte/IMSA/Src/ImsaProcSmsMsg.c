/******************************************************************************

   Copyright(C)2013,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : ImsaProcSmsMsg.c
  Description     : 该C文件实现Sms消息处理和Sms消息发送
  History           :
     1.sunbing 49683      2013-06-19  Draft Enact

******************************************************************************/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "PsTypeDef.h"
#include "ImsaProcSmsMsg.h"
#include "ImsaPublic.h"
#include "ImsaImsInterface.h"
#include "ImsaProcSmsMsg.h"
#include "ImsaImsAdaption.h"
#include "MnMsgApi.h"
/*lint -e767*/
#define    THIS_FILE_ID      PS_FILE_ID_IMSAPROCSMSMSG_C
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

VOS_UINT8   gucImsaSmrMR = 0;

extern VOS_VOID IMSA_SMS_SndMtErrInd(VOS_UINT32 ulErrCode);
extern VOS_VOID IMSA_SMS_GetSmscFromRpData(const VOS_UINT8* pucRpData, VOS_CHAR* pcSmsc);
extern VOS_VOID IMSA_ProcSmsMsgRpDataReq(const MSG_IMSA_RP_DATA_REQ_STRU *pstRpDataReq);
extern VOS_VOID IMSA_ProcSmsMsgReportReq(const MSG_IMSA_REPORT_REQ_STRU *pstReportReq);
extern VOS_VOID IMSA_ProcSmsMsgSmmaReq(const MSG_IMSA_SMMA_REQ_STRU *pstSmmaReq);
extern VOS_VOID IMSA_SMS_SndCancelImsMoSms(VOS_VOID);
/*****************************************************************************
  3 Function
*****************************************************************************/

/*lint -e960*/
/*lint -e961*/
/*****************************************************************************
 Function Name  : IMSA_ProcSmsMsgRpDataReq()
 Description    : MO SMS消息处理函数
 Input          : MSG_IMSA_RP_DATA_REQ_STRU *pstRpDataReq
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-08-08  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcSmsMsgRpDataReq(const MSG_IMSA_RP_DATA_REQ_STRU *pstRpDataReq)
{
    IMSA_SMS_SMR_MO_STRU                *pstSmrMoEntity;
    VOS_UINT8                            aucTmpData[255];
    VOS_CHAR                             acSmsc[IMSA_IMS_ALPHA_STRING_SZ+1];

    pstSmrMoEntity = IMSA_SMS_GetSmrMoEntityAddress();

    if(IMSA_SMS_SMR_STATE_IDLE == pstSmrMoEntity->enState)
    {
        if((pstRpDataReq->ulDataLen == 0)||(pstRpDataReq->ulDataLen >= 255))
        {
            IMSA_WARN_LOG("IMSA_ProcSmsMsgRpDataReq:Data Lenth is ERR! ");
            return;
        }

        /*拷贝短信内容，MSG的数据aucData，第一个字节是短信发送域，IMS忽略，
          数据长度是不包含这个发送域数据的*/
        IMSA_MEM_SET(aucTmpData, 0, 255);
        IMSA_MEM_CPY(aucTmpData,&pstRpDataReq->aucData[1],\
                     pstRpDataReq->ulDataLen);

        pstSmrMoEntity->ucMessageReference = ++gucImsaSmrMR;    /*记录RP-MR*/
        aucTmpData[1] = pstSmrMoEntity->ucMessageReference;

        /*转状态，启动TI_IMSA_SMS_TR1M保护定时器*/
        pstSmrMoEntity->enState = IMSA_SMS_SMR_STATE_WAIT_FOR_RP_ACK;
        IMSA_StartTimer(&pstSmrMoEntity->stTR1MTimer);

        IMSA_INFO_LOG1("IMSA_ProcSmsMsgRpDataReq: Snd SMR MR = ", pstSmrMoEntity->ucMessageReference);

        /*获取短信中心地址*/
        IMSA_MEM_SET(acSmsc, 0, IMSA_IMS_ALPHA_STRING_SZ+1);
        IMSA_SMS_GetSmscFromRpData(aucTmpData, acSmsc);

        IMSA_SMS_SndImsSmsMsg(aucTmpData,pstRpDataReq->ulDataLen,acSmsc);

    }
    else
    {
        /* modify by jiqiang 2014.03.27 pclint fix error 56 begin */
        /*lint -e56*/
        IMSA_SMS_SndMsgReportInd((SMR_SMT_ERROR_IMSA_ERROR_BEGIN + MN_MSG_IMSA_CAUSE_SMS_OPERATING), VOS_NULL_PTR, 0);
        /*lint +e56*/
        /* modify by jiqiang 2014.03.27 pclint fix error 56 end */

        /*状态错误，上报错误*/
        IMSA_WARN_LOG("IMSA_ProcSmsMsgRpDataReq: Smr Mo State is IMSA_SMS_SMR_STATE_WAIT_FOR_RP_ACK!");
    }
}


/*****************************************************************************
 Function Name  : IMSA_ProcSmsMsgReportReq()
 Description    : SMS Report消息处理函数
 Input          : MSG_IMSA_REPORT_REQ_STRU *pstReportReq
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-08-08  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcSmsMsgReportReq(const MSG_IMSA_REPORT_REQ_STRU *pstReportReq)
{
    IMSA_SMS_SMR_MT_STRU                *pstSmrMtEntity;
    VOS_UINT8                            aucTmpData[255];
    VOS_UINT32                           ulLength;

    pstSmrMtEntity = IMSA_SMS_GetSmrMtEntityAddress();

    if(IMSA_SMS_SMR_STATE_WAIT_TO_SND_RP_ACK == pstSmrMtEntity->enState)
    {
        /*状态转换为空闲态*/
        pstSmrMtEntity->enState = IMSA_SMS_SMR_STATE_IDLE;

        /*停止TI_IMSA_SMS_TR2M定时器*/
        IMSA_StopTimer(&pstSmrMtEntity->stTimerInfo);

        /* 没有User Data 的 rp_ack */
        if(pstReportReq->ulDataLen <= 2)
        {
            /* 消息长度                                 */
            ulLength = 2;

            /* 消息类型                                 */
            aucTmpData[0] = IMSA_SMS_MTN_DATA_TYPE_RP_ACK;

            /* MR                                       */
            aucTmpData[1] = pstSmrMtEntity->ucMessageReference;
        }
        else
        {
            /*拷贝短信内容*/
            IMSA_MEM_SET(aucTmpData, 0, 255);

            ulLength = pstReportReq->ulDataLen;

            /*MSG的数据aucData，report数据不带域信息*/
            IMSA_MEM_CPY(aucTmpData,&pstReportReq->aucData[0],\
                         pstReportReq->ulDataLen);

            /* MR                                       */
            aucTmpData[1] = pstSmrMtEntity->ucMessageReference;
        }

        IMSA_INFO_LOG1("IMSA_ProcSmsMsgReportReq: Snd SMR MR = ", pstSmrMtEntity->ucMessageReference);

        /*消息发给IMS协议栈*/
        IMSA_SMS_SndImsSmsMsg(aucTmpData, ulLength,VOS_NULL_PTR);
    }
    else
    {
        /*状态错误，上报错误*/
        IMSA_WARN_LOG("IMSA_ProcSmsMsgReportReq: Smr Mt State is IMSA_SMS_SMR_STATE_WAIT_TO_SND_RP_ACK!");
    }

}

/*****************************************************************************
 Function Name  : IMSA_ProcSmsMsgSmmaReg()
 Description    : SMS Smma消息处理函数
 Input          : MSG_IMSA_SMMA_REQ_STRU *pstSmmaReq
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-08-08  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcSmsMsgSmmaReq(const MSG_IMSA_SMMA_REQ_STRU *pstSmmaReq)
{
    IMSA_SMS_SMR_MO_STRU                *pstSmrMoEntity;
    VOS_UINT8                            aucTmpData[2];

    pstSmrMoEntity = IMSA_SMS_GetSmrMoEntityAddress();

    switch(pstSmrMoEntity->enState)
    {

    case IMSA_SMS_SMR_STATE_IDLE:

        if (SMT_SMR_ABORT_FLG_NO_EXIST == pstSmmaReq->ucAbortFlg )
        {
            pstSmrMoEntity->ucMemAvailFlg      = IMSA_SMS_TRUE;
            pstSmrMoEntity->ucRetransFlg       = IMSA_SMR_RETRANS_PERMIT;
            pstSmrMoEntity->ucMessageReference = ++gucImsaSmrMR;

            /* 填写消息类型 */
            aucTmpData[0] = IMSA_SMS_MTN_DATA_TYPE_RP_SMMA;
            aucTmpData[1] = pstSmrMoEntity->ucMessageReference;

            /*转状态，启动TI_IMSA_SMS_TR1M保护定时器*/
            pstSmrMoEntity->enState = IMSA_SMS_SMR_STATE_WAIT_FOR_RP_ACK;
            IMSA_StartTimer(&pstSmrMoEntity->stTR1MTimer);

            IMSA_INFO_LOG1("IMSA_ProcSmsMsgSmmaReq: Snd SMR MR = ", pstSmrMoEntity->ucMessageReference);

            IMSA_SMS_SndImsSmsMsg(aucTmpData, 2,VOS_NULL_PTR);
        }
        break;
    case IMSA_SMS_SMR_STATE_WAIT_FOR_RETRANS_TIMER:

        if (SMT_SMR_ABORT_FLG_EXIST == pstSmmaReq->ucAbortFlg )
        {
            /*转状态，停止TI_IMSA_SMS_TRAM保护定时器*/
            pstSmrMoEntity->enState = IMSA_SMS_SMR_STATE_IDLE;
            IMSA_StopTimer(&pstSmrMoEntity->stTRAMTimer);

            IMSA_SMS_SndMsgReportInd(SMR_SMT_ERROR_USER_ABORT, VOS_NULL_PTR, 0);

            pstSmrMoEntity->ucMemAvailFlg      = IMSA_SMS_FALSE;
            pstSmrMoEntity->ucRetransFlg       = IMSA_SMR_RETRANS_PERMIT;
        }
        break;

    default:

        break;
    }




}

/*****************************************************************************
 Function Name  : IMSA_SMS_SndCancelImsMoSms()
 Description    : 发送取消短信发送消息
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-09-06  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SMS_SndCancelImsMoSms(VOS_VOID)
{
    IMSA_IMS_INPUT_EVENT_STRU                    *pstImsaImsInputEvent;

    /*把短信发给IMS协议栈*/
    pstImsaImsInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if(VOS_NULL_PTR == pstImsaImsInputEvent)
    {
        IMSA_ERR_LOG("IMSA_SMS_SndCancelImsMoSms:Alloc mem Err! ");
        return;
    }

    IMSA_MEM_SET(&pstImsaImsInputEvent->evt.stInputSmsEvent,0,sizeof(IMSA_IMS_INPUT_SMS_EVENT_STRU));

    pstImsaImsInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_SMS;
    pstImsaImsInputEvent->evt.stInputSmsEvent.enInputSmsReason = IMSA_IMS_INPUT_SMS_REASON_TR1M_EXP;
    pstImsaImsInputEvent->evt.stInputSmsEvent.ulOpId = IMSA_AllocImsOpId();

    IMSA_SndImsMsgSmsEvent(pstImsaImsInputEvent);

    IMSA_MEM_FREE(pstImsaImsInputEvent);

}


/*****************************************************************************
 Function Name  : IMSA_SMS_SndImsSmsMsg()
 Description    : 发送短信码流
 Input          : pucData 短信数据，
                  ulLength 短信长度
                  pcSmsc 短信中心
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-08-08  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SMS_SndImsSmsMsg(const VOS_UINT8 *pucData,
                                            VOS_UINT32 ulLength,
                                            VOS_CHAR *pcSmsc)
{
    IMSA_IMS_INPUT_EVENT_STRU                    *pstImsaImsInputEvent;

    /*把短信发给IMS协议栈*/
    pstImsaImsInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if(VOS_NULL_PTR == pstImsaImsInputEvent)
    {
        IMSA_ERR_LOG("IMSA_ProcSmsMsgRpDataReq:Alloc mem Err! ");
        return;
    }

    IMSA_MEM_SET(pstImsaImsInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    pstImsaImsInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_SMS;
    pstImsaImsInputEvent->evt.stInputSmsEvent.enInputSmsReason = IMSA_IMS_INPUT_SMS_REASON_SEND_MESSAGE;
    pstImsaImsInputEvent->evt.stInputSmsEvent.ulOpId = IMSA_AllocImsOpId();

    /*短信内容*/
    pstImsaImsInputEvent->evt.stInputSmsEvent.bitOpPdu = IMSA_OP_TRUE;
    pstImsaImsInputEvent->evt.stInputSmsEvent.ucPduLen = (VOS_UINT8)ulLength;
    IMSA_MEM_SET(pstImsaImsInputEvent->evt.stInputSmsEvent.acPdu, 0, ulLength+1);
    IMSA_MEM_CPY(pstImsaImsInputEvent->evt.stInputSmsEvent.acPdu, pucData, ulLength);

    /*短信中心地址不为空，需要携带给IMS协议栈*/
    if(VOS_NULL_PTR != pcSmsc)
    {
        pstImsaImsInputEvent->evt.stInputSmsEvent.bitOpSmsc = IMSA_OP_TRUE;
        (VOS_VOID)VOS_StrNCpy(pstImsaImsInputEvent->evt.stInputSmsEvent.acSmsc, pcSmsc, IMSA_IMS_ALPHA_STRING_SZ);
    }
    else
    {
        pstImsaImsInputEvent->evt.stInputSmsEvent.bitOpSmsc = 0;
    }

    IMSA_SndImsMsgSmsEvent(pstImsaImsInputEvent);

    IMSA_MEM_FREE(pstImsaImsInputEvent);

}

/*****************************************************************************
 Function Name  : IMSA_SMS_GetSmscFromRpData()
 Description    : 从短信码流中获取短信中心地址
 Input          :
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-09-12  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SMS_GetSmscFromRpData(const VOS_UINT8* pucRpData, VOS_CHAR* pcSmsc)
{
    VOS_UINT8             ucSmscLength;
    VOS_UINT8             ucCallTon;

    if((VOS_NULL_PTR == pucRpData)||(VOS_NULL_PTR == pcSmsc))
        return;

    /*短信中心的长度在RP DATA的第4个字节(包含了类型的长度，一个字节)，
      第6个字节开始是BCD格式的号码*/
    ucSmscLength = pucRpData[3]-1;

    /*长度超过短信中心存放空间，则退出，避免溢出*/
    if(ucSmscLength > IMSA_IMS_ALPHA_STRING_SZ/2)
    {
        IMSA_WARN_LOG("IMSA_SMS_GetSmscFromRpData:Data Lenth is ERR! ");
        return;
    }

    /*RP DATA第5个字节是编码类型，如果是0x91，说明号码带+号，如果是0x81，不带加号*/
    /*24.008 10.5.4.9,Type of Number,5,6,7bit of the MN_CALL_NUM_TYPE_ENUM_U8*/
    ucCallTon = (pucRpData[4]>>4)&0x7;

    /*国际号码，号码中有"+"*/
    if(ucCallTon == MN_CALL_TON_INTERNATIONAL)
    {
        /*给IMS的短信中心号码，加上‘+’*/
        pcSmsc[0] = '+';
        (VOS_VOID)IMSA_UtilBcdNumberToAscii(&pucRpData[5], ucSmscLength, &pcSmsc[1]);
    }
    else
    {
        (VOS_VOID)IMSA_UtilBcdNumberToAscii(&pucRpData[5], ucSmscLength, pcSmsc);
    }
}
VOS_VOID IMSA_SMS_MemNotifyRetrans( VOS_UINT32 ulErrCode )
{
    IMSA_SMS_SMR_MO_STRU                *pstSmrMoEntity;

    IMSA_INFO_LOG("IMSA_SMS_MemNotifyRetrans: enter.");

    pstSmrMoEntity = IMSA_SMS_GetSmrMoEntityAddress();

    if ( IMSA_SMR_RETRANS_PERMIT != pstSmrMoEntity->ucRetransFlg )
    {                                                                           /* RETRANS FLAG 置位                        */
        IMSA_SMS_SndMsgReportInd(ulErrCode, VOS_NULL_PTR, 0);                   /* 发送SMRL_REPORT_IND                      */
        pstSmrMoEntity->ucRetransFlg  = IMSA_SMR_RETRANS_PERMIT;                /* 将RETRANS FLAG置0清除                    */
        pstSmrMoEntity->ucMemAvailFlg = IMSA_SMS_FALSE;                         /* 清除标志位                               */
        pstSmrMoEntity->enState = IMSA_SMS_SMR_STATE_IDLE;                      /* 状态迁移到空闲状态                       */
    }
    else
    {
        /*清除前面的消息*/
        pstSmrMoEntity->ucRetransFlg = IMSA_SMR_RETRANS_NO_PERMIT;              /* 将RETRANS FLAG置0清除                    */

        pstSmrMoEntity->enState      = IMSA_SMS_SMR_STATE_WAIT_FOR_RETRANS_TIMER;/* 状态迁移                                 */
        IMSA_StartTimer(&pstSmrMoEntity->stTRAMTimer);
    }
}

/*****************************************************************************
 Function Name  : IMSA_SMS_ProcTimerMsgTr1m()
 Description    : Tr2m定时器超时处理函数
 Input          : VOS_VOID *pTimerMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-08-09  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SMS_ProcTimerMsgTr1m(const VOS_VOID *pTimerMsg)
{
    IMSA_SMS_SMR_MO_STRU                *pstSmrMoEntity;

    (VOS_VOID)pTimerMsg;

    IMSA_INFO_LOG("IMSA_SMS_ProcTimerMsgTr1m: Tr1M expired.");

    pstSmrMoEntity = IMSA_SMS_GetSmrMoEntityAddress();

    if(IMSA_SMS_SMR_STATE_WAIT_FOR_RP_ACK == pstSmrMoEntity->enState)
    {
        if (IMSA_SMS_TRUE == pstSmrMoEntity->ucMemAvailFlg )
        {
            if ( IMSA_SMR_RETRANS_PERMIT != pstSmrMoEntity->ucRetransFlg )
            {
                /*TR1M超时，通知IMS协议栈取消发送*/
                IMSA_SMS_SndCancelImsMoSms();
            }
            IMSA_SMS_MemNotifyRetrans(SMR_SMT_ERROR_TR1M_TIMEOUT);         /* 调用重发过程的处理                       */
        }
        else
        {
            /*TR1M超时，通知IMS协议栈取消发送*/
            IMSA_SMS_SndCancelImsMoSms();

            /*给上层MSG上报错误指示*/
            IMSA_SMS_SndMsgReportInd(SMR_SMT_ERROR_TR1M_TIMEOUT, VOS_NULL_PTR, 0);

            /*MO实体进入空闲态*/
            pstSmrMoEntity->enState = IMSA_SMS_SMR_STATE_IDLE;
        }
    }
}

/*****************************************************************************
 Function Name  : IMSA_SMS_ProcTimerMsgTr2m()
 Description    : Tr2m定时器超时处理函数
 Input          : VOS_VOID *pTimerMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-08-09  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SMS_ProcTimerMsgTr2m(const VOS_VOID *pTimerMsg)
{
    IMSA_SMS_SMR_MT_STRU                *pstSmrMtEntity;

    (VOS_VOID)pTimerMsg;

    IMSA_INFO_LOG("IMSA_SMS_ProcTimerMsgTr2m: Tr2M expired.");

    pstSmrMtEntity = IMSA_SMS_GetSmrMtEntityAddress();

    if(IMSA_SMS_SMR_STATE_WAIT_TO_SND_RP_ACK == pstSmrMtEntity->enState)
    {
        /*TR2M超时，无需向网络发送指示*/

        /*给上层MSG上报错误指示*/
        IMSA_SMS_SndMtErrInd(SMR_SMT_ERROR_TR2M_TIMEOUT);

        /*MO实体进入空闲态*/
        pstSmrMtEntity->enState = IMSA_SMS_SMR_STATE_IDLE;

    }
}
VOS_VOID IMSA_SMS_ProcTimerMsgTram(const VOS_VOID *pTimerMsg)
{
    IMSA_SMS_SMR_MO_STRU                *pstSmrMoEntity;
    VOS_UINT8                            aucTmpData[255];

    (VOS_VOID)pTimerMsg;

    IMSA_INFO_LOG("IMSA_SMS_ProcTimerMsgTram: TrAM expired.");

    pstSmrMoEntity = IMSA_SMS_GetSmrMoEntityAddress();

    if(IMSA_SMS_SMR_STATE_WAIT_FOR_RETRANS_TIMER == pstSmrMoEntity->enState)
    {
        pstSmrMoEntity->ucRetransFlg       = IMSA_SMR_RETRANS_PROCESS;

        pstSmrMoEntity->ucMessageReference = ++gucImsaSmrMR;    /*记录RP-MR*/

        /* 填写消息类型 */
        aucTmpData[0] = IMSA_SMS_MTN_DATA_TYPE_RP_SMMA;
        aucTmpData[1] = pstSmrMoEntity->ucMessageReference;

        /*转状态，启动TI_IMSA_SMS_TR1M保护定时器*/
        pstSmrMoEntity->enState = IMSA_SMS_SMR_STATE_WAIT_FOR_RP_ACK;
        IMSA_StartTimer(&pstSmrMoEntity->stTR1MTimer);

        IMSA_INFO_LOG1("IMSA_SMS_ProcTimerMsgTram: Snd SMR MR = ", pstSmrMoEntity->ucMessageReference);

        IMSA_SMS_SndImsSmsMsg(aucTmpData, 2,VOS_NULL_PTR);


    }
}


/*****************************************************************************
 Function Name  : IMSA_ProcSmsMsg()
 Description    : MSG消息处理函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-08-08  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcSmsMsg(const VOS_VOID *pRcvMsg)
{
    /* 定义消息头指针*/
    PS_MSG_HEADER_STRU          *pHeader = VOS_NULL_PTR;

    /* 获取消息头指针*/
    pHeader = (PS_MSG_HEADER_STRU *) pRcvMsg;

    switch(pHeader->ulMsgName)
    {
        case ID_MSG_IMSA_RP_DATA_REQ:
            IMSA_ProcSmsMsgRpDataReq((MSG_IMSA_RP_DATA_REQ_STRU*)pRcvMsg);
            break;

        case ID_MSG_IMSA_REPORT_REQ:
            IMSA_ProcSmsMsgReportReq((MSG_IMSA_REPORT_REQ_STRU*)pRcvMsg);
            break;

        case ID_MSG_IMSA_SMMA_REQ:
            IMSA_ProcSmsMsgSmmaReq((MSG_IMSA_SMMA_REQ_STRU*)pRcvMsg);
            break;

        default:
            break;
    }

}

/*****************************************************************************
 Function Name  : IMSA_SMS_SndMtErrInd()
 Description    : 发送MT err ind消息
 Input          : ucRpStatus，ucRpCause
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-08-09  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SMS_SndMtErrInd(VOS_UINT32 ulErrCode)
{
    IMSA_MSG_MT_ERR_IND_STRU    *pstMtErrInd;

    pstMtErrInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_MSG_MT_ERR_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstMtErrInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SMS_SndMtErrInd:ERROR:Alloc Msg fail!");
        return ;
    }

    IMSA_WRITE_MSG_MSG_HEAD(pstMtErrInd,ID_IMSA_MSG_MT_ERR_IND);

    pstMtErrInd->enErrorCode = ulErrCode;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstMtErrInd);


}


/*****************************************************************************
 Function Name  : IMSA_SMS_Init()
 Description    : 短信模块初始化函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-08-09  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SMS_Init(VOS_VOID)
{
    IMSA_SMS_SMR_MO_STRU                *pstSmrMoEntity;
    IMSA_SMS_SMR_MT_STRU                *pstSmrMtEntity;

    /*初始化变量*/
    pstSmrMoEntity = IMSA_SMS_GetSmrMoEntityAddress();
    IMSA_MEM_SET(pstSmrMoEntity, 0, sizeof(IMSA_SMS_SMR_MO_STRU));
    pstSmrMoEntity->enState = IMSA_SMS_SMR_STATE_IDLE;

    pstSmrMtEntity = IMSA_SMS_GetSmrMtEntityAddress();
    IMSA_MEM_SET(pstSmrMtEntity, 0, sizeof(IMSA_SMS_SMR_MT_STRU));
    pstSmrMtEntity->enState = IMSA_SMS_SMR_STATE_IDLE;

    /*设置定时器长度，名称等*/
    pstSmrMoEntity->stTR1MTimer.usName = TI_IMSA_SMS_TR1M;
    pstSmrMoEntity->stTR1MTimer.ulTimerLen = IMSA_SMS_TIMER_TR1M_LENGTH;
    pstSmrMoEntity->stTR1MTimer.ucMode = 0;

    pstSmrMoEntity->stTRAMTimer.usName = TI_IMSA_SMS_TRAM;
    pstSmrMoEntity->stTRAMTimer.ulTimerLen = IMSA_SMS_TIMER_TRAM_LENGTH;
    pstSmrMoEntity->stTRAMTimer.ucMode = 0;

    pstSmrMtEntity->stTimerInfo.usName = TI_IMSA_SMS_TR2M;
    pstSmrMtEntity->stTimerInfo.ulTimerLen = IMSA_SMS_TIMER_TR2M_LENGTH;
    pstSmrMtEntity->stTimerInfo.ucMode = 0;

}



VOS_VOID IMSA_SMS_ClearResource(VOS_VOID)
{
    IMSA_SMS_SMR_MO_STRU                *pstSmrMoEntity;
    IMSA_SMS_SMR_MT_STRU                *pstSmrMtEntity;

    pstSmrMoEntity = IMSA_SMS_GetSmrMoEntityAddress();
    pstSmrMtEntity = IMSA_SMS_GetSmrMtEntityAddress();

    /*停止定时器*/
    if (VOS_TRUE == IMSA_IsTimerRunning(&pstSmrMoEntity->stTR1MTimer))
    {
        IMSA_StopTimer(&pstSmrMoEntity->stTR1MTimer);

        /* modify by jiqiang 2014.03.27 pclint fix error 56 begin */
        /*lint -e56*/
        IMSA_SMS_SndMsgReportInd(SMR_SMT_ERROR_IMSA_ERROR_BEGIN+MN_MSG_IMSA_CAUSE_NO_SERVICE, VOS_NULL_PTR, 0);
        /*lint +e56*/
        /* modify by jiqiang 2014.03.27 pclint fix error 56 end */
    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstSmrMoEntity->stTRAMTimer))
    {
        IMSA_StopTimer(&pstSmrMoEntity->stTRAMTimer);

        /* modify by jiqiang 2014.03.27 pclint fix error 56 begin */
        /*lint -e56*/
        IMSA_SMS_SndMsgReportInd(SMR_SMT_ERROR_IMSA_ERROR_BEGIN+MN_MSG_IMSA_CAUSE_NO_SERVICE, VOS_NULL_PTR, 0);
        /*lint +e56*/
        /* modify by jiqiang 2014.03.27 pclint fix error 56 end */
    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstSmrMtEntity->stTimerInfo))
    {
        IMSA_StopTimer(&pstSmrMtEntity->stTimerInfo);

        /*给上层MSG上报错误指示*/
        /* modify by jiqiang 2014.03.27 pclint fix error 56 begin */
        /*lint -e56*/
        IMSA_SMS_SndMtErrInd(SMR_SMT_ERROR_IMSA_ERROR_BEGIN + MN_MSG_IMSA_CAUSE_NO_SERVICE);
        /*lint +e56*/
        /* modify by jiqiang 2014.03.27 pclint fix error 56 end */
    }

    /*初始化变量*/
    pstSmrMoEntity->enState = IMSA_SMS_SMR_STATE_IDLE;
    pstSmrMoEntity->ucMessageReference = 0;

    pstSmrMoEntity->ucMemAvailFlg = IMSA_SMS_FALSE;                   /* 清除mem avail标志                        */
    pstSmrMoEntity->ucRetransFlg  = IMSA_SMR_RETRANS_PERMIT;          /* 复位重发标志                             */

    pstSmrMtEntity->enState = IMSA_SMS_SMR_STATE_IDLE;
    pstSmrMtEntity->ucMessageReference= 0;
}
/*lint +e961*/
/*lint +e960*/
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaProcSmsMsg.c */



