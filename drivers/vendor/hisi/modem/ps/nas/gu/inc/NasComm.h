

#ifndef _NAS_COMM_H_
#define _NAS_COMM_H_

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "Nas_Mem.h"
#include "pslog.h"
#include "NasCommDef.h"
#include "TafAppMma.h"

#include "NasOmInterface.h"
#include "NasOmTrans.h"

#if (FEATURE_ON == FEATURE_PTM)
#include "omerrorlog.h"
#include "NasErrorLog.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 常量定义
*****************************************************************************/

/*********************************************************************/
/************** MTF地址宏定义 ****************************************/
/*********************************************************************/
#define              ADDRESS_MTF                            32
#define              ADDRESS_TAF                            100

/*********************************************************************/
/************** PD定义 (27.A02) **************************************/
/*********************************************************************/
#define              PD_ACMP                                1
#define              PD_RSMP                                2
#define              PD_CCP                                 3
#define              PD_PCP                                 4
#define              PD_SSP                                 5
#define              PD_MTMP                                21
#define              PD_DCP                                 22

/*********************************************************************/
/************** PD定义 (24.008) **************************************/
/*********************************************************************/
#define              NAS_PD_GCC                             0x00
#define              NAS_PD_BCC                             0x01
#define              NAS_PD_CC                              0x03
#define              NAS_PD_GTTP                            0x04
#define              NAS_PD_MM                              0x05
#define              NAS_PD_RR                              0x06
#define              NAS_PD_GMM                             0x08
#define              NAS_PD_SMS                             0x09
#define              NAS_PD_SM                              0x0A
#define              NAS_PD_SS                              0x0B
#define              NAS_PD_LS                              0x0C

/* SM发给TAF的消息 */
#define     NAS_SND_TAFMSG_SM2TAF                           0
/* SM接收TAF的原语 */
#define     NAS_RCV_TAFMSG_TAF2SM                           1


/* TAF发给MMC的原语 */
#define     NAS_RCV_TAFMSG_TAF2MMC                          2
/* MMC发给TAF的原语 */
#define     NAS_SND_TAFMSG_MMC2TAF                          3


/* TAF发送给RABM的原语 */
#define     NAS_RCV_TAFMSG_TAF2RABM                         4
/* RABM发送给TAF的原语  */
#define     NAS_SND_TAFMSG_RABM2TAF                         5

/* TAF发给CC的宏定义 */
#define     NAS_RCV_TAFMSG_TAF2CC                           6
/* CC发给TAF的宏定义 */
#define     NAS_SND_TAFMSG_CC2TAF                           7

/* TAF发给SS的宏定义 */
#define     NAS_RCV_TAFMSG_TAF2SS                           8
/* SS发给TAF的宏定义 */
#define     NAS_SND_TAFMSG_SS2TAF                           9


/*********************************************************************/
/************** NAS TIMER ID 定义 ************************************/
/*********************************************************************/
#define NAS_TIMER_ALL                   0xFF

/* SMS TIMER ID */
#define SMC_TIMER_ID_PS_TC1M_MO         0                                             /* TC1M                                     */
#define SMC_TIMER_ID_PS_TC1M_MT         1                                             /* TC1M                                     */
#define SMC_TIMER_ID_CS_TC1M_MO         2                                             /* TC1M                                     */
#define SMC_TIMER_ID_CS_TC1M_MT         3                                             /* TC1M                                     */
#define SMC_TIMER_ID_TR1M               4
#define SMC_TIMER_ID_TR2M               5
#define SMC_TIMER_ID_TRAM               6

/* GMM TIMER ID */
#define     GMM_TIMER_T3302                              0                      /* T3302定时器                              */
#define     GMM_TIMER_T3310                              1                      /* T3310定时器                              */
#define     GMM_TIMER_T3311                              2                      /* T3311定时器                              */
#define     GMM_TIMER_T3312                              3                      /* T3312定时器                              */
#define     GMM_TIMER_T3316                              4                      /* T3316定时器                              */
#define     GMM_TIMER_T3317                              5                      /* T3317定时器                              */
#define     GMM_TIMER_T3318                              6                      /* T3318定时器                              */
#define     GMM_TIMER_T3320                              7                      /* T3320定时器                              */
#define     GMM_TIMER_T3321                              8                      /* T3321定时器                              */
#define     GMM_TIMER_T3330                              9                      /* T3330定时器                              */
#define     GMM_TIMER_1S                                10                      /* 5秒定时器                                */
#define     GMM_TIMER_PROTECT                           11                      /* 保护TIMER(agent)*/
#define     GMM_TIMER_PROTECT_FOR_SIGNALING             12                      /* 保护TIMER(建信令)                        */
#define     GMM_TIMER_SUSPENDED                         13                      /* 保护挂起状态 */
#define     GMM_TIMER_T3314                             14                      /* GPRS READY TIMER定时器 */
#define     GMM_TIMER_RAU_RSP                           15                      /* GMM RAU RSP TIMER定时器 */
#define     GMM_TIMER_PROTECT_FOR_RR_REL                16                      /* 保护TIMER(释放信令连接)                  */
#define     GMM_TIMER_PROTECT_OLD_TLLI                  17
#define     GMM_TIMER_T3319                             18                      /* T3319定时器                              */
#define     GMM_TIMER_T3340                             19                      /* T3340定时器                              */
#define     GMM_TIMER_INTERRAT_HANDOVER_INFO_CNF        20                      /* 等待WRR回复InterRatHoInfoCnf保护定时器   */
#define     GMM_TIMER_WAIT_GET_RESEL_SECU_INFO_CNF      (21)                      /* 等待LMM回复重选安全参数的保护定时器   */
#define     GMM_TIMER_WAIT_GET_HO_SECU_INFO_CNF         (22)                      /* 等待LMM回复切换安全参数的保护定时器   */


#define     GMM_TIMER_WAIT_CONNECT_REL                  (23)                    /* 等待连接释放保护定时器   */

#define     GMM_TIMER_TC_DELAY_SUSPEND_RSP              (24)                    /* 在TC的GCF测试中，GMM延迟向MMC回复SUSPEND_RSP定时器 */

#define     GMM_TIMER_T3323                             (25)                    /* ISR去激活定时器 */

#define     GMM_TIMER_CS_CONN_NOT_EXIST_WAIT_SYSINFO    (26)                    /* GMM收到MM的cs信令连接不存在消息时判断如果W下，GS关联不存在，RRC连接存在网络模式I,cs ps mode需要启动等系统消息定时器，D态WAS不会上报系统消息，定时器超时需要做联合rau*/


#define     GMM_TIMER_DELAY_RADIO_CAPA_TRIGED_RAU       (27)                    /* 在TC的GCF测试中，GMM延迟向MMC回复SUSPEND_RSP定时器 */


#define    GMM_TIMER_WAIT_AS_MS_RADIO_CAPA_INFO         (28)                    /* 发rau或attach前判断当前L能力和g_GmmGlobalCtrl.ucMsRadioCapSupportLteFromAs中记录的不一致，启动定时器等接入层上报最新的MS Radio Access capability IE信息*/

#define     GMM_TIMER_DETACH_FOR_POWER_OFF              (29)                    /* GMM关机detach定时器 */

#define     GMM_TIMER_PROTECT_PS_DETACH                 (30)                    /* GMMCS域detach保护定时器 */

/* 注意:    增加新定时器，请同步更新 GMM_TIMER_NUM */

/* 当对所有的TIMER操作相同时，可以使用下面的宏作为TIMER的种类 */
#define     GMM_REGSTER_AND_SHUTDOWN_TIMER               99                     /* 不支持GPRS时需要停止的定时器 */
#define     GMM_TIMER_ALL                                100

#define     GMM_TIMER_ALL_EXCEPT_T3312_T3323                (101)

/* MM TIMER ID */
#define     MM_TIMER_T3210                              0
#define     MM_TIMER_T3211                              1
#define     MM_TIMER_T3212                              2
#define     MM_TIMER_T3213                              3
#define     MM_TIMER_T3214                              4
#define     MM_TIMER_T3216                              5
#define     MM_TIMER_T3218                              6
#define     MM_TIMER_T3220                              7
#define     MM_TIMER_T3230                              8
#define     MM_TIMER_T3240                              9
#define     MM_TIMER_PROTECT_AGENT                      10
#define     MM_TIMER_PROTECT_DETACH                     11
#define     MM_TIMER_PROTECT_SIGNALLING                 12
#define     MM_TIMER_PROTECT_CC                         13
#define     MM_TIMER_PROTECT_CCBS                       14
#define     MM_TIMER_PROTECT_SUSPEND                    15
/* G下，联合注册, CS失败5次后，延迟1秒发起LU*/
#define     MM_TIMER_DELAY_LU_GSM                       16
/* G下，联合注册成功后，CS需要延迟发起缓存的CS业务，等待GMM RAU过程完全结束,
   暂定延迟1秒 */
#define     MM_TIMER_DELAY_CS_SERVICE_GSM               17
/* 注意:    增加新定时器，请同步更新 MM_TIMER_NUM */

/* 主动请求底层释放连接后，启动底层连接释放的保护定时器 */
#define     MM_TIMER_WAIT_CONNECT_REL                   (18)

/* CSFB 流程从L HO到GU后等系统消息时长定时器 */
#define     MM_TIMER_HO_WAIT_SYSINFO                    (19)

/* 紧急呼CSFB 流程从L HO到GU后等系统消息时长定时器 */
#define     MM_TIMER_EMERGENCY_CSFB_HO_WAIT_SYSINFO      (20)

/* CSPS 网络模式I的情况下, 关机时由于MM的很多状态都不处理GMMMM_GPRS_DETACH_COMPLETED,
   于是MM不向MMC回复POWER_OFF_CNF. 为规避此问题, 当MM收到POWER_OFF_REQ时启动定时器,
   当定时器超时后, MM向MMC回复POWER_OFF_CNF.
   启动原则:
   1. 收到关机请求时如果已向MMC回复POWER_OFF_CNF, 则不需启动
   2. 收到关机请求时如果有启动其它保护定时器,如MM_TIMER_PROTECT_DETACH, 则不需启动
   3. 其它情况, 如果是CSPS 网络模式I的情况, 则需要启动此定时器
   无需显示停止此定时器, 因为再回复POWER_OFF_CNF时会 Mm_TimerStop(MM_TIMER_STOP_ALL). */
#define     MM_TIMER_MODE_I_CS_PS_POWER_OFF_PROTECT      (21)

#define     MM_TIMER_PROTECT_CS_DETACH                   (22)


/* 增加MM模块的定时器，记得需要修改定时器的总个数: MM_TIMER_NUM */


#define     MM_TIMER_WAIT_GET_HO_SECU_INFO_CNF          (23)

/* MMA TIMER ID List */
#define TAF_USIM_OPPIN                  (80)      /*操作PIN的定时器ID */
#define TAF_USIM_SPN                    (81)      /*读SPN文件定时器   */
#define MMA_INTERNAL_TIMER_ID           (0x82)    /*内部启动定时器，用于ATTACH操作与DETACH操作*/
#define MMA_TIMER_FOR_PC_REPLAY         (40)      /* 新增加定时器，周期性输出MM层回放消息 */
#define TI_MN_PH_RESET                  (41)
#define TI_MN_PH_PIN_OPER               (42)
#define TI_MN_PH_CPHS_SIM_ONS           (43)

#define TI_MN_MMA_SYSCFG_WAIT_DETACH_CNF           (44)    /*syscfg设置需要detach时等待detach cnf的保护定时器 ，时长20s*/


/* 6F15文件Refresh触发自动搜网，周期性尝试定时器 */
#define TI_TAF_MMA_PERIOD_TRYING_NETWORK_SELECTION_MENU      (45)

/* 4F36文件Refresh触发SYSCFG，周期性尝试定时器*/
#define TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING               (46)

#define TI_TAF_MMA_PNN_LIST_MEM_PROTECT_TIMER           (47)     /* 收到AT^PNN查询PNN LIST的请求时启动定时器，时长为15s */


/* 延迟掉电关机定时器: E5掉电关机前需延迟50ms供AT结果回复，将TASKDELAY修改为定时器 */
#define TI_TAF_MMA_DELAY_POWER_DOWN                          (48)

#define TI_TAF_MMA_NET_SCAN_TIMER                           (49)
#define TI_TAF_MMA_ABORT_NET_SCAN_TIMER                     (50)

/******************************************************************************/
/* End of NAS Timer List */
/******************************************************************************/

#define     MAX_DYNAMIC_MSG_LEN                             1024

#ifndef WUEPS_MEM_NO_WAIT
#define     WUEPS_MEM_NO_WAIT                               (VOS_UINT32)1       /* 不等待   */
#endif

#ifndef WUEPS_MEM_WAIT
#define     WUEPS_MEM_WAIT                                  (VOS_UINT32)(-1)    /* 等待     */
#endif

#define NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetfileInfo, enAppTypePara, usEfIdPara, ucRecordNumPara) \
{ \
    (pstGetfileInfo)->usEfId       = usEfIdPara; \
    (pstGetfileInfo)->ucRecordNum  = ucRecordNumPara; \
    (pstGetfileInfo)->enAppType    = enAppTypePara; \
    (pstGetfileInfo)->ucRsv        = 0; \
}

#define NAS_COMM_BUILD_USIM_SET_FILE_INFO(pstSetfileInfo, enAppTypePara, usEfIdPara, ucRecordNumPara, ulEfLenPara, pucEfPara) \
{ \
    (pstSetfileInfo)->enAppType    = enAppTypePara; \
    (pstSetfileInfo)->usEfId       = usEfIdPara; \
    (pstSetfileInfo)->ucRecordNum  = ucRecordNumPara; \
    (pstSetfileInfo)->ucRsv        = 0; \
    (pstSetfileInfo)->ulEfLen      = ulEfLenPara; \
    (pstSetfileInfo)->pucEfContent = pucEfPara; \
}

#define NAS_COMM_MIN(ucValue1, ucValue2) \
    (ucValue1) > (ucValue2) ? (ucValue2) : (ucValue1);


#define NAS_COMM_BULID_ERRLOG_HEADER_INFO(pstHeader, ModemId, AlmId, AlmLevel, ulSlice, ulLength) \
{ \
    (pstHeader)->ulMsgModuleId     = OM_ERR_LOG_MOUDLE_ID_GUNAS; \
    (pstHeader)->usModemId         = ModemId; \
    (pstHeader)->usAlmId           = AlmId; \
    (pstHeader)->usAlmLevel        = AlmLevel; \
    (pstHeader)->usAlmType         = NAS_ERR_LOG_ALM_TYPE_COMMUNICATION; \
    (pstHeader)->usAlmLowSlice     = ulSlice; \
    (pstHeader)->usAlmHighSlice    = 0; \
    (pstHeader)->ulAlmLength       = ulLength; \
}


#define NAS_COMM_BULID_FTM_HEADER_INFO(pstHeader, ulLen, ModemId, usFuncId) \
{ \
    (pstHeader)->ulMsgModuleId     = OM_ERR_LOG_MOUDLE_ID_GUNAS; \
    (pstHeader)->usModemId         = ModemId; \
    (pstHeader)->usProjectId       = usFuncId; \
    (pstHeader)->ulProjectLength   = ulLen; \
}


/*****************************************************************************
  3类型定义
*****************************************************************************/
typedef struct
{
    VOS_PID                             ulPid;                    /* 启动Timer的PID */
    VOS_UINT32                          ulTimerName;              /* Timer名 */
    VOS_TIMER_PRECISION_ENUM_UINT32     ulPrecision;              /* 定时器精度 */
}NAS_TIMER_PRECISION_STRU;


enum NAS_TIMER_OPERATION_TYPE_ENUM
{
    NAS_TIMER_OPERATION_START,
    NAS_TIMER_OPERATION_STOP
};
typedef VOS_UINT32  NAS_TIMER_OPERATION_TYPE_ENUM_UINT32;


typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;
    VOS_UINT32                          ulTimeLen;
    NAS_TIMER_OPERATION_TYPE_ENUM_UINT32        ulTimeAction;
}NAS_TIMER_OPERATION_STRU;

/* 巴西TIM拨号错误码结构体 */
typedef struct
{
    VOS_UINT32    ulUsed;                           /* 1: 错误码有效 0：错误码无效 */
    VOS_UINT32    ulErrCode;
}SM_PDP_ACT_ERR_CODE_STRU;
typedef struct
{
    VOS_UINT16   usPid;
    VOS_UINT16   usTimerName;
}NAS_TIMER_EVENT_STRU;


typedef struct
{
    VOS_UINT32 ulItems;
    VOS_UINT32 aulTimerMsg[NAS_MAX_TIMER_EVENT];
}NAS_TIMER_EVENT_INFO_STRU;


typedef struct
{
    VOS_UINT32                          ulCommand;
    NAS_TIMER_EVENT_INFO_STRU           stTimerMsg;
}OM_MMC_TIMER_REPORT_CFG_REQ_STRU;

#define NAS_TRACE_BUF_LEN 	            (256)

#define NAS_TRACE_LEVEL_LOW             (0x00000001)
#define NAS_TRACE_LEVEL_MED             (0x00000002)
#define NAS_TRACE_LEVEL_HIGH            (0x00000004)
#define NAS_TRACE_LEVEL_TOP             (NAS_TRACE_LEVEL_LOW | NAS_TRACE_LEVEL_MED | NAS_TRACE_LEVEL_HIGH)

#define NAS_TRACE_OUTPUT_ASHELL         (0x00000001)
#define NAS_TRACE_OUTPUT_CSHELL         (0x00000002)
#define NAS_TRACE_OUTPUT_ALL            (NAS_TRACE_OUTPUT_ASHELL | NAS_TRACE_OUTPUT_CSHELL)

#define NAS_MNTN_LOG_FORMAT(ulPrintLength, pcBuf, ulBufSize, pcFmt)\
            {\
                va_list pArgList;\
                va_start(pArgList, pcFmt);\
                ulPrintLength += VOS_nvsprintf(pcBuf + ulPrintLength,\
                                    ulBufSize - ulPrintLength, pcFmt, pArgList);\
                va_end(pArgList);\
                if (ulPrintLength > (ulBufSize - 1))\
                {\
                    ulPrintLength = ulBufSize - 1;\
                }\
                *(pcBuf + ulPrintLength) = '\0';\
            }

#if (VOS_OS_VER == VOS_WIN32) || defined(_lint)
#define NAS_DBG_PRINT(lvl, fmt, ...)\
            vos_printf(fmt, ##__VA_ARGS__)
#else
#define NAS_DBG_PRINT(lvl, fmt, ...)\
            do\
            {\
                if (lvl == (g_ulNasTraceLevle & lvl))\
                {\
                    NAS_MNTN_LogPrintf("[TICK:%u][%s][LINE:%d] "fmt"\n", VOS_GetTick(), __FUNCTION__, __LINE__, ##__VA_ARGS__);\
                }\
            }while(0)
#endif

#define NAS_TRACE_LOW(...)\
            NAS_DBG_PRINT(NAS_TRACE_LEVEL_LOW, __VA_ARGS__)

#define NAS_TRACE_MED(...)\
            NAS_DBG_PRINT(NAS_TRACE_LEVEL_MED, __VA_ARGS__)

#define NAS_TRACE_HIGH(...)\
            NAS_DBG_PRINT(NAS_TRACE_LEVEL_HIGH, __VA_ARGS__)


/*****************************************************************************
  4 宏定义
*****************************************************************************/

#define     Cm_MemSet(pBuf,Data,Len)    PS_NAS_MEM_SET((pBuf),(Data),(Len))
#define     Cm_MemCpy(pDst,pSrc,Len)    PS_NAS_MEM_CPY((pDst),(pSrc),(Len))
#define     Cm_Malloc(ulPid, ulSize)    PS_NAS_MEM_ALLOC((ulPid),(ulSize), WUEPS_MEM_NO_WAIT)
#define     Cm_Free(ulPid, pMem)        PS_NAS_MEM_FREE((ulPid),(pMem))

#define     NAS_MEM_FAIL()              PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "NAS Mem Operation Failed!");
#define     NAS_MSG_FAIL()              PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "NAS Msg Opration Failed!");
#define     NAS_TIMER_FAIL()            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "NAS Timer Opration Failed!");

/*****************************************************************************
 NAS层LOG函数定义:
*****************************************************************************/
#ifndef SUBMOD_NULL
#define    SUBMOD_NULL                                                  (0)
#endif

#define    NAS_INFO_LOG(Mod, String)                                    PS_LOG ( (Mod), SUBMOD_NULL,  PS_PRINT_INFO, (String) )
#define    NAS_INFO_LOG1(Mod, String,Para1)                             PS_LOG1 ( (Mod), SUBMOD_NULL, PS_PRINT_INFO, (String), (VOS_INT32)(Para1) )
#define    NAS_INFO_LOG2(Mod, String,Para1,Para2)                       PS_LOG2 ( (Mod), SUBMOD_NULL, PS_PRINT_INFO, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2) )
#define    NAS_INFO_LOG3(Mod, String,Para1,Para2,Para3)                 PS_LOG3 ( (Mod), SUBMOD_NULL, PS_PRINT_INFO, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3) )
#define    NAS_INFO_LOG4(Mod, String,Para1,Para2,Para3,Para4)           PS_LOG4 ( (Mod), SUBMOD_NULL, PS_PRINT_INFO, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3), (VOS_INT32)(Para4) )

#define    NAS_NORMAL_LOG(Mod, String)                                  PS_LOG ( (Mod), SUBMOD_NULL,  PS_PRINT_NORMAL, (String) )
#define    NAS_NORMAL_LOG1(Mod, String,Para1)                           PS_LOG1 ( (Mod), SUBMOD_NULL, PS_PRINT_NORMAL, (String), (VOS_INT32)(Para1) )
#define    NAS_NORMAL_LOG2(Mod, String,Para1,Para2)                     PS_LOG2 ( (Mod), SUBMOD_NULL, PS_PRINT_NORMAL, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2) )
#define    NAS_NORMAL_LOG3(Mod, String,Para1,Para2,Para3)               PS_LOG3 ( (Mod), SUBMOD_NULL, PS_PRINT_NORMAL, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3) )
#define    NAS_NORMAL_LOG4(Mod, String,Para1,Para2,Para3,Para4)         PS_LOG4 ( (Mod), SUBMOD_NULL, PS_PRINT_NORMAL, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3), (VOS_INT32)(Para4) )

#define    NAS_WARNING_LOG(Mod, String)                                 PS_LOG ( (Mod), SUBMOD_NULL,  PS_PRINT_WARNING, (String) )
#define    NAS_WARNING_LOG1(Mod, String,Para1)                          PS_LOG1 ( (Mod), SUBMOD_NULL, PS_PRINT_WARNING, (String), (VOS_INT32)(Para1) )
#define    NAS_WARNING_LOG2(Mod, String,Para1,Para2)                    PS_LOG2 ( (Mod), SUBMOD_NULL, PS_PRINT_WARNING, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2) )
#define    NAS_WARNING_LOG3(Mod, String,Para1,Para2,Para3)              PS_LOG3 ( (Mod), SUBMOD_NULL, PS_PRINT_WARNING, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3) )
#define    NAS_WARNING_LOG4(Mod, String,Para1,Para2,Para3,Para4)        PS_LOG4 ( (Mod), SUBMOD_NULL, PS_PRINT_WARNING, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3), (VOS_INT32)(Para4) )

#define    NAS_ERROR_LOG(Mod, String)                                   PS_LOG ( (Mod), SUBMOD_NULL,  PS_PRINT_ERROR, (String) )
#define    NAS_ERROR_LOG1(Mod, String,Para1)                            PS_LOG1 ( (Mod), SUBMOD_NULL, PS_PRINT_ERROR, (String), (VOS_INT32)(Para1) )
#define    NAS_ERROR_LOG2(Mod, String,Para1,Para2)                      PS_LOG2 ( (Mod), SUBMOD_NULL, PS_PRINT_ERROR, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2) )
#define    NAS_ERROR_LOG3(Mod, String,Para1,Para2,Para3)                PS_LOG3 ( (Mod), SUBMOD_NULL, PS_PRINT_ERROR, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3) )
#define    NAS_ERROR_LOG4(Mod, String,Para1,Para2,Para3,Para4)          PS_LOG4 ( (Mod), SUBMOD_NULL, PS_PRINT_ERROR, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3), (VOS_INT32)(Para4) )


/* 对PID,MsgName,以及EventType的操作 */
#define NAS_BuildEventType(usSndPid,usMsgName)         (((usSndPid) << 16) | (usMsgName))                    /* 根据 Pid以及usMsgName 构建EventType  */
#define NAS_ExtractMsgNameFromEvtType(ulEventType)     ((VOS_UINT16)((ulEventType) & 0x0000FFFF))          /* 从EventType中获取MsgName     */
#define NAS_ExtractSndPidFromEvtType(ulEventType)      ((VOS_UINT16)(((ulEventType) & 0xFFFF0000) >> 16)) /* 从EventType中获取SndPid      */

#define NAS_GetMsgSenderPid(pMsg)                      (((MSG_HEADER_STRU*)pMsg)->ulSenderPid)
#define NAS_GetMsgLength(pMsg)                         (((MSG_HEADER_STRU*)pMsg)->ulLength)
#define NAS_GetMsgName(pMsg)                           (((MSG_HEADER_STRU*)pMsg)->ulMsgName)
#define NAS_GetTimerName(pMsg)                         (PS_GET_REL_TIMER_NAME(pMsg))




/*****************************************************************************
  5 全局变量声明
*****************************************************************************/
/*NAS收到USIM 卡状态上报事件的slice*/
extern VOS_UINT32 g_ulUsimChangeProcSlice;

/*NAS收到PLMN SEARCH CNF的slice*/
extern VOS_UINT32 g_ulPlmnSrchCnfSlice;

/*NAS收到PS注册成功的slice*/
extern VOS_UINT32 g_ulPSRegSuccSlice;

extern VOS_UINT32 g_ulNasTraceLevle;

/*****************************************************************************
  6 接口函数声明
*****************************************************************************/
extern VOS_UINT32   OM_TraceMsgHook(VOS_VOID *pMsg);

extern VOS_UINT32   NAS_StartRelTimer(
    HTIMER                              *phTm,
    VOS_PID                             Pid,
    VOS_UINT32                          ulLength,
    VOS_UINT32                          ulName,
    VOS_UINT32                          ulParam,
    VOS_UINT8                           ucMode
);

extern VOS_TIMER_PRECISION_ENUM_UINT32 NAS_GetTimerPrecision(
        VOS_PID                             Pid,
        VOS_UINT32                          ulName
    );
extern VOS_UINT32   NAS_StopRelTimer(
    VOS_PID                             Pid,
    VOS_UINT32                          ulName,
    HTIMER                              *phTm
);


extern TAF_PH_RAT_TYPE_ENUM_UINT8  MMC_GetCurRadioMode(VOS_VOID);

/***  All the following functions are defined in MM_CellProc1.c  ***/
extern VOS_VOID NAS_MM_StoreCsUnavailableInfo(
    VOS_UINT16                          usCause,
    NAS_MNTN_CS_UNAVAILABLE_INFO_STRU  *pstCsUnavailableInfo
);

/***  All the following functions are defined in GmmAttach.c  ***/
extern VOS_VOID NAS_GMM_StorePsUnavailableInfo(
    VOS_UINT8                           ucCause,
    NAS_MNTN_PS_UNAVAILABLE_INFO_STRU  *pstPsUnavailableInfo
);

/***  All the following functions are defined in MMC_Main.c  ***/
extern VOS_VOID NAS_MMC_OutputPositionInfo(NAS_MNTN_POSITION_INFO_STRU *pstPositionInfo);

/***  All the following functions are defined in MMC_Com.c  ***/

extern VOS_UINT32 NAS_MMC_IsRoam();

/***  All the following functions are defined in GmmAttach.c  ***/
extern VOS_VOID NAS_MNTN_ServiceUnavailableType(
    VOS_UINT16                          usCause,
    VOS_BOOL                           *pbEventRequired
);

/***  All the following functions are defined in MmaAppLocal.c  ***/
extern VOS_VOID NAS_MMA_OutputPlmnSrchBand(VOS_UINT32 *pulBand);

extern VOS_VOID NAS_MMA_OutputUsimInfo(NAS_MNTN_USIM_INFO_STRU *pstUsimInfo);

/***  All the following functions are defined in SmRcvGmm.c  ***/
extern VOS_VOID NAS_MNTN_RecordPdpActiveFail(
    NAS_MNTN_ACTIVE_FAIL_TYPE_ENUM_UINT32                       enFailType,
    VOS_UINT8                                                   ucCntxtIndex,
    VOS_UINT8                                                   ucSmCause,
    VOS_UINT8                                                   ucRegisterFlg
);

/***  All the following functions are defined in NasCom.c  ***/
extern VOS_VOID NAS_MNTN_RecordPlmnUnavailable(
    VOS_UINT8                           ucValIndex,
    VOS_UINT8                           ucCurTimes
);



VOS_VOID  NAS_SetNasOtaSwitch(
    VOS_UINT32                          ulNasOTASwitch
);

VOS_UINT32 NAS_GetNasOtaSwitch( VOS_VOID );

extern VOS_BOOL NAS_PreventTestImsiRegFlg(VOS_VOID);



VOS_UINT32 NAS_IsNeedTimerEventReport(
    VOS_UINT32                          ulEventType
);

extern NAS_TIMER_EVENT_INFO_STRU* NAS_GetTimerEventReportCfg(VOS_VOID);

VOS_VOID NAS_TIMER_EventReport(
    VOS_UINT32                          ulTimerName,
    VOS_UINT32                          ulPid,
    NAS_OM_EVENT_ID_ENUM                enEventId
);


#if (FEATURE_ON == FEATURE_PTM)
VOS_UINT16 NAS_GetErrLogAlmLevel(NAS_ERR_LOG_ALM_ID_ENUM_U16 enAlmId);
VOS_VOID NAS_COM_MntnPutRingbuf(
    VOS_UINT32                          ulName,
    VOS_UINT32                          ulPid,
    VOS_UINT8                          *pucData,
    VOS_UINT32                          ulLen
);
#endif

VOS_VOID NAS_MNTN_LogPrintf(VOS_CHAR *pcFmt, ...);


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif                                        /* __cpluscplus         */

#endif
