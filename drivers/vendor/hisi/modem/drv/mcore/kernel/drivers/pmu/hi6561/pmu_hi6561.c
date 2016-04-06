/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : pmu_hi6561.c */
/* Version       : 2.0 */
/* Created       : 2013-06-20*/
/* Last Modified : */
/* Description   :  The C union definition file for the module LTE_PMU*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
#include <osl_common.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_om.h>
#include <bsp_hardtimer.h>
#include <bsp_nvim.h>
#include <hisi_lpm3.h>
#include <pmu_hi6561.h>

#if 0
/*hi6561 ctrl param*/
typedef struct
{
	u8 onoff_reg_addr;
	u8 onoff_bit_offset;
}HI6561_VLTGS_ATTR;

/*the volt order should be same with "HI6561_POWER_ID"*/
static HI6561_VLTGS_ATTR hi6561_volt_attr[PMU_HI6561_POWER_ID_BUTT]={
	{HI6561_ONOFF2_OFFSET,HI6561_EN_LDO1_INIT_BIT },
	{HI6561_ONOFF2_OFFSET,HI6561_EN_LDO2_INIT_BIT },
	{HI6561_ONOFF1_OFFSET,HI6561_EN_BUCK0_INIT_BIT},
	{HI6561_ONOFF1_OFFSET,HI6561_EN_BUCK1_INIT_BIT},
	{HI6561_ONOFF1_OFFSET,HI6561_EN_BUCK2_INIT_BIT}
};
static int pmu_hi6561_reg_read(u8 reg_addr,u8 *data,HI6561_ID_ENUM chip_id)
{
	u8 reg_data=0xff;
	int ret;
	if(!data){
		mipi_print_error("Error:param pointer is null!\n");
		return (int)MIPI_ERROR;
	}
	ret=bsp_mipi_data_rev(reg_addr, &reg_data,(MIPI_CTRL_ENUM)chip_id);
	if(ret<0)
	{
		mipi_print_error("Error: pmu_hi6561_reg_read fail!\n");
	}
	*data=reg_data;
	return ret;
}


static int pmu_hi6561_reg_write(u8 reg_addr,u8 reg_data,HI6561_ID_ENUM chip_id)
{
	int ret;
	ret=bsp_mipi_data_send(reg_addr, reg_data,(MIPI_CTRL_ENUM)chip_id);
	if(ret<0)
	{
		mipi_print_error("Error: pmu_hi6561_reg_read fail!\n");
		return MIPI_ERROR;
	}
	return MIPI_OK;
}

static int pmu_hi6561_power_switch( HI6561_POWER_ID power_id,POWER_PROC_ENUM power_proc,HI6561_ID_ENUM chip_id )
{
	int ret=MIPI_OK;
	u8 reg_val=0;
	HI6561_VLTGS_ATTR hi6561_volt={0,0};

	/*judge the paramemt is invalid or not*/
	if(power_id>PMU_HI6561_BUCK2||power_id<PMU_HI6561_LDO1)
	{
		mipi_print_error("Error:power id is invalid!\n");
		ret=MIPI_ERROR;
	}

	if(power_id<PMU_HI6561_POWER_ID_BUTT)hi6561_volt = hi6561_volt_attr[power_id];/*for pc-Lint*/

	ret=pmu_hi6561_reg_read(hi6561_volt.onoff_reg_addr, &reg_val, chip_id);
	if(MIPI_OK!=ret)
	{
		mipi_print_error("Error:can't get pastar register value !\n");
		goto out;
	}

	/*if open*/
	if(PA_STAR_POWER_ON==power_proc){
		reg_val |= PA_STAR_POWER_ON<<hi6561_volt.onoff_bit_offset;
	}
	else{/*close*/
	    reg_val &= ~(u8)(1<<hi6561_volt.onoff_bit_offset);
	}

	ret=pmu_hi6561_reg_write(hi6561_volt.onoff_reg_addr,reg_val,chip_id);
	if(MIPI_OK!=ret)
	{
		mipi_print_error("Error:can't write data to pastar!\n");
	}

out:
	return ret;
}

static int pmu_hi6561_power_off_local(HI6561_POWER_ID power_id,HI6561_ID_ENUM chip_id )
{
	int ret=MIPI_OK;
	/*judge the paramemt is invalid or not*/
	if(power_id>PMU_HI6561_BUCK2||power_id<PMU_HI6561_LDO1)
	{
		mipi_print_error("Error:power id is invalid!\n");
		ret=MIPI_ERROR;
	}
	#if 0
	if(power_id>PMU_HI6561_LDO2&&power_id<PMU_HI6561_POWER_ID_BUTT){
		if(!strcmp(pmu_hi6561_exc_isr(chip_id),err_list[power_id])){
			over_flow_tag[chip_id][power_id]=PMU_HI6561_OVER_FLOW;/*需要和zhangliping确认，标志位如何让上层知道*/
		}
	}
	#endif
	ret=pmu_hi6561_power_switch(power_id, PA_STAR_POWER_OFF,chip_id);
	if(MIPI_OK!=ret){		
		mipi_print_error("Error:close buck failed!\n");
		ret=MIPI_ERROR;
	}
	#if 0
	ret=pmu_hi6561_exc_clear((EXCEPTION_TYPE_E)power_id,chip_id);
	if(MIPI_OK!=ret){		
		mipi_print_error("Error:close buck failed!\n");
		ret=MIPI_ERROR;
	}
	#endif
	return ret;
}


static int pmu_hi6561_power_on_local(HI6561_POWER_ID power_id ,HI6561_ID_ENUM chip_id)
{
	int ret=MIPI_OK;

	/*judge the paramemt is invalid or not*/
	if(power_id>PMU_HI6561_BUCK2||power_id<PMU_HI6561_LDO1)
	{
		mipi_print_error("Error:power id is invalid!\n");
		ret=MIPI_ERROR;
	}
#if 0
	/*查看过流标志位，是否可以打开电源*/
	if(power_id>PMU_HI6561_LDO2&&power_id<PMU_HI6561_POWER_ID_BUTT){
		if(over_flow_tag[chip_id][power_id]==PMU_HI6561_OVER_FLOW){
			mipi_print_error("Error:pastar[%d] power id [%d]has over flow!\n",chip_id,power_id);
			return MIPI_ERROR;
		}
	}
	/*清除状态寄存器*/	
	ret=pmu_hi6561_exc_clear((EXCEPTION_TYPE_E)power_id,chip_id);	
	if(MIPI_OK!=ret){		
		mipi_print_error("Error:clear status register failed!\n");
		ret=MIPI_ERROR;
	}
	
#endif
	ret=pmu_hi6561_power_switch(power_id, PA_STAR_POWER_ON,chip_id);
	return ret;

}
int pmu_hi6561_power_off(HI6561_POWER_ID power_id ,HI6561_ID_ENUM chip_id)
{
	int ret=MIPI_OK;
	unsigned long flags = 0;

	/*write after read,add lock */
	local_irq_save(flags);

	ret=pmu_hi6561_power_off_local(power_id,chip_id);
	if(MIPI_OK!=ret){		
		mipi_print_error("Error:close power failed!\n");
		ret=MIPI_ERROR;
	}

	local_irq_restore(flags);

	return ret;
}
int pmu_hi6561_power_on(HI6561_POWER_ID power_id ,HI6561_ID_ENUM chip_id)
{
	int ret=MIPI_OK;
	unsigned long flags=0;

	/*write after read,add lock */
	local_irq_save(flags);

	ret=pmu_hi6561_power_on_local(power_id,chip_id);
	if(MIPI_OK!=ret){		
		mipi_print_error("Error:close power failed!\n");
		ret=MIPI_ERROR;
	}

	local_irq_restore(flags);
	return ret;
}
#endif

__ao_data u32 g_vio_always_on = 0;
/*此函数用于使能pastar */
void pastar_resume_early(void)
{
	u32 *debug_mask_pastar = (u32 *)SRAM_PASTAR_DPM_INFO + PASTAR_DPM_SUSPEND_MASK_OFFSET;
	u32 *time_stamp = (u32 *)(SRAM_PASTAR_DPM_INFO + PASTAR_DPM_EN_TIMESTAMP_OFFSET);

	/*判断pastar是否关闭过smart star LVS5*/
	if(*debug_mask_pastar & 0x1){
        if(!g_vio_always_on){
		LSW_ENABLE(LSW53);
        }
		
		/*记录smart star LVS5使能的时刻点*/
		*time_stamp = bsp_get_slice_value();
	}
	return;
}

void pastar_suspend_late(void)
{
    u32 *ps_switch = (u32 *)(SRAM_PASTAR_DPM_INFO + PASTAR_DPM_SWITCH_OFFSET);
	if(!(*ps_switch)){        
            /* 根据nv36确定是否要关闭.产线版本不关，防止漏电；正式版本要关 */
		if(!g_vio_always_on){
    		LSW_DISABLE(LSW53);
		}
	}

	return;
}

void bsp_pastar_init(void)
{
    int ret = 0;
    if(get_modem_init_flag() == MODEM_ALREADY_INIT_MAGIC){
        return ;
	}
	ret |= (int)bsp_nvm_read((u32)NV_ID_DRV_FEM_VIO_ALWAYS_ON,(u8 *)&g_vio_always_on,(u32)sizeof(DRV_FEM_VIO_ALWAYS_ON));
	if(NV_OK != ret){
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_MIPI, "pastar init fail\r\n");
	}
	return ;
}
