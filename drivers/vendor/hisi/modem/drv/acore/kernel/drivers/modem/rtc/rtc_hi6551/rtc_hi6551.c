
/*lint -save -e537*/
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <osl_bio.h>
#include <osl_types.h>
#include <drv_comm.h>
#include <bsp_icc.h>
#include "rtc_hi6551.h"
/*lint -restore*/
/*lint --e{958}*/
/*lint -save -e413 -e26*/
struct rtc_control g_hi6551_rtc_ctrl =
{
    .rtc_cur_addr = HI6551_RTCCR_A0_OFFSET,
    .rtc_clr_addr = HI6551_RTCLR_A0_OFFSET,
    .alarm_irq = {  PMU_INT_ALARM_A, PMU_INT_ALARM_B, PMU_INT_ALARM_C,
                    PMU_INT_ALARM_D, PMU_INT_ALARM_E, PMU_INT_ALARM_F },
	.alarm_base_addr = {HI6551_RTCMR_A_A0_OFFSET, HI6551_RTCMR_A_B0_OFFSET, HI6551_RTCMR_A_C0_OFFSET,
                        HI6551_RTCMR_A_D0_OFFSET, HI6551_RTCMR_A_E0_OFFSET, HI6551_RTCMR_A_F0_OFFSET },
};
/*lint -restore +e26*/
/*lint -save -e438*/
/*lint -save -e533*/
/*****************************************************************************
 函 数 名  : hi6551_rtc_read_reg
 功能描述  : 读取RTC寄存器
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
u32 hi6551_rtc_read_reg(u16 addr)
{
    /*lint -save -e958*/
    u8 val1 = 0;
    u8 val2 = 0;
    u8 val3 = 0;
    u8 val4 = 0;
    u32 value = 0;

    bsp_hi6551_reg_read(addr + 0, &val1);
    bsp_hi6551_reg_read(addr + 1, &val2);
    bsp_hi6551_reg_read(addr + 2, &val3);
    bsp_hi6551_reg_read(addr + 3, &val4);

    value = (u32)(val1 | ((u32)val2 << 8) | ((u32)val3 << 16) | ((u32)val4 << 24));
    /*lint -restore*/
    return value;
}

/*****************************************************************************
 函 数 名  : hi6551_rtc_write_reg
 功能描述  : 写RTC寄存器
 输入参数  : void
 输出参数  : 无
 返 回 值  : RTC_OK
 调用函数  :
 被调函数  :
*****************************************************************************/
/*lint -save -e958*/
u32 hi6551_rtc_write_reg(u16 addr, u32 pValue)
{
/*lint -restore*/
    u8 val1 = 0;
    u8 val2 = 0;
    u8 val3 = 0;
    u8 val4 = 0;

    val1 = (((u32)pValue)>>0)&(0xFF);
    val2 = (((u32)pValue)>>8)&(0xFF);
    val3 = (((u32)pValue)>>16)&(0xFF);
    val4 = (((u32)pValue)>>24)&(0xFF);

    bsp_hi6551_reg_write( addr + 0, val1);
    bsp_hi6551_reg_write( addr + 1, val2);
    bsp_hi6551_reg_write( addr + 2, val3);
    bsp_hi6551_reg_write( addr + 3, val4);

    return RTC_OK;
}

/**
 * balong_rtc_readtime-read the RTC time
 * @dev:	the rtc device.
 * @tm:		the RTC time pointer which point to the RTC time
 *
 */
s32 hi6551_rtc_readtime(struct device *dev, struct rtc_time *tm)
{
    u32 value = 0;
    value = hi6551_rtc_read_reg(g_hi6551_rtc_ctrl.rtc_cur_addr);
    rtc_time_to_tm(value, tm);

	dev_dbg(dev, "%s: %4d-%02d-%02d %02d:%02d:%02d\n", "readtime",
		1900 + tm->tm_year, tm->tm_mon, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);

	return BSP_OK;
}
/**
 * balong_rtc_settime-set the RTC time
 * @dev:	the RTC device.
 * @tm:		the RTC time which will be set
 *
 */
s32 hi6551_rtc_settime(struct device *dev, struct rtc_time *tm)
{
    u32 secs = 0;
    s32  ret = 0;
    u32 alarmtime = 0;
    u32 alarmenable = 0, alarm_id = 0;
    u32 curtime = 0;/*当前的时间值*/

    ret  = rtc_valid_tm(tm);
    if (0 != ret)
    {
        hi6551_rtc_print_error("RTC: rtc_valid_tm error!\n");
        return BSP_ERROR;
    }

    ret = rtc_tm_to_time(tm, (unsigned long *)&secs);
    if (ret != 0)
         return ret;
    /* store the new base time in rtc register */
    secs += 1;
    curtime = hi6551_rtc_read_reg(g_hi6551_rtc_ctrl.rtc_cur_addr);

    for(alarm_id = 0;alarm_id < HI6551_RTC_DEVICE_NUM; alarm_id++)
    {
        alarmtime = hi6551_rtc_read_reg(g_hi6551_rtc_ctrl.alarm_base_addr[alarm_id]);

        /*只有在设置了闹钟的时候才进行下面的重新设置闹钟时间动作*/
        /*查询是否是能了该闹钟*/
        alarmenable = (u32)bsp_pmu_irq_is_masked(g_hi6551_rtc_ctrl.alarm_irq[alarm_id]);

        if ((alarmenable)&&( curtime < alarmtime )) {
            /* disable interrupts */
            bsp_pmu_irq_mask(g_hi6551_rtc_ctrl.alarm_irq[alarm_id]);
            alarmtime = secs + (alarmtime - curtime);
            hi6551_rtc_write_reg(g_hi6551_rtc_ctrl.alarm_base_addr[alarm_id], alarmtime);
            /* enable alarm interrupts */
            bsp_pmu_irq_unmask(g_hi6551_rtc_ctrl.alarm_irq[alarm_id]);
        }
    }

    hi6551_rtc_write_reg(g_hi6551_rtc_ctrl.rtc_clr_addr, secs);
    return BSP_OK;
}

/**
 * balong_alarm_irq_enable-enable or disenable RTC alarm irq
 * @dev:	the RTC device.
 * @enabled:		1: enable the RTC alrm irq
 *					0: disenable the RTC alarm irq
 *
 */
/*lint --e{533}*/
s32 hi6551_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
    struct platform_device *rtc_dev = NULL;
    u32 alarm_id = 0;
	/* Clear any pending alarm interrupts. */
	/*清除中断*/
    /*lint -save -e413 -e831*/
    rtc_dev = to_platform_device(dev);
    alarm_id = rtc_dev->id;
    /*lint -restore*/
    hi6551_rtc_print_dbg("alarm_id = %d", alarm_id);
    /* if interupt enabled,unmask */
    /* if interupt unenabled,mask */
	if (ALARM_DISABLED == enabled)
    {
        bsp_pmu_irq_mask(g_hi6551_rtc_ctrl.alarm_irq[alarm_id]);
    }
	else
    {
        bsp_pmu_irq_unmask(g_hi6551_rtc_ctrl.alarm_irq[alarm_id]);
    }
	return BSP_OK;
}
/**
 * balong_rtc_readalarm-read rtc alarm
 */
/*lint -save -e64*/
s32 hi6551_rtc_readalarm(struct device *dev, struct rtc_wkalrm *alarm)
{
/*lint -restore*/
    u32 alarmtime = 0;
    struct platform_device *rtc_dev;
    u32 alarm_id = 0;
    /*lint -save -e413 -e831*/
    rtc_dev = to_platform_device(dev);
    alarm_id = rtc_dev->id;
    /*lint -restore*/
    alarmtime = hi6551_rtc_read_reg(g_hi6551_rtc_ctrl.alarm_base_addr[alarm_id]);
    rtc_time_to_tm(alarmtime, &(alarm->time));
    alarm->pending = 0;
    if(bsp_pmu_irq_is_masked(g_hi6551_rtc_ctrl.alarm_irq[alarm_id]))
    {
        alarm->enabled = ALARM_DISABLED;
    }
    else
    {
        alarm->enabled = ALARM_ENABLE;
    }

	return BSP_OK;
}
/**
 * balong_rtc_setalarm-set rtc alarm
 */
/*lint -save -e64*/
s32 hi6551_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alarm)
{
/*lint -restore*/
    struct platform_device *rtc_dev;
    s32 ret = 0;
    u32 alarmtime = 0;
    u32 currenttime = 0;
    u32 alarm_id = 0;
    /*lint -save -e413 -e831*/
    rtc_dev = to_platform_device(dev);

    alarm_id = rtc_dev->id;
    /*lint -restore*/
    hi6551_rtc_print_dbg("alarm_id = %d", alarm_id);

    /*请中断*/
    /********************************************/
    bsp_pmu_irq_mask(g_hi6551_rtc_ctrl.alarm_irq[alarm_id]);

    ret = rtc_valid_tm(&alarm->time);
    if (0 != ret)
    {
	    hi6551_rtc_print_error("balong rtc_valid_tm Fail!\n");
        return BSP_ERROR;
    }

    ret = rtc_tm_to_time(&alarm->time, (unsigned long *)&alarmtime);
    if (0 != ret)
    {
	    hi6551_rtc_print_error("balong rtc_tm_to_time Fail!\n");
        return BSP_ERROR;
    }
    currenttime = hi6551_rtc_read_reg(g_hi6551_rtc_ctrl.rtc_cur_addr);
    if(currenttime <= alarmtime)
    {
        hi6551_rtc_write_reg(g_hi6551_rtc_ctrl.alarm_base_addr[alarm_id], alarmtime);
    }
    else
    {
        hi6551_rtc_print_error("the alarm time to be set is error.\r\n");
    }

    hi6551_alarm_irq_enable(dev, alarm->enabled);

	return BSP_OK;
}
/**
 * balong_rtc_alarmhandler-the rtc irq handler function
 */
/*lint -save -e64*/
void hi6551_rtc_alarmhandler(void *data)
{
/*lint -restore*/
    u32 events = 0;
    struct hi6551_alarm_para *palarm_para = (struct hi6551_alarm_para *)data;

	events = RTC_IRQF | RTC_AF;
	rtc_update_irq(palarm_para->rtc_dev, 1, events);
    /*lint -save -e533*/
    bsp_pmu_irq_mask(g_hi6551_rtc_ctrl.alarm_irq[palarm_para->alarm_id]);
    hi6551_rtc_print_dbg("=========alram is handled==========\r\n");
	return;
    /*lint -restore*/
}

/*RTC操作函数数据结构*/
/*lint -save -e527*/
static const struct rtc_class_ops hi6551_rtc_ops = {
/*lint -restore*/
	.read_time	= hi6551_rtc_readtime,
	.set_time	= hi6551_rtc_settime,
	.read_alarm	= hi6551_rtc_readalarm,
	.set_alarm	= hi6551_rtc_setalarm,
	.alarm_irq_enable = hi6551_alarm_irq_enable,
};

/**
 * balong_rtc_cb_settime-the icc callback function when ccore set the time through icc
 * @channel_id:	the icc channel id.
 * @len:		the type of the container struct this is embedded in.
 * @context:	the parameter pass from the register
 *
 */
#ifndef CONFIG_BALONG_RTC
static s32 balong_rtc_cb_settime(u32 channel_id , u32 len, void* context)
{
	s32 ret = 0;
	struct rtc_time tm = {0};
    hi6551_rtc_print_dbg("balong_rtc_cb_settime is actived\r\n");
	ret = bsp_icc_read(channel_id, (u8*)&tm, len);
    hi6551_rtc_print_dbg("%4d-%02d-%02d %02d:%02d:%02d\n",
            RTC_BASE_YEAR + tm.tm_year, tm.tm_mon, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
	if(len != (u32)ret)
	{
		hi6551_rtc_print_error("balong_rtc_cb_settime error \r\n");
		return BSP_ERROR;
	}
	return hi6551_rtc_settime(NULL, &tm);
}
#endif
/*lint -save -e563*/
static s32 __devinit  hi6551_rtc_probe(struct platform_device *pdev)
{

    struct rtc_device *balong_rtc = NULL;
    u32 alarm_irq = g_hi6551_rtc_ctrl.alarm_irq[pdev->id];
    struct hi6551_alarm_para *palarm_para = (struct hi6551_alarm_para *)kmalloc(sizeof(struct hi6551_alarm_para), GFP_KERNEL);
#ifndef CONFIG_BALONG_RTC
    u32 channel_id_set = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_RTC_SETTIME;
#endif
    s32 ret = 0;
    if(!palarm_para)
    {
        return RTC_ERROR;
    }
	balong_rtc = rtc_device_register(pdev->name, &pdev->dev, &hi6551_rtc_ops,
			THIS_MODULE);
	if (IS_ERR(balong_rtc))
    {
		ret = PTR_ERR(balong_rtc);
		goto cleanup0;
	}
	platform_set_drvdata(pdev, balong_rtc);

    palarm_para->alarm_id = pdev->id;
    palarm_para->rtc_dev = balong_rtc;
    /* coverity[noescape] */
    ret = bsp_pmu_irq_callback_register(alarm_irq, hi6551_rtc_alarmhandler, palarm_para);

    if (ret != 0)
		goto cleanup1;

#ifndef CONFIG_BALONG_RTC
    if(pdev->id == 0)
    {
        ret = bsp_icc_event_register(channel_id_set, (read_cb_func)balong_rtc_cb_settime, (void *)NULL, (write_cb_func)NULL, (void *)NULL);
        if(0 != ret)
        	goto cleanup2;
    }
#endif
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RTC,"hi6551 RTC[%d] init OK!\r\n", pdev->id);
    /* coverity[leak_storage] */
    return BSP_OK;

cleanup2:
	hi6551_rtc_print_error("rtc register icc fail\r\n");
	rtc_device_unregister(balong_rtc);
    balong_rtc = NULL;
    kfree(palarm_para);
    return ret;
cleanup1:
	hi6551_rtc_print_error("rtc request_irq fail\r\n");
	rtc_device_unregister(balong_rtc);
    balong_rtc = NULL;
    kfree(palarm_para);
    return ret;
cleanup0:
    hi6551_rtc_print_error("rtc device register fail\r\n");
    balong_rtc = NULL;
    kfree(palarm_para);
    return ret;
}
/*lint -restore*/
static s32  hi6551_rtc_remove(struct platform_device *pdev)
{
	struct rtc_device *rtc_dev = platform_get_drvdata(pdev);
    u32 alarm_id = 0;
#ifndef CONFIG_BALONG_RTC
    u32 channel_id_set = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_RTC_SETTIME;

    bsp_icc_event_unregister(channel_id_set);
#endif
    if(!rtc_dev)
    {
        return RTC_ERROR;
    }
    bsp_pmu_irq_mask(g_hi6551_rtc_ctrl.alarm_irq[alarm_id]);
    rtc_device_unregister(rtc_dev);
    rtc_dev = NULL;
	return BSP_OK;
}
/*lint -save -e958 -e813*/
struct platform_device hi6551_rtc_device[HI6551_RTC_DEVICE_NUM] = {
        {
            .name           = "hi6551_rtc0",
            .id             = 0,
        },
        {
            .name           = "hi6551_rtc1",
            .id             = 1,
        },
        {
            .name           = "hi6551_rtc2",
             .id             = 2,
        },
        {
            .name           = "hi6551_rtc3",
            .id             = 3,
        },
        {
            .name           = "hi6551_rtc4",
            .id             = 4,
        },
        {
            .name           = "hi6551_rtc5",
            .id             = 5,
        }
};

struct platform_driver hi6551_rtc_driver[HI6551_RTC_DEVICE_NUM] = {
	{
        .driver = {
    		.name = "hi6551_rtc0",
    		.owner = THIS_MODULE,
    	},
    	.probe	= hi6551_rtc_probe,
    	.remove = __devexit_p(hi6551_rtc_remove),
    },
    {
        .driver = {
    		.name = "hi6551_rtc1",
    		.owner = THIS_MODULE,
    	},
    	.probe	= hi6551_rtc_probe,
    	.remove = __devexit_p(hi6551_rtc_remove),
    },
    {
        .driver = {
            .name = "hi6551_rtc2",
            .owner = THIS_MODULE,
        },
        .probe  = hi6551_rtc_probe,
        .remove = __devexit_p(hi6551_rtc_remove),
     },
    {
        .driver = {
            .name = "hi6551_rtc3",
            .owner = THIS_MODULE,
        },
        .probe  = hi6551_rtc_probe,
        .remove = __devexit_p(hi6551_rtc_remove),
    },

    {
        .driver = {
    		.name = "hi6551_rtc4",
    		.owner = THIS_MODULE,
    	},
    	.probe	= hi6551_rtc_probe,
    	.remove = __devexit_p(hi6551_rtc_remove),
    },
    {
        .driver = {
    		.name = "hi6551_rtc5",
    		.owner = THIS_MODULE,
    	},
    	.probe	= hi6551_rtc_probe,
    	.remove = __devexit_p(hi6551_rtc_remove),
    }
};
/*lint -restore*/
s32 __init hi6551_rtc_init (void)
{
    u32 i = 0;
    for(i = 0;i < HI6551_RTC_DEVICE_NUM; i++)
    {
        if(platform_device_register(&hi6551_rtc_device[i]))
        {
            return RTC_ERROR;
        }
        if(platform_driver_register(&hi6551_rtc_driver[i]))
        {
            return RTC_ERROR;
        }
    };
    return RTC_OK;
}
static void __exit hi6551_rtc_exit(void)
{
    u32 i = 0;
    for(i = 0;i < HI6551_RTC_DEVICE_NUM; i++)
    {
    	platform_device_unregister(&hi6551_rtc_device[i]);
    	platform_driver_unregister(&hi6551_rtc_driver[i]);
    }
}
/*lint -restore*/
/*lint -restore*/
u32 hi6551_rtc_show_level(void)
{

    return BSP_MODU_HI6551_RTC;
}

/*
* adapt v9r1
*/
#ifndef CONFIG_BALONG_RTC
u32 hi6551_get_rtc_value (void)
{
    u32 rtc_value = 0;
    rtc_value = hi6551_rtc_read_reg(g_hi6551_rtc_ctrl.rtc_cur_addr);
    return rtc_value;
}
#endif

/*lint -save -e19*/
module_init(hi6551_rtc_init);
module_exit(hi6551_rtc_exit);
MODULE_AUTHOR("HI6930 RTC Device Driver");
MODULE_DESCRIPTION("HI6930 RTC Driver");
/*lint -restore +e19*/
