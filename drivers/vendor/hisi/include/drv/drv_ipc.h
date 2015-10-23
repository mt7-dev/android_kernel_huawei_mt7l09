

#ifndef __DRV_IPCM_H__
#define __DRV_IPCM_H__
#include "drv_comm.h"
#include "drv_ipc_enum.h"
/*************************IPC BEGIN**********************************/
#define INTSRC_NUM                     32

/*****************************************************************************
* 函 数 名  : BSP_DRV_IPCIntInit
*
* 功能描述  : IPC模块初始化
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无

*****************************************************************************/
BSP_S32 BSP_DRV_IPCIntInit(void);


/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMCREATE
*
* 功能描述  : 信号量创建函数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无

*****************************************************************************/
BSP_S32 BSP_IPC_SemCreate(BSP_U32 u32SignalNum);
#define DRV_IPC_SEMCREATE(u32SignalNum) BSP_IPC_SemCreate(u32SignalNum)

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMDELETE
*
* 功能描述  : 删除信号量
*
* 输入参数  :   BSP_U32 u32SignalNum 要删除的信号量编号

* 输出参数  : 无
*
* 返 回 值  : OK&ERROR

*****************************************************************************/
BSP_S32 BSP_IPC_SemDelete(BSP_U32 u32SignalNum);
#define  DRV_IPC_SEMDELETE(u32SignalNum) BSP_IPC_SemDelete(u32SignalNum)

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntEnable
*
* 功能描述  : 使能某个中断
*
* 输入参数  :
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR

*****************************************************************************/
BSP_S32 BSP_IPC_IntEnable (IPC_INT_LEV_E ulLvl);
#define  DRV_IPC_INTENABLE(ulLvl)  BSP_IPC_IntEnable(ulLvl)

/*****************************************************************************
* 函 数 名  : DRV_IPC_INTDISABLE
*
* 功能描述  : 去使能某个中断
*
* 输入参数  :
            BSP_U32 ulLvl 要使能的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR

*****************************************************************************/
BSP_S32 BSP_IPC_IntDisable (IPC_INT_LEV_E ulLvl);
#define  DRV_IPC_INTDISABLE(ulLvl) BSP_IPC_IntDisable(ulLvl)

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntConnect
*
* 功能描述  : 注册某个中断
*
* 输入参数  :
           BSP_U32 ulLvl 要使能的中断号，取值范围0～31
           VOIDFUNCPTR routine 中断服务程序
*             BSP_U32 parameter      中断服务程序参数
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR

*****************************************************************************/
BSP_S32 BSP_IPC_IntConnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);
#define DRV_IPC_INTCONNECT(ulLvl,routine,parameter) BSP_IPC_IntConnect(ulLvl,routine,parameter)

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntDisonnect
*
* 功能描述  : 取消注册某个中断
*
* 输入参数  :
*              BSP_U32 ulLvl 要使能的中断号，取值范围0～31
*              VOIDFUNCPTR routine 中断服务程序
*             BSP_U32 parameter      中断服务程序参数
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR

*****************************************************************************/
BSP_S32 BSP_IPC_IntDisonnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);

/*****************************************************************************
* 函 数 名  : DRV_IPC_INTSEND
*
* 功能描述  : 发送中断
*
* 输入参数  :
                IPC_INT_CORE_E enDstore 要接收中断的core
                BSP_U32 ulLvl 要发送的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*

*****************************************************************************/
BSP_S32 BSP_IPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);
#define  DRV_IPC_INTSEND(enDstCore, ulLvl) BSP_IPC_IntSend(enDstCore, ulLvl)

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMTAKE
*
* 功能描述  : 获取信号量
*
* 输入参数  : u32SignalNum 要获取的信号量
* 输出参数  : 无
*
* 返 回 值  : 无

*****************************************************************************/
BSP_S32 BSP_IPC_SemTake(BSP_U32 u32SignalNum,BSP_S32 s32timeout);
#define  DRV_IPC_SEMTAKE(u32SignalNum, s32timeout) BSP_IPC_SemTake(u32SignalNum, s32timeout)

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMGIVE
*
* 功能描述  : 释放信号量
*
* 输入参数  : u32SignalNum 要释放的信号量
* 输出参数  : 无
*
* 返 回 值  : 无
*
*****************************************************************************/
BSP_VOID BSP_IPC_SemGive(BSP_U32 u32SignalNum);
#define  DRV_IPC_SEMGIVE(u32SignalNum) BSP_IPC_SemGive(u32SignalNum)

/*****************************************************************************
* 函 数 名  : BSP_IPC_SpinLock
*
* 功能描述  : 获取信号量,不带锁中断功能，使用之前需要用户
						锁中断
*
* 输入参数  : u32SignalNum 要获取的信号量
*
* 返 回 值  : 无
*
*****************************************************************************/
BSP_VOID BSP_IPC_SpinLock (BSP_U32 u32SignalNum);
#define DRV_SPIN_LOCK(u32SignalNum)  BSP_IPC_SpinLock(u32SignalNum)

/*****************************************************************************
* 函 数 名  : DRV_SPIN_UNLOCK
*
* 功能描述  : 释放信号量,不带开中断功能，与BSP_IPC_SpinLock配套使用
					使用后开中断
*
* 输入参数  : u32SignalNum 要释放的信号量
* 输出参数  : 无
*
* 返 回 值  : 无
*
*****************************************************************************/
BSP_VOID BSP_IPC_SpinUnLock (BSP_U32 u32SignalNum);
#define DRV_SPIN_UNLOCK(u32SignalNum) BSP_IPC_SpinUnLock(u32SignalNum)

BSP_S32 BSP_IPC_SpinLock_IrqSave(BSP_U32 u32SignalNum,unsigned long *flags);
BSP_S32 BSP_IPC_SpinUnLock_IrqRestore(BSP_U32 u32SignalNum,unsigned long *flags);

/*****************************************************************************
* 函 数 名  : DRV_SPIN_LOCK_IRQSAVE
*
* 功能描述  : 获取信号量,带锁中断功能
* 输入参数  : u32SignalNum 要获取的信号量
					   flags:锁中断标记，本宏的使用参考spin_lock_irqsave用法，
					   本宏与DRV_SPIN_UNLOCK_IRQRESTORE配对,只能在同一个函数
					   内部前后被调用
* 输出参数  : 无
*
* 返 回 值  : 
*****************************************************************************/

#define DRV_SPIN_LOCK_IRQSAVE(u32SignalNum,flags) 		BSP_IPC_SpinLock_IrqSave(u32SignalNum, &flags)
/*****************************************************************************
* 函 数 名  : DRV_SPIN_UNLOCK_IRQRESTORE
*
* 功能描述  : 释放获取的信号量,带恢复中断功能
* 输入参数  : u32SignalNum 要释放的信号量
						flags:锁中断标记，参考spin_lock_irqrestore用法，本函数与
					   DRV_SPIN_LOCK_IRQSAVE配对,只能在同一个函数内部
					   前后被调用
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
*****************************************************************************/

#define DRV_SPIN_UNLOCK_IRQRESTORE(u32SignalNum,flags) 		BSP_IPC_SpinUnLock_IrqRestore(u32SignalNum,&flags);

BSP_S32 BSP_SGI_Connect(BSP_U32 ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);
BSP_S32 BSP_SGI_IntSend(BSP_U32 ulLvl);

/*************************IPC END************************************/

#endif

