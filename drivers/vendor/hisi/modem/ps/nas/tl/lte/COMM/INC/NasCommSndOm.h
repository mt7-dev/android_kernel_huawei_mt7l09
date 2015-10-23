

#ifndef __NASCOMMSNDOM_H__
#define __NASCOMMSNDOM_H__


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "vos.h"
#include  "LPsCommon.h"
#include  "NasLmmPubmTimer.h"
#include  "NasEsmPublic.h"
#include  "NasErabmPublic.h"

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  #pragma pack(*)    设置字节对齐方式
*****************************************************************************/
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


/*****************************************************************************
  4 Enum
*****************************************************************************/
/*****************************************************************************
 枚举名    : NAS_LMM_OM_TI_ENUM
 枚举说明  : MM层所有并行状态机的状态保护定时器定义
             BOUNDARY_START，标识该模块定时器在定时器表中的起始下标;
             BOUNDARY_END，标识该模块定时器在定时器表中的结束下标;
             各个模块的定时器定义需要定义在各自响应的区间中.
*****************************************************************************/
enum LNAS_OM_TI_MSG_ENUM
{
    /*****************************************************************
      EMM模块定时器在此定义
    *****************************************************************/

    /*========== 保留定时器 ===================*/
    TI_LNAS_EMM_STATE_NO_TIMER           = PS_MSG_ID_EMM_TO_EMM_OM_BASE,    /* 稳定状态下无定时器 */
    TI_LNAS_EMM_STATE_T3440                      ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_STATE_DEL_FORB_TA_PROID          ,            /* 删除禁止的TA */ /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    /*========== PUB模块定时器 ===================*/
    TI_LNAS_LMM_TIMER_WAIT_USIM_CNF               ,                              /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_LMM_TIMER_WAIT_USIM_READY_START       ,                              /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    /*========== MRRC 模块定时器 ==============*/
    TI_LNAS_EMM_STATE_MRRC_BOUNDARY_START        ,           /* MRRC 定时器开始边界 */
    TI_LNAS_EMM_MRRC_WAIT_RRC_CONN_CNF           ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_MRRC_WAIT_RRC_REL_CNF            ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    /*========== ATTACH 模块定时器 ============*/
    TI_LNAS_EMM_STATE_REG_BOUNDARY_START         ,           /* REG 定时器开始边界 */
    TI_LNAS_EMM_T3410                            ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_T3411                            ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
     /*TI_LNAS_EMM_T3402                          ,*/
    TI_LNAS_EMM_WAIT_ESM_PDN_RSP                 ,           /*等待ESM的PDN响应*//*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_WAIT_ESM_BEARER_CNF              ,           /*等待ESM的承载响应*//*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_WAIT_RRC_DATA_CNF                ,           /*等待RRC直传消息传输确认*//*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    /*========== DETACH 模块定时器 ============*/
    TI_LNAS_EMM_STATE_DEREG_BOUNDARY_START       ,           /* DEREG 定时器开始边界 *//*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_T3421                            ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    /*========== TAU 模块定时器 ===============*/
    TI_LNAS_EMM_STATE_TAU_BOUNDARY_START         ,           /* TAU 定时器开始边界 */
    TI_LNAS_EMM_STATE_TAU_T3430                  ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_STATE_TAU_T3411                  ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    /*========== SERVICE 模块定时器 ===========*/
    TI_LNAS_EMM_STATE_SERVICE_BOUNDARY_START     ,           /* SERVICE 定时器开始边界 */
    TI_LNAS_EMM_STATE_SERVICE_T3417              ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_STATE_SERVICE_T3417_EXT          ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_STATE_SERVICE_T3442              ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    /*========== PLMN 模块定时器 ==============*/
    TI_LNAS_EMM_STATE_PLMN_BOUNDARY_START        ,           /* PLMN 定时器开始边界 */
    TI_LNAS_EMM_WAIT_MMC_START_CNF_TIMER         ,
    TI_LNAS_EMM_WAIT_RRC_START_CNF_TIMER         ,
    TI_LNAS_EMM_WAIT_MMC_STOP_CNF_TIMER          ,
    TI_LNAS_EMM_WAIT_RRC_STOP_CNF_TIMER          ,

    /*========== AUTH 模块定时器 ==============*/
    TI_LNAS_EMM_STATE_AUTH_BOUNDARY_START        ,           /* AUTH 定时器开始边界 */
    TI_LNAS_EMM_T3418                            ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_T3420                            ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    /*========== SUSPEND定时器 ================*/
    TI_LNAS_EMM_RRCORI_WAIT_OTHER_SUSPEND_RSP_TIMER   ,                          /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_WAIT_SUSPEND_END_TIMER           ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_MMCORI_WAIT_OTHER_SUSPEND_RSP_TIMER   ,                          /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_SYSCFGORI_WAIT_OTHER_SUSPEND_RSP_TIMER   ,                       /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/


    /*========== RESEUME定时器 ================*/
    TI_LNAS_EMM_RRCRSM_WAIT_OTHER_RESUME_RSP_TIMER     ,                         /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_WAIT_SYS_INFO_IND_TIMER         ,                                /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_SYSCFGRSM_WAIT_OTHER_RESUME_RSP_TIMER,                           /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    /*========== SYSCFG的定时器 ============================*/
    TI_LNAS_EMM_STATE_WAIT_SYSCFG_CNF_TIMER,                                     /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    TI_LNAS_EMM_STATE_TI_BUTT                    ,

    /*========== MMC的定时器 ============================*/
    /*========== PLMN_SRCH的定时器 ============================*/
    TI_LNAS_EMMC_STATE_WAIT_PLMN_SRCH_CNF_TIMER,                                 /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    TI_LNAS_EMMC_STATE_TI_BUTT                    ,

    /*========== END ====================================*/
    LNAS_LMM_STATE_TI_BUTT,

    /*=================== EMM的定时器 ===================*/

    /*========== ATTACH 模块定时器 ============*/
    /*========== DETACH 模块定时器 ============*/
    /*========== TAU 模块定时器 ===============*/
    TI_LNAS_EMM_PTL_T3412                        ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_PTL_T3402                        ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/
    TI_LNAS_EMM_PTL_T3423                        ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    /*========== AUTH 模块定时器 ===============*/
    TI_LNAS_EMM_PTL_T3416                        ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/

    /*========== 关机定时器 ===================*/
    TI_LNAS_EMM_PTL_SWITCH_OFF_TIMER             ,                               /*_H2ASN_MsgChoice  NAS_EMM_TIMER_INFO_STRU*/


    TI_LNAS_EMM_PTL_TI_BUTT                      ,

    /*========== MMC的定时器 ==================*/
    LNAS_LMM_PTL_TI_BUTT,

    /*****************************************************************
      ESM模块定时器在此定义
    *****************************************************************/

    TI_LNAS_ESM_T3482                           = PS_MSG_ID_ESM_TO_ESM_OM_BASE, /*_H2ASN_MsgChoice  NAS_ESM_TIMER_INFO_STRU*/
    TI_LNAS_ESM_T3492,                                                          /*_H2ASN_MsgChoice  NAS_ESM_TIMER_INFO_STRU*/
    TI_LNAS_ESM_T3480,                                                          /*_H2ASN_MsgChoice  NAS_ESM_TIMER_INFO_STRU*/
    TI_LNAS_ESM_T3481,                                                          /*_H2ASN_MsgChoice  NAS_ESM_TIMER_INFO_STRU*/

    TI_LNAS_ESM_WAIT_APP_CNF,                                /*等待APP回复定时器*//*_H2ASN_MsgChoice  NAS_ESM_TIMER_INFO_STRU*/

    TI_LNAS_ESM_WAIT_PTI_INVALID,                            /*等待PTI失效*/    /*_H2ASN_MsgChoice  NAS_ESM_TIMER_INFO_STRU*/
    TI_LNAS_ESM_ATTACH_BEARER_REEST,                         /* Timer for Attach Bearer Reest*//*_H2ASN_MsgChoice  NAS_ESM_TIMER_INFO_STRU*/
    TI_LNAS_ESM_TIMER_PARA_BUTT,

    /*****************************************************************
      ERABM模块定时器在此定义
    *****************************************************************/

    TI_LNAS_ERABM_WAIT_EPSB_ACT_TIMER             = PS_MSG_ID_ERABM_TO_ERABM_OM_BASE, /*等待EPS承载激活定时器*//*_H2ASN_MsgChoice  NAS_ERABM_TIMER_INFO_STRU*/
    TI_LNAS_ERABM_WAIT_RB_REEST_TIMER,                     /*等待RB建立定时器*/     /*_H2ASN_MsgChoice  NAS_ERABM_TIMER_INFO_STRU*/
    TI_LNAS_ERABM_WAIT_RB_RESUME_TIMER,                    /*等待RB恢复定时器*/     /*_H2ASN_MsgChoice  NAS_ERABM_TIMER_INFO_STRU*/
    TI_LNAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER,               /*环回缓存定时器*/       /*_H2ASN_MsgChoice  NAS_ERABM_TIMER_INFO_STRU*/

    TI_LNAS_NAS_ERABM_TIMER_NAME_BUTT,

    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*EMM与MMC接口的API*/
    LNAS_OM_LOG_PUB_INFO_IND                 = PS_MSG_ID_LNAS_TO_OM_BASE,       /*_H2ASN_MsgChoice NAS_LMM_LOG_PUB_INFO_STRU*/
    LNAS_OM_LOG_GU_INFO_IND,                                                    /*_H2ASN_MsgChoice NAS_LMM_LOG_GU_INFO_STRU*/
    LNAS_OM_LOG_TIN_INFO_IND,                                                   /*_H2ASN_MsgChoice NAS_LMM_LOG_TIN_INFO_STRU*/
    LNAS_OM_LOG_MS_NETWORK_CAPACILITY_INFO_IND,                                 /*_H2ASN_MsgChoice NAS_LMM_LOG_MS_NETWORK_CAPACILITY_INFO_STRU*/
    LNAS_OM_LOG_UEIDTMSI_IND,                                                   /*_H2ASN_MsgChoice NAS_LMM_LOG_UEIDTMSI_INFO_STRU*/
    LNAS_OM_LOG_CS_SERVICE_FLAG_INFO_IND,                                       /*_H2ASN_MsgChoice NAS_LMM_LOG_CS_SERVICE_FLAG_INFO_STRU*/

    LNAS_OM_LOG_ADD_UPDATE_RSLT_INFO_IND,                                       /*_H2ASN_MsgChoice NAS_LMM_LOG_ADD_UPDATE_RSLT_INFO_STRU*/
    LNAS_OM_LOG_PS_BEAR_CTX_INFO_IND,                                           /*_H2ASN_MsgChoice NAS_LMM_LOG_PS_BEAR_CTX_INFO_STRU*/
    LNAS_OM_LOG_PSBER_EXIST_BEF_ISRACT_INFO_IND,                                /*_H2ASN_MsgChoice NAS_LMM_LOG_PSBER_EXIST_BEF_ISRACT_INFO_STRU*/
    LNAS_OM_LOG_BEAR_ISR_ACT_AFTER_INFO_IND,                                    /*_H2ASN_MsgChoice NAS_LMM_LOG_PSBER_EXIST_AFTER_ISRACT_INFO_STRU*/
    LNAS_OM_LOG_EMERGENCY_NUMLIST_INFO_IND,                                     /*_H2ASN_MsgChoice NAS_LMM_LOG_EMERGENCY_NUMLIST_INFO_STRU*/
    LNAS_OM_LOG_PSREG_CONTAINDRX_INFO_IND,                                      /*_H2ASN_MsgChoice NAS_LMM_LOG_PSREG_CONTAINDRX_INFO_STRU*/

    LNAS_OM_LOG_EUTRAN_PS_DRX_LEN_INFO_IND,                                     /*_H2ASN_MsgChoice NAS_LMM_LOG_UE_EUTRAN_PS_DRX_LEN_INFO_STRU*/
    LNAS_OM_LOG_DRX_TIMER_INFO_IND,                                             /*_H2ASN_MsgChoice NAS_LMM_LOG_NON_DRX_TIMER_INFO_STRU*/
    LNAS_OM_LOG_SPLIT_PG_CYCLE_CODE_INFO_IND,                                   /*_H2ASN_MsgChoice NAS_LMM_LOG_DRX_SPLIT_PG_CYCLE_CODE_INFO_STRU*/
    LNAS_OM_LOG_SPLIT_ON_CCCH_INFO_IND,                                         /*_H2ASN_MsgChoice NAS_LMM_LOG_DRX_SPLIT_ON_CCCH_INFO_STRU*/
    LNAS_OM_LOG_CLASSMARK2_INFO_IND,                                            /*_H2ASN_MsgChoice NAS_LMM_LOG_CLASSMARK2_INFO_STRU*/
    LNAS_OM_LOG_CS_ATTACH_ALLOW_FLG_INFO_IND,                                   /*_H2ASN_MsgChoice NAS_LMM_LOG_CS_ATTACH_ALLOW_FLG_INFO_STRU*/
    LNAS_OM_LOG_PS_ATTACH_ALLOW_FLG_INFO_IND,                                   /*_H2ASN_MsgChoice NAS_LMM_LOG_PS_ATTACH_ALLOW_FLG_INFO_STRU*/
    LNAS_OM_LOG_MS_MODE_INFO_IND,                                               /*_H2ASN_MsgChoice NAS_LMM_LOG_MS_MODE_INFO_STRU*/
    LNAS_OM_LOG_PLMN_IS_FORBID_INFO_IND,                                        /*_H2ASN_MsgChoice NAS_LMM_LOG_PLMNID_IS_FOBID_INFO_STRU*/
    LNAS_OM_LOG_PLMN_EXAXTLY_CMP_INFO_IND,                                      /*_H2ASN_MsgChoice NAS_LMM_LOG_PLMN_EXACTLY_COMPARE_INFO_STRU*/
    LNAS_OM_LOG_PS_REG_STATUS_INFO_IND,                                         /*_H2ASN_MsgChoice NAS_LMM_LOG_PS_REG_STATUS_INFO_STRU*/
    LNAS_OM_LOG_SIM_TYPE_INFO_IND,                                              /*_H2ASN_MsgChoice NAS_LMM_LOG_SIM_TYPE_INFO_STRU*/
    LNAS_OM_LOG_BSP_KDF_KEY_MAKE_INFO_IND,                                      /*_H2ASN_MsgChoice NAS_LMM_LOG_KDF_KEY_INFO_STRU*/
    LNAS_OM_LOG_SDF_PARA_INFO_IND,                                              /*_H2ASN_MsgChoice NAS_OM_LOG_SDF_PARA_INFO_STRU*/
    LNAS_OM_LOG_PDP_MANAGE_INFO_IND,                                            /*_H2ASN_MsgChoice NAS_OM_LOG_PDP_MANAGER_INFO_STRU*/

    LNAS_OM_LOG_CARD_IMSI_INFO_IND,                                             /*_H2ASN_MsgChoice NAS_LMM_LOG_CARD_IMSI_INFO_STRU*/
    LNAS_OM_LOG_USIMM_SERVICE_INFO_IND,                                         /*_H2ASN_MsgChoice NAS_LMM_LOG_USIMM_SERVICE_INFO_STRU*/
    LNAS_OM_LOG_USIMM_TEST_MODE_INFO_IND,                                       /*_H2ASN_MsgChoice NAS_LMM_LOG_USIM_TEST_MODE_INFO_STRU*/

    LNAS_OM_LOG_MSG_BUTT
    /*PC REPLAY MODIFY BY LEILI END*/
};
typedef VOS_UINT32 LNAS_OM_TI_MSG_ENUM_UINT32;


/*****************************************************************************
   5 STRUCT
*****************************************************************************/


typedef struct
{
    LNAS_OM_TI_MSG_ENUM_UINT32          enMsgId;            /*_H2ASN_MsgChoice_Export LNAS_OM_TI_MSG_ENUM_UINT32*/
    VOS_UINT8                           aucMsg[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          LNAS_OM_TI_MSG_ENUM_UINT32
    ****************************************************************************/
}LNAS_OM_INTERFACE_MSG_DATA;
/*_H2ASN_Length UINT32*/


typedef struct
{
    VOS_MSG_HEADER
    LNAS_OM_INTERFACE_MSG_DATA           stMsgData;
} NasCommSndOm_MSG;

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




#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of NasCommPrint.h */
