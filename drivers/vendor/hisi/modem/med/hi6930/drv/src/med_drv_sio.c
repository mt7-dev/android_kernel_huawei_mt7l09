

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_drv_sio.h"
#include "soc_sctrl_interface.h"
#include "ucom_comm.h"
#include "ucom_stub.h"
#include "med_drv_interface.h"
#ifdef _lint
#include "MemoryMap.h"
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* V7R2只有一个SIO，供VOICE使用 */
UCOM_SEC_TCMBSS
DRV_SIO_CFG_STRU    g_stDrvSioVoiceCfg;                        /* SIO voice模块控制全局变量 */

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID DRV_SIO_Init(VOS_VOID)
{
    DRV_SIO_CFG_STRU   *pstVoiceCfg;
    VOS_UINT32          uwVoiceChanNum = DRV_SIO_VOICE_MIC_CHANNEL_NUM;

    /* 获取SIO控制结构体 */
    pstVoiceCfg       = DRV_SIO_GetVoiceCfgPtr();

    /* 初始化控制结构体为全零 */
    UCOM_MemSet(pstVoiceCfg, 0, sizeof(DRV_SIO_CFG_STRU));

    /* 地址初始化 */
    pstVoiceCfg->uwSioBaseAddr  = DRV_SIO_BASE_ADDR_VOICE;

    /************************** 配置AUDIO SIO *****************************/
    /* 无相关SIO，暂不实现 */

    /************************** 配置VOICE SIO *****************************/
    /* 打开SIO时钟 */
    DRV_SIO_Voice_OpenClk();

    /* 复位SIO */
    DRV_SIO_Reset(pstVoiceCfg);

    /* 关闭SIO时钟 */
    DRV_SIO_Voice_CloseClk();

    pstVoiceCfg->enWorkMode = DRV_SIO_Voice_GetWorkMode(uwVoiceChanNum);

    /************************** 配置SMART PA SIO *****************************/
    /* 无相关SIO，暂不实现 */

    /* 因为用DMA中断，所以不用SIO中断，以下代码不打开 */
    #ifdef _MED_ENABLE_SIO_INT_
    /* 注册SIO ISR */
    BSP_INT_Connect((VOIDFUNCPTR *)(DRV_SIO_IRQ), (VOIDFUNCPTR)DRV_SIO_Isr, 0);

    /* 使能中断 */
    BSP_INT_Enable(DRV_SIO_IRQ);
    #endif

}


VOS_UINT32 DRV_SIO_Open(
                DRV_SIO_CFG_STRU               *pstCfg,
                DRV_SIO_INT_MASK_ENUM_UINT32    enIntMask,
                DRV_SIO_INT_FUNC                pfIntHandleFunc,
                VOS_UINT32                      uwPara)
{
    SOC_SIO_CT_SET_UNION                *pstCtSet;
    SOC_SIO_CT_CLR_UNION                *pstCtClr;
    SOC_SIO_MODE_UNION                  *pstMode;
    SOC_SIO_I2S_POS_MERGE_EN_UNION      *pstMerge;
    SOC_SIO_I2S_START_POS_UNION         *pstStartPos;
    SOC_SIO_DATA_WIDTH_SET_UNION        *pstDataWidth;
    SOC_SIO_SIGNED_EXT_UNION            *pstSignedExt;
    SOC_SIO_INTMASK_UNION               *pstIntMask;
    SOC_SIO_INTCLR_UNION                *pstIntclr;

    VOS_UINT32                           uwSioBaseAddr;

    uwSioBaseAddr = pstCfg->uwSioBaseAddr;

    pstIntclr   = (SOC_SIO_INTCLR_UNION*)SOC_SIO_INTCLR_ADDR(pstCfg->uwSioBaseAddr);
    pstCtSet    = (SOC_SIO_CT_SET_UNION*)SOC_SIO_CT_SET_ADDR(uwSioBaseAddr);
    pstCtClr    = (SOC_SIO_CT_CLR_UNION*)SOC_SIO_CT_CLR_ADDR(uwSioBaseAddr);
    pstMode     = (SOC_SIO_MODE_UNION*)SOC_SIO_MODE_ADDR(uwSioBaseAddr);
    pstMerge    = (SOC_SIO_I2S_POS_MERGE_EN_UNION*)SOC_SIO_I2S_POS_MERGE_EN_ADDR(uwSioBaseAddr);
    pstStartPos = (SOC_SIO_I2S_START_POS_UNION*)SOC_SIO_I2S_START_POS_ADDR(uwSioBaseAddr);
    pstDataWidth= (SOC_SIO_DATA_WIDTH_SET_UNION*)SOC_SIO_DATA_WIDTH_SET_ADDR(uwSioBaseAddr);
    pstSignedExt= (SOC_SIO_SIGNED_EXT_UNION*)SOC_SIO_SIGNED_EXT_ADDR(uwSioBaseAddr);
    pstIntMask  = (SOC_SIO_INTMASK_UNION*)SOC_SIO_INTMASK_ADDR(uwSioBaseAddr);

    /************************** 初始化 *****************************/
    /* 设置SIO_CT_SET/SIO_CT_CLR[rx_enable]和SIO_CT_SET/SIO_CT_CLR[tx_enable]为0 使SIO处于禁止状态 */
    pstCtClr->reg.rx_enable = 1; /* SIO_CT_SET[rx_enable]将被清成0 */
    pstCtClr->reg.tx_enable = 1; /* SIO_CT_SET[tx_enable]将被清成0 */

    /* 禁止发送以及接收FIFO, 清空FIFO中的数据 */
    pstCtSet->reg.tx_fifo_disable   = 1;
    pstCtSet->reg.rx_fifo_disable   = 1;

    /* 配置SIO_DATA_WIDTH_SET寄存器、配置SIO_SIGNED_EXT寄存器，以设定正确的位宽 */
    pstDataWidth->reg.tx_mode = 1;          /* 16bit */
    pstDataWidth->reg.rx_mode = 1;          /* 16bit */
    pstSignedExt->reg.signed_ext_en = 0;    /* 禁止符号拓展 */

    /* 设置SIO_MODE[sio_mode],选择I2S或PCM模式;*/
    pstMode->reg.sio_mode   = pstCfg->enWorkMode;

    pstMode->reg.ext_rec_en = 0; /* 标准的接收模式 */

    if(DRV_SIO_MODE_PCM == pstCfg->enWorkMode)
    {
        /* 如果是PCM模式, 设置SIO_MODE[pcm_mode], 选择时序类型.
           多路录音则要设置SIO_MODE[ext_rec_en] SIO_MODE[chn_num]和SIO_MODE[clk_edge]*/
        pstMode->reg.pcm_mode = 0;
    }
    else
    {
        /* 如果是I2S模式, 根据实际读写FIFO数据的方式, 配置SIO_I2S_POS_MERGE_EN和SIO_I2S_START_POS*/
        pstMerge->reg.merge_en = 1;             /* 左右声道合并使能 */
        pstStartPos->reg.start_pos_read  = 0;   /* 左声道开始访问 */
        pstStartPos->reg.start_pos_write = 0;   /* 左声道开始访问 */

        /* 同时, 设置SIO_CT_SET[tx_data_merge_en]和SIO_CT_SET[rx_data_merge_en]. 多路录音则不用进行该步骤设置*/
        pstCtSet->reg.tx_data_merge_en = 1;
        pstCtSet->reg.rx_data_merge_en = 1;

    }

    /* 配置SIO_CT_SET[rx_fifo_threshold]和SIO_CT_SET[tx_fifo_threshold], 即:设置接收FIFO和发送FIFO的水线 */
    pstCtSet->reg.tx_fifo_threshold = 8;
    pstCtSet->reg.rx_fifo_threshold = 8;
    pstCtSet->reg.rst_n = 1;

    /* 配置SIO的DMA通道类型为EDMA_CH16 */
    UCOM_RegBitWr(DRV_SC_CTRL54_ADDR, DRV_SC_SIO_DMA_SEL_BIT, DRV_SC_SIO_DMA_SEL_BIT, 0x0);

    /* 根据中断屏蔽的需要,设置SIO的中断屏蔽寄存器SIO_INTMASK,设置SIO_CT_SET[intr_en] */
    pstCtClr->reg.intr_en    = 1;               /* 全局关中断 */
    pstIntclr->value         = 0x3f;            /* 清除所有SIO中断 */
    pstIntMask->value        = 0x3f;            /* 屏蔽全部中断 */

    /* 因为用DMA中断，所以不用SIO中断，以下代码不打开 */
    #ifdef _MED_ENABLE_SIO_INT_
    /* 若中断回调函数非空，则打开中断屏蔽,保存回调函数信息 */
    if (VOS_NULL != pfIntHandleFunc)
    {
        /* 对enIntMask取反，打开中断屏蔽 */
        pstIntMask->value        = ~enIntMask;

        /* 设置中断使能 */
        pstCtSet->reg.intr_en    = 1;

        pstCfg->stIsr.pfFunc    = pfIntHandleFunc;
        pstCfg->stIsr.uwPara    = uwPara;
    }
    #endif


    /************************** 启动播放 ****************************/

    /* 配置中断屏蔽寄存器SIO_INTMASK[tx_intr]为1, 屏蔽发送中断 */
    pstIntMask->reg.tx_intr  = 1;

    /* 设置SIO_CT_SET[tx_enable]为1, 启动播放 */
    pstCtSet->reg.tx_enable = 1;

    /* 清除FIFO禁止标志, 使能发送FIFO */
    pstCtClr->reg.tx_fifo_disable   = 1;

    /************************** 启动录音 ****************************/

    /* 设置SIO_CT_SET[rx_enable]为1, 启动数据接收 */
    pstCtSet->reg.rx_enable = 1;

    /* 清除FIFO禁止标志, 使能接收FIFO */
    pstCtClr->reg.rx_fifo_disable   = 1;

    return VOS_OK;

}


VOS_VOID DRV_SIO_Close(DRV_SIO_CFG_STRU *pstCfg)
{
    VOS_UINT32            uwSioBaseAddr = pstCfg->uwSioBaseAddr;
    SOC_SIO_CT_CLR_UNION *pstCtClr      = (SOC_SIO_CT_CLR_UNION*)SOC_SIO_CT_CLR_ADDR(uwSioBaseAddr);

    if(DRV_SIO_CLK_STATUS_CLOSE == pstCfg->enClkStatus)
    {
        return;
    }

    /* 设置SIO_CT_SET/SIO_CT_CLR[rx_enable]和SIO_CT_SET/SIO_CT_CLR[tx_enable]为0 使SIO处于禁止状态 */
    pstCtClr->reg.rx_enable = 1; /* SIO_CT_SET[rx_enable]将被清成0 */
    pstCtClr->reg.tx_enable = 1; /* SIO_CT_SET[tx_enable]将被清成0 */

    /* 禁止中断  */
    pstCtClr->reg.intr_en    = 1;

    /* 复位SIO */
    DRV_SIO_Reset(pstCfg);

    /* 清除中断回调函数相关信息 */
    UCOM_MemSet(&pstCfg->stIsr, 0, sizeof(DRV_SIO_INT_ISR_STRU));
}


VOS_VOID DRV_SIO_SetMaster(
                DRV_SIO_CFG_STRU             *pstCfg,
                DRV_SIO_SAMPLING_ENUM_UINT32  enSamplingRate)
{
    /* 记录同步模式 */
    pstCfg->enSyncMode                  = DRV_SIO_SYNC_MODE_MASTER;

    /* 选择为master模式 */
    UCOM_RegBitWr(DRV_SC_CTRL3_ADDR, DRV_SC_SIO_MASTER_MODE_BIT, DRV_SC_SIO_MASTER_MODE_BIT, 0x1);

    /* 配置分频系数 */
    switch(enSamplingRate)
    {
        case DRV_SIO_SAMPLING_8K:
        {
            /* 设置SIO master时时钟分频系数，供窄带使用，120000k÷60÷250= 8k */
            UCOM_RegBitWr(DRV_CRG_CLKDIV1_ADDR,
                          DRV_CRG_PCM_SYNC_DIV_BIT_LO,
                          DRV_CRG_PCM_SYNC_DIV_BIT_HI,
                          0x3c);

            UCOM_RegBitWr(DRV_CRG_CLKDIV1_ADDR,
                          DRV_CRG_PCM_BCLK_DIV_BIT_LO,
                          DRV_CRG_PCM_BCLK_DIV_BIT_HI,
                          0xfa);
        }
        break;

        case DRV_SIO_SAMPLING_16K:
        {
            /* 设置SIO master时时钟分频系数，供窄带使用，120000k÷60÷125= 16k */
            UCOM_RegBitWr(DRV_CRG_CLKDIV1_ADDR,
                          DRV_CRG_PCM_SYNC_DIV_BIT_LO,
                          DRV_CRG_PCM_SYNC_DIV_BIT_HI,
                          0x3c);

            UCOM_RegBitWr(DRV_CRG_CLKDIV1_ADDR,
                          DRV_CRG_PCM_BCLK_DIV_BIT_LO,
                          DRV_CRG_PCM_BCLK_DIV_BIT_HI,
                          0x7d);
        }
        break;

        case DRV_SIO_SAMPLING_48K:
        {
            /* 采样率为48k时的处理，设置SIO master时时钟分频系数，24576k÷16÷32=48k */
            /* 设置SIO master时时钟分频系数，供窄带使用，120000k÷10÷250= 48k */
            UCOM_RegBitWr(DRV_CRG_CLKDIV1_ADDR,
                          DRV_CRG_PCM_SYNC_DIV_BIT_LO,
                          DRV_CRG_PCM_SYNC_DIV_BIT_HI,
                          0x0a);

            UCOM_RegBitWr(DRV_CRG_CLKDIV1_ADDR,
                          DRV_CRG_PCM_BCLK_DIV_BIT_LO,
                          DRV_CRG_PCM_BCLK_DIV_BIT_HI,
                          0xfa);
        }
        break;

        default:
        {
            /* 调用者保证不走到此分支 */
            /* 默认采用采样率为8k时的处理 */
            UCOM_RegBitWr(DRV_CRG_CLKDIV1_ADDR,
                          DRV_CRG_PCM_SYNC_DIV_BIT_LO,
                          DRV_CRG_PCM_SYNC_DIV_BIT_HI,
                          0x3c);

            UCOM_RegBitWr(DRV_CRG_CLKDIV1_ADDR,
                          DRV_CRG_PCM_BCLK_DIV_BIT_LO,
                          DRV_CRG_PCM_BCLK_DIV_BIT_HI,
                          0xfa);
        }
        break;
    }

}
VOS_VOID DRV_SIO_SetSlave(DRV_SIO_CFG_STRU   *pstCfg)
{
    /* 记录同步模式 */
    pstCfg->enSyncMode                   = DRV_SIO_SYNC_MODE_SLAVE;

    /* 设置SIO时钟与PAD时钟同向 */
    UCOM_RegBitWr(DRV_SC_SIO_CLK_POSITIVE, DRV_SC_SIO_CLK_POST_SEL_BIT, DRV_SC_SIO_CLK_POST_SEL_BIT, 0x0);

    /* 设置为SLAVE模式 */
    UCOM_RegBitWr(DRV_SC_CTRL3_ADDR, DRV_SC_SIO_MASTER_MODE_BIT, DRV_SC_SIO_MASTER_MODE_BIT, 0x0);
}
VOS_VOID DRV_SIO_Reset(DRV_SIO_CFG_STRU   *pstCfg)
{
    SOC_SIO_CT_CLR_UNION     *pstCtClr;
    SOC_SIO_INTCLR_UNION     *pstIntclr;
    SOC_SIO_CT_SET_UNION     *pstCtSet;

    pstCtClr  = (SOC_SIO_CT_CLR_UNION*)SOC_SIO_CT_CLR_ADDR(pstCfg->uwSioBaseAddr);
    pstIntclr = (SOC_SIO_INTCLR_UNION*)SOC_SIO_INTCLR_ADDR(pstCfg->uwSioBaseAddr);
    pstCtSet  = (SOC_SIO_CT_SET_UNION*)SOC_SIO_CT_SET_ADDR(pstCfg->uwSioBaseAddr);

    /* 写DRV_SIO_CTRL_CLR 0xffff复位SIO、禁止传输、禁止中断 */
    pstCtClr->value  = 0xffff;

    /* 写DRV_SIO_INTR_CLR 0x3f 清除所有SIO中断 */
    pstIntclr->value = 0x3f;

    /* 通道解复位 */
    pstCtSet->value  = 0xc088;
}
VOS_UINT32 DRV_SIO_GetTxRegAddr(DRV_SIO_CFG_STRU   *pstCfg)
{
    if(DRV_SIO_MODE_PCM == pstCfg->enWorkMode)
    {
        return DRV_SIO_PCM_XD_ADDR(pstCfg->uwSioBaseAddr);
    }
    else
    {
        return DRV_SIO_I2S_DUAL_TX_CHN_ADDR(pstCfg->uwSioBaseAddr);
    }
}


VOS_UINT32 DRV_SIO_GetRxRegAddr( DRV_SIO_CFG_STRU   *pstCfg )
{
    if(DRV_SIO_MODE_PCM == pstCfg->enWorkMode)
    {
        return DRV_SIO_PCM_RD_ADDR(pstCfg->uwSioBaseAddr);
    }
    else
    {
        return DRV_SIO_I2S_DUAL_RX_CHN_ADDR(pstCfg->uwSioBaseAddr);
    }
}


VOS_UINT32 DRV_SIO_GetSioChannelNum( DRV_SIO_CFG_STRU   *pstCfg )
{
    if(DRV_SIO_MODE_PCM == pstCfg->enWorkMode)
    {
        return DRV_SIO_CHANNEL_NUM_PCM;
    }
    else
    {
        return DRV_SIO_CHANNEL_NUM_I2S;
    }
}


VOS_UINT32 DRV_SIO_Audio_Open(
                DRV_SIO_INT_MASK_ENUM_UINT32    enIntMask,
                DRV_SIO_SAMPLING_ENUM_UINT32    enSamplingType,
                VOS_UINT16                      uhwIsModeMaster,
                DRV_SIO_INT_FUNC                pfIntHandleFunc,
                VOS_UINT32                      uwPara)
{
    return 0;
}


VOS_UINT32 DRV_SIO_Voice_Open(
                DRV_SIO_INT_MASK_ENUM_UINT32    enIntMask,
                DRV_SIO_SAMPLING_ENUM_UINT32    enSamplingType,
                VOS_UINT16                      uhwIsModeMaster,
                DRV_SIO_INT_FUNC                pfIntHandleFunc,
                VOS_UINT32                      uwPara)
{
    VOS_UINT32                             uwRet;
    DRV_SIO_CFG_STRU                      *pstCfg = DRV_SIO_GetVoiceCfgPtr();
    if(CODEC_SWITCH_ON == uhwIsModeMaster)
    {
        DRV_SIO_SetMaster(pstCfg, enSamplingType);
    }
    else
    {
        DRV_SIO_SetSlave(pstCfg);
    }

    DRV_SIO_Voice_OpenClk();

    DRV_SIO_Reset(pstCfg);
    uwRet = DRV_SIO_Open(pstCfg, enIntMask, pfIntHandleFunc, uwPara);

    return uwRet;
}


VOS_UINT32 DRV_SIO_SmartPa_Open(
                DRV_SIO_INT_MASK_ENUM_UINT32    enIntMask,
                DRV_SIO_SAMPLING_ENUM_UINT32    enSamplingType,
                VOS_UINT16                      uhwIsModeMaster,
                DRV_SIO_INT_FUNC                pfIntHandleFunc,
                VOS_UINT32                      uwPara)
{
    return 0;
}


VOS_VOID DRV_SIO_Audio_Close(VOS_VOID)
{
    return;
}


VOS_VOID DRV_SIO_Voice_Close(VOS_VOID)
{
    DRV_SIO_CFG_STRU  *pstCfg = DRV_SIO_GetVoiceCfgPtr();

    if(DRV_SIO_CLK_STATUS_OPEN == pstCfg->enClkStatus)
    {
        DRV_SIO_Close(pstCfg);

        /* 关闭SIO时钟  */
        DRV_SIO_Voice_CloseClk();
    }
}
VOS_VOID DRV_SIO_SmartPa_Close(VOS_VOID)
{
    return;
}


VOS_VOID DRV_SIO_Audio_OpenClk(VOS_VOID)
{
    return;
}


VOS_VOID DRV_SIO_Voice_OpenClk(VOS_VOID)
{
    DRV_SIO_CFG_STRU                 *pstVoiceCfg = DRV_SIO_GetVoiceCfgPtr();

    /* 1. 设置总线时钟开始使能 */
    UCOM_RegBitWr(DRV_CRG_CLKEN3_ADDR,
                  DRV_SC_LTE_SIO_CLK_EN_BIT,
                  DRV_SC_LTE_SIO_CLK_EN_BIT,
                  0x1);

    /* 2. V7R2不用设置工作时钟 */

    /* 3. 修改VOICE CFG标志位，标记时钟状态 */
    pstVoiceCfg->enClkStatus = DRV_SIO_CLK_STATUS_OPEN;
}
VOS_VOID DRV_SIO_SmartPa_OpenClk(VOS_VOID)
{
    return;
}


VOS_VOID DRV_SIO_Audio_CloseClk(VOS_VOID)
{
    return;
}


VOS_VOID DRV_SIO_Voice_CloseClk(VOS_VOID)
{
    DRV_SIO_CFG_STRU                  *pstVoiceCfg = DRV_SIO_GetVoiceCfgPtr();

    /* 1. 设置总线时钟关闭使能 */
    UCOM_RegBitWr(DRV_CRG_CLKDIS3_ADDR,
                  DRV_SC_LTE_SIO_CLK_DIS_BIT,
                  DRV_SC_LTE_SIO_CLK_DIS_BIT,
                  0x1);

    /* 2. V7R2不用设置工作时钟 */

    /* 3. 修改VOICE CFG标志位，标记时钟状态 */
    pstVoiceCfg->enClkStatus = DRV_SIO_CLK_STATUS_CLOSE;
}
VOS_VOID DRV_SIO_SmartPa_CloseClk(VOS_VOID)
{
    return;
}


VOS_UINT32 DRV_SIO_Audio_Loop(DRV_SIO_SAMPLING_ENUM_UINT32  enSamplingType)
{

    /* 该函数没有实现 */

    return UCOM_RET_SUCC;
}
VOS_UINT32 DRV_SIO_Voice_GetWorkMode(VOS_UINT32 uwVoiceChanNum)
{

    if (DRV_SIO_CHANNEL_NUM_PCM == uwVoiceChanNum)
    {
        /* 使用PCM工作方式 */
        return DRV_SIO_MODE_PCM;
    }
    else
    {
        /* 使用I2S工作方式 */
        return DRV_SIO_MODE_I2S;
    }
}


VOS_UINT32 DRV_SIO_GetVoiceMicChn(VOS_VOID)
{
    return DRV_SIO_VOICE_MIC_CHANNEL_NUM;
}


VOS_UINT32 DRV_SIO_GetVoiceSpkChn(VOS_VOID)
{
    return DRV_SIO_VOICE_SPK_CHANNEL_NUM;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


