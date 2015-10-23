/*lint -save -e43 -e62 -e63 -e84 -e115 -e413 -e539 -e613 -e614 -e615 -e617 -e958*/
/*
 * u_modem.c - utilities for USB gadget "modem port" support
 *
 */

/* #define VERBOSE_DEBUG */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>

#include "u_serial.h"
#include "drv_acm.h"
#include "drv_udi.h"

#define ACM_MODEM_PREFIX             "acm_"
#define ACM_MODEM_DRV_NAME           "acm_modem"
#define ACM_MODEM_QUEUE_SIZE         16
#define ACM_MODEM_DFT_RD_BUF_SIZE    1536
#define ACM_MODEM_DFT_RD_REQ_NUM     8
#define ACM_MODEM_DFT_WT_REQ_NUM     256

#define ACM_MODEM_NAME_MAX   64

extern struct class *tty_class;

/* cdev driver */
struct u_modem_driver {
	struct kref kref;           /* Reference management */
	struct cdev cdev;
    dev_t dev_no;
    struct module *owner;
	const char *driver_name;
	const char *name;
	int	name_base;              /* offset of printed name */
	int	major;                  /* major device number */
	int	minor_start;            /* start of minor device number */
	int	num;                    /* number of devices allocated */

    struct workqueue_struct *acm_work_queue;

};

struct gs_acm_modem_sglist_info {
    struct scatterlist *sg_list;
    u32 valid;
	u32 item_num;
	u32 index;
	u32 total_size;
	struct gs_acm_modem_sglist_info *next;
    struct sk_buff_head skb_list;
    struct gs_acm_modem_port *port;
};

struct gs_acm_modem_sglist_ctx {
	struct gs_acm_modem_sglist_info sg_list_array[ACM_MODEM_SG_LIST_NUM];
	u32 submit_pos;
    u32 done_pos;
    u32 item_num_threshold;
    u32 buffer_size_threshold;
    u32 tx_timeout;
    u32 tx_timeout_on;
};

/*
 * The port structure holds info for each port, one for each minor number
 * (and thus for each /dev/ node).
 */
struct gs_acm_modem_port {
	spinlock_t port_lock;               /* guard port_* access */
	struct gserial *port_usb;
	unsigned open_count;
	bool openclose;	                    /* open/close in progress */
	u32 port_num;
	wait_queue_head_t close_wait;       /* wait for last close */
    ACM_EVENT_CB_T event_notify_cb;
    u16 line_state;
    u16 line_state_change;
    int sr_change;
    int is_suspend;
    char *in_name;
    char *out_name;
    struct gs_acm_modem_sglist_ctx sglist_ctx;
    bool ps_recv_flow;
    MODEM_MSC_STRU cur_flow_msg;

    char read_domain[4];
	struct list_head read_pool;         /* free read req list */
    struct list_head read_done_queue;   /* done read req list */
    struct list_head read_queue_in_usb; /* reqs in usb core */
    struct sk_buff_head skb_free_list;  /* skb list can be reused */
    struct sk_buff_head skb_done_list;  /* skb list in using */

	int read_started;
	int read_allocated;
    int read_req_enqueued;
    int read_completed;
    unsigned read_req_num;
    unsigned read_buf_size;
    struct usb_request	*reading_req;
    wait_queue_head_t read_wait;
    ACM_READ_DONE_CB_T read_done_cb;

    char write_domain[4];
	struct list_head write_pool;        /* free write req list */
    unsigned write_req_num;
	int write_started;
	int write_allocated;
    int write_completed;
    wait_queue_head_t write_wait;
    ACM_WRITE_DONE_CB_T write_done_cb;
    ACM_FREE_CB_T write_done_free_cb;
    ACM_MODEM_MSC_READ_CB_T read_sig_cb;
    ACM_MODEM_REL_IND_CB_T rel_ind_cb;

	struct delayed_work	rw_work;
	struct usb_cdc_line_coding port_line_coding;	/* 8-N-1 etc */
    unsigned is_realloc;
    wait_queue_head_t realloc_wait;

    char debug_tx_domain[4];
    unsigned stat_write_async_call;
    unsigned stat_write_submit;
    unsigned stat_tx_submit;
    unsigned stat_tx_submit_fail;
    unsigned stat_write_bytes;
    unsigned stat_tx_submit_bytes;
    unsigned stat_tx_done;
    unsigned stat_tx_done_fail;
    unsigned stat_tx_done_bytes;
    unsigned stat_tx_no_req;
    unsigned stat_tx_suspend;
    unsigned stat_tx_fifo_full;
    unsigned stat_tx_inv_param;
    unsigned stat_tx_disconnect;
    unsigned stat_timeout_tx;
    unsigned stat_direct_tx;

    char debug_rx_domain[4];
    unsigned stat_get_buf_call;
    unsigned stat_ret_buf_call;
    unsigned stat_rx_submit;
    unsigned stat_rx_submit_fail;
    unsigned stat_rx_disconnect;
    unsigned stat_rx_no_req;
    unsigned stat_rx_done;
    unsigned stat_rx_done_fail;
    unsigned stat_rx_done_bytes;
    unsigned stat_rx_done_disconnect;
    unsigned stat_rx_done_schdule;
    unsigned stat_rx_callback;
    unsigned stat_rx_cb_not_start;
    unsigned stat_alloc_skb;
    unsigned stat_alloc_skb_fail;
    unsigned stat_free_skb;

    char debug_port_domain[4];
    unsigned stat_port_is_connect;
    unsigned stat_port_connect;
    unsigned stat_port_disconnect;
    unsigned stat_enable_in_fail;
    unsigned stat_enable_out_fail;
    unsigned stat_notify_sched;
    unsigned stat_notify_on_cnt;
    unsigned stat_notify_off_cnt;
    unsigned stat_read_sig_cnt;

};

struct gs_acm_modem_rw_priv {
    struct list_head list;
    struct usb_request *req;
    void * rw_ctx;
    struct kiocb *iocb;
};

static struct acm_modem_port_manager {
    char name_domain[4];
	struct mutex	open_close_lock;			/* protect open/close */
	struct gs_acm_modem_port	*port;
    struct device	*cdev;
} gs_acm_modem_ports[ACM_MDM_COUNT];

static struct gs_acm_evt_manage gs_modem_write_evt_manage;
static struct gs_acm_evt_manage gs_modem_read_evt_manage;
static struct gs_acm_evt_manage gs_modem_sig_stat_evt_manage;


static unsigned	gs_acm_modem_n_ports;
static unsigned gs_modem_drv_invalid;
static unsigned gs_modem_port_num_err;

extern struct acm_name_type_tbl g_acm_mdm_type_table[ACM_MDM_COUNT];

#define ACM_MODEM_GET_NAME(index) (g_acm_mdm_type_table[index].name == NULL) ? \
                                ("unknown") : (g_acm_mdm_type_table[index].name)

static struct u_modem_driver *gs_modem_driver;

static void gs_acm_modem_free_request(struct usb_ep *ep, struct usb_request	*req);
static struct usb_request* gs_acm_modem_alloc_request(struct usb_ep *ep, u32 buf_size, u32 is_mtrans);
static void gs_acm_modem_free_requests(struct usb_ep *ep, struct list_head *head,\
							 int *allocated, struct gs_acm_modem_port *port);
static void gs_acm_modem_write_complete(struct usb_ep *ep, struct usb_request *req);
static void gs_acm_modem_read_complete(struct usb_ep *ep, struct usb_request *req);
static struct sk_buff *gs_acm_modem_alloc_skb(struct gs_acm_modem_port *port, u32 size);
static void gs_acm_modem_free_skb(struct gs_acm_modem_port *port, struct sk_buff *skb);
static void gs_acm_modem_free_skb_list(struct gs_acm_modem_port *port,
                    struct sk_buff_head *skb_list);
/*-------------------------------------------------------------------------*/

/*
 * gs_acm_modem_start_tx
 *
 * This function finds available write requests, calls
 * usb_ep_queue to send the data.
 *
 */
static int gs_acm_modem_start_tx(struct gs_acm_modem_port *port,
                                 struct gs_acm_modem_sglist_info *sglist_info)
{
	struct list_head *pool = &port->write_pool;
	int status = 0;
    struct usb_request *req;
    struct gs_acm_modem_rw_priv *write_priv;
    struct scatterlist *sg_list_item;

    while (port->sglist_ctx.done_pos != sglist_info->next->index) {

        if (unlikely(list_empty(pool))) {
            port->stat_tx_no_req++;
            return -EAGAIN;
        }

        if (0 == sglist_info->item_num) {
            break;
        }

        /* get a write req from the write pool */
        req = list_entry(pool->next, struct usb_request, list);
        list_del_init(&req->list);
        port->write_started++;

        /* mark the last sg item */
        sg_list_item = sglist_info->sg_list + sglist_info->item_num - 1;
        sg_mark_end(sg_list_item);

        /* fill the req */
    	req->buf = NULL;
        req->sg = sglist_info->sg_list;
        req->num_sgs = sglist_info->item_num;
        req->length = sglist_info->total_size;
        write_priv = (struct gs_acm_modem_rw_priv *)req->context;
        write_priv->rw_ctx = (void*)sglist_info;

        if (!port->port_usb) {
            status = -ENODEV;
            port->stat_tx_disconnect++;
            goto tx_fail_restore;
        }

        /* check zlp */
        if (req->length % port->port_usb->in->maxpacket == 0) {
            req->zero = 1;
        }
        else {
            req->zero = 0;
        }


        /* drop lock while we call out; the controller driver
		 * may need to call us back (e.g. for disconnect)
		 */
        spin_unlock(&port->port_lock);
    	status = usb_ep_queue(port->port_usb->in, req, GFP_ATOMIC);
    	spin_lock(&port->port_lock);
        if (status) {
    		port->stat_tx_submit_fail++;
    		goto tx_fail_restore;
    	}
        port->stat_tx_submit_bytes += sglist_info->total_size;
        port->stat_tx_submit++;

        /* move the submit pos */
        sglist_info = sglist_info->next;
        port->sglist_ctx.submit_pos = sglist_info->index;
    }

    return 0;

tx_fail_restore:
	list_add_tail(&req->list, pool);
    port->write_started--;
	return status;
}

/*
 * Context: caller owns port_lock, and port_usb is set
 */
static unsigned gs_acm_modem_start_rx(struct gs_acm_modem_port *port)
{
	struct list_head	*pool = &port->read_pool;
	/* coverity[deref_ptr] */
	struct usb_ep		*out = port->port_usb->out;
    struct usb_request	*req;
    struct gs_acm_modem_rw_priv *rw_priv;
    struct sk_buff      *new_skb;

start_rx_beg:
	while (!list_empty(pool)) {
		int status;

		req = list_entry(pool->next, struct usb_request, list);
		list_del_init(&req->list);
        port->read_started++;

        rw_priv = (struct gs_acm_modem_rw_priv *)req->context;
        /*
         * if req length is 0, it need to alloc a new skb
         * else the req is already alloced, we can reuse the skb
         */
        if (0 == req->length) {
            new_skb = gs_acm_modem_alloc_skb(port, port->read_buf_size);
            if (!new_skb) {
                goto start_rx_ret;
            }
            req->complete = gs_acm_modem_read_complete;
            req->buf = new_skb->data;
            rw_priv->rw_ctx = (void*)new_skb;
        }
        req->length = port->read_buf_size;

		/* drop lock while we call out; the controller driver
		 * may need to call us back (e.g. for disconnect)
		 */
		spin_unlock(&port->port_lock);
		status = usb_ep_queue(out, req, GFP_ATOMIC);
		spin_lock(&port->port_lock);

		if (status) {
            /* if usb queue fail, we can reuse the skb in the rw_priv */
			list_add(&req->list, pool);
            port->read_started--;
            port->stat_rx_submit_fail++;
			goto start_rx_ret;
		}

        list_add_tail(&rw_priv->list, &port->read_queue_in_usb);
        port->read_req_enqueued++;
        port->stat_rx_submit++;

		/* abort immediately after disconnect */
		/* coverity[deref_ptr] */
		if (!port->port_usb) {
            port->stat_rx_disconnect++;
			goto start_rx_ret;
        }
	}

    /* if there are no read req in usb core,
     * get the read done req and submit to usb core
     */

	/* coverity[deref_ptr] */
    if (port->port_usb && 0 == port->read_req_enqueued) {
        struct list_head *queue = &port->read_done_queue;

        if (!list_empty(queue)) {
            req = list_entry(queue->prev, struct usb_request, list);
            list_move(&req->list, pool);
            port->read_started--;

            /* go to beginning of the function,
             * re-submit the read req
             */
            port->stat_rx_no_req++;
            goto start_rx_beg;
        }
    }

start_rx_ret:
	return port->read_started;/* [false alarm]:fortify disable */
}

static int gs_acm_modem_get_read_buf(struct gs_acm_modem_port *port,
                                     ACM_WR_ASYNC_INFO *read_info)
{
    struct list_head *queue = &port->read_done_queue;
    struct usb_request	*req = NULL;
    struct sk_buff *skb;
    int status;
    unsigned long flags;

    spin_lock_irqsave(&port->port_lock, flags);
    if (!list_empty(queue)) {
        req = list_first_entry(queue, struct usb_request, list);
    }

    if (NULL == req) {
        read_info->pBuffer = NULL;
        read_info->u32Size = 0;
        status = -EAGAIN;
    }
    else {
        struct gs_acm_modem_rw_priv *rw_priv;
        rw_priv = (struct gs_acm_modem_rw_priv *)req->context;
        skb = (struct sk_buff *)rw_priv->rw_ctx;
        /* copy skb info to usr */
        skb_put(skb, req->actual);

        read_info->pBuffer = (char*)skb;
        read_info->u32Size = req->actual;

        /* reset the req member and move it to read pool list */
        req->length = 0;
        req->buf = NULL;
        rw_priv->rw_ctx = NULL;
        list_move(&req->list, &port->read_pool);
        port->read_started--;
        status = 0;
    }
    spin_unlock_irqrestore(&port->port_lock, flags);
    return status;
}

static int gs_acm_modem_ret_read_buf(struct gs_acm_modem_port *port,
                                     ACM_WR_ASYNC_INFO *read_info)
{
    struct sk_buff *free_skb = (struct sk_buff *)read_info->pBuffer;

    gs_acm_modem_free_skb(port, free_skb);
    return 0;
}

static int gs_acm_modem_write_signal(struct gs_acm_modem_port *port,
                                     MODEM_MSC_STRU* modem_msc)
{
    u32 capability = 0;
    struct gserial	*gser = port->port_usb;

    if (!gser) {
        return -ENODEV;
    }

    if (SIGNALCH == modem_msc->OP_Cts) {
        if (SIGNALNOCH == modem_msc->ucCts) {
            /* disable ps flow flag */
            port->ps_recv_flow = RECV_DISABLE;
        }
        else {
            /* enable ps flow flag */
            port->ps_recv_flow = RECV_ENABLE;
        }
    }

    /* notify flow control to host */
    if (SIGNALCH == modem_msc->OP_Cts) {
        if (SIGNALNOCH == modem_msc->ucCts) {
            /* enable rx flow control */
            gser->flow_control(gser, RECV_DISABLE, SEND_ENABLE);
        }
        else {
            /* disable rx flow control */
            gser->flow_control(gser, RECV_ENABLE, SEND_ENABLE);
        }
    }
    if((SIGNALCH == modem_msc->OP_Ri)
		||(SIGNALCH == modem_msc->OP_Dsr )
		||(SIGNALCH == modem_msc->OP_Dcd )) {

	    if ((SIGNALCH == modem_msc->OP_Ri)
	        && (HIGHLEVEL == modem_msc->ucRi)) {
	        capability |= U_ACM_CTRL_RING;
	    };

	    /* DSR SIGNAL CHANGE */
	    if ((SIGNALCH == modem_msc->OP_Dsr)
	        && (HIGHLEVEL == modem_msc->ucDsr)){
	        capability |= U_ACM_CTRL_RTS;
	    };

	    /* DCD SIGNAL CHANGE */
	    if ((SIGNALCH == modem_msc->OP_Dcd)
	        && (HIGHLEVEL == modem_msc->ucDcd)) {
	        capability |= U_ACM_CTRL_DTR;
	    };

        gser->notify_state(gser, (u16)capability);
    }

    return 0;
}


static void gs_acm_modem_notify_cb(struct gs_acm_modem_port *port)
{
    ACM_EVENT_CB_T event_cb = NULL;
    ACM_MODEM_MSC_READ_CB_T read_sig_cb = NULL;
    u16 line_state;
    bool old_dtr;
    MODEM_MSC_STRU* new_flow_msg = &port->cur_flow_msg;
    unsigned long flags;

    spin_lock_irqsave(&port->port_lock, flags);
    line_state = port->line_state;
    old_dtr = port->cur_flow_msg.ucDtr;

    if (port->line_state_change) {
        event_cb = port->event_notify_cb;
        read_sig_cb = port->read_sig_cb;

        if (line_state & U_ACM_CTRL_DTR) {
            new_flow_msg->ucDtr = HIGHLEVEL;
        }
        else {
        	new_flow_msg->ucDtr = LOWLEVEL;
        }

        if (line_state & U_ACM_CTRL_RTS) {
        	new_flow_msg->ucRts = HIGHLEVEL;
        }
        else {
        	new_flow_msg->ucRts = LOWLEVEL;
        }
        port->line_state_change = 0;
    }

    /* if modem is suspended, pull down the DTR signal */
    if (port->sr_change) {
        read_sig_cb = port->read_sig_cb;
        event_cb = port->event_notify_cb;
        new_flow_msg->ucDtr = LOWLEVEL;
        port->sr_change = 0;
        line_state = 0;
    }

    spin_unlock_irqrestore(&port->port_lock, flags);

    if (event_cb) {
        if (line_state) {
            port->stat_notify_on_cnt++;
        }
        else {
            port->stat_notify_off_cnt++;
        }
        event_cb((ACM_EVT_E)(line_state & U_ACM_CTRL_DTR));
    }

    if(old_dtr != new_flow_msg->ucDtr && read_sig_cb) {
        new_flow_msg->OP_Dtr = SIGNALCH;
        port->stat_read_sig_cnt++;
        read_sig_cb(new_flow_msg);
        new_flow_msg->OP_Dtr = SIGNALNOCH;
    }

    return;
}


static void gs_acm_modem_read_cb(struct gs_acm_modem_port *port)
{
    struct list_head *queue = &port->read_done_queue;
    struct usb_request	*req = NULL;
    ACM_READ_DONE_CB_T read_cb = NULL;
    unsigned long flags;
    int status = 0;

    for (;;) {
        spin_lock_irqsave(&port->port_lock, flags);
        if (port->read_completed <= 0) {
            spin_unlock_irqrestore(&port->port_lock, flags);
            return;
        }
        port->read_completed--;

        if (!list_empty(queue)) {
            req = list_first_entry(queue, struct usb_request, list);
            status= req->status;

            /* if there are data in queue, prepare the read callback */
            if (!req->status && port->open_count) {
                read_cb = port->read_done_cb;
            }
            else {
                list_move(&req->list, &port->read_pool);
                port->read_started--;
            }
        }

        /* submit the next read req */
        if (-ESHUTDOWN != status && port->port_usb) {
		    gs_acm_modem_start_rx(port);
        }
        else {
            port->stat_rx_cb_not_start++;
        }
        spin_unlock_irqrestore(&port->port_lock, flags);

        if (NULL != read_cb) {
            port->stat_rx_callback++;
            read_cb();
        }
    }
}

static void gs_acm_modem_timeout_tx_cb(struct gs_acm_modem_port *port)
{
    struct gs_acm_modem_sglist_info *sglist_info;
    unsigned long flags;

    spin_lock_irqsave(&port->port_lock, flags);
    port->stat_timeout_tx++;
    sglist_info = &port->sglist_ctx.sg_list_array[port->sglist_ctx.submit_pos];
    (void)gs_acm_modem_start_tx(port, sglist_info);
    port->sglist_ctx.tx_timeout_on = 0;
    spin_unlock_irqrestore(&port->port_lock, flags);
}

/*
 * rw workqueue takes data out of the RX queue and hands it up to the TTY
 * layer until it refuses to take any more data (or is throttled back).
 * Then it issues reads for any further data.
 */
static void gs_acm_modem_rw_push(struct work_struct *work)
{
	struct gs_acm_modem_port *port;

    /* notify callback */
    while (NULL != (port = gs_acm_evt_get(&gs_modem_sig_stat_evt_manage))) {
        gs_acm_modem_notify_cb(port);
    }

    /* read callback */
    while (NULL != (port = gs_acm_evt_get(&gs_modem_read_evt_manage))) {
        gs_acm_modem_read_cb(port);
    }

    /* timeout tx callback */
    while (NULL != (port = gs_acm_evt_get(&gs_modem_write_evt_manage))) {
        gs_acm_modem_timeout_tx_cb(port);
    }

    /* other callback ... */

    return;
}

static void gs_acm_modem_read_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct gs_acm_modem_port	*port = ep->driver_data;
    struct gs_acm_modem_rw_priv *rw_priv;

	/* Queue all received data until the tty layer is ready for it. */
	spin_lock(&port->port_lock);

    if (!req->status) {
        port->stat_rx_done++;
        port->stat_rx_done_bytes += req->actual;
    }
    else {
        port->stat_rx_done_fail++;
    }

    rw_priv = (struct gs_acm_modem_rw_priv *)req->context;
    list_del_init(&rw_priv->list);
    port->read_req_enqueued--;

    if (port->port_usb && req->actual) {
    	list_add_tail(&req->list, &port->read_done_queue);
        port->stat_rx_done_schdule++;
        port->read_completed++;
        gs_acm_evt_push(port, &gs_modem_read_evt_manage);
        queue_delayed_work(gs_modem_driver->acm_work_queue, &port->rw_work, 0);
    }
    else {
        gs_acm_modem_free_skb(port, (struct sk_buff *)rw_priv->rw_ctx);
        req->length = 0;
        req->buf = NULL;
        list_add_tail(&req->list, &port->read_pool);

        port->read_started--;
        port->stat_rx_done_disconnect++;
    }
	spin_unlock(&port->port_lock);
}

static void gs_acm_modem_write_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct gs_acm_modem_port	*port = ep->driver_data;
    struct gs_acm_modem_rw_priv *write_priv;
    struct gs_acm_modem_sglist_info *sglist_info;

	spin_lock(&port->port_lock);
    write_priv = (struct gs_acm_modem_rw_priv *)req->context;
    sglist_info = (struct gs_acm_modem_sglist_info *)write_priv->rw_ctx;

    if (!req->status) {
        port->stat_tx_done++;
        port->stat_tx_done_bytes += req->actual;
    }
    else {
        port->stat_tx_done_fail++;
    }

    port->write_completed++;

    gs_acm_modem_free_skb_list(port, &sglist_info->skb_list);

    sglist_info->item_num = 0;
    sglist_info->total_size = 0;

    /* move the done pos */
    port->sglist_ctx.done_pos = sglist_info->index;

    list_add_tail(&req->list, &port->write_pool);
    port->write_started--;

	spin_unlock(&port->port_lock);
}

/* ---------------- modem skb allocation ---------------- */

static struct sk_buff *
gs_acm_modem_alloc_skb(struct gs_acm_modem_port *port, u32 size)
{
    struct sk_buff *new_skb;

    new_skb = dev_alloc_skb(size);
    if (unlikely(!new_skb)) {
        port->stat_alloc_skb_fail++;
    }

    return new_skb;
}

static void
gs_acm_modem_free_skb(struct gs_acm_modem_port *port, struct sk_buff *skb)
{
    dev_kfree_skb_any(skb);
}

static void gs_acm_modem_free_skb_list(struct gs_acm_modem_port *port,
                    struct sk_buff_head *skb_list)
{
    struct sk_buff *skb, *skbnext;

    skb_queue_walk_safe(skb_list, skb, skbnext) {
        gs_acm_modem_free_skb(port, skb);
    }
    __skb_queue_head_init(skb_list);
}

/* ---------------- modem req managemenet --------------- */
static void gs_acm_modem_free_request(struct usb_ep *ep, struct usb_request	*req)
{
    if (req->context) {
        kfree(req->context);
        req->context = NULL;
    }
	gs_free_req(ep, req);
}

static void gs_acm_modem_free_requests(struct usb_ep *ep, struct list_head *head,
							 int *allocated, struct gs_acm_modem_port *port)
{
	struct usb_request	*req;
	struct gs_acm_modem_rw_priv *rw_priv;

	while (!list_empty(head)) {
		req = list_entry(head->next, struct usb_request, list);
		list_del_init(&req->list);

        rw_priv = (struct gs_acm_modem_rw_priv *)req->context;
        if (req->buf) {
    		gs_acm_modem_free_skb(port, (struct sk_buff *)rw_priv->rw_ctx);
            req->length = 0;
            req->buf = NULL;
        }
        gs_acm_modem_free_request(ep, req);
		if (allocated)
			(*allocated)--;
	}
}

static struct usb_request*
gs_acm_modem_alloc_request(struct usb_ep *ep, u32 buf_size, u32 is_mtrans)
{
    struct gs_acm_modem_rw_priv *rw_priv;
    struct usb_request	*req;

    req = gs_alloc_req(ep, buf_size, GFP_ATOMIC);

    if (!req) {
        pr_emerg("=================== gs_alloc_req fail, line:%d\n", __LINE__);
		return NULL;
    }

    rw_priv = (struct gs_acm_modem_rw_priv *)
              kzalloc(sizeof(struct gs_acm_modem_rw_priv), GFP_ATOMIC);
    if (!rw_priv) {
        pr_emerg("==================== kzalloc fail, line:%d\n", __LINE__);
        gs_free_req(ep, req);
        return NULL;
    }
    req->context = (void*)rw_priv;
    if (is_mtrans) {
        req->sg_mode = USB_REQUEST_M_TRANSFER;
    }
    rw_priv->req = req;
    INIT_LIST_HEAD(&rw_priv->list);

    return req;
}

static int gs_acm_modem_alloc_requests(struct usb_ep *ep, struct list_head *head,
		void (*fn)(struct usb_ep *, struct usb_request *),
		int *allocated, u32 buf_size, u32 buf_num, u32 is_mtrans)
{
	int			i;
	struct usb_request	*req;

	int n = allocated ? buf_num - *allocated : buf_num;

	for (i = 0; i < n; i++) {

        req = gs_acm_modem_alloc_request(ep, buf_size, is_mtrans);
        if (!req)
            return list_empty(head) ? -ENOMEM : 0;
		req->complete = fn;
		list_add_tail(&req->list, head);
		if (allocated)
			(*allocated)++;
	}
	return 0;
}

/*
 * Context: holding port_lock;
 */
static int gs_acm_modem_prepare_io(struct gs_acm_modem_port *port)
{
    struct list_head	*head = &port->read_pool;
	struct usb_ep		*ep = port->port_usb->out;
	int			status;

    /* out ep use block buffer */
    status = gs_acm_modem_alloc_requests(ep, head, gs_acm_modem_read_complete,
		&port->read_allocated, 0, port->read_req_num, 0);
	if (status)
		return status;

    /* in ep use sg_list multitranfer mode */
	status = gs_acm_modem_alloc_requests(port->port_usb->in, &port->write_pool,
			gs_acm_modem_write_complete, &port->write_allocated,
			0, port->write_req_num, 0);
	if (status) {
		gs_acm_modem_free_requests(ep, head, &port->read_allocated, port);
		return status;
	}

    return 0;
}

/*-------------------------------------------------------------------------*/

static inline int gs_acm_modem_get_port_num(struct inode *inode)
{
    int		port_num;

    if (!gs_modem_driver) {
        gs_modem_drv_invalid++;
		return -ENXIO;
    }

    port_num = inode->i_rdev - gs_modem_driver->dev_no;

    if (port_num >= (int)gs_acm_modem_n_ports) {
        gs_modem_port_num_err++;
		return -ENXIO;
    }

    return port_num;
}

static int gs_acm_modem_open(struct inode *inode, struct file *filp)
{
	int	port_num;
	struct gs_acm_modem_port	*port;
	int	status;
    unsigned long flags;

    port_num = gs_acm_modem_get_port_num(inode);
    if (port_num < 0) {
        return port_num;
    }

	do {
		mutex_lock(&gs_acm_modem_ports[port_num].open_close_lock);
		port = gs_acm_modem_ports[port_num].port;
		if (!port) {
		    mutex_unlock(&gs_acm_modem_ports[port_num].open_close_lock);
			return -ENODEV;
		}
		else {
			spin_lock_irqsave(&port->port_lock, flags);

			/* already open?  Great. */
			if (port->open_count) {
				status = 0;
				port->open_count++;

			/* currently opening/closing? wait ... */
			} else if (port->openclose) {
				status = -EBUSY;

			/* ... else we do the work */
			} else {
				status = -EAGAIN;
				port->openclose = true;
			}
			spin_unlock_irqrestore(&port->port_lock, flags);
		}
		mutex_unlock(&gs_acm_modem_ports[port_num].open_close_lock);

		switch (status) {
		default:
			/* fully handled */
			return status;
		case -EAGAIN:
			/* must do the work */
			break;
		case -EBUSY:
			/* wait for EAGAIN task to finish */
			msleep(10);
			/* REVISIT could have a waitchannel here, if
			 * concurrent open performance is important
			 */
			break;
		}
	} while (status != -EAGAIN);

    spin_lock_irqsave(&port->port_lock, flags);
	port->open_count = 1;
	port->openclose = false;

#if ACM_MODEM_SUPPORT_NOTIFY
    /* if connected, start the I/O stream */
	if (port->port_usb) {
        struct gserial	*gser = port->port_usb;
		if (gser->connect)
			gser->connect(gser);
	}
#endif
    spin_unlock_irqrestore(&port->port_lock, flags);

	return 0;
}

static int gs_acm_modem_close(struct inode *inode, struct file *file)
{
	struct gs_acm_modem_port *port;
    unsigned long flags;
    int status;

    status = gs_acm_modem_get_port_num(inode);
    if (status < 0)
        return status;
    port = gs_acm_modem_ports[status].port;

	spin_lock_irqsave(&port->port_lock, flags);

	if (port->open_count != 1) {
		if (port->open_count == 0) {
			WARN_ON(1);
            return -EBADF;
        }
		else {
			--port->open_count;
        }
		goto exit;
	}

	/* mark port as closing but in use; we can drop port lock
	 * and sleep if necessary
	 */
	port->openclose = true;
	port->open_count = 0;

	/* restore the state */
	memset(&port->cur_flow_msg, 0, sizeof(MODEM_MSC_STRU));
	port->ps_recv_flow = 0;
	port->line_state = 0;
	port->line_state_change = 0;

#if ACM_MODEM_SUPPORT_NOTIFY
    {
        struct gserial	*gser;
    	gser = port->port_usb;
    	if (gser && gser->disconnect)
    		gser->disconnect(gser);
    }
#endif

	port->openclose = false;

	wake_up_interruptible(&port->close_wait);
exit:
	spin_unlock_irqrestore(&port->port_lock, flags);
    return 0;
}

static int gs_acm_modem_write_base(struct gs_acm_modem_port *port,
                                   struct sk_buff* skb)
{
	int status = 0;
    unsigned long flags;
    struct gs_acm_modem_sglist_info *sglist_info;
    struct scatterlist *sg_list_item;

    if (unlikely(NULL == skb || 0 == skb->len)) {
        port->stat_tx_inv_param++;
        return -EINVAL;
    }

    port->stat_write_bytes += skb->len;
    spin_lock_irqsave(&port->port_lock, flags);

    /* if suspend, could not send data */
    if (unlikely(port->is_suspend)) {
        port->stat_tx_suspend++;
        status = -ESHUTDOWN;
        goto write_base_exit;
    }

    /* get current sglist info */
    sglist_info = &port->sglist_ctx.sg_list_array[port->sglist_ctx.submit_pos];

    /* check tx fifo pos */
    if (port->sglist_ctx.done_pos == sglist_info->next->index) {
        port->stat_tx_fifo_full++;
        status = -EAGAIN;
        goto write_base_exit;
    }

    /* skb enqueue */
    __skb_queue_tail(&sglist_info->skb_list, skb);

    /* fill the sg_list */
    sg_list_item = sglist_info->sg_list + sglist_info->item_num;
    sg_list_item->page_link = 0;
    sg_set_buf(sg_list_item, (const void *)skb->data, skb->len);

    /* update the sglist info */
    sglist_info->item_num++;
    sglist_info->total_size += skb->len;

#if 0
    /* check zlp */
    if (skb->len % port->port_usb->in->maxpacket == 0) {
        sg_list_item = sglist_info->sg_list + sglist_info->item_num;
        sg_list_item->page_link = 0;
        /* we use pre skb data and length to 0, to send zlp */
        sg_set_buf(sg_list_item, (const void *)skb->data, 0);
        sglist_info->item_num++;
    }
#endif

    /* check whether can be submit to usb core*/
    if ((sglist_info->item_num >= port->sglist_ctx.item_num_threshold)
        || (sglist_info->total_size > port->sglist_ctx.buffer_size_threshold)
        /*|| (sglist_info->item_num == (ACM_MODEM_SG_LIST_ITEM_NUM - 1))*/) {

        port->stat_direct_tx++;
        status = gs_acm_modem_start_tx(port, sglist_info);
        if (status) {
            goto write_base_exit;
        }
    }
    else if (sglist_info->item_num && !port->sglist_ctx.tx_timeout_on) {
        port->sglist_ctx.tx_timeout_on = 1;
        gs_acm_evt_push(port, &gs_modem_write_evt_manage);
        queue_delayed_work(gs_modem_driver->acm_work_queue,
            &port->rw_work, port->sglist_ctx.tx_timeout);
    }

write_base_exit:
    spin_unlock_irqrestore(&port->port_lock, flags);
	return status;
}

/**
 *	gs_acm_modem_write		-	write method for tty device file
 *	@file: acm file pointer
 *	@buf: user data to write
 *	@count: bytes to write
 *	@ppos: unused
 *
 *	Write data to a acm device.
 *
 */
static ssize_t gs_acm_modem_write(struct file *file, const char __user *buf,
						          size_t count, loff_t *ppos)
{
    struct inode *inode = file->f_path.dentry->d_inode;
    int		port_num;
	struct gs_acm_modem_port	*port;
    int status;

    if (unlikely(NULL == buf)) {
        pr_err("%s invalid param buf:%p, count:%d\n", __FUNCTION__, buf, count);
        return -EFAULT;
    }

    if (get_fs() != KERNEL_DS) {
        pr_err("can't support in usr space\n");
        return -ENOTSUPP;
    }

    port_num = gs_acm_modem_get_port_num(inode);
    if (port_num < 0)
        return port_num;

    port = gs_acm_modem_ports[port_num].port;

    port->stat_write_submit++;
    status = gs_acm_modem_write_base(port, (struct sk_buff*)buf);

    return (ssize_t)status;
}

/**
 *	gs_acm_modem_read		-	read method for modem device file
 *	@file: acm file pointer
 *	@buf: user data to read
 *	@count: bytes to read
 *	@ppos: unused
 *
 *	read data form a acm device: we don't support
 *
 */
static ssize_t gs_acm_modem_read(struct file *file, char __user *buf,
						         size_t count, loff_t *ppos)
{
    return -ENOTSUPP;
}

long gs_acm_modem_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct inode *inode = file->f_path.dentry->d_inode;
    ACM_WR_ASYNC_INFO *rw_info;
    ACM_READ_BUFF_INFO *rd_info;
    struct gs_acm_modem_port	*port;
    int	status;

    if (get_fs() != KERNEL_DS) {
        printk(KERN_ERR "can't support in usr space\n");
        return -ENOTSUPP;
    }

    status = gs_acm_modem_get_port_num(inode);
    if (status < 0) {
        printk(KERN_ERR "gs_acm_modem_get_port_num err, status=%d\n",status);
        return status;
    }

    port = gs_acm_modem_ports[status].port;
    if (!port) {
        printk(KERN_ERR "modem is disconnect\n");
        return -ESHUTDOWN;
    }

    /* init the return status */
    status = 0;
    switch (cmd) {
	case ACM_IOCTL_SET_READ_CB:
	case UDI_IOCTL_SET_READ_CB:
	    port->read_done_cb = (ACM_READ_DONE_CB_T)arg;
	    break;

	case ACM_IOCTL_SET_WRITE_CB:
	case UDI_IOCTL_SET_WRITE_CB:
	    port->write_done_cb = (ACM_WRITE_DONE_CB_T)arg;
	    break;

	case ACM_IOCTL_SET_EVT_CB:
        port->event_notify_cb = (ACM_EVENT_CB_T)arg;
	    break;

	case ACM_IOCTL_SET_FREE_CB:
	    port->write_done_free_cb = (ACM_FREE_CB_T)arg;
	    break;

	case ACM_IOCTL_WRITE_ASYNC:
		if (0 == arg) {
			pr_err("gs_acm_modem_ioctl ACM_IOCTL_WRITE_ASYNC invalid param\n");
			return -EFAULT;
		}
		rw_info = (ACM_WR_ASYNC_INFO *)arg;
		port->stat_write_async_call++;
		status = gs_acm_modem_write_base(port, (struct sk_buff*)rw_info->pBuffer);
	    break;

	case ACM_IOCTL_GET_RD_BUFF:
		if (0 == arg) {
		    pr_err("gs_acm_modem_ioctl ACM_IOCTL_GET_RD_BUFF invalid param\n");
		    return -EFAULT;
		}
		port->stat_get_buf_call++;
		status = gs_acm_modem_get_read_buf(port, (ACM_WR_ASYNC_INFO*)arg);
	    break;

	case ACM_IOCTL_RETURN_BUFF:
		if (0 == arg) {
		    pr_err("gs_acm_modem_ioctl ACM_IOCTL_RETURN_BUFF invalid param\n");
		    return -EFAULT;
		}
		port->stat_ret_buf_call++;
		status = gs_acm_modem_ret_read_buf(port, (ACM_WR_ASYNC_INFO*)arg);
	    break;

	case ACM_IOCTL_RELLOC_READ_BUFF:
	    if (0 == arg) {
		    pr_err("gs_acm_modem_ioctl ACM_IOCTL_RETURN_BUFF invalid param\n");
		    return -EFAULT;
		}
	    rd_info = (ACM_READ_BUFF_INFO*)arg;
	    port->read_buf_size = (unsigned)rd_info->u32BuffSize;
	    break;

    case ACM_IOCTL_SEND_BUFF_CAN_DMA:
    case ACM_IOCTL_WRITE_DO_COPY:
		/* modem always return ok */
	    break;

    case ACM_MODEM_IOCTL_SET_MSC_READ_CB:
        port->read_sig_cb = (ACM_MODEM_MSC_READ_CB_T)arg;
	    break;

    case ACM_MODEM_IOCTL_MSC_WRITE_CMD:
        status = gs_acm_modem_write_signal(port, (MODEM_MSC_STRU*)arg);
	    break;

    case ACM_MODEM_IOCTL_SET_REL_IND_CB:
        port->rel_ind_cb = (ACM_MODEM_REL_IND_CB_T)arg;
	    break;

	default:
        status = -1;
        break;
	}

    return status;
}

static const struct file_operations gs_acm_modem_fops = {
	.llseek		= no_llseek,

	.read		=       gs_acm_modem_read,
	.write		=       gs_acm_modem_write,
	.unlocked_ioctl	=   gs_acm_modem_ioctl,
	.open		=       gs_acm_modem_open,
	.release	=       gs_acm_modem_close,
};

/*-------------------------------------------------------------------------*/

int g_modem_sg_cnt = 0;

static int gs_acm_modem_port_alloc(unsigned port_num, struct usb_cdc_line_coding *coding)
{
	struct gs_acm_modem_port	*port;
    u32 i;
    struct gs_acm_modem_sglist_info *sglist_info;

	port = kzalloc(sizeof(struct gs_acm_modem_port), GFP_KERNEL);
	if (port == NULL)
		return -ENOMEM;

    for (i = 0; i < ACM_MODEM_SG_LIST_NUM; i++) {
        sglist_info = &port->sglist_ctx.sg_list_array[i];
        sglist_info->sg_list = kzalloc(ACM_MODEM_SG_LIST_ITEM_NUM *
            sizeof(struct scatterlist), GFP_KERNEL);
            g_modem_sg_cnt++;
        if (!sglist_info->sg_list) {
            goto port_alloc_fail;
        }
        skb_queue_head_init(&sglist_info->skb_list);

        sglist_info->port = port;
		sglist_info->item_num = 0;
		sglist_info->total_size = 0;
		sglist_info->index = i;
		sglist_info->next =
            &port->sglist_ctx.sg_list_array[(i + 1)%ACM_MODEM_SG_LIST_NUM];
    }
    port->sglist_ctx.buffer_size_threshold = ACM_MODEM_SIZE_THRE;
    port->sglist_ctx.item_num_threshold = ACM_MODEM_NUM_THRE;
    port->sglist_ctx.tx_timeout = ACM_MODEM_TIMEOUT_THRE;

    gs_acm_evt_init(&gs_modem_write_evt_manage, "modem_write");
    gs_acm_evt_init(&gs_modem_read_evt_manage, "modem_read");
    gs_acm_evt_init(&gs_modem_sig_stat_evt_manage, "modem_sig_stat");

	spin_lock_init(&port->port_lock);
	init_waitqueue_head(&port->close_wait);

    INIT_DELAYED_WORK(&port->rw_work, gs_acm_modem_rw_push);

	INIT_LIST_HEAD(&port->read_pool);
	INIT_LIST_HEAD(&port->read_done_queue);
    INIT_LIST_HEAD(&port->read_queue_in_usb);
	INIT_LIST_HEAD(&port->write_pool);

    init_waitqueue_head(&port->write_wait);
    init_waitqueue_head(&port->read_wait);
    init_waitqueue_head(&port->realloc_wait);

	port->port_num = port_num;
	port->port_line_coding = *coding;

    port->read_buf_size = ACM_MODEM_DFT_RD_BUF_SIZE;
    port->read_req_num = ACM_MODEM_DFT_RD_REQ_NUM;
    port->write_req_num = ACM_MODEM_DFT_WT_REQ_NUM;

	gs_acm_modem_ports[port_num].port = port;

    /* mark the asic string for debug */
    snprintf(gs_acm_modem_ports[port_num].name_domain, 4, "mdm%d", port_num);
    snprintf(port->read_domain, 4, "%d_rd", port_num);
    snprintf(port->write_domain, 4, "%d_wt", port_num);
    snprintf(port->debug_tx_domain, 4, "dtx%d", port_num);
    snprintf(port->debug_rx_domain, 4, "drx%d", port_num);
    snprintf(port->debug_port_domain, 4, "dpt%d", port_num);

	return 0;

port_alloc_fail:

    for (i = 0; i < ACM_MODEM_SG_LIST_NUM; i++) {
        sglist_info = &port->sglist_ctx.sg_list_array[i];
        if (sglist_info->sg_list) {
            kfree(sglist_info->sg_list);
        }
    }

    if (port)
        kfree(port);
    return -ENOMEM;
}

static struct u_modem_driver *gs_acm_modem_alloc_driver(int lines)
{
	struct u_modem_driver *driver;

	driver = kzalloc(sizeof(struct u_modem_driver), GFP_KERNEL);
	if (driver) {
		kref_init(&driver->kref);
		driver->num = lines;
	}
	return driver;
}

static int gs_acm_modem_register_driver(struct u_modem_driver *driver)
{
    int error;
    dev_t dev;

    error = alloc_chrdev_region(&dev, driver->minor_start,
						driver->num, driver->name);
    if (error < 0) {
		return error;
	}
	/* coverity[uninit_use] */
	driver->major = MAJOR(dev);
	driver->minor_start = MINOR(dev);

    cdev_init(&driver->cdev, &gs_acm_modem_fops);
    driver->cdev.owner = driver->owner;

    error = cdev_add(&driver->cdev, dev, driver->num);
    if (error) {
		unregister_chrdev_region(dev, driver->num);
		return error;
	}
    driver->dev_no = dev;

    return 0;
}

static void gs_acm_modem_unregister_driver(struct u_modem_driver *driver)
{
    cdev_del(&driver->cdev);
    unregister_chrdev_region(gs_modem_driver->dev_no, gs_modem_driver->num);
    return;
}

static struct device *gs_acm_modem_register_device(struct u_modem_driver *driver,
                        unsigned index, struct device *device)
{
	char name[ACM_MODEM_NAME_MAX];
	dev_t dev = MKDEV(driver->major, driver->minor_start) + index;

	if ((int)index >= driver->num) {
		pr_err("Attempt to register invalid tty line number (%d).\n", index);
		return ERR_PTR(-EINVAL);
	}
    snprintf(name, ACM_MODEM_NAME_MAX, "%s",
                   ACM_MODEM_GET_NAME(index));

	return device_create(tty_class, device, dev, NULL, name);
}

static void gs_acm_modem_unregister_device(struct u_modem_driver *driver,
                                           unsigned index)
{
	device_destroy(tty_class,
		MKDEV(driver->major, driver->minor_start) + index);
}

/**
 * gacm_modem_setup - initialize cdev driver for one or more gs_acm_modem_ports
 * @g: gadget to associate with these gs_acm_modem_ports
 * @count: how many gs_acm_modem_ports to support
 * Context: may sleep
 *
 * Returns negative errno or zero.
 */
int gacm_modem_setup(struct usb_gadget *g, unsigned count)
{
	unsigned			i;
	struct usb_cdc_line_coding	coding;
	int				status;

	if (count == 0 || count > ACM_MDM_COUNT)
		return -EINVAL;

	gs_modem_driver = gs_acm_modem_alloc_driver(count);
	if (!gs_modem_driver)
		return -ENOMEM;

    gs_modem_driver->owner = THIS_MODULE;
	gs_modem_driver->driver_name = ACM_MODEM_DRV_NAME;
	gs_modem_driver->name = ACM_MODEM_PREFIX;

	coding.dwDTERate = cpu_to_le32(115200);
	coding.bCharFormat = 8;
	coding.bParityType = USB_CDC_NO_PARITY;
	coding.bDataBits = USB_CDC_1_STOP_BITS;

	/* alloc and init each port */
	for (i = 0; i < count; i++) {
		mutex_init(&gs_acm_modem_ports[i].open_close_lock);
		status = gs_acm_modem_port_alloc(i, &coding);
		if (status) {
			count = i;
			goto setup_fail;
		}
	}
	gs_acm_modem_n_ports = count;

	/* register the driver ... */
	status = gs_acm_modem_register_driver(gs_modem_driver);
	if (status) {
		pr_err("%s: cannot register, err %d\n",
				__func__, status);
		goto setup_fail;
	}

	/* register devices ... */
	for (i = 0; i < count; i++) {
		struct device	*cdev;

		cdev = gs_acm_modem_register_device(gs_modem_driver, i, &g->dev);
		if (IS_ERR(cdev)){
			pr_warning("%s: no classdev for port %d, err %ld\n",
				__func__, i, PTR_ERR(cdev));
            goto setup_fail;
        }
        gs_acm_modem_ports[i].cdev = cdev;
	}

    gs_modem_driver->acm_work_queue = create_singlethread_workqueue("acm_mdm");
    if (!gs_modem_driver->acm_work_queue)
    {
        status = -ENOMEM;
        goto setup_fail;
    }

	return status;

setup_fail:
    if (gs_modem_driver->acm_work_queue)
        destroy_workqueue(gs_modem_driver->acm_work_queue);

	while (count){
	    --count; /* change for coverity */
        if (gs_acm_modem_ports[count].port)
            kfree(gs_acm_modem_ports[count].port);

        if (gs_acm_modem_ports[count].cdev)
            gs_acm_modem_unregister_device(gs_modem_driver, count);
    }

    gs_acm_modem_unregister_driver(gs_modem_driver);

    if (gs_modem_driver){
        kfree(gs_modem_driver);
	    gs_modem_driver = NULL;
    }
	return status;
}

static int gs_acm_modem_closed(struct gs_acm_modem_port *port)
{
	int cond;
    unsigned long flags;

	spin_lock_irqsave(&port->port_lock, flags);
	cond = (port->open_count == 0) && !port->openclose;
	spin_unlock_irqrestore(&port->port_lock, flags);
	return cond;
}

/**
 * gacm_modem_cleanup - remove cdev-over-USB driver and devices
 * Context: may sleep
 *
 * This is called to free all resources allocated by @gserial_setup().
 * Accordingly, it may need to wait until some open /dev/ files have
 * closed.
 *
 * The caller must have issued @gserial_disconnect() for any gs_acm_modem_ports
 * that had previously been connected, so that there is never any
 * I/O pending when it's called.
 */
void gacm_modem_cleanup(void)
{
	unsigned i,j;
	struct gs_acm_modem_port *port;
    struct gs_acm_modem_sglist_info *sglist_info;
    unsigned long flags;


	if (!gs_modem_driver) {
	    printk("gacm_modem_cleanup gs_modem_driver is NULL\n");
		return;
	}
#if 1
	/* start sysfs and /dev/ttyGS* node removal */
	for (i = 0; i < gs_acm_modem_n_ports; i++)
		gs_acm_modem_unregister_device(gs_modem_driver, i);
#endif

	for (i = 0; i < gs_acm_modem_n_ports; i++) {
		/* prevent new opens */
		mutex_lock(&gs_acm_modem_ports[i].open_close_lock);
		port = gs_acm_modem_ports[i].port;
		gs_acm_modem_ports[i].port = NULL;
		mutex_unlock(&gs_acm_modem_ports[i].open_close_lock);

		/* wait for old opens to finish */
		wait_event(port->close_wait, gs_acm_modem_closed(port));

		WARN_ON(port->port_usb != NULL);

		spin_lock_irqsave(&port->port_lock, flags);
        for (j = 0; j < ACM_MODEM_SG_LIST_NUM; j++) {
            sglist_info = &port->sglist_ctx.sg_list_array[j];

            /* if there are skb remain in list, free it */
            if (sglist_info->item_num) {
                gs_acm_modem_free_skb_list(port, &sglist_info->skb_list);
                sglist_info->item_num = 0;
                sglist_info->total_size = 0;
            }
            if (sglist_info->sg_list) {
                g_modem_sg_cnt--;
                kfree(sglist_info->sg_list);
            }
        }
        spin_unlock_irqrestore(&port->port_lock, flags);

		kfree(port);
	}
	gs_acm_modem_n_ports = 0;

#if 1
    if (gs_modem_driver->acm_work_queue) {
        destroy_workqueue(gs_modem_driver->acm_work_queue);
        gs_modem_driver->acm_work_queue = NULL;
    }

    gs_acm_modem_unregister_driver(gs_modem_driver);
#endif

    if (gs_modem_driver){
        kfree(gs_modem_driver);
	    gs_modem_driver = NULL;
    }
}

int gacm_modem_line_state(struct gserial *gser, u32 state)
{
    struct gs_acm_modem_port *port = gser->ioport;
    unsigned long flags;
    u16 line_state;

    if (!port) {
        return -ESHUTDOWN;
    }

	spin_lock_irqsave(&port->port_lock, flags);
	line_state = port->line_state;

	/* if line state is change notify the callback */
	if (line_state != (u16)(state & U_ACM_CTRL_DTR) ||
        line_state != (u16)(state & U_ACM_CTRL_RTS)) {

        port->line_state =
            (u16)((state & U_ACM_CTRL_DTR) | (state & U_ACM_CTRL_RTS));
        port->line_state_change = 1;
    }
	spin_unlock_irqrestore(&port->port_lock, flags);

    /* host may change the state in a short time, delay it, use the last state */
	if (port->line_state_change) {
        port->stat_notify_sched++;
        gs_acm_evt_push(port, &gs_modem_sig_stat_evt_manage);
	    queue_delayed_work(gs_modem_driver->acm_work_queue, &port->rw_work, 50);
	}
	return 0;
}

int gacm_modem_suspend(struct gserial *gser)
{
    struct gs_acm_modem_port *port = gser->ioport;
    unsigned long flags;

    if (!port) {
        return -ESHUTDOWN;
    }

	spin_lock_irqsave(&port->port_lock, flags);
    port->is_suspend = 1;
    port->sr_change = 1;
	spin_unlock_irqrestore(&port->port_lock, flags);

	if (port->sr_change) {
        port->stat_notify_sched++;
        gs_acm_evt_push(port, &gs_modem_sig_stat_evt_manage);
	    queue_delayed_work(gs_modem_driver->acm_work_queue, &port->rw_work, 0);
	}
	return 0;
}

int gacm_modem_resume(struct gserial *gser)
{
    struct gs_acm_modem_port *port = gser->ioport;
    unsigned long flags;

    if (!port) {
        return -ESHUTDOWN;
    }

	spin_lock_irqsave(&port->port_lock, flags);
    port->is_suspend = 0;
    gs_acm_modem_start_rx(port);
	spin_unlock_irqrestore(&port->port_lock, flags);
	return 0;
}


/**
 * gacm_modem_connect - notify TTY I/O glue that USB link is active
 * @gser: the function, set up with endpoints and descriptors
 * @port_num: which port is active
 * Context: any (usually from irq)
 *
 * This is called activate endpoints and let the TTY layer know that
 * the connection is active ... not unlike "carrier detect".  It won't
 * necessarily start I/O queues; unless the TTY is held open by any
 * task, there would be no point.  However, the endpoints will be
 * activated so the USB host can perform I/O, subject to basic USB
 * hardware flow control.
 *
 * Caller needs to have set up the endpoints and USB function in @dev
 * before calling this, as well as the appropriate (speed-specific)
 * endpoint descriptors, and also have set up the TTY driver by calling
 * @gserial_setup().
 *
 * Returns negative errno or zero.
 * On success, ep->driver_data will be overwritten.
 */
int gacm_modem_connect(struct gserial *gser, u8 port_num)
{
	struct gs_acm_modem_port	*port;
	unsigned long	flags;
	int		status;

	if (!gs_modem_driver || port_num >= gs_acm_modem_n_ports) {
        pr_emerg("gacm_modem_connect fail drv:%p, port_num:%d, n_ports:%d\n",
                 gs_modem_driver, port_num, gs_acm_modem_n_ports);
        BUG();
		return -ENXIO;
    }

	/* we "know" gserial_cleanup() hasn't been called */
	port = gs_acm_modem_ports[port_num].port;

    /* mask the not ready interrupt for usb netcard class function driver */
    gser->out->enable_xfer_in_progress = 1;

	/* activate the endpoints */
	status = usb_ep_enable(gser->in);
	if (status < 0) {
        port->stat_enable_in_fail++;
		return status;
    }

	status = usb_ep_enable(gser->out);
	if (status < 0) {
        port->stat_enable_out_fail++;
		goto fail_out;
    }

	/* then tell the tty glue that I/O can work */
	spin_lock_irqsave(&port->port_lock, flags);
    gser->in->driver_data = port;
    gser->out->driver_data = port;
	gser->ioport = (void*)port;
	port->port_usb = gser;

	/* REVISIT unclear how best to handle this state...
	 * we don't really couple it with the Linux TTY.
	 */
	gser->port_line_coding = port->port_line_coding;

    /* prepare requests */
    gs_acm_modem_prepare_io(port);

	/* if it's already open, start I/O ... and notify the serial
	 * protocol about open/close status (connect/disconnect).
	 * don't need to notify host now ...
	 */
#if ACM_MODEM_SUPPORT_NOTIFY
	if (port->open_count) {
		if (gser->connect)
			gser->connect(gser);
	} else {
		if (gser->disconnect)
			gser->disconnect(gser);
	}
#endif

    /* start read requests */
    gs_acm_modem_start_rx(port);

	spin_unlock_irqrestore(&port->port_lock, flags);
	port->in_name = (char*)gser->in->name;
	port->out_name = (char*)gser->out->name;
    port->stat_port_connect++;
    port->stat_port_is_connect = 1;
	return status;

fail_out:
	usb_ep_disable(gser->in);
	gser->in->driver_data = NULL;
	port->stat_port_is_connect = 0;
	return status;
}

/**
 * gacm_modem_disconnect - notify TTY I/O glue that USB link is inactive
 * @gser: the function, on which gserial_connect() was called
 * Context: any (usually from irq)
 *
 * This is called to deactivate endpoints and let the TTY layer know
 * that the connection went inactive ... not unlike "hangup".
 *
 * On return, the state is as if gserial_connect() had never been called;
 * there is no active USB I/O on these endpoints.
 */
void gacm_modem_disconnect(struct gserial *gser)
{
	struct gs_acm_modem_port	*port = gser->ioport;
	unsigned long	flags;

	if (!port) {
        BUG();
		return;
    }

	/* disable endpoints, aborting down any active I/O */
	usb_ep_disable(gser->out);
	usb_ep_disable(gser->in);

    spin_lock_irqsave(&port->port_lock, flags);

    port->port_usb = NULL;
	gser->ioport = NULL;
    gser->out->driver_data = NULL;
    gser->in->driver_data = NULL;

	gs_acm_modem_free_requests(gser->out, &port->read_pool, NULL, port);
	gs_acm_modem_free_requests(gser->out, &port->read_done_queue, NULL, port);
	gs_acm_modem_free_requests(gser->in, &port->write_pool, NULL, port);
	port->read_allocated = 0;
	port->write_allocated = 0;

    port->stat_port_disconnect++;
    port->stat_port_is_connect = 0;
    port->is_suspend = 0;
    port->sr_change = 0;
    spin_unlock_irqrestore(&port->port_lock, flags);
}

static void acm_modem_dump_ep_info(struct gs_acm_modem_port *port)
{
    char* find;
    unsigned ep_num;

    if (port->stat_port_is_connect) {
        pr_emerg("in ep name:\t\t <%s>\n", port->in_name);
        find = strstr(port->in_name, "ep");
        if (find) {
            /* skip "ep" */
            find += 2;
            ep_num = simple_strtoul(find, NULL, 0);
            pr_emerg("in ep num:\t\t <%d>\n", ep_num * 2 + 1);
        }
        pr_emerg("out ep name:\t\t <%s>\n", port->out_name);
        find = strstr(port->out_name, "ep");
        if (find) {
            /* skip "ep" */
            find += 2;
            ep_num = simple_strtoul(find, NULL, 0);
            pr_emerg("out ep num:\t\t <%d>\n", ep_num * 2);
        }
    }
    else {
        pr_emerg("the acm dev is not connect\n");
    }
}

/* ----- functions for debug ----- */
void acm_modem_set_timeout(int port_num, int timeout)
{
    struct gs_acm_modem_port	*port;

    if (!gs_modem_driver || port_num >= (int)gs_acm_modem_n_ports) {
        pr_emerg("gacm_dump fail drv:%p, port_num:%d, n_ports:%d\n",
                 gs_modem_driver, port_num, gs_acm_modem_n_ports);
		return;
    }

	port = gs_acm_modem_ports[port_num].port;
	port->sglist_ctx.tx_timeout = timeout;
	return;
}


void acm_modem_dump(int port_num)
{
    struct gs_acm_modem_port	*port;

    if (!gs_modem_driver || port_num >= (int)gs_acm_modem_n_ports) {
        pr_emerg("gacm_dump fail drv:%p, port_num:%d, n_ports:%d\n",
                 gs_modem_driver, port_num, gs_acm_modem_n_ports);
		return;
    }

	port = gs_acm_modem_ports[port_num].port;

    pr_emerg("=== dump stat dev ctx info ===\n");
    pr_emerg("build version:            %s\n", __VERSION__);
    pr_emerg("build date:               %s\n", __DATE__);
    pr_emerg("build time:               %s\n", __TIME__);
    pr_emerg("dev name                  %s\n", ACM_MODEM_GET_NAME(port_num));
    pr_emerg("gs_modem_drv_invalid      %d\n", gs_modem_drv_invalid);
    pr_emerg("gs_modem_port_num_err     %d\n", gs_modem_port_num_err);
    pr_emerg("open_count                %d\n", port->open_count);
    pr_emerg("openclose                 %d\n", port->openclose);
    pr_emerg("port_num                  %d\n", port->port_num);
    pr_emerg("line_state                %d\n", port->line_state);
    pr_emerg("line_state_change         %d\n", port->line_state_change);
    acm_modem_dump_ep_info(port);

    mdelay(10);
    pr_emerg("\n=== dump stat read info ===\n");
    pr_emerg("read_started              %d\n", port->read_started);
    pr_emerg("read_allocated            %d\n", port->read_allocated);
    pr_emerg("read_req_enqueued         %d\n", port->read_req_enqueued);
    pr_emerg("read_req_num              %d\n", port->read_req_num);
    pr_emerg("read_buf_size             %d\n", port->read_buf_size);
    pr_emerg("read_completed            %d\n", port->read_completed);

    pr_emerg("\n=== dump rx status info ===\n");
    pr_emerg("stat_get_buf_call         %d\n", port->stat_get_buf_call);
    pr_emerg("stat_ret_buf_call         %d\n", port->stat_ret_buf_call);
    pr_emerg("stat_rx_submit            %d\n", port->stat_rx_submit);
    pr_emerg("stat_rx_submit_fail       %d\n", port->stat_rx_submit_fail);
    pr_emerg("stat_rx_disconnect        %d\n", port->stat_rx_disconnect);
    pr_emerg("stat_rx_no_req            %d\n", port->stat_rx_no_req);
    pr_emerg("stat_rx_done              %d\n", port->stat_rx_done);
    pr_emerg("stat_rx_done_fail         %d\n", port->stat_rx_done_fail);
    pr_emerg("stat_rx_done_bytes        %d\n", port->stat_rx_done_bytes);
    pr_emerg("stat_rx_done_disconnect   %d\n", port->stat_rx_done_disconnect);
    pr_emerg("stat_rx_done_schdule      %d\n", port->stat_rx_done_schdule);
    pr_emerg("stat_rx_callback          %d\n", port->stat_rx_callback);
    pr_emerg("stat_rx_cb_not_start      %d\n", port->stat_rx_cb_not_start);
    pr_emerg("stat_alloc_skb            %d\n", port->stat_alloc_skb);
    pr_emerg("stat_alloc_skb_fail       %d\n", port->stat_alloc_skb_fail);
    pr_emerg("stat_free_skb             %d\n", port->stat_free_skb);

    mdelay(10);
    pr_emerg("\n=== dump stat write info ===\n");
    pr_emerg("write_req_num             %d\n", port->write_req_num);
    pr_emerg("write_started             %d\n", port->write_started);
    pr_emerg("write_allocated           %d\n", port->write_allocated);

    pr_emerg("\n=== dump tx status info ===\n");
    pr_emerg("stat_write_async_call     %d\n", port->stat_write_async_call);
    pr_emerg("stat_write_submit         %d\n", port->stat_write_submit);
    pr_emerg("stat_write_bytes          %d\n", port->stat_write_bytes);
    pr_emerg("stat_tx_submit            %d\n", port->stat_tx_submit);
    pr_emerg("stat_tx_submit_fail       %d\n", port->stat_tx_submit_fail);
    pr_emerg("stat_tx_submit_bytes      %d\n", port->stat_tx_submit_bytes);
    pr_emerg("stat_tx_done              %d\n", port->stat_tx_done);
    pr_emerg("stat_tx_done_fail         %d\n", port->stat_tx_done_fail);
    pr_emerg("stat_tx_done_bytes        %d\n", port->stat_tx_done_bytes);
    pr_emerg("stat_tx_no_req            %d\n", port->stat_tx_no_req);
    pr_emerg("stat_tx_inv_param         %d\n", port->stat_tx_inv_param);
    pr_emerg("stat_tx_disconnect        %d\n", port->stat_tx_disconnect);
    pr_emerg("stat_tx_fifo_full         %d\n", port->stat_tx_fifo_full);
    pr_emerg("stat_tx_suspend           %d\n", port->stat_tx_suspend);
    pr_emerg("stat_timeout_tx           %d\n", port->stat_timeout_tx);
    pr_emerg("stat_direct_tx            %d\n", port->stat_direct_tx);

    mdelay(10);
    pr_emerg("\n=== dump port status info ===\n");
    pr_emerg("stat_port_connect         %d\n", port->stat_port_connect);
    pr_emerg("stat_port_disconnect      %d\n", port->stat_port_disconnect);
    pr_emerg("stat_enable_in_fail       %d\n", port->stat_enable_in_fail);
    pr_emerg("stat_enable_out_fail      %d\n", port->stat_enable_out_fail);
    pr_emerg("stat_notify_sched         %d\n", port->stat_notify_sched);
    pr_emerg("stat_notify_on_cnt        %d\n", port->stat_notify_on_cnt);
    pr_emerg("stat_notify_off_cnt       %d\n", port->stat_notify_off_cnt);
    pr_emerg("stat_read_sig_cnt         %d\n", port->stat_read_sig_cnt);
    pr_emerg("sr_change                 %d\n", port->sr_change);
    pr_emerg("is_suspend                %d\n", port->is_suspend);

}

void acm_modem_dump_sglist(int port_num)
{
    struct gs_acm_modem_port	*port;
    int i;
    struct gs_acm_modem_sglist_info *sglist_info;

    if (!gs_modem_driver || port_num >= (int)gs_acm_modem_n_ports) {
        pr_emerg("gacm_dump fail drv:%p, port_num:%d, n_ports:%d\n",
                 gs_modem_driver, port_num, gs_acm_modem_n_ports);
		return;
    }

	port = gs_acm_modem_ports[port_num].port;

    pr_emerg("\n=== dump port sglist_ctx info ===\n");
    pr_emerg("buffer_size_threshold     %d\n", port->sglist_ctx.buffer_size_threshold);
    pr_emerg("item_num_threshold        %d\n", port->sglist_ctx.item_num_threshold);
    pr_emerg("submit_pos                %d\n", port->sglist_ctx.submit_pos);
    pr_emerg("done_pos                  %d\n", port->sglist_ctx.done_pos);
    pr_emerg("tx_timeout                %d\n", port->sglist_ctx.tx_timeout);
    pr_emerg("tx_timeout_on             %d\n", port->sglist_ctx.tx_timeout_on);
    pr_emerg("\n");

	for (i = 0; i < ACM_MODEM_SG_LIST_NUM; i++) {
        sglist_info = &port->sglist_ctx.sg_list_array[i];
        pr_emerg("=== dump sglist info[%d], addr:<0x%x> ===\n", i, (u32)sglist_info);
        pr_emerg("sg_list                   0x%x\n", (u32)sglist_info->sg_list);
        pr_emerg("skb_list.qlen             %d\n", sglist_info->skb_list.qlen);
        pr_emerg("skb_list.prev             0x%x\n", (u32)sglist_info->skb_list.prev);
        pr_emerg("skb_list.next             0x%x\n", (u32)sglist_info->skb_list.next);
        pr_emerg("item_num                  %d\n", sglist_info->item_num);
        pr_emerg("total_size                %d\n", sglist_info->total_size);
        pr_emerg("index                     %d\n", sglist_info->index);
        pr_emerg("next                      0x%x\n", (u32)sglist_info->next);
        pr_emerg("\n");
    }
}

void acm_modem_dump_msg(int port_num)
{
    struct gs_acm_modem_port	*port;

    if (!gs_modem_driver || port_num >= (int)gs_acm_modem_n_ports) {
        pr_emerg("gacm_dump fail drv:%p, port_num:%d, n_ports:%d\n",
                 gs_modem_driver, port_num, gs_acm_modem_n_ports);
		return;
    }

	port = gs_acm_modem_ports[port_num].port;

    pr_emerg("\n=== dump port flow msg info ===\n");
    pr_emerg("ps_recv_flow              %d\n", port->ps_recv_flow);
    pr_emerg("OP_Dtr                    %d\n", port->cur_flow_msg.OP_Dtr);
    pr_emerg("OP_Dsr                    %d\n", port->cur_flow_msg.OP_Dsr);
    pr_emerg("OP_Cts                    %d\n", port->cur_flow_msg.OP_Cts);
    pr_emerg("OP_Rts                    %d\n", port->cur_flow_msg.OP_Rts);
    pr_emerg("OP_Ri                     %d\n", port->cur_flow_msg.OP_Ri);
    pr_emerg("OP_Dcd                    %d\n", port->cur_flow_msg.OP_Dcd);
    pr_emerg("OP_Fc                     %d\n", port->cur_flow_msg.OP_Fc);
    pr_emerg("OP_Brk                    %d\n", port->cur_flow_msg.OP_Brk);
    pr_emerg("OP_Spare                  %d\n", port->cur_flow_msg.OP_Spare);
    pr_emerg("ucDtr                     %d\n", port->cur_flow_msg.ucDtr);
    pr_emerg("ucDsr                     %d\n", port->cur_flow_msg.ucDsr);
    pr_emerg("ucCts                     %d\n", port->cur_flow_msg.ucCts);
    pr_emerg("ucRts                     %d\n", port->cur_flow_msg.ucRts);
    pr_emerg("ucRi                      %d\n", port->cur_flow_msg.ucRi);
    pr_emerg("ucDcd                     %d\n", port->cur_flow_msg.ucDcd);
    pr_emerg("ucFc                      %d\n", port->cur_flow_msg.ucFc);
    pr_emerg("ucBrk                     %d\n", port->cur_flow_msg.ucBrk);
    pr_emerg("ucBrkLen                  %d\n", port->cur_flow_msg.ucBrkLen);
}

void acm_modem_dump_evt(void)
{
    gs_acm_evt_dump_info(&gs_modem_write_evt_manage);
    gs_acm_evt_dump_info(&gs_modem_read_evt_manage);
    gs_acm_evt_dump_info(&gs_modem_sig_stat_evt_manage);
}
/*lint -restore*/
