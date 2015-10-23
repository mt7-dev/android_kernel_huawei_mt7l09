

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "med_drv_interface.h"
#include "med_drv_timer_hifi.h"


#ifndef __DRV_WATCHDOG_H__
#define __DRV_WATCHDOG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define DRV_WATCHDOG_LOAD               (DRV_WATCHDOG_BASE_ADDR + 0x000)        /* SP805 WATCHDOG计数初值，写该寄存器后会重新开始计数，最小有效值为1 */
#define DRV_WATCHDOG_VALUE              (DRV_WATCHDOG_BASE_ADDR + 0x004)        /* SP805 WATCHDOG计数器当前值 */
#define DRV_WATCHDOG_CONTROL            (DRV_WATCHDOG_BASE_ADDR + 0x008)        /* SP805 WATCHDOG控制寄存器 */
#define DRV_WATCHDOG_INTCLR             (DRV_WATCHDOG_BASE_ADDR + 0x00C)        /* SP805 WATCHDOG清除中断寄存器，写入任意值可清除中断重新载入初值开始计数 */
#define DRV_WATCHDOG_RIS                (DRV_WATCHDOG_BASE_ADDR + 0x010)        /* SP805 WATCHDOG原始中断状态寄存器 */
#define DRV_WATCHDOG_MIS                (DRV_WATCHDOG_BASE_ADDR + 0x014)        /* SP805 WATCHDOG屏蔽后中断状态寄存器 */
#define DRV_WATCHDOG_LOCK               (DRV_WATCHDOG_BASE_ADDR + 0xC00)        /* SP805 WATCHDOG锁定寄存器，用于禁止对所有其他寄存器的写访问 */

#define DRV_WATCHDOG_CLOCK_ENABLE       (DRV_AO_SC_BASE_ADDR + 0x630)           /* SP805 WATCHDOG外设时钟使能寄存器 */
#define DRV_WATCHDOG_CLOCK_DISABLE      (DRV_AO_SC_BASE_ADDR + 0x634)           /* SP805 WATCHDOG外设时钟禁止寄存器 */

#define DRV_WATCHDOG_LOCK_NUM           (0x0)                                   /* SP805 WATCHDOG上锁字 */
#define DRV_WATCHDOG_UNLOCK_NUM         (0x1ACCE551)                            /* SP805 WATCHDOG解锁字 */

#define DRV_WATCHDOG_CONTROL_DISABLE    (0x0)                                   /* SP805 WATCHDOG控制字:屏蔽中断并停止，下次打开时会自动重新计数 */
#define DRV_WATCHDOG_CONTROL_ENABLE     (0x3)                                   /* SP805 WATCHDOG控制字:打开中断屏蔽并启动 */

#define DRV_WATCHDOG_INTCLR_NUM         (0x4455)                                /* SP805 WATCHDOG中断清除字，实际上写入任意值都可清除 */

#define DRV_WATCHDOG_TIMEOUT_LEN        (32)                                    /* 看门狗时长,单位:毫秒 */

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
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/
#ifdef _USING_WATCHDOG_
/*****************************************************************************
 函 数 名  : DRV_WATCHDOG_Start
 功能描述  : Watch Dog的启动, HiFi使用DW_APB_TIMER0的定时器4作为watch dog
 输入参数  : uwTimeLen - 时长,单位ms,32K时钟时计算公式为uwTimerLen*32768/1000;
             输入值不得超过0xfffff，建议在1-10000之间
 输出参数  : 无
 返 回 值  : VOS_VOID
*****************************************************************************/
#define DRV_WATCHDOG_Start(uwTimeLen)                           \
    DRV_WATCHDOG_StartWdg((VOS_UINT32)((uwTimeLen)*4096)/125)
/*****************************************************************************
 函 数 名  : DRV_WATCHDOG_Feed
 功能描述  : 喂狗，继续看门监护
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
*****************************************************************************/
#define DRV_WATCHDOG_Feed()                                     \
    DRV_WATCHDOG_RestartWdg()
/*****************************************************************************
 函 数 名  : DRV_WATCHDOG_Stop
 功能描述  : Watch Dog的停止
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
*****************************************************************************/
#define DRV_WATCHDOG_Stop()                                     \
    DRV_WATCHDOG_StopWdg()
#else

#define DRV_WATCHDOG_Start(uwTimeLen)
#define DRV_WATCHDOG_Feed()
#define DRV_WATCHDOG_Stop()

#endif

/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID DRV_WATCHDOG_RestartWdg(VOS_VOID);
extern VOS_VOID DRV_WATCHDOG_StartWdg(VOS_UINT32 uwLoadCnt);
extern VOS_VOID DRV_WATCHDOG_StopWdg(VOS_VOID);










#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_drv_watchdog.h */
