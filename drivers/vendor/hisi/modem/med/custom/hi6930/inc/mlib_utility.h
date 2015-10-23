
#ifndef __MLIB_UTILITY_H__
#define __MLIB_UTILITY_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "mlib_typedef.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  动态内存分配和释放
*****************************************************************************/

/*
动态内存申请, 参数以及函数功能同标准的malloc
*/
extern MLIB_VOID*  MUTIL_Malloc( MLIB_UINT32  uwSize );

/*
动态内存释放, 参数以及函数功能同标准的free
*/
extern MLIB_VOID  MUTIL_Free( MLIB_VOID *pMem );


/*
内存拷贝, 参数以及函数功能同标准的memcpy
*/
extern MLIB_VOID MUTIL_MemCpy(
                       MLIB_VOID              *pvDes,
                       MLIB_VOID              *pvSrc,
                       MLIB_UINT32             uwCount);

/*
内存设置, 参数以及函数功能同标准的memset
*/
extern MLIB_VOID MUTIL_MemSet(
                       MLIB_VOID               *pvDes,
                       MLIB_UINT8               ucData,
                       MLIB_UINT32              uwCount);

/*
NV项读取:
uhwID       - NV ID
pItem       - NV 项数据首地址, 数据空间需要预先分配
uwLength    - 需要读取的 NV 项数据大小
*/
extern MLIB_UINT32 MUTIL_ReadNV(
                       MLIB_UINT16             uhwId,
                       MLIB_VOID               *pItem,
                       MLIB_UINT32              uwLength);

/*
数据勾取:
uhwPos      - 数据索引，有效值为二进制有且只有有1位为1其他位为的数，如1,2,4,8,16...
pvData      - 需要勾取的数据首地址
uwLength    - 需要勾取的数据大小
*/
extern MLIB_VOID MUTIL_DataHook(
                    MLIB_UINT16              uhwPos,
                    MLIB_VOID               *pvData,
                    MLIB_UINT16              uhwLen);

/* 日志上报级别 */
enum MLIB_LOG_LEVEL_ENUM
{
    MLIB_LOG_LEVEL_INFO                 = 0,
    MLIB_LOG_LEVEL_WARNING,
    MLIB_LOG_LEVEL_ERROR,
    MLIB_LOG_LEVEL_BUTT
};
typedef MLIB_UINT16 MLIB_LOG_LEVEL_ENUM_UINT32;

/* apr上报logId,范围0~9 */
enum
{
    /* ext_pp */
    APR_LOG_EXT_PP_TMP     = 0,

};
typedef MLIB_UINT32 MLIB_APR_LOG_ID_ENUM_UINT32;

/*
日志打印:
enLevel - 打印日志级别(取值范围MLIB_LOG_LEVEL_ENUM)
uhwFileID - 自定义文件号(为避免冲突, 建议从0x0a00开始编号)
uhwLineID - 打印所在的行号
swValue1/2/3 - 需要打印的数值

不要直接调用这个函数， 调用下面封装的宏
*/
extern MLIB_VOID MUTIL_LogReport(
                MLIB_LOG_LEVEL_ENUM_UINT32          enLevel,
                MLIB_UINT16                          uhwFileID,
                MLIB_UINT16                          uhwLineID,
                MLIB_INT32                           swValue1,
                MLIB_INT32                           swValue2,
                MLIB_INT32                           swValue3);

/* apr日志打印:
*/
extern MLIB_VOID  MLIB_LogApr(MLIB_APR_LOG_ID_ENUM_UINT32 enLogId);

/* 打印提示信息 */
#define MUTIL_LogInfo(enFileID,swValue1,swValue2,swValue3)          \
    MUTIL_LogReport(MLIB_LOG_LEVEL_INFO,enFileID,__LINE__,(swValue1),(swValue2),(swValue3))

/* 打印警告信息 */
#define MUTIL_LogWarning(enFileID,swValue1,swValue2,swValue3)       \
    MUTIL_LogReport(MLIB_LOG_LEVEL_WARNING,enFileID,__LINE__,(swValue1),(swValue2),(swValue3))

/* 打印错误信息 */
#define MUTIL_LogError(enFileID,swValue1,swValue2,swValue3)         \
    MUTIL_LogReport(MLIB_LOG_LEVEL_ERROR,enFileID,__LINE__,(swValue1),(swValue2),(swValue3))

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of mlib_utility.h */
