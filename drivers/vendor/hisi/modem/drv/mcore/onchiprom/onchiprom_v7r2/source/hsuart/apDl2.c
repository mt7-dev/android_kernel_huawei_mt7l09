/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  apDl.c
*
*   作    者 :  wuzechun
*
*   描    述 :  使用HDLC协议,从AP下载BootLoader/烧写工具引导程序
*
*   修改记录 :  2011年7月15日  v1.00  wuzechun  创建
*
*************************************************************************/

#include "OnChipRom.h"
#include "OcrShare.h"
#include "string.h"
#include "sys.h"
#include "hsuart.h"
#include "hdlc.h"
#include "secBoot.h"
#include "emmcMain.h"
#include "apDl2.h"

/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 内部函数原型说明                                             *
 *--------------------------------------------------------------*/

/*****************************************************************************
* 函 数 名  : apSecChk
*
* 功能描述  : AP封装的安全校验函数
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
INT32 apSecChk( UINT32 ulImgAddr )
{
    INT32 iRet;
    volatile tOcrShareData *pShareData = (tOcrShareData *)M3_TCM_SHARE_DATA_ADDR;

    if(BL_CHECK_INSTRUCTION != *(volatile UINT32 *)(ulImgAddr + BL_CHECK_ADDR_OFFSET))
    {
        /* 镜像未烧入或错误镜像,返回错误供调用者处理 */
        print_info("\r\nimage not program!" );

        return SEC_NO_IMAGE;
    }

    if(!pShareData->bSecEn)
    {
        print_info("\r\nUnSec_boot!" );
        return SEC_SUCCESS;
    }

    iRet = secCheck(ulImgAddr, IMAGE_TYPE_BOOTLOAD);
    switch(iRet)
    {
        case SEC_SUCCESS:/*安全校验通过*/
            print_info("\r\nSec check ok!" );  /*lint !e616*/
            break;
            /* 进入下一case运行BootLoader */
        case SEC_EFUSE_NOT_WRITE:     /*lint !e825*/ /*EFUSE 未烧写*/
            break;

        case SEC_SHA_CALC_ERROR:  /* Hash计算不通过 */
        case SEC_OEMCA_ERROR:  /* OEM CA校验不通过 */
        case SEC_IMAGE_ERROR:   /* 映像校验不通过 */
        case SEC_ROOT_CA_ERROR:/* 根CA校验错误 */
        case SEC_IMAGE_LEN_ERROR:/*安全版本长度错误*/
            print_info("\r\nSec check err!" );

            break;

        case SEC_EFUSE_READ_ERROR:/*Efuse读取失败,使用看门狗复位,再次尝试读取*/
            print_info("\r\nEfuse read err, reboot...");

            /* Efuse读取失败,复位单板 */
            setErrno(SYS_ERR_EFUSE_READ);
            wdtRebootDelayMs(TIME_DELAY_MS_2000_FOR_EFUSE_READERR);
            break;

        default:
            while(1)    /*lint !e716*/
            {
                print_info_with_u32("\r\nunhandered ret:",(UINT32)iRet);
                delay(1000*1000);   /* 延时1s */
            }
            /*break;     */  /*for pc lint*/

    }

    return iRet;
}

/*****************************************************************************
* 函 数 名  : apDlInit
*
* 功能描述  : 下载初始化
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void apDlInit( apDlCtrlS *pstDlCtrl )
{
    pstDlCtrl->ulFileLength = 0;
    pstDlCtrl->ulPosition = 0;
    pstDlCtrl->ulAddress = 0;
    pstDlCtrl->bComplete = FALSE;

    if(HSUART_STATUS_OK != hsUartInit(pstDlCtrl->pstHsUartBuf))
    {
        print_info("\r\nhsuart init failed!");
    }

    HDLC_Init(pstDlCtrl->pstHdlcCtrl);

}

/*****************************************************************************
* 函 数 名  : apDlRecvPkt
*
* 功能描述  : 接收一个完整数据包
*
* 输入参数  : pstDlCtrl,传输控制结构体
*                  eRepCode,错误码
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
apDlStatusE apDlRecvPkt(apDlCtrlS *pstDlCtrl)
{
    INT32 iRet;
    UINT8 ucRecv;
//print_info("\r\nwait for a pkt...");
    while(1)    /*lint !e716*/
    {
        /* 每次处理一个字符 */
        iRet = hsUartRecvData(pstDlCtrl->pstHsUartBuf, &ucRecv, 1);
        if(HSUART_RECV_TIMEOUT == iRet)
        {
            return AP_DL_RECV_TIMEOUT;
        }
        else if(ERROR == iRet)
        {
            print_info_with_u32("hsuart recv err:  ", (UINT32)iRet);
            return AP_DL_ERR_RECV;
        }

//print_info_with_u32(" ", (UINT32)ucRecv);

        iRet = HDLC_Decap(pstDlCtrl->pstHdlcCtrl, ucRecv);

        /* 完整接收一包数据,返回OK */
        if(HDLC_OK == iRet)
        {
            return AP_DL_ERR_OK;
        }
        else if(HDLC_ERR_BUF_FULL == iRet)
        {
            print_info("\r\nhdlc buf full!");
            return AP_DL_BUF_FULL;
        }

    }

}

/*****************************************************************************
* 函 数 名  : sendRepPkt
*
* 功能描述  : 组装应答包
*
* 输入参数  : pstDlCtrl,传输控制结构体
*                  eRepCode,错误码
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
apDlStatusE sendRepPkt(apDlCtrlS *pstDlCtrl, RepCodeE eRepCode )
{
    INT32 iRet;
    UINT32 ulLen = 0;
    UINT8 aucBuf[PKT_LEN_REP];

    aucBuf[ulLen++] = (UINT8)PKT_TYPE_REP;

    aucBuf[ulLen++] = (UINT8)eRepCode;

    /* 使用HDLC内部Buf构造HDLC包 */
    iRet = HDLC_Encap(aucBuf, (UINT16)ulLen, pstDlCtrl->pstHdlcCtrl->au8DecapBuf, (UINT16)HDLC_BUF_SIZE, &(pstDlCtrl->u16SendLen));
    if(HDLC_OK != iRet)
    {
        return AP_DL_ERR_HDLC_ENCAP;
    }

    (void)hsUartSend(pstDlCtrl->pstHdlcCtrl->au8DecapBuf, pstDlCtrl->u16SendLen);

    return AP_DL_ERR_OK;
}

/*****************************************************************************
* 函 数 名  : sendRepErrWaitReset
*
* 功能描述  : 向AP发送Nak并且循环等待AP复位
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void sendRepErrWaitReset(apDlCtrlS *pstDlCtrl, RepCodeE eRepCode )
{
    (void)sendRepPkt(pstDlCtrl, eRepCode);

	while(1) /*lint !e716*//* Wait AP reset modem */
    {
        print_info("\r\nwait for AP to reset me, loop forever...");
        delay(1000*1000);
    }
}

/*****************************************************************************
* 函 数 名  : handerBaudSetPkt
*
* 功能描述  : 处理AP发来的命令包
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
INT32 handerBaudSetPkt( apDlCtrlS *pstDlCtrl )
{
    UINT8 *pu8Buf = pstDlCtrl->pstHdlcCtrl->au8DecapBuf;

    UINT32 ulBaudRate = HSUART_MAKE_LONG(pu8Buf[BAUD_SET_OS], \
                            pu8Buf[BAUD_SET_OS+1],\
                            pu8Buf[BAUD_SET_OS+2],\
                            pu8Buf[BAUD_SET_OS+3]);
//print_info_with_u32("\r\nbaud:", ulBaudRate);
    /* 发送Ack之后再修改波特率 */
    if(!isHsSptBaud(ulBaudRate))
    {
        (void)sendRepPkt(pstDlCtrl, REP_BAUD_NOT_SUPPORT);
        return ERROR;
    }

    (void)sendRepPkt(pstDlCtrl, REP_ACK);
 // print_info_with_u32("\r\nrep_ack ,baud:", ulBaudRate);
    /* 更新波特率 */
    if(hsUartSetBaud(ulBaudRate) != HSUART_STATUS_OK)
    {
        print_info("\r\n hsUartSetBaud error!!!\r\n");
        return ERROR;
    }

    return OK;

}

/*****************************************************************************
* 函 数 名  : handerApCmdPkt
*
* 功能描述  : 处理AP发来的命令包
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
INT32 handerApCmdPkt( apDlCtrlS *pstDlCtrl )
{
    INT32 iRet;
    UINT8 u8Cmd = pstDlCtrl->pstHdlcCtrl->au8DecapBuf[AP_CMD_OS];
    int BOOT_RST_Addr = 0x00;

    switch(u8Cmd)
    {
        case AP_CMD_NAND_BOOT:  /* 自身nand直接启动 */
            print_info("\r\nNF_boot!" );
            iRet = nandReadBl(M3_TCM_BL_ADDR);
            break;

        case AP_CMD_EMMC_BOOT:  /* 自身MMC设备直接启动 */
            print_info("\r\nEMMC_boot!" );
            iRet = emmc_read_bootloader(M3_TCM_BL_ADDR);
            break;

        case AP_CMD_DL_BL_BURN_PRE: /* 等待接收文件,这里直接返回成功 */
            (void)sendRepPkt(pstDlCtrl, REP_ACK);;
            return OK;

        default:    /* 不支持的命令 */
            (void)sendRepPkt(pstDlCtrl, REP_AP_CMD_NOT_SUPPORT);
            return ERROR;
            /* break; */

    }

    /* 如果自身设备启动,开始校验;其他情形已经直接返回了,此处不再做处理 */
    if(OK != iRet)
    {
        /* 发送读取失败,等待AP复位,不再返回 */
        sendRepErrWaitReset(pstDlCtrl, REP_NAND_READ_ERR);
    }

    iRet = apSecChk(M3_TCM_BL_ADDR);

    if(SEC_SUCCESS == iRet
        ||SEC_EFUSE_NOT_WRITE == iRet)
    {
        ocrShareSave();

        /* 校验通过或未使能安全校验,通知AP正常,准备运行镜像 */
        (void)sendRepPkt(pstDlCtrl, REP_ACK);
        /* 不再返回 */
        /*跳转的地址需要计算*/
        BOOT_RST_Addr = *(volatile UINT32 *)BOOT_RST_ADDR_PP;
        go((FUNCPTR)(M3_TCM_BL_ADDR + BOOT_RST_Addr));
    }
    else
    {
        /* 发送校验失败,等待AP复位,不再返回 */
        sendRepErrWaitReset(pstDlCtrl, REP_SEC_CHK_ERR);
    }

    return OK;   /* for compiler happy */
}

/*****************************************************************************
* 函 数 名  : handerStartDataPkt
*
* 功能描述  : 处理开始接收文件包
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
INT32 handerStartDataPkt( apDlCtrlS *pstDlCtrl )
{
    UINT32 ulFileLength;
    UINT8 *pu8Buf = pstDlCtrl->pstHdlcCtrl->au8DecapBuf;

    ulFileLength = HSUART_MAKE_LONG(pu8Buf[START_DATA_FILELEN_OS], \
                            pu8Buf[START_DATA_FILELEN_OS+1],\
                            pu8Buf[START_DATA_FILELEN_OS+2],\
                            pu8Buf[START_DATA_FILELEN_OS+3]);

    if( ulFileLength >= BOOTLOAD_SIZE_MAX)
    {
        print_info("\r\nlen wrong" );
        (void)sendRepPkt(pstDlCtrl, REP_FILE_TOOBIG);
        return ERROR;
    }

    pstDlCtrl->ulFileLength = ulFileLength;

    pstDlCtrl->ulAddress = HSUART_MAKE_LONG(pu8Buf[START_DATA_ADDR_OS], \
                            pu8Buf[START_DATA_ADDR_OS+1],\
                            pu8Buf[START_DATA_ADDR_OS+2],\
                            pu8Buf[START_DATA_ADDR_OS+3]);

    (void)sendRepPkt(pstDlCtrl, REP_ACK);

    /* 包序号从0开始 */
    pstDlCtrl->u8SeqNext = 0;
    pstDlCtrl->ulPosition = pstDlCtrl->ulAddress;

    return OK;
}

/*****************************************************************************
* 函 数 名  : handerMidstDataPkt
*
* 功能描述  : 处理真正的文件包
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
INT32 handerMidstDataPkt( apDlCtrlS *pstDlCtrl )
{
    UINT32 ulPktLen;
    UINT8 *pu8Buf = pstDlCtrl->pstHdlcCtrl->au8DecapBuf;

    /* 帧序号检查 */
    if(pstDlCtrl->u8SeqNext != pu8Buf[MIDST_DATA_SEQ_OS])
    {
        /* 如果是重发的上一DATA帧，则丢弃 */
        if (pstDlCtrl->u8SeqNext == pu8Buf[MIDST_DATA_SEQ_OS] + 1)
        {
//print_info_with_u32("\r\nrecv a dup pkt, seq:", pstDlCtrl->u8SeqNext);
            (void)sendRepPkt(pstDlCtrl, REP_ACK);
            return OK;
        }
        else    /* 丢包,告诉AP从发所有的包,将之前的也舍弃 */
        {
            (void)sendRepPkt(pstDlCtrl, REP_SEQ_BIG);   /* AP收到REP_SEQ_BIG回应后,AP需要重传整个文件 */
            pstDlCtrl->u8SeqNext = 0;
            pstDlCtrl->ulPosition = pstDlCtrl->ulAddress;
            return ERROR;
        }
    }

    ulPktLen = HSUART_MAKE_WORD(pu8Buf[PKT_LEN_OS], pu8Buf[PKT_LEN_OS+1]);

    memcpy((void*)pstDlCtrl->ulPosition, (void*)&pu8Buf[MIDST_DATA_OS], ulPktLen);

    /* 更新指针 */
    pstDlCtrl->ulPosition += ulPktLen;
    pstDlCtrl->u8SeqNext++;

//print_info_with_u32("\r\nseq:", pu8Buf[MIDST_DATA_SEQ_OS]);
//print_info_with_u32("\r\npktLen:", ulPktLen);
//print_info_with_u32("\r\npktPos:", pstDlCtrl->ulPosition);
    /* 发送接收成功消息 */
    (void)sendRepPkt(pstDlCtrl, REP_ACK);

    return OK;
}

/*****************************************************************************
* 函 数 名  : handerEndDataPkt
*
* 功能描述  : 处理结束数据包,检验数据是否下载完整
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
INT32 handerEndDataPkt( apDlCtrlS *pstDlCtrl )
{
    /* 下载不完整,返回错误 */
    if(pstDlCtrl->ulPosition != pstDlCtrl->ulAddress + pstDlCtrl->ulFileLength)
    {
        (void)sendRepPkt(pstDlCtrl, REP_FILE_RECV_ERR);
        return ERROR;
    }

    /* 下载完整,返回ACK */
    pstDlCtrl->bComplete = TRUE;
    (void)sendRepPkt(pstDlCtrl, REP_ACK);
    return OK;
}

/*****************************************************************************
* 函 数 名  : handerExecDataPkt
*
* 功能描述  : 处理执行数据包
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
INT32 handerExecDataPkt( apDlCtrlS *pstDlCtrl )
{
    INT32 iRet;
    int BOOT_RST_Addr;

    if(!pstDlCtrl->bComplete)
    {
        (void)sendRepPkt(pstDlCtrl, REP_FILE_RECV_ERR);
        return ERROR;
    }

    iRet = apSecChk(pstDlCtrl->ulAddress);

    if(SEC_SUCCESS == iRet
        ||SEC_EFUSE_NOT_WRITE == iRet)
    {

#ifdef START_TIME_TEST
    print_info_with_u32("\r\ntime(ms):", (TIMER5_INIT_VALUE - INREG32(TIMER5_REGOFF_VALUE))/MS_TICKS);
#endif
        ocrShareSave();
        (void)sendRepPkt(pstDlCtrl, REP_ACK);
        /* 不再返回 */
        BOOT_RST_Addr = *(volatile UINT32 *)(pstDlCtrl->ulAddress + BOOT_RST_ADDR_OFFEST);
        go((FUNCPTR)(pstDlCtrl->ulAddress + BOOT_RST_Addr));
    }
    else
    {
        /* 发送校验失败,等待AP复位,不再返回 */
        sendRepErrWaitReset(pstDlCtrl, REP_SEC_CHK_ERR);
    }

    return ERROR;
}


/*****************************************************************************
* 函 数 名  : apDlMain
*
* 功能描述  : AP加载入口函数
*
* 输入参数  : ulFileAddr 存储文件的地址
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
apDlStatusE apDlMain( UINT32 ulFileAddr )
{
    INT32 iRet;
    UINT8 ucPktType;

    apDlCtrlS       stApDlCtrl;
    RECV_STR        stHsRecvStr;
    DLOAD_HDLC_S    stHdlcCtrl;

    apDlCtrlS *pstApDlCtrl      = &stApDlCtrl;
    pstApDlCtrl->pstHdlcCtrl    = &stHdlcCtrl;
    pstApDlCtrl->pstHsUartBuf   = &stHsRecvStr;

    apDlInit(pstApDlCtrl);

    /* OnChipRom V7R1 通过HSUART下载的BootLoader或烧写引导程序放在固定位置运行 */
    pstApDlCtrl->ulAddress = ulFileAddr;

    while(1)    /*lint !e716*/
    {
        iRet = apDlRecvPkt(pstApDlCtrl);
        switch(iRet)
        {
            case AP_DL_BUF_FULL:
                /* never return */
                sendRepErrWaitReset(pstApDlCtrl, REP_FILE_RECV_BUF_FULL);
                break;

            case AP_DL_RECV_TIMEOUT:
                /* never return */
                (void)sendRepPkt(pstApDlCtrl, REP_FILE_RECV_TIMEOUT);
                break;

            case AP_DL_ERR_OK:
                break;

            default:
                continue;
        }

        ucPktType = pstApDlCtrl->pstHdlcCtrl->au8DecapBuf[PKT_TYPE_OS];

        //print_info_with_u32("\r\nrecv a pkt, type:", ucPktType);

        switch(ucPktType)
        {
            /* 握手包,发送Ack */
            case PKT_TYPE_CONNECT:
                (void)sendRepPkt(pstApDlCtrl, REP_ACK);
                iRet = OK;

                break;

            case PKT_TYPE_BAUD_SET:
            {
                iRet = handerBaudSetPkt(pstApDlCtrl);

                break;
            }

            /* 执行指令,如果是从Flash直接启动命令,则不再返回 */
            case PKT_TYPE_AP_CMD:
            {
                iRet = handerApCmdPkt(pstApDlCtrl);

                break;
            }

            case PKT_TYPE_STAT_DATA:
            {
                iRet = handerStartDataPkt(pstApDlCtrl);

                break;
            }

            case PKT_TYPE_MIDST_DATA:
                iRet = handerMidstDataPkt(pstApDlCtrl);

                break;

            case PKT_TYPE_END_DATA:
                iRet = handerEndDataPkt(pstApDlCtrl);

                break;

                /* 执行命令,不再返回 */
            case PKT_TYPE_EXEC_DATA:
                iRet = handerExecDataPkt(pstApDlCtrl);

                break;

            default:
                /* 包类型错误,返回Nack */
                break;

        }

        if(OK != iRet)
        {
            print_info_with_u32("\r\nhanderPkt err: ", (UINT32)iRet);
            print_info_with_u32(",PktType:", (UINT32)ucPktType);
        }
    }

}

