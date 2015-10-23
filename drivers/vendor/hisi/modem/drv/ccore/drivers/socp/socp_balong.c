
/*lint -save -e537*/
#include <stdio.h>
#include <taskLib.h>
#include <intLib.h>
#include <logLib.h>
#include <cacheLib.h>
#include <semLib.h>
#include "osl_sem.h"
#include "bsp_version.h"
#include "bsp_icc.h"
#include "socp_balong.h"

/*lint -restore*/
/*lint -save -e506 -e40 -e63 -e516 */
#ifdef __cplusplus
extern "C" {
#endif

SOCP_GBL_STATE g_strSocpStat = {0};
SOCP_DEBUG_INFO_S g_stSocpDebugInfo;
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
osl_sem_id g_st_socp_vote_sem;
u32 g_ul_socp_vote_rst = 0;
#endif
/*****************************************************************************
* 函 数 名  : socp_get_idle_buffer
*
* 功能描述  : 查询空闲缓冲区
*
* 输入参数  :  pRingBuffer       待查询的环形buffer
                    pRWBuffer         输出的环形buffer
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
/*lint -save -e101 -e550 */
void socp_get_idle_buffer(SOCP_RING_BUF_S *pRingBuffer, SOCP_BUFFER_RW_S *pRWBuffer)
{
    if (pRingBuffer->u32Write < pRingBuffer->u32Read)
    {
        /* 读指针大于写指针，直接计算 */
        pRWBuffer->pBuffer   = (char *)(pRingBuffer->u32Write);
        pRWBuffer->u32Size   = (u32)(pRingBuffer->u32Read - pRingBuffer->u32Write - 1);
        pRWBuffer->pRbBuffer = NULL;
        pRWBuffer->u32RbSize = 0;
    }
    else
    {
        /* 写指针大于读指针，需要考虑回卷 */
        if (pRingBuffer->u32Read != pRingBuffer->u32Start)
        {
            pRWBuffer->pBuffer   = (char *)(pRingBuffer->u32Write);
            pRWBuffer->u32Size   = (u32)(pRingBuffer->u32End - pRingBuffer->u32Write + 1);
            pRWBuffer->pRbBuffer = (char *)(pRingBuffer->u32Start);
            pRWBuffer->u32RbSize = (u32)(pRingBuffer->u32Read - pRingBuffer->u32Start - 1);
        }
        else
        {
            pRWBuffer->pBuffer   = (char *)(pRingBuffer->u32Write);
            pRWBuffer->u32Size   = (u32)(pRingBuffer->u32End - pRingBuffer->u32Write);
            pRWBuffer->pRbBuffer = NULL;
            pRWBuffer->u32RbSize = 0;
        }
    }

    return;
}

/*****************************************************************************
* 函 数 名  : socp_get_data_buffer
*
* 功能描述  : 获取空闲缓冲区的数据
*
* 输入参数  :  pRingBuffer       待查询的环形buffer
                    pRWBuffer         输出的环形buffer
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
void socp_get_data_buffer(SOCP_RING_BUF_S *pRingBuffer, SOCP_BUFFER_RW_S *pRWBuffer)
{
    if (pRingBuffer->u32Read <= pRingBuffer->u32Write)
    {
        /* 写指针大于读指针，直接计算 */
        pRWBuffer->pBuffer   = (char *)(pRingBuffer->u32Read);
        pRWBuffer->u32Size   = (u32)(pRingBuffer->u32Write - pRingBuffer->u32Read);
        pRWBuffer->pRbBuffer = NULL;
        pRWBuffer->u32RbSize = 0;
    }
    else
    {
        /* 读指针大于写指针，需要考虑回卷 */
        pRWBuffer->pBuffer   = (char *)(pRingBuffer->u32Read);
        pRWBuffer->u32Size   = (u32)(pRingBuffer->u32End - pRingBuffer->u32Read + 1);
        pRWBuffer->pRbBuffer = (char *)(pRingBuffer->u32Start);
        pRWBuffer->u32RbSize = (u32)(pRingBuffer->u32Write - pRingBuffer->u32Start);
    }

    return;
}

/*****************************************************************************
* 函 数 名  : socp_write_done
*
* 功能描述  : 更新缓冲区的写指针
*
* 输入参数  :  pRingBuffer       待更新的环形buffer
                    u32Size          更新的数据长度
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
void socp_write_done(SOCP_RING_BUF_S *pRingBuffer, u32 u32Size)
{
    u32 tmp_size;
    /*lint -save -e574*/
    tmp_size = pRingBuffer->u32End - pRingBuffer->u32Write + 1;
    if (tmp_size > u32Size)
    {
    /*lint -restore +e574*/

        pRingBuffer->u32Write += u32Size;
    }
    else
    {
        u32 rb_size = u32Size - tmp_size;
        pRingBuffer->u32Write = pRingBuffer->u32Start + rb_size;
    }

    return;
}

/*****************************************************************************
* 函 数 名  : socp_read_done
*
* 功能描述  : 更新缓冲区的读指针
*
* 输入参数  :  pRingBuffer       待更新的环形buffer
                    u32Size          更新的数据长度
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
void socp_read_done(SOCP_RING_BUF_S *pRingBuffer, u32 u32Size)
{
    pRingBuffer->u32Read += u32Size;
    if (pRingBuffer->u32Read > pRingBuffer->u32End)
    {
        pRingBuffer->u32Read -= pRingBuffer->u32Length;
    }
}

/*****************************************************************************
* 函 数 名  : bsp_socp_clean_encsrc_chan
*
* 功能描述  : 清空编码源通道，同步V9 SOCP接口
*
* 输入参数  : enSrcChanID       编码通道号
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/
u32 bsp_socp_clean_encsrc_chan(SOCP_CODER_SRC_ENUM_U32 enSrcChanID)
{
    u32 ulResetFlag;
    u32 i;
    u32 ulChanID;
    u32 ulChanType;

    ulChanID    = SOCP_REAL_CHAN_ID(enSrcChanID);
    ulChanType  = SOCP_REAL_CHAN_TYPE(enSrcChanID);

    SOCP_CHECK_CHAN_TYPE(ulChanType, SOCP_CODER_SRC_CHAN);
    SOCP_CHECK_ENCSRC_CHAN_ID(ulChanID);

    /* 复位通道 */
    SOCP_REG_SETBITS(SOCP_REG_ENCRST, ulChanID, 1, 1);

    /* 等待通道自清 */
    for(i=0; i< SOCP_RESET_TIME; i++)
    {
        ulResetFlag = SOCP_REG_GETBITS(SOCP_REG_ENCRST, ulChanID, 1);

        if(0 == ulResetFlag)
        {
            break;
        }
    }

    if(SOCP_RESET_TIME == i)
    {
        printf("SocpCleanEncChan failed!\n");
    }

    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : socp_reset_dsp_log_chan
*
* 功能描述  : 复位DSP Log通道
*
* 输入参数  : u32ChanID 通道ID
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
s32 socp_reset_dsp_log_chan(u32 u32ChanID)
{
    u32  ResetFlag;
    u32  j;
    u32  u32FixChanId = 0;
    SOCP_ENCSRC_CHAN_S *pChan;

    u32FixChanId = u32ChanID;
    pChan = &g_strSocpStat.sEncSrcChan[u32FixChanId];

    /* 复位通道 */
    SOCP_REG_SETBITS(SOCP_REG_ENCRST, SOCP_DSPLOG_CHN, 1, 1);

    /* 等待通道自清 */
    for(j=0; j<SOCP_RESET_TIME; j++)
    {
        ResetFlag = SOCP_REG_GETBITS(SOCP_REG_ENCRST, SOCP_DSPLOG_CHN, 1);
        if(0 == ResetFlag)
        {
            break;
        }

        if((SOCP_RESET_TIME -1) == j)
        {
            printk("socp_reset_dsp_log_chan 0x%x failed!\n", SOCP_DSPLOG_CHN);
        }
    }

    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(SOCP_DSPLOG_CHN),(u32)SOCP_DSPLOG_CHN_ADDR);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(SOCP_DSPLOG_CHN),(u32)SOCP_DSPLOG_CHN_ADDR);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(SOCP_DSPLOG_CHN), (u32)SOCP_DSPLOG_CHN_ADDR);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(SOCP_DSPLOG_CHN), 0, 27, SOCP_DSPLOG_CHN_SIZE);
	SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(SOCP_DSPLOG_CHN), 27, 5, 0);

    /*配置SOCP 参数*/
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 1, 2, SOCP_ENCSRC_CHNMODE_CTSPACKET);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 4, 4, SOCP_DSPLOG_DST_BUFID);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 8, 2, SOCP_CHAN_PRIORITY_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 10, 1, SOCP_ENCSRC_BYPASS_DISABLE);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 16, 8, SOCP_DATA_TYPE_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 11, 1, SOCP_DATA_TYPE_EN);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 31, 1, SOCP_ENC_DEBUG_DIS);

    /*使能通道*/
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 0, 1, 1);

    /* 保存参数到全局变量*/
    pChan->sEncSrcBuf.u32Read      = (u32)SOCP_DSPLOG_CHN_ADDR;
    pChan->sEncSrcBuf.u32Write     = (u32)SOCP_DSPLOG_CHN_ADDR;
    pChan->u32ChanEn               = SOCP_CHN_ENABLE;

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : socp_reset_bbp_log_chan
*
* 功能描述  : 复位BBP Log通道
*
* 输入参数  : u32ChanID 通道ID
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
s32 socp_reset_bbp_log_chan(u32 u32ChanID)
{
    u32  ResetFlag;
    u32  j;
    u32  u32BbpLogAddr;
    u32  u32FixChanId = 0;
    SOCP_ENCSRC_CHAN_S *pChan;

    u32FixChanId = u32ChanID;
    pChan = &g_strSocpStat.sEncSrcChan[u32FixChanId];

    /* 复位通道 */
    SOCP_REG_SETBITS(SOCP_REG_ENCRST, u32ChanID, 1, 1);
    /* 等待通道自清 */
    for(j=0; j<SOCP_RESET_TIME; j++)
    {
        ResetFlag = SOCP_REG_GETBITS(SOCP_REG_ENCRST, u32ChanID, 1);
        if(0 == ResetFlag)
        {
            break;
        }
        if((SOCP_RESET_TIME -1) == j)
        {
            printk("socp_reset_bbp_log_chan 0x%x failed!\n", u32ChanID);
        }
    }

    BBP_REG_READ(BBP_REG_LOG_ADDR((u32ChanID - SOCP_FIXCHN_BASE)),u32BbpLogAddr);

    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(u32ChanID),u32BbpLogAddr);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(u32ChanID),u32BbpLogAddr);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(u32ChanID), u32BbpLogAddr);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(u32ChanID), 0, 27, SOCP_BBPLOG_CHN_SIZE);
	SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(u32ChanID), 27, 5, 0);

    /*配置SOCP 参数*/
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 1, 2, SOCP_ENCSRC_CHNMODE_CTSPACKET);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 4, 4, SOCP_BBPLOG_DST_BUFID);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 8, 2, SOCP_CHAN_PRIORITY_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 10, 1, SOCP_ENCSRC_BYPASS_DISABLE);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 16, 8, SOCP_DATA_TYPE_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 11, 1, SOCP_DATA_TYPE_EN);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 31, 1, SOCP_ENC_DEBUG_DIS);
    /* 使能通道*/
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 0, 1, 1);

    /* 保存全局变量参数*/
    pChan->sEncSrcBuf.u32Write     = u32BbpLogAddr;
    pChan->sEncSrcBuf.u32Read      = u32BbpLogAddr;
    pChan->u32ChanEn               = SOCP_CHN_ENABLE;

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : socp_reset_bbp_ds_chan
*
* 功能描述  : 复位BBP Ds通道
*
* 输入参数  : u32ChanID 通道ID
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
s32 socp_reset_bbp_ds_chan(u32 u32ChanId)
{
    u32  ResetFlag;
    u32  j;
    u32  u32FixChanId = 0;
    SOCP_ENCSRC_CHAN_S *pChan;

    u32FixChanId = u32ChanId;
    pChan = &g_strSocpStat.sEncSrcChan[u32FixChanId];

    /* 复位通道 */
    SOCP_REG_SETBITS(SOCP_REG_ENCRST, SOCP_BBPDS_CHN, 1, 1);
    /* 等待通道自清 */
    for(j=0; j<SOCP_RESET_TIME; j++)
    {
        ResetFlag = SOCP_REG_GETBITS(SOCP_REG_ENCRST, SOCP_BBPDS_CHN, 1);
        if(0 == ResetFlag)
        {
            break;
        }
        if((SOCP_RESET_TIME -1) == j)
        {
            printk("socp_reset_bbp_ds_chan 0x%x failed!\n", SOCP_BBPDS_CHN);
        }
    }

    /* 配置BBP的数采通道 */
    BBP_REG_WRITE(BBP_REG_DS_ADDR,SOCP_BBPDS_CHN_ADDR);
    BBP_REG_SETBITS(BBP_REG_DS_CFG, 31, 1, SOCP_BBP_DS_MODE_OVERRIDE);
    BBP_REG_SETBITS(BBP_REG_DS_CFG, 0, 28, SOCP_BBPDS_CHN_SIZE);

    /* 配置SOCP 参数*/
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(SOCP_BBPDS_CHN),SOCP_BBPDS_CHN_ADDR);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(SOCP_BBPDS_CHN),SOCP_BBPDS_CHN_ADDR);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(SOCP_BBPDS_CHN), SOCP_BBPDS_CHN_ADDR);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(SOCP_BBPDS_CHN), 0, 27, SOCP_BBPDS_CHN_SIZE);
	SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(SOCP_BBPDS_CHN), 27, 5, 0);

    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_BBPDS_CHN), 1, 2, SOCP_ENCSRC_CHNMODE_FIXPACKET);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_BBPDS_CHN), 4, 4, SOCP_BBPDS_DST_BUFID);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_BBPDS_CHN), 8, 2, SOCP_CHAN_PRIORITY_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_BBPDS_CHN), 10, 1, SOCP_ENCSRC_BYPASS_DISABLE);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_BBPDS_CHN), 16, 8, SOCP_DATA_TYPE_0);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_BBPDS_CHN), 11, 1, SOCP_DATA_TYPE_EN);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_BBPDS_CHN), 31, 1, SOCP_ENC_DEBUG_DIS);

    pChan->sEncSrcBuf.u32Write     = SOCP_BBPDS_CHN_ADDR;
    pChan->sEncSrcBuf.u32Read      = SOCP_BBPDS_CHN_ADDR;

    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : socp_reset_enc_chan
*
* 功能描述  : 复位编码通道
*
* 输入参数  : u32ChanID       编码通道号
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
s32 socp_reset_enc_chan(u32 u32ChanID)
{
    u32 ResetFlag;
    u32 i;
    SOCP_ENCSRC_CHAN_S *pChan;

    pChan = &g_strSocpStat.sEncSrcChan[u32ChanID];

    /* 复位通道 */
    SOCP_REG_SETBITS(SOCP_REG_ENCRST, u32ChanID, 1, 1);

    /* 等待通道自清 */
    for (i = 0; i < SOCP_RESET_TIME; i++)
    {
        ResetFlag = SOCP_REG_GETBITS(SOCP_REG_ENCRST, u32ChanID, 1);
        if (0 == ResetFlag)
        {
            break;
        }

        if ((SOCP_RESET_TIME - 1) == i)
        {
            printf("socp_reset_enc_chan 0x%x failed!\n", u32ChanID);
        }
    }

    /* 重新配置通道*/
    /* 写入起始地址到源buffer起始地址寄存器*/
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(u32ChanID), pChan->sEncSrcBuf.u32Start);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(u32ChanID), pChan->sEncSrcBuf.u32Start);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(u32ChanID), pChan->sEncSrcBuf.u32Start);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(u32ChanID), 0, 27, pChan->sEncSrcBuf.u32Length);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(u32ChanID), 27, 5, 0);

    /* 更新读写指针*/
    g_strSocpStat.sEncSrcChan[u32ChanID].sEncSrcBuf.u32Read  = pChan->sEncSrcBuf.u32Start;
    g_strSocpStat.sEncSrcChan[u32ChanID].sEncSrcBuf.u32Write = pChan->sEncSrcBuf.u32Start;

    /* 如果是用链表缓冲区，则配置RDbuffer的起始地址和长度 */
    if (SOCP_ENCSRC_CHNMODE_LIST == pChan->eChnMode)
    {
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQADDR(u32ChanID), pChan->sRdBuf.u32Start);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQRPTR(u32ChanID), pChan->sRdBuf.u32Start);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQWPTR(u32ChanID), pChan->sRdBuf.u32Start);
        SOCP_REG_SETBITS(SOCP_REG_ENCSRC_RDQCFG(u32ChanID), 0, 16, pChan->sRdBuf.u32Length);
		SOCP_REG_SETBITS(SOCP_REG_ENCSRC_RDQCFG(u32ChanID), 16, 16, 0);

        g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf.u32Read  = pChan->sRdBuf.u32Start;
        g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf.u32Write = pChan->sRdBuf.u32Start;
    }

    /*配置其它参数*/
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 1, 2, pChan->eChnMode);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 4, 4, pChan->u32DestChanID);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 8, 2, pChan->ePriority);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 10, 1, pChan->u32BypassEn);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 16, 8, pChan->eDataType);

    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 11, 1, pChan->eDataTypeEn);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 31, 1, pChan->eDebugEn);

    /*如果通道是启动状态，使能通道*/
    if (pChan->u32ChanEn)
    {
        SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32ChanID), 0, 1, 1);
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : socp_get_enc_rd_size
*
* 功能描述  :  获取编码源通道RDbuffer
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
u32 socp_get_enc_rd_size(u32 u32ChanID)
{
    SOCP_BUFFER_RW_S Buff;

    /* 根据读写指针获取buffer */
    SOCP_REG_READ(SOCP_REG_ENCSRC_RDQRPTR(u32ChanID), g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf.u32Read);
    SOCP_REG_READ(SOCP_REG_ENCSRC_RDQWPTR(u32ChanID), g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf.u32Write);

    socp_get_data_buffer(&g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf, &Buff);
    return (Buff.u32Size + Buff.u32RbSize);
}

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
/*****************************************************************************
* 函 数 名  : socp_icc_read_cb
*
* 功能描述  : icc通道读回调函数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
s32 socp_icc_read_cb(u32 id, u32 len, void * context)
{
	s32 ret = 0;
    socp_vote_cnf_stru cnf_data;

	if((len == 0) || (len > sizeof(socp_vote_cnf_stru)))
	{
		printf("%s: read data len 0x%x err\n", __FUNCTION__, len);
        return BSP_ERROR;
	}

	ret = bsp_icc_read(id, (u8 *)&cnf_data, len);
	if(len != (u32)ret)/*lint !e737*/
	{
		printf("%s: read data len 0x%x not eq to 0x%x\n", __FUNCTION__, ret, len);
        return BSP_ERROR;
	}

    /* 判断投票结果 */
    if(cnf_data.vote_rst == 1)
    {
        g_ul_socp_vote_rst = 1;
    }
    else
    {
        g_ul_socp_vote_rst = 0;
    }

    osl_sem_up(&g_st_socp_vote_sem);

	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_socp_vote_to_mcore
*
* 功能描述  : 通过icc通道向mcore投票
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
s32 bsp_socp_vote_to_mcore(SOCP_VOTE_TYPE_ENUM_U32 type)
{
    socp_vote_req_stru req_data;
    u32 chan_id;
    s32 data_len;
    
    if(type >= SOCP_VOTE_TYPE_BUTT)
    {
        printf("%s: vote type 0x%x err\n", __FUNCTION__, type);
        return BSP_ERROR;
    }

    req_data.type = type;

    g_ul_socp_vote_rst = 0;

    /* icc通道发送数据 */
	chan_id = (ICC_CHN_MCORE_CCORE << 16) | MCORE_CCORE_FUNC_SOCP;
	data_len = sizeof(socp_vote_req_stru);
	if(data_len != bsp_icc_send(ICC_CPU_MCU, chan_id, (u8*)&req_data, sizeof(socp_vote_req_stru)))
	{
		printf("%s: icc send len 0x%x err\n", __FUNCTION__, data_len);
		return BSP_ERROR;
	}

    /* 等待MCORE返回投票结果 */
    if(0 != osl_sem_downtimeout(&g_st_socp_vote_sem, 500))
    {
		printf("%s: time out\n", __FUNCTION__);
		return BSP_ERROR;       
    }

    if(g_ul_socp_vote_rst == 0)
    {
 		printf("%s: vote to mcore fail\n", __FUNCTION__);
		return BSP_ERROR;         
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : socp_icc_chan_init
*
* 功能描述  : icc通道初始化
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
s32 socp_icc_chan_init(void)
{
    u32 chan_id;
    
	chan_id = (ICC_CHN_MCORE_CCORE << 16) | MCORE_CCORE_FUNC_SOCP;
	if(ICC_OK != bsp_icc_event_register(chan_id, socp_icc_read_cb, NULL, (write_cb_func)NULL, (void *)NULL))
	{
		printf("%s: icc chan init err\n", __FUNCTION__);
        return BSP_ERROR;
	}

    return BSP_OK;
}
#endif

/*****************************************************************************
* 函 数 名  : socpInit
*
* 功能描述  : 模块初始化函数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功的标识码
*****************************************************************************/
s32 socp_init(void)
{
    u32 i;

    if (BSP_TRUE == g_strSocpStat.bInitFlag)
    {
        return BSP_OK;
    }

    memset(&g_stSocpDebugInfo, 0x0, sizeof(SOCP_DEBUG_INFO_S));

    /* 初始化全局状态结构体 */
    for (i = SOCP_ENCSRC_CHN_BASE; i < (SOCP_ENCSRC_CHN_BASE + SOCP_ENCSRC_CHN_NUM); i++)
    {
        g_strSocpStat.sEncSrcChan[i].u32ChanID = i;
        g_strSocpStat.sEncSrcChan[i].u32ChanEn = SOCP_CHN_DISABLE;
        g_strSocpStat.sEncSrcChan[i].u32AllocStat  = SOCP_CHN_UNALLOCATED;
        g_strSocpStat.sEncSrcChan[i].u32LastRdSize = 0;
        g_strSocpStat.sEncSrcChan[i].u32DestChanID = 0xff;
        g_strSocpStat.sEncSrcChan[i].u32BypassEn = 0;
        g_strSocpStat.sEncSrcChan[i].ePriority = SOCP_CHAN_PRIORITY_3;
        g_strSocpStat.sEncSrcChan[i].eDataType = SOCP_DATA_TYPE_BUTT;
        g_strSocpStat.sEncSrcChan[i].eDataTypeEn= SOCP_DATA_TYPE_EN_BUTT;
        g_strSocpStat.sEncSrcChan[i].eDebugEn  = SOCP_ENC_DEBUG_EN_BUTT;

        g_strSocpStat.sEncSrcChan[i].event_cb       = (socp_event_cb)NULL;
        g_strSocpStat.sEncSrcChan[i].rd_cb          = (socp_rd_cb)NULL;
    }

    /* Modem Cpu时，需要屏蔽中断 */
    SOCP_REG_WRITE(SOCP_REG_MODEM_MASK1, 0xffffffff);
    SOCP_REG_WRITE(SOCP_REG_MODEM_MASK3, 0xffffffff);
    SOCP_REG_SETBITS(SOCP_REG_DEC_CORE1MASK0, 16, 16, 0xffff);
    SOCP_REG_SETBITS(SOCP_REG_DEC_CORE1MASK2, 16, 16, 0xffff);
    SOCP_REG_SETBITS(SOCP_REG_ENC_CORE1_MASK0, 0, 7, 0x7f);
    SOCP_REG_SETBITS(SOCP_REG_ENC_CORE1_MASK2, 0, 7, 0x7f);
    SOCP_REG_SETBITS(SOCP_REG_ENC_CORE1_MASK2, 16, 7, 0x7f);
    SOCP_REG_WRITE(SOCP_REG_DEC_CORE1_MASK1, 0xffffffff);

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    osl_sem_init(0, &g_st_socp_vote_sem);
    if(BSP_OK != socp_icc_chan_init())
    {
        printf("socp init fail\n");
        return BSP_ERROR;
    }
#endif

    /* 设置初始化状态 */
    g_strSocpStat.bInitFlag = BSP_TRUE;

    printf("socp init succeed\n");

    return BSP_OK;
}

/*lint -restore +e529*/
/*****************************************************************************
* 函 数 名  : bsp_socp_coder_set_src_chan
*
* 功能描述  : 编码源通道配置函数
*
* 输入参数  : pSrcAttr     编码源通道配置参数
*             ulSrcChanID  编码源通道ID
*
* 输出参数  : 无
*
* 返 回 值  : 申请及配置成功与否的标识码
*****************************************************************************/
s32 bsp_socp_coder_set_src_chan(SOCP_CODER_SRC_ENUM_U32 enSrcChanID,
                                            SOCP_CODER_SRC_CHAN_S *pSrcAttr)
{    
    u32 start;
    u32 end;
    u32 rdstart = 0;
    u32 rdend = 0;
    u32 buflength   = 0;
    u32 Rdbuflength = 0;
    u32 i;
    u32 srcChanId;
    u32 u32SrcChanType;
    u32 u32DstChanID;
    u32 u32DstChanType;
    u32 ResetFlag;
    SOCP_ENCSRC_CHAN_S *pChan;

    g_stSocpDebugInfo.sSocpDebugGBl.u32SocpAllocEncSrcCnt++;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(pSrcAttr);
    SOCP_CHECK_CHAN_PRIORITY(pSrcAttr->ePriority);
    SOCP_CHECK_DATA_TYPE(pSrcAttr->eDataType);
    SOCP_CHECK_DATA_TYPE_EN(pSrcAttr->eDataTypeEn);
    SOCP_CHECK_ENC_DEBUG_EN(pSrcAttr->eDebugEn);

    srcChanId       = SOCP_REAL_CHAN_ID(enSrcChanID);
    u32SrcChanType  = SOCP_REAL_CHAN_TYPE(enSrcChanID);

    SOCP_CHECK_CHAN_TYPE(u32SrcChanType, SOCP_CODER_SRC_CHAN);
    SOCP_CHECK_ENCSRC_CHAN_ID(srcChanId);

    u32DstChanID   = SOCP_REAL_CHAN_ID(pSrcAttr->u32DestChanID);
    u32DstChanType = SOCP_REAL_CHAN_TYPE(pSrcAttr->u32DestChanID);
    SOCP_CHECK_CHAN_TYPE(u32DstChanType, SOCP_CODER_DEST_CHAN);
    SOCP_CHECK_CHAN_ID(u32DstChanID, SOCP_MAX_ENCDST_CHN);

    if ((SOCP_ENCSRC_CHNMODE_CTSPACKET != pSrcAttr->eMode)
        && (SOCP_ENCSRC_CHNMODE_LIST != pSrcAttr->eMode))
    {
        printf("CoderAllocSrcChan: chnnel mode is invalid %d!\n", pSrcAttr->eMode);
        return BSP_ERR_SOCP_INVALID_PARA;
    }

    /* 使用配置参数进行配置 */
    /* 判断起始地址是否8字节对齐 */
    start   = pSrcAttr->sCoderSetSrcBuf.u32InputStart;
    end     = pSrcAttr->sCoderSetSrcBuf.u32InputEnd;

    SOCP_CHECK_PARA((void *)start);
    SOCP_CHECK_8BYTESALIGN(start);
    SOCP_CHECK_PARA((void *)end);
    SOCP_CHECK_BUF_ADDR(start, end);
    buflength = end - start + 1;
    SOCP_CHECK_8BYTESALIGN(buflength);
    if(buflength > SOCP_ENC_SRC_BUFLGTH_MAX)
    {
        printf("CoderAllocSrcChan: buffer length is too large!\n");
        return BSP_ERR_SOCP_INVALID_PARA;
    }

    /* 如果是用链表缓冲区，则配置RDbuffer的起始地址和长度 */
    if(SOCP_ENCSRC_CHNMODE_LIST == pSrcAttr->eMode)
    {
        /* 判断RDBuffer的起始地址是否8字节对齐 */
        rdstart = pSrcAttr->sCoderSetSrcBuf.u32RDStart;
        rdend   = pSrcAttr->sCoderSetSrcBuf.u32RDEnd;

        SOCP_CHECK_PARA((void *)rdstart);
        SOCP_CHECK_8BYTESALIGN(rdstart);
        SOCP_CHECK_PARA((void *)rdend);
        SOCP_CHECK_BUF_ADDR(rdstart, rdend);
        //SOCP_CHECK_PARA(pSrcAttr->sCoderSetSrcBuf.u32RDThreshold);
        Rdbuflength = rdend - rdstart + 1;
        SOCP_CHECK_8BYTESALIGN(Rdbuflength);
        if(Rdbuflength > SOCP_ENC_SRC_RDLGTH_MAX)
        {
            printf("CoderAllocSrcChan: RD buffer length is too large!\n");
            return BSP_ERR_SOCP_INVALID_PARA;
        }
    }

    /* 复位通道 */
    SOCP_REG_SETBITS(SOCP_REG_ENCRST, srcChanId, 1, 1);

    /* 等待通道自清 */
    for (i = 0; i < SOCP_RESET_TIME; i++)
    {
        ResetFlag = SOCP_REG_GETBITS(SOCP_REG_ENCRST, srcChanId, 1);
        if(0 == ResetFlag)
        {
            break;
        }

        if ((SOCP_RESET_TIME - 1) == i)
        {
            printf("CoderAllocSrcChan: reset channel 0x%x failed!\n", srcChanId);
        }
    }

    /* 写入起始地址到源buffer起始地址寄存器*/
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(srcChanId), start);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(srcChanId), start);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(srcChanId), start);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(srcChanId), 0, 27, buflength);
	SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(srcChanId), 27, 5, 0);

    /* 如果是用链表缓冲区，则配置RDbuffer的起始地址和长度 */
    if(SOCP_ENCSRC_CHNMODE_LIST == pSrcAttr->eMode)
    {
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQADDR(srcChanId), rdstart);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQRPTR(srcChanId), rdstart);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQWPTR(srcChanId), rdstart);
        SOCP_REG_SETBITS(SOCP_REG_ENCSRC_RDQCFG(srcChanId), 0, 16, Rdbuflength);
		SOCP_REG_SETBITS(SOCP_REG_ENCSRC_RDQCFG(srcChanId), 16, 16, 0);
    }

    /*配置其它参数*/
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 1, 2, pSrcAttr->eMode);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 4, 4, pSrcAttr->u32DestChanID);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 8, 2, pSrcAttr->ePriority);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 10, 1, pSrcAttr->u32BypassEn);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 11, 1, pSrcAttr->eDataTypeEn);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 31, 1, pSrcAttr->eDebugEn);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 16, 8, pSrcAttr->eDataType);

    /* 在g_strSocpStat中保存参数*/
    pChan = &g_strSocpStat.sEncSrcChan[srcChanId];
    pChan->eChnMode               = pSrcAttr->eMode;
    pChan->ePriority              = pSrcAttr->ePriority;
    pChan->eDataType              = pSrcAttr->eDataType;
    pChan->eDataTypeEn            = pSrcAttr->eDataTypeEn;
    pChan->eDebugEn               = pSrcAttr->eDebugEn;
    pChan->u32DestChanID          = pSrcAttr->u32DestChanID;
    pChan->u32BypassEn            = pSrcAttr->u32BypassEn;
    pChan->sEncSrcBuf.u32Start    = start;
    pChan->sEncSrcBuf.u32End      = end;
    pChan->sEncSrcBuf.u32Write    = start;
    pChan->sEncSrcBuf.u32Read     = start;
    pChan->sEncSrcBuf.u32Length   = buflength;
    pChan->sEncSrcBuf.u32IdleSize = 0;

    if(SOCP_ENCSRC_CHNMODE_LIST == pSrcAttr->eMode)
    {
        pChan->sRdBuf.u32Start    = rdstart;
        pChan->sRdBuf.u32End      = rdend;
        pChan->sRdBuf.u32Write    = rdstart;
        pChan->sRdBuf.u32Read     = rdstart;
        pChan->sRdBuf.u32Length   = Rdbuflength;
        pChan->u32RdThreshold     = pSrcAttr->sCoderSetSrcBuf.u32RDThreshold;
    }

    /* 标记通道状态 */
    g_strSocpStat.sEncSrcChan[srcChanId].u32AllocStat = SOCP_CHN_ALLOCATED;
    g_stSocpDebugInfo.sSocpDebugGBl.u32SocpAllocEncSrcSucCnt++;
    return BSP_OK;

}


/*****************************************************************************
* 函 数 名  : bsp_socp_free_channel
*
* 功能描述  : 通道释放函数
*
* 输入参数  : u32ChanID       编解码通道指针
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
s32 bsp_socp_free_channel(u32 u32ChanID)
{
    u32 u32RealChanID;
    u32 u32ChanType;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断通道ID是否有效 */
    u32RealChanID = SOCP_REAL_CHAN_ID(u32ChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32ChanID);
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        SOCP_ENCSRC_CHAN_S *pChan;

        SOCP_CHECK_ENCSRC_CHAN_ID(u32RealChanID);
        SOCP_CHECK_ENCSRC_ALLOC(u32RealChanID);

        pChan = &g_strSocpStat.sEncSrcChan[u32RealChanID];
        if (SOCP_CHN_ENABLE == pChan->u32ChanEn)
        {
            printf("FreeChannel: chan 0x%x is running!\n", u32ChanID);
            return BSP_ERR_SOCP_CHAN_RUNNING;
        }

        (void)socp_reset_enc_chan(u32RealChanID);

        pChan->u32AllocStat = SOCP_CHN_UNALLOCATED;

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpFreeEncSrcCnt[u32RealChanID]++;
    }
    else
    {
        printf("FreeChannel: invalid chan type 0x%x!\n", u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_socp_chan_soft_reset
*
* 功能描述  : 通道软复位函数
*
* 输入参数  : u32ChanID       编解码通道ID
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
s32 bsp_socp_chan_soft_reset(u32 u32ChanID)
{
    u32 u32RealChanID;
    u32 u32ChanType;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断通道ID是否有效 */
    u32RealChanID = SOCP_REAL_CHAN_ID(u32ChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32ChanID);
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        if (u32RealChanID < SOCP_CCORE_ENCSRC_CHN_BASE)
        {
            SOCP_CHECK_ENCSRC_ALLOC(u32RealChanID);
            (void)socp_reset_enc_chan(u32RealChanID);
        }
        else if ((u32RealChanID >= SOCP_CODER_SRC_LBBP1) && (u32RealChanID < SOCP_CODER_SRC_LBBP9))
        {
            (void)socp_reset_bbp_log_chan(u32RealChanID);
        }
        else if (SOCP_CODER_SRC_LBBP9 == u32RealChanID)
        {
            (void)socp_reset_bbp_ds_chan(u32RealChanID);
        }
        else if (SOCP_CODER_SRC_LDSP1 == u32RealChanID)
        {
            (void)socp_reset_dsp_log_chan(u32RealChanID);
        }
        else
        {
            printk("ChnSoftReset: reset enc src chan 0x%x failed!\n", u32ChanID);
            return BSP_ERR_SOCP_INVALID_CHAN;
        }

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpSoftResetEncSrcCnt[u32RealChanID]++;
    }
    else
    {
        printf("ChnSoftReset: invalid chan type: 0x%x!\n", u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_socp_start
*
* 功能描述  : 编码或者解码启动函数
*
* 输入参数  : u32SrcChanID      通道ID
* 输出参数  :
*
* 返 回 值  : 启动成功与否的标识码
*****************************************************************************/
/*lint -save -e529*/
s32 bsp_socp_start(u32 u32SrcChanID)
{
    u32 u32RealChanID;
    u32 u32ChanType;
#if 0     
    u32 u32DstId;   
	u32 u32Start;
#endif	
    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断通道ID是否有效 */
    u32RealChanID = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        if (u32RealChanID < SOCP_MAX_ENCSRC_CHN)
        {
            SOCP_CHECK_ENCSRC_CHAN_ID(u32RealChanID);
            SOCP_CHECK_ENCSRC_ALLOC(u32RealChanID);
        }
        else
        {
            printf("Start: enc src 0x%x is valid!\n", u32SrcChanID);
            return BSP_ERR_SOCP_INVALID_CHAN;
        }

#if 0
        u32DstId = SOCP_REG_GETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32RealChanID), 4, 4);          
        //通过寄存器判断
        SOCP_REG_READ(SOCP_REG_ENCDEST_BUFADDR(u32DstId), u32Start);       
        while(0 == u32Start)
        {
            taskDelay(1);
            SOCP_REG_READ(SOCP_REG_ENCDEST_BUFADDR(u32DstId), u32Start);
        }
#endif
        /* 先清中断，再打开中断*/
        SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT1, u32RealChanID, 1, 1);
        /* 保持C核屏蔽包头错误中断 */
        //SOCP_REG_SETBITS(SOCP_REG_MODEM_MASK1, u32RealChanID, 1, 0);
        /* C核包头错误在A核处理，打开A核中断 */
        SOCP_REG_SETBITS(SOCP_REG_APP_MASK1, u32RealChanID, 1, 0);

        if (SOCP_ENCSRC_CHNMODE_LIST == g_strSocpStat.sEncSrcChan[u32RealChanID].eChnMode)
        {
            SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT3, u32RealChanID, 1, 1);
            /* 保持RD中断屏蔽 */
            //SOCP_REG_SETBITS(SOCP_REG_MODEM_MASK3, u32RealChanID, 1, 0);
        }

        /* 设置打开状态*/
        SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32RealChanID), 0, 1, 1);
        if (u32RealChanID < SOCP_MAX_ENCSRC_CHN)
        {
            g_strSocpStat.sEncSrcChan[u32RealChanID].u32ChanEn = SOCP_CHN_ENABLE;
            g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpStartEncSrcCnt[u32RealChanID]++;
        }
    }
    else
    {
        printf("Start: invalid chan type: 0x%x!\n", u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_socp_stop
*
* 功能描述  : 编码或者解码停止函数
*
* 输入参数  : u32SrcChanID      通道ID
*
* 输出参数  :
*
* 返 回 值  : 停止成功与否的标识码
*****************************************************************************/
s32 bsp_socp_stop(u32 u32SrcChanID)
{
    u32 u32RealChanID;
    u32 u32ChanType;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断通道ID是否有效 */
    u32RealChanID = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        if (u32RealChanID < SOCP_MAX_ENCSRC_CHN)
        {
            SOCP_CHECK_ENCSRC_CHAN_ID(u32RealChanID);
            SOCP_CHECK_ENCSRC_ALLOC(u32RealChanID);
        }
        else
        {
            printf("Stop: enc src 0x%x is valid!\n", u32SrcChanID);
            return BSP_ERR_SOCP_INVALID_CHAN;
        }

        /* 关闭C核中断*/
       // SOCP_REG_SETBITS(SOCP_REG_MODEM_MASK1, u32RealChanID, 1, 1);
        /* 关闭A核中断，包头错误在A核处理 */
        SOCP_REG_SETBITS(SOCP_REG_APP_MASK1, u32RealChanID, 1, 1);

        if (SOCP_ENCSRC_CHNMODE_LIST == g_strSocpStat.sEncSrcChan[u32RealChanID].eChnMode)
        {
            SOCP_REG_SETBITS(SOCP_REG_MODEM_MASK3, u32RealChanID, 1, 1);
        }

        /* 设置通道关闭状态*/
        SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32RealChanID), 0, 1, 0);
        if (u32RealChanID < SOCP_MAX_ENCSRC_CHN)
        {
            g_strSocpStat.sEncSrcChan[u32RealChanID].u32ChanEn = SOCP_CHN_DISABLE;
            g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpStopEncSrcCnt[u32RealChanID]++;
        }
    }
    else
    {
        printf("Stop: invalid chan type: 0x%x!\n", u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return BSP_OK;
}

/*lint -save -e18*/
/*****************************************************************************
* 函 数 名  : bsp_socp_register_event_cb
*
* 功能描述  : 异常事件回调函数注册函数
*
* 输入参数  : u32ChanID      通道ID
*             EventCB        异常事件的回调函数
* 输出参数  :
*
* 返 回 值  : 注册成功与否的标识码
*****************************************************************************/
/*lint -save -e49*/
/*lint -save -e601*/
s32 bsp_socp_register_event_cb(u32 u32ChanID, socp_event_cb EventCB)
{
/*lint -restore +e601*/    
/*lint -restore +e49*/    
    u32 u32RealChanID;
    u32 u32ChanType;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 获取通道类型和实际的通道ID */
    u32RealChanID = SOCP_REAL_CHAN_ID(u32ChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32ChanID);

    switch (u32ChanType)
    {
        case SOCP_CODER_SRC_CHAN:      /* 编码源通道 */
        {
            if (u32RealChanID < SOCP_MAX_ENCSRC_CHN)
            {
                SOCP_CHECK_ENCSRC_CHAN_ID(u32RealChanID);
                SOCP_CHECK_ENCSRC_ALLOC(u32RealChanID);
                g_strSocpStat.sEncSrcChan[u32RealChanID].event_cb = EventCB;

                g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpRegEventEncSrcCnt[u32RealChanID]++;
            }
            break;
        }
        default:
        {
            printf("RegisterEventCB: invalid chan type: 0x%x!\n", u32ChanType);
            return BSP_ERR_SOCP_INVALID_CHAN;
        }
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_socp_get_write_buff
*
* 功能描述  : 上层获取写数据buffer函数
*
* 输入参数  : u32SrcChanID    源通道ID
* 输出参数  : pBuff           获取的buffer
*
* 返 回 值  : 获取成功与否的标识码
*****************************************************************************/
s32 bsp_socp_get_write_buff(u32 u32SrcChanID, SOCP_BUFFER_RW_S *pBuff)
{
    u32 u32ChanID;
    u32 u32ChanType;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(pBuff);

    /* 获得实际通道id */
    u32ChanID   = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpGetWBufEncSrcEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_ENCSRC_CHAN_ID(u32ChanID);
        SOCP_CHECK_ENCSRC_ALLOC(u32ChanID);

        /* 根据读写指针获取buffer */
        SOCP_REG_READ(SOCP_REG_ENCSRC_BUFRPTR(u32ChanID), g_strSocpStat.sEncSrcChan[u32ChanID].sEncSrcBuf.u32Read);
        SOCP_REG_READ(SOCP_REG_ENCSRC_BUFWPTR(u32ChanID), g_strSocpStat.sEncSrcChan[u32ChanID].sEncSrcBuf.u32Write);

        socp_get_idle_buffer(&g_strSocpStat.sEncSrcChan[u32ChanID].sEncSrcBuf, pBuff);

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpGetWBufEncSrcSucCnt[u32ChanID]++;
    }
    else
    {
        printf("GetWriteBuff: invalid chan type: 0x%x!\n", u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_socp_write_done
*
* 功能描述  : 写数据完成函数
*
* 输入参数  : u32SrcChanID    源通道ID
              u32WrtSize      写入数据的长度
*
* 输出参数  :
*
* 返 回 值  : 操作完成与否的标识码
*****************************************************************************/
s32 bsp_socp_write_done(u32 u32SrcChanID, u32 u32WrtSize)
{
    u32 u32ChanID;
    u32 u32ChanType;
    u32 u32WrtPad;
    SOCP_BUFFER_RW_S RwBuff;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(u32WrtSize);

    /* 获得实际通道id */
    u32ChanID   = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        SOCP_ENCSRC_CHAN_S *pChan;

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32socp_write_doneEncSrcEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_ENCSRC_CHAN_ID(u32ChanID);
        SOCP_CHECK_ENCSRC_ALLOC(u32ChanID);

        pChan = &g_strSocpStat.sEncSrcChan[u32ChanID];
        u32WrtPad = u32WrtSize % 8;
        if (0 != u32WrtPad)
        {
            u32WrtSize += (8 - u32WrtPad);
        }

        SOCP_REG_READ(SOCP_REG_ENCSRC_BUFWPTR(u32ChanID), pChan->sEncSrcBuf.u32Write);
        SOCP_REG_READ(SOCP_REG_ENCSRC_BUFRPTR(u32ChanID), pChan->sEncSrcBuf.u32Read);
        socp_get_idle_buffer(&pChan->sEncSrcBuf, &RwBuff);
        /*lint -save -e574*/
        if (RwBuff.u32Size + RwBuff.u32RbSize < u32WrtSize)
        /*lint -restore +e574*/            
        {
            printf("WriteDone: enc src, too large write size!\n");
            printf("WriteDone: enc src, write ptr is 0x%x, read ptr is 0x%x\n", pChan->sEncSrcBuf.u32Write, pChan->sEncSrcBuf.u32Read);
            printf("WriteDone: enc src, u32Size is 0x%x, u32RbSize is 0x%x\n", RwBuff.u32Size, RwBuff.u32RbSize);
            printf("WriteDone: enc src, u32WrtSize is 0x%x, u32SrcChanID is 0x%x\n", u32WrtSize, u32SrcChanID);
            g_stSocpDebugInfo.sSocpDebugEncSrc.u32socp_write_doneEncSrcFailCnt[u32ChanID]++;
            return BSP_ERR_SOCP_INVALID_PARA;
        }

        /* 设置读写指针 */
        socp_write_done(&pChan->sEncSrcBuf, u32WrtSize);

        /* 写入写指针到写指针寄存器*/
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(u32ChanID), pChan->sEncSrcBuf.u32Write);

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32socp_write_doneEncSrcSucCnt[u32ChanID]++;
    }
    else
    {
        printf("WriteDone: invalid chan type: 0x%x!\n", u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_socp_register_rd_cb
*
* 功能描述  : RDbuffer回调函数注册函数
*
* 输入参数  : u32SrcChanID    源通道ID
              RdCB            RDbuffer完成回调函数
*
* 输出参数  :
*
* 返 回 值  : 注册成功与否的标识码
*****************************************************************************/
s32 bsp_socp_register_rd_cb(u32 u32SrcChanID, socp_rd_cb RdCB)
{
    u32 u32RealChanID;
    u32 u32ChanType;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 获取通道类型和实际的通道ID */
    u32RealChanID = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        SOCP_CHECK_ENCSRC_CHAN_ID(u32RealChanID);
        SOCP_CHECK_ENCSRC_ALLOC(u32RealChanID);

        if (SOCP_ENCSRC_CHNMODE_LIST == g_strSocpStat.sEncSrcChan[u32RealChanID].eChnMode)
        {
            /* 设置对应通道的回调函数*/
            g_strSocpStat.sEncSrcChan[u32RealChanID].rd_cb = RdCB;
        }
        else
        {
            printf("RegisterRdCB: invalid chan mode!\n");
            return BSP_ERR_SOCP_CHAN_MODE;
        }

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpRegRdCBEncSrcCnt[u32RealChanID]++;
    }
    else
    {
        printf("RegisterRdCB: invalid chan type: 0x%x!\n", u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_socp_get_rd_buffer
*
* 功能描述  : 获取RDbuffer函数
*
* 输入参数  : u32SrcChanID    源通道ID
*
* 输出参数  : pBuff           获取的RDbuffer
*
* 返 回 值  : 获取成功与否的标识码
*****************************************************************************/
s32 bsp_socp_get_rd_buffer(u32 u32SrcChanID, SOCP_BUFFER_RW_S *pBuff)
{
    u32 u32ChanID;
    u32 u32ChanType;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(pBuff);

    /* 获得实际通道id */
    u32ChanID   = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpGetRdBufEncSrcEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_ENCSRC_CHAN_ID(u32ChanID);
        SOCP_CHECK_ENCSRC_ALLOC(u32ChanID);

        /* 根据读写指针获取buffer */
        SOCP_REG_READ(SOCP_REG_ENCSRC_RDQRPTR(u32ChanID), g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf.u32Read);
        SOCP_REG_READ(SOCP_REG_ENCSRC_RDQWPTR(u32ChanID), g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf.u32Write);

        socp_get_data_buffer(&g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf, pBuff);

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpGetRdBufEncSrcSucCnt[u32ChanID]++;
    }
    else
    {
        printf("GetRDBuffer: invalid chan type: 0x%x!\n", u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_socp_read_rd_done
*
* 功能描述  : 读取RDbuffer数据完成函数
*
* 输入参数  : u32SrcChanID   源通道ID
              u32RDSize      读取的RDbuffer数据长度
*
* 输出参数  :
*
* 返 回 值  : 读取成功与否的标识码
*****************************************************************************/
s32 bsp_socp_read_rd_done(u32 u32SrcChanID, u32 u32RDSize)
{
    u32 u32ChanID;
    u32 u32ChanType;
    SOCP_BUFFER_RW_S RwBuff;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(u32RDSize);

    /* 获得实际通道id */
    u32ChanID   = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        SOCP_ENCSRC_CHAN_S *pChan;

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpReadRdDoneEncSrcEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_ENCSRC_CHAN_ID(u32ChanID);
        SOCP_CHECK_ENCSRC_ALLOC(u32ChanID);

        pChan = &g_strSocpStat.sEncSrcChan[u32ChanID];
        g_strSocpStat.sEncSrcChan[u32ChanID].u32LastRdSize = 0;

        /* 设置读写指针 */
        SOCP_REG_READ(SOCP_REG_ENCSRC_RDQWPTR(u32ChanID), pChan->sRdBuf.u32Write);
        SOCP_REG_READ(SOCP_REG_ENCSRC_RDQRPTR(u32ChanID), pChan->sRdBuf.u32Read);
        socp_get_data_buffer(&pChan->sRdBuf, &RwBuff);
        /*lint -save -e574*/        
        if (RwBuff.u32Size + RwBuff.u32RbSize < u32RDSize)
        /*lint -restore +e574*/
        {
            printf("ReadRDDone: enc src, too large rd size!\n");
            printf("ReadRDDone: enc src, write ptr is 0x%x, read ptr is 0x%x\n", pChan->sRdBuf.u32Write, pChan->sRdBuf.u32Read);
            printf("ReadRDDone: enc src, u32Size is 0x%x, u32RbSize is 0x%x\n", RwBuff.u32Size, RwBuff.u32RbSize);
            printf("ReadRDDone: enc src, u32RDSize is 0x%x, u32SrcChanID is 0x%x\n", u32RDSize, u32SrcChanID);
            g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpReadRdDoneEncSrcFailCnt[u32ChanID]++;

            return BSP_ERR_SOCP_INVALID_PARA;
        }

        socp_read_done(&pChan->sRdBuf, u32RDSize);

        /* 写入写指针到写指针寄存器*/
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQRPTR(u32ChanID), pChan->sRdBuf.u32Read);

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpReadRdDoneEncSrcSucCnt[u32ChanID]++;
    }
    else
    {
        printf("ReadRDDone: invalid chan type: 0x%x!", u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return BSP_OK;
}
/*lint -restore +e101 +e550 */

/*****************************************************************************
* 函 数 名   : socp_help
*
* 功能描述  : 获取socp打印信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
void socp_help(void)
{
    printf("\r |*************************************|\n");
    printf("\r socp_show_debug_gbl   : 查看全局统计信息:通道申请、配置和中断总计数，无参数\n");
    printf("\r socp_show_enc_src_chan_cur : 查看编码源通道属性，参数为通道ID\n");
    printf("\r socp_show_enc_src_chan_add : 查看编码源通道操作统计值，参数为通道ID\n");
    printf("\r socp_show_enc_src_chan_all : 查看所有编码源通道属性和统计值，无参数\n");
    printf("\r socp_debug_cnt_show : 查看全部统计信息，无参数\n");
}

/*****************************************************************************
* 函 数 名   : socp_show_debug_gbl
*
* 功能描述  : 显示全局debug 计数信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
void socp_show_debug_gbl(void)
{
    SOCP_DEBUG_GBL_S *sSocpDebugGblInfo;

    sSocpDebugGblInfo = &g_stSocpDebugInfo.sSocpDebugGBl;

    printf("\r SOCP全局状态维护信息:\n");
    printf("\r socp申请编码源通道的次数                                 : 0x%x\n",
           (s32)sSocpDebugGblInfo->u32SocpAllocEncSrcCnt);
    printf("\r socp申请编码源通道成功的次数                           : 0x%x\n",
           (s32)sSocpDebugGblInfo->u32SocpAllocEncSrcSucCnt);
}

/*****************************************************************************
* 函 数 名   : socp_show_enc_src_chan_cur
*
* 功能描述  : 打印编码源通道当前属性
*
* 输入参数  : 通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
u32 socp_show_enc_src_chan_cur(u32 u32UniqueId)
{
    u32 u32RealId   = 0;
    u32 u32ChanType = 0;

    u32RealId   = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_CODER_SRC_CHAN);
    SOCP_CHECK_ENCSRC_CHAN_ID(u32RealId);

    printf("================== 申请的编码源通道 0x%x  属性:=================\n", u32UniqueId);
    printf("通道ID:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32ChanID);
    printf("通道分配状态:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32AllocStat);
    printf("通道使能状态:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32ChanEn);
    printf("目的通道ID:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32DestChanID);
    printf("通道优先级:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].ePriority);
    printf("通道旁路状态:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32BypassEn);
    printf("通道数据格式类型:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].eChnMode);
    printf("通道所属模类型:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].eDataType);
    printf("通道buffer 起始地址:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Start);
    printf("通道buffer 结束地址:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32End);
    printf("通道buffer 读指针:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Read);
    printf("通道buffer 写指针:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Write);
    printf("通道buffer 长度:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Length);
    if (SOCP_ENCSRC_CHNMODE_LIST == g_strSocpStat.sEncSrcChan[u32RealId].eChnMode)
    {
        printf("通道RD buffer 起始地址:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Start);
        printf("通道RD buffer 结束地址:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32End);
        printf("通道RD buffer 读指针:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Read);
        printf("通道RD buffer 写指针:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Write);
        printf("通道RD buffer 长度:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Length);
        printf("通道RD buffer 门限:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].u32RdThreshold);
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名   : socp_show_enc_src_chan_add
*
* 功能描述  : 打印编码源通道累计统计值
*
* 输入参数  : 通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
u32 socp_show_enc_src_chan_add(u32 u32UniqueId)
{
    u32 u32ChanType;
    u32 u32RealChanID;
    SOCP_DEBUG_ENCSRC_S *sSocpAddDebugEncSrc;

    u32RealChanID = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);
    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_CODER_SRC_CHAN);

    sSocpAddDebugEncSrc = &g_stSocpDebugInfo.sSocpDebugEncSrc;

    SOCP_CHECK_ENCSRC_CHAN_ID(u32RealChanID);

    printf("================== 编码源通道 0x%x  累计统计值:=================\n", u32UniqueId);
    printf("\r socp释放编码源通道成功的次数                           : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpFreeEncSrcCnt[u32RealChanID]);
    printf("\r socp启动编码源通道成功的次数                           : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpStartEncSrcCnt[u32RealChanID]);
    printf("\r socp停止编码源通道成功的次数                           : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpStopEncSrcCnt[u32RealChanID]);
    printf("\r socp软复位编码源通道成功的次数                        : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpSoftResetEncSrcCnt[u32RealChanID]);
    printf("\r socp注册编码源通道异常处理函数的次数               : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpRegEventEncSrcCnt[u32RealChanID]);
    printf("\r socp编码源通道尝试获得写buffer的次数                  : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpGetWBufEncSrcEtrCnt[u32RealChanID]);
    printf("\r socp编码源通道获得写buffer成功的次数                  : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpGetWBufEncSrcSucCnt[u32RealChanID]);
    printf("\r socp编码源通道尝试更新写buffer指针的次数            : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32socp_write_doneEncSrcEtrCnt[u32RealChanID]);
    printf("\r socp编码源通道更新写buffer指针成功的次数            : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32socp_write_doneEncSrcSucCnt[u32RealChanID]);
    printf("\r socp编码源通道更新写buffer指针失败的次数            : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32socp_write_doneEncSrcFailCnt[u32RealChanID]);
    printf("\r socp编码源通道注册RD buffer回调函数成功的次数      : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpRegRdCBEncSrcCnt[u32RealChanID]);
    printf("\r socp编码源通道尝试获得RD buffer的次数                   : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpGetRdBufEncSrcEtrCnt[u32RealChanID]);
    printf("\r socp编码源通道获得RD buffer成功的次数                   : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpGetRdBufEncSrcSucCnt[u32RealChanID]);
    printf("\r socp编码源通道尝试更新RDbuffer指针的次数              : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpReadRdDoneEncSrcEtrCnt[u32RealChanID]);
    printf("\r socp编码源通道更新RDbuffer指针成功的次数              : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpReadRdDoneEncSrcSucCnt[u32RealChanID]);
    printf("\r socp编码源通道更新RDbuffer指针失败的次数              : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpReadRdDoneEncSrcFailCnt[u32RealChanID]);
    printf("\r socp ISR 中进入编码源通道包头错误中断次数                  : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcIsrHeadIntCnt[u32RealChanID]);
    printf("\r socp 任务中回调编码源通道包头错误中断处理函数次数   : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcTskHeadCbOriCnt[u32RealChanID]);
    printf("\r socp 回调编码源通道包头错误中断处理函数成功的次数   : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcTskHeadCbCnt[u32RealChanID]);
    printf("\r socp ISR 中进入编码源通道Rd 完成中断次数                     : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcIsrRdIntCnt[u32RealChanID]);
    printf("\r socp 任务中回调编码源通道Rd 完成中断处理函数次数       : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcTskRdCbOriCnt[u32RealChanID]);
    printf("\r socp 回调编码源通道Rd 完成中断处理函数成功的次数       : 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcTskRdCbCnt[u32RealChanID]);

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名   : socp_show_enc_src_chan_all
*
* 功能描述  : 打印所有编码源通道信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
void socp_show_enc_src_chan_all(void)
{
    u32 i;

    for (i = SOCP_ENCSRC_CHN_BASE; i < (SOCP_ENCSRC_CHN_BASE + SOCP_ENCSRC_CHN_NUM); i++)
    {
        (void)socp_show_enc_src_chan_cur(i);
        (void)socp_show_enc_src_chan_add(i);
    }

    return;
}

/*****************************************************************************
* 函 数 名   : socp_debug_cnt_show
*
* 功能描述  : 显示debug 计数信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
void socp_debug_cnt_show(void)
{
    socp_show_debug_gbl();
    socp_show_enc_src_chan_all();
}

/* 低功耗相关 begin */
/* 低功耗相关暂不修改 */
#define SOCP_REG_ADDR_DRX(addr)  (addr + SOCP_REG_BASEADDR)
#define BBPDMA_REG_ADDR_DRX(addr)(addr + BBP_REG_ARM_BASEADDR)
#define SOCP_LTE_MAX_ENCSRC_CHN  	(24)

u32                 g_aSocpDrxGlobal[12];
u32                 g_aSocpDrxInitStat[17];
u32                 g_aSocpDrxIntMask[13];
SOCP_DRX_ENCSRC_S       g_aSocpDrxEncSrc[SOCP_MAX_ENCSRC_CHN];
SOCP_DRX_ENCDST_S       g_aSocpDrxEncDst[SOCP_MAX_ENCDST_CHN];
SOCP_DRX_DECSRC_S       g_aSocpDrxDecSrc[SOCP_MAX_DECSRC_CHN];
SOCP_DRX_DECDST_S       g_aSocpDrxDecDst[SOCP_MAX_DECDST_CHN];
SOCP_DRX_ENCSRC_S 	    g_aSocpDrxBBPEncSrc[9];
BBPDMA_DRX_LOG_S        g_sBBPDMADrxLog[8];
BBPDMA_DRX_DS_S         g_sBBPDMADrxDs;


s32 bsp_socp_drx_bak_reg(void)    
{ 
    u32 i = 0;

    memcpy(g_aSocpDrxGlobal, (void *)SOCP_REG_ADDR_DRX(SOCP_REG_GBLRST), sizeof(g_aSocpDrxGlobal)); /* [false alarm]:屏蔽Fortify错误 */
    memcpy(g_aSocpDrxInitStat, (void *)SOCP_REG_ADDR_DRX(SOCP_REG_GBL_INTSTAT), sizeof(g_aSocpDrxInitStat)); /* [false alarm]:屏蔽Fortify错误 */
    memcpy(g_aSocpDrxIntMask, (void *)SOCP_REG_ADDR_DRX(SOCP_REG_DEC_CORE0MASK0), sizeof(g_aSocpDrxIntMask)); /* [false alarm]:屏蔽Fortify错误 */
    memcpy(g_aSocpDrxEncSrc, (void *)SOCP_REG_ADDR_DRX(SOCP_REG_ENCSRC_BUFWPTR(0)), sizeof(g_aSocpDrxEncSrc)); /* [false alarm]:屏蔽Fortify错误 */
    memcpy(g_aSocpDrxEncDst, (void *)SOCP_REG_ADDR_DRX(SOCP_REG_ENCDEST_BUFWPTR(0)), sizeof(g_aSocpDrxEncDst)); /* [false alarm]:屏蔽Fortify错误 */
    memcpy(g_aSocpDrxDecSrc, (void *)SOCP_REG_ADDR_DRX(SOCP_REG_DECSRC_BUFWPTR(0)), sizeof(g_aSocpDrxDecSrc)); /* [false alarm]:屏蔽Fortify错误 */
    memcpy(g_aSocpDrxDecDst, (void *)SOCP_REG_ADDR_DRX(SOCP_REG_DECDEST_BUFWPTR(0)), sizeof(g_aSocpDrxDecDst)); /* [false alarm]:屏蔽Fortify错误 */

    for(i=0;i<SOCP_MAX_ENCSRC_CHN;i++)//SOCP_TOTAL_ENCSRC_CHN
    {
        SOCP_REG_SETBITS (SOCP_REG_ENCSRC_BUFCFG1(i), 0, 1, 0);
    }
    
    for(i=0;i<(SOCP_MAX_DECSRC_CHN);i++)
    {
        SOCP_REG_SETBITS (SOCP_REG_DECSRC_BUFCFG0(i), 30, 1,0);
    }
   
    return BSP_OK;
}

s32 BSP_BBPDMA_DrxBakReg(void)
{
#ifndef DRV_BUILD_SEPARATE    
	memcpy(g_aSocpDrxBBPEncSrc, (void *)SOCP_REG_ADDR_DRX(SOCP_REG_ENCSRC_BUFWPTR(16)), sizeof(g_aSocpDrxBBPEncSrc)); /* [false alarm]:屏蔽Fortify错误 */
#endif

    return BSP_OK;
}

s32 BSP_BBPDMA_DrxRestoreReg(void)   
{
#ifndef DRV_BUILD_SEPARATE    
	u32 i= 0;

	// bus
	for(i=0;i<8;i++)
    {
		BBP_REG_WRITE(BBP_REG_LOG_ADDR(i), (u32)g_aSocpDrxBBPEncSrc[i].u32StartAddr);
    	BBP_REG_SETBITS(BBP_REG_LOG_CFG(i), 0, 24, SOCP_BBPLOG_CHN_SIZE);
    }
    BBP_REG_SETBITS(BBP_REG_LOG_EN, 0, 1, 1);

	// data 
	BBP_REG_WRITE(BBP_REG_DS_ADDR,SOCP_BBPDS_CHN_ADDR);
    BBP_REG_SETBITS(BBP_REG_DS_CFG, 31, 1, SOCP_BBP_DS_MODE_OVERRIDE);
    BBP_REG_SETBITS(BBP_REG_DS_CFG, 0, 28, SOCP_BBPDS_CHN_SIZE);
    BBP_REG_WRITE(BBP_REG_PTR_ADDR, (SOCP_REG_BASEADDR + SOCP_REG_ENCSRC_BUFWPTR(SOCP_BBPLOG_CHN)));
#endif

    return BSP_OK;
}

void bsp_socp_drx_restore_reg(void) 
{ 
	u32 i;

    /*恢复全局状态寄存器*/
    memcpy((void *)SOCP_REG_ADDR_DRX(SOCP_REG_GBLRST), g_aSocpDrxGlobal, sizeof(g_aSocpDrxGlobal)); /* [false alarm]:屏蔽Fortify错误 */
    memcpy((void *)SOCP_REG_ADDR_DRX(SOCP_REG_GBL_INTSTAT), g_aSocpDrxInitStat, sizeof(g_aSocpDrxInitStat)); /* [false alarm]:屏蔽Fortify错误 */
    memcpy((void *)SOCP_REG_ADDR_DRX(SOCP_REG_DEC_CORE0MASK0), g_aSocpDrxIntMask, sizeof(g_aSocpDrxIntMask)); /* [false alarm]:屏蔽Fortify错误 */

    /*恢复编码目的通道寄存器*/
    for(i=0;i<SOCP_MAX_ENCDST_CHN;i++)
    {
        SOCP_REG_WRITE(SOCP_REG_ENCDEST_BUFADDR(i), g_aSocpDrxEncDst[i]. u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_ENCDEST_BUFRPTR(i), g_aSocpDrxEncDst[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_ENCDEST_BUFWPTR(i), g_aSocpDrxEncDst[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_ENCDEST_BUFCFG(i), g_aSocpDrxEncDst[i].u32Config);
        SOCP_REG_WRITE(SOCP_REG_ENCDEST_BUFTHRH(i), g_aSocpDrxEncDst[i].u32Thrh);
    }

    /*恢复解码目的通道寄存器*/
   for(i=0;i<SOCP_MAX_DECDST_CHN;i++)
    {
        SOCP_REG_WRITE(SOCP_REG_DECDEST_BUFADDR(i), g_aSocpDrxDecDst[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_DECDEST_BUFRPTR(i), g_aSocpDrxDecDst[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_DECDEST_BUFWPTR(i), g_aSocpDrxDecDst[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_DECDEST_BUFCFG(i), g_aSocpDrxDecDst[i].u32Config);
        /* 解码目的在A核维护 */
        //g_strSocpStat.sDecDstChan[i].sDecDstBuf.u32Read = g_aSocpDrxDecDst[i].u32StartAddr;
        //g_strSocpStat.sDecDstChan[i].sDecDstBuf.u32Write = g_aSocpDrxDecDst[i].u32StartAddr;
    }


    /*恢复编码源通道寄存器*/
    for(i=0;i<SOCP_MAX_ENCSRC_CHN;i++)//SOCP_TOTAL_ENCSRC_CHN
    {
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(i), g_aSocpDrxEncSrc[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(i), g_aSocpDrxEncSrc[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(i), g_aSocpDrxEncSrc[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFCFG0(i), g_aSocpDrxEncSrc[i].u32Config0);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQADDR(i), g_aSocpDrxEncSrc[i].u32RDStartAddr);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQRPTR(i), g_aSocpDrxEncSrc[i].u32RDStartAddr);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQWPTR(i), g_aSocpDrxEncSrc[i].u32RDStartAddr);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQCFG(i),  g_aSocpDrxEncSrc[i].u32RDConfig);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFCFG1(i), (g_aSocpDrxEncSrc[i].u32Config1));
   }

    /*恢复解码源通道寄存器*/
    for(i=0;i<(SOCP_MAX_DECSRC_CHN);i++)
    {
        SOCP_REG_WRITE(SOCP_REG_DECSRC_BUFADDR(i), g_aSocpDrxDecSrc[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_DECSRC_BUFWPTR(i), g_aSocpDrxDecSrc[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_DECSRC_BUFRPTR(i), g_aSocpDrxDecSrc[i].u32StartAddr);
        SOCP_REG_WRITE(SOCP_REG_DECSRC_BUFCFG0(i), g_aSocpDrxDecSrc[i].u32Config );
    }

    for(i=0;i<SOCP_MAX_ENCSRC_CHN;i++)
    {
	    g_strSocpStat.sEncSrcChan[i].sEncSrcBuf.u32Write= g_aSocpDrxEncSrc[i].u32StartAddr;
	    g_strSocpStat.sEncSrcChan[i].sEncSrcBuf.u32Read = g_aSocpDrxEncSrc[i].u32StartAddr;
        g_strSocpStat.sEncSrcChan[i].sRdBuf.u32Write= g_aSocpDrxEncSrc[i].u32RDStartAddr;
	    g_strSocpStat.sEncSrcChan[i].sRdBuf.u32Read = g_aSocpDrxEncSrc[i].u32RDStartAddr;
    }
} /*lint !e529 */

/*****************************************************************************
* 函 数 名      : bsp_socp_cansleep
*
* 功能描述  : 低功耗函数
*
* 输入参数  : 无
*
* 输出参数  : BSP_OK  可以进入低功耗状态
*             其他值，不能进入低功耗状态
*
* 返 回 值      : 无
*****************************************************************************/
u32 bsp_socp_cansleep(void)
{
	u32 i;
	u32 u32EncBusy, u32DecBusy;
	u32 u32BufAddr, u32WritePtr, u32ReadPtr;
    u32 u32IntFlag;

	/*判断各通道的读写指针是否相等*/
	/*申请的编码源通道0--13，A核:0--5;M核: 6--13*/
	for(i = SOCP_ENCSRC_CHN_BASE;i<(SOCP_ENCSRC_CHN_BASE+SOCP_ENCSRC_CHN_NUM);i++)
	{
       	SOCP_REG_READ(SOCP_REG_ENCSRC_BUFADDR(i), u32BufAddr);
		if(0!=u32BufAddr)
		{
			SOCP_REG_READ(SOCP_REG_ENCSRC_BUFWPTR(i), u32WritePtr);
			SOCP_REG_READ(SOCP_REG_ENCSRC_BUFRPTR(i), u32ReadPtr);
			if(u32WritePtr != u32ReadPtr)
			{
				//BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SOCP, "SOCP_PwDown: enc src %d is busy: !\n", i);
              	return 1;
			}
		}
	}

	for(i = 0;i<SOCP_MAX_DECDST_CHN;i++)
	{
       	SOCP_REG_READ(SOCP_REG_DECDEST_BUFADDR(i), u32BufAddr);
		if(0!=u32BufAddr)
		{
			SOCP_REG_READ(SOCP_REG_DECDEST_BUFWPTR(i), u32WritePtr);
			SOCP_REG_READ(SOCP_REG_DECDEST_BUFRPTR(i), u32ReadPtr);
			if(u32WritePtr != u32ReadPtr)
			{
				//BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SOCP, "SOCP_PwDown: dec dst %d is busy: !\n", i);
                return 5;
			}
		}
	}

	SOCP_REG_READ(SOCP_REG_ENCSTAT, u32EncBusy);
	SOCP_REG_READ(SOCP_REG_DECSTAT, u32DecBusy);

    if((0 !=u32EncBusy) || (0 !=u32DecBusy) )
	{
		//BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SOCP, "SOCP_PwDown: chan is busy,enc is : 0x%x, dec is 0x%x!\n", u32EncBusy, u32DecBusy);
        return 6;
	}

    SOCP_REG_READ(SOCP_REG_GBL_INTSTAT, u32IntFlag);
    if((0 != u32IntFlag ))
	{
		//BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SOCP, "SOCP_PwDown: chan is busy,enc is : 0x%x, dec is 0x%x!\n", u32EncBusy, u32DecBusy);
        return 7;
	}

	return BSP_OK;
}

void bsp_socp_stop_dsp(void)
{
	u32 IdleFlag;
    u32 j;
	
	if(SOCP_REG_GETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 0, 1))
	{
		SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 0, 1, 0);
	}

	for (j = 0; j < (SOCP_RESET_TIME*2); j++)
    {
        IdleFlag = SOCP_REG_GETBITS(SOCP_REG_ENCSTAT, SOCP_DSPLOG_CHN, 1);
        if (0 == IdleFlag)
        {
            break;
        }

        if ((SOCP_RESET_TIME - 1) == j)
        {
            printf("bsp_socp_stop_dsp failed!\n");

			return ;
        }
    }	
}

void bsp_socp_start_dsp(void)
{
	if(0==(SOCP_REG_GETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 0, 1)))
	{
		SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(SOCP_DSPLOG_CHN), 0, 1, 1);
	}
	
}
/* 低功耗相关 end */

/*****************************************************************************
* 函 数 名  : bsp_socp_get_state
*
* 功能描述  : 获取SOCP状态
*
* 返 回 值  : SOCP_IDLE    空闲
*             SOCP_BUSY    忙碌
*****************************************************************************/
SOCP_STATE_ENUM_UINT32 bsp_socp_get_state(void)
{
    u32 reg_value;

    reg_value = readl(INTEGRATOR_SC_BASE + SOCP_STATE_SC_OFFSET); 
    /* P530BB bit0指示状态 0 --- 工作；1 --- 空闲 */

    if(1 == (reg_value & SOCP_STATE_SC_MASK))
    {
        return SOCP_IDLE;
    }

    return SOCP_BUSY;
}

void  bsp_socp_enable_lte_dsp_bbp(u32 ulChanId)
{
	if(0==(SOCP_REG_GETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 0, 1)))
	{
		SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulChanId), 0, 1, 1);
	}
}


u32  bsp_socp_init_lte_dsp(u32 ulChanId,u32 ulPhyAddr,u32 ulSize)
{
    SOCP_ENCSRC_CHAN_S *pChan;

    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(ulChanId),(u32)ulPhyAddr);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(ulChanId),(u32)ulPhyAddr);
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(ulChanId), (u32)ulPhyAddr);
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

    pChan = &g_strSocpStat.sEncSrcChan[ulChanId];
    pChan->u32ChanID               = ulChanId;
    pChan->u32DestChanID           = SOCP_DSPLOG_DST_BUFID;
    pChan->eChnMode                = SOCP_ENCSRC_CHNMODE_CTSPACKET;
    pChan->eDataType               = SOCP_DATA_TYPE_0;
    pChan->ePriority               = SOCP_CHAN_PRIORITY_0;
    pChan->eDataTypeEn             = SOCP_DATA_TYPE_EN;
    pChan->eDebugEn                = SOCP_ENC_DEBUG_DIS;
    pChan->sEncSrcBuf.u32Start     = (u32)ulPhyAddr;
    pChan->sEncSrcBuf.u32Write     = (u32)ulPhyAddr;
    pChan->sEncSrcBuf.u32End       = (u32)(ulPhyAddr+ulSize);
    pChan->sEncSrcBuf.u32Read      = (u32)ulPhyAddr;
    pChan->sEncSrcBuf.u32Length    = (u32)ulSize;
    pChan->u32ChanEn               = SOCP_CHN_ENABLE;

    return BSP_OK;
}
/*lint -restore +e18*/

/*****************************************************************************
* 函 数 名  :  bsp_socp_init_lte_bbp_log
*
* 功能描述  : LTE BBP通道初始化函数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
u32  bsp_socp_init_lte_bbp_log(u32 ulChanId,u32 ulPhyAddr,u32 ulSize)
{
    SOCP_ENCSRC_CHAN_S * pChan;

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

    pChan = &g_strSocpStat.sEncSrcChan[ulChanId];

    pChan->u32ChanID               = ulChanId;
    pChan->u32DestChanID           = SOCP_BBPLOG_DST_BUFID;
    pChan->eChnMode                = SOCP_ENCSRC_CHNMODE_CTSPACKET;
    pChan->eDataType               = SOCP_DATA_TYPE_0;
    pChan->ePriority               = SOCP_CHAN_PRIORITY_0;
    pChan->eDataTypeEn             = SOCP_DATA_TYPE_EN;
    pChan->eDebugEn                = SOCP_ENC_DEBUG_DIS;
    pChan->sEncSrcBuf.u32Start     = ulPhyAddr;
    pChan->sEncSrcBuf.u32Write     = ulPhyAddr;
    pChan->sEncSrcBuf.u32Read      = ulPhyAddr;
    pChan->sEncSrcBuf.u32End       = ulPhyAddr + ulSize;
    pChan->sEncSrcBuf.u32Length    = (u32)ulSize;
    pChan->u32ChanEn               = SOCP_CHN_ENABLE;

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  :  bsp_socp_int_lte_bbp_ds
*
* 功能描述  : LTE BBP通道初始化函数, DS通道访问的内存空间在K3为非安全内存
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
u32  bsp_socp_int_lte_bbp_ds(u32 ulChanId,u32 ulPhyAddr,u32 ulSize)
{
   SOCP_ENCSRC_CHAN_S * pChan;

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
    pChan = &g_strSocpStat.sEncSrcChan[ulChanId];
    pChan->u32ChanID               = ulChanId;
    pChan->u32DestChanID           = SOCP_BBPDS_DST_BUFID;
    pChan->eChnMode                = SOCP_ENCSRC_CHNMODE_FIXPACKET;
    pChan->eDataType               = SOCP_DATA_TYPE_0;
    pChan->ePriority               = SOCP_CHAN_PRIORITY_0;
    pChan->eDataTypeEn = SOCP_DATA_TYPE_EN;
    pChan->eDebugEn   = SOCP_ENC_DEBUG_DIS;
    pChan->sEncSrcBuf.u32Start     = (u32)ulPhyAddr;
    pChan->sEncSrcBuf.u32Write     = (u32)ulPhyAddr;
    pChan->sEncSrcBuf.u32Read      = (u32)ulPhyAddr;
    pChan->sEncSrcBuf.u32End       = (u32)(ulPhyAddr+ulSize);
    pChan->sEncSrcBuf.u32Length    = (u32)ulSize;

    return BSP_OK;
}
#ifdef __cplusplus
}
#endif

/*lint -restore +e516 +e63 +e40 +e506*/

