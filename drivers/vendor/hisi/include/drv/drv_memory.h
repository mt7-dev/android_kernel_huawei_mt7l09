/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_MEMORY_H__
#define __DRV_MEMORY_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "drv_comm.h"

/*************************MEMORY START********************************/

/**************************************************************************
  宏定义
**************************************************************************/
/* 内存池类型, Flags标记用 */
typedef enum tagMEM_POOL_TYPE
{
    MEM_NORM_DDR_POOL = 0,
    MEM_ICC_DDR_POOL,
    MEM_ICC_AXI_POOL,
    MEM_POOL_MAX
}MEM_POOL_TYPE;

/**************************************************************************
  接口声明
**************************************************************************/
BSP_VOID* bsp_malloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags);
BSP_VOID* bsp_malloc_dbg(BSP_U32 u32Size, MEM_POOL_TYPE enFlags, BSP_U8* pFileName, BSP_U32 u32Line);
BSP_VOID  bsp_free(BSP_VOID* pMem);
BSP_VOID  bsp_free_dbg(BSP_VOID* pMem, BSP_U8* pFileName, BSP_U32 u32Line);

/*****************************************************************************
* 函 数 名  : BSP_MALLOC
*
* 功能描述  : BSP 动态内存分配
*
* 输入参数  : sz: 分配的大小(byte)
*             flags: 内存属性(暂不使用,预留)
* 输出参数  : 无
* 返 回 值  : 分配出来的内存指针
*****************************************************************************/
#ifdef __BSP_DEBUG__
#define BSP_MALLOC(sz, flags) bsp_malloc_dbg(sz, flags, __FILE__, __LINE__)
#else
#define BSP_MALLOC(sz, flags) bsp_malloc(sz, flags)
#endif

/*****************************************************************************
* 函 数 名  : BSP_FREE
*
* 功能描述  : BSP 动态内存释放
*
* 输入参数  : ptr: 动态内存指针
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
#ifdef __BSP_DEBUG__
#define BSP_FREE(ptr) bsp_free_dbg(ptr, __FILE__, __LINE__)
#else
#define BSP_FREE(ptr) bsp_free(ptr)
#endif

/*****************************************************************************
* 函 数 名  : BSP_SFree
*
* 功能描述  : BSP 动态内存释放(加spin lock保护,多核场景使用)
*
* 输入参数  : pMem: 动态内存指针
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID  bsp_sfree(BSP_VOID* pMem);
#define   BSP_SFree(pMem)       bsp_sfree(pMem)
/*****************************************************************************
* 函 数 名  : BSP_SMalloc
*
* 功能描述  : BSP 动态内存分配(加spin lock保护,多核场景使用)
*
* 输入参数  : u32Size: 分配的大小(byte)
*             enFlags: 内存属性(暂不使用,预留)
* 输出参数  : 无
* 返 回 值  : 分配出来的内存指针
*****************************************************************************/
BSP_VOID* bsp_smalloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags);
#define  BSP_SMalloc(u32Size, enFlags)  bsp_smalloc(u32Size, enFlags)

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern BSP_VOID* cacheDmaMalloc(BSP_U32  bytes);
#define DRV_CACHEDMAM_ALLOC(bytes) cacheDmaMalloc(bytes)
#else
#define DRV_CACHEDMAM_ALLOC(bytes) kmalloc(bytes, GFP_KERNEL)
#endif

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern BSP_S32 cacheDmaFree(BSP_VOID*  pBuf);
#define DRV_CACHEDMAM_FREE(pBuf) cacheDmaFree(pBuf)
#else
#define DRV_CACHEDMAM_FREE(pBuf) kfree(pBuf)
#endif

/************************************************************************
 * FUNCTION
 *       vmEnable
 * DESCRIPTION
 *       内存读写保护功能使能函数
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern int vmEnable(int enable);
#define DRV_VM_ENABLE(flag)   vmEnable(flag)

/*****************************************************************************
 函 数 名  : vmStateSet
 功能描述  : 设置地址空间状态
 输入参数  : 无

 输出参数  : 无。
 返 回 值  : 无
 注意事项  ：
*****************************************************************************/
#define DRV_VM_STATESET(context, virtAdrs, len, stateMask, state) vmStateSet(context, virtAdrs, len, stateMask, state)

/*****************************************************************************
 函 数 名  : BSP_CACHE_DATA_FLUSH
 功能描述  :
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern void BSP_CACHE_DATA_FLUSH(void * addr, int size);

/*************************MEMORY END**********************************/


/*************************内存分配 START******************************/

/* DDR中各个内存段的类型 */
typedef enum tagBSP_DDR_SECT_TYPE_E
{
    BSP_DDR_SECT_TYPE_TTF = 0x0,
    BSP_DDR_SECT_TYPE_ARMDSP,
    BSP_DDR_SECT_TYPE_UPA,
    BSP_DDR_SECT_TYPE_CQI,
    BSP_DDR_SECT_TYPE_APT,
    BSP_DDR_SECT_TYPE_ET,
    BSP_DDR_SECT_TYPE_BBPMASTER,
    BSP_DDR_SECT_TYPE_NV,
    BSP_DDR_SECT_TYPE_DICC,
    BSP_DDR_SECT_TYPE_WAN,
    BSP_DDR_SECT_TYPE_SHARE_MEM,
    BSP_DDR_SECT_TYPE_EXCP,
    BSP_DDR_SECT_TYPE_HIFI,
    BSP_DDR_SECT_TYPE_ZSP_UP,
    BSP_DDR_SECT_TYPE_BBPSAMPLE,
    BSP_DDR_SECT_TYPE_HARQ,
    BSP_DDR_SECT_TYPE_BUTTOM
}BSP_DDR_SECT_TYPE_E;


/* AXI中各个内存段的类型 */
typedef enum tagBSP_AXI_SECT_TYPE_E
{
    BSP_AXI_SECT_TYPE_TEMPERATURE,
    BSP_AXI_SECT_TYPE_DICC,
    BSP_AXI_SECT_TYPE_HIFI,
    BSP_AXI_SECT_TYPE_TTF_BBP,
    BSP_AXI_SECT_TYPE_BUTTOM
}BSP_AXI_SECT_TYPE_E;


/* 内存段属性 */
typedef enum tagBSP_DDR_SECT_ATTR_E
{
    BSP_DDR_SECT_ATTR_CACHEABLE = 0x0,
    BSP_DDR_SECT_ATTR_NONCACHEABLE,
    BSP_DDR_SECT_ATTR_BUTTOM
}BSP_DDR_SECT_ATTR_E;


/* 定义虚实地址是否相同的枚举 */
typedef enum tagBSP_DDR_SECT_PVADDR_E
{
    BSP_DDR_SECT_PVADDR_EQU = 0x0,
    BSP_DDR_SECT_PVADDR_NOT_EQU,
    BSP_DDR_SECT_PVADDR_BUTTOM
}BSP_DDR_SECT_PVADDR_E;


/* DDR内存段的查询结构 */
typedef struct tagBSP_DDR_SECT_QUERY
{
    BSP_DDR_SECT_TYPE_E     enSectType;
    BSP_DDR_SECT_ATTR_E     enSectAttr;
    BSP_DDR_SECT_PVADDR_E   enPVAddr;
    BSP_U32                 ulSectSize;
}BSP_DDR_SECT_QUERY;


/* DDR内存段的详细信息 */
typedef struct tagBSP_DDR_SECT_INFO
{
    BSP_DDR_SECT_TYPE_E    enSectType;
    BSP_DDR_SECT_ATTR_E    enSectAttr;
    BSP_VOID*               pSectVirtAddr;
    BSP_VOID*                pSectPhysAddr;
    BSP_U32                ulSectSize;
}BSP_DDR_SECT_INFO;


/* AXI内存段的详细信息 */
typedef struct tagBSP_AXI_SECT_INFO
{
    BSP_AXI_SECT_TYPE_E    enSectType;
    BSP_VOID*                pSectVirtAddr;
    BSP_VOID*                pSectPhysAddr;
    BSP_U32                ulSectSize;
}BSP_AXI_SECT_INFO;

/*****************************************************************************
 函 数 名  : BSP_DDR_GetSectInfo
 功能描述  : DDR内存段查询接口
 输入参数  : pstSectQuery: 需要查询的内存段类型、属性
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_DDR_GetSectInfo(BSP_DDR_SECT_QUERY *pstSectQuery, BSP_DDR_SECT_INFO *pstSectInfo);
#define DRV_GET_FIX_DDR_ADDR(pstSectQuery, pstSectInfo) BSP_DDR_GetSectInfo(pstSectQuery, pstSectInfo)

/*****************************************************************************
 函 数 名  : BSP_AXI_GetSectInfo
 功能描述  : AXI内存段查询接口
 输入参数  : enSectType: 需要查询的内存段类型
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_AXI_GetSectInfo(BSP_AXI_SECT_TYPE_E enSectType, BSP_AXI_SECT_INFO *pstSectInfo);
#define DRV_GET_FIX_AXI_ADDR(enSectType, pstSectInfo) BSP_AXI_GetSectInfo(enSectType, pstSectInfo)

/*************************内存分配 END********************************/


/*************************虚实地址转换 START**************************/

/*****************************************************************************
 函 数 名  : DRV_DDR_VIRT_TO_PHY
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
void* DRV_DDR_VIRT_TO_PHY(void* ulVAddr);

/*****************************************************************************
 函 数 名  : DRV_DDR_PHY_TO_VIRT
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
void* DRV_DDR_PHY_TO_VIRT(void* ulPAddr);

/*****************************************************************************
 函 数 名  : TTF_VIRT_TO_PHY
 功能描述  : TTF内存虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
void* TTF_VIRT_TO_PHY(void* ulVAddr);

/*****************************************************************************
 函 数 名  : TTF_PHY_TO_VIRT
 功能描述  : TTF内存虚地址往实地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
void* TTF_PHY_TO_VIRT(void* ulPAddr);

/*****************************************************************************
 函 数 名  : IPF_VIRT_TO_PHY
 功能描述  : IPF寄存器虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
void* IPF_VIRT_TO_PHY(void* ulVAddr);

/*****************************************************************************
 函 数 名  : IPF_PHY_TO_VIRT
 功能描述  : IPF寄存器虚地址往实地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
void* IPF_PHY_TO_VIRT(void* ulPAddr);
/*****************************************************************************
 函 数 名  : DRV_AXI_VIRT_TO_PHY
 功能描述  : AXI内虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
void* DRV_AXI_VIRT_TO_PHY(void* ulVAddr);

/*****************************************************************************
 函 数 名  : DRV_AXI_PHY_TO_VIRT
 功能描述  : AXI内实地址往虚地址转换
 输入参数  : ulVAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
void* DRV_AXI_PHY_TO_VIRT(void* ulPAddr);




/*************************虚实地址转换 END****************************/


/*****************************************************************************
* 函 数 名  : BSP_IPM_FreeBspBuf
*
* 功能描述  : 释放内存接口
*
* 输入参数  : BSP_U8 *pBuf 需要释放的指针
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月27日   鲁婷  创建
*****************************************************************************/
BSP_VOID BSP_IPM_FreeBspBuf(BSP_U8 *pBuf);

BSP_VOID DRV_CACHE_SYNC(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

