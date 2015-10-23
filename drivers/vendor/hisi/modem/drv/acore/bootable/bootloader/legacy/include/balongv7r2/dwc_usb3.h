#ifndef __DWC_USB3_H
#define __DWC_USB3_H

void usb3_sysctrl_init(void);
void usb3_sysctrl_exit(void);

uint32_t usb3_is_host_mode(usb3_device_t *dev);
void usb3_common_init(usb3_device_t *dev, volatile uint8_t *base);
void usb3_init(usb3_device_t *dev);

void usb3_ep_start_transfer(usb3_pcd_t *pcd, usb3_pcd_ep_t *ep);

int get_eventbuf_count(usb3_device_t *dev);
uint32_t get_eventbuf_event(usb3_device_t *dev, int size);
void update_eventbuf_count(usb3_device_t *dev, int cnt);

void usb3_handle_dev_intr(usb3_pcd_t *pcd, uint32_t event);
void usb3_handle_ep_intr(usb3_pcd_t *pcd, int physep, uint32_t event);

#endif /* __DWC_USB3_H */
