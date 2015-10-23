/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : LrrcErrLogInterface.h
  Description     : LrrcErrLogInterface.h header file
  History         :
     1.HANLUFENG 41410       2013-8-27     Draft Enact
     2.
******************************************************************************/

#ifndef __LRRCERRLOGINTERFACE_H__
#define __LRRCERRLOGINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "vos.h"


#include "omerrorlog.h"
#include "AppRrcInterface.h"


#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif


/*****************************************************************************
  2 macro
*****************************************************************************/

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/
/* LTE支持CA情况下，支持的最大载频数，V9R1为1，V7R2和K3V3为2，协议规定最大值为5 */
#define LRRC_APP_MAX_NUM 5

/* RRC error发生时，RRC的建链信息记录最大个数，包括建链原因和结果 */
#define RRC_APP_MAX_RRC_EST_INFO_NUM              (5)

/* 协议值:3 */
#define RRC_APP_MAX_NUM_OF_MCC                    (3)

/* RRC error发生时，打印打点信息的最大记录个数 */
#define RRC_APP_ERROR_PRINT_INFO_NUM              (4)

/* RRC error发生时，模块接收最新信息的最大记录个数 */
#define RRC_APP_RECEIVE_MSG_INFO_NUM              (16)

/* RRC error发生时，最新RRC错误码的最大记录个数 */
#define RRC_APP_CODE_NUM                          (4)

/* LTE支持MIMO时，最大同时支持的码字数 */
#define NUM_OF_CODEWORD                           (2)

/*****************************************************************************
  4 Enum
*****************************************************************************/
 enum RRC_APP_FTM_TYPE_ENUM
 {
    RRC_APP_FTM_PERIOD_INFO,     /* 周期型上报的工程模式信息，目前Idle状态上报周期为DRX周期，Connected状态下周期为1秒 */
    RRC_APP_FTM_CHANGE_INFO,     /* 更新后上报型的工程模式信息 */
    RRC_APP_FTM_INFO_BUTT
 };
 typedef VOS_UINT32 RRC_APP_FTM_TYPE_ENUM_UINT32;

/*****************************************************************************
 枚举名    : RRC_APP_PAGING_CYCLE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 寻呼周期，RFXX，XX表示无线帧个数，单位为10ms,RF32周期是320ms，界面显示为XXXms
*****************************************************************************/
enum RRC_APP_PAGING_CYCLE_ENUM
{
    RRC_APP_RF32                                     = 0,
    RRC_APP_RF64                                     = 1,
    RRC_APP_RF128                                    = 2,
    RRC_APP_RF256                                    = 3
};
typedef VOS_UINT32 RRC_APP_PAGING_CYCLE_ENUM_UINT32;
/*****************************************************************************
 枚举名    : RRC_APP_TRANSM_MODE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 传输模式类型，界面直接显示TMX_TRANSM_MODE即可
*****************************************************************************/
enum RRC_APP_TRANSM_MODE_ENUM
{
    RRC_APP_TM1_TRANSM_MODE                          = 0,
    RRC_APP_TM2_TRANSM_MODE                          = 1,
    RRC_APP_TM3_TRANSM_MODE                          = 2,
    RRC_APP_TM4_TRANSM_MODE                          = 3,
    RRC_APP_TM5_TRANSM_MODE                          = 4,
    RRC_APP_TM6_TRANSM_MODE                          = 5,
    RRC_APP_TM7_TRANSM_MODE                          = 6,
    RRC_APP_TM8_TRANSM_MODE                          = 7,
    RRC_APP_TM9_TRANSM_MODE                          = 8,
    RRC_APP_TRANSM_MODE_BUTT
};
typedef VOS_UINT32 RRC_APP_TRANSM_MODE_ENUM_UINT32;
/*****************************************************************************
 枚举名    : RRC_APP_PROTOCOL_STATE_IND
 协议表格  :
 ASN.1描述 :
 枚举说明  : RRC协议状态类型
*****************************************************************************/
enum RRC_APP_PROTOCOL_STATE_IND_ENUM
{
    RRC_APP_PROTOCOL_IDLE                   = 0 ,
    RRC_APP_PROTOCOL_CONNECTED,
    RRC_APP_PROTOCOL_STATE_BUTT
};
typedef VOS_UINT8 RRC_APP_PROTOCOL_STATE_IND_ENUM_UINT8;

/*****************************************************************************
 枚举名    : RRC_APP_ERROR_CODE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : RRC错误码类型
*****************************************************************************/
enum RRC_APP_ERROR_CODE_ENUM
{
    RRC_APP_RB_MEAS_CHECK_ERR               = 0, /* 重配置失败--measConfig检查失败 */
    RRC_APP_RB_MOCTRL_CHECK_ERR             = 1, /* 重配置失败--MobilCtrlInfo 切换消息检查失败 */
    RRC_APP_RB_RADIOSRC_CHECK_ERR           = 2, /* 重配置失败--RadioRsrcConfigDedic 消息检查失败 */
    RRC_APP_RB_TTIBUNDING_CHECK_ERR         = 3, /* 重配置失败--TTIBundling 检查失败 */
    RRC_APP_RB_SECU_CHECK_ERR               = 4, /* 重配置失败--SecurCfgHo 切换中安全检查失败 */
    RRC_APP_RB_CFGDSP_ERR                   = 5, /* 重配置失败--配置DSP，DSP返回失败 */
    RRC_APP_RB_CFGL2_ERR                    = 6, /* 重配置失败--配置L2,L2返回失败 */
    RRC_APP_RB_CFGDSP_TIMEROUT_ERR          = 7, /* 重配置失败--配置DSP，保护定时器超时 */
    RRC_APP_RB_CFGL2_TIMEROUT_ERR           = 8, /* 重配置失败--配置L2，保护定时器超时 */

    RRC_APP_EST_MT_T302_ERR                     = 18, /* RRC连接建立失败--MT Call；T302定时器在运行 */

    RRC_APP_EST_MO_T302_ERR                     = 22, /* RRC连接建立失败--Mo Data；T302定时器在运行 */
    RRC_APP_EST_MO_T303_ERR                     = 23, /* RRC连接建立失败--Mo Data；T303定时器在运行 */
    RRC_APP_EST_MO_ACC_PROBABILITY_ERR          = 24, /* RRC连接建立失败--Mo Data；ACC接入等级问题 */
    RRC_APP_EST_MO_RHPLMN_PROBABILITY_ERR       = 25, /* RRC连接建立失败--Mo Data；Rplmn、Hplmn问题 */
    RRC_APP_EST_MO_ACC_RHPLMN_PROBABILITY_ERR   = 26, /* RRC连接建立失败--Mo Data；AcBarringForMoData存在判断被Bar */

    RRC_APP_EST_MS_T302_ERR                     = 28, /* RRC连接建立失败--Mo Signalling；T302定时器在运行 */
    RRC_APP_EST_MS_T305_ERR                     = 29, /* RRC连接建立失败--Mo Signalling；T305定时器在运行 */
    RRC_APP_EST_MS_ACC_PROBABILITY_ERR          = 30, /* RRC连接建立失败--Mo Signalling；ACC接入等级问题 */
    RRC_APP_EST_MS_RHPLMN_PROBABILITY_ERR       = 31, /* RRC连接建立失败--Mo Signalling；Rplmn、Hplmn问题 */
    RRC_APP_EST_MS_ACC_RHPLMN_PROBABILITY_ERR   = 32, /* Mo Signalling；AcBarringForMoData存在判断被Bar */

    RRC_APP_EST_AREA_LOST_ERR                   = 34, /* 当前已经出服务区直接给NAS回失败 */

    RRC_APP_REEST_T311_TIMEROUT_ERR             = 49, /* 重建立失败--重建过程中小区搜索T311定时器超时 */
    RRC_APP_REEST_T301_TIMEROUT_ERR             = 50, /* 重建立失败--发送建立Req消息后，等待重建响应消息，T301超时 */
    RRC_APP_REEST_SRB0CFGDSP_ERR                = 51, /* 重建立失败--配置DSP，DSP返回失败 */
    RRC_APP_REEST_SRB0CFGL2_ERR                 = 52, /* 重建立失败--配置L2，L2返回失败 */
    RRC_APP_REEST_MACCFG_CHECK_ERR              = 53, /* 重建立失败--消息检查MasCfg失败 */
    RRC_APP_REEST_PHYDEDIC_CHECK_ERR            = 54, /* 重建立失败--消息检查PhyCfgDedic失败 */
    RRC_APP_REEST_TTIBUND_CHECK_ERR             = 55, /* 重建立失败--ttiBundling检查失败 */
    RRC_APP_REEST_SRB_CHECK_ERR                 = 56, /* 重建立失败--SRB检查失败超时 */
    RRC_APP_REEST_SPS_CHECK_ERR                 = 57, /* 重建立失败--SPS检查失败 */
    RRC_APP_REEST_SRB1CFGDSP_ERR                = 58, /* 重建立失败--配置DSP，DSP返回失败 */
    RRC_APP_REEST_SRB1CFGL2_ERR                 = 59, /* 重建立失败--配置L2，L2返回失败 */
    RRC_APP_REEST_SECU_ERR                      = 60, /* 重建立失败--安全上下文失败 */

    RRC_APP_ERR_CODE_BUTT
};
typedef VOS_UINT16 RRC_APP_ERROR_CODE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : LRRC_LNAS_EST_CAUSE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : RRMM_EST_REQ_STRU消息中的enEstCause
              emergency, highPriorityAccess, mt-Access, mo-Signalling, mo-Data,delayTolerantAccess-v1020 ,
*****************************************************************************/
enum LRRC_APP_LNAS_EST_CAUSE_ENUM
{
    LRRC_APP_LNAS_EST_CAUSE_EMERGENCY_CALL          = 0,
    LRRC_APP_LNAS_EST_CAUSE_HIGH_PRIORITY_ACCESS,
    LRRC_APP_LNAS_EST_CAUSE_MT_ACCESS,
    LRRC_APP_LNAS_EST_CAUSE_MO_SIGNALLING,
    LRRC_APP_LNAS_EST_CAUSE_MO_DATA,
    LRRC_APP_LNAS_EST_CAUSE_BUTT
};
typedef VOS_UINT8 LRRC_APP_LNAS_EST_CAUSE_ENUM_UINT8;

/*****************************************************************************
 枚举名    : LRRC_LNAS_EST_RESULT_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 信令连接结果枚举类型
*****************************************************************************/
enum LRRC_APP_LNAS_EST_RESULT_ENUM
{
    LRRC_APP_EST_SUCCESS                         = 0,       /* 信令连接建立成功 */
    LRRC_APP_EST_PRESENT                         = 1,       /* 该信令连接已经存在 */
    LRRC_APP_EST_ESTING                          = 2,       /* 该信令连接正在建立 */
    LRRC_APP_EST_DELING                          = 3,       /* 该信令连接正在释放 */
    LRRC_APP_EST_ACCESS_BARRED_MO_CALL           = 4,       /* access barred when MO Call */
    LRRC_APP_EST_ACCESS_BARRED_MO_SIGNAL         = 5,       /* access barred When MO SIGNAL */
    LRRC_APP_EST_ACCESS_BARRED_EMERGENCY_CALL    = 6,       /* access barred When Emergency Call */
    LRRC_APP_EST_ACCESS_BARRED_ALL               = 7,       /* access barred When Rrc Con Rej */
    LRRC_APP_EST_EST_CONN_FAIL                   = 8,       /* RR connection establish failure  */
    LRRC_APP_EST_RJ_INTER_RAT                    = 9,       /* RR connection Reject: 指示进入异系统 */
    LRRC_APP_EST_IMMEDIATE_ASSIGN_REJECT         = 10,      /* 正常的接入过程，发送RA脉冲后，收到立即指派拒绝消息，
                                                                没有收到立即指派消息而导致的接入失败 */
    LRRC_APP_EST_RANDOM_ACCESS_REJECT            = 11,      /* 正常的接入过程，发送RA脉冲后，没有收到网络侧的立即指派，
                                                                也没有收到立即指派拒绝，接入失败 */
    LRRC_APP_EST_CELL_SEARCHING                  = 12,      /* RRC目前不能处理建链请求，NAS收到该原因值，
                                                                直到收到RRC_MM_SYS_INFO_IND再发起建链*/
    LRRC_APP_EST_RESULT_BUTT
};
typedef VOS_UINT8 LRRC_APP_LNAS_EST_RESULT_ENUM_UINT8;

/*****************************************************************************
 枚举名    : LRRC_FLOW_CTRL_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 流程控制类型枚举
             修改枚举时需要注意，失败类型取值 = 启动类型取值+1
 比如: LRRC_FLOW_CTRL_TYPE_G2L_RESEL_FAIL = LRRC_FLOW_CTRL_TYPE_G2L_START_RESEL + 1
*****************************************************************************/
enum LRRC_APP_FLOW_CTRL_TYPE_ENUM
{
    LRRC_APP_FLOW_CTRL_TYPE_L2L_NORMAL            = 0,     /* LTE内流程 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_START_RESEL,               /* G->L开始重选 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_RESEL_FAIL,                /* G->L重选失败 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_STOP_RESEL,                /* G->L停止重选 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_START_REDIR,               /* G->L开始重定向 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_REDIR_FAIL,                /* G->L重定向失败 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_STOP_REDIR,                /* G->L停止重定向 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_START_CCO,                 /* G->L开始CCO */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_CCO_FAIL,                  /* G->L CCO失败 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_STOP_CCO,                  /* G->L停止CCO */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_START_HO,                  /* G->L开始HO */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_HO_FAIL,                   /* G->L HO失败 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_STOP_HO,                   /* G->L停止HO */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_START_RESEL,               /* W->L开始重选 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_RESEL_FAIL,                /* W->L重选失败 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_STOP_RESEL,                /* W->L停止重选 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_START_REDIR,               /* W->L开始重定向 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_REDIR_FAIL,                /* W->L重定向失败 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_STOP_REDIR,                /* W->L停止重定向 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_START_HO,                  /* W->L开始HO */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_HO_FAIL,                   /* W->L HO失败 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_STOP_HO,                   /* W->L停止HO */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_START_PLMN,                /* W->L开始PLMN搜索 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_DONE_PLMN,                 /* W->L PLMN搜索完成 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_STOP_PLMN,                 /* W->L停止PLMN搜索 */
    LRRC_APP_FLOW_CTRL_TYPE_L2G_START_RESEL,               /* L->G开始重选 */
    LRRC_APP_FLOW_CTRL_TYPE_L2G_RESEL_FAIL,                /* L->G重选失败 */
    LRRC_APP_FLOW_CTRL_TYPE_L2G_START_REDIR,               /* L->G重定向 */
    LRRC_APP_FLOW_CTRL_TYPE_L2G_REDIR_FAIL,                /* L->G重定向失败 */
    LRRC_APP_FLOW_CTRL_TYPE_L2G_START_CCO,                 /* L->G开始CCO */
    LRRC_APP_FLOW_CTRL_TYPE_L2G_CCO_FAIL,                  /* L->G CCO失败 */
    LRRC_APP_FLOW_CTRL_TYPE_L2G_START_HO,                  /* L->G开始切换 */
    LRRC_APP_FLOW_CTRL_TYPE_L2G_HO_FAIL,                   /* L->G切换失败 */
    LRRC_APP_FLOW_CTRL_TYPE_L2W_START_RESEL,               /* L->W开始重选 */
    LRRC_APP_FLOW_CTRL_TYPE_L2W_RESEL_FAIL,                /* L->W重选失败 */
    LRRC_APP_FLOW_CTRL_TYPE_L2W_START_REDIR,               /* L->W重定向 */
    LRRC_APP_FLOW_CTRL_TYPE_L2W_REDIR_FAIL,                /* L->W重定向失败 */
    LRRC_APP_FLOW_CTRL_TYPE_L2W_START_HO,                  /* L->W开始切换 */
    LRRC_APP_FLOW_CTRL_TYPE_L2W_HO_FAIL,                   /* L->W切换失败 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_IDLE_MEAS,                 /* G->L IDLE测量配置 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_IDLE_MEAS,                 /* W->L IDLE测量配置 */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_CONN_MEAS,                 /* G->L CONN测量配置 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_CONN_MEAS,                 /* W->L CONN测量配置 */
    LRRC_APP_FLOW_CTRL_TYPE_W2L_RELALL,                    /* W->L RELALL */
    LRRC_APP_FLOW_CTRL_TYPE_L2W_START_CELL_SRCH,           /* L->W 搜网 */
    LRRC_APP_FLOW_CTRL_TYPE_L2W_STOP_CELL_SRCH,            /* L->W 停止搜网 */
    LRRC_APP_FLOW_CTRL_TYPE_L2G_START_CELL_SRCH,           /* L->G 搜网 */
    LRRC_APP_FLOW_CTRL_TYPE_L2G_STOP_CELL_SRCH,            /* L->G 停止搜网 */
    LRRC_APP_FLOW_CTRL_TYPE_T2L_RELALL,                    /* T->L RELALL */
    LRRC_APP_FLOW_CTRL_TYPE_G2L_RELALL,                    /* G->L RELALL */
    LRRC_APP_FLOW_CTRL_TYPE_L2T_START_REDIR,               /* L->T重定向 */
    LRRC_APP_FLOW_CTRL_TYPE_L2T_START_HO,                  /* L->T开始切换 */
    LRRC_APP_FLOW_CTRL_TYPE_L2T_START_RESEL,               /* L->T开始重选 */
    LRRC_APP_FLOW_CTRL_TYPE_T2L_IDLE_MEAS,                 /* T->L IDLE测量配置 */
    LRRC_APP_FLOW_CTRL_TYPE_T2L_CONN_MEAS,                 /* G->L CONN测量配置 */

    LRRC_APP_FLOW_CTRL_TYPE_BUTT
};
typedef VOS_UINT8 LRRC_APP_FLOW_CTRL_TYPE_ENUM_UINT8;

/*****************************************************************************
 枚举名    : RRC_APP_BOOL_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : PS统一布尔类型枚举定义
*****************************************************************************/
enum RRC_APP_BOOL_ENUM
{
    RRC_APP_PS_FALSE                            = 0,
    RRC_APP_PS_TRUE                             = 1,
    RRC_APP_PS_BOOL_BUTT
};
typedef VOS_UINT8   RRC_APP_BOOL_ENUM_UINT8;

/*****************************************************************************
 枚举名    : RRC_UE_CATEGORY_ENUM_UINT8
 协议表格  :
 ASN.1描述 :
 枚举说明  : UE Category，请参见36306协议
*****************************************************************************/
enum RRC_APP_CATEGORY_ENUM
{
    RRC_APP_UE_CATEGORY_1                   = 0,
    RRC_APP_UE_CATEGORY_2                   = 1,
    RRC_APP_UE_CATEGORY_3                   = 2,
    RRC_APP_UE_CATEGORY_4                   = 3,
    RRC_APP_UE_CATEGORY_5                   = 4,
    RRC_APP_UE_CATEGORY_6                   = 5,
    RRC_APP_UE_CATEGORY_7                   = 6,
    RRC_APP_UE_CATEGORY_BUTT
};
typedef VOS_UINT8 RRC_APP_CATEGORY_ENUM_UINT8;
/*****************************************************************************
 枚举名    : RRC_CONN_REEST_IND_ENUM_UINT8
 协议表格  :
 ASN.1描述 :
 枚举说明  : RRC连接重建发起的原因
*****************************************************************************/
enum RRC_APP_CONN_REEST_IND_ENUM
{
    RRC_APP_CONN_REEST_FOR_RBCFG_ERROR      = 0,                                    /* 0:由于RB配置失败导致的RRC连接重建 */
    RRC_APP_CONN_REEST_FOR_HANDOVER_ERROR   = 1,                                    /* 1:由于切换失败导致的RRC连接重建 */
    RRC_APP_CONN_REEST_FOR_OTHER_ERROR      = 2,                                    /* 2:由于无线链路失败或者完整性验证失败导致的RRC连接重建 */
    RRC_APP_CONN_REEST_FOR_BUTT
};
typedef VOS_UINT8 RRC_APP_CONN_REEST_IND_ENUM_UINT8;

/*********************************************************
 枚举名    : RRC_APP_FTM_CHANGE_INFO_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 变更上报工程模式类型定义
**********************************************************/
enum RRC_APP_FTM_CHANGE_INFO_TYPE_ENUM
{
    RRC_APP_FTM_CHANGE_CELL_INFO,  /* RRC驻留小区信息，变化后上报，切换或重选完成后上报 */
    RRC_APP_FTM_CHANGE_STATE_INFO, /* RRC状态信息，变化后上报 */
    RRC_APP_FTM_CHANGE_BUTT
};

typedef VOS_UINT32 RRC_APP_FTM_CHANGE_INFO_TYPE_ENUM_UINT32;
/*********************************************************
 枚举名    : LRRC_LPHY_LTE_BAND_WIDTH_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : LTE带宽，WIDTH_3M就是表示下行带宽是3M，界面显示3M即可
**********************************************************/
enum LRRC_APP_BAND_WIDTH_ENUM
{
    LRRC_APP_LTE_BAND_WIDTH_1D4M = 0,
    LRRC_APP_LTE_BAND_WIDTH_3M,
    LRRC_APP_LTE_BAND_WIDTH_5M,
    LRRC_APP_LTE_BAND_WIDTH_10M,
    LRRC_APP_LTE_BAND_WIDTH_15M,
    LRRC_APP_LTE_BAND_WIDTH_20M,
    LRRC_APP_LTE_BAND_WIDTH_BUTT
};
typedef VOS_UINT16 LRRC_APP_BAND_WIDTH_ENUM_UINT16;

/*****************************************************************************
   5 STRUCT
*****************************************************************************/
/*****************************************************************************
                  工程模式上报结构体定义
*****************************************************************************/
/*********************************************************************
* ASN 文本 :MCC
*********************************************************************/
typedef struct
{
    VOS_UINT8                           ucCnt;    /*(0, 3)*/
    VOS_UINT8                           aucItem[RRC_APP_MAX_NUM_OF_MCC];
}RRC_APP_MCC_STRU;

/*********************************************************************
* ASN 文本 :PLMN-Identity
*********************************************************************/
typedef struct
{
    RRC_APP_MCC_STRU                         Mcc;                          /* optional */
    RRC_APP_MCC_STRU                         Mnc;
}LRRC_APP_PLMN_ID_STRU;
/*****************************************************************************
 结构名    :APP_RRC_Txpwr
 结构说明  :RRC上报给APP的上行发射功率
*****************************************************************************/
typedef struct
{
    VOS_INT16                        sPuschTxPwr;
    VOS_INT16                        sPucchTxPwr;
    VOS_INT16                        sSrsTxpwr;
    VOS_UINT16                       usReserved;
}RRC_APP_Txpwr_INFO_STRU;
/*****************************************************************************
 结构名    :RRC_APP_PUSCH_BLER_STRU
 结构说明  :RRC上报给APP的PUSCH误码率结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32             ulAckCnt;    /* 收到的ACK个数 */
    VOS_UINT32             ulNackCnt;   /* 收到的NACK个数 */
}RRC_APP_PUSCH_BLER_STRU;

/*****************************************************************************
 结构名    :RRC_APP_PDSCH_BLER_STRU
 结构说明  :RRC上报给APP的PDSCH误码率结构，需要显示初始误码率和累计误码率，需要分码字显示
*****************************************************************************/
typedef struct
{
    VOS_UINT16              ausCWInitTotal[NUM_OF_CODEWORD]; /* 初始误码率，收到的子帧个数，包括错误和正确的 */
    VOS_UINT16              ausCWInitError[NUM_OF_CODEWORD]; /* 初始误码率，收到的错误子帧个数 */
    VOS_UINT16              ausCWReTotal[NUM_OF_CODEWORD];   /* 残留误码率，收到的子帧个数，包括错误和正确的 */
    VOS_UINT16              ausCWReError[NUM_OF_CODEWORD];   /* 残留误码率，收到的错误子帧个数 */
}RRC_APP_PDSCH_BLER_STRU;

/*****************************************************************************
 结构名    :RRC_APP_PDCCH_BLER_STRU
 结构说明  :RRC上报给APP的上下行授权个数
*****************************************************************************/
typedef struct
{
    VOS_UINT32              ulDlGrantCnt; /* 下行授权个数 */
    VOS_UINT32              ulUlGrantCnt; /* 上行授权个数 */
}RRC_APP_PDCCH_BLER_STRU;

/*****************************************************************************
 结构名    :APP_RRC_BLER_INFO_STRU
 结构说明  :RRC上报给APP的误码率结构
*****************************************************************************/
typedef struct
{
    RRC_APP_PUSCH_BLER_STRU         stPuschBlerCnt; /* PUSCH 误块率BLER */
    RRC_APP_PDSCH_BLER_STRU         stPdschBlerCnt; /* PDSCH 误块率BLER */
    RRC_APP_PDCCH_BLER_STRU         stPdcchBlerCnt; /* PDCCH 上下行授权个数 */
}RRC_APP_BLER_INFO_STRU;

/*****************************************************************************
 结构名    :RRC_APP_PERIOD_CELL_INFO_STRU
 结构说明  :RRC周期上报给APP的驻留小区信息
*****************************************************************************/
typedef struct
{
    VOS_INT16                           sRsrp;              /* RSRP测量值范围：(-141,-44)，单位dbm*/
    VOS_INT16                           sRsrq;              /* RSRQ测量值范围：(-40, -6) */
    VOS_INT16                           sRssi;              /* RSSI测量值，单位dbm */
    VOS_UINT16                          usRI;               /* RI值 */
    VOS_UINT16                          ausCQI[2];          /* CQI两个码字，只有一个码字时，数组第一个值有效 */
    VOS_INT32                           lSINR;              /* SINR，小区参考信号信噪比 */
    RRC_APP_Txpwr_INFO_STRU             stTxPwr;            /* LTE上行信道发送功率 */
    RRC_APP_BLER_INFO_STRU              stBler;             /* LTE误码率 */
}RRC_APP_PERIOD_CELL_INFO_STRU;
/*****************************************************************************
 结构名    : RRC_APP_FREQ_PARA_STRU
结构说明  : RRC上报的消息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usUlFreq;          /* 上行中心频点 单位:100Khz */
    VOS_UINT16                              usDlFreq;          /* 下行中心频点 单位:100Khz */
    LRRC_APP_BAND_WIDTH_ENUM_UINT16         enDlBandwidth;     /* 下行带宽 */
    VOS_UINT8                               usBand;            /* 频带指示 */
    VOS_UINT8                               aucResv[1];
} RRC_APP_FREQ_PARA_STRU;

/*****************************************************************************
 结构名    : RRC_APP_CHANGE_CELL_INFO_STRU
 结构说明  : 变化上报的工程模式结构--驻留小区信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                                ulValidNum;                      /* LTE支持CA情况下，有效载频个数 */
    RRC_APP_FREQ_PARA_STRU                    astFreqInfo[LRRC_APP_MAX_NUM];   /* LTE支持CA情况下，上下行频点信息 */
    VOS_UINT16                                astPhyCellId[LRRC_APP_MAX_NUM];  /* LTE支持CA情况下，物理小区ID信息 */
    VOS_UINT8                                 ucSbuFrmAssignmentType;          /* TDD模式下有效，LTE子帧配比，0xFF为无效值 */
    VOS_UINT8                                 ucSpecialSubFrmPatterns;         /* TDD模式下有效，LTE特殊子帧配比，0xFF为无效值 */
    RRC_APP_PAGING_CYCLE_ENUM_UINT32          enPagingCycle;                   /* 寻呼周期 */
    RRC_APP_TRANSM_MODE_ENUM_UINT32           enTranMode;                      /* LTE传输模式 */
}RRC_APP_CHANGE_CELL_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_FTM_CHANGE_INFO_STRU
 结构说明  : 变化上报的工程模式结构
*****************************************************************************/
typedef struct
{
    RRC_APP_FTM_CHANGE_INFO_TYPE_ENUM_UINT32  ulFtmChangeInfoType;

    union
    {
        RRC_APP_CHANGE_CELL_INFO_STRU             stCellInfo;  /* 变化上报的工程模式结构--驻留小区信息 */
        RRC_APP_PROTOCOL_STATE_IND_ENUM_UINT8     enState;     /* 变化上报的工程模式结构--状态信息，包括协议状态和内部代码状态 */
    }u;
}RRC_APP_FTM_CHANGE_INFO_STRU;
/*****************************************************************************
 结构名    : RRC_APP_FTM_PERIOD_INFO_STRU
 结构说明  : 周期上报的工程模式结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                                 ulValidNum;
    RRC_APP_PERIOD_CELL_INFO_STRU              astCellInfo[LRRC_APP_MAX_NUM]; /* RRC周期上报给APP的驻留小区信息 */
}RRC_APP_FTM_PERIOD_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_FTM_INFO_STRU
 结构说明  : 工程模式上报数据结构
*****************************************************************************/
typedef struct
{
    RRC_APP_FTM_TYPE_ENUM_UINT32             enRrcAppFtmType; /* 工程模式下，上报信息类型--周期上报和变更上报类型 */

    union
    {
        RRC_APP_FTM_PERIOD_INFO_STRU         stPeriodInfo;    /* 工程模式下，周期上报内容， Idle状态上报周期目前为DRX周期，Connected状态下周期为1秒*/
        RRC_APP_FTM_CHANGE_INFO_STRU         stChangeInfo;    /* 工程模式下，变更后上报内容 */
    }u;
}RRC_APP_FTM_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_FTM_INFO_STRU
 结构说明  : 工程模式上报数据结构
*****************************************************************************/
typedef struct
{
    /* 子模块ID */
    OM_ERR_LOG_MOUDLE_ID_ENUM_UINT32    ulMsgModuleID;

    /* 00：主卡, 01：副卡 ,10/11:保留  */
    VOS_UINT16                          usModemId;

    VOS_UINT16                          usProjectID;
    VOS_UINT32                          ulLength;

    RRC_APP_FTM_INFO_STRU               stLrrcFtmInfo;

}LRRC_OM_FTM_REPROT_CONTENT_STRU;

/*****************************************************************************
 结构名    : RRC_APP_FTM_INFO_STRU
 结构说明  : 工程模式上报数据结构
*****************************************************************************/

/* 各组件工程模式主动上报内容 各组件 -> OM*/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;
    VOS_UINT32                          ulMsgType;
    VOS_UINT32                          ulMsgSN;
    VOS_UINT32                          ulRptlen;      /* 工程模式上报的内容长度,如果ulRptlen为0,aucContent内容长度也为0 */
    LRRC_OM_FTM_REPROT_CONTENT_STRU     stLrrcFtmContent;
}LRRC_OM_FTM_REPROT_IND_STRU;

/*****************************************************************************
                   ERROR LOG上报结构体定义
*****************************************************************************/

/*****************************************************************************
 结构名    : RRC_APP_ACC_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RRC接入信息存储
*****************************************************************************/
typedef struct
{
    VOS_UINT8                              ucAsRelBitmap;      /* 用来设置UE接入层版本，1：支持，0：不支持，从低到高位分别用来指示Rel8,Rel9,.... */
    VOS_UINT8                              aucReserved[3];
    VOS_UINT32                             ulFeatureGroupInds; /* 请参见36.331 B.1  Feature group indicators 章节，按位取值 */
}RRC_APP_ACC_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_RRC_EST_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RRC建链信息存储
*****************************************************************************/
typedef struct
{
    LRRC_APP_LNAS_EST_CAUSE_ENUM_UINT8     enEstCause;             /* 建链原因 */
    LRRC_APP_LNAS_EST_RESULT_ENUM_UINT8    enResult;               /* 建链结果 */
    VOS_UINT16                             usReserved;
}RRC_APP_RRC_EST_LIST_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_EST_INFO_STRU
 结构说明  : ERR LOG 连接建立数据结构,
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               ucEstCnt; /* 当前存储的建链信息个数 */
    VOS_UINT8                               aucRseverd[3];
    RRC_APP_RRC_EST_LIST_INFO_STRU          astEstInfoList[RRC_APP_MAX_RRC_EST_INFO_NUM];   /* 建链结果 */
}RRC_APP_RRC_EST_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_EST_INFO_STRU
 结构说明  : ERR LOG 连接建立数据结构
*****************************************************************************/
typedef struct
{
    RRC_APP_RRC_EST_INFO_STRU               stEstInfo;   /* RRC建链信息保存结构 */
    RRC_APP_ACC_INFO_STRU                   stAccInfo;   /* RRC建链时，UE能力保存结构 */
}RRC_APP_EST_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_SYNC_INFO_STRU
 结构说明  : ERR LOG 同步信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32          ulInSyncCnt;   /* LRRC收到LPHY的物理层同步个数 */
    VOS_UINT32          ulOutSyncCnt;  /* LRRC收到LPHY的物理层失步个数 */
}RRC_APP_SYNC_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_REEST_INFO_STRU
 结构说明  : ERR LOG 重建立数据结构
*****************************************************************************/
typedef struct
{
    RRC_APP_CONN_REEST_IND_ENUM_UINT8   enReestConnCause;   /* 最近一次重建原因 */
    VOS_UINT8                           aucReserved[3];
    RRC_APP_SYNC_INFO_STRU              stSyncInfo;         /* 物理层同步信息 */
}RRC_APP_REEST_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_ERR_LOG_PRINT_STRU
 结构说明  : ERROR LOG RRC 错误打印信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                   ulFileIdx; /* 文件名，统一编号，使用数字 */
    VOS_UINT32                   ulLineNo;  /* 行号 */
    VOS_INT32                    lPara1;    /* 参数1 */
    VOS_INT32                    lPara2;    /* 参数2 */
    VOS_INT32                    lPara3;    /* 参数3 */
    VOS_INT32                    lPara4;    /* 参数4 */
}RRC_APP_ERROR_PRINT_STRU;
/*****************************************************************************
 结构名    : RRC_APP_RECIVE_MSG_STRU
 结构说明  : RRC 入口消息的信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                   ulSendPid;    /* 发送任务Pid */
    VOS_UINT32                   ulMsgName;    /* 发送消息ID */
}RRC_APP_RECIVE_MSG_STRU;

/*****************************************************************************
 结构名    : RRC_APP_ERROR_PRINT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :打印打点信息保存结构
 ulNextIndex保存下一个的错误打印索引，例如ulNextIndex = 1，则astErrLogPrint[0]
 是最新的错误打印
*****************************************************************************/
typedef struct
{
    VOS_UINT32                     ulNextIndex;
    RRC_APP_ERROR_PRINT_STRU       astErrLogPrint[RRC_APP_ERROR_PRINT_INFO_NUM];
} RRC_APP_ERROR_PRINT_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_RECIVE_MSG_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 发生故障时，模块收到的最新消息
  ulNextIndex保存下一个收到消息索引，例如ulNextIndex = 1，则astReciveMsgInfo[0]
 是最新的收到的消息ID
*****************************************************************************/
typedef struct
{
    VOS_UINT32                         ulNextIndex;
    RRC_APP_RECIVE_MSG_STRU            astReciveMsgInfo[RRC_APP_RECEIVE_MSG_INFO_NUM];
} RRC_APP_RECIVE_MSG_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_STATE_INFO_STRU
 结构说明  : ERROR LOG RRC状态信息
*****************************************************************************/
typedef struct
{
    RRC_APP_PROTOCOL_STATE_IND_ENUM_UINT8       enState;            /* RRC协议状态 */  
    VOS_UINT8                                   ucErrcTopMsState;   /* RRC主状态机，内部代码的状态机 */
    VOS_UINT8                                   ucErrcTopSsState;   /* RRC子状态机，内部代码的状态机 */
    LRRC_APP_FLOW_CTRL_TYPE_ENUM_UINT8          enErrcFlowCtrlType; /* LTE发生异系统过程时，异系统过程标识 */
}RRC_APP_STATE_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_FTM_PERIOD_INFO
 结构说明  : ERROR LOG 数据结构
*****************************************************************************/
typedef struct
{
    RRC_APP_ERROR_PRINT_INFO_STRU    stRrcAppErrorPrintInfo; /* RRC打印打点信息 */
    RRC_APP_RECIVE_MSG_INFO_STRU     stRrcAppReciveMsgInfo;  /* 发生故障时，RRC收到的最新消息 */
    RRC_APP_STATE_INFO_STRU          stRrcAppStateInfo;      /* 发生故障时，RRC状态信息，包括协议状态机和内部代码状态机 */
    RRC_APP_EST_INFO_STRU            stRrcAppEstInfo;        /* 发生故障时，RRC连接建立记录信息 */
    RRC_APP_REEST_INFO_STRU          stRrcAppReestInfo;      /* 发生故障时，RRC重连接建立记录信息 */
}RRC_APP_ERR_LOG_RRC_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_CODE_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RRC错误码保存结构
  ulNextIndex保存下一个错误码索引，例如ulNextIndex = 1，则aenRrcAppErrorCode[0]是最新的收到的消息ID;
                                          ulNextIndex = 0，则aenRrcAppErrorCode[0] = RRC_APP_ERR_CODE_BUTT;则RRC没有保存错误码
*****************************************************************************/
typedef struct
{
    VOS_UINT32                         ulNextIndex;
    RRC_APP_ERROR_CODE_ENUM_UINT16     aenRrcAppErrorCode[RRC_APP_CODE_NUM];
} RRC_APP_CODE_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_FTM_PERIOD_INFO
 结构说明  : ERROR LOG 数据结构
*****************************************************************************/
typedef struct
{
    RRC_APP_CODE_INFO_STRU           stCodeInfo;           /* 错误码上报结构，每个模块单独编号 */
    RRC_APP_ERR_LOG_RRC_INFO_STRU    stErrorLogInfo;       /* 错误定位信息上报结构 */
}RRC_APP_ERR_LOG_INFO_STRU;
/*****************************************************************************
 结构名    : RRC_APP_FTM_PERIOD_INFO
 结构说明  : 与终端交互的ERROR LOG 数据结构
*****************************************************************************/
typedef struct
{
    /* 子模块ID */
    OM_ERR_LOG_MOUDLE_ID_ENUM_UINT32    ulMsgModuleID;

    /* 00：主卡, 01：副卡 ,10/11:保留  */
    VOS_UINT16                          usModemId;

    /*  sub sys,modeule,sub module   暂不使用   */
    VOS_UINT16                          usALMID;

    /* ERR LOG上报级别,
    Warning：0x04代表提示，
    Minor：0x03代表次要
    Major：0x02答标重要
    Critical：0x01代表紧急    */
    VOS_UINT16                          usALMLevel;

    /* 每个ERR LOG都有其类型:
    故障&告警类型
    通信：0x00
    业务质量：0x01
    处理出错：0x02
    设备故障：0x03
    环境故障：0x04    */
    VOS_UINT16                          usALMType;

    VOS_UINT32                          usAlmLowSlice;/*时间戳*/
    VOS_UINT32                          usAlmHighSlice;

    VOS_UINT32                          ulAlmLength;
    RRC_APP_ERR_LOG_INFO_STRU           stAlmInfo;
}LRRC_OM_ERR_LOG_INFO_STRU;

/*****************************************************************************
 结构名    : OM_ERR_LOG_REPORT_CNF_STRU
 结构说明  : Balong平台内部交互的ERROR LOG 数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;
    VOS_UINT32                          ulMsgType;
    VOS_UINT32                          ulMsgSN;
    VOS_UINT32                          ulRptlen;      /* 故障内容长度,如果ulRptlen为0,aucContent内容长度也为0 */
    LRRC_OM_ERR_LOG_INFO_STRU           stErrLogInfo;
} LRRC_OM_ERR_LOG_REPORT_CNF_STRU;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
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

#endif /* end of LrrcErrLogInterface.h */


