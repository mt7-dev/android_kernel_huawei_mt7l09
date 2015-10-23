
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

#ifdef CONFIG_PMIC_HI6559
struct regulator_desc hi6559_regulators_desc[] = {
	[PMIC_HI6559_BUCK0] = {
		.name = "PMIC_HI6559_BUCK0",
		.id = HI6559_BUCK0 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_BUCK3] = {
		.name = "PMIC_HI6559_BUCK3",
		.id = HI6559_BUCK3 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_BUCK4] = {
		.name = "PMIC_HI6559_BUCK4",
		.id = HI6559_BUCK4 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_BUCK5] = {
		.name = "PMIC_HI6559_BUCK5",
		.id = HI6559_BUCK5 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_BUCK6] = {
		.name = "PMIC_HI6559_BUCK6",
		.id = HI6559_BUCK6 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO01] = {
		.name = "PMIC_HI6559_LDO01",
		.id = HI6559_LDO01	 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO03] = {
		.name = "PMIC_HI6559_LDO03",
		.id = HI6559_LDO03 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO06] = {
		.name = "PMIC_HI6559_LDO06",
		.id = HI6559_LDO06 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO07] = {
		.name = "PMIC_HI6559_LDO07",
		.id = HI6559_LDO07 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO08] = {
		.name = "PMIC_HI6559_LDO08",
		.id = HI6559_LDO08 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO09] = {
		.name = "PMIC_HI6559_LDO09",
		.id = HI6559_LDO09 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO10] = {
		.name = "PMIC_HI6559_LDO10",
		.id = HI6559_LDO10 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO11] = {
		.name = "PMIC_HI6559_LDO11",
		.id = HI6559_LDO11 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO12] = {
		.name = "PMIC_HI6559_LDO12",
		.id = HI6559_LDO12 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO13] = {
		.name = "PMIC_HI6559_LDO13",
		.id = HI6559_LDO13 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO14] = {
		.name = "PMIC_HI6559_LDO14",
		.id = HI6559_LDO14 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO22] = {
		.name = "PMIC_HI6559_LDO22",
		.id = HI6559_LDO22 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO23] = {
		.name = "PMIC_HI6559_LDO23",
		.id = HI6559_LDO23 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LDO24] = {
		.name = "PMIC_HI6559_LDO24",
		.id = HI6559_LDO24 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LVS07] = {
		.name = "PMIC_HI6559_LVS07",
		.id = HI6559_LVS07 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_LVS09] = {
		.name = "PMIC_HI6559_LVS09",
		.id = HI6559_LVS09 | HI6559_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
};


/*jingyong*/
static struct regulator_consumer_supply hi6559_ldo7_consumers[] = {
	REGULATOR_SUPPLY("SD_IO-vcc", NULL),
};

/*jingyong*/
static struct regulator_consumer_supply hi6559_ldo10_consumers[] = {
	REGULATOR_SUPPLY("SD_POWER-vcc", NULL),
};

/*liujia*/
static struct regulator_consumer_supply hi6559_ldo14_consumers[] = {
	REGULATOR_SUPPLY("LCD-vcc", NULL),
};

/*chengdongyue*/
static struct regulator_consumer_supply hi6559_ldo22_consumers[] = {
	REGULATOR_SUPPLY("hsic_phy_avdd-vcc", NULL),
};
/*liuwenhui*/
static struct regulator_consumer_supply hi6559_ldo23_consumers[] = {
	REGULATOR_SUPPLY("EFUSE-vcc", NULL),
};
static struct regulator_consumer_supply hi_buck0_consumers[] = {
	REGULATOR_SUPPLY("HI_BUCK0-vcc", NULL),
};
static struct regulator_consumer_supply hi_buck3_consumers[] = {
	REGULATOR_SUPPLY("HI_BUCK3-vcc", NULL),
};
static struct regulator_consumer_supply hi_buck4_consumers[] = {
	REGULATOR_SUPPLY("HI_BUCK4-vcc", NULL),
};
static struct regulator_consumer_supply hi_buck5_consumers[] = {
	REGULATOR_SUPPLY("HI_BUCK5-vcc", NULL),
};
static struct regulator_consumer_supply hi_buck6_consumers[] = {
	REGULATOR_SUPPLY("HI_BUCK6-vcc", NULL),
};
static struct regulator_consumer_supply hi_ldo1_consumers[] = {
	REGULATOR_SUPPLY("HI_LDO1-vcc", NULL),
};
static struct regulator_consumer_supply hi_ldo3_consumers[] = {
	REGULATOR_SUPPLY("HI_LDO3-vcc", NULL),
};
static struct regulator_consumer_supply hi_ldo6_consumers[] = {
	REGULATOR_SUPPLY("HI_LDO6-vcc", NULL),
};
static struct regulator_consumer_supply hi_ldo8_consumers[] = {
	REGULATOR_SUPPLY("HI_LDO8-vcc", NULL),
};
static struct regulator_consumer_supply hi_ldo9_consumers[] = {
	REGULATOR_SUPPLY("HI_LDO9-vcc", NULL),
};
static struct regulator_consumer_supply hi_ldo11_consumers[] = {
	REGULATOR_SUPPLY("HI_LDO11-vcc", NULL),
};
static struct regulator_consumer_supply hi_ldo12_consumers[] = {
	REGULATOR_SUPPLY("HI_LDO12-vcc", NULL),
};
static struct regulator_consumer_supply hi_ldo13_consumers[] = {
	REGULATOR_SUPPLY("HI_LDO13-vcc", NULL),
};
static struct regulator_consumer_supply hi_ldo24_consumers[] = {
	REGULATOR_SUPPLY("HI_LDO24-vcc", NULL),
};
static struct regulator_consumer_supply hi_lvs7_consumers[] = {
	REGULATOR_SUPPLY("HI_lvs7-vcc", NULL),
};
static struct regulator_consumer_supply hi_lvs9_consumers[] = {
	REGULATOR_SUPPLY("HI_lvs9-vcc", NULL),
};


struct regulator_init_data hi6559_regulators_init[] = {
	[PMIC_HI6559_BUCK0] = {
		.constraints = {
			.name = "PMIC_HI6559_BUCK0",
			.min_uV = 900000,
			.max_uV = 4050000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_buck0_consumers),
		.consumer_supplies = hi_buck0_consumers,
	},
	[PMIC_HI6559_BUCK3] = {
		.constraints = {
			.name = "PMIC_HI6559_BUCK3",
			.min_uV = 700000,
			.max_uV = 1204000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_buck3_consumers),
		.consumer_supplies = hi_buck3_consumers,
	},
	[PMIC_HI6559_BUCK4] = {
		.constraints = {
			.name = "PMIC_HI6559_BUCK4",
			.min_uV = 1200000,
			.max_uV = 1575000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_buck4_consumers),
		.consumer_supplies = hi_buck4_consumers,
	},
	[PMIC_HI6559_BUCK5] = {
		.constraints = {
			.name = "PMIC_HI6559_BUCK5",
			.min_uV = 1500000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_buck5_consumers),
		.consumer_supplies = hi_buck5_consumers,
	},
	[PMIC_HI6559_BUCK6] = {
		.constraints = {
			.name = "PMIC_HI6559_BUCK6",
			.min_uV = 1800000,
			.max_uV = 2175000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_buck6_consumers),
		.consumer_supplies = hi_buck6_consumers,
	},
	[PMIC_HI6559_LDO01] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO01",
			.min_uV = 1800000,
			.max_uV = 3000000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_ldo1_consumers),
		.consumer_supplies = hi_ldo1_consumers,
	},
	[PMIC_HI6559_LDO03] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO03",
			.min_uV = 825000,
			.max_uV = 1000000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_ldo3_consumers),
		.consumer_supplies = hi_ldo3_consumers,
	},
	[PMIC_HI6559_LDO06] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO06",
			.min_uV = 1500000,
			.max_uV = 2800000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_ldo6_consumers),
		.consumer_supplies = hi_ldo6_consumers,
	},
	[PMIC_HI6559_LDO07] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO07",
			.min_uV = 1800000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_ldo7_consumers),
		.consumer_supplies = hi6559_ldo7_consumers,
	},
	[PMIC_HI6559_LDO08] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO08",
			.min_uV = 1100000,
			.max_uV = 1375000,
			/*always on software not control*/
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_ldo8_consumers),
		.consumer_supplies = hi_ldo8_consumers,
	},
	[PMIC_HI6559_LDO09] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO09",
			.min_uV = 1800000,
			.max_uV = 3050000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_ldo9_consumers),
		.consumer_supplies = hi_ldo9_consumers,
	},
	[PMIC_HI6559_LDO10] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO10",
			.min_uV = 2800000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_ldo10_consumers),
		.consumer_supplies = hi6559_ldo10_consumers,
	},
	[PMIC_HI6559_LDO11] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO11",
			.min_uV = 1800000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_ldo11_consumers),
		.consumer_supplies = hi_ldo11_consumers,
	},
	[PMIC_HI6559_LDO12] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO12",
			.min_uV = 3000000,
			.max_uV = 3350000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_ldo12_consumers),
		.consumer_supplies = hi_ldo12_consumers,
	},
	[PMIC_HI6559_LDO13] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO13",
			.min_uV = 1800000,
			.max_uV = 3000000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_ldo13_consumers),
		.consumer_supplies = hi_ldo13_consumers,
	},
	[PMIC_HI6559_LDO14] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO14",
			.min_uV = 1800000,
			.max_uV = 3000000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_ldo14_consumers),
		.consumer_supplies = hi6559_ldo14_consumers,
	},
	[PMIC_HI6559_LDO22] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO22",
			.min_uV = 1200000,
			.max_uV = 1500000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_ldo22_consumers),
		.consumer_supplies = hi6559_ldo22_consumers,
	},
	[PMIC_HI6559_LDO23] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO23",
			.min_uV = 1500000,
			.max_uV = 2850000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_ldo23_consumers),
		.consumer_supplies = hi6559_ldo23_consumers,
	},
	[PMIC_HI6559_LDO24] = {
		.constraints = {
			.name = "PMIC_HI6559_LDO24",
			.min_uV = 2800000,
			.max_uV = 3150000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_ldo24_consumers),
		.consumer_supplies = hi_ldo24_consumers,
	},
	[PMIC_HI6559_LVS07] = {
		.constraints = {
			.name = "PMIC_HI6559_LVS07",
			.min_uV = 1800000,
			.max_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_lvs7_consumers),
		.consumer_supplies = hi_lvs7_consumers,
	},
	[PMIC_HI6559_LVS09] = {
		.constraints = {
			.name = "PMIC_HI6559_LVS09",
			.min_uV = 1800000,
			.max_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi_lvs9_consumers),
		.consumer_supplies = hi_lvs9_consumers,
	},
};

struct regulator_desc hi6559_dr_regulators_desc[] = {
	[PMIC_HI6559_DR01] = {
		.name = "PMIC_HI6559_DR01",
		.id = HI6559_DR1 | HI6559_DR_FLAG,
		.n_voltages = 8,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_DR02] = {
		.name = "PMIC_HI6559_DR02",
		.id = HI6559_DR2 | HI6559_DR_FLAG,
		.n_voltages = 8,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_DR03] = {
		.name = "PMIC_HI6559_DR03",
		.id = HI6559_DR3 | HI6559_DR_FLAG,
		.n_voltages = 8,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_DR04] = {
		.name = "PMIC_HI6559_DR04",
		.id = HI6559_DR4 | HI6559_DR_FLAG,
		.n_voltages = 8,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	[PMIC_HI6559_DR05] = {
		.name = "PMIC_HI6559_DR05",
		.id = HI6559_DR5 | HI6559_DR_FLAG,
		.n_voltages = 8,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
	/*用于适配6559中没有，6551中使用的组件*/
	[PMIC_HI6559_DR05 + 1] = {
		.name = "common_for_useless",
		.id = (HI6559_OBSOLETE) | HI6559_OBSOLETE_FLAG,
		.ops = &balong_regulator_ldo_ops,
		.type = REGULATOR_CURRENT,
		.owner = THIS_MODULE,
	},
};
/*设备表复用6551*/
struct regulator_consumer_supply hi6559_dr1_consumers[] = {
	REGULATOR_SUPPLY(H6551_DR1_NM, NULL),
};
struct regulator_consumer_supply hi6559_dr2_consumers[] = {
	REGULATOR_SUPPLY(H6551_DR2_NM, NULL),
};
struct regulator_consumer_supply hi6559_dr3_consumers[] = {
	REGULATOR_SUPPLY(H6551_DR3_NM, NULL),
};
struct regulator_consumer_supply hi6559_dr4_consumers[] = {
	REGULATOR_SUPPLY(H6551_DR4_NM, NULL),
};
struct regulator_consumer_supply hi6559_dr5_consumers[] = {
	REGULATOR_SUPPLY(H6551_DR5_NM, NULL),
};
struct regulator_consumer_supply hi6559_useless_consumers[] = {
	REGULATOR_SUPPLY(HI6551_SD_DR, NULL),
	REGULATOR_SUPPLY(HI6551_LCD_DR, NULL),
	REGULATOR_SUPPLY("SD_IO_M-vcc", NULL),
};

struct regulator_init_data hi6559_dr_regulators_init[] = {
	[PMIC_HI6559_DR01] = {
		.constraints = {
			.name = "PMIC_HI6559_DR01",
            .min_uV = 3000,
            .max_uV = 24000,
			.min_uA = 3000,
			.max_uA = 24000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_dr1_consumers),
		.consumer_supplies = hi6559_dr1_consumers,
	},
	[PMIC_HI6559_DR02] = {
		.constraints = {
			.name = "PMIC_HI6559_DR02",
            .min_uV = 3000,
            .max_uV = 24000,
			.min_uA = 3000,
			.max_uA = 24000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_dr2_consumers),
		.consumer_supplies = hi6559_dr2_consumers,
	},
	[PMIC_HI6559_DR03] = {
		.constraints = {
			.name = "PMIC_HI6559_DR03",
            .min_uV = 1000,
            .max_uV = 45000,
			.min_uA = 1000,
			.max_uA = 45000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_dr3_consumers),
		.consumer_supplies = hi6559_dr3_consumers,
	},
	[PMIC_HI6559_DR04] = {
		.constraints = {
			.name = "PMIC_HI6559_DR04",
            .min_uV = 1000,
			.max_uV = 45000,
			.min_uA = 1000,
			.max_uA = 45000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_dr4_consumers),
		.consumer_supplies = hi6559_dr4_consumers,
	},
	[PMIC_HI6559_DR05] = {
		.constraints = {
			.name = "PMIC_HI6559_DR05",
            .min_uV = 1000,
			.max_uV = 45000,
			.min_uA = 1000,
			.max_uA = 45000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_dr5_consumers),
		.consumer_supplies = hi6559_dr5_consumers,
	},
	/*多定义一个，用于兼容其他pmu中使用，而6559中没有用的组件*/
	[PMIC_HI6559_DR05 + 1] = {
		.constraints = {
			.name = "common_for_useless",
            .min_uV = 1000,
			.max_uV = 9000000,
			.min_uA = 1000,
			.max_uA = 45000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
			.always_on = 0,
		},
		.num_consumer_supplies = ARRAY_SIZE(hi6559_useless_consumers),
		.consumer_supplies = hi6559_useless_consumers,
	},
};
#endif
#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */


