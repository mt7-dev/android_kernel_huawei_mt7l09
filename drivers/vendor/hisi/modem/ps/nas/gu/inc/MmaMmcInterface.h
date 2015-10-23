
#ifndef _MMA_MMC_INTERFACE_H_
#define _MMA_MMC_INTERFACE_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "PsTypeDef.h"
#include "NasCommDef.h"
#include "TafAppMma.h"
#include "NasMmlCtx.h"
#include "NasComm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MMA_MMC_CELL_MAX_NUM            (8)                                     /* 小区最大数量 */
#define MMA_MMC_MAX_PLMN_CNT            (37)
#if (FEATURE_ON == FEATURE_LTE)
#define MMA_MMC_MAX_RAT_NUM             (3)                     /* 当前支持的接入技术个数 */
#else
#define MMA_MMC_MAX_RAT_NUM             (2)                     /* 当前支持的GU接入技术个数 */
#endif

#define MMA_MMC_CAUSE_NULL              (0)

#define MMC_MMA_NET_SCAN_MAX_FREQ_NUM   (20)

#define MMA_MMC_MAX_EQUPLMN_NUM         (18)                    /* 等效的PLMN的最大个数 */
#define MMA_MMC_TDS_ARFCN_MAX_NUM       (9)                                     /* TDD频点列表最大个数 */
#define MMA_MMC_LTE_ARFCN_MAX_NUM       (8)                                     /* LTE频点列表最大个数 */


#define MMA_MMC_REG_MAX_PLMN_NUM        (8)
#define MMA_MMC_REG_MAX_CELL_NUM        (8)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum MMA_MMC_MSG_ID_ENUM
{
    /* MMA发给MMC的消息原语 */
    ID_MMA_MMC_START_REQ                  = 0,                                  /* _H2ASN_MsgChoice MMA_MMC_START_REQ_STRU */
    ID_MMA_MMC_SIGN_REPORT_REQ            = 2,                                  /* _H2ASN_MsgChoice MMA_MMC_SIGN_REPORT_REQ_STRU */
    ID_MMA_MMC_MODE_CHANGE_REQ            = 4,                                  /* _H2ASN_MsgChoice MMA_MMC_MODE_CHANGE_REQ_STRU */
    ID_MMA_MMC_ATTACH_REQ                 = 6,                                  /* _H2ASN_MsgChoice MMA_MMC_ATTACH_REQ_STRU */
    ID_MMA_MMC_DETACH_REQ                 = 8,                                  /* _H2ASN_MsgChoice MMA_MMC_DETACH_REQ_STRU */
    ID_MMA_MMC_PLMN_LIST_REQ              = 10,                                 /* _H2ASN_MsgChoice MMA_MMC_PLMN_LIST_REQ_STRU */
    ID_MMA_MMC_PLMN_LIST_ABORT_REQ        = 12,                                 /* _H2ASN_MsgChoice MMA_MMC_PLMN_LIST_ABORT_REQ_STRU */
    ID_MMA_MMC_PLMN_USER_RESEL_REQ        = 14,                                 /* _H2ASN_MsgChoice MMA_MMC_PLMN_USER_RESEL_REQ_STRU */
    ID_MMA_MMC_PLMN_SPECIAL_REQ           = 16,                                 /* _H2ASN_MsgChoice MMA_MMC_PLMN_SPECIAL_REQ_STRU */
    ID_MMA_MMC_POWER_OFF_REQ              = 18,                                 /* _H2ASN_MsgChoice MMA_MMC_POWER_OFF_REQ_STRU */
    ID_MMA_MMC_SYS_CFG_SET_REQ            = 20,                                 /* _H2ASN_MsgChoice MMA_MMC_SYS_CFG_SET_REQ_STRU */
    ID_MMA_MMC_PLMN_SEARCH_REQ            = 22,                                 /* _H2ASN_MsgChoice MMA_MMC_PLMN_SEARCH_REQ_STRU */
    ID_MMA_MMC_SPEC_PLMN_SEARCH_ABORT_REQ = 24,                                 /* _H2ASN_MsgChoice MMA_MMC_SPEC_PLMN_SEARCH_ABORT_REQ_STRU */
    ID_MMA_MMC_OM_MAINTAIN_INFO_IND       = 26,                                 /* _H2ASN_MsgChoice MMA_MMC_OM_MAINTAIN_INFO_IND_STRU */


    /* 用户通过CPOL命令更新UPLMN信息通知给MMC */
    ID_MMA_MMC_UPDATE_UPLMN_NTF           = 28,                                 /* _H2ASN_MsgChoice MMA_MMC_UPDATE_UPLMN_NTF_STRU */

    ID_MMA_MMC_EOPLMN_SET_REQ             = 30,                                 /* _H2ASN_MsgChoice MMA_MMC_EOPLMN_SET_REQ_STRU */
	
    ID_MMA_MMC_NET_SCAN_REQ               = 32,
    ID_MMA_MMC_ABORT_NET_SCAN_REQ         = 34,

    ID_MMA_MMC_OTHER_MODEM_INFO_NOTIFY    = 36,                                 /* _H2ASN_MsgChoice MMA_MMC_OTHER_MODEM_INFO_NOTIFY_STRU */

    ID_MMA_MMC_NCELL_INFO_NOTIFY  = 38,                                    /* _H2ASN_MsgChoice MMA_MMC_NCELL_INFO_NOTIFY_STRU */
   
    ID_MMA_MMC_PS_TRANSFER_IND    = 40,
    ID_MMA_MMC_IMS_VOICE_CAP_IND          = 42,                                 /* _H2ASN_MsgChoice MMA_MMC_IMS_VOICE_CAP_IND_STRU */

    ID_MMA_MMC_ACQ_REQ                    = 44,
    ID_MMA_MMC_REG_REQ                    = 46,
    ID_MMA_MMC_POWER_SAVE_REQ             = 48,

    /* MMC发送给MMA的消息原语*/
    ID_MMC_MMA_START_CNF                  = 1,                                  /* _H2ASN_MsgChoice MMC_MMA_START_CNF_STRU */
    ID_MMC_MMA_SYS_INFO_IND               = 3,                                  /* _H2ASN_MsgChoice MMC_MMA_SYS_INFO_IND_STRU */
    ID_MMC_MMA_SERVICE_STATUS_IND         = 5,                                  /* _H2ASN_MsgChoice MMC_MMA_SERVICE_STATUS_IND_STRU */
    ID_MMC_MMA_MM_INFO_IND                = 7,                                  /* _H2ASN_MsgChoice MMC_MMA_MM_INFO_IND_STRU */
    ID_MMC_MMA_ATTACH_CNF                 = 9,                                  /* _H2ASN_MsgChoice MMC_MMA_ATTACH_CNF_STRU */
    ID_MMC_MMA_DETACH_CNF                 = 11,                                 /* _H2ASN_MsgChoice MMC_MMA_DETACH_CNF_STRU */
    ID_MMC_MMA_DETACH_IND                 = 13,                                 /* _H2ASN_MsgChoice MMC_MMA_DETACH_IND_STRU */
    ID_MMC_MMA_PLMN_LIST_CNF              = 15,                                 /* _H2ASN_MsgChoice MMC_MMA_PLMN_LIST_CNF_STRU */
    ID_MMC_MMA_PLMN_LIST_REJ              = 17,                                 /* _H2ASN_MsgChoice MMC_MMA_PLMN_LIST_REJ_STRU */
    ID_MMC_MMA_COVERAGE_AREA_IND          = 19,                                 /* _H2ASN_MsgChoice MMC_MMA_COVERAGE_AREA_IND_STRU */
    ID_MMC_MMA_POWER_OFF_CNF              = 21,                                 /* _H2ASN_MsgChoice MMC_MMA_POWER_OFF_REQ_STRU */
    ID_MMC_MMA_RSSI_IND                   = 23,                                 /* _H2ASN_MsgChoice MMC_MMA_RSSI_IND_STRU */
    ID_MMC_MMA_PLMN_SPECIAL_SEL_CNF       = 25,                                 /* _H2ASN_MsgChoice MMC_MMA_PLMN_SPECAIL_SEL_CNF_STRU */
    ID_MMC_MMA_DATATRAN_ATTRI_IND         = 27,                                 /* _H2ASN_MsgChoice MMC_MMA_DATATRAN_ATTRI_IND_STRU */
    ID_MMC_MMA_SYS_CFG_CNF                = 29,                                 /* _H2ASN_MsgChoice MMC_MMA_SYS_CFG_SET_CNF_STRU */
    ID_MMC_MMA_PLMN_SELECTION_RLST_IND    = 31,                                 /* _H2ASN_MsgChoice MMC_MMA_PLMN_SELECTION_RLST_IND_STRU */
    ID_MMC_MMA_PLMN_LIST_ABORT_CNF        = 33,                                 /* _H2ASN_MsgChoice MMC_MMA_PLMN_LIST_ABORT_CNF_STRU */
    ID_MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF = 35,                                 /* _H2ASN_MsgChoice MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF_STRU */
    ID_MMC_MMA_UMTS_CIPHER_INFO_IND       = 37,                                 /* _H2ASN_MsgChoice MMC_MMA_UMTS_CIPHER_INFO_IND_STRU */
    ID_MMC_MMA_GPRS_CIPHER_INFO_IND       = 39,                                 /* _H2ASN_MsgChoice MMC_MMA_GPRS_CIPHER_INFO_IND_STRU */
    ID_MMC_MMA_PLMN_SPECIAL_SEL_REJ       = 41,                                 /* _H2ASN_MsgChoice MMC_MMA_PLMN_SPECIAL_SEL_REJ_STRU */
    ID_MMC_MMA_AC_INFO_CHANGE_IND         = 43,                                 /* _H2ASN_MsgChoice MMC_MMA_AC_INFO_CHANGE_IND_STRU */
    ID_MMC_MMA_PLMN_RESEL_CNF             = 45,                                 /* _H2ASN_MsgChoice MMC_MMA_PLMN_RESEL_CNF_STRU */
    ID_MMC_MMA_REG_RESULT_IND             = 47,

    ID_MMC_MMA_PLMN_SELE_START_IND        = 49, 

    ID_MMC_MMA_EOPLMN_SET_CNF             = 51,
    ID_MMC_MMA_USIM_AUTH_FAIL_IND         = 53,

    ID_MMC_MMA_NET_SCAN_CNF               = 55,
    ID_MMC_MMA_ABORT_NET_SCAN_CNF         = 57,
	    
    ID_MMC_MMA_NETWORK_CAPABILITY_INFO_IND= 59,                                 /* _H2ASN_MsgChoice ID_MMC_MMA_NETWORK_CAPABILITY_INFO_IND */
    ID_MMC_MMA_CAMP_ON_IND                = 61,                                  /* _H2ASN_MsgChoice MMC_MMA_CAMP_ON_IND_STRU_STRU */

    ID_MMC_MMA_EPLMN_INFO_IND             = 63,                                 /* _H2ASN_MsgChoice MMC_MMA_EPLMN_INFO_IND_STRU */

    ID_MMC_MMA_CS_SERVICE_CONN_STATUS_IND = 65,                                 /* _H2ASN_MsgChoice MMC_MMA_CS_SERVICE_CONN_STATUS_IND_STRU */

    ID_MMC_MMA_SRV_REJ_IND                = 67,                                    /* _H2ASN_MsgChoice MMC_MMA_SERV_REJ_IND_STRU */
    ID_MMC_MMA_ACQ_CNF                    = 69,
    ID_MMC_MMA_REG_CNF                    = 71,
    ID_MMC_MMA_POWER_SAVE_CNF             = 73,
    ID_MMC_MMA_ACQ_IND                    = 75,


    ID_MMA_MMC_MSG_ID_ENUM_BUTT
};
typedef VOS_UINT32 MMA_MMC_MSG_ID_ENUM_UINT32;


enum MMA_MMC_CARD_STATUS_ENUM
{
    MMA_MMC_CARD_STATUS_SIM_PRESENT      = 0,                                   /* sim present */
    MMA_MMC_CARD_STATUS_USIM_PRESENT     = 1,                                   /* usim present */
    MMA_MMC_CARD_STATUS_ABSENT           = 2,                                   /* card absent */
    MMA_MMC_CARD_STATUS_BUTT
};
typedef VOS_UINT8 MMA_MMC_CARD_STATUS_ENUM_UINT8;
enum MMA_MMC_PLMN_STATUS_ENUM
{
    MMA_MMC_PLMN_STATUS_UNKNOW           = 0,                                    /* 当前网络状态未知 */
    MMA_MMC_PLMN_STATUS_AVAILIBLE        = 1,                                    /* 当前网络可用 */
    MMA_MMC_PLMN_STATUS_CURRENT          = 2,                                    /* 当前网络已注册 */
    MMA_MMC_PLMN_STATUS_FORBIDDEN        = 3,                                    /* 当前网络禁止 */
    MMA_MMC_PLMN_STATUS_BUTT
};
typedef VOS_UINT8 MMA_MMC_PLMN_STATUS_ENUM_UINT8;


typedef NAS_MML_NET_RAT_TYPE_ENUM_UINT8 MMA_MMC_NET_RAT_TYPE_ENUM_UINT8;


typedef NAS_MML_REG_STATUS_ENUM_UINT8  MMA_MMC_REG_STATE_ENUM_UINT8;


enum MMA_MMC_POWER_OFF_CAUSE_ENUM
{

    MMA_MMC_POWER_OFF_CAUSE_SIM_REMOVE,
    MMA_MMC_POWER_OFF_CAUSE_NORMAL,
    MMA_MMC_POWER_OFF_CAUSE_BUTT
};
typedef VOS_UINT32 MMA_MMC_POWER_OFF_CAUSE_ENUM_UINT32;


enum MMA_MMC_ROAM_CAPABILITY_ENUM
{
    MMA_MMC_ROAM_NATION_ON_INTERNATION_ON                 = 0,
    MMA_MMC_ROAM_NATION_ON_INTERNATION_OFF                = 1,
    MMA_MMC_ROAM_NATION_OFF_INTERNATION_ON                = 2,
    MMA_MMC_ROAM_NATION_OFF_INTERNATION_OFF               = 3,
    MMA_MMC_ROAM_UNCHANGE                                 = 4,
    MMA_MMC_ROAM_BUTT
};
typedef VOS_UINT8 MMA_MMC_ROAM_CAPABILITY_ENUM_UINT8;


enum MMA_MMC_ROAM_FEATURE_ENUM
{
    MMA_MMC_ROAM_FEATURE_OFF                = 0,
    MMA_MMC_ROAM_FEATURE_ON                 = 1,
    MMA_MMC_ROAM_FEATURE_BUTT
};
typedef VOS_UINT8 MMA_MMC_ROAM_FEATURE_ENUM_U8;


enum MMA_MMC_PLMN_SELECTION_RESULT_ENUM
{
    MMA_MMC_PLMN_SELECTION_RESULT_NORMAL_SERVICE    = 0,                        /* 正常服务 */
    MMA_MMC_PLMN_SELECTION_RESULT_LIMITED_SERVICE   = 1,                        /* 限制服务 */
    MMA_MMC_PLMN_SELECTION_RESULT_NO_SERVICE        = 2,                        /* 无服务 */
    MMA_MMC_PLMN_SELECTION_RESULT_INTERRUPT         = 3,                        /* 搜网被中断 */
    MMA_MMC_PLMN_SELECTION_RESULT_BUTT
};
typedef VOS_UINT32 MMA_MMC_PLMN_SELECTION_RESULT_ENUM_UINT32;


enum MMA_MMC_PLMN_SELECTION_START_FLAG_ENUM
{
    MMA_MMC_PLMN_SELECTION_START    = 0,                        /* 搜网开始 */
    MMA_MMC_PLMN_SELECTION_BUTT
};
typedef VOS_UINT32 MMA_MMC_PLMN_SELECTION_START_FLAG_ENUM_UINT32;
enum MMA_MMC_SYS_CFG_SET_RESULT_ENUM
{
    MMA_MMC_SYS_CFG_SET_SUCCESS                  = 0,
    MMA_MMC_SYS_CFG_SET_FAILURE_OTHER            = 1,
    MMA_MMC_SYS_CFG_SET_FAILURE_CS_SERV_EXIST    = 2,
    MMA_MMC_SYS_CFG_SET_BUTT
};
typedef VOS_UINT32 MMA_MMC_SYS_CFG_SET_RESULT_ENUM_UINT32;


enum  MMA_MMC_CELL_RESTRICTION_TYPE_ENUM
{
    MMA_MMC_CELL_RESTRICTION_TYPE_NONE                   = 0,                   /* 当前小区不限制任何业务 */
    MMA_MMC_CELL_RESTRICTION_TYPE_NORMAL_SERVICE         = 1,                   /* 当前小区限制正常业务 */
    MMA_MMC_CELL_RESTRICTION_TYPE_EMERGENCY_SERVICE      = 2,                   /* 当前小区限制紧急业务 */
    MMA_MMC_CELL_RESTRICTION_TYPE_ALL_SERVICE            = 3,                   /* 当前小区限制正常业务和紧急业务 */
    MMA_MMC_CELL_RESTRICTION_TYPE_BUTT
};
typedef VOS_UINT8   MMA_MMC_CELL_RESTRICTION_TYPE_ENUM_UINT8;


enum MMA_MMC_ATTACH_TYPE_ENUM
{
    MMA_MMC_ATTACH_TYPE_GPRS          = 1,
    MMA_MMC_ATTACH_TYPE_IMSI          = 2,
    MMA_MMC_ATTACH_TYPE_GPRS_IMSI     = 3,
    MMA_MMC_ATTACH_TYPE_BUTT
};
typedef VOS_UINT32 MMA_MMC_ATTACH_TYPE_ENUM_UINT32;


enum MMA_MMC_DETACH_TYPE_ENUM
{
    MMA_MMC_DETACH_NULL               = 0,
    MMA_MMC_DETACH_TYPE_GPRS          = 1,
    MMA_MMC_DETACH_TYPE_IMSI          = 2,
    MMA_MMC_DETACH_TYPE_GPRS_IMSI     = 3,
    MMA_MMC_DETACH_TYPE_BUTT
};
typedef VOS_UINT32 MMA_MMC_DETACH_TYPE_ENUM_UINT32;


enum MMA_MMC_PLMN_CFG_MODE_ENUM
{
    MMA_MMC_PLMN_CFG_MODE_USER_SEL           = 1,                               /* 通知用户选定的PLMN信息 */
    MMA_MMC_PLMN_CFG_MODE_USER_RESEL         = 2,                               /* 触发PLMN的User reselection */
    MMA_MMC_PLMN_CFG_MODE_BUTT
};
typedef VOS_UINT32 MMA_MMC_PLMN_CFG_MODE_ENUM_UINT32;


enum MMA_MMC_PLMN_SEL_MODE_ENUM
{
    MMA_MMC_PLMN_SEL_MODE_AUTO           = 0,                                   /* 自动搜网模式 */
    MMA_MMC_PLMN_SEL_MODE_MANUAL         = 1,                                   /* 手动搜网模式 */
    MMA_MMC_PLMN_SEL_MODE_BUTT
};
typedef VOS_UINT32 MMA_MMC_PLMN_SEL_MODE_ENUM_UINT32;


enum MMA_MMC_COVERAGE_AREA_CHANGE_MODE_ENUM
{
    MMA_MMC_COVERAGE_AREA_CHANGE_MODE_LOST          = 0,                        /* 出覆盖区 */
    MMA_MMC_COVERAGE_AREA_CHANGE_MODE_ENTER         = 1,                        /* 进覆盖区 */
    MMA_MMC_COVERAGE_AREA_CHANGE_MODE_BUTT
};
typedef VOS_UINT32 MMA_MMC_COVERAGE_AREA_CHANGE_MODE_ENUM_UINT32;


enum MMA_MMC_MS_MODE_ENUM
{
    MMA_MMC_MS_MODE_CC                  = 0,                                    /* cs mode */
    MMA_MMC_MS_MODE_CG                  = 1,                                    /* ps mode */
    MMA_MMC_MS_MODE_A                   = 2,                                    /* cs ps mode */
    MMA_MMC_MS_MODE_B                   = 4,                                    /* mode B */
    MMA_MMC_MS_MODE_NULL                = 5,                                    /* mode null */
    MMA_MMC_MS_MODE_BUTT
};
typedef VOS_UINT32 MMA_MMC_MS_MODE_ENUM_UINT32;
enum MMA_MMC_SERVICE_STATUS_ENUM
{
    MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE           = 0,                        /* 正常服务 */
    MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE          = 1,                        /* 限制服务 */
    MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE_REGION   = 2,                        /* 有限制区域服务 */
    MMA_MMC_SERVICE_STATUS_NO_IMSI                  = 3,                        /* 无imsi */
    MMA_MMC_SERVICE_STATUS_NO_SERVICE               = 4,                        /* 无服务 */
    MMA_MMC_SERVICE_STATUS_NO_CHANGE                = 5,
    MMA_MMC_SERVICE_STATUS_DEEP_SLEEP               = 6,
    MMA_MMC_SERVICE_STATUS_BUTT
};
typedef VOS_UINT32 MMA_MMC_SERVICE_STATUS_ENUM_UINT32;
enum MMA_MMC_SRVDOMAIN_ENUM
{
    MMA_MMC_SRVDOMAIN_NO_DOMAIN               = 0,
    MMA_MMC_SRVDOMAIN_CS                      = 1,
    MMA_MMC_SRVDOMAIN_PS                      = 2,
    MMA_MMC_SRVDOMAIN_CS_PS                   = 3,
    MMA_MMC_SRVDOMAIN_NO_CHANGE               = 4,
    MMA_MMC_SRVDOMAIN_NOT_REG_PLMN_SEARCHING  = 5,
    MMA_MMC_SRVDOMAIN_BUTT
};
typedef VOS_UINT32 MMA_MMC_SRVDOMAIN_ENUM_UINT32;




enum MMA_MMC_DATATRAN_ATTRI_ENUM
{
    MMA_MMC_DATATRAN_ATTRI_PDP_ACT          = 0,
    MMA_MMC_DATATRAN_ATTRI_PDP_DEACT        = 1,
    MMA_MMC_DATATRAN_ATTRI_R99_ONLY         = 2,
    MMA_MMC_DATATRAN_ATTRI_HSDPA            = 3,
    MMA_MMC_DATATRAN_ATTRI_HSUPA            = 4,
    MMA_MMC_DATATRAN_ATTRI_HSDPA_AND_HSUPA  = 5,
    MMA_MMC_DATATRAN_ATTRI_HSPA_PLUS        = 6,
    MMA_MMC_DATATRAN_ATTRI_LTE_DATA         = 7,
    MMA_MMC_DATATRAN_ATTRI_LTE_IDLE         = 8,
    MMA_MMC_DATATRAN_ATTRI_DC_HSPA_PLUS     = 9,
    MMA_MMC_DATATRAN_ATTRI_DC_MIMO          = 10,
    MMA_MMC_DATATRAN_ATTRI_BUTT
};
typedef VOS_UINT8 MMA_MMC_DATATRAN_ATTRI_ENUM_UINT8;


enum MMA_MMC_ALLOWED_REG_DOMAIN_ENUM
{
    MMA_MMC_ALLOWED_REG_DOMAIN_NULL     = 0,                                    /* 不注册cs和ps */
    MMA_MMC_ALLOWED_REG_DOMAIN_PS       = 1,                                    /* 只注册PS */
    MMA_MMC_ALLOWED_REG_DOMAIN_CS       = 2,                                    /* 只注册CS */
    MMA_MMC_ALLOWED_REG_DOMAIN_CS_PS    = 3,                                    /* 注册CS+PS */
    MMA_MMC_ALLOWED_REG_DOMAIN_BUTT
};
typedef VOS_UINT8 MMA_MMC_ALLOWED_REG_DOMAIN_ENUM_UINT8;


enum MMA_MMC_USER_PLMN_SEARCH_RESULT_ENUM
{
    MMA_MMC_USER_PLMN_SEARCH_RESULT_SUCCESS = 0,                                /* 成功 */
    MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL    = 1,                                /* 失败 */
    MMA_MMC_USER_PLMN_SEARCH_RESULT_REJ     = 2,
    MMA_MMC_USER_PLMN_SEARCH_RESULT_BUTT
};
typedef VOS_UINT8 MMA_MMC_USER_PLMN_SEARCH_RESULT_ENUM_UINT8;


enum MMA_MMC_NW_IMS_VOICE_CAP_ENUM
{
    MMA_MMC_NW_IMS_VOICE_NOT_SUPPORTED    = 0,
    MMA_MMC_NW_IMS_VOICE_SUPPORTED        = 1,

    MMA_MMC_NW_IMS_VOICE_BUTT
};
typedef VOS_UINT8 MMA_MMC_NW_IMS_VOICE_CAP_ENUM_UINT8;


enum MMA_MMC_NW_EMC_BS_CAP_ENUM
{
    MMA_MMC_NW_EMC_BS_NOT_SUPPORTED       = 0,
    MMA_MMC_NW_EMC_BS_SUPPORTED           = 1,

    MMA_MMC_NW_EMC_BS_BUTT
};
typedef VOS_UINT8 MMA_MMC_NW_EMC_BS_CAP_ENUM_UINT8;


enum MMA_MMC_LTE_CS_CAPBILITY_ENUM
{
    MMA_MMC_LTE_CS_CAPBILITY_NO_ADDITION_INFO  = 0,
    MMA_MMC_LTE_CS_CAPBILITY_CSFB_NOT_PREFER   = 1,
    MMA_MMC_LTE_CS_CAPBILITY_SMS_ONLY          = 2,
    MMA_MMC_LTE_CS_CAPBILITY_NOT_SUPPORTED     = 3,
    
    MMA_MMC_LTE_CS_CAPBILITY_BUTT
};
typedef VOS_UINT8 MMA_MMC_LTE_CS_CAPBILITY_ENUM_UINT8;


enum MMA_MMC_IMS_NORMAL_REG_STATUS_ENUM
{
    MMA_MMC_IMS_NORMAL_REG_STATUS_DEREG        = 0x00,
    MMA_MMC_IMS_NORMAL_REG_STATUS_DEREGING     = 0x01,
    MMA_MMC_IMS_NORMAL_REG_STATUS_REGING       = 0x02,
    MMA_MMC_IMS_NORMAL_REG_STATUS_REG          = 0x03,

    MMA_MMC_IMS_NORMAL_REG_STATUS_BUTT         = 0xFF
};
typedef VOS_UINT8 MMA_MMC_IMS_NORMAL_REG_STATUS_ENUM_UINT8;



enum MMA_MMC_REG_CONTROL_ENUM
{
    MMA_MMC_REG_CONTROL_BY_3GPP_MMC = 0,                 /* 搜索到网络后，是否注册按目前3GPP协议模块MMC控制 */
    MMA_MMC_REG_CONTROL_BY_3GPP2_CBP,                    /*搜索到网络后，是否注册由3GPP2协议CDMA Baseband Protocol控制 */
    MMA_MMC_REG_CONTROL_BUTT
};
typedef VOS_UINT8 MMA_MMC_REG_CONTROL_ENUM_UINT8;


enum MMA_MMC_3GPP2_RAT_TYPE_ENUM
{
    MMA_MMC_3GPP2_RAT_TYPE_CDMA1X = 0,
    MMA_MMC_3GPP2_RAT_TYPE_HRPD,
    MMA_MMC_3GPP2_RAT_TYPE_BUTT
};
typedef VOS_UINT8 MMA_MMC_3GPP2_RAT_TYPE_ENUM_UINT8;


enum MMA_MMC_EPS_ATTACH_REASON_ENUM
{
    MMA_MMC_EPS_ATTACH_REASON_INITIAL          = 0,
    MMA_MMC_EPS_ATTACH_REASON_HANDOVER         = 1,
    MMA_MMC_EPS_ATTACH_REASON_BUTT
};
typedef VOS_UINT32 MMA_MMC_EPS_ATTACH_REASON_ENUM_UINT32;


enum MMA_MMC_DETACH_REASON_ENUM
{
    MMA_MMC_DETACH_REASON_NORMAL           = 0,
    MMA_MMC_DETACH_REASON_3GPP2_ATTACHED   = 1,
    MMA_MMC_DETACH_REASON_BUTT
};
typedef VOS_UINT32 MMA_MMC_DETACH_REASON_ENUM_UINT32;


enum MMC_MMA_PLMN_PRIORITY_CLASS_ENUM
{
    MMC_MMA_PLMN_PRIORITY_HOME               = 0,    /* home or ehome plmn */
    MMC_MMA_PLMN_PRIORITY_PREF               = 1,    /* UPLMN or OPLMN */
    MMC_MMA_PLMN_PRIORITY_ANY                = 2,    /* Acceptable PLMN */
    MMC_MMA_PLMN_PRIORITY_BUTT
};
typedef VOS_UINT8 MMC_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8;
enum MMA_MMC_ACQ_REASON_ENUM
{
    MMA_MMC_ACQ_REASON_POWER_UP        = 0,
    MMA_MMC_ACQ_REASON_OOS             = 1,
    MMA_MMC_ACQ_REASON_HIGH_PRIORITY   = 2,
    MMA_MMC_ACQ_REASON_BUTT
};
typedef VOS_UINT32 MMA_MMC_ACQ_REASON_ENUM_UINT32;


enum MMC_MMA_ACQ_RESULT_ENUM
{
    MMC_MMA_ACQ_RESULT_SUCCESS          = 0,
    MMC_MMA_ACQ_RESULT_FAILURE          = 1,
    MMC_MMA_ACQ_RESULT_BUTT
};
typedef VOS_UINT32 MMC_MMA_ACQ_RESULT_ENUM_UINT32;


enum MMC_MMA_REG_RESULT_ENUM
{
    MMC_MMA_REG_RESULT_SUCCESS          = 0,
    MMC_MMA_REG_RESULT_ACQ_FAILURE      = 1,
    MMC_MMA_REG_RESULT_REG_FAILURE      = 2,
    MMC_MMA_REG_RESULT_BUTT
};
typedef VOS_UINT32 MMC_MMA_REG_RESULT_ENUM_UINT32;


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

typedef  NAS_MML_PLMN_RAT_PRIO_STRU MMA_MMC_PLMN_RAT_PRIO_STRU;


typedef  NAS_MML_PLMN_ID_STRU MMA_MMC_PLMN_ID_STRU;


typedef struct
{
    VOS_UINT8                           ucRatNum;                                   /* 当前支持的接入技术个数  */
    MMA_MMC_3GPP2_RAT_TYPE_ENUM_UINT8   auc3GPP2Rat[MMA_MMC_3GPP2_RAT_TYPE_BUTT];
    VOS_UINT8                           aucReserve[1];
}MMA_MMC_3GPP2_RAT_STRU;
typedef struct
{
    VOS_UINT8                           ucEquPlmnNum;                           /* 当前EquPLMN的个数 */                                                                                /* EPLMN被删除或EPLMN个数为0时无效,其它情况都有效 */
    VOS_UINT8                           aucReserved[3];
    MMA_MMC_PLMN_ID_STRU                astEquPlmnAddr[MMA_MMC_MAX_EQUPLMN_NUM];/* 当前EquPLMN列表 */
}MMA_MMC_EPLMN_INFO_STRU;



typedef struct
{
    VOS_UINT8                           ucCellNum;
    VOS_UINT8                           aucReserve[3];
    VOS_UINT32                          aulCellId[MMA_MMC_CELL_MAX_NUM];
}MMA_MMC_CAMP_CELL_INFO_STRU;


typedef  NAS_MML_OPERATOR_NAME_INFO_STRU MMA_MMC_OPERATOR_NAME_INFO_STRU;


typedef NAS_MML_MS_BAND_INFO_STRU MMA_MMC_UE_SUPPORT_FREQ_BAND_STRU;


typedef struct
{
    VOS_INT16                           sRssiValue;
    VOS_INT16                           sRscpValue;
    VOS_UINT8                           ucBer;
    VOS_UINT8                           aucReserved[3];
}MMA_MMC_RSSI_VALUE_STRU;

#define MMA_MMC_SCELL_MEAS_TYPE_NONE                          0x00               /* AT查询上报  */
#define MMA_MMC_SCELL_MEAS_TYPE_RXLEV                         0x01               /* RXLEV变化触发的上报 */
#define MMA_MMC_SCELL_MEAS_TYPE_RXQUALITY                     0x02               /* RXQUALITY变化触发的上报 */
#define MMA_MMC_SCELL_MEAS_TYPE_CELLID                        0x04               /* CELL ID变化触发的上报 */


typedef struct
{
    VOS_UINT8                           ucMeasRxlevType          :1;             /* Rxlev改变触发的测量上报 */
    VOS_UINT8                           ucMeasRxqualityType      :1;             /* Rxquality改变触发的测量上报 */
    VOS_UINT8                           ucMeasCellIdType         :1;             /* Rxquality改变触发的测量上报 */
    VOS_UINT8                           ucMeasTypeSpare5         :5;
}MMA_MMC_SCELL_MEAS_TYPE_STRU;
typedef struct
{
    VOS_UINT8                             ucMeasReportType;

    MMA_MMC_SCELL_MEAS_TYPE_STRU          stMeasReportType;
}MMA_MMC_SCELL_MEAS_REPORT_TYPE_UN;


typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT8                           ucRssiNum;
    VOS_UINT8                           ucReserved;
    VOS_UINT16                          usCellDlFreq;                           /*小区频点(下行)*/
    VOS_UINT16                          usCellUlFreq;                           /*当前频点(上行)*/
    VOS_INT16                           sUeRfPower;                             /*发射功率*/
    MMA_MMC_RSSI_VALUE_STRU             astRssi[MMA_MMC_CELL_MAX_NUM];
    MMA_MMC_SCELL_MEAS_REPORT_TYPE_UN   unMeasReportType;                       /* 本次上报类型 */
    VOS_UINT8                           aucReserved1[3];
}MMC_MMA_RSSI_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                                         MsgHeader;          /* 消息头                                   */
    MMA_MMC_CARD_STATUS_ENUM_UINT8                          enCardStatus;
    VOS_UINT8                                               aucImsi[NAS_MAX_IMSI_LENGTH];
    MMA_MMC_ALLOWED_REG_DOMAIN_ENUM_UINT8                   enRegDomain;        /* 在原接口新增开机允许注册的服务域 */

    MMA_MMC_REG_CONTROL_ENUM_UINT8                          enRegCtrl;

    MMA_MMC_PLMN_RAT_PRIO_STRU                              stPlmnRatPrio;

    MMA_MMC_3GPP2_RAT_STRU                                  st3Gpp2Rat;
}MMA_MMC_START_REQ_STRU;


typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulResult;
}MMC_MMA_START_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    MMA_MMC_PLMN_ID_STRU                stPlmnId;
    MMA_MMC_NET_RAT_TYPE_ENUM_UINT8     enCurNetWork;                           /* 当前网络类型 */
    VOS_UINT8                           ucRac;
    VOS_UINT8                           ucSysSubMode;                           /* 驻留网络的系统子模式，取值与RRC_NAS_SYS_SUBMODE_ENUM相同 */

    VOS_UINT8                           ucIsFobbiddenPlmnFlag;

    VOS_UINT16                          usLac;                                  /* 位置码信息 */
    VOS_UINT8                           ucRoamFlag;                             /* 当前是否处于漫游 */
    VOS_UINT8                           ucPsSupportFlg;                         /* 当前是否支持PS域 */
    MMA_MMC_CAMP_CELL_INFO_STRU         stCellId;                               /* 小区信息 */

    VOS_UINT8                           ucPsAttachAllowFlag;
    VOS_UINT8                           ucCsAttachAllowFlag;
    VOS_UINT16                          usArfcn;
    MMC_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8                  enPrioClass;
    VOS_UINT8                                               aucReserve[3];
}MMC_MMA_SYS_INFO_IND_STRU;

typedef NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16 NAS_MMC_REG_FAIL_CAUSE_ENUM_UINT16;

typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */ /*_H2ASN_Skip*/
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enCnDomainId;
    VOS_UINT8                           ucRegRslt;                              /* VOS_TRUE:指定域注册成功; 反之注册失败 */
    VOS_UINT8                           ucReportCauseCtrl;                      /* 是否需要上报原因值 */
    NAS_MMC_REG_FAIL_CAUSE_ENUM_UINT16  enRejCause;
}MMC_MMA_REG_RESULT_IND_STRU;

typedef NAS_MML_SIM_AUTH_FAIL_ENUM_UINT16 NAS_MMC_SIM_AUTH_FAIL_ENUM_UINT16;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enCnDomainId;
    NAS_MMC_SIM_AUTH_FAIL_ENUM_UINT16   enRejCause;
    VOS_UINT8                           aucReserve[2];
}MMC_MMA_USIM_AUTH_FAIL_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                              MsgHeader;                     /* 消息头                                   */
    VOS_UINT32                                   bitOpSrvSta     : 1;
    VOS_UINT32                                   bitOpRegSta     : 1;
    VOS_UINT32                                   bitSpare        : 30;
    
    MMA_MMC_SRVDOMAIN_ENUM_UINT32                enCnDomainId;
    VOS_UINT8                                    ucSimCsRegStatus;
    VOS_UINT8                                    ucSimPsRegStatus;    
    VOS_UINT8                                    aucReserved[1];
    MMA_MMC_REG_STATE_ENUM_UINT8                 enRegState;
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32           enServiceStatus;
}MMC_MMA_SERVICE_STATUS_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT8                           ucIeFlg;
    VOS_INT8                            cLocalTimeZone;
    VOS_UINT8                           ucDST;
    VOS_UINT8                           aucLSAID[3];
    VOS_UINT8                           aucReserve[2];
    TIME_ZONE_TIME_STRU                 stUniversalTimeandLocalTimeZone;
    MMA_MMC_OPERATOR_NAME_INFO_STRU     stName;
}MMC_MMA_MM_INFO_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT8                           ucActionType;
    VOS_UINT8                           ucRrcMsgType;

    VOS_UINT8                           ucSignThreshold;                        /* 信号质量改变门限值 */
    VOS_UINT8                           ucMinRptTimerInterval;                  /* 间隔上报的时间   */
}MMA_MMC_SIGN_REPORT_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    MMA_MMC_MS_MODE_ENUM_UINT32         enMsMode;
}MMA_MMC_MODE_CHANGE_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulOpID;
    MMA_MMC_ATTACH_TYPE_ENUM_UINT32     enAttachType;

    MMA_MMC_EPS_ATTACH_REASON_ENUM_UINT32      enEpsAttachReason;
}MMA_MMC_ATTACH_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulOpID;
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enCnDomainId;
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32  enServiceStatus;
    MMA_MMC_PLMN_ID_STRU                stPlmnId;
}MMC_MMA_ATTACH_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulOpID;
    MMA_MMC_DETACH_TYPE_ENUM_UINT32     enDetachType;

    MMA_MMC_DETACH_REASON_ENUM_UINT32   enDetachReason;

}MMA_MMC_DETACH_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulOpID;
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enCnDomainId;
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32  enServiceStatus;
}MMC_MMA_DETACH_CNF_STRU;
enum MMA_MMC_NETWORK_DETACH_TYPE_ENUM
{
    MMA_MMC_NETWORK_DETACH_TYPE_REATTACH      = 1,
    MMA_MMC_NETWORK_DETACH_TYPE_NOATTACH      = 2,
    MMA_MMC_NETWORK_DETACH_TYPE_IMSI          = 3,
    MMA_MMC_NETWORK_DETACH_TYPE_BUTT
};
typedef VOS_UINT8 MMA_MMC_NETWORK_DETACH_TYPE_ENUM_UINT8;



typedef struct
{
    MSG_HEADER_STRU                                         MsgHeader;          /* 消息头                                   */
    MMA_MMC_SRVDOMAIN_ENUM_UINT32                           enCnDomainId;
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32                      enServiceStatus;

    VOS_UINT32                                              ulDetachCause;
    MMA_MMC_NETWORK_DETACH_TYPE_ENUM_UINT8                  enNetworkDetachType;
    VOS_UINT8                                               aucReserved[3];


}MMC_MMA_DETACH_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulReserved;
}MMA_MMC_PLMN_LIST_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulCnt;
    MMA_MMC_PLMN_ID_STRU                aPlmnList[MMA_MMC_MAX_PLMN_CNT];
    VOS_UINT8                           aucPlmnStatus[MMA_MMC_MAX_PLMN_CNT];
    VOS_UINT8                           aucRaMode[MMA_MMC_MAX_PLMN_CNT];
}MMC_MMA_PLMN_LIST_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulReserved;
}MMC_MMA_PLMN_LIST_REJ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulReserved;
}MMA_MMC_PLMN_LIST_ABORT_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    MMA_MMC_PLMN_SEL_MODE_ENUM_UINT32   enPlmnSelMode;
}MMA_MMC_PLMN_USER_RESEL_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    MMA_MMC_PLMN_ID_STRU                stPlmnId;
    MMA_MMC_NET_RAT_TYPE_ENUM_UINT8     enAccessMode;
    VOS_UINT8                           aucReserved[3];
}MMA_MMC_PLMN_SPECIAL_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                                         MsgHeader;          /* 消息头                                   */
    MMA_MMC_COVERAGE_AREA_CHANGE_MODE_ENUM_UINT32           enCoverageChgMode;
    MMA_MMC_PLMN_ID_STRU                                    stPlmnId;
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32                      enCsSvcSta;
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32                      enPsSvcSta;
    VOS_UINT32                                              ulTimMaxFlg;
}MMC_MMA_COVERAGE_AREA_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    MMA_MMC_POWER_OFF_CAUSE_ENUM_UINT32 enCause;
}MMA_MMC_POWER_OFF_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulReserved;
}MMC_MMA_POWER_OFF_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                                 MsgHeader;                  /* 消息头                                   */
    MMA_MMC_USER_PLMN_SEARCH_RESULT_ENUM_UINT8      enResult;
    VOS_UINT8                                       aucReserved[3];
}MMC_MMA_PLMN_SPECIAL_SEL_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                                         MsgHeader;          /* 消息头                                   */
    MMA_MMC_USER_PLMN_SEARCH_RESULT_ENUM_UINT8              enResult;
    VOS_UINT8                                               aucReserved[3];
}MMC_MMA_PLMN_RESEL_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    MMA_MMC_DATATRAN_ATTRI_ENUM_UINT8   enDataTranAttri;
    VOS_UINT8                           aucReserved[3];
}MMC_MMA_DATATRAN_ATTRI_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    MMA_MMC_PLMN_RAT_PRIO_STRU          stRatPrioList;
    MMA_MMC_UE_SUPPORT_FREQ_BAND_STRU   stBand;
    MMA_MMC_ROAM_CAPABILITY_ENUM_UINT8  enRoamCapability;
    VOS_UINT8                           ucOnlyRoamParaChanged;
    VOS_UINT8                           aucReserved[2];
}MMA_MMC_SYS_CFG_SET_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                           MsgHeader;                        /* 消息头                                   */
    MMA_MMC_SYS_CFG_SET_RESULT_ENUM_UINT32    enRst;
}MMC_MMA_SYS_CFG_SET_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulReserved;
}MMA_MMC_PLMN_SEARCH_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                           MsgHeader;                        /* 消息头                                   */
    MMA_MMC_PLMN_SELECTION_RESULT_ENUM_UINT32 enResult;
}MMC_MMA_PLMN_SELECTION_RLST_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                           MsgHeader;                        /* 消息头                                   */
    MMA_MMC_PLMN_SELECTION_START_FLAG_ENUM_UINT32 enPlmnSeleStartFlag;
}MMC_MMA_PLMN_SELECTION_START_IND_STRU;


typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulCnt;
    MMA_MMC_PLMN_ID_STRU                aPlmnList[MMA_MMC_MAX_PLMN_CNT];
    VOS_UINT8                           aucPlmnStatus[MMA_MMC_MAX_PLMN_CNT];
    VOS_UINT8                           aucReserved1[3];
    VOS_UINT8                           aucRaMode[MMA_MMC_MAX_PLMN_CNT];
    VOS_UINT8                           aucReserved2[3];
}MMC_MMA_PLMN_LIST_ABORT_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulReserved;
}MMA_MMC_SPEC_PLMN_SEARCH_ABORT_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulReserved;
}MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT8                           ucCipherAlgorForCSDomain;
    VOS_UINT8                           ucCipherAlgorForPSDomain;
    VOS_UINT8                           aucRcv[2];
}MMC_MMA_UMTS_CIPHER_INFO_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT8                           ucGprsCipherAlg;
    VOS_UINT8                           ucGprsCipher;
    VOS_UINT8                           aucRcv[2];
}MMC_MMA_GPRS_CIPHER_INFO_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulReserved;
}MMC_MMA_PLMN_SPECIAL_SEL_REJ_STRU;
typedef struct
{
    MSG_HEADER_STRU                             MsgHeader;                      /* 消息头                                   */
    MMA_MMC_CELL_RESTRICTION_TYPE_ENUM_UINT8    enCellAcType;
    VOS_UINT8                                   ucRestrictRegister;
    VOS_UINT8                                   ucRestrictPagingRsp;
    VOS_UINT8                                   ucReserved;
    MMA_MMC_SRVDOMAIN_ENUM_UINT32               enCnDomainId;
}MMC_MMA_AC_INFO_CHANGE_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头 */

    VOS_UINT8                           ucOmConnectFlg;                         /* UE与PC工具的连接标志, VOS_TRUE: 已连接; VOS_FALSE: 未连接 */
    VOS_UINT8                           ucOmPcRecurEnableFlg;                   /* OM配置的发送NAS PC回放消息的使能标志 */
    VOS_UINT8                           aucRsv2[2];                             /* 保留   */
} MMA_MMC_OM_MAINTAIN_INFO_IND_STRU;


typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT8                           aucRsv4[4];                             /* 保留   */
}MMA_MMC_UPDATE_UPLMN_NTF_STRU;



typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                                             /* 消息头*/
    TAF_UINT8                           aucVersion[NAS_MML_MAX_USER_OPLMN_VERSION_LEN];        /* OPLMN List版本号 */
    TAF_UINT8                           ucIndex;                                /* 当前设置的组Index */ 
    TAF_UINT8                           ucOPlmnCount;                           /* 单组实际设置OPLMN的个数 */
    TAF_UINT8                           aucOPlmnWithRat[NAS_MML_MAX_GROUP_CFG_OPLMN_DATA_LEN];  /* OPLMN的PDU数据，和EFOplmn文件一致 */
}MMA_MMC_EOPLMN_SET_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;          /* 消息头 */
    VOS_UINT32                          ulResult;           /* OPLMN的nv保存结果*/
}MMC_MMA_EOPLMN_SET_CNF_STRU;


typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    VOS_UINT32                          ulReserved;
}MMC_MMA_EOPLMN_SET_ABORT_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头 */

    VOS_UINT16                          usCellNum;                              /* 需要扫描出超过门限的小区个数 */
    VOS_INT16                           sCellPow;                               /* 小区门限值 */
    MMA_MMC_NET_RAT_TYPE_ENUM_UINT8     ucRat;                                  /* 需要扫描的接入模式 */
    VOS_UINT8                           aucReserve[3];
    MMA_MMC_UE_SUPPORT_FREQ_BAND_STRU   stBand;                                 /* 记录GU频段*/
}MMA_MMC_NET_SCAN_REQ_STRU;
typedef struct
{
    VOS_UINT16                          usArfcn;                                /* 频点 */
    VOS_UINT16                          usC1;                                   /* 用于GSM小区重选的C1值(暂不支持) */
    VOS_UINT16                          usC2;                                   /* 用于GSM小区重选的C2值(暂不支持) */
    VOS_UINT16                          usLac;                                  /* 位置区码 */
    VOS_UINT32                          ulMcc;                                  /* PLMN国家码 */
    VOS_UINT32                          ulMnc;                                  /* PLMN网络码 */
    VOS_UINT16                          usBsic;                                 /* GSM小区基站码 */
    VOS_INT16                           sRxlev;                                 /* 接收到的信号强度 */
    VOS_INT16                           sRssi;                                  /* GSM是BCCH信道的RSSI，WCDMA是RSCP */
    VOS_UINT8                           aucReserve[2];
    VOS_UINT32                          ulCellId;                               /* 小区ID */
    MMA_MMC_UE_SUPPORT_FREQ_BAND_STRU   stBand;                               /* 具体取值同接口RRMM_SYS_CFG_REQ_STRU中频段参数 */
}MMC_MMA_NET_SCAN_INFO_STRU;

/*****************************************************************************
 枚举名    : MMC_MMA_NET_SCAN_RESULT_ENUM
 结构说明  : MMC_MMA_NET_SCAN_CNF_STRU消息中的enResult;
*****************************************************************************/
enum MMC_MMA_NET_SCAN_RESULT_ENUM
{
    MMC_MMA_NET_SCAN_RESULT_SUCCESS     = 0,
    MMC_MMA_NET_SCAN_RESULT_FAILURE     = 1,
    MMC_MMA_NET_SCAN_RESULT_BUTT
};
typedef VOS_UINT8  MMC_MMA_NET_SCAN_RESULT_ENUM_UINT8;


enum MMC_MMA_NET_SCAN_CAUSE_ENUM
{
    MMC_MMA_NET_SCAN_CAUSE_SIGNAL_EXIST        = 0,                /* 存在信令连接 */
    MMC_MMA_NET_SCAN_CAUSE_STATE_NOT_ALLOWED   = 1,                /* 当前未驻留 */
    MMC_MMA_NET_SCAN_CAUSE_FREQ_LOCK           = 2,                /* 当前锁频状态 */
    MMC_MMA_NET_SCAN_CAUSE_PARA_ERROR          = 3,                /* 参数错误，包含接入技术不支持，频段不支持，频点个数错误，门限错误 */
    MMC_MMA_NET_SCAN_CAUSE_CONFLICT            = 4,                /* 错误原因值冲突 */
    MMC_MMA_NET_SCAN_CAUSE_SERVICE_EXIST,
    MMC_MMA_NET_SCAN_CAUSE_NOT_CAMPED,
    MMC_MMA_NET_SCAN_CAUSE_RAT_TYPE_ERROR,
    MMC_MMA_NET_SCAN_CAUSE_BUTT
};
typedef VOS_UINT8 MMC_MMA_NET_SCAN_CAUSE_ENUM_UINT8;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头 */

    MMC_MMA_NET_SCAN_RESULT_ENUM_UINT8  enResult;
    MMC_MMA_NET_SCAN_CAUSE_ENUM_UINT8   enCause;
    VOS_UINT8                           ucFreqNum;                                      /* 上报的频点个数 */
    VOS_UINT8                           aucReserve[1];
    MMC_MMA_NET_SCAN_INFO_STRU          astNetScanInfo[MMC_MMA_NET_SCAN_MAX_FREQ_NUM];
}MMC_MMA_NET_SCAN_CNF_STRU;


enum MMA_MMC_REJ_TYPE_ENUM
{
    MMA_MMC_REJ_TYPE_REG        = 0,
    MMA_MMC_REJ_TYPE_USIM_AUTH  = 1,
    MMA_MMC_REJ_TYPE_SERV_REQ   = 2,
    MMA_MMC_REJ_TYPE_NETWORK_DETACH_IND = 3,
    MMA_MMC_REJ_TYPE_BUTT
};
typedef VOS_UINT8 MMA_MMC_REJ_TYPE_ENUM_UINT8;


typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头                                   */
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enCnDomainId;
    VOS_UINT16                          usRejCause;
    VOS_UINT8                           aucReserved[2];
}MMC_MMA_SERV_REJ_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头 */
    VOS_UINT32                          ulReserved;
}MMA_MMC_ABORT_NET_SCAN_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* 消息头 */

    VOS_UINT8                           ucFreqNum;                                      /* 上报的频点个数 */
    VOS_UINT8                           aucReserve[3];
    MMC_MMA_NET_SCAN_INFO_STRU          astNetScanInfo[MMC_MMA_NET_SCAN_MAX_FREQ_NUM];
}MMC_MMA_ABORT_NET_SCAN_CNF_STRU;


typedef struct
{
    MSG_HEADER_STRU                        MsgHeader;          /* _H2ASN_Skip */     
    MMA_MMC_NW_IMS_VOICE_CAP_ENUM_UINT8    enNwImsVoCap ;
    MMA_MMC_NW_EMC_BS_CAP_ENUM_UINT8       enNwEmcBsCap ;
    MMA_MMC_LTE_CS_CAPBILITY_ENUM_UINT8    enLteCsCap ;
    VOS_UINT8                              aucReserve[1];
}MMC_MMA_NETWORK_CAPABILITY_INFO_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;                              /* _H2ASN_Skip */
    VOS_UINT8                           ucCampOnFlg;                            /* VOS_TRUE:驻留;VOS_FALSE:未驻留 */ 
    VOS_UINT8                           aucRsv[3];                              /* 保留   */
}MMC_MMA_CAMP_ON_IND_STRU_STRU;
typedef struct
{
    MSG_HEADER_STRU              stMsgHeader;                                   /*_H2ASN_Skip*/
    VOS_UINT32                   bitOpCurrCampPlmnId:1;

    VOS_UINT32                   bitOpEplmnInfo:1;
    VOS_UINT32                   bitSpare:30;

    MMA_MMC_PLMN_ID_STRU         stCurrCampPlmnId;                              /* 用于填写当前驻留小区的PLMN，如果丢网或者关机等非驻留状态，统一填写全0xff */

    MMA_MMC_EPLMN_INFO_STRU      stEplmnInfo;
} MMA_MMC_OTHER_MODEM_INFO_NOTIFY_STRU;




typedef struct
{
    VOS_UINT8                           ucTdsArfcnNum;                          /* 0表示TDD频点不存在 */
    VOS_UINT8                           ucReserved;
    VOS_UINT16                          ausTdsArfcnList[MMA_MMC_TDS_ARFCN_MAX_NUM];
}MMA_MMC_TDS_NCELL_INFO_STRU;
typedef struct
{
    VOS_UINT8                           ucLteArfcnNum;                          /* 0表示LTE频点不存在 */
    VOS_UINT8                           aucReserved[3];
    VOS_UINT16                          ausLteArfcnList[MMA_MMC_LTE_ARFCN_MAX_NUM];
}MMA_MMC_LTE_NCELL_INFO_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    MMA_MMC_TDS_NCELL_INFO_STRU         stTdsNcellInfo;
    MMA_MMC_LTE_NCELL_INFO_STRU         stLteNcellInfo;
}MMA_MMC_NCELL_INFO_NOTIFY_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    MMA_MMC_EPLMN_INFO_STRU             stEplmnInfo;
}MMC_MMA_EPLMN_INFO_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    VOS_UINT8                           ucCsServiceConnStatusFlag;
    VOS_UINT8                           aucReserved[3];
}MMC_MMA_CS_SERVICE_CONN_STATUS_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    MTC_PS_TRANSFER_ENUM_UINT8          enSolutionCfg;                          /* PS域迁移方案 */
    VOS_UINT8                           ucCause;                                /* PS域切换原因值 */
    VOS_UINT8                           aucReserved[2];                         /*保留位*/
} MMA_MMC_PS_TRANSFER_IND_STRU;



typedef struct
{
    MSG_HEADER_STRU                                         MsgHeader;          /* _H2ASN_Skip */
    
    VOS_UINT8                                               ucAvail;            /* IMS VOICE是否可用 */
    VOS_UINT8                                               aucRsv[3];
}MMA_MMC_IMS_VOICE_CAP_IND_STRU;



typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;
    MMA_MMC_ACQ_REASON_ENUM_UINT32      enAcqReason;
}MMA_MMC_ACQ_REQ_STRU;


typedef struct
{
    MSG_HEADER_STRU                            stMsgHeader;
    MMC_MMA_ACQ_RESULT_ENUM_UINT32             enAcqRslt;
    MMA_MMC_PLMN_ID_STRU                       stPlmnId;
    VOS_UINT16                                 usArfcn;
    MMC_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8     enPrioClass;
    VOS_UINT8                                  aucReserve[1];
}MMC_MMA_ACQ_CNF_STRU;


typedef struct
{
    MMA_MMC_NET_RAT_TYPE_ENUM_UINT8            ucRat;
    VOS_UINT8                                  aucReserved[3];
    VOS_UINT16                                 usArfcn;
    VOS_UINT8                                  ucPlmnNum;
    VOS_UINT8                                  ucCellNum;
    MMA_MMC_PLMN_ID_STRU                       astPlmnId[MMA_MMC_REG_MAX_PLMN_NUM];
    VOS_UINT16                                 ausCellId[MMA_MMC_REG_MAX_CELL_NUM];
} MMA_MMC_REG_CELL_INFO_STRU;


typedef struct
{
    MSG_HEADER_STRU                            stMsgHeader;
    VOS_UINT32                                 bitOpPrioClass  : 1;
    VOS_UINT32                                 bitOpCellInfo   : 1;
    VOS_UINT32                                 bitSpare        : 30;
    MMC_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8     enPrioClass;
    VOS_UINT8                                  aucReserve[3];
    MMA_MMC_REG_CELL_INFO_STRU                 stCellInfo;
}MMA_MMC_REG_REQ_STRU;


typedef struct
{
    MSG_HEADER_STRU                            stMsgHeader;
    MMC_MMA_REG_RESULT_ENUM_UINT32             enRegRslt;
    MMA_MMC_PLMN_ID_STRU                       stPlmnId;
    VOS_UINT16                                 usArfcn;
    MMC_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8     enPrioClass;
    VOS_UINT8                                  aucReserve[1];
}MMC_MMA_REG_CNF_STRU;


typedef struct
{
    MSG_HEADER_STRU                            stMsgHeader;
    VOS_UINT8                                  aucReserve[4];
}MMA_MMC_POWER_SAVE_REQ_STRU;


typedef struct
{
    MSG_HEADER_STRU                            stMsgHeader;
    VOS_UINT8                                  aucReserve[4];
}MMC_MMA_POWER_SAVE_CNF_STRU;


typedef  MMC_MMA_ACQ_CNF_STRU MMC_MMA_ACQ_IND_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    MMA_MMC_MSG_ID_ENUM_UINT32          enMsgID;    /*_H2ASN_MsgChoice_Export MMA_MMC_MSG_ID_ENUM_UINT32*/
                                                    /* 为了兼容NAS的消息头定义，所以转换ASN.1使用MMA_MMC_MSG_ID_ENUM_UINT32 */

    VOS_UINT8                           aucMsgBlock[2];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          MMA_MMC_MSG_ID_ENUM_UINT32
    ****************************************************************************/
}MMA_MMC_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    MMA_MMC_MSG_DATA                    stMsgData;
}MmaMmcInterface_MSG;


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID Api_AppMmLayerContextSndoutForPcReplay(VOS_VOID);


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

#endif
