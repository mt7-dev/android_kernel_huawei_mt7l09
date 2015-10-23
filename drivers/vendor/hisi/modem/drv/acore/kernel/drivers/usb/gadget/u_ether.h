/*
 * u_ether.h -- interface to USB gadget "ethernet link" utilities
 *
 * Copyright (C) 2003-2005,2008 David Brownell
 * Copyright (C) 2003-2004 Robert Schwebel, Benedikt Spranger
 * Copyright (C) 2008 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __U_ETHER_H
#define __U_ETHER_H

#if defined(CONFIG_BALONG_NCM) || defined(CONFIG_BALONG_RNDIS)
#define USB_NET_IP_ALIGN               (0)  /* no need to align */
#endif
#if defined(CONFIG_BALONG_ECM)
/* ip align for ecm */
#define USB_NET_IP_ALIGN               (2 + max(32, L1_CACHE_BYTES))
#endif

#include <linux/err.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/usb/composite.h>
#include <linux/usb/cdc.h>

#include "gadget_chips.h"
#if defined(CONFIG_BALONG_GNET)
#include "bsp_usb.h"
#include "usb_vendor.h"
#endif

struct eth_dev;

#define	WORK_RX_MEMORY		                (0)
#define	WORK_RX_UPTASK                      (1)
#define	WORK_RX_BOTTROM                     (WORK_RX_UPTASK + 1)
#define RX_FRAMES_MAX                       (2048)

#ifdef CONFIG_BALONG_GNET
/* gnet msg level */
#define GNET_LEVEL_ERR	    BIT(0)
#define GNET_LEVEL_TRACE	BIT(1)
#define GNET_LEVEL_DBG	    BIT(2)
#define GNET_LEVEL_INFO	    BIT(3)
#define GNET_LEVEL_BUG	    BIT(4)
#define GNET_LEVEL_SKB	    BIT(5)
#define GNET_LEVEL_BYTE	    BIT(6)

extern unsigned int gnet_msg_level;

/* gnet dbg level */
#define GNET_DBG_DROPUDP    BIT(0)
#define GNET_DBG_DROPALL    BIT(1)
#define GNET_DBG_DROPREQ    BIT(2)
#define GNET_DBG_RX2ADS     BIT(3)
#define GNET_DBG_RXUNWRAP   BIT(4)
#define GNET_DBG_TX2TASK    BIT(5)
#define GNET_DBG_XFERMON    BIT(6)

#define GNET_RX_PROTECT_PKT        120
#define GNET_RX_PROTECT_TIME       1

extern unsigned int gnet_dbg_level;

#define GNET_ERR(args)			do {if (gnet_msg_level & GNET_LEVEL_ERR) printk args;} while (0)
#define GNET_BUG(args,condition)          \
    do {\
        if (condition)\
        {\
            printk args;\
        }\
\
        if (gnet_msg_level & GNET_LEVEL_BUG)\
        {\
            BUG_ON(condition);\
        }\
    } while (0)

#ifdef CONFIG_GNET_DEBUG
#define GNET_TRACE(args)		do {if (gnet_msg_level & GNET_LEVEL_TRACE) printk args;} while (0)
#define GNET_DBG(args)			do {if (gnet_msg_level & GNET_LEVEL_DBG) printk args;} while (0)
#define GNET_INFO(args)		    do {if (gnet_msg_level & GNET_LEVEL_INFO) printk args;} while (0)
static inline void print_bytes(struct eth_dev *dev, char *data, u32 len)
{
    if(gnet_msg_level & GNET_LEVEL_BYTE)
    {
        u32 cnt = 0;

        printk("------pkt start------\n");
        for(cnt=0; cnt<len; cnt++)
        {
            printk("%02x ",data[cnt]);
        }
        printk("------pkt end  ------\n");
    }
}
static inline void print_skb(struct eth_dev *dev, struct sk_buff *skb)
{
    if(gnet_msg_level & GNET_LEVEL_SKB)
    {
        u32 cnt = 0;
        u32 len = skb->len<=64?skb->len:64;

        printk("skb data(len %d):\n",skb->len);
        for(cnt=0; cnt<len; cnt++)
        {
            printk("%02x ",skb->data[cnt]);
        }
        printk("\n");
    }
}
static inline int gnet_rx_isdrop(struct sk_buff *skb)
{
    if(likely(!(gnet_dbg_level & (GNET_DBG_DROPALL | GNET_DBG_DROPUDP))))
    {
        return 0;
    }
    else if((gnet_dbg_level & GNET_DBG_DROPUDP
            && 0x11 == skb->data[0x17])
            || (gnet_dbg_level & GNET_DBG_DROPALL))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
#else   /* CONFIG_GNET_DEBUG */
#define GNET_TRACE(args)
#define GNET_DBG(args)
#define GNET_INFO(args)
static inline void print_bytes(struct eth_dev *dev, char *data, u32 len)
{   return ;    }
static inline void print_skb(struct eth_dev *dev, struct sk_buff *skb)
{   return ;    }
static inline int gnet_rx_isdrop(struct sk_buff *skb)
{   return 0;   }
#endif  /* CONFIG_GNET_DEBUG */

/* match with pc usb driver */
#define GNET_CLASS_TYPE_HUAWEI
#define GNET_SINGLE_INTERFACE

/* common macro for gnet */
#define GNET_RX_MEM_PREALLOC                (1*1024*1024)

/* SKB MAGIC MACRO */
#define GNET_MAGIC_WRAPING                  (0x55AA5AA5)
#define GNET_MAGIC_WRAPED                   (GNET_MAGIC_WRAPING + 1)

#define GNET_MAGIC_SKB_USED                 (0xbeef)

/* it should be very caution to use the 1 jiffy timer */
#define USB_CDC_GNET_TIMEOUT                (2) /* jiffies unit */

#ifdef CONFIG_BALONG_RNDIS
#define GNET_MAX_PKT_PER_TRANSFER           (10)
#define GNET_MAX_TRANSFER                   (0x4000) /* 16k */
#define GNET_ALIGN_FACTOR                   (2) /* word alignment */
#define GNET_ALIGN_LEN                      (1 << GNET_ALIGN_FACTOR)

/* definitions for rndis optimation */
#define GNET_OUT_MAXSIZE_PER_TRANSFER       (2*1024) /* 2k */
#define GNET_IN_MAXSIZE_PER_TRANSFER        (GNET_MAX_TRANSFER)
#define GNET_PKT_LEN_WATERLINE              (120*1024)/* meaningless actually */
#define GNET_PKT_NUM_WATERLINE              (40)

#define QUIRK_DWC_TRB_CACHE                 BIT(0)
#define RX_REQ_WATERLINE_DIV                (2)
#endif  /* CONFIG_BALONG_RNDIS */

#if defined(CONFIG_BALONG_NCM)
#define GNET_MAX_PKT_PER_TRANSFER           (50)
#define GNET_OUT_MAXSIZE_PER_TRANSFER       (16*1024)
#define GNET_IN_MAXSIZE_PER_TRANSFER        (128*1024) /* 128k bytes */
#define GNET_PKT_LEN_WATERLINE              (60*1024)
#define GNET_PKT_NUM_WATERLINE              (50)

#define QUIRK_DWC_TRB_CACHE                 BIT(0)
#define RX_REQ_WATERLINE_DIV                (4)

#define USB_CDC_NCM_NTB_IN_MAXSIZE          (128*1024)
#define USB_CDC_NCM_NDP_IN_DIVISOR          (4)
#define USB_CDC_NCM_NDP_IN_REMAINDER        (2)
#define USB_CDC_NCM_NDP_IN_ALIGNMENT        (4)

#define USB_CDC_NCM_NDP_OUT_DIVISOR         (4)
#define USB_CDC_NCM_NDP_OUT_REMAINDER       (2)
#define USB_CDC_NCM_NDP_OUT_ALIGNMENT       (4)

#define USB_CDC_NCM_NDP16_MIN			    (0x0010)
#define USB_CDC_NCM_NDP32_MIN			    (0x0020)

#define GNET_ROOM_FOR_NTB_INMAXSIZE     (4096)
#endif  /* CONFIG_BALONG_NCM */

#if defined(CONFIG_BALONG_ECM)
#define GNET_MAX_PKT_PER_TRANSFER           (1)
#define GNET_OUT_MAXSIZE_PER_TRANSFER       (2*1024)
#define GNET_IN_MAXSIZE_PER_TRANSFER        (2*1024)
#define GNET_PKT_LEN_WATERLINE              (120*1024)/* meaningless actually */
#define GNET_PKT_NUM_WATERLINE              (40)

#define QUIRK_DWC_TRB_CACHE                 (0)
#define RX_REQ_WATERLINE_DIV                (2)
#endif  /* CONFIG_BALONG_ECM */

#ifdef CONFIG_BALONG_NCM
typedef struct
{
    struct list_head    entry;
    void * addr;
    unsigned int size;
    unsigned int actual;
}ntb_node_t;
#endif  /* CONFIG_BALONG_NCM */

typedef struct
{
    struct list_head    entry;
    struct sg_table sg_tbl;
    u32 actual;
}sg_node_t;

typedef struct
{
    struct eth_dev *dev;
    struct sk_buff_head    pkt_list;
    sg_node_t *sg_node;
#ifdef CONFIG_BALONG_NCM
    ntb_node_t *ntb;
#endif
    struct usb_request *req;

    u32 pkt_cnt;
    u32 pkt_len;
}tx_ctx_t;

typedef struct
{
    ulong dev_no_wrap;
    ulong dev_no_port;
    ulong dev_wrap_fail;
    ulong dev_disconnect;
    ulong dev_connect;
    ulong dev_open;
    ulong dev_start;
    ulong dev_stop;
    ulong dev_suspend;
    ulong dev_resume;

    /* tx */
    ulong tx_total;
    ulong tx_ok_pkts;
    ulong tx_ok_bytes;
    ulong tx_err_pkts;
    ulong tx_err_bytes;
    ulong tx_inep_null;
    ulong tx_dropped;
    ulong tx_filt_out;
    ulong tx_drop_suspend;
    ulong tx_no_ctx;
    ulong tx_no_sg;
    ulong tx_no_ntb;
    ulong tx_ntb_overflow;
    ulong tx_no_req;
    ulong tx_noreq_inwrap;
    ulong tx_bug_noreq;
    ulong tx_bug_nopkt;
    ulong tx_no_donereq;
    ulong tx_stop_queue;
    ulong tx_prestop_queue;
    ulong tx_wake_queue;
    ulong tx_req_dropped;
    ulong tx_complete_ok;
    ulong tx_complete_fail;
    ulong tx_by_timer;
    ulong tx_timer_activated;
    ulong tx_skb_realloc;
    ulong tx_gathered_bytes;
    ulong tx_psh;
    ulong tx_skb_check_err;
    ulong tx_skb_tailrom_lack;
    ulong tx_skb_tailrom_expand;
	ulong tx_zlp;
    ulong tx_expand_zlp;
	ulong tx_expand_zlp1;
    ulong tx_quirk;

    /* rx */
    ulong rx_zlp;
    ulong rx_no_skb;
    ulong rx_req_submitfail;
    ulong rx_req_manudrop;
    ulong rx_skb_protect_drop;
    ulong rx_skb_recycle_delay;
    ulong rx_completed;
    ulong rx_complete_ok;
    ulong rx_unwrap_fail;
    ulong rx_packets;
    ulong rx_bytes;
    ulong rx_packets_fail;
    ulong rx_bytes_fail;
    ulong rx_packets_err;
    ulong rx_packets_overwline;
    ulong rx_kevent_busy[WORK_RX_BOTTROM];
    ulong rx_kevent_ok[WORK_RX_BOTTROM];
}gnet_stat_t;

struct eth_dev_ctx
{
	u32 eth_idx;
    u32 sg_node_nents;
    u32 sg_node_reserve;

#ifdef CONFIG_BALONG_NCM
    u32 ntb_node_size;
#endif
    u8 *dwc_trb_cache_buf;
#if CONFIG_GNET_PREALLOC_RX_MEM
    struct sk_buff_head *rx_skb_free_q;
    struct sk_buff_head *rx_skb_done_q;
    u32 rx_skb_num;
    u32 rx_skb_waterline;
    u32 rx_skb_size;
#endif
    struct timer_list   timer;
    u32 timeout;
    u32 pkt_cnt_waterline;
    u32 pkt_len_waterline;
    u32	quirks;
    u32 dbg_flags;
    u32 protect_num;
    unsigned long protect_jiff;
    gnet_stat_t stats;
};
#else   /*!CONFIG_BALONG_GNET*/
#define GNET_ERR(args)
#define GNET_BUG(args,condition)
#define GNET_TRACE(args)
#define GNET_DBG(args)
#define GNET_INFO(args)
static inline void print_bytes(struct eth_dev *dev, char *data, u32 len)
{   return ;    }
static inline void print_skb(struct eth_dev *dev, struct sk_buff *skb)
{   return ;    }
static inline int gnet_rx_isdrop(struct sk_buff *skb)
{   return 0;   }
#endif  /* CONFIG_BALONG_GNET */

struct eth_dev {
	/* lock is held while accessing port_usb
	 * or updating its backlink port_usb->ioport
	 */
	spinlock_t		lock;
	struct gether		*port_usb;

	struct net_device	*net;
	struct usb_gadget	*gadget;

	spinlock_t		req_lock;	/* guard {rx,tx}_reqs */
    spinlock_t		tx_req_lock;    /* guard tx_reqs */
	struct list_head	tx_reqs, rx_reqs;
#ifdef CONFIG_BALONG_GNET
    struct list_head    tx_done_reqs;
#endif
	atomic_t		tx_qlen;
    atomic_t		rx_qlen;
    u32 tx_req_num;
    u32 rx_req_num;
    u32 rx_req_waterline;

	struct sk_buff_head	rx_frames;

	unsigned		header_len;
	struct sk_buff		*(*wrap)(struct gether *, struct sk_buff *skb);
	int			(*unwrap)(struct gether *,
						struct sk_buff *skb,
						struct sk_buff_head *list);
#ifdef CONFIG_ETH_BYPASS_MODE
    int (*handle_rx)(struct gether *port,
                 struct sk_buff *skb);
    struct sk_buff_head	rx_frames_to_ads;
    u32 rx_frames_waterline;
    struct work_struct rx_work;
#endif
	struct delayed_work	work;

	unsigned long		todo;
	u32			zlp:1;
    u32         doclone:1;
    u32         is_suspend:1;
    u32         remote_wakeup_en:1;
	u8			host_mac[ETH_ALEN];

#ifdef CONFIG_BALONG_GNET
    struct eth_dev_ctx  ctx;
	struct device_attribute dev_attr;
	char dev_sysfs_name[64];
#endif
};

#if defined USB_ETH_RNDIS
#  undef USB_ETH_RNDIS
#endif
#ifdef CONFIG_USB_ETH_RNDIS
#  define USB_ETH_RNDIS y
#endif

/*
 * This represents the USB side of an "ethernet" link, managed by a USB
 * function which provides control and (maybe) framing.  Two functions
 * in different configurations could share the same ethernet link/netdev,
 * using different host interaction models.
 *
 * There is a current limitation that only one instance of this link may
 * be present in any given configuration.  When that's a problem, network
 * layer facilities can be used to package multiple logical links on this
 * single "physical" one.
 */
struct gether {
	struct usb_function		func;

	/* updated by gether_{connect,disconnect} */
	struct eth_dev			*ioport;

	/* endpoints handle full and/or high speeds */
	struct usb_ep			*in_ep;
	struct usb_ep			*out_ep;

	bool				is_zlp_ok;

	u16				cdc_filter;

	/* hooks for added framing, as needed for RNDIS and EEM. */
	u32				header_len;
	/* NCM requires fixed size bundles */
	bool				is_fixed;
	u32				fixed_out_len;
	u32				fixed_in_len;
	struct sk_buff			*(*wrap)(struct gether *port,
						struct sk_buff *skb);
	int				(*unwrap)(struct gether *port,
						struct sk_buff *skb,
						struct sk_buff_head *list);
#ifdef CONFIG_ETH_BYPASS_MODE
    int (*handle_rx)(struct gether *port,
                   struct sk_buff *skb);
#endif
	/* called on network open/close */
	void				(*open)(struct gether *);
	void				(*close)(struct gether *);

	u32 port_no;	/* indicates which function port */
};

#define	DEFAULT_FILTER	(USB_CDC_PACKET_TYPE_BROADCAST \
			|USB_CDC_PACKET_TYPE_ALL_MULTICAST \
			|USB_CDC_PACKET_TYPE_PROMISCUOUS \
			|USB_CDC_PACKET_TYPE_DIRECTED)


/* netdev setup/teardown as directed by the gadget driver */
int gether_setup(struct usb_gadget *g, u8 ethaddr[ETH_ALEN]);
void gether_cleanup(void);
/* variant of gether_setup that allows customizing network device name */
int gether_setup_name(struct usb_gadget *g, u8 ethaddr[ETH_ALEN],
		const char *netname);

/* connect/disconnect is handled by individual functions */
struct net_device *gether_connect(struct gether *);
void gether_disconnect(struct gether *);

/* Some controllers can't support CDC Ethernet (ECM) ... */
static inline bool can_support_ecm(struct usb_gadget *gadget)
{
	if (!gadget_supports_altsettings(gadget))
		return false;

	/* Everything else is *presumably* fine ... but this is a bit
	 * chancy, so be **CERTAIN** there are no hardware issues with
	 * your controller.  Add it above if it can't handle CDC.
	 */
	return true;
}

/* each configuration may bind one instance of an ethernet link */
int geth_bind_config(struct usb_configuration *c, u8 ethaddr[ETH_ALEN]);
int ecm_bind_config(struct usb_configuration *c, u8 ethaddr[ETH_ALEN]);
int ncm_bind_config(struct usb_configuration *c, u8 ethaddr[ETH_ALEN]);
int eem_bind_config(struct usb_configuration *c);

#ifdef USB_ETH_RNDIS

int rndis_bind_config(struct usb_configuration *c, u8 ethaddr[ETH_ALEN]);
int rndis_bind_config_vendor(struct usb_configuration *c, u8 ethaddr[ETH_ALEN],
				u32 vendorID, const char *manufacturer);

#else

static inline int
rndis_bind_config(struct usb_configuration *c, u8 ethaddr[ETH_ALEN])
{
	return 0;
}

static inline int
rndis_bind_config_vendor(struct usb_configuration *c, u8 ethaddr[ETH_ALEN],
				u32 vendorID, const char *manufacturer)
{
	return 0;
}

#endif

#if defined(CONFIG_BALONG_GNET)
void free_sg_node(sg_node_t *sg_node);
sg_node_t *alloc_sg_node(u32 nents, gfp_t gfp_msk);
static inline int32_t gnet_get_sgnode(tx_ctx_t *ctx, u32 nents, gfp_t gfp_msk);
static inline void gnet_put_sgnode(tx_ctx_t *ctx);
struct sk_buff *gnet_get_rx_skb(struct eth_dev  *dev, u32 size, gfp_t gfp_msk);
void gnet_recycle_rx_skb(struct eth_dev  *dev, struct sk_buff *skb);
void gnet_put_rx_skb_to_done(struct eth_dev  *dev, struct sk_buff *skb);
static inline void free_tx_ctx(tx_ctx_t *tx_ctx);
int32_t alloc_tx_ctx(struct eth_dev *dev, unsigned n);
void free_rx_mem(struct eth_dev *dev);
int32_t alloc_rx_mem(struct eth_dev *dev);
void gnet_recycle_ctx_info(tx_ctx_t *ctx, uint success);
void gnet_timeout_handle(unsigned long data);
void make_clone_skb_writable(struct sk_buff *skb);
struct sk_buff * gnet_copy_skb(struct sk_buff *skb, int offset, int len);
int gnet_rx_need_protect(struct eth_dev *dev, unsigned char* data);
void gnet_set_rx_clone(struct eth_dev *dev, u32 doclone);
static void eth_suspend(struct eth_dev *dev);
static void eth_resume(struct eth_dev *dev);

static inline struct eth_dev *func_to_ethdev(struct usb_function *f)
{
    struct gether *port = container_of(f, struct gether, func);
	return port->ioport;
}
/**
 * gnet_wakeup_gadget - wakeup the gadget to normal mode
 * @dev: the gent dev
 * Context: may soft irq
 *
 * This function invokes the gadget-ops->wakeup to resume the gadget.
 *
 * Returns:
 *  0		:success;
 *  Other 	:failed to wakeup the host.
 */
static int gnet_wakeup_gadget(struct eth_dev *dev)
{
    if (dev->gadget->ops && dev->gadget->ops->wakeup) {
        return dev->gadget->ops->wakeup(dev->gadget);
    }

    return 1;
}
#else   /*!CONFIG_BALONG_GNET*/
static int gnet_wakeup_gadget(struct eth_dev *dev)
{
    return 0;
}
#endif

#endif /* __U_ETHER_H */
