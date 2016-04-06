
#ifndef    _BSP_MEM_H_
#define    _BSP_MEM_H_

#include <linux/highmem.h>
#include <bsp_sram.h>
#include <drv_comm.h>
#include <asm/cacheflush.h>
#include <asm/cache.h>
#include <asm/tlbflush.h>
//#include <asm/system.h>
#include <linux/module.h>
#include <bsp_shared_ddr.h>
#include <bsp_om.h>
#include <drv_ipc_enum.h>
#include <bsp_ipc.h>
#include <drv_memory.h>
#include <bsp_memmap.h>
#include "mem_balong_drv.h"
#include <osl_spinlock.h>
#include <bsp_reset.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**************************************************************************
  宏定义
**************************************************************************/
#if defined (VERSION_V3R2)

#else
#define PBXA9_DRAM_SHARE_VIRT            IO_ADDRESS(PBXA9_DRAM_SHARE_PHYS)
#define PBXA9_DRAM_SHARE_SIZE            SZ_1M
#endif

#define VX_PBXA9_DRAM_SHARE_VIRT            IO_ADDRESS(VX_PBXA9_DRAM_SHARE_PHYS)
#define VX_PBXA9_DRAM_SHARE_SIZE            SZ_64M


#define  mem_print_error(fmt,...)      (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_MEM, "[mem]: <%s> <%d>"fmt"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  mem_print_dbg(fmt,...)		       (bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MODU_MEM, "[mem]: <%s> <%d>"fmt"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__))

/**************************************************************************
  函数声明
**************************************************************************/
s32 bsp_mem_init(void);
s32 bsp_mem_ccore_reset_cb(DRV_RESET_CALLCBFUN_MOMENT enParam, int userdata);
s32 bsp_set_most_used_size(u32 u32Size, u32 u32PoolType);

void* bsp_malloc(u32 u32Size, MEM_POOL_TYPE enFlags);
void* bsp_malloc_dbg(u32 u32Size, MEM_POOL_TYPE enFlags, u8* pFileName, u32 u32Line);
void  bsp_free(void* pMem);
void  bsp_free_dbg(void* pMem, u8* pFileName, u32 u32Line);

void* bsp_smalloc(u32 u32Size, MEM_POOL_TYPE enFlags);
void* bsp_smalloc_dbg(u32 u32Size, MEM_POOL_TYPE enFlags, u8* pFileName, u32 u32Line);
void  bsp_sfree(void* pMem);
void  bsp_sfree_dbg(void* pMem, u8* pFileName, u32 u32Line);


/**************************************************************************
  接口声明
**************************************************************************/
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


/**************************************************************************
  接口声明
**************************************************************************/

/*****************************************************************************
* 函 数 名  : BSP_SMALLOC
*
* 功能描述  : BSP 动态内存分配(加spin lock保护,多核场景使用)
*
* 输入参数  : sz: 分配的大小(byte)
*             flags: 内存属性(使用DDR / AXI)
* 输出参数  : 无
* 返 回 值  : 分配出来的内存指针
*****************************************************************************/
#ifdef __BSP_DEBUG__
#define BSP_SMALLOC(sz, flags) bsp_smalloc_dbg(sz, (MEM_POOL_TYPE)flags, (u8*)__FILE__, __LINE__)
#else
#define BSP_SMALLOC(sz, flags) bsp_smalloc(sz, flags)
#endif

/*****************************************************************************
* 函 数 名  : BSP_SFREE
*
* 功能描述  : BSP 动态内存释放(加spin lock保护,多核场景使用)
*
* 输入参数  : ptr: 动态内存指针
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
#ifdef __BSP_DEBUG__
#define BSP_SFREE(ptr) bsp_sfree_dbg(ptr, (u8*)__FILE__, __LINE__)
#else
#define BSP_SFREE(ptr) bsp_sfree(ptr)
#endif



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BSP_MEM_H_ */

