

#ifndef __NAS_ERRORLOG_H__
#define __NAS_ERRORLOG_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "errorlog.h"
#include "omerrorlog.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define NAS_FTM_MAX_PTMSI_LEN                           (4)                     /* 最大的PTMSI的长度 */
#define NAS_FTM_MAX_TMSI_LEN                            (4)                     /* 最大的TMSI的长度 */

#define TAF_SDC_RING_BUFFER_SIZE                        (1024 * 2)              /* TAF层的环形共享缓存大小 */
#define NAS_MML_RING_BUFFER_SIZE                        (1024 * 2)              /* MM层的环形共享缓存大小 */

#if (FEATURE_ON == FEATURE_LTE)
#define NAS_ERR_LOG_MML_MAX_RAT_NUM                     (3)                     /* 当前支持的接入技术个数 */
#else
#define NAS_ERR_LOG_MML_MAX_RAT_NUM                     (2)                     /* 当前支持的GU接入技术个数 */
#endif

#define NAS_ERR_LOG_CTRL_LEVEL_NULL                     (0)                     /* ErrLog等级未定义*/
#define NAS_ERR_LOG_CTRL_LEVEL_CRITICAL                 (1)                     /* ErrLog等级为紧急 */
#define NAS_ERR_LOG_CTRL_LEVEL_MAJOR                    (2)                     /* ErrLog等级为重要 */
#define NAS_ERR_LOG_CTRL_LEVEL_MINOR                    (3)                     /* ErrLog等级为次要 */
#define NAS_ERR_LOG_CTRL_LEVEL_WARNING                  (4)                     /* ErrLog等级为提示 */


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


enum NAS_ERR_LOG_NET_RAT_TYPE_ENUM
{
    NAS_ERR_LOG_NET_RAT_TYPE_GSM,           /* GSM接入技术 */
    NAS_ERR_LOG_NET_RAT_TYPE_WCDMA,         /* WCDMA接入技术 */
    NAS_ERR_LOG_NET_RAT_TYPE_LTE,           /* LTE接入技术 */
    NAS_ERR_LOG_NET_RAT_TYPE_BUTT           /* 无效的接入技术 */
};
typedef VOS_UINT8 NAS_ERR_LOG_NET_RAT_TYPE_ENUM_UINT8;


enum NAS_ERR_LOG_PS_ACTION_TYPE_ENUM
{
    NAS_ERR_LOG_GMM_ACTION_RAU                  = 0,
    NAS_ERR_LOG_GMM_ACTION_ATTACH               = 1,
    NAS_ERR_LOG_GMM_ACTION_COMBINED_RAU         = 2,
    NAS_ERR_LOG_GMM_ACTION_COMBINED_ATTACH      = 3,
    NAS_ERR_LOG_GMM_ACTION_PERIODC_RAU          = 4,
    NAS_ERR_LOG_GMM_ACTION_BUTT
};
typedef VOS_UINT32 NAS_ERR_LOG_GMM_ACTION_TYPE_ENUM_U32;


enum NAS_ERR_LOG_REG_DOMAIN_ENUM
{
    NAS_ERR_LOG_REG_DOMAIN_PS    = 0x00,                                        /* 注册结果域为PS域 */
    NAS_ERR_LOG_REG_DOMAIN_PS_CS = 0x01,                                        /* 注册结果域为CS+PS域 */
    NAS_ERR_LOG_REG_DOMAIN_BUTT
};
typedef VOS_UINT32 NAS_ERR_LOG_REG_DOMAIN_ENUM_UINT32;


enum NAS_ERR_LOG_CALL_EX_STATE_ENUM
{
    NAS_ERR_LOG_CALL_S_ACTIVE,                                                  /* active */
    NAS_ERR_LOG_CALL_S_HELD,                                                    /* held */
    NAS_ERR_LOG_CALL_S_DIALING,                                                 /* dialing (MO call) */
    NAS_ERR_LOG_CALL_S_ALERTING,                                                /* alerting (MO call) */
    NAS_ERR_LOG_CALL_S_INCOMING,                                                /* incoming (MT call) */
    NAS_ERR_LOG_CALL_S_WAITING,                                                 /* waiting (MT call) */
    NAS_ERR_LOG_CALL_S_IDLE,                                                    /* idle */
    NAS_ERR_LOG_CALL_S_CCBS_WAITING_ACTIVE,                                     /* CCBS等待激活态 */
    NAS_ERR_LOG_CALL_S_CCBS_WAITING_RECALL,                                     /* CCBS等待回呼态 */
    NAS_ERR_LOG_CALL_S_UNKNOWN,                                                 /* unknown state */
    NAS_ERR_LOG_CALL_S_MO_FDN_CHECK,                                            /* FDN check */
    NAS_ERR_LOG_CALL_S_MO_CALL_CONTROL_CHECK,                                   /* CALL CONTROL check */
    NAS_ERR_LOG_CALL_S_BUTT
};
typedef VOS_UINT8  NAS_ERR_LOG_CALL_STATE_ENUM_U8;


enum NAS_ERR_LOG_ALM_ID_ENUM
{
    NAS_ERR_LOG_ALM_CS_REG_FAIL     = 0x01,                                     /* CS域注册失败 */
    NAS_ERR_LOG_ALM_PS_REG_FAIL     = 0x02,                                     /* PS域注册失败 */
    NAS_ERR_LOG_ALM_SEARCH_NW_FAIL  = 0x03,                                     /* 搜网失败 */
    NAS_ERR_LOG_ALM_CS_CALL_FAIL    = 0x04,                                     /* CS呼叫失败及异常挂断 */
    NAS_ERR_LOG_ALM_PS_CALL_FAIL    = 0x05,                                     /* PS呼叫失败及异常挂断 */
    NAS_ERR_LOG_ALM_SMS_FAIL        = 0x06,                                     /* 短信失败 */
    NAS_ERR_LOG_ALM_VC_OPT_FAIL     = 0x07,                                     /* VC操作失败 */
    NAS_ERR_LOG_ALM_ID_BUTT
};
typedef VOS_UINT16  NAS_ERR_LOG_ALM_ID_ENUM_U16;
enum NAS_ERR_LOG_ALM_TYPE_ENUM
{
    NAS_ERR_LOG_ALM_TYPE_COMMUNICATION    = 0x00,                               /* 通信 */
    NAS_ERR_LOG_ALM_TYPE_TRAFFIC_QUALITY  = 0x01,                               /* 业务质量 */
    NAS_ERR_LOG_ALM_TYPE_DEAL_WRONG       = 0x02,                               /* 处理出错 */
    NAS_ERR_LOG_ALM_TYPE_DEVICE_FAILURE   = 0x03,                               /* 设备故障 */
    NAS_ERR_LOG_ALM_TYPE_ENV_FAILURE      = 0x04,                               /* 环境故障 */
    NAS_ERR_LOG_ALM_TYPE_BUTT
};
typedef VOS_UINT16  NAS_ERR_LOG_ALM_TYPE_ENUM_U16;
enum NAS_ERR_LOG_FTM_PROJECT_ID_ENUM
{
    NAS_FTM_PROJECT_TMSI_RPT,
    NAS_FTM_PROJECT_PTMSI_RPT,

    NAS_FTM_PROJECT_ID_BUTT,
};
typedef VOS_UINT16  NAS_ERR_LOG_FTM_PROJECT_ID_ENUM_U8;


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

typedef struct
{
    MNTN_PLMN_ID_STRU                   stPlmn;
    VOS_UINT32                          ulLac;
}NAS_ERR_LOG_LAI_INFO_STRU;


typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;
    VOS_UINT32                          ulLuResult;                             /* LU结果            */
    NAS_ERR_LOG_NET_RAT_TYPE_ENUM_UINT8 enNetType;                              /* 当前注册的网络类型 */
    VOS_UINT8                           ucIsComBined;                           /* 是否为联合注册   */
    VOS_UINT16                          usRegFailCause;                         /* 操作失败原因     */
    VOS_UINT32                          ulServiceStatus;                        /* 服务状态          */
    VOS_UINT32                          ulCsUpdateStatus;                       /* CS 域更新状态     */
    VOS_UINT32                          ulLuAttemptCnt;                         /* LU Attempt counter */
    NAS_ERR_LOG_LAI_INFO_STRU           stOldLai;                               /* 旧LAI              */
}NAS_ERR_LOG_CS_REG_RESULT_EVENT_STRU;


typedef struct
{
    OM_ERR_LOG_HEADER_STRU                    stHeader;
    NAS_ERR_LOG_GMM_ACTION_TYPE_ENUM_U32      enGmmActionType;                  /* GMM的操作类型 */
    VOS_UINT32                                ulActionResult;                   /* 注册结果 */
    NAS_ERR_LOG_REG_DOMAIN_ENUM_UINT32        enReqDomain;                      /* 当前注册的结果域，Ps Only或CS_PS */
    NAS_ERR_LOG_REG_DOMAIN_ENUM_UINT32        enRsltDomain;                     /* 当前注册的结果域，Ps Only或CS_PS */
    VOS_UINT16                                usRegFailCause;                   /* 操作失败原因 */
    NAS_ERR_LOG_NET_RAT_TYPE_ENUM_UINT8       enNetType;                        /* 当前注册的网络类型 */
    VOS_UINT8                                 ucReserved;                       /* 保留域 */
    VOS_UINT32                                ulRegCounter;                     /* Attach attempt counter */
    VOS_UINT32                                ulServiceStatus;                  /* 服务状态 */
}NAS_ERR_LOG_PS_REG_RESULT_EVENT_STRU;
typedef struct
{
    VOS_UINT8                           ucSearchAllBand;   /* 是否在当前接入技术执行过全频搜网, VOS_TRUE:执行过全频搜, VOS_FALSE:未执行过 */
    VOS_UINT8                           ucCoverageType;    /* 当前接入技术下的网络覆盖类型, 0:不存在覆盖，1:存在低质量网络覆盖,2:存在高质量网络覆盖 */
    VOS_UINT8                           ucRatType;         /* 网络的接入技术 */
    VOS_UINT8                           ucReserve;
}NAS_ERR_LOG_RAT_SEARCH_INFO_STRU;
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;
    VOS_UINT32                          ulSearchResult;                         /*选网结果*/
    VOS_UINT32                          ulCampFlg;                              /*当前是否驻留标志,VOS_TRUE:已驻留,VOS_FALSE:未驻留*/
    VOS_UINT32                          ulRatNum;                               /*有覆盖的接入技术个数*/
    NAS_ERR_LOG_RAT_SEARCH_INFO_STRU    astSearchRatInfo[NAS_ERR_LOG_MML_MAX_RAT_NUM];  /*保存不同接入技术的搜索信息*/
}NAS_ERR_LOG_SEARCH_NETWORK_RESULT_EVENT_STRU;


typedef struct
{
    VOS_UINT8                           ucIsUser;
    VOS_UINT8                           ucDiscCallFlag;
    VOS_UINT8                           aucReserved[2];
}NAS_ERR_LOG_MN_CALL_DISC_DIR_STRU;


typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;
    VOS_UINT8                           ucCallId;                               /* call id */
    NAS_ERR_LOG_CALL_STATE_ENUM_U8      enCallState;                            /* Call State */
    VOS_UINT8                           aucReserved[2];                         /* 保留域 */
    NAS_ERR_LOG_MN_CALL_DISC_DIR_STRU   stDiscDir;                              /* 挂断电话的方向 */
    VOS_UINT32                          ulCcCause;                              /* 失败原因值 */
}NAS_ERR_LOG_CS_CALL_FAIL_EVENT_STRU;
typedef struct
{
    VOS_UINT32                          ulFsmId;                                /* 当前状态机标识 */
    VOS_UINT32                          ulState;                                /* 当前状态 */
}NAS_ERR_LOG_APS_FSM_CTX_STRU;


typedef struct
{
    NAS_ERR_LOG_APS_FSM_CTX_STRU        stMainFsm;                              /* 主状态机 */
    NAS_ERR_LOG_APS_FSM_CTX_STRU        stSubFsm;                               /* 子状态机 */
    VOS_UINT8                           ucPdpId;                                /* 当前状态机所对应的PDP ID */
    VOS_UINT8                           aucReserve[3];
}NAS_ERR_LOG_APS_PDP_ENTITY_FSM_INFO_STRU;
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                      stHeader;
    NAS_MNTN_USIM_INFO_STRU                     stUsimInfo;                     /* 卡信息 */
    NAS_MNTN_POSITION_INFO_STRU                 stPositionInfo;                 /* 位置信息 */
    VOS_UINT32                                  ulPsRegState;                   /* PS注册状态 */
    NAS_ERR_LOG_APS_PDP_ENTITY_FSM_INFO_STRU    stApsFsmInfo;                   /* 状态机信息 */
    VOS_UINT32                                  ulPsCause;                      /* 失败原因 */
}NAS_ERR_LOG_PS_CALL_FAIL_EVENT_STRU;
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                                  stHeader;
    NAS_MNTN_USIM_INFO_STRU                                 stUsimInfo;
    NAS_MNTN_POSITION_INFO_STRU                             stPositionInfo;
    NAS_MNTN_SMS_MO_INFO_STRU                               stMoFail;
    NAS_MNTN_SEND_DOMAIN_STATUS_ENUM_UINT32                 enCsStatus;
    NAS_MNTN_SEND_DOMAIN_STATUS_ENUM_UINT32                 enPsStatus;
}NAS_ERR_LOG_SMS_MO_FAIL_EVENT_STRU;


typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    VOS_UINT32                              ulOptState;                         /* 操作类型 */
    VOS_UINT32                              ulCasue;                            /* 操作结果 */
    VOS_UINT32                              ulName;                             /* 消息名称 */
}NAS_ERR_LOG_VC_FAILURE_EVENT_STRU;
typedef struct
{
    OM_FTM_HEADER_STRU                      stHeader;
    VOS_UINT8                               aucTmsi[NAS_FTM_MAX_TMSI_LEN];      /* tmsi的内容 */
}NAS_FTM_TMSI_RPT_EVENT_STRU;
typedef struct
{
    OM_FTM_HEADER_STRU                      stHeader;
    VOS_UINT8                               aucPtmsi[NAS_FTM_MAX_PTMSI_LEN];    /* ptmsi的内容 */
}NAS_FTM_PTMSI_RPT_EVENT_STRU;
typedef struct
{
    NAS_ERR_LOG_ALM_ID_ENUM_U16             enAlmID;        /* 异常模块ID */
    VOS_UINT16                              usLogLevel;     /* 上报log等级 */
}NAS_ERR_LOG_ALM_LEVEL_STRU;


typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;
    VOS_UINT8                           aucContent[4];                          /* 消息内容 */
}NAS_ERR_LOG_MNTN_PUT_BUF_INFO_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


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

#endif /* end of NasErrorLog.h */


