/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : omnosig.c
  版 本 号   : 初稿
  作    者   : 甘兰 47350
  生成日期   : 2011年9月27日
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2011年9月27日
    作    者   : 甘兰 47350
    修改内容   : 创建文件

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "omprivate.h"
#include "omnosig.h"
#include "hpacomm.h"
#include "wasphycomminterface.h"
#include "PhyGrmInterface.h"
#include "PhyGasInterface.h"
#include "WttfDspInterface.h"
/*#include "NVIM_Interface.h"
 */
#include "hpamailboxdef.h"
#include "TtfOamInterface.h"

#define    THIS_FILE_ID        PS_FILE_ID_OM_NO_SIG_C


#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

#if (VOS_WIN32 == VOS_OS_VER)

VOS_UINT8 g_aucPhyDataReqAddr[1024];
#define  WTTFPHY_MAC_PHY_DATA_REQ_ADDR  (VOS_UINT32)(&g_aucPhyDataReqAddr[0])

#endif


/* 保存R99 Mailbox VTB 译码数据 */
VOS_UINT8   g_aucOmR99MBData[OM_R99_MAILBOX_MAX_PHY_DATA_IND_LEN];

/*传输格式标识数组*/
VOS_UINT16  g_ausTrchId[OM_UL_TRANS_CHANNEL_NUM] = {1 , 5};

/*保存SN号*/
VOS_UINT32  g_ulOmSerialNum = 0;

VOS_UINT32  g_ulReportFlag = VOS_FALSE;



OM_MSG_FUN_STRU g_astOmNoSigProcTbl[] =
{
    {OM_BtTransProc,     APP_OM_NON_SIG_BT_TRAN_REQ, OM_APP_NON_SIG_BT_TRAN_CNF},
    {OM_WSyncStatusProc, APP_OM_W_SYNC_STATUS_REQ,   OM_APP_W_SYNC_STATUS_CNF},
    {OM_GeDlStatusProc,  APP_OM_GE_DL_STATUS_REQ,    OM_APP_GE_DL_STATUS_CNF},
    {OM_EdgeBlerProc,    APP_OM_EDGE_BLER_REQ,       OM_APP_EDGE_BLER_CNF},
    {OM_BerConfigProc,   APP_OM_W_BER_DATA_REQ,      OM_APP_W_BER_DATA_CNF}
};


/*非信令控制结构体*/
OM_NOSIG_CTRL_STRU  g_stOmNoSigCtrl;

extern VOS_UINT32 OM_TraceMsgHook(VOS_VOID* pMsg);

/*BT非信令支持标记*/
VOS_UINT32  g_ulOmNosigEnable = 0;

/*****************************************************************************
  3 函数实现
*****************************************************************************/


/*****************************************************************************
 函 数 名  : OM_BtTransProc
 功能描述  : BT非信令透明通道处理
 输入参数  : pstAppToOmMsg： 指向工具发来的数据指针
             usReturnPrimId: 返回给PC工具的PRIMID

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 OM_BtTransProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    VOS_UINT16                  usNoSigMsgCnt;
    VOS_UINT16                  usIndex;
    VOS_UINT32                  ulMsgDataAddr;
    OM_NOSIG_MSG_HEAD_STRU      *pstNoSigMsgHead;
    MsgBlock                    *pstSndMsg;

    /* 保存 FuncType */
    g_stOmNoSigCtrl.ucFuncType = pstAppToOmMsg->ucFuncType;

    /* 保存是否使能消息上报 */
    g_stOmNoSigCtrl.usEnableReport = *(VOS_UINT16*)(pstAppToOmMsg->aucPara);

    /* 非信令模式下的交互消息数 */
    usNoSigMsgCnt = *(VOS_UINT16*)(pstAppToOmMsg->aucPara + sizeof(VOS_UINT16));

    ulMsgDataAddr =  (VOS_UINT32)(pstAppToOmMsg->aucPara + sizeof(VOS_UINT32));

    VOS_TaskLock();

    for (usIndex = 1; usIndex <= usNoSigMsgCnt; usIndex++)
    {
        pstNoSigMsgHead = (OM_NOSIG_MSG_HEAD_STRU*)ulMsgDataAddr;

        /* 假如是下发消息则调用VOS接口发送出去 */
        if (VOS_FALSE == pstNoSigMsgHead->usNsMsgType)
        {
            pstSndMsg = VOS_AllocMsg(WUEPS_PID_NOSIG, pstNoSigMsgHead->usNsMsgLen - VOS_MSG_HEAD_LENGTH);

            if (VOS_NULL_PTR == pstSndMsg)
            {
                VOS_TaskUnlock();

                return VOS_ERR;
            }

            VOS_MemCpy(pstSndMsg, &(pstNoSigMsgHead->usRcvMsgPrimId), pstNoSigMsgHead->usNsMsgLen);

            (VOS_VOID)VOS_SendMsg(pstSndMsg->ulSenderPid, pstSndMsg);

            /* 前4个字节为消息类型和消息长度字段 */
            ulMsgDataAddr += (sizeof(VOS_UINT32) + pstNoSigMsgHead->usNsMsgLen);
        }
        /* 否则为等待消息 */
        else
        {
            g_stOmNoSigCtrl.usRcvMsgPrimId = pstNoSigMsgHead->usRcvMsgPrimId;

            VOS_TaskUnlock();

            if (VOS_OK != VOS_SmP(g_stOmNoSigCtrl.ulNoSigSyncSem, OM_NOSIG_WAIT_TIME_LEN))
            {
                OM_SendResult(pstAppToOmMsg->ucFuncType, usIndex, usReturnPrimId);

                return VOS_ERR;
            }

            VOS_TaskLock();
            ulMsgDataAddr += sizeof(OM_NOSIG_MSG_HEAD_STRU);
        }
    }

    VOS_TaskUnlock();
    OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OM_WSyncStatusProc
 功能描述  : 查询W下同步失步状态
 输入参数  : pstAppToOmMsg： 指向工具发来的数据指针
             usReturnPrimId: 返回给PC工具的PRIMID

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 OM_WSyncStatusProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    /*把同步状态发送给PC工具*/
    OM_SendResult(pstAppToOmMsg->ucFuncType, (VOS_UINT32)g_stOmNoSigCtrl.usSyncStatus, usReturnPrimId);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OM_EdgeBlerProc
 功能描述  : 查询BLER
 输入参数  : pstAppToOmMsg： 指向工具发来的数据指针
             usReturnPrimId: 返回给PC工具的PRIMID

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/

VOS_UINT32 OM_EdgeBlerProc (APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    OM_EDGE_BLER_STRU                   stOmEdgeBlerData;
    MODEM_ID_ENUM_UINT16                enModemID;

    enModemID = OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType);

    if(MODEM_ID_BUTT <= enModemID)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_EdgeBlerProc : Modem ID Error!");
        return VOS_ERR;
    }

    stOmEdgeBlerData.ulFirstErrNum    = HPA_Read32Reg(g_aulGBBPRegAddr[BLER_ERR][enModemID]);
    stOmEdgeBlerData.ulFirstTotalNum  = HPA_Read32Reg(g_aulGBBPRegAddr[BLER_TOTAL][enModemID]);

    if( VOS_OK != VOS_TaskDelay(OM_EDGE_BLER_TASK_DELAY))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_EdgeBlerProc : Task Delay Fail!");
        return VOS_ERR;
    }

    stOmEdgeBlerData.ulSecondErrNum   = HPA_Read32Reg(g_aulGBBPRegAddr[BLER_ERR][enModemID]);
    stOmEdgeBlerData.ulSecondTotalNum = HPA_Read32Reg(g_aulGBBPRegAddr[BLER_TOTAL][enModemID]);
    stOmEdgeBlerData.usLength         = (VOS_UINT16)(sizeof(OM_EDGE_BLER_STRU)- VOS_OM_HEADER_LEN);

    OM_SendContent(pstAppToOmMsg->ucFuncType, (OM_APP_MSG_EX_STRU *)(&stOmEdgeBlerData), usReturnPrimId);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  :   OM_GeDlStatusProc
 功能描述  : 获得GSM/EDGE下的BER结果
 输入参数  : pstAppToOmMsg： 指向工具发来的数据指针
             usReturnPrimId: 返回给PC工具的PRIMID

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 OM_GeDlStatusProc (APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32                          ulPreFN;
    VOS_UINT32                          ulCurrentFN;
    VOS_UINT32                          ulRealFN;
    PHP_RRBP_FN_REQ                     *pstPhpRrbpReq;
    MsgBlock                            *pstSndMsg;
    OM_GE_DL_STATUS_REQ_STRU            *pstOmGeDlStatReq;
    MODEM_ID_ENUM_UINT16                enModemID;

    pstOmGeDlStatReq = (OM_GE_DL_STATUS_REQ_STRU*)pstAppToOmMsg;

    enModemID = OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType);

    if(MODEM_ID_BUTT <= enModemID)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GeDlStatusProc :Modem ID Error!");

        return VOS_ERR;
    }

    /* 物理层消息是否上报标记 */
    g_ulReportFlag = VOS_TRUE;

    /* GSM CS 状态下，无需下发，物理层主动上报状态 */
    if (VOS_TRUE == pstOmGeDlStatReq->usModeType)
    {
        return VOS_OK;
    }

    pstSndMsg = VOS_AllocMsg(UEPS_PID_GRM, sizeof(PHP_RRBP_FN_REQ) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstSndMsg)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GeDlStatusProc :DIAG_DataProc Alloc Msg Failed!");

        return VOS_ERR;
    }

    if(MODEM_ID_1 == enModemID)
    {
        pstSndMsg->ulSenderPid      = I1_UEPS_PID_GRM;
        pstSndMsg->ulReceiverPid    = I1_DSP_PID_GPHY;   /*发送给卡1的PID*/
    }
    else
    {
        pstSndMsg->ulSenderPid      = I0_UEPS_PID_GRM;
        pstSndMsg->ulReceiverPid    = I0_DSP_PID_GPHY;   /*发送给卡0的PID*/
    }
    /*通过读取寄存器获得当前帧数*/
    ulPreFN  = HPA_Read32Reg(g_aulGBBPRegAddr[GTC_FN][enModemID]);

    /*当前帧数由两部分组成，高16位*1326 + 低16位寄存器值*/
    ulCurrentFN  = ((ulPreFN >> 16)& OM_FRAME_NUM_REG_MASKBIT)* OM_HIHG_ADDR_FRAME_NUM;
    ulCurrentFN  += (ulPreFN & OM_FRAME_NUM_REG_MASKBIT);

    /*实际帧数为当前帧数加上下发消息时间估计值*/
    ulRealFN     = (ulCurrentFN + OM_DL_MSG_FRAME_NUM ) % OM_FRAME_MAX_NUM;

    pstPhpRrbpReq            = (PHP_RRBP_FN_REQ*)pstSndMsg;
    pstPhpRrbpReq->enRptMeas = VOS_TRUE;
    pstPhpRrbpReq->enMsgID   = ID_PHP_RRBP_FN_REQ ;
    pstPhpRrbpReq->ulFN      = ulRealFN;

    if(VOS_OK != VOS_SendMsg(UEPS_PID_GRM, pstSndMsg))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GeDlStatusProc :VOS_SendMsg Failed");

        return VOS_ERR;
    }
    return VOS_OK;

}

/*****************************************************************************
 函 数 名  : OM_BerConfigProc
 功能描述  : 配置BER上报次数
 输入参数  : pstAppToOmMsg： 指向工具发来的数据指针
             usReturnPrimId: 返回给PC工具的PRIMID

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 OM_BerConfigProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    APP_OM_W_BER_DATA_REQ_STRU          *pstBerConfig;

    pstBerConfig = (APP_OM_W_BER_DATA_REQ_STRU*)pstAppToOmMsg;

    OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);

    VOS_TaskLock();

    g_stOmNoSigCtrl.ulTotalFrameNum = pstBerConfig->usFrameNum;
    g_stOmNoSigCtrl.ulCurFrameNum   = 0;

    VOS_TaskUnlock();

    return VOS_OK;
}


VOS_VOID OM_NoSigFilterMsg(MsgBlock *pMsg)
{
    VOS_UINT16                             usPrimId;
    OM_GE_DL_STATUS_STRU                   stGeDlStatus;
    MPH_CS_DEDICATED_MEAS_IND_STRU        *pstMphMeasInd;
    MPHP_EGPRS_CHANNEL_QUALITY_REPORT_ST  *stMphpEgrsReport;
    MODEM_ID_ENUM_UINT16                   enModemID;

    usPrimId = *(VOS_UINT16*)(pMsg->aucValue);

    if (MAIL_BOX_PROTECTWORD_SND != g_ulOmNosigEnable)
    {
        return;
    }

    enModemID = OM_GET_FUNCID_MODEMINFO(g_stOmNoSigCtrl.ucFuncType);
    if(MODEM_ID_BUTT <= enModemID)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_NoSigFilterMsg : Modem ID Error!");
        return;
    }

    if(MODEM_ID_1 == enModemID)
    {
        if ((I1_DSP_PID_GPHY > pMsg->ulSenderPid)
           || (I1_DSP_PID_APM < pMsg->ulSenderPid))
        {
            return;
        }
    }
    else
    {
        if ((DSP_PID_GU_RESERVED > pMsg->ulSenderPid)
           || (DSP_PID_DRX < pMsg->ulSenderPid))
        {
            return;
        }
    }

    /*透明通道等待的消息,释放等待信号量*/
    if (usPrimId == g_stOmNoSigCtrl.usRcvMsgPrimId)
    {
        if (VOS_TRUE == g_stOmNoSigCtrl.usEnableReport)
        {
#if (FEATURE_ON == FEATURE_CBT_LOG)            
            OM_GreenChannel(OM_CBT_TRANS_FUNC, OM_APP_TRACE_IND, (VOS_UINT8*)pMsg, (VOS_UINT16)(pMsg->ulLength+VOS_MSG_HEAD_LENGTH));
#else
            OM_GreenChannel(OM_TRANS_FUNC, OM_APP_TRACE_IND, (VOS_UINT8*)pMsg, (VOS_UINT16)(pMsg->ulLength+VOS_MSG_HEAD_LENGTH));
#endif
        }

        VOS_SmV(g_stOmNoSigCtrl.ulNoSigSyncSem);
    }

    /*如果接收到W下同步或失步消息, 记录到全局变量中*/
    if ( ID_PHY_RRC_SYNC_IND == usPrimId  )
    {
        g_stOmNoSigCtrl.usSyncStatus = OM_W_SYNC_STATUS_SYNC;

        return;
    }

    if ( ID_PHY_RRC_OUT_OF_SYNC_IND == usPrimId)
    {
        g_stOmNoSigCtrl.usSyncStatus = OM_W_SYNC_STATUS_OUT_OF_SYNC;

        return;
    }

    /*如果接收到GSM/EDGE的BER回复消息 ，取出消息内容，并将结果发送给PC侧工具*/
    if ( (ID_MPH_CS_DEDICATED_MEAS_IND == usPrimId) && (VOS_TRUE == g_ulReportFlag))
    {
        pstMphMeasInd = (MPH_CS_DEDICATED_MEAS_IND_STRU*)pMsg;

        stGeDlStatus.usBerValue[0] = pstMphMeasInd->usMeanBep;
        stGeDlStatus.usBerValue[1] = pstMphMeasInd->usCvBep;
        stGeDlStatus.usLength      = (VOS_UINT16)(sizeof(stGeDlStatus)- VOS_OM_HEADER_LEN);

        /*只上报一次 */
        g_ulReportFlag = VOS_FALSE;

        OM_SendContent(g_stOmNoSigCtrl.ucFuncType, (OM_APP_MSG_EX_STRU *)(&stGeDlStatus), OM_APP_GE_DL_STATUS_CNF);

        return;
    }

    if( (ID_MPHP_EGPRS_CHANNEL_QUALITY_REPORT == usPrimId) && (VOS_TRUE == g_ulReportFlag))
    {
        stMphpEgrsReport = (MPHP_EGPRS_CHANNEL_QUALITY_REPORT_ST*)pMsg;

        stGeDlStatus.usBerValue[0] = stMphpEgrsReport->usMeanBep8Psk;
        stGeDlStatus.usBerValue[1] = stMphpEgrsReport->usCVBep8Psk;
        stGeDlStatus.usLength      = (VOS_UINT16)(sizeof(stGeDlStatus)- VOS_OM_HEADER_LEN);

        /*只上报一次 */
        g_ulReportFlag = VOS_FALSE;

        OM_SendContent(g_stOmNoSigCtrl.ucFuncType, (OM_APP_MSG_EX_STRU *)(&stGeDlStatus), OM_APP_GE_DL_STATUS_CNF);

        return;
    }

    return;
}
VOS_VOID OM_NoSigMsgProc(MsgBlock *pMsg)
{
    APP_OM_MSG_EX_STRU  *pstAppToOmMsg;
    VOS_UINT16          usReturnPrimId;
    VOS_UINT32          ulTotalNum;
    VOS_UINT32          ulIndex;
    VOS_UINT32          ulResult = VOS_ERR;

    if (WUEPS_PID_OM == pMsg->ulSenderPid)
    {
        pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pMsg->aucValue;

        ulTotalNum = sizeof(g_astOmNoSigProcTbl)/sizeof(OM_MSG_FUN_STRU);

        /* 根据PrimId查找对应的处理函数 */
        for (ulIndex = 0; ulIndex < ulTotalNum; ulIndex++)
        {
            if (pstAppToOmMsg->usPrimId == g_astOmNoSigProcTbl[ulIndex].ulPrimId)
            {
                usReturnPrimId = (VOS_UINT16)(g_astOmNoSigProcTbl[ulIndex].ulReturnPrimId);

                ulResult = g_astOmNoSigProcTbl[ulIndex].pfFun(pstAppToOmMsg, usReturnPrimId);

                break;
            }
        }

        if (ulIndex == ulTotalNum)
        {
            PS_LOG2(WUEPS_PID_OM, 0, PS_PRINT_INFO, "OM_NoSigMsgProc: Result :%d, PrimId : %d can't be Found\n",
                    (VOS_INT32)ulResult, (VOS_INT32)pstAppToOmMsg->usPrimId);
        }

    }

    return;
}
VOS_VOID OM_R99MailboxResetMutexFlag(VOS_UINT32  ulMailboxBufIndex)
{

    VOS_UINT32 ulVtbDataFlag;

    ulVtbDataFlag = HPA_Read16Reg(g_aulR99MailboxVtbAddr[ulMailboxBufIndex]);

    /* 清除VTB和TURBO的数据指示标志 */
    if ( OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulVtbDataFlag)
    {
        HPA_Write16Reg(g_aulR99MailboxVtbAddr[ulMailboxBufIndex],
                            OM_R99_MAILBOX_MUTEX_DATA_NOT_EXIST);
    }

    /* 清除所读取译码邮箱的读写互斥标记 */
    if (OM_R99_DECODE_BUF_INDEX_FIRST == ulMailboxBufIndex)
    {
        HPA_Write32Reg(WTTFPHY_MAC_DATA_IND_FIRST_MUTEX_ADDR,
                            OM_R99_MAILBOX_MUTEX_DATA_NOT_EXIST);
    }
    else if (OM_R99_DECODE_BUF_INDEX_SECOND == ulMailboxBufIndex)
    {
        HPA_Write32Reg(WTTFPHY_MAC_DATA_IND_SECOND_MUTEX_ADDR,
                            OM_R99_MAILBOX_MUTEX_DATA_NOT_EXIST);
    }
    else
    {

    }

    return;
}

/*****************************************************************************
 函 数 名  : OM_GetR99MailboxIndex
 功能描述  : 把译码数据邮箱分成0、1两组，每组都包含VITERBI和TURBO译码数据，
              DSP每次收到译码中断,会先看0号BUFF是否可读，如可读，则获取BUFF index
              否则，再看1号BUFF是否可读，如可读，则获取BUFF index ;
              如两块BUFF均不可读，则返回0。
              同一邮箱中VTB数据的序号与Turbo数据的序号一样, 所以取任1个即可。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0:无Buf可读;1,2存在相应的Buf Index可读
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 OM_GetR99MailboxIndex(VOS_VOID)
{

    VOS_UINT32 ulVtbDataFlag;
    VOS_UINT32 ulFirstSerialNum     = 0;
    VOS_UINT32 ulSecondSerialNum    = 0;

    /* 该变量既为表示邮箱1或2存在的掩码，又做邮箱地址数组下标 */
    VOS_UINT32 ulMailboxIndex       = 0;

    VOS_UINT32 ulFirstMailboxMuteFlag;   /* 保存译码邮箱1读写互斥标记 */
    VOS_UINT32 ulSecondMailboxMuteFlag;  /* 保存译码邮箱2读写互斥标记 */

    /* 获取VTB和TURBO BUFFER的数据指示标志 */
    ulFirstMailboxMuteFlag  = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_FIRST_MUTEX_ADDR);
    ulSecondMailboxMuteFlag = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_SECOND_MUTEX_ADDR);

    /* 根据数据BUFFER的互斥标志，置ulMailDataInd */
    if (OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulFirstMailboxMuteFlag)
    {
        ulMailboxIndex |= OM_R99_DECODE_BUF_INDEX_FIRST;
    }
    if (OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulSecondMailboxMuteFlag)
    {
        ulMailboxIndex |= OM_R99_DECODE_BUF_INDEX_SECOND;
    }

    /* 根据数据是否存在的掩码值，判断读取哪个邮箱的数据(用ulMailDataInd存放邮箱地址数组下标) */

    /* 如果只有1个邮箱有数据，则不改变ulMailDataInd，用其做数组下标，取邮箱地址 */

    /* 如果两个邮箱都有数据， */
    if ((OM_R99_DECODE_BUF_INDEX_FIRST|OM_R99_DECODE_BUF_INDEX_SECOND) == ulMailboxIndex)
    {
        /* 两个邮箱中都有数据的情况 */

        /* 同一邮箱中VTB数据的序号与Turbo数据的序号一样, 所以取任1个即可 */
        ulVtbDataFlag = HPA_Read16Reg(g_aulR99MailboxVtbAddr[OM_R99_DECODE_BUF_INDEX_FIRST]);

        if (OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulVtbDataFlag)
        {
            /*lint -e413*/
            ulFirstSerialNum = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_FIRST_VTB_ADDR
                                  + OM_OFFSET_OF(WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU, ulSerialNum));
        }
        else
        {
            ulFirstSerialNum = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_FIRST_TURBO_ADDR
                                   + OM_OFFSET_OF(WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU, ulSerialNum));
        }

        ulVtbDataFlag = HPA_Read16Reg(g_aulR99MailboxVtbAddr[OM_R99_DECODE_BUF_INDEX_SECOND]);

        if (OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulVtbDataFlag)
        {
            ulSecondSerialNum = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_SECOND_VTB_ADDR
                                          + OM_OFFSET_OF(WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU, ulSerialNum));
        }
        else
        {
            ulSecondSerialNum = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_SECOND_TURBO_ADDR + OM_OFFSET_OF(WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU, ulSerialNum));
            /*lint +e413*/
        }

        /* 比较两个邮箱中的数据序号，读数据序号小的邮箱 */
        if (ulFirstSerialNum < ulSecondSerialNum)
        {
            ulMailboxIndex = OM_R99_DECODE_BUF_INDEX_FIRST;
        }
        else
        {
            ulMailboxIndex = OM_R99_DECODE_BUF_INDEX_SECOND;
        }
    }

    return ulMailboxIndex;
}
VOS_UINT32 OM_R99MailboxRead(VOS_UINT8 *pucDataIndBuf,
                                        VOS_UINT16 *pusDataLen,
                                        VOS_UINT32  ulMailboxBufIndex)
{
    VOS_UINT32                          ulMailboxBufAddr;
    VOS_UINT32                          ulVtbDataFlag;
    VOS_UINT16                          usVtbDataLen = 0;

    ulVtbDataFlag = HPA_Read16Reg(g_aulR99MailboxVtbAddr[ulMailboxBufIndex]);

    /* 信令等低速信道一般为VTB编码, 信令优先 ,如果VTB数据RAM有数据，判断数据长度的有效性*/
    if (OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulVtbDataFlag)
    {
        ulMailboxBufAddr    = (g_aulR99MailboxVtbAddr[ulMailboxBufIndex]);

        usVtbDataLen        = HPA_Read16Reg(ulMailboxBufAddr + sizeof(VOS_UINT16));

        if (((usVtbDataLen*4) <= WTTFPHY_MAX_PHY_DATA_IND_VTB_DATA_LEN) && (usVtbDataLen > 0))
        {
            HPA_GetDspMem((VOS_CHAR *)ulMailboxBufAddr, (VOS_CHAR *)pucDataIndBuf,
                                    WTTFPHY_MAX_PHY_DATA_IND_VTB_HEAD_LEN+(usVtbDataLen*4));

            *pusDataLen = (VOS_UINT16)(usVtbDataLen*4);

            return VOS_TRUE;
        }
    }

    return  VOS_FALSE;
}
VOS_VOID OM_LoopTestProc(VOS_VOID)
{
    VOS_UINT32                              ulMailboxBufIndex;
    VOS_UINT32                              ulResult;
    VOS_UINT16                              usDataLen;
    WTTFPHY_PHY_DATA_REQ_BUF_STRU           *pstMacPhyDataReqBuf;
    WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU    *pstPhyDataIndHead;
/*lint -e813 修改人:dongtinghuan;检视人:徐铖 51137 */
    APP_OM_W_BER_DATA_IND_STRU              stBerDataInd;
/*lint +e813 修改人:dongtinghuan;检视人:徐铖 51137 */

    if (MAIL_BOX_PROTECTWORD_SND != g_ulOmNosigEnable)
    {
        return;
    }

    /*译码邮箱有两组, 首先获取译码邮箱索引*/

    ulMailboxBufIndex = OM_GetR99MailboxIndex();

    if ( OM_R99_DECODE_BUF_INDEX_INIT == ulMailboxBufIndex )
    {
       return ;
    }

    ulResult = OM_R99MailboxRead(g_aucOmR99MBData, &usDataLen, ulMailboxBufIndex);

    if (VOS_TRUE == ulResult)
    {
        /*下行控制参数写入上行信道共享缓存中*/
        pstMacPhyDataReqBuf = (WTTFPHY_PHY_DATA_REQ_BUF_STRU *)(WTTFPHY_MAC_PHY_DATA_REQ_ADDR);

        pstPhyDataIndHead = (WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU *)&g_aucOmR99MBData[0];

        pstMacPhyDataReqBuf->enMutexFlag = WTTFPHY_MUTEX_DATA_EXIST;
        pstMacPhyDataReqBuf->enCctrchID  = WTTFPHY_UL_DPCH;
        pstMacPhyDataReqBuf->enMsgName   = ID_MAC_PHY_DATA_REQ;
        pstMacPhyDataReqBuf->usUlTrchCnt = OM_UL_TRANS_CHANNEL_NUM ;

        /*上行信道CFN为当前CFN+1*/
        pstMacPhyDataReqBuf->usCFN = (g_ucHpaCfnRead + 1);

        /*Tfi取值[1,32],传输信道标识*/
        pstMacPhyDataReqBuf->astUlTrchDataInfo[0].usTfi = pstPhyDataIndHead->astDlTrchDataInfo[0].ucTfi;
        /* TrchId取值[0,63],传输格式标识，这里固定为1 */
        pstMacPhyDataReqBuf->astUlTrchDataInfo[0].usTrchId = g_ausTrchId[0];


        pstMacPhyDataReqBuf->astUlTrchDataInfo[1].usTfi = pstPhyDataIndHead->astDlTrchDataInfo[1].ucTfi ;
        pstMacPhyDataReqBuf->astUlTrchDataInfo[1].usTrchId = g_ausTrchId[1];

        pstMacPhyDataReqBuf->ulSerialNum = g_ulOmSerialNum;
        g_ulOmSerialNum++;

        pstMacPhyDataReqBuf->usDataLen = usDataLen;

        if (g_stOmNoSigCtrl.ulCurFrameNum < g_stOmNoSigCtrl.ulTotalFrameNum)
        {
            stBerDataInd.usTotalFrameNum = (VOS_UINT16)g_stOmNoSigCtrl.ulTotalFrameNum;
            stBerDataInd.usCurFrameNum   = (VOS_UINT16)g_stOmNoSigCtrl.ulCurFrameNum;
            g_stOmNoSigCtrl.ulCurFrameNum++;

            if (OM_BER_DATA_MAX_SIZE < (usDataLen + WTTFPHY_MAX_PHY_DATA_IND_VTB_HEAD_LEN))
            {
                stBerDataInd.ulDataLen = OM_BER_DATA_MAX_SIZE;

                LogPrint1("OM_LoopTestProc: BER data is too large %d.\r\n", usDataLen);
            }
            else
            {
                stBerDataInd.ulDataLen = (VOS_UINT32)usDataLen + WTTFPHY_MAX_PHY_DATA_IND_VTB_HEAD_LEN;
            }

            VOS_MemCpy(stBerDataInd.aucData, g_aucOmR99MBData, stBerDataInd.ulDataLen);

            stBerDataInd.usLength = sizeof(stBerDataInd) - VOS_OM_HEADER_LEN;
            OM_SendContent(g_stOmNoSigCtrl.ucFuncType, (OM_APP_MSG_EX_STRU*)&stBerDataInd, OM_APP_W_BER_DATA_IND);
        }

        HPA_PutDspMem((VOS_CHAR*)&g_aucOmR99MBData[WTTFPHY_MAX_PHY_DATA_IND_VTB_HEAD_LEN],
                                      (VOS_CHAR*)pstMacPhyDataReqBuf->aucData, usDataLen);

    }

    OM_R99MailboxResetMutexFlag(ulMailboxBufIndex);

    return;
}

/*****************************************************************************
 函 数 名  : OM_NoSigCtrlInit
 功能描述  : 非信令控制结构初全局变量始化
 输入参数  : 无

 输出参数  : 无
 返 回 值  : VOID
*****************************************************************************/
VOS_VOID OM_NoSigCtrlInit(VOS_VOID)
{
    g_stOmNoSigCtrl.usEnableReport = VOS_FALSE;
    g_stOmNoSigCtrl.usRcvMsgPrimId = 0;
    g_stOmNoSigCtrl.ucFuncType     = OM_NOSIG_FUNC;
    g_stOmNoSigCtrl.usSyncStatus   = OM_W_SYNC_STATUS_UNKNOWN;
    g_stOmNoSigCtrl.ulTotalFrameNum= 0;
    g_stOmNoSigCtrl.ulCurFrameNum  = 0;
}


VOS_UINT32 OM_NoSigPidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    if (VOS_IP_LOAD_CONFIG == ip)
    {
        /* 分配互斥信号量 */
        if (VOS_OK != VOS_SmBCreate( "NOSIG", 0, VOS_SEMA4_FIFO, &(g_stOmNoSigCtrl.ulNoSigSyncSem)))
        {
            return VOS_ERR;
        }

        /* 初始化非信令控制结构 */
        OM_NoSigCtrlInit();
    }

    return VOS_OK;
}


VOS_UINT32 OM_NoSigFidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ulRslt;

    if (VOS_IP_LOAD_CONFIG == ip)
    {
        ulRslt = VOS_RegisterPIDInfo(WUEPS_PID_NOSIG,
                                    (Init_Fun_Type)OM_NoSigPidInit,
                                    (Msg_Fun_Type)OM_NoSigMsgProc);
        if( VOS_OK != ulRslt )
        {
            return VOS_ERR;
        }

        ulRslt = VOS_RegisterTaskPrio(WUEPS_FID_NOSIG, COMM_NOSIG_TASK_PRIO);

        if( VOS_OK != ulRslt )
        {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}
#else

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

#if (VOS_WIN32 == VOS_OS_VER)

VOS_UINT8 g_aucPhyDataReqAddr[1024];
#define  WTTFPHY_MAC_PHY_DATA_REQ_ADDR  (VOS_UINT32)(&g_aucPhyDataReqAddr[0])

#endif


/* 保存R99 Mailbox VTB 译码数据 */
VOS_UINT8   g_aucOmR99MBData[OM_R99_MAILBOX_MAX_PHY_DATA_IND_LEN];

/*传输格式标识数组*/
VOS_UINT16  g_ausTrchId[OM_UL_TRANS_CHANNEL_NUM] = {1 , 5};

/*保存SN号*/
VOS_UINT32  g_ulOmSerialNum = 0;

VOS_UINT32  g_ulReportFlag = VOS_FALSE;



OM_MSG_FUN_STRU g_astOmNoSigProcTbl[] =
{
    {OM_BtTransProc,     APP_OM_NON_SIG_BT_TRAN_REQ, OM_APP_NON_SIG_BT_TRAN_CNF},
    {OM_WSyncStatusProc, APP_OM_W_SYNC_STATUS_REQ,   OM_APP_W_SYNC_STATUS_CNF},
    {OM_GeDlStatusProc,  APP_OM_GE_DL_STATUS_REQ,    OM_APP_GE_DL_STATUS_CNF},
    {OM_EdgeBlerProc,    APP_OM_EDGE_BLER_REQ,       OM_APP_EDGE_BLER_CNF},
    {OM_BerConfigProc,   APP_OM_W_BER_DATA_REQ,      OM_APP_W_BER_DATA_CNF}
};


/*非信令控制结构体*/
OM_NOSIG_CTRL_STRU  g_stOmNoSigCtrl;

extern VOS_UINT32 OM_TraceMsgHook(VOS_VOID* pMsg);

extern VOS_UINT32 OM_GreenChannelChannel(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel, VOS_UINT8 ucFuncType, VOS_UINT16 usPrimId,
                            VOS_UINT8 *pucData, VOS_UINT16 usLen);

/*BT非信令支持标记*/
VOS_UINT32  g_ulOmNosigEnable = 0;

/*****************************************************************************
  3 函数实现
*****************************************************************************/


/*****************************************************************************
 函 数 名  : OM_BtTransProc
 功能描述  : BT非信令透明通道处理
 输入参数  : pstAppToOmMsg： 指向工具发来的数据指针
             usReturnPrimId: 返回给PC工具的PRIMID

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 OM_BtTransProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    VOS_UINT16                  usNoSigMsgCnt;
    VOS_UINT16                  usIndex;
    VOS_UINT32                  ulMsgDataAddr;
    OM_NOSIG_MSG_HEAD_STRU      *pstNoSigMsgHead;
    MsgBlock                    *pstSndMsg;

    /* 保存 FuncType */
    g_stOmNoSigCtrl.ucFuncType = pstAppToOmMsg->ucFuncType;

    /* 保存是否使能消息上报 */
    g_stOmNoSigCtrl.usEnableReport = *(VOS_UINT16*)(pstAppToOmMsg->aucPara);

    /* 非信令模式下的交互消息数 */
    usNoSigMsgCnt = *(VOS_UINT16*)(pstAppToOmMsg->aucPara + sizeof(VOS_UINT16));

    ulMsgDataAddr =  (VOS_UINT32)(pstAppToOmMsg->aucPara + sizeof(VOS_UINT32));

    VOS_TaskLock();

    for (usIndex = 1; usIndex <= usNoSigMsgCnt; usIndex++)
    {
        pstNoSigMsgHead = (OM_NOSIG_MSG_HEAD_STRU*)ulMsgDataAddr;

        /* 假如是下发消息则调用VOS接口发送出去 */
        if (VOS_FALSE == pstNoSigMsgHead->usNsMsgType)
        {
            pstSndMsg = VOS_AllocMsg(WUEPS_PID_NOSIG, pstNoSigMsgHead->usNsMsgLen - VOS_MSG_HEAD_LENGTH);

            if (VOS_NULL_PTR == pstSndMsg)
            {
                VOS_TaskUnlock();

                return VOS_ERR;
            }

            VOS_MemCpy(pstSndMsg, &(pstNoSigMsgHead->usRcvMsgPrimId), pstNoSigMsgHead->usNsMsgLen);

            (VOS_VOID)VOS_SendMsg(pstSndMsg->ulSenderPid, pstSndMsg);

            /* 前4个字节为消息类型和消息长度字段 */
            ulMsgDataAddr += (sizeof(VOS_UINT32) + pstNoSigMsgHead->usNsMsgLen);
        }
        /* 否则为等待消息 */
        else
        {
            g_stOmNoSigCtrl.usRcvMsgPrimId = pstNoSigMsgHead->usRcvMsgPrimId;

            VOS_TaskUnlock();

            if (VOS_OK != VOS_SmP(g_stOmNoSigCtrl.ulNoSigSyncSem, OM_NOSIG_WAIT_TIME_LEN))
            {
                OM_SendResultChannel((OM_LOGIC_CHANNEL_ENUM_UINT32)OM_LOGIC_CHANNEL_CBT, pstAppToOmMsg->ucFuncType, usIndex, usReturnPrimId);

                return VOS_ERR;
            }

            VOS_TaskLock();
            ulMsgDataAddr += sizeof(OM_NOSIG_MSG_HEAD_STRU);
        }
    }

    VOS_TaskUnlock();
    OM_SendResultChannel((OM_LOGIC_CHANNEL_ENUM_UINT32)OM_LOGIC_CHANNEL_CBT, pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OM_WSyncStatusProc
 功能描述  : 查询W下同步失步状态
 输入参数  : pstAppToOmMsg： 指向工具发来的数据指针
             usReturnPrimId: 返回给PC工具的PRIMID

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 OM_WSyncStatusProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    /*把同步状态发送给PC工具*/
    OM_SendResultChannel((OM_LOGIC_CHANNEL_ENUM_UINT32)OM_LOGIC_CHANNEL_CBT, pstAppToOmMsg->ucFuncType, (VOS_UINT32)g_stOmNoSigCtrl.usSyncStatus, usReturnPrimId);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OM_EdgeBlerProc
 功能描述  : 查询BLER
 输入参数  : pstAppToOmMsg： 指向工具发来的数据指针
             usReturnPrimId: 返回给PC工具的PRIMID

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/

VOS_UINT32 OM_EdgeBlerProc (APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    OM_EDGE_BLER_STRU                   stOmEdgeBlerData;
    MODEM_ID_ENUM_UINT16                enModemID;

    enModemID = OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType);

    if(MODEM_ID_BUTT <= enModemID)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_EdgeBlerProc : Modem ID Error!");
        return VOS_ERR;
    }

    stOmEdgeBlerData.ulFirstErrNum    = HPA_Read32Reg(g_aulGBBPRegAddr[BLER_ERR][enModemID]);
    stOmEdgeBlerData.ulFirstTotalNum  = HPA_Read32Reg(g_aulGBBPRegAddr[BLER_TOTAL][enModemID]);

    if( VOS_OK != VOS_TaskDelay(OM_EDGE_BLER_TASK_DELAY))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_EdgeBlerProc : Task Delay Fail!");
        return VOS_ERR;
    }

    stOmEdgeBlerData.ulSecondErrNum   = HPA_Read32Reg(g_aulGBBPRegAddr[BLER_ERR][enModemID]);
    stOmEdgeBlerData.ulSecondTotalNum = HPA_Read32Reg(g_aulGBBPRegAddr[BLER_TOTAL][enModemID]);
    stOmEdgeBlerData.usLength         = (VOS_UINT16)(sizeof(OM_EDGE_BLER_STRU)- VOS_OM_HEADER_LEN);

    OM_SendContentChannel((OM_LOGIC_CHANNEL_ENUM_UINT32)OM_LOGIC_CHANNEL_CBT, pstAppToOmMsg->ucFuncType, (OM_APP_MSG_EX_STRU *)(&stOmEdgeBlerData), usReturnPrimId);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  :   OM_GeDlStatusProc
 功能描述  : 获得GSM/EDGE下的BER结果
 输入参数  : pstAppToOmMsg： 指向工具发来的数据指针
             usReturnPrimId: 返回给PC工具的PRIMID

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 OM_GeDlStatusProc (APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32                          ulPreFN;
    VOS_UINT32                          ulCurrentFN;
    VOS_UINT32                          ulRealFN;
    PHP_RRBP_FN_REQ                     *pstPhpRrbpReq;
    MsgBlock                            *pstSndMsg;
    OM_GE_DL_STATUS_REQ_STRU            *pstOmGeDlStatReq;
    MODEM_ID_ENUM_UINT16                enModemID;

    pstOmGeDlStatReq = (OM_GE_DL_STATUS_REQ_STRU*)pstAppToOmMsg;

    enModemID = OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType);

    if(MODEM_ID_BUTT <= enModemID)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GeDlStatusProc :Modem ID Error!");

        return VOS_ERR;
    }

    /* 物理层消息是否上报标记 */
    g_ulReportFlag = VOS_TRUE;

    /* GSM CS 状态下，无需下发，物理层主动上报状态 */
    if (VOS_TRUE == pstOmGeDlStatReq->usModeType)
    {
        return VOS_OK;
    }

    pstSndMsg = VOS_AllocMsg(UEPS_PID_GRM, sizeof(PHP_RRBP_FN_REQ) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstSndMsg)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GeDlStatusProc :DIAG_DataProc Alloc Msg Failed!");

        return VOS_ERR;
    }

    if(MODEM_ID_1 == enModemID)
    {
        pstSndMsg->ulSenderPid      = I1_UEPS_PID_GRM;
        pstSndMsg->ulReceiverPid    = I1_DSP_PID_GPHY;   /*发送给卡1的PID*/
    }
    else
    {
        pstSndMsg->ulSenderPid      = I0_UEPS_PID_GRM;
        pstSndMsg->ulReceiverPid    = I0_DSP_PID_GPHY;   /*发送给卡0的PID*/
    }
    /*通过读取寄存器获得当前帧数*/
    ulPreFN  = HPA_Read32Reg(g_aulGBBPRegAddr[GTC_FN][enModemID]);

    /*当前帧数由两部分组成，高16位*1326 + 低16位寄存器值*/
    ulCurrentFN  = ((ulPreFN >> 16)& OM_FRAME_NUM_REG_MASKBIT)* OM_HIHG_ADDR_FRAME_NUM;
    ulCurrentFN  += (ulPreFN & OM_FRAME_NUM_REG_MASKBIT);

    /*实际帧数为当前帧数加上下发消息时间估计值*/
    ulRealFN     = (ulCurrentFN + OM_DL_MSG_FRAME_NUM ) % OM_FRAME_MAX_NUM;

    pstPhpRrbpReq            = (PHP_RRBP_FN_REQ*)pstSndMsg;
    pstPhpRrbpReq->enRptMeas = VOS_TRUE;
    pstPhpRrbpReq->enMsgID   = ID_PHP_RRBP_FN_REQ ;
    pstPhpRrbpReq->ulFN      = ulRealFN;

    if(VOS_OK != VOS_SendMsg(UEPS_PID_GRM, pstSndMsg))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GeDlStatusProc :VOS_SendMsg Failed");

        return VOS_ERR;
    }
    return VOS_OK;

}

/*****************************************************************************
 函 数 名  : OM_BerConfigProc
 功能描述  : 配置BER上报次数
 输入参数  : pstAppToOmMsg： 指向工具发来的数据指针
             usReturnPrimId: 返回给PC工具的PRIMID

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 OM_BerConfigProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    APP_OM_W_BER_DATA_REQ_STRU          *pstBerConfig;

    pstBerConfig = (APP_OM_W_BER_DATA_REQ_STRU*)pstAppToOmMsg;

    OM_SendResultChannel((OM_LOGIC_CHANNEL_ENUM_UINT32)OM_LOGIC_CHANNEL_CBT, pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);

    VOS_TaskLock();

    g_stOmNoSigCtrl.ulTotalFrameNum = pstBerConfig->usFrameNum;
    g_stOmNoSigCtrl.ulCurFrameNum   = 0;

    VOS_TaskUnlock();

    return VOS_OK;
}


VOS_VOID OM_NoSigFilterMsg(MsgBlock *pMsg)
{
    VOS_UINT16                             usPrimId;
    OM_GE_DL_STATUS_STRU                   stGeDlStatus;
    MPH_CS_DEDICATED_MEAS_IND_STRU        *pstMphMeasInd;
    MPHP_EGPRS_CHANNEL_QUALITY_REPORT_ST  *stMphpEgrsReport;
    MODEM_ID_ENUM_UINT16                   enModemID;

    usPrimId = *(VOS_UINT16*)(pMsg->aucValue);

    if (MAIL_BOX_PROTECTWORD_SND != g_ulOmNosigEnable)
    {
        return;
    }

    enModemID = OM_GET_FUNCID_MODEMINFO(g_stOmNoSigCtrl.ucFuncType);
    if(MODEM_ID_BUTT <= enModemID)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_NoSigFilterMsg : Modem ID Error!");
        return;
    }

    if(MODEM_ID_1 == enModemID)
    {
        if ((I1_DSP_PID_GPHY > pMsg->ulSenderPid)
           || (I1_DSP_PID_APM < pMsg->ulSenderPid))
        {
            return;
        }
    }
    else
    {
        if ((DSP_PID_GU_RESERVED > pMsg->ulSenderPid)
           || (DSP_PID_DRX < pMsg->ulSenderPid))
        {
            return;
        }
    }

    /*透明通道等待的消息,释放等待信号量*/
    if (usPrimId == g_stOmNoSigCtrl.usRcvMsgPrimId)
    {
        if (VOS_TRUE == g_stOmNoSigCtrl.usEnableReport)
        {
            OM_GreenChannelChannel((OM_LOGIC_CHANNEL_ENUM_UINT32)OM_LOGIC_CHANNEL_CBT, OM_CBT_TRANS_FUNC, OM_APP_TRACE_IND, (VOS_UINT8*)pMsg, (VOS_UINT16)(pMsg->ulLength+VOS_MSG_HEAD_LENGTH));
        }

        VOS_SmV(g_stOmNoSigCtrl.ulNoSigSyncSem);
    }

    /*如果接收到W下同步或失步消息, 记录到全局变量中*/
    if ( ID_PHY_RRC_SYNC_IND == usPrimId  )
    {
        g_stOmNoSigCtrl.usSyncStatus = OM_W_SYNC_STATUS_SYNC;

        return;
    }

    if ( ID_PHY_RRC_OUT_OF_SYNC_IND == usPrimId)
    {
        g_stOmNoSigCtrl.usSyncStatus = OM_W_SYNC_STATUS_OUT_OF_SYNC;

        return;
    }

    /*如果接收到GSM/EDGE的BER回复消息 ，取出消息内容，并将结果发送给PC侧工具*/
    if ( (ID_MPH_CS_DEDICATED_MEAS_IND == usPrimId) && (VOS_TRUE == g_ulReportFlag))
    {
        pstMphMeasInd = (MPH_CS_DEDICATED_MEAS_IND_STRU*)pMsg;

        stGeDlStatus.usBerValue[0] = pstMphMeasInd->usMeanBep;
        stGeDlStatus.usBerValue[1] = pstMphMeasInd->usCvBep;
        stGeDlStatus.usLength      = (VOS_UINT16)(sizeof(stGeDlStatus)- VOS_OM_HEADER_LEN);

        /*只上报一次 */
        g_ulReportFlag = VOS_FALSE;

        OM_SendContentChannel((OM_LOGIC_CHANNEL_ENUM_UINT32)OM_LOGIC_CHANNEL_CBT,g_stOmNoSigCtrl.ucFuncType, (OM_APP_MSG_EX_STRU *)(&stGeDlStatus), OM_APP_GE_DL_STATUS_CNF);

        return;
    }

    if( (ID_MPHP_EGPRS_CHANNEL_QUALITY_REPORT == usPrimId) && (VOS_TRUE == g_ulReportFlag))
    {
        stMphpEgrsReport = (MPHP_EGPRS_CHANNEL_QUALITY_REPORT_ST*)pMsg;

        stGeDlStatus.usBerValue[0] = stMphpEgrsReport->usMeanBep8Psk;
        stGeDlStatus.usBerValue[1] = stMphpEgrsReport->usCVBep8Psk;
        stGeDlStatus.usLength      = (VOS_UINT16)(sizeof(stGeDlStatus)- VOS_OM_HEADER_LEN);

        /*只上报一次 */
        g_ulReportFlag = VOS_FALSE;

        OM_SendContentChannel((OM_LOGIC_CHANNEL_ENUM_UINT32)OM_LOGIC_CHANNEL_CBT, g_stOmNoSigCtrl.ucFuncType, (OM_APP_MSG_EX_STRU *)(&stGeDlStatus), OM_APP_GE_DL_STATUS_CNF);

        return;
    }

    return;
}
VOS_VOID OM_NoSigMsgProc(MsgBlock *pMsg)
{
    APP_OM_MSG_EX_STRU  *pstAppToOmMsg;
    VOS_UINT16          usReturnPrimId;
    VOS_UINT32          ulTotalNum;
    VOS_UINT32          ulIndex;
    VOS_UINT32          ulResult = VOS_ERR;

    if (WUEPS_PID_OM == pMsg->ulSenderPid)
    {
        pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pMsg->aucValue;

        ulTotalNum = sizeof(g_astOmNoSigProcTbl)/sizeof(OM_MSG_FUN_STRU);

        /* 根据PrimId查找对应的处理函数 */
        for (ulIndex = 0; ulIndex < ulTotalNum; ulIndex++)
        {
            if (pstAppToOmMsg->usPrimId == g_astOmNoSigProcTbl[ulIndex].ulPrimId)
            {
                usReturnPrimId = (VOS_UINT16)(g_astOmNoSigProcTbl[ulIndex].ulReturnPrimId);

                ulResult = g_astOmNoSigProcTbl[ulIndex].pfFun(pstAppToOmMsg, usReturnPrimId);

                break;
            }
        }

        if (ulIndex == ulTotalNum)
        {
            PS_LOG2(WUEPS_PID_OM, 0, PS_PRINT_INFO, "OM_NoSigMsgProc: Result :%d, PrimId : %d can't be Found\n",
                    (VOS_INT32)ulResult, (VOS_INT32)pstAppToOmMsg->usPrimId);
        }

    }

    return;
}
VOS_VOID OM_R99MailboxResetMutexFlag(VOS_UINT32  ulMailboxBufIndex)
{

    VOS_UINT32 ulVtbDataFlag;

    ulVtbDataFlag = HPA_Read16Reg(g_aulR99MailboxVtbAddr[ulMailboxBufIndex]);

    /* 清除VTB和TURBO的数据指示标志 */
    if ( OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulVtbDataFlag)
    {
        HPA_Write16Reg(g_aulR99MailboxVtbAddr[ulMailboxBufIndex],
                            OM_R99_MAILBOX_MUTEX_DATA_NOT_EXIST);
    }

    /* 清除所读取译码邮箱的读写互斥标记 */
    if (OM_R99_DECODE_BUF_INDEX_FIRST == ulMailboxBufIndex)
    {
        HPA_Write32Reg(WTTFPHY_MAC_DATA_IND_FIRST_MUTEX_ADDR,
                            OM_R99_MAILBOX_MUTEX_DATA_NOT_EXIST);
    }
    else if (OM_R99_DECODE_BUF_INDEX_SECOND == ulMailboxBufIndex)
    {
        HPA_Write32Reg(WTTFPHY_MAC_DATA_IND_SECOND_MUTEX_ADDR,
                            OM_R99_MAILBOX_MUTEX_DATA_NOT_EXIST);
    }
    else
    {

    }

    return;
}

/*****************************************************************************
 函 数 名  : OM_GetR99MailboxIndex
 功能描述  : 把译码数据邮箱分成0、1两组，每组都包含VITERBI和TURBO译码数据，
              DSP每次收到译码中断,会先看0号BUFF是否可读，如可读，则获取BUFF index
              否则，再看1号BUFF是否可读，如可读，则获取BUFF index ;
              如两块BUFF均不可读，则返回0。
              同一邮箱中VTB数据的序号与Turbo数据的序号一样, 所以取任1个即可。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0:无Buf可读;1,2存在相应的Buf Index可读
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 OM_GetR99MailboxIndex(VOS_VOID)
{

    VOS_UINT32 ulVtbDataFlag;
    VOS_UINT32 ulFirstSerialNum     = 0;
    VOS_UINT32 ulSecondSerialNum    = 0;

    /* 该变量既为表示邮箱1或2存在的掩码，又做邮箱地址数组下标 */
    VOS_UINT32 ulMailboxIndex       = 0;

    VOS_UINT32 ulFirstMailboxMuteFlag;   /* 保存译码邮箱1读写互斥标记 */
    VOS_UINT32 ulSecondMailboxMuteFlag;  /* 保存译码邮箱2读写互斥标记 */

    /* 获取VTB和TURBO BUFFER的数据指示标志 */
    ulFirstMailboxMuteFlag  = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_FIRST_MUTEX_ADDR);
    ulSecondMailboxMuteFlag = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_SECOND_MUTEX_ADDR);

    /* 根据数据BUFFER的互斥标志，置ulMailDataInd */
    if (OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulFirstMailboxMuteFlag)
    {
        ulMailboxIndex |= OM_R99_DECODE_BUF_INDEX_FIRST;
    }
    if (OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulSecondMailboxMuteFlag)
    {
        ulMailboxIndex |= OM_R99_DECODE_BUF_INDEX_SECOND;
    }

    /* 根据数据是否存在的掩码值，判断读取哪个邮箱的数据(用ulMailDataInd存放邮箱地址数组下标) */

    /* 如果只有1个邮箱有数据，则不改变ulMailDataInd，用其做数组下标，取邮箱地址 */

    /* 如果两个邮箱都有数据， */
    if ((OM_R99_DECODE_BUF_INDEX_FIRST|OM_R99_DECODE_BUF_INDEX_SECOND) == ulMailboxIndex)
    {
        /* 两个邮箱中都有数据的情况 */

        /* 同一邮箱中VTB数据的序号与Turbo数据的序号一样, 所以取任1个即可 */
        ulVtbDataFlag = HPA_Read16Reg(g_aulR99MailboxVtbAddr[OM_R99_DECODE_BUF_INDEX_FIRST]);

        if (OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulVtbDataFlag)
        {
            /*lint -e413*/
            ulFirstSerialNum = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_FIRST_VTB_ADDR
                                  + OM_OFFSET_OF(WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU, ulSerialNum));
        }
        else
        {
            ulFirstSerialNum = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_FIRST_TURBO_ADDR
                                   + OM_OFFSET_OF(WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU, ulSerialNum));
        }

        ulVtbDataFlag = HPA_Read16Reg(g_aulR99MailboxVtbAddr[OM_R99_DECODE_BUF_INDEX_SECOND]);

        if (OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulVtbDataFlag)
        {
            ulSecondSerialNum = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_SECOND_VTB_ADDR
                                          + OM_OFFSET_OF(WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU, ulSerialNum));
        }
        else
        {
            ulSecondSerialNum = HPA_Read32Reg(WTTFPHY_MAC_DATA_IND_SECOND_TURBO_ADDR + OM_OFFSET_OF(WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU, ulSerialNum));
            /*lint +e413*/
        }

        /* 比较两个邮箱中的数据序号，读数据序号小的邮箱 */
        if (ulFirstSerialNum < ulSecondSerialNum)
        {
            ulMailboxIndex = OM_R99_DECODE_BUF_INDEX_FIRST;
        }
        else
        {
            ulMailboxIndex = OM_R99_DECODE_BUF_INDEX_SECOND;
        }
    }

    return ulMailboxIndex;
}
VOS_UINT32 OM_R99MailboxRead(VOS_UINT8 *pucDataIndBuf,
                                        VOS_UINT16 *pusDataLen,
                                        VOS_UINT32  ulMailboxBufIndex)
{
    VOS_UINT32                          ulMailboxBufAddr;
    VOS_UINT32                          ulVtbDataFlag;
    VOS_UINT16                          usVtbDataLen = 0;

    ulVtbDataFlag = HPA_Read16Reg(g_aulR99MailboxVtbAddr[ulMailboxBufIndex]);

    /* 信令等低速信道一般为VTB编码, 信令优先 ,如果VTB数据RAM有数据，判断数据长度的有效性*/
    if (OM_R99_MAILBOX_MUTEX_DATA_EXIST == ulVtbDataFlag)
    {
        ulMailboxBufAddr    = (g_aulR99MailboxVtbAddr[ulMailboxBufIndex]);

        usVtbDataLen        = HPA_Read16Reg(ulMailboxBufAddr + sizeof(VOS_UINT16));

        if (((usVtbDataLen*4) <= WTTFPHY_MAX_PHY_DATA_IND_VTB_DATA_LEN) && (usVtbDataLen > 0))
        {
            HPA_GetDspMem((VOS_CHAR *)ulMailboxBufAddr, (VOS_CHAR *)pucDataIndBuf,
                                    WTTFPHY_MAX_PHY_DATA_IND_VTB_HEAD_LEN+(usVtbDataLen*4));

            *pusDataLen = (VOS_UINT16)(usVtbDataLen*4);

            return VOS_TRUE;
        }
    }

    return  VOS_FALSE;
}
VOS_VOID OM_LoopTestProc(VOS_VOID)
{
    VOS_UINT32                              ulMailboxBufIndex;
    VOS_UINT32                              ulResult;
    VOS_UINT16                              usDataLen;
    WTTFPHY_PHY_DATA_REQ_BUF_STRU           *pstMacPhyDataReqBuf;
    WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU    *pstPhyDataIndHead;
/*lint -e813 修改人:dongtinghuan;检视人:徐铖 51137 */
    APP_OM_W_BER_DATA_IND_STRU              stBerDataInd;
/*lint +e813 修改人:dongtinghuan;检视人:徐铖 51137 */

    if (MAIL_BOX_PROTECTWORD_SND != g_ulOmNosigEnable)
    {
        return;
    }

    /*译码邮箱有两组, 首先获取译码邮箱索引*/

    ulMailboxBufIndex = OM_GetR99MailboxIndex();

    if ( OM_R99_DECODE_BUF_INDEX_INIT == ulMailboxBufIndex )
    {
       return ;
    }

    ulResult = OM_R99MailboxRead(g_aucOmR99MBData, &usDataLen, ulMailboxBufIndex);

    if (VOS_TRUE == ulResult)
    {
        /*下行控制参数写入上行信道共享缓存中*/
        pstMacPhyDataReqBuf = (WTTFPHY_PHY_DATA_REQ_BUF_STRU *)(WTTFPHY_MAC_PHY_DATA_REQ_ADDR);

        pstPhyDataIndHead = (WTTFPHY_MAC_DATA_IND_CTRL_PARAM_STRU *)&g_aucOmR99MBData[0];

        pstMacPhyDataReqBuf->enMutexFlag = WTTFPHY_MUTEX_DATA_EXIST;
        pstMacPhyDataReqBuf->enCctrchID  = WTTFPHY_UL_DPCH;
        pstMacPhyDataReqBuf->enMsgName   = ID_MAC_PHY_DATA_REQ;
        pstMacPhyDataReqBuf->usUlTrchCnt = OM_UL_TRANS_CHANNEL_NUM ;

        /*上行信道CFN为当前CFN+1*/
        pstMacPhyDataReqBuf->usCFN = (g_ucHpaCfnRead + 1);

        /*Tfi取值[1,32],传输信道标识*/
        pstMacPhyDataReqBuf->astUlTrchDataInfo[0].usTfi = pstPhyDataIndHead->astDlTrchDataInfo[0].ucTfi;
        /* TrchId取值[0,63],传输格式标识，这里固定为1 */
        pstMacPhyDataReqBuf->astUlTrchDataInfo[0].usTrchId = g_ausTrchId[0];


        pstMacPhyDataReqBuf->astUlTrchDataInfo[1].usTfi = pstPhyDataIndHead->astDlTrchDataInfo[1].ucTfi ;
        pstMacPhyDataReqBuf->astUlTrchDataInfo[1].usTrchId = g_ausTrchId[1];

        pstMacPhyDataReqBuf->ulSerialNum = g_ulOmSerialNum;
        g_ulOmSerialNum++;

        pstMacPhyDataReqBuf->usDataLen = usDataLen;

        if (g_stOmNoSigCtrl.ulCurFrameNum < g_stOmNoSigCtrl.ulTotalFrameNum)
        {
            stBerDataInd.usTotalFrameNum = (VOS_UINT16)g_stOmNoSigCtrl.ulTotalFrameNum;
            stBerDataInd.usCurFrameNum   = (VOS_UINT16)g_stOmNoSigCtrl.ulCurFrameNum;
            g_stOmNoSigCtrl.ulCurFrameNum++;

            if (OM_BER_DATA_MAX_SIZE < (usDataLen + WTTFPHY_MAX_PHY_DATA_IND_VTB_HEAD_LEN))
            {
                stBerDataInd.ulDataLen = OM_BER_DATA_MAX_SIZE;

                LogPrint1("OM_LoopTestProc: BER data is too large %d.\r\n", usDataLen);
            }
            else
            {
                stBerDataInd.ulDataLen = (VOS_UINT32)usDataLen + WTTFPHY_MAX_PHY_DATA_IND_VTB_HEAD_LEN;
            }

            VOS_MemCpy(stBerDataInd.aucData, g_aucOmR99MBData, stBerDataInd.ulDataLen);

            stBerDataInd.usLength = sizeof(stBerDataInd) - VOS_OM_HEADER_LEN;
            OM_SendContentChannel((OM_LOGIC_CHANNEL_ENUM_UINT32)OM_LOGIC_CHANNEL_CBT, g_stOmNoSigCtrl.ucFuncType, (OM_APP_MSG_EX_STRU*)&stBerDataInd, OM_APP_W_BER_DATA_IND);
        }

        HPA_PutDspMem((VOS_CHAR*)&g_aucOmR99MBData[WTTFPHY_MAX_PHY_DATA_IND_VTB_HEAD_LEN],
                                      (VOS_CHAR*)pstMacPhyDataReqBuf->aucData, usDataLen);

    }

    OM_R99MailboxResetMutexFlag(ulMailboxBufIndex);

    return;
}

/*****************************************************************************
 函 数 名  : OM_NoSigCtrlInit
 功能描述  : 非信令控制结构初全局变量始化
 输入参数  : 无

 输出参数  : 无
 返 回 值  : VOID
*****************************************************************************/
VOS_VOID OM_NoSigCtrlInit(VOS_VOID)
{
    g_stOmNoSigCtrl.usEnableReport = VOS_FALSE;
    g_stOmNoSigCtrl.usRcvMsgPrimId = 0;
    g_stOmNoSigCtrl.ucFuncType     = OM_NOSIG_FUNC;
    g_stOmNoSigCtrl.usSyncStatus   = OM_W_SYNC_STATUS_UNKNOWN;
    g_stOmNoSigCtrl.ulTotalFrameNum= 0;
    g_stOmNoSigCtrl.ulCurFrameNum  = 0;
}


VOS_UINT32 OM_NoSigPidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    if (VOS_IP_LOAD_CONFIG == ip)
    {
        /* 分配互斥信号量 */
        if (VOS_OK != VOS_SmBCreate( "NOSIG", 0, VOS_SEMA4_FIFO, &(g_stOmNoSigCtrl.ulNoSigSyncSem)))
        {
            return VOS_ERR;
        }

        /* 初始化非信令控制结构 */
        OM_NoSigCtrlInit();
    }

    return VOS_OK;
}


VOS_UINT32 OM_NoSigFidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ulRslt;

    if (VOS_IP_LOAD_CONFIG == ip)
    {
        ulRslt = VOS_RegisterPIDInfo(WUEPS_PID_NOSIG,
                                    (Init_Fun_Type)OM_NoSigPidInit,
                                    (Msg_Fun_Type)OM_NoSigMsgProc);
        if( VOS_OK != ulRslt )
        {
            return VOS_ERR;
        }

        ulRslt = VOS_RegisterTaskPrio(WUEPS_FID_NOSIG, COMM_NOSIG_TASK_PRIO);

        if( VOS_OK != ulRslt )
        {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

#endif //(FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

