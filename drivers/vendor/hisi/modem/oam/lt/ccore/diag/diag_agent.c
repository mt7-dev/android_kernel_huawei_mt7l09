

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
/*lint -save -e537*/
#include  "vos.h"
#include  "diag_agent.h"
#include  "msp_diag.h"
#include  "msp_drx.h"
#include  "diag_common.h"
#include  "diag_cfg.h"
#include  "diag_debug.h"
#include  "bbp_datalog.h"
#include  "diag_api.h"
#include  "msp_errno.h"
#include  "drx_msp_api.h"
#include  "DrvInterface.h"
#include  "diag_mem.h"
#include  "socp_lfix_chan.h"
/*lint -restore*/
/*lint -save -e740*/
#define    THIS_FILE_ID   MSP_FILE_ID_DIAG_AGENT_C

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
DIAG_GTR_DATA_RCV_PFN g_pfnDiagGtrCallBack = VOS_NULL;

/*****************************************************************************
  3 Function
*****************************************************************************/


VOS_VOID DIAG_GtrRcvCallBackReg(DIAG_GTR_DATA_RCV_PFN pfnGtrRcv)
{
    g_pfnDiagGtrCallBack = pfnGtrRcv;
}


DIAG_GTR_DATA_RCV_PFN diag_GetGtrCallBack(VOS_VOID);
DIAG_GTR_DATA_RCV_PFN diag_GetGtrCallBack(VOS_VOID)
{
    return g_pfnDiagGtrCallBack;
}


VOS_UINT32 diag_GtrProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId)
{
    DIAG_CMD_GTR_SET_REQ_STRU* pstGtrReq = NULL;
    DIAG_CMD_GTR_SET_CNF_STRU stGtrCnf = {0};
    DIAG_GTR_DATA_RCV_PFN pfnCallBack = diag_GetGtrCallBack();
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT32 ulCnfRst = ERR_MSP_SUCCESS;

    pstGtrReq = (DIAG_CMD_GTR_SET_REQ_STRU*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    if (pstGtrReq->ulGtrDtaSize > DIAG_CMD_DATA_MAX_LEN)
    {
        ulCnfRst = ERR_MSP_FAILURE;
    }
    else
    {
        if (pfnCallBack != NULL)
        {
            ulCnfRst = pfnCallBack(pstGtrReq->ulGtrDtaSize, pstGtrReq->aucDta);
        }
        else
        {
            ulCnfRst = ERR_MSP_FAILURE;
        }
    }

    stGtrCnf.ulRc = ulCnfRst;
    ret = diag_AgentCnfFun((VOS_UINT8*)&stGtrCnf,ulCmdId,sizeof(DIAG_CMD_GTR_SET_CNF_STRU));
    return ret;
}


VOS_VOID diag_AgentTimeOutProcEntry(VOS_VOID* pstPara)
{

    return ;
}

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
VOS_SEM g_diagAgentSem = (VOS_SEM)0;
VOS_SEM g_diagMbxSem = (VOS_SEM)0;
DIAG_LDSP_STATE_ENUM_U32 g_diagDspState = LDSP_NOT_INIT;
DIAG_LDSP_DEBUG_INFO g_diagDspDebugInfo = {0, 0};
VOS_UINT32 g_diagSocpPowerOnReq = VOS_FALSE;

VOS_VOID diag_ShowDspInfo(VOS_VOID)
{
    char * dspState;

    switch(g_diagDspState)
    {
        case LDSP_NOT_INIT:
            dspState = "NOT INIT";
            break;

        case LDSP_INITING:
            dspState = "INITING";
            break;

        case LDSP_INITED:
            dspState = "INITED";
            break;

        case LDSP_SOCP_ENABLE:
            dspState = "SOCP ENABLE";
            break;

        case LDSP_SOCP_DISABLE:
            dspState = "SOCP_DISABLE";
            break;

        default:
            dspState = "UNKNOWN";
            break;
    }

    diag_printf("***********DIAG DSP INFO***********\n");
    diag_printf("SEND COUNT    : 0x%x\n", g_diagDspDebugInfo.ulSendOpId);
    diag_printf("RECEIVE COUNT : 0x%x\n", g_diagDspDebugInfo.ulRevOpId);
    diag_printf("DSP STATUS    : %s\n", dspState);
}


VOS_VOID diag_SocpVoteCnfMsgProc(MsgBlock * pMsgBlock)
{
    DIAG_MSG_SOCP_VOTE_WAKE_CNF_STRU * voteCnf;

    voteCnf = (DIAG_MSG_SOCP_VOTE_WAKE_CNF_STRU *)pMsgBlock;
    if((voteCnf->ulVoteId == SOCP_VOTE_DIAG_COMM) && (voteCnf->ulVoteType) == SOCP_VOTE_FOR_WAKE && (g_diagDspState == LDSP_INITING))
    {
        if(voteCnf->ulVoteRst != 0)
        {
            diag_printf("%s: vote fail", __FUNCTION__);
        }
    }
}


VOS_VOID diag_AgentSendMsgToDsp(DIAG_SOCP_STATE_ENUM_U32 state)
{
    DIAG_MSG_TO_DSP_STRU pData;
    VOS_UINT32  ret;

    pData.ulMsgId = 0;
    pData.ulOpId  = g_diagDspDebugInfo.ulSendOpId++;
    pData.ulReq   = state;

    ret = BSP_MailBox_ComMsgWrite(EN_MAILBOX_SERVICE_RTT_SYS_CTRL, &pData, sizeof(DIAG_MSG_TO_DSP_STRU), EN_MAILBOX_SLEEP_WAKEUP);
    if(ERR_MSP_SUCCESS != ret)
    {
        diag_printf("%s: send to dsp fail ret=0x%x\n", __FUNCTION__, ret);
        DIAG_DEBUG_SDM_FUN(EN_DIAG_AGENT_LDSP_MB_MSG, 5, 0, 0);
        return ;
    }
}


VOS_VOID diag_AgentVoteToSocp(SOCP_VOTE_TYPE_ENUM_U32 voteType)
{
    DIAG_MSG_SOCP_VOTE_REQ_STRU * voteReq;

    voteReq = (DIAG_MSG_SOCP_VOTE_REQ_STRU *)VOS_AllocMsg(MSP_PID_DIAG_AGENT,(sizeof(DIAG_MSG_SOCP_VOTE_REQ_STRU) - 20));
    if(voteReq == NULL)
    {
        diag_printf("%s: alloc msg fail\n", __FUNCTION__);
        return;
    }
    voteReq->ulReceiverPid = MSP_PID_DIAG_APP_AGENT;
    voteReq->ulSenderPid   = MSP_PID_DIAG_AGENT;
    voteReq->ulLength      = sizeof(DIAG_MSG_SOCP_VOTE_REQ_STRU) - 20;
    voteReq->ulVoteId      = SOCP_VOTE_DIAG_COMM;
    voteReq->ulVoteType    = voteType;

    if(ERR_MSP_SUCCESS != VOS_SendMsg(MSP_PID_DIAG_AGENT, voteReq))
    {
        diag_printf("%s: send msg fail\n", __FUNCTION__);
    }
}


VOS_VOID diag_AgentDspMsgProc(MsgBlock * pMsgBlock)
{
    DIAG_MSG_DSP_CNF_TO_AGENT_STRU * pData;

    pData = (DIAG_MSG_DSP_CNF_TO_AGENT_STRU *)pMsgBlock;
    g_diagDspDebugInfo.ulRevOpId = pData->ulMsgData.ulOpId;

    /* LDSP首次加载，需要向MCORE投票下电 */
    if(g_diagSocpPowerOnReq == VOS_TRUE)
    {
        if(0 != DRV_SOCP_VOTE_TO_MCORE(SOCP_VOTE_FOR_SLEEP))
        {
            diag_printf("%s: socp power on fail\n", __FUNCTION__);
            return;
        }

        g_diagSocpPowerOnReq = VOS_FALSE;
        return;
    }

    /* LDSP停止使用SOCP的回复 */
    if(g_diagDspState == LDSP_SOCP_DISABLE)
    {
        diag_AgentVoteToSocp(SOCP_VOTE_FOR_SLEEP);
    }
}

/*****************************************************************************
 Function Name   : diag_MailboxCb
 Description     : 诊断向邮箱注册的回调函数，当DSP给DIAG发消息时，会回调此函数
 注意，此回调函数在中断中调用，不要有阻塞性接口
*****************************************************************************/
VOS_VOID diag_MailboxCb(MAILBOX_INT_TYPE_E enIntType)
{
    (VOS_VOID)VOS_SmV(g_diagMbxSem);
    return;
}


VOS_VOID diag_PortDisconnMsgProc(VOS_VOID)
{
    /* LDSP未初始化或者正在初始化，直接投票睡眠 */
    if(g_diagDspState == LDSP_NOT_INIT || g_diagDspState == LDSP_INITING)
    {
        diag_AgentVoteToSocp(SOCP_VOTE_FOR_SLEEP);
        return;
    }

    /* 通知DSP SOCP不可用 */
    diag_AgentSendMsgToDsp(SOCP_DISABLE);
    g_diagDspState = LDSP_SOCP_DISABLE;
}


VOS_VOID diag_AgentConnectCmdProc(DIAG_CONNECT_CMD_ENUM_U32 connCmd)
{
    if(connCmd >= DIAG_CONNECT_CMD_BUTT)
    {
        diag_printf("%s: invalid cmd 0x%x\n", __FUNCTION__, connCmd);
        return;
    }

    /* LDSP未初始化或者正在初始化 */
    if(g_diagDspState == LDSP_NOT_INIT || g_diagDspState == LDSP_INITING)
    {
        /* 连接命令投反对票 */
        if(DIAG_CONNECT_CMD == connCmd)
        {
            diag_AgentVoteToSocp(SOCP_VOTE_FOR_WAKE);
            return;
        }
        /*断开连接投赞成票*/
        diag_AgentVoteToSocp(SOCP_VOTE_FOR_SLEEP);
        return;
    }

    /* LDSP已经初始化，邮箱可用，连接、断开连接需要通知LDSP SOCP状态变化 */
    if(DIAG_CONNECT_CMD == connCmd)
    {
        /* 通知DSP SOCP可用 */
        diag_AgentSendMsgToDsp(SOCP_ENABLE);
        g_diagDspState = LDSP_SOCP_ENABLE;
        /* 投反对票 */
        diag_AgentVoteToSocp(SOCP_VOTE_FOR_WAKE);
        return;
    }

    if(DIAG_DISCONNECT_CMD == connCmd)
    {
        /* 通知DSP SOCP不可用，在等到邮箱回复后，投赞成票 */
        diag_AgentSendMsgToDsp(SOCP_DISABLE);
        g_diagDspState = LDSP_SOCP_DISABLE;
        return;
    }
    /* coverity[dead_error_line] */
}


VOS_VOID DIAG_DspInitFinished(VOS_VOID)
{
    g_diagDspState = LDSP_INITED;
    /* 工具未连接,通知LDSP停止使用SOCP */
    if(!(g_ulDiagCfgInfo & (1 << DIAG_CFG_CONN_BIT)))
    {
        diag_AgentSendMsgToDsp(SOCP_DISABLE);
        g_diagDspState = LDSP_SOCP_DISABLE;
    }
}
VOS_UINT32 DIAG_SocpPowerOn(VOS_VOID)
{
    g_diagDspState = LDSP_INITING;

    /* 工具已连接 */
    if(g_ulDiagCfgInfo & (1 << DIAG_CFG_CONN_BIT))
    {
        return ERR_MSP_SUCCESS;
    }

    if(g_diagSocpPowerOnReq == VOS_TRUE)
    {
        diag_printf("%s: power on req is running\n", __FUNCTION__);
        return ERR_MSP_SUCCESS;
    }

    g_diagSocpPowerOnReq = VOS_TRUE;

    if(0 != DRV_SOCP_VOTE_TO_MCORE(SOCP_VOTE_FOR_WAKE))
    {
        diag_printf("%s: socp power on fail\n", __FUNCTION__);
        return ERR_MSP_FAILURE;
    }

    return ERR_MSP_SUCCESS;
}

VOS_VOID diag_MailboxSelfTask(VOS_VOID)
{
    VOS_UINT32 len, ret;
    DIAG_MSG_DSP_CNF_TO_AGENT_STRU * pData;

    if(ERR_MSP_SUCCESS != VOS_SmBCreate("diagmbx", 0, VOS_SEMA4_FIFO, &g_diagMbxSem))
    {
        diag_printf("[%s]:agent sem init err!\n",__FUNCTION__);
        return ;
    }

    /* 向邮箱注册回调函数 */
    if(ERR_MSP_SUCCESS != BSP_MailBox_ComNotifyReg(EN_MAILBOX_SERVICE_RTT_SYS_CTRL, diag_MailboxCb))
    {
        diag_printf("[%s]:BSP_MailBox_ComNotifyReg err!\n",__FUNCTION__);
        return ;
    }
    /*lint -save -e716*/
    while(1)
    /*lint -restore*/
    {
        if(VOS_OK != VOS_SmP(g_diagMbxSem, 0))
        {
            diag_printf("[%s] VOS_SmP failed.\n", __FUNCTION__);

            continue;
        }

        DIAG_DEBUG_SDM_FUN(EN_DIAG_AGENT_LDSP_MB_MSG, 0, 0, 0);

        len = BSP_MailBox_ComMsgSize(EN_MAILBOX_SERVICE_RTT_SYS_CTRL);
        if(0 == len)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_AGENT_LDSP_MB_MSG, 1, 0, 0);
            continue ;
        }

        /* VOS消息体在事先分配好的内存中查找空闲，没有阻塞操作 */
        pData = (DIAG_MSG_DSP_CNF_TO_AGENT_STRU *)VOS_AllocMsg(MSP_PID_DIAG_AGENT, len);
        if(VOS_NULL == pData)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_AGENT_LDSP_MB_MSG, 2, 0, 0);
            continue ;
        }

        /* (pData+20)表示把邮箱中的内容直接读取到消息中 */
        ret = BSP_MailBox_ComMsgRead(EN_MAILBOX_SERVICE_RTT_SYS_CTRL, ((VOS_UINT8 *)pData+VOS_MSG_HEAD_LENGTH), len, EN_MAILBOX_SLEEP_WAKEUP);
        if(ret)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_AGENT_LDSP_MB_MSG, 3, 0, 0);
            continue;
        }

        pData->ulReceiverPid = MSP_PID_DIAG_AGENT;
        pData->ulSenderPid   = MSP_PID_DIAG_AGENT;
        pData->ulLength      = len;

        /* 注意，只能给自己发消息，跨核发消息中会有阻塞接口，不能在中断中调用 */
        ret = VOS_SendMsg(MSP_PID_DIAG_AGENT, pData);
        if (ret != VOS_OK)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_AGENT_LDSP_MB_MSG, 4, 0, 0);
        }
    }
}

#endif
VOS_UINT32 diag_AgentMsgProcInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    DIAG_MSG_SOCP_VOTE_REQ_STRU * voteReq;
#endif

    if (ip == VOS_IP_RESTART)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_AGENT_INIT,ret,0,0);

        /*DIAG SOCP BUF初始化*/
        ret = diag_BufCtrlGlobalInit();
        if(ret != ERR_MSP_SUCCESS)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_AGENT_INIT_ERROR,ret,0,0);
            return ret;
        }

        /*设置DIAG初始化bit*/
        ret = diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_INIT_BIT,DIAG_CFG_SWT_OPEN);
        if(ret != ERR_MSP_SUCCESS)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_AGENT_INIT_ERROR,ret,0,1);
            return ret;
        }

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
#if 0
        if(ERR_MSP_SUCCESS != VOS_SmBCreate(NULL, 0, VOS_SEMA4_FIFO, &g_diagAgentSem))
        {
            diag_printf("[%s]:agent sem init err!\n",__FUNCTION__);
            return ERR_MSP_FAILURE;
        }
#endif
        voteReq = (DIAG_MSG_SOCP_VOTE_REQ_STRU *)VOS_AllocMsg(MSP_PID_DIAG_AGENT,(sizeof(DIAG_MSG_SOCP_VOTE_REQ_STRU) - 20));
        if(voteReq == NULL)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_AGENT_INIT_ERROR,ERR_MSP_FAILURE,2,2);
            return ERR_MSP_FAILURE;
        }

        voteReq->ulReceiverPid = MSP_PID_DIAG_APP_AGENT;
        voteReq->ulSenderPid   = MSP_PID_DIAG_AGENT;
        voteReq->ulLength      = sizeof(DIAG_MSG_SOCP_VOTE_REQ_STRU) - 20;
        voteReq->ulVoteId      = SOCP_VOTE_DIAG_COMM;
        voteReq->ulVoteType    = SOCP_VOTE_FOR_SLEEP;
        ret = VOS_SendMsg(MSP_PID_DIAG_AGENT, voteReq);
        if(ret != ERR_MSP_SUCCESS)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_AGENT_INIT_ERROR,ret,3,2);
            return ret;
        }
#endif
    }
    return ret;
}


VOS_UINT32 diag_AgentDispatchCmd(VOS_UINT8* pstReq)
{
    VOS_UINT32 ret =ERR_MSP_FAILURE;
    VOS_UINT32 ulCmdId = 0;

    if(NULL == pstReq)
    {
        return ret;
    }
    ulCmdId = (((MSP_DIAG_HEAD_STRU*)DIAG_OFFSET_SOCP_GET_DIAG_HEAD(pstReq))->ulID);
    DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_AGENT_DISPATCH_CMD,ulCmdId,0,0);

    if(MSP_STRU_ID_28_31_GROUP_MSP == MSP_STRU_ID_28_31_CMD_GROUP(ulCmdId))
    {
        switch (MSP_STRU_ID_16_23_CMD_TYPE(ulCmdId))
        {
            /*根据二级字段区分是给ps的透传命令还是msp内部命令或者PMU的命令*/
            case MSP_STRU_ID_16_23_MSP_CMD:
                switch(MSP_STRU_ID_8_11_CMD_CATEGORY(ulCmdId))
                {
                    /*根据命令ID范围区分是哪种msp内部命令*/
                	case DIAG_CATEGORY_BBP:
                        ret = diag_BbpProcEntry(pstReq,ulCmdId);
                        break;
                    case DIAG_CATEGORY_CFG:
                        ret = diag_CfgProcEntry(pstReq,ulCmdId);
                        break;
                    case DIAG_CATEGORY_GTR:
                        ret = diag_GtrProcEntry(pstReq,ulCmdId);
						break;
                    default:
                        break;
                }
                break;

            case MSP_STRU_ID_16_23_SYS_CMD:
                switch(MSP_STRU_ID_8_11_CMD_CATEGORY(ulCmdId))
                {
                    case DIAG_CATEGORY_CONN:
                        ret = diag_ConnProcEntry(pstReq,ulCmdId);
                        break;
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
    else if((MSP_STRU_ID_28_31_GROUP_LTE_DSP == MSP_STRU_ID_28_31_CMD_GROUP(ulCmdId))||
            (MSP_STRU_ID_28_31_GROUP_TDS_DSP == MSP_STRU_ID_28_31_CMD_GROUP(ulCmdId)))
    {
         ret = diag_DspTransProcEntry(pstReq,ulCmdId);
    }
/*lint -e553*/
#if (FEATURE_BSP_LCH_OM == FEATURE_ON)
/*lint +e553*/
    else if(MSP_STRU_ID_28_31_GROUP_BSP == MSP_STRU_ID_28_31_CMD_GROUP(ulCmdId))
    {
        switch (MSP_STRU_ID_16_23_CMD_TYPE(ulCmdId))
        {
            case MSP_STRU_ID_16_23_BSP_CMD:
                ret =  diag_BspProcEntry(pstReq,ulCmdId);
                break;
            default:
                break;
        }
    }
#endif
    else
    {
    }
    return ret;
}
/*****************************************************************************
 Function Name   : diag_AgentPsTransRcv
 Description     : DIAG 上报PS路测消息接口
 Input           : MsgBlock* pMsgBlock
 Output          : None
 Return          : VOS_UINT32
*****************************************************************************/
VOS_VOID diag_AgentPsTransRcv(MsgBlock* pMsgBlock)
{
    /*直接使用PS发送过来的MsgID进行消息上报*/
    VOS_UINT16 ulID = (VOS_UINT16)(((DIAG_DATA_MSG_STRU*)pMsgBlock)->ulMsgId);/* [false alarm]:屏蔽Fortify */
    VOS_UINT32 ulDataSize = pMsgBlock->ulLength;/*lint !e958*/
    VOS_VOID* pData = (VOS_VOID*)pMsgBlock->aucValue;
    VOS_UINT32 ret = 0;

    ret = DIAG_ReportCommand(ulID, (ulDataSize - DIAG_PS_TRANS_HEAD_LEN), (pData + DIAG_PS_TRANS_HEAD_LEN));/*lint !e416 !e124*/
    if((ret != ERR_MSP_SUCCESS)&&(ret != ERR_MSP_NO_INITILIZATION))
    {
        diag_printf("%s  send pid = %d\r\n", __FUNCTION__, pMsgBlock->ulSenderPid);
    }
}

VOS_VOID diag_AgentMsgProc(MsgBlock* pMsgBlock)
{
    DIAG_DATA_MSG_STRU* pMsgTmp =NULL;
    REL_TIMER_MSG *pTimer =NULL;

     /*入参判断*/
    if (NULL == pMsgBlock)
    {
        return;
    }

    /*任务开始处理，不允许睡眠*/
	drx_msp_fid_vote_lock(DRX_DIAG_MODEM_AGENT_VOTE);

    /*根据发送PID，执行不同处理*/
    switch(pMsgBlock->ulSenderPid)
    {
        /*超时消息，用于低功耗数采*/
        case DOPRA_PID_TIMER:
            /*lint -save -e740*/
            pTimer   = (REL_TIMER_MSG*)pMsgBlock;
            /*lint -restore*/
            diag_AgentTimeOutProcEntry(pTimer);
            break;

        /*处理fw发过来的数据 */
        case MSP_PID_DIAG_FW:
            /*lint -save -e740*/
            pMsgTmp = (DIAG_DATA_MSG_STRU*)pMsgBlock;
            /*lint -restore*/
            if(ID_MSG_DIAG_HSO_DISCONN_IND == pMsgTmp->ulMsgId)
            {
               diag_CfgResetAllSwt();
#if (FEATURE_BSP_LCH_OM == FEATURE_ON)
               DRV_OM_SET_HSO_CONN_FLAG(0);
#endif
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
               diag_PortDisconnMsgProc();
#endif
            }
            else
            {
                /*根据命令ID分发处理*/
                diag_AgentDispatchCmd(pMsgTmp->pContext);
            }
            break;

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
        case MSP_PID_DIAG_AGENT:
            diag_AgentDspMsgProc(pMsgBlock);
            break;
        case MSP_PID_DIAG_APP_AGENT:
            diag_SocpVoteCnfMsgProc(pMsgBlock);
            break;
#endif
        /*PS路测消息上报*/
        case PS_PID_ERRC:
        case PS_PID_MM:
        case PS_PID_ESM:
            diag_AgentPsTransRcv(pMsgBlock);
            break;
         /*lint -save -e616*/
         default:
        /*lint -restore*/
            break;
    }

    /*任务开始结束，允许睡眠*/
    drx_msp_fid_vote_unlock(DRX_DIAG_MODEM_AGENT_VOTE);
    return;
}


VOS_UINT32 diag_DspTransProcEntry(VOS_UINT8* pstReq ,VOS_UINT32 ulCmdId)
{
    VOS_UINT32 ulLen = 0;
    DIAG_CMD_DSP_CNF_STRU stRttCnf = {0};
	VOS_UINT32 ret = 0;
	MSP_DIAG_HEAD_STRU *pstDspMsg = NULL;
	VOS_UINT8* pData = NULL;
    VOS_UINT32 ulServeType = EN_MAILBOX_SERVICE_LTE_OM;

    if(MSP_STRU_ID_28_31_GROUP_TDS_DSP == (MSP_STRU_ID_28_31_CMD_GROUP(ulCmdId)))
    {
		DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_TDS_DSP_CNF,ulCmdId,0,0);
		ulServeType = EN_MAILBOX_SERVICE_TDS_OM;
    }
	else
	{
		DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_LTE_DSP_CNF,ulCmdId,0,0);
	}

    /* DSP消息的ID和size */
    pstDspMsg = (MSP_DIAG_HEAD_STRU*)(pstReq+sizeof(MSP_SOCP_HEAD_STRU));

    ulLen = sizeof(VOS_UINT32) + (pstDspMsg->ulDataSize - DRA_RTT_NOT_NEED_CHARS_LEN);

    pData = VOS_MemAlloc(MSP_PID_DIAG_AGENT,DYNAMIC_MEM_PT,ulLen);
    if(NULL == pData)
    {
        return ERR_MSP_MALLOC_FAILUE;
    }

    /* 写入消息ID */
    *(VOS_UINT32*)(pData) = pstDspMsg->ulID;

    /* 写入消息内容 */
    VOS_MemCpy((pData + sizeof(VOS_UINT32)), (pstReq + DIAG_FULL_HEAD_LEN), (pstDspMsg->ulDataSize - DRA_RTT_NOT_NEED_CHARS_LEN));

    ret = BSP_MailBox_ComMsgWrite((MAILBOX_SERVICE_TYPE_E)ulServeType, pData, ulLen, EN_MAILBOX_SLEEP_WAKEUP);
    if(ERR_MSP_SUCCESS != ret)
    {
        /* 开机时DSP未启动时，HSO下发DSP操作导致写失败，避免打印刷屏，屏蔽打印 */
//        printf("write BSP_MailBox_ComMsgWrite fail!\n");
    }

    VOS_MemFree(MSP_PID_DIAG_AGENT,pData);

    /*模拟DSP回复*/
    stRttCnf.ulRc = ret;
    ret = diag_AgentCnfFun((VOS_UINT8*)&stRttCnf,ulCmdId,sizeof(DIAG_CMD_DSP_CNF_STRU));
    return ret;
}
/*lint -e553*/
#if (FEATURE_BSP_LCH_OM == FEATURE_ON)
/*lint +e553*/

VOS_UINT32 diag_BspAxiDataConfigEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
VOS_UINT32 diag_BspAxiDataConfigEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId)
{
    VOS_UINT32  ret ;
    VOS_UINT32 cnf_data_len = 0;
    VOS_UINT8 *pdata =NULL;
    AXI_DATA_CONFIG_CNF_STRU * axi_data_config_cnf = NULL;

    pdata = (VOS_UINT8*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));
    axi_data_config_cnf = DRV_AXI_DATA_CONFIG(pdata, (VOS_UINT*)&cnf_data_len);

    vos_printf("diag_BspAxiDataConfigEntry  cnf_data_len  = 0x%x\n", cnf_data_len);

    if(VOS_NULL != axi_data_config_cnf)
    {
        ret = diag_AgentCnfFun((VOS_UINT8*)axi_data_config_cnf,ulCmdId,cnf_data_len);
        free(axi_data_config_cnf);
    }
    else
    {
        vos_printf("diag_BspAxiDataConfigEntry  ERROR\n");
        ret = ERR_MSP_FAILURE;
    }

    return ret;
}


VOS_UINT32 diag_BspAxiMonConfigEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
VOS_UINT32 diag_BspAxiMonConfigEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId)
{
    VOS_UINT32  ret ;
    VOS_UINT32 cnf_data_len = 0;
    VOS_UINT8 *pdata =NULL;
    AXI_MON_CONFIG_CNF_STRU * axi_mon_config_cnf = NULL;

    pdata = (VOS_UINT8*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));
    axi_mon_config_cnf = DRV_AXI_MON_CONFIG(pdata, (VOS_UINT*)&cnf_data_len);

    vos_printf("diag_BspAxiMonConfigEntry  cnf_data_len  = 0x%x\n", cnf_data_len);

    if(VOS_NULL != axi_mon_config_cnf)
    {
        ret = diag_AgentCnfFun((VOS_UINT8*)axi_mon_config_cnf,ulCmdId,cnf_data_len);
        free(axi_mon_config_cnf);
    }
    else
    {
        vos_printf("diag_BspAxiMonConfigEntry  ERROR\n");
        ret = ERR_MSP_FAILURE;
    }

    return ret;
}


VOS_UINT32 diag_BspAxiMonTerminateEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
VOS_UINT32 diag_BspAxiMonTerminateEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId)
{
    VOS_UINT32  ret ;
    VOS_UINT32 cnf_data_len = 0;
    VOS_UINT8 *pdata =NULL;
    AXI_MON_TERMINATE_CNF_STRU * axi_ter_cnf = NULL;

    pdata = (VOS_UINT8*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));
    axi_ter_cnf = DRV_AXI_MON_TERMINATE(pdata, (VOS_UINT*)&cnf_data_len);

    vos_printf("diag_BspAxiMonTerminateEntry  cnf_data_len  = 0x%x\n", cnf_data_len);

    if(VOS_NULL != axi_ter_cnf)
    {
        ret = diag_AgentCnfFun((VOS_UINT8*)axi_ter_cnf,ulCmdId,cnf_data_len);
        free(axi_ter_cnf);
    }
    else
    {
        vos_printf("diag_BspAxiMonTerminateEntry  ERROR\n");
        ret = ERR_MSP_FAILURE;
    }

    return ret;
}


VOS_UINT32 diag_BspAxiProcEntry (VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
VOS_UINT32 diag_BspAxiProcEntry (VOS_UINT8* pstReq , VOS_UINT32 ulCmdId)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

	VOS_UINT8 *pdata =NULL;
	DIAG_CMD_COMMON_CNF stAxiCnf ={0};

    diag_printf("[%s enter!]\n",__FUNCTION__);
    pdata = (VOS_UINT8*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    switch(MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId))
    {
        case DIAG_CMD_AXI_DATA_CONFIG:
            ret = diag_BspAxiDataConfigEntry(pstReq, ulCmdId);
            break;
        case DIAG_CMD_AXI_REG_CONFIG:
            ret = DRV_AXI_REG_CONFIG(pdata);
            break;
        case DIAG_CMD_AXI_DATA_CTRL:
            ret = DRV_AXI_DATA_CTRL(pdata);
            break;
        case DIAG_CMD_AXI_MON_CONFIG:
            ret = diag_BspAxiMonConfigEntry(pstReq, ulCmdId);
            break;
        case DIAG_CMD_AXI_MON_START:
            ret = DRV_AXI_MON_START(pdata);
            break;
        case DIAG_CMD_AXI_MON_TERMINATE:
            ret = diag_BspAxiMonTerminateEntry(pstReq, ulCmdId);
            break;
        default:
            break;

    }

	stAxiCnf.ulRet = ret;
	ret = diag_AgentCnfFun((VOS_UINT8*)&stAxiCnf,ulCmdId,sizeof(DIAG_CMD_COMMON_CNF));

    return ret;
}


VOS_UINT32 diag_BspProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId)
{
    VOS_UINT32 ret;

    switch (MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId))
    {
        case DIAG_CMD_BSP_LOG_SET_ACORE:
        case DIAG_CMD_BSP_LOG_SET_CCORE:
            ret = diag_BspLogProcEntry(pstReq,ulCmdId);

            break;

        case DIAG_CMD_BSP_SYSVIEW_SWT_ACORE:
        case DIAG_CMD_BSP_SYSVIEW_SWT_CCORE:
        case DIAG_CMD_BSP_CPU_SWT_ACORE:
        case DIAG_CMD_BSP_CPU_SWT_CCORE:
            ret = diag_BspSysviewProcEntry(pstReq,ulCmdId);

            break;

		case DIAG_CMD_AXI_DATA_CONFIG:
        case DIAG_CMD_AXI_REG_CONFIG:
        case DIAG_CMD_AXI_DATA_CTRL:
        case DIAG_CMD_AXI_MON_CONFIG:
        case DIAG_CMD_AXI_MON_START:
        case DIAG_CMD_AXI_MON_TERMINATE:
            ret = diag_BspAxiProcEntry(pstReq,ulCmdId);
            break;

        default:
            ret = ERR_MSP_FAILURE;
            break ;
    }

    return ret;
}



VOS_UINT32 diag_BspLogProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId)
{
    VOS_UINT32  ret = ERR_MSP_SUCCESS;
    VOS_UINT32 data_len = 0;
    VOS_UINT32 cnf_data_len = 0;
    MSP_DIAG_HEAD_STRU *pstDiagHead         = VOS_NULL;
    DIAG_bsp_log_swt_cfg_s *pstLogSet = VOS_NULL;
    DIAG_BSP_PRINT_LOG_SWT_CNF_STRU  stLogSetCnf  = {0};
    VOS_UINT8 *pstCnf = VOS_NULL;

    pstDiagHead = (MSP_DIAG_HEAD_STRU*)(DIAG_OFFSET_SOCP_GET_DIAG_HEAD(pstReq));

    vos_printf("diag_BspLogProcEntry  ulCmdId = 0x%x\n",ulCmdId);

    if(DIAG_CMD_BSP_LOG_SET_ACORE == (MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId)) )
    {
        ret = ERR_MSP_SUCCESS;
    }
    else if(DIAG_CMD_BSP_LOG_SET_CCORE == (MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId)) )
    {
        pstLogSet = (DIAG_bsp_log_swt_cfg_s *)DIAG_OFFSET_HEAD_GET_DATA(pstReq);
        data_len = pstDiagHead->ulDataSize  - sizeof(MSP_DIAG_DATA_REQ_STRU);
        stLogSetCnf.ulRet  = DRV_LOG_LVL_SET(pstLogSet,data_len);

        pstCnf = (VOS_UINT8 *)&stLogSetCnf ;
        cnf_data_len = sizeof(DIAG_BSP_PRINT_LOG_SWT_CNF_STRU);
    }
    else
    {
        ret = ERR_MSP_FAILURE;
    }

     if(VOS_NULL!=pstCnf)
    {
        ret = diag_AgentCnfFun(pstCnf,ulCmdId,cnf_data_len);
    }
	return ret;
}


VOS_UINT32 diag_BspSysviewProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId)
{
    VOS_UINT32  ret ;
    VOS_UINT32 cnf_data_len = 0;

    DIAG_BSP_SYVIEW_SWT_CFG_STRU *pstSysviewSet = VOS_NULL;
    DIAG_BSP_SYVIEW_SWT_CNF_STRU  *pstLogSysviewCnf  = {0};
    VOS_UINT8 *pstCnf = VOS_NULL;



    if((DIAG_CMD_BSP_SYSVIEW_SWT_ACORE == (MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId)))
        ||(DIAG_CMD_BSP_CPU_SWT_ACORE == (MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId))))
    {
        ret = ERR_MSP_SUCCESS;
    }
    else if((DIAG_CMD_BSP_SYSVIEW_SWT_CCORE == (MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId)) )
                ||(DIAG_CMD_BSP_CPU_SWT_CCORE == (MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId))))
    {

        pstSysviewSet = (DIAG_BSP_SYVIEW_SWT_CFG_STRU *)DIAG_OFFSET_HEAD_GET_DATA(pstReq);

         ret= DRV_SYSVIEW_SWT_SET(pstSysviewSet->trace_type,pstSysviewSet->trace_swt,pstSysviewSet->period);

        if(ret  == ERR_MSP_SUCCESS)
        {
            if((pstSysviewSet->trace_type ==SYSVIEW_TASK_INFO )&&(pstSysviewSet->trace_swt ==1 ))
            {
                cnf_data_len = sizeof(DIAG_BSP_SYVIEW_SWT_CNF_STRU)+sizeof(DIAG_BSP_TASK_INFO_STRU)*128;
                pstLogSysviewCnf =(DIAG_BSP_SYVIEW_SWT_CNF_STRU*)VOS_MemAlloc(MSP_PID_DIAG_AGENT, DYNAMIC_MEM_PT,cnf_data_len);
                pstLogSysviewCnf->ullen = sizeof(DIAG_BSP_TASK_INFO_STRU)*128;
                DRV_GET_ALL_TASK_ID_NAME((pstLogSysviewCnf->st_task_info), sizeof(DIAG_BSP_TASK_INFO_STRU)*128);
            }
            else
            {
                cnf_data_len = sizeof(DIAG_BSP_SYVIEW_SWT_CNF_STRU);
                pstLogSysviewCnf =(DIAG_BSP_SYVIEW_SWT_CNF_STRU*)VOS_MemAlloc(MSP_PID_DIAG_AGENT, DYNAMIC_MEM_PT,cnf_data_len);
                pstLogSysviewCnf->ullen  = 0;
            }
        }
        else
        {
            cnf_data_len = sizeof(DIAG_BSP_SYVIEW_SWT_CNF_STRU);
            pstLogSysviewCnf =(DIAG_BSP_SYVIEW_SWT_CNF_STRU*)VOS_MemAlloc(MSP_PID_DIAG_AGENT, DYNAMIC_MEM_PT,cnf_data_len);
            pstLogSysviewCnf->ullen  = 0;

        }

        pstLogSysviewCnf->ulRet = ret;
        pstLogSysviewCnf->trace_type =pstSysviewSet->trace_type;
        pstCnf = (VOS_UINT8 *)pstLogSysviewCnf ;

    }
    else
    {
        ret = ERR_MSP_FAILURE;
    }

    if(VOS_NULL!=pstCnf)
    {
        ret = diag_AgentCnfFun(pstCnf,ulCmdId,cnf_data_len);

         VOS_MemFree(diag_GetAgentPid(), pstCnf);
    }

    return ret;
}
#endif


VOS_UINT32 diag_DspTransCnfProc(VOS_UINT8 * aucSocpPacket)
{
    DIAG_CMD_DSP_CNF_STRU stRttCnf = {0};
	VOS_UINT32 ret = 0;
    VOS_UINT32 ulCmdId = 0;
	MSP_DIAG_HEAD_STRU *pstDspMsg = NULL;
    VOS_UINT32 ulLen = 0;
	VOS_UINT8* pData = NULL;

    ulCmdId = (((MSP_DIAG_HEAD_STRU*)DIAG_OFFSET_SOCP_GET_DIAG_HEAD(aucSocpPacket))->ulID);

    /* DSP消息的ID和size */
    pstDspMsg = (MSP_DIAG_HEAD_STRU*)(aucSocpPacket+sizeof(MSP_SOCP_HEAD_STRU));

    ulLen = sizeof(VOS_UINT32) + (pstDspMsg->ulDataSize - DRA_RTT_NOT_NEED_CHARS_LEN);

    pData = VOS_MemAlloc(MSP_PID_DIAG_AGENT,DYNAMIC_MEM_PT,ulLen);
    if(NULL == pData)
    {
        return ERR_MSP_MALLOC_FAILUE;
    }

    /* 写入消息ID */
    *(VOS_UINT32*)(pData) = pstDspMsg->ulID;

    /* 写入消息内容 */
    VOS_MemCpy((pData + sizeof(VOS_UINT32)), (aucSocpPacket + DIAG_FULL_HEAD_LEN), (pstDspMsg->ulDataSize - DRA_RTT_NOT_NEED_CHARS_LEN));

	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_LTE_DSP_CNF,0,0,0);

    ret = BSP_MailBox_ComMsgWrite(EN_MAILBOX_SERVICE_LTE_OM, pData, ulLen, EN_MAILBOX_SLEEP_WAKEUP);
    if(ERR_MSP_SUCCESS != ret)
    {
        /* 开机时DSP未启动时，HSO下发DSP操作导致写失败，避免打印刷屏，屏蔽打印 */
//        printf("write BSP_MailBox_ComMsgWrite fail!\n");
    }

    VOS_MemFree(MSP_PID_DIAG_AGENT,pData);

    /*模拟DSP回复*/
    stRttCnf.ulRc = ERR_MSP_SUCCESS;
    ret = diag_AgentCnfFun((VOS_UINT8*)&stRttCnf,ulCmdId,sizeof(DIAG_CMD_DSP_CNF_STRU));
    return ret;
}
VOS_UINT32 diag_BbpProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

    switch(MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId))
    {
        case DIAG_CMD_REG_RD:
            ret = diag_RegRdProc(pstReq,ulCmdId);
            break;
        case DIAG_CMD_REG_WR:
            ret = diag_RegWrProc(pstReq,ulCmdId);
            break;
        case DIAG_CMD_DRX_DATA_SAMPLE_REG_WR:
            ret = diag_DrxDataSampleProc(pstReq,ulCmdId);
            break;
	    case DIAG_CMD_DRX_LOG_SAMPLE_REG_WR:
            ret = diag_DrxLogSampleProc(pstReq,ulCmdId);
            break;
		case DIAG_CMD_DRX_SAMPLE_GEN_REQ:
			ret = diag_DrxSampleGenProc(pstReq,ulCmdId);
            break;
		case DIAG_CMD_DRX_SAMPLE_ADDR_REQ:
			ret = diag_DrxSampleGetAddrProc(pstReq,ulCmdId);
            break;
		case DIAG_CMD_DRX_SAMPLE_CHNSIZE_REQ:
			ret = diag_DrxSampleGetChnSizeProc(pstReq,ulCmdId);
            break;
		case DIAG_CMD_DRX_SAMPLE_GET_VERSION_REQ:
			ret = diag_DrxSampleGetVersionProc(pstReq,ulCmdId);
            break;
		case DIAG_CMD_DRX_SAMPLE_ABLE_CHN_REQ:
			ret = diag_DrxSampleAbleChnProc(pstReq,ulCmdId);
            break;
        default:
            ret = ERR_MSP_INVALID_PARAMETER;
            break;
    }

    return ret;
}


VOS_UINT32 diag_SetRegValue(DIAG_CMD_REG_WR_PARA_STRU * pWrtPara)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT32 ulRdRegValue = 0;
    VOS_UINT32 i = 0;
    VOS_UINT32 ulBitTempValue = 0;

    if (pWrtPara->ucBeginBit > 31 || pWrtPara->ucEndBit > 31
        || pWrtPara->ucBeginBit > pWrtPara->ucEndBit) /* 参数错误*/
    {
        return ERR_MSP_INVALID_PARAMETER;
    }

    if(((VOS_UINT32)0) != pWrtPara->ulAddr)
    {
        ret = (VOS_UINT32)diag_RegRead(pWrtPara->ulAddr,ADDRTYPE32BIT,&ulRdRegValue);
        if (ERR_MSP_SUCCESS == ret)
        {
            for (i=0; i < (VOS_UINT32)(pWrtPara->ucEndBit-pWrtPara->ucBeginBit+1); i++)
            {
                ulBitTempValue |= ((VOS_UINT)1<<(pWrtPara->ucBeginBit+i));
            }

            ulBitTempValue = (~ulBitTempValue);
            ulBitTempValue = (ulBitTempValue&ulRdRegValue); /* 留空需要设置的BIT位*/

            /* 要求设置的REG VALUE值必须要是按位来给的*/
            ulBitTempValue = ((pWrtPara->ulRegValue << pWrtPara->ucBeginBit) | ulBitTempValue);
            ret = (VOS_UINT)diag_RegWrite((pWrtPara->ulAddr),ADDRTYPE32BIT,ulBitTempValue);
        }
    }
    else
    {
        ret = ERR_MSP_INVALID_PARAMETER;
    }

    return ret;
}
VOS_UINT32 diag_RegRdProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
	DIAG_CMD_REG_RD_REQ_STRU* pstRegQryReq = NULL;
    DIAG_CMD_REG_RD_CNF_PARA_STRU *pstRegQryCnf = NULL;
    DIAG_CMD_REG_RD_CNF_PARA_STRU *pstTempCnf = NULL;
    MSP_DIAG_HEAD_STRU *pstDiagHead         = NULL;
    VOS_UINT32 *pulReqRdAddr                = NULL;
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT32 ulNum  = 0;
    VOS_UINT32 i = 0;
    VOS_UINT32 ulMsgLen = 0;

    pstDiagHead = (MSP_DIAG_HEAD_STRU*)(DIAG_OFFSET_SOCP_GET_DIAG_HEAD(pstReq));
    pstRegQryReq = (DIAG_CMD_REG_RD_REQ_STRU*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    ulNum = (pstDiagHead->ulDataSize - sizeof(MSP_DIAG_DATA_REQ_STRU)) / sizeof(VOS_UINT32);
    ulMsgLen = sizeof(DIAG_CMD_REG_RD_CNF_PARA_STRU) * ulNum;


	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_REG_RD,ulNum,0,0);

    pstRegQryCnf = (DIAG_CMD_REG_RD_CNF_PARA_STRU*)VOS_MemAlloc(MSP_PID_DIAG_AGENT, DYNAMIC_MEM_PT,ulMsgLen);
    if(NULL == pstRegQryCnf)
    {
        return ERR_MSP_MALLOC_FAILUE;
    }

    pstTempCnf = pstRegQryCnf;

    for (i = 0; i < ulNum; i++)
    {
        /*lint -save -e740*/
        pulReqRdAddr = ((VOS_UINT32*)pstRegQryReq) + i;
        /*lint -restore*/
        ret = (VOS_UINT32)diag_RegRead(*pulReqRdAddr, ADDRTYPE32BIT, &(pstRegQryCnf->ulRegValue));
		if(ERR_MSP_SUCCESS == ret)
		{
			pstTempCnf->ulRc = ret;
			pstTempCnf->ulAddr =*pulReqRdAddr;
		}
        pstTempCnf++;
    }

    ret = diag_AgentCnfFun((VOS_UINT8*)pstRegQryCnf,ulCmdId,ulMsgLen);

    VOS_MemFree(MSP_PID_DIAG_AGENT,pstRegQryCnf);

    return ret;


}



VOS_UINT32 diag_RegWrProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
	DIAG_CMD_REG_WR_REQ_STRU* pstRegWRReq = NULL;
	DIAG_CMD_REG_WR_PARA_STRU* pstReqWrStru     = NULL;
    DIAG_CMD_REG_WR_CNF_PARA_STRU *pstCnfWrStru = NULL;
    DIAG_CMD_REG_WR_CNF_PARA_STRU *pstTempCnf = NULL;
    MSP_DIAG_HEAD_STRU *pstDiagHead             = NULL;
    VOS_UINT32 ulNum  = 0;
    VOS_UINT32 i = 0;
    VOS_UINT32 ret = 0;
    VOS_UINT32 ulMsgLen =0,ulLen = 0;

    /*入参判断*/
    pstDiagHead = (MSP_DIAG_HEAD_STRU*)(DIAG_OFFSET_SOCP_GET_DIAG_HEAD(pstReq));
    pstRegWRReq = (DIAG_CMD_REG_WR_REQ_STRU*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    ulLen = (pstDiagHead->ulDataSize - sizeof(MSP_DIAG_DATA_REQ_STRU));
    if((0 != ulLen % sizeof(DIAG_CMD_REG_WR_PARA_STRU)) ||(0 == ulLen))
    {
        return ERR_MSP_INVALID_PARAMETER;
    }

    ulNum = (pstDiagHead->ulDataSize - sizeof(MSP_DIAG_DATA_REQ_STRU)) / sizeof(DIAG_CMD_REG_WR_PARA_STRU);
    ulMsgLen = sizeof(DIAG_CMD_REG_WR_CNF_PARA_STRU) * ulNum;

	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_NV_WR,ulNum,0,0);

    pstCnfWrStru = (DIAG_CMD_REG_WR_CNF_PARA_STRU*)VOS_MemAlloc(MSP_PID_DIAG_AGENT, DYNAMIC_MEM_PT,ulMsgLen);
    if(NULL == pstCnfWrStru)
    {
        return ERR_MSP_MALLOC_FAILUE;
    }

    pstTempCnf = pstCnfWrStru;
    for (i = 0; i < ulNum; i++)
    {
        /*lint -save -e740*/
        pstReqWrStru = (DIAG_CMD_REG_WR_PARA_STRU*)pstRegWRReq + i;
        /*lint -restore*/
        /* 组包命令参数*/
        ret = diag_SetRegValue(pstReqWrStru);

        pstTempCnf->ulAddr = pstReqWrStru->ulAddr;
        pstTempCnf->ulRc   = ret;
        pstTempCnf ++;
    }

    /*打包回复给FW*/
    ret = diag_AgentCnfFun((VOS_UINT8*)pstCnfWrStru,ulCmdId,ulMsgLen);

     VOS_MemFree(MSP_PID_DIAG_AGENT,pstCnfWrStru);

    return ret;
}


/*x(old)|y(old)|z(old) --->x(old)|reg_value(new)|z(old),,,0x10(end=6)|0010(reg_value=2)|(begin=2)10*/


VOS_UINT32 diag_DrxDataSampleProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
	DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU *psDataSample = NULL;
	DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_CNF_STRU stCnfDataSample = {0};
	VOS_UINT32 ret = 0;
    VOS_UINT32 ulDataLen = 0;

	psDataSample = (DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    ulDataLen = ((MSP_DIAG_HEAD_STRU*)(DIAG_OFFSET_SOCP_GET_DIAG_HEAD(pstReq)))->ulDataSize - sizeof(MSP_DIAG_DATA_REQ_STRU);

	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_BBP_SAMPLE,psDataSample->enBBPDataSampleCtrl,0,0);

	if(BBP_DATA_SAMPLE_START == psDataSample->enBBPDataSampleCtrl)
	{
        ret = diag_SendMsg(MSP_PID_DIAG_AGENT, MSP_PID_BBP_AGENT,ID_MSG_REQ_DRX_DATA_SAMPLE_START, \
        (VOS_UINT8*)psDataSample, ulDataLen);
    }
	else if(BBP_DATA_SAMPLE_STOP == psDataSample->enBBPDataSampleCtrl)
	{
        ret = diag_SendMsg(MSP_PID_DIAG_AGENT, MSP_PID_BBP_AGENT,ID_MSG_REQ_DRX_DATA_SAMPLE_STOP, \
        (VOS_UINT8*)psDataSample, ulDataLen);
    }
    else
    {
        ret = ERR_MSP_FAILURE;
    }

    /*打包回复给FW*/
    stCnfDataSample.ulRet = ret;
    ret = diag_AgentCnfFun((VOS_UINT8*)&stCnfDataSample,ulCmdId,sizeof(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_CNF_STRU));

	return ret;
}
VOS_UINT32 diag_DrxLogSampleProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU *psLogSample = NULL;
	DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_CNF_STRU stCnfLogSample = {0};
	VOS_UINT32 ret = 0;
    VOS_UINT32 ulDataLen = 0;

	psLogSample = (DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    ulDataLen = ((MSP_DIAG_HEAD_STRU*)(DIAG_OFFSET_SOCP_GET_DIAG_HEAD(pstReq)))->ulDataSize - sizeof(MSP_DIAG_DATA_REQ_STRU);

	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_BBP_SAMPLE,psLogSample->enBBPLogCtrl,0,0);

    if(BBP_LOG_SAMPLE_START == psLogSample->enBBPLogCtrl)
	{
        ret = diag_SendMsg(MSP_PID_DIAG_AGENT, MSP_PID_BBP_AGENT,ID_MSG_REQ_DRX_LOG_SAMPLE_START, \
        (VOS_UINT8*)psLogSample,ulDataLen);
    }
	else if(BBP_LOG_SAMPLE_STOP == psLogSample->enBBPLogCtrl)
	{
        ret = diag_SendMsg(MSP_PID_DIAG_AGENT, MSP_PID_BBP_AGENT,ID_MSG_REQ_DRX_LOG_SAMPLE_STOP, \
        (VOS_UINT8*)psLogSample, ulDataLen);
    }
    else
    {
        ret = ERR_MSP_FAILURE;
    }

    /*打包回复给FW*/
    /*lint -save -e438*/
    stCnfLogSample.ulRet = ret;
    ret = diag_AgentCnfFun((VOS_UINT8*)&stCnfLogSample,ulCmdId,sizeof(DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_CNF_STRU));
    if(ret != ERR_MSP_SUCCESS)
    {
        return ERR_MSP_FAILURE;
    }
    /*lint -save -e438*/
	return ERR_MSP_SUCCESS;

}
VOS_UINT32 ulDrxSampleGenEnterCnt = 0;
VOS_UINT32 ulDrxSampleGenExitCnt  = 0;
VOS_UINT32 ulDrxSampleGetAddrEnterCnt = 0;
VOS_UINT32 ulDrxSampleGetAddrExitCnt  = 0;
VOS_UINT32 ulDrxSampleGetChnSizeEnterCnt = 0;
VOS_UINT32 ulDrxSampleGetChnSizeExitCnt  = 0;
VOS_UINT32 ulDrxSampleCfgChnAddrEnterCnt = 0;
VOS_UINT32 ulDrxSampleCfgChnAddrExitCnt  = 0;
VOS_UINT32 ulDrxSampleAbleChnEnterCnt = 0;
VOS_UINT32 ulDrxSampleAbleChnExitCnt  = 0;

VOS_VOID Diag_SamplePrint(VOS_VOID);
VOS_VOID Diag_SamplePrint(VOS_VOID)
{
	diag_printf("ulDrxSampleGenEnterCnt is 0x%x!\n",ulDrxSampleGenEnterCnt);
	diag_printf("ulDrxSampleGenExitCnt is 0x%x!\n",ulDrxSampleGenExitCnt);
	diag_printf("ulDrxSampleGetAddrEnterCnt is 0x%x!\n",ulDrxSampleGetAddrEnterCnt);
	diag_printf("ulDrxSampleGetAddrExitCnt is 0x%x!\n",ulDrxSampleGetAddrExitCnt);
	diag_printf("ulDrxSampleGetChnSizeEnterCnt is 0x%x!\n",ulDrxSampleGetChnSizeEnterCnt);
	diag_printf("ulDrxSampleGetChnSizeExitCnt is 0x%x!\n",ulDrxSampleGetChnSizeExitCnt);
	diag_printf("ulDrxSampleCfgChnAddrEnterCnt is 0x%x!\n",ulDrxSampleCfgChnAddrEnterCnt);
	diag_printf("ulDrxSampleCfgChnAddrExitCnt is 0x%x!\n",ulDrxSampleCfgChnAddrExitCnt);
	diag_printf("ulDrxSampleAbleChnEnterCnt is 0x%x!\n",ulDrxSampleAbleChnEnterCnt);
	diag_printf("ulDrxSampleAbleChnExitCnt is 0x%x!\n",ulDrxSampleAbleChnExitCnt);
}
VOS_UINT32 diag_DrxSampleGenProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    DIAG_CMD_DRX_SAMPLE_REG_WR_REQ_STRU *psDrxSample = NULL;
	DIAG_CMD_DRX_SAMPLE_REG_WR_CNF_STRU stCnfDrxSample = {0};
	VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT32 ulDataLen = 0;

	ulDrxSampleGenEnterCnt ++;
	psDrxSample = (DIAG_CMD_DRX_SAMPLE_REG_WR_REQ_STRU*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    ulDataLen = ((MSP_DIAG_HEAD_STRU*)(DIAG_OFFSET_SOCP_GET_DIAG_HEAD(pstReq)))->ulDataSize - sizeof(MSP_DIAG_DATA_REQ_STRU);

	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_BBP_SAMPLE,psDrxSample->ulOmDrxSampleId,0,0);

    ret = BSP_MailBox_ComMsgWrite(EN_MAILBOX_SERVICE_LTE_HS_DIAG, psDrxSample, ulDataLen, EN_MAILBOX_SLEEP_WAKEUP);
	if(ret != VOS_OK)
	{
		ret = ERR_MSP_FAILURE;
	}

	ulDrxSampleGenExitCnt ++;

	/*打包回复给FW*/
    stCnfDrxSample.ulRet = ret;
    ret = diag_AgentCnfFun((VOS_UINT8*)&stCnfDrxSample, ulCmdId,sizeof(DIAG_CMD_DRX_SAMPLE_REG_WR_CNF_STRU));

	return ret;

}
VOS_UINT32 diag_DrxSampleGetAddrProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    DIAG_CMD_DRX_SAMPLE_GET_ADDR_REQ_STRU *psDrxSample = NULL;
	DIAG_CMD_DRX_SAMPLE_GET_ADDR_CNF_STRU stCnfDrxSample = {0};
	VOS_UINT32 ret = ERR_MSP_SUCCESS;

	VOS_UINT32 ulAddrType 	= 0;


	ulDrxSampleGetAddrEnterCnt ++;
	psDrxSample = (DIAG_CMD_DRX_SAMPLE_GET_ADDR_REQ_STRU*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));


	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_BBP_SAMPLE,psDrxSample->eDiagDrxSampleAddr,0,0);

	ulAddrType = psDrxSample->eDiagDrxSampleAddr;

    stCnfDrxSample.ulDrxSampleType = ulAddrType;
	/* 获取对应的寄存器地址*/
	switch (ulAddrType)
	{
		case DRX_SAMPLE_BBP_DMA_BASE_ADDR:
			stCnfDrxSample.ulDrxSampleAddr = (VOS_UINT32)DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_BBPDMA);
			/* add code here */
			break;
		case DRX_SAMPLE_BBP_DBG_BASE_ADDR:
			stCnfDrxSample.ulDrxSampleAddr = (VOS_UINT32)DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_BBPDBG);
			/* add code here */
			break;
		case DRX_SAMPLE_BBP_SRC_BASE_ADDR:
			stCnfDrxSample.ulDrxSampleAddr = (VOS_UINT32)DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_BBPSRC);
			/* add code here */
			break;
		case DRX_SAMPLE_POW_ONOFF_CLK_BASE_ADDR:
			stCnfDrxSample.ulDrxSampleAddr = (VOS_UINT32)DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_SYSCTRL);
			/* add code here */
			break;
		case DRX_SAMPLE_SOCP_BASE_ADDR:
			stCnfDrxSample.ulDrxSampleAddr = (VOS_UINT32)DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_SOCP);
			/* add code here */
			break;
		default:
			break;
	}

	ulDrxSampleGetAddrExitCnt ++;

	/*打包回复给FW*/
    stCnfDrxSample.ulRet = VOS_OK;
    ret = diag_AgentCnfFun((VOS_UINT8*)&stCnfDrxSample, ulCmdId, sizeof(DIAG_CMD_DRX_SAMPLE_GET_ADDR_CNF_STRU));

	return ret;

}
VOS_UINT32 diag_DrxSampleGetChnSizeProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
	DIAG_CMD_DRX_SAMPLE_GET_CHNSIZE_REQ_STRU *psDrxSample = NULL;
	DIAG_CMD_DRX_SAMPLE_GET_CHNSIZE_CNF_STRU stCnfDrxSample = {0};
	VOS_UINT32 ret = ERR_MSP_SUCCESS;

	VOS_UINT32 ulAddrType = 0;

	ulDrxSampleGetChnSizeEnterCnt ++;
	psDrxSample = (DIAG_CMD_DRX_SAMPLE_GET_CHNSIZE_REQ_STRU*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));


	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_BBP_SAMPLE,psDrxSample->eDiagDrxSampleChnSize,0,0);

	ulAddrType = psDrxSample->eDiagDrxSampleChnSize;
	switch (ulAddrType)
	{
		case DRX_SAMPLE_BBP_DMA_LOG0_CHNSIZE:
			stCnfDrxSample.ulChnAddr = BBP_LOG0_MEM_ADDR;
            stCnfDrxSample.ulChnSize = BBP_LOG0_MEM_SIZE;
			break;
		case DRX_SAMPLE_BBP_DMA_LOG1_CHNSIZE:
			stCnfDrxSample.ulChnAddr = BBP_LOG1_MEM_ADDR;
            stCnfDrxSample.ulChnSize = BBP_LOG1_MEM_SIZE;
			break;
		case DRX_SAMPLE_BBP_DMA_LOG2_CHNSIZE:
			stCnfDrxSample.ulChnAddr = BBP_LOG2_MEM_ADDR;
            stCnfDrxSample.ulChnSize = BBP_LOG2_MEM_SIZE;
			break;
		case DRX_SAMPLE_BBP_DMA_LOG3_CHNSIZE:
			stCnfDrxSample.ulChnAddr = BBP_LOG3_MEM_ADDR;
            stCnfDrxSample.ulChnSize = BBP_LOG3_MEM_SIZE;
            break;
		case DRX_SAMPLE_BBP_DMA_LOG4_CHNSIZE:
			stCnfDrxSample.ulChnAddr = BBP_LOG4_MEM_ADDR;
            stCnfDrxSample.ulChnSize = BBP_LOG4_MEM_SIZE;
            break;
		case DRX_SAMPLE_BBP_DMA_LOG5_CHNSIZE:
			stCnfDrxSample.ulChnAddr = BBP_LOG5_MEM_ADDR;
            stCnfDrxSample.ulChnSize = BBP_LOG5_MEM_SIZE;
            break;
		case DRX_SAMPLE_BBP_DMA_LOG6_CHNSIZE:
			stCnfDrxSample.ulChnAddr = BBP_LOG6_MEM_ADDR;
            stCnfDrxSample.ulChnSize = BBP_LOG6_MEM_SIZE;
            break;
		case DRX_SAMPLE_BBP_DMA_LOG7_CHNSIZE:
			stCnfDrxSample.ulChnAddr = BBP_LOG7_MEM_ADDR;
            stCnfDrxSample.ulChnSize = BBP_LOG7_MEM_SIZE;
			break;
		case DRX_SAMPLE_BBP_DMA_DATA_CHNSIZE:
			stCnfDrxSample.ulChnAddr = BBP_DS_MEM_ADDR;
            stCnfDrxSample.ulChnSize = BBP_DS_MEM_SIZE;
			break;
		default:
			break;
	}
	ulDrxSampleGetChnSizeExitCnt ++;

	/*打包回复给FW*/
	stCnfDrxSample.ulRet = ret;
    stCnfDrxSample.ulChnType = ulAddrType;
	ret = diag_AgentCnfFun((VOS_UINT8*)&stCnfDrxSample, ulCmdId,sizeof(DIAG_CMD_DRX_SAMPLE_GET_CHNSIZE_CNF_STRU));

	return ret;

}
VOS_UINT32 diag_DrxSampleGetVersionProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
	DIAG_CMD_DRX_SAMPLE_GET_VERSION_CNF_STRU stCnfDrxSample;
	VOS_UINT32 ret = ERR_MSP_SUCCESS;
	VOS_INT32 value = ERR_MSP_SUCCESS;
    /* coverity[assign_zero] */
	va_list arglist = (va_list)VOS_NULL;


	ulDrxSampleCfgChnAddrEnterCnt ++;


	/* config regs */
    /* coverity[var_deref_model] */
    value = VOS_nvsprintf((VOS_CHAR*)(stCnfDrxSample.ulProductName), DIAG_PRODUCT_VERSION_LENGTH, (VOS_CHAR *)PRODUCT_NAME, arglist);
    if(value < 0){}
    /* coverity[var_deref_model] */
	value = VOS_nvsprintf((VOS_CHAR*)(stCnfDrxSample.ulSolutiongName), DIAG_PRODUCT_VERSION_LENGTH, (VOS_CHAR *)PRODUCT_CFG_CHIP_SOLUTION_NAME, arglist);
    if(value < 0){}
	ulDrxSampleCfgChnAddrExitCnt ++;

	/*打包回复给FW*/
    ret = ERR_MSP_SUCCESS;
	stCnfDrxSample.ulRet = ret;
	ret = diag_AgentCnfFun((VOS_UINT8*)&stCnfDrxSample, ulCmdId,sizeof(DIAG_CMD_DRX_SAMPLE_GET_VERSION_CNF_STRU));

	return ret;

}
VOS_UINT32 diag_DrxSampleAbleChnProc(VOS_UINT8* pstReq, VOS_UINT32 ulCmdId)
{
	DIAG_CMD_DRX_SAMPLE_ABLE_CHN_REQ_STRU *psDrxSample = NULL;
	DIAG_CMD_DRX_SAMPLE_ABLE_CHN_CNF_STRU stCnfDrxSample = {0};
	VOS_UINT32 ret = ERR_MSP_SUCCESS;

	VOS_UINT32 ulChnAbleType = 0;

	ulDrxSampleAbleChnEnterCnt ++;
	psDrxSample = (DIAG_CMD_DRX_SAMPLE_ABLE_CHN_REQ_STRU*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));


	/* 调用socp 通道使能函数打开通道或者关闭通道*/
	ulChnAbleType = psDrxSample->eDiagDrxSampleAbleChn;
	switch (ulChnAbleType)
	{
		case DRX_SAMPLE_SOCP_CHN_ENABLE:
			{
				if(VOS_OK != DRV_SOCP_START(SOCP_CODER_SRC_LBBP9))
				{
					vos_printf("%s: enable channel failed!\n",__FUNCTION__);
				}
			break;
			}
		case DRX_SAMPLE_SOCP_CHN_DISABLE:
			{
				if(VOS_OK != DRV_SOCP_STOP(SOCP_CODER_SRC_LBBP9))
				{
					vos_printf("%s: disable channel failed!\n",__FUNCTION__);
				}
			break;
			}
		default:
			break;
	}
	ulDrxSampleAbleChnExitCnt ++;

	/*打包回复给FW*/
	stCnfDrxSample.ulRet = ret;
	ret = diag_AgentCnfFun((VOS_UINT8*)&stCnfDrxSample, ulCmdId,sizeof(DIAG_CMD_DRX_SAMPLE_ABLE_CHN_CNF_STRU));

	return ret;

}



VOS_UINT32 MSP_DiagFidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ulRelVal = 0;

    switch (ip)
    {
    case VOS_IP_LOAD_CONFIG:

        ulRelVal = VOS_RegisterPIDInfo(MSP_PID_DIAG_AGENT, (Init_Fun_Type) diag_AgentMsgProcInit, (Msg_Fun_Type) diag_AgentMsgProc);

        if (ulRelVal != VOS_OK)
        {
            return VOS_ERR;
        }

        ulRelVal = VOS_RegisterPIDInfo(MSP_PID_BBP_AGENT, (Init_Fun_Type) bbp_AgentMsgProcInit, (Msg_Fun_Type) bbp_AgentMsgProc);

        if (ulRelVal != VOS_OK)
        {
            return VOS_ERR;
        }
        ulRelVal = VOS_RegisterMsgTaskPrio(MSP_FID_DIAG_CCPU, VOS_PRIORITY_M2);
        if (ulRelVal != VOS_OK)
        {
            return VOS_ERR;
        }

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
        ulRelVal = VOS_RegisterSelfTask(MSP_FID_DIAG_CCPU, (VOS_TASK_ENTRY_TYPE)diag_MailboxSelfTask, VOS_PRIORITY_M2, 8192);
        if (VOS_NULL_BYTE  == ulRelVal)
        {
            return VOS_ERR;
        }
#endif
        ulRelVal = VOS_RegisterSelfTask(MSP_FID_DIAG_CCPU, (VOS_TASK_ENTRY_TYPE)bbp_DataLogIndSelfTask, VOS_PRIORITY_M2, 0x2000);
        if (VOS_NULL_BYTE  == ulRelVal)
        {
            return VOS_ERR;
        }
        if(VOS_OK != TLSLEEP_Init())
        {
            return VOS_ERR;
        }
        break;
    default:
         break;
    }

    return VOS_OK;
}

VOS_BOOL ulChanEnableStat = FALSE;
VOS_VOID diag_EnableSocpChan(VOS_VOID)
{
    if(TRUE == ulChanEnableStat)
    {
        return ;
    }
	/*使能BBP LOG通道*/
	DRV_SOCP_ENABLE_LTE_BBP_DSP(SOCP_CODER_SRC_LBBP1);
	DRV_SOCP_ENABLE_LTE_BBP_DSP(SOCP_CODER_SRC_LBBP2);
	DRV_SOCP_ENABLE_LTE_BBP_DSP(SOCP_CODER_SRC_LBBP3);
	DRV_SOCP_ENABLE_LTE_BBP_DSP(SOCP_CODER_SRC_LBBP4);
	DRV_SOCP_ENABLE_LTE_BBP_DSP(SOCP_CODER_SRC_LBBP5);
	DRV_SOCP_ENABLE_LTE_BBP_DSP(SOCP_CODER_SRC_LBBP6);
	DRV_SOCP_ENABLE_LTE_BBP_DSP(SOCP_CODER_SRC_LBBP7);
	DRV_SOCP_ENABLE_LTE_BBP_DSP(SOCP_CODER_SRC_LBBP8);

    ulChanEnableStat = TRUE;
}

VOS_VOID diag_InitSocpChan(VOS_VOID)
{
	/*初始化DSP*/
	DRV_SOCP_INIT_LTE_DSP(SOCP_CODER_SRC_LDSP1,LPHY_MAILBOX_HDLC_BUF_BASE_ADDR,LPHY_MAILBOX_HDLC_BUF_SIZE);
	/*初始化BBP LOG通道*/
	DRV_SOCP_INIT_LTE_BBP_LOG(SOCP_CODER_SRC_LBBP1,BBP_LOG0_MEM_ADDR,BBP_LOG0_MEM_SIZE);
	DRV_SOCP_INIT_LTE_BBP_LOG(SOCP_CODER_SRC_LBBP2,BBP_LOG1_MEM_ADDR,BBP_LOG1_MEM_SIZE);
	DRV_SOCP_INIT_LTE_BBP_LOG(SOCP_CODER_SRC_LBBP3,BBP_LOG2_MEM_ADDR,BBP_LOG2_MEM_SIZE);
	DRV_SOCP_INIT_LTE_BBP_LOG(SOCP_CODER_SRC_LBBP4,BBP_LOG3_MEM_ADDR,BBP_LOG3_MEM_SIZE);
	DRV_SOCP_INIT_LTE_BBP_LOG(SOCP_CODER_SRC_LBBP5,BBP_LOG4_MEM_ADDR,BBP_LOG4_MEM_SIZE);
	DRV_SOCP_INIT_LTE_BBP_LOG(SOCP_CODER_SRC_LBBP6,BBP_LOG5_MEM_ADDR,BBP_LOG5_MEM_SIZE);
	DRV_SOCP_INIT_LTE_BBP_LOG(SOCP_CODER_SRC_LBBP7,BBP_LOG6_MEM_ADDR,BBP_LOG6_MEM_SIZE);
	DRV_SOCP_INIT_LTE_BBP_LOG(SOCP_CODER_SRC_LBBP8,BBP_LOG7_MEM_ADDR,BBP_LOG7_MEM_SIZE);
	/*初始化BBP DS通道*/
	DRV_SOCP_INIT_LTE_BBP_DS(SOCP_CODER_SRC_LBBP9,BBP_DS_MEM_ADDR,BBP_DS_MEM_SIZE);
}

VOS_VOID diag_FixMemInfo(VOS_VOID);
VOS_VOID diag_FixMemInfo(VOS_VOID)
{
    diag_printf("BBP Channel   1 ,addr : 0x%x ,size : 0x%x\n",BBP_LOG0_MEM_ADDR,BBP_LOG0_MEM_SIZE);
    diag_printf("BBP Channel   2 ,addr : 0x%x ,size : 0x%x\n",BBP_LOG1_MEM_ADDR,BBP_LOG1_MEM_SIZE);
    diag_printf("BBP Channel   3 ,addr : 0x%x ,size : 0x%x\n",BBP_LOG2_MEM_ADDR,BBP_LOG2_MEM_SIZE);
    diag_printf("BBP Channel   4 ,addr : 0x%x ,size : 0x%x\n",BBP_LOG3_MEM_ADDR,BBP_LOG3_MEM_SIZE);
    diag_printf("BBP Channel   5 ,addr : 0x%x ,size : 0x%x\n",BBP_LOG4_MEM_ADDR,BBP_LOG4_MEM_SIZE);
    diag_printf("BBP Channel   6 ,addr : 0x%x ,size : 0x%x\n",BBP_LOG5_MEM_ADDR,BBP_LOG5_MEM_SIZE);
    diag_printf("BBP Channel   7 ,addr : 0x%x ,size : 0x%x\n",BBP_LOG6_MEM_ADDR,BBP_LOG6_MEM_SIZE);
    diag_printf("BBP Channel   8 ,addr : 0x%x ,size : 0x%x\n",BBP_LOG7_MEM_ADDR,BBP_LOG7_MEM_SIZE);
    diag_printf("BBP Channel   9 ,addr : 0x%x ,size : 0x%x\n",BBP_DS_MEM_ADDR,BBP_DS_MEM_SIZE);
}
/*lint -restore*/
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

