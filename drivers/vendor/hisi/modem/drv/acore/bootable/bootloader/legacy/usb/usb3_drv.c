#if __GNUC__ >= 4 && __GNUC_MINOR__ >=6
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "bsp_memmap.h"
#include "hi_syssc_interface.h"
#include "hi_syscrg_interface.h"
#include <boot/boot.h>
#include "usb3_hw.h"
#include "usb3_drv.h"

void usb3_handle_event(usb3_device_t *dev);
void usb3_enable_device_interrupts(usb3_device_t *dev);
void usb3_dis_flush_eventbuf_intr(usb3_device_t *dev);
void usb3_init_eventbuf(usb3_device_t *dev, int size, uint32_t dma_addr);

usb3_device_t *usb3_ctx = NULL;

uint32_t usb3_is_host_mode(usb3_device_t *dev)
{
    return dwc_readl(&dev->core_global_regs->gsts) & 0x01;
}

void pcd_epinit(usb3_pcd_t *pcd)
{
    usb3_pcd_ep_t *ep;

    /* Init EP0 */
    do {
        ep = &pcd->ep0;

        ep->pcd = pcd;
        ep->stopped = 1;
        ep->is_in = 0;
        ep->active = 0;
        ep->phys = 0;
        ep->num = 0;
        ep->tx_fifo_num = 0;
        ep->out_ep_reg = &pcd->out_ep_regs[0];
        ep->in_ep_reg = &pcd->in_ep_regs[0];

        ep->type = USB3_EP_TYPE_CONTROL;
        ep->maxburst = 0;
        ep->maxpacket = USB3_MAX_EP0_SIZE;
        ep->send_zlp = 0;

        ep->req.length = 0;
        ep->req.actual = 0;

        pcd->ep0_req.length = 0;
        pcd->ep0_req.actual = 0;
    } while(0);

    /* Init EP1-OUT */
    do {
        ep = &pcd->out_ep;

        ep->pcd = pcd;
        ep->stopped = 1;
        ep->is_in = 0;
        ep->active = 0;
        ep->phys = USB3_BULK_OUT_EP << 1;
        ep->num = 1;
        ep->tx_fifo_num = 0;
        ep->out_ep_reg = &pcd->out_ep_regs[USB3_BULK_OUT_EP];

        /* Bulk EP is activated */
        ep->type = USB3_EP_TYPE_BULK;
        ep->maxburst = 0;
        ep->maxpacket = USB3_MAX_PACKET_SIZE;
        ep->send_zlp = 0;

        ep->req.length = 0;
        ep->req.actual = 0;
    } while(0);

    /* Init EP1-IN */
    do {
        ep = &pcd->in_ep;

        ep->pcd = pcd;
        ep->stopped = 1;
        ep->is_in = 1;
        ep->active = 0;
        ep->phys = (USB3_BULK_IN_EP << 1) | 1;
        ep->num = 1;
        ep->tx_fifo_num = USB3_BULK_IN_EP;
        ep->in_ep_reg = &pcd->in_ep_regs[USB3_BULK_IN_EP];

        /* Bulk EP is activated */
        ep->type = USB3_EP_TYPE_BULK;
        ep->maxburst = 0;
        ep->maxpacket = USB3_MAX_PACKET_SIZE;
        ep->send_zlp = 0;

        ep->req.length = 0;
        ep->req.actual = 0;
    } while(0);

    pcd->ep0state = EP0_IDLE;
    pcd->ep0.maxpacket = USB3_MAX_EP0_SIZE;
    pcd->ep0.type = USB3_EP_TYPE_CONTROL;
}

void usb3_set_address(usb3_pcd_t *pcd, uint32_t addr)
{
    uint32_t dcfg;

	dcfg = dwc_readl(&pcd->dev_global_regs->dcfg);
	dcfg &= ~USB3_DCFG_DEVADDR_BITS;
	dcfg |= addr << USB3_DCFG_DEVADDR_SHIFT;
	dwc_writel(dcfg, &pcd->dev_global_regs->dcfg);
}

#define RAM_WIDTH       8
#define RAM_RX_DEPTH    4096
#define RAM_TX0_DEPTH   1024
#define RAM_TX1_DEPTH   2048
void usb3_set_tx_fifo_size(usb3_device_t *dev)
{
	usb3_core_global_regs_t *global_regs =
						dev->core_global_regs;
    uint32_t prev_start = 0;
    /* Set 1K for tx fifo0 */
	dwc_writel(((RAM_TX0_DEPTH / RAM_WIDTH) << USB3_FIFOSZ_DEPTH_SHIFT) |
		 (prev_start << USB3_FIFOSZ_STARTADDR_SHIFT),
		 &global_regs->gtxfifosiz[0]);

    prev_start += RAM_TX0_DEPTH / RAM_WIDTH;
    /* Set 2K for tx fifo1 */
	dwc_writel(((RAM_TX1_DEPTH / RAM_WIDTH) << USB3_FIFOSZ_DEPTH_SHIFT) |
		 (prev_start << USB3_FIFOSZ_STARTADDR_SHIFT),
		 &global_regs->gtxfifosiz[1]);
}

void usb3_set_rx_fifo_size(usb3_device_t *dev)
{
	usb3_core_global_regs_t *global_regs =
						dev->core_global_regs;
    /* Set 4K for rx fifo */
	dwc_writel(((RAM_RX_DEPTH / RAM_WIDTH) << USB3_FIFOSZ_DEPTH_SHIFT),
		 &global_regs->grxfifosiz[0]);
}

void usb3_resume_usb2_phy(usb3_pcd_t *pcd)
{
	uint32_t usb2phycfg;

	usb2phycfg = dwc_readl(&pcd->usb3_dev->core_global_regs->gusb2phycfg[0]);
	usb2phycfg &= ~USB3_USB2PHYCFG_SUS_PHY_BIT;
	dwc_writel(usb2phycfg, &pcd->usb3_dev->core_global_regs->gusb2phycfg[0]);
}

void usb3_resume_usb3_phy(usb3_pcd_t *pcd)
{
	uint32_t pipectl;

	pipectl = dwc_readl(&pcd->usb3_dev->core_global_regs->gusb3pipectl[0]);
	pipectl &= ~USB3_PIPECTL_SUS_PHY_BIT;
	dwc_writel(pipectl, &pcd->usb3_dev->core_global_regs->gusb3pipectl[0]);
}

void usb3_accept_u1(usb3_pcd_t *pcd)
{
	uint32_t dctl;

	dctl = dwc_readl(&pcd->dev_global_regs->dctl);
	dctl |= USB3_DCTL_ACCEPT_U1_EN_BIT;
	dwc_writel(dctl, &pcd->dev_global_regs->dctl);
}

void usb3_accept_u2(usb3_pcd_t *pcd)
{
	uint32_t dctl;

	dctl = dwc_readl(&pcd->dev_global_regs->dctl);
	dctl |= USB3_DCTL_ACCEPT_U2_EN_BIT;
	dwc_writel(dctl, &pcd->dev_global_regs->dctl);
}

void usb3_enable_u1(usb3_pcd_t *pcd)
{
	uint32_t dctl;

	dctl = dwc_readl(&pcd->dev_global_regs->dctl);
	dctl |= USB3_DCTL_INIT_U1_EN_BIT;
	dwc_writel(dctl, &pcd->dev_global_regs->dctl);
}

void usb3_enable_u2(usb3_pcd_t *pcd)
{
	uint32_t dctl;

	dctl = dwc_readl(&pcd->dev_global_regs->dctl);
	dctl |= USB3_DCTL_INIT_U2_EN_BIT;
	dwc_writel(dctl, &pcd->dev_global_regs->dctl);
}

void usb3_disable_u1(usb3_pcd_t *pcd)
{
	uint32_t dctl;

	dctl = dwc_readl(&pcd->dev_global_regs->dctl);
	dctl &= ~USB3_DCTL_INIT_U1_EN_BIT;
	dwc_writel(dctl, &pcd->dev_global_regs->dctl);
}

void usb3_disable_u2(usb3_pcd_t *pcd)
{
	uint32_t dctl;

	dctl = dwc_readl(&pcd->dev_global_regs->dctl);
	dctl &= ~USB3_DCTL_INIT_U2_EN_BIT;
	dwc_writel(dctl, &pcd->dev_global_regs->dctl);
}

uint32_t usb3_u1_enabled(usb3_pcd_t *pcd)
{
	uint32_t dctl;

	dctl = dwc_readl(&pcd->dev_global_regs->dctl);
	return !!(dctl & USB3_DCTL_INIT_U1_EN_BIT);
}

uint32_t usb3_u2_enabled(usb3_pcd_t *pcd)
{
	uint32_t dctl;

	dctl = dwc_readl(&pcd->dev_global_regs->dctl);
	return !!(dctl & USB3_DCTL_INIT_U2_EN_BIT);
}

void usb3_dep_cstall(usb3_pcd_t *pcd,
			usb3_dev_ep_regs_t *ep_reg)
{
	/* Start the command */
	dwc_writel(USB3_EPCMD_CLR_STALL | USB3_EPCMD_ACT_BIT, &ep_reg->depcmd);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, USB3_EPCMD_ACT_BIT, 0);
}

void usb3_dep_sstall(usb3_pcd_t *pcd,
			usb3_dev_ep_regs_t *ep_reg)
{
	/* Start the command */
	dwc_writel(USB3_EPCMD_SET_STALL | USB3_EPCMD_ACT_BIT, &ep_reg->depcmd);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, USB3_EPCMD_ACT_BIT, 0);
}

uint32_t handshake(usb3_device_t *dev, volatile uint32_t *ptr,
		      uint32_t mask, uint32_t done)
{
	uint32_t usec = 100000;
	uint32_t result;

	do {
		result = dwc_readl(ptr);
		if ((result & mask) == done) {
			return 1;
		}

		udelay(1);
		usec -= 1;
	} while (usec > 0);

	return 0;
}

void usb3_fill_desc(usb3_dma_desc_t *desc, uint32_t dma_addr,
			uint32_t dma_len, uint32_t stream, uint32_t type,
			uint32_t ctrlbits, int own)
{
	desc->bptl = (uint32_t)(dma_addr & 0xffffffffU);
	desc->bpth = 0;
	desc->status = dma_len << USB3_DSCSTS_XFRCNT_SHIFT;

	/* Note: If type is 0, leave original control bits intact (for isoc) */
	if (type)
		desc->control = type << USB3_DSCCTL_TRBCTL_SHIFT;

	desc->control |= (stream << USB3_DSCCTL_STRMID_SOFN_SHIFT) | ctrlbits;

	/* Must do this last! */
	if (own)
		desc->control |= USB3_DSCCTL_HWO_BIT;
}


void usb3_dep_startnewcfg(usb3_pcd_t *pcd,
			     usb3_dev_ep_regs_t *ep_reg,
			     uint32_t rsrcidx)
{
	/* Start the command */
	dwc_writel((rsrcidx << USB3_EPCMD_XFER_RSRC_IDX_SHIFT) |
		 USB3_EPCMD_START_NEW_CFG | USB3_EPCMD_ACT_BIT,
		 &ep_reg->depcmd);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, USB3_EPCMD_ACT_BIT, 0);
}

void usb3_dep_cfg(usb3_pcd_t *pcd,
		     usb3_dev_ep_regs_t *ep_reg,
		     uint32_t depcfg0, uint32_t depcfg1, uint32_t depcfg2)
{
	/* Set param 2 */
	dwc_writel(depcfg2, &ep_reg->depcmdpar2);

	/* Set param 1 */
	dwc_writel(depcfg1, &ep_reg->depcmdpar1);

	/* Set param 0 */
	dwc_writel(depcfg0, &ep_reg->depcmdpar0);

	/* Start the command */
	dwc_writel(USB3_EPCMD_SET_EP_CFG | USB3_EPCMD_ACT_BIT,
		 &ep_reg->depcmd);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, USB3_EPCMD_ACT_BIT, 0);
}

void usb3_dep_xfercfg(usb3_pcd_t *pcd,
			 usb3_dev_ep_regs_t *ep_reg,
			 uint32_t depstrmcfg)
{
	/* Set param 0 */
	dwc_writel(depstrmcfg, &ep_reg->depcmdpar0);

	/* Start the command */
	dwc_writel(USB3_EPCMD_SET_XFER_CFG | USB3_EPCMD_ACT_BIT,
		 &ep_reg->depcmd);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, USB3_EPCMD_ACT_BIT, 0);
}

uint8_t usb3_dep_startxfer(usb3_pcd_t *pcd,
			   usb3_dev_ep_regs_t *ep_reg,
			   uint32_t dma_addr, uint32_t stream_or_uf)
{
	uint32_t depcmd;

	/* Set param 1 */
	dwc_writel(dma_addr & 0xffffffffU, &ep_reg->depcmdpar1);

	/* Set param 0 */
	dwc_writel(0, &ep_reg->depcmdpar0);

	dwc_writel((stream_or_uf << USB3_EPCMD_STR_NUM_OR_UF_SHIFT) |
		 USB3_EPCMD_START_XFER | USB3_EPCMD_ACT_BIT,
		 &ep_reg->depcmd);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, USB3_EPCMD_ACT_BIT, 0);

	depcmd = dwc_readl(&ep_reg->depcmd);

	return (depcmd >> USB3_EPCMD_XFER_RSRC_IDX_SHIFT) &
	       (USB3_EPCMD_XFER_RSRC_IDX_BITS >> USB3_EPCMD_XFER_RSRC_IDX_SHIFT);
}

void usb3_dep_updatexfer(usb3_pcd_t *pcd,
			    usb3_dev_ep_regs_t *ep_reg,
			    uint32_t tri)
{
	/* Start the command */
	dwc_writel((tri << USB3_EPCMD_XFER_RSRC_IDX_SHIFT) |
		 USB3_EPCMD_UPDATE_XFER | USB3_EPCMD_ACT_BIT,
		 &ep_reg->depcmd);
    
	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, USB3_EPCMD_ACT_BIT, 0);
}

void usb3_enable_ep(usb3_pcd_t *pcd, usb3_pcd_ep_t *ep)
{
	uint32_t ep_index_num, dalepena;

	ep_index_num = ep->num * 2;

	if (ep->is_in)
		ep_index_num += 1;

	dalepena = dwc_readl(&pcd->dev_global_regs->dalepena);

	/* If we have already enabled this EP, leave it alone
	 * (shouldn't happen)
	 */
	if (dalepena & (1 << ep_index_num))
		return;

	dalepena |= 1 << ep_index_num;
	dwc_writel(dalepena, &pcd->dev_global_regs->dalepena);
	return;
}

void usb3_ep0_activate(usb3_pcd_t *pcd)
{
	uint32_t diepcfg0, doepcfg0, diepcfg1, doepcfg1;
	uint32_t diepcfg2 = 0, doepcfg2 = 0;
	usb3_dev_ep_regs_t *ep_reg;

	diepcfg0 = USB3_EP_TYPE_CONTROL << USB3_EPCFG0_EPTYPE_SHIFT;
	diepcfg1 = USB3_EPCFG1_XFER_CMPL_BIT | USB3_EPCFG1_XFER_NRDY_BIT |
		   USB3_EPCFG1_EP_DIR_BIT;

	doepcfg0 = USB3_EP_TYPE_CONTROL << USB3_EPCFG0_EPTYPE_SHIFT;
	doepcfg1 = USB3_EPCFG1_XFER_CMPL_BIT | USB3_EPCFG1_XFER_NRDY_BIT;

	/* Default to MPS of 512 (will reconfigure after ConnectDone event) */
	diepcfg0 |= 512 << USB3_EPCFG0_MPS_SHIFT;
	doepcfg0 |= 512 << USB3_EPCFG0_MPS_SHIFT;

	diepcfg0 |= pcd->ep0.tx_fifo_num << USB3_EPCFG0_TXFNUM_SHIFT;

	/* Issue "DEPCFG" command to EP0-OUT */

	ep_reg = &pcd->out_ep_regs[0];

	/* If core is version 1.09a or later */
	/* Must issue DEPSTRTNEWCFG command first */
	usb3_dep_startnewcfg(pcd, ep_reg, 0);

	usb3_dep_cfg(pcd, ep_reg, doepcfg0, doepcfg1, doepcfg2);

	/* Issue "DEPSTRMCFG" command to EP0-OUT */

	/* One stream */
	usb3_dep_xfercfg(pcd, ep_reg, 1);

	/* Issue "DEPCFG" command to EP0-IN */
	ep_reg = &pcd->in_ep_regs[0];
	usb3_dep_cfg(pcd, ep_reg, diepcfg0, diepcfg1, diepcfg2);

	/* Issue "DEPSTRMCFG" command to EP0-IN */

	/* One stream */
	usb3_dep_xfercfg(pcd, ep_reg, 1);

	pcd->ep0.active = 1;
}

void usb3_ep_activate(usb3_pcd_t *pcd, usb3_pcd_ep_t *ep)
{
	usb3_dev_ep_regs_t *ep_reg, *ep0_reg;
	uint32_t depcfg0, depcfg1, depcfg2 = 0;

	/*
	 * Get the appropriate EP registers
	 */
	if (ep->is_in)
		ep_reg = ep->in_ep_reg;
	else
		ep_reg = ep->out_ep_reg;

	/* If this is first EP enable (ie. start of a new configuration) */
	if (!pcd->eps_enabled) {
		pcd->eps_enabled = 1;

		/* NOTE: When setting a new configuration, we must issue a
		 * "DEPCFG" command to physical EP1 (logical EP0-IN) first.
		 * This resets the core's Tx FIFO mapping table
		 */
		depcfg0 = USB3_EP_TYPE_CONTROL << USB3_EPCFG0_EPTYPE_SHIFT;
		depcfg0 |= USB3_CFG_ACTION_MODIFY << USB3_EPCFG0_CFG_ACTION_SHIFT;
		depcfg1 = USB3_EPCFG1_XFER_CMPL_BIT | USB3_EPCFG1_XFER_NRDY_BIT
			| USB3_EPCFG1_EP_DIR_BIT;

		switch (pcd->speed) {
		case USB_SPEED_SUPER:
			depcfg0 |= 512 << USB3_EPCFG0_MPS_SHIFT;
			break;

		case USB_SPEED_HIGH:
		case USB_SPEED_FULL:
			depcfg0 |= 64 << USB3_EPCFG0_MPS_SHIFT;
			break;

		case USB_SPEED_LOW:
			depcfg0 |= 8 << USB3_EPCFG0_MPS_SHIFT;
			break;
		}

		ep0_reg = &pcd->in_ep_regs[0];
		usb3_dep_cfg(pcd, ep0_reg, depcfg0, depcfg1, 0);

		/* If core is version 1.09a or later */
		/* Must issue DEPSTRTNEWCFG command first */
		ep0_reg = &pcd->out_ep_regs[0];
		usb3_dep_startnewcfg(pcd, ep0_reg, 2);
	}

	/*
	 * Issue "DEPCFG" command to EP
	 */
	depcfg0 = ep->type << USB3_EPCFG0_EPTYPE_SHIFT;
	depcfg0 |= ep->maxpacket << USB3_EPCFG0_MPS_SHIFT;

	if (ep->is_in) {
		depcfg0 |= ep->tx_fifo_num << USB3_EPCFG0_TXFNUM_SHIFT;
	}

	depcfg0 |= ep->maxburst << USB3_EPCFG0_BRSTSIZ_SHIFT;
    
	depcfg1 = ep->num << USB3_EPCFG1_EP_NUM_SHIFT;
    
	if (ep->is_in)
		depcfg1 |= USB3_EPCFG1_EP_DIR_BIT;

	depcfg1 |= USB3_EPCFG1_XFER_CMPL_BIT;

	usb3_dep_cfg(pcd, ep_reg, depcfg0, depcfg1, depcfg2);

	/*
	 * Issue "DEPSTRMCFG" command to EP
	 */
    /* Setting 1 stream resource */
	usb3_dep_xfercfg(pcd, ep_reg, 1);

	/* Enable EP in DALEPENA reg */
	usb3_enable_ep(pcd, ep);

	ep->active = 1;
}

void usb3_ep0_out_start(usb3_pcd_t *pcd)
{
	usb3_dev_ep_regs_t *ep_reg;
	usb3_dma_desc_t *desc;
	uint32_t desc_dma;
	uint8_t tri;

	/* Get the SETUP packet DMA Descriptor (TRB) */
	desc = pcd->ep0_setup_desc;
    desc_dma = (uint32_t)pcd->ep0_setup_desc;

	/* DMA Descriptor setup */
	usb3_fill_desc(desc, (uint32_t)pcd->ep0_setup_pkt,
			   pcd->ep0.maxpacket,
			   0, USB3_DSCCTL_TRBCTL_SETUP, USB3_DSCCTL_IOC_BIT |
			   USB3_DSCCTL_ISP_BIT | USB3_DSCCTL_LST_BIT, 1);

	ep_reg = &pcd->out_ep_regs[0];

	/* Issue "DEPSTRTXFER" command to EP0-OUT */
	tri = usb3_dep_startxfer(pcd, ep_reg, desc_dma, 0);
	pcd->ep0.tri_out = tri;
}

void usb3_core_dev_init(usb3_device_t *dev)
{
    usb3_core_global_regs_t *global_regs = dev->core_global_regs;
    usb3_pcd_t *pcd = &dev->pcd;
    usb3_dev_global_regs_t *dev_global_regs = pcd->dev_global_regs;
    uint32_t temp_t;

    /* Soft-reset the core */
    do {
        temp_t = dwc_readl(&dev_global_regs->dctl);
        temp_t &= ~USB3_DCTL_RUN_STOP_BIT;
        temp_t |= USB3_DCTL_CSFT_RST_BIT;
        dwc_writel(temp_t, &dev_global_regs->dctl);

        do {
            mdelay(1);
            temp_t = dwc_readl(&dev_global_regs->dctl);
        } while(temp_t & USB3_DCTL_CSFT_RST_BIT);

        /* Wait for at least 3 PHY clocks */
        mdelay(1);
    } while(0);

    pcd->link_state = 0;

    /* TI PHY: Set Turnaround Time = 9 (8-bit UTMI+ / ULPI) */
    temp_t = dwc_readl(&global_regs->gusb2phycfg[0]);
    temp_t &= ~USB3_USB2PHYCFG_USB_TRD_TIM_BITS;
    temp_t |= 9 << USB3_USB2PHYCFG_USB_TRD_TIM_SHIFT;
    dwc_writel(temp_t, &global_regs->gusb2phycfg[0]);

    /* Set TX FIFO Sizes */
    usb3_set_tx_fifo_size(dev);
    /* Set RX FIFO Sizes */
    usb3_set_rx_fifo_size(dev);

    temp_t = dwc_readl(&global_regs->gctl);
    temp_t &= ~(USB3_GCTL_PRT_CAP_DIR_BITS | USB3_GCTL_SCALE_DOWN_BITS);
    temp_t |= USB3_GCTL_PRT_CAP_DEVICE << USB3_GCTL_PRT_CAP_DIR_SHIFT;
#ifdef USB3_SYNOPSYS_PHY   /* SYNOPSYS PHY */
	/* Set LFPS filter */
	dwc_writel(USB3_PIPECTL_LFPS_FILTER_BIT | (1 << USB3_PIPECTL_TX_DEMPH_SHIFT),
		 &global_regs->gusb3pipectl[0]);
#endif
    temp_t |= 0x270 << USB3_GCTL_PWR_DN_SCALE_SHIFT;
    dwc_writel(temp_t, &global_regs->gctl);

    usb3_init_eventbuf(dev, USB3_EVENT_BUF_SIZE, (uint32_t)dev->event_buf);
    dev->event_ptr = dev->event_buf;

    /* Set Speed to Super */
    temp_t = dwc_readl(&pcd->dev_global_regs->dcfg);
    temp_t &= ~(USB3_DCFG_DEVSPD_BITS << USB3_DCFG_DEVSPD_SHIFT);
#ifndef CONFIG_USB_FORCE_HIGHSPEED
    temp_t |= USB3_SPEED_SS_PHY_125MHZ_OR_250MHZ
                        << USB3_DCFG_DEVSPD_SHIFT;
#endif
    dwc_writel(temp_t, &pcd->dev_global_regs->dcfg);

    /* Set Nump */
    temp_t = dwc_readl(&pcd->dev_global_regs->dcfg);
    temp_t &= ~USB3_DCFG_NUM_RCV_BUF_BITS;
	temp_t |= 16 << USB3_DCFG_NUM_RCV_BUF_SHIFT;
	dwc_writel(temp_t, &pcd->dev_global_regs->dcfg);

    usb3_set_address(pcd, 0);

	/* Enable Phy suspend */
	usb3_resume_usb3_phy(pcd);
	usb3_resume_usb2_phy(pcd);

    /* Enable Global and Device interrupts */
    usb3_enable_device_interrupts(dev);

	/* Activate EP0 */
    usb3_ep0_activate(pcd);

	/* Start EP0 to receive SETUP packets */
    usb3_ep0_out_start(pcd);

	/* Enable EP0-OUT/IN in DALEPENA register */
	dwc_writel(3, &pcd->dev_global_regs->dalepena);

	/* Set Run/Stop bit */
	temp_t = dwc_readl(&pcd->dev_global_regs->dctl);
	temp_t |= USB3_DCTL_RUN_STOP_BIT;
	dwc_writel(temp_t, &pcd->dev_global_regs->dctl);
}

void usb3_pcd_init(usb3_device_t *dev)
{
    usb3_pcd_t *pcd = &dev->pcd;

    pcd->usb3_dev = dev;
    pcd->speed = 0;   //unknown

    /* Initialize EP structures */
    pcd_epinit(pcd);

    /* Initialize the Core (also enables interrupts and sets Run/Stop bit) */
    usb3_core_dev_init(dev);
}

void usb3_common_init(usb3_device_t *dev, volatile uint8_t *base)
{
    usb3_pcd_t *pcd;
    
    dev->core_global_regs = (usb3_core_global_regs_t *)(base + USB3_CORE_GLOBAL_REG_OFFSET);

    pcd = &dev->pcd;

    pcd->dev_global_regs = (usb3_dev_global_regs_t *)(base + USB3_DEV_GLOBAL_REG_OFFSET);
    pcd->out_ep_regs = (usb3_dev_ep_regs_t *)(base + USB3_DEV_OUT_EP_REG_OFFSET);
    pcd->in_ep_regs = (usb3_dev_ep_regs_t *)(base + USB3_DEV_IN_EP_REG_OFFSET);
}

void usb3_init(usb3_device_t *dev)
{
    /* Init the PCD (also enables interrupts and sets Run/Stop bit) */
    usb3_pcd_init(dev);
}
void bc_set_soft_mode(void)
{
    uint32_t reg;

    reg = readl(HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x0);
    reg &= ~0x3;
    reg |= 0x1;
    writel(reg, HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x0);
    return;
}

void bc_set_bypass_mode(void)
{
    uint32_t reg;

    reg = readl(HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x0);
    reg &= ~0x03;
    reg |=  0x02;
    writel(reg, HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x0);

    return;
}

void usb3_sysctrl_init(void)
{
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
    hi_syssc_usb_iddig(1);
#endif

    hi_syssc_usb_txpreempamptune(3);

    hi_syssc_usb_phy3_init();

    /* set bc_mode to bypass */
    /* writel(2, HI_BC_CTRL_REGBASE_ADDR_VIRT + 0x0); */
    bc_set_bypass_mode();

    /* release controller and PHY */
    hi_syscrg_usb_release();

    mdelay(10);
}

void usb3_sysctrl_exit(void)
{
    /* enable the usb-bc clock */
    hi_syscrg_usb_bc_clk_on();
    /* reset controller and phy */
    hi_syscrg_usb_reset();

    /* reset vbusvldext and override_en */
    hi_syssc_usb_override(0);
    hi_syssc_usb_vbusvldext(0);

    /* reset ref_ssp_en */
    hi_syssc_usb_ref_ssp(0);

    /* reset bc to soft mode */
    bc_set_soft_mode();

    /* power down the usb2.0 and usb3.0 phy */
    hi_syssc_usb_powerdown_hsp(1);
    hi_syssc_usb_powerdown_ssp(1);

    hi_syscrg_usb_bc_clk_off();
    mdelay(2);
}


