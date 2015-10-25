/*lint -save -e537*/
#include <bsp_ipc.h>
#include <drv_ipc.h>
#include <osl_bio.h>
#include <osl_irq.h>
#include <bsp_memmap.h>
/*lint -restore +e537*/
BSP_S32 BSP_DRV_IPCIntInit(void)
{
	return  0;
}
BSP_S32 BSP_IPC_SemCreate(BSP_U32 u32SignalNum)
{
	return bsp_ipc_sem_create(u32SignalNum);
}

BSP_S32 BSP_IPC_SemDelete(BSP_U32 u32SignalNum)
{
	return bsp_ipc_sem_delete(u32SignalNum);
}

BSP_S32 BSP_IPC_IntEnable (IPC_INT_LEV_E ulLvl)
{
	return bsp_ipc_int_enable(ulLvl);
}
BSP_S32 BSP_IPC_IntDisable (IPC_INT_LEV_E ulLvl)
{
	return bsp_ipc_int_disable(ulLvl);
}
BSP_S32 BSP_IPC_IntConnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter)
{
	return bsp_ipc_int_connect(ulLvl,(voidfuncptr)routine,parameter);
}

BSP_S32 BSP_IPC_IntDisonnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter)
{
	return bsp_ipc_int_disconnect(ulLvl,(voidfuncptr)routine,parameter);
}
BSP_S32 BSP_IPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl)
{
	return bsp_ipc_int_send(enDstCore, ulLvl);
}

BSP_S32 BSP_IPC_SemTake(BSP_U32 u32SignalNum,BSP_S32 s32timeout)
{
	return bsp_ipc_sem_take(u32SignalNum,s32timeout);
}
BSP_VOID BSP_IPC_SemGive(BSP_U32 u32SignalNum)
{
	(void)bsp_ipc_sem_give(u32SignalNum);
	return;
}

BSP_VOID BSP_IPC_SpinLock (BSP_U32 u32SignalNum)
{
	(void)bsp_ipc_spin_lock (u32SignalNum);
 	return;
}

BSP_VOID BSP_IPC_SpinUnLock (BSP_U32 u32SignalNum)
{
	(void)bsp_ipc_spin_unlock (u32SignalNum);
	return;
}
BSP_S32 BSP_IPC_SpinLock_IrqSave(BSP_U32 u32SignalNum, unsigned long *flags)
{
	local_irq_save(*flags);
	return  bsp_ipc_spin_lock(u32SignalNum);
	
}
BSP_S32	BSP_IPC_SpinUnLock_IrqRestore(BSP_U32 u32SignalNum,unsigned long *flags)
{
	BSP_S32 ret = 0;
	ret = bsp_ipc_spin_unlock(u32SignalNum);
	local_irq_restore(*flags);
	return ret;
}

