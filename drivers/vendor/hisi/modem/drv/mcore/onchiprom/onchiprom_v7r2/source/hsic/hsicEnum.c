
#include "OnChipRom.h"
#include "hsicDrv.h"
#include "enumProt.h"
#include "OcrShare.h"

inline uint16_t BSP_UBOOT_CalcCRC(uint8_t byte, uint16_t crc)
{
    uint8_t  da = 0;
    uint32_t ta[16] = { 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
                        0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF };

    da = ((uint8_t)(crc >> 8)) >> 4;
    crc <<= 4;
    crc ^= (uint16_t)ta[da ^ (byte >> 4)];
    da = ((uint8_t)(crc >> 8)) >> 4;
    crc <<= 4;
    crc ^= (uint16_t)ta[da ^ (byte & 0x0F)];

    return crc;
}

static uint32_t BSP_UBOOT_ImageIdentify(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    int32_t ret_val;
    IMAGE_TYPE_E eImageType;
    volatile tOcrShareData *pShareData = (tOcrShareData *)M3_TCM_SHARE_DATA_ADDR;

    /* 非安全USB启动，无需校验 */
    if (pShareData->bSecEn == 0) {
        return ENUM_XFR_PROT_OK;
    }

    /* 判断USB自举下载的程序文件类型 */
    if (puBootCtrl->u32uBootFileType == ENUM_XFR_FTYPE_RAMINIT) {
        eImageType = IMAGE_TYPE_RAM_INIT;
    } else {
        eImageType = IMAGE_TYPE_DOWNLOAD;
    }

    /* 对下载的程序文件实施安全校验 */
    ret_val = secCheck((uint32_t)(*(puBootCtrl->puBootLoadFunc)), eImageType);

    /* 返回程序文件的安全校验结果 */
    return ((ret_val == SEC_SUCCESS) || (ret_val == SEC_EFUSE_NOT_WRITE)) ?
            ENUM_XFR_PROT_OK : ENUM_XFR_PROT_ERR;
}

static void BSP_UBOOT_TransInfoReset(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    puBootCtrl->u32uBootFileType = 0;
    puBootCtrl->u32uBootFileAddress = 0;
    puBootCtrl->u32uBootFileCapacity = 0;
    puBootCtrl->u32USBCoreRecvLen = 0;
    puBootCtrl->u32uBootTotalFrame = 0;
    puBootCtrl->u32uBootCurrFrame = 0;
    puBootCtrl->u8uBootNextFrame = 0;
    puBootCtrl->puBootLoadFunc = (void *)0;
}

static uint32_t BSP_UBOOT_FileFrameHandle(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    pUBOOT_USB_XFR_INFO pXfrInfo = &puBootCtrl->stuBootXfrInfo[1];
    /* FILE帧序号必须为0且长度必须为14 */
    if ((puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_SEQ] != 0) ||
        (pXfrInfo->u32USBCoreXfrLen != ENUM_FRAME_LEN_FILE))
    {
        return ENUM_XFR_PROT_ERR;
    }
    /* 下载文件类型必须为RAM或USB */
    if ((puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_TYPE] != ENUM_XFR_FTYPE_RAMINIT) &&
        (puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_TYPE] != ENUM_XFR_FTYPE_USB))
    {
        return ENUM_XFR_PROT_ERR;
    }
    /* 记录下载文件类型 */
    puBootCtrl->u32uBootFileType = puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_TYPE];
    /* 记录文件下载容量 */
    puBootCtrl->u32uBootFileCapacity = (uint32_t)(puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_LEN+0]<<24) |
                                       (uint32_t)(puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_LEN+1]<<16) |
                                       (uint32_t)(puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_LEN+2]<<8) |
                                       (uint32_t)(puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_LEN+3]);
    /* 记录文件下载地址 */
    puBootCtrl->u32uBootFileAddress = (uint32_t)(puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_ADDR+0]<<24) |
                                      (uint32_t)(puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_ADDR+1]<<16) |
                                      (uint32_t)(puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_ADDR+2]<<8) |
                                      (uint32_t)(puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_FILE_ADDR+3]);
    /* 记录文件需要下载的帧数量 */
    puBootCtrl->u32uBootTotalFrame = puBootCtrl->u32uBootFileCapacity / UBOOT_FRAME_LEN_DATA +
                    (puBootCtrl->u32uBootFileCapacity % UBOOT_FRAME_LEN_DATA ? 2 : 1);
    /* 记录下载程序文件的执行地址 */
    puBootCtrl->puBootLoadFunc = (pFuncPtr)puBootCtrl->u32uBootFileAddress;
    /* 准备接收DATA帧 */
    puBootCtrl->u32USBCoreRecvLen = 0;
    puBootCtrl->u32uBootCurrFrame = 1;
    puBootCtrl->u8uBootNextFrame = 1;

    return ENUM_XFR_PROT_OK;
}

static uint32_t BSP_UBOOT_DataFrameHandle(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    uint32_t data_len, code_len;
    uint8_t *pCodeDst = (void *)0;
    uint8_t *pCodeSrc = (void *)0;
    pUBOOT_USB_XFR_INFO pXfrInfo = &puBootCtrl->stuBootXfrInfo[1];
    /* 帧序号检查 */
    if (puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_SEQ] != puBootCtrl->u8uBootNextFrame)
    {
        /* 如果是重发的上一DATA帧，则处理上一DATA帧数据 */
        if ((puBootCtrl->u32uBootCurrFrame > 1) &&
            (puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_SEQ] == (puBootCtrl->u8uBootNextFrame - 1)))
        {
            puBootCtrl->u32uBootCurrFrame--;
            puBootCtrl->u8uBootNextFrame--;
        }
        else
        {
            return ENUM_XFR_PROT_ERR;
        }
    }
    /* 计算本次DATA帧接收的数据长度 */
    if (puBootCtrl->u32uBootCurrFrame == (puBootCtrl->u32uBootTotalFrame - 1))
    {
        data_len = puBootCtrl->u32uBootFileCapacity - (puBootCtrl->u32uBootCurrFrame - 1) * UBOOT_FRAME_LEN_DATA;
    }
    else
    {
        data_len = UBOOT_FRAME_LEN_DATA;
    }

    if (pXfrInfo->u32USBCoreXfrLen != (data_len + UBOOT_FRAME_DATA_HEAD_LEN))
    {
        return ENUM_XFR_PROT_ERR;
    }
    /* 计算本次DATA帧的目标地址 */
    pCodeDst = (uint8_t *)(puBootCtrl->u32uBootFileAddress + (puBootCtrl->u32uBootCurrFrame - 1) * UBOOT_FRAME_LEN_DATA);
    /* 计算本次DATA帧的源地址 */
    pCodeSrc = (uint8_t *)(puBootCtrl->uBootBlkXfrBuf + ENUM_FRAME_DATA_LOC);
    /* 计算本次DATA帧的写入长度 */
    code_len = pXfrInfo->u32USBCoreXfrLen - UBOOT_FRAME_DATA_HEAD_LEN;
    /* 将程序数据写入指定地址 */
    while (code_len--)
    {
        *pCodeDst++ = *pCodeSrc++;
    }
    /* 准备接收下一帧数据 */
    puBootCtrl->u32USBCoreRecvLen -= UBOOT_FRAME_DATA_HEAD_LEN;
    puBootCtrl->u32uBootCurrFrame++;
    puBootCtrl->u8uBootNextFrame++;

    return ENUM_XFR_PROT_OK;
}

static uint32_t BSP_UBOOT_EotFrameHandle(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    /* 帧序号检查 & EOT帧长度检查 */
    if ((puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_SEQ] != puBootCtrl->u8uBootNextFrame) ||
        (puBootCtrl->stuBootXfrInfo[1].u32USBCoreXfrLen!= ENUM_FRAME_LEN_EOT))
    {
        return ENUM_XFR_PROT_ERR;
    }
    /* EOT帧序号是否与总帧数量相同 */
    if (puBootCtrl->u32uBootCurrFrame == puBootCtrl->u32uBootTotalFrame)
    {
        return ENUM_XFR_PROT_COMPLETE;
    }
    else
    {
        return ENUM_XFR_PROT_ERR;
    }
}

static uint32_t BSP_UBOOT_USBTrans2Prot(pUBOOT_XFR_CTRL_INFO puBootCtrl)
{
    uint32_t cnt;
    uint16_t crcgo = 0, crcval = 0;
    pUBOOT_USB_XFR_INFO pXfrInfo = &puBootCtrl->stuBootXfrInfo[1];
    /* 校验序列号是否符合协议要求 */
    if (((uint8_t)(puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_SEQ]) +
        (uint8_t)(puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_CSEQ])) != 0xFF)
    {
        return ENUM_XFR_PROT_ERR;
    }
    /* CRC校验符合性检查 */
    crcgo = (uint16_t)((puBootCtrl->uBootBlkXfrBuf[pXfrInfo->u32USBCoreXfrLen - 2]) << 8) |
            (uint16_t)((puBootCtrl->uBootBlkXfrBuf[pXfrInfo->u32USBCoreXfrLen - 1]) << 0);
    for (cnt = 0; cnt < (pXfrInfo->u32USBCoreXfrLen - 2); cnt++)
    {
        crcval = BSP_UBOOT_CalcCRC(puBootCtrl->uBootBlkXfrBuf[cnt], crcval);
    }
    if (crcval != crcgo)
    {
        print_info("CRC Error!\r\n");
        return ENUM_XFR_PROT_ERR;
    }
    /* 文件传输的第一个帧必须是FILE帧，否则丢弃 */
    if ((puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_HEAD] != ENUM_FRAME_TYPE_FILE) &&
        (puBootCtrl->u32uBootCurrFrame == 0))
    {
        return ENUM_XFR_PROT_SKIP;
    }
    /* 帧类型散传处理 */
    switch (puBootCtrl->uBootBlkXfrBuf[ENUM_FRAME_HEAD])
    {
        /* FILE帧处理流程 */
        case ENUM_FRAME_TYPE_FILE:
            return BSP_UBOOT_FileFrameHandle(puBootCtrl);
        /* DATA帧处理流程 */
        case ENUM_FRAME_TYPE_DATA:
            return BSP_UBOOT_DataFrameHandle(puBootCtrl);
        /* EOT帧处理流程 */
        case ENUM_FRAME_TYPE_EOT:
            return BSP_UBOOT_EotFrameHandle(puBootCtrl);
        default:
            return ENUM_XFR_PROT_SKIP;
    }
}

static void BSP_UBOOT_Response2App(pUBOOT_XFR_CTRL_INFO puBootCtrl, uint8_t res)
{
    puBootCtrl->uBootBlkXfrBuf[0] = res;
    puBootCtrl->stuBootXfrInfo[1].u32USBCoreXfrLen = 1;
    puBootCtrl->stuBootXfrInfo[1].pUSBCoreXfrBuf = puBootCtrl->uBootBlkXfrBuf;
    BSP_UBOOT_USBCoreInXfr(puBootCtrl, 1);
}

void hsic_works(void)
{
    UBOOT_XFR_CTRL_INFO  stUBootCtrl;
    pUBOOT_XFR_CTRL_INFO pstUBootCtrl = &stUBootCtrl;
    uint32_t instr_offset;
    uint32_t ret_val;

    hsic_memset((void *)pstUBootCtrl, 0, sizeof(UBOOT_XFR_CTRL_INFO));

    /* Initialize the circumstances */
    pstUBootCtrl->stuBootXfrInfo[0].u32USBMaxPktSize = SYN_USB_MPS_EP0;
    pstUBootCtrl->stuBootXfrInfo[1].u32USBMaxPktSize = SYN_USB_MPS_EP1;

    /* 初始化USB控制器 */
    if (hsic_driver_init() != UBOOT_TRUE)
    {
        /* 作为HOST情形下,直接返回,调用者会进行处理,by wuzechun */
        print_info("HsicInit failed!\r\n");
        return;
    }

    /* 轮询处理USB数据收发任务和数据解析 */
    FOREVER
    {
        /* 监听USB数据收发进程 */
        hsic_handle_event(pstUBootCtrl);
        /* USB数据接收处理 */
        ret_val = BSP_UBOOT_USBTrans2Prot(pstUBootCtrl);
        /* 根据协议处理结果向主机反馈命令处理结果 */
        switch (ret_val)
        {
            /* FILE帧、DATA帧正常 */
            case ENUM_XFR_PROT_OK:
                BSP_UBOOT_Response2App(pstUBootCtrl, ENUM_RESP_TYPE_ACK);
                break;
            /* FILE帧、DATA帧或EOT帧异常 */
            case ENUM_XFR_PROT_ERR:
                BSP_UBOOT_Response2App(pstUBootCtrl, ENUM_RESP_TYPE_NAK);
                break;
            /* EOT帧正常 */
            case ENUM_XFR_PROT_COMPLETE:
                /* 下载的程序数据经过安全校验 */
                if (BSP_UBOOT_ImageIdentify(pstUBootCtrl) == ENUM_XFR_PROT_OK)
                {
                    BSP_UBOOT_Response2App(pstUBootCtrl, ENUM_RESP_TYPE_ACK);
                    if (pstUBootCtrl->u32uBootFileType == ENUM_XFR_FTYPE_RAMINIT) {
                        //print_info("\r\nRamInit!");
                        /* 执行以HSIC BOOT方式下载的raminit程序 */
                        instr_offset = *(volatile UINT32 *)(pstUBootCtrl->puBootLoadFunc + BOOT_RST_ADDR_OFFEST);
                        (pstUBootCtrl->puBootLoadFunc + instr_offset)();

                    } else {
                        //print_info("\r\nHSICDownload!");
                        ocrShareSave();
                        /*告知A核启动地址，唤醒A核，使A核执行下载的bootrom*/
                        OUTREG32((INTEGRATOR_SC_BASE+0x414), pstUBootCtrl->puBootLoadFunc);
                        SETREG32((INTEGRATOR_SC_BASE+0x64), 0x4000);
                        while(1);
                    }

                    /* 复位USB boot下载协议帧控制结构体 */
                    BSP_UBOOT_TransInfoReset(pstUBootCtrl);
                }
                /* 下载的程序数据非法 */
                else
                {
                    print_info("\r\nSecCheck Err!");
                    BSP_UBOOT_Response2App(pstUBootCtrl, ENUM_RESP_TYPE_SNAK);
                }
                break;
            /* 其他错误类型不做任何响应 */
            default:
                break;
        }
    }
}

