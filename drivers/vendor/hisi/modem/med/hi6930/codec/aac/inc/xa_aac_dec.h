/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : xa_aac_dec.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2012年12月20日
  最近修改   :
  功能描述   : xa_aac_dec.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   :
    作    者   :
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "ucom_comm.h"
#include  "xa_aac_dec_api.h"
#include  "xa_comm.h"
#ifndef __XA_AAC_DEC_H__
#define __XA_AAC_DEC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define XA_AACDEC_SET_MONO2STEREO_ENABLE  (1)
#define XA_AACDEC_SET_PCMWIDTH_16         (16)
#define XA_AACDEC_SET_MAXOUTCHAN_2        (2)

#define XA_AACDEC_MAXCHAN_NUMBER          (8)
#define XA_AACDEC_CHANNELMAP_SHIFT_SCALE  (4)

#define XA_AACDEC_SBR_TYPE_1              (1)
#define XA_AACDEC_SBR_TYPE_2              (2)

#define XA_AACDEC_GetMemCfgPtr()          (&g_stAacDecMemCfg)
#define XA_AACDEC_GetParaCfgPtr()         (&g_stAacDecParamCfg)
#define XA_AACDEC_GetParamsPtr()          (&g_stAacParams)

#define XA_AACDEC_GetApiObjPtr()          (g_pstAacApiObj)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : AAC输入码流编码格式，常见的有ADIF和ADTS
 功能描述  : AAC输入码流编码格式，Tensilica Lib库只支持部分格式
*****************************************************************************/
enum
{
    XA_AACDEC_EBITSTREAM_TYPE_AAC_UNKNOW = 0,
    XA_AACDEC_EBITSTREAM_TYPE_AAC_ADIF,
    XA_AACDEC_EBITSTREAM_TYPE_AAC_ADTS,
    XA_AACDEC_EBITSTREAM_TYPE_AAC_LATM,                                         /* Tensilica Lib Not supported */
    XA_AACDEC_EBITSTREAM_TYPE_AAC_LATM_OUTOFBAND_CONFIG,                        /* Tensilica Lib Not supported */
    XA_AACDEC_EBITSTREAM_TYPE_AAC_LOAS,                                         /* Supported if library is built with LOAS_SUPPORT=1 */
    XA_AACDEC_EBITSTREAM_TYPE_AAC_RAW,
    XA_AACDEC_EBITSTREAM_TYPE_DABPLUS_RAW_SIDEINFO,                             /* Supported only if short side-info
                                                                                (format: Bit15 = badFramIndicator & Bit 0-14 = payloadSizeInBytes)
                                                                                is provided as config parameter for every raw-input frame */
    XA_AACDEC_EBITSTREAM_TYPE_DABPLUS,
    XA_AACDEC_EBITSTREAM_TYPE_BUTT
};
typedef VOS_INT16 XA_AACDEC_EBITSTREAM_TYPE_INT16;

/*****************************************************************************
 实体名称  : AAC格式
 功能描述  : AAC LIB的版本格式信息
*****************************************************************************/
enum
{
    XA_AACDEC_EBITSTREAM_FORMAT_PLAIN_AAC = 0,
    XA_AACDEC_EBITSTREAM_FORMAT_AAC_PLUS,
    XA_AACDEC_EBITSTREAM_FORMAT_AAC_PLUS_V2,
    XA_AACDEC_EBITSTREAM_FORMAT_BUTT
};
typedef VOS_INT16 XA_AACDEC_FORMAT_TYPE_INT16;

/*****************************************************************************
 实体名称  : 输入码流立体声格式
 功能描述  : 确认输入码流的立体声格式
*****************************************************************************/
enum
{
    XA_AACDEC_CHANNELMODE_MONO = 0,
    XA_AACDEC_CHANNELMODE_STEREO,
    XA_AACDEC_CHANNELMODE_DUAL_CHANNEL,
    XA_AACDEC_CHANNELMODE_PARAMETRIC_STEREO,
    XA_AACDEC_CHANNELMODE_3_CHANNEL_FRONT,
    XA_AACDEC_CHANNELMODE_3_CHANNEL_SURR,
    XA_AACDEC_CHANNELMODE_4_CHANNEL_1SURR,
    XA_AACDEC_CHANNELMODE_4_CHANNEL_2SURR,
    XA_AACDEC_CHANNELMODE_6_CHANNEL,
    XA_AACDEC_CHANNELMODE_7_CHANNEL,
    XA_AACDEC_CHANNELMODE_5_CHANNEL,
    XA_AACDEC_CHANNELMODE_2_1_STEREO,
    XA_AACDEC_CHANNELMODE_3_1_CHANNEL_FRONT,
    XA_AACDEC_CHANNELMODE_3_1_CHANNEL_SURR,
    XA_AACDEC_CHANNELMODE_4_1_CHANNEL_1SURR,
    XA_AACDEC_CHANNELMODE_4_1_CHANNEL_2SURR,
    XA_AACDEC_CHANNELMODE_6_1_CHANNEL,
    XA_AACDEC_CHANNELMODE_5_1_CHANNEL,
    XA_AACDEC_CHANNELMODE_7_1_CHANNEL,
    XA_AACDEC_CHANNELMODE_BUTT
};
typedef VOS_INT16 XA_AACDEC_CHANNELMODE_INT16;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : AAC_DEC_CONFIG_PARAMS_STRU
 功能描述  : AAC解码输出码流配置参数结构体
*****************************************************************************/
typedef struct
{
    VOS_INT32 swPcmWidthSize;      /* 数据位宽 */
    VOS_INT32 swMaxOutChanNum;     /* 输出最大声道数 */
    VOS_INT32 swForceOutputStereo; /* 单声道强制输出双声道 */
}AAC_DEC_CONFIG_PARAMS_STRU;

/*****************************************************************************
 实体名称  : AAC_DEC_PARAMS_STRU
 功能描述  : AAC解码输入码流参数结构体
*****************************************************************************/
typedef struct
{
    XA_AACDEC_FORMAT_TYPE_INT16       enAacLibFormat;/* AAC库格式 */
    XA_AACDEC_EBITSTREAM_TYPE_INT16   enAacFormat;   /* AAC编码格式 */
    VOS_INT32                         swNumChannels; /* 声道数 */
    XA_AACDEC_CHANNELMODE_INT16       enAcmod;       /* 立体声模式 */
    VOS_INT16                         shwReserved;
    VOS_INT32                         swSampFreq;    /* 采样率 */
    VOS_INT32                         swSbrType;     /* SBR类型 */
    VOS_INT32                         swAacRate;     /* AAC编码速率 */
    VOS_INT32                         swChanMap;     /* 通道映射表 */
    VOS_INT32                         swOutNChans;   /* 解码允许的最大声道数 */
}AAC_DEC_PARAMS_STRU;


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern XA_COMM_MEMORY_CFG_STRU                   g_stAacDecMemCfg;       /* AAC解码内存配置管理 */
extern AAC_DEC_CONFIG_PARAMS_STRU                g_stAacDecParamCfg;     /* AAC解码参数配置管理 */
extern AAC_DEC_PARAMS_STRU                       g_stAacParams;          /* AAC编解码参数 */

/* AAC解码库API指针 */
extern xa_codec_handle_t                         g_pstAacApiObj;


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID AAC_DECODER_Close(VOS_VOID);
extern VOS_UINT32 AAC_DECODER_Decode(XA_AUDIO_DEC_USER_CFG_STRU *pstUserCfg);
extern VOS_INT32 AAC_DECODER_GetCurrDecPos(VOS_UINT32 *puwPosInBytes);
extern VOS_UINT32  AAC_DECODER_GetParams(AAC_DEC_PARAMS_STRU *pstConfig);
extern VOS_UINT32  AAC_DECODER_Init(XA_AUDIO_DEC_USER_CFG_STRU *pstUserCfg);
extern VOS_UINT32 AAC_DECODER_PcmDataConvert
                       (VOS_INT32       *pswTimeCh,
                        VOS_INT32        swPcmSampleSize,
                        VOS_INT32        swOutputSize,       /* AUDIO_DEC_MEMORY_CFG_STRU中的临时输出Buff的长度 */
                        VOS_INT16       *pshwOutBuff,
                        VOS_INT32       *pswTotalOutSize);
extern VOS_UINT32  AAC_DECODER_Seek(
                       VOS_UINT32                    uwSkipSize,
                       CODEC_SEEK_DERECT_ENUM_UINT16 enSeekDirect);
extern VOS_UINT32  AAC_DECODER_SetParams(AAC_DEC_CONFIG_PARAMS_STRU *pstParamCfg);
//extern VOS_VOID AAC_DECODER_SetOutChanFixedTo2(VOS_INT32 *pswChanMap);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of xa_aac_dec.h */


