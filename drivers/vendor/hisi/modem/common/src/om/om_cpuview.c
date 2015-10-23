/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : om_cpuview.c
  版 本 号   : 初稿
  作    者   : 苏庄銮 59026
  生成日期   : 2011年5月31日
  最近修改   :
  功能描述   : 可维可测-负载统计功能实现
  函数列表   :
              OM_CPUVIEW_AddSliceRecord
              OM_CPUVIEW_EnterArea
              OM_CPUVIEW_EnterIntHook
              OM_CPUVIEW_ExitArea
              OM_CPUVIEW_ExitIntHook
              OM_CPUVIEW_Init
              OM_CPUVIEW_MsgCfgReq
              OM_CPUVIEW_MsgRptInd
              OM_CPUVIEW_ReportBasicInfo
              OM_CPUVIEW_ReportDetailInfo
              OM_CPUVIEW_StatTimerHandler
              OM_CPUVIEW_TaskSwitchHook
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

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_OM_CPUVIEW_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* 负载统计控制信息记录 */
OM_CPUVIEW_CTRL_STRU                    g_stOmCpuviewCtrl;

/* 负载统计基本统计信息 */
OM_CPUVIEW_BASIC_STRU                   g_stOmCpuviewBasic;

/* 负载统计详细信息记录 */
OM_CPUVIEW_DETAILS_STRU                 g_stOmCpuviewDetails;

/* 各堆栈大小表格,每行为堆栈ID、堆栈大小 */
VOS_UINT32 g_auwOmCpuviewStackSize[OM_CPUVIEW_TASK_NUM][3] =
{
    {DSP_FID_RT,        VOS_TSK_RT_STK_SIZE,        0},
    {DSP_FID_NORMAL,    VOS_TSK_NORMAL_STK_SIZE,    0},
    {DSP_FID_LOW,       VOS_TSK_LOW_STK_SIZE,       0},
    {IDLE_STK_ID,       VOS_TASK_IDLE_STK_SIZE,     0},
};


/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_Init
 功能描述  : 可维可测模块CPU统计功能处始化
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
VOS_VOID OM_CPUVIEW_Init(VOS_VOID)
{

    /* 默认关闭负载统计上报 */
    UCOM_MemSet(OM_CPUVIEW_GetCtrlObjPtr(), 0, sizeof(OM_CPUVIEW_CTRL_STRU));
    UCOM_MemSet(OM_CPUVIEW_GetDetailPtr(), 0, sizeof(OM_CPUVIEW_DETAILS_STRU));

    /* 注册任务切换记录钩子函数 */
    VOS_RegTaskSwitchHook(OM_CPUVIEW_TaskSwitchHook);

    /* 注册中断进、出钩子函数 */
    VOS_RegisterEnterIntrHook(OM_CPUVIEW_EnterIntHook);
    VOS_RegisterExitIntrHook(OM_CPUVIEW_ExitIntHook);

}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_MsgCfgReq
 功能描述  : 处理CPU负载统计功能设置消息
 输入参数  : VOS_VOID *pvOsaMsg - ID_OM_CODEC_CPU_VIEW_REQ消息
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_CPUVIEW_MsgCfgReq(VOS_VOID *pvOsaMsg)
{
    VOS_UINT32                          uwRet     = UCOM_RET_SUCC;
    OM_CODEC_CPU_VIEW_REQ_STRU         *pstReqMsg = VOS_NULL;
    OM_CPUVIEW_CTRL_STRU               *pstCtrl   = OM_CPUVIEW_GetCtrlObjPtr();
    CODEC_OM_CPU_VIEW_CNF_STRU          stCnfMsg;
    OM_CPUVIEW_BASIC_STRU              *pstBasic  = OM_CPUVIEW_GetBasicPtr();
    VOS_CPU_SR                          srSave;

    UCOM_MemSet(&stCnfMsg, 0, sizeof(stCnfMsg));

    /* 按照OM_CODEC_CPU_VIEW_REQ_STRU结构进行消息解析 */
    pstReqMsg = (OM_CODEC_CPU_VIEW_REQ_STRU*)pvOsaMsg;

    /* 参数检查 */
    if (   (OM_SWITCH_BUTT <= pstReqMsg->enEnable)
        || (OM_SWITCH_BUTT <= pstReqMsg->enDetailEnable)
        || (0 == pstReqMsg->uhwPeriod)
        || (OM_CPUVIEW_MAX_PERIOD < pstReqMsg->uhwPeriod))
    {
        uwRet  = UCOM_RET_ERR_PARA;
    }
    else
    {
        /* 锁中断 */
        srSave = VOS_SplIMP();

        /* 访问全局变量,将消息中配置保存 */
        pstCtrl->enEnable       = pstReqMsg->enEnable;
        pstCtrl->enDetailEn     = pstReqMsg->enDetailEnable;
        pstCtrl->uhwPeriod      = (VOS_UINT16)(pstReqMsg->uhwPeriod
                                * OM_MSGHOOK_TENTH_SECOND_PER20MS);             /* 配置参数单位为20ms */

        /* 启动周期性统计功能 */
        if (   (OM_SWITCH_ON == pstCtrl->enEnable)
            || (OM_SWITCH_ON == pstCtrl->enDetailEn))
        {
            /* 停止定时器 */
            uwRet = DRV_TIMER_Stop(&(pstCtrl->uwTimer));

            /* 清空残留的基本统计数据 */
            UCOM_MemSet(pstBasic, 0, sizeof(OM_CPUVIEW_BASIC_STRU));

            /* 启动定时器 */
            uwRet = DRV_TIMER_Start(&(pstCtrl->uwTimer),
                                     pstCtrl->uhwPeriod,
                                     DRV_TIMER_MODE_PERIODIC,
                                     OM_CPUVIEW_StatTimerHandler,
                                     0);
            OM_LogInfo(OM_CPUVIEW_MsgCfgReq_StartOk);

        }
        /* 关闭周期性统计功能 */
        else
        {
            /* 停止定时器 */
            uwRet = DRV_TIMER_Stop(&(pstCtrl->uwTimer));

            /* 清空残留的基本统计数据 */
            UCOM_MemSet(pstBasic, 0, sizeof(OM_CPUVIEW_BASIC_STRU));
            OM_LogInfo(OM_CPUVIEW_MsgCfgReq_StopOk);
        }

        /* 释放中断 */
        VOS_Splx(srSave);
    }

    /* 填充回复消息 */
    stCnfMsg.uhwMsgId   = ID_CODEC_OM_CPU_VIEW_CNF;
    stCnfMsg.enReturn   = (UCOM_RET_ENUM_UINT16)uwRet;

    OM_COMM_SendTranMsg(&stCnfMsg, sizeof(stCnfMsg));

    return uwRet;
}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_MsgRptInd
 功能描述  : 定时负载统计消息处理函数
 输入参数  : VOS_VOID *pvOsaMsg -
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月20日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_CPUVIEW_MsgRptInd(VOS_VOID *pvOsaMsg)
{
    /* 上报基本CPU负载信息 */
    OM_CPUVIEW_ReportBasicInfo();

    /* 上报CPU运行详细信息 */
    OM_CPUVIEW_ReportDetailInfo();

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_ReportBasicInfo
 功能描述  : 上报CPU负载统计基本信息
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月20日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_CPUVIEW_ReportBasicInfo(VOS_VOID)
{
    OM_CPUVIEW_BASIC_STRU              *pstBasic    = OM_CPUVIEW_GetBasicPtr();
    CODEC_OM_CPUVIEW_BASIC_IND_STRU     stBasicMsg;
    VOS_UINT32                          uwCnt;
    VOS_UINT32                          uwRate;
    VOS_CPU_SR                          srSave;

    UCOM_MemSet(&stBasicMsg, 0, sizeof(stBasicMsg));

    /* CPU负载统计未使能则直接返回 */
    if (OM_SWITCH_OFF == OM_CPUVIEW_GetBasicEn())
    {
        return;
    }

    /* 接口宏定义一致性检查 */ /*lint --e(506)*/
    if (   (HIFI_TASK_NUM != OM_CPUVIEW_TASK_NUM)
        || (HIFI_INTR_NUM != OM_CPUVIEW_INTR_NUM))
    {
        OM_LogError(OM_CPUVIEW_ReportBasicInfo_MacroDefErr);
        return;
    }

    /* 填充上报消息 */

    stBasicMsg.uhwMsgId = ID_CODEC_OM_CPUVIEW_BASIC_IND;

    /* 填充任务信息 */
    for (uwCnt = 0; uwCnt < OM_CPUVIEW_TASK_NUM; uwCnt++)
    {
        uwRate  = OM_CPUVIEW_GetStackUsed(OM_CPUVIEW_GetStackId(uwCnt));
        uwRate  = (uwRate << OM_CPUVIEW_Q8) / OM_CPUVIEW_GetStackSize(uwCnt);

        stBasicMsg.auwStackRate[uwCnt]  = uwRate;
        stBasicMsg.auwTaskTime[uwCnt]   = pstBasic->astTaskRec[uwCnt].uwRunTime;
    }

    /* 填充中断时长信息 */
    for (uwCnt = 0; uwCnt < OM_CPUVIEW_INTR_NUM; uwCnt++)
    {
        stBasicMsg.auwIntrTime[uwCnt] = pstBasic->astIntrRec[uwCnt].uwRunTime;
    }

    /* 上报消息 */
    OM_COMM_SendTranMsg(&stBasicMsg, sizeof(stBasicMsg));

    /* 锁中断 */
    srSave = VOS_SplIMP();

    /* 清空此前的记录 */
    UCOM_MemSet(pstBasic->astIntrRec, 0, sizeof(pstBasic->astIntrRec));
    UCOM_MemSet(pstBasic->astTaskRec, 0, sizeof(pstBasic->astTaskRec));

    /* 释放中断 */
    VOS_Splx(srSave);

}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_ReportDetailInfo
 功能描述  : 上报CPU负载统计详细信息
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月20日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_CPUVIEW_ReportDetailInfo(VOS_VOID)
{
    OM_CPUVIEW_DETAILS_STRU            *pstDetails  = OM_CPUVIEW_GetDetailPtr();
    CODEC_OM_CPUVIEW_DETAIL_IND_STRU      stCnfMsg;
    UCOM_DATA_BLK_STRU                  astBlk[3];  /* 3块:消息头、两个记录块 */
    VOS_UINT32                          uwBlkCnt;
    VOS_UINT16                          uhwRecNum;
    VOS_UINT16                          uhwRptIdx;

    /* CPU负载统计未使能则直接返回 */
    if (OM_SWITCH_OFF == OM_CPUVIEW_GetDetailEn())
    {
        return;
    }

    /* 填充第一个数据块:信息头 */
    stCnfMsg.uhwMsgId   = ID_CODEC_OM_CPUVIEW_DETAIL_IND;
    astBlk[0].pucData   = (VOS_UCHAR*)&stCnfMsg;
    astBlk[0].uwSize    = sizeof(stCnfMsg) - sizeof(stCnfMsg.astRecords);

    /* 计算待上报记录数 */
    uhwRecNum           = UCOM_COMM_CycSub(pstDetails->uhwCurrIdx,
                                           pstDetails->uhwRptIdx,
                                           OM_CPUVIEW_DETAIL_MAX_NUM);

    /* 没有数据需要上报则直接退出 */
    if (0 == uhwRecNum)
    {
        return;
    }

    /* 单次上报长度控制 */
    if (uhwRecNum > OM_CPUVIEW_DETAIL_RPT_MAX_NUM)
    {
        uhwRecNum = OM_CPUVIEW_DETAIL_RPT_MAX_NUM;
    }
    uhwRptIdx = OM_COMM_CycAdd(pstDetails->uhwRptIdx,
                               uhwRecNum,
                               OM_CPUVIEW_DETAIL_MAX_NUM);

    /* 上报数据在环形队列的连续线性空间 */
    if (uhwRptIdx > pstDetails->uhwRptIdx)
    {
        uwBlkCnt            = 2;        /* 消息头+1块上报数据 */
        astBlk[1].pucData   = (VOS_UCHAR*)&(pstDetails->astRecords[pstDetails->uhwRptIdx]);
        astBlk[1].uwSize    = uhwRecNum * sizeof(OM_CPUVIEW_SLICE_RECORD_STRU);
    }
    /* 上报数据位于环形队列的尾部和头部(不连续的两个线性空间) */
    else
    {
        uwBlkCnt            = 3;        /* 消息头+2块上报数据 */
        astBlk[1].pucData   = (VOS_UCHAR*)&(pstDetails->astRecords[pstDetails->uhwRptIdx]);
        astBlk[1].uwSize    = (OM_CPUVIEW_DETAIL_MAX_NUM - pstDetails->uhwRptIdx)
                            * sizeof(OM_CPUVIEW_SLICE_RECORD_STRU);
        astBlk[2].pucData   = (VOS_UCHAR*)&(pstDetails->astRecords[0]);
        astBlk[2].uwSize    = uhwRptIdx * sizeof(OM_CPUVIEW_SLICE_RECORD_STRU);
    }

    /* 上报消息 */
    OM_COMM_SendTrans(astBlk, uwBlkCnt);

    /* 更新待(下次)上报记录位置 */
    pstDetails->uhwRptIdx = uhwRptIdx;

}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_AddSliceRecord
 功能描述  : 新添加一条CPU负载详细记录
 输入参数  : OM_CPUVIEW_TARGET_ENUM_UINT8    enTarget   - 记录对象类型
             VOS_UINT8                       ucTargetId - 记录对象编号
             OM_CPUVIEW_ACTION_ENUM_UINT8    enAction   - 记录动作类型
             VOS_UINT32                      uwTimeStamp- 时戳
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_CPUVIEW_AddSliceRecord(
                OM_CPUVIEW_TARGET_ENUM_UINT8                enTarget,
                VOS_UINT8                                   ucTargetId,
                OM_CPUVIEW_ACTION_ENUM_UINT8                enAction,
                VOS_UINT32                                  uwTimeStamp)
{
    VOS_UINT16                          uhwIdx;
    OM_CPUVIEW_SLICE_RECORD_STRU       *pstRec;
    OM_CPUVIEW_DETAILS_STRU            *pstDetails;
    VOS_CPU_SR                          srSave;

    /* 锁中断 */
    srSave = VOS_SplIMP();

    /* 获取可写入位置 */
    pstDetails              = OM_CPUVIEW_GetDetailPtr();
    uhwIdx                  = pstDetails->uhwCurrIdx;
    pstRec                  = &(pstDetails->astRecords[uhwIdx]);

    /* 写入一条新的详细记录 */
    pstRec->uwTarget        = (enTarget & 0x3);             /* 取低2比特 */
    pstRec->uwTargetId      = (ucTargetId & 0x1f);          /* 取低5比特 */
    pstRec->uwAction        = (enAction & 0x1);             /* 取低1比特 */
    pstRec->uwTimeStamp     = (uwTimeStamp & 0xffffff);     /* 取低24比特*/
    pstDetails->uhwCurrIdx  = ((uhwIdx + 1) % OM_CPUVIEW_DETAIL_MAX_NUM);

    /* 释放中断 */
    VOS_Splx(srSave);

}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_TaskSwitchHook
 功能描述  : 任务切换钩子函数，将任务切换内容记录下来;注册后在任务切换时调用
 输入参数  : VOS_VOID *pvOldTcb - 退出任务,任务A的TCB指针
             VOS_VOID *pvNewTcb - 进入任务,任务B的TCB指针
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月20日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_CPUVIEW_TaskSwitchHook(VOS_VOID *pvOldTcb, VOS_VOID *pvNewTcb)
{
    VOS_UINT32                          uwTimeStamp;
    VOS_UINT32                          uwDuration;
    VOS_UINT8                           ucTaskId;
    OM_CPUVIEW_BASIC_STRU              *pstBasic;
    OM_CPUVIEW_DETAILS_STRU            *pstDetails;
    VOS_TCB                            *pstOldTcb   = (VOS_TCB*)pvOldTcb;
    VOS_TCB                            *pstNewTcb   = (VOS_TCB*)pvNewTcb;
    VOS_CPU_SR                          srSave;

    /* 获取任务切换时戳 */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* 获取进入任务号 */
    ucTaskId    = (VOS_UINT8)(pstNewTcb->OSTCBPrio & 0x1f);

    /* 暂只记录任务进入详细信息, 截取任务号低5bit(取值0-31) */
    OM_CPUVIEW_AddSliceRecord(OM_CPUVIEW_TARGET_TASK,
                              ucTaskId,
                              OM_CPUVIEW_ACTION_ENTER,
                              uwTimeStamp);

    /* 基本信息上报打开时 */
    if (OM_SWITCH_ON == OM_CPUVIEW_GetBasicEn())
    {
        /* 锁中断 */
        srSave = VOS_SplIMP();

        pstBasic    = OM_CPUVIEW_GetBasicPtr();

        /* 计算OLD任务已运行时段长度(含中断) */
        uwDuration  = UCOM_COMM_CycSub(uwTimeStamp, pstBasic->uwEnterTaskTs, 0xffffffff);

        /* 计算OLD任务已运行时间 */
        uwDuration  -= pstBasic->uwIntrTime;

        /* 更新OLD任务时间信息 */
        ucTaskId    = (VOS_UINT8)(pstOldTcb->OSTCBPrio & 0x1f);
        pstBasic->astTaskRec[ucTaskId].uwRunTime += uwDuration;

        /* 更新当前任务起始时间, 清除中断运行时长 */
        pstBasic->uwIntrTime    = 0;
        pstBasic->uwEnterTaskTs = uwTimeStamp;

        /* 释放中断 */
        VOS_Splx(srSave);

    }

    /* 详细信息上报打开时 */
    if (OM_SWITCH_ON == OM_CPUVIEW_GetDetailEn())
    {
        pstDetails = OM_CPUVIEW_GetDetailPtr();

        /* 待上报记录超过守护阈值时主动上报 */
        if (OM_CPUVIEW_DETAIL_RPT_GUARD_NUM
            < UCOM_COMM_CycSub(pstDetails->uhwCurrIdx,
                               pstDetails->uhwRptIdx,
                               OM_CPUVIEW_DETAIL_MAX_NUM))
        {
            /* 上报基本CPU负载信息 */
            OM_CPUVIEW_ReportBasicInfo();

            /* 上报CPU运行详细信息 */
            OM_CPUVIEW_ReportDetailInfo();
        }
    }

}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_EnterIntHook
 功能描述  : 记录进入中断的时间信息, 进入中断时被调用
 输入参数  : VOS_UINT32 uwIntNo - 中断号
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_CPUVIEW_EnterIntHook(VOS_UINT32 uwIntNo)
{
    VOS_UINT32                          uwTimeStamp;
    OM_CPUVIEW_BASIC_STRU              *pstBasic;

    /* 获取中断进入时戳 */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* 记录基本信息 */
    if (OM_SWITCH_ON == OM_CPUVIEW_GetBasicEn())
    {
        pstBasic = OM_CPUVIEW_GetBasicPtr();
        pstBasic->uwEnterIntrTs = uwTimeStamp;
    }

    /* 记录详细信息, 截取中断号低5bit(取值0-31) */
    OM_CPUVIEW_AddSliceRecord(OM_CPUVIEW_TARGET_INT,
                              (VOS_UINT8)(uwIntNo & 0x1f),
                              OM_CPUVIEW_ACTION_ENTER,
                              uwTimeStamp);
}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_ExitIntHook
 功能描述  : 中断退出时被调用, 计算中断运行时长, 依赖于中断不嵌套
 输入参数  : VOS_UINT32 uwIntNo - 中断号
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_CPUVIEW_ExitIntHook(VOS_UINT32 uwIntNo)
{
    VOS_UINT32                          uwTimeStamp;
    VOS_UINT32                          uwDuration;
    OM_CPUVIEW_BASIC_STRU              *pstBasic;

    /* 获取中断进入时戳 */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* 记录详细信息, 截取中断号低5bit(取值0-31) */
    OM_CPUVIEW_AddSliceRecord(OM_CPUVIEW_TARGET_INT,
                              (VOS_UINT8)(uwIntNo & 0x1f),
                              OM_CPUVIEW_ACTION_EXIT,
                              uwTimeStamp);

    if (OM_SWITCH_ON == OM_CPUVIEW_GetBasicEn())
    {
        pstBasic = OM_CPUVIEW_GetBasicPtr();

        /* 根据中断进入时间计算中断停留时间 */
        uwDuration = UCOM_COMM_CycSub(uwTimeStamp, pstBasic->uwEnterIntrTs, 0xffffffff);

        /* 更新对应中断的运行总时间 */
        pstBasic->astIntrRec[uwIntNo].uwRunTime += uwDuration;
        pstBasic->uwIntrTime                    += uwDuration;
    }
}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_EnterArea
 功能描述  : 负载统计区域进入
 输入参数  : VOS_UCHAR ucAreaId
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_CPUVIEW_EnterArea(VOS_UCHAR ucAreaId)
{
    VOS_UINT32              uwTimeStamp;

    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    OM_CPUVIEW_AddSliceRecord(OM_CPUVIEW_TARGET_REGION,
                              ucAreaId,
                              OM_CPUVIEW_ACTION_ENTER,
                              uwTimeStamp);
}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_ExitArea
 功能描述  : 负载统计区域退出
 输入参数  : VOS_UCHAR ucAreaId
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_CPUVIEW_ExitArea(VOS_UCHAR ucAreaId)
{
    VOS_UINT32              uwTimeStamp;

    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    OM_CPUVIEW_AddSliceRecord(OM_CPUVIEW_TARGET_REGION,
                              ucAreaId,
                              OM_CPUVIEW_ACTION_EXIT,
                              uwTimeStamp);
}

/*****************************************************************************
 函 数 名  : OM_CPUVIEW_StatTimerHandler
 功能描述  : 触发周期性CPU统计的定时器处理函数
 输入参数  : VOS_UINT32 uwTimer - 定时器标志
             VOS_UINT32 uwPara  - 回调参数
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月20日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_CPUVIEW_StatTimerHandler(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    TIMER_OM_CPUVIEW_RPT_IND_STRU       stReqMsg;

    stReqMsg.uhwMsgId      = ID_TIMER_CODEC_CPUVIEW_RPT_IND;

    UCOM_SendOsaMsg(DSP_PID_HIFI_OM, DSP_PID_HIFI_OM, &stReqMsg, sizeof(stReqMsg));
}


VOS_VOID OM_CPUVIEW_RecordStackUsage( VOS_VOID )
{
    VOS_UINT32 uwCnt, uwSize;

    for (uwCnt = 0; uwCnt < OM_CPUVIEW_TASK_NUM; uwCnt++)
    {
        uwSize  = OM_CPUVIEW_GetStackUsed(OM_CPUVIEW_GetStackId(uwCnt));

        OM_CPUVIEW_SetStackUsed(uwCnt, uwSize);
    }
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

