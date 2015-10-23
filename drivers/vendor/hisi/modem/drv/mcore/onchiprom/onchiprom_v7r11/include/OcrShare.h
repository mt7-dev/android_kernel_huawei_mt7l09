/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  OcrShare.h
*
*   作    者 :  wuzechun
*
*   描    述 :  OnChipRom与BootLoader共享数据区
*
*   修改记录 :  2011年4月15日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __OCRSHARE_H__
#define __OCRSHARE_H__

#include "types.h"
#include "config.h"

/*--------------------------------------------------------------*
 * 宏定义                                                    *
 *--------------------------------------------------------------*/

/* 只有OEM CA中才有OEM ID和HWID,根CA中无 */
#define KEY_OEMID_INDEX        sizeof(KEY_STRUCT)
#define KEY_HWID_INDEX         (KEY_OEMID_INDEX+0x04)

#define ROOT_CA_LEN            260      /*CA证书的长度 260 Byte*/
#define OEM_CA_LEN             268      /*CA证书的长度 268 Byte*/
#define IDIO_LEN               128      /*签名的长度 128 Byte*/

#define MAX_N_LENGTH    32

#define SHA256_HASH_SIZE  8       /*HASH签名占的word数*/

#define MD5_HASH_SIZE   4       /* MD5摘要所占的word数 */

#define NAND_ARGS_FULL_AVAIL	0x3a
#define NAND_ARGS_PART_AVAIL	0x45

/*--------------------------------------------------------------*
 * 数据结构                                                     *
 *--------------------------------------------------------------*/

/* 公钥格式 */
typedef struct
{
    unsigned short  eLen;    /* E长度以64bits为单位，取值范围为0~15，0表示16 */
    unsigned short  nLen;    /* N长度以32bits为单位，取值范围为0~31，0表示32 */
    unsigned long  e[MAX_N_LENGTH];       /* 幂e(公钥) */
    unsigned long  n[MAX_N_LENGTH];       /* 模数 */
}KEY_STRUCT;


/* nand spec info，每个参数的取值都是真实的值，如page_size可取2048、4096等，而非b01\b11等 */
struct nand_spec_real
{
	unsigned long page_size;
	unsigned long page_num_per_block;
	unsigned long addr_cycle;
	unsigned long ecc_type;
	unsigned long oob_size;
	unsigned long chip_size;	/* 以MB为单位 */
};

/* onchiprom里把nand spec info保存到SRAM的一个32bit的buffer中，下面是每个字段的含义 */
struct nand_spec_sram
{
	/* pagesize/ecctype/addrnum/pagenumperblock 这几个参数与寄存器中配置值一致 */
    unsigned int    page_size          : 4;  /* [0~3]   */
    unsigned int    ecc_type           : 4;  /* [4~7]   */
    unsigned int    addr_num           : 4;  /* [8~11]  */
    unsigned int    page_num_per_block : 4;  /* [12~15] */

	/* sparesize/chipsize这两个参数可能获取不到 */
	unsigned int    spare_size		   : 5;  /* [16~20] */
	unsigned int    chip_size		   : 4;	 /* [21~24] */	

	/* flag */
    unsigned int    flag           	   : 7;  /* [25~31] NAND_ARGS_FULL_AVAIL - 传全部参数，NAND_ARGS_FULL_AVAIL - 传4个参数，else - buffer值无效 */
};

/* 签名校验函数指针 */
typedef int (*idioIdentifyPtr)(unsigned long dataAddr, unsigned long dataLen, KEY_STRUCT *pKey, unsigned long* pIdio);

/* 计算Hash值函数指针 */
typedef int (*SHA256HashPtr)(unsigned long dataAddr, unsigned long dataLen, unsigned long* hash);

/* 计算RSA函数指针 */
typedef int (*RSAPtr)(KEY_STRUCT *pKey, unsigned long* pIndata, unsigned long* pOutdata);

/* 获取nand flash规格的函数指针，给先贴后烧用 */
typedef int (*bsp_nand_get_spec_and_save_ptr)(volatile struct nand_spec_sram *spec_sram);

/* 共享数据结构体,放在AXI顶部,可以往下加元素(即添加在前面) */
typedef struct tagOcrShareData
{
	struct nand_spec_sram nandspec;
	bsp_nand_get_spec_and_save_ptr bsp_nand_get_spec_and_save;	/* 共享函数指针 */

    long errno;
	
    /* 共享函数指针 */
    SHA256HashPtr     SHA256Hash;
    RSAPtr          RSA;
    idioIdentifyPtr idioIdentify;	

    /* 共享数据段 */
    BOOL    bSecEn;
    BOOL    bRootCaInited;
    KEY_STRUCT *pRootKey;

    unsigned long  ulEnumFlag;             /* 自举置位标志,AXI Mem Top - 8 */
    unsigned long  ulOcrInitedFlag;        /* AXI Mem Top - 4 */
}tOcrShareData;                  /* AXI Mem Top */

int SHA256Hash(unsigned long dataAddr, unsigned long dataLen, unsigned long* hash);

int RSA(KEY_STRUCT *pubKey, unsigned long* pIndata, unsigned long* pOutdata);

void ocrShareSave( void );

int bsp_nand_get_spec_and_save(volatile struct nand_spec_sram *spec_sram);

#endif /* OcrShare.h */


