/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/otg/linux/drivers/dwc_otg_hcd_linux.c $
 * $Revision: #20 $
 * $Date: 2011/10/26 $
 * $Change: 1872981 $
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */
  /*lint -save -e7 -e30 -e82 -e110 -e322 --e{402} -e438 -e502 -e515 
  -e527 -e528 -e529 -e533 -e537 -e539 -e695 
  -e573 -e701 -e713 -e730 -e732 -e734 -e737 -e740 -e744 -e958*/

#ifndef DWC_DEVICE_ONLY

/**
 * @file
 *
 * This file contains the implementation of the HCD. In Linux, the HCD
 * implements the hc_driver API.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
#include <asm/io.h>
#include <linux/usb.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
#include <../drivers/usb/core/hcd.h>
#else
#include <linux/usb/hcd.h>
#endif

#include "dwc_otg_hcd_if.h"
#include "dwc_otg_dbg.h"
#include "dwc_otg_driver.h"
#include "dwc_otg_hcd.h"



/**
 * Gets the endpoint number from a _bEndpointAddress argument. The endpoint is
 * qualified with its direction (possible 32 endpoints per device).
 */
#define dwc_ep_addr_to_endpoint(_bEndpointAddress_) ((_bEndpointAddress_ & USB_ENDPOINT_NUMBER_MASK) | \
						     ((_bEndpointAddress_ & USB_DIR_IN) != 0) << 4)

static const char dwc_otg_hcd_name[] = "dwc_otg_hcd";

#ifdef DWC2_SUPPORT_URB_SG
struct dwc_otg_urb_sg urb_sg_data;
void hsic_sg_timeout(unsigned long time_out_data);
#endif

/** @name Linux HC Driver API Functions */
/** @{ */
static int urb_enqueue(struct usb_hcd *hcd,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
		       struct usb_host_endpoint *ep,
#endif
		       struct urb *urb, gfp_t mem_flags);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
static int urb_dequeue(struct usb_hcd *hcd, struct urb *urb);
#else
static int urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status);
#endif

static void endpoint_disable(struct usb_hcd *hcd, struct usb_host_endpoint *ep);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
static void endpoint_reset(struct usb_hcd *hcd, struct usb_host_endpoint *ep);
#endif
static irqreturn_t dwc_otg_hcd_irq(struct usb_hcd *hcd);
extern int hcd_start(struct usb_hcd *hcd);
extern void hcd_stop(struct usb_hcd *hcd);
static int get_frame_number(struct usb_hcd *hcd);
extern int hub_status_data(struct usb_hcd *hcd, char *buf);
extern int hub_control(struct usb_hcd *hcd,
		       u16 typeReq,
		       u16 wValue, u16 wIndex, char *buf, u16 wLength);

#ifdef CONFIG_PM
static int dwc_bus_suspend(struct usb_hcd *hcd);
static int dwc_bus_resume(struct usb_hcd *hcd);
#else
#define dwc_bus_suspend		NULL
#define dwc_bus_resume		NULL
#endif

struct wrapper_priv_data {
	dwc_otg_hcd_t *dwc_otg_hcd;
};

/** @} */

static struct hc_driver dwc_otg_hc_driver = {

	.description = dwc_otg_hcd_name,
	.product_desc = "DWC OTG Controller",
	.hcd_priv_size = sizeof(struct wrapper_priv_data),

	.irq = dwc_otg_hcd_irq,

	.flags = HCD_MEMORY | HCD_USB2,

	//.reset =              
	.start = hcd_start,
	//.suspend =            
	//.resume =             
	.stop = hcd_stop,

	.urb_enqueue = urb_enqueue,
	.urb_dequeue = urb_dequeue,
	.endpoint_disable = endpoint_disable,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
	.endpoint_reset = endpoint_reset,
#endif
	.get_frame_number = get_frame_number,

	.hub_status_data = hub_status_data,
	.hub_control = hub_control,
	.bus_suspend = dwc_bus_suspend,
	.bus_resume = dwc_bus_resume,
};

/** Gets the dwc_otg_hcd from a struct usb_hcd */
static inline dwc_otg_hcd_t *hcd_to_dwc_otg_hcd(struct usb_hcd *hcd)
{
	struct wrapper_priv_data *p;
	p = (struct wrapper_priv_data *)(hcd->hcd_priv);
	return p->dwc_otg_hcd;
}

/** Gets the struct usb_hcd that contains a dwc_otg_hcd_t. */
static inline struct usb_hcd *dwc_otg_hcd_to_hcd(dwc_otg_hcd_t * dwc_otg_hcd)
{
	return dwc_otg_hcd_get_priv_data(dwc_otg_hcd);
}

/** Gets the usb_host_endpoint associated with an URB. */
inline struct usb_host_endpoint *dwc_urb_to_endpoint(struct urb *urb)
{
	struct usb_device *dev = urb->dev;
	int ep_num = usb_pipeendpoint(urb->pipe);

	if (usb_pipein(urb->pipe))
		return dev->ep_in[ep_num];
	else
		return dev->ep_out[ep_num];
}

static int _disconnect(dwc_otg_hcd_t * hcd)
{
	struct usb_hcd *usb_hcd = dwc_otg_hcd_to_hcd(hcd);

	usb_hcd->self.is_b_host = 0;
	return 0;
}

static int _start(dwc_otg_hcd_t * hcd)
{
	struct usb_hcd *usb_hcd = dwc_otg_hcd_to_hcd(hcd);

	usb_hcd->self.is_b_host = dwc_otg_hcd_is_b_host(hcd);
	hcd_start(usb_hcd);

	return 0;
}

static int _hub_info(dwc_otg_hcd_t * hcd, void *urb_handle, uint32_t * hub_addr,
		     uint32_t * port_addr)
{
	struct urb *urb = (struct urb *)urb_handle;
	if (urb->dev->tt) {
		*hub_addr = urb->dev->tt->hub->devnum;
	} else {
		*hub_addr = 0;
	}
	*port_addr = urb->dev->ttport;
	return 0;
}

static int _speed(dwc_otg_hcd_t * hcd, void *urb_handle)
{
	struct urb *urb = (struct urb *)urb_handle;
	return urb->dev->speed;
}

static int _get_b_hnp_enable(dwc_otg_hcd_t * hcd)
{
	struct usb_hcd *usb_hcd = dwc_otg_hcd_to_hcd(hcd);
	return usb_hcd->self.b_hnp_enable;
}

static void allocate_bus_bandwidth(struct usb_hcd *hcd, uint32_t bw,
				   struct urb *urb)
{
	hcd_to_bus(hcd)->bandwidth_allocated += bw / urb->interval;/*lint !e573*/
	if (usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS) {
		hcd_to_bus(hcd)->bandwidth_isoc_reqs++;
	} else {
		hcd_to_bus(hcd)->bandwidth_int_reqs++;
	}
}

static void free_bus_bandwidth(struct usb_hcd *hcd, uint32_t bw,
			       struct urb *urb)
{
	hcd_to_bus(hcd)->bandwidth_allocated -= bw / urb->interval;
	if (usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS) {
		hcd_to_bus(hcd)->bandwidth_isoc_reqs--;
	} else {
		hcd_to_bus(hcd)->bandwidth_int_reqs--;
	}
}

/**
 * Sets the final status of an URB and returns it to the device driver. Any
 * required cleanup of the URB is performed.
 */
static int _complete(dwc_otg_hcd_t * hcd, void *urb_handle,
		     dwc_otg_hcd_urb_t * dwc_otg_urb, int32_t status)
{
	struct urb *urb = (struct urb *)urb_handle;
#ifdef DEBUG
	if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB)) {
		DWC_PRINTF("%s: urb %p, device %d, ep %d %s, status=%d\n",
			   __func__, urb, usb_pipedevice(urb->pipe),
			   usb_pipeendpoint(urb->pipe),
			   usb_pipein(urb->pipe) ? "IN" : "OUT", status);
		if (usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS) {
			int i;
			for (i = 0; i < urb->number_of_packets; i++) {
				DWC_PRINTF("  ISO Desc %d status: %d\n",
					   i, urb->iso_frame_desc[i].status);
			}
		}
	}
#endif

	urb->actual_length = dwc_otg_hcd_urb_get_actual_length(dwc_otg_urb);

#ifdef WIFI_HSIC_DEBUG
	/* fuxiaowei: this code is added for debugging. 1522 is the max size of the buffer on BULK endpoint
	   from WIFI dhd driver. If we recevie a large packet, we need to mark it */
	if (dwc_otg_urb->actual_length > dwc_otg_urb->length) {
		/* int i = 0, j = 0; */
		/* unsigned char* buf = (unsigned char*)dwc_otg_urb->buf;*/

		printk("\n");
		printk("WIFI HSIC recv large packet, buffer length: %u, packet length: %u\n", 
			dwc_otg_urb->length, dwc_otg_urb->actual_length);
		printk("urb buffer len: %u, urb pipe type: %u\n", urb->transfer_buffer_length, usb_pipetype(urb->pipe));
		/*for (i = 0; i < dwc_otg_urb->actual_length; i++) {
			printk("%02x ", buf[i]);
			j++;
			if ((j % 16) == 0) printk("\n"); 
		}
		printk("\n");*/
	}
	/* fuxiaowei */
#endif

	/* Convert status value. */
	switch (status) {
	case -DWC_E_PROTOCOL:
		status = -EPROTO;
		break;
	case -DWC_E_IN_PROGRESS:
		status = -EINPROGRESS;
		break;
	case -DWC_E_PIPE:
		status = -EPIPE;
		break;
	case -DWC_E_IO:
		status = -EIO;
		break;
	case -DWC_E_TIMEOUT:
		status = -ETIMEDOUT;
		break;
	case -DWC_E_OVERFLOW:
		status = -EOVERFLOW;
		break;
	default:
		if (status) {
			DWC_PRINTF("Uknown urb status %d\n", status);

		}
	}

	if (usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS) {
		int i;

		urb->error_count = dwc_otg_hcd_urb_get_error_count(dwc_otg_urb);
		for (i = 0; i < urb->number_of_packets; ++i) {
			urb->iso_frame_desc[i].actual_length =
			    dwc_otg_hcd_urb_get_iso_desc_actual_length
			    (dwc_otg_urb, i);
			urb->iso_frame_desc[i].status =
			    dwc_otg_hcd_urb_get_iso_desc_status(dwc_otg_urb, i);
		}
	}

	urb->status = status;
	urb->hcpriv = NULL;
	if (!status) {
		if ((urb->transfer_flags & URB_SHORT_NOT_OK) &&
		    (urb->actual_length < urb->transfer_buffer_length)) {
			urb->status = -EREMOTEIO;
		}
	}

	if ((usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS) ||
	    (usb_pipetype(urb->pipe) == PIPE_INTERRUPT)) {
		struct usb_host_endpoint *ep = dwc_urb_to_endpoint(urb);
		if (ep) {
			free_bus_bandwidth(dwc_otg_hcd_to_hcd(hcd),
					   dwc_otg_hcd_get_ep_bandwidth(hcd,
									ep->hcpriv),
					   urb);
		}
	}

	DWC_FREE(dwc_otg_urb);

	DWC_SPINUNLOCK(hcd->lock);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
	usb_hcd_giveback_urb(dwc_otg_hcd_to_hcd(hcd), urb);
#else
	usb_hcd_giveback_urb(dwc_otg_hcd_to_hcd(hcd), urb, status);
#endif
	DWC_SPINLOCK(hcd->lock);

	return 0;
}

static struct dwc_otg_hcd_function_ops hcd_fops = {
	.start = _start,
	.disconnect = _disconnect,
	.hub_info = _hub_info,
	.speed = _speed,
	.complete = _complete,
	.get_b_hnp_enable = _get_b_hnp_enable,
};

/**
 * Initializes the HCD. This function allocates memory for and initializes the
 * static parts of the usb_hcd and dwc_otg_hcd structures. It also registers the
 * USB bus with the core and calls the hc_driver->start() function. It returns
 * a negative error on failure.
 */
int hcd_init(
#ifdef LM_INTERFACE
		    struct lm_device *_dev
#elif  defined(PCI_INTERFACE)
		    struct pci_dev *_dev
#endif
    )
{
	struct usb_hcd *hcd = NULL;
	dwc_otg_hcd_t *dwc_otg_hcd = NULL;
#ifdef LM_INTERFACE
	dwc_otg_device_t *otg_dev = lm_get_drvdata(_dev);
#elif  defined(PCI_INTERFACE)
	dwc_otg_device_t *otg_dev = pci_get_drvdata(_dev);
#endif

	int retval = 0;

	DWC_DEBUGPL(DBG_HCD, "DWC OTG HCD INIT\n");

	/* Set device flags indicating whether the HCD supports DMA. */
	if (dwc_otg_is_dma_enable(otg_dev->core_if)) {
#ifdef LM_INTERFACE
		_dev->dev.dma_mask = (void *)~0;
		_dev->dev.coherent_dma_mask = ~0;
#elif  defined(PCI_INTERFACE)
		pci_set_dma_mask(_dev, DMA_32BIT_MASK);
		pci_set_consistent_dma_mask(_dev, DMA_32BIT_MASK);
#endif

	} else {
#ifdef LM_INTERFACE
		_dev->dev.dma_mask = (void *)0;
		_dev->dev.coherent_dma_mask = 0;
#elif  defined(PCI_INTERFACE)
		pci_set_dma_mask(_dev, 0);
		pci_set_consistent_dma_mask(_dev, 0);
#endif
	}

	/*
	 * Allocate memory for the base HCD plus the DWC OTG HCD.
	 * Initialize the base HCD.
	 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)

	hcd = usb_create_hcd(&dwc_otg_hc_driver, &_dev->dev, _dev->dev.bus_id);
#else
	/* coverity[returned_null] */
	hcd = usb_create_hcd(&dwc_otg_hc_driver, &_dev->dev, dev_name(&_dev->dev));/* [false alarm]:fortify disable */
	/* coverity[deref_ptr] */
	hcd->has_tt = 1;
//      hcd->uses_new_polling = 1;
//      hcd->poll_rh = 0;
#endif
	/* coverity[check_after_deref] */
	if (!hcd) {
		retval = -ENOMEM;
		goto error1;
	}

	hcd->regs = otg_dev->os_dep.base;

	/* Initialize the DWC OTG HCD. */
	dwc_otg_hcd = dwc_otg_hcd_alloc_hcd();
	if (!dwc_otg_hcd) {
		goto error2;
	}
	((struct wrapper_priv_data *)(hcd->hcd_priv))->dwc_otg_hcd =
	    dwc_otg_hcd;
	otg_dev->hcd = dwc_otg_hcd;/*lint !e63*/

	if (dwc_otg_hcd_init(dwc_otg_hcd, otg_dev->core_if)) {
		goto error2;
	}

	otg_dev->hcd->otg_dev = otg_dev;/*lint !e63*/
	hcd->self.otg_port = dwc_otg_hcd_otg_port(dwc_otg_hcd);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33) //don't support for LM(with 2.6.20.1 kernel)
	/* hcd->self.otg_version = dwc_otg_get_otg_version(otg_dev->core_if); */
	/* Don't support SG list at this point */
	hcd->self.sg_tablesize = 0;
#endif
	/*
	 * Finish generic HCD initialization and start the HCD. This function
	 * allocates the DMA buffer pool, registers the USB bus, requests the
	 * IRQ line, and calls hcd_start method.
	 */
	retval = usb_add_hcd(hcd, _dev->irq, IRQF_SHARED | IRQF_DISABLED);
	if (retval < 0) {
		goto error2;
	}

	dwc_otg_hcd_set_priv_data(dwc_otg_hcd, hcd);

	#ifdef DWC2_SUPPORT_URB_SG
	urb_sg_data.u32_hsic_sg_max_urb_num = 16;
	urb_sg_data.u32_hsic_sg_max_urb_size = 19200;
	urb_sg_data.u32_hsic_sg_timeout = 2;

	urb_sg_data.u32_urb_sg_num = 0;
	urb_sg_data.u32_urb_sg_size = 0;

	urb_sg_data.u32_hsic_sg_timeout_count = 0;
	urb_sg_data.u32_hsic_sg_send_count = 0;
	urb_sg_data.u32_hsic_sg_urb_count = 0;
	urb_sg_data.u32_hsic_sg_queue_full_count = 0;

	/* init timer */
	init_timer(&urb_sg_data.hsic_sg_timer);
	urb_sg_data.hsic_sg_timer.function = hsic_sg_timeout;
	urb_sg_data.hsic_sg_timer.data = hcd;
	#endif

	return 0;

error2:
	usb_put_hcd(hcd);
error1:
	return retval;
}

/**
 * Removes the HCD.
 * Frees memory and resources associated with the HCD and deregisters the bus.
 */
void hcd_remove(
#ifdef LM_INTERFACE
		       struct lm_device *_dev
#elif  defined(PCI_INTERFACE)
		       struct pci_dev *_dev
#endif
    )
{
#ifdef LM_INTERFACE
	dwc_otg_device_t *otg_dev = lm_get_drvdata(_dev);
#elif  defined(PCI_INTERFACE)
	dwc_otg_device_t *otg_dev = pci_get_drvdata(_dev);
#endif

	dwc_otg_hcd_t *dwc_otg_hcd;
	struct usb_hcd *hcd;

	DWC_DEBUGPL(DBG_HCD, "DWC OTG HCD REMOVE\n");

	if (!otg_dev) {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev NULL!\n", __func__);
		return;
	}

	dwc_otg_hcd = otg_dev->hcd;

	if (!dwc_otg_hcd) {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev->hcd NULL!\n", __func__);
		return;
	}

	hcd = dwc_otg_hcd_to_hcd(dwc_otg_hcd);

	if (!hcd) {
		DWC_DEBUGPL(DBG_ANY,
			    "%s: dwc_otg_hcd_to_hcd(dwc_otg_hcd) NULL!\n",
			    __func__);
		return;
	}
	usb_remove_hcd(hcd);
	dwc_otg_hcd_set_priv_data(dwc_otg_hcd, NULL);
	dwc_otg_hcd_remove(dwc_otg_hcd);
	usb_put_hcd(hcd);
}

/* =========================================================================
 *  Linux HC Driver Functions
 * ========================================================================= */

/** Initializes the DWC_otg controller and its root hub and prepares it for host
 * mode operation. Activates the root port. Returns 0 on success and a negative
 * error code on failure. */
int hcd_start(struct usb_hcd *hcd)
{
	dwc_otg_hcd_t *dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);
	struct usb_bus *bus;

	DWC_DEBUGPL(DBG_HCD, "DWC OTG HCD START\n");
	bus = hcd_to_bus(hcd);

	hcd->state = HC_STATE_RUNNING;
	if (dwc_otg_hcd_start(dwc_otg_hcd, &hcd_fops)) {
		return 0;
	}

	/* Initialize and connect root hub if one is not already attached */
	if (bus->root_hub) {
		DWC_DEBUGPL(DBG_HCD, "DWC OTG HCD Has Root Hub\n");
		/* Inform the HUB driver to resume. */
		usb_hcd_resume_root_hub(hcd);
	}

	return 0;
}

/**
 * Halts the DWC_otg host mode operations in a clean manner. USB transfers are
 * stopped.
 */
void hcd_stop(struct usb_hcd *hcd)
{
	dwc_otg_hcd_t *dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);

	dwc_otg_hcd_stop(dwc_otg_hcd);
}

/** Returns the current frame number. */
static int get_frame_number(struct usb_hcd *hcd)
{
	dwc_otg_hcd_t *dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);

	return dwc_otg_hcd_get_frame_number(dwc_otg_hcd);
}

#ifdef DEBUG
static void dump_urb_info(struct urb *urb, char *fn_name)
{
	DWC_PRINTF("%s, urb %p\n", fn_name, urb);
	DWC_PRINTF("  Device address: %d\n", usb_pipedevice(urb->pipe));
	DWC_PRINTF("  Endpoint: %d, %s\n", usb_pipeendpoint(urb->pipe),
		   (usb_pipein(urb->pipe) ? "IN" : "OUT"));
	DWC_PRINTF("  Endpoint type: %s\n", ( {
					     char *pipetype;
					     switch (usb_pipetype(urb->pipe)) {
case PIPE_CONTROL:
pipetype = "CONTROL"; break; case PIPE_BULK:
pipetype = "BULK"; break; case PIPE_INTERRUPT:
pipetype = "INTERRUPT"; break; case PIPE_ISOCHRONOUS:
pipetype = "ISOCHRONOUS"; break; default:
					     pipetype = "UNKNOWN"; break;};
					     pipetype;}
		   )) ;
	DWC_PRINTF("  Speed: %s\n", ( {
				     char *speed; switch (urb->dev->speed) {
case USB_SPEED_HIGH:
speed = "HIGH"; break; case USB_SPEED_FULL:
speed = "FULL"; break; case USB_SPEED_LOW:
speed = "LOW"; break; default:
				     speed = "UNKNOWN"; break;};
				     speed;}
		   )) ;
	DWC_PRINTF("  Max packet size: %d\n",
		   usb_maxpacket(urb->dev, urb->pipe, usb_pipeout(urb->pipe)));
	DWC_PRINTF("  Data buffer length: %d\n", urb->transfer_buffer_length);
	DWC_PRINTF("  Transfer buffer: %p, Transfer DMA: %p\n",
		   urb->transfer_buffer, (void *)urb->transfer_dma);
	DWC_PRINTF("  Setup buffer: %p, Setup DMA: %p\n",
		   urb->setup_packet, (void *)urb->setup_dma);
	DWC_PRINTF("  Interval: %d\n", urb->interval);
	if (usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS) {
		int i;
		for (i = 0; i < urb->number_of_packets; i++) {
			DWC_PRINTF("  ISO Desc %d:\n", i);
			DWC_PRINTF("    offset: %d, length %d\n",
				   urb->iso_frame_desc[i].offset,
				   urb->iso_frame_desc[i].length);
		}
	}
}

#endif
#ifdef DWC2_SUPPORT_URB_SG

void hsic_sg_status()
{
	DWC_PRINTF(" hsic_sg_timeout_count = %u \n u32_hsic_sg_queue_full_count = %u \n  hsic_sg_send_count = %u \n  hsic_sg_urb_count = %u \n \n", 
		urb_sg_data.u32_hsic_sg_timeout_count,urb_sg_data.u32_hsic_sg_queue_full_count,urb_sg_data.u32_hsic_sg_send_count,urb_sg_data.u32_hsic_sg_urb_count);

	DWC_PRINTF("  u32_hsic_sg_max_urb_num = %u \n u32_hsic_sg_max_urb_size = %u \n  u32_hsic_sg_timeout = %u \n ", 
		urb_sg_data.u32_hsic_sg_max_urb_num,urb_sg_data.u32_hsic_sg_max_urb_size,urb_sg_data.u32_hsic_sg_timeout);

	return;
}
void reset_hsic_sg_status()
{
	urb_sg_data.u32_hsic_sg_timeout_count = 0;
	urb_sg_data.u32_hsic_sg_send_count = 0;
	urb_sg_data.u32_hsic_sg_urb_count = 0;
	urb_sg_data.u32_hsic_sg_queue_full_count = 0;
	return;
}
void hsic_set_max_urb_num(unsigned int u32_sg_max_urb_num)
{
	urb_sg_data.u32_hsic_sg_max_urb_num = u32_sg_max_urb_num;
	return;
}
void hsic_set_sg_max_urb_size(unsigned int u32_sg_max_urb_size)
{
	urb_sg_data.u32_hsic_sg_max_urb_size = u32_sg_max_urb_size;
	return;
}

void hsic_set_sg_timeout(unsigned int u32_sg_timeout)
{

	urb_sg_data.u32_hsic_sg_timeout = u32_sg_timeout;
	return;
}
void hsic_sg_timeout(unsigned long time_out_data)
{
	struct usb_hcd *hcd = time_out_data;
	dwc_irqflags_t flags;
	int retval = 0;
	dwc_otg_hcd_t *dwc_otg_hcd= hcd_to_dwc_otg_hcd(hcd);
	gintmsk_data_t intr_mask = {.d32 = 0 };

	if (!dwc_otg_hcd->flags.b.port_connect_status) {
		/* No longer connected. */
		DWC_ERROR("Not connected\n");
		return -DWC_E_NO_DEVICE;
	}

	dwc_otg_transaction_type_e tr_type;
	DWC_SPINLOCK_IRQSAVE(dwc_otg_hcd->lock, &flags);
	tr_type = dwc_otg_hcd_select_transactions(dwc_otg_hcd);
	if (tr_type != DWC_OTG_TRANSACTION_NONE) {
		dwc_otg_hcd_queue_transactions(dwc_otg_hcd, tr_type);
	}
	DWC_SPINUNLOCK_IRQRESTORE(dwc_otg_hcd->lock, flags);
	urb_sg_data.u32_hsic_sg_send_count++;
	urb_sg_data.u32_hsic_sg_urb_count += urb_sg_data.u32_urb_sg_num;

	urb_sg_data.u32_urb_sg_num = 0;
	/*size = 0*/
	urb_sg_data.u32_urb_sg_size = 0;
	urb_sg_data.u32_hsic_sg_timeout_count++;
}

#endif

/** Starts processing a USB transfer request specified by a USB Request Block
 * (URB). mem_flags indicates the type of memory allocation to use while
 * processing this URB. */
static int urb_enqueue(struct usb_hcd *hcd,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
		       struct usb_host_endpoint *ep,
#endif
		       struct urb *urb, gfp_t mem_flags)
{
	int retval = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28)
	struct usb_host_endpoint *ep = urb->ep;
#endif
	dwc_otg_hcd_t *dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);
	dwc_otg_hcd_urb_t *dwc_otg_urb;
	int i;
	int alloc_bandwidth = 0;
	uint8_t ep_type = 0;
	uint32_t flags = 0;
	void *buf;

#ifdef DEBUG
	if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB)) {
		dump_urb_info(urb, "urb_enqueue");
	}
#endif

	if ((usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS)
	    || (usb_pipetype(urb->pipe) == PIPE_INTERRUPT)) {
		if (!dwc_otg_hcd_is_bandwidth_allocated
		    (dwc_otg_hcd, &ep->hcpriv)) {
			alloc_bandwidth = 1;
		}
	}

	switch (usb_pipetype(urb->pipe)) {
	case PIPE_CONTROL:
		ep_type = USB_ENDPOINT_XFER_CONTROL;
		break;
	case PIPE_ISOCHRONOUS:
		ep_type = USB_ENDPOINT_XFER_ISOC;
		break;
	case PIPE_BULK:
		ep_type = USB_ENDPOINT_XFER_BULK;
		break;
	case PIPE_INTERRUPT:
		ep_type = USB_ENDPOINT_XFER_INT;
		break;
	default:
		DWC_WARN("Wrong ep type\n");
	}

	dwc_otg_urb = dwc_otg_hcd_urb_alloc(dwc_otg_hcd,
					    urb->number_of_packets,
					    mem_flags == GFP_ATOMIC ? 1 : 0);
    /* BEGIN: Modified by Mabinjie at 2012-4-16 */
    /* Prevent failed to alloc urb memory */
    if (dwc_otg_urb == NULL)
    {
        DWC_ERROR("Alloc urb memory failed\n");
        return -ENOMEM;
    }
    /* END:   Modified by Mabinjie at 2012-04-16 */

	dwc_otg_hcd_urb_set_pipeinfo(dwc_otg_urb, usb_pipedevice(urb->pipe),
				     usb_pipeendpoint(urb->pipe), ep_type,
				     usb_pipein(urb->pipe),
				     usb_maxpacket(urb->dev, urb->pipe,
						   !(usb_pipein(urb->pipe))));

	buf = urb->transfer_buffer;
	if (hcd->self.uses_dma) {
		/*
		 * Calculate virtual address from physical address,
		 * because some class driver may not fill transfer_buffer.
		 * In Buffer DMA mode virual address is used,
		 * when handling non DWORD aligned buffers.
		 */
		buf = phys_to_virt(urb->transfer_dma);
	}
#ifdef DWC2_SUPPORT_URB_SG
	/*Gather bulk out urbs*/
	if((ep_type == USB_ENDPOINT_XFER_BULK)&&(usb_pipeout(urb->pipe))/*bulk out ep*/){

		urb_sg_data.u32_urb_sg_num++;
		/*size+*/
		urb_sg_data.u32_urb_sg_size += urb->transfer_buffer_length;

		if(1 == urb_sg_data.u32_urb_sg_num){			
			/*add timer*/
			urb_sg_data.hsic_sg_timer.expires = jiffies + urb_sg_data.u32_hsic_sg_timeout;
			add_timer(&urb_sg_data.hsic_sg_timer);
		}
		/*Hsic stop gather pkt conduction :
		1.Gathered pkt number reach threshole
		2.Gathered pkt length reach threshole
		3.urb_no_interrupt did not set
		*/
		if((urb_sg_data.u32_urb_sg_num >= urb_sg_data.u32_hsic_sg_max_urb_num)
			||(urb_sg_data.u32_urb_sg_size >= urb_sg_data.u32_hsic_sg_max_urb_size)			
			|| !(urb->transfer_flags & URB_NO_INTERRUPT)){
			del_timer(&urb_sg_data.hsic_sg_timer); /* delete timer */
			if(urb_sg_data.u32_urb_sg_num >= urb_sg_data.u32_hsic_sg_max_urb_num){
				urb_sg_data.u32_hsic_sg_queue_full_count++;
			}
			urb_sg_data.u32_hsic_sg_send_count++;
			urb_sg_data.u32_hsic_sg_urb_count += urb_sg_data.u32_urb_sg_num;
			flags |= URB_GIVEBACK_ASAP;

			/* clear the statis */
			urb_sg_data.u32_urb_sg_num = 0;
			urb_sg_data.u32_urb_sg_size = 0;			
		}
	}
	else{
		/*bulk out only, other ep don't need sg, altought they were all been set URB_NO_INTERRUPT
		if(!(urb->transfer_flags & URB_NO_INTERRUPT))*/
		flags |= URB_GIVEBACK_ASAP;
		}
#else
	if (!(urb->transfer_flags & URB_NO_INTERRUPT))
		flags |= URB_GIVEBACK_ASAP;
#endif
	if (urb->transfer_flags & URB_ZERO_PACKET)
		flags |= URB_SEND_ZERO_PACKET;

	dwc_otg_hcd_urb_set_params(dwc_otg_urb, urb, buf,
				   urb->transfer_dma,
				   urb->transfer_buffer_length,
				   urb->setup_packet,
				   urb->setup_dma, flags, urb->interval);/*lint !e119*/

	for (i = 0; i < urb->number_of_packets; ++i) {
		dwc_otg_hcd_urb_set_iso_desc_params(dwc_otg_urb, i,
						    urb->
						    iso_frame_desc[i].offset,
						    urb->
						    iso_frame_desc[i].length);
	}

	urb->hcpriv = dwc_otg_urb;

	retval = dwc_otg_hcd_urb_enqueue(dwc_otg_hcd, dwc_otg_urb, &ep->hcpriv,
					 mem_flags == GFP_ATOMIC ? 1 : 0);
	if (!retval) {
		if (alloc_bandwidth) {
			allocate_bus_bandwidth(hcd,
					       dwc_otg_hcd_get_ep_bandwidth
					       (dwc_otg_hcd, ep->hcpriv), urb);
		}
	} else {
		if (retval == -DWC_E_NO_DEVICE) {
			retval = -ENODEV;
		}
	}

	return retval;
}

/** Aborts/cancels a USB transfer request. Always returns 0 to indicate
 * success.  */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
static int urb_dequeue(struct usb_hcd *hcd, struct urb *urb)
#else
static int urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status)
#endif
{
	dwc_irqflags_t flags;
	dwc_otg_hcd_t *dwc_otg_hcd;
	DWC_DEBUGPL(DBG_HCD, "DWC OTG HCD URB Dequeue\n");

	dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);

#ifdef DEBUG
	if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB)) {
		dump_urb_info(urb, "urb_dequeue");
	}
#endif

	DWC_SPINLOCK_IRQSAVE(dwc_otg_hcd->lock, &flags);

	dwc_otg_hcd_urb_dequeue(dwc_otg_hcd, urb->hcpriv);

	DWC_FREE(urb->hcpriv);
	urb->hcpriv = NULL;
	DWC_SPINUNLOCK_IRQRESTORE(dwc_otg_hcd->lock, flags);

	/* Higher layer software sets URB status. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
	usb_hcd_giveback_urb(hcd, urb);
#else
	usb_hcd_giveback_urb(hcd, urb, status);
#endif
	if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB)) {
		DWC_PRINTF("Called usb_hcd_giveback_urb()\n");
		DWC_PRINTF("  urb->status = %d\n", urb->status);
	}

	return 0;
}

/* Frees resources in the DWC_otg controller related to a given endpoint. Also
 * clears state in the HCD related to the endpoint. Any URBs for the endpoint
 * must already be dequeued. */
static void endpoint_disable(struct usb_hcd *hcd, struct usb_host_endpoint *ep)
{
	dwc_otg_hcd_t *dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);

	DWC_DEBUGPL(DBG_HCD,
		    "DWC OTG HCD EP DISABLE: _bEndpointAddress=0x%02x, "
		    "endpoint=%d\n", ep->desc.bEndpointAddress,
		    dwc_ep_addr_to_endpoint(ep->desc.bEndpointAddress));
	dwc_otg_hcd_endpoint_disable(dwc_otg_hcd, ep->hcpriv, 250);
	ep->hcpriv = NULL;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
/* Resets endpoint specific parameter values, in current version used to reset 
 * the data toggle(as a WA). This function can be called from usb_clear_halt routine */
static void endpoint_reset(struct usb_hcd *hcd, struct usb_host_endpoint *ep)
{
	dwc_irqflags_t flags;
	struct usb_device *udev = NULL;
	int epnum = usb_endpoint_num(&ep->desc);
	int is_out = usb_endpoint_dir_out(&ep->desc);
	int is_control = usb_endpoint_xfer_control(&ep->desc);
	dwc_otg_hcd_t *dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);
#ifdef LM_INTERFACE
	struct lm_device *_dev = dwc_otg_hcd->otg_dev->os_dep.lmdev;
#elif defined(PCI_INTERFACE)
	struct pci_dev *_dev = dwc_otg_hcd->otg_dev->os_dep.pcidev;
#endif

	if (_dev)
		udev = to_usb_device(&_dev->dev);
	else
		return;

	DWC_DEBUGPL(DBG_HCD, "DWC OTG HCD EP RESET: Endpoint Num=0x%02d\n", epnum);

	DWC_SPINLOCK_IRQSAVE(dwc_otg_hcd->lock, &flags);
	usb_settoggle(udev, epnum, is_out, 0);/*lint !e502 !e413*/
	if (is_control)
		usb_settoggle(udev, epnum, !is_out, 0);/*lint !e413*/

	if (ep->hcpriv) {
		dwc_otg_hcd_endpoint_reset(dwc_otg_hcd, ep->hcpriv);
	}
	DWC_SPINUNLOCK_IRQRESTORE(dwc_otg_hcd->lock, flags);
}
#endif

/** Handles host mode interrupts for the DWC_otg controller. Returns IRQ_NONE if
 * there was no interrupt to handle. Returns IRQ_HANDLED if there was a valid
 * interrupt.
 *
 * This function is called by the USB core when an interrupt occurs */
static irqreturn_t dwc_otg_hcd_irq(struct usb_hcd *hcd)
{
	dwc_otg_hcd_t *dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);
	int32_t retval = dwc_otg_hcd_handle_intr(dwc_otg_hcd);
	if (retval != 0) {
		S3C2410X_CLEAR_EINTPEND();
	}
	return IRQ_RETVAL(retval);/*lint !e64*/
}

/** Creates Status Change bitmap for the root hub and root port. The bitmap is
 * returned in buf. Bit 0 is the status change indicator for the root hub. Bit 1
 * is the status change indicator for the single root port. Returns 1 if either
 * change indicator is 1, otherwise returns 0. */
int hub_status_data(struct usb_hcd *hcd, char *buf)
{
	dwc_otg_hcd_t *dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);

	buf[0] = 0;
	buf[0] |= (dwc_otg_hcd_is_status_changed(dwc_otg_hcd, 1)) << 1;

	return (buf[0] != 0);
}

/** Handles hub class-specific requests. */
int hub_control(struct usb_hcd *hcd,
		u16 typeReq, u16 wValue, u16 wIndex, char *buf, u16 wLength)
{
	int retval;

	retval = dwc_otg_hcd_hub_control(hcd_to_dwc_otg_hcd(hcd),
					 typeReq, wValue, wIndex, buf, wLength);/*lint !e64*/

	switch (retval) {
	case -DWC_E_INVALID:
		retval = -EINVAL;
		break;
	}

	return retval;
}

#ifdef CONFIG_PM
static int dwc_bus_suspend(struct usb_hcd *hcd)
{
	dwc_otg_hcd_t *dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);
	dwc_irqflags_t flags;

	DWC_SPINLOCK_IRQSAVE(dwc_otg_hcd->lock, &flags);
	hcd->state = HC_STATE_SUSPENDED;
	DWC_SPINUNLOCK_IRQRESTORE(dwc_otg_hcd->lock, flags);

	return 0;
}

static int dwc_bus_resume(struct usb_hcd *hcd)
{
	dwc_otg_hcd_t *dwc_otg_hcd = hcd_to_dwc_otg_hcd(hcd);
	dwc_irqflags_t flags;

	DWC_SPINLOCK_IRQSAVE(dwc_otg_hcd->lock, &flags);
	hcd->state = HC_STATE_RUNNING;
	DWC_SPINUNLOCK_IRQRESTORE(dwc_otg_hcd->lock, flags);

	return 0;
}
#endif

#ifdef DWC2_SUPPORT_URB_SG
EXPORT_SYMBOL(hsic_sg_status);
EXPORT_SYMBOL(hsic_set_max_urb_num);
EXPORT_SYMBOL(hsic_set_sg_max_urb_size);
EXPORT_SYMBOL(hsic_set_sg_timeout);
#endif

#endif /* DWC_DEVICE_ONLY */

/*lint -restore*/

