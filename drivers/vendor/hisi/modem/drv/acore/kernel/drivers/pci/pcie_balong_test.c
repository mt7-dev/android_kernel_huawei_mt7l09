#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/io.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/printk.h>
#include <linux/random.h>
#include <linux/jiffies.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#include <linux/gpio.h>

#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/pci_regs.h>

#include <asm/mach/pci.h>

#include <bsp_memmap.h>

#include <hi_pcie_interface.h>

#include <product_config.h>

#include "pcie_balong.h"


#define PCIE_TEST_BUFFER_ADDR       (DDR_GU_ADDR)
#define PCIE_TEST_BUFFER_SIZE       (1* 1024 * 1024)

#define PCIE_TEST_INT_MODE_INTX (0)
#define PCIE_TEST_INT_MODE_MSI  (1)
#define PCIE_TEST_INT_MODE_MSIX (2)
#define PCIE_TEST_INT_MODE_MAX  (PCIE_TEST_INT_MODE_MSIX)
#if defined(CONFIG_PCI_MSI)
#define PCIE_TEST_INT_MODE_DEFAULT  (PCIE_TEST_INT_MODE_MSI)
#else
#define PCIE_TEST_INT_MODE_DEFAULT  (PCIE_TEST_INT_MODE_INTX)
#endif

struct pcie_info
{
    struct pci_dev* balong_rc;
    struct pci_dev* balong_ep;

    u32 rc_vendor_id;   /* RC's vendor id */
    u32 rc_device_id;   /* RC's device id */
    u32 ep_vendor_id;   /* EP's vendor id */
    u32 ep_device_id;   /* EP's device id */

    u32 loopback_enable;

    u32 buffer_size;
    void *rc_phys_addr; /* RC buffer's CPU physical address */
    void *rc_virt_addr;
    void *ep_phys_addr; /* EP buffer's CPU physical address */
    void *ep_virt_addr;

    void *ep_in_rc_cpu_phys_addr;       /* EP buffer's PCI physical address (CPU domain) */
    void *ep_in_rc_cpu_virt_addr;
    void *ep_in_rc_pci_phys_addr;       /* EP buffer's PCI physical address (PCI domain) */
    void *ep_in_rc_cpu_sc_phys_addr;    /* EP sysctrl's CPU physical address */
    void *ep_in_rc_cpu_sc_virt_addr;

    volatile u32 interrupt_mode;        /* INTX, MSI or MSI-X */

    u32 msi_intx_interrupt_send_count;  /* MSI/MSIX/INTX interrupt send and recieve count */
    u32 msi_intx_interrupt_recieve_count;

    u32 dma_interrupt_done_number;      /* DMA interrupt number */
    u32 dma_interrupt_abort_number;

    struct semaphore dma_read_semaphore;
    struct semaphore dma_write_semaphore;

    struct pcie_dma_transfer_info dma_transfer_info;
};

struct pcie_info g_pcie_info[] =
{
#if defined(BALONG_PCIE0_EXISTS)
    {
        .balong_rc = NULL,
        .balong_ep = NULL,

        .rc_vendor_id = BALONG_PCI_VENDOR_ID,
        .rc_device_id = BALONG_PCI_DEVICE_ID,
        .ep_vendor_id = BALONG_PCI_VENDOR_ID,
        .ep_device_id = BALONG_PCI_DEVICE_ID,

        .loopback_enable = 0,

        .buffer_size = PCIE_TEST_BUFFER_SIZE,
        .rc_phys_addr = (void*)(PCIE_TEST_BUFFER_ADDR),
        .rc_virt_addr = NULL,
        .ep_phys_addr = (void*)(PCIE_TEST_BUFFER_ADDR + PCIE_TEST_BUFFER_SIZE),
        .ep_virt_addr = NULL,

        .ep_in_rc_cpu_phys_addr = NULL,
        .ep_in_rc_cpu_virt_addr = NULL,
        .ep_in_rc_pci_phys_addr = NULL,
        .ep_in_rc_cpu_sc_phys_addr = NULL,
        .ep_in_rc_cpu_sc_virt_addr = NULL,

        .interrupt_mode = PCIE_TEST_INT_MODE_DEFAULT,

        .msi_intx_interrupt_send_count = 0,
        .msi_intx_interrupt_recieve_count = 0,

        .dma_interrupt_done_number = 0,
        .dma_interrupt_abort_number = 0,

        .dma_transfer_info.channel = 0,
    },
#endif
#if defined(BALONG_PCIE1_EXISTS)
    {
        .balong_rc = NULL,
        .balong_ep = NULL,

        .rc_vendor_id = BALONG_PCI_VENDOR_ID,
        .rc_device_id = BALONG_PCI_DEVICE_ID,
        .ep_vendor_id = BALONG_PCI_VENDOR_ID,
        .ep_device_id = BALONG_PCI_DEVICE_ID,

        .loopback_enable = 0,

        .buffer_size = PCIE_TEST_BUFFER_SIZE,
        .rc_phys_addr = (void*)(PCIE_TEST_BUFFER_ADDR + PCIE_TEST_BUFFER_SIZE),
        .rc_virt_addr = NULL,
        .ep_phys_addr = (void*)(PCIE_TEST_BUFFER_ADDR + 2 * PCIE_TEST_BUFFER_SIZE),
        .ep_virt_addr = NULL,

        .ep_in_rc_cpu_phys_addr = NULL,
        .ep_in_rc_cpu_virt_addr = NULL,
        .ep_in_rc_pci_phys_addr = NULL,
        .ep_in_rc_cpu_sc_phys_addr = NULL,
        .ep_in_rc_cpu_sc_virt_addr = NULL,

        .interrupt_mode = PCIE_TEST_INT_MODE_DEFAULT,

        .msi_intx_interrupt_send_count = 0,
        .msi_intx_interrupt_recieve_count = 0,

        .dma_interrupt_done_number = 0,
        .dma_interrupt_abort_number = 0,

        .dma_transfer_info.channel = 0,
    },
#endif
};


int pcie_test_info(void)
{
    u32 i = 0;

    for (i = 0; i < sizeof(g_pcie_info)/sizeof(g_pcie_info[0]); i++)
    {
        printk(KERN_ERR"Controller %u\n", i);
        printk(KERN_ERR"RC's Vendor ID: 0x%04X, Device ID: 0x%04X, PCI_DEV @0x%08X\n",
            g_pcie_info[i].rc_vendor_id, g_pcie_info[i].rc_device_id, (u32)g_pcie_info[i].balong_rc);

        printk(KERN_ERR"EP's Vendor ID: 0x%04X, Device ID: 0x%04X, PCI_DEV @0x%08X\n",
            g_pcie_info[i].ep_vendor_id, g_pcie_info[i].ep_device_id, (u32)g_pcie_info[i].balong_ep);

        printk(KERN_ERR"PCIe Test buffer size: 0x%08X\n", g_pcie_info[i].buffer_size);

        printk(KERN_ERR"RC buffer's CPU physical address: 0x%08X, virtual address: 0x%08X\n",
            (u32)g_pcie_info[i].rc_phys_addr, (u32)g_pcie_info[i].rc_virt_addr);

        printk(KERN_ERR"EP buffer's CPU physical address: 0x%08X, virtual address: 0x%08X\n",
            (u32)g_pcie_info[i].ep_phys_addr, (u32)g_pcie_info[i].ep_virt_addr);

        printk(KERN_ERR"EP buffer's PCI physical address: 0x%08X, virtual address: 0x%08X\n",
            (u32)g_pcie_info[i].ep_in_rc_cpu_phys_addr, (u32)g_pcie_info[i].ep_in_rc_cpu_virt_addr);

        printk(KERN_ERR"EP buffer's PCI physical address in PCI domain: 0x%08X\n",
            (u32)g_pcie_info[i].ep_in_rc_pci_phys_addr);

        printk(KERN_ERR"EP sysctrl's PCI physical address: 0x%08X, virtual address: 0x%08X\n",
            (u32)g_pcie_info[i].ep_in_rc_cpu_sc_phys_addr, (u32)g_pcie_info[i].ep_in_rc_cpu_sc_virt_addr);

        printk(KERN_ERR"DMA interrupt done number: %u, abort number: %u\n",
            g_pcie_info[i].dma_interrupt_done_number, g_pcie_info[i].dma_interrupt_abort_number);

        if (g_pcie_info[i].loopback_enable)
            printk(KERN_ERR"PCIe loopback is enabled\n\n");
        else
            printk(KERN_ERR"PCIe loopback is disabled\n\n");
    }
    return 0;
}

static void calc_rate(char* description,    /* print description */
                      u32 trans_count,      /* transfer count */
                      u32 block_size,       /* transfer's block size */
                      u32 start_time,       /* transfer's start time */
                      u32 end_time)         /* transfer's end time */
{
    /* HZ: system timer interrupt number per seconds */
    u32 cost_ms = (end_time - start_time) * 1000 / HZ;

    u32 mbyte_size = (trans_count * block_size) >> 20;
    u32 gbyte_size = (trans_count * block_size) >> 30;

    u32 mbit_rate = (cost_ms) ? (mbyte_size * 8 * 1000) / (cost_ms) : 0;
    u32 gbit_rate = (cost_ms) ? (mbyte_size * 8) / (cost_ms) : 0;

    printk(KERN_ERR"%s total size: %u MB(%u GB), cost times: %u ms, rate: %u Mb/s(%u Gb/s)\n",
        description, mbyte_size, gbyte_size, cost_ms, mbit_rate, gbit_rate);
}

static u32 get_random(u32 base, u32 scope)
{
    u32 random = 0;

    if (!scope)
        return base;

    get_random_bytes((void*)&random, (int)sizeof(random));

    random = random % scope + base;

    return random;
}

static irqreturn_t msi_intx_isr(int irq, void* dev_info)
{
    struct balong_pcie *info = (struct balong_pcie *)dev_info;

    g_pcie_info[info->hw_pci.domain].msi_intx_interrupt_recieve_count++;

    writel(0x0, g_pcie_info[info->hw_pci.domain].ep_in_rc_cpu_sc_virt_addr + HI_PCIE_SC_ASSERT_INT_OFFSET);

    printk(KERN_ERR"received msi/intx irq %d: send count %u, recieve count %u\n", irq,
                    g_pcie_info[info->hw_pci.domain].msi_intx_interrupt_send_count,
                    g_pcie_info[info->hw_pci.domain].msi_intx_interrupt_recieve_count);

    return IRQ_HANDLED;
}

static void dma_callback(u32 direction, u32 status, void* dev_info)
{
    struct balong_pcie *info = (struct balong_pcie *)dev_info;

    if (status)
    {
        g_pcie_info[info->hw_pci.domain].dma_interrupt_abort_number++;
        pcie_trace("DMA transfer abort, status: 0x%08X\n", status);
    }
    else
    {
        g_pcie_info[info->hw_pci.domain].dma_interrupt_done_number++;
    }

    /* the read/write semaphore is for calc rate */
    if (PCIE_DMA_DIRECTION_READ == direction)
        up(&g_pcie_info[info->hw_pci.domain].dma_read_semaphore);
    else
        up(&g_pcie_info[info->hw_pci.domain].dma_write_semaphore);
}

/* Attention: the dbi must be enabled when this function is called */
u32 get_iatu_index(u32 id, u32 direction)
{
    u32 index = 0;
    struct balong_pcie_iatu_table iatu_table;

    if (direction == PCIE_TLP_DIRECTION_INBOUND)
        index |= 0x80000000;

    while (1)
    {
        writel(index, balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x200);

        iatu_table.control2.value = readl(balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x208);

        if (!iatu_table.control2.attr.enable)
            break;

        index++;
    }

    return index;
}

static int rc_to_ep_by_cpu(u32 id, void* destination, void* source, u32 size)
{
    pcie_app_req_clk(id, 1);
    memcpy(destination, source, size);
    pcie_app_req_clk(id, 0);

    return 0;
}

static int ep_to_rc_by_cpu(u32 id, void* destination, void* source, u32 size)
{
    pcie_app_req_clk(id, 1);
    memcpy(destination, source, size);
    pcie_app_req_clk(id, 0);

    return 0;
}

static int rc_to_ep_by_dma(u32 id, void* destination, void* source, u32 size)
{
    int ret = 0;

    g_pcie_info[id].dma_transfer_info.direction = PCIE_DMA_DIRECTION_WRITE;
    g_pcie_info[id].dma_transfer_info.dar_high = 0;
    g_pcie_info[id].dma_transfer_info.dar_low = (u32)destination;
    g_pcie_info[id].dma_transfer_info.sar_high = 0;
    g_pcie_info[id].dma_transfer_info.sar_low = (u32)source;
    g_pcie_info[id].dma_transfer_info.transfer_size = size;
    g_pcie_info[id].dma_transfer_info.callback = dma_callback;
    g_pcie_info[id].dma_transfer_info.callback_args = (void*)&balong_pcie[id];

    ret = pcie_dma_transfer(&g_pcie_info[id].dma_transfer_info);

    if (!ret)
        while(down_interruptible(&g_pcie_info[id].dma_write_semaphore)) ;

    return ret;
}

static int ep_to_rc_by_dma(u32 id, void* destination, void* source, u32 size)
{
    int ret = 0;

    g_pcie_info[id].dma_transfer_info.direction = PCIE_DMA_DIRECTION_READ;
    g_pcie_info[id].dma_transfer_info.dar_high = 0;
    g_pcie_info[id].dma_transfer_info.dar_low = (u32)destination;
    g_pcie_info[id].dma_transfer_info.sar_high = 0;
    g_pcie_info[id].dma_transfer_info.sar_low = (u32)source;
    g_pcie_info[id].dma_transfer_info.transfer_size = size;
    g_pcie_info[id].dma_transfer_info.callback = dma_callback;
    g_pcie_info[id].dma_transfer_info.callback_args = (void*)&balong_pcie[id];

    ret = pcie_dma_transfer(&g_pcie_info[id].dma_transfer_info);

    if (!ret)
        while(down_interruptible(&g_pcie_info[id].dma_read_semaphore)) ;

    return ret;
}

int rate_rc_to_ep_by_cpu(u32 id, u32 total_size, u32 block_size)
{
    u32 trans_times = 0;
    u32 start_time, end_time;

    if (id >= sizeof(g_pcie_info)/sizeof(g_pcie_info[0]))
    {
        pcie_trace("controller id error, id %u\n", id);
        return -1;
    }

    if (block_size > g_pcie_info[id].buffer_size)
    {
        pcie_trace("block size is too big, the max block size is limit to 0x%08X\n", g_pcie_info[id].buffer_size);
        return -1;
    }

    start_time = jiffies;
    while (total_size >= block_size)
    {
        (void)rc_to_ep_by_cpu(id, g_pcie_info[id].ep_in_rc_cpu_virt_addr, g_pcie_info[id].rc_virt_addr, block_size);
        total_size -= block_size;
        trans_times++;
    }
    end_time = jiffies;

    calc_rate("Write to EP by CPU:", trans_times, block_size, start_time, end_time);

    return 0;
}

int rate_ep_to_rc_by_cpu(u32 id, u32 total_size, u32 block_size)
{
    u32 trans_times = 0;
    u32 start_time, end_time;

    if (id >= sizeof(g_pcie_info)/sizeof(g_pcie_info[0]))
    {
        pcie_trace("controller id error, id %u\n", id);
        return -1;
    }

    if (block_size > g_pcie_info[id].buffer_size)
    {
        pcie_trace("block size is too big, the max block size is limit to 0x%08X\n", g_pcie_info[id].buffer_size);
        return -1;
    }

    start_time = jiffies;
    while (total_size >= block_size)
    {
        (void)ep_to_rc_by_cpu(id, g_pcie_info[id].rc_virt_addr, g_pcie_info[id].ep_in_rc_cpu_virt_addr, block_size);
        total_size -= block_size;
        trans_times++;
    }
    end_time = jiffies;

    calc_rate("Read from EP by CPU:", trans_times, block_size, start_time, end_time);

    return 0;
}

int rate_rc_to_ep_by_dma(u32 id, u32 total_size, u32 block_size)
{
    int ret = 0;
    u32 trans_times = 0;
    u32 start_time, end_time;

    if (id >= sizeof(g_pcie_info)/sizeof(g_pcie_info[0]))
    {
        pcie_trace("controller id error, id %u\n", id);
        return -1;
    }

    if (block_size > g_pcie_info[id].buffer_size)
    {
        pcie_trace("block size is too big, the max block size is limit to 0x%08X\n", g_pcie_info[id].buffer_size);
        return -1;
    }

    start_time = jiffies;
    while (total_size >= block_size)
    {
        ret = rc_to_ep_by_dma(id, g_pcie_info[id].ep_in_rc_pci_phys_addr, g_pcie_info[id].rc_phys_addr, block_size);
        if (ret)
            return ret;
        total_size -= block_size;
        trans_times++;
    }
    end_time = jiffies;

    calc_rate("Write to EP by DMA:", trans_times, block_size, start_time, end_time);

    return 0;
}

int rate_ep_to_rc_by_dma(u32 id, u32 total_size, u32 block_size)
{
    int ret = 0;
    u32 trans_times = 0;
    u32 start_time, end_time;

    if (id >= sizeof(g_pcie_info)/sizeof(g_pcie_info[0]))
    {
        pcie_trace("controller id error, id %u\n", id);
        return -1;
    }

    if (block_size > g_pcie_info[id].buffer_size)
    {
        pcie_trace("block size is too big, the max block size is limit to 0x%08X\n", g_pcie_info[id].buffer_size);
        return -1;
    }

    start_time = jiffies;
    while (total_size >= block_size)
    {
        ret = ep_to_rc_by_dma(id, g_pcie_info[id].rc_phys_addr, g_pcie_info[id].ep_in_rc_pci_phys_addr, block_size);
        if (ret)
            return ret;
        total_size -= block_size;
        trans_times++;
    }
    end_time = jiffies;

    calc_rate("Read from EP by DMA:", trans_times, block_size, start_time, end_time);

    return 0;
}

int pcie_set_id(u32 id, u32 rc_vendor_id, u32 rc_device_id, u32 ep_vendor_id, u32 ep_device_id)
{
    if (id >= sizeof(g_pcie_info)/sizeof(g_pcie_info[0]))
    {
        pcie_trace("controller id error, id %u\n", id);
        return -1;
    }

    g_pcie_info[id].rc_vendor_id = rc_vendor_id & 0x0000FFFF;
    g_pcie_info[id].rc_device_id = rc_device_id & 0x0000FFFF;
    g_pcie_info[id].ep_vendor_id = ep_vendor_id & 0x0000FFFF;
    g_pcie_info[id].ep_device_id = ep_device_id & 0x0000FFFF;

    return 0;
}

int pcie_set_dma(u32 id, u32 is_local_dma, u32 channel)
{
    if (id >= sizeof(g_pcie_info)/sizeof(g_pcie_info[0]))
    {
        pcie_trace("controller id error, id %u\n", id);
        return -1;
    }

    if (!g_pcie_info[id].balong_ep)
    {
        pcie_trace("the EP doesn't exists, force to use local DMA\n");
        return 0;
    }

    if (is_local_dma)
        g_pcie_info[id].dma_transfer_info.dev = g_pcie_info[id].balong_rc;
    else
        g_pcie_info[id].dma_transfer_info.dev = g_pcie_info[id].balong_ep;

    g_pcie_info[id].dma_transfer_info.channel = channel;

    return 0;
}

int pcie_set_buffer(u32 id, void* rc_addr, void* ep_addr, u32 size)
{
    if (id >= sizeof(g_pcie_info)/sizeof(g_pcie_info[0]))
    {
        pcie_trace("controller id error, id %u\n", id);
        return -1;
    }

    g_pcie_info[id].rc_phys_addr = rc_addr;
    g_pcie_info[id].ep_phys_addr = ep_addr;
    g_pcie_info[id].buffer_size  = size;

    return 0;
}

int pcie_set_int_mode(u32 id, u32 interrupt_mode)
{
    int ret = 0;

    if (id >= sizeof(g_pcie_info)/sizeof(g_pcie_info[0]))
    {
        pcie_trace("controller id error, id %u\n", id);
        return -1;
    }

#if !defined(CONFIG_PCI_MSI)
    ret = -1;
    pcie_trace("the MSI is not enable, force to use INTX\n");
#else
    if (interrupt_mode <= PCIE_TEST_INT_MODE_MAX)
        g_pcie_info[id].interrupt_mode = interrupt_mode;
    else
    {
        ret = -1;
        pcie_trace("invalid interrupt mode: 0x%X\n", interrupt_mode);
    }
#endif

    return ret;
}

struct pci_dev* pcie_get_rc(u32 id, u32 vendor_id, u32 device_id)
{
    struct pci_dev *pdev = NULL;

    do
    {
        pdev = pci_get_device(vendor_id, device_id, pdev);
        if (pdev && ((pdev->class >> 8) == PCI_CLASS_BRIDGE_PCI) && (pci_domain_nr(pdev->bus) == (int)id))
            break;
    } while (pdev != NULL);

    return pdev;
}

struct pci_dev* pcie_get_dev(u32 id, u32 vendor_id, u32 device_id)
{
    struct pci_dev *pdev = NULL;

    do
    {
        pdev = pci_get_device(vendor_id, device_id, pdev);
        if (pdev && ((pdev->class >> 8) != PCI_CLASS_BRIDGE_PCI) && (pci_domain_nr(pdev->bus) == (int)id))
            break;
    } while (pdev != NULL);

    return pdev;
}

static void pcie_test_int_init(u32 id)
{
    u32 i = 0;
    int ret = 0;
    struct msix_entry entries[1];

    for (i = 0; i < sizeof(entries)/sizeof(entries[0]); i++)
        entries[i].entry = i;

    switch (g_pcie_info[id].interrupt_mode)
    {
        case PCIE_TEST_INT_MODE_INTX:
            if (request_irq(g_pcie_info[id].balong_ep->irq, msi_intx_isr, IRQF_SHARED,
                            "PCIe INTX", (void*)&balong_pcie[id]))
            {
                pcie_trace("request_irq fail, intx irq = %d\n", g_pcie_info[id].balong_ep->irq);
                return;
            }

            if (request_irq(g_pcie_info[id].balong_ep->irq, msi_intx_isr, IRQF_SHARED,
                            "PCIe INTX", (void*)&balong_pcie[id]))
            {
                pcie_trace("request_irq fail, msi irq = %d\n", g_pcie_info[id].balong_ep->irq);
                return;
            }

            break;
        case PCIE_TEST_INT_MODE_MSI:
            ret = pci_enable_msi(g_pcie_info[id].balong_ep);
            if (ret != 0)
            {
                pcie_trace("pci_enable_msi fail, ret = %d\n", ret);
                return;
            }

            if (request_irq(g_pcie_info[id].balong_ep->irq, msi_intx_isr, IRQF_SHARED,
                            "PCIe MSI", (void*)&balong_pcie[id]))
            {
                pcie_trace("request_irq fail, msi irq = %d\n", g_pcie_info[id].balong_ep->irq);
                return;
            }

            break;
        case PCIE_TEST_INT_MODE_MSIX:
            ret = pci_enable_msix(g_pcie_info[id].balong_ep, &entries[0], sizeof(entries)/sizeof(entries[0]));
            if (ret != 0)
            {
                pcie_trace("pci_enable_msix fail, ret = %d\n", ret);
                return;
            }

            for (i = 0; i < sizeof(entries)/sizeof(entries[0]); i++)
            {
                if (request_irq(entries[i].vector, msi_intx_isr, IRQF_SHARED,
                                "PCIe MSI", (void*)&balong_pcie[id]))
                {
                    pcie_trace("request_irq fail, msi irq = %d\n", entries[i].vector);
                    return;
                }
            }

            break;
    }
}

static void pcie_test_rc_init(u32 id)
{
    /* EP buffer's CPU address */
    void* cpu_phys_start_addr = 0;
    void* cpu_virt_start_addr = 0;
    u32 ep_buffer_size = 0;

    sema_init(&g_pcie_info[id].dma_read_semaphore, 0);
    sema_init(&g_pcie_info[id].dma_write_semaphore, 0);

    g_pcie_info[id].balong_rc = pcie_get_rc(id, g_pcie_info[id].rc_vendor_id, g_pcie_info[id].rc_device_id);
    if(NULL == g_pcie_info[id].balong_rc)
    {
        pcie_trace("can't find balong pcie rc\n");
        return;
    }
    g_pcie_info[id].dma_transfer_info.dev = g_pcie_info[id].balong_rc;

    g_pcie_info[id].balong_ep = pcie_get_dev(id, g_pcie_info[id].ep_vendor_id, g_pcie_info[id].ep_device_id);
    if(NULL == g_pcie_info[id].balong_ep)
    {
        pcie_trace("can't find balong pcie ep\n");
        return;
    }

    if (pci_enable_device(g_pcie_info[id].balong_ep))
    {
        pcie_trace("pci_enable_device fail\n");
        return;
    }

    pci_set_master(g_pcie_info[id].balong_ep);

    cpu_phys_start_addr = (void*)pci_resource_start(g_pcie_info[id].balong_ep, 0);
    if (!cpu_phys_start_addr)
    {
        pcie_trace("pci_resource_start fail\n");
        return;
    }
    pcie_trace("cpu_phys_start_addr: 0x%08X\n", (u32)cpu_phys_start_addr);

    /* pci_resource_len = pci_resource_end - pci_resource_start + 1 */
    ep_buffer_size = pci_resource_len(g_pcie_info[id].balong_ep, 0);
    if (!ep_buffer_size)
    {
        pcie_trace("pci_resource_len fail\n");
        return;
    }
    pcie_trace("ep_buffer_size: 0x%08X\n", ep_buffer_size);

    cpu_virt_start_addr = ioremap((u32)cpu_phys_start_addr, ep_buffer_size);
    if (!cpu_virt_start_addr)
    {
        pcie_trace("fail to ioremap, addr: 0x%08X\n", (u32)cpu_phys_start_addr);
        return;
    }
    pcie_trace("cpu_virt_start_addr: 0x%08X\n", (u32)cpu_virt_start_addr);

    g_pcie_info[id].ep_in_rc_cpu_phys_addr = cpu_phys_start_addr;
    g_pcie_info[id].ep_in_rc_cpu_virt_addr = cpu_virt_start_addr;

    /* in normal mode, pci domain address == cpu domain address */
    g_pcie_info[id].ep_in_rc_pci_phys_addr = g_pcie_info[id].ep_in_rc_cpu_phys_addr;

    cpu_phys_start_addr = (void*)pci_resource_start(g_pcie_info[id].balong_ep, 2);
    if (!cpu_phys_start_addr)
    {
        pcie_trace("pci_resource_start fail\n");
        return;
    }
    pcie_trace("cpu_phys_start_addr: 0x%08X\n", (u32)cpu_phys_start_addr);

    /* pci_resource_len = pci_resource_end - pci_resource_start + 1 */
    ep_buffer_size = pci_resource_len(g_pcie_info[id].balong_ep, 2);
    if (!ep_buffer_size)
    {
        pcie_trace("pci_resource_len fail\n");
        return;
    }
    pcie_trace("ep_buffer_size: 0x%08X\n", ep_buffer_size);

    cpu_virt_start_addr = ioremap((u32)cpu_phys_start_addr, ep_buffer_size);
    if (!cpu_virt_start_addr)
    {
        pcie_trace("fail to ioremap, addr: 0x%08X\n", (u32)cpu_phys_start_addr);
        return;
    }
    pcie_trace("cpu_virt_start_addr: 0x%08X\n", (u32)cpu_virt_start_addr);

    g_pcie_info[id].ep_in_rc_cpu_sc_phys_addr = cpu_phys_start_addr;
    g_pcie_info[id].ep_in_rc_cpu_sc_virt_addr = cpu_virt_start_addr;

    pcie_test_int_init(id);

    g_pcie_info[id].rc_virt_addr = (void*)ioremap_nocache((u32)g_pcie_info[id].rc_phys_addr, g_pcie_info[id].buffer_size);
    if (!g_pcie_info[id].rc_virt_addr)
    {
        pcie_trace("fail to ioremap, addr: 0x%08X\n", (u32)g_pcie_info[id].rc_phys_addr);
        return;
    }
    pcie_trace("g_pcie_rc_virt_addr: 0x%08X, g_pcie_rc_phys_addr: 0x%08X\n",
        (u32)g_pcie_info[id].rc_virt_addr, (u32)g_pcie_info[id].rc_phys_addr);

    get_random_bytes(g_pcie_info[id].rc_virt_addr, (int)g_pcie_info[id].buffer_size);
}

static void pcie_test_ep_init(u32 id)
{
    unsigned long irq_flags= 0;

    struct balong_pcie_iatu_table iatu_table[] = {
        {
            .index.attr.index = 0x0,
            .index.attr.direction = PCIE_TLP_DIRECTION_INBOUND,
            .control1.attr.type = PCIE_TLP_TYPE_MEM_RW,
            .control2.attr.message_code = 0,
            .control2.attr.bar_index    = 0,
            .control2.attr.reserved     = 0,
            .control2.attr.cfg_shift    = 0,
            .control2.attr.invert_mode  = 0,
            .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_BAR,
            .control2.attr.enable       = 1,
            .lower_addr = 0,
            .upper_addr = 0,
            .limit_addr = 0,
            .lower_target_addr = 0,
            .upper_target_addr = 0,
            .control3 = 0,
        },
        {
            .index.attr.index = 0x0,
            .index.attr.direction = PCIE_TLP_DIRECTION_INBOUND,
            .control1.attr.type = PCIE_TLP_TYPE_MEM_RW,
            .control2.attr.message_code = 0,
            .control2.attr.bar_index    = 2,
            .control2.attr.reserved     = 0,
            .control2.attr.cfg_shift    = 0,
            .control2.attr.invert_mode  = 0,
            .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_BAR,
            .control2.attr.enable       = 1,
            .lower_addr = 0,
            .upper_addr = 0,
            .limit_addr = 0,
            .lower_target_addr = 0,
            .upper_target_addr = 0,
            .control3 = 0,
        },
    };

    pcie_app_req_clk(id, 1);

    spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);
    dbi_enable(id);

    iatu_table[0].index.value = get_iatu_index(id, PCIE_TLP_DIRECTION_INBOUND);
    iatu_table[0].lower_target_addr = (u32)g_pcie_info[id].ep_phys_addr;

    iatu_table[1].index.value = iatu_table[0].index.value + 1;
    iatu_table[1].lower_target_addr = HI_PCIE_SC_BASE_ADDR;

    pcie_set_iatu(id, &iatu_table[0], sizeof(iatu_table)/sizeof(iatu_table[0]));

    dbi_disable(id);
    spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);

    pcie_app_req_clk(id, 0);
}

int pcie_test_init(void)
{
    u32 id = 0;
    u32 init_mode;

    static u32 is_inited = 0;

    /* there are two loops, the first one is for EP and the second one is for RC.
     * we must init EP at first to ensure that the EP is ready when the RC start the ltssm.
     */
    init_mode = PCIE_WORK_MODE_EP;
    for (id = 0; id < sizeof(g_pcie_info)/sizeof(g_pcie_info[0]); id++)
    {
        if (!balong_pcie[id].enabled)
            continue;

        if (balong_pcie[id].work_mode != init_mode)
            continue;

        pcie_test_ep_init(id);
    }

    /* the ep can be inited for multiple times */
    if (is_inited)
        return 0;

    is_inited = 1;

    init_mode = PCIE_WORK_MODE_RC;
    for (id = 0; id < sizeof(g_pcie_info)/sizeof(g_pcie_info[0]); id++)
    {
        if (!balong_pcie[id].enabled)
            continue;

        if (balong_pcie[id].work_mode != init_mode)
            continue;

        pcie_test_rc_init(id);
    }

    return 0;
}


#define PCIE_LOOPBACK_PCI_DOMAIN_ADDRESS    (0x60000000)

int pcie_loopback_init(void)
{
    u32 id = 0;
    u32 value = 0;
    unsigned long irq_flags= 0;

    struct balong_pcie_iatu_table iatu_table[] = {
        {
            .index.attr.index = 0x0,
            .index.attr.direction = PCIE_TLP_DIRECTION_OUTBOUND,
            .control1.attr.type = PCIE_TLP_TYPE_MEM_RW,
            .control2.attr.message_code = 0,
            .control2.attr.bar_index    = 0,
            .control2.attr.reserved     = 0,
            .control2.attr.cfg_shift    = 0,
            .control2.attr.invert_mode  = 0,
            .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
            .control2.attr.enable       = 1,
            .lower_addr = 0,
            .upper_addr = 0,
            .limit_addr = 0,
            .lower_target_addr = PCIE_LOOPBACK_PCI_DOMAIN_ADDRESS,
            .upper_target_addr = 0,
            .control3 = 0,
        },
        {
            .index.attr.index =    0x0,
            .index.attr.direction = PCIE_TLP_DIRECTION_INBOUND,
            .control1.attr.type = PCIE_TLP_TYPE_MEM_RW,
            .control2.attr.message_code = 0,
            .control2.attr.bar_index    = 0,
            .control2.attr.reserved     = 0,
            .control2.attr.cfg_shift    = 0,
            .control2.attr.invert_mode  = 0,
            .control2.attr.match_mode   = PCIE_IATU_MATCH_MODE_ADDR,
            .control2.attr.enable       = 1,
            .lower_addr = PCIE_LOOPBACK_PCI_DOMAIN_ADDRESS,
            .upper_addr = 0,
            .lower_target_addr = 0,
            .upper_target_addr = 0,
            .control3 = 0,
        },
    };

    for (id = 0; id < sizeof(g_pcie_info)/sizeof(g_pcie_info[0]); id++)
    {
        if (!balong_pcie[id].enabled)
            continue;

        sema_init(&g_pcie_info[id].dma_read_semaphore, 0);
        sema_init(&g_pcie_info[id].dma_write_semaphore, 0);

        g_pcie_info[id].balong_rc = pcie_get_rc(id, g_pcie_info[id].rc_vendor_id, g_pcie_info[id].rc_device_id);
        if(NULL == g_pcie_info[id].balong_rc)
        {
            pcie_trace("can't find balong pcie rc\n");
            return -1;
        }
        g_pcie_info[id].dma_transfer_info.dev = g_pcie_info[id].balong_rc;

        g_pcie_info[id].rc_virt_addr = (void*)ioremap_nocache((u32)g_pcie_info[id].rc_phys_addr, g_pcie_info[id].buffer_size);
        if (!g_pcie_info[id].rc_virt_addr)
        {
            pcie_trace("fail to ioremap, addr: 0x%08X\n", (u32)g_pcie_info[id].rc_phys_addr);
            return -1;
        }
        pcie_trace("g_pcie_rc_virt_addr: 0x%08X, g_pcie_rc_phys_addr: 0x%08X\n",
            (u32)g_pcie_info[id].rc_virt_addr, (u32)g_pcie_info[id].rc_phys_addr);

        get_random_bytes(g_pcie_info[id].rc_virt_addr, (int)g_pcie_info[id].buffer_size);

        g_pcie_info[id].ep_virt_addr = (void*)ioremap_nocache((u32)g_pcie_info[id].ep_phys_addr, g_pcie_info[id].buffer_size);
        if (!g_pcie_info[id].ep_virt_addr)
        {
            pcie_trace("fail to ioremap, addr: 0x%08X\n", (u32)g_pcie_info[id].ep_phys_addr);
            return -1;
        }
        pcie_trace("g_pcie_ep_virt_addr: 0x%08X, g_pcie_ep_phys_addr: 0x%08X\n",
            (u32)g_pcie_info[id].ep_virt_addr, (u32)g_pcie_info[id].ep_phys_addr);

        get_random_bytes(g_pcie_info[id].ep_virt_addr, (int)g_pcie_info[id].buffer_size);

        g_pcie_info[id].ep_in_rc_cpu_phys_addr = (void*)BALONG_PCIE0_MEM_BASE;
        g_pcie_info[id].ep_in_rc_cpu_virt_addr = (void*)ioremap_nocache((u32)g_pcie_info[id].ep_in_rc_cpu_phys_addr, g_pcie_info[id].buffer_size);
        if (!g_pcie_info[id].ep_in_rc_cpu_virt_addr)
        {
            pcie_trace("fail to ioremap, addr: 0x%08X\n", (u32)g_pcie_info[id].ep_in_rc_cpu_phys_addr);
            return -1;
        }
        pcie_trace("g_pcie_ep_in_rc_cpu_virt_addr: 0x%08X, g_pcie_ep_in_rc_cpu_phys_addr: 0x%08X\n",
            (u32)g_pcie_info[id].ep_in_rc_cpu_virt_addr, (u32)g_pcie_info[id].ep_in_rc_cpu_phys_addr);

        g_pcie_info[id].ep_in_rc_pci_phys_addr = (void*)PCIE_LOOPBACK_PCI_DOMAIN_ADDRESS;

        spin_lock_irqsave(&balong_pcie[id].spinlock, irq_flags);
        dbi_enable(id);

        /* disable Gen3 equalization feature */
        value = readl(balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x190);
        value &= (~(0x1<<16));
        writel(value, balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x190);

        /* enable PIPE loopback */
        value = readl(balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x1B8);
        value |= (0x1<<31);
        writel(value, balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x1B8);

        /* enable loopback */
        value = readl(balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x010);
        value |= (0x1<<2);
        writel(value, balong_pcie[id].virt_rc_cfg_addr + 0x700 + 0x010);

        /* modify bar to non-prefetchmem */
        writel(PCIE_LOOPBACK_PCI_DOMAIN_ADDRESS, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_0);
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_1);
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_2);
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_3);
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_4);
        writel(0x0, balong_pcie[id].virt_rc_cfg_addr + PCI_BASE_ADDRESS_5);

        /* set bar0 size */
        writel(g_pcie_info[id].buffer_size - 1, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_0));
        /* disable other bar */
        writel(0, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_1));
        writel(0, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_2));
        writel(0, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_3));
        writel(0, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_4));
        writel(0, balong_pcie[id].virt_rc_cfg_addr + ((0x1<<12)|PCI_BASE_ADDRESS_5));

        iatu_table[0].lower_addr = balong_pcie[id].res_mem.start;
        iatu_table[0].limit_addr = balong_pcie[id].res_mem.end;
        iatu_table[1].limit_addr = PCIE_LOOPBACK_PCI_DOMAIN_ADDRESS + g_pcie_info[id].buffer_size - 1;
        iatu_table[1].lower_target_addr = (u32)g_pcie_info[id].ep_phys_addr;

        pcie_set_iatu(id, &iatu_table[0], sizeof(iatu_table)/sizeof(iatu_table[0]));

        g_pcie_info[id].ep_in_rc_cpu_sc_phys_addr = (void*)HI_PCIE_SC_BASE_ADDR;
        g_pcie_info[id].ep_in_rc_cpu_sc_virt_addr = (void*)HI_PCIE_SC_BASE_ADDR_VIRT;

        g_pcie_info[id].loopback_enable = 1;

        dbi_disable(id);
        spin_unlock_irqrestore(&balong_pcie[id].spinlock, irq_flags);
    }

    return 0;
}


static u32 g_pcie_stress_test_sleep_ms = 0;
static u32 g_pcie_stress_test_run_count = 0;
static u32 g_pcie_stress_test_task_running = 0;
struct task_struct *g_pcie_stress_test_task = NULL;


#define PCIE_STRESS_TEST_MODE_CPU_READ  (0x00000001)    /* enable CPU read */
#define PCIE_STRESS_TEST_MODE_CPU_WRITE (0x00000002)    /* enable CPU write */
#define PCIE_STRESS_TEST_MODE_DMA_READ  (0x00000004)    /* enable DMA read */
#define PCIE_STRESS_TEST_MODE_DMA_WRITE (0x00000008)    /* enable DMA write */
#define PCIE_STRESS_TEST_MODE_INTERRUPT (0x00000010)    /* enable interrupt */

#define PCIE_STRESS_TEST_MODE_RANDOM    (0x80000000)    /* enable random address and random size */

#define PCIE_STRESS_TEST_MODE_VALID    (PCIE_STRESS_TEST_MODE_CPU_READ | \
                                        PCIE_STRESS_TEST_MODE_CPU_WRITE | \
                                        PCIE_STRESS_TEST_MODE_DMA_READ | \
                                        PCIE_STRESS_TEST_MODE_DMA_WRITE | \
                                        PCIE_STRESS_TEST_MODE_INTERRUPT)

static volatile u32 g_pcie_stree_test_mode = (PCIE_STRESS_TEST_MODE_DMA_READ | \
                                              PCIE_STRESS_TEST_MODE_CPU_WRITE);

int pcie_stress_test_set_mode(u32 mode)
{
    if (!(mode & PCIE_STRESS_TEST_MODE_VALID))
        pcie_trace("[WARNING]No valid bit, valid mode is 0x%08X\n", PCIE_STRESS_TEST_MODE_VALID);

    g_pcie_stree_test_mode = mode;

    return 0;
}

int pcie_stress_test_thread(void *data)
{
    u32 id = 0;
    u32 size = 0;
    void* source = NULL;
    void* destination = NULL;

    /* the buffer of rc and ep is different,
     * we should make them the same in case of the random mode is enable,
     * otherwise the stress test result will be wrong.
     */
    for (id = 0; id < sizeof(g_pcie_info)/sizeof(g_pcie_info[0]); id++)
    {
        if (!balong_pcie[id].enabled)
            continue;

        if (g_pcie_stree_test_mode & PCIE_STRESS_TEST_MODE_RANDOM)
        {
            (void)rc_to_ep_by_cpu(id, g_pcie_info[id].ep_in_rc_cpu_virt_addr, g_pcie_info[id].rc_virt_addr, g_pcie_info[id].buffer_size);
        }
    }

    while (g_pcie_stress_test_task_running)
    {
        for (id = 0; id < sizeof(g_pcie_info)/sizeof(g_pcie_info[0]); id++)
        {
            if (!balong_pcie[id].enabled)
                continue;

            if (g_pcie_stree_test_mode & PCIE_STRESS_TEST_MODE_CPU_READ)
            {
                size = g_pcie_info[id].buffer_size;
                source = g_pcie_info[id].ep_in_rc_cpu_virt_addr;
                destination = g_pcie_info[id].rc_virt_addr;

                if (g_pcie_stree_test_mode & PCIE_STRESS_TEST_MODE_RANDOM)
                {
                    size = get_random(1, g_pcie_info[id].buffer_size - 1);
                    source += (g_pcie_info[id].buffer_size - size);
                    destination += (g_pcie_info[id].buffer_size - size);
                }

                (void)ep_to_rc_by_cpu(id, destination, source, size);
            }

            if (g_pcie_stree_test_mode & PCIE_STRESS_TEST_MODE_CPU_WRITE)
            {
                size = g_pcie_info[id].buffer_size;
                source = g_pcie_info[id].rc_virt_addr;
                destination = g_pcie_info[id].ep_in_rc_cpu_virt_addr;

                if (g_pcie_stree_test_mode & PCIE_STRESS_TEST_MODE_RANDOM)
                {
                    size = get_random(1, g_pcie_info[id].buffer_size - 1);
                    source += (g_pcie_info[id].buffer_size - size);
                    destination += (g_pcie_info[id].buffer_size - size);
                }

                (void)rc_to_ep_by_cpu(id, destination, source, size);
            }

            if (g_pcie_stree_test_mode & PCIE_STRESS_TEST_MODE_DMA_READ)
            {
                size = g_pcie_info[id].buffer_size;
                source = g_pcie_info[id].ep_in_rc_pci_phys_addr;
                destination = g_pcie_info[id].rc_phys_addr;

                if (g_pcie_stree_test_mode & PCIE_STRESS_TEST_MODE_RANDOM)
                {
                    size = get_random(1, g_pcie_info[id].buffer_size - 1);
                    source += (g_pcie_info[id].buffer_size - size);
                    destination += (g_pcie_info[id].buffer_size - size);
                }

                (void)ep_to_rc_by_dma(id, destination, source, size);
            }

            if (g_pcie_stree_test_mode & PCIE_STRESS_TEST_MODE_DMA_WRITE)
            {
                size = g_pcie_info[id].buffer_size;
                source = g_pcie_info[id].rc_phys_addr;
                destination = g_pcie_info[id].ep_in_rc_pci_phys_addr;

                if (g_pcie_stree_test_mode & PCIE_STRESS_TEST_MODE_RANDOM)
                {
                    size = get_random(1, g_pcie_info[id].buffer_size - 1);
                    source += (g_pcie_info[id].buffer_size - size);
                    destination += (g_pcie_info[id].buffer_size - size);
                }

                (void)rc_to_ep_by_dma(id, destination, source, size);
            }

            if (g_pcie_stree_test_mode & PCIE_STRESS_TEST_MODE_INTERRUPT)
            {
                g_pcie_info[id].msi_intx_interrupt_send_count++;
                writel(0x3, g_pcie_info[id].ep_in_rc_cpu_sc_virt_addr + HI_PCIE_SC_ASSERT_INT_OFFSET);
            }
        }

        g_pcie_stress_test_run_count++;

        if (g_pcie_stress_test_sleep_ms)
            msleep(g_pcie_stress_test_sleep_ms);
    }

    return 0;
}

/* priority: 0   -- 99  real time
             100 -- 139 normal */
int pcie_stress_test_start(u32 priority, u32 sleep_ms)
{
    u32 id = 0;
    int policy = 0;
    struct sched_param sched_para;

    if (g_pcie_stress_test_task_running)
        return 0;

    if (priority < 100)
    {
        policy = SCHED_FIFO;
    }
    else
    {
        priority = 0;
        policy = SCHED_NORMAL;
    }

    sched_para.sched_priority = priority;
    g_pcie_stress_test_sleep_ms = sleep_ms;

    for (id = 0; id < sizeof(g_pcie_info)/sizeof(g_pcie_info[0]); id++)
    {
        if (!balong_pcie[id].enabled)
            continue;

        g_pcie_info[id].msi_intx_interrupt_send_count = 0;
        g_pcie_info[id].msi_intx_interrupt_recieve_count = 0;
    }

    g_pcie_stress_test_run_count = 0;
    g_pcie_stress_test_task_running = 1;

    g_pcie_stress_test_task = kthread_run(pcie_stress_test_thread, NULL, "PCIe_Test");
    if (IS_ERR(g_pcie_stress_test_task))
    {
        g_pcie_stress_test_task = NULL;
        printk(KERN_ERR"kthread_run %s fail\n", "PCIe_Test");
        return -1;
    }

    if (sched_setscheduler(g_pcie_stress_test_task, policy, &sched_para))
    {
        printk(KERN_ERR"sched_setscheduler %s fail\n", "PCIe_Test");
        return -1;
    }

    return 0;
}

int pcie_stress_test_stop(void)
{
    g_pcie_stress_test_task_running = 0;

    if (!g_pcie_stress_test_task)
        return kthread_stop(g_pcie_stress_test_task);

    return 0;
}

int pcie_stress_test_count(void)
{
    if (g_pcie_stress_test_task_running)
        printk(KERN_ERR"Running, PCIe stress test run count: %u\n", g_pcie_stress_test_run_count);
    else
        printk(KERN_ERR"Stopped, PCIe stress test run count: %u\n", g_pcie_stress_test_run_count);

    return 0;
}

int pcie_stress_test_result(void)
{
    u32 id = 0;
    int ret = 0;
    int error = 0;

    for (id = 0; id < sizeof(g_pcie_info)/sizeof(g_pcie_info[0]); id++)
    {
        if (!balong_pcie[id].enabled)
            continue;

        pcie_app_req_clk(id, 1);

        ret = memcmp(g_pcie_info[id].rc_virt_addr,
                     g_pcie_info[id].ep_in_rc_cpu_virt_addr,
                     g_pcie_info[id].buffer_size);

        pcie_app_req_clk(id, 0);

        if (ret)
        {
            printk(KERN_ERR"[FAIL]PCIe controller id: %u, stress test run count: %u\n",
                id, g_pcie_stress_test_run_count);
        }
        else
        {
            printk(KERN_ERR"[PASS]PCIe controller id: %u, stress test run count: %u\n",
                id, g_pcie_stress_test_run_count);
        }

        error |= ret;
    }

    return error;
}


