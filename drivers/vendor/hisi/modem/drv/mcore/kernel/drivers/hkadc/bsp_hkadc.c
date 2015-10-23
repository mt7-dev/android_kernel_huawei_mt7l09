/*lint --e{551} --e{537} */

#include <cmsis_os.h>
#include <product_config.h>
#include <osl_common.h>

#include <hi_hkadc.h>
#include <soc_memmap.h>

#ifndef BSP_HKADC_K3V3
#include <printk.h>
#include <bsp_pmu.h>
#endif
#include <bsp_om.h>
#include <bsp_icc.h>
#include <bsp_hardtimer.h>
#include <bsp_softtimer.h>
#include <bsp_hkadc.h>


/* use 2 args to ensure the security of the args */
struct bsp_hkadc_timer_args
{
    u32 icc_channel_id;
    enum HKADC_CHANNEL_ID hkadc_channel_id;
};
/* 0 for acore, 1 for ccore */
static struct bsp_hkadc_timer_args g_hkadc_timer_args[2];

static struct softtimer_list bsp_hkadc_timer_id[2];

#ifndef BSP_HKADC_K3V3

osMutexDef (bsp_hkadc_mutex);
osMutexId bsp_hkadc_mutex_id = NULL;

static u32 g_last_convert_time = 0;
static u32 g_last_power_on_time = 0;
static u32 g_is_hkadc_has_rs_bug = 0;

#define TIME_500US (500)
#define TIME_1MS (1000)
#define TICK_32K (32*1000)

#define HKADC_SSI_TIMEOUT (0x10000)
static int bsp_hkadc_ssi_transfer(u32 value_20, u32 value_24, u32 value_28)
{
    u32 result = 0;
    u32 count = HKADC_SSI_TIMEOUT;
#ifdef BSP_BBP_HKADC_BUG
    u32 read_count = 0;
#endif

    writel(value_20, HI_HKADCSSI_REGBASE_ADDR + 0x20);
    writel(value_24, HI_HKADCSSI_REGBASE_ADDR + 0x24);
    writel(value_28, HI_HKADCSSI_REGBASE_ADDR + 0x28);

    writel(0x01, HI_HKADCSSI_REGBASE_ADDR + 0x00);

#ifdef BSP_BBP_HKADC_BUG
    do
    {
        result = readl(HI_HKADCSSI_REGBASE_ADDR + 0x00);
        read_count = result ? 0 : (read_count + 1);
    }while((read_count < 10) && count);
#else
    do
    {
        result = readl(HI_HKADCSSI_REGBASE_ADDR + 0x00);
        count--;
    }while(result && count);
#endif

    if (!count)
    {
        /* time out, clean the flag */
        writel(0x01, HI_HKADCSSI_REGBASE_ADDR + 0x10);
        return -1;
    }
    else
    {
        return 0;
    }
}

int bsp_hkadc_convert(enum HKADC_CHANNEL_ID channel_id, u16* value)
{
    int ret = 0;
    u32 channel = (u32)channel_id;
    u32 result = 0;
    u32 now, interval;
#ifdef BSP_BBP_HKADC_BUG
    u32 read_count = 0;
#endif

    osStatus mutex_status;

    if ((NULL == value) || (channel_id > HKADC_CHANNEL_MAX))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_HKADC,
            "hkadc args error\r\n");
        return -1;
    }

    do
    {
        now = bsp_get_slice_value();
        interval = get_timer_slice_delta(g_last_power_on_time, now);
    }while (interval < (TICK_32K / TIME_1MS / 2)); /* 500us */

    do
    {
        now = bsp_get_slice_value();
        interval = get_timer_slice_delta(g_last_convert_time, now);
    }while (interval < (TICK_32K / TIME_1MS * 1)); /* 1ms */

    g_last_convert_time = bsp_get_slice_value();

    mutex_status = osMutexWait(bsp_hkadc_mutex_id, 0);
    if (mutex_status != osOK)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_HKADC, "fail to get hkadc mutex\r\n");
        return -1;
    }

    ret = bsp_hkadc_ssi_transfer(0x01010000 | channel, 0x84008300, HKADC_DELAY_CYCLES);
    if (!ret)
    {
#ifdef BSP_BBP_HKADC_BUG
        do
        {
            result = readl(HI_HKADCSSI_REGBASE_ADDR + 0x14);
            read_count = result ? 0 : (read_count + 1);
        }while((!result) && (read_count < 10));
#else
        result = readl(HI_HKADCSSI_REGBASE_ADDR + 0x14);
#endif
        /*
         * only the low 10 bits is valid (&0x3FF)
         * the low 10 bits is the high 10 bits of hkadc result's total 12 bits (*4)
         * convert the result to mv unit (*2500/4096)
         */
        result = ((result & 0x3FF) * 4 * 2500) >> 12;
        *value = (u16)result;
    }
    else
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_HKADC, "hkadc convert timeout\r\n");
    }

    /* By pass on */
    if (!g_is_hkadc_has_rs_bug)
    {
        ret |= bsp_hkadc_ssi_transfer(0x00800080, 0x00800080, 0x00010001);
    }

    mutex_status = osMutexRelease(bsp_hkadc_mutex_id);
    if (mutex_status != osOK)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_HKADC, "fail to release hkadc mutex\r\n");
        return -1;
    }

    return ret;
}

#endif

void bsp_hkadc_timer_callback(u32 args)
{
    int ret = 0;
    u32 cpuid = 0;
    struct bsp_hkadc_timer_args *hkadc_agrs = (struct bsp_hkadc_timer_args *)args;

    u16 voltage = 0;

    ret = bsp_hkadc_convert(hkadc_agrs->hkadc_channel_id, &voltage);
    if (ret) voltage = 0xFFFF;

    cpuid = (hkadc_agrs->icc_channel_id >> 16) == ICC_CHN_MCORE_ACORE ? ICC_CPU_APP : ICC_CPU_MODEM;
    ret = bsp_icc_send(cpuid, hkadc_agrs->icc_channel_id,
        (u8*)&voltage, sizeof(voltage));
    if (ret != (int)sizeof(voltage))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_HKADC, "icc send error %x\r\n", ret);
    }
}

int bsp_hkadc_icc_callback(u32 icc_channel_id , u32 len, void* context)
{
    int ret, timer_index;
    u32 hkadc_channel_id = 0;

    ret = bsp_icc_read(icc_channel_id, (u8*)&hkadc_channel_id, len);
    if (ret != (int)sizeof(hkadc_channel_id))
    {
        return -1;
    }

    timer_index = (icc_channel_id >> 16) == ICC_CHN_MCORE_ACORE ? 0 : 1;
    g_hkadc_timer_args[timer_index].icc_channel_id = icc_channel_id;
    g_hkadc_timer_args[timer_index].hkadc_channel_id = (enum HKADC_CHANNEL_ID)hkadc_channel_id;
    bsp_softtimer_add(&bsp_hkadc_timer_id[timer_index]);

    return 0;
}

void bsp_hkadc_resume(void)
{
#ifndef BSP_HKADC_K3V3
    g_last_power_on_time = bsp_get_slice_value();

    if (g_is_hkadc_has_rs_bug)
    {
        /*udelay(500);*/
        bsp_hkadc_ssi_transfer(0x00830003, 0x00, HKADC_INIT_DELAY_CYCLES);
        udelay(20);
    }
#endif
    return;
}

void bsp_hkadc_init(void)
{
    int ret = 0;
    u32 icc_channel_id;

#ifndef BSP_HKADC_K3V3
    bsp_hkadc_mutex_id = osMutexCreate(osMutex(bsp_hkadc_mutex));

#if defined(CONFIG_PMIC_HI6551) 
    u8 smart_star_version = 0;
    smart_star_version = bsp_pmu_version_get();
    if ((BSP_PMU_V210 == smart_star_version) || (BSP_PMU_V220 == smart_star_version))
    {
        g_is_hkadc_has_rs_bug = 1;
    }
#endif

    g_last_power_on_time = bsp_get_slice_value();
    g_last_convert_time = bsp_get_slice_value();
#endif

    bsp_hkadc_timer_id[0].func = bsp_hkadc_timer_callback;
	bsp_hkadc_timer_id[0].wake_type=SOFTTIMER_WAKE;
    bsp_hkadc_timer_id[0].timeout = 0;
	bsp_hkadc_timer_id[1].func = bsp_hkadc_timer_callback;
	bsp_hkadc_timer_id[1].wake_type=SOFTTIMER_WAKE;
    bsp_hkadc_timer_id[1].timeout = 0;
#ifndef BSP_HKADC_K3V3
    bsp_hkadc_timer_id[0].para = (u32)&g_hkadc_timer_args[0];
    ret |= bsp_softtimer_create(&bsp_hkadc_timer_id[0] );
#endif
    bsp_hkadc_timer_id[1].para = (u32)&g_hkadc_timer_args[1];
    ret |= bsp_softtimer_create( &bsp_hkadc_timer_id[1] );

#ifndef BSP_HKADC_K3V3
    icc_channel_id = ICC_CHN_MCORE_ACORE << 16 | MCORE_ACORE_FUNC_HKADC;

    ret |= bsp_icc_event_register(icc_channel_id,
        (read_cb_func)bsp_hkadc_icc_callback, NULL, NULL, NULL);
#endif

    icc_channel_id = ICC_CHN_MCORE_CCORE << 16 | MCORE_CCORE_FUNC_HKADC;

    ret |= bsp_icc_event_register(icc_channel_id,
        (read_cb_func)bsp_hkadc_icc_callback, NULL, NULL, NULL);

#ifndef BSP_HKADC_K3V3
    if(NULL == bsp_hkadc_mutex_id)
        ret = -1;
#endif
    if (ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_HKADC, "fail to init hkadc\r\n");
    }
}

#ifndef BSP_HKADC_K3V3
int bsp_hkadc_convert_test(int channel)
{
    int ret;
    u16 value;

    ret = bsp_hkadc_convert((enum HKADC_CHANNEL_ID)channel, &value);
    printk("channel %d, value %d, return %x\n", channel, value, ret);

    return ret;
}
#endif


