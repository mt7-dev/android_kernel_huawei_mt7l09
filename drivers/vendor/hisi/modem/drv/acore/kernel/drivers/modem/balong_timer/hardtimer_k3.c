
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <osl_sem.h>
#include <osl_spinlock.h>
#include <osl_module.h>
#include <soc_interrupts_app.h> /*lint !e537*/
#include <bsp_hardtimer.h>
#include <bsp_om.h>

#define DRIVER_NAME "balong_timer_device"
/*lint --e{129, 63 ,732,746} */
/*lint -save -e631*/
struct timer_ctrl
{
   timer_func routine;                    /*中断处理函数     */
   void* arg;                               /*中断处理函数参数 */
   void* base_addr;
   void* load_addr;                         /*timer的初值地址   */
   void* value_addr;                        /*timer的当前值地址   */
   void* ctrl_addr;                         /*timer的控制地址   */
   void* intclr_addr;                       /*timer的中断清除地址   */
   void* intris_addr;                       /*timer的原始中断地址   */
   void* bgload_addr;                       /*timer的周期初值地址   */
   u32 interrupt_num;                     /*timer的中断号   */
   u32 clk;                               /*timer的时钟频率 */
   u32 init_timeout;
   spinlock_t lock;
};
/*lint -restore +e631*/

static struct timer_ctrl hard_timer_control[TIMER_NUM] ={
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER0,HI_TIMER0_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER1,HI_TIMER1_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER2,HI_TIMER2_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER3,HI_TIMER3_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER4,HI_TIMER4_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER5,HI_TIMER5_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER6,HI_TIMER6_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER7,HI_TIMER7_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER8,HI_TIMER8_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER9,HI_TIMER9_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER10,HI_TIMER10_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER11,HI_TIMER11_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER12,HI_TIMER12_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER13,HI_TIMER13_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER14,HI_TIMER14_CLK,0},
		{NULL,NULL,0,0,0,0,0,0,0,INT_LVL_TIMER15,HI_TIMER15_CLK,0},
	};
/*lint -save -e14*/

void bsp_hardtimer_load_value(u32 timer_id,u32 value)
{
	writel(value,(volatile void *)hard_timer_control[timer_id].load_addr);
	hard_timer_control[timer_id].init_timeout = value;
}
u32 bsp_get_timer_current_value(u32 timer_id)
{
	u32 ret = 0;
	ret = readl((const volatile void *)hard_timer_control[timer_id].ctrl_addr);
	if(ret&0x80)
	{
		ret = readl((const volatile void *)hard_timer_control[timer_id].value_addr);
		if(ret > hard_timer_control[timer_id].init_timeout )
			return hard_timer_control[timer_id].init_timeout;
		else
			return ret;
	}
	else
		return 0;
}


u32 bsp_hardtimer_int_status(u32 timer_id)
{
	u32 ret = 0;
	ret = readl((const volatile void *)hard_timer_control[timer_id].intris_addr);
	return ret;
}

void bsp_hardtimer_int_clear(u32 timer_id)
{
	u32 stamp =0;
	writel(0x1,(volatile void *)hard_timer_control[timer_id].intclr_addr);
	stamp = bsp_get_slice_value();
	while(get_timer_slice_delta(stamp,bsp_get_slice_value())< 2){}
}
static s32 bsp_hardtimer_disable_noirq(u32 timer_id)
{
	/*最后1bit写0,关闭之前先清中断*/
	u32 ret = 0;
	ret = readl((const volatile void *)hard_timer_control[timer_id].ctrl_addr);
	writel(ret&(~0xA0),(volatile void *)hard_timer_control[timer_id].ctrl_addr);
	bsp_hardtimer_int_clear(timer_id);
	return OK;
}
s32 bsp_hardtimer_disable(u32 timer_id)
{
	unsigned long flags=0;
	s32 ret = 0;
	spin_lock_irqsave(&hard_timer_control[timer_id].lock,flags);
	ret = bsp_hardtimer_disable_noirq(timer_id);
	spin_unlock_irqrestore(&hard_timer_control[timer_id].lock,flags);
	return ret;
}

/*lint -save -e550*/

s32 bsp_hardtimer_alloc(struct bsp_hardtimer_control  *timer_ctrl)
{
	u32 readValue = 0, intLev = 0;
	s32 ret = 0;
	unsigned long flags = 0;
	void *timerAddr;

	intLev = hard_timer_control[timer_ctrl->timerId].interrupt_num;
	hard_timer_control[timer_ctrl->timerId].routine = timer_ctrl->func;
	hard_timer_control[timer_ctrl->timerId].arg = timer_ctrl->para;
	spin_lock_irqsave(&hard_timer_control[timer_ctrl->timerId].lock,flags);
	timerAddr = hard_timer_control[timer_ctrl->timerId].ctrl_addr;
	if (TIMER_ONCE_COUNT == timer_ctrl->mode)
	{
		readValue = readl((const volatile void *)timerAddr);
		writel(readValue|0x23,(volatile void *)timerAddr);
	}
	else if(TIMER_FREERUN_COUNT == timer_ctrl->mode)
	{
		readValue = readl((const volatile void *)timerAddr);
		writel(readValue|0x22,(volatile void *)timerAddr);
	}
	else if(TIMER_PERIOD_COUNT == timer_ctrl->mode)
	{
		readValue = readl((const volatile void *)timerAddr);
		writel(readValue|0x62,(volatile void *)timerAddr);
	}
	bsp_hardtimer_load_value(timer_ctrl->timerId,timer_ctrl->timeout);
	spin_unlock_irqrestore(&hard_timer_control[timer_ctrl->timerId].lock,flags);
	if(timer_ctrl->func)
	{
		ret = request_irq(intLev, (irq_handler_t)timer_ctrl->func, 0, "hard_timer_irq",(void *)(timer_ctrl->para));
		if (ret)
		{
			hardtimer_print_error("request_irq error,ret = %d\n",ret);
			return ERROR;
		}
	}
	return OK;
}

static s32 bsp_hardtimer_enable_noirq(u32 timer_id)
{
	/*lint --e{516 } */
	unsigned int ret = 0;
	(void)bsp_hardtimer_disable_noirq(timer_id);
	ret = readl((const volatile void *)hard_timer_control[timer_id].ctrl_addr);
	writel(ret|0XA0,(volatile void *)hard_timer_control[timer_id].ctrl_addr);
	return OK;
}
s32 bsp_hardtimer_enable(u32 timer_id)
{
	unsigned long flags=0;
	s32 ret = 0;
	spin_lock_irqsave(&hard_timer_control[timer_id].lock,flags);
	ret = bsp_hardtimer_enable_noirq(timer_id);
	spin_unlock_irqrestore(&hard_timer_control[timer_id].lock,flags);
	return ret;
}

/*lint -restore +e550*/

s32 bsp_hardtimer_free(u32 timer_id)
{
	u32 intLev = 0;
	(void)bsp_hardtimer_disable(timer_id);
	intLev = hard_timer_control[timer_id].interrupt_num;
	osl_free_irq(intLev,hard_timer_control[timer_id].routine, hard_timer_control[timer_id].arg);
	hard_timer_control[timer_id].routine = NULL;
	hard_timer_control[timer_id].arg = 0;
	return OK;
}
s32 bsp_hardtimer_start(struct bsp_hardtimer_control  *timer_ctrl)
{return 0;}


static int k3_timer_probe(struct platform_device *dev)
{
	u32 i = 0,ret = 0;
	/*lint -save -e64*/
	hard_timer_control[0].base_addr = ioremap_nocache(HI_TIMER_00_REGBASE_ADDR, TIMER_ADDR_SIZE);
	hard_timer_control[1].base_addr = hard_timer_control[0].base_addr;
	/*timer 2\3，即timer10\11属性为安全，AP侧不可访问*/
	/*
	hard_timer_control[2].base_addr = ioremap_nocache(HI_TIMER_02_REGBASE_ADDR,TIMER_ADDR_SIZE);
	hard_timer_control[3].base_addr = hard_timer_control[2].base_addr;
	*/
	hard_timer_control[4].base_addr = ioremap_nocache(HI_TIMER_04_REGBASE_ADDR, TIMER_ADDR_SIZE);
	hard_timer_control[5].base_addr = hard_timer_control[4].base_addr;
	hard_timer_control[6].base_addr = ioremap_nocache(HI_TIMER_06_REGBASE_ADDR, TIMER_ADDR_SIZE);
	hard_timer_control[7].base_addr = hard_timer_control[6].base_addr;
	hard_timer_control[8].base_addr = ioremap_nocache(HI_TIMER_08_REGBASE_ADDR, TIMER_ADDR_SIZE);
	hard_timer_control[9].base_addr = hard_timer_control[8].base_addr;
	hard_timer_control[10].base_addr = ioremap_nocache(HI_TIMER_10_REGBASE_ADDR, TIMER_ADDR_SIZE);
	hard_timer_control[11].base_addr = hard_timer_control[10].base_addr;
	hard_timer_control[12].base_addr = ioremap_nocache(HI_TIMER_12_REGBASE_ADDR, TIMER_ADDR_SIZE);
	hard_timer_control[13].base_addr = hard_timer_control[12].base_addr;
	hard_timer_control[14].base_addr = ioremap_nocache(HI_TIMER_14_REGBASE_ADDR, TIMER_ADDR_SIZE);
	hard_timer_control[15].base_addr = hard_timer_control[14].base_addr;
	/*lint -restore +e64*/
	for(i = 0;i < TIMER_NUM-8;i+=2)
	{
		/*timer 2\3，即timer10\11属性为安全，AP侧不可访问*/
		if(i==2)
			continue;
		hard_timer_control[i].bgload_addr = hard_timer_control[i].base_addr + 0x18;
		hard_timer_control[i].ctrl_addr = hard_timer_control[i].base_addr + 0x08;
		hard_timer_control[i].intclr_addr = hard_timer_control[i].base_addr +0x0c;
		hard_timer_control[i].intris_addr = hard_timer_control[i].base_addr + 0x14;
		hard_timer_control[i].load_addr = hard_timer_control[i].base_addr +0x00;
		hard_timer_control[i].value_addr = hard_timer_control[i].base_addr +0x04;
		hard_timer_control[i+1].bgload_addr = hard_timer_control[i+1].base_addr + 0x38;
		hard_timer_control[i+1].ctrl_addr = hard_timer_control[i+1].base_addr + 0x28;
		hard_timer_control[i+1].intclr_addr = hard_timer_control[i+1].base_addr +0x2c;
		hard_timer_control[i+1].intris_addr = hard_timer_control[i+1].base_addr + 0x34;
		hard_timer_control[i+1].load_addr = hard_timer_control[i+1].base_addr +0x20;
		hard_timer_control[i+1].value_addr = hard_timer_control[i+1].base_addr +0x24;
		spin_lock_init(&hard_timer_control[i].lock);
		spin_lock_init(&hard_timer_control[i+1].lock);
	}
	/*第8比特置一，timer才可作为唤醒源*/
	ret = readl((const volatile void *)HI_AP_SYSCTRL_BASE_ADDR_VIRT);
	writel(ret|0x100,(volatile void *)HI_AP_SYSCTRL_BASE_ADDR_VIRT);
	return 0;
}
/*lint -restore +e14*/
/*lint -save -e19*/
#ifdef CONFIG_PM
#define LP_TIMER_NUM 3
struct timer_dpm_s
{
	u32 lp_timer_valuereg[LP_TIMER_NUM];
	u32 lp_timer_ctrlreg[LP_TIMER_NUM];
};

static struct timer_dpm_s timer_dpm_ctrl = {{0},{0}};
static inline void dpm_timer_disable(void)
{
	(void)bsp_hardtimer_disable((u32)ACORE_SOFTTIMER_NOWAKE_ID);
	(void)bsp_hardtimer_disable((u32)TIMER_ACPU_CPUVIEW_ID);
	(void)bsp_hardtimer_disable((u32)TIMER_ACPU_OM_TCXO_ID);
}
static s32 hardtimer_suspend_noirq(struct device *dev)
{
	
	timer_dpm_ctrl.lp_timer_valuereg[0] = readl((const volatile void *)hard_timer_control[ACORE_SOFTTIMER_NOWAKE_ID].load_addr);
	timer_dpm_ctrl.lp_timer_valuereg[1] = readl((const volatile void *)hard_timer_control[TIMER_ACPU_CPUVIEW_ID].load_addr);
	timer_dpm_ctrl.lp_timer_valuereg[2] = bsp_get_timer_current_value((u32)TIMER_ACPU_OM_TCXO_ID);
	timer_dpm_ctrl.lp_timer_ctrlreg[0] = readl((const volatile void *)hard_timer_control[ACORE_SOFTTIMER_NOWAKE_ID].ctrl_addr);
	timer_dpm_ctrl.lp_timer_ctrlreg[1] = readl((const volatile void *)hard_timer_control[TIMER_ACPU_CPUVIEW_ID].ctrl_addr);
	timer_dpm_ctrl.lp_timer_ctrlreg[2] = readl((const volatile void *)hard_timer_control[TIMER_ACPU_OM_TCXO_ID].ctrl_addr);
	dpm_timer_disable();
	return OK;
}
static void set_timer_32bit_count(void)
{
	u32 ret = 0;
	ret = readl((const volatile void *)hard_timer_control[ACORE_SOFTTIMER_NOWAKE_ID].ctrl_addr);
	writel(ret|0x2,(volatile void *)hard_timer_control[ACORE_SOFTTIMER_NOWAKE_ID].ctrl_addr);
	ret = readl((const volatile void *)hard_timer_control[TIMER_ACPU_CPUVIEW_ID].ctrl_addr);
	writel(ret|0x2,(volatile void *)hard_timer_control[TIMER_ACPU_CPUVIEW_ID].ctrl_addr);
	ret = readl((const volatile void *)hard_timer_control[TIMER_ACPU_OM_TCXO_ID].ctrl_addr);
	writel(ret|0x2,(volatile void *)hard_timer_control[TIMER_ACPU_OM_TCXO_ID].ctrl_addr);
}
static s32 hardtimer_resume_noirq(struct device *dev)
{
	dpm_timer_disable();
	/*在load值之前，先统一配为32bit计时，load值之后，再恢复保存的控制寄存器的值*/
	set_timer_32bit_count();
	bsp_hardtimer_load_value((u32)ACORE_SOFTTIMER_NOWAKE_ID,timer_dpm_ctrl.lp_timer_valuereg[0]);
	bsp_hardtimer_load_value((u32)TIMER_ACPU_CPUVIEW_ID,timer_dpm_ctrl.lp_timer_valuereg[1]);
	bsp_hardtimer_load_value((u32)TIMER_ACPU_OM_TCXO_ID,timer_dpm_ctrl.lp_timer_valuereg[2]);
	writel(timer_dpm_ctrl.lp_timer_ctrlreg[0],(volatile void *)hard_timer_control[ACORE_SOFTTIMER_NOWAKE_ID].ctrl_addr);
	writel(timer_dpm_ctrl.lp_timer_ctrlreg[1] ,(volatile void *)hard_timer_control[TIMER_ACPU_CPUVIEW_ID].ctrl_addr);
	writel(timer_dpm_ctrl.lp_timer_ctrlreg[2] ,(volatile void *)hard_timer_control[TIMER_ACPU_OM_TCXO_ID].ctrl_addr );
	return OK;
}
static const struct dev_pm_ops balong_timer_pm_ops ={
	.suspend_noirq = hardtimer_suspend_noirq,
	.resume_noirq   = hardtimer_resume_noirq,
};
#define BALONG_DEV_PM_OPS (&balong_timer_pm_ops)
#else
#define BALONG_DEV_PM_OPS NULL
#endif
static struct platform_driver balong_timer_driver = {
	.probe = k3_timer_probe,
	.driver = {
		.name = DRIVER_NAME,
		.owner  = THIS_MODULE,
		.pm     = BALONG_DEV_PM_OPS,
	},
};
static struct platform_device balong_timer_device =
{
    .name = DRIVER_NAME,
    .id       = -1,
    .num_resources = 0,
};
static int __init hi_timer_init(void)
{
	s32 ret = 0;
	ret = platform_device_register(&balong_timer_device);
	if(ret)
	{
		hardtimer_print_error("Platform timer device register is failed!\n");
        	return ret;
	}
	ret = platform_driver_register(&balong_timer_driver);
	if (ret)
	{
		hardtimer_print_error("Platform timer deriver register is failed!\n");
		platform_device_unregister(&balong_timer_device);
		return ret;
	}
	return ret;
}
EXPORT_SYMBOL(bsp_hardtimer_int_clear);
EXPORT_SYMBOL(bsp_hardtimer_int_status);
EXPORT_SYMBOL(bsp_hardtimer_alloc);
EXPORT_SYMBOL(bsp_hardtimer_enable);
EXPORT_SYMBOL(bsp_hardtimer_disable);
arch_initcall(hi_timer_init);
/*lint -restore +e19*/

