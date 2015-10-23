
#ifdef __cplusplus
extern "C" {
#endif
#include <cmsis_os.h>
#include <product_config.h>
#include <osl_common.h>


#ifdef CONFIG_CCORE_WDT
/*lint --e{537, 762 } */
#include <soc_interrupts_m3.h>
#include <bsp_wdt.h>
#include <hi_wdt.h>
#include <bsp_memmap.h>
#include "wdt_balong.h"

#ifndef CONFIG_HI3630_CCORE_WDT
#include <osl_irq.h>
#include <bsp_sram.h>
#include <bsp_ipc.h>
#include <hi_timer.h>
#include <irq.h>
#include <printk.h>
#include "wdt_balong.h"
#include <drv_onoff.h>

struct wdt_info hi6930_wdt_ident;

unsigned int current_timeout = 0;


unsigned long start_time = 0;

static struct wdt_control g_wdt_ctrl={
	.wdt_cb = WDT_NULL,
};
wdt_timeout_cb g_wdt_rebootfunc = WDT_NULL;

s32 bsp_wdt_start(void);
static int hi6930_wdt_stop(void);
static int hi6930_wdt_keepalive(void);
static int hi6930_wdt_set_timeout(u32 timeout);
static u32 hi6930_wdt_get_timeleft(void);
s32 bsp_wdt_suspend(u32 timeout);
void bsp_wdt_resume(void);
s32 bsp_wdt_keepalive(void);

void bsp_wdt_feed(void)
{
	u32 end_time = bsp_get_slice_value();
	if (get_timer_slice_delta(start_time, end_time) >= (WDT_KEEPALIVE_TIME * WDT_DEF_CLK_FREQ))
	{
		start_time = bsp_get_slice_value();
		(void)bsp_wdt_keepalive();
	}
}
/* functions */
static int hi6930_wdt_keepalive(void)
{
	int irqlock = 0;
	local_irq_save(irqlock);
	/* 寄存器解锁 */
	writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
	/*中断清除*/
	writel(0x0, HI_WDT_BASE_ADDR_VIRT + HI_WDG_INTCLR_OFFSET);
	/*给Watchdog上锁*/
	writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);

	local_irq_restore(irqlock);

	return WDT_OK;
}

static int hi6930_wdt_stop(void)
{
    int irqlock = 0;
    local_irq_save(irqlock);
    /* 寄存器解锁 */
    writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
    writel(0x0, HI_WDT_BASE_ADDR_VIRT + HI_WDG_CONTROL_OFFSET);
    /*给Watchdog上锁*/
    writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
    local_irq_restore(irqlock);

    return WDT_OK;
}

void wdt_timer_event(u32 agrv)
{
	if (BSP_FALSE == hi6930_wdt_ident.enable)
	{
		wdt_err("wdt nv enabled??\n");
		return;
	}
	bsp_softtimer_add(&hi6930_wdt_ident.wdt_timer_list);
	//(void)bsp_wdt_keepalive();
}

static int hi6930_wdt_start(void)
{
    int irqlock = 0;
    DRV_WDT_INIT_PARA_STRU wdt_nv_param = {0};
	s32 retValue = -1;
    retValue = (s32)bsp_nvm_read(NV_ID_DRV_WDT_INIT_PARAM, (u8*)&wdt_nv_param, sizeof(DRV_WDT_INIT_PARA_STRU));
    if (NV_OK != retValue)
    {
        wdt_err("read nv failed use default value\n");
		wdt_nv_param.wdt_enable = 0;
		wdt_nv_param.wdt_timeout = 30;
		wdt_nv_param.wdt_suspend_timerout = 120;
    }
	
	hi6930_wdt_ident.enable = (s32)wdt_nv_param.wdt_enable;
	hi6930_wdt_ident.wdt_timeout = wdt_nv_param.wdt_timeout;
	hi6930_wdt_ident.wdt_suspend_timeout = wdt_nv_param.wdt_suspend_timerout;
	
	hi6930_wdt_ident.wdt_timer_list.func = ( softtimer_func )wdt_timer_event;
	hi6930_wdt_ident.wdt_timer_list.para = 0;
	hi6930_wdt_ident.wdt_timer_list.timeout = WDT_KEEPALIVE_TIME * 1000;
	hi6930_wdt_ident.wdt_timer_list.wake_type = SOFTTIMER_WAKE;

	retValue= bsp_softtimer_create(&hi6930_wdt_ident.wdt_timer_list);
	if (retValue)
    	{
		printk("bsp_softtimer_create failed\n");
		return WDT_ERROR;
    	}
	
	
	retValue = (s32)(WDT_DEF_CLK_FREQ * hi6930_wdt_ident.wdt_timeout);/*lint !e713*/
    if (BSP_FALSE == hi6930_wdt_ident.enable)
    {
		printk("nv enabled? %d\n", hi6930_wdt_ident.enable);
		return WDT_OK;
    }
    bsp_softtimer_add(&hi6930_wdt_ident.wdt_timer_list);
    local_irq_save(irqlock);
    
    hi_syssc_wdt_enble();
    writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
    writel((u32)retValue, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOAD_OFFSET);
    writel(WDT_RST_INT_EN, HI_WDT_BASE_ADDR_VIRT + HI_WDG_CONTROL_OFFSET);
    writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
	current_timeout = (u32)retValue / WDT_DEF_CLK_FREQ;
    local_irq_restore(irqlock);

    return WDT_OK;
}

static int hi6930_wdt_set_timeout(u32 timeout) /*输入参数单位是秒*/
{
    int irqlock = 0;
    local_irq_save(irqlock);
    writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);

    writel(timeout * WDT_DEF_CLK_FREQ, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOAD_OFFSET);

    writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);

    local_irq_restore(irqlock);
    current_timeout = timeout;
    return WDT_OK;
}

static u32 hi6930_wdt_get_timeleft(void)
{
	u32 reg = 0;
	u32 timeleft = 0;
	int irqlock = 0;
	
	local_irq_save(irqlock);
	
	writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
	reg = readl(HI_WDT_BASE_ADDR_VIRT + HI_WDG_VALUE_OFFSET);
	writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
	
	local_irq_restore(irqlock);

	timeleft = reg/WDT_DEF_CLK_FREQ;

	return timeleft;
}

/* interrupt handler code */
static irqreturn_t  hi6930_wdt_irq(int irqno, void*param)
{
    (void)disable_irq(M3_WDT_INT);
	printk("mcore wdt irq \n");
	if(g_wdt_ctrl.wdt_cb)
	{
		g_wdt_ctrl.wdt_cb(MCORE_WDT_TIMEOUT);
	}
	return IRQ_HANDLED;
}

/*模拟硬狗复位*/
unsigned int acore_wdt_flag = 0;
unsigned int ccore_wdt_flag = 0;
static irqreturn_t  hi6930_acore_wdt_irq(int irqno, void*param)
{
	/*clear timer3 interrupt*/
	readl(HI_TIMER_03_REGBASE_ADDR_VIRT + HI_TIMER_EOI_OFFSET);
	printk("acore irq\n");
	if (0 == acore_wdt_flag)
	{
		acore_wdt_flag++;
		bsp_ipc_int_send(IPC_CORE_ACORE,IPC_ACPU_INT_SRC_MCPU_WDT);
		if(g_wdt_ctrl.wdt_cb)
		{
			g_wdt_ctrl.wdt_cb(ACORE_WDT_TIMEOUT);
		}
	}
	else if ((0 ==  ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_WDT_AM_FLAG) 
				&& acore_wdt_flag != 0)
	{
		acore_wdt_flag = 0;
		bsp_wdt_reboot_direct();
	}

	return IRQ_HANDLED;
}


static irqreturn_t  hi6930_ccore_wdt_irq(int irqno, void* param)
{
	/*clear timer4 interrupt*/
	readl(HI_TIMER_04_REGBASE_ADDR_VIRT + HI_TIMER_EOI_OFFSET);
	printk("ccore irq\n");
	if (0 == ccore_wdt_flag)
	{
		ccore_wdt_flag++;
		bsp_ipc_int_send(IPC_CORE_CCORE,IPC_CCPU_INT_SRC_MCPU_WDT);
		if(g_wdt_ctrl.wdt_cb)
		{
			g_wdt_ctrl.wdt_cb(CCORE_WDT_TIMEOUT);
		}
	}
	else if ((0 ==  ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_WDT_CM_FLAG) /*是否有仿真器连接*/
					&& (ccore_wdt_flag != 0))
	{
		ccore_wdt_flag = 0;
		bsp_wdt_reboot_direct();
	}
	return IRQ_HANDLED;
}


s32 hi6930_wdt_init(void)
{
   int ret = 0;

    ret = request_irq(M3_WDT_INT, hi6930_wdt_irq, 0, "wdt mcore irq", NULL);
    if (ret != 0) {
        printk("failed to install mcore irq (%d)\n", ret);
    }
    ret = request_irq(M3_TIMER3_INT, hi6930_acore_wdt_irq, 0, "wdt acore irq", NULL);
    if (ret != 0) {
        printk("failed to install acore irq (%d)\n", ret);
    }
    ret = request_irq(M3_TIMER4_INT, hi6930_ccore_wdt_irq, 0, "wdt ccore irq", NULL);
    if (ret != 0) {
        printk("failed to install ccore irq (%d)\n", ret);
    }
    bsp_wdt_start();
    printk("wdt init ok\n");
    return WDT_OK;
}


s32 bsp_wdt_start(void)
{
	start_time = bsp_get_slice_value();
	hi6930_wdt_start();
	return WDT_OK;
}

s32 bsp_wdt_stop(void)
{
    hi6930_wdt_stop();
    return WDT_OK;
}

s32 bsp_wdt_keepalive(void)
{
    hi6930_wdt_keepalive();
    return WDT_OK;
}

s32 bsp_wdt_get_timeleft(u32 *timeleft)
{
	if(WDT_NULL == timeleft)
	{
		printk("timeleft is NULL\n");
		return WDT_ERROR;
	}
    *timeleft = hi6930_wdt_get_timeleft();
    return WDT_OK;
}

/*系统重启*/
void bsp_wdt_reboot_direct(void)
{
    bsp_drv_power_reboot_direct();
}

signed int bsp_wdt_register_hook(void *hook)
{
    if(NULL == hook)
    {
    	printk("hook is NULL\n");
       return WDT_ERROR;
    }

	g_wdt_ctrl.wdt_cb = hook;
    return WDT_OK;
}

signed int  bsp_wdt_unregister_hook()
{
	g_wdt_ctrl.wdt_cb = NULL;
	return WDT_OK;
}
s32 bsp_wdt_reboot_register_hook(void *hook)
{
    if(WDT_NULL == hook)
    {
        return WDT_ERROR;
    }
	g_wdt_rebootfunc = hook;
	
	return WDT_OK;
}

s32  bsp_wdt_reboot_unregister_hook(void)
{
	g_wdt_rebootfunc = WDT_NULL;
	
	return WDT_OK;
}

s32 bsp_wdt_set_timeout(u32 timeout)
{
	s32 result;
	result = hi6930_wdt_set_timeout(timeout);
	return result;
}

s32 bsp_wdt_suspend(u32 timeout)
{
	u32 suspend_timeout = hi6930_wdt_ident.wdt_suspend_timeout;
	
	if (BSP_FALSE == hi6930_wdt_ident.enable)
	{
		return WDT_OK;
	}
	bsp_softtimer_delete_sync(&hi6930_wdt_ident.wdt_timer_list);
	bsp_softtimer_modify(&hi6930_wdt_ident.wdt_timer_list, 
										(suspend_timeout - WDT_KEEPALIVE_TIME) * 1000);
	bsp_softtimer_add(&hi6930_wdt_ident.wdt_timer_list);
	bsp_wdt_set_timeout(suspend_timeout);
	return WDT_OK;
}

void bsp_wdt_resume(void)
{
	if (BSP_FALSE == hi6930_wdt_ident.enable)
	{
		return;
	}
	bsp_softtimer_delete_sync(&hi6930_wdt_ident.wdt_timer_list);
	bsp_softtimer_modify(&hi6930_wdt_ident.wdt_timer_list, hi6930_wdt_ident.wdt_timer_list.timeout);
	bsp_softtimer_add(&hi6930_wdt_ident.wdt_timer_list);
    bsp_wdt_set_timeout(hi6930_wdt_ident.wdt_timeout);
}


void bsp_wdt_stop_awdt(void)
{
	u32 ret = 0;
	if (BSP_FALSE == hi6930_wdt_ident.enable)
    {
		return;
    }
	ret = readl(HI_TIMER_CONTROLREG_OFFSET + HI_TIMER_03_REGBASE_ADDR_VIRT);
	ret &= ~HARD_TIMER_ENABLE;
	ret |=TIMER_INT_MASK;
	writel(ret, HI_TIMER_CONTROLREG_OFFSET + HI_TIMER_03_REGBASE_ADDR_VIRT);
}
void bsp_wdt_restart_awdt(void)
{
	u32 ret = 0;
	if (BSP_FALSE == hi6930_wdt_ident.enable)
    {
		return;
    }
	ret = readl(HI_TIMER_CONTROLREG_OFFSET + HI_TIMER_03_REGBASE_ADDR_VIRT);
	ret |= HARD_TIMER_ENABLE ;
	ret&=(~TIMER_INT_MASK);
	writel(ret,HI_TIMER_CONTROLREG_OFFSET + HI_TIMER_03_REGBASE_ADDR_VIRT);
}
void bsp_wdt_stop_cwdt(void)
{
	u32 ret = 0;
	if (BSP_FALSE == hi6930_wdt_ident.enable)
    {
		return;
    }
	ret = readl(HI_TIMER_CONTROLREG_OFFSET + HI_TIMER_04_REGBASE_ADDR_VIRT);
	ret &= ~HARD_TIMER_ENABLE;
	ret |=TIMER_INT_MASK;
	writel(ret, HI_TIMER_CONTROLREG_OFFSET + HI_TIMER_04_REGBASE_ADDR_VIRT);
}
void bsp_wdt_restart_cwdt(void)
{
	u32 ret = 0;
	if (BSP_FALSE == hi6930_wdt_ident.enable)
    {
		return;
    }
	ret = readl(HI_TIMER_CONTROLREG_OFFSET + HI_TIMER_04_REGBASE_ADDR_VIRT);
	ret |= HARD_TIMER_ENABLE ;
	ret&=(~TIMER_INT_MASK);
	writel(ret,HI_TIMER_CONTROLREG_OFFSET + HI_TIMER_04_REGBASE_ADDR_VIRT);
}
void bsp_wdt_print_debug(void)
{
    unsigned int timeleft = 0;
    bsp_wdt_get_timeleft(&timeleft);
    printk("*******************wdt debug  start*******************\n");
    printk("timeleft: %d\n", timeleft);
    printk("cur_timeout: %d\n", current_timeout);
    timeleft = readl(HI_WDT_BASE_ADDR_VIRT + HI_WDG_CONTROL_OFFSET);
    printk("ctr_reg: 0x%x\n", timeleft);
    timeleft = readl(HI_WDT_BASE_ADDR_VIRT + HI_WDG_VALUE_OFFSET);
    printk("wdt_value: 0x%x\n", timeleft);
    printk("*******************wdt debug  end*******************\n"); 
}

#elif defined(CONFIG_HI3630_CCORE_WDT)

#include <m3_modem.h>


s32 bsp_wdt_stop(void)
{
	writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
    writel(0x0, HI_WDT_BASE_ADDR_VIRT + HI_WDG_CONTROL_OFFSET);
    /*给Watchdog上锁*/
    writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
	return WDT_OK;
}
s32 bsp_wdt_set_timeout(u32 timeout)
{
	s32 retValue = -1;
	retValue = WDT_DEF_CLK_FREQ * timeout;
    writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
    	/*中断清除*/
	writel(0x0, HI_WDT_BASE_ADDR_VIRT + HI_WDG_INTCLR_OFFSET);
    writel(retValue, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOAD_OFFSET);
    writel(WDT_RST_INT_EN, HI_WDT_BASE_ADDR_VIRT + HI_WDG_CONTROL_OFFSET);
    writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);

    return WDT_OK;
}
extern u32 get_modem_init_flag(void);

__ao_data int hi6930_wdt_timeout = 30;
__ao_data int hi6930_wdt_enable = 0;
void bsp_wdt_stop_cwdt(void)
{
	if (BSP_FALSE == hi6930_wdt_enable)
    {
		return;
    }
	(void)bsp_wdt_stop();
}
void bsp_wdt_restart_cwdt(void)
{
	
	if (BSP_FALSE == hi6930_wdt_enable)
    {
		return;
    }
	 (void)bsp_wdt_set_timeout(hi6930_wdt_timeout);
}

s32 hi6930_wdt_init(void)
{
	//添加初始化判断
	s32 retValue = -1;
	DRV_WDT_INIT_PARA_STRU wdt_nv_param = {0};
	if (MODEM_ALREADY_INIT_MAGIC != get_modem_init_flag())
	{
	    retValue = bsp_nvm_read(NV_ID_DRV_WDT_INIT_PARAM, (u8*)&wdt_nv_param, sizeof(DRV_WDT_INIT_PARA_STRU));
	    if (NV_OK != retValue)
	    {
	       printk("read nv failed use default value\n");
			wdt_nv_param.wdt_enable = 0;
			wdt_nv_param.wdt_timeout = 30;
	    }
	    /*读取系统控制寄存器0x33c bit[0]为0打开看门狗，1关闭看门狗*/
	    retValue = readl(HI_AP_SYSCTRL_BASE_ADDR + HI_WDG_SYSCTRL_ENABLE_OFFSET) & 0x1;
		/*NV 、寄存器共同判别是否可开狗*/
		if ((0 == retValue) && (wdt_nv_param.wdt_enable != 0))
		{
			wdt_nv_param.wdt_enable = 1;
		}
		else
		{
			wdt_nv_param.wdt_enable = 0;
		}
		hi6930_wdt_enable = wdt_nv_param.wdt_enable;
		hi6930_wdt_timeout = wdt_nv_param.wdt_timeout;
	}

   	return 0;
}

void bsp_wdt_enable(void)
{
	s32 timeout = 600;
	if (BSP_FALSE == hi6930_wdt_enable)
    {
		return;
    }
	(void)bsp_wdt_set_timeout(timeout);
}
void bsp_wdt_feed(void)
{
	
}

 s32 bsp_wdt_suspend(u32 timeout)
 {
	return WDT_OK;
 }
 void bsp_wdt_resume(void)
 {
 }
 
void bsp_wdt_stop_awdt(void)
{
	
}
void bsp_wdt_restart_awdt(void)
{
	
}

#endif     /*end CONFIG_HI3630_CCORE_WDT*/

#else       /* !CONFIG_CCORE_WDT   此分支为打桩*/

#ifndef WDT_OK
#define WDT_OK                   			(0)
#endif
#ifndef WDT_ERROR
#define WDT_ERROR                			(-1)
#endif

s32 hi6930_wdt_init(void)
{
	return WDT_OK;
}
s32 bsp_wdt_start(void)
{
    return WDT_OK;
}

s32 bsp_wdt_stop(void)
{
	return WDT_OK;
}
void bsp_wdt_enable(void)
{

}
/*****************************************************************************
* 函 数 名  : BSP_WDT_HardwareFeed
*
* 功能描述  : 重置看门狗计数寄存器（喂狗）
*
* 输入参数  : BSP_U8 u8WdtId :看门狗ID
* 输出参数  : 无
*
* 返 回 值  : WDT_OK:    操作成功
*             WDT_ERROR: 操作失败
* 其它说明  :
*
*****************************************************************************/
s32 bsp_wdt_keepalive(void)
{
    return WDT_OK;
}


/*****************************************************************************
* 函 数 名  : BSP_WDT_TimerReboot
*
* 功能描述  : 通过设置timer中断，通知mcore重启。
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : WDT_OK:    操作成功
*             WDT_ERROR: 操作失败
* 其它说明  :
*
*****************************************************************************/

s32 bsp_wdt_set_timeout(u32 timeout)/*s*/
{
	return WDT_OK;
}
 s32 bsp_wdt_get_timeleft(u32 *timeleft)/*s*/
{
	return WDT_OK;
}
 s32 bsp_wdt_register_hook(void *hook)
{
	return WDT_OK;
}

s32  bsp_wdt_unregister_hook(void)
{
	return WDT_OK;
}

 s32 bsp_wdt_suspend(u32 timeout)
 {
	return WDT_OK;
 }
 void bsp_wdt_resume(void)
 {
 }
 
void bsp_wdt_stop_awdt(void)
{
	
}
void bsp_wdt_restart_awdt(void)
{
	
}
void bsp_wdt_stop_cwdt(void)
{
	
}
void bsp_wdt_restart_cwdt(void)
{
	
}
void bsp_wdt_feed(void)
{

}
void bsp_wdt_print_debug(void)
{

}
#endif   /* end of CONFIG_CCORE_WDT */

#ifdef __cplusplus
}
#endif
