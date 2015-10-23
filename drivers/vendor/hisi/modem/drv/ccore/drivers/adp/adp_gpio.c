/*lint -save -e537*/
#include <product_config.h>
#include "osl_common.h"
#include "hi_gpio.h"
#include "drv_gpio.h"
#include "drv_nv_id.h"
#include "drv_nv_def.h"
#include "pintrl_balong.h"
#include "gpio_balong.h"
#include "bsp_om.h"
#include "bsp_nvim.h"


/*lint -restore */

#define  gpio_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_GPIO, "[gpio]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  gpio_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_GPIO, "[gpio]: "fmt, ##__VA_ARGS__))

#ifndef HI_K3_GPIO
int gpio_oprt_write_test(void);
int gpio_oprt_read_test(void);

/*天线开关不下电特性开关，只在k3中使用，v7R2不涉及*/
unsigned int  sw_unpd_en = 0;

/*****************************************************************************
 函 数 名  : drv_gpio_oprt
 功能描述  : at^GPIOPL,设置和查询GPIO的电平
 输入参数  : 操作类型 ulOp:
             0  设置各GPIO的PL值
             1  查询当前各GPIO的PL值

             pucPL 对应20Byte的数组,每个BYTE代表一个8个管脚的值

             设置操作时,pucPL为设置的20的Byte
             查询操作时,pucPL为当前获取到的PL的实际值组成的16进制数据
             例如用户设置at^GPIOPL = ABCDEF,则对应pucPL的数组值为{A,B,C,D,E,F,0,0,...}

 输出参数  : 无
 返 回 值  :  0 ：OK  非 0 ：Error

*****************************************************************************/

unsigned long drv_gpio_oprt(unsigned long ulOp, unsigned char *pucPL)
{
    unsigned int i = 0;
    unsigned int j = 0;

    int value = 0;
    unsigned int bype_pin = 8;

    if(NULL == pucPL)
    {
        return GPIO_OPRT_ERROR;
    }
    
    if(GPIO_OPRT_SET == ulOp)
    {
        for(i = 0;i < GPIO_MAX_BANK_NUM * GPIO_MAX_PINS / bype_pin;i++)
        {
            for(j = 0;j < bype_pin;j++)
            {
            	/*lint -save -e701*/
                if(pucPL[i] & (0x1 << j))
                {
                    bsp_gpio_set_value(i * bype_pin + j, 1);
                }
				/*lint -restore */

            }
        }
    }
    else if(GPIO_OPRT_GET == ulOp)
    {
        for(i = 0;i < GPIO_MAX_BANK_NUM * GPIO_MAX_PINS / bype_pin;i++)
        {
            pucPL[i] = 0;
            for(j = 0;j < bype_pin;j++)
            {
                value = bsp_gpio_get_value(i * bype_pin + j);
                if(GPIO_ERROR == value)
                {
                    continue;
                }
                pucPL[i] = (unsigned char)(pucPL[i] | ( (unsigned int)value << j));
            }
        }
    }
    
	return 0;
}

int gpio_oprt_write_test(void)
{
    unsigned char pucPL[GPIO_MAX_BANK_NUM * GPIO_MAX_PINS / 8]  = {0, 32, 0, 0};
    if(0 != drv_gpio_oprt(0, pucPL))
    {
        gpio_print_error("drv_gpio_oprt is fail.\n");
        return -1;
    }
    
    return 0;
}

int gpio_oprt_read_test(void)
{
    unsigned int i = 0;
    unsigned char pucPL[GPIO_MAX_BANK_NUM * GPIO_MAX_PINS / 8] = {0};
    drv_gpio_oprt(1, pucPL);

    for(i = 0;i < GPIO_MAX_BANK_NUM * GPIO_MAX_PINS / 8;i++)
    {
        gpio_print_info("char[%d] is %d.\n", i, pucPL[i]);
    }
    return 0;
    
}

int bsp_rf_ldo_init(void)
{
    return -1;
}

void rf_gpio_set_high(void)
{
    return;
}

void rf_gpio_set_low(void)
{
    return;
}

int ldo_gpio123_get_value(void)
{

    return -1;
}

int ldo_gpio124_get_value(void)
{

    return -1;
}

int outer_rfswitch_set(unsigned int status)
{
    return -1;
}

int outer_rfswitch_get(unsigned int *status)
{
    return -1;
}

int bsp_rf_switch_init(void)
{
    return -1;
}

int bsp_ant_modem_set(GPIO_ANT_MODESET_E mode)
{
    return -1;
}

int bsp_gpio_rf_pin_set(unsigned int mask, unsigned int is_hz, unsigned int value)
{
    return -1;
}

int bsp_antn_sw_unpd_config(ANTN_SW_GROUP_E sw_group ,unsigned int mux)
{
	return 0;
}

void bsp_dpm_powerup_antn_config(void)
{
	return ;
}
void bsp_dpm_powerdown_antn_config(void)
{
	return ;
}

int bsp_antn_sw_init(void)
{
	return 0;
}
#else

#define RF_GPIO_NV_MAX     16
#define RF_GPIO_ALLOW_MAX  30
#define ANTN_SW_GROUP_NUM  3	/*天线开关的组数*/
#define ANTN_SW_GPIO_MAX   8 	/*每组天线开关，最多由8个gpio组成*/
struct rf_gpio_cfg
{
    unsigned int gpio;
    unsigned int gpio_value;
    unsigned int mux_addr;		/*功能复用地址偏移*/
    unsigned int mux_gpio;		/*gpio功能*/
    unsigned int mux_ant;		/*线控功能*/
    unsigned int cg_addr;
    unsigned int cg_pullup;
    unsigned int cg_pulldown;
};

struct rf_gpio_cfg rf_gpio_cfg_tab[RF_GPIO_ALLOW_MAX] = {
    [0]  = {GPIO_127,1,iomg_096,0,1,iocg_130,1,0},/* ANTPA_SEL00 */
    [1]  = {GPIO_128,1,iomg_097,0,1,iocg_131,1,0},
    [2]  = {GPIO_129,1,iomg_098,0,1,iocg_132,1,0},
    [3]  = {GPIO_130,1,iomg_099,0,1,iocg_133,1,0},
    [4]  = {GPIO_131,1,iomg_100,0,1,iocg_134,1,0},
    [5]  = {GPIO_132,1,iomg_101,0,1,iocg_135,1,0},
    [6]  = {GPIO_133,1,iomg_102,0,1,iocg_136,1,0},
    [7]  = {GPIO_134,1,iomg_103,0,1,iocg_137,1,0},
    [8]  = {GPIO_135,1,iomg_104,0,1,iocg_138,1,0},
    [9]  = {GPIO_136,1,iomg_105,0,1,iocg_139,1,0},
    [10] = {GPIO_137,1,iomg_106,0,1,iocg_140,1,0},
    [11] = {GPIO_138,1,iomg_107,0,1,iocg_141,1,0},
    [12] = {GPIO_139,1,iomg_108,0,1,iocg_142,1,0},
    [13] = {GPIO_140,1,iomg_109,0,1,iocg_143,1,0},
    [14] = {GPIO_141,1,iomg_110,0,1,iocg_144,1,0},
    [15] = {GPIO_142,1,iomg_111,0,1,iocg_145,1,0},
    [16] = {GPIO_143,1,iomg_112,0,1,iocg_146,1,0},
    [17] = {GPIO_144,1,iomg_113,0,1,iocg_147,1,0},
    [18] = {GPIO_145,1,iomg_114,0,1,iocg_148,1,0},
    [19] = {GPIO_146,1,iomg_115,0,1,iocg_149,1,0},
    [20] = {GPIO_147,1,iomg_116,0,1,iocg_150,1,0},
    [21] = {GPIO_148,1,iomg_117,0,1,iocg_151,1,0},
    [22] = {GPIO_149,1,iomg_118,0,1,iocg_152,1,0},
    [23] = {GPIO_150,1,iomg_119,0,1,iocg_153,1,0},
    [24] = {GPIO_151,1,iomg_120,0,1,iocg_154,1,0},
    [25] = {GPIO_152,1,iomg_121,0,1,iocg_155,1,0},
    [26] = {GPIO_153,1,iomg_122,0,1,iocg_156,1,0},/* ANTPA_SEL26 */
    [27] = {GPIO_191,1,iomg_015,0,1,iocg_015,1,0},/* ANTPA_SEL27 */
    [28] = {GPIO_192,1,iomg_016,0,1,iocg_016,1,0},/* ANTPA_SEL28 */
    [29] = {GPIO_194,1,iomg_018,0,1,iocg_018,1,0},/* ANTPA_SEL30 */
};


#ifdef CONFIG_MODEM_PINTRL

struct rf_gpio_cfg gpio_outer[RF_GPIO_NV_MAX];
struct rf_gpio_cfg gpio_inside[RF_GPIO_NV_MAX];

#else

struct rf_gpio_cfg* gpio_outer[RF_GPIO_NV_MAX] = {0};
struct rf_gpio_cfg* gpio_inside[RF_GPIO_NV_MAX] = {0};

#endif


int is_in_global_cfg_table(unsigned int gpio)
{
    unsigned int i = 0;
    
    for(i = 0;i < RF_GPIO_ALLOW_MAX;i++)
    {
        if(rf_gpio_cfg_tab[i].gpio == gpio)
        {
            return 0;
        }

    }

    return -1;

}


struct rf_gpio_cfg*  get_global_rf_gpio_cfg(unsigned int gpio)
{
    unsigned int i = 0;

    for(i = 0;i < RF_GPIO_ALLOW_MAX;i++)
    {
        if(rf_gpio_cfg_tab[i].gpio == gpio)
        {
            return &(rf_gpio_cfg_tab[i]);
        }

    }

    return NULL;

}

void bsp_pintrl_mux_set_without_dts(unsigned int gpio, unsigned int mux)
{
    unsigned int base_addr = 0;
    struct rf_gpio_cfg *cfg = get_global_rf_gpio_cfg(gpio);

    if(NULL == cfg)
    {
        gpio_print_error("gpio%d is not in global table.\n", gpio);
        return;
    }
    if(cfg->gpio <= 164)
    {
        base_addr = HI_PINTRL_REG_ADDR;
    }
    else
    {
        base_addr = HI_PINTRL_SYS_REG_ADDR;
    }
    
    if(0 == mux)
    {
        writel(cfg->mux_gpio, base_addr + cfg->mux_addr);
        bsp_gpio_direction_output(cfg->gpio, cfg->gpio_value); 
    }
    else
    {
        writel(cfg->mux_ant, base_addr + cfg->mux_addr);
    }


}

int bsp_pintrl_mux_get_without_dts(unsigned int gpio, unsigned int *func)
{
    unsigned int base_addr = 0;
    struct rf_gpio_cfg *cfg = get_global_rf_gpio_cfg(gpio);

    if(NULL == cfg)
    {
        gpio_print_error("gpio%d is not in global table.\n", gpio);
        return -1;
    }
    if(cfg->gpio <= 164)
    {
        base_addr = HI_PINTRL_REG_ADDR;
    }
    else
    {
        base_addr = HI_PINTRL_SYS_REG_ADDR;
    }

    *func = readl(base_addr + cfg->mux_addr);

    return 0;

}

unsigned long drv_gpio_oprt(unsigned long ulOp, unsigned char *pucPL)
{
    return 0;
}
DRV_DRV_LDO_GPIO_STRU ldo_gpio_cfg_data;
unsigned int ldo_gpio_flag[LDO_GPIO_MAX];

/*********天线开关不下电特性接口Begin********/
unsigned int   sw_unpd_en = 0;
DRV_ANT_SW_UNPD_CFG sw_unpd_cfg[ANTN_SW_GROUP_NUM];

/*功能: 将天线开关设置为默认值
   场景: 1.RFIC下电时或dpm唤醒后配置为GPIO功能,并配置值;
                2.RFIC上电时配置为antn功能
   入参: sw_group 表示天线开关编号
                0为主分集1组,1为主分集2组,2为副modem天线开关
                
*/
int bsp_antn_sw_unpd_config(ANTN_SW_GROUP_E sw_group ,unsigned int mux)
{
	int i       = 0;
	int gpio_no = 0;	/*gpio编号*/
	int is_used = 0;	/*是否使用*/
	int value   = 0;	/*gpio配置值*/

	if(0 == sw_unpd_en){/*如果该特性功能未打开，则直接返回*/
		return 0;
	}
	if(sw_group >= ANTN_SW_GROUP_NUM){
		gpio_print_error("sw_group is : 0x%x,is invalid\n",sw_group);
		return -1;
	}

	/*根据NV值配置管脚*/
	for( ; i < ANTN_SW_GPIO_MAX; i++)
	{
		gpio_no = sw_unpd_cfg[sw_group].antn_switch[i].gpio_num;
		is_used = sw_unpd_cfg[sw_group].antn_switch[i].is_used;
		value   = sw_unpd_cfg[sw_group].antn_switch[i].value;

		/*判断当前gpio配置是否生效*/
		if(is_used){

			/*判断是否属于天线开关*/
			if(0 == is_in_global_cfg_table(gpio_no)){
				
				/*将此管脚工作模式，gpio还是antn*/
				bsp_pintrl_mux_set_without_dts(gpio_no , mux);

				if(0 == mux){
					/*将gpio方向设置为输出,并配置值*/
					bsp_gpio_direction_output(gpio_no , value);
				}
			}		
		}
	}
	return 0;
}

void bsp_dpm_powerup_antn_config(void)
{
	if(0 == sw_unpd_en){/*如果该特性功能未打开，则直接返回*/
		return ;
	}

	/*将天线开关相关管脚配置为默认态*/
	(void)bsp_antn_sw_unpd_config(MASTER_0 , 0);
	(void)bsp_antn_sw_unpd_config(MASTER_1 , 0);
	(void)bsp_antn_sw_unpd_config(NAGTIVE  , 0);

	/*打开天线开关*/
	bsp_pmu_hi6561_rf_poweron(0);
	bsp_pmu_hi6561_rf_poweroff(0);
	bsp_pmu_hi6561_rf_poweron(1);
	bsp_pmu_hi6561_rf_poweroff(1);
	return ;
}

void bsp_dpm_powerdown_antn_config(void)
{
    unsigned int ldo_gpio_num = sizeof(DRV_DRV_LDO_GPIO_STRU)/sizeof(DRV_DRV_LDO_GPIO_CFG);
    unsigned int i = 0;

	if(0 == sw_unpd_en){/*如果该特性功能未打开，则直接返回*/
		return ;
	}

	/*关闭主modem天线开关电源*/
	/*由lws53关闭保证*/
	
	/*关闭副modem天线开关电源(拉低gpio123)*/
    for(i = 0;i < ldo_gpio_num;i++)
    {    
		if((1 == ldo_gpio_cfg_data.ldo_gpio[i].used) && (GPIO_123 == ldo_gpio_cfg_data.ldo_gpio[i].gpio))
		{
			bsp_gpio_direction_output(GPIO_123 , 0);
			break;
		}
	}		
}
/*********天线开关不下电特性接口END************/


int bsp_ldo_gpio_init(void)
{
    unsigned int ret = 0;
//    unsigned int i = 0;
//    unsigned int j = 0;

    memset((void*)&ldo_gpio_cfg_data, 0, sizeof(DRV_DRV_LDO_GPIO_STRU));
    memset((void*)ldo_gpio_flag, 0, sizeof(ldo_gpio_flag));

    /* Get LDO NV data by id.*/
    ret = bsp_nvm_read(NV_ID_DRV_LDO_GPIO_CFG,(u8*)&ldo_gpio_cfg_data,sizeof(DRV_DRV_LDO_GPIO_STRU));
    if (ret !=  0)
    {
        gpio_print_error("rf_gpio read NV=0x%x, ret = %d \n",NV_ID_DRV_LDO_GPIO_CFG, ret);
        return -1;
    }

    return 0;
}

void rf_gpio_set_high(void)
{
    unsigned int ldo_gpio_num = sizeof(DRV_DRV_LDO_GPIO_STRU)/sizeof(DRV_DRV_LDO_GPIO_CFG);
    unsigned int i = 0;
    for(i = 0;i < ldo_gpio_num;i++)
    {
        if(1 == ldo_gpio_cfg_data.ldo_gpio[i].used)
        {
            bsp_gpio_direction_output(ldo_gpio_cfg_data.ldo_gpio[i].gpio, 1);
        }
        else
        {
            ldo_gpio_flag[i] = 1;
        }

    }


    //bsp_gpio_direction_output(GPIO_123, 1);
    //bsp_gpio_direction_output(GPIO_124, 1);

}

void rf_gpio_set_low(void)
{
    unsigned int ldo_gpio_num = sizeof(DRV_DRV_LDO_GPIO_STRU)/sizeof(DRV_DRV_LDO_GPIO_CFG);
    unsigned int i = 0;
    for(i = 0;i < ldo_gpio_num;i++)
    {
        if(1 == ldo_gpio_cfg_data.ldo_gpio[i].used)
        {
	        
			/*如果天线开关不下电特性打开并且为gpio_123,则不可以拉低此*/
			if(ldo_gpio_cfg_data.ldo_gpio[i].gpio == GPIO_123 && sw_unpd_en)
				continue;
			
	        bsp_gpio_direction_output(ldo_gpio_cfg_data.ldo_gpio[i].gpio, 0);
    	}		
		else
		{
			ldo_gpio_flag[i] = 0;
		}
    }
    //bsp_gpio_direction_output(GPIO_123, 0);
    //bsp_gpio_direction_output(GPIO_124, 0);

}

int ldo_gpio123_get_value(void)
{
    if(1 == ldo_gpio_cfg_data.ldo_gpio[0].used)
    {
        return bsp_gpio_get_value(ldo_gpio_cfg_data.ldo_gpio[0].gpio);
    }

    return (int)(ldo_gpio_flag[0]);


    
    /*if(1 == bsp_gpio_direction_get(GPIO_123))
    {
        return bsp_gpio_get_value(GPIO_123);
    }
    gpio_print_error("gpio123 direction is error.\n");
    return -1;*/
}

int ldo_gpio124_get_value(void)
{
    if(1 == ldo_gpio_cfg_data.ldo_gpio[1].used)
    {
        return bsp_gpio_get_value(ldo_gpio_cfg_data.ldo_gpio[1].gpio);
    }

    return (int)(ldo_gpio_flag[1]);

    /*if(1 == bsp_gpio_direction_get(GPIO_124))
    {
        return bsp_gpio_get_value(GPIO_124);

    }
    gpio_print_error("gpio124 direction is error.\n");
    return -1;*/
    
}

#ifdef CONFIG_MODEM_PINTRL

#define RF_PIN_MAX  31
#define RF_GPIO_SEP 27
#define RF_UNSUPPORT_NO 29

int bsp_gpio_rf_pin_set(unsigned int mask, unsigned int is_hz, unsigned int value)
{
    unsigned int i = 0;
    unsigned int gpio = 0;
    unsigned int base_addr = 0;
    unsigned int pull = 0;

    struct rf_gpio_cfg *cfg = NULL;
    
    while(i < RF_PIN_MAX)
    {       
        if((mask & 0x1) && (RF_UNSUPPORT_NO != i))
        {
            if(i < RF_GPIO_SEP)
            {
                gpio = GPIO_127 + i;
            }
            else
            {
                gpio = GPIO_191 + (i - RF_GPIO_SEP);
            }

            if(gpio <= 164)
            {
                base_addr = HI_PINTRL_REG_ADDR;
            }
            else
            {
                base_addr = HI_PINTRL_SYS_REG_ADDR;
            }
            
            pull = (value & 0x1) ? 0x1 : 0x2;
            
            if(0 == is_in_modem_pintrl(gpio))
            {
                if(is_hz & 0x1)
                {
                    bsp_set_pintrl(MODEM_GPIO_INPUT, gpio, 0x0);
                    bsp_set_pintrl(MODEM_IO_CFG, gpio, pull);

                }
                else
                {
                    bsp_set_pintrl(MODEM_IO_CFG, gpio, 0x0);
                    bsp_set_pintrl(MODEM_IO_MUX, gpio, 0x1);
                }
            }
            else if(0 == is_in_global_cfg_table(gpio))
            {
                cfg = get_global_rf_gpio_cfg(gpio);
                if(is_hz & 0x1)
                {
                    writel(0x0, base_addr + cfg->mux_addr);
                    writel(pull, base_addr + cfg->cg_addr);
                    bsp_gpio_direction_input(gpio);
                }
                else
                {
                    writel(0x0, base_addr + cfg->cg_addr);
                    writel(0x1, base_addr + cfg->mux_addr);
                }
            }
            else
            {
                gpio_print_error("Please make sure that gpio%d has been configed in modem dts.\n", gpio);
                return -1;

            }

        }

        mask = mask>>1;
        value = value>>1;
        is_hz = is_hz>>1;
        ++i;
    }

    return 0;
}

int bsp_rse_gpio_set(unsigned int flag, unsigned int mask, unsigned int value)
{
    unsigned int i = 0;
    unsigned int gpio = 0;
    unsigned int base_addr = 0;

    struct rf_gpio_cfg *cfg = NULL;
    
    if(0x0 != flag && (0x1 != flag))
    {
        gpio_print_error("para error, flag = %d.\n", flag);
        return -1;
    }

    while(i < RF_PIN_MAX)
    {       
        if((mask & 0x1) && (RF_UNSUPPORT_NO != i))
        {
            if(i < RF_GPIO_SEP)
            {
                gpio = GPIO_127 + i;
            }
            else
            {
                gpio = GPIO_191 + (i - RF_GPIO_SEP);
            }

            if(gpio <= 164)
            {
                base_addr = HI_PINTRL_REG_ADDR;
            }
            else
            {
                base_addr = HI_PINTRL_SYS_REG_ADDR;
            }

            if(0x1 == flag)
            {
                if(0 == is_in_modem_pintrl(gpio))
                {
                    bsp_set_pintrl(MODEM_GPIO_OUTPUT, gpio, value & 0x1);
                }
                else if(0 == is_in_global_cfg_table(gpio))
                {
                    cfg = get_global_rf_gpio_cfg(gpio);
                    writel(0x0, base_addr + cfg->mux_addr);
                    writel(0x0, base_addr + cfg->cg_addr);
                    bsp_gpio_direction_output(gpio, value & 0x1);
                }
                else
                {
                    gpio_print_error("Please make sure that gpio%d has been configed in modem dts.\n", gpio);
                    return -1;
                }
            }
            else
            {
                if(0 == is_in_modem_pintrl(gpio))
                {
                    bsp_set_pintrl(MODEM_IO_MUX, gpio, 0x1);
                }
                else if(0 == is_in_global_cfg_table(gpio))
                {
                    cfg = get_global_rf_gpio_cfg(gpio);
                    writel(0x1, base_addr + cfg->mux_addr);
                }
                else
                {
                    gpio_print_error("Please make sure that gpio%d has been configed in modem dts.\n", gpio);
                    return -1;
                }

            }

        }

        mask = mask>>1;
        value = value>>1;
        ++i;
            
    }
    return 0;
}

/*会调用pastar接口，放在初始化最执行*/
int bsp_antn_sw_init(void)
{
	int ret;

	/*读取天线开关不下电特性是否使能*/
    ret = bsp_nvm_read(NV_ID_DRV_ANTN_UNPD_FLAG,(u8*)(&sw_unpd_en),sizeof(DRV_ANT_SW_UNPD_ENFLAG));	
    if (ret !=  0)
    {
        gpio_print_error("rf_gpio read NV=0x%x, ret = %d \n",NV_ID_DRV_ANTN_UNPD_FLAG, ret);
    }

	/*读取天线开关不下电特性中gpio的默认配置*/
    ret = bsp_nvm_read(NV_ID_DRV_ANTN_UNPD_CFG,(u8*)sw_unpd_cfg,sizeof(NV_DRV_ANT_SW_UNPD_CFG));	
    if (ret !=  0)
    {
        gpio_print_error("rf_gpio read NV=0x%x, ret = %d \n",NV_ID_DRV_ANTN_UNPD_CFG, ret);
    }
    /*初始化时配置天线开关默为认值，与dpm唤醒时是一致的*/
	bsp_dpm_powerup_antn_config();

	return ret;

}


int bsp_rf_switch_init(void)
{
    unsigned int  ret = 0;
    unsigned int i = 0;

    unsigned int outer_index = 0;
    unsigned int inside_index = 0;
    RF_GPIO_CFG rf_gpio_cfg_data[RF_GPIO_NV_MAX];

    memset((void*)rf_gpio_cfg_data, 0, sizeof(rf_gpio_cfg_data));
    memset((void*)gpio_outer,  0, sizeof(gpio_outer));
    memset((void*)gpio_inside, 0, sizeof(gpio_inside));

    /*Get NV data by NV id*/
    ret = bsp_nvm_read(NV_ID_RF_SWITCH_CFG,(u8*)rf_gpio_cfg_data,sizeof(rf_gpio_cfg_data));
    if (ret !=  0)
    {
        gpio_print_error("rf_gpio read NV=0x%x, ret = %d \n",NV_ID_RF_SWITCH_CFG, ret);
        return -1;
    }

    /*Write NV data to global struct*/
    for(i = 0; i < RF_GPIO_NV_MAX; i++)
    {
        if(1 == rf_gpio_cfg_data[i].modem_inside.is_used)
        {
            gpio_inside[inside_index].gpio = rf_gpio_cfg_data[i].rf_gpio_num;
            gpio_inside[inside_index].gpio_value = rf_gpio_cfg_data[i].modem_inside.gpio_level;
            ++inside_index;
        }
        else if(1 == rf_gpio_cfg_data[i].modem_outside.is_used)
        {
            gpio_outer[outer_index].gpio = rf_gpio_cfg_data[i].rf_gpio_num;
            gpio_outer[outer_index].gpio_value = rf_gpio_cfg_data[i].modem_outside.gpio_level;
            ++outer_index;

        }

    }

    return 0;

}


int bsp_ant_modem_set(GPIO_ANT_MODESET_E mode)
{
    unsigned int i = 0;

    if(0 == gpio_inside[0].gpio)
    {
        //gpio_print_error("nv config error.\n");
        return -1;
    }
    
    if(ANT_LTE == mode)
    {
        for( i = 0 ; i < RF_GPIO_NV_MAX ; i++ )
        {
            if(0 == gpio_inside[i].gpio)
            {
                return 0;
            }
            
            if(0 == is_in_modem_pintrl(gpio_inside[i].gpio))
            {
                bsp_set_pintrl(MODEM_GPIO_OUTPUT, gpio_inside[i].gpio, gpio_inside[i].gpio_value);

            }
            else if(0 == is_in_global_cfg_table(gpio_inside[i].gpio))
            {
                bsp_pintrl_mux_set_without_dts(gpio_inside[i].gpio, 0);

            }
            else
            {
            }
            

        }
    }
    else if(ANT_GSM == mode)
    {
        for( i = 0 ; i < RF_GPIO_NV_MAX ; i++ )
        {
            if(0 == gpio_inside[i].gpio)
            {
                return 0;
            }
            
            if(0 == is_in_modem_pintrl(gpio_inside[i].gpio))
            {
                bsp_set_pintrl(MODEM_IO_MUX, gpio_inside[i].gpio, 1);

            }
            else if(0 == is_in_global_cfg_table(gpio_inside[i].gpio))
            {
                bsp_pintrl_mux_set_without_dts(gpio_inside[i].gpio, 1);

            }
            else
            {
            }
            

        }
    }
    else
    {
        gpio_print_error("para error, mode=%d.\n", mode);
    }

    return -1;
}




int outer_rfswitch_set(unsigned int status)
{   
    unsigned int i = 0;

    if(0 == gpio_outer[0].gpio)
    {
        //gpio_print_error("nv config error.\n");
        return -1;
    }
    
    if(OUTER_RFSWITCH_ON == status)
    {
        for( i = 0 ; i < RF_GPIO_NV_MAX ; i++ )
        {
            if(0 == gpio_outer[i].gpio)
            {
                return 0;
            }
            if(0 == is_in_modem_pintrl(gpio_outer[i].gpio))
            {
                bsp_set_pintrl(MODEM_GPIO_OUTPUT, gpio_outer[i].gpio, gpio_outer[i].gpio_value);

            }
            else if(0 == is_in_global_cfg_table(gpio_outer[i].gpio))
            {
                bsp_pintrl_mux_set_without_dts(gpio_outer[i].gpio, 0);

            }
            else
            {
            }
            

        }
    }
    else if(OUTER_RFSWITCH_OFF == status)
    {
        for( i = 0 ; i < RF_GPIO_NV_MAX ; i++ )
        {
            if(0 == gpio_outer[i].gpio)
            {
                return 0;
            }
            if(0 == is_in_modem_pintrl(gpio_outer[i].gpio))
            {
                bsp_set_pintrl(MODEM_IO_MUX, gpio_outer[i].gpio, 1);

            }
            else if(0 == is_in_global_cfg_table(gpio_outer[i].gpio))
            {
                bsp_pintrl_mux_set_without_dts(gpio_outer[i].gpio, 1);

            }
            else
            {
            }


        }
    }
    else
    {
        gpio_print_error("para error, status=%d.\n", status);
    }

    return -1;
}


int outer_rfswitch_get(unsigned int *status)
{
    unsigned int i = 0;
    unsigned int func = 0;
    unsigned int func_t = 0;
    
    if(NULL == status)
    {
        //gpio_print_error("para error, status=%d.\n", status);
        return -1;
    }

    if(0 == gpio_outer[0].gpio)
    {
        gpio_print_error("nv config error.\n");
        return -1;
    }
    
    for( i = 0 ; i < RF_GPIO_NV_MAX ; i++ )
    {
        if(0 == gpio_outer[i].gpio)
        {
            break;
        }

        if(0 == is_in_modem_pintrl(gpio_outer[i].gpio) && (0 == bsp_get_pintrl(MODEM_IO_MUX, gpio_outer[i].gpio, &func)))
        {
            func_t |= func;

        }
        else if(0 == is_in_global_cfg_table(gpio_outer[i].gpio) && (0 == bsp_pintrl_mux_get_without_dts(gpio_outer[i].gpio, &func)))
        {
            func_t |= func;

        }        
        else
        {
            gpio_print_error("bsp_get_pintrl fail, i =%d.\n", i);
            return -1;
        }

    }
    
    if(0 == func_t)
    {
        *status = OUTER_RFSWITCH_ON;
    }
    else
    {
        *status = OUTER_RFSWITCH_OFF;
    }

    return 0;
}

#else

int bsp_gpio_rf_pin_set(unsigned int mask, unsigned int is_hz, unsigned int value)
{
    return -1;
}


int bsp_rf_switch_init(void)
{
    unsigned int  ret = 0;
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int outer_index = 0;
    unsigned int inside_index = 0;
    RF_GPIO_CFG rf_gpio_cfg_data[RF_GPIO_NV_MAX];

    memset((void*)rf_gpio_cfg_data, 0, sizeof(rf_gpio_cfg_data));
    
    /*Get NV data by NV id*/
    ret = bsp_nvm_read(NV_ID_RF_SWITCH_CFG,(u8*)rf_gpio_cfg_data,sizeof(rf_gpio_cfg_data));
    if (ret !=  0)
    {
        gpio_print_error("rf_gpio read NV=0x%x, ret = %d \n",NV_ID_RF_SWITCH_CFG, ret);
        return -1;
    }

    /*Write NV data to global struct*/
    for(i = 0; i < RF_GPIO_NV_MAX; i++)
    {
        if(1 == rf_gpio_cfg_data[i].modem_inside.is_used)
        {
            for(j = 0;j < RF_GPIO_ALLOW_MAX;j++)
            {
                if(rf_gpio_cfg_tab[j].gpio == rf_gpio_cfg_data[i].rf_gpio_num)
                {
                    rf_gpio_cfg_tab[j].gpio_value = rf_gpio_cfg_data[i].modem_inside.gpio_level;
                    gpio_inside[inside_index] = &(rf_gpio_cfg_tab[j]);

                    ++inside_index;
                }

            }

        }
        else if(1 == rf_gpio_cfg_data[i].modem_outside.is_used)
        {
            for(j = 0;j < RF_GPIO_ALLOW_MAX;j++)
            {
                if(rf_gpio_cfg_tab[j].gpio == rf_gpio_cfg_data[i].rf_gpio_num)
                {
                    rf_gpio_cfg_tab[j].gpio_value = rf_gpio_cfg_data[i].modem_outside.gpio_level;
                    gpio_outer[outer_index] = &(rf_gpio_cfg_tab[j]);

                    ++outer_index;
                }

            }
        }
    }
    
    return 0;

}


int bsp_ant_modem_set(GPIO_ANT_MODESET_E mode)
{
    unsigned int i = 0;
    unsigned int base_addr = 0;

    if(NULL == gpio_inside[0])
    {
        gpio_print_error("nv config error.\n");
        return -1;
    }
    
    if(ANT_LTE == mode)
    {
        for( i = 0 ; i < RF_GPIO_NV_MAX ; i++ )
        {
            if(NULL == gpio_inside[i])
            {
                return 0;
            }
            
            if(gpio_inside[i]->gpio <= 164)
            {
                base_addr = HI_PINTRL_REG_ADDR;
            }
            else
            {
                base_addr = HI_PINTRL_SYS_REG_ADDR;
            }
            
            writel(gpio_inside[i]->mux_gpio, base_addr + gpio_inside[i]->mux_addr);
            bsp_gpio_direction_output(gpio_inside[i]->gpio, gpio_inside[i]->gpio_value);

        }
    }
    else if(ANT_GSM == mode)
    {
        for( i = 0 ; i < RF_GPIO_NV_MAX ; i++ )
        {
            if(NULL == gpio_inside[i])
            {
                return 0;
            }
            
            if(gpio_inside[i]->gpio <= 164)
            {
                base_addr = HI_PINTRL_REG_ADDR;
            }
            else
            {
                base_addr = HI_PINTRL_SYS_REG_ADDR;       
            }
            
            writel(gpio_inside[i]->mux_ant, base_addr + gpio_inside[i]->mux_addr);

        }
    }
    else
    {
        gpio_print_error("para error, mode=%d.\n", mode);
    }

    return -1;
}




int outer_rfswitch_set(unsigned int status)
{   
    unsigned int i = 0;
    unsigned int base_addr = 0;

    if(NULL == gpio_outer[0])
    {
        gpio_print_error("nv config error.\n");
        return -1;
    }
    
    if(OUTER_RFSWITCH_ON == status)
    {
        for( i = 0 ; i < RF_GPIO_NV_MAX ; i++ )
        {
            if(NULL == gpio_outer[i])
            {
                return 0;
            }
            
            if(gpio_outer[i]->gpio <= 164)
            {
                base_addr = HI_PINTRL_REG_ADDR;
            }
            else
            {
                base_addr = HI_PINTRL_SYS_REG_ADDR;
            }
            
            writel(gpio_outer[i]->mux_gpio, base_addr + gpio_outer[i]->mux_addr);
            bsp_gpio_direction_output(gpio_outer[i]->gpio, gpio_outer[i]->gpio_value);

        }
    }
    else if(OUTER_RFSWITCH_OFF == status)
    {
        for( i = 0 ; i < RF_GPIO_NV_MAX ; i++ )
        {
            if(NULL == gpio_outer[i])
            {
                return 0;
            }
            
            if(gpio_outer[i]->gpio <= 164)
            {
                base_addr = HI_PINTRL_REG_ADDR;
            }
            else
            {
                base_addr = HI_PINTRL_SYS_REG_ADDR;       
            }
            
            writel(gpio_outer[i]->mux_ant, base_addr + gpio_outer[i]->mux_addr);

        }
    }
    else
    {
        gpio_print_error("para error, status=%d.\n", status);
    }

    return -1;
}


int outer_rfswitch_get(unsigned int *status)
{
    unsigned int i = 0;
    unsigned int base_addr = 0;
    unsigned int func = 0;
    
    if(NULL == status)
    {
        gpio_print_error("para error, status=%d.\n", status);
        return -1;
    }

    if(NULL == gpio_outer[0])
    {
        gpio_print_error("nv config error.\n");
        return -1;
    }
    
    for( i = 0 ; i < RF_GPIO_NV_MAX ; i++ )
    {
        if(NULL == gpio_outer[i])
        {
            break;
        }
        
        if(gpio_outer[i]->gpio <= 164)
        {
            base_addr = HI_PINTRL_REG_ADDR;
        }
        else
        {
            base_addr = HI_PINTRL_SYS_REG_ADDR;       
        }
        
        func |= readl(base_addr + gpio_outer[i]->mux_addr);

    }
    
    if(0 == func)
    {
        *status = OUTER_RFSWITCH_ON;
    }
    else
    {
        *status = OUTER_RFSWITCH_OFF;
    }

    return 0;
}

#endif

#endif



