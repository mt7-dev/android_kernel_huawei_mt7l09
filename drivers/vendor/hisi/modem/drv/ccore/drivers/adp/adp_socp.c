

#include "bsp_socp.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ENABLE_BUILD_SOCP /* do not build socp, stub */
/*****************************************************************************
* 函 数 名  : DRV_SOCP_INIT
*
* 功能描述  : 模块初始化函数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功的标识码
*****************************************************************************/
BSP_S32 DRV_SOCP_INIT()
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_CORDER_SET_SRC_CHAN
 功能描述  : 此接口完成SOCP编码器源通道的分配，根据编码器源通道参数设置通道属性，连接目标通道，返回函数执行结果。
 输入参数  : pSrcAttr:编码器源通道参数结构体指针。
             pSrcChanID:申请到的源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码源通道分配成功。
             SOCP_ERROR:编码源通道分配失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_CORDER_SET_SRC_CHAN(SOCP_CODER_SRC_ENUM_U32 enSrcChanID, SOCP_CODER_SRC_CHAN_STRU *pSrcAttr)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_FREE_CHANNEL
 功能描述  : 此接口根据通道ID释放分配的编解码通道。
 输入参数  : u32ChanID:通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:通道释放成功。
             SOCP_ERROR:通道释放失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_FREE_CHANNEL(BSP_U32 u32ChanID)
{
    return 0;
}

/*****************************************************************************
* 函 数 名  : DRV_SOCP_CLEAN_ENCSRC_CHAN
*
* 功能描述  : 清空编码源通道，同步V9 SOCP接口
*
* 输入参数  : enSrcChanID       编码通道号
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/
BSP_U32 DRV_SOCP_CLEAN_ENCSRC_CHAN(SOCP_CODER_SRC_ENUM_U32 enSrcChanID)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_REGISTER_EVENT_CB
 功能描述  : 此接口为给定通道注册事件回调函数。
 输入参数  : u32ChanID:通道ID。
             EventCB:事件回调函数，参考socp_event_cb函数定义
 输出参数  : 无。
 返 回 值  : SOCP_OK:注册事件回调函数成功。
             SOCP_ERROR:注册事件回调函数失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_REGISTER_EVENT_CB(BSP_U32 u32ChanID, socp_event_cb EventCB)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_START
 功能描述  : 此接口用于源通道，启动编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码启动成功。
             SOCP_ERROR:编码或解码启动失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_START(BSP_U32 u32SrcChanID)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_STOP
 功能描述  : 此接口用于源通道，停止编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码停止成功。
             SOCP_ERROR:编码或解码停止失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_STOP(BSP_U32 u32SrcChanID)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_CHAN_SOFT_RESET
 功能描述  : 源通道软复位
 输入参数  : u32ChanID:通道ID
 输出参数  : 无。
 返 回 值  : SOCP_OK:设复位成功。
             其他值:复位失败
*****************************************************************************/
BSP_S32 DRV_SOCP_CHAN_SOFT_RESET(BSP_U32 u32ChanID)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_GET_WRITE_BUFF
 功能描述  : 此接口用于获取写数据buffer。
 输入参数  : u32SrcChanID:源通道ID
             pBuff:           :写数据buffer

 输出参数  : 无。
 返 回 值  : SOCP_OK:获取写数据buffer成功。
             SOCP_ERROR:获取写数据buffer失败
*****************************************************************************/
BSP_S32 DRV_SOCP_GET_WRITE_BUFF( BSP_U32 u32SrcChanID, SOCP_BUFFER_RW_STRU *pBuff)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_WRITE_DONE
 功能描述  : 该接口用于数据的写操作，提供写入数据的长度。
 输入参数  : u32SrcChanID:源通道ID
             u32WrtSize:   已写入数据的长度
 输出参数  : 无。
 返 回 值  : SOCP_OK:写入数据成功。
             SOCP_ERROR:写入数据失败
*****************************************************************************/
BSP_S32 DRV_SOCP_WRITE_DONE(BSP_U32 u32SrcChanID, BSP_U32 u32WrtSize)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_REGISTER_RD_CB
 功能描述  : 该接口用于注册从RD缓冲区中读取数据的回调函数。
 输入参数  : u32SrcChanID:源通道ID
             RdCB:  事件回调函数
 输出参数  : 无。
 返 回 值  : SOCP_OK:注册RD环形缓冲区读数据回调函数成功。
             SOCP_ERROR:注册RD环形缓冲区读数据回调函数失败
*****************************************************************************/
BSP_S32 DRV_SOCP_REGISTER_RD_CB(BSP_U32 u32SrcChanID, socp_rd_cb RdCB)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_GET_RD_BUFFER
 功能描述  : 该此接口用于获取RD buffer的数据指针。
 输入参数  : u32SrcChanID:源通道ID
             pBuff:  RD buffer
 输出参数  : 无。
 返 回 值  : SOCP_OK:获取RD环形缓冲区成功
             SOCP_ERROR:获取RD环形缓冲区失败
*****************************************************************************/
BSP_S32 DRV_SOCP_GET_RD_BUFFER( BSP_U32 u32SrcChanID,SOCP_BUFFER_RW_STRU *pBuff)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : bsp_socp_read_rd_done
 功能描述  : 此接口用于上层通知SOCP驱动，从RD buffer中实际读取的数据。
 输入参数  : u32SrcChanID:源通道ID
             u32RDSize:  从RD buffer中实际读取的数据长度
 输出参数  : 无。
 返 回 值  : SOCP_OK:读取RDbuffer中的数据成功
             SOCP_ERROR:读取RDbuffer中的数据失败
*****************************************************************************/
BSP_S32 DRV_SOCP_READ_RD_DONE(BSP_U32 u32SrcChanID, BSP_U32 u32RDSize)
{
    return 0;
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
    return; 
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
    return;
}

/*****************************************************************************
* 函 数 名  : DRV_BBPDMA_DRX_BAK_REG
* 功能描述  : BBPDMA备份
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
BSP_S32  DRV_BBPDMA_DRX_BAK_REG(BSP_VOID)
{
    return 0;
}

/*****************************************************************************
* 函 数 名  : DRV_BBPDMA_DRX_RESTORE_REG
* 功能描述  : 恢复BBPDMA
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
BSP_S32  DRV_BBPDMA_DRX_RESTORE_REG(BSP_VOID)
{
    return 0;
}


/*****************************************************************************
* 函 数 名  : DRV_SOCP_GET_STATE
*
* 功能描述  : 获取SOCP状态
*
* 返 回 值  : SOCP_IDLE    空闲
*             SOCP_BUSY    忙碌
*****************************************************************************/
SOCP_STATE_ENUM_UINT32 DRV_SOCP_GET_STATE(BSP_VOID)
{
    return 0;
}

BSP_U32  DRV_SOCP_INIT_LTE_DSP(u32 ulChanId,u32 ulPhyAddr,u32 ulSize)
{
    return 0;
}

BSP_U32  DRV_SOCP_INIT_LTE_BBP_LOG(BSP_U32 ulChanId,BSP_U32 ulPhyAddr,BSP_U32 ulSize)
{
    return 0;
}

BSP_U32  DRV_SOCP_INIT_LTE_BBP_DS(BSP_U32 ulChanId,BSP_U32 ulPhyAddr,BSP_U32 ulSize)
{
    return 0;
}

BSP_VOID DRV_SOCP_ENABLE_LTE_BBP_DSP(BSP_U32 ulChanId)
{
    return;
}

/*****************************************************************************
* 函 数 名  : DRV_SOCP_VOTE_TO_MCORE
* 功能描述  : SOCP投票接口，该接口只在C核提供，适配LDSP首次加载的SOCP上电需求
* 输入参数  : type --- 投票类型
* 输出参数  : 无
* 返 回 值  : BSP_S32 0 --- 投票成功，0xFFFFFFFF --- 投票失败
*****************************************************************************/
BSP_S32 DRV_SOCP_VOTE_TO_MCORE(SOCP_VOTE_TYPE_ENUM_U32 type)
{
    return 0;
}

#else
/*****************************************************************************
* 函 数 名  : DRV_SOCP_INIT
*
* 功能描述  : 模块初始化函数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功的标识码
*****************************************************************************/
BSP_S32 DRV_SOCP_INIT()
{
    return socp_init();
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_CORDER_SET_SRC_CHAN
 功能描述  : 此接口完成SOCP编码器源通道的分配，根据编码器源通道参数设置通道属性，连接目标通道，返回函数执行结果。
 输入参数  : pSrcAttr:编码器源通道参数结构体指针。
             pSrcChanID:申请到的源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码源通道分配成功。
             SOCP_ERROR:编码源通道分配失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_CORDER_SET_SRC_CHAN(SOCP_CODER_SRC_ENUM_U32 enSrcChanID, SOCP_CODER_SRC_CHAN_STRU *pSrcAttr)
{
    return bsp_socp_coder_set_src_chan(enSrcChanID, pSrcAttr);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_FREE_CHANNEL
 功能描述  : 此接口根据通道ID释放分配的编解码通道。
 输入参数  : u32ChanID:通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:通道释放成功。
             SOCP_ERROR:通道释放失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_FREE_CHANNEL(BSP_U32 u32ChanID)
{
    return bsp_socp_free_channel(u32ChanID);
}

/*****************************************************************************
* 函 数 名  : DRV_SOCP_CLEAN_ENCSRC_CHAN
*
* 功能描述  : 清空编码源通道，同步V9 SOCP接口
*
* 输入参数  : enSrcChanID       编码通道号
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/
BSP_U32 DRV_SOCP_CLEAN_ENCSRC_CHAN(SOCP_CODER_SRC_ENUM_U32 enSrcChanID)
{
    return bsp_socp_clean_encsrc_chan(enSrcChanID);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_REGISTER_EVENT_CB
 功能描述  : 此接口为给定通道注册事件回调函数。
 输入参数  : u32ChanID:通道ID。
             EventCB:事件回调函数，参考socp_event_cb函数定义
 输出参数  : 无。
 返 回 值  : SOCP_OK:注册事件回调函数成功。
             SOCP_ERROR:注册事件回调函数失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_REGISTER_EVENT_CB(BSP_U32 u32ChanID, socp_event_cb EventCB)
{
    return bsp_socp_register_event_cb(u32ChanID, EventCB);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_START
 功能描述  : 此接口用于源通道，启动编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码启动成功。
             SOCP_ERROR:编码或解码启动失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_START(BSP_U32 u32SrcChanID)
{
    return bsp_socp_start(u32SrcChanID);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_STOP
 功能描述  : 此接口用于源通道，停止编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码停止成功。
             SOCP_ERROR:编码或解码停止失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_STOP(BSP_U32 u32SrcChanID)
{
    return bsp_socp_stop(u32SrcChanID);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_CHAN_SOFT_RESET
 功能描述  : 源通道软复位
 输入参数  : u32ChanID:通道ID
 输出参数  : 无。
 返 回 值  : SOCP_OK:设复位成功。
             其他值:复位失败
*****************************************************************************/
BSP_S32 DRV_SOCP_CHAN_SOFT_RESET(BSP_U32 u32ChanID)
{
    return bsp_socp_chan_soft_reset(u32ChanID);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_GET_WRITE_BUFF
 功能描述  : 此接口用于获取写数据buffer。
 输入参数  : u32SrcChanID:源通道ID
             pBuff:           :写数据buffer

 输出参数  : 无。
 返 回 值  : SOCP_OK:获取写数据buffer成功。
             SOCP_ERROR:获取写数据buffer失败
*****************************************************************************/
BSP_S32 DRV_SOCP_GET_WRITE_BUFF( BSP_U32 u32SrcChanID, SOCP_BUFFER_RW_STRU *pBuff)
{
    return bsp_socp_get_write_buff(u32SrcChanID, pBuff);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_WRITE_DONE
 功能描述  : 该接口用于数据的写操作，提供写入数据的长度。
 输入参数  : u32SrcChanID:源通道ID
             u32WrtSize:   已写入数据的长度
 输出参数  : 无。
 返 回 值  : SOCP_OK:写入数据成功。
             SOCP_ERROR:写入数据失败
*****************************************************************************/
BSP_S32 DRV_SOCP_WRITE_DONE(BSP_U32 u32SrcChanID, BSP_U32 u32WrtSize)
{
    return bsp_socp_write_done(u32SrcChanID, u32WrtSize);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_REGISTER_RD_CB
 功能描述  : 该接口用于注册从RD缓冲区中读取数据的回调函数。
 输入参数  : u32SrcChanID:源通道ID
             RdCB:  事件回调函数
 输出参数  : 无。
 返 回 值  : SOCP_OK:注册RD环形缓冲区读数据回调函数成功。
             SOCP_ERROR:注册RD环形缓冲区读数据回调函数失败
*****************************************************************************/
BSP_S32 DRV_SOCP_REGISTER_RD_CB(BSP_U32 u32SrcChanID, socp_rd_cb RdCB)
{
    return bsp_socp_register_rd_cb(u32SrcChanID, RdCB);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_GET_RD_BUFFER
 功能描述  : 该此接口用于获取RD buffer的数据指针。
 输入参数  : u32SrcChanID:源通道ID
             pBuff:  RD buffer
 输出参数  : 无。
 返 回 值  : SOCP_OK:获取RD环形缓冲区成功
             SOCP_ERROR:获取RD环形缓冲区失败
*****************************************************************************/
BSP_S32 DRV_SOCP_GET_RD_BUFFER( BSP_U32 u32SrcChanID,SOCP_BUFFER_RW_STRU *pBuff)
{
    return bsp_socp_get_rd_buffer(u32SrcChanID, pBuff);
}

/*****************************************************************************
 函 数 名  : bsp_socp_read_rd_done
 功能描述  : 此接口用于上层通知SOCP驱动，从RD buffer中实际读取的数据。
 输入参数  : u32SrcChanID:源通道ID
             u32RDSize:  从RD buffer中实际读取的数据长度
 输出参数  : 无。
 返 回 值  : SOCP_OK:读取RDbuffer中的数据成功
             SOCP_ERROR:读取RDbuffer中的数据失败
*****************************************************************************/
BSP_S32 DRV_SOCP_READ_RD_DONE(BSP_U32 u32SrcChanID, BSP_U32 u32RDSize)
{
    return bsp_socp_read_rd_done(u32SrcChanID, u32RDSize);
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
    bsp_socp_start_dsp(); 
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
    bsp_socp_stop_dsp();
}

/*****************************************************************************
* 函 数 名  : DRV_BBPDMA_DRX_BAK_REG
* 功能描述  : BBPDMA备份
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
extern BSP_S32 BSP_BBPDMA_DrxBakReg(BSP_VOID);
BSP_S32  DRV_BBPDMA_DRX_BAK_REG(BSP_VOID)
{
    return (BSP_S32)BSP_BBPDMA_DrxBakReg();
}

/*****************************************************************************
* 函 数 名  : DRV_BBPDMA_DRX_RESTORE_REG
* 功能描述  : 恢复BBPDMA
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
extern BSP_S32 BSP_BBPDMA_DrxRestoreReg(BSP_VOID);
BSP_S32  DRV_BBPDMA_DRX_RESTORE_REG(BSP_VOID)
{
    return BSP_BBPDMA_DrxRestoreReg();
}


/*****************************************************************************
* 函 数 名  : DRV_SOCP_GET_STATE
*
* 功能描述  : 获取SOCP状态
*
* 返 回 值  : SOCP_IDLE    空闲
*             SOCP_BUSY    忙碌
*****************************************************************************/
SOCP_STATE_ENUM_UINT32 DRV_SOCP_GET_STATE(BSP_VOID)
{
    return bsp_socp_get_state();
}

extern BSP_U32 bsp_socp_init_lte_dsp(u32 ulChanId,u32 ulPhyAddr,u32 ulSize);
BSP_U32 DRV_SOCP_INIT_LTE_DSP(u32 ulChanId,u32 ulPhyAddr,u32 ulSize)
{
    return bsp_socp_init_lte_dsp(ulChanId,ulPhyAddr,ulSize);
}

extern BSP_U32 bsp_socp_init_lte_bbp_log(BSP_U32 ulChanId,BSP_U32 ulPhyAddr,BSP_U32 ulSize);
BSP_U32 DRV_SOCP_INIT_LTE_BBP_LOG(BSP_U32 ulChanId,BSP_U32 ulPhyAddr,BSP_U32 ulSize)
{
    return bsp_socp_init_lte_bbp_log(ulChanId,ulPhyAddr,ulSize);
}

extern BSP_U32 bsp_socp_int_lte_bbp_ds(BSP_U32 ulChanId,BSP_U32 ulPhyAddr,BSP_U32 ulSize);
BSP_U32 DRV_SOCP_INIT_LTE_BBP_DS(BSP_U32 ulChanId,BSP_U32 ulPhyAddr,BSP_U32 ulSize)
{
    return bsp_socp_int_lte_bbp_ds(ulChanId,ulPhyAddr,ulSize);
}

extern BSP_VOID bsp_socp_enable_lte_dsp_bbp(BSP_U32 ulChanId);
BSP_VOID DRV_SOCP_ENABLE_LTE_BBP_DSP(BSP_U32 ulChanId)
{
    bsp_socp_enable_lte_dsp_bbp(ulChanId);
}

/*****************************************************************************
* 函 数 名  : DRV_SOCP_VOTE_TO_MCORE
* 功能描述  : SOCP投票接口，该接口只在C核提供，适配LDSP首次加载的SOCP上电需求
* 输入参数  : type --- 投票类型
* 输出参数  : 无
* 返 回 值  : BSP_S32 0 --- 投票成功，0xFFFFFFFF --- 投票失败
*****************************************************************************/
BSP_S32 DRV_SOCP_VOTE_TO_MCORE(SOCP_VOTE_TYPE_ENUM_U32 type)
{
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    return bsp_socp_vote_to_mcore(type);
#else
    return 0;
#endif
}

#endif /* ENABLE_BUILD_SOCP */

BSP_VOID BSP_SOCP_RefreshSDLogCfg(BSP_U32 ulTimerLen)
{
	return;
}

#ifdef __cplusplus
}
#endif

