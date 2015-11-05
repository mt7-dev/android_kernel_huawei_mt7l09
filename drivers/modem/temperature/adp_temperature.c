#include <bsp_shared_ddr.h>
#include "drv_temp_cfg.h"
#include "bsp_memmap.h"
#include "drv_temp_cfg.h"
#include "bsp_nvim.h"
#include "drv_nv_id.h"
#include "drv_nv_def.h"
#include "bsp_om.h"
#include "bsp_temperature.h"

void tem_volt_get_test(void);
void tem_phy_logic(void);
void tem_battery_show(void);
void tem_config_show(void);
void tem_volt_null_test(void);
void tem_output_show(void);

void drv_hkadc_get_temp_debug(HKADC_TEMP_PROTECT_E mode);

unsigned int chan_error_num[HKADC_TEMP_BUTT]= {0};

#ifdef CONFIG_TEMPERATURE_PROTECT
static int phy_chan_num_get(int logic)
{
    int i = 0;
    DRV_HKADC_DATA_AREA *p_area = (DRV_HKADC_DATA_AREA *)TEMPERATURE_VIRT_ADDR;
    unsigned short *phy_tbl = p_area->phy_tbl;

    if(logic < 0 || logic >= HKADC_CHAN_MAX)
    {
        return TEMPERATURE_ERROR;
    }
    
    for(i = 0;i < HKADC_CHAN_MAX;i++)
    {
        if(phy_tbl[i] == logic)
        {
            return i;
        }
    }
    return TEMPERATURE_ERROR;
}

BSP_S32 drv_hkadc_get_temp(HKADC_TEMP_PROTECT_E enMode, unsigned short* pusVolt, short* psTemp, HKADC_CONV_MODE_E ulMode)
{

    short tem    = 0;
    unsigned short volt   = 0;    
    //short tem_r  = 0;
    //unsigned short volt_r = 0;
    unsigned int ret = 0;    
    int phy_chan = -1;
    
    DRV_HKADC_DATA_AREA *p_area = (DRV_HKADC_DATA_AREA *)TEMPERATURE_VIRT_ADDR;
    
    if(TEMPERATURE_MAGIC_DATA != p_area->magic_start || (TEMPERATURE_MAGIC_DATA != p_area->magic_end))
    {
        tem_print_error("tem mem is writed by others.\n");
        return TEMPERATURE_ERROR;
    }
    
    phy_chan = phy_chan_num_get(enMode);

    if(TEMPERATURE_ERROR == phy_chan)
    {
        ++(chan_error_num[enMode]);
        if(chan_error_num[enMode] > ERROR_CHAN_MAX)
        {
            return TEMPERATURE_ERROR;
        }
        
        tem_phy_logic();
        
        tem_print_info("phy_chan_num_get para is %d.\n" , enMode);

        if(0 != (ret = bsp_nvm_read(NV_ID_DRV_TEMP_CHAN_MAP, (u8*)(p_area->phy_tbl), sizeof(BSP_U16) * HKADC_CHAN_MAX)))
        {
            tem_print_error("bsp_nvm_read is error, ret is 0x%x.\n" , ret);
            return TEMPERATURE_ERROR;
        }
        
        phy_chan = phy_chan_num_get(enMode);

        if(TEMPERATURE_ERROR == phy_chan)
        {
            tem_print_error("phy_chan_num_get is error, para is %d.\n" , enMode);
            
            return TEMPERATURE_ERROR;
        }

    }

    tem    = p_area->chan_out[phy_chan].temp_l;
    //tem_r  = p_area->chan_out[phy_chan].temp_r;

    volt   = p_area->chan_out[phy_chan].volt_l;
    //volt_r = p_area->chan_out[phy_chan].volt_r;

    /*取反校验暂时不判断*/
    if(NULL != pusVolt)
    {
        *pusVolt = volt;
    }
    if(NULL != psTemp)
    {
        *psTemp = tem;
    }

    return TEMPERATURE_OK;
}

#else

BSP_S32 drv_hkadc_get_temp(HKADC_TEMP_PROTECT_E enMode, unsigned short* pusVolt, short* psTemp, HKADC_CONV_MODE_E ulMode)
{    
    if((HKADC_TEMP_PA0 == enMode || (HKADC_TEMP_PA1 == enMode)) && (NULL != psTemp))
    {
        *psTemp = 260;
        return TEMPERATURE_OK;
    }

    return TEMPERATURE_OK;
}

#endif

void tem_volt_null_test(void)
{
    unsigned short volt = 0;
    short temp = 0;
    
    if(TEMPERATURE_OK != drv_hkadc_get_temp(0,NULL,&temp, HKADC_CONV_DELAY))
    {
        tem_print_error("volt null is fail.\n");
    }
    if(TEMPERATURE_OK != drv_hkadc_get_temp(0,&volt,NULL, HKADC_CONV_DELAY))
    {
        tem_print_error("tem volt null is fail.\n");
    }
}


void tem_volt_get_test(void)
{
    unsigned short volt = 0;
    short temp = 0;

    int i = 0;

    for(i = 0; i < HKADC_TEMP_BUTT; i++)
    {
        if(TEMPERATURE_OK == drv_hkadc_get_temp(i, &volt, &temp, HKADC_CONV_DELAY))
        {
            tem_print_info("volt = %d, tem = %d.\n", volt, temp);

        }

    }
}

/* 温度保护DEBUG函数 */
void tem_phy_logic(void)
{
    int i = 0;
    
    DRV_HKADC_DATA_AREA *p_area = (DRV_HKADC_DATA_AREA *)TEMPERATURE_VIRT_ADDR;
    
    tem_print_info("-------------TEMPERATURE PHYSICAL LOGFIC TABLE  ------------\n");
    for(i = 0; i < HKADC_CHAN_MAX; i++)
    {
        tem_print_info("phy num = %d ,logic num = %d.\n", i, p_area->phy_tbl[i]);
    }
    tem_print_info("-------------------------------------------------------------\n");

}

void tem_battery_show(void)
{
    DRV_HKADC_DATA_AREA *p_area = (DRV_HKADC_DATA_AREA *)TEMPERATURE_VIRT_ADDR;
    
    tem_print_info("-------------BATTERY TEMPERATURE PROTECT CONFIG------------\n");
    tem_print_info("high tem protect:        %s\n", p_area->sys_temp_cfg.enable & 0x1 ? "yes" : "no"); /* bit0：高温保护使能；bit1：低温保护使能 */
    tem_print_info("low tem protect:         %s\n", p_area->sys_temp_cfg.enable & 0x2 ? "yes" : "no");
    
    tem_print_info("hkadc count is:          %d\n", p_area->sys_temp_cfg.hkadc_id);          /* 高低温保护的hkadc通道ID */
    tem_print_info("high tem gate is:        %d\n", p_area->sys_temp_cfg.high_thres);        /* 高温保护的阀值 */
    tem_print_info("high gate count is:      %d\n", p_area->sys_temp_cfg.high_count);        /* 高温保护次数门限，到达该次数后系统关机 */
    
    tem_print_info("low tem gate is:         %d\n", p_area->sys_temp_cfg.low_thres);         /* 低温保护的阀值 */
    tem_print_info("low gate count is:       %d\n", p_area->sys_temp_cfg.low_count);         /* 低温保护次数门限，到达该次数后系统关机 */
    
    tem_print_info("-------------------------------------------------------------\n");
    
}

void tem_chip_show(void)
{
    int i = 0;
    
    DRV_HKADC_DATA_AREA *p_area = (DRV_HKADC_DATA_AREA *)TEMPERATURE_VIRT_ADDR;
    
    tem_print_info("-------------CHIP TEMPERATURE PROTECT CONFIG------------\n");
    
    for(i = 0;i < TSENS_REGION_MAX;i++)
    {
        tem_print_info("tsens%d high tem protect:         %s\n", i, p_area->tens_cfg[i].enable ? "yes" : "no");    
        tem_print_info("tsens%d high tem gate is:         %d\n", i, p_area->tens_cfg[i].high_thres);
        tem_print_info("tsens%d high gate count is:       %d\n", i, p_area->tens_cfg[i].high_count);
    }
    
    tem_print_info("-------------------------------------------------------------\n");
}

void tem_config_show(void)
{
    int i = 0;
    
    DRV_HKADC_DATA_AREA *p_area = (DRV_HKADC_DATA_AREA *)TEMPERATURE_VIRT_ADDR;

    tem_print_info("  channel \t  enable \t period \t table len\n");
    tem_print_info("  ------- \t  ------ \t ------ \t -------\n");
    for(i = 0;i < HKADC_CHAN_MAX;i++)
    {
        tem_print_info("%6d\t%6d\t       %6d\t       %6d\n", i, p_area->chan_cfg[i].outcfg.out_config, p_area->chan_cfg[i].out_peroid, p_area->chan_cfg[i].temp_data_len);        
    }
    tem_print_info("  ------- \t  ------ \t ------ \t -------\n");
    
    tem_print_info("\n");   
}

void tem_table_show(void)
{
    int i = 0;
    int j = 0;
    
    DRV_HKADC_DATA_AREA *p_area = (DRV_HKADC_DATA_AREA *)TEMPERATURE_VIRT_ADDR;
    
    for(i = 0;i < HKADC_CHAN_MAX;i++)
    {
        tem_print_info("channel number is %d\n", i);
        
        tem_print_info("  volt \t   tem  \n");
        tem_print_info("  ---- \t -------\n");

        for(j = 0;j < p_area->chan_cfg[i].temp_data_len;j++)
        {
            tem_print_info("%6d \t %6d\n", p_area->chan_cfg[i].temp_table[j].volt, p_area->chan_cfg[i].temp_table[j].temp);
        }
      
        tem_print_info("  ---- \t -------\n");
        
        tem_print_info("\n");
    }
    
}



void drv_hkadc_get_temp_debug(HKADC_TEMP_PROTECT_E mode)
{
    short psTemp = 0;
    unsigned short pusVolt = 0;
    
    if(TEMPERATURE_OK == drv_hkadc_get_temp(mode, &pusVolt, &psTemp, HKADC_CONV_DELAY))
    {
        tem_print_info("mode = %d volt = %d tem = %d.\n", mode, pusVolt, psTemp);
    }

}

void tem_output_show(void)
{
    int i = 0;
    
    DRV_HKADC_DATA_AREA *p_area = (DRV_HKADC_DATA_AREA *)TEMPERATURE_VIRT_ADDR;

    tem_print_info(" channel \t   volt  \t   tem  \n");
    tem_print_info(" ------- \t ------- \t -------\n");
    for(i = 0;i < HKADC_CHAN_MAX;i++)
    {
        tem_print_info("%6d \t %6d \t %6d\n", i, p_area->chan_out[i].volt_l,p_area->chan_out[i].temp_l);
    }
    tem_print_info(" ------- \t ------- \t -------\n");

    tem_print_info("\n");

    tem_print_info(" channel \t reverse volt \t reverse tem\n");
    tem_print_info(" ------- \t ------------ \t -----------\n");
    for(i = 0;i < HKADC_CHAN_MAX;i++)
    {
        tem_print_info("%6d \t %6d \t %6d \n", i, p_area->chan_out[i].volt_r,p_area->chan_out[i].temp_r);

    }    
    tem_print_info(" ------- \t ------------ \t -----------\n");

}



