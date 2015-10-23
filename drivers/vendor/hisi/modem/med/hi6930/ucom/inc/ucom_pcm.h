

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_com_codec.h"
#include "ucom_stub.h"
#include "med_drv_ipc.h"
#include "med_drv_dma.h"
#include "med_drv_sio.h"


#ifndef __UCOM_PCM_H__
#define __UCOM_PCM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 通用 */
/* 最大支持采样率为48k，一帧的大小为48000*0.02 */
#define UCOM_PCM_MAX_FRAME_LENGTH          (960)

/* 数据为I2S格式，即最多拥有左右2路PCM数据 */
#define UCOM_PCM_I2S_CHANNEL_NUM           (2)

/* 乒乓BUFF最大长度 */
#define UCOM_PCM_PINGPONG_LEN              (UCOM_PCM_MAX_FRAME_LENGTH * UCOM_PCM_I2S_CHANNEL_NUM)

/* 兵乓BUFF共有2路缓存 */
#define UCOM_PCM_PINGPONG_BUFF_NUM         (2)

/* 以下为SMART PA使用 */
/* V7R2中没有分配此通道 */
#define UCOM_PCM_DMAC_CHN_SPA              (VOS_NULL)
#define UCOM_PCM_GetSpaCfgPtr()            (&g_stSmartPaCfg)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/
/* 公用PCM数据控制结构体 */
typedef struct
{
    VOS_UINT16            enIsInit;             /* 是否初始化 */
    VOS_UINT16            uhwDataLenBytes;      /* 数据长度 */
    VOS_INT16            *pshwDestVirtualAddr;  /* 当前数据地址 */
}UCOM_PCM_CFG_STRU;


typedef struct
{
    VOS_INT16             ashwSmartPaBuffA[UCOM_PCM_PINGPONG_LEN];
    VOS_INT16             ashwSmartPaBuffB[UCOM_PCM_PINGPONG_LEN];
}UCOM_PCM_BUFFER_STRU;

/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern DRV_DMA_CXCFG_STRU                      g_astSmartPaInDmaPara[2];
extern UCOM_PCM_CFG_STRU                       g_stSmartPaCfg;
extern UCOM_PCM_BUFFER_STRU                    g_stUcomPcmBuff;
/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID UCOM_PCM_SmartPaPcmInit(VOS_VOID);
extern VOS_UINT32 UCOM_PCM_SmartPaStartLoopDma(
                      VOS_UINT16              uhwFrameLenBytes,
                      VOS_UINT32              uwSampleRate,
                      VOS_UINT16              uhwSmartPaMaster,
                      VOS_UINT16              usChNum,
                      DRV_DMA_INT_FUNC        pfuncIsr);
extern VOS_VOID UCOM_PCM_SmartPaStop(VOS_VOID);
extern VOS_UINT32 UCOM_PCM_SmartPaGetAvailVirtAddr(VOS_VOID);
extern VOS_UINT16 UCOM_PCM_GetSmartPaDmacChn(VOS_VOID);




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of ucom_pcm.h */
