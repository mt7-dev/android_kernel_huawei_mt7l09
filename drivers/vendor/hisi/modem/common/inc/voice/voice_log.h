

#ifndef __VOICE_LOG_H__
#define __VOICE_LOG_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "OmCodecInterface.h"
#include "codec_typedefine.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _MED_ERRORLOG
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define VOICE_LOG_CNTER_TOP                                   0x7F00              /* 计数日志起始号 */
#define VOICE_LOG_CNTER_MAX_NUM                               (15)                /* 日志计数器最大个数, <0xFF */

#define VOICE_ERRLOG_ERRNO_BASE                               (0x00000601)        /* VOICE 组件 ErrorLog 起始错误序号 */
#define VOICE_ERRLOG_EVENT_REPORT_MAX                         (1)                 /* 每轮通话中单个事件允许上报的最大次数 */
#define VOICE_ERRLOG_EVENT_DEFAULT_THD                        (50)                /* 默认的事件错误门限 */
#define VOICE_ERRLOG_EVENT_MAXIMUM_THD                        (502)               /* 事件错误门限最大值 */
#define VOICE_ERRLOG_LAST_CYCLE_INI_VAL                       (-2)                /* 最近一次事件发生周期初始化数值 */

#define MSG_VOICE_OM_ERRLOG_PID                               (0x5100)            /* ErrorLog PID 号 */
#define VOICE_ERRLOG_CHECK_PERIOD                             (250)               /* 5秒检查周期 */

#define VOICE_ERRLOG_CONFIG_HEAD_LEN                          (2)                 /* ERROR配置事件门限起始地址偏移长度, 单位VOICE_INT16 */

#define VOICE_ERRLOG_EVENTS_WCDMA_DEC_INT_RECEIVE \
                CODEC_ERRLOG_EVENTS_WCDMA_DEC_INT_LOST                            /* 转定义 LOST 事件 */
#define VOICE_LOG_PERIOD                                      (250)

/* 封装全局变量 */
#define VOICE_ERRLOG_GetErrLogMsgPtr()                        (&g_stVoiceErrLogMsg)
#define VOICE_ERRLOG_GetRecordHeadPtr()                       (&g_stVoiceErrLogMsg.stRecordHead)
#define VOICE_ERRLOG_GetEventsInfoPtr()                       (g_stVoiceErrLogStatus.astEventsInfo)

#endif

#ifdef _MED_ERRORLOG

/*****************************************************************************
 宏    名  : VOICE_VsErrLogEventRec
 功能描述  : 记录ErrorLog事件
 输入参数  : enEvent, 事件编号
 输出参数  : 无
 返 回 值  : 无

 ****************************************************************************/
#define VOICE_ErrLogEventRec(enEvent) \
{ \
    VOICE_ErrLogRecord((enEvent)); \
}

/*****************************************************************************
 宏    名  : VOICE_VsErrLogEventCycle
 功能描述  : 周期触发的事件上报检查函数
 输入参数  : shwPeriod, 检查事件的周期, 即调用shwPeriod次数该函数后,进行事件检查
 输出参数  : 无
 返 回 值  : 无

 ****************************************************************************/
#define VOICE_ErrLogEventCycle(shwPeriod) \
{ \
    VOICE_ErrLogCycle((shwPeriod)); \
}

/*****************************************************************************
 宏    名  : VOICE_VsErrLogReset
 功能描述  : 重置计数器
 输入参数  : shwPeriod, 检查事件的周期, 即调用shwPeriod次数该函数后,进行事件检查
 输出参数  : 无
 返 回 值  : 无

 ****************************************************************************/
#define VOICE_ErrLogReset() \
{ \
    VOICE_ErrLogResetStatus(); \
}
#else
#define VOICE_ErrLogEventRec(enEvent)
#define VOICE_ErrLogEventCycle(shwPeriod)
#define VOICE_ErrLogReset()
#endif

#ifdef _MED_ERRORLOG
/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 消息头定义
*****************************************************************************/

/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/* ErrorLog 事件条件 */
typedef struct
{
    CODEC_ERRLOG_EVENT_TYPE_ENUM_UINT16                       enType;             /* 事件类型 */
    CODEC_ERRLOG_EVENT_MODE_ENUM_UINT16                       enMode;             /* 事件场景 */
    VOS_UINT16                                              uhwThd;             /* 事件门限 */
    VOS_UINT16                                              uhwReserve;

} VOICE_ERRLOG_EVENT_RULE_STRU;

/* ErrorLog 事件错误信息 */
typedef struct
{
    VOICE_ERRLOG_EVENT_RULE_STRU                              stRules;            /* 事件约束条件 */
    VOS_UINT16                                              uhwCnt;             /* 事件计数 */
    VOS_INT16                                               shwLastCyc;         /* 最近一次出现错误的Cycle */
    VOS_UINT16                                              uhwRprtd;           /* 错误上报标志 */
    VOS_UINT16                                              uhwReserve;

} VOICE_ERRLOG_EVENT_INFO_STRU;

/* ErrorLog 运行时状态 */
typedef struct
{
    VOS_UINT16                                              uhwEnable;          /* ErrorLog 使能标志 */
    VOS_INT16                                               shwCycleCnt;        /* 运行次数计数器 */
    VOICE_ERRLOG_EVENT_INFO_STRU                              astEventsInfo[CODEC_ERRLOG_EVENTS_ENUM_BUTT]; /* 统计各类错误事件的信息 */

} VOICE_ERRLOG_STATUS_STRU;

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
extern VOS_VOID VOICE_ErrLogInit( CODEC_ERRLOG_CONFIG_STRU *pstErrlogCfg );
extern VOS_VOID VOICE_ErrLogResetEventCnts( VOICE_ERRLOG_EVENT_INFO_STRU *pstEvent );
extern VOS_UINT16 VOICE_ErrLogGetEventThd( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent );
extern VOS_VOID VOICE_ErrLogRecord( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent );
extern VOS_VOID VOICE_ErrLogUpdateEnv( VOS_VOID );
extern VOS_VOID VOICE_ErrLogPackLogHead(MNTN_HEAD_INFO_STRU *pstMsgMntnHead,
                                       CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent );
extern VOS_VOID VOICE_ErrLogReport( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent );
extern VOS_UINT32 VOICE_ErrLogCheckEvent( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent );
extern VOS_VOID VOICE_ErrLogCycle( VOS_INT16 shwPeriod );
extern VOS_VOID VOICE_ErrLogResetStatus( VOS_VOID );
#endif
extern VOS_VOID VOICE_ErrLogCheckDecStatus(VOS_VOID *pstDecObj);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* end of med_vs_log.h */

