/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  wangxiandong
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2013年3月12日  v1.00  wangxiandong修改
*************************************************************************/

#ifndef __DRV_CYPTO_H__
#define __DRV_CYPTO_H__

#include "drv_comm.h"

/*for create_crypto_key,hash algorithm enum*/
typedef enum 
{
    CREATE_CRYPTO_KEY_ALGORITHM_MD5 = 0x0,
    CREATE_CRYPTO_KEY_ALGORITHM_SHA1,
    CREATE_CRYPTO_KEY_ALGORITHM_SHA256,
    CREATE_CRYPTO_KEY_ALGORITHM_MAX
}CREATE_CRYPTO_KEY_ALGORITHM;

/*for crypto_hash,hash algorithm enum*/
typedef enum 
{
    CRYPTO_ALGORITHM_MD5 = 0x0,
    CRYPTO_ALGORITHM_SHA1,
    CRYPTO_ALGORITHM_SHA256,
    CRYPTO_ALGORITHM_MAX
}CRYPTO_HASH_ALGORITHM;

/*for crypto_encrypt,aes algorithm enum*/
typedef enum 
{
    CRYPTO_ENCRYPT_ALGORITHM_AES_ECB = 0x0,
    CRYPTO_ENCRYPT_ALGORITHM_MAX
}CRYPTO_ENCRYPT_ALGORITHM;



/*****************************************************************************
* 函 数 名  : create_crypto_key
*
* 功能描述  : 使用输入的数据和HUK，生成密钥。
*当前支持MD5、和SHA-1算法。生成密钥的方法：把HUK和输入
*的数据连接起来作为MD5或SHA-1算法的输入，计算其HASH值
*
* 输入参数  : data：输入参数。存放用于生成密钥的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           algorithm：输入参数。用于产生密钥的算法。
*                           key：输出参数。存放生成的密钥。
*            注意：其长度必须不小于16字节。因为密钥为16字节。
*                           klen：输入输出参数。作为输入参数，存放key的
*            缓冲区的长度。作为输出参数，存放生成的密钥的长度。(字节)
* 输出参数  : 
*
* 返 回 值  :  BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  :内存由调用者申请
*
*****************************************************************************/
int create_crypto_key(char *data, int len, CREATE_CRYPTO_KEY_ALGORITHM algorithm, char *key, int *klen);
#define CREATE_CRYPTO_KEY(data,len,algorithm,key,klen)  create_crypto_key(data,len,algorithm,key,klen)

/*****************************************************************************
* 函 数 名  : crypto_hash
*
* 功能描述  : 计算输入的数据的HASH值。
*                           当前支持MD5、和SHA-1算法。输出HASH值长度是16字节
*
* 输入参数  : data：输入参数。存放用于需要计算HASH值的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           algorithm：输入参数。HASH算法。
*                           hash：输出参数。存放生成的HASH值。
*                           hlen：输入输出参数。作为输入参数，存放HASH值的缓冲区的长度。(字节)
*                           作为输出参数，存放生成的HASH值的长度。
* 输出参数  : 
*
* 返 回 值  : BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_hash(char *data, int len, CRYPTO_HASH_ALGORITHM algorithm, char *hash, int *hlen);
#define CRYPTO_HASH(data,len,algorithm,hash,hlen)  crypto_hash(data,len,algorithm,hash,hlen)

/*****************************************************************************
* 函 数 名  : crypto_encrypt
*
* 功能描述  : 使用指定的密钥和指定的算法对输入的数据加密，输出加密后的数据。
                             当前支持AES-ECB算法。
*
* 输入参数  : data：输入参数。存放需要加密的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           algorithm：输入参数。HASH算法。
*                           key：输入参数。存放加密密钥。
*                           klen：输入参数。key的长度。(字节)
*                           cipher_data：输出参数。存放加密后的数据。
*                           cipher_len：输入输出参数。作为输入参数，存放密文的缓冲区的长度。(字节)
*               作为输出参数，存放生成的密文的长度。
* 输出参数  : 
*
* 返 回 值  :  BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_encrypt (char *data, int len, CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *cipher_data, int *cipher_len);
#define CRYPTO_ENCRYPT(data,len,algorithm,key,klen,cipher_data,cipher_len)  \
crypto_encrypt(data,len,algorithm,key,klen,cipher_data,cipher_len)

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
extern int crypto_decrypt (char *cipher_data,int cipher_len,CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *data, int *len);
#define CRYPTO_DECRYPT(cipher_data,cipher_len,algorithm, key, klen, data, len)  \
crypto_decrypt(cipher_data,cipher_len,algorithm, key, klen, data, len)


/*****************************************************************************
* 函 数 名  : crypto_rsa_encrypt
*
* 功能描述  : 使用保存在NV中的改制用RSA公钥（读取时需要同样进行签名验证）
*           对输入的数据加密，输出加密后的数据。
* 输入参数  : data：输入参数。存放需要加密的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           rsa_key:RSA公钥
*                           rsa_len:RSA公钥长度(字节)
*                           cipher_data：输出参数。存放加密后的数据。
*                           cipher_len：输入输出参数。作为输入参数，存放密文的缓冲区的长度。(字节)
*               作为输出参数，存放生成的密文的长度。
* 输出参数  : 
*
* 返 回 值  :  BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_rsa_encrypt (char *data, int len, char *rsa_key, int rsa_klen, char *cipher_data, int *cipher_len);
#define CRYPTO_RSA_ENCRYT(data,len,rsa_key,rsa_klen,cipher_data,cihper_len) \
crypto_rsa_encrypt(data,len,rsa_key,rsa_klen,cipher_data,cihper_len)

/*****************************************************************************
* 函 数 名  : crypto_rsa_decrypt
*
* 功能描述  : 使用保存在NV中的改制用RSA公钥，对输入的数据解密
*               输出解密后的数据。
* 输入参数  : cipher_data：输入参数。存放加密数据。
*                           cipher_len：输入参数。存放密文的缓冲区的长度。(字节)
*                           rsa_key:RSA公钥
*                           rsa_len:RSA公钥长度(字节)
*                           data：输出参数。存放需解密后的数据。
*                           len：输入输出参数。作为输入参数，存放解密后的缓冲区的长度(字节)
*               作为输出参数，存放生成的明文的长度
* 输出参数  : 
*
* 返 回 值  :  BSP_OK--解密成功;BSP_ERROR--解密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_rsa_decrypt (char *cipher_data, int cipher_len, char *rsa_key, int rsa_klen, char *data, int *len);
#define CRYPTO_RSA_DECRYPT(cipher_data,cihper_len,rsa_key,rsa_klen,data,len) \
crypto_rsa_decrypt(cipher_data,cihper_len,rsa_key,rsa_klen,data,len)

/*****************************************************************************
* 函 数 名  : crypto_rand
*
* 功能描述  : 随机数生成接口
* 输入参数  : rand_data:随机数存放buffer
*                           len:期望得到的随机数字节数
*
* 输出参数  : 
*
* 返 回 值  :  BSP_OK--获取随机数成功;BSP_ERROR--获取失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_rand (char *rand_data, int len);
#define CRYPTO_RAND(rand_data,len)  crypto_rand(rand_data,len)


void _Clarinet_MD5Init(void *mdContext);
void _Clarinet_MD5Update(void *mdContext, unsigned char *inBuf, unsigned int inLen);
void _Clarinet_MD5Final(unsigned char *hash, void *mdContext);

#endif

