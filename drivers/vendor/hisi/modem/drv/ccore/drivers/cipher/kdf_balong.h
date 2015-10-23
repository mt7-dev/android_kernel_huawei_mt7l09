/*************************************************************************
*   版权所有(C) 2008-2013, 深圳华为技术有限公司.
*
*   文 件 名 :  kdf_balong.h
*
*   作    者 :  wangxiandong
*
*   描    述 :  kdf功能实现头文件
*
*   修改记录 :  2013年03月12日  v1.00  wangxiandong 创建
*************************************************************************/
#ifndef	_CIPHER_KDF_H
#define	_CIPHER_KDF_H

#include "bsp_cipher.h"
#include "cipher_balong_common.h"


/* KDF相关宏定义*/
#define KDF_CHN_BD_NUM         0x10
#define KDF_CHN_RD_NUM         KDF_CHN_BD_NUM
#define KDF_CHN_NUM            0x4
#define KDF_CHN_USRFIELD_LEN   0x0
#define KDF_SHA_KEY_LENGTH     0x20
#define KDF_SHA_S_LENGTH_MAX   0x200

enum kdf_op_type_e
{
    kdf_op_keymake = 0,
    kdf_op_write = 1,
    kdf_op_read = 2,
    kdf_op_max
};

/* KDF使用的BD结构*/
struct kdf_bd_s
{
    u32 kdf_config;
    u32 address;
};
/* KDF使用的RD结构*/
struct kdf_rd_s
{
    u32 address;
    u32 kdf_config;
};
/* KDF通道管理结构体 */
struct kdf_chx_mgr_s
{   
    struct kdf_bd_s *p_bd_base;          /* 当前通道BD描述符基地址 */
    struct kdf_rd_s *p_rd_base;          /* 当前通道RD描述符基地址 */
    CIPHER_MUTEX_T mtx_chn_opt;          /* KDF作的互斥量 */
};


struct kdf_bdconfig_info_s
{
	u32	sha_key_index;                    /*sha_key来源于KeyRam时，其在keyRam中的索引*/
	u32	sha_s_index;                      /*sha_s来源于KeyRam时，其在keyRam中的索引*/
	u32 length;                           /*KDF运算作为sha_s的长度*/
	u32 dest_index;                       /*sha_result_sn在key_ram中的位置*/
	void * p_address;                     /*目的(源)地址*/
	enum kdf_op_type_e opt_type;          /*操作类型*/
	KDF_SHA_KEY_SOURCE_E sha_key_source;  /*sha_key来源*/
	KDF_SHA_S_SOURCE_E sha_s_source;      /*sha_s来源*/
};

void cipher_set_kdf_debug_level(s32 level);
static void cipher_kdf_chx_mag_reset();

#endif
