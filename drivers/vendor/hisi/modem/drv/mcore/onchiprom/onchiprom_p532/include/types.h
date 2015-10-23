/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  types.h
*
*   作    者 :  wuzechun
*
*   描    述 :  数据类型定义头文件
*
*   修改记录 :  2011年4月22日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/

typedef char            INT8;
typedef short           INT16;
typedef long            INT32;
typedef long long       INT64;
typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned long   UINT32;
typedef unsigned long long    UINT64;

#define PUCHAR  UINT8*
#define NULL    (void *)0
#define VOID    void

#define BOOL    int
#define STATUS  int

#define ERROR         -1
#define OK            0

#define TRUE 1
#define FALSE 0


typedef void         (*FUNCPTR)();


/* 归一化错误码定义 */
#define ERR_OK                  0
#define ERR_NO_BASE             0x10000000

#define ERR_SEC_BASE            (ERR_NO_BASE + 0)
#define ERR_NAND_BASE           (ERR_NO_BASE + 0x1000)
#define ERR_SPI_BASE            (ERR_NO_BASE + 0x2000)
#define ERR_HSUART_BASE         (ERR_NO_BASE + 0x3000)
#define ERR_USB_BASE            (ERR_NO_BASE + 0x4000)
#define ERR_MMC_BASE            (ERR_NO_BASE + 0x5000)
#define ERR_SYS_BASE            (ERR_NO_BASE + 0x9000)

/* 安全校验模块错误码定义 */
#define SEC_SUCCESS             0
#define SEC_EFUSE_READ_ERROR    (ERR_SEC_BASE + 1)                /*EFUSE 读取ERROR*/
#define SEC_EFUSE_NOT_WRITE     (ERR_SEC_BASE + 2)                     /*EFUSE 未烧写*/
#define SEC_OEMCA_ERROR         (ERR_SEC_BASE + 3)                          /*CA ERROR*/
#define SEC_IMAGE_ERROR         (ERR_SEC_BASE + 4)                  /*映像ERROR*/
#define SEC_ROOT_CA_ERROR       (ERR_SEC_BASE + 5)            /* 根CA校验错误 */
#define SEC_IMAGE_LEN_ERROR     (ERR_SEC_BASE + 6)            /*安全版本映象长度错误*/
#define SEC_SHA_CALC_ERROR      (ERR_SEC_BASE + 7)            /* SHA计算错误 */
#define SEC_RSA_CALC_ERROR      (ERR_SEC_BASE + 8)            /* RSA计算错误 */
#define SEC_NO_IMAGE            (ERR_SEC_BASE + 9)          /* 没有烧写BootLoader镜像 */
#define NAND_NO_IN_BOOTMODE     (ERR_SEC_BASE + 10)          /* Nand处于非Boot模式,需要重启单板进入Boot模式*/

/* NANDC模块错误码定义 */
#define NAND_ECC_ERR            (ERR_NAND_BASE + 1)             /* Nand Flash产生Ecc不可纠正的错误 */
#define NAND_SPEC_FAIL			(ERR_NAND_BASE + 2)				/* Nand获取spec info失败 */
#define NAND_EFUSE_FAIL			(ERR_NAND_BASE + 3)				/* 从efuse中获取spec info失败 */
#define NAND_READID_FAIL		(ERR_NAND_BASE + 4)				/* 读nand id失败 */
#define NAND_CRC_ERR			(ERR_NAND_BASE + 5)				/* ONFI param crc校验错误 */
#define NAND_ONFI_ERR			(ERR_NAND_BASE + 6)				/* 读出的ONFI id不是"ONFI" */
#define NAND_ID_NOT_MATCH		(ERR_NAND_BASE + 7)				/* 两次读id值不匹配 */
#define NAND_READSIZE_TOO_LARGE	(ERR_NAND_BASE + 8)				/* 读buffer长度超过nandc buffer大小 */
#define NAND_RDBUF_FAIL			(ERR_NAND_BASE + 9)				/* 读nandc buffer failed */
#define NAND_WAIT_TIMEOUT		(ERR_NAND_BASE + 10)			/* 等待结果超时 */
#define NAND_EFUSE_ID_ERR		(ERR_NAND_BASE + 11)			/* efuse group id不合法，目前只支持0/1两个值 */
#define NAND_ECC_MODE_ERR		(ERR_NAND_BASE + 12)			/* boot模式下使用了ecc0配置 */

/* SPI模块错误码定义 */
typedef enum _tagSpiReadStatus_t
{
    SPI_READ_OK = 0,
    SPI_READ_ERR_PARA = (ERR_SPI_BASE+1),
    SPI_READ_ERR_RECV,
    SPI_READ_ERR_GETSTATUS,
    SPI_READ_ERR_READ_LENBYTE,
    SPI_READ_ERR_READ_BL,
    SPI_READ_ERR_BL_LEN
}spiReadStatus;

/* AP HSUART模块错误码定义 */
#define ERR_AP_HSUART_TRY_ALL_FAILED    (ERR_HSUART_BASE + 1)

/* 系统错误码定义 */
typedef enum _tagSYS_ERRNO_E
{
    SYS_ERR_USB_BOOT_RETURNS = (ERR_SYS_BASE+1),
    SYS_ERR_AP_DL_RETURNS,
    SYS_ERR_EFUSE_READ,
    SYS_ERR_BOOT_MODE,
    SYS_ERR_HSIC_BOOT_RETURNS,
    SYS_ERR_SEC_UNKNOWN_RET
}SYS_ERRNO_E;

typedef enum _tagDELAY_TIME_MS
{
    TIME_DELAY_MS_1000_FOR_UBOOT_RETURN  = 1000,
    TIME_DELAY_MS_2000_FOR_EFUSE_READERR = 2000,
    TIME_DELAY_MS_3000_FOR_BOOT_MODE     = 3000,
    TIME_DELAY_MS_4000_FOR_AP_RETURNS    = 4000,
    TIME_DELAY_MS_5000_FOR_SEC_UNKNOWN_RET       = 5000,
    TIME_DELAY_MS_6000_FOR_NF_OPBOOT = 6000,
}DELAY_TIME;

#endif /* types.h */

