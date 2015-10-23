


#ifndef __OSL_COMMON_H__
#define __OSL_COMMON_H__

#include <product_config.h>

#if defined(__KERNEL__) || defined(__VXWORKS__) /* acore/ccore */
#include <osl_types.h>
#include <osl_bio.h>
#include <osl_irq.h>

#elif defined(__CMSIS_RTOS)                     /* mcore */
#include <ARMCM3.h>

#ifdef OS_K3V3_USE_LPM3_API
#include <hisi_lpm3.h>
#include <common.h>
#include <log.h>
#include <string.h>
#include <stddef.h>
#include <m3_modem.h>
#include <hwspinlock.h>
#ifndef UNUSED
#define UNUSED(a) (a=a)
#endif

typedef enum {
	IRQ_NONE,
	IRQ_HANDLED
} irqreturn_t;

typedef enum {
	false	= 0,
	true	= 1
}bool;

#define request_irq(irq_no, irq_handler, flags, name, dev) request_irq(irq_no, irq_handler)
#define free_irq(irq_no, dev_id)                           put_irq(irq_no)
#define enable_irq(irq_no)								   IRQEN(irq_no)
#define disable_irq(irq_no) 							   IRQDIS(irq_no)
#define printk(fmt, args...)                               PRINT_ERR(MODEM_LOG_ID, fmt, ##args)

static inline unsigned int get_irq()
{
	return __get_IPSR() & 0xFF;
}

#define local_irq_save(flags)	    \
do {							\
	flags = __get_PRIMASK(); __disable_irq();			\
} while (0)

#define local_irq_restore(flags)   \
do {                           \
	if(!flags) __enable_irq();          \
} while (0)

#ifndef DDR_WIN_ADDR
#define DDR_WIN_ADDR(offset)	(0x10000000 + (offset))
#endif

/*begin:list*/
#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((u32) &((TYPE *)0)->MEMBER)
#endif
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) 			\
	 ((type *)((char *)(ptr) - offsetof(type,member)))


/*end:list*/
#else  /* !OS_K3V3_USE_LPM3_API */
#include <osl_types.h>
#include <osl_bio.h>
#include <osl_irq.h>
#include <irq.h>
#include <printk.h>
#include <libc.h>

#define __ao_data
#define __ao_func

#define MODEM_ALREADY_INIT_MAGIC    0x5a5a5a5a
static inline u32 get_modem_init_flag(void){return 0;}


#endif  /* OS_K3V3_USE_LPM3_API */

#endif  /* acore/ccore/mcore */

#endif /* __OSL_COMMON_H__ */

