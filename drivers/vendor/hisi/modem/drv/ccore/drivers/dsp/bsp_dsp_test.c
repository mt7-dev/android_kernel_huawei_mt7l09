
/*lint --e{537} */

#include <stdio.h>

#include <osl_types.h>
#include <osl_bio.h>
#include <osl_sem.h>
#include <osl_wait.h>
#include <osl_spinlock.h>

#include <hi_base.h>
#include <hi_syssc.h>
#include <hi_syscrg.h>
#include <hi_pwrctrl.h>
#include <hi_dsp.h>
#include <bsp_sram.h>

#include <bsp_om.h>
#include <bsp_edma.h>
#include <bsp_dpm.h>
#include <bsp_dsp.h>
#include <bsp_icc.h>
#include <bsp_version.h>
#include <bsp_hardtimer.h>
#include <bsp_busstress.h>





int g_bbe_stress_test_result = 0;


int bsp_bbe_stress_test_start(void);
int bsp_bbe_stress_test_start(void)
{
    int ret = 0;

    ret = bsp_bbe_load_muti();
    if (ret)
        return ret;

    writel(DDR_LPHY_SDR_ADDR + 0x1C0000, BBE_TCM_ADDR + 0x40000);
    writel(1024, BBE_TCM_ADDR + 0x40004);
    writel(MEM_FOR_BBE16_BUSSTRESS, BBE_TCM_ADDR + 0x40008);
    writel(AXI_MEM_64_SIZE_FOR_BBE16, BBE_TCM_ADDR + 0x4000C);
    writel(0, BBE_TCM_ADDR + 0x4001C);

    return bsp_bbe_run();
}

int bsp_bbe_stress_test_stop(void);
int bsp_bbe_stress_test_stop(void)
{
    int ret = 0;
    u32 value = 0;

    ret |= bsp_bbe_stop();

    value = readl(BBE_TCM_ADDR + 0x40018);
    if (0xAAAAAAAA == value)
        g_bbe_stress_test_result = 0;
    else
        g_bbe_stress_test_result = 1;

    ret |= bsp_bbe_reset();
    ret |= bsp_bbe_clock_disable();

    return ret;
}

int bsp_bbe_stress_test_result(void);
int bsp_bbe_stress_test_result(void)
{
    return g_bbe_stress_test_result;
}


