
#include "OnChipRom.h"
#include "hsicDrv.h"

const uint8_t gUSB_DEVICE_DESC[USB_DESC_DEVICE_LEN + 2] =
{
    /* Device Descriptor */
    /* bLength */           USB_DESC_DEVICE_LEN,
    /* bDescriptorType */   USB_DESC_DEVICE,
    /* bcdUSB */            W2B(0x110),   /*lint !e778*/
    /* bDeviceClass */      USB_CLASS_COMM,
    /* bDeviceSubClass */   0x00,
    /* bDeviceProtocol */   0x00,
    /* bMaxPacketSize0 */   SYN_USB_MPS_EP0,
    /* idVendor */          W2B(USB_HUAWEI_VID),
    /* idProduct */         W2B(USB_HUAWEI_PID),
    /* bcdDevice */         0x01, 0x00,
    /* iManufacture */      USB_DESC_STRING_MANU_IDX,
    /* iProduct */          USB_DESC_STRING_PROD_IDX,
    /* iSerialNumber */     0x00,
    /* bNumConfigurations */USB_CONFIGURATION_NUM,
                            USB_DESC_PAD,
                            USB_DESC_PAD
};

const uint8_t gUSB_CONFIG_DESC[USB_DESC_CONFIG_TOTAL_LEN] =
{
    /* Configuration Descriptor */
    /* bLength */               USB_DESC_CONFIG_LEN,
    /* bDescriptorType */       USB_DESC_CONFIG,
    /* wTotalLength */          USB_DESC_CONFIG_TOTAL_LEN, 0x00,
    /* bNumInterfaces */        USB_INTERFACE_NUM,
    /* bConfigurationValue */   USB_CONFIG_VALUE,
    /* bConfiguration */        0x00,
    /* bmAttributes */          0x80,
    /* MaxPower */              0xFA,
    /* Interface Descriptor */
    /* bLength */               USB_DESC_INTERFACE_LEN,
    /* bDescriptorType */       USB_DESC_INTERFACE,
    /* bInterfaceNumber */      0x00,
    /* bAlternateSetting */     0x00,
    /* bNumEndpoints */         USB_ENDPOINT_COUNT,
    /* bInterfaceClass */       USB_PROT_VENDOR_SPECIFIC,
    /* bInterfaceSubClass */    USB_PROT_VENDOR_SPECIFIC,
    /* bInterfaceProtocol */    USB_PROT_VENDOR_SPECIFIC,
    /* iInterface */            0x00,
    /* Endpoint Descriptor */
    /* bLength */               USB_DESC_ENDPOINT_LEN,
    /* bDescriptorType */       USB_DESC_ENDPOINT,
    /* bEndpointAddress */      USB_UBOOT_BULK_IN,
    /* bmAttributes */          USB_ENDPOINT_XFR_BULK,
    /* wMaxPacketSize */        W2B(SYN_USB_MPS_EP1),   /*lint !e778*/
    /* bInterval */             0x00,
    /* Endpoint Descriptor */
    /* bLength */               USB_DESC_ENDPOINT_LEN,
    /* bDescriptorType */       USB_DESC_ENDPOINT,
    /* bEndpointAddress */      USB_UBOOT_BULK_OUT,
    /* bmAttributes */          USB_ENDPOINT_XFR_BULK,
    /* wMaxPacketSize */        W2B(SYN_USB_MPS_EP1),   /*lint !e778*/
    /* bInterval */             0x00
};

const uint8_t gUSB_STRING_DESC[USB_DESC_STRING_LANG_LEN + USB_DESC_STRING_MANU_LEN + USB_DESC_STRING_PROD_LEN] =
{
    /* Language */
    /* bLength */               USB_DESC_STRING_LANG_LEN,
    /* bDescriptorType */       USB_DESC_STRING,
    /* Language Code (ENG) */   W2B(0x0409),
    /* Manufacturer */
    /* bLength */               USB_DESC_STRING_MANU_LEN,
    /* bDescriptorType */       USB_DESC_STRING,
    /* ASCII */                 'H', 0,
    /* ASCII */                 'U', 0,
    /* ASCII */                 'A', 0,
    /* ASCII */                 ' ', 0,
    /* ASCII */                 'W', 0,
    /* ASCII */                 'E', 0,
    /* ASCII */                 'I', 0, /* HUA WEI */
    /* Product */
    /* bLength */               USB_DESC_STRING_PROD_LEN,
    /* bDescriptorType */       USB_DESC_STRING,
    /* ASCII */                 'U', 0,
    /* ASCII */                 'S', 0,
    /* ASCII */                 'B', 0,
    /* ASCII */                 ' ', 0,
    /* ASCII */                 'C', 0,
    /* ASCII */                 'O', 0,
    /* ASCII */                 'M', 0  /* USB COM */
};

const uint8_t gUSB_QUALIFIER_DESC[USB_DESC_QUALIFIER_LEN + 2] =
{
    /* bLength */               USB_DESC_QUALIFIER_LEN,
    /* bDescriptorType */       USB_DESC_DEVICE_QUALIFIER,
    /* bcdUSB */                W2B(0x200),   /*lint !e778*/
    /* bDeviceClass */          0x00,
    /* bDeviceSubClass */       0x00,
    /* bDeviceProtocol */       0x00,
    /* bMaxPacketSize0 */       SYN_USB_MPS_EP0,
    /* bNumConfigurations */    USB_CONFIGURATION_NUM,
    /* bReserved */             0x00,
                                USB_DESC_PAD,
                                USB_DESC_PAD
};

void hsic_memset(void *buf, uint8_t value, uint32_t size)
{
    uint8_t *pbuf = (uint8_t *)buf;
    volatile uint32_t count = size;

    while (count--) {
        *pbuf++ = value;
    }
}


static void syn_hsfc_soft_reset(void)
{
    volatile uint32_t grstctl;
    /* Wait for USB Idle state */
    do {
        grstctl = syn_read_reg32(SYN_REG_GRSTCTL);
    } while ((grstctl & ((uint32_t)1 << 31)) == 0);
    /* Configure USB to reset */
    grstctl |= 0x01;
    syn_write_reg32(SYN_REG_GRSTCTL, grstctl);
    /* Wait for USB reset completion */
    do {
        grstctl = syn_read_reg32(SYN_REG_GRSTCTL);
    } while(grstctl & 0x01);
    /* Wait for at least 3 PHY Clocks, 1ms here */
    delay(DELAY_FOR_USB_CORE_RESET_US);
}
#if 0
static void syn_hsfc_bus_disconnect(void)
{
    syn_modify_reg32(SYN_REG_DCTL, SftDiscon, 0);
    /* Section 5.3.4.2 in Synopsys spec */
    delay(4000);
}
#endif
static void syn_hsfc_bus_connect(void)
{
    syn_modify_reg32(SYN_REG_DCTL, 0, SftDiscon);
}

static void BSP_UBOOT_StallEP0(void)
{
    syn_modify_reg32(SYN_REG_DIEPCTL(0), DEPCTL_STALL, 0);   /* Stall EP0 IN */
    syn_modify_reg32(SYN_REG_DOEPCTL(0), DEPCTL_STALL, 0);   /* Stall EP0 OUT */
}

void hsic_release(void)
{
    /* Adjust HSIC signals */
    BSP_REG_SETBITS(INTEGRATOR_SC_BASE, 0x488, 7, 8, 0xFF);   /* to be test */
    /* Configure  as device */
    BSP_REG_SETBITS(INTEGRATOR_SC_BASE, 0x488, 31, 1, 1);
    /* dppulldown, dmpulldown clear */
    BSP_REG_CLRBITS(INTEGRATOR_SC_BASE, 0x488, 22, 1);
    BSP_REG_CLRBITS(INTEGRATOR_SC_BASE, 0x488, 21, 1);
    /* Release HSIC controller */
    BSP_REG_SETBITS(INTEGRATOR_SC_BASE, 0x70, 26, 1, 1);
    /* Release HSIC PHY Por */
    BSP_REG_SETBITS(INTEGRATOR_SC_BASE, 0x70, 27, 1, 1);
    /* delay 100us */
    delay(100);
}

static void BSP_UBOOT_PrepareNextSetup(pUBOOT_XFR_CTRL_INFO);
static void BSP_UBOOT_PrepareNextXfr(pUBOOT_XFR_CTRL_INFO, uint32_t);

uint32_t hsic_driver_init(void)
{
    volatile uint32_t gusbcfg;

    /* release hsic */
    hsic_release();

    /* Configure USB Reset */
    syn_hsfc_soft_reset();

    /* Check USB mode, MUST in device mode */
    if ((syn_read_reg32(SYN_REG_GINTSTS) & CurMod) != DevMode) {
        print_info("Not Device Mode!\r\n");
        return UBOOT_FALSE;
    }

    /* Configure null to device */
    syn_write_reg32(SYN_REG_GAHBCFG, 0);
    /* Configure the GUSBCFG register */
    gusbcfg = syn_read_reg32(SYN_REG_GUSBCFG);
    gusbcfg &= ~USBTrdTimMsk;
    gusbcfg |= USBTrdTim8bit;
    gusbcfg &= ~PHYif16;
    syn_write_reg32(SYN_REG_GUSBCFG, gusbcfg);
    /* Clear all the interrupt */
    syn_write_reg32(SYN_REG_GINTSTS, 0xffffffff);
    /* Configure RX fifo */
    syn_write_reg32(SYN_REG_GRXFSIZ, RxFDep);
    /* Configure TX fifo 0 */
    syn_write_reg32(SYN_REG_GNPTXFSIZ, (INEPTxF0StAddr | INEPTxF0Dep << 16));
    /* Configure TX fifo 1 */
    syn_write_reg32(SYN_REG_TXFIFO(1), (INEPnTxFStAddr | INEPnTxFDep << 16));   /*lint !e778*/
    /* Enable sg dma mode */
    syn_modify_reg32(SYN_REG_DCFG, DescDMA, 0);
    /* Reset the Device address to 0 */
    syn_modify_reg32(SYN_REG_DCFG, 0, (uint32_t)DevAddr(0x7F));
    /* Unmask the interrupts */
    syn_write_reg32(SYN_REG_GINTMSK, GINT_MASK);
    /* Unmask the setup done and transfer complete interrupt */
    syn_write_reg32(SYN_REG_DOEPMSK, DOEP_MSK);
    /* Unmask the transfer complete interrupt */
    syn_write_reg32(SYN_REG_DIEPMSK, DIEP_MSK);
    /* Unmask the EP0 & EP1 interrupt */
    syn_write_reg32(SYN_REG_DAINTMSK, DAINT_MSK);
    /* Configure the GAHBCFG register */
    syn_write_reg32(SYN_REG_GAHBCFG, (DMAEn | NPTxFEmpLvl | HBstLenINCR16));
    /* Configure Bus Connect */
    /* USB Device is disconnected by default in 2.94a */
    syn_hsfc_bus_connect();
    /* Connect hsic */
    syn_modify_reg32(SYN_REG_GLPMCFG, HSICCon, 0);

    return UBOOT_TRUE;
}

static void USB_UBOOT_HandleUSBReset(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
#if 0
    uint32_t doepmsk, diepmsk;
    /* Unmask the setup done and transfer complete interrupt */
    doepmsk = Setup | XfrCompl;
    syn_write_reg32(SYN_REG_DOEPMSK, doepmsk);
    /* Unmask the transfer complete interrupt */
    diepmsk = XfrCompl;
    syn_write_reg32(SYN_REG_DIEPMSK, diepmsk);
    /* Unmask the EP0 & EP1 interrupt */
    syn_write_reg32(SYN_REG_DAINTMSK, DAINT_MSK);
#endif
    /* Configure TX Endpoint 1 */ /* Endpoint 0 are configured by default */
    syn_write_reg32(SYN_REG_DIEPCTL(1), (DEPCTL_MPS(0x200) | DEPCTL_BULK | DEPCTL_ACTIVE) | DEPCTL_TXFNUM(1));
    /* Configure RX Endpoint 1 */
    syn_write_reg32(SYN_REG_DOEPCTL(1), (DEPCTL_MPS(0x200) | DEPCTL_BULK | DEPCTL_ACTIVE));
    /* Reset the Device address to 0 */
    syn_modify_reg32(SYN_REG_DCFG, 0, (uint32_t)DevAddr(0x7F));
    /* Prepare for the setup */
    BSP_UBOOT_PrepareNextSetup(puBootCtrl);
    /* Clear USB Reset Interrupt source */
    syn_write_reg32(SYN_REG_GINTSTS, USBRst);
}

static void USB_UBOOT_HandleUSBEnumDone(void)
{
    uint32_t cur_speed;

    cur_speed = syn_read_reg32(SYN_REG_DSTS) & EnumSpdMsk;
    cur_speed = cur_speed;
    /* Clear USB Reset Interrupt source */
    syn_write_reg32(SYN_REG_GINTSTS, EnumDone);
}

static uint32_t BSP_UBOOT_GetStatus(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    puBootCtrl->uBootCtrlXfrBuf[0] = 0;
    puBootCtrl->uBootCtrlXfrBuf[1] = 0;
    puBootCtrl->stuBootXfrInfo[0].u32USBCoreXfrLen = 2;
    puBootCtrl->stuBootXfrInfo[0].pUSBCoreXfrBuf = puBootCtrl->uBootCtrlXfrBuf;
    return UBOOT_TRUE;
}

static uint32_t BSP_UBOOT_ClearFeature(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    uint32_t target_addr;
    uint32_t target_endp;

    target_endp = (uint32_t)(puBootCtrl->setup_pkt[0].req.wIndex & 0x0F);

    if ((puBootCtrl->setup_pkt[0].req.wIndex & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN)
        target_addr = SYN_REG_DIEPCTL(target_endp);
    else
        target_addr = SYN_REG_DOEPCTL(target_endp);

    syn_modify_reg32(target_addr, 0, DEPCTL_STALL);

    puBootCtrl->stuBootXfrInfo[0].u32USBCoreXfrLen = 0;
    puBootCtrl->stuBootXfrInfo[0].pUSBCoreXfrBuf = (void *)0;

    return UBOOT_TRUE;
}

static uint32_t BSP_UBOOT_SetAddress(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    puBootCtrl->stuBootXfrInfo[0].u32USBCoreXfrLen = 0;
    puBootCtrl->stuBootXfrInfo[0].pUSBCoreXfrBuf = (void *)0;
    syn_modify_reg32(SYN_REG_DCFG, DevAddr(puBootCtrl->setup_pkt[0].req.wValue), 0);
    return UBOOT_TRUE;
}

static uint32_t BSP_UBOOT_GetDescriptor(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    pUBOOT_USB_XFR_INFO pXfrInfo = &puBootCtrl->stuBootXfrInfo[0];
    uint8_t desc_type = (uint8_t)(puBootCtrl->setup_pkt[0].req.wValue >> 8);
    uint8_t desc_indx = (uint8_t)(puBootCtrl->setup_pkt[0].req.wValue & 0x00ff);

    switch (desc_type) {
    case USB_DESC_DEVICE:
        pXfrInfo->pUSBCoreXfrBuf = (uint8_t *)gUSB_DEVICE_DESC;
        pXfrInfo->u32USBCoreXfrLen = USB_DESC_DEVICE_LEN;
        break;

    case USB_DESC_CONFIG:
        pXfrInfo->pUSBCoreXfrBuf = (uint8_t *)gUSB_CONFIG_DESC;
        pXfrInfo->u32USBCoreXfrLen = USB_DESC_CONFIG_TOTAL_LEN;
        break;

    case USB_DESC_STRING:
        if (desc_indx != USB_DESC_STRING_LANG_IDX &&
            desc_indx != USB_DESC_STRING_MANU_IDX &&
            desc_indx != USB_DESC_STRING_PROD_IDX) {
            return UBOOT_FALSE;
        }

        pXfrInfo->pUSBCoreXfrBuf = (uint8_t *)gUSB_STRING_DESC + desc_indx;
        pXfrInfo->u32USBCoreXfrLen = gUSB_STRING_DESC[desc_indx];
        break;

    case USB_DESC_DEVICE_QUALIFIER:
        pXfrInfo->pUSBCoreXfrBuf = (uint8_t *)gUSB_QUALIFIER_DESC;
        pXfrInfo->u32USBCoreXfrLen = USB_DESC_QUALIFIER_LEN;
        break;

    default:
        return UBOOT_FALSE;
    }
    return UBOOT_TRUE;
}

static uint32_t BSP_UBOOT_GetConfiguration(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    puBootCtrl->uBootCtrlXfrBuf[0] = USB_CONFIG_VALUE;
    puBootCtrl->uBootCtrlXfrBuf[1] = 0;
    puBootCtrl->stuBootXfrInfo[0].u32USBCoreXfrLen = 2;
    puBootCtrl->stuBootXfrInfo[0].pUSBCoreXfrBuf = puBootCtrl->uBootCtrlXfrBuf;
    return UBOOT_TRUE;
}

static uint32_t BSP_UBOOT_SetConfiguration(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    if (puBootCtrl->setup_pkt[0].req.wValue == USB_CONFIG_VALUE) {
        puBootCtrl->stuBootXfrInfo[0].u32USBCoreXfrLen = 0;
        puBootCtrl->stuBootXfrInfo[0].pUSBCoreXfrBuf = (void *)0;
        /* Prepare for next bulk transfer */
        BSP_UBOOT_PrepareNextXfr(puBootCtrl, 1);
        return UBOOT_TRUE;
    } else {
        return UBOOT_FALSE;
    }
}

static void BSP_UBOOT_USBCoreOutXfr(pUBOOT_XFR_CTRL_INFO puBootCtrl, uint32_t ep_num)
{
    pUBOOT_USB_XFR_INFO pXfrInfo = &puBootCtrl->stuBootXfrInfo[ep_num];
    uint32_t tsize = pXfrInfo->u32USBMaxPktSize;

    pXfrInfo->sg_desc.header = 0;
    SG_OUT_SET_BUFF_STS(pXfrInfo->sg_desc.header, SG_OUT_BUFF_DMA_BUSY);
    SG_OUT_SET_BUFF(pXfrInfo->sg_desc.buffer, (uint32_t)pXfrInfo->pUSBCoreXfrBuf);
    SG_OUT_SET_SIZE(pXfrInfo->sg_desc.header, tsize);

    SG_OUT_SET_HDR_STS(pXfrInfo->sg_desc.header, SG_OUT_INTR_COMPLETE | SG_OUT_LAST_TD);
    SG_OUT_SET_BUFF_STS(pXfrInfo->sg_desc.header, SG_OUT_BUFF_HOST_READY);

    syn_write_reg32(SYN_REG_DOEPDMA_ADDR(ep_num), (uint32_t)&pXfrInfo->sg_desc);
    syn_modify_reg32(SYN_REG_DOEPCTL(ep_num), DEPCTL_ENA | DEPCTL_CNAK, 0);
}

void BSP_UBOOT_USBCoreInXfr(pUBOOT_XFR_CTRL_INFO puBootCtrl, uint32_t ep_num)
{
    pUBOOT_USB_XFR_INFO pXfrInfo = &puBootCtrl->stuBootXfrInfo[ep_num];
    uint32_t tsize = pXfrInfo->u32USBCoreXfrLen;

    pXfrInfo->sg_desc.header = 0;
    SG_IN_SET_BUFF_STS(pXfrInfo->sg_desc.header, SG_IN_BUFF_DMA_BUSY);
    SG_IN_SET_BUFF(pXfrInfo->sg_desc.buffer, (uint32_t)pXfrInfo->pUSBCoreXfrBuf);
    SG_IN_SET_SIZE(pXfrInfo->sg_desc.header, tsize);

    if (pXfrInfo->ep_send_zlp) {
        pXfrInfo->ep_send_zlp = 0;
        SG_IN_SET_HDR_STS(pXfrInfo->sg_desc.header, SG_IN_SHORT_PKT);
    }

    SG_IN_SET_HDR_STS(pXfrInfo->sg_desc.header, SG_IN_INTR_COMPLETE | SG_IN_LAST_TD);
    SG_IN_SET_BUFF_STS(pXfrInfo->sg_desc.header, SG_IN_BUFF_HOST_READY);

    syn_write_reg32(SYN_REG_DIEPDMA_ADDR(ep_num), (uint32_t)&pXfrInfo->sg_desc);
    syn_modify_reg32(SYN_REG_DIEPCTL(ep_num), DEPCTL_ENA | DEPCTL_CNAK, 0);
}

static void BSP_UBOOT_PrepareNextSetup(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    pUBOOT_USB_XFR_INFO pXfrInfo = &puBootCtrl->stuBootXfrInfo[0];

    pXfrInfo->u32USBCoreXfrLen = 0;
    pXfrInfo->sg_desc.header = 0;

    SG_OUT_SET_BUFF_STS(pXfrInfo->sg_desc.header, SG_OUT_BUFF_DMA_BUSY);
    SG_OUT_SET_SIZE(pXfrInfo->sg_desc.header, pXfrInfo->u32USBMaxPktSize);
    SG_OUT_SET_HDR_STS(pXfrInfo->sg_desc.header, SG_OUT_INTR_COMPLETE | SG_OUT_LAST_TD);
    SG_OUT_SET_BUFF(pXfrInfo->sg_desc.buffer, (uint32_t)puBootCtrl->setup_pkt);
    SG_OUT_SET_BUFF_STS(pXfrInfo->sg_desc.header, SG_OUT_BUFF_HOST_READY);

    syn_write_reg32(SYN_REG_DOEPDMA_ADDR(0), (uint32_t)&pXfrInfo->sg_desc);
    syn_modify_reg32(SYN_REG_DOEPCTL(0), DEPCTL_ENA | DEPCTL_CNAK, 0);
}

static void BSP_UBOOT_PrepareNextXfr(pUBOOT_XFR_CTRL_INFO puBootCtrl, uint32_t ep_num)
{
    puBootCtrl->stuBootXfrInfo[ep_num].u32USBCoreXfrLen = 0;
    puBootCtrl->stuBootXfrInfo[ep_num].pUSBCoreXfrBuf = puBootCtrl->uBootBlkXfrBuf;
    puBootCtrl->stuBootXfrInfo[ep_num].sg_desc.header = 0;
    BSP_UBOOT_USBCoreOutXfr(puBootCtrl, ep_num);
}

static uint32_t BSP_UBOOT_HandSetup(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    uint32_t tsize;
    pUBOOT_USB_XFR_INFO pXfrInfo = &puBootCtrl->stuBootXfrInfo[0];

    tsize = pXfrInfo->u32USBMaxPktSize - SG_OUT_GET_SIZE(pXfrInfo->sg_desc.header);

    if (tsize == USB_SETUP_PACKET_LEN) {
        //print_info("Setup Ok\r\n");
        if ((puBootCtrl->setup_pkt[0].req.bmRequest & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD)
        {
            switch (puBootCtrl->setup_pkt[0].req.bRequest) {
            case USB_STDREQ_GET_STATUS:
                //print_info("BSP_UBOOT_GetStatus\r\n");
                return BSP_UBOOT_GetStatus(puBootCtrl);
            case USB_STDREQ_CLEAR_FEATURE:
                return BSP_UBOOT_ClearFeature(puBootCtrl);
            case USB_STDREQ_SET_ADDRESS:
                //print_info("SetAddress\r\n");
                return BSP_UBOOT_SetAddress(puBootCtrl);
            case USB_STDREQ_GET_DESCRIPTOR:
                //print_info("Descriptor\r\n");
                return BSP_UBOOT_GetDescriptor(puBootCtrl);
            case USB_STDREQ_GET_CONFIGURATION:
                //print_info("BSP_UBOOT_GetConfiguration\r\n");
                return BSP_UBOOT_GetConfiguration(puBootCtrl);
            case USB_STDREQ_SET_CONFIGURATION:
                //print_info("BSP_UBOOT_SetConfiguration\r\n");
                return BSP_UBOOT_SetConfiguration(puBootCtrl);
            default:
                return UBOOT_FALSE;
            }
        }
    }
    return UBOOT_FALSE;
}

static void BSP_UBOOT_HandleOutEP0(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    uint32_t doepint;
    pUBOOT_USB_XFR_INFO pXfrInfo = &puBootCtrl->stuBootXfrInfo[0];

    doepint = syn_read_reg32(SYN_REG_DOEPINT(0));
    /* Transfer complete */
    if (doepint & XfrCompl) {
        /* Clear XfrCompl Source */
        syn_write_reg32(SYN_REG_DOEPINT(0), XfrCompl);
        //print_info("XfrCompl!\r\n");
        /* Check if setup in s/g dma */
        if (!(pXfrInfo->sg_desc.header & SG_OUT_SETUP_RECV)) {
            /* Prepare for the next setup transfer */
            BSP_UBOOT_PrepareNextSetup(puBootCtrl);
        }
    }
    /* Setup phase done */
    if (doepint & Setup) {
        /* clear Setup Source */
        syn_write_reg32(SYN_REG_DOEPINT(0), Setup);
        //print_info("Setup!\r\n");
        /* Handle Setup Process */
        if (BSP_UBOOT_HandSetup(puBootCtrl) == UBOOT_TRUE) {
            pXfrInfo->u32USBCoreXfrLen = MIN(pXfrInfo->u32USBCoreXfrLen,
                            puBootCtrl->setup_pkt[0].req.wLength);
            /* Need zlp or not? */
            pXfrInfo->ep_send_zlp = (pXfrInfo->u32USBCoreXfrLen &
                        (pXfrInfo->u32USBMaxPktSize - 1)) ? 0 : 1;
            /* Start Transferring from Device to Host */
            BSP_UBOOT_USBCoreInXfr(puBootCtrl, 0);
        } else {
            /* Invalid Setup Packet, return Stall */
            BSP_UBOOT_StallEP0();
            /* Prepare Next Setup Process */
            BSP_UBOOT_PrepareNextSetup(puBootCtrl);
        }
    }
}

static void BSP_UBOOT_CompleteOutEP(pUBOOT_XFR_CTRL_INFO puBootCtrl, uint32_t ep_num)
{
    uint32_t tsize;
    uint32_t doepint;
    pUBOOT_USB_XFR_INFO pXfrInfo = &puBootCtrl->stuBootXfrInfo[ep_num];

    doepint = syn_read_reg32(SYN_REG_DOEPINT(ep_num));
    /* Transfer complete */
    if (doepint & XfrCompl) {
        /* Clear XfrCompl Source */
        syn_write_reg32(SYN_REG_DOEPINT(ep_num), XfrCompl);
        /* Get Transfer Size */
        tsize = pXfrInfo->u32USBMaxPktSize - SG_OUT_GET_SIZE(pXfrInfo->sg_desc.header);
        /* Should ignore the zlp */
        if (tsize != 0) {
            /* Update Transfer Size */
            pXfrInfo->pUSBCoreXfrBuf   += tsize;
            pXfrInfo->u32USBCoreXfrLen += tsize;
            puBootCtrl->u32USBCoreRecvLen += tsize;
            /* Finsih the transfer when received short packet */
            if ((tsize < pXfrInfo->u32USBMaxPktSize) || (puBootCtrl->u32uBootFileCapacity == puBootCtrl->u32USBCoreRecvLen - UBOOT_FRAME_DATA_HEAD_LEN))
                /* Only the data phase of transfer could meet the second condition */
                /*(((puBootCtrl->u32uBootCurrFrame ? (puBootCtrl->u32uBootCurrFrame - 1) : 0) * UBOOT_FRAME_LEN_DATA +
                pXfrInfo->u32USBCoreXfrLen - UBOOT_FRAME_DATA_HEAD_LEN) == puBootCtrl->u32uBootFileCapacity)) */
            {
                /* Mark to quit the USB transfer process */
                puBootCtrl->u8USBFrameXfrCompl = UBOOT_FALSE;
                /* Maybe host continue to send zlp */
                if (tsize == pXfrInfo->u32USBMaxPktSize) {
                    BSP_UBOOT_USBCoreOutXfr(puBootCtrl, ep_num);
                }
            }
            else
            {
                /* Still need to continue transferring */
                BSP_UBOOT_USBCoreOutXfr(puBootCtrl, ep_num);
            }
        }
    }
}

static void BSP_UBOOT_HandleOutEPIntr(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    uint32_t ep_intr;
    uint32_t ep_num = 0;
    /* 获取发生OUT事件中断的端点序号 */
    ep_intr = (syn_read_reg32(SYN_REG_DAINT) & DAINT_MSK & 0xffff0000) >> 16;
    /* 轮询执行OUT端点中断服务程序 */
    for (; ep_intr; ep_num++, ep_intr >>= 1) {
        if (ep_intr & 0x01) {
            if (ep_num == 0) {
                BSP_UBOOT_HandleOutEP0(puBootCtrl);
            } else {
                BSP_UBOOT_CompleteOutEP(puBootCtrl, ep_num);
            }
        }
    }
}

static void BSP_UBOOT_HandleInEP0(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    uint32_t diepint;

    diepint = syn_read_reg32(SYN_REG_DIEPINT(0));
    /* Transfer complete */
    if (diepint & XfrCompl) {
        /* Clear XfrCompl Source */
        syn_write_reg32(SYN_REG_DIEPINT(0), XfrCompl);
        //print_info("XfrCompl!\r\n");
        /* Note that in USB boot mode, we have no more than 1MPS packet to transfer through EP0 */
        /* Consider for the status stage of control read and next setup */
        BSP_UBOOT_PrepareNextSetup(puBootCtrl);
    }
}

static void BSP_UBOOT_CompleteInEP(pUBOOT_XFR_CTRL_INFO puBootCtrl, uint32_t ep_num)
{
    uint32_t diepint;

    diepint = syn_read_reg32(SYN_REG_DIEPINT(ep_num));
    /* Transfer complete */
    if (diepint & XfrCompl) {
        /* Clear XfrCompl Source */
        syn_write_reg32(SYN_REG_DIEPINT(ep_num), XfrCompl);
        /* Note that in USB boot mode, we have only a byte to transfer through bulk EP */
        /* Prepare for the next bulk transfer */
        BSP_UBOOT_PrepareNextXfr(puBootCtrl, ep_num);
    }
}

static void BSP_UBOOT_HandleInEPIntr(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    uint32_t ep_intr;
    uint32_t ep_num = 0;
    /* 获取发生IN事件中断的端点序号 */
    ep_intr = syn_read_reg32(SYN_REG_DAINT) & DAINT_MSK & 0xffff;
    /* 轮询执行IN端点中断服务程序 */
    for (; ep_intr; ep_num++, ep_intr >>= 1) {
        if (ep_intr & 0x01) {
            if (ep_num == 0) {
                BSP_UBOOT_HandleInEP0(puBootCtrl);
            } else {
                BSP_UBOOT_CompleteInEP(puBootCtrl, ep_num);
            }
        }
    }
}

void hsic_handle_event(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    volatile uint32_t gint_sts;
    /* Listen to the USB Event */
    FOREVER
    {
        do {
            gint_sts = syn_read_reg32(SYN_REG_GINTSTS) & GINT_MASK;
        } while (gint_sts == 0);
        /* Handle USB Enumerate Done Event */
        if (gint_sts & EnumDone) {
            //print_info("Enum\r\n");
            USB_UBOOT_HandleUSBEnumDone();
        }
        /* Handle USB Reset Event */
        if (gint_sts & USBRst) {
            //print_info("Reset\r\n");
            USB_UBOOT_HandleUSBReset(puBootCtrl);
        }
        /* Handle USB Out Endpoint Event */
        if (gint_sts & OutEPInt) {
            //print_info("Out\r\n");
            BSP_UBOOT_HandleOutEPIntr(puBootCtrl);
        }
        /* Handle USB In Endpoint Event */
        if (gint_sts & InEPInt) {
            //print_info("In\r\n");
            BSP_UBOOT_HandleInEPIntr(puBootCtrl);
        }

        if (puBootCtrl->u8USBFrameXfrCompl == UBOOT_FALSE) {
            puBootCtrl->u8USBFrameXfrCompl = UBOOT_TRUE;
            return;
        }
    }
}

