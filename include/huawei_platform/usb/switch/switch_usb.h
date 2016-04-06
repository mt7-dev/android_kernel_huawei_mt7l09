

#ifndef __LINUX_SWITCH_USB_H__
#define __LINUX_SWITCH_USB_H__

#define GPIO_HI                                1
#define    GPIO_LOW                            0

enum USB_SWITCH_STATE
{
    USB_TO_AP = 0,
    USB_TO_MODEM1 = 1,
    USB_TO_MODEM2 = 2,
    USB_TO_MODEM1_DOWNLOAD = 3,
    USB_TO_MODEM2_DOWNLOAD = 4,
    USB_TO_MHL = 5,
    USB_OFF = 6,
};

struct usb_switch_platform_data {
    const char      *name;
    unsigned long    irq_flags;
};

struct switch_usb_dev {
    const char    *name;
    struct device    *dev;
    int        state;
    int        irq1;
    struct work_struct      work;
    struct usb_switch_platform_data *pdata;
};

extern void switch_usb_set_state_through_fs(int state);
extern int switch_usb_register_notifier(struct notifier_block *nb);
extern int switch_usb_unregister_notifier(struct notifier_block *nb);

#endif /* __LINUX_SWITCH_USB_H__ */
