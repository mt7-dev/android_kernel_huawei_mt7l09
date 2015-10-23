


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "NasETcMain.h"
#include  "NasETcEmmMsgProc.h"
#include  "NasETcPdcpMsgProc.h"
#include  "NasETcERabmMsgProc.h"
#include  "NasCommPrint.h"
#include  "NasETcPublic.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASTCMAIN_C
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
/* L模状态 */
NAS_ETC_L_MODE_STATUS_ENUM_UINT8         g_enEtcLModeStatus = NAS_ETC_L_MODE_STATUS_SUSPENDED;

NAS_ETC_MODE_TYPE_ENUM_UINT32            g_enEtcModeType = NAS_ETC_MODE_TYPE_NORMAL;

NAS_COMM_PRINT_MSG_LIST_STRU g_astETcPdcpMsgIdArray[] =
{
    {   ID_LTC_LPDCP_ACTIVATE_TEST_MODE               ,
            "MSG:  ID_LTC_LPDCP_ACTIVATE_TEST_MODE                           "},
    {   ID_LTC_LPDCP_ACTIVATE_TEST_MODE_CNF               ,
            "MSG:  ID_LTC_LPDCP_ACTIVATE_TEST_MODE_CNF                           "},
    {   ID_LTC_LPDCP_START_TEST_LOOP               ,
            "MSG:  ID_LTC_LPDCP_START_TEST_LOOP                           "},
    {   ID_LTC_LPDCP_START_TEST_LOOP_CNF               ,
            "MSG:  ID_LTC_LPDCP_START_TEST_LOOP_CNF                           "},
    {   ID_LTC_LPDCP_STOP_TEST_LOOP               ,
            "MSG:  ID_LTC_LPDCP_STOP_TEST_LOOP                           "},
    {   ID_LTC_LPDCP_STOP_TEST_LOOP_CNF               ,
            "MSG:  ID_LTC_LPDCP_STOP_TEST_LOOP_CNF                           "},
    {   ID_LTC_LPDCP_DEACTIVATE_TEST_MODE               ,
            "MSG:  ID_LTC_LPDCP_DEACTIVATE_TEST_MODE                           "},
    {   ID_LTC_LPDCP_DEACTIVATE_TEST_MODE_CNF               ,
            "MSG:  ID_LTC_LPDCP_DEACTIVATE_TEST_MODE_CNF                           "}

};

NAS_COMM_PRINT_MSG_LIST_STRU g_astETcCdsMsgIdArray[] =
{
    {   ID_ETC_CDS_ACT_TEST_MODE_NOTIFY               ,
            "MSG:  ID_ETC_CDS_ACT_TEST_MODE_NOTIFY                           "},
    {   ID_ETC_CDS_DEACT_TEST_MODE_NOTIFY               ,
            "MSG:  ID_ETC_CDS_DEACT_TEST_MODE_NOTIFY                           "}
};


/*****************************************************************************
  3 Function
*****************************************************************************/

/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_ETC_TaskEntry( MsgBlock* pMsg )
{
    PS_MSG_HEADER_STRU       *pHeader = VOS_NULL_PTR;              /*定义消息头指针*/

    /* 判断入口参数是否合法*/
    if (VOS_NULL_PTR == pMsg)
    {
        /*报告错误消息*/
        NAS_ETC_WARN_LOG("NAS_ETC_TaskEntry:ERROR: No Msg!");
        return;
    }

    pHeader = (PS_MSG_HEADER_STRU *)pMsg;          /* 获取消息头指针*/

    NAS_ETC_PrintTcRevMsg(pHeader,NAS_COMM_GET_ESM_PRINT_BUF());

    /*判断接收到的消息,进行分发处理*/
    switch (pHeader->ulSenderPid)
    {
        /*如果是EMM消息，调用EMM分发函数*/
        case PS_PID_MM:
            NAS_ETC_EmmMsgDistr(pMsg);
            break;

        /*如果是RABM消息，调用RABM分发函数*/
        case PS_PID_RABM:
            NAS_ETC_RabmMsgDistr(pMsg);
            break;

        /*如果是PDCP UL消息，调用PDCP UL分发函数*/
        case PS_PID_PDCP_UL:
            NAS_ETC_PdcpUlMsgDistr(pMsg);
            break;

        /*发送对象错误，报警*/
        default:
            NAS_ETC_LOG1("NAS_ETC_TaskEntry:NORM: Error SenderPid :",(VOS_INT32)pHeader->ulSenderPid);
            break;
    }
}
VOS_VOID NAS_ETC_Init(VOS_VOID)
{
    /*设置L MODE的状态为SUSPENDED*/
    NAS_ETC_SetLModeStatus(NAS_ETC_L_MODE_STATUS_SUSPENDED);
}
VOS_UINT32 NAS_ETC_PidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{
    switch ( ip )
    {
        case    VOS_IP_LOAD_CONFIG:
                {
                    NAS_ETC_Init();
                }
                break;

        case    VOS_IP_FARMALLOC:
        case    VOS_IP_INITIAL:
        case    VOS_IP_ENROLLMENT:
        case    VOS_IP_LOAD_DATA:
        case    VOS_IP_FETCH_DATA:
        case    VOS_IP_STARTUP:
        case    VOS_IP_RIVAL:
        case    VOS_IP_KICKOFF:
        case    VOS_IP_STANDBY:
        case    VOS_IP_BROADCAST_STATE:
        case    VOS_IP_RESTART:
        case    VOS_IP_BUTT:
                break;

        default:
                break;
    }

    return VOS_OK;
}


VOS_INT32  NAS_ETC_PrintTcPdcpMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    EMM_ERABM_MSG_TYPE_ENUM_UINT32        enMsgId
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulTcPdcpMsgNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* 获得消息表的长度 */
    ulTcPdcpMsgNum = sizeof(g_astETcPdcpMsgIdArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    /* 查找对应的消息 */
    for (ulLoop = 0; ulLoop< ulTcPdcpMsgNum ; ulLoop++)
    {
        if (enMsgId == g_astETcPdcpMsgIdArray[ulLoop].ulId)
        {
            break;
        }
    }

    /* 打印对应的消息 */
    if ( ulLoop < ulTcPdcpMsgNum )
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         (VOS_CHAR *)(g_astETcPdcpMsgIdArray[ulLoop].aucPrintString),
                         &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           "[ TICK : %ld ]",
                           PS_GET_TICK(),
                          &ilOutPutLen);
    }
    else
    {
        NAS_ETC_LOG1("NAS_ETC_PrintTcPdcpMsg, Invalid enMsgId: ",enMsgId);
    }

    return ilOutPutLen;

}
VOS_INT32  NAS_COMM_PrintEtcCdsMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    EMM_ERABM_MSG_TYPE_ENUM_UINT32        enMsgId
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulTcPdcpMsgNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* 获得消息表的长度 */
    ulTcPdcpMsgNum = sizeof(g_astETcCdsMsgIdArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    /* 查找对应的消息 */
    for (ulLoop = 0; ulLoop< ulTcPdcpMsgNum ; ulLoop++)
    {
        if (enMsgId == g_astETcCdsMsgIdArray[ulLoop].ulId)
        {
            break;
        }
    }

    /* 打印对应的消息 */
    if ( ulLoop < ulTcPdcpMsgNum )
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         (VOS_CHAR *)(g_astETcCdsMsgIdArray[ulLoop].aucPrintString),
                         &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           "[ TICK : %ld ]",
                           PS_GET_TICK(),
                          &ilOutPutLen);
    }
    else
    {
        NAS_ETC_LOG1("NAS_COMM_PrintEtcCdsMsg, Invalid enMsgId: ",enMsgId);
    }

    return ilOutPutLen;

}
VOS_VOID NAS_ETC_PrintTcRevMsg
(
    const PS_MSG_HEADER_STRU           *pstMsg,
    VOS_CHAR                           *pcBuff
)
{
    VOS_INT32                           ilOutPutLenHead     = 0;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen          = 0;

    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "NAS TC Receive Message:",
                     &ilOutPutLen);

    if ( 0 == ilOutPutLen )
    {
        NAS_ETC_WARN_LOG("NAS_ETC_PrintTcRevMsg, Print receive msg header exception.");
        return ;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* 打印各个PID的消息 */
    switch ( pstMsg->ulSenderPid )
    {

        case PS_PID_MM:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "MM-->TC\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintMmTcMsg(pcBuff,
                                                   usTotalLen,
                                                   pstMsg->ulMsgName);

            break;

        case PS_PID_RABM:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "RABM-->TC\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintRabmTcMsg(pcBuff,
                                             usTotalLen,
                                             pstMsg->ulMsgName);

            break;

        case PS_PID_PDCP_UL:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "PDCP-->TC\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_ETC_PrintTcPdcpMsg(pcBuff,
                                             usTotalLen,
                                             pstMsg->ulMsgName);

            break;

        default:
            NAS_ETC_LOG2("NAS_ETC_PrintTcRevMsg,Invalid Pid, MsgId: ",
                        pstMsg->ulSenderPid,
                        pstMsg->ulMsgName);

            return ;
    }

    if ( 0 == ilOutPutLen )
    {
        NAS_ETC_LOG2("NAS_ETC_PrintTcRevMsg, print return zero length.SenderPid, MsgId: ",
                    pstMsg->ulSenderPid,
                    pstMsg->ulMsgName);
        return;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_Print(pcBuff, usTotalLen);

    return ;

}
VOS_VOID NAS_ETC_PrintTcSendMsg
(
    const PS_MSG_HEADER_STRU           *pstMsg,
    VOS_CHAR                           *pcBuff
)
{
    VOS_INT32                           ilOutPutLenHead     = 0;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen          = 0;

    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "NAS TC Send Message:",
                     &ilOutPutLen);

    if ( 0 == ilOutPutLen )
    {
        NAS_ETC_WARN_LOG("NAS_ETC_PrintTcSendMsg, Print receive msg header exception.");
        return ;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* 打印各个PID的消息 */
    switch ( pstMsg->ulReceiverPid )
    {

        case PS_PID_MM:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "TC-->MM\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintMmTcMsg(pcBuff,
                                               usTotalLen,
                                               pstMsg->ulMsgName);

            break;

        case PS_PID_RABM:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "TC-->RABM\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintRabmTcMsg(pcBuff,
                                             usTotalLen,
                                             pstMsg->ulMsgName);

            break;

        case PS_PID_PDCP_UL:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "TC-->PDCP\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_ETC_PrintTcPdcpMsg(pcBuff,
                                             usTotalLen,
                                             pstMsg->ulMsgName);

            break;

        case UEPS_PID_CDS:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "TC-->CDS\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintEtcCdsMsg(pcBuff,
                                             usTotalLen,
                                             pstMsg->ulMsgName);

            break;

        default:
            NAS_ETC_LOG2("NAS_ETC_PrintTcSendMsg,Invalid Pid, MsgId: ",
                        pstMsg->ulSenderPid,
                        pstMsg->ulMsgName);

            return ;
    }

    if ( 0 == ilOutPutLen )
    {
        NAS_ETC_LOG2("NAS_ETC_PrintTcSendMsg, print return zero length.SenderPid, MsgId: ",
                    pstMsg->ulSenderPid,
                    pstMsg->ulMsgName);
        return;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* 打印接收到该消息时对应的系统TICK值 */
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       "[ TICK : %ld ]",
                       PS_GET_TICK(),
                       &ilOutPutLen);
    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_Print(pcBuff, usTotalLen);

    return ;

}
/*lint +e961*/
/*lint +e960*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


