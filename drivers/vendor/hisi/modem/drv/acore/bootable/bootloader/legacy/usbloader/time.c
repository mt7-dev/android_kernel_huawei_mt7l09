#include <boot/boot.h>
#include <balongv7r2/config.h>
#include <balongv7r2/time.h>
#include <soc_clk.h>
void udelay(unsigned int us)
{
#ifndef BSP_CONFIG_EDA
    while(us--) {
		unsigned long t = (CFG_CLK_CPU/4)/1000000;
		while(t--)
			__asm__ __volatile__("nop");
	}
#endif
}

void mdelay(unsigned int ms)
{
#ifndef BSP_CONFIG_EDA
	/*select timer5,32KHZ to realize mdelay.*/
	unsigned int timer_value_now  = 0;
	unsigned int delay_time = 32*ms*1024/1000;
	unsigned int timer_value_base = readl(FASTBOOT_TIMER_BASE + FASTBOOT_TIMER_CURRENTVALUE);

	for(;;)
	{
		timer_value_now = readl(FASTBOOT_TIMER_BASE + FASTBOOT_TIMER_CURRENTVALUE);
		#ifdef CONFIG_USE_TIMER_STAMP
		if((timer_value_base - timer_value_now) >= delay_time)
		{
			break;
		}
		#else
		if((timer_value_now - timer_value_base) >= delay_time)
		{
			break;
		}
		#endif
	}
#endif
}

unsigned int get_timer_value(void)
{
	return readl(FASTBOOT_TIMER_BASE+FASTBOOT_TIMER_CURRENTVALUE);
}

unsigned int get_passed_ms(unsigned int timer_value_base)
{
	unsigned int timer_value_current,timer_value_delta;
	timer_value_current = get_timer_value();
	#ifdef CONFIG_USE_TIMER_STAMP
	timer_value_delta = timer_value_base - timer_value_current ;
	#else
	timer_value_delta =  timer_value_current - timer_value_base;
	#endif
	return ((timer_value_delta*1000)>>15);
}
unsigned int get_elapse_ms()
{
	unsigned int tmp;
	unsigned int timer_get = 0;
	timer_get = get_timer_value();
	#ifdef CONFIG_USE_TIMER_STAMP
	timer_get = (unsigned int)0xFFFFFFFF - timer_get;
	#endif
	tmp = (unsigned int)(timer_get & 0xFFFFFFFF);
	tmp = tmp/(HI_TIMER5_CLK/1000);
	return (unsigned int)tmp;
}


int init_system_timer(void)
{
    unsigned int reg = 0;
	#ifdef CONFIG_USE_TIMER_STAMP
	/*使能时间戳timer*/
    writel(0xFFFFFFFF,FASTBOOT_TIMER_BASE+FASTBOOT_TIMER_LOADCOUNT);
    reg = 0x5;
    writel(reg,FASTBOOT_TIMER_BASE+FASTBOOT_TIMER_CONTROLREG);
	#endif
	/*使能udelay timer*/
    writel(0xFFFFFFFF,UDELAY_TIMER_BASE+FASTBOOT_TIMER_LOADCOUNT);
    reg = 0x5;
    writel(reg,UDELAY_TIMER_BASE+FASTBOOT_TIMER_CONTROLREG);
    return 0;
}

void sleep(unsigned int sleep_ms)
{
	unsigned int timer_value_base,passed_ms = 0;

	timer_value_base = get_timer_value();

	while(sleep_ms > passed_ms)
	{
		passed_ms = get_passed_ms(timer_value_base);
	}
}
static void set_timer_rate()
{
		/*切频率前先关闭时钟，timer6,12-16，其他为32K*/
	writel(0x7C100,HI_SYSCRG_BASE_ADDR+0x1C);
	writel(0x4017040,HI_SYSCRG_BASE_ADDR+0X144);
	/*打开时钟*/
	writel(0x7C100,HI_SYSCRG_BASE_ADDR+0x18);
}
int timer_init(void)
{
#ifndef BSP_EDA
	set_timer_rate();
	return (init_system_timer());
#endif
}

