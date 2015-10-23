

#ifndef __RRMDEBUG_H__
#define __RRMDEBUG_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "PsCommonDef.h"
#include "PsRrmInterface.h"
#include "RrmLog.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

#if (FEATURE_ON == FEATURE_DSDS)
/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


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
 结构名    : RRM_MODEM_STATS_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 不同Modem的统计信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32              ulRrmRcvResApplyNum;            /* RRM收到RFID资源申请的次数 */
    VOS_UINT32              ulRrmSndApplyCnfNum;            /* RRM回复RFID资源申请的次数 */
    VOS_UINT32              ulRrmRcvResReleaseNum;          /* RRM收到释放RFID资源消息的次数 */
    VOS_UINT32              ulRrmRcvRegNum;                 /* RRM收到REG消息的次数 */
    VOS_UINT32              ulRrmRcvDeregNum;               /* RRM收到DEREG消息的次数 */
}RRM_MODEM_STATS_INFO_STRU;


/*****************************************************************************
 结构名    : RRM_STATS_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RRM的统计信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                  ulRrmSndStatusIndNum;              /* RRM发送状态通知的次数 */
    RRM_MODEM_STATS_INFO_STRU   astRrmModemStats[MODEM_ID_BUTT];   /* RRM MODEM的统计信息 */
}RRM_STATS_INFO_STRU;


extern   RRM_STATS_INFO_STRU    g_stRrmStats;


/* RRM收到RFID资源申请统计量 */
#define  RRM_DBG_MODEM_RX_RES_APPLY_REQ_NUM(ModemId,n) \
    ((ModemId < MODEM_ID_BUTT) ? (g_stRrmStats.astRrmModemStats[ModemId].ulRrmRcvResApplyNum  += (n)) : PS_FALSE)

/* RRM回复RFID资源申请统计量 */
#define  RRM_DBG_MODEM_SND_RES_APPLY_CNF_NUM(ModemId,n) \
    ((ModemId < MODEM_ID_BUTT) ? (g_stRrmStats.astRrmModemStats[ModemId].ulRrmSndApplyCnfNum  += (n)) : PS_FALSE)


/* RRM收到RFID资源释放统计量 */
#define  RRM_DBG_MODEM_RX_RES_RELEASE_NUM(ModemId,n) \
    ((ModemId < MODEM_ID_BUTT) ? (g_stRrmStats.astRrmModemStats[ModemId].ulRrmRcvResReleaseNum  += (n)) : PS_FALSE)

/* RRM收到REG消息统计量 */
#define  RRM_DBG_MODEM_RX_REG_NUM(ModemId,n) \
    ((ModemId < MODEM_ID_BUTT) ? (g_stRrmStats.astRrmModemStats[ModemId].ulRrmRcvRegNum  += (n)) : PS_FALSE)

/* RRM收到DEREG消息统计量 */
#define  RRM_DBG_MODEM_RX_DEREG_NUM(ModemId,n) \
    ((ModemId < MODEM_ID_BUTT) ? (g_stRrmStats.astRrmModemStats[ModemId].ulRrmRcvDeregNum  += (n)) : PS_FALSE)

/* RRM发送STATUS IND消息统计量 */
#define  RRM_DBG_SND_STATUS_IND_NUM(n) \
    (g_stRrmStats.ulRrmSndStatusIndNum  += (n))

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

VOS_VOID  RRM_InitDebugInfo(VOS_VOID);

#endif /* FEATURE_ON == FEATURE_DSDS */


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

#endif /* end of RrmDebug.h */

