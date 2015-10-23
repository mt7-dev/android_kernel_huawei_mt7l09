

#include "drv_dump.h"
#include "bsp_dump.h"
#include "bsp_om.h"

#ifdef __cplusplus
extern "C"
{
#endif

u32 g_ExcOmSaveFileFlag[MNTN_TYPE_MAX] = {0};


BSP_VOID DRV_SYSTEM_ERROR(BSP_S32 modId, BSP_S32 arg1, BSP_S32 arg2, BSP_CHAR* arg3, BSP_S32 arg3Length)
{
    return system_error(modId, arg1, arg2, arg3, arg3Length);
}

BSP_S32 DRV_SAVE_REGISTER(BSP_S32 funcType, OM_SAVE_FUNC *pFunc)
{
    return BSP_OK;
}

BSP_U32  DRV_EXCH_MEM_MALLOC(BSP_U32 ulSize)
{
    char *buffer = NULL;
    u32 length = 0;

    if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_OSA_COMM, &buffer, &length))
    {
        return (BSP_U32)NULL;
    }
    else
    {
        return (BSP_U32)((ulSize > length) ? (NULL) : (buffer));
    }
}

BSP_S32 DRV_EXCH_FILE_SAVE(BSP_VOID *address, BSP_U32 length, BSP_U8 IsFileEnd, BSP_U8 type)
{
    dump_save_file_t file_t; /* [false alarm]:ÆÁ±ÎFortify´íÎó */
    dump_save_file_mode_t file_mode; /* [false alarm]:ÆÁ±ÎFortify´íÎó */
    BSP_S32 ret;

    if((!address) || (!length) || (type >= MNTN_TYPE_MAX))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DUMP, "DRV_EXCH_FILE_SAVE param is invalid!\n");
        return BSP_ERROR;
    }

    if(MNTN_TYPE_HIFI == type)
    {
        file_t = DUMP_SAVE_FILE_HIFI; /* [false alarm]:ÆÁ±ÎFortify´íÎó */
    }
    else
    {
        file_t = DUMP_SAVE_FILE_ZSP; /* [false alarm]:ÆÁ±ÎFortify´íÎó */
    }

    if(0 == g_ExcOmSaveFileFlag[type])
    {
        file_mode = DUMP_SAVE_FILE_MODE_CREATE; /* [false alarm]:ÆÁ±ÎFortify´íÎó */
    }
    else
    {
        file_mode = DUMP_SAVE_FILE_MODE_APPEND; /* [false alarm]:ÆÁ±ÎFortify´íÎó */
    }

    ret = bsp_dump_save_file(file_t, address, length, file_mode);
    if(BSP_OK == ret)
    {
        if(IsFileEnd) /* [false alarm]:ÆÁ±ÎFortify´íÎó */
        {
            g_ExcOmSaveFileFlag[type] = 0;
        }
        else
        {
            g_ExcOmSaveFileFlag[type] = 1;
        }
    }
    else
    {
        if(IsFileEnd)
        {
            g_ExcOmSaveFileFlag[type] = 0;
        }
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DUMP, "DRV_EXCH_FILE_SAVE save file failed ret = 0x%x!\n", ret);
    }

    return ret;
}

BSP_VOID DRV_EXCH_HOOK_FUNC_ADD(BSP_VOID)
{
    return;
}

BSP_VOID DRV_EXCH_HOOK_FUNC_DELETE(BSP_VOID)
{
    bsp_dump_trace_stop();
    return;
}

BSP_VOID DRV_STACK_HIGH_RECORD(BSP_VOID)
{
    return;
}

BSP_VOID DRV_VICINT_HOOK_ADD(pFUNCPTR p_Func)
{
    return;
}

BSP_VOID DRV_VICINT_HOOK_DEL(BSP_VOID)
{
    return;
}

BSP_VOID DRV_VICINT_IN_HOOK_ADD(pFUNCPTR p_Func)
{
    return;
}

BSP_VOID DRV_VICINT_IN_HOOK_DEL(BSP_VOID)
{
    return;
}

BSP_VOID DRV_VICINT_OUT_HOOK_ADD(pFUNCPTR p_Func)
{
    return;
}

BSP_VOID DRV_VICINT_OUT_HOOK_DEL(BSP_VOID)
{
    return;
}

BSP_VOID DRV_VICINT_LVLCHG_HOOK_ADD(pFUNCPTR p_Func)
{
    return;
}

BSP_VOID DRV_VICINT_LVLCHG_HOOK_DEL(BSP_VOID)
{
    return;
}

/*********************************** V7R2 ÐÂÔö **********************************/
BSP_S32 DRV_DUMP_SAVE_REGISTER_HOOK(DUMP_SAVE_MOD_ENUM ModId, DUMP_SAVE_HOOK pFunc)
{
    return bsp_dump_register_hook(ModId, pFunc);
}

BSP_S32 DRV_DUMP_GET_BUFFER(DUMP_SAVE_MOD_ENUM ModId, BSP_CHAR** buffer, BSP_U32* length)
{
    return bsp_dump_get_buffer(ModId, buffer, length);
}

BSP_S32 DRV_DUMP_SAVE_FILE(DUMP_SAVE_FILE_ENUM FileId, BSP_CHAR* buffer, BSP_U32 length, DUMP_SAVE_FILE_MODE_ENUM mode)
{
    return bsp_dump_save_file(FileId, buffer, length, mode);
}

#ifdef __cplusplus
}
#endif


