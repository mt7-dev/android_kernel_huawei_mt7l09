

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "ucom_comm.h"
#include "med_drv_interface.h"
#include "ucom_share.h"

#ifndef __UCOM_LOW_POWER_H__
#define __UCOM_LOW_POWER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* 下电最大空闲时长，若HIFI空闲超过此时长，则应请求下电，单位((16384*10000)/DRV_TIMER_OM_FREQ)ms = 500ms */
#define UCOM_HIFI_PD_TIME_THD           (16384)

/* 低功耗模式最大空闲时长，若HIFI空闲超过此时长，则应进入低功耗模式，单位((33*10000)/DRV_TIMER_OM_FREQ)ms = 1ms */
#define UCOM_HIFI_WI_TIME_THD           (33)

#define UCOM_IS_FIRST_IDLE()            (g_uwUcomPowerCtrl.uwIsFirstIdle != 0x5A5A5A5A)
#define UCOM_GET_LAST_IDLE_TIME()       (g_uwUcomPowerCtrl.uwLastIdleTime)
#define UCOM_GET_LAST_SW_CNT()          (g_uwUcomPowerCtrl.uwLastSwCnt)
#define UCOM_SET_FIRST_IDLE()           (g_uwUcomPowerCtrl.uwIsFirstIdle = 0x5A5A5A5A)
#define UCOM_SET_LAST_IDLE_TIME(uwVar)  (g_uwUcomPowerCtrl.uwLastIdleTime = (uwVar))
#define UCOM_SET_LAST_SW_CNT(uwVar)     (g_uwUcomPowerCtrl.uwLastSwCnt = (uwVar))

#define UCOM_GET_POWEROFF_ENABLE()       (g_uwUcomPowerCtrl.uhwPowerOffEnable)
#define UCOM_SET_POWEROFF_ENABLE(uwVar)  (g_uwUcomPowerCtrl.uhwPowerOffEnable = uwVar)
#define UCOM_GET_WI_TIME_THD()           (g_uwUcomPowerCtrl.uwWITimeThd)
#define UCOM_SET_WI_TIME_THD(uwVar)      (g_uwUcomPowerCtrl.uwWITimeThd = uwVar)
#define UCOM_GET_PD_TIME_THD()           (g_uwUcomPowerCtrl.uwPDTimeThd)
#define UCOM_SET_PD_TIME_THD(uwVar)      (g_uwUcomPowerCtrl.uwPDTimeThd = uwVar)
#define UCOM_GET_WATCHDOG_ENABLE()       (g_uwUcomPowerCtrl.uhwWatchDogEnable)
#define UCOM_SET_WATCHDOG_ENABLE(uwVar)  (g_uwUcomPowerCtrl.uhwWatchDogEnable = uwVar)
#define UCOM_GET_WD_TIMEOUT_LEN()        (g_uwUcomPowerCtrl.uwWDTimeoutLen)
#define UCOM_SET_WD_TIMEOUT_LEN(uwVar)   (g_uwUcomPowerCtrl.uwWDTimeoutLen = uwVar)

#if (VOS_CPU_TYPE == VOS_HIFI)
#define UCOM_SET_WFI(var1)              asm ("waiti 0": :)                      /* 进入Wait for interrupt模式 */
#define UCOM_SET_WFI_NMI(var1)          asm ("waiti 5": :)                      /* 进入Wait for interrupt模式，只响应NMI中断 */
#else
#define UCOM_SET_WFI(var1)
#define UCOM_SET_WFI_NMI(var1)
#endif
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
/*****************************************************************************
 实体名称  : UCOM_POWER_CTRL_STRU
 功能描述  : HIFI上下电控制结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32      uwIsFirstIdle;      /* 标明是否系统第一次初始化完毕 */
    VOS_UINT32      uwLastIdleTime;     /* 上一次进入空闲钩子的系统时戳  */
    VOS_UINT32      uwLastSwCnt;        /* 上一次进入空闲钩子的上下文切换次数  */
    VOS_UINT16      uhwPowerOffEnable;  /* HIFI下电是否使能 */
    VOS_UINT16      uhwWatchDogEnable;  /* HIFI watchdog是否使能 */
    VOS_UINT32      uwWITimeThd;        /* 进入低功耗模式最大空闲时长 */
    VOS_UINT32      uwPDTimeThd;        /* 下电最大空闲时长 */
    VOS_UINT32      uwWDTimeoutLen;     /* watchdog timeout时长 */
}UCOM_POWER_CTRL_STRU;
/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern UCOM_POWER_CTRL_STRU                    g_uwUcomPowerCtrl;


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID UCOM_LOW_PowerInit(VOS_VOID);
extern VOS_VOID UCOM_LOW_PowerDRF(VOS_VOID);
extern VOS_VOID UCOM_LOW_PowerNmiHook(   );
extern VOS_VOID UCOM_LOW_PowerTask(VOS_UINT32 ulVosCtxSw);



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of ucom_low_power.h */
