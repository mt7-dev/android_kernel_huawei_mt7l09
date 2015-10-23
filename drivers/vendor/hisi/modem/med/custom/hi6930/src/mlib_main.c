

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "mlib_typedef.h"
#include "mlib_interface.h"
#include "mlib_utility.h"

/* 注册各种Profile */
#include "mlib_register_profile0.inc"
#include "mlib_register_profile1.inc"
#include "mlib_register_profile2.inc"
#include "mlib_register_profile3.inc"
#include "mlib_register_profile4.inc"
#include "mlib_register_profile5.inc"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 配置注册表 */
MLIB_PROFILE_REG_STRU           g_astMlibProfileRegTab[] = \
{
    {MLIB_GetPaths(MLIB_PROFILE0), MLIB_GetPathNum(MLIB_PROFILE0)},
    {MLIB_GetPaths(MLIB_PROFILE1), MLIB_GetPathNum(MLIB_PROFILE1)},
    {MLIB_GetPaths(MLIB_PROFILE2), MLIB_GetPathNum(MLIB_PROFILE2)},
    {MLIB_GetPaths(MLIB_PROFILE3), MLIB_GetPathNum(MLIB_PROFILE3)},
    {MLIB_GetPaths(MLIB_PROFILE4), MLIB_GetPathNum(MLIB_PROFILE4)},
    {MLIB_GetPaths(MLIB_PROFILE5), MLIB_GetPathNum(MLIB_PROFILE5)},
};
#define MLIB_GetProfile(uwID)   (&g_astMlibProfileRegTab[uwID])
#define MLIB_GetProfileNum()    (sizeof(g_astMlibProfileRegTab)/sizeof(MLIB_PROFILE_REG_STRU))

/* 运算库配置 */
#if (VOS_CPU_TYPE == VOS_HIFI)
__attribute__((section(".sram.uninit")))
MLIB_CONFIG_STRU                g_stMlibConfig = {0};
#else
MLIB_CONFIG_STRU                g_stMlibConfig = {0};
#endif
#define MLIB_GetConfig()        (&g_stMlibConfig)

/*****************************************************************************
  3 函数实现
*****************************************************************************/


MLIB_ERROR_ENUM_UINT32  MLIB_ModuleInitBasic(
                MLIB_MODULE_REG_STRU   *pstModules,
                MLIB_UINT32             uwNum )
{
    MLIB_UINT32                         uwID;
    MLIB_ERROR_ENUM_UINT32              uwRet;

    uwRet = MLIB_ERROR_NONE;

    for(uwID = 0; uwID < uwNum; uwID++)
    {
        if(pstModules[uwID].pfInit != NULL)
        {
            uwRet = pstModules[uwID].pfInit();

            if(uwRet != MLIB_ERROR_NONE)
            {
                break;
            }
        }
    }

    return uwRet;
}


MLIB_MODULE_REG_STRU*  MLIB_ModuleGetItem(
                MLIB_MODULE_ENUM_UINT32 enID,
                MLIB_MODULE_REG_STRU   *pstModules,
                MLIB_UINT32             uwNum)
{
    MLIB_UINT32 uwID;

    for(uwID=0; uwID < uwNum; uwID++)
    {
        if(pstModules->enID == enID)
        {
            return pstModules;
        }

        pstModules++;
    }

    return NULL;
}


MLIB_ERROR_ENUM_UINT32  MLIB_ModuleSetBasic(
                MLIB_MODULE_ENUM_UINT32 enID,
                MLIB_PARA_STRU         *pstPara,
                MLIB_UINT32             uwSize,
                MLIB_MODULE_REG_STRU   *pstModules,
                MLIB_UINT32             uwNum )
{
    MLIB_ERROR_ENUM_UINT32              uwRet;
    MLIB_MODULE_REG_STRU               *pstModule;

    pstModule =  MLIB_ModuleGetItem(enID, pstModules, uwNum);

    if(pstModule == NULL)
    {
        return MLIB_ERROR_NULL_PTR;
    }

    if(pstModule->pfSet != NULL)
    {
        uwRet = pstModule->pfSet(pstPara, uwSize);
    }
    else
    {
        uwRet = MLIB_ERROR_NULL_FUNC;
    }

    return uwRet;
}



MLIB_ERROR_ENUM_UINT32  MLIB_ModuleGetBasic(
                MLIB_MODULE_ENUM_UINT32 enID,
                MLIB_VOID              *pvData,
                MLIB_UINT32             uwSize,
                MLIB_MODULE_REG_STRU   *pstModules,
                MLIB_UINT32             uwNum )
{
    MLIB_ERROR_ENUM_UINT32              uwRet;
    MLIB_MODULE_REG_STRU               *pstModule;

    pstModule =  MLIB_ModuleGetItem(enID, pstModules, uwNum);

    if(pstModule == NULL)
    {
        return MLIB_ERROR_NULL_PTR;
    }

    if(pstModule->pfGet != NULL)
    {
        uwRet = pstModule->pfGet(pvData, uwSize);
    }
    else
    {
        uwRet = MLIB_ERROR_NULL_FUNC;
    }

    return uwRet;
}



MLIB_ERROR_ENUM_UINT32  MLIB_ModuleProcBasic(
                MLIB_SOURCE_STRU       *pstSourceOut,
                MLIB_SOURCE_STRU       *pstSourceIn,
                MLIB_MODULE_REG_STRU   *pstModules,
                MLIB_UINT32             uwNum )
{
    MLIB_UINT32                         uwID, uwChnID;
    MLIB_SOURCE_STRU                    stSource;
    MLIB_ERROR_ENUM_UINT32              uwRet;

    uwRet = MLIB_ERROR_NONE;

    MUTIL_MemCpy(&stSource, pstSourceIn, sizeof(MLIB_SOURCE_STRU));

    /* 准备缓冲数据通道 */
    for(uwChnID = 0; uwChnID < stSource.uwChnNum; uwChnID++)
    {
        /* 为缓冲数据各通道申请内存 */
        stSource.astChn[uwChnID].pucData = (MLIB_UINT8*)MUTIL_Malloc(stSource.astChn[uwChnID].uwSize);

        /* 将输入各通道数据依次拷贝至缓冲数据通道 */
        MUTIL_MemCpy(stSource.astChn[uwChnID].pucData,
            pstSourceIn->astChn[uwChnID].pucData,
            stSource.astChn[uwChnID].uwSize);
    }

    for(uwID = 0; uwID < uwNum; uwID++)
    {
        /* 对缓冲通道数据进行处理， 结果保存在输出数据通道 */
        uwRet = pstModules[uwID].pfProc(pstSourceOut, &stSource);

        if(uwRet != MLIB_ERROR_NONE)
        {
            break;
        }

        /* 将输出各通道数据依次拷贝至缓冲数据通道 */
        for(uwChnID = 0; uwChnID < stSource.uwChnNum; uwChnID++)
        {
            MUTIL_MemCpy(stSource.astChn[uwChnID].pucData,
                pstSourceOut->astChn[uwChnID].pucData,
                stSource.astChn[uwChnID].uwSize);
        }
    }

    for(uwID = 0; uwID < stSource.uwChnNum; uwID++)
    {
        /* 释放为缓冲数据各通道申请内存 */
        MUTIL_Free(stSource.astChn[uwID].pucData);
    }

    return uwRet;
}


MLIB_ERROR_ENUM_UINT32  MLIB_ModuleClearBasic(
                MLIB_MODULE_REG_STRU   *pstModules,
                MLIB_UINT32             uwNum )
{
    MLIB_UINT32                         uwID;
    MLIB_ERROR_ENUM_UINT32              uwRet;

    uwRet = MLIB_ERROR_NONE;

    for(uwID = 0; uwID < uwNum; uwID++)
    {
        if(pstModules[uwID].pfClear != NULL)
        {
            uwRet = pstModules[uwID].pfClear();

            if(uwRet != MLIB_ERROR_NONE)
            {
                break;
            }
        }
    }

    return uwRet;
}


MLIB_MODULE_REG_STRU*  MLIB_PathGetModules(
                MLIB_PATH_ENUM_UINT32   enID,
                MLIB_UINT32            *puwNum)
{
    MLIB_UINT32                         uwID, uwSize;
    MLIB_PATH_REG_STRU                 *pstPaths;
    MLIB_PROFILE_REG_STRU              *pstProfile;
    MLIB_CONFIG_STRU                   *pstConfig;

    pstConfig = MLIB_GetConfig();

    /* 获取当前使用配置 */
    pstProfile  = MLIB_GetProfile(pstConfig->uwProfile);

    /* 设置配置 */
    pstPaths    = pstProfile->pstPaths;
    uwSize      = pstProfile->uwNum;

    for(uwID=0; uwID < uwSize; uwID++)
    {
        if(pstPaths->enID == enID)
        {
            *puwNum = pstPaths->uwModuleNum;
            return pstPaths->pstModules;
        }

        pstPaths++;
    }

    return NULL;
}


MLIB_ERROR_ENUM_UINT32 MLIB_SelectProfile( MLIB_UINT32 uwID)
{
    MLIB_CONFIG_STRU    *pstConfig;

    pstConfig = MLIB_GetConfig();

    if(uwID >= MLIB_GetProfileNum())
    {
        return MLIB_ERROR_FAIL;
    }

    pstConfig->uwProfile = uwID;

    return MLIB_ERROR_NONE;
}


MLIB_ERROR_ENUM_UINT32  MLIB_PathInit( MLIB_PATH_ENUM_UINT32 enPath)
{
    MLIB_UINT32                         uwNum;
    MLIB_MODULE_REG_STRU               *pstModules;

    uwNum       = 0;
    pstModules  = MLIB_PathGetModules(enPath, &uwNum);

    if(pstModules != NULL)
    {
        return MLIB_ModuleInitBasic(pstModules, uwNum);
    }
    else
    {
        return MLIB_ERROR_NULL_PTR;
    }
}


MLIB_ERROR_ENUM_UINT32  MLIB_PathModuleSet(
                MLIB_PATH_ENUM_UINT32   enPath,
                MLIB_MODULE_ENUM_UINT32 enModule,
                MLIB_PARA_STRU         *pstPara,
                MLIB_UINT32             uwSize )
{
    MLIB_UINT32                         uwNum;
    MLIB_MODULE_REG_STRU               *pstModules;

    uwNum       = 0;
    pstModules  = MLIB_PathGetModules(enPath, &uwNum);

    if(pstModules != NULL)
    {
        return MLIB_ModuleSetBasic(enModule, pstPara, uwSize, pstModules, uwNum);
    }
    else
    {
        return MLIB_ERROR_NULL_PTR;
    }
}


MLIB_ERROR_ENUM_UINT32  MLIB_PathModuleGet(
                MLIB_PATH_ENUM_UINT32   enPath,
                MLIB_MODULE_ENUM_UINT32 enModule,
                MLIB_VOID              *pvData,
                MLIB_UINT32             uwSize )
{
    MLIB_UINT32                         uwNum;
    MLIB_MODULE_REG_STRU               *pstModules;

    uwNum       = 0;
    pstModules  = MLIB_PathGetModules(enPath, &uwNum);

    if(pstModules != NULL)
    {
        return MLIB_ModuleGetBasic(enModule, pvData, uwSize, pstModules, uwNum);
    }
    else
    {
        return MLIB_ERROR_NULL_PTR;
    }
}


MLIB_ERROR_ENUM_UINT32  MLIB_PathSet(
                MLIB_PATH_ENUM_UINT32   enPath,
                MLIB_PARA_STRU         *pstPara,
                MLIB_UINT32             uwSize )
{
    MLIB_UINT32                         uwNum;
    MLIB_UINT32                         uwID;
    MLIB_MODULE_REG_STRU               *pstModules;
    MLIB_ERROR_ENUM_UINT32              uwRet;

    uwNum       = 0;
    pstModules  = MLIB_PathGetModules(enPath, &uwNum);

    uwRet = MLIB_ERROR_NONE;

    if(NULL == pstModules)
    {
        return MLIB_ERROR_NULL_PTR;
    }

    for(uwID = 0; uwID < uwNum; uwID++)
    {
        if(pstModules[uwID].pfSet != NULL)
        {
            uwRet = pstModules[uwID].pfSet(pstPara, uwSize);

            if(uwRet != MLIB_ERROR_NONE)
            {
                break;
            }
        }
    }

    return uwRet;
}



MLIB_ERROR_ENUM_UINT32  MLIB_PathProc(
                MLIB_PATH_ENUM_UINT32   enPath,
                MLIB_SOURCE_STRU       *pstSourceOut,
                MLIB_SOURCE_STRU       *pstSourceIn)
{
    MLIB_UINT32                         uwNum;
    MLIB_MODULE_REG_STRU               *pstModules;

    uwNum       = 0;
    pstModules  = MLIB_PathGetModules(enPath, &uwNum);

    if(pstModules != NULL)
    {
        return MLIB_ModuleProcBasic(pstSourceOut, pstSourceIn, pstModules, uwNum);
    }
    else
    {
        return MLIB_ERROR_NULL_PTR;
    }
}


MLIB_ERROR_ENUM_UINT32  MLIB_PathClear( MLIB_PATH_ENUM_UINT32   enPath )
{
    MLIB_UINT32                         uwNum;
    MLIB_MODULE_REG_STRU               *pstModules;

    uwNum       = 0;
    pstModules  = MLIB_PathGetModules(enPath, &uwNum);

    if(pstModules != NULL)
    {
        return MLIB_ModuleClearBasic(pstModules, uwNum);
    }
    else
    {
        return MLIB_ERROR_NULL_PTR;
    }
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

