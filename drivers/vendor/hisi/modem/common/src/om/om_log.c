/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : om_log.c
  版 本 号   : 初稿
  作    者   : 苏庄銮 59026
  生成日期   : 2011年5月31日
  最近修改   :
  功能描述   : 可维可测-日志上报及日志记录功能实现
  函数列表   :
              OM_LOG_DefauleCfg
              OM_LOG_Init
              OM_LOG_InnerRecord
              OM_LOG_IsrSoftIntr
              OM_LOG_LogReport
              OM_LOG_MsgSetLogReq
  修改历史   :
  1.日    期   : 2011年5月31日
    作    者   : 苏庄銮 59026
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "om.h"
#include "med_drv_timer_hifi.h"
#include "ucom_comm.h"
#include "ucom_low_power.h"

#ifdef _OM_UART_PRINT
#include "stdlib.h"
#include "stdio.h"
#include "med_drv_uart.h"
#endif

#ifdef _OM_STD_PRINT
#include "stdlib.h"
#include "stdio.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_OM_LOG_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* OM模块初始化标志 */
VOS_UINT32              g_uwOmLogInitFlag = 0;

/* 日志控制全局变量 */
OM_LOG_CTRL_STRU        g_stOmLogCtrl;

/* 内部打点记录, 位于TCM */
OM_LOG_INNER_OBJ_STRU   g_stOmLogInnerRec;

/* 最后一条错误记录 */
OM_LOG_ITEM_STRU        g_stOmLogLastErrRec;

/* APR日志记录 */
OM_LOG_APR_OBJ_STRU     g_stOmAprLog;

/* log级别到报告消息ID映射表 */
VOS_UINT16              g_auhwOmLogLevelToMsgIdTable[] =
{
    ID_CODEC_OM_LOG_INFO_IND,
    ID_CODEC_OM_LOG_WARNING_IND,
    ID_CODEC_OM_LOG_ERR_IND
};

/* 引用中断封装程序的定义，内部存储有软中断信息 */
extern VOS_UINT32       g_uwSoftIntInfo;

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : OM_LOG_Init
 功能描述  : 可维可测日志功能初始化
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月3日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_LOG_Init(VOS_VOID)
{
    /* 初始化全局变量空间 */
    UCOM_MemSet(OM_LOG_GetCtrlObjPtr(), 0, sizeof(OM_LOG_CTRL_STRU));
    UCOM_MemSet(OM_LOG_GetInnerObjPtr(), 0, sizeof(OM_LOG_INNER_OBJ_STRU));
    UCOM_MemSet(OM_LOG_GetAprLogPtr(), 0, sizeof(OM_LOG_APR_OBJ_STRU));

    /* 注册软中断用于记录致命错误 */
    VOS_ConnectInterrupt(OS_INTR_CONNECT_01, OM_LOG_IsrSoftIntr);
    VOS_EnableInterrupt(OS_INTR_CONNECT_01);

    /* 标志为已初始化 */
    OM_LOG_SetInit();
}

/*****************************************************************************
 函 数 名  : OM_LOG_DefaultCfg
 功能描述  : 通过NV项设置log上报参数
 输入参数  : CODEC_LOG_HOOK_CONFIG_STRU *
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月21日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_LOG_DefaultCfg(CODEC_LOG_HOOK_CONFIG_STRU *pstLogHookCfg)
{
    /* 判断NV项设置的值的合法性 */
    if (   (pstLogHookCfg->uhwEnable >= OM_SWITCH_BUTT)
        || (pstLogHookCfg->uhwLevel >= OM_LOG_LEVEL_BUTT))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* 根据NV项进行相应的修改 */
    OM_LOG_SetEnable(pstLogHookCfg->uhwEnable);
    OM_LOG_SetLevel(pstLogHookCfg->uhwLevel);

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : OM_LOG_InnerRecord
 功能描述  : 添加日志到内部记录
 输入参数  : OM_LOG_ID_ENUM_UINT32 enLogId - log id
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_LOG_InnerRecord(
                OM_LOG_ID_ENUM_UINT32   enLogId,
                VOS_UINT16              uhwFileID,
                VOS_UINT16              uhwLineID)
{
    OM_LOG_INNER_OBJ_STRU      *pstInnerObj = OM_LOG_GetInnerObjPtr();
    OM_LOG_INNER_RECORD_STRU   *pstRecord   = VOS_NULL;
    VOS_UINT32                  uwIndx;
    VOS_CPU_SR                  srSave;

    /* 未初始化则直接退出 */
    if (!OM_LOG_CheckInit())
    {
        return;
    }

    /* 锁中断 */
    srSave = VOS_SplIMP();

    /* 写入记录 */
    uwIndx                  = pstInnerObj->uwCurrIdx;
    pstRecord               = OM_LOG_GetInnerRecord(uwIndx);
    pstRecord->enLogId      = enLogId;
    pstRecord->uwTimeStamp  = DRV_TIMER_ReadSysTimeStamp();
    pstRecord->uhwFileID    = uhwFileID;
    pstRecord->uhwLineID    = uhwLineID;
    pstInnerObj->uwCurrIdx  = (uwIndx + 1) % OM_LOG_INNER_MAX_NUM;

    /* 释放中断 */
    VOS_Splx(srSave);

}

#ifndef VOS_VENUS_TEST_STUB
/*****************************************************************************
 函 数 名  : OM_LOG_LogReport
 功能描述  : 上报调试日志
 输入参数  : OM_LOG_RPT_LEVEL_ENUM_UINT16        enLevel    - 日志级别
             OM_FILE_ID_ENUM_UINT16              enFileNo   - 日志文件号
             VOS_UINT16                          uhwLineNo  - 日志行号
             OM_LOG_ID_ENUM_UINT32               enLogId    - 日志编号
             VOS_UINT16                          uhwLogFreq - 日志频度
             VOS_INT32                           swValue1   - 附带值1
             VOS_INT32                           swValue2   - 附带值2
             VOS_INT32                           swValue3   - 附带值3
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_LOG_LogReport(
                OM_LOG_RPT_LEVEL_ENUM_UINT16        enLevel,
                OM_FILE_ID_ENUM_UINT16              enFileNo,
                VOS_UINT16                          uhwLineNo,
                OM_LOG_ID_ENUM_UINT32               enLogId,
                VOS_UINT16                          uhwLogFreq,
                VOS_INT32                           swValue1,
                VOS_INT32                           swValue2,
                VOS_INT32                           swValue3)
{
    CODEC_OM_LOG_IND_STRU               stLogMsg;
    VOS_UINT32                          uwCntIndx;
    VOS_UINT16                          uhwCycleCnt;
    OM_LOG_ITEM_STRU                   *pstLastErr;

    UCOM_MemSet(&stLogMsg, 0, sizeof(stLogMsg));

    if (!OM_LOG_CheckInit())
    {
        return UCOM_RET_SUCC;
    }

    /* 记录日志信息到内部TCM, 供复位调试 */
    OM_LOG_InnerRecord(enLogId, enFileNo, uhwLineNo);

    /* 最后一条错误记录中记录信息 */
    if(OM_LOG_LEVEL_ERROR == enLevel)
    {
        pstLastErr              = OM_LOG_GetLastErrRec();

        pstLastErr->enLogId     = enLogId;
        pstLastErr->uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();
        pstLastErr->uhwFileID   = enFileNo;
        pstLastErr->uhwLineID   = uhwLineNo;

        pstLastErr->swValue1 = swValue1;
        pstLastErr->swValue2 = swValue2;
        pstLastErr->swValue3 = swValue3;
    }

    /* log上报未打开则退出 */
    if (OM_SWITCH_ON != OM_LOG_GetEnalbe())
    {
        return UCOM_RET_SUCC;
    }

    /* 待log信息级别未达到设置级别则退出 */
    if (enLevel < OM_LOG_GetLevel())
    {
        return UCOM_RET_SUCC;
    }

    /* 查询其日志上报编号是否处于周期性上报日志范围内 */
    if (   (enLogId >= OM_LOG_COUNTER_BASE)
        && (enLogId <  (OM_LOG_COUNTER_BASE + OM_LOG_COUNTER_MAX_NUM)))
    {
        uwCntIndx   = enLogId - OM_LOG_COUNTER_BASE;
        uhwCycleCnt = OM_LOG_GetCycleCnt(uwCntIndx);
        uhwCycleCnt = (uhwCycleCnt + 1) % uhwLogFreq;

        /*记录计数值*/
        OM_LOG_SetCycleCnt(uwCntIndx, uhwCycleCnt);

        /* 若还未到上报周期，则直接返回 */
        if (uhwCycleCnt > 0)
        {
            return UCOM_RET_SUCC;
        }
    }

    /* 填充log消息*/
    stLogMsg.uhwMsgId       = OM_LOG_GetLogMsgId(enLevel);
    stLogMsg.uhwFileId      = enFileNo;
    stLogMsg.uhwLineNo      = uhwLineNo;
    stLogMsg.uwLogId        = enLogId;
    stLogMsg.aswData[0]     = swValue1;
    stLogMsg.aswData[1]     = swValue2;
    stLogMsg.aswData[2]     = swValue3;

    /* 以透明消息发送 */
    OM_COMM_SendTranMsg(&stLogMsg, sizeof(stLogMsg));

    /* 通过串口打印 */
    #ifdef _OM_UART_PRINT
    {
        VOS_CHAR    ascString[140]; /* 格式字符串长, 再充分考虑7个输出数值的宽度 */

        sprintf(ascString,
                "OM_LOG:: Level %d, File %d, Line %d, Log id %d, Log value %x, %x, %x.\n",
                enLevel,
                enFileNo,
                uhwLineNo,
                enLogId,
                swValue1,
                swValue2,
                swValue3);

        /* 如下函数为阻塞式串口写, 实际调试时可考虑缩短字符串长度 */
        DRV_UART_SendData(ascString, strlen(ascString) + 1);

    }
    #endif

    /* 供PC测试使用 */
    #ifdef _OM_STD_PRINT
    {
        printf("OM_LOG:: Level %d, File %d, Line %d, Log id %d, Log value %x, %x, %x.\n",
               enLevel,
               enFileNo,
               uhwLineNo,
               enLogId,
               swValue1,
               swValue2,
               swValue3);
    }
    #endif

    return UCOM_RET_SUCC;

}
#endif

/*****************************************************************************
 函 数 名  : OM_LOG_MsgCfgLog
 功能描述  : HIFI日志上报设置消息MSG_OM_VOICE_SET_LOG_REQ处理函数
 输入参数  : VOS_VOID *pvOsaMsg - VOS消息
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月31日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_LOG_MsgSetLogReq(VOS_VOID *pvOsaMsg)
{
    VOS_UINT16                  uhwRet     = UCOM_RET_SUCC;
    OM_CODEC_SET_LOG_REQ_STRU    *pstReqMsg  = VOS_NULL;
    OM_LOG_CTRL_STRU           *pstCtrlObj = OM_LOG_GetCtrlObjPtr();
    CODEC_OM_SET_LOG_CNF_STRU     stCnfMsg;

    UCOM_MemSet(&stCnfMsg, 0, sizeof(stCnfMsg));

    /*按OM_CODEC_SET_LOG_REQ_STRU解析消息结构*/
    pstReqMsg = (OM_CODEC_SET_LOG_REQ_STRU *)pvOsaMsg;

    /* 参数检查，若非法，则返回参数错误 */
    if (   (pstReqMsg->enLogEnable >= OM_SWITCH_BUTT)
        || (pstReqMsg->enLogLevel  >= OM_LOG_LEVEL_BUTT))
    {
        uhwRet                  = UCOM_RET_ERR_PARA;
    }
    /*若合法，则设置可维可测日志控制全局变量 */
    else
    {
        UCOM_MemSet(pstCtrlObj->auhwCycleCnt, 0, sizeof(pstCtrlObj->auhwCycleCnt));
        OM_LOG_SetEnable(pstReqMsg->enLogEnable);
        OM_LOG_SetLevel(pstReqMsg->enLogLevel);
    }

    /* 回复消息 */
    stCnfMsg.uhwMsgId           = ID_CODEC_OM_SET_LOG_CNF;
    stCnfMsg.enReturn           = uhwRet;

    OM_COMM_SendTranMsg(&stCnfMsg, sizeof(stCnfMsg));

    return uhwRet;

}

/*****************************************************************************
 函 数 名  : OM_LOG_IsrSoftIntr
 功能描述  : 软中断处理, 软中断用于软件系统致命异常统一处理, 触发软中断后进入本
             处理函数, 记录相关信息后系统进入死循环等待被硬狗守护程序复位
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
VOS_VOID OM_LOG_IsrSoftIntr(VOS_VOID)
{
    OM_LOG_INNER_RESET_STRU            *pstCnter;

    /* 记录错误号到内部日志 */
    OM_LOG_InnerRecord(OM_LOG_GetSoftIntrInfo(), (VOS_UINT16)THIS_FILE_ID, (VOS_UINT16)__LINE__);

    /* 进入死循环, 等待硬件狗检测并复位系统 */
    pstCnter = OM_LOG_GetInnerCnt();
    for ( ; ;)
    {
        pstCnter->uwCnt++;
        pstCnter->uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();
    }
}


VOS_VOID  OM_LOG_AprLogReport(
                OM_APR_MODULE_ID_ENUM_UINT32        enModuleId,
                VOS_UINT32                          uwLogId)
{
    OM_LOG_APR_OBJ_STRU        *pstAprLog;

    /* 异常入参检查 */
    if (  (!OM_LOG_CheckInit())
        ||(OM_LOG_APR_MAX_NUM <= uwLogId)
        ||(OM_APR_MODULE_BUTT <= enModuleId))
    {
        /* 不做日志记录 */
        return;
    }

    /* 获取apr日志结构体 */
    pstAprLog      = OM_LOG_GetAprLogPtr();

    /* 将对应的log计数加一 */
    if (OM_LOG_APR_MAX_LOG_CNT_NUM > pstAprLog->auwAprCnt[enModuleId][uwLogId])
    {
        pstAprLog->auwAprCnt[enModuleId][uwLogId]++;
    }

    return;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

