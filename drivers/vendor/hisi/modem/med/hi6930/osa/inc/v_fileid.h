/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_fileid.h                                                      */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
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
/* 2. Date: 2006-10                                                          */
/*    Author: Xu Cheng                                                       */
/*    Modification: Standardize code                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef __V_FILE_ID_H__
#define __V_FILE_ID_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum
{
/*  000  */    FILE_ID_V_BLKMEM_C,
/*  001  */    FILE_ID_V_LIB_C,
/*  002  */    FILE_ID_V_MSG_C,
/*  003  */    FILE_ID_V_QUEUE_C,
/*  004  */    FILE_ID_V_TASK_C,
/*  005  */    FILE_ID_V_WIN32_SEM_C,
/*  006  */    FILE_ID_V_WIN32_TASK_C,
/*  007  */    FILE_ID_VOS_ID_C,
/*  008  */    FILE_ID_VOS_MAIN_C,
/*  009  */    FILE_ID_KERNEL_C,
/*  010  */    FILE_ID_V_WIN32_ID_C
}DRV_FILE_ID_DEFINE_ENUM;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __V_FILE_ID_H__ */

