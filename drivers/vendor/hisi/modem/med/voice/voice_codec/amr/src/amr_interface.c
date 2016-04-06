

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_op_vec.h"
#include "codec_com_codec.h"
#include "amr_interface.h"
#include "amr_convert.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "amr_comm.h"
#include "e_homing.h"
#include "d_homing.h"
#include "sp_enc.h"
#include "sp_dec.h"

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
Speech_Encode_FrameState               *g_pstAmrEncObj = VOS_NULL;                  /*AMR编码器状态指针*/
Speech_Decode_FrameState               *g_pstAmrDecObj = VOS_NULL;                  /*AMR解码器状态指针*/

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 AMR_Enc_Init(
                CODEC_DTX_ENUM_UINT16 enDtxMode,
                VOS_VOID             *pfCodecCallBack)
{
    CODEC_RET_ENUM_UINT32       enEncIntRet;

    Speech_Encode_FrameState   **ppstAmrEncObj;

    /*获取全局变量AMR编码状态结构体指针的指针*/
    ppstAmrEncObj     = AMR_GetPtrOfEncObjPtr();

    /* 调用AMR声码器编码初始化函数 */
    enEncIntRet = (VOS_UINT32)Speech_Encode_Frame_init(ppstAmrEncObj, enDtxMode, "encoder");

    /* 初始化CODEC回掉函数，在完成VAD检测后直接输出帧类型给RLC层*/
    if((*ppstAmrEncObj)->cod_amr_state != VOS_NULL)
    {
        (*ppstAmrEncObj)->cod_amr_state->pfCallBack = (VOS_INT16*)pfCodecCallBack;
    }

    return enEncIntRet;
}
VOS_UINT32 AMR_Dec_Init(VOS_VOID)
{
    VOS_UINT16                  *puhwSidFirstFlag;

    CODEC_RET_ENUM_UINT32        enDecIntRet;

    Speech_Decode_FrameState   **ppstAmrDecObj;

    /*获取全局变量AMR一次通话过程中是否收到过SID_FIRST帧标志,并初始化为0*/
    puhwSidFirstFlag  = AMR_GetSidFirstFlagPtr();
    *puhwSidFirstFlag = 0;

    /*获取全局变量AMR解码状态结构体指针的指针*/
    ppstAmrDecObj     = AMR_GetPtrOfDecObjPtr();

    /* 调用AMR声码器解码初始化函数 */
    enDecIntRet = (VOS_UINT32)Speech_Decode_Frame_init(ppstAmrDecObj, "Decoder");

    return enDecIntRet;
}
VOS_UINT32 AMR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara)
{
    VOS_INT16                           *pshwSpeech;
    VOS_INT16                            shwResetFlag;

    enum Mode                            enMode     = MR475;
    enum Mode                            enUsedMode = MR475;
    enum TXFrameType                     enTxType   = TX_SPEECH_GOOD;

    AMR_ENCODED_SERIAL_STRU              stSerial;
    Speech_Encode_FrameState            *pstAmrEncObj;

    CODEC_RET_ENUM_UINT32            enRetResult = 0;

    enRetResult = CODEC_RET_OK;

    if ((VOS_NULL == pstEncInPara) || (VOS_NULL == pstEncOutPara))
    {
        return CODEC_RET_ERR;
    }

    /*获取全局变量AMR编码状态结构体指针*/
    pstAmrEncObj = AMR_GetEncObjPtr();

    pshwSpeech = pstEncInPara->pshwEncSpeech;
    /* 当AMR速率合法时，更新声码器速率信息 */
    if ( (enum Mode)pstEncInPara->enAmrMode <= MR122 )
    {
        enMode     = (enum Mode)pstEncInPara->enAmrMode;
    }

    /*检查输入PCM帧是否为homing帧*/
    shwResetFlag = encoder_homing_frame_test(pshwSpeech);

    /*调用AMR编码接口编码一帧*/
    (VOS_VOID)Speech_Encode_Frame(pstAmrEncObj,
                                  enMode,
                                  pshwSpeech,
                                  stSerial.ashwEncSerial,
                                  &enUsedMode,
                                  &enTxType);

    /*获取帧类型及编码模式*/
    stSerial.uhwFrameType = (VOS_UINT16)enTxType;

    if (enTxType != TX_NO_DATA)
    {
        stSerial.enAmrMode = (CODEC_AMR_MODE_ENUM_UINT16)enMode;
    }
    else
    {
        stSerial.enAmrMode = CODEC_AMR_MODE_MRDTX;
    }

    /*IF1 UMTS码流格式*/
    if (CODEC_AMR_FORMAT_IF1 == pstEncInPara->enAmrFormat)
    {
        enRetResult = AMR_UmtsUlConvert(&stSerial,
                                        (VOICE_WPHY_AMR_SERIAL_STRU *)pstEncOutPara->pshwEncSerial);
    }
    /*GSM码流格式*/
    else if (CODEC_AMR_FORMAT_BITS == pstEncInPara->enAmrFormat)
    {
        enRetResult = AMR_GsmUlConvert(&stSerial,
                                       pstEncOutPara->pshwEncSerial);
    }
    /*IMS码流格式*/
    else if (CODEC_AMR_FORMAT_IMS == pstEncInPara->enAmrFormat)
    {
        /* 先进行IF1 UMTS码流格式转换 */
        enRetResult = AMR_UmtsUlConvert(&stSerial,
                                        (VOICE_WPHY_AMR_SERIAL_STRU *)pstEncOutPara->pshwEncSerial);
        /* 再进行IMS码流格式转换 */
        AMR_ImsUlConvert(enTxType,enMode,pstEncOutPara->pshwEncSerial);
    }
    /*不支持*/
    else
    {
        return CODEC_RET_ERR;
    }

    if (CODEC_RET_ERR == enRetResult)
    {
        return CODEC_RET_ERR;
    }

    if (shwResetFlag != 0)
    {
        Speech_Encode_Frame_reset(pstAmrEncObj);
    }

    pstEncOutPara->enAmrFrameType = (CODEC_AMR_TYPE_TX_ENUM_UINT16)enTxType;

    /* 保存AMR编码速率 */
    pstEncOutPara->enAmrMode = enMode;

    return CODEC_RET_OK;
}


VOS_UINT32 AMR_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech)
{

    VOS_INT16                            shwResetFlag    = 0;
    VOS_INT16                            shwOldResetFlag = 0;

    AMR_ENCODED_SERIAL_STRU              stSerial;

    Speech_Decode_FrameState            *pstAmrDecObj;

    CODEC_AMR_MODE_ENUM_UINT16           enMode;
    CODEC_AMR_TYPE_RX_ENUM_UINT16        enRxType;

    CODEC_RET_ENUM_UINT32                enRetResult;

    enRetResult     = CODEC_RET_OK;

    /*获得AMR解码状态结构体全局变量指针*/
    pstAmrDecObj    = AMR_GetDecObjPtr();

    shwOldResetFlag = pstAmrDecObj->shwOldResetFlag;

    if (CODEC_AMR_FORMAT_IF1 == pstDecPara->enAmrFormat)
    {
        enRetResult = AMR_UmtsDlConvert((VOICE_WPHY_AMR_SERIAL_STRU *)pstDecPara->pshwDecSerial,
                                        &stSerial);

        enRxType = stSerial.uhwFrameType;

        enMode   = (VOS_UINT16)stSerial.enAmrMode;
        if (CODEC_AMR_MODE_MRDTX == enMode)
        {
            enMode = pstAmrDecObj->prev_mode;
        }
    }
    else if (CODEC_AMR_FORMAT_BITS == pstDecPara->enAmrFormat)
    {
        enRxType    = pstDecPara->enAmrFrameType;
        enMode      = pstDecPara->enAmrMode;

        enRetResult = AMR_GsmDlConvert(pstDecPara->pshwDecSerial,
                                       enRxType,
                                       enMode,
                                       stSerial.ashwEncSerial);
    }
    else if (CODEC_AMR_FORMAT_IMS == pstDecPara->enAmrFormat)
    {
        /* 对ims下行数据进行重排,排列成Umts约定的格式 */
        enRetResult = AMR_ImsDlConvert((IMS_DL_DATA_OBJ_STRU *)pstDecPara->pshwDecSerial,&stSerial);

        enRxType = stSerial.uhwFrameType;

        enMode   = (MED_UINT16)stSerial.enAmrMode;
        if (CODEC_AMR_MODE_MRDTX == enMode)
        {
            enMode = pstAmrDecObj->prev_mode;
        }
        pstDecPara->uhwRxTypeForAjb = enRxType;
    }
    else
    {
        return CODEC_RET_ERR;
    }

    /* 声码器AMR初始化后收到的首个SID_FIRST/SID_UPDATE帧，需要修改为SID_BAD帧，并修改标志位 */
    if (    (CODEC_AMR_TYPE_RX_SID_FIRST == enRxType)
         || (CODEC_AMR_TYPE_RX_SID_UPDATE== enRxType))
    {
        if (AMR_SID_RESET_FLG_NO == pstAmrDecObj->shwRxSidResetFlg)
        {
            enRxType                       = CODEC_AMR_TYPE_RX_SID_BAD;
            pstAmrDecObj->shwRxSidResetFlg = AMR_SID_RESET_FLG_YES;
        }
    }

    if (CODEC_AMR_TYPE_RX_NO_DATA == enRxType)
    {
        enMode = pstAmrDecObj->prev_mode;
    }
    else
    {
        pstAmrDecObj->prev_mode = (enum Mode)enMode;
    }

    /*如果上一帧为HOMING帧，本帧进行第一子帧HOMING帧检测*/
    if ((1 == shwOldResetFlag) && (enMode < CODEC_AMR_MODE_MRDTX))
    {
        shwResetFlag = decoder_homing_frame_test_first(stSerial.ashwEncSerial,
                                                       (enum Mode)enMode);
    }

    /*如果本帧和上一帧均为HOMING帧码流，则本帧直接置为PCM HOMING 帧*/
    if ((shwResetFlag != 0) && (shwOldResetFlag != 0))
    {
        CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_NB,
                      CODEC_EHF_MASK);
    }
    else
    {
        /* 调用解码器进行解码 */
        Speech_Decode_Frame(pstAmrDecObj,
                            (enum Mode)enMode,
                            stSerial.ashwEncSerial,
                            (enum RXFrameType)enRxType,
                            pshwDecSpeech);
    }

    /* 如果上一帧不为HOMING帧，则本帧需要进行全帧HOMING帧检测 */
    if ((0 == shwOldResetFlag) && (enMode < CODEC_AMR_MODE_MRDTX))
    {
        shwResetFlag = decoder_homing_frame_test(stSerial.ashwEncSerial,
                                                 (enum Mode)enMode);
    }

    /* 如果当前帧为HOMING帧，则对解码器进行初始化,置下行码流为Homing格式 */
    if (shwResetFlag != 0)
    {
        Speech_Decode_Frame_reset(pstAmrDecObj);

        CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_NB,
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

