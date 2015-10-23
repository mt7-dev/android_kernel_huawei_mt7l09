/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  sec_balong.c
*
*   作    者 :  wangxiandong
*
*   描    述 :
*
*   修改记录 :
*
*************************************************************************/
#include <drv_rfile.h>
#include <drv_cipher.h>
#include <drv_nv_id.h>
#include <drv_mntn.h>
#include <bsp_nvim.h>
#include <bsp_om.h>
#include <soc_onchiprom.h>
#include <osl_malloc.h>
#include "../security/md5.h"
#include "sec_balong.h"
#include "efuse_balong.h"


s32 kdf_sha256(const u8*  pucInBuf, u32 ulInLen, u8* pucOutBuf)
{
    s32 iRet = SEC_OK;
	KEY_CONFIG_INFO_S key_cfg = {SHA_KEY_SOURCE_MAX, 0, NULL};
	S_CONFIG_INFO_S s_cfg = {SHA_S_SOURCE_MAX, 0, 0, NULL};
	KEY_MAKE_S k_make = {CIPHER_KEY_OUTPUT_BUTTOM, {0, 0, 0}};
	KEY_GET_S k_g_s = {0, 0, 0};
	u8 * temp_hash = NULL;
	u32 out_hash_len;

    if ((NULL == pucInBuf) || (0 == ulInLen) || (NULL == pucOutBuf))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "secDataSha: Input Parameter Error!\n");
        return -1;
    }

    {
#ifdef CONFIG_CIPHER
    	/*这里应该是使用SHA256，cipher实现的，因为onchiprom换成这样的*/
		temp_hash = (u8*)osl_malloc(SHA256_HASH_SIZE * sizeof(u32));
		if(NULL == temp_hash)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "secDataSha: Error! NO MEM TO MALLOC\n");
			goto end;
		}
		memset(temp_hash, 0, SHA256_HASH_SIZE * sizeof(u32));/*onchiprom里使用的初始key也是256bit全零*/
		k_g_s.enKeyLen = CIPHER_KEY_L256;
		k_g_s.penOutKeyLen = &out_hash_len;
		k_g_s.pKeyAddr = temp_hash;
		k_make.enKeyOutput = CIPHER_KEY_OUTPUT;
		k_make.stKeyGet = k_g_s;
		key_cfg.enShaKeySource = SHA_KEY_SOURCE_DDR;
		key_cfg.pKeySourceAddr = temp_hash;
		key_cfg.u32ShaKeyIndex = 0;
		s_cfg.enShaSSource = SHA_S_SOURCE_DDR;
		s_cfg.pSAddr = (void*)pucInBuf;
		s_cfg.u32ShaSIndex = 0;
		s_cfg.u32ShaSLength = ulInLen > 512 ? 512 : ulInLen;/*kdf 一次最大只能处理512字节*/
		do
		{
			iRet = bsp_kdf_key_make(&key_cfg, &s_cfg, 0/*不使用，所以无所谓*/, &k_make);
			if(iRet)
			{
				bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY,
					"secDataSha: run sha error : %X!\n", iRet);
				break;
			}
			pucInBuf += s_cfg.u32ShaSLength;
			s_cfg.pSAddr = (void*)pucInBuf;
			ulInLen -= s_cfg.u32ShaSLength;
			s_cfg.u32ShaSLength = ulInLen > 512 ? 512 : ulInLen;
			/*memcpy(key_cfg.pKeySourceAddr, k_make.stKeyGet.pKeyAddr, 256 >> 3);*//*没必要，两个指针是同一块地址空间*/
		}while(ulInLen > 0);
		memcpy(pucOutBuf, temp_hash, 256 >> 3);/*key的最大长度256bits*/
#else
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "cipher is not compiled and the onchiprom SHA algorithm is not used\n");
#endif
    }

end:
	free(temp_hash);
    return iRet;
}

/*************************************************
 函 数 名   : secureSupport
 功能描述   : 当前版本是否支持安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值   : SEC_OK/SEC_ERROR
 调用函数   :
 被调函数   :

 修改历史   :
 日    期   :
 作    者   :
 修改内容   :

*************************************************/
s32 secureSupport(u8* pData)
{
    u8 usNVSecBootEnableFlag = 0xFF;
    if(NULL == pData)
    {
        return SEC_ERROR_NULL_PTR;
    }
    if (SEC_OK != bsp_nvm_read(NV_ID_DRV_SEC_BOOT_ENABLE_FLAG,  &usNVSecBootEnableFlag, sizeof(u16)))
    {
    	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "FUNC secureSupport ERR! line : %d ", __LINE__);
        return SEC_ERROR_NV_READ_ERR;
    }
	*pData = (SECURE_NOT_SUPPORT == usNVSecBootEnableFlag) ? (u8)SECURE_NOT_SUPPORT : (u8)SECURE_SUPPORT;

    return SEC_OK;
}

/*************************************************
 函 数 名   : secureAlreadyUse
 功能描述   : 查询当前版本是否已经启用安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值   : OK/ERROR
 调用函数   :
 被调函数   :

 修改历史   :
 日    期   : 2010年12月21日
 作    者   :
 修改内容   :

*************************************************/
s32 secureAlreadyUse(u8 * pData)
{
    u32 efuse_security_flag = 0;
	s32 sec_err_code = SEC_OK;

    if(NULL == pData)
    {
        return SEC_ERROR_NULL_PTR;
    }

	sec_err_code = bsp_efuse_read(&efuse_security_flag, EFUSE_GRP_SECURITY, 1);
    if( SEC_OK != sec_err_code)
    {
        return sec_err_code;
    }
    /*判断安全位是否已写入*/
	*pData = ((efuse_security_flag & EFUSEC_SEC_EN_MASK) == EFUSEC_SEC_EN) ? (u8)SECURE_ENABLE : (u8)SECURE_DISABLE;

    return sec_err_code;
}
s32 startSecure(void)
{
	s32 sec_err_code = SEC_OK;
    u8  flag = 0;
	u8 * m3boot_ram_addr = NULL;
    u32 ulEfuseRootCaHash[SHA256_HASH_SIZE] = {0}; /*用来存放SHA256值的临时buffer*/
    u32 efuse_security_flag = 0;
	u32 md5_hash[MD5_HASH_SIZE] = {0};

    sec_err_code = secureAlreadyUse(&flag);
	if(sec_err_code)
	{
		return -1;
	}
    if( SECURE_ENABLE == flag )                    /* has been written */
    {
        return SEC_OK;
    }
	sec_err_code = secureSupport(&flag);
    if(SEC_OK != sec_err_code)
    {
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "[SEC ERROR]check Secure support error!\n");
        return sec_err_code;
    }
    if(SECURE_NOT_SUPPORT == flag)
    {
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "[SEC ERROR]Secure not support!\n");
        return SEC_ERROR_IMG_SECURY_NOT_SUPPORT;
    }

	m3boot_ram_addr = (u8*)osl_malloc(ROOT_CA_LEN);
	if(!m3boot_ram_addr)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "[SEC ERROR]NO mem error!\n");
		return -1;
	}
	sec_err_code = BSP_mass_read("m3boot", /*P531_M3_LEN_INDEX + sizeof(u32)*/ROOT_CA_INDEX, ROOT_CA_LEN, m3boot_ram_addr);
    if(SEC_OK != sec_err_code)
    {
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "BSP_mass_read error!\n");
        return sec_err_code;
    }

#if 0//for debug, print rootca
	printf("\n");
	for(i = 0; i < ROOT_CA_LEN / 4; i += 4)
	{
		printf("%X\n", *(unsigned long *)(m3boot_ram_addr + i));
	}
	printf("\n");
#endif

	/*计算ROOT CA HASH*/
	sec_err_code = kdf_sha256(m3boot_ram_addr, ROOT_CA_LEN, (u8*)ulEfuseRootCaHash);
    if(SEC_OK != sec_err_code)
    {
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "\r\ncalc RootCa sha1 err!\n");
        return SEC_ERROR_SHA_ERR;
    }
	/*计算HASH值的MD5值*/
	sec_err_code = encrypt_lock_md5_data(ulEfuseRootCaHash, SHA256_HASH_SIZE * sizeof(u32), md5_hash);
	if(sec_err_code)
	{
		return -1;
	}

	sec_err_code = bsp_efuse_read(&efuse_security_flag, EFUSE_GRP_SECURITY, 1);
    if( SEC_OK != sec_err_code)
    {
    	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "startSecure: run efuse_read error!\n");
        return SEC_ERROR_EFUSE_READ_ERR;
    }
    efuse_security_flag |= EFUSEC_SEC_EN;

    /* write RootCA hash */
    if( SEC_OK != bsp_efuse_write( md5_hash, EFUSE_GRP_ROOT_CA, 4 ) )
    {
        return -1;
    }

    /* Last step, enable security boot */
    if( SEC_OK != bsp_efuse_write( (u32 *)&efuse_security_flag, EFUSE_GRP_SECURITY, 1 ) )
    {
        return -1;
    }

	bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_SECURITY, "\r\nstartSecure SUCC!\r\n", 0, 0, 0, 0, 0, 0 );
    return 0;
}
s32 drvSecCheck(void)
{
	u8 sec_eable_flag = UNSECBOOT_FLAG;
	(void)secureAlreadyUse(&sec_eable_flag);
    if( SECURE_ENABLE == sec_eable_flag )
    {
        return SECBOOT_FLAG;
    }
	return sec_eable_flag;
}


BSP_S32 BSP_SEC_Check()
{
#if( FEATURE_SEC_BOOT == FEATURE_ON)
    return drvSecCheck();
#else
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "\n[SEC ERROR]drvSecCheck : stub \n");
    return UNSECBOOT_FLAG;
#endif
}

BSP_S32 BSP_SEC_Support(BSP_U8 * p_data)
{
#if( FEATURE_SEC_BOOT == FEATURE_ON)
     return secureSupport( p_data );
#else
     bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "\n[SEC ERROR]BSP_SEC_Support : stub \n");
     return ERROR;
#endif
}

BSP_S32 BSP_SEC_AlreadyUse(BSP_U8 * p_data)
{
#if( FEATURE_SEC_BOOT == FEATURE_ON)
     return secureAlreadyUse( p_data );
#else
     bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "\n[SEC ERROR]BSP_SEC_AlreadyUse : stub \n");
     return ERROR;
#endif
}

BSP_S32 BSP_SEC_Start()
{
#if( FEATURE_SEC_BOOT == FEATURE_ON)
     return startSecure();
#else
     bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SECURITY, "\n[SEC ERROR]BSP_SEC_Start : stub \n");
     return ERROR;
#endif
}

/*以下是测试代码*/
#include <hi_cipher.h>
#include <bsp_memmap.h>

#define CIPHER_BSWAP32(val) \
        (((val) >> 24) | (((val) >> 8) & 0x0000ff00) | \
        (((val) << 8) & 0x00ff0000) | ((val) << 24))
static BSP_S32 cipher_memcmp_ext(const void* mem1,const void* mem2,int size)
{
	BSP_S32 i = 0;

	for(i = 0; i < size; i++)
	{
		if(*((char*)mem1+i) != *((char*)mem2+i))
		{
			//printk("diff_pos:%d,addr1(0x%x):%x,addr2(0x%x):%x\n",i,(int)((char*)mem1+i),(int)*((char*)mem1+i),(int)((char*)mem2+i),(int)*((char*)mem2+i),0);
			return BSP_ERROR;
		}
	}
	return BSP_OK;
}

int test_kdf_sha256()
{
	u32 pDestAddr = (HI_CIPHER_BASE_ADDR_VIRT + HI_KEY_RAM_OFFSET);
	u8 right_hash[32] = {0x65,0xb9,0x30,0x04,0x64,0x47,0x0a,0x9a,0xe4,0xc2,0x30,0xf9,0xd0,0xf3,0xe4,0x03,0x60,0xc7,0x56,0x20,0xf1,0x9f,0x89,0x64,0xf5,0x29,0xc7,0x78,0x2e,0xc8,0x88,0xc8};
	int i = 0;
	u32 src_data[1024] = {0};
	u32 hashrelt[8] = {0,0,0,0,0,0,0,0};
	u32 result_hash[8] = {0};
	s32 err = 0;

	for(i = 0; i < 1024; i++)
		src_data[i] = i;
	err = kdf_sha256((u8*)&src_data[0], 512 * 4, (u8*)&hashrelt[0]);
	if(err)
	{
		//printf("ERR : kdf_sha256\n");
		return err;
	}
	#if 0
	for(i = 0;i < 8; i++)
		printf("%X\n", hashrelt[i]);
	printf("\n\n");
	#endif
	for(i = 0; i < 8; i++)
		result_hash[i] = CIPHER_BSWAP32(*((u32 *)pDestAddr + i));
	err = cipher_memcmp_ext(result_hash, right_hash, 32);
	return err;
}

int test_secureSupport()
{
	u8 p_img_flag = 0;
	(void)secureSupport(&p_img_flag);
	//printf("%d\n\n", p_img_flag);
	return p_img_flag;
}

int test_secureAlreadyUse()
{
	u8 efuse_en_sec = 0xFF;

	(void)secureAlreadyUse(&efuse_en_sec);
	#if 0
	if(secureAlreadyUse(&efuse_en_sec))
		printf("ERR secureAlreadyUse\n");
	printf("%d\n", efuse_en_sec);
	#endif
	return efuse_en_sec;
}

