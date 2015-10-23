/*
 * Copyright (c) 2010, HiSilicon Technologies Co., Ltd.
 * All rights reserved.
 */

#include <boot.h>
#include <usb.h>
#include <time.h>
#include <config.h>
#include <dwc_usb.h>

struct ep_type
{
    char active;
    char busy;
    char done;
    int rc;
    int size;
    //struct wakeup complete;
} ;

extern struct usb_endpoint *ep1in, *ep1out;
extern struct usb_request *rx_req, *tx_req;
static struct ep_type endpoints[5];
static setup_packet ctrlreq[3];
static unsigned char ctrlresp[2];

static struct usb_device_descriptor device_descriptor =
{
    .bLength            = sizeof(struct usb_device_descriptor),
    .bDescriptorType    = USB_DT_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = 0x40,
    /*.idVendor           = 0x12D1,
    .idProduct          = 0x360D,*/
    .idVendor           = 0x18D1,
    .idProduct          = 0x0D02,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = 3,
    .bNumConfigurations = 1
};

static struct usb_config_bundle
{
    struct usb_config_descriptor config_descriptor;
    struct usb_interface_descriptor interface_descriptor;
    struct usb_endpoint_descriptor endpoint1_descriptor;
    struct usb_endpoint_descriptor endpoint2_descriptor;
} __attribute__((packed))  config_bundle = 
{
    .config_descriptor =
    {
        .bLength             = sizeof(struct usb_config_descriptor),
        .bDescriptorType     = USB_DT_CONFIG,
        .wTotalLength        = sizeof(struct usb_config_descriptor)
                             + sizeof(struct usb_interface_descriptor)
                             + sizeof(struct usb_endpoint_descriptor) * USB_NUM_ENDPOINTS,
        .bNumInterfaces      = 1,
        .bConfigurationValue = 1,
        .iConfiguration      = 0,
        .bmAttributes        = USB_CONFIG_ATT_ONE,
        .bMaxPower           = 0x80
    },
    .interface_descriptor =
    {
        .bLength             = sizeof(struct usb_interface_descriptor),
        .bDescriptorType     = USB_DT_INTERFACE,
        .bInterfaceNumber    = 0,
        .bAlternateSetting   = 0,
        .bNumEndpoints       = USB_NUM_ENDPOINTS,
        .bInterfaceClass     = USB_CLASS_VENDOR_SPEC,
        .bInterfaceSubClass  = 0x42,
        .bInterfaceProtocol  = 0x03,
        .iInterface          = 0
    },
    .endpoint1_descriptor =
    {
        .bLength             = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType     = USB_DT_ENDPOINT,
        .bEndpointAddress    = 0x81,
        .bmAttributes        = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize      = 0,
        .bInterval           = 0
    },
    .endpoint2_descriptor =
    {
        .bLength             = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType     = USB_DT_ENDPOINT,
        .bEndpointAddress    = 0x01,
        .bmAttributes        = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize      = 0,
        .bInterval           = 1
    }
};

static const struct usb_string_descriptor string_devicename =
{
    24,
    USB_DT_STRING,
    {'A', 'n', 'd', 'r', 'o', 'i', 'd', ' ', '2', '.', '0'}
};

static const struct usb_string_descriptor lang_descriptor =
{
    4,
    USB_DT_STRING,
    {0x0409}
};

static const struct usb_string_descriptor serial_string = 
{
    16,
    USB_DT_STRING,
    {'K', '3', ' ', 'V', '2', '0', '0'}
};

extern int bootmode;
extern int usb_enum_comp;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

struct usb_endpoint
{
    struct usb_endpoint *next;
    unsigned int maxpkt;
    struct usb_request *req;

    unsigned char num;
    unsigned char in;
};


extern void cprintf(const char *fmt, ...);
struct usb_endpoint *ept_list = 0;


struct usb_endpoint *usb_endpoint_alloc(unsigned num, unsigned in, unsigned maxpkt)
{
    struct usb_endpoint *ept;
    ept = alloc(sizeof(*ept));
    ept->num = num;
    ept->in = !!in;
    ept->req = 0;
    ept->maxpkt = maxpkt;
    ept->next = ept_list;
    ept_list = ept;
    
    return ept;
}

struct usb_request *usb_request_alloc(unsigned bufsiz)
{
    struct usb_request *req;
    req = alloc(sizeof(*req));
    req->buf = alloc(bufsiz);
    req->item = alloc(32);
    
    return req;
}

/*removed by qyf for compile warning : defined but not used*/
#if 0
static const char *reqname(unsigned char r)  //reserved
{
    switch(r) 
    {
        case USB_REQ_GET_STATUS: return "GET_STATUS";
        case USB_REQ_CLEAR_FEATURE: return "CLEAR_FEATURE";
        case USB_REQ_SET_FEATURE: return "SET_FEATURE";
        case USB_REQ_SET_ADDRESS: return "SET_ADDRESS";
        case USB_REQ_GET_DESCRIPTOR: return "GET_DESCRIPTOR";
        case USB_REQ_SET_DESCRIPTOR: return "SET_DESCRIPTOR";
        case USB_REQ_GET_CONFIGURATION: return "GET_CONFIGURATION";
        case USB_REQ_SET_CONFIGURATION: return "SET_CONFIGURATION";
        case USB_REQ_GET_INTERFACE: return "GET_INTERFACE";
        case USB_REQ_SET_INTERFACE: return "SET_INTERFACE";
        default: return "*UNKNOWN*";
    }
}
#endif

int usb_drv_port_speed(void)/*To detect which mode was run, high speed or full speed*/
{
    //return (readl(dev_global_regs->dsts) & 2) == 0 ? 1 : 0;
    return (DSTS & 2)== 0 ? 1 : 0;
}

static void reset_endpoints(void)
{
    unsigned int i;

    /*Init endpoints struct*/
    for (i = 0; i < sizeof(endpoints)/sizeof(struct ep_type); i++)
    {
        endpoints[i].active = 0;  //false
        endpoints[i].busy = 0;    //false
        endpoints[i].rc = -1;
        endpoints[i].done = 1;    //true
    }

    /* EP0 IN ACTIVE NEXT=1 */
    DIEPCTL0 = 0x8800;
    
    /* EP0 OUT ACTIVE */
    DOEPCTL0 = 0x8000;

    /* Clear any pending OTG Interrupts */
    GOTGINT  = 0xFFFFFFFF;
    
    /* Clear any pending interrupts */
    GINTSTS = 0xFFFFFFFF;
    DIEPINT0 = 0xFFFFFFFF;
    DOEPINT0 = 0xFFFFFFFF;
    DIEPINT1 = 0xFFFFFFFF;
    DOEPINT1 = 0xFFFFFFFF;

    /* IN EP interrupt mask */
    DIEPMSK = 0x0D;
    
    /* OUT EP interrupt mask */
    DOEPMSK = 0x0D;
    
    /* Enable interrupts on Ep0 */
    DAINTMSK = 0x00010001;
    
    /* EP0 OUT Transfer Size:64 Bytes, 1 Packet, 3 Setup Packet, Read to receive setup packet*/
    DOEPTSIZ0 = 0x60080040;
    DOEPDMA0 = (unsigned long)(&ctrlreq[0]); //notes that:the compulsive conversion is expectable.
    /* EP0 OUT ENABLE CLEARNAK */
    DOEPCTL0 |= 0x84000000;

}

int usb_drv_request_endpoint(int type, int dir)
{
    int ep = 1;    /*FIXME*/
    int ret = -1;
    unsigned long newbits;
    
    ret = ep | dir;
    newbits = (type << 18) | 0x10000000;
    if (dir) DIEPCTL(ep) = (DIEPCTL(ep) & ~0x000C0000) | newbits | (ep<<22)|0x20000;
    else DOEPCTL(ep) = (DOEPCTL(ep) & ~0x000C0000) | newbits;
    endpoints[ep].active = 1;//true

    return ret;
}

void usb_drv_release_endpoint(int ep)
{
    ep = ep & 0x7f;
    if (ep < 1 || ep > USB_NUM_ENDPOINTS) return;
    endpoints[ep].active = 0;//false
}

static void usb_config(void)
{

    /*FIXME, 接收和发送各端点分配FIFO，接收共用FIFO，发送各端点独立分配，单位是32bit.*/
    GDFIFOCFG = 0x07600780; 

    /*不能确定FPGA上实际RAM大小，待逻辑人员确认，目前是在0x780的范围之内分配。*/
    /* RX FIFO: 2048 bytes */
    GRXFSIZ = 0x00000200;
    /* Non-periodic TX FIFO: 128 bytes. start address: 0x200 * 4. */
    GNPTXFSIZ = 0x00200200; 
    
    /* EP1 TX FIFO: 1024 bytes. start address: 0x220 * 4. */
    DIEPTXF1 = 0x01000220;
    /* EP2 TX FIFO: 1024 bytes. start address: 0x320 * 4. */
    DIEPTXF2 = 0x01000320;
    /* EP3 TX FIFO: 1024 bytes. start address: 0x420 * 4. */
    DIEPTXF3 = 0x01000420;
    /* EP4 TX FIFO: 1024 bytes. start address: 0x520 * 4. */
    DIEPTXF4 = 0x01000520;
    
    /* EP5 TX FIFO: 128 bytes. start address: 0x620 * 4. */
    DIEPTXF5 = 0x00200620;
    /* EP6 TX FIFO: 128 bytes. start address: 0x640 * 4. */
    DIEPTXF6 = 0x00200640;
    /* EP7 TX FIFO: 128 bytes. start address: 0x660 * 4. */
    DIEPTXF7 = 0x00200660;
    /* EP8 TX FIFO: 128 bytes. start address: 0x680 * 4. */
    DIEPTXF8 = 0x00200680;
    /* EP9 TX FIFO: 128 bytes. start address: 0x6a0 * 4. */
    DIEPTXF9 = 0x002006a0;
    /* EP10 TX FIFO: 128 bytes. start address: 0x6c0 * 4. */
    DIEPTXF10 = 0x002006c0;
    /* EP11 TX FIFO: 128 bytes. start address: 0x6e0 * 4. */
    DIEPTXF11 = 0x002006e0;
    /* EP12 TX FIFO: 128 bytes. start address: 0x700 * 4. */
    DIEPTXF12 = 0x00200700;
    /* EP13 TX FIFO: 128 bytes. start address: 0x720 * 4. */
    DIEPTXF13 = 0x00200720;
    
    /* EP14 TX FIFO: 64 bytes. start address: 0x740 * 4. */
    DIEPTXF14 = 0x00100740;
    /* EP15 TX FIFO: 64 bytes. start address: 0x750 * 4. */
    DIEPTXF15= 0x00100750;

    /*Init global csr register.*/
    GAHBCFG = 0x1a1; /*set  Periodic TxFIFO Empty Level, Non-Periodic TxFIFO Empty Level, Enable DMA Unmask Global Intr*/

    #ifdef NANO_PHY
    GUSBCFG = 0x2400;/*select 8bit UTMI+ */
    #else
    GUSBCFG = 0x2411;/*select 8bit UTMI+, ULPI Inerface*/
    #endif

    /* Detect usb work mode,host or device? */
    while (GINTSTS & 1); 
    udelay(3);

    /*Init global and device mode csr register.*/
    DCFG=4;/*set Non-Zero-Length status out handshake */
    //DTHRCTL |= 0x10000;
    
    /* Interrupt unmask: IN event, OUT event, bus reset */
    GINTMSK = 0xC3C08; 
    
    while (GINTSTS & 0x2000); 
    //USB_DBG(DBG_INFO,"Enum Done.\n");

    /* Clear any pending OTG Interrupts */
    GOTGINT  = 0xFFFFFFFF;
    /* Clear any pending interrupts */
    GINTSTS = 0xFFFFFFFF;
    GINTMSK = 0xFFFFFFFF; 
    GINTSTS &=~ 0x3000; 

    /*endpoint settings cfg*/
    reset_endpoints();

    udelay(1);

    /*init finish. and ready to transfer data*/
    
    /* Soft Disconnect */
    DCTL = 0x802;
    udelay(10000);
    
    /* Soft Reconnect */
    DCTL = 0x800;

    //USB_DBG(DBG_INFO,"exit %s.\n",__func__);
}

void usb_drv_set_address(int address)
{
    //writel((readl( dev_global_regs->dcfg) & ~0x7F0) | (address << 4), dev_global_regs->dcfg);
    DCFG = (DCFG & ~0x7F0) | (address << 4);
}

static void ep_send(int ep, const void *ptr, int length)
{
    int blocksize,packets;
    endpoints[ep].busy = 1;//true
    endpoints[ep].size = length;

    DIEPCTL(ep) |= 0x8000;  /* EPx OUT ACTIVE */
    if(!ep)
    {
        blocksize = 64;
    }
    else
    {
        blocksize = usb_drv_port_speed() ? 512 : 64;
    }
    packets = (length + blocksize - 1) / blocksize;

    if (!length)  //send a null packet
    {
        DIEPTSIZ(ep) = 1 << 19;  /* one empty packet */
        DIEPDMA(ep) = 0;  /* NULL */
    }
    else    //prepare to send a packet
    {
        //cprintf("ep_send, size = 0x%x, ptr = 0x%x, packets = %d, len = %d.\n",length | (packets << 19),ptr,packets,length);
        DIEPTSIZ(ep) = length | (packets << 19);
        DIEPDMA(ep) = ptr;
    }
    DIEPCTL(ep) |= 0x84000800;  /* epena & cnak*/
}

void ep_recv(int ep, void *ptr, int length)
{
    int blocksize,packets;
    
    endpoints[ep].busy = 1;//true
    endpoints[ep].size = length;
    DOEPCTL(ep) &= ~0x00200000;  /* EPx UNSTALL */
    DOEPCTL(ep) |= 0x8000;  /* EPx OUT ACTIVE */
    blocksize = usb_drv_port_speed() ? 512 : 64;
    packets = (length + blocksize - 1) / blocksize;
    if (!length)
    {
        //cprintf("ep%d_recv, size = 0x%x, ptr = 0x%x.\n",ep, length, ptr);
        DOEPTSIZ(ep) = 1 << 19;  /* one empty packet */
        DOEPDMA(ep) = 0;//NULL  /* dummy address */
    }
    else
    {
        //cprintf("ep%d_recv, size = 0x%x, ptr = 0x%x,packets=%d.\n",ep, length, ptr, packets);
        DOEPTSIZ(ep) = length | (packets << 19);
        DOEPDMA(ep) = ptr;        
    }
    DOEPCTL(ep) |= 0x84000000;  /* EPx OUT ENABLE CLEARNAK */
}

void usb_drv_stall(int endpoint, char stall, char in)
{
    if (in)
    {
        if (stall) DIEPCTL(endpoint) |= 0x00200000;
        else DIEPCTL(endpoint) &= ~0x00200000;
    }
    else
    {
        if (stall) DOEPCTL(endpoint) |= 0x00200000;
        else DOEPCTL(endpoint) &= ~0x00200000;
    }
}

int usb_drv_send_nonblocking(int endpoint, const void *ptr, int length)
{
    //cprintf("usb_drv_send_nonblocking,endpoint = %d, ptr = 0x%x, Len=%d.\n",endpoint,ptr,length);
    ep_send(endpoint & 0x7f, ptr, length);
    return 0;
}

int usb_drv_recv(int endpoint, void* ptr, int length)
{
    ep_recv(endpoint & 0x7f, ptr, length);
    return 0;
}

void usb_drv_cancel_all_transfers(void)
{
    reset_endpoints();
}

static int hiusb_epx_tx(unsigned ep, void *buf, unsigned len)
{
    int blocksize,packets;
    unsigned long epints;
    endpoints[ep].busy = 1;//true
    endpoints[ep].size = len;

    DIEPCTL(ep) |= 0x8000;  /* EPx OUT ACTIVE */
    if(!ep)
    {
        blocksize = 64;
    }
    else
    {
        blocksize = usb_drv_port_speed() ? 512 : 64;
    }
    packets = (len + blocksize - 1) / blocksize;

    if (!len)
    {
        DIEPTSIZ(ep) = 1 << 19;  /* one empty packet */
        DIEPDMA(ep) = 0;  /* NULL */
    }
    else
    {
        //cprintf("ep%d_send, size = 0x%x, ptr = 0x%x, packets = %d, len = %d.\n",ep, len | (packets << 19),buf,packets,len);
        DIEPTSIZ(ep) = len | (packets << 19);
        DIEPDMA(ep) = buf;
    }
    DIEPCTL(ep) |= 0x84000000;  /* epena & cnak*/

    while(GINTSTS & 0x40)
    {
        GINTSTS = 0x40;
    }

    while(1){
        //cprintf("GINTSTS: 0x%x,DIEPINT(%d):0x%x,DIEPTSIZ(%d):0x%x .\n",GINTSTS, ep,DIEPINT(ep),ep,DIEPTSIZ(ep));
        if((GINTSTS & 0x40000) && (epints = DIEPINT(ep) & 1)) {
            DIEPINT(ep) = epints;
            if (endpoints[ep].busy)
            {
                endpoints[ep].busy = 0;//false
                endpoints[ep].rc = 0;
                endpoints[ep].done = 1;//true
            }
            break;
        }
    }
    return 0;
}

static int hiusb_epx_rx(unsigned ep, void *buf, unsigned len)
{
    int blocksize,packets;
    //cprintf("ep%d rx, len = 0x%x, buf = 0x%x.\n",ep, len, buf);

    //memset(buf,0,len);   //for performance
    
    endpoints[ep].busy = 1;//true
    DOEPCTL(ep) &= ~0x00200000;  /* EPx UNSTALL */
    DOEPCTL(ep) |= 0x8000;  /* EPx OUT ACTIVE */
    blocksize = usb_drv_port_speed() ? 512 : 64;
    packets = (len + blocksize - 1) / blocksize;

#define MAX_RX_PACKET 0x3FF
    
    if(packets > MAX_RX_PACKET) /*Max recv packets is 1023*/
    {
        endpoints[ep].size = MAX_RX_PACKET*blocksize;
        packets=MAX_RX_PACKET;
        len = MAX_RX_PACKET*blocksize;
    }
    else
    {
        endpoints[ep].size = len;
    }
    
    if (!len)
    {
        //cprintf("ep%d_recv, size = 0x%x, ptr = 0x%x.\n",ep, len, buf);
        DOEPTSIZ(ep) = 1 << 19;  /* one empty packet */
        DOEPDMA(ep) = 0;//NULL  /* dummy address */
    }
    else
    {
        //cprintf("ep%d_recv, size = 0x%x, ptr = 0x%x,packets=%d.\n",ep, len, buf, packets);
        DOEPTSIZ(ep) = len | (packets << 19);
        DOEPDMA(ep) = buf;
        
    }
    DOEPCTL(ep) |= 0x84000000;  /* EPx OUT ENABLE CLEARNAK */

    return 0;
}

int usb_queue_req(struct usb_endpoint *ept, struct usb_request *req)
{    
    if(ept->in)
        hiusb_epx_tx(ept->num, req->buf, req->length);
    else
        hiusb_epx_rx(ept->num, req->buf, req->length);
    
    return 0;
}

void usb_handle_control_request(setup_packet* req)
{
    const void* addr;
    int size = -1;
    
   /*b_printf("type=%x req=%x val=%x idx=%x len=%x (%s).\n",
		req->bRequestType, 
		req->bRequest,
		req->wValue, 
		req->wIndex, 
		req->wLength, 
		reqname(req->bRequest));*/
    
    switch (req->bRequest)
    {
        case USB_REQ_GET_STATUS:
            if (req->bRequestType == USB_DIR_IN) ctrlresp[0] = 1;
            else ctrlresp[0] = 0;
            ctrlresp[1] = 0;
            addr = ctrlresp;
            size = 2;
            break;
        
        case USB_REQ_CLEAR_FEATURE:
            if (req->bRequestType == USB_RECIP_ENDPOINT && req->wValue == USB_ENDPOINT_HALT)
            usb_drv_stall(req->wIndex & 0xf, 0, req->wIndex >> 7);
            size = 0;
            break;
        
        case USB_REQ_SET_FEATURE:
            size = 0;
            break;
        
        case USB_REQ_SET_ADDRESS:
            size = 0;
            usb_drv_cancel_all_transfers();
            usb_drv_set_address(req->wValue);
            break;
        
        case USB_REQ_GET_DESCRIPTOR:
            switch (req->wValue >> 8)
            {
                case USB_DT_DEVICE:
                    addr = &device_descriptor;
                    size = sizeof(device_descriptor);
                    //USB_DBG(DBG_INFO,"Get device descriptor.\n");
                    break;
            
                case USB_DT_OTHER_SPEED_CONFIG:
                case USB_DT_CONFIG:
                    if ((req->wValue >> 8) == USB_DT_CONFIG)
                    {
                        int maxpacket = usb_drv_port_speed() ? 512 : 64;
                        config_bundle.endpoint1_descriptor.wMaxPacketSize = maxpacket;
                        config_bundle.endpoint2_descriptor.wMaxPacketSize = maxpacket;
                        config_bundle.config_descriptor.bDescriptorType = USB_DT_CONFIG;
                    }
                    else
                    {
                        int maxpacket = usb_drv_port_speed() ? 64 : 512;
                        config_bundle.endpoint1_descriptor.wMaxPacketSize = maxpacket;
                        config_bundle.endpoint2_descriptor.wMaxPacketSize = maxpacket;
                        config_bundle.config_descriptor.bDescriptorType = USB_DT_OTHER_SPEED_CONFIG;
                    }
                    addr = &config_bundle;
                    size = sizeof(config_bundle);
                    //USB_DBG(DBG_INFO,"Get config descriptor.\n");
                    break;
                
                case USB_DT_STRING:
                    switch (req->wValue & 0xff)
                    {
                    case 0:
                        addr = &lang_descriptor;
                        size = lang_descriptor.bLength;
                        break;
                    case 1:
                        addr = &string_devicename;
                        size = 14;
                        break;
                    case 2:
                        addr = &string_devicename;
                        size = string_devicename.bLength;
                        break;
                    case 3:
                        addr = &serial_string;
                        size = serial_string.bLength;
                        break;                        
                    }
                    break;
            }
            break;
        
        case USB_REQ_GET_CONFIGURATION:
            ctrlresp[0] = 1;
            addr = ctrlresp;
            size = 1;
            break;
        
        case USB_REQ_SET_CONFIGURATION:
            usb_drv_cancel_all_transfers();

            usb_drv_request_endpoint(USB_ENDPOINT_XFER_BULK, USB_DIR_OUT);
            usb_drv_request_endpoint(USB_ENDPOINT_XFER_BULK, USB_DIR_IN);
            DIEPCTL1 |= 0x10088800;            
            /* Enable interrupts on all endpoints */
            DAINTMSK = 0xFFFFFFFF;
                        
            usb_status(req->wValue? 1 : 0, usb_drv_port_speed() ? 1 : 0);        
            size = 0;
            //USB_DBG(DBG_INFO,"Set config descriptor.\n");

            break;
    }
    
    if (!size) 
    {
        usb_drv_send_nonblocking(0, 0, 0);
    }
    else if (size == -1)
    {
        usb_drv_stall(0, 1, 1);
        usb_drv_stall(0, 1, 0);
    }
    else
    {    
        usb_drv_stall(0, 0, 1);
        usb_drv_stall(0, 0, 0);
    
        usb_drv_send_nonblocking(0, addr, size > req->wLength ? req->wLength : size);
    }
}

#ifdef NANO_PHY

void ehci_write8(unsigned char regaddr, unsigned char value)
{
    GPIO1_DIR = 0xff;
    GPIO2_DIR = 0xff;
    GPIO1_DATA = 0xff;

    GPIO2_DATA = regaddr;

    /*cs low, d/c high */
    GPIO1_DATA_SEL = 0xfb;
    /*wr low */
    GPIO1_DATA_SEL = 0xeb;
    /*wr high */
    GPIO1_DATA_SEL = 0xfb;
    /*cs high */
    GPIO1_DATA_SEL = 0xff;

    GPIO2_DATA = value;
    
    /*cs low, d/c low */
    GPIO1_DATA_SEL = 0xf9;
    /*wr low */
    GPIO1_DATA_SEL = 0xe9;
    /*wr high */
    GPIO1_DATA_SEL = 0xf9;
    /*cs high */
    GPIO1_DATA_SEL = 0xff;
}

unsigned char ehci_read8(unsigned char regaddr)
{
    unsigned char value=0;

    GPIO1_DIR = 0xff;
    GPIO2_DIR = 0xff;
    GPIO1_DATA = 0xff;

    GPIO2_DATA = regaddr;
    
    /*cs low, d/c high */
    GPIO1_DATA_SEL = 0xfb;
    /*wr low */
    GPIO1_DATA_SEL = 0xeb;
    /*wr high */
    GPIO1_DATA_SEL = 0xfb;
    /*cs high */
    GPIO1_DATA_SEL = 0xff;

    GPIO2_DIR = 0x0;

    /*cs low, d/c low */
    GPIO1_DATA_SEL = 0xf9;
    /*rd low */
    GPIO1_DATA_SEL = 0xf1;
    /*rd high */
    GPIO1_DATA_SEL = 0xf9;
    value = GPIO2_DATA;
    /*cs high */
    GPIO1_DATA_SEL = 0xff;
    return value;
}
#endif

void usb_init(void)
{
	

#if 0 //def PHILIPS_PHY   //For PHILIPS_PHY only
    /*set GPIO22 direction.OUT */   
    GPIO22_DIR = 0x03;
    /*undo-reset phy*/
    GPIO22_DATA = 0x02;
    udelay(1);
#endif        

#if  0 //def NANO_PHY  //For NANO_PHY only
    /*reset phy*/
    GPIO3_DIR = 0x07; //set direction
    GPIO3_DATA_SEL = 0x3;  //set gpio_3_0,gpio_3_1,and clear gpio_3_2.
    udelay(10);//must be delay for op
    
    val = ehci_read8(0x80);//0x80:A10 Test chip, CONTROL register(USB_CFG)
    ehci_write8(0x80,val | 0xc0); //set Vbus detect form external
    
#if 0 //reserved
    /*FIXME: set sqrxtune, conference: NANO PHY datasheet,A10 datasheet*/
    val = ehci_read8(0x8b);
    ehci_write8(0x8b,0x07);
    val = ehci_read8(0x8b);
#endif
    
    /*undo-reset phy*/
    GPIO3_DATA_SEL = 0x7;//set gpio_3_0,gpio_3_1, gpio_3_2.
    udelay(1);//must be delay for op
#endif

    /*Reset usb controller.*/
    while (!(GRSTCTL & 0x80000000));  /* Wait for OTG AHB master idle */

    GRSTCTL = 1;  /* OTG: Assert Software Reset */
    while (GRSTCTL & 1);  /* Wait for OTG to ack reset */
    
    while (!(GRSTCTL & 0x80000000));  /* Wait for OTG AHB master idle */
    

    /*configure the usb*/
    usb_config();
    
}

/* IRQ handler */
void usb_poll(void)
{
    unsigned long ints = GINTSTS;
    unsigned long epints;

    /*if(ints != 0x04000020 && ints != 0x04008028&&ints != 0x04008020 && ints != 0x04000028 )
      b_printf("\n   ------------->ints =0x%x ****** **\n",ints);*/

    if (ints & 0x1000)  /* bus reset */
    {
        //USB_DBG(DBG_INFO,"reset intr\n");
        DCFG =4;/*set Non-Zero-Length status out handshake */
        reset_endpoints();  
    }
    
    if (ints & 0x2000)  /* enumeration done, we now know the speed */
    {
        //USB_DBG(DBG_INFO,"enum done intr\n");
        /* Set up the maximum packet sizes accordingly */
        unsigned long maxpacket = usb_drv_port_speed() ? 512 : 64;
        DIEPCTL1 = (DIEPCTL1 & ~0x000003FF) | maxpacket; //set it
        DOEPCTL1 =  (DOEPCTL1 & ~0x000003FF) | maxpacket;//set it
    }

    if (ints & 0x40000)  /* IN EP event */
    { 
        epints = DIEPINT0;
        DIEPINT0 = epints;        
        //USB_DBG(DBG_INFO,"IN EP event,DOEPINT0 :0x%x, DOEPINT1 :0x%x.\n",DIEPINT0, DIEPINT1);
        if (epints & 1)  /* Transfer completed */
        {
            //cprintf("USB:  TX completed.\n");
            /*FIXME,Maybe you can use bytes*/
            //int bytes = endpoints[0].size - (DIEPTSIZ(0) & 0x3FFFF); //actual transfer
            if (endpoints[0].busy)
            {
                endpoints[0].busy = 0;//false
                endpoints[0].rc = 0;
                endpoints[0].done = 1;//true
            }
        }
        if (epints & 4)  /* AHB error */
            cprintf("USB: AHB error on IN EP0.\n");

        if (epints & 8)  /* Timeout */
        {
            cprintf("USB: Timeout on IN EP0.\n");
            if (endpoints[0].busy)
            {
                endpoints[0].busy = 1;//false
                endpoints[0].rc = 1;
                endpoints[0].done = 1;//true
            }
        }
    }

    if (ints & 0x80000)  /* OUT EP event */
    {
        //USB_DBG(DBG_INFO,"OUT EP event,DOEPINT0 :0x%x, DOEPINT1 :0x%x.\n",DOEPINT0, DOEPINT1);
        if((epints = DOEPINT0) != 0)/*Notes that: Just get a value ,not compare*/
        {
            DOEPINT0 = epints;
            if (epints & 1)  /* Transfer completed */
            {
                /*FIXME,need use bytes*/
                //cprintf("USB:  EP0 RX completed.\n");
                if (endpoints[0].busy)
                {
                    endpoints[0].busy = 0;
                    endpoints[0].rc = 0;
                    endpoints[0].done = 1;
                }
            }
            if (epints & 4)  /* AHB error */
	         cprintf("\nAHB error on OUT EP0.\n");	
               // USB_DBG(DBG_INFO,"AHB error on OUT EP0.\n");
            if (epints & 8)  /* SETUP phase done */
            {
                DIEPINT0 = 0xffffffff;/*clear IN EP intr*/
                usb_handle_control_request((setup_packet *)&ctrlreq[0]);
            }
            
            /* Make sure EP0 OUT is set up to accept the next request */
            memset(&ctrlreq[0],0,3*8);
            DOEPTSIZ0 = 0x30080040;
            DOEPDMA0 = (unsigned)&ctrlreq[0]; //notes that:the compulsive conversion is expectable.
            DOEPCTL0 = 0x84000000;
        }
		
        if((epints = DOEPINT1) != 0)/*Notes that: Just get a value ,not compare*/
        {
            DOEPINT1 = epints;
            //cprintf("OUT EP: DOEPINT1 :0x%x,DOEPTSIZ1 :0x%x.\n",epints,DOEPTSIZ(1));
            if (epints & 1)  /* Transfer completed */
            {
                int bytes = endpoints[1].size - (DOEPTSIZ(1) & 0x7FFFF);
                if (endpoints[1].busy)
                {
                    endpoints[1].busy = 0;
                    endpoints[1].rc = 0;
                    endpoints[1].done = 1;
//                    rx_req->complete(bytes, 0);
                }
            }
            
            if (epints & 4)  /* AHB error */
	         cprintf("\nAHB error on OUT EP1.\n");	
                //USB_DBG(DBG_INFO,"AHB error on OUT EP1.\n");
            if (epints & 8)  /* SETUP phase done */
	         cprintf("\nSETUP phase done  on OUT EP1.\n");	
                //USB_DBG(DBG_INFO,"SETUP phase done  on OUT EP1.\n");
            
        }
        
    }
    
    GINTSTS = ints;  //write clear ints
}

void usb_shutdown(void)
{
        /* disable pullup */
}