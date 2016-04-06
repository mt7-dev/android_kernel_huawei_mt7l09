

#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/io.h>
#include <linux/kernel.h>
#include <product_config.h>
#include <bsp_memmap.h>

void* g_ddr_share_mem_addr_virt = NULL;
//unsigned long  g_ddr_share_mem_addr_virt = 0;
void* g_ddr_gu_addr_virt = NULL;
void* g_ddr_mntn_addr_virt = NULL;
void* g_ipf_regbase_addr_virt = NULL;
void* g_socp_regbase_addr_virt = NULL;
void* g_ipc_regbase_addr_virt = NULL;
void* g_bbp_systimer_regbase_addr_virt = NULL;
void* g_lpm3_tcm_virt  = NULL;
void* g_ap_sysctrl_regbase_addr_virt = NULL;
void* g_sram_virt = NULL;
void* g_ap_syscnt_regbase_addr_virt=NULL;
void* g_modem_sysctrl_base_addr_virt=NULL;

int balong_ioremap_init(void)
{
    g_ap_sysctrl_regbase_addr_virt   = ioremap_nocache(HI_AP_SYSCTRL_BASE_ADDR, HI_AP_SYSCTRL_REG_SIZE);
    g_ap_syscnt_regbase_addr_virt = ioremap_nocache(HI_AP_SYS_CNT_BASE_ADDR, HI_AP_SYS_CNT_SIZE);
    g_ddr_share_mem_addr_virt   = ioremap_wc(DDR_SHARED_MEM_ADDR, DDR_SHARED_MEM_SIZE);
    g_ddr_gu_addr_virt          = ioremap_wc(DDR_GU_ADDR, DDR_GU_SIZE);
    g_ddr_mntn_addr_virt        = ioremap_wc(DDR_MNTN_ADDR, DDR_MNTN_SIZE);
    g_ipf_regbase_addr_virt     = ioremap_nocache(HI_IPF_REGBASE_ADDR, HI_IPF_REG_SIZE);
    g_socp_regbase_addr_virt    = ioremap_nocache(HI_SOCP_REGBASE_ADDR, HI_SOCP_REG_SIZE);
    g_ipc_regbase_addr_virt     = ioremap_nocache(HI_IPCM_REGBASE_ADDR, HI_IPCM_REG_SIZE);
#if (HI_BBP_SYSTIME_BASE_ADDR != HI_AP_SYSCTRL_BASE_ADDR)
    g_bbp_systimer_regbase_addr_virt     = g_ap_syscnt_regbase_addr_virt;
#else
	g_bbp_systimer_regbase_addr_virt = g_ap_sysctrl_regbase_addr_virt;
#endif
    g_lpm3_tcm_virt             = ioremap_wc(HI_M3TCM0_MEM_ADDR, HI_M3TCM0_MEM_SIZE + HI_M3TCM1_MEM_SIZE);
    g_sram_virt                 = ioremap_wc(HI_SRAM_MEM_BASE_ADDR, HI_SRAM_MEM_SIZE);
    g_modem_sysctrl_base_addr_virt     = ioremap_nocache(HI_SYSCTRL_BASE_ADDR, HI_SYSCTRL_REG_SIZE);
    printk("################ balong_ioremap_init ok! #####################\n");
    printk("ap sysctrl regbase: 		v: %p p: 0x%x ok!\n",g_ap_sysctrl_regbase_addr_virt, HI_AP_SYSCTRL_BASE_ADDR);
    printk("share memory: 		v: %p p: 0x%x ok!\n",g_ddr_share_mem_addr_virt, DDR_SHARED_MEM_ADDR);
    printk("gu    memory: 		v: %p p: 0x%x ok!\n",g_ddr_gu_addr_virt, DDR_GU_ADDR);
    printk("mntn  memory: 		v: %p p: 0x%x ok!\n",g_ddr_mntn_addr_virt, DDR_MNTN_ADDR);
    printk("ipf  regbase: 		v: %p p: 0x%x ok!\n",g_ipf_regbase_addr_virt, HI_IPF_REGBASE_ADDR);
    printk("socp regbase: 		v: %p p: 0x%x ok!\n",g_socp_regbase_addr_virt, HI_SOCP_REGBASE_ADDR);
    printk("ipc  regbase: 		v: %p p: 0x%x ok!\n",g_ipc_regbase_addr_virt, HI_IPCM_REGBASE_ADDR);
    printk("syscnt_regbase: 	        v: %p p: 0x%x ok!\n",g_ap_syscnt_regbase_addr_virt, HI_AP_SYS_CNT_BASE_ADDR);
    printk("lpm3 tcm: 	                v: %p p: 0x%x ok!\n",g_lpm3_tcm_virt, HI_M3TCM0_MEM_ADDR);
    printk("sram: 	                v: %p p: 0x%x ok!\n",g_sram_virt, HI_SRAM_MEM_BASE_ADDR);
    printk("modem sysctrl : 	                v: %p p: 0x%x ok!\n",g_modem_sysctrl_base_addr_virt, HI_SYSCTRL_BASE_ADDR);

    return 0;
}

arch_initcall(balong_ioremap_init);

#ifdef __cplusplus
}
#endif

