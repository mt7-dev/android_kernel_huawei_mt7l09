/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : fr_dtx.h
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年8月15日
  最近修改   :
  功能描述   : fr_dtx.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年8月15日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef __FR_DTX_H__
#define __FR_DTX_H__

#include "codec_op_etsi.h"
#include "fr_vad.h"
#include "CodecInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define FR_FRAME_TYPE_NUM                   (8)
#define FR_DTX_NUM_OF_LAR_COEF              (8)                                 /*反射系数对数面积比参数个数*/
#define FR_DTX_NUM_OF_XMC                   (13)                                /*RPE-pluse码字xMc参数比特数*/
#define FR_DTX_NUM_OF_BLOCK_COEF            (4)                                 /*子块个数*/
#define FR_DTX_NUM_OF_FRAME_RECORD          (4)                                 /*DTX控制保存的历史帧数*/

#define FR_DTX_INIT_VALUE_NELAPSED          (0x7fff)                            /*结构体状态变量距离上一个SID更新帧时长初始化值*/
#define FR_DTX_PN_INITIAL_SEED              (0x70816958L)                       /*感知噪声生成种子值*/

#define FR_DTX_HANGOVER                     (4)                                 /*hangover常量*/
#define FR_DTX_ELAPSED_THRESHOLD            (27)                                /*DTX判决平滑阈值*/
#define FR_DTX_CN_INT_PERIOD                (24)                                /*舒适噪声内插周期常量*/
#define FR_DTX_COMPRESS_SEVEN_TO_FIVE       (23405)                             /*5/7的Q15表示*/
#define FR_DTX_MUTE_THRESH                  (7)                                 /*静音帧构造帧数阈值*/

#define FR_DTX_SET_VALUE_SID_LARC           (2)                                 /*SID帧反射系数对数面积比设定值*/
#define FR_DTX_SET_VALUE_SID_XMAX           (8)                                 /*SID帧子块最大幅度值设定值*/
#define FR_DTX_SET_VALUE_SID_CN_NC_ODD      (40)                                /*SID感知噪声LTP延迟参数Nc奇数点值*/
#define FR_DTX_SET_VALUE_SID_CN_NC_EVEN     (120)                               /*SID感知噪声LTP延迟参数Nc偶数点值*/
#define FR_DTX_SET_VALUE_SID_SILENCE_NC     (40)                                /*静音帧LTP延迟参数设定值*/
#define FR_DTX_SET_VALUE_SID_SILENCE_MC     (1)                                 /*静音帧RPE-grid位置参数设定值*/

#define FR_DTX_STATE_MACHINE_HIGH_VALUE     (6)                                 /*DTX下行控制状态机高值*/
#define FR_DTX_STATE_MACHINE_HIGH_SUB_VALUE (5)                                 /*DTX下行控制状态机次高值*/

#define FR_DTX_MASK_FRAME_RECORD_INDEX      (0x0003)                            /*2比特帧索引值掩码*/

/* DTX上行控制字常量标识 */
#define FR_TX_SP_FLAG                       (0x0001)
#define FR_TX_VAD_FLAG                      (0x0002)
#define FR_TX_HANGOVER_ACTIVE               (0x0004)
#define FR_TX_PREV_HANGOVER_ACTIVE          (0x0008)
#define FR_TX_SID_UPDTE                     (0x0010)
#define FR_TX_USE_OLD_SID                   (0x0020)

/* DTX下行控制字常量标识 */
#define FR_RX_SP_FLAG                       (0x0001)
#define FR_RX_UPD_SID_QUANT_MEM             (0x0002)
#define FR_RX_FIRST_SID_UPDATE              (0x0004)
#define FR_RX_CONT_SID_UPDATE               (0x0008)
#define FR_RX_LOST_SID_FRAME                (0x0010)
#define FR_RX_INVALID_SID_FRAME             (0x0020)
#define FR_RX_NO_TRANSMISSION               (0x0040)
#define FR_RX_DTX_MUTING                    (0x0080)
#define FR_RX_PREV_DTX_MUTING               (0x0100)
#define FR_RX_CNI_BFI                       (0x0200)
#define FR_RX_FIRST_SP_FLAG                 (0x0400)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : FR_DTX_ENUM
 功能描述  : DTX取值定义
*****************************************************************************/
enum FR_CODEC_DTX_ENUM
{
    FR_DTX_DISABLE                      = CODEC_DTX_DISABLE,                /* DTX不使能 */
    FR_DTX_ENABLE                       = CODEC_DTX_ENABLE,                 /* DTX使能   */
    FR_DTX_BUTT
};
typedef UWord16 FR_DTX_ENUM_UINT16;

/*****************************************************************************
 枚 举 名  : FR_BFI_STATUS_ENUM
 枚举说明  : BFI帧标志状态
*****************************************************************************/
enum FR_BFI_STATUS_ENUM
{
    FR_BFI_STATUS_NOT                   = CODEC_BFI_NO,                     /*非BFI帧*/
    FR_BFI_STATUS_YES                   = CODEC_BFI_YES,                    /*  BFI帧*/
    FR_BFI_STATUS_BUTT
};
typedef UWord16 FR_BFI_STATUS_ENUM_UINT16;

/*****************************************************************************
 枚 举 名  : FR_TAF_STATUS_ENUM
 枚举说明  : TAF帧标志状态
 *****************************************************************************/
enum FR_TAF_STATUS_ENUM
{
    FR_TAF_STATUS_NOT                   = CODEC_TAF_NONE,                   /*非TAF帧*/
    FR_TAF_STATUS_YES                   = CODEC_TAF_ALIGNED,                /*  TAF帧*/
    FR_TAF_STATUS_BUTT
};
typedef UWord16 FR_TAF_STATUS_ENUM_UINT16;

/*****************************************************************************
 枚 举 名  : FR_SID_STATUS_ENUM
 枚举说明  : SID帧标志状态
 *****************************************************************************/
enum FR_SID_STATUS_ENUM
{
    FR_SID_STATUS_NOT                   = 0,                                    /*有效语音帧*/
    FR_SID_STATUS_INVALID,                                                      /*无效SID帧*/
    FR_SID_STATUS_YES,                                                          /*有效SID帧*/
    FR_SID_STATUS_BUTT
};
typedef UWord16 FR_SID_STATUS_ENUM_UINT16;


/*****************************************************************************
 枚 举 名  : FR_HOMING_STATUS_ENUM
 枚举说明  : Homing帧标志状态
 *****************************************************************************/
enum FR_HOMING_STATUS_ENUM
{
    FR_HOMING_STATUS_NOT                = 0,                                    /*非Homing帧*/
    FR_HOMING_STATUS_YES,                                                       /*Homing帧*/
    FR_HOMING_STATUS_BUTT
};
typedef UWord16 FR_HOMING_STATUS_ENUM_UINT16;


/*****************************************************************************
 枚 举 名  : FR_FRAME_TYPE_ENUM
 枚举说明  : 帧类型分类
 *****************************************************************************/
enum FR_FRAME_TYPE_ENUM
{
    FR_FRAME_TYPE_VALID_SID             = 1,                                    /*有效SID帧*/
    FR_FRAME_TYPE_INVALID_SID           = 2,                                    /*无效SID帧*/
    FR_FRAME_TYPE_GOOD_SPEECH           = 3,                                    /*有效语音帧*/
    FR_FRAME_TYPE_UNUSABLE              = 4,                                    /*不可用帧*/
    FR_FRAME_TYPE_BUTT
};
typedef UWord16 FR_FRAME_TYPE_ENUM_UINT16;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/

/*****************************************************************************
  5 消息定义
*****************************************************************************/

/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/************************************************************************
实体名称: FR_ENC_DTX_STATE_STRU
功能描述: FR上行处理DTX控制和舒适噪声估计主体结构体
************************************************************************/
typedef struct
{
    Word16                              shwCtrlWord;                            /*编码端DTX控制码字*/
    Word16                              shwNelapsed;                            /*距离上一个SID更新帧时长*/
    Word32                              swPnSeed;                               /*编码端PN产生种子*/
    Word16                              shwHangover;                            /*hangover时长(VAD=0,SP=1)*/
    Word16                              shwHistIndx;
    Word16                              ashwLarcHist[FR_DTX_NUM_OF_BLOCK_COEF][FR_DTX_NUM_OF_LAR_COEF];   /*前4帧LARC*/
    Word16                              ashwXmaxHist[FR_DTX_NUM_OF_BLOCK_COEF][FR_DTX_NUM_OF_BLOCK_COEF]; /*前4帧Xmax*/
    Word16                              ashwSidLar[FR_DTX_NUM_OF_LAR_COEF];     /*上一个SID帧LAR*/
    Word16                              ashwSidXmax[FR_DTX_NUM_OF_BLOCK_COEF];  /*上一个SID帧Xmax*/
} FR_ENC_DTX_STATE_STRU;

/************************************************************************
实体名称: FR_DEC_DTX_STATE_STRU
功能描述: FR下行处理DTX控制、舒适噪声产生及丢失帧恢复主体结构体
************************************************************************/
typedef struct
{
    Word16                              shwCtrlWord;                            /*解码端DTX控制字*/
    Word16                              shwCniState;                            /*舒适噪声插入周期状态*/
    Word16                              shwNelapsed;                            /*距离上一个SID帧时长*/
    Word16                              shwSidLostFrameNum;                     /*丢失的SID帧数累计*/
    Word16                              shwHangoverPeriod;                      /*hangover时长*/

    /*丢失帧处理状态机*/
    FR_BFI_STATUS_ENUM_UINT16           enPrevBfi;                              /*上一帧BFI标志*/
    Word16                              shwStateCnt;
    Word16                              shwMuteCount;                           /*已构造静音帧数*/
    Word32                              swPnSeed;                               /*PN噪声产生种子*/

    /*舒适噪声参数*/
    Word16                              ashwLarCnOld[FR_DTX_NUM_OF_LAR_COEF];   /*备用舒适噪声LAR参数*/
    Word16                              ashwLarCnNew[FR_DTX_NUM_OF_LAR_COEF];   /*当前舒适噪声LAR参数*/
    Word16                              ashwXmaxCnOld[FR_DTX_NUM_OF_BLOCK_COEF];/*备用舒适噪声Xmax参数*/
    Word16                              ashwXmaxCnNew[FR_DTX_NUM_OF_BLOCK_COEF];/*当前舒适噪声Xmax参数*/
} FR_DEC_DTX_STATE_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern void FR_DtxDecSidLarAndXmaxc(
                Word16                    *pshwLarc,
                Word16                    *pshwLarppNew,
                Word16                    *pshwLarppOld,
                Word16                    *pshwXmaxc,
                FR_BFI_STATUS_ENUM_UINT16  enBfi,
                FR_DEC_DTX_STATE_STRU     *pstDecDtxState);
extern void FR_DtxDecSidOtherPrms(
                Word16                     pshwNc[],
                Word16                     pshwBc[],
                Word16                     pshwMc[],
                Word16                     pshwXmc[],
                Word32                    *pswSeed);
extern void FR_DtxControlRx (
                FR_TAF_STATUS_ENUM_UINT16  enTaf,
                FR_BFI_STATUS_ENUM_UINT16  enBfi,
                FR_SID_STATUS_ENUM_UINT16  enSidFlag,
                Word16                    *pshwState,
                FR_BFI_STATUS_ENUM_UINT16 *penPrevBfi,
                FR_DEC_DTX_STATE_STRU     *pstDecDtxState);
extern void FR_DtxUpdateCniState(
                FR_TAF_STATUS_ENUM_UINT16                   enTaf,
                Word16                                     *pshwCtrlWord,
                UWord16                                     uhwFrameType,
                FR_DEC_DTX_STATE_STRU                      *pstDecDtxState);

extern void FR_DtxControlTx(
                        FR_VAD_STATUS_ENUM_UINT16  enVadFlag,
                        FR_ENC_DTX_STATE_STRU     *pstEncDtxState);
extern void FR_DtxRxControlState(
                Word16                     shwCtrlWord,
                FR_BFI_STATUS_ENUM_UINT16  enBfi,
                FR_BFI_STATUS_ENUM_UINT16 *penPrevBfi,
                Word16                    *pshwState);
extern void FR_DtxRxInit(FR_DEC_DTX_STATE_STRU * pstDecDtxState);
extern void FR_DtxTxInit(FR_ENC_DTX_STATE_STRU * pstEncDtxState);
extern void FR_DtxEncSidPrm(
                Word16                 *pshwLARc,
                Word16                 *pshwXmaxc,
                FR_ENC_DTX_STATE_STRU  *pstEncDtxState);
extern void FR_DtxGenSilence(
                Word16                  pshwLARpp[],
                Word16                  pshwNc[],
                Word16                  pshwBc[],
                Word16                  pshwMc[],
                Word16                  pshwXmaxc[],
                Word16                  pshwXmc[]);
extern void FR_DtxInterpolateCnVec(Word16 pshwOldCn[],
                Word16                  pshwNewCn[],
                Word16                  pshwInterpCn[],
                Word16                  pshwRxDtxState,
                Word16                  shwLen);
extern Word16 FR_DtxPseudoNoise(Word32 *pswShiftReg, Word16 shwNumBits);
extern Word16 FR_DtxQxmax(Word16 shwXmax);
extern void FR_DtxUpdateSidMem(
                Word16 *pshwLaruq,
                Word16                 *pshwXmaxuq,
                FR_ENC_DTX_STATE_STRU  *pstEncDtxState);
extern void FR_DtxUpdateSidRxState(
                Word16                 *shwCtrlWord,
                FR_DEC_DTX_STATE_STRU  *pstDecDtxState);


#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of fr_dtx.h */
