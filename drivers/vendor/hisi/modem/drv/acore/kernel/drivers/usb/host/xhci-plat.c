/*
 * xhci-plat.c - xHCI host controller driver platform Bus Glue.
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com
 * Author: Sebastian Andrzej Siewior <bigeasy@linutronix.de>
 *
 * A lot of code borrowed from the Linux xHCI driver.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "xhci.h"

#ifdef CONFIG_USB_OTG
#include <linux/usb/otg.h>
#endif

static void xhci_plat_quirks(struct device *dev, struct xhci_hcd *xhci)
{
	/*
	 * As of now platform drivers don't provide MSI support so we ensure
	 * here that the generic code does not try to make a pci_dev from our
	 * dev struct in order to setup MSI
	 */
	xhci->quirks |= XHCI_BROKEN_MSI;
}

/* called during probe() after chip reset completes */
static int xhci_plat_setup(struct usb_hcd *hcd)
{
	return xhci_gen_setup(hcd, xhci_plat_quirks);
}

static const struct hc_driver xhci_plat_xhci_driver = {
	.description =		"xhci-hcd",
	.product_desc =		"xHCI Host Controller",
	.hcd_priv_size =	sizeof(struct xhci_hcd *),

	/*
	 * generic hardware linkage
	 */
	.irq =			xhci_irq,
	.flags =		HCD_MEMORY | HCD_USB3 | HCD_SHARED,

	/*
	 * basic lifecycle operations
	 */
	.reset =		xhci_plat_setup,
	.start =		xhci_run,
	.stop =			xhci_stop,
	.shutdown =		xhci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue =		xhci_urb_enqueue,
	.urb_dequeue =		xhci_urb_dequeue,
	.alloc_dev =		xhci_alloc_dev,
	.free_dev =		xhci_free_dev,
	.alloc_streams =	xhci_alloc_streams,
	.free_streams =		xhci_free_streams,
	.add_endpoint =		xhci_add_endpoint,
	.drop_endpoint =	xhci_drop_endpoint,
	.endpoint_reset =	xhci_endpoint_reset,
	.check_bandwidth =	xhci_check_bandwidth,
	.reset_bandwidth =	xhci_reset_bandwidth,
	.address_device =	xhci_address_device,
	.update_hub_device =	xhci_update_hub_device,
	.reset_device =		xhci_discover_or_reset_device,

	/*
	 * scheduling support
	 */
	.get_frame_number =	xhci_get_frame,

	/* Root hub support */
	.hub_control =		xhci_hub_control,
	.hub_status_data =	xhci_hub_status_data,
	.bus_suspend =		xhci_bus_suspend,
	.bus_resume =		xhci_bus_resume,

    .start_port_reset = xhci_start_port_reset,
};

#ifdef CONFIG_USB_OTG
static int otg_irqnum = 0;

int hcd_otg_reset_port(struct usb_hcd *hcd)
{
    printk("%s()\n", __func__);

    if (!hcd)
        return -EINVAL;

    if (hcd->driver->start_port_reset) {
        printk("%s() - Resetting the port\n", __func__);
        hcd->driver->start_port_reset(hcd, 1);
    }
    return 0;
}

int hcd_otg_start_host(struct usb_hcd *hcd)
{
	const struct hc_driver	*driver;
	struct xhci_hcd		*xhci;
    int ret = -EINVAL;

    printk("%s: hcd=%p\n", __func__, hcd);

    if (!hcd) {
        printk("%s() - NULL pointer returned", __func__);
        return ret;
    }

    if (hcd->rh_registered) {
        printk("%s() - Already registered", __func__);
        return 0;
    }

	driver = &xhci_plat_xhci_driver;

    printk("%s() - Host Parimary HCD started\n", __func__);

    ret = usb_add_hcd(hcd, otg_irqnum, IRQF_SHARED);
    if (ret) {
        printk("%s() - Host Parimary HCD start failed, err = %d\n", __func__, ret);
        return ret;
    }

    printk("%s() - Host Shared HCD started\n", __func__);

    xhci = hcd_to_xhci(hcd);

	xhci->shared_hcd = usb_create_shared_hcd(driver, hcd->self.controller,
			dev_name(hcd->self.controller), hcd);
	if (!xhci->shared_hcd) {
		ret = -ENOMEM;
		goto error1;
	}

	*((struct xhci_hcd **) xhci->shared_hcd->hcd_priv) = xhci;

    ret = usb_add_hcd(xhci->shared_hcd, otg_irqnum, IRQF_SHARED);
    if (ret) {
        printk("%s() - Host Shared HCD start failed, err = %d\n", __func__, ret);
        goto error2;
    }
        
    return 0;

error2:
    usb_put_hcd(xhci->shared_hcd);
error1:
    usb_remove_hcd(hcd);
    
    return ret;
}

int hcd_otg_stop_host(struct usb_hcd *hcd)
{
	struct xhci_hcd		*xhci;
    
    printk("%s: hcd=%p\n", __func__, hcd);
    
    if (!hcd) {
        printk("%s() - NULL pointer returned", __func__);
        return -EINVAL;
    }

    xhci = hcd_to_xhci(hcd);

    usb_remove_hcd(xhci->shared_hcd);
    usb_put_hcd(xhci->shared_hcd);
    usb_remove_hcd(hcd);

    return 0;
}

int hcd_otg_host_release(struct usb_hcd *hcd)
{
    int i;
    struct usb_device *udev;
    struct usb_bus *bus;
    struct usb_device *rh;

    if (!hcd)
        return -EINVAL;

    udev = NULL;
    bus = &hcd->self;
    rh = bus->root_hub;

    for (i=0; i<=rh->maxchild; i++) {
        udev = rh->children[i];
        if (!udev)
            continue;

        if (udev->config
            && udev->parent == udev->bus->root_hub) {

            struct usb_otg_descriptor	*desc = NULL;

            if (__usb_get_extra_descriptor (udev->rawdescriptors[0],
                            le16_to_cpu(udev->config[0].desc.wTotalLength),
                            USB_DT_OTG, (void **) &desc) == 0) {
                int err = usb_control_msg(udev,
                              usb_sndctrlpipe(udev, 0),
                              USB_REQ_SET_FEATURE, 0,
                              USB_NTF_HOST_REL,
                              0, NULL, 0, USB_CTRL_SET_TIMEOUT);
                if (err < 0) {
                    dev_info(&udev->dev,
                        "can't release host on device: %d\n",
                         err);
                    return -1;
                }
            }
        }
    }
    return 0;
}

EXPORT_SYMBOL(hcd_otg_start_host);
EXPORT_SYMBOL(hcd_otg_stop_host);
EXPORT_SYMBOL(hcd_otg_reset_port);
EXPORT_SYMBOL(hcd_otg_host_release);
#endif

static int xhci_plat_probe(struct platform_device *pdev)
{
	const struct hc_driver	*driver;
	struct xhci_hcd		*xhci;
	struct resource         *res;
	struct usb_hcd		*hcd;
	int			ret;
	int			irq;

	if (usb_disabled())
		return -ENODEV;

	driver = &xhci_plat_xhci_driver;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return -ENODEV;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;

	hcd = usb_create_hcd(driver, &pdev->dev, dev_name(&pdev->dev));
	if (!hcd)
		return -ENOMEM;

	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len,
				driver->description)) {
		dev_dbg(&pdev->dev, "controller already in use\n");
		ret = -EBUSY;
		goto put_hcd;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		dev_dbg(&pdev->dev, "error mapping memory\n");
		ret = -EFAULT;
		goto release_mem_region;
	}

#ifdef CONFIG_USB_OTG_DWC
    /* The XHCI controller is configured as a Synopsys USB 3.0 OTG XHCI */
    if (driver->flags & HCD_USB3) {
        struct usb_phy *otg = usb_get_transceiver();
        if (otg) {
            otg_set_host(otg->otg, &hcd->self);
            usb_put_transceiver(otg);
        }
        
        otg_irqnum = irq;
        hcd->self.otg_port = 1;

        return 0;
    }
#endif

	ret = usb_add_hcd(hcd, irq, IRQF_SHARED);
	if (ret)
		goto unmap_registers;

	/* USB 2.0 roothub is stored in the platform_device now. */
	hcd = dev_get_drvdata(&pdev->dev);
	xhci = hcd_to_xhci(hcd);
	xhci->shared_hcd = usb_create_shared_hcd(driver, &pdev->dev,
			dev_name(&pdev->dev), hcd);
	if (!xhci->shared_hcd) {
		ret = -ENOMEM;
		goto dealloc_usb2_hcd;
	}

	/*
	 * Set the xHCI pointer before xhci_plat_setup() (aka hcd_driver.reset)
	 * is called by usb_add_hcd().
	 */
	*((struct xhci_hcd **) xhci->shared_hcd->hcd_priv) = xhci;

	ret = usb_add_hcd(xhci->shared_hcd, irq, IRQF_SHARED);
	if (ret)
		goto put_usb3_hcd;

	return 0;

put_usb3_hcd:
	usb_put_hcd(xhci->shared_hcd);

dealloc_usb2_hcd:
	usb_remove_hcd(hcd);

unmap_registers:
	iounmap(hcd->regs);

release_mem_region:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);

put_hcd:
	usb_put_hcd(hcd);

	return ret;
}

static int xhci_plat_remove(struct platform_device *dev)
{
	/* coverity[returned_null] */
	struct usb_hcd	*hcd = platform_get_drvdata(dev);
	struct xhci_hcd	*xhci = hcd_to_xhci(hcd);

#ifdef CONFIG_USB_OTG_DWC
	const struct hc_driver *driver = &xhci_plat_xhci_driver;

    if (driver->flags & HCD_USB3) {
        struct usb_phy *otg = usb_get_transceiver();

        printk("OTG: Removing host on otg=%p\n", otg);
        otg_set_host(otg->otg, NULL);
        usb_put_transceiver(otg);
    }

    if(xhci){
        iounmap(hcd->regs);
        usb_put_hcd(hcd);
        kfree(xhci);
    }

#else
	usb_remove_hcd(xhci->shared_hcd);
	usb_put_hcd(xhci->shared_hcd);

    usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	usb_put_hcd(hcd);
    kfree(xhci);
#endif

	return 0;
}

static struct platform_driver usb_xhci_driver = {
	.probe	= xhci_plat_probe,
	.remove	= xhci_plat_remove,
	.driver	= {
		.name = "xhci-hcd",
	},
};
MODULE_ALIAS("platform:xhci-hcd");

int xhci_register_plat(void)
{
	return platform_driver_register(&usb_xhci_driver);
}

void xhci_unregister_plat(void)
{
	platform_driver_unregister(&usb_xhci_driver);
}
