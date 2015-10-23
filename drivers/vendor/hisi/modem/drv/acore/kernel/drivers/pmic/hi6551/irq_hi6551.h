#ifndef __HI6551_IRQ_H
#define __HI6551_IRQ_H

#include <osl_types.h>
#include <linux/workqueue.h>
#include "osl_spinlock.h"

#define INT_LVL_SMART   31
/* number of interrupt*/
#define HI6551_IRQ_NR   31
#define IRQ_HI6551_NR_BASE  0

#define IRQ_HI6551_OTMP                         (IRQ_HI6551_NR_BASE + 0)
#define IRQ_HI6551_VSYS_UNDER_2P5               (IRQ_HI6551_NR_BASE + 1)
#define IRQ_HI6551_VSYS_UNDER_2P7               (IRQ_HI6551_NR_BASE + 2)
#define IRQ_HI6551_VSYS_OVER_6P0                (IRQ_HI6551_NR_BASE + 3)
#define IRQ_HI6551_POWER_KEY_4S_PRESS           (IRQ_HI6551_NR_BASE + 4)
#define IRQ_HI6551_POWER_KEY_20MS_RELEASE       (IRQ_HI6551_NR_BASE + 5)
#define IRQ_HI6551_POWER_KEY_20MS_PRESS         (IRQ_HI6551_NR_BASE + 6)
#define IRQ_HI6551_OCP_SCP                      (IRQ_HI6551_NR_BASE + 7)
#define IRQ_HI6551_VBUS_DET_1P375_90US          (IRQ_HI6551_NR_BASE + 8)
#define IRQ_HI6551_VBUS_DET_UNDER_0P9_3MS       (IRQ_HI6551_NR_BASE + 9)
#define IRQ_HI6551_VBUS_DET_OVER_0P9_3MS        (IRQ_HI6551_NR_BASE + 10)
#define IRQ_HI6551_ALARM_ON_A                   (IRQ_HI6551_NR_BASE + 11)
#define IRQ_HI6551_ALARM_ON_B                   (IRQ_HI6551_NR_BASE + 12)
#define IRQ_HI6551_ALARM_ON_C                   (IRQ_HI6551_NR_BASE + 13)
#define IRQ_HI6551_ALARM_ON_D                   (IRQ_HI6551_NR_BASE + 14)
#define IRQ_HI6551_ALARM_ON_E                   (IRQ_HI6551_NR_BASE + 15)
#define IRQ_HI6551_ALARM_ON_F                   (IRQ_HI6551_NR_BASE + 16)
#define IRQ_HI6551_SIM1_HPD_OUT_FALL            (IRQ_HI6551_NR_BASE + 17)
#define IRQ_HI6551_SIM1_HPD_OUT_RAISE           (IRQ_HI6551_NR_BASE + 18)
#define IRQ_HI6551_SIM1_HPD_IN_FALL             (IRQ_HI6551_NR_BASE + 19)
#define IRQ_HI6551_SIM1_HPD_IN_RAISE            (IRQ_HI6551_NR_BASE + 20)
#define IRQ_HI6551_SIM2_HPD_OUT_FALL            (IRQ_HI6551_NR_BASE + 21)
#define IRQ_HI6551_SIM2_HPD_OUT_RAISE           (IRQ_HI6551_NR_BASE + 22)
#define IRQ_HI6551_SIM2_HPD_IN_FALL             (IRQ_HI6551_NR_BASE + 23)
#define IRQ_HI6551_SIM2_HPD_IN_RAISE            (IRQ_HI6551_NR_BASE + 24)
#define IRQ_HI6551_COUL_INT                     (IRQ_HI6551_NR_BASE + 25)
#define IRQ_HI6551_COUL_OUT                     (IRQ_HI6551_NR_BASE + 26)
#define IRQ_HI6551_COUL_IN                      (IRQ_HI6551_NR_BASE + 27)
#define IRQ_HI6551_CHG_TIMER                    (IRQ_HI6551_NR_BASE + 28)
#define IRQ_HI6551_LOAD_TIMER                   (IRQ_HI6551_NR_BASE + 29)
#define IRQ_HI6551_VBAT_INT                     (IRQ_HI6551_NR_BASE + 30)

#define HI6551_IRQM2_BASE_OFFSET 7
#define HI6551_IRQM3_BASE_OFFSET 11
#define HI6551_IRQM4_BASE_OFFSET 17
#define HI6551_IRQM5_BASE_OFFSET 25

struct hi6551_irq_data{
        unsigned        irq;
        struct resource *res;
        struct clk *clk_hi6551;
        spinlock_t      irq_lock;
        struct workqueue_struct *hi6551_irq_wq;
        struct work_struct hi6551_irq_wk;
};

typedef unsigned long hi6551_irqflags_t;
typedef void (*hi6551funcptr)(void *);
/*中断处理回调函数的结构体*/
typedef struct {
    hi6551funcptr   routine;
    void *  data;
    unsigned cnt;/*中断引用计数*/
    } hi6551_irq_st;

/*错误码*/
#define HI6551_IRQ_OK       0
#define HI6551_IRQ_ERROR    -1
/*函数声明*/
void hi6551_irq_mask(unsigned int irq);
void hi6551_irq_unmask(unsigned int irq);
int hi6551_irq_is_masked(unsigned int irq);
int hi6551_irq_callback_register(unsigned int irq, hi6551funcptr routine, void *data);
#endif