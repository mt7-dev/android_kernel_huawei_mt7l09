

#ifndef __CDS_DEPEND_H__
#define __CDS_DEPEND_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "vos.h"
#include "PsCommonDef.h"
#include "PsLogdef.h"
#include "PsTypeDef.h"
#include "TTFMem.h"
#include "TTFMemInterface.h"
#include "TTFComm.h"
#include "LUPQueue.h"
#include "ApsCdsInterface.h"
#include "ApsCdsInterface.h"
#include "CdsAdsInterface.h"
#include "CdsMmcInterface.h"
#include "CdsNdInterface.h"
#include "CdsRabmInterface.h"
#include "CdsRabmInterface.h"
#include "QosFcCommon.h"
#include "QosFcOm.h"
#include "CdsInterface.h"
#include "DrvInterface.h"
#include "BSP_IPF.h"
#include "product_config.h"
#include "ImsaCdsInterface.h"

#if (FEATURE_ON ==FEATURE_LTE)
#include "OmCommon.h"
#include "CdsLPdcpInterface.h"
#include "CdsErabmInterface.h"
#include "CdsEtcInterface.h"
#endif

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif



/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#define CDS_FEATURE_ON                  (1)
#define CDS_FEATURE_OFF                 (0)

#if (FEATURE_ON == FEATURE_LTE)
#define CDS_FEATURE_LTE                 CDS_FEATURE_ON
#else
#define CDS_FEATURE_LTE                 CDS_FEATURE_OFF
#endif

#if (FEATURE_OFF  == FEATURE_MULTI_MODEM)
#define CDS_FEATURE_MULTI_MODEM         CDS_FEATURE_OFF
#define CDS_MAX_MODEM_NUM               (1)
#else
#define CDS_FEATURE_MULTI_MODEM         CDS_FEATURE_ON
#define CDS_MAX_MODEM_NUM               (2)
#endif


#if (FEATURE_OFF  == FEATURE_SKB_EXP)
#define CDS_FEATURE_IPF_BRIDGE          CDS_FEATURE_ON
#else
#define CDS_FEATURE_IPF_BRIDGE          CDS_FEATURE_OFF
#endif

#if (FEATURE_OFF == FEATURE_IMS)
#define CDS_FEATURE_IMS                 CDS_FEATURE_OFF
#else
#define CDS_FEATURE_IMS                 CDS_FEATURE_ON
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  FOR PC-LINT
*****************************************************************************/
#ifdef _lint
#define WUEPS_PID_MMC       210
#define ACPU_PID_ADS_UL     211
#define ACPU_PID_ADS_DL     212
#define WUEPS_PID_RABM      213
#define PS_PID_RABM         214
#define UEPS_PID_NDCLIENT   216
#define PS_PID_TC           217
#define PS_PID_IMSA         240
#define WUEPS_PID_PDCP      250

#define I0_WUEPS_PID_MMC    210
#define I0_WUEPS_PID_RABM   214
#define I1_WUEPS_PID_MMC    218
#define I1_WUEPS_PID_RABM   219

#define I0_WUEPS_PID_TAF    220
#define I1_WUEPS_PID_TAF    221

#endif



#if (CDS_FEATURE_ON == CDS_FEATURE_LTE)

/*定义消息上报接口*/
extern  VOS_UINT32 LTE_MsgHook( VOS_VOID *pMsg );
#define CDS_MSG_HOOK(pstMsgBlk)    (VOS_VOID)LTE_MsgHook(pstMsgBlk)


/*LOG PRINT定义*/
#define CDS_INFO_LOG(ModuleId, String)                                 LPS_LOG(  ModuleId, 0,LOG_LEVEL_INFO, String )
#define CDS_INFO_LOG1(ModuleId, String,Para1)                          LPS_LOG1( ModuleId, 0,LOG_LEVEL_INFO, String, (VOS_INT32)Para1)
#define CDS_INFO_LOG2(ModuleId, String,Para1,Para2)                    LPS_LOG2( ModuleId, 0,LOG_LEVEL_INFO, String, (VOS_INT32)Para1, (VOS_INT32)Para2 )
#define CDS_INFO_LOG3(ModuleId, String,Para1,Para2,Para3)              LPS_LOG3( ModuleId, 0,LOG_LEVEL_INFO, String, (VOS_INT32)Para1, (VOS_INT32)Para2, (VOS_INT32)Para3 )
#define CDS_INFO_LOG4(ModuleId, String,Para1,Para2,Para3,Para4)        LPS_LOG4( ModuleId, 0,LOG_LEVEL_INFO, String, (VOS_INT32)Para1, (VOS_INT32)Para2, (VOS_INT32)Para3,(VOS_INT32)Para4 )

#define CDS_WARNING_LOG(ModuleId, String)                              LPS_LOG(  ModuleId, 0,LOG_LEVEL_WARNING, String )
#define CDS_WARNING_LOG1(ModuleId, String,Para1)                       LPS_LOG1( ModuleId, 0,LOG_LEVEL_WARNING, String, Para1)
#define CDS_WARNING_LOG2(ModuleId, String,Para1,Para2)                 LPS_LOG2( ModuleId, 0,LOG_LEVEL_WARNING, String, Para1, Para2 )
#define CDS_WARNING_LOG3(ModuleId, String,Para1,Para2,Para3)           LPS_LOG3( ModuleId, 0,LOG_LEVEL_WARNING, String, Para1, Para2, Para3 )
#define CDS_WARNING_LOG4(ModuleId, String,Para1,Para2,Para3,Para4)     LPS_LOG4( ModuleId, 0,LOG_LEVEL_WARNING, String, Para1, Para2, Para3, Para4 )

#define CDS_ERROR_LOG(ModuleId, String)                                LPS_LOG(  ModuleId, 0,LOG_LEVEL_ERROR, String )
#define CDS_ERROR_LOG1(ModuleId, String,Para1)                         LPS_LOG1( ModuleId, 0,LOG_LEVEL_ERROR, String, Para1)
#define CDS_ERROR_LOG2(ModuleId, String,Para1,Para2)                   LPS_LOG2( ModuleId, 0,LOG_LEVEL_ERROR, String, Para1, Para2 )
#define CDS_ERROR_LOG3(ModuleId, String,Para1,Para2,Para3)             LPS_LOG3( ModuleId, 0,LOG_LEVEL_ERROR, String, Para1, Para2, Para3 )
#define CDS_ERROR_LOG4(ModuleId, String,Para1,Para2,Para3,Para4)       LPS_LOG4( ModuleId, 0,LOG_LEVEL_ERROR, String, Para1, Para2, Para3, Para4 )

/*用户面事件上报*/
#define CDS_UP_EVENT_UL_IPF_ERR                                         (TL_UP_EVENT_CDS_UL_IPF_ERR)
#define CDS_UP_EVENT_UL_SOFT_IPF_ERR                                    (TL_UP_EVENT_CDS_UL_SOFT_IPF_ERR)
#define CDS_UP_EVENT_UL_FLOW_CTRL_DISCARD                               (TL_UP_EVENT_CDS_UL_FLOW_CTRL)
#define CDS_UP_EVENT_DL_FLOW_CTRL_DISCARD                               (TL_UP_EVENT_CDS_DL_FLOW_CTRL)
#define CDS_UP_EVENT_DL_SDU_QUE_FULL                                    (TL_UP_EVENT_CDS_DL_SDU_QUE_FULL)

#define CDS_UP_EVENT_RPT(Event)                                         TL_OM_ReportSingleUpEvent(Event, UEPS_PID_CDS, 0xFFFFUL, 0xFFFFUL, 0xFFFFUL)
#else

/*定义消息上报接口*/
extern VOS_UINT32   OM_TraceMsgHook(VOS_VOID *pMsg);
#define CDS_MSG_HOOK(pstMsgBlk)         (VOS_VOID)OM_TraceMsgHook(pstMsgBlk)


/*LOG PRINT定义*/
#define CDS_INFO_LOG(ModuleId, String)
#define CDS_INFO_LOG1(ModuleId, String,Para1)
#define CDS_INFO_LOG2(ModuleId, String,Para1,Para2)
#define CDS_INFO_LOG3(ModuleId, String,Para1,Para2,Para3)
#define CDS_INFO_LOG4(ModuleId, String,Para1,Para2,Para3,Para4)

#define CDS_WARNING_LOG(ModuleId, String)
#define CDS_WARNING_LOG1(ModuleId, String,Para1)
#define CDS_WARNING_LOG2(ModuleId, String,Para1,Para2)
#define CDS_WARNING_LOG3(ModuleId, String,Para1,Para2,Para3)
#define CDS_WARNING_LOG4(ModuleId, String,Para1,Para2,Para3,Para4)

#define CDS_ERROR_LOG(ModuleId, String)                              vos_printf(" %s, %d, %s\r\n ", __FILE__, __LINE__,String)
#define CDS_ERROR_LOG1(ModuleId, String,Para1)                       vos_printf (" %s, %d, %s, %d, \r\n ",  __FILE__, __LINE__, String, Para1)
#define CDS_ERROR_LOG2(ModuleId, String,Para1,Para2)                 vos_printf (" %s, %d, %s, %d, %d \r\n ",  __FILE__, __LINE__, String, Para1, Para2)
#define CDS_ERROR_LOG3(ModuleId, String,Para1,Para2,Para3)           vos_printf (" %s, %d, %s, %d, %d, %d\r\n ",  __FILE__, __LINE__, String, Para1, Para2, Para3)
#define CDS_ERROR_LOG4(ModuleId, String,Para1,Para2,Para3,Para4)     vos_printf (" %s, %d, %s, %d, %d, %d, %d\r\n ",  __FILE__, __LINE__, String, Para1, Para2, Para3, Para4)

/*用户面事件上报*/
#define CDS_UP_EVENT_UL_IPF_ERR
#define CDS_UP_EVENT_UL_SOFT_IPF_ERR
#define CDS_UP_EVENT_UL_FLOW_CTRL_DISCARD
#define CDS_UP_EVENT_DL_FLOW_CTRL_DISCARD
#define CDS_UP_EVENT_DL_SDU_QUE_FULL

#define CDS_UP_EVENT_RPT(Event)

#endif



/*CDS DEBUG*/
#ifdef CDS_DEBUG
#define CDS_ASSERT(exp) \
              if (!(exp))\
              {\
                  vos_printf("%s,%d " #exp "\r\n", __FILE__,__LINE__);\
                  return;\
              }

#define CDS_ASSERT_RTN(exp,ret)\
              if (!(exp))\
              {\
                  vos_printf("%s,%d " #exp "\r\n",,__FILE__,__LINE__);\
                  return ret;\
              }

#else
#define CDS_ASSERT(exp)
#define CDS_ASSERT_RTN(exp,ret)
#endif


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 枚举定义
*****************************************************************************/

/*****************************************************************************
   5 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  6 UNION定义
*****************************************************************************/


/*****************************************************************************
  7 全局变量声明
*****************************************************************************/


/*****************************************************************************
  8 函数声明
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
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

#endif

