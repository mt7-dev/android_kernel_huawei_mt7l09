
#include <soc_interrupts_mdm.h>
#include <osl_spinlock.h>
#include <osl_module.h>
#include <bsp_om.h>
#include <bsp_hardtimer.h>
#include "hardtimer_k3.h"
/*lint -save -e631*/

struct timer_ctrl
{
   timer_func routine;                    /*中断处理函数     */
   int arg;                               /*中断处理函数参数 */
   u32 base_addr;
   u32 load_addr;                         /*timer的初值地址   */
   u32 value_addr;                        /*timer的当前值地址   */
   u32 ctrl_addr;                         /*timer的控制地址   */
   u32 intclr_addr;                       /*timer的中断清除地址   */
   u32 intris_addr;                       /*timer的原始中断地址   */
   u32 bgload_addr;                       /*timer的周期初值地址   */
   u32 interrupt_num;                     /*timer的中断号   */
   u32 clk;                               /*timer的时钟频率 */
   u32 init_timeout;
   spinlock_t lock;
};
/*lint -restore +e631*/

static struct timer_ctrl hard_timer_control[TIMER_NUM] ={
		{NULL,0,HI_TIMER_00_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER0,HI_TIMER0_CLK,0},
		{NULL,0,HI_TIMER_01_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER1,HI_TIMER1_CLK,0},
		{NULL,0,HI_TIMER_02_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER2,HI_TIMER2_CLK,0},
		{NULL,0,HI_TIMER_03_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER3,HI_TIMER3_CLK,0},
		{NULL,0,HI_TIMER_04_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER4,HI_TIMER4_CLK,0},
		{NULL,0,HI_TIMER_05_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER5,HI_TIMER5_CLK,0},
		{NULL,0,HI_TIMER_06_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER6,HI_TIMER6_CLK,0},
		{NULL,0,HI_TIMER_07_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER7,HI_TIMER7_CLK,0},
		{NULL,0,HI_TIMER_08_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER8,HI_TIMER8_CLK,0},
		{NULL,0,HI_TIMER_09_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER9,HI_TIMER9_CLK,0},
		{NULL,0,HI_TIMER_10_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER10,HI_TIMER10_CLK,0},
		{NULL,0,HI_TIMER_11_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER11,HI_TIMER11_CLK,0},
		{NULL,0,HI_TIMER_12_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER12,HI_TIMER12_CLK,0},
		{NULL,0,HI_TIMER_13_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER13,HI_TIMER13_CLK,0},
		{NULL,0,HI_TIMER_14_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER14,HI_TIMER14_CLK,0},
		{NULL,0,HI_TIMER_15_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER15,HI_TIMER15_CLK,0},
		{NULL,0,HI_TIMER_16_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER16,HI_TIMER16_CLK,0},
		{NULL,0,HI_TIMER_17_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER17,HI_TIMER17_CLK,0},
		{NULL,0,HI_TIMER_18_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER18,HI_TIMER18_CLK,0},
		{NULL,0,HI_TIMER_19_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER19,HI_TIMER19_CLK,0},
		{NULL,0,HI_TIMER_20_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER20,HI_TIMER20_CLK,0},
		{NULL,0,HI_TIMER_21_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER21,HI_TIMER21_CLK,0},
		{NULL,0,HI_TIMER_22_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER22,HI_TIMER22_CLK,0},
		{NULL,0,HI_TIMER_23_REGBASE_ADDR,0,0,0,0,0,0,INT_LVL_TIMER23,HI_TIMER23_CLK,0},
	};

void bsp_hardtimer_load_value_k3(u32 timer_id,u32 value)
{
	writel(value,hard_timer_control[timer_id].load_addr);
	hard_timer_control[timer_id].init_timeout = value;
}
u32 bsp_get_timer_current_value_k3(u32 timer_id)
{
	u32 ret = 0;
	ret = readl(hard_timer_control[timer_id].ctrl_addr);
	if(ret&0x80)
	{
		ret = readl(hard_timer_control[timer_id].value_addr);
		if(ret > hard_timer_control[timer_id].init_timeout )
			return hard_timer_control[timer_id].init_timeout;
		else
			return ret;
	}
	else
		return 0;
}

u32 bsp_get_timer_rest_time_k3(u32 timer_id, DRV_TIMER_UNIT unit)
{
	u32 ret = 0;
	ret = readl(hard_timer_control[timer_id].ctrl_addr);
	/*在使能的情况下，返回结果*/
	if(ret&0x80)
	{
		ret = bsp_get_timer_current_value_k3(timer_id);
		if(TIMER_UNIT_NONE == unit){
			return ret;
		}
		else if(TIMER_UNIT_US == unit){
			ret=(ret*1000)/(hard_timer_control[timer_id].clk/1000);
			return ret;
		}
		else if(TIMER_UNIT_MS== unit){
			ret=ret/(hard_timer_control[timer_id].clk/1000);
			return ret;
		}
	}else
		/*如果没有使能，则返回0xFFFFFFFF*/
		return 0xFFFFFFFF;
	return OK;
}

u32 bsp_hardtimer_int_status_k3(u32 timer_id)
{
	u32 ret = 0;
	ret = readl(hard_timer_control[timer_id].intris_addr);
	return ret;
}

void bsp_hardtimer_int_clear_k3(u32 timer_id)
{
	writel(0x1,hard_timer_control[timer_id].intclr_addr);
}

static s32 bsp_hardtimer_disable_k3_noirq(u32 timer_id)
{
	/*最后1bit写0,关闭之前先清中断*/
	u32 ret = 0;
	ret = bsp_hardtimer_int_status_k3(timer_id);
	if (ret )
	{
		bsp_hardtimer_int_clear_k3(timer_id);
	}
	ret = readl(hard_timer_control[timer_id].ctrl_addr);
	writel(ret&(~0x80),hard_timer_control[timer_id].ctrl_addr);
	return OK;
}
s32 bsp_hardtimer_disable_k3(u32 timer_id)
{
	s32 ret = 0;
	unsigned long flags = 0;
	spin_lock_irqsave(&hard_timer_control[timer_id].lock,flags);
	ret = bsp_hardtimer_disable_k3_noirq(timer_id);
	spin_unlock_irqrestore(&hard_timer_control[timer_id].lock,flags);
	return ret;
}

s32 bsp_hardtimer_alloc_k3(struct bsp_hardtimer_control  *timer_ctrl)
{
	u32 readValue = 0, intLev = 0, timerAddr = 0;
	s32 ret = 0;
	unsigned long flags = 0;
	intLev = hard_timer_control[timer_ctrl->timerId].interrupt_num;
	hard_timer_control[timer_ctrl->timerId].routine = timer_ctrl->func;
	hard_timer_control[timer_ctrl->timerId].arg = (int)timer_ctrl->para;
	spin_lock_irqsave(&hard_timer_control[timer_ctrl->timerId].lock,flags);
	(void)bsp_hardtimer_disable_k3_noirq(timer_ctrl->timerId);
	timerAddr = hard_timer_control[timer_ctrl->timerId].ctrl_addr;	
	if (TIMER_ONCE_COUNT == timer_ctrl->mode)
	{
		readValue = readl(timerAddr);
		writel(readValue|0x23,timerAddr);
	}
	else if(TIMER_FREERUN_COUNT == timer_ctrl->mode)
	{
		readValue = readl(timerAddr);
		writel(readValue|0x22,timerAddr);
	}
	else if(TIMER_PERIOD_COUNT == timer_ctrl->mode)
	{
		readValue = readl(timerAddr);
		writel(readValue|0x62,timerAddr);
	}
	bsp_hardtimer_load_value_k3(timer_ctrl->timerId,timer_ctrl->timeout);
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

static s32 bsp_hardtimer_enable_k3_noirq(u32 timer_id)
{
	u32 ret = 0;
	(void)bsp_hardtimer_disable_k3_noirq(timer_id);
	ret = readl(hard_timer_control[timer_id].ctrl_addr);
	writel(ret|(~0xFFFFFF7F),hard_timer_control[timer_id].ctrl_addr);
	return OK;
}
s32 bsp_hardtimer_enable_k3(u32 timer_id)
{
	s32 ret = 0;
	unsigned long flags = 0;
	spin_lock_irqsave(&hard_timer_control[timer_id].lock,flags);
	ret = bsp_hardtimer_enable_k3_noirq(timer_id);
	spin_unlock_irqrestore(&hard_timer_control[timer_id].lock,flags);
	return ret;
}


/*lint -save -e64 -e119*/

s32 bsp_hardtimer_free_k3(u32 timer_id)
{
	u32 intLev = 0;
	(void)bsp_hardtimer_disable_k3(timer_id);
	intLev = hard_timer_control[timer_id].interrupt_num;/* [false alarm]:误报 */
	osl_free_irq(intLev,hard_timer_control[timer_id].routine,hard_timer_control[timer_id].arg);
	hard_timer_control[timer_id].routine = NULL;
	hard_timer_control[timer_id].arg = 0;
	return OK;
}
/*lint -restore +e64 +e119*/
/*lint -save -e661 -e662*/
void k3_timer_init()
{
	u32 i = 0;
	u32 regvalue = 0;
	/*modem timer set clk,0/3/5/6/7 32KHZ*/
	/*disable clk*/
	writel(0x74800000,HI_SYSCTRL_BASE_ADDR_VIRT+0x28);
	regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT+0x144);
	writel(regvalue&(~0xe9),HI_SYSCTRL_BASE_ADDR_VIRT+0x144);
	/*enable clk*/
	writel(0x74800000,HI_SYSCTRL_BASE_ADDR_VIRT+0x24);
	/*初始化K3的timer*/
	for(i = 8;i < TIMER_NUM;i+=2)
	{
		/*k3 timer 0/2/4/6/8/10/12/14*/
			hard_timer_control[i].bgload_addr = hard_timer_control[i].base_addr + 0x18;
			hard_timer_control[i].ctrl_addr = hard_timer_control[i].base_addr + 0x08;
			hard_timer_control[i].intclr_addr = hard_timer_control[i].base_addr +0x0c;
			hard_timer_control[i].intris_addr = hard_timer_control[i].base_addr + 0x14;
			hard_timer_control[i].load_addr = hard_timer_control[i].base_addr +0x00;
			hard_timer_control[i].value_addr = hard_timer_control[i].base_addr +0x04;
		/*k3 timer 1/3/5/7/9/11/13/15*/
			hard_timer_control[i+1].bgload_addr = hard_timer_control[i+1].base_addr + 0x38;
			hard_timer_control[i+1].ctrl_addr = hard_timer_control[i+1].base_addr + 0x28;
			hard_timer_control[i+1].intclr_addr = hard_timer_control[i+1].base_addr +0x2c;
			hard_timer_control[i+1].intris_addr = hard_timer_control[i+1].base_addr + 0x34;
			hard_timer_control[i+1].load_addr = hard_timer_control[i+1].base_addr +0x20;
			hard_timer_control[i+1].value_addr = hard_timer_control[i+1].base_addr +0x24;
			spin_lock_init(&hard_timer_control[i].lock);
			spin_lock_init(&hard_timer_control[i+1].lock);
	}
	/*对于modem侧使用AP侧的三个常开区timer，需要多做一次关闭和清中断操作，否则单独复位后会有异常*/
	bsp_hardtimer_disable_k3(TIMER_CCPU_DSP_DRX_PROT_ID);
	bsp_hardtimer_disable_k3(TIMER_CCPU_OSA_ID);
	bsp_hardtimer_disable_k3(TIMER_CCPU_DRX_TIMER_ID);	
}
/*lint -restore +e661 +e662*/


