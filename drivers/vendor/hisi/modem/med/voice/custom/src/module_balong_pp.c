

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "mlib_interface.h"
#include "module_balong_pp.h"
#include "med_pp_main.h"
#include "VcCodecInterface.h"
#include "CodecNvId.h"
#include "hifidrvinterface.h"
#include "ucom_nv.h"
#include "om_comm.h"

#ifdef __cplusplus
    #if __cplusplus
extern "C" {
    #endif
#endif

/*lint -e(767)*/
#define THIS_FILE_ID                OM_FILE_ID_MODULE_BALONG_PP_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* NV ID表，
第1维: 采样率   8000/16000
第2维: 场景模式 HandSet/HandFree/CarFree/HeadSet/BlueTooth/PcVoice/HeadPhone
第3维: 一个场景模式所占用的ID个数 */
VOS_UINT16  g_auhwMedProcNvIdTable[MED_CTRL_SAMPLE_RATE_NUM][MED_CTRL_DEVICE_MODE_NUM][MED_CTRL_MODE_SUB_ID_NUM] =
{
    {
        {en_NV_NB_HandSet1,     en_NV_NB_HandSet2,       en_NV_NB_HandSet3},
        {en_NV_NB_HandFree1,    en_NV_NB_HandFree2,      en_NV_NB_HandFree3},
        {en_NV_NB_CarFree1,     en_NV_NB_CarFree2,       en_NV_NB_CarFree3},
        {en_NV_NB_HeadSet1,     en_NV_NB_HeadSet2,       en_NV_NB_HeadSet3},
        {en_NV_NB_BlueTooth1,   en_NV_NB_BlueTooth2,     en_NV_NB_BlueTooth3},
        {en_NV_NB_PCVoice1,     en_NV_NB_PCVoice2,       en_NV_NB_PCVoice3},
        {en_NV_NB_HeadPhone1,   en_NV_NB_HeadPhone2,     en_NV_NB_HeadPhone3},
    },
    {
        {en_NV_WB_HandSet1,     en_NV_WB_HandSet2,       en_NV_WB_HandSet3},
        {en_NV_WB_HandFree1,    en_NV_WB_HandFree2,      en_NV_WB_HandFree3},
        {en_NV_WB_CarFree1,     en_NV_WB_CarFree2,       en_NV_WB_CarFree3},
        {en_NV_WB_HeadSet1,     en_NV_WB_HeadSet2,       en_NV_WB_HeadSet3},
        {en_NV_WB_BlueTooth1,   en_NV_WB_BlueTooth2,     en_NV_WB_BlueTooth3},
        {en_NV_WB_PCVoice1,     en_NV_WB_PCVoice2,       en_NV_WB_PCVoice3},
        {en_NV_WB_HeadPhone1,   en_NV_WB_HeadPhone2,     en_NV_WB_HeadPhone3},
    }
};

/* 声学处理NV_ID表 */
VOS_UINT16  g_auhwAudiProcNvIdTable[AUDIO_ENHANCE_DEVICE_BUTT][AUDIO_PP_NV_ARRAY_ID_NUM] =
{
    {en_NV_Audio_HandSet1,      en_NV_Audio_HandSet2,        en_NV_Audio_HandSet3},
    {en_NV_Audio_HandFree1,     en_NV_Audio_HandFree2,       en_NV_Audio_HandFree3},
    {en_NV_Audio_HandFree1,     en_NV_Audio_HandFree2,       en_NV_Audio_HandFree3},
    {en_NV_Audio_HeadPhone1,    en_NV_Audio_HeadPhone2,      en_NV_Audio_HeadPhone3},
    {en_NV_Audio_HeadPhone1,    en_NV_Audio_HeadPhone2,      en_NV_Audio_HeadPhone3},
    {en_NV_Audio_HandFree1,     en_NV_Audio_HandFree2,       en_NV_Audio_HandFree3},
    {en_NV_Audio_HeadPhone1,    en_NV_Audio_HeadPhone2,      en_NV_Audio_HeadPhone3},
};

VOS_INT16                               g_ashwPpNv[MED_PP_NV_ARRAY_TOTAL_LEN];

/* SET NV 消息处理NV拆分消息计数标志 */
VOS_UINT16                              g_uhwSetNvMsgFlag = 0;

MODULE_BALONG_CTRL_STRU                 g_stModuleBalongVoiceCtrl = {0};
MODULE_BALONG_CTRL_STRU                 g_stModuleBalongAudioCtrl = {0};

/*****************************************************************************
  历史代码
*****************************************************************************/


VOS_UINT32 VOICE_ProcReadNv(
                VOS_UINT32 uwDevMode,
                VOS_UINT32 uwNetMode,
                VOS_UINT32 uwSampleRate,
                VOS_INT16 *pshwNv)
{
    VOS_INT16                       shwCntI;
    VOS_UINT16                      uhwNvId;
    VOS_UINT16                      uwSubTblId;
    VOS_UINT32                      uwRet;

    /* 采样率映射到子表号 */
    uwSubTblId = (CODEC_SAMPLE_RATE_MODE_8000 == uwSampleRate) ? 0 : 1;

    /* 根据enDevMode enNetMode、enSampleRate 从表中得到ID号shwIdNum */
    for (shwCntI = 0; shwCntI < MED_CTRL_MODE_SUB_ID_NUM; shwCntI++)
    {
        uhwNvId  = g_auhwMedProcNvIdTable[uwSubTblId][uwDevMode][shwCntI];

        uwRet    = UCOM_NV_Read(uhwNvId, pshwNv, MED_CTRL_MODE_SUB_ID_LEN*sizeof(VOS_INT16));

        pshwNv   = pshwNv + MED_CTRL_MODE_SUB_ID_LEN;
    }

    return uwRet;

}


VOS_UINT32 VOICE_ProcSetNv(VOS_INT16 *pshwNv, VOS_UINT16 uhwMicNum)
{
    VOS_UINT32                 swRet      = 0;

    swRet = MED_PpSetPara((VOICE_NV_STRU *)pshwNv, uhwMicNum);

    /* NV参数合法性检测汇总，若参数非法则返回失败，否则返回合法 */
    if (swRet > 0)
    {
        return UCOM_RET_ERR_STATE;
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_ProcUpdateNv(
                VOS_UINT32 uwDevMode,
                VOS_UINT32 uwNetMode,
                VOS_UINT32 uwSampleRate)
{
    VOS_UINT32                          uwRet;
    VOS_INT16                          *pshwPpNv;
    VOS_UINT16                          uhwPpNum;

    pshwPpNv = MED_CTRL_McPpNvArrayPtr();

    /* 从板侧读取NV参数到stNv */
    uwRet = VOICE_ProcReadNv(uwDevMode, uwNetMode, uwSampleRate, pshwPpNv);

    if ( UCOM_RET_SUCC != uwRet )
    {
        OM_LogError(VOICE_ReadNvError);
    }
    else
    {

        /* 读取配置的麦克风个数 NV项 */
        UCOM_NV_Read(en_NV_PHY_MIC_NUM,
                     &uhwPpNum,
                     sizeof(uhwPpNum));

        if ((MED_PP_MIC_NUM_2 == uhwPpNum)
         && (VCVOICE_DEV_MODE_HANDSET == uwDevMode))
        {
            uhwPpNum = MED_PP_MIC_NUM_2;
        }
        else
        {
            uhwPpNum = MED_PP_MIC_NUM_1;
        }

        uwRet = VOICE_ProcSetNv(pshwPpNv, uhwPpNum);

        if ( UCOM_RET_SUCC != uwRet )
        {
            OM_LogError(VOICE_SetNvError);
        }
        else
        {
            OM_LogInfo(VOICE_UpdateNvOk);
        }
    }

    return uwRet;
}


VOS_UINT32 VOICE_ProcOmSetNv(VOS_VOID *pData)
{
    VOS_UINT32                      uwRet = UCOM_RET_SUCC;
    OM_BALONG_VOICE_NV_DATA_STRU   *pstNv = VOS_NULL;

    VOS_UINT16                      uhwParaLen;
    VOS_INT16                      *pshwPpNv = MED_CTRL_McPpNvArrayPtr();
    VOS_UINT16                      uhwMicNum;

    pstNv = (OM_BALONG_VOICE_NV_DATA_STRU*)pData;

    uhwMicNum = (pstNv->usDeviceMode == VCVOICE_DEV_MODE_HANDSET) ? 2 : 1;

    /* 保留拆分消息顺序记录 */
    g_uhwSetNvMsgFlag = g_uhwSetNvMsgFlag | (VOS_UINT16)(1 << pstNv->usIndex);

    uhwParaLen = (VOS_UINT16)(CODEC_PP_NV_ARRAY_ID_LEN * pstNv->usIndex);

    CODEC_OpVecCpy(&pshwPpNv[uhwParaLen], pstNv->ashwNv, CODEC_PP_NV_ARRAY_ID_LEN);

    if (MED_CTRL_MC_SET_NV_MSG_MASK == (g_uhwSetNvMsgFlag & MED_CTRL_MC_SET_NV_MSG_MASK))
    {
        /* 使用读到的NV参数更新PP各模块 */
        uwRet = VOICE_ProcSetNv(pshwPpNv, uhwMicNum);

        if(UCOM_RET_SUCC == uwRet)
        {
            OM_LogInfo(VOICE_MsgSetNvReq_Ok);
        }
        else
        {
            OM_LogError(VOICE_MsgSetNvReq_ParamError);
        }

        /* 消息接收标志清0 */
        g_uhwSetNvMsgFlag = 0;
    }

    return uwRet;
}
VOS_UINT32 AUDIO_ProcReadNv(
                VOS_UINT32 uwDevMode,
                VOS_INT16 *pshwNv)
{
    VOS_INT16                       shwCntI;
    VOS_UINT16                      uhwNvId;
    VOS_UINT32                      uwRet;

    /* 根据enDevMode enNetMode、enSampleRate 从表中得到ID号shwIdNum */
    for (shwCntI = 0; shwCntI < AUDIO_PP_NV_ARRAY_ID_NUM; shwCntI++)
    {
        uhwNvId  = g_auhwAudiProcNvIdTable[uwDevMode][shwCntI];

        uwRet    = UCOM_NV_Read(uhwNvId, pshwNv, AUDIO_PP_NV_ARRAY_ID_LEN*sizeof(VOS_INT16));

        pshwNv   = pshwNv + AUDIO_PP_NV_ARRAY_ID_LEN;
    }

    return uwRet;

}
VOS_UINT32 AUDIO_ProcSetNv(VOS_INT16 *pshwNv, VOS_UINT32 uwSampleRate)
{
    VOS_UINT32                 swRet      = 0;

    swRet = AUDIO_PpSetPara((AUDIO_NV_STRU *)pshwNv, uwSampleRate);

    /* NV参数合法性检测汇总，若参数非法则返回失败，否则返回合法 */
    if (swRet > 0)
    {
        return UCOM_RET_ERR_STATE;
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 AUDIO_ProcUpdateNv(
                VOS_UINT32 uwDevMode,
                VOS_UINT32 uwSampleRate)
{
    VOS_UINT32                          uwRet;
    VOS_INT16                          *pshwPpNv;

    pshwPpNv = MED_CTRL_McPpNvArrayPtr();

    /* 从板侧读取NV参数到stNv */
    uwRet = AUDIO_ProcReadNv(uwDevMode, pshwPpNv);

    if ( UCOM_RET_SUCC != uwRet )
    {
        OM_LogError(VOICE_ReadNvError);
    }
    else
    {
        uwRet = AUDIO_ProcSetNv(pshwPpNv, uwSampleRate);

        if ( UCOM_RET_SUCC != uwRet )
        {
            OM_LogError(VOICE_SetNvError);
        }
        else
        {
            OM_LogInfo(VOICE_UpdateNvOk);
        }
    }

    return uwRet;
}
VOS_UINT32 AUDIO_ProcOmSetNv(VOS_VOID *pData)
{
    VOS_UINT32                      uwRet = UCOM_RET_SUCC;
    OM_BALONG_AUDIO_NV_DATA_STRU   *pstNv = VOS_NULL;
    VOS_UINT32                      uwSampleRate;

    VOS_UINT16                      uhwParaLen;
    VOS_INT16                      *pshwPpNv = MED_CTRL_McPpNvArrayPtr();

    pstNv = (OM_BALONG_AUDIO_NV_DATA_STRU*)pData;

    /* 计算采样率 */
    uwSampleRate = (pstNv->usSampleRate + 1) * 8000;

    /* 保留拆分消息顺序记录 */
    g_uhwSetNvMsgFlag = g_uhwSetNvMsgFlag | (VOS_UINT16)(1 << pstNv->usIndex);

    uhwParaLen = (VOS_UINT16)(AUDIO_PP_NV_ARRAY_ID_LEN * pstNv->usIndex);

    CODEC_OpVecCpy(&pshwPpNv[uhwParaLen], pstNv->ashwNv, AUDIO_PP_NV_ARRAY_ID_LEN);

    if (MED_CTRL_MC_SET_NV_MSG_MASK == (g_uhwSetNvMsgFlag & MED_CTRL_MC_SET_NV_MSG_MASK))
    {
        /* 使用读到的NV参数更新PP各模块 */
        uwRet = AUDIO_ProcSetNv(pshwPpNv, uwSampleRate);

        if(UCOM_RET_SUCC == uwRet)
        {
            OM_LogInfo(VOICE_MsgSetNvReq_Ok);
        }
        else
        {
            OM_LogError(VOICE_MsgSetNvReq_ParamError);
        }

        /* 消息接收标志清0 */
        g_uhwSetNvMsgFlag = 0;
    }

    return uwRet;
}

/*****************************************************************************
  3 函数实现
*****************************************************************************/


MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_Init( MLIB_VOID )
{
    MLIB_INT32                  swRet;
    MODULE_BALONG_CTRL_STRU    *pstCtrl;

    pstCtrl = MODULE_BALONG_GetVoiceCtrl();

    /* 只在当前引用计数为0时进行初始化 */
    if(0 == pstCtrl->swRef)
    {
        swRet = MED_PP_Init();

        UCOM_MemSet(MODULE_BALONG_GetVoiceParaSet(), 0, sizeof(MLIB_PARA_STRU));
    }
    else
    {
        swRet = MLIB_ERROR_NONE;
    }

    pstCtrl->swRef++;

    return (MLIB_ERROR_ENUM_UINT32)((swRet == 0) ? MLIB_ERROR_NONE : MLIB_ERROR_FAIL);
}

/*****************************************************************************
 函 数 名  : BA_VOICE_PP_Set
 功能描述  : 设置库参数
 输入参数  : pstPara    - 参数结构体指针
             uwSize     - 参数大小(byte)
 输出参数  : 无
 附加说明  : 系统在语音/音频运行过程中环境参数发生变化时调用这个函数

*****************************************************************************/
MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_Set(
                MLIB_PARA_STRU         *pstPara,
                MLIB_UINT32             uwSize)
{
    MLIB_ERROR_ENUM_UINT32  enRet;

    /* 设置采样率 */
    MED_PP_SetSampleRate((VOS_INT32)pstPara->uwSampleRate);

    /* 设置帧长 */
    MED_PP_SetFrameLength((VOS_INT16)pstPara->uwFrameLen);

    /* 重置FFT */
    /*lint -e(746)*/
    MED_FFT_InitAllObjs();

    /* 记录当前设置 */
    UCOM_MemCpy(MODULE_BALONG_GetVoiceParaSet(), pstPara, sizeof(MLIB_PARA_STRU));

    /* 根据数据源分别处理 */
    switch(pstPara->enParaSource)
    {
        case MLIB_PARA_SOURCE_NV:
        {
            enRet = VOICE_ProcUpdateNv(
                        pstPara->enDevice,
                        0,
                        pstPara->uwSampleRate);
        }
        break;

        case MLIB_PARA_SOURCE_TUNER:
        {
            enRet = VOICE_ProcOmSetNv(pstPara->aucData);
        }
        break;

        default:
        {
            enRet = MLIB_ERROR_FAIL;
        }
        break;

    }

    return enRet;
}


MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_Get(
                MLIB_VOID              *pvData,
                MLIB_UINT32             uwSize)
{
    VOS_UINT16                          uhwCntI;
    VOS_UINT16                          uhwParaLen;
    VOS_INT16                          *pshwPpNv;
    MSG_OM_MLIB_PARA_GET_STRU          *pstGetPara;
    VOS_UINT32                          uwBufSize;
    OM_BALONG_VOICE_NV_DATA_STRU        stNvData;
    MLIB_PARA_STRU                     *pstParaSet;

    pshwPpNv     = MED_CTRL_McPpNvArrayPtr();
    pstParaSet   = MODULE_BALONG_GetVoiceParaSet();

    uwBufSize = sizeof(MSG_OM_MLIB_PARA_GET_STRU) + sizeof(OM_BALONG_VOICE_NV_DATA_STRU);
    pstGetPara = (MSG_OM_MLIB_PARA_GET_STRU*)UCOM_MemAlloc(uwBufSize);

    /* 初始化结构体*/
    UCOM_MemSet(pstGetPara, 0, uwBufSize);
    UCOM_MemSet(&stNvData,  0, sizeof(OM_BALONG_VOICE_NV_DATA_STRU));

    /* 拷贝回复信息 */
    UCOM_MemCpy(pstGetPara, pvData, sizeof(MSG_OM_MLIB_PARA_GET_STRU));

    /* 读取NV项 */
    MED_PpGetPara((VOICE_NV_STRU *)pshwPpNv);

    stNvData.usDeviceMode = (VOS_UINT16)pstParaSet->enDevice;
    stNvData.usNetMode    = 0;
    stNvData.usSampleRate = (VOS_UINT16)((pstParaSet->uwSampleRate/8000) - 1);

    /* 向OM回复原语 */
    pstGetPara->usMsgName = ID_VOICE_OM_GET_NV_CNF;
    pstGetPara->uwSize    = sizeof(OM_BALONG_VOICE_NV_DATA_STRU);

    for (uhwCntI = 0; uhwCntI < CODEC_PP_NV_ARRAY_ID_NUM; uhwCntI++)
    {
        stNvData.usIndex = uhwCntI;

        uhwParaLen = (VOS_UINT16)(uhwCntI * CODEC_PP_NV_ARRAY_ID_LEN);

        CODEC_OpVecCpy(stNvData.ashwNv, &pshwPpNv[uhwParaLen], CODEC_PP_NV_ARRAY_ID_LEN);

        UCOM_MemCpy(pstGetPara->aucData, &stNvData, sizeof(OM_BALONG_VOICE_NV_DATA_STRU));

        OM_COMM_SendTranMsg(pstGetPara, uwBufSize);
    }

    OM_LogInfo(VOICE_MsgGetNvReq_Ok);

    UCOM_MemFree(pstGetPara);

    /* 不再重复发送数据 */
    return UCOM_RET_FAIL;

}
MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_ProcMicIn(
                MLIB_SOURCE_STRU       *pstSourceOut,
                MLIB_SOURCE_STRU       *pstSourceIn)
{
    MLIB_UINT32                 uwRet;
    MODULE_BALONG_CTRL_STRU    *pstCtrl;

    pstCtrl = MODULE_BALONG_GetVoiceCtrl();

    /* 未初始化 */
    if(0 == pstCtrl->swRef)
    {
        return MLIB_ERROR_NULL_FUNC;
    }

    uwRet = MED_TxPp(
                (VOS_INT16*)pstSourceIn->astChn[0].pucData,
                (VOS_INT16*)pstSourceIn->astChn[1].pucData,
                (VOS_INT16*)pstSourceOut->astChn[0].pucData,
                (VOS_INT16*)pstSourceIn->astChn[2].pucData);

    UCOM_MemCpy(pstSourceOut->astChn[1].pucData,
                pstSourceIn->astChn[1].pucData,
                pstSourceIn->astChn[1].uwSize);

    UCOM_MemCpy(pstSourceOut->astChn[2].pucData,
                pstSourceIn->astChn[2].pucData,
                pstSourceIn->astChn[2].uwSize);

    return (MLIB_ERROR_ENUM_UINT32)((uwRet == 0) ? MLIB_ERROR_NONE : MLIB_ERROR_FAIL);
}
MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_ProcSpkOut(
                MLIB_SOURCE_STRU       *pstSourceOut,
                MLIB_SOURCE_STRU       *pstSourceIn)
{
    MLIB_UINT32                 uwRet;
    MODULE_BALONG_CTRL_STRU    *pstCtrl;

    pstCtrl = MODULE_BALONG_GetVoiceCtrl();

    /* 未初始化 */
    if(0 == pstCtrl->swRef)
    {
        return MLIB_ERROR_NULL_FUNC;
    }

    UCOM_MemCpy(pstSourceOut->astChn[0].pucData,
                pstSourceIn->astChn[0].pucData,
                pstSourceIn->astChn[0].uwSize);

    uwRet = MED_RxPp((VOS_INT16*)pstSourceOut->astChn[0].pucData);

    return (MLIB_ERROR_ENUM_UINT32)((uwRet == 0) ? MLIB_ERROR_NONE : MLIB_ERROR_FAIL);
}
MLIB_ERROR_ENUM_UINT32  BA_VOICE_PP_Clear( MLIB_VOID )
{
    MODULE_BALONG_CTRL_STRU    *pstCtrl;

    pstCtrl = MODULE_BALONG_GetVoiceCtrl();

    if(0 == pstCtrl->swRef)
    {
        return MLIB_ERROR_NULL_FUNC;
    }

    pstCtrl->swRef--;

    /* 只在当前引用计数为0时进行销毁 */
    if(0 == pstCtrl->swRef)
    {
        MED_PpDestroy();
    }

    return MLIB_ERROR_NONE;
}
MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_Init( MLIB_VOID )
{
    MLIB_INT32          swRet;
   MODULE_BALONG_CTRL_STRU    *pstCtrl;

    pstCtrl = MODULE_BALONG_GetAudioCtrl();

    /* 只在当前引用计数为0时进行初始化 */
    if(0 == pstCtrl->swRef)
    {
        swRet = AUDIO_PP_Init();

        UCOM_MemSet(MODULE_BALONG_GetAudioParaSet(), 0, sizeof(MLIB_PARA_STRU));
    }
    else
    {
        swRet = MLIB_ERROR_NONE;
    }

    pstCtrl->swRef++;

    return (MLIB_ERROR_ENUM_UINT32)((swRet == 0) ? MLIB_ERROR_NONE : MLIB_ERROR_FAIL);
}
MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_Set(
                MLIB_PARA_STRU         *pstPara,
                MLIB_UINT32             uwSize)
{
    MLIB_ERROR_ENUM_UINT32  enRet;

    /* 设置帧长 */
    AUDIO_PP_SetFrameLength((VOS_INT16)pstPara->uwFrameLen);

    /* 重置FFT */
    MED_FFT_InitAllObjs();

    /* 记录当前设置 */
    UCOM_MemCpy(MODULE_BALONG_GetAudioParaSet(), pstPara, sizeof(MLIB_PARA_STRU));

    /* 根据数据源分别处理 */
    switch(pstPara->enParaSource)
    {
        case MLIB_PARA_SOURCE_NV:
        {
            enRet = AUDIO_ProcUpdateNv(
                        pstPara->enDevice,
                        pstPara->uwSampleRate);
        }
        break;

        case MLIB_PARA_SOURCE_TUNER:
        {
            enRet = AUDIO_ProcOmSetNv(pstPara->aucData);
        }
        break;

        default:
        {
            enRet = MLIB_ERROR_FAIL;
        }
        break;

    }

    return enRet;
}


MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_Get(
                MLIB_VOID              *pvData,
                MLIB_UINT32             uwSize)
{
    VOS_UINT16                          uhwCntI;
    VOS_UINT16                          uhwParaLen;
    VOS_INT16                          *pshwPpNv;
    MSG_OM_MLIB_PARA_GET_STRU          *pstGetPara;
    VOS_UINT32                          uwBufSize;
    OM_BALONG_AUDIO_NV_DATA_STRU        stNvData;
    MLIB_PARA_STRU                     *pstParaSet;

    pshwPpNv     = MED_CTRL_McPpNvArrayPtr();
    pstParaSet   = MODULE_BALONG_GetAudioParaSet();

    uwBufSize = sizeof(MSG_OM_MLIB_PARA_GET_STRU) + sizeof(OM_BALONG_AUDIO_NV_DATA_STRU);
    pstGetPara = (MSG_OM_MLIB_PARA_GET_STRU*)UCOM_MemAlloc(uwBufSize);

    /* 初始化结构体*/
    UCOM_MemSet(pstGetPara, 0, uwBufSize);
    UCOM_MemSet(&stNvData,  0, sizeof(OM_BALONG_AUDIO_NV_DATA_STRU));

    /* 拷贝回复信息 */
    UCOM_MemCpy(pstGetPara, pvData, sizeof(MSG_OM_MLIB_PARA_GET_STRU));

    /* 读取NV项 */
    AUDIO_PpGetPara((AUDIO_NV_STRU *)pshwPpNv);

    stNvData.usDeviceMode = (VOS_UINT16)pstParaSet->enDevice;
    stNvData.usSampleRate = (VOS_UINT16)((pstParaSet->uwSampleRate/8000) - 1);

    /* 向OM回复原语 */
    pstGetPara->usMsgName = ID_VOICE_OM_GET_NV_CNF;
    pstGetPara->uwSize    = sizeof(OM_BALONG_AUDIO_NV_DATA_STRU);

    for (uhwCntI = 0; uhwCntI < AUDIO_PP_NV_ARRAY_ID_NUM; uhwCntI++)
    {
        stNvData.usIndex = uhwCntI;

        uhwParaLen = (VOS_UINT16)(uhwCntI * AUDIO_PP_NV_ARRAY_ID_LEN);

        CODEC_OpVecCpy(stNvData.ashwNv, &pshwPpNv[uhwParaLen], AUDIO_PP_NV_ARRAY_ID_LEN);

        UCOM_MemCpy(pstGetPara->aucData, &stNvData, sizeof(OM_BALONG_AUDIO_NV_DATA_STRU));

        OM_COMM_SendTranMsg(pstGetPara, uwBufSize);
    }

    OM_LogInfo(VOICE_MsgGetNvReq_Ok);

    UCOM_MemFree(pstGetPara);

    /* 不再重复发送数据 */
    return UCOM_RET_FAIL;

}
MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_ProcMicIn(
                MLIB_SOURCE_STRU       *pstSourceOut,
                MLIB_SOURCE_STRU       *pstSourceIn)
{
    MLIB_UINT32                 uwRet;
    MODULE_BALONG_CTRL_STRU    *pstCtrl;

    pstCtrl = MODULE_BALONG_GetAudioCtrl();

    /* 未初始化 */
    if(0 == pstCtrl->swRef)
    {
        return MLIB_ERROR_NULL_FUNC;
    }

    uwRet = AUDIO_ProcPpTx(
                (VOS_INT16*)pstSourceIn->astChn[0].pucData,
                (VOS_INT16*)pstSourceIn->astChn[1].pucData,
                (VOS_INT16*)pstSourceOut->astChn[0].pucData,
                (VOS_INT16*)pstSourceOut->astChn[1].pucData,
                AUDIO_PP_CHANNEL_ALL);

    return (MLIB_ERROR_ENUM_UINT32)((uwRet == 0) ? MLIB_ERROR_NONE : MLIB_ERROR_FAIL);
}
MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_ProcSpkOut(
                MLIB_SOURCE_STRU       *pstSourceOut,
                MLIB_SOURCE_STRU       *pstSourceIn)
{
    MLIB_UINT32                 uwRet;
    MODULE_BALONG_CTRL_STRU    *pstCtrl;

    pstCtrl = MODULE_BALONG_GetAudioCtrl();

    /* 未初始化 */
    if(0 == pstCtrl->swRef)
    {
        return MLIB_ERROR_NULL_FUNC;
    }

    uwRet = AUDIO_ProcPpRx(
                (VOS_INT16*)pstSourceIn->astChn[0].pucData,
                (VOS_INT16*)pstSourceIn->astChn[1].pucData,
                (VOS_INT16*)pstSourceOut->astChn[0].pucData,
                (VOS_INT16*)pstSourceOut->astChn[1].pucData,
                AUDIO_PP_CHANNEL_ALL);

    return (MLIB_ERROR_ENUM_UINT32)((uwRet == 0) ? MLIB_ERROR_NONE : MLIB_ERROR_FAIL);
}
MLIB_ERROR_ENUM_UINT32  BA_AUDIO_PP_Clear( MLIB_VOID )
{
    MODULE_BALONG_CTRL_STRU    *pstCtrl;

    pstCtrl = MODULE_BALONG_GetAudioCtrl();

    if(0 == pstCtrl->swRef)
    {
        return MLIB_ERROR_NULL_FUNC;
    }

    pstCtrl->swRef--;

    /* 只在当前引用计数为0时进行销毁 */
    if(0 == pstCtrl->swRef)
    {
        AUDIO_PpDestroy();
    }

    return MLIB_ERROR_NONE;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

