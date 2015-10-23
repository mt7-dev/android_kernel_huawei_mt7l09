

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <xtensa/coreasm.h>
#include <xtensa/simcall.h>
#define PIF_CACHED  1
#define PIF_BYPASS  2
#define PIF_INVALID 15



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





/*set memory mapping attribute*/
.macro set_access_mode am
    rdtlb1 a4, a3
    ritlb1 a5, a3
    srli a4, a4, 4
    slli a4, a4, 4
    srli a5, a5, 4
    slli a5, a5, 4
    addi a4, a4, \am
    addi a5, a5, \am
    wdtlb a4, a3
    witlb a5, a3
.endm







#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

