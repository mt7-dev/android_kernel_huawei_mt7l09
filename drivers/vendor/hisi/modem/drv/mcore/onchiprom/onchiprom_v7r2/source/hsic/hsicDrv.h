
#ifndef __USB_BOOT_H
#define	__USB_BOOT_H

#include "OnChipRom.h"
#include "sys.h"
#include "secBoot.h"


#if PLATFORM == PLATFORM_V7R2_EDA
#define DELAY_FOR_USB_PHY_UP_US 2
#define DELAY_FOR_USB_CORE_RESET_US 10
#else
#define DELAY_FOR_USB_PHY_UP_US 10000
#define DELAY_FOR_USB_CORE_RESET_US 3000
#endif

/* Type Definition */
typedef signed int          int32_t;
typedef unsigned int        uint32_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed char         int8_t;
typedef unsigned char       uint8_t;

/* USB Controller Register definiton */
#define SYN_REG_BASE            0x90080000
#define SYN_REG_GAHBCFG         0x08
    #define GlblIntrMsk         (1 << 0)
    #define DMAEn               (1 << 5)
    #define NPTxFEmpLvl         (1 << 7)
    #define HBstLenSingle       (0 << 1)
    #define HBstLenINCR         (1 << 1)
    #define HBstLenINCR4        (3 << 1)
    #define HBstLenINCR8        (5 << 1)
    #define HBstLenINCR16       (7 << 1)
#define SYN_REG_GUSBCFG         0x0C
    #define USBTrdTimMsk        (0x0f << 10)
    #define USBTrdTim8bit       (9 << 10)
    #define USBTrdTim16bit      (5 << 10)
    #define PHYif16             (1 << 3)
#define SYN_REG_GRSTCTL         0x10
#define SYN_REG_GINTSTS         0x14
    #define CurMod              (1 << 0)
    #define DevMode             0
    #define HostMode            1
#define SYN_REG_GINTMSK         0x18
    #define USBRst              (1 << 12)
    #define EnumDone            (1 << 13)
    #define OutEPInt            (1 << 19)
    #define InEPInt             (1 << 18)
    #define GINT_MASK           (USBRst | EnumDone | OutEPInt | InEPInt)
#define SYN_REG_GLPMCFG         0x54
    #define HSICCon             (1 << 30)
    
#define SYN_REG_GRXFSIZ         0x24
    #define RxFDep              0x200
#define SYN_REG_GNPTXFSIZ       0x28
    #define INEPTxF0StAddr      RxFDep
    #define INEPTxF0Dep         0x100
#define SYN_REG_DCFG            0x800
    #define DescDMA             (1 << 23)
    #define DevAddr(addr)       ((addr) << 4)
#define SYN_REG_DCTL            0x804
    #define IgnrFrmNum          (1 << 15)
    #define SftDiscon           (1 << 1)
#define SYN_REG_DSTS            0x808
    #define EnumSpdMsk          (3 << 1)
    #define EnumSpdHigh         (0 << 1)
    #define EnumSpdFull         (1 << 1)
    #define EnumSpdLow          (2 << 1)
#define SYN_REG_DIEPINT(x)      (0x908 + ((x) * 0x20))
#define SYN_REG_DOEPINT(x)      (0xb08 + ((x) * 0x20))
#define SYN_REG_DIEPDMA_ADDR(x) (0x914 + ((x) * 0x20))
#define SYN_REG_DOEPDMA_ADDR(x) (0xb14 + ((x) * 0x20))
#define SYN_REG_DIEPMSK         0x810
#define SYN_REG_DOEPMSK         0x814
    #define XfrCompl            (1 << 0)
    #define Setup               (1 << 3)
    #define DIEP_MSK            (XfrCompl)
    #define DOEP_MSK            (Setup | XfrCompl)
#define SYN_REG_DAINT           0x818
#define SYN_REG_DAINTMSK        0x81C
    #define DAINT_MSK           (3 << 16 | 3 << 0)
#define SYN_REG_DIEPCTL(x)      (0x900 + ((x) * 0x20))
#define SYN_REG_DOEPCTL(x)      (0xb00 + ((x) * 0x20))
    #define DEPCTL_MPS(mps)     ((mps) << 0)
    #define DEPCTL_ACTIVE       (1 << 15)
    #define DEPCTL_CTRL         (0 << 18)
    #define DEPCTL_ISOC         (1 << 18)
    #define DEPCTL_BULK         (2 << 18)
    #define DEPCTL_INTR         (3 << 18)
    #define DEPCTL_STALL        (1 << 21)
    #define DEPCTL_TXFNUM(ep)   (1 << 22)
    #define DEPCTL_CNAK         (1 << 26)
    #define DEPCTL_SNAK         (1 << 27)
    #define DEPCTL_DATA0        (1 << 28)
    #define DEPCTL_DATA1        (1 << 29)
    #define DEPCTL_ENA          ((uint32_t)1 << 31)
#define SYN_REG_DIEPTSIZE(x)    (0x910 + ((x) * 0x20))
#define SYN_REG_DOEPTSIZE(x)    (0xb10 + ((x) * 0x20))
#define SYN_REG_DIEPEMPMSK      0x834
#define SYN_REG_TXFIFO(x)       (0x104 + ((x) - 1) * 0x04)
    #define INEPnTxFStAddr      (INEPTxF0StAddr + INEPTxF0Dep)
    #define INEPnTxFDep         0x200

/* s/g related operation */
typedef struct tagUSB_SG_BUF_DESC
{
    uint32_t header;
    uint32_t buffer;
} USB_SG_BUF_DESC, *pUSB_SG_BUF_DESC;

/* OUT Direction */
#define SG_OUT_BUFF_STS_MSK         ((uint32_t)3 << 30)
#define SG_OUT_BUFF_HOST_READY      0
#define SG_OUT_BUFF_DMA_BUSY        1
#define SG_OUT_BUFF_DMA_DONE        2
#define SG_OUT_BUFF_HOST_BUSY       3
#define SG_OUT_SET_BUFF_STS(v, s)   ((v) = (((v) & ~SG_OUT_BUFF_STS_MSK) | \
    ((uint32_t)(s) << 30 & SG_OUT_BUFF_STS_MSK)))

#define SG_OUT_SET_HDR_STS(v, s)    ((v) |= (s))
#define SG_OUT_SET_BUFF(v, s)       ((v) = (s))
    
#define SG_OUT_SIZE_MSK             0xffff
#define SG_OUT_SIZE_MAX             0xffff
#define SG_OUT_GET_SIZE(v)          ((v) & SG_OUT_SIZE_MSK)
#define SG_OUT_SET_SIZE(v, s)       ((v) = ((v) & ~SG_OUT_SIZE_MSK) | \
    ((s) & SG_OUT_SIZE_MSK))

#define SG_OUT_SETUP_RECV           (1 << 24)
#define SG_OUT_INTR_COMPLETE        (1 << 25)
#define SG_OUT_SHORT_PKT            (1 << 26)
#define SG_OUT_LAST_TD              (1 << 27)

/* IN Direction */
#define SG_IN_BUFF_STATUS_MASK      ((uint32_t)3 << 30)
#define SG_IN_BUFF_HOST_READY       0
#define SG_IN_BUFF_DMA_BUSY         1
#define SG_IN_BUFF_DMA_DONE         2
#define SG_IN_BUFF_HOST_BUSY        3
#define SG_IN_SET_BUFF_STS(v,s)  ((v) = (((v) & ~SG_IN_BUFF_STATUS_MASK) | \
    (((uint32_t)(s) << 30) & SG_IN_BUFF_STATUS_MASK)))

#define SG_IN_SET_HDR_STS(v, s)     ((v) |= (s))
#define SG_IN_SET_BUFF(v, s)        ((v) = (s))

#define SG_IN_SIZE_MASK             0xffff
#define SG_IN_SIZE_MAX              0xffff
#define SG_IN_GET_SIZE(v)           ((v) & SG_IN_SIZE_MASK)
#define SG_IN_SET_SIZE(v,s)         ((v) = (((v) & ~SG_IN_SIZE_MASK) | \
    ((s) & SG_IN_SIZE_MASK)))
    
#define SG_IN_INTR_COMPLETE         (1 << 25)
#define SG_IN_SHORT_PKT             (1 << 26)
#define SG_IN_LAST_TD               (1 << 27)

/* USB Endpoint Max Packet Size */
#define SYN_USB_MPS_EP0             0x40
#define SYN_USB_MPS_EP1             0x200
#define SYN_USB_MPS_EP2             0x200

#define FOREVER         for(;;)
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))

/* USB Manuipulation */
#define syn_read_reg32(offset)          (*(volatile uint32_t *)(SYN_REG_BASE + (offset)))
#define syn_write_reg32(offset, value)  (*(volatile uint32_t *)(SYN_REG_BASE + (offset)) = (value))
#define syn_modify_reg32(offset, setmsk, clrmsk) syn_write_reg32(offset, (syn_read_reg32(offset) & ~(clrmsk)) | (setmsk))

#define BSP_REG(base, reg) (*(volatile uint32_t *)((uint32_t)base + (reg)))
#define BSP_REG_CLRBITS(base, reg, pos, bits) \
    (BSP_REG(base, reg) &= ~((((uint32_t)1 << (bits)) - 1) << (pos)))
#define BSP_REG_SETBITS(base, reg, pos, bits, val) \
    (BSP_REG(base, reg) = (BSP_REG(base, reg) & (~((((uint32_t)1 << (bits)) - 1) << (pos)))) | \
    ((uint32_t)((val) & (((uint32_t)1 << (bits)) - 1)) << (pos)))

/* EP Index & Number switch */
#define EP_INX_TO_NUM(i)        ((i) >> 1)
#define EP_NUM_TO_IN_IDX(n)     ((n) << 1)
#define EP_NUM_TO_OUT_IDX(n)    (((n) << 1) + 1)

/* USB Endpoint Attribute */
#define USB_ENDPOINT_NUM_MASK           0x0f    /* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK           0x80
#define USB_ENDPOINT_XFR_MASK           0x03    /* in bmAttributes */
#define USB_ENDPOINT_XFR_CTRL           0
#define USB_ENDPOINT_XFR_ISOC           1
#define USB_ENDPOINT_XFR_BULK           2
#define USB_ENDPOINT_XFR_INTR           3

/* CONTROL REQUEST SUPPORT */
#define USB_DIR_OUT                     0       /* to device */
#define USB_DIR_IN                      0x80    /* to host */

/* USB types, the second of three bRequestType fields */
#define USB_REQ_TYPE_MASK               (0x03 << 5)
#define USB_REQ_TYPE_STANDARD           (0x00 << 5)
#define USB_REQ_TYPE_CLASS              (0x01 << 5)
#define USB_REQ_TYPE_VENDOR             (0x02 << 5)
#define USB_REQ_TYPE_RESERVED           (0x03 << 5)

/* USB recipients, the third of three bRequestType fields */
#define USB_REQ_RECIP_MASK                  0x03
#define USB_REQ_RECIP_DEVICE                0x00
#define USB_REQ_RECIP_INTERFACE             0x01
#define USB_REQ_RECIP_ENDPOINT              0x02
#define USB_REQ_RECIP_OTHER                 0x03

/* USB Standard Request */
#define USB_STDREQ_GET_STATUS              0x00
#define USB_STDREQ_CLEAR_FEATURE           0x01
#define USB_STDREQ_SET_FEATURE             0x03
#define USB_STDREQ_SET_ADDRESS             0x05
#define USB_STDREQ_GET_DESCRIPTOR          0x06
#define USB_STDREQ_SET_DESCRIPTOR          0x07
#define USB_STDREQ_GET_CONFIGURATION       0x08
#define USB_STDREQ_SET_CONFIGURATION       0x09
#define USB_STDREQ_GET_INTERFACE           0x0A
#define USB_STDREQ_SET_INTERFACE           0x0B
#define USB_STDREQ_SYNCH_FRAME             0x0C

/* USB Descriptor Type */
#define USB_DESC_DEVICE                   0x01
#define USB_DESC_CONFIG                   0x02
#define USB_DESC_STRING                   0x03
#define USB_DESC_INTERFACE                0x04
#define USB_DESC_ENDPOINT                 0x05
#define USB_DESC_DEVICE_QUALIFIER         0x06
#define USB_DESC_OTHER_SPEED_CONFIG       0x07
#define USB_DESC_INTERFACE_POWER          0x08

/* USB Descriptor Type Length */
#define USB_DESC_DEVICE_LEN                 0x12
#define USB_DESC_CONFIG_LEN                 0x09
#define USB_DESC_INTERFACE_LEN              0x09
#define USB_DESC_ENDPOINT_LEN               0x07
#define USB_DESC_QUALIFIER_LEN              0x0A
#define USB_DESC_STRING_LANG_LEN            0x04
#define USB_DESC_STRING_PROD_LEN            0x10
#define USB_DESC_STRING_MANU_LEN            0x10
#define USB_DESC_CONFIG_TOTAL_LEN           (USB_DESC_CONFIG_LEN + USB_DESC_INTERFACE_LEN + USB_DESC_ENDPOINT_LEN * 2)

/* USB Descriptor Type Index */
#define USB_DESC_STRING_LANG_IDX            0
#define USB_DESC_STRING_MANU_IDX            USB_DESC_STRING_LANG_LEN
#define USB_DESC_STRING_PROD_IDX            (USB_DESC_STRING_LANG_LEN + USB_DESC_STRING_MANU_LEN)

/* Device/Interface Class Code */
#define USB_CLASS_COMM                      2
#define USB_PROT_VENDOR_SPECIFIC            0xFF

/* USB Configuration Value & Number & Interface Number */
#define USB_CONFIG_VALUE                    1
#define USB_INTERFACE_NUM                   1
#define USB_CONFIGURATION_NUM               1

/* USB Boot Endpoint Count */
#define USB_ENDPOINT_COUNT                  2

/* USB Boot Endpoint Address */
#define USB_UBOOT_BULK_IN                   0x01
#define USB_UBOOT_BULK_OUT                  0x81

/* Word to Byte Convert */
#define W2B(val)                            (uint8_t)(val & 0x00FF), (uint8_t)(val >> 8)

/* USB Boot VID & PID */
#define USB_HUAWEI_VID                      0x12D1
#define USB_HUAWEI_PID                      0x1443

/* For Word Alignment */
#define USB_DESC_PAD                        0x00

/* UBOOT Result Value */
#define UBOOT_TRUE                          0
#define UBOOT_FALSE                         1

#define USB_SETUP_PACKET_LEN                8
typedef struct tagUSB_SETUP_PACKET
{
    uint8_t  bmRequest;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} USB_SETUP_PACKET, *pUSB_SETUP_PACKET;

typedef void (*pFuncPtr)(void);

typedef struct tagUBOOT_USB_XFR_INFO
{
    /* USB是否需要zlp标志 */
    uint32_t ep_send_zlp;
    /* USB端点最大包长度 */
    uint32_t u32USBMaxPktSize;
    /* USB需要传输总长度 */
    uint32_t u32USBCoreXfrLen;
    /* USB传输指向的s/g描述符 */
    USB_SG_BUF_DESC sg_desc;
    /* USB数据传输指向的缓冲区指针 */
    uint8_t *pUSBCoreXfrBuf;
} UBOOT_USB_XFR_INFO, *pUBOOT_USB_XFR_INFO;

#define UBOOT_FRAME_LEN_DATA            1024
#define UBOOT_FRAME_DATA_HEAD_LEN       5
#define UBOOT_FRAME_DATA_LEN            (UBOOT_FRAME_DATA_HEAD_LEN + UBOOT_FRAME_LEN_DATA)

typedef struct tagUBOOT_XFR_CTRL_INFO
{
    /* 下载的文件类型 */
    uint32_t u32uBootFileType;
    /* 下载的文件写入地址 */
    uint32_t u32uBootFileAddress;
    /* 下载的文件容量 */
    uint32_t u32uBootFileCapacity;
    /* USB已经接收的数据长度 */
    uint32_t u32USBCoreRecvLen;
    /* 下载文件需要的总帧数 */
    uint32_t u32uBootTotalFrame;
    /* 当前正在传输的帧数 */
    uint32_t u32uBootCurrFrame;
    /* 下载的程序文件执行入口 */
    pFuncPtr puBootLoadFunc;
    /* 期待传输的下一个帧数 */
    uint8_t u8uBootNextFrame;
    /* 帧传输完成标志 */
    volatile uint8_t u8USBFrameXfrCompl;
    /* USB控制端点传输需要的Buffer */
    uint8_t  uBootCtrlXfrBuf[2];
    /* CONTROL端点枚举请求变量 */
    union {
        USB_SETUP_PACKET req;
        uint32_t b32[2];
    } setup_pkt[3];
    /* USB传输控制结构体 */
    UBOOT_USB_XFR_INFO stuBootXfrInfo[2];
    /* USB Bulk端点传输需要的Buffer */
    uint8_t  uBootBlkXfrBuf[UBOOT_FRAME_DATA_LEN];
} UBOOT_XFR_CTRL_INFO, *pUBOOT_XFR_CTRL_INFO;

extern void hsic_memset(void *buf, uint8_t value, uint32_t size);
extern uint32_t hsic_driver_init(void);
extern void hsic_handle_event(pUBOOT_XFR_CTRL_INFO puBootCtrl);
extern void BSP_UBOOT_USBCoreInXfr(pUBOOT_XFR_CTRL_INFO puBootCtrl, uint32_t ep_num);

#endif  /* __USB_BOOT_H */

