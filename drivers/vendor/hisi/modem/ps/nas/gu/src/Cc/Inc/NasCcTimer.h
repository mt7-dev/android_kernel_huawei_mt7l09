
#ifndef  NAS_CC_TIMER_PROC_H
#define  NAS_CC_TIMER_PROC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "NasCcCommon.h"


/*****************************************************************************
  2 类型定义
*****************************************************************************/
/* CC定时器ID定义 */
typedef enum
{
    TI_NAS_CC_T303,
    TI_NAS_CC_T305,
    TI_NAS_CC_T308,
    TI_NAS_CC_T310,
    TI_NAS_CC_T313,
    TI_NAS_CC_T323,
    TI_NAS_CC_T332,
    TI_NAS_CC_T335,
    TI_NAS_CC_T336,
    TI_NAS_CC_T337,
    TI_NAS_CC_HOLD,                                                             /* 呼叫保持/恢复操作的定时器 */
    TI_NAS_CC_MPTY,                                                             /* MPTY操作的定时器 */
    TI_NAS_CC_ECT,
    TI_NAS_CC_USER_CONN,                                                        /* 用户连接定时器 */
    TI_NAS_CC_RABMINACT_PROTECT,                                                        /* 用户连接定时器 */
    TI_NAS_CC_MAX
} NAS_CC_TIMER_ID_ENUM;

/* 启动T308的次数 */
enum
{
    NAS_CC_T308_FIRST                                       = 1,
    NAS_CC_T308_SECOND
};


/*****************************************************************************
  3 函数声明
*****************************************************************************/


VOS_UINT8 NAS_CC_GetNvTimerLen(
    NAS_CC_TIMER_ID_ENUM                enTimerId,
    VOS_UINT32                         *ulTimerLen
);


VOS_VOID NAS_CC_SetNvTimerLen(
    NAS_CC_TIMER_ID_ENUM                enTimerId,
    VOS_UINT32                          ulTimerLen
);

/*****************************************************************************
 函 数 名  : NAS_CC_InitAllTimers
 功能描述  : 初始化所有定时器，应在CC初始化及Reset时被调用
 输入参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月17日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  NAS_CC_InitAllTimers(VOS_VOID);


/*****************************************************************************
 函 数 名  : NAS_CC_StartTimer
 功能描述  : 启动指定的CC定时器
 输入参数  : entityId  - 需要启动定时器的CC实体
             enTimerId - 需要启动的定时器ID
             ulParam   - 定时器启动者携带的自定义参数(可选)
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月17日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  NAS_CC_StartTimer(
    NAS_CC_ENTITY_ID_T                  EntityId,
    NAS_CC_TIMER_ID_ENUM                enTimerId,
    VOS_UINT32                          ulParam
);


/*****************************************************************************
 函 数 名  : NAS_CC_StopTimer
 功能描述  : 停止指定的CC定时器
 输入参数  : entityId  - 需要停止定时器的CC实体
             enTimerId - 需要停止的定时器ID
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月17日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  NAS_CC_StopTimer(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_TIMER_ID_ENUM                enTimerId
);


/*****************************************************************************
 函 数 名  : NAS_CC_StopAllTimer
 功能描述  : 停止指定CC实体的所有CC定时器
 输入参数  : entityId  - 需要停止所有定时器的CC实体
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月17日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  NAS_CC_StopAllTimer(
    NAS_CC_ENTITY_ID_T                  entityId
);


/*****************************************************************************
 函 数 名  : NAS_CC_ProcTimeoutMsg
 功能描述  : 处理定时器超时消息。该函数将根据超时消息中携带的定时器句柄信息找到
             超时的CC实体和对应的超时处理函数，并调用该超时处理函数进行处理。
 输入参数  : pMsg  - 定时器超时消息
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月17日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  NAS_CC_ProcTimeoutMsg(
    VOS_VOID                            *pMsg
);

/*****************************************************************************
 函 数 名  : NAS_CC_StopAllRunningTimer
 功能描述  : 停止运行当前所有正在运行的定时器
 输入参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月17日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  NAS_CC_StopAllRunningTimer(VOS_VOID);



VOS_VOID  NAS_CC_StartRabProtectTimer(VOS_VOID);


VOS_VOID  NAS_CC_StopRabProtectTimer(VOS_VOID);


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* NAS_CC_TIMER_PROC_H */

