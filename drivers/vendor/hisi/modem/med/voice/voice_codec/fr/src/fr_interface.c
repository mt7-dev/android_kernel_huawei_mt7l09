

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_com_codec.h"
#include "fr_interface.h"
#include "fr_codec.h"
#include "fr_enc.h"
#include "fr_dec.h"
#include "fr_homing.h"
#include "stdlib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  3 全局变量定义
*****************************************************************************/
const VOS_INT16 g_ashwFrBitNo[FR_CODED_SERIAL_LENGTH] =
{
    6, 6, 5, 5, 4, 4, 3, 3,                                                     /*LARc系数对应比特数*/
    7, 2, 2, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,                          /*第一个子帧系数对应比特数*/
    7, 2, 2, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,                          /*第二个子帧系数对应比特数*/
    7, 2, 2, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,                          /*第三个子帧系数对应比特数*/
    7, 2, 2, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3                           /*第四个子帧系数对应比特数*/
};

FR_ENCODE_STATE_STRU   g_stFrEncodeState;                                       /*FR编码主结构体全局变量*/

FR_DECODE_STATE_STRU   g_stFrDecodeState;                                       /*FR编码主结构体全局变量*/

VOS_UINT16 g_uhwFrOldResetFlag;                                                  /*解码器重置标志记录*/

/*****************************************************************************
  4 函数实现
*****************************************************************************/


VOS_UINT32 FR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode)
{
    /*获取全局变量FR编码主结构体指针*/
    FR_ENCODE_STATE_STRU *pstFrEncState = FR_GetEncStatePtr();

    /*调用MED_FR_EncInit函数进行编码初始化*/
    FR_EncInit(pstFrEncState, enDtxMode);

    return CODEC_RET_OK;
}


VOS_UINT32 FR_Dec_Init(VOS_VOID)
{
    /*获取全局变量FR解码主结构体指针*/
    FR_DECODE_STATE_STRU *pstFrDecState = FR_GetDecStatePtr();

    /*获取全局变量FR上一帧reset标志指针*/
    VOS_UINT16 *puhwOldResetFlag        = FR_GetOldResetPtr();

    /*将全局变量FR上一帧reset标志初始化为1*/
    *puhwOldResetFlag = 1;

    /*调用MED_FR_DecInit函数进行解码初始化*/
    FR_DecInit(pstFrDecState);

    return CODEC_RET_OK;
}


VOS_UINT32 FR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara)
{
    VOS_UINT16  uhwResetFlag;
    VOS_INT16   ashwNewSpeech[CODEC_FRAME_LENGTH_NB];                       /*编码原始PCM码流缓存,共160个16Bit*/
    VOS_INT16   ashwEncodedSerial[FR_CODED_BITS_LENGTH];                        /*编码参数流提取后的逐比特排列的比特流缓存,共260个比特*/
    VOS_INT16   shwCnt;
    VOS_INT16   shwTemp;
    FR_ENCODED_SERIAL_STRU stFrSerial;

    /*获取全局变量FR编码主结构体指针*/
    FR_ENCODE_STATE_STRU *pstFrEncState = FR_GetEncStatePtr();

    /*更新DTX标志*/
    pstFrEncState->enDtxFlag = pstEncInPara->enDtxMode;

    /*上行Homing检测*/
    uhwResetFlag = FR_EncDetectHomingFrame(pstEncInPara->pshwEncSpeech);

    /* 插接近静音帧的小幅度随机数 */
    for(shwCnt = 0; shwCnt < CODEC_FRAME_LENGTH_NB; shwCnt++)
    {
        shwTemp             = (rand() & 0x7fff ) & FR_SIGNAL_RAND_RANGE_15; /* [false alarm]:经确认此处使用rand无问题  */
        pstEncInPara->pshwEncSpeech[shwCnt] =  CODEC_OpAdd(pstEncInPara->pshwEncSpeech[shwCnt],
                                                           shwTemp - (FR_SIGNAL_RAND_RANGE_15>>1));
    }

    /*取PCM数据高13Bit有效*/
    CODEC_OpVcAnd(pstEncInPara->pshwEncSpeech,
                CODEC_FRAME_LENGTH_NB,
                (VOS_INT16)CODEC_PCM_MASK,
                &ashwNewSpeech[0]);

    /*FR编码生成76个编码参数*/
    gsm_encode(pstFrEncState, &ashwNewSpeech[0], (VOS_INT16 *)&stFrSerial);

    /*输出enVadFlag、enSpFlag参数，分别存于ashwLarc[0]和ashwLarc[1]高16bit位*/
    pstEncOutPara->enVadFlag = ((stFrSerial.ashwLarc[0] & BIT15_MASK) >> 15) & BIT0_MASK;
    pstEncOutPara->enSpFlag  = ((stFrSerial.ashwLarc[1] & BIT15_MASK) >> 15) & BIT0_MASK;

    /*若上行为Homing帧,重置编码器*/
    if (1 == uhwResetFlag)
    {
        FR_EncInit(pstFrEncState, pstEncInPara->enDtxMode);
    }

    /*将76个参数转换为260个非紧凑比特流*/
    FR_UlFrameConvert((VOS_INT16 *)&stFrSerial, &ashwEncodedSerial[0]);

    /*将260个非紧凑比特流转换为紧凑码流格式*/
    CODEC_ComCodecBits2Bytes(&ashwEncodedSerial[0],
                           FR_CODED_BITS_LENGTH,
                           (VOS_INT8 *)(pstEncOutPara->pshwEncSerial));

    return CODEC_RET_OK;
}
VOS_UINT32 FR_Decode(
                CODEC_DEC_IN_PARA_STRU  *pstDecPara,
                VOS_INT16                   *pshwDecSpeech)
{
    VOS_UINT16  uhwResetFlag;
    VOS_UINT16  uhwOldResetFlag;
    VOS_INT16   ashwDecodedSerial[FR_CODED_BITS_LENGTH];                        /*FR解码前260个比特逐比特排列的比特流缓存*/

    FR_DECODE_FRAME_STRU   stFrFrame;

    /*获取全局变量FR编码主结构体指针*/
    FR_DECODE_STATE_STRU *pstFrDecodeState = FR_GetDecStatePtr();

    /*获取全局变量FR上一帧reset标志指针*/
    VOS_UINT16 *puhwOldResetFlag           = FR_GetOldResetPtr();

    /*临时变量FR上一帧reset标志赋值*/
    uhwOldResetFlag                        = *puhwOldResetFlag;

    /*将4字节紧凑排列的码流格式转换为260个逐比特排列的比特流格式*/
    CODEC_ComCodecBytes2Bits((VOS_INT8 *)(pstDecPara->pshwDecSerial),
                           FR_CODED_BITS_LENGTH,
                           ashwDecodedSerial);

    /*将260个比特的比特流转换合并为76个参数的解码参数流*/
    FR_DlFrameConvert(&ashwDecodedSerial[0], (VOS_INT16 *)(&stFrFrame));

    /* 接收SID/TAF/BFI解码标志,依次放入解码参数流对应位置*/
    stFrFrame.enSidFlag = pstDecPara->enSidFlag;
    stFrFrame.enTafFlag = pstDecPara->enTafFlag;
    stFrFrame.enBfiFlag = pstDecPara->enBfiFlag;

    /* 根据协议规定，若上帧进行了重置(Homing全帧检测为TRUE)，则本帧只需要进行下行Homing第一子帧检测 */
    if (1 == uhwOldResetFlag)
    {
        uhwResetFlag = FR_DecDetectHomingFrame((VOS_INT16 *)(&stFrFrame),
                                                FR_HOMING_FIRST_SUBFRAME);
    }
    else
    {
        uhwResetFlag = 0;
    }

    /* 若上帧为Homing帧,且本帧Homing第一子帧检测也为真,则直接置下行码流为Homing格式 */
    if (uhwResetFlag && uhwOldResetFlag)
    {
        CODEC_OpVecSet(pshwDecSpeech,
                     CODEC_FRAME_LENGTH_NB,
                     CODEC_EHF_MASK);
    }
    else
    {
        /* 调用FR解码函数gsm_decode根据进行解码,输入76+3个参数(包括SID/TAF/BFI) */
        gsm_decode(pstFrDecodeState,
                   (VOS_INT16 *)(&stFrFrame),
                   &pshwDecSpeech[0]);
    }

    /* 取输出的PCM码流的高13Bit有效 */
    CODEC_OpVcAnd(pshwDecSpeech,
                CODEC_FRAME_LENGTH_NB,
                (VOS_INT16)CODEC_PCM_MASK,
                pshwDecSpeech);

    /* 若上帧非Homing帧,则本帧需进行Homing全帧检测 */
    if (0 == uhwOldResetFlag)
    {
        uhwResetFlag = FR_DecDetectHomingFrame((VOS_INT16 *)(&stFrFrame),
                                                FR_HOMING_WHOLE_FRAME);
    }

    /* 若本帧为Homing帧,则重置声码器,置下行码流为Homing格式 */
    if (uhwResetFlag)
    {
        FR_DecInit(pstFrDecodeState);

        CODEC_OpVecSet(pshwDecSpeech,
                     CODEC_FRAME_LENGTH_NB,
                     CODEC_EHF_MASK);
    }

    /*保存本帧Homing检测结果,便于下帧应用*/
    *puhwOldResetFlag = uhwResetFlag;

    return CODEC_RET_OK;
}



VOS_VOID FR_DlFrameConvert(
                VOS_INT16               *pshwFrFrame,
                VOS_INT16               *pshwEncodedSerial)
{
    VOS_INT32 i;

    /*获取FR各参数比特个数表*/
    VOS_INT16 *pshwFrParsBitsNumTable = (VOS_INT16 *)FR_GetParsNumPtr();

    /*将260bit的下行比特流pFrFrame转换为76个解码参数流pEncodedSerial*/
    for (i = 0; i < FR_CODED_SERIAL_LENGTH; i++)
    {
        CODEC_ComCodecBits2Prm(pshwFrFrame,
                             pshwFrParsBitsNumTable[i],
                             pshwEncodedSerial++);

        pshwFrFrame += pshwFrParsBitsNumTable[i];
    }
}


VOS_VOID FR_UlFrameConvert(VOS_INT16 *pshwEncodedSerial, VOS_INT16 *pshwFrFrame)
{
    VOS_INT32 i;

    /*获取FR各参数比特个数表*/
    VOS_INT16 *pshwFrParsBitsNumTable = (VOS_INT16 *)FR_GetParsNumPtr();

    /*将76个参数转换为260个非紧凑比特流*/
    for (i = 0; i < FR_CODED_SERIAL_LENGTH; i++)
    {
        CODEC_ComCodecPrm2Bits(pshwEncodedSerial[i],
                             pshwFrParsBitsNumTable[i],
                             pshwFrFrame);

        pshwFrFrame += pshwFrParsBitsNumTable[i];
    }
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


