/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : amrwb_convert.h
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年11月23日
  最近修改   :
  功能描述   : amrwb_convert.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年11月23日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_com_codec.h"


#ifndef __AMRWB_CONVERT_H__
#define __AMRWB_CONVERT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*6.60kbps速率AMR-WB模式下A子流的长度，以bit为单位 */
#define AMRWB_MR660_A_LENGTH            (54)

/*AMR-WB码流数组所占大小，单位为MED_INT16 */
#define AMRWB_NUM_WORDS_OF_ENC_SERIAL   (40)

/*AMR-WB ABC子流总和最大比特数*/
#define AMRWB_UMTS_MAX_ABC_SUM_LENTH    (477)

/*AMR-WB SID帧A子流的长度，以bit为单位*/
#define AMRWB_SID_A_LENGTH              (40)

/*AMR-WB SID帧B子流的长度，以bit为单位*/
#define AMRWB_SID_B_LENGTH              (0)

/*AMR-WB SID帧C子流的长度，以bit为单位*/
#define AMRWB_SID_C_LENGTH              (0)

/*AMR-WB SID UPDATE帧码流长度*/
#define AMRWB_SID_UPDATE_LENGTH         (35)

/*获取全局变量AMR帧类型比特数数据对照表*/
#define AMRWB_GetNumOfFrmBits(uhwMode, uhwSubFrmNum) /*lint -e(661)*/ \
                                         (g_ashwAmrwbFrmBitsLen[uhwMode][uhwSubFrmNum])

/*获取全局变量相应模式比特顺序对应关系表*/
#define AMRWB_GetBitsOrderTabPtr(uhwMode)                \
                                         (g_apAmrwbBitsOrderTab[uhwMode])

/*获取全局变量相应模式比特长度*/
#define AMRWB_GetBitsLenOfMode(uhwMode)                  \
                                         (g_ashwAmrwbBitsOrderTabLen[uhwMode])

/*通过外部编码速率获取实际编码速率*/
#define AMRWB_GetRealEncMode(uhwMode)                  \
                                         (g_ashwAmrwbModeMapTab[uhwMode])

/* ABC子流转换表,FrameType个数 */
#define AMRWB_FRAMETYPE_NUM     (16)

/* SID、NODATA帧类型 */
#define AMRWB_FRAMETYPE_SID (0x09)
#define AMRWB_FRAMETYPE_NODATA (0xf)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : AMRWB_SUB_FRM_ENUM_UINT16
 功能描述  : AMR子帧类型
*****************************************************************************/
enum AMRWB_SUB_FRM_ENUM
{
    AMRWB_SUB_FRM_A                   = 0,
    AMRWB_SUB_FRM_B                   = 1,
    AMRWB_SUB_FRM_C                   = 2,
    AMRWB_SUB_FRM_ABC                 = 3,
    AMRWB_SUB_FRM_BUTT
};
typedef VOS_UINT16 AMRWB_SUB_FRM_ENUM_UINT16;

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
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/

extern const VOS_UINT16 g_ashwAmrwbModeMapTab[11];

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 AMRWB_GsmDlConvert(
                VOS_INT16                       *pshwAmrFrame,
                CODEC_AMRWB_TYPE_RX_ENUM_UINT16  enRxType,
                CODEC_AMRWB_MODE_ENUM_UINT16     enMode,
                VOS_INT16                       *pshwDecSerial);
extern VOS_UINT32 AMRWB_GsmUlConvert(
                AMRWB_ENCODED_SERIAL_STRU        *pstFrameBefConv,
                VOS_INT16                        *pshwEncSerialAfter);
extern VOS_VOID AMRWB_UlConvertSidFirst(
                VOS_UINT16                    uhwMode,
                AMRWB_ENCODED_SID_FRAME_STRU *pstSidEncSerialAfter);
extern VOS_VOID AMRWB_UlConvertSidUpdate(
                VOS_UINT16                        uhwMode,
                VOS_INT16                        *pshwEncSerial,
                AMRWB_ENCODED_SID_FRAME_STRU     *pstSidEncSerialAfter);
extern VOS_UINT32 AMRWB_UmtsDlConvert(
                VOICE_WPHY_AMR_SERIAL_STRU         *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU        *pstDecSerialAfter);
extern VOS_VOID AMRWB_UmtsDlConvertBadFrame(
                VOICE_WPHY_AMR_SERIAL_STRU         *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU        *pstDecSerialAfter);
extern VOS_VOID AMRWB_UmtsDlConvertGoodFrame(
                VOICE_WPHY_AMR_SERIAL_STRU         *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU        *pstDecSerialAfter);
extern VOS_VOID AMRWB_UmtsDlConvertSid(
                AMRWB_ENCODED_SID_FRAME_STRU     *pstSidEncSerial,
                AMRWB_ENCODED_SERIAL_STRU        *pstDecSerialAfter);
extern VOS_VOID AMRWB_UmtsDlConvertSpeech(
                VOS_UINT16                        uhwMode,
                VOICE_WPHY_AMR_SERIAL_STRU         *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU        *pstDecSerialAfter);
extern VOS_VOID AMRWB_UmtsDlConvertSubFrame(
                VOS_UINT16                        uhwLen,
                const VOS_UINT16                 *puhwBitsOrder,
                VOS_INT16                        *pshwSerialBef,
                VOS_INT16                        *pshwSerialAfter);
extern VOS_UINT32 AMRWB_UmtsUlConvert(
                AMRWB_ENCODED_SERIAL_STRU        *pstFrameBefConv,
                VOICE_WPHY_AMR_SERIAL_STRU         *pstFrameAfterConv);
extern VOS_UINT32 AMRWB_UmtsUlConvertSpeech(
                VOS_UINT16                        uhwMode,
                VOS_INT16                        *pshwEncSerialBef,
                VOICE_WPHY_AMR_SERIAL_STRU         *pstFrameAfterConv);
extern VOS_VOID AMRWB_UmtsUlConvertSubFrame(
                VOS_UINT16                        uhwLen,
                const VOS_UINT16                 *puhwBitsOrder,
                VOS_INT16                        *pshwEncSerialBef,
                VOS_INT16                        *pshwEncSerialAfter);
extern VOS_UINT32 AMRWB_ImsDlConvert(
                IMS_DL_DATA_OBJ_STRU      *pstSrc,
                AMRWB_ENCODED_SERIAL_STRU *pstDecSerialAfter);
extern VOS_VOID AMRWB_ImsUlConvert(
                VOS_UINT16 uhwAMRType,
                VOS_UINT16 uhwMode,
                VOS_INT16* pstEncSerial);
extern VOS_VOID AMRWB_FilledImsDlData(
                VOS_UINT8 *puchData,
                VOICE_WPHY_AMR_SERIAL_STRU *pstDes);
extern VOS_VOID AMRWB_FilledImsUlData(
                VOS_UINT8   *puchImsaData,
                VOICE_WPHY_AMR_SERIAL_STRU* pstHifiData);
extern VOS_VOID AMRWB_ImsUlBitFilled(
                VOS_UINT8* puchSrc,
                VOS_UINT8* puchDes,
                VOS_UINT32* puwCurBit,
                VOS_UINT32 uwLen);
extern VOS_VOID AMRWB_ImsDlBitFilled(
                VOS_UINT8* puchSrc,
                VOS_UINT8* puchDes,
                VOS_UINT32* puwCurBit,
                VOS_UINT32 uwLen);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

