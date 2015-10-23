
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/suspend.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>

#include <bsp_pmu.h>
#include <regulator_balong.h>
//extern static struct regulator_init_data hi6451_regulators_init[];
#ifdef CONFIG_PMIC_HI6451
extern struct regulator_desc hi6451_regulators_desc[];
extern struct regulator_init_data hi6451_regulators_init[];
#endif
#ifdef CONFIG_PMIC_HI6551
extern struct regulator_desc hi6551_regulators_desc[];
extern struct regulator_init_data hi6551_regulators_init[];
#endif
extern struct regulator_desc hi6551_mtcmos_regulator_desc[];
extern struct regulator_init_data hi6551_mtcmos_regulators_init[];
extern struct regulator_desc hi6451_mtcmos_regulator_desc[];
extern struct regulator_init_data mtcmos_regulators_init[];


extern struct regulator_dev *get_regulator_by_id(int id);

/******************************自动化测试 start**************************/
#define AUTO_TEST_RETURN_OK			(0)
#define AUTO_TEST_RETURN_ERROR		(-1)
struct regulator_supply_test {
	const char *id;
	int min_uV;
	int max_uV;
};
#ifdef CONFIG_PMIC_HI6451
/*LDO 6/12, BUCK 3/4/6不能开关及设置电压*/
struct regulator_supply_test hi6451_supply_test_arr[] = {
	[PMIC_HI6451_BUCK1] = {
		.id = "BUCK1-vcc",
		.min_uV = 800000,
		.max_uV = 1850000,
	},
	[PMIC_HI6451_BUCK2] = {
		.id = "BUCK2-vcc",
		.min_uV = 900000,
		.max_uV = 1300000,
	},
	[PMIC_HI6451_BUCK3] = {
		.id = "JTAG245-vcc",
		.min_uV = 1200000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_BUCK4] = {
		.id = "LCD_LED-vcc",
		.min_uV = 2900000,
		.max_uV = 3600000,
	},
	[PMIC_HI6451_BUCK5] = {
		.id = "BUCK5-vcc",
		.min_uV = 1200000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_BUCK6] = {
		.id = "EMMC_NAND-vcc",
		.min_uV = 1200000,
		.max_uV = 3450000,
	},
	[PMIC_HI6451_LDO1] = {
		.id = "LDO1-vcc",
		.min_uV = 1100000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_LDO2] = {
		.id = "LDO2-vcc",
		.min_uV = 1100000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_LDO3] = {
		.id = "LDO3-vcc",
		.min_uV = 1100000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_LDO4] = {
		.id = "LDO4-vcc",
		.min_uV = 1100000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_LDO5] = {
		.id = "LDO5-vcc",
		.min_uV = 1100000,
		.max_uV = 2600000,
	},
	[PMIC_HI6451_LDO6] = {
		.id = "TCXO-vcc",
		.min_uV = 2200000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_LDO7] = {
		.id = "SD_IO-vcc",
		.min_uV = 1200000,
		.max_uV = 3500000,
	},
	[PMIC_HI6451_LDO8] = {
		.id = "LDO8-vcc",
		.min_uV = 900000,
		.max_uV = 1500000,
	},
	[PMIC_HI6451_LDO9] = {
		.id = "LDO9-vcc",
		.min_uV = 900000,
		.max_uV = 1500000,
	},
	[PMIC_HI6451_LDO10] = {
		.id = "SD_POWER-vcc",
		.min_uV = 1200000,
		.max_uV = 3300000,
	},
	[PMIC_HI6451_LDO11] = {
		.id = "SIM1-vcc",
		.min_uV = 1200000,
		.max_uV = 3050000,
	},
	[PMIC_HI6451_LDO12] = {
		.id = "P530_USB-vcc",
		.min_uV = 3000000,
		.max_uV = 3000000,
	},
	[PMIC_HI6451_LDO13] = {
		.id = "SIM0-vcc",
		.min_uV = 1100000,
		.max_uV = 3000000,
	},
	[PMIC_HI6451_LDO14] = {
		.id = "LCD-vcc",
		.min_uV = 1100000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_LDO15] = {
		.id = "LDO15-vcc",
		.min_uV = 1100000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_LDO15 + 1] = {
		.id = "xxxxx-vcc",
		.min_uV = 1100000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_LDO15 + 2] = {
		.id = "",
		.min_uV = 1100000,
		.max_uV = 2850000,
	},
	[PMIC_HI6451_LDO15 + 3] = {
		.id = "LDO15-vcc",
		.min_uV = 1000000,
		.max_uV = 2850000 + 1100000,
	},
	[PMIC_HI6451_LDO15 + 4] = {
		.id = "LDO15-vcc",
		.min_uV = 1100000,
		.max_uV = 2850000,
	},
};
int hi6451_regulator_auto_test001(void)
{
	int ret = 1;
	int i = 0;
	struct regulator *auto_test_regu = ERR_PTR(-EPROBE_DEFER);
	for (i = PMIC_HI6451_BUCK1; i <= PMIC_HI6451_LDO15; i++)
	{

		auto_test_regu = regulator_get(NULL, hi6451_supply_test_arr[i].id);
		if (IS_ERR(auto_test_regu))
		{
			ret = 1;
			regu_pr_err("get regulator error id is:%s\n", hi6451_supply_test_arr[i].id);
			goto out;
		}
		if (i == PMIC_HI6451_BUCK3 || i == PMIC_HI6451_BUCK4 || i == PMIC_HI6451_BUCK6
									|| i == PMIC_HI6451_LDO6 || i == PMIC_HI6451_LDO12)
		{
			continue;
		}
		ret = regulator_enable(auto_test_regu);
		if (ret)
		{
			regu_pr_err("enable regulator error id is:%s\n", hi6451_supply_test_arr[i].id);
			goto out;
		}
		ret = regulator_disable(auto_test_regu);
		if (ret)
		{
			regu_pr_err("enable regulator error id is:%s\n", hi6451_supply_test_arr[i].id);
			goto out;
		}
		if (i == PMIC_HI6451_LDO7)
		{
			continue;
		}
		ret = regulator_set_voltage(auto_test_regu, hi6451_supply_test_arr[i].min_uV, hi6451_supply_test_arr[i].min_uV);
		if (ret)
		{
			regu_pr_err("set regulator voltage error id is:%s min volt: %d\n", hi6451_supply_test_arr[i].id, hi6451_supply_test_arr[i].min_uV);
			goto out;
		}
		ret = regulator_set_voltage(auto_test_regu, hi6451_supply_test_arr[i].min_uV, hi6451_supply_test_arr[i].max_uV);
		if (ret)
		{
			regu_pr_err("set regulator voltage error id is:%s max volt: %d\n", hi6451_supply_test_arr[i].id, hi6451_supply_test_arr[i].max_uV);
			goto out;
		}
	}
out:
	return ret;
}

int hi6451_regulator_auto_test002(void)
{
	int ret = 1;
	int i = 0;
	struct regulator *auto_test_regu02 = ERR_PTR(-EPROBE_DEFER);
	for (i = PMIC_HI6451_LDO15; i <= PMIC_HI6451_LDO15 + 4; i++)
	{
		auto_test_regu02 = regulator_get(NULL, hi6451_supply_test_arr[i].id);
		if (IS_ERR(auto_test_regu02))
		{
			continue;
		}

		if (i == PMIC_HI6451_LDO15 + 3)
		{
			ret = regulator_set_voltage(auto_test_regu02, hi6451_supply_test_arr[i].min_uV, hi6451_supply_test_arr[i].min_uV);
			if (!ret)
			{
				regu_pr_err("set regulator volt success??? id is:%s, volt:%d\n", hi6451_supply_test_arr[i].id, hi6451_supply_test_arr[i].min_uV);
				ret = 1;
				goto out;
			}
			ret = regulator_set_voltage(auto_test_regu02, hi6451_supply_test_arr[i].max_uV, hi6451_supply_test_arr[i].max_uV);
			if (!ret)
			{
				regu_pr_err("set regulator volt success??? id is:%s, volt:%d\n", hi6451_supply_test_arr[i].id, hi6451_supply_test_arr[i].max_uV);
				ret = 1;
				goto out;
			}
		}
		else if (i == PMIC_HI6451_LDO15 + 4)
		{
			ret = regulator_set_voltage(auto_test_regu02, hi6451_supply_test_arr[i].min_uV, hi6451_supply_test_arr[i].min_uV);
			if (ret)
			{
				regu_pr_err("set regulator voltage error id is:%s min volt: %d\n", hi6451_supply_test_arr[i].id, hi6451_supply_test_arr[i].min_uV);
				goto out;
			}
			ret = regulator_set_voltage(auto_test_regu02, hi6451_supply_test_arr[i].min_uV, hi6451_supply_test_arr[i].max_uV);
			if (ret)
			{
				regu_pr_err("set regulator voltage error id is:%s max volt: %d\n", hi6451_supply_test_arr[i].id, hi6451_supply_test_arr[i].max_uV);
				goto out;
			}
		}
	}
out:
	return ret;
}

#else
int hi6451_regulator_auto_test001(void)
{
	regu_pr_err("macro 6451 not open\n");
	return 0;
}
int hi6451_regulator_auto_test002(void)
{
	regu_pr_err("macro 6451 not open\n");
	return 0;
}
#endif

#ifdef CONFIG_PMIC_HI6551
struct regulator_supply_test hi6551_supply_test_arr[] = {
	[PMIC_HI6551_BUCK1] = {
		.id = "HI6551_BUCK3-vcc",
		.min_uV = 700000,
		.max_uV = 1204000,
	},
	[PMIC_HI6551_BUCK2] = {
		.id = "RSV4REIC-vcc",
		.min_uV = 700000,
		.max_uV = 1800000,
	},
	[PMIC_HI6551_BUCK3] = {
		.id = "HI6551_BUCK3-vcc",
		.min_uV = 750000,
		.max_uV = 1125000,
	},
	[PMIC_HI6551_BUCK4] = {
		.id = "LPDDR2-vcc",
		.min_uV = 1200000,
		.max_uV = 1575000,
	},
	[PMIC_HI6551_BUCK5] = {
		.id = "RSV4RFIC_BUCK5-vcc",
		.min_uV = 700000,
		.max_uV = 1800000,
	},
	[PMIC_HI6551_BUCK6] = {
		.id = "LPDDR2_VDD1-vcc",
		.min_uV = 1800000,
		.max_uV = 2175000,
	},
	[PMIC_HI6551_BOOST] = {
		.id = "BOOST-vcc",
		.min_uV = 3400000,
		.max_uV = 5500000,
	},
	[PMIC_HI6551_LDO01] = {
		.id = "HKADC_REF-vcc",
		.min_uV = 1800000,
		.max_uV = 3000000,
	},
	[PMIC_HI6551_LDO02] = {
		.id = "RSV_4PA_BIAS-vcc",
		.min_uV = 2500000,
		.max_uV = 3300000,
	},
	[PMIC_HI6551_LDO03] = {
		.id = "SOC-vcc",
		.min_uV = 800000,
		.max_uV = 1000000,
	},
	[PMIC_HI6551_LDO04] = {
		.id = "HI6551_LDO4-vcc",
		.min_uV = 1500000,
		.max_uV = 2800000,
	},
	[PMIC_HI6551_LDO05] = {
		.id = "HI6551_RSV4PABIAS-vcc",
		.min_uV = 2850000,
		.max_uV = 3150000,
	},
	[PMIC_HI6551_LDO06] = {
		.id = "HI6551_LDO6-vcc",
		.min_uV = 1500000,
		.max_uV = 2800000,
	},
	[PMIC_HI6551_LDO07] = {
		.id = "SD_IO-vcc",
		.min_uV = 1800000,
		.max_uV = 3050000,
	},
	[PMIC_HI6551_LDO08] = {
		.id = "HI6551_LDO8-vcc",
		.min_uV = 1100000,
		.max_uV = 1375000,
	},
	[PMIC_HI6551_LDO09] = {
		.id = "SIM0-vcc",
		.min_uV = 1800000,
		.max_uV = 3050000,
	},
	[PMIC_HI6551_LDO10] = {
		.id = "SD_POWER-vcc",
		.min_uV = 2800000,
		.max_uV = 3150000,
	},
	[PMIC_HI6551_LDO11] = {
		.id = "SIM1-vcc",
		.min_uV = 1800000,
		.max_uV = 3050000,
	},
	[PMIC_HI6551_LDO12] = {
		.id = "HI6551_LDO12-vcc",
		.min_uV = 3000000,
		.max_uV = 3350000,
	},
	[PMIC_HI6551_LDO13] = {
		.id = "HI6551_LDO13-vcc",
		.min_uV = 1800000,
		.max_uV = 3000000,
	},
	[PMIC_HI6551_LDO14] = {
		.id = "LCD-vcc",
		.min_uV = 1800000,
		.max_uV = 3000000,
	},
	[PMIC_HI6551_LDO15] = {
		.id = "RSV_4FEM-vcc",
		.min_uV = 1800000,
		.max_uV = 3000000,
	},
	[PMIC_HI6551_LDO16] = {
		.id = "HI6551_LDO16-vcc",
		.min_uV = 1800000,
		.max_uV = 3000000,
	},
	[PMIC_HI6551_LDO17] = {
		.id = "hsic_phy_avdd-vcc",
		.min_uV = 900000,
		.max_uV = 1250000,
	},
	[PMIC_HI6551_LDO18] = {
		.id = "HI6551_LDO18-vcc",
		.min_uV = 1800000,
		.max_uV = 3000000,
	},
	[PMIC_HI6551_LDO19] = {
		.id = "HI6551_LDO19-vcc",
		.min_uV = 900000,
		.max_uV = 1075000,
	},
	[PMIC_HI6551_LDO20] = {
		.id = "HI6551_LDO20-vcc",
		.min_uV = 2500000,
		.max_uV = 3100000,
	},
	[PMIC_HI6551_LDO21] = {
		.id = "HI6551_LDO21-vcc",
		.min_uV = 1200000,
		.max_uV = 1800000,
	},
	[PMIC_HI6551_LDO22] = {
		.id = "SD_IO_M-vcc",
		.min_uV = 1400000,
		.max_uV = 1525000,
	},
	[PMIC_HI6551_LDO23] = {
		.id = "EFUSE-vcc",
		.min_uV = 1500000,
		.max_uV = 2850000,
	},
	[PMIC_HI6551_LDO24] = {
		.id = "HKADC-vcc",
		.min_uV = 2800000,
		.max_uV = 3150000,
	},
	[PMIC_HI6551_LVS02] = {
		.id = "HI6551_LVS2-vcc",
		.min_uV = 1100000,
		.max_uV = 2850000,
	},
	[PMIC_HI6551_LVS03] = {
		.id = "HI6551_LVS3-vcc",
		.min_uV = 1800000,
		.max_uV = 1800000,
	},
	[PMIC_HI6551_LVS04] = {
		.id = "LCD_IO-vcc",
		.min_uV = 1800000,
		.max_uV = 1800000,
	},
	[PMIC_HI6551_LVS05] = {
		.id = "PA_STAR_VDD-vcc",
		.min_uV = 1800000,
		.max_uV = 1800000,
	},
	[PMIC_HI6551_LVS06] = {
		.id = "HI6551_LVS6-vcc",
		.min_uV = 1800000,
		.max_uV = 1800000,
	},
	[PMIC_HI6551_LVS07] = {
		.id = "HI6551_LVS7-vcc",
		.min_uV = 1800000,
		.max_uV = 1800000,
	},
	[PMIC_HI6551_LVS08] = {
		.id = "HI6551_LVS8-vcc",
		.min_uV = 1800000,
		.max_uV = 1800000,
	},
	[PMIC_HI6551_LVS09] = {
		.id = "HI6551_LVS9-vcc",
		.min_uV = 1800000,
		.max_uV = 1800000,
	},
	[PMIC_HI6551_LVS10] = {
		.id = "HI6551_LVS10-vcc",
		.min_uV = 1800000,
		.max_uV = 1800000,
	},

};
int hi6551_regulator_auto_test003(void)
{
	int ret = 1;
	int i = 0;
	struct regulator *auto_test_regu = ERR_PTR(-EPROBE_DEFER);
	for (i = PMIC_HI6551_BUCK1; i <= PMIC_HI6551_LVS10; i++)
	{
		if (i == PMIC_HI6551_BUCK1 || i == PMIC_HI6551_BUCK2 || i == PMIC_HI6551_BUCK3 || i == PMIC_HI6551_BUCK4 || i == PMIC_HI6551_BUCK6)
			continue;
		if (i == PMIC_HI6551_LDO12 || i == PMIC_HI6551_LDO02 || i == PMIC_HI6551_LDO03 || i == PMIC_HI6551_LDO04 || i == PMIC_HI6551_LDO06
			|| i == PMIC_HI6551_LDO08 || i == PMIC_HI6551_LDO09 || i == PMIC_HI6551_LDO12 || i == PMIC_HI6551_LDO13 || i == PMIC_HI6551_LDO16
				|| i == PMIC_HI6551_LDO18 || i == PMIC_HI6551_LDO19 || i == PMIC_HI6551_LDO20 || i == PMIC_HI6551_LDO21)
			continue;
		if (i == PMIC_HI6551_LVS05 || i == PMIC_HI6551_LVS09 || i == PMIC_HI6551_LVS02 || i == PMIC_HI6551_LVS03 || i == PMIC_HI6551_LVS06
			|| i == PMIC_HI6551_LVS07 || i == PMIC_HI6551_LVS08 || i == PMIC_HI6551_LVS10)
			continue;
		auto_test_regu = regulator_get(NULL, hi6551_supply_test_arr[i].id);
		if (IS_ERR(auto_test_regu))
		{
			ret = 1;
			regu_pr_err("get regulator error id is:%s\n", hi6551_supply_test_arr[i].id);
			goto out;
		}
		ret = regulator_is_enabled(auto_test_regu);
		ret = regulator_enable(auto_test_regu);
		if (ret)
		{
			regu_pr_err("enable regulator error id is:%s\n", hi6551_supply_test_arr[i].id);
			goto out;
		}
		ret = regulator_disable(auto_test_regu);
		if (ret)
		{
			regu_pr_err("enable regulator error id is:%s\n", hi6551_supply_test_arr[i].id);
			goto out;
		}
		if (i >= PMIC_HI6551_LVS02 && i <= PMIC_HI6551_LVS10)
		{
			continue;
		}
		ret = regulator_get_voltage(auto_test_regu);
		ret = regulator_set_voltage(auto_test_regu, hi6551_supply_test_arr[i].min_uV, hi6551_supply_test_arr[i].min_uV);
		if (ret)
		{
			regu_pr_err("set regulator voltage error id is:%s min volt: %d\n", hi6551_supply_test_arr[i].id, hi6551_supply_test_arr[i].min_uV);
			goto out;
		}
		ret = regulator_set_voltage(auto_test_regu, hi6551_supply_test_arr[i].min_uV, hi6551_supply_test_arr[i].max_uV);
		if (ret)
		{
			regu_pr_err("set regulator voltage error id is:%s max volt: %d\n", hi6551_supply_test_arr[i].id, hi6551_supply_test_arr[i].max_uV);
			goto out;
		}
	}
out:
	return ret;
}

#else
int hi6551_regulator_auto_test003(void)
{
	regu_pr_err("macro 6551 not open\n");
	return 0;
}
#endif
struct regulator_supply_test p531_mtcmos_supply_test_arr[] = {
	[P531_MTCMOS_COUNT1] = {
		.id = "tcsdrv1-vcc",
	},
	[P531_MTCMOS_COUNT2] = {
		.id = "fpgaif-vcc",
	},
	[P531_MTCMOS_COUNT3] = {
		.id = "t570t-vcc",
	},
	[P531_MTCMOS_COUNT4] = {
		.id = "a15-vcc",
	},
	[P531_MTCMOS_COUNT5] = {
		.id = "hsicphy-vcc",
	},
	[P531_MTCMOS_COUNT6] = {
		.id = "usbphy-vcc",
	},
	[P531_MTCMOS_COUNT7] = {
		.id = "appa9-vcc",
	},
	[P531_MTCMOS_COUNT8] = {
		.id = "hifi-vcc",
	},
	[P531_MTCMOS_COUNT9] = {
		.id = "bbe16-vcc",
	},
	[P531_MTCMOS_COUNT10] = {
		.id = "moda9-vcc",
	},
};

struct regulator_supply_test v7r2_mtcmos_supply_test_arr[] = {
	[V7R2_MTCMOS_COUNT1] = {
		.id = "sd_mtcmos-vcc",
	},
	[V7R2_MTCMOS_COUNT2] = {
		.id = "dwc3_usb-vcc",
	},
	[V7R2_MTCMOS_COUNT3] = {
		.id = "irm_mtcmos-vcc",
	},
	[V7R2_MTCMOS_COUNT4] = {
		.id = "g2bbp_mtcmos-vcc",
	},
	[V7R2_MTCMOS_COUNT5] = {
		.id = "g1bbp_mtcmos-vcc",
	},
	[V7R2_MTCMOS_COUNT6] = {
		.id = "wbbp_mtcmos-vcc",
	},
	[V7R2_MTCMOS_COUNT7] = {
		.id = "twbbp_mtcmos-vcc",
	},
	[V7R2_MTCMOS_COUNT8] = {
		.id = "lbbp_mtcmos-vcc",
	},
	[V7R2_MTCMOS_COUNT9] = {
		.id = "hifi_mtcmos-vcc",
	},
	[V7R2_MTCMOS_COUNT10] = {
		.id = "dsp0_mtcmos-vcc",
	},
	[V7R2_MTCMOS_COUNT11] = {
		.id = "hsic_phy_vdd-vcc",
	},
	[V7R2_MTCMOS_COUNT12] = {
		.id = "moda9_mtcmos-vcc",
	},
	[V7R2_MTCMOS_COUNT13] = {
		.id = "appa9_mtcmos-vcc",
	},
};
#ifdef REGULATOR_P531_MTCMOS
int p531_mtcmos_regulator_auto_test004(void)
{
	int ret = 1;
	int i = 0;
	struct regulator *auto_test_regu = ERR_PTR(-EPROBE_DEFER);
	for (i = P531_MTCMOS_COUNT1; i <= P531_MTCMOS_COUNT10; i++)
	{

		auto_test_regu = regulator_get(NULL, p531_mtcmos_supply_test_arr[i].id);
		if (IS_ERR(auto_test_regu))
		{
			ret = 1;
			regu_pr_err("get regulator error id is:%s\n", p531_mtcmos_supply_test_arr[i].id);
			goto out;
		}
		if (i == P531_MTCMOS_COUNT7 || i == P531_MTCMOS_COUNT10)
		{
			continue;
		}
		ret = regulator_enable(auto_test_regu);
		if (ret)
		{
			regu_pr_err("enable regulator error id is:%s\n", p531_mtcmos_supply_test_arr[i].id);
			goto out;
		}
		ret = regulator_disable(auto_test_regu);
		if (ret)
		{
			regu_pr_err("enable regulator error id is:%s\n", p531_mtcmos_supply_test_arr[i].id);
			goto out;
		}
	}
out:
	return ret;
}
#else
int p531_mtcmos_regulator_auto_test004(void)
{
	regu_pr_err("macro p531 not open\n");
	return 0;
}
#endif

#ifdef REGULATOR_V7R2_MTCMOS
int v7r2_mtcmos_regulator_auto_test005(void)
{
	int ret = 1;
	int i = 0;
	struct regulator *auto_test_regu = ERR_PTR(-EPROBE_DEFER);
	for (i = V7R2_MTCMOS_COUNT1; i <= V7R2_MTCMOS_COUNT13; i++)
	{
		auto_test_regu = regulator_get(NULL, v7r2_mtcmos_supply_test_arr[i].id);
		if (IS_ERR(auto_test_regu))
		{
			ret = 1;
			regu_pr_err("get regulator error id is:%s\n", v7r2_mtcmos_supply_test_arr[i].id);
			goto out;
		}
		if (i == V7R2_MTCMOS_COUNT12 || i == V7R2_MTCMOS_COUNT13)
		{
			continue;
		}
		regulator_is_enabled(auto_test_regu);
		ret = regulator_enable(auto_test_regu);
		if (ret)
		{
			regu_pr_err("enable regulator error id is:%s\n", v7r2_mtcmos_supply_test_arr[i].id);
			goto out;
		}
		ret = regulator_disable(auto_test_regu);
		if (ret)
		{
			regu_pr_err("disable regulator error id is:%s\n", v7r2_mtcmos_supply_test_arr[i].id);
			goto out;
		}
	}
out:
	return ret;
}
#else
int v7r2_mtcmos_regulator_auto_test005(void)
{
	regu_pr_err("macro v7r2 not open\n");
	return 0;
}
#endif
#ifdef CONFIG_PMIC_HI6451
struct regulator *regu_test = NULL;
int regulator_get_test001(int flag)
{
	if (PMIC_HI6451_BUCK1 <= flag && flag <= PMIC_HI6451_LDO15)
	regu_test = regulator_get(NULL, hi6451_supply_test_arr[flag].id);
	if (IS_ERR(regu_test) || NULL == regu_test)
	{
		regu_pr_err("get regulator failed ! flag->%d\n", flag);
		return AUTO_TEST_RETURN_ERROR;
	}
	return AUTO_TEST_RETURN_OK;
}
int regulator_enable_test002(void)
{
	int ret;
	ret = regulator_enable(regu_test);
	return ret;
}
int regulator_disable_test003(void)
{
	int ret;
	ret = regulator_disable(regu_test);
	return ret;
}
int regulator_set_voltage_test004(int flag, int volt)
{
	int ret = 0;
	ret = regulator_set_voltage(regu_test, volt, volt);
	if (ret < 0 || ret == 1)
	{
		regu_pr_err("set voltage error !\n flag->%d", flag);
		return AUTO_TEST_RETURN_ERROR;
	}
	return AUTO_TEST_RETURN_OK;
}
int regulator_get_voltage_test005(void)
{
	int ret = 0;
	ret = regulator_get_voltage(regu_test);
	return ret;
}
#endif
/******************************自动化测试 end**************************/
