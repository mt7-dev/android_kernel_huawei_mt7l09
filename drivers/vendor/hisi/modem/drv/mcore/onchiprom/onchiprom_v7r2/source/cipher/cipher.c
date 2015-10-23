#ifdef __cplusplus
extern "C" {
#endif

#include "cipher.h"
#include "types.h"
#include "string.h"
#include "sys.h"

/* 参数判断相关函数定义*/
#define CIPHER_CHECK_PTR(ptr) \
    do{\
        if (NULL == (ptr))\
        {\
            return CIPHER_NULL_PTR;\
        }\
    }while(0)


#define CIPHER_CHECK_ENUM(en, last) \
    do{\
        if ((UINT32)(en) >= (UINT32)(last))\
        {\
            return CIPHER_INVALID_ENUM;\
        }\
    }while(0)


#define CIPHER_REG_READ(base, reg, value) (value) = (*(volatile UINT32 *)((UINT32)(base) + (reg)))
#define CIPHER_REG_WRITE(base, reg, value) (*(volatile UINT32 *)((UINT32)(base) + (reg))) = (value)

/*****************************************************************************
* 函 数 名  : CIPHER_Init
*
* 功能描述  : Cipher初始化函数
*
* 输入参数  : void
* 输出参数  : 无
*
* 返 回 值  : 成功/失败
*
* 其它说明  : 无
*
*****************************************************************************/
INT32 CIPHER_Init(void *bdq_pt, void *rdq_pt)
{
    UINT32 u32Value = 0;
    INT32 s32Ret = CIPHER_SUCCESS;
	UINT32 i = 0;

	/*复位通道KDF_CHN_NUM*/
	CIPHER_REG_WRITE(CIPHER_REGBASE_ADDR, CIPHER_CHN_RESET(KDF_CHN_NUM), 7);
	for(i = 0; i < 1000; i++)
	{
		CIPHER_REG_READ(CIPHER_REGBASE_ADDR, CIPHER_CHN_RESET(KDF_CHN_NUM), u32Value);
		if(0 == (u32Value & 7))
			break;
		delay(1);/*睡1us*/
	}
	if(i == 1000)
	{
		return CIPHER_TIME_OUT;
	}

    /*通道BDQ空间获取*/
    memset(bdq_pt, 0, BDQ_SIZE);
    /*设置通道BDQ基地址*/
    CIPHER_REG_WRITE(CIPHER_REGBASE_ADDR, CIPHER_CHNBDQ_BASE(KDF_CHN_NUM), (UINT32)bdq_pt);
    /*设置通道BDQ大小*/
    CIPHER_REG_WRITE(CIPHER_REGBASE_ADDR, CIPHER_CHNBDQ_SIZE(KDF_CHN_NUM), 1);


	/*通道RDQ空间置0*/
    memset(rdq_pt, 0, RDQ_SIZE);
    /*设置通道RDQ基地址*/
    CIPHER_REG_WRITE(CIPHER_REGBASE_ADDR, CIPHER_CHNRDQ_BASE(KDF_CHN_NUM), (UINT32)rdq_pt);
    /*设置通道RDQ大小*/
    CIPHER_REG_WRITE(CIPHER_REGBASE_ADDR, CIPHER_CHNRDQ_SIZE(KDF_CHN_NUM), 1);


    /* 使能KDF通道*/
    CIPHER_REG_WRITE(CIPHER_REGBASE_ADDR, CIPHER_CHN_ENABLE(KDF_CHN_NUM), 0x1);
    /* 配置KDF通道config寄存器*/
    u32Value = CHN_IV_SEL \
               | (CHN_RDQ_CTRL << CHN_RDQCTRL_BIT) \
               | KDF_CHN_USRFIELD_LEN << CHN_USRFIELDLEN_BIT;
    CIPHER_REG_WRITE(CIPHER_REGBASE_ADDR, CIPHER_CHN_CONFIG(KDF_CHN_NUM), u32Value);

    /* KeyRam中的内容设置为0，只使用了0号索引位置*/
	for(i = 0; i < 8/*1个key最长8 words*/; i++)
		*(((UINT32*)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET)) + i) = 0;

    return s32Ret;
}

INT32 CIPHER_KdfBDConfigure(KDF_BDCONFIG_INFO_S *stKeyCfgInfo)
{
    KDF_BD_SCPT_S *pWaddr = NULL;
	UINT32 reg_val_bdq_wptr;

	CIPHER_REG_READ(CIPHER_REGBASE_ADDR, CIPHER_CHNBDQ_WRPTR(KDF_CHN_NUM), reg_val_bdq_wptr);
	pWaddr = (KDF_BD_SCPT_S *)reg_val_bdq_wptr;
	if(!pWaddr)
	{
		return CIPHER_NULL_PTR;
	}

    pWaddr->u32KdfConfig=
                    0              								|
                    (stKeyCfgInfo->enShaKeySource & 0x3) << 0x2 |
                    (stKeyCfgInfo->enShaSSource & 0x1)   << 0x4 |
                    (stKeyCfgInfo->u32ShaKeyIndex & 0xf) << 0x7 |
                    (stKeyCfgInfo->u32ShaSIndex & 0xf)   << 0xb |
                    (stKeyCfgInfo->u32Length & 0x1ff)    << 0xf |
                    (stKeyCfgInfo->u32DestIndex & 0xf)   << 0x18;

    pWaddr->u32Address = (UINT32)(stKeyCfgInfo->pAddress);

    return CIPHER_SUCCESS;
}

INT32 Cipher_StartKdfChannel()
{
    UINT32 u32RegValue;
	UINT32 reg_val_bdq_size;
    KDF_RD_SCPT_S *pCurRDAddr = NULL;
	UINT32 delay_loops = 20000;/*循环20000次，每次延迟1us，共延迟20000us*/

	CIPHER_REG_READ(CIPHER_REGBASE_ADDR, CIPHER_CHNBDQ_RWPTR(KDF_CHN_NUM), u32RegValue);
	u32RegValue &= 0x3FF;
	u32RegValue += 1;
	CIPHER_REG_READ(CIPHER_REGBASE_ADDR, CIPHER_CHNBDQ_SIZE(KDF_CHN_NUM), reg_val_bdq_size);
	/* 移动寄存器写指针 */
	CIPHER_REG_WRITE(CIPHER_REGBASE_ADDR, CIPHER_CHNBDQ_RWPTR(KDF_CHN_NUM), u32RegValue % (1 + reg_val_bdq_size));

	do
    {
        CIPHER_REG_READ(CIPHER_REGBASE_ADDR, CIPHER_CHN_ENABLE(KDF_CHN_NUM),u32RegValue);
		--delay_loops;
		delay(1);/*睡1us*/
    }
    while((u32RegValue & 0x80000000) && (delay_loops));
	if(!delay_loops)
	{
		return CIPHER_TIME_OUT;
	}

    /* 获取当前的RD描述符*/
	CIPHER_REG_READ(CIPHER_REGBASE_ADDR, CIPHER_CHNRDQ_RDPTR(KDF_CHN_NUM), u32RegValue);
    pCurRDAddr = (KDF_RD_SCPT_S *)u32RegValue;
    /* 提交操作为阻塞类型时，需要移动RD*/
    /* 清除RD有效位*/
    pCurRDAddr->u32KdfConfig &= (~0x80000000);
    /* 判断是否有完整性检查错误*/
    if(CIPHER_STAT_CHECK_ERR == ((pCurRDAddr->u32KdfConfig >> 29) & 0x3))
    {
        return CIPHER_CHECK_ERROR;
    }
	/* 移动寄存器中的RD地址*/
	CIPHER_REG_READ(CIPHER_REGBASE_ADDR, CIPHER_CHNRDQ_RWPTR(KDF_CHN_NUM), u32RegValue);
	u32RegValue &= 0x3FF0000;
	u32RegValue = u32RegValue >> 16;
	u32RegValue = (++u32RegValue) % (reg_val_bdq_size + 1);
	u32RegValue = u32RegValue << 16;
	CIPHER_REG_WRITE(CIPHER_REGBASE_ADDR, CIPHER_CHNRDQ_RWPTR(KDF_CHN_NUM), u32RegValue);

    return CIPHER_SUCCESS;
}

/*****************************************************************************
* 函 数 名  : BSP_KDF_KeyMake
*
* 功能描述  : KDF计算，用于产生新的Key
*
* 输入参数  : stKeyCfgInfo: KDF运算时参数sha_key属性配置
            : stKeyCfgInfo: KDF运算时参数sha_s属性配置
            : enDestIndex : KDF运算产生的Key放置在KeyRam中的位置
* 输出参数  : 无
* 返 回 值  : 成功:OK
*           : 失败:BSP_ERROR
*****************************************************************************/
INT32 KDF_KeyMake(KEY_CONFIG_INFO_S *pstKeyCfgInfo, S_CONFIG_INFO_S *pstSCfgInfo,
                        UINT32 u32DestIndex, KEY_MAKE_S *pstKeyMake)
{
    KDF_BDCONFIG_INFO_S stBDCfgInfo = {0};

    stBDCfgInfo.enShaKeySource = pstKeyCfgInfo->enShaKeySource;
    stBDCfgInfo.enShaSSource   = pstSCfgInfo->enShaSSource;
    stBDCfgInfo.u32ShaKeyIndex  = pstKeyCfgInfo->u32ShaKeyIndex;
    stBDCfgInfo.u32ShaSIndex    = pstSCfgInfo->u32ShaSIndex;
    stBDCfgInfo.u32Length      = pstSCfgInfo->u32ShaSLength - 1;/*应该减1，逻辑会加1*/
    stBDCfgInfo.u32DestIndex   = u32DestIndex;
    stBDCfgInfo.pAddress       = pstSCfgInfo->pSAddr;

    CIPHER_KdfBDConfigure(&stBDCfgInfo);

    if(CIPHER_SUCCESS != Cipher_StartKdfChannel())
    {
    	return CIPHER_CHECK_ERROR;
    }
	/* 上层需要保存Key值到目的地址*/
	if(pstKeyMake->enKeyOutput == CIPHER_KEY_OUTPUT)
	{
		if(!(pstKeyMake->stKeyGet.pKeyAddr) || !(pstKeyMake->stKeyGet.penOutKeyLen))
		{
			return CIPHER_NULL_PTR;
		}

		/* 将Key的长度反馈给上层*/
		(*(pstKeyMake->stKeyGet.penOutKeyLen)) = (UINT32)CIPHER_KEY_L256;

		/* 如果实际Key的长度大于上层希望的长度，返回错误*/
		if((UINT32)CIPHER_KEY_L256 > (pstKeyMake->stKeyGet.enKeyLen))
		{
			return CIPHER_KEYLEN_ERROR;
		}
	}
    return CIPHER_SUCCESS;
}

int bsp_calc_hash(unsigned long * s_addr, long size, unsigned long * key_addr,
	unsigned long * out_hash, unsigned long * out_hash_len)
{
	struct Data_Buff BD_RD_BUFF = {{0}, {0}};
	KEY_CONFIG_INFO_S key_input = {0};
	S_CONFIG_INFO_S s_input = {0};
	int ret = CIPHER_SUCCESS;
	UINT32	index = 0;
	KEY_MAKE_S key_make_out = {0};
	KEY_GET_S key_get = {0};
	int i = 0;

	if(!s_addr || !size || !key_addr || !out_hash || ! out_hash_len)
	{
		return CIPHER_NULL_PTR;
	}

	key_input.enShaKeySource = SHA_KEY_SOURCE_KEYRAM;
	key_input.pKeySourceAddr = key_addr;
	key_input.u32ShaKeyIndex = index;

	s_input.enShaSSource = SHA_S_SOURCE_DDR;
	s_input.pSAddr = s_addr;
	s_input.u32ShaSLength = size >= KDF_SHA_S_LENGTH_MAX ? KDF_SHA_S_LENGTH_MAX : size;
	s_input.u32ShaSIndex = index;

	key_get.enKeyLen = 256;
	key_get.pKeyAddr = out_hash;
	key_get.penOutKeyLen = out_hash_len;
	*(key_get.penOutKeyLen) = 0;

	key_make_out.enKeyOutput = CIPHER_KEY_OUTPUT;
	key_make_out.stKeyGet = key_get;

	CIPHER_Init(&(BD_RD_BUFF.BD_BUFF[0]), &(BD_RD_BUFF.RD_BUFF[0]));
	/*将传入的key先放入key ram的0号位置*/
	memcpy((void*)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET), key_addr, 32);//key最大长度256bit，即32bytes

	do
	{
		ret = KDF_KeyMake(&key_input, &s_input, index, &key_make_out);
		if(CIPHER_SUCCESS != ret)
			return ret;

		size -= s_input.u32ShaSLength;
		s_addr = (unsigned long *)(((UINT8*)s_addr) + s_input.u32ShaSLength);
		s_input.pSAddr = s_addr;

		key_input.enShaKeySource = SHA_KEY_SOURCE_RESULT;
		s_input.u32ShaSLength = size >= KDF_SHA_S_LENGTH_MAX ? KDF_SHA_S_LENGTH_MAX : size;
	}while(size > 0);

	/* 获得KeyMake生成的Key的地址*/
	memcpy(out_hash, CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET, CIPHER_KEY_LEN);
	/*反转最后结果*/
	for(i = 0; i < CIPHER_KEY_LEN/sizeof(UINT32); i++)
	{
		*(out_hash + i) = CIPHER_BSWAP32(*(out_hash + i));
	}

	return ret;
}

#ifdef __cplusplus
}
#endif


