/*************************************************************************
*   版权所有(C) 2008-2013, 深圳华为技术有限公司.
*
*   文 件 名 :  kdf.c
*
*   作    者 :  wangxiandong
*
*   描    述 :  kdf计算密钥实现，为一键升级计算HASH定制功能
*
*   修改记录 :  2013年06月19日  v1.00  wangxiandong 创建
*************************************************************************/
#include <linux/gfp.h>
#include <osl_module.h>
#include <linux/dma-mapping.h>
#include <osl_bio.h>
#include "kdf.h"

/*lint --e{550} */

int kdf_start_channel(void);
void kdf_move_bdq_w_pos(void);
void kdf_move_rdq_r_pos(void);

static struct kdf_s ch4_info = {0, 0, 0, 0, BD_RD_NUM, 0, 0};

/*****************************************************************************
* 函 数 名  : kdf_reg_init
*
* 功能描述  : A核KDF寄存器配置，用于计算HASH值，在C核启动之前使用
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
void kdf_reg_init(void)
{
	u32 reg_val = 0;
	int i = 0;

	/*复位ch4*/
	writel(0x7, HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_SOFTRESET_OFFSET);
	i = 1000;
	do
	{
		reg_val = readl(HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_SOFTRESET_OFFSET);
		i--;
	}while((0 != (reg_val & 0x7)) && (i > 0));
	if(i <= 0)
	{
		return;
	}

	/*配置BD寄存器*/
	writel(ch4_info.bdq_real_addr, HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_BDQ_BADDR_OFFSET);
	writel(BD_RD_NUM - 1, HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_BDQ_SIZE_OFFSET);

	/*配置RD寄存器*/
	writel(ch4_info.rdq_real_addr, HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_RDQ_BADDR_OFFSET);
	writel(BD_RD_NUM - 1, HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_RDQ_SIZE_OFFSET);

    /*使能KDF通道*/
	writel(0x1, HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_EN_OFFSET);

    /* 配置KDF通道config寄存器*/
    reg_val= CHN_IV_SEL| (CHN_RDQ_CTRL << CHN_RDQCTRL_BIT)| KDF_CHN_USRFIELD_LEN << CHN_USRFIELDLEN_BIT;/* [false alarm]:误报 */
	writel(reg_val, HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_CONFIG_OFFSET);

    /* KeyRam中的内容设置为0，只使用了0号索引位置*/
    for(i = 0; i < KDF_SHA_KEY_LENGTH; i += 4)
    {
        writel(0, (volatile void *)(HI_CIPHER_BASE_ADDR_VIRT + HI_KEY_RAM_OFFSET + KEY_INDEX * CIPHER_KEY_LEN + i));/*lint !e737*/
    }

}

/*****************************************************************************
* 函 数 名  : kdf_init
*
* 功能描述  : A核KDF队列空间申请初始化，用于计算HASH值，在C核启动之前使用
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 0    : 正确
*             其他 : 错误
*****************************************************************************/
int kdf_init(void)
{
	void * virtual_rdq_pt = NULL;
	u32  rdq_pt           = 0;
	void * virtual_bdq_pt = NULL;
	u32  bdq_pt           = 0;

	/*申请RDQ和BDQ空间*/
	#pragma pack(8)
	virtual_rdq_pt = dma_alloc_coherent(NULL, RDQ_SIZE, &rdq_pt, GFP_KERNEL);
	virtual_bdq_pt = dma_alloc_coherent(NULL, BDQ_SIZE, &bdq_pt, GFP_KERNEL);
	#pragma pack()
	ch4_info.sha_s_buff_virt_addr = (u32)dma_alloc_coherent(NULL, KDF_SHA_S_LENGTH_MAX, &(ch4_info.sha_s_buff_real_addr), GFP_KERNEL);
	if(!rdq_pt || !virtual_rdq_pt || !bdq_pt || !virtual_bdq_pt || !(ch4_info.sha_s_buff_virt_addr) || !(ch4_info.sha_s_buff_real_addr))
	{
		dma_free_coherent(NULL, RDQ_SIZE, virtual_rdq_pt, rdq_pt);
		dma_free_coherent(NULL, BDQ_SIZE, virtual_bdq_pt, bdq_pt);
		return CIPHER_NULL_PTR;
	}
	if((rdq_pt & 0x7) || (bdq_pt & 0x7) || ((u32)virtual_bdq_pt & 0x7) || ((u32)virtual_rdq_pt & 0x7))
	{
		dma_free_coherent(NULL, RDQ_SIZE, virtual_rdq_pt, rdq_pt);
		dma_free_coherent(NULL, BDQ_SIZE, virtual_bdq_pt, bdq_pt);
		return CIPHER_ALIGN_ERROR;
	}
	memset(virtual_rdq_pt, 0, RDQ_SIZE);
	memset(virtual_bdq_pt, 0, BDQ_SIZE);

	ch4_info.bdq_virt_addr = (u32)virtual_bdq_pt;
	ch4_info.bdq_real_addr = bdq_pt;
	ch4_info.rdq_virt_addr = (u32)virtual_rdq_pt;
	ch4_info.rdq_real_addr = rdq_pt;

    return CIPHER_SUCCESS;
}

int kdf_bd_configure(KDF_BDCONFIG_INFO_S *stKeyCfgInfo)
{
    KDF_BD_SCPT_S *p_w_addr = (KDF_BD_SCPT_S *)(ch4_info.bdq_virt_addr);
	u32 reg_val_bdq_ptr;

	reg_val_bdq_ptr = readl(HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_BDQ_PTR_OFFSET);
	reg_val_bdq_ptr = reg_val_bdq_ptr & 0x3FF;
	p_w_addr += reg_val_bdq_ptr;

    p_w_addr->u32KdfConfig= 0                                           |
		                    (stKeyCfgInfo->enShaKeySource & 0x3) << 0x2 |
                            (stKeyCfgInfo->enShaSSource & 0x1)   << 0x4 |
                            (stKeyCfgInfo->u32ShaKeyIndex & 0xf) << 0x7 |
                            (stKeyCfgInfo->u32ShaSIndex & 0xf)   << 0xb |
                            (stKeyCfgInfo->u32Length & 0x1ff)    << 0xf |
                            (stKeyCfgInfo->u32DestIndex & 0xf)   << 0x18;

    p_w_addr->u32Address = (u32)(stKeyCfgInfo->pAddress);

    return CIPHER_SUCCESS;
}

void kdf_move_rdq_r_pos(void)
{
	u32 reg_val_rdq_rwptr;
	u32 reg_val_rdq_size;

	reg_val_rdq_rwptr = readl(HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_RDQ_PTR_OFFSET);
	reg_val_rdq_size  = readl(HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_RDQ_SIZE_OFFSET);
	reg_val_rdq_rwptr = (reg_val_rdq_rwptr & 0x3FF0000);
	reg_val_rdq_rwptr = reg_val_rdq_rwptr >> 16;
	reg_val_rdq_rwptr += 1;
	reg_val_rdq_size++;
	reg_val_rdq_rwptr = reg_val_rdq_rwptr % reg_val_rdq_size;
	/*reg_val_rdq_rwptr = ((reg_val_rdq_rwptr & 0x3FF0000) >> 16 + 1) % (1 + reg_val_rdq_size);*/
	reg_val_rdq_rwptr = reg_val_rdq_rwptr << 16;/* [false alarm]:误报 */
	writel(reg_val_rdq_rwptr, HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_RDQ_PTR_OFFSET);
}

void kdf_move_bdq_w_pos(void)
{
	u32 reg_val_bdq_rwptr;
	u32 reg_val_bdq_size;

	reg_val_bdq_rwptr = readl(HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_BDQ_PTR_OFFSET);
	reg_val_bdq_size  = readl(HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_BDQ_SIZE_OFFSET);
	reg_val_bdq_rwptr = reg_val_bdq_rwptr & 0x3FF;
	reg_val_bdq_rwptr += 1;
	reg_val_bdq_size++;
	reg_val_bdq_rwptr = reg_val_bdq_rwptr % reg_val_bdq_size;/* [false alarm]:误报 */
	/*reg_val_bdq_rwptr = (reg_val_bdq_rwptr & 0x3FF + 1) % (1 + reg_val_bdq_size);*/
	writel(reg_val_bdq_rwptr, HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_BDQ_PTR_OFFSET);
}

int kdf_start_channel(void)
{
    u32 reg_val;
    KDF_RD_SCPT_S *pCurRDAddr = NULL;
	s32 delay_loops = 20000;/*循环20000次，每次延迟1us，共延迟20000us*/

	/* 移动寄存器写指针 */
	kdf_move_bdq_w_pos();

	do
    {
		reg_val = readl(HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_EN_OFFSET);
        delay_loops = delay_loops - 1;
		/*delay(1);睡1us*/
    }
    while((reg_val & 0x80000000) && (delay_loops > 0));
	if(delay_loops <= 0)
	{
		return CIPHER_TIME_OUT;
	}

    /* 获取当前的RD描述符*/ 
	reg_val = readl((volatile void *)(HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_RDQ_PTR_OFFSET));
	reg_val = (reg_val & 0x3FF0000) >> 16;
    pCurRDAddr = (KDF_RD_SCPT_S *)(ch4_info.rdq_virt_addr) + reg_val;
    /* 清除RD有效位*/
    pCurRDAddr->u32KdfConfig &= (~0x80000000);
    /* 判断是否有完整性检查错误*/
    if(CIPHER_STAT_CHECK_ERR == ((pCurRDAddr->u32KdfConfig >> 29) & 0x3))
    {
        return CIPHER_CHECK_ERROR;
    }
	/* 移动寄存器中的RD读地址*/
	kdf_move_rdq_r_pos();

    return CIPHER_SUCCESS;
}

int kdf_hash_make(void * sha_s_addr, u32 sha_s_len)
{
    KDF_BDCONFIG_INFO_S stBDCfgInfo;
	u32 i = 0;

    stBDCfgInfo.enShaKeySource  = SHA_KEY_SOURCE_KEYRAM;           /*sha_key总是去key ram里取*/ 
    stBDCfgInfo.u32ShaKeyIndex  = KEY_INDEX;                       /*sha_s在key ram中的位置*/
    stBDCfgInfo.u32ShaSIndex    = 0;                               /*sha_s总是来源于DDR，索引位置无所谓*/
    stBDCfgInfo.u32Length       = sha_s_len - 1;                   /*应该减1，逻辑会加1*/
    stBDCfgInfo.u32DestIndex    = KEY_INDEX;                       /*生成的HASH结果固定放在key ram的第15个位置*/
    stBDCfgInfo.pAddress        = (void*)(ch4_info.sha_s_buff_real_addr);   /*sha_s内存地址*/
	stBDCfgInfo.enShaSSource    = SHA_S_SOURCE_DDR;                /*sha_s总是来源于DDR*/

	for(i = 0; i < sha_s_len; i++)
	{
		*((u8 *)(ch4_info.sha_s_buff_virt_addr) + i) = *((u8*)sha_s_addr + i);
	}
    kdf_bd_configure(&stBDCfgInfo);
    return kdf_start_channel();
}
int kdf_hash_setkey(void * key_addr)
{
	int i = 0;
    u32 val = 0;
	
	if(!key_addr)
	{
		return CIPHER_NULL_PTR;
	}
	for(i = 0; i < CIPHER_KEY_LEN; i += 4)
	{
        val = (u32)readl((volatile void *)((char*)key_addr + i));
		writel(val, (volatile void *)(HI_CIPHER_BASE_ADDR_VIRT + HI_KEY_RAM_OFFSET + KEY_INDEX * CIPHER_KEY_LEN + i));/*lint !e737*/
	}
	return 0;
}

int kdf_result_hash_get(void * out_hash)
{
	int i = 0;
    u32 val = 0;
	
	if(!out_hash)
		return CIPHER_NULL_PTR;

    for(i = 0; i < CIPHER_KEY_LEN; i += 4)
    {
        val = readl((volatile void *)(HI_CIPHER_BASE_ADDR_VIRT + HI_KEY_RAM_OFFSET + KEY_INDEX * CIPHER_KEY_LEN + i));/*lint !e737*/
        writel(val, (volatile void *)((char*)out_hash + i));
    }
	return 0;
}

int kdf_hash_init(void)
{
	if(kdf_init())
		return -1;
	kdf_reg_init();
	return 0;
}
