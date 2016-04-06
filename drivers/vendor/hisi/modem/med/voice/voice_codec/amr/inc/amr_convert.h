

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_com_codec.h"


#ifndef __AMR_CONVERT_H__
#define __AMR_CONVERT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define AMR_MR475_A_LENGTH               (42)                                   /*4.75kbps速率模式下A子流的长度，以bit为单位 */

#define AMR_NUM_WORDS_OF_ENC_SERIAL      (40)                                   /*AMR码流数组所占大小，单位为Word*/
#define AMR_UMTS_MAX_FRAME_SIZE          (32)                                   /*与W物理层约定的交互的AMR帧的最大长度，以32bit为单位 */
#define AMR_UMTS_MAX_SUB_FRAME_SIZE      (103)                                  /*AMR子流最大比特数*/

#define AMR_STI_SID_UPDATE               (1)
#define AMR_STI_SID_FIRST                (0)

#define AMR_SID_A_LENGTH                 (39)                                   /*AMR SID帧A子流的长度，以bit为单位*/
#define AMR_SID_B_LENGTH                 (0)                                    /*AMR SID帧B子流的长度，以bit为单位*/
#define AMR_SID_C_LENGTH                 (0)                                    /*AMR SID帧C子流的长度，以bit为单位*/
#define AMR_SID_UPDATE_LENGTH            (35)                                   /*AMR SID UPDATE帧码流长度*/
#define AMR_FULL_SPEECH_DATALEN          (250)                                  /*AMR 码流结构体长度*/

/*获取全局变量AMR帧类型比特数数据对照表*/
#define AMR_GetNumOfFrmBits(uhwMode, uhwSubFrmNum) /*lint -e(661)*/ \
                                         (g_ashwAmrFrmBitsLen[uhwMode][uhwSubFrmNum])

/*获取全局变量相应模式比特顺序对应关系表*/
#define AMR_GetBitsOrderTabPtr(uhwMode)                \
                                         (g_apAmrBitsOrderTab[uhwMode])

/*获取全局变量相应模式比特长度*/
#define AMR_GetBitsLenOfMode(uhwMode)                  \
                                         (g_ashwAmrBitsOrderTabLen[uhwMode])
/* ABC子流转换表,FrameType个数 */
#define AMR_FRAMETYPE_NUM     (16)

/* SID、NODATA帧类型 */
#define AMR_FRAMETYPE_SID    (0x08)
#define AMR_FRAMETYPE_NODATA (0xf)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : AMR_SUB_FRM_ENUM
 功能描述  : AMR子帧类型
*****************************************************************************/
enum AMR_SUB_FRM_ENUM
{
    AMR_SUB_FRM_A                   = 0,
    AMR_SUB_FRM_B                   = 1,
    AMR_SUB_FRM_C                   = 2,
    AMR_SUB_FRM_BUTT
};
typedef VOS_UINT16 AMR_SUB_FRM_ENUM_UINT16;

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


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 AMR_GsmDlConvert(VOS_INT16                        *pshwAmrFrame,
                                          CODEC_AMR_TYPE_RX_ENUM_UINT16  enRxType,
                                          CODEC_AMR_MODE_ENUM_UINT16     enMode,
                                          VOS_INT16                         *pshwDecSerial);
extern VOS_UINT32 AMR_GsmUlConvert(
                AMR_ENCODED_SERIAL_STRU *pstFrameBefConv,
                VOS_INT16               *pshwEncSerialAfter);
extern VOS_VOID AMR_UlConvertSidFirst(
                VOS_UINT16                  uhwMode,
                AMR_ENCODED_SID_FRAME_STRU *pstSidEncSerialAfter);
extern VOS_VOID AMR_UlConvertSidUpdate(
                VOS_UINT16                  uhwMode,
                VOS_INT16                  *pshwEncSerial,
                AMR_ENCODED_SID_FRAME_STRU *pstSidEncSerialAfter);
extern VOS_UINT32  AMR_UmtsDlConvert(
                VOICE_WPHY_AMR_SERIAL_STRU   *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU    *pstDecSerialAfter);
extern VOS_VOID AMR_UmtsDlConvertBadFrame(
                VOICE_WPHY_AMR_SERIAL_STRU   *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU    *pstDecSerialAfter);
extern VOS_VOID AMR_UmtsDlConvertGoodFrame(
                VOICE_WPHY_AMR_SERIAL_STRU   *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU    *pstDecSerialAfter);
extern VOS_VOID AMR_UmtsDlConvertSid(
                AMR_ENCODED_SID_FRAME_STRU  *pstSidEncSerial,
                AMR_ENCODED_SERIAL_STRU     *pstDecSerialAfter);
extern VOS_VOID AMR_UmtsDlConvertSpeech(
                VOS_UINT16                  uhwMode,
                VOICE_WPHY_AMR_SERIAL_STRU    *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU     *pstDecSerialAfter);
extern VOS_VOID AMR_UmtsDlConvertSubFrame(
                VOS_UINT16         uhwLen,
                const VOS_UINT16  *puhwBitsOrder,
                VOS_INT16         *pshwSerialBef,
                VOS_INT16         *pshwSerialAfter);
extern VOS_UINT32  AMR_UmtsUlConvert(
                AMR_ENCODED_SERIAL_STRU   *pstFrameBefConv,
                VOICE_WPHY_AMR_SERIAL_STRU  *pstFrameAfterConv);
extern VOS_UINT32 AMR_UmtsUlConvertSpeech(
                VOS_UINT16                 uhwMode,
                VOS_INT16                 *pshwEncSerialBef,
                VOICE_WPHY_AMR_SERIAL_STRU  *pstFrameAfterConv);
extern VOS_VOID AMR_UmtsUlConvertSubFrame(
                VOS_UINT16          uhwLen,
                const VOS_UINT16   *puhwBitsOrder,
                VOS_INT16          *pshwEncSerialBef,
                VOS_INT16          *pshwEncSerialAfter);
extern VOS_UINT32 AMR_ImsDlConvert(
                IMS_DL_DATA_OBJ_STRU    *pstSrc,
                AMR_ENCODED_SERIAL_STRU *pstDecSerialAfter);
extern VOS_VOID AMR_ImsUlConvert(
                VOS_UINT16 uhwAMRType,
                VOS_UINT16 uhwMode,
                VOS_INT16* pstEncSerial);
extern VOS_VOID AMR_FilledImsDlData(
                VOS_UINT8 *puchData,
                VOICE_WPHY_AMR_SERIAL_STRU *pstDes);
extern VOS_VOID AMR_FilledImsUlData(
                VOS_UINT8   *puchImsaData,
                VOICE_WPHY_AMR_SERIAL_STRU* pstHifiData);
extern VOS_VOID AMR_ImsUlBitFilled(
                VOS_UINT8* puchSrc,
                VOS_UINT8* puchDes,
                VOS_UINT32* puwCurBit,
                VOS_UINT32 uwLen);
extern VOS_VOID AMR_ImsDlBitFilled(
                VOS_UINT8* puchSrc,
                VOS_UINT8* puchDes,
                VOS_UINT32* puwCurBit,
                VOS_UINT32 uwLen);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of amr_convert.h */

