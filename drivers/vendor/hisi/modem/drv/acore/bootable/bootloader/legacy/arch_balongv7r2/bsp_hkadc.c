/*lint --e{537} */
#include <hi_hkadc.h>
#include <soc_memmap.h>

#include <boot/boot.h>
#include <balongv7r2/types.h>

#include <bsp_pmu.h>
#include <bsp_hkadc.h>

#include <product_config.h>

static u32 g_is_hkadc_has_rs_bug = 0;

int bsp_hkadc_ssi_transfer(u32 value_20, u32 value_24, u32 value_28)
{
    u32 result;
    u32 count = 0x10000;

    writel(value_20, HI_HKADCSSI_REGBASE_ADDR + 0x20);
    writel(value_24, HI_HKADCSSI_REGBASE_ADDR + 0x24);
    writel(value_28, HI_HKADCSSI_REGBASE_ADDR + 0x28);

    writel(0x01, HI_HKADCSSI_REGBASE_ADDR + 0x00);

    do
    {
        result = readl(HI_HKADCSSI_REGBASE_ADDR + 0x00);
        count--;
    }while(result && count);

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

//lint -e{14}
int bsp_hkadc_convert(enum HKADC_CHANNEL_ID channel_id, u16* value)
{
    int ret = 0;
    u32 channel = (u32)channel_id;
    u32 result = 0;

    if (NULL == value)
    {
        cprintf("hkadc value is null\r\n");
        return -1;
    }

	/* coverity[unsigned_compare] */
    if (channel_id > HKADC_CHANNEL_MAX)
    {
        cprintf("hkadc channel error, %d\r\n", channel);
        return -1;
    }

    ret = bsp_hkadc_ssi_transfer(0x01010000 | channel, 0x84008300, HKADC_DELAY_CYCLES);
    if (!ret)
    {
        result = readl(HI_HKADCSSI_REGBASE_ADDR + 0x14);
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
        cprintf("hkadc convert timeout\r\n");
    }

    /* By pass on */
    if (!g_is_hkadc_has_rs_bug)
    {
        ret |= bsp_hkadc_ssi_transfer(0x00800080, 0x00800080, 0x00010001);
    }

    return ret;
}

void bsp_hkadc_init()
{
#if defined(CONFIG_PMIC_HI6551)
    u8 smart_star_version = 0;
#endif

    udelay(500);

#if defined(CONFIG_PMIC_HI6551)
    smart_star_version = bsp_pmu_version_get();
    if ((BSP_PMU_V210 == smart_star_version) || (BSP_PMU_V220 == smart_star_version))
    {
        g_is_hkadc_has_rs_bug = 1;
        bsp_hkadc_ssi_transfer(0x00830003, 0x00, HKADC_INIT_DELAY_CYCLES);
        udelay(20);
    }
#endif
    return;
}

