
#ifndef    _BSP_MEM_DRV_H_
#define    _BSP_MEM_DRV_H_

#include <linux/slab.h>
#include "bsp_sram.h"
#include "bsp_shared_ddr.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* ASIC2 调试 */

#ifndef INLINE
#define INLINE /*__inline__*/
#endif

/**************************************************************************
  宏定义
**************************************************************************/
/* 主核定义 */
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#define BSP_MEM_MAIN_PART
#endif

/* 长度定义 */
/* 长度定义 */
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#define MEM_NORM_DDR_POOL_SIZE      (512*1024)
#else
#define MEM_NORM_DDR_POOL_SIZE      (4096)/*2^n*4K*/
#endif
#define MEM_ICC_DDR_POOL_SIZE       (SHM_MEM_MEMMGR_SIZE)
#define MEM_ICC_AXI_POOL_SIZE       (SRAM_DYNAMIC_SEC_SIZE)

/* Mem 全局变量放在AXI中 */
#define MEM_CTX_ADDR                (SHM_MEMMGR_FLAG_ADDR)


/* 基地址定义 */
#define _ALIGN(addr, size) (((addr) + (size) - 1) & (~((size) - 1)))
#define MEM_NORM_DDR_POOL_BASE_ADDR (__get_free_pages(GFP_KERNEL, 0))//(kmalloc( _ALIGN(MEM_NORM_DDR_POOL_SIZE, 32), GFP_KERNEL))
#define MEM_ICC_AXI_POOL_BASE_ADDR  (SRAM_DYNAMIC_SEC_ADDR)
#define MEM_ICC_DDR_POOL_BASE_ADDR  (SHM_MEM_MEMMGR_ADDR)


/* 使用ACP模式不需要刷 Cache */
//#define MEM_USE_ACP

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _BSP_MEM_DRV_H_ */



