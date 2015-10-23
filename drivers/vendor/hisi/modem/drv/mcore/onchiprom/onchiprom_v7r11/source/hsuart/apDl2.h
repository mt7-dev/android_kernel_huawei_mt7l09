/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  apDl2.h
*
*   作    者 :  wuzechun
*
*   描    述 :  apDl2.c 的头文件
*
*   修改记录 :  2011年7月15日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __APDL2_H__
#define __APDL2_H__

#include "hdlc.h"

/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/
#define AP_DL_CTRL_BUF_SIZE_SEND 32
#define AP_DL_CTRL_BUF_SIZE_RECV 1024

/* 数据传输时每一包有效数据的最大长度 */
#define DATA_PKT_LEN        512

/* 包长定义,包长为除去HDLC协议头/尾/CRC部分 */
#define PKT_LEN_CONNECT     3
#define PKT_LEN_BAUD_SET    7
#define PKT_LEN_AP_CMD      4
#define PKT_LEN_START_DATA  11
#define PKT_LEN_MIDST_DATA  (DATA_PKT_LEN + 4)
#define PKT_LEN_END_DATA    3
#define PKT_LEN_EXEC_DATA   3
#define PKT_LEN_REP         4

/* 包格式定义 */
/****************************************************************************
包格式: 包头 包类型    数据    FCS      包尾
            0x7E    1字节    N Bytes  2字节  0x7E

CONNECT:    0x7E    0x00        FCS    0x7E

波特率:  0x7E    0x10        波特率(4字节)    FCS    0x7E

操作码:  0x7E    0x11        指令(1字节)    FCS    0x7E

START_DATA: 0x7E    0x12        文件长度(4字节)    下载地址(4字节)    FCS    0x7E

MIDST_DATA: 0x7E    0x13    包长(2字节)    包序号(1字节)    数据(<512Bytes)    FCS    0x7E

END_DATA:   0x7E    0x14        FCS    0x7E

EXEC_DATA:  0x7E    0x15        FCS    0x7E 

应答包格式: 0x7E    0x01     错误码(1字节)    FCS    0x7E
******************************************************************************/
#define PKT_TYPE_OS     0
#define PKT_TYPE_LEN    1

#define BAUD_SET_OS     (PKT_TYPE_OS+PKT_TYPE_LEN)
#define AP_CMD_OS       (PKT_TYPE_OS+PKT_TYPE_LEN)

#define START_DATA_FILELEN_OS   (PKT_TYPE_OS+PKT_TYPE_LEN)
#define FILELEN_LEN     4
#define START_DATA_ADDR_OS   (START_DATA_FILELEN_OS+FILELEN_LEN)

#define PKT_LEN_OS      (PKT_TYPE_OS+PKT_TYPE_LEN)
#define PKT_LEN_LEN     2
#define MIDST_DATA_SEQ_OS   (PKT_LEN_OS+PKT_LEN_LEN)
#define MIDST_DATA_SEQ_LEN  1
#define MIDST_DATA_OS       (MIDST_DATA_SEQ_OS+MIDST_DATA_SEQ_LEN)

#define REP_CODE_OS     (PKT_TYPE_OS+PKT_TYPE_LEN)

/*--------------------------------------------------------------*
 * 数据结构                                                 *
 *--------------------------------------------------------------*/

/* 包类型定义 */
typedef enum _tagPktTypeE
{
    PKT_TYPE_CONNECT    = 0x00,
    PKT_TYPE_REP        = 0x01,
    
    PKT_TYPE_BAUD_SET   = 0x10,
    PKT_TYPE_AP_CMD     = 0x11,
    PKT_TYPE_STAT_DATA  = 0x12,
    PKT_TYPE_MIDST_DATA = 0x13,
    PKT_TYPE_END_DATA   = 0x14,
    PKT_TYPE_EXEC_DATA  = 0x15,
    PKT_TYPE_MAX
}pktTypeE;

/* 指令包指令定义 */
typedef enum _tagApCmdE
{
    AP_CMD_DL_BL_BURN_PRE = 0x20,   /* 下载BootLoader或烧写引导程序 */
    AP_CMD_NAND_BOOT = 0x21,        /* 直接从Nand启动 */
    AP_CMD_EMMC_BOOT = 0x22,         /* 直接从MMC设备(eMMC/MMC/SD)启动 */
    AP_CMD_HSIC_BOOT = 0x23          /* 直接从SPI设备(EEPROM/SFlash)启动 */
}apCmdE;

/* 应答包错误码定义 */
typedef enum _tagRepCodeE
{
    REP_ACK = 0x80,
    REP_BAUD_NOT_SUPPORT,
    REP_AP_CMD_NOT_SUPPORT,
    REP_NAND_READ_ERR,
    REP_SEQ_BIG,
    REP_FILE_TOOBIG,
    REP_FILE_RECV_ERR,
    REP_FILE_RECV_BUF_FULL,
    REP_FILE_RECV_TIMEOUT,
    REP_SEC_CHK_ERR
}RepCodeE;

/* 下载交互流程错误码定义 */
typedef enum _tagApDlStatusE
{
    AP_DL_ERR_OK = 0,
    AP_DL_ERR_HDLC_ENCAP,
    AP_DL_ERR_HDLC_DECAP,
    AP_DL_ERR_RECV,
    AP_DL_BUF_FULL,
    AP_DL_RECV_TIMEOUT,
    AP_DL_ERR_MAX
}apDlStatusE;

typedef struct _tagApDlCtrlS
{
    UINT32  ulAddress;      /* 数据存放的地址 */
    UINT32  ulFileLength;   /* 预期应该收到数据的长度 */
    UINT32  ulPosition;     /* 已经收到数据的数目 */
    UINT8   u8SeqNext;      /* 下一个预期收到的包号 */
    BOOL    bComplete;      /* 当前帧是否传输完成 */
    
    UINT16  u16SendLen;
    UINT16  u16RecvLen;

    RECV_STR *pstHsUartBuf;

    DLOAD_HDLC_S *pstHdlcCtrl;
}apDlCtrlS;

/*--------------------------------------------------------------*
 * 函数原型声明                                                 *
 *--------------------------------------------------------------*/
apDlStatusE apDlMain( UINT32 ulFileAddr );

#endif /* apDl2.h */

