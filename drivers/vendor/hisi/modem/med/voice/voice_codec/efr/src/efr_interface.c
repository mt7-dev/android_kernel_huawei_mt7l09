

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_com_codec.h"
#include "efr_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "efr_codec.h"
#include "efr_e_homing.h"
#include "efr_d_homing.h"
#include "efr_sig_proc.h"
#include "efr_cnst.h"
#include "efr_dtx.h"

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
VOS_INT16 g_shwEfrOldResetFlag;                                                 /*标识上一次EFR声码器是否进行了重置*/

VOS_INT16 efr_synth_buf[CODEC_FRAME_LENGTH_NB + EFR_M];                       /*EFR解码输入序列缓存*/

VOS_INT16 efr_dtx_mode;                                                         /*EFR编解码器DTX功能打开标志*/

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 EFR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode)
{
    /*获取全局变量DTX标志指针*/
    VOS_INT16 *pshwDtxFlag      = EFR_GetDtxFlagPtr();

    /*置DTX标志*/
    *pshwDtxFlag      = (VOS_INT16)enDtxMode;

    /*初始化编码模块*/
    EFR_reset_enc();

    return CODEC_RET_OK;
}
VOS_UINT32 EFR_Dec_Init(VOS_VOID)
{
    /*获取全局变量FR上一帧reset标志指针*/
    VOS_INT16 *pshwOldResetFlag = EFR_GetOldResetPtr();

    /*置reset标志*/
    *pshwOldResetFlag = 1;

    /*初始化解码模块*/
    EFR_reset_dec();

    return CODEC_RET_OK;
}
VOS_UINT32 EFR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara)
{
    VOS_INT16     shwResetFlag;
    VOS_INT16     ashwParam[ERF_CODED_PRMS_LENGTH];                             /*编码器输出结果参数流,共57个参数*/
    VOS_INT16     ashwSyn[CODEC_FRAME_LENGTH_NB];                           /*Debug信息输出缓存 */
    VOS_INT16     ashwSerial[EFR_CODED_BITS_LENGTH];                            /*编码结果参数流提取后逐比特排列的比特流，共244个比特*/

    VOS_INT16    *pshwNewSpeech;
    VOS_INT16    *pshwDtxFlag;
    VOS_INT16    *pshwTxDtxCtrl;

    /*引用编码器内部全局变量，上行编码输入序列缓存指针*/
    pshwNewSpeech          = EFR_GetNewSpeechPtr();

    /*引用编码器内部全局变量，DTX模式标志指针*/
    pshwDtxFlag            = EFR_GetDtxFlagPtr();

    /*引用编码器内部全局变量，上行DTX控制字*/
    pshwTxDtxCtrl          = &EFR_GetTxDtxCtrl();

    /*每帧更新DTX参数*/
    *pshwDtxFlag = (VOS_INT16)(pstEncInPara->enDtxMode);

    /*将编码前PCM数据拷入编码器内部全局变量地址中*/

    /* 调用EFR函数EFR_encoder_homing_frame_test进行上行Homing帧检测 */
    shwResetFlag = EFR_encoder_homing_frame_test (pstEncInPara->pshwEncSpeech);

    /*取PCM数据高13Bit有效*/
    CODEC_OpVcAnd(pstEncInPara->pshwEncSpeech,
                CODEC_FRAME_LENGTH_NB,
                (VOS_INT16)CODEC_PCM_MASK,
                pshwNewSpeech);

    /* 调用EFR函数EFR_Pre_Process进行编码预处理，包括滤波、缩放 */
    EFR_Pre_Process(pshwNewSpeech, CODEC_FRAME_LENGTH_NB);

    /* 调用EFR函数EFR_Coder_12k2进行编码，输出57个参数及部分Debug信息 */
    EFR_Coder_12k2(&ashwParam[0], &ashwSyn[0]);

    /* 若编码后检测为非语音，则构造舒适噪声参数*/
    if ((*pshwTxDtxCtrl & EFR_TX_SP_FLAG) == 0)
    {
        EFR_CN_encoding(&ashwParam[0], *pshwTxDtxCtrl);
    }

    /*调用EFR函数EFR_Prm2bits_12k2，将57个参数提取为244个逐比特排列的比特流*/
    EFR_Prm2bits_12k2(&ashwParam[0], &ashwSerial[0]);

    /*若编码后检测为非语音，则还需要对提取后的某些比特进行额外处理S*/
    if ((*pshwTxDtxCtrl & EFR_TX_SP_FLAG) == 0)
    {
        EFR_sid_codeword_encoding(&ashwSerial[0]);
    }

    /*将244比特逐比特排列的比特流格式转换为紧凑排列的码流格式*/
    CODEC_ComCodecBits2Bytes(&ashwSerial[0],
                           EFR_CODED_BITS_LENGTH,
                           (VOS_INT8 *)&(pstEncOutPara->pshwEncSerial[0]));

    /* 根据编码器编码后状态，获取VadFlag、SpFlag标志值 */
    pstEncOutPara->enVadFlag = CODEC_VAD_SILENCE;
    pstEncOutPara->enSpFlag  = CODEC_SP_SID;

    if ((*pshwTxDtxCtrl & EFR_TX_VAD_FLAG) != 0)
    {
        pstEncOutPara->enVadFlag = CODEC_VAD_SPEECH;
    }
    if ((*pshwTxDtxCtrl & EFR_TX_SP_FLAG) != 0)
    {
        pstEncOutPara->enSpFlag = CODEC_SP_SPEECH;
    }

    /*若上行Homing检测为真，则重置编码器 */
    if (shwResetFlag != 0)
    {
        EFR_reset_enc ();
    }

    return CODEC_RET_OK;
}


VOS_UINT32 EFR_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech)
{
    VOS_INT16   shwResetFlag;
    VOS_INT16   ashwAzDec[EFR_AZ_SIZE];                                         /*解码后滤波系数，共44个*/
    VOS_INT16   ashwSerial[EFR_CODED_BITS_LENGTH + 1];                          /*BFI+解码参数流合并前的逐比特排列的比特流，共1+244个比特*/

    VOS_INT16  *pshwSynthBuf;
    VOS_INT16  *pshwSynthSpBuf;

    VOS_INT16  *pshwOldResetFlag;
    VOS_INT16   shwOldResetFlag;

    EFR_DECODE_FRAME_STRU stEfrFrame;                                           /*解码参数结构体*/

    /*获取全局变量EFR解码输入序列指针*/
    pshwSynthBuf   = EFR_GetSynthBufPtr();
    pshwSynthSpBuf = pshwSynthBuf + EFR_M;

    /*获取全局变量EFR上一帧reset标志指针*/
    pshwOldResetFlag = EFR_GetOldResetPtr();
    shwOldResetFlag  = *pshwOldResetFlag;

    /*将BFI标志直接赋给解码输入参数的对应位置，按协议代码进行，未作修改*/
    ashwSerial[0] = (VOS_INT16)(pstDecPara->enBfiFlag);

    /*将紧凑排列的码流格式转换为244比特逐比特排列的比特流格式*/
    CODEC_ComCodecBytes2Bits((VOS_INT8 *)pstDecPara->pshwDecSerial,
                           EFR_CODED_BITS_LENGTH,
                           &ashwSerial[1]);

    /*调用EFR函数EFR_Bits2prm_12k2，将1+244比特的比特流转换为1+57个参数的解码参数流*/
    EFR_Bits2prm_12k2(&ashwSerial[0], (VOS_INT16 *)&stEfrFrame);

    /*根据协议，当BFI为FALSE时进行上行Homing检测*/
    if (0 == stEfrFrame.enBfiFlag)
    {
        /*若上帧进行了重置(Homing全帧检测为TRUE)，则本帧只需要进行下行Homing第一子帧检测*/
        if (1 == shwOldResetFlag)
        {
            shwResetFlag = EFR_decoder_homing_frame_test(stEfrFrame.ashwSerial,
                                                         EFR_HOMING_FIRST_SUBFRAME);
        }
        else
        {
            /*否则，不做Homing检测，也无需重置解码器*/
            shwResetFlag = 0;
        }
    }
    /* 否则，本帧为坏帧,跳过本帧的Homing检测*/
    else
    {
         shwResetFlag = 0;
    }

    /*若上帧为Homing帧,且本帧Homing第一子帧检测也为真,则直接置下行码流为Homing格式*/
    if ((shwResetFlag != 0) && (shwOldResetFlag != 0))
    {
        CODEC_OpVecSet(pshwSynthSpBuf,
                     CODEC_FRAME_LENGTH_NB,
                     (VOS_INT16)CODEC_EHF_MASK);
    }
    else
    {
        /*调用EFR函数EFR_Decoder_12k2进行解码，输出PCM码流*/
        EFR_Decoder_12k2((VOS_INT16 *)&stEfrFrame,
                         &pshwSynthSpBuf[0],
                         &ashwAzDec[0],
                         (VOS_INT16)pstDecPara->enTafFlag,
                         (VOS_INT16)pstDecPara->enSidFlag);

        /*调用EFR函数EFR_Post_Filter进行解码后码流后滤波处理*/
        EFR_Post_Filter(&pshwSynthSpBuf[0], &ashwAzDec[0]);                      /* Post-filter */

        /*将15比特范围扩展至16比特范围，即左移1bit*/
        CODEC_OpVecShl(pshwSynthSpBuf,
                     CODEC_FRAME_LENGTH_NB,
                     1,
                     pshwSynthSpBuf);

        /*取输出的PCM码流的高13Bit有效 */
        CODEC_OpVcAnd(pshwSynthSpBuf,
                    CODEC_FRAME_LENGTH_NB,
                    (VOS_INT16)CODEC_PCM_MASK,
                    pshwSynthSpBuf);

    }

    CODEC_OpVecCpy(pshwDecSpeech,
                 pshwSynthSpBuf,
                 CODEC_FRAME_LENGTH_NB);

    /* 若本帧BFI为FALSE且上帧非Homing帧,则本帧需进行Homing全帧检测 */
    if ((0 == stEfrFrame.enBfiFlag) && (0 == shwOldResetFlag))
    {
        shwResetFlag = EFR_decoder_homing_frame_test(stEfrFrame.ashwSerial,
                                                     EFR_HOMING_WHOLE_FRAME);
    }

    /* 若全帧或第一子帧检测出本帧为Homing帧,则重置声码器,置下行码流为Homing格式 */
    if (shwResetFlag != 0)
    {
       /*调用EFR提供的接口EFR_reset_dec，重置解码器*/
        EFR_reset_dec();

        CODEC_OpVecSet(pshwSynthSpBuf,
                     CODEC_FRAME_LENGTH_NB,
                     (VOS_INT16)CODEC_EHF_MASK);
    }

    /*保存本帧Homing检测结果,便于下帧应用*/
    *pshwOldResetFlag = shwResetFlag;

    return CODEC_RET_OK;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

