/*
 * File:	portdrv_pci.c
 * Purpose:	PCI Express Port Bus Driver
 *
 * Copyright (C) 2004 Intel
 * Copyright (C) Tom Long Nguyen (tom.l.nguyen@intel.com)
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/pm.h>
#include <linux/init.h>
#include <linux/pcieport_if.h>
#include <linux/aer.h>
#include <linux/dmi.h>
#include <linux/pci-aspm.h>

#include "portdrv.h"
#include "aer/aerdrv.h"

/*
 * Version Information
 */
#define DRIVER_VERSION "v1.0"
#define DRIVER_AUTHOR "tom.l.nguyen@intel.com"
#define DRIVER_DESC "PCIe Port Bus Driver"
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

/* If this switch is set, PCIe port native services should not be enabled. */
bool pcie_ports_disabled;

/*
 * If this switch is set, ACPI _OSC will be used to determine whether or not to
 * enable PCIe port native services.
 */
bool pcie_ports_auto = true;

static int __init pcie_port_setup(char *str)
{
	if (!strncmp(str, "compat", 6)) {
		pcie_ports_disabled = true;
	} else if (!strncmp(str, "native", 6)) {
		pcie_ports_disabled = false;
		pcie_ports_auto = false;
	} else if (!strncmp(str, "auto", 4)) {
		pcie_ports_disabled = false;
		pcie_ports_auto = true;
	}

	return 1;
}
__setup("pcie_ports=", pcie_port_setup);

/* global data */

/**
 * pcie_clear_root_pme_status - Clear root port PME interrupt status.
 * @dev: PCIe root port or event collector.
 */
void pcie_clear_root_pme_status(struct pci_dev *dev)
{
	int rtsta_pos;
	u32 rtsta;

	rtsta_pos = pci_pcie_cap(dev) + PCI_EXP_RTSTA;

	pci_read_config_dword(dev, rtsta_pos, &rtsta);
	rtsta |= PCI_EXP_RTSTA_PME;
	pci_write_config_dword(dev, rtsta_pos, rtsta);
}

static int pcie_portdrv_restore_config(struct pci_dev *dev)
{
	int retval;

	retval = pci_enable_device(dev);
	if (retval)
		return retval;
	pci_set_master(dev);
	return 0;
}

#ifdef CONFIG_PM
static int pcie_port_resume_noirq(struct device *dev)
{
	struct pci_dev *pdev = to_pci_dev(dev);

	/*
	 * Some BIOSes forget to clear Root PME Status bits after system wakeup
	 * which breaks ACPI-based runtime wakeup on PCI Express, so clear those
	 * bits now just in case (shouldn't hurt).
	 */
	if(pdev->pcie_type == PCI_EXP_TYPE_ROOT_PORT)
		pcie_clear_root_pme_status(pdev);
	return 0;
}

static const struct dev_pm_ops pcie_portdrv_pm_ops = {
	.suspend	= pcie_port_device_suspend,
	.resume		= pcie_port_device_resume,
	.freeze		= pcie_port_device_suspend,
	.thaw		= pcie_port_device_resume,
	.poweroff	= pcie_port_device_suspend,
	.restore	= pcie_port_device_resume,
	.resume_noirq	= pcie_port_resume_noirq,
};

#define PCIE_PORTDRV_PM_OPS	(&pcie_portdrv_pm_ops)

#else /* !PM */

#define PCIE_PORTDRV_PM_OPS	NULL
#endif /* !PM */

/*
 * pcie_portdrv_probe - Probe PCI-Express port devices
 * @dev: PCI-Express port device being probed
 *
 * If detected invokes the pcie_port_device_register() method for
 * this port device.
 *
 */
static int __devinit pcie_portdrv_probe(struct pci_dev *dev,
					const struct pci_device_id *id)
{
	int status;

	if (!pci_is_pcie(dev) ||
	    ((dev->pcie_type != PCI_EXP_TYPE_ROOT_PORT) &&
	     (dev->pcie_type != PCI_EXP_TYPE_UPSTREAM) &&
	     (dev->pcie_type != PCI_EXP_TYPE_DOWNSTREAM)))
		return -ENODEV;

	if (!dev->irq && dev->pin) {
		dev_warn(&dev->dev, "device [%04x:%04x] has invalid IRQ; "
			 "check vendor BIOS\n", dev->vendor, dev->device);
	}
	status = pcie_port_device_register(dev);
	if (status)
		return status;

	pci_save_state(dev);
	return 0;
}

static void pcie_portdrv_remove(struct pci_dev *dev)
{
	pcie_port_device_remove(dev);
	pci_disable_device(dev);
}

static int error_detected_iter(struct device *device, void *data)
{
	struct pcie_device *pcie_device;
	struct pcie_port_service_driver *driver;
	struct aer_broadcast_data *result_data;
	pci_ers_result_t status;

	result_data = (struct aer_broadcast_data *) data;

	if (device->bus == &pcie_port_bus_type && device->driver) {
		driver = to_service_driver(device->driver);
		if (!driver ||
			!driver->err_handler ||
			!driver->err_handler->error_detected)
			return 0;

		pcie_device = to_pcie_device(device);

		/* Forward error detected message to service drivers */
		status = driver->err_handler->error_detected(
			pcie_device->port,
			result_data->state);
		result_data->result =
			merge_result(result_data->result, status);
	}

	return 0;
}

static pci_ers_result_t pcie_portdrv_error_detected(struct pci_dev *dev,
					enum pci_channel_state error)
{
	struct aer_broadcast_data data = {error, PCI_ERS_RESULT_CAN_RECOVER};
	int ret;

	/* can not fail */
	ret = device_for_each_child(&dev->dev, &data, error_detected_iter);

	return data.result;
}

static int mmio_enabled_iter(struct device *device, void *data)
{
	struct pcie_device *pcie_device;
	struct pcie_port_service_driver *driver;
	pci_ers_result_t status, *result;

	result = (pci_ers_result_t *) data;

	if (device->bus == &pcie_port_bus_type && device->driver) {
		driver = to_service_driver(device->driver);
		if (driver &&
			driver->err_handler &&
			driver->err_handler->mmio_enabled) {
			pcie_device = to_pcie_device(device);

			/* Forward error message to service drivers */
			status = driver->err_handler->mmio_enabled(
					pcie_device->port);
			*result = merge_result(*result, status);
		}
	}

	return 0;
}

static pci_ers_result_t pcie_portdrv_mmio_enabled(struct pci_dev *dev)
{
	pci_ers_result_t status = PCI_ERS_RESULT_RECOVERED;
	int retval;

	/* get true return value from &status */
	retval = device_for_each_child(&dev->dev, &status, mmio_enabled_iter);
	return status;
}

static int slot_reset_iter(struct device *device, void *data)
{
	struct pcie_device *pcie_device;
	struct pcie_port_service_driver *driver;
	pci_ers_result_t status, *result;

	result = (pci_ers_result_t *) data;

	if (device->bus == &pcie_port_bus_type && device->driver) {
		driver = to_service_driver(device->driver);
		if (driver &&
			driver->err_handler &&
			driver->err_handler->slot_reset) {
			pcie_device = to_pcie_device(device);

			/* Forward error message to service drivers */
			status = driver->err_handler->slot_reset(
					pcie_device->port);
			*result = merge_result(*result, status);
		}
	}

	return 0;
}

static pci_ers_result_t pcie_portdrv_slot_reset(struct pci_dev *dev)
{
	pci_ers_result_t status = PCI_ERS_RESULT_RECOVERED;
	int retval;

	/* If fatal, restore cfg space for possible link reset at upstream */
	if (dev->error_state == pci_channel_io_frozen) {
		dev->state_saved = true;
		pci_restore_state(dev);
		pcie_portdrv_restore_config(dev);
		pci_enable_pcie_error_reporting(dev);
	}

	/* get true return value from &status */
	retval = device_for_each_child(&dev->dev, &status, slot_reset_iter);

	return status;
}

static int resume_iter(struct device *device, void *data)
{
	struct pcie_device *pcie_device;
	struct pcie_port_service_driver *driver;

	if (device->bus == &pcie_port_bus_type && device->driver) {
		driver = to_service_driver(device->driver);
		if (driver &&
			driver->err_handler &&
			driver->err_handler->resume) {
			pcie_device = to_pcie_device(device);

			/* Forward error message to service drivers */
			driver->err_handler->resume(pcie_device->port);
		}
	}

	return 0;
}

static void pcie_portdrv_err_resume(struct pci_dev *dev)
{
	int retval;
	/* nothing to do with error value, if it ever happens */
	retval = device_for_each_child(&dev->dev, NULL, resume_iter);
}

/*
 * LINUX Device Driver Model
 */
static const struct pci_device_id port_pci_ids[] = { {
	/* handle any PCI-Express port */
	PCI_DEVICE_CLASS(((PCI_CLASS_BRIDGE_PCI << 8) | 0x00), ~0),
	}, { /* end: all zeroes */ }
};
MODULE_DEVICE_TABLE(pci, port_pci_ids);

static struct pci_error_handlers pcie_portdrv_err_handler = {
		.error_detected = pcie_portdrv_error_detected,
		.mmio_enabled = pcie_portdrv_mmio_enabled,
		.slot_reset = pcie_portdrv_slot_reset,
		.resume = pcie_portdrv_err_resume,
};

static struct pci_driver pcie_portdriver = {
	.name		= "pcieport",
	.id_table	= &port_pci_ids[0],

	.probe		= pcie_portdrv_probe,
	.remove		= pcie_portdrv_remove,

	.err_handler 	= &pcie_portdrv_err_handler,

	.driver.pm 	= PCIE_PORTDRV_PM_OPS,
};

static int __init dmi_pcie_pme_disable_msi(const struct dmi_system_id *d)
{
	pr_notice("%s detected: will not use MSI for PCIe PME signaling\n",
			d->ident);
	pcie_pme_disable_msi();
	return 0;
}

static struct dmi_system_id __initdata pcie_portdrv_dmi_table[] = {
	/*
	 * Boxes that should not use MSI for PCIe PME signaling.
	 */
	{
	 .callback = dmi_pcie_pme_disable_msi,
	 .ident = "MSI Wind U-100",
	 .matches = {
		     DMI_MATCH(DMI_SYS_VENDOR,
		     		"MICRO-STAR INTERNATIONAL CO., LTD"),
		     DMI_MATCH(DMI_PRODUCT_NAME, "U-100"),
		     },
	 },
	 {}
};

static int __init pcie_portdrv_init(void)
{
	int retval;

	if (pcie_ports_disabled)
		return pci_register_driver(&pcie_portdriver);

	dmi_check_system(pcie_portdrv_dmi_table);

	retval = pcie_port_bus_register();
	if (retval) {
		printk(KERN_WARNING "PCIE: bus_register error: %d\n", retval);
		goto out;
	}
	retval = pci_register_driver(&pcie_portdriver);
	if (retval)
		pcie_port_bus_unregister();
 out:
	return retval;
}

module_init(pcie_portdrv_init);

#include <product_config.h>
/*pwrctrl interface for pcie wifi*/
#if defined(BSP_CONFIG_V7R2_ASIC) && !defined(CONFIG_BALONG_PCIE)
#include <linux/irq.h>
#include <linux/pci_ids.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include "product_config.h"
#include "soc_interrupts.h"

/*#define PCIE_WIFI_STUB_TEST*/
#if !defined(PCIE_WIFI_STUB_TEST)
/*BCM43217*/
#define WIFI_VENDOR_ID 0x14e4
#define WIFI_DEVICE_ID 0x43a9
#define WIFI_SS_VENDOR_ID 0x14e4
#define WIFI_SS_ID 0x05e9
#else
#define WIFI_VENDOR_ID PCI_VENDOR_ID_HISILICON
#if defined(CONFIG_ARCH_V7R2)
#define WIFI_DEVICE_ID PCI_DEVICE_ID_HI6930
#elif defined(CONFIG_ARCH_P531)
#define WIFI_DEVICE_ID PCI_DEVICE_ID_P531
#endif
#define WIFI_SS_VENDOR_ID 0x1
#define WIFI_SS_ID PCI_ANY_ID
#endif

static struct pci_dev* pcie_port_dev = NULL;
static struct pci_dev* pcie_wifi_dev = NULL;
static struct pci_bus* root_bus = NULL;
static u8 pcie_port_dev_int_pin = 0;
static int pcie_port_dev_int_irq = 0;
static u8 pcie_wifi_dev_int_pin = 0;
static int pcie_wifi_dev_int_irq = 0;

extern int pcie_clk_enable(void);
extern int pcie_clk_disable(void);
extern int pcie_phy_ctrl_reset(void);
extern int pcie_phy_ctrl_undo_reset(void);
extern int pcie_hw_preinit(void);
extern int pcie_ltssm_enable(bool en);
extern int pcie_is_link_up(void);
extern int pcie_link_up_confirm(void);
extern int pcie_hw_postinit(void);

extern struct regulator *regulator pcie_regulator;

/*GPIO_0_22(P531 Chip)/GPIO_1_12(HI6930 Chip)-> PCIE_PERST(P531 VC/V7R2 UDP Board)-> PERST#(mini pcie)*/
#if defined(CONFIG_ARCH_P531)
#define GPIO_PIN_PCIE_PERST  GPIO_0_22
#else
#define GPIO_PIN_PCIE_PERST  GPIO_1_12
#endif

int pcie_perst_release(void)
{
#if defined(PCIE_WIFI_STUB_TEST)
    printk(KERN_ERR "pcie perst release stub.\n");
#else
    int i = 0,ret = 0;
    printk(KERN_ERR "pcie_perst_release.\n");
	ret = gpio_request(GPIO_PIN_PCIE_PERST,"pcie-perst");
    if(ret)
    {
        printk(KERN_ERR "gpio_request %d fail,ret:%x.\n",GPIO_PIN_PCIE_PERST,ret);
    }
    for(i = 0; i < 100;i++)
    {
        udelay(1000);
    }
   	gpio_direction_output(GPIO_PIN_PCIE_PERST,1);
#endif
    return 0;
}
EXPORT_SYMBOL(pcie_perst_release);

int pcie_wifi_poweron(void)
{
#if defined(PCIE_WIFI_STUB_TEST)
    printk(KERN_ERR "pcie wifi power on stub.\n");
#else
    int i = 0;
    for(i = 0; i < 50;i++)
    {
        udelay(1000);
    }
    /*now, I do not know how to develop,tbd.*/
    /*
    printk(KERN_ERR "pcie wifi power on.\n");
    gpio_direction_output(GPIO_PIN_PCIE_PERST,1);*/
#endif
    return 0;
}
EXPORT_SYMBOL(pcie_wifi_poweron);

int pcie_wifi_powerdown(void)
{
#if defined(PCIE_WIFI_STUB_TEST)
    printk(KERN_ERR "pcie wifi power down stub.\n");
#else
    /*now, I do not know how to develop,tbd.*/
    /*
    printk(KERN_ERR "pcie wifi power down.\n");
    gpio_direction_output(GPIO_PIN_PCIE_PERST,0);*/
#endif
    return 0;
}
EXPORT_SYMBOL(pcie_wifi_powerdown);

int pcie_wifi_pwrctrl_save(void)
{
    /*
    the precondition of invoking pci_get_class:
      there is only one bridge in the system, and the bridge vendor is hisilicon.
      otherwise, we have to invoke other api, such as pci_get_subsys
    */
    pcie_port_dev = pci_get_class((PCI_CLASS_BRIDGE_PCI << 8), NULL);
    if(!pcie_port_dev)
    {
        printk(KERN_ERR "bridge not found,please check.\n");
        return -EIO;
    }
    pcie_wifi_dev = pci_get_subsys(WIFI_VENDOR_ID,WIFI_DEVICE_ID,
                                   WIFI_SS_VENDOR_ID,WIFI_SS_ID,NULL);
    if(!pcie_wifi_dev)
    {
        printk(KERN_ERR "wifi dev not found,please check.\n");
        return -EIO;
    }

    /*save interrupt pin&line*/
    pcie_port_dev_int_pin = pcie_port_dev->pin;
    pcie_port_dev_int_irq = pcie_port_dev->irq;
    pcie_wifi_dev_int_pin = pcie_wifi_dev->pin;
    pcie_wifi_dev_int_irq = pcie_wifi_dev->irq;

    pci_unregister_driver(&pcie_portdriver);
    pcie_port_bus_unregister();
    root_bus = pcie_port_dev->bus;
    pci_stop_and_remove_bus_device(pcie_port_dev);
    pcie_port_dev = NULL;
    pcie_wifi_powerdown();
    pcie_ltssm_enable(false);
    disable_irq(INT_LVL_PCIE0_LINK_DOWN);
#if !defined(BSP_CONFIG_V7R2_SFT)
    /*
    the power consumption when disable clk and reset is more than only diable clk.
    */
    /*pcie_phy_ctrl_reset();*/
    pcie_clk_disable();
#endif

    return regulator_disable(pcie_regulator);

}
EXPORT_SYMBOL(pcie_wifi_pwrctrl_save);

int pcie_wifi_pwrctrl_restore(void)
{
    int ret = 0;

    ret = regulator_enable(pcie_regulator);
    if (ret)
    {
        printk(KERN_ERR "fail to enable regulator\n");
        return ret;
    }

    pcie_wifi_poweron();
#if !defined(BSP_CONFIG_V7R2_SFT)
    pcie_clk_enable();
    pcie_phy_ctrl_reset();
    pcie_phy_ctrl_undo_reset();
#endif
    enable_irq(INT_LVL_PCIE0_LINK_DOWN);
    pcie_hw_preinit();
    pcie_ltssm_enable(true);
    ret = pcie_link_up_confirm();
    if(-EIO == ret)
    {
        goto out;
    }
    pcie_hw_postinit();
    pci_rescan_bus(root_bus);

    pcie_port_dev = pci_get_class((PCI_CLASS_BRIDGE_PCI << 8), NULL);
    if(!pcie_port_dev)
    {
        printk(KERN_ERR "bridge not found,please check.\n");
        return -EIO;
    }
    pcie_wifi_dev = pci_get_subsys(WIFI_VENDOR_ID,WIFI_DEVICE_ID,
                                   WIFI_SS_VENDOR_ID,WIFI_SS_ID,NULL);
    if(!pcie_wifi_dev)
    {
        printk(KERN_ERR "wifi dev not found,please check.\n");
        return -EIO;
    }

    /*recover interrupt pin&line*/
    pci_write_config_byte(pcie_port_dev,PCI_INTERRUPT_PIN,pcie_port_dev_int_pin);
    pci_write_config_byte(pcie_wifi_dev,PCI_INTERRUPT_PIN,pcie_wifi_dev_int_pin);
    pcie_port_dev->pin = pcie_port_dev_int_pin;
    pcie_port_dev->irq = pcie_port_dev_int_irq;
    pcie_wifi_dev->pin = pcie_wifi_dev_int_pin;
    pcie_wifi_dev->irq = pcie_wifi_dev_int_irq;
    pcibios_update_irq(pcie_port_dev,pcie_port_dev_int_irq);
    pcibios_update_irq(pcie_wifi_dev,pcie_wifi_dev_int_irq);

    ret = pcie_port_bus_register();
	if (ret) {
		printk(KERN_WARNING "PCIE: bus_register error: %d\n", ret);
		goto out;
	}
    ret = pci_register_driver(&pcie_portdriver);
	if (ret)
	{
		pcie_port_bus_unregister();
	}
 out:
	return ret;
}
EXPORT_SYMBOL(pcie_wifi_pwrctrl_restore);

int pcie_wifi_pwrctrl_suspend(void)
{
    /*TBD*/
    return 0;
}
EXPORT_SYMBOL(pcie_wifi_pwrctrl_suspend);

int pcie_wifi_pwrctrl_resume(void)
{
    int ret = 0;

    if(!pcie_is_link_up())
    {
        pcie_hw_preinit();
        pcie_ltssm_enable(true);
        ret = pcie_link_up_confirm();
        if(-EIO == ret)
        {
            goto out;
        }
        pcie_hw_postinit();
    }
    /*TBD*/
out:
	return ret;
}
EXPORT_SYMBOL(pcie_wifi_pwrctrl_resume);

#endif
