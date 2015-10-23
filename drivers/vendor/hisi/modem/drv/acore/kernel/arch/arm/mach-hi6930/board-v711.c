/*
 *  linux/arch/arm/mach-balong/core.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/amba/bus.h>
#include <linux/io.h>
#include <linux/gfp.h>

#include <linux/spi/spi_balong.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include<linux/i2c.h>

#include <asm/system.h>
#include <asm/irq.h>
#include <asm/mach-types.h>
#include <asm/pgtable.h>
#include <asm/hardware/gic.h>
#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#include <asm/hardware/gic.h>

#include <mach/hardware.h>
#include <mach/board.h>
#include <mach/irqs.h>
#include <mach/timer.h>
#include <mach/balongv7r2_iomap.h>
#include <bsp_clk.h>
#include <product_config.h>
#include <bsp_sram.h>
#include "bsp_wdt.h"

/* used by entry-macro.S */

/*for wdt restart*/
typedef void (*wdt_restart_func)(void);
wdt_restart_func wdt_restart_func_bc = NULL;

struct platform_i2c{
	int bus_num;
	unsigned int flags;
	unsigned int slave_addr;
	unsigned long frequency;
	unsigned int sda_delay;
};

/*
 * Balong devices
 */

static struct amba_device uart0_device = {
	.dev =
	{
		.coherent_dma_mask = ~0,
		.init_name = "uart0",
		.platform_data = NULL,
	},
	.res =
	{
		.start	= HI_UART1_REGBASE_ADDR,
		.end	= (HI_UART1_REGBASE_ADDR) + HI_UART1_REG_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	.dma_mask	= ~0,
	.irq		= { INT_LVL_UART1, 0 },
    .periphid = 0x000c21c0,
};

static struct amba_device *amba_devs[] __initdata = {
	&uart0_device,
};


	static struct resource balong_i2c0_resource[]={
		[0]={
		.start=HI_I2C0_REGBASE_ADDR,
		.end = HI_I2C0_REGBASE_ADDR+HI_I2C0_REG_SIZE-32,
		.flags=IORESOURCE_MEM,
			},
		[1]={
		.start=INT_LVL_I2C0,/*ÖÐ¶ÏºÅ*/
		.end=INT_LVL_I2C0,
		.flags=IORESOURCE_IRQ,
		}
	};

	static struct resource balong_i2c1_resource[]={
		[0]={
		.start=HI_I2C1_REGBASE_ADDR,
		.end =HI_I2C1_REGBASE_ADDR+HI_I2C1_REG_SIZE,
		.flags=IORESOURCE_MEM,
		},
		[1]={
		.start=INT_LVL_I2C1,
		.end=INT_LVL_I2C1,
		.flags=IORESOURCE_IRQ,
		}
	};
	struct platform_device balong_device_i2c0={
		.name="balong_i2c_v7r2_0",
		.id = 0,
		.num_resources=ARRAY_SIZE(balong_i2c0_resource),
		.resource=balong_i2c0_resource,
	};

	struct platform_device balong_device_i2c1={
		.name="balong_i2c_v7r2_1",
		.id = 1,
		.num_resources=ARRAY_SIZE(balong_i2c1_resource),
		.resource=balong_i2c1_resource,
	};

static struct platform_i2c default_i2c0_data __initdata={
	.bus_num = 0,
	.flags=0,
	.slave_addr=0x10,
	.frequency=100*1000,
	.sda_delay=100
};

static struct platform_i2c default_i2c1_data __initdata={
	.bus_num = 1,
	.flags=0,
	.slave_addr=0x10,
	.frequency=100*1000,
	.sda_delay=100
};

static void __init gic_init_irq(void)
{
	/* board GIC, primary */
	gic_init(0, GIC_PPI_START, (void __iomem *)GIC_DIST_VIRT_BASE, (void __iomem *)GIC_CPU_VIRT_BASE);
}

static void __init balongv7r2_init(void)
{
	int i;
	int ret;

	uart0_device.res.start = ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[0].base_addr;
	uart0_device.res.end = ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[0].base_addr + HI_UART0_REG_SIZE - 1;
	uart0_device.irq[0] = ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[0].interrupt_num;

	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		struct amba_device *d = amba_devs[i];
		amba_device_register(d, &iomem_resource);
	}

	balong_device_i2c0.dev.platform_data=&default_i2c0_data;
	balong_device_i2c1.dev.platform_data=&default_i2c1_data;
	ret = platform_device_register(&balong_device_i2c0);
	if(ret){
		printk(KERN_ERR "i2c0 device register failed! \n");
	}

	ret = platform_device_register(&balong_device_i2c1);
	if(ret){
		printk(KERN_ERR "i2c0 device register failed! \n");
	}
}

static void __init balongv7r2_reserve(void)
{
}

static void __init balongv7r2_init_early(void)
{
#ifdef CONFIG_BALONG_CLK
	hi6930_clock_init ();
#endif
}

/* restart.c  */
void balongv7r2_restart(char mode, const char *cmd)
{
	if (wdt_restart_func_bc)
	{
		wdt_restart_func_bc();
	}
}

extern void __init l2x0_init(void __iomem *base, u32 aux_val, u32 aux_mask);

extern int map_io_finished;
static void __init balongv7r2_map_io(void)
{
	balong_map_io();
    map_io_finished = 1;//indicate to init;
#ifdef CONFIG_CACHE_L2X0

    *(unsigned int*)(HI_APPA9_L2_REGBASE_ADDR_VIRT+0xf80) = 0x3;
	l2x0_init((void __iomem *)HI_APPA9_L2_REGBASE_ADDR_VIRT, 0x42040001, 0x000fffe);
#endif
}

MACHINE_START(HI6930, "Hisilicon hi6930")
	.atag_offset  = 0x100,
	.map_io		  = balongv7r2_map_io,
	.reserve      = balongv7r2_reserve,
	.init_irq	  = gic_init_irq,
	.timer		  = &balongv7r2_timer,
	.init_early	  = balongv7r2_init_early,
	.init_machine = balongv7r2_init,
	.handle_irq   = gic_handle_irq,
#if !defined(CONFIG_BALONG_ONOFF)
	.restart      = balongv7r2_restart,
#endif
MACHINE_END

