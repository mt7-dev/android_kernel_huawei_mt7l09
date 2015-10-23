
#include <drv_comm.h>
#include <drv_memory.h>
#include <osl_types.h>
#include <stdlib.h>

/*******************以下为打桩函数*********************************/
BSP_VOID* bsp_malloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags)
{
    return malloc(u32Size);
}
BSP_VOID* bsp_malloc_dbg(BSP_U32 u32Size, MEM_POOL_TYPE enFlags, BSP_U8* pFileName, BSP_U32 u32Line)
{
    return malloc(u32Size);
}
BSP_VOID  bsp_free(BSP_VOID* pMem)
{
    free(pMem);
    return;
}
BSP_VOID  bsp_free_dbg(BSP_VOID* pMem, BSP_U8* pFileName, BSP_U32 u32Line)
{
    free(pMem);
    return;
}
BSP_VOID* bsp_smalloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags)
{
    return malloc(u32Size);
}
void* bsp_smalloc_dbg(u32 u32Size, MEM_POOL_TYPE enFlags, u8* pFileName, u32 u32Line)
{
    return malloc(u32Size);

}
BSP_VOID  bsp_sfree(BSP_VOID* pMem)
{
    free(pMem);
    return;
}
void  bsp_sfree_dbg(void* pMem, u8* pFileName, u32 u32Line)
{
    free(pMem);
    return;
}




