#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/pci_regs.h>
#include <linux/mbus.h>
#include <asm/irq.h>
#include <asm/mach/pci.h>
#include <asm/delay.h>
#include <linux/spinlock.h>
#include <asm/signal.h>
#include <mach/pcie.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include "bsp_memmap.h"

#if defined(CONFIG_ARCH_V7R2)
/*v7r2 only support x1*/
static unsigned int pcie_sel = pcie_x1_sel;
#elif defined(CONFIG_ARCH_P531)
/*p531 support x1 and x2*/
static unsigned int pcie_sel = pcie_x1_sel;
#endif

/*sysctrl reg include sc and crg*/
static unsigned long sctrl_addr = (unsigned long)HI_IO_ADDRESS(REG_BASE_SCTRL); 
/*pcie application interface*/
static unsigned long pctrl_addr = (unsigned long)HI_IO_ADDRESS(REG_BASE_PCTRL); 
static unsigned long pcie_cfg_addr = 0;

static u8* gen_str[] = {"GEN1","GEN2","GEN3"};
static u8* mac_phy_rate_str[] = {"2.5GT/s","5GT/s","8GT/s"};

static irqreturn_t pcie_linkdown_isr(s32 irq, void* private);

/*pcie software init*/
static int __init pcie_sw_init(void)
{
	pcie_cfg_addr = (unsigned long)ioremap(REG_BASE_PCIE_CFG,REG_BASE_PCIE_CFG_SIZE);
	if((!sctrl_addr)||(!pctrl_addr)||(!pcie_cfg_addr))
	{
		pcie_error("Cannot map physical base for pcie");
		return -EINVAL;
	}
    
    if(request_irq(INT_LVL_PCIE0_LINK_DOWN, pcie_linkdown_isr,IRQF_DISABLED, "pcie linkdown int", NULL))
    {
        printk(KERN_ERR "pcie linkdown int irq request failed!\n");
        return -1;
    } 
    return 0;
}

static void pcie_clk_enable(void)
{
    unsigned int val;
    
    /*enable pcie clock*/    
#if defined(CONFIG_ARCH_P531)
	val = readl(sctrl_addr  + CLKEN5);
	val |= (0x1 << pcie_clk_en);
	writel(val, sctrl_addr + CLKEN5);
#elif defined(CONFIG_ARCH_V7R2)
    val = readl(sctrl_addr + CLKEN2);
    val |= ((0x1<<pcie_phy_clk_pos)|(pcie_ctrl_clk_mask<<pcie_ctrl_clk_pos));
    val |= (0x1<<soc_peri_usb_pd_clk_pos);
    writel(val,sctrl_addr + CLKEN2);    
#endif
    udelay(100);
}

static inline int pcie_is_link_up(void){
    unsigned int val,xmlh_linkup,rdlh_linkup;
    
    val = readl(pctrl_addr+PCIE0_STAT0);
    xmlh_linkup = (val>>pcie_xmlh_link_up)&0x1;
    rdlh_linkup = (val>>pcie_rdlh_link_up)&0x1; 

    return (xmlh_linkup&&rdlh_linkup) ? 1 : 0;
}

static inline int pcie_ltssm_enable(bool en)
{
    u32 val = 0;

	val = readl(pctrl_addr + PCIE0_CTRL7);
    if(en)
    {
        val |= 1 << pcie_app_ltssm_enable;

    }
    else
    {
	    val &= (~(1 << pcie_app_ltssm_enable));
    }
	writel(val, pctrl_addr + PCIE0_CTRL7);    

#if defined(CONFIG_ARCH_P531)
    if(en)
    {
        int i = 0;
        /*force link mode, to fixed up p531 pcie phy bug*/
        for(i = 0; i < 20; i++)
        {
            if(!pcie_is_link_up())
            {
                dbi_enable(pctrl_addr);
                if(pcie_x1_sel == pcie_sel)
                {
                    writel(0x07028001, pcie_cfg_addr + 0x708);
                }
                else if(pcie_x2_sel == pcie_sel)
                {
                    writel(0x07028002, pcie_cfg_addr + 0x708);
                }
                dbi_disable(pctrl_addr);
            }
        }
    }
#endif 
    return 0;
}

int pcie_phy_cfg_for_asic_debug(void)
{
    unsigned int val;
    val = readl(pctrl_addr  + PCIE_PHY_CTRL0);
    val &= ~(0x1<<22);
    writel(val, pctrl_addr + PCIE_PHY_CTRL0);

    val = readl(pctrl_addr  + PCIE_PHY_CTRL1);
    val &= ~(0x1<<16);
    writel(val, pctrl_addr + PCIE_PHY_CTRL1);    
    return 0;
}

static inline int pcie_phy_ctrl_release(void)
{
    u32 val = 0;
    
#if defined(CONFIG_ARCH_P531)  	
	/*reset pcie ctrl and phy*/
	val = readl(sctrl_addr  + RSTREQ3);
	val |= ((0x1 << pcie_phy_ahb_srst_req) | 
            (0x1 << pcie_phy_por_srst_req) | 
            (0x1f << pcie_phy_srst_req) | 
            (0x1 << pcie_ctrl_srst_req));
	writel(val, sctrl_addr + RSTREQ3);
	udelay(100);

	/*undo reset pcie phy*/
	val = readl(sctrl_addr  + RSTREQ3);
	val &= ~((0x1 << pcie_phy_ahb_srst_req) | 
             (0x1 << pcie_phy_por_srst_req) | 
             (0x3f << pcie_phy_srst_req));
	writel(val, sctrl_addr + RSTREQ3);

    /*check phy status*/
    while(readl(sctrl_addr + PCIE0_PHY_STAT)&0xc0000000)
    {
      udelay(100);
    }
    
	/*undo reset pcie ctrl*/
	val = readl(sctrl_addr  + RSTREQ3);
	val &= ~(0x1 << pcie_ctrl_srst_req);
	writel(val, sctrl_addr + RSTREQ3);
    udelay(1000);
#elif defined(CONFIG_ARCH_V7R2)
    /*reset pcie ctrl and phy*/
    val = readl(sctrl_addr  + SRSTEN2);
	val |= ((0x1 << pcie_phy_srst_pos) | (0x1 << pcie_ctrl_srst_pos));
	writel(val, sctrl_addr + SRSTEN2);
    
    /*unreset pcie phy*/
    val = readl(sctrl_addr  + SRSTDIS2);
	val |= (0x1 << pcie_phy_srst_pos);
    val |= (0x1 << soc_peri_usb_pd_srst_pos);
	writel(val, sctrl_addr + SRSTDIS2);    
    udelay(100);
    pcie_phy_cfg_for_asic_debug();
    
    /*unreset pcie ctrl*/
    val = readl(sctrl_addr  + SRSTDIS2);
	val |= (0x1 << pcie_ctrl_srst_pos);
	writel(val, sctrl_addr + SRSTDIS2);     
    udelay(100);
    
#if defined(BSP_CONFIG_V7R2_SFT)
    /*tensi_dps0_pd_srst_dis*/
    val = readl(sctrl_addr  + SRSTDIS2);
	val |= 0x1;
	writel(val, sctrl_addr + SRSTDIS2);     
    udelay(100);
#endif
    printk(KERN_ERR "wait pcie phy reset ready...\n");
    for(;;)
    {
        val = readl(pctrl_addr  + PCIE0_PHY_STAT);
        val = (val>>19)&0x1;
        if(val == 0)
        {
            break;
        }
    }
    printk(KERN_ERR "pcie phy reset ready.\n");
#endif 
    return 0;
}
static inline int pcie_hw_preinit(void)
{
	unsigned int val;
	unsigned int class_code;
        
	/*set work mode*/
	val = readl(pctrl_addr + PCIE0_CTRL0);
	val &=~(0xf << pcie_device_type);
	val |= (pcie_wm_ep << pcie_device_type);
	writel(val, pctrl_addr + PCIE0_CTRL0);

    dbi_enable(pctrl_addr);

	/*setup correct class code*/
#if defined(CONFIG_BALONG_PCIE_DOCKING_TEST)
    class_code = PCI_CLASS_DOCKING_GENERIC<<8;
#else
    class_code = PCI_CLASS_COMMUNICATION_MODEM<<8;
#endif
    val = readl(pcie_cfg_addr + 0x8);
	val &= ~(0xffffff00);
	val |= (class_code<<8);
    writel(val, pcie_cfg_addr + 0x8);

	val = readl(pcie_cfg_addr + 0x710);
    if(pcie_x1_sel == pcie_sel)
    {
        val &= ~(0x3f<<16);
        val |= 0x1<<16;
    }
    else if(pcie_x2_sel == pcie_sel)
    {
        val &= ~(0x3f<<16);
        val |= 0x3<<16;
    }
    writel(val, pcie_cfg_addr + 0x710);

    val = readl(pcie_cfg_addr + 0x700+ 0x10c);
    if(pcie_x1_sel == pcie_sel)
    {
        val &= ~(0x1ff<<8);
        val |= 0x1<<8;
    }
    else if(pcie_x2_sel == pcie_sel)
    {
        val &= ~(0x1ff<<8);
        val |= 0x2<<8;
    }
    writel(val, pcie_cfg_addr + 0x700+ 0x10c);
    
#if defined(CONFIG_ARCH_V7R2)
    /*
    ASPM Setting reference: 
    http://wireless.kernel.org/en/users/Documentation/ASPM
    */
    /*Active State Link PM Control: 10b L1 Entry Enabled*/
    val = readl(pcie_cfg_addr + 0x80);
    val |= 2;
    writel(val, pcie_cfg_addr + 0x80);
  
    /*Enter ASPM L1 Without Receive In L0s*/
    val = readl(pcie_cfg_addr + 0x70c);
    val |= 1<<30;
    writel(val, pcie_cfg_addr + 0x70c);


#if defined(BSP_CONFIG_V7R2_SFT)
    /*
    set DEFAULT_EP_L0S_ACCPT_LATENCY and DEFAULT_EP_L1_ACCPT_LATENCY to right 
    values.
    DEFAULT_EP_L0S_ACCPT_LATENCY:0x4
    DEFAULT_EP_L1_ACCPT_LATENCY:0x4.
    the values are drived from retrain-link,equal to the re-calcuated values 
    of l0s and l1 latency exit .
    Warning: the values are dependent on platform and just for test aim.
    */
    val = readl(pcie_cfg_addr + 0x74);
    val |= 0x900;
    writel(val, pcie_cfg_addr + 0x74);
#endif

    /*set DEFAULT_CLK_PM_CAP to 1*/
    val = readl(pcie_cfg_addr + 0x7c);
    val |= 1<<18;
    writel(val, pcie_cfg_addr + 0x7c); 

    /*Enalbe Clock PM*/
    val = readl(pcie_cfg_addr + 0x80);
    val |= 1<<8;
    writel(val, pcie_cfg_addr + 0x80);
#endif    
    /*modify bar0 to non-prefetchmem*/
    writel(0x0,pcie_cfg_addr + PCI_BASE_ADDRESS_0);
    
    /*modify bar2 to non-prefetchmem*/
    writel(0x0,pcie_cfg_addr + PCI_BASE_ADDRESS_2);
    
#if defined(CONFIG_ARCH_P531)
    #if defined(CONFIG_BALONG_PCIE_DOCKING_TEST)
    /*set bar0 size to 16MB*/
    writel(0xffffff,pcie_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_0));
    /*set bar2 size to 16MB*/
    writel(0xffffff,pcie_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_2));

    #else
    /*set bar0 size to 32MB for sram/ddr*/
    writel(0x1ffffff,pcie_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_0));

    /*enable bar1,we can't resize the bar(default size is 64K)*/
    writel(0xffff,pcie_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_1)); 
    
    /*set bar2 size to 2M for p531 reg*/
    writel(0x1fffff,pcie_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_2)); 

    /*we can't use bar3*/
    
    /*modify bar4 to non-prefetch mem*/
    writel(0x0,pcie_cfg_addr + PCI_BASE_ADDRESS_4);
    /*set bar4 size to 16M for fpga reg*/
    writel(0xffffff,pcie_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_4));
    
    /*modify bar5 to non-prefetch mem*/
    writel(0x0,pcie_cfg_addr + PCI_BASE_ADDRESS_5);
    /*disable bar5, we can't resize the bar(default size is 64K)*/
    writel(0x0,pcie_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_5));     
    #endif
#endif
    
    /*
    set subsystem vender id to 0x1 for distingushing hisilicon rc or ep.
    rc ss vendor id is 0.
    ep ss vendor id is 1.
    */
    writel(0x1,pcie_cfg_addr + PCI_SUBSYSTEM_VENDOR_ID);   
    dbi_disable(pctrl_addr);
    return 0;
}
static irqreturn_t pcie_linkdown_isr(s32 irq, void* private)
{
    int val = 0;
    
    writel(0x3<<11,pctrl_addr + PCIE0_CTRL11);
#if defined(BSP_CONFIG_V7R2_SFT)
    __delay(4000);
#endif
    val = readl(pctrl_addr + PCIE0_CTRL11);
    val &=(~(0x3<<11));
    writel(val,pctrl_addr + PCIE0_CTRL11);
    
    printk(KERN_ERR "pcie linkdown irq(%d) rising.\n",irq);  
    pcie_hw_preinit();
    pcie_ltssm_enable(true);

    return IRQ_HANDLED;
}

static inline void pcie_show_link_status(void)
{
	unsigned int val,link_width,link_speed; 
    
    /*wait rc set Directed Speed Change, if rc do it.*/
    udelay(100);
    
    dbi_enable(pctrl_addr);
    val = readl(pcie_cfg_addr + 0x70 + 0x10);
    dbi_disable(pctrl_addr);
    /*negotiated link width*/
    link_width = (val >> 20)&0x3f;
    /*link speed*/
    link_speed = ((val >> 16)&0xf)/2;
        
    printk(KERN_ALERT "PCI: Balong Pcie Now Support Bus Spec:%s," 
        "Single Lane Peer Rate:%s,Lane Num:x%d.",
        gen_str[link_speed],mac_phy_rate_str[link_speed],link_width);    
}

/*pcie hardware init*/
static int __init pcie_hw_init(void){
#if defined(CONFIG_ARCH_V7R2)
    /*pcie phy pre-cfg*/
    writel(0x1500024b, sctrl_addr + HI_SC_CTRL111_OFFSET);
    writel(0x15003fff, sctrl_addr + HI_SC_CTRL112_OFFSET);
#endif    
    pcie_clk_enable();
    pcie_ltssm_enable(false);
    pcie_phy_ctrl_release();
    pcie_hw_preinit();
    pcie_ltssm_enable(true);
    /*check ltssm state*/
    printk(KERN_ALERT "PCI: Balong Pcie Core Waiting Root Complex Enable Ltssm...");
    while(!pcie_is_link_up())
    {
        udelay(100);
    } 
    printk(KERN_ALERT "PCI: Balong Pcie Link Up.");

    pcie_show_link_status();
	return 0;
}

static int __init balong_pcie_init(void){
	printk(KERN_ERR "PCI: Balong Pcie Core Work Mode: End Point.\n");

	if(pcie_sw_init()){
		return -EIO;
	} 
    
	if(pcie_hw_init()){
		return -EIO;
	}    
	return 0;
}
subsys_initcall(balong_pcie_init);

