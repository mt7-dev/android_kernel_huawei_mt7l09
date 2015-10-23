#ifndef __HI6451_IRQ_H
#define __HI6451_IRQ_H

#include <osl_types.h>
#include <linux/workqueue.h>
#include "osl_spinlock.h"

/* number of interrupt*/
#define HI6451_IRQ_NR   19
#define IRQ_HI6451_NR_BASE  0

#define IRQ_HI6451_OTMP                         (IRQ_HI6451_NR_BASE + 0)
#define IRQ_HI6451_HRESET                       (IRQ_HI6451_NR_BASE + 1)
#define IRQ_HI6451_PWRINI_UNDER_2P7             (IRQ_HI6451_NR_BASE + 2)
#define IRQ_HI6451_PWRINI_OVER_6P0_3MS                 (IRQ_HI6451_NR_BASE + 3)
#define IRQ_HI6451_PWRINI_OVER_6P0_100MS                 (IRQ_HI6451_NR_BASE + 4)
#define IRQ_HI6451_POWER_KEY_1SS_PRESS          (IRQ_HI6451_NR_BASE + 5)
#define IRQ_HI6451_POWER_KEY_20MS_RELEASE       (IRQ_HI6451_NR_BASE + 6)
#define IRQ_HI6451_POWER_KEY_20MS_PRESS         (IRQ_HI6451_NR_BASE + 7)

#define IRQ_HI6451_OCP                  (IRQ_HI6451_NR_BASE + 8)
#define IRQ_HI6451_VIN_LDO_OVER_2P5                 (IRQ_HI6451_NR_BASE + 9)
#define IRQ_HI6451_USB_UNDER_4P0                    (IRQ_HI6451_NR_BASE + 10)
#define IRQ_HI6451_USB_OVER_4P0                 (IRQ_HI6451_NR_BASE + 11)
#define IRQ_HI6451_USB_OVER_6P0                 (IRQ_HI6451_NR_BASE + 12)
#define IRQ_HI6451_VBUS_UNDER_4P0           (IRQ_HI6451_NR_BASE + 13)
#define IRQ_HI6451_VBUS_OVER_4P0_3MS            (IRQ_HI6451_NR_BASE + 14)
#define IRQ_HI6451_VBUS_OVER_4P0_100MS          (IRQ_HI6451_NR_BASE + 15)

#define IRQ_HI6451_ALARM_ON_A                   (IRQ_HI6451_NR_BASE + 16)
#define IRQ_HI6451_ALARM_ON_B                   (IRQ_HI6451_NR_BASE + 17)
#define IRQ_HI6451_ALARM_ON_C                   (IRQ_HI6451_NR_BASE + 18)


#define HI6451_IRQM1_BASE_OFFSET 8
#define HI6451_IRQM2_BASE_OFFSET 16


struct hi6451_irq_data{
        unsigned        irq;
        struct resource *res;
        struct clk *clk_hi6451;
        spinlock_t      irq_lock;
        struct workqueue_struct *hi6451_irq_wq;
        struct work_struct hi6451_irq_wk;
};

typedef unsigned long hi6451_irqflags_t;
typedef void (*hi6451funcptr)(void *);
/*中断处理回调函数的结构体*/
typedef struct {
    hi6451funcptr   routine;
    void *  data;
    } hi6451_irq_st;

/*错误码*/
#define HI6451_IRQ_OK       0
#define HI6451_IRQ_ERROR    -1
/*函数声明*/
int hi6451_irq_callback_register(unsigned int irq, hi6451funcptr routine, void *data);
#endif