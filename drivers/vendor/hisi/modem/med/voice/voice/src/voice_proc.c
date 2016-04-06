/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : VOICE_proc.c
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年5月10日
  最近修改   :
  功能描述   : 该文件主要完成语音软件流程控制中详细处理函数实现
  函数列表   :
              VOICE_ProcCfgGsmDlCodec
              VOICE_ProcCfgGsmUlCodec
              VOICE_ProcCfgUmtsDlCodec
              VOICE_ProcCfgUmtsUlCodec
              VOICE_ProcDecode
              VOICE_ProcDestroy
              VOICE_ProcEncode
              VOICE_ProcFrameTypeToRlc
              VOICE_ProcGetSampleRate
              VOICE_ProcInit
              VOICE_ProcInitCodec
              VOICE_ProcCheckUpdate
              VOICE_ProcOutputFrameType
              VOICE_ProcReadNv
              VOICE_ProcRxPp
              VOICE_ProcSetDevMode
              VOICE_ProcSetNv
              VOICE_ProcTxPp
              VOICE_ProcUpdate
              VOICE_ProcUpdateNv
  修改历史   :
  1.日    期   : 2011年5月10日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
   1 头文件包含
******************************************************************************/
#include "OmCodecInterface.h"
#include "om.h"

#include "ucom_comm.h"
#include "ucom_nv.h"

#include "codec_op_vec.h"

#include "voice_proc.h"
#include "voice_amr_mode.h"
#include "voice_pcm.h"
#include "voice_mc.h"
#include "voice_debug.h"
#include "med_gain.h"
#include "ucom_pcm.h"
#include "mlib_interface.h"
#include "voice_diagnose.h"
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
#define THIS_FILE_ID                OM_FILE_ID_VOICE_PROC_C

/*****************************************************************************
   2 全局变量定义
******************************************************************************/
/* 语音码流结构体全局变量，该全局变量需要定义在Hifi的TCM中 */
UCOM_ALIGN(4)
UCOM_SEC_TCMBSS
VOICE_PROC_CODEC_DATA_STRU           g_stVoiceCodecData;

/* PROC模块控制全局变量 */
UCOM_ALIGN(4)
UCOM_SEC_TCMBSS
VOICE_PROC_OBJS_STRU                 g_stVoiceProcObjs = {0};

VOICE_DEC_SERIAL_STRU               *g_pstDecSerialWrite = &g_stVoiceCodecData.astDecSerial[0];

/* 声码器与采样率的映射表 */
VOS_UINT32  g_auwCodecToSampleRateTbl[CODEC_BUTT][2]
    = {{CODEC_AMR,      CODEC_SAMPLE_RATE_MODE_8000},
       {CODEC_EFR,      CODEC_SAMPLE_RATE_MODE_8000},
       {CODEC_FR,       CODEC_SAMPLE_RATE_MODE_8000},
       {CODEC_HR,       CODEC_SAMPLE_RATE_MODE_8000},
       {CODEC_AMR2,     CODEC_SAMPLE_RATE_MODE_8000},
       {CODEC_AMRWB,    CODEC_SAMPLE_RATE_MODE_16000},
       {CODEC_G711,     CODEC_SAMPLE_RATE_MODE_16000}};

/*****************************************************************************
   3 外部函数声明
******************************************************************************/

/*****************************************************************************
   4 函数实现
******************************************************************************/

VOS_UINT32 VOICE_ProcInit(VOS_VOID)
{
    VOS_INT16                     *pshwEncSerial = VOICE_ProcGetEncBufPtr();
    VOICE_DEC_SERIAL_STRU         *pstDecSerial  = VOICE_ProcGetDecBufPtr();
    VOS_INT16                     *pshwLineOut   = VOICE_ProcGetLineOutBufPtr();

    /* 初始化编码码流buff */
    UCOM_MemSet(pshwEncSerial,
                0,
                (VOICE_CODED_FRAME_WITH_OBJ_LEN  * sizeof(VOS_UINT16)));

    /* 初始化解码码流buff */
    UCOM_MemSet(pstDecSerial,
                0,
                VOICE_CODED_DECODE_BUF_FRAME_NUM * sizeof(VOICE_DEC_SERIAL_STRU));

    /* 初始化编码前PCM码流buff */
    UCOM_MemSet(pshwLineOut,
                0,
                ((VOS_UINT32)(VOS_INT32)CODEC_PCM_MAX_FRAME_LENGTH * sizeof(VOS_UINT16)));

    /* 声学处理初始化 */
    MLIB_PathInit(MLIB_PATH_CS_VOICE_CALL_MICIN);
    MLIB_PathInit(MLIB_PATH_CS_VOICE_CALL_SPKOUT);

    return UCOM_RET_SUCC;

}


VOS_UINT32 VOICE_ProcUpdate(VOS_UINT16 uhwDevMode,VOS_UINT16 uhwNetMode)
{
    VOS_UINT32                          uwRet;
    MLIB_PARA_STRU                      stPara;

    UCOM_MemSet(&stPara, 0, sizeof(MLIB_PARA_STRU));

    /* 麦克风通路参数设置 */
    stPara.uwChnNum     = 2;
    stPara.uwSampleRate = VOICE_ProcGetSampleRate();
    stPara.uwFrameLen   = ( (stPara.uwSampleRate == 16000) ? CODEC_FRAME_LENGTH_WB: CODEC_FRAME_LENGTH_NB);
    stPara.uwResolution = 16;
    stPara.enDevice     = uhwDevMode;
    stPara.enParaSource = MLIB_PARA_SOURCE_NV;
    stPara.uwDataSize   = 0;

    uwRet = MLIB_PathSet(MLIB_PATH_CS_VOICE_CALL_MICIN,
                 &stPara,
                 sizeof(MLIB_PARA_STRU));

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError1(VOICE_SetNvError, uwRet);
    }

    /* 扬声器通路参数设置 */
    stPara.uwChnNum     = 2;

    uwRet = MLIB_PathSet(MLIB_PATH_CS_VOICE_CALL_SPKOUT,
                 &stPara,
                 sizeof(MLIB_PARA_STRU));

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError1(VOICE_SetNvError, uwRet);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcInitEncoder(VOS_UINT16 usType, VOS_UINT16 usDtxMode)
{
    VOS_UINT32               uwInitRet        = UCOM_RET_SUCC;
    VOICE_PROC_CODEC_STRU   *pstCodec         = VOICE_ProcGetCodecPtr();
    VOS_UINT16               uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16               uhwNetMode       = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);


    /* 声码器编码器刚被初始化时上行需要发送HOMING帧,让对端解码声码器同时进行初始化 */
    CODEC_OpVecSet((pstCodec->stEncInObj).pshwEncSpeech,
                   CODEC_PCM_FRAME_LENGTH,
                   VOICE_PROC_HOMING_MASK);

    switch(usType)
    {
        case CODEC_AMR:
        {
            if(CODEC_NET_MODE_W == uhwNetMode)
            {
                uwInitRet = AMR_Enc_Init(usDtxMode,VOICE_ProcOutputFrameType);
            }
            else
            {
                uwInitRet = AMR_Enc_Init(usDtxMode,MED_NULL);
            }
            OM_LogInfo1(VOICE_ProcInitCodec_AmrEncInit_Done, uwInitRet);
            break;
        }
        case CODEC_EFR:
        {
            uwInitRet = EFR_Enc_Init(usDtxMode);
            OM_LogInfo1(VOICE_ProcInitCodec_EfrEncInit_Done, uwInitRet);
            break;
        }
        case CODEC_FR:
        {
            uwInitRet = FR_Enc_Init(usDtxMode);
            OM_LogInfo1(VOICE_ProcInitCodec_FrEncInit_Done, uwInitRet);
            break;
        }
        case CODEC_HR:
        {
            uwInitRet = HR_Enc_Init(usDtxMode);
            OM_LogInfo1(VOICE_ProcInitCodec_HrEncInit_Done, uwInitRet);
            break;
        }
        case CODEC_AMRWB:
        {
            if (CODEC_NET_MODE_W == uhwNetMode)
            {
                uwInitRet = AMRWB_Enc_Init(usDtxMode, VOICE_ProcOutputFrameType);
            }
            else
            {
                uwInitRet = AMRWB_Enc_Init(usDtxMode, MED_NULL);
            }
            OM_LogInfo1(VOICE_ProcInitCodec_AmrWbEncInit_Done, uwInitRet);
            break;
        }

        default:
        {
            OM_LogError(VOICE_ProcInitCodec_UnkownEncoder);
            break;
        }
    }

    return uwInitRet;
}
VOS_UINT32 VOICE_ProcInitDecoder(VOS_UINT16 usType)
{
    VOS_UINT32                      uwInitRet = UCOM_RET_SUCC;
    VOICE_PROC_CODEC_STRU          *pstCodec  = VOICE_ProcGetCodecPtr();

    /* 下行相对上行丢帧检测计数置0 */
    VOICE_SetRxLostFrameCnt(0);

    /* 解码前一帧类型置为无前一帧,BFI计数值清0,前一帧结构体清0 */
    pstCodec->stDecInObj.enGoodFrameType = CODEC_PREFRAME_NONE;
    pstCodec->stDecInObj.uhwFrameBfiCnt  = 0;

    UCOM_MemSet(VOICE_ProcGetPreDecInObj(),
                0,
                sizeof(CODEC_DEC_PREIN_PARA_STRU));

    /* 初始化前一帧结构体中AMR帧类型和置为BFI帧 */
    pstCodec->stPreDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_NO_DATA;
    pstCodec->stPreDecInObj.enBfiFlag      = CODEC_BFI_YES;

    switch(usType)
    {
        case CODEC_AMR:
        {
            uwInitRet = AMR_Dec_Init();
            OM_LogInfo1(VOICE_ProcInitCodec_AmrDecInit_Done, uwInitRet);
            break;
        }
        case CODEC_EFR:
        {
            uwInitRet = EFR_Dec_Init();
            OM_LogInfo1(VOICE_ProcInitCodec_EfrDecInit_Done, uwInitRet);
            break;
        }
        case CODEC_FR:
        {
            uwInitRet = FR_Dec_Init();
            OM_LogInfo1(VOICE_ProcInitCodec_FrDecInit_Done, uwInitRet);
            break;
        }
        case CODEC_HR:
        {
            uwInitRet = HR_Dec_Init();
            OM_LogInfo1(VOICE_ProcInitCodec_HrDecInit_Done, uwInitRet);
            break;
        }
        case CODEC_AMRWB:
        {
            uwInitRet = AMRWB_Dec_Init();
            OM_LogInfo1(VOICE_ProcInitCodec_AmrWbDecInit_Done, uwInitRet);
            break;
        }

        default:
        {
            OM_LogError(VOICE_ProcInitCodec_UnkownDecoder);
            break;
        }
    }

    return uwInitRet;
}
VOS_UINT32 VOICE_ProcDestroy(VOICE_PROC_OBJS_STRU *pstProcObj)
{
    MLIB_PathClear(MLIB_PATH_CS_VOICE_CALL_MICIN);
    MLIB_PathClear(MLIB_PATH_CS_VOICE_CALL_SPKOUT);

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcGetSampleRate(VOS_VOID)
{
    VOICE_PROC_CODEC_STRU  *pstCodec = VOICE_ProcGetCodecPtr();
    VOS_UINT32              uwCodecType;

    /* 获取编解码器类型(采用32bit,提高数组索引效率) */
    if (VOS_NULL == pstCodec)
    {
        /* pstCodec未被赋值 */
        OM_LogError(VOICE_ProcCodecPtrIsNULLError);
        /* 返回默认采样率 */
        return CODEC_SAMPLE_RATE_MODE_8000;

    }
    uwCodecType = pstCodec->enCodecType;

    /* 检查编解码器类型是否正确 */
    if (g_auwCodecToSampleRateTbl[uwCodecType][0] == uwCodecType)
    {
        /* 返回采样率 */
        return g_auwCodecToSampleRateTbl[uwCodecType][1];

    }
    else
    {
        /* 编解码类型枚举与映射表定义的顺序不一致 */
        OM_LogError(VOICE_ProcCodecToSampleRateMapError);

        /* 返回默认采样率 */
        return CODEC_SAMPLE_RATE_MODE_8000;
    }

}
VOS_UINT32 VOICE_ProcCfgGsmDlCodec(GPHY_VOICE_RX_DATA_IND_STRU *pstRxDataMsg)
{
    VOICE_PROC_CODEC_STRU      *pstCodec    = VOICE_ProcGetCodecPtr();
    CODEC_DEC_IN_PARA_STRU     *pstDecInObj = &pstCodec->stDecInObj;

    /* 进行参数合法性检查，异常时返回参数错误 */
    if (pstRxDataMsg->enCodecType >= CODEC_BUTT)
    {
        return UCOM_RET_ERR_PARA;
    }

    /* AMR参数检查，仅在声码器为AMR时进行，异常时返回参数错误 */
    if (CODEC_AMR == pstRxDataMsg->enCodecType)
    {
        /* AMR */
        if (pstRxDataMsg->enAmrMode >= (VOS_UINT16)CODEC_AMR_RATE_MODE_BUTT)
        {
            return UCOM_RET_ERR_PARA;
        }
    }
    /* AMR-WB参数检查，仅在声码器为AMR-WB时进行，异常时返回参数错误 */
    else if (CODEC_AMRWB == pstRxDataMsg->enCodecType)
    {
        /* AMR-WB */
        if (pstRxDataMsg->enAmrMode > (VOS_UINT16)CODEC_AMRWB_RATE_MODE_1265K)
        {
            return UCOM_RET_ERR_PARA;
        }
    }
    else
    {
        /* 非AMR下检查BFI/SID/TAF/UFI等参数 */
        if (   (pstRxDataMsg->enBfi >= CODEC_BFI_BUTT)
            || (pstRxDataMsg->enSid >= CODEC_SID_BUTT)
            || (pstRxDataMsg->enTaf >= CODEC_TAF_BUTT)
            || (pstRxDataMsg->enUfi >= CODEC_BFI_BUTT))
        {
            return UCOM_RET_ERR_PARA;
        }
    }

    /* 接收解码输入参数,包括BFI/SID/TAF/UFI/AmrMode/AmrFrameType,并将帧格式标志为GSM下的AMR语音帧 */
    pstDecInObj->enBfiFlag      = pstRxDataMsg->enBfi;
    pstDecInObj->enSidFlag      = pstRxDataMsg->enSid;
    pstDecInObj->enTafFlag      = pstRxDataMsg->enTaf;
    pstDecInObj->enHrUfiFlag    = pstRxDataMsg->enUfi;
    pstDecInObj->enAmrMode      = pstRxDataMsg->enAmrMode;
    pstDecInObj->enAmrFrameType = pstRxDataMsg->enAmrFrameType;
    pstDecInObj->enAmrFormat    = CODEC_AMR_FORMAT_BITS; /* 将帧格式标志为GSM下的AMR语音帧 */

    /* 下行偷帧检测 */
    if (CODEC_FRAME_FACCH_YES == pstRxDataMsg->enIsFrameFacch)
    {
        VOICE_ProcFacchFrameCntAdd();
        OM_LogInfo1(VOICE_ProcGsmRxFacch, VOICE_ProcGetFacchFrameCnt());
    }
    else
    {
        VOICE_ProcFacchFrameCntReset();
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_ProcCfgGsmUlCodec(GPHY_VOICE_UPDATE_PARA_CMD_STRU *pstUpdateMsg, VOS_UINT16 uhwModemNo)
{
    VOICE_PROC_CODEC_STRU       *pstCodec    = VOICE_McGetModemCodecPtr(uhwModemNo);
    CODEC_ENC_IN_PARA_STRU      *pstEncInObj = &pstCodec->stEncInObj;

    /* 进行参数合法性检查，异常时返回参数错误 */
    if ((pstUpdateMsg->enCodecType >= CODEC_BUTT)
        || (pstUpdateMsg->enDtxMode >= CODEC_SWITCH_BUTT)
        || (pstUpdateMsg->enAmrNsync >=CODEC_AMR_NSYNC_BUTT))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* AMRNB参数检查，仅在声码器为AMR时进行，异常时返回参数错误 */
    if ((CODEC_AMR == pstUpdateMsg->enCodecType)
        && (pstUpdateMsg->enAmrMode >= (VOS_UINT16)CODEC_AMR_RATE_MODE_BUTT))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* AMR-WB参数检查，仅在声码器为AMR时进行，异常时返回参数错误 */
    if ((CODEC_AMRWB == pstUpdateMsg->enCodecType)
        && (pstUpdateMsg->enAmrMode > (VOS_UINT16)CODEC_AMRWB_RATE_MODE_1265K))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* 若声码器DTX模式需要更新则需要同时重新初始化声码器 */
    if ((VOS_INT16)pstUpdateMsg->enDtxMode != pstEncInObj->enDtxMode)
    {
        pstCodec->usIsEncInited = VOICE_PROC_IS_NO;
        pstEncInObj->enDtxMode  = pstUpdateMsg->enDtxMode;
    }

    /* 接收其他编码输入参数，并将帧格式标志为GSM下的AMR语音帧 */
    if (   (CODEC_AMR == pstUpdateMsg->enCodecType)
        || (CODEC_AMRWB == pstUpdateMsg->enCodecType))
    {
        pstEncInObj->enAmrMode      = pstUpdateMsg->enAmrMode;
        pstEncInObj->enAmrNsync     = pstUpdateMsg->enAmrNsync;
    }

    pstEncInObj->enAmrFormat    = CODEC_AMR_FORMAT_BITS;

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_ProcCfgImsCodec(VOS_UINT16 uhwCodecType,
                                         VOS_UINT16 uhwDtxEnable,
                                         VOS_UINT16 uhwRateMode)
{
    VOICE_PROC_CODEC_STRU       *pstCodec    = VOICE_ProcGetCodecPtr();

    /* 声码器合法性检测 */
    if (uhwCodecType != pstCodec->enCodecType)
    {
        return UCOM_RET_FAIL;
    }

    if(CODEC_G711 != pstCodec->enCodecType)
    {
        /* DTX类型检查,若DTX类型更新则需要同时重新初始化声码器 */
        if (pstCodec->stEncInObj.enDtxMode != uhwDtxEnable)
        {
            pstCodec->usIsEncInited         = VOICE_PROC_IS_NO;
            pstCodec->stEncInObj.enDtxMode  = uhwDtxEnable;
        }

        /* 配置速率模式 */
        pstCodec->stEncInObj.enAmrMode      = uhwRateMode;

        /* 码流格式标志为IMS下语音帧格式 */
        pstCodec->stEncInObj.enAmrFormat    = CODEC_AMR_FORMAT_IMS;
        pstCodec->stDecInObj.enAmrFormat    = CODEC_AMR_FORMAT_IMS;
    }

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : VOICE_ProcEncode
 功能描述  : 根据声码器类型调用相应的编码器进行编码处理
 输入参数  : psPcm       - 待编码PCM数据
 输出参数  : psStrm      - 编码输出码流
 返 回 值  : VOS_UINT32  -具体值及范围详见VOICE_RET_ENUM语音返回值定义
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月1日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数头注释模板

*****************************************************************************/
VOS_UINT32 VOICE_ProcEncode(VOS_INT16 *psPcm, VOS_INT16 *psStrm)
{
    VOICE_PROC_CODEC_STRU          *pstCodec    = VOICE_ProcGetCodecPtr();
    VOS_UINT32                      uwRet       = UCOM_RET_SUCC;
    IMS_UL_DATA_OBJ_STRU           *pstImsUlObj;
    CODEC_ENC_IN_PARA_STRU         *pstEncInPara;
    CODEC_ENC_OUT_PARA_STRU        *pstEncOutPara;

    /* 参数合法性检查 */
    if ((VOS_NULL == psPcm) || (VOS_NULL == psStrm))
    {
        return UCOM_RET_NUL_PTR;
    }

    pstCodec->stEncInObj.pshwEncSpeech  = psPcm;
    pstCodec->stEncOutObj.pshwEncSerial = psStrm;

    /* 下行相对上行丢帧检测计数加1 */
    VOICE_AddRxLostFrameCnt(1);

    //若声码器未初始化则初始化声码器
    if (VOICE_PROC_IS_YES != pstCodec->usIsEncInited)
    {
        if (UCOM_RET_SUCC == VOICE_ProcInitEncoder(pstCodec->enCodecType,
            pstCodec->stEncInObj.enDtxMode))
        {
            pstCodec->usIsEncInited = VOICE_PROC_IS_YES;
        }
        else
        {
            return UCOM_RET_FAIL;
        }
    }

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_ENCODE);

    /* 根据声码器类型调用相应的编码器进行编码处理 */
    switch(pstCodec->enCodecType)
    {
        case CODEC_AMR:
        {
            uwRet = AMR_Encode(&(pstCodec->stEncInObj), &(pstCodec->stEncOutObj));
            break;
        }
        case CODEC_EFR:
        {
            uwRet = EFR_Encode(&(pstCodec->stEncInObj), &(pstCodec->stEncOutObj));
            break;
        }
        case CODEC_FR:
        {
            uwRet = FR_Encode(&(pstCodec->stEncInObj), &(pstCodec->stEncOutObj));
            break;
        }
        case CODEC_HR:
        {
            uwRet = HR_Encode(&(pstCodec->stEncInObj), &(pstCodec->stEncOutObj));
            break;
        }
        case CODEC_AMRWB:
        {
            uwRet = AMRWB_Encode(&(pstCodec->stEncInObj), &(pstCodec->stEncOutObj));
            break;
        }
        case CODEC_G711:
        {
            /* G711不需要编码,直接上传PCM数据 */
            pstEncInPara  = &(pstCodec->stEncInObj);
            pstEncOutPara = &(pstCodec->stEncOutObj);
            pstImsUlObj   = (IMS_UL_DATA_OBJ_STRU*)pstEncOutPara->pshwEncSerial;
            pstImsUlObj->usStatus    = 0;
            pstImsUlObj->usFrameType = 0;
            UCOM_MemCpy(&(pstImsUlObj->ausData[0]),pstEncInPara->pshwEncSpeech,IMSA_CODEC_MAX_DATA_LEN*sizeof(MED_UINT16));
            break;
        }
        default:
        {
            /* 内部保证不会进入本分支 */
            return UCOM_RET_ERR_PARA;
        }
    }

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_ENCODE);

    return uwRet;
}

/*****************************************************************************
 函 数 名  : VOICE_ProcDecode
 功能描述  : 根据声码器类型调用相应的解码器进行解码处理
 输入参数  : psStrm      - 待解码码流
 输出参数  : psPcm       - 解码输出PCM数据
 返 回 值  : VOS_UINT32  -具体值及范围详见VOICE_RET_ENUM语音返回值定义
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月1日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数头注释模板

*****************************************************************************/
VOS_UINT32 VOICE_ProcDecode(VOS_INT16 *psStrm, VOS_INT16 *psPcm)
{
    VOICE_PROC_CODEC_STRU          *pstCodec         = VOICE_ProcGetCodecPtr();
    VOS_UINT32                      uwRet            = UCOM_RET_SUCC;
    VOS_INT16                       shwPosTmp;
    VOS_UINT16                      uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                      uhwNetMode       = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    IMS_DL_DATA_OBJ_STRU           *pstImsDlObj;
    CODEC_DEC_IN_PARA_STRU         *pstDecInPara;
    /* 参数合法性检查 */
    if ((VOS_NULL == psPcm) || (VOS_NULL == psStrm))
    {
        return UCOM_RET_NUL_PTR;
    }

    pstCodec->stDecInObj.pshwDecSerial = (VOS_INT16*)psStrm;

    /* 若声码器未初始化则初始化声码器 */
    if ((  VOICE_GetRxLostFrameCnt() > VOICE_RX_LOST_FRAME_THD)
        ||(VOICE_PROC_IS_YES != pstCodec->usIsDecInited))
    {
        if (UCOM_RET_SUCC == VOICE_ProcInitDecoder(pstCodec->enCodecType))
        {
            pstCodec->usIsDecInited = VOICE_PROC_IS_YES;
        }
        else
        {
            return UCOM_RET_FAIL;
        }
    }

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_DECODE);

    /* GSM下需要进行坏帧处理和偷帧处理 */
    if(CODEC_NET_MODE_G == uhwNetMode)
    {
        /* BFI处理 */
        uwRet = VOICE_ProcBFI(pstCodec);

        /* 若BFI处理为置0，则不作解码直接返回 */
        if (UCOM_RET_ERR_STATE == uwRet)
        {
            UCOM_MemSet(psPcm, 0, (VOS_UINT32)((VOS_UINT16)CODEC_PCM_FRAME_LENGTH * sizeof(VOS_INT16)));

            return UCOM_RET_SUCC;
        }

        /* GSM下偷帧问题处理 */
        uwRet = VOICE_ProcFacch(psPcm);

        /* 连续偷帧，不进行解码，直接返回成功 */
        if(UCOM_RET_ERR_STATE == uwRet)
        {
            return UCOM_RET_SUCC;
        }
    }

    /* 根据声码器类型调用相应的解码器进行解码处理 */
    switch(pstCodec->enCodecType)
    {
        case CODEC_AMR:
        {
            uwRet = AMR_Decode(&(pstCodec->stDecInObj), psPcm);
            break;
        }
        case CODEC_EFR:
        {
            uwRet = EFR_Decode(&(pstCodec->stDecInObj), psPcm);
            break;
        }
        case CODEC_FR:
        {
            uwRet = FR_Decode(&(pstCodec->stDecInObj), psPcm);
            break;
        }
        case CODEC_HR:
        {
            uwRet = HR_Decode(&(pstCodec->stDecInObj), psPcm);
            break;
        }
        case CODEC_AMRWB:
        {
            uwRet = AMRWB_Decode(&(pstCodec->stDecInObj), psPcm);
            break;
        }
        case CODEC_G711:
        {
            /* G711不需要解码,直接填充lineIn */
            pstDecInPara = &(pstCodec->stDecInObj);
            pstImsDlObj = (IMS_DL_DATA_OBJ_STRU *)pstDecInPara->pshwDecSerial;

            /* error或者bad帧，填充0 */
            if ((IMSA_VOICE_ERROR == (pstImsDlObj->usErrorFlag)) || (CODEC_AMR_FQI_QUALITY_BAD == (pstImsDlObj->usQualityIdx)))
            {
                UCOM_MemSet(psPcm,0,CODEC_PCM_MAX_FRAME_LENGTH);
            }
            else
            {
                UCOM_MemCpy(psPcm,
                            &(pstImsDlObj->ausData[0]),
                            (IMSA_CODEC_MAX_DATA_LEN * sizeof(MED_UINT16)));
            }

            break;
        }
        default:
        {
            /* 内部保证不会进入本分支 */
            return UCOM_RET_ERR_PARA;
        }
    } //end for switch

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_DECODE);

    VOICE_SetRxLostFrameCnt(0);

    /* 保存本帧时域信号幅度的最大绝对值 */
    VOICE_ProcSetMaxFrameRange(CODEC_OpVecMaxAbs(psPcm, CODEC_PCM_FRAME_LENGTH, &shwPosTmp));

    return uwRet;
}
#if 0
/*****************************************************************************
 函 数 名  : VOICE_ProcConfigSound
 功能描述  : 配置SOUND合成器
 输入参数  : usEnable    - SOUND合成器使能, CODEC_SWITCH_ON/CODEC_SWITCH_OFF
             pstSndCfg   - SOUND合成器配置参数, usEnable为CODEC_SWITCH_OFF忽视本参数
 输出参数  : 无
 返 回 值  : VOS_UINT32  -具体值及范围详见VOICE_RET_ENUM语音返回值定义
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月1日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数头注释模板

*****************************************************************************/
VOS_UINT32 VOICE_ProcConfigSound(
                VOS_UINT16                                  enEnable,
                VOICE_PROC_SND_CFG_STRU                 *pstSndCfg)
{
    VOS_UINT32                          uwRet      = UCOM_RET_SUCC;
    VOICE_PROC_SOUND_STRU           *pstSound   = &g_stVoiceProcObjs.stSound;

    if ( CODEC_SWITCH_ON == enEnable )
    {
        if ( VOS_NULL == pstSndCfg )
        {
            return UCOM_RET_NUL_PTR;
        }

        if ( VOS_NULL == pstSndCfg->funcSndInd )
        {
            return UCOM_RET_ERR_PARA;
        }

        //设置Sound使能标志
        switch (pstSndCfg->enTxRx)
        {
            case VOICE_TXRX_TX:
            {
                pstSound->enTxEnable = CODEC_SWITCH_ON;
                pstSound->enRxEnable = CODEC_SWITCH_OFF;
                break;
            }
            case VOICE_TXRX_RX:
            {
                pstSound->enTxEnable = CODEC_SWITCH_OFF;
                pstSound->enRxEnable = CODEC_SWITCH_ON;
                break;
            }
            case VOICE_TXRX_TXRX:
            {
                pstSound->enTxEnable = CODEC_SWITCH_ON;
                pstSound->enRxEnable = CODEC_SWITCH_ON;
                break;
            }
            default:
            {
                return UCOM_RET_ERR_PARA;
            }
        }

        //更新音量值
        pstSound->sSndTxVol = pstSndCfg->sTxVolume;
        pstSound->sSndRxVol = pstSndCfg->sRxVolume;

        //设置回调函数
        pstSound->funcSndInd = pstSndCfg->funcSndInd;

        //复位预停止标志
        pstSound->usPreDisable = CODEC_SWITCH_OFF;

        //初始化Sound合成器
        #if (_FEATURE_VOICE_SOUND == FEATURE_ON)
        uwRet = VoiceCodecSoundInit(pstSndCfg->usSndId, pstSndCfg->usRptCnt);
        #endif

    }
    else if ( CODEC_SWITCH_OFF == enEnable )
    {
        if ( (CODEC_SWITCH_ON == pstSound->enRxEnable)
            || (CODEC_SWITCH_ON == pstSound->enTxEnable) )
        {
            //设置Sound使能标志
            pstSound->usPreDisable = CODEC_SWITCH_ON;
        }
    }
    else
    {
        //传入参数错误
        return UCOM_RET_ERR_PARA;
    }

    if (UCOM_RET_SUCC == uwRet)
    {
        OM_LogInfo(VOICE_ProcConfigSound_Ok);
    }

    return uwRet;
}

/*****************************************************************************
 函 数 名  : VOICE_ProcGenerateSound
 功能描述  : 合成SOUND数据
 输入参数  : psSound - SOUND合成数据输出缓冲区
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月1日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数头注释模板

*****************************************************************************/
VOS_VOID VOICE_ProcGenerateSound(VOS_INT16 *psSound)
{
    VOS_UINT32                uwRet    = 0;
    VOICE_PROC_SOUND_STRU *pstSound = &g_stVoiceProcObjs.stSound;

    #if (_FEATURE_VOICE_SOUND == FEATURE_ON)
    uwRet = VoiceCodecSoundPlay(psSound);
    #endif
    if ( (UCOM_RET_SUCC != uwRet) || (CODEC_SWITCH_ON == pstSound->usPreDisable) )
    {
        //更新合成器使能
        pstSound->enTxEnable = CODEC_SWITCH_OFF;
        pstSound->enRxEnable = CODEC_SWITCH_OFF;

        //预停止标志复位
        pstSound->usPreDisable = CODEC_SWITCH_OFF;

        //上报播放结束
        if ( pstSound->funcSndInd != VOS_NULL )
        {
            pstSound->funcSndInd();
        }

        //上报日志
        OM_LogInfo(VOICE_ProcGenerateSound_SoundEnd);
    }
}
#endif


VOS_INT16* VOICE_ProcGetEcRef( VOS_VOID )
{
    VOS_INT16      *pshwEcRef;
    VOS_UINT32      uwSpaI2SVirAddr;

    /* 确定EC REF的指针 */
    if (   (VCVOICE_DEV_MODE_HANDFREE == VOICE_McGetDeviceMode())
        && (VOICE_SMART_PA_EN_ENABLE  == VOICE_McGetSmartPaEn()))
    {
        uwSpaI2SVirAddr = UCOM_PCM_SmartPaGetAvailVirtAddr();

        VOICE_PcmHybrid2Stereo((VOS_INT16 *)uwSpaI2SVirAddr,
                               VOICE_PcmGetEcRefLeftChanPtr(),
                               VOICE_PcmGetEcRefRightChanPtr(),
                               CODEC_PCM_FRAME_LENGTH);

        /* 判断输入I2S数据，左声道有效还是右声道的数据有效 */
        if (VOICE_SMART_PA_L_CHAN_VALID == VOICE_McGetSmartPaValidChan())
        {
            pshwEcRef = VOICE_PcmGetEcRefLeftChanPtr();
        }
        else
        {
            pshwEcRef = VOICE_PcmGetEcRefRightChanPtr();
        }

        OM_LogInfo(VOICE_ProcGetEcRef_GetSmartPADataOk);
    }
    else
    {
        /* 获取下行声学处理以后的数据 */
        pshwEcRef = VOICE_PcmGetSpkOutBufTempPtr();
    }

    return pshwEcRef;

}
VOS_UINT32 VOICE_ProcTxPp(
                VOS_INT16   *pshwMcIn,
                VOS_INT16   *pshwRcIn,
                VOS_INT16   *pshwLineOut)
{
    /* 获取单双麦克标志 */
    VOS_UINT32                          uwRet;
    VOICE_MC_MODEM_STRU                *pstForeGroundObj = VOICE_McGetForeGroundObj();
    VOS_INT16                          *pshwEcRef;
    MLIB_SOURCE_STRU                    stSourceIn, stSourceOut;
    VOS_UINT32                          uwSampleRate;
    VOS_UINT32                          uwFrameSize;

    /* 入参判断 */
    if (   (VOS_NULL == pshwMcIn)
        || (VOS_NULL == pshwRcIn)
        || (VOS_NULL == pshwLineOut)
        || (VOS_NULL == pstForeGroundObj))
    {
        return UCOM_RET_NUL_PTR;
    }

    /* 将输入输出信息清0 */
    UCOM_MemSet(&stSourceIn,  0, sizeof(MLIB_SOURCE_STRU));
    UCOM_MemSet(&stSourceOut, 0, sizeof(MLIB_SOURCE_STRU));

    pshwEcRef       = VOICE_ProcGetEcRef();
    uwSampleRate    = VOICE_ProcGetSampleRate();
    uwFrameSize     = (VOS_UINT32)((VOS_UINT16)(CODEC_PcmGetPcmFrameLength())*sizeof(VOS_INT16));

    /* 设置输入\输出数据 */
    stSourceIn.uwChnNum = 3;
    MLIB_UtilitySetSource(&stSourceIn,  0, pshwMcIn,  uwFrameSize,  uwSampleRate);
    MLIB_UtilitySetSource(&stSourceIn,  1, pshwRcIn,  uwFrameSize,  uwSampleRate);
    MLIB_UtilitySetSource(&stSourceIn,  2, pshwEcRef, uwFrameSize,  uwSampleRate);

    stSourceOut.uwChnNum = 3;
    MLIB_UtilitySetSource(&stSourceOut, 0, pshwLineOut, uwFrameSize, uwSampleRate);
    MLIB_UtilitySetSource(&stSourceOut, 1, pshwRcIn,  uwFrameSize,  uwSampleRate);
    MLIB_UtilitySetSource(&stSourceOut, 2, pshwEcRef, uwFrameSize,  uwSampleRate);

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_PROC_MICIN);

    /* 调用声学处理 */
    uwRet = MLIB_PathProc(
        MLIB_PATH_CS_VOICE_CALL_MICIN,
        &stSourceOut,
        &stSourceIn);

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_PROC_MICIN);

    return uwRet;

}
VOS_UINT32 VOICE_ProcRxPp(
                VOS_INT16   *pshwLineIn,
                VOS_INT16   *pshwSpkOut)
{
    VOS_UINT32                          uwRet          = 0;
    VOICE_MC_MODEM_STRU                *pstForeGroundObj = VOICE_McGetForeGroundObj();    /* 入参判断 */
    MLIB_SOURCE_STRU                    stSourceIn, stSourceOut;
    VOS_UINT32                          uwSampleRate;
    VOS_UINT32                          uwFrameSize;

    if (   (VOS_NULL == pshwSpkOut)
        || (VOS_NULL == pshwLineIn)
        || (VOS_NULL == pstForeGroundObj))
    {
        return UCOM_RET_NUL_PTR;
    }

    /* 将输入输出信息清0 */
    UCOM_MemSet(&stSourceIn,  0, sizeof(MLIB_SOURCE_STRU));
    UCOM_MemSet(&stSourceOut, 0, sizeof(MLIB_SOURCE_STRU));

    uwSampleRate    = VOICE_ProcGetSampleRate();
    uwFrameSize     = (VOS_UINT32)((VOS_UINT16)(CODEC_PcmGetPcmFrameLength())*sizeof(VOS_INT16));

    /* 设置输入\输出数据 */
    stSourceIn.uwChnNum = 1;
    MLIB_UtilitySetSource(&stSourceIn,  0, pshwLineIn, uwFrameSize,  uwSampleRate);

    stSourceOut.uwChnNum = 1;
    MLIB_UtilitySetSource(&stSourceOut, 0, pshwSpkOut, uwFrameSize, uwSampleRate);

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_PROC_SPKOUT);

    /* 调用声学处理 */
    uwRet = MLIB_PathProc(
        MLIB_PATH_CS_VOICE_CALL_SPKOUT,
        &stSourceOut,
        &stSourceIn);

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_PROC_SPKOUT);

    return uwRet;
}


VOS_VOID VOICE_ProcInitDecSerialWrite( VOS_VOID )
{
    VOICE_DEC_SERIAL_STRU       *pstDecBuf       = VOICE_ProcGetDecBufPtr();

    /* 清空解码缓存中的数据 */
    UCOM_MemSet(pstDecBuf, 0, VOICE_CODED_DECODE_BUF_FRAME_NUM * sizeof(VOICE_DEC_SERIAL_STRU));

    /* 初始化解码缓存写指针 */
    VOICE_ProcInitDecSerialWritePtr();
}


VOS_VOID VOICE_ProcSubDecSerialWrite( VOS_UINT16 uhwRxFrmLost)
{
    VOICE_DEC_SERIAL_STRU          *pstDecBuf       = VOICE_ProcGetDecBufPtr();
    VOICE_DEC_SERIAL_STRU          *pstDecSerialWrite;

    /* 下行没有丢帧 */
    if(VOICE_PROC_IS_NO == uhwRxFrmLost)
    {
        /* 剩余数据移动到buff首地址 */
        VOICE_ProcSubDecSerialWritePtr();

        pstDecSerialWrite = VOICE_ProcGetDecSerialWritePtr();

        if(pstDecSerialWrite != pstDecBuf)
        {
            UCOM_MemCpy(pstDecBuf,
                        pstDecSerialWrite,
                        sizeof(VOICE_DEC_SERIAL_STRU));
        }

    }
}


VOS_VOID VOICE_ProcAddDecSerialWrite(VOS_VOID *pvMsg, VOS_UINT32 uwLenBytes)
{
    VOICE_DEC_SERIAL_STRU       *pstDecSerialWrite= VOICE_ProcGetDecSerialWritePtr();
    VOICE_DEC_SERIAL_STRU       *pstDecBuf        = VOICE_ProcGetDecBufPtr();

    /* 解码数据缓存已满 */
    if(pstDecSerialWrite >= (pstDecBuf + VOICE_CODED_DECODE_BUF_FRAME_NUM))
    {
        OM_LogWarning(VOICE_ProcAddDecSerialWrite_RemoveExpiredDecSerial);

        /* 将原有的数据向前POP一帧 */
        VOICE_ProcSubDecSerialWrite(VOICE_PROC_IS_NO);

        /* 更新缓存队列写指针 */
        pstDecSerialWrite = VOICE_ProcGetDecSerialWritePtr();
    }

    /* 将下行解码数据拷入解码缓存中 */
    UCOM_MemCpy((VOS_VOID *)pstDecSerialWrite->asDecSerial,
                pvMsg,
                uwLenBytes);

    pstDecSerialWrite->uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* 指向下一个解码数据存储单元 */
    VOICE_ProcAddDecSerialWritePtr();
}
VOS_UINT32 VOICE_ProcCfgDlCodec(
                CODEC_NET_MODE_ENUM_UINT16         enNetMode,
                VOS_INT16                         *pshwDecBuf,
                VOS_UINT16                         uhwRxFrmLost,
                VOS_INT16                        **pshwDecData )
{
    VOS_UINT32                    uwRet         = UCOM_RET_SUCC;
    GPHY_VOICE_RX_DATA_IND_STRU  *pstGsmRxData  = VOS_NULL;
    WPHY_VOICE_RX_DATA_IND_STRU  *pstUmtsRxData = VOS_NULL;
    TDPHY_VOICE_RX_DATA_IND_STRU *pstTdRxData   = VOS_NULL;
    IMS_DL_DATA_OBJ_STRU         *pstImsDLData  = VOS_NULL;
    VOICE_PROC_CODEC_STRU        *pstCodec        = VOICE_ProcGetCodecPtr();
    VOICE_DIAG_RX_DATA_STRU       stDiagRxData;
    VOICE_MC_MODEM_NUM_ENUM_UINT16 enActiveModemNo = VOICE_McGetForeGroundNum(); /* 前台通道号 */

    UCOM_MemSet(&stDiagRxData, 0, sizeof(VOICE_DIAG_RX_DATA_STRU));

    switch(enNetMode)
    {
        case CODEC_NET_MODE_G:
        {
            pstGsmRxData = (GPHY_VOICE_RX_DATA_IND_STRU *)pshwDecBuf;

            /* 下行丢帧，置为BFI帧 */
            if(VOICE_PROC_IS_YES == uhwRxFrmLost)
            {
                pstGsmRxData->enBfi = CODEC_BFI_YES;
                pstGsmRxData->enAmrFrameType = CODEC_AMR_TYPE_RX_NO_DATA;
            }

            /* GSM下配置解码参数 */
            uwRet = VOICE_ProcCfgGsmDlCodec(pstGsmRxData);

            if (UCOM_RET_SUCC != uwRet)
            {
                return uwRet;
            }

            /* 获取G下待解码数据的首地址 */
            *pshwDecData = (VOS_INT16 *)pstGsmRxData->puhwData;

            /* 保存下行帧类型等相关信息，用于单通检测 */
            stDiagRxData.enNetMode      = CODEC_NET_MODE_G;
            stDiagRxData.enCodecType    = pstGsmRxData->enCodecType;
            stDiagRxData.enBfi          = pstGsmRxData->enBfi;
            stDiagRxData.enSid          = pstGsmRxData->enSid;
            stDiagRxData.enAmrFrameType = pstGsmRxData->enAmrFrameType;

            break;
        }

        case CODEC_NET_MODE_W:
        {
            pstUmtsRxData = (WPHY_VOICE_RX_DATA_IND_STRU *)pshwDecBuf;

            /* 下行丢帧，置为BAD帧 */
            if(VOICE_PROC_IS_YES == uhwRxFrmLost)
            {
                pstUmtsRxData->enQualityIdx = CODEC_AMR_FQI_QUALITY_BAD;

                /* 置A/B/C子流长度为0 */
                UCOM_MemSet(pstUmtsRxData->auhwLen, 0, (VOS_UINT32)((VOS_UINT16)WPHY_VOICE_AMR_SUBFRAME_NUM * sizeof(VOS_INT16)));
            }

            /* 调用VOICE_ProcCfgUmtsDlCodec进行UMTS下行解码参数配置 */
            uwRet = VOICE_ProcCfgUmtsDlCodec();

            if (uwRet == UCOM_RET_FAIL)
            {
                return UCOM_RET_FAIL;
            }

            /* 获取W下待解码数据的首地址 */
            *pshwDecData = (VOS_INT16 *)&pstUmtsRxData->enQualityIdx;

            /* 保存下行帧类型等相关信息，用于单通检测 */
            stDiagRxData.enNetMode      = CODEC_NET_MODE_W;
            stDiagRxData.enQualityIdx   = pstUmtsRxData->enQualityIdx;

            break;
        }

        case CODEC_NET_MODE_TD:
        {
            pstTdRxData = (TDPHY_VOICE_RX_DATA_IND_STRU *)pshwDecBuf;

            /* 下行丢帧，置为BAD帧 */
            if(VOICE_PROC_IS_YES == uhwRxFrmLost)
            {
                pstTdRxData->enQualityIdx = CODEC_AMR_FQI_QUALITY_BAD;

                /* 置A/B/C子流长度为0 */
                UCOM_MemSet(pstTdRxData->auhwLen, 0, (VOS_UINT32)((VOS_UINT16)WPHY_VOICE_AMR_SUBFRAME_NUM * sizeof(VOS_INT16)));
            }

            /* 调用VOICE_ProcCfgUmtsDlCodec进行TD下行解码参数配置 */
            uwRet = VOICE_ProcCfgUmtsDlCodec();

            if (uwRet == UCOM_RET_FAIL)
            {
                return UCOM_RET_FAIL;
            }

            /* 获取TD下待解码数据的首地址 */
            *pshwDecData = (VOS_INT16 *)&pstTdRxData->enQualityIdx;

            /* 保存下行帧类型等相关信息，用于单通检测 */
            stDiagRxData.enNetMode      = CODEC_NET_MODE_TD;
            stDiagRxData.enQualityIdx   = pstTdRxData->enQualityIdx;

            break;
        }

        case CODEC_NET_MODE_L:
        {
            *pshwDecData = pshwDecBuf;

            /* 保存下行帧类型等相关信息，用于单通检测 */
            pstImsDLData = (IMS_DL_DATA_OBJ_STRU*)pshwDecBuf;
            stDiagRxData.enNetMode      = CODEC_NET_MODE_L;
            stDiagRxData.enQualityIdx   = (CODEC_AMR_FQI_QUALITY_ENUM_UINT16)pstImsDLData->usQualityIdx;

            break;
        }
        default:
        {
            OM_LogError(VOICE_ProcCfgDlCodec_UnkownMode);
            return UCOM_RET_FAIL;
        }
    }


    /* 根据帧类型检测下行语音是否单通或断续 */
    if(pstCodec->enCodecType != CODEC_G711)
    {
        stDiagRxData.enDevMode      = VOICE_McGetDeviceMode();
        stDiagRxData.usIsDecInited  = pstCodec->usIsDecInited;
        stDiagRxData.usIsEncInited  = pstCodec->usIsEncInited;

        VOICE_DiagLineIn(enActiveModemNo, &stDiagRxData);
    }


    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_ProcCfgUmtsDlCodec( VOS_VOID )
{
    VOICE_PROC_CODEC_STRU     *pstCodec   = VOICE_ProcGetCodecPtr();

    /* 检测声码器类型合法性 */
    if ( (CODEC_AMR != pstCodec->enCodecType)
        &&(CODEC_AMR2 != pstCodec->enCodecType)
        &&(CODEC_AMRWB != pstCodec->enCodecType))
    {
        OM_LogError(VOICE_CfgUmtsDlCodecError);
        return UCOM_RET_FAIL;
    }

    /* 码流格式标志为UMTS下语音帧格式 */
    pstCodec->stDecInObj.enAmrFormat    = CODEC_AMR_FORMAT_IF1;

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcCfgUmtsUlCodec( VOS_VOID )
{
    VOICE_PROC_CODEC_STRU  *pstCodec = VOICE_ProcGetCodecPtr();

    /* 声码器类型检查,若声码器类型更新则需要同时重新初始化声码器 */
    if ( (CODEC_AMR != pstCodec->enCodecType)
        &&(CODEC_AMR2 != pstCodec->enCodecType)
        &&(CODEC_AMRWB != pstCodec->enCodecType))
    {
        OM_LogError(VOICE_CfgUmtsUlCodecError);
        return UCOM_RET_FAIL;
    }

    /* DTX类型检查,若DTX类型更新则需要同时重新初始化声码器 */
    if (pstCodec->stEncInObj.enDtxMode != (VOS_INT16)VOICE_AmrModeCtrlGetDtx())
    {
        pstCodec->usIsEncInited         = VOICE_PROC_IS_NO;
        pstCodec->stEncInObj.enDtxMode  = VOICE_AmrModeCtrlGetDtx();
    }

    /* 配置速率模式 */
    pstCodec->stEncInObj.enAmrMode      = VOICE_AmrModeCtrlChangeMode();

    /* 码流格式标志为UMTS下语音帧格式 */
    pstCodec->stEncInObj.enAmrFormat    = CODEC_AMR_FORMAT_IF1;

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcFrameTypeToRlc(VOS_UINT16 uhwFrameTypeIdx)
{
    WTTFVOICE_MAC_AMR_DATA_STRU     stAmr;
    VOICE_PROC_CODEC_STRU          *pstCodec    = VOICE_ProcGetCodecPtr();


    UCOM_MemSet(&stAmr, 0, sizeof(stAmr));

    stAmr.enFrameType    = uhwFrameTypeIdx;

    if (CODEC_AMRWB == pstCodec->enCodecType)
    {
        stAmr.enCodecType = WTTFVOICE_AMR_BANDWIDTH_TYPE_WB;

        /* 若当前编码速率为特殊的UMTS盲检测速率类型，则给TTF的外部帧类型需要改变 */
        if ((WTTFVOICE_AMRWB_CODECMODE_TYPE_1585K_B == pstCodec->stEncInObj.enAmrMode)
            &&(CODEC_AMRWB_FRM_TYPE_1585K  == uhwFrameTypeIdx))
        {
            stAmr.enFrameType    = WTTFVOICE_AMRWB_FRAME_TYPE_1585K_B;
        }
        else if ((WTTFVOICE_AMRWB_CODECMODE_TYPE_2385K_B == pstCodec->stEncInObj.enAmrMode)
                &&(CODEC_AMRWB_FRM_TYPE_2385K  == uhwFrameTypeIdx))

        {
            stAmr.enFrameType    = WTTFVOICE_AMRWB_FRAME_TYPE_2385K_B;
        }
        else
        {
            /* for pc-lint */
        }
    }
    else
    {
        stAmr.enCodecType = WTTFVOICE_AMR_BANDWIDTH_TYPE_NB;
    }

    stAmr.enMsgName      = ID_VOICE_MAC_AMR_DATA_REQ;
    stAmr.usMuteFlag     = 1;                                                 /*最后写入MuteFlag*/

    /* 调用通用VOS发送接口，发送消息 */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    UCOM_PID_PS_WTTF,
                    &stAmr,
                    sizeof(stAmr));

    return UCOM_RET_SUCC;
}



VOS_UINT32 VOICE_ProcOutputFrameType(VOS_UINT16 uhwFrameTypeIdx)
{
    VOS_UINT32              uwRet;

    /* R99下将本帧编码帧类型通过AHB提前通知MAC计算加密序列 */
    uwRet = VOICE_ProcFrameTypeToRlc(uhwFrameTypeIdx);

    /* 调用VOICE_McChkMicConflick，进行UMTS下上行时序冲突检测 */
    VOICE_McChkMicConflick();

    return uwRet;
}


VOS_UINT32 VOICE_ProcVolume(
                VOS_INT16               shwVolumeDb,
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwOut)
{
    VOS_INT16              shwFrameLength =  CODEC_PcmGetPcmFrameLength();

    /* 判断音量的合法性 */
    if ((shwVolumeDb > VOICE_PROC_GAIN_MAX) || (shwVolumeDb < VOICE_PROC_GAIN_MIN))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* 音量增益，其中，MED_GAIN_Db2Linear(shwVolumeDb)的结果为Q13 */
    CODEC_OpVcMultScale(pshwIn,
                      (VOS_INT32)shwFrameLength,
                      MED_GAIN_Db2Linear(shwVolumeDb),
                      MED_PP_SHIFT_BY_14,
                      pshwOut);

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcFacch(VOS_INT16   *psPcm)
{
    VOICE_PROC_CODEC_STRU     *pstCodec = VOICE_ProcGetCodecPtr();
    VOS_INT16                  shwMaxRandRange;

    shwMaxRandRange = VOICE_ProcGetMaxFrameRange();

    /* 偷帧处理 */
    /* 1. 当连续偷帧时，插接近静音帧的小幅度随机数，不解码直接返回 */
    if (VOICE_ProcGetFacchFrameCnt() > VOICE_PROC_GSM_RX_FACCH_THD)
    {
        VOICE_PcmInsertCn(psPcm, CODEC_PCM_FRAME_LENGTH, shwMaxRandRange);

        return UCOM_RET_ERR_STATE;
    }
    /* 2. 若存在1-2帧偷帧，将解码前的数据使用上帧的数据进行覆盖 */
    else if (VOICE_ProcGetFacchFrameCnt() > 0)
    {
        UCOM_MemCpy(&pstCodec->stDecInObj,
                    VOICE_ProcGetPreDecInObj(),
                    sizeof(CODEC_DEC_PREIN_PARA_STRU));

        UCOM_MemCpy(pstCodec->stDecInObj.pshwDecSerial,
                    VOICE_ProcGetPrevDecDataPtr(),
                    GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));

        /* 注: 这里G-FR/EFR/HR/AMR/AMR2/AMRWB都可能走到此分支，这里同时将AMR和BFI的标志进行修改 */
        pstCodec->stDecInObj.enBfiFlag      = CODEC_BFI_YES;
        pstCodec->stDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_NO_DATA;
    }
    /* 3. 若非偷帧帧，将此帧数据保存下来 */
    else
    {
        /* 存储该帧编码前的数据 */
        UCOM_MemCpy(VOICE_ProcGetPreDecInObj(),
                    &pstCodec->stDecInObj,
                    sizeof(CODEC_DEC_PREIN_PARA_STRU));

        UCOM_MemCpy(VOICE_ProcGetPrevDecDataPtr(),
                    pstCodec->stDecInObj.pshwDecSerial,
                    GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcBFI(VOICE_PROC_CODEC_STRU  *pstCodec)
{
    VOS_UINT32  uwRet     = UCOM_RET_SUCC;

    /* 若为AMR声码器则返回 */
    if ((CODEC_AMR   == pstCodec->enCodecType)
      ||(CODEC_AMR2  == pstCodec->enCodecType)
      ||(CODEC_AMRWB == pstCodec->enCodecType))
    {
        return uwRet;
    }

    /* 将本帧状态保存至上一帧状态 */
    if ((CODEC_BFI_NO == pstCodec->stDecInObj.enBfiFlag)
      && (0 == VOICE_ProcGetFacchFrameCnt()))
    {
        pstCodec->stDecInObj.enGoodFrameType
        = (pstCodec->stDecInObj.enSidFlag == CODEC_SID_SPEECH)? CODEC_PREFRAME_SPEECH: CODEC_PREFRAME_SID;

        pstCodec->stDecInObj.uhwFrameBfiCnt  = 0;

        /* 存储该帧编码前的数据 */
        UCOM_MemCpy(VOICE_ProcGetPreDecInObj(),
                    &pstCodec->stDecInObj,
                    sizeof(CODEC_DEC_PREIN_PARA_STRU));

        UCOM_MemCpy(VOICE_ProcGetPrevDecDataPtr(),
                    pstCodec->stDecInObj.pshwDecSerial,
                    GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));
    }
    /* 若当前帧为BFI帧则根据前一帧帧类型进行处理 */
    else
    {
        /* 若无前一帧，则对本帧置0 */
        if (CODEC_PREFRAME_NONE == pstCodec->stDecInObj.enGoodFrameType)
        {
            return UCOM_RET_ERR_STATE;
        }
        /* 若连续前两帧BFI帧且无SID，则重复上一帧 */
        else if (CODEC_PREFRAME_SPEECH == pstCodec->stDecInObj.enGoodFrameType)
        {
            pstCodec->stDecInObj.uhwFrameBfiCnt = pstCodec->stDecInObj.uhwFrameBfiCnt + 1;

            if(pstCodec->stDecInObj.uhwFrameBfiCnt <= VOICE_PROC_BFI_MAX)
            {
                UCOM_MemCpy(&pstCodec->stDecInObj,
                            VOICE_ProcGetPreDecInObj(),
                            sizeof(CODEC_DEC_PREIN_PARA_STRU));

                UCOM_MemCpy(pstCodec->stDecInObj.pshwDecSerial,
                            VOICE_ProcGetPrevDecDataPtr(),
                            GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));

                /* 注: 因为只有G-FR/EFR/HR会走到此分支，这里只将BFI的标志置为YES */
                pstCodec->stDecInObj.enBfiFlag      = CODEC_BFI_YES;
            }
            else
            {
                return UCOM_RET_ERR_STATE;
            }
        }
        else
        {
            //pstDecInObj->enGoodFrameType == CODEC_PREFRAME_SID
            //for lint
        }
    }

    return uwRet;
}
MED_UINT16 VOICE_ProcCheckUpdate( VOS_UINT16 uhwCodecType  )
{
    VOS_INT16                           shwOldFrameLength;
    VOS_INT16                           shwNewFrameLength;
    VOS_UINT16                          ushwRet = VOICE_PROC_IS_NO;

    /* 获取更新前的帧长，用于判断是否是宽窄带切换 */
    shwOldFrameLength = CODEC_PcmGetPcmFrameLength();

    /* 计算set_codec之后的帧长 */
    if((CODEC_AMRWB == uhwCodecType) || (CODEC_G711 == uhwCodecType))
    {
        shwNewFrameLength = CODEC_FRAME_LENGTH_WB;
    }
    else
    {
        shwNewFrameLength = CODEC_FRAME_LENGTH_NB;
    }

    /* 采样率有变化则更新 */
    if(shwOldFrameLength != shwNewFrameLength)
    {
        ushwRet =  VOICE_PROC_IS_YES;
    }
    else
    {
        ushwRet =  VOICE_PROC_IS_NO;
    }

    return ushwRet;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

