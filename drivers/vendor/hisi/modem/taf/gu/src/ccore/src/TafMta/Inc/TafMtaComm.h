/******************************************************************************

                  版权所有 (C), 2001-2012, 华为技术有限公司

 ******************************************************************************
  文 件 名   : TafMtaComm.h
  版 本 号   : 初稿
  作    者   : 闫志吉
  生成日期   : 2012年6月27日
  最近修改   :
  功能描述   : MTA模块公共函数声明
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月27日
    作    者   : Y00213812
    修改内容   : A-GPS项目新增

******************************************************************************/

#ifndef __TAFMTACOMM_H__
#define __TAFMTACOMM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "pslog.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#if (VOS_WIN32 == VOS_OS_VER)
#define   MTA_WARNING_LOG(string)                vos_printf("%s \r\n", string)
#define   MTA_WARNING1_LOG(string, para1)        vos_printf("%s %d.\r\n", string, para1)
#define   MTA_WARNING2_LOG(string, para1, para2) vos_printf("%s %d, %d.\r\n", string, para1, para2)

#define   MTA_ERROR_LOG(string)                  vos_printf("%s \r\n", string)
#define   MTA_ERROR1_LOG(string, para1)          vos_printf("%s %d.\r\n", string, para1)
#define   MTA_ERROR2_LOG(string, para1, para2)   vos_printf("%s %d, %d.\r\n", string, para1, para2)
#else
#define   MTA_WARNING_LOG(string)                PS_LOG(UEPS_PID_MTA, 0, PS_PRINT_WARNING,  string)
#define   MTA_WARNING1_LOG(string, para1)        PS_LOG1(UEPS_PID_MTA, 0, PS_PRINT_WARNING,  string, (VOS_INT32)para1)
#define   MTA_WARNING2_LOG(string, para1, para2) PS_LOG2(UEPS_PID_MTA, 0, PS_PRINT_WARNING,  string, (VOS_INT32)para1, (VOS_INT32)para2)

#define   MTA_ERROR_LOG(string)                  PS_LOG(UEPS_PID_MTA, 0, PS_PRINT_ERROR,  string)
#define   MTA_ERROR1_LOG(string, para1)          PS_LOG1(UEPS_PID_MTA, 0, PS_PRINT_ERROR,  string, (VOS_INT32)para1)
#define   MTA_ERROR2_LOG(string, para1, para2)   PS_LOG2(UEPS_PID_MTA, 0, PS_PRINT_ERROR,  string, (VOS_INT32)para1, (VOS_INT32)para2)
#endif

/* 字符串转化为数值时所采用的宏 */
#define   MTA_UNSIGNED_LONG_FORMER_NINE_DIGIT    (429496729)      /* VOS_UINT32类型最大值4294967295的前9位*/
#define   MTA_UNSIGNED_SHORT_FORMER_FOUR_DIGIT   (6553)           /* VOS_UINT16类型最大值65535的前4位*/
#define   MTA_UNSIGNED_CHAR_FORMER_TWO_DIGIT     (25)             /* VOS_UINT8类型最大值255的前2位*/
#define   MTA_SIGNED_LONG_FORMER_NINE_DIGIT      (214748364)      /* VOS_INT32类型最大值2147483647的前9位*/

#define   MTA_UNSIGNED_LONG_LAST_DIGIT           '5'              /* VOS_UINT32类型最大值4294967295的最后1位*/
#define   MTA_UNSIGNED_SHORT_LAST_DIGIT          '5'              /* VOS_UINT16类型最大值65535的最后1位*/
#define   MTA_UNSIGNED_CHAR_LAST_DIGIT           '5'              /* VOS_UINT8类型最大值255的最后1位*/
#define   MTA_SIGNED_LONG_POSITIVE_LAST_DIGIT    '7'              /* VOS_INT32类型正数最大值2147483647的最后1位*/
#define   MTA_SIGNED_LONG_NEGATIVE_LAST_DIGIT    '8'              /* VOS_INT32类型负数最小值-2147483648的最后1位*/

#define   MTA_POSITIVE                           (1)              /* 正数 */
#define   MTA_NEGATIVE                           (-1)             /* 负数 */
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
  8 UNION定义
*****************************************************************************/
/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 TAF_MTA_Ac2ul(
    VOS_CHAR                           *pcStr,
    VOS_UINT32                          usLen,
    VOS_UINT32                         *pulRtn);
extern VOS_UINT32 TAF_MTA_Ac2us(
    VOS_CHAR                           *pcStr,
    VOS_UINT32                          ulLen,
    VOS_UINT16                         *pusRtn);
extern VOS_UINT32 TAF_MTA_Ac2uc(
    VOS_CHAR                           *pcStr,
    VOS_UINT32                          ulLen,
    VOS_UINT8                          *pucRtn);
extern VOS_UINT32 TAF_MTA_Ac2sl(
    VOS_CHAR                           *pcStr,
    VOS_UINT32                          usLen,
    VOS_INT32                          *plRtn);

extern VOS_UINT32 TAF_MTA_AcNums2DecNums(
    VOS_UINT8                          *pucAsciiNum,
    VOS_UINT8                          *pucDecNum,
    VOS_UINT32                          ulLen
);

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif


