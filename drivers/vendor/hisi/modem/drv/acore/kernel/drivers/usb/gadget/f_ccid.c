/*
 * f_ccid.c -- USB CCID function driver
 *
 * This software is distributed under the terms of the GNU General
 * Public License ("GPL") as published by the Free Software Foundation,
 * either version 2 of that License or (at your option) any later version.
 */


#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/device.h>

#include "u_usim.h"
#include "gadget_chips.h"


#define USB_CCID_MSG_TYPE_LEN                   1
#define USB_CCID_REQ_SIZE_RESERV                4
#define USB_CCID_MSG_END_OF_HEADER_POS          7
#define USB_CCID_MSG_HEADER_SIZE                10
#define USB_CCID_RESERVED                       0

#define USB_CCID_INTERFACE_CLASS                0x0B
#define USB_CCID_INTERFACE_SUBCLASS             0x00
#define USB_CCID_INTERFACE_PROTOCOL             0x00

#define USB_CCID_GET_COMMAND_STATUS_BITS        0x03
#define USB_CCID_COMMAND_BITS_TIME_EXT          0x02

#define AUTOMATIC_PARAMETER_CONFIG              0x00000002
#define AUTOMATIC_ACTIVATION_ICC_ON_INSERTING   0x00000004
#define AUTOMATIC_ICC_VOLTAGE_SELECTION         0x00000008
#define AUTOMATIC_ICC_CLOCK_FREQ                0x00000010
#define AUTOMATIC_BAUD_RATE                     0x00000020
#define AUTOMATIC_PARAMETER_NEGOTIATION         0x00000040
#define AUTOMATIC_PPS                           0x00000080
#define ICC_STOP_CLOCK                          0x00000100
#define NAD_VALUE_SUPPORT                       0x00000200
#define AUTOMATIC_IFSD                          0x00000400
#define SHORT_APDU_EXCHANGE                     0x00020000

#define USB_CCID_SECURE_MSG_PARAM_0                    0x0000
#define USB_CCID_SECURE_MSG_PARAM_1                    0x0001
#define USB_CCID_SECURE_MSG_PARAM_2                    0x0002
#define USB_CCID_SECURE_MSG_PARAM_3                    0x0003
#define USB_CCID_SECURE_MSG_PARAM_10                   0x0010

#define USB_CCID_SECURE_VERIFICATION_OP                0x0000
#define USB_CCID_SECURE_MODIFICATION_OP                0x0001

#define USB_CCID_INTR_SLOT_CHANGE_MSG                  0x50
#define USB_CCID_INTR_ERROR_MSG                        0x51

#define USB_CCID_CTRL_ABORT_MSG                        0x01
#define USB_CCID_CTRL_GET_CLK_FRQ_MSG                  0x02
#define USB_CCID_CTRL_GET_DATA_RATES_MSG               0x03

#define USB_CCID_VOLTAGE_AUTO                          0x00
#define USB_CCID_VOLTAGE_5_0                           0x01
#define USB_CCID_VOLTAGE_3_0                           0x02
#define USB_CCID_VOLTAGE_1_8                           0x03

#define USB_CCID_HOST_TO_RDR_ICC_POWER_ON              0x62
#define USB_CCID_HOST_TO_RDR_ICC_POWER_OFF             0x63
#define USB_CCID_HOST_TO_RDR_GET_SLOT_STATUS           0x65
#define USB_CCID_HOST_TO_RDR_XFER_BLOCK                0x6f
#define USB_CCID_HOST_TO_RDR_GET_PARAMETERS            0x6c
#define USB_CCID_HOST_TO_RDR_RESET_PARAMETERS          0x6d
#define USB_CCID_HOST_TO_RDR_SET_PARAMETERS            0x61
#define USB_CCID_HOST_TO_RDR_ESCAPE                    0x6b
#define USB_CCID_HOST_TO_RDR_ICC_CLOCK                 0x6e
#define USB_CCID_HOST_TO_RDR_T0_APDU                   0x6a
#define USB_CCID_HOST_TO_RDR_SECURE                    0x69
#define USB_CCID_HOST_TO_RDR_MECHANICAL                0x71
#define USB_CCID_HOST_TO_RDR_ABORT                     0x72
#define USB_CCID_HOST_TO_RDR_SET_D_RATE_AND_CLK_FREQ   0x73

#define USB_CCID_RDR_TO_HOST_DATA_BLOCK                0x80
#define USB_CCID_RDR_TO_HOST_SLOT_STATUS               0x81
#define USB_CCID_RDR_TO_HOST_PARAMETERS                0x82
#define USB_CCID_RDR_TO_HOST_ESCAPE                    0x83
#define USB_CCID_RDR_TO_HOST_D_RATE_AND_CLK_FREQ       0x84

#define CCID_LOG2_NOTIFY_INTERVAL		                5	/* 1 << 5 == 32 msec */
#define CCID_NOTIFY_MAXPACKET		                    10	/* notification + 2 bytes */


/* CCID extra descriptor: */
struct ccid_descriptor_descriptor {
    u8      length;                      /* 0x36 */
    u8      descriptor_type;             /* 0x21 */
    u16     release_number;
    u8      max_slot_index;
    u8      voltage_support;
    u32     protocols;
    u32     default_clock;
    u32     maximum_clock;
    u8      num_clock_supported;
    u32     data_rate;
    u32     max_data_rate;
    u8      num_data_rate_supported;
    u32     max_ifsd;
    u32     synch_protocols;
    u32     mechanical;
    u32     features;
    u32     max_ccid_message_length;
    u8      class_get_response;
    u8      class_envelope;
    u16     lcd_layout;
    u8      pin_support;
    u8      max_ccid_busy_slots;
} __attribute__ ((packed));

/*
 *      Host to RDR messages
 *      --------------------
 */
typedef struct {
    u8  message_type;
    u32 length;
    u8  slot;
    u8  sequence;
}ccid_host_to_rdr_common_header_t;

typedef struct {
    ccid_host_to_rdr_common_header_t header;
    u8      power_select;
} ccid_host_to_rdr_icc_power_on_t;

typedef struct {
    ccid_host_to_rdr_common_header_t header;
    u8      bwi;        /* Block timeout multiplier */
    u16     level_parameter;
    u8      *data;      /* The data byte array */
} ccid_host_to_rdr_xfer_block_t;

typedef struct {
    u8      f_index_d_index;
    u8      tccks;
    u8      guard_time;
    u8      waiting_integer;
    u8      clock_stop;
    /* ifsc_size and nad_value are relevant only when protocol is T=1 */
    u8      ifsc_size;
    u8      nad_value;
} ccid_protocol_data_t;

typedef struct {
    ccid_host_to_rdr_common_header_t header;
    u8      protocol_num;
    ccid_protocol_data_t protocol_data;
} ccid_host_to_rdr_set_params_t;

typedef struct {
    ccid_host_to_rdr_common_header_t header;
    u8 *data; /* The byte array of size header.length */
} ccid_host_to_rdr_escape_t;

typedef struct {
    ccid_host_to_rdr_common_header_t header;
    u8      clock_command;
} ccid_host_to_rdr_icc_clk_t;

typedef struct {
    ccid_host_to_rdr_common_header_t header;
    u8      changes;
    u8      class_get_response;
    u8      class_envelope;
} ccid_host_to_rdr_t0_apdu_t;

typedef struct {
    u8      time_out;
    u8      format_string;
    u8      pin_block_string;
    u8      pin_length_format;
} ccid_pin_common_data_t;

typedef struct {
    ccid_host_to_rdr_common_header_t header;
    u8      bwi;        /* Block timeout multiplier */
    u16     level_parameter;
    u8      pin_operation;
    /* Pointer to pin_verification_t
     * - pin_modification_t structures or a continuation of a previous secure
     * msg depends on the value of level_parameter and pin_operation fields */
    void    *pin_data;
} ccid_host_to_rdr_secure_t;

/* PIN verification */
typedef struct {
    ccid_pin_common_data_t common_data;
    u16     pin_max_extra_digit;
    u8      entry_validation_condition;
    u8      number_message;
    u16     lang_id;
    u8      msg_index;
    u8      teo_prologue[3];
    u32     pin_apdu_len;
    u8      *pin_apdu;
} ccid_pin_verification_t;

/* PIN modification */
typedef struct {
    ccid_pin_common_data_t common_data;
    u8      insertion_offset_old;
    u8      insertion_offset_new;
    u16     pin_max_extra_digit;
    u8      confirm_pin;
    u8      entry_validation_condition;
    u8      number_message;
    u16     lang_id;
    u8      msg_index1;
    u8      msg_index2;
    u8      msg_index3;
    u8      teo_prologue[3];
    u32     pin_apdu_len;
    u8      *pin_apdu;
} ccid_pin_modification_t;

typedef struct {
    ccid_host_to_rdr_common_header_t header;
    u8      function;
} ccid_host_to_rdr_mechanical_t;

typedef struct {
    ccid_host_to_rdr_common_header_t header;
    u32     clock_frequency;
    u32     data_rate;
}  ccid_host_to_rdr_set_d_rate_and_clk_freq_t;


/*
 *      RDR to Host messages
 *      --------------------
 */
typedef struct {
    u8      message_type;
    u32     length;
    u8      slot;
    u8      sequence;
    u8      status;
    u8      error;
} ccid_rdr_to_host_common_header_t;

typedef struct {
    ccid_rdr_to_host_common_header_t header;
    u8      chain_parameter;
    u8      *data; /* The byte array */
} ccid_rdr_to_host_data_block_t;

typedef struct {
    ccid_rdr_to_host_common_header_t header;
    u8      clock_status;
} ccid_rdr_to_host_slot_status_t;

typedef struct {
    ccid_rdr_to_host_common_header_t header;
    u8      protocol_num;
    ccid_protocol_data_t protocol_data;
} ccid_rdr_to_host_params_t;

typedef struct {
    ccid_rdr_to_host_common_header_t header;
    u8      *data; /* The byte array of size header.length */
} ccid_rdr_to_host_escape_t;

typedef struct {
    ccid_rdr_to_host_common_header_t header;
    u32     clock_frequency;
    u32     data_rate;
} ccid_rdr_to_host_d_rate_and_clk_freq_t;


struct f_ccid {
    struct gusim            port;
    u8                      ctrl_id, data_id;
    u8                      port_num;
    u8                      pending;
    u8                      num_of_slots;
    u32                     num_data_rates;
    u32                     num_clock_frqs;
    spinlock_t              lock;

    struct usb_ep           *notify;
    struct usb_request      *notify_req;
    slot_status_t           *status_array;
    ccid_host_to_rdr_common_header_t *cur_header;
    ccid_host_to_rdr_common_header_t **msg_array;
    u8*                     ret_msg_buf;
    u32                     ret_msg_size;

    struct workqueue_struct *ccid_work_queue;
    struct delayed_work rw_work;

    u32                     stat_parse_msg_err;
    u32                     stat_handle_msg_sched;
    u32                     stat_proc_msg;
    u32                     stat_return_msg;
    u32                     stat_send_msg_err;
    u32                     stat_alloc_msg;
    u32                     stat_alloc_msg_fail;
    u32                     stat_free_msg;
    u32                     stat_notify_no_req;
    u32                     stat_notify_submit;
    u32                     stat_notify_submit_err;
    u32                     stat_notify_complete;
};

/* pack/unpack macros */
#define CHECK_POS(pos, total, ret, exit) \
do {\
    if (pos > total) {\
        pr_emerg("PACK/UNPACK MSG Fail line:%d, func:%s\n", \
            __LINE__, __FUNCTION__);\
        ret = -EINVAL;\
        goto exit;\
    }\
} while(0)

/* pack ... */
#define PACK_BYTE(buffer, pos, val, total, ret, exit) \
do {\
    (buffer)[(pos)++] = (val);\
    CHECK_POS(pos, total, ret, exit);\
} while(0)

#define PACK_WORD(buffer, pos, val, total, ret, exit) \
do { \
    put_unaligned_le16((val), (char*)(buffer) + (pos)); \
    (pos) += 2; \
    CHECK_POS(pos, total, ret, exit);\
} while (0)

#define PACK_DWORD(buffer, pos, val, total, ret, exit) \
do { \
    put_unaligned_le32((val), (char*)(buffer) + (pos)); \
    (pos) += 4; \
    CHECK_POS(pos, total, ret, exit);\
} while (0)

#define PACK_BYTE_ARRAY(buffer, pos, from, size, total, ret, exit) \
do { \
    if ((size)) \
    { \
        memcpy((buffer) + (pos), (void *)(from), (size)); \
        (pos) += (size); \
        CHECK_POS(pos, total, ret, exit);\
    } \
} while(0)

/* unpack ... */
#define UNPACK_BYTE(buf, pos, to, total, ret, exit) \
do {\
    (to) = ((char*)(buf))[(pos)++];\
    CHECK_POS(pos, total, ret, exit); \
} while(0)


#define UNPACK_WORD(buf, pos, to, total, ret, exit) \
do { \
    (to) = get_unaligned_le16((char*)(buf) + (pos)); \
    (pos) += 2; \
    CHECK_POS(pos, total, ret, exit); \
} while(0)

#define UNPACK_DWORD(buf, pos, to, total, ret, exit) \
do { \
    (to) = get_unaligned_le32((char*)(buf) + (pos)); \
    (pos) += 4; \
    CHECK_POS(pos, total, ret, exit); \
} while(0)

#define UNPACK_BYTE_ARRAY(buffer, pos, to, size, total, ret, exit) \
do { \
    if ((size)) \
    { \
        memcpy((to), (void *)((char*)(buffer) + (pos)), (size)); \
        (pos) += (size); \
        CHECK_POS(pos, total, ret, exit); \
    } \
} while(0)


static inline struct f_ccid *func_to_ccid(struct usb_function *f)
{
    return container_of(f, struct f_ccid, port.func);
}

static inline struct f_ccid *port_to_ccid(struct gusim *p)
{
    return container_of(p, struct f_ccid, port);
}
static void ccid_dump(struct gusim *port);
static void ccid_connect(struct gusim *port);
static void ccid_disconnect(struct gusim *port);


/*-------------------------------------------------------------------------*/

/* notification endpoint uses smallish and infrequent fixed-size messages */
static struct usb_interface_descriptor ccid_interface_desc = {
    .bLength =              USB_DT_INTERFACE_SIZE,
    .bDescriptorType =      USB_DT_INTERFACE,
    /* .bInterfaceNumber = DYNAMIC */
    .bNumEndpoints =        3,
    .bInterfaceClass =      USB_CCID_INTERFACE_CLASS,
    .bInterfaceSubClass =   USB_CCID_INTERFACE_SUBCLASS,
    .bInterfaceProtocol =   USB_CCID_INTERFACE_PROTOCOL,
    /* .iInterface = DYNAMIC */
};

static struct ccid_descriptor_descriptor ccid_extra_if_desc = {
    .length =               0x36,
    .descriptor_type =      0x21,
    .release_number =       cpu_to_le16(0x0100),
    .max_slot_index =       0,
    .voltage_support =      0x07,
    .protocols =            cpu_to_le32(0x00000003),
    .default_clock =        cpu_to_le32(0x000012c0),
    .maximum_clock =        cpu_to_le32(0x00001f40),
    .num_clock_supported =  0x04,
    .data_rate =            cpu_to_le32(0x00002a00),
    .max_data_rate =        cpu_to_le32(0x00064ce7),
    .num_data_rate_supported = 0x6a,
    .max_ifsd =             cpu_to_le32(0x000000FE),
    .synch_protocols =      cpu_to_le32(0x00000007),
    .mechanical =           0,
    .features =             cpu_to_le32(AUTOMATIC_PARAMETER_CONFIG | \
                                        AUTOMATIC_ICC_CLOCK_FREQ | \
                                        AUTOMATIC_BAUD_RATE | AUTOMATIC_PPS | \
                                        ICC_STOP_CLOCK | NAD_VALUE_SUPPORT | \
                                        AUTOMATIC_IFSD | SHORT_APDU_EXCHANGE),

    .max_ccid_message_length =  cpu_to_le32(0x0000010f),
    .class_get_response =   0xff,
    .class_envelope =       0xff,
    .lcd_layout =           cpu_to_le16(0),
    .pin_support =          0,
    .max_ccid_busy_slots =  1,
};

/* full speed support: */

static struct usb_endpoint_descriptor ccid_fs_notify_desc = {
    .bLength =          USB_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DT_ENDPOINT,
    .bEndpointAddress = USB_DIR_IN,
    .bmAttributes =     USB_ENDPOINT_XFER_INT,
    .wMaxPacketSize =   cpu_to_le16(CCID_NOTIFY_MAXPACKET),
    .bInterval =        1 << CCID_LOG2_NOTIFY_INTERVAL,
};

static struct usb_endpoint_descriptor ccid_fs_in_desc = {
    .bLength =          USB_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DT_ENDPOINT,
    .bEndpointAddress = USB_DIR_IN,
    .bmAttributes =     USB_ENDPOINT_XFER_BULK,
};

static struct usb_endpoint_descriptor ccid_fs_out_desc = {
    .bLength =          USB_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DT_ENDPOINT,
    .bEndpointAddress = USB_DIR_OUT,
    .bmAttributes =     USB_ENDPOINT_XFER_BULK,
};

static struct usb_descriptor_header *ccid_fs_function[] = {
    (struct usb_descriptor_header *) &ccid_interface_desc,
    (struct usb_descriptor_header *) &ccid_extra_if_desc,
    (struct usb_descriptor_header *) &ccid_fs_out_desc,
    (struct usb_descriptor_header *) &ccid_fs_in_desc,
    (struct usb_descriptor_header *) &ccid_fs_notify_desc,
    NULL,
};

/* high speed support: */

static struct usb_endpoint_descriptor ccid_hs_notify_desc = {
    .bLength =          USB_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DT_ENDPOINT,
    .bEndpointAddress = USB_DIR_IN,
    .bmAttributes =     USB_ENDPOINT_XFER_INT,
    .wMaxPacketSize =   cpu_to_le16(CCID_NOTIFY_MAXPACKET),
    .bInterval =        1 << CCID_LOG2_NOTIFY_INTERVAL,
};

static struct usb_endpoint_descriptor ccid_hs_in_desc = {
    .bLength =          USB_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DT_ENDPOINT,
    .bmAttributes =     USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize =   cpu_to_le16(512),
};

static struct usb_endpoint_descriptor ccid_hs_out_desc = {
    .bLength =          USB_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DT_ENDPOINT,
    .bmAttributes =     USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize =   cpu_to_le16(512),
};

static struct usb_descriptor_header *ccid_hs_function[] = {
    (struct usb_descriptor_header *) &ccid_interface_desc,
    (struct usb_descriptor_header *) &ccid_extra_if_desc,
    (struct usb_descriptor_header *) &ccid_hs_out_desc,
    (struct usb_descriptor_header *) &ccid_hs_in_desc,
    (struct usb_descriptor_header *) &ccid_hs_notify_desc,
    NULL,
};


static struct usb_endpoint_descriptor ccid_ss_in_desc = {
    .bLength =          USB_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DT_ENDPOINT,
    .bmAttributes =     USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize =   cpu_to_le16(1024),
};

static struct usb_endpoint_descriptor ccid_ss_out_desc = {
    .bLength =          USB_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DT_ENDPOINT,
    .bmAttributes =     USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize =   cpu_to_le16(1024),
};

static struct usb_ss_ep_comp_descriptor ccid_ss_bulk_comp_desc = {
    .bLength =              sizeof ccid_ss_bulk_comp_desc,
    .bDescriptorType =      USB_DT_SS_ENDPOINT_COMP,
};

static struct usb_descriptor_header *ccid_ss_function[] = {
    (struct usb_descriptor_header *) &ccid_interface_desc,
    (struct usb_descriptor_header *) &ccid_extra_if_desc,
    (struct usb_descriptor_header *) &ccid_ss_out_desc,
    (struct usb_descriptor_header *) &ccid_ss_bulk_comp_desc,
    (struct usb_descriptor_header *) &ccid_ss_in_desc,
    (struct usb_descriptor_header *) &ccid_ss_bulk_comp_desc,
    (struct usb_descriptor_header *) &ccid_hs_notify_desc,
    (struct usb_descriptor_header *) &ccid_ss_bulk_comp_desc,
    NULL,
};


/* string descriptors: */


/* static strings, in UTF-8 */
static struct usb_string ccid_string_defs[] = {
    [0].s = "Huawei PCSC",
    {  /* ZEROES END LIST */ },
};

static struct usb_gadget_strings ccid_string_table = {
    .language =     0x0409, /* en-us */
    .strings =      ccid_string_defs,
};

static struct usb_gadget_strings *ccid_strings[] = {
    &ccid_string_table,
    NULL,
};


/*-------------------------------------------------------------------------*/
static int ccid_setup(struct usb_function *f, const struct usb_ctrlrequest *ctrl)
{
    struct f_ccid *ccid = func_to_ccid(f);
    struct usb_composite_dev *cdev = f->config->cdev;
    struct usb_request  *req = cdev->req;
    int         value = -EOPNOTSUPP;
    u16         w_index = le16_to_cpu(ctrl->wIndex);
    u16         w_value = le16_to_cpu(ctrl->wValue);
    u16         w_length = le16_to_cpu(ctrl->wLength);
    int         ret;
    void        *buffer;

    switch ((ctrl->bRequestType << 8) | ctrl->bRequest) {

    /* CCID_CTRL_ABORT_MSG ... abort slot */
    case ((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
            | USB_CCID_CTRL_ABORT_MSG:
        if (w_index != ccid->ctrl_id)
            goto invalid;

        value = 0;
        ret = gusim_abort_slot(&ccid->port, (w_value & 0xFF),
                ((w_value >> 8) & 0xFF));
        if (ret) {
            ERROR(cdev, "ccid abort slot on port%d, err %d\n",
                    ccid->port_num, ret);
        }
        break;

    /* CCID_CTRL_GET_CLK_FRQ_MSG ...  */
    case ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
            | USB_CCID_CTRL_GET_CLK_FRQ_MSG:
    /* CCID_CTRL_GET_DATA_RATES_MSG ...  */
    case ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
            | USB_CCID_CTRL_GET_DATA_RATES_MSG:
        if (w_index != ccid->ctrl_id)
            goto invalid;

        value = (ctrl->bRequest == USB_CCID_CTRL_GET_DATA_RATES_MSG ?
            ccid->num_data_rates : ccid->num_clock_frqs) * sizeof(u32);

        buffer =  req->buf;
        if (ctrl->bRequest == USB_CCID_CTRL_GET_DATA_RATES_MSG)
            ret = gusim_get_data_rates(&ccid->port, buffer);/* [false alarm]:fortify disable */
        else
            ret = gusim_get_clock_freqs(&ccid->port, buffer);/* [false alarm]:fortify disable */

        break;

    default:
invalid:
        USB_DBG_GADGET(&cdev->gadget->dev, "invalid control req%02x.%02x v%04x i%04x l%d\n",
            ctrl->bRequestType, ctrl->bRequest,
            w_value, w_index, w_length);
    }

    /* respond with data transfer or status phase? */
    if (value >= 0) {
        DBG(cdev, "ccid port%d req%02x.%02x v%04x i%04x l%d\n",
            ccid->port_num, ctrl->bRequestType, ctrl->bRequest,
            w_value, w_index, w_length);
        req->zero = 0;
        req->length = value;

        value = usb_ep_queue(cdev->gadget->ep0, req, GFP_ATOMIC);
        if (value < 0)
            ERROR(cdev, "ccid response on port%d, err %d\n",
                    ccid->port_num, value);
    }

    /* device either stalls (value < 0) or reports success */
    return value;
}

static int ccid_set_alt(struct usb_function *f, unsigned intf, unsigned alt)
{
    struct f_ccid   *ccid = func_to_ccid(f);
    struct usb_composite_dev *cdev = f->config->cdev;
    bool is_setting = 0;

    /* we know alt == 0, so this is an activation or a reset */

    if (intf == ccid->data_id) {
        is_setting = 1;
        if (ccid->notify) {
    		if (ccid->notify->driver_data) {
    			usb_ep_disable(ccid->notify);
    		} else {
    			if (config_ep_by_speed(cdev->gadget, f, ccid->notify))
    				return -EINVAL;
    		}
    		(void)usb_ep_enable(ccid->notify);
    		ccid->notify->driver_data = ccid;
		}

        if (ccid->port.in->driver_data) {
            DBG(cdev, "reset ccid %d\n", ccid->port_num);
            gusim_disconnect(&ccid->port);
        }
        if (!ccid->port.in->desc || !ccid->port.out->desc) {
            DBG(cdev, "activate ccid %d\n", ccid->port_num);
            if (config_ep_by_speed(cdev->gadget, f,
                           ccid->port.in) ||
                config_ep_by_speed(cdev->gadget, f,
                           ccid->port.out)) {
                ccid->port.in->desc = NULL;
                ccid->port.out->desc = NULL;
                return -EINVAL;
            }
        }
        gusim_connect(&ccid->port, ccid->port_num);
        bsp_usb_set_enum_stat(ccid->data_id, 1);
    }

    if (!is_setting)
        return -EINVAL;

    return 0;
}

static void ccid_disable(struct usb_function *f)
{
    struct f_ccid   *ccid = func_to_ccid(f);
    //struct usb_composite_dev *cdev = f->config->cdev;

    gusim_disconnect(&ccid->port);
    usb_ep_disable(ccid->notify);
    ccid->notify->driver_data = NULL;
    bsp_usb_set_enum_stat(ccid->data_id, 0);
}

/*-------------------------------------------------------------------------*/

static int ccid_notify(struct f_ccid *ccid)
{
    struct usb_ep           *ep = ccid->notify;
    struct usb_request      *req;
    const unsigned          len = 1 + (ccid->num_of_slots>>2) +
                                    ((ccid->num_of_slots & 0x3) ? 1 : 0);
    void                    *buf;
    int                     status;
    u16                     slot_status = 0;
    u32                     i;

    /* get status for all slot */
    if (gusim_get_slot_status(&ccid->port, ccid->status_array)) {
        return -ENOENT;
    }

    req = ccid->notify_req;
    ccid->notify_req = NULL;
    ccid->pending = false;

    /* prepare req members */
    req->length = len;
    buf = req->buf;

    /* convert to u16 */
    for (i = 0; i < ccid->num_of_slots; i++)
    {
        /* current slot status */
        if (ccid->status_array[i].current_state)
            slot_status |= 1 << (2 * i);

        /* changed status */
        if (ccid->status_array[i].status_changed)
            slot_status |= 1 << (2 * i + 1);
    }

    /* write status msg to req buffer */
    *(u8 *)buf = USB_CCID_INTR_SLOT_CHANGE_MSG;
    put_unaligned_le16(slot_status, buf);

    /* ep_queue() can complete immediately if it fills the fifo... */
    spin_unlock(&ccid->lock);
    status = usb_ep_queue(ep, req, GFP_ATOMIC);
    spin_lock(&ccid->lock);

    if (status < 0) {
        ccid->notify_req = req;
        ccid->stat_notify_submit_err++;
    }
    else {
        ccid->stat_notify_submit++;
    }

    return status;
}

static int ccid_notify_slot_change(struct f_ccid *ccid)
{
    //struct usb_composite_dev *cdev = ccid->port.func.config->cdev;
    int status;

    spin_lock(&ccid->lock);
    if (ccid->notify_req) {
        status = ccid_notify(ccid);
    } else {
        ccid->pending = true;
        ccid->stat_notify_no_req++;
        status = 0;
    }
    spin_unlock(&ccid->lock);
    return status;
}

static void ccid_notify_complete(struct usb_ep *ep, struct usb_request *req)
{
    struct f_ccid   *ccid = req->context;
    u8  doit = false;

    spin_lock(&ccid->lock);
    ccid->stat_notify_complete++;
    if (req->status != -ESHUTDOWN)
        doit = ccid->pending;
    ccid->notify_req = req;
    spin_unlock(&ccid->lock);

    if (doit)
        ccid_notify_slot_change(ccid);
}

/* connect == notify slot change */
static void ccid_connect(struct gusim *port)
{
    struct f_ccid   *ccid = port_to_ccid(port);

    ccid_notify_slot_change(ccid);
}

static void ccid_disconnect(struct gusim *port)
{
    return;
}


/*
 * parse the ccid msg (be care, we are in atomic contex)
 */
static ccid_host_to_rdr_common_header_t *
ccid_alloc_msg(struct f_ccid *ccid, u8 type)
{
    u32 length = 0;
    void* buffer;

    switch (type) {

    case USB_CCID_HOST_TO_RDR_ICC_POWER_ON:
        length = sizeof(ccid_host_to_rdr_icc_power_on_t);
        break;

    case USB_CCID_HOST_TO_RDR_ICC_POWER_OFF:
    case USB_CCID_HOST_TO_RDR_GET_SLOT_STATUS:
    case USB_CCID_HOST_TO_RDR_GET_PARAMETERS:
    case USB_CCID_HOST_TO_RDR_RESET_PARAMETERS:
    case USB_CCID_HOST_TO_RDR_ABORT:
        length = sizeof(ccid_host_to_rdr_common_header_t);
        break;

    case USB_CCID_HOST_TO_RDR_XFER_BLOCK:
        length = sizeof(ccid_host_to_rdr_xfer_block_t);
        break;

    case USB_CCID_HOST_TO_RDR_SET_PARAMETERS:
        length = sizeof(ccid_host_to_rdr_set_params_t);
        break;

    case USB_CCID_HOST_TO_RDR_ESCAPE:
        length = sizeof(ccid_host_to_rdr_escape_t);
        break;

    case USB_CCID_HOST_TO_RDR_ICC_CLOCK:
        length = sizeof(ccid_host_to_rdr_icc_clk_t);
        break;

    case USB_CCID_HOST_TO_RDR_T0_APDU:
        length = sizeof(ccid_host_to_rdr_t0_apdu_t);
        break;

    case USB_CCID_HOST_TO_RDR_SECURE:
        length = sizeof(ccid_host_to_rdr_secure_t);
        break;

    case USB_CCID_HOST_TO_RDR_MECHANICAL:
        length = sizeof(ccid_host_to_rdr_mechanical_t);
        break;

    case USB_CCID_HOST_TO_RDR_SET_D_RATE_AND_CLK_FREQ:
        length = sizeof(ccid_host_to_rdr_set_d_rate_and_clk_freq_t);
        break;

    default:
        pr_emerg("ccid_alloc_msg Unknown Msg:0x%x!\n", type);
        return NULL;
    }

    buffer = kzalloc(length, GFP_ATOMIC);
    if (buffer)
        ccid->stat_alloc_msg++;
    else
        ccid->stat_alloc_msg_fail++;

    return (ccid_host_to_rdr_common_header_t *)buffer;
}

static void ccid_free_msg(struct f_ccid *ccid,
                          ccid_host_to_rdr_common_header_t *header)
{

    switch (header->message_type) {

    case USB_CCID_HOST_TO_RDR_ICC_POWER_ON:
    case USB_CCID_HOST_TO_RDR_ICC_POWER_OFF:
    case USB_CCID_HOST_TO_RDR_GET_SLOT_STATUS:
    case USB_CCID_HOST_TO_RDR_GET_PARAMETERS:
    case USB_CCID_HOST_TO_RDR_RESET_PARAMETERS:
    case USB_CCID_HOST_TO_RDR_ABORT:
    case USB_CCID_HOST_TO_RDR_ICC_CLOCK:
    case USB_CCID_HOST_TO_RDR_SET_PARAMETERS:
    case USB_CCID_HOST_TO_RDR_T0_APDU:
    case USB_CCID_HOST_TO_RDR_MECHANICAL:
    case USB_CCID_HOST_TO_RDR_SET_D_RATE_AND_CLK_FREQ:
        break;

    case USB_CCID_HOST_TO_RDR_XFER_BLOCK:
        {
            ccid_host_to_rdr_xfer_block_t *msg =
                (ccid_host_to_rdr_xfer_block_t *)header;

            if (msg->data)
                kfree(msg->data);
            break;
        }

    case USB_CCID_HOST_TO_RDR_ESCAPE:
        {
            ccid_host_to_rdr_escape_t *msg =
                (ccid_host_to_rdr_escape_t *)header;

            if (msg->data)
                kfree(msg->data);
            break;
        }

    case USB_CCID_HOST_TO_RDR_SECURE:
        {
            ccid_host_to_rdr_secure_t *msg =
                (ccid_host_to_rdr_secure_t *)header;

            switch (msg->level_parameter)
            {
            case USB_CCID_SECURE_MSG_PARAM_2:
            case USB_CCID_SECURE_MSG_PARAM_3:
                if (msg->pin_data) {
                    kfree(msg->pin_data);
                    msg->pin_data = NULL;
                }
                break;

            case USB_CCID_SECURE_MSG_PARAM_0:
            case USB_CCID_SECURE_MSG_PARAM_1:
                {
                    if (msg->pin_operation ==
                        USB_CCID_SECURE_VERIFICATION_OP) {
                        ccid_pin_verification_t *pin_ver =
                            (ccid_pin_verification_t *)msg->pin_data;

                        if (pin_ver->pin_apdu)
                            kfree(pin_ver->pin_apdu);

                        kfree(pin_ver);
                        msg->pin_data = NULL;
                    }

                    else if (msg->pin_operation ==
                             USB_CCID_SECURE_MODIFICATION_OP) {
                        ccid_pin_modification_t *pin_mod =
                            (ccid_pin_modification_t *)msg->pin_data;

                        if (pin_mod->pin_apdu)
                            kfree(pin_mod->pin_apdu);

                        kfree(pin_mod);
                        msg->pin_data = NULL;
                    }
                }
                break;

            default:
                break;
            }
        }
    }
    ccid->stat_free_msg++;
    kfree(header);
}

static int ccid_get_xfer_block_msg(
            ccid_host_to_rdr_common_header_t *header, u8 *buffer, u32 size)
{
    u8 pos = USB_CCID_MSG_END_OF_HEADER_POS;
    int ret = 0;
    ccid_host_to_rdr_xfer_block_t *msg =
        (ccid_host_to_rdr_xfer_block_t *)header;/* [false alarm]:fortify disable */

    UNPACK_BYTE(buffer, pos, msg->bwi, size, ret, block_msg_exit);
    UNPACK_WORD(buffer, pos, msg->level_parameter, size, ret, block_msg_exit);

    msg->data = kzalloc(header->length, GFP_ATOMIC);
    if (!msg->data) {
        pr_emerg("%s: failed to allocate data buffer\n", __FUNCTION__);
        ret = -ENOMEM;
        goto block_msg_exit;
    }

    UNPACK_BYTE_ARRAY(buffer, pos, msg->data,
        header->length, size, ret, block_msg_exit);

block_msg_exit:
    if (ret && header) {
        kfree(msg->data);
        msg->data = NULL;
    }
    return ret;
}

static int ccid_set_params_msg(ccid_host_to_rdr_common_header_t *header,
        u8 *buffer, u32 size)
{
    u8 pos = USB_CCID_MSG_END_OF_HEADER_POS;
    int ret = 0;
    ccid_host_to_rdr_set_params_t *msg =
        (ccid_host_to_rdr_set_params_t *)header;

    UNPACK_BYTE(buffer, pos, msg->protocol_num, size, ret, set_params_msg_exit);
    pos += 2; /* reserved word*/
    CHECK_POS(pos, size, ret, set_params_msg_exit);
    UNPACK_BYTE(buffer, pos, msg->protocol_data.f_index_d_index,
        size, ret, set_params_msg_exit);
    UNPACK_BYTE(buffer, pos, msg->protocol_data.tccks,
        size, ret, set_params_msg_exit);
    UNPACK_BYTE(buffer, pos, msg->protocol_data.guard_time,
        size, ret, set_params_msg_exit);
    UNPACK_BYTE(buffer, pos, msg->protocol_data.waiting_integer,
        size, ret, set_params_msg_exit);
    UNPACK_BYTE(buffer, pos, msg->protocol_data.clock_stop,
        size, ret, set_params_msg_exit);

    if (msg->protocol_num) {
        UNPACK_BYTE(buffer, pos, msg->protocol_data.ifsc_size,
            size, ret, set_params_msg_exit);
        UNPACK_BYTE(buffer, pos, msg->protocol_data.nad_value,
            size, ret, set_params_msg_exit);
    }

set_params_msg_exit:
    return ret;
}

static int ccid_get_t0_apdu_msg(ccid_host_to_rdr_common_header_t *header,
            u8 *buffer, u32 size)
{
    u8 pos = USB_CCID_MSG_END_OF_HEADER_POS;
    int ret = 0;
    ccid_host_to_rdr_t0_apdu_t *msg =
        (ccid_host_to_rdr_t0_apdu_t *)header;

    UNPACK_BYTE(buffer, pos, msg->changes, size, ret, get_t0_msg_exit);
    UNPACK_BYTE(buffer, pos, msg->class_get_response, size, ret, get_t0_msg_exit);
    UNPACK_BYTE(buffer, pos, msg->class_envelope, size, ret, get_t0_msg_exit);

get_t0_msg_exit:
    return ret;
}

static int ccid_get_secure_msg(ccid_host_to_rdr_common_header_t *header,
            u8 *buffer, u32 size)
{
    int ret = 0;
    u8 pos = USB_CCID_MSG_END_OF_HEADER_POS;
    ccid_host_to_rdr_secure_t *msg = (ccid_host_to_rdr_secure_t *)header;
    u8 **apdu_data = NULL;
    u32 *apdu_data_len;

    UNPACK_BYTE(buffer, pos, msg->bwi, size, ret, get_sec_msg_exit);
    UNPACK_WORD(buffer, pos, msg->level_parameter, size, ret, get_sec_msg_exit);

    switch (msg->level_parameter) {
    case USB_CCID_SECURE_MSG_PARAM_10:
        goto get_sec_msg_exit;

    case USB_CCID_SECURE_MSG_PARAM_2:
    case USB_CCID_SECURE_MSG_PARAM_3:

        msg->pin_data = kzalloc(header->length, GFP_ATOMIC);
        if (!msg->pin_data) {
            pr_emerg("%s: failed to allocate pin data buffer\n", __FUNCTION__);
            ret = -ENOMEM;
            goto get_sec_msg_exit;
        }

        UNPACK_BYTE_ARRAY(buffer, pos, msg->pin_data, header->length,
            size, ret, get_sec_msg_exit);
        goto get_sec_msg_exit;

    case USB_CCID_SECURE_MSG_PARAM_0:
    case USB_CCID_SECURE_MSG_PARAM_1:
        break;

    default:
        pr_emerg("%s: Unknown level param!\n", __FUNCTION__);
        goto get_sec_msg_exit;

    }

    UNPACK_BYTE(buffer, pos, msg->pin_operation, size, ret, get_sec_msg_exit);

    if (msg->pin_operation == USB_CCID_SECURE_VERIFICATION_OP ||
        msg->pin_operation == USB_CCID_SECURE_MODIFICATION_OP) {
        ccid_pin_common_data_t *pin_data;

        /* following code has bugs, disable it */
        if (msg->pin_operation == USB_CCID_SECURE_MODIFICATION_OP)
            pin_data = kzalloc(sizeof(ccid_pin_verification_t), GFP_ATOMIC);
        else
            pin_data = kzalloc(sizeof(ccid_pin_modification_t), GFP_ATOMIC);

        if (!pin_data) {
            pr_emerg("%s: failed to allocate pin structure\n", __FUNCTION__);
            ret = -ENOMEM;
            goto get_sec_msg_exit;
        }

        /* free the old one and replace a new one */
        if (msg->pin_data) {
            kfree(msg->pin_data);
        }
        msg->pin_data = pin_data;

        UNPACK_BYTE(buffer, pos, pin_data->time_out,
            size, ret, get_sec_msg_exit);
        UNPACK_BYTE(buffer, pos, pin_data->format_string,
            size, ret, get_sec_msg_exit);
        UNPACK_BYTE(buffer, pos, pin_data->pin_block_string,
            size, ret, get_sec_msg_exit);
        UNPACK_BYTE(buffer, pos, pin_data->pin_length_format,
            size, ret, get_sec_msg_exit);

        if (msg->pin_operation == USB_CCID_SECURE_MODIFICATION_OP) {
            ccid_pin_modification_t *pin_mod =
                (ccid_pin_modification_t *)msg->pin_data;

            UNPACK_BYTE(buffer, pos, pin_mod->insertion_offset_old,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_mod->insertion_offset_new,
                size, ret, get_sec_msg_exit);
            UNPACK_WORD(buffer, pos, pin_mod->pin_max_extra_digit,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_mod->confirm_pin,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_mod->entry_validation_condition,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_mod->number_message,
                size, ret, get_sec_msg_exit);
            UNPACK_WORD(buffer, pos, pin_mod->lang_id,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_mod->msg_index1,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_mod->msg_index2,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_mod->msg_index3,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_mod->teo_prologue[0],
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_mod->teo_prologue[1],
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_mod->teo_prologue[2],
                size, ret, get_sec_msg_exit);

            apdu_data = &pin_mod->pin_apdu;
            apdu_data_len = &pin_mod->pin_apdu_len;
        }
        else {
            ccid_pin_verification_t *pin_ver =
                (ccid_pin_verification_t *)msg->pin_data;

            UNPACK_WORD(buffer, pos, pin_ver->pin_max_extra_digit,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_ver->entry_validation_condition,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_ver->number_message,
                size, ret, get_sec_msg_exit);
            UNPACK_WORD(buffer, pos, pin_ver->lang_id,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_ver->msg_index,
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_ver->teo_prologue[0],
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_ver->teo_prologue[1],
                size, ret, get_sec_msg_exit);
            UNPACK_BYTE(buffer, pos, pin_ver->teo_prologue[2],
                size, ret, get_sec_msg_exit);

            apdu_data = &pin_ver->pin_apdu;
            apdu_data_len = &pin_ver->pin_apdu_len;
        }

        *apdu_data_len = (header->length + USB_CCID_MSG_HEADER_SIZE) - pos;

        if (!(*apdu_data_len)) {
            pr_emerg("%s: fail, apdu_data_len is 0\n", __FUNCTION__);
            ret = -EINVAL;
            goto get_sec_msg_exit;
        }

        *apdu_data = kzalloc(*apdu_data_len, GFP_ATOMIC);
        if (!*apdu_data) {
            pr_emerg("%s: failed to allocate apdu data buffer\n", __FUNCTION__);
            ret = -ENOMEM;
            goto get_sec_msg_exit;
        }

        UNPACK_BYTE_ARRAY(buffer, pos, *apdu_data, *apdu_data_len,
            size, ret, get_sec_msg_exit);
    }

get_sec_msg_exit:
    return ret;
}

static ccid_host_to_rdr_common_header_t *
ccid_get_host_to_rdr_msg(struct f_ccid *ccid, void *buffer, u32 size)
{
    int ret = 0;
    u8 pos = 0;
    u8 type = 0;
    ccid_host_to_rdr_common_header_t *header = NULL;

    UNPACK_BYTE(buffer, pos, type, size, ret, h2rdr_exit);

    header = ccid_alloc_msg(ccid, type);
    if (!header)
        goto h2rdr_exit;

    header->message_type = type;

    UNPACK_DWORD(buffer, pos, header->length, size, ret, h2rdr_exit);
    UNPACK_BYTE(buffer, pos, header->slot, size, ret, h2rdr_exit);
    UNPACK_BYTE(buffer, pos, header->sequence, size, ret, h2rdr_exit);

    switch (type)
    {
    case USB_CCID_HOST_TO_RDR_ICC_POWER_ON:
        {
            ccid_host_to_rdr_icc_power_on_t *msg =
                (ccid_host_to_rdr_icc_power_on_t *)header;
            UNPACK_BYTE(buffer, pos, msg->power_select, size, ret, h2rdr_exit);
            break;
        }

    case USB_CCID_HOST_TO_RDR_ICC_POWER_OFF:
    case USB_CCID_HOST_TO_RDR_GET_SLOT_STATUS:
    case USB_CCID_HOST_TO_RDR_GET_PARAMETERS:
    case USB_CCID_HOST_TO_RDR_RESET_PARAMETERS:
    case USB_CCID_HOST_TO_RDR_ABORT:
        break;

    case USB_CCID_HOST_TO_RDR_XFER_BLOCK:
        ret = ccid_get_xfer_block_msg(header, buffer, size);
        if (ret) {
            pr_emerg("%s Failed to ccid_get_xfer_block_msg!\n", __FUNCTION__);
            goto h2rdr_exit;
        }
        break;

    case USB_CCID_HOST_TO_RDR_SET_PARAMETERS:
        ret = ccid_set_params_msg(header, buffer, size);
        if (ret) {
            pr_emerg("%s Failed to ccid_set_params_msg!\n", __FUNCTION__);
            goto h2rdr_exit;
        }
        break;

    case USB_CCID_HOST_TO_RDR_ESCAPE:
        {
            ccid_host_to_rdr_escape_t *msg =
                (ccid_host_to_rdr_escape_t *)header;

            pos += 3; /* 3 reserved bytes*/
            CHECK_POS(pos, size, ret, h2rdr_exit);

            msg->data = kzalloc(header->length, GFP_ATOMIC);
            if (!msg->data)
            {
                pr_emerg("%s Failed allocate data buffer!\n", __FUNCTION__);
                ret = -ENOMEM;
                goto h2rdr_exit;
            }

            UNPACK_BYTE_ARRAY(buffer, pos, msg->data, header->length, size, ret, h2rdr_exit);
            break;
        }

    case USB_CCID_HOST_TO_RDR_ICC_CLOCK:
        {
            ccid_host_to_rdr_icc_clk_t *msg =
                (ccid_host_to_rdr_icc_clk_t *)header;
            UNPACK_BYTE(buffer, pos, msg->clock_command, size, ret, h2rdr_exit);
            break;
        }

    case USB_CCID_HOST_TO_RDR_T0_APDU:
        ret = ccid_get_t0_apdu_msg(header, buffer, size);
        if (ret) {
            pr_emerg("%s Failed to ccid_set_params_msg!\n", __FUNCTION__);
            goto h2rdr_exit;
        }
        break;

    case USB_CCID_HOST_TO_RDR_SECURE:
        ret = ccid_get_secure_msg(header, buffer, size);
        if (ret)
        {
            pr_emerg("%s Failed to unpack secure_msg!\n", __FUNCTION__);
            goto h2rdr_exit;
        }
        break;

    case USB_CCID_HOST_TO_RDR_MECHANICAL:
        {
            ccid_host_to_rdr_mechanical_t *msg =
                (ccid_host_to_rdr_mechanical_t *)header;
            UNPACK_BYTE(buffer, pos, msg->function, size, ret, h2rdr_exit);
            break;
        }

    case USB_CCID_HOST_TO_RDR_SET_D_RATE_AND_CLK_FREQ:
        {
            ccid_host_to_rdr_set_d_rate_and_clk_freq_t *msg =
                (ccid_host_to_rdr_set_d_rate_and_clk_freq_t *)header;
            pos += 3;
            CHECK_POS(pos, size, ret, h2rdr_exit);
            UNPACK_DWORD(buffer, pos, msg->clock_frequency, size, ret, h2rdr_exit);
            UNPACK_DWORD(buffer, pos, msg->data_rate, size, ret, h2rdr_exit);
            break;
        }

    default:
        pr_emerg("%s Unknown Msg!\n", __FUNCTION__);
        ret= -EINVAL;
        break;
    }

h2rdr_exit:
    if (ret && header) {
        ccid_free_msg(ccid, header);
        header = NULL;
    }

    return header;
}

static int ccid_pack_header(ccid_rdr_to_host_common_header_t *header,
    u8 *buffer, u32 size, u8 *pos)
{
    int ret = 0;

    PACK_DWORD(buffer, *pos, header->length, size, ret, p_header_exit);
    PACK_BYTE(buffer, *pos, header->slot, size, ret, p_header_exit);
    PACK_BYTE(buffer, *pos, header->sequence, size, ret, p_header_exit);
    PACK_BYTE(buffer, *pos, header->status, size, ret, p_header_exit);
    PACK_BYTE(buffer, *pos, header->error, size, ret, p_header_exit);
p_header_exit:
    return ret;
}

static int ccid_pack_data_block_msg(ccid_rdr_to_host_common_header_t *header,
    u8 *buffer, u32 size)
{
    u8 pos = 0;
    int ret = 0;
    ccid_rdr_to_host_data_block_t *msg =
        (ccid_rdr_to_host_data_block_t *)header;

    PACK_BYTE(buffer, pos, USB_CCID_RDR_TO_HOST_DATA_BLOCK,
        size, ret, p_data_exit);

    ret = ccid_pack_header(header, buffer, size, &pos);
    if (ret) {
        goto p_data_exit;
    }

    PACK_BYTE(buffer, pos, msg->chain_parameter, size, ret, p_data_exit);
    PACK_BYTE_ARRAY(buffer, pos, msg->data, header->length,
        size, ret, p_data_exit);
p_data_exit:
    return ret;
}

static int ccid_pack_slot_status_msg(ccid_rdr_to_host_common_header_t *header,
    u8 *buffer, u32 size)
{
    u8 pos = 0;
    int ret = 0;
    ccid_rdr_to_host_slot_status_t *msg =
        (ccid_rdr_to_host_slot_status_t *)header;

    PACK_BYTE(buffer, pos, USB_CCID_RDR_TO_HOST_SLOT_STATUS,
        size, ret, p_slot_exit);

    ret = ccid_pack_header(header, buffer, size, &pos);
    if (ret) {
        goto p_slot_exit;
    }

    PACK_BYTE(buffer, pos, msg->clock_status,
        size, ret, p_slot_exit);
p_slot_exit:
    return ret;
}

static int ccid_pack_parameters_msg(ccid_rdr_to_host_common_header_t *header,
    u8 *buffer, u32 size)
{
    u8 pos = 0;
    int ret = 0;
    ccid_rdr_to_host_params_t *msg =
        (ccid_rdr_to_host_params_t *)header;

    PACK_BYTE(buffer, pos, USB_CCID_RDR_TO_HOST_PARAMETERS,
        size, ret, p_param_exit);

    ret = ccid_pack_header(header, buffer, size, &pos);
    if (ret) {
        goto p_param_exit;
    }

    PACK_BYTE(buffer, pos, msg->protocol_num, size, ret, p_param_exit);
    PACK_BYTE(buffer, pos, msg->protocol_data.f_index_d_index,
        size, ret, p_param_exit);
    PACK_BYTE(buffer, pos, msg->protocol_data.tccks,
        size, ret, p_param_exit);
    PACK_BYTE(buffer, pos, msg->protocol_data.guard_time,
        size, ret, p_param_exit);
    PACK_BYTE(buffer, pos, msg->protocol_data.waiting_integer,
        size, ret, p_param_exit);
    PACK_BYTE(buffer, pos, msg->protocol_data.clock_stop,
        size, ret, p_param_exit);

    if (msg->protocol_num)
    {
        PACK_BYTE(buffer, pos, msg->protocol_data.ifsc_size,
            size, ret, p_param_exit);
        PACK_BYTE(buffer, pos, msg->protocol_data.nad_value,
            size, ret, p_param_exit);
    }
p_param_exit:
    return ret;
}

static int ccid_pack_escape_msg(ccid_rdr_to_host_common_header_t *header,
    u8 *buffer, u32 size)
{
    u8 pos = 0;
    int ret = 0;
    ccid_rdr_to_host_escape_t *msg =
        (ccid_rdr_to_host_escape_t *)header;

    PACK_BYTE(buffer, pos, USB_CCID_RDR_TO_HOST_ESCAPE,
        size, ret, p_escape_exit);

    ret = ccid_pack_header(header, buffer, size, &pos);
    if (ret) {
        goto p_escape_exit;
    }

    PACK_WORD(buffer, pos, USB_CCID_RESERVED, size, ret, p_escape_exit);
    PACK_BYTE(buffer, pos, USB_CCID_RESERVED, size, ret, p_escape_exit);
    PACK_BYTE_ARRAY(buffer, pos, msg->data, header->length,
        size, ret, p_escape_exit);
p_escape_exit:
    return ret;
}

static int ccid_pack_d_rate_and_clk_frq_msg(
    ccid_rdr_to_host_common_header_t *header, u8 *buffer, u32 size)
{
    u8 pos = 0;
    int ret = 0;
    ccid_rdr_to_host_d_rate_and_clk_freq_t *msg =
        (ccid_rdr_to_host_d_rate_and_clk_freq_t *)header;

    PACK_BYTE(buffer, pos, USB_CCID_RDR_TO_HOST_D_RATE_AND_CLK_FREQ,
        size, ret, p_rate_clk_exit);

    ret = ccid_pack_header(header, buffer, size, &pos);
    if (ret) {
        goto p_rate_clk_exit;
    }

    PACK_BYTE(buffer, pos, USB_CCID_RESERVED, size, ret, p_rate_clk_exit);
    PACK_DWORD(buffer, pos, msg->clock_frequency, size, ret, p_rate_clk_exit);
    PACK_DWORD(buffer, pos, msg->data_rate, size, ret, p_rate_clk_exit);
p_rate_clk_exit:
    return ret;
}


static int ccid_send_msg(struct f_ccid *ccid,
    ccid_rdr_to_host_common_header_t *header)
{
    int ret = 0;
    u32 size;

    size = header->length + USB_CCID_MSG_HEADER_SIZE;

    if (size > ccid->ret_msg_size) {
        if (ccid->ret_msg_buf) {
            kfree(ccid->ret_msg_buf);
            ccid->ret_msg_buf = NULL;
            ccid->ret_msg_size = 0;
        }
        ccid->ret_msg_buf = kzalloc(size, GFP_KERNEL);
        if (!ccid->ret_msg_buf) {
            ret = -ENOMEM;
            goto send_msg_exit;
        }
        ccid->ret_msg_size = size;
    }

    switch (header->message_type)
    {
    case USB_CCID_RDR_TO_HOST_DATA_BLOCK:
        ret = ccid_pack_data_block_msg(header, ccid->ret_msg_buf, size);
        break;
    case USB_CCID_RDR_TO_HOST_SLOT_STATUS:
        ret = ccid_pack_slot_status_msg(header, ccid->ret_msg_buf, size);
        break;
    case USB_CCID_RDR_TO_HOST_PARAMETERS:
        ret = ccid_pack_parameters_msg(header, ccid->ret_msg_buf, size);
        break;
    case USB_CCID_RDR_TO_HOST_ESCAPE:
        ret = ccid_pack_escape_msg(header, ccid->ret_msg_buf, size);
        break;
    case USB_CCID_RDR_TO_HOST_D_RATE_AND_CLK_FREQ:
        ret = ccid_pack_d_rate_and_clk_frq_msg(header, ccid->ret_msg_buf, size);
        break;
    default:
        pr_emerg("%s: Unknown Message type\n", __FUNCTION__);
        ret = -EINVAL;
        break;
    }
    if (ret) {
        goto send_msg_exit;
    }

    ret = gusim_return_msg(&ccid->port, ccid->ret_msg_buf, ccid->ret_msg_size);
    if (ret) {
        pr_emerg("%s: failed to send data to core\n", __FUNCTION__);
        ccid->stat_send_msg_err++;
        goto send_msg_exit;
    }

    /* free out msg if app is finished with the data */
    if ((header->status & USB_CCID_GET_COMMAND_STATUS_BITS) !=
         USB_CCID_COMMAND_BITS_TIME_EXT) {
        ccid_free_msg(ccid, ccid->msg_array[header->slot]);
        ccid->msg_array[header->slot] = NULL;
        ccid->cur_header = NULL;
    }

send_msg_exit:
    /* if fail, don't need to free ccid->ret_msg_buf
     * it can be reused.
     */

    return ret;
}

static void
ccid_return_msg(struct f_ccid *ccid, ccid_host_to_rdr_common_header_t *header)
{
    int ret = 0;
    //u32 length = 0;

    switch(header->message_type)
    {
        case USB_CCID_HOST_TO_RDR_SET_D_RATE_AND_CLK_FREQ:
        {
            ccid_rdr_to_host_d_rate_and_clk_freq_t msg;

            msg.header.message_type = USB_CCID_RDR_TO_HOST_D_RATE_AND_CLK_FREQ;
            msg.header.length = 0x08;
            msg.header.slot = header->slot;
            msg.header.sequence = header->sequence;
            msg.header.status = 0;
            msg.header.error = 0;
            msg.clock_frequency = 0x0012c0;
            msg.data_rate = 0x002a00;

            ret = ccid_send_msg(ccid, (ccid_rdr_to_host_common_header_t *)&msg);/* [false alarm]:fortify disable */
            break;
        }
        case USB_CCID_HOST_TO_RDR_ICC_POWER_OFF:
        {
            /* msg can be local as the fd copies to a new buffer */
            ccid_rdr_to_host_slot_status_t msg;

            msg.header.message_type = USB_CCID_RDR_TO_HOST_SLOT_STATUS;
            msg.header.length = 0;
            msg.header.slot = header->slot;
            msg.header.sequence = header->sequence;
            msg.header.status = 0x01;
            msg.header.error = 0;
            msg.clock_status = 0x01;

            ccid->port.usim_cmd = PCSC_USIM_POWER_OFF;
            ccid->port.apdu_len = 0;
            (void)gusim_msg_proc(&ccid->port);

            ret = ccid_send_msg(ccid, (ccid_rdr_to_host_common_header_t *)&msg);/* [false alarm]:fortify disable */
            break;
        }
        case USB_CCID_HOST_TO_RDR_GET_SLOT_STATUS:
        case USB_CCID_HOST_TO_RDR_ICC_CLOCK:
        case USB_CCID_HOST_TO_RDR_T0_APDU:
        case USB_CCID_HOST_TO_RDR_MECHANICAL:
        case USB_CCID_HOST_TO_RDR_ABORT:
        {
            /* msg can be local as the fd copies to a new buffer */
            ccid_rdr_to_host_slot_status_t msg;

            msg.header.message_type = USB_CCID_RDR_TO_HOST_SLOT_STATUS;
            msg.header.length = 0;
            msg.header.slot = header->slot;
            msg.header.sequence = header->sequence;
            msg.header.status = 0x01;
            msg.header.error = 0;
            msg.clock_status = 0x01;

            ret = ccid_send_msg(ccid, (ccid_rdr_to_host_common_header_t *)&msg);/* [false alarm]:fortify disable */
            break;
        }
        case USB_CCID_HOST_TO_RDR_ICC_POWER_ON:
        {
            ccid_rdr_to_host_data_block_t msg;

            msg.header.message_type = USB_CCID_RDR_TO_HOST_DATA_BLOCK;
            msg.header.slot = header->slot;
            msg.header.sequence = header->sequence;
            msg.chain_parameter = 0;
            msg.header.status = 0;
            msg.header.error = 0;
            msg.data = ccid->port.apdu_data;

            ccid->port.usim_cmd = PCSC_USIM_POWER_ON;
            ccid->port.apdu_len = 0;
            ret = gusim_msg_proc(&ccid->port);
            if(ret) {
                msg.header.length = 0;
                msg.header.status = 0x41;
                msg.header.error = 0xFE;
            }
            else {
                msg.header.length = ccid->port.apdu_len;
                msg.header.status = 0;
                msg.header.error = 0;
            }

            ret = ccid_send_msg(ccid, (ccid_rdr_to_host_common_header_t *)&msg);/* [false alarm]:fortify disable */
            break;
        }
        case USB_CCID_HOST_TO_RDR_SET_PARAMETERS:
        case USB_CCID_HOST_TO_RDR_GET_PARAMETERS:
        {
            ccid_host_to_rdr_set_params_t *params =
                (ccid_host_to_rdr_set_params_t *)header;
            ccid_rdr_to_host_params_t msg;

            msg.header.message_type = USB_CCID_RDR_TO_HOST_PARAMETERS;
            msg.header.length = 0x05;
            msg.header.slot = header->slot;
            msg.header.sequence = header->sequence;
            msg.header.status = 0x00;
            msg.header.error = 0x00;
            msg.protocol_num = 0;


            msg.protocol_data.f_index_d_index =
            params->protocol_data.f_index_d_index;
            msg.protocol_data.tccks = 0x02;
            msg.protocol_data.guard_time = params->protocol_data.guard_time;
            msg.protocol_data.waiting_integer =
            params->protocol_data.waiting_integer;
            msg.protocol_data.clock_stop = params->protocol_data.clock_stop;

            ret = ccid_send_msg(ccid, (ccid_rdr_to_host_common_header_t *)&msg);/* [false alarm]:fortify disable */
            break;
        }
        case USB_CCID_HOST_TO_RDR_XFER_BLOCK:
        {
            ccid_rdr_to_host_data_block_t msg;
            ccid_host_to_rdr_xfer_block_t *xfer =
                (ccid_host_to_rdr_xfer_block_t *)header;

            if(header->length) {
                memcpy(ccid->port.apdu_data, xfer->data, header->length);
            }
            ccid->port.usim_cmd = PCSC_USIM_APDU_CMD;
            ccid->port.apdu_len = header->length;
            ret = gusim_msg_proc(&ccid->port);

            msg.header.length = ccid->port.apdu_len;
            msg.data = ccid->port.apdu_data;
            msg.header.message_type = USB_CCID_RDR_TO_HOST_DATA_BLOCK;
            msg.header.slot = header->slot;
            msg.header.sequence = header->sequence;
            msg.header.status = 0;
            msg.header.error = 0;
            msg.chain_parameter = 0;

            if(ret) {
                msg.header.length = 0;
            }

            ret = ccid_send_msg(ccid, (ccid_rdr_to_host_common_header_t *)&msg);/* [false alarm]:fortify disable */
            break;
        }
        case USB_CCID_HOST_TO_RDR_ESCAPE:
        {
            uint8_t data[7];
            ccid_rdr_to_host_escape_t msg;
            ccid_host_to_rdr_escape_t *escape =
                (ccid_host_to_rdr_escape_t *)header;

            msg.header.message_type = USB_CCID_RDR_TO_HOST_ESCAPE;
            msg.header.slot = header->slot;
            msg.header.sequence = header->sequence;
            msg.header.error = 0;
            msg.header.status = 0x01;
            msg.data = data;

            if (header->length == 1) {
                data[0] = 0x01;
                data[1] = 0x01;
                data[2] = 0x00;
                data[3] = 0x02;
                data[4] = 0x03;
                data[5] = 0x03;
                data[6] = 0x0e;

                msg.header.length = 0x07;
            }
            else {
                data[0] = escape->data[0];
                msg.header.length = 0x01;
            }

            ret = ccid_send_msg(ccid, (ccid_rdr_to_host_common_header_t *)&msg);/* [false alarm]:fortify disable */
            break;
        }
        default:
            pr_emerg("%s: unknown pcsc command, cmd=%d \n",
                    __FUNCTION__, header->message_type);
            break;
    }
    return;
}

static void ccid_return_msg_proc(struct work_struct *work)
{
    struct f_ccid *ccid;
    ccid_host_to_rdr_common_header_t *header;

    ccid = container_of(work, struct f_ccid, rw_work.work);

    ccid->stat_proc_msg++;
    header = ccid->cur_header;
    if (header) {
        ccid->stat_return_msg++;
        ccid_return_msg(ccid, header);
    }
}

static void ccid_handle_msg(struct gusim *port, void* buf, u32 size)
{
    ccid_host_to_rdr_common_header_t *header = NULL;
    struct f_ccid *ccid = port_to_ccid(port);
    int ret = 0;

    header = ccid_get_host_to_rdr_msg(ccid, buf, size);
    if (!header) {
        pr_emerg("%s: Error unpacking message\n", __FUNCTION__);
        ccid->stat_parse_msg_err++;
        ret = -ENOMEM;
        goto handle_msg_exit;
    }

    if (header->slot >= ccid->num_of_slots) {
        pr_emerg("%s: Invalid slot: %d\n", __FUNCTION__, header->slot);
        ret = -EINVAL;
        goto handle_msg_exit;
    }

    ccid->msg_array[header->slot] = header;
    ccid->cur_header = header;

    /* schedule */
    ccid->stat_handle_msg_sched++;
    queue_delayed_work(ccid->ccid_work_queue, &ccid->rw_work, 0);

handle_msg_exit:
    if (ret && header) {
        u8 slot = header->slot;
        ccid_free_msg(ccid, header);
        ccid->msg_array[slot] = NULL;
    }
}

/*-------------------------------------------------------------------------*/

/* CCID function driver setup/binding */
static int
ccid_bind(struct usb_configuration *c, struct usb_function *f)
{
    struct usb_composite_dev *cdev = c->cdev;
    struct f_ccid *ccid = func_to_ccid(f);
    int status;
    struct usb_ep *ep;
    u32 req_size;

    /* allocate instance-specific interface IDs, and patch descriptors */
    status = usb_interface_id(c, f);
    if (status < 0) {
        goto fail;
    }

    ccid->ctrl_id = ccid->data_id = status;
    ccid_interface_desc.bInterfaceNumber = status;

    status = -ENODEV;
    bsp_usb_add_setup_dev((unsigned)ccid->data_id);

    /* allocate instance-specific endpoints */
    ep = usb_ep_autoconfig(cdev->gadget, &ccid_fs_in_desc);
    if (!ep) {
        goto fail;
    }
    ccid->port.in = ep;
    ep->driver_data = cdev; /* claim */

    ep = usb_ep_autoconfig(cdev->gadget, &ccid_fs_out_desc);
    if (!ep){
        goto fail;
    }
    ccid->port.out = ep;
    ep->driver_data = cdev; /* claim */

    ep = usb_ep_autoconfig(cdev->gadget, &ccid_fs_notify_desc);
    if (!ep){
        goto fail;
    }
    ccid->notify = ep;
    ep->driver_data = cdev; /* claim */

    /* allocate notification
     * size : one byte for msg type, and 2 bits for every slot
     * rounded up to the nearest byte
     */
    req_size = USB_CCID_MSG_TYPE_LEN + (ccid->num_of_slots>>2) +
                ((ccid->num_of_slots & 0x3) ? 1 : 0) + USB_CCID_REQ_SIZE_RESERV;
    ccid->notify_req = gs_usim_alloc_req(ep, req_size + 2, GFP_KERNEL);
    if (!ccid->notify_req){
        goto fail;
    }

    ccid->notify_req->complete = ccid_notify_complete;
    ccid->notify_req->context = ccid;

    /* copy descriptors */
    f->descriptors = usb_copy_descriptors(ccid_fs_function);
    if (!f->descriptors){
        goto fail;
    }

    ccid->status_array =
        kmalloc(sizeof(slot_status_t) * ccid->num_of_slots, GFP_KERNEL);
    if (!ccid->status_array){
        goto fail;
    }

    /* support all relevant hardware speeds... we expect that when
     * hardware is dual speed, all bulk-capable endpoints work at
     * both speeds
     */
    if (gadget_is_dualspeed(c->cdev->gadget)) {
        ccid_hs_in_desc.bEndpointAddress =
                ccid_fs_in_desc.bEndpointAddress;
        ccid_hs_out_desc.bEndpointAddress =
                ccid_fs_out_desc.bEndpointAddress;
        ccid_hs_notify_desc.bEndpointAddress =
                ccid_fs_notify_desc.bEndpointAddress;

        /* copy descriptors */
        f->hs_descriptors = usb_copy_descriptors(ccid_hs_function);
    }

    if (gadget_is_superspeed(c->cdev->gadget)) {
        ccid_ss_in_desc.bEndpointAddress =
            ccid_fs_in_desc.bEndpointAddress;
        ccid_ss_out_desc.bEndpointAddress =
            ccid_fs_out_desc.bEndpointAddress;

        /* copy descriptors, and track endpoint copies */
        f->ss_descriptors = usb_copy_descriptors(ccid_ss_function);
        if (!f->ss_descriptors){
            goto fail;
        }
    }

    DBG(cdev, "ccid port_num(%d): %s speed IN/%s OUT/%s NOTIFY/%s\n",
            ccid->port_num,
            gadget_is_superspeed(c->cdev->gadget) ? "super" :
            gadget_is_dualspeed(c->cdev->gadget) ? "dual" : "full",
            ccid->port.in->name, ccid->port.out->name,
            ccid->notify->name);
    return 0;

fail:
    if (ccid->notify_req)
        gs_usim_free_req(ccid->notify, ccid->notify_req);

    /* we might as well release our claims on endpoints */
    if (ccid->notify)
        ccid->notify->driver_data = NULL;
    if (ccid->port.out)
        ccid->port.out->driver_data = NULL;
    if (ccid->port.in)
        ccid->port.in->driver_data = NULL;

    if (ccid->status_array) {
        kfree(ccid->status_array);
        ccid->status_array = NULL;
    }
    ERROR(cdev, "%s/%p: can't bind, err %d\n", f->name, f, status);

    return status;
}

static void
ccid_unbind(struct usb_configuration *c, struct usb_function *f)
{
    struct f_ccid   *ccid = func_to_ccid(f);

    if (gadget_is_dualspeed(c->cdev->gadget))
        usb_free_descriptors(f->hs_descriptors);
    if (gadget_is_superspeed(c->cdev->gadget))
        usb_free_descriptors(f->ss_descriptors);
    usb_free_descriptors(f->descriptors);
    gs_usim_free_req(ccid->notify, ccid->notify_req);
    if (ccid->status_array) {
        kfree(ccid->status_array);
    }
    if (ccid->msg_array) {
        kfree(ccid->msg_array);
    }
    if (ccid->ccid_work_queue) {
        destroy_workqueue(ccid->ccid_work_queue);
    }
    kfree(ccid);
}

/**
 * ccid_bind_config - add a CCID function to a configuration
 * @c: the configuration to support the USIM instance
 * @port_num: port this interface will use
 * Context: single threaded during gadget setup
 *
 * Returns zero on success, else negative errno.
 */
int ccid_bind_config(struct usb_configuration *c, u32 port_num)
{
    struct f_ccid       *ccid = NULL;
    int                 status = 0;

    if (ccid_string_defs[0].id == 0) {
        status = usb_string_id(c->cdev);
        if (status < 0)
            return status;
        ccid_string_defs[0].id = status;
        ccid_interface_desc.iInterface = status;
    }

    /* allocate and initialize one new instance */
    ccid = kzalloc(sizeof(*ccid), GFP_KERNEL);
    if (!ccid)
        return -ENOMEM;

    spin_lock_init(&ccid->lock);

    ccid->port_num = port_num;

    ccid->port.connect = ccid_connect;
    ccid->port.disconnect = ccid_disconnect;
    ccid->port.handle_msg = ccid_handle_msg;
    ccid->port.dump_fd = ccid_dump;

    ccid->port.func.name = "ccid";
    ccid->port.func.strings = ccid_strings;

    /* descriptors are per-instance copies */
    ccid->port.func.bind = ccid_bind;
    ccid->port.func.unbind = ccid_unbind;
    ccid->port.func.set_alt = ccid_set_alt;
    ccid->port.func.setup = ccid_setup;
    ccid->port.func.disable = ccid_disable;

    /* init the ctx members */
    ccid->num_of_slots = ccid_extra_if_desc.max_slot_index + 1;
    ccid->num_data_rates = ccid_extra_if_desc.num_data_rate_supported;
    ccid->num_clock_frqs = ccid_extra_if_desc.num_clock_supported;
    ccid->msg_array = kzalloc(sizeof(ccid_host_to_rdr_common_header_t *) *
                        ccid->num_of_slots, GFP_ATOMIC);
    if (!ccid->msg_array)
    {
        status = -ENOMEM;
        pr_emerg("%s: failed to allocate message array\n", __FUNCTION__);
        goto bind_exit;
    }

    ccid->ccid_work_queue = create_singlethread_workqueue("ccid_proc");
    if (!ccid->ccid_work_queue) {
        status = -EFAULT;
        goto bind_exit;
    }
    INIT_DELAYED_WORK(&ccid->rw_work, ccid_return_msg_proc);

    status = usb_add_function(c, &ccid->port.func);

bind_exit:
    if (status) {
        if (ccid) {
            if (ccid->msg_array) {
                kfree(ccid->msg_array);
                ccid->msg_array = NULL;
            }
            if (ccid->ccid_work_queue) {
                destroy_workqueue(ccid->ccid_work_queue);
                ccid->ccid_work_queue = NULL;
            }
            kfree(ccid);
        }
    }

    /* coverity[leaked_storage] */
    return status;
}

static void ccid_dump(struct gusim *port)
{
    struct f_ccid *ccid = port_to_ccid(port);

    pr_emerg("\n=== dump f_ccid info ===\n");
    pr_emerg("stat_parse_msg_err        %d\n", ccid->stat_parse_msg_err);
    pr_emerg("stat_handle_msg_sched     %d\n", ccid->stat_handle_msg_sched);
    pr_emerg("stat_proc_msg             %d\n", ccid->stat_proc_msg);
    pr_emerg("stat_return_msg           %d\n", ccid->stat_return_msg);
    pr_emerg("stat_send_msg_err         %d\n", ccid->stat_send_msg_err);
    pr_emerg("stat_alloc_msg            %d\n", ccid->stat_alloc_msg);
    pr_emerg("stat_alloc_msg_fail       %d\n", ccid->stat_alloc_msg_fail);
    pr_emerg("stat_free_msg             %d\n", ccid->stat_free_msg);
    pr_emerg("stat_notify_no_req        %d\n", ccid->stat_notify_no_req);
    pr_emerg("stat_notify_submit        %d\n", ccid->stat_notify_submit);
    pr_emerg("stat_notify_submit_err    %d\n", ccid->stat_notify_submit_err);
    pr_emerg("stat_notify_complete      %d\n", ccid->stat_notify_complete);
    return;
}
