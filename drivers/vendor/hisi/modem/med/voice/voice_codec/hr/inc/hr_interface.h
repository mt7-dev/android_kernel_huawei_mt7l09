

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_com_codec.h"

#ifndef __HR_INTERFACE_H__
#define __HR_INTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define HR_CODED_SERIAL_LENGTH          (18)                                    /*HR编码器编码结果,18个参数共36个Byte*/
#define HR_CODED_BITS_LENGTH            (112)                                   /*HR编码器编码比特提取结果,18个参数提取共112比特*/
#define HR_HOMING_WHOLE_FRAME           (18)                                    /*HR下行全帧HOMING检测参数个数*/
#define HR_HOMING_FIRST_SUBFRAME        (9)                                     /*HR下行第一子帧HOMING检测参数个数*/


#define HR_GetDtxFlagPtr()              (&giDTXon)                              /*获取全局变量HR的DTX模式变量指针*/

#define HR_GetOldResetPtr()             (&g_shwHrOldResetFlag)                  /*获取全局变量HR上一帧reset标志指针*/

#define HR_GetUnvoiceParsNumPtr()       (g_ashwHrUnvoicedParmBitsTable)         /*获取全局变量HR在Unvoice模式下各参数所占比特数表指针*/

#define HR_GetVoiceParsNumPtr()         (g_ashwHrVoicedParmBitsTable)           /*获取全局变量HR在Voice模式下各参数所占比特数表指针*/



/*****************************************************************************
  3 枚举定义
*****************************************************************************/


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
 实体名称  : HR_ENCODE_SERIAL_STRU
 功能描述  : HR声码器编码结果序列结构体，共20个参数，含VadFlag、SpFlag
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwR0;                                  /*本帧能量*/
    VOS_INT16                           ashwVqIndeces[3];                       /*LPC 矢量脉冲码本*/
    VOS_INT16                           shwSoftInterp;                          /*是否软件插值,1或0*/
    VOS_INT16                           shwVoicing;                             /*是否为Voice模式,0为非,1、2、3为是*/
    VOS_INT16                           ashwCodeWords[12];                      /*编码码本结果*/
    CODEC_VAD_ENUM_UINT16           enVadFlag;                              /*编码结果VAD标志,*/
    CODEC_SP_ENUM_UINT16            enSpFlag;                               /*编码结果SP标志,*/
}HR_ENCODE_SERIAL_STRU;

/*****************************************************************************
 实体名称  : HR_DECODE_SERIAL_STRUCT
 功能描述  : HR声码器解码输入序列结构体，共22个参数
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwR0;                                  /*本帧能量*/
    VOS_INT16                           ashwVqIndeces[3];                       /*LPC 矢量脉冲码本*/
    VOS_INT16                           shwSoftInterp;                          /*是否软件插值,1或0*/
    VOS_INT16                           shwVoicing;                             /*是否为Voice模式,0为非,1、2、3为是*/
    VOS_INT16                           ashwCodeWords[12];                      /*编码码本结果*/
    CODEC_BFI_ENUM_UINT16           enBfiFlag;                              /* HR/FR/EFR使用, Bad Frame Indication */
    CODEC_BFI_ENUM_UINT16           enHrUfiFlag;                            /* HR使用, Unreliable Frame Indication */
    CODEC_SID_ENUM_UINT16           enSidFlag;                              /* HR/FR/EFR使用, Silence Descriptor flag */
    CODEC_TAF_ENUM_UINT16           enTafFlag;                              /* HR/FR/EFR使用, Time Alignment flag  */
}HR_DECODE_SERIAL_STRU;

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
extern VOS_VOID HR_DlFrameConvert(VOS_INT16 *pshwBits, VOS_INT16 *pshwEncodedSerial);
extern VOS_VOID HR_UlFrameConvert(VOS_INT16 *pshwEncodedSerial, VOS_INT16 *pshwBits);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hr_interface.h */
