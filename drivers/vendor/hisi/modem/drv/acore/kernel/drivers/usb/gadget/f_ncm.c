/*
 * f_ncm.c -- USB CDC Network (NCM) link function driver
 *
 * Copyright (C) 2010 Nokia Corporation
 * Contact: Yauheni Kaliuta <yauheni.kaliuta@nokia.com>
 *
 * The driver borrows from f_ecm.c which is:
 *
 * Copyright (C) 2003-2005,2008 David Brownell
 * Copyright (C) 2008 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*lint -save -e30 -e51 -e57 -e574 -e516 -e958 -e539 -e550 -e64 -e115 -e128  -e124
-e142 -e438 -e527 -e529 -e530 -e533 -e650 -e651 -e665 -e666 -e701 -e713 -e718 -e730 -e732 
-e734 -e737 -e740 -e744 -e746 -e747 -e830*/
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/etherdevice.h>
#include <linux/crc32.h>
#include <linux/kthread.h>
#include <linux/usb/cdc.h>
#include <net/ip.h>

#ifdef CONFIG_ETH_BYPASS_MODE
#include "ncm_balong.h"
#endif

#include "drv_dump.h"
#include "bsp_dump.h"
#include "u_ether.h"
#include "bsp_ncm.h"

/*
 * This function is a "CDC Network Control Model" (CDC NCM) Ethernet link.
 * NCM is intended to be used with high-speed network attachments.
 *
 * Note that NCM requires the use of "alternate settings" for its data
 * interface.  This means that the set_alt() method has real work to do,
 * and also means that a get_alt() method is required.
 */

/* to trigger crc/non-crc ndp signature */

#define NCM_NDP_HDR_CRC_MASK	0x01000000
#define NCM_NDP_HDR_CRC		0x01000000
#define NCM_NDP_HDR_NOCRC	0x00000000

#define USB_NOTIFY_VENDOR_NCM_ACCUMULATION_TIMEOUT	0x03


static struct f_ncm	*the_ncm[GNET_MAX_NUM] = {0};
extern unsigned int glink_idx;

#ifdef CONFIG_ETH_BYPASS_MODE
typedef struct
{
	struct list_head	list;
	u8			*buf;
	u32			length;
	int			send;
} ncm_resp_t;

typedef struct
{
	void			(*resp_avail)(void *v);
	void			*v;
    spinlock_t          resp_lock;
	struct list_head	resp_queue;
    struct semaphore    sem;
    struct timer_list   timer;
} ncm_params_t;
#endif

enum ncm_notify_state {
	NCM_NOTIFY_NONE,		/* don't notify */
	NCM_NOTIFY_CONNECT,		/* issue CONNECT next */
	NCM_NOTIFY_SPEED,		/* issue SPEED_CHANGE next */
	NCM_NOTIFY_SET_HOST_TIMEOUT,
	NCM_NOTIFY_RESPONSE_AVAI  /* response available notify */
};

struct f_ncm {
	struct gether			port;
	u8				ctrl_id, data_id;

	char				ethaddr[14];

	struct usb_ep			*notify;
	struct usb_request		*notify_req;
	u8				notify_state;
	bool				is_open;

	struct ndp_parser_opts		*parser_opts;
	u32       host_timeout;
	u32       stat_host_timeout;
	bool				is_crc;

	/*
	 * for notification, it is accessed from both
	 * callback and ethernet open/close
	 */
	spinlock_t			lock;
    bool is_mbim_mode;
    bool is_single_interface;
    bool is_class_balong;

#define GNET_NCM_PRESERVE_HEADER_LEN 16
    u32 header_room_resved;      /* len of the reserved header room */
    u32 curr_alt;

#ifdef CONFIG_ETH_BYPASS_MODE
    ncm_params_t        params;

    ncm_vendor_ctx_t    vendor_ctx;
#endif
};

#ifdef CONFIG_ETH_BYPASS_MODE
extern netdev_tx_t eth_vendor_start_xmit(struct sk_buff *skb,
					struct net_device *net);
extern int ncm_vendor_submit_skb(void *app_ctx, struct sk_buff *skb);

int ncm_add_response (void *app_ncm, u8 *buf, u32 len)
{
    struct f_ncm *ncm = (struct f_ncm *)app_ncm;
    ncm_resp_t *ncm_resp = NULL;

    if (!ncm || !ncm->notify)
    {
        return -EINVAL;
    }

    ncm_resp = (ncm_resp_t *)kzalloc(sizeof(ncm_resp_t) + len, GFP_ATOMIC);
    if (!ncm_resp)
    {
        return -ENOMEM;
    }

    ncm_resp->buf = (u8 *)(ncm_resp + 1);
    memcpy(ncm_resp->buf, buf, len);
    ncm_resp->length = len;
    ncm_resp->send = 0;

    /* add to response queue */
    spin_lock(&(ncm->params.resp_lock));
    list_add_tail(&(ncm_resp->list), &(ncm->params.resp_queue));
    spin_unlock(&(ncm->params.resp_lock));

    /* notify pc that at response is available now */
    ncm->params.resp_avail(ncm->params.v);

    return 0;
}

ncm_resp_t *ncm_get_next_response (struct f_ncm *ncm)
{
    ncm_resp_t        *resp,*next;

    spin_lock(&(ncm->params.resp_lock));
    list_for_each_entry_safe(resp, next, &ncm->params.resp_queue, list)
    {
        if (!resp->send)
        {
            resp->send = 1;
            list_del (&resp->list);
            spin_unlock(&(ncm->params.resp_lock));
            return resp;
        }
    }
    spin_unlock(&(ncm->params.resp_lock));

    return NULL;
}


void ncm_free_response (ncm_resp_t *resp)
{
    if (resp) {
        kfree(resp);
    }
}

bool ncm_resp_queue_empty(struct f_ncm *ncm)
{
    ncm_resp_t    *resp, *next;

    spin_lock(&(ncm->params.resp_lock));
    list_for_each_entry_safe(resp, next, &ncm->params.resp_queue, list)
    {
        if (!resp->send)
        {
            spin_unlock(&(ncm->params.resp_lock));
            return false;
        }
    }

    spin_unlock(&(ncm->params.resp_lock));
    return true;
}

void ncm_empty_resp_queue(struct f_ncm *ncm)
{
    ncm_resp_t        *resp, *next;

    spin_lock(&(ncm->params.resp_lock));
    list_for_each_entry_safe(resp, next, &ncm->params.resp_queue, list)
    {
        list_del(&resp->list);
        kfree(resp);
    }
    spin_unlock(&(ncm->params.resp_lock));
}

s32 ncm_vendor_add_response(void *app_ncm, u8 *buf, u32 len)
{
    struct f_ncm *ncm = (struct f_ncm *)app_ncm;
    s32 ret;

    ret = ncm_add_response(ncm, buf, len);

    return ret;
}
#endif

static inline struct f_ncm *func_to_ncm(struct usb_function *f)
{
	return container_of(f, struct f_ncm, port.func);
}

/* peak (theoretical) bulk transfer rate in bits-per-second */
static inline unsigned ncm_bitrate(struct usb_gadget *g)
{
	if (gadget_is_dualspeed(g) && g->speed == USB_SPEED_HIGH)
		return 13 * 512 * 8 * 1000 * 8;
	else if (gadget_is_superspeed(g) && g->speed == USB_SPEED_SUPER)
    	return 1 * 1000 * 1000 * 1000;
	else
		return 19 *  64 * 1 * 1000 * 8;
}

/*-------------------------------------------------------------------------*/

/*
 * We cannot group frames so use just the minimal size which ok to put
 * one max-size ethernet frame.
 * If the host can group frames, allow it to do that, 16K is selected,
 * because it's used by default by the current linux host driver
 */
#define NTB_DEFAULT_IN_SIZE	USB_CDC_NCM_NTB_MIN_IN_SIZE
#define NTB_OUT_SIZE		16384

/*
 * skbs of size less than that will not be aligned
 * to NCM's dwNtbInMaxSize to save bus bandwidth
 */

#define	MAX_TX_NONFIXED		(512 * 3)

#define FORMATS_SUPPORTED	(USB_CDC_NCM_NTB16_SUPPORTED |	\
				 USB_CDC_NCM_NTB32_SUPPORTED)

#ifdef CONFIG_BALONG_NCM
static struct usb_cdc_ncm_ntb_parameters ntb_parameters = {
	.wLength = sizeof ntb_parameters,
	.bmNtbFormatsSupported = cpu_to_le16(FORMATS_SUPPORTED),
	.dwNtbInMaxSize = cpu_to_le32(GNET_IN_MAXSIZE_PER_TRANSFER),
	.wNdpInDivisor = cpu_to_le16(USB_CDC_NCM_NDP_IN_DIVISOR),
	.wNdpInPayloadRemainder = cpu_to_le16(USB_CDC_NCM_NDP_IN_REMAINDER),
	.wNdpInAlignment = cpu_to_le16(USB_CDC_NCM_NDP_IN_ALIGNMENT),

	.dwNtbOutMaxSize = cpu_to_le32(GNET_OUT_MAXSIZE_PER_TRANSFER),
	.wNdpOutDivisor = cpu_to_le16(USB_CDC_NCM_NDP_OUT_DIVISOR),
	.wNdpOutPayloadRemainder = cpu_to_le16(USB_CDC_NCM_NDP_OUT_REMAINDER),
	.wNdpOutAlignment = cpu_to_le16(USB_CDC_NCM_NDP_OUT_ALIGNMENT),
};
#else
static struct usb_cdc_ncm_ntb_parameters ntb_parameters = {
	.wLength = sizeof ntb_parameters,
	.bmNtbFormatsSupported = cpu_to_le16(FORMATS_SUPPORTED),
	.dwNtbInMaxSize = cpu_to_le32(NTB_DEFAULT_IN_SIZE),
	.wNdpInDivisor = cpu_to_le16(4),
	.wNdpInPayloadRemainder = cpu_to_le16(0),
	.wNdpInAlignment = cpu_to_le16(4),

	.dwNtbOutMaxSize = cpu_to_le32(NTB_OUT_SIZE),
	.wNdpOutDivisor = cpu_to_le16(4),
	.wNdpOutPayloadRemainder = cpu_to_le16(0),
	.wNdpOutAlignment = cpu_to_le16(4),
};
#endif
/*
 * Use wMaxPacketSize big enough to fit CDC_NOTIFY_SPEED_CHANGE in one
 * packet, to simplify cancellation; and a big transfer interval, to
 * waste less bandwidth.
 */

#define LOG2_STATUS_INTERVAL_MSEC	5	/* 1 << 5 == 32 msec */
#define NCM_STATUS_BYTECOUNT		16	/* 8 byte header + data */

static struct usb_interface_assoc_descriptor ncm_iad_desc = {
	.bLength =		sizeof ncm_iad_desc,
	.bDescriptorType =	USB_DT_INTERFACE_ASSOCIATION,

	/* .bFirstInterface =	DYNAMIC, */
	.bInterfaceCount =	2,	/* control + data */
	.bFunctionClass =	USB_CLASS_COMM,
	.bFunctionSubClass =	USB_CDC_SUBCLASS_NCM,
	.bFunctionProtocol =	USB_CDC_PROTO_NONE,
	/* .iFunction =		DYNAMIC */
};

/* interface descriptor: */

static struct usb_interface_descriptor ncm_control_intf = {
	.bLength =		sizeof ncm_control_intf,
	.bDescriptorType =	USB_DT_INTERFACE,

	/* .bInterfaceNumber = DYNAMIC */
	.bNumEndpoints =	1,
	.bInterfaceClass =	USB_CLASS_COMM,
	.bInterfaceSubClass =	USB_CDC_SUBCLASS_NCM,
	.bInterfaceProtocol =	USB_CDC_PROTO_NONE,
	/* .iInterface = DYNAMIC */
};

static struct usb_cdc_header_desc ncm_header_desc = {
	.bLength =		sizeof ncm_header_desc,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_HEADER_TYPE,

	.bcdCDC =		cpu_to_le16(0x0110),
};

static struct usb_cdc_union_desc ncm_union_desc = {
	.bLength =		sizeof(ncm_union_desc),
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_UNION_TYPE,
	/* .bMasterInterface0 =	DYNAMIC */
	/* .bSlaveInterface0 =	DYNAMIC */
};

static struct usb_cdc_ncm_mbim_fdesc ncm_mbim_desc =
{
  .bLength = sizeof(ncm_mbim_desc),
  .bType = USB_DT_CS_INTERFACE,
  .bSubtype = 0x1b,
  .bcdVersion = cpu_to_le16(0x0100),
  .wMaxCtrlMsg = cpu_to_le16(0x400),
  .bNumFilters = 0x10,
  .bMaxFilterSize = 0x14,
  .wMaxSegSize = cpu_to_le16(1500),
  .bmNetworkCapabilities = 0x20,
};

static struct usb_cdc_ether_desc ecm_desc = {
	.bLength =		sizeof ecm_desc,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_ETHERNET_TYPE,

	/* this descriptor actually adds value, surprise! */
	/* .iMACAddress = DYNAMIC */
	.bmEthernetStatistics =	cpu_to_le32(0), /* no statistics */
	.wMaxSegmentSize =	cpu_to_le16(ETH_FRAME_LEN),
	.wNumberMCFilters =	cpu_to_le16(0),
	.bNumberPowerFilters =	0,
};

#define NCAPS	(USB_CDC_NCM_NCAP_ETH_FILTER \
                | USB_CDC_NCM_NCAP_NET_ADDRESS \
                | USB_CDC_NCM_NCAP_ENCAP_COMMAND \
                | USB_CDC_NCM_NCAP_MAX_DATAGRAM_SIZE \
                | USB_CDC_NCM_NCAP_CRC_MODE)

static struct usb_cdc_ncm_desc ncm_desc = {
	.bLength =		sizeof ncm_desc,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_NCM_TYPE,

	.bcdNcmVersion =	cpu_to_le16(0x0100),
	/* can process SetEthernetPacketFilter */
	.bmNetworkCapabilities = NCAPS,
};

/* the default data interface has no endpoints ... */

static struct usb_interface_descriptor ncm_data_nop_intf = {
	.bLength =		sizeof ncm_data_nop_intf,
	.bDescriptorType =	USB_DT_INTERFACE,

	.bInterfaceNumber =	1,
	.bAlternateSetting =	0,
	.bNumEndpoints =	0,
	.bInterfaceClass =	USB_CLASS_CDC_DATA,
	.bInterfaceSubClass =	0,
	.bInterfaceProtocol =	USB_CDC_NCM_PROTO_NTB,
	/* .iInterface = DYNAMIC */
};

/* ... but the "real" data interface has two bulk endpoints */

static struct usb_interface_descriptor ncm_data_intf = {
	.bLength =		sizeof ncm_data_intf,
	.bDescriptorType =	USB_DT_INTERFACE,

	.bInterfaceNumber =	1,
	.bAlternateSetting =	1,
	.bNumEndpoints =	2,
	.bInterfaceClass =	USB_CLASS_CDC_DATA,
	.bInterfaceSubClass =	0,
	.bInterfaceProtocol =	USB_CDC_NCM_PROTO_NTB,
	/* .iInterface = DYNAMIC */
};

/* full speed support: */

static struct usb_endpoint_descriptor fs_ncm_notify_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize =	cpu_to_le16(NCM_STATUS_BYTECOUNT),
	.bInterval =		1 << LOG2_STATUS_INTERVAL_MSEC,
};

static struct usb_endpoint_descriptor fs_ncm_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
};

static struct usb_endpoint_descriptor fs_ncm_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
};

static struct usb_descriptor_header *ncm_fs_function_single[] = {
//	(struct usb_descriptor_header *) &ncm_iad_desc,
	/* CDC NCM control descriptors */
	(struct usb_descriptor_header *) &ncm_control_intf,
	(struct usb_descriptor_header *) &ncm_header_desc,
	(struct usb_descriptor_header *) &ncm_desc,
	(struct usb_descriptor_header *) &ecm_desc,
	(struct usb_descriptor_header *) &ncm_union_desc,
	(struct usb_descriptor_header *) &fs_ncm_notify_desc,
	/* data interface, altsettings 0 and 1 */
//	(struct usb_descriptor_header *) &ncm_data_nop_intf,
	(struct usb_descriptor_header *) &ncm_data_intf,
    (struct usb_descriptor_header *) &fs_ncm_notify_desc,
	(struct usb_descriptor_header *) &fs_ncm_in_desc,
	(struct usb_descriptor_header *) &fs_ncm_out_desc,
	NULL,
};

static struct usb_descriptor_header *ncm_fs_function[] = {
	(struct usb_descriptor_header *) &ncm_iad_desc,
	/* CDC NCM control descriptors */
	(struct usb_descriptor_header *) &ncm_control_intf,
	(struct usb_descriptor_header *) &ncm_header_desc,
	(struct usb_descriptor_header *) &ncm_union_desc,
	(struct usb_descriptor_header *) &ecm_desc,
	(struct usb_descriptor_header *) &ncm_desc,
	(struct usb_descriptor_header *) &fs_ncm_notify_desc,
	/* data interface, altsettings 0 and 1 */
	(struct usb_descriptor_header *) &ncm_data_nop_intf,
	(struct usb_descriptor_header *) &ncm_data_intf,
	(struct usb_descriptor_header *) &fs_ncm_in_desc,
	(struct usb_descriptor_header *) &fs_ncm_out_desc,
	NULL,
};

static struct usb_descriptor_header *ncm_mbim_fs_function[]  = {
  (struct usb_descriptor_header *) &ncm_iad_desc,
  /* CDC NCM control descriptors */
  (struct usb_descriptor_header *) &ncm_control_intf,
  (struct usb_descriptor_header *) &ncm_header_desc,
  (struct usb_descriptor_header *) &ncm_mbim_desc,
  (struct usb_descriptor_header *) &ncm_union_desc,
  (struct usb_descriptor_header *) &fs_ncm_notify_desc,
  /* data interface, altsettings 0 and 1 */
  (struct usb_descriptor_header *) &ncm_data_nop_intf,
  (struct usb_descriptor_header *) &ncm_data_intf,
  (struct usb_descriptor_header *) &fs_ncm_in_desc,
  (struct usb_descriptor_header *) &fs_ncm_out_desc,
  NULL,
};
/* high speed support: */

static struct usb_endpoint_descriptor hs_ncm_notify_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize =	cpu_to_le16(NCM_STATUS_BYTECOUNT),
	.bInterval =		LOG2_STATUS_INTERVAL_MSEC + 4,
};
static struct usb_endpoint_descriptor hs_ncm_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(512),
};

static struct usb_endpoint_descriptor hs_ncm_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(512),
};

static struct usb_descriptor_header *ncm_hs_function_single[] = {
//	(struct usb_descriptor_header *) &ncm_iad_desc,
	/* CDC NCM control descriptors */
	(struct usb_descriptor_header *) &ncm_control_intf,
	(struct usb_descriptor_header *) &ncm_header_desc,
	(struct usb_descriptor_header *) &ncm_desc,
	(struct usb_descriptor_header *) &ecm_desc,
	(struct usb_descriptor_header *) &ncm_union_desc,
	(struct usb_descriptor_header *) &hs_ncm_notify_desc,
	/* data interface, altsettings 0 and 1 */
//	(struct usb_descriptor_header *) &ncm_data_nop_intf,
	(struct usb_descriptor_header *) &ncm_data_intf,
	(struct usb_descriptor_header *) &hs_ncm_notify_desc,
	(struct usb_descriptor_header *) &hs_ncm_in_desc,
	(struct usb_descriptor_header *) &hs_ncm_out_desc,
	NULL,
};

static struct usb_descriptor_header *ncm_hs_function[] = {
	(struct usb_descriptor_header *) &ncm_iad_desc,
	/* CDC NCM control descriptors */
	(struct usb_descriptor_header *) &ncm_control_intf,
	(struct usb_descriptor_header *) &ncm_header_desc,
	(struct usb_descriptor_header *) &ncm_union_desc,
	(struct usb_descriptor_header *) &ecm_desc,
	(struct usb_descriptor_header *) &ncm_desc,
	(struct usb_descriptor_header *) &hs_ncm_notify_desc,
	/* data interface, altsettings 0 and 1 */
	(struct usb_descriptor_header *) &ncm_data_nop_intf,
	(struct usb_descriptor_header *) &ncm_data_intf,
	(struct usb_descriptor_header *) &hs_ncm_in_desc,
	(struct usb_descriptor_header *) &hs_ncm_out_desc,
	NULL,
};

static struct usb_descriptor_header *ncm_mbim_hs_function[]  = {
  (struct usb_descriptor_header *) &ncm_iad_desc,
  /* CDC NCM control descriptors */
  (struct usb_descriptor_header *) &ncm_control_intf,
  (struct usb_descriptor_header *) &ncm_header_desc,
  (struct usb_descriptor_header *) &ncm_mbim_desc,
  (struct usb_descriptor_header *) &ncm_union_desc,
  (struct usb_descriptor_header *) &hs_ncm_notify_desc,
  /* data interface, altsettings 0 and 1 */
  (struct usb_descriptor_header *) &ncm_data_nop_intf,
  (struct usb_descriptor_header *) &ncm_data_intf,
  (struct usb_descriptor_header *) &hs_ncm_in_desc,
  (struct usb_descriptor_header *) &hs_ncm_out_desc,
  NULL,
};

/* super speed support: */

static struct usb_endpoint_descriptor ss_ncm_notify_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize =	cpu_to_le16(NCM_STATUS_BYTECOUNT),
	.bInterval =		LOG2_STATUS_INTERVAL_MSEC + 4,
};

static struct usb_ss_ep_comp_descriptor ss_ncm_intr_comp_desc = {
	.bLength =		sizeof ss_ncm_intr_comp_desc,
	.bDescriptorType =	USB_DT_SS_ENDPOINT_COMP,

	/* the following 3 values can be tweaked if necessary */
	/* .bMaxBurst =		0, */
	/* .bmAttributes =	0, */
	.wBytesPerInterval =	cpu_to_le16(NCM_STATUS_BYTECOUNT),
};

static struct usb_endpoint_descriptor ss_ncm_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(1024),
};

static struct usb_endpoint_descriptor ss_ncm_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(1024),
};

static struct usb_ss_ep_comp_descriptor ss_ncm_bulk_comp_desc = {
	.bLength =		sizeof ss_ncm_bulk_comp_desc,
	.bDescriptorType =	USB_DT_SS_ENDPOINT_COMP,

	/* the following 2 values can be tweaked if necessary */
	/* .bMaxBurst =		0, */
	/* .bmAttributes =	0, */
};

static struct usb_descriptor_header *ncm_ss_function_single[] = {
//	(struct usb_descriptor_header *) &ncm_iad_desc,
	/* CDC NCM control descriptors */
	(struct usb_descriptor_header *) &ncm_control_intf,
	(struct usb_descriptor_header *) &ncm_header_desc,
	(struct usb_descriptor_header *) &ncm_desc,
	(struct usb_descriptor_header *) &ecm_desc,
	(struct usb_descriptor_header *) &ncm_union_desc,
	(struct usb_descriptor_header *) &ss_ncm_notify_desc,
	(struct usb_descriptor_header *) &ss_ncm_intr_comp_desc,
	/* data interface, altsettings 0 and 1 */
//	(struct usb_descriptor_header *) &ncm_data_nop_intf,
	(struct usb_descriptor_header *) &ncm_data_intf,
	(struct usb_descriptor_header *) &ss_ncm_notify_desc,
	(struct usb_descriptor_header *) &ss_ncm_intr_comp_desc,
	(struct usb_descriptor_header *) &ss_ncm_in_desc,
	(struct usb_descriptor_header *) &ss_ncm_bulk_comp_desc,
	(struct usb_descriptor_header *) &ss_ncm_out_desc,
	(struct usb_descriptor_header *) &ss_ncm_bulk_comp_desc,
	NULL,
};

static struct usb_descriptor_header *ncm_ss_function[] = {
	(struct usb_descriptor_header *) &ncm_iad_desc,
	/* CDC NCM control descriptors */
	(struct usb_descriptor_header *) &ncm_control_intf,
	(struct usb_descriptor_header *) &ncm_header_desc,
	(struct usb_descriptor_header *) &ncm_union_desc,
	(struct usb_descriptor_header *) &ecm_desc,
	(struct usb_descriptor_header *) &ncm_desc,
	(struct usb_descriptor_header *) &ss_ncm_notify_desc,
	(struct usb_descriptor_header *) &ss_ncm_intr_comp_desc,
	/* data interface, altsettings 0 and 1 */
	(struct usb_descriptor_header *) &ncm_data_nop_intf,
	(struct usb_descriptor_header *) &ncm_data_intf,
	(struct usb_descriptor_header *) &ss_ncm_in_desc,
	(struct usb_descriptor_header *) &ss_ncm_bulk_comp_desc,
	(struct usb_descriptor_header *) &ss_ncm_out_desc,
	(struct usb_descriptor_header *) &ss_ncm_bulk_comp_desc,
	NULL,
};

static struct usb_descriptor_header *ncm_mbim_ss_function[]  = {
  (struct usb_descriptor_header *) &ncm_iad_desc,
  /* CDC NCM control descriptors */
  (struct usb_descriptor_header *) &ncm_control_intf,
  (struct usb_descriptor_header *) &ncm_header_desc,
  (struct usb_descriptor_header *) &ncm_mbim_desc,
  (struct usb_descriptor_header *) &ncm_union_desc,
  (struct usb_descriptor_header *) &ss_ncm_notify_desc,
  (struct usb_descriptor_header *) &ss_ncm_intr_comp_desc,
  /* data interface, altsettings 0 and 1 */
  (struct usb_descriptor_header *) &ncm_data_nop_intf,
  (struct usb_descriptor_header *) &ncm_data_intf,
  (struct usb_descriptor_header *) &ss_ncm_in_desc,
  (struct usb_descriptor_header *) &ss_ncm_bulk_comp_desc,
  (struct usb_descriptor_header *) &ss_ncm_out_desc,
  (struct usb_descriptor_header *) &ss_ncm_bulk_comp_desc,
  NULL,
};

/* string descriptors: */

#define STRING_CTRL_IDX	0
#define STRING_MAC_IDX	1
#define STRING_DATA_IDX	2
#define STRING_IAD_IDX	3

static struct usb_string ncm_string_defs[] = {
	[STRING_CTRL_IDX].s = "NCM Network Control Model",
	[STRING_MAC_IDX].s = NULL /* DYNAMIC */,
	[STRING_DATA_IDX].s = "CDC Network Data",
	[STRING_IAD_IDX].s = "CDC NCM",
	{  } 
};

static struct usb_gadget_strings ncm_string_table = {
	.language =		0x0409,	/* en-us */
	.strings =		ncm_string_defs,
};

static struct usb_gadget_strings *ncm_strings[] = {
	&ncm_string_table,
	NULL,
};

/*
 * Here are options for NCM Datagram Pointer table (NDP) parser.
 * There are 2 different formats: NDP16 and NDP32 in the spec (ch. 3),
 * in NDP16 offsets and sizes fields are 1 16bit word wide,
 * in NDP32 -- 2 16bit words wide. Also signatures are different.
 * To make the parser code the same, put the differences in the structure,
 * and switch pointers to the structures when the format is changed.
 */

struct ndp_parser_opts {
	u32		nth_sign;
	u32		ndp_sign;
	unsigned	nth_size;
	unsigned	ndp_size;
	unsigned	ndplen_align;
	/* sizes in u16 units */
	unsigned	dgram_item_len; /* index or length */
	unsigned	block_length;
	unsigned	fp_index;
	unsigned	reserved1;
	unsigned	reserved2;
	unsigned	next_fp_index;
};

#define INIT_NDP16_OPTS {					\
		.nth_sign = USB_CDC_NCM_NTH16_SIGN,		\
		.ndp_sign = USB_CDC_NCM_NDP16_NOCRC_SIGN,	\
		.nth_size = sizeof(struct usb_cdc_ncm_nth16),	\
		.ndp_size = sizeof(struct usb_cdc_ncm_ndp16),	\
		.ndplen_align = 4,				\
		.dgram_item_len = 1,				\
		.block_length = 1,				\
		.fp_index = 1,					\
		.reserved1 = 0,					\
		.reserved2 = 0,					\
		.next_fp_index = 1,				\
	}


#define INIT_NDP32_OPTS {					\
		.nth_sign = USB_CDC_NCM_NTH32_SIGN,		\
		.ndp_sign = USB_CDC_NCM_NDP32_NOCRC_SIGN,	\
		.nth_size = sizeof(struct usb_cdc_ncm_nth32),	\
		.ndp_size = sizeof(struct usb_cdc_ncm_ndp32),	\
		.ndplen_align = 8,				\
		.dgram_item_len = 2,				\
		.block_length = 2,				\
		.fp_index = 2,					\
		.reserved1 = 1,					\
		.reserved2 = 2,					\
		.next_fp_index = 2,				\
	}

static struct ndp_parser_opts ndp16_opts = INIT_NDP16_OPTS;
static struct ndp_parser_opts ndp32_opts = INIT_NDP32_OPTS;

static inline void put_ncm(__le16 **p, unsigned size, unsigned val)
{
	switch (size) {
	case 1:
		put_unaligned_le16((u16)val, *p);
		break;
	case 2:
		put_unaligned_le32((u32)val, *p);
		break;
	default:
		BUG();
	}

	*p += size;
}

static inline unsigned get_ncm(__le16 **p, unsigned size)
{
	unsigned tmp = 0;

	switch (size) {
	case 1:
		tmp = get_unaligned_le16(*p);
		break;
	case 2:
		tmp = get_unaligned_le32(*p);
		break;
	default:
		BUG();
	}

	*p += size;
	return tmp;
}

/*-------------------------------------------------------------------------*/

static inline void ncm_reset_values(struct f_ncm *ncm)
{
#ifdef CONFIG_BALONG_NCM
    ncm->parser_opts = &ndp32_opts;
#else
	ncm->parser_opts = &ndp16_opts;
#endif
	ncm->is_crc = false;
	ncm->port.cdc_filter = DEFAULT_FILTER;

	/* doesn't make sense for ncm, fixed size used */
	ncm->port.header_len = 0;

	ncm->port.fixed_out_len = le32_to_cpu(ntb_parameters.dwNtbOutMaxSize);
#ifdef CONFIG_BALONG_NCM
    ncm->port.fixed_in_len = le32_to_cpu(ntb_parameters.dwNtbInMaxSize);
#else
	ncm->port.fixed_in_len = NTB_DEFAULT_IN_SIZE;
#endif
}

/*
 * Context: ncm->lock held
 */
static void ncm_do_notify(struct f_ncm *ncm)
{
	struct usb_request		*req = ncm->notify_req;
	struct usb_cdc_notification	*event;
	struct usb_composite_dev	*cdev = ncm->port.func.config->cdev;
	__le32				*data;
	int				status;
	struct eth_dev *dev = ncm->port.ioport;

#ifdef CONFIG_ETH_BYPASS_MODE
    if (ncm->is_mbim_mode)
    {
        ERROR(cdev, "%s:mbim mode.\n", __FUNCTION__);
        return;
    }
#endif

	/* notification already in flight? */
	if (!req)
		return;

    /* ncm not enable, reset the notify status */
	if (!dev) {
	    ncm->notify_state = NCM_NOTIFY_NONE;
	    return;
	}

	event = req->buf;
	switch (ncm->notify_state) {
	case NCM_NOTIFY_NONE:
		return;

    case NCM_NOTIFY_SET_HOST_TIMEOUT:
        event->bNotificationType = USB_NOTIFY_VENDOR_NCM_ACCUMULATION_TIMEOUT;
        event->wValue = cpu_to_le16(0);
        event->wLength = sizeof(u32);
        event->bmRequestType = USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE;
        req->length = sizeof(u32) + sizeof(*event);
        data = req->buf + sizeof(*event);
        memcpy((void*)data, &ncm->host_timeout, sizeof(u32));
        ncm->notify_state = NCM_NOTIFY_NONE;
        break;

	case NCM_NOTIFY_CONNECT:
		event->bNotificationType = USB_CDC_NOTIFY_NETWORK_CONNECTION;
#ifdef CONFIG_ETH_BYPASS_MODE
        event->wValue = cpu_to_le16(ncm->vendor_ctx.connect);
#else
		if (ncm->is_open)
			event->wValue = cpu_to_le16(1);
		else
			event->wValue = cpu_to_le16(0);
#endif
		event->wLength = 0;
		req->length = sizeof *event;

		USB_DBG_GADGET(&cdev->gadget->dev, "notify connect %s\n",
				ncm->is_open ? "true" : "false");
		ncm->notify_state = NCM_NOTIFY_NONE;
		event->bmRequestType = 0xA1;
		break;

	case NCM_NOTIFY_SPEED:
		event->bNotificationType = USB_CDC_NOTIFY_SPEED_CHANGE;
		event->wValue = cpu_to_le16(0);
		event->wLength = cpu_to_le16(8);
		req->length = NCM_STATUS_BYTECOUNT;

		/* SPEED_CHANGE data is up/down speeds in bits/sec */
		data = req->buf + sizeof *event;
#ifdef CONFIG_ETH_BYPASS_MODE
		data[0] = cpu_to_le32(ncm->vendor_ctx.speed.u32DownBitRate);
		data[1] = cpu_to_le32(ncm->vendor_ctx.speed.u32UpBitRate);

        USB_DBG_GADGET(&cdev->gadget->dev, "notify speed [%d:%d]\n",
            ncm->vendor_ctx.speed.u32DownBitRate,
            ncm->vendor_ctx.speed.u32UpBitRate);
#else
		data[0] = cpu_to_le32(ncm_bitrate(cdev->gadget));
		data[1] = data[0];

		USB_DBG_GADGET(&cdev->gadget->dev, "notify speed %d\n", ncm_bitrate(cdev->gadget));
#endif

#ifdef CONFIG_ETH_BYPASS_MODE
		ncm->notify_state = NCM_NOTIFY_NONE;
#else
		ncm->notify_state = NCM_NOTIFY_CONNECT;
#endif
        event->bmRequestType = 0xA1;
		break;
    case NCM_NOTIFY_RESPONSE_AVAI:
        memset(event, 0, sizeof(*event));
        event->bmRequestType = 0xA1;
        event->bNotificationType = USB_CDC_NOTIFY_RESPONSE_AVAILABLE;

        req->length = sizeof(*event);
        /* stop to notify */
        ncm->notify_state = NCM_NOTIFY_NONE;
        break;

	}
	/* there are different bmRequestType in vendor notifiy */
	//event->bmRequestType = 0xA1;
	event->wIndex = cpu_to_le16(ncm->ctrl_id);

	ncm->notify_req = NULL;
	/*
	 * In double buffering if there is a space in FIFO,
	 * completion callback can be called right after the call,
	 * so unlocking
	 */
	spin_unlock(&ncm->lock);
	/* check whether the gadget is suspended.
       if so, try to wake up the host */
    if (unlikely(dev->is_suspend)) {
        if (gnet_wakeup_gadget(dev)) {
            printk("remote wakeup failed\n");
            /* wakeup failed, reset the notify state */
            ncm->notify_state = NCM_NOTIFY_NONE;
            ncm->notify_req = req;
            spin_lock(&ncm->lock);
            return;
        }
    }
	status = usb_ep_queue(ncm->notify, req, GFP_ATOMIC);
	spin_lock(&ncm->lock);
	if (status < 0) {
		ncm->notify_req = req;
		DBG(cdev, "notify --> %d\n", status);
	}
}

/*
 * Context: ncm->lock held
 */
static void ncm_notify(struct f_ncm *ncm)
{
	/*
	 * NOTE on most versions of Linux, host side cdc-ethernet
	 * won't listen for notifications until its netdevice opens.
	 * The first notification then sits in the FIFO for a long
	 * time, and the second one is queued.
	 *
	 * If ncm_notify() is called before the second (CONNECT)
	 * notification is sent, then it will reset to send the SPEED
	 * notificaion again (and again, and again), but it's not a problem
	 */
	ncm->notify_state = NCM_NOTIFY_SPEED;
	ncm_do_notify(ncm);
}

static void ncm_notify_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct f_ncm			*ncm = req->context;
    #ifdef DEBUG
	struct usb_composite_dev	*cdev = ncm->port.func.config->cdev;
	struct usb_cdc_notification	*event = req->buf;
    #endif

	spin_lock(&ncm->lock);
	switch (req->status) {
	case 0:
		USB_DBG_GADGET(&cdev->gadget->dev, "Notification %02x sent\n",
		     event->bNotificationType);
		break;
	case -ECONNRESET:
	case -ESHUTDOWN:
		ncm->notify_state = NCM_NOTIFY_NONE;
		break;
	default:
		USB_DBG_GADGET(&cdev->gadget->dev, "event %02x --> %d\n",
			event->bNotificationType, req->status);
		break;
	}

	ncm->notify_req = req;
	ncm_do_notify(ncm);
	spin_unlock(&ncm->lock);

#ifdef CONFIG_ETH_BYPASS_MODE
    complete(&(ncm->vendor_ctx.notify_done));
#endif
}

static void ncm_ep0out_complete(struct usb_ep *ep, struct usb_request *req)
{
	/* now for SET_NTB_INPUT_SIZE only */
	unsigned		in_size;
	struct usb_function	*f = req->context;
	struct f_ncm		*ncm = func_to_ncm(f);
    #ifdef DEBUG
	struct usb_composite_dev *cdev = ep->driver_data;
    #endif

	req->context = NULL;
	if (req->status || req->actual != req->length) {
		DBG(cdev, "Bad control-OUT transfer\n");
		goto invalid;
	}

	in_size = get_unaligned_le32(req->buf);
	if (in_size < USB_CDC_NCM_NTB_MIN_IN_SIZE ||
	    in_size > le32_to_cpu(ntb_parameters.dwNtbInMaxSize)) {
		DBG(cdev, "Got wrong INPUT SIZE (%d) from host\n", in_size);
		goto invalid;
	}

	ncm->port.fixed_in_len = in_size;
	USB_DBG_GADGET(&cdev->gadget->dev, "Set NTB INPUT SIZE %d\n", in_size);
	return;

invalid:
	usb_ep_set_halt(ep);
	return;
}

#ifdef CONFIG_ETH_BYPASS_MODE
static void ncm_encap_cmd_handle(struct f_ncm *ncm, u8 *buf, u32 len)
{
    ncm_vendor_encap_cmd(ncm->vendor_ctx.vendor_priv, buf, len);
}

static void ncm_encap_cmd_complete(struct usb_ep *ep, struct usb_request *req)
{
    struct f_ncm *ncm = req->context;
    int   status = req->status;

    if(!ncm)
    {
        return;
    }

    switch (status)
    {
        case 0:
        {
            ncm_encap_cmd_handle(ncm, req->buf, req->length);
            break;
        }

        case -ECONNRESET:           /* unlink */
        case -ESHUTDOWN:            /* disconnect etc */
        case -ECONNABORTED:         /* endpoint reset */
        case -EOVERFLOW:            /* data overrun */
        default:
        {
            printk(KERN_ERR "%s line %d: status = %d", __FUNCTION__, __LINE__, status);
            break;
        }
    }

    return;
}

static void ncm_response_complete(struct usb_ep *ep, struct usb_request *req)
{
    struct f_ncm      *ncm = req->context;
#ifdef CONFIG_NCM_MBIM_SUPPORT
    int        status = req->status;
#endif

    if (ncm->is_mbim_mode)
    {
#ifdef CONFIG_NCM_MBIM_SUPPORT
        ncm_mbim_send_encap_rsp_done(status);
#endif
    }

    req->length = 0;

    if (!ncm_resp_queue_empty(ncm))
    {
        mod_timer(&ncm->params.timer,(jiffies + (1*HZ)));
    }

    return;
}

static u32 ncm_encap_cmd_response(struct f_ncm *ncm, struct usb_request *req)
{
    ncm_resp_t *resp;
    u32 value;

    resp = ncm_get_next_response(ncm);
    if (resp)
    {
        memcpy(req->buf, resp->buf, resp->length);
        req->complete = ncm_response_complete;
        value = resp->length;
        ncm_free_response(resp);
    }
    else
    {
        value = 0;
    }

    return value;
}

static void ncm_response_available(void *v)
{
    struct f_ncm         *ncm = (struct f_ncm*)v;
    unsigned long flags;


    if (!ncm)
    {
        /* In this case, ncm interface is removed by ^setport */
        return ;
    }

    spin_lock_irqsave(&ncm->lock,flags);
    ncm->notify_state = NCM_NOTIFY_RESPONSE_AVAI;
    ncm_do_notify(ncm);
    spin_unlock_irqrestore(&ncm->lock,flags);
}

static void ncm_response_available_notify(unsigned long handle)
{
    struct f_ncm *ncm = (struct f_ncm *)handle;

    if (!ncm_resp_queue_empty(ncm))
    {
        if(ncm->params.resp_avail)
            ncm->params.resp_avail(ncm->params.v);
    }
}
#endif  /* end of CONFIG_ETH_BYPASS_MODE */

static int ncm_setup(struct usb_function *f, const struct usb_ctrlrequest *ctrl)
{
	struct f_ncm		*ncm = func_to_ncm(f);
	struct usb_composite_dev *cdev = f->config->cdev;
	struct usb_request	*req = cdev->req;
	int			value = -EOPNOTSUPP;
	u16			w_index = le16_to_cpu(ctrl->wIndex);
	u16			w_value = le16_to_cpu(ctrl->wValue);
	u16			w_length = le16_to_cpu(ctrl->wLength);

	/*
	 * composite driver infrastructure handles everything except
	 * CDC class messages; interface activation uses set_alt().
	 */
	switch ((ctrl->bRequestType << 8) | ctrl->bRequest) {
	case (((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
			| USB_CDC_SET_ETHERNET_PACKET_FILTER):
		/*
		 * see 6.2.30: no data, wIndex = interface,
		 * wValue = packet filter bitmap
		 */
		if (w_length != 0 || w_index != ncm->ctrl_id)
			goto invalid;
		USB_DBG_GADGET(&cdev->gadget->dev, "packet filter %02x\n", w_value);
		/*
		 * REVISIT locking of cdc_filter.  This assumes the UDC
		 * driver won't have a concurrent packet TX irq running on
		 * another CPU; or that if it does, this write is atomic...
		 */
		ncm->port.cdc_filter = w_value;
		value = 0;
		break;

	/*
	 * and optionally:
	 * case USB_CDC_SEND_ENCAPSULATED_COMMAND:
	 * case USB_CDC_GET_ENCAPSULATED_RESPONSE:
	 * case USB_CDC_SET_ETHERNET_MULTICAST_FILTERS:
	 * case USB_CDC_SET_ETHERNET_PM_PATTERN_FILTER:
	 * case USB_CDC_GET_ETHERNET_PM_PATTERN_FILTER:
	 * case USB_CDC_GET_ETHERNET_STATISTIC:
	 */
#ifdef CONFIG_ETH_BYPASS_MODE
    case  (((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
          | USB_CDC_SEND_ENCAPSULATED_COMMAND):

        if (w_length > req->length || w_value
            || w_index != ncm->ctrl_id)
            goto invalid;
        value = w_length;
        req->complete = ncm_encap_cmd_complete;
        req->context = ncm;
        break;

    case  ((((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
          | USB_CDC_GET_ENCAPSULATED_RESPONSE)):

        if (w_value || w_index != ncm->ctrl_id)
            goto invalid;

        value = ncm_encap_cmd_response(ncm, req);
        break;
#endif

	case (((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
		| USB_CDC_GET_NTB_PARAMETERS):

		if (w_length == 0 || w_value != 0 || w_index != ncm->ctrl_id)
			goto invalid;
		value = w_length > sizeof ntb_parameters ?
			sizeof ntb_parameters : w_length;
		memcpy(req->buf, &ntb_parameters, value);
		USB_DBG_GADGET(&cdev->gadget->dev, "Host asked NTB parameters\n");
		break;

	case (((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
		| USB_CDC_GET_NTB_INPUT_SIZE):

		if (w_length < 4 || w_value != 0 || w_index != ncm->ctrl_id)
			goto invalid;
		put_unaligned_le32(ncm->port.fixed_in_len, req->buf);
		value = 4;
		USB_DBG_GADGET(&cdev->gadget->dev, "Host asked INPUT SIZE, sending %d\n",
		     ncm->port.fixed_in_len);
		break;

	case (((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
		| USB_CDC_SET_NTB_INPUT_SIZE):
	{
		if (w_length != 4 || w_value != 0 || w_index != ncm->ctrl_id)
			goto invalid;
		req->complete = ncm_ep0out_complete;
		req->length = w_length;
		req->context = f;

		value = req->length;
		break;
	}

	case (((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
		| USB_CDC_GET_NTB_FORMAT):
	{
		uint16_t format;

		if (w_length < 2 || w_value != 0 || w_index != ncm->ctrl_id)
			goto invalid;
		format = (ncm->parser_opts == &ndp16_opts) ? 0x0000 : 0x0001;
		put_unaligned_le16(format, req->buf);
		value = 2;
		USB_DBG_GADGET(&cdev->gadget->dev, "Host asked NTB FORMAT, sending %d\n", format);
		break;
	}

	case (((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
		| USB_CDC_SET_NTB_FORMAT):
	{
		if (w_length != 0 || w_index != ncm->ctrl_id)
			goto invalid;
		switch (w_value) {
		case 0x0000:
			ncm->parser_opts = &ndp16_opts;
			USB_DBG_GADGET(&cdev->gadget->dev, "NCM16 selected\n");
			break;
		case 0x0001:
			ncm->parser_opts = &ndp32_opts;
			USB_DBG_GADGET(&cdev->gadget->dev, "NCM32 selected\n");
			break;
		default:
			goto invalid;
		}
		value = 0;
		break;
	}
	case (((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
		| USB_CDC_GET_CRC_MODE):
	{
		uint16_t is_crc;

		if (w_length < 2 || w_value != 0 || w_index != ncm->ctrl_id)
			goto invalid;
		is_crc = ncm->is_crc ? 0x0001 : 0x0000;
		put_unaligned_le16(is_crc, req->buf);
		value = 2;
		USB_DBG_GADGET(&cdev->gadget->dev, "Host asked CRC MODE, sending %d\n", is_crc);
		break;
	}

	case (((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
		| USB_CDC_SET_CRC_MODE):
	{
		int ndp_hdr_crc = 0;

		if (w_length != 0 || w_index != ncm->ctrl_id)
			goto invalid;
		switch (w_value) {
		case 0x0000:
			ncm->is_crc = false;
			ndp_hdr_crc = NCM_NDP_HDR_NOCRC;
			USB_DBG_GADGET(&cdev->gadget->dev, "non-CRC mode selected\n");
			break;
		case 0x0001:
			ncm->is_crc = true;
			ndp_hdr_crc = NCM_NDP_HDR_CRC;
			USB_DBG_GADGET(&cdev->gadget->dev, "CRC mode selected\n");
			break;
		default:
			goto invalid;
		}
		ncm->parser_opts->ndp_sign &= ~NCM_NDP_HDR_CRC_MASK;
		ncm->parser_opts->ndp_sign |= ndp_hdr_crc;
		value = 0;
		break;
	}

#ifdef CONFIG_ETH_BYPASS_MODE
    case (((USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE) << 8)
      | USB_CDC_GET_WWAN_STATE) :
    {
        if (bsp_usb_is_support_wwan()) {
            memcpy(req->buf, "WWAN", 4);
        }
        else {
            memset(req->buf, 0, 4);
        }
        value = 4;
        break;
    }

    /*
    case (((USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE) << 8)
      | CDC_REQ_GET_NCM_PACKETS_ACCUMULATION_TIME) :
    {
        ncm_app_ctx_t  *app_ctx = (ncm_app_ctx_t *)ncm->vendor_ctx.ncm_priv;

        put_unaligned_le32(app_ctx->u32AccumulationTime, req->buf);
        value = 4;
        break;
    }
    */

    case (((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8 )
      | USB_REG_MBIM_RESET) :
    {
        value = 0;
        break;
    }
#endif

	/* and disabled in ncm descriptor: */
	/* case USB_CDC_GET_NET_ADDRESS: */
	/* case USB_CDC_SET_NET_ADDRESS: */
	/* case USB_CDC_GET_MAX_DATAGRAM_SIZE: */
	/* case USB_CDC_SET_MAX_DATAGRAM_SIZE: */

    /* host request to get the reserved_bytes parameter */
    case (((USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE) << 8)
      | USB_CDC_NTFY_HUAWEI_GET_NCM_RESERVE_BYTES):
    {
        if (w_length != 4 || w_value != 0 || w_index != ncm->ctrl_id)
			goto invalid;

        ncm->header_room_resved = GNET_NCM_PRESERVE_HEADER_LEN;
		put_unaligned_le32(ncm->header_room_resved, req->buf);
		value = 4;

		USB_DBG_GADGET(&cdev->gadget->dev, "Host asked reserved header room len, sending %d\n",
		     ncm->header_room_resved);
        break;
    }

	default:
invalid:
		USB_DBG_GADGET(&cdev->gadget->dev, "invalid control req%02x.%02x v%04x i%04x l%d\n",
			ctrl->bRequestType, ctrl->bRequest,
			w_value, w_index, w_length);
	}

	/* respond with data transfer or status phase? */
	if (value >= 0) {
		USB_DBG_GADGET(&cdev->gadget->dev, "ncm req%02x.%02x v%04x i%04x l%d\n",
			ctrl->bRequestType, ctrl->bRequest,
			w_value, w_index, w_length);
		req->zero = 0;
		req->length = value;
		value = usb_ep_queue(cdev->gadget->ep0, req, GFP_ATOMIC);
		if (value < 0)
			ERROR(cdev, "ncm req %02x.%02x response err %d\n",
					ctrl->bRequestType, ctrl->bRequest,
					value);
	}

	/* device either stalls (value < 0) or reports success */
	return value;
}

s32 ncm_set_host_assemble_param(int net_id, unsigned long host_timeout)
{
    unsigned long flags;
    struct f_ncm *ncm = the_ncm[net_id];/* [false alarm]:fortify disable */

    if (unlikely(!ncm)) {
        return -ESHUTDOWN;
    }/* [false alarm]:fortify disable */
    spin_lock_irqsave(&ncm->lock, flags);
    ncm->host_timeout = host_timeout;
    ncm->notify_state = NCM_NOTIFY_SET_HOST_TIMEOUT;
    ncm->stat_host_timeout++;
    ncm_do_notify(ncm);
    spin_unlock_irqrestore(&ncm->lock, flags);

    return 0;
}

#ifdef CONFIG_ETH_BYPASS_MODE
s32 ncm_vendor_notify(void *app_ncm, u32 cmd, void *param)
{
    struct f_ncm *ncm = (struct f_ncm *)app_ncm;
    s32 ret = 0;
    unsigned long flags;

    printk(KERN_DEBUG "%s line %d:cmd=%u\n",
			__FUNCTION__, __LINE__, cmd);

    spin_lock_irqsave(&ncm->lock, flags);

    if(NCM_IOCTL_NETWORK_CONNECTION_NOTIF == cmd)
    {
        ncm->vendor_ctx.connect = *(u32 *)param;
        ncm->notify_state = NCM_NOTIFY_CONNECT;
    }

    if(NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF == cmd)
    {
        NCM_IOCTL_CONNECTION_SPEED_S *speed = (NCM_IOCTL_CONNECTION_SPEED_S *)param;
        ncm->vendor_ctx.speed.u32UpBitRate = speed->u32UpBitRate;
        ncm->vendor_ctx.speed.u32DownBitRate = speed->u32DownBitRate;
        ncm->notify_state = NCM_NOTIFY_SPEED;
    }

	ncm_do_notify(ncm);
    spin_unlock_irqrestore(&ncm->lock, flags);
    ret = wait_for_completion_interruptible_timeout(&ncm->vendor_ctx.notify_done, 1*HZ);
    if(0 >= ret)
    {
        GNET_ERR(("wait for notify_done failed.\n"));
        ncm->vendor_ctx.stat_notify_timeout++;
        return -ETIME;
    }

    return 0;
}

int ncm_vendor_post_skb(void *vendor_ncm, struct sk_buff *skb)
{
    struct f_ncm *ncm = (struct f_ncm *)vendor_ncm;
    struct eth_dev  *dev = NULL;
    ncm_app_ctx_t *app_ctx = NULL;
    netdev_tx_t  tx_ret = NETDEV_TX_OK;

    if(!ncm)
        return -ENOMEM;

    if(gnet_dbg_level & GNET_DBG_TX2TASK)
    {
        if(skb_queue_len(&ncm->vendor_ctx.tx_frames) >= 1000)
        {
            ncm->vendor_ctx.stat_tx_drop++;
            return -EIO;
        }

        skb_queue_tail(&ncm->vendor_ctx.tx_frames, skb);
        ncm->vendor_ctx.stat_tx_post++;
        complete(&ncm->vendor_ctx.tx_kick);

        return 0;
    }

    app_ctx = (ncm_app_ctx_t *)ncm->vendor_ctx.vendor_priv;
    dev = ncm->port.ioport;
    if(!dev)
    {
        ncm->vendor_ctx.stat_tx_nodev++;
        return -ENXIO;
    }
    ncm->vendor_ctx.stat_tx_post++;
    if(skb)
    {
        ncm->vendor_ctx.stat_tx_total++;
        if (true != app_ctx->bDataConnect)
        {
            ncm->vendor_ctx.stat_tx_cancel++;
            dev_kfree_skb_any(skb);
        }
        else
        {
            ncm->vendor_ctx.stat_tx_xmit++;
            tx_ret = eth_vendor_start_xmit(skb, dev->net);
            if (NETDEV_TX_OK != tx_ret)
            {
                ncm->vendor_ctx.stat_tx_xmit_fail++;
                return tx_ret;
            }
        }
    }

    return 0;
}

static int ncm_vendor_rx(struct gether *port, struct sk_buff *skb)
{
    struct f_ncm  *ncm = func_to_ncm(&port->func);

	if (!ncm) {
		 return -ENOMEM;
	}
    if( !ncm->vendor_ctx.vendor_priv)
    {
        ncm->vendor_ctx.stat_rx_einval++;
        return -ENOMEM;
    }

    ncm->vendor_ctx.stat_rx_total++;

    return ncm_vendor_submit_skb((ncm_app_ctx_t *)ncm->vendor_ctx.vendor_priv, skb);
}

static int ncm_vendor_tx_thread(void *param)
{
    struct f_ncm *ncm = (struct f_ncm *)param;
    struct eth_dev  *dev = NULL;
    struct sk_buff *skb = NULL;
    ncm_app_ctx_t *app_ctx = NULL;
    netdev_tx_t  tx_ret = NETDEV_TX_OK;

    GNET_TRACE(("ncm vendor tx thread enter.\n"));

    if (!ncm || !ncm->vendor_ctx.vendor_priv)
    {
        GNET_ERR(("ncm vendor tx thread abnormal quit!\n"));
        return -ENODEV;
    }

    app_ctx = (ncm_app_ctx_t *)ncm->vendor_ctx.vendor_priv;
    dev = ncm->port.ioport;
    if(!dev)
    {
        GNET_ERR(("no eth_dev,ncm vendor tx thread abnormal quit!\n"));
        return -ENODEV;
    }

    ncm->vendor_ctx.tx_task_run = true;
    while (ncm->vendor_ctx.tx_task_run)
    {
        wait_for_completion(&ncm->vendor_ctx.tx_kick);

        do{
            skb = skb_dequeue(&ncm->vendor_ctx.tx_frames);
            if(skb)
            {
                ncm->vendor_ctx.stat_tx_total++;
                if (true != app_ctx->bDataConnect)
                {
                    ncm->vendor_ctx.stat_tx_cancel++;
                    dev_kfree_skb_any(skb);
                }
                else
                {
                    ncm->vendor_ctx.stat_tx_xmit++;
                    tx_ret = eth_vendor_start_xmit(skb, dev->net);
                    if (NETDEV_TX_OK != tx_ret)
                    {
                        ncm->vendor_ctx.stat_tx_xmit_fail++;
                        GNET_ERR(("vendor xmit skb failed\n"));
                    }
                }
            }
        }while(skb);
    }

    complete(&ncm->vendor_ctx.tx_task_stop);
    GNET_TRACE(("ncm vendor tx thread exit.\n"));
    return 0;
}

static int ncm_vendor_tx_thread_init(struct f_ncm *ncm)
{
    GNET_TRACE(("%s:enter.\n",__FUNCTION__));

    if (!ncm)
    {
        return -ENOMEM;
    }

    if (ncm->vendor_ctx.tx_task_run)
    {
        return 0;
    }

    skb_queue_head_init(&ncm->vendor_ctx.tx_frames);
    init_completion(&ncm->vendor_ctx.tx_kick);
    init_completion(&ncm->vendor_ctx.tx_task_stop);
    init_completion(&ncm->vendor_ctx.notify_done);

    /* move tx_task_run flag in thread, if the thread is wakeup after set_alt 1
       if clean up before set_alt 1, thread_uninit will wait for ever */
    //ncm->vendor_ctx.tx_task_run = true;

    ncm->vendor_ctx.tx_task = kthread_create(ncm_vendor_tx_thread, ncm,
			       "ncm_vendor_tx_thread");
    if (IS_ERR(ncm->vendor_ctx.tx_task)) {
        GNET_ERR(("create kthread ncm_tx_thread failed!\n"));
        ncm->vendor_ctx.tx_task_run = false;
		return (int)PTR_ERR(ncm->vendor_ctx.tx_task);
	}

    GNET_TRACE(("%s line %d:leave\n",__FUNCTION__,__LINE__));

    return 0;
}

static void ncm_vendor_tx_thead_stop(struct f_ncm *ncm)
{
    if (ncm->vendor_ctx.tx_task) {
        kthread_stop(ncm->vendor_ctx.tx_task);
        ncm->vendor_ctx.tx_task = NULL;
    }
}

static int ncm_vendor_tx_thread_uninit(struct f_ncm *ncm)
{
    struct sk_buff *skb = NULL;

    if (!ncm)
    {
        return -ENOMEM;
    }

    if (!ncm->vendor_ctx.tx_task_run)
    {
        /* if ncm is disable, the thread will not wakeup,
         * stop it here.
         */
        ncm_vendor_tx_thead_stop(ncm);
        return 0;
    }

    ncm->vendor_ctx.tx_task_run = false;

    complete(&ncm->vendor_ctx.tx_kick);
    wait_for_completion(&ncm->vendor_ctx.tx_task_stop);

    do{
        skb = skb_dequeue(&ncm->vendor_ctx.tx_frames);
        if(skb)
        {
            kfree_skb(skb);
        }
    }while(skb);

    printk("ncm_vendor_tx_thread_uninit ok\n");
    return 0;
}

void ncm_params_init(struct f_ncm *ncm)
{
    spin_lock_init(&(ncm->params.resp_lock));
    INIT_LIST_HEAD(&(ncm->params.resp_queue));
    sema_init(&(ncm->params.sem), 0);

    /* init timer */
    init_timer(&ncm->params.timer);
    ncm->params.timer.function = ncm_response_available_notify;
    ncm->params.timer.data = (unsigned long)ncm;
}

int ncm_params_register(void (*resp_avail)(void *v), void *v)
{
    struct f_ncm *ncm = (struct f_ncm *)v;

	if (!resp_avail)
		return -EINVAL;

    ncm->params.resp_avail = resp_avail;
	ncm->params.v = v;

	return 0;
}

void ncm_params_deregister(struct f_ncm *ncm)
{
	ncm->params.resp_avail = NULL;
	ncm->params.v = NULL;
}
#endif  /* end of CONFIG_ETH_BYPASS_MODE */

static int ncm_set_alt(struct usb_function *f, unsigned intf, unsigned alt)
{
	struct f_ncm		*ncm = func_to_ncm(f);
	struct usb_composite_dev *cdev = f->config->cdev;

#ifdef CONFIG_ETH_BYPASS_MODE
    ncm_app_ctx_t *app_ctx = (ncm_app_ctx_t *)ncm->vendor_ctx.vendor_priv;
#endif

	/* Control interface has only altsetting 0 */
	if ((intf == ncm->ctrl_id && alt == 0 && ncm->is_single_interface)
        || (intf == ncm->ctrl_id && !ncm->is_single_interface)){

		if (alt != 0 && !ncm->is_single_interface)
			goto fail;

		if (ncm->notify->driver_data) {
			USB_DBG_GADGET(&cdev->gadget->dev, "reset ncm control %d\n", intf);
			usb_ep_disable(ncm->notify);
		}

		if (!(ncm->notify->desc)) {
			USB_DBG_GADGET(&cdev->gadget->dev, "init ncm ctrl %d\n", intf);
			if (config_ep_by_speed(cdev->gadget, f, ncm->notify))
				goto fail;
		}
		/* coverity[check_return] */
		usb_ep_enable(ncm->notify);
		ncm->notify->driver_data = ncm;

		bsp_usb_set_enum_stat(ncm->data_id, 1);

	/* Data interface has two altsettings, 0 and 1 */
	} else if (intf == ncm->data_id) {
		if (alt > 1)
			goto fail;
#ifdef CONFIG_ETH_BYPASS_MODE
        if(app_ctx
            && app_ctx->bDataConnect
            && app_ctx->cbs.ndis_stat_change_cb)
        {
            app_ctx->cbs.ndis_stat_change_cb(NCM_IOCTL_STUS_BREAK, NULL);
        }
#endif
		if (ncm->port.in_ep->driver_data) {
			USB_DBG_GADGET(&cdev->gadget->dev, "reset ncm\n");
			gether_disconnect(&ncm->port);
			ncm_reset_values(ncm);
		}

		/*
		 * CDC Network only sends data in non-default altsettings.
		 * Changing altsettings resets filters, statistics, etc.
		 */
		if (alt == 1) {
			struct net_device	*net;

			if (!ncm->port.in_ep->desc ||
			    !ncm->port.out_ep->desc) {
				USB_DBG_GADGET(&cdev->gadget->dev, "init ncm\n");
				if (config_ep_by_speed(cdev->gadget, f,
						       ncm->port.in_ep) ||
				    config_ep_by_speed(cdev->gadget, f,
						       ncm->port.out_ep)) {
					ncm->port.in_ep->desc = NULL;
					ncm->port.out_ep->desc = NULL;
					goto fail;
				}
			}

			/* TODO */
			/* Enable zlps by default for NCM conformance;
			 * override for musb_hdrc (avoids txdma ovhead)
			 */
			ncm->port.is_zlp_ok = !(
				gadget_is_musbhdrc(cdev->gadget)
				);
			ncm->port.cdc_filter = DEFAULT_FILTER;
			USB_DBG_GADGET(&cdev->gadget->dev, "activate ncm\n");
			net = gether_connect(&ncm->port);
			if (IS_ERR(net))
				return PTR_ERR(net);
#ifdef CONFIG_ETH_BYPASS_MODE
            wake_up_process(ncm->vendor_ctx.tx_task);
#endif
		}

		spin_lock(&ncm->lock);
		ncm_notify(ncm);
		spin_unlock(&ncm->lock);
		bsp_usb_set_enum_stat(ncm->data_id, 1);
	} else
		goto fail;

	ncm->curr_alt = alt;

	return 0;
fail:
	ncm->curr_alt = 0;
	return -EINVAL;
}

/*
 * Because the data interface supports multiple altsettings,
 * this NCM function *MUST* implement a get_alt() method.
 */
static int ncm_get_alt(struct usb_function *f, unsigned intf)
{
	struct f_ncm		*ncm = func_to_ncm(f);

	if (intf == ncm->ctrl_id)
		return 0;
	return ncm->port.in_ep->driver_data ? 1 : 0;
}

#ifdef CONFIG_BALONG_NCM
void ncm_free_ntbnode(ntb_node_t *ntb_node)
{
    if(ntb_node)
    {
        if(ntb_node->addr)
        {
            kfree(ntb_node->addr);
        }

        kfree(ntb_node);
    }
}

ntb_node_t *ncm_alloc_ntbnode(u32 size, gfp_t gfp_msk)
{
    ntb_node_t *ntb_node;

    ntb_node = (ntb_node_t *)kzalloc(sizeof(ntb_node_t), gfp_msk);
    if(!ntb_node)
    {
        goto fail;
    }

    ntb_node->addr = kzalloc(size, gfp_msk);
    if(!ntb_node->addr)
    {
        goto fail;
    }

    INIT_LIST_HEAD(&ntb_node->entry);

    return ntb_node;

fail:
    ncm_free_ntbnode(ntb_node);

    return NULL;
}


static inline u32 ncm_get_ntbnode(tx_ctx_t *ctx, u32 size, gfp_t gfp_msk)
{
#ifdef CONFIG_GNET_PREALLOC_TX_MEM
    return 0;
#else
    ntb_node_t *ntb_node = NULL;

    ntb_node = ncm_alloc_ntbnode(size, gfp_msk);
    if(!ntb_node)
        return -ENOMEM;

    ctx->ntb = ntb_node;

    return 0;
#endif
}

static inline void ncm_put_ntbnode(tx_ctx_t *ctx)
{
#ifdef CONFIG_GNET_PREALLOC_TX_MEM
#else
    if(ctx->ntb)
    {
        ncm_free_ntbnode(ctx->ntb);
        ctx->ntb = NULL;
    }
#endif
}

u32 ncm_pad_for_skb(struct sk_buff *skb, u32 ofs, u32 div, u32 rem, bool is_crc)
{
    u32 pad = 0;

    if(!is_crc)
    {
        ofs += skb->len;
        pad = ALIGN(ofs, div) + rem - ofs;  /* pad for ncm align */
        if (unlikely((skb->tail + pad) > skb->end))
        {
            pad |= 0xffff0000;  /* 0xffff0000 means no tailroom */
        }
        else
        {
        	(void)skb_put(skb, pad);
        }

        return pad;
    }
    else
    {
		u32 crc = 0;
        u32 pad0 = 0;
        u32 pad1 = 0;

        /* pad for 802.3 */
        pad0 = (skb->len < ETH_ZLEN)?(ETH_ZLEN-skb->len):0;
        ofs += pad0;
        pad += pad0;

        ofs += 4;   /* 4 bytes for crcs32 */
        pad += 4;

        /* pad for ncm align */
        pad1 = ALIGN(ofs, div) + rem - ofs;
        ofs += pad1;/* [false alarm]:fortify disable */
        pad += pad1;

        if (unlikely((skb->tail + pad) > skb->end))
        {
            pad |= 0xffff0000;  /* 0xffff0000 means no tailroom */
        }
        else
        {
            /* calc crc */
            crc = ~crc32_le(~0,
    				skb->data,
    				skb->len + pad0);
            put_unaligned_le32(crc, skb->data + skb->len + pad0);
            (void)skb_put(skb, pad);
        }

        return pad;
    }
}

u32 ncm_pad_check(u32 pad)
{
    return (u32)(0xffff0000 == (pad & 0xffff0000));
}

struct sk_buff * ncm_skb_expand(tx_ctx_t *ctx, struct sk_buff *skb, u32 pad, u32 div)
{
    struct eth_dev *dev = ctx->dev;
    struct sk_buff  *n = NULL;

    /* alloc more div bytes */
    n = skb_copy_expand(skb, skb_headroom(skb), skb_tailroom(skb) + (pad & 0x0000ffff) + div, GFP_ATOMIC);
    if(!n)
    {
        GNET_ERR(("%s line %d:no room for new skb!\n",__FUNCTION__,__LINE__));
        dev->ctx.stats.tx_skb_tailrom_lack++;
        ctx->pkt_cnt--;
    }
    else
    {
        dev->ctx.stats.tx_skb_tailrom_expand++;
        __skb_insert(n, skb->prev, skb, &ctx->pkt_list);
    }

    __skb_unlink(skb, &ctx->pkt_list);
    dev_kfree_skb_any(skb);
    return n;
}

struct usb_request *ncm_skbs_to_ntb(struct gether *port, struct usb_request *req)
{
#define TRB_CACHE_QUIRK_UPDATE_BYTES(trbs,bytes,maxpacket)\
    do\
    {\
        if((bytes) >= (maxpacket))\
        {\
            (bytes) &= ((maxpacket)-1);\
            (trbs) = (bytes)?1:0;\
        }\
    }while(0)
    struct f_ncm *ncm = func_to_ncm(&port->func);
    struct eth_dev *dev = port->ioport;
    tx_ctx_t *ctx = (tx_ctx_t *)req->context;
	struct sk_buff	*skb_loop;
    struct sk_buff	*skb_next;
	u32 ncb_len = 0;
    u32 ntb_len = 0;
    u32 last_ntb_len = 0;
    u32 last_skb_len = 0;
    u32  pad = 0;
	__le16	*tmp;
    __le16	*tmp_ntb_length;
	int div = le16_to_cpu(ntb_parameters.wNdpInDivisor);
	int	rem = le16_to_cpu(ntb_parameters.wNdpInPayloadRemainder);
	int	ndp_align = le16_to_cpu(ntb_parameters.wNdpInAlignment);
	int	ndp_pad;
	u32 max_size = ncm->port.fixed_in_len;
	struct ndp_parser_opts *opts = ncm->parser_opts;
    u32 cnt = 0;
    u32 trbs_for_quirk = 0;
    u32 bytes_for_quirk = 0;
    u32 maxpacket = port->in_ep->maxpacket;

    /* alloc scatter-gather list */
    if(gnet_get_sgnode(ctx, ctx->pkt_cnt + 1 + dev->ctx.sg_node_reserve, GFP_ATOMIC))   /* add 1 to point to ntb header */
    {
        dev->ctx.stats.tx_no_sg++;
        GNET_BUG(("can't get sgnode\n"),1);

        return NULL;
    }

    /* calc ntb header length */
	ncb_len += opts->nth_size;
	ndp_pad = ALIGN(ncb_len, ndp_align) - ncb_len;
	ncb_len += ndp_pad;
	ncb_len += opts->ndp_size;
	ncb_len += 2 * 2 * opts->dgram_item_len * ctx->pkt_cnt ; /* Datagram entry */
	ncb_len += 2 * 2 * opts->dgram_item_len; /* Zero datagram entry */
	pad = ALIGN(ncb_len, div) + rem - ncb_len;
	ncb_len += pad;

    /* alloc ntb header */
    if(ncm_get_ntbnode(ctx, ncb_len, GFP_ATOMIC))
    {
        dev->ctx.stats.tx_no_ntb++;
        GNET_BUG(("can't get ntbnode\n"),1);

        return NULL;
    }

    /* fill ntb header base info
     */
    /* NTH */
    tmp = (__le16	*)ctx->ntb->addr;

	put_unaligned_le32(opts->nth_sign, tmp); /* dwSignature */
	tmp += 2;
	put_unaligned_le16(opts->nth_size, tmp++);  /* wHeaderLength */
	tmp++; /* skip wSequence */
    tmp_ntb_length = tmp;
	tmp += opts->block_length; /* skip (d)wBlockLength */
	/* (d)wFpIndex */
	/* the first pointer is right after the NTH + align */
	put_ncm(&tmp, opts->fp_index, opts->nth_size + ndp_pad);

	/* NDP */
	tmp = (__le16	*)((u32)tmp + ndp_pad);

	put_unaligned_le32(opts->ndp_sign, tmp); /* dwSignature */
	tmp += 2;

	put_unaligned_le16(ncb_len - opts->nth_size - pad - ndp_pad, tmp++);/* wLength */

	tmp += opts->reserved1;
	tmp += opts->next_fp_index; /* skip reserved (d)wNextFpIndex */
	tmp += opts->reserved2;

    /* fill ntb header */
    cnt = 1;
    ntb_len = ncb_len;
    ctx->sg_node->actual = 0;

    if(dev->ctx.quirks & QUIRK_DWC_TRB_CACHE)
    {
        bytes_for_quirk = ncb_len;
        trbs_for_quirk = 1;
        if(bytes_for_quirk >= maxpacket)
        {
            TRB_CACHE_QUIRK_UPDATE_BYTES(trbs_for_quirk,bytes_for_quirk,maxpacket);
        }
    }

    skb_queue_walk_safe(&ctx->pkt_list,skb_loop,skb_next)
    {
        last_ntb_len = ntb_len;
        last_skb_len = skb_loop->len;
        pad = ncm_pad_for_skb(skb_loop, ntb_len, div, rem, ncm->is_crc);
        if(unlikely(ncm_pad_check(pad)))
        {
            skb_loop = ncm_skb_expand(ctx, skb_loop, pad, div);
            if(unlikely(!skb_loop))
            {
                continue;
            }

            pad = ncm_pad_for_skb(skb_loop, ntb_len, div, rem, ncm->is_crc);
        }
        ntb_len += skb_loop->len;

        if(dev->ctx.quirks & QUIRK_DWC_TRB_CACHE)
        {
            bytes_for_quirk += ntb_len - last_ntb_len;
            if(bytes_for_quirk >= maxpacket)
            {
                TRB_CACHE_QUIRK_UPDATE_BYTES(trbs_for_quirk,bytes_for_quirk,maxpacket);
            }
            else
            {
                u32 trbs_for_quirk_tmp = trbs_for_quirk;
                u32 bytes_for_quirk_tmp = bytes_for_quirk;
                u32 ntb_len_tmp = last_ntb_len;
                u32 actual_tmp = 0;

                trbs_for_quirk++;   /* try to add current trb */
                if((USB_CDC_NCM_DWC_CACHE_TRB_NUM - 1) == trbs_for_quirk)
                {
                    /* temp give up current skb */
                    bytes_for_quirk -= ntb_len - last_ntb_len;
                    skb_trim(skb_loop, last_skb_len);

                    ntb_len = last_ntb_len;
                    ntb_len += maxpacket - bytes_for_quirk; /* fill to maxpacket */
                    #if 0   /* when quirk occur,align is not necessary */
                    pad = ALIGN(ntb_len, div) + rem - ntb_len;  /* pad for ncm align */
                    ntb_len += pad;
                    #endif
                    GNET_TRACE(("line %d:sg[%d]buf 0x%x len 0x%x\n", __LINE__, cnt, (u32)dev->ctx.dwc_trb_cache_buf, ntb_len - last_ntb_len));
                    sg_set_buf(&ctx->sg_node->sg_tbl.sgl[cnt], (void *)dev->ctx.dwc_trb_cache_buf, ntb_len - last_ntb_len);
                    actual_tmp = ctx->sg_node->actual;
                    ctx->sg_node->actual += ntb_len - last_ntb_len;
                    cnt++;

                    bytes_for_quirk += ntb_len - last_ntb_len;
                    TRB_CACHE_QUIRK_UPDATE_BYTES(trbs_for_quirk,bytes_for_quirk,maxpacket);

                    last_ntb_len = ntb_len;
                    last_skb_len = skb_loop->len;
                    pad = ncm_pad_for_skb(skb_loop, ntb_len, div, rem, ncm->is_crc);
                    if(unlikely(ncm_pad_check(pad)))
                    {
                        skb_loop = ncm_skb_expand(ctx, skb_loop, pad, div);
                        if(unlikely(!skb_loop))
                        {
                            trbs_for_quirk = trbs_for_quirk_tmp;
                            bytes_for_quirk = bytes_for_quirk_tmp;
                            ntb_len = ntb_len_tmp;
                            ctx->sg_node->actual = actual_tmp;
                            cnt--;
                            continue;
                        }

                        pad = ncm_pad_for_skb(skb_loop, ntb_len, div, rem, ncm->is_crc);
                    }
                    ntb_len += skb_loop->len;
                    trbs_for_quirk++;
                    bytes_for_quirk += ntb_len - last_ntb_len;
                    TRB_CACHE_QUIRK_UPDATE_BYTES(trbs_for_quirk,bytes_for_quirk,maxpacket);
                }
            }
        }

        dev->ctx.stats.tx_gathered_bytes += skb_loop->len;

    	put_ncm(&tmp, opts->dgram_item_len, last_ntb_len);          /* (d)wDatagramIndex[i] */
    	put_ncm(&tmp, opts->dgram_item_len, skb_loop->len - pad);   /* (d)wDatagramLength[i] */

        GNET_TRACE(("line %d:sg[%d]buf 0x%x len 0x%x\n", __LINE__, cnt, (u32)skb_loop->data, ntb_len - last_ntb_len));
        sg_set_buf(&ctx->sg_node->sg_tbl.sgl[cnt], (const void *)skb_loop->data, ntb_len - last_ntb_len);
        ctx->sg_node->actual += ntb_len - last_ntb_len;
        cnt++;
    }

    /* mark end */
	put_ncm(&tmp, opts->dgram_item_len, 0); /* (d)wDatagramIndex[i] */
	put_ncm(&tmp, opts->dgram_item_len, 0); /* (d)wDatagramIndex[i] */

    ctx->ntb->actual = ncb_len;
    GNET_TRACE(("line %d:sg[%d]buf 0x%x len 0x%x\n", __LINE__, 0, (u32)ctx->ntb->addr, ctx->ntb->actual));
    sg_set_buf(&ctx->sg_node->sg_tbl.sgl[0], (const void *)ctx->ntb->addr, ctx->ntb->actual);    /* let usb core flush cache */
    ctx->sg_node->sg_tbl.nents = cnt;
    if((ctx->ntb->actual + ctx->sg_node->actual) > max_size)
    {
        dev->ctx.stats.tx_ntb_overflow++;
        GNET_BUG(("ntb overflow\n"),1);

        return NULL;
    }
    GNET_BUG(("sg node overflow:%d,%d,%d.\n",cnt,ctx->pkt_cnt,dev->ctx.sg_node_reserve),
        cnt >= (ctx->pkt_cnt + 1 + dev->ctx.sg_node_reserve));

    put_ncm(&tmp_ntb_length, opts->block_length, ntb_len); /* (d)wBlockLength */

    return req;
}

static struct sk_buff *ncm_wrap_ntb(struct gether *port,
				    struct sk_buff *skb)
{
    struct eth_dev *dev = port->ioport;
    struct usb_request *req;
    tx_ctx_t *ctx;
    ulong flags;
    u32 pkt_len_waterline =
        min(port->fixed_in_len - GNET_ROOM_FOR_NTB_INMAXSIZE, dev->ctx.pkt_len_waterline);

    if(skb) /* caller is xmit */
    {
        spin_lock_irqsave(&dev->tx_req_lock, flags);
        if(list_empty_careful(&dev->tx_reqs))
        {
            spin_unlock_irqrestore(&dev->tx_req_lock, flags);
            dev_kfree_skb_any(skb);
            dev->ctx.stats.tx_noreq_inwrap++;
            return NULL;
        }

        req = list_first_entry(&dev->tx_reqs, struct usb_request, list);
        ctx = (tx_ctx_t *)req->context;

        if(skb_queue_empty(&ctx->pkt_list))
        {
            /* start timer */
            dev->ctx.timer.expires = jiffies + dev->ctx.timeout;  /* jiffies unit */
            add_timer(&dev->ctx.timer);
            dev->ctx.stats.tx_timer_activated++;
        }

        /* pkt enqueue */
        skb->used_check = GNET_MAGIC_SKB_USED;
        __skb_queue_tail(&ctx->pkt_list, skb);

        /* update packets info */
        ctx->pkt_cnt++;
        ctx->pkt_len += skb->len;

        if((!skb->psh)
            && (ctx->pkt_cnt < dev->ctx.pkt_cnt_waterline)
            && (ctx->pkt_len < pkt_len_waterline))
        {
            spin_unlock_irqrestore(&dev->tx_req_lock, flags);
            return (struct sk_buff *)GNET_MAGIC_WRAPING;
        }
        else
        {
            /* if waterline reach, construct a ntb wrap */
            del_timer(&dev->ctx.timer); /* delete timer */
            if(skb->psh)
            {
                dev->ctx.stats.tx_psh++;
            }

            dev->ctx.stats.tx_timer_activated--;
            list_del(&req->list);
            spin_unlock_irqrestore(&dev->tx_req_lock, flags);
            GNET_INFO(("%s [xmit]:waterline reach,ctx=%p\n",__FUNCTION__,ctx));
        }
    }
    else    /* caller is timeout */
    {
        GNET_INFO(("%s [timeout]:caller is timeout.\n",__FUNCTION__));

        spin_lock_irqsave(&dev->tx_req_lock, flags);
        if(!list_empty_careful(&dev->tx_done_reqs))    /* ready to send */
        {
            spin_unlock_irqrestore(&dev->tx_req_lock, flags);
            GNET_INFO(("%s line %d:ready to send.\n",__FUNCTION__,__LINE__));
            return (struct sk_buff *)GNET_MAGIC_WRAPED;
        }

        if(list_empty_careful(&dev->tx_reqs))  /* no tx req */
        {
            spin_unlock_irqrestore(&dev->tx_req_lock, flags);
            GNET_ERR(("%s line %d:no tx req.\n",__FUNCTION__,__LINE__));
            dev->ctx.stats.tx_bug_noreq++;
            return (struct sk_buff *)GNET_MAGIC_WRAPING;
        }

        req = list_first_entry(&dev->tx_reqs, struct usb_request, list);
        ctx = (tx_ctx_t *)req->context;
        if(skb_queue_empty(&ctx->pkt_list))
        {
            spin_unlock_irqrestore(&dev->tx_req_lock, flags);
            GNET_ERR(("%s line %d:no packet.\n",__FUNCTION__,__LINE__));
            dev->ctx.stats.tx_bug_nopkt++;
            return (struct sk_buff *)GNET_MAGIC_WRAPING;
        }

        list_del(&req->list);
        spin_unlock_irqrestore(&dev->tx_req_lock, flags);
    }

    if(ncm_skbs_to_ntb(port, req))  /* wrap skbs to ntb according to ncm protocol */
    {
        /* push req to done queue */
        spin_lock_irqsave(&dev->tx_req_lock, flags);
        list_add_tail(&req->list, &dev->tx_done_reqs);
        spin_unlock_irqrestore(&dev->tx_req_lock, flags);
        return (struct sk_buff *)GNET_MAGIC_WRAPED;
    }
    else
    {
        GNET_ERR(("%s line %d:skbs to ntb fail.\n",__FUNCTION__,__LINE__));

        spin_lock_irqsave(&dev->tx_req_lock, flags);
        gnet_recycle_ctx_info((tx_ctx_t *)req->context, 0);  /* memory recycle */
        list_add_tail(&req->list, &dev->tx_reqs);  /* ctx enqueue */
        spin_unlock_irqrestore(&dev->tx_req_lock, flags);

        return NULL;
    }
}
#else   /*of CONFIG_BALONG_NCM*/
static struct sk_buff *ncm_wrap_ntb(struct gether *port,
				    struct sk_buff *skb)
{
	struct f_ncm	*ncm = func_to_ncm(&port->func);
	struct sk_buff	*skb2;
	int		ncb_len = 0;
	__le16		*tmp;
	int		div = ntb_parameters.wNdpInDivisor;
	int		rem = ntb_parameters.wNdpInPayloadRemainder;
	int		pad;
	int		ndp_align = ntb_parameters.wNdpInAlignment;
	int		ndp_pad;
	unsigned	max_size = ncm->port.fixed_in_len;
	struct ndp_parser_opts *opts = ncm->parser_opts;
	unsigned	crc_len = ncm->is_crc ? sizeof(uint32_t) : 0;

	ncb_len += opts->nth_size;
	ndp_pad = ALIGN(ncb_len, ndp_align) - ncb_len;
	ncb_len += ndp_pad;
	ncb_len += opts->ndp_size;
	ncb_len += 2 * 2 * opts->dgram_item_len; /* Datagram entry */
	ncb_len += 2 * 2 * opts->dgram_item_len; /* Zero datagram entry */
	pad = ALIGN(ncb_len, div) + rem - ncb_len;
	ncb_len += pad;

	if (ncb_len + skb->len + crc_len > max_size) {
		dev_kfree_skb_any(skb);
		return NULL;
	}

	skb2 = skb_copy_expand(skb, ncb_len,
			       max_size - skb->len - ncb_len - crc_len,
			       GFP_ATOMIC);
	dev_kfree_skb_any(skb);
	if (!skb2)
		return NULL;

	skb = skb2;

	tmp = (void *) skb_push(skb, ncb_len);
	memset(tmp, 0, ncb_len);

	put_unaligned_le32(opts->nth_sign, tmp); /* dwSignature */
	tmp += 2;
	/* wHeaderLength */
	put_unaligned_le16(opts->nth_size, tmp++);
	tmp++; /* skip wSequence */
	put_ncm(&tmp, opts->block_length, skb->len); /* (d)wBlockLength */
	/* (d)wFpIndex */
	/* the first pointer is right after the NTH + align */
	put_ncm(&tmp, opts->fp_index, opts->nth_size + ndp_pad);

	tmp = (void *)tmp + ndp_pad;

	/* NDP */
	put_unaligned_le32(opts->ndp_sign, tmp); /* dwSignature */
	tmp += 2;
	/* wLength */
	put_unaligned_le16(ncb_len - opts->nth_size - pad, tmp++);

	tmp += opts->reserved1;
	tmp += opts->next_fp_index; /* skip reserved (d)wNextFpIndex */
	tmp += opts->reserved2;

	if (ncm->is_crc) {
		uint32_t crc;

		crc = ~crc32_le(~0,
				skb->data + ncb_len,
				skb->len - ncb_len);
		put_unaligned_le32(crc, skb->data + skb->len);
		skb_put(skb, crc_len);
	}

	/* (d)wDatagramIndex[0] */
	put_ncm(&tmp, opts->dgram_item_len, ncb_len);
	/* (d)wDatagramLength[0] */
	put_ncm(&tmp, opts->dgram_item_len, skb->len - ncb_len);
	/* (d)wDatagramIndex[1] and  (d)wDatagramLength[1] already zeroed */

	if (skb->len > MAX_TX_NONFIXED)
		memset(skb_put(skb, max_size - skb->len),
		       0, max_size - skb->len);

	return skb;
}
#endif  /* end of CONFIG_BALONG_NCM */

struct sk_buff *g_skb = NULL;
static int ncm_unwrap_ntb(struct gether *port,
			  struct sk_buff *skb,
			  struct sk_buff_head *list)
{
	struct f_ncm	*ncm = func_to_ncm(&port->func);
	struct eth_dev  *dev = port->ioport;
	__le16		*tmp = (void *) skb->data;
	unsigned	index, index2;
	unsigned	dg_len, dg_len2;
	unsigned	ndp_len;
	struct sk_buff	*skb2 = NULL;
	int		ret = -EINVAL;
	unsigned	max_size = le32_to_cpu(ntb_parameters.dwNtbOutMaxSize);
	struct ndp_parser_opts *opts = ncm->parser_opts;
	unsigned	crc_len = ncm->is_crc ? sizeof(uint32_t) : 0;
	int		dgram_counter;
    int     doclone = port->ioport->doclone;

	/* dwSignature */
	if (get_unaligned_le32(tmp) != opts->nth_sign) {
		INFO(port->func.config->cdev, "Wrong NTH SIGN, skblen %d\n",
			skb->len);
		print_hex_dump(KERN_INFO, "HEAD:", DUMP_PREFIX_ADDRESS, 32, 1,
			       skb->data, 32, false);

		goto err;
	}
	tmp += 2;
	/* wHeaderLength */
	if (get_unaligned_le16(tmp++) != opts->nth_size) {
		INFO(port->func.config->cdev, "Wrong NTB headersize\n");
		goto err;
	}
	tmp++; /* skip wSequence */

	/* (d)wBlockLength */
	if (get_ncm(&tmp, opts->block_length) > max_size) {
		INFO(port->func.config->cdev, "OUT size exceeded\n");
		goto err;
	}

	index = get_ncm(&tmp, opts->fp_index);
	/* NCM 3.2 */
	if (((index % 4) != 0) && (index < opts->nth_size)) {
		INFO(port->func.config->cdev, "Bad index: %x\n",
			index);
		goto err;
	}

	/* walk through NDP */
	tmp = ((void *)skb->data) + index;
	if (get_unaligned_le32(tmp) != opts->ndp_sign) {
		INFO(port->func.config->cdev, "Wrong NDP SIGN\n");
		goto err;
	}
	tmp += 2;

	ndp_len = get_unaligned_le16(tmp++);
	/*
	 * NCM 3.3.1
	 * entry is 2 items
	 * item size is 16/32 bits, opts->dgram_item_len * 2 bytes
	 * minimal: struct usb_cdc_ncm_ndpX + normal entry + zero entry
	 */
	if ((ndp_len < opts->ndp_size + 2 * 2 * (opts->dgram_item_len * 2))
	    || (ndp_len % opts->ndplen_align != 0)) {
		INFO(port->func.config->cdev, "Bad NDP length: %x\n", ndp_len);
		goto err;
	}
	tmp += opts->reserved1;
	tmp += opts->next_fp_index; /* skip reserved (d)wNextFpIndex */
	tmp += opts->reserved2;

	ndp_len -= opts->ndp_size;
	index2 = get_ncm(&tmp, opts->dgram_item_len);
	dg_len2 = get_ncm(&tmp, opts->dgram_item_len);
	dgram_counter = 0;

	do {
		index = index2;
		dg_len = dg_len2;
		if (dg_len < 14 + crc_len
            || index >= skb->len
            || index + dg_len > skb->len
            || dg_len > ETH_FRAME_LEN + crc_len){ /* ethernet header + crc */
			INFO(port->func.config->cdev, "Bad dgram length: %x\n",dg_len);
            GNET_ERR(("unmatched dgram item:%d,%d,%d\n",index,dg_len,skb->len));
            /*unsigned char log_buf[64] = {0};
            sprintf(log_buf,"unmatched dgram item:%d,%d,%d\n",index,dg_len,skb->len);
            bsp_dump_save_file(DUMP_SAVE_FILE_NCM,log_buf,sizeof(log_buf),DUMP_SAVE_FILE_MODE_CREATE);
            bsp_dump_save_file(DUMP_SAVE_FILE_NCM,skb->data,skb->len,DUMP_SAVE_FILE_MODE_APPEND);*/
            if(gnet_dbg_level & GNET_DBG_RXUNWRAP)
            {
                g_skb = skb;
                BUG();
            }

			goto err;
		}

        if (ncm->is_crc) {
			uint32_t crc, crc2;

			crc = get_unaligned_le32(skb->data +
						 index + dg_len - crc_len);
			crc2 = ~crc32_le(~0,
					 skb->data + index,
					 dg_len - crc_len);
			if (crc != crc2) {
				INFO(port->func.config->cdev, "Bad CRC\n");
				goto err;
			}
		}

		index2 = get_ncm(&tmp, opts->dgram_item_len);
		dg_len2 = get_ncm(&tmp, opts->dgram_item_len);

        if (gnet_rx_need_protect(dev, skb->data + index)) {
            break;
        }

#if !defined(CONFIG_GNET_PREALLOC_RX_MEM)
		if (index2 == 0 || dg_len2 == 0) {
			skb2 = skb;
		} else
#endif
		{
            if (doclone)
                skb2 = skb_clone(skb, GFP_ATOMIC);
            else
                skb2 = gnet_copy_skb(skb, (int)index, (int)(dg_len - crc_len));

			if (skb2 == NULL)
				goto err;
		}

        if (doclone) {
            /* coverity[noescape] */
            if (!skb_pull(skb2, index)) {
                ret = -EOVERFLOW;
                goto err;
		    }

#if !defined(CONFIG_ETH_BYPASS_MODE) && defined(CONFIG_GNET_PREALLOC_RX_MEM)
        /* revise the head to point to the start of the buffer totally owned by this skb.
         * For ncm mode, 16 bytes room is preserved before the position pointed by ndpidx
         * of every ndp, so we set the head pointer 16 bytes ahead of the data pointer.
         */
           skb2->head = skb2->data - ncm->header_room_resved;
#endif

           skb_trim(skb2, dg_len - crc_len);
#if !defined(CONFIG_ETH_BYPASS_MODE)
            if ((skb2 != skb)
                && ip_is_fragment((struct iphdr *)(skb2->data + ETH_HLEN)))
            {
                skb2->truesize = SKB_TRUESIZE(skb2->len);
            }

            make_clone_skb_writable(skb2);
#endif
        }
        /* make sure the psh is cleared */
        skb2->psh = 0;
		skb_queue_tail(list, skb2);

		ndp_len -= 2 * (opts->dgram_item_len * 2);

		dgram_counter++;

		if (index2 == 0 || dg_len2 == 0)
			break;
	} while (ndp_len > 2 * (opts->dgram_item_len * 2)); /* zero entry */

    /* set the psh for the last packet in the ndp */
    if (skb2)
        skb2->psh = 1;
#ifdef CONFIG_BALONG_NCM
    gnet_put_rx_skb_to_done(port->ioport, skb);
#endif

	GNET_TRACE(("Parsed NTB with %d frames\n", dgram_counter));

	return 0;
err:
	skb_queue_purge(list);
#ifdef CONFIG_BALONG_NCM
    gnet_recycle_rx_skb(port->ioport, skb);
#else
	dev_kfree_skb_any(skb);
#endif
	return ret;
}

static void ncm_disable(struct usb_function *f)
{
	struct f_ncm		*ncm = func_to_ncm(f);
#ifdef DEBUG    /*just for clear warning*/
	struct usb_composite_dev *cdev = f->config->cdev;
#endif
#ifdef CONFIG_ETH_BYPASS_MODE
    ncm_app_ctx_t *app_ctx = (ncm_app_ctx_t *)ncm->vendor_ctx.vendor_priv;
#endif

	DBG(cdev, "ncm deactivated\n");

#ifdef CONFIG_ETH_BYPASS_MODE
    ncm_empty_resp_queue(ncm);

    if(app_ctx
        && app_ctx->bDataConnect
        && app_ctx->cbs.ndis_stat_change_cb)
    {
        app_ctx->bDataConnect = false;
        app_ctx->cbs.ndis_stat_change_cb(NCM_IOCTL_STUS_BREAK, NULL);
    }
#endif

	if (ncm->port.in_ep->driver_data)
		gether_disconnect(&ncm->port);

	if (ncm->notify->driver_data) {
		usb_ep_disable(ncm->notify);
		ncm->notify->driver_data = NULL;
		ncm->notify->desc = NULL;
	}
	bsp_usb_set_enum_stat(ncm->data_id, 0);
}
#if defined(CONFIG_BALONG_GNET)
static void ncm_suspend(struct usb_function *f)
{
    struct eth_dev *dev = func_to_ethdev(f);
    struct f_ncm *ncm = func_to_ncm(f);

    if ((!ncm->curr_alt) && (!dev)) {
        GNET_INFO(("ncm function disabled, skip the ncm adapter suspend process\n"));
        return;
    }

    eth_suspend(dev);
}
static void ncm_resume(struct usb_function *f)
{
    struct eth_dev *dev = func_to_ethdev(f);
    struct f_ncm *ncm = func_to_ncm(f);

    if ((!ncm->curr_alt) && (!dev)) {
        GNET_INFO(("ncm function disabled, skip the ncm adapter resume process\n"));
        return;
    }

    eth_resume(dev);
}
#endif
/*-------------------------------------------------------------------------*/

/*
 * Callbacks let us notify the host about connect/disconnect when the
 * net device is opened or closed.
 *
 * For testing, note that link states on this side include both opened
 * and closed variants of:
 *
 *   - disconnected/unconfigured
 *   - configured but inactive (data alt 0)
 *   - configured and active (data alt 1)
 *
 * Each needs to be tested with unplug, rmmod, SET_CONFIGURATION, and
 * SET_INTERFACE (altsetting).  Remember also that "configured" doesn't
 * imply the host is actually polling the notification endpoint, and
 * likewise that "active" doesn't imply it's actually using the data
 * endpoints for traffic.
 */

static void ncm_open(struct gether *geth)
{
	struct f_ncm		*ncm = func_to_ncm(&geth->func);

	USB_DBG_GADGET(&ncm->port.func.config->cdev->gadget->dev, "%s\n", __func__);

	spin_lock(&ncm->lock);
	ncm->is_open = true;
	ncm_notify(ncm);
	spin_unlock(&ncm->lock);
}

static void ncm_close(struct gether *geth)
{
	struct f_ncm		*ncm = func_to_ncm(&geth->func);

	USB_DBG_GADGET(&ncm->port.func.config->cdev->gadget->dev, "%s\n", __func__);

	spin_lock(&ncm->lock);
	ncm->is_open = false;
	ncm_notify(ncm);
	spin_unlock(&ncm->lock);
}

/*-------------------------------------------------------------------------*/

#ifdef CONFIG_ETH_BYPASS_MODE
int ncm_function_bind_config(struct usb_configuration *c)
{
    int ret;
    u8  ethaddr[ETH_ALEN] = {0x00,0x1e,0x10,0x1f,0x00,0x00};

    ret = gether_setup(c->cdev->gadget, ethaddr);
    if (0 == ret)
        ret = ncm_bind_config(c, ethaddr);

    return ret;
}

#endif

/* ethernet function driver setup/binding */
//static int __init
static int
ncm_bind(struct usb_configuration *c, struct usb_function *f)
{
	struct usb_composite_dev *cdev = c->cdev;
	struct f_ncm		*ncm = func_to_ncm(f);
	int			status;
	struct usb_ep		*ep;
    struct usb_descriptor_header **desc;

    ncm->is_mbim_mode = !strcmp(c->label, "config_win8")?true:false;
#ifdef CONFIG_ETH_BYPASS_MODE
    ncm->vendor_ctx.is_mbim_mode = ncm->is_mbim_mode;
#endif
    ncm->is_class_balong = true;

#ifdef CONFIG_ETH_BYPASS_MODE
    status = ncm_bind_func(ncm->vendor_ctx.vendor_priv);
    if (status < 0)
    {
        goto fail;
    }
#endif

	/* allocate instance-specific interface IDs */
	status = usb_interface_id(c, f);
	if (status < 0)
		goto fail;
	ncm->ctrl_id = status;
	ncm_iad_desc.bFirstInterface = status;

	ncm_control_intf.bInterfaceNumber = status;
	ncm_union_desc.bMasterInterface0 = status;

    if (ncm->is_mbim_mode)
    {
        ncm_iad_desc.bFunctionSubClass = 0x0E;
        ndp16_opts.ndp_sign = USB_CDC_MBIM_NDP16_NOCRC_SIGN;
        ndp32_opts.ndp_sign = USB_CDC_MBIM_NDP32_NOCRC_SIGN;
        ncm_control_intf.bInterfaceSubClass = 0x0E;
        ncm_data_nop_intf.bInterfaceProtocol = USB_CDC_MBIM_PROTO_NTB;
        ncm_data_intf.bInterfaceProtocol = USB_CDC_MBIM_PROTO_NTB;

        ncm->is_single_interface = false;
    }
    else
    {
        if (ncm->is_class_balong)
        {
            ncm_control_intf.bInterfaceClass = USB_CLASS_VENDOR_SPEC,
            ncm_control_intf.bInterfaceSubClass = USB_IF_SUBCLASS_BALONG,
            ncm_control_intf.bInterfaceProtocol = USB_IF_PROTOCOL_NCM,
            ncm_data_intf.bInterfaceClass = USB_CLASS_VENDOR_SPEC,
            ncm_data_intf.bInterfaceSubClass = USB_IF_SUBCLASS_BALONG,
            ncm_data_intf.bInterfaceProtocol = USB_IF_PROTOCOL_NCM,
            ncm_data_intf.bNumEndpoints = 3;  /* notify + in + out */

            ncm->is_single_interface = true;
        }
    }

    if(!ncm->is_single_interface)
    {
    	status = usb_interface_id(c, f);
    	if (status < 0)
    		goto fail;
    }

	ncm->data_id = status;
	bsp_usb_add_setup_dev((unsigned)ncm->data_id);

	ncm_data_nop_intf.bInterfaceNumber = status;
	ncm_data_intf.bInterfaceNumber = status;
	ncm_union_desc.bSlaveInterface0 = status;

	status = -ENODEV;

	/* allocate instance-specific endpoints */
	ep = usb_ep_autoconfig(cdev->gadget, &fs_ncm_in_desc);
	if (!ep)
		goto fail;
	ncm->port.in_ep = ep;
	ep->driver_data = cdev;	/* claim */

	ep = usb_ep_autoconfig(cdev->gadget, &fs_ncm_out_desc);
	if (!ep)
		goto fail;
	ncm->port.out_ep = ep;
	ep->driver_data = cdev;	/* claim */

	ep = usb_ep_autoconfig(cdev->gadget, &fs_ncm_notify_desc);
	if (!ep)
		goto fail;
	ncm->notify = ep;
	ep->driver_data = cdev;	/* claim */

	status = -ENOMEM;

	/* allocate notification request and buffer */
	ncm->notify_req = usb_ep_alloc_request(ep, GFP_KERNEL);
	if (!ncm->notify_req)
		goto fail;
	ncm->notify_req->buf = kmalloc(NCM_STATUS_BYTECOUNT, GFP_KERNEL);
	if (!ncm->notify_req->buf)
		goto fail;
	ncm->notify_req->context = ncm;
	ncm->notify_req->complete = ncm_notify_complete;

	/* copy descriptors, and track endpoint copies */
    desc = ncm->is_mbim_mode?ncm_mbim_fs_function:
        (ncm->is_single_interface?ncm_fs_function_single:ncm_fs_function);
	f->descriptors = usb_copy_descriptors(desc);
	if (!f->descriptors)
		goto fail;

	/*
	 * support all relevant hardware speeds... we expect that when
	 * hardware is dual speed, all bulk-capable endpoints work at
	 * both speeds
	 */
	if (gadget_is_dualspeed(c->cdev->gadget)) {
		hs_ncm_in_desc.bEndpointAddress =
				fs_ncm_in_desc.bEndpointAddress;
		hs_ncm_out_desc.bEndpointAddress =
				fs_ncm_out_desc.bEndpointAddress;
		hs_ncm_notify_desc.bEndpointAddress =
				fs_ncm_notify_desc.bEndpointAddress;

		/* copy descriptors, and track endpoint copies */
        desc = ncm->is_mbim_mode?ncm_mbim_hs_function:
            (ncm->is_single_interface?ncm_hs_function_single:ncm_hs_function);
		f->hs_descriptors = usb_copy_descriptors(desc);
		if (!f->hs_descriptors)
			goto fail;
	}

	if (gadget_is_superspeed(c->cdev->gadget)) {
		ss_ncm_in_desc.bEndpointAddress =
				fs_ncm_in_desc.bEndpointAddress;
		ss_ncm_out_desc.bEndpointAddress =
				fs_ncm_out_desc.bEndpointAddress;
		ss_ncm_notify_desc.bEndpointAddress =
				fs_ncm_notify_desc.bEndpointAddress;

		/* copy descriptors, and track endpoint copies */
        desc = ncm->is_mbim_mode?ncm_mbim_ss_function:
            (ncm->is_single_interface?ncm_ss_function_single:ncm_ss_function);
		f->ss_descriptors = usb_copy_descriptors(desc);
		if (!f->ss_descriptors)
			goto fail;
	}

	/*
	 * NOTE:  all that is done without knowing or caring about
	 * the network link ... which is unavailable to this code
	 * until we're activated via set_alt().
	 */
	ncm->port.open = ncm_open;
	ncm->port.close = ncm_close;

	USB_DBG_GADGET(&cdev->gadget->dev, "CDC Network: %s speed IN/%s OUT/%s NOTIFY/%s\n",
            gadget_is_superspeed(c->cdev->gadget) ?"super":
			gadget_is_dualspeed(c->cdev->gadget) ? "dual" : "full",
			ncm->port.in_ep->name, ncm->port.out_ep->name,
			ncm->notify->name);

#ifdef CONFIG_ETH_BYPASS_MODE
    (void)ncm_params_register(ncm_response_available, (void * )ncm);

    status = ncm_vendor_tx_thread_init(ncm);
    if (status)
    {
        printk(KERN_ERR "ncm vendor tx thread init failed.\n");
        goto fail;
    }
#endif

	return 0;

fail:
#ifdef CONFIG_ETH_BYPASS_MODE
    ncm_params_deregister(ncm);
    ncm_vendor_tx_thread_uninit(ncm);
#endif

    if(f->hs_descriptors)
        usb_free_descriptors(f->hs_descriptors);
    if(f->ss_descriptors)
        usb_free_descriptors(f->ss_descriptors);
	if (f->descriptors)
		usb_free_descriptors(f->descriptors);

	if (ncm->notify_req) {
		kfree(ncm->notify_req->buf);
		usb_ep_free_request(ncm->notify, ncm->notify_req);
	}

	/* we might as well release our claims on endpoints */
	if (ncm->notify)
		ncm->notify->driver_data = NULL;
	if (ncm->port.out_ep)
		ncm->port.out_ep->driver_data = NULL;
	if (ncm->port.in_ep)
		ncm->port.in_ep->driver_data = NULL;

	ERROR(cdev, "%s: can't bind, err %d\n", f->name, status);

	return status;
}

static void
ncm_unbind(struct usb_configuration *c, struct usb_function *f)
{
	struct f_ncm		*ncm = func_to_ncm(f);

	GNET_BUG(("ncm_unbind:null the_ncm!\n"),(NULL == the_ncm[ncm->port.port_no]));

	DBG(c->cdev, "ncm unbind\n");

#ifdef CONFIG_ETH_BYPASS_MODE
    ncm_params_deregister(ncm);
    (void)ncm_vendor_tx_thread_uninit(ncm);

    ncm_unbind_func(ncm->vendor_ctx.vendor_priv);
#endif

	if (gadget_is_dualspeed(c->cdev->gadget))
		usb_free_descriptors(f->hs_descriptors);
    if(f->ss_descriptors)
        usb_free_descriptors(f->ss_descriptors);
	if (f->descriptors)
		usb_free_descriptors(f->descriptors);

	kfree(ncm->notify_req->buf);
	usb_ep_free_request(ncm->notify, ncm->notify_req);

	ncm_string_defs[1].s = NULL;

#ifdef CONFIG_ETH_BYPASS_MODE
    ncm_vendor_uninit(&ncm->vendor_ctx);
#endif

	the_ncm[ncm->port.port_no] = NULL;
    glink_idx--;
	kfree(ncm);
}

/**
 * ncm_bind_config - add CDC Network link to a configuration
 * @c: the configuration to support the network link
 * @ethaddr: a buffer in which the ethernet address of the host side
 *	side of the link was recorded
 * Context: single threaded during gadget setup
 *
 * Returns zero on success, else negative errno.
 *
 * Caller must have called @gether_setup().  Caller is also responsible
 * for calling @gether_cleanup() before module unload.
 */
//int __init ncm_bind_config(struct usb_configuration *c, u8 ethaddr[ETH_ALEN])
int ncm_bind_config(struct usb_configuration *c, u8 ethaddr[ETH_ALEN])
{
	struct f_ncm	*ncm;
	int		status;

	GNET_BUG(("ncm_bind_config:glink_idx(%d) overflow!\n",glink_idx),(glink_idx >= GNET_USED_NUM));

	if (the_ncm[glink_idx])
		return -EBUSY;

	if (!can_support_ecm(c->cdev->gadget) || !ethaddr)
		return -EINVAL;

	/* maybe allocate device-global string IDs */
	if (ncm_string_defs[0].id == 0) {

		/* control interface label */
		status = usb_string_id(c->cdev);
		if (status < 0)
			return status;
		ncm_string_defs[STRING_CTRL_IDX].id = status;
		ncm_control_intf.iInterface = status;

		/* data interface label */
		status = usb_string_id(c->cdev);
		if (status < 0)
			return status;
		ncm_string_defs[STRING_DATA_IDX].id = status;
		ncm_data_nop_intf.iInterface = status;
		ncm_data_intf.iInterface = status;

		/* MAC address */
		status = usb_string_id(c->cdev);
		if (status < 0)
			return status;
		ncm_string_defs[STRING_MAC_IDX].id = status;
		ecm_desc.iMACAddress = status;

		/* IAD */
		status = usb_string_id(c->cdev);
		if (status < 0)
			return status;
		ncm_string_defs[STRING_IAD_IDX].id = status;
		ncm_iad_desc.iFunction = status;
	}

	/* allocate and initialize one new instance */
	ncm = kzalloc(sizeof *ncm, GFP_KERNEL);
	if (!ncm)
		return -ENOMEM;

	/* export host's Ethernet address in CDC format */
	snprintf(ncm->ethaddr, sizeof ncm->ethaddr,
		"%02X%02X%02X%02X%02X%02X",
		ethaddr[0], ethaddr[1], ethaddr[2],
		ethaddr[3], ethaddr[4], ethaddr[5]);
	ncm_string_defs[1].s = ncm->ethaddr;

	spin_lock_init(&ncm->lock);
	ncm_reset_values(ncm);
	ncm->port.is_fixed = true;

	ncm->port.func.name = "ncm_network";
	ncm->port.func.strings = ncm_strings;
	/* descriptors are per-instance copies */
	ncm->port.func.bind = ncm_bind;
	ncm->port.func.unbind = ncm_unbind;
	ncm->port.func.set_alt = ncm_set_alt;
	ncm->port.func.get_alt = ncm_get_alt;
	ncm->port.func.setup = ncm_setup;
	ncm->port.func.disable = ncm_disable;
#if defined(CONFIG_BALONG_GNET)
    ncm->port.func.suspend = ncm_suspend;
    ncm->port.func.resume = ncm_resume;
#endif

	ncm->port.wrap = ncm_wrap_ntb;
	ncm->port.unwrap = ncm_unwrap_ntb;
#ifdef CONFIG_ETH_BYPASS_MODE  /* it's alias is 'stick mode' */
    ncm->port.handle_rx = ncm_vendor_rx;

    ncm_params_init(ncm);

    ncm->vendor_ctx.ncm_priv = ncm;
    status = ncm_vendor_init(&ncm->vendor_ctx);
    if (status)
    {
        kfree(ncm);
        return status;
    }
#endif

    status = usb_add_function(c, &ncm->port.func);
	if (status) {
		ncm_string_defs[1].s = NULL;

#ifdef CONFIG_ETH_BYPASS_MODE
        ncm_vendor_uninit(&ncm->vendor_ctx);
#endif
        kfree(ncm);
        return status;
	}

	ncm->port.port_no = glink_idx;
	the_ncm[glink_idx] = ncm;
	glink_idx++;
	return status;
}

void f_ncm_dump(void)
{
    int idx;
    struct f_ncm *ncm;

    for (idx = 0; idx < GNET_MAX_NUM; idx++) {
        ncm = the_ncm[idx];
        if (ncm) {
            printk("notify_state:       %d\n", ncm->notify_state);
            printk("host_timeout:       %d\n", ncm->host_timeout);
            printk("stat_host_timeout:  %d\n", ncm->stat_host_timeout);
        }
    }
}
/*lint -restore*/
