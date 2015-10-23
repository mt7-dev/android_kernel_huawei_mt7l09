

#include <osl_types.h>
#include "pm.h"
#include <osl_bio.h>
#include <bsp_om.h>
#include <bsp_hardtimer.h>
#include <hi_timer.h>
#include <soc_interrupts.h>

#ifdef PM_TEST_WSRC
/* 启用 timer ( 使用 timer 作为wsrc ) */
// a-timer11 0x90004xxx   c-timer20  0x90004050
OSL_IRQ_FUNC(irqreturn_t, timer11_irq, intLvl, dev)
{
    u32 ret = 0;
    u32 t11_addr = HI_TIMER_11_REGBASE_ADDR_VIRT;
    ret = readl(t11_addr+HI_TIMER_EOI_OFFSET); /*clear int*/
    pm_printk(BSP_LOG_LEVEL_ERROR,"acore timer11 irq handled : 0x%x \n",ret);
    return IRQ_HANDLED;
}
static u32 timer_irq_requested = 0;
int timer_wsrc_test_init(u32 sec)
{
    u32 t11_addr = HI_TIMER_11_REGBASE_ADDR_VIRT;
    if(timer_irq_requested)
    {
        free_irq( INT_LVL_TIMER11 , 0);
    }
    /* 初始值 */
    *(u32*)(t11_addr+HI_TIMER_LOADCOUNT_OFFSET) = HI_TIMER11_CLK*sec;
    /*  config */
    *(u32*)(t11_addr+HI_TIMER_CONTROLREG_OFFSET) = 3;
    if(request_irq(INT_LVL_TIMER11, (irq_handler_t)timer11_irq, 0, "timer11", 0))
    {
        pm_printk(BSP_LOG_LEVEL_ERROR, "request_irq INT_LVL_TIMER11 - FAILED! \n");
        return -1;
    }
    timer_irq_requested = 1;
    return 0;
}
#endif

