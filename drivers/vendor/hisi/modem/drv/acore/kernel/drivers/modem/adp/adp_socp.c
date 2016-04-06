

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
 函 数 名  : DRV_SOCP_CODER_SET_DEST_CHAN_ATTR
 功能描述  : 此接口完成某一编码目标通道的配置，返回函数执行的结果。
 输入参数  : u32DestChanID:SOCP编码器的目标通道ID。
             pDestAttr:SOCP编码器目标通道参数结构体指针。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码目的通道设置成功。
             SOCP_ERROR:编码目的通道设置失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_CODER_SET_DEST_CHAN_ATTR(BSP_U32 u32DestChanID, SOCP_CODER_DEST_CHAN_STRU *pDestAttr)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_DECODER_SET_DEST_CHAN
 功能描述  : 此接口完成SOCP解码器目标通道的分配，
             根据解码目标通道参数设置通道属性，
             并连接源通道，返回函数执行结果。
 输入参数  : pAttr:解码器目标通道参数结构体指针
             pDestChanID:申请到的目标通道ID
 输出参数  : 无。
 返 回 值  : SOCP_OK:解码目的通道分配成功。
             SOCP_ERROR:解码目的通道设置失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_DECODER_SET_DEST_CHAN(SOCP_DECODER_DST_ENUM_U32 enDestChanID, SOCP_DECODER_DEST_CHAN_STRU *pAttr)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_DECODER_SET_SRC_CHAN_ATTR
 功能描述  : 此接口完成某一解码源通道的配置，返回函数执行的结果。
 输入参数  : u32SrcChanID:解码器源通道ID
             pInputAttr:解码器源通道参数结构体指针
 输出参数  : 无。
 返 回 值  : SOCP_OK:解码源通道设置成功
             SOCP_ERROR:解码源通道设置失败
*****************************************************************************/
BSP_S32 DRV_SOCP_DECODER_SET_SRC_CHAN_ATTR ( BSP_U32 u32SrcChanID,SOCP_DECODER_SRC_CHAN_STRU *pInputAttr)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_DECODER_GET_ERR_CNT
 功能描述  :此接口给出解码通道中四种异常情况的计数值。
 输入参数  : u32ChanID:解码器通道ID
                         pErrCnt:解码器异常计数结构体指针
 输出参数  : 无。
 返 回 值      : SOCP_OK:返回异常计数成功
                             SOCP_ERROR:返回异常计数失败
*****************************************************************************/
BSP_S32 DRV_SOCP_DECODER_GET_ERR_CNT (BSP_U32 u32ChanID, SOCP_DECODER_ERROR_CNT_STRU *pErrCnt)
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
 函 数 名  : DRV_SOCP_SET_TIMEOUT
 功能描述  : 此接口设置超时阈值。
 输入参数  : u32Timeout:超时阈值

 输出参数  : 无。
 返 回 值  : SOCP_OK:设置超时时间阈值成功。
             SOCP_ERROR:设置超时时间阈值失败
*****************************************************************************/
BSP_S32 DRV_SOCP_SET_TIMEOUT (SOCP_TIMEOUT_EN_ENUM_UIN32 eTmOutEn, BSP_U32 u32Timeout)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_SET_DEC_PKT_LGTH
 功能描述  : 设置解码包长度极限值
 输入参数  : pPktlgth:解码包长度极值

 输出参数  : 无。
 返 回 值  : SOCP_OK:设置成功。
             其他值:设置失败
*****************************************************************************/
BSP_S32 DRV_SOCP_SET_DEC_PKT_LGTH(SOCP_DEC_PKTLGTH_STRU *pPktlgth)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_SET_DEBUG
 功能描述  : 设置解码源通道的debug模式
 输入参数  : u32ChanID:通道ID
             u32DebugEn: debug标识
 输出参数  : 无。
 返 回 值  : SOCP_OK:设置成功。
             其他值:设置失败
*****************************************************************************/
BSP_S32 DRV_SOCP_SET_DEBUG(BSP_U32 u32DestChanID, BSP_U32 u32DebugEn)
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
 函 数 名  : bsp_socp_register_read_cb
 功能描述  : 该接口用于注册读数据的回调函数。
 输入参数  : u32DestChanID:目标通道ID
             ReadCB: 事件回调函数
 输出参数  : 无。
 返 回 值  : SOCP_OK:注册读数据回调函数成功
             SOCP_ERROR:注册读数据回调函数失败
*****************************************************************************/
BSP_S32 DRV_SOCP_REGISTER_READ_CB( BSP_U32 u32DestChanID, socp_read_cb ReadCB)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_GET_READ_BUFF
 功能描述  : 该此接口用于获取读数据缓冲区指针。
 输入参数  : u32DestChanID:目标通道ID
             ReadCB: 读数据buffer
 输出参数  : 无。
 返 回 值  : SOCP_OK:获取读数据缓冲区成功。
             SOCP_ERROR:获取读数据缓冲区成功。
*****************************************************************************/
BSP_S32 DRV_SOCP_GET_READ_BUFF(BSP_U32 u32DestChanID,SOCP_BUFFER_RW_STRU *pBuffer)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_READ_DATA_DONE
 功能描述  : 该接口用于上层告诉SOCP驱动，从目标通道中读走的实际数据。
 输入参数  : u32DestChanID:目标通道ID
             u32ReadSize: 已读出数据的长度
 输出参数  : 无。
 返 回 值  : SOCP_OK:读数据成功。
             SOCP_ERROR:读数据失败
*****************************************************************************/
BSP_S32 DRV_SOCP_READ_DATA_DONE(BSP_U32 u32DestChanID,BSP_U32 u32ReadSize)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_SET_BBP_ENABLE
 功能描述  : 使能或停止BBP通道。
 输入参数  : bEnable:通道ID
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                   其他值:设置失败
*****************************************************************************/
BSP_S32 DRV_SOCP_SET_BBP_ENABLE(int bEnable)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_SET_BBP_DS_MODE
 功能描述  : 设置BBP DS通道数据溢出处理模式。
 输入参数  : eDsMode:DS通道数据溢出时处理模式设置
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                   其他值:设置失败
*****************************************************************************/
BSP_S32 DRV_SOCP_SET_BBP_DS_MODE(SOCP_BBP_DS_MODE_ENUM_UIN32 eDsMode)
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
SOCP_STATE_ENUM_UINT32 DRV_SOCP_GET_STATE()
{
    return 0;
}

/*****************************************************************************
* 函 数 名  : bsp_socp_enc_dst_buf_flush
* 功能描述  : SOCP编码目的buffer缓存数据立即输出
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/

BSP_S32 DRV_SOCP_ENC_DST_BUF_FLUSH(void)
{
	return 0;
}

/*****************************************************************************
* 函 数 名  : DRV_SOCP_VOTE
* 功能描述  : SOCP投票接口，根据投票结果决定SOCP是否睡眠，该接口只在A核提供
* 输入参数  : id --- 投票组件ID，type --- 投票类型
* 输出参数  : 无
* 返 回 值  : BSP_S32 0 --- 投票成功，0xFFFFFFFF --- 投票失败
*****************************************************************************/
BSP_S32 DRV_SOCP_VOTE(SOCP_VOTE_ID_ENUM_U32 id, SOCP_VOTE_TYPE_ENUM_U32 type)
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
 函 数 名  : DRV_SOCP_CODER_SET_DEST_CHAN_ATTR
 功能描述  : 此接口完成某一编码目标通道的配置，返回函数执行的结果。
 输入参数  : u32DestChanID:SOCP编码器的目标通道ID。
             pDestAttr:SOCP编码器目标通道参数结构体指针。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码目的通道设置成功。
             SOCP_ERROR:编码目的通道设置失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_CODER_SET_DEST_CHAN_ATTR(BSP_U32 u32DestChanID, SOCP_CODER_DEST_CHAN_STRU *pDestAttr)
{
    return bsp_socp_coder_set_dest_chan_attr(u32DestChanID, pDestAttr);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_DECODER_SET_DEST_CHAN
 功能描述  : 此接口完成SOCP解码器目标通道的分配，
             根据解码目标通道参数设置通道属性，
             并连接源通道，返回函数执行结果。
 输入参数  : pAttr:解码器目标通道参数结构体指针
             pDestChanID:申请到的目标通道ID
 输出参数  : 无。
 返 回 值  : SOCP_OK:解码目的通道分配成功。
             SOCP_ERROR:解码目的通道设置失败。
*****************************************************************************/
BSP_S32 DRV_SOCP_DECODER_SET_DEST_CHAN(SOCP_DECODER_DST_ENUM_U32 enDestChanID, SOCP_DECODER_DEST_CHAN_STRU *pAttr)
{
    return bsp_socp_decoder_set_dest_chan(enDestChanID, pAttr);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_DECODER_SET_SRC_CHAN_ATTR
 功能描述  : 此接口完成某一解码源通道的配置，返回函数执行的结果。
 输入参数  : u32SrcChanID:解码器源通道ID
             pInputAttr:解码器源通道参数结构体指针
 输出参数  : 无。
 返 回 值  : SOCP_OK:解码源通道设置成功
             SOCP_ERROR:解码源通道设置失败
*****************************************************************************/
BSP_S32 DRV_SOCP_DECODER_SET_SRC_CHAN_ATTR ( BSP_U32 u32SrcChanID,SOCP_DECODER_SRC_CHAN_STRU *pInputAttr)
{
    return bsp_socp_decoder_set_src_chan_attr(u32SrcChanID, pInputAttr);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_DECODER_GET_ERR_CNT
 功能描述  :此接口给出解码通道中四种异常情况的计数值。
 输入参数  : u32ChanID:解码器通道ID
                         pErrCnt:解码器异常计数结构体指针
 输出参数  : 无。
 返 回 值      : SOCP_OK:返回异常计数成功
                             SOCP_ERROR:返回异常计数失败
*****************************************************************************/
BSP_S32 DRV_SOCP_DECODER_GET_ERR_CNT (BSP_U32 u32ChanID, SOCP_DECODER_ERROR_CNT_STRU *pErrCnt)
{
    return bsp_socp_decoder_get_err_cnt(u32ChanID, pErrCnt);
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
 函 数 名  : DRV_SOCP_SET_TIMEOUT
 功能描述  : 此接口设置超时阈值。
 输入参数  : u32Timeout:超时阈值

 输出参数  : 无。
 返 回 值  : SOCP_OK:设置超时时间阈值成功。
             SOCP_ERROR:设置超时时间阈值失败
*****************************************************************************/
BSP_S32 DRV_SOCP_SET_TIMEOUT (SOCP_TIMEOUT_EN_ENUM_UIN32 eTmOutEn, BSP_U32 u32Timeout)
{
    return bsp_socp_set_timeout(eTmOutEn, u32Timeout);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_SET_DEC_PKT_LGTH
 功能描述  : 设置解码包长度极限值
 输入参数  : pPktlgth:解码包长度极值

 输出参数  : 无。
 返 回 值  : SOCP_OK:设置成功。
             其他值:设置失败
*****************************************************************************/
BSP_S32 DRV_SOCP_SET_DEC_PKT_LGTH(SOCP_DEC_PKTLGTH_STRU *pPktlgth)
{
    return bsp_socp_set_dec_pkt_lgth(pPktlgth);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_SET_DEBUG
 功能描述  : 设置解码源通道的debug模式
 输入参数  : u32ChanID:通道ID
             u32DebugEn: debug标识
 输出参数  : 无。
 返 回 值  : SOCP_OK:设置成功。
             其他值:设置失败
*****************************************************************************/
BSP_S32 DRV_SOCP_SET_DEBUG(BSP_U32 u32DestChanID, BSP_U32 u32DebugEn)
{
    return bsp_socp_set_debug(u32DestChanID, u32DebugEn);
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
 函 数 名  : bsp_socp_register_read_cb
 功能描述  : 该接口用于注册读数据的回调函数。
 输入参数  : u32DestChanID:目标通道ID
             ReadCB: 事件回调函数
 输出参数  : 无。
 返 回 值  : SOCP_OK:注册读数据回调函数成功
             SOCP_ERROR:注册读数据回调函数失败
*****************************************************************************/
BSP_S32 DRV_SOCP_REGISTER_READ_CB( BSP_U32 u32DestChanID, socp_read_cb ReadCB)
{
    return bsp_socp_register_read_cb(u32DestChanID, ReadCB);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_GET_READ_BUFF
 功能描述  : 该此接口用于获取读数据缓冲区指针。
 输入参数  : u32DestChanID:目标通道ID
             ReadCB: 读数据buffer
 输出参数  : 无。
 返 回 值  : SOCP_OK:获取读数据缓冲区成功。
             SOCP_ERROR:获取读数据缓冲区成功。
*****************************************************************************/
BSP_S32 DRV_SOCP_GET_READ_BUFF(BSP_U32 u32DestChanID,SOCP_BUFFER_RW_STRU *pBuffer)
{
    return bsp_socp_get_read_buff(u32DestChanID, pBuffer);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_READ_DATA_DONE
 功能描述  : 该接口用于上层告诉SOCP驱动，从目标通道中读走的实际数据。
 输入参数  : u32DestChanID:目标通道ID
             u32ReadSize: 已读出数据的长度
 输出参数  : 无。
 返 回 值  : SOCP_OK:读数据成功。
             SOCP_ERROR:读数据失败
*****************************************************************************/
BSP_S32 DRV_SOCP_READ_DATA_DONE(BSP_U32 u32DestChanID,BSP_U32 u32ReadSize)
{
    return bsp_socp_read_data_done(u32DestChanID, u32ReadSize);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_SET_BBP_ENABLE
 功能描述  : 使能或停止BBP通道。
 输入参数  : bEnable:通道ID
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                   其他值:设置失败
*****************************************************************************/
BSP_S32 DRV_SOCP_SET_BBP_ENABLE(int bEnable)
{
    return bsp_socp_set_bbp_enable(bEnable);
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_SET_BBP_DS_MODE
 功能描述  : 设置BBP DS通道数据溢出处理模式。
 输入参数  : eDsMode:DS通道数据溢出时处理模式设置
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                   其他值:设置失败
*****************************************************************************/
BSP_S32 DRV_SOCP_SET_BBP_DS_MODE(SOCP_BBP_DS_MODE_ENUM_UIN32 eDsMode)
{
    return bsp_socp_set_bbp_ds_mode(eDsMode);
}

/*****************************************************************************
* 函 数 名  : DRV_SOCP_GET_STATE
*
* 功能描述  : 获取SOCP状态
*
* 返 回 值  : SOCP_IDLE    空闲
*             SOCP_BUSY    忙碌
*****************************************************************************/
SOCP_STATE_ENUM_UINT32 DRV_SOCP_GET_STATE()
{
    return bsp_socp_get_state();
}

/*****************************************************************************
* 函 数 名  : bsp_socp_enc_dst_buf_flush
* 功能描述  : SOCP编码目的buffer缓存数据立即输出
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/

BSP_S32 DRV_SOCP_ENC_DST_BUF_FLUSH(void)
{
	return bsp_socp_enc_dst_buf_flush();
}

/*****************************************************************************
* 函 数 名  : DRV_SOCP_VOTE
* 功能描述  : SOCP投票接口，根据投票结果决定SOCP是否睡眠，该接口只在A核提供
* 输入参数  : id --- 投票组件ID，type --- 投票类型
* 输出参数  : 无
* 返 回 值  : BSP_S32 0 --- 投票成功，0xFFFFFFFF --- 投票失败
*****************************************************************************/
BSP_S32 DRV_SOCP_VOTE(SOCP_VOTE_ID_ENUM_U32 id, SOCP_VOTE_TYPE_ENUM_U32 type)
{
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    return bsp_socp_vote(id, type);
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


