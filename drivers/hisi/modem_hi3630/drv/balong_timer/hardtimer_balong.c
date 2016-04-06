
#include <linux/delay.h>
#include <osl_sem.h>
#include <osl_spinlock.h>
#include <soc_interrupts.h> /*lint !e537*/
#include <osl_module.h>
#include <osl_irq.h>/*lint !e537*/
#include <bsp_om.h>
#include <bsp_hardtimer.h>
#include "hardtimer_balong.h"

struct timer_ctrl
{
   timer_func routine;                    /*中断处理函数    */
   void *arg;                             /*中断处理函数参数*/
   u32 addr;                              /*timer的基地址   */
   u32 interrupt_num;                     /*timer的中断号   */
   u32 clk;                               /*timer的时钟频率 */
   u32 timeout;                           /*以1为单位记录使用timer的超时时间*/
   spinlock_t lock;
};
static struct timer_ctrl hard_timer_control[TIMER_NUM] ={
		{NULL,0,HI_TIMER_00_REGBASE_ADDR_VIRT,INT_LVL_TIMER0,HI_TIMER0_CLK,0},
		{NULL,0,HI_TIMER_01_REGBASE_ADDR_VIRT,INT_LVL_TIMER1,HI_TIMER1_CLK,0},
		{NULL,0,HI_TIMER_02_REGBASE_ADDR_VIRT,INT_LVL_TIMER2,HI_TIMER2_CLK,0},
		{NULL,0,HI_TIMER_03_REGBASE_ADDR_VIRT,INT_LVL_TIMER3,HI_TIMER3_CLK,0},
		{NULL,0,HI_TIMER_04_REGBASE_ADDR_VIRT,INT_LVL_TIMER4,HI_TIMER4_CLK,0},
		{NULL,0,HI_TIMER_05_REGBASE_ADDR_VIRT,INT_LVL_TIMER5,HI_TIMER5_CLK,0},
		{NULL,0,HI_TIMER_06_REGBASE_ADDR_VIRT,INT_LVL_TIMER6,HI_TIMER6_CLK,0},
		{NULL,0,HI_TIMER_07_REGBASE_ADDR_VIRT,INT_LVL_TIMER7,HI_TIMER7_CLK,0},
		{NULL,0,HI_TIMER_08_REGBASE_ADDR_VIRT,INT_LVL_TIMER8,HI_TIMER8_CLK,0},
		{NULL,0,HI_TIMER_09_REGBASE_ADDR_VIRT,INT_LVL_TIMER9,HI_TIMER9_CLK,0},
		{NULL,0,HI_TIMER_10_REGBASE_ADDR_VIRT,INT_LVL_TIMER10,HI_TIMER10_CLK,0},
		{NULL,0,HI_TIMER_11_REGBASE_ADDR_VIRT,INT_LVL_TIMER11,HI_TIMER11_CLK,0},
		{NULL,0,HI_TIMER_12_REGBASE_ADDR_VIRT,INT_LVL_TIMER12,HI_TIMER12_CLK,0},
		{NULL,0,HI_TIMER_13_REGBASE_ADDR_VIRT,INT_LVL_TIMER13,HI_TIMER13_CLK,0},
		{NULL,0,HI_TIMER_14_REGBASE_ADDR_VIRT,INT_LVL_TIMER14,HI_TIMER14_CLK,0},
		{NULL,0,HI_TIMER_15_REGBASE_ADDR_VIRT,INT_LVL_TIMER15,HI_TIMER15_CLK,0},
		{NULL,0,HI_TIMER_16_REGBASE_ADDR_VIRT,INT_LVL_TIMER16,HI_TIMER16_CLK,0},
		{NULL,0,HI_TIMER_17_REGBASE_ADDR_VIRT,INT_LVL_TIMER17,HI_TIMER17_CLK,0},
		{NULL,0,HI_TIMER_18_REGBASE_ADDR_VIRT,INT_LVL_TIMER18,HI_TIMER18_CLK,0},
		{NULL,0,HI_TIMER_19_REGBASE_ADDR_VIRT,INT_LVL_TIMER19,HI_TIMER19_CLK,0},
		{NULL,0,HI_TIMER_20_REGBASE_ADDR_VIRT,INT_LVL_TIMER20,HI_TIMER20_CLK,0},
		{NULL,0,HI_TIMER_21_REGBASE_ADDR_VIRT,INT_LVL_TIMER21,HI_TIMER21_CLK,0},
		{NULL,0,HI_TIMER_22_REGBASE_ADDR_VIRT,INT_LVL_TIMER22,HI_TIMER22_CLK,0},
		{NULL,0,HI_TIMER_23_REGBASE_ADDR_VIRT,INT_LVL_TIMER23,HI_TIMER23_CLK,0},
	};

static u32 schedule_time_info[]=wake_timer_array;
#define WAKE_TIMER_NUM (sizeof(schedule_time_info)/sizeof(u32))

/*lint -save -e550*/

void bsp_hardtimer_load_value(u32 timer_id, u32 value)
{
	writel(value,TIMER_LOADCOUNT(hard_timer_control[timer_id].addr));
	hard_timer_control[timer_id].timeout = value;
}

u32 bsp_get_timer_current_value(u32 timer_id)
{
	u32 ret = 0;
	ret=readl(TIMER_CONTROLREG(hard_timer_control[timer_id].addr));
	if(ret&0x1)
	{
		ret=readl(TIMER_CURRENTVALUE(hard_timer_control[timer_id].addr));
		if(ret >hard_timer_control[timer_id].timeout )
			return hard_timer_control[timer_id].timeout;
		else
			return ret;
	}
	else
	{
		return 0;
	}
}

u32 bsp_get_timer_rest_time(u32 timer_id, DRV_TIMER_UNIT unit)
{
	u32 addr = 0,ret = 0;
	addr = TIMER_CONTROLREG(hard_timer_control[timer_id].addr);
	ret = readl(addr);
	/*在使能的情况下，返回结果*/
	if(ret&0x1)
	{
		ret = bsp_get_timer_current_value(timer_id);
		if(TIMER_UNIT_NONE == unit){
			return ret;
		}
		else if(TIMER_UNIT_US == unit){
			ret=(ret*Second_To_Millisecond)/(hard_timer_control[timer_id].clk/Second_To_Millisecond);
			return ret;
		}
		else if(TIMER_UNIT_MS== unit){
			ret=ret/(hard_timer_control[timer_id].clk/Second_To_Millisecond);
			return ret;
		}
	}else
		/*如果没有使能，则返回0xFFFFFFFF*/
		return 0xFFFFFFFF;
	return OK;
}

void bsp_hardtimer_int_unmask(u32 timer_id)
{
	u32 ret = 0,addr = 0;
	addr = TIMER_CONTROLREG(hard_timer_control[timer_id].addr);
	ret = readl(addr);/* [false alarm]:误报 */
	writel(ret&(~TIMER_INT_MASK),addr);
}

void bsp_hardtimer_int_mask(u32 timer_id)
{
	u32 ret = 0,addr = 0;
	addr = TIMER_CONTROLREG(hard_timer_control[timer_id].addr);
	ret = readl(addr);/* [false alarm]:误报 */
	writel(ret|TIMER_INT_MASK,addr);
}
u32 bsp_hardtimer_int_status(u32 timer_id)
{
	u32 ret = 0;
	ret = readl(TIMER_INTSTATUS(hard_timer_control[timer_id].addr));
	return ret;
}


void bsp_hardtimer_int_clear(u32 timer_id)
{
	(void)readl(TIMER_EOI(hard_timer_control[timer_id].addr));
}
static s32 bsp_hardtimer_disable_noirq(u32 timer_id)
{
	/*最后1bit写0,关闭之前先清中断*/
	u32 ret = 0;
	ret = readl(TIMER_INTSTATUS(hard_timer_control[timer_id].addr));
	if (ret )
	{
		bsp_hardtimer_int_clear(timer_id);
	}
	ret = readl(TIMER_CONTROLREG(hard_timer_control[timer_id].addr));
	ret &= ~HARD_TIMER_ENABLE;
	ret |=TIMER_INT_MASK;/* [false alarm]:误报 */
	writel(ret,TIMER_CONTROLREG(hard_timer_control[timer_id].addr));
	do{
		ret = readl(TIMER_CONTROLREG(hard_timer_control[timer_id].addr));
	}while(ret&0x1);
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

s32 bsp_hardtimer_alloc(struct bsp_hardtimer_control  *timer_ctrl)
{
	u32 readValue = 0, intLev = 0, timerAddr = 0;
	s32 ret = 0;
	unsigned long flags=0;
	intLev = hard_timer_control[timer_ctrl->timerId].interrupt_num;
	timerAddr = hard_timer_control[timer_ctrl->timerId].addr;
	hard_timer_control[timer_ctrl->timerId].routine = timer_ctrl->func;
	hard_timer_control[timer_ctrl->timerId].arg = timer_ctrl->para;
	spin_lock_irqsave(&hard_timer_control[timer_ctrl->timerId].lock,flags);
	(void)bsp_hardtimer_disable_noirq(timer_ctrl->timerId);
	bsp_hardtimer_load_value(timer_ctrl->timerId,timer_ctrl->timeout);
	if (TIMER_ONCE_COUNT == timer_ctrl->mode||TIMER_FREERUN_COUNT == timer_ctrl->mode)/*自由模式,第2bit写0*/
	{
		readValue = readl(TIMER_CONTROLREG(timerAddr));
		readValue &= (~0x2);/* [false alarm]:误报 */
		writel(readValue,TIMER_CONTROLREG(timerAddr));
	}
	else/*周期模式,第2bit写1*/
	{
		readValue = readl(TIMER_CONTROLREG(timerAddr));
		readValue |= 0x2;/* [false alarm]:误报 */
		writel(readValue,TIMER_CONTROLREG(timerAddr));
	}
	spin_unlock_irqrestore(&hard_timer_control[timer_ctrl->timerId].lock,flags);
	if(timer_ctrl->func != NULL)
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

static s32 hardtimer_start_ms(struct bsp_hardtimer_control  *timer_ctrl)
{
	u32 timer_id = 0,timer_clk=0;
	s32 ret = 0;
	timer_id = timer_ctrl->timerId;
	timer_clk = hard_timer_control[timer_id].clk;
	if(HI_TCXO_CLK == timer_clk)
	{
		if(timer_ctrl->timeout<TCXO_CLK_MAX_LENGTH)
			timer_ctrl->timeout= HI_TCXO_CLK*timer_ctrl->timeout/1000;
		else
			timer_ctrl->timeout= HI_TCXO_CLK/1000*timer_ctrl->timeout;
	}
	else
	{
		timer_ctrl->timeout = timer_clk/1000*timer_ctrl->timeout ;
	}
	ret = bsp_hardtimer_alloc(timer_ctrl);
	if(OK!=ret){
		hardtimer_print_error("bsp_hardtimer_alloc error \n");
		return ERROR;
	}
	ret = bsp_hardtimer_enable(timer_id);
	return ret;
}

static s32 hardtimer_start_us(struct bsp_hardtimer_control  *timer_ctrl)
{
	u32 timer_id=0,timer_clk=0;
	s32 ret = 0;
	timer_id = timer_ctrl->timerId;
	timer_clk = hard_timer_control[timer_id].clk;
	if(HI_TCXO_CLK == timer_clk)
	{
		timer_ctrl->timeout= HI_TCXO_CLK/1000*timer_ctrl->timeout/1000;
	}
	else
	{
		timer_ctrl->timeout = timer_clk/1000000*timer_ctrl->timeout;
	}
	ret = bsp_hardtimer_alloc(timer_ctrl);
	if(OK!=ret){
		hardtimer_print_error("bsp_hardtimer_alloc error \n");
		return ERROR;
	}
	ret = bsp_hardtimer_enable(timer_id);
	return ret;
}
s32 bsp_hardtimer_start(struct bsp_hardtimer_control  *timer_ctrl)
{
	s32 ret = 0;
	if(NULL == timer_ctrl){
		hardtimer_print_error("timer_ctrl is NULL \n");
		return ERROR;
	}
	/*直接操作寄存器*/
	if(TIMER_UNIT_NONE==timer_ctrl->unit){
		ret = bsp_hardtimer_alloc(timer_ctrl);
		if(OK!=ret){
			hardtimer_print_error("bsp_hardtimer_alloc error \n");
			return ERROR;
		}
		ret = bsp_hardtimer_enable(timer_ctrl->timerId);
		return ret;
	}
	else if(TIMER_UNIT_MS ==timer_ctrl->unit ){
		ret = hardtimer_start_ms(timer_ctrl);
		if(OK!=ret)
			return ERROR;
		else
			return OK;
	}
	else if(TIMER_UNIT_US ==timer_ctrl->unit ){
		ret = hardtimer_start_us(timer_ctrl);
		if(OK!=ret)
			return ERROR;
		else
			return OK;
	}
	else{
		hardtimer_print_error("timer_ctrl->unit error \n");
		return ERROR;
	}
}

static s32 bsp_hardtimer_enable_noirq(u32 timer_id)
{
	u32 ret = 0;
	(void)bsp_hardtimer_disable_noirq(timer_id);
	while(readl(TIMER_CURRENTVALUE(hard_timer_control[timer_id].addr))!=(u32)0xffffffff)
	{}
	/*lint -save -e527*/
	ret = readl(TIMER_CONTROLREG(hard_timer_control[timer_id].addr));
	/*lint -restore +e527*/
	ret |= HARD_TIMER_ENABLE ;
	ret&=(~TIMER_INT_MASK);/* [false alarm]:误报 */
	writel(ret,TIMER_CONTROLREG(hard_timer_control[timer_id].addr));
	do{
		ret = readl(TIMER_CONTROLREG(hard_timer_control[timer_id].addr));
	}while((ret&0x1)!=0x1);
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

s32 bsp_hardtimer_free(u32 timer_id)
{
	u32 intLev = 0;
	(void)bsp_hardtimer_disable(timer_id);
	intLev = hard_timer_control[timer_id].interrupt_num;
	osl_free_irq(intLev,hard_timer_control[timer_id].routine,hard_timer_control[timer_id].arg);
	hard_timer_control[timer_id].routine = NULL;
	hard_timer_control[timer_id].arg = 0;
	return OK;
}
/*lint -restore +e550*/

/*获取唤醒源timer的下一个最近到时时间，供低功耗模块使用*/
u32 get_next_schedule_time(void)
{
	u32 i=0,min = 0xffffffff,ret = 0;
	for(i=0;i<WAKE_TIMER_NUM;i++)
	{
		ret = platform_timer_rest_time(schedule_time_info[i],TIMER_UNIT_MS);
		if(ret<min)
			min = ret;
	}
 	return min;
}
static int bsp_balong_timer_init(void)
{
	u32 i = 0;
	for(i = 0;i<TIMER_NUM;i++)
		spin_lock_init(&hard_timer_control[i].lock);
	return 0;
}
/*lint -save -e19*/
arch_initcall(bsp_balong_timer_init);
EXPORT_SYMBOL(bsp_hardtimer_int_clear);
EXPORT_SYMBOL(bsp_hardtimer_int_mask);
EXPORT_SYMBOL(bsp_hardtimer_int_unmask);
EXPORT_SYMBOL(bsp_hardtimer_int_status);
EXPORT_SYMBOL(bsp_hardtimer_alloc);
EXPORT_SYMBOL(bsp_hardtimer_start);
EXPORT_SYMBOL(bsp_hardtimer_enable);
EXPORT_SYMBOL(bsp_hardtimer_disable);
EXPORT_SYMBOL(bsp_hardtimer_free);
EXPORT_SYMBOL(get_next_schedule_time);
/*lint -restore +e19*/

