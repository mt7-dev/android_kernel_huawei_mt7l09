
#ifndef __SLEEPFLOW_H__
#define __SLEEPFLOW_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "DrvInterface.h"
#include "omrl.h"
#include "sleepsleep.h"
#include "phyoaminterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BBP_AWAKE_BIT                           0x01

#define WSLEEP_32K_TO_64M_TYPE_BIT              0       /*DRX唤醒后时钟切换到61.44M中断*/
#define WSLEEP_64M_TO_32K_TYPE_BIT              1       /*sleep*/

#define BBP_DRX_INT_SLICE_COUNT                 800       /*BBP DRX INT SLICE DELAY COUNT*/

#define SLEEP_SLOT_LEN                          15      /*W每帧包含15个SLOT*/
#define SLEEP_SFN_CYCLE_LEN                     (4096*SLEEP_SLOT_LEN) /*SFN范围为12bit，每次轮回4096*/

#define SLEEP_WAKE_DELAY                        2       /*增加延迟防止函数执行中正常唤醒已经来到*/
#define SLEEP_WAKE_INTERVAL                     30      /*强制唤醒时刻与正常唤醒时刻间隔的最小值*/
#define SLEEP_WAKE_BASE_FRM_PROTECT_CHIP        (38200) /* 在绝对计时器先后判断时避开帧头前的CHIP */
#define SLEEP_WAKE_BASE_FRM_PROTECT_VALUE       (500)
#define SLEEP_WAKE_BBP_POS_PROTECT_SLICE        (330)   /* BBP保护流程超时门限,10ms */

/* GU下支持模式的BUTT值 */
#define SLEEP_GU_MODE_TYPE_BUTT                 (2)

#define BBP_SLEEP_32K_CLOCK_SWITCH_IND          (4)
#define BBP_SLEEP_WAKE_IND                      (5)
#define BBP_SLEEP_NORMAL_CLOCK_SWITCH_IND       (6)
#define SLEEP_TL_WAKE_GU_IND                    (7)
#define SLEEP_ACTIVATE_ERROR_ID                 (8)

#define BBP_ERROR_VALUE                         (0xDEADBEEF)

#define SLEEP_TRACK_RECORD_NUM                  (100)

#define SLEEP_RECORD_4MS                        (128)
#define SLEEP_RECORD_3MS                        (96)
#define SLEEP_RECORD_2MS                        (64)
#define SLEEP_RECORD_1MS                        (32)
#define SLEEP_RECORD_MAX_EVENT_COUNT            (5)

#define SLEEP_BBP_ERROR_TIMEOUT                 (30000)

#define SLEEP_INFO_MAX_FILE                     (2)
#define SLEEP_INFO_MAX_NAME_LEN                 (40)
#define SLEEP_INFO_MAX_SIZE                     (1024)
#define SLEEP_INFO_MAX_LEN                      (16)
#define SLEEP_INFO_RESET_ENABLE                 (0x5A5A5A5A)

#define SLEEP_DEVICE_POWUP(enModemId, enMode, enCommMode) \
    { \
        DRV_PWRCTRL_PWRUP((PWC_COMM_MODE_E)enMode, enCommMode, (PWC_COMM_MODEM_E)enModemId); \
        if ( VOS_NULL_PTR != g_pstSleepDeviceState ) \
        { \
            g_pstSleepDeviceState->aulDevicePwState[enModemId][enMode][enCommMode]  = PWRCTRL_COMM_ON; \
            g_pstSleepDeviceState->aulDeviceUpSlice[enModemId][enMode][enCommMode]  = OM_GetSlice(); \
        } \
        \
    } \


#define SLEEP_DEVICE_POWDOWN(enModemId, enMode, enCommMode) \
    { \
        DRV_PWRCTRL_PWRDOWN((PWC_COMM_MODE_E)enMode, enCommMode, (PWC_COMM_MODEM_E)enModemId); \
        if ( VOS_NULL_PTR != g_pstSleepDeviceState ) \
        { \
            g_pstSleepDeviceState->aulDevicePwState[enModemId][enMode][enCommMode]      = PWRCTRL_COMM_OFF; \
            g_pstSleepDeviceState->aulDeviceDownSlice[enModemId][enMode][enCommMode]    = OM_GetSlice(); \
        } \
        \
    } \

#define SLEEP_DEVICEPLL_POWUP(enModemId, enMode, enCommMode) \
    { \
        DRV_PWRCTRL_PLLENABLE((PWC_COMM_MODE_E)enMode, enCommMode, (PWC_COMM_MODEM_E)enModemId); \
        if ( VOS_NULL_PTR != g_pstSleepDeviceState ) \
        { \
            g_pstSleepDeviceState->aulPllPwState[enModemId][enMode][enCommMode]  = PWRCTRL_COMM_ON; \
            g_pstSleepDeviceState->aulPLLUpSlice[enModemId][enMode][enCommMode]  = OM_GetSlice(); \
        } \
        \
    } \


#define SLEEP_DEVICEPLL_POWDOWN(enModemId, enMode, enCommMode) \
    { \
        DRV_PWRCTRL_PLLDISABLE((PWC_COMM_MODE_E)enMode, enCommMode, (PWC_COMM_MODEM_E)enModemId); \
        if ( VOS_NULL_PTR != g_pstSleepDeviceState ) \
        { \
            g_pstSleepDeviceState->aulPllPwState[enModemId][enMode][enCommMode]     = PWRCTRL_COMM_OFF; \
            g_pstSleepDeviceState->aulPLLDownSlice[enModemId][enMode][enCommMode]   = OM_GetSlice(); \
        } \
        \
    } \

/* 通道参数 */
#define SLEEP_ABB_CHAN(enMode)          (g_stSleepChanPara.astModeBasicParam[enMode].ucABBSwitch)
#define SLEEP_RF_CHAN(enMode)           (g_stSleepChanPara.astModeBasicParam[enMode].ucRFSwitch)
#define SLEEP_TCXO_CHAN(enMode)         (g_stSleepChanPara.astModeBasicParam[enMode].ucTCXOSwitch)

#define SLEEP_DEBUG

#ifdef SLEEP_DEBUG

#define SLEEP_ISR_RECORD(ulMsgId, enMode) \
    if (VOS_NULL_PTR != g_pstSleepTrackRecord) \
    { \
        g_pstSleepTrackRecord->astIsrRecord[g_pstSleepTrackRecord->ulIsrIndex].usMsgId    = (VOS_UINT16)ulMsgId; \
        g_pstSleepTrackRecord->astIsrRecord[g_pstSleepTrackRecord->ulIsrIndex].usModeType = (VOS_UINT16)enMode; \
        g_pstSleepTrackRecord->astIsrRecord[g_pstSleepTrackRecord->ulIsrIndex].ulSlice    = OM_GetSlice(); \
        \
        g_pstSleepTrackRecord->ulIsrIndex++; \
        /* 达到末尾需要翻转*/ \
        if (SLEEP_TRACK_RECORD_NUM == g_pstSleepTrackRecord->ulIsrIndex) \
        { \
            g_pstSleepTrackRecord->ulIsrIndex = 0; \
        } \
    } \

#define SLEEP_MSG_RECORD(ulMsgId, enMode, StartSlice) \
    if (VOS_NULL_PTR != g_pstSleepTrackRecord) \
    { \
        g_pstSleepTrackRecord->astMsgRecord[g_pstSleepTrackRecord->ulMsgIndex].usMsgId      = (VOS_UINT16)ulMsgId; \
        g_pstSleepTrackRecord->astMsgRecord[g_pstSleepTrackRecord->ulMsgIndex].usModeType   = (VOS_UINT16)enMode; \
        g_pstSleepTrackRecord->astMsgRecord[g_pstSleepTrackRecord->ulMsgIndex].ulStartSlice = StartSlice; \
        g_pstSleepTrackRecord->astMsgRecord[g_pstSleepTrackRecord->ulMsgIndex].ulEndSlice   = OM_GetSlice(); \
        \
        g_pstSleepTrackRecord->ulMsgIndex++; \
        /* 达到末尾需要翻转*/ \
        if (SLEEP_TRACK_RECORD_NUM == g_pstSleepTrackRecord->ulMsgIndex) \
        { \
            g_pstSleepTrackRecord->ulMsgIndex = 0; \
        } \
    } \

#define SLEEP_NOR_STATE_TRACK(enMode, enType) \
    if (VOS_NULL_PTR != g_pstSleepNormalState) \
    { \
        g_pstSleepNormalState->astRecordItem[enMode][enType].ulCount++; \
        g_pstSleepNormalState->astRecordItem[enMode][enType].ulSlice = OM_GetSlice(); \
    } \

#else

#define SLEEP_ISR_RECORD(ulMsgId, enMode)
#define SLEEP_MSG_RECORD(ulMsgId, enMode, ulStartSlice) (ulStartSlice = ulStartSlice) /* for pclint
 */
#define SLEEP_NOR_STATE_TRACK(enMode, enType)

#endif

#define HPA_UART_PRINT(data)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


enum SLEEP_SYSTEM_STATE_ENUM
{
    SLEEP_NORMAL_STATE = 0,                             /* 正常状态 */
    SLEEP_MASTER_TEMP_LP_STATE,                         /* 主模低功耗状态(暂态) */
    SLEEP_MASTER_LP_STATE,                              /* 主模低功耗状态 */
    SLEEP_TEMP_NORMAL_STATE,                            /* 正常状态(暂态) */
    SLEEP_SLAVE_LP_STATE,                               /* 从模低功耗状态 */
    SLEEP_STATE_BUTT
};
typedef VOS_UINT32 SLEEP_SYSTEM_STATE_ENUM_UINT32;
enum SLEEP_NORMAL_STATE_TRACK_ENUM
{
    SLEEP_MASTER_BBP_32K = 0,
    SLEEP_MASTER_PHY_HALT,
    SLEEP_MASTER_LP,
    SLEEP_MASTER_BBP_WAKE,
    SLEEP_MASTER_ACTIVATE_START,
    SLEEP_MASTER_ACTIVATE_END,
    SLEEP_MASTER_BBP_RESUME_START,
    SLEEP_MASTER_BBP_RESUME_END,
    SLEEP_MASTER_FORCE_WAKE,
    SLEEP_MASTER_BBP_NORMAL,
    SLEEP_MASTER_WAKE_PHY_PRE,
    SLEEP_MASTER_WAKE_PHY,
    SLEEP_SLAVE_PHY_HALT,
    SLEEP_SLAVE_LP,
    SLEEP_SLAVE_FORCE_WAKE,
    SLEEP_NORMAL_BUTT
};


enum SLEEP_CHANNEL_ENUM
{
    SLEEP_CHANNEL_ZERO = 0,
    SLEEP_CHANNEL_ONE,
    SLEEP_CHANNEL_BOTH,
};

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

/*****************************************************************************
 结构名    : SLEEP_MODE_INFO_STRU
 结构说明  : GU模当前系统状态
*****************************************************************************/
typedef struct
{
    SLEEP_SYSTEM_STATE_ENUM_UINT32      enSystemState;
    UPHY_OAM_WAKE_TYPE_ENUM_UINT32      enWakeType;
}SLEEP_MODE_INFO_STRU;

/*****************************************************************************
 结构名    : SLEEP_MSG_FUN_STRU
 结构说明  : 指示各消息的处理函数
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulMsgId;                /* 消息ID */
    Msg_Fun_Type                        pMsgFun;                /* 消息处理函数 */
}SLEEP_MSG_FUN_STRU;

/*****************************************************************************
 结构名    : SLEEP_INFO_STATE_STRU
 结构说明  : GU模定位信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulAwakeIsrSlice;
    VOS_UINT32                      ulAwakeStartSlice;
    VOS_UINT32                      ulAwakeEndSlice;
    VOS_UINT32                      ulSwitchIsrSlice;
    VOS_UINT32                      ulSwitchTaskSlice;
    VOS_UINT32                      ulWarnEventFlag;
    VOS_UINT32                      ulWarnEventCount;
    VOS_UINT32                      ulSwitchDelayType1Count;
    VOS_UINT32                      ulSwitchDelayType2Count;
    VOS_UINT32                      ulSwitchDelayType3Count;
    VOS_UINT32                      ulSwitchDelayType4Count;
    VOS_UINT32                      ulSwitchDelayType5Count;
    VOS_UINT32                      ulEndFlag;
}SLEEP_INFO_STATE_STRU;

/*****************************************************************************
 结构名    : SLEEP_ISR_MSG_STRU
 结构说明  : 中断消息体
*****************************************************************************/
typedef struct
{
   VOS_MSG_HEADER
   VOS_UINT16                           usMsgId;                /* 消息ID */
   VOS_UINT16                           usReserved;             /* 保留 */
   SLEEP_MODE_ENUM_UINT32               enRatMode;              /* 上报模式 */
   VOS_UINT32                           ulSlice;                /* 中断接收时间 */
}SLEEP_ISR_MSG_STRU;

/*****************************************************************************
 结构名    : SLEEP_RECORD_ITEM_STRU
 结构说明  : 流程记录单元
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usMsgId;                /* 消息ID */
    VOS_UINT16                          usModeType;             /* 模式类型 */
    VOS_UINT32                          ulSlice;                /* Slice值 */
}SLEEP_RECORD_ITEM_STRU;

/*****************************************************************************
 结构名    : SLEEP_RECORD_ITEM_EX_STRU
 结构说明  : 流程记录扩展单元
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usMsgId;                /* 消息ID */
    VOS_UINT16                          usModeType;             /* 模式类型 */
    VOS_UINT32                          ulStartSlice;           /* 开始Slice值 */
    VOS_UINT32                          ulEndSlice;             /* 结束Slice值 */
}SLEEP_RECORD_ITEM_EX_STRU;

/*****************************************************************************
 结构名    : SLEEP_TRACK_RECORD_STRU
 结构说明  : 保存SLEEP中断和消息流程记录
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulIsrIndex;             /* 中断记录索引 */
    VOS_UINT32                          ulMsgIndex;             /* 消息记录索引 */
    SLEEP_RECORD_ITEM_STRU              astIsrRecord[SLEEP_TRACK_RECORD_NUM]; /* 中断流程记录 */
    SLEEP_RECORD_ITEM_EX_STRU           astMsgRecord[SLEEP_TRACK_RECORD_NUM]; /* 消息流程记录 */
}SLEEP_TRACK_RECORD_STRU;

/*****************************************************************************
 结构名    : SLEEP_NORMAL_STATE_RECORD_STRU
 结构说明  : 保存SLEEP正常状态切换流程
*****************************************************************************/
typedef struct
{
    SLEEP_RECORD_STRU   astRecordItem[SLEEP_GU_MODE_TYPE_BUTT][SLEEP_NORMAL_BUTT];
}SLEEP_NORMAL_STATE_RECORD_STRU;

/*****************************************************************************
 结构名    : SLEEP_DEVICE_STATE
 结构说明  : GU模当前器件状态
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulDeviceFlag;
    VOS_UINT32                      aulDevicePwState[SLEEP_CHANNEL_BOTH][SLEEP_GU_MODE_TYPE_BUTT][PWC_COMM_MODULE_BUTT];
    VOS_UINT32                      aulDeviceUpSlice[SLEEP_CHANNEL_BOTH][SLEEP_GU_MODE_TYPE_BUTT][PWC_COMM_MODULE_BUTT];
    VOS_UINT32                      aulDeviceDownSlice[SLEEP_CHANNEL_BOTH][SLEEP_GU_MODE_TYPE_BUTT][PWC_COMM_MODULE_BUTT];
    VOS_UINT32                      aulPllPwState[SLEEP_CHANNEL_BOTH][SLEEP_GU_MODE_TYPE_BUTT][PWC_COMM_MODULE_BUTT];
    VOS_UINT32                      aulPLLUpSlice[SLEEP_CHANNEL_BOTH][SLEEP_GU_MODE_TYPE_BUTT][PWC_COMM_MODULE_BUTT];
    VOS_UINT32                      aulPLLDownSlice[SLEEP_CHANNEL_BOTH][SLEEP_GU_MODE_TYPE_BUTT][PWC_COMM_MODULE_BUTT];
    SLEEP_RECORD_STRU               astDevicePowerError[SLEEP_GU_MODE_TYPE_BUTT];
    SLEEP_RECORD_STRU               stDeviceTcxoError;
    VOS_UINT32                      aulABBState[64];
    VOS_UINT32                      ulBBPResumeTimeout;
}SLEEP_DEVICE_STATE_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
extern SLEEP_NORMAL_STATE_RECORD_STRU         *g_pstSleepNormalState;

extern SLEEP_TRACK_RECORD_STRU                *g_pstSleepTrackRecord;

/*****************************************************************************
  10 函数声明
*****************************************************************************/

VOS_VOID SLEEP_EnableDspPll(VOS_VOID);
VOS_VOID SLEEP_ExitLowPower(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_BOOL SLEEP_IsSleep(VOS_VOID);
VOS_VOID Sleep_ActiveDspPll(VOS_VOID);
VOS_VOID SLEEP_NotifySndMsg(VOS_VOID);
VOS_VOID GUSLEEP_MasterAwakeSlave(VOS_VOID);
VOS_VOID SLEEP_32KIsr(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID SLEEP_NormalClockIsr(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID SLEEP_AwakeIsr(SLEEP_MODE_ENUM_UINT32 enMode);

VOS_VOID SLEEP_VoteLock(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID SLEEP_VoteUnlock(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID SLEEP_TimeCal(VOS_VOID);
VOS_VOID SLEEP_ResumeReg(VOS_VOID);
VOS_VOID SLEEP_PowerUp(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID SLEEP_DeactivateHw(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID SLEEP_ActivateHw(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID SLEEP_InfoInd( SLEEP_MODE_ENUM_UINT32 enMode );
VOS_UINT32  SLEEP_ForceWakeProtected(VOS_VOID);
VOS_UINT32  SLEEP_WakePhy(VOS_UINT32 ulMsgId, SLEEP_MODE_ENUM_UINT32 enMode, UPHY_OAM_WAKE_TYPE_ENUM_UINT32 enWakeType);
VOS_VOID    SLEEP_IsrProc(VOS_UINT32 ulMsgId, SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID    SLEEP_MasterSleepMsgProc(MsgBlock *pstMsg);
VOS_VOID    SLEEP_32KClkMsgProc(MsgBlock *pstMsg);
VOS_VOID    SLEEP_WakeMasterMsgProc(MsgBlock *pstMsg);
VOS_VOID    SLEEP_NormalClkMsgProc(MsgBlock *pstMsg);
VOS_VOID    SLEEP_SlaveSleepMsgProc(MsgBlock *pstMsg);
VOS_VOID    SLEEP_TLWakeGUProc(MsgBlock *pstMsg);
VOS_VOID    SLEEP_MasterForceWake(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID    SLEEP_SlaveForceWake(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID    SLEEP_HookMsgProc(MsgBlock *pstMsg);
VOS_VOID    SLEEP_MsgProc(MsgBlock* pstMsg);
VOS_VOID    SLEEP_Init(VOS_VOID);
VOS_UINT32  WuepsSleepPidInit(enum VOS_INIT_PHASE_DEFINE ip);
VOS_UINT32  WuepsSleepFidInit(enum VOS_INIT_PHASE_DEFINE ip);
VOS_UINT32  SLEEP_ReadBaseCntChip( VOS_VOID );

VOS_VOID    I1_SLEEP_VoteLock(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_VOID    I1_SLEEP_PowerUp(SLEEP_MODE_ENUM_UINT32 enMode);
VOS_BOOL    SLEEP_IsAwakeProc(SLEEP_MODE_ENUM_UINT32 enMode);

VOS_UINT32  SLEEP_InfoFileName( VOS_CHAR * cFileName );
VOS_UINT32  SLEEP_InfoWriteLog( VOS_UINT32 *pstInfo, VOS_UINT32 uLen );
VOS_VOID    SLEEP_ActivateHWMsgProc(MsgBlock *pstMsg);
VOS_UINT32  SLEEP_GetLogPath(VOS_CHAR *pucBuf, VOS_CHAR *pucOldPath, VOS_CHAR *pucUnitaryPath);

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SLEEPFLOW_H__ */
