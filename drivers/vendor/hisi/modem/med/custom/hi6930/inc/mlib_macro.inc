
#ifndef __MLIB_MACRO_INC__
#define __MLIB_MACRO_INC__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

/*

提示:
阅读本文件, 需要一定的软件技能修养.

*/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 模块注册宏定义
*****************************************************************************/

/* 注册通路处理模块集 */
#define MLIB_REGISTER_PATH_MODULE_BEGIN(profile,path)      \
                    MLIB_MODULE_REG_STRU g_astMLIB_##profile##_##path##_Path[]={\

#define MLIB_REGISTER_PATH_MODULE_END()                      };

/* 注册模块 */
#define MLIB_REGISTER_MODULE(id,init,set,get,proc,clear)    \
    {id,init,set,get,proc,clear},

/*****************************************************************************
  2 模块获取宏定义
*****************************************************************************/

/* 获取通路模块首地址 */
#define MLIB_GetPathModules(profile,path)                  \
                    g_astMLIB_##profile##_##path##_Path

/* 获取通路模块个数 */
#define MLIB_GetPathModuleNum(profile,path)                \
                    sizeof(g_astMLIB_##profile##_##path##_Path)/sizeof(MLIB_MODULE_REG_STRU)


/*****************************************************************************
  3 场景注册宏定义
*****************************************************************************/

#define MLIB_REGISTER_PATH_BEGIN(profile)      \
                    MLIB_PATH_REG_STRU g_astMLIB_##profile##_Paths[]={\

#define MLIB_REGISTER_PATH_END()                      };

/* 注册场景 */
#define MLIB_REGISTER_PATH(id,modules,num)    \
    {id,modules,num},

/*****************************************************************************
  3 场景获取宏定义
*****************************************************************************/

/* 获取通道首地址 */
#define MLIB_GetPaths(profile)                  \
                    g_astMLIB_##profile##_Paths

/* 获取通道个数 */
#define MLIB_GetPathNum(profile)                \
                    sizeof(g_astMLIB_##profile##_Paths)/sizeof(MLIB_PATH_REG_STRU)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of mlib_module.h */
