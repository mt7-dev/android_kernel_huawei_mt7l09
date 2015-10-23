
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "audio_player.h"
#include "audio_pcm.h"
#include "mp3_dec.h"
#include "xa_aac_dec.h"
#include "om_log.h"
#include "xa_src_pp.h"
#include "dm3.h"
#include "ucom_mem_dyn.h"
#include "audio_enhance.h"
#include "codec_com_codec.h"
#include "om_cpuview.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_AUDIO_PLAYER_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 保存MP3/AAC等音频通路相关控制信息 */
UCOM_SET_UNINIT
AUDIO_PLAYER_OBJ_STRU                   g_stAudioPlayerObj;

/* 保存解码后码流播放Buffer信息 */
AUDIO_PLAYER_BUF_STRU                   g_stAudioPlayerBuff;

/* 备份用交换分区 */
AUDIO_PLAYER_SWAP_BUF_STRU              g_stAudioPlayerSwapBuf;

/* 输入缓存 */
AUDIO_PLAYER_IN_BUF_STRU                g_stAudioPlayerInBuf;

/* 输出内存指针 */
VOS_VOID                               *g_pvOutputBuff = VOS_NULL;

/* SRC一帧处理完数据缓冲 */
AUDIO_PLAYER_SRC_BUF_STRU               g_stAudioPlayerSrcBuff;

/* DTS音效处理之后的缓冲buff */
UCOM_SEC_TCMBSS
AUDIO_PLAYER_DTS_BUF_STRU               g_stAudioPlayerDtsBuff;


/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID AUDIO_PLAYER_InitBuff( VOS_VOID )
{
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer;
    AUDIO_PLAYER_BUF_STRU              *pstAudioPlayerBuff;
    AUDIO_PLAYER_SWAP_BUF_STRU         *pstAudioPlayerSwapBuf;
    AUDIO_PLAYER_IN_BUF_STRU           *pstAudioPlayerInBuf;
    AUDIO_PLAYER_SRC_BUF_STRU          *pstAudioPlayerSrcBuff;
    AUDIO_PLAYER_DTS_BUF_STRU          *pstAudioPlayerDtsBuff;

    /* 保护AudioPlayer控制结构体 */
    pstPlayer  = AUDIO_PlayerGetPlayPtr();

    pstPlayer->uwProtectWord1 = UCOM_PROTECT_WORD1;
    pstPlayer->uwProtectWord2 = UCOM_PROTECT_WORD2;
    pstPlayer->uwProtectWord3 = UCOM_PROTECT_WORD3;
    pstPlayer->uwProtectWord4 = UCOM_PROTECT_WORD4;

    /* 保护解码输出 */
    pstAudioPlayerBuff = AUDIO_PlayerGetPlayBuf();

    UCOM_MemSet(pstAudioPlayerBuff, 0, sizeof(AUDIO_PLAYER_BUF_STRU));

    pstAudioPlayerBuff->uwProtectWord1 = UCOM_PROTECT_WORD1;
    pstAudioPlayerBuff->uwProtectWord2 = UCOM_PROTECT_WORD2;
    pstAudioPlayerBuff->uwProtectWord3 = UCOM_PROTECT_WORD3;
    pstAudioPlayerBuff->uwProtectWord4 = UCOM_PROTECT_WORD4;

    /* 保护交换区 */
    pstAudioPlayerSwapBuf   = AUDIO_PlayerGetSwapBuf();

    UCOM_MemSet(pstAudioPlayerSwapBuf, 0, sizeof(AUDIO_PLAYER_SWAP_BUF_STRU));

    pstAudioPlayerSwapBuf->uwProtectWord1 = UCOM_PROTECT_WORD1;
    pstAudioPlayerSwapBuf->uwProtectWord2 = UCOM_PROTECT_WORD2;
    pstAudioPlayerSwapBuf->uwProtectWord3 = UCOM_PROTECT_WORD3;
    pstAudioPlayerSwapBuf->uwProtectWord4 = UCOM_PROTECT_WORD4;

    /* 保护解码输入 */
    pstAudioPlayerInBuf = AUDIO_PlayerGetInputBuf();

    UCOM_MemSet(pstAudioPlayerInBuf, 0, sizeof(AUDIO_PLAYER_IN_BUF_STRU));

    pstAudioPlayerInBuf->uwProtectWord1 = UCOM_PROTECT_WORD1;
    pstAudioPlayerInBuf->uwProtectWord2 = UCOM_PROTECT_WORD2;
    pstAudioPlayerInBuf->uwProtectWord3 = UCOM_PROTECT_WORD3;
    pstAudioPlayerInBuf->uwProtectWord4 = UCOM_PROTECT_WORD4;

    /* 保护变采样 */
    pstAudioPlayerSrcBuff = AUDIO_PlayerGetSrcBuff();

    UCOM_MemSet(pstAudioPlayerSrcBuff, 0, sizeof(AUDIO_PLAYER_SRC_BUF_STRU));

    pstAudioPlayerSrcBuff->uwProtectWord1 = UCOM_PROTECT_WORD1;
    pstAudioPlayerSrcBuff->uwProtectWord2 = UCOM_PROTECT_WORD2;
    pstAudioPlayerSrcBuff->uwProtectWord3 = UCOM_PROTECT_WORD3;
    pstAudioPlayerSrcBuff->uwProtectWord4 = UCOM_PROTECT_WORD4;

    /* 保护DTS */
    pstAudioPlayerDtsBuff = AUDIO_PlayerGetDtsBuff();

    UCOM_MemSet(pstAudioPlayerDtsBuff, 0, sizeof(AUDIO_PLAYER_DTS_BUF_STRU));

    pstAudioPlayerDtsBuff->uwProtectWord1 = UCOM_PROTECT_WORD1;
    pstAudioPlayerDtsBuff->uwProtectWord2 = UCOM_PROTECT_WORD2;
    pstAudioPlayerDtsBuff->uwProtectWord3 = UCOM_PROTECT_WORD3;
    pstAudioPlayerDtsBuff->uwProtectWord4 = UCOM_PROTECT_WORD4;

}


VOS_UINT32 AUDIO_PLAYER_CheckBuff( VOS_VOID )
{
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer;
    AUDIO_PLAYER_BUF_STRU              *pstAudioPlayerBuff;
    AUDIO_PLAYER_SWAP_BUF_STRU         *pstAudioPlayerSwapBuf;
    AUDIO_PLAYER_IN_BUF_STRU           *pstAudioPlayerInBuf;
    AUDIO_PLAYER_SRC_BUF_STRU          *pstAudioPlayerSrcBuff;
    AUDIO_PLAYER_DTS_BUF_STRU          *pstAudioPlayerDtsBuff;
    VOS_UINT32                          uwRet;

    uwRet = 0;

    /* 检查AudioPlayer控制结构体 */
    pstPlayer  = AUDIO_PlayerGetPlayPtr();

    uwRet += (pstPlayer->uwProtectWord1 ^ UCOM_PROTECT_WORD1);
    uwRet += (pstPlayer->uwProtectWord2 ^ UCOM_PROTECT_WORD2);
    uwRet += (pstPlayer->uwProtectWord3 ^ UCOM_PROTECT_WORD3);
    uwRet += (pstPlayer->uwProtectWord4 ^ UCOM_PROTECT_WORD4);

    if(uwRet != 0)
    {
        OM_LogError1(AUDIO_PlayerProtectWordFail, uwRet);
    }

    /* 检查解码输出 */
    pstAudioPlayerBuff = AUDIO_PlayerGetPlayBuf();

    uwRet += (pstAudioPlayerBuff->uwProtectWord1 ^ UCOM_PROTECT_WORD1);
    uwRet += (pstAudioPlayerBuff->uwProtectWord2 ^ UCOM_PROTECT_WORD2);
    uwRet += (pstAudioPlayerBuff->uwProtectWord3 ^ UCOM_PROTECT_WORD3);
    uwRet += (pstAudioPlayerBuff->uwProtectWord4 ^ UCOM_PROTECT_WORD4);

    if(uwRet != 0)
    {
        OM_LogError1(AUDIO_PlayerProtectWordFail, uwRet);
    }

    /* 检查交换区 */
    pstAudioPlayerSwapBuf   = AUDIO_PlayerGetSwapBuf();

    uwRet += (pstAudioPlayerSwapBuf->uwProtectWord1 ^ UCOM_PROTECT_WORD1);
    uwRet += (pstAudioPlayerSwapBuf->uwProtectWord2 ^ UCOM_PROTECT_WORD2);
    uwRet += (pstAudioPlayerSwapBuf->uwProtectWord3 ^ UCOM_PROTECT_WORD3);
    uwRet += (pstAudioPlayerSwapBuf->uwProtectWord4 ^ UCOM_PROTECT_WORD4);

    if(uwRet != 0)
    {
        OM_LogError1(AUDIO_PlayerProtectWordFail, uwRet);
    }

    /* 检查解码输入 */
    pstAudioPlayerInBuf = AUDIO_PlayerGetInputBuf();

    uwRet += (pstAudioPlayerInBuf->uwProtectWord1 ^ UCOM_PROTECT_WORD1);
    uwRet += (pstAudioPlayerInBuf->uwProtectWord2 ^ UCOM_PROTECT_WORD2);
    uwRet += (pstAudioPlayerInBuf->uwProtectWord3 ^ UCOM_PROTECT_WORD3);
    uwRet += (pstAudioPlayerInBuf->uwProtectWord4 ^ UCOM_PROTECT_WORD4);

    if(uwRet != 0)
    {
        OM_LogError1(AUDIO_PlayerProtectWordFail, uwRet);
    }

    /* 检查变采样 */
    pstAudioPlayerSrcBuff = AUDIO_PlayerGetSrcBuff();

    uwRet += (pstAudioPlayerSrcBuff->uwProtectWord1 ^ UCOM_PROTECT_WORD1);
    uwRet += (pstAudioPlayerSrcBuff->uwProtectWord2 ^ UCOM_PROTECT_WORD2);
    uwRet += (pstAudioPlayerSrcBuff->uwProtectWord3 ^ UCOM_PROTECT_WORD3);
    uwRet += (pstAudioPlayerSrcBuff->uwProtectWord4 ^ UCOM_PROTECT_WORD4);

    if(uwRet != 0)
    {
        OM_LogError1(AUDIO_PlayerProtectWordFail, uwRet);
    }

    /* 检查DTS */
    pstAudioPlayerDtsBuff = AUDIO_PlayerGetDtsBuff();

    uwRet += (pstAudioPlayerDtsBuff->uwProtectWord1 ^ UCOM_PROTECT_WORD1);
    uwRet += (pstAudioPlayerDtsBuff->uwProtectWord2 ^ UCOM_PROTECT_WORD2);
    uwRet += (pstAudioPlayerDtsBuff->uwProtectWord3 ^ UCOM_PROTECT_WORD3);
    uwRet += (pstAudioPlayerDtsBuff->uwProtectWord4 ^ UCOM_PROTECT_WORD4);

    if(uwRet != 0)
    {
        OM_LogError1(AUDIO_PlayerProtectWordFail, uwRet);
    }

    return uwRet;

}


VOS_UINT32 AUDIO_PLAYER_ApiInit(VOS_VOID)
{
    AUDIO_PLAYER_OBJ_STRU       *pstPlayer   = VOS_NULL;
    VOS_UINT32                  uwPcmWdSz    = AUDIO_PLAYER_PCM_WIDTH_SIZE;
    XA_AUDIO_DEC_USER_CFG_STRU  stAacUserCfg;
    XA_AUDIO_DEC_USER_CFG_STRU  stMp3UserCfg;
    XA_SRC_USER_CFG_STRU        stSrcUserCfg;
    XA_DTS_USER_CFG_STRU        stDtsUserCfg;
    VOS_UINT32                  uwApBuffAddr;
    VOS_UINT32                  uwRet       = UCOM_RET_SUCC;

    pstPlayer  = AUDIO_PlayerGetPlayPtr();

    /* 如果是MP3文件,保存第1帧位置 */
    if (AUDIO_FILE_FORMAT_MP3 == pstPlayer->enFormat)
    {
        /* 配置MP3初始化信息 */
        UCOM_MemSet(&stMp3UserCfg, 0, sizeof(XA_AUDIO_DEC_USER_CFG_STRU));
        stMp3UserCfg.pvHeadBuff         = (VOS_VOID *)pstPlayer->uwApBufAddr;
        stMp3UserCfg.swHeaderMemSize    = AUDIO_PLAYER_BUFF_SIZE_512K;
        stMp3UserCfg.uwPcmWidthSize     = uwPcmWdSz;

        /* 对Tensilica MP3解码库进行初始化 */
        uwRet += MP3_DECODER_Init(&stMp3UserCfg);

        /* 由于初始化消耗的AP数据，更新AP指针，AP剩余数据 */
        uwApBuffAddr    = AUDIO_PlayerGetApBufAddr();
        uwApBuffAddr    = uwApBuffAddr + (VOS_UINT32)stMp3UserCfg.swConsumed;
        pstPlayer->uwApDataLeft = pstPlayer->uwApDataLeft - (VOS_UINT32)stMp3UserCfg.swConsumed;
        AUDIO_PlayerSetApBufAddr(uwApBuffAddr);

        pstPlayer->uwDecInMemSize = (VOS_UINT32)stMp3UserCfg.swInSize;

    }
    else if (AUDIO_FILE_FORMAT_AAC == pstPlayer->enFormat)
    {
        /* 配置AAC初始化信息 */
        UCOM_MemSet(&stAacUserCfg, 0, sizeof(XA_AUDIO_DEC_USER_CFG_STRU));
        stAacUserCfg.pvHeadBuff         = (VOS_VOID *)pstPlayer->uwApBufAddr;
        stAacUserCfg.swHeaderMemSize    = AUDIO_PLAYER_BUFF_SIZE_512K;
        stAacUserCfg.uwPcmWidthSize     = uwPcmWdSz;

        /* 对Tensilica AAC解码库进行初始化 */
        uwRet += AAC_DECODER_Init(&stAacUserCfg);

        /* 由于初始化消耗的AP数据，更新AP指针，AP剩余数据 */
        uwApBuffAddr    = AUDIO_PlayerGetApBufAddr();
        uwApBuffAddr    = uwApBuffAddr + (VOS_UINT32)stAacUserCfg.swConsumed;
        pstPlayer->uwApDataLeft = pstPlayer->uwApDataLeft - (VOS_UINT32)stAacUserCfg.swConsumed;
        AUDIO_PlayerSetApBufAddr(uwApBuffAddr);

        pstPlayer->uwDecInMemSize = (VOS_UINT32)stAacUserCfg.swInSize;
    }
    else
    {   /* 其他各式文件不作播放 */
        /* 回复失败ID_AUDIO_AP_PLAY_START_CNF给AP */
        uwRet = UCOM_RET_ERR_PARA;
    }

    /* 初始化解码之后的变采样 */
    stSrcUserCfg.swChannels         = AUDIO_PLAYER_PLAY_CHN_NUM;
    stSrcUserCfg.swInputChunkSize   = XA_SRC_INPUT_CHUNKSIZE_128;
    stSrcUserCfg.swInputSmpRat      = (VOS_INT32)pstPlayer->uwSampleRate;
    stSrcUserCfg.swOutputSmpRat     = AUDIO_PLAYER_OUTPUT_SAMPLE_RATE;

    XA_SRC_Init(XA_SRC_PlAYBACK, &stSrcUserCfg);


    /* 配置DTS对象所需的信息并创建DTS对象 */
    stDtsUserCfg.swChannelNum       = AUDIO_PLAYER_PLAY_CHN_NUM;
    stDtsUserCfg.uwBlockSize        = AUDIO_PLAYER_DTS_CHUNK_SIZE;
    stDtsUserCfg.uwFilterCfg        = 0;
    stDtsUserCfg.uwSampleRate       = AUDIO_PLAYER_OUTPUT_SAMPLE_RATE;

    /* 初始化DTS */
    uwRet += XA_DTS_Init(&stDtsUserCfg);

    return uwRet;
}


VOS_UINT32 AUDIO_PLAYER_Init(VOS_VOID)
{
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer           = VOS_NULL;
    VOS_CHAR                           *pscPlayBufPool      = VOS_NULL;
    VOS_CHAR                           *pscSrcDolbyBufPool  = VOS_NULL;
    VOS_CHAR                           *pscDtsBufPool       = VOS_NULL;
    VOS_UINT32                          uwInitDataUse       = 0;
    VOS_UINT32                          uwCurPos            = 0;
    VOS_UINT32                          uwRet               = UCOM_RET_SUCC;

    /* 得到player全局控制变量 */
    pstPlayer                   = AUDIO_PlayerGetPlayPtr();

    /* 得到解码数据池指针 */
    pscPlayBufPool              = AUDIO_PlayerGetPlayBufPoolPtr();

    /* 得到SRC、DOLBY处理之后的缓冲池 */
    pscSrcDolbyBufPool          = AUDIO_PlayerGetSrcBuffPtr();

    /* 得到DTS处理之后的缓冲池 */
    pscDtsBufPool              = AUDIO_PlayerGetDtsBuffPtr();

    /* 将运行状态设置为runing */
    AUDIO_PlayerSetState(AUDIO_PLAYER_STATE_RUNNING);

    /* 清零已播放buffer的数据读取计数 */
    pstPlayer->uwPlayedBytes    = 0;

    /* 初始化dts音效处理帧数 */
    pstPlayer->stVolume.uwDtsEnableFrame = 0;

    /* 设置当前为静音 */
    pstPlayer->stVolume.uwOriginal  = 0;
    pstPlayer->stVolume.uwCurr      = 0;

    pstPlayer->uwDecBufAddr     = pstPlayer->uwApBufAddr;

    pstPlayer->uwDecDataLeft    = pstPlayer->uwApDataLeft;

    pstPlayer->uhwIsDataBak     = VOS_FALSE;

    /* 播放结果初始化 */
    pstPlayer->enPlayStatus     = AUDIO_PLAY_DONE_NORMAL;

    /* 清空解码后缓冲池信息 */
    AUDIO_PlayerSetPlayBufPoolDepth(0);

    UCOM_MemSet(pscPlayBufPool, 0, AUDIO_PLAYER_BUFF_POOL_SIZE);

    /* 清空SRC、DOLBY处理之后的缓冲池信息 */
    AUDIO_PlayerSetSrcBufPoolDepth(0);

    UCOM_MemSet(pscSrcDolbyBufPool, 0, AUDIO_PLAYER_SRC_DOLBY_BUFF_SIZE);

    /* 清空DTS处理之后的缓冲池 */
    UCOM_MemSet(pscDtsBufPool, 0, AUDIO_PLAYER_DTS_BUFF_SIZE * sizeof(VOS_CHAR));

    /* 执行解码/音效处理等API初始化 */
    uwRet = AUDIO_PLAYER_ApiInit();

    if (UCOM_RET_SUCC == uwRet)
    {
        /* 初始化当前已处理数据计数，包含考虑resume场景 */
        pstPlayer->uwPlayedBytes    = pstPlayer->uwStartPos;

        /* 如果AP Buff中包含文件头信息，则此时头信息已处理完，直接从StartPos开始进行解码 */
        if (VOS_FALSE == pstPlayer->uwFileHeaderRdy)
        {
            pstPlayer->uwFileHeaderRdy  = VOS_TRUE;

            uwInitDataUse   = AUDIO_PLAYER_BUFF_SIZE_512K;
        }
        else
        {
            uwInitDataUse   = pstPlayer->uwStartPos;
        }

        /* 将待解码地址指向AP侧Buff数据初始化消费完之后的地址 */
        pstPlayer->uwDecBufAddr     += uwInitDataUse;

        /* 复位当前播放buffer的数据状态 */
        pstPlayer->uwDecDataLeft    -= uwInitDataUse;

        /* 初始化InputBuff中已消耗的数据，意味着第一帧解码前需要填满InputBuff */
        pstPlayer->uwDecConsumed    = pstPlayer->uwDecInMemSize;

        /* 更新播放位置 */
        uwCurPos = AUDIO_PLAYER_GetCurrDecPos(pstPlayer->enFormat);
        AUDIO_PLAYER_DoSeek(uwCurPos, CODEC_SEEK_BACKWARDS);
        AUDIO_PLAYER_DoSeek(pstPlayer->uwPlayedBytes, CODEC_SEEK_FORWARDS);

    }
    else
    {
        OM_LogError1(AUDIO_PlayerXADecode_InitFail, uwRet);
    }

    return uwRet;
}
VOS_UINT32  AUDIO_PLAYER_IsIdle(VOS_VOID)
{
    /*表示无业务运行，投票:可下电*/
    if (AUDIO_PLAYER_STATE_IDLE   == AUDIO_PlayerGetState())
    {
        return VOS_OK;
    }
    else
    {
        return VOS_ERR;
    }
}
VOS_VOID  AUDIO_PLAYER_CpySwapBuffIsr(
                DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                VOS_UINT32                   uwPara)
{
    /* 若中断类型为TC中断,为DMA正常中断处理流程 */
    if ((DRV_DMA_INT_TYPE_TC1 == enIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == enIntType))
    {
        OM_LogInfo1(AUDIO_PlayerCpySwapBuffOK, enIntType);
    }
    /* 若中断类型为ERROR中断,记录异常 */
    else
    {
        /*记录异常，出现DMA Error中断*/
        OM_LogError1(AUDIO_PlayerCpySwapBuffFail, enIntType);
    }
}
VOS_VOID AUDIO_PLAYER_FillBuff(AUDIO_PLAYER_OBJ_STRU *pstPlayer)
{
    VOS_UINT32                      uwDataLeft      = pstPlayer->uwDecDataLeft;
    VOS_UINT32                      uwDataFill      = pstPlayer->uwDecConsumed;
    VOS_UINT32                      uwDecInputRemind;
    VOS_UINT32                      uwTotalDataLeft = 0;
    VOS_UINT32                      uwAp2InputSize;
    VOS_UINT32                      uwCopy50KCnt,uwCopyLeft,uwCnt;

    if (pstPlayer->uwPlayedBytes < pstPlayer->uwTotalFileSize)
    {
        uwTotalDataLeft = pstPlayer->uwTotalFileSize - pstPlayer->uwPlayedBytes;
    }

    XA_COMM_ShiftBuff((VOS_CHAR*)AUDIO_PlayerGetInputBufPtr(),
                  (VOS_INT32)pstPlayer->uwDecInMemSize,
                  (VOS_INT32)uwDataFill);

    /* 当前播放数据处于主buffer */
    if (VOS_TRUE != pstPlayer->uhwIsDataBak)
    {
        /* 当前AP buffer剩余数据少于200k时进行数据备份，此时DataLeft明确大于DecInMemSize */
        if (uwDataLeft <= AUDIO_PLAYER_BUFF_SIZE_200K)
        {
            /* 计算需要搬运50K的次数 */
            uwCopy50KCnt    = uwDataLeft/AUDIO_PLAYER_BUFF_SIZE_50K;

            /* 如果计算商值错误，返回;防止踩内存 */
            if (uwCopy50KCnt > 4)
            {
                OM_LogError1(AUDIO_PlayerCpySwapBuffFail, uwCopy50KCnt);

                return;
            }

            /* 计算不足50K需要搬运的大小 */
            uwCopyLeft      = uwDataLeft%AUDIO_PLAYER_BUFF_SIZE_50K;

            for (uwCnt = 0; uwCnt < uwCopy50KCnt; uwCnt++)
            {
                /* 搬运50K数据 */
                DRV_DMA_MemCpy(DRV_DMA_AUDIO_MEMCPY_CHN_NUM,
                               (VOS_UCHAR *)UCOM_GetUncachedAddr((VOS_UINT32)AUDIO_PlayerGetSwapBufAddr()+(AUDIO_PLAYER_BUFF_SIZE_50K * uwCnt)),
                               (VOS_UCHAR *)(AUDIO_PlayerGetBufAddr()+(AUDIO_PLAYER_BUFF_SIZE_50K * uwCnt)),
                               AUDIO_PLAYER_BUFF_SIZE_50K,
                               AUDIO_PLAYER_CpySwapBuffIsr,
                               0);

                /* 睡眠0.5ms，确保DMA搬运完毕。*/
                UCOM_COMM_TimeDelayFor(500);
            }

             /* 搬运剩余不足50K的数据 */
            DRV_DMA_MemCpy(DRV_DMA_AUDIO_MEMCPY_CHN_NUM,
                           (VOS_UCHAR *)UCOM_GetUncachedAddr((VOS_UINT32)AUDIO_PlayerGetSwapBufAddr()+(AUDIO_PLAYER_BUFF_SIZE_50K * 3)),
                           (VOS_UCHAR *)(AUDIO_PlayerGetBufAddr()+(AUDIO_PLAYER_BUFF_SIZE_50K * uwCopy50KCnt)),
                           uwCopyLeft,
                           AUDIO_PLAYER_CpySwapBuffIsr,
                           0);

            /* 睡眠0.5ms，确保DMA搬运完毕。*/
            UCOM_COMM_TimeDelayFor(500);

            /* 切换待解码数据Buff地址 */
            AUDIO_PlayerSetBufAddr((VOS_UINT32)AUDIO_PlayerGetSwapBufAddr());

            /* 初始化待解码数据 */
            pstPlayer->uwDecDataLeft    = uwDataLeft;

            pstPlayer->uwApDataLeft     = 0;

            /* 标志数据读取从辅助buffer开始 */
            pstPlayer->uhwIsDataBak     = VOS_TRUE;

            if (uwTotalDataLeft > AUDIO_PLAYER_BUFF_SIZE_200K)
            {
               /* 通知AP当期播放数据需要更新 */
               AUDIO_PLAYER_SendPlayDoneInd(AUDIO_PLAY_DONE_NORMAL);
            }
        }

        /* 获取能够填充的待解码数据大小，此时可以保证足够填充一次DecInput，不需要更新填充长度 */
    }
    else
    {
        if (uwDataLeft < pstPlayer->uwDecConsumed)
        {
            /* 当前播放数据处于sub buffer且备份缓存中数据不足，切换至AP Buff中且直接填充InputBuff*/
            if ((uwDataLeft + pstPlayer->uwApDataLeft)>= pstPlayer->uwDecConsumed)
            {
                uwDataFill = pstPlayer->uwDecInMemSize - uwDataLeft;

                /* tensilica lib库所用解码inputbuff还剩余可用数据大小 */
                uwDecInputRemind    = pstPlayer->uwDecInMemSize - pstPlayer->uwDecConsumed;

                /* tensilica lib库所用解码inputbuff还需要从AP拷贝的数据大小 */
                uwAp2InputSize      = pstPlayer->uwDecInMemSize - (uwDecInputRemind + uwDataLeft);

                UCOM_MemCpy((AUDIO_PlayerGetInputBufPtr() + uwDecInputRemind),
                            AUDIO_PlayerGetBufAddr(),
                            uwDataLeft);

                UCOM_MemCpy((AUDIO_PlayerGetInputBufPtr() + uwDecInputRemind + uwDataLeft),
                            AUDIO_PlayerGetApBufAddr(),
                            uwAp2InputSize);

                /* 切换待解码数据Buff地址 */
                AUDIO_PlayerSetBufAddr(AUDIO_PlayerGetApBufAddr()+ uwAp2InputSize);

                /* 初始化待解码数据 */
                pstPlayer->uwDecDataLeft    = pstPlayer->uwApDataLeft - uwAp2InputSize;

                /* 标志数据读取从主buffer开始 */
                pstPlayer->uhwIsDataBak     = VOS_FALSE;

                /* 清空辅buff，以免引入噪音 */
                UCOM_MemSet(AUDIO_PlayerGetSwapBufAddr(), 0, AUDIO_PLAYER_BUFF_SIZE_200K);

                return;
            }
            else
            {
                /* 若备份数据不足且AP侧数据未更新，则仅填充现有备份数据 */
                uwDataFill = uwDataLeft;

                pstPlayer->uwPlayedBytes = pstPlayer->uwTotalFileSize;

                /* 告警，但此情况可能是由于用户不进行连续播放引起 */
                OM_LogWarning1(AUDIO_PlayerFillBuff_NoApDataUpdate, uwDataLeft);
            }
        }
    }

    /* 从当前DecBuffer中填充InputBuff */
    UCOM_MemCpy(((VOS_CHAR *)AUDIO_PlayerGetInputBufPtr() + pstPlayer->uwDecInMemSize) - uwDataFill,
                AUDIO_PlayerGetBufAddr(),
                uwDataFill);

    /* 更新待解码Buff地址 */
    AUDIO_PlayerSetBufAddr((VOS_UINT32)AUDIO_PlayerGetBufAddr() + uwDataFill);

    /* 更新待解码数据长度 */
    pstPlayer->uwDecDataLeft -= uwDataFill;

}



VOS_UINT32 AUDIO_PLAYER_DoPlay(VOS_VOID)
{
    AUDIO_PLAYER_OBJ_STRU         *pstPlayer    = VOS_NULL ;
    CODEC_STATE_ENUM_UINT16        enDecoderState;

    /* 获取播放模块控制体 */
    pstPlayer      = AUDIO_PlayerGetPlayPtr();

    enDecoderState = CODEC_STATE_INITIALIZED;

    /* MP3/AAC播放通道Buff状态为Active，说明AP提供的Buff中有数据需要播出 */
    if (  (AUDIO_PLAYER_STATE_RUNNING == pstPlayer->enState)
       || (AUDIO_PLAYER_STATE_PAUSE   == pstPlayer->enState))
    {
        /* 处理MP3/AAC音频数据，得到处理后码流 */
        AUDIO_PLAYER_HandleFile(&enDecoderState);

        /* 如果一首音乐文件播放完毕，关闭通路并通知AP */
        if (( pstPlayer->uwPlayedBytes >= pstPlayer->uwTotalFileSize)
           ||(CODEC_STATE_INITIALIZED != enDecoderState))
        {
            /* 如果播放完毕或异常中断，关闭通路 */
            AUDIO_PLAYER_DoStop();

            /* 申请停止PCM工作通道 */
            AUDIO_PLAYER_PcmStop();

            /* 调用跨核接口,回复AP消息ID_AUDIO_AP_PLAY_DONE_IND播放完成 */
            if (CODEC_STATE_FINISH == enDecoderState)
            {
                pstPlayer->enPlayStatus = AUDIO_PLAY_DONE_COMPLETE;

                AUDIO_PLAYER_SendPlayDoneInd(AUDIO_PLAY_DONE_COMPLETE);
            }
            else
            {
                pstPlayer->enPlayStatus = AUDIO_PLAY_DONE_ABNORMAL;

                AUDIO_PLAYER_SendPlayDoneInd(AUDIO_PLAY_DONE_ABNORMAL);
            }

            return UCOM_RET_SUCC;
        }
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 AUDIO_PLAYER_DoSeek(VOS_UINT32 uwSkipSize, CODEC_SEEK_DERECT_ENUM_UINT16 enSeekDirect)
{
    VOS_UINT32                     uwResult     = VOS_OK;
    AUDIO_PLAYER_OBJ_STRU         *pstPlayer    = VOS_NULL;

    pstPlayer       = AUDIO_PlayerGetPlayPtr();

    /* 调用TENSILICA SEEK API判定返回结果 */
    if (AUDIO_FILE_FORMAT_MP3 == pstPlayer->enFormat)
    {
        uwResult = MP3_DECODER_Seek(uwSkipSize, enSeekDirect);
    }
    else if (AUDIO_FILE_FORMAT_AAC == pstPlayer->enFormat)
    {
        uwResult = AAC_DECODER_Seek(uwSkipSize, enSeekDirect);
    }
    else
    {
        uwResult = VOS_ERR;
    }

    return uwResult;
}
VOS_UINT32 AUDIO_PLAYER_DoStop(VOS_VOID)
{
    AUDIO_PLAYER_OBJ_STRU           *pstPlayer       = VOS_NULL;

    pstPlayer = AUDIO_PlayerGetPlayPtr();

    /* 调用XA api关闭decoder */
    if (AUDIO_FILE_FORMAT_MP3 == pstPlayer->enFormat)
    {
        MP3_DECODER_Close();
    }
    else if (AUDIO_FILE_FORMAT_AAC == pstPlayer->enFormat)
    {
        AAC_DECODER_Close();
    }
    else
    {
        return UCOM_RET_FAIL;
    }

    /* XA音效api关闭 */
    //AUDIO_EFFECT_Close();

    /* 更新该通道下Buff使用状态为idle状态 */
    AUDIO_PlayerSetState(AUDIO_PLAYER_STATE_IDLE);

    return UCOM_RET_SUCC;
}
VOS_UINT32  AUDIO_PLAYER_GetCurrDecPos(AUDIO_FILE_FORMAT_ENUM_UINT16 enFormat)
{
    VOS_UINT32              uwPosInBytes = 0;
    VOS_INT32               swErrCode    = XA_NO_ERROR;

    /* 如果解码MP3 */
    if ( AUDIO_FILE_FORMAT_MP3 == enFormat )
    {
        swErrCode = MP3_DECODER_GetCurrDecPos(&uwPosInBytes);
    }
    else if ( AUDIO_FILE_FORMAT_AAC == enFormat )
    {
        swErrCode = AAC_DECODER_GetCurrDecPos(&uwPosInBytes);
    }
    else
    {
        return UCOM_RET_FAIL;
    }

    /* 入参检查，如果该函数执行错误，将播放进度重置为0 */
    if (swErrCode != XA_NO_ERROR)
    {
        uwPosInBytes = 0;
    }

    return uwPosInBytes;
}


VOS_UINT16 AUDIO_PLAYER_GetCurrVol( VOS_VOID )
{
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer       = VOS_NULL;

    /* 获取播放模块控制体 */
    pstPlayer       = AUDIO_PlayerGetPlayPtr();

    /* 非运行状态全部静音 */
    if(AUDIO_PlayerGetState() != AUDIO_PLAYER_STATE_RUNNING)
    {
        return 0;
    }

    if (AUDIO_PLAYER_DTS_ENABLE_VOL == pstPlayer->stVolume.enSetVolEnum)
    {
        AUDIO_PLAYER_DtsMorphingVol();
    }
    else
    {
        if(pstPlayer->stVolume.uwCurr != pstPlayer->stVolume.uwTarget)
        {
            /* 音量逐级变动, 50帧调整到位 */
            pstPlayer->stVolume.uwCurr += (pstPlayer->stVolume.uwTarget - pstPlayer->stVolume.uwOriginal)/50;

            if(pstPlayer->stVolume.uwTarget >= pstPlayer->stVolume.uwOriginal)
            {
                if(pstPlayer->stVolume.uwCurr >= pstPlayer->stVolume.uwTarget)
                {
                    pstPlayer->stVolume.uwCurr = pstPlayer->stVolume.uwTarget;
                }
            }
            else
            {
                if(pstPlayer->stVolume.uwCurr <= pstPlayer->stVolume.uwTarget)
                {
                    pstPlayer->stVolume.uwCurr = pstPlayer->stVolume.uwTarget;
                }
            }
        }
    }

    return (VOS_UINT16) pstPlayer->stVolume.uwCurr;
}
VOS_UINT32 AUDIO_PLAYER_MsgStartReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_PLAYER_START_REQ_STRU        *pstStartReqMsg      = VOS_NULL;
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer           = VOS_NULL;
    AUDIO_PLAYER_DECODE_IND_STRU        stDecodeInd;
    VOS_UINT32                          uwRet;
    VOS_VOID                           *pvMsgTail      = VOS_NULL;

    /* 按AUDIO_PLAYER_START_REQ_STRU格式解析消息 */
    pstStartReqMsg    = (AUDIO_PLAYER_START_REQ_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 该消息需要将原消息的CmdId及Sn号回复给Hifi_misc驱动 */
    pvMsgTail       = (VOS_VOID *)((VOS_UINT32)(((MsgBlock *) pvOsaMsg)->aucValue) + sizeof(AUDIO_PLAYER_START_REQ_STRU));

    /* 检查入参 */
    if (  (pstStartReqMsg->enFormat >= AUDIO_FILE_FORMAT_BUT)
        ||(pstStartReqMsg->uwSampleRate > AUDIO_PCM_MAX_SAMPLE_RATE))
    {
        return UCOM_RET_ERR_PARA;
    }

    UCOM_MemSet(&stDecodeInd, 0, sizeof(AUDIO_PLAYER_DECODE_IND_STRU));

    pstPlayer   = AUDIO_PlayerGetPlayPtr();

    /* 开始解码位置  */
    pstPlayer->uwStartPos          = pstStartReqMsg->uwStartPos;

    /* 本次UpdateBuff是否包含文件头，若包含则需要更新文件头 */
    if (VOS_TRUE == pstStartReqMsg->uhwWithHeader)
    {
        pstPlayer->uwFileHeaderRdy = VOS_FALSE;
    }
    else
    {
        pstPlayer->uwFileHeaderRdy = VOS_TRUE;
    }

    /* 保存指定通道的音频信息，保存于对应的全局变量通道中 */
    pstPlayer->enFormat             = pstStartReqMsg->enFormat;
    pstPlayer->uwSampleRate         = pstStartReqMsg->uwSampleRate;

    /* 计算DMA搬运长度，模拟codec实际仅支持采样率48k,16bit数据传递, 并且搬运时只采用双声道，所以每次搬运0xF00 */
    pstPlayer->uw20msDataSize       = ((pstStartReqMsg->uwSampleRate * AUDIO_PLAYER_PLAY_CHN_NUM)\
                                       / AUDIO_PCM_FRAME_PER_SEC) * AUDIO_PCM_FORMAT_16_SIZE;

    pstPlayer->uwTotalFileSize      = pstStartReqMsg->uwTotalFileSize;

    /* 执行播放器初始化 */
    uwRet = AUDIO_PLAYER_Init();

    /* 初始化成功继续往下走，失败直接返回 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* 打开PCM通道，开始数据传输 */
        AUDIO_PLAYER_PcmStart();

        /* 初始化成功，开始解码数据，进行声效处理后填充播放buffer池 */
        stDecodeInd.uhwMsgId = ID_AUDIO_PLAYER_START_DECODE_IND;

        /* 调用通用VOS发送接口，发送消息 */
        UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                        DSP_PID_AUDIO,
                       &stDecodeInd,
                        sizeof(AUDIO_PLAYER_DECODE_IND_STRU));
    }
    else
    {
        /* 如果初始化失败，停止流程执行 */
        stDecodeInd.uhwMsgId = ID_AUDIO_PLAYER_STOP_DECODE_IND;

        /* 调用通用VOS发送接口，发送消息 */
        UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                        DSP_PID_AUDIO,
                       &stDecodeInd,
                        sizeof(AUDIO_PLAYER_DECODE_IND_STRU));

        OM_LogError1(AUDIO_PlayerXADecode_InitFail, uwRet);
    }

    /* 回复AP start req结果 */
    AUDIO_PLAYER_SendApAudioCnf(ID_AUDIO_AP_PLAY_START_CNF,
                                uwRet,
                                pvMsgTail,
                                sizeof(AUDIO_HIFIMISC_MSG_TAIL_STRU));

    return uwRet;
}
VOS_UINT32  AUDIO_PLAYER_MsgStartDecodeInd(VOS_VOID *pvOsaMsg)
{
    AUDIO_PLAYER_DoPlay();

    return UCOM_RET_SUCC;
}


VOS_UINT32  AUDIO_PLAYER_MsgStopDecodeInd(VOS_VOID *pvOsaMsg)
{
    AUDIO_PLAYER_DoStop();

    return UCOM_RET_SUCC;
}


VOS_UINT32 AUDIO_PLAYER_MsgQueryTimeReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer       = VOS_NULL;
    AUDIO_PLAYER_QUERY_TIME_CNF_STRU    stQueryTimeCnfMsg;
    VOS_VOID                           *pvMsgTail      = VOS_NULL;

    UCOM_MemSet(&stQueryTimeCnfMsg, 0, sizeof(AUDIO_PLAYER_QUERY_TIME_CNF_STRU));

    /* 该消息需要将原消息的CmdId及Sn号回复给Hifi_misc驱动 */
    pvMsgTail       = (VOS_VOID *)((VOS_UINT32)(((MsgBlock *) pvOsaMsg)->aucValue) + sizeof(AUDIO_PLAYER_QUERY_TIME_REQ_STRU));

    pstPlayer               = AUDIO_PlayerGetPlayPtr();

    /* 根据当前已播放的进度进行回复 */
    stQueryTimeCnfMsg.uhwMsgId        = ID_AUDIO_AP_PLAY_QUERY_TIME_CNF;
    stQueryTimeCnfMsg.uwPlayedBytes   = pstPlayer->uwPlayedBytes;

    /* 调用跨核音频channel接口,通知AP播放进度 */
    UCOM_COMM_SendDspMsg(DSP_PID_AUDIO,
                         ACPU_PID_OM,           /* 此处填写AP侧DRV对应的PID,,待定 */
                        &stQueryTimeCnfMsg,
                         sizeof(stQueryTimeCnfMsg),
                         pvMsgTail,
                         sizeof(AUDIO_HIFIMISC_MSG_TAIL_STRU));

    return UCOM_RET_SUCC;
}


VOS_UINT32 AUDIO_PLAYER_MsgQueryStatusReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_PLAYER_OBJ_STRU                  *pstPlayer           = VOS_NULL;
    AUDIO_PLAYER_QUERY_STATUS_CNF_STRU      stQueryStatusCnfMsg;
    VOS_VOID                               *pvMsgTail      = VOS_NULL;

    pstPlayer           = AUDIO_PlayerGetPlayPtr();
    /* 该消息需要将原消息的CmdId及Sn号回复给Hifi_misc驱动 */
    pvMsgTail       = (VOS_VOID *)((VOS_UINT32)(((MsgBlock *) pvOsaMsg)->aucValue) + sizeof(AUDIO_PLAYER_QUERY_STATUS_REQ_STRU));

    stQueryStatusCnfMsg.uhwMsgId        = ID_AUDIO_AP_PLAY_QUERY_STATUS_CNF;

    /* 根据当前播放状态回复 */
    stQueryStatusCnfMsg.enPlayStatus    = pstPlayer->enPlayStatus;

    /* 调用跨核音频channel接口,通知AP播放进度 */
    UCOM_COMM_SendDspMsg(DSP_PID_AUDIO,
                         ACPU_PID_OM,           /* 此处填写AP侧DRV对应的PID,,待定 */
                        &stQueryStatusCnfMsg,
                         sizeof(stQueryStatusCnfMsg),
                         pvMsgTail,
                         sizeof(AUDIO_HIFIMISC_MSG_TAIL_STRU));

    return UCOM_RET_SUCC;
}


VOS_UINT32 AUDIO_PLAYER_MsgSeekReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_PLAYER_SEEK_REQ_STRU         *pstSeekReqMsg   = VOS_NULL;
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer       = VOS_NULL;
    AUDIO_PLAY_RESULT_CNF_ENUM_UINT16   enSeekResult;
    VOS_UINT32                          uwResult        = UCOM_RET_FAIL;
    VOS_UINT32                          uwSkipSize;
    CODEC_SEEK_DERECT_ENUM_UINT16       enSeekDirect;
    VOS_UINT32                          uwPosInDecBytes;
    VOS_UINT32                          uwDataLeft;
    VOS_UINT32                          uwDecBuff;
    AUDIO_PLAYER_DECODE_IND_STRU        stDecodeInd;
    VOS_VOID                           *pvMsgTail      = VOS_NULL;

    UCOM_MemSet(&stDecodeInd, 0, sizeof(AUDIO_PLAYER_DECODE_IND_STRU));

    /* 按ID_AP_AUDIO_PLAY_SEEK_REQ格式解析消息 */
    pstSeekReqMsg   = (AUDIO_PLAYER_SEEK_REQ_STRU *)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 该消息需要将原消息的CmdId及Sn号回复给Hifi_misc驱动 */
    pvMsgTail       = (VOS_VOID *)((VOS_UINT32)(((MsgBlock *) pvOsaMsg)->aucValue) + sizeof(AUDIO_PLAYER_SEEK_REQ_STRU));

    pstPlayer       = AUDIO_PlayerGetPlayPtr();

    /* IDLE状态不进行Seek处理 */
    if(AUDIO_PLAYER_STATE_IDLE == AUDIO_PlayerGetState())
    {
        /* 回复AP seek执行结果 */
        AUDIO_PLAYER_SendPlaySeekCnf(AUDIO_PLAY_RESULT_FAIL,
                                     pvMsgTail,
                                     sizeof(AUDIO_HIFIMISC_MSG_TAIL_STRU));

        return UCOM_RET_FAIL;
    }

    /* 先设置player状态为暂停 */
    AUDIO_PlayerSetState(AUDIO_PLAYER_STATE_PAUSE);

    /* 设置当前为静音 */
    pstPlayer->stVolume.uwOriginal  = 0;
    pstPlayer->stVolume.uwCurr      = 0;

    /* 从XA得到已解码位置 */
    uwPosInDecBytes             = AUDIO_PLAYER_GetCurrDecPos(pstPlayer->enFormat);

    if ( pstSeekReqMsg->uwSeekPos > uwPosInDecBytes )
    {
        enSeekDirect = CODEC_SEEK_FORWARDS;
        uwSkipSize   = pstSeekReqMsg->uwSeekPos - uwPosInDecBytes;
    }
    else
    {
        enSeekDirect = CODEC_SEEK_BACKWARDS;
        uwSkipSize   = uwPosInDecBytes - pstSeekReqMsg->uwSeekPos;
    }

    /* 调用TENSILICA seek API执行seek */
    uwResult = AUDIO_PLAYER_DoSeek(uwSkipSize, enSeekDirect);

    if (VOS_OK == uwResult)
    {
        /* 更新start position */
        pstPlayer->uwStartPos       = pstSeekReqMsg->uwSeekPos;

        /* 重新获取当前已处理数据计数 */
        pstPlayer->uwPlayedBytes    = pstSeekReqMsg->uwSeekPos;

    }
    else
    {
        OM_LogError3(AUDIO_PlayerDecoderSeekFailed, uwResult, uwSkipSize, uwPosInDecBytes);
    }

    if( VOS_TRUE != pstSeekReqMsg->uhwIsNeedNewMem )
    {
        /* 重新计算当前块播放buffer的数据读取计数 */
        uwDataLeft = pstPlayer->uwDecDataLeft - (pstSeekReqMsg->uwSeekPos - uwPosInDecBytes);

        /* 重新计算DecBufAddr */
        uwDecBuff  = pstPlayer->uwDecBufAddr + (pstSeekReqMsg->uwSeekPos - uwPosInDecBytes);

        if((uwDecBuff >= pstPlayer->uwApBufAddr) && (0 < uwDataLeft))
        {
            pstPlayer->uwDecDataLeft    = uwDataLeft;
            pstPlayer->uwDecBufAddr     = uwDecBuff;
        }
        else
        {
            OM_LogWarning3(AUDIO_PlayerSeekDecBufCalcErr, uwDataLeft, (VOS_INT32)uwDecBuff, (VOS_INT32)pstPlayer->uwApBufAddr);

            pstPlayer->uwDecBufAddr     = pstPlayer->uwApBufAddr;
            pstPlayer->uwDecDataLeft    = pstPlayer->uwApDataLeft;
        }
    }

    if( UCOM_RET_SUCC != uwResult ) /* seek failed */
    {
        enSeekResult = AUDIO_PLAY_RESULT_FAIL;

        OM_LogInfo(AUDIO_PlayerSeekFail);
    }
    else /* seek sucessfully */
    {
        enSeekResult = AUDIO_PLAY_RESULT_OK;

        /* 标志数据读取从主buffer开始 */
        pstPlayer->uhwIsDataBak     = VOS_FALSE;

        /* 重新计算当前播放bytes */
        pstPlayer->uwPlayedBytes = AUDIO_PLAYER_GetCurrDecPos(pstPlayer->enFormat);

        OM_LogInfo(AUDIO_PlayerSeekSucc);
    }

    /* 回复AP seek执行结果 */
    AUDIO_PLAYER_SendPlaySeekCnf(enSeekResult,
                                 pvMsgTail,
                                 sizeof(AUDIO_HIFIMISC_MSG_TAIL_STRU));

    /* 如果seek失败，停止音频播放 */
    if(AUDIO_PLAY_RESULT_FAIL == enSeekResult)
    {
        stDecodeInd.uhwMsgId = ID_AUDIO_PLAYER_STOP_DECODE_IND;

        /* 调用通用VOS发送接口，发送消息 */
        UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                        DSP_PID_AUDIO,
                       &stDecodeInd,
                        sizeof(AUDIO_PLAYER_DECODE_IND_STRU));

        /* 申请停止PCM工作通道 */
        AUDIO_PLAYER_PcmStop();
    }
    else
    {
        if( VOS_FALSE == pstSeekReqMsg->uhwIsNeedNewMem )
        {
            /* 设置player状态runing */
            AUDIO_PlayerSetState(AUDIO_PLAYER_STATE_RUNNING);
        }
    }

    return uwResult;
}
VOS_UINT32 AUDIO_PLAYER_MsgStopReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_PLAYER_DECODE_IND_STRU        stDecodeInd;
    VOS_VOID                           *pvMsgTail      = VOS_NULL;

    /* 该消息需要将原消息的CmdId及Sn号回复给Hifi_misc驱动 */
    pvMsgTail       = (VOS_VOID *)((VOS_UINT32)(((MsgBlock *) pvOsaMsg)->aucValue) + sizeof(AUDIO_PLAYER_PAUSE_REQ_STRU));

    if(AUDIO_PlayerGetState() != AUDIO_PLAYER_STATE_IDLE)
    {
        UCOM_MemSet(&stDecodeInd, 0, sizeof(AUDIO_PLAYER_DECODE_IND_STRU));

        /* 处理AP侧停止超低功耗播放指令，该消息在用户暂停或停止播放时均会发送 */
        stDecodeInd.uhwMsgId = ID_AUDIO_PLAYER_STOP_DECODE_IND;

        /* 调用通用VOS发送接口，发送消息 */
        UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                    DSP_PID_AUDIO,
                   &stDecodeInd,
                    sizeof(AUDIO_PLAYER_DECODE_IND_STRU));

        /* 申请停止PCM工作通道 */
        AUDIO_PLAYER_PcmStop();
    }

    /* 回复AP停止播放执行结果 */
    AUDIO_PLAYER_SendPlayStopCnf(pvMsgTail, sizeof(AUDIO_HIFIMISC_MSG_TAIL_STRU));

    return UCOM_RET_SUCC;
}
VOS_UINT32 AUDIO_PLAYER_MsgPlayDoneSignalReq(VOS_VOID *pvOsaMsg)
{
    VOS_UINT32                          uwRet = VOS_OK;

    /* 播放结束 */
    uwRet = AUDIO_PLAYER_SendPlayDoneInd(AUDIO_PLAY_DONE_RESET);

    return uwRet;
}
VOS_UINT32 AUDIO_PLAYER_MsgUpdateApBuffCmd(VOS_VOID *pvOsaMsg)
{
    AUDIO_PLAYER_UPDATE_BUF_CMD_STRU       *pstUpdateCmd  = VOS_NULL;
    AUDIO_PLAYER_OBJ_STRU                  *pstPlayer     = VOS_NULL;

    /* 按AUDIO_PLAYER_UPDATE_BUF_CMD_STRU格式解析消息 */
    pstUpdateCmd            = (AUDIO_PLAYER_UPDATE_BUF_CMD_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    pstPlayer               = AUDIO_PlayerGetPlayPtr();

    /* 若AP侧Buff还未播放完毕时更新了Buff信息，报错但继续播放 */
    if ((0 != pstPlayer->uwApDataLeft) &&
        (AUDIO_PLAYER_STATE_RUNNING == AUDIO_PlayerGetState()))
    {
        OM_LogError(AUDIO_PlayerUpdateApBuffCmd_StateError);
    }

    pstPlayer->uwApBufAddr     = pstUpdateCmd->uwBufAddr;
    pstPlayer->uwApBufSize     = pstUpdateCmd->uwBufSize;
    pstPlayer->uwApDataLeft    = pstUpdateCmd->uwDataSize;

    /* 如果为seek时暂停状态，重新置为runing */
    if (AUDIO_PLAYER_STATE_PAUSE == AUDIO_PlayerGetState())
    {
        pstPlayer->uwDecBufAddr = pstPlayer->uwApBufAddr;
        pstPlayer->uwDecDataLeft= pstPlayer->uwApDataLeft;
        AUDIO_PlayerSetState(AUDIO_PLAYER_STATE_RUNNING);
    }
    else if(AUDIO_PLAYER_STATE_RUNNING != AUDIO_PlayerGetState())
    {
        AUDIO_PlayerSetState(AUDIO_PLAYER_STATE_BUTT);
    }
    else
    {
        /* pclint */
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 AUDIO_PLAYER_MsgUpdatePcmBuffCmd(VOS_VOID *pvOsaMsg)
{
    VOS_UINT32                          uwBufPoolDepth, uwSrcDepth;
    VOS_UINT32                          uwProcessTime,uwSrcConsum,uwChunkSize;
    VOS_CHAR                           *pSrcDolbyBuff   = VOS_NULL;
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer       = VOS_NULL;
    VOS_UINT32                          uwRet           = VOS_OK;

    /* 【1】 此处表明前20ms数据已输出，需更新src缓冲池 */
    uwRet += AUDIO_PLAYER_ShiftBuff(AUDIO_PLAYER_SRC_BUFF, AUDIO_PLAYER_PCM_20MS_BUFF_SIZE);

    /* 【2】进行变采样处理 */
    uwSrcDepth = AUDIO_PlayerGetSrcBufPoolDepth();
    pstPlayer  = AUDIO_PlayerGetPlayPtr();

    /* 若剩余src缓冲池数据不足一次20ms搬运，则继续处理 */
    if (uwSrcDepth < AUDIO_PLAYER_PCM_20MS_BUFF_SIZE)
    {
        /* 获取一帧20ms所有chunksize */
        uwChunkSize     = (pstPlayer->uw20msDataSize)/sizeof(VOS_INT16);

        /* 获取需要处理的次数 */
        uwProcessTime   = (uwChunkSize/(XA_SRC_INPUT_CHUNKSIZE_128 * AUDIO_PLAYER_PLAY_CHN_NUM)) + 1;

        /* 计算src需要消耗解码buff大小 */
        uwSrcConsum     = (uwProcessTime * (XA_SRC_INPUT_CHUNKSIZE_128 * AUDIO_PLAYER_PLAY_CHN_NUM)) * sizeof(VOS_INT16);

        /* 获取最新的解码池深度 */
        uwBufPoolDepth  = AUDIO_PlayerGetPlayBufPoolDepth();

        /* 【3】如果解码池剩余数据不足uwSrcConsum，则解码两帧 */
        if (uwBufPoolDepth < uwSrcConsum)
        {
            uwRet += AUDIO_PLAYER_DoPlay();
        }

        /* 获取SRC缓冲池、缓冲池深度 */
        uwSrcDepth      = AUDIO_PlayerGetSrcBufPoolDepth();
        pSrcDolbyBuff   = AUDIO_PlayerGetSrcBuffPtr();

        uwRet += AUDIO_PLAYER_DoSrc((VOS_INT16 *)AUDIO_PlayerGetPlayBufPoolPtr(),
                                    (VOS_INT16 *)(pSrcDolbyBuff + uwSrcDepth),
                                    uwProcessTime);

        /* 【4】由于变采样消耗了解码之后数据，故将消耗的解码数据移除 */
        uwRet += AUDIO_PLAYER_ShiftBuff(AUDIO_PLAYER_DEC_BUFF, uwSrcConsum);

    }

    /* 【5】 设置音量 */
    CODEC_OpVcMultR((VOS_INT16*)AUDIO_PlayerGetSrcBuffPtr(),
                    ((AUDIO_PLAYER_PCM_20MS_BUFF_SIZE)/2),
                    (VOS_INT16)AUDIO_PLAYER_GetCurrVol(),
                    (VOS_INT16*)AUDIO_PlayerGetSrcBuffPtr());

    /* 【6】 在发送给PCM模块之前，进行DTS处理 */
    uwRet += (VOS_UINT32)AUDIO_PLAYER_DoDts((VOS_INT16 *)AUDIO_PlayerGetSrcBuffPtr(),
                                            (VOS_INT16 *)AUDIO_PlayerGetDtsBuffPtr());

    /* 不管缓冲池中是否有足够已解码数据，都需要给PCM发送数据更新消息 */
    uwRet += AUDIO_PLAYER_SendPcmSetBufCmd((VOS_UINT32)AUDIO_PlayerGetDtsBuffPtr(), AUDIO_PLAYER_PCM_20MS_BUFF_SIZE);

    /* 如果执行失败，则上报error */
    if (VOS_OK != uwRet)
    {
        OM_LogError1(AUDIO_PlayerProcError, uwRet);
    }

    return uwRet;
}


VOS_UINT32 AUDIO_PLAYER_MsgSetVolReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_PLAYER_SET_VOL_REQ_STRU      *pstSetVolReqMsg     = VOS_NULL;
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer           = VOS_NULL;

    /* 按AUDIO_PLAYER_SET_VOL_REQ_STRU格式解析消息 */
    pstSetVolReqMsg     = (AUDIO_PLAYER_SET_VOL_REQ_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;
    pstPlayer           = AUDIO_PlayerGetPlayPtr();

    pstPlayer->stVolume.uwOriginal  = pstSetVolReqMsg->uwValue;
    pstPlayer->stVolume.uwCurr      = pstSetVolReqMsg->uwValue;
    pstPlayer->stVolume.uwTarget    = pstSetVolReqMsg->uwValue;

    return VOS_OK;
}
VOS_UINT32 AUDIO_PLAYER_HandleFile(CODEC_STATE_ENUM_UINT16 *penDecoderState)
{
    VOS_CHAR                           *pscPlayBufPool      = VOS_NULL;
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer           = VOS_NULL ;
    VOS_UINT32                          uwPlayBufPoolDepth;
    VOS_UINT32                          uwDecoderState;
    VOS_INT32                           swOutputSize        = 0;
    VOS_INT32                          *pswDecConsumed      = VOS_NULL;
    VOS_UINT32                          enIsEndOfStream;
    XA_AUDIO_DEC_USER_CFG_STRU          stDecUserCfg;                           /* MP3、AAC解码配置信息 */
    VOS_UINT32                          uwErrorCode;

    uwDecoderState     = CODEC_STATE_INITIALIZED;

    /* 获取解码后码流存放buffer pool */
    pscPlayBufPool      = AUDIO_PlayerGetPlayBufPoolPtr();

    /* 获取播放模块控制体 */
    pstPlayer           = AUDIO_PlayerGetPlayPtr();

    /* 获取当前播放期间的Input Buff数据消耗情况 */
    pswDecConsumed      = (VOS_INT32*)&pstPlayer->uwDecConsumed;

    /* 获取解码后码流存放buffer pool size */
    uwPlayBufPoolDepth  = AUDIO_PlayerGetPlayBufPoolDepth();

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_PLAYER_DECODE);

    /* 如果buffer pool中的数据量小于两帧20ms数据，那么需要解码直至数据足够 */
    while((uwPlayBufPoolDepth < (2 * pstPlayer->uw20msDataSize) )
       && (CODEC_STATE_INITIALIZED == uwDecoderState))
    {
        /* 如果当前状态为pause，填充buffer pool为全0 */
        if (AUDIO_PLAYER_STATE_PAUSE== pstPlayer->enState)
        {
            UCOM_MemSet(pscPlayBufPool, 0, pstPlayer->uw20msDataSize);

            /* 更新buffer pool depth */
            uwPlayBufPoolDepth = pstPlayer->uw20msDataSize;
            AUDIO_PlayerSetPlayBufPoolDepth(uwPlayBufPoolDepth);
            break;
        }

        /* 当前播放的字节大小 */
        pstPlayer->uwPlayedBytes = AUDIO_PLAYER_GetCurrDecPos(pstPlayer->enFormat);

        /* 根据当前buffe状态填充Input数据切换 */
        AUDIO_PLAYER_FillBuff(pstPlayer);

        /* MP3、AAC解码用户配置信息清空 */
        UCOM_MemSet(&stDecUserCfg, 0, sizeof(XA_AUDIO_DEC_USER_CFG_STRU));

        if (pstPlayer->uwPlayedBytes >= pstPlayer->uwTotalFileSize)
        {
            enIsEndOfStream = AUDIO_PLAYER_POS_END_OF_STEAM;
        }
        else
        {
            enIsEndOfStream = AUDIO_PLAYER_POS_NORMAL;
        }

        /* 更新AUDIO_PlayerGetOutputBufPtr() */
        AUDIO_PlayerGetOutputBufPtr() = pscPlayBufPool + uwPlayBufPoolDepth;

        /* 如果解码MP3 */
        if (AUDIO_FILE_FORMAT_MP3 == pstPlayer->enFormat)
        {
            /* 配置MP3信息 */
            stDecUserCfg.enIsEndOfStream   = enIsEndOfStream;
            stDecUserCfg.uwPcmWidthSize    = AUDIO_PLAYER_PCM_WIDTH_SIZE;

            /* 配置输入BUFF的首地址 */
            stDecUserCfg.pvInBuff          = AUDIO_PlayerGetInputBufPtr();

            /* 配置输入BUFF有效大小 */
            stDecUserCfg.swInSize          = (VOS_INT32)pstPlayer->uwDecInMemSize;

            /* 配置MP3输入BUFF */
            stDecUserCfg.pvOutBuff         = AUDIO_PlayerGetOutputBufPtr();

            /* 启动解码操作，在解码操作中，应该配置完成下面项:
            stMP3UserCfg.swConsumed;     // 此次解码消耗的BUFF大小
            stMP3UserCfg.swOutputSize;   // 解码后输出数据的总大小
            stMP3UserCfg.uwDecoderState; // 解码状态 */
            uwErrorCode = MP3_DECODER_Decode(&stDecUserCfg);

            XA_ERROR_HANDLE(uwErrorCode);

            /* 保存已消耗BUFF数量 */
            *pswDecConsumed                = stDecUserCfg.swConsumed;

            /* 保存解码后输出数据总大小 */
            swOutputSize                   = stDecUserCfg.swOutputSize;

            /* 保存解码状态 */
            uwDecoderState                 = stDecUserCfg.uwDecoderState;

        }

        /* 如果解码AAC */
        else
        {
            /* 配置AAC信息 */
            stDecUserCfg.enIsEndOfStream   = enIsEndOfStream;
            stDecUserCfg.uwPcmWidthSize    = AUDIO_PLAYER_PCM_WIDTH_SIZE;

            /* 配置输入BUFF的首地址 */
            stDecUserCfg.pvInBuff          = AUDIO_PlayerGetInputBufPtr();

            /* 配置输入BUFF有效大小 */
            stDecUserCfg.swInSize          = (VOS_INT32)pstPlayer->uwDecInMemSize;

            /* 配置AAC输入BUFF */
            stDecUserCfg.pvOutBuff         = AUDIO_PlayerGetOutputBufPtr();

            /* 启动解码操作，在解码操作中，应该配置完成下面项:
            stAacUserCfg.swConsumed;     // 此次解码消耗的BUFF大小
            stAacUserCfg.swOutputSize;   // 解码后输出数据的总大小
            stAacUserCfg.uwDecoderState; // 解码状态 */
            uwErrorCode = AAC_DECODER_Decode(&stDecUserCfg);
            XA_ERROR_HANDLE(uwErrorCode);

            /* 保存已消耗BUFF数量 */
            *pswDecConsumed                = stDecUserCfg.swConsumed;

            /* 保存解码后输出数据总大小 */
            swOutputSize                   = stDecUserCfg.swOutputSize;

            /* 保存解码状态 */
            uwDecoderState                 = stDecUserCfg.uwDecoderState;

        }

        /* 更新buffer pool depth */
        uwPlayBufPoolDepth += (VOS_UINT32)swOutputSize;

        AUDIO_PlayerSetPlayBufPoolDepth(uwPlayBufPoolDepth);

    }
    *penDecoderState = (CODEC_STATE_ENUM_UINT16)uwDecoderState;

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_PLAYER_DECODE);

    return VOS_OK;
}


VOS_UINT32 AUDIO_PLAYER_PcmStart(VOS_VOID)
{
    AUDIO_PCM_OPEN_REQ_STRU             stPcmOpenMsg;
    AUDIO_PCM_HW_PARAMS_REQ_STRU        stPcmHwParaMsg;
    AUDIO_PCM_TRIGGER_REQ_STRU          stPcmTriggerMsg;
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer   = VOS_NULL;

    UCOM_MemSet(&stPcmOpenMsg, 0, sizeof(AUDIO_PCM_OPEN_REQ_STRU));
    UCOM_MemSet(&stPcmHwParaMsg, 0, sizeof(AUDIO_PCM_HW_PARAMS_REQ_STRU));
    UCOM_MemSet(&stPcmTriggerMsg, 0, sizeof(AUDIO_PCM_TRIGGER_REQ_STRU));

    /* 填充消息ID_AP_AUDIO_PCM_OPEN_REQ */
    stPcmOpenMsg.uhwMsgId  = ID_AP_AUDIO_PCM_OPEN_REQ;
    stPcmOpenMsg.enPcmMode = AUDIO_PCM_MODE_PLAYBACK;

    /* 模拟AP发送消息ID_AP_AUDIO_PCM_OPEN_REQ给PCM */
    UCOM_SendAudioMsg(DSP_PID_AUDIO,
                      DSP_PID_AUDIO_RT,
                      &stPcmOpenMsg,
                      sizeof(AUDIO_PCM_OPEN_REQ_STRU));

    /* 模拟AP发送消息ID_AP_AUDIO_PCM_HW_PARA_REQ给PCM */
    stPcmHwParaMsg.uhwMsgId     = ID_AP_AUDIO_PCM_HW_PARA_REQ;
    stPcmHwParaMsg.enPcmMode    = AUDIO_PCM_MODE_PLAYBACK;
    stPcmHwParaMsg.uwChannelNum = AUDIO_PCM_MAX_CHANNEL_NUM;
    stPcmHwParaMsg.uwSampleRate = DRV_SIO_SAMPLING_48K;

    /* 模拟AP发送消息ID_AP_AUDIO_PCM_HW_PARA_REQ给PCM */
    UCOM_SendAudioMsg(DSP_PID_AUDIO,
                      DSP_PID_AUDIO_RT,
                      &stPcmHwParaMsg,
                      sizeof(AUDIO_PCM_HW_PARAMS_REQ_STRU));

    pstPlayer   = AUDIO_PlayerGetPlayPtr();

    /* 模拟AP发送消息ID_AP_AUDIO_PCM_TRIGGER_REQ给PCM */
    stPcmTriggerMsg.uhwMsgId     = ID_AP_AUDIO_PCM_TRIGGER_REQ;
    stPcmTriggerMsg.enPcmMode    = AUDIO_PCM_MODE_PLAYBACK;
    stPcmTriggerMsg.enCmd        = AUDIO_PCM_TRIGGER_START;
    stPcmTriggerMsg.enPcmObj     = AUDIO_PCM_OBJ_HIFI;
    stPcmTriggerMsg.uwBufAddr    = (VOS_UINT32)AUDIO_PlayerGetPlayBufPoolPtr();
    stPcmTriggerMsg.uwBufSize    = pstPlayer->uw20msDataSize;

    /* 模拟AP发送消息ID_AP_AUDIO_PCM_TRIGGER_REQ给PCM */
    UCOM_SendAudioMsg(DSP_PID_AUDIO,
                      DSP_PID_AUDIO_RT,
                      &stPcmTriggerMsg,
                      sizeof(AUDIO_PCM_TRIGGER_REQ_STRU));

    return UCOM_RET_SUCC;
}


VOS_VOID AUDIO_PLAYER_PcmStop(VOS_VOID)
{
    AUDIO_PCM_TRIGGER_REQ_STRU          stPcmTriggerMsg;
    AUDIO_PCM_CLOSE_REQ_STRU            stPcmCloseMsg;

    UCOM_MemSet(&stPcmTriggerMsg, 0, sizeof(AUDIO_PCM_TRIGGER_REQ_STRU));
    UCOM_MemSet(&stPcmCloseMsg, 0, sizeof(AUDIO_PCM_CLOSE_REQ_STRU));

    /* 模拟AP发送消息ID_AP_AUDIO_PCM_TRIGGER_REQ给PCM */
    stPcmTriggerMsg.uhwMsgId     = ID_AP_AUDIO_PCM_TRIGGER_REQ;
    stPcmTriggerMsg.enPcmMode    = AUDIO_PCM_MODE_PLAYBACK;
    stPcmTriggerMsg.enCmd        = AUDIO_PCM_TRIGGER_STOP;
    stPcmTriggerMsg.enPcmObj     = AUDIO_PCM_OBJ_HIFI;

    /* 调用音频专用通道发送跨核消息 */
    UCOM_SendAudioMsg(DSP_PID_AUDIO,
                    DSP_PID_AUDIO_RT,
                   &stPcmTriggerMsg,
                    sizeof(AUDIO_PCM_TRIGGER_REQ_STRU));

    /* 模拟AP发送消息ID_AP_AUDIO_PCM_CLOSE_REQ给PCM */
    stPcmCloseMsg.uhwMsgId     = ID_AP_AUDIO_PCM_CLOSE_REQ;
    stPcmCloseMsg.enPcmMode    = AUDIO_PCM_MODE_PLAYBACK;

    /* 调用音频专用通道发送跨核消息 */
    UCOM_SendAudioMsg(DSP_PID_AUDIO,
                    DSP_PID_AUDIO_RT,
                    &stPcmCloseMsg,
                    sizeof(AUDIO_PCM_CLOSE_REQ_STRU));


    return;
}


VOS_VOID AUDIO_PLAYER_SendApAudioCnf(VOS_UINT16    uhwMsgId,
                                                  VOS_UINT32    uwRet,
                                                  VOS_VOID     *pvTail,
                                                  VOS_UINT32    uwTailLen)
{
    APAUDIO_OP_RSLT_STRU    stCnfMsg;

    /* 清空消息 */
    UCOM_MemSet(&stCnfMsg, 0, sizeof(APAUDIO_OP_RSLT_STRU));

    /* 配置消息和返回值 */
    stCnfMsg.uhwMsgId         = uhwMsgId;
    stCnfMsg.uhwResult        = (VOS_UINT16)uwRet;

    /*  回复消息给AP */
    UCOM_COMM_SendDspMsg(DSP_PID_AUDIO,
                         ACPU_PID_OM,
                        &stCnfMsg,
                         sizeof(APAUDIO_OP_RSLT_STRU),
                         pvTail,
                         uwTailLen);
}
VOS_UINT32 AUDIO_PLAYER_SendPcmSetBufCmd(VOS_UINT32 uwBufPoolAddr,
                                                        VOS_UINT32 uwBufSize)
{
    AUDIO_PCM_SET_BUF_CMD_STRU          stSetBufMsg;

    UCOM_MemSet(&stSetBufMsg, 0, sizeof(AUDIO_PCM_SET_BUF_CMD_STRU));

    /* 模拟AP发送消息ID_AP_AUDIO_PCM_SET_BUF_CMD给PCM */
    stSetBufMsg.uhwMsgId    = ID_AP_AUDIO_PCM_SET_BUF_CMD;
    stSetBufMsg.enPcmMode   = AUDIO_PCM_MODE_PLAYBACK;
    stSetBufMsg.enPcmObj    = AUDIO_PCM_OBJ_HIFI;
    stSetBufMsg.uwBufAddr   = uwBufPoolAddr;
    stSetBufMsg.uwBufSize   = uwBufSize;

    /* 调用通用VOS发送接口，发送消息 */
    UCOM_SendAudioMsg(DSP_PID_AUDIO,
                      DSP_PID_AUDIO_RT,
                     &stSetBufMsg,
                      sizeof(AUDIO_PCM_SET_BUF_CMD_STRU));

    /* 此处表明一帧20ms语音数据已处理完成，hifi无需访问ddr，hifi向MCU投票，要求DDR进入自刷新模式 */
    UCOM_LOW_PowerDRF();

    return UCOM_RET_SUCC;
}
VOS_UINT32 AUDIO_PLAYER_SendPlayDoneInd(AUDIO_PLAY_DONE_IND_ENUM_UINT16   enPlayDone)
{
    AUDIO_PLAYER_DONE_IND_STRU         stPlayDoneIndMsg;
    AUDIO_HIFIMISC_MSG_TAIL_STRU       stMsgTail;

    /* 按AUDIO_PLAYER_DONE_IND_STRU格式填充消息 */
    stPlayDoneIndMsg.uhwMsgId      = ID_AUDIO_AP_PLAY_DONE_IND;
    stPlayDoneIndMsg.enPlayDone    = enPlayDone;
    stMsgTail.uwCmdId              = AUDIO_MISC_CMD_NOTIFY;
    stMsgTail.uwSn                 = AUDIO_HIFIMISC_MSG_SN_DEFAULT;

    /* 调用跨核音频channel接口,通知AP播放完毕 */
    UCOM_COMM_SendDspMsg(DSP_PID_AUDIO,
                         ACPU_PID_OM,           /* 此处填写AP侧DRV对应的PID,,待定 */
                        &stPlayDoneIndMsg,
                         sizeof(AUDIO_PLAYER_DONE_IND_STRU),
                         &stMsgTail,
                         sizeof(AUDIO_HIFIMISC_MSG_TAIL_STRU));

    return UCOM_RET_SUCC;
}
VOS_VOID AUDIO_PLAYER_SendPlayStopCnf(VOS_VOID *pvTail, VOS_UINT32 uwTailLen)
{
    AUDIO_PLAYER_PAUSE_CNF_STRU         stPlayPauseCnfMsg;
    AUDIO_PLAYER_OBJ_STRU              *pstPlayer    = VOS_NULL;

    pstPlayer                           = AUDIO_PlayerGetPlayPtr();

    /* 按stPlayPauseCnfMsg格式填充消息 */
    stPlayPauseCnfMsg.uhwMsgId          = ID_AUDIO_AP_PLAY_PAUSE_CNF;
    stPlayPauseCnfMsg.uhwResult         = UCOM_RET_SUCC;
    stPlayPauseCnfMsg.uwPlayedBytes     = pstPlayer->uwPlayedBytes;

    /* 调用跨核音频channel接口,通知AP播放进度 */
    UCOM_COMM_SendDspMsg(DSP_PID_AUDIO,
                         ACPU_PID_OM,           /* 此处填写AP侧DRV对应的PID,,待定 */
                         &stPlayPauseCnfMsg,
                         sizeof(AUDIO_PLAYER_PAUSE_CNF_STRU),
                         pvTail,
                         uwTailLen);

    return;

}
VOS_VOID AUDIO_PLAYER_SendPlaySeekCnf(AUDIO_PLAY_RESULT_CNF_ENUM_UINT16 enSeekResult,
                                                  VOS_VOID               *pvTail,
                                                  VOS_UINT32              uwTailLen)
{
    AUDIO_PLAYER_SEEK_CNF_STRU          stSeekCnfMsg;

    /* 按AUDIO_PLAYER_SEEK_CNF_STRU格式填充消息 */
    stSeekCnfMsg.uhwMsgId           = ID_AUDIO_AP_PLAY_SEEK_CNF;
    stSeekCnfMsg.enSeekResult       = enSeekResult;

    /* 调用跨核音频channel接口,通知AP seek result */
    UCOM_COMM_SendDspMsg(DSP_PID_AUDIO,
                         ACPU_PID_OM,           /* 此处填写AP侧DRV对应的PID */
                        &stSeekCnfMsg,
                         sizeof(AUDIO_PLAYER_SEEK_CNF_STRU),
                         pvTail,
                         uwTailLen);
    return;

}
VOS_UINT32 AUDIO_PLAYER_DoSrc(
                                VOS_INT16  *pshwBufIn,
                                VOS_INT16  *pshwBufOut,
                                VOS_UINT32  uwProcTime)
{
    VOS_UINT32                          uwCnt;
    VOS_UINT32                          uwRet;
    VOS_UINT32                          uwSrcDolbyPoolDepth = AUDIO_PlayerGetSrcBufPoolDepth();
    VOS_VOID                           *pInputBuff;
    VOS_VOID                           *pOutputBuff;
    VOS_UINT32                          uwOutputSize;

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_PLAYER_SRC);

    /* 设置输出buff */
    pOutputBuff = pshwBufOut;

    /* 由于SRC一次处理128个采样点，故采用分段处理，每次处理AUDIO_PLAYER_CHUNK_SIZE个采样点 */
    for (uwCnt = 0; uwCnt < uwProcTime; uwCnt++)
    {
        /* 设置输入buff */
        pInputBuff = (VOS_VOID *)(pshwBufIn + (uwCnt * (XA_SRC_INPUT_CHUNKSIZE_128 * AUDIO_PLAYER_PLAY_CHN_NUM)));

        /* 执行变采样 */
        uwRet = XA_SRC_ExeProcess(XA_SRC_PlAYBACK,
                                  pInputBuff,
                                  pOutputBuff,
                                  &uwOutputSize);

        /* 执行异常，返回 */
        if(uwRet != VOS_OK)
        {
            OM_LogError1(XA_SRC_ExeProcess_Fail, uwRet);

            return VOS_ERR;
        }

        /* 更新输出数据指针 */
        pOutputBuff = (VOS_INT16*)pOutputBuff + \
                      (uwOutputSize/(VOS_INT32)sizeof(VOS_INT16));

        /* 更新SRC、DOLBY缓冲池深度 */
        uwSrcDolbyPoolDepth     = uwSrcDolbyPoolDepth + uwOutputSize;
        AUDIO_PlayerSetSrcBufPoolDepth(uwSrcDolbyPoolDepth);

    }

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_PLAYER_SRC);

    return VOS_OK;

}
VOS_INT32  AUDIO_PLAYER_DoDts(
                VOS_INT16 *pshwInput,
                VOS_INT16 *pshwOutput)
{
    XA_DTS_USER_CFG_STRU            stDtsCfg;
    VOS_UINT32                      uwCnt;
    VOS_INT32                       swRet = VOS_OK;

    /* 设置DTS参数 */
    stDtsCfg.swChannelNum       = AUDIO_PLAYER_PLAY_CHN_NUM;
    stDtsCfg.uwBlockSize        = AUDIO_PLAYER_DTS_CHUNK_SIZE;
    stDtsCfg.uwFilterCfg        = 0;
    stDtsCfg.uwSampleRate       = AUDIO_PLAYER_OUTPUT_SAMPLE_RATE;

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_PLAYER_DTS);

    /* 即将给DMA搬运的数据做DTS处理 */
    for ( uwCnt = 0; uwCnt < AUDIO_PLAYER_DTS_PROC_NUM; uwCnt++ )
    {
        /* 配置输入数据 */
        stDtsCfg.pshwInputBuff  = pshwInput + ((stDtsCfg.uwBlockSize * (VOS_UINT32)stDtsCfg.swChannelNum) * uwCnt);

        /* 配置输出数据 */
        stDtsCfg.pshwOutputBuff = pshwOutput + ((stDtsCfg.uwBlockSize * (VOS_UINT32)stDtsCfg.swChannelNum) * uwCnt);

        /* 执行DTS音效处理 */
        swRet += XA_DTS_ExeProcess(&stDtsCfg);
    }

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_PLAYER_DTS);

    return swRet;

}
VOS_UINT32  AUDIO_PLAYER_ShiftBuff(
                AUDIO_PLAYER_BUFF_ENUM_UINT16 enBuffType,
                VOS_UINT32                    uwShiftSize)
{
    VOS_UINT32                  uwDecBuffSize;
    VOS_UINT32                  uwSrcBuffSize;

    /* 移动解码之后的缓冲buff */
    if (AUDIO_PLAYER_DEC_BUFF == enBuffType)
    {
        /* 获取最新的解码池深度 */
        uwDecBuffSize  = AUDIO_PlayerGetPlayBufPoolDepth();

        if (uwDecBuffSize >= uwShiftSize)
        {
            /* 更新dec缓冲池 */
            XA_COMM_ShiftBuff(AUDIO_PlayerGetPlayBufPoolPtr(),
                              (VOS_INT32)uwDecBuffSize,
                              (VOS_INT32)uwShiftSize);

            uwDecBuffSize = uwDecBuffSize - uwShiftSize;
        }
        else
        {
            uwDecBuffSize = 0;

            OM_LogWarning2(AUDIO_PlayerDecBuffSizeWarning, uwDecBuffSize, uwShiftSize);
        }

        /* 更新dec缓冲池深度 */
        AUDIO_PlayerSetPlayBufPoolDepth(uwDecBuffSize);

    }
    /* 移动变采样之后的缓冲buff */
    else if(AUDIO_PLAYER_SRC_BUFF == enBuffType)
    {
        /* 获取最新的SRC缓冲池的深度 */
        uwSrcBuffSize = AUDIO_PlayerGetSrcBufPoolDepth();

        /* 更新src缓冲池 */
        if (uwSrcBuffSize >= uwShiftSize)
        {
             XA_COMM_ShiftBuff(AUDIO_PlayerGetSrcBuffPtr(),             \
                      (VOS_INT32)(uwSrcBuffSize),       \
                      (VOS_INT32)uwShiftSize);

             uwSrcBuffSize = uwSrcBuffSize - uwShiftSize;
        }
        else
        {
            uwSrcBuffSize = 0;

            OM_LogWarning2(AUDIO_PlayerSrcBuffSizeWarning, uwSrcBuffSize, uwShiftSize);
        }

        /* 更新src缓冲池深度 */
        AUDIO_PlayerSetSrcBufPoolDepth(uwSrcBuffSize);

    }
    else
    {
        /* 目前线性buff移动只涉及dec、src */
        OM_LogError1(AUDIO_PlayerShiftBuffError, enBuffType);

        return VOS_ERR;
    }

    return VOS_OK;

}
VOS_VOID AUDIO_PLAYER_AudioCallBack(
                VOS_VOID  *pshwBufOut,
                VOS_VOID  *pshwBufIn,
                VOS_UINT32 uwSampleRate,
                VOS_UINT32 uwChannelNum,
                VOS_UINT32 uwBufSize)
{
    AUDIO_ENHANCE_ProcSpkOut(
                pshwBufOut,
                pshwBufIn,
                uwSampleRate,
                uwChannelNum,
                uwBufSize);
}


VOS_UINT32  AUDIO_PLAYER_MsgSetDtsEnableCmd(VOS_VOID *pvOsaMsg)
{
    AUDIO_PLAYER_OBJ_STRU                   *pstPlayer          = AUDIO_PlayerGetPlayPtr();
    AUDIO_EFFECT_DTS_SET_ENABLE_REQ_STRU    *pstSetParaCmd;
    AUDIO_PLAYER_DTS_SET_ENABLE_CMD_STRU     stSetDtsEnable;

    /* 异常入参检查 */
    if ( VOS_NULL == pvOsaMsg )
    {
        OM_LogError(AUDIO_PlayerSetDtsEnableFail);
        return VOS_ERR;
    }

    /* 按照DTS音效参数结构体解析 */
    pstSetParaCmd   = (AUDIO_EFFECT_DTS_SET_ENABLE_REQ_STRU *)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 设置标志位 */
    pstPlayer->enDtsEnable               = pstSetParaCmd->uhwEnableFlag;

    /* 如果在运行状态，则调用音量渐变 */
    if (AUDIO_PLAYER_STATE_RUNNING == AUDIO_PlayerGetState())
    {
        pstPlayer->stVolume.enSetVolEnum        = AUDIO_PLAYER_DTS_ENABLE_VOL;

        pstPlayer->stVolume.uwDtsEnableFrame    = AUDIO_PLAYER_MORPHING_VOL_FRAME_0;

        /* 为避免连续多次使能DTS而导致音量异常，每次调用前先重新设置当前音量 */
        pstPlayer->stVolume.uwCurr              = pstPlayer->stVolume.uwTarget;
    }
    /* 如果不在运行状态，则直接发消息给DTS */
    else
    {
        UCOM_MemSet(&stSetDtsEnable, 0, sizeof(AUDIO_PLAYER_DTS_SET_ENABLE_CMD_STRU));

        stSetDtsEnable.uhwMsgId         = ID_AUDIO_PLAYER_SET_DTS_ENABLE_IND;
        stSetDtsEnable.uhwEnableFlag    = pstPlayer->enDtsEnable;

        /* 调用通用VOS发送接口，发送消息 */
        UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                        DSP_PID_AUDIO_RT,
                       &stSetDtsEnable,
                        sizeof(AUDIO_PLAYER_DTS_SET_ENABLE_CMD_STRU));
    }

    return VOS_OK;

}
VOS_UINT32  AUDIO_PLAYER_DtsMorphingVol(VOS_VOID)
{
    AUDIO_PLAYER_OBJ_STRU                   *pstPlayer          = AUDIO_PlayerGetPlayPtr();
    AUDIO_PLAYER_DTS_SET_ENABLE_CMD_STRU     stSetDtsEnable;
    VOS_UINT32                               uwTmpVol;

    /* 保存当前音量值、设置tmp音量值 */
    uwTmpVol        = 0;

    /* 【1】音量下降过程。0 - 10 帧 */
    if ( pstPlayer->stVolume.uwDtsEnableFrame < AUDIO_PLAYER_MORPHING_VOL_FRAME_10 )
    {
        /* 音量逐级变动, 5帧调整到位 */
        pstPlayer->stVolume.uwCurr -= (pstPlayer->stVolume.uwTarget - uwTmpVol)/AUDIO_PLAYER_MORPHING_VOL_FRAME_10;

        /* 记录已处理帧数 */
        pstPlayer->stVolume.uwDtsEnableFrame++;

        /* 消除由于除法引入的误差 */
        if (10 == pstPlayer->stVolume.uwDtsEnableFrame)
        {
            pstPlayer->stVolume.uwCurr = 0;
        }
    }
    /* 【2】静音过程。第10帧 */
    else if ( pstPlayer->stVolume.uwDtsEnableFrame < AUDIO_PLAYER_MORPHING_VOL_FRAME_11 )
    {
        /* 在第5帧设置静音 */
        pstPlayer->stVolume.uwCurr = 0;

        /* 记录已处理帧数 */
        pstPlayer->stVolume.uwDtsEnableFrame++;

    }
    /* 【3】音量上升过程。11 - 20 帧 */
    else if ( pstPlayer->stVolume.uwDtsEnableFrame < AUDIO_PLAYER_MORPHING_VOL_FRAME_21 )
    {
        /* 音量逐级变动, 5帧调整到位 */
        pstPlayer->stVolume.uwCurr += (pstPlayer->stVolume.uwTarget - uwTmpVol)/AUDIO_PLAYER_MORPHING_VOL_FRAME_10;

        /* 在第11帧向DTS模块转发该消息 */
        if (11 == pstPlayer->stVolume.uwDtsEnableFrame)
        {
            UCOM_MemSet(&stSetDtsEnable, 0, sizeof(AUDIO_PLAYER_DTS_SET_ENABLE_CMD_STRU));

            stSetDtsEnable.uhwMsgId      = ID_AUDIO_PLAYER_SET_DTS_ENABLE_IND;
            stSetDtsEnable.uhwEnableFlag = pstPlayer->enDtsEnable;

            /* 调用通用VOS发送接口，发送消息 */
            UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                            DSP_PID_AUDIO_RT,
                           &stSetDtsEnable,
                            sizeof(AUDIO_PLAYER_DTS_SET_ENABLE_CMD_STRU));
        }

        /* 记录已处理帧数 */
        pstPlayer->stVolume.uwDtsEnableFrame++;

    }
    /* 【4】音量调节完成 */
    else
    {
        /* 恢复已处理帧数 */
        pstPlayer->stVolume.uwDtsEnableFrame = AUDIO_PLAYER_MORPHING_VOL_FRAME_0;

        /* 恢复调节音量类型 */
        pstPlayer->stVolume.enSetVolEnum = AUDIO_PLAYER_NORMAL_VOL;

        /* 消除由于除法引入的误差，恢复之前关于音量的设置 */
        pstPlayer->stVolume.uwOriginal  = pstPlayer->stVolume.uwTarget;
        pstPlayer->stVolume.uwCurr      = pstPlayer->stVolume.uwTarget;
    }

    return VOS_OK;

}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif




