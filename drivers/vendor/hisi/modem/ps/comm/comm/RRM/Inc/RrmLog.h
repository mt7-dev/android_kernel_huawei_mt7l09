

#ifndef __RRMLOG_H__
#define __RRMLOG_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "om.h"
#include "PsLogdef.h"
#include "PsTypeDef.h"
#include "PsCommonDef.h"


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
#define     RRM_LOG_PARAM_MAX_NUM       (4)
#define     RRM_LOG_RECORD_MAX_NUM      (10)
#define     ID_RRM_TRACE_LOG_MSG        (0xcccc)


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
typedef struct
{
    VOS_UINT32                          ulTick;
    VOS_UINT32                          enLevel;
    PS_FILE_ID_DEFINE_ENUM_UINT32       enFile;
    VOS_UINT32                          ulLine;
    VOS_INT32                           alPara[RRM_LOG_PARAM_MAX_NUM];
}RRM_LOG_RECORD_STRU;

typedef struct
{
    VOS_UINT32                          ulCnt;                  /* 实际缓存的打印记录数目 */
    RRM_LOG_RECORD_STRU                 astData[RRM_LOG_RECORD_MAX_NUM];
}RRM_LOG_ENT_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16              usMsgType;
    VOS_UINT16              usTransId;
    RRM_LOG_ENT_STRU        stLogMsgCont;   /* LOG_MSG内容 */
}RRM_TRACE_LOG_MSG_STRU;



/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
extern    RRM_LOG_ENT_STRU                        g_stRrmLogEnt;     /* RRM打印实体 */

#define RRM_INIT_LOG_ENT()        RRM_MNTN_InitLogEnt(&g_stRrmLogEnt)

#if (VOS_WIN32 == VOS_OS_VER)
#define RRM_TASK_INFO_LOG(pstLogEnt, ModulePID, SubMod, String)                                 {PS_LOG((ModulePID), (SubMod),  PS_PRINT_INFO, (String));}
#define RRM_TASK_INFO_LOG1(pstLogEnt, ModulePID, SubMod, String, Para1)                         {PS_LOG1((ModulePID), (SubMod),  PS_PRINT_INFO, (String), (VOS_INT32)(Para1));}
#define RRM_TASK_INFO_LOG2(pstLogEnt, ModulePID, SubMod, String, Para1, Para2)                  {PS_LOG2((ModulePID), (SubMod),  PS_PRINT_INFO, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2));}
#define RRM_TASK_INFO_LOG3(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3)           {PS_LOG3((ModulePID), (SubMod),  PS_PRINT_INFO, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3));}
#define RRM_TASK_INFO_LOG4(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3, Para4)    {PS_LOG4((ModulePID), (SubMod),  PS_PRINT_INFO, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3), (VOS_INT32)(Para4));}

#define RRM_TASK_WARNING_LOG(pstLogEnt, ModulePID, SubMod, String)                              {PS_LOG((ModulePID), (SubMod),  PS_PRINT_WARNING, (String));}
#define RRM_TASK_WARNING_LOG1(pstLogEnt, ModulePID, SubMod, String, Para1)                      {PS_LOG1((ModulePID), (SubMod),  PS_PRINT_WARNING, (String), (VOS_INT32)(Para1));}
#define RRM_TASK_WARNING_LOG2(pstLogEnt, ModulePID, SubMod, String, Para1, Para2)               {PS_LOG2((ModulePID), (SubMod),  PS_PRINT_WARNING, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2));}
#define RRM_TASK_WARNING_LOG3(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3)        {PS_LOG3((ModulePID), (SubMod),  PS_PRINT_WARNING, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3));}
#define RRM_TASK_WARNING_LOG4(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3, Para4) {PS_LOG4((ModulePID), (SubMod),  PS_PRINT_WARNING, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3), (VOS_INT32)(Para4));}

#define RRM_TASK_ERROR_LOG(pstLogEnt, ModulePID, SubMod, String)                                {PS_LOG((ModulePID), (SubMod),  PS_PRINT_ERROR, (String));}
#define RRM_TASK_ERROR_LOG1(pstLogEnt, ModulePID, SubMod, String, Para1)                        {PS_LOG1((ModulePID), (SubMod),  PS_PRINT_ERROR, (String), (VOS_INT32)(Para1));}
#define RRM_TASK_ERROR_LOG2(pstLogEnt, ModulePID, SubMod, String, Para1, Para2)                 {PS_LOG2((ModulePID), (SubMod),  PS_PRINT_ERROR, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2));}
#define RRM_TASK_ERROR_LOG3(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3)          {PS_LOG3((ModulePID), (SubMod),  PS_PRINT_ERROR, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3));}
#define RRM_TASK_ERROR_LOG4(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3, Para4)   {PS_LOG4((ModulePID), (SubMod),  PS_PRINT_ERROR, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), (VOS_INT32)(Para3), (VOS_INT32)(Para4));}
#else
            /* 目前不输出Info级别打印 */
#define RRM_TASK_INFO_LOG(pstLogEnt, ModulePID, SubMod, String)
#define RRM_TASK_INFO_LOG1(pstLogEnt, ModulePID, SubMod, String, Para1)
#define RRM_TASK_INFO_LOG2(pstLogEnt, ModulePID, SubMod, String, Para1, Para2)
#define RRM_TASK_INFO_LOG3(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3)
#define RRM_TASK_INFO_LOG4(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3, Para4)

#define RRM_TASK_WARNING_LOG(pstLogEnt, ModulePID, SubMod, String)                              {RRM_MNTN_LogSave((pstLogEnt), (ModulePID), PS_PRINT_WARNING, THIS_FILE_ID, __LINE__, 0,0,0,0);}
#define RRM_TASK_WARNING_LOG1(pstLogEnt, ModulePID, SubMod, String, Para1)                      {RRM_MNTN_LogSave((pstLogEnt), (ModulePID), PS_PRINT_WARNING, THIS_FILE_ID, __LINE__, (VOS_INT32)(Para1),0,0,0);}
#define RRM_TASK_WARNING_LOG2(pstLogEnt, ModulePID, SubMod, String, Para1, Para2)               {RRM_MNTN_LogSave((pstLogEnt), (ModulePID), PS_PRINT_WARNING, THIS_FILE_ID, __LINE__, (VOS_INT32)(Para1),(VOS_INT32)(Para2),0,0);}
#define RRM_TASK_WARNING_LOG3(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3)        {RRM_MNTN_LogSave((pstLogEnt), (ModulePID), PS_PRINT_WARNING, THIS_FILE_ID, __LINE__, (VOS_INT32)(Para1),(VOS_INT32)(Para2),(VOS_INT32)(Para3),0);}
#define RRM_TASK_WARNING_LOG4(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3, Para4) {RRM_MNTN_LogSave((pstLogEnt), (ModulePID), PS_PRINT_WARNING, THIS_FILE_ID, __LINE__, (VOS_INT32)(Para1),(VOS_INT32)(Para2),(VOS_INT32)(Para3),(VOS_INT32)(Para4));}

#define RRM_TASK_ERROR_LOG(pstLogEnt, ModulePID, SubMod, String)                                {RRM_MNTN_LogSave((pstLogEnt), (ModulePID), PS_PRINT_ERROR, THIS_FILE_ID, __LINE__, 0,0,0,0);}
#define RRM_TASK_ERROR_LOG1(pstLogEnt, ModulePID, SubMod, String, Para1)                        {RRM_MNTN_LogSave((pstLogEnt), (ModulePID), PS_PRINT_ERROR, THIS_FILE_ID, __LINE__, (VOS_INT32)(Para1),0,0,0);}
#define RRM_TASK_ERROR_LOG2(pstLogEnt, ModulePID, SubMod, String, Para1, Para2)                 {RRM_MNTN_LogSave((pstLogEnt), (ModulePID), PS_PRINT_ERROR, THIS_FILE_ID, __LINE__, (VOS_INT32)(Para1),(VOS_INT32)(Para2),0,0);}
#define RRM_TASK_ERROR_LOG3(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3)          {RRM_MNTN_LogSave((pstLogEnt), (ModulePID), PS_PRINT_ERROR, THIS_FILE_ID, __LINE__, (VOS_INT32)(Para1),(VOS_INT32)(Para2),(VOS_INT32)(Para3),0);}
#define RRM_TASK_ERROR_LOG4(pstLogEnt, ModulePID, SubMod, String, Para1, Para2, Para3, Para4)   {RRM_MNTN_LogSave((pstLogEnt), (ModulePID), PS_PRINT_ERROR, THIS_FILE_ID, __LINE__, (VOS_INT32)(Para1),(VOS_INT32)(Para2),(VOS_INT32)(Para3),(VOS_INT32)(Para4));}
#endif


#define RRM_INFO_LOG(ModulePID, String)                                        RRM_TASK_INFO_LOG(&g_stRrmLogEnt, ModulePID, 0, String)
#define RRM_INFO_LOG1(ModulePID, String, Para1)                                RRM_TASK_INFO_LOG1(&g_stRrmLogEnt, ModulePID, 0, String, Para1)
#define RRM_INFO_LOG2(ModulePID, String, Para1, Para2)                         RRM_TASK_INFO_LOG2(&g_stRrmLogEnt, ModulePID, 0, String, Para1, Para2)
#define RRM_INFO_LOG3(ModulePID, String, Para1, Para2, Para3)                  RRM_TASK_INFO_LOG3(&g_stRrmLogEnt, ModulePID, 0, String, Para1, Para2, Para3)
#define RRM_INFO_LOG4(ModulePID, String, Para1, Para2, Para3, Para4)           RRM_TASK_INFO_LOG4(&g_stRrmLogEnt, ModulePID, 0, String, Para1, Para2, Para3, Para4)

#define RRM_WARNING_LOG(ModulePID, String)                                     RRM_TASK_WARNING_LOG(&g_stRrmLogEnt, ModulePID, 0, String)
#define RRM_WARNING_LOG1(ModulePID, String, Para1)                             RRM_TASK_WARNING_LOG1(&g_stRrmLogEnt, ModulePID, 0, String, Para1)
#define RRM_WARNING_LOG2(ModulePID, String, Para1, Para2)                      RRM_TASK_WARNING_LOG2(&g_stRrmLogEnt, ModulePID, 0, String, Para1, Para2)
#define RRM_WARNING_LOG3(ModulePID, String, Para1, Para2, Para3)               RRM_TASK_WARNING_LOG3(&g_stRrmLogEnt, ModulePID, 0, String, Para1, Para2, Para3)
#define RRM_WARNING_LOG4(ModulePID, String, Para1, Para2, Para3, Para4)        RRM_TASK_WARNING_LOG4(&g_stRrmLogEnt, ModulePID, 0, String, Para1, Para2, Para3, Para4)

#define RRM_ERROR_LOG(ModulePID, String)                                       RRM_TASK_ERROR_LOG(&g_stRrmLogEnt, ModulePID, 0, String)
#define RRM_ERROR_LOG1(ModulePID, String, Para1)                               RRM_TASK_ERROR_LOG1(&g_stRrmLogEnt, ModulePID, 0, String, Para1)
#define RRM_ERROR_LOG2(ModulePID, String, Para1, Para2)                        RRM_TASK_ERROR_LOG2(&g_stRrmLogEnt, ModulePID, 0, String, Para1, Para2)
#define RRM_ERROR_LOG3(ModulePID, String, Para1, Para2, Para3)                 RRM_TASK_ERROR_LOG3(&g_stRrmLogEnt, ModulePID, 0, String, Para1, Para2, Para3)
#define RRM_ERROR_LOG4(ModulePID, String, Para1, Para2, Para3, Para4)          RRM_TASK_ERROR_LOG4(&g_stRrmLogEnt, ModulePID, 0, String, Para1, Para2, Para3, Para4)




/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_UINT32 RRM_MNTN_CreateMutexSem(VOS_UINT32 ulPid);
extern VOS_VOID RRM_MNTN_InitLogEnt(RRM_LOG_ENT_STRU *pstLogEnt);
extern VOS_VOID RRM_MNTN_LogOutput(RRM_LOG_ENT_STRU *pstLogEnt, VOS_UINT32 ulPid);
extern VOS_VOID RRM_MNTN_LogSave
       (
           RRM_LOG_ENT_STRU               *pstLogEnt,
           VOS_UINT32                      ulPid,
           LOG_LEVEL_EN                    enLevel,
           PS_FILE_ID_DEFINE_ENUM_UINT32   enFile,
           VOS_UINT32                      ulLine,
           VOS_INT32                       lpara1,
           VOS_INT32                       lpara2,
           VOS_INT32                       lpara3,
           VOS_INT32                       lpara4
       );

extern VOS_UINT32       OM_TraceMsgHook(VOS_VOID *pMsg);

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

#endif /* end of RrmLog.h */

