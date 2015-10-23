/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : OmAgent.c
  版 本 号      : 初稿
  作    者      : 甘兰47350
  生成日期      : 2011年7月4日
  最近修改      :
  功能描述      : 该C文件给出了OM AGENT模块的实现
  函数列表      :
  修改历史      :
  1.日    期    : 2011年7月1日
    作    者    : 甘兰47350
    修改内容    : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "Omappagent.h"
#include "NVIM_Interface.h"
#include "pslog.h"
#include "om.h"
#include "FileSysInterface.h"
#include "omfilesystem.h"
#include "omnvinterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/*lint -e767 修改人：甘兰47350；检视人：李霄46160；原因简述：LOG方案设计需要*/
#define    THIS_FILE_ID        PS_FILE_ID_OM_AGENT_C
/*lint +e767 修改人：甘兰47350；检视人：lixiao；*/

typedef VOS_VOID (*OM_AGENT_PROC)(MsgBlock *pMsg);

typedef struct
{
    VOS_UINT32      ulPrimId;           /*Indicates current msg type.*/
    OM_AGENT_PROC   pfFun;              /*Reserves the pointer of function handles current msg.*/
}OM_AGENT_ITME_STRU;

#if (RAT_GU == RAT_MODE)
extern VOS_UINT32 NV_RestoreResult(VOS_VOID);
extern void  MNTN_RecordVersionInfo(void);
#endif


VOS_VOID OM_SendConfigMsg(VOS_UINT32 ulRcvPid, MsgBlock* pstMsg)
{
    MsgBlock    *pstDestMsg;

    pstDestMsg = VOS_AllocMsg(WUEPS_PID_OM, pstMsg->ulLength);

    if (VOS_NULL_PTR == pstDestMsg)
    {
        return;
    }

    VOS_MemCpy(pstDestMsg, pstMsg, VOS_MSG_HEAD_LENGTH + pstMsg->ulLength);

    pstDestMsg->ulSenderPid   = WUEPS_PID_OM;
    pstDestMsg->ulReceiverPid = ulRcvPid;

    (VOS_VOID)VOS_SendMsg(WUEPS_PID_OM, pstDestMsg);

    return;
}


VOS_UINT32 OM_OledClear(VOS_VOID)
{
    OM_OLED_CLEAR_REQ_STRU    *pstOledCLearMsg;

    /* 分配消息内存 */
    pstOledCLearMsg = (OM_OLED_CLEAR_REQ_STRU*)VOS_AllocMsg(CCPU_PID_OMAGENT,
                    sizeof(OM_OLED_CLEAR_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstOledCLearMsg)
    {
        return VOS_ERR;
    }

    pstOledCLearMsg->ulReceiverPid = ACPU_PID_OMAGENT;
    pstOledCLearMsg->usPrimId      = OM_OLED_CLEAR_REQ;

    /* 将消息发送给ACPU */
    return VOS_SendMsg(CCPU_PID_OMAGENT, pstOledCLearMsg);
}


VOS_UINT32 OM_OledDisplay(VOS_UINT16 usXPos, VOS_UINT16 usYPos, VOS_CHAR *pcString)
{
    VOS_UINT32                  ulLen;
    OM_OLED_DISPLAY_REQ_STRU    *pstOledMsg;

    /* 参数检测 */
    if (VOS_NULL_PTR == pcString)
    {
        return VOS_ERR;
    }

    /* 获取字符串的长度 */
    ulLen = VOS_StrLen(pcString);

    /* 消息内容中需要包括\0 */
    ulLen++;

    /* 分配消息内存 */
    pstOledMsg = (OM_OLED_DISPLAY_REQ_STRU*)VOS_AllocMsg(CCPU_PID_OMAGENT,
                    (sizeof(OM_OLED_DISPLAY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) + ulLen);

    if (VOS_NULL_PTR == pstOledMsg)
    {
        return VOS_ERR;
    }

    pstOledMsg->ulReceiverPid = ACPU_PID_OMAGENT;
    pstOledMsg->usPrimId      = OM_OLED_DISPLAY_REQ;
    pstOledMsg->usXPos        = usXPos;
    pstOledMsg->usYPos        = usYPos;

    VOS_MemCpy((VOS_UINT8*)pstOledMsg + sizeof(OM_OLED_DISPLAY_REQ_STRU), pcString, ulLen);

    /* 将消息发送给ACPU */
    return VOS_SendMsg(CCPU_PID_OMAGENT, pstOledMsg);
}
VOS_UINT32 OM_HsicConnectReq(VOS_VOID)
{
    OM_HSIC_CONNECT_REQ_STRU           *pstOmHsicMsg;

    /* 分配消息内存 */
    pstOmHsicMsg = (OM_HSIC_CONNECT_REQ_STRU*)VOS_AllocMsg(CCPU_PID_OMAGENT,
                                                           sizeof(OM_HSIC_CONNECT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstOmHsicMsg)
    {
        return VOS_ERR;
    }

    pstOmHsicMsg->ulReceiverPid = ACPU_PID_OMAGENT;
    pstOmHsicMsg->usPrimId      = OM_HSIC_CONNECT_REQ;

    /* 将消息发送给ACPU */
    return VOS_SendMsg(CCPU_PID_OMAGENT, pstOmHsicMsg);
}


VOS_VOID OM_ErrorLogProc(MsgBlock* pMsg)
{
    ERRORLOG_REQ_STRU       *pstErrorLogReq;
    ERRORLOG_CNF_STRU       *pstErrorLogCnf;
    VOS_UINT32              ulResult;

    pstErrorLogCnf = (ERRORLOG_CNF_STRU*)VOS_AllocMsg(CCPU_PID_OMAGENT,
                            sizeof(ERRORLOG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 分配消息失败 */
    if (VOS_NULL_PTR == pstErrorLogCnf)
    {
        return;
    }

    pstErrorLogReq = (ERRORLOG_REQ_STRU*)pMsg;

    /* 执行ACPU发来的ErrorLog请求 */
    ulResult = MNTN_ErrorLog((VOS_CHAR*)pstErrorLogReq->aucFileName, pstErrorLogReq->ulFileId,
                            pstErrorLogReq->ulLine, pstErrorLogReq->ulErrNo,
                            pstErrorLogReq->aucData,  pstErrorLogReq->ulLen);

    pstErrorLogCnf->ulReceiverPid  = ACPU_PID_OMAGENT;
    pstErrorLogCnf->usPrimId       = ERRORLOG_CNF;
    pstErrorLogCnf->usResult       = (VOS_UINT16)ulResult;

    (VOS_VOID)VOS_SendMsg(CCPU_PID_OMAGENT, pstErrorLogCnf);

    return;
}

/*****************************************************************************
 Prototype       : OM_WriteLogProc
 Description     : 处理ACPU发来的write Log请求
 Input           : pMsg - 指向消息地址
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2012-05-08
    Author       : zhuli
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_WriteLogProc(MsgBlock* pMsg)
{
    OM_WRITELOG_REQ_STRU    *pstLogReq;
    VOS_UINT32              ulLen;
    FILE                    *fp;

    pstLogReq = (OM_WRITELOG_REQ_STRU*)pMsg;

    fp = DRV_FILE_OPEN((VOS_CHAR*)pstLogReq->aucFileName, "ab+");  /*按照追加方式打开文件*/

    if(VOS_NULL_PTR == fp)                      /*如果文件打开失败说明建立文件也不成功*/
    {
        return;
    }

    DRV_FILE_LSEEK(fp, 0, DRV_SEEK_END);

    ulLen = (VOS_UINT32)DRV_FILE_TELL(fp);

    if(ulLen >= OM_LOG_FILE_MAX_SIZE)            /*文件大小超过限制*/
    {
        DRV_FILE_CLOSE(fp);

        fp = DRV_FILE_OPEN((VOS_CHAR*)pstLogReq->aucFileName, "wb+");  /*清空文件内容*/
    }

    if(VOS_NULL_PTR == fp)                      /*如果文件打开失败说明建立文件也不成功*/
    {
        return;
    }

    DRV_FILE_WRITE(pstLogReq->aucData, sizeof(VOS_CHAR), pstLogReq->ulLen, fp);

    DRV_FILE_CLOSE(fp);

    return;
}
VOS_VOID OM_SdConfigProc(MsgBlock* pMsg)
{
    OM_AUTOCONFIG_CNF_STRU       *pstSdConfigCnf;

    pstSdConfigCnf = (OM_AUTOCONFIG_CNF_STRU*)VOS_AllocMsg(CCPU_PID_OMAGENT,
                            sizeof(OM_AUTOCONFIG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 分配消息失败 */
    if (VOS_NULL_PTR == pstSdConfigCnf)
    {
        return;
    }

    pstSdConfigCnf->ulReceiverPid  = ACPU_PID_OMAGENT;
    pstSdConfigCnf->usPrimId       = OM_AUTOCONFIG_CNF;

    (VOS_VOID)VOS_SendMsg(CCPU_PID_OMAGENT, pstSdConfigCnf);

#if (RAT_GU == RAT_MODE)
    /* PID初始化完后，查看是否是升级后，是则errorlog需要写版本信息 */
    if (VOS_OK == NV_RestoreResult())
    {
        MNTN_RecordVersionInfo();
    }
#endif

    return;
}


VOS_VOID OM_LogShowToFileProc(MsgBlock* pMsg)
{
    /* 将debug 信息记录到文件系统中 */
    OM_LogShowToFile(VOS_FALSE);

    return;
}

/*****************************************************************************
  2 全局变量声明
*****************************************************************************/

/* 处理ACPU发来的请求函数的对应表 */
OM_AGENT_ITME_STRU g_astOmAgentProcTbl[] =
{
    {ERRORLOG_REQ,          OM_ErrorLogProc},
    {OM_AUTOCONFIG_REQ,     OM_SdConfigProc},
    {OM_WRITE_LOG_REQ,      OM_WriteLogProc},
    {OM_RECORD_DBU_INFO_REQ,OM_LogShowToFileProc},
};
VOS_VOID OM_AgentMsgProc(MsgBlock *pMsg)
{
    VOS_UINT16  usPrimId;
    VOS_UINT32  ulTotalNum;
    VOS_UINT32  ulIndex;

    usPrimId = *(VOS_UINT16*)(pMsg->aucValue);

    ulTotalNum = sizeof(g_astOmAgentProcTbl)/sizeof(OM_AGENT_ITME_STRU);

    /* 根据PrimId查找对应的处理函数 */
    for (ulIndex = 0; ulIndex < ulTotalNum; ulIndex++)
    {
        if (usPrimId == g_astOmAgentProcTbl[ulIndex].ulPrimId)
        {
            g_astOmAgentProcTbl[ulIndex].pfFun(pMsg);
            return;
        }
    }

    return;
}
VOS_UINT32 Om_AgentPidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    return VOS_OK;
}


VOS_UINT32 OM_AgentFidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ulRslt;

    if( VOS_IP_LOAD_CONFIG == ip )
    {
        ulRslt = VOS_RegisterPIDInfo(CCPU_PID_OMAGENT,
                                    (Init_Fun_Type)Om_AgentPidInit,
                                    (Msg_Fun_Type)OM_AgentMsgProc);
        if( VOS_OK != ulRslt )
        {
            return VOS_ERR;
        }

        ulRslt = VOS_RegisterMsgTaskPrio(AGENT_FID_OM, VOS_PRIORITY_P6);

        if( VOS_OK != ulRslt )
        {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
