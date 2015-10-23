#include <boot/boot.h>
#include "usb3_hw.h"
#include "usb3_drv.h"

extern void usb3_handle_dev_intr(usb3_pcd_t *pcd, uint32_t event);
extern void usb3_handle_ep_intr(usb3_pcd_t *pcd, int physep, uint32_t event);

void ena_eventbuf_intr(usb3_device_t *dev)
{
	uint32_t eventsiz;

	eventsiz =
	      dwc_readl(&dev->core_global_regs->geventbuf[0].geventsiz);
	eventsiz &= ~USB3_EVENTSIZ_INT_MSK_BIT;
	dwc_writel(eventsiz,
        &dev->core_global_regs->geventbuf[0].geventsiz);
}

void dis_eventbuf_intr(usb3_device_t *dev)
{
	uint32_t eventsiz;

	eventsiz =
	      dwc_readl(&dev->core_global_regs->geventbuf[0].geventsiz);
	eventsiz |= USB3_EVENTSIZ_INT_MSK_BIT;
	dwc_writel(eventsiz,
		 &dev->core_global_regs->geventbuf[0].geventsiz);
}

void usb3_dis_flush_eventbuf_intr(usb3_device_t *dev)
{
	uint32_t cnt;

	dis_eventbuf_intr(dev);
	cnt = dwc_readl(&dev->core_global_regs->geventbuf[0].geventcnt);
	dwc_writel(cnt, &dev->core_global_regs->geventbuf[0].geventcnt);
}

int get_eventbuf_count(usb3_device_t *dev)
{
	uint32_t cnt;

	cnt = dwc_readl(&dev->core_global_regs->geventbuf[0].geventcnt);
	return cnt & USB3_EVENTCNT_CNT_BITS;
}

void update_eventbuf_count(usb3_device_t *dev, int cnt)
{
	dwc_writel(cnt, &dev->core_global_regs->geventbuf[0].geventcnt);
}

uint32_t get_eventbuf_event(usb3_device_t *dev, int size)
{
	uint32_t event;

	event = *dev->event_ptr++;
	if (dev->event_ptr >= dev->event_buf + size)
		dev->event_ptr = dev->event_buf;
	return event;
}

void usb3_init_eventbuf(usb3_device_t *dev, int size, uint32_t dma_addr)
{
	dwc_writel(dma_addr & 0xffffffffU,
		 &dev->core_global_regs->geventbuf[0].geventadr_lo);
	dwc_writel(0, &dev->core_global_regs->geventbuf[0].geventadr_hi);
	dwc_writel(size << 2,
		 &dev->core_global_regs->geventbuf[0].geventsiz);
	dwc_writel(0, &dev->core_global_regs->geventbuf[0].geventcnt);
}

void usb3_enable_device_interrupts(usb3_device_t *dev)
{
	/* Clear any pending interrupts */
	usb3_dis_flush_eventbuf_intr(dev);

	/* Enable device interrupts */
	dwc_writel(USB3_DEVTEN_CONNDONE_BIT  | USB3_DEVTEN_USBRESET_BIT,
		 &dev->pcd.dev_global_regs->devten);
}

