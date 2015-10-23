

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "msp.h"
#include    "NasEmmPubUInclude.h"
#include    "LRrcLPhyInterface.h"
#include    "LPsOm.h"
#include    "EmmUeModeInterface.h"
#include    "SysNvId.h"
#include    "NVIM_Interface.h"
#include    "NasEmmSecuSmc.h"
#include    "NasNvInterface.h"
#include    "msp_nvim.h"
#include    "ScInterface.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMPUBUGLOBAL_C
/*lint +e767*/


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/

/*VOS_UINT32                              g_ulRatMode = 2;*/    /* PS_FIRST_CALL 使用 */
NAS_LMM_PUB_INFO_STRU                   g_stEmmInfo;
VOS_UINT32                              g_ulSmcControl      = NAS_EMM_SMC_CLOSE;
VOS_UINT32                              g_ulUeSecuCapMatch  = NAS_EMM_SMC_OPEN;

/*
VOS_UINT32                              g_ulDetachPowerOff  = NAS_EMM_DO_DETACH_WHEN_POWEROFF;
*/
/*VOS_UINT32                              g_ulAutoStartFlag   = NOT_AUTO_START;*/
VOS_UINT32                              g_ulAutoAttFlag     = NAS_EMM_AUTO_ATTACH_ALLOW;
VOS_UINT32                              g_ulCsAutoAttFlag   = NAS_EMM_AUTO_ATTACH_ALLOW;
NAS_LMM_SYSINFO_COUNT_STRU               g_stMsgSysInfCnt;

OM_EMM_STATE_INFO_STRU                   g_stEmmOmInfo;

/*****************************************************************************
  3 Function
*****************************************************************************/
/*lint -e960*/
/*lint -e961*/


VOS_VOID    NAS_LMM_EmmStateInit( VOS_VOID )
{

    NAS_LMM_FSM_STATE_STRU              *pEmmCurState;

    /* 当前状态初始化 */
    pEmmCurState = NAS_LMM_GetCurFsmAddr(NAS_LMM_PARALLEL_FSM_EMM);
    pEmmCurState->enFsmId               = NAS_LMM_PARALLEL_FSM_EMM;
    pEmmCurState->enMainState           = EMM_MS_NULL;
    pEmmCurState->enSubState            = EMM_SS_NULL_WAIT_APP_START_REQ;
    pEmmCurState->enStaTId              = TI_NAS_EMM_STATE_NO_TIMER;

    return;
}
VOS_VOID    NAS_EMM_FsmInit(            VOS_VOID )
{

    NAS_LMM_FSM_STATE_STRU              *pEmmCurState;
    NAS_LMM_FSM_STATE_STACK_STRU        *pEmmStack;

    NAS_LMM_PUBM_LOG_NORM(
    "NAS_EMM_FsmInit                   START INIT...");

    /* 当前状态初始化 */
    pEmmCurState = NAS_LMM_GetCurFsmAddr(NAS_LMM_PARALLEL_FSM_EMM);
    pEmmCurState->enFsmId               = NAS_LMM_PARALLEL_FSM_EMM;
    pEmmCurState->enMainState           = EMM_MS_NULL;
    pEmmCurState->enSubState            = EMM_SS_NULL_WAIT_APP_START_REQ;
    pEmmCurState->enStaTId              = TI_NAS_EMM_STATE_NO_TIMER;


    /* 状态机栈 初始化 */
    pEmmStack   = NAS_LMM_GetFsmStackAddr(NAS_LMM_PARALLEL_FSM_EMM);
    pEmmStack->ucStackDepth             = NAS_EMM_STACK_EMPTY;

    NAS_EMM_InitBufMsgQue();

    return;
}


VOS_VOID    NAS_EMM_PubmInfoUeIdInit(   VOS_VOID )
{
    /*IMSI,IMEI必须有，所以初始化UeId有效*/
    NAS_LMM_GetEmmInfoAddr()->bitOpUeId  = NAS_EMM_BIT_SLCT;

    NAS_LMM_GetEmmInfoUeidAddr()->bitOpTmsi     = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_GetEmmInfoUeidAddr()->bitOpPtmsi    = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_GetEmmInfoUeidAddr()->bitOpTmgiMbms = NAS_EMM_BIT_NO_SLCT;
    return;
}
VOS_VOID    NAS_EMM_PubmInfoNetInfoInit(VOS_VOID )
{
    /*初始网络信息有效*/
    NAS_LMM_GetEmmInfoAddr()->bitOpNetInfo = NAS_EMM_BIT_SLCT;

    /*如果IMSI有效，即卡有效，设置HPLMN有效，MMC初始化时计算赋值*/
    if (NAS_EMM_BIT_SLCT == NAS_LMM_GetEmmInfoUeidAddr()->bitOpImsi)
    {
        NAS_LMM_GetEmmInfoNetInfoAddr()->bitOpHplmn = NAS_EMM_BIT_SLCT;
    }

    /*未开机，当前网络信息无效*/
    NAS_LMM_GetEmmInfoNetInfoAddr()->bitOpPresentNetId = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_UndefPlmn(NAS_LMM_GetEmmInfoPresentPlmnAddr());

    /*未开机，上次尝试发起注册的TA信息无效*/
    NAS_LMM_GetEmmInfoNetInfoAddr()->bitOpLastAttmpRegTa = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_UndefPlmn(NAS_LMM_GetEmmInfoLastAttmpRegPlmnAddr());


    /* 将当前网络信息中TAC及Cell ID初始化为无效值*/
    NAS_EMMC_DeleteCurrentAreaCellId();
    NAS_LMM_UndefTac(&(NAS_LMM_GetEmmInfoNetInfoPresentNetAddr()->stTac));

    /*ForbTaList For Roaming关机时擦除,上电无效*/
    /*ForbTaList For Rpos关机时擦除,上电无效*/
    NAS_LMM_GetEmmInfoNetInfoAddr()->bitOpFoibTaForRoam = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_GetEmmInfoNetInfoAddr()->bitOpFoibTaForRpos = NAS_EMM_BIT_NO_SLCT;

    NAS_LMM_GetEmmInfoNetInfoForbTaForRoamAddr()->ulTaNum = 0;
    NAS_LMM_GetEmmInfoNetInfoForbTaForRposAddr()->ulTaNum = 0;

    /*EHPLMN List目前未使用,初始化无效*/
    NAS_LMM_GetEmmInfoNetInfoAddr()->bitOpEHplmnList = NAS_EMM_BIT_NO_SLCT;


    /*L.V.R TAI初始化*/
    NAS_EMM_ClearLVRTai();

    return;
}
VOS_VOID  NAS_EMM_PubmInfoDrxInit(VOS_VOID)
{
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    #if 0
    NAS_EMM_DRX_STRU                    *pstDrx;

    pstDrx = NAS_LMM_GetEmmInfoDrxAddr();
    NAS_EMM_PUBU_LOG2_INFO("NAS_EMM_PubmInfoDrxInit:ucNonDrxTimer = , ucSplitOnCcch = ",
                           NAS_MML_GetNonDrxTimer(),NAS_MML_GetSplitOnCcch());

    NAS_EMM_PUBU_LOG2_INFO("NAS_EMM_PubmInfoDrxInit:ucSplitPgCode = , ucPsDrxLen = ",
                           NAS_MML_GetSplitPgCycleCode(),NAS_MML_GetUeEutranPsDrxLen());
    pstDrx->ucNonDrxTimer = NAS_EMM_GetNonDrxTimer();
    pstDrx->ucSplitOnCcch = NAS_EMM_GetSplitOnCcch();
    pstDrx->ucSplitPgCode = NAS_EMM_GetSplitPgCycleCode();
    pstDrx->ucPsDrxLen = NAS_EMM_GetUeEutranPsDrxLen();
    #endif
    /*PC REPLAY MODIFY BY LEILI END*/
    /*设置DRX改变标识*/
    NAS_LMM_SetEmmInfoDrxNetCapChange(NAS_EMM_NO);
    NAS_EMM_SetDrxCycleLenChangeFlag(NAS_EMM_DRX_CYCLE_LEN_CHANGED);

    return;
}
/*PC REPLAY MODIFY BY LEILI BEGIN*/

VOS_UINT32  NAS_EMM_IsGuContainPsRegDrx(VOS_VOID)
{
    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_IsGuContainPsRegDrx:NAS_MML_GetPsRegContainDrx =",
                           NAS_MML_GetPsRegContainDrx());
    #ifndef __PS_WIN32_RECUR__
    if (NAS_MML_GU_PS_REG_CONTAIN_DRX_PARA == NAS_MML_GetPsRegContainDrx())
    {
       NAS_LMM_LogPsRegContainDrxInfo(NAS_MML_GetPsRegContainDrx());
       return NAS_EMM_SUCC;
    }
    NAS_LMM_LogPsRegContainDrxInfo(NAS_MML_GetPsRegContainDrx());
    return NAS_EMM_FAIL;
    #else
    if (NAS_MML_GU_PS_REG_CONTAIN_DRX_PARA == Stub_NAS_MML_GetPsRegContainDrx())
    {
        return NAS_EMM_SUCC;
    }
    return NAS_EMM_FAIL;
    #endif
}
/*PC REPLAY MODIFY BY LEILI END*/

VOS_VOID  NAS_EMM_UpdateLteContainDrxFlag( VOS_VOID )
{
    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_UpdateLteContainDrxFlag before:NAS_MML_GetPsRegContainDrx() = ",
                               NAS_MML_GetPsRegContainDrx());
    if (NAS_EMM_YES == NAS_EMM_TAU_GetEmmTauContainDrxFlag())
    {
        NAS_MML_SetPsRegContainDrx(NAS_MML_LTE_PS_REG_CONTAIN_DRX_PARA);
    }
    else
    {
        NAS_MML_SetPsRegContainDrx(NAS_MML_LTE_PS_REG_NOT_CONTAIN_DRX_PARA);
    }
    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_UpdateLteContainDrxFlag After:NAS_MML_GetPsRegContainDrx() = ",
                               NAS_MML_GetPsRegContainDrx());
    return;
}








VOS_VOID    NAS_EMM_ReadNvUeNetCap(VOS_VOID)
{
    LNAS_LMM_NV_UE_NET_CAP_STRU         stNvUeNetCap;
    NAS_MM_UE_NET_CAP_STRU             *pstUeNetCap = VOS_NULL_PTR;
    NAS_EMM_PUB_INFO_STRU              *pstEmmInfo  = VOS_NULL_PTR;
    VOS_VOID                           *pData = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;
    FTM_TMODE_ENUM                      enMspFtmMode = EN_FTM_TMODE_SIGNAL ;
    NAS_MML_UE_NETWORK_CAPABILITY_STRU  stUeNetworkCapbility = {0};

    /* 指向 UeNetCap*/
    pstEmmInfo  = NAS_LMM_GetEmmInfoAddr();
    pstUeNetCap = NAS_LMM_GetEmmInfoSecuParaUeNetCapAddr();

    /* 初始化写入MML的UeNetCap为默认值 */
    /*stUeNetworkCapbility.bitOpUeNetCap      = NAS_EMM_BIT_SLCT;*/
    stUeNetworkCapbility.ucUeNetCapLen      = 0x04;

    stUeNetworkCapbility.aucUeNetCap[0]     = 0xF0;
    stUeNetworkCapbility.aucUeNetCap[1]     = 0xF0;
    stUeNetworkCapbility.aucUeNetCap[2]     = 0x40;
    stUeNetworkCapbility.aucUeNetCap[3]     = 0x40;

    /* 初始化本地全局变量UeNetCap为默认值 */
    pstEmmInfo->bitOpUeNetCap       = NAS_EMM_BIT_SLCT;
    pstUeNetCap->ucUeNetCapLen      = 0x04;

    pstUeNetCap->aucUeNetCap[0]     = 0xF0;
    pstUeNetCap->aucUeNetCap[1]     = 0xF0;

    pstUeNetCap->aucUeNetCap[2]     = 0x40;
    pstUeNetCap->aucUeNetCap[3]     = 0x40;
#if (VOS_OS_VER != VOS_WIN32)
    /*如果获取当前模式失败，则默认为当前模式为EN_FTM_TMODE_SIGNAL，打印错误信息*/
    if(LPS_OM_GetTmode(&enMspFtmMode)!= ERR_MSP_SUCCESS)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_ReadNvUeNetCap : FTM_GetMode() called fail!");
    }
    else
    {
        /*产线测试模式，则取默认值:0xE0,0xE0,0x40,0x40*/
        if( EN_FTM_TMODE_SIGNAL_NOCARD == enMspFtmMode)
        {
            NAS_LMM_PUBM_LOG_INFO("NAS_EMM_ReadNvUeNetCap : EN_FTM_TMODE_SIGNAL_NOCARD, UE NetCap is 0xe0,0xe0!");

            /* 将默认值写入MML */
            NAS_MML_SetUeNetWorkCapability(&stUeNetworkCapbility);

            return;
        }
    }
#endif
    /*read UE NET CAP from NVIM*/
    pData = (VOS_VOID *)&(stNvUeNetCap);
    usDataLen = sizeof(LNAS_LMM_NV_UE_NET_CAP_STRU);

    ulRslt = NAS_LMM_NvimRead(EN_NV_ID_UE_NET_CAPABILITY, pData, &usDataLen);

    /* 读取 NVIM成功 */
    if((NAS_LMM_NVIM_OK == ulRslt) && (NAS_EMM_BIT_SLCT == stNvUeNetCap.bitOpUeNetCap))
    {
        /*验证读取数据的有效性:目前阶段只验证长度符合协议。
          后续实现还需要考虑EEA,EIA,UEA,UIA的取值，与开机支持的接入技术是否符合，
          例如开机指示支持L和U，如果读取的UEA,UIA不支持任何算法，则认为数据无效，
          与接入模式的验证，目前阶段暂未实现
        */
        if ((NAS_MM_MIN_UE_NET_CAP_LEN <= stNvUeNetCap.stUeNetCap.ucUeNetCapLen)
            && (NAS_MM_MAX_UE_NET_CAP_LEN >= stNvUeNetCap.stUeNetCap.ucUeNetCapLen))
        {
            NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvUeNetCap:NV value is Valid");
            pstEmmInfo->bitOpUeNetCap           = NAS_EMM_BIT_SLCT;
            NAS_LMM_MEM_CPY(    pstUeNetCap,
                                &(stNvUeNetCap.stUeNetCap),
                                sizeof(NAS_MM_UE_NET_CAP_STRU));

            /* 设置写入MML的UeNetCap为NV中读取出的值 */
            NAS_LMM_MEM_SET(    &stUeNetworkCapbility,
                                0,
                                sizeof(NAS_MML_UE_NETWORK_CAPABILITY_STRU));

            stUeNetworkCapbility.ucUeNetCapLen = stNvUeNetCap.stUeNetCap.ucUeNetCapLen;
            NAS_LMM_MEM_CPY(    &(stUeNetworkCapbility.aucUeNetCap[0]),
                                &(stNvUeNetCap.stUeNetCap.aucUeNetCap[0]),
                                stUeNetworkCapbility.ucUeNetCapLen);
        }
        /* NVIM中读取的数据无效，则赋为默认值*/
        else
        {
            NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvUeNetCap:Nv value is Invalid");
        }
    }
    /* 读取 NVIM 失败，赋为默认值*/
    else
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_ReadNvUeNetCap:Read Net Cap Fail!");
    }

    /* 如果UeNetCap读取成功，将读NV得到的值写入MML；如果失败，写入默认值 */
    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvUeNetCap: Set to MML UeNetCap value is:");
    NAS_MML_SetUeNetWorkCapability(&stUeNetworkCapbility);

    NAS_COMM_PrintArray(    NAS_COMM_GET_MM_PRINT_BUF(),
                            (VOS_UINT8*)&stUeNetworkCapbility,
                            sizeof(NAS_MML_UE_NETWORK_CAPABILITY_STRU));

    return;
}

#if 0

VOS_VOID  NAS_EMM_ReadNvCsService(VOS_VOID )
{

    LNAS_LMM_NV_CS_SERVICE_STRU         stNvCsService;
    NAS_EMM_PUB_INFO_STRU               *pstPubInfo;
    VOS_VOID                           *pData = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();
    pData                               = (VOS_VOID *)&(stNvCsService);
    usDataLen = sizeof(LNAS_LMM_NV_CS_SERVICE_STRU);

    ulRslt = NAS_LMM_NvimRead(          EN_NV_ID_UE_CS_SERVICE,
                                        pData,
                                        &usDataLen);

    if((EN_NV_OK == ulRslt)
        && (stNvCsService.bitOpCsService == NAS_EMM_BIT_SLCT))
    {
        NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_ReadNvCsService: read NV success.",
            stNvCsService.enCsService);

        if ((NAS_LMM_CS_SERVICE_CSFB_SMS <= stNvCsService.enCsService)
            && (NAS_LMM_CS_SERVICE_1xCSFB >= stNvCsService.enCsService))
        {
            pstPubInfo->bitOpCsService = NAS_EMM_BIT_SLCT;
            pstPubInfo->ulCsService = stNvCsService.enCsService;
        }
        else
        {
            NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvCsService:NV Value is Invalid");
            pstPubInfo->bitOpCsService  = NAS_EMM_BIT_NO_SLCT;
            pstPubInfo->ulCsService     = NAS_LMM_CS_SERVICE_BUTT;
        }
    }
    else
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvCsService: read NV err.");
        pstPubInfo->bitOpCsService      = NAS_EMM_BIT_NO_SLCT;
        pstPubInfo->ulCsService         = NAS_LMM_CS_SERVICE_BUTT;
    }

    return;
}

#endif


VOS_VOID  NAS_EMM_ReadNvVoiceDomain(VOS_VOID )
{

    LNAS_LMM_NV_VOICE_DOMAIN_STRU       stNvVoiceDomain;
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;
    VOS_VOID                           *pData = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();
     pData                               = (VOS_VOID *)&(stNvVoiceDomain);
    usDataLen = sizeof(LNAS_LMM_NV_VOICE_DOMAIN_STRU);

    ulRslt = NAS_LMM_NvimRead(          EN_NV_ID_UE_VOICE_DOMAIN,
                                        pData,
                                        &usDataLen);

    /*sunbing 49683 2013-10-28 VoLTE begin*/
    if((EN_NV_OK == ulRslt) && (stNvVoiceDomain.bitOpVoicDomain == NAS_EMM_BIT_SLCT))
    {
        NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_ReadNvVoiceDomain: read NV success.",
            stNvVoiceDomain.enVoicDomain);

        if (NAS_LMM_VOICE_DOMAIN_IMS_PS_PREFERRED >= stNvVoiceDomain.enVoicDomain)
        {
            pstPubInfo->bitOpVoiceDomain = NAS_EMM_BIT_SLCT;
            pstPubInfo->ulVoiceDomain = stNvVoiceDomain.enVoicDomain;
            /* lihong00150010 csfb begin */

            NAS_MML_SetVoiceDomainPreference((VOS_UINT8)pstPubInfo->ulVoiceDomain);

        }
        else
        {
            NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvVoiceDomain:NV Value is Invalid");
            pstPubInfo->bitOpVoiceDomain  = NAS_EMM_BIT_SLCT;
            pstPubInfo->ulVoiceDomain     = NAS_LMM_VOICE_DOMAIN_IMS_PS_PREFERRED;

            NAS_MML_SetVoiceDomainPreference(NAS_LMM_VOICE_DOMAIN_IMS_PS_PREFERRED);

        }
    }
    else
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvVoiceDomain: read NV err.");
        pstPubInfo->bitOpVoiceDomain      = NAS_EMM_BIT_SLCT;
        pstPubInfo->ulVoiceDomain         = NAS_LMM_VOICE_DOMAIN_IMS_PS_PREFERRED;

        NAS_MML_SetVoiceDomainPreference(NAS_LMM_VOICE_DOMAIN_IMS_PS_PREFERRED);

    }
    /*sunbing 49683 2013-10-28 VoLTE end*/

    return;
}
VOS_VOID  NAS_EMM_ReadNvNasRelease(VOS_VOID )
{
    LNAS_LMM_NV_NAS_RELEASE_STRU        stNvNasRelease;
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;
    VOS_VOID                           *pData = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();
    pData                               = (VOS_VOID *)&(stNvNasRelease);
    usDataLen = sizeof(LNAS_LMM_NV_NAS_RELEASE_STRU);

    ulRslt = NAS_LMM_NvimRead(          EN_NV_ID_NAS_RELEASE,
                                        pData,
                                        &usDataLen);

    if((EN_NV_OK == ulRslt)
        && (stNvNasRelease.bitOpReleaseName  == NAS_EMM_BIT_SLCT))
    {

        NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_ReadNvNasRelease: read NV success.",
            stNvNasRelease.ulReleaseName);

        pstPubInfo->ulNasRelease = stNvNasRelease.ulReleaseName;
    }
    else
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvNasRelease: read NV err.");
        pstPubInfo->ulNasRelease        = 0x00000900;

    }

    return;
}

/*sunbing 49683 2013-10-21 VoLTE begin*/
/*****************************************************************************
 Function Name   : NAS_EMM_ReadNvUmtsCodecInfo
 Description     : 从NV项读取Umts codec支持信息
                   参考MN_CALL_FillUmtsSupCodecList,MN_CALL_UmtsCapaProcInit的实现
 Input           : None
 Output          : None
 Return          : None

 History         :
    1.sunbing 49683      2013-10-21  Draft Enact
*****************************************************************************/
VOS_VOID  NAS_EMM_ReadNvUmtsCodecInfo(NAS_EMM_SUPPORT_CODEC_UNION *punCodecList)
{
    VOS_UINT8       aucNvCodecInfo[NAS_EMM_MAX_UMTS_CODEC_TYPE_NUM+1];/*codec个数+具体codec信息*/
    VOS_UINT32      ulDefaultValue;
    VOS_UINT32      i;
    VOS_UINT16      usDataLen;

    NAS_LMM_MEM_SET(aucNvCodecInfo, 0x00, sizeof(aucNvCodecInfo));

    ulDefaultValue          = VOS_FALSE;

    usDataLen               = sizeof(aucNvCodecInfo);

    /*该项NV的结构的原始定义为MN_CALL_UMTS_CODEC_TYPE_STRU，codec个数+具体codec信息*/
    if ( NAS_LMM_NVIM_OK != NAS_LMM_NvimRead(en_NV_Item_UMTS_CODEC_TYPE,
                                   aucNvCodecInfo,
                                   &usDataLen))
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvUmtsCodecInfo: read NV err.");
        ulDefaultValue = VOS_TRUE;
    }

    /* 检查NVIM中的参数是否合法,不合法则使用默认值,个数是否越界 */
    if ( aucNvCodecInfo[0] > NAS_EMM_MAX_UMTS_CODEC_TYPE_NUM )
    {
        ulDefaultValue     = VOS_TRUE;

        aucNvCodecInfo[0]  = NAS_EMM_MAX_UMTS_CODEC_TYPE_NUM;

        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvUmtsCodecInfo: Codec num is err.");

    }

    /* 检查是否有重复定义和超出范围的codec类型*/
    for ( i = 0; i < aucNvCodecInfo[0]; i++ )
    {
        switch ( aucNvCodecInfo[i+1])
        {
            case NAS_EMM_UMTS_CODEC_TYPE_AMR :
                punCodecList->stSupportCodec.CodecType_UMTSAMR     = VOS_TRUE;
                break;

            case NAS_EMM_UMTS_CODEC_TYPE_AMR2 :
                punCodecList->stSupportCodec.CodecType_UMTSAMR2     = VOS_TRUE;
                break;

            case NAS_EMM_UMTS_CODEC_TYPE_AMRWB :
                punCodecList->stSupportCodec.CodecType_UMTSAMRWB   = VOS_TRUE;
                break;

            default:
                NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvUmtsCodecInfo: Codec Type is err.");
                ulDefaultValue = VOS_TRUE;
                break;
        }

        if(ulDefaultValue == VOS_TRUE)
        {
            /*NV内容错误，清空*/
            punCodecList->usSupportCodec = 0;
            break;
        }

    }

    if ( VOS_TRUE == ulDefaultValue )
    {
        punCodecList->stSupportCodec.CodecType_UMTSAMR     = VOS_TRUE;
        punCodecList->stSupportCodec.CodecType_UMTSAMR2    = VOS_TRUE;
    }

}

/*****************************************************************************
 Function Name   : NAS_EMM_ReadNvGsmCodecInfo
 Description     : 从NV项读取Gsm codec支持信息
                   参考MN_CALL_FillGsmSupCodecList,MN_CALL_GsmCapaProcInit的实现
 Input           : None
 Output          : None
 Return          : None

 History         :
    1.sunbing 49683      2013-10-21  Draft Enact
*****************************************************************************/
VOS_VOID  NAS_EMM_ReadNvGsmCodecInfo(NAS_EMM_SUPPORT_CODEC_UNION *punCodecList)
{
    VOS_UINT8       aucNvGsmCodecInfo[NAS_EMM_BC_MAX_SPH_VER_NUM + 2] ;/*codec个数+具体codec信息+1个spare byte*/
    VOS_UINT32      ulDefaultValue;
    VOS_UINT32      i;
    VOS_UINT16      usDataLen;

    NAS_LMM_MEM_SET(aucNvGsmCodecInfo, 0x00, sizeof(aucNvGsmCodecInfo));

    ulDefaultValue          = VOS_FALSE;

    usDataLen               = sizeof(aucNvGsmCodecInfo);

    /*该项NV的结构的原始定义为MN_CALL_NIMV_ITEM_CODEC_TYPE_STRU，codec个数+具体codec信息*/
    if ( NAS_LMM_NVIM_OK != NAS_LMM_NvimRead(en_NV_Item_MED_CODEC_TYPE,
                         aucNvGsmCodecInfo,
                          &usDataLen))
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvGsmCodecInfo: read NV err.");
        ulDefaultValue = VOS_TRUE;
    }

    /* 检查NVIM中的参数是否合法,不合法则使用默认值,个数是否越界 */
    if ( aucNvGsmCodecInfo[0] > NAS_EMM_BC_MAX_SPH_VER_NUM )
    {
        ulDefaultValue = VOS_TRUE;

        aucNvGsmCodecInfo[0] = NAS_EMM_BC_MAX_SPH_VER_NUM;

        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvGsmCodecInfo: Codec num is err.");
    }

    /* 检查是否有重复定义和超出范围的codec类型*/
    for ( i = 0; i < aucNvGsmCodecInfo[0] ; i++ )
    {
        switch ( aucNvGsmCodecInfo[i+1])
        {
            case NAS_EMM_BC_VAL_SPH_VER_FR_1 :
                punCodecList->stSupportCodec.CodecType_GSMFR        = VOS_TRUE;
                break;

            case NAS_EMM_BC_VAL_SPH_VER_FR_2 :
                punCodecList->stSupportCodec.CodecType_GSMEFR       = VOS_TRUE;
                break;

            case NAS_EMM_BC_VAL_SPH_VER_FR_3 :
                punCodecList->stSupportCodec.CodecType_GSMFRAMR     = VOS_TRUE;
                break;

            case NAS_EMM_BC_VAL_SPH_VER_HR_1 :
                punCodecList->stSupportCodec.CodecType_GSMHR        = VOS_TRUE;
                break;

            case NAS_EMM_BC_VAL_SPH_VER_HR_3 :
                punCodecList->stSupportCodec.CodecType_GSMHRAMR     = VOS_TRUE;
                break;

            case NAS_EMM_BC_VAL_SPH_VER_FR_5 :
                punCodecList->stSupportCodec.CodecType_GSMFRAMRWB   = VOS_TRUE;
                break;

            default:
                NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvGsmCodecInfo: Codec Type is err.");
                ulDefaultValue = VOS_TRUE;
                break;

        }

        if(ulDefaultValue == VOS_TRUE)
        {
            /*NV内容错误，清空*/
            punCodecList->usSupportCodec = 0;
            break;
        }
    }

    /* 此处表示需要使用默认值 */
    if ( VOS_TRUE == ulDefaultValue )
    {
        punCodecList->stSupportCodec.CodecType_GSMFR        = VOS_TRUE;
        punCodecList->stSupportCodec.CodecType_GSMEFR       = VOS_TRUE;
        punCodecList->stSupportCodec.CodecType_GSMFRAMR     = VOS_TRUE;
        punCodecList->stSupportCodec.CodecType_GSMHR        = VOS_TRUE;
        punCodecList->stSupportCodec.CodecType_GSMHRAMR     = VOS_TRUE;
        punCodecList->stSupportCodec.CodecType_GSMFRAMRWB   = VOS_TRUE;
    }
}

/*****************************************************************************
 Function Name   : NAS_EMM_ReadNvCodecSupportList
 Description     : 从NV项读取GU codec支持list信息
 Input           : None
 Output          : None
 Return          : None

 History         :
    1.sunbing 49683      2013-10-21  Draft Enact
*****************************************************************************/
VOS_VOID  NAS_EMM_ReadNvCodecSupportList(VOS_VOID )
{
    NAS_LMM_PUB_INFO_STRU               *pstPubInfo;
    NAS_EMM_SUPPORT_CODEC_UNION          unCodec;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    pstPubInfo->bitOpSupCodecList               = NAS_EMM_BIT_SLCT;
    pstPubInfo->stSupCodecList.ucLenOfCodecList = 0x08;

     /* 设置支持的列表，目前填入了GSM和UMTS支持的codec list,
       00 表示GSM, 04表示UMTS */
    /* 具体赋值的含义参见26.103 */
    NAS_LMM_MEM_SET(&unCodec,0x00,sizeof(unCodec));
    NAS_EMM_ReadNvUmtsCodecInfo(&unCodec);
    pstPubInfo->stSupCodecList.astCodec[0].ucSysId = 0x04;
    pstPubInfo->stSupCodecList.astCodec[0].ucLenOfBitmap = 0x02;
    pstPubInfo->stSupCodecList.astCodec[0].aucCodecBitmap[0] = (VOS_UINT8)(unCodec.usSupportCodec);
    pstPubInfo->stSupCodecList.astCodec[0].aucCodecBitmap[1] = (VOS_UINT8)(unCodec.usSupportCodec >> 8);

    NAS_LMM_MEM_SET(&unCodec,0x00,sizeof(unCodec));
    NAS_EMM_ReadNvGsmCodecInfo(&unCodec);
    pstPubInfo->stSupCodecList.astCodec[1].ucSysId = 0x00;
    pstPubInfo->stSupCodecList.astCodec[1].ucLenOfBitmap = 0x02;
    pstPubInfo->stSupCodecList.astCodec[1].aucCodecBitmap[0] = (VOS_UINT8)(unCodec.usSupportCodec);
    pstPubInfo->stSupCodecList.astCodec[1].aucCodecBitmap[1] = (VOS_UINT8)(unCodec.usSupportCodec >> 8);

}
/*sunbing 49683 2013-10-21 VoLTE end*/

/*self-adaption NW cause modify begin for leili*/

VOS_VOID  NAS_EMM_ReadNvConfigNwCause(VOS_VOID )
{
    LNAS_LMM_NV_CONFIG_NWCAUSE_STRU    stNvConfigCause;
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;
    VOS_VOID                           *pData = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();
    pData                               = (VOS_VOID *)&(stNvConfigCause);
    usDataLen = sizeof(LNAS_LMM_NV_CONFIG_NWCAUSE_STRU);

    ulRslt = NAS_LMM_NvimRead(          EN_NV_ID_CONFIG_NWCAUSE,
                                        pData,
                                        &usDataLen);

    if((EN_NV_OK == ulRslt)
        && (stNvConfigCause.ucCauseNum > NAS_EMM_NULL))
    {

        NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_ReadNvConfigNwCause: read NV success, CauseNum=",
            stNvConfigCause.ucCauseNum);
        if ((NAS_EMM_NULL == stNvConfigCause.ucCauseNum)
            || (NAS_LMM_MAX_CAUSE_NUM < stNvConfigCause.ucCauseNum))
        {
            NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvConfigNwCause: config NV err.");
            return;
        }

        pstPubInfo->stConfCause.ucCauseNum = stNvConfigCause.ucCauseNum;
        NAS_LMM_MEM_CPY(pstPubInfo->stConfCause.astAdaptCause,
                        stNvConfigCause.astAdaptCause,
                        sizeof(NAS_LMM_ADAPTION_CAUSE_STRU) * NAS_LMM_MAX_CAUSE_NUM);
    }
    else
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvConfigNwCause: read NV err.");
        pstPubInfo->stConfCause.ucCauseNum = 0;
        NAS_LMM_MEM_SET(pstPubInfo->stConfCause.astAdaptCause,
                        0,
                        sizeof(NAS_LMM_ADAPTION_CAUSE_STRU) * NAS_LMM_MAX_CAUSE_NUM);

    }
    return;
}

/*self-adaption NW cause modify end for leili*/

VOS_VOID  NAS_EMM_SetLteUeUsageSetting2Mml
(
    NAS_EMM_USAGE_SETTING_UINT32        enUeUsageSetting
)
{
    NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_SetLteUeUsageSetting2Mml: ue's usage setting:",
                           enUeUsageSetting);

    NAS_MML_SetLteUeUsageSetting((VOS_UINT8)enUeUsageSetting);
}


VOS_VOID  NAS_EMM_ReadNvUeSetting( VOS_VOID )
{
    VOS_VOID                           *pData           = VOS_NULL_PTR;
    LNAS_LMM_NV_UE_CENTER_STRU          stNvUeCenter    = {0};
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo      = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt          = NAS_EMM_NULL;
    VOS_UINT16                          usDataLen       = NAS_EMM_NULL;

    pstPubInfo  = NAS_LMM_GetEmmInfoAddr();
    pData       = (VOS_VOID *)&(stNvUeCenter);
    usDataLen   = sizeof(LNAS_LMM_NV_UE_CENTER_STRU);

    ulRslt      = NAS_LMM_NvimRead(     EN_NV_ID_UE_CENTER,
                                        pData,
                                        &usDataLen);

    if((EN_NV_OK == ulRslt)
        && (stNvUeCenter.bitOpUeCenter == NAS_EMM_BIT_SLCT))
    {
        NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_ReadNvUeSetting: read NV success.",
            stNvUeCenter.enUeCenter);

        if (EMM_USAGE_SETTING_BUTT > stNvUeCenter.enUeCenter)
        {
            pstPubInfo->ulUsageSetting  = stNvUeCenter.enUeCenter;
        }
        else
        {
            NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvUeSetting:NV Value is Invalid");
            pstPubInfo->ulUsageSetting  = EMM_SETTING_DATA_CENTRIC;
        }
    }
    else
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ReadNvUeSetting: read NV err.");
        pstPubInfo->ulUsageSetting      = EMM_SETTING_DATA_CENTRIC;
    }

    NAS_EMM_SetLteUeUsageSetting2Mml(pstPubInfo->ulUsageSetting);

    return;
}



VOS_VOID    NAS_EMM_PubmInfoSupCodecListInit( VOS_VOID )
{
    NAS_LMM_PUB_INFO_STRU               *pstPubInfo;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    /*sunbing 49683 2013-10-14 VoLTE begin*/
    /*初始化设置为无效，后续读取NV，会进一步赋值*/
    pstPubInfo->bitOpSupCodecList       = NAS_EMM_BIT_NO_SLCT;

     /* 设置支持的列表，目前填入了GSM和UMTS支持的codec list,
       00 表示GSM, 04表示UMTS */
    /* 具体赋值的含义参见26.103 */
    /*
    pstPubInfo->stSupCodecList.ucLenOfCodecList = 0x08;
    pstPubInfo->stSupCodecList.astCodec[0].ucSysId = 0x04;
    pstPubInfo->stSupCodecList.astCodec[0].ucLenOfBitmap = 0x02;
    pstPubInfo->stSupCodecList.astCodec[0].aucCodecBitmap[0] = 0x60;
    pstPubInfo->stSupCodecList.astCodec[0].aucCodecBitmap[1] = 0x00;
    pstPubInfo->stSupCodecList.astCodec[1].ucSysId = 0x00;
    pstPubInfo->stSupCodecList.astCodec[1].ucLenOfBitmap = 0x02;
    pstPubInfo->stSupCodecList.astCodec[1].aucCodecBitmap[0] = 0x1F;
    pstPubInfo->stSupCodecList.astCodec[1].aucCodecBitmap[1] = 0x00;
    */
    /*sunbing 49683 2013-10-14 VoLTE end*/

    return;
}
VOS_VOID     NAS_EMM_PubmInfoBearerContextInit(VOS_VOID)
{
    /*leili modify for isr begin*/
#if 0
    g_stEmmInfo.stEpsBearerCxt.enStatusChange   = NAS_EMM_EPS_BEARER_STATUS_NOCHANGE;
    g_stEmmInfo.stEpsBearerCxt.stEpsContextStatus.ulEpsIdNum = 0;
    NAS_LMM_MEM_SET(g_stEmmInfo.stEpsBearerCxt.stEpsContextStatus.aulEsmEpsId,
                    0, EMM_ESM_MAX_EPS_BEARER_NUM*sizeof(VOS_UINT32));
#endif
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PubmInfoBearerContextInit: GET MML PS BEARER INFO:");
    NAS_COMM_PrintArray(                NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                        *EMM_ESM_MAX_EPS_BEARER_NUM);
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    NAS_LMM_MEM_SET(                    NAS_EMM_GetPsBearerCtx(),
                                        0,
                                        EMM_ESM_MAX_EPS_BEARER_NUM*
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU));

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PubmInfoBearerContextInit: UPDATE MML PS BEARER INFO:");
    NAS_COMM_PrintArray(                NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                        *EMM_ESM_MAX_EPS_BEARER_NUM);

    /*PC REPLAY MODIFY BY LEILI END*/
    /*leili modify for isr end*/
    return;
}


VOS_VOID    NAS_EMM_FsmReg( VOS_VOID )
{
    VOS_UINT32                      ulRslt = NAS_LMM_FAIL;

    VOS_UINT32                      ulSizeof1;
    VOS_UINT32                      ulSizeof2;

    ulSizeof1                       = g_ulSizeof_g_astEmmStaTbl;
    ulSizeof2                       = sizeof(NAS_LMM_STA_STRU);

    /* 注册RRC_FSM_CESL状态机 */
    ulRslt = NAS_LMM_FsmRegisterFsm(NAS_LMM_PARALLEL_FSM_EMM,
                                    (VOS_UINT32)(ulSizeof1 / ulSizeof2),
                                    g_astEmmStaTbl,
                                    NAS_EMM_FsmErr);
    if (ulRslt != NAS_LMM_SUCC)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_FsmReg, register fail!");
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_EMM_FsmReg, register SUCC!");
    }

    return;
}
VOS_UINT32  NAS_EMM_FsmErr(             VOS_UINT32           ulMsgId,
                                                VOS_VOID             *pMsg )
{
/*    NAS_LMM_PUBM_LOG2_ERR("NAS_EMM_FsmErr: ", ulMsgId, pMsg);*/
    (VOS_VOID)(ulMsgId);
    (VOS_VOID)(pMsg);
    return  NAS_LMM_MSG_DISCARD;
}
VOS_VOID    NAS_EMM_FSM_PushState(VOS_VOID)
{
    NAS_LMM_FSM_PushStackCombine(NAS_LMM_PARALLEL_FSM_EMM);
    return;
}


VOS_VOID    NAS_EMM_FSM_PopState(VOS_VOID)
{
    NAS_LMM_FSM_PopStackCombine(NAS_LMM_PARALLEL_FSM_EMM);
    return;
}


VOS_VOID    NAS_EMM_FSM_PopAllState(VOS_VOID)
{
    VOS_UINT32                          ulStackCurDepth;
    NAS_LMM_FSM_STATE_STACK_STRU        *pstFsmStack;        /* 状态机栈地址 */


    /* 获取EMM状态机当前状态的地址和状态机栈的地址 */
    pstFsmStack = NAS_LMM_GetFsmStackAddr(NAS_LMM_PARALLEL_FSM_EMM);
    ulStackCurDepth = pstFsmStack->ucStackDepth;
    NAS_LMM_PUBM_LOG1_INFO("NAS_EMM_FSM_PopAllState: ulStackCurDepth = ", ulStackCurDepth);

    for(; 0 < ulStackCurDepth; ulStackCurDepth-- )
    {
        NAS_LMM_FSM_PopStackCombine(NAS_LMM_PARALLEL_FSM_EMM);
    }

    return;
}


VOS_VOID    NAS_EMM_FSM_ClearState(VOS_VOID)
{
    NAS_LMM_FSM_ClearStackCombine(NAS_LMM_PARALLEL_FSM_EMM);
    return;
}



VOS_UINT32  NAS_LMM_ReadSimImsi(VOS_VOID)
{
    VOS_VOID                            *pData;
    LNAS_LMM_NV_IMSI_STRU               stNvImsi;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                          *pucImsi;
    VOS_UINT16                          usDataLen;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadSimImsi is entered!");


    /*read Imsi from Nvim*/
    pData                               = &(stNvImsi);
    usDataLen                           = sizeof(LNAS_LMM_NV_IMSI_STRU);
    ulRslt = NAS_LMM_NvimRead(           EN_NV_ID_IMSI_FILE,
                                        pData,
                                        &usDataLen);


    /*process the result of reading,具体处理方式有待需求确定*/
    if((NAS_LMM_NVIM_OK == ulRslt) && (NAS_EMM_BIT_SLCT == stNvImsi.bitOpImsi))
    {
        NAS_LMM_GetEmmInfoUeidAddr()->bitOpImsi = stNvImsi.bitOpImsi;
        pucImsi = (VOS_UINT8 *)NAS_LMM_GetEmmInfoUeidImsiAddr();
        NAS_LMM_MEM_CPY(                 pucImsi,
                                        stNvImsi.aucImsi,
                                        NAS_LMM_NVIM_IMSI_FILE_LEN);
        return  NAS_LMM_SUCC;
    }
    else
    {
        NAS_LMM_GetEmmInfoUeidAddr()->bitOpImsi = NAS_EMM_BIT_NO_SLCT;
        return  NAS_LMM_FAIL;
    }


}



VOS_UINT32  NAS_LMM_MatchNvImsi( VOS_VOID )
{
    VOS_VOID                           *pData;
    LNAS_LMM_NV_IMSI_STRU               stNvImsi;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;


    /*read Imsi from Nvim*/
    pData                               = &(stNvImsi);
    usDataLen                           = sizeof(LNAS_LMM_NV_IMSI_STRU);
    ulRslt = NAS_LMM_NvimRead(           EN_NV_ID_IMSI,
                                        pData,
                                        &usDataLen);

    if((NAS_LMM_NVIM_OK == ulRslt) && (NAS_EMM_BIT_SLCT == stNvImsi.bitOpImsi))
    {
        if (0 == NAS_LMM_MEM_CMP((const VOS_VOID *) g_stEmmInfo.stMmUeId.aucImsi,
                         (const VOS_VOID *) stNvImsi.aucImsi,
                         NAS_LMM_NVIM_IMSI_FILE_LEN))
        {
            NAS_LMM_PUBM_LOG_INFO("NAS_LMM_MatchNvImsi: IMSI EQUAL. ");
            return NAS_EMM_SUCC;

        }
    }

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_MatchNvImsi: IMSI DIFF. ");
    return NAS_EMM_FAIL;
}
VOS_VOID  NAS_LMM_UpdateNvImsi(const VOS_UINT8  *pucImsi )
{

    VOS_UINT32                          ulRst;
    VOS_VOID                           *pData;
    LNAS_LMM_NV_IMSI_STRU               stNvImsi;
    VOS_UINT16                          usDataLen;
    NAS_LMM_UEID_STRU                    *pstUeId;

    NAS_LMM_NVIM_LOG_INFO("NAS_LMM_UpdateNvImsi is entered");

    NAS_LMM_MEM_SET(&stNvImsi, 0, sizeof(LNAS_LMM_NV_IMSI_STRU));

    pstUeId                             = NAS_LMM_GetEmmInfoUeidAddr();

    /*填充新的IMSI*/
    stNvImsi.bitOpImsi                  = pstUeId->bitOpImsi;
    NAS_LMM_MEM_CPY(                    stNvImsi.aucImsi,
                                        pucImsi,
                                        NAS_LMM_NVIM_IMSI_FILE_LEN);

    /*write new IMSI to NVIM*/
    pData                               = &stNvImsi;
    usDataLen                           = sizeof(LNAS_LMM_NV_IMSI_STRU);

    ulRst = NAS_LMM_NvimWrite(           EN_NV_ID_IMSI,
                                        pData,
                                        usDataLen);
    if(EN_NV_OK != ulRst)
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_UpdateNvImsi: nvim ERR!!");
        return;
    }

    return;
}



VOS_VOID NAS_LMM_ReadNvimImei(VOS_VOID)
{
    VOS_UINT8                           aucDefImei[NAS_MM_MAX_UEID_BUF_SIZE] = {0};

    VOS_UINT8                           aucBuf[NV_ITEM_IMEI_SIZE] = {0};

    VOS_UINT32                          i = 0;

    /*IMEI读取失败，则使用缺省值*/
    if(ERR_MSP_SUCCESS != SC_PERS_NvRead(MODEM_ID_0, en_NV_Item_IMEI, aucBuf, NV_ITEM_IMEI_SIZE))
    {
        NAS_LMM_MEM_SET(aucDefImei, 0, NAS_MM_MAX_UEID_BUF_SIZE);
        aucDefImei[0] = 0x08;
        aucDefImei[1] = 0x3A;
        aucDefImei[2] = 0x65;
        aucDefImei[3] = 0x26;
        aucDefImei[4] = 0x06;
        aucDefImei[5] = 0x70;
        aucDefImei[6] = 0x37;
        aucDefImei[7] = 0x71;
        aucDefImei[8] = 0x04;
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_ReadNvimImei: Read IMEI from NV fail!");
    }
    else
    {
        /* NV读取成功则重新计算IMEI校验位，并转换为LNAS内部IMEI号存储格式 */
        aucDefImei[0] = 0x08;
        aucDefImei[1] = 0x0A;
#if 0
        /* 重新计算IMEI校验位 */
        ucCheckData = 0;
        for (i = 0; i < (NV_ITEM_IMEI_SIZE - 2); i += 2)
        {
            ucCheckData += (aucBuf[i] + ((aucBuf[i+1] + aucBuf[i+1])/10)
                                       +((aucBuf[i+1] + aucBuf[i+1])%10));
        }

        ucCheckData = (10 - (ucCheckData % 10)) % 10;
        aucBuf[NV_ITEM_IMEI_SIZE - 2] = ucCheckData;
#endif
        /* 将NV中格式转变为LNAS内部IMEI存放格式 */
        for (i = 0; i < (NV_ITEM_IMEI_SIZE - 1); i++)
        {
             aucDefImei[((i + 1) / 2) + 1] |= (VOS_UINT8)(aucBuf[i] << (((i + 1) % 2) * NAS_LMM_MOVEMENT_4_BITS));
        }
    }

    NAS_LMM_GetEmmInfoUeidAddr()->bitOpImei = NAS_EMM_BIT_SLCT;
    NAS_LMM_MEM_CPY(                NAS_LMM_GetEmmInfoUeidImeiAddr(),
                                    aucDefImei,
                                    NAS_MM_MAX_UEID_BUF_SIZE);
    /* 打印获取到的IMEI值 */
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadNvimImei: IMEI value is :");
    NAS_COMM_PrintArray(NAS_COMM_GET_MM_PRINT_BUF(),
                   NAS_LMM_GetEmmInfoUeidImeiAddr(),
                   NAS_MM_MAX_UEID_BUF_SIZE);

    return;
}





VOS_VOID  NAS_LMM_ReadNvEpsSecContext(VOS_VOID )
{
    LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU    stNvEpsSecContext;
    NAS_LMM_SECU_CONTEXT_STRU            stSecuCurContext;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadNvEpsSecContext is entered!");

    /*read  from Nvim*/
    usDataLen                           = sizeof(LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU);
    ulRslt = NAS_LMM_NvimRead(           EN_NV_ID_SEC_CONTEXT,
                                        (VOS_VOID *)&(stNvEpsSecContext),
                                        &usDataLen);

    /* 从NV的数据结构向 NAS 核心结构转换 */
    NAS_LMM_MEM_SET(                     &stSecuCurContext,
                                        0,
                                        sizeof(NAS_LMM_SECU_CONTEXT_STRU));

    NAS_LMM_MEM_SET( NAS_LMM_GET_USIM_EPS_SECU_CONTEXT_ADDR(),
                     0,
                     NAS_NVIM_SECU_CONTEXT_MAX_LEN);

    /*process the result of reading,具体处理方式有待需求确定*/
    if((NAS_LMM_NVIM_OK == ulRslt) && (NAS_EMM_BIT_SLCT == stNvEpsSecContext.bitOpEpsSec))
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadNvEpsSecContext: read NV Succ.");

        /* 将NV中的安全上下文码流保存在上次写卡的全局变量中，用于下次写卡时的比较 */
        NAS_LMM_MEM_CPY( NAS_LMM_GET_USIM_EPS_SECU_CONTEXT_ADDR(),
                         (VOS_VOID *)&(stNvEpsSecContext),
                         sizeof(LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU));


        /*当前安全上下文结构赋值*/
        stSecuCurContext.ucKSIasme      = stNvEpsSecContext.ucKSIasme;

        NAS_LMM_MEM_CPY(                 stSecuCurContext.aucKasme,
                                        stNvEpsSecContext.aucKasme,
                                        NAS_MM_AUTH_KEY_ASME_LEN);
        stSecuCurContext.ulUlNasCount   = stNvEpsSecContext.ulUlNasCount;
        stSecuCurContext.ulDlNasCount   = stNvEpsSecContext.ulDlNasCount;
        stSecuCurContext.ucSecuAlg      = stNvEpsSecContext.ucSecuAlg;

        /*调用安全上下文更新函数,并计算NasCK ,NasIk*/
        NAS_EMM_SecuSetCurCntxt(&stSecuCurContext);
    }
    else
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_ReadNvEpsSecContext: read NV Fail!");
    }

    return;
}



VOS_VOID  NAS_LMM_ReadSimEpsSecContext( VOS_VOID )
{
    VOS_VOID                            *pData;
    LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU    stNvEpsSecContext;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadSimEpsSecContext is entered!");


    /*read ESP SECURITY CONTEXT from USIM*/
    pData                               = &(stNvEpsSecContext);
    usDataLen                           = sizeof(LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU);
    ulRslt = NAS_LMM_NvimRead(           EN_NV_ID_SEC_CONTEXT_FILE,
                                        pData,
                                        &usDataLen);

    if (NAS_LMM_NVIM_HARD_USIM == ulRslt)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_ReadSimEpsSecContext: read usim EPS SEC CONTEXT FILE!");
    }

    return;
}
VOS_VOID  NAS_LMM_ReadNvPsLoc( VOS_VOID )
{
    VOS_VOID                            *pData;
    LNAS_LMM_NV_EPS_LOC_STRU            stNvPsLoc;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;
    NAS_LMM_UEID_STRU                    *pstUeId;
    NAS_MM_NETWORK_ID_STRU              stMmNetId;
    VOS_UINT8                          *pstLastPsLoc;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadNvPsLoc is entered!");

    pstUeId                             = NAS_LMM_GetEmmInfoUeidAddr();
    pstLastPsLoc                        =NAS_LMM_GetEmmInfoLastWriteUsimPsLocAddr();

    /*read EPS PS LOC from Nvim*/
    pData                               = &(stNvPsLoc);
    usDataLen                           = sizeof(LNAS_LMM_NV_EPS_LOC_STRU);
    ulRslt = NAS_LMM_NvimRead(           EN_NV_ID_EPS_LOC,
                                        pData,
                                        &usDataLen);

    /*process the result of reading,具体处理方式有待需求确定*/
    if(NAS_LMM_NVIM_OK == ulRslt)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadNvPsLoc: read NV Succ.");

        /* 读NV成功，将本次读NV的码流保存到上次成功写卡的全局变量中 */
        NAS_LMM_MEM_CPY( pstLastPsLoc,
                         pData,
                         sizeof(LNAS_LMM_NV_EPS_LOC_STRU));

        /*GUTI*/
        if (NAS_EMM_BIT_SLCT == stNvPsLoc.bitOpGuti)
        {
            NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadNvPsLoc: GUTI VALID");
            pstUeId->bitOpGuti          = NAS_EMM_BIT_SLCT;
            NAS_LMM_MEM_CPY(            &pstUeId->stGuti,
                                        &(stNvPsLoc.stGuti),
                                        sizeof(NAS_LMM_GUTI_STRU));
        }

        /*L.R.V TAI*/
        if (NAS_EMM_BIT_SLCT == stNvPsLoc.bitOpTai)
        {
            NAS_LMM_MEM_CPY(            &(stMmNetId.stPlmnId),
                                        &(stNvPsLoc.stLastRegTai.stPlmnId),
                                        sizeof(NAS_MM_PLMN_ID_STRU));

            NAS_LMM_MEM_CPY(            &(stMmNetId.stTac),
                                        &(stNvPsLoc.stLastRegTai.stTac),
                                        sizeof(NAS_MM_TAC_STRU));


            /*PLMN有效时,更新全局变量*/
            if (VOS_FALSE == NAS_LMM_CheckPlmnIsInvalid(&stMmNetId.stPlmnId))
            {
                NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadNvPsLoc: L.V.R TAI PLMN VALID");
                if (VOS_TRUE == NAS_LMM_TacIsUnexist(&(stMmNetId.stTac)))
                {
                    NAS_LMM_UndefTac(&(stMmNetId.stTac));
                }
                NAS_EMM_SetLVRTai(&stMmNetId);
            }
            else
            {
                NAS_EMM_ClearLVRTai();
            }

        }

        /*UPDATE STATUS*/
        if (NAS_EMM_BIT_SLCT == stNvPsLoc.bitOpUpState)
        {
            NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadNvPsLoc: EU VALID");
            NAS_EMM_MAIN_CONTEXT_SET_AUX_UPDATE_STAE((VOS_UINT8)stNvPsLoc.enUpdateState);

        }
    }
    else
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_ReadNvPsLoc: read NV Fail!");
        NAS_EMM_ClearGuti();
        NAS_EMM_ClearLVRTai();
    }

    return;
}
VOS_VOID  NAS_LMM_ReadSimPsLoc( VOS_VOID )
{
    VOS_VOID                            *pData;
    LNAS_LMM_NV_EPS_LOC_STRU            stNvPsLoc;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadSimPsLoc is entered!");

    /*read ESP PS LOC from USIM*/
    pData                               = &(stNvPsLoc);
    usDataLen                           = sizeof(LNAS_LMM_NV_EPS_LOC_STRU);
    ulRslt = NAS_LMM_NvimRead(           EN_NV_ID_EPS_LOC_FILE,
                                        pData,
                                        &usDataLen);

    if (NAS_LMM_NVIM_HARD_USIM == ulRslt)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_ReadSimPsLoc: read usim PS LOC FILE!");
    }
}
VOS_VOID  NAS_LMM_WriteSimEpsSecurityContext(NAS_NV_ITEM_OP_TYPE_ENUM_UINT32 enOpType)
{
    VOS_UINT32                          ulRst;
    LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU    stNvSecContext;
    NAS_LMM_SECU_CONTEXT_STRU           *pstCurSecuCntxt = VOS_NULL_PTR;

    NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteSimEpsSecurityContext is entered.");

    NAS_LMM_MEM_SET(&stNvSecContext, 0, sizeof(LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU));

    if(NAS_NV_ITEM_UPDATE == enOpType)
    {
        /*获取当前安全上下文*/
        pstCurSecuCntxt = NAS_EMM_SecuGetCurCntxtAddr();

        /* 如果算法是EIA0，则不写卡，直接退出 */
        if (NAS_EMM_SECU_ALG_EIA0 == (pstCurSecuCntxt->ucSecuAlg & NAS_EMM_LOW_3_BITS_F))
        {
            NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteSimEpsSecurityContext:EIA0");
            return ;
        }

        /* 如果注册状态为紧急注册，则不写卡，直接退出 */
        if (NAS_LMM_REG_STATUS_EMC_REGED == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteSimEpsSecurityContext:emc regged");
            return ;
        }

        /*填充写入USIM的数据*/
        stNvSecContext.ucKSIsgsn        = pstCurSecuCntxt->ucKSIsgsn;
        stNvSecContext.ucKSIasme        = pstCurSecuCntxt->ucKSIasme;
        stNvSecContext.ucSecuAlg        = pstCurSecuCntxt->ucSecuAlg;
        stNvSecContext.ulUlNasCount     = pstCurSecuCntxt->ulUlNasCount;
        stNvSecContext.ulDlNasCount     = pstCurSecuCntxt->ulDlNasCount;
        NAS_LMM_MEM_CPY(                 stNvSecContext.aucKasme,
                                        pstCurSecuCntxt->aucKasme,
                                        NAS_LMM_SECU_AUTH_KEY_ASME_LEN);
    }
    else
    {
        NAS_LMM_MEM_SET(&stNvSecContext, (VOS_CHAR)0xff, sizeof(LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU));
    }

    /*write EPS SECURITY CONTEXT to USIM*/
    ulRst = NAS_LMM_NvimWrite(           EN_NV_ID_SEC_CONTEXT_FILE,
                                        &stNvSecContext,
                                        sizeof(LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU));

    if( NAS_LMM_NVIM_WRITTEN_UNNEEDED == ulRst)
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteSimEpsSecurityContext: The value is already in usim, return.");
        return;
    }

    if(EN_NV_OK != ulRst)
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_WriteSimEpsSecurityContext: Write Sim ERR!!");
    }

    return;
}
VOS_VOID  NAS_LMM_WriteNvEpsSecurityContext( NAS_NV_ITEM_OP_TYPE_ENUM_UINT32 enOpType )
{
    VOS_UINT32                          ulRst;
    LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU    stNvSecContext;
    NAS_LMM_SECU_CONTEXT_STRU           *pstCurSecuCntxt = VOS_NULL_PTR;

    NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteNvEpsSecurityContext entered.");

    NAS_LMM_MEM_SET(&stNvSecContext, 0, sizeof(LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU));

    if (NAS_NV_ITEM_UPDATE == enOpType)
    {
        /*获取当前安全上下文*/
        pstCurSecuCntxt = NAS_EMM_SecuGetCurCntxtAddr();

        /* 如果算法是EIA0，则不写NV，直接退出 */
        if (NAS_EMM_SECU_ALG_EIA0 == (pstCurSecuCntxt->ucSecuAlg & NAS_EMM_LOW_3_BITS_F))
        {
            NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteNvEpsSecurityContext:EIA0");
            return ;
        }

        /* 如果注册状态为紧急注册，则不写NV，直接退出 */
        if (NAS_LMM_REG_STATUS_EMC_REGED == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteNvEpsSecurityContext:emc regged");
            return ;
        }

        /*填充写入USIM的数据*/
        stNvSecContext.bitOpEpsSec      = NAS_NVIM_BIT_SLCT;

        stNvSecContext.ucKSIsgsn        = pstCurSecuCntxt->ucKSIsgsn;
        stNvSecContext.ucKSIasme        = pstCurSecuCntxt->ucKSIasme;
        stNvSecContext.ucSecuAlg        = pstCurSecuCntxt->ucSecuAlg;
        stNvSecContext.ulUlNasCount     = pstCurSecuCntxt->ulUlNasCount;
        stNvSecContext.ulDlNasCount     = pstCurSecuCntxt->ulDlNasCount;
        NAS_LMM_MEM_CPY(                 stNvSecContext.aucKasme,
                                        pstCurSecuCntxt->aucKasme,
                                        NAS_LMM_SECU_AUTH_KEY_ASME_LEN);
    }
    else
    {
        stNvSecContext.bitOpEpsSec      = NAS_NVIM_BIT_SLCT;
        stNvSecContext.ucKSIasme        = NAS_LMM_NKSI_NO_KEY_IS_AVAILABLE;
        stNvSecContext.ucKSIsgsn        = NAS_LMM_NKSI_NO_KEY_IS_AVAILABLE;
    }

    /* 如果NV项中保存的与本次要写入的完全相同，则无需写入 */
    if((0 == NAS_LMM_MEM_CMP( (const VOS_VOID *)NAS_LMM_GET_USIM_EPS_SECU_CONTEXT_ADDR(),
                              (const VOS_VOID *)&stNvSecContext,
                              sizeof(LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU))))
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteNvEpsSecurityContext:  Don't need to write Nvim.");
        return;
    }


    /*write EPS SECURITY CONTEXT to NVIM*/
    ulRst = NAS_LMM_NvimWrite(       EN_NV_ID_SEC_CONTEXT,
                                    &stNvSecContext,
                                    sizeof(LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU));
    if(EN_NV_OK != ulRst)
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_WriteNvEpsSecurityContext: Write nvim Err!");
    }
    else
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteNvEpsSecurityContext:Write Nvim SUCC, update last write NV EpsSecurityContext.");

        /* 写NV成功需将本次写NV内容更新到上次写卡EpsSecurityContext信息中 */
        NAS_LMM_MEM_SET(NAS_LMM_GET_USIM_EPS_SECU_CONTEXT_ADDR(),
                        0,
                        NAS_NVIM_SECU_CONTEXT_MAX_LEN);

        NAS_LMM_MEM_CPY(NAS_LMM_GET_USIM_EPS_SECU_CONTEXT_ADDR(),
                        &stNvSecContext,
                        sizeof(LNAS_LMM_NV_EPS_SEC_CONTEXT_STRU));
    }


    return;
}
VOS_VOID  NAS_LMM_WriteNvPsLoc
(
    NAS_NV_ITEM_OP_TYPE_ENUM_UINT32     enOpType
)
{
    VOS_UINT32                          ulRst;
    LNAS_LMM_NV_EPS_LOC_STRU            stNvPsLoc = {0};
    NAS_LMM_UEID_STRU                    *pstUeId;
    NAS_MM_NETWORK_ID_STRU              *pstNetId;
    VOS_UINT8                          *pstLastPsLoc;

    NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteNvPsLoc is entered.");

    pstLastPsLoc                        =NAS_LMM_GetEmmInfoLastWriteUsimPsLocAddr();

    /*填充新的GUTI*/
    if (NAS_NV_ITEM_UPDATE == enOpType)
    {
        pstUeId                         = NAS_LMM_GetEmmInfoUeidAddr();
        pstNetId                        = NAS_LMM_GetEmmInfoNetInfoLastRegNetAddr();
        stNvPsLoc.bitOpGuti             = pstUeId->bitOpGuti;
        stNvPsLoc.bitOpTai              = NAS_EMM_GetLVRTaiOpBit();
        stNvPsLoc.bitOpUpState          = NAS_NVIM_BIT_SLCT;

        NAS_LMM_MEM_CPY(                &(stNvPsLoc.stGuti),
                                        &(pstUeId->stGuti),
                                        sizeof(NAS_LMM_GUTI_STRU));


        NAS_LMM_MEM_CPY(                &(stNvPsLoc.stLastRegTai.stPlmnId),
                                        &(pstNetId->stPlmnId),
                                        sizeof(NAS_MM_PLMN_ID_STRU));

        NAS_LMM_MEM_CPY(                &(stNvPsLoc.stLastRegTai.stTac),
                                        &(pstNetId->stTac),
                                        sizeof(NAS_MM_TAC_STRU));

        stNvPsLoc.enUpdateState         = NAS_EMM_UPDATE_STAE;
    }
    else
    {
        NAS_LMM_MEM_SET(&stNvPsLoc, 0, sizeof(LNAS_LMM_NV_EPS_LOC_STRU));

        NAS_LMM_UndefPlmn(&stNvPsLoc.stLastRegTai.stPlmnId);
        NAS_LMM_UndefTac(&stNvPsLoc.stLastRegTai.stTac);

    }

    /* 如果NV项中保存的与本次要写入的完全相同，则无需写入 */
    if((0 == NAS_LMM_MEM_CMP( (const VOS_VOID *)pstLastPsLoc,
                              (const VOS_VOID *)&stNvPsLoc,
                              sizeof(LNAS_LMM_NV_EPS_LOC_STRU))))
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteNvPsLoc:  Don't need to write Nvim.");
        return;
    }

    /*write new PS LOC to NVIM*/
    ulRst = NAS_LMM_NvimWrite(           EN_NV_ID_EPS_LOC,
                                        &stNvPsLoc,
                                        sizeof(LNAS_LMM_NV_EPS_LOC_STRU));
    if(EN_NV_OK != ulRst)
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_WriteNvPsLoc: nvim ERR!!");
    }
    else
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteNvPsLoc:Write Nvim SUCC, update last write NV EPS LOC  info.");

        /* 写NV成功需将本次写NV内容更新到上次写卡PS LOC信息中 */
        NAS_LMM_MEM_CPY( pstLastPsLoc,
                         &stNvPsLoc,
                         sizeof(LNAS_LMM_NV_EPS_LOC_STRU));
    }

    return;
}


VOS_VOID  NAS_LMM_WriteSimPsLoc( VOS_VOID )
{

    LNAS_LMM_NV_EPS_LOC_STRU             stNvPsLoc;
    NAS_LMM_UEID_STRU                    *pstUeid;
    NAS_MM_GUTI_STRU                    *pstGuti;
    NAS_MM_NETWORK_ID_STRU              *pstNetId;
    VOS_UINT32                           ulRslt;

    NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteSimPsLoc is entered.");

    NAS_LMM_MEM_SET(&stNvPsLoc, 0, sizeof(LNAS_LMM_NV_EPS_LOC_STRU));

    pstUeid                             = NAS_LMM_GetEmmInfoUeidAddr();
    pstGuti                             = NAS_LMM_GetEmmInfoUeidGutiAddr();
    pstNetId                            = NAS_LMM_GetEmmInfoNetInfoLastRegNetAddr();

    /*GUTI的填充*/
    if (NAS_NVIM_BIT_NO_SLCT == pstUeid->bitOpGuti)
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_WriteSimPsLoc : GUTI Invalid");

        NAS_LMM_SetArrayAllBytesFF((VOS_UINT8 *)&stNvPsLoc.stGuti,
                                   sizeof(NAS_LMM_GUTI_STRU));
        stNvPsLoc.stGuti.stGutiHead.ucLenth = NAS_LMM_NVIM_GUTI_AND_OETOI_LEN;
        stNvPsLoc.stGuti.stGutiHead.ucOeToi = NAS_LMM_NVIM_GUTI_OETOI;

    }
    else
    {

        NAS_LMM_MEM_CPY(                &stNvPsLoc.stGuti,
                                        pstGuti,
                                        sizeof(NAS_LMM_GUTI_STRU));
    }

    /*L.V.R TAI的填充*/
    if (NAS_NVIM_BIT_NO_SLCT == NAS_EMM_GetLVRTaiOpBit())
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_WriteSimPsLoc : L.V.R Invalid");
        NAS_LMM_UndefPlmn(&stNvPsLoc.stLastRegTai.stPlmnId);
        NAS_LMM_UndefTac(&stNvPsLoc.stLastRegTai.stTac);
    }
    else
    {
        NAS_LMM_MEM_CPY(                &stNvPsLoc.stLastRegTai.stPlmnId,
                                        &(pstNetId->stPlmnId),
                                        sizeof(NAS_MM_PLMN_ID_STRU));

        NAS_LMM_MEM_CPY(                &stNvPsLoc.stLastRegTai.stTac,
                                        &(pstNetId->stTac),
                                        sizeof(NAS_MM_TAC_STRU));
    }

    /*EU的填写*/
    stNvPsLoc.enUpdateState             = NAS_EMM_UPDATE_STAE;



    /*write new PS LOC to USIM*/
    ulRslt = NAS_LMM_NvimWrite(          EN_NV_ID_EPS_LOC_FILE,
                                        &stNvPsLoc,
                                        sizeof(LNAS_LMM_NV_EPS_LOC_STRU));

    if( NAS_LMM_NVIM_WRITTEN_UNNEEDED == ulRslt)
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteSimPsLoc: The value is already in usim, return.");
        return;
    }

    if (NAS_LMM_NVIM_OK != ulRslt)
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteSimPsLoc:write usim fail");
    }

    return;
}


VOS_VOID  NAS_LMM_WriteEpsSecuContext( NAS_NV_ITEM_OP_TYPE_ENUM_UINT32 enOpType )
{
    VOS_UINT32                          ulRslt;

    NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WriteEpsSecuContext enter.");

    if (NAS_LMM_SIM_STATUS_UNAVAILABLE == NAS_LMM_GetSimState())
    {
        return ;
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    ulRslt = NAS_EMM_IsServiceAvailable(USIM_SVR_EPS_INFO);

    if (PS_USIM_SERVICE_AVAILIABLE == ulRslt)
    {
        /* 向USIM写入EPS SECURITY CONTEXT */
        NAS_LMM_WriteSimEpsSecurityContext(enOpType);
    }
    else
    {
        /*向NV中写入EPS SECURITY CONTEXT*/
        NAS_LMM_WriteNvEpsSecurityContext(enOpType);
    }
    /*PC REPLAY MODIFY BY LEILI END*/

    return;
}


VOS_VOID  NAS_LMM_WritePsLoc
(
    NAS_NV_ITEM_OP_TYPE_ENUM_UINT32     enOpType
)
{
    VOS_UINT32                          ulRslt;

    NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WritePsLoc enter.");

    if (NAS_LMM_SIM_STATUS_UNAVAILABLE == NAS_LMM_GetSimState())
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_LMM_WritePsLoc: USIM is unavailable!.");
        return;
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    ulRslt = NAS_EMM_IsServiceAvailable(USIM_SVR_EPS_INFO);
    if (PS_USIM_SERVICE_AVAILIABLE == ulRslt)
    {
        /*向USIM写入PS LOC*/
        NAS_LMM_WriteSimPsLoc();
    }
    else
    {
        /*向NV中写入PS LOC*/
        NAS_LMM_WriteNvPsLoc(enOpType);
    }
    /*PC REPLAY MODIFY BY LEILI END*/

    return;
}

/*leili modify for isr begin*/

VOS_VOID  NAS_EMM_SetTinType(MMC_LMM_TIN_TYPE_ENUM_UINT32 ulTinType)
{
    NAS_NVIM_TIN_INFO_STRU         stNvTinType;
    VOS_UINT32                          ulRslt;

    NAS_LMM_MEM_SET(&stNvTinType, 0, sizeof(NAS_NVIM_TIN_INFO_STRU));

    /* 读全局变量中的TIN*/

    /*更新MML参数*/
    NAS_MML_SetTinType((VOS_UINT8)ulTinType);

    /* 如果是紧急注册过程中或者紧急注册，则直接退出，不写NV */
    if ((NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
         || (NAS_LMM_REG_STATUS_EMC_REGED == NAS_LMM_GetEmmInfoRegStatus()))
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_EMM_SetTinType : Need not write nv!");
        return ;
    }
    /*lint -e718*/
    /*lint -e732*/
    /*lint -e516*/
    /*lint -e746*/
    /*lint -e830*/
    ulRslt = LPs_NvimItem_Read(en_NV_Item_TIN_INFO, &stNvTinType, sizeof(NAS_NVIM_TIN_INFO_STRU));
    /*lint +e830*/
    /*lint +e746*/
    /*lint +e516*/
    /*lint +e732*/
    /*lint +e718*/
    if ((ERR_MSP_SUCCESS != ulRslt) || (MMC_LMM_TIN_INVALID < stNvTinType.ucTinType))
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_EMM_SetTinType:read NVIM fail or Value Invalid!");
    }

    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_SetTinType: read NV ITEM VALUE:IMSI = \n");
    NAS_COMM_PrintArray(                NAS_COMM_GET_MM_PRINT_BUF(),
                                        stNvTinType.aucImsi,
                                        NVIM_ITEM_MAX_IMSI_LEN);

    NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_SetTinType: read NV ITEM VALUE:ulTinType = ",stNvTinType.ucTinType);

    /*填充 NV  TIN 结构*/
    stNvTinType.ucTinType = (VOS_UINT8)ulTinType;
    /*lint -e718*/
    /*lint -e746*/
    /*lint -e732*/
    /*lint -e516*/
    ulRslt = LPs_NvimItem_Write(en_NV_Item_TIN_INFO, &stNvTinType, sizeof(NAS_NVIM_TIN_INFO_STRU));
    /*lint +e516*/
    /*lint +e732*/
    /*lint +e746*/
    /*lint +e718*/
    if (ERR_MSP_SUCCESS != ulRslt)
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_EMM_SetTinType:write NVIM fail!");
    }


    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_SetTinType: write NV ITEM VALUE:IMSI = \n");
    NAS_COMM_PrintArray(                NAS_COMM_GET_MM_PRINT_BUF(),
                                           stNvTinType.aucImsi,
                                           NAS_MML_MAX_IMSI_LEN);

    NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_SetTinType: write NV ITEM VALUE:ulTinType = ",stNvTinType.ucTinType);

    #if 0
    /*给MMC发送TIN消息 */
    NAS_EMM_SendMmcTinTypeInd(ulTinType);

    #endif
    return;
}
/*leili modify for isr end*/


VOS_VOID NAS_EMM_WriteNvMmInfo(VOS_VOID)
{
    if (NAS_LMM_SIM_STATUS_AVAILABLE == NAS_LMM_GetSimState())
    {
        /*保存安全上下文*/
        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);

        /* 判断没有放到函数NAS_LMM_WritePsLoc中来做，是由于紧急注册被拒或者尝试
           次数达到5次时还是要删除参数 ，协议只规定是紧急注册成功后才不写卡或者
           NV项 */
        if ((NAS_LMM_REG_STATUS_EMC_REGING != NAS_LMM_GetEmmInfoRegStatus())
            && (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus()))
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }


        /*GU维护，L不再 写NV*/
        /*NAS_LMM_WriteGuUeMode();*/
    }

    return;
}
VOS_VOID NAS_LMM_GetImeisvFromImei(VOS_VOID)
{

    VOS_UINT8                           aucImeiSvn[4] = {0,0,0,0};

    if (NAS_EMM_BIT_SLCT != NAS_LMM_GetEmmInfoUeidAddr()->bitOpImei)
    {
        NAS_LMM_ReadNvimImei();
    }

    NAS_LMM_GetEmmInfoUeidAddr()->bitOpImeisv   = NAS_EMM_BIT_SLCT;
    NAS_LMM_MEM_CPY(                     NAS_LMM_GetEmmInfoUeidImeisvAddr(),
                                        NAS_LMM_GetEmmInfoUeidImeiAddr(),
                                        NAS_MM_MAX_UEID_BUF_SIZE);
    NAS_LMM_GetEmmInfoUeidAddr()->aucImeisv[0] =  0x09; /*长度为固定值9*/
    NAS_LMM_GetEmmInfoUeidAddr()->aucImeisv[1] &= 0xF0;
    NAS_LMM_GetEmmInfoUeidAddr()->aucImeisv[1] |= 0x03;
    NAS_LMM_GetEmmInfoUeidAddr()->aucImeisv[8] &= 0x0F;
    NAS_LMM_GetEmmInfoUeidAddr()->aucImeisv[9] = 0xF0;

    /* 获取IMEISV的内容 */
    /*lint -e516*/
    if (NV_OK != NVM_Read(en_NV_Item_Imei_Svn,
                   aucImeiSvn, sizeof(aucImeiSvn)))
    {
       NAS_LMM_PUBM_LOG_WARN("NAS_LMM_GetImeisvFromImei:Read Nvim Failed");
       return;
    }
    /*lint +e516*/

    /* NV项有效,则更新 */
    if (VOS_TRUE == aucImeiSvn[0])
    {
        NAS_LMM_GetEmmInfoUeidAddr()->aucImeisv[8] &= 0x0F;
        NAS_LMM_GetEmmInfoUeidAddr()->aucImeisv[9] &= 0x00;
        NAS_LMM_GetEmmInfoUeidAddr()->aucImeisv[8] |= (VOS_UINT8)(aucImeiSvn[1] << NAS_LMM_MOVEMENT_4_BITS);
        NAS_LMM_GetEmmInfoUeidAddr()->aucImeisv[9] |= (aucImeiSvn[2] | 0xF0);
    }

     /* 打印获取到的IMEISV值 */
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_GetImeisvFromImei: IMEISV value is :");
    NAS_COMM_PrintArray(NAS_COMM_GET_MM_PRINT_BUF(),
                   NAS_LMM_GetEmmInfoUeidImeisvAddr(),
                   NAS_MM_MAX_UEID_BUF_SIZE);

    return;
}
#if 0

VOS_VOID    NAS_EMM_StopAllEmmStateTimer( VOS_VOID )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstStateTimerList;
    VOS_UINT32                          ulTcbIdxLoop;
    VOS_UINT32                          ulRslt;
    VOS_BOOL                            bFlag               = VOS_TRUE;

    /*获取状态定时器列表首地址*/
    pstStateTimerList = NAS_LMM_GetStateTimerListAddr();

    /*停止所有处于运行状态的协议定时器*/
    for ( ulTcbIdxLoop = TI_NAS_EMM_STATE_NO_TIMER + 1; ulTcbIdxLoop < TI_NAS_EMM_STATE_TI_BUTT; ulTcbIdxLoop++ )
    {
        /*检查该状态定时器是否在运行，如果正在运行，停止该定时器。
          停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
        if ( VOS_NULL_PTR != pstStateTimerList[ulTcbIdxLoop].psthTimer )
        {
            ulRslt = PS_STOP_REL_TIMER(&(pstStateTimerList[ulTcbIdxLoop].psthTimer));
            /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
            if ( VOS_OK != ulRslt )
            {
                bFlag = VOS_FALSE;
                /* 启动失败，打印失败信息 */
                NAS_LMM_PUBM_LOG1_ERR("NAS_EMM_StopAllEmmStateTimer, Stop pstStateTimerList[%d] Failure!",
                                     ulTcbIdxLoop);
            }
        }
    }

    NAS_LMM_PUBM_LOG1_INFO("NAS_EMM_StopAllEmmStateTimer: bFlag =", bFlag);

    return;
}


VOS_VOID    NAS_EMM_StopAllEmmPtlTimer( VOS_VOID )
{
    NAS_LMM_TIMER_CTRL_STRU              *pstPtlTimerList;
    VOS_UINT32                          ulTcbIdxLoop;
    VOS_UINT32                          ulRslt;
    VOS_BOOL                            bFlag               = VOS_TRUE;

    /*获取状态定时器列表首地址*/
    pstPtlTimerList = NAS_LMM_GetPtlTimerListAddr();

    /*停止所有处于运行状态的协议定时器*/
    for ( ulTcbIdxLoop = TI_NAS_LMM_PTL_TI_PUB_BOUNDARY_START +1; ulTcbIdxLoop < TI_NAS_EMM_PTL_TI_BUTT; ulTcbIdxLoop++ )
    {
        /*检查该协议定时器是否在运行，如果正在运行，停止该定时器。
          停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
        if ( VOS_NULL_PTR != pstPtlTimerList[ulTcbIdxLoop].psthTimer )
        {
            ulRslt = PS_STOP_REL_TIMER(&(pstPtlTimerList[ulTcbIdxLoop].psthTimer));

            /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
            if ( VOS_OK != ulRslt )
            {
                bFlag = VOS_FALSE;
                NAS_LMM_PUBM_LOG1_ERR("NAS_EMM_StopAllEmmPtlTimer, Stop pstPtlTimerList[%d] Failure!",
                                     ulTcbIdxLoop);
            }
        }
    }

    NAS_LMM_PUBM_LOG1_INFO("NAS_EMM_StopAllEmmPtlTimer: bFlag =", bFlag);

    return;
}
#endif


/***************************************************************************
 Function Name  :NAS_EMM_GetEmmGlobleAddr
 Discription    :用于调用其他模块的全局变量
 Input          :
 Output         : None
 Return         : None
 History:
      1.  yangfan  2009-10-19  Draft Enact
*****************************************************************************/
VOS_UINT32  * NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_PARA_ENUM_UINT32 ulGlobleParaName)
{
    VOS_UINT32  * pucGlobleAddr;

    /* PRINT */

    switch(ulGlobleParaName )
    {
        case    NAS_LMM_GLOBLE_PAEA_IMSI:
            pucGlobleAddr = (VOS_VOID * )NAS_LMM_GetEmmInfoUeidImsiAddr();
            break;

        case    NAS_LMM_GLOBLE_PAEA_UEID:
            pucGlobleAddr = (VOS_VOID * )NAS_LMM_GetEmmInfoUeidAddr();
            break;

        case    NAS_LMM_GLOBLE_FORB_TA_FOR_ROAMING:
            pucGlobleAddr = (VOS_VOID * )NAS_LMM_GetEmmInfoNetInfoForbTaForRoamAddr();
            break;
        case    NAS_LMM_GLOBLE_FORB_TA_FOR_RPOS:
            pucGlobleAddr = (VOS_VOID * )NAS_LMM_GetEmmInfoNetInfoForbTaForRposAddr();
            break;
        case    NAS_LMM_GLOBLE_PRESENT_NET_ID:
            pucGlobleAddr = (VOS_VOID * )NAS_LMM_GetEmmInfoNetInfoPresentNetAddr();
            break;
        case    NAS_LMM_GLOBLE_NET_INFO:
            pucGlobleAddr = (VOS_VOID * )NAS_LMM_GetEmmInfoNetInfoAddr();
            break;

        case    NAS_LMM_GLOBLE_TA_LIST:
            pucGlobleAddr = (VOS_VOID * )NAS_LMM_GetEmmInfoNetInfoTaiListAddr();
            break;

        default :
            pucGlobleAddr = VOS_NULL;
            break;
    }

    return pucGlobleAddr;
}
VOS_VOID  NAS_EMM_SendIntraConn2IdleReq( VOS_VOID )
{
    NAS_EMM_INTRA_CONN2IDLE_REQ_STRU   *pIntraConn2IdleReq = NAS_LMM_NULL_PTR;
    VOS_UINT32                          ulLen;

    /*确定消息长度*/
/*    ulLen = NAS_EMM_GIM_VOS_MSG_HEADER_LEN + NAS_EMM_MSG_ID_LEN;*/
    ulLen = sizeof(NAS_EMM_INTRA_CONN2IDLE_REQ_STRU);

    /*分配空间*/
    pIntraConn2IdleReq  = (VOS_VOID *)NAS_LMM_GetSendIntraMsgBuffAddr(ulLen);
    if(NAS_EMM_NULL_PTR == pIntraConn2IdleReq)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_SendIntraConn2IdleReq: INTRA MSG ALLOC ERR !!");
        return;
    }

    /* 打包VOS消息头 */
    NAS_EMM_INTRA_MSG_HEADER(pIntraConn2IdleReq,(ulLen-NAS_EMM_GIM_VOS_MSG_HEADER_LEN));

    /* 填充DOPRA消息ID    */
    pIntraConn2IdleReq->ulMsgId = ID_NAS_LMM_INTRA_CONN2IDLE_REQ;

    /*发送内部消息入队列*/
    NAS_EMM_SEND_INTRA_MSG(pIntraConn2IdleReq);

    return;
}


VOS_VOID  NAS_EMM_CommProcConn2Ilde( VOS_VOID )
{
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_CommProcConn2Ilde enter!");

    /*如果当前连接状态不是IDLE，则向 MMC上报*/
    if(NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
    {
        NAS_EMM_SendMmcStatusInd(MMC_LMM_STATUS_TYPE_CONN_STATE,
                                 MMC_LMM_CONN_IDLE);
    }

    if((NAS_EMM_CONN_SIG == NAS_EMM_GetConnState())
        || (NAS_EMM_CONN_DATA == NAS_EMM_GetConnState())
        || (NAS_EMM_CONN_RELEASING == NAS_EMM_GetConnState()))
    {
        /* 把RRC连接状态设置为释放过程中 */
        NAS_EMM_SetConnState(NAS_EMM_CONN_RELEASING);
    }
    else
    {
        /* 把RRC连接状态设置为IDLE */
        NAS_EMM_SetConnState(NAS_EMM_CONN_IDLE);
    }

    if (NAS_LMM_CONNECTION_CLIENT_ID_SMS == NAS_LMM_GetConnectionClientId())
    {
        NAS_LMM_SndLmmSmsErrInd(LMM_SMS_ERR_CAUSE_OTHERS);
        NAS_LMM_SetConnectionClientId(NAS_LMM_CONNECTION_CLIENT_ID_NULL);
    }
    /*连接态从CONN进入IDLE，导致当前安全上下文状态变化*/
    NAS_EMM_ConnToIdleTransSecuSta();

    /*发送内部消息:INTRA_CONN2IDLE_REQ*/
    NAS_EMM_SendIntraConn2IdleReq();

    return;
}
VOS_VOID NAS_EMM_SaveAppMsgPara( VOS_UINT32 ulMsgId, VOS_UINT32 ulOpId)
{
    g_stEmmInfo.bitOpAppPara         = NAS_EMM_BIT_SLCT;
    g_stEmmInfo.stAppPara.ulMsgId    = ulMsgId;
    g_stEmmInfo.stAppPara.ulOpId     = ulOpId;

    return;
}



VOS_VOID  NAS_EMM_ClearAppMsgPara( VOS_VOID )
{
    /*清空APP参数*/
    g_stEmmInfo.bitOpAppPara       = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_MEM_SET(                &g_stEmmInfo.stAppPara,
                                   0,
                                   sizeof(NAS_LMM_APP_PARA_STRU));
}
VOS_UINT32  NAS_EMM_CheckAppMsgPara( VOS_UINT32 ulMsgId )
{
    if ((ulMsgId              == g_stEmmInfo.stAppPara.ulMsgId)
        && (NAS_EMM_BIT_SLCT  == g_stEmmInfo.bitOpAppPara))
    {
        return                   VOS_OK;
    }
    else
    {
        return                   VOS_ERR;
    }
}


VOS_UINT32  NAS_EMM_GetAppMsgOpId( VOS_VOID )
{
    return                      g_stEmmInfo.stAppPara.ulOpId;
}


VOS_VOID  NAS_LMM_CountSysInfo( VOS_UINT16 usSubState )
{
    NAS_LMM_SYSINFO_COUNT_STRU           *pstSysInfoCnt;

    pstSysInfoCnt                      = NAS_EMM_GetSysInfCntAddr();

    switch( usSubState )
    {
        case    EMM_SS_RRC_CONN_WAIT_EST_CNF:
            pstSysInfoCnt->ulEstCnfCount++;
            NAS_LMM_PUBM_LOG1_INFO("MsRrcConnEstInitSsWaitRrcEstCnf: Receive SysInfo ",pstSysInfoCnt->ulEstCnfCount);
            break;

        case    EMM_SS_RRC_CONN_WAIT_REL_CNF:
            pstSysInfoCnt->ulRelCnfCount++;
            NAS_LMM_PUBM_LOG1_INFO("MsRrcConnRelInitSsWaitRrcRelCnf: Receive SysInfo ",pstSysInfoCnt->ulRelCnfCount);
            break;

        /*case    EMM_SS_ATTACH_WAIT_RRC_REL_IND:
            pstSysInfoCnt->ulRegInitRrcRelIndCount++;
            NAS_LMM_PUBM_LOG1_INFO("MsRegInitSsWaitRrcRelInd: Receive SysInfo ",pstSysInfoCnt->ulRegInitRrcRelIndCount);
            break;*/


        default :
            NAS_LMM_PUBM_LOG_ERR("NAS_LMM_CountSysInfo,Error State!");
            break;
    }

}


VOS_VOID  NAS_LMM_EmmInfoInit( VOS_VOID )
{
    NAS_EMM_PubmInfoUeIdInit();
    NAS_EMM_PubmInfoNetInfoInit();
    NAS_EMM_PubmInfoSupCodecListInit();
    NAS_EMM_PubmInfoBearerContextInit();
    NAS_EMM_PubmInfoDrxInit();

    NAS_EMM_PubmInfoUeModeInit();
#if 0
    /*初始化FPLMN为6,当soft usim时FPLMN数就为6，hard usim时FPLMN会赋卡中携带的值*/
    if (NAS_EMM_USIM_FEATURE_SOFT == NAS_LMM_GetUsimFeature())
    {
        NAS_EMMC_GetMaxNumOfFplmnByUsim() = NAS_EMMC_SOFT_FPLMN_NUM;
    }
#endif
    /*根据IMSI的读取设置卡状态*/
    /* NAS_EMM_InitSimState();*/
    NAS_EMM_SetUsimState(NAS_LMM_SIM_STATUS_UNAVAILABLE);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_VALID);

    /* 开机参数赋值 */
    NAS_EMM_GetTransceiverType()        = NAS_EMM_TRANSCEIVER_TYPE_BUTT;
    NAS_LMM_GetCurGsmPrio()             = NAS_LMM_RAT_PRIO_BUTT;
    NAS_LMM_GetCurWcdmaPrio()           = NAS_LMM_RAT_PRIO_BUTT;
    NAS_LMM_GetCurLtePrio()             = NAS_LMM_RAT_PRIO_BUTT;
    /*leili modify for isr begin*/
    NAS_LMM_SetTempEmmInfoTinType(MMC_LMM_TIN_BUTT);
    /*leili modify for isr end*/

    return;
}


VOS_VOID  NAS_EMM_ReadEmmNvim( VOS_VOID )
{
    /*无卡开机时需读取的基本基础的NV*/
    NAS_EMM_ReadNvimNoImsi();

}
VOS_VOID  NAS_EMM_ReadNvimNoImsi( VOS_VOID )
{
    /*read IMEI from NVIM*/
    NAS_LMM_ReadNvimImei();

    /*caculate Imeisv with IMEI*/
    NAS_LMM_GetImeisvFromImei();

    #if 0
    /*read MsNetCap from NVIM*/
    NAS_EMM_ReadNvMsNetCap();
    #endif

    /*read UeNetCap from NVIM*/
    NAS_EMM_ReadNvUeNetCap();

    /*read ue operation mode from NVIM*/
    /*NAS_EMM_ReadNvUeOperationMode();*/

    /*read CsService from NVIM*/
    /* lihong00150010 csfb begin */

    /*NAS_EMM_ReadNvCsService();*/

    /* lihong00150010 csfb end */

    /*read Voice Domain for E-UTRAN from NVIM*/
    NAS_EMM_ReadNvVoiceDomain();

    /* 从NV项中读取UE's setting，并设置到MML中 */
    NAS_EMM_ReadNvUeSetting();


    /*read ClassMark from NVIM*/
    /*NAS_EMM_ReadNvClassMark();*/

    /*read Drx from NVIM*/
    /*NAS_EMM_ReadNvDrx();*/

    /*read RaMod from NVIM*/
    /*NAS_EMM_ReadNvRaMod();  */
    NAS_EMM_ReadNvNasRelease();

    /*sunbing 49683 2013-10-21 VoLTE begin*/
    NAS_EMM_ReadNvCodecSupportList();
    /*sunbing 49683 2013-10-21 VoLTE end*/
    /*self-adaption NW cause modify begin for leili*/
    NAS_EMM_ReadNvConfigNwCause();
    /*self-adaption NW cause modify end for leili*/
}
VOS_VOID  NAS_EMM_NoUsimInit( VOS_VOID )
{
    VOS_UINT32           ulReturnCode = 0;
    LPS_SWITCH_PARA_STRU stPsSwitchPara;
    VOS_UINT32           ulDeleteContent = 0;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_NoUsimInit IS ENTERED ");

    /****无卡环境下初始化EMM*****/
    /* 从NV中读取无卡状态下所需要的项 */
    NAS_EMM_ReadNvimNoImsi();

    /*无卡场景下，按照33401的要求，删除NV中的EPS SECURITY CONTEXT*/
    /*为解决在工信部卡接口一致性测试时,测试时手机开机时设置PIN码,无卡开机进入此函数删除了NV中的安全上下文，用例失败,
        故增加NV控制，打开开关，则不删除安全上下文 */
    /*lint -e732*/
    /*lint -e516*/
    ulReturnCode = LPs_NvimItem_Read(EN_NV_ID_SWITCH_PARA,(VOS_VOID *)(&stPsSwitchPara),sizeof(LPS_SWITCH_PARA_STRU));
    /*lint +e516*/
    /*lint +e732*/
    if (PS_SUCC != ulReturnCode)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_NoUsimInit,LPs_NvimItem_Read SWITCH_PARA fail");
        ulDeleteContent = 0;
    }
    else
    {
        ulDeleteContent = (*((VOS_UINT32 *)(&(stPsSwitchPara.stPsFunFlag04))))&(0x00000002);
    }

    if( 0 == ulDeleteContent )
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_NoUsimInit,delete content");
        NAS_LMM_WriteNvEpsSecurityContext(0);
    }
    else
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_NoUsimInit,don't delete content");
    }

    /*无卡场景下，不再删除NV中的PS LOC*/
    /*NAS_LMM_WriteNvPsLoc(0);*/

    NAS_EMM_PubmInfoNetInfoInit();
    NAS_EMM_SecuInit();

    return;

}
/* lihong00150010 emergency tau&service begin */
#if 0
VOS_VOID  NAS_EMM_InitSimState( VOS_VOID )
{
    NAS_EMM_UEID_STRU                   *pstUeId = NAS_LMM_NULL_PTR;

    /* 获取UEID地址 */
    pstUeId = NAS_EMM_GLO_AD_GetUeIdAddr();

    if (NAS_EMM_BIT_SLCT == pstUeId->bitOpImsi)
    {
        NAS_LMM_GetSimState()           = NAS_LMM_SIM_STATUS_AVAILABLE;
    }
    else
    {
        NAS_LMM_GetSimState()           = NAS_LMM_SIM_STATUS_UNAVAILABLE;
    }

    return;
}
#endif
/* lihong00150010 emergency tau&service end */

VOS_UINT32  NAS_LMM_CheckPlmnAct(VOS_UINT8 const *pucPlmn)
{

    if ((NAS_LMM_NULL == (pucPlmn[NAS_EMM_PLMNACT_IDEN] & NAS_LMM_BYTE_FF))
        && (NAS_LMM_NULL == (pucPlmn[NAS_EMM_PLMNACT_IDEN + 1] & NAS_LMM_BYTE_FF)))
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_CheckPlmnAct: PLMNAct is support");
        return VOS_OK;
    }
    switch ( NAS_LMM_GetEmmInfoRaMod())
    {
        case    NAS_MM_RAT_LTE_FDD:
        case    NAS_MM_RAT_LTE_TDD:
            if (NAS_EMM_PLMNACT_LTE == (pucPlmn[NAS_EMM_PLMNACT_IDEN] & NAS_EMM_PLMNACT_LTE))
            {
                NAS_LMM_PUBM_LOG_INFO("NAS_LMM_CheckPlmnAct: PLMNAct: LTE");
                return VOS_OK;
            }
            break;

        case    NAS_MM_RAT_WCDMA:
        case    NAS_MM_RAT_GSM_GPRS:
            break;

        default:
            break;
    }

    NAS_LMM_PUBM_LOG_WARN("NAS_LMM_CheckPlmnAct: PLMNAct: NO LTE");
    return VOS_ERR;

}


VOS_VOID NAS_EMM_ClearTaiList( VOS_VOID )
{
    NAS_LMM_GetEmmInfoNetInfoAddr()->bitOpTaiList  = NAS_EMM_BIT_NO_SLCT;

    /*删除TA LIST*/
    NAS_LMM_MEM_SET(           NAS_LMM_GetEmmInfoNetInfoTaiListAddr(),
                              0,
                              sizeof(NAS_EMM_TA_LIST_STRU));
}
VOS_VOID  NAS_EMM_ClearLVRTai( VOS_VOID )
{
    /*删除 L.V.R TAI*/
    NAS_LMM_GetEmmInfoNetInfoAddr()->bitOpLastRegNetId      = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_GetEmmInfoNetInfoLastRegNetAddr()->bitOpTac     = NAS_EMM_BIT_NO_SLCT;

    NAS_LMM_UndefPlmn(NAS_LMM_GetEmmInfoLastRegPlmnAddr());
    NAS_LMM_UndefTac(NAS_LMM_GetEmmInfoLastRegTacAddr());

}
VOS_VOID  NAS_EMM_SetLVRTacInvalid( VOS_VOID )
{
    NAS_LMM_UndefTac(NAS_LMM_GetEmmInfoLastRegTacAddr());

    if (NAS_EMM_BIT_NO_SLCT == NAS_EMM_GetLVRTaiOpBit())
    {
        NAS_LMM_UndefPlmn(NAS_LMM_GetEmmInfoLastRegPlmnAddr());
    }
    return;
}




VOS_UINT32 NAS_EMM_GetLVRTaiOpBit(VOS_VOID)
{
    NAS_LMM_NETWORK_INFO_STRU            *pstNetInfo;
    VOS_UINT32                          ulRslt;

    pstNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    ulRslt = (pstNetInfo->bitOpLastRegNetId) && (pstNetInfo->stLastRegNetId.bitOpTac);

    return ulRslt;
}


VOS_VOID  NAS_EMM_SetLVRTai(const NAS_MM_NETWORK_ID_STRU *pstNetId)
{
    NAS_LMM_NETWORK_INFO_STRU            *pstNetInfo;

    pstNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    if (NAS_EMM_NULL_PTR == pstNetId)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_SetLVRTai: Point is null");

        return;
    }

    pstNetInfo->bitOpLastRegNetId       = NAS_EMM_BIT_SLCT;
    pstNetInfo->stLastRegNetId.bitOpTac = NAS_EMM_BIT_SLCT;

    NAS_LMM_MEM_CPY (                    &pstNetInfo->stLastRegNetId.stPlmnId,
                                        &pstNetId->stPlmnId,
                                        sizeof(NAS_MM_PLMN_ID_STRU));

    NAS_LMM_MEM_CPY (                    &pstNetInfo->stLastRegNetId.stTac,
                                        &pstNetId->stTac,
                                        sizeof(NAS_MM_TAC_STRU));
    return;
}


VOS_VOID  NAS_EMM_SetGuti
(
    const NAS_EMM_GUTI_STRU *pstGuti
)
{
    NAS_LMM_UEID_STRU                   *pstUeId;

    pstUeId = NAS_LMM_GetEmmInfoUeidAddr();

    if (NAS_EMM_NULL_PTR == pstGuti)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_SetGuti: Point is null");

        return;
    }

    pstUeId->bitOpGuti                  = NAS_EMM_BIT_SLCT;
    NAS_LMM_MEM_CPY (                   &(pstUeId->stGuti),
                                        pstGuti,
                                        sizeof(NAS_LMM_GUTI_STRU));
    return;
}



VOS_UINT32  NAS_EMM_UpdatePresentPlmnInfo(VOS_VOID)
{
    VOS_UINT32                          ulCount          = 0;
    VOS_UINT32                          ulUpdateResult   = NAS_EMM_FAIL;
    NAS_LMM_UEID_STRU                  *pstMmUeId        = NAS_EMM_NULL_PTR;
    NAS_MM_PLMN_ID_STRU                *pstPresentPlmnId = NAS_EMM_NULL_PTR;
    NAS_MM_GUTI_STRU                   *pstGuti          = NAS_EMM_NULL_PTR;

    pstGuti          = NAS_LMM_GetEmmInfoUeidGutiAddr();
    pstMmUeId        = NAS_LMM_GetEmmInfoUeidAddr();
    pstPresentPlmnId = NAS_LMM_GetEmmInfoPresentPlmnAddr();

    /* 存在GUTI, 并且Present Plmn ID 与GUTI中的Plmn ID不同, 将Present Plmn ID更新成GUTI中的Plmn ID */
    if ( NAS_EMM_BIT_SLCT== pstMmUeId->bitOpGuti )
    {
        for ( ulCount = 0; ulCount < 3; ulCount++ )
        {
            if ( pstPresentPlmnId->aucPlmnId[ulCount] != pstGuti->stPlmnId.aucPlmnId[ulCount] )
            {
                pstPresentPlmnId->aucPlmnId[ulCount] = pstGuti->stPlmnId.aucPlmnId[ulCount];
                ulUpdateResult = NAS_EMM_SUCC;
            }
        }
    }

    return   ulUpdateResult;

}
VOS_UINT32 NAS_EMM_GetMmlFormatMcc
(
    const NAS_MM_PLMN_ID_STRU             *pstPlmnId
)
{
    VOS_UINT32                          ulMcc       = NAS_EMM_NULL;
    VOS_UINT8                           ucMccDigit  = NAS_EMM_NULL;

    /* 获取MCC DIGIT1 */
    ucMccDigit  = pstPlmnId->aucPlmnId[0] & NAS_EMM_LOW_4_BIT;
    ulMcc       |=  ucMccDigit;

    /* 获取MCC DIGIT2 */
    ucMccDigit  = (pstPlmnId->aucPlmnId[0] & NAS_EMM_HIGH_4_BIT) >> NAS_EMM_MOVEMENT_4_BITS;
    ulMcc       |=  (ucMccDigit << NAS_EMM_MOVEMENT_8_BITS);

    /* 获取MCC DIGIT3 */
    ucMccDigit  = pstPlmnId->aucPlmnId[1] & NAS_EMM_LOW_4_BIT;
    ulMcc       |=  (ucMccDigit << NAS_EMM_MOVEMENT_16_BITS);

    return ulMcc;
}
/* lihong00150010 csfb begin */


VOS_UINT32 NAS_EMM_IsMccChanged( VOS_VOID )
{
    NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstMmlEmerNumLIst   = VOS_NULL_PTR;
    VOS_UINT32                          ulMcc               = NAS_EMM_NULL;

    pstMmlEmerNumLIst = NAS_EMM_GetEmergencyListFromMml();

    /* 获取当前PLMN中的MCC，格式与MML中一致 */
    ulMcc = NAS_EMM_GetMmlFormatMcc(NAS_LMM_GetEmmInfoPresentPlmnAddr());

    /* 判断当前网络PLMN中的MCC与MML中存储的MCC是否一致 */
    if (ulMcc == (pstMmlEmerNumLIst->ulMcc & NAS_EMM_LOW_3_BYTE_F))
    {
        return VOS_FALSE;
    }
    return VOS_TRUE;
}


VOS_VOID  NAS_EMM_PrintEmergencyList
(
    NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstMmlEmerNumList
)
{
    VOS_UINT32                          ulMcc           = NAS_EMM_NULL;
    VOS_UINT8                           ucMccDigit      = NAS_EMM_NULL;
    VOS_UINT32                          ulLoop          = NAS_EMM_NULL;
    VOS_UINT32                          ulLoop2         = NAS_EMM_NULL;
    NAS_MML_EMERGENCY_NUM_STRU         *pstMmlEmerNum   = VOS_NULL_PTR;

    /*add by lifuxin00253982 pc-lint error */
    if(NULL == pstMmlEmerNumList)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_PrintEmergencyList: NULL Input!");
        return;
    }

    /* 打印MCC信息 */
    ulMcc = pstMmlEmerNumList->ulMcc;
    ucMccDigit = ulMcc & NAS_EMM_FIRST_BYTE_F;
    NAS_LMM_PUBM_LOG1_NORM("MCC DIGIT1: ",ucMccDigit);

    ucMccDigit = (ulMcc & NAS_EMM_SECOND_BYTE_F) >> NAS_EMM_MOVEMENT_8_BITS;
    NAS_LMM_PUBM_LOG1_NORM("MCC DIGIT2: ",ucMccDigit);

    ucMccDigit = (ulMcc & NAS_EMM_THIRD_BYTE_F) >> NAS_EMM_MOVEMENT_16_BITS;
    NAS_LMM_PUBM_LOG1_NORM("MCC DIGIT3: ",ucMccDigit);

    /* 打印紧急呼号码列表 */
    NAS_LMM_PUBM_LOG1_NORM("EMERGENCY NUM LIST AMOUT: ",pstMmlEmerNumList->ucEmergencyNumber);

    for (ulLoop = NAS_EMM_NULL; ulLoop < pstMmlEmerNumList->ucEmergencyNumber; ulLoop++)
    {
        pstMmlEmerNum = &pstMmlEmerNumList->aucEmergencyList[ulLoop];

        NAS_LMM_PUBM_LOG1_NORM("EMER NUM INFO",ulLoop);

        NAS_LMM_PUBM_LOG1_NORM("EMER NUM CATOGORY: ",pstMmlEmerNum->ucCategory);

        for (ulLoop2 = NAS_EMM_NULL; ulLoop2 < pstMmlEmerNum->ucEmcNumLen; ulLoop2++)
        {
            NAS_LMM_PUBM_LOG1_NORM("EMER NUM : ",pstMmlEmerNum->aucEmcNum[ulLoop2]);
        }
    }
}
/*PC REPLAY MODIFY BY LEILI BEGIN*/

NAS_MML_EMERGENCY_NUM_LIST_STRU*  NAS_EMM_GetEmergencyListFromMml( VOS_VOID )
{
    NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstMmlEmerNumList = VOS_NULL_PTR;
    #ifndef __PS_WIN32_RECUR__
    pstMmlEmerNumList = NAS_MML_GetEmergencyNumList();

    /* 打印从MML读取的紧急呼号码列表信息 */
    NAS_EMM_PrintEmergencyList(pstMmlEmerNumList);

    NAS_LMM_LogEmergencyNumListInfo(pstMmlEmerNumList);

    #else
    pstMmlEmerNumList = Stub_NAS_MML_GetEmergencyNumList();
    #endif
    return pstMmlEmerNumList;
}
/*PC REPLAY MODIFY BY LEILI END*/

VOS_VOID  NAS_EMM_SetEmergencyList2Mml
(
    const NAS_MM_PLMN_ID_STRU             *pstCurPlmnId,
    const NAS_LMM_EMERGENCY_NUM_LIST_STRU *pstEmerNumList
)
{
    VOS_UINT32                          ulMcc           = NAS_EMM_NULL;
    NAS_MML_EMERGENCY_NUM_LIST_STRU     stMmlEmerNumList= {NAS_EMM_NULL};

    /* 取出当前PLMN中的MCC，并转化为MML中MCC的格式 */
    ulMcc = NAS_EMM_GetMmlFormatMcc(pstCurPlmnId);

    stMmlEmerNumList.ulMcc              = ulMcc;
    stMmlEmerNumList.ucEmergencyNumber  = pstEmerNumList->ucEmergencyNumAmount;
    NAS_LMM_MEM_CPY(    stMmlEmerNumList.aucEmergencyList,
                        pstEmerNumList->astEmergencyNumList,
                        sizeof(LMM_MM_EMERGENCY_NUM_STRU) * NAS_MML_EMERGENCY_NUM_LIST_MAX_RECORDS);

    /* 打印写入MML的紧急呼列表信息 */
    NAS_EMM_PrintEmergencyList(&stMmlEmerNumList);

    NAS_MML_SetEmergencyNumList(&stMmlEmerNumList);
}


VOS_VOID NAS_EMM_ClearEmergencyList( VOS_VOID )
{
    NAS_LMM_EMERGENCY_NUM_LIST_STRU      stEmerNumListTmp   = {NAS_EMM_NULL};

    /* 清除MML中的紧急呼号码列表 */
    NAS_EMM_SetEmergencyList2Mml(   NAS_LMM_GetEmmInfoPresentPlmnAddr(),
                                    &stEmerNumListTmp);
}

/* lihong00150010 csfb end */


VOS_VOID  NAS_EMM_SetEmergencyList
(
    VOS_UINT32                             ulOpEmerList,
    const NAS_LMM_EMERGENCY_NUM_LIST_STRU *pstEmerNumList
)
{
    NAS_LMM_PUB_INFO_STRU               *pstPubInfo = VOS_NULL_PTR;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    if (NAS_EMM_NULL_PTR == pstEmerNumList)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_SetEmergencyList: Point is null");

        return;
    }
/* lihong00150010 csfb begin */

    if (NAS_EMM_BIT_SLCT == ulOpEmerList)
    {
        /* 将紧急呼号码列表设置到MML */
        NAS_EMM_SetEmergencyList2Mml(   &pstPubInfo->stNetInfo.stPresentNetId.stPlmnId,
                                        pstEmerNumList);
        return ;
    }

    /* 如果空口没有携带紧急呼号码列表，则比较当前PLMN中的MCC是否变更，若发生变更，
       则设置紧急呼号码列表个数为0 */
    if (VOS_TRUE == NAS_EMM_IsMccChanged())
    {
        NAS_EMM_ClearEmergencyList();
        return ;
    }
/* lihong00150010 csfb end */
}
VOS_VOID  NAS_EMM_SetEpsNetFeature
(
    VOS_UINT32                          ulOpNetFeature,
    VOS_UINT8                           ucNetFeature
)
{
    NAS_LMM_PUB_INFO_STRU               *pstPubInfo = VOS_NULL_PTR;

    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_SetEpsNetFeature is entered");

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    if (NAS_EMM_BIT_SLCT == ulOpNetFeature)
    {
        pstPubInfo->bitOpNetFeature         = NAS_EMM_BIT_SLCT;
        pstPubInfo->ucNetFeature            = ucNetFeature;
    }
    else
    {
        pstPubInfo->bitOpNetFeature         = NAS_EMM_BIT_NO_SLCT;
        pstPubInfo->ucNetFeature            = NAS_EMM_NULL;
    }

    return;
}
/* lihong00150010 ims begin */
/*leili modify for isr begin*/

VOS_VOID  NAS_EMM_SetAddUpdateRslt
(
    VOS_UINT32                                  ulOpAddUpdateRslt,
    NAS_LMM_ADDITIONAL_UPDATE_RSLT_ENUM_UINT32  enAddUpdateRslt
)
{
    NAS_LMM_PUB_INFO_STRU              *pstEmmInfo = VOS_NULL_PTR;

    pstEmmInfo = NAS_LMM_GetEmmInfoAddr();

    /* ATTACH ACP或者TAU ACCEPT携带Additional update result,更新Additional update
       result到全局变量中 */
    if (NAS_EMM_BIT_SLCT == ulOpAddUpdateRslt)
    {
        NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_SetAddUpdateRslt:enAddUpdateRslt = ,",
                               enAddUpdateRslt);

        pstEmmInfo->enAdditionUpRslt = enAddUpdateRslt;
        /*NAS_MML_SetAdditionUpdateRslt((VOS_UINT8)enAddUpdateRslt);*/
        return ;
    }

    /* 如果UE能力不支持CSFB，只支持SMS ONLY，则即使ATTACH ACCPET中没有携带
       Additional update result信元，也要将EMM INFO全局变量中的Additional update
       result值设为SMS ONLY */
    if (NAS_LMM_CS_SERVICE_SMS_ONLY == NAS_EMM_GetCsService())
    {
        NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_SetAddUpdateRslt:enAddUpdateRslt = ,",
                               NAS_MML_ADDITION_UPDATE_RSLT_SMS_ONLY);

        pstEmmInfo->enAdditionUpRslt = NAS_LMM_ADDITIONAL_UPDATE_SMS_ONLY;
        /*NAS_MML_SetAdditionUpdateRslt(NAS_MML_ADDITION_UPDATE_RSLT_SMS_ONLY);*/
    }
    else
    {
        NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_SetAddUpdateRslt:enAddUpdateRslt = ,",
                               NAS_MML_ADDITION_UPDATE_RSLT_BUTT);

        pstEmmInfo->enAdditionUpRslt = NAS_LMM_ADDITIONAL_UPDATE_BUTT;
        /*NAS_MML_SetAdditionUpdateRslt(NAS_MML_ADDITION_UPDATE_RSLT_BUTT);*/
    }

    return;
}

NAS_LMM_ADDITIONAL_UPDATE_RSLT_ENUM_UINT32  NAS_EMM_GetAddUpdateRslt
(
   VOS_VOID
)
{
    NAS_LMM_ADDITIONAL_UPDATE_RSLT_ENUM_UINT32  enAddUpdateRslt;
    NAS_LMM_PUB_INFO_STRU                      *pstEmmInfo = VOS_NULL_PTR;

    pstEmmInfo = NAS_LMM_GetEmmInfoAddr();

    /*enAddUpdateRslt = NAS_MML_GetAdditionUpdateRslt();*/
    enAddUpdateRslt = pstEmmInfo->enAdditionUpRslt;

    NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_GetAddUpdateRslt:enAddUpdateRslt = ",
                            enAddUpdateRslt);

    NAS_LMM_LogAdditionUpdateRsltInfo(enAddUpdateRslt);
    return enAddUpdateRslt;
}

/* leili moidfy for isr end */
/* lihong00150010 ims end */

#if 0
VOS_UINT32  NAS_EMM_GetLaiOpBit( VOS_VOID)
{
    NAS_LMM_NETWORK_INFO_STRU            *pstNetInfo;
    VOS_UINT32                          ulRslt;

    pstNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    /*ulRslt = (pstNetInfo->bitOpPresentNetId) && (pstNetInfo->stPresentNetId.bitOpLac);*/

    ulRslt = pstNetInfo->stPresentNetId.bitOpLac;

    return ulRslt;
}
VOS_VOID  NAS_EMM_SetLai(const NAS_EMM_LA_STRU *pstLai)
{
    NAS_LMM_NETWORK_INFO_STRU            *pstNetInfo;

    pstNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    if (NAS_EMM_NULL_PTR == pstLai)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_SetLai: Point is null");

        return;
    }

    /*pstNetInfo->bitOpPresentNetId       = NAS_EMM_BIT_SLCT;*/
    pstNetInfo->stPresentNetId.bitOpLac = NAS_EMM_BIT_SLCT;

    NAS_LMM_MEM_CPY (                    &pstNetInfo->stPresentNetId.stPlmnId,
                                        &pstLai->stPlmnId,
                                        sizeof(NAS_MM_PLMN_ID_STRU));

    NAS_LMM_MEM_CPY (                    &pstNetInfo->stPresentNetId.stLac,
                                        &pstLai->stLac,
                                        sizeof(NAS_MM_LAC_STRU));
    return;
}
VOS_VOID  NAS_EMM_GetLai(NAS_EMM_LA_STRU *pstLai)
{
    NAS_LMM_NETWORK_INFO_STRU            *pstNetInfo;

    pstNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    if (NAS_EMM_NULL_PTR == pstLai)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_GetLai: Point is null");

        return;
    }

    NAS_LMM_MEM_CPY (                    &pstLai->stPlmnId,
                                        &pstNetInfo->stPresentNetId.stPlmnId,
                                        sizeof(NAS_MM_PLMN_ID_STRU));

    NAS_LMM_MEM_CPY (                    &pstLai->stLac,
                                        &pstNetInfo->stPresentNetId.stLac,
                                        sizeof(NAS_MM_LAC_STRU));
    return;
}
#endif


VOS_VOID NAS_EMM_GetCurrentTa( NAS_MM_TA_STRU *pstTa )
{
    NAS_EMM_NETWORK_ID_STRU             *pstAreaInfo;

    pstAreaInfo                         = NAS_LMM_GetEmmInfoNetInfoPresentNetAddr();

    NAS_LMM_PlmnCpy(&(pstTa->stPlmnId), &pstAreaInfo->stPlmnId);

    pstTa->stTac.ucTac                  = pstAreaInfo->stTac.ucTac;

    pstTa->stTac.ucTacCnt               = pstAreaInfo->stTac.ucTacCnt;

    return;
}

VOS_VOID NAS_EMM_AddForbTa
(
    const NAS_MM_TA_STRU               *pstTa,
    NAS_MM_TA_LIST_STRU                *pstTaList
)
{
    NAS_LMM_PUBM_LOG_NORM("NAS_EMMC_AddForbTa is entered");

    NAS_LMM_AddTaInTaList(pstTa, pstTaList, NAS_LMM_MAX_FORBTA_NUM);

    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_DEL_FORB_TA_PROID);
    return;
}



VOS_UINT32  NAS_LMM_IsLastAttemptRegTaInTaiList(VOS_VOID)
{
    NAS_MM_TA_STRU                     *pstLastAttmpRegTa = NAS_EMM_NULL_PTR;
    NAS_LMM_NETWORK_INFO_STRU          *pstNetInfo        = NAS_EMM_NULL_PTR;

    pstNetInfo                         = NAS_LMM_GetEmmInfoNetInfoAddr();
    pstLastAttmpRegTa                  = NAS_LMM_GetEmmInfoNetInfoLastAttmpRegTaAddr();

    if (NAS_EMM_BIT_NO_SLCT == pstNetInfo->bitOpLastAttmpRegTa)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_IsLastAttemptRegTaInTaiList: Not exist last attempt TA.");
        return NAS_EMM_NO;
    }

    if (NAS_LMM_MATCH_SUCCESS == NAS_LMM_TaMatchTaList(pstLastAttmpRegTa, NAS_LMM_GetEmmInfoNetInfoTaiListAddr()))
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_IsLastAttemptRegTaInTaiList: Last attempt TA is in TAI LIST.");
        return NAS_EMM_YES;
    }
    else
    {
        return NAS_EMM_NO;
    }

}



VOS_UINT32  NAS_EMM_IsRegStatusEmcRegingOrContainEmcBear(VOS_VOID)
{
    /* 处于紧急注册过程中 */
    if(NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        return NAS_EMM_YES;
    }

    /* 处于已经紧急注册或者正常注册有紧急承载 */
    if ((NAS_LMM_REG_STATUS_EMC_REGED == NAS_LMM_GetEmmInfoRegStatus())
        ||(NAS_LMM_REG_STATUS_NORM_REGED_AND_EMC_BEAR == NAS_LMM_GetEmmInfoRegStatus()))
    {
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;

}



VOS_VOID NAS_EMM_ClearGuti(VOS_VOID)
{
    NAS_LMM_GetEmmInfoUeidAddr()->bitOpGuti      = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_MEM_SET(                               NAS_LMM_GetEmmInfoUeidGutiAddr(),
                                                  0,
                                                  sizeof(NAS_EMM_GUTI_STRU));
    return;
}


VOS_VOID NAS_EMM_ClearRegInfo(VOS_UINT32 ulDeleteRplmn )
{
    /*删除GUTI*/
    NAS_EMM_ClearGuti();

    /*删除LVR TAI*/

    if (NAS_EMM_DELETE_RPLMN == ulDeleteRplmn)
    {
        NAS_EMM_ClearLVRTai();
    }
    else
    {
        NAS_EMM_SetLVRTacInvalid();
    }
    /*删除TAI list*/
    NAS_EMM_ClearTaiList();

    /*删除KSIasme*/
    /*NAS_EMM_ClearCurSecuCntxt();*/
    NAS_EMM_ClearAllSecuCntxt();

    /* 将无效的安全上下文直接在函数内部写卡 */
    NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);

    /*保存PS LOC信息*/
    NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
}
VOS_VOID NAS_EMM_ClearResumeInfo( VOS_VOID )
{
    NAS_EMM_FSM_STATE_STRU             *pstStateBeforeResume;

    NAS_LMM_PUBM_LOG_NORM( "NAS_EMM_ClearResumeInfoOriTypeDir: ResumeInfo  is  cleared!");

    pstStateBeforeResume = NAS_EMM_GetStateBeforeResumeAddr();

    pstStateBeforeResume->enMainState   = NAS_LMM_MAIN_STATE_BUTT;
    pstStateBeforeResume->enSubState    = MM_SS_BUTT;
    pstStateBeforeResume->enStaTId      = TI_NAS_EMM_STATE_NO_TIMER;

    NAS_EMM_SetResumeOri(NAS_LMM_SYS_CHNG_ORI_BUTT);
    NAS_EMM_SetResumeType(NAS_LMM_SYS_CHNG_TYPE_BUTT);
    NAS_EMM_SetResumeDir(LRRC_LMM_RSM_SYS_CHNG_DIR_BUTT);

    return;

}


VOS_VOID NAS_LMM_ClearEmmInfoMmcAttachReason( VOS_VOID )
{
    NAS_LMM_PUBM_LOG_NORM( "NAS_LMM_ClearEmmInfoMmcAttachReason is  enter!");
    NAS_LMM_SetEmmInfoMmcAttachReason(MMC_LMM_ATT_REASON_INITIAL);
    return;

}



VOS_VOID  NAS_EMM_LocalDetachProc( VOS_VOID )
{
    /* 停止所有EMM状态定时器 */
    NAS_LMM_StopAllEmmStateTimer();

    /* 停止所有协议定时器 */
    NAS_LMM_StopAllPtlTimer();

    /* 释放可能存在的动态内存 */
    NAS_LMM_FreeDynMem();

    /* 通知ESM清除资源 */
    NAS_EMM_PLMN_SendEmmEsmStautsInd(   EMM_ESM_ATTACH_STATUS_DETACHED);

    /*清除bar流程和类型*/
    NAS_EMM_ClearBarResouce();

    /*通知 RABM:RRC_CONN_REL_IND*/
    NAS_LMM_SendRabmRrcConRelInd();
}


VOS_UINT32  NAS_EMM_TransferUeSpecDrxCycleLen( VOS_VOID )
{
    VOS_UINT32                  ulUeSpecDrxCycle = 0;
    /*NAS_MM_DRX_STRU            *pstUeDrxParam = VOS_NULL_PTR;*/

    /*获取UE指定的DRX参数*/
    /*pstUeDrxParam = NAS_LMM_GetEmmInfoDrxAddr();*/
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*根据24008-930 10.5.5.6协议描述进行转换*/
    switch(NAS_EMM_GetUeEutranPsDrxLen())
    {
        case NAS_LMM_DEC_DIGIT_0:
            ulUeSpecDrxCycle = 0;
            break;

        case NAS_LMM_DEC_DIGIT_6:
            ulUeSpecDrxCycle = 32;
            break;

        case NAS_LMM_DEC_DIGIT_7:
            ulUeSpecDrxCycle = 64;
            break;

        case NAS_LMM_DEC_DIGIT_8:
            ulUeSpecDrxCycle = 128;
            break;
        case NAS_LMM_DEC_DIGIT_9:
            ulUeSpecDrxCycle = 256;
            break;

             /*其他值，报错,转换为0*/
        default:
            NAS_EMM_PUBU_LOG1_WARN("NAS_EMM_TransferUeSpecDrxCycleLen:Drx Err.",
                                        NAS_MML_GetUeEutranPsDrxLen());
            ulUeSpecDrxCycle = 0;
            break;
    }

     /*PC REPLAY MODIFY BY LEILI END*/
    return ulUeSpecDrxCycle;
}
VOS_VOID  NAS_EMM_SendUeSpecDrxCycleLenToRrc( VOS_VOID )
{
    LRRC_LNAS_INFO_STRU                   stNasInfo;
    VOS_UINT32                          ulUeSpecDrxCycle;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_SendUeSpecDrxCycleLenToRrc entered.");

    /*清空内存*/
    NAS_LMM_MEM_SET(&stNasInfo, 0, sizeof(LRRC_LNAS_INFO_STRU));

    /*获取全局变量中的DRX Param,并按照协议转换为 UeSpecDrxCycle*/
    ulUeSpecDrxCycle = NAS_EMM_TransferUeSpecDrxCycleLen();

    /*填充UE指定的 DRX_Cycle_Len*/
    stNasInfo.bitOpDrxLength            = NAS_EMM_BIT_SLCT;
    stNasInfo.ulDrxLength               = ulUeSpecDrxCycle;

    /*发送RRC_MM_INFO_CHANGE_REQ消息给RRC*/
    NAS_EMM_SendInfoChangeReqMsg(&stNasInfo);

    return;

}


VOS_VOID  NAS_EMM_SendInfoChangeReqMsg( CONST LRRC_LNAS_INFO_STRU *pstNasInfo )
{
    LRRC_LMM_NAS_INFO_CHANGE_REQ_STRU    *pstInfoChgReqMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgLen;

    /*计算长度不包含VOS消息头*/
    ulMsgLen     = NAS_EMM_MSG_ID_LEN + sizeof(LRRC_LNAS_INFO_STRU);

    /*分配内存*/
    pstInfoChgReqMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(ulMsgLen + NAS_EMM_LEN_VOS_MSG_HEADER);
    if(NAS_EMM_NULL_PTR == pstInfoChgReqMsg)
    {
        NAS_EMM_PUBU_LOG_WARN("NAS_EMM_SendInfoChangeReqMsg: MSG ALLOC ERR!");
        return;
    }

    /*清空申请到的消息空间*/
    PS_MEM_SET(pstInfoChgReqMsg,0,sizeof(LRRC_LMM_NAS_INFO_CHANGE_REQ_STRU));

    /*填充VOS消息头*/
    NAS_EMM_SET_RRC_MSG_HEADER(pstInfoChgReqMsg, ulMsgLen);

    /*填充消息ID*/
    pstInfoChgReqMsg->enMsgId           = ID_LRRC_LMM_NAS_INFO_CHANGE_REQ;

    /*填充消息体 */
    NAS_LMM_MEM_CPY(                    &(pstInfoChgReqMsg->stNasInfo),
                                        pstNasInfo,
                                        sizeof(LRRC_LNAS_INFO_STRU));

    /* 发送RRC_MM_NAS_INFO_CHANGE_REQ */
    NAS_LMM_SEND_MSG(pstInfoChgReqMsg);

    return;
}
VOS_VOID    NAS_EMM_PUBU_ClearResource(VOS_VOID)
{
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PUBU_ClearResource: enter.");

    /* 清除ESM的承载状态 */
    NAS_EMM_PubmInfoBearerContextInit();

    /* 清除保存的APP来的消息的参数 */
    /*NAS_EMM_ClearAppMsgPara();*/

    /* 清除最后一个发出的CN消息的类型 */
    NAS_LMM_GetLastAirMsgType() = EMM_MSG_TYPE_INVALID;

    return;
}



VOS_VOID    NAS_EMM_PUBU_SuspendClearResource(VOS_VOID)
{
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PUBU_SuspendClearResource: enter.");

    /* 清除ESM的承载状态 */
    /*NAS_EMM_PubmInfoBearerContextInit();*/

    /* 清除保存的APP来的消息的参数 */
    /*NAS_EMM_ClearAppMsgPara();*/

    /* 清除最后一个发出的CN消息的类型 */
    NAS_LMM_GetLastAirMsgType() = EMM_MSG_TYPE_INVALID;

    return;
}


/*****************************************************************************
 Function Name   : NAS_EMM_SetUsimState
 Description     : 设置卡状态
 Input           : NAS_LMM_SIM_STAT_ENUM_UINT32 ulUsimStatus
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-3-3  Draft Enact

*****************************************************************************/
VOS_VOID    NAS_EMM_SetUsimState( NAS_LMM_SIM_STAT_ENUM_UINT32 ulUsimStatus )
{
    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_SetUsimState: set ulUsimStatus = ", ulUsimStatus);

    if( (NAS_LMM_SIM_STATUS_UNAVAILABLE != ulUsimStatus) &&
        (NAS_LMM_SIM_STATUS_AVAILABLE != ulUsimStatus))
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SetUsimState: Input Para err ! ");
        return;
    }
    else
    {
        NAS_LMM_GetSimState() = ulUsimStatus;
        return;
    }
}


VOS_VOID NAS_EMM_LmmEmmInfo2MmcEmmInfo(const NAS_EMM_INFO_STRU *pstLmmEmmInfo, MMC_EMM_INFO_STRU *pstMmcEmmInfo)
{
    /* convert bit flag field */
    pstMmcEmmInfo->bitOpLocTimeZone        = pstLmmEmmInfo->bitOpLocTimeZone;
    pstMmcEmmInfo->bitOpUniTimeLocTimeZone = pstLmmEmmInfo->bitOpUniTimeLocTimeZone;
    pstMmcEmmInfo->bitOpDaylightTime       = pstLmmEmmInfo->bitOpDaylightTime;
    pstMmcEmmInfo->bitReserved             = pstLmmEmmInfo->bitShare;

    /* convert LTZ field */
    if (NAS_LMM_TIMEZONE_POSITIVE == pstLmmEmmInfo->stLocTimeZone.enPosNegTimeZone)
    {
        pstMmcEmmInfo->cLocTimeZone =  (VOS_INT8)(pstLmmEmmInfo->stLocTimeZone.ucTimeZone & 0x7F);
    }
    else
    {
        pstMmcEmmInfo->cLocTimeZone = -(VOS_INT8)(pstLmmEmmInfo->stLocTimeZone.ucTimeZone & 0x7F);
    }

    /* convert UT_LTZ field */
    pstMmcEmmInfo->stTimeZoneAndTime.ucYear   = pstLmmEmmInfo->stTimeZoneAndTime.ucYear;
    pstMmcEmmInfo->stTimeZoneAndTime.ucMonth  = pstLmmEmmInfo->stTimeZoneAndTime.ucMonth;
    pstMmcEmmInfo->stTimeZoneAndTime.ucDay    = pstLmmEmmInfo->stTimeZoneAndTime.ucDay;
    pstMmcEmmInfo->stTimeZoneAndTime.ucHour   = pstLmmEmmInfo->stTimeZoneAndTime.ucHour;
    pstMmcEmmInfo->stTimeZoneAndTime.ucMinute = pstLmmEmmInfo->stTimeZoneAndTime.ucMinute;
    pstMmcEmmInfo->stTimeZoneAndTime.ucSecond = pstLmmEmmInfo->stTimeZoneAndTime.ucSecond;
    if (NAS_LMM_TIMEZONE_POSITIVE == pstLmmEmmInfo->stTimeZoneAndTime.stTimeZone.enPosNegTimeZone)
    {
        pstMmcEmmInfo->stTimeZoneAndTime.cTimeZone =  (VOS_INT8)(pstLmmEmmInfo->stTimeZoneAndTime.stTimeZone.ucTimeZone & 0x7F);
    }
    else
    {
        pstMmcEmmInfo->stTimeZoneAndTime.cTimeZone = -(VOS_INT8)(pstLmmEmmInfo->stTimeZoneAndTime.stTimeZone.ucTimeZone & 0x7F);
    }
    /* convert DST field */
    switch(pstLmmEmmInfo->enDaylightSavingTime)
    {
    case NAS_LMM_DAYLIGHT_NO_ADJUST:
        pstMmcEmmInfo->enDaylightSavingTime = MMC_EMM_INFO_DST_NO_ADJUST;
        break;
    case NAS_LMM_DAYLIGHT_1_HOUR_ADJUST:
        pstMmcEmmInfo->enDaylightSavingTime = MMC_EMM_INFO_DST_1_HOUR_ADJUST;
        break;
    case NAS_LMM_DAYLIGHT_2_HOUR_ADJUST:
        pstMmcEmmInfo->enDaylightSavingTime = MMC_EMM_INFO_DST_2_HOUR_ADJUST;
        break;
    default:
        pstMmcEmmInfo->enDaylightSavingTime = MMC_EMM_INFO_DST_BUTT;
    }
}


VOS_VOID NAS_EMM_SendMmcEmmInfoInd(const NAS_EMM_INFO_STRU *pLmmEmmInfo)
{
    LMM_MMC_EMM_INFO_IND_STRU *pstMmcEmmInfoIndMsg = VOS_NULL_PTR;
    NAS_EMM_INFO_STRU                   *pstEmmInfo;


    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_SendMmcEmmInfoInd: enter");

    /* check input parameter */
    if (NAS_LMM_NULL_PTR == pLmmEmmInfo)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendMmcEmmInfoInd: invalid input parameter");
        return;
    }

    /* alloc memory */
    pstMmcEmmInfoIndMsg = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_EMM_INFO_IND_STRU));
    if (NAS_LMM_NULL_PTR == pstMmcEmmInfoIndMsg)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendMmcEmmInfoInd: MSG ALLOC ERROR");
        return;
    }

    /* initialization */
    NAS_LMM_MEM_SET(pstMmcEmmInfoIndMsg, 0, sizeof(LMM_MMC_EMM_INFO_IND_STRU));

    /* composite message header */
    EMM_PUBU_COMP_MMC_MSG_HEADER((pstMmcEmmInfoIndMsg), NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MMC_EMM_INFO_IND_STRU));

    /* assign message id */
    pstMmcEmmInfoIndMsg->ulMsgId = ID_LMM_MMC_EMM_INFO_IND;

    /* assign message context */
    NAS_EMM_LmmEmmInfo2MmcEmmInfo(pLmmEmmInfo, &pstMmcEmmInfoIndMsg->stLteEmmInfo);


    pstEmmInfo  = NAS_LMM_GetEmmInfoNasEmmInfoListAddr();

    if(NAS_EMM_BIT_SLCT == pstEmmInfo->bitOpFullName)
    {
        pstMmcEmmInfoIndMsg->stLteEmmInfo.bitOpLongName = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(pstMmcEmmInfoIndMsg->stLteEmmInfo.aucOperatorNameLong,
                        pLmmEmmInfo->aucOperatorNameLong,
                        MMC_LMM_MAX_OPER_LONG_NAME_LEN);
    }

    if(NAS_EMM_BIT_SLCT == pstEmmInfo->bitOpShortName)
    {
        pstMmcEmmInfoIndMsg->stLteEmmInfo.bitOpShortName = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(pstMmcEmmInfoIndMsg->stLteEmmInfo.aucOperatorNameShort,
                        pLmmEmmInfo->aucOperatorNameShort,
                        MMC_LMM_MAX_OPER_SHORT_NAME_LEN);
    }

    /* send message to MMC */
    NAS_LMM_SendLmmMmcMsg(pstMmcEmmInfoIndMsg);
}
VOS_VOID    NAS_EMM_SendMmcStatusInd(
                                MMC_LMM_STATUS_TYPE_ENUM_UINT32 ulMmcLmmStaType,
                                MMC_LMM_CONN_STATE_ENUM_UINT32  ulConnState )
{
    LMM_MMC_STATUS_IND_STRU           *pstMmcStatusIndMsg = VOS_NULL_PTR;

    NAS_EMM_PUBU_LOG2_INFO("NAS_EMM_SendMmcStatusInd: ulMmcLmmStaType = , ulConnState =",
                        ulMmcLmmStaType,
                        ulConnState);

    /* 申请MMC内部消息 */
    pstMmcStatusIndMsg = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_STATUS_IND_STRU));
    if (NAS_LMM_NULL_PTR == pstMmcStatusIndMsg)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendMmcStatusInd: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstMmcStatusIndMsg, 0, sizeof(LMM_MMC_STATUS_IND_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER((pstMmcStatusIndMsg),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MMC_STATUS_IND_STRU));

    /* 填充消息ID */
    pstMmcStatusIndMsg->ulMsgId         = ID_LMM_MMC_STATUS_IND;

    /* 填充消息体 */
    pstMmcStatusIndMsg->ulOpId          = NAS_EMM_OPID_MMC;
    pstMmcStatusIndMsg->ulStatusType    = ulMmcLmmStaType;

    /* 如果上报的类型为MMC_LMM_STATUS_TYPE_CONN_STATE，则填充连接状态*/
    if(MMC_LMM_STATUS_TYPE_CONN_STATE == ulMmcLmmStaType)
    {
        pstMmcStatusIndMsg->bitOpConnState  = NAS_EMM_BIT_SLCT;
        pstMmcStatusIndMsg->ulConnState     = ulConnState;
    }

    /* 发送MMC内部消息 */
    NAS_LMM_SendLmmMmcMsg(pstMmcStatusIndMsg);

    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_SendMmcUsimStatusCnf
 Description     : 向MMC发送LMM_MMC_USIM_STATUS_CNF消息
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-7-2  Draft Enact

*****************************************************************************/
VOS_VOID NAS_EMM_SendMmcUsimStatusCnf( VOS_VOID )
{
    LMM_MMC_USIM_STATUS_CNF_STRU       *pstMmcUsimStatusCnf = VOS_NULL_PTR;

    /* 申请MMC内部消息*/
    pstMmcUsimStatusCnf = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_USIM_STATUS_CNF_STRU));
    if (NAS_LMM_NULL_PTR == pstMmcUsimStatusCnf)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendMmcUsimStatusCnf: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstMmcUsimStatusCnf, 0, sizeof(LMM_MMC_USIM_STATUS_CNF_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER((pstMmcUsimStatusCnf),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MMC_USIM_STATUS_CNF_STRU));

    /* 填充消息ID */
    pstMmcUsimStatusCnf->ulMsgId    = ID_LMM_MMC_USIM_STATUS_CNF;

    /* 填充消息体 */
    pstMmcUsimStatusCnf->ulOpId     = NAS_EMM_OPID_MMC;

    /* 发送MMC内部消息 */
    NAS_LMM_SendLmmMmcMsg(pstMmcUsimStatusCnf);

    return;
}
VOS_UINT32  NAS_EMM_CheckSupportGUMode( VOS_VOID)
{
    if ( ((NAS_LMM_RAT_PRIO_NULL < NAS_LMM_GetCurGsmPrio())
       &&(NAS_LMM_RAT_PRIO_BUTT > NAS_LMM_GetCurGsmPrio()))
      ||((NAS_LMM_RAT_PRIO_NULL < NAS_LMM_GetCurWcdmaPrio())
       &&(NAS_LMM_RAT_PRIO_BUTT > NAS_LMM_GetCurWcdmaPrio())))
    {
        return NAS_LMM_SUCC;
    }
    return NAS_EMM_FAIL;
}



VOS_UINT32  NAS_EMM_CheckMutiModeSupport(VOS_VOID  )
{
    VOS_UINT32                          ulRslt = NAS_EMM_FAIL;

    if ( (NAS_LMM_RAT_PRIO_NULL == NAS_LMM_GetCurLtePrio ())
       ||(NAS_LMM_RAT_PRIO_BUTT == NAS_LMM_GetCurLtePrio ()))
    {
        ulRslt = NAS_LMM_FAIL;
    }
    else if( (NAS_LMM_RAT_PRIO_NULL < NAS_LMM_GetCurLtePrio ())
            &&(NAS_LMM_RAT_PRIO_BUTT > NAS_LMM_GetCurLtePrio ()))
    {
        ulRslt = NAS_EMM_CheckSupportGUMode();
    }
    else
    {
    }
    return ulRslt;

}


VOS_UINT32  NAS_EMM_CheckPtmsiAndRaiValidity( VOS_VOID )
{
    NAS_GUMM_INFO_STRU                  stGuInfo;
    MMC_LMM_RESULT_ID_ENUM_UINT32       ulRslt1;
    MMC_LMM_RESULT_ID_ENUM_UINT32       ulRslt2;
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*判断P-TMSI 和 RAI是否有效*/
    ulRslt1 = NAS_EMM_GetGuInfo(NAS_GUMM_PTMSI,&stGuInfo);

    ulRslt2 = NAS_EMM_GetGuInfo(NAS_GUMM_RAI,&stGuInfo);
    /*PC REPLAY MODIFY BY LEILI END*/

    if ((MMC_LMM_SUCC == ulRslt1) &&
        (MMC_LMM_SUCC == ulRslt2))
    {

        return NAS_EMM_SUCC;
    }

    return NAS_EMM_FAIL;

}
VOS_VOID  NAS_EMM_GetPtmsiAndRai
(
    NAS_GUMM_PTMSI_STRU     *pstPtmsi,
    MMC_LMM_RAI_STRU        *pstRai
)
{
    NAS_GUMM_INFO_STRU                  stGuInfo;
    MMC_LMM_RESULT_ID_ENUM_UINT32       ulRslt1;
    MMC_LMM_RESULT_ID_ENUM_UINT32       ulRslt2;

    /*初始化*/
    NAS_LMM_MEM_SET(&stGuInfo, 0, sizeof(NAS_GUMM_INFO_STRU));
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*判断P-TMSI 和 RAI是否有效*/
    ulRslt1 = NAS_EMM_GetGuInfo(NAS_GUMM_PTMSI,&stGuInfo);

    if (MMC_LMM_SUCC == ulRslt1)
    {
        NAS_LMM_MEM_CPY(                 pstPtmsi,
                                        &(stGuInfo.u.stPtmsi),
                                        sizeof(NAS_GUMM_PTMSI_STRU));
    }

    ulRslt2 = NAS_EMM_GetGuInfo(NAS_GUMM_RAI,&stGuInfo);

    if (MMC_LMM_SUCC == ulRslt2)
    {
        NAS_LMM_MEM_CPY(                 pstRai,
                                        &(stGuInfo.u.stRAI),
                                        sizeof(MMC_LMM_RAI_STRU));
    }
    /*PC REPLAY MODIFY BY LEILI END*/
    return;
}

VOS_VOID  NAS_EMM_DeriveMappedGuti
(
    NAS_LMM_GUTI_STRU                   *pstMappedGuti
)
{
    NAS_GUMM_PTMSI_STRU                 stPtmsi;
    MMC_LMM_RAI_STRU                    stRai;

    NAS_EMM_PUBU_LOG_NORM("NAS_EMM_DeriveMappedGuti entered.");

    if(VOS_NULL_PTR == pstMappedGuti)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_DeriveMappedGuti: Input para is invalid");
        return;
    }
    /* 清空入参空间*/
    NAS_LMM_MEM_SET(pstMappedGuti, 0, sizeof(NAS_LMM_GUTI_STRU))

    /* 获取 P-TMSI和 RAI*/
    NAS_EMM_GetPtmsiAndRai(&stPtmsi,&stRai);

    /* 组装Mapped GUTI*/
    pstMappedGuti->stGutiHead.ucLenth   = NAS_EMM_AD_LEN_GUTI;
    pstMappedGuti->stGutiHead.ucOeToi   = NAS_EMM_AD_ID_GUTI;

    NAS_LMM_MEM_CPY(                    &(pstMappedGuti->stPlmnId),
                                       &(stRai.stLai.stPlmnId),
                                        sizeof(NAS_MM_PLMN_ID_STRU));
    pstMappedGuti->stMmeGroupId.ucGroupId = stRai.stLai.stLac.ucLac;
    pstMappedGuti->stMmeGroupId.ucGroupIdCnt = stRai.stLai.stLac.ucLacCnt;
    pstMappedGuti->stMmeCode.ucMmeCode  = stPtmsi.aucPtmsi[1];
    pstMappedGuti->stMTmsi.ucMTmsi      = ((stPtmsi.aucPtmsi[0])|(NAS_EMM_HITH_2_BITS_F));
    pstMappedGuti->stMTmsi.ucMTmsiCnt1  = stRai.ucRac;
    pstMappedGuti->stMTmsi.ucMTmsiCnt2  = stPtmsi.aucPtmsi[2];
    pstMappedGuti->stMTmsi.ucMTmsiCnt3  = stPtmsi.aucPtmsi[3];

    return;
}


/*leili modify for isr begin*/
/*PC REPLAY MODIFY BY LEILI BEGIN*/

VOS_UINT32  NAS_EMM_GetTinType(VOS_VOID )
{
    MMC_LMM_TIN_TYPE_ENUM_UINT32        ulTinType;

    #ifndef __PS_WIN32_RECUR__
    ulTinType = NAS_MML_GetTinType();

    NAS_LMM_LogTinInfo(ulTinType);
    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_GetTinType: ulTinType = ", ulTinType);

    #else

    ulTinType = Stub_NAS_MML_GetTinType();
    #endif
    return ulTinType;
}
/*PC REPLAY MODIFY BY LEILI END*/
#if 0
/*****************************************************************************
 Function Name   : NAS_EMM_SendMmcTinTypeInd
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-4-26  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_SendMmcTinTypeInd(MMC_LMM_TIN_TYPE_ENUM_UINT32    ulTinType)
{
    LMM_MMC_TIN_TYPE_IND_STRU           *pstMmcTinTypeIndMsg;

    /* 申请MMC内部消息 */
    pstMmcTinTypeIndMsg = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_TIN_TYPE_IND_STRU));
    if (NAS_LMM_NULL_PTR == pstMmcTinTypeIndMsg)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_SendMmcTinTypeInd: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstMmcTinTypeIndMsg,0,sizeof(LMM_MMC_TIN_TYPE_IND_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER((pstMmcTinTypeIndMsg),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MMC_TIN_TYPE_IND_STRU));

    /* 填充消息ID */
    pstMmcTinTypeIndMsg->ulMsgId        = ID_LMM_MMC_TIN_TYPE_IND;

    /* 填充消息体 */
    pstMmcTinTypeIndMsg->ulOpId         = NAS_EMM_OPID_MMC;
    pstMmcTinTypeIndMsg->ulTinType      = ulTinType;

    /* 发送MMC内部消息 */
    NAS_LMM_SendLmmMmcMsg(pstMmcTinTypeIndMsg);

    return;
}
#endif
VOS_VOID  NAS_EMM_SendTimerStateNotify
(
    NAS_LMM_PTL_TI_ENUM_UINT16      ulTimerId,
    LMM_MMC_TIMER_STATE_ENUM_UINT32  ulTimerState
)
{
    LMM_MMC_TIMER_STATE_NOTIFY_STRU           *pstTimerStateMsg;

    /* 申请MMC内部消息 */
    pstTimerStateMsg = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_TIMER_STATE_NOTIFY_STRU));

    NAS_EMM_PUBU_LOG2_INFO("NAS_EMM_SendTimerStateNotify", ulTimerId, ulTimerState);

    if (NAS_LMM_NULL_PTR == pstTimerStateMsg)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_WARN("NAS_EMM_SendTimerStateNotify: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstTimerStateMsg,0,sizeof(LMM_MMC_TIMER_STATE_NOTIFY_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER((pstTimerStateMsg),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MMC_TIMER_STATE_NOTIFY_STRU));

    /* 填充消息ID */
    pstTimerStateMsg->ulMsgId           = ID_LMM_MMC_TIMER_STATE_NOTIFY;

    /* 填充消息体 */
    pstTimerStateMsg->ulOpId            = NAS_EMM_OPID_MMC;

    if (TI_NAS_EMM_PTL_T3412 == ulTimerId)
    {
        pstTimerStateMsg->bitOpT3412    = NAS_EMM_BIT_SLCT;
        pstTimerStateMsg->enT3412State  = ulTimerState;
    }
    else if (TI_NAS_EMM_PTL_T3423 == ulTimerId)
    {
        pstTimerStateMsg->bitOpT3423    = NAS_EMM_BIT_SLCT;
        pstTimerStateMsg->enT3423State  = ulTimerState;
    }
    else
    {
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_SendTimerStateNotify: Timer type is err!!!");
    }

    /* 发送MMC内部消息 */
    NAS_LMM_SendLmmMmcMsg(pstTimerStateMsg);

    return;
}

/*leili modify for isr end*/


VOS_VOID LNAS_LPSOM_GetCampedCellInfo(LPS_NAS_MM_TA_LIST_STRU *pstRegPlmnTaList,
    LPS_NAS_IMSI_STRU *pstImsiInfo,LPS_NAS_MM_GUTI_STRU *pstGutiInfo)
{
    VOS_UINT8                          *pucImsi;

    pucImsi = (VOS_UINT8 *)NAS_LMM_GetEmmInfoUeidImsiAddr();
    NAS_LMM_MEM_CPY(pstRegPlmnTaList, NAS_LMM_GetEmmInfoNetInfoTaiListAddr(), sizeof(NAS_MM_TA_LIST_STRU));
    NAS_LMM_MEM_CPY(pstGutiInfo, NAS_LMM_GetEmmInfoUeidGutiAddr(), sizeof(NAS_MM_GUTI_STRU));
    NAS_LMM_MEM_CPY(&(pstImsiInfo->aucImsi[0]),pucImsi,NAS_LMM_NVIM_IMSI_FILE_LEN);

    return;
}
/*****************************************************************************
 Function Name  : NAS_EMM_IsCsPsUeMode
 Description    : 判断是否为CP/PS1或者CS/PS2
 Input          : None
 Output         : None
 Return Value   : VOS_UINT32

 History        :
      1.lihong00150010      2011-08-27  Draft Enact
*****************************************************************************/
VOS_UINT32 NAS_EMM_IsCsPsUeMode( VOS_VOID )
{
    if ((NAS_LMM_UE_CS_PS_MODE_1 == NAS_LMM_GetEmmInfoUeOperationMode())
        || (NAS_LMM_UE_CS_PS_MODE_2 == NAS_LMM_GetEmmInfoUeOperationMode()))
    {
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}


/*****************************************************************************
 Function Name   : Nas_InitUeModeInfo
 Description     : 初始化GU的UE MODE 信息
 Input           : ulInfoType:
 Output          :
 Return          :
 History         :
     1.    hanlufeng 41410       2011-9-27   Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_PubmInfoUeModeInit(VOS_VOID)
{
    NAS_LMM_UE_OPERATION_MODE_ENUM_UINT32       ulLteUeMode;
    NAS_EMM_PUB_INFO_STRU                       *pstPubInfo = NAS_LMM_GetEmmInfoAddr();


    /* 给全局变量赋值

    NAS_LMM_GU_UE_MODE_ENUM_UINT32              ulGuUeMode;
    NAS_LMM_UE_OPERATION_MODE_ENUM_UINT32       ulLteUeMode;
    NAS_EMM_USAGE_SETTING_UINT8                 ulUsageSetting;
    NAS_LMM_CS_SERVICE_ENUM_UINT32              ulCsService;


    NAS_LMM_SUP_ENUM_UINT32                     aulLmmSupVoiceDomain[NAS_LMM_SUP_VOICE_DOMAIN_BUTT];
    NAS_LMM_VOICE_DOMAIN_ENUM_UINT32            ulVoiceDomain;

    NAS_LMM_SUP_ENUM_UINT32                     aulLmmSupSmsDomain[NAS_LMM_SUP_SMS_DOMAIN_BUTT];
    NAS_LMM_SUP_SMS_DOMAIN_ENUM_UINT32          ulSmsDomainPrefer;

    NAS_LMM_SUP_ENUM_UINT32                     ulLmmSupIms;
    NAS_LMM_SUP_ENUM_UINT32                     ulCSFBAvail;

    NAS_LMM_EMERGENCY_NUM_LIST_STRU             stEmergencyNumList;
    VOS_UINT8                                   ucNetFeature;
    VOS_UINT8                                   aucRsv1[3];
    NAS_LMM_ADDITIONAL_UPDATE_RSLT_ENUM_UINT32  ulAdditionUpRslt;
    */

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PubmInfoUeModeInit:  UE MODE init begin: ");


    /* 该值随后在读NV时更新 NAS_EMM_ReadNvUeOperationMode，届时也同步更新LTE UE MODE*/
    NAS_LMM_SetEmmInfoGuUeMode(NAS_LMM_GU_UE_MODE_PS);
    NAS_EMM_PUBU_LOG1_INFO("GU UE MODE = ",
                            NAS_MML_GetMsMode());

    NAS_LMM_SetEmmInfoUeCenter(EMM_SETTING_DATA_CENTRIC);
    NAS_EMM_PUBU_LOG1_INFO("UE CENTER = ",
                            NAS_LMM_GetEmmInfoUeCenter());

    /* 生成对应的LTE UE MODE */
    ulLteUeMode = NAS_LMM_ComputeLteUeMode( NAS_MML_GetMsMode(),
                                            NAS_LMM_GetEmmInfoUeCenter());
    NAS_LMM_SetEmmInfoUeOperationMode(ulLteUeMode);
    NAS_EMM_PUBU_LOG1_INFO("LTE UE MODE = ",
                            NAS_LMM_GetEmmInfoUeOperationMode());

    pstPubInfo->ulCsService = NAS_LMM_CS_SERVICE_BUTT;  /* 该值随后在读NV时更新 */
    NAS_EMM_PUBU_LOG1_INFO("ulCsService = ",
                            pstPubInfo->ulCsService);

    pstPubInfo->aulLmmSupVoiceDomain[NAS_LMM_SUP_VOICE_DOMAIN_CS]  = NAS_LMM_NOT_SUP;
    pstPubInfo->aulLmmSupVoiceDomain[NAS_LMM_SUP_VOICE_DOMAIN_IMS] = NAS_LMM_NOT_SUP;
    NAS_EMM_PUBU_LOG2_INFO("aulLmmSupVoiceDomain CS and PS = ",
                            pstPubInfo->aulLmmSupVoiceDomain[NAS_LMM_SUP_VOICE_DOMAIN_CS],
                            pstPubInfo->aulLmmSupVoiceDomain[NAS_LMM_SUP_VOICE_DOMAIN_IMS]);

    /* 该值随后在读NV时更新 NAS_EMM_ReadNvVoiceDomain*/
    pstPubInfo->ulVoiceDomain = NAS_LMM_VOICE_DOMAIN_BUTT;
    NAS_EMM_PUBU_LOG1_INFO("ulVoiceDomain = ",
                            pstPubInfo->ulVoiceDomain);

    pstPubInfo->aulLmmSupSmsDomain[NAS_LMM_SUP_SMS_DOMAIN_SGS]  = NAS_LMM_NOT_SUP;
    pstPubInfo->aulLmmSupSmsDomain[NAS_LMM_SUP_SMS_DOMAIN_IP ]  = NAS_LMM_NOT_SUP;
    NAS_EMM_PUBU_LOG2_INFO("aulLmmSupSmsDomain SGS and IP = ",
                            pstPubInfo->aulLmmSupSmsDomain[NAS_LMM_SUP_SMS_DOMAIN_SGS],
                            pstPubInfo->aulLmmSupSmsDomain[NAS_LMM_SUP_SMS_DOMAIN_IP]);

    /* 由于暂不支持SMS，故填为无效值 */
    pstPubInfo->ulSmsDomainPrefer = NAS_LMM_SUP_SMS_DOMAIN_BUTT;
    NAS_EMM_PUBU_LOG1_INFO("ulSmsDomainPrefer = ",
                            pstPubInfo->ulSmsDomainPrefer);

    pstPubInfo->ulLmmSupIms = NAS_LMM_NOT_SUP;
    NAS_EMM_PUBU_LOG1_INFO("ulLmmSupIms = ",
                            pstPubInfo->ulLmmSupIms);

    /*
    pstPubInfo->ulCSFBAvail = NAS_LMM_NOT_SUP;
    NAS_EMM_PUBU_LOG1_INFO("ulCSFBAvail = ",
                            pstPubInfo->ulCSFBAvail);
    */
    /* lihong00150010 ims begin */
    /* 初始值赋为NOT_ATTACHED */
    pstPubInfo->enAdditionUpRslt = NAS_LMM_ADDITIONAL_UPDATE_NOT_ATTACHED;
    /* lihong00150010 ims end */

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PubmInfoUeModeInit:  UE MODE init END. ");

    return;
}
VOS_UINT32  Nas_LMM_SetLteUeModeInfo(
    NAS_LMM_UEMODE_TYPE_ENUM_UINT32     ulInfoType,
    NAS_LMM_UEMODE_INFO_STRU           *pstLmmUeModeInfo)
{

    NAS_EMM_PUBU_LOG2_INFO("Nas_LMM_SetLteUeModeInfo:  ulInfoType = , pstLmmUeModeInfo = ",
                            ulInfoType,
                            pstLmmUeModeInfo);

    /* 入参检查 */
    if((NAS_EMM_NULL_PTR == pstLmmUeModeInfo) ||
        (NAS_LMM_UEMODE_INFO_GU_UEMODE != ulInfoType))
    {

        NAS_EMM_PUBU_LOG_WARN("Nas_LMM_SetLteUeModeInfo: ERR.");
        return  NAS_EMM_FAIL;
    }

    NAS_EMM_PUBU_LOG_INFO("Nas_LMM_SetLteUeModeInfo: NAS_LMM_UEMODE_INFO_STRU content are :");
    NAS_COMM_PrintArray(             NAS_COMM_GET_MM_PRINT_BUF(),
                                     (VOS_UINT8 *)pstLmmUeModeInfo,
                                     sizeof(NAS_LMM_UEMODE_INFO_STRU));

    /*Coverity 告警消除 2013-9-26 Begin*/
    NAS_LMM_SetGuUeMode(pstLmmUeModeInfo->u.ulGuUeMode);
    /*Coverity 告警消除 2013-9-26 End*/

    return  NAS_EMM_SUCC;
}

/*****************************************************************************
 Function Name   : Nas_GetLteUeModeInfo
 Description     : 设置GU的UE MODE，给LNAS的内部模块调用
 Input           : ulInfoType:
 Output          :
 Return          :
 History         :
    1.Hanlufeng 41410      2011-10-13  Draft Enact

*****************************************************************************/
VOS_UINT32  Nas_LMM_GetLteUeModeInfo(
    NAS_LMM_UEMODE_TYPE_ENUM_UINT32     ulInfoType,
    NAS_LMM_UEMODE_INFO_STRU           *pstLmmUeModeInfo)
{

    NAS_EMM_PUB_INFO_STRU                       *pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    NAS_EMM_PUBU_LOG2_INFO("Nas_LMM_GetLteUeModeInfo:  ulInfoType = , pstLmmUeModeInfo = ",
                            ulInfoType,
                            pstLmmUeModeInfo);

    /* 入参检查 */
    if((NAS_EMM_NULL_PTR == pstLmmUeModeInfo) ||
        (NAS_LMM_UEMODE_INFO_BUTT <= ulInfoType))
    {
        NAS_EMM_PUBU_LOG_WARN("Nas_LMM_GetLteUeModeInfo:  ERR");
        return  NAS_EMM_FAIL;
    }

    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    switch(ulInfoType)
    {
        case    NAS_LMM_UEMODE_INFO_GU_UEMODE:
                pstLmmUeModeInfo->u.ulGuUeMode = NAS_EMM_GetMsMode();
                break;

        case    NAS_LMM_UEMODE_INFO_CENTER:
                pstLmmUeModeInfo->u.ulUsageSetting = NAS_LMM_GetEmmInfoUeCenter();
                break;

        case    NAS_LMM_UEMODE_INFO_LTE_UEMODE:
                pstLmmUeModeInfo->u.ulLteUeMode
                    = NAS_LMM_ComputeLteUeMode( NAS_EMM_GetMsMode(),
                                                NAS_LMM_GetEmmInfoUeCenter());
                break;
        /*PC REPLAY MODIFY BY LEILI END*/
        case    NAS_LMM_UEMODE_INFO_SUP_VOICE_DOMAIN:
                pstLmmUeModeInfo->u.aulLmmSupVoiceDomain[NAS_LMM_SUP_VOICE_DOMAIN_CS ]
                    = pstPubInfo->aulLmmSupVoiceDomain[NAS_LMM_SUP_VOICE_DOMAIN_CS ];
                pstLmmUeModeInfo->u.aulLmmSupVoiceDomain[NAS_LMM_SUP_VOICE_DOMAIN_IMS]
                    = pstPubInfo->aulLmmSupVoiceDomain[NAS_LMM_SUP_VOICE_DOMAIN_IMS ];
                break;

        case    NAS_LMM_UEMODE_INFO_VOICE_DOMAIN_PRE:
                pstLmmUeModeInfo->u.ulVoiceDomain
                    = pstPubInfo->ulVoiceDomain;
                break;

        case    NAS_LMM_UEMODE_INFO_SUP_SMS_DOMAIN:
                pstLmmUeModeInfo->u.aulLmmSupSmsDomain[NAS_LMM_SUP_SMS_DOMAIN_SGS ]
                    = pstPubInfo->aulLmmSupSmsDomain[NAS_LMM_SUP_SMS_DOMAIN_SGS];
                pstLmmUeModeInfo->u.aulLmmSupSmsDomain[NAS_LMM_SUP_SMS_DOMAIN_IP]
                    = pstPubInfo->aulLmmSupSmsDomain[NAS_LMM_SUP_SMS_DOMAIN_IP ];
                break;

        case    NAS_LMM_UEMODE_INFO_SMS_DOMAIN_PRE:
                pstLmmUeModeInfo->u.ulSmsDomainPrefer
                    = pstPubInfo->ulSmsDomainPrefer;
                break;

        case    NAS_LMM_UEMODE_INFO_SUP_IMS:
                pstLmmUeModeInfo->u.ulLmmSupIms
                    = pstPubInfo->ulLmmSupIms;
                break;

        case    NAS_LMM_UEMODE_INFO_NET_FEATURE:
                pstLmmUeModeInfo->u.ucNetFeature
                    = pstPubInfo->ucNetFeature;
                break;

        default:
                NAS_EMM_PUBU_LOG_WARN("Nas_LMM_GetLteUeModeInfo: TYPE ERR.");
                return  NAS_EMM_FAIL;
    }

    NAS_EMM_PUBU_LOG_INFO("Nas_LMM_GetLteUeModeInfo: NAS_LMM_UEMODE_INFO_STRU content are :");
    NAS_COMM_PrintArray(             NAS_COMM_GET_MM_PRINT_BUF(),
                                     (VOS_UINT8 *)pstLmmUeModeInfo,
                                     sizeof(NAS_LMM_UEMODE_INFO_STRU));

    return  NAS_EMM_SUCC;
}

/*****************************************************************************
 Function Name   : NAS_LMM_SetGuUeMode
 Description     : GU UE MODE改变会导致LTE UE MODE修改
                   从GU的UE MODE和UE CENTER生成LTE UE MODE，并设置入全局变量
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.Hanlufeng 41410      2011-10-10  Draft Enact

*****************************************************************************/
VOS_VOID NAS_LMM_SetGuUeMode
(
    NAS_LMM_GU_UE_MODE_ENUM_UINT32      ulGuUeMode
)
{
    NAS_LMM_UE_OPERATION_MODE_ENUM_UINT32   ulLteUeModeCur;
    NAS_LMM_UE_OPERATION_MODE_ENUM_UINT32   ulLteUeModeNew;

    NAS_EMM_PUBU_LOG1_INFO("NAS_LMM_SetGuUeMode: income ulGuUeMode = ", ulGuUeMode);
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /* 获取当前的 LTE UE MODE */
    ulLteUeModeCur = NAS_LMM_ComputeLteUeMode(NAS_EMM_GetMsMode(),
                                              NAS_LMM_GetEmmInfoUeCenter());

    /* 从GU的UE MODE和UE CENTER生成LTE UE MODE */
    ulLteUeModeNew = NAS_LMM_ComputeLteUeMode(NAS_EMM_GetMsMode(), NAS_LMM_GetEmmInfoUeCenter());
    /*PC REPLAY MODIFY BY LEILI END*/
    /* 打印变化前后的数据 */
    NAS_EMM_PUBU_LOG1_INFO(" NAS_LMM_SetGuUeMode: CUR LTE_UE_MODE = ", ulLteUeModeCur);
    NAS_EMM_PUBU_LOG_INFO("  LTE_UE_MODE change   ===>");
    NAS_EMM_PUBU_LOG1_INFO(" NAS_LMM_SetGuUeMode: NEW LTE_UE_MODE = ", ulLteUeModeNew);


    /* LMM的全局变量设置GU UE MODE */
    NAS_LMM_SetEmmInfoGuUeMode(ulGuUeMode);
    NAS_LMM_SetEmmInfoUeOperationMode(ulLteUeModeNew);

    /* Ue Center 变化对应的各模块的操作 */
    NAS_LMM_LteModeChange(ulLteUeModeCur, ulLteUeModeNew);

    return;
}
NAS_LMM_GU_UE_MODE_ENUM_UINT32  NAS_LMM_GetEmmInfoGuUeMode(VOS_VOID)
{
    /* 返回GU OPERATION MODE，MML开放后用MML函数代替 */
    NAS_LMM_GU_UE_MODE_ENUM_UINT32      ulGuOperationMode;

    ulGuOperationMode       = NAS_LMM_GetEmmInfoAddr()->ulGuUeMode;
    return ulGuOperationMode;
}
NAS_LMM_UE_OPERATION_MODE_ENUM_UINT32  NAS_LMM_GetEmmInfoUeOperationMode(VOS_VOID)
{
    /* 返回lte OPERATION MODE，
        实时获取GU OPRATION MODE，加上ue setting 计算出LTE OPERATION MODE*/
    NAS_LMM_UE_OPERATION_MODE_ENUM_UINT32      ullteOperationMode;
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    ullteOperationMode = NAS_LMM_ComputeLteUeMode( NAS_EMM_GetMsMode(),
                                            NAS_LMM_GetEmmInfoUeCenter());
    /*PC REPLAY MODIFY BY LEILI END*/
    return ullteOperationMode;
}

/*****************************************************************************
 Function Name   : NAS_LMM_LteModeChange
 Description     : 各模块在此添加函数 ,以把LTE UE MODE的变更通知给自己
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.Hanlufeng 41410      2011-10-13  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_LMM_LteModeChange(
    NAS_LMM_UE_OPERATION_MODE_ENUM_UINT32   ulLteUeModeCur,
    NAS_LMM_UE_OPERATION_MODE_ENUM_UINT32   ulLteUeModeNew)
{
    /* 需要及时了解LTE UE MODE变更的模块在此添加回调函数 */

    (void)ulLteUeModeCur;
    (void)ulLteUeModeNew;
    return;
}
NAS_LMM_UE_OPERATION_MODE_ENUM_UINT32   NAS_LMM_ComputeLteUeMode
(
    NAS_MML_MS_MODE_ENUM_UINT8          ulGuUeMode,
    NAS_EMM_USAGE_SETTING_UINT32        ulUeCenter
)
{

    NAS_EMM_PUBU_LOG2_INFO("NAS_LMM_ComputeLteUeMode: ulGuUeMode =,  ucUeCenter = ",
                             ulGuUeMode, ulUeCenter);

    NAS_EMM_PUBU_LOG_INFO("Compute out : LTE UE MODE = ");

    /* 从GU的UE MODE和UE CENTER生成LTE UE MODE */
    if( (NAS_MML_MS_MODE_PS_ONLY == ulGuUeMode) ||
        (NAS_MML_MS_MODE_CS_ONLY == ulGuUeMode))
    {   /* GU PS  ,GU的CS没有协议对应，SRS要求当做LTE 的PS来处理 */

        if(EMM_SETTING_DATA_CENTRIC == ulUeCenter)
        {
            NAS_EMM_PUBU_LOG_INFO(" NAS_LMM_UE_PS_MODE_2 ");
            return  NAS_LMM_UE_PS_MODE_2;
        }
        else
        {
            NAS_EMM_PUBU_LOG_INFO(" NAS_LMM_UE_PS_MODE_1 ");
            return  NAS_LMM_UE_PS_MODE_1;
        }
    }
    else
    {   /*CS_PS*/

        if(EMM_SETTING_DATA_CENTRIC == ulUeCenter)
        {
            NAS_EMM_PUBU_LOG_INFO(" NAS_LMM_UE_CS_PS_MODE_2 ");
            return  NAS_LMM_UE_CS_PS_MODE_2;
        }
        else
        {
            NAS_EMM_PUBU_LOG_INFO("NAS_LMM_UE_CS_PS_MODE_1 ");
            return  NAS_LMM_UE_CS_PS_MODE_1;
        }
    }
}
/*****************************************************************************
 Function Name   : NAS_EMM_lteRatIsExist
 Description     :
 Input           :
 Output          :
 Return          : NAS_LMM_FAIL:接入技术中无L
                   NAS_LMM_SUCC:接入技术中有L
 History         :
    1.ZHANGCAIXIA      2011-9-9  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_lteRatIsExist(VOS_VOID)
{
    if ( (NAS_LMM_RAT_PRIO_NULL ==  NAS_LMM_GetCurLtePrio())
      ||(NAS_LMM_RAT_PRIO_BUTT == NAS_LMM_GetCurLtePrio()))
    {
        NAS_LMM_EMM_PLMN_LOG_ERR("NAS_EMM_lteRatIsExist: MMC START REQ RAT LIST LTE NOT EXIST ");
        return NAS_LMM_FAIL;
    }
    return NAS_LMM_SUCC;
}


VOS_UINT32  NAS_EMM_IsLteDisabled( VOS_VOID )
{
    if(NAS_LMM_LTE_DISABLED == NAS_EMM_GetLteEnableSta())
    {
        NAS_LMM_EMM_PLMN_LOG_INFO("NAS_EMM_IsLteDisabled:LTE DISABLTED.");
        return NAS_LMM_SUCC;
    }

    return NAS_LMM_FAIL;
}
/*****************************************************************************
 Function Name   : NAS_EMM_SendMmCombinedStartNotifyReq
 Description     : 向MM发送LMM_MM_COMBINED_START_NOTIFY_REQ消息
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-11-15  Draft Enact

*****************************************************************************/
VOS_VOID NAS_EMM_SendMmCombinedStartNotifyReq
(
    MM_LMM_COMBINED_TYPE_ENUM_UINT32    enCombinedType
)
{
    LMM_MM_COMBINED_START_NOTIFY_REQ_STRU   *pstMmComStartNotfiyReq = VOS_NULL_PTR;

    /* 申请DOPRA消息 */
    pstMmComStartNotfiyReq = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MM_COMBINED_START_NOTIFY_REQ_STRU));
    if (NAS_LMM_NULL_PTR == pstMmComStartNotfiyReq)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendMmCombinedStartNotifyReq: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstMmComStartNotfiyReq, 0, sizeof(LMM_MM_COMBINED_START_NOTIFY_REQ_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MM_MSG_HEADER((pstMmComStartNotfiyReq),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MM_COMBINED_START_NOTIFY_REQ_STRU));

    /* 填充消息ID */
    pstMmComStartNotfiyReq->ulMsgId    = ID_LMM_MM_COMBINED_START_NOTIFY_REQ;

    /* 填充消息体 */
    pstMmComStartNotfiyReq->ulOpId     = NAS_EMM_OPID_MM;

    /* 填充联合流程类型 */
    pstMmComStartNotfiyReq->enCombinedType = enCombinedType;

    /* 发送DOPRA消息 */
    NAS_LMM_SendLmmMmcMsg((VOS_VOID*)pstMmComStartNotfiyReq);

    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_SetTauTypeNoProcedure
 Description     : 未发起TAU流程时UE当前是否有能力发起联合TAU来填写TAU类型
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-11-15  Draft Enact

*****************************************************************************/
VOS_VOID NAS_EMM_SetTauTypeNoProcedure( VOS_VOID )
{
    /* LMM会根据当前注册域来填写TAU请求类型，如果当前注册域为CS+PS，则TAU请求类
       型填为MMC_LMM_COMBINED_TA_LA_UPDATING；如果当前注册域为PS，则TAU请求类型
       填为MMC_LMM_TA_UPDATING；*/
    if (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain())
    {
        NAS_EMM_TAU_SaveTAUtype(NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING);
    }
    else if (NAS_LMM_REG_DOMAIN_PS == NAS_LMM_GetEmmInfoRegDomain())
    {
        NAS_EMM_TAU_SaveTAUtype(NAS_EMM_CN_TAU_TYPE_TA_UPDATING);
    }
    else
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SetTauTypeNoProcedure: Reg Domain is NULL!");
    }
}
VOS_UINT32 NAS_LMM_SetEmmInfoUeRadioCapChg(
           NAS_LMM_UE_RADIO_CAP_CHANGE_ENUM_UINT32   enCurrentUeCapChg,
           NAS_LMM_UE_RADIO_CAP_CHANGE_ENUM_UINT32   enLastUeCapChg)
{
    NAS_EMM_PUBU_LOG_INFO("NAS_LMM_SetEmmInfoUeRadioCapChg is entered!");

    /*如果当前的更新类型大于等于上次的更新类型，则设置当前的UE能力改变，
      否则不再重新设置更新类型, 这里为什么要等于? 解释如下:
      等于时也认为需要更新，主要是考虑到NAS_LMM_ProcRrcSysCfgCnfNotSuspend
      函数在REG+REG_IMSI_DETACH_WATI_CN_DETACH_CNF状态和AUTH_INIT+AUTH_WAIT_CN_AUTH
      状态下的低优先级缓存分支，如果等于不认为需要更新，则当处理低优先级缓存的SYSCFG
      时会认为没有更新而直接退出
     */
    if(enCurrentUeCapChg >= enLastUeCapChg ||
       NAS_LMM_UE_RADIO_CAP_CHG_BUTT == enLastUeCapChg)
    {
        NAS_LMM_SetEmmInfoUeRadioCapChgFlag(enCurrentUeCapChg);
        return NAS_UE_CAP_CHG_HAVE_UPDATE;
    }
    else
    {
        return  NAS_UE_CAP_CHG_NO_UPDATE;
    }
}


VOS_UINT32  NAS_EMM_RecordUeRadioAccCapChg(
    VOS_UINT8                           ucRadioAccCapChgInd )
{
    NAS_UE_CAP_CHG_UPDATE_TYPE_ENUM_UINT32      enResult = NAS_UE_CAP_CHG_BUTT;

    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_RecordUeRadioAccCapChg: ucRadioAccCapChgInd value ="
                             , ucRadioAccCapChgInd);
    /*如果当前的主状态是EMM_MS_DEREG，直接返回无线变更类型没有更新*/
    if(EMM_MS_DEREG == NAS_EMM_CUR_MAIN_STAT)
    {
        return  NAS_UE_CAP_CHG_NO_UPDATE;
    }

    /* LTE能力发生变化*/
    if(NAS_EMM_UE_LTE_RADIO_CAP_NOT_CHG != (ucRadioAccCapChgInd & NAS_EMM_BIT_3))
    {

        /* 如果是挂起态下收到L无线能力变化，为了回到L模发起TAU而非ATTACH，改为记录成GU变化 */
        if (NAS_LMM_CUR_LTE_SUSPEND == NAS_EMM_GetCurLteState())
        {
            NAS_EMM_PUBU_LOG_NORM("NAS_EMM_RecordUeRadioAccCapChg: LTE current is suspended, record GU change.");
            enResult = NAS_LMM_SetEmmInfoUeRadioCapChg(NAS_LMM_UE_RADIO_CAP_GU_CHG,
                                                       NAS_LMM_GetEmmInfoUeRadioCapChgFlag());
        }
        else
        {
            enResult = NAS_LMM_SetEmmInfoUeRadioCapChg(NAS_LMM_UE_RADIO_CAP_LTE_CHG,
                                                       NAS_LMM_GetEmmInfoUeRadioCapChgFlag());
        }
    }

    /* GU能力发生变化*/
    else if(NAS_EMM_UE_GU_RADIO_CAP_NOT_CHG != (ucRadioAccCapChgInd & NAS_EMM_LOW_2_BITS_F))
    {
        enResult = NAS_LMM_SetEmmInfoUeRadioCapChg(NAS_LMM_UE_RADIO_CAP_GU_CHG,
                                                   NAS_LMM_GetEmmInfoUeRadioCapChgFlag());
    }

    /* 无变化*/
    else
    {
        enResult = NAS_LMM_SetEmmInfoUeRadioCapChg(NAS_LMM_UE_RADIO_CAP_NOT_CHG,
                                                   NAS_LMM_GetEmmInfoUeRadioCapChgFlag());
    }
    return enResult;
}
VOS_VOID  NAS_EMM_ClearUeRadioAccCapChgFlag(VOS_VOID)
{
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ClearUeRadioAccCapChgFlag: UeRadioCapChgFlag is cleared !");

    NAS_LMM_SetEmmInfoUeRadioCapChgFlag(NAS_LMM_UE_RADIO_CAP_NOT_CHG);

    return ;
}




VOS_VOID  NAS_EMM_SomeStateRcvMsgSysInfoLteRadioCapChgCommProc(VOS_VOID)
{
        VOS_UINT32                      ulRst = NAS_EMM_FAIL;

        /* 本地DETACH, 同时释放各模块资源:动态内存、赋初值 */
        NAS_EMM_LocalDetachProc();
        NAS_LMM_DeregReleaseResource();

        NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                              EMM_SS_DEREG_NORMAL_SERVICE,
                              TI_NAS_EMM_STATE_NO_TIMER);


        /* 向MMC发送本地LMM_MMC_DETACH_IND消息 */
        NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

        /* 重新发起ATTACH */
        ulRst = NAS_EMM_SendIntraAttachReq();
        if (NAS_EMM_SUCC != ulRst)
        {
            NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SomeStateRcvMsgSysInfoLteRadioCapChgCommProc:  send INTRA ATTACH REQ ERR !");
        }

    return;
}

/* lihong00150010 ims begin */
#if 0

VOS_VOID    NAS_EMM_SendMmInfoChangeNotify( VOS_VOID )
{
/* lihong00150010 csfb begin */

    LMM_MM_INFO_CHANGE_NOTIFY_STRU     *pstMmInfoChangeNotify   = VOS_NULL_PTR;
    NAS_LMM_PUB_INFO_STRU              *pstPubInfo              = VOS_NULL_PTR;
    NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstMmlEmerNumList       = VOS_NULL_PTR;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_SendMmInfoChangeNotify is entered!");;

    /* 申请MMC内部消息 */
    pstMmInfoChangeNotify = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MM_INFO_CHANGE_NOTIFY_STRU));
    if (NAS_LMM_NULL_PTR == pstMmInfoChangeNotify)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendMmInfoChangeNotify: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstMmInfoChangeNotify, 0, sizeof(LMM_MM_INFO_CHANGE_NOTIFY_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MM_MSG_HEADER((pstMmInfoChangeNotify),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MM_INFO_CHANGE_NOTIFY_STRU));

    /* 填充消息ID */
    pstMmInfoChangeNotify->ulMsgId                  = ID_LMM_MM_INFO_CHANGE_NOTIFY;

    /* 填充消息体 */
    pstMmInfoChangeNotify->ulOpId                   = NAS_EMM_OPID_MM;

    pstMmlEmerNumList   = NAS_EMM_GetEmergencyListFromMml();
    pstPubInfo          = NAS_LMM_GetEmmInfoAddr();

    /* 填充紧急呼号码列表 */
    pstMmInfoChangeNotify->bitOpEmcNumList          = NAS_EMM_BIT_SLCT;
    pstMmInfoChangeNotify->ucEmergencyNumAmount     = pstMmlEmerNumList->ucEmergencyNumber;
    NAS_LMM_MEM_CPY(    pstMmInfoChangeNotify->astEmergencyNumList,
                        pstMmlEmerNumList->aucEmergencyList,
                        sizeof(LMM_MM_EMERGENCY_NUM_STRU) * MM_LMM_EMERGENCY_NUM_LIST_MAX_RECORDS);

    /* 填充网侧是否支持IMS VOICE和紧急呼 */
    if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpNetFeature)
    {
        pstMmInfoChangeNotify->bitOpImsVoPS         = NAS_EMM_BIT_SLCT;
        pstMmInfoChangeNotify->enNwImsVoPS          = pstPubInfo->ucNetFeature & NAS_EMM_BIT_1;

        pstMmInfoChangeNotify->bitOpEmcBS           = NAS_EMM_BIT_SLCT;
        pstMmInfoChangeNotify->enNwEmcBS            = (pstPubInfo->ucNetFeature & NAS_EMM_BIT_2) >> NAS_EMM_MOVEMENT_1_BYTE;
    }
    else
    {
        pstMmInfoChangeNotify->bitOpImsVoPS         = NAS_EMM_BIT_SLCT;
        pstMmInfoChangeNotify->enNwImsVoPS          = LMM_MM_NW_IMS_VOICE_NOT_SUPPORTED;

        pstMmInfoChangeNotify->bitOpEmcBS           = NAS_EMM_BIT_SLCT;
        pstMmInfoChangeNotify->enNwEmcBS            = LMM_MM_NW_EMC_BS_NOT_SUPPORTED;
    }

    /* 发送MMC内部消息 */
    NAS_LMM_SendLmmMmcMsg((VOS_VOID*)pstMmInfoChangeNotify);

/* lihong00150010 csfb end */

    return;
}
#endif

#if 0
VOS_VOID    NAS_EMM_SendGmmInfoChangeNotify( VOS_VOID )
{
    LMM_GMM_INFO_CHANGE_NOTIFY_STRU    *pstGmmInfoChangeNotify  = VOS_NULL_PTR;
    NAS_LMM_PUB_INFO_STRU              *pstPubInfo              = VOS_NULL_PTR;
    NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstMmlEmerNumList       = VOS_NULL_PTR;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_SendMmInfoChangeNotify is entered!");;

    /* 申请MMC内部消息 */
    pstGmmInfoChangeNotify = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_GMM_INFO_CHANGE_NOTIFY_STRU));
    if (NAS_LMM_NULL_PTR == pstGmmInfoChangeNotify)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendMmInfoChangeNotify: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstGmmInfoChangeNotify, 0, sizeof(LMM_GMM_INFO_CHANGE_NOTIFY_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_GMM_MSG_HEADER((pstGmmInfoChangeNotify),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_GMM_INFO_CHANGE_NOTIFY_STRU));

    /* 填充消息ID */
    pstGmmInfoChangeNotify->ulMsgId                 = ID_LMM_GMM_INFO_CHANGE_NOTIFY;

    /* 填充消息体 */
    pstGmmInfoChangeNotify->ulOpId                  = NAS_EMM_OPID_MM;

    pstMmlEmerNumList   = NAS_EMM_GetEmergencyListFromMml();
    pstPubInfo          = NAS_LMM_GetEmmInfoAddr();

    /* 填充紧急呼号码列表 */
    pstGmmInfoChangeNotify->bitOpEmcNumList         = NAS_EMM_BIT_SLCT;
    pstGmmInfoChangeNotify->ucEmergencyNumAmount    = pstMmlEmerNumList->ucEmergencyNumber;
    NAS_LMM_MEM_CPY(    pstGmmInfoChangeNotify->astEmergencyNumList,
                        pstMmlEmerNumList->aucEmergencyList,
                        sizeof(LMM_GMM_EMERGENCY_NUM_STRU) * LMM_GMM_EMERGENCY_NUM_LIST_MAX_RECORDS);

    /* 填充网侧是否支持IMS VOICE和紧急呼 */
    if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpNetFeature)
    {
        pstGmmInfoChangeNotify->bitOpImsVoPS        = NAS_EMM_BIT_SLCT;
        pstGmmInfoChangeNotify->enNwImsVoPS         = pstPubInfo->ucNetFeature & NAS_EMM_BIT_1;

        pstGmmInfoChangeNotify->bitOpEmcBS          = NAS_EMM_BIT_SLCT;
        pstGmmInfoChangeNotify->enNwEmcBS           = (pstPubInfo->ucNetFeature & NAS_EMM_BIT_2) >> NAS_EMM_MOVEMENT_1_BYTE;
    }
    else
    {
        pstGmmInfoChangeNotify->bitOpImsVoPS        = NAS_EMM_BIT_SLCT;
        pstGmmInfoChangeNotify->enNwImsVoPS         = LMM_MM_NW_IMS_VOICE_NOT_SUPPORTED;

        pstGmmInfoChangeNotify->bitOpEmcBS          = NAS_EMM_BIT_SLCT;
        pstGmmInfoChangeNotify->enNwEmcBS           = LMM_MM_NW_EMC_BS_NOT_SUPPORTED;
    }

    pstGmmInfoChangeNotify->bitOpLteCsCap           = NAS_EMM_BIT_SLCT;
    if (NAS_LMM_ADDITIONAL_UPDATE_BUTT == NAS_EMM_GetAddUpdateRslt())
    {
        pstGmmInfoChangeNotify->enLteCsCap          = LMM_GMM_LTE_CS_CAPBILITY_NO_ADDITION_INFO;
    }
    else
    {
        pstGmmInfoChangeNotify->enLteCsCap          = NAS_EMM_GetAddUpdateRslt();
    }

    /* 发送MMC内部消息 */
    NAS_LMM_SendLmmMmcMsg((VOS_VOID*)pstGmmInfoChangeNotify);

    return;
}

#endif
/* lihong00150010 ims end */


VOS_VOID    NAS_EMM_SendMmcInfoChangeNotify( VOS_VOID )
{
    LMM_MMC_INFO_CHANGE_NOTIFY_STRU    *pstMmcInfoChangeNotify  = VOS_NULL_PTR;
    NAS_LMM_PUB_INFO_STRU              *pstPubInfo              = VOS_NULL_PTR;
    NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstMmlEmerNumList       = VOS_NULL_PTR;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_SendMmcInfoChangeNotify is entered!");;

    /* 申请MMC内部消息 */
    pstMmcInfoChangeNotify = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_INFO_CHANGE_NOTIFY_STRU));
    if (NAS_LMM_NULL_PTR == pstMmcInfoChangeNotify)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendMmcInfoChangeNotify: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstMmcInfoChangeNotify, 0, sizeof(LMM_MMC_INFO_CHANGE_NOTIFY_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER((pstMmcInfoChangeNotify),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MMC_INFO_CHANGE_NOTIFY_STRU));

    /* 填充消息ID */
    pstMmcInfoChangeNotify->ulMsgId                 = ID_LMM_MMC_INFO_CHANGE_NOTIFY;

    /* 填充消息体 */
    pstMmcInfoChangeNotify->ulOpId                  = NAS_EMM_OPID_MM;

    pstMmlEmerNumList   = NAS_EMM_GetEmergencyListFromMml();
    pstPubInfo          = NAS_LMM_GetEmmInfoAddr();

    /* 填充紧急呼号码列表 */
    /* 与GU同事确认LMM_MMC_INFO_CHANGE_NOTIFY_STRU中所有bit 位都去掉 */
    /*pstMmcInfoChangeNotify->bitOpEmcNumList         = NAS_EMM_BIT_SLCT;*/
    pstMmcInfoChangeNotify->ucEmergencyNumAmount    = pstMmlEmerNumList->ucEmergencyNumber;
    NAS_LMM_MEM_CPY(    pstMmcInfoChangeNotify->astEmergencyNumList,
                        pstMmlEmerNumList->aucEmergencyList,
                        sizeof(LMM_MMC_EMERGENCY_NUM_STRU) * LMM_MMC_EMERGENCY_NUM_LIST_MAX_RECORDS);

    /* 填充网侧是否支持IMS VOICE和紧急呼 */
    if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpNetFeature)
    {
        /*pstMmcInfoChangeNotify->bitOpImsVoPS        = NAS_EMM_BIT_SLCT;*/
        pstMmcInfoChangeNotify->enNwImsVoPS         = pstPubInfo->ucNetFeature & NAS_EMM_BIT_1;

        /*pstMmcInfoChangeNotify->bitOpEmcBS          = NAS_EMM_BIT_SLCT;*/
        pstMmcInfoChangeNotify->enNwEmcBS           = (pstPubInfo->ucNetFeature & NAS_EMM_BIT_2) >> NAS_EMM_MOVEMENT_1_BYTE;
    }
    else
    {
        /*pstMmcInfoChangeNotify->bitOpImsVoPS        = NAS_EMM_BIT_SLCT;*/
        pstMmcInfoChangeNotify->enNwImsVoPS         = LMM_MMC_NW_IMS_VOICE_NOT_SUPPORTED;

        /*pstMmcInfoChangeNotify->bitOpEmcBS          = NAS_EMM_BIT_SLCT;*/
        pstMmcInfoChangeNotify->enNwEmcBS           = LMM_MMC_NW_EMC_BS_NOT_SUPPORTED;
    }
    /* pstMmcInfoChangeNotify->bitOpLteCsCap           = NAS_EMM_BIT_SLCT; */
    if (NAS_LMM_ADDITIONAL_UPDATE_BUTT == NAS_EMM_GetAddUpdateRslt())
    {
        pstMmcInfoChangeNotify->enLteCsCap          = LMM_MMC_LTE_CS_CAPBILITY_NO_ADDITION_INFO;
    }
    else
    {
        pstMmcInfoChangeNotify->enLteCsCap          = NAS_EMM_GetAddUpdateRslt();
    }

    /* 发送MMC内部消息 */
    NAS_LMM_SendLmmMmcMsg((VOS_VOID*)pstMmcInfoChangeNotify);

    return;
}



VOS_VOID NAS_EMM_SaveLastAttemptRegTa(VOS_VOID)
{
    NAS_MM_NETWORK_ID_STRU             *pstPresentNetInfo = NAS_EMM_NULL_PTR;
    NAS_LMM_NETWORK_INFO_STRU          *pstNetInfo        = NAS_EMM_NULL_PTR;
    NAS_MM_TA_STRU                     *pstLastAttmpRegTa = NAS_EMM_NULL_PTR;

    pstNetInfo                         = NAS_LMM_GetEmmInfoNetInfoAddr();
    pstPresentNetInfo                  = NAS_LMM_GetEmmInfoNetInfoPresentNetAddr();
    pstLastAttmpRegTa                  = NAS_LMM_GetEmmInfoNetInfoLastAttmpRegTaAddr();

    pstNetInfo->bitOpLastAttmpRegTa    = NAS_EMM_BIT_SLCT;
    pstLastAttmpRegTa->stTac.ucTac     = pstPresentNetInfo->stTac.ucTac;
    pstLastAttmpRegTa->stTac.ucTacCnt  = pstPresentNetInfo->stTac.ucTacCnt;
    NAS_LMM_PlmnCpy(&pstLastAttmpRegTa->stPlmnId, &pstPresentNetInfo->stPlmnId);

}


VOS_UINT32 NAS_EMM_IsT3411orT3402Running
(
    NAS_LMM_PTL_TI_ENUM_UINT16  *pPtlTimerId
)
{
    VOS_UINT32    ulCheckRslt = NAS_EMM_NO;
    if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
    {
        *pPtlTimerId = TI_NAS_EMM_PTL_T3411;
        ulCheckRslt = NAS_EMM_YES;
    }
    else if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
    {
        *pPtlTimerId = TI_NAS_EMM_PTL_T3402;
        ulCheckRslt = NAS_EMM_YES;
    }
    else
    {
        *pPtlTimerId = NAS_LMM_PTL_TI_BUTT;
        ulCheckRslt = NAS_EMM_NO;
    }

    return ulCheckRslt;
}


VOS_UINT32  NAS_LMM_IsRrcConnectStatusIdle(VOS_VOID)
{
    /* IDLE态和释放过程中对于MMC和RRC来讲都认为是IDLE, 仅LMM内部进行区分 */
    if ((NAS_EMM_CONN_IDLE == NAS_EMM_GetConnState())
        || (NAS_EMM_CONN_RELEASING == NAS_EMM_GetConnState()))
    {
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;

}
VOS_VOID  NAS_EMM_DisableLteCommonProc( VOS_VOID )
{
   if(NAS_EMM_TAU_COMPLETE_VALID == NAS_EMM_TAU_GetEmmTauCompleteFlag())
   {
       NAS_LMM_PUBM_LOG_NORM("NAS_EMM_DisableLteCommonProc,TAU COMPLETE NEEDED DELAY");

       /* 延迟500ms，是为了使得TAU COMPLETE消息能够尽量发送出去 */
       (VOS_VOID)TLPS_TaskDelay(500);
   }
   NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
}
VOS_VOID  NAS_EMM_SendIntraEia0ActNotify( VOS_VOID )
{
    NAS_LMM_INTRA_EIA0_ACT_NOTIFY_STRU         *pstEia0ActNotify    = NAS_LMM_NULL_PTR;
    VOS_UINT32                                  ulLen               = NAS_EMM_NULL;

    /*确定消息长度*/
    ulLen = sizeof(NAS_LMM_INTRA_EIA0_ACT_NOTIFY_STRU);

    /*分配空间*/
    pstEia0ActNotify  = (VOS_VOID *)NAS_LMM_GetSendIntraMsgBuffAddr(ulLen);
    if(NAS_EMM_NULL_PTR == pstEia0ActNotify)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_SendIntraEia0ActNotify: INTRA MSG ALLOC ERR !!");
        return;
    }

    /* 打包VOS消息头 */
    NAS_EMM_INTRA_MSG_HEADER(pstEia0ActNotify,(ulLen-NAS_EMM_VOS_HEADER_LEN));

    /* 填充DOPRA消息ID    */
    pstEia0ActNotify->ulMsgId = ID_NAS_LMM_INTRA_EIA0_ACT_NOTIFY;

    /*发送内部消息入队列*/
    NAS_EMM_SEND_INTRA_MSG(pstEia0ActNotify);

    return;
}
VOS_UINT32 NAS_EMM_IsSmsOnlyCsfbNotPrefDisableNeeded( VOS_VOID )
{
    if (NAS_LMM_REG_DOMAIN_CS_PS != NAS_LMM_GetEmmInfoRegDomain())
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_IsCsPs1DisableNeeded:Reg domain is not cs+ps");
        return NAS_EMM_NO;
    }

    if (NAS_LMM_UE_CS_PS_MODE_1 != NAS_LMM_GetEmmInfoUeOperationMode())
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_IsCsPs1DisableNeeded:ue mode is not cs/ps1");
        return NAS_EMM_NO;
    }

    if (NAS_EMM_SUCC != NAS_EMM_CheckMutiModeSupport())
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_IsCsPs1DisableNeeded:lte only");
        return NAS_EMM_NO;
    }

    if ((NAS_LMM_ADDITIONAL_UPDATE_SMS_ONLY == NAS_EMM_GetAddUpdateRslt())
            || (NAS_LMM_ADDITIONAL_UPDATE_CSFB_NOT_PREFERED == NAS_EMM_GetAddUpdateRslt()))
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_IsCsPs1DisableNeeded:SMS ONLY or CSFB not preferred");
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}


VOS_VOID NAS_EMM_RecogAndProcSmsOnlyCsfbNotPrefDisable( VOS_VOID )
{
    if (NAS_EMM_YES == NAS_EMM_IsSmsOnlyCsfbNotPrefDisableNeeded())
    {

        NAS_EMM_DisableLteCommonProc();

    }
}
#if 0

VOS_UINT32 NAS_EMM_IsCause161722DisableNeeded
(
    NAS_EMM_CN_CAUSE_ENUM_UINT8         enEMMCause
)
{
    if (NAS_LMM_UE_CS_PS_MODE_1 != NAS_LMM_GetEmmInfoUeOperationMode())
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_IsCsPs1DisableNeeded:ue mode is not cs/ps1");
        return NAS_EMM_NO;
    }

    if (NAS_EMM_SUCC != NAS_EMM_CheckMutiModeSupport())
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_IsCsPs1DisableNeeded:lte only");
        return NAS_EMM_NO;
    }
    if (((NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS != enEMMCause)
            && (NAS_LMM_CAUSE_CS_NOT_AVAIL != enEMMCause))
        && (5 == NAS_EMM_TAU_GetEmmTAUAttemptCnt()))
    {
        return NAS_EMM_YES;
    }
    #if 0
    if (((NAS_LMM_CAUSE_MSC_UNREACHABLE == enEMMCause)
            || (NAS_LMM_CAUSE_NETWORK_FAILURE == enEMMCause)
            || (NAS_LMM_CAUSE_PROCEDURE_CONGESTION == enEMMCause))
        && (5 == NAS_EMM_TAU_GetEmmTAUAttemptCnt()))
    {
        return NAS_EMM_YES;
    }
    #endif
    return NAS_EMM_NO;
}


VOS_VOID NAS_EMM_RecogAndProc161722Disable
(
    NAS_EMM_CN_CAUSE_ENUM_UINT8         enEMMCause
)
{
    if (NAS_EMM_YES == NAS_EMM_IsCause161722DisableNeeded(enEMMCause))
    {

        NAS_EMM_DisableLteCommonProc();

    }
}


VOS_UINT32 NAS_EMM_IsCause18DisableNeeded
(
    NAS_EMM_CN_CAUSE_ENUM_UINT8         enEMMCause
)
{
    if (NAS_LMM_UE_CS_PS_MODE_1 != NAS_LMM_GetEmmInfoUeOperationMode())
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_IsCsPs1DisableNeeded:ue mode is not cs/ps1");
        return NAS_EMM_NO;
    }

    if (NAS_EMM_SUCC != NAS_EMM_CheckMutiModeSupport())
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_IsCsPs1DisableNeeded:lte only");
        return NAS_EMM_NO;
    }

    if (NAS_LMM_CAUSE_CS_NOT_AVAIL == enEMMCause)
    {
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}


VOS_VOID NAS_EMM_RecogAndProc18Disable
(
    NAS_EMM_CN_CAUSE_ENUM_UINT8         enEMMCause
)
{
    if (NAS_EMM_YES == NAS_EMM_IsCause18DisableNeeded(enEMMCause))
    {

        NAS_EMM_DisableLteCommonProc();

    }
}
#endif

/*****************************************************************************
 Function Name   : NAS_EMM_ClearEmcEsmMsg
 Description     : 清除紧急类型的ESM消息
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2012-10-31  Draft Enact
*****************************************************************************/
VOS_VOID  NAS_EMM_ClearEmcEsmMsg( VOS_VOID )
{
    VOS_UINT8                          *pucEsmMsg   = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulRslt      = NAS_EMM_NULL;

    pucEsmMsg = NAS_LMM_GetEmmInfoEsmBuffAddr();

    if (NAS_EMM_NULL_PTR == pucEsmMsg)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_ClearEmcEsmMsg, Memory is null");
        return ;
    }

    ulRslt = NAS_COMM_FreeBuffItem(NAS_COMM_BUFF_TYPE_EMM,(VOS_VOID *)pucEsmMsg);

    if (NAS_COMM_BUFF_SUCCESS != ulRslt)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_ClearEmcEsmMsg, Memory Free is not succ");
    }

    /* lihong00150010 emergency tau&service begin */
    NAS_LMM_GetEmmInfoEsmBuffAddr() = NAS_EMM_NULL_PTR;
    /* lihong00150010 emergency tau&service end */

    return ;
}

/* lihong00150010 emergency tau&service begin */
/*****************************************************************************
 Function Name   : NAS_EMM_SndEmcEsmMsg
 Description     : 发送紧急类型的ESM消息
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2012-12-13  Draft Enact
*****************************************************************************/
VOS_VOID  NAS_EMM_SndEmcEsmMsg( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg   = NAS_EMM_NULL_PTR;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_SndEmcEsmMsg is entered!");

    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU *)(VOS_VOID*)NAS_LMM_GetEmmInfoEsmBuffAddr();

    if (NAS_EMM_NULL_PTR == pstEsmMsg)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_SndEmcEsmMsg, Memory is null");
        return ;
    }

    NAS_EMM_SER_SendMrrcDataReq_ESMdata(&pstEsmMsg->stEsmMsg);

    /* 紧急PDN引起SERVICE，然后被TAU打断，TAU成功完成发送紧急PDN建链请求消息后，
       需要清除SERVICE缓存中存储的紧急PDN建链请求消息，否则在SERVICE冲突处理时，
       还会发送一次 */
    NAS_EMM_SER_DeleteEsmMsg(pstEsmMsg->ulOpid);

    NAS_EMM_ClearEmcEsmMsg();

    return ;
}
/* lihong00150010 emergency tau&service end */

/*****************************************************************************
 Function Name   : NAS_EMM_SaveEmcEsmMsg
 Description     : 保存紧急类型的ESM消息
 Input           : pstMsgStru
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2012-10-31  Draft Enact
*****************************************************************************/
VOS_VOID  NAS_EMM_SaveEmcEsmMsg
(
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg       = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulBufSize       = NAS_EMM_NULL;
    EMM_ESM_DATA_REQ_STRU              *pstEsmDataReq   = NAS_EMM_NULL_PTR;

    if (NAS_EMM_NULL_PTR != NAS_LMM_GetEmmInfoEsmBuffAddr())
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_SaveEmcEsmMsg, Memory is already used!");
        NAS_EMM_ClearEmcEsmMsg();
    }

    pstEsmDataReq = (EMM_ESM_DATA_REQ_STRU*)pMsgStru;

    /* 分配空间 */
    ulBufSize = (sizeof(NAS_EMM_ESM_MSG_BUFF_STRU) +
                                        pstEsmDataReq->stEsmMsg.ulEsmMsgSize) - 4;

    NAS_LMM_GetEmmInfoEsmBuffAddr() = NAS_COMM_AllocBuffItem(   NAS_COMM_BUFF_TYPE_EMM,
                                                                ulBufSize);

    if (NAS_LMM_NULL_PTR != NAS_LMM_GetEmmInfoEsmBuffAddr())
    {
        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)(VOS_VOID*)NAS_LMM_GetEmmInfoEsmBuffAddr();

        /* 存储ESM消息 */
        pstEsmMsg->stEsmMsg.ulEsmMsgSize = pstEsmDataReq->stEsmMsg.ulEsmMsgSize;

        NAS_LMM_MEM_CPY(        pstEsmMsg->stEsmMsg.aucEsmMsg,
                                pstEsmDataReq->stEsmMsg.aucEsmMsg,
                                pstEsmDataReq->stEsmMsg.ulEsmMsgSize);

        /* 存储OPID */
        pstEsmMsg->ulOpid       = pstEsmDataReq->ulOpId;

        /* 存储紧急标识 */
        pstEsmMsg->ulIsEmcType  = pstEsmDataReq->ulIsEmcType;
    }
    else
    {
        NAS_LMM_PUBM_LOG_WARN( "NAS_EMM_SaveEmcEsmMsg: NAS_AllocBuffItem return null pointer.");
    }

    return ;
}

/*****************************************************************************
 Function Name   : NAS_EMM_IsDeregState
 Description     : 判断是否是DEREG态
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2012-10-31  Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_EMM_IsDeregState( VOS_VOID )
{

    VOS_UINT32                          ulCurEmmStat            = NAS_EMM_NULL;

    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(   NAS_EMM_CUR_MAIN_STAT,
                                                NAS_EMM_CUR_SUB_STAT);

    /* 当前状态为RESUME+EMM_SS_RESUME_RRCORI_WAIT_SYS_INFO_IND时，如果是回退，
       则查看挂起前状态，否则查看恢复前状态 */
    if (ulCurEmmStat == NAS_LMM_PUB_COMP_EMMSTATE(  EMM_MS_RESUME,
                                                    EMM_SS_RESUME_RRCORI_WAIT_SYS_INFO_IND))
    {
        if ((NAS_LMM_SYS_CHNG_TYPE_REVERSE_RSL == NAS_EMM_GetResumeType())
            || (NAS_LMM_SYS_CHNG_TYPE_REVERSE_HO == NAS_EMM_GetResumeType())
            || (NAS_LMM_SYS_CHNG_TYPE_REVERSE_CCO == NAS_EMM_GetResumeType())
            || (NAS_LMM_SYS_CHNG_TYPE_REVERSE_REDIR == NAS_EMM_GetResumeType()))
        {
            /* 判断挂起前状态 */
            if (EMM_MS_DEREG == NAS_EMM_GetMsBefSuspend())
            {
                return NAS_EMM_YES;
            }

            return NAS_EMM_NO;
        }
        else
        {
            /* 判断恢复前状态 */
            if (EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
            {
                return NAS_EMM_YES;
            }

            return NAS_EMM_NO;
        }
    }

    if (ulCurEmmStat == NAS_LMM_PUB_COMP_EMMSTATE(  EMM_MS_RESUME,
                                                    EMM_SS_RESUME_MMCORI_WAIT_SYS_INFO_IND))
    {
        /* 判断恢复前状态 */
        if (EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
        {
            return NAS_EMM_YES;
        }

        return NAS_EMM_NO;
    }

    if (EMM_MS_DEREG == NAS_EMM_CUR_MAIN_STAT)
    {
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
VOS_VOID  NAS_EMM_SendUsimStatusToRrc( LRRC_LNAS_SIM_STATUS_ENUM_UINT32  enSimStatus)
{
    LRRC_LNAS_INFO_STRU                   stNasInfo;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_SendSecuParamToRrc entered.");

    /*清空内存*/
    NAS_LMM_MEM_SET(&stNasInfo, 0, sizeof(LRRC_LNAS_INFO_STRU));

    /*填充安全参数: Kasme + UL_NasCount*/
    stNasInfo.bitOpUsimStatus = NAS_EMM_BIT_SLCT;
    stNasInfo.enUsimStatus = enSimStatus;

    /*发送RRC_MM_INFO_CHANGE_REQ消息给RRC*/
    NAS_EMM_SendInfoChangeReqMsg(&stNasInfo);

    return;

}

#if 0
VOS_VOID  NAS_LMM_ChangeRegRejCauseAvoidInvalidSim
(
    VOS_UINT8        *pucCause
)
{
    NAS_MM_PLMN_LIST_STRU               stEHplmnList    = {0};
    VOS_UINT32                          ulRslt          = MMC_LMM_FAIL;
    MMC_LMM_PUB_INFO_STRU               stPubInfo;
    NAS_EMM_PLMN_ID_STRU               *pstCurentPlmn   = VOS_NULL_PTR;

    pstCurentPlmn = NAS_LMM_GetEmmInfoPresentPlmnAddr();

    if ((NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == *pucCause)
        || (NAS_LMM_CAUSE_ILLEGAL_UE == *pucCause)
        || (NAS_LMM_CAUSE_ILLEGAL_ME == *pucCause)
        || (NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW == *pucCause)
        || (NAS_LMM_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW == *pucCause))
    {
        ulRslt = Nas_GetPubInfo(MMC_LMM_EHPLMN, &stPubInfo);

        /* 如果存在EHPLMN，匹配当前驻留的PLMN是否在HPLMN/EHPLMN中 */
        if ((MMC_LMM_SUCC == ulRslt)&&( stPubInfo.u.stEHplmnList.ulPlmnNum > 0))
        {
            NAS_LMM_MEM_CPY(&stEHplmnList, &stPubInfo.u.stEHplmnList, sizeof(MMC_LMM_PLMN_LIST_STRU));

            if (NAS_LMM_MATCH_SUCCESS == NAS_LMM_PlmnIDMatchHplmnList(pstCurentPlmn, &stEHplmnList))
            {
                /* 当前PLMN在HPLMN/EHPLMN中时，使用定制原因值，默认值#17 */
                *pucCause = NAS_MML_GetPreferredRegRejCause_HPLMN_EHPLMN();
                return;

            }
        }

        /* 获取失败，或者当前PLMN不在HPLMN/EHPLMN中时，使用定制原因值，默认值#13 */
        *pucCause = NAS_MML_GetPreferredRegRejCause_NOT_HPLMN_EHPLMN();

    }

    /* 不是#2#3#6#7#8，不做任何修改返回 */
    return;

}
#endif

VOS_VOID  NAS_LMM_ChangeRegRejCauseAvoidInvalidSim
(
    VOS_UINT8        *pucCause
)
{
    VOS_UINT32                          ulRslt          = MMC_LMM_FAIL;
    VOS_UINT32                          i;
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    ulRslt = NAS_LMM_IsRegisteredInHplmn();

    for (i = 0; i < pstPubInfo->stConfCause.ucCauseNum; i++)
    {
        if (pstPubInfo->stConfCause.astAdaptCause[i].ucCnCause == *pucCause)
        {
            if (NAS_EMM_YES == ulRslt)
            {
                NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ChangeRegRejCauseAvoidInvalidSim:HPLMN entered.");
                /* 当前PLMN在HPLMN/EHPLMN中时，使用用户配置的HPLMN原因值 */
                *pucCause = pstPubInfo->stConfCause.astAdaptCause[i].ucHplmnCause;
            }
            else
            {
                NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ChangeRegRejCauseAvoidInvalidSim:NOT HPLMN entered.");
                /* 获取失败，或者当前PLMN不在HPLMN/EHPLMN中时，使用用户配置的非HPLMN原因值*/
                *pucCause = pstPubInfo->stConfCause.astAdaptCause[i].ucNotHplmnCause;
            }

            break;
        }

    }
    /* 不是用户需要修改的原因值，不做任何修改返回 */
    return;
}
VOS_VOID NAS_LMM_AdaptRegRejCau(VOS_UINT8 *pucCause)
{
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;


    pstPubInfo = NAS_LMM_GetEmmInfoAddr();
#if (VOS_OS_VER != VOS_WIN32)

    /* 是测试卡，不考虑特性开关直接走协议流程，不修改原因值 */
    if (PS_SUCC == LPS_OM_IsTestMode())
    {
        return;
    }
#endif

    /* 不是测试卡, 如果自适应网络拒绝原因值特性开关打开，修正原因值 */
    #if 0
    if (VOS_TRUE == NAS_MML_GetChangeRegRejCauFlg())
    {
        NAS_LMM_ChangeRegRejCauseAvoidInvalidSim(pucCause);
    }
    #endif
    if (NAS_LMM_NULL < pstPubInfo->stConfCause.ucCauseNum)
    {
        NAS_LMM_ChangeRegRejCauseAvoidInvalidSim(pucCause);
    }

    return;

}




/*PC REPLAY MODIFY BY LEILI BEGIN*/

MMC_LMM_RESULT_ID_ENUM_UINT32 NAS_EMM_GetGulPubInfo
(
    MMC_LMM_INFO_TYPE_ENUM_UINT32    ulInfoType,
    MMC_LMM_PUB_INFO_STRU           *pPubInfo
)
{
    MMC_LMM_RESULT_ID_ENUM_UINT32       ulRslt;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_GetGulPubInfo is entered");

    #ifndef __PS_WIN32_RECUR__
    ulRslt = Nas_GetPubInfo (ulInfoType, pPubInfo);

    NAS_LMM_LogPubInfo(ulInfoType, pPubInfo,ulRslt);
    #else
    /*调用API的桩函数*/
    ulRslt = Stub_Nas_GetPubInfo(ulInfoType, pPubInfo);
    #endif

    return ulRslt;
}
MMC_LMM_RESULT_ID_ENUM_UINT32 NAS_EMM_GetGuInfo
(
    NAS_GUMM_INFO_TYPE_ENUM_UINT32  ulInfoType,
    NAS_GUMM_INFO_STRU *            pstGuInfo
)
{
    MMC_LMM_RESULT_ID_ENUM_UINT32       ulRslt;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_GetGuInfo is entered");

    #ifndef __PS_WIN32_RECUR__
    ulRslt = Nas_GetGuInfo (ulInfoType, pstGuInfo);

    NAS_LMM_LogGuInfo(ulInfoType, pstGuInfo,ulRslt);
    #else
    /*调用API的桩函数*/
    ulRslt = Stub_Nas_GetGuInfo(ulInfoType, pstGuInfo);
    #endif

    return ulRslt;
}
VOS_VOID NAS_EMM_GetMsNetworkCapability
(
    NAS_MML_MS_NETWORK_CAPACILITY_STRU  *pstMsNetworkCapbility
)
{
    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_GetMsNetworkCapability is entered");

    #ifndef __PS_WIN32_RECUR__
    NAS_MML_GetMsNetworkCapability (pstMsNetworkCapbility);

    NAS_LMM_LogMsNetworkCapabilityInfo(pstMsNetworkCapbility);

    #else
    /*调用API的桩函数*/
    Stub_NAS_MML_GetMsNetworkCapability(pstMsNetworkCapbility);
    #endif

    return;
}
VOS_UINT8* NAS_EMM_GetUeIdTmsi(VOS_VOID)
{

    VOS_UINT8               *pucTmsi;
    #ifndef __PS_WIN32_RECUR__
    pucTmsi = NAS_MML_GetUeIdTmsi ();

    NAS_LMM_LogUeIdTmsiInfo (pucTmsi);
    #else
    pucTmsi = Stub_NAS_MML_GetUeIdTmsi();
    #endif
    return pucTmsi;

}


NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8  NAS_EMM_GetLteCsServiceCfg(  VOS_VOID  )
{
    NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8   ucRslt;

    #ifndef __PS_WIN32_RECUR__
    ucRslt = NAS_MML_GetLteCsServiceCfg ();

    NAS_LMM_LogCsServiceCfgInfo(ucRslt);
    #else

    ucRslt = Stub_NAS_MML_GetLteCsServiceCfg();
    #endif
    return ucRslt;
}


NAS_MML_PS_BEARER_CONTEXT_STRU* NAS_EMM_GetPsBearerCtx(VOS_VOID)
{
    NAS_MML_PS_BEARER_CONTEXT_STRU *pstPsBearerCtx;
    #ifndef __PS_WIN32_RECUR__
    pstPsBearerCtx   =  NAS_MML_GetPsBearerCtx();

    NAS_LMM_LogPsBearerCtxInfo(pstPsBearerCtx);

    #else
    pstPsBearerCtx = Stub_NAS_MML_GetPsBearerCtx();

    #endif

    return pstPsBearerCtx;
}


VOS_UINT8  NAS_EMM_GetUeEutranPsDrxLen(  VOS_VOID  )
{
    VOS_UINT8                 ucRslt;
    #ifndef __PS_WIN32_RECUR__
    ucRslt = NAS_MML_GetUeEutranPsDrxLen ();

    NAS_LMM_LogUeEutranPsDrxLenInfo (ucRslt);

    #else
    ucRslt = Stub_NAS_MML_GetUeEutranPsDrxLen();

    #endif

    return ucRslt;

}

VOS_UINT8  NAS_EMM_GetNonDrxTimer(  VOS_VOID  )
{
    VOS_UINT8                 ucRslt;
    #ifndef __PS_WIN32_RECUR__
    ucRslt = NAS_MML_GetNonDrxTimer ();

    NAS_LMM_LogNonDrxTimerInfo (ucRslt);
    #else
    ucRslt = Stub_NAS_MML_GetNonDrxTimer();
    #endif
    return ucRslt;
}


VOS_UINT8  NAS_EMM_GetSplitPgCycleCode(  VOS_VOID  )
{
    VOS_UINT8                 ucRslt;
    #ifndef __PS_WIN32_RECUR__
    ucRslt = NAS_MML_GetSplitPgCycleCode ();

    NAS_LMM_LogDrxSplitPgCycleCodeInfo (ucRslt);
    #else
    ucRslt = Stub_NAS_MML_GetSplitPgCycleCode();
    #endif
    return ucRslt;
}

VOS_UINT8  NAS_EMM_GetSplitOnCcch(  VOS_VOID  )
{
    VOS_UINT8                 ucRslt;

    #ifndef __PS_WIN32_RECUR__
    ucRslt = NAS_MML_GetSplitOnCcch ();

    NAS_LMM_LogDrxSplitOnCcch(ucRslt);

    #else

    ucRslt = Stub_NAS_MML_GetSplitOnCcch();
    #endif

    return ucRslt;
}


VOS_VOID NAS_EMM_FillIEClassMark2(VOS_UINT8  *pClassMark2)
{

    #ifndef __PS_WIN32_RECUR__
    NAS_MML_Fill_IE_ClassMark2(pClassMark2);

    NAS_LMM_LogClassMark2Info(pClassMark2);

    #else

    Stub_NAS_MML_Fill_IE_ClassMark2(pClassMark2);
    #endif
    return;
}

VOS_UINT8  NAS_EMM_GetCsAttachAllowFlg(  VOS_VOID  )
{
    VOS_UINT8                 ucRslt;
    #ifndef __PS_WIN32_RECUR__
    ucRslt = NAS_MML_GetCsAttachAllowFlg ();

    NAS_LMM_LogCsAttachAllowFlgInfo (ucRslt);
    #else
    ucRslt = Stub_NAS_MML_GetCsAttachAllowFlg();
    #endif
    return ucRslt;
}

VOS_UINT8  NAS_EMM_GetPsAttachAllowFlg(  VOS_VOID  )
{
    VOS_UINT8                 ucRslt;
    #ifndef __PS_WIN32_RECUR__
    ucRslt = NAS_MML_GetPsAttachAllowFlg ();

    NAS_LMM_LogPsAttachAllowFlgInfo (ucRslt);
    #else
    ucRslt = Stub_NAS_MML_GetPsAttachAllowFlg();
    #endif

    return ucRslt;
}


NAS_MML_MS_MODE_ENUM_UINT8  NAS_EMM_GetMsMode(  VOS_VOID  )
{
    NAS_MML_MS_MODE_ENUM_UINT8         ucMsMode;
    #ifndef __PS_WIN32_RECUR__
    ucMsMode = NAS_MML_GetMsMode ();

    NAS_LMM_LogMsMode (ucMsMode);
    #else

    ucMsMode = Stub_NAS_MML_GetMsMode();
    #endif
    return ucMsMode;
}


MMC_LMM_PLMN_CTRL_ENUM_UINT32  NAS_EMM_PlmnIdIsForbid(MMC_LMM_PLMN_ID_STRU *pstPlmn )
{
    MMC_LMM_PLMN_CTRL_ENUM_UINT32      ulRslt;
    #ifndef __PS_WIN32_RECUR__
    ulRslt = Nas_PlmnIdIsForbid (pstPlmn);

    NAS_LMM_LogPlmnIsForbid (pstPlmn,ulRslt);
    #else

    ulRslt = Stub_Nas_PlmnIdIsForbid(pstPlmn);
    #endif
    return ulRslt;
}


NAS_MML_REG_STATUS_ENUM_UINT8  NAS_EMM_GetPsRegStatus( VOS_VOID)
{
    NAS_MML_REG_STATUS_ENUM_UINT8      ucPsRegStatus;
    #ifndef __PS_WIN32_RECUR__
    ucPsRegStatus = NAS_MML_GetPsRegStatus ();

    NAS_LMM_LogPsRegStatus (ucPsRegStatus);
    #else
    ucPsRegStatus = Stub_NAS_MML_GetPsRegStatus();
    #endif
    return ucPsRegStatus;
}


NAS_MML_REG_STATUS_ENUM_UINT8  NAS_EMM_GetSimType( VOS_VOID)
{
    NAS_MML_SIM_TYPE_ENUM_UINT8      ucSimType;
    #ifndef __PS_WIN32_RECUR__
    ucSimType = NAS_MML_GetSimType ();

    NAS_LMM_LogSimType (ucSimType);
    #else
    ucSimType = Stub_NAS_MML_GetSimType();
    #endif
    return ucSimType;
}


VOS_UINT32  NAS_EMM_GetCardIMSI
(
    VOS_UINT8      *pucImsi
)
{
    VOS_UINT32      ulRslt;
    #ifndef __PS_WIN32_RECUR__
    ulRslt = USIMM_GetCardIMSI (pucImsi);

    NAS_LMM_LogCardIMSIInfo (pucImsi,ulRslt);
    #else
    ulRslt = Stub_USIMM_GetCardIMSI(pucImsi);
    #endif
    return ulRslt;
}


VOS_UINT32  NAS_EMM_IsServiceAvailable
(
    UICC_SERVICES_TYPE_ENUM_UINT32  enService
)
{
    VOS_UINT32      ulRslt;
    #ifndef __PS_WIN32_RECUR__
    ulRslt = USIMM_IsServiceAvailable (enService);
    /*lint -e718*/
    /*lint -e746*/
    NAS_LMM_LogUsimServiceInfo (enService,ulRslt);
    /*lint +e746*/
    /*lint +e718*/
    #else
    ulRslt = Stub_USIMM_IsServiceAvailable();
    #endif
    return ulRslt;
}
/*PC REPLAY MODIFY BY LEILI END*/

VOS_UINT32  NAS_EMM_IsAnnexP2ConditionSatisfied(VOS_VOID)
{
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;

    NAS_LMM_PUBM_LOG2_INFO("NAS_EMM_IsAnnexP2ConditionSatisfied:MM IMS VOICE =,lte nw ims voice=",
                            NAS_MML_GetImsVoiceMMEnableFlg(),
                            NAS_MML_GetLteNwImsVoiceSupportFlg());

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    /*1.IMS voice从not available变为available
      2.NV Mobility Management for IMS Voice Termination为enable
      3.IMS voice over PS session in S1 mode supported或
        IMS voice over PS session supported in Iu mode, but not supported in A/Gb mode
      4.voice domain preference for UTRAN不为CS voice only 或
        voice domain preference for E-UTRAN不为CS voice only*/
    if ((VOS_TRUE == NAS_MML_GetImsVoiceMMEnableFlg())
        &&((NAS_MML_NW_IMS_VOICE_SUPPORTED == NAS_MML_GetLteNwImsVoiceSupportFlg())
            ||(NAS_MML_NW_IMS_VOICE_SUPPORTED == NAS_MML_GetGUNwImsVoiceSupportFlg()))
        &&((NAS_EMM_BIT_SLCT == pstPubInfo->bitOpVoiceDomain)
            && (NAS_LMM_VOICE_DOMAIN_CS_ONLY != pstPubInfo->ulVoiceDomain)))
    {

        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}


VOS_UINT32  NAS_EMM_IsAnnexP4ConditionSatisfied(VOS_VOID)
{
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;

    NAS_LMM_PUBM_LOG2_INFO("NAS_EMM_IsAnnexP4ConditionSatisfied:MM IMS VOICE =,lte nw ims voice=",
                               NAS_MML_GetImsVoiceMMEnableFlg(),
                               NAS_MML_GetLteNwImsVoiceSupportFlg());

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

     /*1.IMS voice为available
       2.NV Mobility Management for IMS Voice Termination为enable
       3.IMS voice over PS session in S1 mode supported
       4.voice domain preference for E-UTRAN不为CS voice only*/
    if ((MMC_LMM_IMS_VOICE_CAP_AVAILABLE == pstPubInfo->enImsaVoiceCap)
        &&(VOS_TRUE == NAS_MML_GetImsVoiceMMEnableFlg())
        &&(NAS_MML_NW_IMS_VOICE_SUPPORTED == NAS_MML_GetLteNwImsVoiceSupportFlg())
        &&((NAS_EMM_BIT_SLCT == pstPubInfo->bitOpVoiceDomain)
            && (NAS_LMM_VOICE_DOMAIN_CS_ONLY != pstPubInfo->ulVoiceDomain)))
    {

        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}


/* lihong00150010 emergency tau&service begin */
/*****************************************************************************
 Function Name   : NAS_EMM_IsEnterRegLimitService
 Description     : 判断是否进入REG+LIMIT_SERVICE态
 Input           : pstMsgStru
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2012-10-31  Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_EMM_IsEnterRegLimitService( VOS_VOID )
{
    NAS_MM_NETWORK_ID_STRU             *pstPresentNwInfo = NAS_EMM_NULL_PTR;

    /* 如果无卡，则进入REG+LIMIT_SERVICE */
    if (NAS_LMM_SIM_STATUS_UNAVAILABLE == NAS_LMM_GetSimState())
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_EMM_IsEnterRegLimitService:NO CARD");
        return VOS_TRUE;
    }

    /* 如果卡无效，则进入REG+LIMIT_SERVICE */
    if (NAS_LMM_SIM_INVALID == NAS_LMM_GetPsSimValidity())
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_EMM_IsEnterRegLimitService:CARD INVALID");
        return VOS_TRUE;
    }

    /* 如果注册状态为紧急注册，则进入REG+LIMIT_SERVICE */
    if (NAS_LMM_REG_STATUS_EMC_REGED == NAS_LMM_GetEmmInfoRegStatus())
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_EMM_IsEnterRegLimitService:EMC REGED");
        return VOS_TRUE;
    }

    pstPresentNwInfo = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_PRESENT_NET_ID);

    /* 当前网络的小区是被禁小区，则进入REG+LIMIT_SERVICE */
    if (EMMC_EMM_NO_FORBIDDEN != pstPresentNwInfo->ulForbiddenInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_EMM_IsEnterRegLimitService:forbidden");
        return VOS_TRUE;
    }

    /* 当前网络的小区是ANYCELL小区，则进入REG+LIMIT_SERVICE */
    if (EMMC_EMM_CELL_STATUS_ANYCELL == pstPresentNwInfo->ulCellStatus)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_EMM_IsEnterRegLimitService:forbidden");
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_VOID NAS_EMM_TranStateRegNormalServiceOrRegLimitService(VOS_VOID)
{
    if (VOS_TRUE == NAS_EMM_IsEnterRegLimitService())
    {
        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {
        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }
}
/* lihong00150010 emergency tau&service end */
/*lint +e961*/
/*lint +e960*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

/* end of 子系统+模块+文件名.c */
