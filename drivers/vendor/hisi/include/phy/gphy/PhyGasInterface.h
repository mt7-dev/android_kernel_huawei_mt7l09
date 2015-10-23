/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : PhyGasInterface.h
  版 本 号   : 初稿
  作    者   : 方剑
  生成日期   : 2007年9月5日
  最近修改   :
  功能描述   : 该头文件是协议栈和物理层之间的接口文件。
  函数列表   :
  修改历史   :
  1.日    期   : 2007年9月5日
    作    者   : 方剑
    修改内容   : 创建文件

******************************************************************************/

#ifndef __PHYGASINTERFACE_H__
#define __PHYGASINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "EdgeInterface.h"
#include "PhyGrrInterface.h"
#include "PhyGasComm.h"


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#pragma pack(1)


/********************* 结构MPH_RXLEV_REQ_STRU 中相关域取值 ******************/
#define SEARCH_FREQ_BA_MAX_NUM                      ( 66 )                      /* 频点搜索时，在BA中取到的发送至物理层的最大频点数目 */


/*****************************  WCDMA相关宏定义  ****************************/
#define MAX_INTER_CARRIER_NUM                       ( 3 )
#define MAX_INTER_FREQ_INFO_NUM                     ( 32 )

/*****************  结构MPH_CELL_RESELECT_REQ_STRU 中使用的宏 ***************/
#define GSM_STRONGEST_FREQLIST_NUM                  ( 6 )

/******************  原语MPH_MEAS_REPORT_IND 中使用的宏 *********************/
#define GAS_GCOM_DTX_USED                           ( 1 )
#define GAS_GCOM_DTX_UNUSED                         ( 0 )
#define MPH_MEAS_REPORT_IND_CHANNEL_NUM_MAX         ( 32 )


/********************* 数据结构中usERRCode参数常量的宏定义 ******************/
#define    PHY_SUCCESS                              ( 0 )
#define    PHY_FAILURE                              ( 1 )
#define    PHY_START_TIME_ELAPSED                   ( 2 )


/******************  结构MPH_UPDATE_PAGING_PARA_REQ_STRU中使用的宏 **********/
#define GSM_IMSI_MAX_LEN                            ( 15 )
#define GSM_IMEI_LEN                                ( 15 )
#define GSM_IMEISV_LEN                              ( 16 )


/*********  结构MPH_NCELL_RESULT_IND_STRU和MPH_MEAS_REPORT_REQ中使用的宏 ****/
#define PHY_REPORT_REASON_INIT_NCELL                ( 0x00000001 )              /* 通知初始化六强邻小区 */
#define PHY_REPORT_REASON_DSC_FAIL                  ( 0x00000002 )              /* 下行链路失败 */
#define PHY_REPORT_REASON_RESEL_C1                  ( 0x00000004 )              /* C1持续5s小于0 */
#define PHY_REPORT_REASON_RESEL_C2                  ( 0x00000008 )              /* NCELL比SCELL信号好时 */
#define PHY_REPORT_REASON_3G_MEAS                   ( 0x00000010 )              /* 异系统测量启动 */
#define PHY_REPORT_REASON_NCELL_CHANGE              ( 0x00000020 )              /* 邻区改变 */
#define PHY_REPORT_REASON_SCELL_RXLEV               ( 0x00000040 )              /* 服务小区强度改变 */
#define PHY_REPORT_REASON_RANDOM_ACC_FAIL           ( 0x00000080 )              /* 上层随机接入失败 */
#define PHY_REPORT_REASON_NC_MEAS                   ( 0x00000100 )              /* NC测量报告(包括干扰测量) */
#define PHY_REPORT_REASON_DEDICATED_MEAS            ( 0x00000200 )              /* 专用态测量报告 */

#define PHY_REPORT_REASON_NAS_REQUEST               ( 0x00001000 )              /* NAS请求小区重选*/
#define PHY_REPORT_REASON_GRR_N3102                 ( 0x00002000 )              /* GRR N3102请求小区重选*/
#define PHY_REPORT_REASON_SCELL_SI_ACQU_FAIL        ( 0x00004000 )              /* GCOMSI 读取服务小区系统消息失败*/
#define PHY_REPORT_REASON_SCELL_BARRED              ( 0x00008000 )              /* 服务小区被禁*/
#define PHY_REPORT_REASON_RXLEV_REQUST              ( 0x00010000 )              /* 服务小区以及邻区上报*/
#define PHY_REPORT_REASON_GRR_ACCESS_FAILURE        ( 0x00020000 )              /* GRR 随机接入失败请求小区重选*/

#define PHY_REPORT_REASON_THRESH_GSM_LOW            ( 0x00040000 )              /* 服务小区及六强邻区从满足thresh_gsm_low门限到不满足或从不满足到满足时上报 */
#define PHY_REPORT_REASON_SCELL_TIMING_ERR          ( 0x00080000 )              /* 服务小区定时错误 */

/******  原因PHY_REPORT_REASON_NCELL_CHANGE具体原因，携带给上层便于观测 *****/
#define PHY_CHANGE_RESON_FIRST_BUILD                ( 0x01000000 )              /* 第一次构建小区上报邻小区改变 */
#define PHY_CHANGE_RESON_UPDATE_BALIST              ( 0x02000000 )              /* BA更新上报邻小区改变 */
#define PHY_CHANGE_RESON_ENTER_NCELL                ( 0x04000000 )              /* 新小区入围六强邻小区 */
#define PHY_CHANGE_RESON_OUT_NCELL                  ( 0x08000000 )              /* 小区踢出六强邻小区 */
#define PHY_CHANGE_RESON_BSIC_FAIL                  ( 0x10000000 )              /* 小区同步BSIC失败 */
#define PHY_CHANGE_RESON_BSIC_SUCC                  ( 0x20000000 )              /* 小区同步BSIC成功 */
#define PHY_CHANGE_RESON_BSIC_CHANGE                ( 0x40000000 )              /* 小区同步BSIC改变 */
#define PHY_CHANGE_RESON_NCC_PERMIT                 ( 0x80000000 )              /* 小区同步BSIC禁止 */

/************************  GAS全读系统消息的TCBitmap ************************/
#define GAS_GPHY_START_BCCH_REDADING_BITMAP_FULL    ( 0xA9FF )

#define GAS_GCOM_PHY_PARAM_INVALID_VALUE            ( 0xFFFF )                  /* 无效值，主要用于表征与DSP参数相关的无效值，以便于更新参数给DSP时可直接赋值 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


typedef enum
{
    GSM_PAGE_MODE_NORMAL                            =   0,
    GSM_PAGE_MODE_EXT                               =   1,
    GSM_PAGE_MODE_REORG                             =   2,
    GSM_PAGE_MODE_SAME                              =   3
}GSM_PAGE_MODE_ENUM;
typedef VOS_UINT16 GSM_PAGE_MODE_ENUM_UINT16;


enum GAS_PHY_CHANNEL_MODE_ENUM
{
    GAS_PHY_CHANNEL_MODE_SIGNALLING_ONLY    = 0,
    GAS_PHY_CHANNEL_MODE_TCH_FS             = 1,
    GAS_PHY_CHANNEL_MODE_TCH_EFR            = 2,
    GAS_PHY_CHANNEL_MODE_TCH_AFS            = 3,
    GAS_PHY_CHANNEL_MODE_TCH_F24            = 4,
    GAS_PHY_CHANNEL_MODE_TCH_F48            = 5,
    GAS_PHY_CHANNEL_MODE_TCH_F96            = 6,
    GAS_PHY_CHANNEL_MODE_TCH_F144           = 7,
    GAS_PHY_CHANNEL_MODE_TCH_HS             = 8,
    GAS_PHY_CHANNEL_MODE_TCH_AHS            = 9,
    GAS_PHY_CHANNEL_MODE_TCH_H24            = 10,
    GAS_PHY_CHANNEL_MODE_TCH_H48            = 11,
    GAS_PHY_CHANNEL_MODE_TCH_WFS            = 12,

    GAS_PHY_CHANNEL_MODE_BUTT
};
typedef VOS_UINT16 GAS_PHY_CHANNEL_MODE_ENUM_UINT16;


typedef enum
{
    GAS_PHY_ABType_8bit                             =   0,
    GAS_PHY_ABType_11bit                            =   1
}GAS_PHY_ABType_ENUM;
typedef VOS_UINT16 GAS_PHY_ABType_ENUM_UINT16;



typedef enum
{
    GAS_PHY_RA_TSC0                                 =   0,
    GAS_PHY_RA_TSC1                                 =   1
}GAS_PHY_RA_ENUM;
typedef VOS_UINT16 GAS_PHY_RA_ENUM_UINT16;



typedef enum
{
    GSM_CBCH_TYPE_BASIC                             =   0,                      /* BASIC CBCH 信道 */
    GSM_CBCH_TYPE_EXT                               =   1,                      /* EXT   CBCH 信道 */
    GSM_CBCH_TYPE_BUTT
}GSM_CBCH_TYPE_ENUM;
typedef VOS_UINT16 GSM_CBCH_TYPE_ENUM_UINT16;



typedef enum
{
    GSM_CBCH_SCHEDULE_INFO_INVALID                  =   0,                      /* CBCH信道调度信息无效 */
    GSM_CBCH_SCHEDULE_INFO_VALID                    =   1,                      /* CBCH信道调度信息有效 */
    GSM_CBCH_SCHEDULE_INFO_BUTT
}GSM_CBCH_SCHEDULE_INFO_ENUM;
typedef VOS_UINT16 GSM_CBCH_SCHEDULE_INFO_ENUM_UINT16;



typedef enum
{
    GAS_PHY_NETLOST_CASUE_NORMAL                    =   0,
    GAS_PHY_NETLOST_CASUE_G_INACTIVE                =   1
}GAS_PHY_NETLOST_CASUE_ENUM;
typedef VOS_UINT16 GAS_PHY_NETLOST_CASUE_ENUM_UINT16;



typedef enum
{
    PHY_MONI_AGCH_FLG_NEEDLESS                      =   0,
    PHY_MONI_AGCH_FLG_NEED                          =   1
}PHY_MONI_AGCH_FLG_ENUM;
typedef VOS_UINT16 PHY_MONI_AGCH_FLG_ENUM_UINT16;


enum GAS_PHY_MODE_ENUM
{
    GAS_PHY_MODE_G          = 0,
    GAS_PHY_MODE_W          = 1,
    GAS_PHY_MODE_L          = 2,
    GAS_PHY_MODE_T          = 3,
    GAS_PHY_MODE_NONE       = 4,    /* GTL时，G为主模不关心谁是从模，配置该值 */
    GAS_PHY_MODE_BUTT
};
typedef VOS_UINT16 GAS_PHY_MODE_ENUM_UINT16;
enum GAS_PHY_BG_SEARCH_ENUM
{
    GAS_PHY_BG_SEARCH_START             = 0,
    GAS_PHY_BG_SEARCH_STOP              = 1,

    GAS_PHY_BG_SEARCH_BUTT
};
typedef VOS_UINT16 GAS_PHY_BG_SEARCH_ENUM_UINT16;


enum PHY_GAS_HO_RESULT_ENUM
{
    PHY_GAS_HO_RESULT_SUCCESS               = 0,        /* 切换成功 */
    PHY_GAS_HO_RESULT_TAOUTRANGE            = 1,        /* 切换失败: TA OUTRANGE */
    PHY_GAS_HO_RESULT_TIMEOUT               = 2,        /* 切换失败: TIMEOUT */
    PHY_GAS_HO_RESULT_OTHERS                = 3,        /* 切换失败: 其他原因值 */

    PHY_GAS_HO_RESULT_BUTT
};
typedef VOS_UINT16 PHY_GAS_HO_RESULT_ENUM_UINT16;



enum PHY_GAS_COMM_FLAG_ENUM
{
    PHY_GAS_COMM_FLAG_FALSE             = 0,        /* 不打开或不存在 */
    PHY_GAS_COMM_FLAG_TRUE              = 1,        /* 打开或存在 */

    PHY_GAS_COMM_FLAG_BUTT
};
typedef VOS_UINT16 PHY_GAS_COMM_FLAG_ENUM_UINT16;


enum PHY_GAS_STOP_MEAS_TYPE_ENUM
{
    PHY_GAS_STOP_MEAS_TYPE_ABNORMAL         = 0,        /* 异常停止测量 */
    PHY_GAS_STOP_MEAS_TYPE_NORMAL           = 1,        /* 正常停止测量 */

    PHY_GAS_STOP_MEAS_TYPE_BUTT
};
typedef VOS_UINT16 PHY_GAS_STOP_MEAS_TYPE_ENUM_UINT16;


typedef enum
{
    GAS_PHY_SYNC_VALID_TYPE_NOT_PRESENT                  =   0,         /* 同步参数不存在 */
    GAS_PHY_SYNC_VALID_TYPE_PRESENT_AND_USABLE           =   1,         /* 同步参数存在,且可用 */
    GAS_PHY_SYNC_VALID_TYPE_PRESENT_NOT_USABLE           =   2,         /* 同步参数存在,但不可用 */

    GAS_PHY_SYNC_VALID_TYPE_BUTT
}GAS_PHY_SYNC_VALID_TYPE_ENUM;
typedef VOS_UINT16 GAS_PHY_SYNC_VALID_TYPE_ENUM_UINT16;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


typedef struct
{
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_MsgChoice_Export      PHY_GAS_MSG_ID_ENUM_UINT16 */
    VOS_UINT8                               aucMsgBlock[4];
    /******************************************************************************************************
        _H2ASN_MsgChoice_When_Comment          PHY_GAS_MSG_ID_ENUM_UINT16
    ******************************************************************************************************/
}PHY_GAS_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    PHY_GAS_MSG_DATA                        stMsgData;
}PhyGasInterface_MSG;


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

/**************************5.1 NULL状态下的原语******************************/

/*****************************************************************************
 消息名称  : MPHC_NETWORK_LOST_REQ_STRU
 功能描述  : 在小区选择失败后，RR发该原语给L1。开机后和收到该原语使L1进入
             NULL状态，该状态为低功耗状态（该状态需要和DRX的几个状态统一考虑)。
 修改历史  :
  1.日    期   : 2007年09月29日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    GAS_PHY_NETLOST_CASUE_ENUM_UINT16       usCause;                            /* 0: NORMAL, 1: G Inactive */
} MPHC_NETWORK_LOST_REQ_STRU;


/*****************************************************************************
 消息名称  : MPHC_NETWORK_LOST_CNF_STRU
 功能描述  : MPHC_NETWORK_LOST_REQ的确认原语。
 修改历史  :
  1.日    期   : 2007年09月29日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRsv;
} MPHC_NETWORK_LOST_CNF_STRU;


/**********************5.2 Searching BCH状态下的原语*************************/

/*****************************************************************************
 消息名称  :  MPH_RXLEV_REQ_STRU
 功能描述  :  物理层在NULL和Searching BCH状态收到原语MPH_RXLEV_REQ复位数据结构，
              开始测量，该原语应用于小区选择过程中。
 修改历史  :
  1.日    期   : 2007年09月29日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usFreqBand;
    VOS_UINT16                              usFreqNum;                          /* 0~33，0表示需要搜索整个规定频段 */
    VOS_UINT16                              usFreqInfo[SEARCH_FREQ_BA_MAX_NUM];
} MPH_RXLEV_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_RXLEV_CNF_STRU
 功能描述  : 物理层在小区选择状态收到原语MPH_RXLEV_REQ 测量完所有载频后使用原语
              MPH_RXLEV_CNF上报测量结果。该原语应用于小区选择过程中。
 修改历史  :
  1.日    期    : 2007年09月10日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构TBD
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usFreqNum;
    RXLEV_MEAS_STRU                         astRxlevResult[SEARCH_SEND_FREQ_INFO_NUM];
} MPH_RXLEV_CNF_STRU;

#define MPH_BCCH_READ_ADDI_ACT_MASK_DEFAULT_ACTION                      (0x0000)
#define MPH_BCCH_READ_ADDI_ACT_MASK_NO_NEED_TO_SYNC                     (0x0001)
#define MPH_BCCH_READ_ADDI_ACT_MASK_NO_NEED_TO_LOCK_AFC                 (0x0002)

/*****************************************************************************
 消息名称   : MPH_START_BCCH_READING_STRU
 功能描述   : 在小区选择过程中，RR排列各个载频的电平后，RR发该原语给L1启动
              对该载频的同步并读取BCCH消息。
 修改历史   :
  1.日    期    : 2007年09月10日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usFreqInfo;
    VOS_UINT16                              usSIBitMap;
    VOS_UINT16                              usAddiActMask;                      /* bit0: 1:不必重新同步;    0:需要重新同步
                                                                                   bit1: 1:不必重新锁定AFC; 0:需要重新锁定AFC */
} MPH_START_BCCH_READING_STRU;


/*****************************************************************************
 消息名称  : MPH_CELL_SB_IND_STRU
 功能描述  : 在小区选择过程中，L1读取BCCH失败后反馈该消息给GAS
 修改历史  :
  1.日    期    : 2007年10月31日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usFreqInfo;
    VOS_UINT16                              usErrCode;                          /* 0－Success；1—SB Fail；2—AFC Fail */
    VOS_UINT16                              usBsic;
} MPH_CELL_SB_IND_STRU;


/*****************************************************************************
 消息名称  : MPH_STOP_BCCH_READING_STRU
 功能描述  : 物理层收到原语MPH_STOP_BCCH_READING停止读BCCH过程。该原语应用于
             小区选择过程中。
 修改历史  :
  1.日    期    : 2007年09月10日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usFreqInfo;
} MPH_STOP_BCCH_READING_STRU;


/**************************5.3 BCH状态下的原语*******************************/

/*****************************************************************************
 消息名称  : MPH_RA_REQ_STRU
 功能描述  : 发送接入请求
 修改历史  :
  1.日    期   : 2007年9月28日
    作    者   : fangjian (48555)
    修改内容   : 修改原有结构名PH_RA_REQ为MPH_RA_REQ_STRU
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    GAS_PHY_ABType_ENUM_UINT16              usABType;
    GAS_PHY_RA_ENUM_UINT16                  usTsc;                              /* 8PSK调制用TSC1；GMSK调制用TSC0 */
    VOS_UINT16                              usTXPwr;
    VOS_UINT16                              usPowerClassGSM;
    VOS_UINT16                              usPowerclassDCS;
    VOS_UINT16                              usRand;
    VOS_UINT16                              usChanReq;
} MPH_RA_REQ_STRU;


/*****************************************************************************
 消息名称  : PH_RA_REQ_STRU
 功能描述  : 发送接入请求
 修改历史  :
  1.日    期   : 2007年9月28日
    作    者   : fangjian (48555)
    修改内容   : 修改原有结构名PH_RA_REQ为MPH_RA_REQ_STRU
*****************************************************************************/
/* 为了兼容Moiri的定义 */
typedef MPH_RA_REQ_STRU PH_RA_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRASuccess;                        /* 0：失败，1：成功 */
    VOS_UINT32                              ulFN;
    GAS_PHY_ABType_ENUM_UINT16              usRaBurstType;
    VOS_UINT16                              usChanReq;
} MPH_RA_CNF_STRU;


/*****************************************************************************
 消息名称  : PH_RA_CNF_STRU
 功能描述  : 对消息 PH_RA_REQ_STRU 的回复
 修改历史  :
  1.日    期   : 2007年9月28日
    作    者   : fangjian (48555)
    修改内容   : 修改原有结构名PH_RA_CNF_STRU为MPH_RA_CNF_STRU
*****************************************************************************/
/* 为了兼容Moiri的定义 */
typedef MPH_RA_CNF_STRU PH_RA_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usFreInfo;
    GAS_PHY_SYNC_VALID_TYPE_ENUM_UINT16     usSyncValid;                        /* 下面的同步信息是否有效 */
    VOS_UINT16                              usTimeAlignment;                    /* 新小区和原来小区之间的QNOFFSET差，模5000 */
    VOS_UINT32                              ulFnOffset;                         /* 新小区和原来小区之间的FN差，模2715648*/
    VOS_UINT16                              usBSIC;
    VOS_UINT16                              usRsv;                              /* 四字节对齐 */
} MPH_CELL_RESELECT_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_BA_LIST_STRU
 功能描述  : 在BCH态，MS需要监视并读出6个最强邻区信号，L3通过MPH_BA_LIST原语
             告诉L1 BA列表，L1把它存在内存中，然后开始周期性测量。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构TBD
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usNumOfChans;
    VOS_UINT16                              usRsv;                              /* 保留位 */
    VOS_UINT16                              ausA[32];
} BCCH_LIST_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRsv;                              /* 保留位 */
    BCCH_LIST_STRU                          stBCCHAllocation;
} MPH_BA_LIST_STRU;


/*****************************************************************************
 消息名称  : MPH_BA_LIST_CNF_STRU
 功能描述  : MPH_BA_LIST的确认原语。
 修改历史  :
  1.日    期   : 2007年10月06日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRsv;
} MPH_BA_LIST_CNF_STRU;

/*****************************************************************************
 消息名称  : MPH_SCELL_BCCH_READ_STRU
 功能描述  : L3用原语MPH_SCELL_BCCH_READ请求L1读服务小区BCCH上的系统消息，
             L1读出后通过原语PH_DATA_IND上报L3。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usSIBitMap;
} MPH_SCELL_BCCH_READ_STRU;


/*****************************************************************************
 消息名称  : MPH_SCELL_BCCH_READ_CNF_STRU
 功能描述  : L3用原语MPH_SCELL_BCCH_READ请求L1读服务小区BCCH上的系统消息，
             该原语是MPH_SCELL_BCCH_READ的回复。
 修改历史  :
  1.日    期   : 2007年10月31日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usERRCode;
} MPH_SCELL_BCCH_READ_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_FULL_LIST_REQ_STRU
 功能描述  : L3用原语MPH_FULL_LIST请求L1测量全部载波，该测量可以和周期测量同时存在。
 修改历史  :
  1.日    期   : 2007年09月07日
    作    者   : fangjian(48555)
    修改内容   : 合并MPH_BAND_SELECT_REQ_STRU的内容
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usBandSelect;                       /* MS支持的GSM频段 */
    VOS_UINT16                              ausMSClass[5];                      /* 各频段对应的功率等级 */
    VOS_UINT16                              usRsv;                              /* 保留字段 */
} MPH_FULL_LIST_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_FULL_LIST_CNF_STRU
 功能描述  : 该原语是L1对于MPH_FULL_LIST的确认。
 修改历史  :
  1.日    期   : 2007年09月14日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usERRCode;                          /* 错误码 */
} MPH_FULL_LIST_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_SI_UPDATE_REQ_STRU
 功能描述  : L3用原语MPH_SI_UPDATE_REQ请求L1更新系统消息，用原语
             MPH_SI_UPDATE_CNF确认。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
  2.日    期    : 2007年11月01日
    作    者    : fangjian(48555)
    修改内容    : 根据物理层需求将usPAGE_GROUP修改为usPAGE_MULTIFRAME51_INDEX
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usChannelType;                      /* 1=TCH_F, 2=TCH_H, 3=SDCCH_4, 4=SDCCH_8 */
    VOS_UINT16                              usSubchannel;
    VOS_UINT16                              usTimeslotNumber;
    VOS_UINT16                              usTSC;
    VOS_UINT16                              usHOPING;
    VOS_UINT16                              usFreInfo;
    VOS_UINT16                              usMAIO;
    VOS_UINT16                              usHSN;
} CHANNEL_DESCRIPTION_STRU;


typedef struct
{
    VOS_UINT16                              usBS_PA_MFRMS;
    VOS_UINT16                              usBS_AG_BLOCK_RES;
    VOS_UINT16                              usBS_CC_CHANS;
    VOS_UINT16                              usBS_CCCH_SDCCH_COMB;
    VOS_UINT16                              usCCCH_GROUP;
    VOS_UINT16                              usPAGE_MULTIFRAME51_INDEX;          /* 寻呼组在第几个51复帧, 45.002 6.5.3 */
    VOS_UINT16                              usPAGE_BLOCK_INDEX;
    VOS_UINT16                              usMS_TXPWR_MAX_CCH;
    VOS_UINT16                              usLB_MS_TXPWR_MAX_CCH;              /* 0xFFFF表示无效 */
    VOS_UINT16                              usPOWER_OFFSET;
    VOS_UINT16                              usPowerClassGSM;
    VOS_UINT16                              usPowerclassDCS;
    VOS_UINT16                              usTX_Integer;
    VOS_UINT16                              usSplitPageCycle;
    VOS_UINT16                              usCBCHINFOVALID;
    VOS_UINT16                              usReserved;                         /* 四字节对齐保留 */
    CHANNEL_DESCRIPTION_STRU                stCHCHINFO;
} SI_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRsv;                              /* 保留位 */
    SI_STRU                                 stSIInfo;
} MPH_SI_UPDATE_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_SI_UPDATE_CNF_STRU
 功能描述  : L3用原语MPH_ SI_UPDATE_REQ请求L1更新系统消息，用原语
             MPH_ SI_UPDATE_CNF确认。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usERRCode;
} MPH_SI_UPDATE_CNF_STRU;


/************************5.4  DCH状态下的原语********************************/

/*****************************************************************************
 消息名称  : MPH_IMMED_ASSIGN_REQ_STRU
 功能描述  : L1接收L3发来的原语MPH_IMMED_ASSIGN_REQ，该消息包含有信道的全部消息，
             L1立即激活该信道，因为前面没有信道。该原语指配的信道只能是信令信道。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
  2.日    期   : 2007年10月12日
    作    者   : fangjian(48555)
    修改内容   : 增加Before time相关的信道结构和标志位
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usRFChanCnt;
    VOS_UINT16                              usRsv;
    VOS_UINT16                              ausMA[63+1];
} MOBILE_ALLOCATION_STRU;

typedef struct
{
    VOS_UINT16                              usStartTimePresent;
    VOS_UINT16                              usT1;
    VOS_UINT16                              usT2;
    VOS_UINT16                              usT3;
} STARTING_TIME_STRU;

typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usTA;
    VOS_UINT16                              usDTX;
    VOS_UINT16                              usBeforeChanMark;                   /* 0:不存在，1:存在 */
    CHANNEL_DESCRIPTION_STRU                stBeforeChanDesc;                   /* BEFORE TIME的信道描述 */
    MOBILE_ALLOCATION_STRU                  stBeforeFreqList;                   /* BEFORE TIME的频率列表 */
    CHANNEL_DESCRIPTION_STRU                stChanDesc;
    MOBILE_ALLOCATION_STRU                  stFreqList;
    STARTING_TIME_STRU                      stStartTime;
    BCCH_LIST_STRU                          stBCCHAllocation;
} MPH_IMMED_ASSIGN_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_IMMED_ASSIGN_CNF_STRU
 功能描述  : 对消息MPH_IMMED_ASSIGN_REQ的回复
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usERRCode;
} MPH_IMMED_ASSIGN_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_CHANNEL_ASSIGN_REQ_STRU
 功能描述  : L1接收L3发来的原语MPH_CHANNEL_ASSIGN_REQ，该消息包含有信道的全部
             消息，L1立即激活该信道，该原语指配的信道可以是任何信道。该原语使
             用的前提是MS已经工作在一些专用信道上。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_INT16                               sThreshold;
    VOS_INT16                               sHysteresis;
} AMRPARA;

typedef struct
{
    VOS_UINT16                              usLength;
    VOS_UINT16                              usMRVersion;
    VOS_UINT16                              usNSCB;
    VOS_UINT16                              usICMI;
    VOS_UINT16                              usStartMode;
    VOS_UINT16                              usSetofAMRCodecModes;
    AMRPARA                                 ustAmrPara[3];
} AMR_CONFIG_STRU;

typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    GAS_PHY_CHANNEL_MODE_ENUM_UINT16        usChanMode;                         /* 0-Signalling Only,1-TCH/FS,2-TCH/EFR,3-TCH/AFS,4-TCH/F24
                                                                                   5-TCH/F48,6-TCH/F96,7-TCH/F144,8-TCH/HS,9-TCH/AHS,
                                                                                   10-TCH/H24,11-TCH/H48, 12-TCH/WFS */
    VOS_UINT16                              usTXPwr;
    VOS_UINT16                              usCipherMode;
    VOS_UINT16                              ausCipherKey[4];
    VOS_UINT16                              usDTX;
    VOS_UINT16                              usBeforeChanMark;                   /* 0:不存在，1:存在 */
    CHANNEL_DESCRIPTION_STRU                stBeforeChanDesc;                   /* BEFORE TIME的信道描述 */
    MOBILE_ALLOCATION_STRU                  stBeforeFreqList;                   /* BEFORE TIME的频率列表 */
    STARTING_TIME_STRU                      stStartTime;
    CHANNEL_DESCRIPTION_STRU                stChanDesc;                         /* AFTER TIME的信道描述 */
    MOBILE_ALLOCATION_STRU                  stFreqList;                         /* AFTER TIME的频率列表 */
    AMR_CONFIG_STRU                         stAMR_CONFIG;
} MPH_CHANNEL_ASSIGN_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_CHANNEL_ASSIGN_CNF_STRU
 功能描述  : 消息MPH_CHANNEL_ASSIGN_REQ的回复
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usERRCode;
} MPH_CHANNEL_ASSIGN_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_HO_REQ_STRU
 功能描述  : L1可以接收原语MPH_ HO_REQ表示同步切换、预同步切换、伪同步切换和
             异步切换。前三种切换L1发出4个接入burst，后一种异步切换L1发接入burst
             直到接入完成（异步切换时，发送一个RA后给RR回MPH_ASYNC_HO_CNF。
             如果在FACCH上收到PHYSICAL INFORMATION消息后或定时器T3124到时，
             RR给L1发MPH_ABORT_HO_RA_REQ），L1发送完接入burst后，回原语
             MPH_HANDOVER_FINISHED给L3。
 修改历史  :
  2.日    期   : 2007年09月14日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usNCC;
    VOS_UINT16                              usBCC;
    VOS_UINT16                              usBCCHARFCN;
} CELL_DESC_STRU;

typedef struct
{
    VOS_UINT16                              usBSIC;
    VOS_UINT16                              usBCCHARFCN;
    VOS_UINT16                              usChanMode;                         /* 0-Signalling Only,1-TCH/FS,2-TCH/EFR,3-TCH/AFS,4-TCH/F24
                                                                                   5-TCH/F48,6-TCH/F96,7-TCH/F144,8-TCH/HS,9-TCH/AHS,
                                                                                   10-TCH/H24,11-TCH/H48, 12-TCH/WFS */
    VOS_UINT16                              usHOAcc;
    VOS_UINT16                              usTXPwr;
    VOS_UINT16                              usCipherMode;
    VOS_UINT16                              ausCipherKey[4];
    VOS_UINT16                              usBeforeChanMark;                   /* 0:不存在，1:存在 */
    VOS_UINT16                              usRsv;
    CHANNEL_DESCRIPTION_STRU                stBeforeChanDesc;                   /* BEFORE TIME的信道描述 */
    MOBILE_ALLOCATION_STRU                  stBeforeFreqList;                   /* BEFORE TIME的频率列表 */
    STARTING_TIME_STRU                      stStartTime;
    CHANNEL_DESCRIPTION_STRU                stAfterChanDesc;                    /* AFTER TIME的信道描述 */
    MOBILE_ALLOCATION_STRU                  stAfterFreqList;                    /* AFTER TIME的频率列表 */
} HO_PARAMS_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usHOType;                           /* 切换类型: 1-ASYNC, 2-SYNC
                                                                                   4-PRE_SYNC, 8-PSEUDO_SYNC */
    HO_PARAMS_STRU                          stHOCom;
    VOS_UINT32                              ulFNOffset;
    VOS_UINT16                              usTimeAlignmt;                      /* 物理层最新文档缺该参数，待确认 */
    VOS_UINT16                              usTA;
    VOS_UINT16                              usNCI;
    VOS_UINT16                              usRTDValid;
    VOS_UINT16                              usRTD;
    GAS_PHY_SYNC_VALID_TYPE_ENUM_UINT16     usSyncValid;
    AMR_CONFIG_STRU                         stAMR_CONFIG;
} MPH_HO_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /*消息ID，MPH_BHO_NCELL_FB_SB_REQ*/
    VOS_UINT16                              usFreqInfo;                         /* 小区频点，包含频段MARK*/
} MPH_BHO_NCELL_FB_SB_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /*消息ID，MPH_BHO_NCELL_FB_SB_CNF */
    VOS_UINT16                              usFreqInfo;                         /* 小区频点，包含频段MARK */
    VOS_UINT16                              usSBFoundFlag;                      /* 1，表示发现SB；0，表示解调SB失败 */
    VOS_UINT16                              usBSIC;                             /* 解调出的BSIC */
    VOS_UINT32                              ulFNOffset;                         /* 帧偏差 */
    VOS_UINT16                              usTimeAlignmt;                      /* 时间偏差 */
    VOS_UINT16                              usRsv;                              /* 填充字段 */
} MPH_BHO_NCELL_FB_SB_CNF_STRU;



typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /*消息ID，MPH_BHO_NCELL_FB_SB_CNF */
    VOS_UINT16                              usFreqInfo;                         /* 小区频点，包含频段MARK */
} MPH_STOP_BHO_NCELL_FB_SB_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /*消息ID，MPH_BHO_NCELL_FB_SB_CNF */
    VOS_UINT16                              usRsved;                            /* 填充字段*/
} MPH_STOP_BHO_NCELL_FB_SB_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_ASYNC_HO_CNF_STRU
 功能描述  : 异步切换发送一个RA burst后给RR回MPH_ASYNC_HO_CNF
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usERRCause;
} MPH_ASYNC_HO_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_ABORT_HO_RA_REQ_STRU
 功能描述  : 在非同步切换中，RR收到对等实体的物理消息或定时器超时，命令L1停止
             发送RA，停止读BCCH和CCCH，同时应用TA值给现在的信道。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usAbortCause;
    VOS_UINT16                              usTA;
    VOS_UINT16                              usRsv;
} MPH_ABORT_HO_RA_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_HANDOVER_FINISHED_STRU
 功能描述  : L1发送完接入burst后，回原语MPH_HANDOVER_FINISHED给L3
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usHOType;
    VOS_UINT16                              usOTA;
    PHY_GAS_HO_RESULT_ENUM_UINT16           enERRCause;                         /* 切换结果 */
} MPH_HANDOVER_FINISHED_STRU;


/*****************************************************************************
 消息名称  : MPH_HANDOVER_COMPLETE_STRU
 功能描述  : RR收到LAPDm的DL_RESUME_CNF后, 通知物理层切换已经结束,可以删除保存的历史信道的信息
 修改历史  :
  1.日    期   : 2006年03月06日
    作    者   : zhanghongjun(49106)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usCause;                            /* 保留字段, 目前暂不使用, 目前只填0x00*/
} MPH_HANDOVER_COMPLETE_STRU;


/*****************************************************************************
 消息名称  : MPH_HANDOVER_FAIL_REQ_STRU
 功能描述  : L1可以接收原语MPH_HANDOVER_FAIL_REQ回退到切换前的信道。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usBSIC;
    VOS_UINT16                              usBCCHARFCN;
    VOS_UINT16                              usChanMode;                         /* 0-Signalling Only,1-TCH/FS,2-TCH/EFR,3-TCH/AFS,4-TCH/F24
                                                                                   5-TCH/F48,6-TCH/F96,7-TCH/F144,8-TCH/HS,9-TCH/AHS,
                                                                                   10-TCH/H24,11-TCH/H48, 12-TCH/WFS */
    VOS_UINT16                              usTXPwr;
    VOS_UINT16                              usCipherMode;
    VOS_UINT16                              ausCipherKey[4];
    VOS_UINT16                              usDTX;
    VOS_UINT16                              usTA;
    VOS_UINT16                              usRsv ;
    VOS_UINT16                              usTimeAlignmt ;                     /* 0xFFFF=无效值 */
    VOS_UINT32                              ulFNOffset ;                        /* 0xFFFFFF=无效值 */
    CHANNEL_DESCRIPTION_STRU                stChanDesc;
    MOBILE_ALLOCATION_STRU                  stFreqList;
    STARTING_TIME_STRU                      stStartTime;
    AMR_CONFIG_STRU                         stAMR_CONFIG;
} MPH_HANDOVER_FAIL_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_HANDOVER_FAIL_CNF_STRU
 功能描述  : 对消息MPH_HANDOVER_FAIL_REQ的回复。注意这里的成功仅仅是物理层对
             该信道激活成功，并不表示信道回退成功。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usERRCode;
} MPH_HANDOVER_FAIL_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_CHANNEL_MODE_MODIFY_REQ_STRU
 功能描述  : L1可以接收原语MPH_CHANNEL_MODE_MODIFY_REQ修改专用信道模式。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usChanMode;
    VOS_UINT16                              usDTX;
    VOS_UINT16                              usRsv;
    CHANNEL_DESCRIPTION_STRU                stChanDesc;
    AMR_CONFIG_STRU                         stAMRInterface;
} MPH_CHANNEL_MODE_MODIFY_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_CHANNEL_MODE_MODIFY_CNF_STRU
 功能描述  : 对MPH_CHANNEL_MODE_MODIFY_REQ的回复
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usERRCode;
} MPH_CHANNEL_MODE_MODIFY_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_SET_CIPHERING_REQ_STRU
 功能描述  : 改变信道加密模式
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usCipherMode;
    VOS_UINT16                              ausCipherKey[4];
} MPH_SET_CIPHERING_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_SET_CIPHERING_CNF_STRU
 功能描述  : 对消息MPH_SET_CIPHERING_REQ的回复
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usERRCode;
} MPH_SET_CIPHERING_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_CHANGE_FREQUENCY_REQ_STRU
 功能描述  : 改变跳频频率
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usTransactionID;
    STARTING_TIME_STRU                      stStartTime;
    CHANNEL_DESCRIPTION_STRU                stChanDesc;
    MOBILE_ALLOCATION_STRU                  stFreqList;
} MPH_CHANGE_FREQUENCY_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_CHANGE_FREQUENCY_CNF_STRU
 功能描述  : 对消息MPH_CHANGE_FREQUENCY_REQ的回复
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usTransactionID;
    VOS_UINT16                              usERRCode;
    VOS_UINT16                              usRsv;
} MPH_CHANGE_FREQUENCY_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_STOP_DEDICATED_REQ_STRU
 功能描述  : L1可以接收原语MPH_STOP_DEDICATED_REQ释放专用信道，如果指示BCCH
             可以使用，就附着在该BCCH上，否则等待RR通知启动小区搜索以搜索到
             合适信道。
 修改历史  :
  1.日    期   : 2007年09月10日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usBCCHFreValid;
    VOS_UINT16                              usFreqInfo;
    VOS_UINT16                              usBSIC;
} MPH_STOP_DEDICATED_REQ_STRU;



typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usERRCode;
    VOS_UINT16                              usTA;                               /* 异系统切换失败后，回退到原信道时使用 */
    VOS_UINT16                              usTimeAlignmt;
    VOS_UINT32                              ulFNOffset;
} MPH_STOP_DEDICATED_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_CS_DEDICATED_MEAS_IND_STRU
 功能描述  : 上报测量电平结果。L3可以通过MPH_BA_LIST原语告诉L1更改BA列表，L1
             停止现在的测量，更新后重新初始化测量数据，开始测量。
 修改历史  :
  1.日    期    : 2007年09月11日
    作    者    : jiajing(55331)
    修改内容    : 删除usNumFullBlkMeas，usNumSubBlkMeas，usNumBitMeasFull，usNumBitMeasSub
                   物理层直接上报质量和电平测量等级,增加邻区BSIC信息
  2.日    期    : 2007年10月12日
    作    者    : fangjian(48555)
    修改内容    : 增加usWMeasMark标志
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usArfcn;                            /* 邻小区的载频 */
    VOS_UINT16                              usRxlev;                            /* 邻小区的信号测量值 */
    VOS_UINT16                              usSyncMark;                         /* 该邻小区是否存在同步信息 1:存在 0:不存在 */
    VOS_UINT16                              usBsic;                             /* 邻小区的BSIC */
    VOS_UINT32                              ulFNOffset;                         /* 邻小区与当前服务小区的帧偏差 */
    VOS_UINT16                              usTimeAlignmt;                      /* 邻小区与当前服务小区时间偏差 */
    VOS_INT16                               sOrder;                             /* 邻小区的排序结果. */
} GSM_STRONG_NCELL_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usDTX;
    VOS_UINT16                              usRxlevFull;                        /* 信号强度+偏置(316) */
    VOS_UINT16                              usRxlevSub;                         /* 信号强度+偏置(316) */
    VOS_UINT16                              usFrmNumFull;                       /* FULL集的电平统计帧数 */
    VOS_UINT16                              usFrmNumSub;                        /* SUB集的电平统计帧数 */
    VOS_UINT16                              usErrBitsFull;                      /* FULL集的误码BIT数 */
    VOS_UINT16                              usErrBitsSub;                       /* SUB集的误码BIT数 */
    VOS_UINT16                              usTotalBitsFull;                    /* FULL集总的比特数 */
    VOS_UINT16                              usTotalBitsSub;                     /* SUB集总的比特数 */
    VOS_UINT16                              usRxLevVal;
    VOS_UINT16                              usMeanBep;
    VOS_UINT16                              usCvBep;
    VOS_UINT16                              usNbrRcvdBlocks;
    VOS_UINT16                              usRsv;
    VOS_UINT16                              usNChanNum;
    GSM_STRONG_NCELL_STRU                   astNcellInfo[GSM_STRONGEST_FREQLIST_NUM];   /* 测量上报的相关邻区信息 */
} MPH_CS_DEDICATED_MEAS_IND_STRU;


/*****************************************************************************
 消息名称  : MPH_CHANGE_DTX_REQ_STRU
 功能描述  : 在专用模式，RR请求改变DTX方式。用于配置物理层
 修改历史  :
  1.日    期   : 2007年09月23日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usDTX;
} MPH_CHANGE_DTX_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_CHANGE_DTX_CNF_STRU
 功能描述  : 在专用模式，RR请求改变DTX方式，以该原语应答
 修改历史  :
  1.日    期   : 2007年09月23日
    作    者   : fangjian(48555)
    修改内容   : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usDTX;
} MPH_CHANGE_DTX_CNF_STRU;


/******************************5.6 新增的原语********************************/

/*****************************************************************************
 消息名称  : MPH_PAGING_MODE_REQ_STRU
 功能描述  : 该原语用于控制物理层修改寻呼模式，和通知物理层驻留目标小区。
 修改历史  :
  1.日    期    : 2007年09月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
  2.日    期    : 2007年10月12日
    作    者    : fangjian(48555)
    修改内容    : 根据物理层需求将usPAGE_GROUP修改为usPAGE_MULTIFRAME51_INDEX
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    GSM_PAGE_MODE_ENUM_UINT16               enPageMode;                         /* 寻呼模式 */
    VOS_UINT16                              usBS_PA_MFRMS;                      /* 寻呼周期中51复帧的数量 */
    VOS_UINT16                              usBS_AG_BLKS_REGS;                  /* 每个CCCH上AGCH保留块数 */
    VOS_UINT16                              usBCCH_COMBINED;                    /* CCCH是否与SDCCH结合,0不结合，1结合 */
    VOS_UINT16                              usCCCH_GROUP;                       /* CCCH_GROUP，45.002 6.5.2,[0 .. BS_CC_CHANS-1] */
    VOS_UINT16                              usPAGE_MULTIFRAME51_INDEX;          /* 寻呼组在第几个51复帧, 45.002 6.5.3 */
    VOS_UINT16                              usPAGE_BLOCK_INDEX;                 /* 51复帧中PAGE BLOCK 的INDEX */
} MPH_PAGING_MODE_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_PAGING_MODE_CNF_STRU
 功能描述  : 该原语用于物理层对于协议栈MPH_PAGING_MODE_REQ原语的回复
 修改历史  :
  1.日    期    : 2007年09月07日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    GSM_PAGE_MODE_ENUM_UINT16               enPageMode;                         /* 寻呼模式 */
} MPH_PAGING_MODE_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_UPDATE_PARSE_PAGING_PARA_REQ_STRU
 功能描述  : 该原语用于
 修改历史  :
  1.日    期    : 2007年09月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usReserved;
    VOS_UINT32                              ulTlli;                             /* 下发最新的TLLI值 */
    VOS_UINT16                              usTmsiFlag;                         /* TMSI存在标记,1:valid,0:invalid */
    VOS_UINT16                              usPtmsiFlag;                        /* PTMSI存在标记,1:valid,0:invalid */
    VOS_UINT32                              ulTmsi;
    VOS_UINT32                              ulPtmsi;
    VOS_UINT16                              usImsiFlag;                         /* IMSI存在标记,1:valid,0:invalid */
    VOS_UINT16                              usImsiLen;                          /* IMSI 占用的长度 */
    VOS_UINT16                              ausImsi[GSM_IMSI_MAX_LEN];          /* IMSI  */
    VOS_UINT16                              ausImei[GSM_IMEI_LEN];              /* IMEI 占用的长度 */
    VOS_UINT16                              ausImeiSV[GSM_IMEISV_LEN];          /* IMEISV 占用的长度 */
    VOS_UINT16                              usDscInitValueCS;                   /* CS的DSC初始值 */
    VOS_UINT16                              usDscInitValuePS;                   /* PS的DSC初始值 */
} MPH_UPDATE_PARSE_PAGING_PARA_REQ_STRU;


/*****************************************************************************
 消息名称  :  MPH_UPDATE_PARSE_PAGING_PARA_CNF_STRU
 功能描述  :  MPH_UPDATE_PARSE_PAGING_PARA_REQ的确认原语。
 修改历史  :
  1.日    期    : 2007年10月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRsv;
} MPH_UPDATE_PARSE_PAGING_PARA_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_UPDATE_RESEL_PARA_REQ_STRU
 功能描述  : 该原语用于向物理层更新GSM/GPRS小区重选相关参数
 修改历史  :
  1.日    期    : 2007年09月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usArfcn;                            /* 邻小区频点 */
    VOS_INT16                               sNcellC1Value;                      /* 邻小区C1值 */
    VOS_INT16                               sNcellC31Value;                     /* 邻小区C31值 */
    VOS_INT16                               sNcellC32Value;                     /* 邻小区C2值、邻小区C32值 */
    VOS_INT16                               sReselOffset;                       /* 用于存在PBCCH信道下的重选 */
    VOS_UINT16                              usPriorityClass;                    /* 用于存在PBCCH信道下的重选 */
} GSM_NCELL_C_THRESHOLD_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usScellCValueFlag;
    VOS_UINT16                              usCValueMark;
    VOS_INT16                               sScellC1Value;                      /* 服务小区C1值 */
    VOS_INT16                               sScellC31Value;                     /* 服务小区C31值 */
    VOS_INT16                               sScellC32Value;                     /* 服务小区C2或C32值 */
    VOS_UINT16                              usC32Qual;                          /* 存在PBCCH信道时 */
    VOS_UINT16                              us3GSearchPrio;                     /* 3G测量是否优先参数,0:2G的BSIC验证优先于3G测量，1:3G测量优先于2G的BSIC验证 */
    VOS_UINT16                              usSCellPriority;                    /* 服务小区优先级 */
    VOS_UINT16                              usLteSearchPrio;                    /* Lte测量是否优先参数,0:2G的BSIC验证优先于Lte测量，1:Lte测量优先于2G的BSIC验证 */
    VOS_UINT16                              usBand18001900Indicator;            /* 0：1800频段,1：1900频段 */
    VOS_UINT16                              usMsSupportedBand;                  /* 手机系统支持的频段 */
    VOS_UINT16                              usMultiBandMode;                    /* 系统支持的频段模式 */
    VOS_UINT16                              usNccPermitted;                     /* 网络允许的NCC掩码 */
    VOS_UINT16                              usNcMode;
    VOS_UINT16                              usGmmState;
    VOS_UINT16                              usMeasOrder;
    VOS_UINT16                              usNcellArraySize;                   /* 邻小区个数 */

    /* A threshold below which the MS is allowed to reselect to lower priority layers,
       0 = 0 dB, 1 = 2 dB, 2 = 4 dB, …, 13 = 26 dB, 14 = 28 dB, 15 = infinite (always). */
    /* 允许MS重选到低优先级小区的门限，默认值为0,GAS下发的值为转换为电平值之前的值[0,15] */
    VOS_UINT16                              usThreshGsmLow;
    VOS_UINT16                              usRsv;                              /* GAS和GDSP之间的接口保留位应使用U16 */
    GSM_NCELL_C_THRESHOLD_STRU              astStrongNCellArray[GSM_STRONGEST_FREQLIST_NUM];    /* 邻小区的C值是否有改变 */
} MPH_UPDATE_RESEL_PARA_REQ_STRU;


/*****************************************************************************
 消息名称  :  MPH_UPDATE_RESEL_PARA_CNF_STRU
 功能描述  :  MPH_UPDATE_RESEL_PARA_REQ的确认原语。
 修改历史  :
  1.日    期    : 2007年10月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRsv;
} MPH_UPDATE_RESEL_PARA_CNF_STRU;



typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              us3GSearchPrio;                     /* 3G测量是否优先参数,0:2G的BSIC验证优先于3G测量，1:3G测量优先于2G的BSIC验证 */
    VOS_UINT16                              usBand18001900Indicator;            /* 0：1800频段,1：1900频段 */
    VOS_UINT16                              usMsSupportedBand;                  /* 手机系统支持的频段 */
    VOS_UINT16                              usMultiBandMode;                    /* 系统支持的频段模式 */
    VOS_UINT16                              usNccPermitted;                     /* 网络允许的NCC掩码 */
    VOS_UINT16                              usPwrc;                             /* Power control indicator */
    VOS_UINT16                              usLteSearchPrio;                    /* Lte测量是否优先参数,0:2G的BSIC验证优先于Lte测量，1:Lte测量优先于2G的BSIC验证 */
} MPH_UPDATE_DEDICATED_CTRL_PARA_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRsv;
} MPH_UPDATE_DEDICATED_CTRL_PARA_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_CELL_INFO_REPORT_REQ_STRU
 功能描述  : 该原语用于物理层执行小区重选预判决后以及邻区列表更新后，将结果通知协议栈
 修改历史  : MPH_CELL_RESELECT_IND,MPH_UPDATE_STRONG_NCELL_LIST_IND合并而成
  1.日    期    : 2007年09月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usReserved;
    VOS_UINT32                              u1ActionMask;                       /* 指示物理层根据Mask判断是上报服务小区强度、
                                                                                   NC测量报告(包括干扰测量)、tunning态测量等 */
} MPH_CELL_INFO_REPORT_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_CELL_INFO_REPORT_IND_STRU
 功能描述  : 该原语用于物理层执行小区重选预判决后以及邻区列表更新后，将结果通知协议栈
 修改历史  : MPH_CELL_RESELECT_IND,MPH_UPDATE_STRONG_NCELL_LIST_IND合并而成
  1.日    期    : 2007年09月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usArfcn;                            /* 服务小区频点号 */
    VOS_UINT32                              ulActionMask;                       /* 物理层上报原语动作类型 */
    VOS_UINT16                              usScellRxlev;                       /* 服务小区测量值 */
    VOS_UINT16                              usC;                                /* C值，范围0~63 */
    VOS_UINT16                              usTimerStatus;                      /* 重选定时器状态,7bit：服务小区C1定时器,5bit：上报的第一个邻小区C2定时器 */
    VOS_UINT16                              usTargetArfcn;                      /* 主动重选的目标小区频点 */
    VOS_UINT16                              usReadyMark;                        /* 邻小区是否已经构建好 */
    VOS_UINT16                              usStrongNCellNum;                   /* 物理层维护的最强邻小区个数，每次都报 */
    GSM_STRONG_NCELL_STRU                   astStrongNCellArray[GSM_STRONGEST_FREQLIST_NUM];
} MPH_CELL_INFO_REPORT_IND_STRU;



typedef struct
{
    VOS_UINT16                              usPrimaryScramCode;                 /* 小区的主扰码,9bit,0xFFFF标识没有 */
    VOS_UINT16                              usDiversityInd;                     /* 分集指示,0:not applied,1:applied */
} WCELL_INFO_STRU;


typedef struct
{
    VOS_UINT16                              usArfcn;                            /* 频点信息 [0，16383] */
    VOS_UINT16                              usBandInfo;                         /* 频段信息 [0，9] */
    VOS_UINT16                              usCellNum;                          /* 一个频点下的小区数目,[1，32] */
    VOS_UINT16                              usNonNCellMeasFlg;
    WCELL_INFO_STRU                         astWCellInfo[MAX_INTER_FREQ_INFO_NUM];
} WCELL_INFO_LIST_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usInterFreqNum;                     /* 需测量的WCDMA频点数目，[0，3] */
    WCELL_INFO_LIST_STRU                    astInterFreqInfo[MAX_INTER_CARRIER_NUM];
} MPH_WCDMA_MEAS_REQ_STRU;



typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRsv;
    VOS_UINT32                              ulFrameNum;
} MPH_WCDMA_MEAS_CNF_STRU;


/*****************************************************************************
 消息名称  :  MPH_WCDMA_MEAS_IND_STRU
 功能描述  :  l1进行WCDMA测量后的结果上报
 修改历史  :
  1.日    期    : 2007年09月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usPrimaryScramCode;                 /* 小区的主扰码,9bit,0xFFFF标识没有 */
    VOS_INT16                               sCpichRscp;                         /* CPICH RSCP测量值,[-116，-25] */
    VOS_INT16                               sCpichEcN0;                         /* CPICH EcN0测量值,[-50，0] ，单位0.5dbm*/
} WCELL_MEAS_RESULT_STRU;


typedef struct
{
    VOS_UINT16                              usArfcn;                            /* 频点信息 [0，16383]*/
    VOS_INT16                               sCarrierRSSI;                       /* UtraCarrierRSSI测量值 */
    VOS_UINT16                              usCellNum;                          /* 一个频点下的小区数目 */
    WCELL_MEAS_RESULT_STRU                  astInterFreqInfo[MAX_INTER_FREQ_INFO_NUM];
} INTER_CELL_REPORT_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usInterFreqNum;                     /* 上报的WCDMA频点数目，[0，3] */
    INTER_CELL_REPORT_STRU                  astInterFreqReport[MAX_INTER_CARRIER_NUM];
} MPH_WCDMA_MEAS_IND_STRU;


/*****************************************************************************
 消息名称  :  MPH_NC_MEAS_REPORT_REQ_STRU
 功能描述  :  该原语用于指示物理层根据Mask判断是上报服务小区强度、NC测量报告(包括干扰测量)、tunning态测量等
 修改历史  :
  1.日    期    : 2007年09月06日
    作    者    : jiajing(55331)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usReserved;
    VOS_UINT32                              ulActionMask;
} MPH_NC_MEAS_REPORT_REQ_STRU;


/*****************************************************************************
 消息名称  :  MPH_NC_MEAS_REPORT_IND_STRU
 功能描述  :  该原语用于物理层上报协议栈NC测量结果
 修改历史  :
  1.日    期    : 2007年09月06日
    作    者    : jiajing(55331)
    修改内容    : 新生成结构
  2.日    期    : 2007年10月12日
    作    者    : fangjian(48555)
    修改内容    : 增加usWMeasMark标志
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                                    /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usScellRxlev;                               /* 服务小区电平 */
    VOS_UINT16                              usPccchIntRxlev;                            /* 空闲态上报PCCCH干扰测量 */
    VOS_UINT16                              usNcellNum;                                 /* 维护的邻区个数 */
    GSM_STRONG_NCELL_STRU                   astNcellInfo[GSM_STRONGEST_FREQLIST_NUM];   /* 邻区信息 */
} MPH_NC_MEAS_REPORT_IND_STRU;


/*****************************************************************************
 消息名称  :  MPH_RA_ABORT_REQ_ST
 功能描述  :  协议栈通过该原语通知物理层结束RACH接入过程，但如果需要继续监听AGCH，
              那么协议栈使用参数MonitorPagch来指示物理层监听AGCH.
 修改历史  :
  1.日    期    : 2007年10月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    PHY_MONI_AGCH_FLG_ENUM_UINT16           enMonitorAgch;                      /* 指示PHY是否监听AGCH,0表示不再监听，1表示继续监听 */
} MPH_RA_ABORT_REQ_ST;


/*****************************************************************************
 消息名称  :  MPHP_GPRS_CHANNEL_QUALITY_REPORT_ST
 功能描述  :  该原语用于在GPRS模式下PHY周期性的向GRR上报信道质量报告。
 修改历史  :
  1.日    期    : 2007年10月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usArfcn;                            /* 本服务小区的频点 */
    VOS_UINT16                              usC;                                /* C值，范围为0～63 */
    VOS_UINT16                              usRxqual;                           /* RXQUAL值，为测量质量的映射等级值，范围为0～7 */
    VOS_UINT16                              usSignalVar;                        /* SIGN_VAR值，接收信号强度的方差映射等级值，范围为0～63 */
    VOS_UINT16                              ausInLevelTn[GAS_INT_LEVEL_TN_MAX]; /* 在TN0-TN7上进行干扰测量的结果映射等级值，范围为0～15 */
} MPHP_GPRS_CHANNEL_QUALITY_REPORT_ST;


/*****************************************************************************
 消息名称  :  MPHP_EGPRS_CHANNEL_QUALITY_REPORT_ST
 功能描述  :  该原语用于在EGPRS模式下PHY周期性的向GRR上报信道质量报告。
 修改历史  :
  1.日    期    : 2008年2月22日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GRR_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usARFCN;                            /* 本服务小区的频点 */
    VOS_UINT16                              usC;                                /* C值，范围为0～63 */
    VOS_UINT16                              usMeanBepGmsk;                      /* GMSK下所有时隙的平均误比特率，0～31    */
    VOS_UINT16                              usMeanBep8Psk;                      /* 8PSK下所有时隙的平均误比特率，0～31    */
    VOS_UINT16                              usCVBepGmsk;                        /* GMSK下所有时隙的误比特率方差系数，0～7 */
    VOS_UINT16                              usCVBep8Psk;                        /* 8PSK下所有时隙的误比特率方差系数，0～7 */
    VOS_UINT16                              usTsQualityPresent;                 /* 表示以下几个与EGPRS Timeslot Link Quality相关的测量是否存在，0,不存在，1,存在 */
    VOS_UINT16                              ausMeanBepGmskTn[GAS_MAX_TIMESLOT_NUM];     /* GMSK下TN0-TN7的平均误比特率，0～31,无效值为0xFF    */
    VOS_UINT16                              ausMeanBep8PskTn[GAS_MAX_TIMESLOT_NUM];     /* 8PSK下TN0-TN7的平均误比特率，0～31,无效值为0xFF    */
    VOS_UINT16                              ausInLevelTn[GAS_MAX_TIMESLOT_NUM]; /* 在TN0-TN7上进行干扰测量的结果映射等级值，0～15,无效值为0xFF */
} MPHP_EGPRS_CHANNEL_QUALITY_REPORT_ST;


/*****************************************************************************
  * MPH_PCCCH_ST结构:沿用原有定义
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usTsc;
    VOS_UINT16                              usSlotBitMap;                       /* 时隙映射使用低8bit，bit0对应时隙0，bit7对应时隙7，对应比特为1表示该时隙被分配*/
    VOS_UINT16                              usMaio;                             /* 跳频MAIO     */
    VOS_UINT16                              usHsn;                              /* 跳频序列号*/
    VOS_UINT16                              usFreqNum;                          /* 跳频频点个数*/
    VOS_UINT16                              ausFreqList[GAS_MAX_HOPPING_FREQUENCY_NUM];
} MPH_PCCCH_ST;


/*****************************************************************************
 消息名称  :  MPHP_UPDATE_PSI_PARAM_REQ_ST
 功能描述  :  当小区驻留成功后，GAS通过该原语通知L1更新系统消息。
 修改历史  :
  1.日    期    : 2007年10月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usPb;                               /* 功率衰减值, 0-15，每级2dB*/
    VOS_UINT16                              usABType;                           /* 接入burst类型 */
    VOS_UINT16                              usGPRSMaxTxPow;                     /* MS最大发射功率 */
    VOS_UINT16                              usLB_MS_TXPWR_MAX_CCH;              /* 0xFFFF表示无效 */
    VOS_UINT16                              usPccchChannels;                    /* 小区中Pccch的信道个数 */
    VOS_UINT16                              usPbcchBlks;                        /* Pbcch所占用的块数 */
    VOS_UINT16                              usPagBlksRes;                       /* PAGCH所占用的块数 */
    VOS_UINT16                              usPrachBlks;                        /* Prach所占用的块数 */
    VOS_UINT16                              usSpgcCcchSup;                      /* CCCH寻呼计算参数*/
    VOS_UINT16                              usDrxCycle;                         /* SPLIT_PG_CYCLE */
    VOS_UINT16                              usKTavgT;                           /* 分组传输模式下功率控制中计算信号强度滤波周期参数*/
    VOS_UINT16                              usKTavgW;                           /* 分组传输模式下功率控制中计算信号强度滤波周期参数*/
    VOS_UINT16                              usKTavgI;                           /* 功率控制中计算干扰强度滤波器参数*/
    VOS_UINT16                              usBepPeriod;                        /* 新增用于计算BEP,取值范围[0,10],无效值为0XFFFF*/
    VOS_UINT16                              usPCMeasCh;                         /* 测量信道指示，0：BCCH，1：PDCH；*/
    VOS_UINT16                              usNetWorkMode;                      /* Mode1、2、3；*/
    VOS_UINT16                              usImsiMod1000;                      /* 该参数用来计算PPCH位置，参见05.02 6.5.6*/
    VOS_UINT16                              usPccchValid;                       /* 0表示不存在,1表示存在 */
    VOS_UINT16                              usReserved;                         /* 四字节对齐保留 */
    MPH_PCCCH_ST                            stPCCChDescription;
} MPHP_UPDATE_PSI_PARAM_REQ_ST;


/*****************************************************************************
 消息名称  :  MPHP_UPDATE_PSI_PARAM_CNF_ST
 功能描述  :  该原语用于L1向GRR上报分组域系统消息更新确认。
 修改历史  :
  1.日    期    : 2007年10月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRsv;
} MPHP_UPDATE_PSI_PARAM_CNF_ST;


/*****************************************************************************
 消息名称  :  MPHP_SERVING_PBCCH_SETUP_REQ_ST
 功能描述  :  服务小区PBCCH建立原语。协议栈使用该原语通知物理层接收服务小区系统消息，
              物理层通过该消息判断当前的服务小区，并且驻留在该小区。
 修改历史  :
  1.日    期    : 2007年10月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usHopping;                          /* 跳频指示，0表示不跳频，1表示跳频; */
    VOS_UINT16                              usHsn;                              /* 跳频序列号;0～63;*/
    VOS_UINT16                              usMaio;                             /* Mobile Allocation Index Offset;用于产生跳频序列，参考05.02描述;*/
    VOS_UINT16                              usTsc;                              /* 训练序列号，0～7;*/
    VOS_UINT16                              usPb;                               /* power reduction (usPb) used on PBCCH，0～15;参考05.08描述;*/
    VOS_UINT16                              usPbcchArfcn;                       /* 无线频率信道号；0～1023；非跳频时存在   */
    VOS_UINT16                              usFreqNum;                          /* 跳频频点数目，[0,64] 跳频时有效 */
    VOS_UINT16                              usPbcchFreqList[GAS_MAX_HOPPING_FREQUENCY_NUM]; /* 跳频时有效*/
    VOS_UINT16                              usSlotBitMap;                       /* PBCCH所在的时隙TimeSlot */
    VOS_UINT16                              usPSIRepeatPeriod;                  /* 52 复帧内PSI 1的重复周期，1～16;*/
    VOS_UINT16                              usPbcchModeBcc;                     /* 定义PBCCH的译码操作模式，0表示接收全部系统消息，1表示接收PSI 1后的系统消息，2表示只接收PSI 1系统消息;*/
    VOS_UINT16                              usPcMeasCh;                         /* 测量信道指示，0表示BCCH;1表示PDCH; 与物理层确认了，默认值可以为0 */
} MPHP_SERVING_PBCCH_SETUP_REQ_ST;


/*****************************************************************************
 消息名称  :  MPHP_SERVING_PBCCH_SETUP_CNF_ST
 功能描述  :  服务小区PBCCH建立确认原语。
 修改历史  :
  1.日    期    : 2007年10月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usSucessInd;                        /* 0表示成功，1表示失败；*/
} MPHP_SERVING_PBCCH_SETUP_CNF_ST;


/*****************************************************************************
 消息名称  :  MPHP_SERVING_PBCCH_RELEASE_REQ_ST
 功能描述  :  服务小区PBCCH释放原语，物理层使用该原语停止服务小区系统消息接收。
 修改历史  :
  1.日    期    : 2007年10月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usReserved;
} MPHP_SERVING_PBCCH_RELEASE_REQ_ST;


/*****************************************************************************
 消息名称  :  MPHP_SERVING_PBCCH_RELEASE_CNF
 功能描述  :  服务小区PBCCH释放确认原语。
 修改历史  :
  1.日    期    : 2007年10月06日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usSucessInd;                        /* 0表示成功，1表示失败；*/
} MPHP_SERVING_PBCCH_RELEASE_CNF;


/*****************************************************************************
 消息名称  :  MPH_CELL_RESELECT_CNF_STRU
 功能描述  :  对消息MPHP_CELL_RESELECT_REQ的应答消息。
 修改历史  :
  1.日    期    : 2007年11月27日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    EDGE_CNF_RESULT_ENUM_UINT16             enSucessInd;
    VOS_UINT16                              usBSIC;
    VOS_UINT16                              usRsv;
} MPH_CELL_RESELECT_CNF_STRU;


/* 测量中断模拟原语 */
typedef struct /*测量信息上报*/
{
    VOS_UINT16                              enAgcNextGain;                      /* 下一次期望的AGC档位*/
    VOS_UINT16                              enExactRssiTsn;                     /* 原语类型 *//* 应该产生中断的时隙*/
    VOS_UINT16                              uhwFreq;                            /* 频点*/
    VOS_INT16                               shwAgcRssi;                         /* 测量电平值*/
    VOS_UINT16                              uhwFnMod102;                        /* 帧号模上102的结果 */
    VOS_UINT16                              uhwFnMod104;                        /* 帧号模上104的结果 */
    VOS_UINT16                              uhwFnMod26;                         /* 帧号模上26的结果 */
    VOS_UINT16                              uhwReserved;                        /* 四字节对齐*/
} COM_MEAS_ISR_RPT_INFO_STUB_STRU;


/*****************************************************************************
 消息名称  :  MPH_PHY_MEA_ISR_STUB_REQ_STRU
 功能描述  :  ITT测试测量中断消息
 修改历史  :
  1.日    期    : 2007年11月27日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usISRID;
    COM_MEAS_ISR_RPT_INFO_STUB_STRU         stMeaIsrResult;
} MPH_PHY_MEA_ISR_STUB_REQ_STRU;


/* 同步中断模拟原语 *//* 同步信息上报 */
typedef struct
{
    VOS_UINT16                              enSyncStateWord;                    /* 同步状态字 */
    VOS_UINT16                              enAfcLock ;                         /* AFC 状态 */
    VOS_UINT16                              uhwBSIC;                            /* 0xffff 无效 */
    VOS_UINT16                              uhwT1;                              /* 帧号信息 0xffff 无效 */
    VOS_UINT16                              uhwT2;                              /* 帧号信息 0xffff 无效 */
    VOS_UINT16                              uhwT3a;                             /* 帧号信息 0xffff 无效 */
    VOS_UINT16                              uhwScellTOAOffset;                  /* 服务区帧头偏差，相对于TIMEBASE的偏差 */
    VOS_UINT16                              uhwNcellTOAOffset;                  /* 邻区任务使用，相对于TIMEBASE的偏差 */
    VOS_UINT32                              uwScellFrameNum;                    /* 服务区使用低16bitTDMA帧号低位0~26x51-1循环计数
                                                                                   高16bitTDMA帧号高位0~2047循环计数 */
    VOS_UINT32                              uwNcellFrameNum;                    /* 邻区使用低16bitTDMA帧号低位0~26x51-1循环计数
                                                                                   高16bitTDMA帧号高位0~2047循环计数 */

} COM_SYN_ISR_RPT_INFO_STUB_STRU;


/*****************************************************************************
 消息名称  :  MPH_PHY_SYN_ISR_STUB_REQ_STRU
 功能描述  :  ITT测试同步中断消息
 修改历史  :
  1.日    期    : 2007年11月27日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usISRID;
    COM_SYN_ISR_RPT_INFO_STUB_STRU          stMeaIsrResult;
} MPH_PHY_SYN_ISR_STUB_REQ_STRU;


/* cs译码中断模拟原语 *//* CS译码信息上报 */
typedef struct
{
    VOS_UINT16                              enChannelType;
    VOS_UINT16                              enAssignRxTs;
    VOS_UINT16                              uhwFacchStealFlg;                   /* 偷帧标志 1表示当前为FACCH译码，0表示非FACCH译码*/
    VOS_UINT16                              uhwTchHStealFlg;                    /* TCH/H 情况下偷帧使用,本次中断无数据,
                                                                                   0 表示正常译码上报 2表示当前参与译码的四个时隙为FACCH/H的前四个时隙，不能启动译码 */
    VOS_UINT16                              enCrcFlg;
    VOS_UINT16                              uhwBerBitNum;                       /* ber 统计错误bit数 */
    VOS_UINT16                              uhwBerTotalBitNum;                  /* ber 统计总bit数 */
    VOS_UINT16                              uhwBerClassIbNum;                   /* ClassIb bit error num */
    VOS_UINT16                              enAmrFrameType;                     /* 上报AMR下的帧类型 */
    VOS_UINT16                              auhwAmrIc[2];                       /* AMR带内比特 ic0,ic1，如果只有ic，
                                                                                   则放在auhwAmrIc[0]，0xfffff无效 */
    VOS_UINT16                              uhwTC51;                            /* TC = (FN div 51 ) mod 8 */
    VOS_UINT32                              uwFN;                               /* 帧号 */
    VOS_UINT16                              uhwFnMod104;                        /* 帧号模上104的结果 */
    VOS_UINT16                              uhwFnMod102;                        /* 帧号模上102的结果 */
    VOS_UINT16                              uhwDecDataLen;                      /* 有效数据长度,32BIT为单位长度 */
    VOS_UINT16                              uhwReserved;                        /* 四字节对齐 */
    VOS_UINT16                              auhwDecDataInfo[10*2];              /* 保存译码上报的数据信息 */
} COM_CSDEC_ISR_RPT_INFO_STUB_STRU;


/*****************************************************************************
 消息名称  :  MPH_PHY_CSDEC_ISR_STUB_REQ_STRU
 功能描述  :  ITT测试CS译码中断消息
 修改历史  :
  1.日    期    : 2007年11月27日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usISRID;
    COM_CSDEC_ISR_RPT_INFO_STUB_STRU        stMeaIsrResult;
} MPH_PHY_CSDEC_ISR_STUB_REQ_STRU;


/* PS译码中断模拟原语 *//* PS译码信息上报 */
typedef struct
{
    VOS_UINT16                              enChannelType;
    VOS_UINT16                              enAssignRxTs;
    VOS_UINT16                              enPscodeType;                       /* PDTCH下的编码类型 */
    VOS_UINT16                              uhwUSFDec;                          /* 解析的usf */
    VOS_UINT16                              uhwPayLoadType;                     /* PACCH指示 */
    VOS_UINT16                              uhwTFIDec;                          /* 解析的tfi */
    VOS_UINT16                              uhwPacchFlag;                       /* 指示当前块为PACCH，1表示为PACCH */
    VOS_UINT16                              uhwTFIFlag;                         /* 为1表示解析的tfi与分配的TFI一致 */
    VOS_UINT16                              aenCrcFlg[3];
    VOS_UINT16                              uhwBerBitNum;                       /* ber 统计错误bit数 */
    VOS_UINT16                              uhwBerTotalBitNum;                  /* ber 统计总bit数 */
    VOS_UINT16                              auhwBepBitNum[4];                   /* BEP bit num */
    VOS_UINT16                              auhwHarqPos[2];                     /* 上报译码前配置存放的HARQ缓存位置，harq功能启动有效 */
    VOS_UINT16                              auhwBsn[2];                         /* RLC bsn号码，用于HARQ */
    VOS_UINT16                              uhwFnMod26;                         /* 帧号模上26的结果 */
    VOS_UINT16                              uhwDecCycleRptNum;                  /* 码块上报的位置 */
    VOS_UINT16                              uhwBbpDecDataAddrOffset;            /* 译码数据偏移地址 */
    VOS_UINT16                              uhwDecDataLen;                      /* 有效数据长度 */
    VOS_UINT16                              uhwCbNum;                           /* 指示本次译码上报的码块数 */
    VOS_UINT16                              auhwDecDataInfo[12];                /* 保存译码上报的数据信息,这里的信息仅为PPCH和PTCCH,PACCH */
} COM_PSDEC_ISR_RPT_INFO_STUB_STRU;


/*****************************************************************************
 消息名称  :  MPH_PHY_PSDEC_ISR_STUB_REQ_STRU
 功能描述  :  ITT测试PS译码中断消息
 修改历史  :
  1.日    期    : 2007年11月27日
    作    者    : fangjian(48555)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16              usMsgID;                            /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usISRID;
    COM_PSDEC_ISR_RPT_INFO_STUB_STRU        stMeaIsrResult;
} MPH_PHY_PSDEC_ISR_STUB_REQ_STRU;



/*****************************************************************************
 消息名称  : MPH_CBCH_READ_REQ_STRU
 功能描述  : GAS向物理层发出的CBCH信道读取请求。
 修改历史  :
  1.日    期    : 2010年04月30日
    作    者    : zhaopeng(59430)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    GSM_CBCH_TYPE_ENUM_UINT16               enCBCHType;                         /* CBCH信道类型 */
    CHANNEL_DESCRIPTION_STRU                stChanDescription;                  /* CBCH信道描述 */
    MOBILE_ALLOCATION_STRU                  stFreqList;                         /* CBCH信道跳频时的频点列表 */
    GSM_CBCH_SCHEDULE_INFO_ENUM_UINT16      enScheInfoFlg;                      /* CBCH调度信息是否有效的标志 */
    VOS_UINT16                              usUsedRcvNMBits;                    /* 接收NM BIT MAP中使用了多少个BIT */
    VOS_UINT32                              ulScheMsgFN;                        /* 最新接收到的SCHEDULE Message FIRST BLOCK的帧号, 由GDSP上报, GAS填回 */

    /*******************************************************
       需要DSP接收的PAGE BIT MAP, aulRcvNMBitmap[0]BIT0代表NM1是否接收,
       BIT31代表NM32是否接收, aulRcvNMBitmap[1] BIT0代表NM33是否接收,
       BIT N为1代表需要接收, BIT N为0代表不需要接收, 最高有效BIT代表调度消息BIT
    ********************************************************/

    VOS_UINT32                              aulRcvNMBitmap[2];
} MPH_CBCH_READ_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_STOP_CBCH_READ_REQ_STRU
 功能描述  : GAS向物理层发出的CBCH信道停止读取请求
 修改历史  :
  1.日    期    : 2010年04月30日
    作    者    : zhaopeng(59430)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    GSM_CBCH_TYPE_ENUM_UINT16               enCBCHType;                         /* CBCH信道类型 */
} MPH_STOP_CBCH_READ_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_STOP_CBCH_CUR_PAGE_REQ_STRU
 功能描述  : GCBS向物理层发出的停止当前页读取请求
 修改历史  :
  1.日    期    : 2010年04月30日
    作    者    : zhaopeng(59430)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    GSM_CBCH_TYPE_ENUM_UINT16               enCBCHType;                         /* CBCH信道类型 */
    VOS_UINT32                              ulPageFirstBlkFn;                   /* PAGE FIRST BLOCK的帧号, 最初由DSP上报, 接入层回填给DSP */
} MPH_STOP_CBCH_CUR_PAGE_REQ_STRU;


/*****************************************************************************
 消息名称  : MPH_CBCH_READ_CNF_STRU
 功能描述  : 物理层收到CBCH读取请求后的回复消息
 修改历史  :
  1.日    期    : 2010年04月30日
    作    者    : zhaopeng(59430)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usResult;                           /* 0表示成功，1表示失败 */
    GSM_CBCH_TYPE_ENUM_UINT16               enCBCHType;                         /* CBCH信道类型 */
    VOS_UINT16                              usRsv;
} MPH_CBCH_READ_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_STOP_CBCH_READ_CNF_STRU
 功能描述  : 物理层收到停止读取CBCH后的回复消息
 修改历史  :
  1.日    期    : 2010年04月30日
    作    者    : zhaopeng(59430)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usResult;                           /* 0表示成功，1表示失败 */
    GSM_CBCH_TYPE_ENUM_UINT16               enCBCHType;                         /* CBCH信道类型 */
    VOS_UINT16                              usRsv;
} MPH_STOP_CBCH_READ_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_STOP_CBCH_CUR_PAGE_CNF_STRU
 功能描述  : 物理层收到停止当前页读取后的回复消息
 修改历史  :
  1.日    期    : 2010年04月30日
    作    者    : zhaopeng(59430)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usResult;                           /* 0表示成功，1表示失败 */
    GSM_CBCH_TYPE_ENUM_UINT16               enCBCHType;                         /* CBCH信道类型 */
    VOS_UINT16                              usRsv;
} MPH_STOP_CBCH_CUR_PAGE_CNF_STRU;


/*****************************************************************************
 消息名称  : MPH_CBS_DATA_IND_STRU
 功能描述  : 物理层上报的CBS消息
 修改历史  :
  1.日    期    : 2010年04月30日
    作    者    : zhaopeng(59430)
    修改内容    : 新生成结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    VOS_UINT16                              usMsgID;                            /* _H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                              usRxlev;                            /* 译码信号强度 */
    GSM_CBCH_TYPE_ENUM_UINT16               enCBCHType;                         /* CBCH信道类型 */
    VOS_UINT16                              usTC;                               /* 51复帧号, 取值范围0-7 */
    VOS_UINT32                              ulFN;                               /* 当前帧号 */
    GSM_CBCH_SCHEDULE_INFO_ENUM_UINT16      enScheInfoFlg;                      /* 当前调度信息是否生效 */
    VOS_UINT16                              usPageIndex;                        /* 接收的PAGE INDEX */
    VOS_UINT16                              usRsv;
    VOS_UINT16                              usErrFlg;                           /* 译码错误指示 */
    VOS_UINT16                              ausData[12];                        /* CBS数据 */
} MPH_CBS_DATA_IND_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    GAS_PHY_MODE_ENUM_UINT16        enActiveMode;       /* 设置为主模的模式 */
    GAS_PHY_MODE_ENUM_UINT16        enInactiveMode;     /* 设置为从模的模式 */
    VOS_UINT16                      usReserved;
} MPH_SET_WORK_MODE_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    GAS_PHY_MODE_ENUM_UINT16        enActiveMode;       /* 设置为主模的模式 */
    GAS_PHY_MODE_ENUM_UINT16        enInactiveMode;     /* 设置为从模的模式 */
    VOS_UINT16                      usReserved;
}MPH_SET_WORK_MODE_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                      usReserved;
} MPH_LTE_MEAS_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                      usReserved;
}MPH_LTE_MEAS_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16          enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    PHY_GAS_STOP_MEAS_TYPE_ENUM_UINT16  enStopType;         /* 停止测量的类型 */
} MPH_STOP_MEAS_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16          enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    PHY_GAS_STOP_MEAS_TYPE_ENUM_UINT16  enStopType;         /* 停止测量的类型 */
}MPH_STOP_MEAS_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                      usReserved;
}MPH_TA_INFO_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                      usTAValue;
}MPH_TA_INFO_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    GAS_PHY_BG_SEARCH_ENUM_UINT16   enType;             /* 背景搜请求的类型 */
} MPH_L_BG_SEARCH_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */ /* 消息头 */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    GAS_PHY_BG_SEARCH_ENUM_UINT16   enType;             /* 背景搜请求的类型 */
    VOS_UINT16                      usResult;           /* 启动、停止物理层背景搜 L的确认结果，PHY_SUCCESS表示成功，PHY_FAILURE表示失败 */
    VOS_UINT16                      usReserved;         /* 填充位 */
}MPH_L_BG_SEARCH_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                      /*_H2ASN_Skip */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                      usReserved;
} MPH_TDS_MEAS_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /*_H2ASN_Skip */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                      usReserved;
}MPH_TDS_MEAS_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /*_H2ASN_Skip */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */

    /* 测量结果上报的变化门限, 单位:dBm ,取值范围[1,5], 如果该值不在[1,5]范围内, 
       则物理层仍然使用默认值 3 */
    VOS_UINT16                      usScellMeasRptGapThresh;  
    VOS_UINT16                      ausResv[2];
} MPH_MEAS_RLT_RPT_CTRL_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /*_H2ASN_Skip */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                      usReserved;
}MPH_MEAS_RLT_RPT_CTRL_CNF_STRU;



typedef struct
{
    VOS_MSG_HEADER                                      /*_H2ASN_Skip */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    PHY_GAS_COMM_FLAG_ENUM_UINT16   enStartFlag;        /* _TRUE:启动，_FALSE:不启动 */
}MPH_NOTCH_CHANNEL_IND_STRU;
typedef struct
{
    VOS_MSG_HEADER                                      /*_H2ASN_Skip */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    PHY_GAS_COMM_FLAG_ENUM_UINT16   enNarrowDcsFlag;    /* _TRUE:启动，_FALSE:不启动 */
}MPH_BAND_CFG_IND_STRU;
typedef struct
{
    VOS_MSG_HEADER                                      /*_H2ASN_Skip */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    PHY_GAS_COMM_FLAG_ENUM_UINT16   enUplinkCloseFlag;  /* _TRUE:关闭上行发射，_FALSE:不关闭上行发射 */
}MPH_TDS_LTE_RF_CONTROL_IND_STRU;
typedef struct
{
    VOS_MSG_HEADER                                      /*_H2ASN_Skip */
    PHY_GAS_MSG_ID_ENUM_UINT16      enMsgID;            /*_H2ASN_Skip */ /* 原语类型 */
    PHY_GAS_COMM_FLAG_ENUM_UINT16   enRseOnFlag;        /* _TRUE:RSE ON，_FALSE:RSE OFF */
}MPH_RSE_CFG_IND_STRU;


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of __PHYGASINTERFACE_H__ */

