/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : voice_jb_interface.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2014年5月5日
  最近修改   :
  功能描述   : JB的接口头文件,包含SJB和AJB的接口封装
  函数列表   :
  修改历史   :
  1.日    期   : 2014年5月5日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/
#ifndef _VOICE_JB_INTERFACE_H_
#define _VOICE_JB_INTERFACE_H_


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "hme_jb_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define VOICE_JB_GetJbObjsPtr()         (&g_stVoiceJbObjs)
#define VOICE_JB_GetAjbModual()         (g_stVoiceJbObjs.pvAjbModule)                   /* 获取AJB模块的指针 */
#define VOICE_JB_GetAjbBuff()           (g_stVoiceJbObjs.pvAjbBuff)                     /* 获取AJB的Buff地址 */
#define VOICE_JB_SetAjbBuff(pAddr)      (g_stVoiceJbObjs.pvAjbBuff = pAddr)             /* 记录AJB的Buff地址 */
#define VOICE_JB_GetImsaSsrc()          (g_stVoiceJbObjs.uwSsrc)                        /* 取当前IMSA通话中的ssrc值 */
#define VOICE_JB_SetImsaSsrc(uwVar)     (g_stVoiceJbObjs.uwSsrc = uwVar)                /* 设置ssrc值 */
#define VOICE_JB_IsAjbEnable()          (g_stVoiceJbObjs.uhwAjbEnabled)                 /* 获取AJB是否使能 */
#define VOICE_JB_SetAjbEnable(uhwVar)   (g_stVoiceJbObjs.uhwAjbEnabled = uhwVar)        /* 设置AJB是否使能 */
#define VOICE_JB_GetLtePktTime()        (g_stVoiceJbObjs.uwLtePktTime)                  /* 取当前的打包时长*/
#define VOICE_JB_SetLtePktTime(uwVar)   (g_stVoiceJbObjs.uwLtePktTime = uwVar)          /* 设置打包时长 */
#define VOICE_JB_ImsTxPktExist()        (g_stVoiceJbObjs.uhwTxPktExist)                 /* 获取IMS缓存的上行包是否存在 */
#define VOICE_JB_SetImsTxPktExist(uwVar)(g_stVoiceJbObjs.uhwTxPktExist = uwVar)         /* 设置IMS缓存的上行包是否存在 */
#define VOICE_JB_GetImsTxPktAddr()      (g_stVoiceJbObjs.pvTxPkt)                       /* 获取IMS缓存的上行包地址 */
#define VOICE_JB_SetImsTxPktAddr(pstPkt)(g_stVoiceJbObjs.pvTxPkt = pstPkt)              /* 设置IMS缓存的上行包地址 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* 中硬的log等级 */
enum VOICE_JB_HME_LOG_LEVEL_ENUM
{
    VOICE_JB_HME_LOG_LEVEL_ERR        = 1,                 /* ERROR */
    VOICE_JB_HME_LOG_LEVEL_WARNING    = 4,                 /* WARNING */
    VOICE_JB_HME_LOG_LEVEL_BUTT
};

/* 解码后数据类型 */
enum VOICE_FRAME_TYPE_ENUM
{
    VOICE_FRAME_TYPE_OTHER            = 0,                 /* 其他 */
    VOICE_FRAME_TYPE_NO_DATA,                              /* NoData帧 */
    VOICE_FRAME_TYPE_SID,                                  /* SID帧 */
    VOICE_FRAME_TYPE_VOICE_GOOD,                           /* 语音帧 */
    VOICE_FRAME_TYPE_BUTT
};
typedef VOS_UINT16 VOICE_FRAME_TYPE_ENUM_UINT16;

/* LTE上行数据模式，多久发一次数据给IMSA */
enum VOICE_JB_PKT_TIME_ENUM
{
    VOICE_JB_PKT_TIME_20MS            = 20,                /* 20ms */
    VOICE_JB_PKT_TIME_40MS            = 40,                /* 40ms */
    VOICE_JB_PKT_TIME_BUTT
};
typedef VOS_UINT32 VOICE_JB_PKT_TIME_ENUM_UINT32;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/

/*****************************************************************************
  5 消息定义
*****************************************************************************/

/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : VOICE_MC_IMS_CTRL_STRU
 功能描述  : 描述IMS语音的一些参数
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwSsrc;                    /* VoLTE通话时的SSRC，变化时需要InitJB */
    VOS_UINT32                          uwLtePktTime;              /* VoLTE时的打包时长，即多长时间发一包 */
    VOS_UINT16                          uhwAjbEnabled;             /* AJB是否使能,0/1:否/是 */
    VOS_UINT16                          uhwTxPktExist;             /* 本结构体中的上行包是否存在,0/1:否/是 */
    VOS_VOID                           *pvTxPkt;                   /* 上行IMS帧缓存帧的指针，当打包时长为40ms时，
                                                                      第奇数帧指针存在此，与下一个偶数帧一起发 */
    VOS_VOID                           *pvAjbBuff;                 /* AJB申请的内存地址 */
    VOS_VOID                           *pvAjbModule;               /* AJB的控制块指针 */
} VOICE_JB_OBJS_STRU;

/*****************************************************************************
 实体名称  : VOICE_MC_IMS_CTRL_STRU
 功能描述  : 描述IMS语音的一些参数
*****************************************************************************/
typedef struct
{
    HME_JB_HIFI_STATICS                 stData;                    /* SJB勾取的数据与AJB保持一致 */
} VOICE_JB_HOOK_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/

/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOICE_JB_OBJS_STRU              g_stVoiceJbObjs;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID VOICE_JB_Init(VOS_VOID);
extern VOS_UINT32 VOICE_JB_Start(VOS_UINT16  uhwCodecType);
extern VOS_VOID VOICE_JB_Free(VOS_VOID);
extern VOS_UINT32 VOICE_JB_AddPkt( VOS_VOID *pstPkt );
extern VOS_UINT32 VOICE_JB_GetSjbPkt(VOS_VOID);
extern VOS_UINT32 VOICE_JB_GetAjbPcmData(VOS_UINT16 usLength, VOS_VOID *pPcmOut, VOS_UINT32 *puiOutLen);
extern VOS_VOID VOICE_JB_SendImsaPkt(VOS_VOID *pvEncRslt);
extern VOS_UINT32 VOICE_JB_SetSjbPktTimeTx(VOS_UINT32 uwPktTime);

extern VOS_INT32 VOICE_JB_AjbGetTsInMs(unsigned int *uwTimeStamp);
extern VOS_INT32 VOICE_JB_AjbWriteLog(VOS_VOID *pLogMessage);
extern VOS_INT32 VOICE_JB_AjbWriteHookTrace(VOS_VOID *pTraceData);
extern VOS_INT32 VOICE_JB_AjbDecode(void *pstJBmodule, VOS_VOID *pstDecPrm);

#ifdef __cplusplus
}
#endif

#endif /* end of _VOICE_JB_INTERFACE_H_ */

