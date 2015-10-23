/*lint -save -e537*/
#include <osl_common.h>
#include <osl_bio.h>

#include "bsp_nvim.h"
#include "drv_nv_id.h"
#include "drv_nv_def.h"

#include "bsp_om.h"
#include "bsp_memmap.h"
#include "bsp_hardtimer.h"

#include "hi_base.h"
#include "hi_syssc.h"
#include "hi_tsensor.h"

#include "tsensor_balong.h"
/*lint -restore */

#if defined(__FASTBOOT__)
#define bsp_trace(log_level, mod_id, fmt,...) cprintf(fmt, ##__VA_ARGS__)
#endif

#define  tsens_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_TSENSOR, "[tsensor]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  tsens_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_TSENSOR, "[tsensor]: "fmt, ##__VA_ARGS__))

static int is_tsensor_init = 0;

#ifndef __CMSIS_RTOS
DRV_TSENSOR_TRIM_STRU trim_array;
DRV_TSENS_TEMP_TABLE table_array;
#endif

#ifdef HI_TSENS_THRSMAX0_OFFSET
/*Tsensor初始化设置*/
int tsensor_init()
{
    if(1 == is_tsensor_init)
    {
        return TSENSOR_OK;
    }
    
    set_hi_tsens_en_freq(0x3);/*设置时间单位为96ms*/
    set_hi_tsens_cnt0_tsensor0_measure_peri(0xA);/*tsensor0温度检测周期960ms*/
    set_hi_tsens_cnt1_tsensor1_measure_peri(0xA);/*tsensor1温度检测周期960ms*/
    set_hi_tsens_cnt2_tsensor2_measure_peri(0xA);/*tsensor2温度检测周期960ms*/

    set_hi_tsens_intclr0_tsensor0_intmin_clr(0x1);/*tsensor0 中断清除*/
    set_hi_tsens_intclr0_tsensor0_intlow_clr(0x1);
    set_hi_tsens_intclr0_tsensor0_intupp_clr(0x1);
    set_hi_tsens_intclr0_tsensor0_intmax_clr(0x1);
    set_hi_tsens_intclr0_tsensor0_intchg_clr(0x1);

    set_hi_tsens_intclr1_tsensor1_intmin_clr(0x1);/*tsensor1 中断清除*/
    set_hi_tsens_intclr1_tsensor1_intlow_clr(0x1);
    set_hi_tsens_intclr1_tsensor1_intupp_clr(0x1);
    set_hi_tsens_intclr1_tsensor1_intmax_clr(0x1);
    set_hi_tsens_intclr1_tsensor1_intchg_clr(0x1);

    set_hi_tsens_intclr2_tsensor2_intmin_clr(0x1);/*tsensor2 中断清除*/
    set_hi_tsens_intclr2_tsensor2_intlow_clr(0x1);
    set_hi_tsens_intclr2_tsensor2_intupp_clr(0x1);
    set_hi_tsens_intclr2_tsensor2_intmax_clr(0x1);
    set_hi_tsens_intclr2_tsensor2_intchg_clr(0x1);

    set_hi_tsens_intmask0_tsensor0_intmin_mask(0x1);/*tsensor0 中断屏蔽*/
    set_hi_tsens_intmask0_tsensor0_intlow_mask(0x1);
    set_hi_tsens_intmask0_tsensor0_intupp_mask(0x1);
    set_hi_tsens_intmask0_tsensor0_intmax_mask(0x1);
    set_hi_tsens_intmask0_tsensor0_intchg_mask(0x1);

    set_hi_tsens_intmask1_tsensor1_intmin_mask(0x1);/*tsensor1 中断屏蔽*/
    set_hi_tsens_intmask1_tsensor1_intlow_mask(0x1);
    set_hi_tsens_intmask1_tsensor1_intupp_mask(0x1);
    set_hi_tsens_intmask1_tsensor1_intmax_mask(0x1);
    set_hi_tsens_intmask1_tsensor1_intchg_mask(0x1);

    set_hi_tsens_intmask2_tsensor2_intmin_mask(0x1);/*tsensor2 中断屏蔽*/
    set_hi_tsens_intmask2_tsensor2_intlow_mask(0x1);
    set_hi_tsens_intmask2_tsensor2_intupp_mask(0x1);
    set_hi_tsens_intmask2_tsensor2_intmax_mask(0x1);
    set_hi_tsens_intmask2_tsensor2_intchg_mask(0x1);

    set_hi_tsens_en_tsensor0_en(0x1);/*tsensor0工作使能*/
    set_hi_tsens_en_tsensor1_en(0x1);/*tsensor1工作使能*/
    set_hi_tsens_en_tsensor2_en(0x1);/*tsensor2工作使能*/

    is_tsensor_init = 1;
    
	tsens_print_info("tsensor init is ok!\n");

    return TSENSOR_OK;
    
}

/*获取芯片温度码*/
int tsens_tem_get(TSENSOR_REGION region)
{
    int temperature = 0;

    if(0 == is_tsensor_init)
    {
        tsens_print_error("tsensor is not inited!\n");
        return TSENSOR_ERROR;
    }
    
    switch(region)
    {
        case A9_REGION:
            temperature = get_hi_tsens_stat0_tsensor0_last_temp();
            break;
        case TSENS_REGION:
            temperature = get_hi_tsens_stat1_tsensor1_last_temp();
            break;
        case DDR_REGION:
            temperature = get_hi_tsens_stat2_tsensor2_last_temp();
            break;
        default:
            tsens_print_error(" tsensor region%d is error.\n", (int)region);
            return TSENSOR_ERROR;
    }

    return temperature;
}

#else

int tsensor_init()
{
#ifndef __CMSIS_RTOS
    unsigned int ret = 0;
#endif

    if(1 == is_tsensor_init)
    {
        return TSENSOR_OK;
    }

#ifndef __CMSIS_RTOS
    if(0 != (ret = bsp_nvm_read(NV_ID_DRV_TSENS_TABLE, (u8*)(&table_array), sizeof(DRV_TSENS_TEMP_TABLE))))
    {
        tsens_print_error("nv =0x%x read fail, ret = 0x%x, i = %d.\n", NV_ID_DRV_TSENS_TABLE, ret);
        return TSENSOR_ERROR;
    }
    
    if(0 != (ret = bsp_nvm_read(NV_ID_DRV_TSENSOR_TRIM, (u8*)(&trim_array), sizeof(DRV_TSENSOR_TRIM_STRU))))
    {
        tsens_print_error("nv =0x%x read fail, ret = 0x%x, i = %d.\n", NV_ID_DRV_TSENSOR_TRIM, ret);
        return TSENSOR_ERROR;
    }
#endif

    set_hi_temp_config_sample_num(0x2);    /*同一通道采样的次数*/

    set_hi_temp_config_sample_mode(0x2);    /*多次采样采用均值*/

    set_hi_temp_config_test(0x0);           /*Tsensor通道选择local*/

    //set_hi_temp_config_ct_sel(0x01);        /*转换时间为6.144ms*/
    
    set_hi_temp_config_ct_sel(0x0);        /*转换时间为0.768ms*/
    
    set_hi_temp_dis_time_temp_dis_time(0x6);/*切换Tsensor时间间隔*/
    
    set_hi_temp_en_temp_en(0x1);    /*使能*/
    
    is_tsensor_init = 1;

    tsens_print_info("tsensor init ok!\n");
    
    return TSENSOR_OK;
}


int tsens_tem_get(TSENSOR_REGION region)
{
    int code = 0;
    
    switch(region)
    {
        case TSENS_REGION_0:
            set_hi_temp_config_test(0x0);
            break;
        case TSENS_REGION_1:
            set_hi_temp_config_test(0x1);
            break;
        case TSENS_REGION_2:
            set_hi_temp_config_test(0x2);
            break;
        default:
            tsens_print_error(" tsensor region%d is error.\n", (int)region);
            return TSENSOR_ERROR;
    }
    
    code =(int) get_hi_temp_temp();
    
    return code;
}

#ifndef __CMSIS_RTOS
int chip_tem_get()
{
    short main_tem = 0;
    short trim_tem = 0;
    
    short main_code = 0;
    short trim_code = 0;

    udelay(1500);
    
    main_code = get_hi_temp_temp();
    trim_code = get_hi_sc_stat86_efuse_tsensor_trim() & ((0x1 << 8) - 1);

    main_tem = table_array.temp[main_code];
    trim_tem = trim_array.tsensor_trim[trim_code];
    
    //cprintf("main_code %d trim_code %d main_tem %d trim_tem %d\n", main_code, trim_code, main_tem, trim_tem);
    
    return (main_tem + trim_tem);

}
#endif

#endif

