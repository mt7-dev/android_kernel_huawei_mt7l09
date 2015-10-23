

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "med_drv_interface.h"


#ifndef __DRV_IPC_H__
#define __DRV_IPC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define DRV_IPC_MAX_INT_NUM         (32)                                        /*  */
#define DRV_IPC_INT_NO_HIFI         (OS_INTR_CONNECT_02)                        /* IPC互发中断连接HIFI的中断号(快速中断是2，信号量中断是3，只需要使用快速中断) */

#define DRV_IPC_CPU_RAW_INT(i)      (DRV_IPC_BASE_ADDR+(0x400+(i*0x10)))       /* CPU_i快速中断原始中断寄存器     */
#define DRV_IPC_CPU_INT_MASK(i)     (DRV_IPC_BASE_ADDR+(0x404+(i*0x10)))       /* CPU_i快速中断掩码寄存器         */
#define DRV_IPC_CPU_INT_STAT(i)     (DRV_IPC_BASE_ADDR+(0x408+(i*0x10)))       /* CPU_i屏蔽后的快速中断状态寄存器 */
#define DRV_IPC_CPU_INT_CLR(i)      (DRV_IPC_BASE_ADDR+(0x40C+(i*0x10)))       /* CPU_i快速中断清除寄存器         */
#define DRV_IPC_SEM_RAW_INT(j)      (DRV_IPC_BASE_ADDR+(0x600+(j*0x10)))       /* CPU_j信号量释放原始中断寄存器   */
#define DRV_IPC_SEM_INT_MASK(j)     (DRV_IPC_BASE_ADDR+(0x604+(j*0x10)))       /* CPU_j信号量释放中断掩码寄存器   */
#define DRV_IPC_SEM_INT_STAT(j)     (DRV_IPC_BASE_ADDR+(0x608+(j*0x10)))       /* CPU_j信号量释放中断状态寄存器   */
#define DRV_IPC_SEM_INT_CLR(j)      (DRV_IPC_BASE_ADDR+(0x60C+(j*0x10)))       /* CPU_j信号量释放中断清除寄存器   */
#define DRV_IPC_HS_CTRL(j,k)        (DRV_IPC_BASE_ADDR+(0x800+(j*0x100)+(k*0x8))) /* CPU_j信号量k请求寄存器      */
#define DRV_IPC_HS_STAT(j,k)        (DRV_IPC_BASE_ADDR+(0x804+(j*0x100)+(k*0x8))) /* CPU_j信号量k状态寄存器      */

#define DRV_IPC_CPU_RAW_INT_HIFI    (DRV_IPC_CPU_RAW_INT(IPC_CORE_HiFi))        /* HIFI快速中断原始中断寄存器     */
#define DRV_IPC_CPU_INT_ENABLE_HIFI (DRV_IPC_CPU_INT_MASK(IPC_CORE_HiFi))       /* HIFI快速中断掩码寄存器         */
#define DRV_IPC_CPU_INT_STAT_HIFI   (DRV_IPC_CPU_INT_STAT(IPC_CORE_HiFi))       /* HIFI屏蔽后的快速中断状态寄存器 */
#define DRV_IPC_CPU_INT_CLR_HIFI    (DRV_IPC_CPU_INT_CLR(IPC_CORE_HiFi))        /* HIFI快速中断清除寄存器         */

#define DRV_IPC_GetIntSrcIsrPtr()   (&g_astDrvIpcIntSrcIsr[0])                  /* 获取IPC中断处理函数全局变量首地址 */

#define DRV_IPC_MCU_CORE            (IPC_CORE_MCORE)

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
 函 数 名  : DRV_IPC_INT_FUNC
 功能描述  : IPC互发中断处理回调函数
 输入参数  : VOS_UINT32 uwPara - 回调时输入注册时提供的参数
 输出参数  : 无
 返 回 值  : VOS_VOID
*****************************************************************************/
typedef VOS_VOID (*DRV_IPC_INT_FUNC)( VOS_UINT32 uwPara);

/*****************************************************************************
 实 体 名  : DRV_IPC_INT_SRC_ISR_STRU
 功能描述  : 互发中断响应程序的函数指针和回调参数
*****************************************************************************/
typedef struct
{
    DRV_IPC_INT_FUNC    pfFunc;         /* 回调函数指针 */
    VOS_UINT32          uwPara;         /* 回调函数参数 */
} DRV_IPC_INT_SRC_ISR_STRU;

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

extern VOS_VOID DRV_IPC_Init( VOS_VOID );
extern VOS_VOID DRV_IPC_Isr( VOS_VOID );
extern VOS_VOID DRV_IPC_RegIntSrc(
                       IPC_INT_LEV_E                   enSrc,
                       DRV_IPC_INT_FUNC                pfFunc,
                       VOS_UINT32                      uwPara);
extern VOS_VOID DRV_IPC_TrigInt(
                       VOS_UINT16                  enTarget,
                       VOS_UINT16                  enIntSrc);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_drv_ipc.h */

