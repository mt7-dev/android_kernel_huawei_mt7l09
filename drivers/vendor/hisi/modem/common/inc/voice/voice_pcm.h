/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : VOICE_pcm.h
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年6月2日
  最近修改   :
  功能描述   : VOICE_pcm.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年6月2日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/
#ifndef __VOICE_PCM_H__
#define __VOICE_PCM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_com_codec.h"
#include "OmCodecInterface.h"
#include "VcCodecInterface.h"
#include "ucom_stub.h"
#include "med_drv_ipc.h"
#include "med_drv_dma.h"
#include "med_drv_sio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define VOICE_PCM_FRAME_INSERT_THD   (5)                                     /* 插帧数阈值 */
#define VOICE_PCM_FRAME_WIN_LEN      (1000)                                  /* 缓冲区数据统计窗长*/

#define VOICE_PCM_SHIFT_RIGHT_16     (16)                                    /* 右移16位*/
#define VOICE_PCM_AHB_ADDR_INC       (4)                                     /* AHB邮箱地址增量*/
#define VOICE_PCM_TX_BUF_SIZE_INIT   (5)                                     /* 上行环形buf初始值*/
#define VOICE_PCM_RX_BUF_SIZE_INIT   (3)                                     /* 上行环形buf初始值*/
#define VOICE_PCM_SIGNAL_RAND_RANGE_32  (32)                                    /* 小信号随机数幅度 */

#define VOICE_PcmSaveNvMemAddr(uwAddr)\
                (g_stVoicePcVoiceObj.uwRingBuffBaseAddr = (uwAddr))
/*****************************************************************************
 宏    名  : VOICE_PcmTransferRxDataInd
 功能描述  : 通知OM进行下行数据搬运
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无

 ****************************************************************************/
#define VOICE_PcmTransferRxDataInd() \
{ \
    DRV_IPC_TrigInt(DRV_IPC_CORE_ACPU, PC_VOICE_RX_DATA_ACPU_IPC_BIT); \
}

#define VOICE_CODED_FRAME_LENGTH                   ((VOS_UINT16)164)               /* 一帧编码后的语音数据的最大帧长，单位双字节，最大的为AMR_WB下72个字节*/
#define VOICE_CODED_FRAME_WITH_OBJ_LEN             ((VOS_UINT16)256)               /* 一帧编码后的语音数据的最大帧长，单位双字节，带标志*/
#define VOICE_CODED_DECODE_BUF_FRAME_NUM           ((VOS_UINT16)2)               /* 解码序列缓存的最大帧数为2，由于GSM下20ms内最多会收到2帧待解码数据 */
#define VOICE_CODED_DECODE_SERIAL_LEN  \
        ((VOICE_CODED_DECODE_BUF_FRAME_NUM * VOICE_CODED_FRAME_WITH_OBJ_LEN))   /* 解码序列缓存的长度，缓存2帧待解码数据 */


/* 全局变量封装 */
/* 暂时先改为只处理主MIC数据,后续再进行左右同时处理 */
#define VOICE_PcmGetMcInBufPtr()        (g_stVoicePcmBuff.asMicInBuffLeft)
#define VOICE_PcmGetRcInBufPtr()        (g_stVoicePcmBuff.asMicInBuffRight)
#define VOICE_PcmGetSpkOutBufPtr()      (g_psVoicePcmSpkOut)
#define VOICE_PcmGetSpkOutBufTempPtr()  (g_stVoicePcmBuff.asSpkOutBuffTemp)
#define VOICE_PcmGetRxBufStatePtr()     (&g_stVoicePcVoiceObj.stTxRingBufferState)
#define VOICE_PcmGetEcRefLeftChanPtr()  (g_stVoicePcmBuff.asEcRefBuffLeft)
#define VOICE_PcmGetEcRefRightChanPtr() (g_stVoicePcmBuff.asEcRefBuffRight)

#if (VOS_CPU_TYPE == VOS_HIFI)
/*PC VOICE 上行环形buffer控制信息 ，HIFI通过地址映射实现uncache的访问*/
#define CODEC_PC_VOICE_OM_CODEC_ADDR        \
                (UCOM_GetUncachedAddr(g_stVoicePcVoiceObj.uwRingBuffBaseAddr))

/*PC VOICE 下行环形buffer控制信息 ，HIFI通过地址映射实现uncache的访问*/
#define CODEC_PC_VOICE_CODEC_OM_ADDR        \
                (UCOM_GetUncachedAddr(g_stVoicePcVoiceObj.uwRingBuffBaseAddr))
#endif

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* 3.1 乒乓缓存枚举 */
typedef enum
{
    VOICE_PCM_BUFF_A = 0,
    VOICE_PCM_BUFF_B,
    VOICE_PCM_BUFF_BUTT
} VOICE_PCM_BUFF_ENUM;

/* 3.2 搬运到上下行枚举 */
typedef enum
{
    VOICE_PCM_PLAY_TX = 0,
    VOICE_PCM_PLAY_RX,
    VOICE_PCM_PLAY_TXRX,
    VOICE_PCM_PLAY_BUTT
} VOICE_PCM_PLAY_ENUM;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 消息头定义
*****************************************************************************/

/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

/* 7.1 PCM码流buffer结构体 */
typedef struct
{
    VOS_INT16                           asMicInBuffA[CODEC_PCM_MAX_FRAME_LENGTH * 2];
    VOS_INT16                           asMicInBuffB[CODEC_PCM_MAX_FRAME_LENGTH * 2];
    VOS_INT16                           asMicInBuffLeft[CODEC_PCM_MAX_FRAME_LENGTH];
    VOS_INT16                           asMicInBuffRight[CODEC_PCM_MAX_FRAME_LENGTH];
    VOS_INT16                           asSpkOutBuffA[CODEC_PCM_MAX_FRAME_LENGTH * 2];
    VOS_INT16                           asSpkOutBuffB[CODEC_PCM_MAX_FRAME_LENGTH * 2];
    VOS_INT16                           asSpkOutBuffTemp[CODEC_PCM_MAX_FRAME_LENGTH];
    VOS_INT16                           asEcRefBuffA[CODEC_PCM_MAX_FRAME_LENGTH * 2];
    VOS_INT16                           asEcRefBuffB[CODEC_PCM_MAX_FRAME_LENGTH * 2];
    VOS_INT16                           asEcRefBuffLeft[CODEC_PCM_MAX_FRAME_LENGTH];
    VOS_INT16                           asEcRefBuffRight[CODEC_PCM_MAX_FRAME_LENGTH];
} VOICE_PCM_BUFFER_STRU;

/* ring buffer状态结构体*/
typedef struct
{
    VOS_UINT16                          uhwAdpBufferSize;                       /*自适应缓冲大小，单位：帧*/
    VOS_UINT16                          uhwCntDataSizeIsOne;                    /*缓冲区数据大小等于1的次数*/
    VOS_UINT16                          uhwCurrDataSize;                        /*当前缓冲区数据大小，单位：帧*/
    VOS_INT16                           shwInsertFrameCnt;                      /*插帧数*/
    VOS_UINT16                          uhwFrameCnt;                            /*帧数计数器*/
    VOS_UINT16                          uhwReserved;
}VOICE_PCM_RING_BUFFER_STATE_STRU;

/* PC Voice对象结构体*/
typedef struct
{
    VOS_UINT32                          uwRingBuffBaseAddr;
    VOICE_PCM_RING_BUFFER_STATE_STRU    stTxRingBufferState;                    /*上行ring buffer状态结构体*/
    VOICE_PCM_RING_BUFFER_STATE_STRU    stRxRingBufferState;                    /*下行ring buffer状态结构体*/
}VOICE_PCM_PC_VOICE_OBJ_STRU;
/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOS_INT16                       *g_psVoicePcmMicIn;                    /*指向当前Mic输入的PCM缓存Buffer的指针*/
extern VOS_INT16                       *g_psVoicePcmSpkOut;                   /*指向当前输出到Speaker的PCM缓存Buffer的指针*/
extern VOS_INT16                       *g_psVoiceSmartPaPtr;
extern VOS_INT16                       *g_psVoiceEcRef;
extern VOS_INT16                       *g_psVoiceSpaDes;

/* DMA通道配置参数结构体全局变量 */
extern DRV_DMA_CXCFG_STRU               g_astVoicePcmMicInDmaPara[2];
extern DRV_DMA_CXCFG_STRU               g_astVoicePcmSpkOutDmaPara[2];
extern DRV_DMA_CXCFG_STRU               g_astVoiceEcRefInDmaPara[2];
extern VOICE_PCM_BUFFER_STRU            g_stVoicePcmBuff;                     /*PCM码流结构体全局变量*/

extern VOICE_PCM_PC_VOICE_OBJ_STRU      g_stVoicePcVoiceObj;                  /*PC Voice实体全局变量 */

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID VOICE_EcRefDmaIsr(
                       DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                       VOS_UINT32 uwPara);
extern VOS_VOID VOICE_MicDmaIsr(
                       DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                       VOS_UINT32 uwPara);
extern VOS_VOID VOICE_PcmAdjustRingBufSize(
                       OMMED_RING_BUFFER_CONTROL_STRU      *pstTxRingBufferControl,
                       VOICE_PCM_RING_BUFFER_STATE_STRU    *pstTxRingBufferState);
extern VOS_VOID VOICE_PcmClrLastSpkBuff(VOS_VOID);
extern VOS_INT16* VOICE_PcmGetWritableSpkBuffPtr(VOS_VOID);
extern VOS_UINT32 VOICE_PcmHybrid2Stereo(VOS_INT16 *pshwI2S, VOS_INT16 *pshwLeft, VOS_INT16 *pshwRight, VOS_INT16 shwPcmLen);
extern VOS_VOID VOICE_PcmInit(VOS_VOID);
extern VOS_VOID VOICE_PcmInsertCn(
                       VOS_INT16   *pshwIn,
                       VOS_INT16    shwLen,
                       VOS_INT16    shwPcmRandRange);
extern VOS_VOID VOICE_PcmManageRingBuf(
                       VOS_UINT16  uhwAdpBufferSize,
                       VOS_UINT16  uhwCurrDataSize,
                       VOS_INT16  *pshwBufProcess);
extern VOS_VOID VOICE_PcmMicInNoData(VOS_VOID);
extern VOS_UINT32 VOICE_PcmMicInStartLoopDMA(VOS_UINT16 usChNum, DRV_DMA_INT_FUNC pfuncIsr);
extern VOS_UINT32 VOICE_PcmSpkOutStartLoopDMA(VOS_UINT16 usChNum, DRV_DMA_INT_FUNC pfuncIsr);
extern VOS_UINT32 VOICE_PcmStartDMAFromDataIn(VOS_UINT16 usChNum);
extern VOS_UINT32 VOICE_PcmStartDMAToDataOut(VOS_UINT16 usChNum);
extern VOS_UINT32 VOICE_PcmStereo2Hybrid(
                       VOS_INT16  *pshwSpkOutTemp,
                       VOS_INT16  *pshwSpkOut,
                       VOS_INT16   shwPcmLen);
extern VOS_VOID VOICE_PcmSwitchMicBuff(VOS_VOID);
extern VOS_VOID VOICE_PcmSwitchSpkBuff(VOS_VOID);
extern VOS_UINT32 VOICE_PcmUpdate(
                       VOS_UINT32                      swSampleRate,
                       VCVOICE_DEV_MODE_ENUM_UINT16    enDevMode);
extern VOS_VOID VOICE_PcmUpdateRxRingBuf(VOS_VOID);
extern VOS_VOID VOICE_PcmUpdateTxRingBuf(VOS_VOID);
extern VOS_VOID VOICE_PcmVcLoopMicIsr(DRV_DMA_INT_TYPE_ENUM_UINT16 uhwPara, VOS_UINT32 uwIntType);
extern VOS_VOID VOICE_PcmVcLoopSpkIsr(DRV_DMA_INT_TYPE_ENUM_UINT16 uhwPara, VOS_UINT32 uwIntType);
extern VOS_VOID VOICE_SioIsr(DRV_SIO_INT_MASK_ENUM_UINT32 enIntType, VOS_UINT32 uwPara);
extern VOS_VOID VOICE_SpkDmaIsr(
                       DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                       VOS_UINT32 uwPara);





#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of med_pcm.h */
