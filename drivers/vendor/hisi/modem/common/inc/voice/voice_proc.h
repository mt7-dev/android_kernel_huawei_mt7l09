/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : med_vs_proc.h
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2008年5月28日
  最近修改   :
  功能描述   : med_vs_proc.h 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2010年4月8日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/
#ifndef __VOICE_PROC_H__
#define __VOICE_PROC_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_com_codec.h"
#include "voice_pcm.h"
#include "PhyCodecInterface.h"
#include "TdphyCodecInterface.h"
#include "VcCodecInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
缩略语说明:

COMP    compensate filter   补偿滤波器
TXRX    Tx and Rx item      上下行选项
PROC    Pre & post process  语音前后处理
HPF     High pass filter    高通滤波器
SND     sound generator     sound序列合成器

*****************************************************************************/

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 回调函数定义 */
typedef void (*VOICE_PROC_SONDIND_CALLBACK)(void);

/* 检查并返回 */
#define VOICE_ProcCheckAndRet(usRet);\
    if ((usRet) != UCOM_RET_SUCC)   \
    {                               \
        return (usRet);             \
    }

#define VOICE_PROC_HOMING_MASK      (0x0008)
#define VOICE_RX_LOST_FRAME_THD     (3)                                     /*下行相对上行丢帧检测帧数差别阈值*/
#define VOICE_PROC_GSM_RX_FACCH_THD (2)                                     /*GSM下连续FA帧插舒适噪声阈值*/
#define VOICE_PROC_GAIN_MAX         (12)                                    /* 最大侧音增益 */
#define VOICE_PROC_GAIN_MIN         (-80)
#define VOICE_PROC_BFI_MAX          (2)                                     /* 最多处理连续2帧BFI */

/* 获取语音处理对象属性 */
#define VOICE_ProcGetObjsPtr()           (&g_stVoiceProcObjs)

#define VOICE_ProcGetCodecPtr()      (g_stVoiceProcObjs.pstCodec)
#define VOICE_ProcGetEncBufPtr()     (&g_stVoiceCodecData.asEncSerial[0])
#define VOICE_ProcGetDecBufPtr()     (&g_stVoiceCodecData.astDecSerial[0])
#define VOICE_ProcGetLineInBufPtr()  (&g_stVoiceCodecData.asLineInBuff[0])
#define VOICE_ProcGetLineOutBufPtr() (&g_stVoiceCodecData.asLineOutBuff[0])
#define VOICE_ProcGetPreDecInObj()   (&g_stVoiceProcObjs.pstCodec->stPreDecInObj)
#define VOICE_ProcGetPrevDecDataPtr() (&g_stVoiceCodecData.asDecPrevData[0])

#define VOICE_GetRxLostFrameCnt()    (g_stVoiceProcObjs.pstCodec->uhwRxLostFrmCnt)              /* 获取丢帧计数值 */
#define VOICE_SetRxLostFrameCnt(uhwVar) \
                                     (g_stVoiceProcObjs.pstCodec->uhwRxLostFrmCnt = (uhwVar))   /* 设置丢帧计数值 */
#define VOICE_AddRxLostFrameCnt(uhwVar) \
                                     (g_stVoiceProcObjs.pstCodec->uhwRxLostFrmCnt += (uhwVar))  /* 丢帧检测计数值累加计数 */
#define VOICE_ProcFacchFrameCntAdd() (g_stVoiceProcObjs.pstCodec->stDecInObj.uhwFrameFacchCnt++)   /* GSM连续FA帧计数加1 */
#define VOICE_ProcFacchFrameCntReset()  \
                                     (g_stVoiceProcObjs.pstCodec->stDecInObj.uhwFrameFacchCnt = 0) /* GSM连续FA帧计数清0 */
#define VOICE_ProcGetFacchFrameCnt() (g_stVoiceProcObjs.pstCodec->stDecInObj.uhwFrameFacchCnt)     /* 获取GSM下连续的FA帧计数 */
#define VOICE_ProcSetMaxFrameRange(uhwRange)  \
                                     (g_stVoiceProcObjs.pstCodec->stDecInObj.shwFrameRangeMax = uhwRange) /* 存储幅度绝对值最大值 */
#define VOICE_ProcGetMaxFrameRange() (g_stVoiceProcObjs.pstCodec->stDecInObj.shwFrameRangeMax)     /* 获取上一帧的幅度绝对值最大值 */



/* 管理存储解码数据的写指针 */
#define VOICE_ProcGetDecSerialWritePtr()  (g_pstDecSerialWrite)                    /* 获取存储解码数据的地址*/
#define VOICE_ProcInitDecSerialWritePtr() (g_pstDecSerialWrite = &g_stVoiceCodecData.astDecSerial[0])  /* 初始化解码数据写指针*/
#define VOICE_ProcAddDecSerialWritePtr()  (g_pstDecSerialWrite += 1)            /* 指向下一个解码数据存储单元 */
#define VOICE_ProcSubDecSerialWritePtr()  (g_pstDecSerialWrite -= 1)            /* 指向上一个解码数据存储单元*/

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* 是否 */
enum VOICE_PROC_IS_ENUM
{
    VOICE_PROC_IS_NO                 = 0,
    VOICE_PROC_IS_YES                = 1,
    VOICE_PROC_IS_BUTT
};
typedef VOS_UINT16 VOICE_PROC_IS_ENUM_UINT16;

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

/* 7.1 对外数据结构 */
/* 7.1.1 SOUND合成器配置参数 */
typedef struct
{
    VOS_INT16                           sTxVolume;                              /*上行Snd播放音量:-80~12dB*/
    VOS_INT16                           sRxVolume;                              /*下行Snd播放音量:-80~12dB*/
    VOS_UINT16                          enTxRx;                                 /*SOUND播放目标通道, VOICE_PROC_TXRX_ENUM*/
    VOS_UINT16                          usSndId;                                /*Snd ID*/
    VOS_UINT16                          usRptCnt;                               /*重复播放次数:0 表示无限循环*/
    VOS_UINT16                          usReserve;
    VOICE_PROC_SONDIND_CALLBACK         funcSndInd;                             /*SOUND播放结束上报回调函数*/
} VOICE_PROC_SND_CFG_STRU;


/* 7.2.10 Codec对象 */
typedef struct
{
    VOS_UINT16                          enCodecType;                            /*编解码类型*/
    VOS_UINT16                          usIsEncInited;                          /*编码器是否已初始化*/
    VOS_UINT16                          usIsDecInited;                          /*解码器是否已初始化*/
    VOS_UINT16                          uhwRxLostFrmCnt;                        /*解码器丢帧计数*/
    CODEC_ENC_IN_PARA_STRU              stEncInObj;                             /*编码输入结构*/
    CODEC_ENC_OUT_PARA_STRU             stEncOutObj;                            /*编码输出结构*/
    CODEC_DEC_IN_PARA_STRU              stDecInObj;                             /*解码输入结构(编码输出仅为PCM码流,因此不在此描述)*/
    CODEC_DEC_PREIN_PARA_STRU           stPreDecInObj;                          /*前一帧解码输入结构(编码输出仅为PCM码流,因此不在此描述)*/
} VOICE_PROC_CODEC_STRU;

/* 7.2.11 SOUND合成器 */
typedef struct
{
    VOS_UINT16                          enTxEnable;                             /*snd上行使能*/
    VOS_UINT16                          enRxEnable;                             /*snd下行使能*/
    VOS_INT16                           sSndTxVol;                              /*Snd上行音量:-80~12*/
    VOS_INT16                           sSndRxVol;                              /*Snd下行音量:-80~12*/
    VOS_UINT16                          usPreDisable;
    VOS_UINT16                          usReserve;
    VOICE_PROC_SONDIND_CALLBACK         funcSndInd;                             /* _H2ASN_Replace VOS_UINT32 funcSndInd; *///Snd播放结束上报回调函数
} VOICE_PROC_SOUND_STRU;

/* 7.2.12 语音处理对象集 */
typedef struct
{
    VOS_UINT16                          enPpMicNum;                             /* PP处理麦克个数配置:单双麦克 */
    VOS_UINT16                          usReserve;
    VOICE_PROC_CODEC_STRU              *pstCodec;                               /* MC传入的codec指针 */
} VOICE_PROC_OBJS_STRU;

typedef struct
{
    VOS_INT16                           asDecSerial[VOICE_CODED_FRAME_WITH_OBJ_LEN ];  /* 待解码码流 */
    VOS_UINT32                          uwTimeStamp;                                   /* 待解码码流时间戳 */
} VOICE_DEC_SERIAL_STRU;

typedef struct
{
    VOS_INT16                           asEncSerial[VOICE_CODED_FRAME_WITH_OBJ_LEN ]; /* 编码后码流 */
    VOICE_DEC_SERIAL_STRU               astDecSerial[VOICE_CODED_DECODE_BUF_FRAME_NUM]; /* 待解码码流 */
    VOS_UINT32                          uwReserve;
    VOS_INT16                           asDecPrevData[GPHY_VOICE_DATA_LEN];           /* 上一帧带解码数据 */
    VOS_INT16                           asReserved;
    VOS_INT16                           asLineInBuff[CODEC_PCM_MAX_FRAME_LENGTH];     /* 解码后缓冲区 */
    VOS_INT16                           asLineOutBuff[CODEC_PCM_MAX_FRAME_LENGTH];    /* 编码前缓冲区 */
} VOICE_PROC_CODEC_DATA_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOICE_PROC_OBJS_STRU          g_stVoiceProcObjs;
extern VOICE_PROC_CODEC_DATA_STRU    g_stVoiceCodecData;                   /*语音码流结构体全局变量*/
extern VOICE_DEC_SERIAL_STRU        *g_pstDecSerialWrite;


/*****************************************************************************
  11 FUNCTION声明
*****************************************************************************/
extern VOS_UINT32 VOICE_ProcCfgGsmDlCodec(GPHY_VOICE_RX_DATA_IND_STRU *pstRxDataMsg);
extern VOS_UINT32 VOICE_ProcCfgGsmUlCodec(GPHY_VOICE_UPDATE_PARA_CMD_STRU *pstUpdateMsg, VOS_UINT16 uhwModemNo);
extern VOS_UINT32 VOICE_ProcCfgUmtsDlCodec( VOS_VOID );
extern VOS_UINT32 VOICE_ProcCfgUmtsUlCodec( VOS_VOID );
extern VOS_UINT32 VOICE_ProcCfgDlCodec(
                        CODEC_NET_MODE_ENUM_UINT16         enNetMode,
                        VOS_INT16                         *pshwDecBuf,
                        VOS_UINT16                         uhwRxFrmLost,
                        VOS_INT16                        **pshwDecData );
extern VOS_VOID   VOICE_ProcInitDecSerialWrite( VOS_VOID );
extern VOS_VOID   VOICE_ProcAddDecSerialWrite(VOS_VOID *pvMsg, VOS_UINT32 uwLenBytes);
extern VOS_VOID   VOICE_ProcSubDecSerialWrite( VOS_UINT16 uhwRxFrmLost);

extern VOS_UINT32 VOICE_ProcDecode(VOS_INT16 *psStrm, VOS_INT16 *psPcm);
extern VOS_UINT32 VOICE_ProcDestroy(VOICE_PROC_OBJS_STRU *pstProcObj);
extern VOS_UINT32 VOICE_ProcEncode(VOS_INT16 *psPcm, VOS_INT16 *psStrm);
extern VOS_UINT32 VOICE_ProcFrameTypeToRlc(VOS_UINT16 uhwFrameTypeIdx);
extern VOS_INT16* VOICE_ProcGetEcRef( VOS_VOID );
extern VOS_UINT32 VOICE_ProcGetSampleRate(VOS_VOID);
extern VOS_UINT32 VOICE_ProcInit(VOS_VOID);
extern VOS_UINT32 VOICE_ProcInitDecoder(VOS_UINT16 usType);
extern VOS_UINT32 VOICE_ProcInitEncoder(VOS_UINT16 usType, VOS_UINT16 usDtxMode);
extern VOS_UINT32 VOICE_ProcOutputFrameType(VOS_UINT16 uhwFrameTypeIdx);
extern VOS_UINT32 VOICE_ProcRxPp(
                       VOS_INT16   *pshwLineIn,
                       VOS_INT16   *pshwSpkOut);
extern VOS_UINT32 VOICE_ProcTxPp(
                       VOS_INT16   *pshwMcIn,
                       VOS_INT16   *pshwRcIn,
                       VOS_INT16   *pshwLineOut);
extern VOS_UINT32 VOICE_ProcUpdate(VOS_UINT16 uhwDevMode,VOS_UINT16 uhwNetMode);
extern VOS_UINT32 VOICE_ProcVolume(
                       VOS_INT16               shwVolumeDb,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 VOICE_ProcFacch(VOS_INT16   *psPcm);
extern VOS_UINT32 VOICE_ProcBFI(VOICE_PROC_CODEC_STRU  *pstCodec);
extern VOS_UINT32 VOICE_ProcCfgImsCodec(
                VOS_UINT16 uhwCodecType,
                VOS_UINT16 uhwDtxEnable,
                VOS_UINT16 uhwRateMode);
extern MED_UINT16 VOICE_ProcCheckUpdate( VOS_UINT16 uhwCodecType  );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /*_VOICE_PROC_H*/

