/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : VOICE_pcm.c
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2008年5月28日
  最近修改   :
  功能描述   : DMA控制函数
  函数列表   :
              VOICE_MicDmaIsr
              VOICE_PcmAdjustRingBufSize
              VOICE_PcmClrLastSpkBuff
              VOICE_PcmGetWritableSpkBuffPtr
              VOICE_PcmInsertCn
              VOICE_PcmManageRingBuf
              VOICE_PcmMicInStartLoopDMA
              VOICE_PcmSpkOutStartLoopDMA
              VOICE_PcmStartDMAFromDataIn
              VOICE_PcmStartDMAToDataOut
              VOICE_PcmSwitchMicBuff
              VOICE_PcmSwitchSpkBuff
              VOICE_PcmUpdate
              VOICE_PcmInit
              VOICE_PcmUpdateRxRingBuf
              VOICE_PcmUpdateTxRingBuf
              VOICE_SioIsr
              VOICE_SpkDmaIsr
  修改历史   :
  1.日    期   : 2008年5月28日
    作    者   :  谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
   1 头文件包含
******************************************************************************/
#include "om_log.h"
#include "ucom_comm.h"
#include "med_drv_sio.h"
#include "med_drv_dma.h"
#include "med_drv_ipc.h"
#include "med_drv_mb_hifi.h"

#include "codec_typedefine.h"
#include "voice_pcm.h"
#include "voice_mc.h"
#include "voice_proc.h"
#include "voice_log.h"
#include "ucom_share.h"
#include "ucom_nv.h"
#include "ucom_pcm.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif


/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_PCM_C

/*****************************************************************************
   2 全局变量定义
******************************************************************************/
VOS_INT16                              *g_psVoicePcmMicIn;                      /*指向当前Mic输入的PCM缓存Buffer的指针*/
VOS_INT16                              *g_psVoicePcmSpkOut;                     /*指向当前输出到Speaker的PCM缓存Buffer的指针*/
VOS_INT16                              *g_psVoiceSpaDes;                        /*指向当前用于EC REF的Buffer指针*/
VOS_INT16                               g_shwCodecPcmFrameLength = CODEC_FRAME_LENGTH_NB; /* PCM数据帧长度 */

/* DMA通道配置参数结构体全局变量 */
UCOM_ALIGN(32)
DRV_DMA_CXCFG_STRU                      g_astVoicePcmMicInDmaPara[2] = {0};

UCOM_ALIGN(32)
DRV_DMA_CXCFG_STRU                      g_astVoicePcmSpkOutDmaPara[2] = {0};

UCOM_ALIGN(32)
DRV_DMA_CXCFG_STRU                      g_astVoiceEcRefInDmaPara[2] = {0};

/* PCM码流结构体全局变量，该全局变量需要定义在Hifi的TCM中 */
UCOM_SEC_TCMBSS
VOICE_PCM_BUFFER_STRU                   g_stVoicePcmBuff = {0};

VOICE_PCM_PC_VOICE_OBJ_STRU             g_stVoicePcVoiceObj= {0};               /*PC Voice实体全局变量 */

VOS_UINT32                              g_uwVcLoopCnt = 0;                      /* VC LOOP功能的循环计数(上行+下行) */
/*****************************************************************************
   3 外部函数声明
******************************************************************************/


/*****************************************************************************
   4 函数实现
******************************************************************************/

/*****************************************************************************
 函 数 名  : VOICE_PcmInit
 功能描述  : 初始化PCM内存数据
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月30日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数头注释模板

*****************************************************************************/
VOS_VOID VOICE_PcmInit(VOS_VOID)
{
    VOS_VOID                    *pshwPcmMicInTmp  = VOS_NULL;
    VOS_VOID                    *pshwPcmSpkOutTmp = VOS_NULL;
    VOICE_PCM_BUFFER_STRU       *pstPcmBuffTmp  = VOS_NULL;

    /* 记录RingBuff基地址,暂时与v3r3实现不一致 */
    /* CARM_HIFI_DYN_ADDR_SHARE_STRU      *pstShare = UCOM_GetCarmShareAddrStru(); */

    /* 获取Uncache地址 */
    pshwPcmMicInTmp     = (VOS_VOID *)UCOM_GetUncachedAddr((VOS_UINT32)(&g_astVoicePcmMicInDmaPara[0]));
    pshwPcmSpkOutTmp    = (VOS_VOID *)UCOM_GetUncachedAddr((VOS_UINT32)(&g_astVoicePcmSpkOutDmaPara[0]));
    pstPcmBuffTmp       = (&g_stVoicePcmBuff);

    /* 初始化全局变量全0，需保证Uncache */
    UCOM_MemSet(pshwPcmMicInTmp,  0, sizeof(g_astVoicePcmMicInDmaPara));
    UCOM_MemSet(pshwPcmSpkOutTmp, 0, sizeof(g_astVoicePcmSpkOutDmaPara));
    UCOM_MemSet(pstPcmBuffTmp,  0, sizeof(g_stVoicePcmBuff));

    g_psVoicePcmMicIn      = pstPcmBuffTmp->asMicInBuffA;
    g_psVoicePcmSpkOut     = pstPcmBuffTmp->asSpkOutBuffA;

    /* Smart PA初始化 */
    UCOM_PCM_SmartPaPcmInit();

    /*初始化PC Voice实体全局变量*/
    UCOM_MemSet(&g_stVoicePcVoiceObj, 0, sizeof(VOICE_PCM_PC_VOICE_OBJ_STRU));

    /* 记录RingBuff基地址,暂时与v3r3实现不一致 */
    /* VOICE_PcmSaveNvMemAddr(pstShare->uwCarmHifiMBAddr); */


    g_stVoicePcVoiceObj.stTxRingBufferState.uhwAdpBufferSize
        = VOICE_PCM_TX_BUF_SIZE_INIT;

    g_stVoicePcVoiceObj.stRxRingBufferState.uhwAdpBufferSize
        = VOICE_PCM_RX_BUF_SIZE_INIT;

}

/*****************************************************************************
 函 数 名  : VOICE_PcmUpdate
 功能描述  : 根据采样率重新设置输入输出
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月30日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数头注释模板

*****************************************************************************/
VOS_UINT32 VOICE_PcmUpdate(
                VOS_UINT32                      swSampleRate,
                VCVOICE_DEV_MODE_ENUM_UINT16    enDevMode)
{
    DRV_SIO_CFG_STRU               *pstSioStatus  = DRV_SIO_GetVoiceCfgPtr();
    DRV_SIO_SAMPLING_ENUM_UINT32    enSioSampleRate;
    DRV_SIO_INT_MASK_ENUM_UINT32    enSioMask;
    VOS_UINT16                      uhwModeMaster;
    VOS_UINT32                      uwVoiceChanNum= DRV_SIO_GetVoiceMicChn();

    /* 设置采样率模式和一帧PCM采样点数 */
    switch(swSampleRate)
    {
        case CODEC_SAMPLE_RATE_MODE_8000:
        {
            CODEC_PcmSetPcmFrameLength(CODEC_FRAME_LENGTH_NB);
            enSioSampleRate = DRV_SIO_SAMPLING_8K;
        }
        break;
        case CODEC_SAMPLE_RATE_MODE_16000:
        {
            CODEC_PcmSetPcmFrameLength(CODEC_FRAME_LENGTH_WB);
            enSioSampleRate = DRV_SIO_SAMPLING_16K;
        }
        break;
        default:
        {
            return UCOM_RET_ERR_PARA;
        }
    }

    /* SIO模式 */
    if (VCVOICE_DEV_MODE_PCVOICE != enDevMode)
    {
        /* 假如SIO处于打开状态,首先关闭 */
        if(DRV_SIO_CLK_STATUS_OPEN == pstSioStatus->enClkStatus)
        {
            DRV_SIO_Voice_Close();
        }

        /* 读取SIO主从模式设置 */
        UCOM_NV_Read(en_NV_Item_SIO_Voice_Master,
                    &uhwModeMaster,
                    sizeof(uhwModeMaster));

        /* 读取Voice SIO PCM/I2S模式设置 */
        /*
        UCOM_NV_Read(en_NV_Item_SIO_Voice_Pcm_I2S,
                    &uwWorkMode,
                    sizeof(uwWorkMode));
        */
        if (CODEC_SWITCH_ON == uhwModeMaster)
        {
            pstSioStatus->enWorkMode = DRV_SIO_MODE_PCM;
        }
        else
        {
            pstSioStatus->enWorkMode = DRV_SIO_Voice_GetWorkMode(uwVoiceChanNum);
        }

        /* 设置Mask, 分布操作防止PC-Lint告警 */
        enSioMask  = DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER;
        enSioMask |= DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER;

        /* 调用DRV_SIO_Open接口，打开SIO设备 */
        DRV_SIO_Voice_Open(enSioMask,
                           enSioSampleRate,
                           uhwModeMaster,
                           VOICE_SioIsr,
                           0);
    }

    return UCOM_RET_SUCC;
}


VOS_VOID VOICE_PcmSwitchMicBuff(VOS_VOID)
{
    VOS_UINT32              ulDestAddr, ulDestAddrB;   /* MIC采集的DMA通道的目的地址 */
    VOS_UINT32              uwChannelNum;
    VOICE_PCM_BUFFER_STRU  *pstPcmBuffTmp = (&g_stVoicePcmBuff);

    /*获取I2S/PCM模式下数据通道个数*/
    uwChannelNum    = DRV_SIO_GetSioChannelNum(DRV_SIO_GetVoiceCfgPtr());

    ulDestAddrB     = (VOS_UINT32)&pstPcmBuffTmp->asMicInBuffB[0];

    /*读取通道0目的地址寄存器*/
    ulDestAddr      = UCOM_RegRd(DRV_DMA_GetCxDesAddr(VOICE_MC_DMAC_CHN_MIC));

    /*当目的地址已经为BufferB范围内(LOOP模式)或目的地址为BufferA的最后一个位宽(非LOOP模式)*/
    /*此逻辑依赖于BufferA与BufferB地址连续，且BufferA在前*/
    if ( (ulDestAddr >= (ulDestAddrB - sizeof(VOS_UINT32)))
        &&(ulDestAddr < (ulDestAddrB + (((VOS_UINT32)(VOS_INT32)CODEC_PCM_FRAME_LENGTH_BYTES * uwChannelNum)
                            - sizeof(VOS_UINT32)))))
    {
        g_psVoicePcmMicIn = pstPcmBuffTmp->asMicInBuffA;
    }
    else
    {
        g_psVoicePcmMicIn = pstPcmBuffTmp->asMicInBuffB;
    }
}
VOS_INT16* VOICE_PcmGetWritableSpkBuffPtr(VOS_VOID)
{
    VOS_UINT32              ulSrcAddr, ulSrcAddrB;   /* SPK播放的DMA通道的源地址 */
    VOS_UINT32              uwChannelNum;
    VOICE_PCM_BUFFER_STRU  *pstPcmBuffTmp = (&g_stVoicePcmBuff);

    ulSrcAddrB      = (VOS_UINT32)&pstPcmBuffTmp->asSpkOutBuffB[0];

    /*获取I2S/PCM模式下数据通道个数*/
    uwChannelNum    = DRV_SIO_GetSioChannelNum(DRV_SIO_GetVoiceCfgPtr());

    /*读取通道1源地址寄存器*/
    ulSrcAddr       = UCOM_RegRd(DRV_DMA_GetCxSrcAddr(VOICE_MC_DMAC_CHN_SPK));

    /*当源地址已经为BufferB范围内(LOOP模式)或源地址为BufferA的最后一个位宽(非LOOP模式)*/
    /*此逻辑依赖于BufferA与BufferB地址连续，且BufferA在前*/
    if((ulSrcAddr >= (ulSrcAddrB - sizeof(VOS_UINT32)))
        &&(ulSrcAddr < (ulSrcAddrB + (((VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES * uwChannelNum)
                                - sizeof(VOS_UINT32)))))
    {
        return pstPcmBuffTmp->asSpkOutBuffA;
    }
    else
    {
        return pstPcmBuffTmp->asSpkOutBuffB;
    }

}
VOS_VOID VOICE_PcmSwitchSpkBuff(VOS_VOID)
{
    g_psVoicePcmSpkOut = VOICE_PcmGetWritableSpkBuffPtr();
}


VOS_VOID VOICE_PcmClrLastSpkBuff(VOS_VOID)
{
    UCOM_MemSet(VOICE_PcmGetWritableSpkBuffPtr(),
                0,
                (VOS_UINT16)CODEC_PCM_FRAME_LENGTH * sizeof(VOS_UINT16));
}

/*****************************************************************************
 函 数 名  : VOICE_PcmMicInStartLoopDMA
 功能描述  : 配置并启动链式循环DMA，开始Mic采集PCM数据搬运到数据缓冲区
 输入参数  : ChNum  -  MIC采集的DMA通道号
 输出参数  : 无
 返 回 值  : VOICE_RET_OK         -   成功
             VOICE_RET_ERR_PARAM  -   参数错误
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月30日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数头注释模板

*****************************************************************************/
VOS_UINT32 VOICE_PcmMicInStartLoopDMA(VOS_UINT16 usChNum, DRV_DMA_INT_FUNC pfuncIsr)
{
    VOS_UINT32              uwRet;
    VOS_UINT32              uwChannelNum;
    VOS_UINT32              uwDmaCfg;
    VOS_UINT32              uwDestAddrA, uwDestAddrB;   /* MIC采集的DMA通道的目的地址 */
    VOS_UINT32              uwLliAddrA, uwLliAddrB;     /* MIC采集的DMA通道的LLI地址 */
    DRV_DMA_CXCFG_STRU     *pstDmaCfgA     = VOS_NULL;
    DRV_DMA_CXCFG_STRU     *pstDmaCfgB     = VOS_NULL;
    VOICE_PCM_BUFFER_STRU  *pvPcmBuffTmp   = VOS_NULL;
    DRV_SIO_CFG_STRU       *pStSioCfg      = VOS_NULL;

    if (usChNum >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        OM_LogError(VOICE_PcmMicInStartLoopDMA_DmaChanError);
        return UCOM_RET_ERR_PARA;
    }

    pStSioCfg   = DRV_SIO_GetVoiceCfgPtr();
    uwChannelNum= DRV_SIO_GetSioChannelNum(pStSioCfg);

    /* 根据当前SIO工作模式来进行DMA搬运 */
    if (DRV_SIO_MODE_PCM== pStSioCfg->enWorkMode)
    {
        uwDmaCfg = DRV_DMA_GetVoicePcmSioMemCfg();
    }
    else
    {
        uwDmaCfg = DRV_DMA_GetVoiceI2sSioMemCfg();
    }

    /* 获取DMA配置全局变量的Uncache访问地址 */
    pstDmaCfgA   = (DRV_DMA_CXCFG_STRU *)UCOM_GetUncachedAddr((VOS_UINT32)&g_astVoicePcmMicInDmaPara[0]);
    pstDmaCfgB   = (DRV_DMA_CXCFG_STRU *)UCOM_GetUncachedAddr((VOS_UINT32)&g_astVoicePcmMicInDmaPara[1]);
    pvPcmBuffTmp = (&g_stVoicePcmBuff);

    /* 禁止MIC采集的DMA */
    DRV_DMA_Stop(usChNum);

    /*切换buffer并更新DestAddr的值*/
    if ( g_psVoicePcmMicIn == pvPcmBuffTmp->asMicInBuffA)
    {
        uwDestAddrA = (VOS_UINT32)&pvPcmBuffTmp->asMicInBuffB[0];
        uwDestAddrB = (VOS_UINT32)&pvPcmBuffTmp->asMicInBuffA[0];
    }
    else
    {
        uwDestAddrA = (VOS_UINT32)&pvPcmBuffTmp->asMicInBuffA[0];
        uwDestAddrB = (VOS_UINT32)&pvPcmBuffTmp->asMicInBuffB[0];
    }

    /*LLI数组的地址*/
    uwLliAddrA = UCOM_GetUnmappedAddr((VOS_UINT32)&g_astVoicePcmMicInDmaPara[0]);
    uwLliAddrB = UCOM_GetUnmappedAddr((VOS_UINT32)&g_astVoicePcmMicInDmaPara[1]);

    UCOM_MemSet(pstDmaCfgA, 0, sizeof(g_astVoicePcmMicInDmaPara));

    /*设置DMA配置参数,配置为链表连接，使用MIC通道LOOP配置，每个节点上报一个中断*/
    pstDmaCfgA->uwLli            = DRV_DMA_LLI_LINK(uwLliAddrB);
    pstDmaCfgA->uhwACount        = (VOS_UINT16)((VOS_INT32)CODEC_PCM_FRAME_LENGTH_BYTES * (VOS_INT32)uwChannelNum);
    pstDmaCfgA->uwSrcAddr        = DRV_SIO_GetRxRegAddr(pStSioCfg);
    pstDmaCfgA->uwDstAddr        = uwDestAddrA;
    pstDmaCfgA->uwConfig         = uwDmaCfg;

    pstDmaCfgB->uwLli            = DRV_DMA_LLI_LINK(uwLliAddrA);
    pstDmaCfgB->uhwACount        = (VOS_UINT16)((VOS_INT32)CODEC_PCM_FRAME_LENGTH_BYTES * (VOS_INT32)uwChannelNum);
    pstDmaCfgB->uwSrcAddr        = DRV_SIO_GetRxRegAddr(pStSioCfg);
    pstDmaCfgB->uwDstAddr        = uwDestAddrB;
    pstDmaCfgB->uwConfig         = uwDmaCfg;

    /*配置MIC采集DMA通道进行数据搬运，并注册MIC处DMA中断处理钩子*/
    uwRet = DRV_DMA_StartWithCfg(usChNum,
                                 pstDmaCfgA,
                                 pfuncIsr,//VOICE_MicDmaIsr,
                                 0);

    return uwRet;
}

/*****************************************************************************
 函 数 名  : VOICE_PcmSpkOutStartLoopDMA
 功能描述  : G模式下，配置并启动DMA，将缓冲区PCM数据搬运到语音设备播放
 输入参数  : ChNum  -  SPEAKER播放的DMA通道号
 输出参数  : 无
 返 回 值  : VOICE_RET_OK         -   成功
             VOICE_RET_ERR_PARAM  -   参数错误
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月30日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数头注释模板

*****************************************************************************/
VOS_UINT32 VOICE_PcmSpkOutStartLoopDMA(VOS_UINT16 usChNum, DRV_DMA_INT_FUNC pfuncIsr)
{
    VOS_UINT32              uwRet;
    VOS_UINT32              uwDmaCfg;
    VOS_UINT32              uwChannelNum;
    VOS_UINT32              uwSrcAddrA, uwSrcAddrB;   /* SPK播放的DMA通道的源地址 */
    DRV_DMA_CXCFG_STRU     *pstDmaCfgA     = VOS_NULL;
    DRV_DMA_CXCFG_STRU     *pstDmaCfgB     = VOS_NULL;
    VOICE_PCM_BUFFER_STRU  *pvPcmBuffTmp   = VOS_NULL;
    DRV_SIO_CFG_STRU       *pStSioCfg      = VOS_NULL;
    VOS_UINT32              uwLliAddrA;
    VOS_UINT32              uwLliAddrB;

    /* 检查参数是否非法,通道号0-15 */
    if (usChNum >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        OM_LogError(VOICE_PcmSpkOutStartLoopDMA_DmaChanError);
        return UCOM_RET_ERR_PARA;
    }

    pStSioCfg   = DRV_SIO_GetVoiceCfgPtr();
    uwChannelNum= DRV_SIO_GetSioChannelNum(pStSioCfg);

    /* 根据当前SIO工作模式来进行DMA搬运 */
    if (DRV_SIO_MODE_PCM== pStSioCfg->enWorkMode)
    {
        uwDmaCfg = DRV_DMA_GetVoicePcmMemSioCfg();
    }
    else
    {
        uwDmaCfg = DRV_DMA_GetVoiceI2sMemSioCfg();
    }


    /* 获取DMA配置全局变量的Uncache访问地址 */
    pstDmaCfgA   = (DRV_DMA_CXCFG_STRU *)UCOM_GetUncachedAddr((VOS_UINT32)&g_astVoicePcmSpkOutDmaPara[0]);
    pstDmaCfgB   = (DRV_DMA_CXCFG_STRU *)UCOM_GetUncachedAddr((VOS_UINT32)&g_astVoicePcmSpkOutDmaPara[1]);
    pvPcmBuffTmp = (&g_stVoicePcmBuff);

    /* 禁止SPK播放的DMA */
    DRV_DMA_Stop(usChNum);

    /*切换buffer并更新DestAddr的值*/
    if (g_psVoicePcmSpkOut == pvPcmBuffTmp->asSpkOutBuffA)
    {
        uwSrcAddrA = (VOS_UINT32)&pvPcmBuffTmp->asSpkOutBuffB[0];
        uwSrcAddrB = (VOS_UINT32)&pvPcmBuffTmp->asSpkOutBuffA[0];
    }
    else
    {
        uwSrcAddrA = (VOS_UINT32)&pvPcmBuffTmp->asSpkOutBuffA[0];
        uwSrcAddrB = (VOS_UINT32)&pvPcmBuffTmp->asSpkOutBuffB[0];
    }

    /*LLI数组的地址*/
    uwLliAddrA = UCOM_GetUnmappedAddr((VOS_UINT32)&g_astVoicePcmSpkOutDmaPara[0]);
    uwLliAddrB = UCOM_GetUnmappedAddr((VOS_UINT32)&g_astVoicePcmSpkOutDmaPara[1]);

    UCOM_MemSet(pstDmaCfgA, 0, sizeof(g_astVoicePcmSpkOutDmaPara));

    /*设置DMA配置参数,配置为链表连接，使用SPK通道LOOP配置，每个节点上报一个中断*/
    pstDmaCfgA->uwLli     = DRV_DMA_LLI_LINK(uwLliAddrB);
    pstDmaCfgA->uhwACount = (VOS_UINT16)((VOS_INT32)CODEC_PCM_FRAME_LENGTH_BYTES * (VOS_INT32)uwChannelNum);
    pstDmaCfgA->uwSrcAddr = uwSrcAddrA;
    pstDmaCfgA->uwDstAddr = DRV_SIO_GetTxRegAddr(pStSioCfg);
    pstDmaCfgA->uwConfig  = uwDmaCfg;

    pstDmaCfgB->uwLli     = DRV_DMA_LLI_LINK(uwLliAddrA);
    pstDmaCfgB->uhwACount = (VOS_UINT16)((VOS_INT32)CODEC_PCM_FRAME_LENGTH_BYTES * (VOS_INT32)uwChannelNum);
    pstDmaCfgB->uwSrcAddr = uwSrcAddrB;
    pstDmaCfgB->uwDstAddr = DRV_SIO_GetTxRegAddr(pStSioCfg);
    pstDmaCfgB->uwConfig  = uwDmaCfg;

    /*配置SPEAKER播放DMA通道进行播放到下行的数据搬运, 并注册MIC/SPK处DMA中断处理钩子*/
    uwRet = DRV_DMA_StartWithCfg(usChNum,
                                 pstDmaCfgA,
                                 pfuncIsr,
                                 0);

    return uwRet;
}
VOS_UINT32 VOICE_PcmStartDMAToDataOut(VOS_UINT16 usChNum)
{
    OMMED_RING_BUFFER_CONTROL_STRU     *pstRxRingBufferControl;
    VOS_UINT32                          uwRet;
    VOS_UINT32                          uwSrcAddr;                              /* DMA通道的源地址和目的地址 */
    VOS_UINT32                          uwDestAddr;
    VOS_INT16                           shwFreeSize;

    uwRet                               = UCOM_RET_SUCC;
    shwFreeSize                         = 0;

    /* 检查参数是否非法,通道号0-15 */
    if (usChNum >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        OM_LogError(VOICE_PcmStartDMAToDataOut_DmaChanError);
        return UCOM_RET_ERR_PARA;
    }

    /*从AXI-MEM中读取下行控制信息*/
    pstRxRingBufferControl  = (OMMED_RING_BUFFER_CONTROL_STRU *)(CODEC_PC_VOICE_CODEC_OM_ADDR);

    /*若出现下行环形buffer控制体保护字不匹配，说明控制体被踩，此状态无法恢复，做复位处理*/
    if ((  DRV_MAILBOX_PROTECT1 != pstRxRingBufferControl->uwProtectWord1)
        ||(DRV_MAILBOX_PROTECT1 != pstRxRingBufferControl->uwProtectWord3)
        ||(DRV_MAILBOX_PROTECT2 != pstRxRingBufferControl->uwProtectWord2)
        ||(DRV_MAILBOX_PROTECT2 != pstRxRingBufferControl->uwProtectWord4))
    {
        /* 标明环形buffer控制体保护字异常 */
        OM_LogWarning(VOICE_PcmStartDMAFromDataIn_InvalidMagicNum);

        return UCOM_RET_FAIL;
    }

    if ((0 == pstRxRingBufferControl->uwBufAddr)
    || (0 == pstRxRingBufferControl->uwBufSize))
    {
        OM_LogError(VOICE_PcmStartDMAToDataOut_RingBufUnInit);
        return UCOM_RET_ERR_PARA;
    }

    if (pstRxRingBufferControl->uwReadAddr
    >= pstRxRingBufferControl->uwBufSize)
    {
        OM_LogError(VOICE_PcmStartDMAToDataOut_ReadAddrErr);
        return UCOM_RET_ERR_PARA;
    }

    /* 计算缓冲区空闲长度*/
    if (pstRxRingBufferControl->uwReadAddr > pstRxRingBufferControl->uwWriteAddr)
    {
        shwFreeSize  = (VOS_INT16)(pstRxRingBufferControl->uwReadAddr
                                   - pstRxRingBufferControl->uwWriteAddr);
    }
    else if (pstRxRingBufferControl->uwReadAddr < pstRxRingBufferControl->uwWriteAddr)
    {
        shwFreeSize
            = (VOS_INT16)(
              (pstRxRingBufferControl->uwReadAddr
              + pstRxRingBufferControl->uwBufSize)
              - pstRxRingBufferControl->uwWriteAddr);
    }
    else
    {
        shwFreeSize = 0;
    }

    /*ring buffer是否已满*/
    if ((shwFreeSize <= (VOS_INT16)CODEC_PCM_FRAME_LENGTH_BYTES) && (shwFreeSize != 0))
    {
        OM_LogWarning(VOICE_PcmStartDMAToDataOut_RingBufFull);

        /* ErrorLog 记录事件 */
        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_PCVOICE_RINGBUF_FULL);

        /* 仍然要求OM搬运数据 */
        VOICE_PcmTransferRxDataInd();

        return UCOM_RET_SUCC;
    }

    /* 禁止SPK播放的DMA */
    DRV_DMA_Stop(usChNum);

    /*更新uwDestAddr的值 */
    uwDestAddr = pstRxRingBufferControl->uwBufAddr
                 + pstRxRingBufferControl->uwWriteAddr;

    /*更新uwSrcAddr的值*/
    uwSrcAddr  = (VOS_UINT32)&g_psVoicePcmSpkOut[0];

    /*配置SPEAKER播放DMA通道进行播放到下行的数据搬运*/
    uwRet = DRV_DMA_MemCpy(usChNum,
                           (VOS_UCHAR *)uwDestAddr,
                           (VOS_UCHAR *)uwSrcAddr,
                           (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES,
                           VOICE_SpkDmaIsr,
                           0);

    return uwRet;
}
VOS_UINT32 VOICE_PcmStartDMAFromDataIn(VOS_UINT16 usChNum)
{
    VOS_UINT32                          uwRet;
    VOS_INT16                           shwBufProcess;
    VOS_UINT32                          uwSrcAddr;                              /* DMA通道的源地址和目的地址 */
    VOS_UINT32                          uwDestAddr;
    OMMED_RING_BUFFER_CONTROL_STRU     *pstTxRingBufferControl;
    VOICE_PCM_RING_BUFFER_STATE_STRU   *pstTxRingBufferState;

    pstTxRingBufferState                = VOICE_PcmGetRxBufStatePtr();
    uwRet                               = UCOM_RET_SUCC;

    /* 检查参数是否非法,通道号0-15 */
    if (usChNum >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        OM_LogError(VOICE_PcmStartDMAFromDataIn_DmaChanError);
        return UCOM_RET_ERR_PARA;
    }

    /*从AXI-MEM中读取上行控制信息*/
    pstTxRingBufferControl  = (OMMED_RING_BUFFER_CONTROL_STRU *)(CODEC_PC_VOICE_OM_CODEC_ADDR);

    /*若出现上行环形buffer控制体保护字不匹配，说明控制体被踩，此状态无法恢复，做复位处理*/
    if ((  DRV_MAILBOX_PROTECT1 != pstTxRingBufferControl->uwProtectWord1)
        ||(DRV_MAILBOX_PROTECT1 != pstTxRingBufferControl->uwProtectWord3)
        ||(DRV_MAILBOX_PROTECT2 != pstTxRingBufferControl->uwProtectWord2)
        ||(DRV_MAILBOX_PROTECT2 != pstTxRingBufferControl->uwProtectWord4))
    {
        /* 环形buffer控制字异常，插入舒适噪声 */
        VOICE_PcmMicInNoData();

        OM_LogWarning(VOICE_PcmStartDMAFromDataIn_InvalidMagicNum);

        return UCOM_RET_FAIL;
    }

    if ((0 == pstTxRingBufferControl->uwBufAddr)
    || (0 == pstTxRingBufferControl->uwBufSize))
    {
        OM_LogError(VOICE_PcmStartDMAFromDataIn_RingBufUnInit);
        return UCOM_RET_ERR_PARA;
    }

    if (pstTxRingBufferControl->uwWriteAddr
    >= pstTxRingBufferControl->uwBufSize)
    {
        OM_LogError(VOICE_PcmStartDMAFromDataIn_WriteAddrErr);
        return UCOM_RET_ERR_PARA;
    }

    /*自适应调整数据缓冲长度*/
    VOICE_PcmAdjustRingBufSize(
                    pstTxRingBufferControl,
                    pstTxRingBufferState);

    /*上行缓冲区管理*/
    VOICE_PcmManageRingBuf(
                    pstTxRingBufferState->uhwAdpBufferSize,
                    pstTxRingBufferState->uhwCurrDataSize,
                    &shwBufProcess);

    if (0 != pstTxRingBufferState->uhwCurrDataSize)
    {
        /*插帧数目更新 */
        pstTxRingBufferState->shwInsertFrameCnt
            = pstTxRingBufferState->shwInsertFrameCnt - shwBufProcess;

        if (pstTxRingBufferState->shwInsertFrameCnt < 0)
        {
            pstTxRingBufferState->shwInsertFrameCnt = 0;
        }

        if(0 < shwBufProcess)
        {
            OM_LogWarning(VOICE_PcmStartDMAFromDataIn_DelFrame);
        }

        /*读取地址更新 */
        pstTxRingBufferControl->uwReadAddr
              += (VOS_UINT32)(VOS_INT32)shwBufProcess
                           * (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES;

        if (pstTxRingBufferControl->uwReadAddr >= pstTxRingBufferControl->uwBufSize)
        {
            pstTxRingBufferControl->uwReadAddr
                = pstTxRingBufferControl->uwReadAddr
                  - pstTxRingBufferControl->uwBufSize;
        }

        /* 禁止MIC播放的DMA */
        DRV_DMA_Stop(usChNum);

        /* 更新SrcAddr的值 */
        uwSrcAddr  = pstTxRingBufferControl->uwBufAddr
                     + pstTxRingBufferControl->uwReadAddr;

        /*切换buffer并更新DestAddr的值*/
        uwDestAddr = (VOS_UINT32)&g_psVoicePcmMicIn[0];

        /*配置DMA进行上行数据搬运*/
        uwRet = DRV_DMA_MemCpy(usChNum,
                               (VOS_UCHAR *)uwDestAddr,
                               (VOS_UCHAR *)uwSrcAddr,
                               (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES,
                               VOICE_MicDmaIsr,
                               0);

    }
    else
    {
        /* 插入舒适噪声 */
        VOICE_PcmMicInNoData();
    }

    return uwRet;
}



VOS_VOID VOICE_PcmAdjustRingBufSize(
                OMMED_RING_BUFFER_CONTROL_STRU      *pstTxRingBufferControl,
                VOICE_PCM_RING_BUFFER_STATE_STRU    *pstTxRingBufferState)
{
    VOS_UINT32                          uwTxReadAddr;
    VOS_UINT32                          uwTxWriteAddr;
    VOS_UINT16                          uhwTxCurrDataSize;
    VOS_UINT32                          uwTxBufSize;
    VOS_UINT16                          uhwTxAdpBufSize;
    VOS_INT16                           shwTxInsertFrameCnt;
    VOS_UINT16                          uhwTxCntDataSizeIsOne;
    VOS_UINT16                          uhwTxFrameCnt;

    uwTxReadAddr          = pstTxRingBufferControl->uwReadAddr;
    uwTxWriteAddr         = pstTxRingBufferControl->uwWriteAddr;
    uwTxBufSize           = pstTxRingBufferControl->uwBufSize;
    uhwTxAdpBufSize       = pstTxRingBufferState->uhwAdpBufferSize;
    shwTxInsertFrameCnt   = pstTxRingBufferState->shwInsertFrameCnt;
    uhwTxCntDataSizeIsOne = pstTxRingBufferState->uhwCntDataSizeIsOne;
    uhwTxFrameCnt         = pstTxRingBufferState->uhwFrameCnt;


    /* 计算缓冲区数据长度*/
    if (uwTxReadAddr <= uwTxWriteAddr)
    {
        uhwTxCurrDataSize
            = (VOS_UINT16)(uwTxWriteAddr - uwTxReadAddr) / (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES;
    }
    else
    {
        uhwTxCurrDataSize
            = (VOS_UINT16)(
              (uwTxWriteAddr + uwTxBufSize) - uwTxReadAddr) / (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES;
    }

    if (0 == uhwTxCurrDataSize)
    {
        /* 缓冲区已空，插帧计数器增1*/
        uhwTxCurrDataSize = 0;
        shwTxInsertFrameCnt++;

        if ((shwTxInsertFrameCnt > VOICE_PCM_FRAME_INSERT_THD)
        && (uhwTxAdpBufSize < ((uwTxBufSize / (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES) - 1 )))
        {
            pstTxRingBufferState->shwInsertFrameCnt   = 0;
            pstTxRingBufferState->uhwFrameCnt         = 0;
            pstTxRingBufferState->uhwCntDataSizeIsOne = 0;
            pstTxRingBufferState->uhwCurrDataSize     = 0;
            pstTxRingBufferState->uhwAdpBufferSize++;
            OM_LogWarning(VOICE_PcmAdjustRingBufSize_BufSizeInc);

            return;
        }
    }

    /* 缓冲区数据长度小于等于1，相应计数器增1*/
    if (uhwTxCurrDataSize <= 1 )
    {
        uhwTxCntDataSizeIsOne++;
    }

    /*判断是否达到统计窗长*/
    if (uhwTxFrameCnt < VOICE_PCM_FRAME_WIN_LEN )
    {
        uhwTxFrameCnt++;
    }
    else
    {
        /*达到统计窗长，判断缓冲区是否过大*/
        if ((0 == shwTxInsertFrameCnt)
        &&(uhwTxCntDataSizeIsOne <= 1))
        {
            /*自适应缓冲区减1，下限1*/
            pstTxRingBufferState->uhwAdpBufferSize--;
            OM_LogWarning(VOICE_PcmAdjustRingBufSize_BufSizeDec);

        }

        pstTxRingBufferState->shwInsertFrameCnt   = 0;
        pstTxRingBufferState->uhwFrameCnt         = 0;
        pstTxRingBufferState->uhwCntDataSizeIsOne = 0;
        pstTxRingBufferState->uhwCurrDataSize     = uhwTxCurrDataSize;
        return;
    }
    pstTxRingBufferState->uhwAdpBufferSize    = uhwTxAdpBufSize;
    pstTxRingBufferState->shwInsertFrameCnt   = shwTxInsertFrameCnt;
    pstTxRingBufferState->uhwCntDataSizeIsOne = uhwTxCntDataSizeIsOne;
    pstTxRingBufferState->uhwFrameCnt         = uhwTxFrameCnt;
    pstTxRingBufferState->uhwCurrDataSize     = uhwTxCurrDataSize;

    return;
}


VOS_VOID VOICE_PcmManageRingBuf(
                VOS_UINT16  uhwAdpBufferSize,
                VOS_UINT16  uhwCurrDataSize,
                VOS_INT16  *pshwBufProcess)
{
    /*正常读取*/
    *pshwBufProcess = 0;

    /* 计算缓冲区数据长度*/
    if (0 == uhwCurrDataSize)
    {
        *pshwBufProcess = -1;
    }

    /*正常状态*/
    if (uhwCurrDataSize > uhwAdpBufferSize)
    {
        /*计算丢帧数量*/
        *pshwBufProcess = (VOS_INT16)uhwCurrDataSize - (VOS_INT16)uhwAdpBufferSize;
    }

    return;
}


VOS_VOID VOICE_PcmUpdateTxRingBuf(VOS_VOID)
{
    VOS_UINT16                          uhwDevMode     = VOICE_McGetDeviceMode();
    OMMED_RING_BUFFER_CONTROL_STRU     *pstTxRingBufferControl;

    /*从AHB邮箱读取上行控制信息*/
    pstTxRingBufferControl  = (OMMED_RING_BUFFER_CONTROL_STRU *)(CODEC_PC_VOICE_OM_CODEC_ADDR);

    /* 如果设置模式是PC VOICE */
    if (VCVOICE_DEV_MODE_PCVOICE == uhwDevMode )
    {
        pstTxRingBufferControl->uwReadAddr
            = pstTxRingBufferControl->uwReadAddr + (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES;
        /* 如果地址超过buffer范围，从头开始*/
        if (pstTxRingBufferControl->uwReadAddr >= pstTxRingBufferControl->uwBufSize)
        {
            pstTxRingBufferControl->uwReadAddr = 0;
        }
    }
}
VOS_VOID VOICE_PcmUpdateRxRingBuf(VOS_VOID)
{
    VOS_UINT16                          uhwDevMode   = VOICE_McGetDeviceMode();
    OMMED_RING_BUFFER_CONTROL_STRU     *pstRxRingBufferControl;

    /*从AHB邮箱读取下行控制信息*/
    pstRxRingBufferControl  = (OMMED_RING_BUFFER_CONTROL_STRU *)(CODEC_PC_VOICE_CODEC_OM_ADDR);

    /* 如果设置模式是PC VOICE */
    if (VCVOICE_DEV_MODE_PCVOICE == uhwDevMode)
    {
        pstRxRingBufferControl->uwWriteAddr
            = pstRxRingBufferControl->uwWriteAddr + (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES;

        /* 如果地址超过buffer范围，从头开始*/
        if (pstRxRingBufferControl->uwWriteAddr >= pstRxRingBufferControl->uwBufSize)
        {
            pstRxRingBufferControl->uwWriteAddr = 0;
        }

        /* 通知OM进行下行数据搬运 */
        VOICE_PcmTransferRxDataInd();
    }
}


VOS_VOID VOICE_MicDmaIsr(
                DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                VOS_UINT32 uwPara)
{
    VOICE_VOICE_TX_PROC_REQ_STRU    stTxProcReqMsg;
    VOICE_MC_DMA_FLAG_STRU         *pstDmaFlag  = VOICE_McGetDmaFlagPtr();

    /* 初始化回复消息结构体 */
    UCOM_MemSet(&stTxProcReqMsg, 0, sizeof(VOICE_VOICE_TX_PROC_REQ_STRU));

    /* 若中断类型为TC中断,为Mic处DMA正常中断处理流程 */
    if ((DRV_DMA_INT_TYPE_TC1 == enIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == enIntType))
    {
        /* RUNNING态下进行上行编码控制，包括缓冲区切换，编码消息发送 */
        if (VOICE_MC_STATE_RUNNING == VOICE_McGetForeGroundVoiceState())
        {
            if (CODEC_SWITCH_ON == pstDmaFlag->usMicInSwEnable)
            {
                /* 切换上行缓冲区 */
                VOICE_PcmSwitchMicBuff();
            }

            /* PC-VOICE模式下, 更新环形BUFFER的控制信息*/
            VOICE_PcmUpdateTxRingBuf();

            /*  填充消息ID */
            stTxProcReqMsg.uhwMsgId = ID_VOICE_VOICE_ENC_REQ;

            /* 发送编码消息ID_VOICE_VOICE_ENC_REQ至PID_VOICE,开始语音上行处理及编码 */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            DSP_PID_VOICE,
                            &stTxProcReqMsg,
                            sizeof(stTxProcReqMsg));
        }
        else
        {
            /*非RUNNING态下记录异常，非RUNNING态下收到语音上行DMA搬运中断*/
            OM_LogError(VOICE_MicDmaIsr_NotRunning);
        }
    }
    /* 若中断类型为ERROR中断,记录异常 */
    else
    {
        /*记录异常，出现DMA Error中断*/
        OM_LogError1(VOICE_MicDmaIsr_ErrInt, enIntType);
    }

    return;
}


VOS_VOID VOICE_SpkDmaIsr(
                DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                VOS_UINT32 uwPara)
{
    VOICE_VOICE_RX_PROC_REQ_STRU    stRxProcReqMsg;

    VOICE_MC_DMA_FLAG_STRU          *pstDmaFlag  = VOICE_McGetDmaFlagPtr();

    /* 初始化回复消息结构体 */
    UCOM_MemSet(&stRxProcReqMsg, 0, sizeof(VOICE_VOICE_RX_PROC_REQ_STRU));

    /* 若中断类型为TC中断,为SPK处DMA正常中断处理流程 */
    if ((DRV_DMA_INT_TYPE_TC1 == enIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == enIntType))
    {
        /* RUNNING态下进行下行控制，包括缓冲区切换、UMTS下行时序冲突检测*/
        if (VOICE_MC_STATE_RUNNING == VOICE_McGetForeGroundVoiceState())
        {
            if (CODEC_SWITCH_ON == pstDmaFlag->usSpkOutSwEnable)
            {
                /* 切换下行缓冲区 */
                VOICE_PcmSwitchSpkBuff();
            }
            else
            {
                /* 清空上一帧语音数据 */
                VOICE_PcmClrLastSpkBuff();
            }

            /*  填充消息ID */
            stRxProcReqMsg.uhwMsgId = ID_VOICE_VOICE_DEC_REQ;

            /* 发送解码消息ID_VOICE_VOICE_DEC_REQ至PID_VOICE,开始语音下行处理及解码 */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            DSP_PID_VOICE,
                            &stRxProcReqMsg,
                            sizeof(stRxProcReqMsg));

            /* 下行时序监控 */
            VOICE_McChkSpkConflick();

            /* PC-VOICE模式下, 更新下行环形buffer控制信息 */
            VOICE_PcmUpdateRxRingBuf();
        }
        else
        {
            /*非RUNNING态下记录异常，非RUNNING态下收到语音下行DMA搬运中断*/
            OM_LogError1(VOICE_SpkDmaIsr_NotRunning, enIntType);
        }
    }
    /* 若中断类型为ERROR中断,记录异常 */
    else
    {
        /*记录异常，出现DMA Error中断*/
        OM_LogError1(VOICE_SpkDmaIsr_ErrInt, enIntType);
    }
}


VOS_VOID VOICE_SioIsr(DRV_SIO_INT_MASK_ENUM_UINT32 enIntType, VOS_UINT32 uwPara)
{
    if (DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER
        == (enIntType & DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER))
    {
        /* 记录异常,PCM模式下，为PCM发送FIFO下溢 */
        OM_LogError1(VOICE_SioIsr_DownOverFlow, enIntType);
    }
    else if (DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER
             == (enIntType & DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER))
    {
        /* 记录异常,PCM模式下，为PCM接收FIFO上溢 */
        OM_LogError1(VOICE_SioIsr_UpOverFlow, enIntType);
    }
    else
    {
        /* 记录异常，非语音中断 */
        OM_LogError1(VOICE_SioIsr_ErrInt, enIntType);
    }
}
VOS_VOID VOICE_EcRefDmaIsr(
                DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                VOS_UINT32 uwPara)
{
    /* 若中断类型为TC中断,为Mic处DMA正常中断处理流程 */
    if ((DRV_DMA_INT_TYPE_TC1 == enIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == enIntType))
    {
        /* RUNNING态 */
        if (VOICE_MC_STATE_RUNNING != VOICE_McGetForeGroundVoiceState())
        {
            /*非RUNNING态下记录异常，非RUNNING态下收到语音上行DMA搬运中断*/
            OM_LogError(VOICE_EcRefDmaIsr_NotRunning);
        }
    }
    /* 若中断类型为ERROR中断,记录异常 */
    else
    {
        /*记录异常，出现DMA Error中断*/
        OM_LogError1(VOICE_EcRefDmaIsr_ErrInt, enIntType);
    }

    return;
}
VOS_VOID VOICE_PcmInsertCn(
                VOS_INT16   *pshwIn,
                VOS_INT16    shwLen,
                VOS_INT16    shwPcmRandRange)
{
    VOS_INT16  shwCnt;
    VOS_INT16  shwData;
    VOS_INT16  shwTemp;

    /* 小幅噪声的最大幅度的绝对值不超过上一帧解码后数据最大绝对值，且需要满足:
       1) 不能大于VOICE_PCM_SIGNAL_RAND_RANGE
       2) 为2^N */
    if (shwPcmRandRange >= VOICE_PCM_SIGNAL_RAND_RANGE_32)
    {
        shwPcmRandRange = VOICE_PCM_SIGNAL_RAND_RANGE_32;
    }
    else
    {
        for(shwTemp = VOICE_PCM_SIGNAL_RAND_RANGE_32 >> 1; shwTemp != 0; shwTemp = shwTemp >> 1)
        {
            if ((shwPcmRandRange & shwTemp) != 0)
            {
                shwPcmRandRange = shwTemp;
                break;
            }
        }
    }

    if (0 == shwPcmRandRange)
    {
        UCOM_MemSet(pshwIn, 0, (VOS_UINT32)((VOS_UINT16)shwLen * sizeof(VOS_INT16)));
    }
    else
    {
        shwData = (VOS_INT16)(shwPcmRandRange * 2) - 1;      /* uhwData 二进制为[0111..111]的形式 */
        for(shwCnt = 0; shwCnt < shwLen; shwCnt++)
        {
            shwTemp        = (rand() & 0x7fff ) & shwData;/* [false alarm]:经确认此处使用rand无问题  */
            pshwIn[shwCnt] = shwTemp - shwPcmRandRange;
        }
    }
}


VOS_VOID VOICE_PcmMicInNoData(VOS_VOID)
{
    VOICE_VOICE_TX_PROC_REQ_STRU stTxProcReqMsg;

    UCOM_MemSet(&stTxProcReqMsg, 0, sizeof(stTxProcReqMsg));

    /* 插接近静音帧的小幅度随机数 */
    VOICE_PcmInsertCn(g_psVoicePcmMicIn, CODEC_PCM_FRAME_LENGTH, VOICE_PCM_SIGNAL_RAND_RANGE_32);

    OM_LogWarning(VOICE_PcmStartDMAFromDataIn_InsertFrame);

    /* ErrorLog 记录事件 */
    VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_PCVOICE_INTERPOLATE);

    /* 切换缓冲区 */
    VOICE_PcmSwitchMicBuff();

    /*  填充消息ID */
    stTxProcReqMsg.uhwMsgId = ID_VOICE_VOICE_ENC_REQ;

    /* 发送编码消息ID_VOICE_VOICE_ENC_REQ至PID_VOICE,开始语音处理及编码 */
    UCOM_SendOsaMsg(DSP_PID_VOICE, DSP_PID_VOICE, &stTxProcReqMsg, sizeof(stTxProcReqMsg));
}
VOS_UINT32 VOICE_PcmHybrid2Stereo(VOS_INT16 *pshwI2S, VOS_INT16 *pshwLeft, VOS_INT16 *pshwRight, VOS_INT16 shwPcmLen)
{
    VOS_INT16                   shwCnt      = 0;
    VOS_UINT32                  uwSize;
    DRV_SIO_CFG_STRU           *pstCfg      = VOS_NULL;

    pstCfg  = DRV_SIO_GetVoiceCfgPtr();

    if (DRV_SIO_MODE_I2S ==pstCfg->enWorkMode)
    {
        for (shwCnt = 0; shwCnt < shwPcmLen; shwCnt++)
        {
            /* SIO左声道高16bit,右声道低16bit */
            pshwRight[shwCnt]  = (VOS_INT16)pshwI2S[shwCnt*2];
            pshwLeft[shwCnt]   = (VOS_INT16)pshwI2S[(shwCnt * 2) + 1];
        }
    }
    else
    {
        uwSize = (VOS_UINT32)((VOS_UINT16)shwPcmLen*(VOS_UINT16)sizeof(VOS_INT16));

        UCOM_MemCpy(pshwRight, pshwI2S, uwSize);
        UCOM_MemCpy(pshwLeft,  pshwI2S, uwSize);
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_PcmStereo2Hybrid(
                VOS_INT16  *pshwSpkOutTemp,
                VOS_INT16  *pshwSpkOut,
                VOS_INT16   shwPcmLen)
{
    VOS_INT16           shwCnt = 0;
    DRV_SIO_CFG_STRU   *pstCfg = VOS_NULL;

    /* 入参判断 */
    if (   (VOS_NULL == pshwSpkOutTemp)
        || (VOS_NULL == pshwSpkOut))
    {
        return UCOM_RET_NUL_PTR;
    }

    pstCfg  = DRV_SIO_GetVoiceCfgPtr();

    if (DRV_SIO_MODE_I2S ==pstCfg->enWorkMode)
    {
        /* 将PP处理以后的数据填充为I2S双声道数据 */
        for (shwCnt = 0; shwCnt < shwPcmLen; shwCnt++)
        {
            pshwSpkOut[2 * shwCnt]       = pshwSpkOutTemp[shwCnt];
            pshwSpkOut[(2 * shwCnt) + 1] = pshwSpkOutTemp[shwCnt];
        }
    }
    else
    {
        UCOM_MemCpy(pshwSpkOut, pshwSpkOutTemp, (VOS_UINT16)shwPcmLen*(VOS_UINT16)sizeof(VOS_INT16));
    }

    return UCOM_RET_SUCC;
}


VOS_VOID VOICE_PcmVcLoopMicIsr(DRV_DMA_INT_TYPE_ENUM_UINT16 uhwPara, VOS_UINT32 uwIntType)
{
    VOS_UINT32                      uwSize;
    VOS_UINT32                      uwChannelNum;

    uwChannelNum    = DRV_SIO_GetSioChannelNum(DRV_SIO_GetVoiceCfgPtr());

    /* 传输数据大小 */
    uwSize          = (CODEC_FRAME_LENGTH_NB * sizeof(VOS_INT16)) * uwChannelNum;

    /* 若中断类型为TC中断,为Mic处DMA正常中断处理流程；若中断类型为ERROR中断,记录异常 */
    if (  (DRV_DMA_INT_TYPE_TC1 == uwIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == uwIntType))
    {
        /* 切换上行缓冲区 */
        VOICE_PcmSwitchMicBuff();

        /* VOICE PCM SPK OUT和VOICE PCM MIC IN都是I2S的数据，不需要进行I2S<->PCM的转换 */
        UCOM_MemCpy((void*)g_psVoicePcmSpkOut,
                    (void*)g_psVoicePcmMicIn,
                    uwSize);

        g_uwVcLoopCnt++;
        OM_LogInfo1(VOICE_PcmVcLoopMic, g_uwVcLoopCnt);
    }
    else
    {
        /*记录异常，出现DMA Error中断*/
        OM_LogError(VOICE_PcmVcLoopMicIsr_IntTypeError);
    }

    return;
}
VOS_VOID VOICE_PcmVcLoopSpkIsr(DRV_DMA_INT_TYPE_ENUM_UINT16 uhwPara, VOS_UINT32 uwIntType)
{
    /* 若中断类型为TC中断,为SPK处DMA正常中断处理流程；若中断类型为ERROR中断,记录异常 */
    if (  (DRV_DMA_INT_TYPE_TC1 == uwIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == uwIntType))
    {
        /* 切换下行缓冲区 */
        VOICE_PcmSwitchSpkBuff();
        g_uwVcLoopCnt++;
        OM_LogInfo1(VOICE_PcmVcLoopSpk, g_uwVcLoopCnt);
    }
    else
    {
        /*记录异常，出现DMA Error中断*/
        OM_LogError(VOICE_PcmVcLoopSpkIsr_IntTypeError);
    }
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

