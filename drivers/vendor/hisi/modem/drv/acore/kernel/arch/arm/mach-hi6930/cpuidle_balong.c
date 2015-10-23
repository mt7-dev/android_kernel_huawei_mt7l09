
/*lint --e{537}*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cpuidle.h>
#include <linux/cpu_pm.h>
#include <linux/io.h>
#include <linux/export.h>
#include <linux/time.h>
#include <asm/proc-fns.h>
#include <asm/smp_scu.h>
#include <asm/suspend.h>
#include <asm/unified.h>
#include <linux/io.h>
#include <bsp_memmap.h>
#include "sleep.h"
#include <linux/irqflags.h>
#include <bsp_lowpower_mntn.h>
#include "cpuidle_balong.h"
static int hi6930_enter_idle(struct cpuidle_device *dev, struct cpuidle_driver *drv, int index);
static int hi6930_enter_lowpower(struct cpuidle_device *dev, struct cpuidle_driver *drv, int index);

static struct cpuidle_state hi6930_cpuidle_set[] __initdata = {
	[0] = {
		.enter			= hi6930_enter_idle,
		.exit_latency		= 1,
		.target_residency	= 100000,
		.flags			= CPUIDLE_FLAG_TIME_VALID,
		.name			= "C0",
		.desc			= "ARM clock gating(WFI)",
	},
	[1] = {
		.enter			= hi6930_enter_lowpower,
		.exit_latency		= 100,
		.target_residency	= 100000,
		.flags			= CPUIDLE_FLAG_TIME_VALID,
		.name			= "C1",
		.desc			= "ARM power down",
	},
};
/*lint -save -e49*//*lint -save -e601*/
/*lint --e{752,808}*/
static DEFINE_PER_CPU(struct cpuidle_device, hi6930_cpuidle_device);

static struct cpuidle_driver hi6930_idle_driver = {
	.name		= "hi6930_idle",
	.owner		= THIS_MODULE,
};

static int hi6930_enter_deepsleep(struct cpuidle_device *dev,
				struct cpuidle_driver *drv,
				int index)
{
	struct timeval before, after;
	int idle_time;
	unsigned long flags =0;

	local_irq_save(flags);
	do_gettimeofday(&before);

	//cpu_pm_enter();
	//cpu_suspend(0, idle_finisher);
 	cpu_enter_idle();
	//cpu_pm_exit();

	do_gettimeofday(&after);
	local_irq_restore(flags);
	local_irq_enable();
	idle_time = (after.tv_sec - before.tv_sec) * USEC_PER_SEC +
		    (after.tv_usec - before.tv_usec);

	dev->last_residency = idle_time;
	return index;
}


/*WFI enter function*/
static int hi6930_enter_idle(struct cpuidle_device *dev, struct cpuidle_driver *drv, int index)
{
	struct timeval before, after;
	int idle_time;
	unsigned long flags =0;

	local_irq_save(flags);

	do_gettimeofday(&before);

	cpu_enter_idle();

	do_gettimeofday(&after);

	local_irq_restore(flags);
	local_irq_enable();
	idle_time = (after.tv_sec - before.tv_sec) * USEC_PER_SEC + (after.tv_usec - before.tv_usec);

	dev->last_residency = idle_time;
	return index;
}

/*deepsleep enter function*/
static int hi6930_enter_lowpower(struct cpuidle_device *dev, struct cpuidle_driver *drv, int index)
{
	int new_index = index;

	/* This mode only can be entered when other core's are offline */
	if (num_online_cpus() > 1)
		new_index = drv->safe_state_index;

       /*用于检查醒来时间是否超过2秒*/
       check_awake_time_limit();
	if (new_index == 0)
		return hi6930_enter_idle(dev, drv, new_index);
	else
		return hi6930_enter_deepsleep(dev, drv, new_index);
}
/*lint --e{516,718,746}*/
static int __init hi6930_init_cpuidle(void)
{
	int i, max_cpuidle_state, cpu_id;
	struct cpuidle_device *device;
	struct cpuidle_driver *drv = &hi6930_idle_driver;

	/* Setup cpuidle driver */
	drv->state_count = (sizeof(hi6930_cpuidle_set) /
				       sizeof(struct cpuidle_state));
	max_cpuidle_state = drv->state_count;
	for (i = 0; i < max_cpuidle_state; i++) {
		memcpy(&drv->states[i], &hi6930_cpuidle_set[i],
				sizeof(struct cpuidle_state));
	}
	drv->safe_state_index = 0;
	cpuidle_register_driver(&hi6930_idle_driver);

	for_each_cpu(cpu_id, cpu_online_mask) { /*lint !e521*/
		
		/*lint -save -e50*//*lint -save -e64*/
		device = &per_cpu(hi6930_cpuidle_device, cpu_id); 
		device->cpu = (unsigned int)cpu_id;

		if (cpu_id == 0)
			device->state_count = (sizeof(hi6930_cpuidle_set) /
					       sizeof(struct cpuidle_state));
		else
			device->state_count = 1;	/* Support IDLE only */

		if (cpuidle_register_device(device)) {
			printk(KERN_ERR "CPUidle register device failed\n,");
			return -EIO;
		}
	}

	return 0;
}
device_initcall(hi6930_init_cpuidle);
/*lint -restore +e49*//*lint -restore +e601*//*lint -restore +e50*//*lint -restore +e64*/

