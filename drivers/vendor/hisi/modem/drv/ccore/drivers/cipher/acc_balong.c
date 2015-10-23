
#ifdef __cplusplus
extern "C" {
#endif

#include <vxWorks.h>
#include <logLib.h>
#include <memLib.h>
#include <intLib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cacheLib.h>
#include <vxAtomicLib.h>
#include <semLib.h>
#include <taskLib.h>
#include <product_config.h>
#include <osl_bio.h>
#include <bsp_om.h>
#include "cipher_balong.h"
#include "acc_balong.h"
/*lint --e{661, 831} */
s32 bsp_acc_init(void);
void bsp_acc_debug_show(void);

struct acc_chx_mgr_s st_acc_chx_mgr = {{{0, ACC_STAT_IDLE, 0}, {0, ACC_STAT_IDLE, 0}},
	                                   {0, 0, 0, 0, 0},
	                                   ACC_FIFO_MAX_DEEP,
	                                   0,
	                                   0};


/*****************************************************************************
* 函 数 名  : bsp_acc_init
*
* 功能描述  : 初始化组包加速模块
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 成功/失败
*
* 修改记录  : 2011年1月8日   wangjing  creat
             2014年1月16日  wangxiandong  modify
*****************************************************************************/
s32 bsp_acc_init(void)
{
    u32 fifo_addr[ACC_FIFO_NUM] = {0};
    u32 size, i;


    if (TRUE == st_acc_chx_mgr.init_flag)
    {
        return OK;
    }
	st_acc_chx_mgr.acc_fifo_deep = ACC_FIFO_MAX_DEEP;

    /* 计算单个BDFIFO的尺寸 */
    size = ACC_FIFO_MAX_DEEP * CIPHER_DESC_SIZE;

    /*为ACC_FIFO_NUM个BDFIFO申请内存空间*/
	fifo_addr[0] = (u32)CIPHER_MALLOC((size + ALIN_BYTES) * ACC_FIFO_NUM + ALIN_BYTES);
	if(0 == fifo_addr[0])
	{
		ACC_ERROR_PRINT(CIPHER_NO_MEM);
        return CIPHER_NO_MEM;
	}
	memset((void*)fifo_addr[0], 0, (size + ALIN_BYTES) * ACC_FIFO_NUM + ALIN_BYTES);
    fifo_addr[0] = ROUND_UP(fifo_addr[0], ALIN_BYTES);
	if(fifo_addr[0] & 7)
	{
		ACC_ERROR_PRINT(CIPHER_ALIGN_ERROR);
		return CIPHER_ALIGN_ERROR;
	}
	fifo_addr[0] += ALIN_BYTES;
	fifo_addr[1] = (fifo_addr[0] + size + ALIN_BYTES);
	if(fifo_addr[1] & 7)
	{
		ACC_ERROR_PRINT(CIPHER_ALIGN_ERROR);
		return CIPHER_ALIGN_ERROR;
	}
	for(i = 0; i < ACC_FIFO_NUM; i++)
	{
		writel(i, (fifo_addr[i] - sizeof(u32)));
	}

    /*初始化全局变量acc_fifo_mgr[2] ，写入申请到的内存的首地址*/
    for (i = 0; i < ACC_FIFO_NUM; i++)
    {
        st_acc_chx_mgr.acc_fifo_mgr[i].fifo_stat = ACC_STAT_IDLE;
        st_acc_chx_mgr.acc_fifo_mgr[i].bd_fifo_addr   = fifo_addr[i];
        st_acc_chx_mgr.acc_fifo_mgr[i].bd_fifo_cfg_num = 0;
    }

    st_acc_chx_mgr.init_flag= TRUE;

    return CIPHER_SUCCESS;
}

/*****************************************************************************
* 函 数 名  : bsp_acc_get_bdfifo_addr
*
* 功能描述  : 用于获取当前可用的BDFIFO首地址
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值  : BDFIFO首地址
*****************************************************************************/
u32 bsp_acc_get_bdfifo_addr()
{
    u32 fifo_base_addr = ACC_FIFO_ERROR;
    u32 i = 0;
	u32 reg_val = 0;
	u32 reg_fifo_addr = 0;

	if(cipher_open_clk())
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_CIPHER,"CIPHER:fail to open clk\n");
		return CIPHER_UNKNOWN_ERROR;
	}
	/* 更新fifo状态 */
	reg_val = readl(CIPHER_REGBASE_ADDR + CIPHER_CHN_ENABLE(ACC_CHN_NUM));
	reg_val &= CHN_STATBIT;
	//reg_fifo_addr = readl(CIPHER_REGBASE_ADDR + CIPHER_CHNBDQ_BASE(ACC_CHN_NUM));
	reg_fifo_addr = st_acc_chx_mgr.acc_fifo_mgr[st_acc_chx_mgr.cur_used_fifo_num].bd_fifo_addr;
	for(i = 0; i < ACC_FIFO_NUM; i++)
	{
		if(st_acc_chx_mgr.acc_fifo_mgr[i].fifo_stat == ACC_STAT_WORK)
		{
			if((i == readl(reg_fifo_addr - sizeof(u32))) && ((!reg_val)))
			{
				continue;
			}
			else
			{
				st_acc_chx_mgr.acc_fifo_mgr[i].fifo_stat = ACC_STAT_IDLE;
			}
		}
	}

	/*第一遍循环，寻找ACC_STAT_IDLE状态FIFO*/
	for(i = 0; i < ACC_FIFO_NUM; i++)
	{
		if(st_acc_chx_mgr.acc_fifo_mgr[i].fifo_stat == ACC_STAT_IDLE)
		{
			fifo_base_addr = st_acc_chx_mgr.acc_fifo_mgr[i].bd_fifo_addr;
			goto GET_FIFO_ADDR_EXIT;
		}
	}
	/*第二遍循环，寻找ACC_STAT_CFG或者ACC_STAT_IDLE状态FIFO*/
	for(i = 0; i < ACC_FIFO_NUM; i++)
	{
		if((st_acc_chx_mgr.acc_fifo_mgr[i].fifo_stat == ACC_STAT_CFG) ||
			(st_acc_chx_mgr.acc_fifo_mgr[i].fifo_stat == ACC_STAT_IDLE))
		{
			fifo_base_addr = st_acc_chx_mgr.acc_fifo_mgr[i].bd_fifo_addr;
			goto GET_FIFO_ADDR_EXIT;
		}
	}

    if (ACC_FIFO_ERROR == fifo_base_addr)
    {
		ACC_ERROR_PRINT(ACC_ALL_FIFO_WORK);
        return ACC_ALL_FIFO_WORK;
    }

GET_FIFO_ADDR_EXIT:
	st_acc_chx_mgr.acc_fifo_mgr[i].bd_fifo_cfg_num = 0; 
	st_acc_chx_mgr.acc_debug.get_fifo_times++;

    return fifo_base_addr;
}

/*****************************************************************************
* 函 数 名  : bsp_acc_dma
*
* 功能描述  : 为指定通道配置描述符，用于单纯的DMA搬移
*
* 输入参数  : bd_fifo_addr: BDFIFO首地址
*                          p_in_mem_mgr    : 输入buffer数据块的首地址
*                          p_out_mem_mgr   : 输出buffer数据块的首地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acc_dma(u32 bd_fifo_addr, const void* p_in_mem_mgr, const void* p_out_mem_mgr, ACC_SINGLE_CFG_S *p_cfg)
{/*lint !e18*/
    struct cipher_bd_s *p_w_addr;
    u32 fifo_num = 0;

    /*参数检查，判断当前可用FIFO是FIFO1还是FIFO2*/
	fifo_num =  readl(bd_fifo_addr - sizeof(u32));
	if(fifo_num >= ACC_FIFO_NUM)
	{
		ACC_ERROR_PRINT(CIPHER_INVALID_NUM);
        return CIPHER_INVALID_NUM;
	}

    /*判断配置的BDFIFO的个数是否超过最大值，必须空出最后一个BD不使用，否则设置BDQ写指针时会导致回绕*/
    if (st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_cfg_num < (st_acc_chx_mgr.acc_fifo_deep - 1))
    {
        /*配置BD描述符*/
		p_w_addr = (struct cipher_bd_s *)(st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_addr);/* [false alarm]:误报 */
		p_w_addr += st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_cfg_num;/* [false alarm]:误报 */
        memset((void*)p_w_addr, 0, sizeof(struct cipher_bd_s));/* [false alarm]:误报 */

        /*InputAttr中只需配置数据块长度，OutputAttr配成0即可*/
        p_w_addr->input_attr  = (u32)p_cfg->u32BlockLen;
        p_w_addr->input_addr  = (u32)p_in_mem_mgr;
        p_w_addr->output_addr = (u32)p_out_mem_mgr;

        /*更新对应FIFO全局结构体，包括enFifoStat和u32BdFifoCfgNum*/
        st_acc_chx_mgr.acc_fifo_mgr[fifo_num].fifo_stat = ACC_STAT_CFG;
        st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_cfg_num += 1;

        st_acc_chx_mgr.acc_debug.cfg_dma_times++;
        return OK;
    }
    else
    {
        /*BDFIFO满，丢弃不配置*/
        st_acc_chx_mgr.acc_debug.drop_pak_times++;
		ACC_ERROR_PRINT(CIPHER_FIFO_FULL);
        return CIPHER_FIFO_FULL;
    }
}

/*****************************************************************************
* 函 数 名  : bsp_acc_cipher
*
* 功能描述  : 为指定通道配置描述符，用于需要Cipher进行加密的操作
*
* 输入参数  : bd_fifo_addr: BDFIFO首地址
*                          p_in_mem_mgr    : 输入buffer数据块的首地址
*                          p_out_mem_mgr   : 输出buffer数据块的首地址
*                          p_cfg       : 组包加速配置信息
* 输出参数   : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acc_cipher(u32 bd_fifo_addr, const void* p_in_mem_mgr, const void* p_out_mem_mgr, ACC_SINGLE_CFG_S *p_cfg)
{/*lint !e18*/
    struct cipher_bd_s *p_w_addr;
    u32 fifo_num = 0;

	fifo_num =  readl(bd_fifo_addr - sizeof(u32));
    if(fifo_num >= ACC_FIFO_NUM)
    {
		ACC_ERROR_PRINT(CIPHER_INVALID_NUM);
        return CIPHER_INVALID_NUM;
    }

    /*判断配置的BDFIFO的个数是否超过最大值，必须空出最后一个BD不使用，否则设置BDQ写指针时会导致回绕*/
	if (st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_cfg_num < (st_acc_chx_mgr.acc_fifo_deep - 1))
    {
        /*配置BD描述符*/
		p_w_addr = (struct cipher_bd_s *)(st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_addr);/* [false alarm]:误报 */
		p_w_addr += st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_cfg_num;/* [false alarm]:误报 */
		memset(p_w_addr, 0, sizeof(struct cipher_bd_s));/* [false alarm]:误报 */

        /*Cipher属性配置*/
        p_w_addr->cipher_cfg = ((p_cfg->stAlgKeyInfo.u32KeyIndexSec << 20) | /*加密操作所需要key的序号*/
                                (p_cfg->u32AppdHeaderLen<< 18)             | /*附加包头的长度*/
                                (p_cfg->u32HeaderLen<< 16)                 | /*包头packer head的长度*/
                                (1 << 12)                                  | /*AES算法加解密操作模式为CTR模式*/
                                (p_cfg->stAlgKeyInfo.enAlgSecurity << 4)   | /*加密操作配置*/
                                (1 << 1));                                   /*进行加密操作*/

        /*配置OutputAttr,InputAttr*/
        if ((p_cfg->u32Offset != 0) || (p_cfg->u32OutLen < p_cfg->u32BlockLen))
        {
            p_w_addr->input_attr  = (1 << 19);       /*选取结果中从offset开始到offset＋obuf_len之内的结果输出*/
            p_w_addr->output_attr = p_cfg->u32Offset << 16;
        }

        p_w_addr->input_attr |= p_cfg->u32BlockLen;
        p_w_addr->input_addr   = (u32)p_in_mem_mgr;
        p_w_addr->output_attr |= p_cfg->u32OutLen;
        p_w_addr->output_addr  = (u32)p_out_mem_mgr;

        /*配置Aph*/
        p_w_addr->aph_attr = ((p_cfg->u32BearId << 27) |
                             (p_cfg->u32Aph << ((0x3 - p_cfg->u32AppdHeaderLen) << 0x3) & 0x00FFFFFF));
        p_w_addr->count = p_cfg->u32Count;

        /*更新对应FIFO全局结构体*/
        st_acc_chx_mgr.acc_fifo_mgr[fifo_num].fifo_stat = ACC_STAT_CFG;
        st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_cfg_num += 1;

        st_acc_chx_mgr.acc_debug.cfg_cipher_times++;

        return OK;
    }
    else
    {
        /*BDFIFO满，丢弃不配置*/
        st_acc_chx_mgr.acc_debug.drop_pak_times++;
		ACC_ERROR_PRINT(CIPHER_FIFO_FULL);
        return CIPHER_FIFO_FULL;
    }
}

/*****************************************************************************
* 函 数 名  : bsp_acc_get_status
*
* 功能描述  : 获取当前通道状态
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 通道忙/空闲
*****************************************************************************/
s32 bsp_acc_get_status()
{
    u32 chx_stat = 0;

	if(cipher_open_clk())
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_CIPHER,"CIPHER:fail to open clk\n");
		return CIPHER_UNKNOWN_ERROR;
	}
	chx_stat = readl(CIPHER_REGBASE_ADDR + CIPHER_CHN_ENABLE(ACC_CHN_NUM));
    chx_stat = chx_stat & CHN_STATBIT;
	chx_stat = chx_stat >> 30;
	return (s32)chx_stat;
}

/*****************************************************************************
* 函 数 名  : bsp_acc_enable
*
* 功能描述  : 使能组包加速
*
* 输入参数  : bd_fifo_addr : BDFIFO首地址
*
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acc_enable(u32 bd_fifo_addr)
{
    u32 chx_stat = 0;
    u32 ptr = 0;
    u32 fifo_num = 0;
    u32 ret;
    u32 last_bd;

	if(cipher_open_clk())
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_CIPHER,"CIPHER:fail to open clk\n");
		return CIPHER_UNKNOWN_ERROR;
	}
    /*禁止组包，将CHX_EN寄存器的第30位写0*/
	ret = readl(CIPHER_REGBASE_ADDR + CIPHER_CHN_ENABLE(ACC_CHN_NUM));
	writel(ret & CHN_PACK_ENBITS, CIPHER_REGBASE_ADDR + CIPHER_CHN_ENABLE(ACC_CHN_NUM));

    /*去使能当前通道*/
	chx_stat = readl(CIPHER_REGBASE_ADDR + CIPHER_CHN_ENABLE(ACC_CHN_NUM));
	writel(chx_stat & CHN_DENBITS, CIPHER_REGBASE_ADDR + CIPHER_CHN_ENABLE(ACC_CHN_NUM));

    /*配置通道0配置寄存器*/
	chx_stat = readl(CIPHER_REGBASE_ADDR + CIPHER_CHN_CONFIG(ACC_CHN_NUM));
	writel(chx_stat | CHN_CFGUSR, CIPHER_REGBASE_ADDR + CIPHER_CHN_CONFIG(ACC_CHN_NUM));

    /*申请清空BD队列，复位读指针*/
	ptr = readl(CIPHER_REGBASE_ADDR + CIPHER_CHN_RESET(ACC_CHN_NUM));
	writel(ptr | CHN_BDRESET, CIPHER_REGBASE_ADDR + CIPHER_CHN_RESET(ACC_CHN_NUM));

    /*将BDFIFO首地址写入寄存器*/
	writel(bd_fifo_addr, CIPHER_REGBASE_ADDR + CIPHER_CHNBDQ_BASE(ACC_CHN_NUM));

    /*获取当前可用FIFO序号*/
	fifo_num = readl(bd_fifo_addr - sizeof(u32));
	if(fifo_num >= ACC_FIFO_NUM)
	{
		ACC_ERROR_PRINT(CIPHER_INVALID_NUM);
        return CIPHER_INVALID_NUM;
	}

    /*更新全局结构体*/
    st_acc_chx_mgr.acc_fifo_mgr[fifo_num].fifo_stat = ACC_STAT_WORK;
	st_acc_chx_mgr.cur_used_fifo_num = fifo_num;

    /*设置最后一个bd为gen_Int_en ，这样硬件就可以通过中断将CHX_EN寄存器的第30自动置反*/
    last_bd = bd_fifo_addr + (st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_cfg_num - 1) * sizeof(struct cipher_bd_s);
	ptr = readl(last_bd + 0);
	writel(ptr | (1U << 13), last_bd + 0);
	//cacheFlush(DATA_CACHE,(void*)bd_fifo_addr,st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_cfg_num * sizeof(struct cipher_bd_s));

    /*更新写指针*/
	ptr = readl(CIPHER_REGBASE_ADDR + CIPHER_CHNBDQ_RWPTR(ACC_CHN_NUM));
    ptr = ptr & CHN_WPT;
    ptr = ptr | st_acc_chx_mgr.acc_fifo_mgr[fifo_num].bd_fifo_cfg_num;
	writel(ptr, CIPHER_REGBASE_ADDR + CIPHER_CHNBDQ_RWPTR(ACC_CHN_NUM));

    /*将通道标志位置为0，表示通道忙，使能组包加速*/
	chx_stat = readl(CIPHER_REGBASE_ADDR + CIPHER_CHN_ENABLE(ACC_CHN_NUM));
	writel(chx_stat | CHN_ENBITS, CIPHER_REGBASE_ADDR + CIPHER_CHN_ENABLE(ACC_CHN_NUM));

    st_acc_chx_mgr.acc_debug.enable_times++;
    return CIPHER_SUCCESS;
}

void bsp_acc_debug_show(void)
{
    printf("\rGetFifoAddrTimes:   %d\n", st_acc_chx_mgr.acc_debug.get_fifo_times);
    printf("\rCfgDmaTimes     :   %d\n", st_acc_chx_mgr.acc_debug.cfg_dma_times);
    printf("\rCfgCipherTimes  :   %d\n", st_acc_chx_mgr.acc_debug.cfg_cipher_times);
    printf("\rEnableTimes     :   %d\n", st_acc_chx_mgr.acc_debug.enable_times);
    printf("\rDropPakTimes    :   %d\n", st_acc_chx_mgr.acc_debug.drop_pak_times);

    return;
}

#ifdef __cplusplus
}
#endif
