#include <osl_bio.h>
#include <bsp_memmap.h>
#include <bsp_shared_ddr.h>
#include <pmu.h>
#include <hi_bbstar.h>

/*每路电压源的档位的最大值,从0开始,不使用档位数量是为了节省num属性的空间*/
#define NUMS_OF_HI6559_BUCK0_VOLT 63
#define NUMS_OF_HI6559_BUCK3_VOLT 63
#define NUMS_OF_HI6559_BUCK4_VOLT 15
#define NUMS_OF_HI6559_BUCK5_VOLT 15
#define NUMS_OF_HI6559_BUCK6_VOLT 15
#define NUMS_OF_HI6559_LDO01_VOLT 7
#define NUMS_OF_HI6559_LDO03_VOLT 7
#define NUMS_OF_HI6559_LDO06_VOLT 5
#define NUMS_OF_HI6559_LDO07_VOLT 7
#define NUMS_OF_HI6559_LDO08_VOLT 7
#define NUMS_OF_HI6559_LDO09_VOLT 7
#define NUMS_OF_HI6559_LDO10_VOLT 7
#define NUMS_OF_HI6559_LDO11_VOLT 7
#define NUMS_OF_HI6559_LDO12_VOLT 7
#define NUMS_OF_HI6559_LDO13_VOLT 7
#define NUMS_OF_HI6559_LDO14_VOLT 7
#define NUMS_OF_HI6559_LDO22_VOLT 5
#define NUMS_OF_HI6559_LDO23_VOLT 7
#define NUMS_OF_HI6559_LDO24_VOLT 7

/*电压源电压档位表，单位：uv*/
#define HI6559_NULL_VSET_LIST {0}
#define HI6559_BUCK5_VSET_LIST {1500000,1600000,1700000,1800000,1850000,1900000,2000000,2100000,\
2200000,2250000,2300000,2350000,2500000,2800000,2850000,3300000}
#define HI6559_LDO01_VSET_LIST {1800000,2500000,2600000,2700000,2800000,2850000,2900000,3000000}
#define HI6559_LDO03_VSET_LIST { 825000, 850000, 875000, 900000, 925000, 950000, 975000,1000000}
#define HI6559_LDO06_VSET_LIST {1500000,1800000,1850000,2200000,2500000,2800000}
#define HI6559_LDO07_VSET_LIST {1800000,1850000,2800000,2850000,2900000,2950000,3000000,3300000}
#define HI6559_LDO08_VSET_LIST {1100000,1200000,1250000,1275000,1300000,1325000,1350000,1375000}
#define HI6559_LDO09_VSET_LIST {1800000,1850000,2800000,2850000,2900000,2950000,3000000,3050000}
#define HI6559_LDO10_VSET_LIST {2800000,2850000,2900000,2950000,3000000,3050000,3100000,3300000}
#define HI6559_LDO11_VSET_LIST {1800000,1850000,2200000,2800000,2850000,2900000,3000000,3300000}
#define HI6559_LDO12_VSET_LIST {3000000,3050000,3100000,3150000,3200000,3250000,3300000,3350000}
#define HI6559_LDO13_VSET_LIST {1800000,2500000,2600000,2700000,2800000,2850000,2900000,3000000}
#define HI6559_LDO14_VSET_LIST {1800000,2200000,2600000,2700000,2800000,2850000,2900000,3000000}
#define HI6559_LDO22_VSET_LIST {1200000,1225000,1250000,1300000,1425000,1500000}
#define HI6559_LDO23_VSET_LIST {1500000,1800000,1850000,1900000,1950000,2500000,2800000,2850000}
#define HI6559_LDO24_VSET_LIST {2800000,2850000,2900000,2950000,3000000,3050000,3100000,3150000}

/*定义一个PMU_VLTGS类型的全局数组，存储所有电压源的属性。*/
PMIC_HI6559_VLTGS_TABLE hi6559_volt_table_init =
{
    .magic_start = SHM_PMU_VOLTTABLE_MAGIC_START_DATA,
    .hi6559_volt_attr ={
        [PMIC_HI6559_BUCK0] = {
            .enable_reg_addr       = HI6559_ENABLE1_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE1_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS1_OFFSET,
            .voltage_reg_addr      = HI6559_VSET_BUCK0_ADJ_OFFSET,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 2,
            .disable_bit_offset    = 2,
            .is_enabled_bit_offset = 2,
            .voltage_bit_mask      = 0xff,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_BUCK0_VOLT,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_NULL_VSET_LIST,
        },
        [PMIC_HI6559_BUCK3] = {
            .enable_reg_addr       = HI6559_ENABLE1_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE1_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS1_OFFSET,
            .voltage_reg_addr      = HI6559_VSET_BUCK3_ADJ_OFFSET,
            .eco_follow_reg_addr   = HI6559_ECO_MOD_CFG1_OFFSET,
            .eco_force_reg_addr    = HI6559_ECO_MOD_CFG3_OFFSET,
            .enable_bit_offset     = 3,
            .disable_bit_offset    = 3,
            .is_enabled_bit_offset = 3,
            .voltage_bit_mask      = 0x3f,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_BUCK3_VOLT,
            .eco_follow_bit_offset = 0,
            .eco_force_bit_offset  = 0,
            .voltage_list          = HI6559_NULL_VSET_LIST,
        },
        [PMIC_HI6559_BUCK4] = {
            .enable_reg_addr       = HI6559_ENABLE1_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE1_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS1_OFFSET,
            .voltage_reg_addr      = HI6559_VSET_BUCK4_ADJ_OFFSET,
            .eco_follow_reg_addr   = HI6559_ECO_MOD_CFG1_OFFSET,
            .eco_force_reg_addr    = HI6559_ECO_MOD_CFG3_OFFSET,
            .enable_bit_offset     = 4,
            .disable_bit_offset    = 4,
            .is_enabled_bit_offset = 4,
            .voltage_bit_mask      = 0x0f,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_BUCK4_VOLT,
            .eco_follow_bit_offset = 1,
            .eco_force_bit_offset  = 1,
            .voltage_list          = HI6559_NULL_VSET_LIST,
        },
        [PMIC_HI6559_BUCK5] = {
            .enable_reg_addr       = HI6559_ENABLE1_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE1_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS1_OFFSET,
            .voltage_reg_addr      = HI6559_VSET_BUCK5_ADJ_OFFSET,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 5,
            .disable_bit_offset    = 5,
            .is_enabled_bit_offset = 5,
            .voltage_bit_mask      = 0xff,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_BUCK5_VOLT,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_BUCK5_VSET_LIST,
        },
        [PMIC_HI6559_BUCK6] = {
            .enable_reg_addr       = HI6559_ENABLE1_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE1_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS1_OFFSET,
            .voltage_reg_addr      = HI6559_VSET_BUCK6_ADJ_OFFSET,
            .eco_follow_reg_addr   = HI6559_ECO_MOD_CFG1_OFFSET,
            .eco_force_reg_addr    = HI6559_ECO_MOD_CFG3_OFFSET,
            .enable_bit_offset     = 6,
            .disable_bit_offset    = 6,
            .is_enabled_bit_offset = 6,
            .voltage_bit_mask      = 0x0f,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_BUCK6_VOLT,
            .eco_follow_bit_offset = 3,
            .eco_force_bit_offset  = 3,
            .voltage_list          = HI6559_NULL_VSET_LIST,
        },
        [PMIC_HI6559_LDO01] = {
            .enable_reg_addr       = HI6559_ENABLE2_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE2_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS2_OFFSET,
            .voltage_reg_addr      = HI6559_LDO1_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 0,
            .disable_bit_offset    = 0,
            .is_enabled_bit_offset = 0,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO01_VOLT,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_LDO01_VSET_LIST,
        },
        [PMIC_HI6559_LDO03] = {
            .enable_reg_addr       = HI6559_ENABLE2_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE2_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS2_OFFSET,
            .voltage_reg_addr      = HI6559_LDO3_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = HI6559_ECO_MOD_CFG2_OFFSET,
            .eco_force_reg_addr    = HI6559_ECO_MOD_CFG4_OFFSET,
            .enable_bit_offset     = 2,
            .disable_bit_offset    = 2,
            .is_enabled_bit_offset = 2,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO03_VOLT,
            .eco_follow_bit_offset = 0,
            .eco_force_bit_offset  = 0,
            .voltage_list          = HI6559_LDO03_VSET_LIST,
        },        
        [PMIC_HI6559_LDO06] = {
            .enable_reg_addr       = HI6559_ENABLE2_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE2_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS2_OFFSET,
            .voltage_reg_addr      = HI6559_LDO6_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 5,
            .disable_bit_offset    = 5,
            .is_enabled_bit_offset = 5,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO06_VOLT,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_LDO06_VSET_LIST,
        },
        [PMIC_HI6559_LDO07] = {
            .enable_reg_addr       = HI6559_ENABLE2_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE2_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS2_OFFSET,
            .voltage_reg_addr      = HI6559_LDO7_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = HI6559_ECO_MOD_CFG2_OFFSET,
            .eco_force_reg_addr    = HI6559_ECO_MOD_CFG4_OFFSET,
            .enable_bit_offset     = 6,
            .disable_bit_offset    = 6,
            .is_enabled_bit_offset = 6,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO07_VOLT,
            .eco_follow_bit_offset = 1,
            .eco_force_bit_offset  = 1,
            .voltage_list          = HI6559_LDO07_VSET_LIST,
        },
        [PMIC_HI6559_LDO08] = {
            .enable_reg_addr       = HI6559_ENABLE2_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE2_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS2_OFFSET,
            .voltage_reg_addr      = HI6559_LDO8_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 7,
            .disable_bit_offset    = 7,
            .is_enabled_bit_offset = 7,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO08_VOLT,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_LDO08_VSET_LIST,
        },
        [PMIC_HI6559_LDO09] = {
            .enable_reg_addr       = HI6559_SIM_LDO9_LDO11_EN_OFFSET,
            .disable_reg_addr      = HI6559_SIM_LDO9_LDO11_EN_OFFSET,
            .is_enabled_reg_addr   = HI6559_SIM_LDO9_LDO11_EN_OFFSET,
            .voltage_reg_addr      = HI6559_LDO9_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = HI6559_ECO_MOD_CFG2_OFFSET,
            .eco_force_reg_addr    = HI6559_ECO_MOD_CFG4_OFFSET,
            .enable_bit_offset     = 0,
            .disable_bit_offset    = 0,
            .is_enabled_bit_offset = 0,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO09_VOLT,
            .eco_follow_bit_offset = 2,
            .eco_force_bit_offset  = 2,
            .voltage_list          = HI6559_LDO09_VSET_LIST,
        },
        [PMIC_HI6559_LDO10] = {
            .enable_reg_addr       = HI6559_ENABLE3_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE3_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS3_OFFSET,
            .voltage_reg_addr      = HI6559_LDO10_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = HI6559_ECO_MOD_CFG2_OFFSET,
            .eco_force_reg_addr    = HI6559_ECO_MOD_CFG4_OFFSET,
            .enable_bit_offset     = 0,
            .disable_bit_offset    = 0,
            .is_enabled_bit_offset = 0,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO10_VOLT,
            .eco_follow_bit_offset = 3,
            .eco_force_bit_offset  = 3,
            .voltage_list          = HI6559_LDO10_VSET_LIST,
        },
        [PMIC_HI6559_LDO11] = {
            .enable_reg_addr       = HI6559_SIM_LDO9_LDO11_EN_OFFSET,
            .disable_reg_addr      = HI6559_SIM_LDO9_LDO11_EN_OFFSET,
            .is_enabled_reg_addr   = HI6559_SIM_LDO9_LDO11_EN_OFFSET,
            .voltage_reg_addr      = HI6559_LDO11_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 2,
            .disable_bit_offset    = 2,
            .is_enabled_bit_offset = 2,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO11_VOLT,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_LDO11_VSET_LIST,
        },
        [PMIC_HI6559_LDO12] = {
            .enable_reg_addr       = HI6559_ENABLE3_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE3_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS3_OFFSET,
            .voltage_reg_addr      = HI6559_LDO12_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = HI6559_ECO_MOD_CFG2_OFFSET,
            .eco_force_reg_addr    = HI6559_ECO_MOD_CFG4_OFFSET,
            .enable_bit_offset     = 1,
            .disable_bit_offset    = 1,
            .is_enabled_bit_offset = 1,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO12_VOLT,
            .eco_follow_bit_offset = 5,
            .eco_force_bit_offset  = 5,
            .voltage_list          = HI6559_LDO12_VSET_LIST,
        },
        [PMIC_HI6559_LDO13] = {
            .enable_reg_addr       = HI6559_ENABLE3_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE3_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS3_OFFSET,
            .voltage_reg_addr      = HI6559_LDO13_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 2,
            .disable_bit_offset    = 2,
            .is_enabled_bit_offset = 2,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO13_VOLT,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_LDO13_VSET_LIST,
        },
        [PMIC_HI6559_LDO14] = {
            .enable_reg_addr       = HI6559_ENABLE3_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE3_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS3_OFFSET,
            .voltage_reg_addr      = HI6559_LDO14_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 3,
            .disable_bit_offset    = 3,
            .is_enabled_bit_offset = 3,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO14_VOLT,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_LDO14_VSET_LIST,
        },
        [PMIC_HI6559_LDO22] = {
            .enable_reg_addr       = HI6559_ENABLE4_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE4_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS4_OFFSET,
            .voltage_reg_addr      = HI6559_LDO22_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = HI6559_ECO_MOD_CFG2_OFFSET,
            .eco_force_reg_addr    = HI6559_ECO_MOD_CFG4_OFFSET,
            .enable_bit_offset     = 3,
            .disable_bit_offset    = 3,
            .is_enabled_bit_offset = 3,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO22_VOLT,
            .eco_follow_bit_offset = 6,
            .eco_force_bit_offset  = 6,
            .voltage_list          = HI6559_LDO22_VSET_LIST,
        },
        [PMIC_HI6559_LDO23] = {
            .enable_reg_addr       = HI6559_ENABLE4_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE4_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS4_OFFSET,
            .voltage_reg_addr      = HI6559_LDO23_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 4,
            .disable_bit_offset    = 4,
            .is_enabled_bit_offset = 4,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO23_VOLT,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_LDO23_VSET_LIST,
        },
        [PMIC_HI6559_LDO24] = {
            .enable_reg_addr       = HI6559_ENABLE4_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE4_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS4_OFFSET,
            .voltage_reg_addr      = HI6559_LDO24_REG_ADJ_OFFSET,
            .eco_follow_reg_addr   = HI6559_ECO_MOD_CFG2_OFFSET,
            .eco_force_reg_addr    = HI6559_ECO_MOD_CFG4_OFFSET,
            .enable_bit_offset     = 5,
            .disable_bit_offset    = 5,
            .is_enabled_bit_offset = 5,
            .voltage_bit_mask      = 0x07,
            .voltage_bit_offset    = 0,
            .voltage_nums          = NUMS_OF_HI6559_LDO24_VOLT,
            .eco_follow_bit_offset = 7,
            .eco_force_bit_offset  = 7,
            .voltage_list          = HI6559_LDO24_VSET_LIST,
        },
        [PMIC_HI6559_LVS07] = {
            .enable_reg_addr       = HI6559_ENABLE5_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE5_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS5_OFFSET,
            .voltage_reg_addr      = PMU_INVAILD_ADDR,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 5,
            .disable_bit_offset    = 5,
            .is_enabled_bit_offset = 5,
            .voltage_bit_mask      = PMU_INVAILD_DATA,
            .voltage_bit_offset    = PMU_INVAILD_OFFSET,
            .voltage_nums          = PMU_INVAILD_DATA,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_NULL_VSET_LIST,
        },
        [PMIC_HI6559_LVS09] = {
            .enable_reg_addr       = HI6559_ENABLE5_OFFSET,
            .disable_reg_addr      = HI6559_DISABLE5_OFFSET,
            .is_enabled_reg_addr   = HI6559_ONOFF_STATUS5_OFFSET,
            .voltage_reg_addr      = PMU_INVAILD_ADDR,
            .eco_follow_reg_addr   = PMU_INVAILD_ADDR,
            .eco_force_reg_addr    = PMU_INVAILD_ADDR,
            .enable_bit_offset     = 7,
            .disable_bit_offset    = 7,
            .is_enabled_bit_offset = 7,
            .voltage_bit_mask      = PMU_INVAILD_DATA,
            .voltage_bit_offset    = PMU_INVAILD_OFFSET,
            .voltage_nums          = PMU_INVAILD_DATA,
            .eco_follow_bit_offset = PMU_INVAILD_OFFSET,
            .eco_force_bit_offset  = PMU_INVAILD_OFFSET,
            .voltage_list          = HI6559_NULL_VSET_LIST,
        },
    },
    .magic_end = SHM_PMU_VOLTTABLE_MAGIC_END_DATA,
};


/*****************************************************************************
 函 数 名  : hi6559_volttable_copytoddr
 功能描述  : 将hi6559电压控制表拷贝到DDR中
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : pmu初始化模块
*****************************************************************************/
void hi6559_volttable_copytoddr(void)
{
    void *hi6559_volttable = (void *)SHM_PMU_VOLTTABLE_ADDR;/*pmu控制各路电源的reg列表*/
    s32 table_max_size = SHM_PMU_VOLTTABLE_SIZE;
    /*表前和表后加入魔幻数*/

    if(SHM_PMU_VOLTTABLE_SIZE < sizeof(hi6559_volt_table_init))
    {
        pmic_print_error("hi6559_volt_table_init too big ,max size is %x",table_max_size);
        return;
    }
    memcpy(hi6559_volttable,&hi6559_volt_table_init,sizeof(hi6559_volt_table_init));
}

