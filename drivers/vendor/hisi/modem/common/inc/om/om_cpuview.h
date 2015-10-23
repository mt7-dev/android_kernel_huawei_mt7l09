/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : om_cpuview.h
  版 本 号   : 初稿
  作    者   : 苏庄銮 59026
  生成日期   : 2011年5月31日
  最近修改   :
  功能描述   : om_cpuview.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年5月31日
    作    者   : 苏庄銮 59026
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "om_comm.h"


#ifndef __OM_CPUVIEW_H__
#define __OM_CPUVIEW_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define OM_CPUVIEW_TASK_NUM             VOS_MAX_TASKS                           /* 当前操作系统支持最大任务个数*/
#define OM_CPUVIEW_INTR_NUM             (32)                                    /* 当前系统最大中断个数*/
#define OM_CPUVIEW_MAX_PERIOD           (5)                                     /* 最大上报周期数 */
#define OM_CPUVIEW_Q8                   (8)                                     /* Q8 */
#define OM_CPUVIEW_DETAIL_RPT_MAX_NUM   (100)                                   /* CPU负载详细信息单次上报最大条数*/
#define OM_CPUVIEW_DETAIL_MAX_NUM       (1024)                                  /* CPU负载详细信息记录最大条数*/
#define OM_CPUVIEW_DETAIL_RPT_GUARD_NUM (200)                                   /* cPU负载详细信息上报守护条数 */

#define OM_CPUVIEW_GetCtrlObjPtr()      (&g_stOmCpuviewCtrl)
#define OM_CPUVIEW_GetBasicPtr()        (&g_stOmCpuviewBasic)
#define OM_CPUVIEW_GetDetailPtr()       (&g_stOmCpuviewDetails)
#define OM_CPUVIEW_GetStackSize(uhwIdx) (g_auwOmCpuviewStackSize[(uhwIdx)][1])
#define OM_CPUVIEW_GetStackId(uhwIdx)   (g_auwOmCpuviewStackSize[(uhwIdx)][0])
#define OM_CPUVIEW_SetStackUsed(uhwIdx,uwSize) (g_auwOmCpuviewStackSize[(uhwIdx)][2] = (uwSize))
#define OM_CPUVIEW_GetBasicEn()         (g_stOmCpuviewCtrl.enEnable)
#define OM_CPUVIEW_GetDetailEn()        (g_stOmCpuviewCtrl.enDetailEn)

#if (VOS_STK_CHK_EN == VOS_YES)
#define OM_CPUVIEW_GetStackUsed(uwFid)  VOS_GetMaxStackUsed(uwFid)              /* 获取指定FID堆栈最大使用量 */
#else
#define OM_CPUVIEW_GetStackUsed(uwFid)  (0)
#endif
/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* CPUVIEW中的区域定义枚举 */
enum OM_CPUVIEW_AREA_ENUM
{
    OM_CPUVIEW_AREA_UCOM_WFI                                = 0,
    OM_CPUVIEW_AREA_UCOM_DRF,
    OM_CPUVIEW_AREA_UCOM_PD,

    OM_CPUVIEW_AREA_AUDIO_PCM_UPDATE_BUFF_PLAY              = 8,
    OM_CPUVIEW_AREA_AUDIO_PCM_UPDATE_BUFF_CAPTURE,
    OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_MLIB_MICIN,
    OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_MLIB_SPKOUT,
    OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_MICIN,
    OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_SPKOUT,
    OM_CPUVIEW_AREA_AUDIO_PLAYER_DECODE,
    OM_CPUVIEW_AREA_AUDIO_PLAYER_SRC,
    OM_CPUVIEW_AREA_AUDIO_PLAYER_DTS,

    OM_CPUVIEW_AREA_VOICE_PROC_MICIN                        = 24,
    OM_CPUVIEW_AREA_VOICE_PROC_SPKOUT,
    OM_CPUVIEW_AREA_VOICE_ENCODE,
    OM_CPUVIEW_AREA_VOICE_DECODE,

    OM_CPUVIEW_AREA_ENUM_BUTT
};
typedef VOS_UINT8   OM_CPUVIEW_AREA_ENUM_UINT8;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : TIMER_OM_CPUVIEW_RPT_IND_STRU
 功能描述  : ID_TIMER_MED_CPUVIEW_RPT_IND结构体
 *****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          uhwMsgId;
    VOS_UINT16                          uhwReserve;
}TIMER_OM_CPUVIEW_RPT_IND_STRU;

/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : OM_CPUVIEW_TASK_RECORD_STRU
 功能描述  : 任务状态描述结构体,每个任务对应一个结构,可扩充
 *****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwRunTime;                              /* 周期内剔除中断打断的任务运行时长，单位与时戳相同 */
}OM_CPUVIEW_TASK_RECORD_STRU;

/*****************************************************************************
 实体名称  : OM_CPUVIEW_INTR_RECORD_STRU
 功能描述  : 中断运行描述结构体,每个中断对应一个该结构,可扩充
 *****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwRunTime;                              /* 周期内中断运行总时长，单位与时戳相同 */
}OM_CPUVIEW_INTR_RECORD_STRU;

/*****************************************************************************
 实体名称  : OM_CPUVIEW_BASIC_STRU
 功能描述  : 当前任务/中断总体状态描述结构体,记录当前任务/中断运行时间信息
 *****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwEnterTaskTs;                          /*当前任务开始时戳*/
    VOS_UINT32                          uwEnterIntrTs;                          /*当前中断进入时戳*/
    VOS_UINT32                          uwIntrTime;                             /*当前任务中断运行时长,每次进行任务切换时此域清零*/
    OM_CPUVIEW_TASK_RECORD_STRU         astTaskRec[OM_CPUVIEW_TASK_NUM];        /*任务统计记录*/
    OM_CPUVIEW_INTR_RECORD_STRU         astIntrRec[OM_CPUVIEW_INTR_NUM];        /*中断统计记录*/
}OM_CPUVIEW_BASIC_STRU;

/*****************************************************************************
 实体名称  : OM_CPUVIEW_DETAILS_STRU
 功能描述  : 负载统计详细状态记录结构体
 *****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwCurrIdx;                             /*当前记录的标号(即从当前记录开始顺序循环覆盖)*/
    VOS_UINT16                          uhwRptIdx;                              /*上报记录标号*/
    OM_CPUVIEW_SLICE_RECORD_STRU        astRecords[OM_CPUVIEW_DETAIL_MAX_NUM];  /* CPU负载记录详细记录 */
}OM_CPUVIEW_DETAILS_STRU;

/*****************************************************************************
 实体名称  : OM_CPUVIEW_CTRL_STRU
 功能描述  : 负载统计控制信息记录结构体
 *****************************************************************************/
typedef struct
{
    OM_SWITCH_ENUM_UINT16               enEnable;                               /* 负载统计上报开关 */
    VOS_UINT16                          uhwReserved;                            /* 保留*/
    VOS_UINT16                          uhwPeriod;                              /* 负载统计上报周期，单位0.1ms*/
    VOS_UINT16                          enDetailEn;                             /* 负载统计详细信息上报开关 */
    VOS_UINT32                          uwTimer;                                /* 定时器 */

}OM_CPUVIEW_CTRL_STRU;


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID OM_CPUVIEW_AddSliceRecord(
                       OM_CPUVIEW_TARGET_ENUM_UINT8                enTarget,
                       VOS_UINT8                                   ucTargetId,
                       OM_CPUVIEW_ACTION_ENUM_UINT8                enAction,
                       VOS_UINT32                                  uwTimeStamp);
extern VOS_VOID OM_CPUVIEW_EnterArea(VOS_UCHAR ucAreaId);
extern VOS_VOID OM_CPUVIEW_EnterIntHook(VOS_UINT32 uwIntNo);
extern VOS_VOID OM_CPUVIEW_ExitArea(VOS_UCHAR ucAreaId);
extern VOS_VOID OM_CPUVIEW_ExitIntHook(VOS_UINT32 uwIntNo);
extern VOS_VOID OM_CPUVIEW_Init(VOS_VOID);
extern VOS_UINT32 OM_CPUVIEW_MsgCfgReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 OM_CPUVIEW_MsgRptInd(VOS_VOID *pvOsaMsg);
extern VOS_VOID OM_CPUVIEW_ReportBasicInfo(VOS_VOID);
extern VOS_VOID OM_CPUVIEW_ReportDetailInfo(VOS_VOID);
extern VOS_VOID OM_CPUVIEW_StatTimerHandler(VOS_UINT32 uwTimer, VOS_UINT32 uwPara);
extern VOS_VOID OM_CPUVIEW_TaskSwitchHook(VOS_VOID *pvOldTcb, VOS_VOID *pvNewTcb);
extern VOS_VOID OM_CPUVIEW_RecordStackUsage( VOS_VOID );


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of om_cpuview.h */
