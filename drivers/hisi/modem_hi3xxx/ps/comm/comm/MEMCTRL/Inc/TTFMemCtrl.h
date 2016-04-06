

#ifndef __TTF_MEMCTRL_H__
#define __TTF_MEMCTRL_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "TtfMemoryMap.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* TTF内存类型初始化标记 */
#define TTF_MEMCTRL_TYPE_INIT_FLG                       (1)

/* TTF内存类型存在不存在标记 */
#define TTF_MEMCTRL_TYPE_EXIST_FLG                      (1)

/* TTF内存类型可见不可见标记 */
#define TTF_MEMCTRL_TYPE_VISIBLE_FLG                    (1<<1)

#define TTF_MEMCTRL_INVAILD_SIZE                        (0xFFFFFFFF)

#define TTF_MEMCTRL_SET_ADDR_INIT_FLG(flg)              (g_stTtfMemCtrlAddr.stTtfMemCtrlAddrComm.ucInitFlg = flg)

#define TTF_MEMCTRL_GET_ADDR_INIT_FLG()                 (g_stTtfMemCtrlAddr.stTtfMemCtrlAddrComm.ucInitFlg)

#define TTF_MEMCTRL_SET_TYPE_FLG(type, flg)             (g_stTtfMemCtrlAddr.astTtfMemCtrlAddrType[type].ulAttribute |= flg)

#define TTF_MEMCTRL_GET_TYPE_ATTRIBUTE(type)            (g_stTtfMemCtrlAddr.astTtfMemCtrlAddrType[type].ulAttribute)

#define TTF_MEMCTRL_SET_TYPE_LEN(type, len)             (g_stTtfMemCtrlAddr.astTtfMemCtrlAddrType[type].ulLen = len)

#define TTF_MEMCTRL_GET_TYPE_LEN(type)                  (g_stTtfMemCtrlAddr.astTtfMemCtrlAddrType[type].ulLen)

#define TTF_MEMCTRL_SET_TYPE_OFFSET_BASE_LEN(type, len) (g_stTtfMemCtrlAddr.astTtfMemCtrlAddrType[type].ulOffsetBaseLen = len)

#define TTF_MEMCTRL_GET_TYPE_OFFSET_BASE_LEN(type)      (g_stTtfMemCtrlAddr.astTtfMemCtrlAddrType[type].ulOffsetBaseLen)

/*****************************************************************************
  3 枚举定义
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
结构名    : TTF_MEMCTRL_ADDR_COMM_STRU
协议表格  : 无
ASN.1描述 : 无
结构说明  : TTF内存地址公共结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucInitFlg;          /* 初始化标记 */
    VOS_UINT8                           ucRcv[3];           /* 保留位 */
} TTF_MEMCTRL_ADDR_COMM_STRU;


/*****************************************************************************
结构名    : TTF_MEMCTRL_ADDR_TYPE_STRU
协议表格  : 无
ASN.1描述 : 无
结构说明  : TTF内存地址类型结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulAttribute;        /* TTF内存地址类型属性 */
    VOS_UINT32                          ulLen;              /* TTF内存地址类型占用的长度 */
    VOS_UINT32                          ulOffsetBaseLen;    /* TTF内存地址类型基于基地址偏移的长度 */
} TTF_MEMCTRL_ADDR_TYPE_STRU;


/*****************************************************************************
结构名    : TTF_MEMCTRL_ADDR_TYPE_STRU
协议表格  : 无
ASN.1描述 : 无
结构说明  : TTF内存地址类型结构
*****************************************************************************/
typedef struct
{
    TTF_MEMCTRL_ADDR_COMM_STRU          stTtfMemCtrlAddrComm;
    TTF_MEMCTRL_ADDR_TYPE_STRU          astTtfMemCtrlAddrType[TTF_MEMCTRL_ADDR_TYPE_BUTT];
} TTF_MEMCTRL_ADDR_STRU;


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 OTHERS定义
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



#pragma pack(0)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of Fc.h */


