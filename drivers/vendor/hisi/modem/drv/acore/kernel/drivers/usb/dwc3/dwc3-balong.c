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
/*lint -save -e34 -e537 -e737*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h> 
#include <linux/gfp.h>
#include <linux/delay.h>
#include <linux/clk.h>

#include <asm/mach/irq.h>
#include "bsp_memmap.h"
#include "bsp_usb.h"
#include "bsp_clk.h"
#include "../gadget/usb_vendor.h"
#include "hi_syssc_interface.h"
#include "hi_syscrg_interface.h"


static struct resource dwc3_platform_resource[] =
{
    [0]={
        .start = HI_USB3_REGBASE_ADDR,
        .end = HI_USB3_REGBASE_ADDR + HI_USB3_REG_SIZE - 1,
        .flags = IORESOURCE_MEM
    },
    [1]={
        .start = INT_LVL_USB3,
        .end = INT_LVL_USB3,
        .flags = IORESOURCE_IRQ
    }
};

u64 dwc3_dma_mask = 0xffffffffUL;

static struct platform_device* dwc3_platform_dev_ptr;

#if 0
static struct platform_device dwc3_platform_dev = {
	.name = "dwc3",
    .id = -1,
    .num_resources = ARRAY_SIZE(dwc3_platform_resource),
    .resource = dwc3_platform_resource,
    .dev = {
        .coherent_dma_mask = 0xffffffffUL,
        .dma_mask = &dwc3_dma_mask,
        /* if no release setting kernel/drivers/base/core.c:196 will print WARNNING */
        .release = dwc3_release,
    }
};
#endif

/*
 * bc(battery charger) interfaces
 */
static void bc_set_soft_mode(void)
{
    unsigned reg;

    reg = readl(HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x0);
    reg &= ~0x3;
    reg |= 0x1;/* [false alarm]:Disable fortify false alarm */
    writel(reg, HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x0);
    return;
}/*lint !e550*/

static void bc_set_bypass_mode(void)
{
    unsigned reg;

    reg = readl(HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x0);
    reg &= ~0x03;
    reg |=  0x02;/* [false alarm]:Disable fortify false alarm */
    writel(reg, HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x0);

    return;
}/*lint !e550*/

static unsigned bc_set_test_volt(void)
{
    unsigned reg;
    unsigned record_reg;

    reg = readl(HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x4);
    record_reg = reg;
    reg |= 0xC;
    reg &= ~0x10;/* [false alarm]:Disable fortify false alarm */
    writel(reg, HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x4);

    return record_reg;
}

static void bc_set_record_volt(unsigned record_reg)
{
    writel(record_reg, HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x4);
    return;
}

/*
 * 0:huawei charger , 1:non-huawei charger, -1:invalid charger
 */
static int bc_get_charger_type(void)
{
    unsigned reg;

    reg = readl(HI_BC_CTRL_REGBASE_ADDR_VIRT + 0xc);
    if (0 != (0x2000 & reg)) {
        return USB_CHARGER_TYPE_HUAWEI;
    }

    return USB_CHARGER_TYPE_NOT_HUAWEI;
}



/*
 * note: the function will sleep in long time !!!
 * 0:huawei charger , 1:non-huawei charger, -1:invalid charger
 */
int bc_charger_type_identify(void)
{
    unsigned record_reg;
    int type = USB_CHARGER_TYPE_INVALID;

    /* setup to soft mode */
    bc_set_soft_mode();

    /*
    VBUS ------
    D+     ----
    D-     ----
    RND  ------
    when usb insert into host, VBUS and RND connect first, then D+/D- signal on
    we need msleep to wait the signal ok.
    */
    msleep(2000);

    /*
    bit[2]=1
    bit[3]=1
    bit[4]=0
    supply D+ 0.6V volt(normal volt is 0.8V)for charger identify
    */
    record_reg = bc_set_test_volt();

    /* wait 50ms for D+ signal go to D- */
    mdelay(50);/*lint !e62*/

    /*
    get D- signal, and detect the charger type
    bit[13]==0 : SDP (PC)
    bit[13]==1 : DCP (CHARGER)
    */
    type = bc_get_charger_type();

    /* restore the default setting */
    bc_set_record_volt(record_reg);

    mdelay(20);/*lint !e62*/

    /* restore to bypass mode */
    bc_set_bypass_mode();

    mdelay(2);/*lint !e62*/

    return type;
}

/* Attention: make sure the clock name is consistent with the name defined in clock_balong_hi6930.c */
#define USB_OTG_CLK_NAME "usbotg_clk"
#define USB_BC_CLK_NAME "usb_bc_clk"
static struct clk *usb3_otg_clk, *usb3_bc_clk;
static void usb3_clk_get(void)
{
    usb3_otg_clk = clk_get(NULL, USB_OTG_CLK_NAME);
	if (IS_ERR(usb3_otg_clk)) {
		printk(KERN_ERR "dwc usb3.0: failed to get the usb otg clk %d\n",(int)PTR_ERR(usb3_otg_clk));
		usb3_otg_clk = NULL;
	}
	usb3_bc_clk = clk_get(NULL, USB_BC_CLK_NAME);
	if (IS_ERR(usb3_bc_clk)) {
		printk(KERN_ERR "dwc usb3.0: failed to get the usb otg clk %d\n",(int)PTR_ERR(usb3_bc_clk));
		usb3_bc_clk = NULL;
	}
}
static void usb3_clk_put(void)
{
    if(usb3_otg_clk)
       clk_put(usb3_otg_clk);

    if (usb3_bc_clk)
        clk_put(usb3_bc_clk);
}

static void usb3_otg_clk_enable(void)
{
    int ret;
    if (!usb3_otg_clk) {
        printk(KERN_ERR "dwc usb3.0: invalid otg clk\n");
        return;
    }
    ret = clk_enable(usb3_otg_clk);
    if(ret) {
        printk(KERN_ERR "dwc usb3.0: failed to enable the otg clk(err code:%d)\n",ret);
    }
}
static void usb3_otg_clk_disable(void)
{
    if (!usb3_otg_clk) {
        printk(KERN_ERR "dwc usb3.0: invalid otg clk\n");
        return;
    }
    clk_disable(usb3_otg_clk);
}
static void usb3_bc_clk_enable(void)
{
    int ret;
    if (!usb3_bc_clk) {
        printk(KERN_ERR "dwc usb3.0: invalid bc clk\n");
        return;
    }
    ret = clk_enable(usb3_bc_clk);
    if(ret) {
        printk(KERN_ERR "dwc usb3.0: failed to enable the bc clk(err code:%d)\n",ret);
    }
}
void usb3_bc_clk_disable(void)
{
    if (!usb3_bc_clk) {
        printk(KERN_ERR "dwc usb3.0: invalid bc clk\n");
        return;
    }
    clk_disable(usb3_bc_clk);
}

void usb3_sysctrl_init(void)
{
    usb3_clk_get();
    /* enable the usb-bc clk at first */
    usb3_bc_clk_enable();

    /* power up the usb2.0 and usb3.0 phy */
    hi_syssc_usb_powerdown_hsp(0);
    hi_syssc_usb_powerdown_ssp(0);

    /* ref_ssp_en */
    hi_syssc_usb_ref_ssp(1);

#ifdef CONFIG_USB_DWC3_VBUS_DISCONNECT
    /* vbusvldext and vbusvldextsel */
    hi_syssc_usb_vbusvldext(1);

    /* override_en and override_value*/
    hi_syssc_usb_override(1);
#else
    /* vbusvldext and vbusvldextsel */
    hi_syssc_usb_vbusvldext(0);

    /* override_en and override_value*/
    hi_syssc_usb_override(0);
#endif

#ifdef CONFIG_USB_OTG_USBID_BYGPIO
    hi_syssc_usb_iddig_en(1);
#endif

    hi_syssc_usb_txpreempamptune(3);

    hi_syssc_usb_phy3_init();

    /* set bc_mode to bypass */
    bc_set_bypass_mode();

    /* enable the otg clock */
    usb3_otg_clk_enable();

    /* release controller and PHY */
    hi_syscrg_usb_release();

    mdelay(10);/*lint !e62*/
}

void usb3_sysctrl_exit(void)
{
    /* enable the usb bc clock to change the bc mode */
    usb3_bc_clk_enable();

    /* reset controller and phy */
    hi_syscrg_usb_reset();

    /* disable the usb otg clock */
    usb3_otg_clk_disable();

    /* reset vbusvldext and override_en */
    hi_syssc_usb_override(0);
    hi_syssc_usb_vbusvldext(0);

    /* reset ref_ssp_en */
    hi_syssc_usb_ref_ssp(0);

    /* reset bc_mode to soft modes */
    bc_set_soft_mode();

    /* power down the usb2.0 and usb3.0 phy */
    hi_syssc_usb_powerdown_hsp(1);
    hi_syssc_usb_powerdown_ssp(1);

    mdelay(2);/*lint !e62*/
    usb3_bc_clk_disable();

	/* put the usb-otg & usb-bc clock */
    usb3_clk_put();
}


int usb_dwc3_platform_dev_init(void)
{
    int ret;

#ifndef CONFIG_USB_OTG_DWC
    /* init otg controller and phy */
    usb3_sysctrl_init();

    /* if the version support charger, identify the charger type
     * we must detect charger type before usb core init
     */
    if (bsp_usb_is_support_charger()) {
        bsp_usb_set_charger_type(bc_charger_type_identify());
    }

    /* clk off the bc controller, when we never use it */
    usb3_bc_clk_disable();

#endif

    dwc3_platform_dev_ptr = platform_device_alloc("dwc3", -1);
    if (!dwc3_platform_dev_ptr) {
        printk("%s:platform_device_alloc fail.\n",__FUNCTION__);
        return -ENOMEM;
    }

    ret = platform_device_add_resources(dwc3_platform_dev_ptr, dwc3_platform_resource, 2);
    if (ret) {
        printk("%s:platform_device_add_resources fail: %d.\n",__FUNCTION__, ret);
        goto fail_put_dev;
    }

    dwc3_platform_dev_ptr->dev.coherent_dma_mask = 0xffffffffUL;
    dwc3_platform_dev_ptr->dev.dma_mask = &dwc3_dma_mask;
	ret = platform_device_add(dwc3_platform_dev_ptr);
    if (ret) {
        printk("%s:platform_device_register fail: %d.\n",__FUNCTION__, ret);
        goto fail_put_dev;
    }
    return 0;

fail_put_dev:
    platform_device_put(dwc3_platform_dev_ptr);
    return ret;
}


void usb_dwc3_plaform_dev_exit(void)
{
    platform_device_unregister(dwc3_platform_dev_ptr);

#ifndef CONFIG_USB_OTG_DWC
    /* reset the otg controller and phy */
    usb3_sysctrl_exit();
#endif
}


MODULE_AUTHOR("BALONG USBNET GROUP");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("BALONG USB3 DRD Controller Driver");
/*lint -restore*/
