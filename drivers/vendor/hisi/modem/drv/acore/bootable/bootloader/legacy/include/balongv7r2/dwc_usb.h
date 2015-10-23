/*
 * Copyright (c) 2010, HiSilicon Technologies Co., Ltd.
 * All rights reserved.
 */

#if 0

#ifndef __HIUSB_H_
#define __HIUSB_H_

//#define NANO_PHY
//#define PHILIPS_PHY

#define DWC_OTG_BASE		        0x82080000  //0xFE600000  /*FIXME*/
#define GPIO22_BASE                      0xFE105000
#define USB_NUM_ENDPOINTS          2
#define MAX_EPS_CHANNELS 16

#define BULK_OUT_EP 1
#define BULK_IN_EP 1

#define RX_REQ_LEN 512
#define MAX_PACKET_LEN 512

#define USB_DBG 
#define DBG_ERR		0x0001	/* report all error returns */
#define DBG_WARNING		0x0002	/* warning info*/
#define DBG_INFO		0x0004	/* debug info */
#define DBG_ALL		0xffff
#define DBG_NONE	0x0000

#define MHZ (1000*1000)
#define CFG_CLK_CPU_BASE 40//FIXME??FPGA???¡§??CPU??????90MHZ??
#define CFG_CLK_OTHER_BASE 26//FIXME
#define CFG_CLK_CPU (CFG_CLK_CPU_BASE*MHZ)
#define CONFIG_HINAND_OOB_SIZE 64

#if 1
typedef struct 
{
    unsigned char bRequestType;
    unsigned char bRequest;
    unsigned short wValue;
    unsigned short wIndex;
    unsigned short wLength;
} setup_packet;
#endif

struct ept_queue_item
{
    unsigned next;
    unsigned info;
};

struct usb_request
{
    struct ept_queue_item *item;

    void *buf;
    unsigned length;
    
    void (*complete)(unsigned actual, int status);
    void *context;
};


/*DWC_OTG regsiter descriptor*/
/*Global CSR MAP*/
#define GLOBAL_CSR_BASE		(DWC_OTG_BASE)
/*Device mode CSR MAP*/
#define DEVICE_CSR_BASE		(DWC_OTG_BASE+0x800)
/*Device mode CSR MAP*/
#define DEVICE_INEP_BASE		(DWC_OTG_BASE+0x900)
/*Device mode CSR MAP*/
#define DEVICE_OUTEP_BASE		(DWC_OTG_BASE+0xB00)

#ifdef PHILIPS_PHY
#define GPIO22_DATA     (*((unsigned volatile*)(GPIO22_BASE + 0x3FC)))
#define GPIO22_DIR     (*((unsigned volatile*)(GPIO22_BASE + 0x400)))
#endif
#ifdef NANO_PHY
#define GPIO0_BASE                      0xFE105000 
#define GPIO1_BASE                      0xFE107000
#define GPIO2_BASE                      0xFE108000
#define GPIO3_BASE                      0xFE109000

#define GPIO0_DATA     (*((unsigned volatile*)(GPIO0_BASE + 0x3FC)))
#define GPIO0_DIR     (*((unsigned volatile*)(GPIO0_BASE + 0x400)))
#define GPIO1_DATA     (*((unsigned volatile*)(GPIO1_BASE + 0x3FC)))
#define GPIO1_DATA_SEL     (*((unsigned volatile*)(GPIO1_BASE + 0x78))) //just op bit[1:4]
#define GPIO1_DIR     (*((unsigned volatile*)(GPIO1_BASE + 0x400)))
#define GPIO2_DATA     (*((unsigned volatile*)(GPIO2_BASE + 0x3FC)))
#define GPIO2_DIR     (*((unsigned volatile*)(GPIO2_BASE + 0x400)))
#define GPIO3_DATA_SEL     (*((unsigned volatile*)(GPIO3_BASE + 0x1C)))//just op bit[0:2]
#define GPIO3_DATA     (*((unsigned volatile*)(GPIO3_BASE + 0x3FC)))
#define GPIO3_DIR     (*((unsigned volatile*)(GPIO3_BASE + 0x400)))
#endif

/*** OTG LINK CORE REGISTERS ***/
/* Core Global Registers */
#define GOTGCTL     (*((unsigned volatile*)(DWC_OTG_BASE + 0x000)))
#define GOTGINT     (*((unsigned volatile*)(DWC_OTG_BASE + 0x004)))
#define GAHBCFG     (*((unsigned volatile*)(DWC_OTG_BASE + 0x008)))
#define GUSBCFG     (*((unsigned volatile*)(DWC_OTG_BASE + 0x00C)))
#define GRSTCTL     (*((unsigned volatile*)(DWC_OTG_BASE + 0x010)))
#define GINTSTS     (*((unsigned volatile*)(DWC_OTG_BASE + 0x014)))
#define GINTMSK     (*((unsigned volatile*)(DWC_OTG_BASE + 0x018)))
#define GRXSTSR     (*((unsigned volatile*)(DWC_OTG_BASE + 0x01C)))
#define GRXSTSP     (*((unsigned volatile*)(DWC_OTG_BASE + 0x020)))
#define GRXFSIZ     (*((unsigned volatile*)(DWC_OTG_BASE + 0x024)))
#define GNPTXFSIZ   (*((unsigned volatile*)(DWC_OTG_BASE + 0x028)))
#define GNPTXSTS    (*((unsigned volatile*)(DWC_OTG_BASE + 0x02C)))

#define GHWCFG1     (*((unsigned volatile*)(DWC_OTG_BASE + 0x044)))
#define GHWCFG2     (*((unsigned volatile*)(DWC_OTG_BASE + 0x048)))
#define GHWCFG3     (*((unsigned volatile*)(DWC_OTG_BASE + 0x04c)))
#define GHWCFG4     (*((unsigned volatile*)(DWC_OTG_BASE + 0x050)))
#define GLPMCFG     (*((unsigned volatile*)(DWC_OTG_BASE + 0x054)))

#define GDFIFOCFG     (*((unsigned volatile*)(DWC_OTG_BASE + 0x05c)))

#define HPTXFSIZ    (*((unsigned volatile*)(DWC_OTG_BASE + 0x100)))
#define DIEPTXF(x) (*((unsigned volatile*)(DWC_OTG_BASE + 0x100 + 4 * (x))))
#define DIEPTXF1   (*((unsigned volatile*)(DWC_OTG_BASE + 0x104)))
#define DIEPTXF2   (*((unsigned volatile*)(DWC_OTG_BASE + 0x108)))
#define DIEPTXF3   (*((unsigned volatile*)(DWC_OTG_BASE + 0x10C)))
#define DIEPTXF4   (*((unsigned volatile*)(DWC_OTG_BASE + 0x110)))
#define DIEPTXF5   (*((unsigned volatile*)(DWC_OTG_BASE + 0x114)))
#define DIEPTXF6   (*((unsigned volatile*)(DWC_OTG_BASE + 0x118)))
#define DIEPTXF7   (*((unsigned volatile*)(DWC_OTG_BASE + 0x11C)))
#define DIEPTXF8   (*((unsigned volatile*)(DWC_OTG_BASE + 0x120)))
#define DIEPTXF9   (*((unsigned volatile*)(DWC_OTG_BASE + 0x124)))
#define DIEPTXF10  (*((unsigned volatile*)(DWC_OTG_BASE + 0x128)))
#define DIEPTXF11  (*((unsigned volatile*)(DWC_OTG_BASE + 0x12C)))
#define DIEPTXF12  (*((unsigned volatile*)(DWC_OTG_BASE + 0x130)))
#define DIEPTXF13  (*((unsigned volatile*)(DWC_OTG_BASE + 0x134)))
#define DIEPTXF14  (*((unsigned volatile*)(DWC_OTG_BASE + 0x138)))
#define DIEPTXF15  (*((unsigned volatile*)(DWC_OTG_BASE + 0x13C)))

/*** HOST MODE REGISTERS ***/
/* Host Global Registers */
#define HCFG        (*((unsigned volatile*)(DWC_OTG_BASE + 0x400)))
#define HFIR        (*((unsigned volatile*)(DWC_OTG_BASE + 0x404)))
#define HFNUM       (*((unsigned volatile*)(DWC_OTG_BASE + 0x408)))
#define HPTXSTS     (*((unsigned volatile*)(DWC_OTG_BASE + 0x410)))
#define HAINT       (*((unsigned volatile*)(DWC_OTG_BASE + 0x414)))
#define HAINTMSK    (*((unsigned volatile*)(DWC_OTG_BASE + 0x418)))

/* Host Port Control and Status Registers */
#define HPRT        (*((unsigned volatile*)(DWC_OTG_BASE + 0x440)))

/* Host Channel-Specific Registers */
#define HCCHAR(x)   (*((unsigned volatile*)(DWC_OTG_BASE + 0x500 + 0x20 * (x))))
#define HCSPLT(x)   (*((unsigned volatile*)(DWC_OTG_BASE + 0x504 + 0x20 * (x))))
#define HCINT(x)    (*((unsigned volatile*)(DWC_OTG_BASE + 0x508 + 0x20 * (x))))
#define HCINTMSK(x) (*((unsigned volatile*)(DWC_OTG_BASE + 0x50C + 0x20 * (x))))
#define HCTSIZ(x)   (*((unsigned volatile*)(DWC_OTG_BASE + 0x510 + 0x20 * (x))))
#define HCDMA(x)    (*((unsigned volatile*)(DWC_OTG_BASE + 0x514 + 0x20 * (x))))
#define HCCHAR0     (*((unsigned volatile*)(DWC_OTG_BASE + 0x500)))
#define HCSPLT0     (*((unsigned volatile*)(DWC_OTG_BASE + 0x504)))
#define HCINT0      (*((unsigned volatile*)(DWC_OTG_BASE + 0x508)))
#define HCINTMSK0   (*((unsigned volatile*)(DWC_OTG_BASE + 0x50C)))
#define HCTSIZ0     (*((unsigned volatile*)(DWC_OTG_BASE + 0x510)))
#define HCDMA0      (*((unsigned volatile*)(DWC_OTG_BASE + 0x514)))
#define HCCHAR1     (*((unsigned volatile*)(DWC_OTG_BASE + 0x520)))
#define HCSPLT1     (*((unsigned volatile*)(DWC_OTG_BASE + 0x524)))
#define HCINT1      (*((unsigned volatile*)(DWC_OTG_BASE + 0x528)))
#define HCINTMSK1   (*((unsigned volatile*)(DWC_OTG_BASE + 0x52C)))
#define HCTSIZ1     (*((unsigned volatile*)(DWC_OTG_BASE + 0x530)))
#define HCDMA1      (*((unsigned volatile*)(DWC_OTG_BASE + 0x534)))
#define HCCHAR2     (*((unsigned volatile*)(DWC_OTG_BASE + 0x540)))
#define HCSPLT2     (*((unsigned volatile*)(DWC_OTG_BASE + 0x544)))
#define HCINT2      (*((unsigned volatile*)(DWC_OTG_BASE + 0x548)))
#define HCINTMSK2   (*((unsigned volatile*)(DWC_OTG_BASE + 0x54C)))
#define HCTSIZ2     (*((unsigned volatile*)(DWC_OTG_BASE + 0x550)))
#define HCDMA2      (*((unsigned volatile*)(DWC_OTG_BASE + 0x554)))
#define HCCHAR3     (*((unsigned volatile*)(DWC_OTG_BASE + 0x560)))
#define HCSPLT3     (*((unsigned volatile*)(DWC_OTG_BASE + 0x564)))
#define HCINT3      (*((unsigned volatile*)(DWC_OTG_BASE + 0x568)))
#define HCINTMSK3   (*((unsigned volatile*)(DWC_OTG_BASE + 0x56C)))
#define HCTSIZ3     (*((unsigned volatile*)(DWC_OTG_BASE + 0x570)))
#define HCDMA3      (*((unsigned volatile*)(DWC_OTG_BASE + 0x574)))
#define HCCHAR4     (*((unsigned volatile*)(DWC_OTG_BASE + 0x580)))
#define HCSPLT4     (*((unsigned volatile*)(DWC_OTG_BASE + 0x584)))
#define HCINT4      (*((unsigned volatile*)(DWC_OTG_BASE + 0x588)))
#define HCINTMSK4   (*((unsigned volatile*)(DWC_OTG_BASE + 0x58C)))
#define HCTSIZ4     (*((unsigned volatile*)(DWC_OTG_BASE + 0x590)))
#define HCDMA4      (*((unsigned volatile*)(DWC_OTG_BASE + 0x594)))
#define HCCHAR5     (*((unsigned volatile*)(DWC_OTG_BASE + 0x5A0)))
#define HCSPLT5     (*((unsigned volatile*)(DWC_OTG_BASE + 0x5A4)))
#define HCINT5      (*((unsigned volatile*)(DWC_OTG_BASE + 0x5A8)))
#define HCINTMSK5   (*((unsigned volatile*)(DWC_OTG_BASE + 0x5AC)))
#define HCTSIZ5     (*((unsigned volatile*)(DWC_OTG_BASE + 0x5B0)))
#define HCDMA5      (*((unsigned volatile*)(DWC_OTG_BASE + 0x5B4)))
#define HCCHAR6     (*((unsigned volatile*)(DWC_OTG_BASE + 0x5C0)))
#define HCSPLT6     (*((unsigned volatile*)(DWC_OTG_BASE + 0x5C4)))
#define HCINT6      (*((unsigned volatile*)(DWC_OTG_BASE + 0x5C8)))
#define HCINTMSK6   (*((unsigned volatile*)(DWC_OTG_BASE + 0x5CC)))
#define HCTSIZ6     (*((unsigned volatile*)(DWC_OTG_BASE + 0x5D0)))
#define HCDMA6      (*((unsigned volatile*)(DWC_OTG_BASE + 0x5D4)))
#define HCCHAR7     (*((unsigned volatile*)(DWC_OTG_BASE + 0x5E0)))
#define HCSPLT7     (*((unsigned volatile*)(DWC_OTG_BASE + 0x5E4)))
#define HCINT7      (*((unsigned volatile*)(DWC_OTG_BASE + 0x5E8)))
#define HCINTMSK7   (*((unsigned volatile*)(DWC_OTG_BASE + 0x5EC)))
#define HCTSIZ7     (*((unsigned volatile*)(DWC_OTG_BASE + 0x5F0)))
#define HCDMA7      (*((unsigned volatile*)(DWC_OTG_BASE + 0x5F4)))
#define HCCHAR8     (*((unsigned volatile*)(DWC_OTG_BASE + 0x600)))
#define HCSPLT8     (*((unsigned volatile*)(DWC_OTG_BASE + 0x604)))
#define HCINT8      (*((unsigned volatile*)(DWC_OTG_BASE + 0x608)))
#define HCINTMSK8   (*((unsigned volatile*)(DWC_OTG_BASE + 0x60C)))
#define HCTSIZ8     (*((unsigned volatile*)(DWC_OTG_BASE + 0x610)))
#define HCDMA8      (*((unsigned volatile*)(DWC_OTG_BASE + 0x614)))
#define HCCHAR9     (*((unsigned volatile*)(DWC_OTG_BASE + 0x620)))
#define HCSPLT9     (*((unsigned volatile*)(DWC_OTG_BASE + 0x624)))
#define HCINT9      (*((unsigned volatile*)(DWC_OTG_BASE + 0x628)))
#define HCINTMSK9   (*((unsigned volatile*)(DWC_OTG_BASE + 0x62C)))
#define HCTSIZ9     (*((unsigned volatile*)(DWC_OTG_BASE + 0x630)))
#define HCDMA9      (*((unsigned volatile*)(DWC_OTG_BASE + 0x634)))
#define HCCHAR10    (*((unsigned volatile*)(DWC_OTG_BASE + 0x640)))
#define HCSPLT10    (*((unsigned volatile*)(DWC_OTG_BASE + 0x644)))
#define HCINT10     (*((unsigned volatile*)(DWC_OTG_BASE + 0x648)))
#define HCINTMSK10  (*((unsigned volatile*)(DWC_OTG_BASE + 0x64C)))
#define HCTSIZ10    (*((unsigned volatile*)(DWC_OTG_BASE + 0x650)))
#define HCDMA10     (*((unsigned volatile*)(DWC_OTG_BASE + 0x654)))
#define HCCHAR11    (*((unsigned volatile*)(DWC_OTG_BASE + 0x660)))
#define HCSPLT11    (*((unsigned volatile*)(DWC_OTG_BASE + 0x664)))
#define HCINT11     (*((unsigned volatile*)(DWC_OTG_BASE + 0x668)))
#define HCINTMSK11  (*((unsigned volatile*)(DWC_OTG_BASE + 0x66C)))
#define HCTSIZ11    (*((unsigned volatile*)(DWC_OTG_BASE + 0x670)))
#define HCDMA11     (*((unsigned volatile*)(DWC_OTG_BASE + 0x674)))
#define HCCHAR12    (*((unsigned volatile*)(DWC_OTG_BASE + 0x680)))
#define HCSPLT12    (*((unsigned volatile*)(DWC_OTG_BASE + 0x684)))
#define HCINT12     (*((unsigned volatile*)(DWC_OTG_BASE + 0x688)))
#define HCINTMSK12  (*((unsigned volatile*)(DWC_OTG_BASE + 0x68C)))
#define HCTSIZ12    (*((unsigned volatile*)(DWC_OTG_BASE + 0x690)))
#define HCDMA12     (*((unsigned volatile*)(DWC_OTG_BASE + 0x694)))
#define HCCHAR13    (*((unsigned volatile*)(DWC_OTG_BASE + 0x6A0)))
#define HCSPLT13    (*((unsigned volatile*)(DWC_OTG_BASE + 0x6A4)))
#define HCINT13     (*((unsigned volatile*)(DWC_OTG_BASE + 0x6A8)))
#define HCINTMSK13  (*((unsigned volatile*)(DWC_OTG_BASE + 0x6AC)))
#define HCTSIZ13    (*((unsigned volatile*)(DWC_OTG_BASE + 0x6B0)))
#define HCDMA13     (*((unsigned volatile*)(DWC_OTG_BASE + 0x6B4)))
#define HCCHAR14    (*((unsigned volatile*)(DWC_OTG_BASE + 0x6C0)))
#define HCSPLT14    (*((unsigned volatile*)(DWC_OTG_BASE + 0x6C4)))
#define HCINT14     (*((unsigned volatile*)(DWC_OTG_BASE + 0x6C8)))
#define HCINTMSK14  (*((unsigned volatile*)(DWC_OTG_BASE + 0x6CC)))
#define HCTSIZ14    (*((unsigned volatile*)(DWC_OTG_BASE + 0x6D0)))
#define HCDMA14     (*((unsigned volatile*)(DWC_OTG_BASE + 0x6D4)))
#define HCCHAR15    (*((unsigned volatile*)(DWC_OTG_BASE + 0x6E0)))
#define HCSPLT15    (*((unsigned volatile*)(DWC_OTG_BASE + 0x6E4)))
#define HCINT15     (*((unsigned volatile*)(DWC_OTG_BASE + 0x6E8)))
#define HCINTMSK15  (*((unsigned volatile*)(DWC_OTG_BASE + 0x6EC)))
#define HCTSIZ15    (*((unsigned volatile*)(DWC_OTG_BASE + 0x6F0)))
#define HCDMA15     (*((unsigned volatile*)(DWC_OTG_BASE + 0x6F4)))

/*** DEVICE MODE REGISTERS ***/
/* Device Global Registers */
#define DCFG        (*((unsigned volatile*)(DWC_OTG_BASE + 0x800)))
#define DCTL        (*((unsigned volatile*)(DWC_OTG_BASE + 0x804)))
#define DSTS        (*((unsigned volatile*)(DWC_OTG_BASE + 0x808)))
#define DIEPMSK     (*((unsigned volatile*)(DWC_OTG_BASE + 0x810)))
#define DOEPMSK     (*((unsigned volatile*)(DWC_OTG_BASE + 0x814)))
#define DAINT       (*((unsigned volatile*)(DWC_OTG_BASE + 0x818)))
#define DAINTMSK    (*((unsigned volatile*)(DWC_OTG_BASE + 0x81C)))
#define DTKNQR1     (*((unsigned volatile*)(DWC_OTG_BASE + 0x820)))
#define DTKNQR2     (*((unsigned volatile*)(DWC_OTG_BASE + 0x824)))
#define DVBUSDIS    (*((unsigned volatile*)(DWC_OTG_BASE + 0x828)))
#define DVBUSPULSE  (*((unsigned volatile*)(DWC_OTG_BASE + 0x82C)))
#define DTHRCTL     (*((unsigned volatile*)(DWC_OTG_BASE + 0x830)))

/* Device Logical IN Endpoint-Specific Registers */
#define DIEPCTL(x)  (*((unsigned volatile*)(DWC_OTG_BASE + 0x900 + 0x20 * (x))))
#define DIEPINT(x)  (*((unsigned volatile*)(DWC_OTG_BASE + 0x908 + 0x20 * (x))))
#define DIEPTSIZ(x) (*((unsigned volatile*)(DWC_OTG_BASE + 0x910 + 0x20 * (x))))
#define DIEPDMA(x)  (*((const void* volatile*)(DWC_OTG_BASE + 0x914 + 0x20 * (x))))
#define DIEPCTL0    (*((unsigned volatile*)(DWC_OTG_BASE + 0x900)))
#define DIEPINT0    (*((unsigned volatile*)(DWC_OTG_BASE + 0x908)))
#define DIEPTSIZ0   (*((unsigned volatile*)(DWC_OTG_BASE + 0x910)))
#define DIEPDMA0    (*((const void* volatile*)(DWC_OTG_BASE + 0x914)))
#define DIEPCTL1    (*((unsigned volatile*)(DWC_OTG_BASE + 0x920)))
#define DIEPINT1    (*((unsigned volatile*)(DWC_OTG_BASE + 0x928)))
#define DIEPTSIZ1   (*((unsigned volatile*)(DWC_OTG_BASE + 0x930)))
#define DIEPDMA1    (*((const void* volatile*)(DWC_OTG_BASE + 0x934)))
#define DIEPCTL2    (*((unsigned volatile*)(DWC_OTG_BASE + 0x940)))
#define DIEPINT2    (*((unsigned volatile*)(DWC_OTG_BASE + 0x948)))
#define DIEPTSIZ2   (*((unsigned volatile*)(DWC_OTG_BASE + 0x950)))
#define DIEPDMA2    (*((const void* volatile*)(DWC_OTG_BASE + 0x954)))
#define DIEPCTL3    (*((unsigned volatile*)(DWC_OTG_BASE + 0x960)))
#define DIEPINT3    (*((unsigned volatile*)(DWC_OTG_BASE + 0x968)))
#define DIEPTSIZ3   (*((unsigned volatile*)(DWC_OTG_BASE + 0x970)))
#define DIEPDMA3    (*((const void* volatile*)(DWC_OTG_BASE + 0x974)))
#define DIEPCTL4    (*((unsigned volatile*)(DWC_OTG_BASE + 0x980)))
#define DIEPINT4    (*((unsigned volatile*)(DWC_OTG_BASE + 0x988)))
#define DIEPTSIZ4   (*((unsigned volatile*)(DWC_OTG_BASE + 0x990)))
#define DIEPDMA4    (*((const void* volatile*)(DWC_OTG_BASE + 0x994)))
#define DIEPCTL5    (*((unsigned volatile*)(DWC_OTG_BASE + 0x9A0)))
#define DIEPINT5    (*((unsigned volatile*)(DWC_OTG_BASE + 0x9A8)))
#define DIEPTSIZ5   (*((unsigned volatile*)(DWC_OTG_BASE + 0x9B0)))
#define DIEPDMA5    (*((const void* volatile*)(DWC_OTG_BASE + 0x9B4)))
#define DIEPCTL6    (*((unsigned volatile*)(DWC_OTG_BASE + 0x9C0)))
#define DIEPINT6    (*((unsigned volatile*)(DWC_OTG_BASE + 0x9C8)))
#define DIEPTSIZ6   (*((unsigned volatile*)(DWC_OTG_BASE + 0x9D0)))
#define DIEPDMA6    (*((const void* volatile*)(DWC_OTG_BASE + 0x9D4)))
#define DIEPCTL7    (*((unsigned volatile*)(DWC_OTG_BASE + 0x9E0)))
#define DIEPINT7    (*((unsigned volatile*)(DWC_OTG_BASE + 0x9E8)))
#define DIEPTSIZ7   (*((unsigned volatile*)(DWC_OTG_BASE + 0x9F0)))
#define DIEPDMA7    (*((const void* volatile*)(DWC_OTG_BASE + 0x9F4)))
#define DIEPCTL8    (*((unsigned volatile*)(DWC_OTG_BASE + 0xA00)))
#define DIEPINT8    (*((unsigned volatile*)(DWC_OTG_BASE + 0xA08)))
#define DIEPTSIZ8   (*((unsigned volatile*)(DWC_OTG_BASE + 0xA10)))
#define DIEPDMA8    (*((const void* volatile*)(DWC_OTG_BASE + 0xA14)))
#define DIEPCTL9    (*((unsigned volatile*)(DWC_OTG_BASE + 0xA20)))
#define DIEPINT9    (*((unsigned volatile*)(DWC_OTG_BASE + 0xA28)))
#define DIEPTSIZ9   (*((unsigned volatile*)(DWC_OTG_BASE + 0xA30)))
#define DIEPDMA9    (*((const void* volatile*)(DWC_OTG_BASE + 0xA34)))
#define DIEPCTL10   (*((unsigned volatile*)(DWC_OTG_BASE + 0xA40)))
#define DIEPINT10   (*((unsigned volatile*)(DWC_OTG_BASE + 0xA48)))
#define DIEPTSIZ10  (*((unsigned volatile*)(DWC_OTG_BASE + 0xA50)))
#define DIEPDMA10   (*((const void* volatile*)(DWC_OTG_BASE + 0xA54)))
#define DIEPCTL11   (*((unsigned volatile*)(DWC_OTG_BASE + 0xA60)))
#define DIEPINT11   (*((unsigned volatile*)(DWC_OTG_BASE + 0xA68)))
#define DIEPTSIZ11  (*((unsigned volatile*)(DWC_OTG_BASE + 0xA70)))
#define DIEPDMA11   (*((const void* volatile*)(DWC_OTG_BASE + 0xA74)))
#define DIEPCTL12   (*((unsigned volatile*)(DWC_OTG_BASE + 0xA80)))
#define DIEPINT12   (*((unsigned volatile*)(DWC_OTG_BASE + 0xA88)))
#define DIEPTSIZ12  (*((unsigned volatile*)(DWC_OTG_BASE + 0xA90)))
#define DIEPDMA12   (*((const void* volatile*)(DWC_OTG_BASE + 0xA94)))
#define DIEPCTL13   (*((unsigned volatile*)(DWC_OTG_BASE + 0xAA0)))
#define DIEPINT13   (*((unsigned volatile*)(DWC_OTG_BASE + 0xAA8)))
#define DIEPTSIZ13  (*((unsigned volatile*)(DWC_OTG_BASE + 0xAB0)))
#define DIEPDMA13   (*((const void* volatile*)(DWC_OTG_BASE + 0xAB4)))
#define DIEPCTL14   (*((unsigned volatile*)(DWC_OTG_BASE + 0xAC0)))
#define DIEPINT14   (*((unsigned volatile*)(DWC_OTG_BASE + 0xAC8)))
#define DIEPTSIZ14  (*((unsigned volatile*)(DWC_OTG_BASE + 0xAD0)))
#define DIEPDMA14   (*((const void* volatile*)(DWC_OTG_BASE + 0xAD4)))
#define DIEPCTL15   (*((unsigned volatile*)(DWC_OTG_BASE + 0xAE0)))
#define DIEPINT15   (*((unsigned volatile*)(DWC_OTG_BASE + 0xAE8)))
#define DIEPTSIZ15  (*((unsigned volatile*)(DWC_OTG_BASE + 0xAF0)))
#define DIEPDMA15   (*((const void* volatile*)(DWC_OTG_BASE + 0xAF4)))

/* Device Logical OUT Endpoint-Specific Registers */
#define DOEPCTL(x)  (*((unsigned volatile*)(DWC_OTG_BASE + 0xB00 + 0x20 * (x))))
#define DOEPINT(x)  (*((unsigned volatile*)(DWC_OTG_BASE + 0xB08 + 0x20 * (x))))
#define DOEPTSIZ(x) (*((unsigned volatile*)(DWC_OTG_BASE + 0xB10 + 0x20 * (x))))
#define DOEPDMA(x)  (*((void* volatile*)(DWC_OTG_BASE + 0xB14 + 0x20 * (x))))
#define DOEPCTL0    (*((unsigned volatile*)(DWC_OTG_BASE + 0xB00)))
#define DOEPINT0    (*((unsigned volatile*)(DWC_OTG_BASE + 0xB08)))
#define DOEPTSIZ0   (*((unsigned volatile*)(DWC_OTG_BASE + 0xB10)))
#define DOEPDMA0    (*((void* volatile*)(DWC_OTG_BASE + 0xB14)))
#define DOEPCTL1    (*((unsigned volatile*)(DWC_OTG_BASE + 0xB20)))
#define DOEPINT1    (*((unsigned volatile*)(DWC_OTG_BASE + 0xB28)))
#define DOEPTSIZ1   (*((unsigned volatile*)(DWC_OTG_BASE + 0xB30)))
#define DOEPDMA1    (*((void* volatile*)(DWC_OTG_BASE + 0xB34)))
#define DOEPCTL2    (*((unsigned volatile*)(DWC_OTG_BASE + 0xB40)))
#define DOEPINT2    (*((unsigned volatile*)(DWC_OTG_BASE + 0xB48)))
#define DOEPTSIZ2   (*((unsigned volatile*)(DWC_OTG_BASE + 0xB50)))
#define DOEPDMA2    (*((void* volatile*)(DWC_OTG_BASE + 0xB54)))
#define DOEPCTL3    (*((unsigned volatile*)(DWC_OTG_BASE + 0xB60)))
#define DOEPINT3    (*((unsigned volatile*)(DWC_OTG_BASE + 0xB68)))
#define DOEPTSIZ3   (*((unsigned volatile*)(DWC_OTG_BASE + 0xB70)))
#define DOEPDMA3    (*((void* volatile*)(DWC_OTG_BASE + 0xB74)))
#define DOEPCTL4    (*((unsigned volatile*)(DWC_OTG_BASE + 0xB80)))
#define DOEPINT4    (*((unsigned volatile*)(DWC_OTG_BASE + 0xB88)))
#define DOEPTSIZ4   (*((unsigned volatile*)(DWC_OTG_BASE + 0xB90)))
#define DOEPDMA4    (*((void* volatile*)(DWC_OTG_BASE + 0xB94)))
#define DOEPCTL5    (*((unsigned volatile*)(DWC_OTG_BASE + 0xBA0)))
#define DOEPINT5    (*((unsigned volatile*)(DWC_OTG_BASE + 0xBA8)))
#define DOEPTSIZ5   (*((unsigned volatile*)(DWC_OTG_BASE + 0xBB0)))
#define DOEPDMA5    (*((void* volatile*)(DWC_OTG_BASE + 0xBB4)))
#define DOEPCTL6    (*((unsigned volatile*)(DWC_OTG_BASE + 0xBC0)))
#define DOEPINT6    (*((unsigned volatile*)(DWC_OTG_BASE + 0xBC8)))
#define DOEPTSIZ6   (*((unsigned volatile*)(DWC_OTG_BASE + 0xBD0)))
#define DOEPDMA6    (*((void* volatile*)(DWC_OTG_BASE + 0xBD4)))
#define DOEPCTL7    (*((unsigned volatile*)(DWC_OTG_BASE + 0xBE0)))
#define DOEPINT7    (*((unsigned volatile*)(DWC_OTG_BASE + 0xBE8)))
#define DOEPTSIZ7   (*((unsigned volatile*)(DWC_OTG_BASE + 0xBF0)))
#define DOEPDMA7    (*((void* volatile*)(DWC_OTG_BASE + 0xBF4)))
#define DOEPCTL8    (*((unsigned volatile*)(DWC_OTG_BASE + 0xC00)))
#define DOEPINT8    (*((unsigned volatile*)(DWC_OTG_BASE + 0xC08)))
#define DOEPTSIZ8   (*((unsigned volatile*)(DWC_OTG_BASE + 0xC10)))
#define DOEPDMA8    (*((void* volatile*)(DWC_OTG_BASE + 0xC14)))
#define DOEPCTL9    (*((unsigned volatile*)(DWC_OTG_BASE + 0xC20)))
#define DOEPINT9    (*((unsigned volatile*)(DWC_OTG_BASE + 0xC28)))
#define DOEPTSIZ9   (*((unsigned volatile*)(DWC_OTG_BASE + 0xC30)))
#define DOEPDMA9    (*((void* volatile*)(DWC_OTG_BASE + 0xC34)))
#define DOEPCTL10   (*((unsigned volatile*)(DWC_OTG_BASE + 0xC40)))
#define DOEPINT10   (*((unsigned volatile*)(DWC_OTG_BASE + 0xC48)))
#define DOEPTSIZ10  (*((unsigned volatile*)(DWC_OTG_BASE + 0xC50)))
#define DOEPDMA10   (*((void* volatile*)(DWC_OTG_BASE + 0xC54)))
#define DOEPCTL11   (*((unsigned volatile*)(DWC_OTG_BASE + 0xC60)))
#define DOEPINT11   (*((unsigned volatile*)(DWC_OTG_BASE + 0xC68)))
#define DOEPTSIZ11  (*((unsigned volatile*)(DWC_OTG_BASE + 0xC70)))
#define DOEPDMA11   (*((void* volatile*)(DWC_OTG_BASE + 0xC74)))
#define DOEPCTL12   (*((unsigned volatile*)(DWC_OTG_BASE + 0xC80)))
#define DOEPINT12   (*((unsigned volatile*)(DWC_OTG_BASE + 0xC88)))
#define DOEPTSIZ12  (*((unsigned volatile*)(DWC_OTG_BASE + 0xC90)))
#define DOEPDMA12   (*((void* volatile*)(DWC_OTG_BASE + 0xC94)))
#define DOEPCTL13   (*((unsigned volatile*)(DWC_OTG_BASE + 0xCA0)))
#define DOEPINT13   (*((unsigned volatile*)(DWC_OTG_BASE + 0xCA8)))
#define DOEPTSIZ13  (*((unsigned volatile*)(DWC_OTG_BASE + 0xCB0)))
#define DOEPDMA13   (*((void* volatile*)(DWC_OTG_BASE + 0xCB4)))
#define DOEPCTL14   (*((unsigned volatile*)(DWC_OTG_BASE + 0xCC0)))
#define DOEPINT14   (*((unsigned volatile*)(DWC_OTG_BASE + 0xCC8)))
#define DOEPTSIZ14  (*((unsigned volatile*)(DWC_OTG_BASE + 0xCD0)))
#define DOEPDMA14   (*((void* volatile*)(DWC_OTG_BASE + 0xCD4)))
#define DOEPCTL15   (*((unsigned volatile*)(DWC_OTG_BASE + 0xCE0)))
#define DOEPINT15   (*((unsigned volatile*)(DWC_OTG_BASE + 0xCE8)))
#define DOEPTSIZ15  (*((unsigned volatile*)(DWC_OTG_BASE + 0xCF0)))
#define DOEPDMA15   (*((void* volatile*)(DWC_OTG_BASE + 0xCF4)))

/* Power and Clock Gating Register */
#define PCGCCTL     (*((unsigned volatile*)(DWC_OTG_BASE + 0xE00)))

/*
 * USB directions
 *
 * This bit flag is used in endpoint descriptors' bEndpointAddress field.
 * It's also one of three fields in control requests bRequestType.
 */
#define USB_DIR_OUT                     0               /* to device */
#define USB_DIR_IN                      0x80            /* to host */

/*
 * Descriptor types ... USB 2.0 spec table 9.5
 */
#define USB_DT_DEVICE                   0x01
#define USB_DT_CONFIG                   0x02
#define USB_DT_STRING                   0x03
#define USB_DT_INTERFACE                0x04
#define USB_DT_ENDPOINT                 0x05
#define USB_DT_DEVICE_QUALIFIER         0x06
#define USB_DT_OTHER_SPEED_CONFIG       0x07
#define USB_DT_INTERFACE_POWER          0x08
/* these are from a minor usb 2.0 revision (ECN) */
#define USB_DT_OTG                      0x09
#define USB_DT_DEBUG                    0x0a
#define USB_DT_INTERFACE_ASSOCIATION    0x0b
/* these are from the Wireless USB spec */
#define USB_DT_SECURITY                 0x0c
#define USB_DT_KEY                      0x0d
#define USB_DT_ENCRYPTION_TYPE          0x0e
#define USB_DT_BOS                      0x0f
#define USB_DT_DEVICE_CAPABILITY        0x10
#define USB_DT_WIRELESS_ENDPOINT_COMP   0x11
#define USB_DT_WIRE_ADAPTER             0x21
#define USB_DT_RPIPE                    0x22
#define USB_DT_CS_RADIO_CONTROL         0x23

/*
 * USB recipients, the third of three bRequestType fields
 */
#define USB_RECIP_MASK                  0x1f
#define USB_RECIP_DEVICE                0x00
#define USB_RECIP_INTERFACE             0x01
#define USB_RECIP_ENDPOINT              0x02
#define USB_RECIP_OTHER                 0x03

#define USB_ENDPOINT_HALT       0   /* IN/OUT will STALL */

/*
 * Endpoints
 */
#define USB_ENDPOINT_NUMBER_MASK        0x0f    /* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK           0x80

#define USB_ENDPOINT_XFERTYPE_MASK      0x03    /* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL       0
#define USB_ENDPOINT_XFER_ISOC          1
#define USB_ENDPOINT_XFER_BULK          2
#define USB_ENDPOINT_XFER_INT           3
#define USB_ENDPOINT_MAX_ADJUSTABLE     0x80

/*
 * Standard requests, for the bRequest field of a SETUP packet.
 *
 * These are qualified by the bRequestType field, so that for example
 * TYPE_CLASS or TYPE_VENDOR specific feature flags could be retrieved
 * by a GET_STATUS request.
 */
#define USB_REQ_GET_STATUS              0x00
#define USB_REQ_CLEAR_FEATURE           0x01
#define USB_REQ_SET_FEATURE             0x03
#define USB_REQ_SET_ADDRESS             0x05
#define USB_REQ_GET_DESCRIPTOR          0x06
#define USB_REQ_SET_DESCRIPTOR          0x07
#define USB_REQ_GET_CONFIGURATION       0x08
#define USB_REQ_SET_CONFIGURATION       0x09
#define USB_REQ_GET_INTERFACE           0x0A
#define USB_REQ_SET_INTERFACE           0x0B
#define USB_REQ_SYNCH_FRAME             0x0C


/* USB_DT_DEVICE: Device descriptor */
struct usb_device_descriptor {
        unsigned char  bLength;
        unsigned char  bDescriptorType;

        unsigned short bcdUSB;
        unsigned char  bDeviceClass;
        unsigned char  bDeviceSubClass;
        unsigned char  bDeviceProtocol;
        unsigned char  bMaxPacketSize0;
        unsigned short idVendor;
        unsigned short idProduct;
        unsigned short bcdDevice;
        unsigned char  iManufacturer;
        unsigned char  iProduct;
        unsigned char  iSerialNumber;
        unsigned char  bNumConfigurations;
} __attribute__ ((packed));

#define USB_DT_DEVICE_SIZE              18

/*
 * Device and/or Interface Class codes
 * as found in bDeviceClass or bInterfaceClass
 * and defined by www.usb.org documents
 */
#define USB_CLASS_PER_INTERFACE         0       /* for DeviceClass */
#define USB_CLASS_AUDIO                 1
#define USB_CLASS_COMM                  2
#define USB_CLASS_HID                   3
#define USB_CLASS_PHYSICAL              5
#define USB_CLASS_STILL_IMAGE           6
#define USB_CLASS_PRINTER               7
#define USB_CLASS_MASS_STORAGE          8
#define USB_CLASS_HUB                   9
#define USB_CLASS_CDC_DATA              0x0a
#define USB_CLASS_CSCID                 0x0b    /* chip+ smart card */
#define USB_CLASS_CONTENT_SEC           0x0d    /* content security */
#define USB_CLASS_VIDEO                 0x0e
#define USB_CLASS_WIRELESS_CONTROLLER   0xe0
#define USB_CLASS_MISC                  0xef
#define USB_CLASS_APP_SPEC              0xfe
#define USB_CLASS_VENDOR_SPEC           0xff

/*-------------------------------------------------------------------------*/

/* USB_DT_CONFIG: Configuration descriptor information.
 *
 * USB_DT_OTHER_SPEED_CONFIG is the same descriptor, except that the
 * descriptor type is different.  Highspeed-capable devices can look
 * different depending on what speed they're currently running.  Only
 * devices with a USB_DT_DEVICE_QUALIFIER have any OTHER_SPEED_CONFIG
 * descriptors.
 */
struct usb_config_descriptor {
       unsigned char  bLength;
       unsigned char  bDescriptorType;

       unsigned short wTotalLength;
       unsigned char  bNumInterfaces;
       unsigned char  bConfigurationValue;
       unsigned char  iConfiguration;
       unsigned char  bmAttributes;
       unsigned char  bMaxPower;
} __attribute__ ((packed));

#define USB_DT_CONFIG_SIZE              9

/* from config descriptor bmAttributes */
#define USB_CONFIG_ATT_ONE              (1 << 7)        /* must be set */
#define USB_CONFIG_ATT_SELFPOWER        (1 << 6)        /* self powered */
#define USB_CONFIG_ATT_WAKEUP           (1 << 5)        /* can wakeup */
#define USB_CONFIG_ATT_BATTERY          (1 << 4)        /* battery powered */

/*-------------------------------------------------------------------------*/

/* USB_DT_STRING: String descriptor */
struct usb_string_descriptor {
        unsigned char  bLength;
        unsigned char  bDescriptorType;

        unsigned short wString[];             /* UTF-16LE encoded */
} __attribute__ ((packed));

/*-------------------------------------------------------------------------*/
/* USB_DT_INTERFACE: Interface descriptor */
struct usb_interface_descriptor {
        unsigned char  bLength;
        unsigned char  bDescriptorType;

        unsigned char  bInterfaceNumber;
        unsigned char  bAlternateSetting;
        unsigned char  bNumEndpoints;
        unsigned char  bInterfaceClass;
        unsigned char  bInterfaceSubClass;
        unsigned char  bInterfaceProtocol;
        unsigned char  iInterface;
} __attribute__ ((packed));

#define USB_DT_INTERFACE_SIZE           9

/*-------------------------------------------------------------------------*/

/* USB_DT_ENDPOINT: Endpoint descriptor */
struct usb_endpoint_descriptor {
        unsigned char  bLength;
        unsigned char  bDescriptorType;

        unsigned char  bEndpointAddress;
        unsigned char  bmAttributes;
        unsigned short wMaxPacketSize;
        unsigned char  bInterval;
} __attribute__ ((packed));

#define USB_DT_ENDPOINT_SIZE            7
#define USB_DT_ENDPOINT_AUDIO_SIZE      9       /* Audio extension */

#endif
#endif
