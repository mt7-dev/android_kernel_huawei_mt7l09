/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : Om.c
  版 本 号      : 初稿
  作    者      : 甘兰47350
  生成日期      : 2008年5月3日
  最近修改      :
  功能描述      : 该C文件给出了OM模块的实现
  函数列表      :
  修改历史      :
  1.日    期    : 2008年5月3日
    作    者    : 甘兰47350
    修改内容    : 创建文件

******************************************************************************/
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "product_config.h"

#if (FEATURE_SAMPLE_LTE_CHAN == FEATURE_ON)
/*lint -e830 -e628*/
#include "SOCPInterface.h"
#include "BSP_SOCP_DRV.h"
#include "socp_lfix_chan.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  3 全局变量定义
*****************************************************************************/
SOCP_ENCSRC_FIXCHAN_S g_stEncSrcFixChan[SOCP_CODER_SRC_BUTT];
BSP_S32 g_armBaseAddr = 0;


BSP_U32  DRV_SOCP_INIT_LTE_DSP(BSP_U32 ulChanId,BSP_U32 ulPhyAddr,BSP_U32 ulSize)
{
    SOCP_ENCSRC_FIXCHAN_S *pChan;

    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(ulChanId),(BSP_U32)ulPhyAddr);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(ulChanId),(BSP_U32)ulPhyAddr);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(ulChanId), (BSP_U32)ulPhyAddr);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(ulChanId), 0, 27, ulSize);
	SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(ulChanId), 27, 5, 0);

    /*配置SOCP 参数*/
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 1, 2, SOCP_ENCSRC_CHNMODE_CTSPACKET);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 4, 4, SOCP_DSPLOG_DST_BUFID);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 8, 2, SOCP_CHAN_PRIORITY_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 10, 1, SOCP_ENCSRC_BYPASS_DISABLE);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 16, 8, SOCP_DATA_TYPE_0);

    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 11, 1, SOCP_DATA_TYPE_EN);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 31, 1, SOCP_ENC_DEBUG_DIS);

    /* 使能中断*/
    SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT1, ulChanId, 1, 1);
    SOCP_REG_SETBITS(SOCP_REG_APP_MASK1, ulChanId, 1, 0);

    /* 保存参数到全局变量*/

    pChan = &g_stEncSrcFixChan[ulChanId];
    pChan->u32ChanID               = ulChanId;
    pChan->u32DestChanID           = SOCP_DSPLOG_DST_BUFID;
    pChan->eChnMode                = SOCP_ENCSRC_CHNMODE_CTSPACKET;
    pChan->eDataType               = SOCP_DATA_TYPE_0;
    pChan->ePriority               = SOCP_CHAN_PRIORITY_0;
    pChan->sEncSrcBuf.u32Start     = (BSP_U32)ulPhyAddr;
    pChan->sEncSrcBuf.u32Write     = (BSP_U32)ulPhyAddr;
    pChan->sEncSrcBuf.u32End       = (BSP_U32)(ulPhyAddr+ulSize);
    pChan->sEncSrcBuf.u32Read      = (BSP_U32)ulPhyAddr;
    pChan->sEncSrcBuf.u32Length    = (BSP_U32)ulSize;
    pChan->u32ChanEn               = SOCP_CHN_ENABLE;

    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  :  DRV_SOCP_INIT_LTE_BBP_LOG
*
* 功能描述  : LTE BBP通道初始化函数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
BSP_U32  DRV_SOCP_INIT_LTE_BBP_LOG(BSP_U32 ulChanId,BSP_U32 ulPhyAddr,BSP_U32 ulSize)
{
    SOCP_ENCSRC_FIXCHAN_S * pChan;
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(ulChanId), ulPhyAddr);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(ulChanId), ulPhyAddr);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(ulChanId), ulPhyAddr);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(ulChanId), 0, 27, ulSize);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(ulChanId), 27, 5, 0);

    /*配置SOCP 参数*/
    //SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 0, 1, 1);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 1, 2, SOCP_ENCSRC_CHNMODE_CTSPACKET);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 4, 4, SOCP_BBPLOG_DST_BUFID);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 8, 2, SOCP_CHAN_PRIORITY_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 10, 1, SOCP_ENCSRC_BYPASS_DISABLE);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 16, 8, SOCP_DATA_TYPE_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 11, 1, SOCP_DATA_TYPE_EN);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 31, 1, SOCP_ENC_DEBUG_DIS);

    /* 使能中断*/
    SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT1, ulChanId, 1, 1);
    SOCP_REG_SETBITS(SOCP_REG_APP_MASK1, ulChanId, 1, 0);

    pChan = &g_stEncSrcFixChan[ulChanId];

    pChan->u32ChanID               = ulChanId;
    pChan->u32DestChanID           = SOCP_BBPLOG_DST_BUFID;
    pChan->eChnMode                = SOCP_ENCSRC_CHNMODE_CTSPACKET;
    pChan->eDataType               = SOCP_DATA_TYPE_0;
    pChan->ePriority               = SOCP_CHAN_PRIORITY_0;
    pChan->sEncSrcBuf.u32Start     = ulPhyAddr;
    pChan->sEncSrcBuf.u32Write     = ulPhyAddr;
    pChan->sEncSrcBuf.u32Read      = ulPhyAddr;
    pChan->sEncSrcBuf.u32End       = ulPhyAddr + ulSize;
    pChan->sEncSrcBuf.u32Length    = (BSP_U32)ulSize;
    pChan->u32ChanEn               = SOCP_CHN_ENABLE;

    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  :  DRV_SOCP_INIT_LTE_BBP_DS
*
* 功能描述  : LTE BBP通道初始化函数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
BSP_U32  DRV_SOCP_INIT_LTE_BBP_DS(BSP_U32 ulChanId,BSP_U32 ulPhyAddr,BSP_U32 ulSize)
{
   SOCP_ENCSRC_FIXCHAN_S * pChan;
	SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(ulChanId),ulPhyAddr);
	SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(ulChanId),ulPhyAddr);
   	SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(ulChanId),ulPhyAddr);
	/*配置SOCP 参数*/
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(ulChanId), 0, 27, ulSize);
	SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(ulChanId), 27, 5, 0);

    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 1, 2, SOCP_ENCSRC_CHNMODE_FIXPACKET);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 4, 4, SOCP_BBPDS_DST_BUFID);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 8, 2, SOCP_CHAN_PRIORITY_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 10, 1, SOCP_ENCSRC_BYPASS_DISABLE);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 16, 8, SOCP_DATA_TYPE_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 11, 1, SOCP_DATA_TYPE_EN);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 31, 1, SOCP_ENC_DEBUG_DIS);

    /* 数采通道默认不打开，由上层打开 */
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 0, 1, 0);

    /* 打开数采通道的 中断*/
    SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT1, ulChanId, 1, 1);
    SOCP_REG_SETBITS(SOCP_REG_APP_MASK1, ulChanId, 1, 0);
    /* 保存参数到全局变量*/
    pChan = &g_stEncSrcFixChan[ulChanId];
    pChan->u32ChanID               = ulChanId;
    pChan->u32DestChanID           = SOCP_BBPDS_DST_BUFID;
    pChan->eChnMode                = SOCP_ENCSRC_CHNMODE_FIXPACKET;
    pChan->eDataType               = SOCP_DATA_TYPE_0;
    pChan->ePriority               = SOCP_CHAN_PRIORITY_0;
    pChan->sEncSrcBuf.u32Start     = (BSP_U32)ulPhyAddr;
    pChan->sEncSrcBuf.u32Write     = (BSP_U32)ulPhyAddr;
    pChan->sEncSrcBuf.u32Read      = (BSP_U32)ulPhyAddr;
    pChan->sEncSrcBuf.u32End       = (BSP_U32)(ulPhyAddr+ulSize);
    pChan->sEncSrcBuf.u32Length    = (BSP_U32)ulSize;
    return BSP_OK;

}

BSP_VOID DRV_SOCP_ENABLE_LTE_BBP_DSP(BSP_U32 ulChanId)
{
	if(0==(SOCP_REG_GETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 0, 1)))
	{
		SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 0, 1, 1);
	}
}


/*****************************************************************************
* 函 数 名  : DRV_SOCP_DSPCHN_START
* 功能描述  : enable DSP channel
* 输入参数  :
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
BSP_VOID  DRV_SOCP_DSPCHN_START(BSP_VOID)
{
	if(0==(SOCP_REG_GETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 0, 1)))
	{
		SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 0, 1, 1);
	}

}

/*****************************************************************************
* 函 数 名  : DRV_SOCP_DSPCHN_STOP
* 功能描述  : disable DSP channel
* 输入参数  :
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
BSP_VOID  DRV_SOCP_DSPCHN_STOP(BSP_VOID)
{
	VOS_UINT32 IdleFlag;
       VOS_UINT32 j;

	if(SOCP_REG_GETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 0, 1))
	{
		SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 0, 1, 0);
	}
/*lint -e525 -e746*/
	for (j = 0; j < (SOCP_RESET_TIME*2); j++)
    {
        IdleFlag = SOCP_REG_GETBITS(SOCP_REG_ENCSTAT, SOCP_DSPLOG_CHN, 1);
        if (0 == IdleFlag)
        {
            break;
        }

        if ((SOCP_RESET_TIME - 1) == j)
        {
            vos_printf("bsp_socp_stop_dsp failed!\n");

			return ;
        }
    }
/*lint +e525 +e746*/
}

BSP_VOID  SOCP_EnableLFixChan(VOID)
{
}
BSP_VOID SOCP_LFixChan_Init(BSP_VOID)
{
}
/*lint +e830 +e628*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


