

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "mlib_interface.h"
#include "mlib_utility.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/



MLIB_ERROR_ENUM_UINT32 MLIB_DEFAULT_Proc(
                MLIB_SOURCE_STRU        *pstSourceOut,
                MLIB_SOURCE_STRU        *pstSourceIn )
{
    MLIB_UINT32 uwChnID, i;

    for(uwChnID = 0; uwChnID < pstSourceIn->uwChnNum; uwChnID++)
    {
        MUTIL_MemCpy(
                pstSourceOut->astChn[uwChnID].pucData,
                pstSourceIn->astChn[uwChnID].pucData,
                pstSourceIn->astChn[uwChnID].uwSize);
    }

    return MLIB_ERROR_NONE;
}









#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

