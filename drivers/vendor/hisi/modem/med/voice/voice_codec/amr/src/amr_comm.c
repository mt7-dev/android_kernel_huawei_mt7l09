

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "amr_comm.h"
#include "gc_pred.h"


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


Word32 AMR_OpVcMac4(
                Word16                 (*pshwX)[40],
                Word32                  swSum,
                Word32                  swCnst,
                Word32                  swCol,
                Word32                  swRow0,
                Word32                  swRow1,
                Word32                  swRow2,
                Word32                  swRow3 )
{
    swSum = L_mac(swSum, pshwX[swRow0][swCol], swCnst);
    swSum = L_mac(swSum, pshwX[swRow1][swCol], swCnst);
    swSum = L_mac(swSum, pshwX[swRow2][swCol], swCnst);
    swSum = L_mac(swSum, pshwX[swRow3][swCol], swCnst);

    return swSum;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

