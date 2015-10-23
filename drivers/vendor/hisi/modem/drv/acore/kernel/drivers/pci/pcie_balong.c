#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

#include <linux/clk.h>
#include <linux/msi.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/interrupt.h>
#include <linux/regulator/consumer.h>

#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/pci_regs.h>
#include <asm/mach/pci.h>

#include <bsp_memmap.h>
#include <product_config.h>
#include <hi_pcie_interface.h>

#include "pcie_balong.h"

#include <bsp_sram.h>
#include <bsp_pcie.h>


static char* balong_pcie_info_gen_str[] = {"Gen 1.1", "Gen 2", "Gen 3"};
static char* balong_pcie_info_phy_rate_str[] = {"2.5-Gbps", "5-Gbps", "8-Gbps"};

static struct pci_ops balong_pcie_ops = {
    .read = balong_pcie_read_config,
    .write = balong_pcie_write_config,
};

#if defined(BALONG_PCIE0_EXISTS)
static struct balong_pcie_iatu_table balong_pcie0_iatu_table[] = {
    /* PCIE_TLP_DIRECTION_OUTBOUND */
    {
        .index.attr.index = 0,
        .index.attr.direction= PCIE_TLP_DIRECTION_OUTBOUND,
        .control1.attr.type  = PCIE_TLP_TYPE_CFG0_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 1,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable       = 1,
        .lower_addr = BALONG_PCIE0_TYPE0_CONFIG_ADDR,
        .upper_addr = 0,
        .limit_addr = BALONG_PCIE0_TYPE0_CONFIG_ADDR+BALONG_PCIE0_TYPE0_CONFIG_SIZE-1,
        .lower_target_addr = 0,
        .upper_target_addr = 0,
        .control3 = 0,
    },
    {
        .index.attr.index = 1,
        .index.attr.direction = PCIE_TLP_DIRECTION_OUTBOUND,
        .control1.attr.type  = PCIE_TLP_TYPE_CFG1_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 1,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable       = 1,
        .lower_addr = BALONG_PCIE0_TYPE1_CONFIG_ADDR,
        .upper_addr = 0,
        .limit_addr = BALONG_PCIE0_TYPE1_CONFIG_ADDR+BALONG_PCIE0_TYPE1_CONFIG_SIZE-1,
        .lower_target_addr = 0,
        .upper_target_addr = 0,
        .control3 = 0,
    },
    {
        .index.attr.index = 2,
        .index.attr.direction= PCIE_TLP_DIRECTION_OUTBOUND,
        .control1.attr.type = PCIE_TLP_TYPE_IO_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 0,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable       = 1,
        .lower_addr = BALONG_PCIE0_IO_BASE,
        .upper_addr = 0,
        .limit_addr = BALONG_PCIE0_IO_BASE+BALONG_PCIE0_IO_SIZE-1,
        .lower_target_addr = BALONG_PCIE0_IO_BASE,
        .upper_target_addr = 0,
        .control3 = 0,
    },
    {
        .index.attr.index = 3,
        .index.attr.direction = PCIE_TLP_DIRECTION_OUTBOUND,
        .control1.attr.type = PCIE_TLP_TYPE_MEM_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 0,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable       = 1,
        .lower_addr = BALONG_PCIE0_MEM_BASE,
        .upper_addr = 0,
        .limit_addr = BALONG_PCIE0_MEM_BASE+BALONG_PCIE0_MEM_SIZE-1,
        .lower_target_addr = BALONG_PCIE0_MEM_BASE,
        .upper_target_addr = 0,
        .control3 = 0,
    },

    /* PCIE_TLP_DIRECTION_INBOUND */
    {
        /* this entry is for ep's DMA */
        .index.attr.index = 0,
        .index.attr.direction = PCIE_TLP_DIRECTION_INBOUND,
        .control1.attr.type = PCIE_TLP_TYPE_MEM_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 0,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable    = 1,
        .lower_addr = DDR_MEM_ADDR,
        .upper_addr = 0,
        .limit_addr = DDR_MEM_ADDR+DDR_MEM_SIZE-1,
        .lower_target_addr = DDR_MEM_ADDR,
        .upper_target_addr = 0,
        .control3 = 0,
    },
#if defined(CONFIG_PCI_MSI)
    {
        .index.attr.index = 1,
        .index.attr.direction = PCIE_TLP_DIRECTION_INBOUND,
        .control1.attr.type = PCIE_TLP_TYPE_MEM_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 0,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable       = 1,
        .lower_addr = BALONG_PCIE0_MSI_PCI_LOWER_ADDR,
        .upper_addr = BALONG_PCIE0_MSI_PCI_UPPER_ADDR,
        .limit_addr = BALONG_PCIE0_MSI_PCI_LOWER_ADDR+BALONG_PCIE0_MSI_PCI_SIZE-1,
        .lower_target_addr = BALONG_PCIE0_MSI_CPU_LOWER_ADDR,   /* the msi tlp will never arrive here */
        .upper_target_addr = BALONG_PCIE0_MSI_CPU_UPPER_ADDR,
        .control3 = 0,
    },
#endif
};
#endif

#if defined(BALONG_PCIE1_EXISTS)
static struct balong_pcie_iatu_table balong_pcie1_iatu_table[] = {
    /* PCIE_TLP_DIRECTION_OUTBOUND */
    {
        .index.attr.index = 0,
        .index.attr.direction= PCIE_TLP_DIRECTION_OUTBOUND,
        .control1.attr.type  = PCIE_TLP_TYPE_CFG0_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 1,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable       = 1,
        .lower_addr = BALONG_PCIE1_TYPE0_CONFIG_ADDR,
        .upper_addr = 0,
        .limit_addr = BALONG_PCIE1_TYPE0_CONFIG_ADDR+BALONG_PCIE1_TYPE0_CONFIG_SIZE-1,
        .lower_target_addr = 0,
        .upper_target_addr = 0,
        .control3 = 0,
    },
    {
        .index.attr.index = 1,
        .index.attr.direction = PCIE_TLP_DIRECTION_OUTBOUND,
        .control1.attr.type  = PCIE_TLP_TYPE_CFG1_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 1,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable       = 1,
        .lower_addr = BALONG_PCIE1_TYPE1_CONFIG_ADDR,
        .upper_addr = 0,
        .limit_addr = BALONG_PCIE1_TYPE1_CONFIG_ADDR+BALONG_PCIE1_TYPE1_CONFIG_SIZE-1,
        .lower_target_addr = 0,
        .upper_target_addr = 0,
        .control3 = 0,
    },
    {
        .index.attr.index = 2,
        .index.attr.direction= PCIE_TLP_DIRECTION_OUTBOUND,
        .control1.attr.type = PCIE_TLP_TYPE_IO_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 0,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable       = 1,
        .lower_addr = BALONG_PCIE1_IO_BASE,
        .upper_addr = 0,
        .limit_addr = BALONG_PCIE1_IO_BASE+BALONG_PCIE1_IO_SIZE-1,
        .lower_target_addr = BALONG_PCIE1_IO_BASE,
        .upper_target_addr = 0,
        .control3 = 0,
    },
    {
        .index.attr.index = 3,
        .index.attr.direction = PCIE_TLP_DIRECTION_OUTBOUND,
        .control1.attr.type = PCIE_TLP_TYPE_MEM_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 0,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable       = 1,
        .lower_addr = BALONG_PCIE1_MEM_BASE,
        .upper_addr = 0,
        .limit_addr = BALONG_PCIE1_MEM_BASE+BALONG_PCIE1_MEM_SIZE-1,
        .lower_target_addr = BALONG_PCIE1_MEM_BASE,
        .upper_target_addr = 0,
        .control3 = 0,
    },

    /* PCIE_TLP_DIRECTION_INBOUND */
    {
        /* this entry is for ep's DMA */
        .index.attr.index = 0,
        .index.attr.direction = PCIE_TLP_DIRECTION_INBOUND,
        .control1.attr.type = PCIE_TLP_TYPE_MEM_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 0,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable    = 1,
        .lower_addr = DDR_MEM_ADDR,
        .upper_addr = 0,
        .limit_addr = DDR_MEM_ADDR+DDR_MEM_SIZE-1,
        .lower_target_addr = DDR_MEM_ADDR,
        .upper_target_addr = 0,
        .control3 = 0,
    },
#if defined(CONFIG_PCI_MSI)
    {
        .index.attr.index = 1,
        .index.attr.direction = PCIE_TLP_DIRECTION_INBOUND,
        .control1.attr.type = PCIE_TLP_TYPE_MEM_RW,
        .control2.attr.message_code = 0,
        .control2.attr.bar_index    = 0,
        .control2.attr.reserved     = 0,
        .control2.attr.cfg_shift    = 0,
        .control2.attr.invert_mode  = 0,
        .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
        .control2.attr.enable       = 1,
        .lower_addr = BALONG_PCIE1_MSI_PCI_LOWER_ADDR,
        .upper_addr = BALONG_PCIE1_MSI_PCI_UPPER_ADDR,
        .limit_addr = BALONG_PCIE1_MSI_PCI_LOWER_ADDR+BALONG_PCIE1_MSI_PCI_SIZE-1,
        .lower_target_addr = BALONG_PCIE1_MSI_CPU_LOWER_ADDR,   /* the msi tlp will never arrive here */
        .upper_target_addr = BALONG_PCIE1_MSI_CPU_UPPER_ADDR,
        .control3 = 0,
    },
#endif
};
#endif

struct balong_pcie balong_pcie[] = {
#if defined(BALONG_PCIE0_EXISTS)
    {
        .hw_pci = {
            .domain = 0,
            /*.ops = &balong_pcie_ops,
             * we can use this member under linux 3.10 and later,
             * then, the global variable balong_pcie_ops can be deleted
             */
            .nr_controllers = 1,
            .swizzle = pci_std_swizzle,
            .setup = balong_pcie_setup,
            .scan = balong_pcie_scan,
            .map_irq = balong_pcie_map_irq,
        },
        .irqs = {
            .link_down_int = BALONG_PCIE0_INT_LVL_LINK_DOWN,
            .edma_int = BALONG_PCIE0_INT_LVL_EDMA,
            .pm_int = BALONG_PCIE0_INT_LVL_PM,
            .radm_int_a = BALONG_PCIE0_INT_LVL_RADM_A,
            .radm_int_b = BALONG_PCIE0_INT_LVL_RADM_B,
            .radm_int_c = BALONG_PCIE0_INT_LVL_RADM_C,
            .radm_int_d = BALONG_PCIE0_INT_LVL_RADM_D,
            .msi_irq = BALONG_PCIE0_INT_LVL_MSI,
            .msi_irq_base = BALONG_PCIE_INT_LVL_MSI_BASE,

            .dma_channel_state = 0,
        },
        .res_io = {
            .name = "PCIe0 I/O space",
            .start = BALONG_PCIE0_IO_BASE,
            .end = BALONG_PCIE0_IO_BASE + BALONG_PCIE0_IO_SIZE - 1,
            .flags = IORESOURCE_IO,
        },
        .res_mem = {
            .name = "PCIe0 mem space",
            .start = BALONG_PCIE0_MEM_BASE,
            .end = BALONG_PCIE0_MEM_BASE + BALONG_PCIE0_MEM_SIZE - 1,
            .flags = IORESOURCE_MEM,
        },

        .iatu_table = &balong_pcie0_iatu_table[0],
        .iatu_table_entry_num = sizeof(balong_pcie0_iatu_table)/sizeof(balong_pcie0_iatu_table[0]),
        .enabled = 1,
        .work_mode = PCIE_WORK_MODE_RC,
        .link_mode = PCIE_LINK_MODE_X1,
        .phys_rc_cfg_addr = BALONG_PCIE0_RC_CFG_ADDR,
        .phys_rc_cfg_size = BALONG_PCIE0_RC_CFG_SIZE,
        .phys_device_config_addr = BALONG_PCIE0_DEVICE_CFG_ADDR,
        .phys_device_config_size = BALONG_PCIE0_DEVICE_CFG_SIZE,

        .reset_gpio = BALONG_PCIE0_PERST_GPIO,
        .regulator_id = "pcie_mtcmos-vcc",
        .clock_core_id = "pcie_ctrl1_clk",
        .clock_aux_id = "pcie_ctrl2_clk",
        .clock_phy_id = "pcie_phy_clk",

        .linked = 0,
        .root_bus_number = 0,
        .virt_rc_cfg_addr = 0,
        .virt_device_config_addr = 0,
        .regulator = NULL,
        .clock_core = NULL,
        .clock_aux = NULL,
        .clock_phy = NULL,
    },
#endif
#if defined(BALONG_PCIE1_EXISTS)
    {
        .hw_pci = {
            .domain = 1,
            /*.ops = &balong_pcie_ops,
             * we can use this member under linux 3.10 and later,
             * then, the global variable balong_pcie_ops can be deleted
             */
            .nr_controllers = 1,
            .swizzle = pci_std_swizzle,
            .setup = balong_pcie_setup,
            .scan = balong_pcie_scan,
            .map_irq = balong_pcie_map_irq,
        },
        .irqs = {
            .link_down_int = BALONG_PCIE1_INT_LVL_LINK_DOWN,
            .edma_int = BALONG_PCIE1_INT_LVL_EDMA,
            .pm_int = BALONG_PCIE1_INT_LVL_PM,
            .radm_int_a = BALONG_PCIE1_INT_LVL_RADM_A,
            .radm_int_b = BALONG_PCIE1_INT_LVL_RADM_B,
            .radm_int_c = BALONG_PCIE1_INT_LVL_RADM_C,
            .radm_int_d = BALONG_PCIE1_INT_LVL_RADM_D,
            .msi_irq = BALONG_PCIE1_INT_LVL_MSI,
            .msi_irq_base = BALONG_PCIE_INT_LVL_MSI_BASE,

            .dma_channel_state = 0,
        },
        .res_io = {
            .name = "PCIe1 I/O space",
            .start = BALONG_PCIE1_IO_BASE,
            .end = BALONG_PCIE1_IO_BASE + BALONG_PCIE1_IO_SIZE - 1,
            .flags = IORESOURCE_IO,
        },
        .res_mem = {
            .name = "PCIe1 mem space",
            .start = BALONG_PCIE1_MEM_BASE,
            .end = BALONG_PCIE1_MEM_BASE + BALONG_PCIE1_MEM_SIZE - 1,
            .flags = IORESOURCE_MEM,
        },

        .iatu_table = &balong_pcie1_iatu_table[0],
        .iatu_table_entry_num = sizeof(balong_pcie1_iatu_table)/sizeof(balong_pcie1_iatu_table[0]),
        .enabled = 1,
        .work_mode = PCIE_WORK_MODE_RC,
        .link_mode = PCIE_LINK_MODE_X1,
        .phys_rc_cfg_addr = BALONG_PCIE1_RC_CFG_ADDR,
        .phys_rc_cfg_size = BALONG_PCIE1_RC_CFG_SIZE,
        .phys_device_config_addr = BALONG_PCIE1_DEVICE_CFG_ADDR,
        .phys_device_config_size = BALONG_PCIE1_DEVICE_CFG_SIZE,

        .reset_gpio = BALONG_PCIE1_PERST_GPIO,
        .regulator_id = "pcie_mtcmos-vcc",
        .clock_core_id = "pcie_ctrl1_clk",
        .clock_aux_id = "pcie_ctrl2_clk",
        .clock_phy_id = "pcie_phy_clk",

        .linked = 0,
        .root_bus_number = 0,
        .virt_rc_cfg_addr = 0,
        .virt_device_config_addr = 0,
        .regulator = NULL,
        .clock_core = NULL,
        .clock_aux = NULL,
        .clock_phy = NULL,
    },
#endif
};

u32 balong_pcie_num = sizeof(balong_pcie)/sizeof(balong_pcie[0]);

#if defined(CONFIG_PCI_MSI)
static void balong_pcie_msi_isr(unsigned int irq, struct irq_desc *desc);
#endif

void dbi_enable(u32 id)
{
    hi_pcie_dbi_enable(id);

#if defined(PCIE_FPGA)
    mdelay(10);
#endif
}

void dbi_disable(u32 id)
{
    hi_pcie_dbi_disable(id);

#if defined(PCIE_FPGA)
    mdelay(10);
#endif
}

/* Attention: the dbi must be enabled when this function is called */
void pcie_set_iatu(u32 id, struct balong_pcie_iatu_table *iatu_table, u32 iatu_table_entry_num)
{
    u32 i = 0;

    for (i = 0; i < iatu_table_entry_num; i++)
    {
        writel(iatu_table->index.value,
            balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x200);
        writel(iatu_table->lower_addr,
            balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x20C);
        writel(iatu_table->upper_addr,
            balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x210);
        writel(iatu_table->limit_addr,
            balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x214);
        writel(iatu_table->lower_target_addr,
            balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x218);
        writel(iatu_table->upper_target_addr,
            balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x21C);
        writel(iatu_table->control1.value,
            balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x204);
        /* writel(iatu_table->control3,
            balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x220); */
        writel(iatu_table->control2.value,
            balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x208);

        iatu_table++;
    }
}

void pcie_app_req_clk(u32 id, u32 request)
{
#if defined(CONFIG_BALONG_PCIE_L1SS)
    unsigned long irq_flags = 0;
    volatile static unsigned long req_clk_count = 0;

    spin_lock_irqsave(&balong_pcie[id].spinlock_req_clk, irq_flags);

    if (request)
    {
        if (++req_clk_count == 1)
        {
            hi_pcie_app_clk_req_n(id, request);
        }
    }
    else
    {
        if (--req_clk_count == 0)
        {
            hi_pcie_app_clk_req_n(id, request);
        }
    }

    spin_unlock_irqrestore(&balong_pcie[id].spinlock_req_clk, irq_flags);
#endif
}

u32 pcie_convert_to_pcie_domain_address(struct pci_bus *bus, unsigned int devfn, int where)
{
    u32 pcie_address = 0;
    struct balong_pcie *info = (struct balong_pcie *)((struct pci_sys_data *)bus->sysdata)->private_data;

    pcie_address = info->virt_device_config_addr
        | PCIE_CFG_BUS(bus->number)
        | PCIE_CFG_DEV(PCI_SLOT(devfn))
        | PCIE_CFG_FUNC(PCI_FUNC(devfn))
        | PCIE_CFG_REG(where);

    return pcie_address;
}

int pcie_read_config_from_dbi(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *value)
{
    unsigned long irq_flags = 0;
    struct balong_pcie *info = (struct balong_pcie *)((struct pci_sys_data *)bus->sysdata)->private_data;

    /*for host-side config space read, ignore device func nr.*/
    if(devfn > 0)
        return -EIO;

    spin_lock_irqsave(&info->spinlock, irq_flags);

    pcie_app_req_clk(info->hw_pci.domain, 1);

    dbi_enable(info->hw_pci.domain);

    if (1 == size)
        *value = readb(info->virt_rc_cfg_addr + where);
    else if (2 == size)
        *value = readw(info->virt_rc_cfg_addr + where);
    else if (4 == size)
        *value = readl(info->virt_rc_cfg_addr + where);

    dbi_disable(info->hw_pci.domain);

    pcie_app_req_clk(info->hw_pci.domain, 0);

    spin_unlock_irqrestore(&info->spinlock, irq_flags);

    return PCIBIOS_SUCCESSFUL;
}

int pcie_read_config_from_device(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *value)
{
    u32 pcie_address = 0;
    struct balong_pcie *info = (struct balong_pcie *)((struct pci_sys_data *)bus->sysdata)->private_data;

    if (!info->linked)
    {
        pcie_trace("fail to read config from device, controller %d is link down\n", info->hw_pci.domain);
        return -EIO;
    }

    pcie_app_req_clk(info->hw_pci.domain, 1);

    pcie_address = pcie_convert_to_pcie_domain_address(bus, devfn, where);

    if (1 == size)
        *value = readb(pcie_address);
    else if (2 == size)
        *value = readw(pcie_address);
    else if (4 == size)
        *value = readl(pcie_address);

    pcie_app_req_clk(info->hw_pci.domain, 0);

    return PCIBIOS_SUCCESSFUL;
}

int pcie_write_config_to_dbi(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 value)
{
    unsigned long irq_flags = 0;
    struct balong_pcie *info = (struct balong_pcie *)((struct pci_sys_data *)bus->sysdata)->private_data;

    spin_lock_irqsave(&info->spinlock, irq_flags);

    pcie_app_req_clk(info->hw_pci.domain, 1);

    dbi_enable(info->hw_pci.domain);

    if (1 == size)
        writeb(value, info->virt_rc_cfg_addr + where);
    else if (2 == size)
        writew(value, info->virt_rc_cfg_addr + where);
    else if (4 == size)
        writel(value, info->virt_rc_cfg_addr + where);

    dbi_disable(info->hw_pci.domain);

    pcie_app_req_clk(info->hw_pci.domain, 0);

    spin_unlock_irqrestore(&info->spinlock, irq_flags);

    return PCIBIOS_SUCCESSFUL;
}

int pcie_write_config_to_device(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 value)
{
    u32 pcie_address = 0;
    struct balong_pcie *info = (struct balong_pcie *)((struct pci_sys_data *)bus->sysdata)->private_data;

    if (!info->linked)
    {
        pcie_trace("fail to write config from device, controller %d is link down\n", info->hw_pci.domain);
        return -EIO;
    }

    pcie_app_req_clk(info->hw_pci.domain, 1);

    pcie_address = pcie_convert_to_pcie_domain_address(bus, devfn, where);

    if (1 == size)
        writeb(value, pcie_address);
    else if (2 == size)
        writew(value, pcie_address);
    else if (4 == size)
        writel(value, pcie_address);

    pcie_app_req_clk(info->hw_pci.domain, 0);

    return PCIBIOS_SUCCESSFUL;
}

int balong_pcie_read_config(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *value)
{
    int ret = 0;
    struct balong_pcie *info = (struct balong_pcie *)((struct pci_sys_data *)bus->sysdata)->private_data;

    if (bus->number == info->root_bus_number)
        ret = pcie_read_config_from_dbi(bus, devfn, where, size, value);
    else
        ret = pcie_read_config_from_device(bus, devfn, where, size, value);

    return ret;
}

int balong_pcie_write_config(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 value)
{
    int ret = 0;
    struct balong_pcie *info = (struct balong_pcie *)((struct pci_sys_data *)bus->sysdata)->private_data;

    if (bus->number == info->root_bus_number)
        ret = pcie_write_config_to_dbi(bus, devfn, where, size, value);
    else
        ret = pcie_write_config_to_device(bus, devfn, where, size, value);

    return ret;
}

int balong_pcie_setup(int nr, struct pci_sys_data *sys_data)
{
    int ret = 0;

    /* record busnr so we can use it in config read/write */
    balong_pcie[sys_data->domain].root_bus_number = sys_data->busnr;

    sys_data->mem_offset = 0;
    sys_data->io_offset = 0;
    sys_data->private_data = (void*)&balong_pcie[sys_data->domain];

    ret = request_resource(&ioport_resource, &balong_pcie[sys_data->domain].res_io);
    if (ret)
    {
        pcie_trace("fail to request resource, controller: %d\n", sys_data->domain);
        return ret;
    }

    ret = request_resource(&iomem_resource, &balong_pcie[sys_data->domain].res_mem);
    if (ret)
    {
        release_resource(&balong_pcie[sys_data->domain].res_io);
        pcie_trace("fail to request resource, controller: %d\n", sys_data->domain);
        return ret;
    }

    pci_add_resource_offset(&sys_data->resources, &balong_pcie[nr].res_io, sys_data->io_offset);
    pci_add_resource_offset(&sys_data->resources, &balong_pcie[nr].res_mem, sys_data->mem_offset);

    return 1;
}

struct pci_bus* balong_pcie_scan(int nr, struct pci_sys_data *sys_data)
{
    struct pci_bus *bus;

    bus = pci_scan_root_bus(NULL, nr, &balong_pcie_ops, sys_data, &sys_data->resources);

    return bus;
}

int balong_pcie_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
    struct balong_pcie *info = (struct balong_pcie *)(((struct pci_sys_data *)(dev->bus->sysdata))->private_data);

    switch (pin)
    {
        case 1:
            return info->irqs.radm_int_a;
        case 2:
            return info->irqs.radm_int_b;
        case 3:
            return info->irqs.radm_int_c;
        case 4:
            return info->irqs.radm_int_d;
        default:
        {
            pcie_trace("Unkown pin %d for mapping irq!\n", pin);
            return -1;
        }
    }
}

static irqreturn_t balong_pcie_linkdown_isr(int irq, void* dev_info)
{
    struct balong_pcie *info = (struct balong_pcie *)dev_info;

    hi_pcie_linkdown_interrupt_clear(info->hw_pci.domain);

    info->linked = 0;
    pcie_trace("link down interrupt, controller %d\n", info->hw_pci.domain);

    return IRQ_HANDLED;
}

static irqreturn_t balong_pcie_dma_isr(int irq, void* dev_info)
{
    u32 read_status = 0;
    u32 write_status = 0;
    u32 done_status = 0;
    u32 abort_status = 0;
    u32 channel_status = 0;
    u32 channel_index = 0;

    unsigned long irq_flags = 0;
    struct balong_pcie *info = (struct balong_pcie *)dev_info;

    spin_lock_irqsave(&info->spinlock, irq_flags);
    dbi_enable(info->hw_pci.domain);

    /* DMA read interrupt status */
    read_status = readl(info->virt_rc_cfg_addr+ 0x700 + 0x310);
    /* DMA read interrupt clear */
    writel(read_status, info->virt_rc_cfg_addr + 0x700 + 0x31C);
    /* DMA write interrupt status */
    write_status = readl(info->virt_rc_cfg_addr + 0x700 + 0x2BC);
    /* DMA write interrupt clear */
    writel(write_status, info->virt_rc_cfg_addr + 0x700 + 0x2C8);

    dbi_disable(info->hw_pci.domain);
    spin_unlock_irqrestore(&info->spinlock, irq_flags);

    if (read_status & write_status)
    {
        pcie_trace("error interrupt status, read status: 0x%08X, write status: 0x%08X\n",
                    read_status, write_status);
        BUG();
    }

    if (read_status)
    {
        done_status = read_status & 0x0000FFFF;
        abort_status = (read_status >> 16) & 0x0000FFFF;
        channel_status = done_status | abort_status;

        for (channel_index = 0; channel_index < BALONG_PCIE0_MAX_DMA_CHANNEL_NUM; channel_index++)
        {
            if (channel_status & (1 << channel_index))
            {
                if (info->irqs.dma_int_callback[channel_index])
                {
                    info->irqs.dma_int_callback[channel_index](PCIE_DMA_DIRECTION_READ, abort_status,
                        info->irqs.dma_int_callback_args[channel_index]);
                }
                info->irqs.dma_channel_state &= (~(1 << channel_index));
                pcie_app_req_clk(info->hw_pci.domain, 0);
                up(&info->irqs.dma_semaphore[channel_index]);
            }
        }
    }

    if (write_status)
    {
        done_status = write_status & 0x0000FFFF;
        abort_status = (write_status >> 16) & 0x0000FFFF;
        channel_status = done_status | abort_status;

        for (channel_index = 0; channel_index < BALONG_PCIE0_MAX_DMA_CHANNEL_NUM; channel_index++)
        {
            if (channel_status & (1 << channel_index))
            {
                if (info->irqs.dma_int_callback[channel_index])
                {
                    info->irqs.dma_int_callback[channel_index](PCIE_DMA_DIRECTION_WRITE, abort_status,
                        info->irqs.dma_int_callback_args[channel_index]);
                }
                info->irqs.dma_channel_state &= (~(1 << channel_index));
                pcie_app_req_clk(info->hw_pci.domain, 0);
                up(&info->irqs.dma_semaphore[channel_index]);
            }
        }
    }

    return IRQ_HANDLED;
}

static irqreturn_t balong_pcie_pm_isr(int irq, void* dev_info)
{
    struct balong_pcie *info = (struct balong_pcie *)dev_info;

    pcie_trace("pm interrupt, controller %d\n", info->hw_pci.domain);

    return IRQ_HANDLED;
}

/* assert a global reset through PCIE_PERST singnal */
int pcie_global_reset(u32 id)
{
    int ret = 0;

    if (balong_pcie[id].work_mode != PCIE_WORK_MODE_RC)
        return ret;

    ret = gpio_request(balong_pcie[id].reset_gpio, "pcie-perst");
    if (ret)
    {
        pcie_trace("fail to request for GPIO:%u\n", balong_pcie[id].reset_gpio);
        return ret;
    }

    ret = gpio_direction_output(balong_pcie[id].reset_gpio, 0);
    if (ret)
    {
        pcie_trace("fail to set GPIO:%u\n", balong_pcie[id].reset_gpio);
        goto error_set_gpio;
    }

    /* stay in low for 100ms */
    mdelay(100);

    ret = gpio_direction_output(balong_pcie[id].reset_gpio, 1);
    if (ret)
    {
        pcie_trace("fail to set GPIO:%u\n", balong_pcie[id].reset_gpio);
        goto error_set_gpio;
    }

error_set_gpio:
    gpio_free(balong_pcie[id].reset_gpio);

    return ret;
}

void pcie_common_init(void)
{
    int i = 0;
    int id = 0;
    struct pcie_cfg *pcie_cfg_table = (struct pcie_cfg *)SRAM_PCIE_INFO_ADDR;

    for (id = 0; id < (int)(sizeof(balong_pcie)/sizeof(balong_pcie[0])); id++)
    {
        for (i = 0; i < (id - 1); i++)
        {
            if (!strcmp(balong_pcie[i].regulator_id, balong_pcie[id].regulator_id))
            {
                balong_pcie[id].regulator = balong_pcie[i].regulator;
                break;
            }
        }

        if (!balong_pcie[id].regulator)
        {
            balong_pcie[id].regulator = regulator_get(NULL, balong_pcie[id].regulator_id);
            if (IS_ERR(balong_pcie[id].regulator))
            {
                pcie_trace("fail to get regulator\n");
            }
        }

#if defined(CONFIG_PCIE_CFG)
        if (pcie_cfg_table[id].valid == PCIE_CFG_VALID)
        {
            balong_pcie[id].work_mode = pcie_cfg_table[id].work_mode;
        }
#endif
    }
}

int pcie_software_init(u32 id)
{
    u32 i = 0;
    int ret = 0;

    spin_lock_init(&balong_pcie[id].spinlock);
    spin_lock_init(&balong_pcie[id].spinlock_req_clk);

    for (i = 0; i < sizeof(balong_pcie[id].irqs.dma_semaphore)/sizeof(balong_pcie[id].irqs.dma_semaphore[0]); i++)
    {
        sema_init(&balong_pcie[id].irqs.dma_semaphore[i], 1);
    }

    balong_pcie[id].clock_core = clk_get(NULL, balong_pcie[id].clock_core_id);
    if (IS_ERR(balong_pcie[id].clock_core))
    {
        pcie_trace("fail to get core clock\n");
    }

    balong_pcie[id].clock_aux = clk_get(NULL, balong_pcie[id].clock_aux_id);
    if (IS_ERR(balong_pcie[id].clock_aux))
    {
        pcie_trace("fail to get aux clock\n");
    }

    balong_pcie[id].clock_phy = clk_get(NULL, balong_pcie[id].clock_phy_id);
    if (IS_ERR(balong_pcie[id].clock_phy))
    {
        pcie_trace("fail to get phy clock\n");
    }

    balong_pcie[id].virt_rc_cfg_addr = (u32)ioremap(balong_pcie[id].phys_rc_cfg_addr, balong_pcie[id].phys_rc_cfg_size);
    if (!balong_pcie[id].virt_rc_cfg_addr)
    {
        pcie_trace("fail to remap\n");
        return -1;
    }

    ret |= request_irq(balong_pcie[id].irqs.link_down_int, balong_pcie_linkdown_isr,
            IRQF_DISABLED, "pcie link down", (void*)&balong_pcie[id]);

    ret |= request_irq(balong_pcie[id].irqs.edma_int, balong_pcie_dma_isr,
            IRQF_DISABLED, "pcie dma", (void*)&balong_pcie[id]);

    ret |= request_irq(balong_pcie[id].irqs.pm_int, balong_pcie_pm_isr,
            IRQF_DISABLED, "pcie pm", (void*)&balong_pcie[id]);
    if (ret)
    {
        pcie_trace("fail to request irq, controller: %u\n", id);
        return ret;
    }

    if (PCIE_WORK_MODE_RC == balong_pcie[id].work_mode)
    {
        balong_pcie[id].virt_device_config_addr = (u32)ioremap(balong_pcie[id].phys_device_config_addr, balong_pcie[id].phys_device_config_size);
        if (!balong_pcie[id].virt_device_config_addr)
        {
            iounmap((void*)balong_pcie[id].virt_rc_cfg_addr);
            pcie_trace("fail to remap\n");
            return -1;
        }

#if defined(CONFIG_PCI_MSI)
        /*setup msi irq handler*/
        irq_set_chained_handler(balong_pcie[id].irqs.msi_irq, balong_pcie_msi_isr);
#endif
    }

    return 0;
}

void pcie_power_on(u32 id)
{
    int ret = 0;

    if (balong_pcie[id].regulator)
        ret |= regulator_enable(balong_pcie[id].regulator);

    if (ret)
        pcie_trace("fail to enable regulator\n");
}

void pcie_power_off(u32 id)
{
    int ret = 0;

    if (balong_pcie[id].regulator)
        ret |= regulator_disable(balong_pcie[id].regulator);

    if (ret)
        pcie_trace("fail to disable regulator\n");
}

void pcie_clk_init(u32 id)
{
#if defined(BALONG_PCI_INNER_CLK)
    if (PCIE_WORK_MODE_EP == balong_pcie[id].work_mode)
        return;

    /* switch to inner clock */
    hi_pcie_inner_clk_enable(id);

#if defined(CONFIG_BALONG_PCIE_OUTPUT_CLK)

    /* enable to output clock */
    hi_pcie_output_clk_enable(id);

#endif  /* CONFIG_BALONG_PCIE_OUTPUT_CLK */

#endif  /* BALONG_PCI_INNER_CLK */
}

void pcie_clk_enable(u32 id)
{
    int ret = 0;

    /* enable core_clk, aux_clk and phy clock */
    if (balong_pcie[id].clock_core)
        ret |= clk_enable(balong_pcie[id].clock_core);

    if (balong_pcie[id].clock_aux)
        ret |= clk_enable(balong_pcie[id].clock_aux);

    if (balong_pcie[id].clock_phy)
        ret |= clk_enable(balong_pcie[id].clock_phy);

    if (ret)
        pcie_trace("fail to enable clock\n");

    udelay(100);
}

void pcie_clk_disable(u32 id)
{
    /* disable core_clk, aux_clk and phy clock */
    if (balong_pcie[id].clock_core)
        clk_disable(balong_pcie[id].clock_core);

    if (balong_pcie[id].clock_aux)
        clk_disable(balong_pcie[id].clock_aux);

    if (balong_pcie[id].clock_phy)
        clk_disable(balong_pcie[id].clock_phy);

#if defined(PCIE_FPGA)
    mdelay(10);
#endif
}

void pcie_ltssm_enable(u32 id)
{
    unsigned long irq_flags = 0;

    spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);

    hi_pcie_ltssm_enable(id);

    spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);

#if defined(PCIE_FPGA)
    mdelay(10);
#endif
}

void pcie_ltssm_disable(u32 id)
{
    unsigned long irq_flags = 0;

    spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);

    hi_pcie_ltssm_disable(id);

    spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);

#if defined(PCIE_FPGA)
    mdelay(10);
#endif
}

void pcie_phy_init(u32 id)
{
    hi_pcie_phy_init(id);
}

void pcie_reset(u32 id)
{
    unsigned long irq_flags = 0;

    spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);

    hi_pcie_core_reset(id);

    hi_pcie_phy_reset(id);

    spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);

    udelay(100);
}

void pcie_unreset(u32 id)
{
    u32 count = 0;
    unsigned long irq_flags = 0;

#if defined(PCIE_FPGA)
    /* unreset DSP domain */
    value = readl(HI_SYSSC_BASE_ADDR_VIRT + HI_CRG_SRSTDIS2_OFFSET);
    value |= (0x1 << 0);
    writel(value, HI_SYSSC_BASE_ADDR_VIRT + HI_CRG_SRSTDIS2_OFFSET);
    mdelay(500);   /* this can be delayed in the next FPGA version */
#endif
    spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);

    hi_soc_peri_usb_unreset(id);

    hi_pcie_phy_unreset(id);

    hi_pcie_core_unreset(id);

    spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);

#if defined(PCIE_FPGA)
    mdelay(20);
#endif
    udelay(100);

    while (hi_pcie_is_under_reset(id))
    {
        count++;
        mdelay(1);
        if (count > 1000)
        {
            pcie_trace("waiting for phy ready timeout\n");
            break;
        }
    }
}

enum PCIE_CAPABILITY_TYPE
{
    PCIE_CAPABILITY_TYPE_PCI_STANDARD,
    PCIE_CAPABILITY_TYPE_PCIE_EXTENDED,
};

/* Attention: the dbi must be enabled when called this function */
u32 pcie_get_capability_by_id(u32 id, u32 capability_id, enum PCIE_CAPABILITY_TYPE type)
{
    u32 value = 0;
    u32 offset = 0;
    u32 current_id = 0;

    if (PCIE_CAPABILITY_TYPE_PCI_STANDARD == type)
    {
        /* PCI Standard Capability */
        offset = readl(balong_pcie[id].virt_rc_cfg_addr + PCI_CAPABILITY_LIST) & 0xFF;
        do
        {
            value = readl(balong_pcie[id].virt_rc_cfg_addr + offset);
            current_id = value & 0xFF;

            if (current_id == capability_id)
                break;

            offset = (value >> 8) & 0xFF;
        } while (offset);
    }
    else
    {
        /* PCI Express Extended Capability */
        offset = 0x100;
        do
        {
            value = readl(balong_pcie[id].virt_rc_cfg_addr + offset);
            current_id = PCI_EXT_CAP_ID(value);

            if (current_id == capability_id)
                break;

            offset = PCI_EXT_CAP_NEXT(value);
        } while (offset);
    }

    if (current_id != capability_id)
        offset = 0;

    return offset;
}

void pcie_ctrl_init(u32 id)
{
    u32 value = 0;
    u32 offset = 0;
    unsigned long irq_flags = 0;

    spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);

    /* set work mode */
    hi_pcie_set_work_mode(id, balong_pcie[id].work_mode);

    pcie_app_req_clk(id, 1);

    dbi_enable(id);

    /* set to correct class mode
     * High 24 bits are class, low 8 revision
     */
    value = readl(balong_pcie[id].virt_rc_cfg_addr + PCI_CLASS_REVISION);
    value &= 0x000000FF;
    if (PCIE_WORK_MODE_RC == balong_pcie[id].work_mode)
        value |= ((PCI_CLASS_BRIDGE_PCI << 8) << 8);
    else
        value |= ((PCI_CLASS_DOCKING_GENERIC << 8) << 8);
    writel(value, balong_pcie[id].virt_rc_cfg_addr + PCI_CLASS_REVISION);

    /* set to correct link mode */
    value = readl(balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x010);
    value &= (~(0x3F<<16));
    value |= (balong_pcie[id].link_mode<<16);
    writel(value, balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x010);

    value = readl(balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x10C);
    value &= (~(0x1FF<<8));
    if (PCIE_LINK_MODE_X1 == balong_pcie[id].link_mode)
        value |= (0x1<<8);
    else if (PCIE_LINK_MODE_X2 == balong_pcie[id].link_mode)
        value |= (0x2<<8);
    writel(value, balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x10C);

    /* PCI_CAP_ID_EXP = 0x10, PCI Express Capability */
    offset = pcie_get_capability_by_id(id, PCI_CAP_ID_EXP, PCIE_CAPABILITY_TYPE_PCI_STANDARD);
    if (offset)
    {
#if defined(CONFIG_BALONG_PCIE_FORCE_GEN1)
        /* force to set to gen1 */
        value = readl(balong_pcie[id].virt_rc_cfg_addr + offset + 0x30);
        value = ((value & (~0xF)) | 0x1);
        writel(value, balong_pcie[id].virt_rc_cfg_addr + offset + 0x30);
#endif
        if (PCIE_WORK_MODE_EP == balong_pcie[id].work_mode)
        {
            /* set acceptable latency for L0s and L1 */
            value = readl(balong_pcie[id].virt_rc_cfg_addr + offset + 0x04);
            value |= (0x7 << 6);    /* L0s  : more than 4 us */
            value |= (0x7 << 9);    /* L1   : more than 4 us */
            writel(value, balong_pcie[id].virt_rc_cfg_addr + offset + 0x04);
        }
    }

#if defined(CONFIG_BALONG_PCIE_L1SS)
    /* L1SS Setting L1 Substates Capabality ID == 0x1E */
    offset = pcie_get_capability_by_id(id, 0x1E, PCIE_CAPABILITY_TYPE_PCIE_EXTENDED);
    if (offset)
    {
        value = readl(balong_pcie[id].virt_rc_cfg_addr + offset + 0x04);
        value &= 0x0F;
        value |= readl(balong_pcie[id].virt_rc_cfg_addr + offset + 0x08);
        writel(value, balong_pcie[id].virt_rc_cfg_addr + offset + 0x08);
    }
#endif

    if (PCIE_WORK_MODE_RC == balong_pcie[id].work_mode)
    {
        /* disable BAR0 */
        writel(0x00, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_0));

#if defined(CONFIG_PCI_MSI)
        /* setup msi addr for msi interrupt controller */
        writel(BALONG_PCIE0_MSI_CPU_LOWER_ADDR, balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x120);
        writel(BALONG_PCIE0_MSI_CPU_UPPER_ADDR, balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x124);
#endif

        /* init iatu table */
        pcie_set_iatu(id, balong_pcie[id].iatu_table, balong_pcie[id].iatu_table_entry_num);
    }
    else/* work mode: EP */
    {
        /* BAR Register
         * bit[0:0]   0 = Memory BAR; 1 = I/O BAR
         * bit[2:1]   00= 32-bit BAR; 1 = 64-bit BAR
         * bit[3:3]   0 = Non-prefetchable; 1 = Prefetchable
         * bit[31:4]  Base Address bits
         */
        /* modify bar to non-prefetchmem */
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_0);
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_1);
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_2);
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_3);
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_4);
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_5);

        /* BAR Mask Register
         * bit[0:0]   0 = BAR is disable; 1 = BAR is enable
         * bit[31:1]  Indicates which BAR bits to mask
         */
        /* set bar0 size to 2MB */
        writel(0x1FFFFF, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_0));
        /* set bar2 size to 2MB
         * we use BAR2 here because the BAR1 can not be resize on 6930(default size is 64KB)
         */
        writel(0x1FFFFF, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_2));
        /* disable other bar */
        writel(0, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_1));
        writel(0, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_3));
        writel(0, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_4));
        writel(0, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_5));


        /* set subsystem vender id to 0x1 for distingushing hisilicon rc or ep.
         * rc subsystem vendor id is 0.
         * ep subsystem vendor id is 1.
         */
        writel(0x1, balong_pcie[id].virt_rc_cfg_addr + PCI_SUBSYSTEM_VENDOR_ID);
    }

    dbi_disable(id);

    pcie_app_req_clk(id, 0);

    spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);
}

void pcie_link_up_check(u32 id)
{
    balong_pcie[id].linked = hi_pcie_is_linkup(id);

#if defined(CONFIG_BALONG_PCIE_L1SS)
    if (balong_pcie[id].linked)
        hi_pcie_l1ss_auto_gate_enable(id);
#endif
}

void pcie_wait_for_linkup(u32 id)
{
    int count = 0;

    if (PCIE_WORK_MODE_RC == balong_pcie[id].work_mode)
    {
        do
        {
            count++;
            udelay(1000);
            pcie_link_up_check(id);
        }while ((!balong_pcie[id].linked) && (count < 1*1000));
    }
    else
    {
        printk(KERN_ERR "*************************************************\n"
                        "* [PCIE]: EP init ok, waiting for RC connect... *\n"
                        "*************************************************\n");
    }
}

void pcie_try_change_speed(u32 id)
{
#if !defined(CONFIG_BALONG_PCIE_FORCE_GEN1)
    u32 value = 0;
    u32 link_speed = 0;
    unsigned long irq_flags = 0;

    spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);

    pcie_app_req_clk(id, 1);

    dbi_enable(id);

    value = readl(balong_pcie[id].virt_rc_cfg_addr + 0x70 + 0x10);
    link_speed = ((value >> 16) & 0x0F);
    if (link_speed != 2)
    {
        value = readl(balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x10C);
        value |= (0x01<<17);
        writel(value, balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x10C);

        udelay(200);
    }

    dbi_disable(id);

    pcie_app_req_clk(id, 0);

    spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);
#endif
}

void pcie_show_link_status(u32 id)
{
    u32 value = 0;
    u32 link_speed = 0;
    u32 link_width = 0;
    unsigned long irq_flags = 0;

    spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);

    pcie_app_req_clk(id, 1);

    dbi_enable(id);
    value = readl(balong_pcie[id].virt_rc_cfg_addr + 0x70 + 0x10);
    dbi_disable(id);

    pcie_app_req_clk(id, 0);

    spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);

    link_width = ((value >> 20) & 0x3F);
    link_speed = ((value >> 16) & 0x0F) / 2;

    pcie_trace("Balong PCIe Info: %s %s, Lane Num: x%d\n",
        balong_pcie_info_phy_rate_str[link_speed],
        balong_pcie_info_gen_str[link_speed],
        link_width);
}

void pcie_hardware_init(u32 id)
{
    pcie_clk_init(id);
    pcie_phy_init(id);
    pcie_power_on(id);
    pcie_clk_enable(id);
    pcie_ltssm_disable(id);
    pcie_reset(id);
    pcie_unreset(id);
    pcie_ctrl_init(id);
    pcie_ltssm_enable(id);
}

#if defined(CONFIG_PCI_MSI)

#define MAX_MSI_NUM (32*8)

static DECLARE_BITMAP(msi_irq_bits, MAX_MSI_NUM);

static void balong_pcie_msi_isr(unsigned int irq, struct irq_desc *desc)
{
    u32 id = 0;
    u32 bit = 0;
    u32 index = 0;
    u32 status = 0;
    unsigned long irq_flags = 0;

    for (id = 0; id < sizeof(balong_pcie)/sizeof(balong_pcie[0]); id++)
    {
        if (balong_pcie[id].irqs.msi_irq == irq)
            break;
    }

    /*
     * The chained irq handler installation would have replaced normal
     * interrupt driver handler so we need to take care of mask/unmask and
     * ack operation.
     *
     */
    desc->irq_data.chip->irq_mask((struct irq_data*)&desc->irq_data);

    for(index = 0; index < 8; index++)
    {
        spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);
        pcie_app_req_clk(id, 1);
        dbi_enable(id);
        status = readl(balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x130 + index*0x0C);
        writel(status, balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x130 + index*0x0C);
        dbi_disable(id);
        pcie_app_req_clk(id, 0);
        spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);

        for (bit = 0; bit < 32; bit++)
        {
            if (status & (1 << bit))
            {
                generic_handle_irq(balong_pcie[id].irqs.msi_irq_base + index * 32 + bit);
            }
        }
    }

    desc->irq_data.chip->irq_eoi((struct irq_data*)&desc->irq_data);
    desc->irq_data.chip->irq_unmask((struct irq_data*)&desc->irq_data);
}


static void msi_ack(struct irq_data *data)
{
    /* clear interrupt by user isr,not here. */
}

static void msi_mask(struct irq_data *data)
{
    u32 value = 0;
    u32 index = 0;
    u32 status = 0;
    unsigned long irq_flags = 0;

    struct msi_desc *desc = NULL;
    struct balong_pcie *info = NULL;

    desc = irq_data_get_msi(data);
    info = (struct balong_pcie *)((struct pci_sys_data *)desc->dev->bus->sysdata)->private_data;

    index = (data->irq - info->irqs.msi_irq_base) / 32;
    status = 1 << ((data->irq - info->irqs.msi_irq_base) % 32);

    spin_lock_irqsave(&info->spinlock, irq_flags);
    pcie_app_req_clk(info->hw_pci.domain, 1);
    dbi_enable(info->hw_pci.domain);
    value = readl(info->virt_rc_cfg_addr + 0x700 + 0x12C + index*0x0C);
    value |= status;
    writel(value, info->virt_rc_cfg_addr + 0x700 + 0x12C + index*0x0C);
    dbi_disable(info->hw_pci.domain);
    pcie_app_req_clk(info->hw_pci.domain, 0);
    spin_unlock_irqrestore(&info->spinlock, irq_flags);
}

static void msi_unmask(struct irq_data *data)
{
    u32 value = 0;
    u32 index = 0;
    u32 status = 0;
    unsigned long irq_flags = 0;

    struct msi_desc *desc = NULL;
    struct balong_pcie *info = NULL;

    desc = irq_data_get_msi(data);
    info = (struct balong_pcie *)((struct pci_sys_data *)desc->dev->bus->sysdata)->private_data;

    index = (data->irq - info->irqs.msi_irq_base) / 32;
    status = 1 << ((data->irq - info->irqs.msi_irq_base) % 32);

    spin_lock_irqsave(&info->spinlock, irq_flags);
    pcie_app_req_clk(info->hw_pci.domain, 1);
    dbi_enable(info->hw_pci.domain);
    value = readl(info->virt_rc_cfg_addr + 0x700 + 0x12C + index*0x0C);
    value &= ~status;
    writel(value, info->virt_rc_cfg_addr + 0x700 + 0x12C + index*0x0C);
    dbi_disable(info->hw_pci.domain);
    pcie_app_req_clk(info->hw_pci.domain, 0);
    spin_unlock_irqrestore(&info->spinlock, irq_flags);
}

static void msi_enable(struct irq_data *data)
{
    u32 value = 0;
    u32 index = 0;
    u32 status = 0;
    unsigned long irq_flags = 0;

    struct msi_desc *desc = NULL;
    struct balong_pcie *info = NULL;

    desc = irq_data_get_msi(data);
    info = (struct balong_pcie *)((struct pci_sys_data *)desc->dev->bus->sysdata)->private_data;

    index = (data->irq - info->irqs.msi_irq_base) / 32;
    status = 1 << ((data->irq - info->irqs.msi_irq_base) % 32);

    spin_lock_irqsave(&info->spinlock, irq_flags);
    pcie_app_req_clk(info->hw_pci.domain, 1);
    dbi_enable(info->hw_pci.domain);
    value = readl(info->virt_rc_cfg_addr + 0x700 + 0x128 + index*0x0C);
    value |= status;
    writel(value, info->virt_rc_cfg_addr + 0x700 + 0x128 + index*0x0C);
    dbi_disable(info->hw_pci.domain);
    pcie_app_req_clk(info->hw_pci.domain, 0);
    spin_unlock_irqrestore(&info->spinlock, irq_flags);
}

static void msi_disable(struct irq_data *data)
{
    u32 value = 0;
    u32 index = 0;
    u32 status = 0;
    unsigned long irq_flags = 0;

    struct msi_desc *desc = NULL;
    struct balong_pcie *info = NULL;

    desc = irq_data_get_msi(data);
    info = (struct balong_pcie *)((struct pci_sys_data *)desc->dev->bus->sysdata)->private_data;

    index = (data->irq - info->irqs.msi_irq_base) / 32;
    status = 1 << ((data->irq - info->irqs.msi_irq_base) % 32);

    spin_lock_irqsave(&info->spinlock, irq_flags);
    pcie_app_req_clk(info->hw_pci.domain, 1);
    dbi_enable(info->hw_pci.domain);
    value = readl(info->virt_rc_cfg_addr + 0x700 + 0x128 + index*0x0C);
    value &= ~status;
    writel(value, info->virt_rc_cfg_addr + 0x700 + 0x128 + index*0x0C);
    dbi_disable(info->hw_pci.domain);
    pcie_app_req_clk(info->hw_pci.domain, 0);
    spin_unlock_irqrestore(&info->spinlock, irq_flags);
}

/**
 * get_free_msi() - Get a free MSI number
 *
 * Checks for availability of MSI and returns the first available.
 */
static unsigned long get_free_msi(void)
{
    unsigned long offset = 0;

    do
    {
        offset = find_first_zero_bit(msi_irq_bits, MAX_MSI_NUM);
        if (offset >= MAX_MSI_NUM)
            return offset;
    } while (test_and_set_bit(offset, msi_irq_bits));

    return offset;
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

#if 0
int arch_setup_msi_irqs(struct pci_dev *dev, int nvec, int type)
{
    return 0;
}
#endif

int arch_setup_msi_irq(struct pci_dev *pdev, struct msi_desc *desc)
{
    int ret = 0;
    unsigned int irq;
    unsigned long offset;

    struct msi_msg msg;
    struct balong_pcie *info;

    if ((!pdev) || (!pdev->bus) || (!pdev->bus->sysdata) ||
        (!((struct pci_sys_data *)pdev->bus->sysdata)->private_data))
    {
        pcie_trace("error args\n");
        return -1;
    }

    info = (struct balong_pcie *)((struct pci_sys_data *)pdev->bus->sysdata)->private_data;

    offset = get_free_msi();
    if (offset >= MAX_MSI_NUM)
    {
        pcie_trace("fail to get free MSI\n");
        return -1;
    }
    else
    {
        msg.data = offset;
        irq = info->irqs.msi_irq_base + offset;

        dynamic_irq_init(irq);

        ret = irq_set_msi_desc(irq, desc);
        if (!ret)
        {
            msg.address_hi = BALONG_PCIE0_MSI_PCI_UPPER_ADDR;
            msg.address_lo = BALONG_PCIE0_MSI_PCI_LOWER_ADDR;

            /* pcie_trace("MSI %d, irq = %d\n", msg.data, irq); */

            write_msi_msg(irq, &msg);
            irq_set_chip_and_handler_name(irq,
                &balong_msi_chip, handle_level_irq, NULL);
            set_irq_flags(irq, IRQF_VALID);
        }
    }

    return ret;
}

void arch_teardown_msi_irq(unsigned int irq)
{
    u32 msi_num = 0;
    struct balong_pcie *info = NULL;

    info = (struct balong_pcie *)irq_get_chip_data(irq);
    msi_num = irq - info->irqs.msi_irq_base;

    dynamic_irq_cleanup(irq);
    clear_bit(msi_num, msi_irq_bits);
}
#endif


int pcie_dma_transfer(struct pcie_dma_transfer_info *transfer_info)
{
    int ret = 0;
    struct balong_pcie *info = NULL;

    if ((NULL == transfer_info) || (NULL == transfer_info->dev))
    {
        pcie_trace("error args, transfer_info is NULL\n");
        return -1;
    }

    if (transfer_info->channel > BALONG_PCIE0_MAX_DMA_CHANNEL_NUM)
    {
        pcie_trace("error channel index, the max channel index is %u\n", BALONG_PCIE0_MAX_DMA_CHANNEL_NUM);
        return -1;
    }

    info = (struct balong_pcie *)((struct pci_sys_data *)transfer_info->dev->bus->sysdata)->private_data;

    while(down_interruptible(&info->irqs.dma_semaphore[transfer_info->channel])) ;
    pcie_app_req_clk(info->hw_pci.domain, 1);

    info->irqs.dma_channel_state |= (1 << transfer_info->channel);
    info->irqs.dma_int_callback[transfer_info->channel] = transfer_info->callback;
    info->irqs.dma_int_callback_args[transfer_info->channel] = transfer_info->callback_args;

    if (PCIE_DMA_DIRECTION_READ == transfer_info->direction)
    {
        /* engine enable */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x29C, 0x01);
        /* interrupt mask */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x318, 0x00);
        /* channel context */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x36C, transfer_info->channel | 0x80000000);
        /* channel control 1 */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x370, 0x04000008);
        /* transfer size */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x378, transfer_info->transfer_size);
        /* transfer address */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x37C, transfer_info->sar_low);
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x380, transfer_info->sar_high);
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x384, transfer_info->dar_low);
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x388, transfer_info->dar_high);
        /* doorbell */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x2A0, transfer_info->channel);
    }
    else    /* DMA write */
    {
        /* engine enable */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x27C, 0x01);
        /* interrupt mask */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x2C4, 0x00);
        /* channel context */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x36C, transfer_info->channel);
        /* channel control 1 */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x370, 0x04000008);
        /* transfer size */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x378, transfer_info->transfer_size);
        /* transfer address */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x37C, transfer_info->sar_low);
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x380, transfer_info->sar_high);
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x384, transfer_info->dar_low);
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x388, transfer_info->dar_high);
        /* doorbell */
        ret |= pci_write_config_dword(transfer_info->dev, 0x700 + 0x280, transfer_info->channel);
    }

    return ret;
}


static int __init balong_pcie_init(void)
{
    u32 id = 0;
    int ret = 0;

    u32 init_mode;

    pcie_common_init();

    /* there are two loops, the first one is for EP and the second one is for RC.
     * we must init EP at first to ensure that the EP is ready when the RC start the ltssm.
     */
    init_mode = PCIE_WORK_MODE_EP;
    for (id = 0; id < sizeof(balong_pcie)/sizeof(balong_pcie[0]); id++)
    {
        if (!balong_pcie[id].enabled)
            continue;

        if (balong_pcie[id].work_mode != init_mode)
            continue;

        ret = pcie_software_init(id);
        if (ret)
            continue;
        pcie_hardware_init(id);
        pcie_wait_for_linkup(id);
    }

    init_mode = PCIE_WORK_MODE_RC;
    for (id = 0; id < sizeof(balong_pcie)/sizeof(balong_pcie[0]); id++)
    {
        if (!balong_pcie[id].enabled)
            continue;

        if (balong_pcie[id].work_mode != init_mode)
            continue;

        ret = pcie_software_init(id);
        if (ret)
            continue;

        ret = pcie_global_reset(id);
        if (ret)
            pcie_trace("fail to assert a perst, controller: %u\n", id);

        pcie_hardware_init(id);
        pcie_wait_for_linkup(id);

        if (balong_pcie[id].linked)
        {
            pcie_try_change_speed(id);
            pcie_show_link_status(id);
            /* wait for all devices ready to accept configuration request */
            mdelay(1000);
        }
        else
        {
            pcie_trace("link up time out, controller: %u\n", id);
        }

        pci_common_init(&balong_pcie[id].hw_pci);
    }

    return ret;
}


subsys_initcall_sync(balong_pcie_init);


int d_rc_config(u32 id, u32 offset)
{
    u32 result = 0;
    unsigned long irq_flags = 0;

    offset &= (~(0x03));

    spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);

    pcie_app_req_clk(id, 1);

    dbi_enable(id);

    result = readl(balong_pcie[id].virt_rc_cfg_addr + offset);

    printk(KERN_ERR"%08X: 0x%08X\n", balong_pcie[id].phys_rc_cfg_addr + offset, result);

    dbi_disable(id);

    pcie_app_req_clk(id, 0);

    spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);

    return 0;
}

int m_rc_config(u32 id, u32 offset, u32 value)
{
    unsigned long irq_flags = 0;

    spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);

    pcie_app_req_clk(id, 1);

    dbi_enable(id);

    writel(value, balong_pcie[id].virt_rc_cfg_addr + offset);

    dbi_disable(id);

    pcie_app_req_clk(id, 0);

    spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);

    return 0;
}

int pcie_info(void)
{
    u32 id = 0;
    u32 value = 0;

    for (id = 0; id < sizeof(balong_pcie)/sizeof(balong_pcie[0]); id++)
    {
        pcie_trace("PCIe RC ID %u\n", id);
        pcie_trace("PCIe RC Config Space VirtAddr: 0x%08X\n", balong_pcie[id].virt_rc_cfg_addr);

        pcie_link_up_check(id);
        if (balong_pcie[id].linked)
        {
            pcie_show_link_status(id);
        }
        else
        {
            pcie_trace("link down\n");
        }

        value = hi_pcie_get_link_status(id);
        pcie_trace("PCIe Controller Status: 0x%08X\n", value);
    }

    return 0;
}


