

#ifndef _BSP_SOCP_H
#define _BSP_SOCP_H

#include "osl_common.h"
#include "drv_socp.h"
#include "bsp_om.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************
  错误码定义
**************************************************************************/
#define BSP_ERR_SOCP_BASE            BSP_DEF_ERR(BSP_MODU_SOCP, 0)
#define BSP_ERR_SOCP_NULL            (BSP_ERR_SOCP_BASE + 0x1)
#define BSP_ERR_SOCP_NOT_INIT        (BSP_ERR_SOCP_BASE + 0x2)
#define BSP_ERR_SOCP_MEM_ALLOC       (BSP_ERR_SOCP_BASE + 0x3)
#define BSP_ERR_SOCP_SEM_CREATE      (BSP_ERR_SOCP_BASE + 0x4)
#define BSP_ERR_SOCP_TSK_CREATE      (BSP_ERR_SOCP_BASE + 0x5)
#define BSP_ERR_SOCP_INVALID_CHAN    (BSP_ERR_SOCP_BASE + 0x6)
#define BSP_ERR_SOCP_INVALID_PARA    (BSP_ERR_SOCP_BASE + 0x7)
#define BSP_ERR_SOCP_NO_CHAN         (BSP_ERR_SOCP_BASE + 0x8)
#define BSP_ERR_SOCP_SET_FAIL        (BSP_ERR_SOCP_BASE + 0x9)
#define BSP_ERR_SOCP_TIMEOUT         (BSP_ERR_SOCP_BASE + 0xa)
#define BSP_ERR_SOCP_NOT_8BYTESALIGN (BSP_ERR_SOCP_BASE + 0xb)
#define BSP_ERR_SOCP_CHAN_RUNNING    (BSP_ERR_SOCP_BASE + 0xc)
#define BSP_ERR_SOCP_CHAN_MODE       (BSP_ERR_SOCP_BASE + 0xd)
#define BSP_ERR_SOCP_DEST_CHAN       (BSP_ERR_SOCP_BASE + 0xe)
#define BSP_ERR_SOCP_DECSRC_SET      (BSP_ERR_SOCP_BASE + 0xf)

#if (FEATURE_SOCP_DECODE_INT_TIMEOUT == FEATURE_ON)
typedef enum timeout_module
{
    DECODE_TIMEOUT_INT_TIMEOUT = 0,
    DECODE_TIMEOUT_DEC_INT_TIMEOUT = 1,
    DECODE_TIMEOUT_BUTTON = 2,

} DECODE_TIMEOUT_MODULE;

#endif

typedef struct
{
    SOCP_VOTE_TYPE_ENUM_U32 type;
}socp_vote_req_stru;

typedef struct
{
    u32 vote_rst;   /* 1:成功，非1:失败 */
}socp_vote_cnf_stru;

/*****************************************************************************
* 函 数 名  : socp_init
*
* 功能描述  : 模块初始化函数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功的标识码
*****************************************************************************/
s32 socp_init(void);

/*****************************************************************************
 函 数 名  : bsp_socp_coder_set_src_chan
 功能描述  : 此接口完成SOCP编码器源通道的分配，根据编码器源通道参数设置通道属性，连接目标通道，返回函数执行结果。
 输入参数  : pSrcAttr:编码器源通道参数结构体指针。
             pSrcChanID:申请到的源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码源通道分配成功。
             SOCP_ERROR:编码源通道分配失败。
*****************************************************************************/
s32 bsp_socp_coder_set_src_chan(SOCP_CODER_SRC_ENUM_U32 enSrcChanID, SOCP_CODER_SRC_CHAN_STRU *pSrcAttr);

/*****************************************************************************
 函 数 名  : bsp_socp_coder_set_dest_chan_attr
 功能描述  : 此接口完成某一编码目标通道的配置，返回函数执行的结果。
 输入参数  : u32DestChanID:SOCP编码器的目标通道ID。
             pDestAttr:SOCP编码器目标通道参数结构体指针。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码目的通道设置成功。
             SOCP_ERROR:编码目的通道设置失败。
*****************************************************************************/
s32 bsp_socp_coder_set_dest_chan_attr(u32 u32DestChanID, SOCP_CODER_DEST_CHAN_STRU *pDestAttr);

/*****************************************************************************
 函 数 名      : bsp_socp_decoder_set_dest_chan
 功能描述  :此接口完成SOCP解码器目标通道的分配，
                根据解码目标通道参数设置通道属性，
                并连接源通道，返回函数执行结果。
 输入参数  : pAttr:解码器目标通道参数结构体指针
                         pDestChanID:申请到的目标通道ID
 输出参数  : 无。
 返 回 值      : SOCP_OK:解码目的通道分配成功。
                             SOCP_ERROR:解码目的通道设置失败。
*****************************************************************************/
s32 bsp_socp_decoder_set_dest_chan(SOCP_DECODER_DST_ENUM_U32 enDestChanID, SOCP_DECODER_DEST_CHAN_STRU *pAttr);

/*****************************************************************************
 函 数 名      : bsp_socp_decoder_set_src_chan_attr
 功能描述  :此接口完成某一解码源通道的配置，返回函数执行的结果。
 输入参数  : u32SrcChanID:解码器源通道ID
                         pInputAttr:解码器源通道参数结构体指针
 输出参数  : 无。
 返 回 值      : SOCP_OK:解码源通道设置成功
                             SOCP_ERROR:解码源通道设置失败
*****************************************************************************/
s32 bsp_socp_decoder_set_src_chan_attr ( u32 u32SrcChanID,SOCP_DECODER_SRC_CHAN_STRU *pInputAttr);

/*****************************************************************************
 函 数 名      : bsp_socp_decoder_get_err_cnt
 功能描述  :此接口给出解码通道中四种异常情况的计数值。
 输入参数  : u32ChanID:解码器通道ID
                         pErrCnt:解码器异常计数结构体指针
 输出参数  : 无。
 返 回 值      : SOCP_OK:返回异常计数成功
                             SOCP_ERROR:返回异常计数失败
*****************************************************************************/
s32 bsp_socp_decoder_get_err_cnt (u32 u32ChanID, SOCP_DECODER_ERROR_CNT_STRU *pErrCnt);

/*****************************************************************************
 函 数 名  : bsp_socp_free_channel
 功能描述  : 此接口根据通道ID释放分配的编解码通道。
 输入参数  : u32ChanID:通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:通道释放成功。
             SOCP_ERROR:通道释放失败。
*****************************************************************************/
s32 bsp_socp_free_channel(u32 u32ChanID);

/*****************************************************************************
* 函 数 名  : socp_clean_encsrc_chan
*
* 功能描述  : 清空编码源通道，同步V9 SOCP接口
*
* 输入参数  : enSrcChanID       编码通道号
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/
u32 bsp_socp_clean_encsrc_chan(SOCP_CODER_SRC_ENUM_U32 enSrcChanID);

/*****************************************************************************
 函 数 名  : bsp_socp_register_event_cb
 功能描述  : 此接口为给定通道注册事件回调函数。
 输入参数  : u32ChanID:通道ID。
             EventCB:事件回调函数，参考socp_event_cb函数定义
 输出参数  : 无。
 返 回 值  : SOCP_OK:注册事件回调函数成功。
             SOCP_ERROR:注册事件回调函数失败。
*****************************************************************************/
s32 bsp_socp_register_event_cb(u32 u32ChanID, socp_event_cb EventCB);

/*****************************************************************************
 函 数 名  : bsp_socp_start
 功能描述  : 此接口用于源通道，启动编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码启动成功。
             SOCP_ERROR:编码或解码启动失败。
*****************************************************************************/
s32 bsp_socp_start(u32 u32SrcChanID);

/*****************************************************************************
 函 数 名  : bsp_socp_stop
 功能描述  : 此接口用于源通道，停止编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码停止成功。
             SOCP_ERROR:编码或解码停止失败。
*****************************************************************************/
s32 bsp_socp_stop(u32 u32SrcChanID);

/*****************************************************************************
 函 数 名      : bsp_socp_set_timeout
 功能描述  :此接口设置超时阈值。
 输入参数  : u32Timeout:超时阈值

 输出参数  : 无。
 返 回 值      : SOCP_OK:设置超时时间阈值成功。
                             SOCP_ERROR:设置超时时间阈值失败
*****************************************************************************/
s32 bsp_socp_set_timeout (SOCP_TIMEOUT_EN_ENUM_UIN32 eTmOutEn, u32 u32Timeout);

/*****************************************************************************
 函 数 名   : bsp_socp_set_dec_pkt_lgth
 功能描述  :设置解码包长度极限值
 输入参数  : pPktlgth:解码包长度极值

 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                    其他值:设置失败
*****************************************************************************/
s32 bsp_socp_set_dec_pkt_lgth(SOCP_DEC_PKTLGTH_STRU *pPktlgth);

/*****************************************************************************
 函 数 名   : bsp_socp_set_debug
 功能描述  :设置解码源通道的debug模式
 输入参数  : u32ChanID:通道ID
                  u32DebugEn: debug标识
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                     其他值:设置失败
*****************************************************************************/
s32 bsp_socp_set_debug(u32 u32DestChanID, u32 u32DebugEn);

/*****************************************************************************
 函 数 名   : bsp_socp_chan_soft_reset
 功能描述  : 源通道软复位
 输入参数  : u32ChanID:通道ID
 输出参数  : 无。
 返 回 值      : SOCP_OK:设复位成功。
                     其他值:复位失败
*****************************************************************************/
s32 bsp_socp_chan_soft_reset(u32 u32ChanID);

/*****************************************************************************
 函 数 名      : bsp_socp_get_write_buff
 功能描述  :此接口用于获取写数据buffer。
 输入参数  : u32SrcChanID:源通道ID
                  pBuff:           :写数据buffer

 输出参数  : 无。
 返 回 值      : SOCP_OK:获取写数据buffer成功。
                             SOCP_ERROR:获取写数据buffer失败
*****************************************************************************/
s32 bsp_socp_get_write_buff( u32 u32SrcChanID, SOCP_BUFFER_RW_STRU *pBuff);

/*****************************************************************************
 函 数 名      : bsp_socp_write_done
 功能描述  :该接口用于数据的写操作，提供写入数据的长度。
 输入参数  : u32SrcChanID:源通道ID
                  u32WrtSize:   已写入数据的长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:写入数据成功。
                             SOCP_ERROR:写入数据失败
*****************************************************************************/
s32 bsp_socp_write_done(u32 u32SrcChanID, u32 u32WrtSize);

/*****************************************************************************
 函 数 名      : bsp_socp_register_rd_cb
 功能描述  :该接口用于注册从RD缓冲区中读取数据的回调函数。
 输入参数  : u32SrcChanID:源通道ID
                  RdCB:  事件回调函数
 输出参数  : 无。
 返 回 值      : SOCP_OK:注册RD环形缓冲区读数据回调函数成功。
                             SOCP_ERROR:注册RD环形缓冲区读数据回调函数失败
*****************************************************************************/
s32 bsp_socp_register_rd_cb(u32 u32SrcChanID, socp_rd_cb RdCB);

/*****************************************************************************
 函 数 名      : bsp_socp_get_rd_buffer
 功能描述  :该此接口用于获取RD buffer的数据指针。
 输入参数  : u32SrcChanID:源通道ID
                  pBuff:  RD buffer
 输出参数  : 无。
 返 回 值      : SOCP_OK:获取RD环形缓冲区成功
                             SOCP_ERROR:获取RD环形缓冲区失败
*****************************************************************************/
s32 bsp_socp_get_rd_buffer( u32 u32SrcChanID,SOCP_BUFFER_RW_STRU *pBuff);

/*****************************************************************************
 函 数 名      : bsp_socp_read_rd_done
 功能描述  :此接口用于上层通知SOCP驱动，从RD buffer中实际读取的数据。
 输入参数  : u32SrcChanID:源通道ID
                  u32RDSize:  从RD buffer中实际读取的数据长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:读取RDbuffer中的数据成功
                             SOCP_ERROR:读取RDbuffer中的数据失败
*****************************************************************************/
s32 bsp_socp_read_rd_done(u32 u32SrcChanID, u32 u32RDSize);

/*****************************************************************************
 函 数 名      : bsp_socp_register_read_cb
 功能描述  :该接口用于注册读数据的回调函数。
 输入参数  : u32DestChanID:目标通道ID
                  ReadCB: 事件回调函数
 输出参数  : 无。
 返 回 值      : SOCP_OK:注册读数据回调函数成功
                             SOCP_ERROR:注册读数据回调函数失败
*****************************************************************************/
s32 bsp_socp_register_read_cb( u32 u32DestChanID, socp_read_cb ReadCB);

/*****************************************************************************
 函 数 名      : bsp_socp_register_read_cb
 功能描述  :该此接口用于获取读数据缓冲区指针。
 输入参数  : u32DestChanID:目标通道ID
                  ReadCB: 读数据buffer
 输出参数  : 无。
 返 回 值      : SOCP_OK:获取读数据缓冲区成功。
                             SOCP_ERROR:获取读数据缓冲区成功。
*****************************************************************************/
s32 bsp_socp_get_read_buff(u32 u32DestChanID,SOCP_BUFFER_RW_STRU *pBuffer);

/*****************************************************************************
 函 数 名      : bsp_socp_read_data_done
 功能描述  :该接口用于上层告诉SOCP驱动，从目标通道中读走的实际数据。
 输入参数  : u32DestChanID:目标通道ID
                  u32ReadSize: 已读出数据的长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:读数据成功。
                             SOCP_ERROR:读数据失败
*****************************************************************************/
s32 bsp_socp_read_data_done(u32 u32DestChanID,u32 u32ReadSize);

/*****************************************************************************
 函 数 名      : bsp_socp_set_bbp_enable
 功能描述  :使能或停止BBP通道。
 输入参数  : bEnable:通道ID
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                   其他值:设置失败
*****************************************************************************/
s32 bsp_socp_set_bbp_enable(int bEnable);

/*****************************************************************************
 函 数 名      : bsp_socp_set_bbp_ds_mode
 功能描述  :设置BBP DS通道数据溢出处理模式。
 输入参数  : eDsMode:DS通道数据溢出时处理模式设置
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                   其他值:设置失败
*****************************************************************************/
s32 bsp_socp_set_bbp_ds_mode(SOCP_BBP_DS_MODE_ENUM_UIN32 eDsMode);

/*****************************************************************************
* 函 数 名  : bsp_socp_start_dsp
* 功能描述  :enable DSP channel
* 输入参数  :
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
void bsp_socp_start_dsp(void);

/*****************************************************************************
* 函 数 名  : bsp_socp_stop_dsp
* 功能描述  :disable DSP channel
* 输入参数  :
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
void bsp_socp_stop_dsp(void);

/*****************************************************************************
* 函 数 名  : bsp_socp_get_state
*
* 功能描述  : 获取SOCP状态
*
* 返 回 值  : SOCP_IDLE    空闲
*             SOCP_BUSY    忙碌
*****************************************************************************/
SOCP_STATE_ENUM_UINT32 bsp_socp_get_state(void);

/*****************************************************************************
* 函 数 名  : bsp_socp_enc_dst_buf_flush
* 功能描述  : SOCP编码目的buffer缓存数据立即输出
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
s32 bsp_socp_enc_dst_buf_flush(void);

s32 socp_init(void);

/*****************************************************************************
* 函 数 名  : bsp_socp_vote
* 功能描述  : SOCP投票接口，根据投票结果决定SOCP是否睡眠，该接口只在A核提供
* 输入参数  : id --- 投票组件ID，type --- 投票类型
* 输出参数  : 无
* 返 回 值  : BSP_S32 0 --- 投票成功，0xFFFFFFFF --- 投票失败
*****************************************************************************/
BSP_S32 bsp_socp_vote(SOCP_VOTE_ID_ENUM_U32 id, SOCP_VOTE_TYPE_ENUM_U32 type);

/*****************************************************************************
* 函 数 名  : bsp_socp_vote_to_mcore
* 功能描述  : SOCP投票接口，该接口只在C核提供，适配LDSP首次加载的SOCP上电需求
* 输入参数  : type --- 投票类型
* 输出参数  : 无
* 返 回 值  : BSP_S32 0 --- 投票成功，0xFFFFFFFF --- 投票失败
*****************************************************************************/
BSP_S32 bsp_socp_vote_to_mcore(SOCP_VOTE_TYPE_ENUM_U32 type);

#if (FEATURE_SOCP_DECODE_INT_TIMEOUT == FEATURE_ON)
/*****************************************************************************
* 函 数 名  : bsp_socp_set_decode_timeout_register
* 功能描述  :选择解码超时寄存器
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/


s32 bsp_socp_set_decode_timeout_register(DECODE_TIMEOUT_MODULE module);
#endif 
#ifdef __cplusplus
}
#endif

#endif /* end of _BSP_SOCP_H*/


