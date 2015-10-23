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

/*--------------------------------------------------------------*
 * 数据结构                                                     *
 *--------------------------------------------------------------*/

/* 公钥格式 */
typedef struct
{
    UINT16  eLen;    /* E长度以64bits为单位，取值范围为0~15，0表示16 */
    UINT16  nLen;    /* N长度以32bits为单位，取值范围为0~31，0表示32 */
    UINT32  e[MAX_N_LENGTH];       /* 幂e(公钥) */
    UINT32  n[MAX_N_LENGTH];       /* 模数 */
}KEY_STRUCT;


/* 签名校验函数指针 */
typedef int (*idioIdentifyPtr)(UINT32 dataAddr, UINT32 dataLen, KEY_STRUCT *pKey, UINT32* pIdio);

/* 计算Hash值函数指针 */
typedef int (*SHA256HashPtr)(UINT32 dataAddr, UINT32 dataLen, UINT32* hash);

/* 计算RSA函数指针 */
typedef int (*RSAPtr)(KEY_STRUCT *pKey, UINT32* pIndata, UINT32* pOutdata);

/* 共享数据结构体,放在AXI顶部,可以往下加元素(即添加在前面) */
typedef struct tagOcrShareData
{
    INT32 errno;

    /* 共享函数指针 */
    SHA256HashPtr     SHA256Hash;
    RSAPtr          RSA;
    idioIdentifyPtr idioIdentify;

    /* 共享数据段 */
    BOOL    bSecEn;
    BOOL    bRootCaInited;
    KEY_STRUCT *pRootKey;
    UINT32  ulEnumFlag;             /* 自举置位标志,AXI Mem Top - 8 */
    UINT32  ulOcrInitedFlag;        /* AXI Mem Top - 4 */
}tOcrShareData;                  /* AXI Mem Top */

int SHA256Hash(UINT32 dataAddr, UINT32 dataLen, UINT32* hash);

int RSA(KEY_STRUCT *pubKey, UINT32* pIndata, UINT32* pOutdata);

void ocrShareSave( void );

#endif /* OcrShare.h */


