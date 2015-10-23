
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "om.h"
#include "SOCPInterface.h"
#include "omprivate.h"
#include "ombbplog.h"
#include "SCMProc.h"
#include "BSP_SOCP_DRV.h"
#include "NvIdList.h"
#include "NVIM_Interface.h"
#include "bbp_wcdma_interface.h"
#include "bbp_comm_interface.h"
#include "gbbp_interface.h"
#include "omnvinterface.h"



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


/*lint -e767 修改人：蒋德彬 */
#define    THIS_FILE_ID        PS_FILE_ID_OM_BBP_LOG_C
/*lint +e767 修改人：蒋德彬 */

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* GU BBP 可维可测上报SOCP 通道配置参数 */
SCM_CODER_SRC_CFG_STRU                  g_astBbpDbgCoderSrcCfg[BBP_DBG_CODER_SRC_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_GUBBP1,   SOCP_CODER_DST_GU_OM,   SOCP_DATA_TYPE_1, SOCP_ENCSRC_CHNMODE_CTSPACKET, SOCP_CHAN_PRIORITY_3, VOS_NULL_PTR, VOS_NULL_PTR, BBP_DBG_CODER_SRC_SIZE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_GUBBP2,   SOCP_CODER_DST_GU_OM,   SOCP_DATA_TYPE_1, SOCP_ENCSRC_CHNMODE_CTSPACKET, SOCP_CHAN_PRIORITY_3, VOS_NULL_PTR, VOS_NULL_PTR, BBP_DBG_CODER_SRC_SIZE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR}
};

/* 记录BBP 可维可测上报初始化的调试信息 */
BBP_DBG_ERR_INFO_STRU                   g_stBbpDbgErrInfo;

/* 记录BBP 可维可测上报的全局信息 */
BBP_DBG_INFO_STRU                       g_stBbpDbgInfo;

/* 记录BBP 数采的调试信息 */
BBP_DUMP_ERR_INFO_STRU                  g_stBbpDumpErrInfo;

/* 记录BBP数采的全局信息 */
BBP_DUMP_INFO_STRU                      g_stBbpDumpInfo;

/* 保留信息地址，用于在数采中断中发送消息 */
MsgBlock*                               g_pstBbpDumpMsg         = VOS_NULL_PTR;

/* 保留信息地址，用于在可维可测中断中发送消息 */
MsgBlock*                               g_pstBbpDbgMsg          = VOS_NULL_PTR;

/* 保存EDMA循环链表头指针 */
BALONG_DMA_CB                           *g_pstBbpCycBufHead       = VOS_NULL_PTR;

/*控制BBP可维可测的计数信号量*/
VOS_UINT32                              g_ulBbpDbgSem = VOS_NULL;

/*****************************************************************************
 3  函数声明
*****************************************************************************/
extern VOS_UINT32 Om_ConfigBbpDump(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId);

extern VOS_UINT32 Om_EndBbpDump(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId);

extern VOS_UINT32 Om_ConfigBbpDbg(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId);

extern VOS_UINT32 Om_SendEdmaErrToPc(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId);

extern VOS_UINT32 Om_QueryMsgFunc(OM_REQ_PACKET_STRU *pRspPacket,
                            OM_MSG_FUN_STRU *pstMsgFuncTable, VOS_UINT32 ulTotalNum);

/*****************************************************************************
 4 消息处理函数映射表
*****************************************************************************/
/*PC下发的 BBP采数与可维可测消息处理函数映射表 */
OM_MSG_FUN_STRU                         g_astOmBbpMsgFunTbl[]   =
{
    {APP_OM_BBP_DUMP_SET_REQ,      Om_ConfigBbpDump     ,    OM_APP_BBP_DUMP_SET_CNF },
    {APP_OM_BBP_DUMP_END_REQ ,     Om_EndBbpDump        ,    OM_APP_BBP_DUMP_END_CNF },
    {APP_OM_BBP_DBG_SET_REQ,       Om_ConfigBbpDbg      ,    OM_APP_BBP_DBG_SET_CNF  },
    {APP_OM_BBP_EDMA_ERR_REQ ,     Om_SendEdmaErrToPc   ,    OM_APP_BBP_EDMA_ERR_CNF },
};

/*****************************************************************************
  5 函数实现
*****************************************************************************/

VOS_UINT32 Om_CheckDumpLen(VOS_UINT32 ulLen)
{
    VOS_UINT32                          ulSizeTemp;

    /* 判断数采长度是否小于最小长度 */
    if (ulLen < BBP_DUMP_MIN_LEN)
    {
        return BBP_DUMP_MEM_SMALL;
    }

    /* 判断是否大于2G */
    if (ulLen > BBP_DUMP_MAX_LEN)
    {
        return BBP_DUMP_MEM_BIG;
    }

    /* 判断数采长度是否是32KB的整数倍 */
    ulSizeTemp = ulLen % BBP_DUMP_UNIT_LEN;
    if (0 != ulSizeTemp)
    {
        return BBP_DUMP_MEM_NOT_32KB;
    }

    return BBP_DUMP_OK;
}
VOS_UINT32 Om_SendEdmaErrToPc(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    OM_REQ_PACKET_STRU                 *pstPcMsg;
    BBP_EDMA_ERR_STRU                  *pstEdmaErr;

    pstPcMsg    = (OM_REQ_PACKET_STRU*)pstAppToOmMsg->aucPara;
    pstEdmaErr  = (BBP_EDMA_ERR_STRU*)pstPcMsg->aucCommand;
    OM_SendResult(pstAppToOmMsg->ucFuncType, pstEdmaErr->enEdmaErr, usReturnPrimId);

    return VOS_OK;
}



VOS_VOID Om_CloseBbpDump(VOS_VOID)
{
    /* 清启动数据采集 */
    OM_BBP_REG_SETBITS(WBBP_DUMP_START_W_ADDR, 0, 1, 0x0);

    /* 软件强制停止数据采集 */
    OM_BBP_REG_SETBITS(WBBP_DUMP_STOP_W_ADDR, 0, 1, 0x1);
}


VOS_UINT32 Om_DumpEdmaCallBack(VOS_UINT32 ulChannelArg, VOS_UINT32 ulFlag)
{
    OM_REQ_PACKET_STRU                 *pstPcMsg;
    BBP_EDMA_ERR_STRU                  *pstEdmaErr;

    LogPrint("Om_DumpEdmaCallBack: Start\r\n");

    /* 关闭数采 */
    Om_CloseBbpDump();

    if (VOS_NULL_PTR == g_pstBbpDumpMsg)
    {
        LogPrint("Om_DumpEdmaCallBack: g_pstBbpDumpMsg is NULL\r\n");
        return VOS_ERR;
    }

    pstPcMsg = (OM_REQ_PACKET_STRU*)g_pstBbpDumpMsg->aucValue;

    /* DMA传输完成中断 */
    if ( BALONG_DMA_INT_DONE == ulFlag)
    {
        g_stBbpDumpInfo.usDUMPDone++;
        /* 模拟工具发送消息通知 CCPU OM结束数采 */
        *(VOS_UINT16 *)pstPcMsg->aucCommand = (VOS_UINT16)APP_OM_BBP_DUMP_END_REQ;
        (VOS_VOID)VOS_SendMsg(UEPS_PID_OMRL, g_pstBbpDumpMsg);
    }
    else /* EDMA错误中断 */
    {
        pstEdmaErr = (BBP_EDMA_ERR_STRU*)pstPcMsg->aucCommand;

        pstEdmaErr->usMsgName           = (VOS_UINT16)APP_OM_BBP_EDMA_ERR_REQ;

        /* 标识是可维可测产生的EDMA错误中断 */
        pstEdmaErr->enEdmaErr           = OM_BBP_DUMP_EDMA_ERR;

        /* 模拟PC给CCPU OM发送消息 */
        (VOS_VOID)VOS_SendMsg(UEPS_PID_OMRL, g_pstBbpDumpMsg);
        g_stBbpDumpErrInfo.usEdmaErrCode = (VOS_UINT16)ulFlag;
    }

    g_stBbpDumpInfo.usDUMPStatus = OM_BBP_STOP;

    return VOS_OK;
}


VOS_UINT32 Om_BbpDumpEdmaInit(VOS_VOID)
{
    VOS_INT                             lRet;
    OM_REQ_PACKET_STRU                 *pstPcMsg;
    OM_BBP_DUMP_ENABLE_STRU             stEnableFlag;

    stEnableFlag.usEnable = OM_BBP_DUMP_DISABLE;

    /*读NV项*/
    if(NV_OK != NV_Read(en_NV_Item_BBP_DUMP_ENABLE,
        (VOS_VOID *)(&stEnableFlag), sizeof(OM_BBP_DUMP_ENABLE_STRU)))
    {
        LogPrint("Om_BbpDumpEdmaInit: Read Nv Fail\r\n");
        return VOS_ERR;
    }

    /* 判断是否启动BBP数采 */
    if (OM_BBP_DUMP_DISABLE == stEnableFlag.usEnable)
    {
        LogPrint("Om_BbpDumpEdmaInit: Not Enable BBP DUMP\r\n");
        return VOS_OK;
    }

    VOS_MemSet(&g_stBbpDumpInfo,    0, sizeof(g_stBbpDumpInfo));
    VOS_MemSet(&g_stBbpDumpErrInfo, 0, sizeof(g_stBbpDumpErrInfo));

    /* 调用DRV函数 初始化BBP DUMP使用的EDMA,并注册回调 */
    g_stBbpDumpInfo.lEdmaChanID = DRV_EDMA_CHANNEL_INIT(EDMA_BBP_GRIF,
                                    (channel_isr)Om_DumpEdmaCallBack,
                                    VOS_NULL, BBP_DUMP_EDMA_INT);
    if (g_stBbpDumpInfo.lEdmaChanID < 0)
    {
        g_stBbpDumpErrInfo.usInitEdmaErr++;
        return VOS_ERR;
    }

    /* 调用DRV函数配置 EDMA , BBP数采的burst长宽为16 * 32bit */
    lRet = DRV_EDMA_CHANNEL_CONFIG((VOS_UINT32)g_stBbpDumpInfo.lEdmaChanID, BALONG_DMA_P2M, EDMA_TRANS_WIDTH_32, EDMA_BUR_LEN_16);
    if (BSP_OK != lRet)
    {
        g_stBbpDumpErrInfo.usConfigEdmaErr++;
        return VOS_ERR;
    }

    /* 申请一条保留消息，用于中断中发送 */
    g_pstBbpDumpMsg = (MsgBlock*)VOS_AllocMsg(UEPS_PID_OMRL, sizeof(OM_REQ_PACKET_STRU));
    if (VOS_NULL_PTR == g_pstBbpDumpMsg)
    {
        g_stBbpDumpErrInfo.usAllocMsgErr++;
        return VOS_ERR;
    }
    pstPcMsg = (OM_REQ_PACKET_STRU*)g_pstBbpDumpMsg->aucValue;
    pstPcMsg->ucModuleId = OM_BBP_FUNC;
    g_pstBbpDumpMsg->ulReceiverPid = WUEPS_PID_OM;

    /* 标记该消息不用释放 */
    VOS_ReserveMsg(UEPS_PID_OMRL, g_pstBbpDumpMsg);

    LogPrint("Om_BbpDumpEdmaInit: ok!\r\n");

    return VOS_OK;

}


VOS_UINT32 Om_ConfigBbpDumpEdma(VOS_UINT32 ulLen,
                            VOS_UINT32* pulPhyAddr, OM_BBP_DUMP_MODE_ENUM_U32 enMode )
{
    BSP_DDR_SECT_INFO                   stSectInfo;
    BSP_DDR_SECT_QUERY                  stSectQuery;
    VOS_INT                             lRet;
    VOS_UINT32                          ulLenByte;

    stSectQuery.enSectType = BSP_DDR_SECT_TYPE_BBPSAMPLE;

    /* 调用DRV 接口获取内存地址的大小 */
    lRet = DRV_GET_FIX_DDR_ADDR(&stSectQuery, &stSectInfo);
    if(BSP_OK != lRet)
    {
        LogPrint("Om_ConfigBbpDumpEdma: Query Mem Err!\r\n");
        return BBP_DUMP_ALLOC_MEM_ERR;
    }

    *pulPhyAddr = stSectInfo.ulSectPhysAddr;

    ulLenByte = ulLen * 1024;
    if(stSectInfo.ulSectSize < ulLenByte)
    {
         LogPrint1("Om_ConfigBbpDumpEdma: Size Small %d!\r\n", (VOS_INT)stSectInfo.ulSectSize);
         return BBP_DUMP_MEM_BIG;
    }

    lRet = DRV_EDMA_CHANNEL_STOP((VOS_UINT32)g_stBbpDumpInfo.lEdmaChanID);
    if(BSP_ERROR == lRet)
    {
        g_stBbpDumpErrInfo.usStopEdmaErr++;
        LogPrint("Om_ConfigBbpDumpEdma:Stop Edma Fail\r\n");
        return BBP_DUMP_STOP_EDMA_ERR;
    }

    if (OM_BBP_DUMP_MODE_W == enMode)
    {
        /* 使能EDMA */
        lRet = DRV_EDMA_CHANNEL_ASYNC_START((VOS_UINT32)g_stBbpDumpInfo.lEdmaChanID, WBBP_DUMP_DMA_RDATA_W_ADDR,
                                            stSectInfo.ulSectPhysAddr, ulLenByte);
    }
    else
    {
        /* 使能EDMA */
        lRet = DRV_EDMA_CHANNEL_ASYNC_START((VOS_UINT32)g_stBbpDumpInfo.lEdmaChanID, OAM_GBBP_CPU_GDUMP_FIFO_RD_ADDR,
                                            stSectInfo.ulSectPhysAddr, ulLenByte);
    }

    if (BSP_OK != lRet)
    {
        g_stBbpDumpErrInfo.usStartEdmaErr++;
        LogPrint("Om_ConfigBbpDumpEdma:Start Edma Fail\r\n");
        return BBP_DUMP_START_EDMA_ERR;
    }

    return BBP_DUMP_OK;
}
VOS_UINT32 Om_CheckBbpDumpPara(APP_OM_BBP_DUMP_SET_STRU *pstPcMsg)
{
    VOS_UINT32                          ulResult;

    /* 判断下发的模式是否正确 */
    if (pstPcMsg->enMode >= OM_BBP_DUMP_MODE_BUTT)
    {
        return BBP_DUMP_MODE_ERR;
    }

        /* 检查长度是否符合要求 */
    ulResult = Om_CheckDumpLen(pstPcMsg->ulDataLen);
    if(ulResult != BBP_DUMP_OK)
    {
        LogPrint("Om_ConfigBbpDumpEdma: Lenth err!\r\n");
        return ulResult;
    }

    return BBP_DUMP_OK;
}


VOS_VOID Om_ConfigBbpDumpReg(APP_OM_BBP_DUMP_SET_STRU *pstPcMsg)
{
    VOS_UINT32                          ulSizeTemp;

    /* 配置BBP可维可测模式 */
    OM_BBP_REG_SETBITS(BBPCOMM_BBP_SYS_1CONTROL_ADDR, 0, 2, pstPcMsg->enMode);

    if (OM_BBP_DUMP_MODE_W == pstPcMsg->enMode)
    {
        /* 先强制关闭BBP数采 */
        OM_BBP_REG_SETBITS(WBBP_DUMP_STOP_W_ADDR, 0, 1, 0x1);

        /* 采数模块总使能，并控制时钟门控，无论哪种模式采数，首先要配置此信号有效 */
        OM_BBP_REG_SETBITS(WBBP_DUMP_EN_W_ADDR, 0, 1, 0x1);

        /* 开启BBP门控时钟寄存器 */
        OM_BBP_REG_SETBITS(WBBP_DUMP_CLKGATE_BYPASS_W_ADDR, 0, 3, 0x07);

        /*采数使能 */
        OM_BBP_WRITE_REG(WBBP_WT_DBG_EN_ADDR, pstPcMsg->ulWtDbgEnW);

        /* FIFO清零信号 */
        OM_BBP_REG_SETBITS(WBBP_DUMP_FIFO_CLR_W_ADDR, 0, 1, 0x1);

        /* PC侧下发的数采长度以KB为单位，但配置需要配置32bit的整数倍 */
        ulSizeTemp = (pstPcMsg->ulDataLen * 1024) / 4;

        /* 采集数据的长度，即需要采集的32bit数据个数 */
        OM_BBP_WRITE_REG(WBBP_DUMP_DATA_LENGTH_W_ADDR, ulSizeTemp);

        /* BBP 选择空口数据的采样位置 */
        OM_BBP_WRITE_REG(WBBP_DBG_AGC_DATA_SEL_ADDR, pstPcMsg->ulDataModelSelW);

        /* BBP 采数起始位置 */
        OM_BBP_WRITE_REG(WBBP_DUMP_START_TIME_W_ADDR, pstPcMsg->ulStartTimeW);

        /* 水线深度 */
        OM_BBP_REG_SETBITS(WBBP_DUMP_FIFO_WATERMARKER_W_ADDR, 0, 3, pstPcMsg->enFifoWaterW);

        /* 清数据采集停止寄存器 */
        OM_BBP_REG_SETBITS(WBBP_DUMP_STOP_W_ADDR, 0, 1, 0x0);

        /* 软件启动数据采集 */
        OM_BBP_REG_SETBITS(WBBP_DUMP_START_W_ADDR, 0, 1, 0x1);
    }
    else
    {
        /* 先强制关闭BBP数采 */
        OM_BBP_REG_SETBITS(OAM_GBBP_CPU_GDUMP_RECV_CONFIG_ADDR, 8, 1, 0x1);

        /* CPU配置异步FIFO清零信号 */
        OM_BBP_REG_SETBITS(OAM_GBBP_CPU_GDUMP_RECV_CONFIG_ADDR, 16, 1, 0x1);

        /* FIFO水线深度选择,采样数据源选择,采数模块配置类型选择 */
        OM_BBP_WRITE_REG(OAM_GBBP_CPU_GDUMP_GEN_CONFIG_ADDR, pstPcMsg->ulGenConfig);

        /* 选择使用CPU配置接口 */
        OM_BBP_REG_SETBITS(OAM_GBBP_CPU_GDUMP_GEN_CONFIG_ADDR, 8, 1, 0x1);

        /* 采数模块总使能 */
        OM_BBP_REG_SETBITS(OAM_GBBP_CPU_GDUMP_GEN_CONFIG_ADDR, 31, 1, 0x1);

        /* PC侧下发的数采长度以KB为单位，但配置需要配置32bit的整数倍 */
        ulSizeTemp = (pstPcMsg->ulDataLen * 1024) / 4;

        /* 采集数据的长度，即需要采集的32bit数据个数 */
        OM_BBP_WRITE_REG(OAM_GBBP_CPU_GDUMP_LEN_CONFIG_ADDR, ulSizeTemp);

        /* 配启动时刻采数模式下，采数起始位置 */
        OM_BBP_WRITE_REG(OAM_GBBP_CPU_GDUMP_SAM_LOW_TIMING_ADDR, pstPcMsg->ulStartLowTimeG);

        /* 可配启动时刻采数模式下，采数起始位置*/
        OM_BBP_WRITE_REG(OAM_GBBP_CPU_GDUMP_SAM_HIGH_TIMING_ADDR, pstPcMsg->ulStartHiTimeG);

        /* 启动G 模数采 */
        OM_BBP_REG_SETBITS(OAM_GBBP_CPU_GDUMP_RECV_CONFIG_ADDR, 0, 1, 0x1);
    }

    LogPrint("Om_ConfigBbpDumpReg: config BBP success\r\n");

}
VOS_UINT32 Om_ConfigBbpDump(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    APP_OM_BBP_DUMP_SET_STRU           *pstPcMsg;
    OM_APP_MSG_EX_STRU                 *pstOmToAppMsg;
    OM_APP_BBP_DUMP_EDMA_STRU          *pstResult;
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulLen;
    VOS_UINT32                          ulPhyAddr;
    VOS_UINT16                          usTotalLen;
    OM_BBP_DUMP_ENABLE_STRU             stEnableFlag;

    stEnableFlag.usEnable = OM_BBP_DUMP_DISABLE;

    /*读NV项*/
    if(NV_OK != NV_Read(en_NV_Item_BBP_DUMP_ENABLE,
        (VOS_VOID *)(&stEnableFlag), sizeof(OM_BBP_DUMP_ENABLE_STRU)))
    {
        LogPrint("Om_ConfigBbpDump: Read Nv Fail\r\n");
        OM_SendResult(pstAppToOmMsg->ucFuncType, BBP_DUMP_READ_NV_ERR, usReturnPrimId);
        return VOS_ERR;
    }

    usTotalLen = OM_APP_MSG_EX_LEN + sizeof(OM_APP_BBP_DUMP_EDMA_STRU);

    /* 分配消息空间 */
    pstOmToAppMsg   = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, usTotalLen);
    if(VOS_NULL_PTR == pstOmToAppMsg)
    {
        g_stBbpDumpErrInfo.usAllocMsgErr++;
        OM_SendResult(pstAppToOmMsg->ucFuncType, BBP_DUMP_ALLOC_MSG_ERR, usReturnPrimId);
        return VOS_ERR;
    }

    pstOmToAppMsg->usLength = usTotalLen - VOS_OM_HEADER_LEN;
    pstResult               = (OM_APP_BBP_DUMP_EDMA_STRU*)pstOmToAppMsg->aucPara;

    /* 判断是否使能BBP数采 */
    if (OM_BBP_DUMP_DISABLE == stEnableFlag.usEnable)
    {
        pstResult->enErr = BBP_DUMP_DISABLE;
        LogPrint("Om_ConfigBbpDump: Not Enable BBP DUMP\r\n");
        OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        return VOS_OK;
    }

    pstPcMsg    = (APP_OM_BBP_DUMP_SET_STRU*)pstAppToOmMsg->aucPara;

    /* 检查参数是否正确 */
    ulRet = Om_CheckBbpDumpPara(pstPcMsg);
    if (BBP_DUMP_OK != ulRet)
    {
        pstResult->enErr = ulRet;
        LogPrint1("Om_ConfigBbpDump:para err: %d!\r\n",(VOS_INT)ulRet);
        OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        return VOS_ERR;
    }

    ulLen       = pstPcMsg->ulDataLen;
    ulRet       = Om_ConfigBbpDumpEdma(ulLen, &ulPhyAddr, pstPcMsg->enMode);
    if(BBP_DUMP_OK != ulRet)
    {
        pstResult->enErr = ulRet;
        LogPrint("Om_ConfigBbpDump:Config Edma fail!\r\n");
        OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        return VOS_ERR;
    }

    /*配置BBP寄存器 */
    Om_ConfigBbpDumpReg(pstPcMsg);

    /* 置BBP 开始采数标志 */
    g_stBbpDumpInfo.usDUMPStatus = OM_BBP_START;
    g_stBbpDumpInfo.usDUMPMode   = (VOS_UINT16)pstPcMsg->enMode;
    pstResult->enErr             = BBP_DUMP_OK;
    pstResult->ulPhyAddr         = ulPhyAddr;

    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);

    return VOS_OK;

}


VOS_UINT32 Om_EndBbpDump(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    VOS_INT                             lRet;

    /* 调用DRV函数关闭EDMA,如果关闭EDMA失败，对全局没有影响，所以不用返回 */
    lRet = DRV_EDMA_CHANNEL_STOP((VOS_UINT32)g_stBbpDumpInfo.lEdmaChanID);
    if (BSP_ERROR == lRet)
    {
        LogPrint1("Om_EndBbpDump: Close EDMA: %d  fail\r\n", g_stBbpDumpInfo.lEdmaChanID);
        g_stBbpDumpErrInfo.usCloseEdmaErr++;
    }

    LogPrint("Om_EndBbpDump:Send Msg To PC\r\n");

    OM_SendResult(pstAppToOmMsg->ucFuncType, BBP_DUMP_END, usReturnPrimId);

    return VOS_OK;
}
VOS_VOID Om_EndBbpDbg(VOS_VOID)
{
    if ( OM_BBP_DBG_START == g_stBbpDbgInfo.ulBbpDbgStatus)
    {
        /* 清DBG打开信号 */
        OM_BBP_REG_SETBITS(WBBP_BBP_DBG_START_ADDR, 0, 1, 0x0);

        /* DBG停止信号 */
        OM_BBP_REG_SETBITS(WBBP_BBP_DBG_STOP_ADDR, 0, 1, 0x1);

        DRV_EDMA_CHANNEL_STOP((VOS_UINT32)g_stBbpDbgInfo.lBbpEdmaChanID);

        OM_BBP_READ_REG(g_stBbpDbgInfo.ulIndexValue,WBBP_DDR_BIT_CNT_ADDR);

        g_stBbpDbgInfo.ulBbpDbgStatus = OM_BBP_DBG_STOP;
    }

    return;
}


VOS_UINT32 Om_CheckBbpDbgPara(APP_OM_BBP_DBG_SET_W_STRU *pstPcMsg)
{
    /* 判断BBP接口选择信号是否正确 */
    if (pstPcMsg->enBBPIfSel >= BBP_IF_SEL_BUTT)
    {
        return BBP_DBG_IF_SEL_ERR;
    }

    /* 判断时钟选择信息是否正确 */
    if (pstPcMsg->enClkInSel >= BBP_DBG_CLK_IN_SEL_BUTT)
    {
        return BBP_DBG_CLK_IN_SEL_ERR;
    }

    /* 判断下发的模式是否正确 */
    if (pstPcMsg->enMode >= OM_BBP_DUMP_MODE_BUTT)
    {
        return BBP_DBG_MODE_ERR;
    }

    if ((pstPcMsg->ulMaskAddrEnd0 > MAX_BBP_OFFSET_ADDR )
        || (pstPcMsg->ulMaskAddrEnd1 > MAX_BBP_OFFSET_ADDR)
        || (pstPcMsg->ulMaskAddrEnd2 > MAX_BBP_OFFSET_ADDR)
        || (pstPcMsg->ulMaskAddrEnd3 > MAX_BBP_OFFSET_ADDR))
    {
        return BBP_DBG_ADDR_ERR;
    }

    /* 判断地址范围是否正确 */
    if ((pstPcMsg->ulMaskAddrStart0 > pstPcMsg->ulMaskAddrEnd0)
        || (pstPcMsg->ulMaskAddrStart1 > pstPcMsg->ulMaskAddrEnd1)
        || (pstPcMsg->ulMaskAddrStart2 > pstPcMsg->ulMaskAddrEnd2)
        || (pstPcMsg->ulMaskAddrStart3 > pstPcMsg->ulMaskAddrEnd3))
    {
        return BBP_DBG_ADDR_ERR;
    }

    return BBP_DBG_OK;
}


VOS_UINT32 Om_EnableDBGEdma(VOS_VOID)
{
    VOS_INT                             lRet;

    /* 关闭EDMA */
    lRet = DRV_EDMA_CHANNEL_STOP((VOS_UINT32)g_stBbpDbgInfo.lBbpEdmaChanID);
    if(BSP_ERROR == lRet)
    {
        g_stBbpDbgErrInfo.usStopEdmaErrNum++;;
        return BBP_DBG_STOP_EDMA_ERR;
    }

    /* 调用DRV 函数使能EDMA*/
    lRet = DRV_EDMA_CHANNEL_lli_ASYNC_START((VOS_UINT32)g_stBbpDbgInfo.lBbpEdmaChanID);
    if (BSP_OK != lRet)
    {
        g_stBbpDbgErrInfo.usStartEdmaErrNum++;
        return BBP_DBG_ENABLE_EDMA_ERR;
    }

    return BBP_DBG_OK;

}



VOS_UINT32 Om_ConfigBbpDbgReg(APP_OM_BBP_DBG_SET_W_STRU *pstPcMsg)
{
    VOS_UINT32                          ulValue;
    VOS_UINT32                          ulResult;

    /* 检查PC侧下发的配置信息是否正确 */
    ulResult = Om_CheckBbpDbgPara(pstPcMsg);
    if (BBP_DBG_OK != ulResult)
    {
        return ulResult;
    }

    /* DBG上报使能，1表示打开上报功能，0表示关闭上报功能。完成时钟门控 */
    OM_BBP_REG_SETBITS(WBBP_BBP_DBG_EN_ADDR, 0, 1, 0x1);

    /* 时钟bypass使能 */
    OM_BBP_REG_SETBITS(WBBP_BBP_DBG_BYPASS_ADDR, 0, 10, 0x3FF);

    /* 先强制关闭可维可测打开信号 */
    OM_BBP_REG_SETBITS(WBBP_BBP_DBG_STOP_ADDR, 0, 1, 0x1);

    /* 配置BBP可维可测模式 */
    OM_BBP_REG_SETBITS(BBPCOMM_BBP_SYS_1CONTROL_ADDR, 0, 2, pstPcMsg->enMode);

    /* 清FIFO */
    OM_BBP_REG_SETBITS(WBBP_DBG_FLOW_CLR_ADDR, 0, 9, 0x1FF);
    OM_BBP_REG_SETBITS(WBBP_DBG_FLOW_CLR_ADDR, 16, 1, 0x1);

    /* BBP接口选择信号 */
    OM_BBP_REG_SETBITS(WBBP_BBP_IF_SEL_ADDR, 0, 2, pstPcMsg->enBBPIfSel);

    /* DDR中DBG空间大小，2Kbyte为单位。如配置为8表示DBG空间为8个2Kbyte */
    OM_BBP_REG_SETBITS(WBBP_DBG_DDR_2K_NO_ADDR, 0, 4, 0x01);

    /* 可维可测输入时钟选择 */
    OM_BBP_REG_SETBITS(WBBP_DBG_CLK_IN_SEL_ADDR, 0, 2, pstPcMsg->enClkInSel);

    /* 软件配置的包头中reserve信息 */
    OM_BBP_REG_SETBITS(WBBP_DBG_HEAD_SERVE_ADDR, 0, 4, OM_TOOL_GUBBP_ID);

    /* 使用时间戳选择 */
    ulValue     = pstPcMsg->ulTimeSel & 0x7F;
    OM_BBP_REG_SETBITS(WBBP_DBG_TIMING_SEL_ADDR, 0, 6, ulValue);

    /* 屏蔽地址配置  */
    ulValue     = pstPcMsg->ulMaskAddrStart0 & 0xFFFFF;
    OM_BBP_REG_SETBITS(WBBP_MASK_ADDR0_START_ADDR, 0, 20, ulValue);

    ulValue     = pstPcMsg->ulMaskAddrStart1 & 0xFFFFF;
    OM_BBP_REG_SETBITS(WBBP_MASK_ADDR1_START_ADDR, 0, 20, ulValue);

    ulValue     = pstPcMsg->ulMaskAddrStart2 & 0xFFFFF;
    OM_BBP_REG_SETBITS(WBBP_MASK_ADDR2_START_ADDR, 0, 20, ulValue);

    ulValue     = pstPcMsg->ulMaskAddrStart3 & 0xFFFFF;
    OM_BBP_REG_SETBITS(WBBP_MASK_ADDR3_START_ADDR, 0, 20, ulValue);

    ulValue     = pstPcMsg->ulMaskAddrEnd0 & 0xFFFFF;
    OM_BBP_REG_SETBITS(WBBP_MASK_ADDR0_END_ADDR, 0, 20, ulValue);

    ulValue     = pstPcMsg->ulMaskAddrEnd1 & 0xFFFFF;
    OM_BBP_REG_SETBITS(WBBP_MASK_ADDR1_END_ADDR, 0, 20, ulValue);

    ulValue     = pstPcMsg->ulMaskAddrEnd2 & 0xFFFFF;
    OM_BBP_REG_SETBITS(WBBP_MASK_ADDR2_END_ADDR, 0, 20, ulValue);

    ulValue     = pstPcMsg->ulMaskAddrEnd3 & 0xFFFFF;
    OM_BBP_REG_SETBITS(WBBP_MASK_ADDR3_END_ADDR, 0, 20, ulValue);

    /* 可维可测 FIFO水线选择 */
    ulValue     = pstPcMsg->enFifoWater & 0x7;
    OM_BBP_REG_SETBITS(WBBP_DBG_FIFO_WATERMARKER_ADDR, 0, 3, ulValue);

    /* 可维可测上报信息开关，1表示上报相应信息，0表示不上报相应信息 */
    ulValue     = pstPcMsg->ulDbgSwitch & 0x1F;
    OM_BBP_REG_SETBITS(WBBP_BBP_DBG_SWITCH_ADDR, 0, 5, ulValue);

    /* 清可维可测停止信号 */
    OM_BBP_REG_SETBITS(WBBP_BBP_DBG_STOP_ADDR, 0, 1, 0x0);

    /* 可维可测打开信号 */
    OM_BBP_REG_SETBITS(WBBP_BBP_DBG_START_ADDR, 0, 1, 0x1);

    LogPrint("Om_ConfigBbpDbgReg: Config Reg success\r\n");

    return BBP_DBG_OK;
}


VOS_UINT32 Om_ConfigBbpDbg(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32                          ulRet;
    APP_OM_BBP_DBG_SET_W_STRU          *pstPcMsg;

    pstPcMsg        = (APP_OM_BBP_DBG_SET_W_STRU *)pstAppToOmMsg->aucPara;

    /* 检查初始化EDMA和SOCP是否成功 */
    if (BBP_DBG_OK != g_stBbpDbgInfo.ulBbpErrCode)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, g_stBbpDbgInfo.ulBbpErrCode, usReturnPrimId);
        return VOS_ERR;
    }

    /* 工具下发停止 BBP寄存器上报 */
    if(OM_BBP_STOP == pstPcMsg->enStatus)
    {
        Om_EndBbpDbg();
        OM_SendResult(pstAppToOmMsg->ucFuncType, BBP_DBG_OK, usReturnPrimId);
        return VOS_OK;
    }

    /* 使能 EDMA */
    ulRet = Om_EnableDBGEdma();
    if(BBP_DBG_OK != ulRet)
    {
        LogPrint1("Om_ConfigBbpDbg: Enable EDMA Fail %d\r\n", (VOS_INT)ulRet);
        OM_SendResult(pstAppToOmMsg->ucFuncType, ulRet, usReturnPrimId);
        return ulRet;
    }

    /* 配置BBP寄存器 */
    ulRet = Om_ConfigBbpDbgReg(pstPcMsg);
    if(BBP_DBG_OK != ulRet)
    {
        LogPrint1("Om_ConfigBbpDbg: Config Reg Fail %d\r\n", (VOS_INT)ulRet);
        OM_SendResult(pstAppToOmMsg->ucFuncType, ulRet, usReturnPrimId);
        return ulRet;
    }

    OM_SendResult(pstAppToOmMsg->ucFuncType, BBP_DBG_OK, usReturnPrimId);

    g_stBbpDbgInfo.ulBbpDbgStatus  = OM_BBP_DBG_START;

    return VOS_OK;

}


VOS_UINT32 Om_BbpDbgAllocCycBuf(VOS_VOID)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulVirBufAddr;
    VOS_UINT32                          ulPhyBufAddr;
    BALONG_DMA_CB                      *pstCycBuf;
    BALONG_DMA_CB                      *pstCycBufTemp;


    /* 申请空间 */
    ulVirBufAddr = (VOS_UINT32)VOS_UnCacheMemAlloc(BBP_DBG_TOTAL_MEM_LENTH, &ulPhyBufAddr);
    if (VOS_NULL == ulVirBufAddr)
    {
        LogPrint("Om_BbpDbgAllocMem: Alloc Mem err!\r\n");
        g_stBbpDbgErrInfo.usAllocErr++;
        g_stBbpDbgInfo.ulBbpErrCode = BBP_DBG_ALLOC_ERR;
        return VOS_ERR;
    }

    g_stBbpDbgInfo.ulPhyAddr = ulPhyBufAddr;

    /* 配置SOCP通道编码源通道的物理地址和虚地址 */
    for (i = 0; i < BBP_DBG_CODER_SRC_NUM; i++)
    {
        g_astBbpDbgCoderSrcCfg[i].pucSrcBuf = (VOS_UINT8*)ulVirBufAddr + BBP_DBG_CODER_SRC_SIZE * i;
        g_astBbpDbgCoderSrcCfg[i].pucSrcPHY = (VOS_UINT8*)ulPhyBufAddr + BBP_DBG_CODER_SRC_SIZE * i;
    }


    /* 为保证256bit对齐，多分配32 byte */
    pstCycBuf = (BALONG_DMA_CB*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT,
                        (BBP_DBG_CODER_SRC_NUM * BBP_DBG_NODE_NUM * sizeof(BALONG_DMA_CB) + 32));
    if (VOS_NULL_PTR == pstCycBuf)
    {
        g_stBbpDbgErrInfo.usAllocCycErr++;
        g_stBbpDbgInfo.ulBbpErrCode = BBP_DBG_ALLOC_CYC_ERR;
        LogPrint("Om_BbpDbgAllocCycBuf: alloc BALONG_DMA_CB fail\r\n");
        return VOS_ERR;
    }

    /* 256 bit对齐 */
    pstCycBuf =(BALONG_DMA_CB*)(((VOS_UINT32)pstCycBuf + 31)  & ~ 31);

    pstCycBufTemp = (BALONG_DMA_CB*)pstCycBuf;
    g_pstBbpCycBufHead = pstCycBuf;

    /* 构成循环链表 */
    for (i = 0; i < BBP_DBG_CODER_SRC_NUM * BBP_DBG_NODE_NUM; i++)
    {
        pstCycBufTemp->cnt0     = BBP_DBG_NODE_SIZE;
        pstCycBufTemp->cnt1     = 0;
        pstCycBufTemp->bindx    = 0;
        pstCycBufTemp->cindx    = 0;
        pstCycBufTemp->src_addr = (VOS_UINT32)WBBP_BBP_DBG_RDATA_ADDR;
        pstCycBufTemp->des_addr = ulPhyBufAddr
                                    + BBP_DBG_NODE_SIZE
                                    * ((i%BBP_DBG_CODER_SRC_NUM) * BBP_DBG_NODE_NUM
                                    + (i / BBP_DBG_CODER_SRC_NUM));
        pstCycBufTemp->config   = BALONG_DMA_SET_CONFIG(EDMA_BBP_DBG, BALONG_DMA_P2M, EDMA_TRANS_WIDTH_32, EDMA_BUR_LEN_16);
        if(0 == pstCycBufTemp->config)
        {
            LogPrint("Om_BbpDbgAllocCycBuf: Config EDMA fail\r\n");
            g_stBbpDbgErrInfo.usConfigEdmaErr++;
            g_stBbpDbgInfo.ulBbpErrCode = BBP_DBG_CONFIG_EDMA_ERR;
            return VOS_ERR;
        }

        /* 如果是最后一个结点，退出 */
        if ((BBP_DBG_CODER_SRC_NUM * BBP_DBG_NODE_NUM - 1) == i)
        {
            break;
        }
        pstCycBufTemp->lli = (((VOS_UINT32)(pstCycBufTemp + 1)) & 0xFFFFFFE0) | 0x02;
        pstCycBufTemp++;
    }

    pstCycBufTemp->lli = (((VOS_UINT32)g_pstBbpCycBufHead) & 0xFFFFFFE0) | 0x02;

    /* 第一个结点，需将config的bit0 置为0 */
    pstCycBufTemp = g_pstBbpCycBufHead;
    pstCycBufTemp->config &= 0xFFFFFFFE;

    return VOS_OK;

}
VOS_UINT32 Om_BbpDbgEdmaCallBack(VOS_UINT32 ulChannelArg, VOS_UINT32 ulFlag)
{
    OM_REQ_PACKET_STRU                 *pstPcMsg;
    BBP_EDMA_ERR_STRU                  *pstEdmaErr;
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulDbgSwitchValue;

    /* DMA传输完成中断 */
    if (BALONG_DMA_INT_LLT_DONE == ulFlag)
    {
        ulIndex = g_stBbpDbgInfo.ulBbpEdmaCallBackNum% 8;
        g_stBbpDbgInfo.ulLastAddr = (VOS_UINT32)(g_stBbpDbgInfo.ulPhyAddr
                + BBP_DBG_NODE_SIZE * ((ulIndex % BBP_DBG_CODER_SRC_NUM) * BBP_DBG_NODE_NUM
                + (ulIndex / BBP_DBG_CODER_SRC_NUM)));
        if(BBP_DBG_MAGIC_SIGN != *((VOS_UINT32*)g_stBbpDbgInfo.ulLastAddr))
        {
            *((VOS_UINT32*)g_stBbpDbgInfo.ulLastAddr) = (VOS_UINT32)BBP_DBG_MAGIC_SIGN;
            *(((VOS_UINT32*)g_stBbpDbgInfo.ulLastAddr) + 1) = (VOS_UINT32)BBP_DBG_PACKAGE_LENTH;
            *(((VOS_UINT32*)g_stBbpDbgInfo.ulLastAddr) + 2) = (VOS_UINT32)BBP_DBG_ERR_PACKAGE_SIGN;

            /* 停止可给可维可测 */
            OM_BBP_REG_SETBITS(WBBP_BBP_DBG_STOP_ADDR, 0, 1, 0x1);
            OM_BBP_READ_REG(ulDbgSwitchValue,WBBP_BBP_DBG_SWITCH_ADDR);

            /* 清FIFO */
            OM_BBP_REG_SETBITS(WBBP_DBG_FLOW_CLR_ADDR, 0, 9, 0x1FF);
            OM_BBP_REG_SETBITS(WBBP_DBG_FLOW_CLR_ADDR, 16, 1, 0x1);

            /* BBP需要一个电平转换才能清掉FIFO */
            OM_BBP_WRITE_REG(WBBP_BBP_DBG_SWITCH_ADDR,ulDbgSwitchValue);

            /* 查询BBP总包数 */
            OM_BBP_READ_REG(g_stBbpDbgInfo.ulIndexValue,WBBP_DDR_BIT_CNT_ADDR);

            /* 重新启动EDMA */
            Om_EnableDBGEdma();

            /* 重新开启可维可测 */
            OM_BBP_REG_SETBITS(WBBP_BBP_DBG_START_ADDR, 0, 1, 0x1);

            g_stBbpDbgInfo.ulFlowNum++;

        }

        VOS_SmV(g_ulBbpDbgSem);
        g_stBbpDbgInfo.ulBbpEdmaCallBackNum++;
    }
    /* EDMA错误中断 */
    else
    {
        if (VOS_NULL_PTR == g_pstBbpDbgMsg)
        {
            return VOS_ERR;
        }

        pstPcMsg = (OM_REQ_PACKET_STRU*)g_pstBbpDbgMsg->aucValue;
        pstEdmaErr = (BBP_EDMA_ERR_STRU*)pstPcMsg->aucCommand;

        pstPcMsg->ucModuleId  = OM_BBP_FUNC;
        pstEdmaErr->usMsgName = (VOS_UINT16)APP_OM_BBP_EDMA_ERR_REQ;

        /* 标识是可维可测产生的EDMA错误中断 */
        pstEdmaErr->enEdmaErr = OM_BBP_DBG_EDMA_ERR;

        /* 模拟PC给CCPU OM发送消息 */
        (VOS_VOID)VOS_SendMsg(UEPS_PID_OMRL, g_pstBbpDbgMsg);

        g_stBbpDbgErrInfo.usEdmaErrCode = (VOS_UINT16)ulFlag;
    }

    return VOS_OK;

}



VOS_VOID Om_BbpDbgChanInit(VOS_VOID)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulRet;
    OM_REQ_PACKET_STRU                 *pstPcMsg;
    SOCP_CODER_SRC_CHAN_S               stChannel;   /* 通道的属性信息 */
    BALONG_DMA_CB                      *pstConfigAddr;

    /* 初始化全局变量 */
    VOS_MemSet(&g_stBbpDbgErrInfo,  0, sizeof(g_stBbpDbgErrInfo));
    VOS_MemSet(&g_stBbpDbgInfo,     0, sizeof(g_stBbpDbgInfo));

    /* 置可维可测状态标志 */
    g_stBbpDbgInfo.ulBbpDbgStatus = OM_BBP_DBG_STOP;

    /*初始化BBP 可维可测使用的EDMA通道 */
    g_stBbpDbgInfo.lBbpEdmaChanID = DRV_EDMA_CHANNEL_INIT(EDMA_BBP_DBG,
                                    (channel_isr)Om_BbpDbgEdmaCallBack,
                                    VOS_NULL, BBP_DBG_EDMA_INT);
    if (g_stBbpDbgInfo.lBbpEdmaChanID < BSP_OK)
    {
        g_stBbpDbgErrInfo.usInitEdmaErr++;
        g_stBbpDbgInfo.ulBbpErrCode = BBP_DBG_INIT_EDMA_ERR;
        return;
    }

    /* 申请空间，形成循环链表 */
    ulRet =  Om_BbpDbgAllocCycBuf();
    if (VOS_ERR == ulRet)
    {
        LogPrint("Om_BbpDbgChanInit: Alloc Mem Error\r\n");
        return ;
    }

    /* 获取可维可测EDMA通道循环链表配置地址 */
    pstConfigAddr = DRV_EDMA_CHAN_GET_LLI_ADDR((VOS_UINT32)g_stBbpDbgInfo.lBbpEdmaChanID);
    if (VOS_NULL_PTR == pstConfigAddr)
    {
        g_stBbpDbgErrInfo.usGetConAddrErr++;
        g_stBbpDbgInfo.ulBbpErrCode = BBP_DBG_GET_CONFIGADD_ERR;
        LogPrint("Om_BbpDbgChanInit: get lli config add fail\r\n");
        return ;
    }

    /* 将配置信息写入EDMA配置地址 */
    VOS_MemCpy(pstConfigAddr, g_pstBbpCycBufHead, sizeof(BALONG_DMA_CB));

    for (i = 0; i < BBP_DBG_CODER_SRC_NUM; i++)
    {
        VOS_MemSet(&stChannel, 0, sizeof(stChannel));
        stChannel.u32DestChanID = (BSP_U32)g_astBbpDbgCoderSrcCfg[i].enDstCHID;   /*  目标通道ID */
        stChannel.eDataType     = g_astBbpDbgCoderSrcCfg[i].enDataType;           /*  数据类型，指明数据封装协议，用于复用多平台 */
        stChannel.eMode         = g_astBbpDbgCoderSrcCfg[i].enCHMode;             /*  通道数据模式 */
        stChannel.ePriority     = g_astBbpDbgCoderSrcCfg[i].enCHLevel;            /*  通道优先级 */
        stChannel.u32BypassEn   = SOCP_HDLC_ENABLE;                               /*  通道bypass使能 */
        stChannel.eDataTypeEn   = SOCP_DATA_TYPE_EN;                              /*  数据类型使能位 */
        stChannel.eDebugEn      = SOCP_ENC_DEBUG_DIS;                             /*  调试位使能 */

        stChannel.sCoderSetSrcBuf.u32InputStart  = (VOS_UINT32)g_astBbpDbgCoderSrcCfg[i].pucSrcPHY;                               /*  输入通道起始地址 */
        stChannel.sCoderSetSrcBuf.u32InputEnd    = (VOS_UINT32)((g_astBbpDbgCoderSrcCfg[i].pucSrcPHY + g_astBbpDbgCoderSrcCfg[i].ulSrcBufLen)-1);   /*  输入通道结束地址 */

        /* 配置通道 */
        if (VOS_OK != BSP_SOCP_CoderSetSrcChan(g_astBbpDbgCoderSrcCfg[i].enChannelID, &stChannel))
        {
            g_astBbpDbgCoderSrcCfg[i].enInitState = SCM_CHANNEL_CFG_FAIL;/* 记录通道开启配置错误 */
            g_stBbpDbgErrInfo.usInitSocpErr++;
            g_stBbpDbgInfo.ulBbpErrCode = BBP_DBG_INIT_SOCP_ERR;

            return;
        }

        /* 使能通道 */
        if(VOS_OK != BSP_SOCP_Start(g_astBbpDbgCoderSrcCfg[i].enChannelID))
        {
            g_astBbpDbgCoderSrcCfg[i].enInitState = SCM_CHANNEL_START_FAIL;  /* 记录通道开启错误 */
            g_stBbpDbgErrInfo.usStartSocpErr++;
            g_stBbpDbgInfo.ulBbpErrCode = BBP_DBG_START_SOCP_ERR;

            return;
        }

        g_astBbpDbgCoderSrcCfg[i].enInitState = SCM_CHANNEL_INIT_SUCC; /* 记录通道初始化配置信息 */
    }

    /*创建信号量*/
    if ( VOS_OK != VOS_SmCCreate("BbpD", 0, VOS_SEMA4_FIFO, &g_ulBbpDbgSem))
    {
        g_stBbpDbgErrInfo.usCreateSemErr++;
        g_stBbpDbgInfo.ulBbpErrCode = BBP_DBG_CREATE_SEM_ERR;
        return;
    }

    /* 申请一条保留消息，用于中断中发送 */
    g_pstBbpDbgMsg = (MsgBlock*)VOS_AllocMsg(UEPS_PID_OMRL, sizeof(OM_REQ_PACKET_STRU));
    if (VOS_NULL_PTR == g_pstBbpDbgMsg)
    {
        g_stBbpDumpErrInfo.usAllocMsgErr++;
        g_stBbpDbgInfo.ulBbpErrCode = BBP_DBG_ALLOC_MSG_ERR;
        return;
    }

    pstPcMsg = (OM_REQ_PACKET_STRU*)g_pstBbpDbgMsg->aucValue;

    pstPcMsg->ucModuleId = OM_BBP_FUNC;

    g_pstBbpDbgMsg->ulReceiverPid = WUEPS_PID_OM;

    /* 标记该消息不用释放 */
    VOS_ReserveMsg(UEPS_PID_OMRL,g_pstBbpDbgMsg);
    return ;
}


VOS_VOID Om_BbpDbgSelfTask(unsigned long ulPara1, unsigned long ulPara2,
                  unsigned long ulPara3, unsigned long ulPara4)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulRet;
    SOCP_BUFFER_RW_S                    stRwBuf;

#if (VOS_WIN32 == VOS_OS_VER)
    VOS_UINT32                          i;
    for (i = 0;i < 1; i++)
#else

    for (;;)
#endif
    {
        if(VOS_OK != VOS_SmP(g_ulBbpDbgSem, 0))
        {
            continue;
        }

        /* 计算这次中断应该处理的通道 */
        ulIndex = g_stBbpDbgInfo.ulBbpSelfTaskNum & (BBP_DBG_CODER_SRC_NUM -1);

        /* 获取通道的编码源空间 */
        if (VOS_OK != BSP_SOCP_GetWriteBuff(g_astBbpDbgCoderSrcCfg[ulIndex].enChannelID, &stRwBuf))
        {
            g_stBbpDbgErrInfo.astBbpDbgSocpInfo[ulIndex].ulGetWriteBuffErr++;
            continue;
        }

        /* 判断空间是否足够 */
        if ((stRwBuf.u32RbSize + stRwBuf.u32Size) < BBP_DBG_NODE_SIZE)
        {
            g_stBbpDbgErrInfo.astBbpDbgSocpInfo[ulIndex].ulBuffNotEnough++;
            g_stBbpDbgErrInfo.astBbpDbgSocpInfo[ulIndex].ulWriteBufSize = stRwBuf.u32RbSize + stRwBuf.u32Size;
            continue;
        }

        ulRet = BSP_SOCP_WriteDone(g_astBbpDbgCoderSrcCfg[ulIndex].enChannelID, BBP_DBG_NODE_SIZE );
        if (VOS_OK != ulRet)
        {
            g_stBbpDbgErrInfo.astBbpDbgSocpInfo[ulIndex].ulWriteDoneErr++;

            continue;
        }

        g_stBbpDbgErrInfo.astBbpDbgSocpInfo[ulIndex].ulWriteDoneOk++;

        /* 回调次数加 1 */
        g_stBbpDbgInfo.ulBbpSelfTaskNum++;

    }
}


VOS_VOID Om_CcpuBbpConfigMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr)
{
    APP_OM_MSG_EX_STRU                 *pstAppToOmMsg;
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulTotalNum;
    VOS_UINT16                          usPrimId;
    VOS_UINT16                          usReturnPrimId;
    VOS_UINT32                          ulResult = VOS_ERR;

    pstAppToOmMsg   = (APP_OM_MSG_EX_STRU*)pRspPacket;
    ulTotalNum      = sizeof(g_astOmBbpMsgFunTbl) / sizeof(OM_MSG_FUN_STRU);
    usPrimId = pstAppToOmMsg->usPrimId;
    /* 查询处理函数 */
    for (ulIndex = 0; ulIndex < ulTotalNum; ulIndex++)
    {
        if (usPrimId == (VOS_UINT16)(g_astOmBbpMsgFunTbl[ulIndex].ulPrimId))
        {
            usReturnPrimId = (VOS_UINT16)(g_astOmBbpMsgFunTbl[ulIndex].ulReturnPrimId);
            ulResult = g_astOmBbpMsgFunTbl[ulIndex].pfFun(pstAppToOmMsg, usReturnPrimId);
            break;
        }
    }

    /* 没有查询到处理函数  */
    if (ulIndex == ulTotalNum)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usPrimId);
    }
    return;

}


VOS_VOID Om_BbpDbgShow(VOS_VOID)
{
    vos_printf("可维可测EDMA通道号:             %d\r\n",g_stBbpDbgInfo.lBbpEdmaChanID);
    vos_printf("BBP可维可测EDMA 完成回调次数:   %d\r\n",g_stBbpDbgInfo.ulBbpEdmaCallBackNum);
    vos_printf("自处理任务处理的次数:           %d\r\n", g_stBbpDbgInfo.ulBbpSelfTaskNum);
    vos_printf("BBP可维可测初始化错误码:        %d\r\n",g_stBbpDbgInfo.ulBbpErrCode);
    vos_printf("溢出包数:                       0x%x\r\n", g_stBbpDbgInfo.ulFlowNum);
    vos_printf("分配内存的物理地址 :            0x%x\r\n",g_stBbpDbgInfo.ulPhyAddr);
    vos_printf("BBP中发出的2K数据的数量:        0x%x\r\n",g_stBbpDbgInfo.ulIndexValue);
    vos_printf("前一次数采地址 g_ulUpAddr:      0x%x\r\n", g_stBbpDbgInfo.ulLastAddr);
}


VOS_VOID Om_BbpDbgErrShow(VOS_VOID)
{
    VOS_UINT32                          i;
    vos_printf("内存分配失败次数:       %d \r\n",g_stBbpDbgErrInfo.usAllocErr);
    vos_printf("SOCP初始化失败次数:     %d \r\n",g_stBbpDbgErrInfo.usInitSocpErr);
    vos_printf("启动SOCP失败次数:       %d \r\n",g_stBbpDbgErrInfo.usStartSocpErr);
    vos_printf("初始化EDMA失败次数:     %d \r\n",g_stBbpDbgErrInfo.usInitEdmaErr);
    vos_printf("配置EDMA失败次数:       %d \r\n",g_stBbpDbgErrInfo.usConfigEdmaErr);
    vos_printf("EDMA失败回调次数:       %d \r\n",g_stBbpDbgErrInfo.usEdmaErrCode);
    vos_printf("启动EDMA失败的次数:     %d \r\n",g_stBbpDbgErrInfo.usStartEdmaErrNum);
    vos_printf("关闭EDMA失败的次数:     %d \r\n",g_stBbpDbgErrInfo.usStopEdmaErrNum);
    vos_printf("分配循环链表节点失败:   %d \r\n",g_stBbpDbgErrInfo.usAllocCycErr);
    vos_printf("获取EDMA配置址失败:     %d \r\n",g_stBbpDbgErrInfo.usGetConAddrErr);
    vos_printf("创建信号量失败:         %d \r\n",g_stBbpDbgErrInfo.usCreateSemErr);
    for (i = 0;i < BBP_DBG_CODER_SRC_NUM; i++)
    {
        vos_printf("通道[%d]编码源空间不够的次数:    %d\r\n",i,g_stBbpDbgErrInfo.astBbpDbgSocpInfo[i].ulBuffNotEnough);
        vos_printf("通道[%d]编码源空间大小:          %d\r\n",i,g_stBbpDbgErrInfo.astBbpDbgSocpInfo[i].ulWriteBufSize);
        vos_printf("通道[%d]获取写buffer失败的次数:  %d\r\n",i,g_stBbpDbgErrInfo.astBbpDbgSocpInfo[i].ulGetWriteBuffErr);
        vos_printf("通道[%d]WriteDone失败次数:       %d\r\n",i,g_stBbpDbgErrInfo.astBbpDbgSocpInfo[i].ulWriteDoneErr);
        vos_printf("通道[%d]WriteDone成功次数:       %d\r\n",i,g_stBbpDbgErrInfo.astBbpDbgSocpInfo[i].ulWriteDoneOk);
    }
}


VOS_VOID Om_BbpDumpShow(VOS_VOID)
{
    vos_printf("分配消息空间失败:   %d\r\n", g_stBbpDumpErrInfo.usAllocMsgErr);
    vos_printf("关闭EDMA失败:       %d\r\n", g_stBbpDumpErrInfo.usCloseEdmaErr);
    vos_printf("配置EDMA失败:       %d\r\n", g_stBbpDumpErrInfo.usConfigEdmaErr);
    vos_printf("错误中断类型 :      %d\r\n", g_stBbpDumpErrInfo.usEdmaErrCode);
    vos_printf("初始化EDMA失败 :    %d\r\n", g_stBbpDumpErrInfo.usInitEdmaErr);
    vos_printf("启动EDMA失败:       %d\r\n", g_stBbpDumpErrInfo.usStartEdmaErr);
    vos_printf("停止EDMA失败:       %d\r\n", g_stBbpDumpErrInfo.usStopEdmaErr);

    vos_printf("数采EDMA通道号:     %d\r\n", g_stBbpDumpInfo.lEdmaChanID);
    vos_printf("数采状态:           %d\r\n", g_stBbpDumpInfo.usDUMPStatus);
    vos_printf("数采EDMA完成回调:   %d\r\n", g_stBbpDumpInfo.usDUMPDone);
    vos_printf("数采模式:           %d\r\n", g_stBbpDumpInfo.usDUMPMode);
}


VOS_VOID Om_BbpDbgCycShow(VOS_VOID)
{
    VOS_UINT32                          i;
    BALONG_DMA_CB                      *pstCycBufTemp;

    pstCycBufTemp = g_pstBbpCycBufHead;

    /* 构成循环链表 */
    for (i = 0; i < BBP_DBG_CODER_SRC_NUM * BBP_DBG_NODE_NUM; i++)
    {
        vos_printf("Node[%d]lli :       0x%x\r\n",i,pstCycBufTemp->lli);
        vos_printf("Node[%d]bindx :     0x%x\r\n",i,pstCycBufTemp->bindx);
        vos_printf("Node[%d]cindx :     0x%x\r\n",i,pstCycBufTemp->cindx);
        vos_printf("Node[%d]cnt1 :      0x%x\r\n",i,pstCycBufTemp->cnt1);
        vos_printf("Node[%d]cnt0 :      0x%x\r\n",i,pstCycBufTemp->cnt0);
        vos_printf("Node[%d]src_addr :  0x%x\r\n",i,pstCycBufTemp->src_addr);
        vos_printf("Node[%d]des_addr :  0x%x\r\n",i,pstCycBufTemp->des_addr);
        vos_printf("Node[%d]config :    0x%x\r\n",i,pstCycBufTemp->config);
        pstCycBufTemp++;
    }

}
VOS_VOID Om_BbpBaseAddrShow(VOS_VOID)
{
    vos_printf("GBBP_BASE_ADDR:          0x%x\r\n",GBBP_BASE_ADDR);
    vos_printf("WBBP_BASE_ADDR:          0x%x\r\n",WBBP_BASE_ADDR);
    vos_printf("SOC_BBP_COMM_BASE_ADDR:  0x%x\r\n",SOC_BBP_COMM_BASE_ADDR);
    vos_printf("SOC_BBP_WCDMA_BASE_ADDR: 0x%x\r\n",SOC_BBP_WCDMA_BASE_ADDR);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

