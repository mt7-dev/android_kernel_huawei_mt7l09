/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : pmu_hi6561_test.c */
/* Version       : 2.0 */
/* Created       : 2013-03-14*/
/* Last Modified : */
/* Description   :  The C union definition file for the module LTE_PMU*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/

#include "bsp_pmu_hi6561.h"
#include "bsp_mipi.h"
#include "bsp_pmu.h"
#include "pmu_hi6561.h"
#include <pmu_balong.h>


extern HI6561_VLTGS_ATTR hi6561_volt_attr[];
int test_hi6561_power_on(void)
{
	int power_id = 0;
	int chip_id = 0;
	u8 status = 0;
	int ret = 0;
	for(chip_id= 0; chip_id<HI6561_BUTT;chip_id++){
		for(power_id = PMU_HI6561_LDO1 ; power_id < PMU_HI6561_POWER_ID_BUTT ; power_id ++)
		{
			ret |=pmu_hi6561_power_on(power_id,chip_id);
			ret |=pmu_hi6561_power_status(power_id,&status,chip_id);
			if(MIPI_OK!=ret){
				mipi_print_error("pmu hi6561 power on or get status fail!\n");
				return MIPI_ERROR;
			}
			if(0 == status)
			{
			    mipi_print_error("\n power id [%d]: enable or is_enabled failed!\n", power_id);
			    return power_id;
			}
		}
		mipi_print_error("hi6561_volt_enable_test ok!!\n");
	}

	return MIPI_OK;
}


int test_hi6561_power_off(void)
{
	int power_id = 0;
	int chip_id = 0;
	u8 status = 0;
	int ret = 0;
	for(chip_id= 0; chip_id<HI6561_BUTT;chip_id++){
		for(power_id = PMU_HI6561_LDO1 ; power_id < PMU_HI6561_POWER_ID_BUTT ; power_id ++)
		{
			ret |=pmu_hi6561_power_off(power_id,chip_id);
			ret |=pmu_hi6561_power_status(power_id,&status,chip_id);
			if(MIPI_OK!=ret){
				mipi_print_error("pmu hi6561 power off or get status fail!\n");
				return MIPI_ERROR;
			}
			if(0 != status)
			{
				mipi_print_error("\n power id [%d]: enable or is_enabled failed!\n", power_id);
				return power_id;
			}
		}
		mipi_print_error("hi6561_volt_enable_test ok!!\n");
	}

	return MIPI_OK;
}


int test_hi6561_voltage_get_set(void)
{
	HI6561_POWER_ID power_id = 0;
	HI6561_ID_ENUM chip_id = 0;
	u32 vol_readback=0;
	u32 size = 0;
	int vol_num =0;
	int retval = 0;
	u16 *vol_list=NULL;
    u16 voltage =0;

	vol_list = (u16 *)malloc(VOLTAGEMAX*sizeof(u16));
	if(vol_list == NULL){
		mipi_print_error("malloc vol_list error!\n");
		return MIPI_OK;
	}
	for(chip_id= 0; chip_id<HI6561_BUTT;chip_id++){
		for(power_id = PMU_HI6561_LDO1 ; power_id < PMU_HI6561_POWER_ID_BUTT ; power_id ++)
		{
			if(hi6561_volt_attr[power_id].voltage_list != NULL)
			{
				for(vol_num = 0; vol_num < VOLTAGEMAX;vol_num++)
				{
					voltage = hi6561_volt_attr[power_id].voltage_list[vol_num];
					retval=pmu_hi6561_voltage_set(power_id,voltage,chip_id);
					if(MIPI_OK!=retval){
						mipi_print_error("pastar voltage set failed!\n");
					}
					retval = pmu_hi6561_voltage_get(power_id, &vol_readback,chip_id);
					if(voltage != (u16)vol_readback )
					{
						mipi_print_error("id %d set or get voltage error!!\n",power_id);
						return power_id;
					}
					retval= pmu_hi6561_voltage_list_get(power_id,&vol_list,&size);
					if(MIPI_OK!=retval){
						mipi_print_error("id %d get voltage list error!\n",power_id);
						return power_id;
					}
					if(vol_readback!= (u32)vol_list[vol_num])
					{
						mipi_print_error("id %d set or list voltage error!!\n",power_id);
						return power_id;
					}
				}
			}
			else
			{
				mipi_print_error("hi6561_volt_enable_test ok!!\n");
				return power_id;
			}
		}
		mipi_print_error("hi6561_volt_enable_test ok!!\n");
	}

	free(vol_list);
	return MIPI_OK;
}


int bsp_hi6561_volt_onoff_test(u32 test_times)
{
    int fail_times[2] = {0,0};
    u32 test_counts = 0;
    int result = MIPI_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = test_hi6561_power_on();
        if(MIPI_OK != result)
        {
            mipi_print_error("hi6451_volt_enable_test failed!!\n");
            fail_times[0]++;
        }
        result = test_hi6561_power_off();
        if(MIPI_OK != result)
        {
            mipi_print_error("hi6451_volt_disable_test failed!!\n");
            fail_times[1]++;
        }
    }

    mipi_print_error("test %d times ,enable %d times failed,disable %d times failed!!\n",test_times,fail_times[0],fail_times[1]);

    if((0 != fail_times[0])||(0 != fail_times[1]))
        return MIPI_ERROR;
    else
        return MIPI_OK;

}

int bsp_hi6561_volt_setget_test(u32 test_times)
{
    int fail_times = 0;
    u32 test_counts = 0;
    int result = MIPI_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = test_hi6561_voltage_get_set();
        if(BSP_PMU_OK != result)
        {
            mipi_print_error("hi6451_volt_set_get_voltage_test failed!!\n");
            fail_times++;
        }
    }

    mipi_print_error("test %d times ,%d times failed!!\n",test_times,fail_times);

    if(0 != fail_times)
        return MIPI_ERROR;
    else
        return MIPI_OK;

}

/*将测试函数注册进测试框架*/
/*****************************************************************************
 函 数 名  : bsp_hi6561_test_init
 功能描述  : hi6561 test模块的初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
void bsp_hi6561_test_init(void)
{
    int iret = MIPI_OK;

    struct pmu_test_ops hi6561_test_ops = {
        .pmu_volt_onoff_test = bsp_hi6561_volt_onoff_test,
        .pmu_volt_setget_test = bsp_hi6561_volt_setget_test,
    };

    iret = bsp_pmu_test_register(PMIC_HI6561,hi6561_test_ops);

    if(BSP_PMU_OK != iret)
        mipi_print_error("bsp_pmu_test_register error!\n");
    else
        mipi_print_error("bsp_hi6551_test_init ok!\n");
}

