#include "usb3.h"
#include "usb3_drv.h"
#include "usb3_pcd.h"
#include "usb3_prot.h"

#include "OnChipRom.h"
#include "secBoot.h"
#include "OcrShare.h"

inline uint16_t usb3_calculate_crc(uint8_t byte, uint16_t crc)
{
    uint8_t  da = 0;
    const static uint32_t ta[16] = { 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
                        0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF };

    da = ((uint8_t)(crc >> 8)) >> 4;
    crc <<= 4;
    crc ^= (uint16_t)ta[da ^ (byte >> 4)];
    da = ((uint8_t)(crc >> 8)) >> 4;
    crc <<= 4;
    crc ^= (uint16_t)ta[da ^ (byte & 0x0F)];

    return crc;
}

static uint32_t usb3_image_identify(usb3_pcd_t *pcd)
{
    volatile tOcrShareData *pShareData = (tOcrShareData *)M3_SRAM_SHARE_DATA_ADDR;
    IMAGE_TYPE_E image_type;
    uint32_t ret;

    if (pShareData->bSecEn == 0)
        return USB3_XFR_PROT_OK;

    if (pcd->file_type == FILE_RAMINIT) {
        image_type = IMAGE_TYPE_RAM_INIT;
    } else {
        image_type = IMAGE_TYPE_DOWNLOAD;
    }

    ret = secCheck((uint32_t)(*pcd->file_complete), image_type);

    return (ret == SEC_SUCCESS || ret == SEC_EFUSE_NOT_WRITE) ?
            USB3_XFR_PROT_OK : USB3_XFR_PROT_ERR;
}

static void usb3_reset_protocol_info(usb3_pcd_t *pcd)
{
    pcd->file_type = 0;
    pcd->file_address = 0;
    pcd->file_capacity = 0;
    pcd->file_total_frame = 0;
    pcd->file_curr_frame = 0;
    pcd->file_next_frame = 0;
    pcd->file_received = 0;
    pcd->file_complete = NULL;
}

static uint32_t usb3_handle_file_frame(usb3_pcd_t *pcd)
{
    uint32_t len = pcd->out_ep.req.actual;
    uint8_t *buf = pcd->ss_bulk_buf;

    if (buf[FRAME_SEQ] || len != FRAME_FILE_LEN)
        return USB3_XFR_PROT_ERR;

    if (buf[FRAME_TYPE] != FILE_RAMINIT && buf[FRAME_TYPE] != FILE_USB)
        return USB3_XFR_PROT_ERR;

    pcd->file_type = buf[FRAME_TYPE];
    pcd->file_capacity = (uint32_t)buf[FRAME_LENGTH] << 24 |
                (uint32_t)buf[FRAME_LENGTH+1] << 16 |
                (uint32_t)buf[FRAME_LENGTH+2] << 8 |
                (uint32_t)buf[FRAME_LENGTH+3];
    pcd->file_address = (uint32_t)buf[FRAME_ADDRESS] << 24 |
                (uint32_t)buf[FRAME_ADDRESS+1] << 16 |
                (uint32_t)buf[FRAME_ADDRESS+2] << 8 |
                (uint32_t)buf[FRAME_ADDRESS+3];
    pcd->file_complete = (void (*)())pcd->file_address;
    pcd->file_total_frame = pcd->file_capacity / FRAME_DATA_LEN +
                (pcd->file_capacity % FRAME_DATA_LEN ? 2 : 1);
    pcd->file_received = 0;
    pcd->file_curr_frame = 0;
    pcd->file_next_frame = 1;

    return USB3_XFR_PROT_OK;
}

static uint32_t usb3_handle_data_frame(usb3_pcd_t *pcd)
{
    uint32_t len = pcd->out_ep.req.actual;
    uint8_t *buf = pcd->ss_bulk_buf;
    uint8_t *pdst, *psrc;
    uint32_t copylen;

    if (buf[FRAME_SEQ] != (uint8_t)pcd->file_next_frame) {
        if (buf[FRAME_SEQ] && buf[FRAME_SEQ] == pcd->file_curr_frame) {
            pcd->file_curr_frame -= 1;
            pcd->file_next_frame -= 1;
        } else {
            pcd->file_received -= len;
            return USB3_XFR_PROT_ERR;
        }
    }

    if (pcd->file_next_frame == pcd->file_total_frame - 1) {
        copylen = pcd->file_capacity - pcd->file_curr_frame * FRAME_DATA_LEN;
    } else {
        copylen = FRAME_DATA_LEN;
    }

    if (len != copylen + FRAME_HEAD_LEN) {
        pcd->file_received -= len;
        return USB3_XFR_PROT_ERR;
    }

    pdst = (uint8_t *)(pcd->file_address + pcd->file_curr_frame * FRAME_DATA_LEN);
    psrc = &buf[FRAME_DATA_START];

    usb3_memcpy((void *)pdst, (void *)psrc, copylen);

    pcd->file_received -= FRAME_HEAD_LEN;
    pcd->file_curr_frame += 1;
    pcd->file_next_frame += 1;

    return USB3_XFR_PROT_OK;
}

static uint32_t usb3_handle_eot_frame(usb3_pcd_t *pcd)
{
    uint32_t len = pcd->out_ep.req.actual;
    uint8_t *buf = pcd->ss_bulk_buf;

    if (buf[FRAME_SEQ] != pcd->file_next_frame && len != FRAME_EOT_LEN)
        return USB3_XFR_PROT_ERR;

    pcd->file_curr_frame += 1;
    pcd->file_next_frame += 1;

    if (pcd->file_curr_frame == pcd->file_total_frame) {
        return USB3_XFR_PROT_COMPLETE;
    } else {
        return USB3_XFR_PROT_ERR;
    }
}

static uint32_t usb3_interpret_protocol(usb3_pcd_t *pcd)
{
    uint32_t len = pcd->out_ep.req.actual;
    uint8_t *buf = pcd->ss_bulk_buf;
    uint16_t crcgo = 0, crcval = 0;
    uint32_t i;

    if (buf[FRAME_SEQ] + buf[FRAME_CSEQ] != 0xFF)
        return USB3_XFR_PROT_ERR;

    crcgo = (uint16_t)buf[len-2] << 8 | (uint16_t)buf[len-1];
    for (i = 0; i < len - 2; i++) {
        crcval = usb3_calculate_crc(buf[i], crcval);
    }

    if (crcgo != crcval) {
        return USB3_XFR_PROT_SKIP;
    }

    if ((buf[FRAME_HEAD] != FRAME_FILE) && !pcd->file_next_frame) {
        return USB3_XFR_PROT_SKIP;
    }

    switch (buf[FRAME_HEAD]) {
    case FRAME_FILE:
        return usb3_handle_file_frame(pcd);

    case FRAME_DATA:
        return usb3_handle_data_frame(pcd);

    case FRAME_EOT:
        return usb3_handle_eot_frame(pcd);

    default:
        return USB3_XFR_PROT_SKIP;
    }
}

void usb3_handle_protocol(void *dev)
{
    usb3_pcd_t *pcd = (usb3_pcd_t *)dev;
    uint32_t ret;
	uint32_t instr_offset;

    ret = usb3_interpret_protocol(pcd);

    switch (ret) {
    case USB3_XFR_PROT_OK:
        /* Response ACK to note protocol is OK */
        usb3_bulk_in_transfer(dev, USB3_RESPONSE_ACK);
        break;

    case USB3_XFR_PROT_ERR:
        /* Response NAK to note protocol is ERROR */
        usb3_bulk_in_transfer(dev, USB3_RESPONSE_NAK);
        break;

    case USB3_XFR_PROT_COMPLETE:
        /* Verify image */
        ret = usb3_image_identify(pcd);

        if (ret == USB3_XFR_PROT_OK) {
            /* valid image */
            usb3_bulk_in_transfer(dev, USB3_RESPONSE_ACK);

            if (pcd->file_type == FILE_RAMINIT) {
               // print_info("\r\nFILE_RAMINIT");
                /* Startup target function */
                instr_offset = *(volatile UINT32 *)(pcd->file_complete + BOOT_RST_ADDR_OFFEST);
                (pcd->file_complete + instr_offset)();
            } else {
                //print_info("\r\nFILE_USB");
                ocrShareSave();
                OUTREG32((INTEGRATOR_PERI_BASE+0x400), pcd->file_complete);
                SETREG32((INTEGRATOR_PERI_BASE+0x064), 0x4000);
                while(1);
            }

            /* Reset protocol control information */
            usb3_reset_protocol_info(pcd);
        } else {
            /* Invalid image */
            usb3_bulk_in_transfer(dev, USB3_RESPONSE_SNAK);
        }

    default:
        /* Silence while other error code */
        break;
    }
}
