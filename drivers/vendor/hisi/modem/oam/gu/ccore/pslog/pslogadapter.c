
/******************************************************************************

                版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : PsLogAdapter.c
  版 本 号   : 初稿
  作    者   : 甘兰 47350
  生成日期   : 2008年7月16日
  最近修改   :
  功能描述   : PsLogAdapter的文件
  函数列表   :
  修改历史   :
  1.日    期   : 2008年7月16日
    作    者   : 甘兰 47350
    修改内容   : 创建文件

*****************************************************************************/
#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

#include "pslogadapter.h"
#include "om.h"

/*lint -e718*/
/*lint -e746*/
/*lint -e40*/
/*lint -e64*/
/*lint -e63*/
/*****************************************************************************
 函 数 名  : LOG_CreateMutex
 功能描述  : 创建计数型信号量
 输入参数  : pLogSem
 输出参数  : 无
 返 回 值  : LOG_ERR - 操作失败
             LOG_OK  - 操作成功


 修改历史      :
  1.日    期   : 2008年7月17日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
LOG_ULONG LOG_CreateMutex(LOG_SEM *pLogSem)
{
#if(VOS_OS_VER == VOS_VXWORKS)
    *pLogSem = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);

    if (LOG_NULL_PTR == *pLogSem)
    {
        return LOG_ERR;
    }
#elif(VOS_OS_VER == VOS_RTOSCK)
    if (SRE_OK != SRE_SemBCreate(OS_SEM_FULL, pLogSem, SEM_MODE_PRIOR))
    {
        return LOG_ERR;
    }
#endif
    return LOG_OK;
}
/*****************************************************************************
 函 数 名  : LOG_CreateSemaphore
 功能描述  : 创建计数型信号量
 输入参数  : pLogSem
 输出参数  : 无
 返 回 值  : LOG_ERR - 操作失败
             LOG_OK  - 操作成功


 修改历史      :
  1.日    期   : 2008年7月17日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
LOG_ULONG LOG_CreateSemaphore(LOG_SEM *pLogSem)
{
#if(VOS_OS_VER == VOS_VXWORKS)
    *pLogSem = semCCreate(SEM_Q_FIFO, SEM_EMPTY);

    if (LOG_NULL_PTR == *pLogSem)
    {
        return LOG_ERR;
    }
#elif(VOS_OS_VER == VOS_RTOSCK)
    if (SRE_OK != SRE_SemCCreate(OS_SEM_EMPTY, pLogSem, SEM_MODE_FIFO))
    {
        return LOG_ERR;
    }
#endif

    return LOG_OK;
}
/*****************************************************************************
 函 数 名  : LOG_DeleteSemaphore
 功能描述  : 删除计数型信号量
 输入参数  : pLogSem
 输出参数  : 无
 返 回 值  : LOG_VOID

 修改历史      :
  1.日    期   : 2008年7月17日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
LOG_VOID LOG_DeleteSemaphore(LOG_SEM *pLogSem)
{
#if(VOS_OS_VER == VOS_VXWORKS)
    semDelete(*pLogSem);
#elif(VOS_OS_VER == VOS_RTOSCK)
    SRE_SemDelete(*pLogSem);
#endif
}
/*****************************************************************************
 函 数 名  : Log_SmV
 功能描述  : 对信号量进行V操作
 输入参数  : pLogSem
 输出参数  : 无
 返 回 值  : LOG_ERR - 操作失败
             LOG_OK  - 操作成功


 修改历史      :
  1.日    期   : 2008年7月17日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
LOG_ULONG Log_SmV(LOG_SEM *pLogSem)
{
#if(VOS_OS_VER == VOS_VXWORKS)
    if (LOG_OK != semGive(*pLogSem))
    {
        return LOG_ERR;
    }
#elif(VOS_OS_VER == VOS_RTOSCK)
    if (SRE_OK != SRE_SemPost(*pLogSem))
    {
        return LOG_ERR;
    }
#endif
    return LOG_OK;
}
/*****************************************************************************
 函 数 名  : Log_SmV
 功能描述  : 对信号量进行P操作
 输入参数  : pLogSem
 输出参数  : 无
 返 回 值  : LOG_ERR - 操作失败
             LOG_OK  - 操作成功

 修改历史      :
  1.日    期   : 2008年7月17日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
LOG_ULONG Log_SmP(LOG_SEM *pLogSem)
{
#if(VOS_OS_VER == VOS_VXWORKS)
    if (LOG_OK != semTake(*pLogSem, WAIT_FOREVER))
    {
        return LOG_ERR;
    }
#elif(VOS_OS_VER == VOS_RTOSCK)
    if (SRE_OK != SRE_SemPend (*pLogSem, WAIT_FOREVER))
    {
        return LOG_ERR;
    }
#endif

    return LOG_OK;
}

/*****************************************************************************
 函 数 名  : LOG_RegisterDrv
 功能描述  : LOG输出接口，仅在非VxWorks平台有效
 输入参数  : fptr      - 指向打印LOG信息的函数指针
 输出参数  : 无
 返 回 值  : LOG_ERR - 操作失败
             LOG_OK  - 操作成功

 修改历史      :
  1.日    期   : 2008年7月17日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
LOG_ULONG LOG_RegisterDrv(LOG_PFUN fptr, LOG_PFUN fptr1, LOG_PFUN fptr2,
                                  LOG_PFUN fptr3, LOG_PFUN fptr4)
{
#if((VOS_OS_VER == VOS_VXWORKS)||(VOS_OS_VER == VOS_RTOSCK))
    return (LOG_ULONG)DRV_LOG_INSTALL((VOIDFUNCPTR)fptr, (VOIDFUNCPTR)fptr1,
             (VOIDFUNCPTR)fptr2, (VOIDFUNCPTR)fptr3, (VOIDFUNCPTR)fptr4);
#else
    return LOG_OK;
#endif
}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

