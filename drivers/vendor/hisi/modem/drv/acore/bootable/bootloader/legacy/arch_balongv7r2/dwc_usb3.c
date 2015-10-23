#include <boot/boot.h>
#include <boot/usb.h>
#include <bsp_memmap.h>
#include <usb3_hw.h>
#include <usb3_drv.h>
#include <balongv7r2/dwc_usb3.h>
#include "hi_syscrg_interface.h"

extern usb3_device_t *usb3_ctx;

uint8_t string_manu[] =
    {'H',0,'u',0,'a',0,'w',0,'e',0,'i',0,',',0,' ',0,'I',0,'n',0,'c'};
uint8_t string_prod[] =
    {'B',0,'a',0,'l',0,'o',0,'n',0,'g',0,' ',0,'7',0,'.',0,'2'};

struct usb_endpoint *usb_endpoint_alloc(unsigned num, unsigned in, unsigned max_pkt)
{
    usb3_device_t *dev = usb3_ctx;
    struct usb_endpoint *ept;
    usb3_pcd_ep_t *ep;

    max_pkt = max_pkt;

    /* Allocate except ep0, max_pkt is ignored */
    if (num == 0)
        return 0;

    ept = alloc(sizeof(*ept));

    ept->num = num;
    ept->in = !!in;
    ept->req = 0;

    if (in) {
        ep = &dev->pcd.in_ep;
    } else {
        ep = &dev->pcd.out_ep;
    }

    ep->ept = ept;
    return ept;
}

struct usb_request *usb_request_alloc(unsigned bufsiz)
{
    struct usb_request *req;
    req = alloc(sizeof(*req));
    req->buf = alloc(bufsiz);
    return req;
}

int usb_queue_req(struct usb_endpoint *ept, struct usb_request *request)
{
    usb3_pcd_t *pcd = &usb3_ctx->pcd;
    usb3_pcd_ep_t *ep;
    usb3_pcd_req_t *req;

    /* Don't queue request on ep0 */
    if (ept->num == 0)
        return 1;

    if (ept->in) {
        ep = &pcd->in_ep;
    } else {
        ep = &pcd->out_ep;
    }

    req = &ep->req;
    req->length = request->length;
    req->bufdma = request->buf;
    ept->req = request;

    usb3_ep_start_transfer(pcd, ep);
    return 0;
}

void usb_init(void)
{
    usb3_device_t *usb3_dev;
    usb3_pcd_t *pcd;
    uint32_t addr_ofs = 0xc000; /* synopsys id */

    /* alloc usb3 device context */
    usb3_ctx = (usb3_device_t *)alloc(sizeof(usb3_device_t));

    /* save global context to local */
    usb3_dev = usb3_ctx;

    /* clear the global dev context */
    memset((void *)usb3_dev, 0, sizeof(usb3_device_t));

    /* initialize base variable */
    usb3_dev->base = (volatile uint8_t *)HI_USB3_REGBASE_ADDR;
    usb3_dev->dev_desc = alloc(sizeof(usb_device_descriptor_t));
    usb3_dev->string_manu = string_manu;
    usb3_dev->string_prod = string_prod;
    usb3_dev->string_manu_len = sizeof(string_manu);
    usb3_dev->string_prod_len = sizeof(string_prod);

    /* allocate descriptors */
    pcd = &usb3_dev->pcd;
    pcd->ep0_setup_desc = (usb3_dma_desc_t *)alloc(sizeof(usb3_dma_desc_t));
    pcd->ep0_in_desc = (usb3_dma_desc_t *)alloc(sizeof(usb3_dma_desc_t));
    pcd->ep0_out_desc = (usb3_dma_desc_t *)alloc(sizeof(usb3_dma_desc_t));
    pcd->in_ep.ep_desc = (usb3_dma_desc_t *)alloc(sizeof(usb3_dma_desc_t));
    pcd->out_ep.ep_desc = (usb3_dma_desc_t *)alloc(sizeof(usb3_dma_desc_t));

    /* allocate event buffer */
    usb3_dev->event_buf = (uint32_t *)alloc_page_aligned(USB3_EVENT_BUF_SIZE << 2);

    /* Release usb3.0 controller */
	hi_syscrg_usb_bc_clk_on();
    usb3_sysctrl_init();
	hi_syscrg_usb_bc_clk_off();

    /* Get usb3.0 version number */
    usb3_dev->snpsid = dwc_readl((volatile uint32_t *)
                (usb3_dev->base + addr_ofs + 0x120));
    dprintf("\r\nsnpsid: %x\n", usb3_dev->snpsid);

    /* Initialize usb3.0 core */
    usb3_common_init(usb3_dev, usb3_dev->base + addr_ofs);

    /* Must be under device mode */
    if (usb3_is_host_mode(usb3_dev)) {
        dprintf("\r\nUSB should work in device mode!\n");
        return;
    }

    /* Initialize usb3.0 pcd */
    usb3_init(usb3_dev);
}

void usb_shutdown(void)
{
    usb3_pcd_t *pcd;
    uint32_t temp;

    /* if usb not init, do nothing */
    if (!usb3_ctx) {
        return;
    }

    pcd = &usb3_ctx->pcd;

	/* Clear Run/Stop bit */
	temp = dwc_readl(&pcd->dev_global_regs->dctl);
	temp &= ~USB3_DCTL_RUN_STOP_BIT;
	dwc_writel(temp, &pcd->dev_global_regs->dctl);

    /* power down the usb */
	usb3_sysctrl_exit();
	usb3_ctx = NULL;
}

void usb_poll(void)
{
    usb3_device_t *dev = usb3_ctx;
	usb3_pcd_t *pcd = &dev->pcd;
	uint32_t event;
	int count, intr, physep, i;

	i = 1000;
    do {
    	count = get_eventbuf_count(dev);
    	if ((count & USB3_EVENTCNT_CNT_BITS) ==
    					(0xffffffff & USB3_EVENTCNT_CNT_BITS) ||
    	        count >= USB3_EVENT_BUF_SIZE * 4) {

    		update_eventbuf_count(dev, count);
    		count = 0;
    	}
    } while (i-- && !count);//!count);

	for (i = 0; i < count; i += 4) {
		event = get_eventbuf_event(dev, USB3_EVENT_BUF_SIZE);
		update_eventbuf_count(dev, 4);
		if (event == 0) {
			/* Ignore null events */
			continue;
		}

		if (event & USB3_EVENT_NON_EP_BIT) {
			intr = event & USB3_EVENT_INTTYPE_BITS;

			if (intr ==
			    (USB3_EVENT_DEV_INT << USB3_EVENT_INTTYPE_SHIFT)) {
				usb3_handle_dev_intr(pcd, event);
			} else {
				/* @todo Handle non-Device interrupts
				 * (OTG, CarKit, I2C)
				 */
			}
		} else {
			physep = (event >> USB3_DEPEVT_EPNUM_SHIFT) &
			      (USB3_DEPEVT_EPNUM_BITS >> USB3_DEPEVT_EPNUM_SHIFT);
			usb3_handle_ep_intr(pcd, physep, event);
		}
	}
}

