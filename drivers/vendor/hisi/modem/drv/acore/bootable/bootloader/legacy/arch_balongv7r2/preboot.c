/******************************************************************************
  File Name       : preboot.c
  Description     : power on process handler
  History         :
******************************************************************************/
/*lint --e{537} */
#include <hi_syssc_interface.h>
#include <boot/boardid.h>
#include <boot/boot.h>
#include <boot/flash.h>
#include <balongv7r2/types.h>
#include <balongv7r2/preboot.h>
#include <balongv7r2/time.h>
#include <balongv7r2/gpio.h>
#include <balongv7r2/pmu.h>
#include <balongv7r2/chg_boot.h>
#include <balongv7r2/version.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <soc_onchiprom.h>
#include <power_com.h>
#include <soc_memmap.h>
#include <bsp_nvim.h>
#include <bsp_dump.h>
#include <bsp_pmu.h>
#include <bsp_sram.h>
static u32 power_down_charge_flag = 0;
static u32 soft_version_flag = NORMAL_SW_TYPE;
static u32 power_on_mode_flag = BOOT_ON_NORMAL_MODE;
static u32 abnormal_reset_flag = NORMAL_RST_TYPE;

static boot_cmdword balong_map[] = {
	{"warmreset", POWER_REBOOT_CMD_WARMRESET},
	{"bootloader", POWER_REBOOT_CMD_BOOTLOADER},
	{"recovery", POWER_REBOOT_CMD_RECOVERY},
	{"resetfactory", POWER_REBOOT_CMD_FACTORY},
	{"update", POWER_REBOOT_CMD_UPDATE},
};

extern int boot_linux_from_flash(void);
extern int boot_recovery_from_flash(void);
extern int modify_recovery_count(void);
extern int boot_rtx_cm3_from_flash(void);
extern u32 balong_version_get_hw_version(void);


/*****************************************************************************
 函 数 名  : find_rebootmap
 功能描述  : write boot message to the misc.
             this message will be read in recovery.
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 u8* find_rebootmap(u32 num)
{
	u32 n = 0;
	u32 i;
	for (i = 0; i < sizeof(balong_map)/sizeof(boot_cmdword); i++)
    {
		if ((num) == balong_map[i].num) {
			pr_dbg("[fastboot] rebootmap = %s\n", balong_map[i].name);
			n = i;
			break;
		}
	}

    if((sizeof(balong_map)/sizeof(boot_cmdword)) == i)
        return NULL;

	return balong_map[n].name;
}

/*****************************************************************************
 函 数 名  : set_misc_message
 功能描述  : write boot message to the misc.
             this message will be read in recovery.
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 int set_misc_message(misc_message_type *mesg)
{
	struct ptentry *ptn;
	char* str_misc = PTABLE_MISC_NM;
	unsigned extra = 0;
	int ret;

	if(mesg == NULL)
		return MISC_ERROR;

	ptn = flash_find_ptn(str_misc);
	if (0 == ptn) {
		pr_dbg("can't find ptn misc\n");
		return MISC_ERROR;
	}

	ret = flash_write(ptn, extra, mesg, sizeof(misc_message_type));

	return (0 == ret) ? MISC_OK : MISC_ERROR;
}

/*****************************************************************************
 函 数 名  : get_misc_message
 功能描述  : read boot message.
             this message will be read in recovery.
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 int get_misc_message(misc_message_type *mesg)
{
	struct ptentry *ptn;
	char* str_misc = PTABLE_MISC_NM;
	u32 offset = 0;

	if (mesg == NULL)
		return MISC_ERROR;

	ptn = flash_find_ptn(str_misc);
	if (0 == ptn) {
		pr_dbg("can't find ptn misc\n");
		return MISC_ERROR;
	}

	if (flash_read(ptn, offset, mesg, sizeof(misc_message_type), NULL)) {
		pr_dbg("[get_misc_message] flash read failed \n");
		return MISC_ERROR;
	}

	return MISC_OK;
}

#if 0
/*****************************************************************************
 函 数 名  : get_misc_info
 功能描述  : 获取misc信息
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 int get_misc_info()
{
    misc_message_type misc_msg;

	if (MISC_ERROR == get_misc_message(&misc_msg))
		return REDA_MISC_ERROR;

	if (!strcmp(misc_msg.command, "CHECKPOINT")){
		return EXIST_MISC_INFO;
    }

    /* 暂时不完整 */

	return NO_MISC_INFO;
}
#endif

#ifdef FEATURE_E5_ONOFF
/*****************************************************************************
 函 数 名  : get_power_state
 功能描述  : 获取电源状态
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 int get_power_state(void)
{
	int ret = 0;

#ifndef FEATURE_E5_ONOFF
    return CHARGE_BATTERY_GOOD;
#endif
    /*lint -save -e527 -e746 */
    /* coverity[unreachable] */
	ret = (int)chg_battery_check_boot(); /* 非E5产品，直接返回CHARGE_BATTERY_GOOD */
    /*lint -restore */

	pr_dbg("[fastboot]: get_power_state %d\n", ret);

	return ret;
}

/*****************************************************************************
 函 数 名  : last_shutdown_reason_get
 功能描述  : 上次关机原因获取
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 power_off_reboot_flag last_shutdown_reason_get( void )
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    return (power_off_reboot_flag)(power_info->last_shut_reason);
}

/*****************************************************************************
 函 数 名  : last_shutdown_reason_set
 功能描述  : 上次关机原因设置
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 void last_shutdown_reason_set(power_off_reboot_flag value )
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    power_info->last_shut_reason = (u32)(value);
}

/*****************************************************************************
 函 数 名  : power_on_start_reason_set_fastboot
 功能描述  : 开机原因设置
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void power_on_start_reason_set_fastboot( power_on_start_reason enReason )
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    power_info->power_on_reason = (u32)(enReason);
}
#endif

/*****************************************************************************
 函 数 名  : power_on_c_status_set
 功能描述  : 清空C核状态
 输入参数  : power_on_c_status_set
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
static void power_on_c_status_set(c_power_st_e val)
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    power_info->c_power_state = (u32)val;
}

/*****************************************************************************
 函 数 名  : power_on_normal_status_set
 功能描述  : 设置开机原因
 输入参数  :
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
static a_kernel_st_e power_on_acore_state_get()
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    return (a_kernel_st_e)(power_info->a_power_state);
}

/*****************************************************************************
 函 数 名  : power_on_normal_status_set
 功能描述  : 设置开机原因
 输入参数  :
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
static void power_on_acore_state_set(a_kernel_st_e state)
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    power_info->a_power_state = state;
}

/*****************************************************************************
 函 数 名  : is_boot_warm_reset
 功能描述  : 判断charger是否在位
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
bool bsp_pmu_hreset_state_get(void);
LOCAL_1 bool is_boot_warm_reset(void)
{
    bool pmu_rst = 0;
#if 0
    power_info_s *power_info = (power_info_s *)SRAM_REBOOT_ADDR;
#endif

    pmu_rst = bsp_pmu_hreset_state_get();

    if (!pmu_rst) /* 冷启动，清零计数器 */
    {
        return 0;
    }
    else
    {
#if 0   /* for test, p531 pmu reset工作不正常，无法进入冷启动 */
        power_on_acore_state_set(POWER_NORMAL);
        power_info->wdg_rst_cnt = 1;
#endif
        return 1;
    }
}

#ifdef FEATURE_E5_ONOFF
/*****************************************************************************
 函 数 名  : is_boot_pmu_charge_in
 功能描述  : 判断charger是否在位
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 bool is_boot_pmu_charge_in(void)
{
    return bsp_pmu_usb_state_get();
}

/*****************************************************************************
 函 数 名  : is_current_version_factory
 功能描述  : 从NV中读取当前版本是否为工厂版本
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 bool is_boot_power_key_press(void)
{
    return bsp_pmu_key_state_get();
}
#endif

/*****************************************************************************
 函 数 名  : is_current_version_factory
 功能描述  : 从NV中读取当前版本是否为工厂版本
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 bool is_current_version_factory(void)
{
    u32 rt = 0;
    sw_type vtype = NORMAL_SW_TYPE;

    rt = bsp_nvm_read((u32)NV_ID_DRV_NV_FACTORY_INFO_I, (u8*)&vtype, sizeof(FACTORY_MODE_TYPE));
    if(NV_OK != rt){
        pr_dbg("[fastboot]: read nv failed err 0x%x. \n", rt);

        soft_version_flag = NORMAL_SW_TYPE;
    }else{
        soft_version_flag= vtype;
    }

    if(FACTORY_SW_TYPE == soft_version_flag)
        return 1;

    return 0;
}

/*****************************************************************************
 函 数 名  : boot_do_reboot
 功能描述  : 执行重启
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 void boot_do_reboot(void )
{
    pr_dbg( "\r\n boot_reboot. \r\n" );
    hi_syssc_pmu_reboot();

    /* coverity[no_escape] */
    for(;;) ;
}

#ifdef FEATURE_E5_ONOFF
/*****************************************************************************
 函 数 名  : boot_do_power_off
 功能描述  : 执行下电
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 void boot_do_power_off( void )
{
    pr_dbg( "\r\n boot power off. \r\n" );
    hi_syssc_pmu_hold_down();

    /* coverity[no_escape] */
    for(;;) ;
}

/*****************************************************************************
 函 数 名  : boot_power_off
 功能描述  : 根据充电器在位状态决定重启还是下电
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 void boot_power_off( void )
{
    if( is_boot_pmu_charge_in() )
    {
        boot_do_reboot();
    }
    else
    {
        boot_do_power_off();
    }

    /* enter dead loop, wait power off or restart by charge */
    pr_dbg( "\r\n power off : dead-loop.....\r\n" );

    /* coverity[no_escape] */
    for(;;) ;
}

/*****************************************************************************
 函 数 名  : boot_start_reason_detect
 功能描述  : 开机原因检测
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 power_on_start_reason boot_start_reason_detect(void)
{
    if( is_boot_warm_reset() )
    {
        pr_dbg((const char*)"\r\n boot by warm-reset\r\n" );
        return POWER_ON_START_REASON_WARM_RESET;
    }

	if(is_boot_pmu_charge_in())
	{
		pr_dbg((const char*)"\r\n boot by charge\r\n" );
		return POWER_ON_START_REASON_CHARGE;
	}

    if(is_boot_power_key_press())
    {
        pr_dbg((const char*)"\r\n boot by power key\r\n" );
        return POWER_ON_START_REASON_POWER_KEY;
    }

    pr_dbg((const char*)"\r\n boot invalid condition and power off\r\n");
    boot_power_off(); /* Unexpect reason, power off it */

    return POWER_ON_START_REASON_POWER_KEY;
}

/*****************************************************************************
 函 数 名  : boot_power_key_wait
 功能描述  : 等待按键按够足够长时间
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 void boot_power_key_wait( u32 press_time )
{
    volatile u32 ul_start = 0;
             u32 ul_end   = 0;

    ul_start = get_timer_value();
    ul_end   = ul_start + press_time;

    while( ul_start < ul_end )
    {
		/* 按键开机期间不能松开按键 */
    	if(!is_boot_power_key_press())
		{
	        pr_dbg( "\r\n The power key is short, power off now\r\n" );
	        boot_power_off();
		}

        ul_start = get_timer_value();
    }
}

/*****************************************************************************
 函 数 名  : boot_powerkey_long_press_handler
 功能描述  : 长按键处理
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 void boot_powerkey_long_press_handler( u32 press_time )
{
    boot_power_key_wait(press_time);

    /* 超时时间到，等待松开按键，否则不允许开机 */
    while(is_boot_power_key_press()) ;
}

/*****************************************************************************
 函 数 名  : boot_over_temp_check_handler
 功能描述  : 坏电池处理
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void boot_show_battery_bad(void);
LOCAL_1 void boot_bad_battery_check_handler( void )
{
    power_off_reboot_flag  reboot_flag = last_shutdown_reason_get();
    int power_stat;

    if( POWER_OFF_REASON_BAD_BATTERY == reboot_flag )
    {
        pr_dbg( "\r\n power battery error handler\r\n" );
        last_shutdown_reason_set( POWER_OFF_REASON_INVALID ); /* Clear the flag before handle it */

        boot_show_battery_bad();

        for(;;)
        {
            power_stat = get_power_state();
            if(CHARGE_BATTERY_GOOD == power_stat){
                pr_dbg("battery resume normal\n");
                break;
            }

            else if(BATTERY_BAD_WITHOUT_CHARGE == power_stat){
                boot_power_off();
                break;
            }
        }
    }
}

/*****************************************************************************
 函 数 名  : boot_over_temp_check_handler
 功能描述  : fastboot过温处理
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void boot_show_battery_over_tmp(void);
LOCAL_1 void boot_over_temp_check_handler( void )
{
    power_off_reboot_flag  reboot_flag = last_shutdown_reason_get();

    if( POWER_OFF_REASON_OVER_TEMP == reboot_flag )
    {
        pr_dbg( "\r\n power battery error handler\r\n" );
        last_shutdown_reason_set( POWER_OFF_REASON_INVALID ); /* Clear the flag before handle it */

        boot_show_battery_over_tmp();

        for(;;)
        {
            if(CHARGE_BATTERY_GOOD == get_power_state()){
                pr_dbg("battery resume normal\n");
                break;
            }
        }
    }
}
#endif

/*****************************************************************************
 函 数 名  : boot_warm_up_handler
 功能描述  : 热启动处理，kernel->recovery->备份recovery
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 warm_up_e boot_warm_up_handler(void)
{
    power_info_s *power_info = (power_info_s *)SRAM_REBOOT_ADDR;
    u32 try_times = 0;
    u32 rt;
    /* warm start count +1 */
    power_info->wdg_rst_cnt++;

    /* pr_dbg("SRAM_REBOOT_ADDR 0x%x\n", SRAM_REBOOT_ADDR); */

    rt = bsp_nvm_read((u32)NV_ID_DRV_BOOT_TRY_TIMES, (u8*)&try_times, sizeof(BOOT_TRY_TIMES_STRU));
    if(NV_OK != rt)
    {
        pr_dbg("read nv failed err:%d .\n", rt);
        try_times = STARTUP_TRY_TIMES; /* 读失败，使用默认值 */
    }

    if(try_times < power_info->wdg_rst_cnt)
    {
        if(POWER_NORMAL == power_on_acore_state_get())
        {
            /* normal 模式启动失败 */
            pr_dbg("kernel load failed, try to start up from recovery\r\n");
            power_info->wdg_rst_cnt = 1;
            power_on_acore_state_set(POWER_RECOVERY_A);     /* 状态切换到recovery-a */
            return WARM_RECOVERY;
        }
        else if(POWER_RECOVERY_A == power_on_acore_state_get())
        {
            /* recovery-a 模式启动失败, 尝试备份区 */
            pr_dbg("recovery-a load failed, try to start up from recovery-b\r\n");
            power_info->wdg_rst_cnt = 1;

            if(modify_recovery_count() < 0)
            {
        		pr_dbg("modify recovery count failed!\n");
            }

            power_on_acore_state_set(POWER_RECOVERY_B);     /* 状态切换到recovery-b */
            return WARM_RECOVERY;
        }
        else if(POWER_RECOVERY_B == power_on_acore_state_get())
        {
            /* recovery a/b分区都启动失败 */
            pr_dbg("recovery-a and b load failed\r\n");
            power_info->wdg_rst_cnt = 1;

            /* 设备usb自举标志 ,重启进入USB自举模式，进行先贴后烧 */
            writel(AUTO_ENUM_FLAG_VALUE, OCR_AUTO_ENUM_FLAG_ADDR);
            boot_do_reboot();
        }
        else
        {
            power_on_acore_state_set(POWER_NORMAL);     /* 打桩 */
            pr_dbg("invalid status!\r\n");
        }

    }
    else
    {
        if(POWER_NORMAL != power_on_acore_state_get())
        {
            pr_dbg("warm up recovery image\r\n");
            return WARM_RECOVERY;
        }
    }

    return WARM_NORMAL;
}

/*****************************************************************************
 函 数 名  : is_boot_fastboot_key_press
 功能描述  : 判断Fastboot键是否按下
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 int is_boot_fastboot_key_press( void )
{
    volatile int gpio_value;

    /* read gpio value of fastboot key */
    gpio_value = gpio_get_value(BOOT_GPIO_FASTBOOT_ID);

    return (gpio_value) ? 0 : 1;
}

/*****************************************************************************
 函 数 名  : is_boot_usb_dump_enable
 功能描述  : 判断USB DUMP功能是否启用
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 int is_boot_usb_dump_enable(void)
{
    dump_nv_s dump_nv;
    int state;
    int ret = 0;
    dump_global_info_t * dump_info = (dump_global_info_t*)DDR_MNTN_ADDR_VIRT;
    u32 * dump_flag = (u32 *)SRAM_DUMP_POWER_OFF_FLAG_ADDR;
 
    if(BSP_OK != bsp_nvm_read(NV_ID_DRV_DUMP, (u8*)&dump_nv, sizeof(dump_nv_s)))
    {
        pr_dbg("read om dump nv 0x%x failed\n", NVID_DUMP);
    }
    else
    {
        if(1 == dump_nv.dump_cfg.Bits.dump_switch)
        {
            /* PMU异常，不启用usb dump */
            state = bsp_pmu_get_boot_state();
            if(0 != state)
            {
                pr_dbg("pmu error: 0x%x\n", state);
                dump_info->internal_info.app_internal.start_flag = DUMP_START_REBOOT;
                dump_info->internal_info.comm_internal.start_flag = DUMP_START_REBOOT;
                dump_info->internal_info.m3_internal.start_flag = DUMP_START_REBOOT;
                dump_info->reboot_reason = DUMP_REASON_PMU;
                return 0;
            }

			/* 单板掉电 */
			if(*dump_flag != DUMP_START_CRASH)
			{
                dump_info->internal_info.app_internal.start_flag = DUMP_START_POWER_ON;
                dump_info->internal_info.comm_internal.start_flag = DUMP_START_POWER_ON;
                dump_info->internal_info.m3_internal.start_flag = DUMP_START_POWER_ON;
				return 0;
			}
			
            /* 软件异常，reboot_reason已经处理 */
            if((DUMP_START_EXCH == dump_info->internal_info.app_internal.start_flag)    \
                || (DUMP_START_EXCH == dump_info->internal_info.comm_internal.start_flag) \
                || (DUMP_START_EXCH == dump_info->internal_info.m3_internal.start_flag))
            {
                dump_info->internal_info.app_internal.start_flag = DUMP_START_REBOOT;
                dump_info->internal_info.comm_internal.start_flag = DUMP_START_REBOOT;
                dump_info->internal_info.m3_internal.start_flag = DUMP_START_REBOOT;
                ret = 1;
            }
            /* CRASH或者UDP复位键 */
            else if((DUMP_START_CRASH == dump_info->internal_info.app_internal.start_flag) \
                || (DUMP_START_CRASH == dump_info->internal_info.comm_internal.start_flag) \
                || (DUMP_START_CRASH == dump_info->internal_info.m3_internal.start_flag))
            {
                dump_info->internal_info.app_internal.start_flag = DUMP_START_REBOOT;
                dump_info->internal_info.comm_internal.start_flag = DUMP_START_REBOOT;
                dump_info->internal_info.m3_internal.start_flag = DUMP_START_REBOOT;
                dump_info->reboot_reason = DUMP_REASON_UNDEF;
                ret = 1;
            }
            else
            {
                ret = 0;
            }
        }
    }

    return ret;
}

#ifdef FEATURE_E5_ONOFF
void boot_show_battery_low(void);
/*****************************************************************************
 函 数 名  : is_boot_wps_key_press
 功能描述  : 判断RESET键是否按下
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 int is_boot_reset_key_press( void )
{
    volatile int gpio_value;

    /* read gpio value of wps key */
    gpio_value = gpio_get_value(BOOT_GPIO_RESET_ID);

    return (gpio_value) ? 0 : 1;
}
#if 0
/*****************************************************************************
 函 数 名  : is_boot_force_update
 功能描述  : 判断是否进入强制升级
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 int is_boot_force_update( void )
{
    u32 ul_start           = 0;
    u32 ul_end             = 0;
    int  reset_status      = 1;
    int  reset_toggle_cnt  = 0;
    int  temp              = 0;

    if (!is_boot_reset_key_press())
    {
        return 0;
    }

    display_upgrade();

    ul_start = get_timer_value();
    ul_end   = ul_start - BOOT_FORCE_UPDATE_TIMEOUT;

    while (ul_start >= ul_end)
    {
        temp = is_boot_reset_key_press();
        if (reset_status != temp)
        {
            /* status changed, increase the count */
            reset_status = temp;
            reset_toggle_cnt++;
        }

        if (reset_toggle_cnt >= 2)
        {
            /* usr has release / press the wps within 500ms, try Update */
            pr_dbg( "\r\n check force-update!\r\n" );
            return 1;
        }

        ul_start = get_timer_value();
    }
    pr_dbg( "\r\n check force-update FAIL!\r\n" );
    return 0;
}
#endif
#endif

/*****************************************************************************
 函 数 名  : get_power_on_mode
 功能描述  : 开机模式判断
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
LOCAL_1 boot_on_mode get_power_on_mode()
{
	misc_message_type misc_message;
    power_info_s *power_info = (power_info_s *)SRAM_REBOOT_ADDR;
    power_reboot_cmd reboot_cmd = POWER_REBOOT_CMD_BUTT;
    char misc_str[32] = {0};
    char *prb = NULL;
    u32 board_type;

#ifdef FEATURE_E5_ONOFF
    u32 key_time_thres = BOOT_POWER_KEY_TIMEOUT;
    chg_batt_check_type batt_stat = CHARGE_BATTERY_GOOD;
    power_on_start_reason en_boot_reason = POWER_ON_START_REASON_BUTT;
#endif

    /* read and clear reboot cmd */
    reboot_cmd = (power_reboot_cmd)(power_info->reboot_cmd);

    power_info->reboot_cmd = (u32)POWER_REBOOT_CMD_BUTT;

    board_type = balong_version_get_hw_version();
    if ((HW_VER_PRODUCT_UDP == board_type) || (HW_VER_PRODUCT_E5379 == board_type))
    {
        if (is_boot_fastboot_key_press())
        {
            pr_dbg("fastboot key is pressed\n");
            return BOOT_ON_FASTBOOT_MODE;
        }
    }

    if (is_boot_usb_dump_enable())
    {
        pr_dbg("usb dump is enabled\n");
        return BOOT_ON_FASTBOOT_MODE;
    }

    prb = (char *)find_rebootmap((u32)reboot_cmd);

    /* 软件预置重启, 进入fastboot模式 */
	if ((NULL != prb) && (0 == strcmp((const char *)prb, "bootloader")))
    {
        pr_dbg("power on cmd: bootloader\n");
        return BOOT_ON_FASTBOOT_MODE;
    }
	else if ((NULL != prb) && (!strcmp((const char *)prb, "recovery") || \
        !strcmp((const char *)prb, "resetfactory")))
    {
        pr_dbg("power on cmd: recovery\n");
        strcpy(misc_str, "BOOT:RECOVERY");
        goto boot_from_recovery;
    }

    /* 热启动 */
    if(is_boot_warm_reset())
    {
        if(WARM_RECOVERY == boot_warm_up_handler())
        {
            strcpy(misc_str, "BOOT:FAIL");
            goto boot_from_recovery;
        }
    }
    else    /* 冷启动设置成normal状态 */
    {
        power_info->wdg_rst_cnt = 0;
        power_on_acore_state_set(POWER_NORMAL);
    }

    memset(&misc_message, 0, sizeof(misc_message_type));
    /* 升级版本进入recovery模式,fastboot中只处理"UPDATE"命令 */

    if(MISC_OK == get_misc_message(&misc_message))
    {
        if(!strcmp(misc_message.command, "UPDATE")){
            strcpy(misc_str, misc_message.command);
            /* 升级情况下，跳过不用判断normal模式,升级完成后设置回normal模式 */
            power_on_acore_state_set(POWER_RECOVERY_A);
            goto boot_from_recovery;
        }
        else{
            /* clear misc message */
            memset(&misc_message, 0, sizeof(misc_message_type));
            set_misc_message(&misc_message);
        }
    }
    else
    {
        pr_dbg("[fastboot]:get_misc_message failed.\n");
    }

    /* 工厂版本跳过下面所有操作 */
    if(is_current_version_factory())
    {
        return BOOT_ON_NORMAL_MODE;
    }

#ifdef FEATURE_E5_ONOFF
    en_boot_reason = boot_start_reason_detect();

    /* 设置开机原因 */
    power_on_start_reason_set_fastboot(en_boot_reason);

    if( POWER_ON_START_REASON_POWER_KEY != en_boot_reason)
    {
        /* start by warm reset ,normal boot mode */
        display_logo();
    }

    /* 坏电池或者过温关机时，只在USB在位时才置关机重启原因 */
    boot_bad_battery_check_handler();
    boot_over_temp_check_handler();
    last_shutdown_reason_set( POWER_OFF_REASON_INVALID );

    if(POWER_ON_START_REASON_POWER_KEY == en_boot_reason)
    {
        /* start by power key */
        if(NV_OK != bsp_nvm_read((u32)NV_ID_DRV_POWER_KEY_TIME, (u8*)&key_time_thres, sizeof(POWER_KEY_TIME_STRU)))
        {
            pr_dbg("[fastboot]:read nv failed %d .\n", key_time_thres);
            key_time_thres = BOOT_POWER_KEY_TIMEOUT;
        }

        boot_powerkey_long_press_handler(key_time_thres);

        if (is_boot_reset_key_press())
        {
            pr_dbg("the reset key is pressed\n");
            mdelay(2000);
            if (is_boot_reset_key_press())
            {
                pr_dbg("check force update ok\n");
                strcpy(misc_str, "UPDATE:FORCE");
                goto boot_from_recovery;
            }
            return BOOT_ON_FASTBOOT_MODE;
        }

        display_logo();
    }

    batt_stat = (chg_batt_check_type)get_power_state();

    if(CHARGE_BATTERY_GOOD == batt_stat && POWER_ON_START_REASON_CHARGE == en_boot_reason) {
        /*
         * this is just for hisi
         */
#if 0
        strcpy(misc_str, "PWD:CHG_BATTERY");
        power_down_charge_flag = 1;
        power_on_acore_state_set(POWER_RECOVERY_A);


        goto boot_from_recovery;
#else
        power_on_acore_state_set(POWER_NORMAL);
        return BOOT_ON_NORMAL_MODE;
#endif
    }
    else if(CHARGE_BATTERY_GOOD == batt_stat || CHARGE_BATTERY_ONLY == batt_stat)
    {
        return BOOT_ON_NORMAL_MODE;             /* 热复位或者按键直接进入正常启动模式 */
    }
    else if(CHARGE_BATTERY_LOWER == batt_stat){ /* 低电且充电器不在位 */
        boot_show_battery_low();
        boot_power_off();
    }else if(CHARGE_BATTERY_LOWER_WITH_CHARGE == batt_stat){
        strcpy(misc_str, "PWD:LOW_BATTERY");   /*低电，充电器在位，进入recovery */
        power_down_charge_flag = 1;
        goto boot_from_recovery;
    }
    else if(CHARGE_BATTERY_OVER_TEMP == batt_stat){
        /* 过温恢复进入recovery */
        strcpy(misc_str, "PWD:OVER_TEMP");
        power_down_charge_flag = 1;
        goto boot_from_recovery;

    }else if(CHARGE_BATTERY_BAD == batt_stat){
        /* 坏电池恢复进入recovery */
        strcpy(misc_str, "PWD:BATT_BAD");
        power_down_charge_flag = 1;
        goto boot_from_recovery;
    }
    else if(BATTERY_BAD_WITHOUT_CHARGE == batt_stat){
        boot_power_off();
    }
    else
    {

    }

    return BOOT_ON_NORMAL_MODE;

#endif

    /*lint -save -e527*/
    return BOOT_ON_FASTBOOT_MODE;
    /*lint -restore*/

boot_from_recovery:
	memset(&misc_message, 0, sizeof(misc_message_type));
    /* coverity[secure_coding] */
	strcpy(misc_message.command, misc_str);

	if (MISC_ERROR == set_misc_message(&misc_message)) {
		pr_dbg("[fastboot]: misc command write Failed\n");
        return BOOT_ON_FASTBOOT_MODE;
	} else
#if 0
        return BOOT_ON_RECOVERY_MODE;
#else
        return BOOT_ON_FASTBOOT_MODE;
#endif
}

/*****************************************************************************
 函 数 名  : fastboot_preboot
 功能描述  : 根据开机模式，启动内核
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int fastboot_preboot(void)
{
    int rt = 0;
    boot_on_mode boot_mode = BOOT_ON_NORMAL_MODE;

    /* 清空C核启动状态 */
    power_on_c_status_set(POWER_NO_START);

	/*return 0;*/

    boot_mode = get_power_on_mode();

    power_on_mode_flag = boot_mode;

	pr_dbg("[fastboot]: boot_mode %d\n", boot_mode);

    if(BOOT_ON_NORMAL_MODE == boot_mode){
          rt = boot_rtx_cm3_from_flash();
          rt |= boot_linux_from_flash();      /* 只在normal模式下加载m3 firmware */
    }else if(BOOT_ON_RECOVERY_MODE == boot_mode){
          rt = boot_recovery_from_flash();
    }else{
        /* 进入fastboot模式 */
    }

    if(rt < 0){
        /* 加载镜像失败重启 */
        boot_do_reboot();
    }

    return 0;

}

/*****************************************************************************
 函 数 名  : get_pw_charge_flag
 功能描述  : 判断charger是否在位
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int get_pw_charge_flag(void)
{
	pr_dbg("[fastboot]: power_down_charge_flag %d. \n", power_down_charge_flag);
	return (int)power_down_charge_flag;
}

/*****************************************************************************
 函 数 名  : get_soft_version_flag
 功能描述  : 获取软件版本标志
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int get_soft_version_flag(void)
{
	pr_dbg("[fastboot]: soft_version_flag %d. \n", soft_version_flag);
    return (int)soft_version_flag;
}

/*****************************************************************************
 函 数 名  : get_pw_mode_flag
 功能描述  : 获取启动模式标志
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int get_pw_mode_flag(void)
{
	pr_dbg("[fastboot]: get_pw_on_mode %d. \n", power_on_mode_flag);
	return (int)power_on_mode_flag;
}

/*****************************************************************************
 函 数 名  : get_abnormal_reset_flag
 功能描述  : 获取复位类型标志
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int get_abnormal_reset_flag(void)
{
	pr_dbg("[fastboot]: abnormal_reset_flag %d. \n", abnormal_reset_flag);
	return (int)abnormal_reset_flag;
}

