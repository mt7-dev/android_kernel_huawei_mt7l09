/*
 * u_cdev.c - utilities for USB gadget "serial port"/TTY support
 *
 */

/* #define VERBOSE_DEBUG */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <linux/usb/composite.h>


#include "u_usim.h"


/*
 * The port structure holds info for each port, one for each minor number
 * (and thus for each /dev/ node).
 */
struct gs_usim_port {
	spinlock_t port_lock;               /* guard port_* access */

	struct gusim *port_usb;
    struct workqueue_struct *usim_work_queue;
    wait_queue_head_t proc_wait;
    int proc_blocked;

    struct usb_request *req_in;
    struct usb_request *req_out;
    char *in_name;
    char *out_name;
    BSP_CCID_REGFUNC ccid_usim_cb;
    int ccid_started;

    struct delayed_work	rw_work;

    int read_completed;
    int read_started;

    int write_started;
    bool sim_pre_status;
    unsigned port_num;

    unsigned stat_tx_submit_fail;
    unsigned stat_tx_submit;
    unsigned stat_tx_done;
    unsigned stat_tx_no_req;
    unsigned stat_tx_buf_size;

    unsigned stat_proc_wait_wakeup;
    unsigned stat_proc_wait_timeout;
    unsigned stat_proc_wait_error;
    unsigned stat_proc_no_cb;
    unsigned stat_proc_apdu_err;

    unsigned stat_rx_submit_fail;
    unsigned stat_rx_submit;
    unsigned stat_rx_done;
    unsigned stat_rx_no_req;
    unsigned stat_rx_cb_not_start;
    unsigned stat_rx_cb_sched;
    unsigned stat_read_cb;
    unsigned stat_handle_msg;
    unsigned stat_rx_buf_size;

    unsigned stat_notify_cb;
    unsigned stat_notify_cb_sched;
    unsigned stat_port_connect;
    unsigned stat_port_disconnect;
    unsigned stat_port_is_connect;
    unsigned stat_enable_in_fail;
    unsigned stat_enable_out_fail;
};


static struct usim_port_manager {
	struct gs_usim_port	*port;
} gs_usim_ports[USIM_MAX_DEV_NUM];


static unsigned	gs_usim_n_ports;
static unsigned gs_stat_drv_invalid;
static unsigned gs_stat_port_num_err;
static unsigned char gs_pcsc_clk_tbl[] = U_USIM_CKL_TBL_VALUE;
static unsigned char gs_pcsc_baud_tbl[] = U_USIM_BAND_TBL_VALUE;
static unsigned char gs_pcsc_fake_atr[] = U_USIM_FAKE_ATR;

/* pcsc buffer is too large, so we alloc in first time and don't free it */
static void* gs_out_req_buffer = NULL;

static void gs_usim_write_complete(struct usb_ep *ep, struct usb_request *req);
static void gs_usim_read_complete(struct usb_ep *ep, struct usb_request *req);

/*-------------------------------------------------------------------------*/

/*
 * gs_usim_start_tx
 *
 * This function finds available write requests, calls
 * usb_ep_queue to send the data.
 *
 */
static int gs_usim_start_tx(struct gs_usim_port *port,
                                void* buf, u32 len)
{
    int status;
	struct usb_ep		*out = port->port_usb->out;
    struct usb_request	*req =  port->req_in;

    /* if the req not complete, we can't re-submit it */
    if (port->write_started) {
        port->stat_tx_no_req++;
        return -EAGAIN;
    }

	/*
	 * buf is global buffer in f_ccid
	 * so we use the buffer ptr for the req
	 */
	req->buf = buf;
    req->length = len;
	spin_unlock(&port->port_lock);
	status = usb_ep_queue(out, req, GFP_ATOMIC);
    spin_lock(&port->port_lock);
	if (status) {
	    /* usim tx req buffer is belong to usr */
	    req->buf = NULL;
	    req->length = 0;
        port->write_started--;
        port->stat_tx_submit_fail++;
		return status;
	}
    port->write_started++;
    port->stat_tx_submit++;


	return status;
}

/*
 * Context: caller owns port_lock, and port_usb is set
 */
static void gs_usim_start_rx(struct gs_usim_port *port)
{
    int status;
	struct usb_ep		*out = port->port_usb->out;
    struct usb_request	*req =  port->req_out;

    /* if the req not complete, we can't re-submit it */
    if (port->read_started) {
        port->stat_rx_no_req++;
        return;
    }

	/* drop lock while we call out; the controller driver
	 * may need to call us back (e.g. for disconnect)
	 */
	spin_unlock(&port->port_lock);
	status = usb_ep_queue(out, req, GFP_ATOMIC);
	spin_lock(&port->port_lock);

	if (status) {
        port->read_started--;
        port->stat_rx_submit_fail++;
		return;
	}
    port->read_started++;
    port->stat_rx_submit++;

	return;
}

static void gs_usim_notify_cb(struct gs_usim_port *port)
{
    //struct usb_request	*req = port->req_out;
    unsigned long flags;
    struct gusim	*gusim;

    for (;;) {
        spin_lock_irqsave(&port->port_lock, flags);
        port->stat_notify_cb++;
        if (port->ccid_started <= 0) {
            spin_unlock_irqrestore(&port->port_lock, flags);
            return;
        }
        port->ccid_started--;
        spin_unlock_irqrestore(&port->port_lock, flags);

        /* notify the slot change */
        gusim = port->port_usb;
        if (gusim->connect)
            gusim->connect(gusim);

    }
}

static void gs_usim_read_cb(struct gs_usim_port *port)
{
    struct usb_request	*req = port->req_out;
    unsigned long flags;
    struct gusim	*gusim;

    for (;;) {
        spin_lock_irqsave(&port->port_lock, flags);
        port->stat_read_cb++;
        if (port->read_completed <= 0) {
            spin_unlock_irqrestore(&port->port_lock, flags);
            return;
        }
        port->read_completed--;

        spin_unlock_irqrestore(&port->port_lock, flags);

        /* handle the ccid msg */
        gusim = port->port_usb;
    	if (gusim && gusim->handle_msg) {
            port->stat_handle_msg++;
    		gusim->handle_msg(gusim, req->buf, req->actual);
        }

        /* submit the next read req */
        spin_lock_irqsave(&port->port_lock, flags);
        if (port->port_usb) {
		    gs_usim_start_rx(port);
        }
        else {
            port->stat_rx_cb_not_start++;
        }
        spin_unlock_irqrestore(&port->port_lock, flags);
    }
}

/*
 * rw workqueue takes data out of the RX queue and hands it up to the TTY
 * layer until it refuses to take any more data (or is throttled back).
 * Then it issues reads for any further data.
 */
static void gs_usim_rw_push(struct work_struct *work)
{
	struct gs_usim_port *port;
    int i;

    /* read callback */
    for (i = 0; i < gs_usim_n_ports; i++) {
        port = gs_usim_ports[i].port;
        gs_usim_read_cb(port);
    }

    /* notify callback ... */
    for (i = 0; i < gs_usim_n_ports; i++) {
        port = gs_usim_ports[i].port;
        gs_usim_notify_cb(port);
    }

    return;
}

static void gs_usim_read_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct gs_usim_port	*port = ep->driver_data;
    int status;

	/* Queue all received data until the tty layer is ready for it. */
	spin_lock(&port->port_lock);

    port->read_started--;
    port->stat_rx_done++;

    status = req->status;
    /* if rcv ok, schedule the workqueue to handle the msg */
    if (!status) {
        port->read_completed++;
        port->stat_rx_cb_sched++;
        /* handle the ccid msg */
        queue_delayed_work(port->usim_work_queue, &port->rw_work, 0);
    }

	spin_unlock(&port->port_lock);

    /* if status err, but not shutdonw, submit the next rx */
    if (status && -ESHUTDOWN != status) {
        gs_usim_start_rx(port);
    }
}

static void gs_usim_write_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct gs_usim_port	*port = ep->driver_data;

	spin_lock(&port->port_lock);
	/* usim tx req buffer is belong to usr */
	req->buf = NULL;
	req->length = 0;
    port->write_started--;
    port->stat_tx_done++;
	spin_unlock(&port->port_lock);
}


struct usb_request *
gs_usim_alloc_req(struct usb_ep *ep, unsigned len, gfp_t kmalloc_flags)
{
	struct usb_request *req;

	req = usb_ep_alloc_request(ep, kmalloc_flags);

	if (req != NULL) {
		req->length = len;
        /* if len is 0, alloc the empty req */
        if (0 == len) {
            req->buf = NULL;
            req->dma = (dma_addr_t)NULL;
        }
        else {
    		req->buf = kmalloc(len, kmalloc_flags);
    		if (req->buf == NULL) {
    			usb_ep_free_request(ep, req);
    			return NULL;
    	    }
        }
	}

	return req;
}

void gs_usim_free_req(struct usb_ep *ep, struct usb_request *req)
{
    if (req->buf) {
	    kfree(req->buf);
	}
	usb_ep_free_request(ep, req);
}

/*
 * Context: holding port_lock;
 */
static int gs_ccid_prepare_io(struct gs_usim_port *port)
{
	struct usb_ep *ep_in = port->port_usb->in;
    struct usb_ep *ep_out = port->port_usb->out;

    if (!port->req_out) {
        if (!gs_out_req_buffer) {
            gs_out_req_buffer =
                kmalloc(USIM_MSG_BUFFER_SIZE + USIM_MSG_HEADER_SIZE, GFP_ATOMIC);
            if (!gs_out_req_buffer)
                return -ENOMEM;
        }

        port->req_out = gs_usim_alloc_req(ep_out, 0, GFP_ATOMIC);
        if (!port->req_out) {
            kfree(gs_out_req_buffer);
            gs_out_req_buffer = NULL;
		    return -ENOMEM;
		}
		port->req_out->buf = gs_out_req_buffer;
		port->req_out->length = USIM_MSG_BUFFER_SIZE + USIM_MSG_HEADER_SIZE;
        port->req_out->complete = gs_usim_read_complete;
        port->stat_rx_buf_size = USIM_MSG_BUFFER_SIZE + USIM_MSG_HEADER_SIZE;
    }

    if (!port->req_in) {
        port->req_in = gs_usim_alloc_req(ep_in, 0, GFP_ATOMIC);
        if (!port->req_in)
		    return -ENOMEM;

        port->req_in->complete = gs_usim_write_complete;
    }

    return 0;
}

/*-------------------------------------------------------------------------*/

static int gs_usim_port_alloc(unsigned port_num)
{
	struct gs_usim_port	*port;

	port = kzalloc(sizeof(struct gs_usim_port), GFP_KERNEL);
	if (port == NULL)
		return -ENOMEM;

	spin_lock_init(&port->port_lock);

    INIT_DELAYED_WORK(&port->rw_work, gs_usim_rw_push);
    init_waitqueue_head(&port->proc_wait);

	port->port_num = port_num;

	gs_usim_ports[port_num].port = port;

    port->usim_work_queue = create_singlethread_workqueue("usim_push");
    if (!port->usim_work_queue)
        return -ENOMEM;

	return 0;
}

/**
 * gusim_setup - initialize usim driver for one or more usim_port
 * @g: gadget to associate with these gs_usim_ports
 * @count: how many ports to support
 * Context: may sleep
 *
 * Returns negative errno or zero.
 */
int gusim_setup(struct usb_gadget *g, unsigned count)
{
	unsigned			i;
	int				status = 0;

	if (count == 0 || count > USIM_MAX_DEV_NUM)
		return -EINVAL;

	/* alloc and init each port */
	for (i = 0; i < count; i++) {
		status = gs_usim_port_alloc(i);
		if (status) {
			count = i;
			goto setup_fail;
		}
	}
	gs_usim_n_ports = count;

	return status;

setup_fail:

	while (count){
	    --count; /* change for coverity */
        if (gs_usim_ports[count].port) {
            if (gs_usim_ports[count].port->usim_work_queue)
                destroy_workqueue(gs_usim_ports[count].port->usim_work_queue);
            kfree(gs_usim_ports[count].port);
        }

    }

	return status;
}

void gusim_cleanup(void)
{
	unsigned i;
	struct gs_usim_port *port;

	for (i = 0; i < gs_usim_n_ports; i++) {

		port = gs_usim_ports[i].port;
		if (port) {
    		if (port->usim_work_queue) {
                destroy_workqueue(port->usim_work_queue);
                port->usim_work_queue = NULL;
            }
    		gs_usim_ports[i].port = NULL;

    		kfree(port);
		}
	}
	gs_usim_n_ports = 0;
}


int gusim_connect(struct gusim *gusim, u8 port_num)
{
	struct gs_usim_port	*port;
	unsigned long	flags;
	int		status;

	if (port_num >= gs_usim_n_ports) {
        pr_emerg("gusim_connect fail port_num:%d, n_ports:%d\n",
                 port_num, gs_usim_n_ports);
        BUG();
		return -ENXIO;
    }

	port = gs_usim_ports[port_num].port;

    /* mask the not ready interrupt for usb netcard class function driver */
    gusim->out->enable_xfer_in_progress = 1;

	/* activate the endpoints */
	status = usb_ep_enable(gusim->in);
	if (status < 0) {
        port->stat_enable_in_fail++;
		return status;
    }

	status = usb_ep_enable(gusim->out);
	if (status < 0) {
        port->stat_enable_out_fail++;
		goto fail_out;
    }

	/* then tell the tty glue that I/O can work */
	spin_lock_irqsave(&port->port_lock, flags);
    gusim->in->driver_data = port;
    gusim->out->driver_data = port;
	gusim->ioport = (void*)port;
	port->port_usb = gusim;

    /* prepare requests */
    gs_ccid_prepare_io(port);

    /* start read requests */
    gs_usim_start_rx(port);

    if (NULL != port->ccid_usim_cb.pFuncApdu)
    {
        port->ccid_started++;
        port->stat_notify_cb_sched++;
        queue_delayed_work(port->usim_work_queue, &port->rw_work, 0);
    }

	spin_unlock_irqrestore(&port->port_lock, flags);
	port->in_name = (char*)gusim->in->name;
	port->out_name = (char*)gusim->out->name;
    port->stat_port_connect++;
    port->stat_port_is_connect = 1;
    port->sim_pre_status = true;
	return status;

fail_out:
	usb_ep_disable(gusim->in);
	gusim->in->driver_data = NULL;
	port->stat_port_is_connect = 0;
	return status;
}

void gusim_disconnect(struct gusim *gusim)
{
	struct gs_usim_port	*port = gusim->ioport;
	unsigned long flags;

	if (!port) {
        BUG();
		return;
    }

	/* disable endpoints, aborting down any active I/O */
	usb_ep_disable(gusim->out);
	usb_ep_disable(gusim->in);

    spin_lock_irqsave(&port->port_lock, flags);

    port->port_usb = NULL;
	gusim->ioport = NULL;
    gusim->out->driver_data = NULL;
    gusim->in->driver_data = NULL;

    /* out req buffer is save in gs_out_req_buffer, don't need to free */
    port->req_out->buf = NULL;
    port->req_out->length = 0;
    gs_usim_free_req(gusim->out, port->req_out);
    port->req_out = NULL;
    gs_usim_free_req(gusim->in, port->req_in);
    port->req_in = NULL;

	spin_unlock_irqrestore(&port->port_lock, flags);
    port->stat_port_disconnect++;
    port->stat_port_is_connect = 0;
    port->sim_pre_status = false;
}


int gusim_get_slot_status(struct gusim *gusim, slot_status_t *status_array)
{
    struct gs_usim_port	*port = gusim->ioport;
	bool sim_status = false;

    if(port->ccid_usim_cb.pFuncGetCardStatus)
    {
        sim_status = ((port->ccid_usim_cb.pFuncGetCardStatus())? false : true);
    }
    else
    {
        sim_status = false;
    }

    /* it just get the slot 0 status */
    status_array[0].status_changed =
        ((sim_status == port->sim_pre_status)? false : true);
	status_array[0].current_state = sim_status;
    if(status_array[0].current_state)
    {
        status_array[0].status_changed = true;
    }

	port->sim_pre_status = sim_status;

    return 0;
}

int gusim_abort_slot(struct gusim *gusim, u8 slot, u8 sequence)
{
    return 0;
}

int gusim_get_data_rates(struct gusim *gusim, void* buffer)
{
    struct gs_usim_port	*port = gusim->ioport;

    memcpy(buffer, gs_pcsc_baud_tbl, sizeof(gs_pcsc_baud_tbl));

    if (NULL != port->ccid_usim_cb.pFuncApdu) {

        /* notify slot change */
        if (gusim->connect)
            gusim->connect(gusim);
    }

    return 0;
}

int gusim_get_clock_freqs(struct gusim *gusim, void* buffer)
{
    memcpy(buffer, gs_pcsc_clk_tbl, sizeof(gs_pcsc_clk_tbl));
    return 0;
}

int gusim_return_msg(struct gusim *gusim, void* buffer, u32 size)
{
    struct gs_usim_port	*port = gusim->ioport;
    int status;
    unsigned long flags;

    spin_lock_irqsave(&port->port_lock, flags);
    port->stat_tx_buf_size = size;
    status = gs_usim_start_tx(port, buffer, size);
    spin_unlock_irqrestore(&port->port_lock, flags);

	return status;
}

int gusim_msg_proc(struct gusim *gusim)
{
    struct gs_usim_port	*port = gusim->ioport;
    int ret = 1;
    pFunAPDUProcess pFuncApdu = port->ccid_usim_cb.pFuncApdu;

    if(NULL == pFuncApdu) {
        /* stat counter */
        port->stat_proc_no_cb++;
    }
    else {
        ret = (*pFuncApdu)(gusim->usim_cmd, gusim->apdu_data, gusim->apdu_len);
        if(ret) {
            port->stat_proc_apdu_err++;
        }
        else
        {
            /* block the work queue in f_ccid */
            port->proc_blocked = 1;
            ret = wait_event_interruptible_timeout(port->proc_wait, (port->proc_blocked == 0), PCSC_CMD_TIMEOUT);
            /* wake up */
            if(ret > 0) {
    	        ret = 0;
                port->stat_proc_wait_wakeup++;
            }
            /* timeout */
            else if (!ret) {
                port->stat_proc_wait_timeout++;
            }
            /* error */
            else {
                port->stat_proc_wait_error++;
            }
        }
    }

    return ret;
}

int gusim_ctrl_cmd(u32 port_num, u32 cmd_type,
                   u32 status, u8 *buf, u32 length)
{
    struct gs_usim_port	*port;

    if (port_num >= gs_usim_n_ports) {
        pr_emerg("gusim_ctrl_cmd fail port_num:%d, n_ports:%d\n",
                 port_num, gs_usim_n_ports);
		return -ENXIO;
    }

	port = gs_usim_ports[port_num].port;

    if(cmd_type != port->port_usb->usim_cmd) {
        pr_emerg("%s:USIMM response Wrong! cmd_type 0x%X should be 0x%X. status=0x%X\n",
                 __FUNCTION__, cmd_type, port->port_usb->usim_cmd, status);

        return -EINVAL;
    }

    switch(cmd_type) {

        case PCSC_USIM_POWER_ON:
        case PCSC_USIM_POWER_OFF:
        case PCSC_USIM_SIM_QUIRY:
        case PCSC_USIM_APDU_CMD:
        case PCSC_USIM_GET_ATR:
        case PCSC_USIM_GET_PARA:
        case PCSC_USIM_GET_DATA_RATE:
        case PCSC_USIM_GET_CLK_FREQ:
        {
            port->port_usb->usim_cmd_status = status;
            if(buf && length) {
                if(PCSC_USIM_POWER_ON == cmd_type) {
                    length = sizeof(gs_pcsc_fake_atr);
                    port->port_usb->apdu_len = length;
                    memcpy(&(port->port_usb->apdu_data[0]), gs_pcsc_fake_atr, length);
                }
                else {
                    port->port_usb->apdu_len = length;
                    memcpy(&(port->port_usb->apdu_data[0]), buf, length);
                }
            }
            else {
                port->port_usb->apdu_len = 0;
            }

            port->proc_blocked = 0;
            wake_up_interruptible(&port->proc_wait);

            break;
        }
        default: {
            pr_emerg("%s: unknown PCSC_USIM_CMD cmd_type=0x%X, status=0x%X \n",
                     __FUNCTION__, cmd_type, status);
            return -EINVAL;
        }
    }

    return 0;
}

void gusim_reg_cb(u32 port_num, pFunAPDUProcess pFun1, GetCardStatus pFun2)
{
    struct gs_usim_port	*port;
    unsigned long flags;

    if (port_num >= gs_usim_n_ports) {
        pr_emerg("gusim_ctrl_cmd fail port_num:%d, n_ports:%d\n",
                 port_num, gs_usim_n_ports);
		return;
    }

	port = gs_usim_ports[port_num].port;

    if (!port)
        return;

    port->ccid_usim_cb.pFuncApdu           = pFun1;
    port->ccid_usim_cb.pFuncGetCardStatus  = pFun2;

    spin_lock_irqsave(&port->port_lock, flags);
    if(port->stat_port_is_connect)
    {
        port->ccid_started++;
        port->stat_notify_cb_sched++;
        queue_delayed_work(port->usim_work_queue, &port->rw_work, 0);
    }
    spin_unlock_irqrestore(&port->port_lock, flags);
    return;
}

static void gusim_dump_ep_info(struct gs_usim_port *port)
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
        pr_emerg("the usim dev is not connect\n");
    }
}

void usim_dump(int port_num)
{
    struct gs_usim_port *port;

    if (port_num >= gs_usim_n_ports) {
        pr_emerg("usim_cdev_dump fail port_num:%d, n_ports:%d\n",
                 port_num, gs_usim_n_ports);
		return;
    }

	port = gs_usim_ports[port_num].port;

    pr_emerg("=== dump stat info ===\n");
    pr_emerg("build version:            %s\n", __VERSION__);
    pr_emerg("build date:               %s\n", __DATE__);
    pr_emerg("build time:               %s\n", __TIME__);
    pr_emerg("port_num                  %d\n", port->port_num);
    pr_emerg("sim_pre_status            %d\n", port->sim_pre_status);
    pr_emerg("pFuncGetCardStatus        0x%x\n", (u32)port->ccid_usim_cb.pFuncGetCardStatus);
    pr_emerg("pFuncApdu                 0x%x\n", (u32)port->ccid_usim_cb.pFuncApdu);
    if (port->port_usb) {
        pr_emerg("usim_cmd                  0x%x\n", port->port_usb->usim_cmd);
        pr_emerg("usim_cmd_status           0x%x\n", port->port_usb->usim_cmd_status);
        pr_emerg("apdu_len                  0x%x\n", port->port_usb->apdu_len);
    }
    gusim_dump_ep_info(port);

    mdelay(10);
    pr_emerg("\n=== dump r|w info ===\n");
    pr_emerg("write_started             %d\n", port->write_started);
    pr_emerg("stat_tx_submit_fail       %d\n", port->stat_tx_submit_fail);
    pr_emerg("stat_tx_submit            %d\n", port->stat_tx_submit);
    pr_emerg("stat_tx_done              %d\n", port->stat_tx_done);
    pr_emerg("stat_tx_buf_size          %d\n\n", port->stat_tx_buf_size);

    pr_emerg("stat_proc_wait_wakeup     %d\n", port->stat_proc_wait_wakeup);
    pr_emerg("stat_proc_wait_timeout    %d\n", port->stat_proc_wait_timeout);
    pr_emerg("stat_proc_wait_error      %d\n", port->stat_proc_wait_error);
    pr_emerg("stat_proc_no_cb           %d\n", port->stat_proc_no_cb);
    pr_emerg("stat_proc_apdu_err        %d\n\n", port->stat_proc_apdu_err);

    pr_emerg("read_completed            %d\n", port->read_completed);
    pr_emerg("read_started              %d\n", port->read_started);
    pr_emerg("stat_rx_submit_fail       %d\n", port->stat_rx_submit_fail);
    pr_emerg("stat_rx_submit            %d\n", port->stat_rx_submit);
    pr_emerg("stat_rx_done              %d\n", port->stat_rx_done);
    pr_emerg("stat_rx_no_req            %d\n", port->stat_rx_no_req);
    pr_emerg("stat_rx_cb_not_start      %d\n", port->stat_rx_cb_not_start);
    pr_emerg("stat_rx_cb_sched          %d\n", port->stat_rx_cb_sched);
    pr_emerg("stat_read_cb              %d\n", port->stat_read_cb);
    pr_emerg("stat_handle_msg           %d\n", port->stat_handle_msg);
    pr_emerg("stat_rx_buf_size          %d\n\n", port->stat_rx_buf_size);

    pr_emerg("stat_notify_cb            %d\n", port->stat_notify_cb);
    pr_emerg("stat_notify_cb_sched      %d\n", port->stat_notify_cb_sched);
    pr_emerg("stat_port_connect         %d\n", port->stat_port_connect);
    pr_emerg("stat_port_disconnect      %d\n", port->stat_port_disconnect);
    pr_emerg("stat_port_is_connect      %d\n", port->stat_port_is_connect);
    pr_emerg("stat_enable_in_fail       %d\n", port->stat_enable_in_fail);
    pr_emerg("stat_enable_out_fail      %d\n\n", port->stat_enable_out_fail);

    mdelay(10);
    if (port->port_usb) {
        port->port_usb->dump_fd(port->port_usb);
    }
}


