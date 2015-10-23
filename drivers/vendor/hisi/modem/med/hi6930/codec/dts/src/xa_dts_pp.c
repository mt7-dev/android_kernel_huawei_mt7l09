

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "xa_dts_pp.h"
#include "ucom_mem_dyn.h"
#include "om_log.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_CODEC_XA_DTS_PP_C
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* DTS内部维护成员变量:private */
XA_DTS_CURRENT_CFG_STRU             g_stDtsCurtCfg;

/* DTS的外界控制参数只允许对外接口改变成员值
 * DTS模块不允许对其改变，uninit段HIFI动态加载不被初始化 */
UCOM_SET_UNINIT
XA_DTS_SET_PARA_STRU                g_stDtsUintPara;

/* Max HPF filter order and number of TruEQ bands */
static SRSTrMediaHDCreateData       g_stCreateData = {{6, 4, 4}};

/* DTS当前配置参数
 * 有且仅有使能与否、设备模式这两种情况会改变当前配置的参数 */
SRSTrMediaHDControls                g_stDtsCurrentPara;

VOS_INT16                           g_ashwDefaultGeqBandGains[XA_DTS_GEQ_BAND_NUM]={4096,4096,4096,4096,4096,4096,4096,4096,4096,4096};

/*****************************************************************************
  4 函数实现
*****************************************************************************/


VOS_UINT32 XA_DTS_Init( XA_DTS_USER_CFG_STRU * pstUserCfg )
{
    XA_DTS_SET_PARA_STRU                *pstDtsSetPara = XA_DTS_GetUinitDDRParaPtr();
    VOS_INT32                            swRet;

    /* 清空之前的配置信息 */
    XA_DTS_DeleObj();

    /* 创建DTS对象 */
    swRet = XA_DTS_CreatObj(pstUserCfg);

    /* 如果创建失败则析构DTS对象并上报OM信息 */
    if (VOS_OK != swRet)
    {
        XA_DTS_DeleObj();

        OM_LogError1(XA_DTS_Init_Fail, VOS_ERR);

        return VOS_ERR;
    }

    /* 再次调用XA_DTS_Init说明歌曲切换，需保持上一首的DTS设置
     * 包括是否使能、输出设备的设置
     * 有且仅有此处对其使能 */
    pstDtsSetPara->enIsNeedSetPara = XA_DTS_NEED_SET_PARA_YES;

    OM_LogInfo1(XA_DTS_Init_OK, pstDtsSetPara->enIsNeedSetPara);

    return VOS_OK;
}




VOS_INT32  XA_DTS_CreatObj(XA_DTS_USER_CFG_STRU * pstUserCfg)
{
    XA_DTS_CURRENT_CFG_STRU       *pstDtsCurtCfg      = XA_DTS_GetCurrentCfgPtr();
    VOS_UINT32                     uwBlockSize, uwFilterCfg, uwSampleRate;
    VOS_CHAR                      *pDtsObjBuff;
    VOS_INT32                     *pswLeftBuff, *pswRightBuff;
    VOS_VOID                      *pvWorkSpaceBuff;
    SRSTrMediaHDObj                stTmhdObj;
    VOS_INT32                      swRet = VOS_OK;

    /* 异常入参检查 */
    if ( VOS_NULL == pstUserCfg)
    {
        /* 上报OM信息 */
        OM_LogError1(XA_DTS_InvalidPara, VOS_ERR);
        return VOS_ERR;
    }

    /* 清空当前配置信息全局变量 */
    UCOM_MemSet(pstDtsCurtCfg, 0, sizeof(XA_DTS_CURRENT_CFG_STRU));

    /* 获取并记录BlockSize、滤波器配置、采样率、声道数 */
    uwBlockSize                 = pstUserCfg->uwBlockSize;
    uwFilterCfg                 = pstUserCfg->uwFilterCfg;
    uwSampleRate                = pstUserCfg->uwSampleRate;
    uwSampleRate                = uwSampleRate/1000;

    pstDtsCurtCfg->uwBlockSize  = uwBlockSize;
    pstDtsCurtCfg->uwFilterCfg  = uwFilterCfg;
    pstDtsCurtCfg->uwSampleRate = uwSampleRate;
    pstDtsCurtCfg->swChannelNum = pstUserCfg->swChannelNum;

    /* 初始化FFT Lib库空间 */
    XA_DTS_InitFFT_Lib();

    /* 配置DTSObj内存空间 */
    /*lint -e655*/
    pDtsObjBuff     = (VOS_CHAR *)UCOM_MemAllocFair(SRS_TrMediaHD_GetObjSize(&g_stCreateData));
    pstDtsCurtCfg->pDtsObjBuff  = pDtsObjBuff;
    pstDtsCurtCfg->uwDtsObjSize = (VOS_UINT32)SRS_TrMediaHD_GetObjSize(&g_stCreateData);

    /* 配置左右声道内存空间 */
    /*lint -e655*/
    pswLeftBuff         = (VOS_INT32 *)UCOM_MemAllocFair((uwBlockSize * XA_DTS_CHANNEL_NUM) * sizeof(VOS_INT32));
    pswRightBuff        = pswLeftBuff + uwBlockSize;
    pstDtsCurtCfg->pswLeftBuff      = pswLeftBuff;
    pstDtsCurtCfg->pswRightBuff     = pswRightBuff;
    pstDtsCurtCfg->uwLeftSize       = (uwBlockSize * 1) * sizeof(VOS_INT32);
    pstDtsCurtCfg->uwRightSize      = (uwBlockSize * 1) * sizeof(VOS_INT32);

    /* 配置workspace内存空间，scratch buffer */
    /*lint -e961*/
    /*lint -e506*/
    /*lint -e655*/
    pvWorkSpaceBuff      = UCOM_MemAllocFair(SRS_TRMEDIAHD_WORKSPACE_SIZE(uwBlockSize));
    pstDtsCurtCfg->pvWorkSpaceBuff  = pvWorkSpaceBuff;
    pstDtsCurtCfg->uwWorkSpaceSize  = SRS_TRMEDIAHD_WORKSPACE_SIZE(uwBlockSize);
    /*lint +e961*/
    /*lint +e506*/
    /*lint +e655*/

    swRet += SRS_TrMediaHD_CreateObj(&stTmhdObj, pDtsObjBuff, 0, 0, &g_stCreateData);

    /* 记录DTS对象 */
    pstDtsCurtCfg->stDtsObj         = stTmhdObj;

    /* 初始化DTSObj */
    XA_DTS_InitObj(pstDtsCurtCfg->stDtsObj, (VOS_UINT16)uwFilterCfg, uwSampleRate);

    /*********************** 设置默认参数 ********************** */
    /* 默认输出设备为speaker，设置speaker参数 */
    swRet += (VOS_INT32)XA_DTS_SetDev(XA_DTS_DEV_SPEAKRE);

    /* 默认DTS不打开 */
    swRet += (VOS_INT32)XA_DTS_SetEnable(XA_DTS_ENABLE_NO);

    swRet += XA_DTS_SetGEQBandGains(pstDtsCurtCfg->stDtsObj, g_ashwDefaultGeqBandGains);

    return swRet;

}
VOS_VOID XA_DTS_InitObj(SRSTrMediaHDObj stTmhdObj, XA_DTS_FILTER_CFG_ENUM_UINT16 uhwFset, VOS_UINT32 uwSampleRate)
{
    SRSTrMediaHDFilterConfig     stCfg;
    SRSTrMediaHDFilterConfig    *pstCfg;

    /*读取AP_NV项，若读取失败则使用默认参数*/
    UCOM_NV_Read(XA_DTS_NV_ID_Speaker, &DTS_speaker, sizeof(SRSTrMediaHDControls));
    UCOM_NV_Read(XA_DTS_NV_ID_HeadPhone, &DTS_headphone, sizeof(SRSTrMediaHDControls));
    UCOM_NV_Read(XA_DTS_NV_ID_HpfOrder, &HPF_Order, sizeof(SRSTrMediaHDHpfOrder));
    UCOM_NV_Read(XA_DTS_NV_ID_UntitledHpf1, &Untitled_HPF1[0], sizeof(SRSInt16)*XA_DTS_HPF_PARA_LEN);
    UCOM_NV_Read(XA_DTS_NV_ID_UntitledHpf2, &Untitled_HPF2[0], sizeof(SRSInt16)*XA_DTS_HPF_PARA_LEN);
    UCOM_NV_Read(XA_DTS_NV_ID_UntitledPeq1, &Untitled_PEQ1[0], sizeof(SRSInt32)*XA_DTS_PEQ_PARA_LEN);
    UCOM_NV_Read(XA_DTS_NV_ID_UntitledPeq2, &Untitled_PEQ2[0], sizeof(SRSInt32)*XA_DTS_PEQ_PARA_LEN);
    UCOM_NV_Read(XA_DTS_NV_ID_UntitledTbhd1, &Untitled_TBHD1[0], sizeof(SRSInt32)*XA_DTS_TBHD_PARA_LEN);
    UCOM_NV_Read(XA_DTS_NV_ID_UntitledTbhd2, &Untitled_TBHD2[0], sizeof(SRSInt32)*XA_DTS_TBHD_PARA_LEN);

    /* 默认使用speaker */
    stCfg  = GetFilterNonflatPeqCfg((VOS_INT32)uwSampleRate, XA_DTS_DEV_SPEAKRE);
    pstCfg = &stCfg;

    /* 根据不同采样率初始化不同对象 */
    switch(uwSampleRate)
    {
        case 32:
            SRS_TrMediaHD_InitObj32k(stTmhdObj, pstCfg); break;
        case 44:
            SRS_TrMediaHD_InitObj44k(stTmhdObj, pstCfg); break;
        case 48:
            SRS_TrMediaHD_InitObj48k(stTmhdObj, pstCfg); break;
        default:
        {
            /* for_pclint */
        }
    }

}


VOS_INT32  XA_DTS_ExeProcess(XA_DTS_USER_CFG_STRU * pstUserCfg)
{
    XA_DTS_CURRENT_CFG_STRU                     *pstDtsCurtCfg   = XA_DTS_GetCurrentCfgPtr();
    XA_DTS_SET_PARA_STRU                        *pstUinitDDRPara = XA_DTS_GetUinitDDRParaPtr();
    SRSTrMediaHDObj                              stDtsObj;
    SRSStereoCh                                  stAudioIO;
    VOS_VOID                                    *pvWorkSpace;
    VOS_INT32                                    swBlokSize;
    VOS_INT32                                    swRet = VOS_OK;

    /* 如果需要设置参数，则重新设置unit DDR 保存的DTS参数
     * 1、歌曲切换时需要重新设置
     * 2、由于只有在Init之后才能设置使能、设备参数，所以在执行函数中设置而不在消息响应函数中设置 */
    if ( XA_DTS_NEED_SET_PARA_YES == pstUinitDDRPara->enIsNeedSetPara )
    {
        /* 上报OM信息 */
        OM_LogInfo2(XA_DTS_SetParaWhileExe, pstUinitDDRPara->enIsEnable, pstUinitDDRPara->enCurrentDev);

        /* 设置是否使能 */
        swRet += (VOS_INT32)XA_DTS_SetEnable(pstUinitDDRPara->enIsEnable);

        /* 设置输出设备模式 */
        swRet += (VOS_INT32)XA_DTS_SetDev(pstUinitDDRPara->enCurrentDev);

        if (VOS_OK != swRet)
        {
            XA_DTS_DeleObj();

            /* 上报OM信息 */
            OM_LogError1(XA_DTS_SetParaWhileExeFail, VOS_ERR);
            return VOS_ERR;
        }

        /* 恢复标志位 */
        pstUinitDDRPara->enIsNeedSetPara  = XA_DTS_NEED_SET_PARA_NO;

    }

    /* 对输入数据进行DTS音效处理 */
    pstDtsCurtCfg       = XA_DTS_GetCurrentCfgPtr();
    stDtsObj            = pstDtsCurtCfg->stDtsObj;
    stAudioIO.Left      = (SRSInt32 *)pstDtsCurtCfg->pswLeftBuff;
    stAudioIO.Right     = (SRSInt32 *)pstDtsCurtCfg->pswRightBuff;
    pvWorkSpace         = pstDtsCurtCfg->pvWorkSpaceBuff;
    swBlokSize          = (VOS_INT32)pstUserCfg->uwBlockSize;

    /* 获取输入数据 */
    XA_DTS_Hybrid2Stereo(pstUserCfg->pshwInputBuff, (VOS_INT32 *)stAudioIO.Left, (VOS_INT32 *)stAudioIO.Right, swBlokSize);

    swRet += SRS_TrMediaHD_Process(stDtsObj, &stAudioIO, swBlokSize, pvWorkSpace);

    /* 若果失败，则返回并dele DTS对象  */
    if ( SRS_STAT_NO_ERROR != swRet )
    {
        XA_DTS_DeleObj();

        /* 上报OM信息 */
        OM_LogError1(CODEC_TENSILICA_ERR, VOS_ERR);
        return VOS_ERR;
    }

    /* 输出处理后数据 */
    XA_DTS_Stereo2Hybrid((VOS_INT32 *)stAudioIO.Left, (VOS_INT32 *)stAudioIO.Right, swBlokSize, (VOS_INT16 *)pstUserCfg->pshwOutputBuff);

    return swRet;

}



VOS_UINT32 XA_DTS_DeleObj( VOS_VOID )
{
    XA_DTS_CURRENT_CFG_STRU     *pstDtsCurtCfg      = XA_DTS_GetCurrentCfgPtr();
    SRSTrMediaHDControls        *pstCurrentPara     = XA_DTS_GetCurrentParaPtr();

    /* 将分配的内存空间释放 */
    UCOM_MemFree(pstDtsCurtCfg->pDtsObjBuff);
    UCOM_MemFree(pstDtsCurtCfg->pswLeftBuff);
    UCOM_MemFree(pstDtsCurtCfg->pswRightBuff);
    UCOM_MemFree(pstDtsCurtCfg->pvWorkSpaceBuff);
    UCOM_MemFree(pstDtsCurtCfg->pvFFTLibBuff);

    /* 将成员变量赋为default */
    UCOM_MemSet(pstDtsCurtCfg, 0, sizeof(XA_DTS_CURRENT_CFG_STRU));
    UCOM_MemSet(pstCurrentPara, 0, sizeof(SRSTrMediaHDControls));

    return VOS_OK;
}


VOS_UINT32 XA_DTS_MsgSetDtsEnableCmd (VOS_VOID *pvOsaMsg)
{
    XA_DTS_SET_PARA_STRU                   *pstUinitDDRPara = XA_DTS_GetUinitDDRParaPtr();
    AUDIO_PLAYER_DTS_SET_ENABLE_CMD_STRU   *pstSetParaReq;
    XA_DTS_ENABLE_ENUM_UINT16               enEnableFlag;

    /* 异常入参检查 */
    if ( VOS_NULL == pvOsaMsg )
    {
        /* 上报OM信息 */
        OM_LogError1(XA_DTS_InvalidPara, VOS_ERR);
        return VOS_ERR;
    }

    /* 按照DTS音效参数结构体解析 */
    pstSetParaReq   = (AUDIO_PLAYER_DTS_SET_ENABLE_CMD_STRU *)pvOsaMsg;

    /* 取出DTS是否使能配置信息 */
    enEnableFlag    = pstSetParaReq->uhwEnableFlag;

    /* 当是否使能DTS发生了变化时设置是否使能 */
    if (enEnableFlag != pstUinitDDRPara->enIsEnable)
    {
        if ( (XA_DTS_ENABLE_NO != enEnableFlag) && (XA_DTS_ENABLE_YES != enEnableFlag) )
        {
            /* 告警 */
            OM_LogWarning1(XA_DTS_SetEableInvalidPara, enEnableFlag);

            enEnableFlag = XA_DTS_ENABLE_NO;
        }
        else
        {
            /* 在uint区记录是否使能 */
            pstUinitDDRPara->enIsEnable       = enEnableFlag;
        }

        /* 设置使能只能在Init之后，故在此只做标记，在exe中设置参数 */
        pstUinitDDRPara->enIsNeedSetPara = XA_DTS_NEED_SET_PARA_YES;

        /* 上报om信息 */
        OM_LogInfo2(XA_DTS_SetDevOK, pstUinitDDRPara->enIsNeedSetPara, pstUinitDDRPara->enIsEnable);

    }

    return VOS_OK;

}
VOS_UINT32  XA_DTS_MsgSetDtsDevCmd( VOS_VOID *pvOsaMsg )
{
    XA_DTS_SET_PARA_STRU                *pstDtsUinitPara    = XA_DTS_GetUinitDDRParaPtr();
    AUDIO_EFFECT_DTS_SET_DEV_REQ_STRU   *pstDtsSetDev;
    XA_DTS_CURRENT_DEV_ENUM_UINT32       enDtsDev;

    /* 异常入参检查 */
    if ( VOS_NULL == pvOsaMsg )
    {
        /* 上报OM信息 */
        OM_LogError1(XA_DTS_InvalidPara, VOS_ERR);
        return VOS_ERR;
    }

    /* 按照设置DTS音效输出设备结构体解析 */
    pstDtsSetDev   = (AUDIO_EFFECT_DTS_SET_DEV_REQ_STRU *)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 取出DTS输出设备配置信息 */
    enDtsDev      = pstDtsSetDev->uhwDevMode;

    /* 如果AP发送的设备模式有变换，则进行设置 */
    if (enDtsDev != pstDtsUinitPara->enCurrentDev)
    {
        if ( (XA_DTS_DEV_HEADSET != enDtsDev) && (XA_DTS_DEV_SPEAKRE != enDtsDev) )
        {
            /* 告警 */
            OM_LogWarning1(XA_DTS_SetDevInvalidPara, enDtsDev);

            /* 未知设备设置为speaker */
            pstDtsUinitPara->enCurrentDev = XA_DTS_DEV_SPEAKRE;
        }
        else
        {
            /* 在Uint DDR记录当前使用的输出设备 */
            pstDtsUinitPara->enCurrentDev = enDtsDev;
        }

        /* 设置设备只能在Init之后，故在此只做标记，在exe中设置参数 */
        pstDtsUinitPara->enIsNeedSetPara = XA_DTS_NEED_SET_PARA_YES;

        /* 上报om信息 */
        OM_LogInfo2(XA_DTS_SetDevOK, pstDtsUinitPara->enIsNeedSetPara, pstDtsUinitPara->enCurrentDev);

    }

    return VOS_OK;

}


VOS_INT32 XA_DTS_SetGEQBandGains(SRSTrMediaHDObj stDtsObj, VOS_INT16 * pshwGeqGain)
{
    VOS_INT32                                   swCnt;
    VOS_INT32                                   swRet = VOS_OK;

    for ( swCnt = 0; swCnt < XA_DTS_GEQ_BAND_NUM; swCnt++)
    {
        swRet += SRS_TrMediaHD_SetGEQBandGain(stDtsObj, swCnt, pshwGeqGain[swCnt]);
    }

    return swRet;
}


VOS_INT32 XA_DTS_Hybrid2Stereo(
                VOS_INT16 *pshwI2S,
                VOS_INT32 *pswLeft,
                VOS_INT32 *pswRight,
                VOS_INT32  swBlockSize)
{
    XA_DTS_CURRENT_CFG_STRU                     *pstDtsCurtCfg  = XA_DTS_GetCurrentCfgPtr();
    VOS_INT32                                    swChannelNum;
    VOS_INT32                                    swCnt         = 0;

    /* 获取当前配置声道数 */
    swChannelNum = pstDtsCurtCfg->swChannelNum;

    /* 小端数据处理方法，将16bit数据置于分开后的左右声道32bit的高16bit */
    for (swCnt = 0; swCnt < ((swBlockSize * swChannelNum)/2); swCnt++)
    {
        pswLeft[swCnt]     = pshwI2S[swCnt * 2]<<16;
        pswRight[swCnt]    = pshwI2S[(swCnt * 2) + 1]<<16;
    }

    return swCnt;
}

VOS_UINT32 XA_DTS_Stereo2Hybrid(
                VOS_INT32  *pswLeft,
                VOS_INT32  *pswRight,
                VOS_INT32   swBlockSize,
                VOS_INT16  *shwHybrid)
{
    VOS_INT32                  swCnt;

    for ( swCnt = 0; swCnt < swBlockSize; swCnt++ )
    {
        shwHybrid[swCnt * 2]            = (VOS_INT16)(pswLeft[swCnt]>>16);
        shwHybrid[(swCnt * 2) + 1]      = (VOS_INT16)(pswRight[swCnt]>>16);
    }

    return (VOS_UINT32)swCnt;
}


VOS_VOID  XA_DTS_InitFFT_Lib(VOS_VOID)
{
    XA_DTS_CURRENT_CFG_STRU     *pstDtsCurtCfg      = XA_DTS_GetCurrentCfgPtr();
    VOS_VOID                    *pFftLibBuf;
    VOS_INT32                    swPlanBuffSize;

    /* 获取大小并分配FFTllib库buff */
    swPlanBuffSize  = SRS_Fft_GetPlanBufSize(SRS_WOWHDX_BLK_SZ*2);

    /*lint -e655*/
    pFftLibBuf = UCOM_MemAllocFair(swPlanBuffSize);

    /* 记录FFTllib库buff大小 */
    pstDtsCurtCfg->pvFFTLibBuff = pFftLibBuf;

    SRS_Fft_CreatePlan(pFftLibBuf, SRS_WOWHDX_BLK_SZ*2, SRS_RFFT_32C16);

}
VOS_UINT32  XA_DTS_SetDev( XA_DTS_CURRENT_DEV_ENUM_UINT32 enDevice )
{
    SRSTrMediaHDControls        *pstCurrentPara     = XA_DTS_GetCurrentParaPtr();
    XA_DTS_CURRENT_CFG_STRU     *pstDtsCurtCfg      = XA_DTS_GetCurrentCfgPtr();
    XA_DTS_SET_PARA_STRU        *pstDtsUinitPara    = XA_DTS_GetUinitDDRParaPtr();
    SRSTrMediaHDObj              stDtsObj           = pstDtsCurtCfg->stDtsObj;
    SRSTrMediaHDFilterConfig    *pstCfg             = 0;
    SRSTrMediaHDFilterConfig     stCfg;
    VOS_INT32                    swRet              = VOS_OK;

    /* 异常入参检查 */
    if (XA_DTS_DEV_BUTT <= enDevice)
    {
        OM_LogError1(XA_DTS_SetDevError, enDevice);

        return VOS_ERR;
    }

    /* 耳机模式 */
    if (XA_DTS_DEV_HEADSET == enDevice)
    {
        /* 设置耳机模式的参数 */
        UCOM_MemCpy(pstCurrentPara, &DTS_headphone, sizeof(SRSTrMediaHDControls));

        /* 获取滤波器配置 */
        stCfg  = GetFilterNonflatPeqCfg((VOS_INT32)pstDtsCurtCfg->uwSampleRate, XA_DTS_DEV_HEADSET);

        /* 调用实时接口设置headset滤波器系数 */
        swRet += SRS_TrMediaHD_SetCshpTBHDCustomSpeakerFilterCoefs(stDtsObj, Untitled_TBHD2);
        swRet += SRS_TrMediaHD_SetWowhdxTBHDCustomSpeakerFilterCoefs(stDtsObj, Untitled_TBHD2);
    }
    else /* 其他设备目前均设置为speaker */
    {
        /* 设置speaker模式参数 */
        UCOM_MemCpy(pstCurrentPara, &DTS_speaker, sizeof(SRSTrMediaHDControls));

        /* 获取滤波器配置 */
        stCfg  = GetFilterNonflatPeqCfg((VOS_INT32)pstDtsCurtCfg->uwSampleRate, XA_DTS_DEV_SPEAKRE);

        /* 调用实时接口设置speaker滤波器系数 */
        swRet += SRS_TrMediaHD_SetCshpTBHDCustomSpeakerFilterCoefs(stDtsObj, Untitled_TBHD1);
        swRet += SRS_TrMediaHD_SetWowhdxTBHDCustomSpeakerFilterCoefs(stDtsObj, Untitled_TBHD1);
    }

    /* 根据是否是能调整参数 */
    if (XA_DTS_ENABLE_YES == pstDtsUinitPara->enIsEnable)
    {
        pstCurrentPara->Enable = XA_DTS_ENABLE_YES;
    }
    else if (XA_DTS_ENABLE_NO == pstDtsUinitPara->enIsEnable)
    {
        pstCurrentPara->Enable = XA_DTS_ENABLE_NO;
    }
    else
    {
        pstCurrentPara->Enable = XA_DTS_ENABLE_NO;

        OM_LogError1(XA_DTS_SetDevError, pstDtsUinitPara->enIsEnable);
    }

    /* 调用实时接口设置参数 */
    swRet += SRS_TrMediaHD_SetControls(stDtsObj, pstCurrentPara);

    /* 设置滤波器系数 */
    pstCfg = &stCfg;
    swRet += SRS_TrMediaHD_InitObj48k(stDtsObj, pstCfg);

    /* 上报执行结果 */
    if (SRS_STAT_NO_ERROR == swRet)
    {
        OM_LogInfo1(XA_DTS_SetDevOK, enDevice);
    }
    else
    {
        OM_LogError1(XA_DTS_SetDevError, enDevice);
    }

    return (VOS_UINT32)swRet;

}
VOS_UINT32  XA_DTS_SetEnable(XA_DTS_ENABLE_ENUM_UINT16 enEnableFlag)
{
    SRSTrMediaHDControls        *pstCurrentPara     = XA_DTS_GetCurrentParaPtr();
    XA_DTS_CURRENT_CFG_STRU     *pstDtsCurtCfg      = XA_DTS_GetCurrentCfgPtr();
    SRSTrMediaHDObj              stDtsObj           = pstDtsCurtCfg->stDtsObj;
    VOS_INT32                    swRet;

    /* 更改当前运行参数 */
    if (XA_DTS_ENABLE_YES == enEnableFlag)
    {
        /* 设置使能参数 */
        pstCurrentPara->Enable = XA_DTS_ENABLE_YES;
    }
    else /* (XA_DTS_ENABLE_NO == enEnableFlag)*/
    {
        /* 设置不使能参数 */
        pstCurrentPara->Enable = XA_DTS_ENABLE_NO;
    }

    /* 设置参数 */
    swRet = SRS_TrMediaHD_SetControls(stDtsObj, pstCurrentPara);

    /* 上报执行结果 */
    if (SRS_STAT_NO_ERROR == swRet)
    {
        OM_LogInfo1(XA_DTS_SetEnableOK, enEnableFlag);
    }
    else
    {
        OM_LogError1(XA_DTS_SetEnableFail, enEnableFlag);
    }

    return (VOS_UINT32)swRet;

}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

