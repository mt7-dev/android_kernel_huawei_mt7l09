

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */


/*lint --e{537 } */
#include <vxWorks.h>
#include <intLib.h>
#include <errnoLib.h>
#include <sioLib.h>
#include <string.h>

#include <product_config.h>
#include <osl_types.h>
#include <osl_sem.h>
#include <osl_bio.h>
#include <osl_irq.h>
#include <bsp_ipc.h>
#include <bsp_sram.h>
#include "regulator_balong.h"

void regulator_table_verify(void);
extern struct regulator regulators[];

#define REGULATOR_TYPE_NUM(id) 			((id >> REGULATOR_SHIFT) & REGULATOR_TYPE_FLAG)
#define REGULATOR_ID_FOR_PMU(id)		(id & REGULATOR_TYPE_FLAG)

SEM_ID g_sem_k3get_volt  = NULL;
SEM_ID g_sem_k3isenable  = NULL;

int g_k3isenable = 0;
int g_k3getvolt = 0;

#define K3_REGU_CB_TIMEOUT 				(10)
/************************** reg pwr start ************************/
/*MTCMOS */
/*MTCMOS interface*/
int p531_enable_mtcmos(int id);
int p531_disable_mtcmos(int id);
int p531_is_enabled_mtcmos(int id);
int v7r2_enable_mtcmos(int id);
int v7r2_disable_mtcmos(int id);
int v7r2_is_enabled_mtcmos(int id);
int k3_enable_regu(int id);
int k3_disable_regu(int id);
int k3_is_enabled_regu(int id);
int k3_get_voltage_regu(int id);
int k3_set_voltage_regu(int id, int min_uV, int max_uV, unsigned *selector);
int balong_regulator_icc_send(struct regulator_msg *msg);

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
static int dummy_set_mode(int volt_id, unsigned int mode)
{
	regu_pr_err("dummy_set_mode id:%d \n", volt_id);
	return 0;
}
static unsigned int dummy_get_mode(int volt_id)
{
	regu_pr_err("dummy get_mode id:%d \n", volt_id);
	return 0;
}
static struct regulator_id_ops dummy_ops = {
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
static struct regulator_id_ops hi6451_ops = {
		.list_voltage = bsp_hi6451_volt_list_voltage,
		.set_voltage = bsp_hi6451_volt_set_voltage,
		.get_voltage = bsp_hi6451_volt_get_voltage,
		.is_enabled = bsp_hi6451_volt_is_enabled,
		.enable = bsp_hi6451_volt_enable,
		.disable = bsp_hi6451_volt_disable,
		.set_mode = bsp_hi6451_volt_set_mode,/*lint !e64 */
		.get_mode = bsp_hi6451_volt_get_mode,/*lint !e64 */
		.set_current = bsp_hi6451_volt_set_voltage,
		.get_current = bsp_hi6451_volt_get_voltage,
};
#else
static struct regulator_id_ops hi6451_ops = {
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
static struct regulator_id_ops hi6551_ops = {
		.list_voltage = bsp_hi6551_volt_list_voltage,
		.set_voltage = bsp_hi6551_volt_set_voltage,
		.get_voltage = bsp_hi6551_volt_get_voltage,
		.is_enabled = bsp_hi6551_volt_is_enabled,
		.enable = bsp_hi6551_volt_enable,
		.disable = bsp_hi6551_volt_disable,
		.set_current = bsp_hi6551_volt_set_voltage,
		.get_current = bsp_hi6551_volt_get_voltage,
};
#else
static struct regulator_id_ops hi6551_ops = {
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

#ifdef CONFIG_REGULATOR_FOR_HI6421
static struct regulator_id_ops k3_ops = {
		.set_voltage = k3_set_voltage_regu,
		.get_voltage = k3_get_voltage_regu,
		.is_enabled = k3_is_enabled_regu,
		.enable = k3_enable_regu,
		.disable = k3_disable_regu,
};
#else
static struct regulator_id_ops k3_ops = {
		.set_voltage = dummy_set_voltage,
		.get_voltage = dummy_get_voltage,
		.is_enabled = dummy_is_enabled,
		.enable = dummy_enable,
		.disable = dummy_disable,
};
#endif

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
	[REGULATOR_TYPE_K3] = {
		.base_num = K3_REGULATOR_BASE_NUM,
		.regulator_type = REGULATOR_TYPE_K3,
		.ops = &k3_ops,
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
	unsigned long flags;
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
	unsigned long flags;
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
	int ret = 0;
	unsigned long flags;
	regu_pr_debug("hi6541 mtcmos is enabled id is : %d", id);
	if (id < P531_MTCMOS_COUNT1 || id > P531_MTCMOS_COUNT10)
	{
		regu_pr_err("error isenabled id is : %d", id);
		return 0;
	}
	local_irq_save(flags);
	bsp_ipc_spin_lock(IPC_SEM_MTCMOS);

	ret = (s32)p531_mtcmos_interface[id].mtcmos_is_enable();

	bsp_ipc_spin_unlock(IPC_SEM_MTCMOS);
	local_irq_restore(flags);

	return ret;
}

int v7r2_enable_mtcmos(int id)
{
	int ret = 0;
	regu_pr_debug("v7r2 mtcmos enable id is : %d", id);
	if (id < V7R2_MTCMOS_COUNT1 || id > V7R2_MTCMOS_COUNT13)
	{
		regu_pr_err("error enable id is : %d", id);
		return -1;
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
		return -1;
	}

	v7r2_mtcmos_interface[id].mtcmos_disable();

	return ret;
}
int v7r2_is_enabled_mtcmos(int id)
{
	int ret = 0;
	regu_pr_debug("v7r2 mtcmos is enabled id is : %d", id);
	if (id < V7R2_MTCMOS_COUNT1 || id > V7R2_MTCMOS_COUNT13)
	{
		regu_pr_err("error isenabled id is : %d", id);
		return -1;
	}

	ret = (s32)v7r2_mtcmos_interface[id].mtcmos_is_enable();
	return ret;
}

int k3_enable_regu(int id)
{
	struct regulator_msg msg = {0,0,0,0,0};
	msg.volt_id = (u32)id;
	msg.ops_flag = HI6421_OPS_ENABLE;
	balong_regulator_icc_send(&msg);

	return 0;
}
int k3_disable_regu(int id)
{
	struct regulator_msg msg = {0,0,0,0,0};
	msg.volt_id = (u32)id;
	msg.ops_flag = HI6421_OPS_DISABLE;
	balong_regulator_icc_send(&msg);

	return 0;
}

int k3_is_enabled_regu(int id)
{
	struct regulator_msg msg = {0,0,0,0,0};
	int ret = 0;
	msg.volt_id = (u32)id;
	msg.ops_flag = HI6421_OPS_IS_ENABLED;
	balong_regulator_icc_send(&msg);
	/*获取信号量*/
	ret = semTake(g_sem_k3isenable, K3_REGU_CB_TIMEOUT);
	if (OK != ret)
	{
		regu_pr_err("semTake error:%d\n", ret);
	}
	return g_k3isenable;
}

int k3_get_voltage_regu(int id)
{
	struct regulator_msg msg = {0,0,0,0,0};
	int ret = 0;
	msg.volt_id = (u32)id;
	msg.ops_flag = HI6421_OPS_GET_VOLT;
	balong_regulator_icc_send(&msg);
	/*获取信号量*/
	ret = semTake(g_sem_k3get_volt, K3_REGU_CB_TIMEOUT);
	if (OK != ret)
	{
		regu_pr_err("semTake error:%d\n", ret);
	}
	return g_k3getvolt;
}

int k3_set_voltage_regu(int id, int min_uV, int max_uV, unsigned *selector)
{
	struct regulator_msg msg = {0,0,0,0,0};
	msg.volt_id = (u32)id;
	msg.ops_flag = HI6421_OPS_SET_VOLT;
	msg.min_uV = (u32)min_uV;
	msg.max_uV = (u32)max_uV;
	*selector = 0;
	balong_regulator_icc_send(&msg);

	return 0;
}
/************************** reg pwrctrl end *************************/
static int balong_regulator_hi6421_opscb(u32 channel_id , u32 len, void* context)
{
	s32 ret = BSP_OK;
	struct regulator_msg msg = {0};
	ret = bsp_icc_read(channel_id, (u8*)&msg, len);

	if(len != (u32)ret)
	{
		regu_pr_err("balong_cpufreq_cb_getload error \r\n");
		return BSP_ERROR;
	}
	switch  (msg.ops_flag)
	{
		case HI6421_OPS_ENABLE:
			regu_pr_info("enable\n");
			break;
		case HI6421_OPS_DISABLE:
			regu_pr_info("disable\n");
			break;
		case HI6421_OPS_IS_ENABLED:
			regu_pr_info("is_enabled sem\n");
			g_k3isenable = msg.result;
			semGive(g_sem_k3isenable);
			break;
		case HI6421_OPS_GET_VOLT:
			regu_pr_info("get voltage\n");
			g_k3getvolt = msg.result;
			semGive(g_sem_k3get_volt);
			break;
		case HI6421_OPS_SET_VOLT:
			regu_pr_info("set voltage\n");
			break;
		default:
			break;
	}
	return BSP_OK;
}
int balong_regulator_icc_send(struct regulator_msg *msg)
{
	u32 channel_id = ICC_CHN_MCORE_CCORE << 16 | MCORE_CCORE_FUNC_REGULATOR;
	s32 ret = 0;
	u32 msg_len = sizeof(struct regulator_msg);
	regu_pr_info("send icc to mcore volt_id:%d min_uV:%d max_uV:%d ops_flag:%d\n", msg->volt_id, msg->min_uV, msg->max_uV, msg->ops_flag);

	ret = bsp_icc_send(ICC_CPU_MCU, channel_id, (u8 *)msg, msg_len);
	if((ret < 0) && (ret != BSP_ERR_ICC_CCORE_RESETTING))
	{
		regu_pr_err("mcore return an ERROR\n");
		return BSP_ERROR;
	}
    return BSP_OK;
}

void regulator_init(void)
{
	s32 ret = 0;
	u32 channel_id_set = ICC_CHN_MCORE_CCORE << 16 | MCORE_CCORE_FUNC_REGULATOR;
	BUCK3_ONOFF_FLAG = 0;
	
	g_sem_k3get_volt = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY); /*lint !e64 */
    if (NULL == g_sem_k3get_volt)
    {
        regu_pr_err("Create g_sem_k3get_volt Failed %d\n", g_sem_k3get_volt);
    }
	g_sem_k3isenable = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY); /*lint !e64 */
    if (NULL == g_sem_k3isenable)
    {
        regu_pr_err("Create g_sem_k3disable Failed %d\n", g_sem_k3get_volt);
    }
    ret = bsp_icc_event_register(channel_id_set, (read_cb_func)balong_regulator_hi6421_opscb, (void *)NULL, (write_cb_func)NULL, (void *)NULL);
    if (ret != BSP_OK)
	{
		regu_pr_err("icc register failed %d\n", ret);
	}
	regu_pr_info("register icc \n");
     return;
}

struct regulator *regulator_get(unsigned char *name, char *supply)
{
	u32 regulator_id, i;

	for (regulator_id = 0; regulator_id <= NUM_OF_REGUALTORS; regulator_id++)
	{
		for (i = 0; i < regulators[regulator_id].num_consumer_supplies; i++)
		{
			if (strcmp(supply, regulators[regulator_id].consumer_supplies[i].supply) == 0)/*lint !e605 */
			{
				regu_pr_debug("get regulator name is : %s", regulators[regulator_id].name);
				return &regulators[regulator_id];
			}
		}
	}
	regu_pr_err("\nfailed to get regulator!\n");
	return NULL;
}

void regulator_put(struct regulator *regulator)
{
	return;
}
/*lint --e{737, 713 } */
int regulator_is_enabled(struct regulator *regulator)
{
	int ret = 0;
	unsigned int regulator_id = 0;

	regulator_id = (unsigned int)regulator->id;

	regu_pr_info("balong id enable id:%d type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

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

int regulator_enable(struct regulator *regulator)
{
	int ret = 0;
	unsigned int regulator_id = 0;
	regulator_id = (unsigned int)regulator->id;
	regu_pr_info("balong enable id:%d type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

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


int regulator_disable(struct regulator *regulator)
{
	int ret = 0;
	unsigned int regulator_id = 0;
	regulator_id = (unsigned int)regulator->id;
	regu_pr_info("balong disable id:%d type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

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

int regulator_set_voltage(struct regulator *regulator, s32 min_uV, s32 max_uV)
{
	int ret = 0;
	unsigned int regulator_id = 0;

	unsigned int selector = 0;
	regulator_id = (unsigned int)regulator->id;

	regu_pr_info("balong set volt :%d, min_uV:%d, max_uV:%d, type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id),
											min_uV, max_uV, REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->set_voltage)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->set_voltage((REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num), min_uV, max_uV, &selector);
	}
	else
	{
		regu_pr_err("ops set_voltage not exist\n");
	}
	return ret;
}


int regulator_get_voltage(struct regulator *regulator)
{
	int ret = 0;
	unsigned int regulator_id = 0;

	regulator_id = (unsigned int)regulator->id;

	regu_pr_info("balong get_volt id:%d type:%d\n", REGULATOR_ID_FOR_PMU(regulator_id), REGULATOR_TYPE_NUM(regulator_id));

	if (regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->get_voltage)
	{
		ret = regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].ops->get_voltage(REGULATOR_ID_FOR_PMU(regulator_id)
								- regulator_type_init[REGULATOR_TYPE_NUM(regulator_id)].base_num	);
	}
	else
	{
		regu_pr_err("ops get_voltage not exist\n");
	}
	return ret;
}

int regulator_set_mode(struct regulator *regulator, u32 mode)
{
	/*mutex start*/
	int ret = 0;

	/*下面调用PMU的接口*/

	/*mutex end*/
	return ret;
}

unsigned int regulator_get_mode(struct regulator *regulator)
{
	/*mutex start*/
	unsigned int ret = 0;

	/*下面调用PMU的接口*/

	/*mutex end*/
	return ret;
}





/*****************************************************************************
* 函 数 名  :  regulator_table_verify
*
* 功能描述  :  验证是否有重复的consumer

*
* 输入参数  :

* 输出参数  :  无
*
* 返 回 值     :  无
*
* 修改记录  :
*****************************************************************************/
void regulator_table_verify(void)
{
	u32 regulator_id, i, id, j;
	regu_pr_err("total : %d\n", NUM_OF_REGUALTORS);
	for (regulator_id = 0; regulator_id <= NUM_OF_REGUALTORS; regulator_id++)
	{
		for (id = 0; id < regulator_id; id++)
		{
			for (j = 0; j < regulators[regulator_id].num_consumer_supplies; j++)
			{
				for (i = 0; i < regulators[id].num_consumer_supplies; i++)
				{
					if (regulators[regulator_id].consumer_supplies[j].supply != NULL &&
						regulators[id].consumer_supplies[i].supply != NULL)
					{
						if (strcmp(regulators[id].consumer_supplies[i].supply, regulators[regulator_id].consumer_supplies[j].supply))/*lint !e605 */
						{
							continue;
						}
						else
						{
							regu_pr_err("have same supply: %s, id:%d\n", regulators[regulator_id].consumer_supplies[j].supply,
																			REGULATOR_ID_FOR_PMU(regulators[regulator_id].id));
							regu_pr_err("supply: %s, id:%d\n",regulators[id].consumer_supplies[i].supply,
																			REGULATOR_ID_FOR_PMU(regulators[id].id));
						}
					}
					else
					{
							regu_pr_debug("id:%d is NULL\n", regulator_id);
					}
				}
			}
		}
	}
}

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */
