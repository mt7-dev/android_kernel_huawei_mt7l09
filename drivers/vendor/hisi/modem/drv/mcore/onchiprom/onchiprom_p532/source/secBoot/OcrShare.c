/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  OcrShare.c
*
*   作    者 :  wuzechun
*
*   描    述 :  保存共享代码段
*
*   修改记录 :  2011年7月18日  v1.00  wuzechun  创建
*
*************************************************************************/

#include "config.h"
#include "secBoot.h"
#include "OcrShare.h"
#include "string.h"
#include "cipher.h"
#include "nnex.h"
#include "sha2.h"


/******************************************************************************
*  Function:  SHA256Hash
*  Description:
*      SHA256.
*
*
*  Called By:
*      identify
*
*  Input:
*         dataAddr: 输入数据的地址
*         dataAddr: 输入数据的长度(字节数)
*
*  Output:
*         hash: 指向输入数据进行SHA256计算后的哈希值的指针(哈希值的长度为32Byte)
*
*  Return:
*         OK: the function successful returned.
*         !OK: the function encounter OSAL_ERROR while running.
*
********************************************************************************/
int SHA256Hash(UINT32 dataAddr, UINT32 dataLen, UINT32* hash)
{
    UINT32 length = 0;
    UINT32 hmacKey[SHA256_HASH_SIZE];

    memset((void*)hmacKey, 0, sizeof(hmacKey));
    memset((void*)hash, 0, sizeof(hmacKey));

#ifdef CIPHER
    return bsp_calc_hash((UINT32*)dataAddr, dataLen, hmacKey, hash, &length);
#else
    while (dataLen >= 512)
    {
        memcpy(hmacKey, hash, sizeof(hmacKey));
        sha2_hmac((unsigned char*)hmacKey, sizeof(hmacKey), (unsigned char*)dataAddr, 512, (unsigned char *)hash, 0);
        dataLen -= 512;
        dataAddr += 512;
    }
    if (dataLen)
    {
        memcpy(hmacKey, hash, sizeof(hmacKey));
        sha2_hmac((unsigned char*)hmacKey, sizeof(hmacKey), (unsigned char*)dataAddr, dataLen, (unsigned char *)hash, 0);
    }
    return OK;
#endif
}

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
*         OK: the function successful returned.
*         !OK: the function encounter OSAL_ERROR while running.
*
********************************************************************************/
int RSA(KEY_STRUCT *pubKey, UINT32* pIndata, UINT32* pOutdata)
{
    if ((NULL == pubKey) || (NULL == pIndata) || (NULL == pOutdata))
        return !OK;

    NN_ModExpex(pOutdata, pIndata, pubKey->e, MAX_N_LENGTH, pubKey->n, MAX_N_LENGTH);

    return OK;
}

/*****************************************************************************
* 函 数 名  : ocrShareSave
*
* 功能描述  : 保存共享代码段
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void ocrShareSave( void )
{
    volatile tOcrShareData *pShareData = (tOcrShareData*)M3_SRAM_SHARE_DATA_ADDR;

    /* 保存共享给BootLoader的函数指针,不管是否使能安全,这些校验函数都可给上层使用 */
    pShareData->idioIdentify = (idioIdentifyPtr)idioIdentify;
    pShareData->SHA256Hash = (SHA256HashPtr)SHA256Hash;
    pShareData->RSA = (RSAPtr)RSA;
    pShareData->ulOcrInitedFlag = OCR_INITED_FLAG_VALUE;
	pShareData->bsp_nand_get_spec_and_save = (bsp_nand_get_spec_and_save_ptr)bsp_nand_get_spec_and_save;	
}

