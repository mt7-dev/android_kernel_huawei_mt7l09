
#include <product_config.h>

#ifdef MODEM_MEM_REPAIR

#include <bsp_memmap.h>
#include <hi_base.h>
#include <hi_ap_sctrl.h>
#include <hi_syscrg.h>
#include <hi_syssc.h>

#include <bsp_om.h>
#include <bsp_shared_ddr.h>
#include <bsp_hardtimer.h>
#endif
#include <bsp_memrepair.h>

#ifdef MODEM_MEM_REPAIR

#define  mr_print_error(fmt, ...)   (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_MEMREPAIR, "[memrepair]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  mr_print_info(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_MEMREPAIR, "[memrepair]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

/*sram中申请了0x20个字节*/
static unsigned int *mr_poweron_flag[MODEM_MEMREPAIR_BUTT] = {
	[MODEM_MEMREPAIR_DSP] =(unsigned int *)(SHM_MEM_MEMREPAIR_ADDR + 0),
	[MODEM_MEMREPAIR_IRM] =(unsigned int *)(SHM_MEM_MEMREPAIR_ADDR + MODEM_MEMREPAIR_IRM*4),
};

int bsp_modem_is_need_memrepair(void)
{
    int ret = 0;
    ret = (int)(get_hi_ap_sctrl_scbakdata14_bakdata() & 0x01UL);
    return ret;
}

int bsp_modem_memrepair(modem_memrepair_ip module)
{
	int ret = 0;
	unsigned int timestamp;
	unsigned int new_slice;
	unsigned int bus_stat;
	unsigned int bit;

    if (!bsp_modem_is_need_memrepair())
        return 0;

	/*3.	配置SYSCTRL中的SCPERCTRL3(0x20c) bit 3 bit4为0*/
	set_hi_ap_sctrl_scperctrl3_repair_frm_sram(0);
	set_hi_ap_sctrl_scperctrl3_repair_frm_efu(0);

	switch(module){
		case MODEM_MEMREPAIR_DSP:
			bit = MODEM_MEMREPAIR_DSP_BIT;
			break;
		case MODEM_MEMREPAIR_IRM:
			bit = MODEM_MEMREPAIR_IRM_BIT;
			break;
		default:
			ret = -1;
			goto out;
	}

	/*4.	使能待修复模块，配置SYSCTRL中的SCPERCTRL2(0x208)bit14为0，
	其他bit为1,[31:20] bit 保持不变*/
	set_hi_ap_sctrl_scperctrl2_repair_disable(~(1<<bit)&0xfffff);
	udelay(1);
	/*5.若首次上电，则配置SYSCTRL中的SCPERCTRL3 (0x20C) bit4为1,指示修复数据来源EFUSE*/
	if(*mr_poweron_flag[module] == MR_POWERON_FLAG){
		set_hi_ap_sctrl_scperctrl3_repair_frm_efu(1);
		*mr_poweron_flag[module] = 0;
	}
	else if(*mr_poweron_flag[module] == 0){
	/*6.非首次上电，则配置SYSCTRL中的SCPERCTRL3 (0x20C)bit3为1,指示修复数据来源efuse*/
		set_hi_ap_sctrl_scperctrl3_repair_frm_sram(1);
	}
	else{/*内存被踩*/
		mr_print_error("shared ddr has trampled!module id is %d\n",module);/*内存被踩，后面流程还要继续走吗?*/
	}

	/*7.软件等待15us */
	udelay(20);

	/*8/9/10查询SYSCTRL中的SCMRBBUSYSTAT（0x304）bit14，如果其为0，打印修复完成*/
	timestamp = bsp_get_slice_value();
	do{
		bus_stat = get_hi_ap_sctrl_scmrbbusystat_mrb_efuse_busy() & (1<<bit);/*304*/
		new_slice = bsp_get_slice_value();
	}while((bus_stat != 0)&&(get_timer_slice_delta(timestamp, new_slice) < MEMRAIR_WAIT_TIMEOUT));

	if(bus_stat != 0){
		mr_print_error("wait memrepair efuse busy bit timeout,wait slice is %x\n",(new_slice - timestamp));
		/* coverity[no_escape] */
		for(;;) ;
	}

	/*11. 配置SYSCTRL中的SCPERCTRL3(0x20c)bit 3 bit4为0*/
	set_hi_ap_sctrl_scperctrl3_repair_frm_sram(0);/*bit 3*/
	set_hi_ap_sctrl_scperctrl3_repair_frm_efu(0);/*bit 4*/

out:
	return ret;
}

int bsp_get_memrepair_time(void)
{
    if (bsp_modem_is_need_memrepair())
        return 900; /* us */
    else
        return 0;
}

#else

int bsp_modem_memrepair(modem_memrepair_ip module){
	return 0;
}

int bsp_get_memrepair_time(void)
{
    return 0;
}
#endif

