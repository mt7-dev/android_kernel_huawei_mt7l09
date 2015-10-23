

#ifndef _SSDMA_H_
#define _SSDMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "soc_onchiprom.h"

#define SSDMA_OK        0
#define SSDMA_ERROR     1

#define SOFT_ARM_LOCK   0x2
#define SOFT_DSP_LOCK   0x1

#define SOFT_PUBKEY_USE     0
#define SOFT_EKEY_USE       1
#define SOFT_INTERKEY_USE   2

#define SOFT_AES_TARGET     0
#define SOFT_DES1_TARGET    1
#define SOFT_DES2_TARGET    2
#define SOFT_DES3_TARGET    3
#define SOFT_ERSA_TARGET    4
#define SOFT_NRSA_TARGET    5

#define SOFT_AES_KEY_128_LENGTH     0
#define SOFT_AES_KEY_192_LENGTH     1
#define SOFT_AES_KEY_256_LENGTH     2

#define SOFT_AES_ECB_MODE           0
#define SOFT_AES_CBC_MODE           1
#define SOFT_AES_CTR_MODE           2

#define SOFT_USE_ROOTKEY0   0
#define SOFT_USE_ROOTKEY1   1
#define SOFT_USE_ROOTKEY2   2
#define SOFT_USE_ROOTKEY3   3
#define SOFT_USE_ROOTKEY4   4
#define SOFT_USE_ROOTKEY5   5
#define SOFT_USE_ROOTKEY6   6
#define SOFT_USE_ROOTKEY7   7

#define SOFT_AES_SEG_LENGTH     0x10
#define SOFT_DES_SEG_LENGTH     0x8
#define SOFT_SHA1_SEG_LENGTH    0x40
#define SOFT_MD5_SEG_LENGTH     0x40
#define SOFT_RSA_SEG_LENGTH     0x80

#define SOFT_CBC_IP_MODE        0x1
#define SOFT_AES_IP_MODE        0x2
#define SOFT_DES_IP_MODE        0x4
#define SOFT_SHA1_IP_MODE       0x8
#define SOFT_MD5_IP_MODE        0x10
#define SOFT_RSA_IP_MODE        0x20

#define DATA_LARGE  1
#define DATA_SMALL  -1
#define DATA_EQUAL  0

/* register control */
#define READ_REGISTER_UCHAR(addr) (*(volatile UINT8 *)(addr))
#define READ_REGISTER_USHORT(addr) (*(volatile UINT16 *)(addr))
#define READ_REGISTER_ULONG(addr) (*(volatile UINT32 *)(addr))

#define WRITE_REGISTER_UCHAR(addr,val) (*(volatile UINT8 *)(addr) = (val))
#define WRITE_REGISTER_USHORT(addr,val) (*(volatile UINT16 *)(addr) = (val))
#define WRITE_REGISTER_ULONG(addr,val) (*(volatile UINT32 *)(addr) = (val))

#define INREG8(x)           READ_REGISTER_UCHAR(x)
#define OUTREG8(x, y)       WRITE_REGISTER_UCHAR(x, (UINT8)(y))
#define SETREG8(x, y)       OUTREG8(x, INREG8(x)|(y))
#define CLRREG8(x, y)       OUTREG8(x, INREG8(x)&~(y))
#define SETBITVALUE8(addr, mask, value) \
                OUTREG8(addr, (INREG8(addr)&(~mask)) | (value&mask))

#define INREG16(x)          READ_REGISTER_USHORT(x)
#define OUTREG16(x, y)      WRITE_REGISTER_USHORT(x,(UINT16)(y))
#define SETREG16(x, y)      OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)      OUTREG16(x, INREG16(x)&~(y))
#define SETBITVALUE16(addr, mask, value) \
                OUTREG16(addr, (INREG16(addr)&(~mask)) | (value&mask))

#define INREG32(x)          READ_REGISTER_ULONG(x)
#define OUTREG32(x, y)      WRITE_REGISTER_ULONG(x, (UINT32)(y))
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))
#define SETBITVALUE32(addr, mask, value) \
                OUTREG32(addr, (INREG32(addr)&(~mask)) | (value&mask))

#define MAX_N_LENGTH    32
#define SHA1_BLOCK_SIZE 64      /*每次SHA1运算输入数据的字节数*/
#define SHA1_HASH_SIZE  5       /*HASH签名占的word数*/
#define SHA1_PAD_SIZE   8       /*SHA1运算输入数据的PAD所占的字节数*/

#define WORD_SIZE   (sizeof(unsigned int)/sizeof(char))

#define TIMEMAXDELAY    0x1000000       /*最大延迟时间*/

/*ssdma aes*/
typedef enum
{
    CRYPTO_ENCRYPT_MODE = 0,
    CRYPTO_DECRYPT_MODE     

}CRYPTO_ENCRYPT_DECRYPT_SELECT;

#define  AES_BLOCK_SIZE   16

/*********************************************************************************************  
Function:           SsdmaSha1Init
Description:        SHA1模块初始化
*********************************************************************************************/
int SsdmaSha1Init(void);

/*********************************************************************************************  
Function:           SetSsdmaSha1Iv
Description:        设置SHA1模块的iv
*********************************************************************************************/
int SetSsdmaSha1Iv(UINT32 *iv);

/*********************************************************************************************  
Function:           RunSsdmaSha1
Description:        执行SHA1算法
*********************************************************************************************/
int RunSsdmaSha1(UINT32 *din, UINT32 *ivout);

/*********************************************************************************************  
Function:           SsdmaRsaInit
Description:        RSA模块初始化
*********************************************************************************************/
int SsdmaRsaInit(void);

/*********************************************************************************************  
Function:           SetSsdmaRsaMode
Description:        设置RSA模块的工作模式，n密钥长度，e密钥长度
*********************************************************************************************/
int SetSsdmaRsaMode(int isExp, UINT32 nLength, UINT32 eLength);

/*********************************************************************************************  
Function:           RunSsdmaRsa
Description:        执行RSA算法
*********************************************************************************************/
int RunSsdmaRsa(UINT32 *c, UINT32 *m, UINT32 *r);

/******************************************************************************
*  Function:  SHA1Hash
*  Description:
*      SHA1.
*
*  Calls:
*      SsdmaSha1Init, SetSsdmaSha1Iv, RunSsdmaSha1, byteCopy 
*
*  Called By:
*      identify
*
*  Input:
*         dataAddr: 输入数据的地址
*         dataAddr: 输入数据的长度(字节数)
*
*  Output:
*         hash: 指向输入数据进行SHA1计算后的哈希值的指针(哈希值的长度为20Byte)
*
*  Return:
*         SSDMA_OK: the function successful returned.
*         SSDMA_ERROR: the function encounter OSAL_ERROR while running.
*
********************************************************************************/
int SHA1Hash(char* dataPtr, unsigned int dataLen, unsigned int* hash);



/******************************************************************************
*  Function:  RSA
*  Description:
*      RSA.
*
*  Calls:
*      SsdmaRsaInit, SetSsdmaRsaMode
*
*  Called By:
*      identify
*
*  Input:
*         pubKey: 密钥
*         pIndata: 输入数据的指针(输入数据的长度不超过128Byte)
*
*  Output:
*         pOutdata: 输出数据的指针(输出数据的长度不超过128Byte)
*
*  Return:
*         SSDMA_OK: the function successful returned.
*         SSDMA_ERROR: the function encounter OSAL_ERROR while running.
*
********************************************************************************/
int RSA(KEY_STRUCT* pubKey, UINT32* pIndata, UINT32* pOutdata);

#ifdef __cplusplus
}
#endif

#endif  /*_SSDMA_H_*/

