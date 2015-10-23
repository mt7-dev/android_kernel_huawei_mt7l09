

/*lint --e{537,438}*/
#include <osl_common.h>
#include <bsp_memmap.h>
#include <drv_version.h>
#include <edma_balong.h>

/*******************************************************************************
函数名:     s32 bsp_edma_state(void)
函数描述:   判断 edma 是否在工作 ， 深睡前调用。
输入参数:   u32 channel_id
输出参数:   无
返回值:     busy：非0,非-1
            idle：0
            error: -1
*******************************************************************************/
s32 bsp_edma_channel_state(u32 channel_id)
{
    u32 ch_stat;
    if(channel_id>=16)
    {
        printk("bsp_edma_channel_state channel_id=%d error", channel_id);
        return -1;
    }
    ch_stat = readl(HI_EDMA_REGBASE_ADDR_VIRT + HI_CH_STAT_OFFSET);
    return (ch_stat & ((u32)0x1<<channel_id));/*lint !e713*/
}

/*******************************************************************************
函数名:     s32 bsp_edma_state(void)
函数描述:   判断 edma 是否在工作 ， 深睡前调用。
输入参数:   无
输出参数:   无
返回值:     busy：非0
            idle：0
*******************************************************************************/
u32 bsp_edma_state(void)
{
    u32 edma16_ch_stat;
    /*u32 edma4_ch_stat;*/
    edma16_ch_stat = readl(HI_EDMA_REGBASE_ADDR_VIRT + HI_CH_STAT_OFFSET);
#ifdef HI_EDMA_CH4_REGBASE_ADDR_VIRT
/*    if(BOARD_TYPE_GUL == bsp_get_board_mode_type())
    {
        edma4_ch_stat = readl(HI_EDMA_CH4_REGBASE_ADDR_VIRT + HI_CH_STAT_OFFSET);
    }
    else
    {
    }
    edma16_ch_stat = edma16_ch_stat|edma4_ch_stat;
*/
#endif
    return edma16_ch_stat;
}


/*******************************************************************************
函数名:     u32 bsp_edma_m2m_chan(u32 chan, u32 src_addr, u32 dest_addr, u32 len)

函数描述:   启动一次 EDMA传输。启动EDMA传输后，就返回，需要用户通过接口
            判断通道是否搬移完成。

输入参数:   channel_id：通道ID, TL DSP 搬移 TCM 使用 0 / 1 通道, 低功耗复用。
            src_addr：数据传输源地址，必须是物理地址
            des_addr：数据传输目的地址，必须是物理地址
            len：数据传输长度，单位：字节；
                可支持 len<65535字节的任意长度。
                当len>=64k，需要使用二维传输，要求 len 4k(0x1000)对齐
输出参数:   无
返回值:     成功：0
            失败：负数
*******************************************************************************/
__ao_func s32 bsp_edma_m2m_channel(u32 channel_id, u32 src_addr, u32 dest_addr, u32 len)
{
    u32 cx_config;
    u32 bindx;
    u32 cnt0, a_count, b_count;

    u32 edma_cfg_base;
    edma_cfg_base = HI_EDMA_REGBASE_ADDR_VIRT + 0x40*channel_id;

    cx_config = EDMA_CONFIG_FAST;

    writel(src_addr , edma_cfg_base + HI_CX_SRC_ADDR_0_OFFSET);
    writel(dest_addr, edma_cfg_base + HI_CX_DES_ADDR_0_OFFSET);
    writel(cx_config, edma_cfg_base + HI_CX_CONFIG_0_OFFSET);

    if(len<(SZ_64K-1))
    {
        writel(len, edma_cfg_base + HI_CX_CNT0_0_OFFSET);
    }
    else
    {
        b_count = (!(len%SZ_16K)) ? (len/SZ_16K - 1) \
                : (!(len%SZ_8K )) ? (len/SZ_8K  - 1) \
                : (!(len%SZ_4K )) ? (len/SZ_4K  - 1) : 0;
        if(0==b_count)
        {
            printk("try 2vec failed! not aligned. channel:%d size:0x%x \n",channel_id ,len);
            return -1;
        }
        else
        {
            a_count = len / (b_count+1);
            bindx = ( (a_count << 16) | a_count );
            cnt0  =  (b_count<<16)|a_count;
            writel( bindx, edma_cfg_base + HI_CX_BINDX_OFFSET);
            writel( cnt0 , edma_cfg_base + HI_CX_CNT0_0_OFFSET);
        }
    }
    writel((cx_config|EDMA_CONFIG_CH_EN), edma_cfg_base + HI_CX_CONFIG_0_OFFSET);
    return 0;
}


