

#ifndef __OSL_SEM_H
#define __OSL_SEM_H

#include "osl_common.h"

#ifdef __KERNEL__
#include <linux/semaphore.h>

typedef struct semaphore osl_sem_id;

static inline void osl_sem_init(u32 val, osl_sem_id* sem)
{
    sema_init(sem, val);
}

static inline void osl_sem_up(osl_sem_id* sem)
{
	up(sem);
}

static inline void osl_sem_down(osl_sem_id* sem)
{
	//down(sem);
	while(down_interruptible(sem)!=0);
}

static inline int osl_sem_downtimeout(osl_sem_id* sem, long jiffies)
{
	return down_timeout(sem, jiffies);
}

static inline s32 osl_sema_delete(osl_sem_id*sem)
{
	return 0;
}

static inline s32 sema_delete(struct semaphore *sem)
{
	return 0;
}


#elif defined(__VXWORKS__)
#include <semLib.h>

typedef SEM_ID osl_sem_id;

static __inline__ void osl_sem_init(u32 val, osl_sem_id* sem)
{
	if(val == 0)
	{
		*sem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
	}
	else
	{
		*sem = semBCreate(SEM_Q_FIFO, SEM_FULL);
	}
}

static __inline__ void osl_sem_up(osl_sem_id* sem)
{
	semGive(*sem);
}

static __inline__ void osl_sem_down(osl_sem_id* sem)
{
	semTake(*sem, WAIT_FOREVER);
}

static __inline__ int osl_sem_downtimeout(osl_sem_id* sem, long jiffies)
{
	return semTake(*sem, jiffies);
}

static __inline__ s32 osl_sema_delete(osl_sem_id*sem)
{
	return semDelete(*sem);
}

static __inline__ void sema_init(struct semaphore *sem, int val)
{
	if(val == 0)
	{
		sem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
	}
	else
	{
		sem = semBCreate(SEM_Q_FIFO, SEM_FULL);
	}
}

static __inline__ void up(struct semaphore *sem)
{
	semGive(sem);
}

static __inline__ void down(struct semaphore *sem)
{
	semTake(sem, WAIT_FOREVER);
}

static __inline__ int down_timeout(struct semaphore *sem, long jiffies)
{
	return semTake(sem, jiffies);
}

static __inline__ s32 sema_delete(struct semaphore *sem)
{
	return semDelete(sem);
}

#elif defined(__CMSIS_RTOS)

#else

#endif /* __KERNEL__ */

#endif

