
#include <linux/init.h>
#include <linux/module.h>

#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <bsp_pmu.h>
#include "regulator_balong.h"
#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

extern struct regulator_ops balong_regulator_ldo_ops;

/*********************************HI6451 REGULATOR START************************/
#ifdef CONFIG_PMIC_HI6451
struct regulator_desc hi6451_regulators_desc[] = {
	[PMIC_HI6451_BUCK1] = {
		.name = "HI6451_BUCK1",
		.id = HI6451_BUCK1 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_BUCK2] = {
		.name = "HI6451_BUCK2",
		.id = HI6451_BUCK2 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_BUCK3] = {
		.name = "HI6451_BUCK3",
		.id = HI6451_BUCK3 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_BUCK4] = {
		.name = "HI6451_BUCK4",
		.id = HI6451_BUCK4 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_BUCK5] = {
		.name = "HI6451_BUCK5",
		.id = HI6451_BUCK5 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_BUCK6] = {
		.name = "HI6451_BUCK5",
		.id = HI6451_BUCK6 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO1] = {
		.name = "HI6451_LDO1",
		.id = HI6451_LDO1 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO2] = {
		.name = "HI6451_LDO2",
		.id = HI6451_LDO2 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO3] = {
		.name = "HI6451_LDO3",
		.id = HI6451_LDO3 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO4] = {
		.name = "HI6451_LDO4",
		.id = HI6451_LDO4 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO5] = {
		.name = "HI6451_LDO5",
		.id = HI6451_LDO5 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO6] = {
		.name = "HI6451_LDO6",
		.id = HI6451_LDO6 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO7] = {
		.name = "HI6451_LDO7",
		.id = HI6451_LDO7 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO8] = {
		.name = "HI6451_LDO8",
		.id = HI6451_LDO8 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO9] = {
		.name = "HI6451_LDO9",
		.id = HI6451_LDO9 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO10] = {
		.name = "HI6451_LDO10",
		.id = HI6451_LDO10 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO11] = {
		.name = "HI6451_LDO11",
		.id = HI6451_LDO11 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO12] = {
		.name = "HI6451_LDO12",
		.id = HI6451_LDO12 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO13] = {
		.name = "HI6451_LDO13",
		.id = HI6451_LDO13 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO14] = {
		.name = "HI6451_LDO14",
		.id = HI6451_LDO14 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_LDO15] = {
		.name = "HI6451_LDO15",
		.id = HI6451_LDO15 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_DR1] = {
		.name = "HI6451_DR1",
		.id = HI6451_DR1 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_DR2] = {
		.name = "HI6451_DR2",
		.id = HI6451_DR2 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6451_DR3] = {
		.name = "HI6451_DR3",
		.id = HI6451_DR3 | HI6451_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
};

static struct regulator_consumer_supply hi6451_ldo1_consumers[] = {
	REGULATOR_SUPPLY("LDO1-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_ldo2_consumers[] = {
	REGULATOR_SUPPLY("LDO2-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_ldo3_consumers[] = {
	REGULATOR_SUPPLY("LDO3-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_ldo4_consumers[] = {
	REGULATOR_SUPPLY("LDO4-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_ldo5_consumers[] = {
	REGULATOR_SUPPLY("LDO5-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_ldo6_consumers[] = {
	REGULATOR_SUPPLY("TCXO-vcc", NULL),
};
/*jingyong*/
static struct regulator_consumer_supply hi6451_ldo7_consumers[] = {
	REGULATOR_SUPPLY("SD_IO-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_ldo8_consumers[] = {
	REGULATOR_SUPPLY("LDO8-vcc", NULL),
};

static struct regulator_consumer_supply hi6451_ldo9_consumers[] = {
	REGULATOR_SUPPLY("LDO9-vcc", NULL),
};
/*jingyong*/
static struct regulator_consumer_supply hi6451_ldo10_consumers[] = {
	REGULATOR_SUPPLY("SD_POWER-vcc", NULL),
};
/*yangzhi*/
static struct regulator_consumer_supply hi6451_ldo11_consumers[] = {
	REGULATOR_SUPPLY("SIM1-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_ldo12_consumers[] = {
	REGULATOR_SUPPLY("P530_USB-vcc", NULL),
	REGULATOR_SUPPLY("PCIE_CLK-vcc", NULL),
	REGULATOR_SUPPLY("384TCXO-vcc", NULL),
	REGULATOR_SUPPLY("ETH_CLK-vcc", NULL),
};
/*yangzhi*/
static struct regulator_consumer_supply hi6451_ldo13_consumers[] = {
	REGULATOR_SUPPLY("SIM0-vcc", NULL),
};
/*liujia*/
static struct regulator_consumer_supply hi6451_ldo14_consumers[] = {
	REGULATOR_SUPPLY("LCD-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_ldo15_consumers[] = {
	REGULATOR_SUPPLY("LDO15-vcc", NULL),
};

static struct regulator_consumer_supply hi6451_buck1_consumers[] = {
	REGULATOR_SUPPLY("BUCK1-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_buck2_consumers[] = {
	REGULATOR_SUPPLY("BUCK2-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_buck3_consumers[] = {
	REGULATOR_SUPPLY("JTAG245-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_buck4_consumers[] = {
	REGULATOR_SUPPLY("PCM_CLK-vcc", NULL),
	REGULATOR_SUPPLY("LCD_LED-vcc", NULL),
	REGULATOR_SUPPLY("ETH_LED-vcc", NULL),
	REGULATOR_SUPPLY("UART-vcc", NULL),
	REGULATOR_SUPPLY("LED-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_buck5_consumers[] = {
	REGULATOR_SUPPLY("BUCK5-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_buck6_consumers[] = {
	REGULATOR_SUPPLY("EMMC_NAND-vcc", NULL),
	REGULATOR_SUPPLY("PWR1220-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_dr1_consumers[] = {
	REGULATOR_SUPPLY("DR1-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_dr2_consumers[] = {
	REGULATOR_SUPPLY("DR2-vcc", NULL),
};
static struct regulator_consumer_supply hi6451_dr3_consumers[] = {
	REGULATOR_SUPPLY("DR3-vcc", NULL),
};

struct regulator_init_data hi6451_regulators_init[] = {
	[PMIC_HI6451_BUCK1] = {
	.constraints = {
		.name = "HI6451_BUCK1",
		.min_uV = 800000,
		.max_uV = 1850000,
		/*REGULATOR_CHANGE_VOLTAGE:电压是否可变;REGULATOR_CHANGE_STATUS:状态是否可变enable/disable \
			REGULATOR_CHANGE_MODE:valid_modes_mask中的模式，normal/idle/sleep等；REGULATOR_CHANGE_DRMS:支持动态调压?*/
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,
	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_buck1_consumers),
	.consumer_supplies = hi6451_buck1_consumers,
	},
	[PMIC_HI6451_BUCK2] = {
	.constraints = {
		.name = "HI6451_BUCK2",
		.min_uV = 900000,
		.max_uV = 1300000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_buck2_consumers),
	.consumer_supplies = hi6451_buck2_consumers,

	},
	[PMIC_HI6451_BUCK3] = {
	.constraints = {
		.name = "HI6451_BUCK3",
		.min_uV = 1200000,
		.max_uV = 2850000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,
	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_buck3_consumers),
	.consumer_supplies = hi6451_buck3_consumers,
	},
	[PMIC_HI6451_BUCK4] = {
	.constraints = {
		.name = "HI6451_BUCK4",
		.min_uV = 2900000,
		.max_uV = 3600000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,
	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_buck4_consumers),
	.consumer_supplies = hi6451_buck4_consumers,
	},
	[PMIC_HI6451_BUCK5] = {
	.constraints = {
		.name = "HI6451_BUCK5",
		.min_uV = 1200000,
		.max_uV = 2850000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,
	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_buck5_consumers),
	.consumer_supplies = hi6451_buck5_consumers,
	},
	[PMIC_HI6451_BUCK6] = {
	.constraints = {
		.name = "HI6451_BUCK6",
		.min_uV = 1200000,
		.max_uV = 3450000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_buck6_consumers),
	.consumer_supplies = hi6451_buck6_consumers,
	},
	[PMIC_HI6451_LDO1] = {
		.constraints = {
			.name = "HI6451_LDO1",
			.min_uV = 1100000,
			.max_uV = 1850000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo1_consumers),
		.consumer_supplies = hi6451_ldo1_consumers,
	},
	[PMIC_HI6451_LDO2] = {
	.constraints = {
		.name = "HI6451_LDO2",
		.min_uV = 1100000,
		.max_uV = 2850000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,
	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo2_consumers),
	.consumer_supplies = hi6451_ldo2_consumers,
	},
	[PMIC_HI6451_LDO3] = {
	.constraints = {
		.name = "HI6451_LDO3",
		.min_uV = 1100000,
		.max_uV = 2850000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,
	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo3_consumers),
	.consumer_supplies = hi6451_ldo3_consumers,
	},
	[PMIC_HI6451_LDO4] = {
	.constraints = {
		.name = "HI6451_LDO4",
		.min_uV = 1100000,
		.max_uV = 2850000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,
	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo4_consumers),
	.consumer_supplies = hi6451_ldo4_consumers,
	},
	[PMIC_HI6451_LDO5] = {
	.constraints = {
		.name = "HI6451_LDO5",
		.min_uV = 1100000,
		.max_uV = 2600000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,
	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo5_consumers),
	.consumer_supplies = hi6451_ldo5_consumers,
	},
	[PMIC_HI6451_LDO6] = {
	.constraints = {
		.name = "HI6451_LDO6",
		.min_uV = 2200000,
		.max_uV = 2850000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo6_consumers),
	.consumer_supplies = hi6451_ldo6_consumers,
	},
	[PMIC_HI6451_LDO7] = {
	.constraints = {
		.name = "HI6451_LDO7",
		.min_uV = 1200000,
		.max_uV = 3500000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo7_consumers),
	.consumer_supplies = hi6451_ldo7_consumers,
	},
	[PMIC_HI6451_LDO8] = {
	.constraints = {
		.name = "HI6451_LDO8",
		.min_uV = 900000,
		.max_uV = 1500000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo8_consumers),
	.consumer_supplies = hi6451_ldo8_consumers,
	},
	[PMIC_HI6451_LDO9] = {
	.constraints = {
		.name = "HI6451_LDO9",
		.min_uV = 900000,
		.max_uV = 1500000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo9_consumers),
	.consumer_supplies = hi6451_ldo9_consumers,
	},
	[PMIC_HI6451_LDO10] = {
	.constraints = {
		.name = "HI6451_LDO10",
		.min_uV = 1200000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo10_consumers),
	.consumer_supplies = hi6451_ldo10_consumers,
	},
	[PMIC_HI6451_LDO11] = {
	.constraints = {
		.name = "HI6451_LDO11",
		.min_uV = 1200000,
		.max_uV = 3050000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo11_consumers),
	.consumer_supplies = hi6451_ldo11_consumers,
	},
	[PMIC_HI6451_LDO12] = {
	.constraints = {
		.name = "HI6451_LDO12",
		.min_uV = 3300000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo12_consumers),
	.consumer_supplies = hi6451_ldo12_consumers,
	},
	[PMIC_HI6451_LDO13] = {
	.constraints = {
		.name = "HI6451_LDO13",
		.min_uV = 1100000,
		.max_uV = 3000000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo13_consumers),
	.consumer_supplies = hi6451_ldo13_consumers,
	},
	[PMIC_HI6451_LDO14] = {
	.constraints = {
		.name = "HI6451_LDO14",
		.min_uV = 1100000,
		.max_uV = 2850000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo14_consumers),
	.consumer_supplies = hi6451_ldo14_consumers,
	},
	[PMIC_HI6451_LDO15] = {
	.constraints = {
		.name = "HI6451_LDO15",
		.min_uV = 1100000,
		.max_uV = 2850000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_ldo15_consumers),
	.consumer_supplies = hi6451_ldo15_consumers,
	},
	[PMIC_HI6451_DR1] = {
	.constraints = {
		.name = "HI6451_DR1",
		.min_uA = 3000,
		.max_uA = 24000,
		.valid_ops_mask = REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_dr1_consumers),
	.consumer_supplies = hi6451_dr1_consumers,
	},
	[PMIC_HI6451_DR2] = {
	.constraints = {
		.name = "HI6451_DR2",
		.min_uA = 3000,
		.max_uA = 24000,
		.valid_ops_mask = REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_dr2_consumers),
	.consumer_supplies = hi6451_dr2_consumers,
	},
	[PMIC_HI6451_DR3] = {
	.constraints = {
		.name = "HI6451_DR3",
		.min_uA = 3000,
		.max_uA = 24000,
		.valid_ops_mask = REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
		.always_on = 0,

	},
	.num_consumer_supplies = ARRAY_SIZE(hi6451_dr3_consumers),
	.consumer_supplies = hi6451_dr3_consumers,
	},
};
#endif
/*********************************HI6451 REGULATOR END************************/

/*********************************P531 MTCMOS REGULATOR START************************/
extern struct regulator_ops p531_mtcmos_ops;
#ifdef REGULATOR_P531_MTCMOS
struct regulator_desc p531_mtcmos_regulator_desc[] = {
	[P531_MTCMOS_COUNT1] = {
		.name = "P531_MTCMOS1",
		.id = P531_MTCMOS1 | P531_MTCMOS_FLAG,
		.ops = &p531_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[P531_MTCMOS_COUNT2] = {
		.name = "P531_MTCMOS2",
		.id = P531_MTCMOS2 | P531_MTCMOS_FLAG,
		.ops = &p531_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[P531_MTCMOS_COUNT3] = {
		.name = "P531_MTCMOS3",
		.id = P531_MTCMOS3 | P531_MTCMOS_FLAG,
		.ops = &p531_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[P531_MTCMOS_COUNT4] = {
		.name = "P531_MTCMOS4",
		.id = P531_MTCMOS4 | P531_MTCMOS_FLAG,
		.ops = &p531_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[P531_MTCMOS_COUNT5] = {
		.name = "P531_MTCMOS5",
		.id = P531_MTCMOS5 | P531_MTCMOS_FLAG,
		.ops = &p531_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[P531_MTCMOS_COUNT6] = {
		.name = "P531_MTCMOS6",
		.id = P531_MTCMOS6 | P531_MTCMOS_FLAG,
		.ops = &p531_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[P531_MTCMOS_COUNT7] = {
		.name = "P531_MTCMOS7",
		.id = P531_MTCMOS7 | P531_MTCMOS_FLAG,
		.ops = &p531_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[P531_MTCMOS_COUNT8] = {
		.name = "P531_MTCMOS8",
		.id = P531_MTCMOS8 | P531_MTCMOS_FLAG,
		.ops = &p531_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[P531_MTCMOS_COUNT9] = {
		.name = "P531_MTCMOS9",
		.id = P531_MTCMOS9 | P531_MTCMOS_FLAG,
		.ops = &p531_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[P531_MTCMOS_COUNT10] = {
		.name = "P531_MTCMOS10",
		.id = P531_MTCMOS10 | P531_MTCMOS_FLAG,
		.ops = &p531_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
};
/*与芯片确认，软件不可控*/
static struct regulator_consumer_supply tcsdrv1_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("tcsdrv1-vcc", NULL),
};
static struct regulator_consumer_supply fpgaif_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("fpgaif-vcc", NULL),
};
static struct regulator_consumer_supply t570t_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("t570t-vcc", NULL),
};
static struct regulator_consumer_supply a15_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("a15-vcc", NULL),
};

static struct regulator_consumer_supply hsicphy_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("hsic_phy_vdd-vcc", NULL),
};
/*与芯片确认，USBPHY无mtcmos*/
static struct regulator_consumer_supply usbphy_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("usbphy-vcc", NULL),
};
static struct regulator_consumer_supply appa9_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("appa9-vcc", NULL),
};
static struct regulator_consumer_supply hifi_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("hifi-vcc", NULL),
};
static struct regulator_consumer_supply bbe16_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("bbe16-vcc", NULL),
};
static struct regulator_consumer_supply moda9_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("moda9-vcc", NULL),
};
struct regulator_init_data mtcmos_regulators_init[] = {
	[P531_MTCMOS_COUNT1] = {
	.constraints = {
		.name = "P531_MTCMOS1",
		/*REGULATOR_CHANGE_VOLTAGE:电压是否可变;REGULATOR_CHANGE_STATUS:状态是否可变enable/disable \
			REGULATOR_CHANGE_MODE:valid_modes_mask中的模式，normal/idle/sleep等；REGULATOR_CHANGE_DRMS:支持动态调压?*/
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(tcsdrv1_mtcmos_consumers),
	.consumer_supplies = tcsdrv1_mtcmos_consumers,
	},
	[P531_MTCMOS_COUNT2] = {
	.constraints = {
		.name = "P531_MTCMOS2",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(fpgaif_mtcmos_consumers),
	.consumer_supplies = fpgaif_mtcmos_consumers,
	},
	[P531_MTCMOS_COUNT3] = {
	.constraints = {
		.name = "P531_MTCMOS3",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(t570t_mtcmos_consumers),
	.consumer_supplies = t570t_mtcmos_consumers,
	},
	[P531_MTCMOS_COUNT4] = {
	.constraints = {
		.name = "P531_MTCMOS4",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(a15_mtcmos_consumers),
	.consumer_supplies = a15_mtcmos_consumers,
	},
	[P531_MTCMOS_COUNT5] = {
	.constraints = {
		.name = "P531_MTCMOS5",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(hsicphy_mtcmos_consumers),
	.consumer_supplies = hsicphy_mtcmos_consumers,
	},
	[P531_MTCMOS_COUNT6] = {
	.constraints = {
		.name = "P531_MTCMOS6",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(usbphy_mtcmos_consumers),
	.consumer_supplies = usbphy_mtcmos_consumers,
	},
	[P531_MTCMOS_COUNT7] = {
	.constraints = {
		.name = "P531_MTCMOS7",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(appa9_mtcmos_consumers),
	.consumer_supplies = appa9_mtcmos_consumers,
	},
	[P531_MTCMOS_COUNT8] = {
	.constraints = {
		.name = "P531_MTCMOS8",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(hifi_mtcmos_consumers),
	.consumer_supplies = hifi_mtcmos_consumers,
	},
	[P531_MTCMOS_COUNT9] = {
	.constraints = {
		.name = "P531_MTCMOS9",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(bbe16_mtcmos_consumers),
	.consumer_supplies = bbe16_mtcmos_consumers,
	},
	[P531_MTCMOS_COUNT10] = {
	.constraints = {
		.name = "P531_MTCMOS10",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(moda9_mtcmos_consumers),
	.consumer_supplies = moda9_mtcmos_consumers,
	},
};
#endif
/***************************HI6551 MTCMOS REGULATOR START*****************/
extern struct regulator_ops v7r2_mtcmos_ops;
#ifdef REGULATOR_V7R2_MTCMOS
struct regulator_desc v7r2_mtcmos_regulator_desc[] = {
	[V7R2_MTCMOS_COUNT1] = {
		.name = "V7R2_MTCMOS1",
		.id = V7R2_MTCMOS1 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT2] = {
		.name = "V7R2_MTCMOS2",
		.id = V7R2_MTCMOS2 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT3] = {
		.name = "V7R2_MTCMOS3",
		.id = V7R2_MTCMOS3 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT4] = {
		.name = "V7R2_MTCMOS4",
		.id = V7R2_MTCMOS4 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT5] = {
		.name = "V7R2_MTCMOS5",
		.id = V7R2_MTCMOS5 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT6] = {
		.name = "V7R2_MTCMOS6",
		.id = V7R2_MTCMOS6 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT7] = {
		.name = "V7R2_MTCMOS7",
		.id = V7R2_MTCMOS7 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT8] = {
		.name = "V7R2_MTCMOS8",
		.id = V7R2_MTCMOS8 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT9] = {
		.name = "V7R2_MTCMOS9",
		.id = V7R2_MTCMOS9 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT10] = {
		.name = "V7R2_MTCMOS10",
		.id = V7R2_MTCMOS10 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT11] = {
		.name = "V7R2_MTCMOS11",
		.id = V7R2_MTCMOS11 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT12] = {
		.name = "V7R2_MTCMOS12",
		.id = V7R2_MTCMOS12 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V7R2_MTCMOS_COUNT13] = {
		.name = "V7R2_MTCMOS13",
		.id = V7R2_MTCMOS13 | V7R2_MTCMOS_FLAG,
		.ops = &v7r2_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
};
static struct regulator_consumer_supply v7r2_sd_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("sd_mtcmos-vcc", NULL),
	REGULATOR_SUPPLY("hsic_pmu-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_usb_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("dwc3_usb-vcc", NULL),
	REGULATOR_SUPPLY("hsic_pd-vcc", NULL),
	REGULATOR_SUPPLY("sd_mmc0_mtcmos-vcc", NULL),
	REGULATOR_SUPPLY("pcie_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_irm_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("irm_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_g2bbp_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("g2bbp_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_g1bbp_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("g1bbp_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_wbbp_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("wbbp_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_twbbp_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("twbbp_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_lbbp_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("lbbp_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_hifi_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("hifi_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_dsp0_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("dsp0_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_hisc_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("hsic_phy_vdd-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_moda9_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("moda9_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply v7r2_appa9_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("appa9_mtcmos-vcc", NULL),
};
struct regulator_init_data v7r2_mtcmos_regulators_init[] = {
	[V7R2_MTCMOS_COUNT1] = {
	.constraints = {
		.name = "V7R2_MTCMOS1",
		/*REGULATOR_CHANGE_VOLTAGE:电压是否可变;REGULATOR_CHANGE_STATUS:状态是否可变enable/disable \
			REGULATOR_CHANGE_MODE:valid_modes_mask中的模式，normal/idle/sleep等；REGULATOR_CHANGE_DRMS:支持动态调压?*/
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_sd_mtcmos_consumers),
	.consumer_supplies = v7r2_sd_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT2] = {
	.constraints = {
		.name = "V7R2_MTCMOS2",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_usb_mtcmos_consumers),
	.consumer_supplies = v7r2_usb_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT3] = {
	.constraints = {
		.name = "V7R2_MTCMOS3",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_irm_mtcmos_consumers),
	.consumer_supplies = v7r2_irm_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT4] = {
	.constraints = {
		.name = "V7R2_MTCMOS4",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_g2bbp_mtcmos_consumers),
	.consumer_supplies = v7r2_g2bbp_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT5] = {
	.constraints = {
		.name = "V7R2_MTCMOS5",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_g1bbp_mtcmos_consumers),
	.consumer_supplies = v7r2_g1bbp_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT6] = {
	.constraints = {
		.name = "V7R2_MTCMOS6",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_wbbp_mtcmos_consumers),
	.consumer_supplies = v7r2_wbbp_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT7] = {
	.constraints = {
		.name = "V7R2_MTCMOS7",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_twbbp_mtcmos_consumers),
	.consumer_supplies = v7r2_twbbp_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT8] = {
	.constraints = {
		.name = "V7R2_MTCMOS8",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_lbbp_mtcmos_consumers),
	.consumer_supplies = v7r2_lbbp_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT9] = {
	.constraints = {
		.name = "V7R2_MTCMOS9",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_hifi_mtcmos_consumers),
	.consumer_supplies = v7r2_hifi_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT10] = {
	.constraints = {
		.name = "V7R2_MTCMOS10",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_dsp0_mtcmos_consumers),
	.consumer_supplies = v7r2_dsp0_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT11] = {
	.constraints = {
		.name = "V7R2_MTCMOS11",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_hisc_mtcmos_consumers),
	.consumer_supplies = v7r2_hisc_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT12] = {
	.constraints = {
		.name = "V7R2_MTCMOS12",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_moda9_mtcmos_consumers),
	.consumer_supplies = v7r2_moda9_mtcmos_consumers,
	},
	[V7R2_MTCMOS_COUNT13] = {
	.constraints = {
		.name = "V7R2_MTCMOS13",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(v7r2_appa9_mtcmos_consumers),
	.consumer_supplies = v7r2_appa9_mtcmos_consumers,
	},
};
#endif

#ifdef REGULATOR_V711_MTCMOS
extern struct regulator_ops vendor_mtcmos_ops;
struct regulator_desc vendor_mtcmos_regulator_desc[] = {
	[V711_MTCMOS_COUNT1] = {
		.name = "V711_MTCMOS1",
		.id = V711_MTCMOS1 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT2] = {
		.name = "V711_MTCMOS2",
		.id = V711_MTCMOS2 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT3] = {
		.name = "V711_MTCMOS3",
		.id = V711_MTCMOS3 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT4] = {
		.name = "V711_MTCMOS4",
		.id = V711_MTCMOS4 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT5] = {
		.name = "V711_MTCMOS5",
		.id = V711_MTCMOS5 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT6] = {
		.name = "V711_MTCMOS6",
		.id = V711_MTCMOS6 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT7] = {
		.name = "V711_MTCMOS7",
		.id = V711_MTCMOS7 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT8] = {
		.name = "V711_MTCMOS8",
		.id = V711_MTCMOS8 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT9] = {
		.name = "V711_MTCMOS9",
		.id = V711_MTCMOS9 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT10] = {
		.name = "V711_MTCMOS10",
		.id = V711_MTCMOS10 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT11] = {
		.name = "V711_MTCMOS11",
		.id = V711_MTCMOS11 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[V711_MTCMOS_COUNT12] = {
		.name = "V711_MTCMOS12",
		.id = V711_MTCMOS12 | V711_MTCMOS_FLAG,
		.ops = &vendor_mtcmos_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
};
static struct regulator_consumer_supply vendor_sd_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("sd_mtcmos-vcc", NULL),/* now in peri_usb power field */
	REGULATOR_SUPPLY("sd_mmc0_mtcmos-vcc", NULL),/* now in peri_usb power field */
};
static struct regulator_consumer_supply vendor_usb_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("dwc3_usb-vcc", NULL),
	REGULATOR_SUPPLY("hsic_pd-vcc", NULL),
	REGULATOR_SUPPLY("pcie_mtcmos-vcc", NULL),
	REGULATOR_SUPPLY("hsic_pmu-vcc", NULL),
	REGULATOR_SUPPLY("pcie_phy-vcc", NULL),/* no caller */
	REGULATOR_SUPPLY("hsic_phy_vdd-vcc", NULL),/* no caller */
	REGULATOR_SUPPLY("gmac-vcc", NULL),/* no caller */
};
static struct regulator_consumer_supply vendor_irm_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("irm_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply vendor_g2bbp_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("g2bbp_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply vendor_g1bbp_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("g1bbp_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply vendor_wbbp_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("wbbp_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply vendor_twbbp_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("twbbp_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply vendor_lbbp_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("lbbp_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply vendor_hifi_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("hifi_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply vendor_dsp0_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("dsp0_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply vendor_moda9_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("moda9_mtcmos-vcc", NULL),
};
static struct regulator_consumer_supply vendor_appa9_mtcmos_consumers[] = {
	REGULATOR_SUPPLY("appa9_mtcmos-vcc", NULL),
};

struct regulator_init_data vendor_mtcmos_regulators_init[] = {
	[V711_MTCMOS_COUNT1] = {
	.constraints = {
		.name = "V711_MTCMOS1",
		/*REGULATOR_CHANGE_VOLTAGE:电压是否可变;REGULATOR_CHANGE_STATUS:状态是否可变enable/disable \
			REGULATOR_CHANGE_MODE:valid_modes_mask中的模式，normal/idle/sleep等；REGULATOR_CHANGE_DRMS:支持动态调压?*/
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_sd_mtcmos_consumers),
	.consumer_supplies = vendor_sd_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT2] = {
	.constraints = {
		.name = "V711_MTCMOS2",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_usb_mtcmos_consumers),
	.consumer_supplies = vendor_usb_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT3] = {
	.constraints = {
		.name = "V711_MTCMOS3",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_irm_mtcmos_consumers),
	.consumer_supplies = vendor_irm_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT4] = {
	.constraints = {
		.name = "V711_MTCMOS4",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_g2bbp_mtcmos_consumers),
	.consumer_supplies = vendor_g2bbp_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT5] = {
	.constraints = {
		.name = "V711_MTCMOS5",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_g1bbp_mtcmos_consumers),
	.consumer_supplies = vendor_g1bbp_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT6] = {
	.constraints = {
		.name = "V711_MTCMOS6",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_wbbp_mtcmos_consumers),
	.consumer_supplies = vendor_wbbp_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT7] = {
	.constraints = {
		.name = "V711_MTCMOS7",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_twbbp_mtcmos_consumers),
	.consumer_supplies = vendor_twbbp_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT8] = {
	.constraints = {
		.name = "V711_MTCMOS8",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_lbbp_mtcmos_consumers),
	.consumer_supplies = vendor_lbbp_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT9] = {
	.constraints = {
		.name = "V711_MTCMOS9",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_hifi_mtcmos_consumers),
	.consumer_supplies = vendor_hifi_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT10] = {
	.constraints = {
		.name = "V711_MTCMOS10",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_dsp0_mtcmos_consumers),
	.consumer_supplies = vendor_dsp0_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT11] = {
	.constraints = {
		.name = "V711_MTCMOS11",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_moda9_mtcmos_consumers),
	.consumer_supplies = vendor_moda9_mtcmos_consumers,
	},
	[V711_MTCMOS_COUNT12] = {
	.constraints = {
		.name = "V711_MTCMOS12",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(vendor_appa9_mtcmos_consumers),
	.consumer_supplies = vendor_appa9_mtcmos_consumers,
	},
};
#endif

/***************************HI6551 MTCMOS REGULATOR end*****************/

/*********************************HI6551 REGULATOR START************************/
#ifdef CONFIG_PMIC_HI6551

struct regulator_desc hi6551_regulators_desc[] = {
	[PMIC_HI6551_BUCK1] = {
		.name = "HI6551_BUCK1",
		.id = HI6551_BUCK1 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_BUCK2] = {
		.name = "HI6551_BUCK2",
		.id = HI6551_BUCK2 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_BUCK3] = {
		.name = "HI6551_BUCK3",
		.id = HI6551_BUCK3 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_BUCK4] = {
		.name = "HI6551_BUCK4",
		.id = HI6551_BUCK4 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_BUCK5] = {
		.name = "HI6551_BUCK5",
		.id = HI6551_BUCK5 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_BUCK6] = {
		.name = "HI6551_BUCK6",
		.id = HI6551_BUCK6 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_BOOST] = {
		.name = "HI6551_BOOST",
		.id = HI6551_BOOST | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO01] = {
		.name = "HI6551_LDO1",
		.id = HI6551_LDO1 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO02] = {
		.name = "HI6551_LDO2",
		.id = HI6551_LDO2 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO03] = {
		.name = "HI6551_LDO3",
		.id = HI6551_LDO3 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO04] = {
		.name = "HI6551_LDO4",
		.id = HI6551_LDO4 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO05] = {
		.name = "HI6551_LDO5",
		.id = HI6551_LDO5 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO06] = {
		.name = "HI6551_LDO6",
		.id = HI6551_LDO6 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO07] = {
		.name = "HI6551_LDO7",
		.id = HI6551_LDO7 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO08] = {
		.name = "HI6551_LDO8",
		.id = HI6551_LDO8 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO09] = {
		.name = "HI6551_LDO9",
		.id = HI6551_LDO9 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO10] = {
		.name = "HI6551_LDO10",
		.id = HI6551_LDO10 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO11] = {
		.name = "HI6551_LDO11",
		.id = HI6551_LDO11 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO12] = {
		.name = "HI6551_LDO12",
		.id = HI6551_LDO12 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO13] = {
		.name = "HI6551_LDO13",
		.id = HI6551_LDO13 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO14] = {
		.name = "HI6551_LDO14",
		.id = HI6551_LDO14 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO15] = {
		.name = "HI6551_LDO15",
		.id = HI6551_LDO15 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO16] = {
		.name = "HI6551_LDO16",
		.id = HI6551_LDO16 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO17] = {
		.name = "HI6551_LDO17",
		.id = HI6551_LDO17 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO18] = {
		.name = "HI6551_LDO18",
		.id = HI6551_LDO18 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO19] = {
		.name = "HI6551_LDO19",
		.id = HI6551_LDO19 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO20] = {
		.name = "HI6551_LDO20",
		.id = HI6551_LDO20 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO21] = {
		.name = "HI6551_LDO21",
		.id = HI6551_LDO21 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO22] = {
		.name = "HI6551_LDO22",
		.id = HI6551_LDO22 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO23] = {
		.name = "HI6551_LDO23",
		.id = HI6551_LDO23 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LDO24] = {
		.name = "HI6551_LDO24",
		.id = HI6551_LDO24 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LVS02] = {
		.name = "HI6551_LVS2",
		.id = HI6551_LVS2 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LVS03] = {
		.name = "HI6551_LVS3",
		.id = HI6551_LVS3 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LVS04] = {
		.name = "HI6551_LVS4",
		.id = HI6551_LVS4 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LVS05] = {
		.name = "HI6551_LVS5",
		.id = HI6551_LVS5 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LVS06] = {
		.name = "HI6551_LVS6",
		.id = HI6551_LVS6 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LVS07] = {
		.name = "HI6551_LVS7",
		.id = HI6551_LVS7 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LVS08] = {
		.name = "HI6551_LVS8",
		.id = HI6551_LVS8 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LVS09] = {
		.name = "HI6551_LVS9",
		.id = HI6551_LVS9 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6551_LVS10] = {
		.name = "HI6551_LVS10",
		.id = HI6551_LVS10 | HI6551_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
};



static struct regulator_consumer_supply hi6551_buck1_consumers[] = {
	REGULATOR_SUPPLY("HI6551_BUCK1-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_buck2_consumers[] = {
	REGULATOR_SUPPLY("RSV4REIC-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_buck3_consumers[] = {
	REGULATOR_SUPPLY("HI6551_BUCK3-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_buck4_consumers[] = {
	REGULATOR_SUPPLY("LPDDR2-vcc", NULL),
	REGULATOR_SUPPLY("IO_P1-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_buck5_consumers[] = {
	REGULATOR_SUPPLY("RSV4RFIC_BUCK5-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_buck6_consumers[] = {
	REGULATOR_SUPPLY("LPDDR2_VDD1-vcc", NULL),
	REGULATOR_SUPPLY("IO_P3-vcc", NULL),
	REGULATOR_SUPPLY("PMU_IO-vcc", NULL),
	REGULATOR_SUPPLY("WIFI-vcc", NULL),
	REGULATOR_SUPPLY("GPS-vcc", NULL),
	REGULATOR_SUPPLY("FM_IO-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_boost_consumers[] = {
	REGULATOR_SUPPLY("BOOST-vcc", NULL),
};

static struct regulator_consumer_supply hi6551_ldo1_consumers[] = {
	REGULATOR_SUPPLY("HKADC_REF-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo2_consumers[] = {
	REGULATOR_SUPPLY("RSV_4PA_BIAS-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo3_consumers[] = {
	REGULATOR_SUPPLY("SOC-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo4_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LDO4-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo5_consumers[] = {
	REGULATOR_SUPPLY("HI6551_RSV4PABIAS-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo6_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LDO6-vcc", NULL),
};
/*jingyong*/
static struct regulator_consumer_supply hi6551_ldo7_consumers[] = {
	REGULATOR_SUPPLY("SD_IO-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo8_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LDO8-vcc", NULL),
};
/*yangzhi*/
static struct regulator_consumer_supply hi6551_ldo9_consumers[] = {
	REGULATOR_SUPPLY("SIM0-vcc", NULL),
};
/*jingyong*/
static struct regulator_consumer_supply hi6551_ldo10_consumers[] = {
	REGULATOR_SUPPLY("SD_POWER-vcc", NULL),
};
/*yangzhi*/
static struct regulator_consumer_supply hi6551_ldo11_consumers[] = {
	REGULATOR_SUPPLY("SIM1-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo12_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LDO12-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo13_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LDO13-vcc", NULL),
};
/*liujia*/
static struct regulator_consumer_supply hi6551_ldo14_consumers[] = {
	REGULATOR_SUPPLY("LCD-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo15_consumers[] = {
	REGULATOR_SUPPLY("RSV_4FEM-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo16_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LDO16-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo17_consumers[] = {
	REGULATOR_SUPPLY("hsic_phy_avdd-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo18_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LDO18-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo19_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LDO19-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo20_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LDO20-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo21_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LDO21-vcc", NULL),
};
/*jingyong*/
static struct regulator_consumer_supply hi6551_ldo22_consumers[] = {
	REGULATOR_SUPPLY("SD_IO_M-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo23_consumers[] = {
	REGULATOR_SUPPLY("EFUSE-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_ldo24_consumers[] = {
	REGULATOR_SUPPLY("HKADC-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_lvs2_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LVS2-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_lvs3_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LVS3-vcc", NULL),
};
/*liujia*/
static struct regulator_consumer_supply hi6551_lvs4_consumers[] = {
	REGULATOR_SUPPLY("LCD_IO-vcc", NULL),
	//REGULATOR_SUPPLY("TP_IO-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_lvs5_consumers[] = {
	REGULATOR_SUPPLY("PA_STAR_VDD-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_lvs6_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LVS6-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_lvs7_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LVS7-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_lvs8_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LVS8-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_lvs9_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LVS9-vcc", NULL),
};
static struct regulator_consumer_supply hi6551_lvs10_consumers[] = {
	REGULATOR_SUPPLY("HI6551_LVS10-vcc", NULL),
};
struct regulator_init_data hi6551_regulators_init[] = {
	[PMIC_HI6551_BUCK1] = {
		.constraints = {
			.name = "HI6551_BUCK1",
			.min_uV = 700000,
			.max_uV = 1204000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_buck1_consumers),
		.consumer_supplies = hi6551_buck1_consumers,
	},
	[PMIC_HI6551_BUCK2] = {
		.constraints = {
			.name = "HI6551_BUCK2",
			.min_uV = 700000,
			.max_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_buck2_consumers),
		.consumer_supplies = hi6551_buck2_consumers,
	},
	[PMIC_HI6551_BUCK3] = {
		.constraints = {
			.name = "HI6551_BUCK3",
			.min_uV = 750000,
			.max_uV = 1125000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_buck3_consumers),
		.consumer_supplies = hi6551_buck3_consumers,
	},
	[PMIC_HI6551_BUCK4] = {
		.constraints = {
			.name = "HI6551_BUCK4",
			.min_uV = 1200000,
			.max_uV = 1575000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			//.always_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_buck4_consumers),
		.consumer_supplies = hi6551_buck4_consumers,
	},
	[PMIC_HI6551_BUCK5] = {
		.constraints = {
			.name = "HI6551_BUCK5",
			.min_uV = 700000,
			.max_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_buck5_consumers),
		.consumer_supplies = hi6551_buck5_consumers,
	},
	[PMIC_HI6551_BUCK6] = {
		.constraints = {
			.name = "HI6551_BUCK6",
			.min_uV = 1800000,
			.max_uV = 2175000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			//.always_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_buck6_consumers),
		.consumer_supplies = hi6551_buck6_consumers,
	},
	[PMIC_HI6551_BOOST] = {
		.constraints = {
			.name = "HI6551_BOOST",
			.min_uV = 3400000,
			.max_uV = 5500000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_boost_consumers),
		.consumer_supplies = hi6551_boost_consumers,
	},
	[PMIC_HI6551_LDO01] = {
		.constraints = {
			.name = "HI6551_LDO1",
			.min_uV = 1800000,
			.max_uV = 3000000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo1_consumers),
		.consumer_supplies = hi6551_ldo1_consumers,
		//.supply_regulator = "HI6551_BOOST",
	},
	[PMIC_HI6551_LDO02] = {
		.constraints = {
			.name = "HI6551_LDO2",
			.min_uV = 2500000,
			.max_uV = 3300000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo2_consumers),
		.consumer_supplies = hi6551_ldo2_consumers,
		//.supply_regulator = "HI6551_BOOST",
	},
	[PMIC_HI6551_LDO03] = {
		.constraints = {
			.name = "HI6551_LDO3",
			.min_uV = 800000,
			.max_uV = 1000000,
			/*always on software not control*/
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			//.always_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo3_consumers),
		.consumer_supplies = hi6551_ldo3_consumers,
		//.supply_regulator = "HI6551_BUCK4",
	},
	[PMIC_HI6551_LDO04] = {
		.constraints = {
			.name = "HI6551_LDO4",
			.min_uV = 1500000,
			.max_uV = 2800000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo4_consumers),
		.consumer_supplies = hi6551_ldo4_consumers,
	},
	[PMIC_HI6551_LDO05] = {
		.constraints = {
			.name = "HI6551_LDO5",
			.min_uV = 2850000,
			.max_uV = 3150000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo5_consumers),
		.consumer_supplies = hi6551_ldo5_consumers,
	},
	[PMIC_HI6551_LDO06] = {
		.constraints = {
			.name = "HI6551_LDO6",
			.min_uV = 1500000,
			.max_uV = 2800000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo6_consumers),
		.consumer_supplies = hi6551_ldo6_consumers,
	},
	[PMIC_HI6551_LDO07] = {
		.constraints = {
			.name = "HI6551_LDO7",
			.min_uV = 1800000,
			.max_uV = 3050000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo7_consumers),
		.consumer_supplies = hi6551_ldo7_consumers,
		//.supply_regulator = "HI6551_BOOST",
	},
	[PMIC_HI6551_LDO08] = {
		.constraints = {
			.name = "HI6551_LDO8",
			.min_uV = 1100000,
			.max_uV = 1375000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo8_consumers),
		.consumer_supplies = hi6551_ldo8_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LDO09] = {
		.constraints = {
			.name = "HI6551_LDO9",
			.min_uV = 1800000,
			.max_uV = 3050000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo9_consumers),
		.consumer_supplies = hi6551_ldo9_consumers,
	},
	[PMIC_HI6551_LDO10] = {
		.constraints = {
			.name = "HI6551_LDO10",
			.min_uV = 2800000,
			.max_uV = 3150000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo10_consumers),
		.consumer_supplies = hi6551_ldo10_consumers,
	},
	[PMIC_HI6551_LDO11] = {
		.constraints = {
			.name = "HI6551_LDO11",
			.min_uV = 1800000,
			.max_uV = 3050000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo11_consumers),
		.consumer_supplies = hi6551_ldo11_consumers,
	},
	[PMIC_HI6551_LDO12] = {
		.constraints = {
			.name = "HI6551_LDO12",
			.min_uV = 3000000,
			.max_uV = 3350000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo12_consumers),
		.consumer_supplies = hi6551_ldo12_consumers,
	},
	[PMIC_HI6551_LDO13] = {
		.constraints = {
			.name = "HI6551_LDO13",
			.min_uV = 1800000,
			.max_uV = 3000000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo13_consumers),
		.consumer_supplies = hi6551_ldo13_consumers,
	},
	[PMIC_HI6551_LDO14] = {
		.constraints = {
			.name = "HI6551_LDO14",
			.min_uV = 1800000,
			.max_uV = 3000000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo14_consumers),
		.consumer_supplies = hi6551_ldo14_consumers,
		//.supply_regulator = "HI6551_BOOST",
	},
	[PMIC_HI6551_LDO15] = {
		.constraints = {
			.name = "HI6551_LDO15",
			.min_uV = 1800000,
			.max_uV = 3000000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo15_consumers),
		.consumer_supplies = hi6551_ldo15_consumers,
	},
	[PMIC_HI6551_LDO16] = {
		.constraints = {
			.name = "HI6551_LDO16",
			.min_uV = 1800000,
			.max_uV = 3000000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo16_consumers),
		.consumer_supplies = hi6551_ldo16_consumers,
	},[PMIC_HI6551_LDO17] = {
		.constraints = {
			.name = "HI6551_LDO17",
			.min_uV = 900000,
			.max_uV = 1250000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo17_consumers),
		.consumer_supplies = hi6551_ldo17_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LDO18] = {
		.constraints = {
			.name = "HI6551_LDO18",
			.min_uV = 1800000,
			.max_uV = 3000000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo18_consumers),
		.consumer_supplies = hi6551_ldo18_consumers,
	},
	[PMIC_HI6551_LDO19] = {
		.constraints = {
			.name = "HI6551_LDO19",
			.min_uV = 900000,
			.max_uV = 1075000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo19_consumers),
		.consumer_supplies = hi6551_ldo19_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LDO20] = {
		.constraints = {
			.name = "HI6551_LDO20",
			.min_uV = 2500000,
			.max_uV = 3100000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo20_consumers),
		.consumer_supplies = hi6551_ldo20_consumers,
	},
	[PMIC_HI6551_LDO21] = {
		.constraints = {
			.name = "HI6551_LDO21",
			.min_uV = 1200000,
			.max_uV = 1800000,
			//.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo21_consumers),
		.consumer_supplies = hi6551_ldo21_consumers,
	},
	[PMIC_HI6551_LDO22] = {
		.constraints = {
			.name = "HI6551_LDO22",
			.min_uV = 1400000,
			.max_uV = 1525000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo22_consumers),
		.consumer_supplies = hi6551_ldo22_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LDO23] = {
		.constraints = {
			.name = "HI6551_LDO23",
			.min_uV = 1500000,
			.max_uV = 2850000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo23_consumers),
		.consumer_supplies = hi6551_ldo23_consumers,
	},
	[PMIC_HI6551_LDO24] = {
		.constraints = {
			.name = "HI6551_LDO24",
			.min_uV = 2800000,
			.max_uV = 3150000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_ldo24_consumers),
		.consumer_supplies = hi6551_ldo24_consumers,
		//.supply_regulator = "HI6551_BOOST",
	},
	[PMIC_HI6551_LVS02] = {
		.constraints = {
			.name = "HI6551_LVS2",
			.min_uV = 1800000,
			.max_uV = 1800000,
			//.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_lvs2_consumers),
		.consumer_supplies = hi6551_lvs2_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LVS03] = {
		.constraints = {
			.name = "HI6551_LVS3",
			.min_uV = 1800000,
			.max_uV = 1800000,
			//.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_lvs3_consumers),
		.consumer_supplies = hi6551_lvs3_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LVS04] = {
		.constraints = {
			.name = "HI6551_LVS4",
			.min_uV = 1800000,
			.max_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_lvs4_consumers),
		.consumer_supplies = hi6551_lvs4_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LVS05] = {
		.constraints = {
			.name = "HI6551_LVS5",
			.min_uV = 1800000,
			.max_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_lvs5_consumers),
		.consumer_supplies = hi6551_lvs5_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LVS06] = {
		.constraints = {
			.name = "HI6551_LVS6",
			.min_uV = 1800000,
			.max_uV = 1800000,
			//.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_lvs6_consumers),
		.consumer_supplies = hi6551_lvs6_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LVS07] = {
		.constraints = {
			.name = "HI6551_LVS7",
			.min_uV = 1800000,
			.max_uV = 1800000,
			//.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_lvs7_consumers),
		.consumer_supplies = hi6551_lvs7_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LVS08] = {
		.constraints = {
			.name = "HI6551_LVS8",
			.min_uV = 1800000,
			.max_uV = 1800000,
			//.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_lvs8_consumers),
		.consumer_supplies = hi6551_lvs8_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LVS09] = {
		.constraints = {
			.name = "HI6551_LVS9",
			.min_uV = 1800000,
			.max_uV = 1800000,
			//.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_lvs9_consumers),
		.consumer_supplies = hi6551_lvs9_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
	[PMIC_HI6551_LVS10] = {
		.constraints = {
			.name = "HI6551_LVS10",
			.min_uV = 1800000,
			.max_uV = 1800000,
			//.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_lvs10_consumers),
		.consumer_supplies = hi6551_lvs10_consumers,
		//.supply_regulator = "HI6551_BUCK6",
	},
};


struct regulator_desc hi6551_dr_regulators_desc[] = {
	[1] = {
		.name = "HI6551_DR1",
		.id = HI6551_DR1 | HI6551_DR_FLAG,
		.n_voltages = 8,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	[2] = {
		.name = "HI6551_DR2",
		.id = HI6551_DR2 | HI6551_DR_FLAG,
		.n_voltages = 8,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	[3] = {
		.name = "HI6551_DR3",
		.id = HI6551_DR3 | HI6551_DR_FLAG,
		.n_voltages = 8,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	[4] = {
		.name = "HI6551_DR4",
		.id = HI6551_DR4 | HI6551_DR_FLAG,
		.n_voltages = 8,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	[5] = {
		.name = "HI6551_DR5",
		.id = HI6551_DR5 | HI6551_DR_FLAG,
		.n_voltages = 8,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
};

struct regulator_consumer_supply hi6551_dr1_consumers[] = {
	REGULATOR_SUPPLY(H6551_DR1_NM, NULL),
};
struct regulator_consumer_supply hi6551_dr2_consumers[] = {
	REGULATOR_SUPPLY(H6551_DR2_NM, NULL),
};
struct regulator_consumer_supply hi6551_dr3_consumers[] = {
	REGULATOR_SUPPLY(H6551_DR3_NM, NULL),
};
struct regulator_consumer_supply hi6551_dr4_consumers[] = {
	REGULATOR_SUPPLY(H6551_DR4_NM, NULL),
};
struct regulator_consumer_supply hi6551_dr5_consumers[] = {
	REGULATOR_SUPPLY(H6551_DR5_NM, NULL),
};
struct regulator_init_data hi6551_dr_regulators_init[] = {
	[PMIC_HI6551_DR01] = {
		.constraints = {
			.name = "HI6551_DR1",
            .min_uV = 3000,
            .max_uV = 24000,
			.min_uA = 3000,
			.max_uA = 24000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_dr1_consumers),
		.consumer_supplies = hi6551_dr1_consumers,
	},
	[PMIC_HI6551_DR02] = {
		.constraints = {
			.name = "HI6551_DR2",
            .min_uV = 3000,
            .max_uV = 24000,
			.min_uA = 3000,
			.max_uA = 24000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_dr2_consumers),
		.consumer_supplies = hi6551_dr2_consumers,
	},
	[PMIC_HI6551_DR03] = {
		.constraints = {
			.name = "HI6551_DR3",
            .min_uV = 1000,
            .max_uV = 45000,
			.min_uA = 1000,
			.max_uA = 45000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_dr3_consumers),
		.consumer_supplies = hi6551_dr3_consumers,
	},
	[PMIC_HI6551_DR04] = {
		.constraints = {
			.name = "HI6551_DR4",
            .min_uV = 1000,
			.max_uV = 45000,
			.min_uA = 1000,
			.max_uA = 45000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_dr4_consumers),
		.consumer_supplies = hi6551_dr4_consumers,
	},
	[PMIC_HI6551_DR05] = {
		.constraints = {
			.name = "HI6551_DR5",
            .min_uV = 1000,
			.max_uV = 45000,
			.min_uA = 1000,
			.max_uA = 45000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6551_dr5_consumers),
		.consumer_supplies = hi6551_dr5_consumers,
	},
};
#endif
/*********************************HI6551 REGULATOR END************************/
#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

