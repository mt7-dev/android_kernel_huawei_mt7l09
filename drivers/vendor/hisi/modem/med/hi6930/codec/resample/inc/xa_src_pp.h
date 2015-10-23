

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "audio.h"
#include "xa_comm.h"
#include "xa_src_pp_api.h"

#ifndef __XA_SRC_MY_H__
#define __XA_SRC_MY_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define XA_SRC_MAX_PROCESS_NUM              (10)                                /* 最大支持独立变采样处理的个数 */
#define XA_SRC_MAX_CHANLE_NUM               (8)                                 /* 最大支持8声道 */
#define XA_SRC_CURRENT_CHANLE_NUM           (2)                                 /* 当前最多为双声道数据 */
#define XA_SRC_MAX_INPUT_CHUNK_LEN          (512)                               /* SRC模块输入最大采样点个数，4~512 为4的倍数 */
#define XA_SRC_INPUT_CHUNKSIZE_128          (128)                               /* 每次输入128个样点(N声道) */
#define XA_SRC_INPUT_CHUNKSIZE_160          (160)                               /* 每次输入160个样点(N声道) */
#define XA_SRC_INPUT_CHUNKSIZE_240          (240)                               /* 每次输入240个样点(N声道) */
#define XA_SRC_REQUIRED_ALIGNMENT           (8)                                 /* SRC模块所用到的内存首地址要求8字节对齐 */
#define XA_SRC_FINAL_OUTPUT_BUFF_SIZE       (0x4E00)                            /* SRC、DOLBY缓冲池。192K长为26ms双声道数据长度，单位:byte */

#define XA_SRC_GetApiPtr()                          (g_pfuncAudioSrcApi)                /* 获取SRC 处理函数指针 */
#define XA_SRC_SetApiPtr(apiPtr)                    (g_pfuncAudioSrcApi = apiPtr)       /* 设置SRC 处理函数指针 */
#define XA_SRC_GetApiObjPtr(enProcId)               (g_pastAudioSrcApiObj[enProcId])    /* 获取SRC 配置结构体指针 */
#define XA_SRC_SetApiObjPtr(enProcId, objPtr)       (g_pastAudioSrcApiObj[enProcId] = objPtr)
                                                                                        /* 获取SRC 配置结构体指针 */
#define XA_SRC_GetMemCfgPtr(enProcId)               (&g_astSrcMemCfg[enProcId])         /* 获取SRC模块所涉及所有内存配置结构体 */
#define XA_SRC_GetInputBuff(enProcId)               (g_astSrcCurrentCfg[enProcId].pswSrcInput)
                                                                                        /* 获取SRC模块输入Buff指针 */
#define XA_SRC_GetOutputBuff(enProcId)              (g_astSrcCurrentCfg[enProcId].pswSrcOutput)
                                                                                        /* 获取SRC模块输出Buff指针 */
#define XA_SRC_GetCurrentCfgPtr(enProcId)           (&g_astSrcCurrentCfg[enProcId])     /* 获取当前运行状态配置信息 */
#define XA_SRC_SetInputBuff(enProcId, pswInput)     (g_astSrcCurrentCfg[enProcId].pswSrcInput = pswInput)
                                                                                        /* 获取SRC模块输入Buff指针 */
#define XA_SRC_SetOutputBuff(enProcId, pswOutput)   (g_astSrcCurrentCfg[enProcId].pswSrcOutput = pswOutput)
                                                                                        /* 获取SRC模块输出Buff指针 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : XA_SRC_PROC_ID_ENUM_UINT32
 功能描述  : SRC处理的枚举
*****************************************************************************/
enum
{
    XA_SRC_PlAYBACK = 0,                                                        /* 音频播放时的变采样 */

    XA_SRC_VOIP_MICIN_48K_16K_L,                                                /* VOIP MICIN左声道48K降采样到16K */
    XA_SRC_VOIP_MICIN_48K_16K_R,                                                /* VOIP MICIN右声道48K降采样到16K */
    XA_SRC_VOIP_MICIN_16K_48K_L,                                                /* VOIP MICIN左声道16K升采样到48K */
    XA_SRC_VOIP_MICIN_16K_48K_R,                                                /* VOIP MICIN右声道16K升采样到48K */

    XA_SRC_VOIP_SPKOUT_48K_16K_L,                                               /* VOIP SPKOUT左声道48K降采样到16K */
    XA_SRC_VOIP_SPKOUT_48K_16K_R,                                               /* VOIP SPKOUT右声道48K降采样到16K */
    XA_SRC_VOIP_SPKOUT_16K_48K_L,                                               /* VOIP SPKOUT左声道16K升采样到48K */
    XA_SRC_VOIP_SPKOUT_16K_48K_R,                                               /* VOIP SPKOUT右声道16K升采样到48K */

    XA_SRC_VOIP_SMARTPA_48K_16K_L,                                              /* VOIP SMARTPA左声道48K降采样到16K */
    XA_SRC_VOIP_SMARTPA_48K_16K_R,                                              /* VOIP SMARTPA右声道48K降采样到16K */

    XA_SRC_VOICE_RECORD_TX,                                                     /* 通话录音上行变采样 */
    XA_SRC_VOICE_RECORD_RX,                                                     /* 通话录音下行变采样 */

    XA_SRC_PROC_ID_BUTT
};
typedef VOS_UINT32 XA_SRC_PROC_ID_ENUM_UINT32;

/*****************************************************************************
 实体名称  : XA_SRC_INIT_ENUM_UINT32
 功能描述  : SRC初始化的枚举
*****************************************************************************/
enum
{
    XA_SRC_INIT_NO = 0,                                                         /* 变采样未初始化 */
    XA_SRC_INIT_OK = 1,                                                         /* 变采样已经初始化 */
    XA_SRC_INIT_BUT
};
typedef VOS_UINT32 XA_SRC_INIT_ENUM_UINT32;

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
 实体名称  : XA_SRC_USER_CFG_STRU
 功能描述  : SRC对外提供的配置信息结构体
*****************************************************************************/
typedef struct
{
    VOS_INT32           swChannels;                                             /* 外部配置声道数 */
    VOS_INT32           swInputSmpRat;                                          /* 外部配置输入采样率 */
    VOS_INT32           swInputChunkSize;                                       /* 输入采样点个数，32~512，必须为4的倍数 */
    VOS_INT32           swOutputSmpRat;                                         /* 外部配置输出采样率 */
} XA_SRC_USER_CFG_STRU;

/*****************************************************************************
 实体名称  : XA_SRC_CURRENT_CFG_STRU
 功能描述  : SRC当前使用的配置信息结构体
*****************************************************************************/
typedef struct
{
    VOS_INT32                       *pswSrcInput;                               /* 本模块内部使用的输入缓冲，采用动态申请内存,音频播放完成后统一释放 */
    VOS_INT32                       *pswSrcOutput;                              /* 本模块内部使用的输出缓冲，输入输出缓冲要求8字节对齐 */
    XA_SRC_INIT_ENUM_UINT32          enInitFlag;                                /* 是否初始化标志位 */
    VOS_INT32                        swChannels;                                /* 外部配置声道数 */
    VOS_INT32                        swInputSmpRat;                             /* 外部配置输入采样率 */
    VOS_INT32                        swInputChunkSize;                          /* 输入采样点个数 */
    VOS_INT32                        swOutputSmpRat;                            /* 外部配置输出采样率 */
    VOS_INT32                        swOutputChunkSize;                         /* 输出采样点个数 */
} XA_SRC_CURRENT_CFG_STRU;

/*****************************************************************************
 实体名称  : XA_SRC_IO_CFG_STRU
 功能描述  : SRC的输入输出buff，内存是动态申请的
*****************************************************************************/
typedef struct
{
    VOS_INT32          *paswInputBuff[8];                                       /* 输入数据 */
    VOS_INT32          *paswOutputBuff[8];                                      /* 输出数据 */
} XA_SRC_IO_CFG_STRU;

/*****************************************************************************
 实体名称  : XA_SRC_SOURCE_STRU
 功能描述  : SRC输入输出数据定义
*****************************************************************************/
typedef struct
{
    VOS_UINT8          *pucData;                                                /* 输入数据首地址(4字节对齐)*/
    VOS_INT32           swSize;                                                 /* 输入数据大小(单位Byte) */
    VOS_INT32           swSmpRat;                                               /* 采样率(8000 - 8KHz, 16000 - 16KHz, ...) */
    VOS_INT32           swReso;                                                 /* 每个样点的Byte数 */
    VOS_INT32           swChnNum;                                               /* 通道数 */
} XA_SRC_SOURCE_STRU;

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
extern VOS_UINT32 XA_SRC_Close(XA_SRC_PROC_ID_ENUM_UINT32 enProcId);
extern VOS_VOID XA_SRC_Convert16To24bit(XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                                                VOS_VOID *pSrcBuff,
                                                VOS_INT32 *pswDestBuff);
extern VOS_VOID XA_SRC_Convert24To16bit( XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                                                 VOS_INT32 **ppuwSrcBuff,
                                                 VOS_VOID  *pDestBuff);
extern VOS_VOID  XA_SRC_ConvertChn24To16bit( VOS_INT16* pshwDst, VOS_INT32 *pswSrc, VOS_UINT32 uwLen);
extern VOS_UINT32 XA_SRC_ExeProcess( XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                                            VOS_VOID   *pInputBuff,
                                            VOS_VOID   *pOutputBuff,
                                            VOS_UINT32 *puwOutputBuffSize);
extern VOS_INT32 XA_SRC_Init( XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                                  XA_SRC_USER_CFG_STRU * pstSrcUserCfg);
extern VOS_INT32  XA_SRC_MemoryAlloc (xa_codec_func_t                  *pfuncProcess,
                                              xa_codec_handle_t                 pstApiObj,
                                              XA_COMM_MEMORY_CFG_STRU          *pstMemoryCfg,
                                              XA_SRC_USER_CFG_STRU             *pstSrcUserCfg,
                                              XA_SRC_PROC_ID_ENUM_UINT32        enProcId);
extern VOS_UINT32  XA_SRC_Proc8x(XA_SRC_PROC_ID_ENUM_UINT32   enProcId,
                                       VOS_INT16                   *pshwInputBuff,
                                       VOS_INT16                   *pshwOutputBuff,
                                       VOS_UINT32                  *puwOutPutSize,
                                       VOS_UINT32                   uwProcTimes);
extern VOS_INT32 XA_SRC_SetBlksMemory(xa_codec_func_t        *pfuncProcess,
                                       xa_codec_handle_t               pstApiObj,
                                       XA_COMM_MEMORY_CFG_STRU        *pstMemoryCfg,
                                       XA_SRC_USER_CFG_STRU           *pstSrcUserCfg,
                                       XA_SRC_PROC_ID_ENUM_UINT32      enProcId);
extern VOS_INT32 XA_SRC_SetPara(XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                                     XA_SRC_USER_CFG_STRU *pstSrcUserCfg);






#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of Xa_src_my.h */
