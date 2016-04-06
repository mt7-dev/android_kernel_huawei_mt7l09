

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_op_vec.h"
#include "codec_com_codec.h"
#include "amrwb_interface.h"
#include "amrwb_convert.h"
#include "WttfCodecInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -e958 -e760  */
#include "amrwb_cod_main.h"
#include "amrwb_dec_main.h"
#include "amrwb_main.h"
#include "amrwb_dtx.h"
#include "amrwb_bits.h"
/*lint +e958 +e760  */

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
Coder_State                            *g_pstAmrwbEncObj = VOS_NULL;            /*AMRWB编码器状态指针*/
Decoder_State                          *g_pstAmrwbDecObj = VOS_NULL;            /*AMRWB解码器状态指针*/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : AMRWB_Enc_Init
 功能描述  : AMRWB编码初始化函数
 输入参数  : enDtxMode, 使能DTX功能模式, 1为使能, 0为禁用
             pfCodecCallBack, 帧类型回调函数
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
 调用函数  : Init_coder()
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AMRWB_Enc_Init(
                CODEC_DTX_ENUM_UINT16       enDtxMode,
                VOS_VOID                   *pfCodecCallBack)
{
    Coder_State                **ppstEncObj;

    if (enDtxMode >= CODEC_DTX_BUTT)
    {
        return CODEC_RET_ERR;
    }

    /*获取全局变量AMRWB编码状态结构体指针的指针*/
    ppstEncObj     = AMRWB_GetPtrOfEncObjPtr();

    /* 调用AMRWB声码器编码初始化函数 */
    Init_coder((void **)ppstEncObj);

    /* 初始化CODEC回掉函数，在完成VAD检测后直接输出帧类型给RLC层*/
    (*ppstEncObj)->pfCallBack = (VOS_INT16 *)pfCodecCallBack;

    /* 重置AMR_WB声码器上行状态 */
    Reset_tx_state();

    return CODEC_RET_OK;
}

/*****************************************************************************
 函 数 名  : AMRWB_Dec_Init
 功能描述  : AMRWB解码初始化函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
 调用函数  : AmrwbClrAllocatedObjCnt()
             Init_decoder()
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AMRWB_Dec_Init()
{
    VOS_UINT16                  *puhwSidFirstFlag;
    Decoder_State              **ppstDecObj;

    /*获取全局变量AMRWB一次通话过程中是否收到过SID_FIRST帧标志,并初始化为0*/
    puhwSidFirstFlag  = AMRWB_GetSidFirstFlagPtr();
    *puhwSidFirstFlag = 0;

    /*获取全局变量AMRWB解码状态结构体指针的指针*/
    ppstDecObj     = AMRWB_GetPtrOfDecObjPtr();

    /* 调用AMRWB声码器解码初始化函数 */
    Init_decoder((void **)ppstDecObj);

    /* 重置AMR_WB声码器下行状态 */
    Reset_rx_state();

    return CODEC_RET_OK;
}

/*****************************************************************************
 函 数 名  : AMRWB_Encode
 功能描述  : AMRWB编码
 输入参数  : pstEncInPara      --  指向声码器编码输入参数结构体的指针
 输出参数  : pstEncOutPara     --  指向编码后输出参数结构体的指针
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
 调用函数  : encoder_homing_frame_test()
             Speech_Encode_Frame()
             AMRWB_UmtsUlConvert()
             AMRWB_GsmUlConvert()
             Speech_Encode_Frame_reset()
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AMRWB_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara)
{
    VOS_INT16                           *pshwSpeech;
    VOS_INT16                            shwResetFlag;
    VOS_INT16                            shwEncSerialLen;

    CODEC_AMRWB_MODE_ENUM_UINT16    enMode;
    CODEC_RET_ENUM_UINT32           enRetResult;
    CODEC_AMRWB_TYPE_TX_ENUM_UINT16 enTxType    = CODEC_AMRWB_TYPE_TX_SPEECH_GOOD;

    enum AMRWB_TXFrameType          enTxTypeTmp = (enum AMRWB_TXFrameType)enTxType;

    AMRWB_ENCODED_SERIAL_STRU       stSerial;
    Coder_State                    *pstAmrEncObj;

    /* 需要在编码前保存输入的AMRWB速率，防止该信息被ID_GPHY_MED_UPDATE_PARA_CMD消息更新 */
    VOS_UINT16                      enEncInMode;

    enRetResult     = CODEC_RET_OK;

    /*获取全局变量AMRWB编码状态结构体指针*/
    pstAmrEncObj    = AMRWB_GetEncObjPtr();

    pshwSpeech      = pstEncInPara->pshwEncSpeech;

    /* 当AMR速率合法时，更新声码器速率信息 */
    if ( (WTTFVOICE_AMRWB_CODECMODE_TYPE_ENUM_UINT16)pstEncInPara->enAmrMode < WTTFVOICE_AMRWB_CODECMODE_TYPE_BUTT )
    {
        enEncInMode = pstEncInPara->enAmrMode;
    }
    else
    {
        enEncInMode = WTTFVOICE_AMRWB_CODECMODE_TYPE_660K;
    }

    enMode     = AMRWB_GetRealEncMode(enEncInMode);

    /*检查输入PCM帧是否为homing帧*/
    shwResetFlag = AMRWB_encoder_homing_frame_test(pshwSpeech);

    /* 对于宽带，16比特PCM编解码器仅取高14比特量化有效 */
    CODEC_OpVcAnd(pshwSpeech, (Word32)CODEC_FRAME_LENGTH_WB, CODEC_WB_PCM_MASK, pshwSpeech);

    /*调用AMRWB编码接口编码一帧*/
    AMRWB_coder((VOS_INT16 *)&enMode,
                pshwSpeech,
                stSerial.ashwEncSerial,
                &shwEncSerialLen,
                pstAmrEncObj,
                (VOS_INT16)pstEncInPara->enDtxMode,
                &enTxTypeTmp);

    enTxType    = (CODEC_AMRWB_TYPE_TX_ENUM_UINT16)enTxTypeTmp;

    /*获取帧类型及编码模式*/
    stSerial.uhwFrameType = enTxType;

    if (enTxType != CODEC_AMRWB_TYPE_TX_NO_DATA)
    {
        stSerial.enAmrMode = enEncInMode;
    }
    else
    {
        stSerial.enAmrMode = CODEC_AMRWB_MODE_MRDTX;
    }

    /*IF1 UMTS码流格式*/
    if (CODEC_AMR_FORMAT_IF1 == pstEncInPara->enAmrFormat)
    {
        enRetResult = AMRWB_UmtsUlConvert(&stSerial,
                                        (VOICE_WPHY_AMR_SERIAL_STRU *)pstEncOutPara->pshwEncSerial);
    }
    /*GSM码流格式*/
    else if (CODEC_AMR_FORMAT_BITS == pstEncInPara->enAmrFormat)
    {
        enRetResult = AMRWB_GsmUlConvert(&stSerial,
                                       pstEncOutPara->pshwEncSerial);
    }
    /*IMS码流格式*/
    else if (CODEC_AMR_FORMAT_IMS == pstEncInPara->enAmrFormat)
    {
        /* 先进行IF1 UMTS码流格式转换 */
        enRetResult = AMRWB_UmtsUlConvert(&stSerial,
                                        (VOICE_WPHY_AMR_SERIAL_STRU *)pstEncOutPara->pshwEncSerial);
        /* 再进行IMS码流格式转换 */
        AMRWB_ImsUlConvert(enTxType,enMode,pstEncOutPara->pshwEncSerial);
    }
    /*不支持*/
    else
    {
        return CODEC_RET_ERR;
    }

    if (shwResetFlag != 0)
    {
        Reset_encoder(pstAmrEncObj, 1);
    }

    pstEncOutPara->enAmrFrameType = enTxType;

    /* 保存AMRWB的编码速率 */
    pstEncOutPara->enAmrMode = enMode;

    return enRetResult;
}

/*****************************************************************************
 函 数 名  : AMRWB_Decode
 功能描述  : AMRWB解码
 输入参数  : pstDecPara     --指向声码器解码结构体的指针
 输出参数  : pshwDecSpeech  --解码数据,长度为160个字节的PCM数据
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
 调用函数  : AMRWB_UmtsDlConvert()
             decoder_homing_frame_test_first()
             Speech_Decode_Frame()
             decoder_homing_frame_test()
             Speech_Decode_Frame_reset()
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AMRWB_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech)
{

    VOS_INT16                           shwResetFlag    = 0;
    VOS_INT16                           shwOldResetFlag = 0;
    VOS_INT16                           shwFrameLen;
    AMRWB_ENCODED_SERIAL_STRU           stSerial;

    Decoder_State                      *pstAmrDecObj;

    CODEC_AMRWB_MODE_ENUM_UINT16        enMode;
    CODEC_AMRWB_TYPE_RX_ENUM_UINT16     enRxType;

    CODEC_RET_ENUM_UINT32               enRetResult = CODEC_RET_OK;

    /*获得AMRWB解码状态结构体全局变量指针*/
    pstAmrDecObj    = AMRWB_GetDecObjPtr();

    shwOldResetFlag = pstAmrDecObj->shwOldResetFlag;

    if (CODEC_AMR_FORMAT_IF1 == pstDecPara->enAmrFormat)
    {
        enRetResult = AMRWB_UmtsDlConvert((VOICE_WPHY_AMR_SERIAL_STRU *)pstDecPara->pshwDecSerial,
                                        &stSerial);

        enRxType = stSerial.uhwFrameType;
        enMode   = stSerial.enAmrMode;
    }
    else if (CODEC_AMR_FORMAT_BITS == pstDecPara->enAmrFormat)
    {
        enRxType    = pstDecPara->enAmrFrameType;
        enMode      = pstDecPara->enAmrMode;

        enRetResult = AMRWB_GsmDlConvert(pstDecPara->pshwDecSerial,
                                       enRxType,
                                       enMode,
                                       stSerial.ashwEncSerial);
    }
    else if (CODEC_AMR_FORMAT_IMS == pstDecPara->enAmrFormat)
    {
        /* 对ims下行数据进行重排,排列成Umts约定的格式 */
        enRetResult = AMRWB_ImsDlConvert((IMS_DL_DATA_OBJ_STRU *)pstDecPara->pshwDecSerial,&stSerial);

        enRxType = stSerial.uhwFrameType;
        enMode   = stSerial.enAmrMode;
        pstDecPara->uhwRxTypeForAjb = enRxType;

    }
    else
    {
        return CODEC_RET_ERR;
    }

    /* 声码器AMRWB初始化后收到的首个SID_FIRST/SID_UPDATE帧，需要修改为SID_BAD帧 */
    if (    (CODEC_AMRWB_TYPE_RX_SID_FIRST == enRxType)
         || (CODEC_AMRWB_TYPE_RX_SID_UPDATE== enRxType))
    {
        if (AMRWB_SID_RESET_FLG_NO == pstAmrDecObj->shwRxSidResetFlg)
        {
            enRxType                       = CODEC_AMRWB_TYPE_RX_SID_BAD;
            pstAmrDecObj->shwRxSidResetFlg = AMRWB_SID_RESET_FLG_YES;
        }
    }

    if ((CODEC_AMRWB_TYPE_RX_NO_DATA     == enRxType)
     || (CODEC_AMRWB_TYPE_RX_SPEECH_LOST == enRxType))
    {
        enMode       = pstAmrDecObj->prev_mode;
        shwResetFlag = 0;
    }
    else
    {
        pstAmrDecObj->prev_mode = enMode;
    }

    /*如果上一帧为HOMING帧，本帧进行第一子帧HOMING帧检测*/
    if ((1 == shwOldResetFlag) && (enMode < CODEC_AMRWB_MODE_MRDTX))
    {
        shwResetFlag = AMRWB_decoder_homing_frame_test_first(
                                        stSerial.ashwEncSerial,
                                        (Word16)enMode);
    }

    /*如果本帧和上一帧均为HOMING帧码流，则本帧直接置为PCM HOMING 帧*/
    if ((shwResetFlag != 0) && (shwOldResetFlag != 0))
    {
        CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_WB,
                      CODEC_EHF_MASK);
    }
    else
    {
        /* 调用解码器进行解码 */
        AMRWB_decoder((VOS_INT16)enMode,
                      stSerial.ashwEncSerial,
                      pshwDecSpeech,
                      &shwFrameLen,
                      pstAmrDecObj,
                      (VOS_INT16)enRxType);

        /* 对于宽带，16比特PCM编解码器仅取高14比特量化有效 */
        CODEC_OpVcAnd(pshwDecSpeech, (Word32)CODEC_FRAME_LENGTH_WB, CODEC_WB_PCM_MASK, pshwDecSpeech);
    }

    /* 如果上一帧不为HOMING帧，则本帧需要进行全帧HOMING帧检测 */
    if ((0 == shwOldResetFlag) && (enMode < CODEC_AMRWB_MODE_MRDTX))
    {
        shwResetFlag = AMRWB_decoder_homing_frame_test(stSerial.ashwEncSerial, (VOS_INT16)enMode);
    }

    /* 如果当前帧为HOMING帧，则对解码器进行初始化,置下行码流为Homing格式 */
    if (shwResetFlag != 0)
    {
        Reset_decoder(pstAmrDecObj, 1);

        CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_WB,
                      CODEC_EHF_MASK);
    }

    pstAmrDecObj->shwOldResetFlag = shwResetFlag;

    return enRetResult;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

