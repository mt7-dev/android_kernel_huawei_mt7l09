

#ifndef __SOC_ONCHIP_H__
#define __SOC_ONCHIP_H__

#include "bsp_memmap.h"

/*--------------------------------------------------------------*
 * 宏定义                                                    *
 *--------------------------------------------------------------*/

#define M3_TCM_ADDR                    (0x10000000)	 /* TCM起始地址 */
#define M3_TCM_SIZE                    (64*1024)     /* 64K */

#define SRAM_ADDR                      (0x4FE00000)     /* AXI Memory起始地址 */
#define SRAM_SIZE                      (128*1024)       /* AXI Memory大小 */

/* 共享数据区放在TCM顶端,BootLoader启动后需要拷贝走 */
#define M3_TCM_SHARE_DATA_SIZE sizeof(tOcrShareData)
#define M3_TCM_SHARE_DATA_ADDR (SRAM_ADDR + SRAM_SIZE - M3_TCM_SHARE_DATA_SIZE)

/* OnChipRom运行过标志和自举标志都在tOcrShareData结构体内 */
#define OCR_INITED_FLAG_ADDR    (SRAM_ADDR + SRAM_SIZE - 4)
#define OCR_AUTO_ENUM_FLAG_ADDR (SRAM_ADDR + SRAM_SIZE - 8)
#define OCR_INITED_FLAG_VALUE   (0x23456789)
#define OCR_UNINITED_FLAG_VALUE (0xA55A6789)


#define AUTO_ENUM_FLAG_VALUE    0x82822828

/* 只有OEM CA中才有OEM ID和HWID,根CA中无 */
#define KEY_OEMID_INDEX        sizeof(KEY_STRUCT)
#define KEY_HWID_INDEX         (KEY_OEMID_INDEX+0x04)

#define BL_LEN_INDEX            (144 * 4)    /* bootload.bin文件长度(Byte)保存在bootload.bin文件中的偏移字节数 */
#define ROOT_CA_INDEX           (BL_LEN_INDEX + 4)   /* 根CA在Image中位置 */

#define ROOT_CA_LEN            260      /*CA证书的长度 260 Byte*/
#define OEM_CA_LEN             268      /*CA证书的长度 268 Byte*/
#define IDIO_LEN               128      /*签名的长度 128 Byte*/

#define MAX_N_LENGTH    32

#define SHA256_HASH_SIZE  8       /*HASH签名占的word数*/

#define MD5_HASH_SIZE   4       /* MD5摘要所占的word数 */

#ifndef __ASSEMBLY__
/*--------------------------------------------------------------*
 * 数据结构                                                     *
 *--------------------------------------------------------------*/

/* 公钥格式 */
typedef struct
{
    unsigned short eLen;    /* E长度以64bits为单位，取值范围为0~15，0表示16 */
    unsigned short nLen;    /* N长度以32bits为单位，取值范围为0~31，0表示32 */
    unsigned int   e[MAX_N_LENGTH];       /* 幂e(公钥) */
    unsigned int   n[MAX_N_LENGTH];       /* 模数 */
}KEY_STRUCT;


/* 签名校验函数指针 */
typedef int (*idioIdentifyPtr)(unsigned int dataAddr, unsigned int dataLen, KEY_STRUCT *pKey, unsigned int* pIdio);

/* 计算Hash值函数指针 */
typedef int (*SHA256HashPtr)(unsigned int dataAddr, unsigned int dataLen, unsigned int* hash);

/* 计算RSA函数指针 */
typedef int (*RSAPtr)(KEY_STRUCT *pKey, unsigned int* pIndata, unsigned int* pOutdata);

/* 共享数据结构体,放在AXI顶部,可以往下加元素(即添加在前面) */
typedef struct tagOcrShareData
{
}tOcrShareData;
#endif /* __ASSEMBLY__ */

#endif
