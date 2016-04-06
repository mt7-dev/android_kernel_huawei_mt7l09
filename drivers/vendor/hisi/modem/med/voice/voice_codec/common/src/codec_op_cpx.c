

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "codec_op_lib.h"
#include  "codec_op_cpx.h"
#include  "codec_op_float.h"



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


CODEC_OP_CMPX_STRU CODEC_OpCpxMulQ15(
                CODEC_OP_CMPX_STRU  stA,
                CODEC_OP_CMPX_STRU  stB)
{
    CODEC_OP_CMPX_STRU ret;

    ret.shwR = CODEC_OpSaturate(CODEC_OpL_shr_r(
            CODEC_OpL_sub( CODEC_OpL_mult0(stA.shwR,stB.shwR),
            CODEC_OpL_mult0(stA.shwI, stB.shwI) ), CODEC_OP_Q15));
    ret.shwI = CODEC_OpSaturate(CODEC_OpL_shr_r(
            CODEC_OpL_add( CODEC_OpL_mult0(stA.shwR,stB.shwI),
            CODEC_OpL_mult0(stA.shwI, stB.shwR) ), CODEC_OP_Q15));

    return ret;
}


CODEC_OP_CMPX_STRU CODEC_OpCpxMulQ17(
                CODEC_OP_CMPX_STRU  stA,
                CODEC_OP_CMPX_STRU  stB)
{

    CODEC_OP_CMPX_STRU ret;

    ret.shwR = (VOS_INT16)CODEC_OpL_shr_r(
            CODEC_OpL_sub( CODEC_OpL_mult0(stA.shwR,stB.shwR),
            CODEC_OpL_mult0(stA.shwI, stB.shwI) ), CODEC_OP_Q17);
    ret.shwI = (VOS_INT16)CODEC_OpL_shr_r(
            CODEC_OpL_add( CODEC_OpL_mult0(stA.shwR,stB.shwI),
            CODEC_OpL_mult0(stA.shwI, stB.shwR) ), CODEC_OP_Q17);

    return ret;
}










#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
