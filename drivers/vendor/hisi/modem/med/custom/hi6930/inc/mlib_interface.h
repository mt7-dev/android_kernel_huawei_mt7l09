
#ifndef __MLIB_INTERFACE_H__
#define __MLIB_INTERFACE_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "mlib_typedef.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* 支持的最大通道数 */
#define MLIB_SOURCE_CHN_MAX_NUM         (5)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* 错误枚举 */
enum MLIB_ERROR_ENUM
{
    MLIB_ERROR_NONE                     = 0,
    MLIB_ERROR_FAIL,
    MLIB_ERROR_NULL_FUNC,
    MLIB_ERROR_NULL_PTR,
    MLIB_ERROR_BUTT,
};
typedef MLIB_UINT32 MLIB_ERROR_ENUM_UINT32;

/* 模块枚举 */
enum MLIB_MODULE_ENUM
{
    MLIB_MODULE_DEFAULT                 = 0,
    MLIB_MODULE_BALONG_PP,
    MLIB_MODULE_BUTT,
};
typedef MLIB_UINT32 MLIB_MODULE_ENUM_UINT32;

/* 通路枚举 */
enum MLIB_PATH_ENUM
{
    MLIB_PATH_CS_VOICE_CALL_MICIN       = 0,
    MLIB_PATH_CS_VOICE_CALL_SPKOUT,
    MLIB_PATH_VOIP_CALL_MICIN,
    MLIB_PATH_VOIP_CALL_SPKOUT,
    MLIB_PATH_AUDIO_PLAY,
    MLIB_PATH_AUDIO_RECORD,
    MLIB_PATH_SIRI_MICIN,
    MLIB_PATH_SIRI_SPKOUT,
    MLIB_PATH_BUTT,
};
typedef MLIB_UINT32 MLIB_PATH_ENUM_UINT32;


/* 设备枚举 */
enum MLIB_DEVICE_ENUM
{
    MLIB_DEVICE_HANDSET                 = 0,                /* 手持模式 */
    MLIB_DEVICE_HANDFREE,                                   /* 免提模式 */
    MLIB_DEVICE_CARFREE,                                    /* 车载免提模式 */
    MLIB_DEVICE_HEADSET,                                    /* 耳机模式 */
    MLIB_DEVICE_BLUETOOTH,                                  /* 蓝牙模式 */
    MLIB_DEVICE_PCVOICE,                                    /* PC-VOICE模式 */
    MLIB_DEVICE_HEADPHONE,                                  /* 不带耳机MIC的耳机通话 */
    MLIB_DEVICE_BUTT,
};
typedef MLIB_UINT32 MLIB_DEVICE_ENUM_UINT32;

/* 参数来源 */
enum MLIB_PARA_SOURCE_ENUM
{
    MLIB_PARA_SOURCE_NV                 = 0,                /* 从NV项中读取参数 */
    MLIB_PARA_SOURCE_TUNER,                                 /* 调试工具下发 */
    MLIB_PARA_SOURCE_APP,                                   /* 应用程序下发 */
    MLIB_PARA_SOURCE_BUTT,
};
typedef MLIB_UINT32 MLIB_PARA_SOURCE_ENUM_UINT32;

/*****************************************************************************
  4 STRUCT定义
*****************************************************************************/

/* 数据源通道定义 */
typedef struct
{
    MLIB_UINT8                         *pucData;                                /* 输入数据首地址(不保证字节对齐) */
    MLIB_UINT32                         uwSize;                                 /* 输入数据大小(单位Byte) */
    MLIB_UINT32                         uwSampleRate;                           /* 采样率(8000 - 8KHz, 16000 - 16KHz, ...) */
    MLIB_UINT32                         uwResolution;                           /* 采样点位宽(16 - 16Bit) */
} MLIB_SOURCE_CHN_STRU;

/* 数据源定义 */
typedef struct
{
    MLIB_UINT32                         uwChnNum;                               /* 当前可用的通道数 */
    MLIB_SOURCE_CHN_STRU                astChn[MLIB_SOURCE_CHN_MAX_NUM];        /* 通道集 */
} MLIB_SOURCE_STRU;

typedef struct
{
    MLIB_UINT32                         uwChnNum;                               /* 通道数 */
    MLIB_UINT32                         uwSampleRate;                           /* 采样率(8000 - 8KHz, 16000 - 16KHz, ...) */
    MLIB_UINT32                         uwFrameLen;                             /* 帧长(采样点数) */
    MLIB_UINT32                         uwResolution;                           /* 采样点位宽(16 - 16Bit) */
    MLIB_DEVICE_ENUM_UINT32             enDevice;                               /* 设备模式 */
    MLIB_PARA_SOURCE_ENUM_UINT32        enParaSource;                           /* 参数来源 */
    MLIB_UINT32                         uwDataSize;                             /* 剩余数据长度 */
    MLIB_UINT8                          aucData[4];                             /* 剩余数据起始地址 */
} MLIB_PARA_STRU;

/*****************************************************************************
  5 函数指针
*****************************************************************************/

/* 模块初始化 */
typedef MLIB_ERROR_ENUM_UINT32 (*MLIB_ModuleInit)(MLIB_VOID);

/* 模块参数设置 */
typedef MLIB_ERROR_ENUM_UINT32 (*MLIB_ModuleSet)(           \
                    MLIB_PARA_STRU         *pstPara,        \
                    MLIB_UINT32             uwSize);

/* 模块参数获取 */
typedef MLIB_ERROR_ENUM_UINT32 (*MLIB_ModuleGet)(           \
                    MLIB_VOID              *pvData,         \
                    MLIB_UINT32             uwSize);

/* 模块数据处理 */
typedef MLIB_ERROR_ENUM_UINT32 (*MLIB_ModuleProc)(          \
                    MLIB_SOURCE_STRU       *pstSourceOut,   \
                    MLIB_SOURCE_STRU       *pstSourceIn );

/* 模块清空 */
typedef MLIB_ERROR_ENUM_UINT32 (*MLIB_ModuleClear)(MLIB_VOID);

/* 模块注册信息 */
typedef struct
{
    MLIB_MODULE_ENUM_UINT32             enID;                                   /* 模块ID */
    MLIB_ModuleInit                     pfInit;                                 /* 模块初始化 */
    MLIB_ModuleSet                      pfSet;                                  /* 模块参数设置 */
    MLIB_ModuleGet                      pfGet;                                  /* 模块参数获取 */
    MLIB_ModuleProc                     pfProc;                                 /* 模块数据处理 */
    MLIB_ModuleClear                    pfClear;                                /* 模块清空 */
} MLIB_MODULE_REG_STRU;

/* 场景注册信息 */
typedef struct
{
    MLIB_PATH_ENUM_UINT32               enID;                                   /* 通路ID */
    MLIB_MODULE_REG_STRU               *pstModules;                             /* 通路注册的处理模块首地址 */
    MLIB_UINT32                         uwModuleNum;                            /* 通路注册的处理模块个数 */
} MLIB_PATH_REG_STRU;


/* 配置项结构体 */
typedef struct
{
    MLIB_PATH_REG_STRU                 *pstPaths;                               /* 场景数组首指针 */
    MLIB_UINT32                         uwNum;                                  /* 场景数目 */
} MLIB_PROFILE_REG_STRU;

/* 运算库基本配置 */
typedef struct
{
    MLIB_UINT32                         uwProfile;                              /* 当前选用的配置 */
} MLIB_CONFIG_STRU;

/*****************************************************************************
  6 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  7 全局变量声明
*****************************************************************************/


/*****************************************************************************
  8 函数声明
*****************************************************************************/

extern MLIB_ERROR_ENUM_UINT32  MLIB_PathClear( MLIB_PATH_ENUM_UINT32   enPath );
extern MLIB_MODULE_REG_STRU*  MLIB_PathGetModules(
                       MLIB_PATH_ENUM_UINT32   enID,
                       MLIB_UINT32            *puwNum);
extern MLIB_ERROR_ENUM_UINT32  MLIB_PathInit( MLIB_PATH_ENUM_UINT32 enPath);
extern MLIB_ERROR_ENUM_UINT32  MLIB_PathModuleSet(
                       MLIB_PATH_ENUM_UINT32   enPath,
                       MLIB_MODULE_ENUM_UINT32 enModule,
                       MLIB_PARA_STRU         *pstPara,
                       MLIB_UINT32             uwSize );
extern MLIB_ERROR_ENUM_UINT32  MLIB_PathModuleGet(
                MLIB_PATH_ENUM_UINT32   enPath,
                MLIB_MODULE_ENUM_UINT32 enModule,
                MLIB_VOID              *pvData,
                MLIB_UINT32             uwSize );
extern MLIB_ERROR_ENUM_UINT32  MLIB_PathProc(
                       MLIB_PATH_ENUM_UINT32   enPath,
                       MLIB_SOURCE_STRU       *pstSourceOut,
                       MLIB_SOURCE_STRU       *pstSourceIn);
extern MLIB_VOID  MLIB_UtilitySetSource(
                        MLIB_SOURCE_STRU       *pstSource,
                        MLIB_UINT32             uwChnID,
                        MLIB_VOID              *pvData,
                        MLIB_UINT32             uwSize,
                        MLIB_UINT32             uwSampleRate);
extern MLIB_ERROR_ENUM_UINT32  MLIB_PathSet(
                        MLIB_PATH_ENUM_UINT32   enPath,
                        MLIB_PARA_STRU         *pstPara,
                        MLIB_UINT32             uwSize );

extern MLIB_ERROR_ENUM_UINT32 MLIB_SelectProfile( MLIB_UINT32 uwID);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of mlib_interface.h */
