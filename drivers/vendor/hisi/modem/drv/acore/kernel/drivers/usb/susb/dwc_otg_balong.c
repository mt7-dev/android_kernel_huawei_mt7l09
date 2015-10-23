/*
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
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gfp.h>
#include <linux/delay.h>
#include <mach/lm.h>

#include <asm/mach/irq.h>
#include "bsp_memmap.h"

#include <linux/gpio.h>

#include "bsp_pmu.h"

extern int hi6451_32k_clk_enable(int clk_id);

#define DWC_SYNOP_CORE_HSIC 2
static struct lm_device dwc_usb_hsic={
	.dev={
		.init_name = "synopsys_dwc_hsic",
	},

	.resource.start = HI_HSIC_REGBASE_ADDR,
	.resource.end = HI_HSIC_REGBASE_ADDR + HI_HSIC_REG_SIZE -1,
	.resource.flags = IORESOURCE_MEM,
	.irq = INT_LVL_HSIC,
	.id = DWC_SYNOP_CORE_HSIC,
};
void dwc_wifi_unreset(void)
{
    printk("dwc_wifi_unrest: ===enter===\n");

	if (FEATURE_BCM_VERSION == 43241) {
		gpio_request(GPIO_5_14,"hisi-wifi");
		gpio_direction_output(GPIO_5_14,0);   /* BCM43241_RST_N */
		gpio_direction_output(GPIO_5_14,1);   /* BCM43241_RST_N */
		bsp_pmu_32k_clk_enable(PMU_32K_CLK_B);
	} else if (FEATURE_BCM_VERSION == 43239) {
	   	gpio_request(GPIO_1_19,"hisi-wifi");
   		gpio_direction_output(GPIO_1_19,1);   /* BCM43239_RST_N */
	} else {
		printk("invalid bcm version!\n");
	}
}

void dwc_hsic_unreset(void)
{
    u32 value;

    printk("dwc_hsic_unrest: ====enter===\r\n");

    /* Adjust HSIC signals */
    value = readl(IO_ADDRESS(HI_SYSCTRL_BASE_ADDR)+0x488);
	value |= (0xFF << 7);
    writel(value, IO_ADDRESS(HI_SYSCTRL_BASE_ADDR)+0x488);

    /* Force Host Mode */
	value = readl(IO_ADDRESS(HI_SYSCTRL_BASE_ADDR) + 0x488);
    value &= ~((u32)1<< 31);
    writel(value, IO_ADDRESS(HI_SYSCTRL_BASE_ADDR) + 0x488);

    /* dppulldown,dmpulldown clear */
	value = readl(IO_ADDRESS(HI_SYSCTRL_BASE_ADDR)+0x488);
	value |= (u32)1 << 22;
    writel(value, IO_ADDRESS(HI_SYSCTRL_BASE_ADDR)+0x488);

	value = readl(IO_ADDRESS(HI_SYSCTRL_BASE_ADDR)+0x488);
	value |= (u32)1 << 21;
    writel(value, IO_ADDRESS(HI_SYSCTRL_BASE_ADDR)+0x488);

    /* Activate HSIC Controller */
    value = readl(IO_ADDRESS(HI_SYSCTRL_BASE_ADDR) + 0x3C);
    value &= ~((u32)1<< 26);
    writel(value, IO_ADDRESS(HI_SYSCTRL_BASE_ADDR) + 0x3C);

    /* Release HSIC PHY Por */
    value = readl(IO_ADDRESS(HI_SYSCTRL_BASE_ADDR) + 0x3C);
    value &= ~((u32)1<< 27);
    writel(value, IO_ADDRESS(HI_SYSCTRL_BASE_ADDR) + 0x3C);

    udelay(100);
}

static int __init dwc_hsic_init(void)
{
	dwc_wifi_unreset();
	dwc_hsic_unreset();

    return lm_device_register(&dwc_usb_hsic);
}
module_init(dwc_hsic_init);

static void __init dwc_hsic_exit(void)
{
    return lm_device_unregister(&dwc_usb_hsic);
}
module_exit(dwc_hsic_exit);


MODULE_AUTHOR("BALONG USBNET GROUP");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("BALONG USB HSIC Controller Driver");
