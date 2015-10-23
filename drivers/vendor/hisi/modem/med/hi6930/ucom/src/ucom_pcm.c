

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "ucom_pcm.h"
#include "ucom_comm.h"
#include "om_log.h"
#include "med_drv_sio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_UCOM_PCM_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
UCOM_ALIGN(32)
DRV_DMA_CXCFG_STRU                      g_astSmartPaInDmaPara[2] = {0};


UCOM_PCM_BUFFER_STRU                    g_stUcomPcmBuff = {0};

UCOM_PCM_CFG_STRU                       g_stSmartPaCfg = {0};

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID UCOM_PCM_SmartPaPcmInit(VOS_VOID)
{
    UCOM_PCM_CFG_STRU      *pstSmartPaCfg;
    VOS_VOID               *pvSpaInDma;                                         /* SMART PA DMA配置指针 */
    VOS_VOID               *pvPcmBuff;

    pstSmartPaCfg           = UCOM_PCM_GetSpaCfgPtr();

    /* 1. Smart PA已初始化，不进行操作，返回错误 */
    if (VOS_YES == pstSmartPaCfg->enIsInit)
    {
        OM_LogError(UCOM_PCM_SmartPaPcmInitFail);
        return;
    }

    /* 2. 初始化EMAC通道配置实体和公用PCM码流BUFF结构体，内容初始化为全0 */
    pvPcmBuff              = (VOS_VOID *)&g_stUcomPcmBuff;
    pvSpaInDma             = (VOS_VOID *)&g_astSmartPaInDmaPara[0];

    UCOM_MemSet(pvPcmBuff, 0, sizeof(g_stUcomPcmBuff));
    UCOM_MemSet(pvSpaInDma, 0, sizeof(g_astSmartPaInDmaPara));

    pstSmartPaCfg->pshwDestVirtualAddr = g_stUcomPcmBuff.ashwSmartPaBuffA;

    /* 3. 将标志置为SMART PA已初始化 */
    pstSmartPaCfg->enIsInit = VOS_YES;

    OM_LogInfo(UCOM_PCM_SmartPaPcmInitOk);

    return;
}


VOS_UINT32 UCOM_PCM_SmartPaStartLoopDma(
                VOS_UINT16              uhwFrameLenBytes,
                VOS_UINT32              uwSampleRate,
                VOS_UINT16              uhwSmartPaMaster,
                VOS_UINT16              usChNum,
                DRV_DMA_INT_FUNC        pfuncIsr)
{
    VOS_UINT32                    uwRet;
    VOS_UINT32                    uwDestAddrA, uwDestAddrB;   /* SmartPA处理的DMA通道的目的地址 */
    VOS_UINT32                    uwLliAddrA, uwLliAddrB;     /* SmartPA处理的DMA通道的LLI地址 */
    DRV_DMA_CXCFG_STRU           *pstDmaCfgA     = VOS_NULL;
    DRV_DMA_CXCFG_STRU           *pstDmaCfgB     = VOS_NULL;
    UCOM_PCM_BUFFER_STRU         *pstPcmBuffTmp  = VOS_NULL;
    UCOM_PCM_CFG_STRU            *pstSmartPaCfg  = UCOM_PCM_GetSpaCfgPtr();
    DRV_SIO_INT_MASK_ENUM_UINT32  enSioMask     = 0;

    DRV_SIO_SmartPa_Close();

    DRV_SIO_SmartPa_Open(enSioMask,
                         uwSampleRate,
                         uhwSmartPaMaster,
                         VOS_NULL,
                         0);

    /* 检查Smart PA是否已初始化 */
    if (VOS_NO == pstSmartPaCfg->enIsInit)
    {
        OM_LogError(UCOM_PCM_SmartPaStartLoopDma_ErrStatus);
        return UCOM_RET_ERR_STATE;
    }

    /* 检查参数是否非法,通道号0-15 */
    if (usChNum >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        OM_LogError(UCOM_PCM_SmartPaStartLoopDma_ErrChanNum);
        return UCOM_RET_ERR_PARA;
    }

    /* 检查搬运长度的合法性 */
    if (   (0 == uhwFrameLenBytes)
        || (uhwFrameLenBytes > ((UCOM_PCM_MAX_FRAME_LENGTH * sizeof(VOS_INT16)) * UCOM_PCM_I2S_CHANNEL_NUM)))
    {
        OM_LogError(UCOM_PCM_SmartPaStartLoopDma_InvalidFrameLen);
        return UCOM_RET_ERR_PARA;
    }

    /* 更新SMART PA控制结构体中的帧长度，该长度决定了每次搬运的长度 */
    pstSmartPaCfg->uhwDataLenBytes = uhwFrameLenBytes;

    /* 获取DMA配置全局变量的Uncache访问地址 */
    pstDmaCfgA   = (DRV_DMA_CXCFG_STRU *)UCOM_GetUncachedAddr((VOS_UINT32)&g_astSmartPaInDmaPara[0]);
    pstDmaCfgB   = (DRV_DMA_CXCFG_STRU *)UCOM_GetUncachedAddr((VOS_UINT32)&g_astSmartPaInDmaPara[1]);

    pstPcmBuffTmp= (&g_stUcomPcmBuff);

    /* 禁止SmartPA的DMA */
    DRV_DMA_Stop(usChNum);

    /*切换buffer并更新DestAddr的值*/
    if (pstSmartPaCfg->pshwDestVirtualAddr == pstPcmBuffTmp->ashwSmartPaBuffA)
    {
        uwDestAddrA = UCOM_GetUncachedAddr((VOS_UINT32)&pstPcmBuffTmp->ashwSmartPaBuffB[0]);
        uwDestAddrB = UCOM_GetUncachedAddr((VOS_UINT32)&pstPcmBuffTmp->ashwSmartPaBuffA[0]);
    }
    else
    {
        uwDestAddrA = UCOM_GetUncachedAddr((VOS_UINT32)&pstPcmBuffTmp->ashwSmartPaBuffA[0]);
        uwDestAddrB = UCOM_GetUncachedAddr((VOS_UINT32)&pstPcmBuffTmp->ashwSmartPaBuffB[0]);
    }

    /*LLI数组的地址*/
    uwLliAddrA = (VOS_UINT32)pstDmaCfgA;
    uwLliAddrB = (VOS_UINT32)pstDmaCfgB;
    UCOM_MemSet(pstDmaCfgA, 0, sizeof(g_astSmartPaInDmaPara));

    /*设置DMA配置参数,配置为链表连接，使用SMART_PA通道LOOP配置，每个节点上报一个中断*/
    pstDmaCfgA->uwLli            = DRV_DMA_LLI_LINK(uwLliAddrB);
    pstDmaCfgA->uhwACount        = uhwFrameLenBytes;
    pstDmaCfgA->uwSrcAddr        = DRV_SIO_GetRxRegAddr(DRV_SIO_GetSmartPaCfgPtr());
    pstDmaCfgA->uwDstAddr        = uwDestAddrA;
    pstDmaCfgA->uwConfig         = DRV_DMA_SMARTPA_SIO_MEM_CFG;

    pstDmaCfgB->uwLli            = DRV_DMA_LLI_LINK(uwLliAddrA);
    pstDmaCfgB->uhwACount        = uhwFrameLenBytes;
    pstDmaCfgB->uwSrcAddr        = DRV_SIO_GetRxRegAddr(DRV_SIO_GetSmartPaCfgPtr());
    pstDmaCfgB->uwDstAddr        = uwDestAddrB;
    pstDmaCfgB->uwConfig         = DRV_DMA_SMARTPA_SIO_MEM_CFG;

    /*配置通道进行SmartPA到HIFI的数据搬运, 并注册DMA中断处理钩子*/
    uwRet = DRV_DMA_StartWithCfg(usChNum,
                                 pstDmaCfgA,
                                 pfuncIsr,
                                 0);

    return uwRet;
}


VOS_UINT32 UCOM_PCM_SmartPaGetAvailVirtAddr(VOS_VOID)
{
    VOS_UINT32              ulAvailableVirtualAddr;
    VOS_UINT32              ulDestAddr, ulDestAddrB;   /* SPA播放的DMA通道的目的地址 */
    UCOM_PCM_BUFFER_STRU   *pstPcmBuffTmp = (&g_stUcomPcmBuff);
    UCOM_PCM_CFG_STRU      *pstSmartPaCfg = UCOM_PCM_GetSpaCfgPtr();

    ulDestAddrB  = UCOM_GetUncachedAddr((VOS_UINT32)&pstPcmBuffTmp->ashwSmartPaBuffB[0]);

    /*读取通道5目的地址寄存器*/
    ulDestAddr   = UCOM_RegRd(DRV_DMA_CX_DES_ADDR(UCOM_PCM_DMAC_CHN_SPA));

    /*当目的地址已经为BufferB范围内(LOOP模式)或目的地址为BufferA的最后一个位宽(非LOOP模式)*/
    /*此逻辑依赖于BufferA与BufferB地址连续，且BufferA在前*/
    if((ulDestAddr >= (ulDestAddrB - sizeof(VOS_UINT32)))
        &&(ulDestAddr < (ulDestAddrB + (((VOS_UINT32)pstSmartPaCfg->uhwDataLenBytes)
                                - sizeof(VOS_UINT32)))))
    {
        pstSmartPaCfg->pshwDestVirtualAddr = pstPcmBuffTmp->ashwSmartPaBuffA;
    }
    else
    {
        pstSmartPaCfg->pshwDestVirtualAddr = pstPcmBuffTmp->ashwSmartPaBuffB;
    }

    ulAvailableVirtualAddr = (VOS_UINT32)pstSmartPaCfg->pshwDestVirtualAddr;

    return ulAvailableVirtualAddr;

}


VOS_VOID UCOM_PCM_SmartPaStop(VOS_VOID)
{
    /* 停止Smart PA的DMA */
    DRV_DMA_Stop(UCOM_PCM_DMAC_CHN_SPA);

    /* 停止SIO */
    DRV_SIO_SmartPa_Close();
}


VOS_UINT16 UCOM_PCM_GetSmartPaDmacChn(VOS_VOID)
{
    return UCOM_PCM_DMAC_CHN_SPA;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

