
/* #define DEBUG */
/* #define VERBOSE_DEBUG */
/*lint -save -e617*/
#include <linux/kernel.h>
#include <linux/utsname.h>

#include "bsp_usb.h"
#include "usb_vendor.h"
#include "u_ether.h"

#define DRIVER_DESC		"Balong Gadget"

/*-------------------------------------------------------------------------*/

/*
 * Kbuild is not very cooperative with respect to linking separately
 * compiled library objects into one module.  So for now we won't use
 * separate compilation ... ensuring init/exit sections work to shrink
 * the runtime footprint, and giving us at least some parts of what
 * a "gcc --combine ... part1.c part2.c part3.c ... " build would.
 */
#include "composite.c"
#include "usbstring.c"
#include "config.c"
#include "epautoconf.c"

#include "f_acm.c"
#include "u_serial.c"
#include "u_cdev.c"
#include "u_modem.c"

#include "f_mass_storage.c"

#include "f_ccid.c"
#include "u_usim.c"

#ifdef	CONFIG_BALONG_NCM
#include "f_ncm.c"
#include "u_ether.c"
#endif

#ifdef	CONFIG_BALONG_RNDIS
#include "f_rndis.c"
#include "rndis.c"
#include "u_ether.c"
#endif

#ifdef	CONFIG_BALONG_ECM
#include "f_ecm.c"
#include "u_ether.c"
#endif
/*lint -restore +e617*/
/*lint -save -e19  -e21 -e30 -e84 -e123 -e516 -e539 -e665 -e730 -e734 -e740*/
static struct fsg_module_parameters fsg_mod_data = {
	.file = {"", ""},
	.cdrom = {1, 0},
	.removable = {1, 1},
	.removable_count = 2,
	.file_count = 2,
	.luns = 2,
	.stall = 1
};
FSG_MODULE_PARAMETERS(/* no prefix */, fsg_mod_data);

static struct fsg_common fsg_common;
/*-------------------------------------------------------------------------*/
/* DO NOT REUSE THESE IDs with a protocol-incompatible driver!!  Ever!!
 * Instead:  allocate your own, using normal USB-IF procedures.
 */

/* Thanks to NetChip Technologies for donating this product ID.
 * It's for devices with only CDC Ethernet configurations.
 */
#define CDC_VENDOR_NUM		0x12D1	/* NetChip */
#ifdef	CONFIG_BALONG_RNDIS
#define CDC_PRODUCT_NUM		0x1565	/* Linux-USB Ethernet Gadget */
#else
#define CDC_PRODUCT_NUM		0x1506	/* Linux-USB Ethernet Gadget */
#endif

/*-------------------------------------------------------------------------*/

static struct usb_device_descriptor device_desc = {
	.bLength =		sizeof device_desc,
	.bDescriptorType =	USB_DT_DEVICE,

	.bcdUSB =		cpu_to_le16 (0x0300),

	.bDeviceClass =		0,
	.bDeviceSubClass =	0,
	.bDeviceProtocol =	0,
	/* .bMaxPacketSize0 = f(hardware) */

	/* Vendor and product id defaults change according to what configs
	 * we support.  (As does bNumConfigurations.)  These values can
	 * also be overridden by module parameters.
	 */
	.idVendor =		cpu_to_le16 (CDC_VENDOR_NUM),
	.idProduct =		cpu_to_le16 (CDC_PRODUCT_NUM),
	/* .bcdDevice = f(hardware) */
	/* .iManufacturer = DYNAMIC */
	/* .iProduct = DYNAMIC */
	/* NO SERIAL NUMBER */
	.bNumConfigurations =	1,
};

static struct usb_otg_descriptor otg_descriptor = {
	.bLength =		sizeof otg_descriptor,
	.bDescriptorType =	USB_DT_OTG,

	/* REVISIT SRP-only hardware is possible, although
	 * it would not be called "OTG" ...
	 */
	.bmAttributes =		USB_OTG_SRP | USB_OTG_HNP,
};

static const struct usb_descriptor_header *otg_desc[] = {
	(struct usb_descriptor_header *) &otg_descriptor,
	NULL,
};


/* string IDs are assigned dynamically */

#define STRING_MANUFACTURER_IDX		0
#define STRING_PRODUCT_IDX		1
#define STRING_SERIAL_IDX		2
static char manufacturer[50];

static struct usb_string strings_dev[] = {
	[STRING_MANUFACTURER_IDX].s = manufacturer,
	[STRING_PRODUCT_IDX].s = DRIVER_DESC,
	[STRING_SERIAL_IDX].s = "0123456789ABCDEF",
	{  } /* end of list *//*lint !e651*/
};

static struct usb_gadget_strings tab_dev = {
	.language	= 0x0409,	/* en-us */
	.strings	= strings_dev,
};

static struct usb_gadget_strings *dev_strings[] = {
	&tab_dev,
	NULL,
};

static u8 hostaddr[GNET_MAX_NUM][ETH_ALEN];
static unsigned n_acm_cdev_ports = ACM_CDEV_USED_COUNT;
static unsigned n_acm_mdm_ports = ACM_MDM_USED_COUNT;
static unsigned n_acm_tty_ports = ACM_TTY_USED_COUNT;
static unsigned n_ccid_ports = CCID_USIM_USED_COUNT;
static unsigned n_gnet_ports = GNET_USED_NUM;


/*-------------------------------------------------------------------------*/

static int udc_do_config(struct usb_configuration *c)
{
    unsigned i;
    int	status;
#ifdef CONFIG_BALONG_NCM
	unsigned int loop;
#endif

	/* FIXME alloc iConfiguration string, set it in c->strings */
	if (gadget_is_otg(c->cdev->gadget)) {
		c->descriptors = otg_desc;
		c->bmAttributes |= USB_CONFIG_ATT_WAKEUP;
	}

    /* start to add usb functions */
#ifdef CONFIG_BALONG_NCM
	for (loop = 0; loop < n_gnet_ports; loop++) {
		status = ncm_bind_config(c, hostaddr[loop]);
		if(status){
			printk("ncm_bind_config %d fail!\n", loop);
			return status;
		}
	}
#endif

#ifdef CONFIG_BALONG_RNDIS
    status = rndis_bind_config(c, hostaddr[0]);
    if(status) {
	    printk("rndis_bind_config fail:%d\n", status);
        return status;
	}
#endif

#ifdef CONFIG_BALONG_ECM
	if (can_support_ecm(c->cdev->gadget))
	{
    	status =  ecm_bind_config(c, hostaddr[0]);
        if(status) {
            printk("ecm_bind_config fail:%d\n", status);
	        return status;
	    }
    }
#endif

    for (i = 0; i < n_acm_cdev_ports; i++) {
        status = acm_bind_config(c, i, acm_class_cdev);
        if(status) {
            printk("acm_bind_config(cdev: %d) fail:%d\n", i, status);
            return status;
        }
    }

    for (i = 0; i < n_acm_tty_ports; i++) {
        status = acm_bind_config(c, i, acm_class_tty);
        if(status){
            printk("acm_bind_config(tty: %d) fail:%d\n", i, status);
            return status;
        }
    }

    for (i = 0; i < n_acm_mdm_ports; i++) {
        status = acm_bind_config(c, i, acm_class_modem);
        if(status){
            printk("acm_bind_config(modem: %d) fail:%d\n", i, status);
            return status;
        }
    }

    for (i = 0; i < n_ccid_ports; i++) {
        status = ccid_bind_config(c, i);
        if(status){
            printk("ccid_bind_config(ccid: %d) fail:%d\n", i, status);
            return status;
        }
    }

#ifdef CONFIG_USB_FSG
	status = fsg_bind_config(c->cdev, c, &fsg_common);
	if (status < 0){
            printk("fsg_bind_config(mass storage) fail:%d\n", status);
            return status;
        }
    /* end to add usb functions */
#endif

    return 0;
}

static struct usb_configuration udc_config_driver = {
	/* .label = f(hardware) */
	.label			= "balong udc",
	.bConfigurationValue	= 1,
	/* .iConfiguration = DYNAMIC */
	.bmAttributes		= USB_CONFIG_ATT_SELFPOWER,
	/*.bMaxPower          = 250,*/
};

/*-------------------------------------------------------------------------*/

//static int __init udc_bind(struct usb_composite_dev *cdev)
int udc_bind(struct usb_composite_dev *cdev)
{
	int			gcnum;
	struct usb_gadget	*gadget = cdev->gadget;
	int			status;
	int         err_line = 0;
    unsigned int loop;

    /* don't need to memset 0 g_usb_ctx,
     * it will clear enable/disable cbs, in usb insert pmu detect
     */
    /* memset(&g_usb_ctx, 0, sizeof(g_usb_ctx)); */
    bsp_usb_init_enum_stat();

    /* start to add physical functions */
	/* set up network link layer */
	for(loop=0; loop<n_gnet_ports; loop++){
		status = gether_setup(cdev->gadget, hostaddr[loop]);
		if (status < 0)
			goto fail_clean_eth;
	}

    if (n_acm_tty_ports > 0) {
        status = gserial_setup(cdev->gadget, n_acm_tty_ports);
    	if (status < 0) {
    	    err_line = __LINE__;
    		goto fail_clean_eth;
        }
    }

    if (n_acm_cdev_ports > 0) {
        status = gacm_cdev_setup(cdev->gadget, n_acm_cdev_ports);
    	if (status < 0){
    	    err_line = __LINE__;
    		goto fail_clean_tty;
        }
    }

    if (n_acm_mdm_ports > 0) {
        status = gacm_modem_setup(cdev->gadget, n_acm_mdm_ports);
    	if (status < 0){
    	    err_line = __LINE__;
    		goto fail_clean_cdev;
    	}
    }

    if (n_ccid_ports > 0) {
        status = gusim_setup(cdev->gadget, n_ccid_ports);
    	if (status < 0){
    	    err_line = __LINE__;
    		goto fail_clean_mdm;
    	}
    }
#ifdef	CONFIG_USB_FSG
	/* set up mass storage function */
	{
		void *retp;
		retp = fsg_common_from_params(&fsg_common, cdev, &fsg_mod_data);
		if (IS_ERR(retp)) {
			status = PTR_ERR(retp);
			err_line = __LINE__;
			goto fail_clean_fsg;
		}
	}
#endif
    /* end to add physical functions */

	gcnum = usb_gadget_controller_number(gadget);
	if (gcnum >= 0)
        device_desc.bcdDevice = cpu_to_le16(0x0300 | gcnum);
	else {
		/* We assume that can_support_ecm() tells the truth;
		 * but if the controller isn't recognized at all then
		 * that assumption is a bit more likely to be wrong.
		 */
		dev_warn(&gadget->dev,
			 "controller '%s' not recognized; trying %s\n",
			 gadget->name,
			 udc_config_driver.label);
		device_desc.bcdDevice =
			cpu_to_le16(0x0300 | 0x0099);
	}


	/* Allocate string descriptor numbers ... note that string
	 * contents can be overridden by the composite_dev glue.
	 */

	/* device descriptor strings: manufacturer, product */
	snprintf(manufacturer, sizeof manufacturer, "%s %s with %s",
		init_utsname()->sysname, init_utsname()->release,
		gadget->name);
	status = usb_string_id(cdev);
	if (status < 0)
		goto fail_clean_all;
	strings_dev[STRING_MANUFACTURER_IDX].id = status;
	device_desc.iManufacturer = status;

	status = usb_string_id(cdev);
	if (status < 0) {
	    err_line = __LINE__;
		goto fail_clean_all;
	}
	strings_dev[STRING_PRODUCT_IDX].id = status;
	device_desc.iProduct = status;

	status = usb_string_id(cdev);
	if (status < 0){
	    err_line = __LINE__;
		goto fail_clean_all;
	}
	strings_dev[STRING_SERIAL_IDX].id = status;
	device_desc.iSerialNumber = status;

#if defined(CONFIG_GADGET_SUPPORT_REMOTE_WAKEUP)
    /* enable the remote wakeup, however, this attribute could be disabled
       by the usb gadget driver or host driver */
   udc_config_driver.bmAttributes |= USB_CONFIG_ATT_WAKEUP;
#endif
	status = usb_add_config(cdev, &udc_config_driver,
				udc_do_config);
	if (status < 0) {
	    err_line = __LINE__;
		goto fail_clean_all;
	}

	USB_DBG_GADGET(&gadget->dev, "%s\n", DRIVER_DESC);

	return 0;


fail_clean_all:
#ifdef CONFIG_USB_FSG
	fsg_common_put(&fsg_common);
fail_clean_fsg:
#endif
    if (n_ccid_ports > 0)
        gusim_cleanup();
fail_clean_mdm:
    if (n_acm_mdm_ports > 0)
        gacm_modem_cleanup();
fail_clean_cdev:
    if (n_acm_cdev_ports > 0)
        gacm_cdev_cleanup();
fail_clean_tty:
    if (n_acm_tty_ports > 0)
        gserial_cleanup();
fail_clean_eth:
	if(n_gnet_ports > 0)
		gether_cleanup();

	printk("udc_bind error, err line:%d\n", err_line);
	return status;
}


//static int __exit udc_unbind(struct usb_composite_dev *cdev)
static int udc_unbind(struct usb_composite_dev *cdev)
{
	if(n_gnet_ports > 0)
		gether_cleanup();

    if (n_acm_tty_ports > 0)
        gserial_cleanup();

    if (n_acm_cdev_ports > 0)
        gacm_cdev_cleanup();

    if (n_acm_mdm_ports > 0)
        gacm_modem_cleanup();

    if (n_ccid_ports > 0)
        gusim_cleanup();

	fsg_common_put(&fsg_common);

	return 0;
}

static struct usb_composite_driver udc_driver = {
	.name		= "g_balong_udc",
	.dev		= &device_desc,
	.strings	= dev_strings,
    .max_speed	= USB_SPEED_SUPER,
	.unbind		= udc_unbind,//__exit_p(udc_unbind),
};

static int g_is_usb_balong_init = 0;



#if 0
static int __init init(void)
{
	return usb_composite_probe(&udc_driver, udc_bind);
}
module_init(init);

static void __exit cleanup(void)
{
	usb_composite_unregister(&udc_driver);
}
module_exit(cleanup);
#endif

extern int usb_dwc3_platform_dev_init(void);
extern void usb_dwc3_plaform_dev_exit(void);

extern int usb_dwc3_platform_drv_init(void);
extern void usb_dwc3_platform_drv_exit(void);
#ifdef CONFIG_USB_OTG
extern int dwc_otg_init(void);
extern void dwc_otg_exit(void);
#endif

int usb_balong_init(void)
{
    int ret = 0;

    if (g_is_usb_balong_init) {
        printk("%s:balong usb is already init\n", __FUNCTION__);
        return -EPERM;
    }

#ifdef CONFIG_USB_OTG
    ret = dwc_otg_init();
    if (ret) {
        printk("%s:dwc_otg_init fail:%d\n", __FUNCTION__, ret);
        return ret;
    }
#endif

    ret = usb_dwc3_platform_dev_init();
    if (ret) {
        printk("%s:usb_dwc3_platform_dev_init fail:%d\n", __FUNCTION__, ret);
        goto ret_exit;
    }

    ret = usb_dwc3_platform_drv_init();
    if (ret) {
        printk("%s:usb_dwc3_platform_drv_init fail:%d\n", __FUNCTION__, ret);
        goto ret_dev_exit;
    }

    ret = usb_composite_probe(&udc_driver, udc_bind);
    if (ret) {
        printk("%s:usb_composite_probe fail:%d\n", __FUNCTION__, ret);
        goto ret_drv_exit;
    }

    g_is_usb_balong_init = 1;
    return 0;

ret_drv_exit:
    usb_dwc3_platform_drv_exit();
ret_dev_exit:
    usb_dwc3_plaform_dev_exit();
ret_exit:
#ifdef CONFIG_USB_OTG
    dwc_otg_exit();
#endif
    g_is_usb_balong_init = 0;
    return ret;
}

void usb_balong_exit(void)
{
    if (!g_is_usb_balong_init) {
        printk("%s: balong usb is not init\n", __FUNCTION__);
        return;
    }

    usb_composite_unregister(&udc_driver);

    usb_dwc3_platform_drv_exit();

    usb_dwc3_plaform_dev_exit();

#ifdef CONFIG_USB_OTG
    dwc_otg_exit();
#endif

    printk("%s ok\n", __FUNCTION__);
    g_is_usb_balong_init = 0;
    return;
}

MODULE_DESCRIPTION(UDC_DRIVER_DESC);
MODULE_AUTHOR("balong bsp4net");
MODULE_LICENSE("GPL");
/*lint -restore*/