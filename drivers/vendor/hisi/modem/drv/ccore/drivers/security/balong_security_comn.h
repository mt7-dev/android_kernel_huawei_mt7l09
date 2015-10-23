#ifndef _BALONG_SECURY_COMN_H_
#define _BALONG_SECURY_COMN_H_

#include <bsp_om.h>
#define  security_print(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "[SECURITY]: <%s> "fmt"\n", __FUNCTION__, ##__VA_ARGS__))
typedef enum
{
    IMG_SEC_OK = 0,               /*镜像校验通过*/
    IMG_NOT_FOUND,                /*没有找到镜像*/
    IMG_IS_UNSEC_TYPE,            /*镜像是非安全的*/
    IMG_READ_FLASH_ERR,           /*读取Flash出错*/
    IMG_FILE_CAN_NOT_OPEN,        /*文件不能打开*/
    IMG_FILE_CAN_NOT_SEEK,        /*文件不能移动指针*/
    IMG_FILE_CAN_NOT_FTELL,       /*获取文件长度失败*/
    IMG_FILE_READ_ERR,            /*读取文件内容出错*/

    IMG_MEM_ERR,                  /*内存错误*/
    IMG_LEN_ERR,                  /*镜像长度错误*/
    IMG_READ_OTP_ERR,             /*读取OTP出错*/
    IMG_OTP_ID_NOT_EQUAL,         /*OTP ID不匹配*/
    
    IMG_SHA_ERR,                  /*镜像SHA计算错误*/
    IMG_RSA_ERR,                  /*镜像RSA机密错误*/
    IMG_SHA_RSA_NOT_EQUAL,        /*根据源数据计算的SHA与RSA解密的SHA值不一致*/

    IMG_CA_SHA_ERR,               /*镜像CA SHA计算出错*/
    IMG_CA_RSA_ERR,               /*镜像CA 计算RSA出错*/
    IMG_CA_SHA_RSA_NOT_EQUAL,     /*镜像CA的SHA和RSA不匹配*/

    IMG_IDIO_SHA_ERR,             /*镜像数据计算SHA出错*/
    IMG_IDIO_RSA_ERR,             /*镜像数据计算RSA出错*/
    IMG_IDIO_SHA_RSA_NOT_EQUAL,   /*镜像数据的RSA和SHA不匹配*/

    IMG_CARDLOCK_READ_ERR,        /*读取CardLock NV错误*/
    IMG_IMEI_READ_ERR,            /*从NV项读取IMEI错误*/
    IMG_IMEI_READ_OTP_ERR,        /*读取OTP里面IMEI的SNR出错*/
    IMG_IMEI_SNR_NOT_EQUAL,       /*OTP里面IMEI的SNR和NV计算出来的SNR不匹配*/ 

	CRYPTO_PTR_INVALID,           /*指针无效*/
	CRYPTO_NV_READ_ERR,           /*读取NV项失败*/
	CRYPTO_EFUSE_READ_ERR,        /*efuse读失败*/
	CRYPTO_PARAM_INVALID,         /*参数不正确*/
	CRYPTO_FASTBOOT_BACKUP_CA_ERR /*fastboot备份的ROOT CA错误*/
}IMG_SECCHK_RESULT;

#define SECURITY_BASE_ERR_CODE     0x10000000

#define SECURITY_SHA_ERR                 (SECURITY_BASE_ERR_CODE | IMG_SHA_ERR)
#define SECURITY_IMEI_NV_READ_ERR        (SECURITY_BASE_ERR_CODE | IMG_IMEI_READ_ERR)
#define SECURITY_PTR_NULL                (SECURITY_BASE_ERR_CODE | CRYPTO_PTR_INVALID)
#define SECURITY_NV_READ_ERR             (SECURITY_BASE_ERR_CODE | CRYPTO_NV_READ_ERR)
#define SECURITY_EFUSE_READ_ERR          (SECURITY_BASE_ERR_CODE | CRYPTO_EFUSE_READ_ERR)
#define SECURITY_IN_PARAM_ERR            (SECURITY_BASE_ERR_CODE | CRYPTO_PARAM_INVALID)
#define SECURITY_FASTBOOT_BACKUPCA_ERR   (SECURITY_BASE_ERR_CODE | CRYPTO_FASTBOOT_BACKUP_CA_ERR)

#endif
