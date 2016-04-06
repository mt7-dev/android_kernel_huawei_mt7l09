/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : fr_dtx.c
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年8月15日
  最近修改   :
  功能描述   : FR DTX功能模块函数实现
  函数列表   :
              FR_DtxDecSidLarAndXmaxc
              FR_DtxDecSidOtherPrms
              FR_DtxControlRx
              FR_DtxControlTx
              FR_DtxRxControlState
              FR_DtxRxInit
              FR_DtxTxInit
              FR_DtxEncSidPrm
              FR_DtxGenSilence
              FR_DtxInterpolateCnVec
              FR_DtxPseudoNoise
              FR_DtxQxmax
              FR_DtxUpdateSidMem
  修改历史   :
  1.日    期   : 2011年8月15日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "fr_dtx.h"
#include "fr_short_term.h"
#include "fr_lpc.h"
#include "fr_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*DTX帧类型索引查询表*/
const UWord16 g_auhwFrFrmType[FR_FRAME_TYPE_NUM] =
{
    FR_FRAME_TYPE_GOOD_SPEECH,                          /*BFI=0,SID=0*/
    FR_FRAME_TYPE_INVALID_SID,                          /*BFI=0,SID=1*/
    FR_FRAME_TYPE_VALID_SID,                            /*BFI=0,SID=2*/
    FR_FRAME_TYPE_INVALID_SID,                          /*BFI=0,SID=3*/
    FR_FRAME_TYPE_UNUSABLE,                             /*BFI=1,SID=0*/
    FR_FRAME_TYPE_INVALID_SID,                          /*BFI=1,SID=1*/
    FR_FRAME_TYPE_INVALID_SID,                          /*BFI=1,SID=2*/
    FR_FRAME_TYPE_INVALID_SID                           /*BFI=1,SID=3*/
};

/*上一帧语音帧类型查询表*/
const UWord16 g_auhwFrPrevSp[5] =
{
    0,
    FR_RX_FIRST_SID_UPDATE,
    (FR_RX_FIRST_SID_UPDATE | FR_RX_INVALID_SID_FRAME),
    FR_RX_SP_FLAG,
    FR_RX_SP_FLAG
};

/*上一帧为非语音帧类型查询表*/
const UWord16 g_auhwFrPrevNotSp[10] =
{
    0,
    0,
    FR_RX_CONT_SID_UPDATE,
    FR_RX_CONT_SID_UPDATE,
    (FR_RX_CONT_SID_UPDATE | FR_RX_INVALID_SID_FRAME),
    (FR_RX_CONT_SID_UPDATE | FR_RX_INVALID_SID_FRAME),
    (FR_RX_SP_FLAG | FR_RX_FIRST_SP_FLAG),
    (FR_RX_SP_FLAG | FR_RX_FIRST_SP_FLAG | FR_RX_PREV_DTX_MUTING),
    FR_RX_CNI_BFI,
    FR_RX_CNI_BFI
};

/*舒适噪声矢量内插因子表*/
const Word16 g_ashwFrFrateFactor[FR_DTX_CN_INT_PERIOD*2] =
{
    0x0555, 0x7aab,
    0x0aaa, 0x7556,
    0x1000, 0x7000,
    0x1555, 0x6aab,
    0x1aaa, 0x6556,
    0x2000, 0x6000,
    0x2555, 0x5aab,
    0x2aaa, 0x5556,
    0x3000, 0x5000,
    0x3555, 0x4aab,
    0x3aaa, 0x4556,
    0x4000, 0x4000,
    0x4555, 0x3aab,
    0x4aaa, 0x3556,
    0x5000, 0x3000,
    0x5555, 0x2aab,
    0x5aaa, 0x2556,
    0x6000, 0x2000,
    0x6555, 0x1aab,
    0x6aaa, 0x1556,
    0x7000, 0x1000,
    0x7555, 0x0aab,
    0x7aaa, 0x0556,
    0x7fff, 0x0001
};

/* 静音帧参数LARs和RPE Pulses */
const Word16 g_ashwFrFrateSidPara[21] =
{
    42,39,21,10,9,4,3,2,                /* 静音帧参数LARs[8] */
    3,4,3,4,4,3,3,3,3,4,4,3,3           /* 静音帧参数RPE Pulses[13] */
};



/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : FR_DtxTxInit
 功能描述  : DTX上行编码初始化
 输入参数  : FR_ENC_DTX_STATE_STRU * pstEncDtxState  --DTX编码结构体指针
 输出参数  : FR_ENC_DTX_STATE_STRU * pstEncDtxState  --DTX编码结构体指针
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxTxInit(FR_ENC_DTX_STATE_STRU * pstEncDtxState)
{
    if (VOS_NULL == pstEncDtxState)
    {
        return;
    }

    /*初始化hangover时长*/
    pstEncDtxState->shwHangover = FR_DTX_HANGOVER;

    /*初始化距离上一个SID更新帧时长*/
    pstEncDtxState->shwNelapsed = FR_DTX_INIT_VALUE_NELAPSED;    /*FR_DTX_INIT_VALUE_NELAPSED*/

    /*初始化编码端DTX控制码字*/
    pstEncDtxState->shwCtrlWord = (FR_TX_SP_FLAG | FR_TX_VAD_FLAG);

    /*初始化编码端PN产生种子*/
    pstEncDtxState->swPnSeed    = FR_DTX_PN_INITIAL_SEED;
}


/*****************************************************************************
 函 数 名  : FR_DtxRxInit
 功能描述  : DTX下行控制初始化
 输入参数  : FR_DEC_DTX_STATE_STRU * pstDecDtxState  --DTX解码结构体指针
 输出参数  : FR_DEC_DTX_STATE_STRU * pstDecDtxState  --DTX解码结构体指针
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxRxInit(FR_DEC_DTX_STATE_STRU * pstDecDtxState)
{
    if (VOS_NULL == pstDecDtxState)
    {
        return;
    }

    /* suppose infinitely long speech period before start */

    /*初始化hangover时长*/
    pstDecDtxState->shwHangoverPeriod  = FR_DTX_HANGOVER;

    /*初始化距离上一个SID帧时长*/
    pstDecDtxState->shwNelapsed        = FR_DTX_INIT_VALUE_NELAPSED;

    /*初始化解码端DTX控制字*/
    pstDecDtxState->shwCtrlWord        = FR_TX_SP_FLAG;

    /*初始化PN噪声产生种子*/
    pstDecDtxState->swPnSeed           = FR_DTX_PN_INITIAL_SEED;

    /*初始化舒适噪声插入周期状态*/
    pstDecDtxState->shwCniState        = FR_DTX_CN_INT_PERIOD - 1;

    /*初始化丢失的SID帧数累计值*/
    pstDecDtxState->shwSidLostFrameNum = 0;

    /*初始化上一帧BFI标志*/
    pstDecDtxState->enPrevBfi          = FR_BFI_STATUS_NOT;

    /*初始化状态机*/
    pstDecDtxState->shwStateCnt        = 0;
}


/*****************************************************************************
 函 数 名  : FR_DtxControlTx
 功能描述  : FR上行DTX控制
 输入参数  : FR_VAD_STATUS_ENUM_UINT16  enVadFlag       --VAD检测标志
             FR_ENC_DTX_STATE_STRU *pstEncDtxState  --DTX编码主结构体
 输出参数  : FR_ENC_DTX_STATE_STRU *pstEncDtxState  --DTX编码主结构体
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxControlTx(
                 FR_VAD_STATUS_ENUM_UINT16  enVadFlag,
                 FR_ENC_DTX_STATE_STRU     *pstEncDtxState)
{
    Word16                                  shwTmp;

    /* 累计增加距离上一个SID更新帧的帧数shwNelapsed */
    pstEncDtxState->shwNelapsed         = add(pstEncDtxState->shwNelapsed, 1);

    /* 如果VAD标志为1则直接返回 */
    if (FR_VAD_STATUS_YES == enVadFlag)
    {
        pstEncDtxState->shwHangover     = FR_DTX_HANGOVER;
        pstEncDtxState->shwCtrlWord     = (FR_TX_SP_FLAG | FR_TX_VAD_FLAG);
        return;
    }

    /* 语音延迟期等于0,表示语音延迟期,即hangover结束，需要更新SID */
    if (0 == pstEncDtxState->shwHangover)
    {
        /* 语音延迟期等于0,表示语音延迟期,即hangover结束，需要更新SID */
        pstEncDtxState->shwNelapsed     = 0;

        if ((pstEncDtxState->shwCtrlWord & FR_TX_HANGOVER_ACTIVE) != 0)
        {
            pstEncDtxState->shwCtrlWord = FR_TX_PREV_HANGOVER_ACTIVE
                                          | FR_TX_SID_UPDTE;
            pstEncDtxState->swPnSeed    = FR_DTX_PN_INITIAL_SEED;
        }
        else
        {
            pstEncDtxState->shwCtrlWord = FR_TX_SID_UPDTE;
        }
    }
    else
    {
        pstEncDtxState->shwHangover     = sub(pstEncDtxState->shwHangover, 1);

        shwTmp = add(pstEncDtxState->shwNelapsed, pstEncDtxState->shwHangover);

        if (shwTmp < FR_DTX_ELAPSED_THRESHOLD)
        {
            /* 使用上一帧SID帧参数  */
            pstEncDtxState->shwCtrlWord = FR_TX_USE_OLD_SID;
        }
        else
        {
            if ((pstEncDtxState->shwCtrlWord & FR_TX_HANGOVER_ACTIVE) != 0)
            {
                pstEncDtxState->shwCtrlWord = (FR_TX_PREV_HANGOVER_ACTIVE
                                            | FR_TX_HANGOVER_ACTIVE
                                            | FR_TX_SP_FLAG);
            }
            else
            {
                pstEncDtxState->shwCtrlWord = (FR_TX_HANGOVER_ACTIVE
                                            | FR_TX_SP_FLAG);
            }
        }
    }
}

/*****************************************************************************
 函 数 名  : FR_DtxControlRx
 功能描述  : FR下行DTX控制和丢失帧恢复，即根据帧标志生成DTX控制字，并设定对应帧类型解码参数
 输入参数  : FR_TAF_STATUS_ENUM_UINT16  enTaf           --TAF帧标志
             FR_BFI_STATUS_ENUM_UINT16  enBfi           --BFI帧标志
             FR_SID_STATUS_ENUM_UINT16  enSidFlag       --SID帧标志
             FR_DEC_DTX_STATE_STRU *pstDecDtxState  --下行DTX控制结构体状态
 输出参数  : Word16                    *pshwState       --DTX状态字
             FR_BFI_STATUS_ENUM_UINT16 *penPrevBfi      --上一帧BFI标志
             FR_DEC_DTX_STATE_STRU *pstDecDtxState  --下行DTX控制结构体状态
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxControlRx(
                 FR_TAF_STATUS_ENUM_UINT16                  enTaf,
                 FR_BFI_STATUS_ENUM_UINT16                  enBfi,
                 FR_SID_STATUS_ENUM_UINT16                  enSidFlag,
                 Word16                                    *pshwState,
                 FR_BFI_STATUS_ENUM_UINT16                 *penPrevBfi,
                 FR_DEC_DTX_STATE_STRU                     *pstDecDtxState)
{
    UWord16                             uhwFrameType;
    Word16                             *pshwCtrlWord;
    UWord16                             uhwIndex;

    /* 根据帧标志生成DTX下行结构体变量控制字状态值shwCtrlWord */
    pshwCtrlWord      = (Word16 *)&(pstDecDtxState->shwCtrlWord);

    uhwIndex          = (UWord16)(enBfi << 2) + enSidFlag;
    if (uhwIndex >= FR_FRAME_TYPE_NUM)
    {
        return;
    }

    uhwFrameType      = g_auhwFrFrmType[uhwIndex];

    /* 先更新pshwCtrlWord */
    if ((*pshwCtrlWord & FR_RX_SP_FLAG) != 0)
    {
        *pshwCtrlWord = (Word16)g_auhwFrPrevSp[uhwFrameType];
    }
    else
    {
        uhwIndex      = (0 == *pshwCtrlWord) ? 0 : 1;
        uhwIndex     += (UWord16)(uhwFrameType << 1);
        *pshwCtrlWord = (Word16)g_auhwFrPrevNotSp[uhwIndex];
    }

    /* 更新舒适噪声插入周期状态 */
    FR_DtxUpdateCniState(enTaf, pshwCtrlWord, uhwFrameType, pstDecDtxState);

    /* 更新SID帧状态记录参数 */
    FR_DtxUpdateSidRxState(pshwCtrlWord, pstDecDtxState);

    /* 更新丢失帧处理状态机 */
    FR_DtxRxControlState(*pshwCtrlWord, enBfi, penPrevBfi, pshwState);
}

/*****************************************************************************
 函 数 名  : FR_DtxUpdateCniState
 功能描述  : 更新舒适噪声插入周期状态
 输入参数  : FR_TAF_STATUS_ENUM_UINT16  enTaf       --TAF帧标志
             Word16                    *pshwCtrlWord   -- 控制字
             UWord16                    uhwFrameType-- 帧类型
             FR_DEC_DTX_STATE_STRU *pstDecDtxState  --下行DTX控制结构体状态
 输出参数  : Word16                    *pshwState       --DTX状态字
             FR_BFI_STATUS_ENUM_UINT16 *penPrevBfi      --上一帧BFI标志
             FR_DEC_DTX_STATE_STRU *pstDecDtxState  --下行DTX控制结构体状态
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxUpdateCniState(
                 FR_TAF_STATUS_ENUM_UINT16                  enTaf,
                 Word16                                    *pshwCtrlWord,
                 UWord16                                    uhwFrameType,
                 FR_DEC_DTX_STATE_STRU                     *pstDecDtxState)
{
    /* 语音帧处理 */
    if ((*pshwCtrlWord & FR_RX_SP_FLAG) != 0)
    {
        pstDecDtxState->shwSidLostFrameNum      = 0;
        pstDecDtxState->shwCniState             = FR_DTX_CN_INT_PERIOD - 1;
    }
    else
    {
        /* 第一个有效SID帧处理 */
        if ((*pshwCtrlWord & FR_RX_FIRST_SID_UPDATE) != 0)
        {
            pstDecDtxState->shwSidLostFrameNum  = 0;
            pstDecDtxState->shwCniState         = FR_DTX_CN_INT_PERIOD - 1;
        }

        /* 非第一个SID帧处理 */
        if ((*pshwCtrlWord & FR_RX_CONT_SID_UPDATE) != 0)
        {
            pstDecDtxState->shwSidLostFrameNum  = 0;

            if (FR_FRAME_TYPE_VALID_SID == uhwFrameType)
            {
                pstDecDtxState->shwCniState     = 0;
            }
            else
            {
                if ((FR_FRAME_TYPE_INVALID_SID == uhwFrameType)
                 && (pstDecDtxState->shwCniState < (FR_DTX_CN_INT_PERIOD - 1)))
                {
                    pstDecDtxState->shwCniState = (pstDecDtxState->shwCniState + 1);
                }
            }
         }

        /* 接收BFI为1的SID帧处理 */
        if ((*pshwCtrlWord & FR_RX_CNI_BFI) != 0)
        {

            if (pstDecDtxState->shwCniState < (FR_DTX_CN_INT_PERIOD - 1))
            {
                pstDecDtxState->shwCniState = (pstDecDtxState->shwCniState + 1);
            }

            /* 如果在接收舒适噪声时接收TAF标志1时，当前帧作为丢失SID帧处理*/
            if (FR_TAF_STATUS_YES == enTaf)
            {
                *pshwCtrlWord = *pshwCtrlWord | FR_RX_LOST_SID_FRAME;

                pstDecDtxState->shwSidLostFrameNum = add(pstDecDtxState->shwSidLostFrameNum, 1);
            }
            else
            {
                *pshwCtrlWord = *pshwCtrlWord | FR_RX_NO_TRANSMISSION;

            }

            /* 丢失的SID帧数累计变量shwSidLostFrameNum 大于 1 */
            if (pstDecDtxState->shwSidLostFrameNum > 1)
            {
                *pshwCtrlWord = *pshwCtrlWord | FR_RX_DTX_MUTING;
            }
        }
    }
}

/*****************************************************************************
 函 数 名  : FR_DtxUpdateSidRxState
 功能描述  : 更新SID帧状态记录参数
 输入参数  : Word16                    *pshwCtrlWord   -- 控制字
             FR_DEC_DTX_STATE_STRU *pstDecDtxState  --下行DTX控制结构体状态
 输出参数  : FR_DEC_DTX_STATE_STRU *pstDecDtxState  --下行DTX控制结构体状态
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxUpdateSidRxState(
                Word16                 *shwCtrlWord,
                FR_DEC_DTX_STATE_STRU  *pstDecDtxState)
{
    /* 距离上一个SID帧时长shwNelapsed加1 */
    pstDecDtxState->shwNelapsed = add(pstDecDtxState->shwNelapsed , 1);

    if ((*shwCtrlWord & FR_RX_SP_FLAG) != 0)
    {
        pstDecDtxState->shwHangoverPeriod     = FR_DTX_HANGOVER;
    }
    else
    {
        /* 距离上一个SID帧时长shwNelapsed大于阈值 */
        if (pstDecDtxState->shwNelapsed > FR_DTX_ELAPSED_THRESHOLD)
        {
            *shwCtrlWord                     |= FR_RX_UPD_SID_QUANT_MEM;
            pstDecDtxState->shwNelapsed       = 0;
            pstDecDtxState->shwHangoverPeriod = 0;
            pstDecDtxState->swPnSeed          = FR_DTX_PN_INITIAL_SEED;
        }
        else if (0 == pstDecDtxState->shwHangoverPeriod) /* hangover时长为0 */
        {
            pstDecDtxState->shwNelapsed       = 0;
        }
        else
        {
            pstDecDtxState->shwHangoverPeriod = (pstDecDtxState->shwHangoverPeriod - 1);
        }
    }
}

/*****************************************************************************
 函 数 名  : FR_DtxRxControlState
 功能描述  : 丢失帧处理状态机更新
 输入参数  : Word16                     shwCtrlWord    --DTX状态控制码字
             FR_BFI_STATUS_ENUM_UINT16  enBfi          --当前BFI帧标志
 输出参数  : FR_BFI_STATUS_ENUM_UINT16 *penPrevBfi     --上一帧BFI帧标志
             Word16                    *pshwState      --丢失帧处理状态机
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月9日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxRxControlState(
                 Word16                                     shwCtrlWord,
                 FR_BFI_STATUS_ENUM_UINT16                  enBfi,
                 FR_BFI_STATUS_ENUM_UINT16                 *penPrevBfi,
                 Word16                                    *pshwState)
{
    if (enBfi != CODEC_BFI_NO)
    {
        *pshwState = (*pshwState + 1); /* BFI帧丢失帧处理状态机pshwState加1 */
    }
    else if (6 == *pshwState)
    {
        *pshwState = 5;
    }
    else
    {
        *pshwState = 0;
    }

    /* 丢失帧处理状态机pshwState大于6则置为6 */
    if (*pshwState > 6)
    {
        *pshwState = 6;
    }

    /* 接收第一个语音帧 */
    if ((shwCtrlWord & FR_RX_FIRST_SP_FLAG) != 0)
    {
        *pshwState  = 5;
        *penPrevBfi = CODEC_BFI_NO;
    }
    else if ((shwCtrlWord & FR_RX_PREV_DTX_MUTING) != 0) /* 前一帧为静音帧 */
    {
        *pshwState  = 5;
        *penPrevBfi = 1;
    }
    else
    {
        /* suspend pc lint */
    }
}


/*****************************************************************************
 函 数 名  : FR_DtxEncSidPrm
 功能描述  : FR上行舒适噪声估计，即SID帧参数产生
 输入参数  : FR_ENC_DTX_STATE_STRU *pstEncDtxState  --上行DTX控制结构体
 输出参数  : Word16                    *pshwLarc        --反射系数对数面积比
             Word16                    *pshwXmaxc       --子块最大幅度值
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxEncSidPrm(
                 Word16                                    *pshwLarc,
                 Word16                                    *pshwXmaxc,
                 FR_ENC_DTX_STATE_STRU                 *pstEncDtxState)
{
    Word32                              swCnt1, swCnt2;
    Word32                              swAcc;
    Word16                              shwXmaxc;

    /* 使用上一帧SID帧参数 */
    if (pstEncDtxState->shwCtrlWord & FR_TX_USE_OLD_SID)
    {
        CODEC_OpVecCpy(pshwLarc, pstEncDtxState->ashwSidLar, FR_DTX_NUM_OF_LAR_COEF);
        CODEC_OpVecCpy(pshwXmaxc, pstEncDtxState->ashwSidXmax, FR_DTX_NUM_OF_BLOCK_COEF);
    }
    else
    {
        /* 计算当前SID的参数--对数面积比SID_LAR[8]: LARcHist[4][8] 每列4个元素求均值 */
        for (swCnt1 = 0; swCnt1 < 8; swCnt1++)
        {
            swAcc = 2;

            for(swCnt2 = 0; swCnt2 < 4; swCnt2++)
            {
                swAcc = L_add(swAcc, pstEncDtxState->ashwLarcHist[swCnt2][swCnt1]);
            }

            pshwLarc[swCnt1] = saturate(L_shr(swAcc, 2));
        }

        /* 对反射系数对数面积比pshwLARc量化编码 */
        Quantization_and_coding(pshwLarc);

        CODEC_OpVecCpy(pstEncDtxState->ashwSidLar, pshwLarc, 8);

        /* 计算当前SID的参数--ashwSidXmax[4] */

        swAcc = 8;

        for(swCnt1 = 0; swCnt1 < 4; swCnt1++)
        {
            for(swCnt2 = 0; swCnt2 < 4; swCnt2++)
            {
                swAcc = L_add(swAcc, pstEncDtxState->ashwXmaxHist[swCnt2][swCnt1]);
            }
        }

        /* 对子块最大幅度值pshwXmaxc[4]量化编码 */
        shwXmaxc = FR_DtxQxmax(saturate(L_shr(swAcc, 4)));

        CODEC_OpVecSet(&pshwXmaxc[0], 4, shwXmaxc);

        CODEC_OpVecCpy(pstEncDtxState->ashwSidXmax, pshwXmaxc, 4);
    }
}

/*****************************************************************************
 函 数 名  : FR_DtxQxmax
 功能描述  : 对子块最大幅度值参数进行量化
 输入参数  : Word16 shwXmax        --量化前子块最大幅度值
 输出参数  : 无
 返 回 值  : Word16                --量化后子块最大幅度值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月9日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
Word16 FR_DtxQxmax(Word16 shwXmax)
{
    Word16                              shwTemp, shwExp;
    Word16                              shwXmaxc;

    /* 取shwXmax高7位 */
    shwTemp = (Word16)(shwXmax & 0xfe00);

    if (!shwTemp)
    {
        shwExp = 0;
    }
    else
    {
        shwExp = 6 - norm_s(shwTemp);
    }

    /* 量化后子块最大幅度值shwXmaxc */
    shwTemp = shwExp + 5;
    shwXmaxc = add(shr(shwXmax, shwTemp), shl(shwExp, 3));

    return shwXmaxc;
}

/*****************************************************************************
 函 数 名  : FR_DtxUpdateSidMem
 功能描述  : DTX结构体Sid生成参数反射系数面积比和子块最大幅度值内存更新
 输入参数  : INT16 *LARuq                  --当前帧未量化反射系数对数面积比
             INT16 *xmaxuq                 --当前帧未量化子块最大幅度值
             FR_ENC_DTX_STATE_STRU *pstEncDtxState   --DTX编码主结构体
 输出参数  : FR_ENC_DTX_STATE_STRU *pstEncDtxState   --DTX编码主结构体
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月9日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxUpdateSidMem(
                Word16                                     *pshwLaruq,
                Word16                                     *pshwXmaxuq,
                FR_ENC_DTX_STATE_STRU                      *pstEncDtxState)
{
    Word16                              shwIndex;

    /* 置临时索引变量index为结构体状态变量更新帧索引值ashwHistIndx */
    shwIndex = pstEncDtxState->shwHistIndx;

    CODEC_OpVecCpy(&(pstEncDtxState->ashwLarcHist[shwIndex][0]), pshwLaruq, 8);
    CODEC_OpVecCpy(&(pstEncDtxState->ashwXmaxHist[shwIndex][0]), pshwXmaxuq, 4);

    shwIndex = (shwIndex + 1) & 0x0003;

    /* 更新结构体状态变量更新帧索引值ashwHistIndx */
    pstEncDtxState->shwHistIndx = shwIndex;
}

/*****************************************************************************
 函 数 名  : FR_DtxDecSidLarAndXmaxc
 功能描述  : FR下行产生舒适噪声反射系数对数面积比和子块最大幅度值
 输入参数  : Word16                    *pshwLarc       --LARc参数
             Word16                    *pshwLarppOld   --前一帧解码LAR参数
             Word16                    *pshwXmaxc      --子块最大幅度值
             FR_BFI_STATUS_ENUM_UINT16  enBfi          --BFI标志
             FR_DEC_DTX_STATE_STRU *pstDecDtxState --下行DTX控制结构体状态
 输出参数  : Word16                    *pshwLarppNew   --当前帧解码LAR参数
             Word16                    *pshwXmaxc      --子块最大幅度值
             FR_DEC_DTX_STATE_STRU *pstDecDtxState --下行DTX控制结构体状态
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxDecSidLarAndXmaxc(
                 Word16                                    *pshwLarc,
                 Word16                                    *pshwLarppNew,
                 Word16                                    *pshwLarppOld,
                 Word16                                    *pshwXmaxc,
                 FR_BFI_STATUS_ENUM_UINT16                  enBfi,
                 FR_DEC_DTX_STATE_STRU                     *pstDecDtxState)
{
    Word32                              swCnt;
    Word16                              shwMuteFlag = 0;
    Word16                              shwCtrlWord = pstDecDtxState->shwCtrlWord;

    /* 非BFI帧处理 */
    if (FR_BFI_STATUS_NOT == enBfi)
    {
        /* 对LARc参数解码存至变量pshwLarppNew */
        Decoding_of_the_coded_Log_Area_Ratios(pshwLarc, pshwLarppNew);

        /* 接收第一个有效SID帧 */
        if (shwCtrlWord & FR_RX_FIRST_SID_UPDATE)
        {
            CODEC_OpVecCpy(pstDecDtxState->ashwLarCnOld, pshwLarppNew, 8);
            CODEC_OpVecCpy(pstDecDtxState->ashwXmaxCnOld, pshwXmaxc, 4);
        }
        else
        {
            /* SID update帧, 更新舒适噪声参数 */
            CODEC_OpVecCpy(pstDecDtxState->ashwLarCnOld, pstDecDtxState->ashwLarCnNew, 8);
            CODEC_OpVecCpy(pstDecDtxState->ashwXmaxCnOld, pstDecDtxState->ashwXmaxCnNew, 4);
        }

        CODEC_OpVecCpy(pstDecDtxState->ashwLarCnNew, pshwLarppNew, 8);
        CODEC_OpVecCpy(pstDecDtxState->ashwXmaxCnNew, pshwXmaxc, 4);
    }
    else  /* BFI帧处理 */
    {
        if (shwCtrlWord & FR_RX_NO_TRANSMISSION) /* 舒适噪声不传输帧 */
        {
            /* 内插出当前帧解码LAR参数pshwLarppNew[8] */
            FR_DtxInterpolateCnVec(pstDecDtxState->ashwLarCnOld,
                                   pstDecDtxState->ashwLarCnNew,
                                   pshwLarppNew,
                                   pstDecDtxState->shwCniState,
                                   8);

            /* 内插出当前帧子块最大幅度值pshwXmaxc[4] */
            FR_DtxInterpolateCnVec(pstDecDtxState->ashwXmaxCnOld,
                                   pstDecDtxState->ashwXmaxCnNew,
                                   pshwXmaxc,
                                   pstDecDtxState->shwCniState,
                                   4);
        }
        else
        {
            /* 第一个丢失的语音帧或SID帧处理 */
            if (((shwCtrlWord & FR_RX_SP_FLAG)
              && (1 == pstDecDtxState->shwStateCnt))
              || (shwCtrlWord & FR_RX_DTX_MUTING))
            {
                /* 重复上一次好的LARs和Xmax */
                CODEC_OpVecCpy(pshwLarppNew, pstDecDtxState->ashwLarCnNew, 8);
                CODEC_OpVecCpy(pshwXmaxc, pstDecDtxState->ashwXmaxCnNew, 4);
            }
            else
            {
                /* 非第一个丢失语音帧，置静音标志为1 */
                shwMuteFlag = 1;
            }
        }
    }

    if (1 == shwMuteFlag)
    {
        /* 接下来的坏帧，重复上一次好的LARs，以4递减Xmax直到为0 */
        CODEC_OpVecCpy(pshwLarppNew, pstDecDtxState->ashwLarCnNew, 8);

        for (swCnt = 0; swCnt < 4; swCnt++)
        {
            pshwXmaxc[swCnt] = pstDecDtxState->ashwXmaxCnNew[swCnt] - 4;
            if (pshwXmaxc[swCnt] < 0)
            {
                pshwXmaxc[swCnt] = 0;
            }
        }

        CODEC_OpVecCpy(pstDecDtxState->ashwXmaxCnNew, pshwXmaxc, 4);

        /* 累计muting帧数 */
        pstDecDtxState->shwMuteCount = add(pstDecDtxState->shwMuteCount, 1);
    }
    else
    {
        /* muting帧数累计归0 */
        pstDecDtxState->shwMuteCount = 0;
    }
}

/*****************************************************************************
 函 数 名  : FR_DtxInterpolateCnVec
 功能描述  : 舒适噪声矢量内插
 输入参数  : Word16 pshwOldCn[]           --内插参考矢量2
             Word16 pshwNewCn[]           --内插参考矢量1
             Word16 shwRxDtxState        --输入噪声内插周期状态
             Word16 shwLen                --内插矢量长度
 输出参数  : Word16 pshwInterpCn[]        --内插输出矢量
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月9日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxInterpolateCnVec(
                Word16                  pshwOldCn[],
                Word16                  pshwNewCn[],
                Word16                  pshwInterpCn[],
                Word16                  shwRxDtxState,
                Word16                  shwLen)
{
    Word32                              swCnt;
    Word16                              shwFrateFactor1, shwFrateFactor2;
    Word16                              shwValue;
    Word32                              swTemp;

    /* 计算内插因子shwFrateFactor1和shwFrateFactor2 */
    shwValue                 = shl(shwRxDtxState,1);
    if (shwValue >= ((FR_DTX_CN_INT_PERIOD * 2) - 1))
    {
        return;
    }

    shwFrateFactor1          = g_ashwFrFrateFactor[shwValue];
    shwFrateFactor2          = g_ashwFrFrateFactor[shwValue+1];

    /* 计算舒适噪声矢量内插 */
    for (swCnt = 0; swCnt < shwLen; swCnt++)
    {
        swTemp          = L_mult(shwFrateFactor1, pshwNewCn[swCnt]);
        swTemp          = L_mac(swTemp, shwFrateFactor2, pshwOldCn[swCnt]);
        pshwInterpCn[swCnt] = round(swTemp);
    }
}

/*****************************************************************************
 函 数 名  : FR_DtxDecSidOtherPrms
 功能描述  : FR下行产生舒适噪声其他参数,即不包括反射系数对数面积比和子块最大
             幅度值
 输入参数  : Word16                    *pshwSeed       --舒适噪声RPE脉冲产生种子
 输出参数  : Word16                     pshwNc[4]      --LTP增益
             Word16                     pshwBc[4]      --LTP延迟参数
             Word16                     pshwMc[4]      --RPE-grid位置
             Word16                     pshwXmc[4*13]  --RPE-pluse码字xMc
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxDecSidOtherPrms(
                Word16                  pshwNc[4],
                Word16                  pshwBc[4],
                Word16                  pshwMc[4],
                Word16                  pshwXmc[4*13],
                Word32                 *pswSeed)
{

    Word32                              swCnt1, swCnt2;
    Word16                             *pshwTmpXmc;

    /* 设置LTP增益bc[4]为0 */
    for (swCnt1 = 0; swCnt1 < FR_DTX_NUM_OF_BLOCK_COEF; swCnt1++)
    {
        pshwBc[swCnt1] = 0;
    }

    /* 设置LTP延迟参数Nc[4]为固定常数 */
    pshwNc[0] = 40;
    pshwNc[2] = 40;
    pshwNc[1] = 120;
    pshwNc[3] = 120;

    /* 计算生成RPE-grid位置Mc[4]和RPE-pluse码字xMc[4*13] */

    pshwTmpXmc = pshwXmc;

    for (swCnt1 = 0; swCnt1 < FR_DTX_NUM_OF_BLOCK_COEF; swCnt1++)
    {
        /* RPE grid position Mc[]; 在0-3中随机产生(2bits) */
        pshwMc[swCnt1] = FR_DtxPseudoNoise(pswSeed, 2);

        /* RPE pluse xMc[]; 在0-7中随机产生(3bits) */
        for(swCnt2 = 0; swCnt2 < FR_DTX_NUM_OF_XMC; swCnt2++)
        {
            *pshwTmpXmc = FR_DtxPseudoNoise(pswSeed, 3);

            /* 压缩范围至 0-5 */
            *pshwTmpXmc = mult_r(*pshwTmpXmc, FR_DTX_COMPRESS_SEVEN_TO_FIVE);

            /* 调整范围至 1-6 */
            *pshwTmpXmc += 1;

            pshwTmpXmc++;
        }
    }
}

/*****************************************************************************
 函 数 名  : FR_DtxPseudoNoise
 功能描述  : 根据噪声生成种子生成给定比特数的感知噪声参数
 输入参数  : Word32    *pswShiftReg    --偏移种子
             Word16     shwNumBits     --要求生成参数的比特数
 输出参数  : Word32    *pswShiftReg    --偏移种子
 返 回 值  : Word16    --给定比特数生成参数
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月9日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
Word16 FR_DtxPseudoNoise(Word32 *pswShiftReg, Word16 shwNumBits)
{
    Word32                              swCnt;
    Word16                              shwState;
    Word16                              shwNoiseBits = 0;

    for (swCnt = 0; swCnt < shwNumBits; swCnt++)
    {
        /* State n == 31  */
        if (((*pswShiftReg) & 0x00000001L) != 0)
        {
            shwState = 1;
        }
        else
        {
            shwState = 0;
        }

        /* State n == 3  */
        if (((*pswShiftReg) & 0x10000000L) != 0)
        {
            shwState = shwState ^ 1;
        }
        else
        {
            shwState = shwState ^ 0;
        }

        shwNoiseBits = shl(shwNoiseBits, 1);
        shwNoiseBits = shwNoiseBits | (extract_l(*pswShiftReg) & 1);

        *pswShiftReg = L_shr(*pswShiftReg, 1);

        if (shwState & 0x0001)
        {
            *pswShiftReg = (*pswShiftReg) | 0x40000000L;
        }
    }

    return shwNoiseBits;
}

/*****************************************************************************
 函 数 名  : FR_DtxGenSilence
 功能描述  : FR下行静音帧参数生成
 输入参数  : 静音帧参数常量
 输出参数  : Word16                    pshwLARpp[8]   --待恢复的LAR参数
             Word16                    pshwNc[4]      --待恢复的Nc参数
             Word16                    pshwBc[4]      --待恢复的bc参数
             Word16                    pshwMc[4]      --待恢复的Mc参数
             Word16                    pshwXmaxc[4]   --待恢复的xmaxc参数
             Word16                    pshwXmc[4*13]  --待恢复的xMc参数
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_DtxGenSilence(
                 Word16                 pshwLARpp[],
                 Word16                 pshwNc[],
                 Word16                 pshwBc[],
                 Word16                 pshwMc[],
                 Word16                 pshwXmaxc[],
                 Word16                 pshwXmc[])
{
    Word32                              swCnt;
    Word16                             *pshwTmpXmc;
    Word16                             *pshwSidPara;

    pshwSidPara      = (Word16 *)&g_ashwFrFrateSidPara[0];

    /* 设置反射系数对数面积比LARpp[] */
    CODEC_OpVecCpy(pshwLARpp, pshwSidPara, FR_DTX_NUM_OF_LAR_COEF);

    /* 设置 LTP延迟;LTP增益;RPE-grid位置;RPE子块最大幅度值 */
    for (swCnt = 0; swCnt < FR_DTX_NUM_OF_BLOCK_COEF; swCnt++)
    {
        pshwNc[swCnt]    = 40;
        pshwBc[swCnt]    = 0;
        pshwMc[swCnt]    = 1;
        pshwXmaxc[swCnt] = 0;
    }

    /* 设置xMc[4*13] */
    pshwSidPara += FR_DTX_NUM_OF_LAR_COEF;
    pshwTmpXmc = pshwXmc;

    for (swCnt = 0; swCnt < FR_DTX_NUM_OF_BLOCK_COEF; swCnt++)
    {
        CODEC_OpVecCpy(pshwTmpXmc, pshwSidPara, FR_DTX_NUM_OF_XMC);
        pshwTmpXmc += FR_DTX_NUM_OF_XMC;
    }
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

