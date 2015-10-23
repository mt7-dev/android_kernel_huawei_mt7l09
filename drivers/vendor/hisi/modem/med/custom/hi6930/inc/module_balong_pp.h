
#ifndef __MODULE_BALONG_PP_H__
#define __MODULE_BALONG_PP_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "mlib_interface.h"




#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* PP处理NV参数数组 */
#define MED_PP_NV_ARRAY_TOTAL_LEN           (600)                               /*语音声学处理NV参数总长度*/
#define MED_CTRL_McPpNvArrayPtr()           (&g_ashwPpNv[0])                    /* 获取PP处理NV参数全局数组指针 */

#define MODULE_BALONG_GetVoiceParaSet()     (&g_stModuleBalongVoiceCtrl.stPara)
#define MODULE_BALONG_GetAudioParaSet()     (&g_stModuleBalongAudioCtrl.stPara)

#define MODULE_BALONG_GetVoiceCtrl()        (&g_stModuleBalongVoiceCtrl)
#define MODULE_BALONG_GetAudioCtrl()        (&g_stModuleBalongAudioCtrl)

#define MED_CTRL_SAMPLE_RATE_NUM            (2)                                 /* 支持的采样率个数 8k和16k */
#define MED_CTRL_DEVICE_MODE_NUM            (7)                                 /* 支持的模式个数 handset handfree ... */
#define MED_CTRL_MODE_SUB_ID_NUM            (3)                                 /* 同一个模式下的ID个数 */
#define MED_CTRL_MODE_SUB_ID_LEN            (200)                               /* 单个ID的数据长度 单位:int16 */
#define MED_CTRL_MC_SET_NV_MSG_MASK         (7)                                 /* SETNV消息函数NV拆分完成掩码 */


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/
/* 语音设备参数结构体 */
typedef struct
{
    MLIB_UINT16                          usDeviceMode;                          /* 语音设备：0 手持；1 手持免提；2 车载免提；3 耳机；4 蓝牙；5 PCVOICE */
    MLIB_UINT16                          usNetMode;                             /* 网络模式：0 GSM； 1 WCDMA；   3 TD */
    MLIB_UINT16                          usSampleRate;                          /* 采样率  : 0 8k;   1 16k */
    MLIB_UINT16                          usIndex;                               /* 拆分消息号 即ID个数 */
    MLIB_INT16                           ashwNv[MED_CTRL_MODE_SUB_ID_LEN];      /* 单个ID的NV参数数组 */
}OM_BALONG_VOICE_NV_DATA_STRU;

/* 音频设备参数结构体 */
typedef struct
{
    MLIB_UINT16                          usDeviceMode;                          /* 语音设备：0 手持；1 手持免提；2 车载免提；3 耳机；4 蓝牙；5 PCVOICE */
    MLIB_UINT16                          usSampleRate;                          /* 采样率  : 0 8k;   1 16k */
    MLIB_UINT32                          usIndex;                               /* 拆分消息号 即ID个数 */
    MLIB_INT16                           ashwNv[MED_CTRL_MODE_SUB_ID_LEN];      /* 单个ID的NV参数数组 */
}OM_BALONG_AUDIO_NV_DATA_STRU;

/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/* 控制结构体 */
typedef struct
{
    MLIB_INT32                          swRef;                                  /* 结构体引用计数 */
    MLIB_PARA_STRU                      stPara;                                 /* 参数设置 */
} MODULE_BALONG_CTRL_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern MODULE_BALONG_CTRL_STRU g_stModuleBalongVoiceCtrl;
extern MODULE_BALONG_CTRL_STRU g_stModuleBalongAudioCtrl;

/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_Clear( MLIB_VOID );
extern MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_Get(
                       MLIB_VOID              *pvData,
                       MLIB_UINT32             uwSize);
extern MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_Init( MLIB_VOID );
extern MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_ProcMicIn(
                       MLIB_SOURCE_STRU       *pstSourceOut,
                       MLIB_SOURCE_STRU       *pstSourceIn);
extern MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_ProcSpkOut(
                       MLIB_SOURCE_STRU       *pstSourceOut,
                       MLIB_SOURCE_STRU       *pstSourceIn);
extern MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_Set(
                       MLIB_PARA_STRU         *pstPara,
                       MLIB_UINT32             uwSize);
extern MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_Clear( MLIB_VOID );
extern MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_Get(
                       MLIB_VOID              *pvData,
                       MLIB_UINT32             uwSize);
extern MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_Init( MLIB_VOID );
extern MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_ProcMicIn(
                       MLIB_SOURCE_STRU       *pstSourceOut,
                       MLIB_SOURCE_STRU       *pstSourceIn);
extern MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_ProcSpkOut(
                       MLIB_SOURCE_STRU       *pstSourceOut,
                       MLIB_SOURCE_STRU       *pstSourceIn);
extern MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_Set(
                       MLIB_PARA_STRU         *pstPara,
                       MLIB_UINT32             uwSize);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of module_balong_pp.h */
