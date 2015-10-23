
 
 
#ifndef __OSL_IRQ_H
#define __OSL_IRQ_H
#include "product_config.h"

#ifdef __KERNEL__
#include <linux/interrupt.h>

#define OSL_IRQ_FUNC(ret, func, irq, dev)  \
	ret func(int irq, void* dev)
#define osl_free_irq(irq,routine,dev_id) free_irq(irq,(void*)dev_id)

#elif defined(__VXWORKS__)
#include <vxWorks.h>
#include <intLib.h>
#include <logLib.h>
#include <taskLib.h>

#define IRQF_NO_SUSPEND 0
typedef enum {
	IRQ_NONE,
	IRQ_HANDLED
} irqreturn_t;

typedef irqreturn_t (*irq_handler_t)(int, void *);

#ifdef ENABLE_BUILD_OM
extern  void int_lock_hook(void);
extern void  int_unlock_hook(void);
#else
static void inline int_lock_hook(void)
{
    return;
}

static void inline int_unlock_hook(void)
{
    return;
}

#endif

/*该宏传参不准传入__specific_flags名字*/
#define local_irq_save(__specific_flags)	    \
	do \
    {                           \
		__specific_flags = (unsigned long)intLock();			\
		int_lock_hook();                             \
	} while (0)

#define local_irq_restore(__specific_flags)   \
	do \
    {                           \
        int_unlock_hook();               \
        intUnlock((int)__specific_flags);          \
	} while (0)

#define OSL_IRQ_FUNC(ret, func, irq, dev)  \
	ret func(void* dev)

static __inline__ int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
	    const char *name, void *dev)
{
	int ret = intConnect((VOIDFUNCPTR*)(irq),(VOIDFUNCPTR)handler, (int)dev);
	if(ret != OK)
	{
		logMsg("intConnect irq %d error\n", irq, 0, 0, 0, 0, 0);
		return ret;
	}
	ret = intEnable((int)irq);
	if(ret != OK)
	{
		logMsg("intEnable irq %d error\n", irq, 0, 0, 0, 0, 0);
		return ret;
	}
	return ret;
}

static __inline__ void free_irq(unsigned int irq, void *dev_id)
{
	int ret = intDisable((int)irq);
	if(ret != OK)
	{
		logMsg("intDisable irq %d error\n", irq, 0, 0, 0, 0, 0);
		return;
	}
	ret = intDisconnect((VOIDFUNCPTR*)(irq), NULL, (int)dev_id);/* [false alarm]:误报 */
	if(ret != OK)
	{
		logMsg("intDisconnect irq %d error\n", irq, 0, 0, 0, 0, 0);
		return;
	}

}
static __inline__ void osl_free_irq(unsigned int irq, VOIDFUNCPTR routine,int para)
{
	int ret = intDisable(irq);
	if(ret != OK)
	{
		logMsg("intDisable irq %d error\n", irq, 0, 0, 0, 0, 0);
		return;
	}
	ret = intDisconnect((VOIDFUNCPTR*)(irq), routine, para);
	if(ret != OK)
	{
		logMsg("intDisconnect irq %d error\n", irq, 0, 0, 0, 0, 0);
		return;
	}
}

#elif defined(__CMSIS_RTOS)
#include <ARMCM3.h>

#define IRQF_NO_SUSPEND 0

#define local_irq_save(__specific_flags)	    \
	do {							\
		__specific_flags = __get_PRIMASK(); __disable_irq();			\
	} while (0)

#define local_irq_restore(__specific_flags)   \
	do {                           \
		if(!__specific_flags) __enable_irq();          \
	} while (0)

#else

#endif /* __KERNEL__ */

#endif

