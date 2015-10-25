#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/types.h>
#include <linux/io.h>

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>

#include <product_config.h>
#include <osl_bio.h>
#include <hi_base.h>
#include <hi_syssc.h>
#include <hi_syscrg.h>
#include <hi_pwrctrl.h>

#include <hi_dsp.h>

#include <bsp_busstress.h>


#ifdef __cplusplus
extern "C" {
#endif

#define AXI_MEM_64_SRC_FOR_HIFI_PHY 	SRAM_V2P(AXI_MEM_64_SRC_FOR_HIFI)
#define AXI_MEM_FOR_HIFI_SRC_ADDR_PHY      SRAM_V2P(AXI_MEM_FOR_HIFI_SRC_ADDR)

static u32 bbe16_tcm_addr, hifi2_tcm_addr;

int bsp_hifi_pll_enable(void)
{
#if defined(BSP_DSP_BBE16)
    /* do nothing */
#elif defined(BSP_DSP_DSP0)
/*  for V7R11
    set_hi_crg_dfs3_ctrl3_pd(0);

    while (!get_hi_crg_dfs3_ctrl3_lock()) ;
*/
#endif

    return 0;
}

int bsp_hifi_pll_disable(void)
{
#if defined(BSP_DSP_BBE16)
    /* do nothing */
#elif defined(BSP_DSP_DSP0)
/*
    set_hi_crg_dfs3_ctrl3_pd(1);
*/
#endif

    return 0;
}

int bsp_hifi_run(void)
{
    set_hi_sc_ctrl12_hifi_runstall(0);
    return 0;
}

int bsp_hifi_stop(void)
{
    set_hi_sc_ctrl12_hifi_runstall(1);
    return 0;
}

int bsp_hifi_power_on(void)
{
#ifdef BSP_DSP_BBE16
    /* power on HiFi */
    set_hi_pwr_ctrl7_hifi_mtcmos_ctrl(1);
    while (!get_hi_pwr_stat1_hifi_mtcmos_rdy()) ;

    /* disable ISO clamp */
    set_hi_pwr_ctrl6_hifi_iso_ctrl(0);
#else
    /* power on HiFi */
    set_hi_pwr_ctrl6_hifi_mtcmos_en(1);
    while(!get_hi_pwr_stat1_hifi_mtcmos_rdy()) ;

    /* disable ISO clamp */
    set_hi_pwr_ctrl5_hifi_iso_dis(1);
#endif

    return 0;
}

int bsp_hifi_power_off(void)
{
#ifdef BSP_DSP_BBE16
    /* enable ISO clamp */
    set_hi_pwr_ctrl6_hifi_iso_ctrl(1);

    /* power down HiFi */
    set_hi_pwr_ctrl7_hifi_mtcmos_ctrl(0);
#else
    /* enable ISO clamp */
    set_hi_pwr_ctrl4_hifi_iso_en(1);

    /* power down HiFi */
    set_hi_pwr_ctrl7_hifi_mtcmos_dis(1);
#endif

    return 0;
}

int bsp_hifi_clock_enable(void)
{
#ifdef BSP_DSP_BBE16
    /* enable clock */
    set_hi_crg_ctrl9_tensi_hifi_clk_en(1);
#else
    /* enable clock */
    set_hi_crg_clken3_hifi_clk_en(1);
    set_hi_crg_clken3_hifi_dbg_clk_en(1);
#endif

    return 0;
}

int bsp_hifi_clock_disable(void)
{
#ifdef BSP_DSP_BBE16
    /* disable clock */
    set_hi_crg_ctrl10_tensi_hifi_clk_dis(1);
#else
    /* disable clock */
    set_hi_crg_clkdis3_hifi_clk_dis(1);
    set_hi_crg_clkdis3_hifi_dbg_clk_dis(1);
#endif

    return 0;
}

int bsp_hifi_unreset(void)
{
#ifdef BSP_DSP_BBE16
    set_hi_crg_ctrl15_hifi_srst_req(0);
#else
	set_hi_crg_srstdis2_hifi_core_srst_dis(1);
    set_hi_crg_srstdis2_hifi_pd_srst_dis(1);
    set_hi_crg_srstdis2_hifi_dbg_srst_dis(1);
#endif

    return 0;
}

int bsp_hifi_reset(void)
{
#ifdef BSP_DSP_BBE16
    set_hi_crg_ctrl15_hifi_srst_req(1);
#else
	set_hi_crg_srsten2_hifi_core_srst_en(1);
    set_hi_crg_srsten2_hifi_pd_srst_en(1);
    set_hi_crg_srsten2_hifi_dbg_srst_en(1);
#endif

    return 0;
}

s32 show_bbe16_test_status(void)
{
	u32 status = readl(bbe16_tcm_addr+0x40018);

	printk(KERN_INFO"test result:0x%x.\n", status);

    /*
       success ---- 0xAAAAAAAA
       fail    ---- 0x55555555
     */
    if (0xAAAAAAAA == status)
        return 0;
    else if (0x55555555 == status)
        return -1;
    else
        return -2;
}

s32 bbe16_stress_test_start(void)
{
    u32 readlen = 0;
    unsigned int handle;
    mm_segment_t oldfs;
    u32 buffer[0x10];
    long ret;

#ifdef BSP_DSP_BBE16
    set_hi_crg_ctrl15_tensi_dps0_srst_req(0);
    set_hi_crg_ctrl15_tensi_bbe16_srst_req(0);
#else
	set_hi_crg_srstdis2_tensi_dps0_pd_srst_dis(1);
	set_hi_crg_srstdis2_tensi_dsp0_core_srst_dis(1);
#endif

    bbe16_tcm_addr = (u32)ioremap_nocache(BBE_TCM_ADDR, BBE_TCM_SIZE);
    if (NULL == (void*)bbe16_tcm_addr)
    {
        printk(KERN_ERR"fail to io remap\n");
        return -ENOMEM;
    }
#if 0
    /* BBE16 DMEM1 Æ«ÒÆ0x40000(256KB) */
	writel(DDR_TLPHY_IMAGE_ADDR, bbe16_tcm_addr + 0x40000);
    writel(1024/4, bbe16_tcm_addr+0x40004);
    writel(AXI_MEM_64_SRC_FOR_BBE16_PHY, bbe16_tcm_addr+0x40008);
    writel(AXI_MEM_FOR_MEMCPY_SIZE*2/4, bbe16_tcm_addr+0x4000C);
    writel(AXI_MEM_FOR_BBE16_SRC_ADDR_PHY, bbe16_tcm_addr+0x40010);
    writel(AXI_MEM_FOR_MEMCPY_SIZE*2/4, bbe16_tcm_addr+0x40014);
    writel(0, bbe16_tcm_addr+0x4001C);
#endif

    oldfs = get_fs();
    set_fs(KERNEL_DS);

    handle = (unsigned int)sys_open("/data/bbe16", O_RDONLY, 0);
    if (IS_ERR((const void*)handle))
    {
        printk(KERN_ERR"fail to open file '/data/bbe16'\n");
        return -1;
    }

    do
    {
        ret = sys_read(handle, (char*)buffer, sizeof(buffer));
        memcpy((void*)(bbe16_tcm_addr+0x40080+readlen), (void*)buffer, sizeof(buffer));
        readlen += ret;
    }while(ret == sizeof(buffer));

    sys_close(handle);
    set_fs(oldfs);

#ifdef BSP_DSP_BBE16
    set_hi_sc_ctrl13_bbe16_runstall(0);
#else
    set_hi_sc_ctrl13_dsp0_runstall(0);
#endif

    printk(KERN_INFO"OK, image length: %d\n", readlen);

    return 0;
}

s32 bbe16_stress_test_stop(void)
{
    if (bbe16_tcm_addr)
        iounmap((void*)bbe16_tcm_addr);
#ifdef BSP_DSP_BBE16
    set_hi_sc_ctrl13_bbe16_runstall(1);
#else
    set_hi_sc_ctrl13_dsp0_runstall(1);
#endif

    return 0;
}

s32 show_hifi2_test_status(void)
{
#ifdef BSP_DSP_BBE16
    u32 status = readl(hifi2_tcm_addr+0x4018);
#else
    u32 status = readl(hifi2_tcm_addr+0x8018);
#endif

	printk(KERN_INFO"test result:0x%x.\n", status);

    if (0xAAAAAAAA == status)
        return 0;
    else if (0x55555555 == status)
        return -1;
    else
        return -2;
}
s32 hifi2_stress_test_start(void)
{
    u32 readlen = 0;
    unsigned int handle;
    mm_segment_t oldfs;
    u32 buffer[0x10];
    long ret = 0;

    int error = 0;

    error |= bsp_hifi_pll_enable();
    error |= bsp_hifi_power_on();
    error |= bsp_hifi_clock_enable();
    error |= bsp_hifi_unreset();

    hifi2_tcm_addr = (u32)ioremap_nocache(HIFI_TCM_ADDR, HIFI_TCM_SIZE);
    if (NULL == (void*)hifi2_tcm_addr)
    {
        printk(KERN_ERR"fail to io remap\n");
        return -ENOMEM;
    }

    oldfs = get_fs();
    set_fs(KERNEL_DS);

    handle = (unsigned int)sys_open("/data/hifi2", O_RDONLY, 0);
    if (IS_ERR((const void*)handle))
    {
        printk(KERN_ERR"fail to open file '/data/hifi2'\n");
        return -1;
    }

#ifdef BSP_DSP_BBE16
    do
    {
        ret = sys_read(handle, (char*)buffer, sizeof(buffer));
        memcpy((void*)(hifi2_tcm_addr+0x4080+readlen), (void*)buffer, sizeof(buffer));
        readlen += ret;
    }while(ret == sizeof(buffer));
#else
    do
    {
        ret = sys_read(handle, (char*)buffer, sizeof(buffer));
        memcpy((void*)(hifi2_tcm_addr+readlen), (void*)buffer, sizeof(buffer));
        readlen += ret;
    }while(ret == sizeof(buffer));
#endif

    sys_close(handle);
    set_fs(oldfs);

#if 0
#ifdef BSP_DSP_BBE16
    /* HiFi2 DMEM1Æ«ÒÆ0x4000(16KB) */
    writel(DDR_TLPHY_IMAGE_ADDR, hifi2_tcm_addr+0x4000);
    writel(1024/4, hifi2_tcm_addr+0x4004);
    writel(AXI_MEM_64_SRC_FOR_HIFI_PHY, hifi2_tcm_addr+0x4008);
    writel(AXI_MEM_64_FOR_MEMCPY_SIZE*2/4, hifi2_tcm_addr+0x400C);
    writel(AXI_MEM_FOR_HIFI_SRC_ADDR_PHY, hifi2_tcm_addr+0x4010);
    writel(AXI_MEM_FOR_MEMCPY_SIZE*2/4, hifi2_tcm_addr+0x4014);
    writel(0, hifi2_tcm_addr+0x401C);
#else
    /* HiFi2 DMEM1Æ«ÒÆ0x8000(32KB) */
    writel(DDR_TLPHY_IMAGE_ADDR, hifi2_tcm_addr+0x8000);
    writel(1024/4, hifi2_tcm_addr+0x8004);
    writel(AXI_MEM_64_SRC_FOR_HIFI_PHY, hifi2_tcm_addr+0x8008);
    writel(AXI_MEM_64_FOR_MEMCPY_SIZE*2/4, hifi2_tcm_addr+0x800C);
    writel(AXI_MEM_FOR_HIFI_SRC_ADDR_PHY, hifi2_tcm_addr+0x8010);
    writel(AXI_MEM_FOR_MEMCPY_SIZE*2/4, hifi2_tcm_addr+0x8014);
    writel(0, hifi2_tcm_addr+0x801C);
    set_hi_sc_ctrl12_hifi_runstall(0);
#endif
#endif
    /* HiFi2 DMEM1Æ«ÒÆ0x8000(32KB) */
    writel(SHD_DDR_V2P(HIFI_DDR_BASEADDR), hifi2_tcm_addr+0x8000);
    writel(HIFI_DDR_SIZE/4, hifi2_tcm_addr+0x8004);
    writel(AXI_MEM_64_SRC_FOR_HIFI_PHY, hifi2_tcm_addr+0x8008);
    writel(AXI_MEM_FOR_MEMCPY_SIZE*2/4, hifi2_tcm_addr+0x800C);
    writel(AXI_MEM_FOR_HIFI_SRC_ADDR_PHY, hifi2_tcm_addr+0x8010);
    writel(AXI_MEM_FOR_MEMCPY_SIZE*2/4, hifi2_tcm_addr+0x8014);
    writel(0, hifi2_tcm_addr+0x801C);

    error |= bsp_hifi_run();


    printk(KERN_INFO"OK, image length: %d\n", readlen);

    return error;
}

s32 hifi2_stress_test_stop(void)
{
    int ret = 0;

    if (hifi2_tcm_addr)
        iounmap((void*)hifi2_tcm_addr);

    ret |= bsp_hifi_stop();
    ret |= bsp_hifi_reset();
    ret |= bsp_hifi_clock_enable();
    ret |= bsp_hifi_power_off();
    ret |= bsp_hifi_pll_disable();

    return ret;
}


#ifdef __cplusplus
}
#endif
