

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __XA_DTS_PP_H__
#define __XA_DTS_PP_H__

#include "ucom_comm.h"
#include "srs_trmediahd_api.h"
#include "hifidrvinterface.h"
#include "ucom_nv.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define XA_DTS_DEFAULT_ORDER_HPF                    (6)                         /* default order hpf */
#define XA_DTS_ORDER_HPF1                           (4)                         /* the order of the first HPF */
#define XA_DTS_ORDER_HPF2                           (4)                         /* the order of the second HPF */
#define XA_DTS_BANDS_PEQ                            (8)                         /* the number of PEQ bands */
#define XA_DTS_DEFAULT_BANDS_PEQ                    (8)                         /* the deault number of PEQ bands */
#define XA_DTS_CHANNEL_NUM                          (2)                         /* DTS当前配置声道数 */
#define XA_DTS_DEFAULT_GEQ_BAND_GAIN                (4096)                      /* GEQ 默认缺省增益 */
#define XA_DTS_GEQ_BAND_NUM                         (10)                        /* GEQ子带个数 */
#define XA_DTS_TBHD_PARA_LEN                        (24)
#define XA_DTS_HPF_PARA_LEN                         (20)
#define XA_DTS_PEQ_PARA_LEN                         (26)
#define XA_DTS_GetCurrentCfgPtr()                   (&g_stDtsCurtCfg)           /* 获取当前DTS模块配置信息 */
#define XA_DTS_GetUinitDDRParaPtr()                 (&g_stDtsUintPara)          /* 获取当前DTS使能与否控制信息 */
#define XA_DTS_GetCurrentParaPtr()                  (&g_stDtsCurrentPara)       /* 获取当前DTS使用的配置参数 */
/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : XA_DTS_HPF_ENUM_UINT16
 功能描述  : HPF类型枚举
*****************************************************************************/
enum
{
    XA_DTS_HPF_DEFAULTCOEFS = 0,                                                /* 缺省值 */
    XA_DTS_HPF_TESTCOEFS,                                                       /* 测试 */
    XA_DTS_HPF_BUTT
};
typedef VOS_UINT16 XA_DTS_HPF_ENUM_UINT16;

/*****************************************************************************
 实体名称  : XA_DTS_FILTER_CFG_ENUM_UINT16
 功能描述  : 滤波器配置类型枚举
*****************************************************************************/
enum
{
    XA_DTS_FILTER_CFG_DEFAULT = 0,                                              /* 缺省值 */
    XA_DTS_FILTER_CFG_NON_FLAT_PEQ,                                             /* Any number of different filter configurations can be used  */
    XA_DTS_FILTER_CFG_BUTT
};
typedef VOS_UINT16 XA_DTS_FILTER_CFG_ENUM_UINT16;

/*****************************************************************************
 实体名称  : XA_DTS_USE_PARA_ENUM_UINT16
 功能描述  : DTS使用参数表类型枚举
*****************************************************************************/
enum
{
    XA_DTS_USE_DEFAULT_PARA = 0,                                                /* 使用默认的参数表，即没有是能DTS音效 */
    XA_DTS_USE_ENABLE_PARA,                                                     /* 使用DTS使能后的参数表 */
    XA_DTS_USE_PARA_BUTT
};
typedef VOS_UINT16 XA_DTS_USE_PARA_ENUM_UINT16;

/*****************************************************************************
 实体名称  : XA_DTS_NEED_SET_PARA_ENUM_UINT16
 功能描述  : DTS是否需要设置参数类型枚举
*****************************************************************************/
enum
{
    XA_DTS_NEED_SET_PARA_NO = 0,                                                /* 当前不需要设置DTS参数 */
    XA_DTS_NEED_SET_PARA_YES,                                                   /* 当前需要设置DTS参数 */
    XA_DTS_NEED_SET_PARA_BUTT
};
typedef VOS_UINT16 XA_DTS_NEED_SET_PARA_ENUM_UINT16;

/*****************************************************************************
 实体名称  : XA_DTS_CURRENT_DEV_ENUM_UINT32
 功能描述  : DTS当前输出外设类型枚举
*****************************************************************************/
enum
{
    XA_DTS_DEV_SPEAKRE = 0,                                                     /* 当前使用speaker(外放)作为输出设备 */
    XA_DTS_DEV_HEADSET,                                                         /* 当前使用headset(耳机)作为输出设备 */
    XA_DTS_DEV_BUTT
};
typedef VOS_UINT32 XA_DTS_CURRENT_DEV_ENUM_UINT32;

/*****************************************************************************
 实体名称  : XA_DTS_ENABLE_ENUM_UINT16
 功能描述  : DTS是否使能枚举
*****************************************************************************/
enum
{
    XA_DTS_ENABLE_NO = 0,                                                       /* 不使能DTS音效 */
    XA_DTS_ENABLE_YES,                                                          /* 使能DTS音效 */
    XA_DTS_ENABLE_BUTT
};
typedef VOS_UINT16 XA_DTS_ENABLE_ENUM_UINT16;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : XA_DTS_USER_CFG_STRU
 功能描述  : DTS对外提供的配置信息结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16          *pshwInputBuff;                                          /* 待处理数据，默认为左右声道交叉存储 */
    VOS_INT16          *pshwOutputBuff;                                         /* 处理完成后，输出数据 */
    VOS_UINT32          uwBlockSize;                                            /* 一次处理的样点数 */
    VOS_UINT32          uwFilterCfg;                                            /* 滤波器配置，0为缺省设置 */
    VOS_UINT32          uwSampleRate;                                           /* 输入数据的采样率,对外提供的形式例如480000 */
    VOS_INT32           swChannelNum;                                           /* 输入数据的声道数 */
} XA_DTS_USER_CFG_STRU;

/*****************************************************************************
 实体名称  : XA_DTS_CURRENT_CFG_STRU
 功能描述  : DTS当前使用的配置信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                       uwBlockSize;                               /* 一次处理的样点数 */
    VOS_UINT32                       uwFilterCfg;                               /* 滤波器配置，0为缺省设置 */
    VOS_UINT32                       uwSampleRate;                              /* 输入数据的采样率,DTS模块维护形式例如48/K */
    VOS_INT32                        swChannelNum;                              /* 输入数据的声道数 */

    VOS_CHAR                        *pDtsObjBuff;                               /* DTS对象内存 */
    VOS_UINT32                       uwDtsObjSize;                              /* DTS对象内存大小 */
    SRSTrMediaHDObj                  stDtsObj;                                  /* DTS对象 */
    VOS_VOID                        *pvWorkSpaceBuff;                           /* WorkSpaceBuff */
    VOS_UINT32                       uwWorkSpaceSize;                           /* workspace内存大小 */
    VOS_VOID                        *pvFFTLibBuff;                              /* FFTlib库大小 */
    VOS_INT32                       *pswLeftBuff;                               /* 左声道存储空间 */
    VOS_UINT32                       uwLeftSize;                                /* 左声道内存大小 */
    VOS_INT32                       *pswRightBuff;                              /* 右声道存储空间 */
    VOS_UINT32                       uwRightSize;                               /* 右声道内存大小 */
} XA_DTS_CURRENT_CFG_STRU;

/*****************************************************************************
 实体名称  : XA_DTS_ALL_PARA_STRU
 功能描述  : DTS当前使用的所有配置信息结构体
*****************************************************************************/
typedef struct
{
    SRSTrMediaHDControls            stDtsCtrl;                                  /* DTS控制参数 */
    VOS_INT16                       ashwGeqBandGain[XA_DTS_GEQ_BAND_NUM];       /* Geq子带增益 */

} XA_DTS_ALL_PARA_STRU;

/*****************************************************************************
 实体名称  : XA_DTS_SET_PARA_STRU
 功能描述  : DTS是否使能、使用的输出设备均由发送给DTS的消息维护
*****************************************************************************/
typedef struct
{
    XA_DTS_NEED_SET_PARA_ENUM_UINT16            enIsNeedSetPara;                /* 当前DTS是否需要设置参数 */
    XA_DTS_ENABLE_ENUM_UINT16                   enIsEnable;                     /* 是否使能DTS */
    XA_DTS_CURRENT_DEV_ENUM_UINT32              enCurrentDev;                   /* 当前使用的输出设备 */
} XA_DTS_SET_PARA_STRU;

/*****************************************************************************
 实体名称  : AUDIOPLAYER_DTS_SET_ENABLE_CMD_STRU
 功能描述  : AUDIOPLAYER通知DTS设置DTS音效使能与否
*****************************************************************************/
typedef struct{
    VOS_MSG_HEADER
    unsigned short                      uhwMsgId;                               /* 设置DTS音效使能与否ID */
    unsigned short                      uhwEnableFlag;                          /* DTS音效是否使能 */
}AUDIO_PLAYER_DTS_SET_ENABLE_CMD_STRU;

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
/* device设置参数 */
extern  SRSTrMediaHDControls DTS_speaker;
extern  SRSTrMediaHDControls DTS_headphone;
extern  SRSTrMediaHDHpfOrder HPF_Order;

/* device滤波器参数设置 */
extern SRSInt32 Untitled_TBHD1[XA_DTS_TBHD_PARA_LEN];
extern SRSInt32 Untitled_TBHD2[XA_DTS_TBHD_PARA_LEN];

extern SRSInt16 Untitled_HPF1[XA_DTS_HPF_PARA_LEN];
extern SRSInt16 Untitled_HPF2[XA_DTS_HPF_PARA_LEN];

extern SRSInt32 Untitled_PEQ1[XA_DTS_PEQ_PARA_LEN];
extern SRSInt32 Untitled_PEQ2[XA_DTS_PEQ_PARA_LEN];

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_INT32  XA_DTS_CreatObj(XA_DTS_USER_CFG_STRU * pstUserCfg);
extern VOS_UINT32 XA_DTS_DeleObj( VOS_VOID );
extern VOS_INT32  XA_DTS_ExeProcess(XA_DTS_USER_CFG_STRU * pstUserCfg);
extern VOS_INT32 XA_DTS_Hybrid2Stereo(
                       VOS_INT16 *pshwI2S,
                       VOS_INT32 *pswLeft,
                       VOS_INT32 *pswRight,
                       VOS_INT32  swBlockSize);
extern VOS_UINT32 XA_DTS_Init( XA_DTS_USER_CFG_STRU * pstUserCfg );
extern VOS_VOID  XA_DTS_InitFFT_Lib(VOS_VOID);
extern VOS_VOID XA_DTS_InitObj(SRSTrMediaHDObj stTmhdObj, XA_DTS_FILTER_CFG_ENUM_UINT16 uhwFset, VOS_UINT32 uwSampleRate);
extern VOS_UINT32  XA_DTS_MsgSetDtsDevCmd( VOS_VOID *pvOsaMsg );
extern VOS_UINT32 XA_DTS_MsgSetDtsEnableCmd (VOS_VOID *pvOsaMsg);
extern VOS_UINT32  XA_DTS_SetDev( XA_DTS_CURRENT_DEV_ENUM_UINT32 enDevice );
extern VOS_UINT32  XA_DTS_SetEnable(XA_DTS_ENABLE_ENUM_UINT16 enEnableFlag);
extern VOS_INT32 XA_DTS_SetGEQBandGains(SRSTrMediaHDObj stDtsObj, VOS_INT16 * pshwGeqGain);
extern VOS_UINT32 XA_DTS_Stereo2Hybrid(
                       VOS_INT32  *pswLeft,
                       VOS_INT32  *pswRight,
                       VOS_INT32   swBlockSize,
                       VOS_INT16  *shwHybrid);


/* DTS lib库提供源文件函数申明 */
extern SRSTrMediaHDFilterConfig  GetFilterNonflatPeqCfg(int sampleRate, unsigned int device);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of xa_dts_pp.h */

