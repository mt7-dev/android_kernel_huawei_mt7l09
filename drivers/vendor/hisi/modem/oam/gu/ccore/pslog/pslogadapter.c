
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
VOS_UINT32 LOG_RegisterDrv(LOG_PFUN fptr, LOG_PFUN fptr1, LOG_PFUN fptr2,
                                  LOG_PFUN fptr3, LOG_PFUN fptr4)
{
#if((VOS_OS_VER == VOS_VXWORKS)||(VOS_OS_VER == VOS_RTOSCK))
    return (VOS_UINT32)DRV_LOG_INSTALL((VOIDFUNCPTR)fptr, (VOIDFUNCPTR)fptr1,
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

