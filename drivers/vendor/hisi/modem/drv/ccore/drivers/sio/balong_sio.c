/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : balong_sio.c
  版 本 号   :
  作    者   :
  生成日期   :
  最近修改   :
  功能描述   : SIO驱动软件
  函数列表   :
              bsp_sio_close
              bsp_sio_init
              bsp_sio_isr
              bsp_sio_open

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <vxWorks.h>
#include <intLib.h>
#include <logLib.h>
#include <stdio.h>
#include <string.h>
#include <memLib.h>
#include "hi_base.h"
#include "hi_sio.h"
#include "bsp_sio.h"
#include "soc_interrupts.h"
#include "hi_syssc_interface.h"
#include "hi_syscrg_interface.h"
#include "bsp_edma.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
drv_sio_ctrl_stru   g_stdrvsioctrl;    /* SIO模块控制全局变量 */

/*****************************************************************************
  3 函数实现
*****************************************************************************/

void dma_spk_out_isr(unsigned int uwPara);
void dma_mic_in_isr(unsigned int uwPara);

/*****************************************************************************
 函 数 名  : bsp_sio_dma_init
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_sio_dma_init()
{
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();
    int rx_channel_id = 0;
    int tx_channel_id = 0;

    memset((void*)&pstCfg->dma_cfg, 0 ,sizeof(dma_sio_cfg_stru));

    rx_channel_id = bsp_edma_channel_init((enum edma_req_id)EDMA_LTESIO_RX, (channel_isr)dma_mic_in_isr, (u32)0 , (u32)EDMA_INT_DONE | EDMA_INT_LLT_DONE);
    if(rx_channel_id < 0){
        logMsg("balong_dma_channel_init: req EDMA_LTESIO_RX failed\n",1,2,3,4,5,6);
        return ERROR;
    }

    pstCfg->dma_cfg.rx_channel_id = rx_channel_id;

    tx_channel_id = bsp_edma_channel_init((enum edma_req_id)EDMA_LTESIO_TX, (channel_isr)dma_spk_out_isr, (u32)0 , (u32)EDMA_INT_DONE | EDMA_INT_LLT_DONE);
    if(rx_channel_id < 0){
        logMsg("balong_dma_channel_init: req EDMA_LTESIO_TX failed\n",1,2,3,4,5,6);
        return ERROR;
    }

    pstCfg->dma_cfg.rx_channel_id = tx_channel_id;

    return OK;
}

/*****************************************************************************
 函 数 名  : bsp_sio_dma_cfg
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_sio_record_dma_cfg(unsigned int dst, unsigned int dst1)
{
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();
    unsigned int src = 0;

    src = (DRV_SIO_MODE_I2S == pstCfg->enMode)? HI_SIO_REG_ADDR(HI_SIO_I2S_LEFT_RD_OFFSET):HI_SIO_REG_ADDR(HI_SIO_PCM_RD_OFFSET);

    pstCfg->dma_cfg.record_cfg[0].uwLli = DRV_SIO_DMA_LLI_LINK((unsigned int)&pstCfg->dma_cfg.record_cfg[1]);
    pstCfg->dma_cfg.record_cfg[0].uhwACount = 320;  /* 带AMR每10ms采样80个点 */
    pstCfg->dma_cfg.record_cfg[0].uwSrcAddr = src;
    pstCfg->dma_cfg.record_cfg[0].uwDstAddr = dst;
    pstCfg->dma_cfg.record_cfg[0].uwConfig  = DRV_SIO_DMA_SIO_MEM_CFG;

    pstCfg->dma_cfg.record_cfg[1].uwLli = DRV_SIO_DMA_LLI_LINK((unsigned int)&pstCfg->dma_cfg.record_cfg[0]);
    pstCfg->dma_cfg.record_cfg[1].uhwACount = 320;  /* 窄带AMR每10ms采样80个点 */
    pstCfg->dma_cfg.record_cfg[1].uwSrcAddr = HI_SIO_REG_ADDR(HI_SIO_I2S_RIGHT_RD_OFFSET);
    pstCfg->dma_cfg.record_cfg[1].uwDstAddr = dst1;
    pstCfg->dma_cfg.record_cfg[1].uwConfig  = DRV_SIO_DMA_SIO_MEM_CFG;

    return OK;
}

/*****************************************************************************
 函 数 名  : bsp_sio_dma_cfg
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_sio_play_dma_cfg(unsigned int src, unsigned int src1)
{
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();

    pstCfg->dma_cfg.play_cfg[0].uwLli = DRV_SIO_DMA_LLI_LINK((unsigned int)&pstCfg->dma_cfg.play_cfg[1]);
    pstCfg->dma_cfg.play_cfg[0].uhwACount = 320;  /* 带AMR每10ms采样80个点 */
    pstCfg->dma_cfg.play_cfg[0].uwSrcAddr = HI_SIO_REG_ADDR(HI_SIO_I2S_LEFT_XD_OFFSET);
    pstCfg->dma_cfg.play_cfg[0].uwDstAddr = src;
    pstCfg->dma_cfg.play_cfg[0].uwConfig  = DRV_SIO_DMA_SIO_MEM_CFG;

    pstCfg->dma_cfg.play_cfg[1].uwLli = DRV_SIO_DMA_LLI_LINK((unsigned int)&pstCfg->dma_cfg.play_cfg[0]);
    pstCfg->dma_cfg.play_cfg[1].uhwACount = 320;  /* 窄带AMR每10ms采样80个点 */
    pstCfg->dma_cfg.play_cfg[1].uwSrcAddr = HI_SIO_REG_ADDR(HI_SIO_I2S_RIGHT_XD_OFFSET);
    pstCfg->dma_cfg.play_cfg[1].uwDstAddr = src1;
    pstCfg->dma_cfg.play_cfg[1].uwConfig  = DRV_SIO_DMA_SIO_MEM_CFG;

    return OK;
}

/*****************************************************************************
 函 数 名  : bsp_sio_dma_start
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_sio_dma_start(unsigned int channel_id)
{
    /*
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();

    (void)balong_dma_channel_get_lli_addr(channel_id);

    balong_dma_channel_set_config(channel_id,0,0,0);

    启动EDMA传输，等待传输完成信号量释放后返回
    balong_dma_channel_lli_start(channel_id);
    */

    return OK;
}


/*****************************************************************************
 函 数 名  : bsp_sio_dma_stop
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_sio_dma_stop(unsigned int channel_id)
{
    /* drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR(); */

    /* bsp_edma_channel_stop(channel_id); */

    return OK;
}

/*****************************************************************************
 函 数 名  : dma_spk_out_isr
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void dma_spk_out_isr(unsigned int uwPara)
{
    return ;
}

/*****************************************************************************
 函 数 名  : dma_mic_in_isr
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void dma_mic_in_isr(unsigned int uwPara)
{
    return ;
}

/*****************************************************************************
 函 数 名  : bsp_sio_set_mode
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_set_mode(drv_sio_mode_e mode)
{
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();

    pstCfg->enMode = mode;
}

/*****************************************************************************
 函 数 名  : bsp_sio_set_sync_mode
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_sync_mode(drv_sio_sync_mode_e mode)
{
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();
    unsigned int sync_div = 0;
    unsigned int bclk_div = 0;
    struct clk *sio_bclk;
    struct clk *sio_sync;

    pstCfg->enSyncMode = mode;

    if(DRV_SIO_SYNC_MODE_MASTER == mode)
    {
        sio_bclk = clk_get(NULL, "sio_bclk");
        sio_sync = clk_get(NULL, "sio_syncclk");

        if((NULL == sio_bclk) || (NULL == sio_sync))
        {
            logMsg("sio bclk clk or sio clk get failed\n",1,2,3,4,5,6);
            return;
        }

        /* 设置PCM时钟源 19.2M */
        hi_syscrg_pcm_clk_sel();

        /* 设置SIO 时钟与PAD时钟同向 */
        hi_syscrg_sio_clk_positive();

        /* 主模式 */
        hi_syssc_pcm_mode_master();

        /* 配置xfs分频 */
        switch(pstCfg->sample_rate)
        {
            case DRV_SIO_SAMPLING_8K:
            {
                sync_div = 96;
                bclk_div = 25;
            }
            break;
            case DRV_SIO_SAMPLING_16K:
            {
                sync_div = 48;
                bclk_div = 25;
            }
            break;
            default:
            {
                sync_div = 96;
                bclk_div = 25;
            }
            break;
        }

        /* 设置SIO master时时钟分频系数, 19200k÷96÷25=8k */
        if(0 != clk_set_rate(sio_bclk, bclk_div))
        {
            logMsg("line %d: clk_set_rate is failed, sio_bclk=0x%x,bclk_div is %d.\n",__LINE__, sync_div, bclk_div,4,5,6);
            return;
        }
        if(0 != clk_set_rate(sio_sync, sync_div))
        {
            logMsg("line %d: clk_set_rate is failed, sio_bclk=0x%x,bclk_div is %d.\n",__LINE__, sync_div, bclk_div,4,5,6);
            return;
        }
    }
    else
    {
        /* 设置PCM时钟源 19.2M */
        hi_syscrg_pcm_clk_sel();

        /* 设置SIO 时钟与PAD时钟同向 */
        hi_syscrg_sio_clk_positive();

        /* 从模式 */
        hi_syssc_pcm_mode_slave();
    }
}

/*****************************************************************************
 函 数 名  : bsp_sio_set_sample
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_set_sample(drv_sio_sample_e sample)
{
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();

    if(sample < DRV_SIO_SAMPLING_BUTT){
        pstCfg->sample_rate = sample;
    }
}

/*****************************************************************************
 函 数 名  : bsp_sio_set_chn
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_set_chn(drv_sio_chn_num_e chn_num)
{
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();

    if(chn_num < DRV_SIO_CHN_BUTT){
        pstCfg->chn_num = chn_num;
    }
}

/*****************************************************************************
 函 数 名  : bsp_sio_isr_init
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_isr_init()
{
    HI_SIO_INTCLR_T                *pstIntClr;
    HI_SIO_CT_CLR_T                *pstSioCtlClr;

    pstIntClr   = (HI_SIO_INTCLR_T*)HI_SIO_REG_ADDR(HI_SIO_INTCLR_OFFSET);
    pstSioCtlClr  = (HI_SIO_CT_CLR_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_CT_CLR_OFFSET);

    /* 禁止全局中断 */
    pstSioCtlClr->bits.intr_en = 1;

    /* 禁止所有通道传输 */
    pstSioCtlClr->bits.tx_fifo_disable = 1;
    pstSioCtlClr->bits.rx_fifo_disable = 1;
    pstSioCtlClr->bits.tx_enable = 1;
    pstSioCtlClr->bits.rx_enable = 1;

    /* 清除所有SIO中断 */
    pstIntClr->u32 = 0x3f;

    /*注册SIO ISR*/
    intConnect ((VOIDFUNCPTR *)INT_LVL_LTESIO_SIO, bsp_sio_isr, 0);

    /*使能SIO组合 中断*/
    intEnable(INT_LVL_LTESIO_SIO);

    /*禁止sio rx中断*/

    /* 使能全局中断 */
    pstSioCtlClr->bits.intr_en = 1;
}

/*****************************************************************************
 函 数 名  : bsp_sio_clk_get
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_clk_get()
{
	struct clk          *clk_sio;
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();

    clk_sio = clk_get(NULL, "sio_clk");

    pstCfg->sio_clk = clk_sio;

    logMsg("clk_sio 0x%x  pstCfg 0x%x\n", (u32)clk_sio, (u32)pstCfg, 3,4,5,6);

    return;
}

/*****************************************************************************
 函 数 名  : bsp_sio_clk_enable
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_clk_enable()
{
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();

    if(!pstCfg->sio_clk)
    {
        logMsg("sio clk is null\n",1,2,3,4,5,6);
        return;
    }

    clk_enable(pstCfg->sio_clk);
}

/*****************************************************************************
 函 数 名  : bsp_sio_clk_disable
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_clk_disable()
{
    drv_sio_ctrl_stru  *pstCfg = DRV_SIO_GET_CFG_PTR();

    if(!pstCfg->sio_clk)
    {
        logMsg("sio clk is null\n",1,2,3,4,5,6);
        return;
    }

    clk_disable(pstCfg->sio_clk);
}

/*****************************************************************************
 函 数 名  : bsp_sio_slave_cfg
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_pcm_cfg()
{
    HI_SIO_MODE_T   *pstSioMode;
    pstSioMode      = (HI_SIO_MODE_T*)HI_SIO_REG_ADDR(HI_SIO_MODE_OFFSET);

    /* 选择时序类型.配置成标准模式 */
    pstSioMode->bits.pcm_mode = 0;

    /* 多路录音则要设置SIO_MODE[ext_rec_en] SIO_MODE[chn_num]和SIO_MODE[clk_edge] */
    if(pstSioMode->bits.chn_num > DRV_SIO_CHN_2)
    {
        pstSioMode->bits.clk_edge = 1; /* 采样时钟设置成上升沿有效 */
        pstSioMode->bits.ext_rec_en = 1; /* 扩展模式 */
    }
}

/*****************************************************************************
 函 数 名  : bsp_sio_master_cfg
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_i2s_cfg()
{
    HI_SIO_I2S_POS_MERGE_EN_T      *pstMerge;
    HI_SIO_I2S_START_POS_T         *pstStartPos;
    HI_SIO_CT_SET_T                *pstCtSet;
    HI_SIO_SIGNED_EXT_T            *pstSignedExt;
    HI_SIO_MODE_T                  *pstSioMode;

    pstSioMode  = (HI_SIO_MODE_T*)HI_SIO_REG_ADDR(HI_SIO_MODE_OFFSET);
    pstMerge    = (HI_SIO_I2S_POS_MERGE_EN_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_POS_MERGE_EN_OFFSET);
    pstStartPos = (HI_SIO_I2S_START_POS_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_START_POS_OFFSET);
    pstSignedExt= (HI_SIO_SIGNED_EXT_T*)HI_SIO_REG_ADDR(HI_SIO_SIGNED_EXT_OFFSET);
    pstCtSet   =  (HI_SIO_CT_SET_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_CT_SET_OFFSET);

    /* 根据实际读写FIFO数据的方式, 配置SIO_I2S_POS_MERGE_EN和SIO_I2S_START_POS*/
    pstMerge->bits.merge_en = 1;             /* 左右声道合并使能 */
    pstStartPos->bits.start_pos_read  = 0;   /* 左声道开始访问 */
    pstStartPos->bits.start_pos_write = 0;   /* 左声道开始访问 */

    /* 设置SIO_CT_SET[tx_data_merge_en]和SIO_CT_SET[rx_data_merge_en]. 多路录音则不用进行该步骤设置*/
    pstCtSet->bits.tx_data_merge_en = 1;
    pstCtSet->bits.rx_data_merge_en = 1;
}

/*****************************************************************************
 函 数 名  : bsp_sio_init
 功能描述  : SIO驱动软件初始化, 应用软件配置前初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_sio_init(void)
{
    drv_sio_int_isr_stru                *pstIntIsr = DRV_SIO_GET_INT_ISR_PTR();
    drv_sio_status_stru                 *pstStatus = DRV_SIO_GET_STATUS_PTR();
    drv_sio_ctrl_stru                   *pstCfg = DRV_SIO_GET_CFG_PTR();

    memset(pstIntIsr,0,sizeof(drv_sio_int_isr_stru));
    memset(pstStatus,0,sizeof(drv_sio_status_stru));

#if 0
    /* 初始化接收和发送dma */
    if(OK != (ret = bsp_sio_dma_init()));
    {
        logMsg("drv_sio_dma_init failed \n",1,2,3,4,5,6);
        return ERROR;
    }
#endif

    bsp_sio_clk_get();

    if(!pstCfg->sio_clk)
    {
        logMsg("bsp_sio_clk_get failed \n",1,2,3,4,5,6);
        return ERROR ;
    }

    bsp_sio_clk_enable();

    bsp_sio_reset();

    /* 初始化默认设置 SLAVE模式、PCM接口、采样率为8K */
    bsp_sio_sync_mode(DRV_SIO_SYNC_MODE_SLAVE);

    bsp_sio_set_mode(DRV_SIO_MODE_PCM);

    bsp_sio_set_sample(DRV_SIO_SAMPLING_8K);

    bsp_sio_set_chn(DRV_SIO_CHN_2);

     /* 中断配置 */
    bsp_sio_isr_init();

    bsp_sio_unreset();

    /* 标记为关闭 */
    pstStatus->enOpen = DRV_SIO_STATE_CLOSE;

    bsp_sio_clk_disable();
    return OK;
}

/*****************************************************************************
 函 数 名  : bsp_sio_open
 功能描述  : 打开SIO，并注册回调函数
 输入参数  : enIntMask       - 中断使能字
             pfIntHandleFunc - 回调函数指针
             uwPara          - 回调参数
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_sio_open(
                drv_sio_int_mask    enIntMask,
                drv_sio_int_func    pfIntHandleFunc,
                unsigned int        uwPara)
{
    drv_sio_int_isr_stru                *pstIntIsr      = DRV_SIO_GET_INT_ISR_PTR();
    drv_sio_status_stru                 *pstStatus      = DRV_SIO_GET_STATUS_PTR();
    drv_sio_ctrl_stru                   *pstCfg          = DRV_SIO_GET_CFG_PTR();
    HI_SIO_MODE_T                       *pstSioMode;
    HI_SIO_DATA_WIDTH_SET_T             *pstSioWidth;
    HI_SIO_CT_CLR_T                     *pstSioCtlClr;
    HI_SIO_INTMASK_T                    *pstSioIntMask;
    HI_SIO_CT_SET_T                     *pstSioCtlSet;
    HI_SIO_SIGNED_EXT_T                 *pstSioExt;

    pstSioMode = (HI_SIO_MODE_T*)HI_SIO_REG_ADDR(HI_SIO_MODE_OFFSET);
    pstSioIntMask = (HI_SIO_INTMASK_T*)HI_SIO_REG_ADDR(HI_SIO_INTMASK_OFFSET);
    pstSioCtlClr = (HI_SIO_CT_CLR_T*)HI_SIO_REG_ADDR(HI_SIO_INTCLR_OFFSET);
    pstSioWidth = (HI_SIO_DATA_WIDTH_SET_T*)HI_SIO_REG_ADDR(HI_SIO_DATA_WIDTH_SET_OFFSET);
    pstSioCtlSet = (HI_SIO_CT_SET_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_CT_SET_OFFSET);
    pstSioExt = (HI_SIO_SIGNED_EXT_T*)HI_SIO_REG_ADDR(HI_SIO_SIGNED_EXT_OFFSET);

    if (DRV_SIO_STATE_OPEN == pstStatus->enOpen)
    {
        logMsg("sio already opened \n",1,2,3,4,5,6);
        return ERROR;
    }

    bsp_sio_clk_enable();

    /* 标记为打开 */
    pstStatus->enOpen = DRV_SIO_STATE_OPEN;

    /*写DRV_SIO_CTRL_CLRs 0xfff0复位SIO、禁止传输、禁止中断*/
    pstSioCtlClr->u32 = 0xffff;

    /* 屏蔽所有SIO中断 */
    pstSioIntMask->u32 = 0x3f;

    /* 解复位 */
    pstSioCtlSet->bits.rst_n = 1;

   /*配置为PCM/I2S接口模式*/
    pstSioMode->bits.sio_mode = pstCfg->enMode;

    /* 设置接收FIFO和发送FIFO的水线 */
    pstSioCtlSet->bits.tx_fifo_threshold = 8;
    pstSioCtlSet->bits.rx_fifo_threshold = 8;

    /* 设置通道数 */
    pstSioMode->bits.chn_num = pstCfg->chn_num;

    /*配置为发送、接收16bit位宽*/
    pstSioWidth->bits.tx_mode = 1;
    pstSioWidth->bits.rx_mode = 1;

    /* 多通道情况下设置成扩展模式 */
    if(pstSioMode->bits.chn_num > DRV_SIO_CHN_2)
    {
        pstSioMode->bits.ext_rec_en = 1; /* 扩展模式 */
    }

    if (DRV_SIO_MODE_I2S == pstCfg->enMode)
    {
        bsp_sio_i2s_cfg();
    }
    else
    {
        bsp_sio_pcm_cfg();
    }

    /*配置符号扩展禁止*/
    pstSioExt->u32 = 0;

    if (NULL != pfIntHandleFunc)
    {
        /* 设置中断屏蔽，使能中断*/
        pstSioIntMask->u32 = ~enIntMask;

        pstIntIsr->pfFunc = pfIntHandleFunc;
        pstIntIsr->uwPara = uwPara;
        pstSioCtlSet->bits.intr_en = 1;
    }

    /*使能SIO接收、发送*/
    pstSioCtlSet->bits.rx_enable = 1;
    pstSioCtlSet->bits.tx_enable = 1;

    /* bsp_sio_clk_disable(); */

    return OK;

}

/*****************************************************************************
 函 数 名  : bsp_sio_play_start
 功能描述  : 关闭SIO并清除回调函数信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_play_start()
{
    HI_SIO_CT_SET_T                     *pstSioCtlSet;
    HI_SIO_I2S_LEFT_XD_T                *pstSioLxd;
    HI_SIO_I2S_RIGHT_XD_T               *pstSioRxd;

    pstSioCtlSet = (HI_SIO_CT_SET_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_CT_SET_OFFSET);
    pstSioRxd = (HI_SIO_I2S_RIGHT_XD_T*)HI_SIO_REG_ADDR(HI_SIO_PCM_XD_OFFSET);     /* PCM 方式与右声道复用 */
    pstSioLxd = (HI_SIO_I2S_LEFT_XD_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_LEFT_XD_OFFSET);

    /* EDMA配置 */

    /* 写入初始数据0 */
    pstSioRxd->u32 = 0;
    pstSioLxd->u32 = 0;

    /* 启动发送 */
    pstSioCtlSet->bits.tx_enable = 1;
}

/*****************************************************************************
 函 数 名  : bsp_sio_record_start
 功能描述  : 关闭SIO并清除回调函数信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_record_start()
{
    HI_SIO_CT_SET_T                     *pstSioCtlSet;

    pstSioCtlSet = (HI_SIO_CT_SET_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_CT_SET_OFFSET);

    /* EDMA配置 */

    /* 启动发送 */
    pstSioCtlSet->bits.rx_enable = 1;
}

/*****************************************************************************
 函 数 名  : bsp_sio_play_stop
 功能描述  : 关闭SIO并清除回调函数信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_play_stop()
{
    HI_SIO_CT_CLR_T          *pstSioCtlClr;
    drv_sio_ctrl_stru        *pstCfg = DRV_SIO_GET_CFG_PTR();

    pstSioCtlClr = (HI_SIO_CT_CLR_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_CT_CLR_OFFSET);

    /* 停止发送 */
    pstSioCtlClr->bits.tx_enable = 1;

    bsp_sio_dma_stop(pstCfg->dma_cfg.tx_channel_id);
}

/*****************************************************************************
 函 数 名  : bsp_sio_record_stop
 功能描述  : 关闭SIO并清除回调函数信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_record_stop()
{
    HI_SIO_CT_CLR_T             *pstSioCtlClr;
    drv_sio_ctrl_stru           *pstCfg = DRV_SIO_GET_CFG_PTR();

    pstSioCtlClr = (HI_SIO_CT_CLR_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_CT_CLR_OFFSET);

    /* 停止录音 */
    pstSioCtlClr->bits.rx_enable = 1;

    bsp_sio_dma_stop(pstCfg->dma_cfg.rx_channel_id);
}

/*****************************************************************************
 函 数 名  : bsp_sio_close
 功能描述  : 关闭SIO并清除回调函数信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_close(void)
{
    drv_sio_int_isr_stru                *pstIntIsr = DRV_SIO_GET_INT_ISR_PTR();
    drv_sio_status_stru                 *pstStatus = DRV_SIO_GET_STATUS_PTR();

    HI_SIO_CT_CLR_T                     *pstSioCtlClr;
    HI_SIO_INTMASK_T                    *pstSioIntMask;
    HI_SIO_CT_SET_T                     *pstSioCtlSet;
    HI_SIO_INTCLR_T                 *pstSioIntClr;

    pstSioIntMask = (HI_SIO_INTMASK_T*)HI_SIO_REG_ADDR(HI_SIO_INTMASK_OFFSET);
    pstSioCtlClr = (HI_SIO_CT_CLR_T*)HI_SIO_REG_ADDR(HI_SIO_INTCLR_OFFSET);
    pstSioCtlSet = (HI_SIO_CT_SET_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_CT_SET_OFFSET);
    pstSioIntClr = (HI_SIO_INTCLR_T*)HI_SIO_REG_ADDR(HI_SIO_INTCLR_OFFSET);

    if(DRV_SIO_STATE_CLOSE == pstStatus->enOpen)
    {
        logMsg("drv sio already opened \n", 1,2,3,4,5,6);
        return;
    }

    /*复位SIO、禁止传输、禁止中断*/
    pstSioCtlClr->u32 = 0xffff;

    /*屏蔽所有SIO中断*/
    pstSioIntMask->u32 = 0xffffffff;

    /*清除SIO中断*/
    pstSioIntClr->u32 = 0x3f;

    /*解复位SIO*/
    pstSioCtlSet->bits.rst_n = 1;

    /*清除中断回调函数相关信息*/
    memset(pstIntIsr, 0, sizeof(drv_sio_int_isr_stru));

    bsp_sio_clk_disable();

    /* 标记为关闭 */
    pstStatus->enOpen = DRV_SIO_STATE_CLOSE;

}

/*****************************************************************************
 函 数 名  : bsp_sio_isr
 功能描述  : SIO中断处理回调函数，使用VOS提供的中断注册接口注册本函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_isr(void)
{
    unsigned int                   uwIntState;
    HI_SIO_INTSTATUS_T             *pstIntStat;
    HI_SIO_INTCLR_T                *pstIntClr;
    drv_sio_int_isr_stru           *pstIntIsr = DRV_SIO_GET_INT_ISR_PTR();

    pstIntStat   = (HI_SIO_INTSTATUS_T*)HI_SIO_REG_ADDR(HI_SIO_INTSTATUS_OFFSET);
    pstIntClr   = (HI_SIO_INTCLR_T*)HI_SIO_REG_ADDR(HI_SIO_INTCLR_OFFSET);

    /*读取SIO中断状态*/
    uwIntState  = pstIntStat->u32;

    /*清除查询到的中断*/
    pstIntClr->u32 = uwIntState;

    /*若中断状态为0，直接返回*/
    if (0 == uwIntState)
    {
        return;
    }

    /*若SIO中断回调函数非空,则调用*/
    if (NULL != pstIntIsr->pfFunc)
    {
        pstIntIsr->pfFunc((drv_sio_int_mask)uwIntState, pstIntIsr->uwPara);
    }
}

/*****************************************************************************
 函 数 名  : bsp_sio_reset
 功能描述  : SIO中断处理回调函数，使用VOS提供的中断注册接口注册本函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_reset()
{
    HI_SIO_CT_CLR_T                *pstSioCtlClr;

    pstSioCtlClr   = (HI_SIO_CT_CLR_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_CT_CLR_OFFSET);

    /*复位SIO */
    pstSioCtlClr->bits.rst_n = 1;
}

/*****************************************************************************
 函 数 名  : bsp_sio_unreset
 功能描述  : SIO中断处理回调函数，使用VOS提供的中断注册接口注册本函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_sio_unreset()
{
    HI_SIO_CT_SET_T                *pstSioCtrlSet;

    pstSioCtrlSet   = (HI_SIO_CT_SET_T*)HI_SIO_REG_ADDR(HI_SIO_I2S_CT_SET_OFFSET);

    /*解复位SIO*/
    pstSioCtrlSet->bits.rst_n = 1;
}
#ifdef __cplusplus
}
#endif
