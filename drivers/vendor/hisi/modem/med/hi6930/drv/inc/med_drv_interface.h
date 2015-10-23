

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "ucom_config.h"
#include "soc_baseaddr_interface.h"
#include "soc_sctrl_interface.h"
#include "drv_ipc_enum.h"
#include "drv_mailbox.h"
#include "product_config.h"

#ifndef __DRVINTERFACE_H__
#define __DRVINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define DRV_SC_CRG_BASE_ADDR            (HI_SYSCTRL_BASE_ADDR)                  /* SC基地址 */
#define DRV_SOCP_BASE_ADDR              (HI_SOCP_REGBASE_ADDR)                  /* SOCP基地址 */
#define DRV_IPC_BASE_ADDR               (SOC_IPC_S_BASE_ADDR)                   /* IPC基地址 */
#define DRV_DMA_BASE_ADDR               (SOC_AP_DMAC_BASE_ADDR)                 /* AP侧DMA基地址，待定 */
#define DRV_UART_BASE_ADDR              (SOC_UART3_BASE_ADDR)                   /* HIFI使用的UART3基地址 */
#define DRV_WATCHDOG_BASE_ADDR          (SOC_Watchdog1_BASE_ADDR)               /* HIFI使用的WDG3基地址 */

#define DRV_TIMER_DWAPB_WATCHDOG_ADDR   (0)                                     /* HIFI Watch dog 当前不可用 */
#define DRV_TIMER_DWAPB_WATCHDOGR_IDX   (0)                                     /* HIFI Watch dog 定时器在定时器设备中索引号为4 */
#define DRV_TIMER_DWAPB_WATCHDOG_FREQ   (32767U)                                /* HIFI Watch dog 时钟频率为32K */

#define DRV_TIMER_SC_SLICE_ADDR         (SOC_AO_SCTRL_SC_SLICER_COUNT0_ADDR(VOS_NULL))
#define DRV_TIMER_SC_SLICE_FREQ         (32767U)

#define DRV_TIMER_DWAPB_HIFI_ADDR       (SOC_HIFI_Timer00_BASE_ADDR)            /* V7R2中HIFI独立定时器使用TIMER0 */
#define DRV_TIMER_DWAPB_HIFI_FREQ       (32767U)                                /* V7R2中HIFI独立定时器时钟频率，单位(Hz) */
#define DRV_TIMER_UNLINK_HIFI_ADDR      (SOC_HIFI_Timer08_BASE_ADDR)            /* V7R2中PC VOICE独立定时器预留使用TIMER8 */
#define DRV_TIMER_UNLINK_HIFI_FREQ      (32767U)                                /* V7R2中HIFI独立定时器时钟频率，单位(Hz) */

#define DRV_SOCP_CHAN_START_ADDR_HIFI   ((DDR_HIFI_ADDR + 0x200000) - 0x2000) /* hifi 前2M工作区中最后8K内存作为Hifi可维可测源Buff */

#define DRV_IPC_CORE_ACPU               (MAILBOX_CPUID_ACPU)

#define DRV_TDSCDMA_CTRL_REG_BASE_ADDR   (SOC_BBP_TDS_BASE_ADDR)                           /* HIFI读取TDS-CDMA相关寄存器的基地址 */
#define DRV_TDSCDMA_CTRL_EN_ADDR         (DRV_TDSCDMA_CTRL_REG_BASE_ADDR +(0xa0)) /* TDS-CDMA帧计数寄存器中断使能地址，第0位控制 */
#define DRV_TDSCDMA_CTRL_INT_CLEAR_ADDR  (DRV_TDSCDMA_CTRL_REG_BASE_ADDR +(0xa8)) /* HIFI对TDS-CDMA清中断寄存器地址，第0位控制 */
#define DRV_TDSCDMA_FRM_CNT_REG_ADDR     (DRV_TDSCDMA_CTRL_REG_BASE_ADDR +(0x204)) /* HIFI读取TDS-CDMA帧计数寄存器的映射地址，低16位有效 */
#define DRV_TDSCDMA_CTRL_SYNC_INTR       (OS_INTR_CONNECT_07)

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


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/












#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_drv_interface.h */
