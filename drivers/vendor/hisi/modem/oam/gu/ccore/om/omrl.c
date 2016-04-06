/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : OmRl.c
  版 本 号   : 初稿
  作    者   : 李霄 46160
  生成日期   : 2008年6月11日
  最近修改   :
  功能描述   : R接口数据转发模块
  函数列表   :
  修改历史   :
  1.日    期   : 2008年6月11日
    作    者   : 李霄 46160
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "omrl.h"
#include "Omappagent.h"
#include "OmHdlcInterface.h"
#include "omringbuffer.h"
#include "omprivate.h"
#include "NVIM_Interface.h"
#include "FileSysInterface.h"
#include "pslogadapter.h"

#if (FEATURE_ON == FEATURE_MANUFACTURE_LOG)
#include "OmLittleImageLog.h"
#endif

#include "SCMProc.h"
#include "ombufmngr.h"
#include "OmToolInterface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define    THIS_FILE_ID        PS_FILE_ID_OMRL_C

#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*控制缓冲区访问的互斥信号量*/
VOS_SEM                                 g_OmTxBuffSem;

/*保存OM通行链路的状态，标明是否可以正常通信*/
VOS_UINT32                              g_ulOMSwitchOnOff = OM_STATE_IDLE;

/* 记录CCPU接收到PC侧数据的正常信息 */
OM_CCPU_PC_UE_SUC_STRU                  g_stPcToUeSucRecord;

/* 记录CCPU接收到PC侧数据的异常信息 */
OM_CCPU_PC_UE_FAIL_STRU                 g_stPcToUeErrRecord;

/*记录往PC侧发送数据的正常信息 */
OM_UE_PC_SUC_STRU                       g_stUeToPcSucRecord;

/*记录往PC侧发送数据时发生的各类错误*/
OM_UE_PC_FAIL_STRU                      g_stUeToPcErrRecord;

/*记录开始时间*/
VOS_UINT32                              g_ulLastTick = 0;

/*记录字节长度*/
VOS_UINT32                              g_ulLastByteCnt = 0;

/*指向OM分包头内容的指针*/
OM_APP_MSG_SEG_EX_STRU                 *g_pstSegMsgEx = VOS_NULL_PTR;

/* OM缓冲区控制块 */
OM_BUF_CTRL_STRU                        g_stOmBufCtrl;
extern OM_VCOM_DEBUG_INFO               g_stVComDebugInfo;

#if (FEATURE_ON == FEATURE_CBT_LOG)
extern OM_VCOM_CBT_LOG_DEBUG_INFO       g_stVComCBTDebugInfo;
#endif

extern OM_NV_OPERATE_RECORD_STRU        g_astRecordAcpuToCcpu[SAVE_MAX_SEND_INFO];
#ifdef OAM_UT_DEBUG
VOS_UINT32                              g_ulOmCcpuDbgFlag = VOS_NULL_DWORD;
#else
VOS_UINT32                              g_ulOmCcpuDbgFlag = VOS_FALSE;
#endif

VOS_VOID SCM_LogToFile(FILE *fp);
VOS_VOID SOCP_LogToFile(FILE *fp);

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 OMRL_CcpuReportEvent(VOS_VOID)
{
    PS_OM_EVENT_IND_STRU    *pstEventInd;
    VOS_UINT32               ulEventLenth;
    VOS_UINT32               ulTemp = 0x5a5a5a5a;
    VOS_UINT32               ulIndex;
    VOS_UINT32               ulSendLen;
    OM_SEND_DEBUG_INFO_STRU  astCcpuDebuginfoTab2[3];
    VOS_UINT32               ulAddr;
    VOS_UINT16               usDebugEventID;

    /* 存放C核调测信息的 大小和地址,分包上报消息 */
    OM_SEND_DEBUG_INFO_STRU  astCcpuDebuginfoTab1[] = {
        {sizeof(g_stPcToUeSucRecord),                     (VOS_UINT8*)&g_stPcToUeSucRecord},
        {sizeof(g_stPcToUeErrRecord),                     (VOS_UINT8*)&g_stPcToUeErrRecord},
        {sizeof(g_stUeToPcSucRecord),                     (VOS_UINT8*)&g_stUeToPcSucRecord},
        {sizeof(g_stUeToPcErrRecord),                     (VOS_UINT8*)&g_stUeToPcErrRecord},
        {sizeof(g_ulOMSwitchOnOff),                       (VOS_UINT8*)&g_ulOMSwitchOnOff  },
    };

    astCcpuDebuginfoTab2[0].pucAddr = (VOS_UINT8*)SCM_GetDebugLogInfo();
    astCcpuDebuginfoTab2[0].ulLenth = SCM_GetDebugLogInfoLen();

    astCcpuDebuginfoTab2[1].pucAddr = (VOS_UINT8*)SOCP_GetDebugLogInfo(OM_GET_SOCP_STATUS);
    astCcpuDebuginfoTab2[1].ulLenth = SOCP_GetDebugLogInfoLen(OM_GET_SOCP_STATUS);

    astCcpuDebuginfoTab2[2].pucAddr = (VOS_UINT8*)SOCP_GetDebugLogInfo(OM_GET_SOCP_DEBUG_INFO);
    astCcpuDebuginfoTab2[2].ulLenth = SOCP_GetDebugLogInfoLen(OM_GET_SOCP_DEBUG_INFO);

     /* ---------------第一包数据 astAcpuDbuginfoTab1[]  begin----------- */
    ulEventLenth = (sizeof(PS_OM_EVENT_IND_STRU)-(4 * sizeof(VOS_UINT8)));
    for (ulIndex=0; ulIndex<(sizeof(astCcpuDebuginfoTab1)/sizeof(astCcpuDebuginfoTab1[0])); ulIndex ++)
    {
        ulEventLenth += astCcpuDebuginfoTab1[ulIndex].ulLenth;
    }

    /* 加上分隔符号长度 */
    ulEventLenth += (ulIndex - 1) * sizeof(VOS_UINT32);

    pstEventInd = (PS_OM_EVENT_IND_STRU *)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, ulEventLenth);
    if(VOS_NULL_PTR == pstEventInd)
    {
        LogPrint("OMRL_CcpuReportEvent: mem malloc Fail!\r\n");

        return VOS_ERR;
    }

    pstEventInd->ulLength   = ulEventLenth;
    pstEventInd->usEventId  = OAM_DEGUG_EVENT_ID1;
    pstEventInd->ulModuleId = WUEPS_PID_OM;

    /*lint -e419 -e416 */
    /* pc侧到ue侧时正常接收数据信息 */
    VOS_MemCpy((VOS_UINT8 *)pstEventInd->aucData,
                  (VOS_UINT8*)&g_stPcToUeSucRecord,
                  sizeof(g_stPcToUeSucRecord));
    /*lint +e419 +e416 */

    ulEventLenth = 0;
    for (ulIndex=1;ulIndex<(sizeof(astCcpuDebuginfoTab1)/sizeof(astCcpuDebuginfoTab1[0])); ulIndex ++)
    {
        /*lint -e419 -e416 */
        /* 加上分隔符号 */
        ulEventLenth += astCcpuDebuginfoTab1[ulIndex-1].ulLenth;
        VOS_MemCpy((VOS_UINT8 *)pstEventInd->aucData + ulEventLenth,
                   (VOS_UINT8 *)&ulTemp, sizeof(VOS_UINT32));

        ulEventLenth += sizeof(VOS_UINT32);
        VOS_MemCpy((VOS_UINT8 *)pstEventInd->aucData + ulEventLenth,
            astCcpuDebuginfoTab1[ulIndex].pucAddr, astCcpuDebuginfoTab1[ulIndex].ulLenth);
         /*lint +e419 +e416 */
    }

    if (VOS_OK != OM_Event(pstEventInd))
    {
        PS_LOG(WUEPS_PID_OM, VOS_NULL, PS_PRINT_WARNING, "OMRL_CcpuReportEvent: send event fail.");
    }
    VOS_MemFree(WUEPS_PID_OM, pstEventInd);
    /* ---------------第一包数据 astAcpuDbuginfoTab1[]  end---------- */

    /* ---------------astAcpuDbuginfoTab2[] 每项数据较大拆包发送，每包最大发送2k----------- */
    /* Event Id */
    usDebugEventID  = OAM_DEGUG_EVENT_ID2;

    /* 一次性申请 2k 空间，最后释放 */
    pstEventInd = (PS_OM_EVENT_IND_STRU *)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT,
                                          OM_SEND_DEBUG_INFO_MAX_LEN + sizeof(PS_OM_EVENT_IND_STRU)-(4 * sizeof(VOS_UINT8)));
    if (VOS_NULL_PTR == pstEventInd)
    {
        LogPrint("OMRL_AcpuReportEvent:Alloc Memory Failed!\n");
        return VOS_ERR;
    }

    /* 数据拆分发送 */
    for(ulIndex =0; ulIndex<(sizeof(astCcpuDebuginfoTab2)/sizeof(astCcpuDebuginfoTab2[0])); ulIndex++)
    {
        /* 分配内容空间 最大为 2k*/
        ulSendLen = 0;

        while(ulSendLen < astCcpuDebuginfoTab2[ulIndex].ulLenth)
        {
            if((astCcpuDebuginfoTab2[ulIndex].ulLenth - ulSendLen) > OM_SEND_DEBUG_INFO_MAX_LEN)
            {
                ulEventLenth = OM_SEND_DEBUG_INFO_MAX_LEN;
            }
            else
            {
                ulEventLenth = astCcpuDebuginfoTab2[ulIndex].ulLenth - ulSendLen;
            }

            pstEventInd->ulLength   = ulEventLenth;
            pstEventInd->usEventId  = usDebugEventID++;
            pstEventInd->ulModuleId = WUEPS_PID_OM;

            /* make pclint happy  begin */
            ulAddr = (VOS_UINT32)pstEventInd->aucData;
            VOS_MemCpy((VOS_UINT8 *)ulAddr,
                       astCcpuDebuginfoTab2[ulIndex].pucAddr + ulSendLen, ulEventLenth);
            /* make pclint happy  end */

            ulSendLen += ulEventLenth;
            if (VOS_OK != OM_Event(pstEventInd))
            {
                PS_LOG(WUEPS_PID_OM, VOS_NULL, PS_PRINT_WARNING, "OMRL_AcpuReportEvent: send event fail.");
            }
        }
    }

    /* 释放消息内容空间 */
    VOS_MemFree(WUEPS_PID_OM, pstEventInd);

    return VOS_OK;
}


VOS_UINT32 OMRL_ReleaseSocpData(VOS_VOID)
{
    VOS_UINT_PTR                        DataAddr;
    VOS_UINT32                          ulDataLen;
    VOS_UINT8                           *pucRealAddr;

#if 0
    VOS_BOOL                            bLoopFlag = VOS_TRUE;

    /* 为了测试充分，这里先按一个一个RD释放 */
    do
    {
        /* 从SCM获得已发送消息总长度 */
        if (VOS_OK != SCM_RlsSrcRDFirst(SOCP_CODER_SRC_GU_OM2, &ulDataAddr, &ulDataLen))
        {
            g_stUeToPcErrRecord.usRlsRdErr++;

            return VOS_ERR;
        }

        /* 已经没有RD */
        if ((0 == ulDataAddr) || (0 == ulDataLen))
        {
            break;
        }

        g_stUeToPcSucRecord.stRealRlsData.ulDataLen += ulDataLen;
        g_stUeToPcSucRecord.stRealRlsData.ulNum++;

        pucRealAddr = (VOS_UINT8*)OM_AddrRealToVirt(&g_stOmBufCtrl, (VOS_UINT8*)ulDataAddr);

        if (VOS_NULL_PTR == pucRealAddr)
        {
            g_stUeToPcErrRecord.usRlsAddrErr++;

            return VOS_ERR;
        }

        /* 释放OM缓冲区内存 */
        if (VOS_OK != OM_ReleaseTraceMem(&g_stOmBufCtrl, pucRealAddr, (VOS_INT32)ulDataLen))
        {
            g_stUeToPcErrRecord.usRlsMemErr++;

            return VOS_ERR;
        }

    }while(bLoopFlag);

#else
    /* 清空所有已发送RD */
    if (VOS_OK != SCM_RlsSrcRDAll(SOCP_CODER_SRC_GU_OM2, &DataAddr, &ulDataLen))
    {
        g_stUeToPcErrRecord.usRlsRdErr++;

        return VOS_ERR;
    }

    /* 已经没有RD */
    if ((0 == DataAddr) || (0 == ulDataLen))
    {
        return VOS_OK;
    }

    g_stUeToPcSucRecord.stRealRlsData.ulDataLen += ulDataLen;
    g_stUeToPcSucRecord.stRealRlsData.ulNum++;

    pucRealAddr = (VOS_UINT8*)OM_AddrRealToVirt(&g_stOmBufCtrl, (VOS_UINT8*)DataAddr);

    if (VOS_NULL_PTR == pucRealAddr)
    {
        g_stUeToPcErrRecord.usRlsAddrErr++;

        return VOS_ERR;
    }

    /* 根据已发送RD总长度释放内存 */
    if (VOS_OK != OM_ReleaseTraceMem(&g_stOmBufCtrl, pucRealAddr, (VOS_INT32)ulDataLen))
    {
        g_stUeToPcErrRecord.usRlsMemErr++;

        g_ulOMSwitchOnOff = OM_STATE_IDLE;

        return VOS_ERR;

    }
#endif

    return VOS_OK;
}
VOS_UINT32 OMRL_SendSocpData(VOS_UINT8 *pucHead, VOS_UINT32 ulHeadLen,
                             VOS_UINT8 *pucData, VOS_UINT32 ulDataLen)
{
    VOS_UINT8                          *pucMemAddr;
    VOS_INT32                           lUsedBytes;
    VOS_UINT8                          *pucRealMemAddr;

    /*lint -e40*/
    OM_CCPU_DEBUG_TRACE(pucHead, ulHeadLen, OM_CCPU_PUT_OM_BUFF);
    OM_CCPU_DEBUG_TRACE(pucData, ulDataLen, OM_CCPU_PUT_OM_BUFF);
    /*lint +e40*/

    /* sizeof(VOS_UINT32)代表桢尾的数据长度 */
    pucMemAddr = (VOS_UINT8*)OM_AllocTraceMem(&g_stOmBufCtrl, (VOS_INT32)(ulHeadLen + ulDataLen + sizeof(VOS_UINT32)));

    if (VOS_NULL_PTR == pucMemAddr)
    {
        g_stUeToPcErrRecord.usNoMemErr++;

        return VOS_ERR;
    }

    /* 拷贝数据头 */
    VOS_MemCpy(pucMemAddr, pucHead, ulHeadLen);

    /* 拷贝数据内容 */
    VOS_MemCpy(pucMemAddr + ulHeadLen, pucData, ulDataLen);

    /* 填写桢尾 */
    *(VOS_UINT32*)(pucMemAddr + ulHeadLen + ulDataLen) = OM_USB_FRAME_TAIL;

    VOS_FlushCpuWriteBuf();

    pucRealMemAddr = (VOS_UINT8*)OM_AddrVirtToReal(&g_stOmBufCtrl, pucMemAddr);

    if (VOS_NULL_PTR == pucRealMemAddr)
    {
        g_stUeToPcErrRecord.usSendAddrErr++;

        return VOS_ERR;
    }

    /* 调用SCM提供的接口将数据发送给SOCP */
    if(VOS_OK != SCM_SendCoderSrc(SOCP_CODER_SRC_GU_OM2, pucRealMemAddr, ulHeadLen + ulDataLen + sizeof(VOS_UINT32)))
    {
        g_stUeToPcErrRecord.usSendSocpDataErr++;

        return VOS_ERR;
    }

    g_stUeToPcSucRecord.stRealSendData.ulDataLen += (ulHeadLen + ulDataLen + sizeof(VOS_UINT32));
    g_stUeToPcSucRecord.stRealSendData.ulNum++;

    lUsedBytes = OM_TraceMemNBytes(&g_stOmBufCtrl);

    if (g_stUeToPcSucRecord.ulMaxUsedBufSize < (VOS_UINT32)lUsedBytes)
    {
        g_stUeToPcSucRecord.ulMaxUsedBufSize = (VOS_UINT32)lUsedBytes;
    }

    return VOS_OK;
}
VOS_UINT32 OMRL_IsBDEnough(VOS_UINT8 ucMsgCnt)
{
    VOS_UINT32                          ulFreeBdNum = 0;

    /* 获取OM源通道剩余BD个数 */
    if (VOS_OK != SCM_GetBDFreeNum(SOCP_CODER_SRC_GU_OM2, &ulFreeBdNum))
    {
        g_stUeToPcErrRecord.usGetBDErr++;

        return VOS_ERR;
    }

    /* 判断剩余BD是否满足分包个数 */
    if (ucMsgCnt > ulFreeBdNum)
    {
        g_stUeToPcErrRecord.usBDLackErr++;

        return VOS_ERR;
    }

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_MANUFACTURE_LOG)

/*****************************************************************************
 函 数 名  : OMRL_WriteLittleImg
 功能描述  : 生产小镜像时保存信息到flash
 输入参数  : pucMsg    -- 指向发送数据的指针
             usMsgLen  -- 发送数据的长度
 输出参数  : 无
 返 回 值  : ture or false

 修改历史      :
  1.日    期   : 2009年3月28日
    作    者   : 王笑非
    修改内容   : 新生成函数

*****************************************************************************/
VOS_BOOL OMRL_WriteLittleImg(VOS_UINT8 *pucMsg, VOS_UINT16 usMsgLen)
{
    OM_APP_TRACE_STRU                  *pstAppTrace;
    VOS_UINT32                          ulTempAddress;
    VOS_UINT16                          usLogSaveLen;
    VOS_UINT32                         *pucLogSaveMsg;

    pstAppTrace = (OM_APP_TRACE_STRU*)pucMsg;

    if ((OM_TRACE_FUNC == pstAppTrace->stAppHeader.ucFuncType) && (OM_APP_TRACE_IND == pstAppTrace->usPrimId))
    {

        /* 写入Trace头 */
        OM_LittleImgWriteLogFile((VOS_CHAR*)pucMsg, OM_APP_TRACE_LEN);

        /* 获取消息内容指针 */
        ulTempAddress = *(VOS_UINT32*)(pstAppTrace->aucPara);

        pucLogSaveMsg = (VOS_UINT32*)ulTempAddress;

        usLogSaveLen  = usMsgLen - OM_APP_TRACE_LEN;

        OM_LittleImgWriteLogFile((VOS_CHAR*)pucLogSaveMsg, usLogSaveLen);

        return VOS_TRUE;
    }

    /* 将LOG信息保存到文件系统 */
    if (VOS_OK != OM_LittleImgWriteLogFile((VOS_CHAR*)pucMsg, usMsgLen))
    {
        PS_LOG(WUEPS_PID_OM, 0, LOG_LEVEL_INFO, "OMRL_MsgSplit: No need to send msg to PC.\n");

        return VOS_TRUE;
    }

    return VOS_FALSE;
}

#endif

/*****************************************************************************
 函 数 名  : OMRL_ProcessSendData
 功能描述  : 将可维可测数据发送的SOCP模块
 输入参数  : pucMsg    -- 指向发送数据的指针
             usMsgLen  -- 发送数据的长度
             ucMsgCnt  -- 待发送数据包个数
 输出参数  : 无
 返 回 值  : VOS_UINT32

 修改历史      :
  1.日    期   : 2009年3月28日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OMRL_ProcessSendData(VOS_UINT8 *pucMsg, VOS_UINT8 ucMsgCnt, VOS_UINT16 usMsgLen)
{
    VOS_UINT8                           ucCurSegNum = 1; /*当前段序号*/
    VOS_UINT32                          ulTempAddress;
    VOS_BOOL                            bIsHead     = VOS_FALSE;
    OM_APP_TRACE_STRU                  *pstAppTrace;

    pstAppTrace = (OM_APP_TRACE_STRU*)pucMsg;

    ulTempAddress = (VOS_UINT32)pucMsg;

    if ((OM_TRACE_FUNC == pstAppTrace->stAppHeader.ucFuncType)
        &&(OM_APP_TRACE_IND == pstAppTrace->usPrimId))
    {
        ulTempAddress = *(VOS_UINT32*)(pstAppTrace->aucPara);
        DRV_RT_MEMCPY(g_pstSegMsgEx + 1, pucMsg, OM_APP_TRACE_LEN);
        bIsHead = VOS_TRUE;
    }

    /* 填充SOCP头中的RTC值 */
    g_pstSegMsgEx->stSocpHdr.ulRTC = (VOS_UINT32)DRV_GET_RTC_VALUE();

    g_pstSegMsgEx->stMsgSeg.ucSegNum = ucMsgCnt;
    g_pstSegMsgEx->stMsgSeg.usSegLen = OM_APP_SPLIT_MSG_LEN;


    /* 大于最大分包大小的数据，按照最大分包大小进行数据发送的处理 */
    for (ucCurSegNum = 1; ucCurSegNum < ucMsgCnt; ucCurSegNum++)
    {
        g_pstSegMsgEx->stMsgSeg.ucSegSn = ucCurSegNum;

        if (VOS_TRUE == bIsHead)
        {
            /* 消息头前加上长度信并放入环形缓冲区*/
            if (VOS_OK != OMRL_SendSocpData((VOS_UINT8 *)g_pstSegMsgEx,
                (sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_APP_TRACE_LEN),
                (VOS_UINT8*)ulTempAddress, OM_APP_MSG_SEGMENT_LEN - OM_APP_TRACE_LEN))
            {
                return VOS_ERR;
            }

            bIsHead       =  VOS_FALSE;
            ulTempAddress += (OM_APP_MSG_SEGMENT_LEN - OM_APP_TRACE_LEN);
        }
        else
        {
            /* 消息头前加上长度信并放入环形缓冲区*/
            if (VOS_OK != OMRL_SendSocpData((VOS_UINT8 *)g_pstSegMsgEx,
                 sizeof(OM_APP_MSG_SEG_EX_STRU),
                (VOS_UINT8*)ulTempAddress, OM_APP_MSG_SEGMENT_LEN))
            {
                return VOS_ERR;
            }

            ulTempAddress += OM_APP_MSG_SEGMENT_LEN;
        }

        /* 计算剩余数据包大小 */
        usMsgLen -= OM_APP_MSG_SEGMENT_LEN;
    }

    /* 设置消息头参数 */
    g_pstSegMsgEx->stMsgSeg.usSegLen = (VOS_UINT16)(usMsgLen + OM_APP_MSG_SEGMENT_HEADER_LENGTH);
    g_pstSegMsgEx->stMsgSeg.ucSegSn  = ucCurSegNum;

    /*为了保持Ringbuffer中存放的数据包4字节对齐*/
    usMsgLen = (usMsgLen + 3) & ~0x03;

    if (VOS_TRUE == bIsHead)
    {
        /* 消息头前加上长度并放入环形缓冲区 */
        if ( VOS_OK != OMRL_SendSocpData((VOS_UINT8 *)g_pstSegMsgEx,
                            (sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_APP_TRACE_LEN),
                            (VOS_UINT8*)ulTempAddress,
                            usMsgLen - OM_APP_TRACE_LEN))
        {
            return VOS_ERR;
        }
    }
    else
    {
        /* 消息头前加上长度并放入环形缓冲区 */
        if ( VOS_OK != OMRL_SendSocpData((VOS_UINT8 *)g_pstSegMsgEx,
                                        sizeof(OM_APP_MSG_SEG_EX_STRU),
                                        (VOS_UINT8*)ulTempAddress,
                                        usMsgLen))
        {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OMRL_MsgSplit
 功能描述  : 当在USB上进行通信时，使用的扩展分包函数
 输入参数  : pucMsg    -- 指向发送数据的指针
             usMsgLen  -- 发送数据的长度
 输出参数  : 无
 返 回 值  : VOS_UINT32

 修改历史      :
  1.日    期   : 2009年3月28日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OMRL_MsgSplit(VOS_UINT8 *pucMsg, VOS_UINT16 usMsgLen)
{
    VOS_UINT8                           ucMsgCnt    = 1; /*分段的数量*/
    VOS_UINT32                          ulRet;

    /*进行互斥操作*/
    if ( VOS_OK != VOS_SmP(g_OmTxBuffSem, OM_PV_TIMEOUT) )
    {
        LogPrint("OMRL_MsgSplit, Error, g_OmTxBuffSem VOS_SmP Failed.\n");

        g_stUeToPcErrRecord.usSemErr++;
        return VOS_ERR;
    }

#if (FEATURE_ON == FEATURE_MANUFACTURE_LOG)

    if ( VOS_TRUE == OMRL_WriteLittleImg(pucMsg, usMsgLen) )
    {
        VOS_SmV(g_OmTxBuffSem);

        return VOS_OK;
    }

#endif
    /* 释放已发送数据内存 */
    if (VOS_OK != OMRL_ReleaseSocpData())
    {
        VOS_SmV(g_OmTxBuffSem);

        return VOS_ERR;
    }

    /* 计算分包个数 */
    ucMsgCnt = (VOS_UINT8)(((usMsgLen + OM_APP_MSG_SEGMENT_LEN) - 1)/OM_APP_MSG_SEGMENT_LEN);

    /* 判断剩余BD是否满足分包个数 */
    if (VOS_OK != OMRL_IsBDEnough(ucMsgCnt))
    {
        VOS_SmV(g_OmTxBuffSem);

        return VOS_ERR;
    }

    ulRet = OMRL_ProcessSendData(pucMsg, ucMsgCnt, usMsgLen);

    VOS_SmV(g_OmTxBuffSem);

    return ulRet;
}
VOS_UINT32 OMRL_UsbFrameInit(VOS_VOID)
{
    /*分配分包结构的内存空间*/
    g_pstSegMsgEx = (OM_APP_MSG_SEG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM,
                STATIC_MEM_PT, sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_APP_TRACE_LEN);

    if (VOS_NULL_PTR == g_pstSegMsgEx)
    {
        LogPrint("OM_UsbFrameInit:VOS_MemAlloc g_pstSegMsgEx Failed!\n");
        return VOS_ERR;
    }

    g_pstSegMsgEx->stSocpHdr.ucCPUID   = OM_TOOL_CCPU_ID;
    g_pstSegMsgEx->ulFrameHead         = OM_USB_FRAME_HEAD;

    /* 创建OM发送缓冲区 */
    if (VOS_OK != OM_CreateTraceBuffer(&g_stOmBufCtrl, OM_UPLINK_BUFFER_SIZE))
    {
        VOS_MemFree(ACPU_PID_OM, g_pstSegMsgEx);

        LogPrint("OM_UsbFrameInit:OM_CreateTraceBuffer Failed!\n");

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_INT OM_SaveTraceBuffer(VOS_INT lFuncType, VOS_UINT8* pucData, VOS_UINT32* pulLen)
{
    /*参数检测*/
    if ((OM_TRACE_FUNC != lFuncType) || (VOS_NULL_PTR == pucData)
        || (VOS_NULL_PTR == pulLen) || (0 == *pulLen))
    {
        LogPrint("OM_SaveBuffer:Input para. is not correct.\n");

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_INT OM_SaveLogBuffer(VOS_INT lFuncType, VOS_UINT8* pucData, VOS_UINT32* pulLen)
{
    OM_RING_ID      ringId;
    VOS_INT         lPacketLen = 0;
    VOS_INT         lBufferLen;
    VOS_UINT8       *pucTemp;

    if ((OM_LOG_FUNC != lFuncType) || (VOS_NULL_PTR == pucData)
        || (VOS_NULL_PTR == pulLen) || (0 == *pulLen))
    {
        LogPrint("OM_SaveBuffer:Input para. is not correct.\n");

        return VOS_ERR;
    }

    /*得到LOG数据缓冲区*/
    ringId = g_stLogEnt.rngOmRbufId;

    lBufferLen = (VOS_INT)(*pulLen);

    pucTemp = pucData;

    VOS_TaskLock();

    while (!OM_RingBufferIsEmpty(ringId))
    {
        if (sizeof(lPacketLen) != OM_RingBufferGet(ringId,
                                        (VOS_CHAR*)&lPacketLen, sizeof(lPacketLen)))
        {
            LogPrint("OM_SaveBuffer:LOG OM_RingBufferGet fail\n");

            break;
        }

        /*删除存放在LOG Buffer中的模块ID*/
        if (LOG_MODULE_ID_LEN != OM_RingBufferRemove(ringId, LOG_MODULE_ID_LEN))
        {
            LogPrint("OM_SaveBuffer:LOG OM_RingBufferRemove fail\n");
            break;
        }

        lPacketLen -= LOG_MODULE_ID_LEN;

        /*判断缓冲区是否已满*/
        if (lBufferLen < (lPacketLen + 1))
        {
            break;
        }

        /*从Ringbuffer中删除数据*/
        if (lPacketLen != OM_RingBufferGet(ringId, (VOS_CHAR*)pucTemp, lPacketLen))
        {
            LogPrint1("OM_SaveBuffer:LOG OM_RingBufferGet len : %d\n", lPacketLen);

            break;
        }

        /*为了打点工具解析，需要加\n*/
        pucTemp[lPacketLen] = '\n';

        /*加1是因为前面多加了\n操作*/
        pucTemp += (lPacketLen + 1);

        lBufferLen -= (lPacketLen + 1);
    }

    VOS_TaskUnlock();

    *pulLen = (VOS_UINT32)(pucTemp - pucData);

    return VOS_OK;
}


VOS_VOID OmInitVar(VOS_VOID)
{
    VOS_MemSet(&g_stUeToPcErrRecord, 0, sizeof(g_stUeToPcErrRecord));
    VOS_MemSet(&g_stUeToPcSucRecord, 0, sizeof(g_stUeToPcSucRecord));
    VOS_MemSet(&g_stPcToUeErrRecord, 0, sizeof(g_stPcToUeErrRecord));
    VOS_MemSet(&g_stPcToUeSucRecord, 0, sizeof(g_stPcToUeSucRecord));
}


VOS_UINT32 OMRL_SendMsg(VOS_UINT8 *pucData, VOS_UINT32 ulSize, VOS_UINT32 ulSenderPid, VOS_UINT32 ulReceiverPid)
{

    MsgBlock           *pstAppOmMsg = VOS_NULL_PTR;
    VOS_UINT32          ulRslt;

    pstAppOmMsg = VOS_AllocMsg(ulSenderPid, ulSize);
    if (VOS_NULL_PTR == pstAppOmMsg)
    {
        /* 打印错误 */
        return VOS_ERR;
    }

    pstAppOmMsg->ulReceiverPid = ulReceiverPid;

    /* copy 消息内容 */
    VOS_MemCpy((VOS_UINT8 *)pstAppOmMsg->aucValue, pucData, ulSize);

    ulRslt = VOS_SendMsg(ulSenderPid, pstAppOmMsg);
    if (VOS_OK != ulRslt )
    {
        /* 打印错误 */
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 OMRL_IccDataRecv(VOS_UINT8 *pucData, VOS_UINT16 uslen)
{

    g_stPcToUeSucRecord.stTotalData.ulDataLen += (VOS_UINT32)uslen;
    g_stPcToUeSucRecord.stTotalData.ulNum++;

    if ((VOS_NULL_PTR == pucData) || (0 == uslen) || (APP_OM_HEADER_LENGTH >= uslen))
    {
        PS_LOG(WUEPS_PID_OM, 0, LOG_LEVEL_ERROR, "OMRL_IccDataRecv: Parameter Error!");
        g_stPcToUeErrRecord.stParaErrData.ulDataLen += uslen;
        g_stPcToUeErrRecord.stParaErrData.ulNum++;

        return VOS_ERR;
    }

    /*lint -e40*/
    OM_CCPU_DEBUG_TRACE(pucData, uslen, OM_CCPU_RECV_ICC);
    /*lint +e40*/

    /* 当发生释放消息出错时停止接收数据 */
    if (0 < g_stUeToPcErrRecord.usRlsMemErr)
    {
        return VOS_OK;
    }

    if (VOS_OK != OMRL_SendMsg(pucData, (VOS_UINT32)uslen, UEPS_PID_OMRL, WUEPS_PID_OM))
    {
        g_stPcToUeErrRecord.stSendErrData.ulDataLen += uslen;
        g_stPcToUeErrRecord.stSendErrData.ulNum++;

        return VOS_ERR;
    }

    g_stPcToUeSucRecord.stRealData.ulDataLen += uslen;
    g_stPcToUeSucRecord.stRealData.ulNum++;

    return VOS_OK;
}
VOS_UINT32 OMRL_Init(VOS_VOID)
{
    OmInitVar();

    if(VOS_OK != VOS_SmMCreate("TXRB", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_OmTxBuffSem))
    {
        LogPrint("RL_Init: Error, VOS_SmMCreate Fail\n");
        return VOS_ERR;
    }

    if (VOS_OK != OMRL_UsbFrameInit())
    {
        LogPrint("RL_Init:OM_UsbFrameInit Fail.\n");
        return VOS_ERR;
    }

    if (VOS_OK != DRV_SAVE_REGISTER(OM_TRACE_FUNC, (OM_SAVE_FUNC*)OM_SaveTraceBuffer))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "RL_Init: DRV_SAVE_REGISTER OM_TRACE_FUNC Fail!");
    }

    if (VOS_OK != DRV_SAVE_REGISTER(OM_LOG_FUNC, (OM_SAVE_FUNC*)OM_SaveLogBuffer))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "RL_Init: DRV_SAVE_REGISTER OM_LOG_FUNC Fail!");
    }

    OM_ComRx_RegisterCB(OM_OM_ICC_CHANNEL, OMRL_IccDataRecv);

    return VOS_OK;
}


VOS_VOID OmSendShow(VOS_VOID)
{
    vos_printf("Total Data Send by Ps: num:%6u, len:%6u\n",
                g_stUeToPcSucRecord.stTotalData.ulNum, g_stUeToPcSucRecord.stTotalData.ulDataLen);

    vos_printf("Real Data send to Socp: num:%6u, len:%6u\n",
                g_stUeToPcSucRecord.stRealSendData.ulNum, g_stUeToPcSucRecord.stRealSendData.ulDataLen);

    vos_printf("Real Data release by Socp: num:%6u, len:%6u\n",
                g_stUeToPcSucRecord.stRealRlsData.ulNum, g_stUeToPcSucRecord.stRealRlsData.ulDataLen);

    vos_printf("Max Used Buffer Size: size:%6u\n", g_stUeToPcSucRecord.ulMaxUsedBufSize);

    return;
}


VOS_VOID OmSendErrShow(VOS_VOID)
{
    vos_printf("Total Lost Data: num:%6u, len:%6u\n",
                g_stUeToPcErrRecord.stLostData.ulNum, g_stUeToPcErrRecord.stLostData.ulDataLen);

    vos_printf("Link Err Data: num:%6u, len:%6u\n",
                g_stUeToPcErrRecord.stLinkErrData.ulNum, g_stUeToPcErrRecord.stLinkErrData.ulDataLen);

    vos_printf("Para Err Data: num:%6u, len:%6u\n",
                g_stUeToPcErrRecord.stParaErrData.ulNum, g_stUeToPcErrRecord.stParaErrData.ulDataLen);

    vos_printf("num of sem take err: num:%6u\n", g_stUeToPcErrRecord.usSemErr);

    vos_printf("num of release RD err: num:%6u\n", g_stUeToPcErrRecord.usRlsRdErr);

    vos_printf("num of release Mem err: num:%6u\n", g_stUeToPcErrRecord.usRlsMemErr);

    vos_printf("num of BD Lack err: num:%6u\n", g_stUeToPcErrRecord.usBDLackErr);

    vos_printf("num of get BD err: num:%6u\n", g_stUeToPcErrRecord.usGetBDErr);

    vos_printf("num of No Mem err: num:%6u\n", g_stUeToPcErrRecord.usNoMemErr);

    vos_printf("num of Send Socp Data err: num:%6u\n", g_stUeToPcErrRecord.usSendSocpDataErr);

    vos_printf("num of transfer sending addr err: num:%6u\n", g_stUeToPcErrRecord.usSendAddrErr);

    vos_printf("num of transfer Release addr err: num:%6u\n", g_stUeToPcErrRecord.usRlsAddrErr);

    return;
}



VOS_VOID OmRcvShow(VOS_VOID)
{
    vos_printf("Total Data Recv From ACPU: num:%6u, len:%6u\n",
                g_stPcToUeSucRecord.stTotalData.ulNum, g_stPcToUeSucRecord.stTotalData.ulDataLen);

    vos_printf("Data send OM:   num:%6u, len:%6u\n",
                g_stPcToUeSucRecord.stRealData.ulNum, g_stPcToUeSucRecord.stRealData.ulDataLen);

    vos_printf("Data Recv From ICC: num:%6u, len:%6u, slice:%6u\n",
                g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecMsgNum,
                g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecLen,
                g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecSlice);

    return;
}


VOS_VOID OmRcvErrShow(VOS_VOID)
{
    vos_printf("Lost Data because para Err: num:%6u, len:%6u\n",
            g_stPcToUeErrRecord.stParaErrData.ulNum, g_stPcToUeErrRecord.stParaErrData.ulDataLen);

    vos_printf("Err data when sending msg to om: num:%6u, len:%6u\n",
            g_stPcToUeErrRecord.stSendErrData.ulNum, g_stPcToUeErrRecord.stSendErrData.ulDataLen);

    vos_printf("Data rcv err from ICC: ParaErrNum:%6u, len:%6u, MsgNum:%6u, slice:%6u\n",
            g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmParaErrNum,
            g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmRecErrLen,
            g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmRecErrMsgNum,
            g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmRecErrSlice);

    return;
}


VOS_VOID OmStatus(VOS_UINT32 ulFlag)
{
    return;
}


VOS_VOID OmRate(VOS_VOID)
{
#if 0
    VOS_UINT32            ulCurrTick;                    /*记录现在统计时间*/
    VOS_UINT32            ulDeltaTick;                   /*记录统计时间*/
    VOS_UINT32            ulDelData;                     /*记录统计数据大小*/
    VOS_UINT32            ulBandWidth;                   /*记录统计数据大小*/

    vos_printf("OM Rate Show (CCPU To SOCP):\n");

    ulCurrTick = VOS_GetTick();

    if (ulCurrTick < g_ulLastTick)
    {
        ulDeltaTick = (0xFFFFFFFF - g_ulLastTick) + ulCurrTick;
    }
    else
    {
        ulDeltaTick = ulCurrTick - g_ulLastTick;
    }


    ulBandWidth    = (ulDelData/ulDeltaTick)/10;

    /*统计OM在ulDeltaTick时间(单位10ms)内发送了ulTotalLen字节数据*/
    vos_printf("OmRate,Tick <%u> Byte <%u>, BandWidth <%u>.\n",
                 ulDeltaTick, ulDelData, ulBandWidth);

    g_ulLastTick = ulCurrTick;

    vos_printf("OM Rate Show End\n");
#endif
}
VOS_VOID OmOpenCCPULog(VOS_UINT32 ulFlag)
{
    g_ulOmCcpuDbgFlag = ulFlag;

    return;
}

/*****************************************************************************
 Prototype       : GU_OmCcpuSwitchOnOff
 Description     : ON or Off Ccpu Switch
 Input           : VOS_UINT32
 Output          : None
 Return Value    : None

 History         : ---
    Date         :
    Author       :
    Modification : Created function
 *****************************************************************************/
VOS_VOID GU_OmCcpuSwitchOnOff(VOS_UINT32 ulFlag)
{
    g_ulOMSwitchOnOff = ulFlag;
}


VOS_VOID OM_CpuReadNVLog(VOS_VOID)
{
    FILE                               *fp;
    VOS_UINT32                          ulTemp = 0x5a5a5a5a;

#if(VOS_WIN32 == VOS_OS_VER)
    VOS_CHAR                  acLogPath[] = ".\\yaffs0\\C_NV_OmLog.bin";

#else
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
    VOS_CHAR                  acLogPath[] = "/modem_log/C_NV_OmLog.bin";

#else
    VOS_CHAR                  acLogPath[] = "/yaffs0/C_NV_OmLog.bin";
#endif
#endif

    fp = DRV_FILE_OPEN(acLogPath, "w+");

    if (VOS_NULL_PTR == fp)
    {
        return;
    }

    DRV_FILE_WRITE((VOS_VOID*)g_astRecordAcpuToCcpu, sizeof(VOS_CHAR), sizeof(g_astRecordAcpuToCcpu), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_CLOSE(fp);

}


VOS_VOID OM_LogShowToFile(VOS_BOOL bIsSendMsg)
{
    FILE                               *fp;
    OM_AUTOCONFIG_CNF_STRU             *pstSendCnf;
    VOS_UINT32                          ulTemp = 0x5a5a5a5a;

#if(VOS_WIN32 == VOS_OS_VER)
    VOS_CHAR                  g_acLogPath[] = ".\\yaffs0\\C_OmLog.bin";

#else
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
    VOS_CHAR                  g_acLogPath[] = "/modem_log/C_OmLog.bin";

#else
    VOS_CHAR                  g_acLogPath[] = "/yaffs0/C_OmLog.bin";
#endif
#endif

    fp = DRV_FILE_OPEN(g_acLogPath, "w+");

    if (VOS_NULL_PTR == fp)
    {
        return;
    }

    DRV_FILE_WRITE((VOS_VOID*)&g_stPcToUeSucRecord, sizeof(VOS_CHAR), sizeof(g_stPcToUeSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)&g_stPcToUeErrRecord, sizeof(VOS_CHAR), sizeof(g_stPcToUeErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_WRITE((VOS_VOID*)&g_stUeToPcSucRecord, sizeof(VOS_CHAR), sizeof(g_stUeToPcSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)&g_stUeToPcErrRecord, sizeof(VOS_CHAR), sizeof(g_stUeToPcErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_WRITE((VOS_VOID*)&g_ulOMSwitchOnOff, sizeof(VOS_CHAR), sizeof(g_ulOMSwitchOnOff), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    /* SCM 相关*/
    SCM_LogToFile(fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    /* SOCP相关 */
    SOCP_LogToFile(fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_CLOSE(fp);

    OM_CpuReadNVLog();

    /* 发送消息給 Acpu */
    if(VOS_FALSE == bIsSendMsg)
    {
        return;
    }

    pstSendCnf = (OM_AUTOCONFIG_CNF_STRU*)VOS_AllocMsg(CCPU_PID_OMAGENT,
                            sizeof(OM_AUTOCONFIG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 分配消息失败 */
    if (VOS_NULL_PTR == pstSendCnf)
    {
        return;
    }

    pstSendCnf->ulReceiverPid  = ACPU_PID_OMAGENT;
    pstSendCnf->usPrimId       = OM_RECORD_DBU_INFO_REQ;

    (VOS_VOID)VOS_SendMsg(CCPU_PID_OMAGENT, pstSendCnf);

    return;
}
#else
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*保存OM通行链路的状态，标明是否可以正常通信*/
VOS_UINT32                              g_ulOMSwitchOnOff = OM_STATE_IDLE;

/*保存校准通道链路的状态，标明是否可以正常通信*/
VOS_UINT32                              g_ulCbtSwitchOnOff = OM_STATE_IDLE;

/* 记录CCPU接收到PC侧数据的正常信息,目前基本都是CNF通道的数据 */
OM_CCPU_PC_UE_SUC_STRU                  g_stPcToUeSucRecord;

/* 记录CCPU接收到PC侧数据的异常信息 */
OM_CCPU_PC_UE_FAIL_STRU                 g_stPcToUeErrRecord;

/*记录开始时间*/
VOS_UINT32                              g_ulLastTick = 0;

/*记录字节长度*/
VOS_UINT32                              g_ulLastByteCnt = 0;

OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU        g_stTxCnfCtrlInfo;

OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU        g_stTxIndCtrlInfo;

extern OM_VCOM_DEBUG_INFO               g_stVComDebugInfo;

#ifdef OAM_UT_DEBUG
VOS_UINT32                              g_ulOmCcpuDbgFlag = VOS_NULL_DWORD;
#else
VOS_UINT32                              g_ulOmCcpuDbgFlag = VOS_FALSE;
#endif

VOS_VOID SCM_LogToFile(FILE *fp);
VOS_VOID SOCP_LogToFile(FILE *fp);

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 OMRL_CcpuReportEvent(VOS_VOID)
{
    PS_OM_EVENT_IND_STRU    *pstEventInd;
    VOS_UINT32               ulEventLenth;
    VOS_UINT32               ulTemp = 0x5a5a5a5a;
    VOS_UINT32               ulIndex;
    VOS_UINT32               ulSendLen;
    OM_SEND_DEBUG_INFO_STRU  astCcpuDebuginfoTab2[3];
    VOS_UINT32               ulAddr;
    VOS_UINT16               usDebugEventID;

    /* 存放C核调测信息的 大小和地址,分包上报消息 */
    OM_SEND_DEBUG_INFO_STRU  astCcpuDebuginfoTab1[] = {
        {sizeof(g_stPcToUeSucRecord),                     (VOS_UINT8*)&g_stPcToUeSucRecord},
        {sizeof(g_stPcToUeErrRecord),                     (VOS_UINT8*)&g_stPcToUeErrRecord},
        {sizeof(g_stTxIndCtrlInfo.stUeToPcSucRecord),    (VOS_UINT8*)&g_stTxIndCtrlInfo.stUeToPcSucRecord},
        {sizeof(g_stTxCnfCtrlInfo.stUeToPcSucRecord),    (VOS_UINT8*)&g_stTxCnfCtrlInfo.stUeToPcSucRecord},
        {sizeof(g_stTxIndCtrlInfo.stUeToPcErrRecord),    (VOS_UINT8*)&g_stTxIndCtrlInfo.stUeToPcErrRecord},
        {sizeof(g_stTxCnfCtrlInfo.stUeToPcErrRecord),    (VOS_UINT8*)&g_stTxCnfCtrlInfo.stUeToPcErrRecord},
        {sizeof(g_ulOMSwitchOnOff),                       (VOS_UINT8*)&g_ulOMSwitchOnOff  },
    };

    astCcpuDebuginfoTab2[0].pucAddr = (VOS_UINT8*)SCM_GetDebugLogInfo();
    astCcpuDebuginfoTab2[0].ulLenth = SCM_GetDebugLogInfoLen();

    astCcpuDebuginfoTab2[1].pucAddr = (VOS_UINT8*)SOCP_GetDebugLogInfo(OM_GET_SOCP_STATUS);
    astCcpuDebuginfoTab2[1].ulLenth = SOCP_GetDebugLogInfoLen(OM_GET_SOCP_STATUS);

    astCcpuDebuginfoTab2[2].pucAddr = (VOS_UINT8*)SOCP_GetDebugLogInfo(OM_GET_SOCP_DEBUG_INFO);
    astCcpuDebuginfoTab2[2].ulLenth = SOCP_GetDebugLogInfoLen(OM_GET_SOCP_DEBUG_INFO);

     /* ---------------第一包数据 astAcpuDbuginfoTab1[]  begin----------- */
    ulEventLenth = (sizeof(PS_OM_EVENT_IND_STRU)-(4 * sizeof(VOS_UINT8)));
    for (ulIndex=0; ulIndex<(sizeof(astCcpuDebuginfoTab1)/sizeof(astCcpuDebuginfoTab1[0])); ulIndex ++)
    {
        ulEventLenth += astCcpuDebuginfoTab1[ulIndex].ulLenth;
    }

    /* 加上分隔符号长度 */
    ulEventLenth += (ulIndex - 1) * sizeof(VOS_UINT32);

    pstEventInd = (PS_OM_EVENT_IND_STRU *)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, ulEventLenth);
    if(VOS_NULL_PTR == pstEventInd)
    {
        LogPrint("OMRL_CcpuReportEvent: mem malloc Fail!\r\n");

        return VOS_ERR;
    }

    pstEventInd->ulLength   = ulEventLenth;
    pstEventInd->usEventId  = OAM_DEGUG_EVENT_ID1;
    pstEventInd->ulModuleId = WUEPS_PID_OM;

    /*lint -e419 -e416 */
    /* pc侧到ue侧时正常接收数据信息 */
    VOS_MemCpy((VOS_UINT8 *)pstEventInd->aucData,
                  (VOS_UINT8*)&g_stPcToUeSucRecord,
                  sizeof(g_stPcToUeSucRecord));
    /*lint +e419 +e416 */

    ulEventLenth = 0;
    for (ulIndex=1;ulIndex<(sizeof(astCcpuDebuginfoTab1)/sizeof(astCcpuDebuginfoTab1[0])); ulIndex ++)
    {
        /*lint -e419 -e416 */
        /* 加上分隔符号 */
        ulEventLenth += astCcpuDebuginfoTab1[ulIndex-1].ulLenth;
        VOS_MemCpy((VOS_UINT8 *)pstEventInd->aucData + ulEventLenth,
                   (VOS_UINT8 *)&ulTemp, sizeof(VOS_UINT32));

        ulEventLenth += sizeof(VOS_UINT32);
        VOS_MemCpy((VOS_UINT8 *)pstEventInd->aucData + ulEventLenth,
            astCcpuDebuginfoTab1[ulIndex].pucAddr, astCcpuDebuginfoTab1[ulIndex].ulLenth);
         /*lint +e419 +e416 */
    }

    if (VOS_OK != OM_Event(pstEventInd))
    {
        PS_LOG(WUEPS_PID_OM, VOS_NULL, PS_PRINT_WARNING, "OMRL_CcpuReportEvent: send event fail.");
    }
    VOS_MemFree(WUEPS_PID_OM, pstEventInd);
    /* ---------------第一包数据 astAcpuDbuginfoTab1[]  end---------- */

    /* ---------------astAcpuDbuginfoTab2[] 每项数据较大拆包发送，每包最大发送2k----------- */
    /* Event Id */
    usDebugEventID  = OAM_DEGUG_EVENT_ID2;

    /* 一次性申请 2k 空间，最后释放 */
    pstEventInd = (PS_OM_EVENT_IND_STRU *)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT,
                                          OM_SEND_DEBUG_INFO_MAX_LEN + sizeof(PS_OM_EVENT_IND_STRU)-(4 * sizeof(VOS_UINT8)));
    if (VOS_NULL_PTR == pstEventInd)
    {
        LogPrint("OMRL_AcpuReportEvent:Alloc Memory Failed!\n");
        return VOS_ERR;
    }

    /* 数据拆分发送 */
    for(ulIndex =0; ulIndex<(sizeof(astCcpuDebuginfoTab2)/sizeof(astCcpuDebuginfoTab2[0])); ulIndex++)
    {
        /* 分配内容空间 最大为 2k*/
        ulSendLen = 0;

        while(ulSendLen < astCcpuDebuginfoTab2[ulIndex].ulLenth)
        {
            if((astCcpuDebuginfoTab2[ulIndex].ulLenth - ulSendLen) > OM_SEND_DEBUG_INFO_MAX_LEN)
            {
                ulEventLenth = OM_SEND_DEBUG_INFO_MAX_LEN;
            }
            else
            {
                ulEventLenth = astCcpuDebuginfoTab2[ulIndex].ulLenth - ulSendLen;
            }

            pstEventInd->ulLength   = ulEventLenth;
            pstEventInd->usEventId  = usDebugEventID++;
            pstEventInd->ulModuleId = WUEPS_PID_OM;

            /* make pclint happy  begin */
            ulAddr = (VOS_UINT32)pstEventInd->aucData;
            VOS_MemCpy((VOS_UINT8 *)ulAddr,
                       astCcpuDebuginfoTab2[ulIndex].pucAddr + ulSendLen, ulEventLenth);
            /* make pclint happy  end */

            ulSendLen += ulEventLenth;
            if (VOS_OK != OM_Event(pstEventInd))
            {
                PS_LOG(WUEPS_PID_OM, VOS_NULL, PS_PRINT_WARNING, "OMRL_AcpuReportEvent: send event fail.");
            }
        }
    }

    /* 释放消息内容空间 */
    VOS_MemFree(WUEPS_PID_OM, pstEventInd);

    return VOS_OK;
}


VOS_UINT32 OMRL_ReleaseSocpData(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo)
{
    VOS_UINT_PTR                        DataAddr;
    VOS_UINT32                          ulDataLen;
    VOS_UINT8                           *pucRealAddr;
    SOCP_CODER_SRC_ENUM_U32             enChanlID;
    OM_BUF_CTRL_STRU                    *pstCtrlBuf = VOS_NULL_PTR;

    enChanlID = pstCtrlInfo->enChanlID;
    pstCtrlBuf = &(pstCtrlInfo->OmBufCtrl);

#if 0
    VOS_BOOL                            bLoopFlag = VOS_TRUE;

    /* 为了测试充分，这里先按一个一个RD释放 */
    do
    {
        /* 从SCM获得已发送消息总长度 */
        if (VOS_OK != SCM_RlsSrcRDFirst(SOCP_CODER_SRC_GU_OM2, &ulDataAddr, &ulDataLen))
        {
            return VOS_ERR;
        }

        /* 已经没有RD */
        if ((0 == ulDataAddr) || (0 == ulDataLen))
        {
            break;
        }

        pucRealAddr = (VOS_UINT8*)OM_AddrRealToVirt(&g_stOmBufCtrl, (VOS_UINT8*)ulDataAddr);

        if (VOS_NULL_PTR == pucRealAddr)
        {
            return VOS_ERR;
        }

        /* 释放OM缓冲区内存 */
        if (VOS_OK != OM_ReleaseTraceMem(&g_stOmBufCtrl, pucRealAddr, (VOS_INT32)ulDataLen))
        {
            return VOS_ERR;
        }

    }while(bLoopFlag);

#else
    /* 清空所有已发送RD */
    if (VOS_OK != SCM_RlsSrcRDAll(enChanlID, &DataAddr, &ulDataLen))
    {
        pstCtrlInfo->stUeToPcErrRecord.usRlsRdErr++;

        return VOS_ERR;
    }

    /* 已经没有RD */
    if ((0 == DataAddr) || (0 == ulDataLen))
    {
        return VOS_OK;
    }

    pstCtrlInfo->stUeToPcSucRecord.stRealRlsData.ulDataLen += ulDataLen;
    pstCtrlInfo->stUeToPcSucRecord.stRealRlsData.ulNum++;

    pucRealAddr = (VOS_UINT8*)OM_AddrRealToVirt(pstCtrlBuf, (VOS_UINT8*)DataAddr);

    if (VOS_NULL_PTR == pucRealAddr)
    {
        pstCtrlInfo->stUeToPcErrRecord.usRlsAddrErr++;

        return VOS_ERR;
    }

    /* 根据已发送RD总长度释放内存 */
    if (VOS_OK != OM_ReleaseTraceMem(pstCtrlBuf, pucRealAddr, (VOS_INT32)ulDataLen))
    {
        pstCtrlInfo->stUeToPcErrRecord.usRlsMemErr++;

        g_ulOMSwitchOnOff = OM_STATE_IDLE;

        return VOS_ERR;

    }
#endif

    return VOS_OK;
}
VOS_UINT32 OMRL_SendSocpData(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo,
                             VOS_UINT8 *pucHead, VOS_UINT32 ulHeadLen,
                             VOS_UINT8 *pucData, VOS_UINT32 ulDataLen)
{
    VOS_UINT8                          *pucMemAddr;
    VOS_INT32                           lUsedBytes;
    VOS_UINT8                          *pucRealMemAddr;
    SOCP_CODER_SRC_ENUM_U32             enChanlID;
    OM_BUF_CTRL_STRU                   *pstCtrlBuf = VOS_NULL_PTR;

    enChanlID = pstCtrlInfo->enChanlID;
    pstCtrlBuf = &(pstCtrlInfo->OmBufCtrl);

    /*lint -e40*/
    OM_CCPU_DEBUG_TRACE(pucHead, ulHeadLen, OM_CCPU_PUT_OM_BUFF);
    OM_CCPU_DEBUG_TRACE(pucData, ulDataLen, OM_CCPU_PUT_OM_BUFF);
    /*lint +e40*/

    /* sizeof(VOS_UINT32)代表桢尾的数据长度 */
    pucMemAddr = (VOS_UINT8*)OM_AllocTraceMem(pstCtrlBuf, (VOS_INT32)(ulHeadLen + ulDataLen + sizeof(VOS_UINT32)));

    if (VOS_NULL_PTR == pucMemAddr)
    {
        pstCtrlInfo->stUeToPcErrRecord.usNoMemErr++;

        return VOS_ERR;
    }

    /* 拷贝数据头 */
    VOS_MemCpy(pucMemAddr, pucHead, ulHeadLen);

    /* 拷贝数据内容 */
    VOS_MemCpy(pucMemAddr + ulHeadLen, pucData, ulDataLen);

    /* 填写桢尾 */
    *(VOS_UINT32*)(pucMemAddr + ulHeadLen + ulDataLen) = OM_USB_FRAME_TAIL;

    VOS_FlushCpuWriteBuf();

    pucRealMemAddr = (VOS_UINT8*)OM_AddrVirtToReal(pstCtrlBuf, pucMemAddr);

    if (VOS_NULL_PTR == pucRealMemAddr)
    {
        pstCtrlInfo->stUeToPcErrRecord.usSendAddrErr++;

        return VOS_ERR;
    }

    /* 调用SCM提供的接口将数据发送给SOCP */
    if(VOS_OK != SCM_SendCoderSrc(enChanlID, pucRealMemAddr, ulHeadLen + ulDataLen + sizeof(VOS_UINT32)))
    {
        pstCtrlInfo->stUeToPcErrRecord.usSendSocpDataErr++;

        return VOS_ERR;
    }

    pstCtrlInfo->stUeToPcSucRecord.stRealSendData.ulDataLen += (ulHeadLen + ulDataLen + sizeof(VOS_UINT32));
    pstCtrlInfo->stUeToPcSucRecord.stRealSendData.ulNum++;

    lUsedBytes = OM_TraceMemNBytes(pstCtrlBuf);

    if (pstCtrlInfo->stUeToPcSucRecord.ulMaxUsedBufSize < (VOS_UINT32)lUsedBytes)
    {
        pstCtrlInfo->stUeToPcSucRecord.ulMaxUsedBufSize = (VOS_UINT32)lUsedBytes;
    }

    return VOS_OK;
}
VOS_UINT32 OMRL_IsBDEnough(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 ucMsgCnt)
{
    VOS_UINT32                          ulFreeBdNum = 0;

    /* 获取OM源通道剩余BD个数 */
    if (VOS_OK != SCM_GetBDFreeNum(pstCtrlInfo->enChanlID, &ulFreeBdNum))
    {
        pstCtrlInfo->stUeToPcErrRecord.usGetBDErr++;

        return VOS_ERR;
    }

    /* 判断剩余BD是否满足分包个数 */
    if (ucMsgCnt > ulFreeBdNum)
    {
        pstCtrlInfo->stUeToPcErrRecord.usBDLackErr++;

        return VOS_ERR;
    }

    return VOS_OK;
}


#if (FEATURE_ON == FEATURE_MANUFACTURE_LOG)

/*****************************************************************************
 函 数 名  : OMRL_WriteLittleImg
 功能描述  : 生产小镜像时保存信息到flash
 输入参数  : pucMsg    -- 指向发送数据的指针
             usMsgLen  -- 发送数据的长度
 输出参数  : 无
 返 回 值  : ture or false

 修改历史      :
  1.日    期   : 2009年3月28日
    作    者   : 王笑非
    修改内容   : 新生成函数

*****************************************************************************/
VOS_BOOL OMRL_WriteLittleImg(VOS_UINT8 *pucMsg, VOS_UINT16 usMsgLen)
{
    OM_APP_TRACE_STRU                  *pstAppTrace;
    VOS_UINT32                          ulTempAddress;
    VOS_UINT16                          usLogSaveLen;
    VOS_UINT32                         *pucLogSaveMsg;

    pstAppTrace = (OM_APP_TRACE_STRU*)pucMsg;

    if ((OM_TRACE_FUNC == pstAppTrace->stAppHeader.ucFuncType) && (OM_APP_TRACE_IND == pstAppTrace->usPrimId))
    {

        /* 写入Trace头 */
        OM_LittleImgWriteLogFile((VOS_CHAR*)pucMsg, OM_APP_TRACE_LEN);

        /* 获取消息内容指针 */
        ulTempAddress = *(VOS_UINT32*)(pstAppTrace->aucPara);

        pucLogSaveMsg = (VOS_UINT32*)ulTempAddress;

        usLogSaveLen  = usMsgLen - OM_APP_TRACE_LEN;

        OM_LittleImgWriteLogFile((VOS_CHAR*)pucLogSaveMsg, usLogSaveLen);

        return VOS_TRUE;
    }

    /* 将LOG信息保存到文件系统 */
    if (VOS_OK != OM_LittleImgWriteLogFile((VOS_CHAR*)pucMsg, usMsgLen))
    {
        PS_LOG(WUEPS_PID_OM, 0, LOG_LEVEL_INFO, "OMRL_MsgSplit: No need to send msg to PC.\n");

        return VOS_TRUE;
    }

    return VOS_FALSE;
}

#endif

/*****************************************************************************
 函 数 名  : OMRL_ProcessSendData
 功能描述  : 将可维可测数据发送的SOCP模块
 输入参数  : pucMsg    -- 指向发送数据的指针
             usMsgLen  -- 发送数据的长度
             ucMsgCnt  -- 待发送数据包个数
 输出参数  : 无
 返 回 值  : VOS_UINT32

 修改历史      :
  1.日    期   : 2009年3月28日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OMRL_ProcessSendData(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucMsg, VOS_UINT8 ucMsgCnt, VOS_UINT16 usMsgLen)
{
    VOS_UINT8                           ucCurSegNum = 1; /*当前段序号*/
    VOS_UINT32                          ulTempAddress;
    VOS_BOOL                            bIsHead     = VOS_FALSE;
    OM_APP_TRACE_STRU                  *pstAppTrace;
    /*指向OM分包头内容的指针*/
    OM_APP_MSG_SEG_EX_STRU             *pstSegMsgEx = VOS_NULL_PTR;


    /*分配分包结构的内存空间*/
    pstSegMsgEx = (OM_APP_MSG_SEG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM,
                DYNAMIC_MEM_PT, sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_APP_TRACE_LEN);

    if (VOS_NULL_PTR == pstSegMsgEx)
    {
        LogPrint("OM_UsbFrameInit:VOS_MemAlloc g_pstSegMsgEx Failed!\n");
        return VOS_ERR;
    }

    pstSegMsgEx->stSocpHdr.ucCPUID   = OM_TOOL_CCPU_ID;
    pstSegMsgEx->ulFrameHead         = OM_USB_FRAME_HEAD;

    pstAppTrace = (OM_APP_TRACE_STRU*)pucMsg;

    ulTempAddress = (VOS_UINT32)pucMsg;

    if ((OM_TRACE_FUNC == pstAppTrace->stAppHeader.ucFuncType)
        &&(OM_APP_TRACE_IND == pstAppTrace->usPrimId))
    {
        ulTempAddress = *(VOS_UINT32*)(pstAppTrace->aucPara);
        DRV_RT_MEMCPY(pstSegMsgEx + 1, pucMsg, OM_APP_TRACE_LEN);
        bIsHead = VOS_TRUE;
    }

    /* 填充SOCP头中的RTC值 */
    pstSegMsgEx->stSocpHdr.ulRTC = (VOS_UINT32)DRV_GET_RTC_VALUE();

    pstSegMsgEx->stMsgSeg.ucSegNum = ucMsgCnt;
    pstSegMsgEx->stMsgSeg.usSegLen = OM_APP_SPLIT_MSG_LEN;


    /* 大于最大分包大小的数据，按照最大分包大小进行数据发送的处理 */
    for (ucCurSegNum = 1; ucCurSegNum < ucMsgCnt; ucCurSegNum++)
    {
        pstSegMsgEx->stMsgSeg.ucSegSn = ucCurSegNum;

        if (VOS_TRUE == bIsHead)
        {
            /* 消息头前加上长度信并放入环形缓冲区*/
            if (VOS_OK != OMRL_SendSocpData(pstCtrlInfo, (VOS_UINT8 *)pstSegMsgEx,
                (sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_APP_TRACE_LEN),
                (VOS_UINT8*)ulTempAddress, OM_APP_MSG_SEGMENT_LEN - OM_APP_TRACE_LEN))
            {
                VOS_MemFree(WUEPS_PID_OM, pstSegMsgEx);

                return VOS_ERR;
            }

            bIsHead       =  VOS_FALSE;
            ulTempAddress += (OM_APP_MSG_SEGMENT_LEN - OM_APP_TRACE_LEN);
        }
        else
        {
            /* 消息头前加上长度信并放入环形缓冲区*/
            if (VOS_OK != OMRL_SendSocpData(pstCtrlInfo, (VOS_UINT8 *)pstSegMsgEx,
                 sizeof(OM_APP_MSG_SEG_EX_STRU),
                (VOS_UINT8*)ulTempAddress, OM_APP_MSG_SEGMENT_LEN))
            {
                VOS_MemFree(WUEPS_PID_OM, pstSegMsgEx);

                return VOS_ERR;
            }

            ulTempAddress += OM_APP_MSG_SEGMENT_LEN;
        }

        /* 计算剩余数据包大小 */
        usMsgLen -= OM_APP_MSG_SEGMENT_LEN;
    }

    /* 设置消息头参数 */
    pstSegMsgEx->stMsgSeg.usSegLen = (VOS_UINT16)(usMsgLen + OM_APP_MSG_SEGMENT_HEADER_LENGTH);
    pstSegMsgEx->stMsgSeg.ucSegSn  = ucCurSegNum;

    /*为了保持Ringbuffer中存放的数据包4字节对齐*/
    usMsgLen = (usMsgLen + 3) & ~0x03;

    if (VOS_TRUE == bIsHead)
    {
        /* 消息头前加上长度并放入环形缓冲区 */
        if ( VOS_OK != OMRL_SendSocpData(pstCtrlInfo, (VOS_UINT8 *)pstSegMsgEx,
                            (sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_APP_TRACE_LEN),
                            (VOS_UINT8*)ulTempAddress,
                            usMsgLen - OM_APP_TRACE_LEN))
        {
            VOS_MemFree(WUEPS_PID_OM, pstSegMsgEx);

            return VOS_ERR;
        }
    }
    else
    {
        /* 消息头前加上长度并放入环形缓冲区 */
        if ( VOS_OK != OMRL_SendSocpData(pstCtrlInfo, (VOS_UINT8 *)pstSegMsgEx,
                                        sizeof(OM_APP_MSG_SEG_EX_STRU),
                                        (VOS_UINT8*)ulTempAddress,
                                        usMsgLen))
        {
            VOS_MemFree(WUEPS_PID_OM, pstSegMsgEx);

            return VOS_ERR;
        }
    }

    VOS_MemFree(WUEPS_PID_OM, pstSegMsgEx);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OMRL_MsgSplit
 功能描述  : 当在USB上进行通信时，使用的扩展分包函数
 输入参数  : pucMsg    -- 指向发送数据的指针
             usMsgLen  -- 发送数据的长度
 输出参数  : 无
 返 回 值  : VOS_UINT32

 修改历史      :
  1.日    期   : 2009年3月28日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OMRL_MsgSplit(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucMsg, VOS_UINT16 usMsgLen)
{
    VOS_UINT8                           ucMsgCnt    = 1; /*分段的数量*/
    VOS_UINT32                          ulRet;

    /*进行互斥操作*/
    if ( VOS_OK != VOS_SmP(pstCtrlInfo->OmTxBuffSem, OM_PV_TIMEOUT) )
    {
        LogPrint("OMRL_MsgSplit, Error, g_OmTxBuffSem VOS_SmP Failed.\n");

        pstCtrlInfo->stUeToPcErrRecord.usSemErr++;
        return VOS_ERR;
    }

    /* 释放已发送数据内存 */
    if (VOS_OK != OMRL_ReleaseSocpData(pstCtrlInfo))
    {
        VOS_SmV(pstCtrlInfo->OmTxBuffSem);

        return VOS_ERR;
    }

    /* 计算分包个数 */
    ucMsgCnt = (VOS_UINT8)(((usMsgLen + OM_APP_MSG_SEGMENT_LEN) - 1)/OM_APP_MSG_SEGMENT_LEN);

    /* 判断剩余BD是否满足分包个数 */
    if (VOS_OK != OMRL_IsBDEnough(pstCtrlInfo, ucMsgCnt))
    {
        VOS_SmV(pstCtrlInfo->OmTxBuffSem);

        return VOS_ERR;
    }

    ulRet = OMRL_ProcessSendData(pstCtrlInfo, pucMsg, ucMsgCnt, usMsgLen);

    VOS_SmV(pstCtrlInfo->OmTxBuffSem);

    return ulRet;
}
VOS_UINT32 OMRL_UsbFrameInit(VOS_VOID)
{
    VOS_MemSet(&g_stTxCnfCtrlInfo, 0, sizeof(g_stTxCnfCtrlInfo));

    VOS_MemSet(&g_stTxIndCtrlInfo, 0, sizeof(g_stTxIndCtrlInfo));

    g_stTxCnfCtrlInfo.enChanlID = SOCP_CODER_SRC_GU_CNF2;

    g_stTxCnfCtrlInfo.enChannel = OM_LOGIC_CHANNEL_CNF;

    g_stTxIndCtrlInfo.enChanlID = SOCP_CODER_SRC_GU_IND2;

    g_stTxIndCtrlInfo.enChannel = OM_LOGIC_CHANNEL_IND;

    /* 创建OM CNF发送缓冲区 */
    if (VOS_OK != OM_CreateTraceBuffer(&(g_stTxCnfCtrlInfo.OmBufCtrl), OM_CNF_BUFFER_SIZE))
    {
        LogPrint("OMRL_UsbFrameInit:OM_CreateTraceBuffer Failed!\n");

        return VOS_ERR;
    }

    /* 创建OM IND发送缓冲区 */
    if (VOS_OK != OM_CreateTraceBuffer(&(g_stTxIndCtrlInfo.OmBufCtrl), OM_UPLINK_BUFFER_SIZE))
    {
        LogPrint("OMRL_UsbFrameInit:OM_CreateTraceBuffer Failed!\n");

        return VOS_ERR;
    }

    /* 创建发送信号量 */
    if (VOS_OK != VOS_SmMCreate("TXCNF", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_stTxCnfCtrlInfo.OmTxBuffSem))
    {
        LogPrint("OMRL_UsbFrameInit: Error, VOS_SmMCreate Fail\n");
        return VOS_ERR;
    }

    /* 创建发送信号量 */
    if (VOS_OK != VOS_SmMCreate("TXIND", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_stTxIndCtrlInfo.OmTxBuffSem))
    {
        LogPrint("OMRL_UsbFrameInit: Error, VOS_SmMCreate Fail\n");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_INT OM_SaveTraceBuffer(VOS_INT lFuncType, VOS_UINT8* pucData, VOS_UINT32* pulLen)
{
    /*参数检测*/
    if ((OM_TRACE_FUNC != lFuncType) || (VOS_NULL_PTR == pucData)
        || (VOS_NULL_PTR == pulLen) || (0 == *pulLen))
    {
        LogPrint("OM_SaveBuffer:Input para. is not correct.\n");

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_INT OM_SaveLogBuffer(VOS_INT lFuncType, VOS_UINT8* pucData, VOS_UINT32* pulLen)
{
    OM_RING_ID      ringId;
    VOS_INT         lPacketLen = 0;
    VOS_INT         lBufferLen;
    VOS_UINT8       *pucTemp;

    if ((OM_LOG_FUNC != lFuncType) || (VOS_NULL_PTR == pucData)
        || (VOS_NULL_PTR == pulLen) || (0 == *pulLen))
    {
        LogPrint("OM_SaveBuffer:Input para. is not correct.\n");

        return VOS_ERR;
    }

    /*得到LOG数据缓冲区*/
    ringId = g_stLogEnt.rngOmRbufId;

    lBufferLen = (VOS_INT)(*pulLen);

    pucTemp = pucData;

    VOS_TaskLock();

    while (!OM_RingBufferIsEmpty(ringId))
    {
        if (sizeof(lPacketLen) != OM_RingBufferGet(ringId,
                                        (VOS_CHAR*)&lPacketLen, sizeof(lPacketLen)))
        {
            LogPrint("OM_SaveBuffer:LOG OM_RingBufferGet fail\n");

            break;
        }

        /*删除存放在LOG Buffer中的模块ID*/
        if (LOG_MODULE_ID_LEN != OM_RingBufferRemove(ringId, LOG_MODULE_ID_LEN))
        {
            LogPrint("OM_SaveBuffer:LOG OM_RingBufferRemove fail\n");
            break;
        }

        lPacketLen -= LOG_MODULE_ID_LEN;

        /*判断缓冲区是否已满*/
        if (lBufferLen < (lPacketLen + 1))
        {
            break;
        }

        /*从Ringbuffer中删除数据*/
        if (lPacketLen != OM_RingBufferGet(ringId, (VOS_CHAR*)pucTemp, lPacketLen))
        {
            LogPrint1("OM_SaveBuffer:LOG OM_RingBufferGet len : %d\n", lPacketLen);

            break;
        }

        /*为了打点工具解析，需要加\n*/
        pucTemp[lPacketLen] = '\n';

        /*加1是因为前面多加了\n操作*/
        pucTemp += (lPacketLen + 1);

        lBufferLen -= (lPacketLen + 1);
    }

    VOS_TaskUnlock();

    *pulLen = (VOS_UINT32)(pucTemp - pucData);

    return VOS_OK;
}


VOS_VOID OmInitVar(VOS_VOID)
{
    VOS_MemSet(&g_stPcToUeErrRecord, 0, sizeof(g_stPcToUeErrRecord));
    VOS_MemSet(&g_stPcToUeSucRecord, 0, sizeof(g_stPcToUeSucRecord));
}


VOS_UINT32 OMRL_SendMsg(VOS_UINT8 *pucData, VOS_UINT32 ulSize, VOS_UINT32 ulSenderPid, VOS_UINT32 ulReceiverPid)
{

    MsgBlock           *pstAppOmMsg = VOS_NULL_PTR;
    VOS_UINT32          ulRslt;

    pstAppOmMsg = VOS_AllocMsg(ulSenderPid, ulSize);
    if (VOS_NULL_PTR == pstAppOmMsg)
    {
        /* 打印错误 */
        return VOS_ERR;
    }

    pstAppOmMsg->ulReceiverPid = ulReceiverPid;

    /* copy 消息内容 */
    VOS_MemCpy((VOS_UINT8 *)pstAppOmMsg->aucValue, pucData, ulSize);

    ulRslt = VOS_SendMsg(ulSenderPid, pstAppOmMsg);
    if (VOS_OK != ulRslt )
    {
        /* 打印错误 */
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 OMRL_IccDataRecv(VOS_UINT8 *pucData, VOS_UINT16 uslen)
{

    g_stPcToUeSucRecord.stTotalData.ulDataLen += (VOS_UINT32)uslen;
    g_stPcToUeSucRecord.stTotalData.ulNum++;

    if ((VOS_NULL_PTR == pucData) || (0 == uslen) || (APP_OM_HEADER_LENGTH >= uslen))
    {
        PS_LOG(WUEPS_PID_OM, 0, LOG_LEVEL_ERROR, "OMRL_IccDataRecv: Parameter Error!");
        g_stPcToUeErrRecord.stParaErrData.ulDataLen += uslen;
        g_stPcToUeErrRecord.stParaErrData.ulNum++;

        return VOS_ERR;
    }

    /*lint -e40*/
    OM_CCPU_DEBUG_TRACE(pucData, uslen, OM_CCPU_RECV_ICC);
    /*lint +e40*/

    /* 当发生释放消息出错时停止接收数据 */
    if ((0 < g_stTxCnfCtrlInfo.stUeToPcErrRecord.usRlsMemErr) || (0 < g_stTxIndCtrlInfo.stUeToPcErrRecord.usRlsMemErr))
    {
        return VOS_OK;
    }

    if (VOS_OK != OMRL_SendMsg(pucData, (VOS_UINT32)uslen, UEPS_PID_OMRL, WUEPS_PID_OM))
    {
        g_stPcToUeErrRecord.stSendErrData.ulDataLen += uslen;
        g_stPcToUeErrRecord.stSendErrData.ulNum++;

        return VOS_ERR;
    }

    g_stPcToUeSucRecord.stRealData.ulDataLen += uslen;
    g_stPcToUeSucRecord.stRealData.ulNum++;

    return VOS_OK;
}
VOS_UINT32 OMRL_Init(VOS_VOID)
{
    OmInitVar();

    if (VOS_OK != OMRL_UsbFrameInit())
    {
        LogPrint("RL_Init:OM_UsbFrameInit Fail.\n");
        return VOS_ERR;
    }

    if (VOS_OK != DRV_SAVE_REGISTER(OM_TRACE_FUNC, (OM_SAVE_FUNC*)OM_SaveTraceBuffer))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "RL_Init: DRV_SAVE_REGISTER OM_TRACE_FUNC Fail!");
    }

    if (VOS_OK != DRV_SAVE_REGISTER(OM_LOG_FUNC, (OM_SAVE_FUNC*)OM_SaveLogBuffer))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "RL_Init: DRV_SAVE_REGISTER OM_LOG_FUNC Fail!");
    }

    OM_ComRx_RegisterCB(OM_OM_ICC_CHANNEL, OMRL_IccDataRecv);

    return VOS_OK;
}


VOS_VOID OmSendIndSucShow(VOS_VOID)
{
    OM_UE_PC_SUC_STRU                  *pstUeToPcSucRecord = &(g_stTxIndCtrlInfo.stUeToPcSucRecord);

    vos_printf("\r\n *****IND channel success info show *******\r\n");

    vos_printf("Total Data Send by Ps: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stTotalData.ulNum, pstUeToPcSucRecord->stTotalData.ulDataLen);

    vos_printf("Real Data send to Socp: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stRealSendData.ulNum, pstUeToPcSucRecord->stRealSendData.ulDataLen);

    vos_printf("Real Data release by Socp: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stRealRlsData.ulNum, pstUeToPcSucRecord->stRealRlsData.ulDataLen);

    vos_printf("Max Used Buffer Size: size:%6u\n", pstUeToPcSucRecord->ulMaxUsedBufSize);

    return;
}


VOS_VOID OmSendCnfSucShow(VOS_VOID)
{
    OM_UE_PC_SUC_STRU                  *pstUeToPcSucRecord = &(g_stTxCnfCtrlInfo.stUeToPcSucRecord);

    vos_printf("\r\n *****CNF channel success info show *******\r\n");

    vos_printf("Total Data Send by Ps: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stTotalData.ulNum, pstUeToPcSucRecord->stTotalData.ulDataLen);

    vos_printf("Real Data send to Socp: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stRealSendData.ulNum, pstUeToPcSucRecord->stRealSendData.ulDataLen);

    vos_printf("Real Data release by Socp: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stRealRlsData.ulNum, pstUeToPcSucRecord->stRealRlsData.ulDataLen);

    vos_printf("Max Used Buffer Size: size:%6u\n", pstUeToPcSucRecord->ulMaxUsedBufSize);

    return;
}


VOS_VOID OmSendIndErrShow(VOS_VOID)
{
    OM_UE_PC_FAIL_STRU                  *pstUeToPcErrRecord = &(g_stTxIndCtrlInfo.stUeToPcErrRecord);

    vos_printf("\r\n *****IND channel error info show *******\r\n");

    vos_printf("Total Lost Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stLostData.ulNum, pstUeToPcErrRecord->stLostData.ulDataLen);

    vos_printf("Link Err Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stLinkErrData.ulNum, pstUeToPcErrRecord->stLinkErrData.ulDataLen);

    vos_printf("Para Err Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stParaErrData.ulNum, pstUeToPcErrRecord->stParaErrData.ulDataLen);

    vos_printf("num of sem take err: num:%6u\n", pstUeToPcErrRecord->usSemErr);

    vos_printf("num of release RD err: num:%6u\n", pstUeToPcErrRecord->usRlsRdErr);

    vos_printf("num of release Mem err: num:%6u\n", pstUeToPcErrRecord->usRlsMemErr);

    vos_printf("num of BD Lack err: num:%6u\n", pstUeToPcErrRecord->usBDLackErr);

    vos_printf("num of get BD err: num:%6u\n", pstUeToPcErrRecord->usGetBDErr);

    vos_printf("num of No Mem err: num:%6u\n", pstUeToPcErrRecord->usNoMemErr);

    vos_printf("num of Send Socp Data err: num:%6u\n", pstUeToPcErrRecord->usSendSocpDataErr);

    vos_printf("num of transfer sending addr err: num:%6u\n", pstUeToPcErrRecord->usSendAddrErr);

    vos_printf("num of transfer Release addr err: num:%6u\n", pstUeToPcErrRecord->usRlsAddrErr);

    return;
}


VOS_VOID OmSendCnfErrShow(VOS_VOID)
{
    OM_UE_PC_FAIL_STRU                  *pstUeToPcErrRecord = &(g_stTxCnfCtrlInfo.stUeToPcErrRecord);

    vos_printf("\r\n *****CNF channel error info show *******\r\n");

    vos_printf("Total Lost Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stLostData.ulNum, pstUeToPcErrRecord->stLostData.ulDataLen);

    vos_printf("Link Err Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stLinkErrData.ulNum, pstUeToPcErrRecord->stLinkErrData.ulDataLen);

    vos_printf("Para Err Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stParaErrData.ulNum, pstUeToPcErrRecord->stParaErrData.ulDataLen);

    vos_printf("num of sem take err: num:%6u\n", pstUeToPcErrRecord->usSemErr);

    vos_printf("num of release RD err: num:%6u\n", pstUeToPcErrRecord->usRlsRdErr);

    vos_printf("num of release Mem err: num:%6u\n", pstUeToPcErrRecord->usRlsMemErr);

    vos_printf("num of BD Lack err: num:%6u\n", pstUeToPcErrRecord->usBDLackErr);

    vos_printf("num of get BD err: num:%6u\n", pstUeToPcErrRecord->usGetBDErr);

    vos_printf("num of No Mem err: num:%6u\n", pstUeToPcErrRecord->usNoMemErr);

    vos_printf("num of Send Socp Data err: num:%6u\n", pstUeToPcErrRecord->usSendSocpDataErr);

    vos_printf("num of transfer sending addr err: num:%6u\n", pstUeToPcErrRecord->usSendAddrErr);

    vos_printf("num of transfer Release addr err: num:%6u\n", pstUeToPcErrRecord->usRlsAddrErr);

    return;
}


VOS_VOID OmRcvShow(VOS_VOID)
{
    vos_printf("Total Data Recv From ACPU: num:%6u, len:%6u\n",
                g_stPcToUeSucRecord.stTotalData.ulNum, g_stPcToUeSucRecord.stTotalData.ulDataLen);

    vos_printf("Data send OM:   num:%6u, len:%6u\n",
                g_stPcToUeSucRecord.stRealData.ulNum, g_stPcToUeSucRecord.stRealData.ulDataLen);

    vos_printf("Data Recv From ICC: num:%6u, len:%6u, slice:%6u\n",
                g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecMsgNum,
                g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecLen,
                g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecSlice);

    return;
}


VOS_VOID OmRcvErrShow(VOS_VOID)
{
    vos_printf("Lost Data because para Err: num:%6u, len:%6u\n",
            g_stPcToUeErrRecord.stParaErrData.ulNum, g_stPcToUeErrRecord.stParaErrData.ulDataLen);

    vos_printf("Err data when sending msg to om: num:%6u, len:%6u\n",
            g_stPcToUeErrRecord.stSendErrData.ulNum, g_stPcToUeErrRecord.stSendErrData.ulDataLen);

    vos_printf("Data rcv err from ICC: ParaErrNum:%6u, len:%6u, MsgNum:%6u, slice:%6u\n",
            g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmParaErrNum,
            g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmRecErrLen,
            g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmRecErrMsgNum,
            g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmRecErrSlice);

    return;
}


VOS_VOID OmStatus(VOS_UINT32 ulFlag)
{
    return;
}


VOS_VOID OmRate(VOS_VOID)
{
#if 0
    VOS_UINT32            ulCurrTick;                    /*记录现在统计时间*/
    VOS_UINT32            ulDeltaTick;                   /*记录统计时间*/
    VOS_UINT32            ulDelData;                     /*记录统计数据大小*/
    VOS_UINT32            ulBandWidth;                   /*记录统计数据大小*/

    vos_printf("OM Rate Show (CCPU To SOCP):\n");

    ulCurrTick = VOS_GetTick();

    if (ulCurrTick < g_ulLastTick)
    {
        ulDeltaTick = (0xFFFFFFFF - g_ulLastTick) + ulCurrTick;
    }
    else
    {
        ulDeltaTick = ulCurrTick - g_ulLastTick;
    }


    ulBandWidth    = (ulDelData/ulDeltaTick)/10;

    /*统计OM在ulDeltaTick时间(单位10ms)内发送了ulTotalLen字节数据*/
    vos_printf("OmRate,Tick <%u> Byte <%u>, BandWidth <%u>.\n",
                 ulDeltaTick, ulDelData, ulBandWidth);

    g_ulLastTick = ulCurrTick;

    vos_printf("OM Rate Show End\n");
#endif
}
VOS_VOID OmOpenCCPULog(VOS_UINT32 ulFlag)
{
    g_ulOmCcpuDbgFlag = ulFlag;

    return;
}

/*****************************************************************************
 Prototype       : GU_OmCcpuSwitchOnOff
 Description     : ON or Off Ccpu Switch
 Input           : VOS_UINT32
 Output          : None
 Return Value    : None

 History         : ---
    Date         :
    Author       :
    Modification : Created function
 *****************************************************************************/
VOS_VOID GU_OmCcpuSwitchOnOff(VOS_UINT32 ulFlag)
{
    g_ulOMSwitchOnOff = ulFlag;
}



VOS_VOID OM_LogShowToFile(VOS_BOOL bIsSendMsg)
{
    FILE                               *fp;
    OM_AUTOCONFIG_CNF_STRU             *pstSendCnf;
    VOS_UINT32                          ulTemp = 0x5a5a5a5a;

#if(VOS_WIN32 == VOS_OS_VER)
    VOS_CHAR                  g_acLogPath[] = ".\\yaffs0\\C_OmLog.bin";

#else
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
    VOS_CHAR                  g_acLogPath[] = "/modem_log/C_OmLog.bin";

#else
    VOS_CHAR                  g_acLogPath[] = "/yaffs0/C_OmLog.bin";
#endif
#endif

    fp = DRV_FILE_OPEN(g_acLogPath, "w+");

    if (VOS_NULL_PTR == fp)
    {
        return;
    }

    DRV_FILE_WRITE((VOS_VOID*)&g_stPcToUeSucRecord, sizeof(VOS_CHAR), sizeof(g_stPcToUeSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)&g_stPcToUeErrRecord, sizeof(VOS_CHAR), sizeof(g_stPcToUeErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_WRITE((VOS_VOID*)&(g_stTxCnfCtrlInfo.stUeToPcSucRecord), sizeof(VOS_CHAR), sizeof(g_stTxCnfCtrlInfo.stUeToPcSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&(g_stTxIndCtrlInfo.stUeToPcSucRecord), sizeof(VOS_CHAR), sizeof(g_stTxIndCtrlInfo.stUeToPcSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)&(g_stTxCnfCtrlInfo.stUeToPcErrRecord), sizeof(VOS_CHAR), sizeof(g_stTxCnfCtrlInfo.stUeToPcErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&(g_stTxIndCtrlInfo.stUeToPcErrRecord), sizeof(VOS_CHAR), sizeof(g_stTxIndCtrlInfo.stUeToPcErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_WRITE((VOS_VOID*)&g_ulOMSwitchOnOff, sizeof(VOS_CHAR), sizeof(g_ulOMSwitchOnOff), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    /* SCM 相关*/
    SCM_LogToFile(fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    /* SOCP相关 */
    SOCP_LogToFile(fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_CLOSE(fp);

    /* 发送消息給 Acpu */
    if(VOS_FALSE == bIsSendMsg)
    {
        return;
    }

    pstSendCnf = (OM_AUTOCONFIG_CNF_STRU*)VOS_AllocMsg(CCPU_PID_OMAGENT,
                            sizeof(OM_AUTOCONFIG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 分配消息失败 */
    if (VOS_NULL_PTR == pstSendCnf)
    {
        return;
    }

    pstSendCnf->ulReceiverPid  = ACPU_PID_OMAGENT;
    pstSendCnf->usPrimId       = OM_RECORD_DBU_INFO_REQ;

    (VOS_VOID)VOS_SendMsg(CCPU_PID_OMAGENT, pstSendCnf);

    return;
}
#endif //(FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

