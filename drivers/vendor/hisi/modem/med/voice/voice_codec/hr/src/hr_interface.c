

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_op_vec.h"
#include "codec_com_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "hr_interface.h"
#include "hr_sp_dec.h"
#include "hr_sp_enc.h"
#include "hr_homing.h"
#include "hr_typedefs.h"

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*HR在Unvoice模式下各参数所占比特数表*/
const VOS_INT16 g_ashwHrUnvoicedParmBitsTable[18] =
{
    5, 11, 9, 8, 1, 2, 7, 7, 5,
    7, 7, 5, 7, 7, 5, 7, 7, 5
};

/*HR在Voice模式下各参数所占比特数表*/
const VOS_INT16 g_ashwHrVoicedParmBitsTable[18] =
{
    5, 11, 9, 8, 1, 2, 8, 9, 5,
    4, 9, 5, 4, 9, 5, 4, 9, 5
};

/*HR上一帧reset标志*/
VOS_INT16 g_shwHrOldResetFlag = 1;

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 HR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode)
{
    /*获取全局变量DTX标志指针*/
    VOS_INT32 *pshwDtxFlag      = (VOS_INT32 *)HR_GetDtxFlagPtr();

    /*置DTX标志*/
    *pshwDtxFlag      = (VOS_INT32)enDtxMode;

    /*HR上行初始化*/
    resetEnc();

    return CODEC_RET_OK;
}
VOS_UINT32 HR_Dec_Init(VOS_VOID)
{
    /*获取全局变量FR上一帧reset标志指针*/
    VOS_INT16 *pshwOldResetFlag = HR_GetOldResetPtr();

    /*置reset标志*/
    *pshwOldResetFlag = 1;

    /*HR下行初始化*/
    resetDec();

    return CODEC_RET_OK;
}
VOS_UINT32 HR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU  *pstEncOutPara)
{
    VOS_INT16                   shwResetFlag;
    VOS_INT16                   ashwEncSpeech[CODEC_FRAME_LENGTH_NB];       /*编码输入的8K采样率的PCM码流，每帧160个样点*/
    VOS_INT16                   ashwEncSerial[HR_CODED_BITS_LENGTH];            /*HR编码提取的比特流，共112个比特*/

    HR_ENCODE_SERIAL_STRU       stParamBuf;                                     /*HR编码结果输出20个参数(含VadFlag、SpFlag)*/

    /*获取全局变量DTX标志指针*/
    VOS_INT32 *pshwDtxFlag = (VOS_INT32 *)HR_GetDtxFlagPtr();

    /*每帧更新DTX标志*/
    *pshwDtxFlag = (VOS_INT32)pstEncInPara->enDtxMode;

    /*取PCM数据高13Bit有效*/
    CODEC_OpVcAnd(pstEncInPara->pshwEncSpeech,
                CODEC_FRAME_LENGTH_NB,
                (VOS_INT16)CODEC_PCM_MASK,
                &ashwEncSpeech[0]);

    /* 进行Homing帧检测，其检测结果在本次编码结束后应用 */
    shwResetFlag = (VOS_INT16)encoderHomingFrameTest(&ashwEncSpeech[0]);

    /*调用HR提供的接口speechEncoder，进行HR编码，输出20个参数(含VadFlag、SpFlag)*/
    speechEncoder(&ashwEncSpeech[0], (VOS_INT16 *)&stParamBuf);

    /*调用函数HR_UlFrameConvert进行上行帧格式转换，从18个参数提取出112个比特的比特流*/
    HR_UlFrameConvert((VOS_INT16 *)&stParamBuf, &ashwEncSerial[0]);

    /*调用函数ComCodecBits2DWords将112比特上行比特流转换为四字节紧凑排列码流*/
    CODEC_ComCodecBits2Bytes(&ashwEncSerial[0],
                           HR_CODED_BITS_LENGTH,
                           (VOS_INT8 *)pstEncOutPara->pshwEncSerial);

    /*将标志VadFlag、SpFlag输出*/
    pstEncOutPara->enVadFlag = stParamBuf.enVadFlag;
    pstEncOutPara->enSpFlag  = stParamBuf.enSpFlag;

    /*若检测到本帧为Homming帧，则需要重置编码器*/
    if (1 == shwResetFlag)
    {
        /*调用HR提供的接口resetEnc，重置编码器*/
        resetEnc();
    }

    return CODEC_RET_OK;
}
VOS_UINT32 HR_Decode(
                CODEC_DEC_IN_PARA_STRU  *pstVocodecDecPara,
                VOS_INT16 *pshwDecSpeech)
{
    VOS_INT16                   shwResetFlag;
    VOS_INT16                   shwOldResetFlag;
    VOS_INT16                   ashwDecSerial[HR_CODED_BITS_LENGTH];

    HR_DECODE_SERIAL_STRU       stParamBuf;

    /*获取全局变量上一帧reset标志指针*/
    VOS_INT16 *pshwOldResetFlag = HR_GetOldResetPtr();

    /*临时变量置上一帧reset标志*/
    shwOldResetFlag = *pshwOldResetFlag;

    /*将输入的四字节对齐紧凑排列的码流转换为112比特的比特流*/
    CODEC_ComCodecBytes2Bits((VOS_INT8 *)(pstVocodecDecPara->pshwDecSerial),
                           HR_CODED_BITS_LENGTH,
                           &ashwDecSerial[0]);

    /*将下行112比特的比特流合并为18个参数的参数流*/
    HR_DlFrameConvert(&ashwDecSerial[0], (VOS_INT16*)(&stParamBuf));

    /*获取解码标志*/
    stParamBuf.enBfiFlag    = pstVocodecDecPara->enBfiFlag;
    stParamBuf.enHrUfiFlag  = pstVocodecDecPara->enHrUfiFlag;
    stParamBuf.enSidFlag    = pstVocodecDecPara->enSidFlag;
    stParamBuf.enTafFlag    = pstVocodecDecPara->enTafFlag;

    if (1 == shwOldResetFlag)
    {
        /*上帧reset为1时,本帧Homing检测仅检测第一子帧*/
        shwResetFlag = (VOS_INT16)decoderHomingFrameTest((VOS_INT16*)(&stParamBuf), HR_HOMING_FIRST_SUBFRAME);
    }
    else
    {
        /*否则,不做Homing检测*/
        shwResetFlag = 0;
    }

    if (shwResetFlag && shwOldResetFlag)
    {
        /* 若上帧为Homing帧,且本帧Homing第一子帧检测也为真,则直接置下行码流为Homing格式 */
        CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_NB,
                      (VOS_INT16)CODEC_EHF_MASK);
    }
    else
    {
        /*调用HR函数speechDecoder进行解码，输出PCM码流*/
        speechDecoder((VOS_INT16*)(&stParamBuf), pshwDecSpeech);
    }

    /* 取输出的PCM码流的高13Bit有效 */
    CODEC_OpVcAnd(&pshwDecSpeech[0],
                CODEC_FRAME_LENGTH_NB,
                (VOS_INT16)CODEC_PCM_MASK,
                &pshwDecSpeech[0]);

    /* 若上帧非Homing帧,则本帧需进行Homing全帧检测 */
    if (!shwOldResetFlag)
    {
        shwResetFlag = (VOS_INT16)decoderHomingFrameTest((VOS_INT16*)(&stParamBuf), HR_HOMING_WHOLE_FRAME);
    }

    /* 若全帧或第一子帧检测出本帧为Homing帧,则重置声码器,置下行码流为Homing格式 */
    if (shwResetFlag)
    {
       resetDec();

       CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_NB,
                      (VOS_INT16)CODEC_EHF_MASK);
    }

    /*保存本帧Homing检测结果,便于下帧应用*/
    *pshwOldResetFlag = shwResetFlag;

    return CODEC_RET_OK;
}
VOS_VOID HR_DlFrameConvert(VOS_INT16 *pshwBits, VOS_INT16 *pshwEncodedSerial)
{
    VOS_INT16 i;

    /*HR各参数比特提取序列表指针*/
    VOS_INT16 *pshwParsBitsNumTbl;

    /* 下行输入pshwBits为112个比特的比特流,其Voice模式标志为34、35比特(从0开始计数)*/
    if ((0 == pshwBits[34])&&(0 == pshwBits[35]))
    {
        /*Unvoiced模式,使用Unvoiced模式下的比特提取序列表*/
        pshwParsBitsNumTbl = (VOS_INT16 *)HR_GetUnvoiceParsNumPtr();
    }
    else
    {
        /*Voiced模式，使用Voiced模式下的比特提取序列表*/
        pshwParsBitsNumTbl = (VOS_INT16 *)HR_GetVoiceParsNumPtr();
    }

    /*根据比特提取序列表进行参数恢复，将112个参数恢复为18个参数*/
    for (i = 0; i < HR_CODED_SERIAL_LENGTH; i++)
    {
        CODEC_ComCodecBits2Prm(pshwBits,
                             pshwParsBitsNumTbl[i],
                             pshwEncodedSerial++);

        pshwBits += pshwParsBitsNumTbl[i];
    }
}
VOS_VOID HR_UlFrameConvert(VOS_INT16 *pshwEncodedSerial, VOS_INT16 *pshwBits)
{
    VOS_INT16 i;

    /*HR各参数比特提取序列表指针*/
    VOS_INT16 *pshwParsBitsNumTbl;

    /*按HR码流参数结构解析HR编码器编码结果，共18个参数 */
    HR_ENCODE_SERIAL_STRU *pstSerial = (HR_ENCODE_SERIAL_STRU *)pshwEncodedSerial;

    /*若编码结果为Unvoiced模式*/
    if (0 == pstSerial->shwVoicing)
    {
        /*使用Unvoiced模式下的比特提取序列表*/
        pshwParsBitsNumTbl = (VOS_INT16 *)HR_GetUnvoiceParsNumPtr();
    }
    else
    {
        /*使用Voiced模式下的比特提取序列表*/
        pshwParsBitsNumTbl = (VOS_INT16 *)HR_GetVoiceParsNumPtr();
    }

    /*根据比特提取序列表进行比特提取，从18个参数中提取出共112个比特*/
    for (i = 0; i < HR_CODED_SERIAL_LENGTH; i++)
    {
        CODEC_ComCodecPrm2Bits(pshwEncodedSerial[i],
                             pshwParsBitsNumTbl[i],
                             pshwBits);

        pshwBits += pshwParsBitsNumTbl[i];
    }
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



