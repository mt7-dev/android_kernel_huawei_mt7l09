
#ifndef __UCOM_MEM_DYN_H__
#define __UCOM_MEM_DYN_H__

/*****************************************************************************
  0 其他头文件包含
*****************************************************************************/
#include  "vos.h"

#ifdef _UCOM_USE_STD_MEM_LIB
#include <string.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 外部接口定义
*****************************************************************************/

/*****************************************************************************
 函数宏名  : UCOM_MemAlloc
 功能描述  : 申请位于DDR中的动态内存
 参数说明  : uwSize - 申请内存的大小, 单位Byte
 返回结果  : 申请成功 - 返回申请到的地址
             申请失败 - 返回VOS_NULL(0)
*****************************************************************************/
#ifdef _UCOM_USE_STD_MEM_LIB
#define UCOM_MemAlloc(uwSize)           malloc(uwSize)
#else
#define UCOM_MemAlloc(uwSize)           UCOM_MEM_DYN_Malloc((VOS_UINT32)(uwSize),\
                                                            UCOM_MEM_DYN_REQUEST_DDR)
#endif

/*****************************************************************************
 函数宏名  : UCOM_MemAllocLimited
 功能描述  : 申请位于TCM中的动态内存(此内存访问速度比DDR更快,但可供使用的空间有限)
 参数说明  : uwSize - 申请内存的大小, 单位Byte
 返回结果  : 申请成功 - 返回申请到的地址
             申请失败 - 返回VOS_NULL(0)
*****************************************************************************/
#ifdef _UCOM_USE_STD_MEM_LIB
#define UCOM_MemAllocLimited(uwSize)    malloc(uwSize)
#else
#define UCOM_MemAllocLimited(uwSize)    UCOM_MEM_DYN_Malloc((VOS_UINT32)(uwSize),\
                                                            UCOM_MEM_DYN_REQUEST_TCM)
#endif

/*****************************************************************************
 函数宏名  : UCOM_MemAllocFair
 功能描述  : 首先尝试申请TCM中的动态内存,若失败,则申请DDR中的动态内存
 参数说明  : uwSize - 申请内存的大小, 单位Byte
 返回结果  : 申请成功 - 返回申请到的地址
             申请失败 - 返回VOS_NULL(0)
*****************************************************************************/
#ifdef _UCOM_USE_STD_MEM_LIB
#define UCOM_MemAllocFair(uwSize)       malloc(uwSize)
#else
#define UCOM_MemAllocFair(uwSize)       UCOM_MEM_DYN_Malloc((VOS_UINT32)(uwSize),\
                                                            UCOM_MEM_DYN_REQUEST_TCM\
                                                            |UCOM_MEM_DYN_REQUEST_DDR)
#endif

/*****************************************************************************
 函数宏名  : UCOM_MemFree
 功能描述  : 释放申请的内存
 参数说明  : pMem - 需要释放的内存地址
 返回结果  : 释放成功 - 返回VOS_OK
             释放失败 - 返回VOS_ERR
*****************************************************************************/
#ifdef _UCOM_USE_STD_MEM_LIB
#define UCOM_MemFree(pMem)      free((void*)(pMem))
#else
#define UCOM_MemFree(pMem)      UCOM_MEM_DYN_Free((VOS_VOID*)(pMem))
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define UCOM_MEM_DYN_POOL_SIZE_DDR      (512000L)           /* DDR内存大小, 最好是MALLOC_ALIGNMENT的整数倍 */
#define UCOM_MEM_DYN_POOL_SIZE_TCM      (0x4000)            /* TCM内存大小, 最好是MALLOC_ALIGNMENT的整数倍，暂时分配为16KByte，后续再进行调整 */
#define UCOM_MEM_DYN_MALLOC_ALIGNMENT   (sizeof(VOS_UINT32) * 2)/* 分配内存的地址对齐方式 */

#define UCOM_MEM_DYN_NODE_MAX_NUM       (256)               /* 最大分配的内存节点数 */

/*****************************************************************************
 函数宏名  : UCOM_MEM_DYN_GetMemCtrl
 功能描述  : 获取内存控制结构体指针
*****************************************************************************/
#define UCOM_MEM_DYN_GetMemCtrl()       (&g_stUcomMemDynCtrl)

/*****************************************************************************
 函数宏名  : UCOM_MEM_DYN_GetPoolCfgTbl
 功能描述  : 获取内存池配置表
*****************************************************************************/
#define UCOM_MEM_DYN_GetPoolCfgTbl()    (g_auwUcomMemDynPoolCfgTbl)

/*****************************************************************************
 函数宏名  : UCOM_MEM_DYN_GetIdleList
 功能描述  : 获取未使用链表
*****************************************************************************/
#define UCOM_MEM_DYN_GetIdleList(pstCtrl) (&pstCtrl->stIdleList)

/*****************************************************************************
 函数宏名  : UCOM_MEM_DYN_GetFreeList
 功能描述  : 获取待分配链表
*****************************************************************************/
#define UCOM_MEM_DYN_GetFreeList(pstCtrl,enType) (&pstCtrl->astFreeList[enType])

/*****************************************************************************
 函数宏名  : UCOM_MEM_DYN_GetUnFreeList
 功能描述  : 获取已分配链表
*****************************************************************************/
#define UCOM_MEM_DYN_GetUnFreeList(pstCtrl,enType) (&pstCtrl->astUnFreeList[enType])

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : UCOM_MEM_DYN_TYPE_ENUM
 功能描述  : 描述内存存储类型
*****************************************************************************/
enum UCOM_MEM_DYN_TYPE_ENUM
{
    UCOM_MEM_DYN_TYPE_DDR               = 0,                /* 内存分配区域位于DDR */
    UCOM_MEM_DYN_TYPE_TCM,                                  /* 内存分配区域位于TCM */
    UCOM_MEM_DYN_TYPE_BUTT,
};
typedef VOS_UINT32 UCOM_MEM_DYN_TYPE_ENUM_UINT32;

/*****************************************************************************
 实体名称  : UCOM_MEM_DYN_REQUEST_ENUM
 功能描述  : 描述内存分配请求类型
*****************************************************************************/
enum UCOM_MEM_DYN_REQUEST_ENUM
{
    UCOM_MEM_DYN_REQUEST_DDR            = 0x1,              /* 请求内存申请DDR内存 */
    UCOM_MEM_DYN_REQUEST_TCM            = 0x2,              /* 请求内存申请TCM内存 */
    UCOM_MEM_DYN_REQUEST_BUTT           = 0x4,
};
typedef VOS_UINT32 UCOM_MEM_DYN_REQUEST_ENUM_UINT32;


/*****************************************************************************
 实体名称  : UCOM_MEM_DYN_POOL_CFG_ITEM_ENUM
 功能描述  : 描述内存配置表项
*****************************************************************************/
enum UCOM_MEM_DYN_POOL_CFG_ITEM_ENUM
{
    UCOM_MEM_DYN_POOL_CFG_ITEM_ADDR     = 0,                /* 内存起始地址 */
    UCOM_MEM_DYN_POOL_CFG_ITEM_SIZE,                        /* 内存大小 */
    UCOM_MEM_DYN_POOL_CFG_ITEM_BUTT,
};
typedef VOS_UINT32 UCOM_MEM_DYN_POOL_CFG_ITEM_ENUM_UINT32;


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
 实体名称  : UCOM_MEM_DYN_BLK_STRU
 功能描述  : 使用者的内存块信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwAddr;             /* 内存块地址 */
    VOS_UINT32                          uwSize;             /* 内存块大小 */
} UCOM_MEM_DYN_BLK_STRU;

/*****************************************************************************
 实体名称  : UCOM_MEM_DYN_NODE_STRU
 功能描述  : 内存节点信息
*****************************************************************************/
typedef struct _UCOM_MEM_DYN_NODE_STRU
{
    UCOM_MEM_DYN_BLK_STRU               stBlk;              /* 当前节点的数据 */
    struct _UCOM_MEM_DYN_NODE_STRU     *pstNext;            /* 后一个节点 */
    struct _UCOM_MEM_DYN_NODE_STRU     *pstPrev;            /* 前一个节点 */
} UCOM_MEM_DYN_NODE_STRU;

/*****************************************************************************
 实体名称  : UCOM_MEM_DYN_LIST_STRU
 功能描述  : 内存链表信息
*****************************************************************************/
typedef struct
{
    UCOM_MEM_DYN_NODE_STRU              stHead;             /* 链表头 */
    UCOM_MEM_DYN_NODE_STRU              stTail;             /* 链表尾 */
    VOS_UINT32                          uwLen;              /* 链表长度 */
} UCOM_MEM_DYN_LIST_STRU;

/*****************************************************************************
 实体名称  : UCOM_MEM_DYN_CTRL_STRU
 功能描述  : 内存块控制实体
*****************************************************************************/
typedef struct
{
    UCOM_MEM_DYN_LIST_STRU              astFreeList[UCOM_MEM_DYN_TYPE_BUTT];    /* 待分配内存节点 */
    UCOM_MEM_DYN_LIST_STRU              astUnFreeList[UCOM_MEM_DYN_TYPE_BUTT];  /* 已分配内存节点 */
    UCOM_MEM_DYN_LIST_STRU              stIdleList;                             /* 未使用的节点链表 */
    UCOM_MEM_DYN_NODE_STRU              astNodes[UCOM_MEM_DYN_NODE_MAX_NUM];    /* 保存所有节点信息(节点未使用动态分配) */
} UCOM_MEM_DYN_CTRL_STRU;


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID UCOM_MEM_DYN_AddNode(
                       UCOM_MEM_DYN_NODE_STRU      *pstNode,
                       UCOM_MEM_DYN_LIST_STRU      *pstList );
extern VOS_UINT32  UCOM_MEM_DYN_AllocByType(
                       VOS_UINT32                          uwSize,
                       UCOM_MEM_DYN_TYPE_ENUM_UINT32       enType);
extern VOS_VOID UCOM_MEM_DYN_DelNode(
                       UCOM_MEM_DYN_NODE_STRU      *pstNode,
                       UCOM_MEM_DYN_LIST_STRU      *pstList );
extern VOS_UINT32 UCOM_MEM_DYN_Free(VOS_VOID *pMem);
extern VOS_UINT32  UCOM_MEM_DYN_FreeByType(
                       VOS_UINT32                          uwAddr,
                       UCOM_MEM_DYN_TYPE_ENUM_UINT32       enType);
extern UCOM_MEM_DYN_NODE_STRU* UCOM_MEM_DYN_GetIdleNode( VOS_VOID );
extern VOS_VOID UCOM_MEM_DYN_Init( VOS_VOID );
extern  VOS_VOID UCOM_MEM_DYN_InitList(UCOM_MEM_DYN_LIST_STRU *pstList);
extern VOS_VOID UCOM_MEM_DYN_InsertNode(
                       UCOM_MEM_DYN_NODE_STRU      *pstNode,
                       UCOM_MEM_DYN_LIST_STRU      *pstList );
extern VOS_VOID* UCOM_MEM_DYN_Malloc(
                       VOS_UINT32                          uwSize,
                       UCOM_MEM_DYN_REQUEST_ENUM_UINT32    enReq);
extern UCOM_MEM_DYN_NODE_STRU*  UCOM_MEM_DYN_SeekNodeByAddr(
                       VOS_UINT32                          uwAddr,
                       UCOM_MEM_DYN_LIST_STRU             *pstList );
extern UCOM_MEM_DYN_NODE_STRU*  UCOM_MEM_DYN_SeekNodeBySize(
                       VOS_UINT32                          uwSize,
                       UCOM_MEM_DYN_LIST_STRU             *pstList,
                       VOS_UINT32                         *puwBestFit);
extern VOS_VOID UCOM_MEM_DYN_SetIdleNode(UCOM_MEM_DYN_NODE_STRU *pstNode);











#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of ucom_mem_dyn.h */
