
#ifndef __AUDIO_PCM_H__
#define __AUDIO_PCM_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "audio.h"
#include  "vos.h"
#include  "med_drv_dma.h"
#include  "med_drv_sio.h"
#include  "hifidrvinterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

typedef VOS_VOID (*AUDIO_PCM_CALLBACK)(VOS_VOID *pshwBufOut,    \
                                         VOS_VOID *pshwBufIn,   \
                                         VOS_UINT32 uwSampleRate,\
                                         VOS_UINT32 uwChannelNum,\
                                         VOS_UINT32 uwBufSize);

#define AUDIO_PCM_DMAC_CHN_CAPTURE      (2)                                    /* 音频输入DMA通道，使用AP侧EDMA，通道号待确定 */
#define AUDIO_PCM_DMAC_CHN_PLAY         (1)                                    /* 音频输出DMA通道，使用AP侧EDMA，通道号待确定 */

#define AUDIO_PCM_PINGPONG_BUF_NUM      (2)                                     /* 音频乒乓缓冲的个数，播放/录音个数相同 */
#define AUDIO_PCM_MAX_CHANNEL_NUM       (2)
#define AUDIO_PCM_MAX_SAMPLE_RATE       (192000)
#define AUDIO_PCM_MAX_FRAME_LENGTH      (0x780)                                /* 采样率为192K时20ms的双通道数据，单位16bit */
#define AUDIO_PCM_FORMAT_16_SIZE        (2)                                     /* 16bit采样点所占用的大小，单位Byte */
#define AUDIO_PCM_FRAME_PER_SEC         (50)                                    /* 每秒输入输出的Frame个数，50表示每帧20ms */

#define AUDIO_PcmGetPcmObjPtr()         (&g_stAudioPcmObj)
#define AUDIO_PcmGetPcmPbBufPtr()       (&g_stAudioPcmPbBuff)
#define AUDIO_PcmGetPlayTmpBufPtr()     (g_stAudioPcmTmpBuff.shwPlayTmpBuff)
#define AUDIO_PcmGetCaptureTmpBufPtr()  (g_stAudioPcmTmpBuff.shwCaptureTmpBuff)
#define AUDIO_PcmGetPlayBufAPtr()       (g_stAudioPcmPbBuff.shwBuffA)
#define AUDIO_PcmGetPlayBufBPtr()       (g_stAudioPcmPbBuff.shwBuffB)

#define AUDIO_PcmGetPcmCpBufPtr()       (&g_stAudioPcmCpBuff)
#define AUDIO_PcmGetCaptureBufAPtr()    (g_stAudioPcmCpBuff.shwBuffA)
#define AUDIO_PcmGetCaptureBufBPtr()    (g_stAudioPcmCpBuff.shwBuffB)

#define AUDIO_PcmGetPlayDmaCfgPtr()     (&g_astAudioPlayDmaPara[0])
#define AUDIO_PcmGetCaptureDmaCfgPtr()  (&g_astAudioCaptureDmaPara[0])
#define AUDIO_PcmGetPlayDmaCfgAPtr()    (&g_astAudioPlayDmaPara[0])
#define AUDIO_PcmGetPlayDmaCfgBPtr()    (&g_astAudioPlayDmaPara[1])
#define AUDIO_PcmGetCaptureDmaCfgAPtr() (&g_astAudioCaptureDmaPara[0])
#define AUDIO_PcmGetCaptureDmaCfgBPtr() (&g_astAudioCaptureDmaPara[1])

#define AUDIO_PcmGetState()             (g_stAudioPcmObj.enPcmStatus)
#define AUDIO_PcmGetSpeState(uhwMode)   (g_stAudioPcmObj.astPcmCfg[uhwMode].enPcmStatus)
#define AUDIO_PcmGetPcmCfgPtr(uhwMode)  (&g_stAudioPcmObj.astPcmCfg[uhwMode])

#define AUDIO_PcmSetState(uhwStatus)    (g_stAudioPcmObj.enPcmStatus = (uhwStatus))        /* 设置语音软件运行状态 */
#define AUDIO_PcmSetSpeState(uhwMode, uhwStatus)  \
                                        (g_stAudioPcmObj.astPcmCfg[uhwMode].enPcmStatus = (uhwStatus))

#ifndef AUDIO_IO_SLIMBUS
#define AUDIO_PCM_IO_Open(enIntMask,uwSampleRate,uhwIsAudioMaster,pfIntHandle,uwPara) \
            DRV_SIO_Audio_Open(enIntMask,uwSampleRate,uhwIsAudioMaster,pfIntHandle,uwPara)
#define AUDIO_PCM_IO_Close()            DRV_SIO_Audio_Close()
#else
#define AUDIO_PCM_IO_Open(enIntMask,uwSampleRate,pfIntHandle,uwPara)
#define AUDIO_PCM_IO_Close()            DRV_SLIMBUS_Audio_Close()
#endif

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : AUDIO_PCM_BUFF_ISREADY_ENUM
 功能描述  : ap或hifi发送的buff是否ready标志位
*****************************************************************************/
enum AUDIO_PCM_BUFF_ISREADY_ENUM
{
    AUDIO_PCM_BUFF_ISREADY_NO = 0,
    AUDIO_PCM_BUFF_ISREADY_YES,
    AUDIO_PCM_BUFF_ISREADY_BUT
};
typedef VOS_UINT16  AUDIO_PCM_BUFF_ISREADY_ENUM_UINT16;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : AUDIO_PCM_UPDATE_PLAY_BUFF_CMD_STRU
 功能描述  : 该消息更新Hifi的播放Buff信息，表示一帧数据已经消费完毕
*****************************************************************************/
 typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          uhwMsgId;           /*_H2ASN_Skip */
    VOS_UINT16                          uhwReserve;
} AUDIO_PCM_UPDATE_PLAY_BUFF_CMD_STRU;

/*****************************************************************************
 实体名称  : AUDIO_PCM_UPDATE_CAPTURE_BUFF_CMD_STRU
 功能描述  : 该消息更新Hifi的录音Buff信息，表示一帧数据已经录制完毕
*****************************************************************************/
 typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          uhwMsgId;           /*_H2ASN_Skip */
    VOS_UINT16                          uhwReserve;
} AUDIO_PCM_UPDATE_CAPTURE_BUFF_CMD_STRU;

/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : AUDIO_PCM_PINGPONG_BUF_STRU
 功能描述  : 音频输入输出乒乓BUFF结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwBuffA[AUDIO_PCM_MAX_FRAME_LENGTH];
    VOS_INT16                           shwBuffB[AUDIO_PCM_MAX_FRAME_LENGTH];
}AUDIO_PCM_PINGPONG_BUF_STRU;

typedef struct
{
    VOS_INT16                           shwPlayTmpBuff[AUDIO_PCM_MAX_FRAME_LENGTH];
    VOS_INT16                           shwCaptureTmpBuff[AUDIO_PCM_MAX_FRAME_LENGTH];
}AUDIO_PCM_TMP_BUF_STRU;
/*****************************************************************************
 实体名称  : AUDIO_PCM_BUF_STRU
 功能描述  : 音频输入输出通道状态结构体
*****************************************************************************/
 typedef struct
{
    AUDIO_PCM_STATUS_ENUM_UINT16        enBufStatus;        /* 缓冲状态，主要是是否使用该Buf进行输入输出 */
    AUDIO_PCM_BUFF_ISREADY_ENUM_UINT16  enBufReadyFlag;     /* ap或hifi发送的buff是否ready标志位 */
    VOS_UINT32                          uwBufAddr;          /* 缓存的首地址 */
    AUDIO_PCM_CALLBACK                  pfunCallBack;       /* 通道处理的回调函数 */
    AUDIO_PCM_CALLBACK                  pfunVcRecordCB;     /* 通话录音的回调函数，仅通话录音时有效 */
    VOS_VOID                           *pSubStream;         /* 当前Buff对应的音频子流编号,仅AP侧有效 */
}AUDIO_PCM_BUF_STRU;

/*****************************************************************************
 实体名称  : AUDIO_PCM_CFG_STRU
 功能描述  : 音频输入输出通道状态结构体
*****************************************************************************/
 typedef struct
{
    AUDIO_PCM_STATUS_ENUM_UINT16        enPcmStatus;        /* 音频通路状态 */
    AUDIO_CAPTURE_MODE_ENUM_UINT16      enCaptureMode;      /* 录音模式，仅在录音通道有效 */
    VOS_UINT32                          uwSampleRate;       /* 当前通道采样率 */
    VOS_UINT32                          uwChannelNum;       /* 通道的声道数 */
    VOS_UINT32                          uwBufSize;          /* 缓冲区的大小，单位byte,默认为20ms的数据量 */
    AUDIO_PCM_BUF_STRU                  astPcmBuf[AUDIO_PCM_OBJ_BUT];
                                                            /* AP和Hifi提供的BUF信息，按AUDIO_PCM_OBJ_ENUM顺序排列 */
}AUDIO_PCM_CFG_STRU;

/*****************************************************************************
 实体名称  : AUDIO_PCM_CTRL_OBJ_STRU
 功能描述  : 音频通路整体控制结构体
*****************************************************************************/
 typedef struct
{
    AUDIO_PCM_STATUS_ENUM_UINT16        enPcmStatus;        /* 音频通路任务状态 */
    VOS_UINT16                          uhwReserve;
    AUDIO_PCM_CFG_STRU                  astPcmCfg[AUDIO_PCM_MODE_BUT];
                                                            /* 当前播放、录音的参数，按AUDIO_PCM_MODE_ENUM顺序排列 */
}AUDIO_PCM_CTRL_OBJ_STRU;

/*****************************************************************************
 结构名    : AUDIO_PCM_ENHANCE_START_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 启动音频增强处理消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          uhwMsgId;           /*_H2ASN_Skip */
    VOS_UINT16                          uhwReserve;
    VOS_UINT32                          uwSampleRate;
    AUDIO_ENHANCE_MODULE_ENUM_UINT32    enModule;           /* 需要启动的模块 */
}AUDIO_PCM_ENHANCE_START_IND_STRU;

/*****************************************************************************
 结构名    : AUDIO_PCM_ENHANCE_STOP_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 停止音频增强处理消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          uhwMsgId;           /*_H2ASN_Skip */
    VOS_UINT16                          uhwReserve;
    AUDIO_ENHANCE_MODULE_ENUM_UINT32    enModule;           /* 需要启动的模块 */
}AUDIO_PCM_ENHANCE_STOP_IND_STRU;

/*****************************************************************************
 结构名    : AUDIO_PCM_ENHANCE_SET_DEVICE_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 设置设备模式
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          uhwMsgId;           /*_H2ASN_Skip */
    VOS_UINT16                          uhwReserve;
    AUDIO_VOIP_DEVICE_ENUM_UINT32       uwDevice;
}AUDIO_PCM_ENHANCE_SET_DEVICE_IND_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern AUDIO_PCM_CTRL_OBJ_STRU                 g_stAudioPcmObj;

/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID AUDIO_PCM_CaptureDmaIsr(
                       DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                       VOS_UINT32 uwPara);
extern VOS_INT16* AUDIO_PCM_GetCurCaptureBuff(VOS_VOID);
extern VOS_INT16* AUDIO_PCM_GetCurPlayBuff(VOS_VOID);
extern VOS_VOID AUDIO_PCM_Init(VOS_VOID);
extern VOS_UINT32 AUDIO_PCM_MsgPcmCloseReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PCM_MsgPcmHwFreeReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PCM_MsgPcmHwParaReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PCM_MsgPcmOpenReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PCM_MsgPcmPointerReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PCM_MsgPcmPrepareReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PCM_MsgPcmSetBufCmd(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PCM_MsgPcmTriggerReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PCM_MsgUpdateCaptureBuffCmd(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PCM_MsgUpdatePlayBuffCmd(VOS_VOID *pvOsaMsg);
extern VOS_VOID AUDIO_PCM_PlayDmaIsr(
                       DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                       VOS_UINT32 uwPara);
extern VOS_UINT32 AUDIO_PCM_RegisterPcmCbFunc( AUDIO_PCM_MODE_ENUM_UINT16 enPcmMode,
                                                           AUDIO_PCM_OBJ_ENUM_UINT16 enPcmObj,
                                                           AUDIO_CAPTURE_MODE_ENUM_UINT16  enCaptureMode,
                                                           AUDIO_PCM_CALLBACK pfunCallBack);
extern VOS_UINT32 AUDIO_PCM_StartCaptureLoopDMA(VOS_UINT16 uhwLen, VOS_UINT16 usChNum);
extern VOS_UINT32 AUDIO_PCM_StartPlayLoopDMA(VOS_UINT16 uhwLen, VOS_UINT16 usChNum);
extern VOS_UINT32 AUDIO_PCM_UnRegisterPcmCbFunc(AUDIO_PCM_MODE_ENUM_UINT16 enPcmMode,
                                                             AUDIO_PCM_OBJ_ENUM_UINT16 enPcmObj);








#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of audio_pcm.h */

