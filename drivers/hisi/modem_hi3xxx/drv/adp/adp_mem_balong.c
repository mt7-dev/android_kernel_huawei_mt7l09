

#ifndef CONFIG_MEM_BALONG

#include <drv_comm.h>
#include <drv_memory.h>
#include <linux/slab.h>
#include <osl_types.h>

/*******************以下为打桩函数*********************************/
BSP_VOID* bsp_malloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags)
{
    return kmalloc(u32Size, GFP_KERNEL);
}
BSP_VOID* bsp_malloc_dbg(BSP_U32 u32Size, MEM_POOL_TYPE enFlags, BSP_U8* pFileName, BSP_U32 u32Line)
{
    return kmalloc(u32Size, GFP_KERNEL);
}
BSP_VOID  bsp_free(BSP_VOID* pMem)
{
    kfree(pMem);
    return;
}
BSP_VOID  bsp_free_dbg(BSP_VOID* pMem, BSP_U8* pFileName, BSP_U32 u32Line)
{
    kfree(pMem);
    return;
}
BSP_VOID* bsp_smalloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags)
{
    return kmalloc(u32Size, GFP_KERNEL);
}
void* bsp_smalloc_dbg(u32 u32Size, MEM_POOL_TYPE enFlags, u8* pFileName, u32 u32Line)
{
    return kmalloc(u32Size, GFP_KERNEL);
}
BSP_VOID  bsp_sfree(BSP_VOID* pMem)
{
    kfree(pMem);
    return;
}
void  bsp_sfree_dbg(void* pMem, u8* pFileName, u32 u32Line)
{
    kfree(pMem);
    return;
}
#endif

