/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : om_comm.c
  版 本 号   : 初稿
  作    者   : 苏庄銮 59026
  生成日期   : 2011年6月16日
  最近修改   :
  功能描述   :
  函数列表   :
              OM_COMM_Init
              OM_COMM_InitFuncTable
              OM_COMM_IsrNmi
              OM_COMM_IsrSysException
              OM_COMM_MsgExtDefReq
              OM_COMM_PidInit
              OM_COMM_PidProc
              OM_COMM_SendTrace
              OM_COMM_SendTrans
  修改历史   :
  1.日    期   : 2011年6月16日
    作    者   : 苏庄銮 59026
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "om.h"
#include "fsm.h"
#include "med_drv_timer_hifi.h"
#include "med_drv_mb_hifi.h"
#include "ucom_nv.h"
#include "codec_typedefine.h"


#include "hifidrvinterface.h"
#include "ucom_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_OM_COMM_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* OM消息状态机(经简化,不进行状态切换) */
STA_STRU g_stOmStatusDesc;

/* OM消息处理表,此处没有排序 */
ACT_STRU g_astOmMsgFuncTable[] =
{
    /* 子项定义格式: PID(reserve), 消息类型, 对应处理函数 */
    FSM_ACT_TBL_ITEM(0x0000, ID_OM_CODEC_MSG_HOOK_REQ,    OM_MSGHOOK_MsgCfgMsgHookReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_OM_CODEC_CPU_VIEW_REQ,    OM_CPUVIEW_MsgCfgReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_OM_CODEC_SET_LOG_REQ,     OM_LOG_MsgSetLogReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_OM_CODEC_EXT_DEF_REQ,     OM_COMM_MsgExtDefReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_TIMER_CODEC_CPUVIEW_RPT_IND,  OM_CPUVIEW_MsgRptInd),
    FSM_ACT_TBL_ITEM(0x0000, ID_OM_CODEC_OM_CONNECT_CMD,    OM_COMM_MsgOmConnectCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_OM_CODEC_OM_DISCONNECT_CMD, OM_COMM_MsgOmDisConnectCmd)
};

/* HIFI上报消息序列号记录全局变量，从0开始记录 */
VOS_UINT32 g_uwOmMsgSeqNum = 0;

/* 记录OM连接状态 */
UCOM_SET_UNINIT
VOS_UINT16   g_OMConnectionStatus = CODEC_SWITCH_OFF;

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : OM_COMM_PidInit
 功能描述  : DSP_PID_OM的PID初始化函数
 输入参数  : enum VOS_INIT_PHASE_DEFINE enInitPhrase - 注册PID的过程编号
 输出参数  : 无
 返 回 值  : VOS_UINT32 - VOS_OK / VOS_ERR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月16日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_COMM_PidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase)
{
    VOS_UINT32                          uwCheck = VOS_OK;
    CODEC_NV_PARA_HIFI_TRACE_CFG_STRU     stHifiTraceCfg;

    switch (enInitPhrase)
    {
        case VOS_IP_LOAD_CONFIG:
        {
            /* OM状态机初始化 */
            OM_COMM_InitFuncTable();
            break;
        }
        case VOS_IP_FARMALLOC:
        {
            /* 所有PID初始化后配置OM模块 */

            /* 读取HIFI TRACE相关NV项 */
            UCOM_MemSet(&stHifiTraceCfg, 0, sizeof(stHifiTraceCfg));

            uwCheck  = UCOM_NV_Read(en_NV_HifiTraceCfg,
                                    &stHifiTraceCfg,
                                     sizeof(stHifiTraceCfg));

            uwCheck += OM_LOG_DefaultCfg(&stHifiTraceCfg.stLogHookCfg);

            uwCheck += OM_MSGHOOK_DefaultCfg(&stHifiTraceCfg.stMsgHookCfg);

            break;
        }
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    if (VOS_OK != uwCheck)
    {
        return VOS_ERR;
    }
    else
    {
        return VOS_OK;
    }
}

/*****************************************************************************
 函 数 名  : OM_COMM_PidProc
 功能描述  : DSP_PID_OM的消息处理函数，处理函数表为g_astOmMsgFuncTable
 输入参数  : MsgBlock *pstOsaMsg - 分发到DSP_PID_OM的消息
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月16日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_COMM_PidProc(MsgBlock *pstOsaMsg)
{
    VOS_UINT16     *puhwMsgId           = VOS_NULL;
    ACT_STRU       *pstRetAct           = VOS_NULL;
    VOS_UINT16      uhwOffset           = 0;
    STA_STRU       *pstMsgStatus        = OM_COMM_GetMsgStatusPtr();
    ACT_STRU        stKeyAct;

    /* 获取消息ID */
    puhwMsgId = (VOS_UINT16*)pstOsaMsg->aucValue;

    if ( (ID_OM_CODEC_OM_DISCONNECT_CMD  != *puhwMsgId)
       &&(ID_TIMER_CODEC_CPUVIEW_RPT_IND != *puhwMsgId))
    {
        uhwOffset   = 24;
        puhwMsgId = (VOS_UINT16*)((VOS_UINT32)pstOsaMsg->aucValue + uhwOffset);
    }

    stKeyAct.ulEventType = *puhwMsgId;

    /* 调用二分查找函数bsearch在事件处理表中查找相应的事件处理函数 */
    /* 如果找不到相应的事件处理函数,返回NULL */
    pstRetAct = (ACT_STRU*)FSM_Bsearch((VOS_INT16 *)(&stKeyAct),
                                       (VOS_UINT8 *)(pstMsgStatus->pActTable),
                                       pstMsgStatus->ulSize,
                                       sizeof(ACT_STRU),
                                       FSM_ActCompare);

    /* 若有消息处理函数与之对应,调用之 */
    if (VOS_NULL != pstRetAct)
    {
        (VOS_VOID)(*pstRetAct->pfActionFun)((VOS_UCHAR *)pstOsaMsg + uhwOffset);
    }
    else
    {
        /* 记录异常,DSP_PID_OM收到未知消息,未处理 */
        OM_LogWarning1(OM_COMM_PidProc_UnknownMsg, stKeyAct.ulEventType);
    }
}

/*****************************************************************************
 函 数 名  : OM_COMM_InitFuncTable
 功能描述  : OM消息状态机初始化函数
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月16日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_COMM_InitFuncTable(VOS_VOID)
{
    STA_STRU       *pstStatusDec        = OM_COMM_GetMsgStatusPtr();

    /* 初始化状态机，建立动作表链接*/
    pstStatusDec->pActTable  = OM_COMM_GetMsgFuncTbl();
    pstStatusDec->ulSize     = sizeof(OM_COMM_GetMsgFuncTbl())/sizeof(ACT_STRU);

    /* 将动作表按消息ID进行排序，从低到高顺序排列 */
    FSM_Sort((VOS_UINT8 *)pstStatusDec->pActTable,
             pstStatusDec->ulSize,
             sizeof(ACT_STRU),
             FSM_ActCompare);
}

/*****************************************************************************
 函 数 名  : OM_COMM_Init
 功能描述  : 可维可测模块初始化函数
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月21日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_COMM_Init(VOS_VOID)
{
    OM_COMM_SetMsgSeqNum(0);
    OM_LOG_Init();
    OM_MSGHOOK_Init();
    OM_CPUVIEW_Init();
}

/*****************************************************************************
 函 数 名  : OM_COMM_SendTrans
 功能描述  : 把指定数据块以透明消息格式通过OM通道发送
 输入参数  : OM_DATA_BLK_STRU *pstDataBlk - 待发送数据块
             VOS_UINT32 uwBlkCnt          - 待发送数据块数目
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_COMM_SendTrans(UCOM_DATA_BLK_STRU *pstDataBlk, VOS_UINT32 uwBlkCnt)
{
    VOS_UINT32                          uwLength;           /* 待发送数据长度 */
    VOS_UINT32                          uwCnt;              /* 循环计数 */

    /* 参数合法性检查 */
    if (   (VOS_NULL == pstDataBlk)
        || (       0 == uwBlkCnt)
        || (uwBlkCnt >  OM_COMM_TRANS_BLK_MAX_NUM))
    {
        return;
    }

    /* 统计消息总长度 */
    uwLength = 0;
    for (uwCnt = 0; uwCnt < uwBlkCnt; uwCnt++)
    {
        uwLength += pstDataBlk[uwCnt].uwSize;
    }

    /* 通过跨核VOS消息邮箱发送透明消息 */
    #ifdef _OM_MAILBOX_REPORT
    {
        OM_TRANS_PASS_HEADER_STRU      *pstTrans = VOS_NULL;
        MsgBlock                       *pstMsg   = VOS_NULL;
        VOS_UCHAR                      *pucDes   = VOS_NULL;

        pstMsg = VOS_AllocMsg(DSP_PID_HIFI_OM, uwLength + sizeof(OM_TRANS_PASS_HEADER_STRU));
        if (VOS_NULL == pstMsg)
        {
            return;
        }

        /* 填充透明消息 */
        pstTrans                    = (OM_TRANS_PASS_HEADER_STRU *)pstMsg->aucValue;
        pstTrans->usTransPrimId     = OM_TRANS_PRIM_ID;
        pstTrans->usFuncType        = OM_TRANS_FUNC_TYPE;
        pstTrans->usLength          = (VOS_UINT16)uwLength + OM_APP_HEADER_TAIL_LEN;
        pstTrans->ulSn              = OM_COMM_GetAndIncMsgSeqNum();    /* 各组件分别维护其上报的消息序列号 */
        pstTrans->ulTimeStamp       = DRV_TIMER_ReadSysTimeStamp();

        pstMsg->uwReceiverPid       = UCOM_PID_PS_OM;
        pstMsg->uwLength            = uwLength + sizeof(OM_TRANS_PASS_HEADER_STRU);

        /* 复制消息内容 */ /*lint --e(416)*/
        pucDes = pstMsg->aucValue + sizeof(OM_TRANS_PASS_HEADER_STRU);
        for (uwCnt = 0; uwCnt < uwBlkCnt; uwCnt++)
        {
            UCOM_MemCpy(pucDes, pstDataBlk[uwCnt].pucData, pstDataBlk[uwCnt].uwSize);
            pucDes += pstDataBlk[uwCnt].uwSize;
        }

        /* 发送消息, 若失败由邮箱驱动保证错误处理 */
        (VOS_VOID)VOS_SendMsg(DSP_PID_HIFI_OM, pstMsg);
    }

    /* 通过SOCP发送透明消息 */
    #else
    if (CODEC_SWITCH_ON == OM_COMM_GetOMConnStatus())
    {
        UCOM_DATA_BLK_STRU          astDataBlk[OM_COMM_TRANS_BLK_MAX_NUM + 1];
        OM_TRANS_HEADER_STRU        stTransHeader;

        stTransHeader.usFuncType    = OM_TRANS_FUNC_TYPE;
        stTransHeader.usLength      = (VOS_UINT16)uwLength + OM_APP_HEADER_TAIL_LEN;
        stTransHeader.ulSn          = OM_COMM_GetAndIncMsgSeqNum(); /* 各组件分别维护其上报的消息序列号 */
        stTransHeader.ulTimeStamp   = DRV_TIMER_ReadSysTimeStamp();

        astDataBlk[0].pucData       = (VOS_UCHAR*)&stTransHeader;
        astDataBlk[0].uwSize        = sizeof(OM_TRANS_HEADER_STRU);
        UCOM_MemCpy(&astDataBlk[1], pstDataBlk, uwBlkCnt * sizeof(UCOM_DATA_BLK_STRU));

        /* SOCP写入失败则记录内部错误 */
        if (VOS_OK != DRV_SOCP_Write(astDataBlk, uwBlkCnt + 1))
        {
            OM_LOG_InnerRecord(OM_COMM_SendTrans_SocpWriteFailed, (VOS_UINT16)THIS_FILE_ID, (VOS_UINT16)__LINE__);
        }
    }
    #endif

}


/*****************************************************************************
 函 数 名  : OM_COMM_SendTrace
 功能描述  : 把指定数据以TRACE消息格式通过OM通道发送
 输入参数  : VOS_UCHAR *pucTrace - 待发送数据
             VOS_UINT32 uwLength - 待发送数据长度
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_COMM_SendTrace(VOS_UCHAR *pucTrace, VOS_UINT32 uwLength)
{
    UCOM_DATA_BLK_STRU          astDataBlk[OM_COMM_TRACE_BLK_MAX_NUM];
    OM_TRACE_HEADER_STRU        stTrace;

    /* 参数合法性检查 */
    if ((VOS_NULL == pucTrace) || (0 == uwLength) || (CODEC_SWITCH_OFF == OM_COMM_GetOMConnStatus()))
    {
        return;
    }

    /* 填充TRACE头 */
    stTrace.usFuncType          = OM_TRACE_FUNC_TYPE;
    stTrace.usLength            = (VOS_UINT16)uwLength + OM_APP_HEADER_TAIL_LEN + OM_TRACE_TAIL_LEN ;
    stTrace.ulSn                = OM_COMM_GetAndIncMsgSeqNum(); 	/* 各组件分别维护其上报的消息序列号 */
    stTrace.ulTimeStamp         = DRV_TIMER_ReadSysTimeStamp();
    stTrace.usPrimId            = OM_TRACE_PRIM_ID;
    stTrace.usToolId            = 0;

    /* 组织TRACE数据块 */
    astDataBlk[0].pucData       = (VOS_UCHAR*)&stTrace;
    astDataBlk[0].uwSize        = sizeof(OM_TRACE_HEADER_STRU);
    astDataBlk[1].pucData       = pucTrace;
    astDataBlk[1].uwSize        = uwLength;

    /* SOCP写入失败则记录内部错误 */
    if (VOS_OK != DRV_SOCP_Write(astDataBlk, OM_COMM_TRACE_BLK_MAX_NUM))
    {
        OM_LOG_InnerRecord(OM_COMM_SendTrace_SocpWriteFailed, (VOS_UINT16)THIS_FILE_ID, (VOS_UINT16)__LINE__);
    }

}

/*****************************************************************************
 函 数 名  : OM_COMM_MsgExtDefReq
 功能描述  : 扩展命令接口，HIFI上OM与PC侧SDT工具通过此接口可扩展多个命令
 输入参数  : VOS_VOID *pvOsaMsg - OM_MED_EXT_DEF_REQ_STRU消息
 输出参数  : 无
 返 回 值  : VOS_UINT32 - UCOM_RET_SUCC
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_COMM_MsgExtDefReq(VOS_VOID *pvOsaMsg)
{
    CODEC_OM_EXT_DEF_CNF_STRU     stCnfMsg;
    OM_CODEC_EXT_DEF_REQ_STRU    *pstReqMsg;

    UCOM_MemSet(&stCnfMsg, 0, sizeof(stCnfMsg));

    pstReqMsg = (OM_CODEC_EXT_DEF_REQ_STRU*)pvOsaMsg;

    switch (pstReqMsg->enExtCmd)
    {
        /* 暂未实现 */
        case OM_CODEC_EXT_CMD_DEFAULT:
        {
            stCnfMsg.enExtCmd   = pstReqMsg->enExtCmd;
            stCnfMsg.uhwExtLen  = 0;
            break;
        }
        case OM_CODEC_EXT_ENABLE_OM:
        {
            DRV_SOCP_Init();
            OM_COMM_SetOMConnStatus(CODEC_SWITCH_ON);
            break;
        }
        case OM_CODEC_EXT_AUDIO_ENHANCE_START_VOIP:
        case OM_CODEC_EXT_AUDIO_ENHANCE_STOP_VOIP:
        case OM_CODEC_EXT_AUDIO_ENHANCE_SET_DEVICE:
        case OM_CODEC_EXT_AUDIO_ENHANCE_START_DEFAULT:
        case OM_CODEC_EXT_AUDIO_ENHANCE_STOP_DEFAULT:
        default:
        {
            stCnfMsg.enExtCmd   = pstReqMsg->enExtCmd;
            stCnfMsg.uhwExtLen  = 1;
            stCnfMsg.auhwExtData[0] = UCOM_RET_ERR_MSG;
            break;
        }
    }

    stCnfMsg.uhwMsgId = ID_CODEC_OM_EXT_DEF_CNF;

    /* 回复消息 */
    OM_COMM_SendTranMsg(&stCnfMsg, sizeof(stCnfMsg));

    OM_LogInfo(OM_COMM_MsgExtDefReq_OK);

    return UCOM_RET_SUCC;

}

/*****************************************************************************
 函 数 名  : OM_COMM_IsrSysException
 功能描述  : 用户的异常处理函数,注册后在用户模式下出现异常时调用
 输入参数  : VOS_UINT32 uwExceptionNo - 异常信息
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_COMM_IsrSysException (VOS_UINT32 uwExceptionNo)
{
    /* reserve */
}

/*****************************************************************************
 函 数 名  : OM_COMM_IsrNmi
 功能描述  : NMI中断处理函数，注册后处理NMI中断
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_COMM_IsrNmi(VOS_VOID)
{
    /* Reserved */
}

/*****************************************************************************
 函 数 名  : OM_COMM_ConnectedInd
 功能描述  : OM模块报告连接状态
 输入参数  : VOS_VOID *pvOsaMsg
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月7日
    作    者   : W00164657
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_COMM_MsgOmConnectCmd(VOS_VOID *pvOsaMsg)
{
    OM_COMM_SetOMConnStatus(CODEC_SWITCH_ON);

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : OM_COMM_DisConnectedInd
 功能描述  : OM模块报告连接状态
 输入参数  : VOS_VOID *pvOsaMsg
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月7日
    作    者   : W00164657
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_COMM_MsgOmDisConnectCmd(VOS_VOID *pvOsaMsg)
{
    OM_COMM_SetOMConnStatus(CODEC_SWITCH_OFF);

    return UCOM_RET_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


