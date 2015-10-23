/*lint -save -e26 -e42 -e64 --e{402} -e438 -e564 -e572 -e665 -e413 -e527 -e537 --e{529} --e{530} --e{533}-e539 -e548 
-e613 -e648 -e666 -e701 -e702 -e713 -e718 -e732 -e734 -e737 -e746 --e{752} -e762 --e{830} -e958*/

/**
 * gadget.c - DesignWare USB3 DRD Controller Gadget Framework Link
 *
 * Copyright (C) 2010-2011 Texas Instruments Incorporated - http://www.ti.com
 *
 * Authors: Felipe Balbi <balbi@ti.com>,
 *	    Sebastian Andrzej Siewior <bigeasy@linutronix.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the above-listed copyright holders may not be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2, as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/list.h>
#include <linux/dma-mapping.h>

#include <linux/usb/otg.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>

#include "product_config.h"
#include "core.h"
#include "gadget.h"
#include "io.h"
#include "bsp_usb.h"
#include "../gadget/usb_vendor.h"
#include "hi_syssc_interface.h"
#include "bsp_hardtimer.h"

/* added by wzs in 2013.2.6 start */
//#define DWC3_TRB_TRACE
#ifdef DWC3_TRB_TRACE
#define DWC3_TRB_TRACE_EP_NUM       (3)
#define DWC3_TRB_TRACE_RECORD_NUM   (128)
#define DWC3_TRB_TRACE_RECORD_MSK   (DWC3_TRB_TRACE_RECORD_NUM-1)

typedef struct
{
    u32 free;
    u32 busy;
    u32 sgs;
    u32 flag;
    u32 line;
}DWC3_TRB_TRACE_NODE_S;

typedef struct
{
    DWC3_TRB_TRACE_NODE_S node[DWC3_TRB_TRACE_RECORD_NUM];
    u32 pos;
}DWC3_TRB_TRACE_RECORD_S;

DWC3_TRB_TRACE_RECORD_S g_trb_trace = {0};
#endif

static struct dwc3* the_dwc3 = NULL;
static int g_dwc_phy_auto_pwrdown = 0;

static void dwc3_disable_phy_by_speed(struct dwc3 * dwc);
static void dwc3_enable_both_phy(void);
void dwc3_intr_disable(struct dwc3 *dwc);
void dwc3_intr_enable(struct dwc3 *dwc);

static inline void dwc3_trb_trace_store(struct dwc3_ep *dep, u32 sgs, u32 line)
{
#ifdef DWC3_TRB_TRACE
    if(DWC3_TRB_TRACE_EP_NUM == dep->number)
    {
        g_trb_trace.node[g_trb_trace.pos].free = dep->free_slot;
        g_trb_trace.node[g_trb_trace.pos].busy = dep->busy_slot;
        g_trb_trace.node[g_trb_trace.pos].sgs = sgs;
        g_trb_trace.node[g_trb_trace.pos].line = line;
        g_trb_trace.node[g_trb_trace.pos].flag = dep->flags;
        g_trb_trace.pos++;
        g_trb_trace.pos &= DWC3_TRB_TRACE_RECORD_MSK;
    }
#endif
}

void dwc3_trb_trace_show(void)
{
#ifdef DWC3_TRB_TRACE
    u32 i;

    printk("|-+dwc3_trb_trace_show    :\n");
    printk("| |--cur pos              :%d\n", g_trb_trace.pos);
    for(i=0; i<DWC3_TRB_TRACE_RECORD_NUM; i++)
    {
        printk("%d %d %d 0x%x %d\n",g_trb_trace.node[i].free,
                    g_trb_trace.node[i].busy,
                    g_trb_trace.node[i].sgs,
                    g_trb_trace.node[i].flag,
                    g_trb_trace.node[i].line);
    }
    printk("\n");
#endif
}
/* added by wzs in 2013.2.6 end */

/**
 * dwc3_gadget_set_test_mode - Enables USB2 Test Modes
 * @dwc: pointer to our context structure
 * @mode: the mode to set (J, K SE0 NAK, Force Enable)
 *
 * Caller should take care of locking. This function will
 * return 0 on success or -EINVAL if wrong Test Selector
 * is passed
 */
int dwc3_gadget_set_test_mode(struct dwc3 *dwc, int mode)
{
	u32		reg;

	reg = dwc3_readl(dwc->regs, DWC3_DCTL);
	reg &= ~DWC3_DCTL_TSTCTRL_MASK;

	switch (mode) {
	case TEST_J:
	case TEST_K:
	case TEST_SE0_NAK:
	case TEST_PACKET:
	case TEST_FORCE_EN:
		reg |= mode << 1;
		break;
	default:
		return -EINVAL;
	}

	dwc3_writel(dwc->regs, DWC3_DCTL, reg);

	return 0;
}

/**
 * dwc3_gadget_set_link_state - Sets USB Link to a particular State
 * @dwc: pointer to our context structure
 * @state: the state to put link into
 *
 * Caller should take care of locking. This function will
 * return 0 on success or -ETIMEDOUT.
 */
int dwc3_gadget_set_link_state(struct dwc3 *dwc, enum dwc3_link_state state)
{
	int		retries = 10000;
	u32		reg;

	/*
	 * Wait until device controller is ready. Only applies to 1.94a and
	 * later RTL.
	 */
	if (dwc->revision >= DWC3_REVISION_194A) {
		while (--retries) {
			reg = dwc3_readl(dwc->regs, DWC3_DSTS);
			if (reg & DWC3_DSTS_DCNRD)
				udelay(5);
			else
				break;
		}

		if (retries <= 0)
			return -ETIMEDOUT;
	}

	reg = dwc3_readl(dwc->regs, DWC3_DCTL);
	reg &= ~DWC3_DCTL_ULSTCHNGREQ_MASK;

	/* set requested state */
	reg |= DWC3_DCTL_ULSTCHNGREQ(state);
	dwc3_writel(dwc->regs, DWC3_DCTL, reg);

	/*
	 * The following code is racy when called from dwc3_gadget_wakeup,
	 * and is not needed, at least on newer versions
	 */
	if (dwc->revision >= DWC3_REVISION_194A)
		return 0;

	/* wait for a change in DSTS */
	retries = 10000;
	while (--retries) {
		reg = dwc3_readl(dwc->regs, DWC3_DSTS);

		if (DWC3_DSTS_USBLNKST(reg) == state)
			return 0;

		udelay(5);
	}

	dev_vdbg(dwc->dev, "link state change request timed out\n");

	return -ETIMEDOUT;
}
/**
 * dwc3_gadget_get_link_state - Gets USB Link State
 * @dwc: pointer to our context structure
 *
 * Return:
 * state: the state to put link into
 *
 */
static inline enum dwc3_link_state dwc3_gadget_get_link_state(struct dwc3 *dwc)
{
    enum dwc3_link_state	state;
	u32			reg;

	reg = dwc3_readl(dwc->regs, DWC3_DSTS);
	state = (enum dwc3_link_state)DWC3_DSTS_USBLNKST(reg);

    return state;
}
/**
 * dwc3_gadget_resize_tx_fifos - reallocate fifo spaces for current use-case
 * @dwc: pointer to our context structure
 *
 * This function will a best effort FIFO allocation in order
 * to improve FIFO usage and throughput, while still allowing
 * us to enable as many endpoints as possible.
 *
 * Keep in mind that this operation will be highly dependent
 * on the configured size for RAM1 - which contains TxFifo -,
 * the amount of endpoints enabled on coreConsultant tool, and
 * the width of the Master Bus.
 *
 * In the ideal world, we would always be able to satisfy the
 * following equation:
 *
 * ((512 + 2 * MDWIDTH-Bytes) + (Number of IN Endpoints - 1) * \
 * (3 * (1024 + MDWIDTH-Bytes) + MDWIDTH-Bytes)) / MDWIDTH-Bytes
 *
 * Unfortunately, due to many variables that's not always the case.
 */
int dwc3_gadget_resize_tx_fifos(struct dwc3 *dwc)
{
	int		last_fifo_depth = 0;
	int		fifo_size;
	int		mdwidth;
	int		num;

	if (!dwc->needs_fifo_resize)
		return 0;

	mdwidth = DWC3_MDWIDTH(dwc->hwparams.hwparams0);

	/* MDWIDTH is represented in bits, we need it in bytes */
	mdwidth >>= 3;

	/*
	 * FIXME For now we will only allocate 1 wMaxPacketSize space
	 * for each enabled endpoint, later patches will come to
	 * improve this algorithm so that we better use the internal
	 * FIFO space
	 */
	for (num = 0; num < DWC3_ENDPOINTS_NUM; num++) {
		struct dwc3_ep	*dep = dwc->eps[num];
		int		fifo_number = dep->number >> 1;
		int		mult = 1;
		int		tmp;

		if (!(dep->number & 1))
			continue;

		if (!(dep->flags & DWC3_EP_ENABLED))
			continue;

		if (usb_endpoint_xfer_bulk(dep->endpoint.desc)
				|| usb_endpoint_xfer_isoc(dep->endpoint.desc))
			mult = 3;

		/*
		 * REVISIT: the following assumes we will always have enough
		 * space available on the FIFO RAM for all possible use cases.
		 * Make sure that's true somehow and change FIFO allocation
		 * accordingly.
		 *
		 * If we have Bulk or Isochronous endpoints, we want
		 * them to be able to be very, very fast. So we're giving
		 * those endpoints a fifo_size which is enough for 3 full
		 * packets
		 */
		tmp = mult * (dep->endpoint.maxpacket + mdwidth);
		tmp += mdwidth;

		fifo_size = DIV_ROUND_UP(tmp, mdwidth);

		fifo_size |= (last_fifo_depth << 16);

		dev_vdbg(dwc->dev, "%s: Fifo Addr %04x Size %d\n",
				dep->name, last_fifo_depth, fifo_size & 0xffff);

		dwc3_writel(dwc->regs, DWC3_GTXFIFOSIZ(fifo_number),
				fifo_size);

		last_fifo_depth += (fifo_size & 0xffff);
	}

	return 0;
}

void dwc3_gadget_giveback(struct dwc3_ep *dep, struct dwc3_request *req,
		int status)
{
	struct dwc3			*dwc = dep->dwc;

	if (req->queued) {
		if (req->request.num_mapped_sgs)
			dep->busy_slot += req->request.num_mapped_sgs;
		else
			dep->busy_slot++;

        /* BUGFIX: ZLP refresh trb on complete scene */
        if (req->request.zero)
            dep->busy_slot++;

		/*
		 * Skip LINK TRB. We can't use req->trb and check for
		 * DWC3_TRBCTL_LINK_TRB because it points the TRB we just
		 * completed (not the LINK TRB).
		 */
		if (((dep->busy_slot & DWC3_TRB_MASK) == DWC3_TRB_NUM - 1) &&
				usb_endpoint_xfer_isoc(dep->endpoint.desc))
			dep->busy_slot++;

        /* added by wzs in 2013.1.16 start */
        req->queued = false;    /* let busy and free slot sync */
        /* added by wzs in 2013.1.16 end */
	}
	list_del(&req->list);
	req->trb = NULL;
    /* added by wzs in 2013.1.16 start */
    req->request.zero = 0;  /* let zero to default */
    /* added by wzs in 2013.1.16 end */

	if (req->request.status == -EINPROGRESS)
		req->request.status = status;

	if (dwc->ep0_bounced && dep->number == 0)
		dwc->ep0_bounced = false;
	else
	{
	#if 1
        usb_gadget_unmap_request(&dwc->gadget, &req->request,
			req->direction);
	#endif
	}

	DWC3_TRACE(("request %p from %s completed %d/%d ===> %d\n",
			req, dep->name, req->request.actual,
			req->request.length, status));

	spin_unlock(&dwc->lock);
	req->request.complete(&dep->endpoint, &req->request);
	spin_lock(&dwc->lock);
}

static const char *dwc3_gadget_ep_cmd_string(u8 cmd)
{
	switch (cmd) {
	case DWC3_DEPCMD_DEPSTARTCFG:
		return "Start New Configuration";
	case DWC3_DEPCMD_ENDTRANSFER:
		return "End Transfer";
	case DWC3_DEPCMD_UPDATETRANSFER:
		return "Update Transfer";
	case DWC3_DEPCMD_STARTTRANSFER:
		return "Start Transfer";
	case DWC3_DEPCMD_CLEARSTALL:
		return "Clear Stall";
	case DWC3_DEPCMD_SETSTALL:
		return "Set Stall";
	case DWC3_DEPCMD_GETEPSTATE:
		return "Get Endpoint State";
	case DWC3_DEPCMD_SETTRANSFRESOURCE:
		return "Set Endpoint Transfer Resource";
	case DWC3_DEPCMD_SETEPCONFIG:
		return "Set Endpoint Configuration";
	default:
		return "UNKNOWN command";
	}
}

int dwc3_send_gadget_generic_command(struct dwc3 *dwc, int cmd, u32 param)
{
	u32		timeout = 500;
	u32		reg;

	dwc3_writel(dwc->regs, DWC3_DGCMDPAR, param);
	dwc3_writel(dwc->regs, DWC3_DGCMD, cmd | DWC3_DGCMD_CMDACT);

	do {
		reg = dwc3_readl(dwc->regs, DWC3_DGCMD);
		if (!(reg & DWC3_DGCMD_CMDACT)) {
			DWC3_TRACE(("Command Complete --> %d\n",
					DWC3_DGCMD_STATUS(reg)));
			return 0;
		}

		/*
		 * We can't sleep here, because it's also called from
		 * interrupt context.
		 */
		timeout--;
		if (!timeout)
		{
            dwc->stat.cmd_err++;
            DWC3_ERR(("Command Timeout --> %d\n",
					DWC3_DGCMD_STATUS(reg)));
			return -ETIMEDOUT;
		}
		udelay(1);
	} while (1);
}

int dwc3_send_gadget_ep_cmd(struct dwc3 *dwc, unsigned ep,
		unsigned cmd, struct dwc3_gadget_ep_cmd_params *params)
{
    #ifdef DEBUG
	struct dwc3_ep		*dep = dwc->eps[ep];
    #endif
	u32			timeout = 500;
	u32			reg;

	DWC3_TRACE(("%s: cmd '%s'(%08x) params %08x %08x %08x\n",
			dep->name,
			dwc3_gadget_ep_cmd_string(cmd), cmd, params->param0,
			params->param1, params->param2));

	dwc3_writel(dwc->regs, DWC3_DEPCMDPAR0(ep), params->param0);
	dwc3_writel(dwc->regs, DWC3_DEPCMDPAR1(ep), params->param1);
	dwc3_writel(dwc->regs, DWC3_DEPCMDPAR2(ep), params->param2);

#if 0
    if(((DWC3_LINK_STATE_U0<<18) !=
        (dwc3_readl(dwc->regs, DWC3_DSTS)&DWC3_DSTS_USBLNKST_MASK))
        &&(DWC3_DEPCMD_STARTTRANSFER==(DWC3_DEPCMD(ep)&0xf)))
    {
        WARN_ON(1);
    }
#endif

	dwc3_writel(dwc->regs, DWC3_DEPCMD(ep), cmd | DWC3_DEPCMD_CMDACT);
	do {
		reg = dwc3_readl(dwc->regs, DWC3_DEPCMD(ep));
		if (!(reg & DWC3_DEPCMD_CMDACT)) {
			DWC3_TRACE(("Command Complete --> %d\n",
					DWC3_DEPCMD_STATUS(reg)));
			return 0;
		}

		/*
		 * We can't sleep here, because it is also called from
		 * interrupt context.
		 */
		timeout--;
		if (!timeout)
			return -ETIMEDOUT;

		udelay(1);
	} while (1);
}

static dma_addr_t dwc3_trb_dma_offset(struct dwc3_ep *dep,
		struct dwc3_trb *trb)
{
	u32		offset = (char *) trb - (char *) dep->trb_pool;

	return dep->trb_pool_dma + offset;
}

static int dwc3_alloc_trb_pool(struct dwc3_ep *dep)
{
	struct dwc3		*dwc = dep->dwc;

	if (dep->trb_pool)
		return 0;

	if (dep->number == 0 || dep->number == 1)
		return 0;

    BUILD_BUG_ON_NOT_POWER_OF_2(DWC3_TRB_NUM);

	dep->trb_pool = dma_alloc_coherent(dwc->dev,
			sizeof(struct dwc3_trb) * DWC3_TRB_NUM,
			&dep->trb_pool_dma, GFP_KERNEL);
	if (!dep->trb_pool) {
		dev_err(dep->dwc->dev, "failed to allocate trb pool for %s\n",
				dep->name);
		return -ENOMEM;
	}

	return 0;
}

static void dwc3_free_trb_pool(struct dwc3_ep *dep)
{
	struct dwc3		*dwc = dep->dwc;

    /* ep0 trb_pool not in here, we must check the pointer */
    if (dep->trb_pool) {
    	dma_free_coherent(dwc->dev, sizeof(struct dwc3_trb) * DWC3_TRB_NUM,
    			dep->trb_pool, dep->trb_pool_dma);
    }
	dep->trb_pool = NULL;
	dep->trb_pool_dma = 0;
}

#ifdef CONFIG_USB_OTG_DWC
static int dwc3_xmit_host_role_request(struct dwc3 *dwc, u32 param)
{
    return dwc3_send_gadget_generic_command(dwc,
                DWC3_DGCMD_XMIT_HOST_ROLE_REQUEST, param);
}
#endif

static int dwc3_gadget_start_config(struct dwc3 *dwc, struct dwc3_ep *dep)
{
	struct dwc3_gadget_ep_cmd_params params;
	u32			cmd;

	memset(&params, 0x00, sizeof(params));

	if (dep->number != 1) {
		cmd = DWC3_DEPCMD_DEPSTARTCFG;
		/* XferRscIdx == 0 for ep0 and 2 for the remaining */
		if (dep->number > 1) {
			if (dwc->start_config_issued)
				return 0;
			dwc->start_config_issued = true;
			cmd |= DWC3_DEPCMD_PARAM(2);
		}

		return dwc3_send_gadget_ep_cmd(dwc, 0, cmd, &params);
	}

	return 0;
}

static int dwc3_gadget_set_ep_config(struct dwc3 *dwc, struct dwc3_ep *dep,
		const struct usb_endpoint_descriptor *desc,
		const struct usb_ss_ep_comp_descriptor *comp_desc,
		bool ignore)
{
	struct dwc3_gadget_ep_cmd_params params;

	memset(&params, 0x00, sizeof(params));

	params.param0 = DWC3_DEPCFG_EP_TYPE(usb_endpoint_type(desc))
		| DWC3_DEPCFG_MAX_PACKET_SIZE(usb_endpoint_maxp(desc));

	/* Burst size is only needed in SuperSpeed mode */
	if (dwc->gadget.speed == USB_SPEED_SUPER) {
		u32 burst = dep->endpoint.maxburst;

		params.param0 |= DWC3_DEPCFG_BURST_SIZE(burst);
	}

	if (ignore)
		params.param0 |= DWC3_DEPCFG_IGN_SEQ_NUM;/*lint !e648*/

	params.param1 = DWC3_DEPCFG_XFER_COMPLETE_EN
		| DWC3_DEPCFG_XFER_NOT_READY_EN;

    if(dep->endpoint.masknotready)
    {
    	params.param1 &= ~DWC3_DEPCFG_XFER_NOT_READY_EN;
    }

	if (usb_ss_max_streams(comp_desc) && usb_endpoint_xfer_bulk(desc)) {
		params.param1 |= DWC3_DEPCFG_STREAM_CAPABLE
			| DWC3_DEPCFG_STREAM_EVENT_EN;
		dep->stream_capable = true;
	}

	if (usb_endpoint_xfer_isoc(desc))
		params.param1 |= DWC3_DEPCFG_XFER_IN_PROGRESS_EN;

    if(dep->endpoint.enable_xfer_in_progress)
    {/* enable the XFERINPROGRESS intr, such as out endpoint of rndis/ecm */
    	params.param1 |= DWC3_DEPCFG_XFER_IN_PROGRESS_EN;
    }

	/*
	 * We are doing 1:1 mapping for endpoints, meaning
	 * Physical Endpoints 2 maps to Logical Endpoint 2 and
	 * so on. We consider the direction bit as part of the physical
	 * endpoint number. So USB endpoint 0x81 is 0x03.
	 */
	params.param1 |= DWC3_DEPCFG_EP_NUMBER(dep->number);

	/*
	 * We must use the lower 16 TX FIFOs even though
	 * HW might have more
	 */
	if (dep->direction)
		params.param0 |= DWC3_DEPCFG_FIFO_NUMBER(dep->number >> 1);

	if (desc->bInterval) {
		params.param1 |= DWC3_DEPCFG_BINTERVAL_M1(desc->bInterval - 1);
		dep->interval = 1 << (desc->bInterval - 1);
	}

	return dwc3_send_gadget_ep_cmd(dwc, dep->number,
			DWC3_DEPCMD_SETEPCONFIG, &params);
}

static int dwc3_gadget_set_xfer_resource(struct dwc3 *dwc, struct dwc3_ep *dep)
{
	struct dwc3_gadget_ep_cmd_params params;

	memset(&params, 0x00, sizeof(params));

	params.param0 = DWC3_DEPXFERCFG_NUM_XFER_RES(1);

	return dwc3_send_gadget_ep_cmd(dwc, dep->number,
			DWC3_DEPCMD_SETTRANSFRESOURCE, &params);
}

static void dwc3_stop_active_transfer(struct dwc3 *dwc, u32 epnum);
static void dwc3_remove_queued(struct dwc3 *dwc, struct dwc3_ep *dep)
{
	struct dwc3_request		*req;

	if (!list_empty(&dep->req_queued)) {
		dwc3_stop_active_transfer(dwc, dep->number);

		/* - giveback all requests to gadget driver */
		while (!list_empty(&dep->req_queued)) {
			/* coverity[returned_null] */
			req = next_request(&dep->req_queued);

			dwc3_gadget_giveback(dep, req, -ESHUTDOWN);
		}
	}
}

static inline bool __dwc3_gadget_ep_kick(struct dwc3_ep *dep, u32 kicksource);
void dwc3_gadget_ep_xfercmd_monitor(unsigned long data)
{
    struct dwc3_ep *dep = (struct dwc3_ep *)data;
    struct dwc3	*dwc = dep->dwc;
    unsigned long flags;

    dep->stat.monitor_timeout++;

    spin_lock_irqsave(&dwc->lock, flags);

    dwc3_remove_queued(dwc, dep);

    if(!list_empty_careful(&dep->request_list)){
        (void)__dwc3_gadget_ep_kick(dep, DWC3_EP_KICKSOURCE_MONITOR);
    }

    spin_unlock_irqrestore(&dwc->lock, flags);
}

/**
 * __dwc3_gadget_ep_enable - Initializes a HW endpoint
 * @dep: endpoint to be initialized
 * @desc: USB Endpoint Descriptor
 *
 * Caller should take care of locking
 */
static int __dwc3_gadget_ep_enable(struct dwc3_ep *dep,
		const struct usb_endpoint_descriptor *desc,
		const struct usb_ss_ep_comp_descriptor *comp_desc,
		bool ignore)
{
	struct dwc3		*dwc = dep->dwc;
	u32			reg;
	int			ret = -ENOMEM;

    dep->stat.enable_cnt++;

	if (!(dep->flags & DWC3_EP_ENABLED)) {
		ret = dwc3_gadget_start_config(dwc, dep);
		if (ret)
			return ret;
	}

	ret = dwc3_gadget_set_ep_config(dwc, dep, desc, comp_desc, ignore);
	if (ret)
		return ret;

	if (!(dep->flags & DWC3_EP_ENABLED)) {
		struct dwc3_trb	*trb_st_hw;
		struct dwc3_trb	*trb_link;

		ret = dwc3_gadget_set_xfer_resource(dwc, dep);
		if (ret)
			return ret;

		dep->endpoint.desc = desc;
		dep->comp_desc = comp_desc;
		dep->type = usb_endpoint_type(desc);
		dep->flags |= DWC3_EP_ENABLED;

		reg = dwc3_readl(dwc->regs, DWC3_DALEPENA);
		reg |= DWC3_DALEPENA_EP(dep->number);
		dwc3_writel(dwc->regs, DWC3_DALEPENA, reg);

		if (!usb_endpoint_xfer_isoc(desc))
			return 0;

		memset(&trb_link, 0, sizeof(trb_link));

		/* Link TRB for ISOC. The HWO bit is never reset */
		trb_st_hw = &dep->trb_pool[0];

		trb_link = &dep->trb_pool[DWC3_TRB_NUM - 1];

		trb_link->bpl = lower_32_bits(dwc3_trb_dma_offset(dep, trb_st_hw));
		trb_link->bph = upper_32_bits(dwc3_trb_dma_offset(dep, trb_st_hw));
		trb_link->ctrl |= DWC3_TRBCTL_LINK_TRB;
		trb_link->ctrl |= DWC3_TRB_CTRL_HWO;
	}

	return 0;
}

static void dwc3_stop_active_transfer(struct dwc3 *dwc, u32 epnum);
static void dwc3_remove_requests(struct dwc3 *dwc, struct dwc3_ep *dep)
{
	struct dwc3_request		*req;

	if (!list_empty(&dep->req_queued)) {
		dwc3_stop_active_transfer(dwc, dep->number);

		/* - giveback all requests to gadget driver */
		while (!list_empty(&dep->req_queued)) {
			/* coverity[returned_null] */
			req = next_request(&dep->req_queued);

			dwc3_gadget_giveback(dep, req, -ESHUTDOWN);
		}
	}

	while (!list_empty(&dep->request_list)) {
		/* coverity[returned_null] */
		req = next_request(&dep->request_list);

		dwc3_gadget_giveback(dep, req, -ESHUTDOWN);
	}
}

/**
 * __dwc3_gadget_ep_disable - Disables a HW endpoint
 * @dep: the endpoint to disable
 *
 * This function also removes requests which are currently processed ny the
 * hardware and those which are not yet scheduled.
 * Caller should take care of locking.
 */
static int __dwc3_gadget_ep_disable(struct dwc3_ep *dep)
{
	struct dwc3		*dwc = dep->dwc;
	u32			reg;

    dep->stat.disable_cnt++;

    if(dep->endpoint.xfer_cmd_monitor && dep->timer)
    {
        dep->stat.monitor_disable++;
        (void)del_timer(dep->timer);
        kfree(dep->timer);
        dep->timer = NULL;
    }

	dwc3_remove_requests(dwc, dep);

	reg = dwc3_readl(dwc->regs, DWC3_DALEPENA);
	reg &= ~DWC3_DALEPENA_EP(dep->number);/*lint !e502*/
	dwc3_writel(dwc->regs, DWC3_DALEPENA, reg);

	dep->stream_capable = false;
    /*
    dep->endpoint.desc = NULL;
	dep->comp_desc = NULL;
    */
	dep->type = 0;
	dep->flags = 0;

	return 0;
}

/* -------------------------------------------------------------------------- */

static int dwc3_gadget_ep0_enable(struct usb_ep *ep,
		const struct usb_endpoint_descriptor *desc)
{
	return -EINVAL;
}

static int dwc3_gadget_ep0_disable(struct usb_ep *ep)
{
	return -EINVAL;
}

/* -------------------------------------------------------------------------- */

static int dwc3_gadget_ep_enable(struct usb_ep *ep,
		const struct usb_endpoint_descriptor *desc)
{
	struct dwc3_ep			*dep;
	struct dwc3			*dwc;
	unsigned long			flags;
	int				ret;

	if (!ep || !desc || desc->bDescriptorType != USB_DT_ENDPOINT) {
		pr_debug("dwc3: invalid parameters\n");
		return -EINVAL;
	}

	if (!desc->wMaxPacketSize) {
		pr_debug("dwc3: missing wMaxPacketSize\n");
		return -EINVAL;
	}

	dep = to_dwc3_ep(ep);
	dwc = dep->dwc;

	if (dep->flags & DWC3_EP_ENABLED) {
		dev_WARN_ONCE(dwc->dev, true, "%s is already enabled\n",
				dep->name);
		return 0;
	}

	switch (usb_endpoint_type(desc)) {
	case USB_ENDPOINT_XFER_CONTROL:
		strlcat(dep->name, "-control", sizeof(dep->name));
		break;
	case USB_ENDPOINT_XFER_ISOC:
		strlcat(dep->name, "-isoc", sizeof(dep->name));
		break;
	case USB_ENDPOINT_XFER_BULK:
		strlcat(dep->name, "-bulk", sizeof(dep->name));
		break;
	case USB_ENDPOINT_XFER_INT:
		strlcat(dep->name, "-int", sizeof(dep->name));
		break;
	default:
		dev_err(dwc->dev, "invalid endpoint transfer type\n");
	}

	USB_DBG_DWC3(dwc->dev, "Enabling %s\n", dep->name);

	spin_lock_irqsave(&dwc->lock, flags);
	ret = __dwc3_gadget_ep_enable(dep, desc, ep->comp_desc, false);
	spin_unlock_irqrestore(&dwc->lock, flags);

    if(dep->endpoint.xfer_cmd_monitor){
        if(!dep->timer){
            dep->timer = (struct timer_list *)kzalloc(sizeof(struct timer_list), GFP_ATOMIC);
            if(dep->timer){
                dep->stat.monitor_enable++;
                init_timer(dep->timer);
                dep->timer->function = dwc3_gadget_ep_xfercmd_monitor;
                dep->timer->data = (unsigned long)dep;
            }
        }
    }

	return ret;
}

static int dwc3_gadget_ep_disable(struct usb_ep *ep)
{
	struct dwc3_ep			*dep;
	struct dwc3			*dwc;
	unsigned long			flags;
	int				ret;

	if (!ep) {
		pr_debug("dwc3: invalid parameters\n");
		return -EINVAL;
	}

	dep = to_dwc3_ep(ep);
	dwc = dep->dwc;

	if (!(dep->flags & DWC3_EP_ENABLED)) {
		dev_WARN_ONCE(dwc->dev, true, "%s is already disabled\n",
				dep->name);
		return 0;
	}

	snprintf(dep->name, sizeof(dep->name), "ep%d%s",
			dep->number >> 1,
			(dep->number & 1) ? "in" : "out");

	spin_lock_irqsave(&dwc->lock, flags);
	ret = __dwc3_gadget_ep_disable(dep);
	spin_unlock_irqrestore(&dwc->lock, flags);

	return ret;
}

static struct usb_request *dwc3_gadget_ep_alloc_request(struct usb_ep *ep,
	gfp_t gfp_flags)
{
	struct dwc3_request		*req;
	struct dwc3_ep			*dep = to_dwc3_ep(ep);
	struct dwc3			*dwc = dep->dwc;

	req = kzalloc(sizeof(*req), gfp_flags);
	if (!req) {
		dev_err(dwc->dev, "not enough memory\n");
		return NULL;
	}

	req->epnum	= dep->number;
	req->dep	= dep;

	return &req->request;
}

static void dwc3_gadget_ep_free_request(struct usb_ep *ep,
		struct usb_request *request)
{
	struct dwc3_request		*req = to_dwc3_request(request);

	kfree(req);
}

static inline bool dwc3_ep_is_cleanup(struct dwc3_ep *dep)
{
    return (dep->free_slot == dep->busy_slot);
}

static inline bool dwc3_ep_is_complete(struct dwc3_ep *dep)
{
    struct dwc3_trb	*trb;
    u32 cur_slot = dep->free_slot;

    if(dwc3_ep_is_cleanup(dep))
        return true;

    trb = &dep->trb_pool[(cur_slot-1) & DWC3_TRB_MASK];
    DWC3_BUG(("this trb must be last!\n"), !(trb->ctrl & DWC3_TRB_CTRL_LST));/*lint !e548*/

    return (trb->ctrl & DWC3_TRB_CTRL_HWO)?false:true;
}

/**
 * dwc3_prepare_one_trb - setup one TRB from one request
 * @dep: endpoint for which this request is prepared
 * @req: dwc3_request pointer
 */
static void dwc3_prepare_one_trb(struct dwc3_ep *dep,
		struct dwc3_request *req, dma_addr_t dma,
		unsigned length, unsigned last, unsigned chain)
{
	struct dwc3_trb		*trb;

	unsigned int		cur_slot;

	DWC3_TRACE(("%s: req %p dma %08llx length %d%s%s\n",
			dep->name, req, (unsigned long long) dma,
			length, last ? " last" : "",
			chain ? " chain" : ""));

	trb = &dep->trb_pool[dep->free_slot & DWC3_TRB_MASK];
	cur_slot = dep->free_slot;
	dep->free_slot++;

	/* Skip the LINK-TRB on ISOC */
	if (((cur_slot & DWC3_TRB_MASK) == DWC3_TRB_NUM - 1) &&
			usb_endpoint_xfer_isoc(dep->endpoint.desc))
		return;

	if (!req->trb) {
		dwc3_gadget_move_request_queued(req);
		req->trb = trb;
		req->trb_dma = dwc3_trb_dma_offset(dep, trb);
	}

	trb->size = DWC3_TRB_SIZE_LENGTH(length);
	trb->bpl = lower_32_bits(dma);
	trb->bph = upper_32_bits(dma);

	switch (usb_endpoint_type(dep->endpoint.desc)) {
	case USB_ENDPOINT_XFER_CONTROL:
		trb->ctrl = DWC3_TRBCTL_CONTROL_SETUP;
		break;

	case USB_ENDPOINT_XFER_ISOC:
		trb->ctrl = DWC3_TRBCTL_ISOCHRONOUS_FIRST;

		if (!req->request.no_interrupt)
			trb->ctrl |= DWC3_TRB_CTRL_IOC;
		break;

	case USB_ENDPOINT_XFER_BULK:
        trb->ctrl = DWC3_TRBCTL_NORMAL;
		if (!req->request.no_interrupt
            && dep->endpoint.enable_xfer_in_progress) {
            if (req->direction) /* in */ {
    			/* trb->ctrl |= DWC3_TRB_CTRL_IOC; */
            }
            else {
                trb->ctrl |= DWC3_TRB_CTRL_ISP_IMI;
                trb->ctrl |= DWC3_TRB_CTRL_CSP;
            }
		}
		break;
	case USB_ENDPOINT_XFER_INT:
		trb->ctrl = DWC3_TRBCTL_NORMAL;
		break;
	default:
		/*
		 * This is only possible with faulty memory because we
		 * checked it already :)
		 */
		BUG();
	}

	if (usb_endpoint_xfer_isoc(dep->endpoint.desc)) {
		trb->ctrl |= DWC3_TRB_CTRL_ISP_IMI;
		trb->ctrl |= DWC3_TRB_CTRL_CSP;
	} else {
		if (chain)
			trb->ctrl |= DWC3_TRB_CTRL_CHN;

		if (last)
			trb->ctrl |= DWC3_TRB_CTRL_LST;
	}

	if (usb_endpoint_xfer_bulk(dep->endpoint.desc) && dep->stream_capable)
		trb->ctrl |= DWC3_TRB_CTRL_SID_SOFN(req->request.stream_id);

	trb->ctrl |= DWC3_TRB_CTRL_HWO;
}

/*
 * dwc3_prepare_trbs - setup TRBs from requests
 * @dep: endpoint for which requests are being prepared
 * @starting: true if the endpoint is idle and no requests are queued.
 *
 * The function goes through the requests list and sets up TRBs for the
 * transfers. The function returns once there are no more TRBs available or
 * it runs out of requests.
 */
static void dwc3_prepare_trbs(struct dwc3_ep *dep, bool starting)
{
	struct dwc3_request	*req, *n;
	u32			trbs_left;
    u32			trbs_left_n;
	u32			max;
	unsigned int		last_one = 0;
    struct dwc3			*dwc = dep->dwc;

    dwc3_trb_trace_store(dep, ~0, __LINE__);

	/* the first request must not be queued */
	trbs_left = (dep->busy_slot - dep->free_slot) & DWC3_TRB_MASK;

	/* Can't wrap around on a non-isoc EP since there's no link TRB */
	if (!usb_endpoint_xfer_isoc(dep->endpoint.desc)) {
		max = DWC3_TRB_NUM - (dep->free_slot & DWC3_TRB_MASK);
		if (trbs_left > max)
			trbs_left = max;
	}

	/*
	 * If busy & slot are equal than it is either full or empty. If we are
	 * starting to process requests then we are empty. Otherwise we are
	 * full and don't do anything
	 */
	if (!trbs_left) {
		if (!starting)
			return;
		trbs_left = DWC3_TRB_NUM;
		/*
		 * In case we start from scratch, we queue the ISOC requests
		 * starting from slot 1. This is done because we use ring
		 * buffer and have no LST bit to stop us. Instead, we place
		 * IOC bit every TRB_NUM/4. We try to avoid having an interrupt
		 * after the first request so we start at slot 1 and have
		 * 7 requests proceed before we hit the first IOC.
		 * Other transfer types don't use the ring buffer and are
		 * processed from the first TRB until the last one. Since we
		 * don't wrap around we have to start at the beginning.
		 */
		if (usb_endpoint_xfer_isoc(dep->endpoint.desc)) {
			dep->busy_slot = 1;
			dep->free_slot = 1;
		} else {
			dep->busy_slot = 0;
			dep->free_slot = 0;
		}
	}

    dwc3_trb_trace_store(dep, ~0, __LINE__);

	/* The last TRB is a link TRB, not used for xfer */
	if ((trbs_left <= 1) && usb_endpoint_xfer_isoc(dep->endpoint.desc))
		return;

	list_for_each_entry_safe(req, n, &dep->request_list, list) {/*lint !e413*/
		unsigned	length;
		dma_addr_t	dma;
        last_one = false;

		if (req->request.num_mapped_sgs > 0) {
			struct usb_request *request = &req->request;
			struct scatterlist *sg = request->sg;
			struct scatterlist *s;
			unsigned int		i;

            if(request->num_mapped_sgs >= (req->request.zero?trbs_left:(trbs_left+1)))
            {
                dep->stat.trb_lack++;
                DWC3_INFO(("num_mapped_sgs=%d,trbs_left=%d,request.zero=%d\n",
                    request->num_mapped_sgs,
                    trbs_left,
                    req->request.zero));
                break;
            }

            trbs_left_n = trbs_left - (req->request.zero?(request->num_mapped_sgs+1):request->num_mapped_sgs);

            dwc3_trb_trace_store(dep, request->num_mapped_sgs, __LINE__);
			/* coverity[returned_null] */
			for_each_sg(sg, s, request->num_mapped_sgs, i) {
				unsigned chain = true;
				length = sg_dma_len(s);
				dma = sg_dma_address(s);

				if (i == (request->num_mapped_sgs - 1) ||
						sg_is_last(s)) {

                    /*  multi-transfer quirk:when needn't to set last flag before the last request,
                        unless the next request can't be queued */
                    if (list_is_last(&req->list, &dep->request_list)
                        || (n->request.num_mapped_sgs > (n->request.zero?(trbs_left_n+1):trbs_left_n)))
                    {
                        last_one = true;
                    }
                    chain = false;
				}

				trbs_left--;
				if (!trbs_left)
					last_one = true;

                /* balong quirk:chain entry means transfer boundary */
                if(USB_REQUEST_M_TRANSFER == request->sg_mode
                    || sg_is_chain(s+1))
                {
                    chain = false;
                }

				if (last_one)
					chain = false;

                /* BUGFIX: ZLP on non-endpoint0 under sg case */
                if (false == chain && req->request.zero) {
                    //printk(KERN_ERR "[**NOTE**] %s: sg zlp enter\r\n", __FUNCTION__);

                    if(!trbs_left)
                        dep->stat.trb_zero++;

    				dwc3_prepare_one_trb(dep, req, dma, length,
    						false, true);

                    trbs_left--;

                    if(!trbs_left)
                        dep->stat.trb_zero++;

                    /* NOTE: This is ZLP on non-endpoint0 under sg case */
    				dwc3_prepare_one_trb(dep, req, dwc->zlp_dma,
    				        dep->direction ? 0 : dep->endpoint.maxpacket,
    						last_one, chain);
                } else {
                    if(!trbs_left)
                        dep->stat.trb_zero++;

    				dwc3_prepare_one_trb(dep, req, dma, length,
    						last_one, chain);
                }

				if (last_one)
					break;
			}
            /* add the missed count as the break jump */
            dep->stat.trb_used += i + 1;

            dwc3_trb_trace_store(dep, request->num_mapped_sgs, __LINE__);
		} else {
            if(trbs_left < (req->request.zero?2:1))
            {
                dep->stat.trb_lack++;
                DWC3_INFO(("trbs_left=%d,request.zero=%d\n",
                    trbs_left,req->request.zero));
                break;
            }

			dma = req->request.dma;
			length = req->request.length;
			trbs_left--;

			if (!trbs_left)
				last_one = 1;

			/* multi-transfer quirk:when needn't to set last flag before the last request */
			if (list_is_last(&req->list, &dep->request_list))
				last_one = 1;

            /* BUGFIX: ZLP on non-endpoint0 */
            if (!req->request.zero) {
                if(!trbs_left)
                    dep->stat.trb_zero++;

    			dwc3_prepare_one_trb(dep, req, dma, length,
    					last_one, false);

            } else {
                //printk(KERN_ERR "[**NOTE**] %s: zlp enter\r\n", __FUNCTION__);

                if(!trbs_left)
                    printk("%s line %d:trb not enough\n",__FUNCTION__,__LINE__);

    			dwc3_prepare_one_trb(dep, req, dma, length,
    					false, true);

                trbs_left--;

                if(!trbs_left)
                    printk("%s line %d:trb not enough\n",__FUNCTION__,__LINE__);

                /* NOTE: This is ZLP on non-endpoint0 */
    			dwc3_prepare_one_trb(dep, req, dwc->zlp_dma,
    			        dep->direction ? 0 : dep->endpoint.maxpacket,
    					last_one, false);
            }

            dep->stat.trb_used++;

			if (last_one)
				break;
		}
	}

    dwc3_trb_trace_store(dep, ~0, __LINE__);
}

static int __dwc3_gadget_kick_transfer(struct dwc3_ep *dep, u16 cmd_param,
		int start_new)
{
	struct dwc3_gadget_ep_cmd_params params;
	struct dwc3_request		*req;
	struct dwc3			*dwc = dep->dwc;
	int				ret;
	u32				cmd;

	if (start_new && (dep->flags & DWC3_EP_BUSY)) {
		DWC3_DBG(("%s: endpoint busy\n", dep->name));
        dep->stat.kick_busy++;
		return -EBUSY;
	}
	dep->flags &= ~DWC3_EP_PENDING_REQUEST;

	/*
	 * If we are getting here after a short-out-packet we don't enqueue any
	 * new requests as we try to set the IOC bit only on the last request.
	 */
	if (start_new) {
		if (list_empty(&dep->req_queued))
		{
            dwc3_prepare_trbs(dep, start_new);
        }

		/* req points to the first request which will be sent */
		req = next_request(&dep->req_queued);
	} else {
		dwc3_prepare_trbs(dep, start_new);

		/*
		 * req points to the first request where HWO changed from 0 to 1
		 */
		req = next_request(&dep->req_queued);
	}
	if (!req) {
		dep->flags |= DWC3_EP_PENDING_REQUEST;
        dep->stat.kick_none++;
		return 0;
	}

	/* memset(&params, 0, sizeof(params));*/
	params.param0 = upper_32_bits(req->trb_dma);
	params.param1 = lower_32_bits(req->trb_dma);
    params.param2 = 0;

	if (start_new)
		cmd = DWC3_DEPCMD_STARTTRANSFER | DWC3_DEPCMD_HIPRI_FORCERM;
	else
		cmd = DWC3_DEPCMD_UPDATETRANSFER;

	cmd |= DWC3_DEPCMD_PARAM(cmd_param);
	ret = dwc3_send_gadget_ep_cmd(dwc, dep->number, cmd, &params);
	if (ret < 0) {
		dev_dbg(dwc->dev, "failed to send STARTTRANSFER command\n");

		/*
		 * FIXME we need to iterate over the list of requests
		 * here and stop, unmap, free and del each of the linked
		 * requests instead of what we do now.
		 */
		usb_gadget_unmap_request(&dwc->gadget, &req->request,
				req->direction);
		list_del(&req->list);
        dep->stat.kick_cmderr++;
		return ret;
	}

	dep->flags |= DWC3_EP_BUSY;

	if (start_new) {
		dep->resource_index = dwc3_gadget_ep_get_transfer_index(dwc,
				dep->number);
		WARN_ON_ONCE(!dep->resource_index);
	}

    if(dep->endpoint.xfer_cmd_monitor && dep->timer)
    {
        dep->stat.monitor_start++;
        (void)mod_timer(dep->timer,jiffies + 3*HZ);
    }

    dep->stat.kick_ok++;
	return 0;
}

static void __dwc3_gadget_start_isoc(struct dwc3 *dwc,
		struct dwc3_ep *dep, u32 cur_uf)
{
	u32 uf;

	if (list_empty(&dep->request_list)) {
		DWC3_DBG(("ISOC ep %s run out for requests.\n",
			dep->name));
		dep->flags |= DWC3_EP_PENDING_REQUEST;
		return;
	}

	/* 4 micro frames in the future */
	uf = cur_uf + dep->interval * 4;
	/* coverity[check_return] */
	__dwc3_gadget_kick_transfer(dep, uf, 1);
}

static void dwc3_gadget_start_isoc(struct dwc3 *dwc,
		struct dwc3_ep *dep, const struct dwc3_event_depevt *event)
{
	u32 cur_uf, mask;

	mask = ~(dep->interval - 1);
	cur_uf = event->parameters & mask;

	__dwc3_gadget_start_isoc(dwc, dep, cur_uf);
}

int dwc3_event_process(struct dwc3 *dwc);

static inline bool __dwc3_gadget_ep_kick(struct dwc3_ep *dep, u32 kicksource)
{
    struct dwc3		*dwc = dep->dwc;

    if(dwc3_ep_is_complete(dep))        /* td complete */
    {
        /* there are two cases:
           1.complete intr executed,but kicked nothing.
             in this case, dwc3_ep_is_cleanup matched.
           2.complete intr didn't executed.
             in this case, dwc3_ep_is_cleanup unmatch.
        */
        if(dwc3_ep_is_cleanup(dep))    /* complete events processed */
        {
            (void)__dwc3_gadget_kick_transfer(dep, 0, 1);
            dep->stat.dokick[kicksource]++;
            dep->stat.kick_pos = __LINE__;
        }
        else /* complete events not processed */
        {
            (void)dwc3_event_process(dwc);    /* processe events,including kick */
            dep->stat.eventkick[kicksource]++;
        }

        return true;
    }
    else    /* td not complete, waiting */
    {
        dep->stat.nokick[kicksource]++;
        dep->stat.kick_pos = __LINE__;
        return false;
    }
}

static int __dwc3_gadget_ep_queue(struct dwc3_ep *dep, struct dwc3_request *req)
{
	struct dwc3		*dwc = dep->dwc;
	int			ret;

	req->request.actual	= 0;
	req->request.status	= -EINPROGRESS;
	req->direction		= dep->direction;
	req->epnum		= dep->number;

	/*
	 * We only add to our list of requests now and
	 * start consuming the list once we get XferNotReady
	 * IRQ.
	 *
	 * That way, we avoid doing anything that we don't need
	 * to do now and defer it until the point we receive a
	 * particular token from the Host side.
	 *
	 * This will also avoid Host cancelling URBs due to too
	 * many NAKs.
	 */
	ret = usb_gadget_map_request(&dwc->gadget, &req->request,
			dep->direction);
	if (ret)
		return ret;

	list_add_tail(&req->list, &dep->request_list);

    if(dep->endpoint.masknotready)  /* if we mask notready intr */
    {
        (void)__dwc3_gadget_ep_kick(dep, DWC3_EP_KICKSOURCE_QUEUE);
        return 0;
    }

    /*
	 * There are a few special cases:
	 *
	 * 1. XferNotReady with empty list of requests. We need to kick the
	 *    transfer here in that situation, otherwise we will be NAKing
	 *    forever. If we get XferNotReady before gadget driver has a
	 *    chance to queue a request, we will ACK the IRQ but won't be
	 *    able to receive the data until the next request is queued.
	 *    The following code is handling exactly that.
	 *
	 */
	if (dep->flags & DWC3_EP_PENDING_REQUEST) {
		int	ret;/*lint !e578*/

		/*
		 * If xfernotready is already elapsed and it is a case
		 * of isoc transfer, then issue END TRANSFER, so that
		 * you can receive xfernotready again and can have
		 * notion of current microframe.
		 */
		if (usb_endpoint_xfer_isoc(dep->endpoint.desc)) {
			dwc3_stop_active_transfer(dwc, dep->number);
			return 0;
		}

		ret = __dwc3_gadget_kick_transfer(dep, 0, true);
		if (ret && ret != -EBUSY)
			DWC3_INFO(("%s: failed to kick transfers\n",
					dep->name));
	}

	/*
	 * 2. XferInProgress on Isoc EP with an active transfer. We need to
	 *    kick the transfer here after queuing a request, otherwise the
	 *    core may not see the modified TRB(s).
	 */
	if (usb_endpoint_xfer_isoc(dep->endpoint.desc) &&
			(dep->flags & DWC3_EP_BUSY) &&
			!(dep->flags & DWC3_EP_MISSED_ISOC)) {
		WARN_ON_ONCE(!dep->resource_index);
		ret = __dwc3_gadget_kick_transfer(dep, dep->resource_index,
				false);
		if (ret && ret != -EBUSY)
			dev_dbg(dwc->dev, "%s: failed to kick transfers\n",
					dep->name);
	}

	/*
	 * 3. Missed ISOC Handling. We need to start isoc transfer on the saved
	 * uframe number.
	 */
	if (usb_endpoint_xfer_isoc(dep->endpoint.desc) &&
		(dep->flags & DWC3_EP_MISSED_ISOC)) {
			__dwc3_gadget_start_isoc(dwc, dep, dep->current_uf);
			dep->flags &= ~DWC3_EP_MISSED_ISOC;
	}

	return 0;
}

static int dwc3_gadget_ep_queue(struct usb_ep *ep, struct usb_request *request,
	gfp_t gfp_flags)
{
	struct dwc3_request		*req = to_dwc3_request(request);
	struct dwc3_ep			*dep = to_dwc3_ep(ep);
	struct dwc3			*dwc = dep->dwc;

	unsigned long			flags;

	int				ret;

	if (!(dep->flags & DWC3_EP_ENABLED)) {
		dev_dbg(dwc->dev, "trying to queue request %p to disabled %s\n",
				request, ep->name);
		return -ESHUTDOWN;
	}

	DWC3_TRACE(("queing request %p to %s length %d\n",
			request, ep->name, request->length));

	spin_lock_irqsave(&dwc->lock, flags);
	ret = __dwc3_gadget_ep_queue(dep, req);
	spin_unlock_irqrestore(&dwc->lock, flags);

	return ret;
}

static int dwc3_gadget_ep_dequeue(struct usb_ep *ep,
		struct usb_request *request)
{
	struct dwc3_request		*req = to_dwc3_request(request);
	struct dwc3_request		*r = NULL;

	struct dwc3_ep			*dep = to_dwc3_ep(ep);
	struct dwc3			*dwc = dep->dwc;

	unsigned long			flags;
	int				ret = 0;

    dep->stat.dequeue_cnt++;

	spin_lock_irqsave(&dwc->lock, flags);

	list_for_each_entry(r, &dep->request_list, list) {
		if (r == req)
			break;
	}

	if (r != req) {
		list_for_each_entry(r, &dep->req_queued, list) {
			if (r == req)
				break;
		}
		if (r == req) {
			/* wait until it is processed */
			dwc3_stop_active_transfer(dwc, dep->number);
			goto out1;
		}
		dev_err(dwc->dev, "request %p was not queued to %s\n",
				request, ep->name);
		ret = -EINVAL;
		goto out0;
	}

out1:
	/* giveback the request */
	dwc3_gadget_giveback(dep, req, -ECONNRESET);

out0:
	spin_unlock_irqrestore(&dwc->lock, flags);

	return ret;
}

int __dwc3_gadget_ep_set_halt(struct dwc3_ep *dep, int value)
{
	struct dwc3_gadget_ep_cmd_params	params;
	struct dwc3				*dwc = dep->dwc;
	int					ret;

    dep->stat.sethalt_cnt++;

	memset(&params, 0x00, sizeof(params));
	if (value) {
		ret = dwc3_send_gadget_ep_cmd(dwc, dep->number,
			DWC3_DEPCMD_SETSTALL, &params);
		if (ret)
			dev_err(dwc->dev, "failed to %s STALL on %s\n",
					"set", dep->name);
		else
			dep->flags |= DWC3_EP_STALL;
	} else {
		if (dep->flags & DWC3_EP_WEDGE)
			return 0;

		ret = dwc3_send_gadget_ep_cmd(dwc, dep->number,
			DWC3_DEPCMD_CLEARSTALL, &params);
		if (ret)
			dev_err(dwc->dev, "failed to %s STALL on %s\n",
					"clear", dep->name);
		else
			dep->flags &= ~DWC3_EP_STALL;
	}

	return ret;
}

static int dwc3_gadget_ep_set_halt(struct usb_ep *ep, int value)
{
	struct dwc3_ep			*dep = to_dwc3_ep(ep);
	struct dwc3			*dwc = dep->dwc;

	unsigned long			flags;

	int				ret;

    dep->stat.setwedge_cnt++;

	spin_lock_irqsave(&dwc->lock, flags);

	if (usb_endpoint_xfer_isoc(dep->endpoint.desc)) {
		dev_err(dwc->dev, "%s is of Isochronous type\n", dep->name);
		ret = -EINVAL;
		goto out;
	}

	ret = __dwc3_gadget_ep_set_halt(dep, value);
out:
	spin_unlock_irqrestore(&dwc->lock, flags);

	return ret;
}

static int dwc3_gadget_ep_set_wedge(struct usb_ep *ep)
{
	struct dwc3_ep			*dep = to_dwc3_ep(ep);
	struct dwc3			*dwc = dep->dwc;
	unsigned long			flags;

	spin_lock_irqsave(&dwc->lock, flags);
	dep->flags |= DWC3_EP_WEDGE;
	spin_unlock_irqrestore(&dwc->lock, flags);

	if (dep->number == 0 || dep->number == 1)
		return dwc3_gadget_ep0_set_halt(ep, 1);
	else
		return dwc3_gadget_ep_set_halt(ep, 1);
}

static void dwc3_disconnect_gadget(struct dwc3 *dwc)
{
	if (dwc->gadget_driver && dwc->gadget_driver->disconnect) {
		spin_unlock(&dwc->lock);
		dwc->gadget_driver->disconnect(&dwc->gadget);
		spin_lock(&dwc->lock);
	}
}
static int enumerated_done = 0;
int is_dwc3_enumerated(void);
static void dwc3_gadget_disconnect_interrupt(struct dwc3 *dwc)
{
	int			reg;

	dev_vdbg(dwc->dev, "%s\n", __func__);

	reg = dwc3_readl(dwc->regs, DWC3_DCTL);
	reg &= ~DWC3_DCTL_INITU1ENA;
	dwc3_writel(dwc->regs, DWC3_DCTL, reg);

	reg &= ~DWC3_DCTL_INITU2ENA;
	dwc3_writel(dwc->regs, DWC3_DCTL, reg);

	dwc3_disconnect_gadget(dwc);
	dwc->start_config_issued = false;

	dwc->gadget.speed = USB_SPEED_UNKNOWN;
	dwc->setup_packet_pending = false;
}

static void dwc3_gadget_usb3_phy_suspend(struct dwc3 *dwc, int suspend)
{
	u32			reg;

	reg = dwc3_readl(dwc->regs, DWC3_GUSB3PIPECTL(0));

	if (suspend)
		reg |= DWC3_GUSB3PIPECTL_SUSPHY;
	else
		reg &= ~DWC3_GUSB3PIPECTL_SUSPHY;

	dwc3_writel(dwc->regs, DWC3_GUSB3PIPECTL(0), reg);
}

static void dwc3_gadget_usb2_phy_suspend(struct dwc3 *dwc, int suspend)
{
	u32			reg;

	reg = dwc3_readl(dwc->regs, DWC3_GUSB2PHYCFG(0));

	if (suspend)
		reg |= DWC3_GUSB2PHYCFG_SUSPHY;
	else
		reg &= ~DWC3_GUSB2PHYCFG_SUSPHY;

	dwc3_writel(dwc->regs, DWC3_GUSB2PHYCFG(0), reg);
}


/* -------------------------------------------------------------------------- */

static struct usb_endpoint_descriptor dwc3_gadget_ep0_desc = {
	.bLength	= USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bmAttributes	= USB_ENDPOINT_XFER_CONTROL,
};

static const struct usb_ep_ops dwc3_gadget_ep0_ops = {
	.enable		= dwc3_gadget_ep0_enable,
	.disable	= dwc3_gadget_ep0_disable,
	.alloc_request	= dwc3_gadget_ep_alloc_request,
	.free_request	= dwc3_gadget_ep_free_request,
	.queue		= dwc3_gadget_ep0_queue,
	.dequeue	= dwc3_gadget_ep_dequeue,
	.set_halt	= dwc3_gadget_ep0_set_halt,
	.set_wedge	= dwc3_gadget_ep_set_wedge,
};

static const struct usb_ep_ops dwc3_gadget_ep_ops = {
	.enable		= dwc3_gadget_ep_enable,
	.disable	= dwc3_gadget_ep_disable,
	.alloc_request	= dwc3_gadget_ep_alloc_request,
	.free_request	= dwc3_gadget_ep_free_request,
	.queue		= dwc3_gadget_ep_queue,
	.dequeue	= dwc3_gadget_ep_dequeue,
	.set_halt	= dwc3_gadget_ep_set_halt,
	.set_wedge	= dwc3_gadget_ep_set_wedge,
};

/* -------------------------------------------------------------------------- */

static int dwc3_gadget_get_frame(struct usb_gadget *g)
{
	struct dwc3		*dwc = gadget_to_dwc(g);
	u32			reg;

	reg = dwc3_readl(dwc->regs, DWC3_DSTS);
	return DWC3_DSTS_SOFFN(reg);
}

static void dwc3_gadget_remote_wake_notify(struct dwc3 *dwc, u32 intf)
{
    u32 param;

    if (dwc->speed == DWC3_DCFG_SUPERSPEED) {
        param = DWC3_DGCMD_DEV_NOTIFY_FUNC_WAKEUP |
            (intf << DWC3_DGCMD_DEV_NOTIFY_PARAM_SHIFT);
        dwc3_send_gadget_generic_command(dwc,
            DWC3_DGCMD_FUNCTION_WAKEUP_DEV_NOTIFY, param);
    }
}
static void dwc3_gadget_wakeup_interrupt(struct dwc3 *dwc);

/* should work in atomic context, as the function driver could move on
   ONLY after the gadget is resumed successfully */
static int dwc3_gadget_wakeup(struct usb_gadget *g)
{
	struct dwc3		*dwc = gadget_to_dwc(g);

	unsigned long		timeout;/*lint !e550*/
	unsigned long		flags;

	u32			reg;

	int			ret = 0;

	u8			link_state;

    if (!g->rwakeup) {
        if (printk_ratelimit())
            dev_warn(dwc->dev,"gadget not enabled for remote wakeup\n");
        return -EOPNOTSUPP;
    }

    spin_lock_irqsave(&dwc->lock, flags);

	/*
	 * According to the Databook Remote wakeup request should
	 * be issued only when the device is in early suspend state.
	 *
	 * We can check that via USB Link State bits in DSTS register.
	 */
	reg = dwc3_readl(dwc->regs, DWC3_DSTS);

	link_state = DWC3_DSTS_USBLNKST(reg);

	switch (link_state) {
	case DWC3_LINK_STATE_RX_DET:	/* in HS, means Early Suspend */
	case DWC3_LINK_STATE_U3:	/* in HS, means SUSPEND */
		break;
	default:
		dev_dbg(dwc->dev, "can't wakeup from link state %d\n",
				link_state);
		ret = -EINVAL;
		goto out;
	}

	ret = dwc3_gadget_set_link_state(dwc, DWC3_LINK_STATE_RECOV);
	if (ret < 0) {
		dev_err(dwc->dev, "failed to put link in Recovery\n");
		goto out;
	}

	/* Recent versions do this automatically */
	if (dwc->revision < DWC3_REVISION_194A) {
		/* write zeroes to Link Change Request */
		reg = dwc3_readl(dwc->regs, DWC3_DCTL);
		reg &= ~DWC3_DCTL_ULSTCHNGREQ_MASK;
		dwc3_writel(dwc->regs, DWC3_DCTL, reg);
	}

	/* poll until Link State changes to ON */
	timeout = jiffies + msecs_to_jiffies(500);

	while (!time_after(jiffies, timeout)) {
		reg = dwc3_readl(dwc->regs, DWC3_DSTS);

		/* in HS, means ON */
		if (DWC3_DSTS_USBLNKST(reg) == DWC3_LINK_STATE_U0)
			break;
	}

	if (DWC3_DSTS_USBLNKST(reg) != DWC3_LINK_STATE_U0) {
		dev_err(dwc->dev, "failed to send remote wakeup\n");
		ret = -EINVAL;
		goto out;
	}

    /* send function remote wakeup notify */
    dwc3_gadget_remote_wake_notify(dwc,0);

    dwc3_gadget_wakeup_interrupt(dwc);

out:
	spin_unlock_irqrestore(&dwc->lock, flags);

	return ret;
}/*lint !e550*/

static int dwc3_gadget_set_selfpowered(struct usb_gadget *g,
		int is_selfpowered)
{
	struct dwc3		*dwc = gadget_to_dwc(g);
	unsigned long		flags;

	spin_lock_irqsave(&dwc->lock, flags);
	dwc->is_selfpowered = !!is_selfpowered;
	spin_unlock_irqrestore(&dwc->lock, flags);

	return 0;
}

static int dwc3_gadget_run_stop(struct dwc3 *dwc, int is_on)
{
	u32			reg;
	u32			timeout = 500;

	reg = dwc3_readl(dwc->regs, DWC3_DCTL);
	if (is_on) {
		if (dwc->revision <= DWC3_REVISION_187A) {
			reg &= ~DWC3_DCTL_TRGTULST_MASK;
			reg |= DWC3_DCTL_TRGTULST_RX_DET;
		}

		if (dwc->revision >= DWC3_REVISION_194A)
			reg &= ~DWC3_DCTL_KEEP_CONNECT;
		reg |= DWC3_DCTL_RUN_STOP;
	} else {
		reg &= ~DWC3_DCTL_RUN_STOP;
	}

	dwc3_writel(dwc->regs, DWC3_DCTL, reg);

	do {
		reg = dwc3_readl(dwc->regs, DWC3_DSTS);
		if (is_on) {
			if (!(reg & DWC3_DSTS_DEVCTRLHLT))
				break;
		} else {
			if (reg & DWC3_DSTS_DEVCTRLHLT)
				break;
		}
		timeout--;
		if (!timeout)
			return -ETIMEDOUT;
		udelay(1);
	} while (1);

	USB_DBG_DWC3(dwc->dev, "gadget %s data soft-%s\n",
			dwc->gadget_driver
			? dwc->gadget_driver->function : "no-function",
			is_on ? "connect" : "disconnect");

	return 0;
}

static int dwc3_gadget_pullup(struct usb_gadget *g, int is_on)
{
#ifndef CONFIG_USB_OTG_DWC
	struct dwc3		*dwc = gadget_to_dwc(g);
	unsigned long		flags;
#endif
	int			ret = 0;

	is_on = !!is_on;

	dwc3_enable_both_phy();

#ifndef CONFIG_USB_OTG_DWC
	spin_lock_irqsave(&dwc->lock, flags);
	ret = dwc3_gadget_run_stop(dwc, is_on);
	spin_unlock_irqrestore(&dwc->lock, flags);
#endif

	return ret;
}

static int dwc3_gadget_start(struct usb_gadget *g,
		struct usb_gadget_driver *driver)
{
	struct dwc3		*dwc = gadget_to_dwc(g);/*lint !e413*/
#ifndef CONFIG_USB_OTG_DWC
	struct dwc3_ep		*dep;
#endif
	unsigned long		flags;
	int			ret = 0;
	u32			reg;

#ifdef CONFIG_USB_OTG_DWC
	struct usb_phy *otg = usb_get_transceiver();
	if (!otg) {
		dev_err(dwc->dev, "OTG driver not available!\n");
		return -ENODEV;
	}

	usb_put_transceiver(otg);
#endif

	spin_lock_irqsave(&dwc->lock, flags);

	if (dwc->gadget_driver) {
		dev_err(dwc->dev, "%s is already bound to %s\n",
				dwc->gadget.name,
				dwc->gadget_driver->driver.name);
		ret = -EBUSY;
		goto err0;
	}

	dwc->gadget_driver	= driver;
	dwc->gadget.dev.driver	= &driver->driver;

	reg = dwc3_readl(dwc->regs, DWC3_DCFG);
	reg &= ~(DWC3_DCFG_SPEED_MASK);

	/**
	 * WORKAROUND: DWC3 revision < 2.20a have an issue
	 * which would cause metastability state on Run/Stop
	 * bit if we try to force the IP to USB2-only mode.
	 *
	 * Because of that, we cannot configure the IP to any
	 * speed other than the SuperSpeed
	 *
	 * Refers to:
	 *
	 * STAR#9000525659: Clock Domain Crossing on DCTL in
	 * USB 2.0 Mode
	 */
#ifndef CONFIG_USB_FORCE_HIGHSPEED
	if (dwc->revision < DWC3_REVISION_220A)
		reg |= DWC3_DCFG_SUPERSPEED;
	else
		reg |= dwc->maximum_speed;
#endif
	dwc3_writel(dwc->regs, DWC3_DCFG, reg);

	dwc->start_config_issued = false;

#ifndef CONFIG_USB_OTG_DWC
	/* Start with SuperSpeed Default */
	dwc3_gadget_ep0_desc.wMaxPacketSize = cpu_to_le16(512);

	dep = dwc->eps[0];
	ret = __dwc3_gadget_ep_enable(dep, &dwc3_gadget_ep0_desc, NULL, false);
	if (ret) {
		dev_err(dwc->dev, "failed to enable %s\n", dep->name);
		goto err0;
	}

	dep = dwc->eps[1];
	ret = __dwc3_gadget_ep_enable(dep, &dwc3_gadget_ep0_desc, NULL, false);
	if (ret) {
		dev_err(dwc->dev, "failed to enable %s\n", dep->name);
		goto err1;
	}

	/* begin to receive SETUP packets */
	dwc->ep0state = EP0_SETUP_PHASE;
	dwc3_ep0_out_start(dwc);/*lint !e413*/
#endif

	spin_unlock_irqrestore(&dwc->lock, flags);

#ifdef CONFIG_USB_OTG_DWC
	otg = usb_get_transceiver();
	otg->io_priv = (uint8_t *)dwc->regs - DWC3_GLOBALS_REGS_START;
	otg_set_peripheral(otg->otg, &dwc->gadget);
	usb_put_transceiver(otg);
#endif

	return 0;

#ifndef CONFIG_USB_OTG_DWC
err1:
	__dwc3_gadget_ep_disable(dwc->eps[0]);
#endif

err0:
	spin_unlock_irqrestore(&dwc->lock, flags);

	return ret;
}

static int dwc3_gadget_stop(struct usb_gadget *g,
		struct usb_gadget_driver *driver)
{
	struct dwc3		*dwc = gadget_to_dwc(g);
	unsigned long		flags;

#ifdef CONFIG_USB_OTG_DWC
    struct usb_phy *otg;
#endif

	spin_lock_irqsave(&dwc->lock, flags);

#ifdef CONFIG_USB_OTG_DWC
	otg = usb_get_transceiver();
	otg_set_peripheral(otg->otg, NULL);
	usb_put_transceiver(otg);
#endif

	__dwc3_gadget_ep_disable(dwc->eps[0]);
	__dwc3_gadget_ep_disable(dwc->eps[1]);

	dwc->gadget_driver	= NULL;
	dwc->gadget.dev.driver	= NULL;

	spin_unlock_irqrestore(&dwc->lock, flags);

	return 0;
}

#ifdef CONFIG_USB_OTG_DWC

int pcd_otg_start_peripheral(struct usb_gadget *gadget)
{
    	struct dwc3 *dwc = gadget_to_dwc(gadget);

	return gadget->ops->start(dwc->gadget_driver, NULL);
}

int pcd_otg_stop_peripheral(struct usb_gadget *gadget)
{
    	struct dwc3 *dwc = gadget_to_dwc(gadget);

	return gadget->ops->stop(dwc->gadget_driver);
}

static int dwc3_otg_gadget_start(struct usb_gadget_driver *driver,
			int (*bind)(struct usb_gadget *))
{
    struct usb_gadget *g = usb_udc_get_gadget(driver);
	struct dwc3		*dwc = gadget_to_dwc(g);
	struct dwc3_ep		*dep;
	unsigned long		flags;
	u32					reg;
	int					ret;
    u32                 i;

	spin_lock_irqsave(&dwc->lock, flags);

	dwc3_core_reinit(dwc);

	reg = dwc3_readl(dwc->regs, DWC3_DCFG);
	reg |= DWC3_DCFG_LPM_CAP;
	dwc3_writel(dwc->regs, DWC3_DCFG, reg);

	dwc3_intr_disable(dwc);
	for (i = 0; i < dwc->num_event_buffers; i++) {
		reg = dwc3_readl(dwc->regs, DWC3_GEVNTCOUNT(i));
		dwc3_writel(dwc->regs, DWC3_GEVNTCOUNT(i), reg);
	}
	dwc3_intr_enable(dwc);

	/* Enable all but Start and End of Frame IRQs */
	reg = (DWC3_DEVTEN_VNDRDEVTSTRCVEDEN |
			DWC3_DEVTEN_EVNTOVERFLOWEN |
			DWC3_DEVTEN_CMDCMPLTEN |
			DWC3_DEVTEN_ERRTICERREN |
			DWC3_DEVTEN_WKUPEVTEN |
			DWC3_DEVTEN_ULSTCNGEN |
			DWC3_DEVTEN_CONNECTDONEEN |
			DWC3_DEVTEN_USBRSTEN |
			DWC3_DEVTEN_DISCONNEVTEN);
	dwc3_writel(dwc->regs, DWC3_DEVTEN, reg);

	/* Enable USB2 LPM and automatic phy suspend only on recent versions */
	if (dwc->revision >= DWC3_REVISION_194A) {
		reg = dwc3_readl(dwc->regs, DWC3_DCFG);
		reg |= DWC3_DCFG_LPM_CAP;
		dwc3_writel(dwc->regs, DWC3_DCFG, reg);

		reg = dwc3_readl(dwc->regs, DWC3_DCTL);
		reg &= ~(DWC3_DCTL_HIRD_THRES_MASK | DWC3_DCTL_L1_HIBER_EN);

		/* TODO: This should be configurable */
		reg |= DWC3_DCTL_HIRD_THRES(28);

		dwc3_writel(dwc->regs, DWC3_DCTL, reg);

		dwc3_gadget_usb2_phy_suspend(dwc, false);
		dwc3_gadget_usb3_phy_suspend(dwc, false);
	}

	reg = dwc3_readl(dwc->regs, DWC3_DCFG);
	reg &= ~(DWC3_DCFG_SPEED_MASK);

	/**
	 * WORKAROUND: DWC3 revision < 2.20a have an issue
	 * which would cause metastability state on Run/Stop
	 * bit if we try to force the IP to USB2-only mode.
	 *
	 * Because of that, we cannot configure the IP to any
	 * speed other than the SuperSpeed
	 *
	 * Refers to:
	 *
	 * STAR#9000525659: Clock Domain Crossing on DCTL in
	 * USB 2.0 Mode
	 */
	if (dwc->revision < DWC3_REVISION_220A)
		reg |= DWC3_DCFG_SUPERSPEED;
	else
		reg |= dwc->maximum_speed;
	dwc3_writel(dwc->regs, DWC3_DCFG, reg);

	reg = dwc3_readl(dwc->regs, DWC3_DCFG);
	reg &= ~(DWC3_DCFG_DEVADDR_MASK);
	reg |= DWC3_DCFG_DEVADDR(0);
	dwc3_writel(dwc->regs, DWC3_DCFG, reg);

    dwc->dev_state = DWC3_DEFAULT_STATE;
	dwc->start_config_issued = false;

	/* Start with SuperSpeed Default */
	dwc3_gadget_ep0_desc.wMaxPacketSize = cpu_to_le16(512);

	dep = dwc->eps[0];
	ret = __dwc3_gadget_ep_enable(dep, &dwc3_gadget_ep0_desc, NULL, false);
	if (ret) {
		dev_err(dwc->dev, "failed to enable %s\n", dep->name);
		goto err0;
	}

	dep = dwc->eps[1];
	ret = __dwc3_gadget_ep_enable(dep, &dwc3_gadget_ep0_desc, NULL, false);
	if (ret) {
		dev_err(dwc->dev, "failed to enable %s\n", dep->name);
		goto err1;
	}

	/* begin to receive SETUP packets */
	dwc->ep0state = EP0_SETUP_PHASE;
	dwc3_ep0_out_start(dwc);

    /* connect to host */
    ret = dwc3_gadget_run_stop(dwc, 1);
    if (ret) {
		dev_err(dwc->dev, "failed to run peripheral\n");
        goto err2;
    }

    dwc->wants_host = 0;
    spin_unlock_irqrestore(&dwc->lock, flags);

    return 0;

err2:
    __dwc3_gadget_ep_disable(dwc->eps[1]);

err1:
    __dwc3_gadget_ep_disable(dwc->eps[0]);

err0:
	spin_unlock_irqrestore(&dwc->lock, flags);

    return ret;
}

static int dwc3_otg_gadget_stop(struct usb_gadget_driver *driver)
{
	struct usb_gadget *g = usb_udc_get_gadget(driver);
	struct dwc3		*dwc = gadget_to_dwc(g);
	unsigned long		flags;
	u32             temp;

	spin_lock_irqsave(&dwc->lock, flags);

	/* Remove request and disconnect gadget */
	dwc3_gadget_disconnect_interrupt(dwc);

	__dwc3_gadget_ep_disable(dwc->eps[0]);
	__dwc3_gadget_ep_disable(dwc->eps[1]);

	/* Clear Run/Stop bit */
	dwc3_gadget_run_stop(dwc, 0);

	/* Disable device interrupt */
	dwc3_writel(dwc->regs, DWC3_DEVTEN, 0);

	spin_unlock_irqrestore(&dwc->lock, flags);

	do {
		msleep(1);
		temp = dwc3_readl(dwc->regs, DWC3_DSTS);
	} while (!(temp & DWC3_DSTS_DEVCTRLHLT));

    msleep(10);
    return 0;
}

static int dwc3_otg_gadget_send_hrr(struct usb_gadget *g, int is_init)
{
	struct dwc3		*dwc = gadget_to_dwc(g);
    u32     param;

	param = is_init ? DWC3_DGCMDPAR_HOST_ROLE_REQ_INITIATE :
		DWC3_DGCMDPAR_HOST_ROLE_REQ_CONFIRM;

	return dwc3_xmit_host_role_request(dwc, param);
}

#endif


static const struct usb_gadget_ops dwc3_gadget_ops = {
	.get_frame		= dwc3_gadget_get_frame,
	.wakeup			= dwc3_gadget_wakeup,
	.set_selfpowered	= dwc3_gadget_set_selfpowered,
	.pullup			= dwc3_gadget_pullup,
	.udc_start		= dwc3_gadget_start,
	.udc_stop		= dwc3_gadget_stop,
#ifdef CONFIG_USB_OTG_DWC
    .start          = dwc3_otg_gadget_start,
    .stop           = dwc3_otg_gadget_stop,
    .send_hrr       = dwc3_otg_gadget_send_hrr,
#endif
};

/* -------------------------------------------------------------------------- */

static int __devinit dwc3_gadget_init_endpoints(struct dwc3 *dwc)
{
	struct dwc3_ep			*dep;
	u8				epnum;

	INIT_LIST_HEAD(&dwc->gadget.ep_list);

	for (epnum = 0; epnum < DWC3_ENDPOINTS_NUM; epnum++) {
		dep = kzalloc(sizeof(*dep), GFP_KERNEL);
		if (!dep) {
			dev_err(dwc->dev, "can't allocate endpoint %d\n",
					epnum);
			return -ENOMEM;
		}

		dep->dwc = dwc;
		dep->number = epnum;
		dwc->eps[epnum] = dep;

		snprintf(dep->name, sizeof(dep->name), "ep%d%s", epnum >> 1,
				(epnum & 1) ? "in" : "out");
		dep->endpoint.name = dep->name;
		dep->direction = (epnum & 1);

		if (epnum == 0 || epnum == 1) {
			dep->endpoint.maxpacket = 512;
			dep->endpoint.ops = &dwc3_gadget_ep0_ops;
			if (!epnum)
				dwc->gadget.ep0 = &dep->endpoint;
		} else {
			int		ret;

			dep->endpoint.maxpacket = 1024;
			dep->endpoint.max_streams = 15;
			dep->endpoint.ops = &dwc3_gadget_ep_ops;
			list_add_tail(&dep->endpoint.ep_list,
					&dwc->gadget.ep_list);

			ret = dwc3_alloc_trb_pool(dep);
			if (ret)
				return ret;
		}

		INIT_LIST_HEAD(&dep->request_list);
		INIT_LIST_HEAD(&dep->req_queued);
	}

	return 0;
}

static void dwc3_gadget_free_endpoints(struct dwc3 *dwc)
{
	struct dwc3_ep			*dep;
	u8				epnum;

	for (epnum = 0; epnum < DWC3_ENDPOINTS_NUM; epnum++) {
		dep = dwc->eps[epnum];
		dwc3_free_trb_pool(dep);

		if (epnum != 0 && epnum != 1)
			list_del(&dep->endpoint.ep_list);

		kfree(dep);
	}
}

static void dwc3_gadget_release(struct device *dev)
{
	dev_dbg(dev, "%s\n", __func__);
}

/* -------------------------------------------------------------------------- */
static int dwc3_cleanup_done_reqs(struct dwc3 *dwc, struct dwc3_ep *dep,
		const struct dwc3_event_depevt *event, int status)
{
	struct dwc3_request	*req;
	struct dwc3_trb		*trb = NULL;
	unsigned int		count;
	unsigned int		s_pkt = 0;
	unsigned int		trb_status;

    dwc3_trb_trace_store(dep, ~0, __LINE__);

	do {
		req = next_request(&dep->req_queued);
		if (!req) {
            dep->stat.cleanup_warn++;
            dev_err(dwc->dev, "warning on:ep %s ctrl %x status %x s_pkt%d\n",
							dep->name,trb?trb->ctrl:0,event->status,s_pkt);
			WARN_ON_ONCE(1);
			return 1;
		}

        dwc3_trb_trace_store(dep, ~0, __LINE__);

        trb = req->trb;

        /* BUGFIX: Fix complete warning issue */
        if (req->request.num_mapped_sgs)
            trb += req->request.num_mapped_sgs - 1;

		if ((trb->ctrl & DWC3_TRB_CTRL_HWO) && status != -ESHUTDOWN)
			/*
			 * We continue despite the error. There is not much we
			 * can do. If we don't clean it up we loop forever. If
			 * we skip the TRB then it gets overwritten after a
			 * while since we use them in a ring buffer. A BUG()
			 * would help. Lets hope that if this occurs, someone
			 * fixes the root cause instead of looking away :)
			 */
			dev_err(dwc->dev, "%s's TRB (%p) still owned by HW\n",
					dep->name, req->trb);

		count = trb->size & DWC3_TRB_SIZE_MASK;

		if (dep->direction) {
			if (count) {
				trb_status = DWC3_TRB_SIZE_TRBSTS(trb->size);
				if (trb_status == DWC3_TRBSTS_MISSED_ISOC) {
					dev_dbg(dwc->dev, "incomplete IN transfer %s\n",
							dep->name);
					dep->current_uf = event->parameters &
						~(dep->interval - 1);
					dep->flags |= DWC3_EP_MISSED_ISOC;
				} else {
					dev_err(dwc->dev, "incomplete IN transfer %s\n",
							dep->name);
					status = -ECONNRESET;
				}
			}
		} else {
			if (count && (event->status & DEPEVT_STATUS_SHORT))
				s_pkt = 1;
		}

        /* modified by wzs in 2013.2.6 start */
        if (req->request.zero)
            trb++;
        /* modified by wzs in 2013.2.6 end */

		/*
		 * We assume here we will always receive the entire data block
		 * which we should receive. Meaning, if we program RX to
		 * receive 4K but we receive only 2K, we assume that's all we
		 * should receive and we simply bounce the request back to the
		 * gadget driver for further processing.
		 */
		req->request.actual += req->request.length - count;
		dwc3_gadget_giveback(dep, req, status);
        dwc3_trb_trace_store(dep, ~0, __LINE__);
		if (s_pkt)
			break;
		if ((event->status & DEPEVT_STATUS_LST) &&
				(trb->ctrl & (DWC3_TRB_CTRL_LST |
						DWC3_TRB_CTRL_HWO)))
			break;
		if ((event->status & DEPEVT_STATUS_IOC) &&
				(trb->ctrl & DWC3_TRB_CTRL_IOC))
			break;
	} while (1);

	if ((event->status & DEPEVT_STATUS_IOC) &&
			(trb->ctrl & DWC3_TRB_CTRL_IOC))
		return 0;
	return 1;
}

static void dwc3_endpoint_transfer_complete(struct dwc3 *dwc,
		struct dwc3_ep *dep, const struct dwc3_event_depevt *event,
		int start_new)
{
	int		status = 0;
	int			clean_busy;

	if (event->status & DEPEVT_STATUS_BUSERR)
		status = -ECONNRESET;

	clean_busy = dwc3_cleanup_done_reqs(dwc, dep, event, status);
	if (clean_busy && start_new)
	{
        if(dep->endpoint.xfer_cmd_monitor && dep->timer)
        {
            dep->stat.monitor_stop++;
            (void)del_timer(dep->timer);
        }
        
		dep->flags &= ~DWC3_EP_BUSY;
        
	}

	/*
	 * WORKAROUND: This is the 2nd half of U1/U2 -> U0 workaround.
	 * See dwc3_gadget_linksts_change_interrupt() for 1st half.
	 */
	if (dwc->revision < DWC3_REVISION_183A) {
		u32		reg;
		int		i;

		for (i = 0; i < DWC3_ENDPOINTS_NUM; i++) {
			dep = dwc->eps[i];

			if (!(dep->flags & DWC3_EP_ENABLED))
				continue;

			if (!list_empty(&dep->req_queued))
				return;
		}

		reg = dwc3_readl(dwc->regs, DWC3_DCTL);
		reg |= dwc->u1u2;
		dwc3_writel(dwc->regs, DWC3_DCTL, reg);

		dwc->u1u2 = 0;
	}
}

static void dwc3_endpoint_interrupt(struct dwc3 *dwc,
		const struct dwc3_event_depevt *event)
{
	struct dwc3_ep		*dep;
    struct dwc3_event_stat_t *stat = &dwc->event_stat;
	u8			epnum = event->endpoint_number;
    u32 event_type = event->endpoint_event;
    u32 ts_tmp = 0;

	dep = dwc->eps[epnum];

	if (!(dep->flags & DWC3_EP_ENABLED))
		return;

	DWC3_TRACE(("%s: %s\n", dep->name,
			dwc3_ep_event_string(event->endpoint_event)));

	if (epnum == 0 || epnum == 1) {
		dwc3_ep0_interrupt(dwc, event);
        stat->ep0_event[event_type]++;
		return;
	}

    stat->ep_event[epnum-2][event_type]++;

	switch (event_type) {
	case DWC3_DEPEVT_XFERCOMPLETE:
		dep->resource_index = 0;

		if (usb_endpoint_xfer_isoc(dep->endpoint.desc)) {
			dev_dbg(dwc->dev, "%s is an Isochronous endpoint\n",
					dep->name);
			return;
		}

		dwc3_endpoint_transfer_complete(dwc, dep, event, 1);

        if (dep->endpoint.masknotready
            && !list_empty(&dep->request_list))
        {
            dep->stat.dokick[DWC3_EP_KICKSOURCE_COMPLETE]++;
            dep->stat.kick_pos = __LINE__;
            (void)__dwc3_gadget_kick_transfer(dep, 0, 1);
        }
		break;
	case DWC3_DEPEVT_XFERINPROGRESS:
        /* process the xferinprogress if the xferinprogress intr of
         * this endpoint is enabled.
         */
		if (!dep->endpoint.enable_xfer_in_progress
            && !usb_endpoint_xfer_isoc(dep->endpoint.desc)) {
			dev_dbg(dwc->dev, "%s is not an Isochronous endpoint\n",
					dep->name);
			return;
		}

		dwc3_endpoint_transfer_complete(dwc, dep, event, 0);
		break;
	case DWC3_DEPEVT_XFERNOTREADY:
		if (usb_endpoint_xfer_isoc(dep->endpoint.desc)) {
			dwc3_gadget_start_isoc(dwc, dep, event);
		} else {
			int ret;

			DWC3_TRACE(("%s: reason %s\n",
					dep->name, event->status &
					DEPEVT_STATUS_TRANSFER_ACTIVE
					? "Transfer Active"
					: "Transfer Not Active"));
			ret = __dwc3_gadget_kick_transfer(dep, 0, 1);
			if (!ret || ret == -EBUSY)
				return;

			dev_dbg(dwc->dev, "%s: failed to kick transfers\n",
					dep->name);
		}

		break;
	case DWC3_DEPEVT_STREAMEVT:
		if (!usb_endpoint_xfer_bulk(dep->endpoint.desc)) {
			dev_err(dwc->dev, "Stream event for non-Bulk %s\n",
					dep->name);
			return;
		}

		switch (event->status) {
		case DEPEVT_STREAMEVT_FOUND:
			dev_vdbg(dwc->dev, "Stream %d found and started\n",
					event->parameters);

			break;
		case DEPEVT_STREAMEVT_NOTFOUND:
			/* FALLTHROUGH */
		default:
			dev_dbg(dwc->dev, "Couldn't find suitable stream\n");
		}
		break;
	case DWC3_DEPEVT_RXTXFIFOEVT:
		dev_dbg(dwc->dev, "%s FIFO Overrun\n", dep->name);
		break;
	case DWC3_DEPEVT_EPCMDCMPLT:
		dev_vdbg(dwc->dev, "Endpoint Command Complete\n");

        ts_tmp = bsp_get_slice_value_hrt();
        dep->ts_cur = get_timer_slice_delta(dep->ts_cur, ts_tmp);

        if(dep->ts_cur > dep->ts_peek){
            dep->ts_peek = dep->ts_cur;
        }
        
        if((dep->ts_cur < dep->ts_min) || (!dep->ts_min)){
            dep->ts_min = dep->ts_cur;
        }

        dep->flags &= ~DWC3_EP_BUSY;
        if(!list_empty(&dep->request_list)){
            (void)__dwc3_gadget_ep_kick(dep, DWC3_EP_KICKSOURCE_EPCMDCMPLT);
        }
		break;
	}
}

static void dwc3_stop_active_transfer(struct dwc3 *dwc, u32 epnum)
{
	struct dwc3_ep *dep;
	struct dwc3_gadget_ep_cmd_params params;
	u32 cmd;
	int ret;

	dep = dwc->eps[epnum];

	if (!dep->resource_index)
		return;

	/*
	 * NOTICE: We are violating what the Databook says about the
	 * EndTransfer command. Ideally we would _always_ wait for the
	 * EndTransfer Command Completion IRQ, but that's causing too
	 * much trouble synchronizing between us and gadget driver.
	 *
	 * We have discussed this with the IP Provider and it was
	 * suggested to giveback all requests here, but give HW some
	 * extra time to synchronize with the interconnect. We're using
	 * an arbitraty 100us delay for that.
	 *
	 * Note also that a similar handling was tested by Synopsys
	 * (thanks a lot Paul) and nothing bad has come out of it.
	 * In short, what we're doing is:
	 *
	 * - Issue EndTransfer WITH CMDIOC bit set
	 * - Wait 100us
	 */

	cmd = DWC3_DEPCMD_ENDTRANSFER;
	cmd |= DWC3_DEPCMD_HIPRI_FORCERM | DWC3_DEPCMD_CMDIOC;
	cmd |= DWC3_DEPCMD_PARAM(dep->resource_index);
	memset(&params, 0, sizeof(params));
	ret = dwc3_send_gadget_ep_cmd(dwc, dep->number, cmd, &params);
	WARN_ON_ONCE(ret);
	dep->resource_index = 0;

	/* dep->flags &= ~DWC3_EP_BUSY; */

    dep->ts_cur = bsp_get_slice_value_hrt();

	udelay(100);
}/*lint !e550*/

static void dwc3_stop_active_transfers(struct dwc3 *dwc)
{
	u32 epnum;

	for (epnum = 2; epnum < DWC3_ENDPOINTS_NUM; epnum++) {
		struct dwc3_ep *dep;

		dep = dwc->eps[epnum];
		if (!(dep->flags & DWC3_EP_ENABLED))
			continue;

		dwc3_remove_requests(dwc, dep);
	}
}

static void dwc3_clear_stall_all_ep(struct dwc3 *dwc)
{
	u32 epnum;

	for (epnum = 1; epnum < DWC3_ENDPOINTS_NUM; epnum++) {
		struct dwc3_ep *dep;
		struct dwc3_gadget_ep_cmd_params params;
		int ret;

		dep = dwc->eps[epnum];

		if (!(dep->flags & DWC3_EP_STALL))
			continue;

		dep->flags &= ~DWC3_EP_STALL;

		memset(&params, 0, sizeof(params));
		ret = dwc3_send_gadget_ep_cmd(dwc, dep->number,
				DWC3_DEPCMD_CLEARSTALL, &params);
		WARN_ON_ONCE(ret);
	}/*lint !e550*/
}

void dwc3_gadget_usb2_phy_init(struct dwc3 *dwc)
{
}

void dwc3_gadget_usb3_phy_init(struct dwc3 *dwc)
{
	u32			reg;

#ifdef USB3_SYNOPSYS_PHY
	reg = dwc3_readl(dwc->regs, DWC3_GUSB3PIPECTL(0));
    reg |= DWC3_GUSB3PIPECTL_LFPS | USB3_GUSB3PIPECTL_TX_DEMPH;
	dwc3_writel(dwc->regs, DWC3_GUSB3PIPECTL(0), reg);
#endif
}

static void dwc3_gadget_reset_interrupt(struct dwc3 *dwc)
{
	u32			reg;

    dwc3_enable_both_phy();

	USB_DBG_DWC3(dwc->dev, "%s\n", __func__);

	/*
	 * WORKAROUND: DWC3 revisions <1.88a have an issue which
	 * would cause a missing Disconnect Event if there's a
	 * pending Setup Packet in the FIFO.
	 *
	 * There's no suggested workaround on the official Bug
	 * report, which states that "unless the driver/application
	 * is doing any special handling of a disconnect event,
	 * there is no functional issue".
	 *
	 * Unfortunately, it turns out that we _do_ some special
	 * handling of a disconnect event, namely complete all
	 * pending transfers, notify gadget driver of the
	 * disconnection, and so on.
	 *
	 * Our suggested workaround is to follow the Disconnect
	 * Event steps here, instead, based on a setup_packet_pending
	 * flag. Such flag gets set whenever we have a XferNotReady
	 * event on EP0 and gets cleared on XferComplete for the
	 * same endpoint.
	 *
	 * Refers to:
	 *
	 * STAR#9000466709: RTL: Device : Disconnect event not
	 * generated if setup packet pending in FIFO
	 */
	if (dwc->revision < DWC3_REVISION_188A) {
		if (dwc->setup_packet_pending)
			dwc3_gadget_disconnect_interrupt(dwc);
	}

	/* after reset -> Default State */
	dwc->dev_state = DWC3_DEFAULT_STATE;

    /* clear the suspend & remote wakeup state */
    dwc->gadget.is_suspend = 0;
    dwc->gadget.rwakeup = 0;
    enumerated_done = 0;

	/* Recent versions support automatic phy suspend and don't need this */
	if (dwc->revision < DWC3_REVISION_194A) {
		/* Resume PHYs */
		dwc3_gadget_usb2_phy_suspend(dwc, false);
		dwc3_gadget_usb3_phy_suspend(dwc, false);
	}

	if (dwc->gadget.speed != USB_SPEED_UNKNOWN)
		dwc3_disconnect_gadget(dwc);

	reg = dwc3_readl(dwc->regs, DWC3_DCTL);
	reg &= ~DWC3_DCTL_TSTCTRL_MASK;
	dwc3_writel(dwc->regs, DWC3_DCTL, reg);
	dwc->test_mode = false;

	dwc3_stop_active_transfers(dwc);
	dwc3_clear_stall_all_ep(dwc);
	dwc->start_config_issued = false;

	/* Reset device address to zero */
	reg = dwc3_readl(dwc->regs, DWC3_DCFG);
	reg &= ~(DWC3_DCFG_DEVADDR_MASK);
	dwc3_writel(dwc->regs, DWC3_DCFG, reg);
}

static void dwc3_update_ram_clk_sel(struct dwc3 *dwc, u32 speed)/* [false alarm]:Disable fortify false alarm */
{
	u32 reg;/* [false alarm]:Disable fortify false alarm */
	u32 usb30_clock = DWC3_GCTL_CLK_BUS;

	/*
	 * We change the clock only at SS but I dunno why I would want to do
	 * this. Maybe it becomes part of the power saving plan.
	 */

	if (speed != DWC3_DSTS_SUPERSPEED)
		return;

	/*
	 * RAMClkSel is reset to 0 after USB reset, so it must be reprogrammed
	 * each time on Connect Done.
	 */
	if (!usb30_clock)/* [false alarm]:Disable fortify false alarm */
		return;
	/* coverity[dead_error_begin] */
	reg = dwc3_readl(dwc->regs, DWC3_GCTL);/* [false alarm]:Disable fortify false alarm */
	reg |= DWC3_GCTL_RAMCLKSEL(usb30_clock);/* [false alarm]:Disable fortify false alarm */
	dwc3_writel(dwc->regs, DWC3_GCTL, reg);/* [false alarm]:Disable fortify false alarm */
}

static void dwc3_gadget_phy_suspend(struct dwc3 *dwc, u8 speed)
{
	switch (speed) {
	case USB_SPEED_SUPER:
		dwc3_gadget_usb2_phy_suspend(dwc, true);
		break;
	case USB_SPEED_HIGH:
	case USB_SPEED_FULL:
	case USB_SPEED_LOW:
		dwc3_gadget_usb3_phy_suspend(dwc, true);
		break;
	}
}

static void dwc3_gadget_conndone_interrupt(struct dwc3 *dwc)
{
	struct dwc3_ep		*dep;
	int			ret;
	u32			reg;
	u8			speed;

	USB_DBG_DWC3(dwc->dev, "%s\n", __func__);

	reg = dwc3_readl(dwc->regs, DWC3_DSTS);
	speed = reg & DWC3_DSTS_CONNECTSPD;
	dwc->speed = speed;

	dwc3_update_ram_clk_sel(dwc, speed);

	switch (speed) {
	case DWC3_DCFG_SUPERSPEED:
		/*
		 * WORKAROUND: DWC3 revisions <1.90a have an issue which
		 * would cause a missing USB3 Reset event.
		 *
		 * In such situations, we should force a USB3 Reset
		 * event by calling our dwc3_gadget_reset_interrupt()
		 * routine.
		 *
		 * Refers to:
		 *
		 * STAR#9000483510: RTL: SS : USB3 reset event may
		 * not be generated always when the link enters poll
		 */
		if (dwc->revision < DWC3_REVISION_190A)
			dwc3_gadget_reset_interrupt(dwc);

		dwc3_gadget_ep0_desc.wMaxPacketSize = cpu_to_le16(512);
		dwc->gadget.ep0->maxpacket = 512;
		dwc->gadget.speed = USB_SPEED_SUPER;
		break;
	case DWC3_DCFG_HIGHSPEED:
		dwc3_gadget_ep0_desc.wMaxPacketSize = cpu_to_le16(64);
		dwc->gadget.ep0->maxpacket = 64;
		dwc->gadget.speed = USB_SPEED_HIGH;
		break;
	case DWC3_DCFG_FULLSPEED2:
	case DWC3_DCFG_FULLSPEED1:
		dwc3_gadget_ep0_desc.wMaxPacketSize = cpu_to_le16(64);
		dwc->gadget.ep0->maxpacket = 64;
		dwc->gadget.speed = USB_SPEED_FULL;
		break;
	case DWC3_DCFG_LOWSPEED:
		dwc3_gadget_ep0_desc.wMaxPacketSize = cpu_to_le16(8);
		dwc->gadget.ep0->maxpacket = 8;
		dwc->gadget.speed = USB_SPEED_LOW;
		break;
	}

	/* Recent versions support automatic phy suspend and don't need this */
	if (dwc->revision < DWC3_REVISION_194A) {
		/* Suspend unneeded PHY */
		dwc3_gadget_phy_suspend(dwc, dwc->gadget.speed);
	}

	dep = dwc->eps[0];
	ret = __dwc3_gadget_ep_enable(dep, &dwc3_gadget_ep0_desc, NULL, true);
	if (ret) {
		dev_err(dwc->dev, "failed to enable %s\n", dep->name);
		return;
	}

	dep = dwc->eps[1];
	ret = __dwc3_gadget_ep_enable(dep, &dwc3_gadget_ep0_desc, NULL, true);
	if (ret) {
		dev_err(dwc->dev, "failed to enable %s\n", dep->name);
		return;
	}

    dwc->link_state = DWC3_LINK_STATE_RESET;

	/*
	 * Configure PHY via GUSB3PIPECTLn if required.
	 *
	 * Update GTXFIFOSIZn
	 *
	 * In both cases reset values should be sufficient.
	 */
}
static inline void dwc3_gadget_resume(struct dwc3 *dwc)
{
    dev_vdbg(dwc->dev, "%s\n", __func__);

    if (dwc->gadget_driver->resume) {
        dwc->gadget_driver->resume(&dwc->gadget);
    }

    /* clear stall all the eps */
    dwc3_clear_stall_all_ep(dwc);
}
static void dwc3_gadget_wakeup_interrupt(struct dwc3 *dwc)
{
    enum dwc3_link_state state = DWC3_LINK_STATE_U0;
    unsigned long		timeout;

    /* do the resume job really when the gadget is configured */
    if (dwc->dev_state != DWC3_CONFIGURED_STATE) {
        dev_vdbg(dwc->dev, "%s dev_state = %d,not DWC3_CONFIGURED_STATE \n",
            __func__, (int)dwc->dev_state);
        return;
    }

	/*
	 * TODO take core out of low power mode when that's
	 * implemented.
	 */
	 /* poll until Link State changes to U0 */
	timeout = jiffies + msecs_to_jiffies(100);

	while (!time_after(jiffies, timeout)) {
		state = dwc3_gadget_get_link_state(dwc);

		if (state == DWC3_LINK_STATE_U0)
			break;
	}

	if (state != DWC3_LINK_STATE_U0) {
		dev_err(dwc->dev, "timeout to wait for the link status reg update\n");
		state = DWC3_LINK_STATE_U0;
	}

    dev_vdbg(dwc->dev, "%s: link state:%d -> %d\n", __func__,
        (int)dwc->link_state,(int)state);

    dwc->link_state = state;

    if (dwc->gadget.is_suspend) {
        dwc3_gadget_resume(dwc);
    }

    dwc->gadget.is_suspend = 0;
}/*lint !e550*/

static inline void dwc3_gadget_suspend(struct dwc3 *dwc)
{
    /* do the suspend job really when the gadget enumeration done */
    if (!is_dwc3_enumerated()) {
        USB_DBG_DWC3(dwc->dev, "suspend received before enum done.\n");
        return;
    }

    /* We can't rely on the link state machine to decide
    whether to do the suspend. As link state change intr
    would modify the dwc->link_state if enabled */
    if (dwc->gadget.is_suspend) {
        dev_warn(dwc->dev, "usb gadget device already suspended\n");
        return;
    }

    dev_vdbg(dwc->dev, "%s\n", __func__);

    if (dwc->gadget_driver->suspend) {
        dwc->gadget_driver->suspend(&dwc->gadget);
    }

    /* stop all transfers */
    dwc3_stop_active_transfers(dwc);

    dwc->gadget.is_suspend = 1;
}

static void dwc3_gadget_linksts_change_interrupt(struct dwc3 *dwc,
		unsigned int evtinfo)
{
	enum dwc3_link_state	next = (enum dwc3_link_state)(evtinfo & DWC3_LINK_STATE_MASK);

	/*
	 * WORKAROUND: DWC3 Revisions <1.83a have an issue which, depending
	 * on the link partner, the USB session might do multiple entry/exit
	 * of low power states before a transfer takes place.
	 *
	 * Due to this problem, we might experience lower throughput. The
	 * suggested workaround is to disable DCTL[12:9] bits if we're
	 * transitioning from U1/U2 to U0 and enable those bits again
	 * after a transfer completes and there are no pending transfers
	 * on any of the enabled endpoints.
	 *
	 * This is the first half of that workaround.
	 *
	 * Refers to:
	 *
	 * STAR#9000446952: RTL: Device SS : if U1/U2 ->U0 takes >128us
	 * core send LGO_Ux entering U0
	 */
	if (dwc->revision < DWC3_REVISION_183A) {
		if (next == DWC3_LINK_STATE_U0) {
			u32	u1u2;
			u32	reg;

			switch (dwc->link_state) {
			case DWC3_LINK_STATE_U1:
			case DWC3_LINK_STATE_U2:
				reg = dwc3_readl(dwc->regs, DWC3_DCTL);
				u1u2 = reg & (DWC3_DCTL_INITU2ENA
						| DWC3_DCTL_ACCEPTU2ENA
						| DWC3_DCTL_INITU1ENA
						| DWC3_DCTL_ACCEPTU1ENA);

				if (!dwc->u1u2)
					dwc->u1u2 = reg & u1u2;

				reg &= ~u1u2;

				dwc3_writel(dwc->regs, DWC3_DCTL, reg);
				break;
			default:
				/* do nothing */
				break;
			}
		}
	}

    if (next == DWC3_LINK_STATE_U3 && dwc->link_state != DWC3_LINK_STATE_U3) {
        dev_vdbg(dwc->dev, "%s: link state:%d -> %d\n", __func__,
        (int)dwc->link_state,(int)next);
        dwc3_gadget_suspend(dwc);
    }

	dwc->link_state = next;

	DWC3_INFO(("%s link %d\n", __func__, dwc->link_state));
}

static void dwc3_gadget_suspend_interrupt(struct dwc3 *dwc)
{
    enum dwc3_link_state state;

    /* check the link state */
    state = dwc3_gadget_get_link_state(dwc);

    USB_DBG_DWC3(dwc->dev, "%s: link state:%d -> %d\n", __func__,
        (int)dwc->link_state,(int)state);

    if (state != DWC3_LINK_STATE_U3 || dwc->link_state == DWC3_LINK_STATE_U3) {
        return;
    }

    dwc->link_state = state;

    dwc3_gadget_suspend(dwc);
}

static void dwc3_gadget_interrupt(struct dwc3 *dwc,
		const struct dwc3_event_devt *event)
{
    struct dwc3_event_stat_t *stat = &dwc->event_stat;

	switch (event->type) {
	case DWC3_DEVICE_EVENT_DISCONNECT:
		dwc3_gadget_disconnect_interrupt(dwc);
		break;
	case DWC3_DEVICE_EVENT_RESET:
		dwc3_gadget_reset_interrupt(dwc);
		break;
	case DWC3_DEVICE_EVENT_CONNECT_DONE:
		dwc3_gadget_conndone_interrupt(dwc);
		break;
	case DWC3_DEVICE_EVENT_WAKEUP:
        dev_vdbg(dwc->dev, "resume event received\n");
		dwc3_gadget_wakeup_interrupt(dwc);
		break;
	case DWC3_DEVICE_EVENT_LINK_STATUS_CHANGE:
		dwc3_gadget_linksts_change_interrupt(dwc, event->event_info);
		break;
	case DWC3_DEVICE_EVENT_SUSPEND:
		USB_DBG_DWC3(dwc->dev, "suspend event received\n");
        dwc3_gadget_suspend_interrupt(dwc);
		break;
	case DWC3_DEVICE_EVENT_SOF:
		dev_vdbg(dwc->dev, "Start of Periodic Frame\n");
		break;
	case DWC3_DEVICE_EVENT_ERRATIC_ERROR:
		dev_vdbg(dwc->dev, "Erratic Error\n");
		break;
	case DWC3_DEVICE_EVENT_CMD_CMPL:
		dev_vdbg(dwc->dev, "Command Complete\n");
		break;
	case DWC3_DEVICE_EVENT_OVERFLOW:
		dev_vdbg(dwc->dev, "Overflow\n");
		break;
	default:
		dev_dbg(dwc->dev, "UNKNOWN IRQ %d\n", event->type);
        stat->device_event[DWC3_DEVICE_EVENT_UNKNOWNN]++;
        return;
	}
    stat->device_event[event->type]++;
}

static void dwc3_process_event_entry(struct dwc3 *dwc,
		const union dwc3_event *event)
{
	/* Endpoint IRQ, handle it and return early */
	if (event->type.is_devspec == 0) {
		/* depevt */
        dwc3_endpoint_interrupt(dwc, &event->depevt);
		return ;
	}

	switch (event->type.type) {
	case DWC3_EVENT_TYPE_DEV:
		dwc3_gadget_interrupt(dwc, &event->devt);
		break;
	/* REVISIT what to do with Carkit and I2C events ? */
	default:
		dev_err(dwc->dev, "UNKNOWN IRQ type %d\n", event->raw);
	}
}

void dwc3_ep_show(struct dwc3 *dwc, u32 ep_number)
{
    u32 cnt = 0;
    unsigned long flags = 0;
    u32 request_list = 0;
    u32 req_queued = 0;
    struct dwc3_ep *ep;
    struct list_head *pos;
    int i;

    printk("|-+dwc3 ep dump    :\n");

    spin_lock_irqsave(&dwc->lock, flags);

    printk("| |--ep enable reg :0x%08x\n",dwc3_readl(dwc->regs, DWC3_DALEPENA));
    for(cnt=0; cnt<DWC3_ENDPOINTS_NUM; cnt++)
    {
        if(ep_number < DWC3_ENDPOINTS_NUM)
        {
            cnt = ep_number;
        }

        ep = dwc->eps[cnt];

        request_list = 0;
        list_for_each(pos, &ep->request_list)
        {
            request_list++;
        }

        req_queued= 0;
        list_for_each(pos, &ep->req_queued)
        {
            req_queued++;
        }

        printk("| |--ep %d dump             :\n",ep->number);
        printk("| |--name                   :%s\n",ep->name);
        printk("| |--direction              :%u\n",ep->direction);
        printk("| |--flags                  :%x\n",ep->flags);
        printk("| |--type                   :%u\n",ep->type);
        printk("| |--request_list           :%u\n",request_list);
        printk("| |--req_queued             :%u\n",req_queued);
        printk("| |--trb_pool               :0x%08x\n",(u32)ep->trb_pool);
        printk("| |--current_trb            :%u\n",ep->current_trb);
        printk("| |--busy_slot              :%u\n",ep->busy_slot);
        printk("| |--free_slot              :%u\n",ep->free_slot);
        printk("| |--resource_index         :%u\n",ep->resource_index);
        printk("| |--stream_capable         :%u\n",ep->stream_capable);
        printk("| |--ts_peek                :%u\n",ep->ts_peek);
        printk("| |--ts_min                 :%u\n",ep->ts_min);
        printk("| |--ts_cur                 :%u\n",ep->ts_cur);
        printk("| |--DEPCMDPAR2             :0x%08x\n",(u32)dwc3_readl(dwc->regs, DWC3_DEPCMDPAR2(cnt)));
        printk("| |--DEPCMDPAR1             :0x%08x\n",(u32)dwc3_readl(dwc->regs, DWC3_DEPCMDPAR1(cnt)));
        printk("| |--DEPCMDPAR0             :0x%08x\n",(u32)dwc3_readl(dwc->regs, DWC3_DEPCMDPAR0(cnt)));
        printk("| |--DEPCMD                 :0x%08x\n",(u32)dwc3_readl(dwc->regs, DWC3_DEPCMD(cnt)));
        printk("| |--stat.kick_busy         :%d\n",ep->stat.kick_busy);
        printk("| |--stat.kick_none         :%d\n",ep->stat.kick_none);
        printk("| |--stat.kick_cmderr       :%d\n",ep->stat.kick_cmderr);
        printk("| |--stat.kick_ok           :%d\n",ep->stat.kick_ok);
        printk("| |--stat.trb_zero          :%d\n",ep->stat.trb_zero);
        printk("| |--stat.trb_used          :%d\n",ep->stat.trb_used);
        printk("| |--stat.trb_lack          :%d\n",ep->stat.trb_lack);
        printk("| |--stat.cleanup_warn      :%d\n",ep->stat.cleanup_warn);
        printk("| |--stat.kick_pos          :%d\n",ep->stat.kick_pos);
        printk("| |--stat.enable_cnt        :%d\n",ep->stat.enable_cnt);
        printk("| |--stat.disable_cnt       :%d\n",ep->stat.disable_cnt);
        printk("| |--stat.dequeue_cnt       :%d\n",ep->stat.dequeue_cnt);
        printk("| |--stat.sethalt_cnt       :%d\n",ep->stat.sethalt_cnt);
        printk("| |--stat.setwedge_cnt      :%d\n",ep->stat.setwedge_cnt);
        printk("| |--stat.monitor_timeout   :%d\n",ep->stat.monitor_timeout);
        printk("| |--stat.monitor_start     :%d\n",ep->stat.monitor_start);
        printk("| |--stat.monitor_stop      :%d\n",ep->stat.monitor_stop);
        printk("| |--stat.monitor_enable    :%d\n",ep->stat.monitor_enable);
        printk("| |--stat.monitor_disable   :%d\n",ep->stat.monitor_disable);

        for (i=0;i<DWC3_EP_KICKSOURCE_BOTTOM;i++)
        {
            printk("| |--stat.dokick[%d]        :%d\n",i,ep->stat.dokick[i]);
            printk("| |--stat.nokick[%d]        :%d\n",i,ep->stat.nokick[i]);
            printk("| |--stat.eventkick[%d]     :%d\n",i,ep->stat.eventkick[i]);
            printk("| |--stat.zerokick[%d]      :%d\n",i,ep->stat.zerokick[i]);
        }

        printk("\n");

        if(ep_number < DWC3_ENDPOINTS_NUM)
        {
            break;
        }
    }
    spin_unlock_irqrestore(&dwc->lock, flags);
}

void dwc3_event_stat_show(struct dwc3 *dwc)
{
    struct dwc3_event_stat_t *stat = &dwc->event_stat;
    u32 cnt,epnum;

    printk("dwc3 event stat info:\n");
    printk("  intr_total: %d, ehandle_total: %d, loops_total: %d, events_total: %d\n",
        stat->intr_total,stat->ehandle_total, stat->loops_total,stat->events_total);

    printk("  device envent statis as below:\n");
    for (cnt=0; cnt<(DWC3_DEVICE_EVENT_BOTTOM); cnt++)
    {
        printk("[%u %u] ",cnt, stat->device_event[cnt]);
    }
    printk("\n");

    printk("  ep0 envent statis as below:\n");
    for (cnt=0; cnt<(DWC3_DEPEVT_BOTTOM); cnt++)
    {
        printk("[%u %u] ",cnt, stat->ep0_event[cnt]);
    }
    printk("\n");

    printk("  ep envent statis as below:\n");
    for (epnum=0; epnum < (DWC3_ENDPOINTS_NUM-2); epnum++)
    {
        printk("ep%d:",(epnum+2));
        for (cnt=0; cnt<(DWC3_DEPEVT_BOTTOM); cnt++)
        {
            printk("[%u %u] ", cnt, stat->ep_event[epnum][cnt]);
        }
        printk("\n");
    }
    printk("\n");

    return ;
}

static irqreturn_t dwc3_process_event_buf(struct dwc3 *dwc, u32 buf)
{
	struct dwc3_event_buffer *evt;
	int left;
	u32 count;
    irqreturn_t ret = IRQ_NONE;

    dwc3_intr_disable(dwc);

    while(1)
    {
    	count = dwc3_readl(dwc->regs, DWC3_GEVNTCOUNT(buf));
    	count &= DWC3_GEVNTCOUNT_MASK;
    	if (!count)
    		break;

        dwc->event_stat.loops_total++;
        dwc->event_stat.events_total += count>>2;

    	evt = dwc->ev_buffs[buf];
    	left = count;

    	while (left > 0) {
    		union dwc3_event event;

    		event.raw = *(u32 *) ((u32)evt->buf + evt->lpos);

    		dwc3_process_event_entry(dwc, &event);
    		/*
    		 * XXX we wrap around correctly to the next entry as almost all
    		 * entries are 4 bytes in size. There is one entry which has 12
    		 * bytes which is a regular entry followed by 8 bytes data. ATM
    		 * I don't know how things are organized if were get next to the
    		 * a boundary so I worry about that once we try to handle that.
    		 */
    		evt->lpos = (evt->lpos + 4) % DWC3_EVENT_BUFFERS_SIZE;/*lint !e123*/
    		left -= 4;
    	}
        dwc3_writel(dwc->regs, DWC3_GEVNTCOUNT(buf), count);

        ret = IRQ_HANDLED;

        if(dwc->event_stat.single_intr)
        {
            break;
        }
    }

    dwc3_intr_enable(dwc);

	return ret;
}

/* called in intr context */
void dwc3_intr_enable(struct dwc3 *dwc)
{
   u32 value;
   u32 i;

   for (i = 0; i < dwc->num_event_buffers; i++) {
        value = dwc3_readl(dwc->regs, DWC3_GEVNTSIZ(i));
        /* set b31 to 0 to enbale the intr being generated */
        value &= ~(1<<31);
        dwc3_writel(dwc->regs, DWC3_GEVNTSIZ(i), value);
   }
}

void dwc3_intr_disable(struct dwc3 *dwc)
{
    u32 value;
    u32 i;

    for (i = 0; i < dwc->num_event_buffers; i++) {
        value = dwc3_readl(dwc->regs, DWC3_GEVNTSIZ(i));
        /* set b31 to 1 to prevent intr being generated, while events are queued */
        value |= 1<<31;
        dwc3_writel(dwc->regs, DWC3_GEVNTSIZ(i), value);
    }
}

int dwc3_event_process(struct dwc3 * dwc)
{
    u32				i;
    irqreturn_t			ret = IRQ_NONE;

    dwc->event_stat.ehandle_total++;

	for (i = 0; i < dwc->num_event_buffers; i++) {
		irqreturn_t status;

		status = dwc3_process_event_buf(dwc, i);
		if (status == IRQ_HANDLED)
			ret = status;
	}

    return ret;
}

#ifdef DWC3_EVENT_TASKLET
void dwc3_event_handler(unsigned long data)
{
    struct dwc3	*dwc = (struct dwc3	*)data;
    unsigned long flags;

    spin_lock_irqsave(&dwc->lock,flags);

    dwc3_event_process(dwc);

    dwc3_intr_enable(dwc);

    spin_unlock_irqrestore(&dwc->lock,flags);
}
#endif

#ifdef CONFIG_USB_OTG_DWC
void dwc3_start_hnp(struct dwc3 *dwc)
{
	struct usb_phy *transceiver = usb_get_transceiver();

	if (!transceiver || !transceiver->otg)
		return;

	otg_start_hnp(transceiver->otg);
	usb_put_transceiver(transceiver);
}

void dwc3_host_release(struct dwc3 *dwc)
{
	struct usb_phy *transceiver = usb_get_transceiver();
	struct usb_otg *otg;

	if (!transceiver || !transceiver->otg)
		return;

	otg = transceiver->otg;
	otg->host_release(otg);
	usb_put_transceiver(transceiver);
}

static ssize_t store_srp(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct usb_phy *transceiver = usb_get_transceiver();

	if (!transceiver || !transceiver->otg)
		return count;

	otg_start_srp(transceiver->otg);
	usb_put_transceiver(transceiver);
	return count;
}
static DEVICE_ATTR(srp, 0222, NULL, store_srp);

static ssize_t store_end(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct usb_phy *transceiver = usb_get_transceiver();
    struct usb_otg *otg;

	if (!transceiver || !transceiver->otg)
		return count;

    otg = transceiver->otg;
	otg->end_session(otg);
	usb_put_transceiver(transceiver);
	return count;
}
static DEVICE_ATTR(end, 0222, NULL, store_end);

static ssize_t store_hnp(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
    struct dwc3 *dwc = container_of(dev, struct dwc3, gadget.dev);

	if (dwc->b_hnp_enable) {
		dwc->b_hnp_enable = 0;
		dwc->wants_host = 0;
		dwc3_start_hnp(dwc);
	} else {
		dwc->wants_host = 1;
		/* TODO if we don't receive the SET_FEATURE within 4 secs,
		 * reset this value
		 */
	}
	return count;
}
static DEVICE_ATTR(hnp, 0222, NULL, store_hnp);

static ssize_t store_rsp(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct usb_phy *transceiver = usb_get_transceiver();
    struct usb_otg *otg;

	if (!transceiver || !transceiver->otg)
		return count;

    otg = transceiver->otg;
	otg->start_rsp(otg);
	usb_put_transceiver(transceiver);
	return count;
}
static DEVICE_ATTR(rsp, 0222, NULL, store_rsp);

#endif

static ssize_t
dwc3_show(struct device *pdev, struct device_attribute *attr, char *buf)
{
    int count;
    struct dwc3 *dwc = container_of(pdev, struct dwc3, gadget.dev);
	/* coverity[secure_coding] */
    count = sprintf(buf, "|-+dwc3 info:\n");
	/* coverity[secure_coding] */
    count += sprintf(buf + count, "| |--dwc3_msg_level          :0x%08x\n",(u32)dwc3_msg_level);
/* coverity[secure_coding] */
	count += sprintf(buf + count, "| |--stat.cmd_err            :%d\n",dwc->stat.cmd_err);
/* coverity[secure_coding] */
	count += sprintf(buf + count, "| |--dev_state               :%d\n",dwc->dev_state);
/* coverity[secure_coding] */
	count += sprintf(buf + count, "| |--is_suspend              :%d\n",dwc->gadget.is_suspend);
/* coverity[secure_coding] */
	count += sprintf(buf + count, "| |--speed                   :%d\n",dwc->gadget.speed);
/* coverity[secure_coding] */
	count += sprintf(buf + count, "| |--mode                    :%d\n",dwc->mode);

    return count;
}

static ssize_t
dwc3_store(struct device *pdev, struct device_attribute *attr, const char *buf, size_t size)
{
    unsigned int msg_level;
	/* coverity[secure_coding] */
    (void)sscanf(buf, "%x", &msg_level);

    dwc3_msg_level = (unsigned long)msg_level;

    return size;
}

static DEVICE_ATTR(dwc3_common, S_IRUGO | S_IWUSR, dwc3_show, dwc3_store);

#if defined(CONFIG_DWC3_INTERRUPT_GATHER)
static ssize_t
intr_gather_show(struct device *pdev, struct device_attribute *attr, char *buf)
{
    struct dwc3 *dwc = container_of(pdev, struct dwc3, gadget.dev);

    printk("|-+intr gather info:\n");
    printk("| |--enumerated_done       :%d\n",dwc->intr_gather_manip.enumerated_done);
    printk("| |--isConfigure           :%d\n",dwc->intr_gather_manip.timer_configured);
    printk("| |--timerId               :%d\n",dwc->intr_gather_timer.timerId);
    printk("| |--timeout               :%d\n",dwc->intr_gather_timer.timeout);
    printk("| |--stat.timer_failed     :%d\n",dwc->intr_gather_stat.timer_failed);
    printk("| |--stat.intr_count       :%d\n",dwc->intr_gather_stat.intr_count);

    return 0;
}

static ssize_t
intr_gather_store(struct device *pdev, struct device_attribute *attr, const char *buf, size_t size)
{
    u32 timer_value,timer_ID,is_closed;
    struct dwc3 *dwc = container_of(pdev, struct dwc3, gadget.dev);

    sscanf(buf, "%d %d %d", &timer_value,&timer_ID,&is_closed);

    if (timer_value || timer_ID){
        dwc->intr_gather_manip.timer_configured = 0;

        if (bsp_hardtimer_free(dwc->intr_gather_timer.timerId))
            printk(KERN_ERR "***ERROR:old dwc3 interrupt gather timer free err***\n");

        if (timer_value)
            dwc->intr_gather_timer.timeout = timer_value;

        if (timer_ID)
            dwc->intr_gather_timer.timerId = timer_ID;

        if (bsp_hardtimer_alloc(&dwc->intr_gather_timer)) {
            printk(KERN_ERR "***ERROR:dwc3 interrupt gather timer realloc err***\n");
            return size;
        }

        dwc->intr_gather_manip.timer_configured = 1;

        bsp_hardtimer_enable(dwc->intr_gather_timer.timerId);
    }

    if (is_closed) {
        dwc->intr_gather_manip.timer_configured = 0;
        if (bsp_hardtimer_disable(DWC3_INTR_GATHER_TIMERID))
            printk(KERN_ERR "***ERROR:dwc3 interrupt gather timer disable err***\n");

        printk(KERN_WARNING "***WARNING:dwc interrupt gather timer closed***\n");
    }

    return size;
}

static DEVICE_ATTR(intr_gather_timer, S_IRUGO | S_IWUSR, intr_gather_show, intr_gather_store);

irqreturn_t dwc3_intr_gather_timeout(int irq, void *_dwc)
{
    struct dwc3			*dwc = _dwc;

    /* clear the timeout intr */
    bsp_hardtimer_disable(dwc->intr_gather_timer.timerId);

    dwc->intr_gather_stat.intr_count++;

    spin_lock(&dwc->lock);
    dwc3_intr_enable(dwc);
    spin_unlock(&dwc->lock);

    return IRQ_HANDLED;
}
#endif
int is_dwc3_enumerated(void)
{
    return (enumerated_done == 1);
}

int dwc3_phy_auto_powerdown(int enable)
{
    g_dwc_phy_auto_pwrdown = enable;
    return 0;
}

int dwc3_is_phy_auto_powerdown(void)
{
    return g_dwc_phy_auto_pwrdown;
}

static void dwc3_disable_phy_by_speed(struct dwc3 * dwc)
{
    u32         reg;
    u8          speed;

    if (g_dwc_phy_auto_pwrdown) {
        reg = dwc3_readl(dwc->regs, DWC3_DSTS);
        speed = reg & DWC3_DSTS_CONNECTSPD;

        /* usb3.0 powerdown hsp phy, usb2.0 powerdown ssp phy */
        if (DWC3_DCFG_SUPERSPEED == speed) {
            hi_syssc_usb_powerdown_hsp(1);
        }
        else {
            hi_syssc_usb_powerdown_ssp(1);
        }
    }
    return;
}

static void dwc3_enable_both_phy(void)
{
    if (g_dwc_phy_auto_pwrdown) {
        /* enable both usb2.0 and usb3.0 phy */
        hi_syssc_usb_powerdown_hsp(0);
        hi_syssc_usb_powerdown_ssp(0);
    }
}

static int change_enumeration(struct notifier_block *self,
    unsigned long action, void *data)
{
    unsigned long flags;
    struct dwc3 *dwc = the_dwc3;

    switch (action) {
        case USB_BALONG_ENUM_DONE:

            dwc3_disable_phy_by_speed(dwc);
            enumerated_done = 1;
            break;
        /* if vbus disconnect use the pmu remove interrupt */
        case USB_BALONG_DEVICE_REMOVE:
            if (dwc) {
                spin_lock_irqsave(&dwc->lock, flags);
                dwc3_gadget_disconnect_interrupt(dwc);
                spin_unlock_irqrestore(&dwc->lock, flags);
            }
            enumerated_done = 0;
            break;
        default:
            enumerated_done = 0;
            break;
    }
    return NOTIFY_OK;
}

static struct notifier_block intr_gather_nb = {
    .notifier_call = change_enumeration,
    .priority = USB_NOTIF_PRIO_HAL,
};

#if defined(CONFIG_GADGET_SUPPORT_REMOTE_WAKEUP)
static ssize_t
gadget_rwakeup_show(struct device *pdev, struct device_attribute *attr, char *buf)
{
    ssize_t count;
    struct dwc3 *dwc = container_of(pdev, struct dwc3, gadget.dev);

    count = sprintf(buf, "%d\n",dwc->rwakeup_en);

    return count;
}
static ssize_t
gadget_rwakeup_store(struct device *pdev, struct device_attribute *attr, const char *buf, size_t size)
{
    struct dwc3 *dwc = container_of(pdev, struct dwc3, gadget.dev);
    int gadget_wakeup_en;

    sscanf(buf, "%d", &gadget_wakeup_en);

    dwc->rwakeup_en = gadget_wakeup_en;

    return size;
}
static DEVICE_ATTR(remote_wakeup_enable, S_IRUGO | S_IWUSR, \
    gadget_rwakeup_show, gadget_rwakeup_store);
#endif
static ssize_t
dwc3_wakeup_show(struct device *pdev, struct device_attribute *attr, char *buf)
{
    ssize_t count;
    struct usb_gadget *gadget = container_of(pdev, struct usb_gadget, dev);
	/* coverity[secure_coding] */
    count = sprintf(buf, "%d\n",gadget->rwakeup);

    return count;
}
static ssize_t
dwc3_wakeup_store(struct device *pdev, struct device_attribute *attr, const char *buf, size_t size)
{
    struct dwc3 *dwc = container_of(pdev, struct dwc3, gadget.dev);

#if defined(CONFIG_GADGET_SUPPORT_REMOTE_WAKEUP)
    int wakeup;

    sscanf(buf, "%d", &wakeup);

    if (!wakeup) {
        dev_warn(dwc->dev,"Attention:Write 1 pls, if you want to try to wakeup \
            the host\n");
        return size;
    }

    if (dwc->gadget.rwakeup) {
        int ret;
        dev_dbg(dwc->dev, "try to wakeup the host\n");

        ret = dwc3_gadget_wakeup(&dwc->gadget);

        if (ret) {
            dev_dbg(dwc->dev,"dwc3_gadget_wakeup failed:%d\n",ret);
        } else {
            dev_dbg(dwc->dev,"succeed to invoke dwc3_gadget_wakeup\n");
        }
    }
    else {
        dev_warn(dwc->dev, "remote wakeup disabled by the host\n");
    }
#else
    dev_warn(dwc->dev, "GADGET_SUPPORT_REMOTE_WAKEUP is not defined\n");
#endif

    return size;
}
static DEVICE_ATTR(wakeup, S_IRUGO | S_IWUSR, \
    dwc3_wakeup_show, dwc3_wakeup_store);

static irqreturn_t dwc3_interrupt(int irq, void *_dwc)
{
	struct dwc3			*dwc = _dwc;
	unsigned int				i;
	irqreturn_t			ret = IRQ_NONE;

	spin_lock(&dwc->lock);

    dwc->event_stat.intr_total++;

#ifdef DWC3_EVENT_TASKLET
    dwc3_intr_disable(dwc);

    tasklet_schedule(&dwc->event_task);

    ret = IRQ_HANDLED;
#else
	for (i = 0; i < dwc->num_event_buffers; i++) {
		irqreturn_t status;

		status = dwc3_process_event_buf(dwc, i);
		if (status == IRQ_HANDLED)
			ret = status;
	}
#endif

#if defined(CONFIG_DWC3_INTERRUPT_GATHER)
    if (is_dwc3_enumerated() && dwc->intr_gather_manip.timer_configured) {
        /* disable the intr and start the 500us timer*/
        dwc3_intr_disable(dwc);

        /* return 0 if enabled success */
        if (unlikely(bsp_hardtimer_enable(dwc->intr_gather_timer.timerId))){
            dwc->intr_gather_stat.timer_failed++;
            dwc3_intr_enable(dwc);
            printk(KERN_ERR "***failed to enable the dwc3 intr gather timer***\n");
        }
    }
#endif

    spin_unlock(&dwc->lock);

	return ret;
}

/**
 * dwc3_gadget_init - Initializes gadget related registers
 * @dwc: pointer to our controller context structure
 *
 * Returns 0 on success otherwise negative errno.
 */
int __devinit dwc3_gadget_init(struct dwc3 *dwc)
{
	u32					reg;
	int					ret;
	int					irq;

	dwc->ctrl_req = dma_alloc_coherent(dwc->dev, sizeof(*dwc->ctrl_req),
			&dwc->ctrl_req_addr, GFP_KERNEL);
	if (!dwc->ctrl_req) {
		dev_err(dwc->dev, "failed to allocate ctrl request\n");
		ret = -ENOMEM;
		goto err0;
	}

	dwc->ep0_trb = dma_alloc_coherent(dwc->dev, sizeof(*dwc->ep0_trb) * 2,
			&dwc->ep0_trb_addr, GFP_KERNEL);
	if (!dwc->ep0_trb) {
		dev_err(dwc->dev, "failed to allocate ep0 trb\n");
		ret = -ENOMEM;
		goto err1;
	}

    /* BUGFIX: ZLP on non-endpoint0 */
	dwc->zlp = dma_alloc_coherent(dwc->dev, ZLP_MAX_PACKET_SIZE,
			&dwc->zlp_dma, GFP_KERNEL);
	if (!dwc->zlp) {
		dev_err(dwc->dev, "failed to allocate zlp\n");
		ret = -ENOMEM;
		goto err2;
	}

	dwc->setup_buf = kzalloc(DWC3_EP0_BOUNCE_SIZE, GFP_KERNEL);
	if (!dwc->setup_buf) {
		dev_err(dwc->dev, "failed to allocate setup buffer\n");
		ret = -ENOMEM;
		goto err_append0;
	}

	dwc->ep0_bounce = dma_alloc_coherent(dwc->dev,
			DWC3_EP0_BOUNCE_SIZE, &dwc->ep0_bounce_addr,
			GFP_KERNEL);
	if (!dwc->ep0_bounce) {
		dev_err(dwc->dev, "failed to allocate ep0 bounce buffer\n");
		ret = -ENOMEM;
		goto err3;
	}

	dev_set_name(&dwc->gadget.dev, "gadget");

#ifdef CONFIG_USB_OTG_DWC
    dwc->gadget.is_otg = 1;
#endif

	dwc->gadget.ops			= &dwc3_gadget_ops;
	dwc->gadget.max_speed		= USB_SPEED_SUPER;
	dwc->gadget.speed		= USB_SPEED_UNKNOWN;
	dwc->gadget.dev.parent		= dwc->dev;
	dwc->gadget.sg_supported	= true;

	dma_set_coherent_mask(&dwc->gadget.dev, dwc->dev->coherent_dma_mask);

	dwc->gadget.dev.dma_parms	= dwc->dev->dma_parms;
	dwc->gadget.dev.dma_mask	= dwc->dev->dma_mask;
	dwc->gadget.dev.release		= dwc3_gadget_release;
	dwc->gadget.name		= "dwc3-gadget";

	/*
	 * REVISIT: Here we should clear all pending IRQs to be
	 * sure we're starting from a well known location.
	 */

	ret = dwc3_gadget_init_endpoints(dwc);
	if (ret)
		goto err4;

#if defined(CONFIG_DWC3_INTERRUPT_GATHER)
    {
    struct bsp_hardtimer_control *timer = &dwc->intr_gather_timer;

    timer->timerId = DWC3_INTR_GATHER_TIMERID;
    timer->mode = TIMER_PERIOD_COUNT;
    timer->timeout = 5760; /* nearly 650us(5760/19.2) */
    timer->para = (u32)dwc; /* time out func parameter */
    timer->func = dwc3_intr_gather_timeout;
    if (bsp_hardtimer_alloc(timer)) {
        dev_err(dwc->dev, "failed to alloc hardtimer \n");
        goto err4;
    }

    dwc->intr_gather_manip.timer_configured = 0;
    }
#endif

    bsp_usb_register_notify(&intr_gather_nb);

	irq = platform_get_irq(to_platform_device(dwc->dev), 0);

#ifdef DWC3_EVENT_TASKLET
    tasklet_init(&dwc->event_task, dwc3_event_handler, (unsigned long)dwc);
#endif

	ret = request_irq(irq, dwc3_interrupt, IRQF_SHARED,
			"dwc3", dwc);
	if (ret) {
		dev_err(dwc->dev, "failed to request irq #%d --> %d\n",
				irq, ret);
		goto err5;
	}

	reg = dwc3_readl(dwc->regs, DWC3_DCFG);
	reg |= DWC3_DCFG_LPM_CAP;
	dwc3_writel(dwc->regs, DWC3_DCFG, reg);

#ifndef CONFIG_USB_OTG_DWC
	/* Enable all but Start and End of Frame IRQs */
	reg = (DWC3_DEVTEN_VNDRDEVTSTRCVEDEN |
			DWC3_DEVTEN_EVNTOVERFLOWEN |
			DWC3_DEVTEN_CMDCMPLTEN |
			DWC3_DEVTEN_ERRTICERREN |
			DWC3_DEVTEN_U3L2L1SUSPEN |
			DWC3_DEVTEN_WKUPEVTEN |
		/*	DWC3_DEVTEN_ULSTCNGEN | */
			DWC3_DEVTEN_CONNECTDONEEN |
			DWC3_DEVTEN_USBRSTEN);
	if (!bsp_usb_is_support_pmu_detect()) {
		/* disable the disconn intr if pmu supported */
		reg |= DWC3_DEVTEN_DISCONNEVTEN;
	}
	dwc3_writel(dwc->regs, DWC3_DEVTEN, reg);
#endif

	/* Enable USB2 LPM and automatic phy suspend only on recent versions */
	if (dwc->revision >= DWC3_REVISION_194A) {
		reg = dwc3_readl(dwc->regs, DWC3_DCFG);
		reg |= DWC3_DCFG_LPM_CAP;
		dwc3_writel(dwc->regs, DWC3_DCFG, reg);

		reg = dwc3_readl(dwc->regs, DWC3_DCTL);
		reg &= ~(DWC3_DCTL_HIRD_THRES_MASK | DWC3_DCTL_L1_HIBER_EN);

		/* TODO: This should be configurable */
		reg |= DWC3_DCTL_HIRD_THRES(28);

		dwc3_writel(dwc->regs, DWC3_DCTL, reg);

		dwc3_gadget_usb2_phy_suspend(dwc, false);
		dwc3_gadget_usb3_phy_suspend(dwc, false);
	}

	ret = device_register(&dwc->gadget.dev);
	if (ret) {
		dev_err(dwc->dev, "failed to register gadget device\n");
		put_device(&dwc->gadget.dev);
		goto err6;
	}

	ret = usb_add_gadget_udc(dwc->dev, &dwc->gadget);
	if (ret) {
		dev_err(dwc->dev, "failed to register udc\n");
		goto err7;
	}

#if defined(CONFIG_DWC3_INTERRUPT_GATHER)
    ret = device_create_file(&dwc->gadget.dev, &dev_attr_intr_gather_timer);
    if (ret < 0)
    {
		dev_err(&dwc->gadget.dev, "device_create_file intr_gather_timer failed, %d\n", ret);
		goto err8;
    }
#endif

#ifdef CONFIG_USB_OTG_DWC
	ret = device_create_file(&dwc->gadget.dev, &dev_attr_hnp);
	if (ret) {
		dev_err(&dwc->gadget.dev, "sysfs initialization failed!\n");
		goto err9;
	}

	ret = device_create_file(&dwc->gadget.dev, &dev_attr_rsp);
	if (ret) {
		dev_err(&dwc->gadget.dev, "sysfs initialization failed!\n");
		goto err10;
	}

	ret = device_create_file(&dwc->gadget.dev, &dev_attr_srp);
	if (ret) {
		dev_err(&dwc->gadget.dev, "sysfs initialization failed!\n");
		goto err11;
	}

	ret = device_create_file(&dwc->gadget.dev, &dev_attr_end);
	if (ret) {
		dev_err(&dwc->gadget.dev, "sysfs initialization failed!\n");
		goto err12;
	}
#endif

    ret = device_create_file(&dwc->gadget.dev, &dev_attr_dwc3_common);
    if(ret < 0)
    {
		dev_err(&dwc->gadget.dev, "device_create_file dwc3_common failed, %d\n", ret);
		goto err13;
    }

#if defined(CONFIG_GADGET_SUPPORT_REMOTE_WAKEUP)
    ret = device_create_file(&dwc->gadget.dev,&dev_attr_remote_wakeup_enable);
    if (ret < 0) {
        dev_err(&dwc->gadget.dev, "device_create_file remote_wakeup_enable failed, %d\n", ret);
        goto err14;
    }
#endif
    ret = device_create_file(&dwc->gadget.dev, &dev_attr_wakeup);
    if (ret < 0) {
        dev_err(&dwc->gadget.dev, "device_create_file wakeup failed, %d\n", ret);
        goto err15;
    }

    the_dwc3 = dwc;
	return 0;
err15:
#if defined(CONFIG_GADGET_SUPPORT_REMOTE_WAKEUP)
    device_remove_file(&dwc->gadget.dev, &dev_attr_remote_wakeup_enable);
err14:
#endif
    device_remove_file(&dwc->gadget.dev, &dev_attr_dwc3_common);
err13:
#ifdef CONFIG_USB_OTG_DWC
    device_remove_file(&dwc->gadget.dev, &dev_attr_end);
err12:
    device_remove_file(&dwc->gadget.dev, &dev_attr_srp);
err11:
    device_remove_file(&dwc->gadget.dev, &dev_attr_rsp);
err10:
    device_remove_file(&dwc->gadget.dev, &dev_attr_hnp);
err9:
#endif

#if defined(CONFIG_DWC3_INTERRUPT_GATHER)
    device_remove_file(&dwc->gadget.dev, &dev_attr_intr_gather_timer);
err8:
#endif

    usb_del_gadget_udc(&dwc->gadget);

err7:
	device_unregister(&dwc->gadget.dev);

err6:
	dwc3_writel(dwc->regs, DWC3_DEVTEN, 0x00);
	free_irq(irq, dwc);

err5:
	dwc3_gadget_free_endpoints(dwc);

err4:
	dma_free_coherent(dwc->dev, DWC3_EP0_BOUNCE_SIZE,
			dwc->ep0_bounce, dwc->ep0_bounce_addr);

err3:
	kfree(dwc->setup_buf);

    /* BUGFIX: ZLP on non-endpoint0 */
err_append0:
	dma_free_coherent(dwc->dev, ZLP_MAX_PACKET_SIZE,
			dwc->zlp, dwc->zlp_dma);

err2:
	dma_free_coherent(dwc->dev, sizeof(*dwc->ep0_trb),
			dwc->ep0_trb, dwc->ep0_trb_addr);

err1:
	dma_free_coherent(dwc->dev, sizeof(*dwc->ctrl_req),
			dwc->ctrl_req, dwc->ctrl_req_addr);

err0:
	return ret;
}

void dwc3_gadget_exit(struct dwc3 *dwc)
{
	int			irq;

    bsp_usb_unregister_notify(&intr_gather_nb);

	usb_del_gadget_udc(&dwc->gadget);
	/* coverity[check_return] */
	irq = platform_get_irq(to_platform_device(dwc->dev), 0);

	dwc3_writel(dwc->regs, DWC3_DEVTEN, 0x00);
	free_irq(irq, dwc);
#if defined(CONFIG_DWC3_INTERRUPT_GATHER)
    bsp_hardtimer_free(dwc->intr_gather_timer.timerId);
#endif
	dwc3_gadget_free_endpoints(dwc);

	dma_free_coherent(dwc->dev, DWC3_EP0_BOUNCE_SIZE,
			dwc->ep0_bounce, dwc->ep0_bounce_addr);

	kfree(dwc->setup_buf);

    /* BUGFIX: ZLP on non-endpoint0 */
	dma_free_coherent(dwc->dev, ZLP_MAX_PACKET_SIZE,
			dwc->zlp, dwc->zlp_dma);

	dma_free_coherent(dwc->dev, sizeof(*dwc->ep0_trb),
			dwc->ep0_trb, dwc->ep0_trb_addr);

	dma_free_coherent(dwc->dev, sizeof(*dwc->ctrl_req),
			dwc->ctrl_req, dwc->ctrl_req_addr);

#ifdef CONFIG_USB_OTG_DWC
    device_remove_file(&dwc->gadget.dev, &dev_attr_srp);
    device_remove_file(&dwc->gadget.dev, &dev_attr_rsp);
    device_remove_file(&dwc->gadget.dev, &dev_attr_hnp);
    device_remove_file(&dwc->gadget.dev, &dev_attr_end);
#endif

    device_remove_file(&dwc->gadget.dev, &dev_attr_dwc3_common);
#if defined(CONFIG_DWC3_INTERRUPT_GATHER)
    device_remove_file(&dwc->gadget.dev, &dev_attr_intr_gather_timer);
#endif
	device_unregister(&dwc->gadget.dev);

    the_dwc3 = NULL;
}
/*lint -restore*/
