#include "osl_common.h"

#include "bsp_om.h"
#include <bsp_mipi.h>
#include "bsp_nvim.h"
#include "drv_nv_id.h"
#include "drv_nv_def.h"

#include "gpio_balong.h"
#include "bsp_pmu_hi6561.h"

#include "product_config.h"

#ifdef CONFIG_RSE

#define  rse_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RSE, "[rse]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  rse_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RSE, "[rse]: "fmt, ##__VA_ARGS__))


RF_NV_RSE_CFG_STRU     rse_cfg     = {0x0,0x0,0x0,0x0,0x0};
unsigned int           rse_mipi_en = 0;
DRV_DRV_ANT_SW_MIPI_CONFIG rse_mipi_cfg;

int bsp_rf_rse_init(void)
{
    unsigned int  ret = 0;

    /*Get NV data by NV id*/
    ret = bsp_nvm_read(NV_ID_RF_RSE_CFG,(u8*)&rse_cfg,sizeof(rse_cfg));
    if (ret !=  0)
    {
        rse_print_error(" read rse NV=0x%x, ret = %d \n",NV_ID_RF_RSE_CFG, ret);
    }
	
    ret = bsp_nvm_read(NV_ID_DRV_RSE_MIPI_EN,(u8*)&rse_mipi_en,sizeof(rse_mipi_en));
    if (ret !=  0)
    {
        rse_print_error(" read rse NV=0x%x, ret = %d \n",NV_ID_RF_RSE_CFG, ret);
    }

	if(rse_mipi_en)
	{
	    ret |= bsp_nvm_read(NV_ID_DRV_RSE_MIPI_CONFIG,(u8*)&rse_mipi_cfg,sizeof(rse_mipi_cfg));
	    if (ret !=  0)
	    {
	        rse_print_error(" read rse NV=0x%x, ret = %d ,rse mipi can't config!\n",NV_ID_DRV_RSE_MIPI_CONFIG, ret);
	    }
		else{
			ret = bsp_rse_mipi_config_on();
		}
	}

    return ret;
}

int bsp_rse_on(void)
{
    int ret = 0;
    unsigned int select = rse_cfg.ulFemCtrlInfo;
    unsigned int mask   = rse_cfg.ulRfGpioBitMask;
    unsigned int value  = rse_cfg.ulRfGpioOutValue;

    unsigned int power  = rse_cfg.ulRsePowerOnIds;

    u8 mipi_slave_addr = (u8)((rse_cfg.usFemMipiCmdAddr >> 9) & 0x0F);
    u8 mipi_register_addr = (u8)((rse_cfg.usFemMipiCmdAddr >> 1) & 0x1F);
    u8 mipi_data = (u8)((rse_cfg.usFemMipiCmdData >> 1) & 0xFF);

    ret = pmu_hi6561_power_on(power, HI6561_0);
    if(0 != ret)
    {
        rse_print_error(" pmu_hi6561_power_on error, ret = %d!\n", ret);
        return -1;
    }

    if(select & 0x1)
    {
        ret = bsp_rse_gpio_set(1, mask, value);
    }
    if(0x2 == (select & 0x2))
    {
        ret = bsp_mipi_data_send(MIPI_WRITE, mipi_slave_addr, mipi_register_addr, mipi_data, MIPI_0);
    }

    return ret;


}


int bsp_rse_off(void)
{
    int ret = 0;
    unsigned int select = rse_cfg.ulFemCtrlInfo;
    unsigned int mask   = rse_cfg.ulRfGpioBitMask;
    unsigned int value  = rse_cfg.ulRfGpioOutValue;
    unsigned int power   = rse_cfg.ulRsePowerOnIds;

    u8 mipi_slave_addr = (u8)((rse_cfg.usFemMipiCmdAddr >> 9) & 0x0F);
    u8 mipi_register_addr = (u8)((rse_cfg.usFemMipiCmdAddr >> 1) & 0x1F);
    u8 mipi_data = (u8)((rse_cfg.usFemMipiCmdData >> 1) & 0xFF);

    ret = pmu_hi6561_power_off(power, HI6561_0);
    if(0 != ret)
    {
        rse_print_error(" pmu_hi6561_power_on error, ret = %d!\n", ret);
        return -1;
    }

    if(select & 0x1)
    {
        ret = bsp_rse_gpio_set(0, mask, value);

    }
    if(0x2 == (select & 0x2))
    {
        ret = bsp_mipi_data_send(MIPI_WRITE, mipi_slave_addr, mipi_register_addr, mipi_data, MIPI_0);
    }


    return ret;

}
int bsp_rse_mipi_config_on(void)
{
	int ret = 0;

	/*打开天线开关的电源*/	
	ret =  bsp_pmu_hi6561_rf_poweron(0);
	ret |= bsp_pmu_hi6561_rf_poweroff(0);
	ret |= bsp_pmu_hi6561_rf_poweron(1);
	ret |= bsp_pmu_hi6561_rf_poweroff(1);
	if(ret){
		rse_print_error("pmu 6561 power config fail!\n");
		return -1;
	}
	
	/*rse mipi init*/
	ret =  bsp_rse_mipi_base_config(0);
	ret |= bsp_rse_mipi_base_config(1);	
	if(ret){
		rse_print_error("rse mipi config fail!\n");
		return -1;
	}

	return 0;
}


int bsp_rse_mipi_base_config(PWC_COMM_MODEM_E modem_id)
{
	int ret      = 0;
	int index    = 0;
	u8  mipi_chn = 0;
	u8  slave_id = 0;
	u8  offset   = 0;
	u8  data     = 0;


	/*配置天线开关寄存器*/
	for(; index < RSE_MIPI_SW_REG_NUM ; index++){
		if(rse_mipi_cfg.all_switch[modem_id].modem_switch[index].is_invalid){			
			mipi_chn = rse_mipi_cfg.all_switch[modem_id].modem_switch[index].mipi_chn;
			slave_id = rse_mipi_cfg.all_switch[modem_id].modem_switch[index].slave_id;
			offset	 = rse_mipi_cfg.all_switch[modem_id].modem_switch[index].reg_offset;
			data	 = rse_mipi_cfg.all_switch[modem_id].modem_switch[index].value;

			ret = bsp_mipi_data_send(MIPI_WRITE, slave_id, offset, data, mipi_chn);
			if(ret){
				rse_print_error("mipi send rse data failed! mipi id =%d\n",mipi_chn);
				return ret;
			}
		}
	}

	return 0;
}
#else
unsigned int           rse_mipi_en = 0;
int bsp_rf_rse_init(void)
{
    return 0;
}

int bsp_rse_on(void)
{
    return -1;
}

int bsp_rse_off(void)
{
    return -1;
}
int bsp_rse_mipi_config(PWC_COMM_MODEM_E modem_id)
{
	return -1;
}
int bsp_rse_mipi_config_on(void)
{
	return -1;
}

int bsp_rse_mipi_base_config(PWC_COMM_MODEM_E modem_id)
{
    return 0;
}

#endif


