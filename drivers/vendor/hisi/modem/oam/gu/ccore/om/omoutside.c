/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : OmOutside.c
  版 本 号      : 初稿
  作    者      : 甘兰47350
  生成日期      : 2008年8月2日
  最近修改      :
  功能描述      : 该C文件给出了OM模块的实现
  函数列表      :
  修改历史      :
  1.日    期    : 2008年5月3日
    作    者    : 甘兰47350
    修改内容    : 创建文件

******************************************************************************/
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "omprivate.h"
#include "DrvInterface.h"
#include "NVIM_Interface.h"
#include "ScInterface.h"
#include "DspInterface.h"
#include "omerrorlog.h"

#ifdef  __LDF_FUNCTION__
#include "apminterface.h"
#endif

#if (VOS_WIN32 == VOS_OS_VER)
#include "OamSpecTaskDef.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/*lint -e767 修改人:甘兰 47350;原因:Log打印 */
#define    THIS_FILE_ID        PS_FILE_ID_OM_OUTSIDE_C
/*lint +e767 修改人:甘兰 47350;*/

/*****************************************************************************
  2 外部引用变量定义
*****************************************************************************/
/* DSP CS-ERRLOG PrimId */
#define OM_RECORD_DSP_ERRORLOG         0x5100

#define OM_DSP_DCXO_UPDATE_NV_DATA     0x5101

/* DSP CS-ERRLOG MAX LENGTH */
#define OM_ERRORLOG_DSP_LEN_MAX        (1024)

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16 usPrimId;
    VOS_UINT16 usResv;
    VOS_UINT32 ulTotalLength;
    VOS_UINT32 ulErrNo;
    VOS_UINT8  aucData[4];
}DSP_ERROR_LOG_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16 usPrimId;
    VOS_UINT16 usCount;
    VOS_UINT8  aucNVData[4];
}DSP_WRITE_NV_STRU;

typedef struct
{
    VOS_UINT16 usNvId;
    VOS_UINT16 usNvLen;
    VOS_UINT8  aucNVData[4];
}DSP_WRITE_NV_CONTEXT_STRU;

extern VOS_VOID OM_MmaMsgProc(MsgBlock *pstMsg);

#if(FEATURE_ON == FEATURE_PTM)
/* 用于保存商用Err LOG 注册回调函数指针 */
OM_REGISTER_PROJECT_CTX_STRU            g_astFTMCallBackFuncCtx[]={
                                        {OM_ERR_LOG_MOUDLE_ID_IMS,  VOS_NULL_PTR},
                                        {OM_ERR_LOG_MOUDLE_ID_IMSA, VOS_NULL_PTR},
                                       };

/******************************************************************************
函数名  ：OM_RegisterGetData
功能描述：OM Ccpu接收各组件上报工程模式数据到APP回调函数注册接口
输入参数：enModuleProjectCallback：回调组建编号，固定为OM_VOLTE_MOUDLE_PROJECT
          pFuncProjectCallbak    ：回调函数指针
输出参数：NA
返回值  ：VOS_UINT32
修改历史：
1.  日期    ：2014年1月2日
作者    ：
修改内容：新生成函数
说明:
    1、工程模式主动上报开关通知各组件，见结构体APP_OM_CTRL_STATUS_STRU，Data除去OMHead、ulMsgModuleID以外数据域
    2、工程模式命令上报消息通知各组件，Data域包含完整的OmHeard和OMPayload
*****************************************************************************/
VOS_UINT32 OM_RegisterGetData(OM_ERR_LOG_MOUDLE_ID_ENUM_UINT32 enProjectModule, pFuncOMGetData pFuncProjectCallback)
{
    VOS_UINT32                          ulIndex;
    
    if (VOS_NULL_PTR == enProjectModule)
    {        
        OM_CCPU_ERR_LOG1("OM_RegisterGetData:ModuleID is %d, func name is null.\r\n", enProjectModule);
        return OM_APP_MSG_MODULE_ID_ERR;
    }

    for (ulIndex=0; ulIndex<(sizeof(g_astFTMCallBackFuncCtx) / sizeof(g_astFTMCallBackFuncCtx[0])); ulIndex++)
    {
        if (g_astFTMCallBackFuncCtx[ulIndex].enProjectModule == enProjectModule)
        {
            break;
        }
    }

    if( (sizeof(g_astFTMCallBackFuncCtx) / sizeof(g_astFTMCallBackFuncCtx[0])) <= ulIndex )
    {
        OM_CCPU_ERR_LOG1("OM_RegisterGetData:ModuleID is error %d\r\n", enProjectModule);
        return OM_APP_MSG_MODULE_ID_ERR;
    }

    if (VOS_NULL_PTR == g_astFTMCallBackFuncCtx[ulIndex].pSendUlAtFunc)
    {
        g_astFTMCallBackFuncCtx[ulIndex].pSendUlAtFunc = pFuncProjectCallback;
        return VOS_OK;
    }
    
    OM_CCPU_ERR_LOG1("OM_RegisterGetData:callback Func is Registed,ModuleID is %d\r\n", enProjectModule);
    return VOS_ERR;
    
}

/******************************************************************************
函数名  ：OM_GetData
功能描述：OM Ccpu接收各组件上报工程模式数据
输入参数：enProjectModule    ：组建编号
          pData              ：发送数据
          ulLen              ：发送数据长度
输出参数：NA
返回值  ：VOS_OK/VOS_ERR
修改历史：
1.  日期    ：2014年1月2日
作者    ：
修改内容：新生成函数
说明：pData从OMHeader的MsgType域开始。相当于调用组件预先申请Header部分，其中Header部分的SN字段由COMM填写
*****************************************************************************/
VOS_UINT32 OM_GetData(OM_ERR_LOG_MOUDLE_ID_ENUM_UINT32 enProjectModule, VOS_VOID *pData, VOS_UINT32 ulLen)
{
    OM_ALARM_MSG_HEAD_STRU             *pstOmHead;
    OM_FTM_REPROT_IND_STRU             *pstOmFtmReportInd;
    VOS_UINT32                          ulAddr;
    VOS_UINT32                          ulIndex;

    for (ulIndex=0; ulIndex<(sizeof(g_astFTMCallBackFuncCtx) / sizeof(g_astFTMCallBackFuncCtx[0])); ulIndex++)
    {
        /* 上报消息时，如果该组件没有注册过回调函数，就不传递他的消息 */
        if ( (g_astFTMCallBackFuncCtx[ulIndex].enProjectModule == enProjectModule)
             && (g_astFTMCallBackFuncCtx[ulIndex].pSendUlAtFunc  != VOS_NULL_PTR) )
        {
            break;
        }
    }

    if ((sizeof(g_astFTMCallBackFuncCtx) / sizeof(g_astFTMCallBackFuncCtx[0])) <= ulIndex)
    {
        OM_CCPU_ERR_LOG1("OM_GetData:ModuleID is error %d\r\n", enProjectModule);
        return OM_APP_MSG_MODULE_ID_ERR;
    }

    if ((VOS_NULL_PTR == pData) || ((sizeof(OM_ALARM_MSG_HEAD_STRU) + sizeof(OM_FTM_HEADER_STRU)) > ulLen))
    {
        OM_CCPU_ERR_LOG2("\r\n OM_GetData: Module ID is %d, Send data is NULL or len is error:%d\n", enProjectModule, ulLen);
        return OM_APP_MSG_LENGTH_ERR;
    }

    /*lint -e40*/
    OM_CCPU_DEBUG_TRACE((VOS_UINT8*)pData, ulLen, OM_CCPU_ERRLOG_RCV);
    /*lint +e40*/

    /* 工程模式相关消息上报给OM */
    pstOmFtmReportInd  = (OM_FTM_REPROT_IND_STRU*)VOS_AllocMsg(WUEPS_PID_OM_CALLBACK, (ulLen + sizeof(VOS_UINT32)));

    if (VOS_NULL_PTR == pstOmFtmReportInd)
    {
        OM_CCPU_ERR_LOG1("OM_GetData:Module ID is %d, malloc msg is fail\r\n", enProjectModule);
        return OM_APP_OMCCPU_ALLOC_MSG_ERR;
    }
    
    pstOmFtmReportInd->ulReceiverPid = ACPU_PID_OM;
    pstOmHead = (OM_ALARM_MSG_HEAD_STRU*)pData;

    /* 消息TYPE简单判断*/
    if (OM_ERR_LOG_MSG_FTM_REPORT == pstOmHead->ulMsgType)
    {
        pstOmFtmReportInd->ulMsgName     = ID_OM_FTM_REPROT_IND;
    }
    else if (OM_ERR_LOG_MSG_FTM_CNF == pstOmHead->ulMsgType)
    {
        pstOmFtmReportInd->ulMsgName     = ID_OM_FTM_REQUIRE_CNF;
    }
    else
    {
        OM_CCPU_ERR_LOG2("OM_GetData:Module ID is %d, Msg Type is Err %d\r\n", 
                     enProjectModule, pstOmHead->ulMsgType);

        VOS_FreeMsg(WUEPS_PID_OM_CALLBACK, pstOmFtmReportInd);

        return OM_APP_MSG_TYPE_ERR;
    }

    /* make pclint happy  begin */
    ulAddr = (VOS_UINT32)(&pstOmFtmReportInd->ulMsgType);
    VOS_MemCpy((VOS_VOID *)ulAddr, (VOS_VOID*)pData, ulLen);
    /* make pclint happy  end */

    (VOS_VOID)VOS_SendMsg(WUEPS_PID_OM_CALLBACK, pstOmFtmReportInd);

    return OM_APP_MSG_OK;
}

/******************************************************************************
函数名  ：OM_ErrLogMsgProc
功能描述：OM Ccpu接收各组件上报工程模式数据
输入参数：pMsg    ：收到商用ErrLog工程模式数据
          
输出参数：NA
返回值  ：VOS_VOID
修改历史：
1.  日期    ：2014年1月2日
作者    ：
修改内容：新生成函数
*****************************************************************************/
VOS_VOID OM_ErrLogMsgProc(MsgBlock* pMsg)
{
    OM_FTM_REQUIRE_STRU                *pstOmFTMMsg;
    OM_FTM_CTRL_IND_STRU               *pstOmFtmCtrlInd; /* 工程模式开关消息:开关打开，下层主动上报 */
    APP_OM_FTM_REQ_STRU                *pstAppOmFtmReq;  /* 工程模式命令消息:请求一次，下层上报一次 */
    APP_OM_CTRL_STATUS_STRU             stAppOmCtrlStatus;
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulResult;
  
    pstOmFTMMsg = (OM_FTM_REQUIRE_STRU*)pMsg;
    switch( pstOmFTMMsg->ulMsgName )
    {
        case ID_OM_FTM_CTRL_IND:
        {
            pstOmFtmCtrlInd = (OM_FTM_CTRL_IND_STRU*)pMsg;
        
            /* 工程模式主动上报相关 */
            stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
            stAppOmCtrlStatus.stOmHeader.ulMsgLen  = sizeof(VOS_UINT32) + sizeof(VOS_UINT16) + 2*sizeof(VOS_UINT8); /* 该结构体长度 */
            stAppOmCtrlStatus.usModemID            = pstOmFTMMsg->usModemID;
            stAppOmCtrlStatus.ucAlmStatus          = pstOmFtmCtrlInd->ucActionFlag;

            /*lint -e40*/
            OM_CCPU_DEBUG_TRACE(((VOS_UINT8*)&stAppOmCtrlStatus), sizeof(stAppOmCtrlStatus), OM_CCPU_ERRLOG_RCV);
            /*lint +e40*/

            for (ulIndex=0; ulIndex<(sizeof(g_astFTMCallBackFuncCtx) / sizeof(g_astFTMCallBackFuncCtx[0])); ulIndex++)
            {
                if (VOS_NULL_PTR == g_astFTMCallBackFuncCtx[ulIndex].pSendUlAtFunc)
                {
                    continue;
                }
                
                stAppOmCtrlStatus.ulMsgModuleID = g_astFTMCallBackFuncCtx[ulIndex].enProjectModule;
                ulResult = g_astFTMCallBackFuncCtx[ulIndex].pSendUlAtFunc(stAppOmCtrlStatus.ulMsgModuleID, 
                                                                        (VOS_VOID *)&stAppOmCtrlStatus, 
                                                                        sizeof(stAppOmCtrlStatus));
                if (VOS_OK != ulResult)
                {
                    OM_CCPU_ERR_LOG2("OM_ErrLogMsgProc:Module ID is %d, return value fail: %d\r\n", 
                                 stAppOmCtrlStatus.ulMsgModuleID, ulResult);
                }
            }

            break;
        }

        case ID_OM_FTM_REQUIRE_IND:
        {
            pstAppOmFtmReq = (APP_OM_FTM_REQ_STRU*)pstOmFTMMsg->aucContent;

            /*lint -e40*/
            OM_CCPU_DEBUG_TRACE((VOS_UINT8*)pstAppOmFtmReq, 
                                (sizeof(APP_OM_FTM_REQ_STRU) - 4*sizeof(VOS_UINT8) + pstAppOmFtmReq->ulProjectLength), 
                                OM_CCPU_ERRLOG_RCV);
            /*lint +e40*/

            /* 工程模式命令上报相关 */
            for (ulIndex=0; ulIndex<(sizeof(g_astFTMCallBackFuncCtx) / sizeof(g_astFTMCallBackFuncCtx[0])); ulIndex++)
            {
                if (g_astFTMCallBackFuncCtx[ulIndex].enProjectModule != pstAppOmFtmReq->ulMsgModuleID)
                {
                    continue;
                }

                if (VOS_NULL_PTR == g_astFTMCallBackFuncCtx[ulIndex].pSendUlAtFunc)
                {
                    continue;
                }

                ulResult = g_astFTMCallBackFuncCtx[ulIndex].pSendUlAtFunc(pstAppOmFtmReq->ulMsgModuleID, 
                                                                          (VOS_VOID *)pstAppOmFtmReq, 
                                                                          (sizeof(APP_OM_FTM_REQ_STRU) + pstAppOmFtmReq->ulProjectLength - sizeof(VOS_UINT32)));

                if (VOS_OK != ulResult)
                {
                    OM_CCPU_ERR_LOG2("OM_ErrLogMsgProc:Module ID is %d, return value fail: %d\r\n", 
                                 pstAppOmFtmReq->ulMsgModuleID, ulResult);
                }
            }
            break;
        }

        default:
            OM_CCPU_ERR_LOG1("OM_ErrLogMsgProc:msg name is err %d\r\n", pstOmFTMMsg->ulMsgName);
            break;
    }

    return ;
}

#endif
VOS_UINT32 WuepsOmCallBackPidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    return VOS_OK;
}


VOS_VOID OM_WriteDspNv(MsgBlock* pMsg)
{
    VOS_UINT32                  usIndex;
    VOS_UINT16                  usCount;
    VOS_UINT16                  usNvId;
    VOS_UINT16                  usNvLen;
    VOS_UINT32                  ulResult;
    DSP_WRITE_NV_STRU           *pstWriteNVMsg;
    DSP_WRITE_NV_CONTEXT_STRU   *pstWrteNvContext;
    VOS_UINT8                   *pucNvData;
    MODEM_ID_ENUM_UINT16        enModemID;

    enModemID = VOS_GetModemIDFromPid(pMsg->ulSenderPid); /*如果DCXO确认双份再调试*/

    pstWriteNVMsg = (DSP_WRITE_NV_STRU *)pMsg;

    usCount = pstWriteNVMsg->usCount;

    pstWrteNvContext = (DSP_WRITE_NV_CONTEXT_STRU *)(pstWriteNVMsg->aucNVData);

    for ( usIndex = 0; usIndex < usCount; usIndex++ )
    {
        usNvId = pstWrteNvContext->usNvId;

        usNvLen = pstWrteNvContext->usNvLen;

        pucNvData = pstWrteNvContext->aucNVData;

        ulResult = NV_WriteEx(enModemID, usNvId, pucNvData, (VOS_UINT32)usNvLen);

        if ( NV_OK != ulResult )
        {
            PS_LOG2(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                "OM_WriteDspNv:NV_Write, NV id: ErrCode :", (VOS_INT32)usNvId, (VOS_INT32)ulResult);

            return;
        }

        pstWrteNvContext = (DSP_WRITE_NV_CONTEXT_STRU *)(pstWrteNvContext->aucNVData + usNvLen);
    }

    return;
}
VOS_VOID OM_DspMsgProc(MsgBlock* pMsg)
{
    VOS_UINT16         usPrimId;
    DSP_ERROR_LOG_STRU *pstMsg;

    usPrimId = *(VOS_UINT16 *)(pMsg->aucValue);

    if (OM_RECORD_DSP_ERRORLOG == usPrimId)
    {
        pstMsg  = (DSP_ERROR_LOG_STRU *)pMsg;

        if(pstMsg->ulTotalLength > OM_ERRORLOG_DSP_LEN_MAX)
        {
            return;
        }

        MNTN_RecordErrorLog(pstMsg->ulErrNo, &pstMsg->ulTotalLength, pstMsg->ulTotalLength);
    }
    else if ( OM_DSP_DCXO_UPDATE_NV_DATA == usPrimId )
    {
        Print(" Start DCXO NV UPDATE!\n");

        OM_WriteDspNv(pMsg);
    }
    else
    {
        /* Make  Pclint happy */
        /* blank */
    }

    return;
}


VOS_VOID OM_HsicInitMsgProc(MsgBlock* pMsg)
{
    SC_PORT_STATUS_ENUM_UINT32          enStatus;

    /* 获取DK文件中端口关联设置 */
    if (SC_ERROR_CODE_NO_ERROR != SC_FAC_GetPortAttrib(&enStatus))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_HsicInitMsgProc:SC_FAC_GetPortAttrib error.");

        return;
    }

    /* DK文件设置不做OM和HSIC端口的关联 */
    if (SC_PORT_STATUS_ON != enStatus)
    {
        return;
    }

    /* 将关联请求发到A核的OMAGENT去处理 */
    if (VOS_OK != OM_HsicConnectReq())
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_HsicInitMsgProc:OM_HsicConnectReq error.");

        return;
    }

    return;
}
VOS_VOID OM_CallBackMsgProc(MsgBlock* pMsg)
{
    OM_REQ_PACKET_STRU                 *pstAppMsg;
    OM_REQUEST_PROCEDURE               *pOmFuncProc;

    if (UEPS_PID_APM == pMsg->ulSenderPid)
    {
#ifdef  __LDF_FUNCTION__
        if ( VOS_FALSE == pMsg->aucValue[0])
        {
            SHPA_Ldf_Check();
            VOS_ProtectionReboot(DSP_REPORT_ERROR, 0, 0, VOS_NULL_PTR, 0);
        }
        else
        {
            SHPA_Ldf_Hifi_Saving();
            VOS_ProtectionReboot(HIFI_REPORT_ERROR, 0, 0, VOS_NULL_PTR, 0);
        }
#endif
        return;
    }

    if ((DSP_PID_WPHY == pMsg->ulSenderPid)
        || (I0_DSP_PID_GPHY == pMsg->ulSenderPid)
        || (I1_DSP_PID_GPHY == pMsg->ulSenderPid))
    {
        OM_DspMsgProc(pMsg);

        return;
    }

    if ((I0_WUEPS_PID_MMA == pMsg->ulSenderPid)||(I1_WUEPS_PID_MMA == pMsg->ulSenderPid))
    {
        OM_MmaMsgProc(pMsg);

        return;
    }

    if (WUEPS_PID_OM_CALLBACK == pMsg->ulSenderPid)
    {
        OM_HsicInitMsgProc(pMsg);

        return;
    }

#if(FEATURE_ON == FEATURE_PTM)
    if (ACPU_PID_OM == pMsg->ulSenderPid)
    {
        OM_ErrLogMsgProc(pMsg);
        return;
    }
#endif

    if (UEPS_PID_OMRL != pMsg->ulSenderPid)
    {
        VOS_UnreserveMsg(WUEPS_PID_OM, pMsg);
        PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
             "OM_OutMsgProc: Unknown Send Pid: %d.\n", (VOS_INT32)(pMsg->ulSenderPid));
        return;
    }

    pstAppMsg = (OM_REQ_PACKET_STRU*)pMsg->aucValue;

    /*参数检测*/
    /* 高2Bit为操作modem标示 */
    if ((OM_FUNCID_PART_NUM >= (pstAppMsg->ucModuleId&OM_FUNCID_VALUE_BITS))
        || (OM_FUNCID_MAX_NUM < (pstAppMsg->ucModuleId&OM_FUNCID_VALUE_BITS)))
    {
        VOS_UnreserveMsg(WUEPS_PID_OM, pMsg);
        PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_WARNING,
        "OM_CallBackMsgProc:ucModuleId is overflow :", (VOS_INT32)pstAppMsg->ucModuleId);
        return;
    }

    pOmFuncProc = g_astOmFuncIdProcTbl[pstAppMsg->ucModuleId - 1].pOmFuncProc;
    pOmFuncProc(pstAppMsg, OM_SendData);

    VOS_UnreserveMsg(WUEPS_PID_OM, pMsg);
    return;
}

VOS_UINT32 OMCallBackFidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ulRslt;

    /* Add for L Only version */
    if (BOARD_TYPE_LTE_ONLY == BSP_OM_GetBoardType())
    {
        return 0;
    }
    /* Add for L Only version */

    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
        {
            ulRslt = VOS_RegisterPIDInfo(WUEPS_PID_OM_CALLBACK,
                        VOS_NULL_PTR,
                        (Msg_Fun_Type)OM_CallBackMsgProc);

            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            if(VOS_OK != VOS_RegisterMsgTaskPrio(WUEPS_FID_OM_CALLBACK, VOS_PRIORITY_NULL))
            {
                PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                    "OMCallBackFidInit:VOS_RegisterMsgTaskPrio failure !\n");

                return VOS_ERR;
            }

#if (VOS_WIN32 == VOS_OS_VER)
#ifndef __PS_WIN32_RECUR__

            /* 接收SOCKET数据的自处理任务 */
            ulRslt = VOS_RegisterSelfTaskPrio(WUEPS_FID_OM_CALLBACK,
                             (VOS_TASK_ENTRY_TYPE)Sock_ServerTask,
                             COMM_SOCK_SELFTASK_PRIO, RL_SOCK_TASK_STACK_SIZE);
            if ( VOS_NULL_BYTE == ulRslt )
            {
                return VOS_ERR;
            }

#endif
#endif
            break;
        }
        default:
            break;
    }
    return VOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
