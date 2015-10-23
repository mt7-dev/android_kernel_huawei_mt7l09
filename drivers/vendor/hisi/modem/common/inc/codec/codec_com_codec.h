
#ifndef __CODEC_COM_CODEC_H__
#define __CODEC_COM_CODEC_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_op_lib.h"
#include "CodecInterface.h"
#include "ImsCodecInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
extern VOS_INT16                        g_shwCodecPcmFrameLength;              /* PCM数据帧长度 */

#define CODEC_WB_PCM_MASK           ((VOS_INT16)0xfffC)                     /*对于宽带，16比特PCM编解码器仅取高14比特量化有效 */
#define CODEC_PCM_MASK              (0xfff8)                                /*16比特PCM编解码器仅取高13比特量化有效 */
#define CODEC_EHF_MASK              (0x0008)                                /*Homing帧编码前/解码后固定值*/
#define CODEC_FRAME_LENGTH_NB       (160)                                   /*8K采样率是每帧PCM码流的样点数*/
#define CODEC_FRAME_LENGTH_WB       (320)                                   /*16K采样率是每帧PCM码流的样点数*/
#define CODEC_FRAME_LENGTH_MU       (960)                                   /*48K采样率是每帧PCM码流的样点数*/

#define CODEC_DWORD_BITS_NUM              (32)                                    /* 4字节对应比特数 */
#define CODEC_WORD_BITS_NUM               (16)                                    /* 2字节对应比特数 */
#define CODEC_BYTE_BITS_NUM               (8)                                     /* 1字节对应比特数 */
#define BIT0_MASK                       (0x0001)
#define BIT15_MASK                      (0x8000)
#define BIT31_MASK                      (0x80000000)
#define MIN(var1,var2)                  ((var1)>(var2)?(var2):(var1))

#define CODEC_PCM_WB_FRAME_LENGTH             ((VOS_UINT16)320)              /* 宽带语音一帧PCM数据的帧长,单位采样点 */
#define CODEC_PCM_MAX_FRAME_LENGTH            (CODEC_FRAME_LENGTH_WB)    /* 一帧PCM数据的最大帧长,用于定义PCM缓冲区，单位采样点 */

#define CODEC_PcmGetPcmFrameLength()          (g_shwCodecPcmFrameLength)
#define CODEC_PcmSetPcmFrameLength(shwPcmFrameLen) (g_shwCodecPcmFrameLength = shwPcmFrameLen)
#define CODEC_PCM_FRAME_LENGTH                 CODEC_PcmGetPcmFrameLength()
#define CODEC_PCM_FRAME_LENGTH_BYTES          (CODEC_PCM_FRAME_LENGTH * (VOS_UINT16)sizeof(VOS_INT16))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : CODEC_SAMPLE_RATE_MODE_ENUM
 功能描述  : 采样率定义
*****************************************************************************/
enum CODEC_SAMPLE_RATE_MODE_ENUM
{
    CODEC_SAMPLE_RATE_MODE_8000             = 8000,
    CODEC_SAMPLE_RATE_MODE_16000            = 16000,
    CODEC_SAMPLE_RATE_MODE_BUTT
};
typedef VOS_INT32 CODEC_SAMPLE_RATE_MODE_ENUM_INT32;

/*****************************************************************************
 实体名称  : CODEC_VAD_FLAG_ENUM
 功能描述  : VAD定义
*****************************************************************************/
enum CODEC_VAD_FLAG_ENUM
{
    CODEC_VAD_FLAG_NOISE              = 0,
    CODEC_VAD_FLAG_SPEECH             = 1,
    CODEC_USED_BUTT
};
typedef VOS_INT16 CODEC_VAD_FLAG_ENUM_INT16;

/* 模块是否在使用枚举定义 */
/*****************************************************************************
 实体名称  : CODEC_OBJ_USED_STATUS_ENUM
 功能描述  : 模块是否已使用定义
*****************************************************************************/
enum CODEC_OBJ_USED_STATUS_ENUM
{
    CODEC_OBJ_UNUSED              = 0,
    CODEC_OBJ_USED,
    CODEC_OBJ_BUTT
};
typedef VOS_UINT16  CODEC_OBJ_USED_STATUS_ENUM_UINT16;

/*****************************************************************************
 实体名称  : CODEC_RET_ENUM
 功能描述  : 接口返回值定义
*****************************************************************************/
enum CODEC_RET_ENUM
{
    CODEC_RET_OK                    = 0,
    CODEC_RET_ERR                   = 1,
    CODEC_RET_BUTT
};
typedef VOS_UINT32 CODEC_RET_ENUM_UINT32;

/*****************************************************************************
 实体名称  : CODEC_AMR_MODE_ENUM
 功能描述  : AMR速率模式定义
*****************************************************************************/
enum CODEC_AMR_MODE_ENUM
{
    CODEC_AMR_MODE_475              = 0,
    CODEC_AMR_MODE_515,
    CODEC_AMR_MODE_59,
    CODEC_AMR_MODE_67,
    CODEC_AMR_MODE_74,
    CODEC_AMR_MODE_795,
    CODEC_AMR_MODE_102,
    CODEC_AMR_MODE_122,
    CODEC_AMR_MODE_MRDTX,
    CODEC_AMR_MODE_BUTT
};
typedef VOS_UINT16 CODEC_AMR_MODE_ENUM_UINT16;

/*****************************************************************************
 实体名称  : CODEC_AMRWB_MODE_ENUM
 功能描述  : AMR速率模式定义
*****************************************************************************/
enum CODEC_AMRWB_MODE_ENUM
{
    CODEC_AMRWB_MODE_660            = 0,
    CODEC_AMRWB_MODE_885,
    CODEC_AMRWB_MODE_1265,
    CODEC_AMRWB_MODE_1425,
    CODEC_AMRWB_MODE_1585,
    CODEC_AMRWB_MODE_1825,
    CODEC_AMRWB_MODE_1985,
    CODEC_AMRWB_MODE_2305,
    CODEC_AMRWB_MODE_2385,
    CODEC_AMRWB_MODE_MRDTX,
    CODEC_AMRWB_MODE_BUTT
};
typedef VOS_UINT16 CODEC_AMRWB_MODE_ENUM_UINT16;

/*****************************************************************************
 实体名称  : CODEC_OBJ_ENUM
 功能描述  : 声码器编码器/解码器类型定义
*****************************************************************************/
enum CODEC_OBJ_ENUM
{
    CODEC_OBJ_ENCODER              = 0,
    CODEC_OBJ_DECODER,
    CODEC_OBJ_BUT
};
typedef VOS_UINT16  CODEC_OBJ_ENUM_UINT16;

/*****************************************************************************
 实体名称  : CODEC_PREFRAME_TYPE_ENUM
 功能描述  : 之前帧状态
*****************************************************************************/
enum CODEC_PREFRAME_TYPE_ENUM
{
    CODEC_PREFRAME_NONE            = 0,                                         /* 无上一帧 */
    CODEC_PREFRAME_SPEECH,                                                      /* 语音帧 */
    CODEC_PREFRAME_SID,                                                         /* SID帧 */
    CODEC_PREFRAME_BUTT
};
typedef VOS_UINT16  CODEC_PREFRAME_TYPE_ENUM_UINT16;

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
 实体名称  : CODEC_ENC_IN_PARA_STRU
 功能描述  : 声码器编码输入参数结构体
*****************************************************************************/
typedef struct
{
    CODEC_DTX_ENUM_UINT16               enDtxMode;                              /* 使能DTX功能模式 */
    CODEC_AMR_FORMAT_ENUM_UINT16        enAmrFormat;                            /* 对于AMR，指示当前是UMTS或GSM码流 */
    VOS_UINT16                          enAmrMode;                              /* AMR编码时使用的速率模式 */
    CODEC_AMR_NSYNC_ENUM_UINT16         enAmrNsync;                             /* 输入，AMR DTX使能时指示切换, 功能未实现, 暂未使用 */
    VOS_INT16                          *pshwEncSpeech;                          /* 指向输入编码器的PCM码流指针 */
}CODEC_ENC_IN_PARA_STRU;

/*****************************************************************************
 实体名称  : CODEC_ENC_OUT_PARA_STRU
 功能描述  : 声码器编码输出参数结构体
*****************************************************************************/
typedef struct
{
    CODEC_AMR_TYPE_TX_ENUM_UINT16       enAmrFrameType;                         /* GSM下AMR编码后输出的帧类型 */
    CODEC_VAD_ENUM_UINT16               enVadFlag;                              /* HR/FR/EFR使用, Voice Active Detection flag */
    CODEC_SP_ENUM_UINT16                enSpFlag;                               /* HR/FR/EFR使用, Speech flag */
    VOS_UINT16                          enAmrMode;                              /* AMR/AMR WB编码时使用的速率模式 */
    VOS_INT16                          *pshwEncSerial;                          /* 指向编码器输出的码流指针 */
}CODEC_ENC_OUT_PARA_STRU;

/*****************************************************************************
 实体名称  : CODEC_DEC_IN_PARA_STRU
 功能描述  : 声码器解码输入参数结构体
*****************************************************************************/
typedef struct
{
    CODEC_AMR_FORMAT_ENUM_UINT16        enAmrFormat;                            /* 指示当前是UMTS或GSM码流 */
    VOS_UINT16                          enAmrMode;                              /* AMR解码时使用的速率模式 */
    CODEC_AMR_TYPE_RX_ENUM_UINT16       enAmrFrameType;                         /* GSM下AMR接收帧类型 */
    CODEC_BFI_ENUM_UINT16               enBfiFlag;                              /* HR/FR/EFR使用, Bad Frame Indication */
    CODEC_SID_ENUM_UINT16               enSidFlag;                              /* HR/FR/EFR使用, Silence Descriptor flag */
    CODEC_TAF_ENUM_UINT16               enTafFlag;                              /* HR/FR/EFR使用, Time Alignment flag  */
    CODEC_BFI_ENUM_UINT16               enHrUfiFlag;                            /* HR使用, Unreliable Frame Indication */
    VOS_UINT16                          uhwFrameFacchCnt;                       /* 连续收到的FA帧计数 */
    VOS_INT16                           shwFrameRangeMax;                       /* 时域幅度最大值 */
    VOS_INT16                           shwReserved;
    VOS_INT16                          *pshwDecSerial;                          /* 指向信道译码产生用于声码器解码数据流的指针*/
    CODEC_PREFRAME_TYPE_ENUM_UINT16     enGoodFrameType;                        /* 好帧类型:0-无上一帧 1-语音帧 2-SID帧 */
    VOS_UINT16                          uhwFrameBfiCnt;                         /* 连续收到的BFI帧计数 */
    VOS_UINT16                          uhwRxTypeForAjb;                        /* 帧类型，用于AJB，非AJB时不用关心 */
    VOS_UINT16                          uhwReserve;
}CODEC_DEC_IN_PARA_STRU;

/*****************************************************************************
 实体名称  : CODEC_DEC_PREIN_PARA_STRU
 功能描述  : 声码器解码前一帧输入参数结构体
*****************************************************************************/
typedef struct
{
    CODEC_AMR_FORMAT_ENUM_UINT16        enAmrFormat;                            /* 指示当前是UMTS或GSM码流 */
    VOS_UINT16                          enAmrMode;                              /* AMR解码时使用的速率模式 */
    CODEC_AMR_TYPE_RX_ENUM_UINT16       enAmrFrameType;                         /* GSM下AMR接收帧类型 */
    CODEC_BFI_ENUM_UINT16               enBfiFlag;                              /* HR/FR/EFR使用, Bad Frame Indication */
    CODEC_SID_ENUM_UINT16               enSidFlag;                              /* HR/FR/EFR使用, Silence Descriptor flag */
    CODEC_TAF_ENUM_UINT16               enTafFlag;                              /* HR/FR/EFR使用, Time Alignment flag  */
    CODEC_BFI_ENUM_UINT16               enHrUfiFlag;                            /* HR使用, Unreliable Frame Indication */
    VOS_UINT16                          uhwFrameFacchCnt;                       /* 连续收到的FA帧计数 */
}CODEC_DEC_PREIN_PARA_STRU;
/*****************************************************************************
 实体名称  : IMS_DATA_OBJ_STRU
 功能描述  : Decode Buffer存放的IMS下行数据格式
*****************************************************************************/
typedef struct
{
    unsigned short                      usRateMode;                             /* 上行编码的RateMode */
    unsigned short                      usErrorFlag;                            /* Buffer没数据时的ErrorFlag */
    unsigned short                      usFrameType;                            /* 下行本帧的FrameType */
    unsigned short                      usQualityIdx;                           /* Quality indification */
    unsigned short                      ausData[IMSA_CODEC_MAX_DATA_LEN];       /* IF1协议中的A、B、C子流，ABC紧密排列，Byte对齐 */
}IMS_DL_DATA_OBJ_STRU;

/*****************************************************************************
 实体名称  : IMS_DATA_OBJ_STRU
 功能描述  : Encode Buffer存放的IMS上行数据格式,与ImsCodecInterface对应
*****************************************************************************/
typedef struct
{
    CODEC_AMR_TYPE_TX_ENUM_UINT16       usStatus;
    VOS_UINT16                          usFrameType;                            /* 上行本帧的FrameType */
    VOS_UINT16                          ausData[IMSA_CODEC_MAX_DATA_LEN];       /* IF1协议中的A、B、C子流，ABC紧密排列，Byte对齐 */
}IMS_UL_DATA_OBJ_STRU;
/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/
/* 帧类型回调函数 */
typedef VOS_INT16 (*CODEC_FRAMETYPE_CALLBACK)(VOS_UINT16);

/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

/*****************************************************************************
 函 数 名  : AMR_Dec_Init
 功能描述  : AMR初始化函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMR_Dec_Init(VOS_VOID);

/*****************************************************************************
 函 数 名  : AMR_Enc_Init
 功能描述  : AMR编码初始化函数
 输入参数  : enDtxMode         --使能DTX功能模式, 1为使能, 0为禁用
             pfCodecCallBack   --帧类型回调函数
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMR_Enc_Init(
                       CODEC_DTX_ENUM_UINT16 enDtxMode,
                       VOS_VOID                 *pfCodecCallBack);

/*****************************************************************************
 函 数 名  : AMR_Encode
 功能描述  : AMR编码
 输入参数  : pstEncInPara      --  指向声码器编码输入参数结构体的指针
 输出参数  : pstEncOutPara     --  指向编码后输出参数结构体的指针
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara);

/*****************************************************************************
 函 数 名  : AMR_Decode
 功能描述  : AMR解码
 输入参数  : pstDecPara     --  指向声码器解码结构体的指针
 输出参数  : pshwDecSpeech  --  解码数据,长度为160个字节的PCM数据
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMR_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech);

/*****************************************************************************
 函 数 名  : AMRWB_Dec_Init
 功能描述  : AMRWB解码初始化函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMRWB_Dec_Init(VOS_VOID);

/*****************************************************************************
 函 数 名  : AMRWB_Enc_Init
 功能描述  : AMRWB编码初始化函数
 输入参数  : enDtxMode         --使能DTX功能模式, 1为使能, 0为禁用
             pfCodecCallBack   --帧类型回调函数
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMRWB_Enc_Init(
                CODEC_DTX_ENUM_UINT16  enDtxMode,
                VOS_VOID                  *pfCodecCallBack);

/*****************************************************************************
 函 数 名  : AMRWB_Encode
 功能描述  : AMRWB编码
 输入参数  : pstEncInPara      --  指向声码器编码输入参数结构体的指针
 输出参数  : pstEncOutPara     --  指向编码后输出参数结构体的指针
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMRWB_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara);

/*****************************************************************************
 函 数 名  : AMRv_Decode
 功能描述  : AMRWB解码
 输入参数  : pstDecPara     --  指向声码器解码结构体的指针
 输出参数  : pshwDecSpeech  --  解码数据,长度为160个字节的PCM数据
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMRWB_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech);

/*****************************************************************************
 函 数 名  : EFR_Dec_Init
 功能描述  : EFR解码初始化函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 EFR_Dec_Init(VOS_VOID);

/*****************************************************************************
 函 数 名  : EFR_Enc_Init
 功能描述  : EFR初始化函数
 输入参数  : enDtxMode  --表示使能DTX功能模式，1为使能，0为禁用
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 EFR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode);

/*****************************************************************************
 函 数 名  : EFR_Encode
 功能描述  : EFR编码函数
 输入参数  : pstEncInPara      --指向声码器编码输入参数结构体的指针
 输出参数  : pstEncOutPara     --指向编码后输出参数结构体的指针
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 EFR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara);

/*****************************************************************************
 函 数 名  : EFR_Decode
 功能描述  : EFR解码函数
 输入参数  : pstDecPara     --指向声码器编码结构体的指针
 输出参数  : pshwDecSpeech  --编码数据，长度为160个字节的PCM数据
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 EFR_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech);

/*****************************************************************************
 函 数 名  : FR_Dec_Init
 功能描述  : FR解码初始化函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 FR_Dec_Init(VOS_VOID);

/*****************************************************************************
 函 数 名  : FR_Enc_Init
 功能描述  : FR编码初始化函数
 输入参数  : enDtxMode 表示使能DTX功能模式，1为使能，0为禁用
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 FR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode);

/*****************************************************************************
 函 数 名  : FR_Encode
 功能描述  : 完成一帧160个语音数据的FR编码操作
 输入参数  : pstEncInPara      --  指向声码器编码输入参数结构体的指针
 输出参数  : pstEncOutPara     --  指向编码后输出参数结构体的指针
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 FR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara);

/*****************************************************************************
 函 数 名  : FR_Decode
 功能描述  : FR解码函数
 输入参数  : pstVocodecDecPara --指向声码器解码结构体的指针
 输出参数  : pshwDecSpeech     --解码后数据，长度为160个字节的PCM数据
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 FR_Decode(
                CODEC_DEC_IN_PARA_STRU  *pstVocodecDecPara,
                VOS_INT16 *pshwDecSpeech);

/*****************************************************************************
 函 数 名  : HR_Dec_Init
 功能描述  : HR解码初始化函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 HR_Dec_Init(VOS_VOID);

/*****************************************************************************
 函 数 名  : HR_Enc_Init
 功能描述  : HR编码初始化函数
 输入参数  : enDtxMode 表示使能DTX功能模式，1为使能，0为禁用
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/

extern VOS_UINT32 HR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode);

/*****************************************************************************
 函 数 名  : HR_Encode
 功能描述  : 完成一帧160个语音数据的HR编码操作
 输入参数  : pstEncInPara      --  指向声码器编码输入参数结构体的指针
 输出参数  : pstEncOutPara     --  指向编码后输出参数结构体的指针
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 HR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU  *pstEncOutPara);

/*****************************************************************************
 函 数 名  : HR_Decode
 功能描述  : HR解码函数
 输入参数  : pstVocodecDecPara -- 指向声码器解码结构体的指针
 输出参数  : pshwDecSpeech     -- 解码后数据，长度为160个字节的PCM数据
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 HR_Decode(
                CODEC_DEC_IN_PARA_STRU  *pstVocodecDecPara,
                VOS_INT16                   *pshwDecSpeech);

/*****************************************************************************
 函 数 名  : CODEC_ComCodecBits2Prm
 功能描述  : 将对应比特数转换为参数
 输入参数  : VOS_INT16 *pshwBitStream      --比特流指针
             VOS_INT16 shwNumOfBits        --待转换比特个数
 输出参数  : VOS_INT16 *pshwPrmValue       --转换获得的参数指针
 返 回 值  : VOS_VOID
*****************************************************************************/
extern VOS_VOID CODEC_ComCodecBits2Prm(
                VOS_INT16 *pshwBitStream,
                VOS_INT16 shwNumOfBits,
                VOS_INT16 *pshwPrmValue);

/*****************************************************************************
 函 数 名  : CODEC_ComCodecPrm2Bits
 功能描述  : 将参数转化为对应比特，每个比特占一个word
 输入参数  : VOS_INT16 shwValue         --需要转换的参数值
             VOS_INT16 shwNumOfBits     --参数包含的比特数
 输出参数  : VOS_INT16 *pshwBitStream   --转换后的比特流
 返 回 值  : VOS_VOID
*****************************************************************************/
extern VOS_VOID CODEC_ComCodecPrm2Bits(
                VOS_INT16 shwValue,
                VOS_INT16 shwNumOfBits,
                VOS_INT16 *pshwBitStream);

/*****************************************************************************
 函 数 名  : CODEC_ComCodecBits2Bytes
 功能描述  : 将非紧凑逐比特排列的比特流格式转换为紧凑排列的码流格式
 输入参数  : VOS_INT16 *pshwBits       --输入非紧凑逐比特排列的比特流指针
             VOS_INT16  shwBitsNum     --需要转换的比特数
 输出参数  : VOS_INT8  *pscBytes       --输出紧凑排列的码流指针
 返 回 值  : VOS_VOID
*****************************************************************************/
VOS_VOID CODEC_ComCodecBits2Bytes(
                VOS_INT16 *pshwBits,
                VOS_INT16  shwBitsNum,
                VOS_INT8  *pscBytes);


/*****************************************************************************
 函 数 名  : CODEC_ComCodecBytes2Bits
 功能描述  : 将紧凑排列的码流格式转换为非紧凑逐比特排列的比特流格式
 输入参数  : VOS_INT8  *pscBytes      --输入紧凑排列的码流指针
             VOS_INT16  shwBitsNum    --需要转换的比特数
 输出参数  : VOS_INT16 *pshwBits      --转换后非紧凑比特排列的比特流指针
 返 回 值  : VOS_VOID
*****************************************************************************/
VOS_VOID CODEC_ComCodecBytes2Bits(
                VOS_INT8  *pscBytes,
                VOS_INT16  shwBitsNum,
                VOS_INT16 *pshwBits);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of codec_com_codec.h */

