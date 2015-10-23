#ifndef _OSL_WAIT_H
#define _OSL_WAIT_H

#ifdef __KERNEL__
#include <linux/wait.h>


typedef int (*osl_wait_func_t)(void* data);

struct osl_wait_data
{
	void* task;
	osl_wait_func_t func;
	void* data;
};

int try_to_wake_up(struct task_struct *p, unsigned int state, int wake_flags);

static inline int osl_wait_wake_function(wait_queue_t *curr, unsigned mode, int wake_flags,
			  void *key)
{
	int ret = 0;
	struct osl_wait_data* data = (struct osl_wait_data*)curr->private;

	if(data->func(data->data))
		ret =  try_to_wake_up((struct task_struct *)data->task, mode, wake_flags);

	if (ret)
		list_del_init(&curr->task_list);
	return ret;
}

#define __wait_event_timeout_func(wq, ret, condition_func, condition_data)			\
do {			\
	struct osl_wait_data __private_data = {              \
		.task = current,	\
		.func = condition_func,	\
		.data = condition_data,		\
		};\
	wait_queue_t __wait = {						\
		.private	= &__private_data,				\
		.func		= osl_wait_wake_function,				\
		.task_list	= LIST_HEAD_INIT((__wait).task_list),	\
		};			\
									\
	for (;;) {							\
		prepare_to_wait(&wq, &__wait, TASK_UNINTERRUPTIBLE);	\
		if (condition_func(condition_data))						\
			break;						\
		ret = schedule_timeout(ret);				\
		if (!ret)						\
			break;						\
	}								\
	finish_wait(&wq, &__wait);					\
} while (0)

#define wait_event_timeout_func(wq, timeout, condition_func, condition_data)			\
{									\
	long __ret = timeout;						\
	if (!(condition_func(condition_data))) 						\
		__wait_event_timeout_func(wq, __ret, condition_func, condition_data);		\
	__ret=__ret;								\
}

#elif defined(_WRS_KERNEL) || defined(__VXWORKS__) ||  defined(VXWORKS)

#include <vxWorks.h>
#include <vsbConfig.h>
#include <classLib.h>
#include <errno.h>
#include <taskLib.h>
#include <intLib.h>
#include <errnoLib.h>
#include <eventLib.h>
#include <qLib.h>
#include <common.h>

typedef int (*osl_wait_func_t)(void* data);
typedef SEM_ID wait_queue_head_t;
#define MAX_SCHEDULE_TIMEOUT WAIT_FOREVER

struct osl_wait_data
{
	void* task;
	osl_wait_func_t func;
	void* data;
    Q_NODE qNode;		/* multiway q node: rdy/pend q */
};


wait_queue_head_t osl_waitqueue_create(int options);

STATUS osl_waitqueue_wakeup(wait_queue_head_t qhead);

STATUS osl_waitqueue_wait(wait_queue_head_t qhead, int timeout, struct osl_wait_data* entry);


#define init_waitqueue_head(q)				\
	do {						\
		*(q) = osl_waitqueue_create(SEM_Q_PRIORITY);	\
	} while (0)

#define wait_event_timeout_func(wq, timeout, condition_func, condition_data)			\
{		\
	STATUS __ret = OK;	\
	struct osl_wait_data __wait;		\
	__wait.data = condition_data;	\
	__wait.func = condition_func;	\
	__wait.task = taskIdCurrent;	\
	\
	__ret = osl_waitqueue_wait(wq, timeout, &__wait);	\
	if(ERROR == __ret && errnoGet() == S_objLib_OBJ_TIMEOUT)	\
    	__ret = 0; /* timeout */	\
}

#define wake_up(x)	\
	do{ \
		(void)osl_waitqueue_wakeup(*(x));	\
		/*if(OK != __ret){BUG();};*/	\
	}while(0)

#elif defined(__CMSIS_RTOS)
#define  MAX_SCHEDULE_TIMEOUT         (0xff)
#else

#endif /* __KERNEL__ */

#endif
