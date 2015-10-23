/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : phytoolinterface.h
  版 本 号   : 初稿
  作    者   : luoqingquan
  生成日期   : 2012年8月14日
  最近修改   :
  功能描述   : phytoolinterface.h 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年8月14日
    作    者   : luoqingquan
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"


#ifndef __PHYTOOLINTERFACE_H__
#define __PHYTOOLINTERFACE_H__

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
  2.2 宏定义,W/G宏定义
*****************************************************************************/

/* 打印携带参数个数 */
#define UPHY_TOOL_MNTN_ERROR_RPT_VAR_NUM        ( 5 )
#define UPHY_TOOL_INTER_REG_MAX_NUM             ( 300 )

/* 可维可测的func type为4,intra msg类型需要找TOOL确认 */
#define UPHY_TOOL_MODEM1_FUNC_TYPE_OM_MSG       ( 0x44 )
#define UPHY_TOOL_MODEM1_FUNC_TYPE_INTRA_MSG    ( 0x54 )

#define UPHY_TOOL_MODEM0_FUNC_TYPE_OM_MSG       ( 0x04 )
#define UPHY_TOOL_MODEM0_FUNC_TYPE_INTRA_MSG    ( 0x1A )

/* 大小必须保持和phyoaminterface.h中UPHY_OAM_SLEEP_INFO_NUM一致 */
#define UPHY_TOOL_SLEEP_INFO_NUM                ( 16 )

/*****************************************************************************
  2.2 宏定义,GPHY宏定义
*****************************************************************************/

#ifdef INSTANCE_1
#define GPHY_RF_CAL_NO_SIG_FUNC_TYPE            ( 0x48 )
#define WPHY_RF_CAL_NO_SIG_FUNC_TYPE            ( 0x07 )
#else
#define GPHY_RF_CAL_NO_SIG_FUNC_TYPE            ( 0x08 )
#define WPHY_RF_CAL_NO_SIG_FUNC_TYPE            ( 0x07 )
#endif


/* PS数传时下行最大的时隙个数 */
#define GPHY_PS_MAX_RX_TSN_NUM                  ( 5 )

/* PS数传时上行最大的时隙个数 */
#define GPHY_PS_MAX_TX_TSN_NUM                  ( 4 )

/*****************************************************************************
 原语GPHY_TOOL_FUNC_BIT_CTRL_REQ_STRU中ulFuncBitMask每个bit位的含义;
 如果对应bit位置1:则对应功能失效;
 如果对应bit位置0:则恢复对应功能;
*****************************************************************************/

#define GPHY_FUNC_DRX_USE_DISABLE_FLG           ( BIT0 )
#define GPHY_FUNC_DRX_SIMPLIFY_DISABLE_FLG      ( BIT1 )
#define GPHY_FUNC_BA_MEA_DISABLE_FLG            ( BIT2 )
#define GPHY_FUNC_INTERFERE_MEA_DISABLE_FLG     ( BIT3 )
#define GPHY_FUNC_C1_RESEL_DISABLE_FLG          ( BIT4 )
#define GPHY_FUNC_C2_RESEL_DISABLE_FLG          ( BIT5 )
#define GPHY_FUNC_NCELL_FBSB_DISABLE_FLG        ( BIT6 )
#define GPHY_FUNC_NCELL_SB_DISABLE_FLG          ( BIT7 )
#define GPHY_FUNC_NCELL_TASK_DISABLE_FLG        ( BIT8 )
#define GPHY_FUNC_RF_FRONT_DISPLAY_MNTN_FLG     ( BIT9 )



/* 可维可测数据块最大上报个数 */
#define GPHY_TOOL_RPT_BLK_MAX_NUM               ( 10 )

/* 打印携带参数个数 */
#define GPHY_TOOL_MNTN_ERROR_RPT_VAR_NUM        ( 5 )                           /* 错误上报时，界定的上报变量个数 */


/*****************************************************************************
  2.3 宏定义,WPHY宏定义
*****************************************************************************/

/* 打印携带参数个数 */
#define WPHY_TOOL_MNTN_ERROR_RPT_VAR_NUM        ( 5 )                           /* 错误上报时，界定的上报变量个数 */

/* 激活小区最大个数 */
#define WPHY_MNTN_MAX_ACT_CELL_NUM              ( 7 )                           /* 激活集小区最大数目 */


/* 可维可测数据块最大上报个数 */
#define WPHY_TOOL_RPT_BLK_MAX_NUM               ( 10 )

#define WPHY_MNTN_TRCH_NUM                      ( 8 )

#define GPHY_TOOL_MA_FREQ_NUM                   ( 64 )                          /* 跳频序列的个数 */

#define GPHY_TOOL_BA_LIST_MAX_NUM               ( 32 )                          /* GSM BA LIST最大的个数 */

#define GPHY_TOOL_AMR_MAX_ACS_NUM               ( 4 )                           /* 最大激活集个数 */

#define GPHY_TOOL_RPT_MAX_LEN                   ( 100 )

#define GPHY_TOOL_AMR_CODEC_MAX_NUM             ( 8 )                           /* AMR最大编码个数 */

/* 物理层每帧最大时隙个数 */
#define GPHY_TOOL_TN_NUM_IN_FRAME               ( 8 )

#define GPHY_TOOL_MAX_UL_SLOT_NUM               ( 4 )                           /* Gsm上行最大的时隙个数 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  3.1 枚举定义,GPHY枚举定义
*****************************************************************************/

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 UPHY回复CNF时,携带的处理结果枚举
*****************************************************************************/
enum UPHY_TOOL_RTC_DEBUG_ENUM
{
    UPHY_MNTN_RTC_DEBUG_CLOSE                   = 0,
    UPHY_MNTN_RTC_DEBUG_OPEN                    = 1,

    UPHY_MNTN_RTC_DEBUG_BUTT
};
typedef VOS_UINT16 UPHY_TOOL_RTC_DEBUG_ENUM_UINT16;



/*****************************************************************************
 UPHY<-->TOOL原语列表
*****************************************************************************/
enum UPHY_TOOL_MNTN_MSG_ID_ENUM
{
    ID_TOOL_UPHY_REQ                            = 0x0,                          /* _H2ASN_MsgChoice  TOOL_UPHY_REQ_STRU */

    ID_TOOL_UPHY_QUERY_SHARE_ADDR_REQ           = 0xFE01,                       /* _H2ASN_MsgChoice  UPHY_TOOL_QUERY_SHARE_ADDR_REQ_STRU */
    ID_UPHY_TOOL_QUERY_SHARE_ADDR_IND           = 0xFE02,                       /* _H2ASN_MsgChoice  UPHY_TOOL_QUERY_SHARE_ADDR_IND_STRU */

    ID_TOOL_UPHY_SDT_LINK_STATUS_REQ            = 0xFE03,                       /* _H2ASN_MsgChoice  UPHY_TOOL_SDT_LINK_STATUS_REQ_STRU */
    ID_UPHY_TOOL_SDT_LINK_STATUS_CNF            = 0xFE04,                       /* _H2ASN_MsgChoice  UPHY_TOOL_COMMON_CNF_STRU */

    ID_TOOL_UPHY_ERROR_REPORT_REQ               = 0xFE05,                       /* _H2ASN_MsgChoice  UPHY_TOOL_ERROR_REPORT_REQ_STRU */
    ID_UPHY_TOOL_ERROR_REPORT_CNF               = 0xFE06,                       /* _H2ASN_MsgChoice  UPHY_TOOL_COMMON_CNF_STRU */

    ID_TOOL_UPHY_WPHY_BLER_RPT_REQ              = 0xFE07,
    ID_TOOL_UPHY_WPHY_INTRA_MSG_RPT_CTRL_REQ    = 0xFE08,
    ID_TOOL_UPHY_GPHY_INTRA_MSG_RPT_CTRL_REQ    = 0xFE09,


    ID_TOOL_UPHY_WPHY_INTER_GREG_REPORT_REQ     = 0xFE0A,                       /* _H2ASN_MsgChoice  UPHY_TOOL_WPHY_INTER_GREG_REPORT_REQ_STRU */
    ID_UPHY_TOOL_WPHY_INTER_GREG_REPORT_CNF     = 0xFE0B,                       /* _H2ASN_MsgChoice  UPHY_TOOL_WPHY_INTER_GREG_REPORT_CNF_STRU */
    ID_UPHY_TOOL_WPHY_INTER_GREG_REPORT_IND     = 0xFE0C,                       /* _H2ASN_MsgChoice  UPHY_TOOL_WPHY_INTER_GREG_REPORT_IND_STRU */
    ID_TOOL_UPHY_WPHY_INTER_GREG_STOP_REQ       = 0xFE0D,                       /* _H2ASN_MsgChoice  UPHY_TOOL_WPHY_INTER_GREG_STOP_REQ_STRU */
    ID_UPHY_TOOL_WPHY_INTER_GREG_STOP_CNF       = 0xFE0E,                       /* _H2ASN_MsgChoice  UPHY_TOOL_COMMON_CNF_STRU */

    ID_TOOL_UPHY_GPHY_INTER_WREG_REPORT_REQ     = 0xFE0F,                       /* _H2ASN_MsgChoice  UPHY_TOOL_GPHY_INTER_WREG_REPORT_REQ_STRU */
    ID_UPHY_TOOL_GPHY_INTER_WREG_REPORT_CNF     = 0xFE10,                       /* _H2ASN_MsgChoice  UPHY_TOOL_GPHY_INTER_WREG_REPORT_CNF_STRU */
    ID_UPHY_TOOL_GPHY_INTER_WREG_REPORT_IND     = 0xFE11,                       /* _H2ASN_MsgChoice  UPHY_TOOL_GPHY_INTER_WREG_REPORT_IND_STRU */
    ID_TOOL_UPHY_GPHY_INTER_WREG_STOP_REQ       = 0xFE12,                       /* _H2ASN_MsgChoice  UPHY_TOOL_GPHY_INTER_WREG_STOP_REQ_STRU */
    ID_UPHY_TOOL_GPHY_INTER_WREG_STOP_CNF       = 0xFE13,                       /* _H2ASN_MsgChoice  UPHY_TOOL_COMMON_CNF_STRU */

    ID_UPHY_TOOL_DRX_LATE_INFO_IND              = 0xFE14,                       /* _H2ASN_MsgChoice  UPHY_TOOL_DRX_LATE_INFO_IND_STRU */

    ID_UPHY_TOOL_INFO_REPORT_IND                = 0xFEF7,                       /* _H2ASN_MsgChoice  UPHY_TOOL_ERROR_REPORT_IND_STRU */
    ID_UPHY_TOOL_WARNING_REPORT_IND             = 0xFEF8,                       /* _H2ASN_MsgChoice  UPHY_TOOL_ERROR_REPORT_IND_STRU */
    ID_UPHY_TOOL_ERROR_REPORT_IND               = 0xFEF9,                       /* _H2ASN_MsgChoice  UPHY_TOOL_ERROR_REPORT_IND_STRU */


    ID_UPHY_TOOL_MSG_ID_BUTT
};
typedef VOS_UINT16 UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16;


/*****************************************************************************
 UPHY回复CNF时,携带的处理结果枚举
*****************************************************************************/
enum UPHY_TOOL_MNTN_RSLT_ENUM
{
    UPHY_MNTN_RSLT_SUCC                         = 0,
    UPHY_MNTN_RSLT_FAIL                         = 1,

    UPHY_MNTN_RSLT_BUTT
};
typedef VOS_UINT16 UPHY_TOOL_MNTN_RSLT_ENUM_UINT16;



/*****************************************************************************
 枚举名    : UPHY_TOOL_MNTN_RPT_LEVEL_ENUM
 协议表格  :
 枚举说明  : UPHY错误上报级别
*****************************************************************************/
enum UPHY_TOOL_MNTN_RPT_LEVEL_ENUM
{
    UPHY_MNTN_RPT_LEVEL_INFO                    = 1,
    UPHY_MNTN_RPT_LEVEL_WARNING                 = 2,
    UPHY_MNTN_RPT_LEVEL_ERROR                   = 3,

    UPHY_MNTN_RPT_LEVEL_BUTT
};
typedef VOS_UINT16 UPHY_TOOL_MNTN_RPT_LEVEL_ENUM_UINT16;





/*****************************************************************************
 GPHY<-->TOOL原语列表
*****************************************************************************/
enum GPHY_TOOL_MNTN_MSG_ID_ENUM
{
    /* 下发REQ消息因为带VOS消息，上报的IND或CNF消息不带VOS消息头，应对REQ另外解析，这里指示为了ASN生成方便 */
    ID_TOOL_GPHY_REQ                            = 0x0,                          /* _H2ASN_MsgChoice  TOOL_GPHY_REQ_STRU */

    ID_TOOL_GPHY_BLER_REPORT_REQ                = 0xE001,                       /* _H2ASN_MsgChoice  GPHY_TOOL_BLER_REPORT_REQ_STRU */
    ID_GPHY_TOOL_BLER_REPORT_CNF                = 0xE010,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU */
    ID_GPHY_TOOL_BLER_REPORT_IND                = 0xE011,                       /* _H2ASN_MsgChoice  GPHY_TOOL_BLER_REPORT_IND_STRU */

    ID_TOOL_GPHY_SNR_REPORT_REQ                 = 0xE002,                       /* _H2ASN_MsgChoice  GPHY_TOOL_SNR_REPORT_REQ_STRU */
    ID_GPHY_TOOL_SNR_REPORT_CNF                 = 0xE020,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU */
    ID_GPHY_TOOL_SNR_REPORT_IND                 = 0xE021,                       /* _H2ASN_MsgChoice  GPHY_TOOL_SNR_REPORT_IND_STRU */

    ID_TOOL_GPHY_AFC_REPORT_REQ                 = 0xE003,                       /* _H2ASN_MsgChoice  GPHY_TOOL_AFC_REPORT_REQ_STRU */
    ID_GPHY_TOOL_AFC_REPORT_CNF                 = 0xE030,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU */
    ID_GPHY_TOOL_AFC_REPORT_IND                 = 0xE031,                       /* _H2ASN_MsgChoice  GPHY_TOOL_AFC_REPORT_IND_STRU */

    ID_TOOL_GPHY_POWER_CTRL_REPORT_REQ          = 0xE004,                       /* _H2ASN_MsgChoice  GPHY_TOOL_POWER_CTRL_REPORT_REQ_STRU */
    ID_GPHY_TOOL_POWER_CTRL_REPORT_CNF          = 0xE040,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU */
    ID_GPHY_TOOL_POWER_CTRL_REPORT_IND          = 0xE041,                       /* _H2ASN_MsgChoice  GPHY_TOOL_POWER_CTRL_REPORT_IND_STRU */

    ID_TOOL_GPHY_POWER_CTRL_SET_REQ             = 0xE005,                       /* _H2ASN_MsgChoice  GPHY_TOOL_POWER_CTRL_SET_REQ_STRU */
    ID_GPHY_TOOL_POWER_CTRL_SET_CNF             = 0xE050,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU */

    ID_GPHY_TOOL_CHANNEL_QUALITY_IND            = 0xE080,                       /* _H2ASN_MsgChoice  GPHY_TOOL_CHANNEL_QUALITY_IND_STRU */

    ID_TOOL_GPHY_FUNC_BIT_CONTROL_REQ           = 0xE00A,                       /* _H2ASN_MsgChoice  GPHY_TOOL_FUNC_BIT_CTRL_REQ_STRU */
    ID_GPHY_TOOL_FUNC_BIT_CONTROL_CNF           = 0xE0A0,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU */

    ID_TOOL_GPHY_PARA_CONTROL_REQ               = 0xE00B,                       /* _H2ASN_MsgChoice  GPHY_TOOL_PARA_CONTROL_REQ_STRU */
    ID_GPHY_TOOL_PARA_CONTROL_CNF               = 0xE0B0,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU */

    ID_TOOL_GPHY_RF_FRONT_STATIC_REQ            = 0xE052,                       /* _H2ASN_MsgChoice  GPHY_TOOL_RF_FRONT_STATIC_REQ_STRU */
    ID_GPHY_TOOL_RF_FRONT_STATIC_IND            = 0xE053,                       /* _H2ASN_MsgChoice  GPHY_TOOL_RF_FRONT_STATIC_IND_STRU */

    ID_TOOL_GPHY_INTRA_MSG_RPT_CTRL_REQ         = 0xE054,                       /* _H2ASN_MsgChoice  GPHY_TOOL_INTRA_MSG_RPT_CTRL_REQ_STRU */
    ID_GPHY_TOOL_INTRA_MSG_RPT_CTRL_CNF         = 0xE055,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU */

    ID_TOOL_GPHY_AMR_REPORT_REQ                 = 0xE057,                       /* _H2ASN_MsgChoice  GPHY_TOOL_AMR_REPORT_REQ_STRU */
    ID_GPHY_TOOL_AMR_REPORT_CNF                 = 0xE058,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU */
    ID_GPHY_TOOL_AMR_REPORT_IND                 = 0xE059,                       /* _H2ASN_MsgChoice  GPHY_TOOL_AMR_REPORT_IND_STRU */

    ID_TOOL_GPHY_BLK_REPORT_LIST_REQ            = 0xE05A,                       /* _H2ASN_MsgChoice  GPHY_TOOL_BLK_REPORT_LIST_REQ_STRU */
    ID_GPHY_TOOL_BLK_REPORT_LIST_CNF            = 0xE05B,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU */
    ID_GPHY_TOOL_BLK_REPORT_LIST_IND            = 0xE05C,                       /* _H2ASN_MsgChoice  GPHY_TOOL_BLK_REPORT_LIST_IND_STRU */

    ID_GPHY_TOOL_GTC_TASK_INFO_IND              = 0xE061,                       /* _H2ASN_MsgChoice GPHY_GTC_CONTEXT_STRU */
    ID_GPHY_TOOL_RCV_SINGAL_INFO_IND            = 0xE062,                       /* _H2ASN_MsgChoice GPHY_TOOL_RCV_SINGAL_INFO_REQ_STRU */
    ID_GPHY_TOOL_CS_UL_INFO_IND                 = 0xE064,                       /* _H2ASN_MsgChoice GSM_CHN_UL_INFO_RPT_STRU */
    ID_GPHY_TOOL_CS_DL_INFO_IND                 = 0xE065,                       /* _H2ASN_MsgChoice COM_CS_DECODE_CTRL_INFO_STRU */

    ID_UPHY_TOOL_DCXO_TEMP_COMP_INFO_IND        = 0xE066,                       /* _H2ASN_MsgChoice UPHY_TOOL_DCXO_TEMP_COMP_INFO_IND_STRU */

    ID_GPHY_TOOL_REPORT_INFO_IND                = 0xE0F7,                       /* _H2ASN_MsgChoice  GPHY_TOOL_REPORT_ERROR_IND_STRU */
    ID_GPHY_TOOL_REPORT_WARNING_IND             = 0xE0F8,                       /* _H2ASN_MsgChoice  GPHY_TOOL_REPORT_ERROR_IND_STRU */
    ID_GPHY_TOOL_REPORT_ERROR_IND               = 0xE0F9,                       /* _H2ASN_MsgChoice  GPHY_TOOL_REPORT_ERROR_IND_STRU */

    ID_TOOL_GPHY_BBP_RTC_REQ                    = 0xE0FA,                       /* _H2ASN_MsgChoice GPHY_TOOL_BBP_RTC_REQ_STRU  */
    ID_GPHY_TOOL_BBP_RTC_CNF                    = 0xE0FB,                       /* _H2ASN_MsgChoice GPHY_TOOL_COMMON_CNF_STRU  */

    /* DSP-PROBE */
    ID_TOOL_GPHY_GSM_CELL_SNR_REQ               = 0xE600,                       /* _H2ASN_MsgChoice  GPHY_TOOL_GSM_CELL_SNR_REQ_STRU *//* 该命令用于请求上报服务小区和6强邻区的信噪比，测量值 */
    ID_GPHY_TOOL_GSM_CELL_SNR_CNF               = 0xE601,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU *//* 回复确认消息 */
    ID_GPHY_TOOL_GSM_CELL_SNR_IND               = 0xE602,                       /* _H2ASN_MsgChoice  GPHY_TOOL_GSM_CELL_SNR_IND_STRU *//* 上报服务小区和6强邻区的信噪比，测量值 */

    ID_TOOL_GPHY_BA_LIST_MEAS_REQ               = 0xE603,                       /* _H2ASN_MsgChoice  GPHY_TOOL_BA_LIST_MEAS_REQ_STRU *//* 该命令用于请求上报服务区和32个邻区的信号强度，bsic值 */
    ID_GPHY_TOOL_BA_LIST_MEAS_CNF               = 0xE604,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU *//* 回复确认消息 */
    ID_GPHY_TOOL_BA_LIST_MEAS_IND               = 0xE605,                       /* _H2ASN_MsgChoice  GPHY_TOOL_BA_LIST_MEAS_IND_STRU *//* 上报服务区和32个邻区的信号强度，bsic值 */

    ID_TOOL_GPHY_AMR_SETTING_REQ                = 0xE606,                       /* _H2ASN_MsgChoice  GPHY_TOOL_AMR_SETTING_REQ_STRU *//* 该命令用于请求上报AMR配置信息 */
    ID_GPHY_TOOL_AMR_SETTING_CNF                = 0xE607,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU *//* 回复确认消息 */
    ID_GPHY_TOOL_AMR_SETTING_IND                = 0xE608,                       /* _H2ASN_MsgChoice  GPHY_TOOL_AMR_SETTING_IND_STRU *//* 上报AMR配置信息 */

    ID_TOOL_GPHY_AMR_CODEC_USAGE_REQ            = 0XE609,                       /* _H2ASN_MsgChoice  GPHY_TOOL_AMR_CODEC_USAGE_REQ_STRU *//* 该命令用于请求上报激活集中每个语音速率的使用率 */
    ID_GPHY_TOOL_AMR_CODEC_USAGE_CNF            = 0XE60A,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU *//* 回复确认消息 */
    ID_GPHY_TOOL_AMR_CODEC_USAGE_IND            = 0XE60B,                       /* _H2ASN_MsgChoice  GPHY_TOOL_AMR_CODEC_USAGE_IND_STRU *//* 上报激活集中每个语音速率的使用率上 */

    ID_TOOL_GPHY_AMR_CODEC_STAT_REQ             = 0xE60C,                       /* _H2ASN_MsgChoice  GPHY_TOOL_AMR_CODEC_STAT_REQ_STRU *//* 该命令用于请求各个编码速率的使用率 */
    ID_GPHY_TOOL_AMR_CODEC_STAT_CNF             = 0xE60D,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU *//* 回复确认消息 */
    ID_GPHY_TOOL_AMR_CODEC_STAT_IND             = 0xE60E,                       /* _H2ASN_MsgChoice  GPHY_TOOL_AMR_CODEC_STAT_IND_STRU *//* 上报各个编码速率的使用率 */

    ID_TOOL_GPHY_CS_PS_TX_INFO_REQ              = 0xE60F,                       /* _H2ASN_MsgChoice  GPHY_TOOL_CS_TX_INFO_REQ_STRU *//* 该命令用于请求上报CS域上行信息 */
    ID_GPHY_TOOL_CS_PS_TX_INFO_CNF              = 0xE610,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU *//* 回复确认消息 */
    ID_GPHY_TOOL_CS_PS_TX_INFO_IND              = 0xE611,                       /* _H2ASN_MsgChoice  GPHY_TOOL_CS_TX_INFO_IND_STRU *//* 上报CS域上行信息 */

    ID_TOOL_GPHY_CS_FER_REQ                     = 0xE612,                       /* _H2ASN_MsgChoice  GPHY_TOOL_CS_FER_REQ_STRU *//* 该命令请求上报CS域的FER */
    ID_GPHY_TOOL_CS_FER_CNF                     = 0xE613,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU *//* 回复确认消息 */
    ID_GPHY_TOOL_CS_FER_IND                     = 0xE614,                       /* _H2ASN_MsgChoice  GPHY_TOOL_CS_FER_IND_STRU *//* 上报CS域的FER */

    ID_TOOL_GPHY_GET_CURRENT_DSC_REQ            = 0xE615,                       /* _H2ASN_MsgChoice  GPHY_TOOL_GET_CURRENT_DSC_REQ_STRU *//* 该命令请求上报DSC计数值 */
    ID_GPHY_TOOL_GET_CURRENT_DSC_CNF            = 0xE616,                       /* _H2ASN_MsgChoice  GPHY_TOOL_COMMON_CNF_STRU *//* 回复确认消息 */
    ID_GPHY_TOOL_GET_CURRENT_DSC_IND            = 0xE617,                       /* _H2ASN_MsgChoice  GPHY_TOOL_GET_CURRENT_DSC_IND_STRU *//* 上报DSC计数值 */

    ID_GPHY_TOOL_MASTER_WAKE_LTE_IND            = 0xE620,
    ID_GPHY_TOOL_MASTER_START_LTE_MEAS_IND      = 0xE621,
    ID_GPHY_TOOL_MASTER_REPORT_LTE_MEAS_IND     = 0xE622,
    ID_GPHY_TOOL_MASTER_STOP_LTE_MEAS_IND       = 0xE623,
    ID_GPHY_TOOL_SET_LTE_MEAS_FLAG_IND          = 0xE624,
    ID_GPHY_TOOL_CLEAR_LTE_MEAS_FLAG_IND        = 0xE625,

    ID_GPHY_TOOL_MASTER_WAKE_TDS_IND            = 0xE630,
    ID_GPHY_TOOL_MASTER_START_TDS_MEAS_IND      = 0xE631,
    ID_GPHY_TOOL_MASTER_REPORT_TDS_MEAS_IND     = 0xE632,
    ID_GPHY_TOOL_MASTER_STOP_TDS_MEAS_IND       = 0xE633,
    ID_GPHY_TOOL_SET_TDS_MEAS_FLAG_IND          = 0xE634,
    ID_GPHY_TOOL_CLEAR_TDS_MEAS_FLAG_IND        = 0xE635,

    /* 双天线判决结果消息上报 */
    ID_GPHY_TOOL_DUAL_ANT_ESTIMATE_IND          = 0xE640,                       /* _H2ASN_MsgChoice  GPHY_TOOL_DUAL_ANT_ESTIMATE_IND_STRU *//* 上报主天线能量评估阶段判决信息 */
    ID_GPHY_TOOL_DUAL_ANT_COMPARE_IND           = 0xE641,                       /* _H2ASN_MsgChoice  GPHY_TOOL_DUAL_ANT_COMPARE_IND_STRU *//* 上报主辅天线能量比较阶段判决信息 */
    ID_GPHY_TOOL_DUAL_ANT_DELAY_IND             = 0xE642,                       /* _H2ASN_MsgChoice  GPHY_TOOL_DUAL_ANT_DELAY_IND_STRU *//* 上报天线切换迟滞信息 */

    ID_GPHY_TOOL_MSG_ID_BUTT
};
typedef VOS_UINT16 GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16;


/*****************************************************************************
 GPHY回复CNF时,携带的处理结果枚举
*****************************************************************************/
enum GPHY_TOOL_MNTN_RSLT_ENUM
{
    GPHY_MNTN_RSLT_SUCC                         = 0,
    GPHY_MNTN_RSLT_FAIL                         = 1,

    GPHY_MNTN_RSLT_BUTT
};
typedef VOS_UINT16 GPHY_TOOL_MNTN_RSLT_ENUM_UINT16;



/*****************************************************************************
 TOOL下发参数控制，主要是功率控制和BEP控制
*****************************************************************************/
enum GPHY_TOOL_PARA_CTRL_ENUM
{
    GPHY_TOOL_PARA_PWC_CTRL                     = 0x01,                         /* 下发控制功率参数 */
    GPHY_TOOL_PARA_BEP_CTRL                     = 0x02,                         /* 下发控制BEP参数 */

    GPHY_TOOL_PARA_CTRL_BUTT
};
typedef VOS_UINT16 GPHY_TOOL_PARA_CTRL_ENUM_UINT16;



/*****************************************************************************
 枚举名    : GPHY_TOOL_MNTN_ERROR_RPT_LEVEL_ENUM_UINT16
 协议表格  :
 枚举说明  : GPHY错误上报级别
*****************************************************************************/
enum GPHY_TOOL_ERROR_RPT_LEVEL_ENUM
{
    GPHY_TOOL_ERROR_LEVEL_INFO                  = 1,
    GPHY_TOOL_ERROR_LEVEL_WARNING               = 2,
    GPHY_TOOL_ERROR_LEVEL_ERROR                 = 3,

    GPHY_TOOL_ERROR_LEVEL_BUTT
};
typedef VOS_UINT16 GPHY_TOOL_ERROR_RPT_LEVEL_ENUM_UINT16;






/*****************************************************************************
 GPHY模块定义
*****************************************************************************/

enum GPHY_TOOL_MODULE_ENUM
{
    GPHY_MODULE_DRV                             = 0x5501,                       /* DRIVER模块名编号 */
    GPHY_MODULE_MEA                             = 0x5502,                       /* 测量模块名编号 */
    GPHY_MODULE_NCE_TSK                         = 0x5503,                       /* 邻区任务模块名编号 */
    GPHY_MODULE_NCE_RES                         = 0x5504,                       /* 邻区重选模块名编号 */
    GPHY_MODULE_CHN_CCH                         = 0x5505,                       /* 公共信道模块名编号 */
    GPHY_MODULE_CHN_CSDCH                       = 0x5506,                       /* CS专用信道模块名编号 */
    GPHY_MODULE_CHN_PSDCH                       = 0x5507,                       /* PS专用信道模块名编号 */
    GPHY_MODULE_CHN_AMRDTX                      = 0x5508,                       /* 信道AMRDTX模块名编号 */
    GPHY_MODULE_COM                             = 0x5509,                       /* 公共模块名编号 */
    GPHY_MODULE_COM_MAIL                        = 0x550A,                       /* 公共邮箱模块名编号 */
    GPHY_MODULE_COM_TOA                         = 0x550B,                       /* 公共TOA模块名编号 */
    GPHY_MODULE_DRX                             = 0x550C,                       /* DRX模块名编号 */
    GPHY_MODULE_OM                              = 0x550D,                       /* OM模块名编号 */
    GPHY_MODULE_WMEAS                           = 0x550E,                       /* WMEAS模块名编号 */
    GPHY_MODULE_CAL                             = 0x550F,                       /* CAL模块名编号 */
    GPHY_MODULE_LMEAS                           = 0x5510,                       /* LTE测量模块名编号 */
    GPHY_MODULE_PD_INTERFACE                    = 0x5511,                       /* 下电部分代码接口函数模块 */
    GPHY_MODULE_TMEAS                           = 0x5512,                       /* TD测量模块名编号 */
    GPHY_MODULE_BUTT

};
typedef VOS_UINT16 GPHY_TOOL_MODULE_ENUM_UINT16;


/*****************************************************************************
  3.2 枚举定义,WPHY枚举定义
*****************************************************************************/


/*****************************************************************************
 WPHY<-->TOOL原语列表
*****************************************************************************/

enum WPHY_TOOL_MNTN_MSG_ID_ENUM
{
    /* 下发REQ消息因为带VOS消息，上报的IND或CNF消息不带VOS消息头，应对REQ另外解析，这里指示为了ASN生成方便 */
    ID_TOOL_WPHY_REQ                            = 0x0,                          /* _H2ASN_MsgChoice TOOL_WPHY_REQ_STRU */

    /* 激活集小区上报  */
    ID_TOOL_WPHY_ACT_CELL_REQ                   = 0xF001,                       /* _H2ASN_MsgChoice WPHY_TOOL_ACT_CELL_REQ_STRU */
    ID_WPHY_TOOL_ACT_CELL_CNF                   = 0xF002,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */
    ID_WPHY_TOOL_ACT_CELL_IND                   = 0xF003,                       /* _H2ASN_MsgChoice WPHY_TOOL_ACT_CELL_INFO_IND_STRU */

    /* 小区搜索列表上报 */
    ID_TOOL_WPHY_CELL_SEARCH_LIST_REQ           = 0xF004,                       /* _H2ASN_MsgChoice WPHY_TOOL_CELL_SEARCH_LIST_REQ_STRU  */
    ID_WPHY_TOOL_CELL_SEARCH_LIST_CNF           = 0xF005,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */
    ID_WPHY_TOOL_CELL_SEARCH_LIST_IND           = 0xF006,                       /* _H2ASN_MsgChoice WPHY_TOOL_CELL_SEARCH_LIST_IND_STRU */


    /* 功控开关控制 */
    ID_TOOL_WPHY_PWR_SWITCH_CTRL_REQ            = 0xF00A,                       /* _H2ASN_MsgChoice WPHY_TOOL_PWR_SWITCH_CTRL_REQ_STRU */
    ID_WPHY_TOOL_PWR_SWITCH_CTRL_CNF            = 0xF00B,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */

    /* 功控参数控制 */
    ID_TOOL_WPHY_PWR_PARA_SET_REQ               = 0xF010,                       /* _H2ASN_MsgChoice WPHY_TOOL_PWR_PARA_SET_REQ_STRU */
    ID_WPHY_TOOL_PWR_PARA_SET_CNF               = 0xF011,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */


    /* 误块率上报 */
    ID_TOOL_WPHY_BLER_REPORT_REQ                = 0xF020,                       /* _H2ASN_MsgChoice WPHY_TOOL_BLER_REPORT_REQ_STRU */
    ID_WPHY_TOOL_BLER_REPORT_CNF                = 0xF021,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */
    ID_WPHY_TOOL_BLER_REPORT_IND                = 0xF022,                       /* _H2ASN_MsgChoice WPHY_TOOL_BLER_REPORT_IND_STRU */


    /* 事件上报 */
    ID_TOOL_WPHY_EVENT_REPORT_REQ               = 0xF02A,                       /* _H2ASN_MsgChoice WPHY_TOOL_EVENT_REPORT_REQ_STRU */
    ID_WPHY_TOOL_EVENT_REPORT_CNF               = 0xF02B,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */

    /* 事件上报:同失步上报 */
    ID_WPHY_TOOL_SYNC_STATUS_IND                = 0xF02C,                       /* _H2ASN_MsgChoice WPHY_TOOL_SYNC_STATUS_IND_STRU */

    /* 事件上报:AFC状态 */
    ID_WPHY_TOOL_VCTCXO_STATUS_IND              = 0xF02E,                       /* _H2ASN_MsgChoice WPHY_TOOL_AFC_STATUS_IND_STRU */

    /* 事件上报:中断任务异常报告 */
    ID_WPHY_TOOL_INTERRUPT_EXCEPTIION_IND       = 0xF030,                       /* _H2ASN_MsgChoice WPHY_TOOL_INTERRUPT_EXCEPTIION_IND_STRU */

    /* 事件上报:L1状态上报 */
    ID_WPHY_TOOL_WCDMA_L1_STATUS_IND            = 0xF031,                       /* _H2ASN_MsgChoice WPHY_TOOL_WCDMA_L1_STATUS_IND_STRU */

    /* DRX控制 */
    ID_TOOL_WPHY_DRX_SWITCH_REQ                 = 0xF032,                       /* _H2ASN_MsgChoice WPHY_TOOL_DRX_SWITCH_REQ_STRU */
    ID_WPHY_TOOL_DRX_SWITCH_CNF                 = 0xF033,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */

    /* 门限设置 */
    ID_TOOL_WPHY_THRESHOLD_SET_REQ              = 0xF034,                       /* _H2ASN_MsgChoice WPHY_TOOL_THRESHOLD_SET_REQ_STRU */
    ID_WPHY_TOOL_THRESHOLD_SET_CNF              = 0xF035,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */

    /* 门限查询 */
    ID_TOOL_WPHY_THRESHOLD_QUERY_REQ            = 0xF036,                       /* _H2ASN_MsgChoice WPHY_TOOL_THRESHOLD_QUERY_REQ_STRU */
    ID_WPHY_TOOL_THRESHOLD_QUERY_IND            = 0xF037,                       /* _H2ASN_MsgChoice WPHY_TOOL_THRESHOLD_QUERY_IND_STRU */

    /* 错误上报 */
    ID_TOOL_WPHY_ERROR_REPORT_REQ               = 0xF038,                       /* _H2ASN_MsgChoice WPHY_TOOL_ERROR_REPORT_REQ_STRU */
    ID_WPHY_TOOL_ERROR_REPORT_CNF               = 0xF039,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */
    ID_WPHY_TOOL_ERROR_REPORT_IND               = 0xF03A,                       /* _H2ASN_MsgChoice WPHY_TOOL_ERROR_REPORT_IND_STRU  */
    ID_WPHY_TOOL_WARNING_REPORT_IND             = 0xF03B,                       /* _H2ASN_MsgChoice WPHY_TOOL_ERROR_REPORT_IND_STRU  */
    ID_WPHY_TOOL_INFO_REPORT_IND                = 0xF03C,                       /* _H2ASN_MsgChoice WPHY_TOOL_ERROR_REPORT_IND_STRU  */


    ID_TOOL_WPHY_RF_FRONT_END_PARA_REQ          = 0xF03D,                       /* _H2ASN_MsgChoice WPHY_TOOL_RF_FRONT_END_PARA_REQ_STRU  */
    ID_WPHY_TOOL_RF_FRONT_END_PARA_CNF          = 0xF03E,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU  */
    ID_WPHY_TOOL_RF_FRONT_END_PARA_IND          = 0xF03F,                       /* _H2ASN_MsgChoice WPHY_TOOL_RF_FRONT_END_PARA_IND_STRU  */

    ID_WPHY_TOOL_PI_INT_RESULT_IND              = 0xF040,                       /* _H2ASN_MsgChoice WPHY_TOOL_PI_INT_RESULT_IND_STRU  */
    ID_WPHY_TOOL_AI_INT_RESULT_IND              = 0xF041,                       /* _H2ASN_MsgChoice WPHY_TOOL_AI_INT_RESULT_IND_STRU  */

    /* T313 状态上报 */
    ID_WPHY_TOOL_T313_STATUS_IND                = 0xF042,                       /* _H2ASN_MsgChoice WPHY_TOOL_T313_STATUS_IND_STRU  */
    /* T312 状态上报 */
    ID_WPHY_TOOL_T312_STATUS_IND                = 0xF043,                       /* _H2ASN_MsgChoice WPHY_TOOL_T312_STATUS_IND_STRU */

    /* PA状态上报 */
    ID_TOOL_WPHY_PA_STATUS_REPORT_REQ           = 0xF046,                       /* _H2ASN_MsgChoice WPHY_TOOL_PA_STATUS_REPORT_REQ_STRU */
    ID_WPHY_TOOL_PA_STATUS_REPORT_CNF           = 0xF047,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */
    ID_WPHY_TOOL_PA_STATUS_REPORT_IND           = 0xF048,                       /* _H2ASN_MsgChoice WPHY_TOOL_PA_STATUS_REPORT_IND_STRU */

    ID_TOOL_WPHY_INTRA_MSG_RPT_CTRL_REQ         = 0xF049,                       /* _H2ASN_MsgChoice WPHY_TOOL_INTRA_MSG_RPT_CTRL_REQ_STRU */
    ID_WPHY_TOOL_INTRA_MSG_RPT_CTRL_CNF         = 0xF04A,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */

    /* 前端可维可测 */
    ID_TOOL_WPHY_RF_FRONT_STATIC_REQUIRE_REQ    = 0xF070,                       /* _H2ASN_MsgChoice WPHY_TOOL_RF_FRONT_END_STATIC_REQUIRE_REQ_STRU  */
    ID_WPHY_TOOL_RF_FRONT_STATIC_REQUIRE_IND    = 0xF071,                       /* _H2ASN_MsgChoice WPHY_TOOL_RF_FRONT_END_STATIC_REQUIRE_IND_STRU  */

    ID_TOOL_WPHY_BBP_RTC_REQ                    = 0xF072,                       /* _H2ASN_MsgChoice WPHY_TOOL_BBP_RTC_REQ_STRU  */
    ID_WPHY_TOOL_BBP_RTC_CNF                    = 0xF073,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU  */

    ID_TOOL_WPHY_RF_FRONT_DISPLAY_REQ           = 0xF075,                       /* _H2ASN_MsgChoice WPHY_TOOL_RF_FRONT_END_DISPLAY_REQ_STRU  */
    ID_WPHY_TOOL_RF_FRONT_DISPLAY_CNF           = 0xF077,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU */


    ID_TOOL_WPHY_RAM_REPORT_REQ                 = 0xF078,                       /* _H2ASN_MsgChoice WPHY_TOOL_RAM_REPORT_REQ_STRU  */
    ID_WPHY_TOOL_RAM_REPORT_CNF                 = 0xF079,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU  */
    ID_WPHY_TOOL_RAM_REPORT_IND                 = 0xF07A,                       /* _H2ASN_MsgChoice WPHY_TOOL_RAM_REPORT_IND_STRU  */

    ID_TOOL_WPHY_BLK_REPORT_LIST_REQ            = 0xF07B,                       /* _H2ASN_MsgChoice WPHY_TOOL_BLK_REPORT_LIST_REQ_STRU  */
    ID_WPHY_TOOL_BLK_REPORT_LIST_CNF            = 0xF07C,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU  */
    ID_WPHY_TOOL_BLK_REPORT_LIST_IND            = 0xF07D,                       /* _H2ASN_MsgChoice WPHY_TOOL_BLK_REPORT_LIST_IND_STRU  */

    ID_TOOL_WPHY_RA_MNTN_CTRL_REQ               = 0xF07E,                       /* _H2ASN_MsgChoice WPHY_TOOL_RA_MNTN_CTRL_REQ_STRU */
    ID_WPHY_TOOL_RA_MNTN_CTRL_CNF               = 0xF07F,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU  */

    ID_TOOL_WPHY_ARM_0MS_MNTN_CTRL_REQ          = 0xF080,                       /* _H2ASN_MsgChoice WPHY_TOOL_ARM_0MS_MNTN_CTRL_REQ_STRU */
    ID_WPHY_TOOL_ARM_0MS_MNTN_CTRL_CNF          = 0xF081,                       /* _H2ASN_MsgChoice WPHY_TOOL_COMMON_CNF_STRU  */

    ID_WPHY_TOOL_SLAVE_CDRX_START_IND           = 0xF082,                       /* _H2ASN_MsgChoice WPHY_TOOL_SLAVE_CDRX_START_IND_STRU  */
    ID_WPHY_TOOL_SLAVE_CDRX_STOP_IND            = 0xF083,                       /* _H2ASN_MsgChoice WPHY_TOOL_SLAVE_CDRX_STOP_IND_STRU  */
    ID_WPHY_TOOL_SLAVE_W_FINISH_IND             = 0xF084,                       /* _H2ASN_MsgChoice WPHY_TOOL_SLAVE_W_FINISH_IND_STRU */
    ID_WPHY_TOOL_SLAVE_GRSSI_FINISH_IND         = 0xF085,                       /* _H2ASN_MsgChoice WPHY_TOOL_SLAVE_GRSSI_FINISH_IND_STRU */
    ID_WPHY_TOOL_SLAVE_GBSIC_FINISH_IND         = 0xF086,                       /* _H2ASN_MsgChoice WPHY_TOOL_SLAVE_GBSIC_FINISH_IND_STRU */
    ID_WPHY_TOOL_SLAVE_SCHEDULE_TIMEOUT_IND     = 0xF087,                       /* _H2ASN_MsgChoice WPHY_TOOL_SLAVE_SCHEDULE_TIMEOUT_IND_STRU */

    ID_WPHY_TOOL_MASTER_WAKE_LTE_IND            = 0xF090,                       /* _H2ASN_MsgChoice WPHY_TOOL_MASTER_WAKE_LTE_IND_STRU */
    ID_WPHY_TOOL_MASTER_START_LTE_MEAS_IND      = 0xF091,                       /* _H2ASN_MsgChoice WPHY_TOOL_MASTER_START_LTE_MEAS_IND_STRU */
    ID_WPHY_TOOL_MASTER_REPORT_LTE_MEAS_IND     = 0xF092,                       /* _H2ASN_MsgChoice WPHY_TOOL_MASTER_REPORT_LTE_MEAS_IND_STRU */
    ID_WPHY_TOOL_MASTER_STOP_LTE_MEAS_IND       = 0xF093,                       /* _H2ASN_MsgChoice WPHY_TOOL_MASTER_STOP_LTE_MEAS_IND_STRU */
    ID_WPHY_TOOL_SET_LTE_MEAS_FLAG_IND          = 0xF094,                       /* _H2ASN_MsgChoice WPHY_TOOL_SET_LTE_MEAS_FLAG_IND_STRU */

    ID_WPHY_TOOL_QPC_OPEN_UL_IND                = 0xF0A0,                       /* _H2ASN_MsgChoice WPHY_TOOL_QPC_OPEN_UL_IND_STRU */
    ID_WPHY_TOOL_QPC_CLOSE_UL_IND               = 0xF0A1,                       /* _H2ASN_MsgChoice WPHY_TOOL_QPC_CLOSE_UL_IND_STRU */
    ID_WPHY_TOOL_QPC_RRC_STATE_IND              = 0xF0A2,                       /* _H2ASN_MsgChoice WPHY_TOOL_QPC_RRC_STATE_IND_STRU */
    ID_WPHY_TOOL_QPC_RB_TYPE_IND                = 0xF0A3,                       /* _H2ASN_MsgChoice WPHY_TOOL_QPC_RB_TYPE_IND_STRU */

    ID_WPHY_TOOL_DPDT_ESTIMATE_STATE_IND        = 0xF0B0,                       /* _H2ASN_MsgChoice WPHY_TOOL_DPDT_ESTIMATE_STATE_IND_STRU */
    ID_WPHY_TOOL_DPDT_COMPARE_STATE_IND         = 0xF0B1,                       /* _H2ASN_MsgChoice WPHY_TOOL_DPDT_COMPARE_STATE_IND_STRU */
    ID_WPHY_TOOL_DPDT_PROTECT_STATE_IND         = 0xF0B2,                       /* _H2ASN_MsgChoice WPHY_TOOL_DPDT_PROTECT_STATE_IND_STRU */

    ID_WPHY_TOOL_MSG_ID_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16;


/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_RSLT_ENUM_UINT16
 协议表格  :
 枚举说明  : 各种回复消息的结果值:成功或者失败等
*****************************************************************************/
enum WPHY_TOOL_MNTN_RSLT_ENUM
{
    WPHY_MNTN_RSLT_SUCC                         = 0,                            /* 成功 */
    WPHY_MNTN_RSLT_FAIL                         = 1,                            /* 失败 */
    WPHY_MNTN_RSLT_PERIOD_NOT_SUPPORT           = 2,                            /* 配置周期不支持 */
    WPHY_MNTN_RSLT_PARA_INVALID                 = 3,                            /* 设置参数无效 */
    WPHY_MNTN_RSLT_PARA_TYPE_OVERFLOW           = 4,                            /* 设置参数类型不正确 */

    WPHY_MNTN_RSLT_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_RSLT_ENUM_UINT16;





/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_REPORT_TYPE_ENUM_UINT16
 协议表格  :
 枚举说明  : 各种测量报告上报周期
*****************************************************************************/
enum WPHY_TOOL_MNTN_REPORT_TYPE_ENUM
{
    WPHY_MNTN_REPORT_TYPE_NO_RPT                = 0,                            /* 不上报: */
    WPHY_MNTN_REPORT_TYPE_ONE_TIME              = 1,                            /* 上报1次: */
    WPHY_MNTN_REPORT_TYPE_PER_FRAME             = 2,                            /* 每帧上报: */
    WPHY_MNTN_REPORT_TYPE_ASSIGNED_PERIOD       = 3,                            /* 指定周期:以帧为单位。如果选择此项，则必须填写下一个变量来指定周期。 */
    WPHY_MNTN_REPORT_TYPE_EVENT_TRIGER          = 4,                            /* 事件触发: */

    WPHY_MNTN_REPORT_TYPE_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_REPORT_TYPE_ENUM_UINT16;


/*****************************************************************************
 枚举名    : WPHY_MNTN_PWR_SWITCH_TYPE_ENUM_UINT16
 协议表格  :
 枚举说明  : 开关控制的枚举值
*****************************************************************************/
enum WPHY_TOOL_MNTN_PWR_SWITCH_TYPE_ENUM
{
    WPHY_MNTN_PWR_SWITCH_TYPE_UL_PC             = 0,                            /* 上行功率控制:固定上行发射东率 */
    WPHY_MNTN_PWR_SWITCH_TYPE_OLPC              = 1,                            /* 外环功控 */
    WPHY_MNTN_PWR_SWITCH_TYPE_WINUP             = 2,                            /* winup控制 */
    WPHY_MNTN_PWR_SWITCH_TYPE_WINDOWN           = 3,                            /* windown控制 */
    WPHY_MNTN_PWR_SWITCH_TYPE_FIX_DL_PWR        = 4,                            /* 固定下行发射功率 */

    WPHY_MNTN_PWR_SWITCH_TYPE_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_PWR_SWITCH_TYPE_ENUM_UINT16;


/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_SWITCH_CTRL_ENUM_UINT16
 协议表格  :
 枚举说明  : 开关控制的枚举值
*****************************************************************************/
enum WPHY_TOOL_MNTN_SWITCH_CTRL_ENUM
{
    WPHY_MNTN_SWITCH_CTRL_CLOSE                 = 0,                            /* 关闭 */
    WPHY_MNTN_SWITCH_CTRL_OPEN                  = 1,                            /* 打开 */

    WPHY_MNTN_SWITCH_CTRL_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_SWITCH_CTRL_ENUM_UINT16;


/*****************************************************************************
 枚举名    : WPHY_TOOL_SYNC_STATUS_ENUM_UINT16
 协议表格  :
 枚举说明  : 开关控制的枚举值
*****************************************************************************/
enum WPHY_TOOL_SYNC_STATUS_ENUM
{
    WPHY_MNTN_SYNC_STATUS_FIX                   = 0,                            /* 锁定 */
    WPHY_MNTN_SYNC_STATUS_LOST                  = 1,                            /* 失锁 */

    WPHY_MNTN_SYNC_STATUS_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_SYNC_STATUS_ENUM_UINT16;


/*****************************************************************************
 枚举名    : WPHY_TOOL_TCXO_STATUS_ENUM_UINT16
 协议表格  :
 枚举说明  : 开关控制的枚举值
*****************************************************************************/
enum WPHY_TOOL_TCXO_STATUS_ENUM
{
    WPHY_MNTN_TCXO_STATUS_FREEZE                = 0,                            /* 锁定 */
    WPHY_MNTN_TCXO_STATUS_UNFREEZE              = 1,                            /* 失锁 */

    WPHY_MNTN_TCXO_STATUS_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_TCXO_STATUS_ENUM_UINT16;



/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_PWR_PARA_ENUM_UINT32
 协议表格  :
 枚举说明  : 功率参数类型，查询和设置公用
*****************************************************************************/
enum WPHY_TOOL_MNTN_PWR_PARA_ENUM
{
    WPHY_MNTN_PWR_PARA_SIR_TARGET               = 0x00000001,                   /* SirTarget */
    WPHY_MNTN_PWR_PARA_CURR_TRCH_ID_SELECTED    = 0x00000002,                   /* 当前选择的TrchId */
    WPHY_MNTN_PWR_PARA_DL_TPC                   = 0x00000004,                   /* DL TPC */
    WPHY_MNTN_PWR_PARA_CURR_SIR                 = 0x00000008,                   /* 当前的SIR */
    WPHY_MNTN_PWR_PARA_CURR_BLER                = 0x00000010,                   /* 当前的BLER */
    WPHY_MNTN_PWR_PARA_UL_TPC                   = 0x00000020,                   /* UL TPC */
    WPHY_MNTN_PWR_PARA_BD                       = 0x00000040,                   /* Bd */
    WPHY_MNTN_PWR_PARA_BEC                      = 0x00000080,                   /* Bec */
    WPHY_MNTN_PWR_PARA_BED                      = 0x00000100,                   /* Bed */
    WPHY_MNTN_PWR_PARA_BHS_NACK                 = 0x00000200,                   /* BhsNack */
    WPHY_MNTN_PWR_PARA_BHS_ACK                  = 0x00000400,                   /* BhsAck */
    WPHY_MNTN_PWR_PARA_BHS_CQI                  = 0x00000800,                   /* BhsCQI */
    WPHY_MNTN_PWR_PARA_BHS_TFCI                 = 0x00001000,                   /* TFCI */
    WPHY_MNTN_PWR_PARA_SLOT_FORMAT              = 0x00002000,                   /* SlotFormat */
    WPHY_MNTN_PWR_PARA_MAX_PWR                  = 0x00004000,                   /* 最大发射功率 */
    WPHY_MNTN_PWR_PARA_MIN_PWR                  = 0x00008000,                   /* 最小发射功率 */
    WPHY_MNTN_PWR_PARA_INIT_PWR                 = 0x00010000,                   /* lint !e575 初始发射功率 */

    WPHY_MNTN_PWR_PARA_BUTT
};
typedef VOS_UINT32 WPHY_TOOL_MNTN_PWR_PARA_ENUM_UINT32;



/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_EVENT_TYPE_ENUM_UINT32
 协议表格  :
 枚举说明  : 上报事件类型
*****************************************************************************/
enum WPHY_TOOL_MNTN_EVENT_TYPE_ENUM
{
    WPHY_MNTN_EVENT_TYPE_AFC_STATUS             = 0x00000001,                   /* 6ms 任务超时 */
    WPHY_MNTN_EVENT_TYPE_SYNC_STATUS            = 0x00000002,                   /* 9ms 任务超时 */
    WPHY_MNTN_EVENT_TYPE_L1_STATUS              = 0x00000004,                   /* L1状态 */
    WPHY_MNTN_EVENT_TYPE_INT_EXCEPT             = 0x00000008,                   /* 中断异常 */

    WPHY_MNTN_EVENT_TYPE_BUTT
};
typedef VOS_UINT32 WPHY_TOOL_MNTN_EVENT_TYPE_ENUM_UINT32;




/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_INT_EXCEPT_ENUM_UINT16
 协议表格  :
 枚举说明  : 中断异常类型
*****************************************************************************/
enum WPHY_TOOL_MNTN_INT_EXCEPT_ENUM
{
    WPHY_MNTN_INT_EXCEPT_TYPE_6MS_TASK_EXPIRED  = 0,                            /* 6ms 任务超时 */
    WPHY_MNTN_INT_EXCEPT_TYPE_9MS_TASK_EXPIRED  = 1,                            /* 9ms 任务超时 */

    WPHY_MNTN_INT_EXCEPT_TYPE_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_INT_EXCEPT_ENUM_UINT16;



/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_SYS_STATUS_ENUM_UINT16
 协议表格  :
 枚举说明  : 系统状态
*****************************************************************************/
enum WPHY_TOOL_MNTN_SYS_STATUS_ENUM
{
    WPHY_MNTN_SYS_STATUS_FREE                   = 0,
    WPHY_MNTN_SYS_STATUS_CPICH                  = 1,
    WPHY_MNTN_SYS_STATUS_FACH                   = 2,
    WPHY_MNTN_SYS_STATUS_DPCH                   = 3,

    WPHY_MNTN_SYS_STATUS_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_SYS_STATUS_ENUM_UINT16;



/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_THRESHOLD_TYPE_ENUM_UINT32
 协议表格  :
 枚举说明  : 可配置门限类型
*****************************************************************************/
enum WPHY_TOOL_MNTN_THRESHOLD_TYPE_ENUM
{
    /* 小区搜索门限 */
    WPHY_MNTN_THRESHOLD_TYPE_STEP1_NOISE                    = 0,
    WPHY_MNTN_THRESHOLD_TYPE_STEP2_NOISE                       ,
    WPHY_MNTN_THRESHOLD_TYPE_STEP3_NOISE_AFC_UNLOCK            ,
    WPHY_MNTN_THRESHOLD_TYPE_STEP3_NOISE_AFC_LOCK              ,
    WPHY_MNTN_THRESHOLD_TYPE_STEPB_NOISE                       ,

    WPHY_MNTN_THRESHOLD_TYPE_STEP1_FAIL_FOR_SPEC_FREQ          ,
    WPHY_MNTN_THRESHOLD_TYPE_STEP1_FAIL_FOR_BAND               ,

    WPHY_MNTN_THRESHOLD_TYPE_STEP1_CORRELEN_FOR_INIT_CS        ,

    WPHY_MNTN_THRESHOLD_TYPE_STEP2_RSDEC_LEN                   ,
    WPHY_MNTN_THRESHOLD_TYPE_STEP3_CORRELEN_AFC_UNLOCK         ,
    WPHY_MNTN_THRESHOLD_TYPE_STEP3_CORRELEN_AFC_LOCK           ,
    WPHY_MNTN_THRESHOLD_TYPE_STEP3_NCORRELEN_AFC_UNLOCK        ,
    WPHY_MNTN_THRESHOLD_TYPE_STEP3_NCORRELEN_AFC_LOCK          ,

    WPHY_MNTN_THRESHOLD_TYPE_STEPB_CORRELEN                    ,

    /*  多径搜索门限 */
    WPHY_MNTN_THRESHOLD_TYPE_CORRE_LENTH_LOCK                  ,
    WPHY_MNTN_THRESHOLD_TYPE_INCORRE_LENTH_LOCK                ,
    WPHY_MNTN_THRESHOLD_TYPE_CORRE_LENTH_UNLOCK                ,
    WPHY_MNTN_THRESHOLD_TYPE_INCORRE_LENTH_UNLOCK              ,

    /* 测量门限 */
    WPHY_MNTN_THRESHOLD_TYPE_MEAS_AVERAGE_NOISE                ,
    WPHY_MNTN_THRESHOLD_TYPE_MP_RELATIVE_FACTOR                ,

    WPHY_MNTN_THRESHOLD_TYPE_MEAS_TIMING_ECNO_THRESHOLD        ,
    WPHY_MNTN_THRESHOLD_TYPE_MEAS_TIMING_RSCP_THRESHOLD        ,

    WPHY_MNTN_THRESHOLD_TYPE_MAX_TIMING_MEAS_FRAME_NUMBER      ,

    /* 功控门限 */
    WPHY_MNTN_THRESHOLD_TYPE_SIR_THRESHOLD_PILOT1              ,
    WPHY_MNTN_THRESHOLD_TYPE_SIR_THRESHOLD_PILOT2              ,

    WPHY_MNTN_THRESHOLD_TYPE_TPC_THRESHOLD_TTI1OR2             ,
    WPHY_MNTN_THRESHOLD_TYPE_TPC_THRESHOLD_TTI4                ,
    WPHY_MNTN_THRESHOLD_TYPE_TPC_THRESHOLD_TTI8                ,

    WPHY_MNTN_THRESHOLD_TYPE_INCORRE_CPU_QIN                   ,
    WPHY_MNTN_THRESHOLD_TYPE_INCORRE_CPU_QOUT                  ,
    WPHY_MNTN_THRESHOLD_TYPE_INCORRE_SS_THRESHOLD              ,
    WPHY_MNTN_THRESHOLD_TYPE_INCORRE_MRC_THRESHOLD             ,
    WPHY_MNTN_THRESHOLD_TYPE_INCORRE_INT_THRESHOLD             ,
    WPHY_MNTN_THRESHOLD_TYPE_INCORRE_ML_THRESHOLD              ,

    /* DRX功能门限 */
    WPHY_MNTN_THRESHOLD_TYPE_32K_CLOCK_MSR_WAIT_SFN_NUM        ,
    WPHY_MNTN_THRESHOLD_TYPE_SLEEP_LENTH                       ,
    WPHY_MNTN_THRESHOLD_TYPE_TIME_ADJUST                       ,

    WPHY_MNTN_THRESHOLD_TYPE_BUTT
};
typedef VOS_UINT32 WPHY_TOOL_MNTN_THRESHOLD_TYPE_ENUM_UINT32;


/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_PA_STATUS_ENUM
 协议表格  :
 枚举说明  : 开关控制的枚举值
*****************************************************************************/
enum WPHY_TOOL_MNTN_PA_STATUS_ENUM
{
    WPHY_MNTN_PA_STATUS_CLOSE                       = 0,                        /* 关闭 */
    WPHY_MNTN_PA_STATUS_OPEN                        = 1,                        /* 打开 */

    WPHY_MNTN_PA_STATUS_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_PA_STATUS_ENUM_UINT16;


/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_RPT_ADDR_ENUM_UINT16
 协议表格  :
 枚举说明  : WPHY可维可测固定抓取的几块数据枚举
*****************************************************************************/
enum WPHY_TOOL_MNTN_RAM_RPT_ENUM
{
    WPHY_MNTN_RPT_HSUPA_SHARE_CTRL                  = 0,                        /* UPA控制 */
    WPHY_MNTN_RPT_DSP_MODE                          = 1,                        /* DSP模式 */
    WPHY_MNTN_RPT_HSUPA_RPT                         = 2,
    WPHY_MNTN_RPT_HSUPA_TTI                         = 3,
    WPHY_MNTN_RPT_CPC_DRX_RPT                       = 4,
    WPHY_MNTN_RPT_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_RAM_RPT_ENUM_UINT16;


/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_ERROR_RPT_LEVEL_ENUM_UINT16
 协议表格  :
 枚举说明  : WPHY错误上报级别
*****************************************************************************/
enum WPHY_TOOL_MNTN_ERROR_RPT_LEVEL_ENUM
{
    WPHY_MNTN_ERROR_LEVEL_INFO                      = 1,
    WPHY_MNTN_ERROR_LEVEL_WARNING                   = 2,
    WPHY_MNTN_ERROR_LEVEL_ERROR                     = 3,

    WPHY_MNTN_ERROR_LEVEL_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_ERROR_RPT_LEVEL_ENUM_UINT16;




/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_PA_STATUS_ENUM
 协议表格  :
 枚举说明  : 开关控制的枚举值
*****************************************************************************/
enum WPHY_TOOL_MNTN_RPT_STATUS_ENUM
{
    WPHY_MNTN_RPT_STATUS_CLOSE                      = 0,                        /* 关闭 */
    WPHY_MNTN_RPT_STATUS_OPEN                       = 1,                        /* 打开 */

    WPHY_MNTN_RPT_STATUS_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_RPT_STATUS_ENUM_UINT16;

/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_RSLT_ENUM_UINT16
 协议表格  :
 枚举说明  : T313 Status
ASN.1描述 :
 枚举说明  : T313定时器 状态
*****************************************************************************/
enum WPHY_TOOL_MNTN_T313_STATUS_ENUM
{
    WPHY_MNTN_T313_START                        = 0,
    WPHY_MNTN_T313_STOP                         = 1,
    WPHY_MNTN_T313_TIMEOUT                      = 2,

    WPHY_MNTN_T313_STATUS_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_T313_STATUS_ENUM_UINT16;

/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_RSLT_ENUM_UINT16
 协议表格  :
 枚举说明  : T312 Status
ASN.1描述 :
 枚举说明  : T312定时器 状态
*****************************************************************************/
enum WPHY_TOOL_MNTN_T312_STATUS_ENUM
{
    WPHY_MNTN_T312_START                        = 0,
    WPHY_MNTN_T312_STOP                         = 1,
    WPHY_MNTN_T312_TIMEOUT                      = 2,

    WPHY_MNTN_T312_STATUS_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_T312_STATUS_ENUM_UINT16;

/*****************************************************************************
 枚举名    : WPHY_TOOL_MNTN_MASTER_STOP_LTE_REASION_ENUM
 协议表格  :
 枚举说明  : 强停LTE原因
ASN.1描述 :
 枚举说明  : 强停LTE原因

*****************************************************************************/
enum WPHY_TOOL_MNTN_MASTER_STOP_LTE_REASION_ENUM
{
    WPHY_TOOL_MNTN_MASTER_STOP_LTE_REASION_BG               = 0,
    WPHY_TOOL_MNTN_MASTER_STOP_LTE_REASION_MEAS_TIMEOUT     = 1,
    WPHY_TOOL_MNTN_MASTER_STOP_LTE_REASION_BG_TIMEOUT       = 2,
    WPHY_TOOL_MNTN_MASTER_STOP_LTE_REASION_HIGH_TASK        = 3,
    WPHY_TOOL_MNTN_MASTER_STOP_LTE_REASION_INIT             = 4,

    WPHY_TOOL_MNTN_MASTER_STOP_LTE_REASION_BUTT
};
typedef VOS_UINT16 WPHY_TOOL_MNTN_MASTER_STOP_LTE_REASION_ENUM_UINT16;

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
  6.0 STRUCT定义,WG公共定义
*****************************************************************************/

/*****************************************************************************
 结构名    : UPHY_TOOL_TYPE_HEADER_STRU
 枚举说明  : 工具解析结构头
*****************************************************************************/

typedef struct
{
    VOS_UINT16                                  usFuncType;                     /* 可维可测是0x4,内部消息抓取是0x */
    VOS_UINT16                                  usLength;                       /* 之后的长度,单位为byte */
    VOS_UINT32                                  ulSn;
    VOS_UINT32                                  ulTimeSlice;
}UPHY_TOOL_TYPE_HEADER_STRU;



/*****************************************************************************
 结构名    : UPHY_TOOL_OM_MSG_HEADER_STRU
 枚举说明  : 消息头结构定义
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                  usMsgId;
    VOS_UINT16                                  usReserved;
}UPHY_TOOL_OM_MSG_HEADER_STRU;




/*****************************************************************************
 结构名    : UPHY_TOOL_INTRA_MSG_HEADER_STRU
 枚举说明  : 消息头结构定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                                  usMsgId;
    VOS_UINT16                                  usReserved;
}UPHY_TOOL_INTRA_MSG_HEADER_STRU;



/*****************************************************************************
  6.1 STRUCT定义,UPHY定义
*****************************************************************************/



/*****************************************************************************
 结构名    : GPHY_TOOL_SDT_LINK_STATUS_REQ
 结构说明  : TOOL告诉GPHY,SDT连接状态
*****************************************************************************/


/*****************************************************************************
 结构名    : GPHY_TOOL_COMMON_CNF_STRU
 结构说明  : GPHY可维可测公共回复消息结构
*****************************************************************************/
typedef struct
{
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip *//* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    UPHY_TOOL_MNTN_RSLT_ENUM_UINT16             enResult;                       /* 接收原语的处理结果ID */
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
}UPHY_TOOL_COMMON_CNF_STRU;


/*****************************************************************************
 结构名    : UPHY_TOOL_QUERY_SHARE_ADDR_REQ_STRU
 结构说明  : 共享地址查询请求原语
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT32                                  ulParaValue;                    /* 是否控制该参数 */
}UPHY_TOOL_QUERY_SHARE_ADDR_REQ_STRU;



/*****************************************************************************
 结构名    : UPHY_TOOL_QUERY_SHARE_ADDR_IND_STRU
 结构说明  : UPHY共享地址查询请求回复
*****************************************************************************/
typedef struct
{
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT32                                  ulShareAddr;                    /* 共享首地址 */

}UPHY_TOOL_QUERY_SHARE_ADDR_IND_STRU;


/*****************************************************************************
 结构名    : UPHY_TOOL_SDT_LINK_STATUS_REQ_STRU
 结构说明  : TOOL告诉UPHY,SDT连接状态
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSdtStatus;
    VOS_UINT16                                  usRsv;
}UPHY_TOOL_SDT_LINK_STATUS_REQ_STRU;

/*****************************************************************************
 结构名    : UPHY_TOOL_ERROR_REPORT_REQ_STRU
 结构说明  : 错误上报
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    UPHY_TOOL_MNTN_RPT_LEVEL_ENUM_UINT16        enErrorRptLevel;                /* 控制上报错误级别 */
    VOS_UINT16                                  usRsv;
}UPHY_TOOL_ERROR_REPORT_REQ_STRU;



/*****************************************************************************
 结构名    : UPHY_TOOL_ERROR_REPORT_IND_STRU
 结构说明  : 错误上报
*****************************************************************************/
typedef struct
{
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usFileNo;                       /* 文件名编码 */
    VOS_UINT16                                  usLineNo;                       /* 出错的行号 */
    VOS_UINT32                                  ulTimeSlice;
    VOS_UINT32                                  ulLogId;                        /* PHY软件内统一编号LOG ID */
    VOS_UINT16                                  usReserved;                     /* 保留字段 */
    VOS_UINT16                                  usVarCnt;                       /* 上报变量个数 */
    VOS_UINT32                                  aulVarValue[UPHY_TOOL_MNTN_ERROR_RPT_VAR_NUM]; /* 上报变量值 */

}UPHY_TOOL_ERROR_REPORT_IND_STRU;



/*****************************************************************************
 结构名    : UPHY_TOOL_WPHY_INTER_GREG_REPORT_REQ_STRU
 结构说明  : WPHY下异系统G寄存器列表下发
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usRsv;                         /* 控制上报错误级别 */
    VOS_UINT16                                  usRegNum;                       /* 控制上报错误级别 */
    VOS_UINT32                                  aulRegAddr[UPHY_TOOL_INTER_REG_MAX_NUM];
}UPHY_TOOL_WPHY_INTER_GREG_REPORT_REQ_STRU;



/*****************************************************************************
 结构名    : UPHY_TOOL_WPHY_INTER_GREG_REPORT_CNF_STRU
 结构说明  : WPHY下异系统G寄存器收到回复
*****************************************************************************/
typedef struct
{
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    UPHY_TOOL_MNTN_RSLT_ENUM_UINT16             enResult;                       /* 接收原语的处理结果ID */
    VOS_UINT16                                  usRegNum;                       /* 控制上报错误级别 */
    VOS_UINT32                                  aulRegAddr[UPHY_TOOL_INTER_REG_MAX_NUM];
}UPHY_TOOL_WPHY_INTER_GREG_REPORT_CNF_STRU;


/*****************************************************************************
 结构名    : UPHY_TOOL_WPHY_INTER_GREG_REPORT_IND_STRU
 结构说明  : WPHY下异系统G寄存器值上报
*****************************************************************************/
typedef struct
{
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usRsv;                         /* 控制上报错误级别 */
    VOS_UINT16                                  usRegNum;                       /* 控制上报错误级别 */
    VOS_UINT32                                  aulRegData[UPHY_TOOL_INTER_REG_MAX_NUM];
}UPHY_TOOL_WPHY_INTER_GREG_REPORT_IND_STRU;



/*****************************************************************************
 结构名    : UPHY_TOOL_WPHY_INTER_GREG_STOP_REQ_STRU
 结构说明  : 停止WPHY下异系统G寄存器上报请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT32                                  ulRsv;                          /* 控制上报错误级别 */
}UPHY_TOOL_WPHY_INTER_GREG_STOP_REQ_STRU;




/*****************************************************************************
 结构名    : UPHY_TOOL_GPHY_INTER_WREG_REPORT_REQ_STRU
 结构说明  : GPHY下异系统W寄存器列表下发
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usRsv;                         /* 控制上报错误级别 */
    VOS_UINT16                                  usRegNum;                       /* 控制上报错误级别 */
    VOS_UINT32                                  aulRegAddr[UPHY_TOOL_INTER_REG_MAX_NUM];
}UPHY_TOOL_GPHY_INTER_WREG_REPORT_REQ_STRU;



/*****************************************************************************
 结构名    : UPHY_TOOL_WPHY_INTER_GREG_REPORT_CNF_STRU
 结构说明  : GPHY下异系统W寄存器收到回复
*****************************************************************************/
typedef struct
{
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    UPHY_TOOL_MNTN_RSLT_ENUM_UINT16             enResult;                       /* 接收原语的处理结果ID */
    VOS_UINT16                                  usRegNum;                       /* 控制上报错误级别 */
    VOS_UINT32                                  aulRegAddr[UPHY_TOOL_INTER_REG_MAX_NUM];
}UPHY_TOOL_GPHY_INTER_WREG_REPORT_CNF_STRU;


/*****************************************************************************
 结构名    : UPHY_TOOL_WPHY_INTER_GREG_REPORT_IND_STRU
 结构说明  : GPHY下异系统W寄存器值上报
*****************************************************************************/
typedef struct
{
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usRsv;                         /* 控制上报错误级别 */
    VOS_UINT16                                  usRegNum;                       /* 控制上报错误级别 */
    VOS_UINT32                                  aulRegData[UPHY_TOOL_INTER_REG_MAX_NUM];
}UPHY_TOOL_GPHY_INTER_WREG_REPORT_IND_STRU;



/*****************************************************************************
 结构名    : UPHY_TOOL_GPHY_INTER_WREG_STOP_REQ_STRU
 结构说明  : 停止GPHY下异系统W寄存器上报请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT32                                  ulRsv;                          /* 控制上报错误级别 */
}UPHY_TOOL_GPHY_INTER_WREG_STOP_REQ_STRU;


/*****************************************************************************
 结构名    : UPHY_TOOL_DRX_LATE_INFO_IND_STRU
 结构说明  : 唤醒晚时的打点信息
*****************************************************************************/
typedef struct
{
    UPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usRsv;                                          /* 保留字段 */
    VOS_UINT32                                  ulAwakeTime;                                    /* 需要唤醒的时间，W下为SFN，G下为Slice */
    VOS_UINT32                                  ulCurTime;                                      /* 实际唤醒的时间，W下为SFN，G下为Slice */
    VOS_UINT32                                  aulAwakeHWInfo[UPHY_TOOL_SLEEP_INFO_NUM];       /* OAM可维可测消息 */
}UPHY_TOOL_DRX_LATE_INFO_IND_STRU;


/*****************************************************************************
  6.2 STRUCT定义,GPHY定义
*****************************************************************************/

/*****************************************************************************
 结构名    : GPHY_TOOL_COMMON_CNF_STRU
 结构说明  : GPHY可维可测公共回复消息结构
*****************************************************************************/
typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip *//* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    GPHY_TOOL_MNTN_RSLT_ENUM_UINT16             enResult;                       /* 接收原语的处理结果ID */
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
}GPHY_TOOL_COMMON_CNF_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_BLER_REPORT_REQ_STRU
 结构说明  : GPHY GPRS信道的误码率上报请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usBlerFlag;                     /* 是否上报BLER */
    VOS_UINT16                                  usRptPeriod;                    /* BLER上报的周期,单位:块数 */
}GPHY_TOOL_BLER_REPORT_REQ_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_BLER_REPROT_IND_STRU
 结构说明  : GPHY上报BLER的结构和子结构
*****************************************************************************/

typedef struct
{
    VOS_UINT32                                  ulBlkCnt;
    VOS_UINT32                                  ulErrBlkCnt;
}GPHY_BLER_PARA_STRU;

typedef struct
{
    VOS_UINT32                                  ulRcvBlkCnt;
    VOS_UINT32                                  ulDummyCnt;
    GPHY_BLER_PARA_STRU                         stBlerPara[GPHY_PS_MAX_RX_TSN_NUM];
}GPHY_GPRS_BLER_RECORD_STRU;



typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;
    VOS_UINT16                                  usToolsId;
    GPHY_GPRS_BLER_RECORD_STRU                  stGprsBlerRecord;
}GPHY_TOOL_BLER_REPORT_IND_STRU;



/*****************************************************************************
 结构名    : GPHY_TOOL_SNR_REPORT_REQ_STRU
 结构说明  : GPHY GPRS信道的SNR上报请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSNRFlag;                      /* 是否上报SNR */
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
}GPHY_TOOL_SNR_REPORT_REQ_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_SNR_REPROT_IND_STRU
 结构说明  : GPHY上报SNR的结构和子结构
*****************************************************************************/

typedef struct
{
    VOS_UINT16                                  usRptValidNum;
    VOS_UINT16                                  usRsv;
    VOS_INT16                                   sSNR[GPHY_PS_MAX_RX_TSN_NUM];
    VOS_UINT16                                  usRsv2;
}GPHY_GPRS_SNR_RECORD_STRU;

/* 可维可测:SNR上报结构 */
typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;
    VOS_UINT16                                  usToolsId;
    GPHY_GPRS_SNR_RECORD_STRU                   stGprsSNRRecord;
}GPHY_TOOL_SNR_REPORT_IND_STRU;



/*****************************************************************************
 结构名    : GPHY_TOOL_AFC_REPORT_REQ_STRU
 结构说明  : GPHY AFC积分值信息上报请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usAFCFlag;                      /* 是否上报AFC值 */
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
}GPHY_TOOL_AFC_REPORT_REQ_STRU;



/*****************************************************************************
 结构名    : GPHY_TOOL_AFC_REPROT_IND_STRU
 结构说明  : GPHY上报AFC的结构
*****************************************************************************/
typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;
    VOS_UINT16                                  usToolsId;
    VOS_INT16                                   sAFCOffset;
    VOS_UINT16                                  usRsv;
}GPHY_TOOL_AFC_REPORT_IND_STRU;




/*****************************************************************************
 结构名    : GPHY_TOOL_POWER_CTRL_REPORT_REQ_STRU
 结构说明  : GPHY 数传下功控信息上报请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usPwrCtrlFlag;                  /* 是否上报功率控制 */
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
}GPHY_TOOL_POWER_CTRL_REPORT_REQ_STRU;



/*****************************************************************************
 结构名    : GPHY_GPRS_POWER_CTRL_REPROT_IND_STRU
 结构说明  : GPHY上报功率控制参数的结构和子结构
*****************************************************************************/

typedef struct
{
    VOS_UINT16                                  usTxChannel;
    VOS_UINT16                                  usTxTn;
    VOS_INT16                                   sTxPwr;
    VOS_UINT16                                  usGammaTn;
    VOS_UINT16                                  usGammaBand;
    VOS_UINT16                                  usAlpha;
    VOS_UINT16                                  usPMax;
    VOS_UINT16                                  usRsv;
}GPHY_PWR_CTRL_PARA_STRU;

typedef struct
{
    VOS_UINT16                                  usTxCnt;
    VOS_UINT16                                  usRsv;
    GPHY_PWR_CTRL_PARA_STRU                     stPwrCtrlPara[GPHY_PS_MAX_TX_TSN_NUM];
}GPHY_POWER_CONTROL_RECORD_STRU;


typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;
    VOS_UINT16                                  usToolsId;
    GPHY_POWER_CONTROL_RECORD_STRU              stPwrCtrlRecord;
}GPHY_TOOL_POWER_CTRL_REPORT_IND_STRU;




/*****************************************************************************
 结构名    : GPHY_TOOL_POWER_CTRL_SET_REQ_STRU
 结构说明  : GPHY 数传下指定功率发射设置请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usPwrCtrlFlag;                  /* 是否按照指定功率发射 */
    VOS_UINT16                                  usTxPwr;                        /* 指定功率,单位:0.1dB */
}GPHY_TOOL_POWER_CTRL_SET_REQ_STRU;



/*****************************************************************************
 结构名    : GPHY_GPRS_POWER_CTRL_REPROT_IND_STRU
 结构说明  : GPHY上报EGPRS信道质量上报结构
*****************************************************************************/
typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usGmskMeanBep;                  /* 0-31,0xff为无效值 */
    VOS_UINT16                                  us8PSKMeanBep;                  /* 0-31,0xff为无效值 */
    VOS_UINT16                                  usGmskCvBep;                    /* 0-7,0xff为无效值 */
    VOS_UINT16                                  us8PSKCvBep;                    /* 0-7,0xff为无效值 */
}GPHY_TOOL_CHANNEL_QUALITY_IND_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_FUNC_BIT_CTRL_REQ_STRU
 结构说明  : GPHY 可维可测功能控制下发的结构原语:用来控制功能设置请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT32                                  ulFuncBitMask;                  /* 对应Bit位置位,则控制该功能生效 */
}GPHY_TOOL_FUNC_BIT_CTRL_REQ_STRU;



/*****************************************************************************
 结构名    : GPHY_TOOL_PARA_CONTROL_REQ_STRU
 结构说明  : GPHY可维可测参数控制下发的结构原语:用来控制参数设置请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    GPHY_TOOL_PARA_CTRL_ENUM_UINT16             enParaId;                       /* 参数的ID,控制参数的列表 */
    VOS_UINT16                                  usParaCtrlFlg;                  /* 是否控制该参数 */
    VOS_UINT32                                  ulParaValue1;                   /* 控制参数的值 */
    VOS_UINT32                                  ulParaValue2;                   /* 目前保留,便于扩展 */
}GPHY_TOOL_PARA_CONTROL_REQ_STRU;




/*****************************************************************************
 结构名    : GPHY_TOOL_RF_FRONT_STATIC_REQ_STRU
 结构说明  : GPHY 上报前端动态信息上报请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usMsgRptFlag;                   /* 指示消息监控的上报内容是否有变更 */
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
}GPHY_TOOL_RF_FRONT_STATIC_REQ_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_BBP_RTC_REQ_STRU
 结构说明  : RTC功能打开和关闭设置
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    UPHY_TOOL_RTC_DEBUG_ENUM_UINT16             enRtcCtrl;                      /* RTC打开或者关闭 */
    VOS_UINT16                                  usRsv;

}GPHY_TOOL_BBP_RTC_REQ_STRU;



/*****************************************************************************
 结构名    : GPHY_TOOL_RF_FRONT_STATIC_IND_STRU
 结构说明  : GPHY上报前端动态信息上报
*****************************************************************************/
typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */

    VOS_UINT16                                  usRfTrcvOnState;                /* RF总开关，1为打开，0为关闭 */
    VOS_UINT16                                  usRfRxOnOrOff;                  /* 04h[1:0] = 3为开 */
    VOS_UINT16                                  usRxPathselect;                 /* 04h[4:3] = 3为主分集都开，2开主集 */
    VOS_UINT16                                  usAbbRxConfig;                  /* 7为主分集都开，3开主集 */

    VOS_UINT16                                  usRfTxOnOrOff;                  /* 02h[1:0] = 3为开 */
    VOS_UINT16                                  usRfTxSHDN;                     /* RF上行打开控制，0由BBP控制 */

    VOS_UINT16                                  usGpioFuncOn;                   /* 硬件支持GPIO */
    VOS_UINT16                                  usGpioPaMode;                   /* GPIO PA mode */

    VOS_UINT16                                  usMipiFuncOn;                   /* 硬件支持MIPI */
    VOS_UINT16                                  usMipiPaMode;                   /* MIPI PA mode */

    VOS_UINT16                                  usAptFuncOn;                    /* 硬件支持APT */
    VOS_UINT16                                  usAptPaMode;                    /* APT电压mode */
}GPHY_TOOL_RF_FRONT_STATIC_IND_STRU;





/*****************************************************************************
 结构名    : GPHY_TOOL_INTRA_MSG_RPT_CTRL_REQ_STRU
 结构说明  : PHY 内部消息上报控制请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usMsgRptFlag;                   /* 指示内部消息是否上报 */
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
}GPHY_TOOL_INTRA_MSG_RPT_CTRL_REQ_STRU;




/*****************************************************************************
 结构名    : GPHY_TOOL_AMR_REPORT_REQ_STRU
 结构说明  : GPHY 语音下AMR信道编译码速率上报请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usAmrRptFlag;                   /* 是否上报AMR信息 */
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
}GPHY_TOOL_AMR_REPORT_REQ_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_AMR_REPORT_IND_STRU
 结构说明  : GPHY上报AMR信息参数
*****************************************************************************/
typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usCrcErrFlg;                    /* 译码结果0-正确;1-错误 */
    VOS_UINT16                                  usDecodeMode;                   /* 下行编码方式 */
    VOS_UINT16                                  usDecodeRate;                   /* 下行编码速率 */
    VOS_UINT16                                  usEncodeMode;                   /* 上行编码方式 */
    VOS_UINT16                                  usEncodeRate;                   /* 上行编码速率 */
    VOS_UINT16                                  usRsv;
}GPHY_TOOL_AMR_REPORT_IND_STRU;




/*****************************************************************************
 结构名    : GPHY_TOOL_BLK_REPORT_LIST_REQ_STRU
 结构说明  : GPHY抓取非固定地址的数据
*****************************************************************************/
typedef struct
{
    VOS_UINT32                                  ulBlkAddr;                      /* 抓取数据块的首地址 */
    VOS_UINT32                                  ulBlkLen;                       /* 抓取数据块的长度,单位:32bit */
}GPHY_TOOL_BLK_REPORT_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usRptFlag;
    VOS_UINT16                                  usRsv;
    GPHY_TOOL_BLK_REPORT_STRU                   astRptBlk[GPHY_TOOL_RPT_BLK_MAX_NUM];
}GPHY_TOOL_BLK_REPORT_LIST_REQ_STRU;



/*****************************************************************************
 结构名    : GPHY_TOOL_BLK_REPORT_LIST_IND_STRU
 结构说明  : GPHY抓取非固定地址的数据
*****************************************************************************/

typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    GPHY_TOOL_BLK_REPORT_STRU                   astRptBlk[GPHY_TOOL_RPT_BLK_MAX_NUM];
    VOS_UINT32                                  aulData[2];
}GPHY_TOOL_BLK_REPORT_LIST_IND_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_RCV_BURST_INFO_STRU
 结构说明  : 单时隙接收信息的结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                              ulMeaFn;                            /* 上报测量中断时的帧号 */
    VOS_UINT16                              usMeaArfcn;                         /* 上报测量的频点 */
    VOS_UINT16                              usTsMeasType;                       /* 测量类型 */
    VOS_UINT16                              usNextAgcGain;                      /* AGC档位 */
    VOS_INT16                               sAgcRssi;                           /* 测量值:0.125dB */
    VOS_UINT32                              ulDrssi;                            /* DRSSI求对数运算前数值上报 */
    VOS_UINT32                              ulFastDrssi;                        /* 快速测量第一轮以及第二轮RSSI上报 */
    VOS_UINT32                              ulLsDcValue;
    VOS_UINT32                              ulDemoduFn;                         /* 上报测量中断时的帧号 */
    VOS_UINT16                              usDemArfcn;                         /* 上报测量的频点 */
    VOS_UINT16                              usNbPos;
    VOS_UINT32                              ulGrossCarrierEn;
    VOS_UINT32                              ulGrossInterfereEn;
    VOS_UINT32                              ulValppCarrierEn;
    VOS_UINT32                              ulValppInterfereEn;
    VOS_UINT32                              ulDemTypeToa;
}GPHY_TOOL_RCV_BURST_INFO_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_RCV_SINGAL_INFO_REQ_STRU
 结构说明  : 单帧接收信息的结构
*****************************************************************************/
typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT32                                  ulCurFn;
    VOS_UINT16                                  usAfcValue;
    VOS_INT16                                   sAfcTempComp;
    VOS_UINT32                                  ulReserved[2];
    GPHY_TOOL_RCV_BURST_INFO_STRU               astBurstInfo[GPHY_TOOL_TN_NUM_IN_FRAME];
}GPHY_TOOL_RCV_SINGAL_INFO_REQ_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_REPORT_ERROR_IND_STRU
 结构说明  : GPHY信息,告警,error上报结构
*****************************************************************************/

typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usFileNo;                       /* 文件名编码 */
    VOS_UINT16                                  usLineNo;                       /* 出错的行号 */
    VOS_UINT32                                  ulTimeSlice;
    VOS_UINT32                                  ulLogId;                        /* PHY软件内统一编号LOG ID */
    VOS_UINT16                                  usReserved;                     /* 保留字段 */
    VOS_UINT16                                  usVarCnt;                       /* 上报变量个数 */
    VOS_UINT32                                  aulVarValue[GPHY_TOOL_MNTN_ERROR_RPT_VAR_NUM]; /* 上报变量值 */
}GPHY_TOOL_REPORT_ERROR_IND_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_RCV_SINGAL_INFO_REQ_STRU
 结构说明  : 单帧接收信息的结构
*****************************************************************************/
typedef struct
{
    GPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_INT16                                   shwCurrTemp;                    /* 当前温度 */
    VOS_INT16                                   shwXoInitTemp;                  /* 校准温度 */
    VOS_INT32                                   swTempCompValue;                /* 本次计算的温度补偿结果 */
    VOS_UINT32                                  auwMantissa[4];  /* DCXO多项式系数 */
    VOS_UINT16                                  auhwExponent[4]; /* DCXO多项式系数 */
}UPHY_TOOL_DCXO_TEMP_COMP_INFO_IND_STRU;


/*****************************************************************************
  6.2 STRUCT定义,WPHY定义
*****************************************************************************/



/*****************************************************************************
 结构名    : WPHY_TOOL_COMMON_CNF_STRU
 结构说明  : WPHY可维可测公共回复消息结构
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_RSLT_ENUM_UINT16             enResult;                       /* 接收原语的处理结果ID */
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
}WPHY_TOOL_COMMON_CNF_STRU;





/*****************************************************************************
 结构名    : WPHY_TOOL_ACT_CELL_REQ_STRU
 结构说明  : 激活集小区信息
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_REPORT_TYPE_ENUM_UINT16      enReportType;
    VOS_UINT16                                  usReportPeriod;                /* 默认0，标识无效，当上报周期为指定周期时，该IE有效。*/
}WPHY_MNTN_REPORT_TYPE_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_MNTN_REPORT_TYPE_STRU                  stReportType;                   /* 默认每帧上报  */
}WPHY_TOOL_ACT_CELL_REQ_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_ACT_CELL_INFO_IND_STRU
 结构说明  : 激活集小区信息
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;
    VOS_UINT16                                  ausScramble[WPHY_MNTN_MAX_ACT_CELL_NUM]; /* 激活集小区的扰码 */
    VOS_UINT16                                  usServingCellScramble;                   /* 主小区扰码 */
}WPHY_TOOL_ACT_CELL_INFO_IND_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_CELL_SEARCH_LIST_REQ_STRU
 结构说明  : 小区搜索列表信息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_MNTN_REPORT_TYPE_STRU                  stReportType;                   /* 默认事件触发上报，每次小区搜索上报  */
}WPHY_TOOL_CELL_SEARCH_LIST_REQ_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_CELL_SEARCH_LIST_IND_STRU
 结构说明  : 小区搜索列表信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                  usScramble;
    VOS_UINT16                                  usEnergy;
    VOS_UINT32                                  ulPhase;                        /* 需要转换为Chip级别 */
    VOS_UINT16                                  usLifeCycle;                    /* 生命周期 */
    VOS_UINT16                                  usRsv;
}WPHY_MNTN_CS_LIST_CELL_INFO_STRU;

typedef struct
{
    VOS_UINT16                                  usFreq;
    VOS_UINT16                                  usCnt;
    WPHY_MNTN_CS_LIST_CELL_INFO_STRU            astCellInfo[32];

}WPHY_MNTN_CS_LIST_INFO_STRU;


typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;
    VOS_UINT16                                  usFreqCnt;
    VOS_UINT16                                  usRsv;
    WPHY_MNTN_CS_LIST_INFO_STRU                 astCsList[3];
}WPHY_TOOL_CELL_SEARCH_LIST_IND_STRU;





/*****************************************************************************
 结构名    : WPHY_TOOL_PWR_SWITCH_CTRL_REQ_STRU
 结构说明  : 功控功能控制，指定打开或关闭如下的功控功能:
               1.下行功控开关(如果此开关关闭，则标识所有的功控功能全部失效,
                  包括内环功控，外环功控，Winup,Windown等，
                  此时可以再通过功率设置参数来设置上行TPC,SirTarget等。)
               2.外环功控(如果此功能关闭，仅标识外环功能关闭，此时可以通过
                  功率参数设置功能来设置SirTarget。)
               3.Windown
               4.Winup
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_PWR_SWITCH_TYPE_ENUM_UINT16  enPCType;
    WPHY_TOOL_MNTN_SWITCH_CTRL_ENUM_UINT16      enStatus;

}WPHY_TOOL_PWR_SWITCH_CTRL_REQ_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_PWR_PARA_SET_REQ_STRU
 结构说明  : 功率参数设置，以Mask标识有效标记
               1.SirTarget
               2.初始发射功率
               3.Hold上行发射功率(设置最大最小发射功率)
               4.设置指定发送的上行TPC.
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_PWR_PARA_ENUM_UINT32         enPwrParaMask;
    VOS_UINT32                                  ulSirTarget;                    /* SirTarget */
    VOS_INT16                                   sInitTxPwr;                     /* 初始功率 */
    VOS_INT16                                   sUlTpc;                         /* 上行TPC */
    VOS_INT16                                   sMaxTxPwr;                      /* 最大发射功率 */
    VOS_INT16                                   sMinTxPwr;                      /* 最小发射功率 */

}WPHY_TOOL_PWR_PARA_SET_REQ_STRU;





/*****************************************************************************
 结构名    : WPHY_TOOL_MNTN_BLER_REQ_STRU
 结构说明  : 误块率上报
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_MNTN_REPORT_TYPE_STRU                  stReportType;                   /* 默认每帧上报  */

}WPHY_TOOL_BLER_REPORT_REQ_STRU;


/*****************************************************************************
 结构名    : WPHY_MNTN_TRCH_BLER_STRU
 结构说明  : 误块率上报功能
             误块率 = ulErrorBlocks/ulTotalBlocks
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                  usTrchId;
    VOS_UINT16                                  usTti;                          /* 每个传输信道的TTI */
    VOS_UINT32                                  ulErrorBlocks;                  /* 错块数  */
    VOS_UINT32                                  ulTotalBlocks;                  /* 总块数 */

}WPHY_MNTN_TRCH_BLER_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_MNTN_BLER_IND_STRU
 结构说明  : 误块率上报功能
             每次上报的传输信道个数不固定，个数多少根据CNT确定。
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;
    VOS_UINT16                                  usCnt;
    VOS_UINT16                                  usRsv;
    WPHY_MNTN_TRCH_BLER_STRU                    astTrchBler[WPHY_MNTN_TRCH_NUM];

}WPHY_TOOL_BLER_REPORT_IND_STRU;







/*****************************************************************************
 结构名    : WPHY_MNTN_EVENT_REPORT_REQ_STRU
 结构说明  : 事件上报控制
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_EVENT_TYPE_ENUM_UINT32       enEventTypeMask;

}WPHY_TOOL_EVENT_REPORT_REQ_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_SYNC_STATUS_IND_STRU
 结构说明  : W下同步失步状态上报
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;
    WPHY_TOOL_SYNC_STATUS_ENUM_UINT16           enSyncStatus;
    VOS_UINT16                                  usRsv;
}WPHY_TOOL_SYNC_STATUS_IND_STRU;




/*****************************************************************************
 结构名    : WPHY_TOOL_AFC_STATUS_IND_STRU
 结构说明  : AFC锁定
   EVENT_VCTCXO_FREEZE ,EVENT_VCTCXO_UNFREEZE
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;
    WPHY_TOOL_TCXO_STATUS_ENUM_UINT16           enTcxoStatus;
    VOS_UINT16                                  usRsv;
    VOS_UINT32                                  ulAfcStatusValue;

}WPHY_TOOL_AFC_STATUS_IND_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_MNTN_INTERRUPT_EXCEPTIION_IND_STRU
 结构说明  : 中断异常事件上报，
               1.在6ms任务结束时，读取9ms任务已经到达。
               2.在9ms任务结束时，读取SFN判断该任务是否已经跨帧。
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;

    WPHY_TOOL_MNTN_INT_EXCEPT_ENUM_UINT16       enIntExceptType;                /* 中断异常类型 */
    VOS_UINT16                                  usRsv;
}WPHY_TOOL_INTERRUPT_EXCEPTIION_IND_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_WCDMA_L1_STATUS_IND_STRU
 结构说明  : 上报L1状态
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;

    VOS_UINT32                                  ulChannelStatus;                /* 信道状态 */
    WPHY_TOOL_MNTN_SYS_STATUS_ENUM_UINT16       enSysStatus;                    /* 系统状态 */
    VOS_UINT16                                  usRsv;
}WPHY_TOOL_WCDMA_L1_STATUS_IND_STRU;





/*****************************************************************************
 结构名    : WPHY_TOOL_DRX_SWITCH_REQ_STRU
 结构说明  : DRX功能打开和关闭设置
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_SWITCH_CTRL_ENUM_UINT16      enDrxCtrl;
    VOS_UINT16                                  usRsv;

}WPHY_TOOL_DRX_SWITCH_REQ_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_THRESHOLD_SET_REQ_STRU
 结构说明  : 门限设置
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_THRESHOLD_TYPE_ENUM_UINT32   enThresholdType;
    VOS_INT32                                   lValue;

}WPHY_TOOL_THRESHOLD_SET_REQ_STRU;


/*****************************************************************************
 结构名    : WPHY_TOOL_THRESHOLD_QUERY_REQ_STRU
 结构说明  : 查询
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_THRESHOLD_TYPE_ENUM_UINT32   enThresholdType;

}WPHY_TOOL_THRESHOLD_QUERY_REQ_STRU;


/*****************************************************************************
 结构名    : WPHY_TOOL_THRESHOLD_QUERY_CNF_STRU
 结构说明  : 查询
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;
    WPHY_TOOL_MNTN_THRESHOLD_TYPE_ENUM_UINT32   enThresholdType;
    VOS_INT32                                   lValue;

}WPHY_TOOL_THRESHOLD_QUERY_IND_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_ERROR_REPORT_REQ_STRU
 结构说明  : 错误上报
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_ERROR_RPT_LEVEL_ENUM_UINT16  enErrorRptLevel;                /* 控制上报错误级别 */
    VOS_UINT16                                  usRsv;
}WPHY_TOOL_ERROR_REPORT_REQ_STRU;




/*****************************************************************************
 结构名    : WPHY_TOOL_ERROR_REPORT_IND_STRU
 结构说明  : 错误上报
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usFileNo;                       /* 文件名编码 */
    VOS_UINT16                                  usLineNo;                       /* 出错的行号 */
    VOS_UINT32                                  ulTimeSlice;
    VOS_UINT32                                  ulLogId;                        /* PHY软件内统一编号LOG ID */
    VOS_UINT16                                  usReserved;                     /* 保留字段 */
    VOS_UINT16                                  usVarCnt;                       /* 上报变量个数 */
    VOS_UINT32                                  aulVarValue[WPHY_TOOL_MNTN_ERROR_RPT_VAR_NUM]; /* 上报变量值 */

}WPHY_TOOL_ERROR_REPORT_IND_STRU;




/*****************************************************************************
 结构名    : WPHY_TOOL_RF_FRONT_END_PARA_REQ_STRU
 结构说明  : RF前端参数上报
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */

    WPHY_MNTN_REPORT_TYPE_STRU                  stReportType;                   /* 默认每帧上报  */

}WPHY_TOOL_RF_FRONT_END_PARA_REQ_STRU;


/*****************************************************************************
 结构名    : WPHY_TOOL_RF_FRONT_END_PARA_IND_STRU
 结构说明  : 上报TX Temp Comp,Freq Comp,AFC,LNA,HDET
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;
    VOS_INT16                                   sPaHighTempComp;                /* PA高增益时温度补偿 */
    VOS_INT16                                   sPaMidTempComp;                 /* PA中增益时温度补偿 */
    VOS_INT16                                   sPaLowTempComp;                 /* PA低增益时温度补偿 */

    VOS_INT16                                   sPaHighFreqComp;                /* PA高增益时频率补偿 */
    VOS_INT16                                   sPaMidFreqComp;                 /* PA中增益时频率补偿 */
    VOS_INT16                                   sPaLowFreqComp;                 /* PA低增益时频率补偿 */

    VOS_UINT16                                  usAfcStatus;                    /* AFC值 */
    VOS_UINT16                                  usAnt1AgcStatus;                /* 天线1的AGC增益状态 */
    VOS_UINT16                                  usAnt2AgcStatus;                /* 天线2的AGC增益状态 */
    VOS_UINT16                                  usHdet;                         /* HDET值 */
    VOS_UINT16                                  usPaStatus;                     /* PA状态:上报的是PA的模式控制字 */
    VOS_UINT16                                  usDbbAtten;                     /* DBB衰减 */
    VOS_UINT16                                  usRfAtten;                      /* RF衰减 */
    VOS_UINT16                                  usReserved;

}WPHY_TOOL_RF_FRONT_END_PARA_IND_STRU;


/*****************************************************************************
 结构名    : WPHY_TOOL_PI_INT_RESULT_IND_STRU
 结构说明  : WPHY的PI中断结果上报
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_INT16                                   sPiResult;
    VOS_UINT16                                  usRsv;
}WPHY_TOOL_PI_INT_RESULT_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_AI_INT_RESULT_IND_STRU
 结构说明  : WPHY的AI中断结果上报
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_INT16                                   sAiResult;                      /* AI结果,+1；-1；0 */
    VOS_UINT16                                  usReTranNum;                    /* 重传次数 */
}WPHY_TOOL_AI_INT_RESULT_IND_STRU;

/*****************************************************************************
 结构名    : T313 Timer Status
 结构说明  : T313定时器状态上报
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_T313_STATUS_ENUM_UINT16      enT313Status;                   /* T313状态*/
    VOS_UINT16                                  usRsv;
}WPHY_TOOL_T313_STATUS_IND_STRU;

/*****************************************************************************
 结构名    : T312 Timer Status
 结构说明  : T312定时器状态上报
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_T312_STATUS_ENUM_UINT16      enT312Status;                   /* T313状态*/
    VOS_UINT16                                  usRsv;
}WPHY_TOOL_T312_STATUS_IND_STRU;

/*****************************************************************************
 结构名    : 从模CDRX配置启动
 结构说明  :
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
}WPHY_TOOL_SLAVE_CDRX_START_IND_STRU;

/*****************************************************************************
 结构名    : 从模CDRX配置停止
 结构说明  :
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
}WPHY_TOOL_SLAVE_CDRX_STOP_IND_STRU;


/*****************************************************************************
 结构名    : WPHY_TOOL_PA_STATUS_REPORT_REQ_STRU
 结构说明  : PA状态上报请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */

    WPHY_MNTN_REPORT_TYPE_STRU                  stReportType;                   /* 默认每帧上报  */

}WPHY_TOOL_PA_STATUS_REPORT_REQ_STRU;


/*****************************************************************************
 结构名    : WPHY_TOOL_MASTER_WAKE_LTE_IND_STRU
 结构说明  : 唤醒LTE的指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;                          /* 唤醒时刻的sfn */
    VOS_UINT16                                  usSlot;                         /* 唤醒时刻的slot */
    VOS_UINT32                                  ulWakeReason;                   /* 唤醒目的 */
}WPHY_TOOL_MASTER_WAKE_LTE_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_MASTER_START_LTE_MEAS_IND_STRU
 结构说明  : 启动LTE测量的指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;                          /* 启动时刻的sfn */
    VOS_UINT16                                  usSlot;                         /* 启动时刻的slot */
    VOS_UINT32                                  ulStartReason;                  /* 启动原因 */
    VOS_UINT32                                  ulMeasTime;                     /* 测量时间 us */
    VOS_UINT32                                  ulInt0Int1Time;                 /* int0 int1间隔 us */
    VOS_UINT32                                  ulAfcLockFlag;
    VOS_UINT32                                  ulAfcValue;
}WPHY_TOOL_MASTER_START_LTE_MEAS_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_MASTER_REPORT_LTE_MEAS_IND_STRU
 结构说明  : 启动LTE测量的指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;                          /* 上报时刻的sfn */
    VOS_UINT16                                  usSlot;                         /* 上报时刻的slot */
}WPHY_TOOL_MASTER_REPORT_LTE_MEAS_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_MASTER_STOP_LTE_MEAS_IND_STRU
 结构说明  : 强停LTE测量的指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16                   enMsgId;                /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                          usToolsId;              /* 保留字段 */
    WPHY_TOOL_MNTN_MASTER_STOP_LTE_REASION_ENUM_UINT16  enReason;               /* 强停原因 */
    VOS_UINT16                                          usSfn;                  /* 强停时刻的sfn */
    VOS_UINT16                                          usSlot;                 /* 强停时刻的slot */
    VOS_UINT16                                          usRsv;
}WPHY_TOOL_MASTER_STOP_LTE_MEAS_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_SET_LTE_MEAS_FLAG_IND_STRU
 结构说明  : 设置LTE测量标记的指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usValue;
    VOS_UINT16                                  usRsv;
}WPHY_TOOL_SET_LTE_MEAS_FLAG_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_QPC_OPEN_UL_IND_STRU
 结构说明  : QPC打开上行链路的指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;                          /* 唤醒时刻的sfn */
    VOS_UINT16                                  usSlot;                         /* 唤醒时刻的slot */
    VOS_UINT32                                  ulOpenReason;                   /* 打开原因 */
}WPHY_TOOL_QPC_OPEN_UL_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_QPC_CLOSE_UL_IND_STRU
 结构说明  : QPC关闭上行链路的指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;                          /* 唤醒时刻的sfn */
    VOS_UINT16                                  usSlot;                         /* 唤醒时刻的slot */
    VOS_UINT32                                  ulCloseReason;                  /* 关闭原因 */
}WPHY_TOOL_QPC_CLOSE_UL_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_QPC_RRC_STATE_IND_STRU
 结构说明  : QPC当前RRC状态的指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;                          /* 唤醒时刻的sfn */
    VOS_UINT16                                  usSlot;                         /* 唤醒时刻的slot */
    VOS_UINT32                                  ulRrcState;                     /* RRC状态 */
}WPHY_TOOL_QPC_RRC_STATE_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_QPC_RB_TYPE_IND_STRU
 结构说明  : QPC当前业务类型的指示 CS/PS
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;                          /* 唤醒时刻的sfn */
    VOS_UINT16                                  usSlot;                         /* 唤醒时刻的slot */
    VOS_UINT32                                  ulRbType;                       /* RB TYPE */
}WPHY_TOOL_QPC_RB_TYPE_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_DPDT_ESTIMATE_STATE_IND_STRU
 结构说明  : DPDT评估阶段可维可测上报
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usNextState;                    /* 下一双天线状态 */
    VOS_INT16                                   sRssi;                          /* 主天线的RSSI,滤波后的结果,单位为0.125dbm */
    VOS_INT16                                   sRscp;                          /* 主天线的RSCP,滤波后的结果,单位为0.125dbm */
    VOS_UINT16                                  usAntStatus;                    /* 当前天线状态,0为主天线1为分集 */
}WPHY_TOOL_DPDT_ESTIMATE_STATE_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_DPDT_COMPARE_STATE_IND_STRU
 结构说明  : DPDT比较阶段可维可测上报
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usNextState;                    /* 下一双天线状态 */
    VOS_UINT16                                  usPhase;                        /* 比较阶段下一阶段 */
    VOS_INT16                                   sRscp;                          /* 主分集天线滤波后的RSCP差值,单位为0.125dbm */
    VOS_UINT16                                  usAntStatus;                    /* 当前天线状态,0为主天线1为分集 */
}WPHY_TOOL_DPDT_COMPARE_STATE_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_DPDT_PROTECT_STATE_IND_STRU
 结构说明  : DPDT迟滞阶段可维可测上报
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usNextState;                    /* 下一双天线状态 */
    VOS_UINT16                                  usAntStatus;                    /* 当前天线状态,0为主天线1为分集 */
}WPHY_TOOL_DPDT_PROTECT_STATE_IND_STRU;


/*****************************************************************************
 结构名    : WPHY_TOOL_SLAVE_W_FINISH_IND_STRU
 结构说明  : 从模W任务完成指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT32                                  ulRsv;
}WPHY_TOOL_SLAVE_W_FINISH_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_SLAVE_GRSSI_FINISH_IND_STRU
 结构说明  : 从模W任务完成指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT32                                  ulRsv;
}WPHY_TOOL_SLAVE_GRSSI_FINISH_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_SLAVE_GBSIC_FINISH_IND_STRU
 结构说明  : 从模G BSIC任务完成指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT32                                  ulRsv;
}WPHY_TOOL_SLAVE_GBSIC_FINISH_IND_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_SLAVE_SCHEDULE_TIMEOUT_IND_STRU
 结构说明  : 从模调度超时指示
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usStartChip;
    VOS_UINT16                                  usStartSlot;
    VOS_UINT16                                  usEndChip;
    VOS_UINT16                                  usEndSlot;
    VOS_UINT16                                  usScheduleTime;                 /* 调度时间，单位us */
    VOS_UINT16                                  usRsv;
}WPHY_TOOL_SLAVE_SCHEDULE_TIMEOUT_IND_STRU;
/*****************************************************************************
 结构名    : WPHY_TOOL_PA_STATUS_REPORT_IND_STRU
 结构说明  : PA状态上报
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;
    WPHY_TOOL_MNTN_PA_STATUS_ENUM_UINT16        enPaStatus;
    VOS_UINT16                                  usRsv;
}WPHY_TOOL_PA_STATUS_REPORT_IND_STRU;





/*****************************************************************************
 结构名    : WPHY_TOOL_INTRA_MSG_RPT_CTRL_REQ_STRU
 结构说明  : PHY 内部消息上报控制请求
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usMsgRptFlag;                   /* 指示内部消息是否上报 */
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
}WPHY_TOOL_INTRA_MSG_RPT_CTRL_REQ_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_RF_FRONT_END_STATIC_REQUIRE_REQ_STRU
 结构说明  : 静态查询前端状态请求原语
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
}WPHY_TOOL_RF_FRONT_END_STATIC_REQUIRE_REQ_STRU;




/*****************************************************************************
 结构名    : WPHY_TOOL_RF_FRONT_END_STATIC_REQUIRE_IND_STRU
 结构说明  : 静态查询前端状态回复原语
*****************************************************************************/
typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usRfTrcvOnState;                /* RF总开关，1为打开，0为关闭 */
    VOS_UINT16                                  usRfRxOnOrOff;                  /* 04h[1:0] = 3为开 */
    VOS_UINT16                                  usRxPathselect;                 /* 04h[4:3] = 3为主分集都开，2开主集 */
    VOS_UINT16                                  usWagcSwitch;                   /* WBBP 主分集开关 */

    VOS_UINT16                                  usAbbRxConfig;                  /* 3为主分集都开，1开主集 */
    VOS_UINT16                                  usAbbLineCtrlMode;              /* CH1:[15:8],CH0:[7:0] = 0:IDLE 1:G模 2:W模 3:LTE模 4:TDS模 5:ET模式 6:APT模式 7:CA模式 */
    VOS_UINT16                                  usAbbRxMode;                    /* CH1:[15:8],CH0:[7:0] = 0:2G 1:3G_SC 2:4G 3: 3G_DC 4:TDS */
    VOS_UINT16                                  usAbbVoiceData;                 /* CH1:[15:8],CH0:[7:0] = 1:voice mode 2: data mode */

    VOS_UINT16                                  usRfTxOnOrOff;                  /* 02h[1:0] = 3为开 */
    VOS_UINT16                                  usRfTxSHDN;                     /* RF上行打开控制，0由BBP控制 */
    VOS_UINT16                                  usTxPathselect;                 /* RFIC TX Patch */
    VOS_UINT16                                  usRfTxAgc;                      /* RFIC TXAGC */

    VOS_UINT16                                  usCpcDrxFuncOn;                 /* 当前工作在CPC DRX的标志 */
    VOS_UINT16                                  usCpcDrxStartCfn;               /* CPC DRX开始工作的CFN */
    VOS_UINT16                                  usCpcSleepBitMap;               /* 低16bit，1为醒来时隙，0为睡眠时隙 */

    VOS_UINT16                                  usGpioFuncOn;                   /* 硬件支持GPIO */
    VOS_UINT16                                  usGpioPaMode;                   /* GPIO PA mode */

    VOS_UINT16                                  usMipiFuncOn;                   /* 硬件支持MIPI */
    VOS_UINT16                                  usMipiPaMode;                   /* MIPI PA mode */

    VOS_UINT16                                  usAptFuncOn;                    /* 硬件支持APT */
    VOS_UINT16                                  usAptPaMode;                    /* APT电压mode */
    VOS_UINT16                                  uhwAptVcc;                          /* 保留 */
}WPHY_TOOL_RF_FRONT_END_STATIC_REQUIRE_IND_STRU;




/*****************************************************************************
 结构名    : WPHY_TOOL_RF_FRONT_END_DISPLAY_REQ_STRU
 结构说明  : 是否实时输出前端信息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_SWITCH_CTRL_ENUM_UINT16      enDisplayCtrl;
    VOS_UINT16                                  usReserved;
}WPHY_TOOL_RF_FRONT_END_DISPLAY_REQ_STRU;


/*****************************************************************************
 结构名    : WPHY_TOOL_BBP_RTC_REQ_STRU
 结构说明  : RTC功能打开和关闭设置
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    UPHY_TOOL_RTC_DEBUG_ENUM_UINT16             enRtcCtrl;                      /* RTC打开或者关闭 */
    VOS_UINT16                                  usRsv;

}WPHY_TOOL_BBP_RTC_REQ_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_RAM_REPORT_REQ_STRU
 结构说明  : WPHY抓取固定块画图的RAM
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_RAM_RPT_ENUM_UINT16          enRptRam;
    WPHY_TOOL_MNTN_RPT_STATUS_ENUM_UINT16       enRptStatus;
}WPHY_TOOL_RAM_REPORT_REQ_STRU;





/*****************************************************************************
 结构名    : WPHY_TOOL_RAM_REPORT_IND_STRU
 结构说明  : WPHY抓取固定块画图的RAM
*****************************************************************************/

typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;
    WPHY_TOOL_MNTN_RAM_RPT_ENUM_UINT16          enRptRam;
    VOS_UINT16                                  usRsv;
    VOS_UINT32                                  aulData[2];
}WPHY_TOOL_RAM_REPORT_IND_STRU;



/*****************************************************************************
 结构名    : WPHY_TOOL_BLK_REPORT_LIST_REQ_STRU
 结构说明  : WPHY抓取非固定地址的数据
*****************************************************************************/
typedef struct
{
    VOS_UINT32                                  ulBlkAddr;                      /* 抓取数据块的首地址 */
    VOS_UINT32                                  ulBlkLen;                       /* 抓取数据块的长度,单位:32bit */
}WPHY_TOOL_BLK_REPORT_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_RPT_STATUS_ENUM_UINT16       enRptFlag;
    VOS_UINT16                                  usRsv;
    WPHY_TOOL_BLK_REPORT_STRU                   astRptBlk[WPHY_TOOL_RPT_BLK_MAX_NUM];
}WPHY_TOOL_BLK_REPORT_LIST_REQ_STRU;




/*****************************************************************************
 结构名    : WPHY_TOOL_BLK_REPORT_LIST_IND_STRU
 结构说明  : WPHY抓取非固定地址的数据
*****************************************************************************/

typedef struct
{
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    VOS_UINT16                                  usSfn;
    VOS_UINT16                                  usCfn;
    WPHY_TOOL_BLK_REPORT_STRU                   astRptBlk[WPHY_TOOL_RPT_BLK_MAX_NUM];
    VOS_UINT32                                  aulData[2];
}WPHY_TOOL_BLK_REPORT_LIST_IND_STRU;


/*****************************************************************************
 结构名    : WPHY_TOOL_RA_MNTN_CTRL_REQ_STRU
 结构说明  : 随机接入可维可测上报设置消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_SWITCH_CTRL_ENUM_UINT16      enRaMntnCtrl;                   /* 随机接入可维可测打开或关闭 */
    VOS_UINT16                                  usReserve;                      /* 保留字段 */
}WPHY_TOOL_RA_MNTN_CTRL_REQ_STRU;

/*****************************************************************************
 结构名    : WPHY_TOOL_ARM_0MS_MNTN_CTRL_REQ_STRU
 结构说明  : ARM 0MS可维可测上报设置消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip */
    WPHY_TOOL_MNTN_MSG_ID_ENUM_UINT16           enMsgId;                        /*_H2ASN_Skip */ /* 原语类型 */
    VOS_UINT16                                  usToolsId;                      /* 保留字段 */
    WPHY_TOOL_MNTN_SWITCH_CTRL_ENUM_UINT16      enArm0msMntnCtrl;               /* ARM 0MS打开或关闭 */
    VOS_UINT16                                  usReserve;                      /* 保留字段 */
}WPHY_TOOL_ARM_0MS_MNTN_CTRL_REQ_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_GSM_CELL_SNR_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 该命令用于请求上报服务小区和邻区的信噪比,测量值等信息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    VOS_UINT16                              uhwMsgId;                           /* _H2ASN_Skip */    /* 原语ID */
    VOS_UINT16                              uhwToolId;                              /* 保留字段 */
    VOS_UINT16                              uhwCommand;                             /* 命令。0:停止，1：启动 */
    VOS_UINT16                              uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_GSM_CELL_SNR_REQ_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_GSM_CELL_SNR_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 每个小区的信噪比,测量值等信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwFreqInfo;                            /* 频点信息上报，高12bit表示频段，0:850,1:900,2:1800,3:1900，低12bit表示频点号，0-1023。*/
    VOS_INT16                           shwRxLev;                               /* 信号强度值，[-110,-15],单位为dbm */
    VOS_UINT16                          uhwCellSnr;                             /* 信噪比，[0,99],单位为db */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_GSM_CELL_SNR_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_GSM_CELL_SNR_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 用于上报服务小区和邻区的信噪比,测量值等信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwCellNum;                             /* 频点数 */
    GPHY_TOOL_GSM_CELL_SNR_STRU         astCellInfo[GPHY_TOOL_MA_FREQ_NUM];       /* 小区信息 */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_GSM_CELL_SNR_IND_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_BA_LIST_MEAS_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 请求上报BA LIST的信号强度，bsic信息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwCommand;                             /* 命令。0:停止，1：启动 */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_BA_LIST_MEAS_REQ_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_CELL_MEAS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 每个小区的信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwFreqInfo;                            /* 频点信息上报，高12bit表示频段，0:850,1:900,2:1800,3:1900，低12bit表示频点号，0-1023。*/
    VOS_INT16                           shwRxLev;                               /* 信号强度值，[-110,-15],单位为dbm */
    VOS_UINT16                          uhwBsic;                                /* 基站标识码，0-77，无效0xffff */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_CELL_MEAS_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_BA_LIST_MEAS_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 上报BA LIST的信号强度，bsic信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwScellFreqInfo;                       /* 频点信息上报，高12bit表示频段，0:850,1:900,2:1800,3:1900，低12bit表示频点号，0-1023。*/
    VOS_INT16                           shwScellRxLev;                          /* 信号强度值，[-110,-15],单位为dbm */
    VOS_UINT16                          uhwHoppingInd;                          /* 跳频指示,1:表示跳频,0表示不跳频 */
    VOS_UINT16                          uhwResvered1;                           /* 保留字段 */
    VOS_UINT16                          uhwCellNum;                             /* 邻区的个数 */
    VOS_UINT16                          uhwResvered2;                           /* 保留字段 */
    GPHY_TOOL_CELL_MEAS_STRU            astNeighbourCell[GPHY_TOOL_BA_LIST_MAX_NUM]; /* 邻区的测量信息 */
}GPHY_TOOL_BA_LIST_MEAS_IND_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_AMR_SETTING_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 请求上报AMR配置信息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwCommand;                             /* 命令。0:停止，1：启动 */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_AMR_SETTING_REQ_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_AMR_SETTING_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 上报AMR配置信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwAcsNum;                              /* 激活集个数,范围为0-4 */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
    VOS_UINT16                          auhwAcs[GPHY_TOOL_AMR_MAX_ACS_NUM];       /* 激活集集合，范围0-7 */
    VOS_UINT16                          auhwHyst[GPHY_TOOL_AMR_MAX_ACS_NUM-1];    /* 切换迟滞大小，范围0-15，单位为0.5db */
    VOS_UINT16                          auhwThd[GPHY_TOOL_AMR_MAX_ACS_NUM-1];     /* 切换门限大小，范围为0-63，单位为0.5db */
}GPHY_TOOL_AMR_SETTING_IND_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_AMR_CODEC_USAGE_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 请求上报激活集中每个语音速率的使用率
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwCommand;                             /* 命令。0:停止，1：启动 */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_AMR_CODEC_USAGE_REQ_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_AMR_CODEC_USAGE_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 上报激活集中每个语音速率的使用率
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwAcsNum;                              /* 激活集个数,范围为0-4 */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
    VOS_UINT16                          auhwAcs[GPHY_TOOL_AMR_MAX_ACS_NUM];       /* 激活集集合，范围0-7 */
    VOS_UINT32                          auwUplink[GPHY_TOOL_AMR_MAX_ACS_NUM];     /* 上行链路激活集的每个速率的块数，范围0-2^32-1，单位为块 */
    VOS_UINT32                          auwDownlink[GPHY_TOOL_AMR_MAX_ACS_NUM];   /* 下行链路激活集的每个速率的块数，范围0-2^32-1，单位为块 */
}GPHY_TOOL_AMR_CODEC_USAGE_IND_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_AMR_CODEC_STAT_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 请求各个编码速率的使用率
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwCommand;                             /* 命令。0:停止，1：启动 */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_AMR_CODEC_STAT_REQ_STRU;

/*****************************************************************************
 结构名    : GPHY_AMR_CODEC_AVG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 每个编码速率的使用率
*****************************************************************************/

typedef struct
{
    VOS_UINT32                          auhwUplink[GPHY_TOOL_AMR_CODEC_MAX_NUM];  /* 上行链路激活集的每个速率的块数，范围0-2^32-1，单位为块 */
    VOS_UINT32                          auhwDownlink[GPHY_TOOL_AMR_CODEC_MAX_NUM];/* 下行链路激活集的每个速率的块数，范围0-2^32-1，单位为块 */
}GPHY_AMR_CODEC_AVG_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_AMR_CODEC_STAT_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 小区，呼叫的统计信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    GPHY_AMR_CODEC_AVG_STRU             stCell;                                 /* 小区的统计信息 */
    GPHY_AMR_CODEC_AVG_STRU             stCall;                                 /* 每个呼叫的统计信息 */
}GPHY_TOOL_AMR_CODEC_STAT_IND_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_CS_PS_TX_INFO_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 请求上报CS/PS域上行信息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwCommand;                             /* 命令。0:停止，1：启动 */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_CS_PS_TX_INFO_REQ_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_CS_PS_TX_INFO_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 上报CS/PS域上行信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwTa;                                  /* 时间提前量，范围为0-63，单位为符号 */
    VOS_UINT16                          uhwSlotNumber;                          /* 时隙个数 */
    VOS_UINT16                          uhwTxPower[4];                          /* 上行发射功率，范围为0-99，单位为dbm */
    VOS_UINT16                          uhwPcl;                                 /* 基站指定的功率等级，范围为0-31 */
    VOS_UINT16                          uhwResvered;
}GPHY_TOOL_CS_PS_TX_INFO_IND_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_CS_FER_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 请求上报CS FER信息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwCommand;                             /* 命令。0:停止，1：启动 */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_CS_FER_REQ_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_CS_FER_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 上报CS FER信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT32                          uwErrorFrameFull;                       /* full块的错误帧数 */
    VOS_UINT32                          uwTotalFrameFull;                       /* full总帧数 */
    VOS_UINT32                          uwErrorFrameSub;                        /* sub块的错误帧数 */
    VOS_UINT32                          uwTotalFrameSub;                        /* sub总帧数 */
}GPHY_TOOL_CS_FER_IND_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_CS_GET_CURRENT_DSC_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 请求上报下行链路计数值
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwCommand;                             /* 命令。0:停止，1：启动 */
    VOS_UINT16                          uhwResvered;                            /* 保留字段 */
}GPHY_TOOL_GET_CURRENT_DSC_REQ_STRU;

/*****************************************************************************
 结构名    : GPHY_TOOL_GET_CURRENT_DSC_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 上报下行链路计数值
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwMsgId;                               /* _H2ASN_Skip *//* 原语ID */
    VOS_UINT16                          uhwToolId;                              /* 保留字段 */
    VOS_UINT16                          uhwDsc;                                 /* 下行链路计数值 */
    VOS_UINT16                          uhwResvered;
}GPHY_TOOL_GET_CURRENT_DSC_IND_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_DUAL_ANT_ESTIMATE_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 主天线能量评估阶段判决结果上报
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwMsgID;                               /* _H2ASN_Skip */ /* 原语ID */
    VOS_UINT16                          uhwDualAntCurId;                        /* 天线id */
    VOS_UINT16                          uhwDualAntNextState;                    /* 下一个跳转阶段 */
    VOS_UINT16                          uhwRxlevAvgSub;                         /* 主天线平均每个统计周期SUB块的能量值 */
}GPHY_TOOL_DUAL_ANT_ESTIMATE_IND_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_DUAL_ANT_COMPARE_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 主辅天线能量比较阶段判决结果上报
*****************************************************************************/
typedef struct
{
    VOS_UINT16                      uhwMsgID;                                   /* _H2ASN_Skip */ /* 原语ID */
    VOS_UINT16                      uhwDualAntCurId;                            /* 切换前的天线id */
    VOS_UINT16                      uhwDualAntNextId;                           /* 切换后的天线id */
    VOS_UINT16                      uhwDualAntNextState;                        /* 下一个跳转阶段 */
    VOS_UINT16                      uhwAntCmpNextSubState;                      /* 下一个跳转子状态 */
    VOS_INT16                       shwRxlevFilterPriAnt;                       /* 主天线平均每个统计周期的能量滤波值 */
    VOS_INT16                       shwRxlevFilterSecAnt;                       /* 辅天线平均每个统计周期的能量滤波值 */
    VOS_INT16                       shwDeltaRxlev;                              /* 辅天线与主天线平均每个统计周期的能量滤波差值 */
}GPHY_TOOL_DUAL_ANT_COMPARE_IND_STRU;


/*****************************************************************************
 结构名    : GPHY_TOOL_DUAL_ANT_DELAY_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 天线切换停滞阶段判决结果上报
*****************************************************************************/
typedef struct
{
    VOS_UINT16                      uhwMsgID;                                   /* _H2ASN_Skip */ /* 原语ID */
    VOS_UINT16                      uhwDualAntCurId;                            /* 天线id */
    VOS_UINT16                      uhwDualAntNextState;                        /* 下一个跳转阶段 */
    VOS_UINT16                      uhwHandAntEn;                               /* 停滞阶段启动或结束标志 */
}GPHY_TOOL_DUAL_ANT_DELAY_IND_STRU;


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

#endif /* end of phytoolinterface.h */
