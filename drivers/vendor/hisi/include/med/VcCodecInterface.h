

#ifndef __VCCODECINTERFACE_H__
#define __VCCODECINTERFACE_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum VC_VOICE_MSG_ID_ENUM
{
    /*  ARM->HIFI */
    ID_VC_VOICE_BASE            = 0x9400,
    ID_VC_VOICE_START_REQ       = 0x9402,       /*启动语音处理*/                /* _H2ASN_MsgChoice VCVOICE_START_REQ_STRU */
    ID_VC_VOICE_STOP_REQ        = 0x9403,       /*语音软件停止运行*/            /* _H2ASN_MsgChoice VCVOICE_STOP_REQ_STRU */
    ID_VC_VOICE_SET_DEVICE_REQ  = 0x9404,       /*语音软件参数设置*/            /* _H2ASN_MsgChoice VCVOICE_SET_DEVICE_REQ_STRU */
    ID_VC_VOICE_SET_VOLUME_REQ  = 0x9405,       /*语音软件更新音量*/            /* _H2ASN_MsgChoice VCVOICE_SET_VOLUME_REQ_STRU */
    ID_VC_VOICE_SET_CODEC_REQ   = 0x9407,       /*更新信源编解码参数*/          /* _H2ASN_MsgChoice VCVOICE_SET_CODEC_REQ_STRU */
    ID_VC_VOICE_FOREGROUND_REQ  = 0x940A,       /*设置为前台通路*/              /* _H2ASN_MsgChoice VCVOICE_FOREGROUND_REQ_STRU */
    ID_VC_VOICE_BACKGROUND_REQ  = 0x940B,       /*设置为后台通路*/              /* _H2ASN_MsgChoice VCVOICE_BACKGROUND_REQ_STRU */
    ID_VC_VOICE_GROUND_QRY      = 0x940C,       /*查询通路状态 */               /* _H2ASN_MsgChoice VCVOICE_GROUND_QRY_STRU */
    ID_VC_VOICE_LOOP_REQ        = 0x940D,       /* 设置环回 */                  /* _H2ASN_MsgChoice VCVOICE_LOOP_REQ_STRU */

    /*  HIFI->ARM */
    ID_VOICE_VC_BASE            = 0x4900,
    ID_VOICE_VC_START_CNF       = 0x4902,       /*响应VC_VOICE_START_REQ*/      /* _H2ASN_MsgChoice VCVOICE_OP_RSLT_STRU */
    ID_VOICE_VC_STOP_CNF        = 0x4903,       /*响应VC_VOICE_STOP_REQ*/       /* _H2ASN_MsgChoice VCVOICE_OP_RSLT_STRU */
    ID_VOICE_VC_SET_DEVICE_CNF  = 0x4904,       /*响应VC_VOICE_SET_DEVICE_REQ*/ /* _H2ASN_MsgChoice VCVOICE_OP_RSLT_STRU */
    ID_VOICE_VC_SET_VOLUME_CNF  = 0x4905,       /*响应VC_VOICE_SET_VOLUME_REQ*/ /* _H2ASN_MsgChoice VCVOICE_OP_RSLT_STRU */
    ID_VOICE_VC_SET_CODEC_CNF   = 0x4907,       /*响应VC_VOICE_SET_CODEC_REQ*/  /* _H2ASN_MsgChoice VCVOICE_OP_RSLT_STRU */
    ID_VOICE_VC_FOREGROUND_CNF  = 0x490A,       /*响应VC_VOICE_FOREGROUND_REQ*/ /* _H2ASN_MsgChoice VCVOICE_OP_RSLT_STRU */
    ID_VOICE_VC_BACKGROUND_CNF  = 0x490B,       /*响应VC_VOICE_BACKGROUND_REQ*/ /* _H2ASN_MsgChoice VCVOICE_OP_RSLT_STRU */
    ID_VOICE_VC_GROUND_RSP      = 0x490C,       /*响应VC_VOICE_GROUND_QRY*/     /* _H2ASN_MsgChoice VCVOICE_GROUND_RSP_STRU */
    ID_VOICE_VC_LOOP_CNF        = 0x490D,       /* 设置环回响应 */              /* _H2ASN_MsgChoice VCVOICE_OP_RSLT_STRU */

    VC_VOICE_MSG_ID_BUTT
};
typedef VOS_UINT16 VC_VOICE_MSG_ID_ENUM_UINT16;

enum VCVOICE_NET_MODE_ENUM
{
    VCVOICE_NET_MODE_GSM         = 0,
    VCVOICE_NET_MODE_WCDMA,
    VCVOICE_NET_MODE_TDSCDMA,
    VCVOICE_NET_MODE_IMS_EUTRAN,
    VCVOICE_NET_MODE_BUTT
};
typedef VOS_UINT16 VCVOICE_NET_MODE_ENUM_UINT16;

enum VCVOICE_TYPE_ENUM
{
    VCVOICE_TYPE_AMR      = 0,                                                  /* amr */
    VCVOICE_TYPE_EFR,                                                           /* enhanced full rate */
    VCVOICE_TYPE_FR,                                                            /* full rate */
    VCVOICE_TYPE_HR,                                                            /* half rate */
    VCVOICE_TYPE_AMR2,                                                          /* amr2 */
    VCVOICE_TYPE_AMRWB,                                                         /* AMR-WB */
    VCVOICE_TYPE_G711,                                                          /* G711 */
    VCVOICE_TYPE_BUTT                                                           /* invalid value */
};
typedef VOS_UINT16  VCVOICE_TYPE_ENUM_UINT16;

enum VCVOICE_EXECUTE_RSLT_ENUM
{
    VCVOICE_EXECUTE_RSLT_SUCC = 0,                                              /* 0 成功 */
    VCVOICE_EXECUTE_RSLT_FAIL,                                                  /* 1 失败 */
    VCVOICE_EXECUTE_RSLT_NULL_PTR,                                              /* 2 空指针 */
    VCVOICE_EXECUTE_RSLT_PARA_ERR,                                              /* 3 参数错误 */
    VCVOICE_EXECUTE_RSLT_STATE_ERR,                                             /* 4 状态错误 */
    VCVOICE_EXECUTE_RSLT_UNKNOWN_MSG,                                           /* 5 未知消息 */

    VCVOICE_EXECUTE_RSLT_BUTT
};
typedef VOS_UINT16  VCVOICE_EXECUTE_RSLT_ENUM_UINT16;

enum VCVOICE_VOLUME_TARGET_ENUM
{
    VCVOICE_VOLUME_TARGET_UP = 0,                                               /* 0 上行 */
    VCVOICE_VOLUME_TARGET_DOWN,                                                 /* 1 下行 */

    VCVOICE_VOLUME_TARGET_BUTT
};
typedef VOS_UINT16  VCVOICE_VOLUME_TARGET_ENUM_UINT16;

enum VCVOICE_WORK_TYPE_ENUM
{
    VCVOICE_WORK_TYPE_NORMAL = 0,                                               /* 0 正常工作模式 */
    VCVOICE_WORK_TYPE_TEST,                                                     /* 1 测试模式 */
    VCVOICE_WORK_TYPE_BUTT
};
typedef VOS_UINT16  VCVOICE_WORK_TYPE_ENUM_UINT16;

/* 通话模式枚举 */
enum VCVOICE_DEV_MODE_ENUM
{
    VCVOICE_DEV_MODE_HANDSET                = 0,                                /* 手持模式 */
    VCVOICE_DEV_MODE_HANDFREE,                                                  /* 免提模式 */
    VCVOICE_DEV_MODE_CARFREE,                                                   /* 车载免提模式 */
    VCVOICE_DEV_MODE_HEADSET,                                                   /* 耳机模式 */
    VCVOICE_DEV_MODE_BLUETOOTH,                                                 /* 蓝牙模式 */
    VCVOICE_DEV_MODE_PCVOICE,                                                   /* PC-VOICE模式 */
    VCVOICE_DEV_MODE_HEADPHONE,                                                 /* 不带耳机MIC的耳机通话 */
    VCVOICE_DEV_MODE_SUPER_HANDFREE,                                            /* 超级免提 */
    VCVOICE_DEV_MODE_SMART_TALK,                                                /* 智音通话 */
    VCVOICE_DEV_MODE_BUTT
};
typedef VOS_UINT16  VCVOICE_DEV_MODE_ENUM_UINT16;

/* 通路状态 */
enum VCVOICE_GROUND_ENUM
{
    VCVOICE_GROUND_FOREGROUND               = 0,                                /* 前台模式 */
    VCVOICE_GROUND_BACKGROUND,                                                  /* 后台模式 */
    VCVOICE_GROUND_INVALID,                                                     /* 非法 */
    VCVOICE_GROUND_BUTT
};
typedef VOS_UINT16  VCVOICE_GROUND_ENUM_UINT16;

/* 环回状态 */
enum VCVOICE_LOOP_ENUM
{
    VCVOICE_LOOP_STOP                       = 0,                                /* 退出环回模式 */
    VCVOICE_LOOP_START,                                                         /* 进入环回模式 */
    VCVOICE_LOOP_BUTT
};
typedef VOS_UINT16  VCVOICE_LOOP_ENUM_UINT16;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 结构名    : VCCODEC_START_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 启动语音处理消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve1;                             /* 保留 */
    VCVOICE_NET_MODE_ENUM_UINT16        enMode;                                 /* 通讯服务制式：0 G；1 W */
    VCVOICE_TYPE_ENUM_UINT16            enCodecType;                            /* 声码器类别：0 AMR；1 EFR；2 FR；3 HR；4 AMR2； 5 AMRWB */
    VCVOICE_WORK_TYPE_ENUM_UINT16       enWorkType;                             /* 工作模式:0 正常工作模式 1: 测试模式 */
    VOS_UINT16                          usReserve2;                             /* 保留 */
}VCVOICE_START_REQ_STRU;


/*****************************************************************************
 结构名    : VCCODEC_STOP_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 停止语音处理消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve;                              /* 保留 */
}VCVOICE_STOP_REQ_STRU;


/*****************************************************************************
 结构名    : VCVOICE_SET_DEVICE_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 语音设备参数结构体
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve;                              /* 保留 */
    VCVOICE_DEV_MODE_ENUM_UINT16        usDeviceMode;                           /* 语音设备：0 手持；1 手持免提；2 车载免提；3 耳机；4 蓝牙； 5 PC-VOICE  */
    VOS_UINT16                          usReserve2;                             /* 保留 */
}VCVOICE_SET_DEVICE_REQ_STRU;

/*****************************************************************************
 结构名    : VCCODEC_SET_VOLUME_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 请求语音软件更新音量参数消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve;                              /* 保留 */
    VCVOICE_VOLUME_TARGET_ENUM_UINT16   enVolTarget;
    VOS_INT16                           sVolValue;
}VCVOICE_SET_VOLUME_REQ_STRU;

/*****************************************************************************
 结构名    : VCCODEC_SET_CODEC_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 请求更新信源编解码参数参数消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve;                              /* 保留 */
    VCVOICE_NET_MODE_ENUM_UINT16        enMode;                                 /* 通讯服务制式：0 G；1 W */
    VCVOICE_TYPE_ENUM_UINT16            enCodecType;                            /* 声码器类别：0 AMR；1 EFR；2 FR；3 HR; 4 AMR2; 5 AMRWB */
}VCVOICE_SET_CODEC_REQ_STRU;

/*****************************************************************************
 结构名    : VCVOICE_FOREGROUND_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : VC模块请求MED将该路电话激活, 上下行语音输入输出恢复
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve;                              /* 保留 */
} VCVOICE_FOREGROUND_REQ_STRU;

/*****************************************************************************
 结构名    : VCVOICE_BACKGROUND_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : VC模块请求MED将该路电话去激活，上行进入了静音状态，下行的语音输出通路断开
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve;                              /* 保留 */
} VCVOICE_BACKGROUND_REQ_STRU;

/*****************************************************************************
 结构名    : VCVOICE_GROUND_QRY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : VC模块查询当前通路状态(前台/后台)
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve;                              /* 保留 */
} VCVOICE_GROUND_QRY_STRU;

/*****************************************************************************
 结构名    : VCVOICE_GROUND_RSP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 语音软件回复当前通路状态
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve;                              /* 保留 */
    VCVOICE_GROUND_ENUM_UINT16          enState;                                /* 当前状态 */
    VOS_UINT16                          usReserve2;                             /* 保留 */
} VCVOICE_GROUND_RSP_STRU;

/*****************************************************************************
 结构名    : VCCODEC_OP_RSLT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 对VC消息操作响应的消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve1;                             /* 保留 */
    VCVOICE_EXECUTE_RSLT_ENUM_UINT16    enExeRslt;                              /* 执行结果 */
    VOS_UINT16                          usReserve2;
}VCVOICE_OP_RSLT_STRU;

/*****************************************************************************
 结构名    : VCVOICE_LOOP_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 环回模式请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT16                          usMsgName;                              /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usReserve1;                             /* 保留 */
    VCVOICE_LOOP_ENUM_UINT16            enMode;                                 /* 当前状态 */
    VOS_UINT16                          usReserve2;
}VCVOICE_LOOP_REQ_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
#if 0                                                       /* _H2ASN_Skip */

/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
/* 需要更改相应的ASN编码,去掉L方式,仅保留V方式 */
typedef struct
{
    VC_VOICE_MSG_ID_ENUM_UINT16         enMsgID;            /* _H2ASN_MsgChoice_Export VC_VOICE_MSG_ID_ENUM_UINT16 */

    VOS_UINT8                           aucMsgBlock[2];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          VC_VOICE_MSG_ID_ENUM_UINT16
    ****************************************************************************/
}VOICE_VC_TRACE_MSG_DATA;

/* 协议消息结构*/
typedef struct
{
    VOS_UINT32 uwSenderCpuId;
    VOS_UINT32 uwSenderPid;
    VOS_UINT32 uwReceiverCpuId;
    VOS_UINT32 uwReceiverPid;
    VOS_UINT32 uwLength;
    VOICE_VC_TRACE_MSG_DATA               stMsgData;
}VoiceVcTrace_MSG;

#endif                                                     /* _H2ASN_Skip */

/*****************************************************************************
  10 函数声明
*****************************************************************************/








#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of VcCodecInterface.h */
