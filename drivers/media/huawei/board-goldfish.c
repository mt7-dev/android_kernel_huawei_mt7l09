/* arch/arm/mach-goldfish/board-goldfish.c
**
** Copyright (C) 2007 Google, Inc.
**
** This software is licensed under the terms of the GNU General Public
** License version 2, as published by the Free Software Foundation, and
** may be copied, distributed, and modified under those terms.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
*/

#include <linux/kernel.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ion.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/input.h>

//#include <mach/hardware.h>
//#include <asm/io.h>
//#include <asm/mach-types.h>
//#include <asm/mach/arch.h>
//#include <asm/mach/flash.h>
//#include <asm/mach/map.h>
//#include <asm/mach/time.h>

//int GOLDFISH_READY = 0;
#if 0
static struct resource goldfish_pdev_bus_resources[] = {
	{
		.start  = GOLDFISH_PDEV_BUS_BASE,
		.end    = GOLDFISH_PDEV_BUS_BASE + GOLDFISH_PDEV_BUS_END - 1,
		.flags  = IORESOURCE_IO,
	},
	{
		.start	= IRQ_PDEV_BUS,
		.end	= IRQ_PDEV_BUS,
		.flags	= IORESOURCE_IRQ,
	}
};


struct platform_device goldfish_pdev_bus_device = {
	.name = "goldfish_pdev_bus",
	.id = -1,
	.num_resources = ARRAY_SIZE(goldfish_pdev_bus_resources),
	.resource = goldfish_pdev_bus_resources
};
#endif
static struct platform_device hwcfghub_device = {
	.name	= "huawei,camcfgdev",
};
/*
static struct platform_device fake_sensor_device1 = {
	.name	= "huawei,fake-fore",
};

static struct platform_device fake_sensor_device2 = {
	.name	= "huawei,fake-rear",
};

static struct platform_device imx135_device = {
	.name	= "huawei,imx135",
};

static struct platform_device imx214_device = {
	.name	= "huawei,imx214",
};

static struct platform_device ov5648_device = {
	.name	= "huawei,ov5648",
};

static struct platform_device altek6045_device = {
	.name	= "huawei,altek6045",
};
*/
static struct platform_device ovisp23_device = {
	.name	= "huawei,ovisp23",
};
/*
static struct platform_device fake_isp_device = {
	.name	= "huawei,fake-isp",
};

struct ion_platform_heap _ion_heap_defs = 
{
	.type = ION_HEAP_TYPE_SYSTEM, 
	.id = ION_HEAP_TYPE_SYSTEM,
	.name = "alan,system,ion",
	.base = 0,
	.size = 0,
};

static struct ion_platform_data ion_heap_defs = {
	.nr = 1,
	.heaps = &_ion_heap_defs,    
};

static struct platform_device ion_device = {
	.name	= "huawei,ion",
    .dev = 
    {
        .platform_data = &ion_heap_defs, 
    }, 
};
*/
static int  __init goldfish_init(void)
{
	printk("%s\n",__func__);
    platform_device_register(&hwcfghub_device);
    //platform_device_register(&fake_sensor_device1);
    //platform_device_register(&fake_sensor_device2);
    //platform_device_register(&imx135_device);
    //platform_device_register(&imx214_device);
    //platform_device_register(&ov5648_device);
    //platform_device_register(&altek6045_device);
    platform_device_register(&ovisp23_device);
    //platform_device_register(&fake_isp_device);
    //platform_device_register(&ion_device);
    //platform_device_register(&goldfish_pdev_bus_device);
    return 0;
}
#if 0
void goldfish_mask_irq(struct irq_data *d)
{
	//writel(d->irq, IO_ADDRESS(GOLDFISH_INTERRUPT_BASE) + GOLDFISH_INTERRUPT_DISABLE);
}

void goldfish_unmask_irq(struct irq_data *d)
{
	//writel(d->irq, IO_ADDRESS(GOLDFISH_INTERRUPT_BASE) + GOLDFISH_INTERRUPT_ENABLE);
}

static struct irq_chip goldfish_irq_chip = {
	.name		= "goldfish",
	.irq_mask	= goldfish_mask_irq,
	.irq_mask_ack	= goldfish_mask_irq,
	.irq_unmask	= goldfish_unmask_irq,
};

void goldfish_init_irq(void)
{

	unsigned int i;
	uint32_t int_base = IO_ADDRESS(GOLDFISH_INTERRUPT_BASE);

	/*
	 * Disable all interrupt sources
	 */
	writel(1, int_base + GOLDFISH_INTERRUPT_DISABLE_ALL);

	for (i = 0; i < NR_IRQS; i++) {
		irq_set_chip(i, &goldfish_irq_chip);
		irq_set_handler(i, handle_level_irq);
		set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
	}
	
}

static struct map_desc goldfish_io_desc[] __initdata = {
	{
		.virtual	= IO_BASE,
		.pfn		= __phys_to_pfn(IO_START),
		.length		= IO_SIZE,
		.type		= MT_DEVICE
	},
};

static void __init goldfish_map_io(void)
{

	iotable_init(goldfish_io_desc, ARRAY_SIZE(goldfish_io_desc));
	// alloc memory for DMA, used for fb
	init_consistent_dma_size(SZ_4M);
    GOLDFISH_READY = 1;
	
}
#endif
module_init(goldfish_init);
#if 0
//extern struct sys_timer goldfish_timer;

MACHINE_START(GOLDFISH, "Goldfish")
	.init_machine	= goldfish_init,
MACHINE_END
#endif