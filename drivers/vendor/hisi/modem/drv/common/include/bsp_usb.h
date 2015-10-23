/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_usb.h
*
*   作    者 :  wangzhongshun
*
*   描    述 :  usb驱动内接口头文件
*
*   修改记录 :  2013年2月2日  v1.00  wangzhongshun创建
*************************************************************************/

#ifndef __BSP_USB_H__
#define __BSP_USB_H__

#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */


#include "osl_types.h"
#include "drv_usb.h"

typedef enum
{
    USB_ENUM_DONE_CB_MSP = 0,
    USB_ENUM_DONE_CB_PS,
    USB_ENUM_DONE_CB_BSP,
    USB_ENUM_DONE_CB_BOTTEM
}USB_ENUM_DONE_CB_E;

#define USB_ENABLE_CB_MAX 32
#define USB_CDEV_NAME_MAX 64

/* usb dbg module */
#define USB_DBG_NV  (NV_ID_DRV_USB_DBG)

#define USB_DBGMODU_GADGET  BIT(0)
#define USB_DBGMODU_DWC3    BIT(1)
#define USB_DBGMODU_CORE    BIT(2)
#define USB_DBGMODU_VENDOR  BIT(3)
#define USB_DBGMODU_OTG     BIT(4)
#define USB_DBGMODU_XHCI    BIT(5)
#define USB_DBGMODU_STORAGE BIT(6)

typedef struct
{
    u32 dbg_module;   /* usb调试信息控制 */
}usb_dbg_nv_t;

typedef struct
{
    /* usb nv structure ... */
    usb_dbg_nv_t dbg_info;

    u32 nv_key;

    /* stat counter */
    u32 stat_nv_read_fail;
} usb_dbg_info_t;

extern usb_dbg_info_t g_usb_dbg_info;

#define USB_DBG_GADGET(d, fmt, args...)\
do {\
    if (g_usb_dbg_info.dbg_info.dbg_module & USB_DBGMODU_GADGET){\
        dev_dbg(d, fmt, ## args);\
    }\
} while (0)

#define USB_DBG_CORE(d, fmt, args...)\
do {\
    if (g_usb_dbg_info.dbg_info.dbg_module & USB_DBGMODU_CORE){\
        dev_dbg(d, fmt, ## args);\
    }\
} while (0)

#define USB_DBG_DWC3(d, fmt, args...)\
do {\
    if (g_usb_dbg_info.dbg_info.dbg_module & USB_DBGMODU_DWC3){\
        dev_dbg(d, fmt, ## args);\
    }\
} while (0)

#define USB_DBG_VENDOR(fmt, args...)\
do {\
    if (g_usb_dbg_info.dbg_info.dbg_module & USB_DBGMODU_VENDOR){\
        printk(fmt, ## args);\
    }\
} while (0)

#define USB_DBG_OTG(d, fmt, args...) \
do{\
    if (g_usb_dbg_info.dbg_info.dbg_module & USB_DBGMODU_OTG)\
        dev_dbg((d)->dev, "%s(): " fmt , __func__, ## args);\
}while(0)

#define USB_DBG_XHCI(xhci, fmt, args...) \
do {\
    if (XHCI_DEBUG && (g_usb_dbg_info.dbg_info.dbg_module & USB_DBGMODU_XHCI)) \
        dev_dbg(xhci_to_hcd(xhci)->self.controller , fmt , ## args); \
} while (0)

#define USB_DBG_STORAGE(x...) printk(KERN_DEBUG USB_STORAGE x )

typedef void (*usb_enum_done_cb_t)(void);

typedef struct
{
    usb_enum_done_cb_t enum_done_cbs[USB_ENUM_DONE_CB_BOTTEM];
    USB_UDI_ENABLE_CB_T udi_enable_cb[USB_ENABLE_CB_MAX];
    USB_UDI_DISABLE_CB_T udi_disable_cb[USB_ENABLE_CB_MAX];
}USB_CTX_S;


extern s32 bsp_usb_register_enablecb(USB_UDI_ENABLE_CB_T pFunc);
extern s32 bsp_usb_register_disablecb(USB_UDI_DISABLE_CB_T pFunc);

void bsp_usb_dbg_init(void);

int dwc3_phy_auto_powerdown(int enable);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End of __BSP_USB_H__ */
