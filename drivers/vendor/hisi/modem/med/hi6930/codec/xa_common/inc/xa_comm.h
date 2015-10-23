/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : xa_com.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2012年7月9日
  最近修改   :
  功能描述   : xa_com.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年7月9日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/
#ifndef __XA_COMM_H__
#define __XA_COMM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "ucom_comm.h"
#include  "hifidrvinterface.h"
#include  "xa_apicmd_standards.h"
#include  "xa_error_handler.h"
#include  "xa_error_standards.h"
#include  "xa_memory_standards.h"
#include  "xa_type_def.h"

#include  "om_log.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* tensilica mp3/aac解码器需要的MEMORY大小 */
/* 保存一个结构体，指向MEMORY TABLE */
#define CODEC_API_MEM_SIZE          (4)
/* 保存解码器需要的各个内存的指针，20bytes不准确 */
#define CODEC_TABLE_MEM_SIZE        (20)
/* 保存上下文信息 */
#define CODEC_PERSIST_MEM_SIZE      (19100)
/* 为执行操作提供临时空间 */
#define CODEC_SCRATCH_MEM_SIZE      (47200)
/* 输入数据区域，每次需要填充满，否则可能会返回nonfatal error */
#define CODEC_INPUT_MEM_SIZE        (6100)
/* 输出区域，每次调用解码前可更改 */
#define CODEC_OUTPUT_MEM_SIZE       (32000)

#define CODEC_DECODE_FINISH_FLAG    (1)
#define CODEC_DECODE_UNFINISH_FLAG  (0)

/* 最大初始化次数 */
#define XA_COMM_MAX_INIT_CNT        (20)

/* tensilica库所需的内存对齐最大值 */
#define XA_COMM_MAX_ALIGN_NUM       (8)

#define XA_ERROR_HANDLE(err_code)   \
    if(err_code != XA_NO_ERROR) \
    { \
        OM_LogError(CODEC_TENSILICA_ERR); \
        if((VOS_UINT32)err_code & XA_FATAL_ERROR) \
        { \
            return VOS_ERR; \
        } \
    }

#define XA_FATAL_ERROR_HANDLE(err_code) \
    if((VOS_UINT32)err_code & XA_FATAL_ERROR) \
    {  \
        OM_LogError(CODEC_TENSILICA_ERR); \
        return;  \
    }

#define XA_ERROR_HANDLE_WITH_STATUS_CODE(err_code,pStatus)   \
    if(err_code != XA_NO_ERROR) \
    { \
        if((VOS_UINT32)err_code & XA_FATAL_ERROR) \
        { \
            *pStatus = CODEC_STATE_ERROR; \
        } \
        OM_LogError(CODEC_TENSILICA_ERR); \
        return VOS_ERR; \
    }

#define CODEC_XA_ERRCODE_CHECK(err_code) \
    if(err_code != XA_NO_ERROR) \
    { \
        OM_LogError1(CODEC_TENSILICA_ERR, err_code); \
        if((VOS_UINT32)err_code & XA_FATAL_ERROR)\
        {\
            return err_code; \
        }\
    }

#define CODEC_XA_GetStreamStatus()          (g_uwStreamStatus)
#define CODEC_XA_SetStreamStatus(uwStatus)  (g_uwStreamStatus = (uwStatus))


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : CODEC_STATE_ENUM
 功能描述  : MP3、AAC解码器状态
*****************************************************************************/
enum
{
    CODEC_STATE_UNINITIALIZED = 0,
    CODEC_STATE_INITIALIZED,
    CODEC_STATE_FINISH,                                                         /* AAC/MP3解码器有finish状态 */
    CODEC_STATE_ERROR,
    CODEC_STATE_BUTT
};
typedef VOS_UINT16 CODEC_STATE_ENUM_UINT16;

/*****************************************************************************
 实体名称  : CODEC_SEEK_DIRECT_ENUM
 功能描述  : SEEK方向
*****************************************************************************/
enum CODEC_SEEK_DERECT_ENUM
{
    CODEC_SEEK_FORWARDS = 0,
    CODEC_SEEK_BACKWARDS,
    CODEC_SEEK_BUT
};

typedef VOS_UINT16 CODEC_SEEK_DERECT_ENUM_UINT16;


/*****************************************************************************
 实体名称  : XA_COM_CUSTOM_ERROR_ENUM
 功能描述  : 自定义错误码
*****************************************************************************/
enum XA_COM_CUSTOM_ERROR_ENUM
{
    XA_COM_CUSTOM_NULL_POINT_ERROR       = XA_ERROR_CODE(xa_severity_fatal, xa_class_api, XA_CODEC_GENERIC, 10),
    XA_COM_CUSTOM_MEM_BLOCK_SIZE_ERROR   = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_api, XA_CODEC_GENERIC, 11),
    XA_COM_CUSTOM_READ_DATA_ERROR        = XA_ERROR_CODE(xa_severity_fatal, xa_class_api, XA_CODEC_GENERIC, 12)
};

/*****************************************************************************
 实体名称  : CODEC_POS_STATE_ENUM_UINT16
 功能描述  : 当前帧位置枚举
*****************************************************************************/
typedef enum
{
    AUDIO_PLAYER_POS_NORMAL = 0,                            /* 非文件结尾 */
    AUDIO_PLAYER_POS_END_OF_STEAM,                          /* 文件结尾 */
    AUDIO_PLAYER_POS_BUTT
} CODEC_POS_STATE_ENUM;
typedef VOS_UINT16  CODEC_POS_STATE_ENUM_UINT32;

/*****************************************************************************
 实体名称  : AUDIO_EFFECT_SWITCH_ENUM_UINT16
 功能描述  : 音效使能枚举
*****************************************************************************/
/* 音效使能开关 */
enum AUDIO_EFFECT_SWITCH_ENUM
{
    AUDIO_EFFECT_SWITCH_OFF          = 0,
    AUDIO_EFFECT_SWITCH_ON
};
typedef VOS_UINT16 AUDIO_EFFECT_SWITCH_ENUM_UINT16;


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
 实体名称  : AUDIO_DEC_PARAMS_STRU
 功能描述  : PCM码流信息
*****************************************************************************/
typedef struct
{
    VOS_INT32 swBitRate;            /* 比特率 */
    VOS_INT32 swPcmWidthSize;       /* 数据位宽 */
    VOS_INT32 swChannel;            /* 声道数 */
    VOS_INT32 swSampleRate;         /* 采样率 */
    VOS_INT32 swAacBitStreamType;   /* aac比特流编码方式，mp3不使用 */
}AUDIO_DEC_PARAMS_STRU;

/*****************************************************************************
 实体名称  : XA_COMM_MEMORY_CFG_STRU
 功能描述  : tensilica库初始化时内存配置结构体，内存释放时所用
*****************************************************************************/
typedef struct
{
    VOS_INT32  swApiObjSize;                                                    /* 内存大小均以byte为单位 */
    VOS_INT32  swTableMemSize;
    VOS_INT32  swPersistMemSize;                                                /* 常量内存首地址，Tensilica Lib内部使用 */
    VOS_INT32  swScratchMemSize;                                                /* 变量内存首地址，Tensilica Lib内部使用 */
    VOS_INT32  swHeaderMemSize;                                                 /* size of header memory, aac/mp3 dec exclusive */
    VOS_INT32  swInputMemSize;                                                  /* 输入大小 */
    VOS_INT32  swOutputMemSize;                                                 /* 输出大小 */

    VOS_VOID*  pApiStructBuff;                                                  /* api struct */
    VOS_VOID*  pTableBuff;                                                      /* table memory */
    VOS_VOID*  pPersistBuff;                                                    /* persist memory */
    VOS_VOID*  pScratchBuff;                                                    /* scratch memory */
    VOS_VOID*  pHeaderBuff;                                                     /* file header memory, aac/mp3 dec exclusive */
    VOS_VOID*  pInputBuff;                                                      /* input memory */
    VOS_VOID*  pOutputBuff;                                                     /* output memory */
}XA_COMM_MEMORY_CFG_STRU;

/*****************************************************************************
 实体名称  : XA_AUDIO_DEC_USER_CFG_STRU
 功能描述  : 音频初始化用户面内存配置结构体
*****************************************************************************/
typedef struct
{
    VOS_VOID      *pvInBuff;
    VOS_VOID      *pvOutBuff;
    VOS_VOID      *pvHeadBuff;
    VOS_INT32      swInSize;
    VOS_INT32      swConsumed;
    VOS_INT32      swOutputSize;
    VOS_INT32      swHeaderMemSize;
    VOS_UINT32     enIsEndOfStream;
    VOS_UINT32     uwDecoderState;
    VOS_UINT32     uwPcmWidthSize;
}XA_AUDIO_DEC_USER_CFG_STRU;

/*****************************************************************************
 实体名称  : XA_AUDIO_EFFECT_USER_CFG_STRU
 功能描述  : 音效初始化用户面内存配置结构体
*****************************************************************************/
typedef struct
{
    /* 用户配置项 */
    AUDIO_EFFECT_SWITCH_ENUM_UINT16        enDm3Enable;        /* DTS音效是否使能 */
    AUDIO_EFFECT_SWITCH_ENUM_UINT16        enDtsEnable;        /* 杜比音效是否使能 */
    AUDIO_EFFECT_DEVICE_UINT16             uhwCurDevice;       /* 当前设备 */
    VOS_INT16                              shwReverse;
    VOS_UINT32                             uwSampleRate;       /* 输出采样率 */
    VOS_UINT32                             uwChannel;          /* 输入声道数 */

    /* 内存管理接口 -- 内存指针 */
    VOS_VOID                              *pvInBuff;           /* 输入BUFF指针 */
    VOS_VOID                              *pvOutBuff;          /* 输出BUFF指针 */

    /* 内存管理接口 -- 内存大小 */
    VOS_INT32                              swInputSize;        /* 输入数据大小，单位Bytes */
}XA_AUDIO_EFFECT_USER_CFG_STRU;

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
extern VOS_INT32 XA_COMM_Decode(xa_codec_func_t*  pfuncProcess,
                                xa_codec_handle_t pstApiObj,
                                VOS_INT32         swInputSize,
                                VOS_UINT32        enIsEndOfStream,
                                VOS_INT32*        pswOutputSize,
                                VOS_INT32*        pswConsumeSize,
                                VOS_INT32*        pswDecodeDone);
extern VOS_INT32 XA_COMM_Init(xa_codec_func_t*   pfuncProcess,
                              xa_codec_handle_t  pstApiObj,
                              VOS_VOID*          pvInputBuff,
                              VOS_INT32          swInputSize,
                              VOS_VOID*          pvHeaderBuff,
                              VOS_INT32          swHeaderSize,
                              VOS_INT32         *pswLastConsumed);
extern VOS_INT32 XA_COMM_MemFree(XA_COMM_MEMORY_CFG_STRU* pstMemoryCfg);
extern VOS_INT32  XA_COMM_MemoryAlloc (xa_codec_func_t*           pfuncProcess,
                                                 xa_codec_handle_t           pstApiObj,
                                                 XA_COMM_MEMORY_CFG_STRU*  pstMemoryCfg);
extern VOS_INT32 XA_COMM_SetBlksMemory(xa_codec_func_t*           pfuncProcess,
                                       xa_codec_handle_t          pstApiObj,
                                       XA_COMM_MEMORY_CFG_STRU* pstMemoryCfg);
extern VOS_VOID XA_COMM_ShiftBuff(VOS_CHAR* pchBuff, VOS_INT32 swBuffSize, VOS_INT32 swConsumeBytes);
extern VOS_INT32 XA_COMM_Startup(xa_codec_func_t*           pfuncProcess,
                                 xa_codec_handle_t*       ppstApiObj,
                                 XA_COMM_MEMORY_CFG_STRU* pstMemCfg);
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of xa_comm.h */
