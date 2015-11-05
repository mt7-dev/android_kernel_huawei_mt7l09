

#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/io.h>
#include <linux/kernel.h>
#include <product_config.h>
#include <bsp_memmap.h>

u32 g_ddr_share_mem_addr_virt = 0;
u32 g_ddr_gu_addr_virt = 0;
u32 g_ddr_mntn_addr_virt = 0;
u32 g_ipf_regbase_addr_virt = 0;
u32 g_socp_regbase_addr_virt = 0;
u32 g_ipc_regbase_addr_virt = 0;
u32 g_bbp_systimer_regbase_addr_virt = 0;
u32 g_lpm3_tcm_virt  = 0;
u32 g_ap_sysctrl_regbase_addr_virt = 0;
u32 g_sram_virt = 0;
u32 g_ap_syscnt_regbase_addr_virt=0;

int balong_ioremap_init(void)
{
    g_ap_sysctrl_regbase_addr_virt   = (u32)ioremap_nocache(HI_AP_SYSCTRL_BASE_ADDR, HI_AP_SYSCTRL_REG_SIZE);
    g_ap_syscnt_regbase_addr_virt = (u32)ioremap_nocache(HI_AP_SYS_CNT_BASE_ADDR, HI_AP_SYS_CNT_SIZE);
    g_ddr_share_mem_addr_virt   = (u32)ioremap_nocache(DDR_SHARED_MEM_ADDR, DDR_SHARED_MEM_SIZE);
    g_ddr_gu_addr_virt          = (u32)ioremap_nocache(DDR_GU_ADDR, DDR_GU_SIZE);
    g_ddr_mntn_addr_virt        = (u32)ioremap_nocache(DDR_MNTN_ADDR, DDR_MNTN_SIZE);
    g_ipf_regbase_addr_virt     = (u32)ioremap_nocache(HI_IPF_REGBASE_ADDR, HI_IPF_REG_SIZE);
    g_socp_regbase_addr_virt    = (u32)ioremap_nocache(HI_SOCP_REGBASE_ADDR, HI_SOCP_REG_SIZE);
    g_ipc_regbase_addr_virt     = (u32)ioremap_nocache(HI_IPCM_REGBASE_ADDR, HI_IPCM_REG_SIZE);
#if (HI_BBP_SYSTIME_BASE_ADDR != HI_AP_SYSCTRL_BASE_ADDR)
    g_bbp_systimer_regbase_addr_virt     = g_ap_syscnt_regbase_addr_virt;
#else
	g_bbp_systimer_regbase_addr_virt = g_ap_sysctrl_regbase_addr_virt;
#endif
    g_lpm3_tcm_virt             = (u32)ioremap_nocache(HI_M3TCM0_MEM_ADDR, HI_M3TCM0_MEM_SIZE + HI_M3TCM1_MEM_SIZE);
    g_sram_virt                 = (u32)ioremap_nocache(HI_SRAM_MEM_BASE_ADDR, HI_SRAM_MEM_SIZE);
    printk("################ balong_ioremap_init ok! #####################\n");
    printk("ap sysctrl regbase: 		v: 0x%x p: 0x%x ok!\n",g_ap_sysctrl_regbase_addr_virt, HI_AP_SYSCTRL_BASE_ADDR);
    printk("share memory: 		v: 0x%x p: 0x%x ok!\n",g_ddr_share_mem_addr_virt, DDR_SHARED_MEM_ADDR);
    printk("gu    memory: 		v: 0x%x p: 0x%x ok!\n",g_ddr_gu_addr_virt, DDR_GU_ADDR);
    printk("mntn  memory: 		v: 0x%x p: 0x%x ok!\n",g_ddr_mntn_addr_virt, DDR_MNTN_ADDR);
    printk("ipf  regbase: 		v: 0x%x p: 0x%x ok!\n",g_ipf_regbase_addr_virt, HI_IPF_REGBASE_ADDR);
    printk("socp regbase: 		v: 0x%x p: 0x%x ok!\n",g_socp_regbase_addr_virt, HI_SOCP_REGBASE_ADDR);
    printk("ipc  regbase: 		v: 0x%x p: 0x%x ok!\n",g_ipc_regbase_addr_virt, HI_IPCM_REGBASE_ADDR);
    printk("syscnt_regbase: 	       v: 0x%x p: 0x%x ok!\n",g_ap_syscnt_regbase_addr_virt, HI_AP_SYS_CNT_BASE_ADDR);
    printk("lpm3 tcm: 	        v: 0x%x p: 0x%x ok!\n",g_lpm3_tcm_virt, HI_M3TCM0_MEM_ADDR);
    printk("sram: 	            v: 0x%x p: 0x%x ok!\n",g_sram_virt, HI_SRAM_MEM_BASE_ADDR);

    return 0;
}

arch_initcall(balong_ioremap_init);

#ifdef __cplusplus
}
#endif

