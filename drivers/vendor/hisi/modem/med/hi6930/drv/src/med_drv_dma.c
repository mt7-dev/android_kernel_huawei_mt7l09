

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_drv_dma.h"
#include "ucom_comm.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* DMA通道中断处理函数指针数组, 由DRV_DMA_RegisterIsr赋值 */
DRV_DMA_CXISR_STRU g_astDrvDmaCxIntIsr[DRV_DMA_MAX_CHANNEL_NUM];

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID DRV_DMA_Init( VOS_VOID )
{
    DRV_DMA_CXISR_STRU                 *pstDmaCxIsr = DRV_DMA_GetCxIsrPtr();

    /*初始化g_astDrvDmaCxIntIsr为全零*/
    UCOM_MemSet(pstDmaCxIsr, 0, sizeof(DRV_DMA_CXISR_STRU)*DRV_DMA_MAX_CHANNEL_NUM);

    /*依次写0到如下寄存器默认屏蔽HiFi DMA中断*/
    UCOM_RegWr(DRV_DMA_INT_ERR1_MASK_HIFI, 0);
    UCOM_RegWr(DRV_DMA_INT_ERR2_MASK_HIFI, 0);
    UCOM_RegWr(DRV_DMA_INT_ERR3_MASK_HIFI, 0);
    UCOM_RegWr(DRV_DMA_INT_TC1_MASK_HIFI, 0);
    UCOM_RegWr(DRV_DMA_INT_TC2_MASK_HIFI, 0);

    /*注册DMA ISR*/
    VOS_ConnectInterrupt(DRV_DMA_INT_NO_HIFI, DRV_DMA_Isr);

    /*使能EMAC中断*/
    VOS_EnableInterrupt(DRV_DMA_INT_NO_HIFI);

}


VOS_UINT32 DRV_DMA_StartWithCfg(
                VOS_UINT16              uhwChannelNo,
                DRV_DMA_CXCFG_STRU     *pstCfg,
                DRV_DMA_INT_FUNC        pfIntHandleFunc,
                VOS_UINT32              uwPara)
{
    VOS_UINT32                          uwChannelMask   = (0x1L << uhwChannelNo);
    DRV_DMA_CXISR_STRU                 *pstCxIsr        = DRV_DMA_GetCxIsrPtr();

    /* 检查参数是否非法 */
    if (uhwChannelNo >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        return VOS_ERR;
    }

    /*写0到DRV_DMA_CX_CONFIG(uhwChannelNo) bit0禁止通道*/
    UCOM_RegBitWr(DRV_DMA_CX_CONFIG((VOS_UINT32)uhwChannelNo),
                  UCOM_BIT0,
                  UCOM_BIT0,
                  0);

    /*写通道X当前一维传输剩余的Byte数,[15,0]*/
    UCOM_RegBitWr(DRV_DMA_CX_CNT0((VOS_UINT32)uhwChannelNo),
                  UCOM_BIT0,
                  UCOM_BIT15,
                  pstCfg->uhwACount);

    /*写通道X当前二维传输剩余的Array个数,[31,16]*/
    UCOM_RegBitWr(DRV_DMA_CX_CNT0((VOS_UINT32)uhwChannelNo),
                  UCOM_BIT16,
                  UCOM_BIT31,
                  pstCfg->uhwBCount);

    /*写通道X当前三维传输剩余的Frame数,[15,0]*/
    UCOM_RegBitWr(DRV_DMA_CX_CNT1((VOS_UINT32)uhwChannelNo),
                  UCOM_BIT0,
                  UCOM_BIT15,
                  pstCfg->uhwCCount);

    /*写通道X的二维源地址偏移量[31,16]及目的地址偏移量[15,0]*/
    UCOM_RegBitWr(DRV_DMA_CX_BINDX((VOS_UINT32)uhwChannelNo),
                  UCOM_BIT0,
                  UCOM_BIT15,
                  pstCfg->uhwDstBIndex);
    UCOM_RegBitWr(DRV_DMA_CX_BINDX((VOS_UINT32)uhwChannelNo),
                  UCOM_BIT16,
                  UCOM_BIT31,
                  pstCfg->uhwSrcBIndex);

    /*写通道X的三维源地址偏移量[31,16]及目的地址偏移量[15,0]*/
    UCOM_RegBitWr(DRV_DMA_CX_CINDX((VOS_UINT32)uhwChannelNo),
                  UCOM_BIT0,
                  UCOM_BIT15,
                  pstCfg->uhwDstCIndex);
    UCOM_RegBitWr(DRV_DMA_CX_CINDX((VOS_UINT32)uhwChannelNo),
                  UCOM_BIT16,
                  UCOM_BIT31,
                  pstCfg->uhwSrcCIndex);

    /*写通道X的源地址[31,0]及目的地址[31,0]*/
    UCOM_RegWr(DRV_DMA_CX_DES_ADDR((VOS_UINT32)uhwChannelNo), pstCfg->uwDstAddr);
    UCOM_RegWr(DRV_DMA_CX_SRC_ADDR((VOS_UINT32)uhwChannelNo), pstCfg->uwSrcAddr);

    /*写通道X的链表地址配置*/
    UCOM_RegWr(DRV_DMA_CX_LLI((VOS_UINT32)uhwChannelNo), pstCfg->uwLli);

    /*清除通道X的各种中断状态*/
    UCOM_RegWr(DRV_DMA_INT_TC1_RAW, uwChannelMask);
    UCOM_RegWr(DRV_DMA_INT_TC2_RAW, uwChannelMask);
    UCOM_RegWr(DRV_DMA_INT_ERR1_RAW, uwChannelMask);
    UCOM_RegWr(DRV_DMA_INT_ERR2_RAW, uwChannelMask);
    UCOM_RegWr(DRV_DMA_INT_ERR3_RAW, uwChannelMask);

    /*若回调函数非空,则保存该值*/
    if (VOS_NULL != pfIntHandleFunc)
    {
        pstCxIsr[uhwChannelNo].pfFunc  = pfIntHandleFunc;
        pstCxIsr[uhwChannelNo].uwPara  = uwPara;

        /*依次写(uhwChannelNo对应bit为1)到如下寄存器打开HiFi相应DMA通道中断屏蔽*/
        UCOM_RegBitWr(DRV_DMA_INT_ERR1_MASK_HIFI, uhwChannelNo, uhwChannelNo, 1);
        UCOM_RegBitWr(DRV_DMA_INT_ERR2_MASK_HIFI, uhwChannelNo, uhwChannelNo, 1);
        UCOM_RegBitWr(DRV_DMA_INT_ERR3_MASK_HIFI, uhwChannelNo, uhwChannelNo, 1);
        UCOM_RegBitWr(DRV_DMA_INT_TC1_MASK_HIFI, uhwChannelNo, uhwChannelNo, 1);
        UCOM_RegBitWr(DRV_DMA_INT_TC2_MASK_HIFI, uhwChannelNo, uhwChannelNo, 1);

    }

    /*写通道X的配置*/
    UCOM_RegWr(DRV_DMA_CX_CONFIG((VOS_UINT32)uhwChannelNo), pstCfg->uwConfig);
    return VOS_OK;

}
VOS_UINT32 DRV_DMA_MemCpy(
                VOS_UINT16              uhwChannelNo,
                VOS_UCHAR              *pucDes,
                VOS_UCHAR              *pucSrc,
                VOS_UINT32              uwLen,
                DRV_DMA_INT_FUNC        pfIntHandleFunc,
                VOS_UINT32              uwPara)
{
    VOS_UINT32                          uwRet;
    DRV_DMA_CXCFG_STRU                  stCfg;

    /*初始化配置结构体*/
    UCOM_MemSet(&stCfg, 0, sizeof(DRV_DMA_CXCFG_STRU));

    /*以一维、非链表、非通道链接方式配置参数*/
    stCfg.uhwACount     = (VOS_UINT16)uwLen;
    stCfg.uwDstAddr     = (VOS_UINT32)pucDes;
    stCfg.uwSrcAddr     = (VOS_UINT32)pucSrc;
    stCfg.uwConfig      = DRV_DMA_MEM_MEM_CFG;     /*典型配置(M2M/地址递增)*/

    /*调用内部接口开始配置*/
    uwRet   = DRV_DMA_StartWithCfg(uhwChannelNo, &stCfg, pfIntHandleFunc, uwPara);

    return uwRet;
}


VOS_VOID DRV_DMA_Stop(VOS_UINT16 uhwChannelNo)
{
    DRV_DMA_CXISR_STRU                 *pstCxIsr        = DRV_DMA_GetCxIsrPtr();

    /* 写0到DRV_DMA_CX_CONFIG(uhwChannelNo) bit0 停止对应DMA通道 */
    UCOM_RegBitWr(DRV_DMA_CX_CONFIG((VOS_UINT32)uhwChannelNo), UCOM_BIT0, UCOM_BIT0, 0);

    /*依次写(uhwChannelNo对应bit为0)到如下寄存器屏蔽HiFi相应DMA通道中断*/
    UCOM_RegBitWr(DRV_DMA_INT_ERR1_MASK_HIFI, uhwChannelNo, uhwChannelNo, 0);
    UCOM_RegBitWr(DRV_DMA_INT_ERR2_MASK_HIFI, uhwChannelNo, uhwChannelNo, 0);
    UCOM_RegBitWr(DRV_DMA_INT_ERR3_MASK_HIFI, uhwChannelNo, uhwChannelNo, 0);
    UCOM_RegBitWr(DRV_DMA_INT_TC1_MASK_HIFI, uhwChannelNo, uhwChannelNo, 0);
    UCOM_RegBitWr(DRV_DMA_INT_TC2_MASK_HIFI, uhwChannelNo, uhwChannelNo, 0);

    /*清除全局变量中对应通道处理内容*/
    UCOM_MemSet(&pstCxIsr[uhwChannelNo],0,sizeof(DRV_DMA_CXISR_STRU));
}
VOS_VOID DRV_DMA_Isr(VOS_VOID)
{
    VOS_UINT32                          i;
    VOS_UINT32                          uwErr1;
    VOS_UINT32                          uwErr2;
    VOS_UINT32                          uwErr3;
    VOS_UINT32                          uwTc1;
    VOS_UINT32                          uwTc2;
    VOS_UINT32                          uwIntMask;
    VOS_UINT32                          uwIntState;
    DRV_DMA_INT_TYPE_ENUM_UINT16        enIntType;
    DRV_DMA_CXISR_STRU                 *pstDmaCxIsr = DRV_DMA_GetCxIsrPtr();

    /*读取HIFI CPU的DMA通道中断状态*/
    uwIntState  = UCOM_RegRd(DRV_DMA_INT_STAT_HIFI);

    /*若通道状态全0表示全通道无中断或中断已处理,直接退出*/
    if (0 == uwIntState)
    {
        return;
    }

    /*读取各中断寄存器,查询各中断状态,包括Tc1/Tc2/Err1/Err2/Err3*/
    uwErr1      = UCOM_RegRd(DRV_DMA_INT_ERR1_HIFI);
    uwErr2      = UCOM_RegRd(DRV_DMA_INT_ERR2_HIFI);
    uwErr3      = UCOM_RegRd(DRV_DMA_INT_ERR3_HIFI);
    uwTc1       = UCOM_RegRd(DRV_DMA_INT_TC1_HIFI);
    uwTc2       = UCOM_RegRd(DRV_DMA_INT_TC2_HIFI);

    /*写uwIntState依次到各寄存器清除中断,清除本次查询到的通道中断*/
    UCOM_RegWr(DRV_DMA_INT_TC1_RAW, uwIntState);
    UCOM_RegWr(DRV_DMA_INT_TC2_RAW, uwIntState);
    UCOM_RegWr(DRV_DMA_INT_ERR1_RAW, uwIntState);
    UCOM_RegWr(DRV_DMA_INT_ERR2_RAW, uwIntState);
    UCOM_RegWr(DRV_DMA_INT_ERR3_RAW, uwIntState);

    /*遍历调用各通道注册的回调处理函数*/
    for ( i = 0; i < DRV_DMA_MAX_CHANNEL_NUM; i++)
    {
        uwIntMask = 0x1L << i;

        /*若该通道有中断产生(对应比特为1)*/
        if (uwIntState & uwIntMask)
        {
            if (VOS_NULL != pstDmaCxIsr[i].pfFunc)
            {
                /*若ERR1中断状态对应比特为1*/
                if (uwErr1 & uwIntMask)
                {
                    enIntType = DRV_DMA_INT_TYPE_ERR1;
                }
                /*若ERR2中断状态对应比特为1*/
                else if (uwErr2 & uwIntMask)
                {
                    enIntType = DRV_DMA_INT_TYPE_ERR2;
                }
                /*若ERR3中断状态对应比特为1*/
                else if (uwErr3 & uwIntMask)
                {
                    enIntType = DRV_DMA_INT_TYPE_ERR3;
                }
                /*若TC1中断状态对应比特为1*/
                else if (uwTc1 & uwIntMask)
                {
                    enIntType = DRV_DMA_INT_TYPE_TC1;
                }
                /*若TC2中断状态对应比特为1*/
                else if (uwTc2 & uwIntMask)
                {
                    enIntType = DRV_DMA_INT_TYPE_TC2;
                }
                /*未知中断*/
                else
                {
                    enIntType = DRV_DMA_INT_TYPE_BUTT;
                }

                /*调用注册的中断处理函数*/
                pstDmaCxIsr[i].pfFunc(enIntType, pstDmaCxIsr[i].uwPara);
            }
        }
    }
}
VOS_UINT32 DRV_DMA_GetVoicePcmSioMemCfg(VOS_VOID)
{
    return DRV_DMA_VOICE_PCM_SIO_MEM_CFG;
}


VOS_UINT32 DRV_DMA_GetVoicePcmMemSioCfg(VOS_VOID)
{
    return DRV_DMA_VOICE_PCM_MEM_SIO_CFG;
}


VOS_UINT32 DRV_DMA_GetVoiceI2sSioMemCfg(VOS_VOID)
{
    return DRV_DMA_VOICE_I2S_SIO_MEM_CFG;
}


VOS_UINT32 DRV_DMA_GetVoiceI2sMemSioCfg(VOS_VOID)
{
    return DRV_DMA_VOICE_I2S_MEM_SIO_CFG;
}


VOS_UINT32 DRV_DMA_GetAudioSioMemCfg(VOS_VOID)
{
    return DRV_DMA_AUDIO_SIO_MEM_CFG;
}


VOS_UINT32 DRV_DMA_GetAudioMemSioCfg(VOS_VOID)
{
    return DRV_DMA_AUDIO_MEM_SIO_CFG;
}


VOS_UINT32 DRV_DMA_GetCxSrcAddr(VOS_UINT32 uwChn)
{
    return DRV_DMA_CX_SRC_ADDR(uwChn);
}


VOS_UINT32 DRV_DMA_GetCxDesAddr(VOS_UINT32 uwChn)
{
    return DRV_DMA_CX_DES_ADDR(uwChn);
}


VOS_UINT16 DRV_DMA_GetSioTxChn(VOS_VOID)
{
    return DRV_DMA_SIO_TX_CHN;
}


VOS_UINT16 DRV_DMA_GetSioRxChn(VOS_VOID)
{
    return DRV_DMA_SIO_RX_CHN;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

