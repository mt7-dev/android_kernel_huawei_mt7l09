/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  security.c
*
*   作    者 :  wuzechun
*
*   描    述 :  完成安全校验镜像的功能,可以使用
*                 OnChipRom共享的代码,在不使用OnChipRom的情况
*                 下,也可以使用单独的驱动.
*
*   修改记录 :  2011年8月10日  v1.00  wuzechun  创建
*
*************************************************************************/

/*#include <vxworks.h>*/
#include <string.h>
#include <semLib.h>
#include <stdio.h>
#include <taskLib.h>
#include "DrvInterface.h"
#include "ssdma.h"
#include "security2.h"
#include "cipher_aes.h"
#include "rsa.h"
#include "logLib.h"
#include "hash_md5.h"
#include "ran.h"
#include "sha2.h"
#include "drv_crypto.h"
#include "drv_efuse.h"
#include "bsp_om.h"
#include "balong_security_comn.h"
#include "efuse_balong.h"
#include "hi_efuse.h"

#define EFUSE_HUK_SIZE          (4)
#define EFUSE_GROUP_SIZE        (4)

#ifdef HI_K3_EFUSE
#define EFUSE_GRP_HUK           (28)
#else
#define EFUSE_GRP_HUK           (4)
#endif

//extern int efuse_read(u32 group, u32* pBuf, u32 num);


const BSP_U32 sha1_in_key[] = {0xC3D2E1F0, 0x10325476, 0x98BADCFE, 0xEFCDAB89, 
	                                                 0x67452301, 0x67452301, 0x67452301, 0x67452301};
/*
 FIXME: too many params in each function, change to use struct.
 FIXME: use uint instead int for len/size params.
 FIXME: change char to void*
*/


int param_check(char *data, int len, CREATE_CRYPTO_KEY_ALGORITHM algorithm, char *key, int *klen)
{
    if(data == NULL || key == NULL || klen == NULL)
    {
        security_print("ERROR param_check: param is NULL pointer!\n");
        return BSP_ERROR;
    }

    if(len<=0)
    {
        security_print("ERROR param_check: param is invalid!\n");
        return BSP_ERROR;
    }
    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : create_crypto_key
*
* 功能描述  : 使用输入的数据和HUK，生成密钥。当前支持MD5、和SHA-1算法。
*
*             生成密钥的方法:
*             把HUK和输入的数据连接起来作为MD5或SHA-1算法的输入，计算其HASH值。
*
* 输入参数  : data:      用于生成密钥的数据。
*             len:       用于生成密钥的数据长度。(byte)
*             algorithm: 要使用的密钥生成算法。
*             klen:      作为输入参数，存放key的缓冲区的长度。(byte)
*
* 输出参数  : key:       存放生成的密钥。buffer必须大于16(MD5)/20(SHA-1)字节。
*             klen:      作为输出参数，存放生成的密钥的长度。(byte)
*
* 返 回 值  : BSP_OK:    生成成功。
*             BSP_ERROR: 生成失败。
*
* 其它说明  : klen为输入/输出参数，传入的klen变量所用内存必须可写回。
*             所以避免直接传入类似sizeof()的函数调用结果。
*
*****************************************************************************/
int create_crypto_key_o(char *data, int len, CREATE_CRYPTO_KEY_ALGORITHM algorithm, char *key, int *klen)
{
    char* crypto_data = NULL;
    UINT8 i=0;

        if(BSP_OK != param_check(data,len,algorithm,key,klen))
        {
        security_print("ERROR create_crypto_key: param is invalid!\n");
        goto ERROR_EXIT;
        }

    crypto_data = (char*)malloc(len + HUK_LEN);

    if(crypto_data == NULL)
    {
        security_print("ERROR create_crypto_key: malloc failed!\n");
        goto ERROR_EXIT;
    }

	memset(crypto_data, 0, len + HUK_LEN);

	/* Read & Copy HUK.*/
	if(BSP_OK != bsp_efuse_read((u32*)crypto_data, EFUSE_GRP_HUK, (HUK_LEN/sizeof(uint32)) ))
	{
		/*规避EFUSE写入后需要重启才能读取的问题*/
		if(!CheckHukIsValid())
		{
			security_print("ERROR create_crypto_key: efuseRead failed!\n");
			goto ERROR_EXIT;
		}
	}

    for(i=0;i<HUK_LEN;i++)
    {
            if(0 != *(((UINT8*)crypto_data)+i))
            {
                break;
            }
    }

    if(i>=HUK_LEN)
        {
            security_print("ERROR create_crypto_key: read_efuse NULL!\n");
            goto ERROR_EXIT;
    }


    // Copy user data.
    memcpy((void*)((UINT32)crypto_data + HUK_LEN), data, len);


    // Encrypt.
    switch(algorithm)
    {
    /*case CREATE_CRYPTO_KEY_ALGORITHM_MD5:*/
    /*case CREATE_CRYPTO_KEY_ALGORITHM_SHA1:*/
    case CREATE_CRYPTO_KEY_ALGORITHM_SHA256:
        if(BSP_OK != crypto_hash(crypto_data, len + HUK_LEN, (CRYPTO_HASH_ALGORITHM)algorithm, key, klen))
        {
            security_print("ERROR create_crypto_key: crypto_hash failed!\n");
            goto ERROR_EXIT;
        }

        break;

    default:
        security_print("ERROR create_crypto_key: unknown algorithm!\n");

        goto ERROR_EXIT;
    }

//OK_EXIT:
    if(crypto_data != NULL)
    {
        free(crypto_data);
    }
#ifndef _DRV_LLT_
       /*REG_WRITE_32(PWR_SC_PERIPH_CLKDIS0, SOC_CLK_CLOSE_DIS0);*/
#endif
    return BSP_OK;

ERROR_EXIT:
    if(crypto_data != NULL)
    {
        free(crypto_data);
    }

    return BSP_ERROR;
}


int create_crypto_key(char *data, int len, CREATE_CRYPTO_KEY_ALGORITHM algorithm, char *key, int *klen)
{
    if (create_crypto_key_o(data, len, algorithm, key, klen))
    {
        
        return BSP_ERROR;
    }
    
    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : crypto_hash
*
* 功能描述  : 计算输入的数据，输出16字节的HASH值，当前支持MD5、和SHA-1算法。
*
* 输入参数  : data:      待计算HASH值的数据。
*             len:       待计算HASH值的数据长度。(byte)
*             algorithm: 所要使用的HASH算法。
*             hlen:      作为输入参数，存放HASH值的缓冲区的长度。(byte)
*
* 输出参数  : hash:      存放生成的HASH值。buffer必须大于16(MD5)/20(SHA-1)字节。
*             hlen:      作为输出参数，存放生成的HASH值的长度。(byte)
*
* 返 回 值  : OK:        生成成功。
*             ERROR:     生成失败。
*
* 其它说明  : hlen为输入/输出参数，传入的hlen变量所用内存必须可写回。
*             所以避免直接传入类似sizeof()的函数调用结果。
*
*****************************************************************************/
int crypto_hash_o(char *data, int len, CRYPTO_HASH_ALGORITHM algorithm, char *hash, int *hlen)
{
    if(data == NULL || hash == NULL || hlen == NULL)
    {
        security_print("ERROR crypto_hash: param is NULL pointer!\n");
        return BSP_ERROR;
    }

    if(len<=0 )
    {
        security_print("ERROR crypto_hash: param is invalid!\n");
        return BSP_ERROR;
    }

    switch(algorithm)
    {
    case CRYPTO_ALGORITHM_MD5:

        if(*hlen<16)
        {
            security_print("ERROR crypto_hash: param hlen is invalid!(hash bufsize is too small)\n");
            return BSP_ERROR;
        }

        // FIXME: need rewrite MD5 function! NO return value!
        encrypt_lock_md5_data (data, len, hash);
        /*
        if(BSP_OK != encrypt_lock_md5_data (data, len, hash))
        {
            security_print("ERROR crypto_hash: encrypt_lock_md5_data failed!\n");
            return BSP_ERROR;
        }
        */

        *hlen = MD5_HASH_LEN/8;

        break;

    case CRYPTO_ALGORITHM_SHA1:

        if(*hlen<20)
        {
            security_print("ERROR crypto_hash: param hlen is invalid!(hash bufsize is too small)\n");
            return BSP_ERROR;
        }

        /*if(BSP_OK != SHA1Hash(data,len,(BSP_U32*)hash))*/
        {
            security_print("ERROR crypto_hash: SHA1Hash not support!\n");
            return BSP_ERROR;
        }

        /* *hlen = SHA1_HASH_LEN; */

        break;

    case CRYPTO_ALGORITHM_SHA256:

        if (*hlen < SHA256_DIGEST_LENGTH)
        {
            security_print("ERROR crypto_hash: param hlen is invalid!(hash bufsize is too small)\n");
            return BSP_ERROR;
        }

        SHA256_Hash((unsigned char*)data,len,(unsigned char*)hash);

        *hlen = SHA256_DIGEST_LENGTH;

        break;
    default:
        security_print("ERROR crypto_hash: unknown algorithm!\n");

        return BSP_ERROR;
    }

    return BSP_OK;
}/*lint !e429*/

int crypto_hash(char *data, int len, CRYPTO_HASH_ALGORITHM algorithm, char *hash, int *hlen)
{
    if(crypto_hash_o(data, len,  algorithm, hash,  hlen))
    {
        
        return BSP_ERROR;
    }
    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : crypto_encrypt
*
* 功能描述  : 使用指定的密钥和指定的算法对输入的数据加密，输出加密后的数据。
*             当前支持AES-ECB算法。
*
* 输入参数  : data:        待加密数据。
*             len:         待加密数据长度。(byte)
*             algorithm:   所用HASH算法。
*             key:         密钥buffer。
*             klen:        密钥buffer长度。(byte)
*             cipher_len:  加密后的数据的存放buffer的buffer size。(byte)(没有检查)
*
* 输出参数  : cipher_data: 加密后的数据的存放buffer。
*             cipher_len:  加密后的数据的实际长度。(byte)
*
* 返 回 值  : BSP_OK:      加密成功。
*             BSP_ERROR:   加密失败。
*
* 其它说明  : cipher_len为输入/输出参数，传入的cipher_len变量所用内存必须可写回。
*             所以避免直接传入类似sizeof()的函数调用结果。
*
*****************************************************************************/
int crypto_encrypt_o (char *data, int len, CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *cipher_data, int *cipher_len)
{
    crypto_aes aes_ctx;
    int16 keybits = 0;
    if(data == NULL || key == NULL || cipher_data == NULL || cipher_len == NULL)
    {
        security_print("ERROR crypto_encrypt: param is NULL pointer!\n");
        return BSP_ERROR;
    }

    if(len<=0 || (klen != AES_KEY_LEN && klen != 16))
    {
        security_print("ERROR crypto_encrypt: param is invalid!\n");
        return BSP_ERROR;
    }
    if (klen == 16)
    {
        keybits = 128;/* [false alarm]:误报 */
    }
    else if(klen == AES_KEY_LEN)
    {
        keybits = 256;/* [false alarm]:误报 */
    }
    crypto_aes_init(&aes_ctx,(UINT8*)key,keybits,MODE_ECB,NULL);

    switch(algorithm)
    {
    case CRYPTO_ENCRYPT_ALGORITHM_AES_ECB:
        //if(0 != crypto_aes_encrypt_pad(&aes_ctx,data,len,cipher_data,cipher_len))
        if(0 != crypto_aes_encrypt_block(&aes_ctx,(UINT8*)data,len,(UINT8*)cipher_data,(int32*)cipher_len))
        {
            security_print("ERROR crypto_encrypt: crypto_aes_encrypt_pad failed!\n");
            return BSP_ERROR;
        }
        break;

    default:
        security_print("ERROR crypto_encrypt: unknown algorithm!\n");

        return BSP_ERROR;
    }

    return BSP_OK;
}

int crypto_encrypt (char *data, int len, CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *cipher_data, int *cipher_len)
{
    if(crypto_encrypt_o(data, len, algorithm, key, klen, cipher_data, cipher_len))
    {
        
        return BSP_ERROR;        
    }
    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : crypto_decrypt
*
* 功能描述  : 使用指定的密钥和指定的算法对输入的数据解密，输出解密后的数据。
*             当前支持AES-ECB算法。
*
* 输入参数  :
*             cipher_data: 待密的数据的存放buffer。
*             cipher_len:  待解密的数据的实际长度。(byte)
*             algorithm:   所用解密算法，暂只提供AES-ECB。
*             key:         密钥buffer。
*             klen:        密钥buffer长度。(byte)
*             len:  解密后的数据的存放buffer的buffer size。(byte)(没有检查)
*
* 输出参数  :
*             data:        解密后的数据。
*             len:         解密后的数据长度。(byte)
*
* 返 回 值  : BSP_OK:      解密成功。
*             BSP_ERROR:   解密失败。
*
* 其它说明  : len为输入/输出参数，传入的len变量所用内存必须可写回。
*             所以避免直接传入类似sizeof()的函数调用结果。
*
*****************************************************************************/
int crypto_decrypt_o (char *cipher_data,int cipher_len,CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *data, int *len)
{
    crypto_aes aes_ctx;

    if((cipher_data == NULL) || (cipher_len == 0) || (data == NULL) || (key == NULL))
    {
        security_print("ERROR crypto_decrypt: param is NULL pointer!\n");
        return BSP_ERROR;
    }

    if(cipher_len<=0 || klen != AES_KEY_LEN)
    {
        security_print("ERROR crypto_decrypt: param is invalid!\n");
        return BSP_ERROR;
    }

    crypto_aes_init(&aes_ctx,(UINT8*)key,256,MODE_ECB,NULL);

    switch(algorithm)
    {
    case CRYPTO_ENCRYPT_ALGORITHM_AES_ECB:
        if(0 != crypto_aes_decrypt_block(&aes_ctx, (uint8 *)cipher_data, (int32)cipher_len, (uint8 *)data, (int32 *)len ))
        {
            security_print("ERROR crypto_decrypt: crypto_aes_decrypt_block failed!\n");
            return BSP_ERROR;
        }
        break;

    default:
        security_print("ERROR crypto_decrypt: unknown algorithm!\n");

        return BSP_ERROR;
    }

    return BSP_OK;
}

int crypto_decrypt (char *cipher_data,int cipher_len,CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *data, int *len)
{
    if(crypto_decrypt_o(cipher_data,  cipher_len,  algorithm, key, klen,  data,  len))
    {
        
        return BSP_ERROR;        
    }
    return BSP_OK; 
}
/*****************************************************************************
* 函 数 名  : crypto_rsa_encrypt
*
* 功能描述  : 使用保存在NV中的改制用RSA公钥，对输入的数据加密。
*             (读取时需要同样进行签名验证)
*
* 输入参数  : data:        待加密数据。
*             len:         待加密数据长度。(byte)
*             rsa_key:     RSA公钥buffer指针。
*             rsa_len:     RSA公钥buffer长度。(byte)
*             cipher_len:  加密后的数据的存放buffer的buffer size。(byte)(没有检查)
*
* 输出参数  : cipher_data: 加密后的数据的存放buffer。
*             cipher_len:  加密后的数据的实际长度。(byte)
*
* 返 回 值  : BSP_OK:      加密成功。
*             BSP_ERROR:   加密失败。
*
* 其它说明  : cipher_len为输入/输出参数，传入的cipher_len变量所用内存必须可写回。
*             所以避免直接传入类似sizeof()的函数调用结果。
*
*****************************************************************************/
int crypto_rsa_encrypt_o (char *data, int len, char *rsa_key, int rsa_klen, char *cipher_data, int *cipher_len)
{
    //rsa_public_key* p_rsa_key = (rsa_public_key*)rsa_key;/*模A*/
    rsa_public_key* p_rsa_key = NULL;

    if(data == NULL || rsa_key == NULL || cipher_data == NULL || cipher_len == NULL)
    {
        security_print("ERROR crypto_rsa_encrypt: param is NULL pointer!\n");
        return BSP_ERROR;
    }

    if(len <=0 || rsa_klen != RSA_KEY_LEN)
    {
        security_print("ERROR crypto_rsa_encrypt: param is invalid!\n");
        return BSP_ERROR;
    }

    p_rsa_key = (rsa_public_key*)(rsa_key+rsa_klen/2);/*模B*/

    if(0 != rsa_public_encrypt((UINT8*)data, len, (UINT8*)cipher_data, (UINT16*)cipher_len, p_rsa_key))
    {
        security_print("ERROR crypto_rsa_encrypt: rsa_private_encrypt failed!\n");
        return BSP_ERROR;
    }

    return BSP_OK;
}

int crypto_rsa_encrypt (char *data, int len, char *rsa_key, int rsa_klen, char *cipher_data, int *cipher_len)
{
    if(crypto_rsa_encrypt_o( data,  len,  rsa_key,  rsa_klen,  cipher_data, cipher_len))
    {
        
        return BSP_ERROR;        
    }   
    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : crypto_rsa_decrypt
*
* 功能描述  : 使用保存在NV中的改制用RSA公钥，对输入的数据解密。
*
* 输入参数  : cipher_data: 待解密数据。
*             cipher_len:  待解密数据长度。(byte)
*             rsa_key:     RSA公钥buffer指针
*             rsa_len:     RSA公钥buffer长度。(byte)
*             len:         解密后的数据的存放buffer的buffer size。(byte)(没有检查)
*
* 输出参数  : data:        解密后的数据的存放buffer。
*             len:         解密后的数据的实际长度。(byte)
*
* 返 回 值  : BSP_OK:      解密成功。
*             BSP_ERROR:   解密失败。
*
* 其它说明  : len为输入/输出参数，传入的len变量所用内存必须可写回。
*             所以避免直接传入类似sizeof()的函数调用结果。
*
*****************************************************************************/
int crypto_rsa_decrypt_o (char *cipher_data, int cipher_len, char *rsa_key, int rsa_klen, char *data, int *len)
{
    //rsa_public_key* p_rsa_key = (rsa_public_key*)rsa_key;/*模A*/
    rsa_public_key* p_rsa_key = NULL;

    if(cipher_data == NULL || rsa_key == NULL || data == NULL || len == NULL)
    {
        security_print("ERROR crypto_rsa_decrypt: param is NULL pointer!\n");
        return BSP_ERROR;
    }

    if(cipher_len <=0 || rsa_klen != RSA_KEY_LEN)
    {
        security_print("ERROR crypto_rsa_decrypt: param is invalid!\n");
        return BSP_ERROR;
    }

    p_rsa_key = (rsa_public_key*)(rsa_key+rsa_klen/2);/*模B*/

    if(0 != rsa_public_decrypt((UINT8*)cipher_data,cipher_len,(UINT8*)data,(UINT16*)len,p_rsa_key))
    {
        security_print("ERROR crypto_rsa_decrypt: rsa_public_decrypt failed!\n");
        return BSP_ERROR;
    }

    return BSP_OK;
}

int crypto_rsa_decrypt (char *cipher_data, int cipher_len, char *rsa_key, int rsa_klen, char *data, int *len)
{
    if(crypto_rsa_decrypt_o(cipher_data, cipher_len, rsa_key,  rsa_klen,  data,  len))
    {
        
        return BSP_ERROR;        
    }
    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : crypto_rand
*
* 功能描述  : 随机数生成接口
*
* 输入参数  : len:       期望得到的随机数字节数,只支持4字节。(byte)
*
* 输出参数  : rand_data: 随机数存放buffer。
*
* 返 回 值  : BSP_OK:    获取随机数成功。
*             BSP_ERROR: 获取随机数失败。
*
* 其它说明  : 1. 本接口以时间为种子获取随机数，并且获取结果与时钟强相关。请勿
*                在同一时钟粒度内连续反复调用。连续调用将获得同样结果。
*
*             2. len参数只接受传入4。
*
*****************************************************************************/
int crypto_rand_o (char *rand_data, int len)
{
    if(NULL == rand_data)
    {
        security_print("ERROR crypto_rand: param rand_data is NULL pointer!\n");
        return BSP_ERROR;
    }

    if(len!=4)
    {
        security_print("ERROR crypto_rand: param len is invalid!\n");
        return BSP_ERROR;
    }
	/* coverity[secure_coding] */
    *(uint32*)rand_data = random();/* [false alarm]:coverity消除时已经申请屏蔽 */

    return BSP_OK;
}

int crypto_rand (char *rand_data, int len)
{
        return crypto_rand_o(rand_data, len);
}



/*简单测试函数*/
/*crypto_encrypt加密与crypto_decrypt解密验证*/
int sss_test1()
{
	char * data_in = NULL;
	int data_in_len = 128*10/8;
	CRYPTO_ENCRYPT_ALGORITHM alg = CRYPTO_ENCRYPT_ALGORITHM_AES_ECB;
	char * out_data = NULL;
	int out_data_len = 128*10/8;
	char * decrypto_resul = NULL;
	int ret = 0;

	data_in = (char*)malloc(data_in_len);
	decrypto_resul = (char*)malloc(data_in_len);
	out_data = (char*)malloc(out_data_len);
	if(!data_in || !decrypto_resul || !out_data)
	{
		ret = -1;
		goto OUT;
	}
	memset(data_in, 1, data_in_len);
	memset(decrypto_resul, 0, data_in_len);
	memset(out_data, 0, out_data_len);

	ret = crypto_encrypt(data_in, data_in_len, alg, (char *)sha1_in_key,
		AES_KEY_LEN, out_data, &out_data_len);
	if(BSP_ERROR == ret)
	{
		security_print("function : %s -- linenum : %d -- retval : %X\n", ret);
		goto OUT;
	}

	ret = crypto_decrypt(out_data, out_data_len, alg, (char *)sha1_in_key,
		AES_KEY_LEN, decrypto_resul, &data_in_len);
	if(BSP_ERROR == ret)
	{
		security_print("function : %s -- linenum : %d -- retval : %X\n", ret);
		goto OUT;
	}

	ret = memcmp(data_in, decrypto_resul, (unsigned) data_in_len);
	if(BSP_ERROR == ret)
	{
		security_print("function : %s -- linenum : %d -- retval : %X\n", ret);
		goto OUT;
	}
OUT:
	free(data_in);
	free(out_data);
	free(decrypto_resul);

	return ret;
}


/*RSA加解密验证*/
void rever(uint32* data, int len)
{
    int i;
    u32 temp;
    for (i=0;i<len/2;i++)
    {
        temp = data[i];
        data[i] = data[len-1-i];
        data[len-1-i] = temp;
    }
}

#include "drv_comm.h"
#include "rsann.h"
#define MAX_N_LENGTH 32

int sss_test2()
{
	uint32 D[MAX_N_LENGTH] = 
	{
		0x0e85980c, 0x64ed9426, 0x6965357b, 0x148d7858,
		0x1fccaf34, 0x9dbb975d, 0xdbd454bf, 0xd31430f7,
		0xbcad1f90, 0x8ff6a4ad, 0xec59636f, 0xd0b54bc8,
		0x14614184, 0x7a1bbf74, 0xa7838212, 0x6a782cb3,
		0xd7e372fd, 0x8c146b8d, 0x0ca47816, 0x781323e6,
		0x00e729c5, 0xc9e7a788, 0x4d9a3516, 0xf9b81076,
		0xdf9bbb1d, 0x3edf8067, 0xf10fe7c7, 0x2a147030,
		0x09d038d3, 0x95e72f37, 0x7678c1f9, 0xaf1a81a1
	};
	uint32 E[MAX_N_LENGTH] =
	{
		0x10001,
	};
	uint32 N[MAX_N_LENGTH] =
	{
		0xe6f18faf, 0x4db624a6, 0x9471fbeb, 0x0156b6db,
		0xcadff22a, 0x6cb96fd8, 0x70f28e44, 0x32b10c1b,
		0xeea8dc03, 0x3372e6c0, 0x83a74734, 0x01101ea6,
		0x1f04e6b3, 0x41ec9759, 0xf2f6f77c, 0x16c5dd97,
		0xf5db6c60, 0xbd53bca9, 0xbdd7aebf, 0x70f338f4,
		0xb45a259c, 0xcff12faf, 0xdb5d435a, 0xeb075da7,
		0xd844a150, 0x2a98baba, 0x81c16187, 0x093b548c,
		0x20dec362, 0x64cfc03f, 0xe797a77d, 0x658e31eb
	};

	uint32 rsa[MAX_N_LENGTH];      /* rsa加密后的数据 */
    uint32 rsaDec[MAX_N_LENGTH];   /* rsa解密后的数据 */
	uint32 org_data[MAX_N_LENGTH]; /*源数据*/
	uint32 tmp_cmn_buff[MAX_N_LENGTH];

	rsa_public_key key_input[2] = {{0,{0},{0}},{0,{0},{0}}};
	int ret = 0;
	int rsa_out_len = sizeof(rsa);
	int rsaDec_out_len = sizeof(rsaDec);
	int i = 0;

    rever(N, MAX_N_LENGTH);
    rever(D, MAX_N_LENGTH);

	memset(tmp_cmn_buff, 0, sizeof(u32) * MAX_N_LENGTH);
	memset(rsa, 0, 4 * MAX_N_LENGTH);
	memset(rsaDec, 0, 4 * MAX_N_LENGTH);
	memset(org_data, 0, sizeof(org_data));

	memcpy(tmp_cmn_buff, N, sizeof(u32) * MAX_N_LENGTH);
	nn_encode((uint8*)&N[0], (uint16)(sizeof(u32) * MAX_N_LENGTH), (uint32*)tmp_cmn_buff,(uint16) MAX_N_LENGTH);
	memcpy(tmp_cmn_buff, D, sizeof(u32) * MAX_N_LENGTH);
	nn_encode((uint8*)&D[0], (uint16)(sizeof(u32) * MAX_N_LENGTH), (uint32*)tmp_cmn_buff, (uint16)MAX_N_LENGTH);
	memcpy(tmp_cmn_buff, E, sizeof(u32) * MAX_N_LENGTH);
	nn_encode((uint8*)&E[0], (uint16)(sizeof(u32) * MAX_N_LENGTH), (uint32*)tmp_cmn_buff, (uint16)MAX_N_LENGTH);

	key_input[1].bits = 8*sizeof(N);
	memcpy(key_input[1].modulus, N, sizeof(N));
	memcpy(key_input[1].public_exponent, D, sizeof(D));

	org_data[0] = 0xFFFFFFFF;
	org_data[1] = 116;
	org_data[2] = 116;
	org_data[3] = 116;
	org_data[4] = 116;
	org_data[5] = 116;
	org_data[6] = 116;


	ret = crypto_rsa_encrypt((char*)org_data, 7*4, (char*)&key_input, sizeof(key_input), (char*)rsa, &rsa_out_len);
	if(BSP_ERROR == ret)
	{
		security_print("function : %s -- linenum : %d -- retval : %X\n", ret);
		return ret;
	}
	security_print("rsa_out_len : %d\n\n",rsa_out_len);
	for(i = 0; i < rsa_out_len/4; i++)
		security_print("%lx\n", rsa[i]);
	security_print("========================\n");

	key_input[1].bits = 8*sizeof(N);
	memcpy(key_input[1].modulus, N, sizeof(N));
	memcpy(key_input[1].public_exponent, E, sizeof(E));
	ret = crypto_rsa_decrypt((char*)rsa, rsa_out_len, (char*)&key_input, sizeof(key_input), (char*)rsaDec, &rsaDec_out_len);
	if(BSP_ERROR == ret)
	{
		security_print("function : %s -- linenum : %d -- retval : %X\n", ret);
		return ret;
	}

	security_print("rsaDec_out_len : %d\n\n", rsaDec_out_len);
	for(i = 0; i < rsaDec_out_len/4; i++)
		security_print("%lx\n",rsaDec[i]);

	ret = memcmp(org_data, rsaDec, (unsigned) rsaDec_out_len);
	if(BSP_ERROR == ret)
	{
		security_print("function : %s -- linenum : %d -- retval : %X\n", ret);
		return ret;
	}

	return ret;
}


/*HASH值计算验证*/
int sss_test3()
{
 	int ret = 0;
	unsigned char * data_out = NULL;
	int data_out_len = 1024/8;
	int i = 0;
	u32 N[MAX_N_LENGTH] =
    {
        0xbc7bd14d, 0xc668378f, 0x21ede7db, 0xe6708189,
        0xba9d3be0, 0xd0daeb54, 0x6044fcb7, 0xc0c0af92,
        0x6409dfb5, 0x5b7f52ad, 0xeb38d7e8,  0x8e2619b,
        0x313b8f7e, 0x30b9c657, 0x6d917e6c, 0x250ac29f,
        0x90238692, 0x9b90db74, 0xd2d428eb, 0xc68e7901,
        0xaa6ec926, 0x87f7bc39, 0xf01ce959, 0x1e0da497,
        0x1174e0c6, 0x82a2da44, 0xdd779c99, 0x52b40f33,
        0x7d109cc4, 0xc04a36ce, 0xbc5827d5, 0x68ca5105,
    };

	data_out = (unsigned char*)malloc(data_out_len);
	if(!data_out)
		return -1;
	memset(data_out, 0, (unsigned)data_out_len);
	ret = crypto_hash((char *)N, sizeof(N), CRYPTO_ALGORITHM_SHA1, (char *)data_out, &data_out_len);
	if(ret == BSP_ERROR)
	{
		security_print("function : %s -- linenum : %d -- retval : %X\n", ret);
		free(data_out);
		return ret;
	}
	security_print("CRYPTO_ALGORITHM_SHA1 :\n");
	for(i = 0; i < data_out_len; i += 4)
	{
		security_print("%lx\n", *(long *)&(data_out[i]));
	}

	data_out_len = 1024/8;
	memset(data_out, 0, (unsigned)data_out_len);
	ret = crypto_hash((char *)N, sizeof(N), CRYPTO_ALGORITHM_MD5, (char *)data_out, &data_out_len);
	security_print("\nCRYPTO_ALGORITHM_MD5 :\n\n");
	if(ret == BSP_ERROR)
	{
		security_print("function : %s -- linenum : %d -- retval : %X\n", ret);
		free(data_out);
		return ret;
	}
	for(i = 0; i < data_out_len; i += 4)
	{
		/* coverity[printf_arg_mismatch] */
		security_print("%lx\n", *(long*)(&(data_out[i])));
	}

	data_out_len = 1024/8;
	memset(data_out, 0, (unsigned)data_out_len);
	ret = crypto_hash((char *)N, sizeof(N), CRYPTO_ALGORITHM_SHA256, (char *)data_out, &data_out_len);
	security_print("\nRYPTO_ALGORITHM_SHA256 :\n\n");
	if(ret == BSP_ERROR)
	{
		security_print("function : %s -- linenum : %d -- retval : %x\n",ret);
		free(data_out);
		return ret;
	}
	for(i = 0; i < data_out_len; i += 4)
	{
		/* coverity[printf_arg_mismatch] */
		security_print("%lx\n", *(long*)(&(data_out[i])));
	}
	free(data_out);
	return ret;
}


/*随机数生成验证*/
int sss_test4()
{
	int i = 0, j = 0;
	srandom(10);

	for(i = 0; i < 1000; i++)
	{
		for(j = 0; j < 100; j++)
		{
			/* coverity[secure_coding] */
			security_print("random = %ul\n", random());/* [false alarm]:误报 */
		}
	}
	return 0;
}

