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
#include <linux/clkdev.h>

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

/* used by entry-macro.S */


/*
 * Balong devices
 */

static struct amba_device uart0_device = {
	.dev = 
	{
		.coherent_dma_mask = ~0,	
		.init_name = "dev:uart0",
		.platform_data = NULL,	
	},
	.res = 
	{
		.start	= BALONG_UART0_PHY_BASE,
		.end	= (BALONG_UART0_PHY_BASE) + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},	
	.dma_mask	= ~0,
	.irq		= { BALONG_UART0_IRQ, NO_IRQ },
    .periphid = 0x000c21c0,
};


static struct amba_device *amba_devs[] __initdata = {
	&uart0_device,
};

static void __init gic_init_irq(void)
{
	/* board GIC, primary */
	gic_init(0, GIC_PPI_START, (void __iomem *)GIC_DIST_VIRT_BASE, (void __iomem *)GIC_CPU_VIRT_BASE);
}

static void __init balongv7r2_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		struct amba_device *d = amba_devs[i];
		amba_device_register(d, &iomem_resource);
	}	
}

static void __init balongv7r2_reserve(void)
{
}

static void __init balongv7r2_init_early(void)
{
}

/* restart.c  */
void balongv7r2_restart(char mode, const char *cmd)
{
}

extern int map_io_finished;
static void __init balongv7r2_map_io(void)
{
	balong_map_io();
    map_io_finished = 1;//indicate to init;
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
	.restart      = balongv7r2_restart,
MACHINE_END

