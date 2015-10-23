/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: include/v_iddef.h                                        */
/*                                                                           */
/* Author:                                                                   */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date:                                                                     */
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifndef  _V_IDDEF_H
#define  _V_IDDEF_H


#include "VosPidDef.h"
#include "v_typdef.h"


/************************* MACRO & TYPE DEFINITION ***************************/

#define VOS_LOCAL_CPUID                 VOS_CPU_ID_MEDDSP
#define VOS_PID_DOPRAEND                VOS_PID_CPU_ID_3_DOPRAEND               /* 定义本CPU PID起始编号 */
#define VOS_PID_BUTT                    VOS_CPU_ID_3_PID_BUTT                   /* 定义本CPU PID截至编号 */
#define VOS_PID_DEFAULT                 DSP_PID_VOICE_RESERVED                  /* 默认PID映射到保留PID  */
#define VOS_PidIdx(pid)                 ((pid) - VOS_PID_DOPRAEND)              /* 由PID获取PID数组下标 */
#define VOS_PidCheck(pid)               \
    (((pid) >= VOS_PID_DOPRAEND) && ((pid) < VOS_PID_BUTT))

#define BEGIN_FID_DEFINITION() \
    enum VOS_Function_ID \
    { \
        VOS_FID_RESERVED = VOS_FID_DOPRAEND - 1,

#define END_FID_DEFINITION() \
        VOS_FID_BUTT \
    };

#define DEFINE_FID(fid) fid,

#define END_DECLARE_PID_BELONG_TO_FID() \
    }

#define ON_DECLARE(pid, fid) \
    MOD_RegFidPidRSP(pid , fid);

typedef struct ID_Static_Table_Type
{
    VOS_FID       ulFID;                        /* FID           */
    Init_Fun_Type pfnInitFun;                   /* FID初始化函数 */
    VOS_UINT16    usStackSize;
    VOS_UINT16    usMsgNumber;
} FID_Static_Table_Type;

enum VOS_DOPRA_Function_ID
{
    DOPRA_FID_SYS=0,
    VOS_FID_DOPRAEND                            /* 系统FID定义结束标志 */
};

typedef struct
{
    VOS_INT size;
    VOS_INT number;
}VOS_MEM_BLOCK_INFO;

/* modify this to config memory*/
/* the number of message's control block */
#define VOS_MEM_CTRL_BLOCK_NUMBER                           2

#define BEGIN_DECLARE_PID_BELONG_TO_FID()\
VOS_VOID REG_FID_PID_RSP(VOS_VOID)\
{\


#endif /* _V_IDDEF_H */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

