

#ifndef __OSL_THREAD_H
#define __OSL_THREAD_H

#include "osl_common.h"

#ifndef ERROR
#define ERROR (-1)
#endif

#ifndef OK
#define OK (0)
#endif


#ifdef __KERNEL__
#include <linux/kthread.h>
#include <linux/sched.h>

static inline s32  osl_task_init(char* name, u32 priority, u32 size, void* entry, void *para, u32* task_id)
{
	struct task_struct* tsk;
	struct sched_param	sch_para;

	sch_para.sched_priority = priority;

	tsk =  kthread_run(entry, para, name);
	if (IS_ERR(tsk))
	{
		printk("create kthread %s failed!\n", name);
		return ERROR;
	}
	if (OK != sched_setscheduler(tsk, SCHED_FIFO, &sch_para))
	{
		printk("create kthread %s sched_setscheduler failed!", name);
		return ERROR;
	}
	*task_id =(u32)tsk;

	return OK;
}

#elif defined(__VXWORKS__)

#include <taskLib.h>
#include <logLib.h>


#define VX_DEFAULT_PRIORITY      150
#define VX_DEFAULT_STACK_SIZE    8192


#define kthread_run(threadfn, data, namefmt)    \
	taskSpawn(namefmt, VX_DEFAULT_PRIORITY, 0, VX_DEFAULT_STACK_SIZE, (FUNCPTR)threadfn, (int)data, 0,0,0,0,0,0,0,0,0)

#define kthread_stop(id)    \
do{     \
	taskDelete(id); \
}while(0)

static __inline__ s32 osl_task_init(char* name, u32 priority, u32 size, void* entry, void *para, u32* task_id)
{
	int tsk = taskSpawn(name ,priority,0,size,(FUNCPTR)entry,(int)para,0,0,0,0,0,0,0,0,0);
	if(ERROR == tsk)
	{
		logMsg("osl_task_init %s error\n",(int)name,0,0,0,0,0);
		return ERROR;
	}
	*task_id = (u32)tsk;
	return OK;
}

#elif defined(__CMSIS_RTOS)

#else

#endif /* __KERNEL__ */

#endif /* __OSL_THREAD_H */

