

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_drv_watchdog.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID DRV_WATCHDOG_StartWdg(VOS_UINT32 uwLoadCnt)
{
#if 0
    /* 使能外设时钟,25、26bit置1 */
    UCOM_RegBitWr(DRV_WATCHDOG_CLOCK_ENABLE, UCOM_BIT25, UCOM_BIT26, 0x3);

    /* 写入解锁值0x1ACCE551，解锁 */
    UCOM_RegWr(DRV_WATCHDOG_LOCK, DRV_WATCHDOG_UNLOCK_NUM);

    /* 计数停止 */
    UCOM_RegWr(DRV_WATCHDOG_CONTROL, DRV_WATCHDOG_CONTROL_DISABLE);

    /* 初始化时长，单位 1/32768 s(时钟频率为32K) */
    UCOM_RegWr(DRV_WATCHDOG_LOAD, uwLoadCnt);

    /* 打开中断屏蔽,开始计数 */
    UCOM_RegWr(DRV_WATCHDOG_CONTROL, DRV_WATCHDOG_CONTROL_ENABLE);

    /* 上锁，写任意非解锁值，此处写0x0 */
    UCOM_RegWr(DRV_WATCHDOG_LOCK, DRV_WATCHDOG_LOCK_NUM);
#endif
    return;

}


VOS_VOID DRV_WATCHDOG_RestartWdg(VOS_VOID)
{
#if 0
    /* 写入解锁值0x1ACCE551，解锁 */
    UCOM_RegWr(DRV_WATCHDOG_LOCK,    DRV_WATCHDOG_UNLOCK_NUM);

    /* 清中断，写任意值均可，此处写0x4455，也使Watchdog自动载入计数初值重新开始计数 */
    UCOM_RegWr(DRV_WATCHDOG_INTCLR,  DRV_WATCHDOG_INTCLR_NUM);

    /* 上锁，写任意非解锁值，此处写0x0 */
    UCOM_RegWr(DRV_WATCHDOG_LOCK,    DRV_WATCHDOG_LOCK_NUM);
#endif
    return;

}
VOS_VOID DRV_WATCHDOG_StopWdg(VOS_VOID)
{
#if 0
    /* 解锁 */
    UCOM_RegWr(DRV_WATCHDOG_LOCK,    DRV_WATCHDOG_UNLOCK_NUM);

    /* 计数停止 */
    UCOM_RegWr(DRV_WATCHDOG_CONTROL, DRV_WATCHDOG_CONTROL_DISABLE);

    /* 上锁 */
    UCOM_RegWr(DRV_WATCHDOG_LOCK,    DRV_WATCHDOG_LOCK_NUM);

    /* 禁止外设时钟，watchdog1 */
    UCOM_RegBitWr(DRV_WATCHDOG_CLOCK_DISABLE, UCOM_BIT25, UCOM_BIT26, 0x0);
#endif
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


