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


RF_NV_RSE_CFG_STRU rse_cfg = {0x0,0x0,0x0,0x0,0x0};

int bsp_rf_rse_init(void)
{
    unsigned int  ret = 0;

    /*Get NV data by NV id*/
    ret = bsp_nvm_read(NV_ID_RF_RSE_CFG,(u8*)&rse_cfg,sizeof(rse_cfg));
    if (ret !=  0)
    {
        rse_print_error(" read rse NV=0x%x, ret = %d \n",NV_ID_RF_RSE_CFG, ret);
        return -1;
    }
    return 0;
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

#else
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

#endif


