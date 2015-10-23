#ifndef __USB3_PCD_H__
#define __USB3_PCD_H__

usb3_pcd_ep_t *usb3_get_out_ep(usb3_pcd_t *pcd, uint32_t ep_num);
usb3_pcd_ep_t *usb3_get_in_ep(usb3_pcd_t *pcd, uint32_t ep_num);
usb3_pcd_ep_t *usb3_get_ep_by_addr(usb3_pcd_t *pcd, uint16_t index);
void usb3_ep_clear_stall(usb3_pcd_t *pcd, usb3_pcd_ep_t *ep);
uint32_t usb3_get_device_speed(usb3_pcd_t *pcd);
void usb3_pcd_set_speed(usb3_pcd_t *pcd, int speed);
void usb3_ep_set_stall(usb3_pcd_t *pcd, usb3_pcd_ep_t *ep);
void usb3_bulk_in_transfer(void *pcd, uint8_t response);
void usb3_ep_complete_request(usb3_pcd_t *pcd,
                 usb3_pcd_ep_t *ep, uint32_t event);
void usb3_os_handle_ep0(usb3_pcd_t *pcd, uint32_t event);

#endif /* __USB3_PCD_H__ */
