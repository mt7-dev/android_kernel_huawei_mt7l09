
#ifndef _NAS_MML_CTX_H_
#define _NAS_MML_CTX_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "PsTypeDef.h"
#include  "ComInterface.h"
#include  "product_config.h"

#include "MmcLmmInterface.h"

#include "omringbuffer.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define NAS_MML_CLASSMARK2_LEN                          (4)                     /* ClassMark2的最大长度 */
#define NAS_MML_CLASSMARK3_LEN                          (33)                     /* ClassMark3的最大长度 */
#define NAS_MML_MAX_IMEISV_LEN                          (16)                    /* Imeisv的最大长度 */
#define NAS_MML_MAX_NETWORK_CAPABILITY_LEN              (9)                     /* network capability的最大长度 */
#define NAS_MML_MAX_UE_NETWORK_CAPABILITY_LEN           (13)                    /* ue network capability的最大长度*/
#define NAS_MML_MIN_UE_NETWORK_CAPABILITY_LEN           (2)                     /* ue network capability的最小长度*/
#define NAS_MML_DEFAULT_UE_NETWORK_CAPABILITY_LEN       (4)                     /* ue network capability的默认长度*/
#define NAS_MML_DEFAULT_UE_NET_CAP_FIRST_VALUE          (0xE0)
#define NAS_MML_DEFAULT_UE_NET_CAP_THIRD_VALUE          (0xC0)
#define NAS_MML_MAX_CS_REG_FAIL_CNT                     (4)                     /* CS连续注册失败的最大可配置次数，en_NV_Item_Register_Fail_Cnt中注册失败次数最大取值*/
#define NAS_MML_MAX_PS_REG_FAIL_CNT                     (5)                     /* PS连续注册失败的协议最大次数*/
#define NAS_MML_DEFAULT_ROAM_BROKER_REG_FAIL_CNT        (2)                     /* PS或者CS连续注册失败的默认次数，超过该次数则需发起搜网,en_NV_Item_Register_Fail_Cnt中注册失败次数默认值 */
#define NAS_MML_MAX_IMSI_LEN                            (9)                     /* 最大的IMSI的长度 */
#define NAS_MML_MAX_PTMSI_LEN                           (4)                     /* 最大的PTMSI的长度 */
#define NAS_MML_MAX_TMSI_LEN                            (4)                     /* 最大的TMSI的长度 */
#define NAS_MML_MAX_PTMSI_SIGNATURE_LEN                 (3)                     /* 最大的PTMSI Signature的长度 */

#define NAS_MML_ADDITIONAL_OLD_RAI_IEI_LEN              (6)                     /* ADDITIONAL OLD RAI 选项的IEI的长度值 */

#define NAS_MML_MAX_LAC_LEN                             (2)                     /* LAC的长度 */
#define NAS_MML_SIM_FORMAT_PLMN_LEN                     (3)                     /* Sim卡格式的Plmn长度 */
#define NAS_MML_PLMN_WITH_RAT_UNIT_LEN                  (5)                     /* sim卡中带接入技术网络的基本元素长度，如6F62文件的基本长度单元为5 */
#define NAS_MML_PLMN_WITH_RAT_FIRST_RAT_OFFSET          (3)                     /* sim卡中带接入技术网络中接入技术的起始位置偏移 */
#define NAS_MML_PLMN_WITH_RAT_SECOND_RAT_OFFSET         (4)                     /* sim卡中带接入技术网络中接入技术的起始位置偏移 */

#define NAS_MML_DEFAULT_CLASSMARK1_VALUE                (0x57)
#define NAS_MML_DEFAULT_CLASSMARK2_FIRST_VALUE          (0x57)
#define NAS_MML_DEFAULT_CLASSMARK2_SECOND_VALUE         (0x18)
#define NAS_MML_DEFAULT_CLASSMARK2_THIRD_VALUE          (0x81)
#define NAS_MML_DEFAULT_CLASSMARK2_FOURTH_VALUE         (0x00)

#define NAS_MML_DEFAULT_NETWORKCAPABILITY_LEN           (0x02)
#define NAS_MML_DEFAULT_NETWORKCAPABILITY_FIRST_VALUE   (0xE5)
#define NAS_MML_DEFAULT_NETWORKCAPABILITY_SECOND_VALUE  (0x40)
#define NAS_MML_MSG_NAME_ID_LENGTH                      (0x04)
#define NAS_MML_MSG_TIMER_ID_LENGTH                     (0x08)                  /* TIMER消息结构体的长度 */

#define NAS_MML_DEFAULT_IMEI_VALUE                      (0x01)

#define NAS_MML_DEFAULT_MAX_FORB_ROAM_LA_NUM            (10)                    /* 默认最大禁止的LA个数*/
#define NAS_MML_SIM_PLMN_ID_LEN                         (3)                     /* SIM卡中PLMN ID的长度 */
#define NAS_MML_PS_LOCI_SIM_FILE_LEN                    (14)                    /* USIM中PSLOCI文件的长度 */
#define NAS_MML_CS_LOCI_SIM_FILE_LEN                    (11)                    /* USIM中LOCI文件的长度 */
#define NAS_MML_PS_KEY_USIM_FILE_LEN                    (33)                    /* USIM中PSKEY文件的长度 */
#define NAS_MML_PS_KC_SIM_FILE_LEN                      (9)                     /* SIM卡中EFKc的长度 */
#define NAS_MML_CS_KEY_USIM_FILE_LEN                    (33)                    /* USIM中PSKEY文件的长度 */
#define NAS_MML_CS_KC_SIM_FILE_LEN                      (9)                     /* SIM卡中EFKc的长度 */
#define NAS_MML_PTMSI_SIGNATRUE_POS_EFPSLOCI_FILE       (4)                     /* EFPSLOCI文件中PTMSI Signature的位置 */
#define NAS_MML_LAI_POS_EFLOCI_FILE                     (4)                     /* EFLOCI文件中LAI的位置 */
#define NAS_MML_RAI_POS_EFPSLOCI_FILE                   (7)                     /* EFPSLOCI文件中RAI的位置 */
#define NAS_MML_CK_POS_EFKEY_FILE                       (1)                     /* EFKEY中CK的位置 */
#define NAS_MML_PLMN_LOCI_SIM_FILE_LEN                  (14)                    /* USIM中PSLOCI文件的长度 */
#define NAS_MML_LRPLMNSI_SIM_FILE_LEN                   (1)                     /* LAST RPLMN SELECTION IND文件的长度 */
#define NAS_MML_EHPLMNPI_SIM_FILE_LEN                   (1)                     /* LAST RPLMN SELECTION IND文件的长度 */


#define NAS_MML_SIM_FILE_LEN_VALID                      (0x00)                  /* SIM卡文件长度合法         */
#define NAS_MML_GSMKC_INVALID                           (0x00)                  /* CK无效值的宏定义         */
#define NAS_MML_CK_INVALID                              (0x00)                  /* CK无效值的宏定义         */
#define NAS_MML_IK_INVALID                              (0x00)                  /* IK无效值的宏定义         */
#define NAS_MML_CKSN_INVALID                            (0x07)                  /* CKSN无效值的宏定义       */
#define NAS_MML_IMSI_INVALID                            (0xFF)                  /* IMSI的无效值的宏定义     */
#define NAS_MML_PTMSI_INVALID                           (0xFF)                  /* P-TMSI无效值的宏定义                     */
#define NAS_MML_TMSI_INVALID                            (0xFF)                  /* TMSI无效值的宏定义                     */
#define NAS_MML_IMSI_INVALID                            (0xFF)
#define NAS_MML_LAC_LOW_BYTE_INVALID                    (0xFF)                  /* 消息中被删除的RAI中LAC首字节的取值       */
#define NAS_MML_LAC_HIGH_BYTE_INVALID                   (0xFE)                  /* 消息中被删除的RAI中LAC次字节的取值       */
#define NAS_MML_RAC_INVALID                             (0xFF)                  /* 消息中无效的RAC的值 */
#define NAS_MML_CELLID_INVALID                          (0xFFFFFFFF)
#define NAS_MML_PTMSI_SIGNATURE_INVALID                 (0xFF)                  /* P-TMSI signature的无效取值               */
#define NAS_MML_LAC_INVALID_SPECAL_IN_SIM               (0x00)                  /* SIM中被删除的RAI中LAC的取值              */
#define NAS_MML_UPDATE_STATUS_MASK_IN_SIM               (0x07)                  /* SIM卡中update status的bit位 */
#define NAS_MML_LRPLMN_SELECTION_IND_VALID              (0x01)                  /* LAST RPLMN SELECTION IND功能开启 */
#define NAS_MML_LRPLMN_SELECTION_IND_INVALID            (0x00)                  /* LAST RPLMN SELECTION IND功能未开启 */
#define NAS_MML_MCC_FOURTH_BYTES_VALID                  (0x00)                  /* 判断国家码第四个字节只能为0 */
#define NAS_MML_MAX_DIGIT_VALUE                         (9)
#define NAS_MML_BYTE_MASK                               (0xFF)

#define NAS_MML_LOW_BYTE_INVALID                        (0x0F)

#define NAS_MML_MCC_LOW_VALUE_MASK                      (0x0000FFFF)
#define NAS_MML_MCC_THIRD_BYTE_VALUE_MASK               (0x00FF0000)
#define NAS_MML_PCS1900_MCC_LOW_VALUE                   (0x00000103)
#define NAS_MML_PCS1900_MCC_HIGH_VALUE_RANGE            (0x00060000)
#define NAS_MML_PCS1900_MNC_THIRD_BYTE_VALUE_0          (0x00)
#define NAS_MML_PCS1900_MNC_THIRD_BYTE_VALUE_F          (0x0F)


#define NAS_MML_INVALID_MCC                             (0xffffffff)            /* MCC的无效值 */
#define NAS_MML_INVALID_MNC                             (0xffffffff)            /* MNC的无效值 */
#define NAS_MML_PLMN_ID_ZERO                            (0x00)                  /* 设置PLMNid的内容位全0 */
#define NAS_MML_OCTET_LOW_FOUR_BITS                     (0x0f)                  /* 获取一个字节中的低4位 */
#define NAS_MML_OCTET_HIGH_FOUR_BITS                    (0xf0)                  /* 获取一个字节中的高4位 */
#define NAS_MML_OCTET_LOW_TWO_BITS                      (0x03)                  /* 获取一个字节中的低2位 */
#define NAS_MML_OCTET_LOW_THREE_BITS                    (0x07)                  /* 获取一个字节中的低3位 */
#define NAS_MML_OCTET_MOVE_TWO_BITS                     (0x02)                  /* 将一个字节移动2位 */
#define NAS_MML_HO_WAIT_SYS_INFO_DEFAULT_LEN_VALUE      (5000)                  /* CSFB HO后默认等系统消息的值，单位默认为MS */
#define NAS_MML_ONE_THOUSAND_MILLISECOND                (1000)                  /* 1000MS */

#define NAS_MML_OCTET_MOVE_THREE_BITS                   (0x03)                  /* 将一个字节移动3位 */
#define NAS_MML_MNC_LENGTH_TWO_BYTES_IN_IMSI            (0x02)

#define NAS_MML_OCTET_MOVE_FOUR_BITS                    (0x04)                  /* 将一个字节移动4位 */
#define NAS_MML_OCTET_MOVE_EIGHT_BITS                   (0x08)                  /* 将一个字节移动8位 */
#define NAS_MML_OCTET_MOVE_TWELVE_BITS                  (0x0C)                  /* 将一个字节移动8位 */
#define NAS_MML_OCTET_MOVE_SIXTEEN_BITS                 (0x10)                  /* 将一个字节移动16位 */
#define NAS_MML_OCTET_MOVE_TWENTY_FOUR_BITS             (0x18)                 /* 将一个字节移动24位 */

#define NAS_MML_NET_MODE_BIT_MASK_WAS_SYS_INFO          (0x01)                  /* WRR系统消息中的网络模式对应的Bit位 */
#define NAS_MML_ATTATCH_IND_BIT_MASK_WAS_SYS_INFO       (0x01)                  /* WRR系统消息中的Attach/Detach模式对应的Bit位 */

#define NAS_MML_INVALID_INDEX                           (0xFFFFFFFF)            /* 无效的Index值 */

#define NAS_MML_INVALID_EVENT_TYPE                      (0xFFFFFFFF)            /* 无效的事件类型的值 */



#define NAS_MML_INVALID_SIM_RAT                         (0x0)                   /* SIM卡中无效的接入技术 */
#define NAS_MML_SIM_RAT_UTRN                            (0x8000)
#define NAS_MML_SIM_RAT_E_UTRN                          (0x4000)
#define NAS_MML_SIM_RAT_GSM                             (0x0080)
#define NAS_MML_SIM_RAT_GSM_COMPACT                     (0x0040)
#define NAS_MML_SIM_RAT_ALL_SUPPORT                     (0xFFFF)
#define NAS_MML_SIM_ALL_GSM_RAT                         (0x00C0)
#define NAS_MML_SIM_UTRN_GSM_RAT                        (0x8080)
#define NAS_MML_SIM_NO_RAT                              (0x0000)

#define NAS_MML_SIM_ALL_RAT_SUPPORT_LTE                 (0xC0C0)
#define NAS_MML_SIM_UE_SUPPORT_RAT_SUPPORT_LTE          (0xC080)
#define NAS_MML_SIM_ALL_RAT                             (0x80C0)
#define NAS_MML_SIM_UE_SUPPORT_RAT                      (0x8080)

#define NAS_MML_UMTS_CK_LEN                             (16)                    /* UMTS的CK内容 */
#define NAS_MML_UMTS_IK_LEN                             (16)                    /* UMTS的IK内容 */
#define NAS_MML_GSM_KC_LEN                              (8)                     /* GSM的KC内容 */

#define NAS_MML_MAX_EHPLMN_NUM                          (16)                    /* EHPLMN最大的个数 */
#define NAS_MML_MAX_HPLMN_WITH_RAT_NUM                  (16)                    /* HPLMNWithRat最大的个数 */
#define NAS_MML_MAX_USERPLMN_NUM                        (100)                   /* USERPLMN最大的个数 */

#define NAS_MML_MAX_OPERPLMN_NUM                        (256)                   /* OPERPLMN最大的个数 */

#define NAS_MML_MAX_SELPLMN_NUM                         (85)                    /* SELPLMN最大的个数 */

/* PLMN和LA的禁止信息的存储结构 */
#define NAS_MML_MAX_FORBPLMN_NUM                        (20)                    /* forbidden PLMNs最大个数 */
#define NAS_MML_MAX_FORBGPRSPLMN_NUM                    (16)                     /* forbidden PLMNs for GPRS service最大个数 */
#define NAS_MML_MAX_FORBLA_FOR_ROAM_NUM                 (16)                    /* forbidden las for roaming的最大个数 */
#define NAS_MML_MAX_FORBLA_FOR_REG_NUM                  (10)                    /* forbidden las for regional provision的最大个数 */

/* 网侧上报的EPLMN的最大个数 */
#define NAS_MML_MAX_NETWORK_EQUPLMN_NUM                 (15)                    /* 等效的PLMN的最大个数                     */
/* EPLMN的最大个数 */
#define NAS_MML_MAX_EQUPLMN_NUM                         (18)                    /* 等效的PLMN的最大个数                     */

#define NAS_MML_MAX_PLATFORM_RAT_NUM                    (7)                     /* 平台支持的最大接入技术个数 */
#define NAS_MML_MIN_PLATFORM_RAT_NUM                    (1)                     /* 平台支持的最小接入技术个数 */

#define NAS_MML_MAX_HPLMN_REJ_CAUSE_CHANGE_COUNTER       (10)                    /* 将拒绝原因值修改为#17的最大次数 */
#if (FEATURE_ON == FEATURE_LTE)
#define NAS_MML_MAX_RAT_NUM                             (3)                     /* 当前支持的接入技术个数 */
#else
#define NAS_MML_MAX_RAT_NUM                             (2)                     /* 当前支持的GU接入技术个数 */
#endif

#define NAS_MML_MAX_3GPP2_RAT_NUM                       (2)

#define NAS_MML_MAX_ROAM_PLMN_NUM                       (4)                     /* 漫游允许的最大Plmn个数 */
#define NAS_MML_DFAULT_NATIONAL_ROAM_PLMN_NUM           (0x01)
#define NAS_MML_DFAULT_NOT_ROAM_PLMN_NUM                (0x00)
#define NAS_MML_DFAULT_NATIONAL_ROAM_MCC                (0x50005)
#define NAS_MML_DFAULT_NATIONAL_ROAM_MNC                (0xF0100)

#define NAS_MML_MAX_NATIONAL_MCC_NUM           (0x05)

#define NAS_MML_MAX_OPER_LONG_NAME_LEN                  (40)                    /* spn 采用ucs2编码时需要的长度为32，添加一个\0 对齐到36位,因为最大有39的long name ,调整到40*/
#define NAS_MML_MAX_OPER_SHORT_NAME_LEN                 (36)                    /* spn 采用ucs2编码时需要的长度为32 ,添加一个\0 对齐到36位 */


#define NAS_MML_MAX_WHITE_LOCK_PLMN_NUM                 (16)                    /* 白名单锁网支持的PLMN ID的最大个数 */
#define NAS_MML_MAX_BLACK_LOCK_PLMN_NUM                 (16)                    /* 黑名单锁网支持的PLMN ID的最大个数 */

#define NAS_MML_MAX_USER_CFG_IMSI_PLMN_NUM              (6)                     /* 用户配置的最多可支持的USIM/SIM卡的个数 */
#define NAS_MML_MAX_USER_CFG_EHPLMN_NUM                 (6)                     /* 用户配置的EHplmn的个数 */
#define NAS_MML_MAX_DISABLED_RAT_PLMN_NUM               (8)                     /* 止接入技术的PLMN ID的最大个数 */

#define NAS_MML_MAX_USER_OPLMN_VERSION_LEN               (8)               /* 用户配置的OPLMN版本号最大长度 */
#define NAS_MML_MAX_USER_OPLMN_IMSI_NUM                  (6)               /* 用户配置的OPLMN最多可支持的USIM/SIM卡的个数 */
#define NAS_MML_MAX_USER_CFG_OPLMN_NUM                   (256)             /* 用户配置的OPLMN的最大个数 */
#define NAS_MML_MAX_GROUP_CFG_OPLMN_NUM                  (50)              /* 单组允许配置的OPLMN的最大个数 */
#define NAS_MML_MAX_USER_CFG_OPLMN_GROUP_INDEX           (5)             /* 允许设置的最大组Index,支持设置(0-5)共6组 */
#define NAS_MML_OPLMN_WITH_RAT_UNIT_LEN                  (5)               /* 用户配置的带接入技术OPLMN基本单元长度，如6F61文件的基本长度单元为5 */
#define NAS_MML_MAX_USER_CFG_OPLMN_DATA_LEN              (1280)            /* 用户配置OPLMN的最大字节数*/
#define NAS_MML_MAX_GROUP_CFG_OPLMN_DATA_LEN             (250)             /* 单组配置OPLMN的最大字节数*/

/* 内部消息队列最大的消息长度为list搜网结果，不支持L时1028字节，支持L时1412字节  */
/* 内部消息最大的长度 */
#if (FEATURE_ON == FEATURE_LTE)
#define NAS_MML_MAX_INTERNAL_MSG_LEN                    (1500)
#else
#define NAS_MML_MAX_INTERNAL_MSG_LEN                    (1100)
#endif

#if (FEATURE_ON == FEATURE_LTE)
/* 内部消息队列最大的长度 */
#define NAS_MML_MAX_MSG_QUEUE_SIZE                      (36)
#else
/* 内部消息队列最大的长度 */
#define NAS_MML_MAX_MSG_QUEUE_SIZE                      (14)
#endif

/* 消息ID的无效值 */
#define NAS_MML_INVALID_INTERNAL_MSG_ID                 (0xFFFF)

/* USIM文件的FILE ID */
#define NAS_MML_READ_HPLMN_SEL_FILE_ID                  (0x6F62)
#define NAS_MML_READ_FORBIDDEN_PLMN_LIST_FILE_ID        (0x6F7B)
#define NAS_MML_READ_UPLMN_SEL_FILE_ID                  (0x6F60)
#define NAS_MML_READ_OPLMN_SEL_FILE_ID                  (0x6F61)
#define NAS_MML_READ_HPLMN_PERI_FILE_ID                 (0x6F31)
#define NAS_MML_READ_PLMN_SEL_FILE_ID                   (0x6F30)
#define NAS_MML_READ_MNC_LENGTH_FILE_ID                 (0x6FAD)
#define NAS_MML_READ_UST_FILE_ID                        (0x6F38)
#define NAS_MML_READ_EHPLMN_FILE_ID                     (0x6FD9)
#define NAS_MML_READ_EHPLMNPI_FILE_ID                   (0x6FDB)
#define NAS_MML_READ_LRPLMNSI_FILE_ID                   (0x6FDC)
#define NAS_MML_READ_CS_LOC_FILE_ID                     (0x6F7E)
#define NAS_MML_READ_PS_LOC_FILE_ID                     (0x6F73)
#define NAS_MML_READ_SIM_PS_LOC_FILE_ID                 (0x6f53)
#define NAS_MML_READ_CS_CKIK_FILE_ID                    (0x6F08)
#define NAS_MML_READ_PS_KEY_FILE_ID                     (0x6F09)
#define NAS_MML_READ_CS_KC_FILE_ID                      (0x6F20)
#define NAS_MML_READ_PS_KC_FILE_ID                      (0x6F52)
#define NAS_MML_READ_USIM_CS_KC_FILE_ID                 (0x4F20)
#define NAS_MML_READ_USIM_PS_KC_FILE_ID                 (0x4F52)

#define NAS_MML_READ_ACTING_HPLMN_FILE_ID               (0x4F34)

#define NAS_MML_UE_RFPOWER_INVALID                      (0x1fff)                /* UE RF Power的无效值 */
#define NAS_MML_UE_FREQ_INVALID                         (0xffff)                /* UE FREQ的无效值 */
#define NAS_MML_RSSI_UPPER_LIMIT                        (0)                     /* RSSI的上限 */
#define NAS_MML_RSSI_LOWER_LIMIT                        (-114)                  /* RSSI的下限 */

#define NAS_MML_RSSI_UNVALID                            (-114)                  /* RSSI的无效值 */


#define NAS_MML_GSM_CHANNEL_QUAL_INVALID                (99)                    /* G模下信道质量的无效值 */
#define NAS_MML_UTRA_BLER_INVALID                       (99)                    /* W下误码率的无效值 */
#define NAS_MML_RSCP_UPPER_LIMIT                        (0)                     /* RSCP的上限 */
#define NAS_MML_RSCP_LOWER_LIMIT                        (-140)                  /* RSCP的下限 */
#define NAS_MML_UTRA_RSCP_UNVALID                       (-140)                  /* RSCP的无效值 */
#define NAS_MML_LOWER_RSSI_LEVEL                        (0)                     /* RSSI的等级 */

/* 注册相关信息 */
#define NAS_MML_T3212_INFINITE_TIMEOUT_VALUE            (0)
#define NAS_MML_CS_INVALID_DRX_LEN                      (0)
#define NAS_MML_PS_INVALID_DRX_LEN                      (0)

/* 系统配置相关信息 */
#define NAS_MML_LTE_ALL_BAND_SUPPORTED                  (0xFFFFFFFF)
#define NAS_MML_LTE_ALL_BAND_INVALID                    (0x00)

/* 小区最大数量 */
#define NAS_MML_AT_CELL_MAX_NUM                         (8)

#define NAS_MML_ADDITIONAL_OLD_RAI_IEI_LEN              (6)                     /* ADDITIONAL OLD RAI 选项的IEI的长度值 */

#define NAS_MML_PC_RECUR_COMPRESS_DATA_LEN              (12000)                 /* PC回放压缩后的码流长度，单位：字节 */

#define NAS_MML_ROAM_ENABLED_MAX_MCC_NUMBER             (5)                     /* 允许漫游的国家码最大个数 */

#define NAS_MML_BG_SEARCH_REGARDLESS_MCC_NUMBER         (10)                     /* BG搜不考虑国家码的国家码最大个数 */
#define NAS_MML_SINGLE_DOMAIN_FAIL_ACTION_MAX_LIST      (5)                     /* 定制原因值最大列表 */

#define NAS_MML_MAX_PS_BEARER_NUM                       (11)                    /* 最大PS承载个数 */
#define NAS_MML_MIN_NSAPI                               (5)                     /* 对应NSAPI5 */
#define NAS_MML_MAX_NSAPI                               (15)                    /* 对应NSAPI15 */
#define NAS_MML_NSAPI_OFFSET                            (5)                     /* PDP激活上下文索引和nsapi*/
#define NAS_MML_EMERGENCY_NUM_MAX_LEN                   (46)                    /* Emergency Number information的最大字节数 */
#define NAS_MML_EMERGENCY_NUM_LIST_MAX_RECORDS          (16)                    /* Emergency Number Lists的最多个数         */
#define NAS_MML_MIN_EMERGENCY_NUM_LEN                   (3)
#define NAS_MML_MAX_EMERGENCY_NUM_LEN                   (48)
#define NAS_MML_MIN_EMERGENCY_NUM_INFO_LEN              (2)
#define NAS_MML_MAX_EMERGENCY_NUM_INFO_LEN              (42)                    /*  */



/* 调用DRV_SYSTEM_ERROR时, 如果需要保存其它FID的任务栈信息需将第24 Bit置位 */

#define NAS_MML_SAVE_OTHER_FID_TCB_FLAG                 (1<<24)

#define NAS_MML_TAC_LOW_BYTE_INVALID                    (0xFF)
#define NAS_MML_TAC_HIGH_BYTE_INVALID                   (0xFE)


/* 6F31的文件长度 */
#define NAS_MML_HPLMN_PERI_FILE_LEN                     (1)

#define NAS_MML_MAX_SYS_SUBMODE_NUM                     (9)

#define NAS_MML_MAX_IMSI_FORBIDDEN_LIST_NUM             (16)
#define NAS_MML_MAX_RAT_FORBIDDEN_LIST_NUM              (8)


#define NAS_MML_PS_DEFAULT_DRX_SPLIT_PG_CYCLE_CODE      (32)                    /* 默认初始值，物理层要求非零参数 */
#define NAS_MML_PS_DEFAULT_DRX_SPLIT_ON_CCCH            (0)                     /* 默认不支持CCCH上的Split pg cycle */
#define NAS_MML_PS_DEFAULT_NON_DRX_TIMER                (0)                     /* 默认初始值 */
#define NAS_MML_PS_UE_UTRAN_DEFAULT_DRX_LEN             (0)                     /* 默认初始值 */
#define NAS_MML_PS_UE_LTE_DEFAULT_DRX_LEN               (0)                     /* 默认初始值 */
#define NAS_MML_NON_DRX_TIMER_MAX                       (7)                     /* 有效值为0-7 */

#define NAS_MML_MNC_LENGTH_THREE_BYTES_IN_IMSI          (3)                     /*卡中读取的指示MNC的长度，默认为3*/

#define NAS_MML_SIGN_REPORT_MSG_TYPE                     (7)                    /* nas设置RRMM_AT_MSG_REQ时可以一次设置多种类型
                                                                                   RRC_NAS_AT_CELL_SIGN|RRC_NAS_AT_CELL_ID|RRC_NAS_AT_CELL_BLER*/
#define NAS_MML_NET_RAT_TYPE_INVALID                    (0xFF)

#if (FEATURE_ON == FEATURE_LTE)
#define NAS_MML_ENABLE_LTE_TIMER_DEFAULT_LEN            (5 * 60 * 1000)         /* 默认enable lte timer时长5分钟 */
#endif

#define NAS_MML_SIXTY_SECOND                            (60)                    /* 60秒 */


#define NAS_MML_PS_TRANSFER_FROM_MODEM1_TO_MODEM0           (1)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum NAS_MML_REBOOT_MOD_ID_ENUM
{
    NAS_REBOOT_MOD_ID_MML   = 0X60000000,
    NAS_REBOOT_MOD_ID_BUTT  = 0X6FFFFFFF
};
typedef VOS_UINT32 NAS_MML_REBOOT_MOD_ID_ENUM_UINT32;


/*****************************************************************************
 枚举名    : NAS_MML_SIM_TYPE_ENUM_UINT8
 枚举说明  : 当前SIM卡的类型,SIM卡或USIM卡
 1.日    期   : 2011年5月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
enum NAS_MML_SIM_TYPE_ENUM
{
    NAS_MML_SIM_TYPE_SIM                , /* SIM卡 */
    NAS_MML_SIM_TYPE_USIM               , /* USIM卡 */
    NAS_MML_SIM_TYPE_BUTT
};
typedef VOS_UINT8 NAS_MML_SIM_TYPE_ENUM_UINT8;


/*****************************************************************************
 枚举名    : NAS_MML_SIM_UE_OPER_MODE_ENUM_UINT8
 结构说明  : SIM卡中UE的模式，正常模式或测试模式
  1.日    期   : 2011年5月28日
    作    者   : zhoujun 40661
    修改内容   : 新建
*****************************************************************************/
enum NAS_MML_SIM_UE_OPER_MODE_ENUM
{
    NAS_MML_SIM_UE_NORMAL_MODE                = 0x00,                           /* normal operation*/
    NAS_MML_SIM_UE_NORAML_SPECIFIC_MODE       = 0x01,                           /* normal operation + specific facilities*/
    NAS_MML_SIM_UE_MAINTENACE_MODE            = 0x02,                           /* maintenance (off line)*/
    NAS_MML_SIM_UE_CELLTEST_MODE              = 0x04,                           /* cell test operation*/
    NAS_MML_SIM_UE_APPROVAL_MODE              = 0x80,                           /* type approval operations*/
    NAS_MML_SIM_UE_APPROVAL_SPECIFIC_MODE     = 0x81,                           /* type approval operations + specific facilities.*/
    NAS_MML_SIM_UE_OPER_MODE_BUTT
};
typedef VOS_UINT8 NAS_MML_SIM_UE_OPER_MODE_ENUM_UINT8;

/*****************************************************************************
 枚举名    : NAS_MML_NET_RAT_TYPE_ENUM_UINT8
 结构说明  : 当前的接入技术
 1.日    期   : 2011年5月28日
   作    者   : zhoujun 40661
   修改内容   : 新建

 2.日    期   : 2011年7月9日
   作    者   : zhoujun 40661
   修改内容   : 增加LTE的类型
*****************************************************************************/
enum NAS_MML_NET_RAT_TYPE_ENUM
{
    NAS_MML_NET_RAT_TYPE_GSM,           /* GSM接入技术 */
    NAS_MML_NET_RAT_TYPE_WCDMA,         /* WCDMA接入技术 */
    NAS_MML_NET_RAT_TYPE_LTE,           /* LTE接入技术 */
    NAS_MML_NET_RAT_TYPE_BUTT           /* 无效的接入技术 */
};
typedef VOS_UINT8 NAS_MML_NET_RAT_TYPE_ENUM_UINT8;


enum NAS_MML_3GPP2_RAT_TYPE_ENUM
{
    NAS_MML_3GPP2_RAT_TYPE_CDMA1X,
    NAS_MML_3GPP2_RAT_TYPE_HRPD,
    NAS_MML_3GPP2_RAT_TYPE_BUTT
};
typedef VOS_UINT8 NAS_MML_3GPP2_RAT_TYPE_ENUM_UINT8;


enum NAS_MML_PLATFORM_RAT_TYPE_ENUM
{
    NAS_MML_PLATFORM_RAT_TYPE_GSM,           /* GSM接入技术 */
    NAS_MML_PLATFORM_RAT_TYPE_WCDMA,         /* WCDMA接入技术 */
    NAS_MML_PLATFORM_RAT_TYPE_TD_SCDMA,      /* TDSCDMA接入技术 */
    NAS_MML_PLATFORM_RAT_TYPE_LTE,           /* LTE接入技术 */
    NAS_MML_PLATFORM_RAT_TYPE_BUTT           /* 无效的接入技术 */
};
typedef VOS_UINT8 NAS_MML_PLATFORM_RAT_TYPE_ENUM_UINT8;
/*****************************************************************************
 枚举名    : NAS_MML_NET_MODE_ENUM_UINT8
 结构说明  : 网络模式
 1.日    期   : 2011年5月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
enum NAS_MML_NET_MODE_ENUM
{
    NAS_MML_NET_MODE_INVALID,                                                   /* 网络模式非法 */
    NAS_MML_NET_MODE_I,                                                         /* 网络模式I */
    NAS_MML_NET_MODE_II,                                                        /* 网络模式II */
    NAS_MML_NET_MODE_III,                                                       /* 网络模式III */
    NAS_MML_NET_MODE_BUTT                                                       /* 无效的网络模式 */
};
typedef VOS_UINT8 NAS_MML_NET_MODE_ENUM_UINT8;
enum NAS_MML_3GPP_REL_ENUM
{
    NAS_MML_3GPP_REL_R4,                                                        /* R4 */
    NAS_MML_3GPP_REL_R5,                                                        /* R5 */
    NAS_MML_3GPP_REL_R6,                                                        /* R6 */
    NAS_MML_3GPP_REL_R7,                                                        /* R7 */
	NAS_MML_3GPP_REL_R8,                                                        /* R8 */
    NAS_MML_3GPP_REL_R9,                                                        /* R9 */
    NAS_MML_3GPP_REL_R10,                                                       /* R10 */
	NAS_MML_3GPP_REL_BUTT                                                       /* 无效值 */
};
typedef VOS_UINT8 NAS_MML_3GPP_REL_ENUM_UINT8;

/*****************************************************************************
 枚举名    : NAS_MML_MSC_REL_ENUM_UINT8
 结构说明  : MS上报MSC的版本
 1.日    期   : 2011年6月27日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
enum NAS_MML_MSC_REL_ENUM
{
    NAS_MML_MSC_REL_R97,                                                        /* Rel.97（终端向PS/CS域核心网上报自己是Rel.97版本的终端 */
    NAS_MML_MSC_REL_R99,                                                        /* Rel.99（终端向PS/CS域核心网上报自己是Rel.99版本的终端） */
    NAS_MML_MSC_REL_AUTO,                                                       /* Auto（终端根据接入网获取的信息向网络上报自己的协议等级，默认值） */
    NAS_MML_MSC_REL_BUTT
};
typedef VOS_UINT8 NAS_MML_MSC_REL_ENUM_UINT8;

/*****************************************************************************
 枚举名    : NAS_MML_SGSN_REL_ENUM_UINT8
 结构说明  : MS上报SGSN的版本
 1.日    期   : 2011年6月27日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
enum NAS_MML_SGSN_REL_ENUM
{
    NAS_MML_SGSN_REL_R97,                                                       /* Rel.97（终端向PS/CS域核心网上报自己是Rel.97版本的终端 */
    NAS_MML_SGSN_REL_R99,                                                       /* Rel.99（终端向PS/CS域核心网上报自己是Rel.99版本的终端） */
    NAS_MML_SGSN_REL_AUTO,                                                      /* Auto（终端根据接入网获取的信息向网络上报自己的协议等级，默认值） */
    NAS_MML_SGSN_REL_BUTT
};
typedef VOS_UINT8 NAS_MML_SGSN_REL_ENUM_UINT8;

/*****************************************************************************
 枚举名    : NAS_MML_TIN_TYPE_ENUM_UINT8
 结构说明  : TIN的类型
  1.日    期   : 2011年6月28日
    作    者   : zhoujun 40661
    修改内容   : 新增加操作类型
*****************************************************************************/
enum NAS_MML_TIN_TYPE_ENUM
{
    NAS_MML_TIN_TYPE_PTMSI,                                                     /* TIN类型为PTMSI */
    NAS_MML_TIN_TYPE_GUTI,                                                      /* TIN类型为GUTI */
    NAS_MML_TIN_TYPE_RAT_RELATED_TMSI,                                          /* TIN类型为RAT_RELATED_TMSI */
    NAS_MML_TIN_TYPE_INVALID,                                                   /* TIN类型无效 */
    NAS_MML_TIN_TYPE_BUTT
};
typedef VOS_UINT8 NAS_MML_TIN_TYPE_ENUM_UINT8;

/*****************************************************************************
 枚举名    : NAS_MML_LAST_RPLMN_SEL_IND_ENUM_UINT8
 结构说明  :  EFLRPLMNSI文件的内容This EF contains an indication to the ME for
              the selection of the RPLMN or the home network at switch on, or
              following recovery from lack of coverage.Last RPLMN Selection Indication
  1.日    期   : 2011年6月28日
    作    者   : zhoujun 40661
    修改内容   : 新增加操作类型
*****************************************************************************/
enum NAS_MML_LAST_RPLMN_SEL_IND_ENUM
{
    NAS_MML_LAST_RPLMN_SEL_IND_RPLMN,                                           /* The UE shall attempt registration on the last RPLMN*/
    NAS_MML_LAST_RPLMN_SEL_IND_HPLMN,                                           /* The UE shall attempt registration on the home network */
    NAS_MML_LAST_RPLMN_SEL_IND_BUTT
};
typedef VOS_UINT8 NAS_MML_LAST_RPLMN_SEL_IND_ENUM_UINT8;

/*****************************************************************************
 枚举名    : NAS_MML_EHPLMN_PRESENT_IND_ENUM_UINT8
 结构说明  :  EF_EHPLMNPI文件的内容This EF contains an indication to the ME for
              the presentation of the available EHPLMN(s) during I-WLAN selection procedures.
  1.日    期   : 2011年6月28日
    作    者   : zhoujun 40661
    修改内容   : 新增加操作类型
*****************************************************************************/
enum NAS_MML_EHPLMN_PRESENT_IND_ENUM
{
    /* '00' - No preference for the display mode
    -   '01' - Display the highest-priority available EHPLMN only
    -   '02' - Display all the available EHPLMNs*/

    NAS_MML_EHPLMN_PRESENT_IND_NO_PREF  = 0x00,
    NAS_MML_EHPLMN_PRESENT_IND_HIGHEST  = 0x01,
    NAS_MML_EHPLMN_PRESENT_IND_ALL      = 0x02,
    NAS_MML_EHPLMN_PRESENT_IND_BUTT
};
typedef VOS_UINT8 NAS_MML_EHPLMN_PRESENT_IND_ENUM_UINT8;

/*****************************************************************************
 枚举名    : NAS_MML_ROAM_CAPABILITY_ENUM_UINT8
 结构说明  :  记录是漫游能力
  1.日    期   : 2011年6月28日
    作    者   : zhoujun 40661
    修改内容   : 新增加操作类型
*****************************************************************************/
enum NAS_MML_ROAM_CAPABILITY_ENUM
{
    NAS_MML_ROAM_NATION_ON_INTERNATION_ON       = 0,                            /* 开启国内国际漫游 */
    NAS_MML_ROAM_NATION_ON_INTERNATION_OFF      = 1,                            /* 开启国内漫游，关闭国际漫游 */
    NAS_MML_ROAM_NATION_OFF_INTERNATION_ON      = 2,                            /* 关闭国内漫游，开启国际漫游 */
    NAS_MML_ROAM_NATION_OFF_INTERNATION_OFF     = 3,                            /* 关闭国内国际漫游 */
    NAS_MML_ROAM_BUTT
};
typedef VOS_UINT8 NAS_MML_ROAM_CAPABILITY_ENUM_UINT8;

/*****************************************************************************
 枚举名    : NAS_MML_MS_MODE_ENUM_UINT8
 结构说明  :  记录手机模式
  1.日    期   : 2011年6月28日
    作    者   : zhoujun 40661
    修改内容   : 新增加操作类型
*****************************************************************************/
enum NAS_MML_MS_MODE_ENUM
{
    NAS_MML_MS_MODE_PS_CS,                                                      /* CS和PS都支持 */
    NAS_MML_MS_MODE_PS_ONLY,                                                    /* 仅支持PS域 */
    NAS_MML_MS_MODE_CS_ONLY,                                                    /* 仅支持CS域 */
    NAS_MML_MS_MODE_NULL,                                                       /* 不支持任何域 */
    NAS_MML_MS_MODE_BUTT
};
typedef VOS_UINT8 NAS_MML_MS_MODE_ENUM_UINT8;



enum NAS_MML_LTE_CS_SERVICE_CFG_ENUM
{
    NAS_MML_LTE_SUPPORT_CSFB_AND_SMS_OVER_SGS = 1,           /* 支持cs fallback和sms over sgs*/
    NAS_MML_LTE_SUPPORT_SMS_OVER_SGS_ONLY,                   /* 支持sms over sgs only*/
    NAS_MML_LTE_SUPPORT_1XCSFB,                              /* 支持1XCSFB */
    NAS_MML_LTE_SUPPORT_BUTT
};
typedef VOS_UINT8 NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8;
enum NAS_MML_LTE_UE_OPERATION_MODE_ENUM
{
    NAS_MML_LTE_UE_OPERATION_MODE_PS_1,              /* operation mode 为ps mode1 */
    NAS_MML_LTE_UE_OPERATION_MODE_PS_2,              /* operation mode 为ps mode2 */
    NAS_MML_LTE_UE_OPERATION_MODE_CS_PS_1,           /* operation mode 为cs ps mode1 */
    NAS_MML_LTE_UE_OPERATION_MODE_CS_PS_2,           /* operation mode 为cs ps mode2 */
    NAS_MML_LTE_UE_OPERATION_MODE_BUTT
};
typedef VOS_UINT8 NAS_MML_LTE_UE_OPERATION_MODE_ENUM_UINT8;


enum NAS_MML_LTE_UE_USAGE_SETTING_ENUM
{
    NAS_MML_LTE_UE_USAGE_SETTING_VOICE_CENTRIC,      /* 语音中心 */
    NAS_MML_LTE_UE_USAGE_SETTING_DATA_CENTRIC,       /* 数据中心 */
    NAS_MML_LTE_UE_USAGE_SETTIN_BUTT
};

typedef VOS_UINT8 NAS_MML_LTE_UE_USAGE_SETTING_ENUM_UINT8;


enum NAS_MML_VOICE_DOMAIN_PREFERENCE_ENUM
{
    NAS_MML_CS_VOICE_ONLY,                                /* 仅支持CS语音 */
    NAS_MML_IMS_PS_VOICE_ONLY,                            /* 仅支持IMS PS语音 */
    NAS_MML_CS_VOICE_PREFER_IMS_PS_VOICE_AS_SECONDARY,    /* CS语音优先，IMS PS语音次之 */
    NAS_MML_IMS_PS_VOICE_PREFER_CS_VOICE_AS_SECONDARY,    /* IMS PS语音优先，CS语音次之 */
    NAS_MML_VOICE_DOMAIN_PREFER_BUTT
};

typedef VOS_UINT8 NAS_MML_VOICE_DOMAIN_PREFERENCE_ENUM_UINT8;



enum NAS_MML_CSFB_SERVICE_STATUS_ENUM
{
    NAS_MML_CSFB_SERVICE_STATUS_NOT_EXIST,                     /* 目前不存在CSFB流程 */
    NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_CC_EXIST,            /* 目前在CSFB的MO的正常业务流程中 */
    NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_SS_EXIST,            /* 目前在CSFB的MO的正常业务流程中 */
    NAS_MML_CSFB_SERVICE_STATUS_MO_EMERGENCY_EXIST,            /* 目前在CSFB的紧急呼流程中 */
    NAS_MML_CSFB_SERVICE_STATUS_MT_EXIST,                      /* 目前在CSFB的MT流程中 */
    NAS_MML_CSFB_SERVICE_STATUS_BUTT
};

typedef VOS_UINT8 NAS_MML_CSFB_SERVICE_STATUS_ENUM_UINT8;
enum NAS_MML_ADDITION_UPDATE_RSLT_INFO_ENUM
{
    NAS_MML_ADDITION_UPDATE_RSLT_NO_ADDITION_INFO = 0,                          /* 没有额外的信息 */
    NAS_MML_ADDITION_UPDATE_RSLT_CSFB_NOT_PREFERRED,                            /* CS Fallback not preferred*/
    NAS_MML_ADDITION_UPDATE_RSLT_SMS_ONLY,                                      /* 网侧只支持短信  */
    NAS_MML_ADDITION_UPDATE_RSLT_BUTT
};

typedef VOS_UINT8 NAS_MML_ADDITION_UPDATE_RSLT_INFO_ENUM_UINT8;
enum NAS_MML_TIMER_INFO_ENUM
{
    NAS_MML_TIMER_STOP = 0,
    NAS_MML_TIMER_START,
    NAS_MML_TIMER_EXP,
    NAS_MML_TIMER_BUTT
};
typedef VOS_UINT8 NAS_MML_TIMER_INFO_ENUM_UINT8;


enum NAS_MML_PS_BEARER_STATE_ENUM
{
    NAS_MML_PS_BEARER_STATE_INACTIVE = 0,                                       /* 上下文去激活状态 */
    NAS_MML_PS_BEARER_STATE_ACTIVE,                                             /* 上下文激活状态 */
    NAS_MML_PS_BEARER_STATE_BUTT
};
typedef VOS_UINT8 NAS_MML_PS_BEARER_STATE_ENUM_UINT8;


enum NAS_MML_PS_BEARER_ISR_ENUM
{
    NAS_MML_PS_BEARER_EXIST_BEFORE_ISR_ACT,                                     /* 上下文激活在ISR激活前 */
    NAS_MML_PS_BEARER_EXIST_AFTER_ISR_ACT,                                      /* 上下文激活在ISR激活后 */
    NAS_MML_PS_BEARER_EXIST_BUTT
};
typedef VOS_UINT8 NAS_MML_PS_BEARER_ISR_ENUM_UINT8;




enum NAS_MML_REG_FAIL_CAUSE_ENUM
{

    /* UE跟网侧交互，协议明确给出交互结果的原因值，扩展范围为[0,255], */
    NAS_MML_REG_FAIL_CAUSE_NULL                                     = 0  ,      /* NULL（无原因）                          */
    NAS_MML_REG_FAIL_CAUSE_IMSI_UNKNOWN_IN_HLR                      = 2  ,      /* IMSI unknown in HLR                      */
    NAS_MML_REG_FAIL_CAUSE_ILLEGAL_MS                               = 3  ,      /* Illegal MS                               */
    NAS_MML_REG_FAIL_CAUSE_IMSI_UNKNOWN_IN_VLR                      = 4  ,      /* IMSI unknown in VLR                      */
    NAS_MML_REG_FAIL_CAUSE_IMSI_NOT_ACCEPTED                        = 5  ,      /* IMSI not accepted                        */
    NAS_MML_REG_FAIL_CAUSE_ILLEGAL_ME                               = 6  ,      /* Illegal ME                               */
    NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW                      = 7  ,      /* GPRS services not allowed                */
    NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_AND_NON_GPRS_SERV_NOT_ALLOW    = 8  ,      /* GPRS services and non-GPRS               */
                                                                                /* services not allowed                     */
    NAS_MML_REG_FAIL_CAUSE_MS_ID_NOT_DERIVED                        = 9  ,      /* MS identity cannot be derived            */
                                                                                /* by the network                           */
    NAS_MML_REG_FAIL_CAUSE_IMPLICIT_DETACHED                        = 10 ,      /* Implicitly detached                      */
    NAS_MML_REG_FAIL_CAUSE_PLMN_NOT_ALLOW                           = 11 ,      /* PLMN not allowed                         */
    NAS_MML_REG_FAIL_CAUSE_LA_NOT_ALLOW                             = 12 ,      /* Location area not allowed                */
    NAS_MML_REG_FAIL_CAUSE_ROAM_NOT_ALLOW                           = 13 ,      /* Roaming not allowed in                   */
                                                                                /* this location area                       */
    NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW_IN_PLMN              = 14 ,      /* GPRS services not allowed in this PLMN   */
    NAS_MML_REG_FAIL_CAUSE_NO_SUITABL_CELL                          = 15 ,      /* No Suitable Cells In Location Area       */
    NAS_MML_REG_FAIL_CAUSE_MSC_UNREACHABLE                          = 16 ,      /* MSC temporarily not reachable            */
    NAS_MML_REG_FAIL_CAUSE_NETWORK_FAILURE                          = 17 ,      /* Network failure                          */

    NAS_MML_REG_FAIL_CAUSE_CS_DOMAIN_NOT_AVAILABLE                  = 18,       /* CS domain not available                  */

    NAS_MML_REG_FAIL_CAUSE_ESM_FAILURE                              = 19 ,      /* ESM failure                              */
    NAS_MML_REG_FAIL_CAUSE_MAC_FAILURE                              = 20 ,      /* MAC failure                              */
    NAS_MML_REG_FAIL_CAUSE_SYNCH_FAILURE                            = 21 ,      /* Synch failure                            */
    NAS_MML_REG_FAIL_CAUSE_PROCEDURE_CONGESTION                     = 22 ,      /* Congestion                               */
    NAS_MML_REG_FAIL_CAUSE_GSM_AUT_UNACCEPTABLE                     = 23 ,      /* GSM authentication unacceptable          */
    NAS_MML_REG_FAIL_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG              = 25,       /* Not authorized for this CSG              */
    NAS_MML_REG_FAIL_CAUSE_SERV_OPT_NOT_SUPPORT                     = 32 ,      /* Service option not supported             */
    NAS_MML_REG_FAIL_CAUSE_REQ_SERV_OPT_NOT_SUBSCRIBE               = 33 ,      /* Requested service option not subscribed  */
    NAS_MML_REG_FAIL_CAUSE_SERV_OPT_OUT_OF_ORDER                    = 34 ,      /* Service option temporarily out of order  */
    NAS_MML_REG_FAIL_CAUSE_CALL_CANNOT_IDENTIFY                     = 38 ,      /* Call cannot be identified                */
    NAS_MML_REG_FAIL_CAUSE_CS_DOMAIN_TEMP_NOT_AVAILABLE             = 39 ,      /* NAS_EMM_CAUSE_CS_DOMAIN_TEMP_NOT_AVAILABLE */
    NAS_MML_REG_FAIL_CAUSE_NO_PDP_CONTEXT_ACT                       = 40 ,      /* No PDP context activated                 */
    NAS_MML_REG_FAIL_CAUSE_RETRY_UPON_ENTRY_CELL_MIN                = 48 ,      /* retry upon entry into a new cell (min)   */
    NAS_MML_REG_FAIL_CAUSE_RETRY_UPON_ENTRY_CELL                    = 60 ,      /* retry upon entry into a new cell         */
    NAS_MML_REG_FAIL_CAUSE_RETRY_UPON_ENTRY_CELL_MAX                = 63 ,      /* retry upon entry into a new cell (max)   */
    NAS_MML_REG_FAIL_CAUSE_SEMANTICALLY_INCORRECT_MSG               = 95 ,      /* Semantically incorrect message           */
    NAS_MML_REG_FAIL_CAUSE_INVALID_MANDATORY_INF                    = 96 ,      /* Invalid mandatory information            */
    NAS_MML_REG_FAIL_CAUSE_MSG_NONEXIST_NOTIMPLEMENTE               = 97 ,      /* Message type non-existent or             */
                                                                                /* not implemented                          */

    NAS_MML_REG_FAIL_CAUSE_MSG_TYPE_NOT_COMPATIBLE                  = 98 ,      /* Message type not compatible with the     */
                                                                                /* protocol state                           */

    NAS_MML_REG_FAIL_CAUSE_IE_NONEXIST_NOTIMPLEMENTED               = 99 ,      /* Information element non-existent or      */
                                                                                /* not implemented                          */
    NAS_MML_REG_FAIL_CAUSE_CONDITIONAL_IE_ERROR                     = 100,      /* Conditional IE error                     */
    NAS_MML_REG_FAIL_CAUSE_MSG_NOT_COMPATIBLE                       = 101,      /* Message not compatible with              */
                                                                                /* protocol state                           */
    NAS_MML_REG_FAIL_CAUSE_PROTOCOL_ERROR                           = 111,      /* Protocol error, unspecified              */

    /* 网侧给出交互结果的原因值，扩展范围为[256,300], */
    NAS_MML_REG_FAIL_CAUSE_AUTH_REJ                                 = 256,      /* 鉴权被拒                                */
    NAS_MML_REG_FAIL_CAUSE_ACCESS_BARRED                            = 257,      /* access barred                            */
    NAS_MML_REG_FAIL_CAUSE_COMB_REG_CS_FAIL_OTHER_CAUSE             = 258,      /* other cause expcept #2,#16,#17 and #22 */

    /* UE尝试跟网侧进行交互失败的拒绝原因值，扩展范围为[301,400] */
    NAS_MML_REG_FAIL_CAUSE_TIMER_TIMEOUT                            = 301,      /* Timer timeout                            */
    NAS_MML_REG_FAIL_CAUSE_RR_CONN_EST_FAIL                         = 302,      /* RR connection establish failure          */
    NAS_MML_REG_FAIL_CAUSE_RR_CONN_FAIL                             = 303,      /* RR connection failure                    */
    NAS_MML_REG_FAIL_CAUSE_RR_CONN_ABORT                            = 304,      /* RR connection abort                      */

    /* UE未跟网侧进行交互失败的拒绝原因值，扩展范围为[401,500] */
    NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE                              = 401,      /* Other cause                              */
    NAS_MML_REG_FAIL_CAUSE_MS_CFG_DOMAIN_NOT_SUPPORT                = 402,      /* UE设置为当前域不支持                   */
    NAS_MML_REG_FAIL_CAUSE_FORBIDDEN_PLMN                           = 403,      /* 以及当前域在Forbidden信息中            */
    NAS_MML_REG_FAIL_CAUSE_FORBIDDEN_LA_FOR_REG_PRVS_SVS            = 404,      /* 因被 #12 注册被拒                      */
    NAS_MML_REG_FAIL_CAUSE_FORBIDDEN_LA_FOR_ROAM                    = 405,      /* 因被 #13，#15注册被拒                  */
    NAS_MML_REG_FAIL_CAUSE_FORBIDDEN_PLMN_FOR_GPRS                  = 406,      /* 因被 #14 注册被拒                      */
    NAS_MML_REG_FAIL_CAUSE_SIM_INVALID                              = 407,      /* 因卡无效                                */
    NAS_MML_REG_FAIL_CAUSE_NTDTH_IMSI                               = 408,      /* 网络detach imsi的原因值                 */
    NAS_MML_REG_FAIL_CAUSE_SERVING_CELL_DOMAIN_NOT_SUPPORT          = 409,      /* 因网侧信息目前不支持该域               */
    NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE_REG_MAX_TIMES                = 410,

    NAS_MML_REG_FAIL_CAUSE_PS_ATTACH_NOT_ALLOW                      = 411,      /* ps注册标记不允许注册                   */
    NAS_MML_REG_FAIL_CAUSE_ESM_BEAR_FAILURE                         = 412,      /* ESM承载建立失败 */
    NAS_MML_REG_FAIL_CAUSE_MO_DETACH_FAILURE                        = 413,      /* 注册过程中用户发起detach，注册失败    */
    NAS_MML_REG_FAIL_CAUSE_MT_DETACH_FAILURE                        = 414,      /* 注册过程中收到网侧detach消息，注册失败 */
    NAS_MML_REG_FAIL_CAUSE_T3402_RUNNING                            = 415,      /* 注册失败原因T3402定时器在运行          */

    NAS_MML_REG_FAIL_CAUSE_DELAY_CS_IN_TRANS_LAU                    = 416,      /* CS通信过程中延迟进行LAU         */

    NAS_MML_REG_FAIL_CAUSE_T3212_RUNNING                            = 417,      /* 注册失败原因T3212定时器在运行 */

    /* UE未跟网侧进行交互成功的原因值 */
    NAS_MML_REG_FAIL_CAUSE_LOCAL_NULL                               = 500  ,      /* SUCC（无原因）                          */

    NAS_MML_REG_FAIL_CAUSE_BUTT
} ;

typedef VOS_UINT16 NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16;

enum NAS_MML_SIM_AUTH_FAIL_ENUM
{
    NAS_MML_SIM_AUTH_FAIL_NULL                  = 0,
    NAS_MML_SIM_AUTH_FAIL_GSM_AUT_UNACCEPTABLE  = 1,
    NAS_MML_SIM_AUTH_FAIL_MAC_FAILURE           = 2,
    NAS_MML_SIM_AUTH_FAIL_SYNC_FAILURE          = 3,
    NAS_MML_SIM_AUTH_FAIL_UMTS_OTHER_FAILURE    = 4,
    NAS_MML_SIM_AUTH_FAIL_GSM_OTHER_FAILURE     = 5,
    NAS_MML_SIM_AUTH_FAIL_BUTT
};
typedef VOS_UINT16 NAS_MML_SIM_AUTH_FAIL_ENUM_UINT16;

/*****************************************************************************
 枚举名    : NAS_MML_PLMN_FORBID_TYPE_ENUM_UINT8
 结构说明  : forbid plmn的类型
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
enum NAS_MML_PLMN_FORBID_TYPE_ENUM
{
    NAS_MML_PLMN_FORBID_NULL                = 0x00,    /* 网络未被禁止*/                                                      /* 网络模式I */
    NAS_MML_PLMN_FORBID_PLMN                = 0x01,    /* Plmn被禁止*/                                                          /* 网络模式I */
    NAS_MML_PLMN_FORBID_PLMN_FOR_GPRS       = 0x02,    /* Plmn for gprs被禁止*/                                                     /* 网络模式II */
    NAS_MML_PLMN_FORBID_ROAM_LA             = 0x04,    /* Plmn for roam LAI被禁止*/
    NAS_MML_PLMN_FORBID_REG_LA              = 0x08,    /* Plmn for reg LAI被禁止*/
    NAS_MML_PLMN_FORBID_BUTT                           /* 无效的网络模式 */
};
typedef VOS_UINT8 NAS_MML_PLMN_FORBID_TYPE_ENUM_UINT8;


/*****************************************************************************
 枚举名    : NAS_MML_ROUTING_UPDATE_STATUS_ENUM_UINT8
 结构说明  : routing update status
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新建
    0   0   0   :   updated.
    0   0   1   :   not updated.
    0   1   0   :   PLMN not allowed.
    0   1   1   :   Routing Area not allowed.
*****************************************************************************/
enum NAS_MML_ROUTING_UPDATE_STATUS_ENUM
{
    NAS_MML_ROUTING_UPDATE_STATUS_UPDATED,
    NAS_MML_ROUTING_UPDATE_STATUS_NOT_UPDATED,
    NAS_MML_ROUTING_UPDATE_STATUS_PLMN_NOT_ALLOWED,
    NAS_MML_ROUTING_UPDATE_STATUS_ROUTING_AREA_NOT_ALLOWED,
    NAS_MML_ROUTING_UPDATE_STATUS_BUTT
};
typedef VOS_UINT8 NAS_MML_ROUTING_UPDATE_STATUS_ENUM_UINT8;

/*****************************************************************************
 枚举名    : NAS_MML_LOCATION_UPDATE_STATUS_ENUM_UINT8
 结构说明  : Location update status
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新建
    0   0   0   :   updated.
    0   0   1   :   not updated.
    0   1   0   :   PLMN not allowed.
    0   1   1   :   Location Area not allowed.
*****************************************************************************/
enum NAS_MML_LOCATION_UPDATE_STATUS_ENUM
{
    NAS_MML_LOCATION_UPDATE_STATUS_UPDATED,
    NAS_MML_LOCATION_UPDATE_STATUS_NOT_UPDATED,
    NAS_MML_LOCATION_UPDATE_STATUS_PLMN_NOT_ALLOWED,
    NAS_MML_LOCATION_UPDATE_STATUS_LOCATION_AREA_NOT_ALLOWED,
    NAS_MML_LOCATION_UPDATE_STATUS_BUTT
};
typedef VOS_UINT8 NAS_MML_LOCATION_UPDATE_STATUS_ENUM_UINT8;


/*****************************************************************************
 枚举名    : NAS_MML_REG_RESULT_ENUM_U8
 结构说明  : 注册结果
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
enum NAS_MML_REG_RESULT_ENUM
{
    NAS_MML_REG_RESULT_SUCCESS,                          /* 注册成功 */
    NAS_MML_REG_RESULT_FAILURE,                          /* 注册失败 */
    NAS_MML_REG_RESULT_ACCESS_BAR,                       /* 接入禁止 */
    NAS_MML_REG_RESULT_TIMER_EXP,                        /* 网络无响应 */
    NAS_MML_REG_RESULT_CN_REJ,                           /* 被网络拒绝 */
    NAS_MML_REG_RESULT_FORBID,                           /*禁止信息不触发ATTACH*/
    NAS_MML_REG_RESULT_AUTH_REJ,                         /* 鉴权失败 */
    NAS_MML_REG_RESULT_BUTT
};
typedef VOS_UINT8 NAS_MML_REG_RESULT_ENUM_UINT8;
enum NAS_MML_REG_STATUS_ENUM
{
    NAS_MML_REG_NOT_REGISTERED_NOT_SEARCH,      /* 未注册未搜索 */
    NAS_MML_REG_REGISTERED_HOME_NETWORK,        /* 注册，注册在 HOME PLMN */
    NAS_MML_REG_NOT_REGISTERED_SEARCHING,       /* 未注册，正在搜索PLMN或者正在附着过程 */
    NAS_MML_REG_REGISTERED_DENIED,              /* 注册被拒绝，当前用户不可以启动注册，启动服务被禁止 */
    NAS_MML_REG_UNKNOWN,                        /* 其余情况 */
    NAS_MML_REG_REGISTERED_ROAM,                /* 注册，注册在非 HOME PLMN */
    NAS_MML_REG_STATUS_BUTT
};
typedef VOS_UINT8 NAS_MML_REG_STATUS_ENUM_UINT8;

/*****************************************************************************
 枚举名    : NAS_MML_RRC_INTEGRITY_PROTECT_ENUM
 结构说明  : 注册结果
 1.日    期   : 2011年7月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
enum NAS_MML_RRC_INTEGRITY_PROTECT_ENUM
{
    NAS_MML_RRC_INTEGRITY_PROTECT_DEACTIVE         = 0,                              /* 完整性保护未启动    */
    NAS_MML_RRC_INTEGRITY_PROTECT_ACTIVE           = 1,                              /* 完整性保护启动    */
    NAS_MML_RRC_INTEGRITY_PROTECT_BUTT
};

typedef VOS_UINT8 NAS_MML_RRC_INTEGRITY_PROTECT_ENUM_UINT8;



enum NAS_MML_PROC_TYPE_ENUM
{
    NAS_MML_PROC_ATTACH,
    NAS_MML_PROC_MT_DETACH,
    NAS_MML_PROC_LOCAL_DETACH,
    NAS_MML_PROC_UE_DETACH,
    NAS_MML_PROC_TAU,
    NAS_MML_PROC_RAU,
    NAS_MML_PROC_LAU,
    NAS_MML_PROC_AUTH,
    NAS_MML_PROC_SER,
    NAS_MML_PROC_CM_SER,
    NAS_MML_PROC_COMBINED_ATTACH,
    NAS_MML_PROC_COMBINED_RAU,
    NAS_MML_PROC_PERIODC_RAU,
    NAS_MML_PROC_MM_ABORT,
    NAS_MML_PROC_BUTT
};
typedef VOS_UINT32 NAS_MML_PROC_TYPE_ENUM_U32;


enum NAS_MML_GAS_SUBMODE_ENUM
{
    NAS_MML_GAS_SUBMODE_GSM             = 0,
    NAS_MML_GAS_SUBMODE_GPRS            = 1,
    NAS_MML_GAS_SUBMODE_EDGE            = 2,
    NAS_MML_GAS_SUBMODE_BUTT
};


/*****************************************************************************
 枚举名    : NAS_MML_INIT_CTX_TYPE_ENUM_UINT8
 结构说明  : 初始化MML CTX信息类型
 1.日    期   : 2011年7月14日
   作    者   : zhoujun 40661
   修改内容   : 新增
*****************************************************************************/
enum NAS_MML_INIT_CTX_TYPE_ENUM
{
    NAS_MML_INIT_CTX_STARTUP                    = 0,
    NAS_MML_INIT_CTX_POWEROFF                   = 1,
    NAS_MML_INIT_CTX_BUTT
};
typedef VOS_UINT32 NAS_MML_INIT_CTX_TYPE_ENUM_UINT8;


enum    NAS_MML_READ_SIM_FILES_CNF_ENUM
{
    NAS_MML_READ_USIM_FILE_FLG_NULL                         = 0x0,
    NAS_MML_READ_HPLMN_SEL_FILE_ID_FLG                      = 0x1,
    NAS_MML_READ_FORBIDDEN_PLMN_LIST_FILE_ID_FLG            = 0x2,
    NAS_MML_READ_UPLMN_SEL_FILE_ID_FLG                      = 0x4,
    NAS_MML_READ_OPLMN_SEL_FILE_ID_FLG                      = 0x8,
    NAS_MML_READ_HPLMN_PERI_FILE_ID_FLG                     = 0x10,
    NAS_MML_READ_PLMN_SEL_FILE_ID_FLG                       = 0x20,
    NAS_MML_READ_MNC_LENGTH_FILE_ID_FLG                     = 0x40,
    NAS_MML_READ_UST_FILE_ID_FLG                            = 0x80,
    NAS_MML_READ_EHPLMN_FILE_ID_FLG                         = 0x100,
    NAS_MML_READ_EHPLMNPI_FILE_ID_FLG                       = 0x200,
    NAS_MML_READ_LRPLMNSI_FILE_ID_FLG                       = 0x400,
    NAS_MML_READ_CS_LOC_FILE_ID_FLG                         = 0x800,
    NAS_MML_READ_PS_LOC_FILE_ID_FLG                         = 0x1000,
    NAS_MML_READ_SIM_PS_LOC_FILE_ID_FLG                     = 0x2000,
    NAS_MML_READ_CS_CKIK_FILE_ID_FLG                        = 0x4000,
    NAS_MML_READ_PS_KEY_FILE_ID_FLG                         = 0x8000,
    NAS_MML_READ_CS_KC_FILE_ID_FLG                          = 0x10000,
    NAS_MML_READ_PS_KC_FILE_ID_FLG                          = 0x20000,
    NAS_MML_READ_USIM_CS_KC_FILE_ID_FLG                     = 0x40000,
    NAS_MML_READ_USIM_PS_KC_FILE_ID_FLG                     = 0x80000,

    NAS_MML_READ_ACTING_HPLMN_FILE_ID_FLG                   = 0x100000,

    NAS_MML_READ_USIM_FILE_CNF_BUTT
};

typedef VOS_UINT32 NAS_MML_READ_SIM_FILES_CNF_ENUM_UINT32;


enum NAS_MML_PLMN_TYPE_ENUM
{
    NAS_MML_PLMN_TYPE_BCCH,
    NAS_MML_PLMN_TYPE_SIM,
    NAS_MML_PLMN_TYPE_UNKNOW,
    NAS_MML_PLMN_TYPE_BUTT
};
typedef VOS_UINT32 NAS_MML_PLMN_TYPE_ENUM_UINT8;


enum NAS_MML_LTE_CAPABILITY_STATUS_ENUM
{
    NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS               = 0,        /* LTE特性不可用,但未通知接入层 */
    NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS               = 1,        /* LTE特性不可用,已通知接入层 */
    NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_UNNOTIFY_AS              = 2,        /* LTE特性恢复可用,但未通知接入层 */
    NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS              = 3,        /* LTE特性恢复可用,已通知接入层 */
    NAS_MML_LTE_CAPABILITY_STATUS_BUTT                              = 4         /* 无效值 */
};
typedef VOS_UINT32 NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32;
enum NAS_MML_RRC_SYS_SUBMODE_ENUM
{
    NAS_MML_RRC_SYS_SUBMODE_NULL                  = 0,                /* 无服务 */
    NAS_MML_RRC_SYS_SUBMODE_GSM                   = 1,                /* GSM模式 */
    NAS_MML_RRC_SYS_SUBMODE_GPRS                  = 2,                /* GPRS模式 */
    NAS_MML_RRC_SYS_SUBMODE_EDGE                  = 3,                /* EDGE模式 */
    NAS_MML_RRC_SYS_SUBMODE_WCDMA                 = 4,                /* WCDMA模式 */
    NAS_MML_RRC_SYS_SUBMODE_HSDPA                 = 5,                /* HSDPA模式 */
    NAS_MML_RRC_SYS_SUBMODE_HSUPA                 = 6,                /* HSUPA模式 */
    NAS_MML_RRC_SYS_SUBMODE_HSDPA_HSUPA           = 7,                /* HSDPA+HSUPA模式 */
    NAS_MML_RRC_SYS_SUBMODE_LTE                   = 8,                /* LTE模式 */
    NAS_MML_RRC_SYS_SUBMODE_BUTT
};
typedef VOS_UINT8  NAS_MML_RRC_SYS_SUBMODE_ENUM_UINT8;
enum NAS_MML_NETWORK_MSG_REL_VER_ENUM
{
    NAS_MML_NETWORK_MSG_REL_VER_98_OR_OLDER        = 0,
    NAS_MML_NETWORK_MSG_REL_VER_99_ONWARDS         = 1,
    NAS_MML_NETWORK_MSG_REL_VER_BUTT
};
typedef VOS_UINT8   NAS_MML_NETWORK_MSG_REL_VER_ENUM_UINT8;



enum NAS_MML_NETWORK_SGSN_REL_VER_ENUM
{
    NAS_MML_NETWORK_SGSN_REL_VER_98_OR_OLDER        = 0,
    NAS_MML_NETWORK_SGSN_REL_VER_99_ONWARDS         = 1,
    NAS_MML_NETWORK_SGSN_REL_VER_BUTT
};
typedef VOS_UINT8   NAS_MML_NETWORK_SGSN_REL_VER_ENUM_UINT8;


enum NAS_MML_RESTRICTION_CHANGE_ENUM
{
    NAS_MML_RESTRICTION_NO_CHANGE                   = 0,
    NAS_MML_RESTRICTION_BAR_TO_UNBAR                = 1,
    NAS_MML_RESTRICTION_UNBAR_TO_BAR                = 2,
    NAS_MML_RESTRICTION_BUTT
};

typedef VOS_UINT8   NAS_MML_RESTRICTION_CHANGE_ENUM_UINT8;


enum NAS_MML_HPLMN_TYPE_ENUM
{
    NAS_MML_HPLMN_TYPE_HPLMN,                                                   /* HPLMN */
    NAS_MML_HPLMN_TYPE_EHPLMN,                                                  /* EHPLMN */
    NAS_MML_HPLMN_TYPE_ACTING_HPLMN,                                            /* ACTING-HPLMN */
    NAS_MML_HPLMN_TYPE_BUTT
};
typedef VOS_UINT8 NAS_MML_HPLMN_TYPE_ENUM_UINT8;
enum NAS_MML_REG_DOMAIN_ENUM
{
    NAS_MML_REG_DOMAIN_CS    = 0x01,                                            /* 注册结果域为CS域 */
    NAS_MML_REG_DOMAIN_PS    = 0x02,                                            /* 注册结果域为PS域 */
    NAS_MML_REG_DOMAIN_BUTT
};
typedef VOS_UINT8 NAS_MML_REG_DOMAIN_ENUM_UINT8;


enum NAS_MML_SINGLE_DOMAIN_REG_FAIL_ACTION_ENUM
{
    NAS_MML_SINGLE_DOMAIN_REG_FAIL_ACTION_PLMN_SELECTION                    = 1,            /* 触发搜网 */
    NAS_MML_SINGLE_DOMAIN_REG_FAIL_ACTION_NORMAL_CAMP_ON                    = 2,            /* 正常驻留 */
    NAS_MML_SINGLE_DOMAIN_REG_FAIL_ACTION_OPTIONAL_PLMN_SELECTION           = 3,            /* 触发可选搜网 */
    NAS_MML_SINGLE_DOMAIN_REG_FAIL_ACTION_LIMITED_CAMP_ON                   = 4,            /* 限制驻留 */
    NAS_MML_SINGLE_DOMAIN_REG_FAIL_ACTION_BUTT
};
typedef VOS_UINT8 NAS_MML_SINGLE_DOMAIN_REG_FAIL_ACTION_ENUM_UINT8;


enum NAS_MML_PS_REG_CONTAIN_DRX_PARA_ENUM
{
    NAS_MML_LTE_PS_REG_CONTAIN_DRX_PARA                     = 0,                /* LTE ATTACH或TAU携带了DRX参数 */
    NAS_MML_LTE_PS_REG_NOT_CONTAIN_DRX_PARA                 = 1,                /* LTE ATTACH或TAU未携带DRX参数 */
    NAS_MML_GU_PS_REG_CONTAIN_DRX_PARA                      = 2,                /* GU ATTACH或RAU携带了DRX参数 */
    NAS_MML_GU_PS_REG_NOT_CONTAIN_DRX_PARA                  = 3,                /* GU ATTACH或RAU未携带DRX参数 */
    NAS_MML_PS_REG_CONTAIN_BUTT
};
typedef VOS_UINT8 NAS_MML_PS_REG_CONTAIN_DRX_PARA_ENUM_UINT8;



enum NAS_MML_IMS_NORMAL_REG_STATUS_ENUM
{
    NAS_MML_IMS_NORMAL_REG_STATUS_DEREG        = 0x00,
    NAS_MML_IMS_NORMAL_REG_STATUS_DEREGING     = 0x01,
    NAS_MML_IMS_NORMAL_REG_STATUS_REGING       = 0x02,
    NAS_MML_IMS_NORMAL_REG_STATUS_REG          = 0x03,

    NAS_MML_IMS_NORMAL_REG_STATUS_BUTT         = 0xFF
};
typedef VOS_UINT8 NAS_MML_IMS_NORMAL_REG_STATUS_ENUM_UINT8;



enum NAS_MML_NW_IMS_VOICE_CAP_ENUM
{
    NAS_MML_NW_IMS_VOICE_NOT_SUPPORTED    = 0,
    NAS_MML_NW_IMS_VOICE_SUPPORTED        = 1,

    NAS_MML_NW_IMS_VOICE_BUTT
};
typedef VOS_UINT8 NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8;



enum NAS_MML_AUTO_SRCH_FLG_TYPE_ENUM
{
    NAS_MML_AUTO_SRCH_FLG_TYPE_RPLMN_FIRST               = 0,                /* 自动开机或丢网时，先搜RPLMN */
    NAS_MML_AUTO_SRCH_FLG_TYPE_PLMN_SEL_IND              = 1,      /* PLMN SEL IND文件6FDC中获取PLMN */  
    NAS_MML_AUTO_SRCH_FLG_TYPE_HPLMN_FIRST               = 2,                /* 自动开机或丢网时，先搜HPLMN */
    NAS_MML_AUTO_SRCH_FLG_TYPE_RPLMN_WITH_HPLMN_FIRST    = 3,                /* 自动开机或丢网时，先搜RPLMN但此时需带HPLMN */
    NAS_MML_SEARCH_HPLMN_FLG_TYPE_BUTT
};
typedef VOS_UINT8 NAS_MML_AUTO_SRCH_FLG_TYPE_ENUM_UINT8;



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
    VOS_MSG_HEADER
    VOS_UINT32                           ulMsgName;
    VOS_UINT8                            aucData[4];
}NAS_MML_PC_REPLAY_COMPRESS_CONTEXT_STRU;


typedef struct
{
    VOS_UINT8                                   ucSimPresentStatus;  /* SIM卡在位信息,VOS_TRUE:SIM卡在位,VOS_FALSE:SIM卡不在位 */
    NAS_MML_SIM_TYPE_ENUM_UINT8                 enSimType;           /* SIM卡类型,USIM或SIM卡*/
    VOS_UINT8                                   ucSimCsRegStatus;    /* SIM卡CS域的注册结果导致的卡是否有效VOS_TRUE:CS域的卡有效,VOS_FALSE:CS域的卡无效*/
    VOS_UINT8                                   ucSimPsRegStatus;    /* SIM卡PS域的注册结果导致的卡是否有效VOS_TRUE:PS域的卡有效,VOS_FALSE:PS域的卡无效*/
    NAS_MML_ROUTING_UPDATE_STATUS_ENUM_UINT8    enPsUpdateStatus;    /* status of routing update */
    NAS_MML_LOCATION_UPDATE_STATUS_ENUM_UINT8   enCsUpdateStatus;    /* status of location update */
    VOS_UINT8                                   aucReserve[2];

}NAS_MML_SIM_STATUS_STRU;


/*****************************************************************************
 结构名    : NAS_MML_SIM_MS_IDENTITY_STRU
 结构说明  : 保存SIM中MS标识符，如IMSI,TMSI,Ptmsi等内容
  1.日    期   : 2011年6月27日
    作    者   : zhoujun 40661
    修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucImsi[NAS_MML_MAX_IMSI_LEN];          /* imsi的内容 */
    VOS_UINT8                           aucPtmsiSignature[NAS_MML_MAX_PTMSI_SIGNATURE_LEN]; /* ptmsi signature的内容 */
    VOS_UINT8                           aucPtmsi[NAS_MML_MAX_PTMSI_LEN];        /* ptmsi的内容 */
    VOS_UINT8                           aucTmsi[NAS_MML_MAX_TMSI_LEN];          /* tmsi的内容 */
    NAS_MML_SIM_UE_OPER_MODE_ENUM_UINT8 enUeOperMode;                           /* ue 操作模式 */
    VOS_UINT8                           aucReserve[3];
}NAS_MML_SIM_MS_IDENTITY_STRU;

/*****************************************************************************
 结构名    : NAS_MML_SIM_PS_SECURITY_INFO_STRU
 结构说明  : 保存当前PS域的安全上下文
  1.日    期   : 2011年6月27日
    作    者   : zhoujun 40661
    修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucCKSN;                                 /* PS域的CKSN的内容 */
    VOS_UINT8                           aucReserve[3];
    VOS_UINT8                           aucUmtsCk[NAS_MML_UMTS_CK_LEN];         /* PS域的CK的内容 */
    VOS_UINT8                           aucUmtsIk[NAS_MML_UMTS_IK_LEN];         /* PS域的IK的内容 */
    VOS_UINT8                           aucGsmKc[NAS_MML_GSM_KC_LEN];           /* PS域的KC的内容 */
}NAS_MML_SIM_PS_SECURITY_INFO_STRU;


/*****************************************************************************
 结构名    : NAS_MML_SIM_CS_SECURITY_INFO_STRU
 结构说明  : 保存当前CS域的安全上下文
  1.日    期   : 2011年6月27日
    作    者   : zhoujun 40661
    修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucCKSN;                                 /* CS域的CKSN的内容 */
    VOS_UINT8                           aucReserve[3];
    VOS_UINT8                           aucUmtsCk[NAS_MML_UMTS_CK_LEN];         /* CS域的CK的内容 */
    VOS_UINT8                           aucUmtsIk[NAS_MML_UMTS_IK_LEN];         /* CS域的IK的内容 */
    VOS_UINT8                           aucGsmKc[NAS_MML_GSM_KC_LEN];           /* CS域的KC的内容 */
}NAS_MML_SIM_CS_SECURITY_INFO_STRU;

/*****************************************************************************
 结构名    : NAS_MML_PLMN_ID_STRU
 结构说明  : plmn id的信息
 1.日    期   : 2011年5月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulMcc;                                  /* MCC,3 bytes */
    VOS_UINT32                          ulMnc;                                  /* MNC,2 or 3 bytes */
}NAS_MML_PLMN_ID_STRU;

/*****************************************************************************
 结构名    : NAS_MML_SIM_FORMAT_PLMN_ID
 结构说明  : Sim PLMN ID的存储结构
 1.日    期   : 2011年5月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/

typedef struct {
    VOS_UINT8                           aucSimPlmn[NAS_MML_SIM_FORMAT_PLMN_LEN];
    VOS_UINT8                           aucReserve[1];
}NAS_MML_SIM_FORMAT_PLMN_ID;


typedef struct
{
    NAS_MML_PLMN_ID_STRU                stPlmnId;                               /* PlMN标识 */
    VOS_UINT8                           aucLac[NAS_MML_MAX_LAC_LEN];            /* 注册区信息 */

    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enCampPlmnNetRat;                       /* PLMN的接入技术，收到系统消息时更新*/

    VOS_UINT8                           ucRac;
}NAS_MML_LAI_STRU;
typedef struct
{
    VOS_UINT8                           ucTac;
    VOS_UINT8                           ucTacCnt;
    VOS_UINT8                           aucRsv[2];
}NAS_MML_TAC_STRU;


typedef struct
{
    NAS_MML_PLMN_ID_STRU                 stPlmnId;
    NAS_MML_TAC_STRU                     stTac;
}NAS_MML_TAI_STRU;

/*****************************************************************************
 结构名    : NAS_MML_RAI_STRU
 结构说明  : RAI的信息
 1.日    期   : 2011年6月27日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    NAS_MML_LAI_STRU                    stLai;                                  /* 位置区信息 */
    VOS_UINT8                           ucRac;                                  /* RAC信息 */
    VOS_UINT8                           aucReserve[3];
}NAS_MML_RAI_STRU;

/*****************************************************************************
 结构名    : NAS_MML_SIM_PLMN_WITH_RAT_STRU
 结构说明  : plmn id的信息和接入技术
 1.日    期   : 2011年5月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    NAS_MML_PLMN_ID_STRU                stPlmnId;                               /* PLMN ID */
    VOS_UINT16                          usSimRat;                               /* SIM卡中支持的接入技术 */
    VOS_UINT16                          aucReserve[2];
}NAS_MML_SIM_PLMN_WITH_RAT_STRU;


typedef struct
{
    VOS_UINT8                               ucEhPlmnNum;                        /* EHPLMN的个数*/
    NAS_MML_LAST_RPLMN_SEL_IND_ENUM_UINT8   enPLMNSelInd;                       /* 开机或丢网后选择的plmn类型 */                                                        /* EF_LRPLMNSI的内容 */
    NAS_MML_EHPLMN_PRESENT_IND_ENUM_UINT8   enEHplmnPresentInd;                 /* EHPLMN上报给用户的方式 */                                                                              /* 保存USIM返回的EF_EHPLMNPI 的内容到全局变量中，共上报用户列表时使用 */

    NAS_MML_HPLMN_TYPE_ENUM_UINT8           enHplmnType;

    NAS_MML_SIM_PLMN_WITH_RAT_STRU          astEhPlmnInfo[NAS_MML_MAX_EHPLMN_NUM];/* EHPLMN ID列表 */
}NAS_MML_SIM_EHPLMN_INFO_STRU;

/*****************************************************************************
 结构名    : NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU
 结构说明  : 保存当前HPLMNwithRat信息
 1.日    期   : 2011年6月27日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucHPlmnWithRatNum;                      /* HPLMNwithRat的个数*/
    VOS_UINT8                           aucReserve3[3];
    NAS_MML_SIM_PLMN_WITH_RAT_STRU      astHPlmnWithRatInfo[NAS_MML_MAX_HPLMN_WITH_RAT_NUM];/* HPLMNwithRat列表*/
}NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU;

/*****************************************************************************
 结构名    : NAS_MML_SIM_USERPLMN_INFO_STRU
 结构说明  : 保存当前USERPLMN信息
 1.日    期   : 2011年6月27日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucUserPlmnNum;                          /* UPLMN的个数*/
    VOS_UINT8                           aucReserve3[3];
    NAS_MML_SIM_PLMN_WITH_RAT_STRU      astUserPlmnInfo[NAS_MML_MAX_USERPLMN_NUM];/* UPLMN的列表*/
}NAS_MML_SIM_USERPLMN_INFO_STRU;



typedef struct
{
    VOS_UINT16                          usOperPlmnNum;                          /* OPLMN的个数*/
    VOS_UINT8                           aucReserve2[2];
    NAS_MML_SIM_PLMN_WITH_RAT_STRU      astOperPlmnInfo[NAS_MML_MAX_OPERPLMN_NUM];/* OPLMN的列表*/
}NAS_MML_SIM_OPERPLMN_INFO_STRU;

/*****************************************************************************
 结构名    : NAS_MML_SIM_SELPLMN_INFO_STRU
 结构说明  : 保存当前SelPLMN信息
  1.日    期   : 2011年6月27日
    作    者   : zhoujun 40661
    修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSelPlmnNum;                           /* SELPLMN的个数*/
    VOS_UINT8                           aucReserve3[3];
    NAS_MML_PLMN_ID_STRU                astPlmnId[NAS_MML_MAX_SELPLMN_NUM];     /* SELPLMN的列表*/
}NAS_MML_SIM_SELPLMN_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucForbRoamLaNum;                        /* forbidden las for roaming个数 */
    VOS_UINT8                           aucReserve[3];
    NAS_MML_LAI_STRU                    astForbRomLaList[NAS_MML_MAX_FORBLA_FOR_ROAM_NUM];/* "forbidden las for roaming" list */
}NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU;



/*****************************************************************************
 结构名    : NAS_MML_SIM_FORBIDPLMN_INFO_STRU
 结构说明  : 保存当前Forbid PLMN信息
  1.日    期   : 2011年6月27日
    作    者   : zhoujun 40661
    修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucUsimForbPlmnNum;                      /* SIM卡中禁止的PLMN ID的个数  */

    VOS_UINT8                           ucForbPlmnNum;                          /* 禁止的PLMN ID的个数         */

    VOS_UINT8                           ucForbGprsPlmnNum;                      /* 禁止GPRS的PLMN的个数        */

    VOS_UINT8                           ucForbRoamLaNum;                        /* forbidden las for roaming个数            */

    VOS_UINT8                           ucForbRegLaNum;                         /* forbidden las for regional provision个数 */

    VOS_UINT8                           aucReserve[3];

    NAS_MML_PLMN_ID_STRU                astForbPlmnIdList[NAS_MML_MAX_FORBPLMN_NUM];/* 禁止的PLMN ID列表                        */

    NAS_MML_PLMN_ID_STRU                astForbGprsPlmnList[NAS_MML_MAX_FORBGPRSPLMN_NUM];/* "forbidden PLMNs for GPRS service" list  */

    NAS_MML_LAI_STRU                    astForbRomLaList[NAS_MML_MAX_FORBLA_FOR_ROAM_NUM];/* "forbidden las for roaming" list         */

    NAS_MML_LAI_STRU                    astForbRegLaList[NAS_MML_MAX_FORBLA_FOR_REG_NUM];/* "forbidden las for regional provision"   */

}NAS_MML_SIM_FORBIDPLMN_INFO_STRU;
typedef struct
{
    NAS_MML_SIM_EHPLMN_INFO_STRU         stEhPlmnInfo;                          /* EHPLMN列表信息 */
    NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU stHplmnWithRatInfo;                    /* HPLMNWithRat列表信息 */
    NAS_MML_SIM_USERPLMN_INFO_STRU       stUserPlmnInfo;                        /* UPLMN列表信息 */
    NAS_MML_SIM_OPERPLMN_INFO_STRU       stOperPlmnInfo;                        /* OPLMN列表信息 */
    NAS_MML_SIM_SELPLMN_INFO_STRU        stSelPlmnInfo;                         /* SELPLMN列表信息 */
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU     stForbidPlmnInfo;                      /* 禁止网络信息 */
    VOS_UINT32                           ulSimHplmnTimerLen;                    /* SIM卡HPLMN定时器时长 */

    VOS_UINT8                            ucUsimMncLen;                          /* Usim文件中指示的Mnc长度，默认为3位 */
    VOS_UINT8                            aucReserve[3];
}NAS_MML_SIM_PLMN_INFO_STRU;

/*****************************************************************************
 结构名    : NAS_MML_SIM_NATIVE_CONTENT_STRU
 结构说明  : SIM卡文件中保存的原始内容
  1.日    期   : 2011年12月1日
    作    者   : zhoujun 40661
    修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                   aucCsLociInfoFile[NAS_MML_CS_LOCI_SIM_FILE_LEN];
    VOS_UINT8                                   aucPsLociInfoFile[NAS_MML_PS_LOCI_SIM_FILE_LEN];
}NAS_MML_SIM_NATIVE_CONTENT_STRU;


/*****************************************************************************
 枚举名    : NAS_MML_SIM_INFO_STRU
 结构说明  : SIM卡信息,包括SIM卡状态,MS Identity,CS和PS域的安全参数以及SIM卡的PLMN信息
  1.日    期   : 2011年6月27日
    作    者   : zhoujun 40661
    修改内容   : 新建

  2.日    期   : 2011年12月1日
    作    者   : zhoujun /40661
    修改内容   : 增加SIM卡文件中保存的原始内容以便写卡时进行比较
*****************************************************************************/
typedef struct
{
    NAS_MML_SIM_STATUS_STRU             stSimStatus;                            /* SIM卡状态 */
    NAS_MML_SIM_MS_IDENTITY_STRU        stMsIdentity;                           /* MS Identity信息 */
    NAS_MML_SIM_PS_SECURITY_INFO_STRU   stPsSecurity;                           /* PS域的安全参数 */
    NAS_MML_SIM_CS_SECURITY_INFO_STRU   stCsSecurity;                           /* CS域的安全参数 */
    NAS_MML_SIM_PLMN_INFO_STRU          stSimPlmnInfo;                          /* SIM卡中保存的PLMN信息 */
    NAS_MML_SIM_NATIVE_CONTENT_STRU     stSimNativeContent;                     /* SIM卡文件中保存的原始内容以便写卡时进行比较 */
}NAS_MML_SIM_INFO_STRU;


typedef struct
{
    NAS_MML_3GPP_REL_ENUM_UINT8      enMsGsmRel;                                /* NV中保存的G模下3GPP的版本 */
    NAS_MML_3GPP_REL_ENUM_UINT8      enMsWcdmaRel;                              /* NV中保存的W模下3GPP的版本 */
    NAS_MML_SGSN_REL_ENUM_UINT8      enMsSgsnRel;                               /* NV中保存的MS支持SGSN的版本，主要用于终端上报的SGSN版本 */
    NAS_MML_MSC_REL_ENUM_UINT8       enMsMscRel;                                /* NV中保存的MS支持MSC的版本，主要用于终端上报的MSC版本 */
    NAS_MML_3GPP_REL_ENUM_UINT8      enLteNasRelease;                           /* NV中保存的L模下3GPP的版本 */
}NAS_MML_MS_3GPP_REL_STRU;
typedef struct
{
    NAS_MML_NETWORK_MSG_REL_VER_ENUM_UINT8        enNetMscRel;                            /* 网络下发的MSG的协议版本信息 */
    NAS_MML_NETWORK_SGSN_REL_VER_ENUM_UINT8       enNetSgsnRel;                           /* 网络下发的SGSN的协议版本信息 */
    VOS_UINT8                           aucReserve[2];
}NAS_MML_NETWORK_3GPP_REL_STRU;



typedef struct
{
    VOS_UINT8                           ucUeNetCapLen;                           /* UE Network Capability的长度 */
    VOS_UINT8                           aucUeNetCap[NAS_MML_MAX_UE_NETWORK_CAPABILITY_LEN]; /* L的ue network capability*/
    VOS_UINT8                           aucRsv[2];
}NAS_MML_UE_NETWORK_CAPABILITY_STRU;


typedef struct
{
    VOS_UINT8                               ucRatNum;                                   /* 平台支持的接入技术的个数 */
    NAS_MML_PLATFORM_RAT_TYPE_ENUM_UINT8    aenRatPrio[NAS_MML_MAX_PLATFORM_RAT_NUM];   /* 平台支持的接入技术的优先级顺序 */
}NAS_MML_PLATFORM_RAT_CAP_STRU;
/*****************************************************************************
 结构名    : NAS_MML_MS_NETWORK_CAPACILITY_STRU
 结构说明  : GU MS Network Capability
 1.日    期   : 2011年11月8日
   作    者   : zh00161729
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucNetworkCapabilityLen;                 /* GU的网络能力的长度 */
    VOS_UINT8                           aucNetworkCapability[NAS_MML_MAX_NETWORK_CAPABILITY_LEN];/*GU的network capability*/
    VOS_UINT8                           aucReserv[3];
}NAS_MML_MS_NETWORK_CAPACILITY_STRU;


typedef struct
{
    VOS_UINT32                          ulGsmCapability;
}NAS_MML_PLATFORM_BAND_CAP_STRU;


typedef struct
{
    VOS_UINT8                           ucClassmark1;                           /* classmark 1 information */
    VOS_UINT8                           aucClassmark2[NAS_MML_CLASSMARK2_LEN];  /* classmark 2 information */
    VOS_UINT8                           aucFddClassmark3[NAS_MML_CLASSMARK3_LEN];  /* classmark 3 information 第0位代表实际长度 */    
    VOS_UINT8                           aucTddClassmark3[NAS_MML_CLASSMARK3_LEN];  /* classmark 3 information 第0位代表实际长度 */
    NAS_MML_MS_NETWORK_CAPACILITY_STRU  stMsNetworkCapability;                  /* MS network capability*/
    VOS_UINT8                           aucImeisv[NAS_MML_MAX_IMEISV_LEN];      /* IMEISV */

    NAS_MML_UE_NETWORK_CAPABILITY_STRU  stUeNetworkCapbility;                   /* 记录nv中ue network capability的内容 */

    NAS_MML_PLATFORM_RAT_CAP_STRU       stPlatformRatCap;                       /* 记录NV中的平台接入技术能力 */
    NAS_MML_PLATFORM_BAND_CAP_STRU      stPlatformBandCap;                      /* 记录NV中的平台通路BAND能力 */
}NAS_MML_MS_CAPACILITY_INFO_STRU;

/*****************************************************************************
 结构名    : NAS_MML_MS_BAND_INFO_STRU
 结构说明  : MS band的能力
 1.日    期   : 2011年6月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    WCDMA_BAND_SET_UN                   unWcdmaBand;                            /* wcdma当前设置支持的频段 */
    GSM_BAND_SET_UN                     unGsmBand;                              /* gsm当前设置支持的频段 */
    LTE_BAND_STRU                       stLteBand;                              /* lte当前设置支持的频段 */
}NAS_MML_MS_BAND_INFO_STRU;

/*****************************************************************************
 结构名    : NAS_MMC_PLMN_RAT_PRIO_STRU
 结构说明  : MS band的能力
 1.日    期   : 2011年6月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                       ucRatNum;                                   /* 当前支持的接入技术个数  */
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8 aucRatPrio[NAS_MML_MAX_RAT_NUM];            /* 接入优先级里列表 index表示优先级次序,0最高，2最低 */
}NAS_MML_PLMN_RAT_PRIO_STRU;


typedef struct
{
    VOS_UINT8                           ucRatNum;                               /* 当前支持的接入技术个数  */
    NAS_MML_3GPP2_RAT_TYPE_ENUM_UINT8   aucRatPrio[NAS_MML_MAX_3GPP2_RAT_NUM];  /* 接入优先级里列表 index表示优先级次序,0最高，2最低 */
    VOS_UINT8                           aucReserved[1];
}NAS_MML_3GPP2_RAT_PRIO_STRU;

typedef struct
{

    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLteCapabilityStatus;/* 去使能LTE能力标记 */

    MMC_LMM_DISABLE_LTE_REASON_ENUM_UINT32                  enDisableLteReason;

    VOS_UINT32                                              ulDisableLteRoamFlg;/* 禁止LTE漫游导致的disable LTE标记 */
    NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8                   enLteCsServiceCfg;  /* LTE支持的 cs域业务能力*/

    VOS_UINT8                                               ucImsSupportFlg;    /* IMS是否支持标记,VOS_TRUE:支持,VOS_FALSE:不支持 */
    VOS_UINT8                                               aucReserve[2];


    NAS_MML_LTE_UE_USAGE_SETTING_ENUM_UINT8                 enLteUeUsageSetting;
    NAS_MML_VOICE_DOMAIN_PREFERENCE_ENUM_UINT8              enVoiceDomainPreference;
    NAS_MML_MS_MODE_ENUM_UINT8                              enMsMode;           /* 手机模式 */
    VOS_UINT8                                               ucPsAutoAttachFlg;  /* PS自动Attach标志 */
    NAS_MML_MS_BAND_INFO_STRU                               stMsBand;           /* 当前MS支持的频段 */
    NAS_MML_PLMN_RAT_PRIO_STRU                              stPrioRatList;      /* 接入技术以及优先级 */

    NAS_MML_3GPP2_RAT_PRIO_STRU                             st3Gpp2RatList;     /* 3GPP2接入技术以及优先级 */

    VOS_UINT8                                               ucDelayedCsfbLauFlg;
    VOS_UINT8                                               aucReserved2[3];
}NAS_MML_MS_SYS_CFG_INFO_STRU;

/*****************************************************************************
 结构名    : NAS_MML_RPLMN_CFG_INFO_STRU
 结构说明  : NVIM项中的RPLMN定制需求信息
 1.日    期   : 2011年6月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                   aucLastImsi[NAS_MML_MAX_IMSI_LEN];/* 上次保存的IMSI的内容 */
    VOS_UINT8                                   aucReserve[3];
    NAS_MML_TIN_TYPE_ENUM_UINT8                 enTinType;                      /* TIN类型 */
    VOS_UINT8                                   ucMultiRATRplmnFlg;             /* 是否支持多Rplmn,H3G定制需求,VOS_FALSE:不支持,VOS_TRUE:支持 */
    /*上次关机时，注册成功的plmn的接入技术，后续注册失败RPLMN删除后，该值仍然是有效,VOS_FALSE:不支持,VOS_TRUE:支持*/
    VOS_UINT8                                   ucLastRplmnRatFlg;
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8             enLastRplmnRat;                 /* NVIM中保存的上次关机前RPLMN的接入技术 */
    NAS_MML_PLMN_ID_STRU                        stWRplmnInNV;                   /* NVIM中保存的W模的RPLMN */
    NAS_MML_PLMN_ID_STRU                        stGRplmnInNV;                   /* NVIM中保存的G模的RPLMN */
}NAS_MML_RPLMN_CFG_INFO_STRU;
typedef struct
{
    VOS_UINT8                            ucActiveFlg;                           /* 是否使用定制的HPLMNAct的接入技术优先级，VOS_TRUE:激活；VOS_FALSE:未激活 */
	VOS_UINT8                            ucReserved;
    VOS_UINT16                           usPrioHplmnAct;                            /* 定制的 HPLMN 的优先接入技术，格式与 Usim文件6f62的接入技术相同 */
}NAS_MML_PRIO_HPLMNACT_CFG_STRU;
typedef struct
{
    VOS_UINT8                           ucImsiPlmnListNum;
    VOS_UINT8                           ucEhplmnListNum;
    VOS_UINT8                           aucResv[2];
    NAS_MML_PLMN_ID_STRU                astImsiPlmnList[NAS_MML_MAX_USER_CFG_IMSI_PLMN_NUM];
    NAS_MML_PLMN_ID_STRU                astEhPlmnList[NAS_MML_MAX_USER_CFG_EHPLMN_NUM];
}NAS_MML_USER_CFG_EHPLMN_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucActiveFlg;                                            /* 定制项使能标志 */
    VOS_UINT8                           ucImsiCheckFlg;                                         /* 是否有IMSI列表的白名单，0:不需要 1:需要 */
    VOS_UINT8                           aucVersion[NAS_MML_MAX_USER_OPLMN_VERSION_LEN];         /* 用户配置的OPMN的版本号 */
    VOS_UINT16                          usOplmnListNum;                                         /* 用户配置的的OPlmn的个数 */
    VOS_UINT8                           ucImsiPlmnListNum;                                      /* 定制的IMSI列表个数 */
    VOS_UINT8                           aucResv[3];    
    NAS_MML_PLMN_ID_STRU                astImsiPlmnList[NAS_MML_MAX_USER_OPLMN_IMSI_NUM];
}NAS_MML_USER_CFG_OPLMN_INFO_STRU;
typedef struct
{
    VOS_UINT8   ucAccBarPlmnSearchFlg;
    VOS_UINT8   ucReserve[3];
}NAS_MML_ACC_BAR_PLMN_SEARCH_FLG_STRU;


typedef struct
{
    VOS_UINT8                                               ucEhplmnSupportFlg;                     /* EHPLMN特性是否支持,VOS_FALSE:不支持,VOS_TRUE:支持 */
    NAS_MML_AUTO_SRCH_FLG_TYPE_ENUM_UINT8                   enAutoSearchHplmnFlg;                   /* 记录是否支持协议要求的在自动模式开机或丢网后搜索Hplmn的特性,VOS_FALSE:不支持,VOS_TRUE:支持 */
    VOS_UINT8                                               ucManualSearchHplmnFlg;                 /* 记录是否支持协议要求的手动开机搜索Eplmn失败后是否搜索Hplmn的要求,VOS_FALSE:不支持,VOS_TRUE:支持 */
    VOS_UINT8                                               ucHplmnSearchPowerOn;                   /* 开机无条件搜索HPLMN,VOS_FALSE:不支持,VOS_TRUE:支持 */

    NAS_MML_PRIO_HPLMNACT_CFG_STRU      stPrioHplmnActCfg;                         /* 定制的HPlmnAct的优先接入内容 */
    VOS_UINT8                           ucActingHPlmnSupportFlg;                /* ACTING-HPLMN特性是否支持,VOS_FALSE:不支持,VOS_TRUE:支持 */
    VOS_UINT8                           aucReserve[3];

    NAS_MML_USER_CFG_EHPLMN_INFO_STRU   stUserCfgEhplmnInfo;
}NAS_MML_HPLMN_CFG_INFO_STRU;




typedef struct
{
    VOS_UINT8                           ucEnableFlg;                            /* 该特性是否打开 */
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enHighPrioRatType;                      /* 漫游支持的接入技术 */
    VOS_UINT8                           aucReserve[2];
    NAS_MML_PLMN_ID_STRU                stHighPrioPlmnId;                       /* 漫游支持的PLMN ID,无条件优先选择的PLMN ID,尽管当前驻留在HPLMN上 */
    NAS_MML_PLMN_ID_STRU                stSimHPlmnId;                           /* SIM卡的HPLMN ID */
}NAS_MML_AIS_ROAMING_CFG_STRU;



/*****************************************************************************
 结构名    : NAS_MML_ROAM_CFG_INFO_STRU
 结构说明  : NVIM项中的漫游配置定制信息
 1.日    期   : 2011年6月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               ucRoamFeatureFlg;                   /*记录漫游特性是否激活,VOS_FALSE:不激活,VOS_TRUE:激活*/
    NAS_MML_ROAM_CAPABILITY_ENUM_UINT8      enRoamCapability;                   /*记录用户设置的漫游属性*/
    VOS_UINT8                               ucNotRoamNum;                       /*记录漫游不支持时可以注册网络*/
    VOS_UINT8                               ucNationalRoamNum;                  /*记录国内漫游打开时可以注册网络*/
    NAS_MML_PLMN_ID_STRU                    astRoamPlmnIdList[NAS_MML_MAX_ROAM_PLMN_NUM]; /*记录漫游网络列表*/
    NAS_MML_AIS_ROAMING_CFG_STRU            stAisRoamingCfg;
    VOS_UINT8                               ucRoamSearchRPLMNFlg;                     /*漫游时先搜RPLMN标志*/
    VOS_UINT8                               aucReserve[3];
    VOS_UINT32                              aucNationalMccList[NAS_MML_MAX_NATIONAL_MCC_NUM];                 /*运营商定制的本国的国家码*/
}NAS_MML_ROAM_CFG_INFO_STRU;
typedef struct
{
    VOS_UINT8                           ucNvimActiveFlg;                        /* 默认最大的HPLMN搜索周期是否有效标志,NV项是否激活VOS_TRUE:valid;VOS_FALSE:INVALID */
    VOS_UINT8                           aucReserve[3];
    VOS_UINT32                          ulDefaultMaxHplmnLen;                   /* 默认的最大的HPLMN搜索周期 */
}NAS_MML_DEFAULT_MAX_HPLMN_TIMER_PERIOD_STRU;


typedef struct
{
    VOS_UINT8                           ucNvimActiveFlg;                        /* H3G定制需求，VPLMN下非首次搜HPLMN定时器时长NV可配置特性是否激活，VOS_TRUE:NV项激活；VOS_FALSE:NV项未激活*/
    VOS_UINT8                           aucReserve[3];
    VOS_UINT32                          ulNonFirstHplmnTimerLen;                /* H3G定制特性激活，从NVIM中获取的搜HPLMN定时器时长，非0表示需要启动高优先级搜 */
}NAS_MML_NON_FIRST_HPLMN_TIMER_PERIOD_STRU;


typedef struct
{
    VOS_UINT8                           ucCustomMccNum;
    VOS_UINT8                           aucReserve[3];
    VOS_UINT32                          aulCostumMccList[NAS_MML_BG_SEARCH_REGARDLESS_MCC_NUMBER];/* 允许漫游的国家码列表 */
}NAS_MMC_HPLMN_SEARCH_REGARDLESS_MCC_CTX_STRU;
typedef struct
{
    VOS_UINT8                           ucActiveFLg;                             /* 该定时器是否使能 */
    VOS_UINT8                           aucRsv[3];
    VOS_UINT32                          ulFirstSearchTimeLen;                   /* high prio rat timer定时器第一次的时长 单位:秒 */    
    VOS_UINT32                          ulFirstSearchTimeCount;                 /* high prio rat timer定时器第一次时长的搜索次数 */
    VOS_UINT32                          ulNonFirstSearchTimeLen;                /* high prio rat timer定时器非首次次的时长 单位:秒 */
    VOS_UINT32                          ulRetrySearchTimeLen;                   /* high prio rat 搜被中止或不能立即发起重试的时长 单位:秒*/
}NAS_MML_HIGH_PRIO_RAT_HPLMN_TIMER_CFG_STRU;
typedef struct
{
    NAS_MML_DEFAULT_MAX_HPLMN_TIMER_PERIOD_STRU         stDefaultMaxHplmnTimerPeriod;   /* 记录en_NV_Item_Default_Hplmn_Srch_Peri NV项的信息*/
    VOS_UINT32                                          ulFirstStartHplmnTimerLen;      /* 首次启动HPLMN的时间,默认为2分钟 */
    NAS_MML_NON_FIRST_HPLMN_TIMER_PERIOD_STRU           stNonFirstHplmnTimerPeriod;     /* 记录en_NV_Item_SearchHplmnTtimerValue NV项的信息*/
    VOS_UINT8                                           ucEnhancedHplmnSrchFlg;         /* 增强的Hplmn搜网,法电定制: 启动Hplmn Timer定时器,只要没有驻留在home PLMN上,也需要启动此定时器,第一次启动2Min,后续都以T Min启动 VOS_TRUE:valid;VOS_FALSE:INVALID*/
    VOS_UINT8                                           ucSpecialRoamFlg;               /* Vplmn与Hplmn不同国家码时,是否允许回到Hplmn,VOS_TRUE:允许回HPLMN;VOS_FALSE:不允许回HPLMN */
    VOS_UINT8                                           ucScanCtrlFlg;                  /* 搜索控制标识，VOS_TRUE为激活，VOS_FALSE为未激活 */
    VOS_UINT8                                           aucReserve[1];

    NAS_MMC_HPLMN_SEARCH_REGARDLESS_MCC_CTX_STRU        stBgSearchRegardlessMcc;

    NAS_MML_HIGH_PRIO_RAT_HPLMN_TIMER_CFG_STRU              stHighRatHplmnTimerCfg;
}NAS_MML_BG_SEARCH_CFG_INFO_STRU;


typedef struct
{
    NAS_MML_PLMN_ID_STRU                stPlmnId;
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRat;
    VOS_UINT8                           aucReserve[3];
}NAS_MML_PLMN_WITH_RAT_STRU;


typedef struct
{
    VOS_UINT32                          ulDisabledRatPlmnNum;                   /* 支持禁止接入技术的PLMN个数,个数为0表示不支持该特性 */
    NAS_MML_PLMN_WITH_RAT_STRU          astDisabledRatPlmnId[NAS_MML_MAX_DISABLED_RAT_PLMN_NUM];/* 禁止接入技术的PLMN和RAT信息 */
}NAS_MML_DISABLED_RAT_PLMN_CFG_INFO_STRU;


typedef struct
{
    VOS_UINT32                          ulPlmnNum;
    NAS_MML_PLMN_WITH_RAT_STRU          astPlmnId[NAS_MML_MAX_EHPLMN_NUM];
}NAS_MML_PLMN_LIST_WITH_RAT_STRU;




enum NAS_MML_RAT_FORBIDDEN_LIST_SWITCH_FLAG_ENUM
{
    NAS_MML_RAT_FORBIDDEN_LIST_SWITCH_INACTIVE                   = 0,           /* 功能未激活 */                     
    NAS_MML_RAT_FORBIDDEN_LIST_SWITCH_BLACK                      = 1,           /* 开启黑名单功能 */                 
    NAS_MML_RAT_FORBIDDEN_LIST_SWITCH_WHITE                      = 2,           /* 开启白名单功能 */ 
    NAS_MML_RAT_FORBIDDEN_LIST_SWITCH_BUTT
};
typedef VOS_UINT8 NAS_MML_RAT_FORBIDDEN_LIST_SWITCH_FLAG_ENUM_UINT8;
enum NAS_MML_PLATFORM_SUPPORT_RAT_ENUM
{
    NAS_MML_PLATFORM_SUPPORT_RAT_GERAN                   = 0,           /* GERAN */                     
    NAS_MML_PLATFORM_SUPPORT_RAT_UTRAN                   = 1,           /* UTRAN包括WCDMA/TDS-CDMA */                 
    NAS_MML_PLATFORM_SUPPORT_RAT_EUTRAN                  = 2,           /* E-UTRAN */ 
    NAS_MML_PLATFORM_SUPPORT_RAT_BUTT
};
typedef VOS_UINT8 NAS_MML_PLATFORM_SUPPORT_RAT_ENUM_UINT8;
typedef struct
{
    NAS_MML_RAT_FORBIDDEN_LIST_SWITCH_FLAG_ENUM_UINT8   enSwitchFlag;                                           /*功能是否有效及功能的类型  */
    VOS_UINT8                                           ucImsiListNum;                                          /*功能起效的SIM卡数目(黑名单/白名单)  */
    VOS_UINT8                                           ucForbidRatNum;                                         /*禁止RAT的数目  */
    VOS_UINT8                                           aucReserve[1];
    NAS_MML_PLMN_ID_STRU                                astImsiList[NAS_MML_MAX_IMSI_FORBIDDEN_LIST_NUM];       /* SIM卡列表 (黑名单/白名单) */
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8                     aenForbidRatList[NAS_MML_MAX_RAT_FORBIDDEN_LIST_NUM];   /*禁止的接入技术  */
}NAS_MML_RAT_FORBIDDEN_LIST_STRU;
enum NAS_MML_RAT_CAPABILITY_STATUS_ENUM
{
    NAS_MML_RAT_CAPABILITY_STATUS_DISABLE                   = 0,                       
    NAS_MML_RAT_CAPABILITY_STATUS_REENABLE                  = 1,                      
    NAS_MML_RAT_CAPABILITY_STATUS_BUTT
};
typedef VOS_UINT8 NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8;



typedef struct
{
    NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8   ucGsmCapaStatus;
    NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8   ucUtranCapaStatus;
    NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8   ucLteCapaStatus;
    VOS_UINT8                                  ucIsImsiInForbiddenListFlg;
}NAS_MML_RAT_FORBIDDEN_STATUS_STRU;



typedef struct
{
    VOS_UINT8                               ucWhitePlmnLockNum;                     /* 支持白名单的个数,个数为0时表示不支持白名单 */
    VOS_UINT8                               ucBlackPlmnLockNum;                     /* 支持黑名单的个数,个数为0时表示不支持黑名单 */
    VOS_UINT8                               aucReserve[2];
    NAS_MML_PLMN_ID_STRU                    astWhitePlmnId[NAS_MML_MAX_WHITE_LOCK_PLMN_NUM];
    NAS_MML_PLMN_ID_STRU                    astBlackPlmnId[NAS_MML_MAX_BLACK_LOCK_PLMN_NUM];
    NAS_MML_DISABLED_RAT_PLMN_CFG_INFO_STRU stDisabledRatPlmnCfg;               /* 禁止接入技术的PLMN定制特性 */
}NAS_MML_PLMN_LOCK_CFG_INFO_STRU;
typedef struct
{
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                  enRegCause;
    NAS_MML_REG_DOMAIN_ENUM_UINT8                       enDomain;
    NAS_MML_SINGLE_DOMAIN_REG_FAIL_ACTION_ENUM_UINT8    enAction;
}NAS_MML_SINGLE_DOMAIN_REG_FAIL_ACTION_STRU;


typedef struct
{
    VOS_UINT8                                           ucActiveFlag;
    VOS_UINT8                                           ucCount;
    VOS_UINT8                                           auReserv[2];
    NAS_MML_SINGLE_DOMAIN_REG_FAIL_ACTION_STRU	        astSingleDomainFailActionList[NAS_MML_SINGLE_DOMAIN_FAIL_ACTION_MAX_LIST];
}NAS_MML_SINGLE_DOMAIN_FAIL_ACTION_LIST_CTX_STRU;


typedef struct
{
    VOS_UINT8                                           ucAdditionalLauFlag;
    VOS_UINT8                                           auReserv[3];
    NAS_MML_LAI_STRU                                    stLai;
}NAS_MML_SOR_ADDITIONAL_LAU_STRU;


typedef struct
{
    VOS_UINT8                                           ucStkSteeringOfRoamingSupportFlg;       /* 是否支持stk streer of roaming这个特性,VOS_FALSE:不支持,VOS_TRUE:支持 */
    VOS_UINT8                                           ucCsRejSearchSupportFlg;                /* vodafone的搜网定制需求,CS域失败四次后下发搜网请求,VOS_FALSE:不支持,VOS_TRUE:支持 */
    VOS_UINT8                                           ucRoamBrokerActiveFlag;                 /* Roam broker特性激活的标志:VOS_TRUE:特性打开，VOS_FALSE:特性未打开，*/
    VOS_UINT8                                           ucRoamBrokerRegisterFailCnt;            /* Roam broker特性注册失败的次数*/
    VOS_UINT8                                           ucMaxForbRoamLaFlg;                     /* NV中设置的最大禁止LA个数是否有效: VOS_TRUE:valid;VOS_FALSE:INVALID */
    VOS_UINT8                                           ucMaxForbRoamLaNum;                     /* NV中设置的最大禁止LA个数 */
    VOS_UINT8                                           ucNvGsmForbidFlg;                       /* NV指示GSM是否禁止标志。VOS_TRUE:YES;VOS_FALSE:NO */
    VOS_UINT8                                           ucSingleDomainFailPlmnSrchFlag;         /* DT定制需求，单域注册被拒后，需要出发搜网 */
    VOS_UINT8                                           ucPsOnlyCsServiceSupportFlg;            /* 服务域设置为PS ONLY时，是否支持CS域短信和呼叫业务(紧急呼叫除外)*/
    VOS_UINT8                                           ucWcdmaPriorityGsmFlg;                  /*H3G需求， W网络优先于GSM网络*/
    VOS_UINT16                                          usAppConfigSupportFlg;                  /*控制应用版本*/

	NAS_MML_SINGLE_DOMAIN_FAIL_ACTION_LIST_CTX_STRU     stSingleDomainFailActionCtx;
    VOS_UINT8                                           ucRegFailNetWorkFailureCustomFlg;

	VOS_UINT8                                           ucUserAutoReselActiveFlg;                    /* 是否允许LTE国际漫游标记:VOS_TRUE 表示允许LTE国际漫游 VOS_FALSE 表示禁止LTE国际漫游 */

    VOS_UINT8                                           ucSortAvailalePlmnListRatPrioFlg;    /* 是否按syscfg设置接入技术优先级排序高低质量搜网列表标识，1:是; 0:不是高质量网络按随机排序低质量网络不排序*/
    /* 服务域设置为CS ONLY时，是否支持PS域短信和数据业务 */
    VOS_UINT8                                           ucCsOnlyDataServiceSupportFlg;


    VOS_UINT8                                           ucLteDisabledRauUseLteInfoFlag;  /* L disable时rau是否需要从l下获取安全上下文或guti映射信息，VOS_TRUE:L disabled rau从l获取安全上下文或guti映射信息；VOS_FALSE:L disable的rau不从l获取安全上下文或guti映射信息 */
    VOS_UINT8                                           aucReserved[3];

    NAS_MML_SOR_ADDITIONAL_LAU_STRU                     stAdditionalLau;
}NAS_MML_MISCELLANEOUS_CFG_INFO_STRU;


/*****************************************************************************
 结构名    : NAS_MML_AVAIL_TIMER_CFG_STRU
 结构说明  : NVIM项中的丢网后的搜网配置
 1.日    期   : 2011年6月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulFirstSearchTimeLen;                   /* available timer定时器第一次的时长 */
    VOS_UINT32                          ulFirstSearchTimeCount;                 /* available timer定时器第一次的次数 */
    VOS_UINT32                          ulDeepSearchTimeLen;                    /* available timer定时器深睡的时长 */
    VOS_UINT32                          ulDeepSearchTimeCount;
}NAS_MML_AVAIL_TIMER_CFG_STRU;
typedef struct
{
    VOS_UINT8                           ucLteRoamAllowedFlg;                    /* 是否允许LTE国际漫游标记:VOS_TRUE 表示允许LTE国际漫游 VOS_FALSE 表示禁止LTE国际漫游 */
    VOS_UINT8                           aucReserve[3];
    VOS_UINT32                          aulRoamEnabledMccList[NAS_MML_ROAM_ENABLED_MAX_MCC_NUMBER];/* 允许漫游的国家码列表 */
}NAS_MML_LTE_INTERNATION_ROAM_CFG_STRU;


typedef struct
{
    VOS_UINT8                           ucEnableFlg;                            /* 拨号被拒是否使能 VOS-TRUE:拨号被拒支持 VOS_FALSE:拨号被拒不支持 */
    VOS_UINT8                           aucReserve[3];
}NAS_MML_DAIL_REJECT_CFG_STRU;
enum NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_ENUM
{
    NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_INACTIVE,      /* 功能不生效 */                                             
    NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_CS_PS,         /* 修改CS+PS的拒绝原因值 */                                         
    NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_CS_ONLY,       /* 仅修改CS域的拒绝原因值 */                                     
    NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_PS_ONLY,       /* 仅修改PS域的拒绝原因值 */
    NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_BUTT
};
typedef VOS_UINT8 NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_ENUM_UINT8;


typedef struct
{
    VOS_UINT8   ucGmmHplmnRejCauseChangedCounter;                  /* GMM将拒绝原因值修改为#17的次数 */
    VOS_UINT8   ucMmHplmnRejCauseChangedCounter;                   /* MM将拒绝原因值修改为#17的次数 */
    VOS_UINT8   aucReserve[2];
}NAS_MML_HPLMN_REJ_CAUSE_CHANGED_COUNTER_STRU;

typedef struct
{
    NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_ENUM_UINT8               enChangeRegRejCauseFlg;
    VOS_UINT8   ucPreferredRegRejCau_HPLMN_EHPLMN;             /* HPLMN/EHPLMN时使用的拒绝原因值 */
    VOS_UINT8   ucPreferredRegRejCau_NOT_HPLMN_EHPLMN;         /* 非HPLMN/EHPLMN时使用的拒绝原因值 */
    VOS_UINT8   aucReserve[1];
    NAS_MML_HPLMN_REJ_CAUSE_CHANGED_COUNTER_STRU            stHplmnRejCauseChangedCounter;
}NAS_MML_CHANGE_REG_REJ_CAUSE_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucIgnoreAuthRejFlg;
    VOS_UINT8                           ucHplmnCsAuthRejCounter;
    VOS_UINT8                           ucHplmnPsAuthRejCounter;
    VOS_UINT8                           ucMaxAuthRejNo;
}NAS_MML_IGNORE_AUTH_REJ_INFO_STRU;


typedef struct
{
    VOS_UINT32                                              ulHoWaitSysinfoTimeLen;  /* CSFB HO 流程等系统消息时长,单位为毫秒 */

    NAS_MML_LTE_INTERNATION_ROAM_CFG_STRU                   stLteRoamCfg;       /* LTE国际漫游定制特性 */


    VOS_UINT8                                               ucCsfbEmgCallLaiChgLauFirstFlg;   /* csfb 紧急呼到GU,LAI改变先做lau还是先进行呼叫标志，
                                                                                  VOS_TRUE:LAI改变先做lau再进行紧急呼叫；VOS_FALSE:LAI改变先进行紧急呼叫结束后做LAU*/
    VOS_UINT8                                               ucIsrSupportFlg;

    VOS_UINT8                                               ucIsRauNeedFollowOnCsfbMtFlg;
    VOS_UINT8                                               ucIsDelFddBandSwitchOnFlg;


    VOS_UINT32                                              ulCsPsMode1EnableLteTimerLen;  /* 1)cs ps mode1 L联合注册eps only成功cs被拒#16/#17/#22达最大次数场景disable lte时启动enable lte定时器时长
                                                                                               2)cs ps mode1 L联合注册cs eps均失败原因值other cause 达最大次数场景disable lte启动enable lte定时器时长 */

    VOS_UINT32                                              ulCsfbEmgCallEnableLteTimerLen; /* L下紧急呼叫无法正常csfb到gu，通过搜网到gu场景disable lte启动enable lte定时器时长,单位:毫秒*/
    VOS_UINT8                                               ucDisableLTEOnBandFlg;       /* 根据WCDMA BAND禁用LTE */
    VOS_UINT8                                               ucUltraFlashCsfbSupportFlg;  /* Ultra Flash Csfb是否支持 */
    VOS_UINT8                                               ucIsRauNeedFollowOnCsfbMoFlg;
    VOS_UINT8                                               aucReserve[1];
}NAS_MML_LTE_CUSTOM_CFG_INFO_STRU;
typedef struct
{
    VOS_UINT8                           ucImsVoiceInterSysLauEnable;            /* Ims Voice 可用时，进行L->GU异系统切换，是否需要强制进行LAU */
    VOS_UINT8                           ucImsVoiceMMEnable;          /* Ims Voice 移动性管理是否打开  VOS_TRUE--打开，VOS_FALSE--关闭*/
    VOS_UINT8                           acReserved[2];
}NAS_MML_IMS_VOICE_MOBILE_MANAGEMENT;

typedef struct
{
    NAS_MML_RPLMN_CFG_INFO_STRU         stRplmnCfg;                             /* RPLMN的定制特性 */
    NAS_MML_HPLMN_CFG_INFO_STRU         stHplmnCfg;                             /* HPLMN的定制特性 */
    NAS_MML_ROAM_CFG_INFO_STRU          stRoamCfg;                              /* ROAM的定制特性 */
    NAS_MML_BG_SEARCH_CFG_INFO_STRU     stBgSearchCfg;                          /* BG搜索的定制特性 */
    NAS_MML_PLMN_LOCK_CFG_INFO_STRU     stPlmnLockCfg;                          /* 锁网定制需求,黑名单或白名单 */
    NAS_MML_AVAIL_TIMER_CFG_STRU        stAvailTimerCfg;                        /* 丢网或进入限制服务的搜网时间定制需求 */
    NAS_MML_MISCELLANEOUS_CFG_INFO_STRU stMiscellaneousCfg;                     /* 运营商特殊的定制特性 */
    NAS_MML_DAIL_REJECT_CFG_STRU        stDailRejectCfg;                        /* 拨号被拒定制特性 */

    VOS_UINT8                           ucPlmnExactlyCompareFlg;                /*Plmn精确比较标记,使用同一MCC混用2位与3位MNC的国家并不是很多，
                                                                                 我们现在的做法是MNC高位是0或F时，只要MNC的低两位相同，
                                                                                 就认为这两个MNC是相同的，现在需要加以区分，比如410-310和410-31这两个MCC相同的PLMN，*/
    VOS_UINT8                           ucSupportCsServiceFlg;                  /* 是否支持cs语音业务包括普通语音和vp业务，VOS_TRUE:支持cs语音业务；VOS_FALSE:不支持cs语音业务*/

    VOS_UINT8                           ucHplmnRegisterCtrlFlg;                 /* HPLMN上注册被拒后是否允许重复注册标记 */


    VOS_UINT8                           ucH3gCtrlFlg;

    NAS_MML_LTE_CUSTOM_CFG_INFO_STRU    stLteCustomCfgInfo;

    NAS_MML_CHANGE_REG_REJ_CAUSE_INFO_STRU stChangeRegRejCauInfo;

    NAS_MML_RAT_FORBIDDEN_LIST_STRU             stRatForbiddenListInfo;
    NAS_MML_RAT_FORBIDDEN_STATUS_STRU           stRatFirbiddenStatusCfg;

    NAS_MML_IGNORE_AUTH_REJ_INFO_STRU           stIgnoreAuthRejInfo;

    NAS_MML_USER_CFG_OPLMN_INFO_STRU    stUserCfgOPlmnInfo;
    VOS_UINT8                           ucSupportAccBarPlmnSearchFlg;         /* 非HPLMN/RPLMN接入禁止后是否发起PLMN搜网，*/

    VOS_UINT8                           ucSvlteSupportFlag;

    VOS_UINT16                          usDsdsRfShareSupportFlg;
	
    VOS_UINT8                           ucLcEnableFLg;

    /* 从NV中读取是否需要强制LAU和IMS移动性管理的nv */
    NAS_MML_IMS_VOICE_MOBILE_MANAGEMENT stImsVoiceMM;
    VOS_UINT8                           uc3GPPUplmnNotPrefFlg;
    VOS_UINT8                           ucReserve[3];


}NAS_MML_CUSTOM_CFG_INFO_STRU;


/*****************************************************************************
 结构名    : NAS_MML_MS_CFG_INFO_STRU
 结构说明  : MS支持的MS配置能力
 1.日    期   : 2011年6月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    NAS_MML_MS_3GPP_REL_STRU            stMs3GppRel;                            /* 支持的协议版本 */
    NAS_MML_MS_CAPACILITY_INFO_STRU     stMsCapability;                         /* MS的手机能力 */
    NAS_MML_MS_SYS_CFG_INFO_STRU        stMsSysCfgInfo;                         /* 系统配置信息 */
    NAS_MML_CUSTOM_CFG_INFO_STRU        stCustomCfg;                            /* NVIM中的定制信息 */
}NAS_MML_MS_CFG_INFO_STRU;


typedef struct
{
    VOS_UINT32                      ulCellId;                       /*小区ID*/

    VOS_INT16                       sCellRssi;
    VOS_INT16                       sCellRSCP;                      /*小区信号质量*/

} NAS_MMC_CELL_VALUE_STRU;


typedef struct
{
    VOS_UINT8                       ucRssiLevel;                    /* RSSI等级 */
    VOS_UINT8                       ucChannalQual;                  /* 信号质量 */
    VOS_UINT8                       aucReserve[2];

    VOS_INT16                       sRssiValue;                     /* 信号质量 */
    VOS_INT16                       sRscpValue;
} NAS_MMC_RSSI_VALUE_STRU;
typedef struct
{
    VOS_UINT16                  usArfcn;                                /* 小区Arfcn */

    VOS_UINT8                   ucCellNum;                              /*小区数目*/
    VOS_UINT8                   ucRssiNum;                              /* 信号质量个数 */
    NAS_MMC_CELL_VALUE_STRU     astCellInfo[NAS_MML_AT_CELL_MAX_NUM];   /*小区ID*/
    NAS_MMC_RSSI_VALUE_STRU     astRssiInfo[NAS_MML_AT_CELL_MAX_NUM];   /*信号质量*/
    VOS_UINT16                  usCellDlFreq;                           /*小区频点(下行)*/
    VOS_UINT16                  usCellUlFreq;                           /*当前频点(上行)*/
    VOS_INT16                   sUeRfPower;                             /*发射功率*/
}NAS_MML_CAMP_CELL_INFO_STRU;

/*****************************************************************************
 结构名    : NAS_MML_CAMP_PLMN_INFO_STRU
 结构说明  : MMC当前驻留PLMN的信息
 1.日    期   : 2011年5月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    NAS_MML_PLMN_ID_STRU                stOperatorPlmnId;
    VOS_UINT8                           aucOperatorNameShort[NAS_MML_MAX_OPER_SHORT_NAME_LEN];/* 当前驻留网络运营商的短名称 */
    VOS_UINT8                           aucOperatorNameLong[NAS_MML_MAX_OPER_LONG_NAME_LEN];  /* 当前驻留网络运营商的长名称 */
}NAS_MML_OPERATOR_NAME_INFO_STRU;



typedef struct
{
    VOS_UINT8                           ucUtranNcellExist;
    VOS_UINT8                           ucLteNcellExist;
    VOS_UINT8                           aucReserve[2];
}NAS_MML_RRC_NCELL_INFO_STRU;



typedef struct
{
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enNetRatType;                           /* 当前的网络接入技术 */
    NAS_MML_NET_MODE_ENUM_UINT8         enNetworkMode;                          /* 当前驻留的网络模式 */
    NAS_MML_RRC_SYS_SUBMODE_ENUM_UINT8  enSysSubMode;                           /* 当前驻留网络的系统模式 */

    VOS_UINT8                           ucReserve;

    NAS_MML_LAI_STRU                    stLai;                                  /* 当前驻留网络的PLMN ID和位置区 */
    VOS_UINT8                           ucRac;                                  /* 当前驻留网络的RAC */
    VOS_UINT8                           aucReserve[3];
    NAS_MML_CAMP_CELL_INFO_STRU         stCampCellInfo;                         /* 当前驻留的小区信息 */
    NAS_MML_OPERATOR_NAME_INFO_STRU     stOperatorNameInfo;                     /* 当前驻留的运营商名称信息 */
    NAS_MML_RRC_NCELL_INFO_STRU         stRrcNcellInfo;
}NAS_MML_CAMP_PLMN_INFO_STRU;
typedef struct
{
    VOS_UINT8                           ucRestrictPagingRsp;          /*是否限制响应寻呼 */
    VOS_UINT8                           ucRestrictRegister;           /*是否限制注册 */
    VOS_UINT8                           ucRestrictNormalService;      /* 是否限制正常业务 */
    VOS_UINT8                           ucRestrictEmergencyService;   /* 是否限制紧急业务 */
}NAS_MML_ACCESS_RESTRICTION_STRU;

typedef struct
{
    NAS_MML_TIMER_INFO_ENUM_UINT8                      enT3412Status;           /* T3412定时器的运行状态:0-停止；1-运行；2-超时*/
    NAS_MML_TIMER_INFO_ENUM_UINT8                      enT3423Status;           /* T3423定时器的运行状态:0-停止；1-运行；2-超时*/
    NAS_MML_ADDITION_UPDATE_RSLT_INFO_ENUM_UINT8       enAdditionUpdateRsltInfo;/* 联合attach或tau网侧回复结果中Additional update result IE内容 */
    NAS_MML_REG_STATUS_ENUM_UINT8                      enEpsRegStatus;           /* PS域的注册结果 */

    VOS_UINT8                                          ucIsRelCauseCsfbHighPrio; /* VOS_TRUE表示发起呼叫建立连接时填原因RRC_EST_CAUSE_TERMINAT_HIGH_PRIORITY_SIGNAL
                                                                                 VOS_FALSE表示发起呼叫建立连接时填原因RRC_EST_CAUSE_TERMINAT_CONVERSAT_CALL*/
    VOS_UINT8                                          aucReserved[2];


    NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8                     enNwImsVoCap; /* EPS网络对IMS Voice 支持 */
}NAS_MML_EPS_DOMAIN_INFO_STRU;
typedef struct
{
    NAS_MML_PS_BEARER_STATE_ENUM_UINT8                 enPsBearerState;         /* PS承载状态激活还是去激活 */
    NAS_MML_PS_BEARER_ISR_ENUM_UINT8                   enPsBearerIsrFlg;        /* PS承载激活是在ISR激活前还是激活后 */
    VOS_UINT8                                          ucPsActPending;          /* 正在激活过程中的PS承载 */
    VOS_UINT8                                          aucReserved[1];
}NAS_MML_PS_BEARER_CONTEXT_STRU;
typedef struct
{
    VOS_UINT8                           ucCategory;                             /* 紧急呼叫号码类型 */
    VOS_UINT8                           ucEmcNumLen;                            /*紧急呼号码长度，即aucEmcNum的有效长度*/
    VOS_UINT8                           aucEmcNum[NAS_MML_EMERGENCY_NUM_MAX_LEN];
}NAS_MML_EMERGENCY_NUM_STRU;


typedef struct
{
    VOS_UINT8                      ucEmergencyNumber;                           /* 紧急呼号码个数 */
    VOS_UINT8                      ucReserved[3];
    VOS_UINT32                     ulMcc;
    NAS_MML_EMERGENCY_NUM_STRU     aucEmergencyList[NAS_MML_EMERGENCY_NUM_LIST_MAX_RECORDS]; /* 紧急呼号码列表 */

}NAS_MML_EMERGENCY_NUM_LIST_STRU;



typedef struct
{
    VOS_UINT8                           ucCsSupportFlg;                         /* 当前CS域是否支持 */
    VOS_UINT8                           ucAttFlg;                               /* CS域需要ATT的标志,VOS_FALSE:不需要,VOS_TRUE:需要  */
    VOS_UINT8                           ucCsAttachAllow;                        /* CS域是否允许注册,VOS_TRUE:允许CS域注册,VOS_FALSE:不允许CS域注册 */
    NAS_MML_REG_STATUS_ENUM_UINT8       enCsRegStatus;                          /* CS域的注册结果 */
    NAS_MML_ACCESS_RESTRICTION_STRU     stCsAcRestriction;                      /* 当前CS域接入受限情况 */
    VOS_UINT32                          ulT3212Len;                             /* T3212定时器的时长 */
    VOS_UINT32                          ulCsDrxLen;                             /* CS域DRX的长度 */
    VOS_UINT32                          ulCsRegisterBarToUnBarFlag;             /* CS域从Bar到UnBar的转变标志 */
    NAS_MML_LAI_STRU                    stLastSuccLai;                          /* CS域最后一次注册成功的LAI信息或注册失败后需要删除LAI，则该值为无效值 */
}NAS_MML_CS_DOMAIN_INFO_STRU;
typedef struct
{
    VOS_UINT8                                               ucSplitPgCycleCode; /* SPLIT PG CYCLE CODE*/
    VOS_UINT8                                               ucUeUtranPsDrxLen;  /* CN Specific DRX cycle length coefficien */
    VOS_UINT8                                               ucUeEutranPsDrxLen; /* DRX value for S1 mode */
    VOS_UINT8                                               ucWSysInfoDrxLen;   /* Was系统消息中携带的DrxLength */
    VOS_UINT8                                               ucLSysInfoDrxLen;   /* L系统消息中携带的DrxLength */
    VOS_UINT8                                               ucSplitOnCcch;      /* SPLIT on CCCH */
    VOS_UINT8                                               ucNonDrxTimer;      /* non-DRX  timer*/
    NAS_MML_PS_REG_CONTAIN_DRX_PARA_ENUM_UINT8              enPsRegisterContainDrx; /* ps注册是否携带了DRX参数*/
}NAS_MML_PS_DOMAIN_DRX_PARA_STRU;

typedef struct
{
    VOS_UINT8                           ucPsSupportFlg;                         /* 当前CS域是否支持 */
    VOS_UINT8                           ucPsAttachAllow;                        /* PS域是否允许注册,VOS_TRUE:允许PS域注册,VOS_FALSE:不允许PS域注册 */
    NAS_MML_REG_STATUS_ENUM_UINT8       enPsRegStatus;                          /* PS域的注册结果 */
    VOS_UINT8                           ucPsLocalDetachFlag;                    /* svlte形态，gmm收到mtc的丢网指示会更新该变量为vos_true,gmm或lnas
                                                                                   后续收到detach ps请求做本地detach*/

    NAS_MML_ACCESS_RESTRICTION_STRU     stPsAcRestriction;                      /* 当前PS域接入受限情况 */
    VOS_UINT32                          ulPsRegisterBarToUnBarFlag;             /* PS域从Bar到UnBar的转变标志 */
    NAS_MML_RAI_STRU                    stLastSuccRai;                          /* PS域最后一次注册成功的RAI信息或注册失败后需要删除RAI，则该值为无效值 */

    NAS_MML_PS_DOMAIN_DRX_PARA_STRU     stPsDomainDrxPara;                      /* PS域DRX参数信息 */
    NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8 enNwImsVoCap; /* PS网络下Ims Voice 支持能力 */
    VOS_UINT8                           acReserved[3];
}NAS_MML_PS_DOMAIN_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucEquPlmnNum;                           /* 当前EquPLMN的个数 */
    VOS_UINT8                           ucValidFlg;                             /* 等效PLMN有效标记:VOS_TRUE:EPLMN有效,VOS_FALSE:EPLMN无效 */
                                                                                /* EPLMN被删除或EPLMN个数为0时无效,其它情况都有效 */
    VOS_UINT8                           aucReserved[2];
    NAS_MML_PLMN_ID_STRU                astEquPlmnAddr[NAS_MML_MAX_EQUPLMN_NUM];/* 当前EquPLMN列表 */
}NAS_MML_EQUPLMN_INFO_STRU;
typedef struct
{
    VOS_UINT8                           ucCsSigConnStatusFlg;                   /* CS域信令连接是否存在,VOS_FALSE:不存在,VOS_TRUE:存在 */
    VOS_UINT8                           ucPsSigConnStatusFlg;                   /* PS域信令连接是否存在,VOS_FALSE:不存在,VOS_TRUE:存在 */
    VOS_UINT8                           ucPsTbfStatusFlg;                       /* TBF连接是否存在,VOS_FALSE:不存在,VOS_TRUE:存在 */
    VOS_UINT8                           ucRrcStatusFlg;                         /* RRC连接是否存在,VOS_FALSE:不存在,VOS_TRUE:存在 */
    VOS_UINT8                           ucCsServiceConnStatusFlg;               /* CS域业务连接是否存在,VOS_FALSE:不存在,VOS_TRUE:存在 */
    VOS_UINT8                           ucCsServiceBufferFlg;                   /* CS域缓存业务是否存在,VOS_FALSE:不存在,VOS_TRUE:存在 */
    VOS_UINT8                           ucPsServiceBufferFlg;                   /* PS域缓存业务是否存在,VOS_FALSE:不存在,VOS_TRUE:存在,此变量暂时不要更新,主要原因为MMC暂时不需要判断 */
    VOS_UINT8                           ucPdpStatusFlg;                         /* PDP是否激活,VOS_FALSE:不存在,VOS_TRUE:存在 */
    VOS_UINT8                           ucEpsSigConnStatusFlg;                  /* EPS域信令连接是否存在,VOS_FALSE:不存在,VOS_TRUE:存在 */
    VOS_UINT8                           ucEpsServiceConnStatusFlg;              /* EPS域业务连接是否存在,VOS_FALSE:不存在,VOS_TRUE:存在 */
    VOS_UINT8                           ucEmergencyServiceFlg;                  /* 判断是否是紧急呼叫 */
    VOS_UINT8                           ucPsTcServiceFlg;                       /* PS域触发的TC业务是否存在，VOS_FALSE:不存在，VOS_TRUE:存在
                                                                                   CS域触发的TC业务时就直接使用CS域业务连接存在的标志 */
    VOS_UINT8                           ucEmcPdpStatusFlg;                      /* 紧急PDN是否存在标志,VOS_FALSE:不存在,VOS_TRUE:存在 */
                                                                                   
    NAS_MML_CSFB_SERVICE_STATUS_ENUM_UINT8                  enCsfbServiceStatus;/* 标识当前是否在CSFB流程 */
    VOS_UINT8                                               aucReserved[2];
}NAS_MML_CONN_STATUS_INFO_STRU;


typedef struct
{
    VOS_UINT8                                               ucImsVoiceAvail;         /* VOS_TRUE:  IMS voice可用
                                                                                        VOS_FALSE: IMS voice不可用 */
    NAS_MML_IMS_NORMAL_REG_STATUS_ENUM_UINT8                enImsNormalRegSta;

    VOS_UINT8                                               aucReserved[2];
}NAS_MML_IMS_DOMAIN_INFO_STRU;




typedef struct
{
    NAS_MML_CAMP_PLMN_INFO_STRU         stCampPlmnInfo;                         /* 当前驻留PLMN信息 */
    NAS_MML_CS_DOMAIN_INFO_STRU         stCsDomainInfo;                         /* 当前CS域信息 */
    NAS_MML_PS_DOMAIN_INFO_STRU         stPsDomainInfo;                         /* 当前PS域信息 */
    NAS_MML_MS_BAND_INFO_STRU           stBandInfo;                             /* 当前驻留的BAND信息 */
    NAS_MML_NETWORK_3GPP_REL_STRU       stNetwork3GppRel;                       /* 当前网络的协议版本 */
    NAS_MML_EQUPLMN_INFO_STRU           stEquPlmnInfo;                          /* EQUPLMN信息 */
    NAS_MML_CONN_STATUS_INFO_STRU       stConnStatus;                           /* 当前连接存在状态 */
    NAS_MML_EPS_DOMAIN_INFO_STRU        stEpsDomainInfo;                        /* EPS域信息 */
    NAS_MML_PS_BEARER_CONTEXT_STRU      astPsBearerContext[NAS_MML_MAX_PS_BEARER_NUM];  /* PS承载上下文信息 */
    NAS_MML_EMERGENCY_NUM_LIST_STRU     stEmergencyNumList;

    NAS_MML_IMS_DOMAIN_INFO_STRU        stImsDomainInfo;                        /* IMS域信息 */    
}NAS_MML_NETWORK_INFO_STRU;


typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          usMsgID;                                /* 消息ID */
    VOS_UINT8                           aucReserve[2];

    VOS_UINT8                           aucBuf[NAS_MML_MAX_INTERNAL_MSG_LEN];   /* 消息内容 */
}NAS_MML_INTERNAL_MSG_BUF_STRU;

/*****************************************************************************
 结构名    : NAS_MML_INTERNAL_MSG_QUEUE_STRU
 结构说明  : 保存NAS MM 层内部消息的链表
 1.日    期   : 2011年6月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    /* NAS MM层内部消息队列 */
    NAS_MML_INTERNAL_MSG_BUF_STRU       astNasMmMsgQueue[NAS_MML_MAX_MSG_QUEUE_SIZE];

    /* 发送消息临时缓存buff,申请局部变量时直接使用该空间,为了避免发送内部消息时
    占用较大的栈空间 */
    NAS_MML_INTERNAL_MSG_BUF_STRU       stSendMsgBuf;

    /*内部消息缓冲队列的索引尾指针*/
    VOS_UINT8                           ucTail;

    /*内部消息缓冲队列的索引头指针*/
    VOS_UINT8                           ucHeader;

    VOS_UINT16                          usReserved;
} NAS_MML_INTERNAL_MSG_QUEUE_STRU;



typedef struct
{
    VOS_UINT8                           ucOmConnectFlg;                         /* PC工具的连接状态, VOS_TRUE: 已连接; VOS_FALSE: 未连接 */
    VOS_UINT8                           ucOmPcRecurEnableFlg;                   /* PC工具使能NAS的PC回放标志 , VOS_TRUE:使能; VOS_FALSE: 未使能 */
    VOS_UINT8                           aucRsv2[2];
}NAS_MML_OM_MAINTAIN_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucUeSndPcRecurFlg;                       /* PC回放是否需要导出全局变量,VOS_TRUE:需要, VOS_FALSE:不需要, */
    VOS_UINT8                           ucRsv3[3];
}NAS_MML_UE_MAINTAIN_INFO_STRU;
typedef struct
{
    OM_RING_ID                          pstRingBuffer;                          /* MM层的共享缓存 */
    VOS_UINT8                           ucErrLogCtrlFlag;                       /* ERRLOG打开标识 */
    VOS_UINT8                           ucReserved;                             /* 工程菜单打开标识 */
    VOS_UINT16                          usAlmLevel;                             /* 故障告警级别 */
}NAS_MML_ERRLOG_MNTN_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucFtmCtrlFlag;                          /* 工程菜单打开标识 */
    VOS_UINT8                           aucReserved[3];
}NAS_MML_FTM_MNTN_INFO_STRU;
typedef struct
{
    NAS_MML_OM_MAINTAIN_INFO_STRU       stOmMaintainInfo;
    NAS_MML_UE_MAINTAIN_INFO_STRU       stUeMaintainInfo;
    NAS_MML_ERRLOG_MNTN_INFO_STRU       stErrLogMntnInfo;
    NAS_MML_FTM_MNTN_INFO_STRU          stFtmMntnInfo;

}NAS_MML_MAINTAIN_CTX_STRU;


/*****************************************************************************
 结构名    : NAS_MML_CTX_INFO_STRU
 结构说明  : MML_CTX信息
 1.日    期   : 2011年6月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    NAS_MML_SIM_INFO_STRU               stSimInfo;                              /* SIM信息 */
    NAS_MML_MS_CFG_INFO_STRU            stMsCfgInfo;                            /* MS的配置信息 */
    NAS_MML_NETWORK_INFO_STRU           stNetworkInfo;                          /* 当前驻留的网络信息 */
    NAS_MML_INTERNAL_MSG_QUEUE_STRU     stInternalMsgQueue;                     /* MM子层的内部消息队列 */
    NAS_MML_MAINTAIN_CTX_STRU           stMaintainInfo;
}NAS_MML_CTX_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
NAS_MML_SIM_FORBIDPLMN_INFO_STRU* NAS_MML_GetForbidPlmnInfo( VOS_VOID );

NAS_MML_BG_SEARCH_CFG_INFO_STRU* NAS_MML_GetBgSearchCfg( VOS_VOID );

NAS_MML_CONN_STATUS_INFO_STRU* NAS_MML_GetConnStatus(VOS_VOID);

VOS_VOID NAS_MML_SetRrcConnStatusFlg(
    VOS_UINT8                           ucRrcStatusFlg
);

VOS_VOID NAS_MML_SetCsSigConnStatusFlg(
    VOS_UINT8                           ucCsSigConnStatusFlg
);

VOS_UINT8 NAS_MML_GetCsSigConnStatusFlg(VOS_VOID);

VOS_VOID NAS_MML_SetPsSigConnStatusFlg(
    VOS_UINT8                           ucPsSigConnStatusFlg
);

VOS_VOID NAS_MML_SetPsTbfStatusFlg(
    VOS_UINT8                           ucPsTbfStatusFlg
);

VOS_UINT8 NAS_MML_GetPsTbfStatusFlg( VOS_VOID );


VOS_VOID NAS_MML_SetEpsSigConnStatusFlg(
    VOS_UINT8                           ucEpsSigConnStatusFlg
);

VOS_VOID NAS_MML_SetEpsServiceConnStatusFlg(
    VOS_UINT8                           ucEpsServiceConnStatusFlg
);

VOS_VOID NAS_MML_SetCsServiceConnStatusFlg(
    VOS_UINT8                           ucCsServiceConnStatusFlg
);

VOS_UINT8 NAS_MML_GetCsServiceConnStatusFlg(VOS_VOID);

VOS_VOID NAS_MML_SetCsServiceBufferStatusFlg(
    VOS_UINT8                           ucCsServiceBufferStatusFlg
);

VOS_VOID NAS_MML_SetPsServiceBufferStatusFlg(
    VOS_UINT8                           ucPsServiceBufferStatusFlg
);

VOS_VOID NAS_MML_SetCsEmergencyServiceFlg(
    VOS_UINT8                           ucEmergencyServiceFlg
);

VOS_UINT8 NAS_MML_GetCsEmergencyServiceFlg(VOS_VOID );
VOS_VOID NAS_MML_SetPsTcServiceFlg(
    VOS_UINT8                           ucPsTcServiceFlg
);

NAS_MML_CS_DOMAIN_INFO_STRU* NAS_MML_GetCsDomainInfo( VOS_VOID );

NAS_MML_LAI_STRU* NAS_MML_GetCsLastSuccLai( VOS_VOID );

NAS_MML_RAI_STRU* NAS_MML_GetPsLastSuccRai( VOS_VOID );

VOS_UINT8 NAS_MML_GetCsRejSearchSupportFlg( VOS_VOID );
VOS_UINT8 NAS_MML_GetRoamBrokerActiveFlag( VOS_VOID );

VOS_VOID NAS_MML_SetCsRejSearchSupportFlg(
    VOS_UINT8                           ucCsRejSearchSupportFlg
);


VOS_UINT8 NAS_MML_GetDtSingleDomainPlmnSearchFlag( VOS_VOID );

VOS_VOID NAS_MML_SetDtSingleDomainPlmnSearchFlag(
    VOS_UINT8                           ucPlmnSrchFlag
);

VOS_UINT8 NAS_MML_GetRoamBrokerRegisterFailCnt( VOS_VOID );

VOS_VOID NAS_MML_SetRoamBrokerRegisterFailCnt(
    VOS_UINT8                           ucRoamBrokerRegisterFailCnt
);


NAS_MML_MS_BAND_INFO_STRU* NAS_MML_GetCurrBandInfo( VOS_VOID );


NAS_MML_CAMP_PLMN_INFO_STRU* NAS_MML_GetCurrCampPlmnInfo( VOS_VOID );

NAS_MML_LAI_STRU*  NAS_MML_GetCurrCampLai( VOS_VOID );

VOS_UINT8 NAS_MML_GetCurrCampRac(VOS_VOID);

VOS_UINT16 NAS_MML_GetCurrCampArfcn(VOS_VOID);

VOS_VOID NAS_MML_SetCurrCampArfcn(
    VOS_UINT16                          usArfcn
);

NAS_MML_CAMP_CELL_INFO_STRU* NAS_MML_GetCampCellInfo( VOS_VOID );


NAS_MML_NET_RAT_TYPE_ENUM_UINT8  NAS_MML_GetCurrNetRatType( VOS_VOID );

VOS_VOID  NAS_MML_SetCurrNetRatType(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enNetRatType
);

VOS_VOID  NAS_MML_SetCurrSysSubMode(
    NAS_MML_RRC_SYS_SUBMODE_ENUM_UINT8  enSysSubMode
);
NAS_MML_PLMN_ID_STRU*  NAS_MML_GetCurrCampPlmnId( VOS_VOID );

NAS_MML_EQUPLMN_INFO_STRU* NAS_MML_GetEquPlmnList( VOS_VOID );

NAS_MML_HPLMN_CFG_INFO_STRU* NAS_MML_GetHplmnCfg( VOS_VOID );

NAS_MML_CTX_STRU*  NAS_MML_GetMmlCtx( VOS_VOID );

NAS_MML_MS_3GPP_REL_STRU* NAS_MML_GetMs3GppRel(VOS_VOID);

NAS_MML_MS_CAPACILITY_INFO_STRU* NAS_MML_GetMsCapability(VOS_VOID);

VOS_UINT8* NAS_MML_GetImeisv( VOS_VOID );

NAS_MML_MS_CFG_INFO_STRU* NAS_MML_GetMsCfgInfo( VOS_VOID );

NAS_MML_MS_MODE_ENUM_UINT8 NAS_MML_GetMsMode( VOS_VOID);


NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8 NAS_MML_GetLteCsServiceCfg( VOS_VOID);




VOS_UINT8   NAS_MML_GetLteRoamAllowedFlg(VOS_VOID);

VOS_UINT32 NAS_MML_GetDisableLteRoamFlg( VOS_VOID);

VOS_VOID NAS_MML_GetRoamEnabledMccList(VOS_UINT32 *pulMcc);

VOS_UINT8 NAS_MML_GetDailRejectCfg( VOS_VOID);

VOS_VOID NAS_MML_SetDisableLteRoamFlg(
    VOS_UINT32                          ulDisableFlg
);

VOS_VOID NAS_MML_SetRoamEnabledMccList(VOS_UINT32 *pulMcc);

VOS_VOID NAS_MML_SetRoamRplmnSearchFlg(VOS_UINT8 pulRoamRplmnFlg);
VOS_UINT8 NAS_MML_GetRoamRplmnSearchFlg(VOS_VOID);
VOS_VOID NAS_MML_SetDisableLTEOnBandFlg(VOS_UINT8 ucDisableLTEOnBandFlg);
VOS_UINT8 NAS_MML_GetDisableLTEOnBandFlg(VOS_VOID);
VOS_UINT32 *NAS_MML_GetNationalMCCList(VOS_VOID);
VOS_VOID NAS_MML_SetNationalMCCList(VOS_UINT32 *pulMcc);

VOS_VOID NAS_MML_SetUltraFlashCsfbSupportFlg(
    VOS_UINT8                           ucUltraFlashCsfbSupportFlg
);
VOS_UINT8 NAS_MML_GetUltraFlashCsfbSupportFlg(VOS_VOID);

VOS_VOID NAS_MML_SetLteRoamAllowedFlg(
    VOS_UINT8                           ucAllowedFlg
);

VOS_VOID NAS_MML_SetDailRejectCfg(
    VOS_UINT8                           ucEnableFlg
);


VOS_VOID NAS_MML_SetChangeRegRejCauFlg(
    NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_ENUM_UINT8            enEnableFlg
);

VOS_VOID NAS_MML_SetPreferredRegRejCause_HPLMN_EHPLMN(
    VOS_UINT8                           ucRejectCause
);

VOS_VOID NAS_MML_SetPreferredRegRejCause_NOT_HPLMN_EHPLMN(
    VOS_UINT8                           ucRejectCause
);

NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_ENUM_UINT8 NAS_MML_GetChangeRegRejCauFlg(VOS_VOID);

VOS_UINT8 NAS_MML_GetPreferredRegRejCause_HPLMN_EHPLMN(VOS_VOID);

VOS_UINT8 NAS_MML_GetPreferredRegRejCause_NOT_HPLMN_EHPLMN(VOS_VOID);
VOS_VOID NAS_MML_InitHplmnAuthRejCounter(
    NAS_MML_IGNORE_AUTH_REJ_INFO_STRU                      *pstAuthRejInfo
);

NAS_MML_IGNORE_AUTH_REJ_INFO_STRU * NAS_MML_GetAuthRejInfo(
    VOS_VOID
);

VOS_VOID NAS_MML_SetIgnoreAuthRejFlg(
    VOS_UINT8                           ucIgnoreAuthRejFlg
);

VOS_VOID NAS_MML_SetMaxAuthRejNo(
    VOS_UINT8                           ucMaxAuthRejNo
);


NAS_MML_HPLMN_REJ_CAUSE_CHANGED_COUNTER_STRU* NAS_MML_GetHplmnRejCauseChangedCounter(VOS_VOID);

VOS_VOID  NAS_MML_InitHplmnRejCauseChangedCounter(
    NAS_MML_HPLMN_REJ_CAUSE_CHANGED_COUNTER_STRU           *pstChangeCounInfo
);
VOS_VOID NAS_MML_SetMsMode(NAS_MML_MS_MODE_ENUM_UINT8 enMsMode);

NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32 NAS_MML_GetLteCapabilityStatus(VOS_VOID);

MMC_LMM_DISABLE_LTE_REASON_ENUM_UINT32 NAS_MML_GetDisableLteReason( VOS_VOID);

VOS_VOID NAS_MML_SetLteDisabledRauUseLteInfoFlag(VOS_UINT8 ucLteDisabledUseLteInfoFlag);
VOS_UINT8 NAS_MML_GetLteDisabledRauUseLteInfoFlag( VOS_VOID);


VOS_VOID NAS_MML_SetLteCapabilityStatus(
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus
);

VOS_VOID NAS_MML_SetDisableLteReason(
    MMC_LMM_DISABLE_LTE_REASON_ENUM_UINT32                  enDisableLteReason
);

VOS_VOID NAS_MML_SetLteCsServiceCfg(
    NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8                   enLteCsServiceCfg
);

VOS_UINT8 NAS_MML_GetPsAutoAttachFlg(VOS_VOID);



VOS_VOID NAS_MML_SetUserAutoReselActiveFlg(VOS_UINT8 ucActiveFlag);

VOS_UINT8 NAS_MML_GetUserAutoReselActiveFlg(VOS_VOID);


NAS_MML_PLMN_RAT_PRIO_STRU* NAS_MML_GetMsPrioRatList( VOS_VOID );

NAS_MML_3GPP2_RAT_PRIO_STRU* NAS_MML_GetMs3Gpp2PrioRatList( VOS_VOID );

NAS_MML_MS_BAND_INFO_STRU* NAS_MML_GetMsSupportBand( VOS_VOID );

NAS_MML_NETWORK_3GPP_REL_STRU* NAS_MML_GetNetwork3GppRel( VOS_VOID );

NAS_MML_NETWORK_INFO_STRU* NAS_MML_GetNetworkInfo( VOS_VOID );

NAS_MML_PLMN_LOCK_CFG_INFO_STRU* NAS_MML_GetPlmnLockCfg( VOS_VOID );

NAS_MML_DISABLED_RAT_PLMN_CFG_INFO_STRU* NAS_MML_GetDisabledRatPlmnCfg( VOS_VOID );


NAS_MML_RAT_FORBIDDEN_LIST_STRU* NAS_MML_GetRatForbiddenListCfg( VOS_VOID );

NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 NAS_MML_GetGsmForbiddenStatusFlg( VOS_VOID );

NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 NAS_MML_GetUtranForbiddenStatusFlg( VOS_VOID );

NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 NAS_MML_GetLteForbiddenStatusFlg( VOS_VOID );

VOS_VOID NAS_MML_SetGsmForbiddenStatusFlg( NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 enGsmCapaStatus );

VOS_VOID NAS_MML_SetUtranForbiddenStatusFlg( NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 enUtranCapaStatus );

VOS_VOID NAS_MML_SetLteForbiddenStatusFlg( NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 enLteCapaStatus );

VOS_UINT8 NAS_MML_GetImsiInForbiddenListFlg( VOS_VOID );

VOS_VOID NAS_MML_SetImsiInForbiddenListFlg( 
    VOS_UINT8 ucIsImsiInForbiddenList 
);

NAS_MML_PS_DOMAIN_INFO_STRU* NAS_MML_GetPsDomainInfo( VOS_VOID );

VOS_VOID NAS_MML_SetPsRegStatus(
    NAS_MML_REG_STATUS_ENUM_UINT8       enPsRegStatus
);

VOS_VOID NAS_MML_SetCsRegStatus(
    NAS_MML_REG_STATUS_ENUM_UINT8       enCsRegStatus
);

NAS_MML_REG_STATUS_ENUM_UINT8  NAS_MML_GetPsRegStatus( VOS_VOID );

NAS_MML_REG_STATUS_ENUM_UINT8  NAS_MML_GetCsRegStatus( VOS_VOID );


NAS_MML_REG_STATUS_ENUM_UINT8  NAS_MML_GetEpsRegStatus( VOS_VOID );

VOS_VOID NAS_MML_SetEpsRegStatus(
    NAS_MML_REG_STATUS_ENUM_UINT8       enEpsRegStatus
);



NAS_MML_ROAM_CFG_INFO_STRU* NAS_MML_GetRoamCfg( VOS_VOID );

NAS_MML_AIS_ROAMING_CFG_STRU* NAS_MML_GetAisRoamingCfg( VOS_VOID );



NAS_MML_PRIO_HPLMNACT_CFG_STRU* NAS_MML_GetPrioHplmnActCfg( VOS_VOID );


VOS_VOID NAS_MML_SetRelCauseCsfbHighPrioFlg(
    VOS_UINT8       ucFlg
);

VOS_UINT32 NAS_MML_GetRelCauseCsfbHighPrioFlg( VOS_VOID );

NAS_MML_RPLMN_CFG_INFO_STRU* NAS_MML_GetRplmnCfg( VOS_VOID );

NAS_MML_NET_RAT_TYPE_ENUM_UINT8 NAS_MML_GetLastRplmnRat( VOS_VOID );

VOS_VOID NAS_MML_SetLastRplmnRat(NAS_MML_NET_RAT_TYPE_ENUM_UINT8 enRat);

NAS_MML_SIM_CS_SECURITY_INFO_STRU* NAS_MML_GetSimCsSecurity(VOS_VOID);

NAS_MML_SIM_EHPLMN_INFO_STRU* NAS_MML_GetSimEhplmnList(VOS_VOID);

NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU* NAS_MML_GetSimHplmnWithRatList(VOS_VOID);

NAS_MML_SIM_INFO_STRU*  NAS_MML_GetSimInfo( VOS_VOID );

NAS_MML_SIM_MS_IDENTITY_STRU* NAS_MML_GetSimMsIdentity(VOS_VOID);

VOS_UINT8* NAS_MML_GetSimImsi(VOS_VOID);
VOS_UINT8* NAS_MML_GetUeIdPtmsi(VOS_VOID);
VOS_VOID NAS_MML_SetUeIdPtmsi(
    VOS_UINT8                          *pucPtmsi
);
VOS_UINT8* NAS_MML_GetUeIdPtmsiSignature(VOS_VOID);
VOS_VOID NAS_MML_SetUeIdPtmsiSignature(
    VOS_UINT8                          *pucPtmsiSignature
);
VOS_UINT8* NAS_MML_GetUeIdTmsi(VOS_VOID);
VOS_VOID NAS_MML_SetUeIdTmsi(
    VOS_UINT8                          *pucTmsi
);

VOS_VOID NAS_MML_SetUeOperMode(
    NAS_MML_SIM_UE_OPER_MODE_ENUM_UINT8 enUeOperMode
);

NAS_MML_SIM_UE_OPER_MODE_ENUM_UINT8 NAS_MML_GetUeOperMode(VOS_VOID);

NAS_MML_SIM_OPERPLMN_INFO_STRU* NAS_MML_GetSimOperPlmnList(VOS_VOID);

NAS_MML_SIM_PS_SECURITY_INFO_STRU* NAS_MML_GetSimPsSecurity(VOS_VOID);
VOS_VOID NAS_MML_SetSimPsSecurity(
    NAS_MML_SIM_PS_SECURITY_INFO_STRU   *pstSimPsSecruity

);


NAS_MML_SIM_SELPLMN_INFO_STRU* NAS_MML_GetSimSelPlmnList(VOS_VOID);

NAS_MML_SIM_STATUS_STRU* NAS_MML_GetSimStatus(VOS_VOID);

NAS_MML_SIM_USERPLMN_INFO_STRU* NAS_MML_GetSimUserPlmnList(VOS_VOID);

VOS_UINT8 NAS_MML_GetStkSteeringOfRoamingSupportFlg( VOS_VOID );

VOS_VOID  NAS_MML_InitBgSearchCfgInfo(
    NAS_MML_BG_SEARCH_CFG_INFO_STRU     *pstBgSearchCfg
);
VOS_VOID NAS_MML_InitRssiValue(
    NAS_MML_CAMP_CELL_INFO_STRU        *pstCampCellInfo
);
VOS_VOID NAS_MML_InitCampRai(
    NAS_MML_CAMP_PLMN_INFO_STRU         *pstCampPlmnInfo
);

VOS_VOID  NAS_MML_InitCampPlmnInfo(
    NAS_MML_CAMP_PLMN_INFO_STRU         *pstCampPlmnInfo
);
VOS_VOID  NAS_MML_InitConnStatusInfo(
    NAS_MML_CONN_STATUS_INFO_STRU       *pstConnStatus
);
VOS_VOID  NAS_MML_InitCsDomainInfo(
    NAS_MML_CS_DOMAIN_INFO_STRU         *pstCsDomainInfo
);
VOS_VOID  NAS_MML_InitCtx(
    NAS_MML_INIT_CTX_TYPE_ENUM_UINT8     enInitType,
    NAS_MML_CTX_STRU                    *pstMmlCtx
);
VOS_VOID  NAS_MML_InitCustomCfgInfo(
    NAS_MML_CUSTOM_CFG_INFO_STRU        *pstCustomCfg
);
VOS_VOID  NAS_MML_InitEquPlmnInfo(
    NAS_MML_EQUPLMN_INFO_STRU          *pstEquPlmnInfo
);
VOS_VOID  NAS_MML_InitHplmnCfgInfo(
    NAS_MML_HPLMN_CFG_INFO_STRU         *pstHplmnCfg
);
VOS_VOID  NAS_MML_InitInternalMsgQueue(
    NAS_MML_INTERNAL_MSG_QUEUE_STRU     *pstInternalMsgQueue                   /* MM子层的内部消息队列 */
);
VOS_VOID  NAS_MML_InitMiscellaneousCfgInfo(
    NAS_MML_MISCELLANEOUS_CFG_INFO_STRU *pstMiscellaneousCfg
);
VOS_VOID  NAS_MML_InitMs3GppRel(
    NAS_MML_MS_3GPP_REL_STRU            *pstMs3GppRel
);
VOS_VOID  NAS_MML_InitMsCapability(
    NAS_MML_MS_CAPACILITY_INFO_STRU    *pstMsCapability
);
VOS_VOID  NAS_MML_InitMsCfgCtx(
    NAS_MML_MS_CFG_INFO_STRU           *pstMsCfgInfo
);
VOS_VOID  NAS_MML_InitMsCurrBandInfoInfo(
    NAS_MML_MS_BAND_INFO_STRU         *pstMsBandInfo
);
VOS_VOID  NAS_MML_InitMsSysCfgInfo(
    NAS_MML_MS_SYS_CFG_INFO_STRU        *pstMsSysCfgInfo
);
VOS_VOID  NAS_MML_InitNetwork3GppRel(
    NAS_MML_NETWORK_3GPP_REL_STRU            *pstNetwork3GppRel
);
VOS_VOID  NAS_MML_InitNetworkCtx(
    NAS_MML_NETWORK_INFO_STRU           *pstNetworkInfo
);
VOS_VOID  NAS_MML_InitPlmnLockCfgInfo(
    NAS_MML_PLMN_LOCK_CFG_INFO_STRU     *pstPlmnLockCfg
);

VOS_VOID  NAS_MML_InitRatForbiddenList(
    NAS_MML_RAT_FORBIDDEN_LIST_STRU    *pstRatBlackList
);

VOS_VOID  NAS_MML_InitRatForbiddenStatusCfg(
    NAS_MML_RAT_FORBIDDEN_STATUS_STRU  *pstRatCapaStatus
);

VOS_VOID  NAS_MML_InitPsDomainInfo(
    NAS_MML_PS_DOMAIN_INFO_STRU         *pstPsDomainInfo
);

VOS_VOID  NAS_MML_InitDisabledRatPlmnCfgInfo(
    NAS_MML_DISABLED_RAT_PLMN_CFG_INFO_STRU                *pstDisabledRatPlmnCfg
);

VOS_VOID NAS_MML_InitEpsDomainInfo(
    NAS_MML_EPS_DOMAIN_INFO_STRU        *pstEpsDomainInfo
);
VOS_VOID  NAS_MML_InitRoamCfgInfo(
    NAS_MML_ROAM_CFG_INFO_STRU          *pstRoamCfg
);
VOS_VOID  NAS_MML_InitRplmnCfgInfo(
    NAS_MML_RPLMN_CFG_INFO_STRU         *pstRplmnCfg
);
VOS_VOID  NAS_MML_InitSimCsSecurityCtx(
    NAS_MML_SIM_CS_SECURITY_INFO_STRU   *pstCsSecurity
);
VOS_VOID  NAS_MML_InitCsSecurityUmtsCkInvalid(VOS_VOID);
VOS_VOID  NAS_MML_InitCsSecurityUmtsIkInvalid(VOS_VOID);

VOS_VOID  NAS_MML_InitSimEhPlmnInfo(
    NAS_MML_SIM_EHPLMN_INFO_STRU        *pstEhPlmnInfo
);
VOS_VOID  NAS_MML_InitSimForbidPlmnInfo(
    NAS_MML_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU   *pstForbidPlmnInfo
);

VOS_VOID  NAS_MML_InitSimHPlmnWithRatInfo(
    NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU *pstHplmnWithRatInfo
);
VOS_VOID  NAS_MML_InitSimInfoCtx(
    NAS_MML_INIT_CTX_TYPE_ENUM_UINT8     enInitType,
    NAS_MML_SIM_INFO_STRU              *pstSimInfo
);
VOS_VOID  NAS_MML_InitSimMsIdentityInfoCtx(
    NAS_MML_SIM_MS_IDENTITY_STRU       *pstMsIdentity
);
VOS_VOID  NAS_MML_InitUeIdPtmisInvalid(VOS_VOID);
VOS_VOID  NAS_MML_InitUeIdPtmsiSignatureInvalid(VOS_VOID);
VOS_VOID  NAS_MML_InitUeIdTmsiInvalid(VOS_VOID);

VOS_VOID  NAS_MML_InitSimOperPlmnInfo(
    NAS_MML_SIM_OPERPLMN_INFO_STRU      *pstOperPlmnInfo
);
VOS_VOID  NAS_MML_InitSimPlmnInfoCtx(
    NAS_MML_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    NAS_MML_SIM_PLMN_INFO_STRU         *pstSimStatusInfo
);
VOS_VOID  NAS_MML_InitSimPsSecurityCtx(
    NAS_MML_SIM_PS_SECURITY_INFO_STRU   *pstPsSecurity
);
VOS_VOID  NAS_MML_InitPsSecurityGsmKcInvalid(VOS_VOID);
VOS_VOID  NAS_MML_InitPsSecurityUmtsCkInvalid(VOS_VOID);
VOS_VOID  NAS_MML_InitPsSecurityUmtsIkInvalid(VOS_VOID);
VOS_VOID  NAS_MML_InitCsSecurityGsmKcInvalid(VOS_VOID);

VOS_VOID  NAS_MML_InitSimSelPlmnInfo(
    NAS_MML_SIM_SELPLMN_INFO_STRU        *pstSelPlmnInfo
);
VOS_VOID  NAS_MML_InitSimStatusInfoCtx(
    NAS_MML_SIM_STATUS_STRU             *pstSimStatusInfo
);
VOS_VOID  NAS_MML_InitSimUserPlmnInfo(
    NAS_MML_SIM_USERPLMN_INFO_STRU      *pstUserPlmnInfo
);
VOS_VOID NAS_MML_SetMs3GppRel(
    NAS_MML_MS_3GPP_REL_STRU            *pstMs3GppRel
);
VOS_VOID NAS_MML_SetSimCsSecurity(
    NAS_MML_SIM_CS_SECURITY_INFO_STRU   *pstSimCsSecruity
);

VOS_UINT8 NAS_MML_GetSimCsSecurityCksn(VOS_VOID);
VOS_VOID NAS_MML_SetSimCsSecurityCksn(
    VOS_UINT8                           ucCksn
);
VOS_UINT8* NAS_MML_GetSimCsSecurityUmtsCk(VOS_VOID);
VOS_VOID NAS_MML_SetSimCsSecurityUmtsCk(
    VOS_UINT8                           *pucUmtsCk
);
VOS_UINT8* NAS_MML_GetSimCsSecurityUmtsIk(VOS_VOID);
VOS_VOID NAS_MML_SetSimCsSecurityUmtsIk(
    VOS_UINT8                           *pucUmtsIk
);
VOS_UINT8* NAS_MML_GetSimCsSecurityGsmKc(VOS_VOID);
VOS_VOID NAS_MML_SetSimCsSecurityGsmKc(
    VOS_UINT8                           *pucGsmKc
);



VOS_VOID NAS_MML_SetSimEhplmnList(
    NAS_MML_SIM_EHPLMN_INFO_STRU        *pstEhplmnList
);
VOS_VOID NAS_MML_SetSimForbidPlmnList(
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU    *pstForbidPlmnInfo
);

VOS_VOID NAS_MML_SetSimHplmnTimerLen(
    VOS_UINT32                          ulSimHplmnTimerLen
);

VOS_UINT32 NAS_MML_GetSimHplmnTimerLen(VOS_VOID);

VOS_VOID NAS_MML_SetSimHplmnWithRatList(
    NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU    *pstHplmnWithRatList
);
VOS_VOID NAS_MML_SetSimMsIdentity(
    NAS_MML_SIM_MS_IDENTITY_STRU        *pstSimMsIdentity

);
VOS_VOID NAS_MML_SetSimOperPlmnList(
    NAS_MML_SIM_OPERPLMN_INFO_STRU*     pstOperPlmnList
);

VOS_UINT8 NAS_MML_GetSimPsSecurityCksn(VOS_VOID);
VOS_VOID NAS_MML_SetSimPsSecurityCksn(
    VOS_UINT8                           ucCksn
);
VOS_UINT8* NAS_MML_GetSimPsSecurityUmtsCk(VOS_VOID);
VOS_VOID NAS_MML_SetSimPsSecurityUmtsCk(
    VOS_UINT8                           *pucUmtsCk
);
VOS_UINT8* NAS_MML_GetSimPsSecurityUmtsIk(VOS_VOID);
VOS_VOID NAS_MML_SetSimPsSecurityUmtsIk(
    VOS_UINT8                           *pucUmtsIk
);
VOS_UINT8* NAS_MML_GetSimPsSecurityGsmKc(VOS_VOID);
VOS_VOID NAS_MML_SetSimPsSecurityGsmKc(
    VOS_UINT8                           *pucGsmKc
);

VOS_VOID NAS_MML_SetSimSelPlmnList(
    NAS_MML_SIM_SELPLMN_INFO_STRU       *pstSelPlmnList
);

VOS_VOID NAS_MML_SetSimPresentStatus(
    VOS_UINT8                           ucSimPresentStatus
);

VOS_VOID NAS_MML_SetSimCsRegStatus(
    VOS_UINT8                           ucSimCsRegStatus
);

VOS_VOID NAS_MML_SetSimPsRegStatus(
    VOS_UINT8                           ucSimPsRegStatus
);

VOS_VOID NAS_MML_SetPsUpdateStatus(
    NAS_MML_ROUTING_UPDATE_STATUS_ENUM_UINT8    enPsUpdateStatus
);
NAS_MML_ROUTING_UPDATE_STATUS_ENUM_UINT8 NAS_MML_GetPsUpdateStatus(VOS_VOID);

VOS_VOID NAS_MML_SetCsUpdateStatus(
    NAS_MML_LOCATION_UPDATE_STATUS_ENUM_UINT8   enCsUpdateStatus
);
NAS_MML_LOCATION_UPDATE_STATUS_ENUM_UINT8 NAS_MML_GetCsUpdateStatus(VOS_VOID);

VOS_VOID NAS_MML_SetSimUserPlmnList(
    NAS_MML_SIM_USERPLMN_INFO_STRU      *pstUserPlmnList
);

VOS_UINT8 NAS_MML_GetSimPsRegStatus(VOS_VOID);


VOS_UINT8 NAS_MML_GetSimCsRegStatus(VOS_VOID);


NAS_MML_SIM_TYPE_ENUM_UINT8 NAS_MML_GetSimType(VOS_VOID);
VOS_VOID NAS_MML_SetSimType(
    NAS_MML_SIM_TYPE_ENUM_UINT8                 enSimType
);

VOS_UINT8 NAS_MML_GetSimPresentStatus(VOS_VOID);

VOS_UINT8 NAS_MML_GetEHplmnSupportFlg( VOS_VOID );


VOS_UINT8 NAS_MML_GetCsAttachAllowFlg( VOS_VOID );

VOS_UINT8 NAS_MML_GetPsAttachAllowFlg( VOS_VOID );

VOS_VOID NAS_MML_SetCsAttachAllowFlg(
    VOS_UINT8                           ucCsAttachAllowFlg
);

VOS_VOID NAS_MML_SetPsAttachAllowFlg(
    VOS_UINT8                           ucPsAttachAllowFlg
);

VOS_UINT8 *NAS_MML_GetOperatorNameShort(VOS_VOID);

VOS_UINT8 *NAS_MML_GetOperatorNameLong(VOS_VOID);

NAS_MML_OPERATOR_NAME_INFO_STRU* NAS_MML_GetOperatorNameInfo(VOS_VOID);

NAS_MML_AVAIL_TIMER_CFG_STRU* NAS_MML_GetAvailTimerCfg( VOS_VOID );

VOS_VOID  NAS_MML_InitAvailTimerCfgInfo(NAS_MML_AVAIL_TIMER_CFG_STRU  *pstAvailTimerCfg);

VOS_VOID  NAS_MML_InitUserCfgEHplmnInfo(
    NAS_MML_USER_CFG_EHPLMN_INFO_STRU  *pstUserCfgEhplmnInfo
);

VOS_VOID NAS_MML_SetStkSteeringOfRoamingSupportFlg(
    VOS_UINT8                           ucStkSteeringOfRoamingSupportFlg
);

VOS_VOID NAS_MML_SetPsAutoAttachFlg(
    VOS_UINT8                           usPsAutoAttachFlg
);
NAS_MML_MISCELLANEOUS_CFG_INFO_STRU* NAS_MML_GetMiscellaneousCfgInfo( VOS_VOID );

VOS_UINT8 NAS_MML_GetPsRestrictRegisterFlg(VOS_VOID);
VOS_UINT8 NAS_MML_GetPsRestrictPagingFlg(VOS_VOID);
VOS_UINT8 NAS_MML_GetCsRestrictRegisterFlg(VOS_VOID);
VOS_UINT8 NAS_MML_GetCsRestrictPagingFlg(VOS_VOID);
VOS_VOID NAS_MML_SetCsRestrictRegisterFlg(
    VOS_UINT8                           ucCsRestrictionRegisterFlg
);
VOS_VOID NAS_MML_SetCsRestrictPagingFlg(
    VOS_UINT8                           ucCsRestrictionRegisterFlg
);
NAS_MML_ACCESS_RESTRICTION_STRU* NAS_MML_GetCsAcRestrictionInfo(VOS_VOID);
NAS_MML_ACCESS_RESTRICTION_STRU* NAS_MML_GetPsAcRestrictionInfo(VOS_VOID);

VOS_UINT8 NAS_MML_GetCsRestrictNormalServiceFlg(VOS_VOID);
VOS_UINT8 NAS_MML_GetCsRestrictEmergencyServiceFlg(VOS_VOID);

VOS_VOID NAS_MML_SetCsRestrictNormalServiceFlg(
    VOS_UINT8                           ucCsRestrictiNormalServiceFlg
);

VOS_VOID NAS_MML_SetCsRestrictEmergencyServiceFlg(
    VOS_UINT8                           ucCsRestrictiEmergencyServiceFlg
);

VOS_UINT8 NAS_MML_GetPsRestrictNormalServiceFlg(VOS_VOID);
VOS_UINT8 NAS_MML_GetPsRestrictEmergencyServiceFlg(VOS_VOID);


VOS_VOID NAS_MML_SetPsRestrictNormalServiceFlg(
    VOS_UINT8                           ucPsRestrictNormalServiceFlg
);


VOS_VOID NAS_MML_SetPsRestrictEmergencyServiceFlg(
    VOS_UINT8                           ucPsRestrictiEmergencyServiceFlg
);

VOS_VOID NAS_MML_SetPsRestrictRegisterFlg(
    VOS_UINT8                           ucPsRestrictionRegisterFlg
);
VOS_VOID NAS_MML_SetPsRestrictPagingFlg(
    VOS_UINT8                           ucPsRestrictionRegisterFlg
);

VOS_VOID NAS_MML_ClearCsPsRestrictionAll(VOS_VOID);


VOS_UINT32 NAS_MML_GetPsRegisterBarToUnBarFlg(VOS_VOID);

VOS_VOID NAS_MML_SetPsRegisterBarToUnBarFlg(
    VOS_UINT32                           ulPsUnBarFlg
);

VOS_UINT32 NAS_MML_GetCsRegisterBarToUnBarFlg(VOS_VOID);

VOS_VOID NAS_MML_SetCsRegisterBarToUnBarFlg(
    VOS_UINT32                           ulCsUnBarFlag
);


NAS_MML_TIN_TYPE_ENUM_UINT8 NAS_MML_GetTinType( VOS_VOID );

NAS_MML_UE_NETWORK_CAPABILITY_STRU* NAS_MML_GetUeNetWorkCapability(VOS_VOID);


VOS_VOID NAS_MML_SetUeNetWorkCapability(
NAS_MML_UE_NETWORK_CAPABILITY_STRU *pstUeNetworkCapbility);

NAS_MML_PLATFORM_RAT_CAP_STRU* NAS_MML_GetPlatformRatCap(VOS_VOID);
VOS_VOID NAS_MML_SetRoamCapability(
    NAS_MML_ROAM_CAPABILITY_ENUM_UINT8  enRoamCapability
);


VOS_VOID NAS_MML_SetMsSysCfgBand(
    NAS_MML_MS_BAND_INFO_STRU          *pstMsBand
);


VOS_VOID NAS_MML_SetMsSysCfgPrioRatList(
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstPrioRatList
);

VOS_VOID NAS_MML_SaveEquPlmnList (
    VOS_UINT32                          ulPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pstNewEquPlmnList
);
VOS_VOID NAS_MML_UpdateGURplmn (
    NAS_MML_PLMN_ID_STRU               *pstRPlmnId,
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enCurrRat
);

VOS_VOID NAS_MML_SetEplmnValidFlg(
    VOS_UINT8                           ucEplmnValidFlg
);

VOS_UINT8 NAS_MML_GetEplmnValidFlg(VOS_VOID);

VOS_UINT8* NAS_MML_GetSimCsLociFileContent( VOS_VOID );

VOS_UINT8* NAS_MML_GetSimPsLociFileContent( VOS_VOID );

VOS_VOID  NAS_MML_UpdateSimCsLociFileContent(
    VOS_UINT8                          *pucCsLociFileContent
);

VOS_VOID  NAS_MML_UpdateSimPsLociFileContent(
    VOS_UINT8                          *pucPsLociFileContent
);




VOS_VOID  NAS_MML_InitMaintainInfo(
    NAS_MML_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    NAS_MML_MAINTAIN_CTX_STRU          *pstMaintainInfo
);

VOS_UINT8 NAS_MML_GetUePcRecurFlg(VOS_VOID);

VOS_VOID NAS_MML_SetUePcRecurFlg(
    VOS_UINT8                           ucUePcRecurFlg
);

VOS_UINT8 NAS_MML_GetOmConnectFlg(VOS_VOID);

VOS_VOID NAS_MML_SetOmConnectFlg(
    VOS_UINT8                           ucOmConnectFlg
);

VOS_UINT8 NAS_MML_GetOmPcRecurEnableFlg(VOS_VOID);

VOS_VOID NAS_MML_SetOmPcRecurEnableFlg(
    VOS_UINT8                           ucOmPcRecurEnableFlg
);

NAS_MML_CSFB_SERVICE_STATUS_ENUM_UINT8 NAS_MML_GetCsfbServiceStatus( VOS_VOID );
NAS_MML_VOICE_DOMAIN_PREFERENCE_ENUM_UINT8 NAS_MML_GetVoiceDomainPreference( VOS_VOID );
NAS_MML_LTE_UE_USAGE_SETTING_ENUM_UINT8 NAS_MML_GetLteUeUsageSetting( VOS_VOID );

NAS_MML_TIMER_INFO_ENUM_UINT8 NAS_MML_GetT3412Status(VOS_VOID);
NAS_MML_TIMER_INFO_ENUM_UINT8 NAS_MML_GetT3423Status(VOS_VOID);
VOS_VOID NAS_MML_SetCsfbServiceStatus(NAS_MML_CSFB_SERVICE_STATUS_ENUM_UINT8 enCsfbServiceStatus);
VOS_VOID NAS_MML_SetLteUeUsageSetting(
    NAS_MML_LTE_UE_USAGE_SETTING_ENUM_UINT8                 enUeUsageSetting
);
VOS_VOID NAS_MML_SetVoiceDomainPreference(
    NAS_MML_VOICE_DOMAIN_PREFERENCE_ENUM_UINT8              enVoiceDomainPrefer
);

VOS_VOID NAS_MML_SetT3412Status(NAS_MML_TIMER_INFO_ENUM_UINT8 enTimerStatus);

VOS_VOID NAS_MML_SetT3423Status(NAS_MML_TIMER_INFO_ENUM_UINT8 enTimerStatus);
VOS_VOID NAS_MML_InitEpsDomainInfo(
    NAS_MML_EPS_DOMAIN_INFO_STRU       *pstEpsDomainInfo
);

VOS_VOID NAS_MML_InitImsDomainInfo(
    NAS_MML_IMS_DOMAIN_INFO_STRU       *pstImsDomainInfo
);
NAS_MML_ADDITION_UPDATE_RSLT_INFO_ENUM_UINT8 NAS_MML_GetAdditionUpdateRslt(VOS_VOID);
VOS_VOID NAS_MML_SetAdditionUpdateRslt(
    NAS_MML_ADDITION_UPDATE_RSLT_INFO_ENUM_UINT8            enAdditionUpdRslt
);

VOS_VOID NAS_MML_InitPsBearerContext(
    NAS_MML_PS_BEARER_CONTEXT_STRU     *pstPsBearerCtx
);

NAS_MML_PS_BEARER_CONTEXT_STRU* NAS_MML_GetPsBearerCtx( VOS_VOID );

VOS_VOID NAS_MML_InitEmergencyNumList(
    NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstEmergencyNumList
);

VOS_VOID NAS_MML_SetEmergencyNumList (
    NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstEmcNumList
);

NAS_MML_EMERGENCY_NUM_LIST_STRU* NAS_MML_GetEmergencyNumList (VOS_VOID);

VOS_VOID NAS_MML_SetTinType(
    NAS_MML_TIN_TYPE_ENUM_UINT8         enTinType
);

VOS_VOID NAS_MML_SetPsBearerCtx(
    VOS_UINT8                           ucRabId,
    NAS_MML_PS_BEARER_CONTEXT_STRU     *pstPsBearerCtx
);


VOS_VOID NAS_MML_SetHoWaitSysinfoTimerLen(
    VOS_UINT32                          ulHoWaitSysinfoTimeLen
);

VOS_UINT32 NAS_MML_GetHoWaitSysinfoTimerLen(VOS_VOID);

VOS_VOID NAS_MML_SetCsfbEmgCallLaiChgLauFirstFlg(
    VOS_UINT8                           ucCsfbEmgCallLaiChgLauFirstFlg
);

VOS_UINT8 NAS_MML_GetCsfbEmgCallLaiChgLauFirstFlg(VOS_VOID);


VOS_UINT8 NAS_MML_GetScanCtrlEnableFlg(VOS_VOID);
VOS_VOID NAS_MML_SetScanCtrlEnableFlg(VOS_UINT8 ucScanCtrlEnableFlg);

VOS_VOID NAS_MML_SetHPlmnType(
    NAS_MML_HPLMN_TYPE_ENUM_UINT8       enHPlmnType
);
NAS_MML_HPLMN_TYPE_ENUM_UINT8 NAS_MML_GetHPlmnType(VOS_VOID);
VOS_UINT8  NAS_MML_GetSingleDomainRegFailActionSupportFlag(VOS_VOID);
NAS_MML_SINGLE_DOMAIN_FAIL_ACTION_LIST_CTX_STRU*  NAS_MML_GetSingleDomainFailActionCtx(VOS_VOID);

NAS_MML_SOR_ADDITIONAL_LAU_STRU*  NAS_MML_GetSorAdditionalLauCtx(VOS_VOID);

VOS_VOID NAS_MML_InitSorAdditionalLauCtx(VOS_VOID);
VOS_UINT8  NAS_MML_GetRegFailNetWorkFailureCustomFlag(VOS_VOID);
VOS_VOID  NAS_MML_SetRegFailNetWorkFailureCustomFlag(VOS_UINT8 ucRegFailNetWorkFailureCustomFlg);

VOS_UINT8  NAS_MML_GetBgSearchRegardlessMccSupportFlag(VOS_VOID);
NAS_MMC_HPLMN_SEARCH_REGARDLESS_MCC_CTX_STRU*  NAS_MML_GetBgSearchRegardlessMccCtx(VOS_VOID);
VOS_VOID NAS_MML_SetBgSearchRegardlessMccList(VOS_UINT32 *pulMcc, VOS_UINT8 ucMccNum);

VOS_UINT8 NAS_MML_GetActingHplmnSupportFlg( VOS_VOID );
VOS_VOID NAS_MML_SetActingHplmnSupportFlg(VOS_UINT8 ucActingHPlmnSupportFlg);

NAS_MML_PLMN_ID_STRU  NAS_MML_GetImsiHomePlmn(
    VOS_UINT8                          *pucImsi
);


NAS_MML_EHPLMN_PRESENT_IND_ENUM_UINT8  NAS_MML_GetEHplmnPresentInd( VOS_VOID );

VOS_VOID  NAS_MML_SetPsRegContainDrx(
    NAS_MML_PS_REG_CONTAIN_DRX_PARA_ENUM_UINT8 enPsRegContainDrx
);

NAS_MML_PS_REG_CONTAIN_DRX_PARA_ENUM_UINT8 NAS_MML_GetPsRegContainDrx(VOS_VOID);

VOS_VOID  NAS_MML_SetNonDrxTimer(VOS_UINT8  ucPsNonDrxTimerLength);

VOS_VOID NAS_MML_SetSplitPgCycleCode(VOS_UINT8  ucSplitPgCycleCode);

VOS_UINT8 NAS_MML_GetNonDrxTimer(VOS_VOID);

VOS_VOID  NAS_MML_SetUeUtranPsDrxLen(VOS_UINT8  ucWPsDrxLength);

VOS_UINT8 NAS_MML_GetUeUtranPsDrxLen(VOS_VOID);

VOS_VOID  NAS_MML_SetUeEutranPsDrxLen(VOS_UINT8  ucLPsDrxLength);

VOS_UINT8 NAS_MML_GetUeEutranPsDrxLen(VOS_VOID);

VOS_VOID  NAS_MML_SetWSysInfoDrxLen(VOS_UINT8  ucWSysInfoDrxLength);

VOS_UINT8 NAS_MML_GetWSysInfoDrxLen(VOS_VOID);

VOS_VOID  NAS_MML_SetLSysInfoDrxLen(VOS_UINT8  ucLmmSysInfoDrxLength);

VOS_UINT8 NAS_MML_GetLSysInfoDrxLen(VOS_VOID);

VOS_UINT8 NAS_MML_GetSplitPgCycleCode(VOS_VOID);

VOS_UINT8 NAS_MML_GetSplitOnCcch(VOS_VOID);

VOS_VOID NAS_MML_SetSplitOnCcch(VOS_UINT8 ucSplitOnCcch);

VOS_UINT8 NAS_MML_GetPlmnExactlyComparaFlg(VOS_VOID);

VOS_VOID  NAS_MML_SetPlmnExactlyComparaFlg(VOS_UINT8  ucPlmnExactlyComparaFlag);

VOS_VOID  NAS_MML_SetHplmnRegisterCtrlFlg(VOS_UINT8  ucHplmnRegisterFlg);
VOS_UINT8 NAS_MML_GetHplmnRegisterCtrlFlg(VOS_VOID);

VOS_VOID  NAS_MML_SetH3gCtrlFlg(VOS_UINT8  ucH3gFlg);
VOS_UINT8 NAS_MML_GetH3gCtrlFlg(VOS_VOID);

VOS_VOID  NAS_MML_SetUsimMncLen(VOS_UINT8  ucUsimMncLength);

VOS_UINT8 NAS_MML_GetUsimMncLen(VOS_VOID);

VOS_UINT8 NAS_MML_GetCsServiceBufferStatusFlg(VOS_VOID);


NAS_MML_USER_CFG_EHPLMN_INFO_STRU* NAS_MML_GetUserCfgEhplmnInfo( VOS_VOID );

VOS_VOID  NAS_MML_SetSupportCsServiceFLg(VOS_UINT8  ucCsServiceFlg);
VOS_UINT8  NAS_MML_GetSupportCsServiceFLg(VOS_VOID);

VOS_VOID NAS_MML_SetSvlteSupportFlag(
    VOS_UINT8                           ucSvlteSupportFlag
);

VOS_UINT8 NAS_MML_GetSvlteSupportFlag(VOS_VOID);

VOS_VOID NAS_MML_SetCsfbEmgCallEnableLteTimerLen(
    VOS_UINT32                          ulEnalbeLteTimeLen
);

VOS_UINT32 NAS_MML_GetCsfbEmgCallEnableLteTimerLen(VOS_VOID);

VOS_VOID NAS_MML_SetCsPsMode1EnableLteTimerLen(
    VOS_UINT32                          ulEnalbeLteTimeLen
);
VOS_UINT32 NAS_MML_GetCsPsMode1EnableLteTimerLen(VOS_VOID);



VOS_UINT8 NAS_MML_GetIsrSupportFlg(VOS_VOID);

VOS_VOID NAS_MML_SetIsrSupportFlg(
    VOS_UINT8                           ucIsrSupport
);

#if (FEATURE_ON == FEATURE_PTM)
OM_RING_ID NAS_MML_GetErrLogRingBufAddr(VOS_VOID);
VOS_VOID NAS_MML_SetErrLogRingBufAddr(OM_RING_ID pRingBuffer);
VOS_UINT8 NAS_MML_GetFtmCtrlFlag(VOS_VOID);
VOS_VOID NAS_MML_SetFtmCtrlFlag(VOS_UINT8 ucFlag);
VOS_UINT8 NAS_MML_GetErrlogCtrlFlag(VOS_VOID);
VOS_VOID NAS_MML_SetErrlogCtrlFlag(VOS_UINT8 ucFlag);
VOS_UINT16 NAS_MML_GetErrlogAlmLevel(VOS_VOID);
VOS_VOID NAS_MML_SetErrlogAlmLevel(VOS_UINT16 usAlmLevel);
VOS_VOID NAS_MML_InitErrLogMntnInfo(
    NAS_MML_ERRLOG_MNTN_INFO_STRU          *pstErrLogInfo
);
VOS_UINT32 NAS_MML_PutErrLogRingBuf(
    VOS_CHAR                           *pbuffer,
    VOS_UINT32                          ulbytes
);
VOS_UINT32 NAS_MML_GetErrLogRingBufContent(
    VOS_CHAR                           *pbuffer,
    VOS_UINT32                          ulbytes
);
VOS_UINT32 NAS_MML_GetErrLogRingBufUseBytes(VOS_VOID);
VOS_VOID NAS_MML_CleanErrLogRingBuf(VOS_VOID);
VOS_UINT32 NAS_MML_RegFailCauseNeedRecord(NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16 enRegFailCause);
#endif
VOS_UINT8 NAS_MML_GetDelayedCsfbLauFlg(VOS_VOID);

VOS_VOID NAS_MML_SetDelayedCsfbLauFlg(
    VOS_UINT8                           ucDelayedCsfbLauFlg
);

VOS_UINT8 NAS_MML_GetIsRauNeedFollowOnCsfbMtFlg(VOS_VOID);

VOS_VOID NAS_MML_SetIsRauNeedFollowOnCsfbMtFlg(
    VOS_UINT8                           ucIsRauNeedFollowOnCsfbMtFlg
);



VOS_UINT8 NAS_MML_GetIsRauNeedFollowOnCsfbMoFlg(VOS_VOID);

VOS_VOID NAS_MML_SetIsRauNeedFollowOnCsfbMoFlg(
    VOS_UINT8                           ucIsRauNeedFollowOnCsfbMoFlg
);

VOS_VOID NAS_MML_SetIsDelFddBandSwitchOnFlg(
    VOS_UINT8                           ucIsDelFddBandSwitchOnFlg
);

VOS_UINT8 NAS_MML_GetIsDelFddBandSwitchOnFlg(VOS_VOID);


VOS_VOID NAS_MML_SetPsLocalDetachFlag(VOS_UINT8 ucPsLocalDetachFlag );
VOS_UINT8 NAS_MML_GetPsLocalDetachFlag( VOS_VOID );


VOS_UINT8 NAS_MML_GetSupportAccBarPlmnSearchFlg( VOS_VOID );
NAS_MML_USER_CFG_OPLMN_INFO_STRU* NAS_MML_GetUserCfgOPlmnInfo( VOS_VOID );
VOS_VOID NAS_MML_SetSupportAccBarPlmnSearchFlg(
    VOS_UINT8                           ucAccBarPlmnSearchFlg
);
VOS_VOID NAS_MML_SetUserCfgOPlmnInfo(
    NAS_MML_USER_CFG_OPLMN_INFO_STRU       *pstUserCfgOPlmnInfo
);

VOS_VOID  NAS_MML_InitUserCfgOPlmnInfo(
    NAS_MML_USER_CFG_OPLMN_INFO_STRU       *pstUserCfgOPlmnInfo
);



VOS_VOID  NAS_MML_SetRrcLteNcellExistFlg( 
    VOS_UINT8                           ucLteNcellExist
);

VOS_UINT8  NAS_MML_GetRrcLteNcellExistFlg( VOS_VOID );

VOS_UINT8  NAS_MML_GetRrcUtranNcellExistFlg( VOS_VOID );

VOS_VOID  NAS_MML_SetRrcUtranNcellExistFlg( 
    VOS_UINT8                           ucUtranNcellExist
);

VOS_VOID  NAS_MML_SetDsdsRfShareFlg(
    VOS_UINT16                          usDsdsRfShareFlg    
);
VOS_UINT16 NAS_MML_GetDsdsRfShareFlg(VOS_VOID);
VOS_VOID NAS_MML_SetGsmBandCapability(
    VOS_UINT32                          ulBand
);

VOS_UINT32 NAS_MML_GetGsmBandCapability(VOS_VOID);


VOS_VOID    NAS_MML_SetImsNormalRegStatus(
    NAS_MML_IMS_NORMAL_REG_STATUS_ENUM_UINT8    enRegStatus
);

NAS_MML_IMS_NORMAL_REG_STATUS_ENUM_UINT8   NAS_MML_GetImsNormalRegStatus(VOS_VOID);

VOS_VOID    NAS_MML_SetImsVoiceAvailFlg(
    VOS_UINT8       ucAvail
);

VOS_UINT8   NAS_MML_GetImsVoiceAvailFlg(VOS_VOID);

VOS_VOID NAS_MML_SetEmcPdpStatusFlg( 
    VOS_UINT8                           ucEmcPdpStatusFlg
);
VOS_UINT8 NAS_MML_GetEmcPdpStatusFlg( VOS_VOID );



VOS_UINT8 NAS_MML_GetImsVoiceInterSysLauEnableFlg( VOS_VOID );
VOS_VOID NAS_MML_SetImsVoiceInterSysLauEnableFlg( VOS_UINT8 );

VOS_UINT8 NAS_MML_GetImsVoiceMMEnableFlg( VOS_VOID );
VOS_VOID NAS_MML_SetImsVoiceMMEnableFlg( VOS_UINT8 );

NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8 NAS_MML_GetGUNwImsVoiceSupportFlg(VOS_VOID);
VOS_VOID NAS_MML_SetGUNwImsVoiceSupportFlg(
    NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8 enSupport    
);

NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8 NAS_MML_GetLteNwImsVoiceSupportFlg(VOS_VOID);
VOS_VOID NAS_MML_SetLteNwImsVoiceSupportFlg(
    NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8 enSupport
);

VOS_UINT8 NAS_MML_GetPsSupportFLg(VOS_VOID);

VOS_UINT8 NAS_MML_GetLcEnableFlg(VOS_VOID);

VOS_VOID NAS_MML_SetLcEnableFlg(VOS_UINT8 ucLcEnable);
NAS_MML_HIGH_PRIO_RAT_HPLMN_TIMER_CFG_STRU* NAS_MML_GetHighPrioRatHplmnTimerCfg( VOS_VOID );

VOS_VOID  NAS_MML_InitHighPrioRatHplmnTimerCfgInfo(
    NAS_MML_HIGH_PRIO_RAT_HPLMN_TIMER_CFG_STRU             *pstHighHplmnTimerCfg
);

VOS_UINT8  NAS_MML_GetHighPrioRatHplmnTimerActiveFlg( VOS_VOID );


VOS_UINT8 NAS_MML_Get3GPP2UplmnNotPrefFlg( VOS_VOID );
VOS_VOID NAS_MML_Set3GPP2UplmnNotPrefFlg(VOS_UINT8 uc3GPPUplmnNotPrefFlg);

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

#endif /* end of NasMmlCtx.h */
