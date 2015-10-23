/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : LnasErrLogInterface.h
  Description     : LnasErrLogInterface.h header file
  History         :
     1.HANLUFENG 41410       2013-8-27     Draft Enact
     2.
******************************************************************************/

#ifndef __LNASERRLOGINTERFACE_H__
#define __LNASERRLOGINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include    "omerrorlog.h"
#include    "LnasFtmInterface.h"

#pragma  pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/
#define LNAS_OM_ERRLOG_ID_ESM_REJ_CN_CAUSE     (400)
#define LNAS_OM_ERRLOG_ID_EMM_FAIL_CAUSE       (600)
#define LNAS_OM_ERRLOG_ID_EMM_REJ_CN_CAUSE     (800)
#define LNAS_OM_ERRLOG_MAX_NUM                 (4)


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
enum LNAS_OM_ERRLOG_ID_ENUM
{
    /*ESM模块失败拒绝*/
    LNAS_ESM_CAUSE_OPERATOR_DETERMINE_BARRING = LNAS_OM_ERRLOG_ID_ESM_REJ_CN_CAUSE, /* 网侧拒绝，提示操作被阻止 */
    LNAS_ESM_CAUSE_INSUFFICIENT_RESOURCES,             /* 网侧拒绝,提示资源不足        */
    LNAS_ESM_CAUSE_UNKNOWN_OR_MISSING_APN,             /* 网侧拒绝,提示APN无法识别     */
    LNAS_ESM_CAUSE_UNKNOWN_PDN_TYPE,                   /* 网侧拒绝，提示PDN类型无法识别*/
    LNAS_ESM_CAUSE_USER_AUTH_FAIL,                     /* 网侧拒绝，提示用户鉴权失败   */
    LNAS_ESM_CAUSE_REQ_REJ_BY_SGW_OR_PGW,              /* 网侧拒绝，提示SGW或者PGW拒绝用户请求 */
    LNAS_ESM_CAUSE_REQ_REJ_UNSPECITY,                  /* 网侧拒绝，原因不明 */
    LNAS_ESM_CAUSE_SERVICE_OPTION_NOT_SUPPORT,         /* 网侧拒绝，提示请求选项不支持 */
    LNAS_ESM_CAUSE_REQ_SERVICE_NOT_SUBSCRIBED,         /* 网侧拒绝，提示请求选项没有签约 */
    LNAS_ESM_CAUSE_SERVICE_OPTION_TEMP_OUT_ORDER,      /* 网侧拒绝，提示服务选项临时性不支持*/
    LNAS_ESM_CAUSE_PTI_ALREADY_IN_USED,                /* 网侧拒绝，提示PTI已使用*/
    LNAS_ESM_CAUSE_REGULAR_DEACTIVATION,               /* 网侧拒绝，提示承载去激活*/
    LNAS_ESM_CAUSE_EPS_QOS_NOT_ACCEPT,                 /* 网侧拒绝，提示Qos未被接受*/
    LNAS_ESM_CAUSE_NETWORK_FAILURE,                    /* 网侧拒绝，提示网络错误*/
    LNAS_ESM_CAUSE_REACTIVATION_REQUESTED,             /* 网侧拒绝，提示需要重新激活*/
    LNAS_ESM_CAUSE_SEMANTIC_ERR_IN_TFT,                /* 网侧拒绝，提示TFT语义错误*/
    LNAS_ESM_CAUSE_SYNTACTIC_ERR_IN_TFT,               /* 网侧拒绝，提示TFT语法错误*/
    LNAS_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY,        /* 网侧拒绝，提示非法的承载标识*/
    LNAS_ESM_CAUSE_SEMANTIC_ERR_IN_PACKET_FILTER,      /* 网侧拒绝，提示PacketFilter语义错误*/
    LNAS_ESM_CAUSE_SYNCTACTIC_ERR_IN_PACKET_FILTER,    /* 网侧拒绝，提示PacketFilter语法错误*/
    LNAS_ESM_CAUSE_BEARER_WITHOUT_TFT_ACT,             /* 网侧拒绝，提示激活的上下文没有TFT的承载*/
    LNAS_ESM_CAUSE_PTI_MISMATCH,                       /* 网侧拒绝，提示PTI不匹配*/
    LNAS_ESM_CAUSE_LAST_PDN_DISCONN_NOT_ALLOWED,       /* 网侧拒绝，提示最后的PDN不允许释放*/
    LNAS_ESM_CAUSE_PDNTYPE_IPV4_ONLY_ALLOWED,          /* 网侧拒绝，提示PDN类型只允许IpV4*/
    LNAS_ESM_CAUSE_PDNTYPE_IPV6_ONLY_ALLOWED,          /* 网侧拒绝，提示PDN类型只允许IpV6*/
    LNAS_ESM_CAUSE_SINGLE_ADDR_BEARER_ONLY_ALLOWED,    /* 网侧拒绝，提示PDN只允许单个IP地址*/
    LNAS_ESM_CAUSE_ESM_INFORMATION_NOT_RECEIVED,       /* 网侧拒绝，提示ESM Info消息没有收到*/
    LNAS_ESM_CAUSE_PDN_CONNECTION_DOES_NOT_EXIST,      /* 网侧拒绝，提示PDN链接不存在*/
    LNAS_ESM_CAUSE_SAME_APN_MULTI_PDN_CONNECTION_NOT_ALLOWED, /* 网侧拒绝，提示多PDN不允许对应同一个APN*/
    LNAS_ESM_CAUSE_COLLISION_WITH_NETWORK_INITIATED_REQUEST,  /* 网侧拒绝，提示与网络触发的请求冲突*/
    LNAS_ESM_CAUSE_UNSUPPORTED_QCI_VALUE,              /* 网侧拒绝，提示QCI的值无法支持*/
    LNAS_ESM_CAUSE_INVALID_PTI_VALUE,                  /* 网侧拒绝，提示PTI的值不可用*/
    LNAS_ESM_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE,     /* 网侧拒绝，提示消息语义不正确*/
    LNAS_ESM_CAUSE_INVALID_MANDATORY_INFORMATION,      /* 网侧拒绝，提示必选信元不正确*/
    LNAS_ESM_CAUSE_MESSAGE_TYPE_NON_EXIST_OR_NOT_IMPLEMENTED, /* 网侧拒绝，提示消息类型不存在或不可用*/
    LNAS_ESM_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STA, /* 网侧拒绝，提示消息类型与协议不一致*/
    LNAS_ESM_CAUSE_INFO_ELEMENT_NON_EXIST_OR_NOT_IMPLEMENTED, /* 网侧拒绝，提示信元不存在或不可用*/
    LNAS_ESM_CAUSE_CONDITIONAL_IE_ERROR,               /* 网侧拒绝，提示条件信元错误*/
    LNAS_ESM_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_PROT_STA, /* 网侧拒绝，提示消息与协议规定不一致*/
    LNAS_ESM_CAUSE_PROTOCOL_ERROR,                     /* 网侧拒绝，提示不指定的协议错误*/
    LNAS_ESM_CAUSE_APN_RESTRICTION_VAL_INCOMPATIBLE_WITH_ACT_BEARER, /* 网侧拒绝，提示apn约束条件与激活承载上下文不符*/

    /*EMM模块失败原因*/
    LNAS_OM_ERRLOG_ID_ATTACH_FAIL_NO_IMSI    = LNAS_OM_ERRLOG_ID_EMM_FAIL_CAUSE, /* USIM卡不存在*/
    LNAS_OM_ERRLOG_ID_ATTACH_FAIL_ACCESS_BAR,

    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_ILLEGAL_UE = LNAS_OM_ERRLOG_ID_EMM_REJ_CN_CAUSE, /* 网侧拒绝,提示非法UE*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_ILLEGAL_ME,                                      /* 网侧拒绝,提示非法ME*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_EPS_SERV_NOT_ALLOW,                              /* 网侧拒绝,提示EPS业务不允许*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW,             /* 网侧拒绝,提示EPS和非EPS业务不允许*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_PLMN_NOT_ALLOW,                                  /* 网侧拒绝,提示PLMN不可用*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_TA_NOT_ALLOW,                                    /* 网侧拒绝,提示当前TA不可用*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_ROAM_NOT_ALLOW,                                  /* 网侧拒绝,提示当前TA中不能漫游*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN,                      /* 网络拒绝，提示当前PLMN中EPS业务不允许*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_NO_SUITABL_CELL,                                 /* 网络拒绝，提示当前TA中没有可用的小区*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_ESM_FAILURE,                                     /* 网络拒绝，提示ESM错误*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG,                     /* 网络拒绝，提示当前用户没有CSG授权*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_SEMANTICALLY_INCORRECT_MSG,                      /* 网络拒绝，提示消息语法不正确*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_INVALID_MANDATORY_INFORMATION,                   /* 网络拒绝，提示必选信元非法*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_MSG_TYPE_NON_EXIST_OR_IMPLEMENT,                 /* 网络拒绝，提示不存在或不支持的消息*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_INFO_ELEMENT_NON_EXIST_OR_NOT_IMPLEMENT,         /* 网络拒绝，提示消息信元不存在或不支持*/
    LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_PROTOCOL_ERROR,                                  /* 网络拒绝，提示协议错误*/
    LNAS_OM_ERRLOG_ID_CN_CAUSE_OTHERS                                               /* 网络以其他原因拒绝*/
};
typedef VOS_UINT16  LNAS_OM_ERRLOG_ID_ENUM_UINT16;


enum    LMM_OM_ERR_ID_ENUM
{
    LNAS_OM_ERR_ID_LMM_APP_SUCC             = 1,
    LNAS_OM_ERR_ID_LMM_NULL_PTR             ,
    LNAS_OM_ERR_ID_LMM_TIMER_EXPIRE,
    LNAS_OM_ERR_ID_LMM_INPUT_PARA_ERR,  /* 层间消息入口参数错误 *//* 错误点较多，需要检查各NV项配置，APP入口， */
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_ATTACH_TYPE_NOT_COMPATIBLE_WITH_UE_TYPE,  /* ATTACH REQ的类型与UE设置的类型不兼容，无法发起ATTACH *//* ATTACH REQ类型应为PS类型 */
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_PLMN_SEARCHING,   /* ATTACH 失败，当前没有小区驻留，正在搜小区 */ /* 将UE移动到信号较好的地方 */
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_T3411_IS_RUNNING, /* ATTEMPINT TO ATTACH状态下，TIMER 3411正在运行，等3411超时后，会自动发起ATTACH，不必由APP
                                                    的ATTACH REQ触发ATTACH流程； */  /*UE稍后将自动尝试ATTACH，不必手动参与*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_NO_SUITABLE_CELL, /* 当前小区无法提供正常服务， */ /* 将UE移动到其他小区，或重新启动 */
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_NO_IMSI,/* 无IMSI */ /* 检查是否有卡，卡是否正常；尝试重新插拔卡和重新启动UE */
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_NO_CELL, /* 无小区 */ /* 将UE移动到信号较好的地方 */
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_ATTEMPTING_TO_UPDATE, /*正在试图更新状态*/ /*稍后重试*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_LIMITED_SERVICE,/*限制服务状态 */ /*稍后重试*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_ACCESS_BAR,/*接入被BAR*/ /*稍后重试*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_WAIT_CN_DETACH_CNF,/*当前状态WAIT_CN_DETACH_CNF,不能ATTACH*//*稍后重试*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_WAIT_CN_TAU_CNF,/*当前状态WAIT_CN_TAU_CNF,不能ATTACH*//*稍后重试*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_WAIT_CN_SER_CNF,/*当前状态WAIT_CN_TAU_CNF,不能ATTACH*//*稍后重试*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_AUTH_FAIL,/*鉴权失败*/ /*检查USIM卡是否有效，并重新开机*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_ESM_FAILURE,/*ESM有错*/ /*检查USIM卡是否有效，并重新开机*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_MT_DETACH_FAILURE,/*收到网络发起的DETACH*/ /*重新开机*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_MO_DETACH_FAILURE,/*ATTACH过程中，UE发起的DETACH*/ /*重新开机*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_T3402_IS_RUNNING,/*TIMER 3402正在运行，*/ /*稍后重试*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_FORBID_PLMN,/*FORBID_PLMN*/ /*检查USIM卡是否有效，或移动位置后重新尝试*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_FORBID_PLMN_FOR_GPRS,/*FORBID_PLMN_FOR_GPRS*/ /*检查USIM卡是否开通GPRS业务，或移动位置后重新尝试*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_FORBID_TA_FOR_ROAMING,/*FORBID_TA_FOR_ROAMING*/ /*检查USIM卡是否开通当前TA区的漫游业务，*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_FORBID_TA_FOR_RPOS,/*FORBID_TA_FOR_REGINAL PROVISION OF SERVICE*/ /*检查USIM卡是否有效，重新开机*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_PS_ATT_NOT_ALLOW,/*PS_ATT_NOT_ALLOW*/ /*检查USIM卡是否有效，重新开机*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL_LOCAL_ESM_FAILURE,/*LOCAL_ESM_FAILURE*/ /*重新开机*/
    LNAS_OM_ERR_ID_LMM_ATTACH_FAIL,/**/ /**/
    LNAS_OM_ERR_ID_LMM_DETACH_IMSI_PAGING,/* IMSI_PAGING */ /*重新开机*/
    LNAS_OM_ERR_ID_LMM_DETACH_NONE_SPECIR,/* 未知原因的DETACH */ /*重新开机*/
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_IMSI_UNKNOWN_IN_HSS                        = 0x100 + NAS_OM_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_ILLEGAL_UE                                 = 0x100 + NAS_OM_LMM_CAUSE_ILLEGAL_UE,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_IMSI_UNKNOWN_IN_VLR                        = 0x100 + NAS_OM_LMM_CAUSE_IMSI_UNKNOWN_IN_VLR,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_IMEI_NOT_ACCEPTED                          = 0x100 + NAS_OM_LMM_CAUSE_IMEI_NOT_ACCEPTED,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_ILLEGAL_ME                                 = 0x100 + NAS_OM_LMM_CAUSE_ILLEGAL_ME,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_EPS_SERV_NOT_ALLOW                         = 0x100 + NAS_OM_LMM_CAUSE_EPS_SERV_NOT_ALLOW,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW        = 0x100 + NAS_OM_LMM_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_MS_ID_CANNOT_BE_DERIVED_BY_NW              = 0x100 + NAS_OM_LMM_CAUSE_MS_ID_CANNOT_BE_DERIVED_BY_NW,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_EPS_IMPLICITLY_DETACHED                    = 0x100 + NAS_OM_LMM_CAUSE_EPS_IMPLICITLY_DETACHED,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_PLMN_NOT_ALLOW                             = 0x100 + NAS_OM_LMM_CAUSE_PLMN_NOT_ALLOW,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_TA_NOT_ALLOW                               = 0x100 + NAS_OM_LMM_CAUSE_TA_NOT_ALLOW,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_ROAM_NOT_ALLOW                             = 0x100 + NAS_OM_LMM_CAUSE_ROAM_NOT_ALLOW,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN                 = 0x100 + NAS_OM_LMM_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_NO_SUITABL_CELL                            = 0x100 + NAS_OM_LMM_CAUSE_NO_SUITABL_CELL,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_MSC_TEMPORARILY_NOT_REACHABLE              = 0x100 + NAS_OM_LMM_CAUSE_MSC_TEMPORARILY_NOT_REACHABLE,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_NETWORKFAILURE                             = 0x100 + NAS_OM_LMM_CAUSE_NETWORKFAILURE,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_CS_DOMAIN_NOT_AVAILABLE                    = 0x100 + NAS_OM_LMM_CAUSE_CS_DOMAIN_NOT_AVAILABLE,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_ESM_FAILURE                                = 0x100 + NAS_OM_LMM_CAUSE_ESM_FAILURE,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_CONGESTION                                 = 0x100 + NAS_OM_LMM_CAUSE_CONGESTION,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG                = 0x100 + NAS_OM_LMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_REQUESTED_SER_OPTION_NOT_AUTHORIZED_IN_PLMN= 0x100 + NAS_OM_LMM_CAUSE_REQUESTED_SER_OPTION_NOT_AUTHORIZED_IN_PLMN,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_CS_DOMAIN_TEMP_NOT_AVAILABLE               = 0x100 + NAS_OM_LMM_CAUSE_CS_DOMAIN_TEMP_NOT_AVAILABLE,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_NO_EPS_BEARER_CONTEXT_ACTIVATED            = 0x100 + NAS_OM_LMM_CAUSE_NO_EPS_BEARER_CONTEXT_ACTIVATED,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_SEMANTICALLY_INCORRECT_MSG                 = 0x100 + NAS_OM_LMM_CAUSE_SEMANTICALLY_INCORRECT_MSG,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_INVALID_MANDATORY_INFORMATION              = 0x100 + NAS_OM_LMM_CAUSE_INVALID_MANDATORY_INFORMATION,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_MSG_TYPE_NON_EXIST_OR_IMPLEMENT            = 0x100 + NAS_OM_LMM_CAUSE_MSG_TYPE_NON_EXIST_OR_IMPLEMENT,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_INFO_ELEMENT_NON_EXIST_OR_NOT_IMPLEMENT    = 0x100 + NAS_OM_LMM_CAUSE_INFO_ELEMENT_NON_EXIST_OR_NOT_IMPLEMENT,
    LNAS_OM_ERR_ID_LMM_CN_CAUSE_MSG_NOT_COMPATIBLE_WITH_PROTOCOL_STATE     = 0x100 + NAS_OM_LMM_CAUSE_MSG_NOT_COMPATIBLE_WITH_PROTOCOL_STATE,

    LNAS_OM_ERR_ID_LMM_NONE_SPECIRY,
    LNAS_OM_ERR_ID_BUTT

};
typedef VOS_UINT32  LNAS_OM_ERR_ID_ENUM_UINT32;
typedef VOS_UINT32  LMM_OM_ERR_ID_ENUM_UINT32;
typedef VOS_UINT32  ESM_OM_ERR_ID_ENUM_UINT32;


/*****************************************************************************
 枚举名    : LMM_ERR_LOG_ATTACH_RSLT_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : ATTACH的类型
*****************************************************************************/
enum    LMM_ERRLOG_INFO_TYPE_ENUM
{
    LMM_ERR_LOG_NONE_SPECIFY       = 0,    /* 未知流程，对应的具体信息也无效 */
    LMM_ERR_LOG_ATTACH_IND         = 1,    /* ATTACH类型*/
    LMM_ERR_LOG_TAU_RESULT_IND,            /* TAU类型 */
    LMM_ERR_LOG_SERVICE_RESULT_IND,        /* SERVICE类型 */
    LMM_ERR_LOG_DETACH_IND,                /* DETACH类型 */
    LMM_ERR_LOG_INFO_TYPE_BUTT
};
typedef VOS_UINT32  LMM_ERRLOG_INFO_TYPE_ENUM_UINT32;

/*****************************************************************************
 枚举名    : LMM_ERR_LOG_ATTACH_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : UE触发的ATTACH TYPE
*****************************************************************************/
enum LMM_ERR_LOG_ATTACH_TYPE_ENUM
{
    LMM_ERR_LOG_ATT_TYPE_EPS_ONLY                = 1, /* EPS注册 */
    LMM_ERR_LOG_ATT_TYPE_COMBINED_EPS_IMSI       = 2, /* 联合注册 */
    LMM_ERR_LOG_ATT_TYPE_EPS_EMERGENCY           = 6, /* 紧急注册 */
    LMM_ERR_LOG_ATT_TYPE_BUTT
};
typedef VOS_UINT32 LMM_ERR_LOG_ATTACH_TYPE_ENUM_UINT32;

/*****************************************************************************
 枚举名    : LMM_ERR_LOG_TAU_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : UE触发的TAU类型
*****************************************************************************/
enum LMM_ERR_LOG_TAU_TYPE_ENUM
{
    LMM_ERR_LOG_TA_UPDATING                     = 0,
    LMM_ERR_LOG_COMBINED_TA_LA_UPDATING         = 1,
    LMM_ERR_LOG_COMBINED_TA_LA_WITH_IMSI        = 2,
    LMM_ERR_LOG_PS_PERIODIC_UPDATING            = 3,
    LMM_ERR_LOG_CS_PS_PERIODIC_UPDATING         = 4,
    LMM_ERR_LOG_TAU_TYPE_BUTT
};
typedef VOS_UINT32 LMM_ERR_LOG_TAU_TYPE_ENUM_UINT32;

/*****************************************************************************
 枚举名    : LMM_ERR_LOG_TAU_CN_RSLT_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 网侧响应的TAU结果类型
*****************************************************************************/
enum LMM_ERR_LOG_TAU_CN_RSLT_ENUM
{
    LMM_ERR_LOG_TA_UPDATED                              = 0,
    LMM_ERR_LOG_COMBINED_TA_LA_UPDATED                  = 1,
    LMM_ERR_LOG_TA_UPDATED_ISR_ACTIVATED                = 4,
    LMM_ERR_LOG_COMBINED_TA_LA_UPDATED_ISR_ACTIVATED    = 5,
    LMM_ERR_LOG_TAU_CN_RSLT_BUTT
};
typedef VOS_UINT32 LMM_ERR_LOG_TAU_CN_RSLT_ENUM_UINT32;

/*****************************************************************************
 枚举名    : LMM_ERR_LOG_MT_DETACH_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 网侧触发的DETACH类型
*****************************************************************************/
enum LMM_ERR_LOG_MT_DETACH_TYPE_ENUM
{
    LMM_ERR_LOG_MT_DET_REATTACH_REQUIRED            = 1,
    LMM_ERR_LOG_MT_DET_REATTACH_NOT_REQUIRED        = 2,
    LMM_ERR_LOG_MT_DET_IMSI                         = 3,
    LMM_ERR_LOG_MT_DETACH_TYPE_BUTT
};
typedef VOS_UINT32 LMM_ERR_LOG_MT_DETACH_TYPE_ENUM_UINT32;


/*****************************************************************************
 枚举名    : LMM_ERR_LOG_SERVICE_TYPE_ENUM
 协议表格  : 24301协议9.9.3.27
 ASN.1描述 :
 枚举说明  : UE触发的SERVICE类型，暂时保留
*****************************************************************************/
enum LMM_ERR_LOG_SERVICE_TYPE_ENUM
{
    LMM_ERR_LOG_SERVICE_MO_CSFB_1XCSFB              = 0,
    LMM_ERR_LOG_SERVICR_MT_CSFB_1XCSFB,
    LMM_ERR_LOG_SERVICR_MO_CSFB_1XCSFB_EMERGENCY,
    LMM_ERR_LOG_SERVICE_TYPE_BUTT
};
typedef VOS_UINT32 LMM_ERR_LOG_SERVICE_TYPE_ENUM_UINT32;


/*****************************************************************************
 枚举名    : LMM_ERR_LOG_MO_DETACH_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : UE触发的DETACH TYPE
*****************************************************************************/
enum LMM_ERR_LOG_MO_DETACH_TYPE_ENUM
{
    LMM_ERR_LOG_MO_DET_PS_ONLY                        = 1, /* 仅PS */
    LMM_ERR_LOG_MO_DET_CS_ONLY                        = 2, /* 仅CS */
    LMM_ERR_LOG_MO_DET_CS_PS                          = 3, /* CS+PS */
    LMM_ERR_LOG_MO_DETACH_TYPE_BUTT
};
typedef VOS_UINT32 LMM_ERR_LOG_MO_DETACH_TYPE_ENUM_UINT32;

/*****************************************************************************
 枚举名    : NAS_LMM_ERR_LOG_MAIN_STATE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : MM层所有并行状态机的主状态的枚举定义
*****************************************************************************/
enum    NAS_LMM_ERR_LOG_MAIN_STATE_ENUM
{
    /*=================== LMM的主状态 ===================*/
    LMM_OM_MS_NULL                 = 0x0000,
    LMM_OM_MS_DEREG                        ,
    LMM_OM_MS_REG_INIT                     ,           /*ATTACH过程中状态*/
    LMM_OM_MS_REG                          ,
    LMM_OM_MS_TAU_INIT                     ,           /*TAU过程中状态*/
    LMM_OM_MS_SER_INIT                     ,           /*SERVICE过程中状态*/
    LMM_OM_MS_DEREG_INIT                   ,           /*DETACH过程中状态*/

    LMM_OM_MS_RRC_CONN_EST_INIT            ,           /*建立RRC连接的过程*/
    LMM_OM_MS_RRC_CONN_REL_INIT            ,           /*释放RRC连接的过程*/
    LMM_OM_MS_AUTH_INIT                    ,           /*AUTH过程*/

    LMM_OM_MS_SUSPEND                      ,           /*挂起过程*/
    LMM_OM_MS_RESUME                       ,           /*解挂过程*/

    /*=================== 预留的主状态,暂不使用 ===================*/
    LMM_OM_MS_RESERVE1                     ,           /* RESERVE */
    LMM_OM_MS_RESERVE2                     ,           /* RESERVE */
    LMM_OM_MS_RESERVE3                     ,           /* RESERVE */
    LMM_OM_MS_RESERVE4                     ,           /* RESERVE */
    LMM_OM_MS_RESERVE5                     ,           /* RESERVE */
    LMM_OM_MS_RESERVE6                     ,           /* RESERVE */
    LMM_OM_MS_RESERVE7                     ,           /* RESERVE */

    /*=================== SECU 的CUR 状态 ===================*/
    LMM_OM_CUR_SECU_NOT_EXIST,   /*17*/
    LMM_OM_CUR_SECU_EXIST_NOT_ACTIVE, /*18*/
    LMM_OM_CUR_SECU_EXIST_ACTIVE, /*19*/

    NAS_LMM_ERR_LOG_MAIN_STATE_BUTT
};
typedef VOS_UINT16 NAS_LMM_ERR_LOG_MAIN_STATE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : NAS_LMM_ERR_LOG_MAIN_STATE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : MM层所有并行状态机的子状态的枚举定义
*****************************************************************************/
enum    NAS_LMM_ERR_LOG_SUB_STATE_ENUM
{

    /*=================== LMM的子状态 ===================*/

    /*========== NULL下的子状态 ===============*/
    LMM_OM_SS_NULL_WAIT_APP_START_REQ      = 0x0000,
    LMM_OM_SS_NULL_WAIT_READING_USIM               ,
    LMM_OM_SS_NULL_WAIT_MMC_START_CNF              ,
    LMM_OM_SS_NULL_WAIT_RRC_START_CNF              ,
    LMM_OM_SS_NULL_WAIT_SWITCH_OFF                 ,
    LMM_OM_SS_NULL_WAIT_MMC_STOP_CNF               ,
    LMM_OM_SS_NULL_WAIT_RRC_STOP_CNF               ,


    /*========== DEREG下的子状态 ==============*/
    LMM_OM_SS_DEREG_NORMAL_SERVICE                 ,
    LMM_OM_SS_DEREG_LIMITED_SERVICE                ,
    LMM_OM_SS_DEREG_ATTEMPTING_TO_ATTACH           ,
    LMM_OM_SS_DEREG_PLMN_SEARCH                    ,
    LMM_OM_SS_DEREG_NO_IMSI                        ,
    LMM_OM_SS_DEREG_ATTACH_NEEDED                  ,
    LMM_OM_SS_DEREG_NO_CELL_AVAILABLE              ,

    /*========== ATTACH_INIT的子状态 ==========*/
    LMM_OM_SS_ATTACH_WAIT_ESM_PDN_RSP              ,
    LMM_OM_SS_ATTACH_WAIT_CN_ATTACH_CNF            ,
    LMM_OM_SS_ATTACH_WAIT_MRRC_REL_CNF             ,
    LMM_OM_SS_ATTACH_WAIT_ESM_BEARER_CNF           ,
    LMM_OM_SS_ATTACH_WAIT_RRC_DATA_CNF             ,


    /*========== REG下的子状态 ================*/
    LMM_OM_SS_REG_NORMAL_SERVICE                   ,
    LMM_OM_SS_REG_ATTEMPTING_TO_UPDATE             ,
    LMM_OM_SS_REG_LIMITED_SERVICE                  ,
    LMM_OM_SS_REG_PLMN_SEARCH                      ,
    LMM_OM_SS_REG_WAIT_ACCESS_GRANT_IND            ,
    LMM_OM_SS_REG_NO_CELL_AVAILABLE                ,
    LMM_OM_SS_REG_ATTEMPTING_TO_UPDATE_MM          ,
    LMM_OM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF   ,


    /*========== TAU_INIT的子状态  ============*/
    LMM_OM_SS_TAU_WAIT_CN_TAU_CNF                  ,

    /*========== SER_INIT的子状态 =============*/
    LMM_OM_SS_SER_WAIT_CN_SER_CNF                  ,

    /*========== DETACH_INIT的子状态 ==========*/
    LMM_OM_SS_DETACH_WAIT_CN_DETACH_CNF            ,
    LMM_OM_SS_DETACH_WAIT_MRRC_REL_CNF             ,


    /*========== LMM_OM_MS_RRC_CONN_INIT的子状态 =*/
    LMM_OM_SS_RRC_CONN_WAIT_EST_CNF                ,
    LMM_OM_SS_RRC_CONN_WAIT_REL_CNF                ,


    /*========== AUTH过程中的子状态 ===========*/
    LMM_OM_SS_AUTH_WAIT_CN_AUTH,

    /*========== SUSPEND过程中的子状态 ========*/
    LMM_OM_SS_SUSPEND_RRCORI_WAIT_MMC_SUSPEND,
    LMM_OM_SS_SUSPEND_RRCORI_WAIT_OTHER_SUSPEND,
    LMM_OM_SS_SUSPEND_WAIT_END,
    LMM_OM_SS_SUSPEND_MMCORI_WAIT_OTHER_SUSPEND,
    LMM_OM_SS_SUSPEND_SYSCFGORI_WAIT_OTHER_SUSPEND,

    /*========== RESUME过程中的子状态 ========*/
    LMM_OM_SS_RESUME_RRCRSM_WAIT_OTHER_RESUME,
    LMM_OM_SS_RESUME_RRCORI_WAIT_SYS_INFO_IND,
    LMM_OM_SS_RESUME_MMCORI_WAIT_SYS_INFO_IND,

    /*========== GUTI     过程暂无子状态  =====*/
    /*========== IDEN     过程暂无子状态  =====*/
    /*========== SECURITY 过程暂无子状态  =====*/

    /*=================== SECU的NEW状态 ===================*/
    LMM_OM_NEW_SECU_NOT_EXIST,/*56*/
    LMM_OM_NEW_SECU_EXIST,

    LMM_OM_SS_BUTT
};
typedef VOS_UINT16  NAS_LMM_ERR_LOG_SUB_STATE_ENUM_UINT16;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
   5 STRUCT
*****************************************************************************/

/*检视意见: IE 无效的时候，IE的内容填为 0 */

/*****************************************************************************
 结构名称   :LMM_ERR_LOG_ATTACH_IND_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   :ATTACH结果数据结构
*****************************************************************************/
typedef struct
{
    LMM_ERR_LOG_ATTACH_TYPE_ENUM_UINT32    ulReqType;    /* ATTACH发起时的类型 */
}LMM_ERR_LOG_ATTACH_IND_STRU;

/*****************************************************************************
 结构名称   :LMM_ERR_LOG_TAU_RESULT_IND_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   :TAU结构数据结构
*****************************************************************************/
typedef struct
{
    LMM_ERR_LOG_TAU_CN_RSLT_ENUM_UINT32    ulCnRst;      /* 网侧响应的TAU类型*/
    LMM_ERR_LOG_TAU_TYPE_ENUM_UINT32       ulReqType;    /* TAU发起业务时的类型*/
} LMM_ERR_LOG_TAU_RESULT_IND_STRU;


/*****************************************************************************
 结构名称   :LMM_ERR_LOG_SERVICE_RESULT_IND_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   :SERVICE结果数据结构
*****************************************************************************/
typedef struct
{
    LMM_ERR_LOG_SERVICE_TYPE_ENUM_UINT32    ulReqType;   /* SERVICE结果类型 */
    VOS_UINT32                              ulRsv;       /* 保留 */
} LMM_ERR_LOG_SERVICE_RESULT_IND_STRU;

/*****************************************************************************
 结构名称   :LMM_ERR_LOG_STATE_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   :EMM状态信息数据结构
*****************************************************************************/
typedef struct
{
    NAS_LMM_ERR_LOG_MAIN_STATE_ENUM_UINT16      usLmmMainState; /* EMM主状态*/
    NAS_LMM_ERR_LOG_SUB_STATE_ENUM_UINT16       usLmmSubState;  /* EMM子状态*/
}LMM_ERR_LOG_STATE_STRU;

/*****************************************************************************
 结构名称   :LMM_ERR_LOG_DETACH_IND_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   :DETACH结果数据结构
*****************************************************************************/
typedef struct
{

    LMM_ERR_LOG_MO_DETACH_TYPE_ENUM_UINT32  ulUeReqType;        /* UE触发的DETACH类型*/
    LMM_ERR_LOG_MT_DETACH_TYPE_ENUM_UINT32  ulCnReqType;        /* 网侧触发的DETACH类型*/
}LMM_ERR_LOG_DETACH_IND_STRU;


/*****************************************************************************
 结构名称   : LMM_ERR_INFO_DETAIL_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : LMM错误信息详细结构
*****************************************************************************/
typedef struct
{
    LNAS_OM_ERRLOG_ID_ENUM_UINT16           usErrLogID;         /* 错误码编号 */
    VOS_UINT16                              usRsv;              /* 保留*/
    LMM_ERR_LOG_STATE_STRU                  stLmmState;         /* EMM状态信息*/
    LMM_ERRLOG_INFO_TYPE_ENUM_UINT32        ulInfoType;         /* 标识下面哪个结构有效 */
    LMM_ERR_LOG_ATTACH_IND_STRU             stAttachInd;        /* ATTACH结果 */
    LMM_ERR_LOG_TAU_RESULT_IND_STRU         stTauResultInd;	    /* TAU结果 */
    LMM_ERR_LOG_SERVICE_RESULT_IND_STRU     stServiceResultInd; /* SERVICE结果 */
    LMM_ERR_LOG_DETACH_IND_STRU             stDetachInd;        /* DETACH结果　*/

}LMM_ERR_INFO_DETAIL_STRU;


/*****************************************************************************
 结构名称   : ESM_OM_ERR_INFO_DETAIL_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : ESM错误信息详细结构
*****************************************************************************/
typedef struct
{
    LNAS_OM_ERRLOG_ID_ENUM_UINT16       usErrLogID;    /* 错误码编号 */
    VOS_UINT16                          usRsv;         /* 保留 */
} ESM_ERR_INFO_DETAIL_STRU;


/*****************************************************************************
 结构名称   : LMM_ALM_INFO_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : ERRLOG数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulErrlogNum;   /* 下面数组中有效ERRLOG的个数 */

    /* LMM的ERR LOG发生时，此处保存错误信息 */
    LMM_ERR_INFO_DETAIL_STRU            stLmmErrInfoDetail[LNAS_OM_ERRLOG_MAX_NUM];

}LMM_ALM_INFO_STRU;

/*****************************************************************************
 结构名称   : ESM_ALM_INFO_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : ERRLOG数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulErrlogNum;   /* 下面数组中有效ERRLOG的个数 */

    /* ESM的ERR LOG发生时，此处保存错误信息 */
    ESM_ERR_INFO_DETAIL_STRU            stEsmErrInfoDetail[LNAS_OM_ERRLOG_MAX_NUM];

}ESM_ALM_INFO_STRU;


/*****************************************************************************
 结构名称   : LMM_OM_ERR_LOG_INFO_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : LMM ERRLOG信息数据结构
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
    VOS_UINT32                          ulAlmLowSlice;/*时间戳*/
    VOS_UINT32                          ulAlmHighSlice;
    VOS_UINT32                          ulAlmLength;
    LMM_ALM_INFO_STRU                   stAlmInfo;

}LMM_OM_ERR_LOG_INFO_STRU;

/*****************************************************************************
 结构名称   : ESM_OM_ERR_LOG_INFO_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : ESM ERRLOG信息数据结构
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
    VOS_UINT32                          ulAlmLowSlice;/*时间戳*/
    VOS_UINT32                          ulAlmHighSlice;
    VOS_UINT32                          ulAlmLength;
    ESM_ALM_INFO_STRU                   stAlmInfo;

}ESM_OM_ERR_LOG_INFO_STRU;

/*****************************************************************************
 结构名称   : LMM_OM_ERR_LOG_REPORT_CNF_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : NAS子模块LMM向OM回复ERRLOG应答数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;    VOS_UINT32                          ulMsgType;    VOS_UINT32                          ulMsgSN;
    VOS_UINT32                          ulRptlen;   /* 故障内容长度,如果ulRptlen为0,aucContent内容长度也为0 */
    LMM_OM_ERR_LOG_INFO_STRU            stLmmErrlogInfo;

}LMM_OM_ERR_LOG_REPORT_CNF_STRU;

/*****************************************************************************
 结构名称   : ESM_OM_ERR_LOG_REPORT_CNF_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : NAS子模块ESM向OM回复ERRLOG应答数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;    VOS_UINT32                          ulMsgType;    VOS_UINT32                          ulMsgSN;
    VOS_UINT32                          ulRptlen;   /* 故障内容长度,如果ulRptlen为0,aucContent内容长度也为0 */
    ESM_OM_ERR_LOG_INFO_STRU            stEsmErrlogInfo;

}ESM_OM_ERR_LOG_REPORT_CNF_STRU;


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

#endif /* end of LnasErrlogInterface.h */


