

#ifdef __cplusplus
extern "C"
{
#endif
/*lint --e{537 } */
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/suspend.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>


#include <osl_bio.h>
#include <osl_irq.h>
#include <osl_sem.h>
#include <bsp_ipc.h>
#include <bsp_sram.h>
#include <bsp_version.h>
#include <bsp_pmu.h>
#include "regulator_balong.h"

/*lint --e{34} */

#define REGULATOR_TYPE_NUM(id) 			((id >> REGULATOR_SHIFT) & REGULATOR_TYPE_FLAG)
#define REGULATOR_ID_FOR_PMU(id)		(id & REGULATOR_TYPE_FLAG)


#ifdef CONFIG_PMIC_HI6451
extern struct regulator_desc hi6451_regulators_desc[];
extern struct regulator_init_data hi6451_regulators_init[];
#endif

#ifdef CONFIG_PMIC_HI6551
extern struct regulator_desc hi6551_regulators_desc[];
extern struct regulator_init_data hi6551_regulators_init[];

extern struct regulator_desc hi6551_dr_regulators_desc[];
extern struct regulator_init_data hi6551_dr_regulators_init[];
extern struct regulator_consumer_supply hi6551_dr1_consumers[];
extern struct regulator_consumer_supply hi6551_dr2_consumers[];
#endif

#ifdef REGULATOR_V7R2_MTCMOS
extern struct regulator_desc v7r2_mtcmos_regulator_desc[];
extern struct regulator_init_data v7r2_mtcmos_regulators_init[];
#endif

#ifdef REGULATOR_V711_MTCMOS
extern struct regulator_desc vendor_mtcmos_regulator_desc[];
extern struct regulator_init_data vendor_mtcmos_regulators_init[];
#endif

#ifdef REGULATOR_P531_MTCMOS
extern struct regulator_desc p531_mtcmos_regulator_desc[];
extern struct regulator_init_data mtcmos_regulators_init[];
#endif

#ifdef CONFIG_PMIC_HI6559
extern struct regulator_desc hi6559_regulators_desc[];
extern struct regulator_init_data hi6559_regulators_init[];

extern struct regulator_desc hi6559_dr_regulators_desc[];
extern struct regulator_init_data hi6559_dr_regulators_init[];
#endif
/************************** reg pwr start ************************/
/*MTCMOS interface*/
int p531_enable_mtcmos(int id);
int p531_disable_mtcmos(int id);
int p531_is_enabled_mtcmos(int id);
int v7r2_enable_mtcmos(int id);
int v7r2_disable_mtcmos(int id);
int v7r2_is_enabled_mtcmos(int id);

static int dummy_list_voltage(int volt_id, unsigned selector)
{
	regu_pr_err("dummy list id:%d \n", volt_id);
	return 0;
}
static int dummy_set_voltage(int volt_id, int min_uV, int max_uV, unsigned *selector)
{
	regu_pr_err("dummy_set_voltage id:%d \n", volt_id);
	return 0;
}
static int dummy_get_voltage(int volt_id)
{
	regu_pr_err("dummy_get_voltage id:%d \n", volt_id);
	return 0;
}
static int dummy_is_enabled(int volt_id)
{
	regu_pr_err("dummy_is_enabled id:%d \n", volt_id);
	return 0;
}
static int dummy_enable(int volt_id)
{
	regu_pr_err("dummy_enable id:%d \n", volt_id);
	return 0;
}
static int dummy_disable(int volt_id)
{
	regu_pr_err("dummy_disable id:%d \n", volt_id);
	return 0;
}
static int dummy_set_mode(int volt_id, int mode)
{
	regu_pr_err("dummy_set_mode id:%d \n", volt_id);
	return 0;
}
static unsigned int dummy_get_mode(int volt_id)
{
	regu_pr_err("dummy get_mode id:%d \n", volt_id);
	return 0;
}
struct regulator_id_ops dummy_ops = {
		.list_voltage = dummy_list_voltage,
		.set_voltage = dummy_set_voltage,
		.get_voltage = dummy_get_voltage,
		.is_enabled = dummy_is_enabled,
		.enable = dummy_enable,
		.disable = dummy_disable,
		.set_mode = dummy_set_mode,
		.get_mode = dummy_get_mode,
		.set_current = dummy_set_voltage,
		.get_current = dummy_get_voltage,
};

#ifdef CONFIG_PMIC_HI6451
struct regulator_id_ops hi6451_ops = {
		.list_voltage = bsp_hi6451_volt_list_voltage,
		.set_voltage = bsp_hi6451_volt_set_voltage,
		.get_voltage = bsp_hi6451_volt_get_voltage,
		.is_enabled = bsp_hi6451_volt_is_enabled,
		.enable = bsp_hi6451_volt_enable,
		.disable = bsp_hi6451_volt_disable,
		.set_mode = bsp_hi6451_volt_set_mode,
		.get_mode = bsp_hi6451_volt_get_mode,
		.set_current = bsp_hi6451_volt_set_voltage,
		.get_current = bsp_hi6451_volt_get_voltage,
};
#else
struct regulator_id_ops hi6451_ops = {
		.list_voltage = dummy_list_voltage,
		.set_voltage = dummy_set_voltage,
		.get_voltage = dummy_get_voltage,
		.is_enabled = dummy_is_enabled,
		.enable = dummy_enable,
		.disable = dummy_disable,
		.set_mode = dummy_set_mode,
		.get_mode = dummy_get_mode,
		.set_current = dummy_set_voltage,
		.get_current = dummy_get_voltage,
};
#endif

#ifdef CONFIG_PMIC_HI6551
struct regulator_id_ops hi6551_ops = {
		.list_voltage = bsp_hi6551_volt_list_voltage,
		.set_voltage = bsp_hi6551_volt_set_voltage,
		.get_voltage = bsp_hi6551_volt_get_voltage,
		.is_enabled = bsp_hi6551_volt_is_enabled,
		.enable = bsp_hi6551_volt_enable,
		.disable = bsp_hi6551_volt_disable,
		.set_current = bsp_hi6551_volt_set_voltage,
		.get_current = bsp_hi6551_volt_get_voltage,
};
struct regulator_id_ops hi6551_dr_ops = {
		.list_voltage = bsp_hi6551_dr_list_current,
		.set_voltage = bsp_hi6551_dr_set_current,
		.get_voltage = bsp_hi6551_dr_get_current,
		.is_enabled = bsp_hi6551_dr_is_enabled,
		.enable = bsp_hi6551_dr_enable,
		.disable = bsp_hi6551_dr_disable,
		.set_current = bsp_hi6551_dr_set_current,
		.get_current = bsp_hi6551_dr_get_current,
};
#else
struct regulator_id_ops hi6551_ops = {
		.list_voltage = dummy_list_voltage,
		.set_voltage = dummy_set_voltage,
		.get_voltage = dummy_get_voltage,
		.is_enabled = dummy_is_enabled,
		.enable = dummy_enable,
		.disable = dummy_disable,
		.set_current = dummy_set_voltage,
		.get_current = dummy_get_voltage,
};

struct regulator_id_ops hi6551_dr_ops = {
		.is_enabled = dummy_is_enabled,
		.enable = dummy_enable,
		.disable = dummy_disable,
		.set_current = dummy_set_voltage,
		.get_current = dummy_get_voltage,
};
#endif

static struct regulator_id_ops p531_mtmcos_ops = {
		.is_enabled = p531_is_enabled_mtcmos,
		.enable = p531_enable_mtcmos,
		.disable = p531_disable_mtcmos,
};
static struct regulator_id_ops v7r2_mtmcos_ops = {
		.is_enabled = v7r2_is_enabled_mtcmos,
		.enable = v7r2_enable_mtcmos,
		.disable = v7r2_disable_mtcmos,
};

#ifdef CONFIG_PMIC_HI6559
struct regulator_id_ops hi6559_ops = {
		.list_voltage = bsp_hi6559_volt_list_voltage,
		.set_voltage = bsp_hi6559_volt_set_voltage,
		.get_voltage = bsp_hi6559_volt_get_voltage,
		.is_enabled = bsp_hi6559_volt_is_enabled,
		.enable = bsp_hi6559_volt_enable,
		.disable = bsp_hi6559_volt_disable,
		.set_current = bsp_hi6559_volt_set_voltage,
		.get_current = bsp_hi6559_volt_get_voltage,
};
struct regulator_id_ops hi6559_dr_ops = {
		.list_voltage = bsp_hi6559_dr_list_current,
		.set_voltage = bsp_hi6559_dr_set_current,
		.get_voltage = bsp_hi6559_dr_get_current,
		.is_enabled = bsp_hi6559_dr_is_enabled,
		.enable = bsp_hi6559_dr_enable,
		.disable = bsp_hi6559_dr_disable,
		.set_current = bsp_hi6559_dr_set_current,
		.get_current = bsp_hi6559_dr_get_current,
};
#else
struct regulator_id_ops hi6559_ops = {
		.list_voltage = dummy_list_voltage,
		.set_voltage = dummy_set_voltage,
		.get_voltage = dummy_get_voltage,
		.is_enabled = dummy_is_enabled,
		.enable = dummy_enable,
		.disable = dummy_disable,
		.set_current = dummy_set_voltage,
		.get_current = dummy_get_voltage,
};

struct regulator_id_ops hi6559_dr_ops = {
		.list_voltage = dummy_list_voltage,
		.set_voltage = dummy_set_voltage,
		.get_voltage = dummy_get_voltage,
		.is_enabled = dummy_is_enabled,
		.enable = dummy_enable,
		.disable = dummy_disable,
		.set_current = dummy_set_voltage,
		.get_current = dummy_get_voltage,
};
#endif
static struct regulators_type regulator_type_init[REGULATOR_TYPE_END] = {

	[REGULATOR_TYPE_HI6451] = {
		.base_num = HI6451_BASE_NUM,
		.regulator_type = REGULATOR_TYPE_HI6451,
		.ops = &hi6451_ops,
	},
	[REGULATOR_TYPE_HI6551] = {
		.base_num = HI6551_BASE_NUM,
		.regulator_type = REGULATOR_TYPE_HI6551,
		.ops = &hi6551_ops,
	},
	[REGULATOR_TYPE_HI6561] = {
		.base_num = HI6561_BASE_NUM,
		.regulator_type = REGULATOR_TYPE_HI6561,
		.ops = &dummy_ops,
	},
	[REGULATOR_TYPE_P531_MTCMOS] = {
		.base_num = P531_MTCMOS_BASE_NUM,
		.regulator_type = REGULATOR_TYPE_P531_MTCMOS,
		.ops = &p531_mtmcos_ops,
	},
	[REGULATOR_TYPE_V7R2_MTCMOS] = {
		.base_num = V7R2_MTCMOS_BASE_NUM,
		.regulator_type = REGULATOR_TYPE_V7R2_MTCMOS,
		.ops = &v7r2_mtmcos_ops,
	},
	[REGULATOR_TYPE_HI6551_DR] = {
		.base_num = HI6551_DR_BASE_NUM,
		.regulator_type = REGULATOR_TYPE_HI6551_DR,
		.ops = &hi6551_dr_ops,
	},
    [REGULATOR_TYPE_V711_MTCMOS] = {
		.base_num = V711_MTCMOS_BASE_NUM,
		.regulator_type = REGULATOR_TYPE_V711_MTCMOS,
		.ops = &v7r2_mtmcos_ops,/* use v7r2 methods */
	},
	[REGULATOR_TYPE_HI6559] = {
		.base_num = HI6559_REGULATOR_BASE_NUM,
		.regulator_type = REGULATOR_TYPE_HI6559,
		.ops = &hi6559_ops,
	},
	[REGULATOR_TYPE_HI6559_DR] = {
		.base_num = HI6559_DR_BASE_NUM,
		.regulator_type = REGULATOR_TYPE_HI6559_DR,
		.ops = &hi6559_dr_ops,
	},
	[REGULATOR_TYPE_OBSOLETE] = {
		.base_num = 0,
		.regulator_type = REGULATOR_TYPE_OBSOLETE,
		.ops = &dummy_ops,
	},
};
static struct regulator_mtcmos_ops p531_mtcmos_interface[] = {
	[P531_MTCMOS_COUNT1] = {
		.mtcmos_enable = hi_pwrctrl_regu_tcsdrv1_en,
		.mtcmos_disable = hi_pwrctrl_regu_tcsdrv1_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_tcsdrv1_stat,
	},
	[P531_MTCMOS_COUNT2] = {
		.mtcmos_enable = hi_pwrctrl_regu_fpgaif_en,
		.mtcmos_disable = hi_pwrctrl_regu_fpgaif_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_fpgaif_stat,
	},
	[P531_MTCMOS_COUNT3] = {
		.mtcmos_enable = hi_pwrctrl_regu_t570t_en,
		.mtcmos_disable = hi_pwrctrl_regu_t570t_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_t570t_stat,
	},
	[P531_MTCMOS_COUNT4] = {
		.mtcmos_enable = hi_pwrctrl_regu_a15_en,
		.mtcmos_disable = hi_pwrctrl_regu_a15_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_a15_stat,
	},
	[P531_MTCMOS_COUNT5] = {
		.mtcmos_enable = hi_pwrctrl_regu_hsicphy_en,
		.mtcmos_disable = hi_pwrctrl_regu_hsicphy_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_hsicphy_stat,
	},
	[P531_MTCMOS_COUNT6] = {
		.mtcmos_enable = hi_pwrctrl_regu_usbphy_en,
		.mtcmos_disable = hi_pwrctrl_regu_usbphy_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_usbphy_stat,
	},
	[P531_MTCMOS_COUNT7] = {
		.mtcmos_enable = hi_pwrctrl_regu_appa9_en,
		.mtcmos_disable = hi_pwrctrl_regu_appa9_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_appa9_stat,
	},
	[P531_MTCMOS_COUNT8] = {
		.mtcmos_enable = hi_pwrctrl_regu_hifi_en,
		.mtcmos_disable = hi_pwrctrl_regu_hifi_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_hifi_stat,
	},
	[P531_MTCMOS_COUNT9] = {
		.mtcmos_enable = hi_pwrctrl_regu_bbe16_en,
		.mtcmos_disable = hi_pwrctrl_regu_bbe16_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_bbe16_stat,
	},
	[P531_MTCMOS_COUNT10] = {
		.mtcmos_enable = hi_pwrctrl_regu_moda9_en,
		.mtcmos_disable = hi_pwrctrl_regu_moda9_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_moda9_stat,
	},
};
/****for buck3 onoff start****/
static __inline__ void usb_mtcmos_en(void)
{
	BUCK3_ONOFF_FLAG = 1;
	cache_sync();
	regu_pr_debug("BUCK3_ONOFF_FLAG %x, %d\n", &BUCK3_ONOFF_FLAG, BUCK3_ONOFF_FLAG);
	hi_pwrctrl_regu_usbmtcmos_en();
}
static __inline__ void usb_mtcmos_dis(void)
{
	hi_pwrctrl_regu_usbmtcmos_dis();
	BUCK3_ONOFF_FLAG = 0;
	cache_sync();
}
/****for buck3 onoff end****/
static struct regulator_mtcmos_ops v7r2_mtcmos_interface[] = {
	[V7R2_MTCMOS_COUNT1] = {
		.mtcmos_enable = hi_pwrctrl_regu_sdmtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_sdmtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_sdmtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT2] = {
		.mtcmos_enable = usb_mtcmos_en,
		.mtcmos_disable = usb_mtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_usbmtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT3] = {
		.mtcmos_enable = hi_pwrctrl_regu_irmmtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_irmmtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_irmmtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT4] = {
		.mtcmos_enable = hi_pwrctrl_regu_g2bbpmtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_g2bbpmtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_g2bbpmtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT5] = {
		.mtcmos_enable = hi_pwrctrl_regu_g1bbpmtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_g1bbpmtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_g1bbpmtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT6] = {
		.mtcmos_enable = hi_pwrctrl_regu_wbbpmtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_wbbpmtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_wbbpmtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT7] = {
		.mtcmos_enable = hi_pwrctrl_regu_twbbpmtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_twbbpmtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_twbbpmtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT8] = {
		.mtcmos_enable = hi_pwrctrl_regu_lbbpmtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_lbbpmtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_lbbpmtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT9] = {
		.mtcmos_enable = hi_pwrctrl_regu_hifimtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_hifimtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_hifimtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT10] = {
		.mtcmos_enable = hi_pwrctrl_regu_dsp0mtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_dsp0mtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_dsp0mtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT11] = {
		.mtcmos_enable = hi_pwrctrl_regu_hiscmtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_hiscmtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_hiscmtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT12] = {
		.mtcmos_enable = hi_pwrctrl_regu_moda9mtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_moda9mtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_moda9mtcmos_stat,
	},
	[V7R2_MTCMOS_COUNT13] = {
		.mtcmos_enable = hi_pwrctrl_regu_appa9mtcmos_en,
		.mtcmos_disable = hi_pwrctrl_regu_appa9mtcmos_dis,
		.mtcmos_is_enable = hi_pwrctrl_regu_appa9mtcmos_stat,
	},
};

int p531_enable_mtcmos(int id)
{
	int ret = 0;
	unsigned long flags = 0;
	regu_pr_debug("hi6541 mtcmos enable id is : %d", id);
	if (id < P531_MTCMOS_COUNT1 || id > P531_MTCMOS_COUNT10)
	{
		regu_pr_err("error enable id is : %d", id);
		return 0;
	}
	/*核内互斥，核间锁*/
	local_irq_save(flags);
	bsp_ipc_spin_lock(IPC_SEM_MTCMOS);

	p531_mtcmos_interface[id].mtcmos_enable();

	bsp_ipc_spin_unlock(IPC_SEM_MTCMOS);
	local_irq_restore(flags);

	return ret;
}

int p531_disable_mtcmos(int id)
{
	int ret = 0;
	unsigned long flags = 0;
	regu_pr_debug("hi6541 mtcmos disable id is : %d", id);
	if (id < P531_MTCMOS_COUNT1 || id > P531_MTCMOS_COUNT10)
	{
		regu_pr_err("error disable id is : %d", id);
		return 0;
	}
	/*核内互斥，核间锁*/
	local_irq_save(flags);
	bsp_ipc_spin_lock(IPC_SEM_MTCMOS);

	p531_mtcmos_interface[id].mtcmos_disable();

	bsp_ipc_spin_unlock(IPC_SEM_MTCMOS);
	local_irq_restore(flags);

	return ret;

}
int p531_is_enabled_mtcmos(int id)
{
	unsigned int ret = 0;
	unsigned long flags = 0;
	regu_pr_debug("hi6541 mtcmos is enabled id is : %d", id);
	if (id < P531_MTCMOS_COUNT1 || id > P531_MTCMOS_COUNT10)
	{
		regu_pr_err("error isenabled id is : %d", id);
		return 0;
	}
	local_irq_save(flags);
	bsp_ipc_spin_lock(IPC_SEM_MTCMOS);

	ret = p531_mtcmos_interface[id].mtcmos_is_enable();

	bsp_ipc_spin_unlock(IPC_SEM_MTCMOS);
	local_irq_restore(flags);

	return (int)ret;
}

int v7r2_enable_mtcmos(int id)
{
	int ret = 0;
	regu_pr_debug("v7r2 mtcmos enable id is : %d", id);
	if (id < V7R2_MTCMOS_COUNT1 || id > V7R2_MTCMOS_COUNT13)
	{
		regu_pr_err("error enable id is : %d", id);
		return 0;
	}
	v7r2_mtcmos_interface[id].mtcmos_enable();

	return ret;
}

int v7r2_disable_mtcmos(int id)
{
	int ret = 0;
	regu_pr_debug("v7r2 mtcmos disable id is : %d", id);
	if (id < V7R2_MTCMOS_COUNT1 || id > V7R2_MTCMOS_COUNT13)
	{
		regu_pr_err("error disable id is : %d", id);
		return 0;
	}

	v7r2_mtcmos_interface[id].mtcmos_disable();

	return ret;
}
int v7r2_is_enabled_mtcmos(int id)
{
	unsigned int ret = 0;
	regu_pr_debug("v7r2 mtcmos is enabled id is : %d", id);
	if (id < V7R2_MTCMOS_COUNT1 || id > V7R2_MTCMOS_COUNT13)
	{
		regu_pr_err("error isenabled id is : %d", id);
		return 0;
	}

	ret = v7r2_mtcmos_interface[id].mtcmos_is_enable();
	return (int)ret;
}

/************************** reg pwrctrl end *************************/
/**************************ops start******************************/
/*
	判断regulator是否已使能
	1--已使能
	0--未使能
*/
static int balong_regulator_is_enabled(struct regulator_dev *dev)
{
	int ret = 0;
	unsigned int regulator_id = 0;
	regulator_id = (unsigned int)rdev_get_id(dev);
	/*lint --e{737, 713 } */
	regu_pr_debug("balong id enable id:%d type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->is_enabled)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->is_enabled(REGULATOR_ID_FOR_PMU(regulator_id)
									- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num);
	}
	else
	{
		regu_pr_err("ops is_enable not exist\n");
	}
	return ret;
}

static int balong_regulator_enable(struct regulator_dev *dev)
{
	int ret = 0;
	unsigned int regulator_id = 0;

	regulator_id = (unsigned int)rdev_get_id(dev);
	/*lint --e{737, 713 } */
	regu_pr_debug("balong enable id:%d type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->enable)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->enable(REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num);
	}
	else
	{
		regu_pr_err("ops enable not exist\n");
	}
	return ret;
}
static int balong_regulator_disable(struct regulator_dev *dev)
{
	int ret = 0;
	unsigned int regulator_id = 0;

	regulator_id = (unsigned int)rdev_get_id(dev);
	/*lint --e{737, 713 } */
	regu_pr_debug("balong disable id:%d type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->disable)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->disable(REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num);
	}
	else
	{
		regu_pr_err("ops disable not exist\n");
	}
	return ret;
}
static int balong_regulator_get_voltage(struct regulator_dev *dev)
{
	int ret = 0;
	unsigned int regulator_id = 0;

	regulator_id = (unsigned int)rdev_get_id(dev);
	/*lint --e{737, 713 } */
	//regu_pr_debug("balong get_volt id:%d type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->get_voltage)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->get_voltage(REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num);
	}
	else
	{
		regu_pr_info("ops get_voltage not exist\n");
	}
	return ret;
}
static int balong_regulator_set_voltage(struct regulator_dev *dev, int min_uV, int max_uV, unsigned *selector)
{
	int ret = 0;
	unsigned int regulator_id = 0;

	regulator_id = (unsigned int)rdev_get_id(dev);
	/*lint --e{737, 713 } */
	regu_pr_debug("balong set volt :%d, min_uV:%d, max_uV:%d, type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), min_uV, max_uV, REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->set_voltage)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->set_voltage((REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num), min_uV, max_uV, selector);
	}
	else
	{
		regu_pr_err("ops set_voltage not exist\n");
	}
	return ret;
}
static int balong_regulator_list_voltage(struct regulator_dev *dev, unsigned selector)
{
	int ret = 0;
	unsigned int regulator_id = 0;

	regulator_id = (unsigned int)rdev_get_id(dev);
	/*lint --e{737, 713 } */
	regu_pr_debug("balong list volt id is : %d, selector: %d, type:%d", REGULATOR_ID_FOR_PMU(regulator_id), selector, REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->list_voltage)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->list_voltage((REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num), selector);
	}
	else
	{
		regu_pr_err("ops list_voltage not exist\n");
	}
	return ret;
}

static int balong_regulator_set_current(struct regulator_dev *dev, int min_uA, int max_uA)
{
	int ret = 0;
	unsigned int regulator_id = 0;

	unsigned int selector = 0;
	regulator_id = (unsigned int)rdev_get_id(dev);
	/*lint --e{737, 713 } */
	regu_pr_debug("balong set current id is : %d, type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->set_current)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->set_current((REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num), min_uA, max_uA, &selector);
	}
	else
	{
		regu_pr_err("ops set current not exist\n");
	}
	return ret;
}
static int balong_regulator_get_current(struct regulator_dev *dev)
{
	int ret = 0;
	unsigned int regulator_id = 0;

	regulator_id = (unsigned int)rdev_get_id(dev);
	/*lint --e{737, 713 } */
	regu_pr_debug("balong get_current id:%d type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->get_current)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->get_current(REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num);
	}
	else
	{
		regu_pr_err("ops get_current not exist\n");
	}
	return ret;
}
static unsigned int balong_regulator_get_mode(struct regulator_dev *dev)
{
	unsigned int ret = 0;
	unsigned int regulator_id = 0;

	regulator_id = (unsigned int)rdev_get_id(dev);
	/*lint --e{737, 713 } */
	regu_pr_debug("balong get mode id:%d type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->get_mode)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->get_mode(REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num);
	}
	else
	{
		regu_pr_err("ops get_mode not exist\n");
	}
	return ret;
}
static int balong_regulator_set_mode(struct regulator_dev *dev,unsigned int mode)
{
	unsigned int ret = 0;
	unsigned int regulator_id = 0;

	regulator_id = (unsigned int)rdev_get_id(dev);
	/*lint --e{737, 713, 732 } */
	regu_pr_debug("balong set mode id:%d mode:%d, type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), mode, REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->set_mode)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->set_mode((REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num), mode);
	}
	else
	{
		regu_pr_err("ops set_mode not exist\n");
	}
	return ret;
}


struct regulator_ops balong_regulator_ldo_ops = {
	.is_enabled = balong_regulator_is_enabled,
	.enable = balong_regulator_enable,
	.disable = balong_regulator_disable,
	.get_voltage = balong_regulator_get_voltage,
	.set_voltage = balong_regulator_set_voltage,
	.list_voltage = balong_regulator_list_voltage,
	.set_current_limit	= balong_regulator_set_current,
	.get_current_limit	= balong_regulator_get_current,
	.get_mode =balong_regulator_get_mode,
	.set_mode = balong_regulator_set_mode,
/*	.get_optimum_mode = balong_regulator_get_optimum_mode,*/
};

struct regulator_ops p531_mtcmos_ops = {
	.is_enabled = balong_regulator_is_enabled,
	.enable = balong_regulator_enable,
	.disable = balong_regulator_disable,
};

struct regulator_ops v7r2_mtcmos_ops = {
	.is_enabled = balong_regulator_is_enabled,
	.enable = balong_regulator_enable,
	.disable = balong_regulator_disable,
};

#ifdef REGULATOR_V711_MTCMOS
struct regulator_ops vendor_mtcmos_ops = {
	.is_enabled = balong_regulator_is_enabled,
	.enable = balong_regulator_enable,
	.disable = balong_regulator_disable,
};
#endif

/**************************ops end******************************/
/*change dr user*/
void balong_regulator_change_dr_consumer(void)
{
#if defined(CONFIG_PMIC_HI6551)
	u32 product_type = bsp_version_get_board_chip_type();
	if(HW_VER_PRODUCT_UDP == product_type)
	{
		hi6551_dr1_consumers[0].supply = HI6551_LCD_DR;
		hi6551_dr1_consumers[0].dev_name = NULL;
		hi6551_dr2_consumers[0].supply = HI6551_SD_DR;
		hi6551_dr2_consumers[0].dev_name = NULL;
	}
#endif
}

static int balong_regulator_probe(struct platform_device *pdev)
{
	int regulator_id = 0;
	struct regulator_dev *regualtor_info = NULL;

	BUCK3_ONOFF_FLAG = 1;

#ifdef CONFIG_PMIC_HI6451
	for (regulator_id = 1; regulator_id <= NUM_OF_HI6451_REGULATOR; regulator_id++) {
		regualtor_info = regulator_register(&hi6451_regulators_desc[regulator_id], &pdev->dev,
			hi6451_regulators_init + regulator_id, regualtor_info, NULL);
		if (IS_ERR(regualtor_info)){
			regu_pr_err("regulator %s register failed!!\n", hi6451_regulators_desc[regulator_id].name);
		}
	}
	regu_pr_err("hi6451 regulator register ok\n");
#endif

#ifdef CONFIG_PMIC_HI6551
	for (regulator_id = 2; regulator_id <= NUM_OF_HI6551_REGULATOR; regulator_id++) {
		regualtor_info = regulator_register(&hi6551_regulators_desc[regulator_id], &pdev->dev,
			hi6551_regulators_init + regulator_id, regualtor_info, NULL);
		if (IS_ERR(regualtor_info)){
			regu_pr_err("regulator %s register failed!!\n", hi6551_regulators_desc[regulator_id].name);
		}
	}
	regu_pr_err("hi6551 regulator register ok\n");
	balong_regulator_change_dr_consumer();
	for (regulator_id = PMIC_HI6551_DR01; regulator_id <= PMIC_HI6551_DR05; regulator_id++) {
		regualtor_info = regulator_register(&hi6551_dr_regulators_desc[regulator_id], &pdev->dev,
			hi6551_dr_regulators_init + regulator_id, regualtor_info, NULL);
		if (IS_ERR(regualtor_info)){
			regu_pr_err("regulator %s register failed!!\n", hi6551_dr_regulators_desc[regulator_id].name);
		}
	}
	regu_pr_err("hi6551 dr regulator register ok\n");
#endif

#ifdef REGULATOR_P531_MTCMOS
	for (regulator_id = P531_MTCMOS_COUNT1; regulator_id < P531_MTCMOS_COUNT_END; regulator_id++)
	{
		regualtor_info = regulator_register(&p531_mtcmos_regulator_desc[regulator_id], &pdev->dev,
			mtcmos_regulators_init + regulator_id, regualtor_info, NULL);
		if (IS_ERR(regualtor_info))
		{
			regu_pr_err(" regulator %s register failed!!id is : %d\n", p531_mtcmos_regulator_desc[regulator_id].name, regulator_id);
		}
	}
	regu_pr_err("p531 mtcmos register ok\n");
#endif

#ifdef REGULATOR_V7R2_MTCMOS
	for (regulator_id = V7R2_MTCMOS_COUNT1; regulator_id < V7R2_MTCMOS_COUNT_END; regulator_id++)
	{
		regualtor_info = regulator_register(&v7r2_mtcmos_regulator_desc[regulator_id], &pdev->dev,
			v7r2_mtcmos_regulators_init + regulator_id, regualtor_info, NULL);
		if (IS_ERR(regualtor_info))
		{
			regu_pr_err(" regulator %s register failed!!id is : %d\n", v7r2_mtcmos_regulator_desc[regulator_id].name, regulator_id);
		}
	}
	regu_pr_err("v7r2 mtcmos register ok\n");
#endif

#ifdef REGULATOR_V711_MTCMOS
	for (regulator_id = V711_MTCMOS_COUNT1; regulator_id < V711_MTCMOS_COUNT_END; regulator_id++)
	{
		regualtor_info = regulator_register(&vendor_mtcmos_regulator_desc[regulator_id], &pdev->dev,
			vendor_mtcmos_regulators_init + regulator_id, regualtor_info, NULL);
		if (IS_ERR(regualtor_info))
		{
			regu_pr_err(" regulator %s register failed!!id is : %d\n", vendor_mtcmos_regulator_desc[regulator_id].name, regulator_id);
		}
	}
	regu_pr_err("v711 mtcmos register ok\n");
#endif

#ifdef CONFIG_PMIC_HI6559
	for (regulator_id = PMIC_HI6559_BUCK0; regulator_id <= PMIC_HI6559_LVS09; regulator_id++) {
		regualtor_info = regulator_register(&hi6559_regulators_desc[regulator_id], &pdev->dev,
			hi6559_regulators_init + regulator_id, regualtor_info, NULL);
		if (IS_ERR(regualtor_info)){
			regu_pr_err("regulator %s register failed!!\n", hi6559_regulators_desc[regulator_id].name);
		}
	}
	regu_pr_err("hi6559 regulator register ok\n");
	/*多出一个id是为了6559中不存在的组件可以兼容6551，添加了打桩regulator*/
	for (regulator_id = PMIC_HI6559_DR01; regulator_id <= PMIC_HI6559_DR05 + 1; regulator_id++) {
		regualtor_info = regulator_register(&hi6559_dr_regulators_desc[regulator_id], &pdev->dev,
			hi6559_dr_regulators_init + regulator_id, regualtor_info, NULL);
		if (IS_ERR(regualtor_info)){
			regu_pr_err("regulator %s register failed!!\n", hi6559_dr_regulators_desc[regulator_id].name);
		}
	}
	regu_pr_err("hi6559 dr regulator register ok\n");
#endif

	return 0;
}


/*************************device and driver start****************************/
static struct platform_driver balong_regulator_driver = {
	.probe 		= balong_regulator_probe,
	.driver		= {
		.name	= "balong_regulator",
	},
};

static struct platform_device balong_regulator_device = {
	.name = "balong_regulator",
	.id = 0,
	.dev = {
		.platform_data = NULL,
	},
};
/*************************device and driver start****************************/
static int __init balong_regulator_init(void)
{
	int ret = 0;

	ret = platform_device_register(&balong_regulator_device);
	if (ret)
	{
		regu_pr_debug("register devices failed\n");
		return ret;
	}
	ret = platform_driver_register(&balong_regulator_driver);
	if (ret)
	{
		regu_pr_debug("register driver failed\n");
		platform_device_unregister(&balong_regulator_device);
		return ret;
	}

	return 0;
}
static void __exit balong_regulator_exit(void)
{
	platform_driver_unregister(&balong_regulator_driver);
	platform_device_unregister(&balong_regulator_device);
}
subsys_initcall(balong_regulator_init);
module_exit(balong_regulator_exit);


#ifdef __cplusplus
}
#endif

