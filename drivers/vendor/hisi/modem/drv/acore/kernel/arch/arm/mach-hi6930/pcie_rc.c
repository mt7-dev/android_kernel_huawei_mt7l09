#include <linux/kernel.h>
#include <linux/export.h>
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
#include <linux/irqnr.h>
#include <linux/msi.h>
#include <linux/regulator/consumer.h>

#include "bsp_memmap.h"
#include "soc_interrupts.h"

static struct pcie_info pcie_info[1];

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
static unsigned long pcie_data_addr = 0;
struct regulator *pcie_regulator = NULL;

static u8* gen_str[] = {"GEN1","GEN2","GEN3"};
static u8* mac_phy_rate_str[] = {"2.5GT/s","5GT/s","8GT/s"};

/*pcie0 iatu table*/
struct pcie_iatu pcie0_iatu_table[] = {
	/*config, type 0*/
	{
        /*bit31:0 outbound , bit[3:0]:0 region index 0*/
		.viewport		= 0,
		/*region type, 4:config type0, 2:IO, 0:MEM*/
		.region_ctrl_1	= 0x00000004,
		/*bit28:1 cfg shift mode enable, bit31:1 region enable*/
		.region_ctrl_2	= 0x90000000,
		/*(bus:0,dev:0,func:0)*/
		.lbar			= PCIE0_TYPE0_CFG_SPACE_START,
		.ubar			= 0x0,
		.lar			= PCIE0_TYPE0_CFG_SPACE_END,
		.ltar			= 0x00000000,
		.utar			= 0x00000000,
	},
	/*config, type 1*/
	{
        /*bit31:0 outbound , bit[3:0]:0 region index 1*/
		.viewport		= 1,
		/*region type, 5:config type1, 2:IO, 0:MEM*/
		.region_ctrl_1	= 0x00000005,
		/*bit28:1 cfg shift mode enable, bit31:1 region enable*/
		.region_ctrl_2	= 0x90000000,
		/*(bus:0,dev:0,func:0)*/
		.lbar			= PCIE0_TYPE1_CFG_SPACE_START,
		.ubar			= 0x0,
		.lar			= PCIE0_TYPE1_CFG_SPACE_END,
		.ltar			= 0x00000000,
		.utar			= 0x00000000,
	},
	/*IO*/
	{
        /*bit31:0 outbound , bit[3:0]:0 region index 2*/
		.viewport		= 2,
		/*region type, 4:config, 2:IO, 0:MEM*/
		.region_ctrl_1	= 0x00000002,
		.region_ctrl_2	= 0x80000000,
		.lbar			= PCIE0_IO_SPACE_START,
		.ubar			= 0x0,
		.lar			= PCIE0_IO_SPACE_END,
		.ltar			= 0x00001000,
		.utar			= 0x00000000,
	},
	/*Memory*/
	{
        /*bit31:0 outbound , bit[3:0]:0 region index 3*/
		.viewport		= 3,
		/*region type, 4:config, 2:IO, 0:MEM*/
		.region_ctrl_1	= 0x00000000,
		.region_ctrl_2	= 0x80000000,
		.lbar			= PCIE0_MEM_SPACE0_START,
		.ubar			= 0x0,
		.lar			= PCIE0_MEM_SPACE0_END,
		.ltar			= PCIE0_MEM_SPACE0_START,
		.utar			= 0x00000000,
	},
#if defined(CONFIG_ARCH_P531)
	/*Memory*/
	{
        /*bit31:0 outbound , bit[3:0]:0 region index 3*/
		.viewport		= 4,
		/*region type, 4:config, 2:IO, 0:MEM*/
		.region_ctrl_1	= 0x00000000,
		.region_ctrl_2	= 0x80000000,
		.lbar			= PCIE0_MEM_SPACE1_START,
		.ubar			= 0x0,
		.lar			= PCIE0_MEM_SPACE1_END,
		.ltar			= PCIE0_MEM_SPACE1_START,
		.utar			= 0x00000000,
	},
#endif
#if defined(CONFIG_PCI_MSI)
	/*Memory Inbound for MSI*/
	{
        /*bit31:1 inbound , bit[3:0]:0 region index 0*/
		.viewport		= 0x80000000,
		/*region type, 4:config, 2:IO, 0:MEM*/
		.region_ctrl_1	= 0x00000000,
		.region_ctrl_2	= 0x80000000,
		.lbar			= MSI_PCIE_LOWER_ADDR,
		.ubar			= MSI_PCIE_UPPER_ADDR,
		.lar			= (MSI_PCIE_LOWER_ADDR + SZ_4K - 1),
		.ltar			= MSI_LOCAL_LOWER_ADDR,
		.utar			= MSI_LOCAL_UPPER_ADDR,
	},
#endif
};

static int __init balong_pcie_setup(int nr, struct pci_sys_data* sys);
static struct pci_bus *__init balong_pcie_scan_bus(int nr, struct pci_sys_data* sys);
static int __init balong_pcie_map_irq(const struct pci_dev* dev, u8 slot, u8 pin);

static struct hw_pci balong_pcie = {
	.nr_controllers = 0,        /*default 1 controllers*/
	.swizzle           = pci_std_swizzle,	/*useless??*/
	.setup             = balong_pcie_setup,
	.scan               = balong_pcie_scan_bus,
	.map_irq          = balong_pcie_map_irq,
};

volatile unsigned int  pcie_errorvalue=0;

static int balong_pci_fault(unsigned long addr, unsigned int fsr, struct pt_regs *regs)
{
	pcie_errorvalue=1;
	return 0;
}

/*locks: read lock & write lock.*/
static DEFINE_SPINLOCK(rlock);
static DEFINE_SPINLOCK(wlock);

/*
pcie sel boot args format: pcie_sel=x1 or x2 (p531 support x1 and x2; v7r2 only support x1).
 */
static int __init pcie_sel_parser(char* str){
	if(strncasecmp(str, "x1",2) == 0)
		pcie_sel = pcie_x1_sel;
	else if(strncasecmp(str, "x2",2) == 0)
        pcie_sel = pcie_x2_sel;
    else
		pcie_sel = pcie_x1_sel;

	return 1;
}
__setup("pcie_sel=", pcie_sel_parser);


#if defined(CONFIG_PCI_MSI)
static int msi_irq_base = MSI_IRQ_BASE;
static int msi_irq_num  = MSI_NR_IRQS;
static int msi_irq      = INT_LVL_PCIE0_RADM_B;

static DECLARE_BITMAP(msi_irq_bits, CFG_MAX_MSI_NUM);

/**
 * balong_msi_handler() - Handle MSI interrupt
 * @irq: IRQ line for MSI interrupts
 * @desc: Pointer to irq descriptor
 *
 * Traverse through pending MSI interrupts and invoke handler for each. Also
 * takes care of interrupt controller level mask/ack operation.
 */
static u32 balong_msi_int_count = 0;
void show_msi_int_count(void)
{
    printk(KERN_ERR "balong_msi_int_count:%d.\n",balong_msi_int_count);
}
static void balong_msi_handler(unsigned int irq, struct irq_desc *desc)
{
	int bit = 0;
	u32 status;

	/*pcie_debug(PCIE_DEBUG_LEVEL, "Handling MSI irq %d\n", irq);*/
    /*printk(KERN_ERR "PCI RC:Handling MSI irq %d.\n", irq);*/

	/*
	 * The chained irq handler installation would have replaced normal
	 * interrupt driver handler so we need to take care of mask/unmask and
	 * ack operation.
	 *
	 */
	balong_msi_int_count++;
	desc->irq_data.chip->irq_mask(&(desc->irq_data));
	dbi_enable(pctrl_addr);
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
	status = readl(pcie_cfg_addr + MSIC_IRQ_STATUS(0));

	/* FIXME: Use max loops count? */
	while (status) {
		bit = find_first_bit((unsigned long *)&status, 32);
		generic_handle_irq(msi_irq_base + bit);
        dbi_enable(pctrl_addr);
		status = readl(pcie_cfg_addr + MSIC_IRQ_STATUS(0));
	}
#elif defined(MSI_SUPPORT_1_DEV_32_BIT)
    for(bit = 0; bit < CFG_MAX_MSI_NUM; bit++)
    {
        dbi_enable(pctrl_addr);
        status = readl(pcie_cfg_addr + MSIC_IRQ_STATUS(bit));
        if(status)
        {
            generic_handle_irq(msi_irq_base + bit);
        }
    }
#endif
    dbi_disable(pctrl_addr);
    desc->irq_data.chip->irq_eoi(&(desc->irq_data));
    desc->irq_data.chip->irq_unmask(&(desc->irq_data));
}


static void msi_ack(struct irq_data *data)
{
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
	u32 msi_num = data->irq - msi_irq_base;
    dbi_enable(pctrl_addr);
	writel((1 << (msi_num & 0x1f)), pcie_cfg_addr + MSIC_IRQ_STATUS(0));
    dbi_disable(pctrl_addr);
#elif defined(MSI_SUPPORT_1_DEV_32_BIT)
    /*clear interrupt by user isr,not here.*/
#endif
}

static void msi_mask(struct irq_data *data)
{
	u32 msi_num = data->irq - msi_irq_base;
    u32 val = 0;

    dbi_enable(pctrl_addr);
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
    val = readl(pcie_cfg_addr + MSIC_IRQ_STATUS(0));
	writel(val, pcie_cfg_addr + MSIC_IRQ_MASK(0));
#elif defined(MSI_SUPPORT_1_DEV_32_BIT)
    val = readl(pcie_cfg_addr + MSIC_IRQ_STATUS(msi_num));
    writel(val,pcie_cfg_addr + MSIC_IRQ_MASK(msi_num));
#endif
    dbi_disable(pctrl_addr);
}

static void msi_unmask(struct irq_data *data)
{
	u32 msi_num = data->irq - msi_irq_base;
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
    u32 val = 0;
#endif

    dbi_enable(pctrl_addr);
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
    val = readl(pcie_cfg_addr + MSIC_IRQ_MASK(0));
    val &= ~(1 << (msi_num & 0x1f));
	writel(val, pcie_cfg_addr + MSIC_IRQ_MASK(0));
#elif defined(MSI_SUPPORT_1_DEV_32_BIT)
    writel(0x0,pcie_cfg_addr + MSIC_IRQ_MASK(msi_num));
#endif
    dbi_disable(pctrl_addr);
}

static void msi_enable(struct irq_data *data)
{
	u32 msi_num = data->irq - msi_irq_base;
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
    u32 val = 0;
#endif

    dbi_enable(pctrl_addr);
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
    val = readl(pcie_cfg_addr + MSIC_IRQ_ENABLE(0));
    val |= 1 << (msi_num & 0x1f);
	writel(val, pcie_cfg_addr + MSIC_IRQ_ENABLE(0));
#elif defined(MSI_SUPPORT_1_DEV_32_BIT)
	writel(0xffffffff, pcie_cfg_addr + MSIC_IRQ_ENABLE(msi_num));
#endif
    dbi_disable(pctrl_addr);
}
static void msi_disable(struct irq_data *data)
{
	u32 msi_num = data->irq - msi_irq_base;
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
    u32 val = 0;
#endif

    dbi_enable(pctrl_addr);
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
    val = readl(pcie_cfg_addr + MSIC_IRQ_ENABLE(0));
    val &= ~(1 << (msi_num & 0x1f));
	writel(val, pcie_cfg_addr + MSIC_IRQ_ENABLE(0));
#elif defined(MSI_SUPPORT_1_DEV_32_BIT)
    writel(0, pcie_cfg_addr + MSIC_IRQ_ENABLE(msi_num));
#endif
    dbi_disable(pctrl_addr);
}
/*
 * Note: mask/unmask on remote devices is NOT supported (mask_msi_irq and
 * unmask_msi_irq through mask bits capability on endpoints.
 */

static struct irq_chip balong_msi_chip = {
	.name = "PCIe-MSI",
    .irq_enable = msi_enable,
    .irq_disable = msi_disable,
	.irq_ack = msi_ack,
	.irq_mask = msi_mask,
	.irq_unmask = msi_unmask,
};

/**
 * get_free_msi() - Get a free MSI number
 *
 * Checks for availability of MSI and returns the first available.
 */
static int get_free_msi(void)
{
	int bit;

	do {
		bit = find_first_zero_bit(msi_irq_bits, msi_irq_num);

		if (bit >= msi_irq_num)
			return -ENOSPC;

	} while (test_and_set_bit(bit, msi_irq_bits));

	printk(KERN_ERR "MSI %d available\n", bit);

	return bit;
}

/**
 * arch_setup_msi_irq() - Set up an MSI for Endpoint
 * @pdev: Pointer to PCI device structure of requesting EP
 * @desc: Pointer to MSI descriptor data
 *
 * Assigns an MSI to endpoint and sets up corresponding irq. Also passes the MSI
 * information to the endpont.
 */
int arch_setup_msi_irq(struct pci_dev *pdev, struct msi_desc *desc)
{
	int ret, irq;
	struct msi_msg msg;

	if (msi_irq < 0) {
		printk(KERN_ERR "PCI: MSI irq pin not specified\n");
		return msi_irq;
	}

	ret = get_free_msi();
	if (ret < 0) {
		printk(KERN_ERR "PCI: Failed to get free MSI\n");
	} else {
		irq = msi_irq_base + ret;
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
		msg.data = ret;
#elif defined(MSI_SUPPORT_1_DEV_32_BIT)
        msg.data = (ret&0x7)<<5;
#endif
		dynamic_irq_init(irq);
		ret = irq_set_msi_desc(irq, desc);

		if (!ret) {
			msg.address_hi = MSI_LOCAL_UPPER_ADDR;
			msg.address_lo = MSI_LOCAL_LOWER_ADDR;
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
			printk(KERN_ERR "PCI:MSI %d @%x:%x, irq = %d\n",
					msg.data, msg.address_hi,
					msg.address_lo, irq);
#elif defined(MSI_SUPPORT_1_DEV_32_BIT)
			printk(KERN_ERR "PCI:MSI#%d[0-31] @%x:%x, irq = %d\n",
					(msg.data>>5)&0x7, msg.address_hi,
					msg.address_lo, irq);
#endif
			write_msi_msg(irq, &msg);
			irq_set_chip_and_handler_name(irq, &balong_msi_chip,
						handle_level_irq,NULL);
			set_irq_flags(irq, IRQF_VALID);
		}
	}

	return ret;
}

void arch_teardown_msi_irq(unsigned int irq)
{
	int pos = irq - msi_irq_base;

	dynamic_irq_cleanup(irq);
	clear_bit(pos, msi_irq_bits);
}
#endif

static irqreturn_t pcie_linkdown_isr(s32 irq, void* private);

static int __init pcie_sw_init(void)
{
	if(balong_pcie.nr_controllers){
		pcie_error("inavild param!\n");
		return -EIO;
	}

    pcie_regulator = regulator_get(NULL, "pcie_mtcmos-vcc");
    if (NULL == pcie_regulator)
    {
		pcie_error("fail to get regulator\n");
		return -EIO;
    }

    if (regulator_enable(pcie_regulator))
    {
		pcie_error("fail to enable regulator\n");
		return -EIO;
    }

    /*rc config space*/
	pcie_cfg_addr = (unsigned long)ioremap(REG_BASE_PCIE_CFG,
	                 REG_BASE_PCIE_CFG_SIZE);
    /*
     data space
     just ioremap device config space,other space need driver do it
    */
    pcie_data_addr = (unsigned long)ioremap(PCIE0_TYPE0_CFG_SPACE_START,
                    PCIE0_TYPE0_CFG_SPACE_SIZE + PCIE0_TYPE1_CFG_SPACE_SIZE);

	if((!sctrl_addr)||(!pctrl_addr)||(!pcie_cfg_addr)||(!pcie_data_addr))
	{
		pcie_error("Cannot map physical base for pcie");
		return -EIO;
	}

	pcie_info[balong_pcie.nr_controllers].root_bus_nr = -1;
	pcie_info[balong_pcie.nr_controllers].controller = pcie_controller_0;
	pcie_info[balong_pcie.nr_controllers].conf_base_addr =
                                         (unsigned long)pcie_cfg_addr;
	pcie_info[balong_pcie.nr_controllers].base_addr =
                                         (unsigned long)pcie_data_addr;
	printk(KERN_ERR "PCIe base addr 0x%x, cfg addr 0x%x\n",
                    pcie_info[balong_pcie.nr_controllers].base_addr,
                    pcie_info[balong_pcie.nr_controllers].conf_base_addr);

	balong_pcie.nr_controllers++;

#if defined(CONFIG_PCI_MSI)
    /*setup msi irq handler*/
    irq_set_chained_handler(INT_LVL_PCIE0_RADM_B, balong_msi_handler);
#endif

	return 0;
}


int pcie_clk_enable(void)
{
    unsigned int val;

#if defined(CONFIG_ARCH_P531)
	val = readl(sctrl_addr  + CLKEN5);
	val |= (0x1 << pcie_clk_en);
	writel(val, sctrl_addr + CLKEN5);
    udelay(100);
#elif defined(CONFIG_ARCH_V7R2)
    val = readl(sctrl_addr + CLKEN2);
    val |= ((0x1<<pcie_phy_clk_pos)|(pcie_ctrl_clk_mask<<pcie_ctrl_clk_pos));
    val |= (0x1<<soc_peri_usb_pd_clk_pos);
    writel(val,sctrl_addr + CLKEN2);
#endif
    udelay(100);

    return 0;
}
EXPORT_SYMBOL(pcie_clk_enable);

int pcie_clk_disable(void)
{
    unsigned int val;

#if defined(CONFIG_ARCH_P531)
	val = readl(sctrl_addr  + CLKDIS5);
	val |= (0x1 << pcie_clk_dis);
	writel(val, sctrl_addr + CLKDIS5);
    udelay(100);
#elif defined(CONFIG_ARCH_V7R2)
    val = readl(sctrl_addr + CLKDIS2);
    val |= ((0x1<<pcie_phy_clk_pos)|(pcie_ctrl_clk_mask<<pcie_ctrl_clk_pos));
    val |= (0x1<<soc_peri_usb_pd_clk_pos);
    writel(val,sctrl_addr + CLKDIS2);
#endif
    udelay(100);

    return 0;
}
EXPORT_SYMBOL(pcie_clk_disable);

int pcie_ltssm_enable(bool en)
{
    unsigned int val;

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
    /*
      force link mode,
      to fixed up p531 pcie phy bug
    */
    if(en)
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
#endif
    return 0;
}
EXPORT_SYMBOL(pcie_ltssm_enable);

int pcie_phy_ctrl_reset(void)
{
    unsigned int val;

	/*reset pcie ctrl and phy*/
#if defined(CONFIG_ARCH_P531)
	val = readl(sctrl_addr  + RSTREQ3);
	val |= ((0x1 << pcie_phy_ahb_srst_req) |
            (0x1 << pcie_phy_por_srst_req) |
            (0x1f << pcie_phy_srst_req) |
            (0x1 << pcie_ctrl_srst_req));
	writel(val, sctrl_addr + RSTREQ3);
#elif defined(CONFIG_ARCH_V7R2)
    val = readl(sctrl_addr  + SRSTEN2);
	val |= ((0x1 << pcie_phy_srst_pos) | (0x1 << pcie_ctrl_srst_pos));
#if defined(BSP_CONFIG_V7R2_SFT)
    /*tensi_dps0_pd_srst_en*/
	val |= 0x1;
#endif
	writel(val, sctrl_addr + SRSTEN2);
#endif

    udelay(100);

    return 0;
}
EXPORT_SYMBOL(pcie_phy_ctrl_reset);

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
int pcie_phy_ctrl_undo_reset(void)
{
    unsigned int val;

#if defined(CONFIG_ARCH_P531)
	/*undo reset pcie ctrl and phy*/
	val = readl(sctrl_addr  + RSTREQ3);
	val &= ~((0x1 << pcie_phy_ahb_srst_req) |
             (0x1 << pcie_phy_por_srst_req) |
             (0x3f << pcie_phy_srst_req) |
             (0x1 << pcie_ctrl_srst_req));
	writel(val, sctrl_addr + RSTREQ3);
	udelay(1000);
#elif defined(CONFIG_ARCH_V7R2)
    /*undo reset pcie phy*/
    val = readl(sctrl_addr  + SRSTDIS2);
	val |= (0x1 << pcie_phy_srst_pos);
    val |= (0x1 << soc_peri_usb_pd_srst_pos);
	writel(val, sctrl_addr + SRSTDIS2);
    udelay(100);

    pcie_phy_cfg_for_asic_debug();

    /*undo reset pcie ctrl*/
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
EXPORT_SYMBOL(pcie_phy_ctrl_undo_reset);

int pcie_phy_ctrl_release(void)
{
    pcie_phy_ctrl_reset();
    pcie_phy_ctrl_undo_reset();
    return 0;
}
EXPORT_SYMBOL(pcie_phy_ctrl_release);

/*pcie controller register init*/
int pcie_hw_preinit(void)
{
	unsigned int val;
	int i;

	/*set work mode*/
	val = readl(pctrl_addr + PCIE0_CTRL0);
	val &=~(0xf << pcie_device_type);
	val |= (pcie_wm_rc << pcie_device_type);
	writel(val, pctrl_addr + PCIE0_CTRL0);

    dbi_enable(pctrl_addr);

	/*setup correct classe code*/
    val = readl(pcie_cfg_addr + 0x8);
	val &= ~(0xffffff00);
	val |= ((PCI_CLASS_BRIDGE_PCI<<8)<<8);
    writel(val, pcie_cfg_addr + 0x8);

    /*set x1 or x2*/
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
#endif

    /*disable bar0 and set bar0 len = 0*/
    writel(0x0,pcie_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_0));

	/*setup iatu table*/
	for( i = 0; i < ARRAY_SIZE(pcie0_iatu_table); i++){
	    writel(pcie0_iatu_table[i].viewport, pcie_cfg_addr + IATU_IDX);
	    writel(pcie0_iatu_table[i].region_ctrl_1, pcie_cfg_addr + IATU_CTRL1);
	    writel(pcie0_iatu_table[i].lbar, pcie_cfg_addr + IATU_LOWER_ADDR);
	    writel(pcie0_iatu_table[i].ubar, pcie_cfg_addr + IATU_UPPER_ADDR);
	    writel(pcie0_iatu_table[i].lar, pcie_cfg_addr + IATU_LIMIT_ADDR);
	    writel(pcie0_iatu_table[i].ltar, pcie_cfg_addr + IATU_LTAR_ADDR);
	    writel(pcie0_iatu_table[i].utar, pcie_cfg_addr + IATU_UTAR_ADDR);
	    writel(pcie0_iatu_table[i].region_ctrl_2, pcie_cfg_addr + IATU_CTRL2);
	}

#if defined(CONFIG_PCI_MSI)
    /*setup msi addr for msi interrupt controller*/
    writel(MSI_LOCAL_LOWER_ADDR,pcie_cfg_addr + MSIC_LOWER_ADDR);
    writel(MSI_LOCAL_UPPER_ADDR,pcie_cfg_addr + MSIC_UPPER_ADDR);
#endif

    dbi_disable(pctrl_addr);

    return 0;
}
EXPORT_SYMBOL(pcie_hw_preinit);

int pcie_is_link_up(void){
    unsigned int val,xmlh_linkup,rdlh_linkup;

    val = readl(pctrl_addr+PCIE0_STAT0);
    xmlh_linkup = (val>>pcie_xmlh_link_up)&0x1;
    rdlh_linkup = (val>>pcie_rdlh_link_up)&0x1;

    return (xmlh_linkup&&rdlh_linkup) ? 1 : 0;
}
EXPORT_SYMBOL(pcie_is_link_up);

int pcie_link_up_confirm(void)
{
    int i = 0;

    for(i=0;i<TIME_TO_WAIT;i++)
	{
		udelay(1000);
		if(pcie_is_link_up()) {
			printk(KERN_ALERT "PCI: Balong Pcie Link Up.\n");
			break;
		}
	}

	if(!pcie_is_link_up())
	{
		printk(KERN_ALERT "PCI: The Pci Device Isn't Online,Please Check It.\n");
		return -EIO;
	}

    return 0;
}
EXPORT_SYMBOL(pcie_link_up_confirm);

static inline void pcie_show_link_status(void)
{
    unsigned int val,link_speed = 0,link_width = 0;

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

int pcie_hw_postinit(void)
{
	unsigned int val,link_speed = 0;

    dbi_enable(pctrl_addr);
    /*confirm current speed,if not gen2, direct speed change*/
    val = readl(pcie_cfg_addr + 0x70 + 0x10);
    link_speed = ((val >> 16)&0xf);
    if(link_speed != 2)
    {
        printk(KERN_ERR "PCI: Current link speed is %s\n",mac_phy_rate_str[link_speed/2]);
        /*Directed Speed Change*/
        printk(KERN_ERR "PCI: Now set directed speed change bit for GEN2.\n");
        val = readl(pcie_cfg_addr + 0x700 + 0x10c);
        val |= 1<<17;
        writel(val, pcie_cfg_addr + 0x700 + 0x10c);
        udelay(150);
		val = readl(pcie_cfg_addr + 0x70 + 0x10);
    	link_speed = ((val >> 16)&0xf);
		if(link_speed != 2)
		{
			printk(KERN_ERR "PCI: Directed speed change is rejected, keep the beginning speed.\n");
		}
    }
    dbi_disable(pctrl_addr);

    return 0;
}
EXPORT_SYMBOL(pcie_hw_postinit);

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
    /*
    if linkdown irq is rising, the pcie core is reset.
    we have to re-initialize pcie core and enable ltssm.
    */
    /*
    pcie_hw_preinit();
    pcie_ltssm_enable(true);
    */
    return IRQ_HANDLED;
}

static int __init pcie_hw_init(void)
{
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
    if(pcie_link_up_confirm())
    {
        return -EIO;
    }
    pcie_hw_postinit();
    pcie_show_link_status();
    return 0;
}

static struct resource pcie_mem_space0 = {
	.name   = "pcie memory space0",
	.start  = PCIE0_MEM_SPACE0_START,
	.end    = PCIE0_MEM_SPACE0_END,
	.flags  = IORESOURCE_MEM,
};
static struct resource pcie_mem_space1 = {
	.name   = "pcie memory space1",
	.start  = PCIE0_MEM_SPACE1_START,
	.end    = PCIE0_MEM_SPACE1_END,
	.flags  = IORESOURCE_MEM,
};
static struct resource pcie_io_space = {
	.name   = "pcie io space",
	.start  = PCIE0_IO_SPACE_START,
	.end    = PCIE0_IO_SPACE_END,
	.flags  = IORESOURCE_IO,
};

static int __init balong_pcie_setup(int nr, struct pci_sys_data* sys)
{
	struct pcie_info* info;
	int ret;

	if(nr >= balong_pcie.nr_controllers)
		return 0;

	info = &pcie_info[nr];
	/*record busnr for cfg ops use*/
	info->root_bus_nr = sys->busnr;
	sys->mem_offset = 0;
    sys->io_offset = 0;
    sys->private_data =(void*)info;

	/*requeset resources for corresponding controller*/
	if(info->controller == pcie_controller_0){
		ret = request_resource(&ioport_resource, &pcie_io_space);
		if(ret){
			pcie_error("Cannot request io resource for pcie,"
                       "pcie_io_space.start=0x%x,end:0x%x\n",
                       pcie_io_space.start,
                       pcie_io_space.end);
			return ret;
		}
		ret = request_resource(&iomem_resource, &pcie_mem_space0);
		if(ret){
			pcie_error("Cannot request mem resource 0 for pcie0");
			release_resource(&pcie_io_space);
			return ret;
		}
		ret = request_resource(&iomem_resource, &pcie_mem_space1);
		if(ret){
			pcie_error("Cannot request mem resource1 for pcie0");
			release_resource(&pcie_io_space);
            release_resource(&pcie_mem_space0);
			return ret;
		}
        pci_add_resource_offset(&sys->resources, &pcie_io_space, sys->io_offset);
        pci_add_resource_offset(&sys->resources, &pcie_mem_space0, sys->mem_offset);
        pci_add_resource_offset(&sys->resources, &pcie_mem_space1, sys->mem_offset);
	}

	return 1;
}

static struct pcie_info* bus_to_info(int busnr){
	int i = balong_pcie.nr_controllers, j=0;
	for( j=i-1 ; i > 0; i--,j=i-1 ){
		if(pcie_info[j].controller != pcie_controller_none
				&& pcie_info[j].root_bus_nr <= busnr
				&& pcie_info[j].root_bus_nr != -1)
			return &pcie_info[j];
	}
	return NULL;
}

static int __init balong_pcie_map_irq(const struct pci_dev* dev,
                                                             u8 slot, u8 pin)
{
	struct pcie_info* info = bus_to_info(dev->bus->number);
	if(!info){
		pcie_error("Cannot find corresponding controller for appointed device!");
		BUG();
		return -1;
	}

	if(info->controller == pcie_controller_0){
		switch(pin){
			case PCIE_INTA_PIN: return INT_LVL_PCIE0_RADM_A;
			case PCIE_INTB_PIN: return INT_LVL_PCIE0_RADM_B;
			case PCIE_INTC_PIN: return INT_LVL_PCIE0_RADM_C;
			case PCIE_INTD_PIN: return INT_LVL_PCIE0_RADM_D;
			default :
				pcie_error("Unkown pin for mapping irq!");
				return -1;
		}
	}
	pcie_error("Why I'm here??");
	BUG();
	return -1;
}

#define PCIE_CFG_BUS(busnr)	(busnr << 20)
#define PCIE_CFG_DEV(devfn)	(devfn << 15)
#define PCIE_CFG_FUNC(func) (func << 12)
#define PCIE_CFG_REG(reg)	((reg & 0xffc))/*set dword align*/
static inline unsigned int to_pcie_address(struct pci_bus* bus,
                                                  unsigned int devfn, int where)
{
	struct pcie_info* info = bus_to_info(bus->number);
	unsigned int address = 0;
	if(!info){
		pcie_error("Cannot find corresponding controller for appointed device!");
		BUG();
	}

	address = info->base_addr
		| PCIE_CFG_BUS(bus->number)
		| PCIE_CFG_DEV(PCI_SLOT(devfn))
		| PCIE_CFG_FUNC(PCI_FUNC(devfn))
		| PCIE_CFG_REG(where);

	return address;
}

static int pcie_read_from_device(struct pci_bus* bus,
                            unsigned int devfn, int where, int size, u32* value)
{
	struct pcie_info* info = bus_to_info(bus->number);
	unsigned int val=0x00000000;
	unsigned int addr;
	unsigned long flag;
	int i;

	for (i = 0; i < 1000; i++){
		if(pcie_is_link_up()){
			break;
		}
		udelay(1000);
	}

	if(i >= 1000){
		pcie_debug(PCIE_DEBUG_LEVEL_MODULE, "pcie%d not link up!",
            info->controller == pcie_controller_0 ? 0: 1);
		return -1;
	}
	spin_lock_irqsave(&rlock, flag);

	addr = to_pcie_address(bus, devfn, where);

    dbi_disable(pctrl_addr);
	val = readl(addr);

	i=0;
	while(i<20){
		__asm__ __volatile__("nop\n");
		i++;
	}

	if(pcie_errorvalue==1){
		pcie_errorvalue=0;
		val=0xffffffff;
	}

	/*
	if got data is dword align,
	and val got from offset 0,
	i need to calculate which byte is wanted
	*/
	if(size == 1)
		*value = ((val >> ((where & 0x3) << 3)) & 0xff);
	else if(size == 2)
		*value = ((val >> ((where & 0x3) << 3)) & 0xffff);
	else if(size == 4)
		*value = val;
	else{
		pcie_error("Unkown size(%d) for read ops\n",size);
		BUG();
	}

	spin_unlock_irqrestore(&rlock, flag);
	return PCIBIOS_SUCCESSFUL;
}

static int pcie_read_from_dbi(struct pcie_info* info,
                            unsigned int devfn, int where, int size, u32* value)
{
	unsigned long flag;
	u32 v;
	/*for host-side config space read, ignore device func nr.*/
	if(devfn > 0)
		return -EIO;

	spin_lock_irqsave(&rlock, flag);

    dbi_enable(pctrl_addr);
	v = (u32)readl((void*)(info->conf_base_addr + (where & (~0x3))));
    dbi_disable(pctrl_addr);

	if(1 == size)
		*value = (v >> ((where & 0x3) << 3)) & 0xff;
	else if(2 == size)
		*value = (v >> ((where & 0x3) << 3)) & 0xffff;
	else if(4 == size)
		*value = v;
	else{
		pcie_error("Unkown size for config read operation!");
		BUG();
	}
	spin_unlock_irqrestore(&rlock, flag);

	return PCIBIOS_SUCCESSFUL;
}

static int pcie_read_conf(struct pci_bus* bus,
                            unsigned int devfn, int where, int size, u32* value)
{
	struct pcie_info* info = bus_to_info(bus->number);
	int ret;

	if(unlikely(!info)){
		pcie_error("Cannot find corresponding controller for appointed device!");
		BUG();
	}
	if(bus->number == info->root_bus_nr){
		ret = pcie_read_from_dbi(info, devfn, where, size, value);
	}
	else{
		ret = pcie_read_from_device(bus, devfn, where, size, value);
	}
	return ret;
}

static int pcie_write_to_device(struct pci_bus* bus,
                             unsigned int devfn, int where, int size, u32 value)
{
	struct pcie_info* info = bus_to_info(bus->number);
	unsigned int addr;
	unsigned int org;
	unsigned long flag;
	if(!pcie_is_link_up()){
		pcie_debug(PCIE_DEBUG_LEVEL_MODULE, "pcie%d not link up!",
                           info->controller == pcie_controller_0 ? 0: 1);
		return -1;
	}

	spin_lock_irqsave(&wlock, flag);
	pcie_read_from_device(bus, devfn, where, 4, &org);

	addr = to_pcie_address(bus, devfn, where);

	if(size == 1){
		org &= (~(0xff << ((where & 0x3) << 3)));
		org |= (value << ((where & 0x3) << 3));
	}else if(size == 2){
		org &= (~(0xffff << ((where & 0x3) << 3)));
		org |= (value << ((where & 0x3) << 3));
	}else if(size == 4){
		org = value;
	}else{
		pcie_error("Unkown size(%d) for read ops\n",size);
		BUG();
	}
    dbi_disable(pctrl_addr);
	writel(org, addr);

	spin_unlock_irqrestore(&wlock, flag);
	return PCIBIOS_SUCCESSFUL;
}
static int pcie_write_to_dbi(struct pcie_info* info,
                             unsigned int devfn, int where, int size, u32 value)
{
	unsigned long flag;
	unsigned int org;

	spin_lock_irqsave(&wlock, flag);

	if(pcie_read_from_dbi(info, devfn, where, 4, &org)){
		pcie_error("Cannot read from dbi! 0x%x:0x%x:0x%x!",0, devfn, where);
		spin_unlock_irqrestore(&wlock, flag);
		return -EIO;
	}
	if(size == 1){
		org &= (~(0xff << ((where & 0x3) << 3)));
		org |= (value << ((where & 0x3) << 3));
	}else if(size == 2){
		org &= (~(0xffff << ((where & 0x3) << 3)));
		org |= (value << ((where & 0x3) << 3));
	}else if(size == 4){
		org = value;
	}else{
		pcie_error("Unkown size(%d) for read ops\n",size);
		BUG();
	}

    dbi_enable(pctrl_addr);
	writel(org, info->conf_base_addr + (where & (~0x3)));
    dbi_disable(pctrl_addr);

	spin_unlock_irqrestore(&wlock, flag);
	return PCIBIOS_SUCCESSFUL;
}
static int pcie_write_conf(struct pci_bus *bus,
                           unsigned int devfn, int where, int size, u32 value)
{
	struct pcie_info* info = bus_to_info(bus->number);
	if(unlikely(!info)){
		pcie_error("Cannot find corresponding controller for appointed device!");
		BUG();
	}
	if(bus->number == info->root_bus_nr){
		return pcie_write_to_dbi(info, devfn, where, size, value);
	}
	else{
		return pcie_write_to_device(bus, devfn, where, size, value);
	}
}
static struct pci_ops balong_pcie_ops = {
	.read = pcie_read_conf,
	.write = pcie_write_conf,
};

static struct pci_bus *__init balong_pcie_scan_bus(int nr,
                                                       struct pci_sys_data* sys)
{
	struct pci_bus *bus;

	if (nr < balong_pcie.nr_controllers) {
        bus = pci_scan_root_bus(NULL, nr, &balong_pcie_ops, sys, &sys->resources);
	} else {
		bus = NULL;
		pcie_error("Unkown controller nr :0x%x!",nr);
		BUG();
	}
	return bus;
}
extern int pcie_perst_release(void);
static int __init balong_pcie_init(void)
{
    pcie_perst_release();

	printk(KERN_ERR "PCI: Balong Pcie Core Work Mode:Root Complex.\n");

	/*hook data abort exception*/
	hook_fault_code(22,balong_pci_fault,SIGBUS, 7, "external abort on non-linefetch");

	/*software init*/
	if(pcie_sw_init()){
		return -EIO;
	}

	/*hardware init*/
	if(pcie_hw_init()){
		return -EIO;
	}

    if(request_irq(INT_LVL_PCIE0_LINK_DOWN, pcie_linkdown_isr,
        IRQF_DISABLED, "pcie linkdown int", NULL))
    {
        printk(KERN_ERR "pcie linkdown int irq request failed!\n");
        return -1;
    }

    /*
    pericom switch has to wait,otherwise we can't read its configuration space.
    it must be a bug, need confirm.
    */
    {
        int i = 0;
        for(i = 0; i <1000; i++)
        {
            udelay(1000);
        }
    }
	/*mount point for balong pcie bus driver.*/
    pci_common_init(&balong_pcie);
	return 0;
}
/*
gpio module uses subsys_initcall to init,
but pcie is depend on gpio,
so modify subsys_initcall(balong_pcie_init) to
subsys_initcall_sync(balong_pcie_init);
*/
/*subsys_initcall(balong_pcie_init);*/
subsys_initcall_sync(balong_pcie_init);

/*debug interface*/
int m_rc_cfg(u32 addr,u32 val)
{
    dbi_enable(pctrl_addr);
    writel(val,addr);
    dbi_disable(pctrl_addr);
    return 0;
}

int d_rc_cfg(u32 addr)
{
    u32 val = 0;
    dbi_enable(pctrl_addr);
    val = readl(addr);
    dbi_disable(pctrl_addr);
    printk(KERN_ERR "0x%x:%x.\n",addr,val);
    return 0;
}

