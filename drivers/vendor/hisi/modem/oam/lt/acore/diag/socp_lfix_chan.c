
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "SOCPInterface.h"
#include "BSP_SOCP_DRV.h"
#include "socp_lfix_chan.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

unsigned long long g_socp_dma_mask = 0xffffffffULL;
/*****************************************************************************
  3 全局变量定义
*****************************************************************************/
BSP_VOID  SOCP_EnableLFixChan(VOID)
{
    return ;
}
/*****************************************************************************
* 函 数 名  : SOCP_LFixChan_Init
*
* 功能描述  : 初始化L模的固定通道,已移到C核
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  :
*****************************************************************************/
BSP_VOID SOCP_LFixChan_Init(BSP_VOID)
{
    return ;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


