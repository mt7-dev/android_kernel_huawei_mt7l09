/* linux/drivers/char/watchdog/hi6930_wdt.c*/

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/watchdog.h>
#include <linux/platform_device.h>
#include <linux/hw_breakpoint.h>
#include <osl_types.h>
#include <hi_base.h>
#include <hi_wdt.h>
#include <hi_syssc_interface.h>
#include <hi_syscrg_interface.h>
#include <bsp_sram.h>
#include <bsp_ipc.h>
#include <bsp_wdt.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CONFIG_HI3630_CCORE_WDT


unsigned int current_timeout = 0;
wdt_timeout_cb g_wdt_rebootfunc = WDT_NULL;
struct hi6930_wdt {
	 struct resource	*irq;
	 struct clk		*clock;
};

struct wdt_control{
	/*lint --e{958,959,43}*/
	struct hi6930_wdt *wdt;
	u32 wdt_size;
	wdt_timeout_cb wdt_cb;
	struct watchdog_info info;
	struct watchdog_ops ops ;
	struct watchdog_device wdd;
	struct platform_device wpd;
	struct platform_driver plt_drv;
	spinlock_t wdt_lock;
};

#ifdef CONFIG_OF
static const struct of_device_id hi6930_wdt_match[] = {
    { .compatible = "hisilicon,hi6930-wdt" },
    {},
};
MODULE_DEVICE_TABLE(of, hi6930_wdt_match);
#else
#define hi6930_wdt_match NULL
#endif

static struct wdt_control g_wdt_ctrl={
	.wdt_cb = WDT_NULL,
};
struct wdt_info hi6930_wdt_ident;
static struct resource g_wdt_resource_init[] = {
    [0] = DEFINE_RES_IRQ(INT_LVL_TIMER3),
    [1] = DEFINE_RES_IRQ(INT_LVL_TIMER4),
};

/* functions */
static signed int hi6930_wdt_keepalive(struct watchdog_device *wdd)
{
	if(hi6930_wdt_ident.enable == BSP_FALSE)
	{
		wdt_pinfo("wdt is not enabled \n");
		return WDT_OK;
	}
	bsp_hardtimer_disable(ACORE_WDT_TIMER_ID);
	bsp_hardtimer_enable(ACORE_WDT_TIMER_ID);

    wdt_debug("exit wdt keepalive\n");
    return WDT_OK;
}

static int hi6930_wdt_stop(struct watchdog_device *wdd)
{
	s32 reg = 0;
	reg = bsp_hardtimer_disable(ACORE_WDT_TIMER_ID);
	hi6930_wdt_ident.enable = BSP_FALSE;
	/*A-M 核标志for 仿真器停狗*/
   STOP_WDT_TRACR_RUN_FLAG = 1;
	wdt_debug("exit wdt stop :%d\n", reg);
    return WDT_OK;
}


static int hi6930_wdt_start(struct watchdog_device *wdd)
{
	s32 reg = 0;
	DRV_WDT_INIT_PARA_STRU wdt_nv_param = {0};
	u32 retValue = 0;
    retValue = bsp_nvm_read(NV_ID_DRV_WDT_INIT_PARAM, (u8*)&wdt_nv_param, sizeof(DRV_WDT_INIT_PARA_STRU));
    if (NV_OK != retValue)
    {
    	wdt_err("read nv failed use default value\n");
		wdt_nv_param.wdt_enable = BSP_FALSE;
		wdt_nv_param.wdt_timeout = 30;
    }
    hi6930_wdt_ident.enable = (s32)wdt_nv_param.wdt_enable;
    hi6930_wdt_ident.my_timer.func = NULL;
    hi6930_wdt_ident.my_timer.mode = TIMER_PERIOD_COUNT;
    hi6930_wdt_ident.my_timer.timeout = WDT_HI_TIMER_CLK * wdt_nv_param.wdt_timeout;
    hi6930_wdt_ident.my_timer.timerId = ACORE_WDT_TIMER_ID;
    hi6930_wdt_ident.my_timer.unit = TIMER_UNIT_NONE;

    if (BSP_FALSE == hi6930_wdt_ident.enable)
	{
		wdt_err("wdt nv enabled??\n");
		return WDT_OK;
	}
    reg = bsp_hardtimer_start(&hi6930_wdt_ident.my_timer);
    if (reg)
    {
		wdt_err("wdt start hardtimer failed\n");
		return WDT_ERROR;
    }

    current_timeout = hi6930_wdt_ident.my_timer.timeout/WDT_HI_TIMER_CLK;

    /*A-M 核标志for 仿真器停狗*/
    STOP_WDT_TRACR_RUN_FLAG = 0;

    return WDT_OK;
}
/*输入参数单位是秒*/
static int hi6930_wdt_set_timeout(struct watchdog_device *wdd, u32 timeout)
{
	s32 reg;
	u32 value;
	value = timeout * WDT_HI_TIMER_CLK;
	hi6930_wdt_ident.my_timer.timeout = value;
	wdt_pinfo("wdt set timeout : %d\n", timeout);

	if(hi6930_wdt_ident.enable == BSP_FALSE)
	{
		wdt_err("wdt is not enabled \n");
		return WDT_OK;
	}

	reg = bsp_hardtimer_start(&hi6930_wdt_ident.my_timer);
    if (reg)
    {
		wdt_err("wdt start hardtimer failed\n");
		return WDT_ERROR;
    }
	wdt_debug("exit wdt set timeout: %d\n", value);
	wdd->timeout = timeout;
	current_timeout = timeout;
	return WDT_OK;
}

static u32 hi6930_wdt_get_timeleft(struct watchdog_device *wdd)
{
	u32 reg = 0;
	u32 timeleft = 0;

	reg = bsp_get_timer_current_value(ACORE_WDT_TIMER_ID);

	timeleft = reg/WDT_HI_TIMER_CLK;
	wdt_pinfo("exit wdt get timeleft: %d\n", timeleft);

	return timeleft;
}

/* interrupt handler code */
static void  hi6930_wdt_acore_irq(u32 para)
{
	s32 dscr = 0;
	/*lint --e{718, 746 } */
	ARM_DBG_READ(c1, 0, dscr);
	wdt_err("a\n");

	STOP_WDT_TRACR_RUN_FLAG = 0;

	/* Ensure that halting mode is disabled. */
	if (dscr & ARM_DSCR_HDBGEN) {

		STOP_WDT_TRACR_RUN_FLAG = 1;
	}
	else
	{
		 STOP_WDT_TRACR_RUN_FLAG = 0;
	}
	wdt_err("a:%d flag:%d\n", dscr & ARM_DSCR_HDBGEN, STOP_WDT_TRACR_RUN_FLAG);
	return ;
}

static int hi6930_acpu_timer_init(void)
{
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
	hi6930_wdt_ident.lowtaskid = 0;
	hi6930_wdt_ident.enable = (s32)wdt_nv_param.wdt_enable;
	hi6930_wdt_ident.wdt_timeout = wdt_nv_param.wdt_timeout;

	hi6930_wdt_ident.my_timer.func = NULL;
    hi6930_wdt_ident.my_timer.mode = TIMER_PERIOD_COUNT;
    hi6930_wdt_ident.my_timer.timeout = WDT_HI_TIMER_CLK * wdt_nv_param.wdt_timeout;
    hi6930_wdt_ident.my_timer.timerId = ACORE_WDT_TIMER_ID;
    hi6930_wdt_ident.wdt_suspend_timeout = wdt_nv_param.wdt_suspend_timerout;

    wdt_pinfo("starting watchdog timer\n");
    hi6930_wdt_start(&g_wdt_ctrl.wdd);

    return WDT_OK;
}
static int __devinit hi6930_wdt_probe(struct platform_device *pdev)
{
    int ret = 0;

	g_wdt_ctrl.wdt_size = WDT_NUM;
	g_wdt_ctrl.wdt = kmalloc(sizeof(struct hi6930_wdt) * g_wdt_ctrl.wdt_size, GFP_KERNEL);
	if(NULL == g_wdt_ctrl.wdt)
	{
		wdt_err("no memory to malloc\n");
        return -ENOENT;
	}

    g_wdt_ctrl.wdt[WDT_INDEX].irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (g_wdt_ctrl.wdt[WDT_INDEX].irq == NULL) {
        wdt_err("no irq resource specified\n");
        ret = -ENOENT;
        goto err;
    }

	if (bsp_ipc_int_connect(IPC_ACPU_INT_SRC_MCPU_WDT,hi6930_wdt_acore_irq,0))
	{
		wdt_err("bsp_ipc_int_connect failed\n");
		goto err;
	}
	if (bsp_ipc_int_enable(IPC_ACPU_INT_SRC_MCPU_WDT))
	{
		wdt_err("bsp_ipc_int_enable failed\n");
		goto err;
	}
    watchdog_set_nowayout(&g_wdt_ctrl.wdd, WATCHDOG_NOWAYOUT);/*lint !e747 */

    ret = watchdog_register_device(&g_wdt_ctrl.wdd);
    if (ret) {
        wdt_err("cannot register watchdog (%d)\n", ret);
        goto err_irq;
    }

	hi6930_acpu_timer_init();
    wdt_err("wdt init ok\n");
    return ret;

err_irq:

err:
    g_wdt_ctrl.wdt[WDT_INDEX].irq = NULL;
	kfree(g_wdt_ctrl.wdt);
    return ret;
}


static int __devexit hi6930_wdt_remove(struct platform_device *dev)
{
    int ret = 0;
    watchdog_unregister_device(&g_wdt_ctrl.wdd);

    clk_disable(g_wdt_ctrl.wdt[WDT_INDEX].clock);
    clk_put(g_wdt_ctrl.wdt[WDT_INDEX].clock);
    g_wdt_ctrl.wdt[WDT_INDEX].clock = NULL;
    g_wdt_ctrl.wdt[WDT_INDEX].irq = NULL;

    wdt_err("exit wdt remove\n");
    return ret;
}

static void hi6930_wdt_shutdown(struct platform_device *dev)
{
    hi6930_wdt_stop(&g_wdt_ctrl.wdd);
}


#ifdef CONFIG_PM
static int __devexit balong_wdt_suspend(struct device *dev)
{
	int ret = 0;
	bsp_wdt_suspend(ret);
	return WDT_OK;
}
static int __devexit balong_wdt_resume(struct device *dev)
{
    bsp_wdt_resume();
	return WDT_OK;
}

static const struct dev_pm_ops balong_wdt_dev_pm_ops ={
    .suspend = balong_wdt_suspend,
    .resume = balong_wdt_resume,
};

#define BALONG_DEV_PM_OPS (&balong_wdt_dev_pm_ops)
#else
#define BALONG_DEV_PM_OPS NULL
#endif

#define OPTIONS (WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE)
int __init hi6930_wdt_init(void)
{
    g_wdt_ctrl.info.options = OPTIONS;
    g_wdt_ctrl.info.firmware_version = 0;
    memcpy(g_wdt_ctrl.info.identity, "HI6930 Watchdog", sizeof("HI6930 Watchdog"));

	g_wdt_ctrl.ops.owner = THIS_MODULE;
    g_wdt_ctrl.ops.start = hi6930_wdt_start;
    g_wdt_ctrl.ops.stop = hi6930_wdt_stop;
    g_wdt_ctrl.ops.ping = hi6930_wdt_keepalive;
    g_wdt_ctrl.ops.set_timeout = hi6930_wdt_set_timeout;
    g_wdt_ctrl.ops.get_timeleft = hi6930_wdt_get_timeleft;

    g_wdt_ctrl.wdd.info = &g_wdt_ctrl.info;
    g_wdt_ctrl.wdd.ops = &g_wdt_ctrl.ops;

    g_wdt_ctrl.wpd.name = "hi6930-wdt";
    g_wdt_ctrl.wpd.id = -1;
    g_wdt_ctrl.wpd.num_resources = ARRAY_SIZE(g_wdt_resource_init);/*lint !e30 !e84 */
    g_wdt_ctrl.wpd.resource = g_wdt_resource_init;

    g_wdt_ctrl.plt_drv.probe = hi6930_wdt_probe;
    g_wdt_ctrl.plt_drv.remove = __devexit_p(hi6930_wdt_remove);
    g_wdt_ctrl.plt_drv.shutdown = hi6930_wdt_shutdown;
    //g_wdt_ctrl.plt_drv.driver.pm = BALONG_DEV_PM_OPS;
    g_wdt_ctrl.plt_drv.driver.owner = THIS_MODULE;
    g_wdt_ctrl.plt_drv.driver.name = "hi6930-wdt";
    g_wdt_ctrl.plt_drv.driver.of_match_table = hi6930_wdt_match;


    if (platform_device_register(&g_wdt_ctrl.wpd))
    {
		return WDT_ERROR;
    }

     if (platform_driver_register(&g_wdt_ctrl.plt_drv))
     {
		platform_device_unregister(&g_wdt_ctrl.wpd);
		return WDT_ERROR;
     }
     return WDT_OK;
}

static void __exit hi6930_wdt_exit(void)
{
    platform_driver_unregister(&g_wdt_ctrl.plt_drv);
    platform_device_unregister(&g_wdt_ctrl.wpd);
}



s32 bsp_wdt_start(void)
{
	hi6930_wdt_start(&g_wdt_ctrl.wdd);
	return WDT_OK;
}

s32 bsp_wdt_stop(void)
{
    hi6930_wdt_stop(&g_wdt_ctrl.wdd);
    return WDT_OK;
}

s32 bsp_wdt_keepalive(void)
{
    hi6930_wdt_keepalive(&g_wdt_ctrl.wdd);
    return WDT_OK;
}

s32 bsp_wdt_get_timeleft(u32 *timeleft)
{
	if(WDT_NULL == timeleft)
	{
		wdt_err("timeleft is NULL\n");
		return WDT_ERROR;
	}
    *timeleft = hi6930_wdt_get_timeleft(&g_wdt_ctrl.wdd);
    return WDT_OK;
}

/*系统重启*/
void bsp_wdt_reboot_direct(void)
{
    //hi_syssc_wdt_reboot();
}

signed int bsp_wdt_register_hook(void *hook)
{
    /*参数判断，如果函数指针为空返回错误*/
    if(NULL == hook)
    {
    	wdt_err("hook is NULL\n");
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
		 wdt_err("hook is NULL\n");
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
	result = hi6930_wdt_set_timeout(&g_wdt_ctrl.wdd, timeout);
	return result;
}

s32 bsp_wdt_suspend(u32 timeout)
{
	u32 suspend_timeout = hi6930_wdt_ident.wdt_suspend_timeout;
	if (BSP_FALSE == hi6930_wdt_ident.enable)
	{
		return WDT_OK;
	}
	bsp_softtimer_add(&hi6930_wdt_ident.wdt_timer_list);
	bsp_wdt_set_timeout(suspend_timeout);
	return WDT_OK;
}

void bsp_wdt_resume(void)
{
	if (BSP_FALSE == hi6930_wdt_ident.enable)
	{
		wdt_err("wdt nv enabled??\n");
		return;
	}
    bsp_softtimer_delete(&hi6930_wdt_ident.wdt_timer_list);
	bsp_wdt_set_timeout(hi6930_wdt_ident.wdt_timeout);
}

module_init(hi6930_wdt_init);
module_exit(hi6930_wdt_exit);

/*lint --e{39 } */
MODULE_DESCRIPTION("HI6930 Watchdog Device Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
MODULE_ALIAS("platform:hi6930-wdt");


/*打印WDT 信息*/

void bsp_wdt_print_debug(void)
{
    unsigned int timeleft = 0;
    bsp_wdt_get_timeleft(&timeleft);
    wdt_err("*******************wdt debug  start*******************\n");
    wdt_err("timeleft: %d\n", timeleft);
    wdt_err("cur_timeout: %d\n", current_timeout);
    wdt_err("*******************wdt debug  end*******************\n");
}
#endif     /*end CONFIG_HI3630_CCORE_WDT*/

#ifdef __cplusplus
}
#endif
