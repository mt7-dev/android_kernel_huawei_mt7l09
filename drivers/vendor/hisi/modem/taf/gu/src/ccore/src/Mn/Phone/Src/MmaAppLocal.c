



/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#define TAF_NEED_TAF_TAFM_LOCAL_INC 1

#include "om.h"
#include "TafClientApi.h"
#include "MnComm.h"
#include "Nasrrcinterface.h"
#include "UsimPsInterface.h"
#include "UsimPsApi.h"
#include "PsCommonDef.h"
#include "NVIM_Interface.h"
#include "rfa.h"
/*#include "LightLED.h"
 */
#include "DrvInterface.h"

#include "NasMmlLib.h"
#include "siappstk.h"
#include "pslog.h"
#include "MmaAppLocal.h"
#include "NasComm.h"
#include "NVIM_Interface.h"

#include "ScInterface.h"

#include "MnErrorCode.h"

#include "TafApsComFunc.h"
#include "GasNvInterface.h"

#if ( FEATURE_ON == FEATURE_LTE )
#include "LPsNvInterface.h"
#include "LNvCommon.h"
#endif


#ifdef NAS_STUB
#include "usimmbase.h"
#endif
#include "TafMmaCtx.h"
#include "TafMmaProcNvim.h"
#include "TafMmaMain.h"
#include "TafMmaPreProcAct.h"
#include "TafMmaMntn.h"


#if ( FEATURE_ON == FEATURE_LTE )
#include "gen_msg.h"
#include "msp_errno.h"
#include "osm.h"
#include "v_lib.h"
#include "msp_nvim.h"
#endif

#if (VOS_WIN32 == VOS_OS_VER)
#include "nvim_fileoperateinterface.h"
#endif

#include "TafApsProcNvim.h"

#include "TafSdcCtx.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"
#include "TafMmaNetworkNameTbl.h"

#include "NasUsimmApi.h"

#include "TafStdlib.h"

#include "Taf_Status.h"

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
#include "MtcMmaInterface.h"
#endif
#include "TafMmaSndTaf.h"
#include "TafMmaSndImsa.h"
#include "TafMmaSndInternalMsg.h"

#include "TafMmaSndApp.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_MMA_APPLOCAL_C





VOS_UINT8                 g_ucMmaOmConnectFlg;                           /* 是否已与PC工具连接, VOS_TRUE: 已连接; VOS_FALSE: 未连接.
                                                                                   默认未连接 */
VOS_UINT8                 g_ucMmaOmPcRecurEnableFlg;                     /* 当前PC工具是否使能了NAS的PC回放消息的发送, VOS_TRUE: 使能; VOS_FASLE: 未使能
                                                                                   默认使能PC回放 */


VOS_UINT32                g_ucUsimHotOutFlag;
VOS_UINT32                g_ucUsimHotInFlag;



/* USIM卡热插拔的可维可测计数 */
VOS_UINT32                       g_ulUsimHotOutCount = 0;
VOS_UINT32                       g_ulUsimHotInCount  = 0;



/*APS模块USIM相关初始化函数*/
extern TAF_UINT32   At_HexAlpha2AsciiString(TAF_UINT32 MaxLength,TAF_INT8 *headaddr,TAF_UINT8 *pucDst,TAF_UINT8 *pucSrc,TAF_UINT16 usSrcLen);
extern VOS_VOID     SHPA_ActivateHw(VOS_VOID);
extern VOS_VOID     SHPA_DeactivateHw(VOS_VOID);

typedef struct   TAF_PMU_STATE
{
    unsigned short  charger_PowerOn;         /*1表示充电开机、0表示按键开机*/
    unsigned short  chargerstate;            /* 1表示充电中， 0表示不在充电*/
    unsigned short  bal_Volt;                /*电池电压，共0，100，200……900，10档。0时关机，100时报警， 电池显示格数同这几档对应*/
    unsigned short  chg_timer_overflow;      /*  1表示充电超时，报电池坏。0表示正常*/
    unsigned short  bat_out;                 /* 1表示 电池拔出，0表示未拔出*/
}TAF_PMU_STATE_STRU;




/*****************************************************************************
   2 全局变量定义和宏
****************************************************************************/
#define MMA_REPORT_HEAD                 8 /*TAF_MSG_HEADER + QUERY_TYPE + INFO_LEN*/
#define MMA_PARA_LEN_POSITION           4
#define MMA_PH_PIN_TI                   2
#define MN_MMA_MAX_UPLMN_NUM            (100)
#define MMA_PLMN_ID_LEN                 (6)


MMA_STORED_CLIENT_OPID_ST               gstMmaStoredClientOPID;
TAF_MMA_GLOBAL_VALUE_ST                 gstMmaValue;
MMA_UE_BAND_CAPA_ST                     gstMmaBandCapa;
MMA_UE_FORB_BAND_ST                     gstMmaForbBand;
MMA_TIMER_ST                            g_MmaSimTimer[TAF_SIM_TIMER_NUM];
MMA_TI_TABLE                            gastMmaTiTab[MMA_MAX_TI];
MMA_INTERNAL_TIMER_ST                   gstMmaInternalTimer;


VOS_UINT8                               g_aucMmaImei[TAF_PH_IMEI_LEN-1]={0};
extern STATUS_CONTEXT_STRU              g_StatusContext;
/* 删除gstMmaInitialTimer */
VOS_BOOL                                f_bGetRecRange = VOS_FALSE;

TAF_MMA_LOCATION_STATUS_EVENT_INFO_STRU g_stMmsStkLocStaSysInfo;

MMA_TIMER_ST                            g_MmaTimerforPcReplay;

MMA_TIMER_ST                            g_stPhPhResetTimer;

MMA_TIMER_ST                            g_stPhPinOperTimer;

MMA_TIMER_ST                            g_stSyscfgWaitDetachCnfTimer;

MMA_TIMER_ST                            g_stPnnListMemProtectTimer;

MMA_TIMER_ST                            g_stPowerDownDelayTimer;

MMA_TIMER_ST                            g_stNetScanProtectTimer;
MMA_TIMER_ST                            g_stAbortNetScanProtectTimer;

/* 记录PIN当前操作结果  */
TAF_PH_PIN_CNF_STRU                     g_stPhPinCnf;
VOS_UINT16                              g_usMmaUeSupportCPHSFlg = VOS_FALSE;

MMA_DEACT_SIM_WHEN_POWEROFF_ENUM_UINT8  g_ucMmaDeactSimWhenPoweroff;

MMA_CIPHER_INFO_STRU                    g_stMmaCipherInfo = {0};


ENUM_SYSTEM_APP_CONFIG_TYPE_U16         g_usMmaAppConfigSupportType = SYSTEM_APP_BUTT;




MMA_GET_REC_INFO_STRU                   f_stRecInfo;

NAS_MMA_NEW_ADD_GLABAL_VARIABLE_FOR_PC_REPLAY_ST g_MmaNewValue;

#define MMA_PIN_HANDLE_TIME             25000
#define MMA_GET_FILE_TIME               21000
#define MMA_START_INITIAL_TIME          5000
#define MMA_INTERNAL_TIME               1000


#define MMA_PIN_LEN_MAX                 8
#define MMA_PIN_LEN_MIN                 4
#define MMA_TI_USED                     1


#define MMA_WRITE_NVIM_INDEX            0x1
#define MMA_READ_NVIM_INDEX             0x1
#define MMA_DE_ATTACH_NULL              0x0
#define MMA_WCDMA_BAND_TYPE_MAX         0x3
#define MMA_BAND_TYPE_NULL              0x0


#define MMA_RSCP_MAX                    (-60)
#define MMA_ECNO_MAX                    (0)

#define MMA_RSSI_INVALID                (99)

/* 两秒 */
#define MMA_INVALID_RSCP_SEC_MAX        (2)


 MMA_ME_PERSONALISATION_STATUS_STRU      g_stMmaMePersonalisationStatus;

MMA_MODIFY_PLMN_INFO_STRU               f_stMmaPlmnInfo;
TAF_PH_SPN_TYPE                         f_SpnCurrenReadType;
/* 记录上一次向WAS查询 RSCP 和 ECNO 的有效值 */
VOS_INT16                               g_sLastRscp = MMA_RSCP_MIN;
VOS_INT16                               g_sLastEcNo = MMA_ECNO_MIN;



#define PROTOCOL_STACK_VERSION "V300R001C01B050"

VOS_UINT8                               g_ucMnPhRoamStatus = TAF_PH_INFO_NONE_ROMAING;  /* 记录漫游状态(上报给应用) */



/* 版本号以环境变量传入的为准 */
#ifdef EV_PS_VERSION
    #undef PROTOCOL_STACK_VERSION
    #define PROTOCOL_STACK_VERSION EV_PS_VERSION
#endif




TAF_PH_BAND_NAME_STRU gastMmaSyscfgBandStr[] =
{
    {TAF_PH_BAND_GSM_850     ,      "GSM850"   },
    {TAF_PH_BAND_GSM_900     ,      "GSM900"   },
    {TAF_PH_BAND_GSM_DCS_1800,      "GSM1800"  },
    {TAF_PH_BAND_GSM_PCS_1900,      "GSM1900"  },
    {TAF_PH_BAND_WCDMA_VI_800,      "WCDMA800" },
    {TAF_PH_BAND_WCDMA_V_850,       "WCDMA850" },
    {TAF_PH_BAND_WCDMA_VIII_900,    "WCDMA900" },
    {TAF_PH_BAND_WCDMA_XI_1500,     "WCDMA1500"},
    {TAF_PH_BAND_WCDMA_IV_1700,     "AWS"},
    {TAF_PH_BAND_WCDMA_IX_1700,     "WCDMA1700"},
    {TAF_PH_BAND_WCDMA_III_1800,    "WCDMA1800"},
    {TAF_PH_BAND_WCDMA_II_PCS_1900, "WCDMA1900"},
    {TAF_PH_BAND_WCDMA_I_IMT_2100,  "WCDMA2100"},
    {TAF_PH_BAND_WCDMA_VII_2600,    "WCDMA2600"}

};

/*****************************************************************************
   3 函数实现
*****************************************************************************/
VOS_INT32 MMA_Bcd2Int (VOS_UINT8 ucChar)
{
    return ((ucChar>>4) + ((ucChar&0x0f)*10));
}


VOS_BOOL MN_PH_IsPlmnValid(TAF_PLMN_ID_STRU *pstPlmnId)
{
    VOS_UINT32                          i;

    for (i=0; i<3; i++)
    {
        if ((((pstPlmnId->Mcc >> (i*4)) & 0x0F) > 9)
         || ((((pstPlmnId->Mnc >> (i*4)) & 0x0F) > 9) && (i != 2 ))
         || ((((pstPlmnId->Mnc >> (i*4)) & 0x0F) > 9) && (((pstPlmnId->Mnc >> (i*4)) & 0x0F) != 0x0F)))
        {
            /* PLMN ID无效 */
            return VOS_FALSE;
        }
    }
    return VOS_TRUE;
}
VOS_VOID MN_MMA_SetMsClassTypeDeAttFlg()
{
    if ( MMA_SET_MSCLASSTYPE_YES == gstMmaValue.stSetMsClass.enSetMsClassTypeflag)
    {
        gstMmaValue.stSetMsClass.enSetMsClassTypeDeAttFlg       = MMA_SET_CLASSTYPE_DE_ATTACH_DISABLE;
    }
    return;
}


VOS_VOID MMA_FplmnHandle(VOS_UINT16        ClientId,
                                VOS_UINT8          OpId,
                                TAF_PH_FPLMN_OPERATE_STRU *pstFplmnOperate)
{
    TAF_PH_FPLMN_OPERATE_STRU stFplmnOperateforMmc;
    VOS_UINT8                 ucSimStatus;
    /* 获取卡状态 */
    MMA_PhoneGetUsimStatus(&ucSimStatus);
    /* 卡不存在则返回错误 */
    if (MMA_USIM_PRESENT != ucSimStatus)
    {
        MMA_WARNINGLOG("MMA_FplmnHandle():SIM/USIM Not Present!");
        Taf_ParaSetResult(ClientId, OpId, TAF_PARA_NO_USIM_ERROR, TAF_PH_FPLMN_PARA, VOS_NULL_PTR);
        return;
    }
    stFplmnOperateforMmc.ulCmdType = pstFplmnOperate->ulCmdType;
    stFplmnOperateforMmc.stPlmn.Mcc = pstFplmnOperate->stPlmn.Mcc;
    stFplmnOperateforMmc.stPlmn.Mnc = pstFplmnOperate->stPlmn.Mnc;
    /* PLMN格式转换 */
    MMA_PlmnId2NasStyle(&(stFplmnOperateforMmc.stPlmn));
    /* FPLMN公共处理 */
    Api_MmaFplmnHandleFromMmc(ClientId,OpId,stFplmnOperateforMmc);
}


VOS_VOID MMA_QryPrefPlmnInfo(
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType,
    TAF_USER_PLMN_LIST_STRU             *pstPlmnInfo
)
{
    PS_MEM_SET(pstPlmnInfo,0,sizeof(TAF_USER_PLMN_LIST_STRU));
    if (MN_PH_PREF_PLMN_UPLMN == enPrefPLMNType)
    {
        MMA_PhoneGetUPlmn(pstPlmnInfo);
    }
    else if (MN_PH_PREF_PLMN_OPLMN == enPrefPLMNType)
    {
        MMA_PhoneGetOPlmn(pstPlmnInfo);
    }
    else
    {
        MMA_PhoneGetHPlmn(pstPlmnInfo);
    }
}


VOS_VOID MMA_QryPrefPlmnName(
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType,
    VOS_UINT32                          ulFromIndex,
    VOS_UINT32                          ulPlmnNum,
    TAF_PLMN_NAME_LIST_STRU            *pstPlmnInfo
)
{
    VOS_UINT32                           i;
    TAF_USER_PLMN_LIST_STRU             *pstOrgPlmnInfo;
    TAF_PH_OPERATOR_NAME_FORMAT_OP_STRU  AtOperName;

    pstOrgPlmnInfo = (TAF_USER_PLMN_LIST_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_USER_PLMN_LIST_STRU));

    if (VOS_NULL_PTR == pstOrgPlmnInfo)
    {
        return;
    }

    pstPlmnInfo->ulPlmnNum = ulPlmnNum;
    PS_MEM_SET(pstOrgPlmnInfo, 0x00, sizeof(TAF_USER_PLMN_LIST_STRU));

    /* 获取优先网络列表 */
    MMA_QryPrefPlmnInfo(enPrefPLMNType,pstOrgPlmnInfo);

    for (i = 0; i < ulPlmnNum; i++ )
    {
        /* 由于核间通信最多只能传4K，所以不能获取所有的优先网络列表的长短名。*/
        PS_MEM_SET(&AtOperName,0,sizeof(AtOperName));
        AtOperName.OperName.PlmnId.Mcc              = pstOrgPlmnInfo->Plmn[(i + ulFromIndex)].Mcc;
        AtOperName.OperName.PlmnId.Mnc              = pstOrgPlmnInfo->Plmn[(i + ulFromIndex)].Mnc;
        AtOperName.ListOp                           = TAF_PH_OPER_NAME_PLMN2ALPHANUMERICNAME;
        pstPlmnInfo->ausPlmnRat[i]                  = pstOrgPlmnInfo->PlmnRat[(i + ulFromIndex)];
        pstPlmnInfo->astPlmnName[i].PlmnId.Mcc      = pstOrgPlmnInfo->Plmn[(i + ulFromIndex)].Mcc;
        pstPlmnInfo->astPlmnName[i].PlmnId.Mnc      = pstOrgPlmnInfo->Plmn[(i + ulFromIndex)].Mnc;

        /* 获取优先网络的长短名 */
        if (MMA_SUCCESS == Taf_PhoneGetNetworkNameForListPlmn(&AtOperName))
        {
            PS_MEM_CPY(pstPlmnInfo->astPlmnName[i].aucOperatorNameLong,AtOperName.OperName.aucOperatorNameLong,TAF_PH_OPER_NAME_LONG);
            PS_MEM_CPY(pstPlmnInfo->astPlmnName[i].aucOperatorNameShort,AtOperName.OperName.aucOperatorNameShort,TAF_PH_OPER_NAME_SHORT);
        }
        else
        {
            pstPlmnInfo->astPlmnName[i].aucOperatorNameLong[0]  = '\0';
            pstPlmnInfo->astPlmnName[i].aucOperatorNameShort[0] = '\0';
        }

    }

    PS_MEM_FREE(WUEPS_PID_MMA,pstOrgPlmnInfo);

}
VOS_UINT16 MN_PH_GetUpdateFileForPrefPlmn(
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType
)
{
    VOS_UINT16                  usUpdateFile;

    if (MN_PH_PREF_PLMN_UPLMN == enPrefPLMNType)
    {
        if (VOS_TRUE == MN_PH_IsUsingPlmnSel())
        {
            usUpdateFile = EFPLMNsel;
        }
        else
        {
            usUpdateFile = EFPLMNwAcT;
        }
    }
    else if (MN_PH_PREF_PLMN_OPLMN == enPrefPLMNType)
    {
        usUpdateFile = EFOPLMNwAcT;
    }
    else
    {
        usUpdateFile = EFHPLMNwAcT;
    }
    return usUpdateFile;
}


VOS_UINT32 MMA_DelPrefPlmnInfo(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType,
    VOS_UINT32                          ulIndex,
    VOS_BOOL                           *pbFinish
)
{
    TAF_USER_PLMN_LIST_STRU    *pstPlmnInfo;
    VOS_UINT16                  usIndex = 0;
    VOS_UINT16                  usUpdateFile;
    VOS_UINT32                  ulRet = TAF_PARA_OK;
    VOS_UINT16                  usMaxIndex;
    VOS_BOOL                    bNeedSetUsim;
    VOS_UINT8                   ucUsimStatus;

    bNeedSetUsim = VOS_TRUE;


    ucUsimStatus = MMA_USIM_ABSENT;

    MMA_GetUsimStatusFromUsim(&ucUsimStatus, VOS_NULL_PTR);

    if ( MMA_USIM_ABSENT == ucUsimStatus )
    {
        return TAF_PARA_NO_USIM_ERROR;
    }

    pstPlmnInfo = (TAF_USER_PLMN_LIST_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_USER_PLMN_LIST_STRU));

    if (VOS_NULL_PTR == pstPlmnInfo)
    {
        return TAF_PARA_ALLOC_MEM_FAIL;
    }

    PS_MEM_SET(pstPlmnInfo,0,sizeof(TAF_USER_PLMN_LIST_STRU));
    MMA_QryPrefPlmnInfo(enPrefPLMNType,pstPlmnInfo);

    usMaxIndex  = pstPlmnInfo->usPlmnNum;

    if (((VOS_UINT16)ulIndex > usMaxIndex)
     || ((VOS_UINT16)ulIndex == 0))
    {
        PS_MEM_FREE(WUEPS_PID_MMA,pstPlmnInfo);
        MMA_WARNINGLOG("MMA_DelPrefPlmnInfo():WARNING: Invalid Index!");
        return TAF_PARA_INVALID_INDEX;
    }

    usIndex = (VOS_UINT16)(ulIndex - 1);

    if (VOS_FALSE == MN_PH_IsPlmnValid(&(pstPlmnInfo->Plmn[usIndex])))
    {
        /* PLMN ID无效 */
        bNeedSetUsim = VOS_FALSE;
    }

    if (VOS_FALSE == bNeedSetUsim)
    {
        PS_MEM_FREE(WUEPS_PID_MMA,pstPlmnInfo);
        return TAF_PARA_OK;
    }
    /* 更新相应UPLMN项 */

    /*设置PLMN ID为无效值 */
    pstPlmnInfo->Plmn[usIndex].Mcc = 0xffffffff;
    pstPlmnInfo->Plmn[usIndex].Mnc = 0xffffffff;
    pstPlmnInfo->PlmnRat[usIndex]  = TAF_MMC_USIM_NO_RAT;

    f_stMmaPlmnInfo.stPlmnId.Mcc = pstPlmnInfo->Plmn[usIndex].Mcc ;
    f_stMmaPlmnInfo.stPlmnId.Mnc = pstPlmnInfo->Plmn[usIndex].Mnc;
    f_stMmaPlmnInfo.PlmnRat      = pstPlmnInfo->PlmnRat[usIndex];
    f_stMmaPlmnInfo.usIndex      = usIndex;

    usUpdateFile = MN_PH_GetUpdateFileForPrefPlmn(enPrefPLMNType);

    if (MMA_SUCCESS != MMA_UpdateUsimPrePlmn(ClientId, OpId, pstPlmnInfo, usUpdateFile))
    {
        ulRet = TAF_PARA_SIM_IS_BUSY;
    }

    if (TAF_PARA_OK == ulRet)
    {
        *pbFinish = VOS_FALSE;
    }

    PS_MEM_FREE(WUEPS_PID_MMA,pstPlmnInfo);

    return ulRet;
}
VOS_UINT32 MN_MMA_ModPrefPlmnInfo(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType,
    VOS_UINT32                          ulIndex,
    TAF_MMC_USIM_RAT                    usRat,
    TAF_PLMN_ID_STRU                   *pstPlmn
)
{
    TAF_USER_PLMN_LIST_STRU     *pstPlmnInfo;
    VOS_UINT32                  i;
    VOS_UINT16                  usUpdateFile;
    VOS_UINT32                  ulRet = TAF_PARA_OK;
    VOS_UINT16                  usMaxIndex;
    TAF_PLMN_ID_STRU            stPlmnIdFromAt;
    VOS_UINT8                   ucUsimStatus;


    PS_MEM_SET(&stPlmnIdFromAt, 0x00, sizeof(stPlmnIdFromAt));


    ucUsimStatus = MMA_USIM_ABSENT;

    MMA_GetUsimStatusFromUsim(&ucUsimStatus, VOS_NULL_PTR);

    if ( MMA_USIM_ABSENT == ucUsimStatus )
    {
        return TAF_PARA_NO_USIM_ERROR;
    }


    pstPlmnInfo = (TAF_USER_PLMN_LIST_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_USER_PLMN_LIST_STRU));

    if (VOS_NULL_PTR == pstPlmnInfo)
    {
        return TAF_PARA_ALLOC_MEM_FAIL;
    }

    /* 先将AT过来的PLMNID进行转化 */
    PS_MEM_CPY(&stPlmnIdFromAt,pstPlmn,sizeof(stPlmnIdFromAt));
    MMA_PlmnId2NasStyle(&stPlmnIdFromAt);


    MMA_PlmnId2Bcd(&stPlmnIdFromAt);

    if ( VOS_FALSE == MN_PH_IsPlmnValid(&stPlmnIdFromAt) )
    {
        PS_MEM_FREE(WUEPS_PID_MMA,pstPlmnInfo);
        return TAF_PARA_INVALID_PLMNID;
    }

    PS_MEM_SET(pstPlmnInfo,0,sizeof(TAF_USER_PLMN_LIST_STRU));
    MMA_QryPrefPlmnInfo(enPrefPLMNType,pstPlmnInfo);

    usMaxIndex = pstPlmnInfo->usPlmnNum;

    if ( ( ulIndex > usMaxIndex )
      || ( 0 == ulIndex ) )
    {
        PS_MEM_FREE(WUEPS_PID_MMA,pstPlmnInfo);
        return TAF_PARA_INVALID_INDEX;
    }

    /* 判断修改的PLMN ID是否和当前已有的PLMN ID重复,如果重复则不允许设置 */
    for ( i = 0 ; i < usMaxIndex ; i++ )
    {
        /* 当前待修改项不进行判断 */
        if ( i == ulIndex - 1 )
        {
            continue;
        }

        /* 与其他的UPLMN信息重复时直接返回error  */
        if ( (pstPlmnInfo->Plmn[i].Mcc == stPlmnIdFromAt.Mcc)
          && (pstPlmnInfo->Plmn[i].Mnc == stPlmnIdFromAt.Mnc)
          && ( (pstPlmnInfo->PlmnRat[i] == usRat)
            || (0 != (pstPlmnInfo->PlmnRat[i] & usRat)) ))
        {

            PS_MEM_FREE(WUEPS_PID_MMA,pstPlmnInfo);
            return TAF_PARA_DUPLICATE_PLMNINFO;
        }
    }

    /*设置PLMN ID值 */
    pstPlmnInfo->Plmn[ulIndex - 1].Mcc = stPlmnIdFromAt.Mcc;
    pstPlmnInfo->Plmn[ulIndex - 1].Mnc = stPlmnIdFromAt.Mnc;
    pstPlmnInfo->PlmnRat[ulIndex - 1]  = usRat;

    f_stMmaPlmnInfo.stPlmnId.Mcc   = pstPlmnInfo->Plmn[ulIndex - 1].Mcc ;
    f_stMmaPlmnInfo.stPlmnId.Mnc   = pstPlmnInfo->Plmn[ulIndex - 1].Mnc;
    f_stMmaPlmnInfo.usIndex        = (VOS_UINT16)(ulIndex - 1);
    f_stMmaPlmnInfo.PlmnRat        = pstPlmnInfo->PlmnRat[ulIndex - 1];
    usUpdateFile                   = MN_PH_GetUpdateFileForPrefPlmn(enPrefPLMNType);

    if (MMA_SUCCESS != MMA_UpdateUsimPrePlmn(ClientId, OpId, pstPlmnInfo, usUpdateFile))
    {
        ulRet = TAF_PARA_SIM_IS_BUSY;
    }

    PS_MEM_FREE(WUEPS_PID_MMA,pstPlmnInfo);
    return ulRet;
}
VOS_UINT32 MMA_AddPrefPlmnInfo(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType,
    TAF_MMC_USIM_RAT                    usRat,
    TAF_PLMN_ID_STRU                    *pstPlmn
)
{
    TAF_USER_PLMN_LIST_STRU             *pstPlmnInfo;
    VOS_UINT32                          i;
    VOS_UINT16                          usUpdateFile;
    VOS_UINT32                          ulRet = TAF_PARA_OK;
    VOS_UINT16                          usMaxIndex;
    VOS_BOOL                            bFindInvalidPlmn = VOS_FALSE;
    VOS_BOOL                            bFindSamePlmnId = VOS_FALSE;
    VOS_UINT8                           ucUsimStatus;
    VOS_UINT16                          usFindIndex = 0;
    TAF_PLMN_ID_STRU                    stPlmnIdFromAt;


    PS_MEM_SET(&stPlmnIdFromAt, 0x00, sizeof(stPlmnIdFromAt));


    ucUsimStatus = MMA_USIM_ABSENT;

    MMA_GetUsimStatusFromUsim(&ucUsimStatus, VOS_NULL_PTR);

    if ( MMA_USIM_ABSENT == ucUsimStatus )
    {
        return TAF_PARA_NO_USIM_ERROR;
    }

    pstPlmnInfo = (TAF_USER_PLMN_LIST_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_USER_PLMN_LIST_STRU));

    if (VOS_NULL_PTR == pstPlmnInfo)
    {
        return TAF_PARA_ALLOC_MEM_FAIL;
    }


    /* 先将AT过来的PLMNID进行转化 */
    PS_MEM_CPY(&stPlmnIdFromAt,pstPlmn,sizeof(stPlmnIdFromAt));
    MMA_PlmnId2NasStyle(&stPlmnIdFromAt);
    MMA_PlmnId2Bcd(&stPlmnIdFromAt);
    PS_MEM_SET(pstPlmnInfo, 0, sizeof(TAF_USER_PLMN_LIST_STRU));
    MMA_QryPrefPlmnInfo(enPrefPLMNType, pstPlmnInfo);


    usMaxIndex  = pstPlmnInfo->usPlmnNum;

    /* 搜寻空闲的PLMN ID */
    for (i = 0; i < usMaxIndex;i++)
    {
        if (VOS_FALSE == bFindInvalidPlmn)
        {
            if (VOS_FALSE == MN_PH_IsPlmnValid(&(pstPlmnInfo->Plmn[i])))
            {
                /* PLMN ID无效 */
                usFindIndex = (VOS_UINT16)i;
                bFindInvalidPlmn = VOS_TRUE;
            }
        }


        /* 增加相同PLMN的接入技术的判断 */
        if ( (pstPlmnInfo->Plmn[i].Mcc == stPlmnIdFromAt.Mcc)
          && (pstPlmnInfo->Plmn[i].Mnc == stPlmnIdFromAt.Mnc)
          && ( (pstPlmnInfo->PlmnRat[i] == usRat)
            || (0 != (pstPlmnInfo->PlmnRat[i] & usRat)) ))
        {
            bFindSamePlmnId = VOS_TRUE;
            break;
        }
    }

    if ( (VOS_FALSE == bFindInvalidPlmn)
      || (VOS_TRUE == bFindSamePlmnId))
    {
        PS_MEM_FREE(WUEPS_PID_MMA,pstPlmnInfo);
        return TAF_PARA_MEMORY_FULL;
    }


    /*设置PLMN ID为值 */
    pstPlmnInfo->Plmn[usFindIndex].Mcc = stPlmnIdFromAt.Mcc;
    pstPlmnInfo->Plmn[usFindIndex].Mnc = stPlmnIdFromAt.Mnc;
    pstPlmnInfo->PlmnRat[usFindIndex]  = usRat;

    f_stMmaPlmnInfo.stPlmnId.Mcc     = pstPlmnInfo->Plmn[usFindIndex].Mcc ;
    f_stMmaPlmnInfo.stPlmnId.Mnc     = pstPlmnInfo->Plmn[usFindIndex].Mnc;
    f_stMmaPlmnInfo.usIndex          = usFindIndex;
    f_stMmaPlmnInfo.PlmnRat          = pstPlmnInfo->PlmnRat[usFindIndex];
    usUpdateFile                     = MN_PH_GetUpdateFileForPrefPlmn(enPrefPLMNType);

    if (MMA_SUCCESS != MMA_UpdateUsimPrePlmn(ClientId, OpId, pstPlmnInfo, usUpdateFile))
    {
        ulRet = TAF_PARA_SIM_IS_BUSY;
    }

    PS_MEM_FREE(WUEPS_PID_MMA,pstPlmnInfo);
    return ulRet;
}
VOS_UINT32 MN_PH_GetPrefPlmnCnt(
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType
)
{
    TAF_USER_PLMN_LIST_STRU             *pstOrgPlmnInfo;
    VOS_UINT32                          ulPlmnCnt;

    ulPlmnCnt = 0;

    pstOrgPlmnInfo = (TAF_USER_PLMN_LIST_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_USER_PLMN_LIST_STRU));

    if (VOS_NULL_PTR == pstOrgPlmnInfo)
    {
        return ulPlmnCnt;
    }
    PS_MEM_SET(pstOrgPlmnInfo, 0x00, sizeof(TAF_USER_PLMN_LIST_STRU));
    MMA_QryPrefPlmnInfo(enPrefPLMNType,pstOrgPlmnInfo);

    ulPlmnCnt = pstOrgPlmnInfo->usPlmnNum;

    PS_MEM_FREE(WUEPS_PID_MMA,pstOrgPlmnInfo);

    return ulPlmnCnt;

}


VOS_UINT32 MMA_DbProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT16                          usMsgType;
    VOS_UINT16                          usClientId;
    VOS_UINT8                           ucOpId;
    MN_APP_REQ_MSG_STRU                *pstRcvMsg = VOS_NULL_PTR;

    pstRcvMsg = (MN_APP_REQ_MSG_STRU *)pstMsg;

    /*消息类型*/
    usMsgType = pstRcvMsg->usMsgName;

    /*ClientId*/
    usClientId = pstRcvMsg->clientId;
    ucOpId     = pstRcvMsg->opId;

    if (usClientId >= TAF_MAX_CLIENT_OF_ONE_PROC)
    {
        PS_LOG(WUEPS_PID_MMA, 0, PS_PRINT_ERROR,
            "MMA_TafMsgProc:ERROR:Client ID is out of range!");

        return VOS_TRUE;
    }

    switch((TAF_MSG_TYPE)usMsgType)
    {



        case TAF_MSG_MMA_SET_QUICKSTART:
            MMA_DefQuickStartStatus(usClientId, ucOpId, *((VOS_UINT32 *)pstRcvMsg->aucContent));
            break;
        case TAF_MSG_MMA_SET_AUTOATTACH:
            MMA_DefPhAutoAttachStatus(usClientId, ucOpId, *((VOS_UINT32 *)pstRcvMsg->aucContent));
            break;
        case TAF_MSG_MMA_SET_LOADDEFAULT:
            MMA_LoadDefaultVaule(ulEventType, pstMsg);
            break;
        case TAF_MSG_MMA_FPLMN_HANDLE:
            MMA_FplmnHandle(usClientId, ucOpId, (TAF_PH_FPLMN_OPERATE_STRU *)pstRcvMsg->aucContent);
            break;
        case TAF_MSG_MMA_HANDLE_PREF_PLMN_INFO:
            MMA_PrefPlmnHandle(usClientId, ucOpId, (TAF_PH_PREF_PLMN_OPERATE_STRU *)pstRcvMsg->aucContent);
            break;
        case TAF_MSG_MMA_USIM_INFO:
            MMA_QryUsimInfo(usClientId,ucOpId, (TAF_PH_QRY_USIM_INFO_STRU *)pstRcvMsg->aucContent);
            break;
        case TAF_MSG_MMA_CPNN_INFO:
            MMA_CPnnQuery(usClientId, ucOpId, (TAF_PH_ICC_TYPE*)pstRcvMsg->aucContent);
            break;
        case TAF_MSG_MMA_CRPN_HANDLE:
            MMA_CrpnQuery(usClientId, ucOpId, (MN_MMA_CRPN_QRY_REQ_STRU*)pstRcvMsg->aucContent);
            break;
        case TAF_MSG_MM_TEST_HANDLE:
            MMA_CmmTestCmdProc(usClientId, ucOpId, (MM_TEST_AT_CMD_STRU*)pstRcvMsg->aucContent);
            break;

        case TAF_MSG_MMA_CIPHER_INFO:
            MMA_QryCipherInfo(usClientId, ucOpId, (MN_MMA_CIPHER_QRY_REQ_STRU *)pstRcvMsg->aucContent);
            break;

        case TAF_MSG_MMA_LOCATION_INFO:
            MMA_QryLocInfo(usClientId, ucOpId, (MN_MMA_LOCATION_INFO_REQ_STRU *)pstRcvMsg->aucContent);
            break;

        case TAF_MSG_MMA_AC_INFO_QUERY_REQ:
            TAF_MMA_RcvTafMsgAcInfoQueryReq(usClientId, ucOpId, (TAF_MMA_AC_INFO_QUERY_REQ_STRU *)pstRcvMsg->aucContent);
            break;


        case TAF_MSG_MMA_CERSSI_INFO_QUERY_REQ:
            TAF_MMA_RcvTafMsgCerssiInfoQueryReq(usClientId, ucOpId, (TAF_MMA_CERSSI_INFO_QUERY_REQ_STRU *)pstRcvMsg->aucContent);
            break;
        case TAF_MSG_MMA_COPN_INFO_QUERY_REQ:
            TAF_MMA_RcvTafMsgCopnQueryReq(usClientId, ucOpId, (TAF_MMA_COPN_INFO_QUERY_REQ_STRU *)pstRcvMsg->aucContent);
            break;

        case TAF_MSG_MMA_SIMINSERT_IND:
           TAF_MMA_RcvTafMsgSimInsertInd(usClientId, ucOpId, (TAF_MMA_SIM_INSERT_IND_STRU *)pstRcvMsg->aucContent);
           break;
        case TAF_MSG_MMA_EOPLMN_SET_REQ:
           TAF_MMA_RcvMsgEOPlmnSetReq(usClientId, ucOpId, (TAF_MMA_SET_EOPLMN_LIST_STRU *)pstRcvMsg->aucContent);
           break;
        case TAF_MSG_MMA_EOPLMN_QUERY_REQ:
           TAF_MMA_RcvMsgEOPlmnQueryReq(usClientId, ucOpId);
           break;

        case TAF_MSG_MMA_NET_SCAN_REQ:
            TAF_MMA_RcvTafMsgNetScanReq(usClientId, ucOpId, (TAF_MMA_NET_SCAN_REQ_STRU *)pstRcvMsg->aucContent);
            break;

        case TAF_MSG_MMA_ABORT_NET_SCAN_REQ:
            TAF_MMA_RcvTafMsgAbortNetScanReq(usClientId, ucOpId);
            break;
        case TAF_MSG_MMA_CPOL_INFO_QUERY_REQ:
            TAF_MMA_RcvTafMsgCpolQueryReq(usClientId, ucOpId, (TAF_MMA_CPOL_INFO_QUERY_REQ_STRU *)pstRcvMsg->aucContent);
            break;

        default:
            Taf_ParaSetResult(usClientId,ucOpId, TAF_ERR_CMD_TYPE_ERROR, (TAF_PARA_TYPE)usMsgType, VOS_NULL_PTR);
            MMA_ERRORLOG1("MMA_DbProc:ERROR:ERROR DB PROC INDEX:", usMsgType);
    }

    return VOS_TRUE;
}




VOS_VOID MN_MMA_GetOnlyGURatOrder(
    TAF_PH_RAT_ORDER_STRU              *pstAllRatOrder,
    TAF_PH_RAT_ORDER_STRU              *pstGURatOrder
)
{
    VOS_UINT32                          i;
    VOS_UINT8                           ucIndex;

    ucIndex         = 0;

    /* 获取GU模信息 */
    for (i = 0; i < pstAllRatOrder->ucRatOrderNum; i++)
    {
        if ( (TAF_PH_RAT_WCDMA == pstAllRatOrder->aenRatOrder[i])
          || (TAF_PH_RAT_GSM   == pstAllRatOrder->aenRatOrder[i]) )
        {
            pstGURatOrder->aenRatOrder[ucIndex] = pstAllRatOrder->aenRatOrder[i];
            ucIndex++;
        }
    }

    pstGURatOrder->ucRatOrderNum          = ucIndex;
    pstGURatOrder->aenRatOrder[ucIndex]   = TAF_PH_RAT_BUTT;
    return;
}
VOS_VOID MN_MMA_GetCpamGURatPrio(
   MN_MMA_CPAM_RAT_PRIO_ENUM_UINT8     *pucPlmnPrio,
   TAF_PH_RAT_ORDER_STRU               *pstAllRatOrder
)
{
    TAF_PH_RAT_ORDER_STRU               stGURatOrder;

    /* 从接入信息中取得GU模的信息 */
    MN_MMA_GetOnlyGURatOrder(pstAllRatOrder, &stGURatOrder);

    /* 没有GU模信息时，默认为W优先 */
    if ( 0 == stGURatOrder.ucRatOrderNum )
    {
        *pucPlmnPrio = MN_MMA_CPAM_RAT_PRIO_WCDMA;
        return;
    }

    /* 将当前GU模信息转换为CPAM的接入优先级信息格式 */
    switch ( stGURatOrder.aenRatOrder[0] )
    {
        case TAF_PH_RAT_WCDMA:
            *pucPlmnPrio = MN_MMA_CPAM_RAT_PRIO_WCDMA;
            break;

        case TAF_PH_RAT_GSM :
            *pucPlmnPrio = MN_MMA_CPAM_RAT_PRIO_GSM;
            break;

        default:
            MN_ERR_LOG("MN_MMA_GetGUPlmnPrio,ERROR:RAT WRONG!");
            *pucPlmnPrio = MN_MMA_CPAM_RAT_PRIO_WCDMA;
            break;
    }

    return;
}
VOS_VOID MN_MMA_GetCpamGUAccessRat(
   MN_MMA_CPAM_RAT_TYPE_ENUM_UINT8     *penAccessMode,
   TAF_PH_RAT_ORDER_STRU               *pstAllRatOrder
)
{
    TAF_PH_RAT_ORDER_STRU               stGURatOrder;

    /* 从接入信息中取得GU模的信息 */
    MN_MMA_GetOnlyGURatOrder(pstAllRatOrder, &stGURatOrder);

    if ( 0 == stGURatOrder.ucRatOrderNum)
    {
        /* 没有GU模，默认接入技术为双模都支持 */
        *penAccessMode = MN_MMA_CPAM_RAT_TYPE_BUTT;
    }
    else if (VOS_TRUE == MN_MMA_IsGOnlyMode(&stGURatOrder))
    {
        *penAccessMode = MN_MMA_CPAM_RAT_TYPE_GSM;;
    }
    else if (VOS_TRUE == MN_MMA_IsWOnlyMode(&stGURatOrder))
    {
        *penAccessMode = MN_MMA_CPAM_RAT_TYPE_WCDMA;
    }
    else
    {
        *penAccessMode = MN_MMA_CPAM_RAT_TYPE_WCDMA_GSM;
    }

    return;
}


VOS_VOID MMA_GetAccessModePara(TAF_ACCESS_MODE_QUERY_PARA_STRU* pAccessMode)
{
    TAF_PH_RAT_ORDER_STRU               stRatOrder;
    NAS_MMA_NVIM_ACCESS_MODE_STRU       stAccessMode;
    VOS_UINT32                          ulLength;

    ulLength        = 0;
    PS_MEM_SET(&stRatOrder,   0x00, sizeof(stRatOrder));
    PS_MEM_SET(&stAccessMode, 0x00, sizeof(NAS_MMA_NVIM_ACCESS_MODE_STRU));

    TAF_MMA_ReadNvimRatPrioList(&stRatOrder);


    NV_GetLength(en_NV_Item_MMA_AccessMode, &ulLength);

    if (NV_OK != NV_Read(en_NV_Item_MMA_AccessMode ,
                         &stAccessMode,
                         ulLength))

    {
        stAccessMode.aucAccessMode[1] = TAF_PLMN_PRIO_AUTO;
    }



    /* CPAM查询的优先级与syscfg查询的优先级不一样需要进行转换 */
    if ( TAF_PLMN_GSM_PRIO == stAccessMode.aucAccessMode[1] )
    {
        pAccessMode->PlmnPrio = MN_MMA_CPAM_RAT_PRIO_GSM;
    }
    else
    {
        pAccessMode->PlmnPrio = MN_MMA_CPAM_RAT_PRIO_WCDMA;
    }

    /* 提取GU模接入优先级信息 */
    /* MN_MMA_GetCpamGURatPrio((&pAccessMode->PlmnPrio), &stRatOrder); */

    /* 获取GU模接入技术信息 */
    MN_MMA_GetCpamGUAccessRat(&(pAccessMode->AccessMode), &stRatOrder);


}

VOS_VOID MMA_DbQuery(VOS_UINT16             ClientId,
                     VOS_UINT8                    OpId,
                     TAF_PARA_TYPE       ParaType)
{
    VOS_VOID *pPara = TAF_NULL_PTR;
    TAF_ACCESS_MODE_QUERY_PARA_STRU AccessMode;
    TAF_PH_MS_CLASS_TYPE            MsClass;
    TAF_PLMN_ID_STRU                Plmn;

    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    switch (ParaType)
    {
        case TAF_PH_ACCESS_MODE_PARA:
            /*Access Mode*/
            MMA_GetAccessModePara(&AccessMode);
            pPara = (VOS_VOID *)&AccessMode;
            break;

        case TAF_PH_MS_CLASS_PARA:
            /*Service Mode*/
            MsClass = MMA_GetCurMsClass();
            pPara = (VOS_VOID *)&MsClass;
            break;

        case TAF_PH_REG_PLMN_PARA:
            if ( (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enCsServiceStatus)
              || (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enPsServiceStatus))
            {   /*0:means NORMAL_SERVICE*/
                /*4字节的最高字节为0*/
                Plmn = gstMmaValue.pg_StatusContext->StaPlmnId;
                MMA_PlmnId2Bcd(&Plmn);
            }
            else
            {   /*当前未处于注册成功状态*/
                Plmn.Mcc = MMA_PLMN_MCC_NONE;
                Plmn.Mnc = MMA_PLMN_MNC_NONE;
            }
            pPara = (VOS_VOID *)&Plmn;
            break;

        default:
            /*不可能的异常打印*/
            MMA_WARNINGLOG("MMA_DbQuery():WARNING:ERROR Query Type!");
            ParaType = TAF_TELE_PARA_BUTT;
            break;
    }


    /*调用APP查询参数上报函数*/
    MN_QRYPARA_SendMsg(ClientId, OpId, ParaType, TAF_ERR_NO_ERROR,pPara);

    return;
}
VOS_VOID MMA_GetCpolPara(
    TAF_PH_SET_PREFPLMN_STRU           *pstPrefPlmn,
    VOS_UINT32                         *pulIndex,
    TAF_PLMN_ID_STRU                   *pstPlmnId,
    AT_COPS_FORMAT_TYPE                *pTmpFormat,
    VOS_BOOL                           *pbOperExist,
    VOS_BOOL                           *pbFormat
)
{
    /* 设置<index> */
    if ( 0 != pstPrefPlmn->usIndexLen )
    {
        *pulIndex = pstPrefPlmn->usIndex;
    }

    /* 设置<format> */
    if ( 0 != pstPrefPlmn->usFormatLen )
    {
        *pTmpFormat = (AT_COPS_FORMAT_TYPE)pstPrefPlmn->usFormat;
    }
    else
    {
        *pTmpFormat = AT_COPS_LONG_ALPH_TYPE;
        *pbFormat   = TAF_FALSE;
    }

    PS_MEM_SET(pstPlmnId,(VOS_CHAR)0xFF,sizeof(TAF_PLMN_ID_STRU));
    /* 设置<oper> */
    if ( 0 != pstPrefPlmn->usOperNameLen )
    {
        /* 转化用户输入PLMN字符为PLMN ID */
        if( VOS_TRUE != MMA_FormatPlmnStr2PlmnId(pstPlmnId,
                                                 pstPrefPlmn->usFormat,
                                                 pstPrefPlmn->usOperNameLen,
                                                 pstPrefPlmn->ucOperName) )
        {
            *pbOperExist = TAF_FALSE;
        }
    }
    else
    {
        *pbOperExist = TAF_FALSE;
    }

    return;
}


VOS_UINT16 MMA_SetCpolOperType(
    TAF_PH_SET_PREFPLMN_STRU           *pstPrefPlmnParam,
    TAF_PH_PREF_PLMN_OPERATE_STRU      *pstPrefPlmnOpt)
{
    VOS_UINT32                          ulIndex;
    TAF_PLMN_ID_STRU                    stPlmnId;
    AT_COPS_FORMAT_TYPE                 TmpFormat;
    VOS_BOOL                            bOperExist;
    VOS_BOOL                            bFormat;

    ulIndex      = MMA_INVALID_INDEX;
    TmpFormat    = AT_COPS_LONG_ALPH_TYPE;
    bOperExist   = TAF_TRUE;
    bFormat      = TAF_TRUE;

    MMA_GetCpolPara(pstPrefPlmnParam,&ulIndex,&stPlmnId,&TmpFormat,&bOperExist,&bFormat);

    /* <index>存在,但<oper>不存在，则默认为删除此条记录 */
    if ((MMA_INVALID_INDEX != ulIndex) && (TAF_FALSE == bOperExist))
    {
        pstPrefPlmnOpt->enPrefPlmnOperType = MN_PH_PrefPLMN_DEL_ONE;
        pstPrefPlmnOpt->ulIndex            = ulIndex;
    }
    else if ((MMA_INVALID_INDEX == ulIndex)
          && (TAF_TRUE == bOperExist)
          && (TAF_TRUE == bFormat) )
    {
        /* <oper>存在,但<index>不存在，则默认为增加一条记录 */
        pstPrefPlmnOpt->enPrefPlmnOperType = MN_PH_PrefPLMN_ADD_ONE;
        PS_MEM_CPY(&pstPrefPlmnOpt->stPlmn,&stPlmnId,sizeof(stPlmnId));
    }
    else if ((MMA_INVALID_INDEX != ulIndex)
          && (TAF_TRUE == bOperExist)
          && (TAF_TRUE == bFormat) )
    {
        /* <oper>存在,<index>存在，则认为修改一条记录 */
        pstPrefPlmnOpt->enPrefPlmnOperType = MN_PH_PrefPLMN_MODIFY_ONE;
        pstPrefPlmnOpt->ulIndex            = ulIndex;
        PS_MEM_CPY(&pstPrefPlmnOpt->stPlmn,&stPlmnId,sizeof(stPlmnId));
    }
    else
    {
        return TAF_ERR_PARA_ERROR;
    }

    return VOS_TRUE;
}
VOS_UINT16 MMA_CheckAccessExist(
    TAF_PH_SET_PREFPLMN_STRU             *pstPrefPlmnParam,
    TAF_MMC_USIM_RAT                     *pusRat
)
{
    VOS_BOOL                            bPlmnSelFlg;

#if(FEATURE_ON == FEATURE_LTE)
    VOS_UINT32                          ulPlatformSuppLteFlg;
#endif
    VOS_UINT32                          ulAtParaRatValid;

    *pusRat     = TAF_MMC_USIM_NO_RAT;
    bPlmnSelFlg = VOS_FALSE;

    if (MN_PH_PREF_PLMN_UPLMN == pstPrefPlmnParam->enPrefPlmnType)
    {
        bPlmnSelFlg = MN_PH_IsUsingPlmnSel();
    }

    if ((3 == pstPrefPlmnParam->ucAtParaIndex)
     && (VOS_TRUE == bPlmnSelFlg))
    {
        /* 当前Uplmn为PlmnSel时，不需要输入接入技术,该标识在MMA中处理*/
        *pusRat = TAF_MMC_USIM_UTRN_GSM_RAT;
        return VOS_TRUE;
    }

#if(FEATURE_ON == FEATURE_LTE)
    ulPlatformSuppLteFlg = TAF_SDC_IsPlatformSupportLte();

    /* 当参数个数为7时，LTE宏打开始终认为有效 */
    if (7 == pstPrefPlmnParam->ucAtParaIndex)
    {
        ulAtParaRatValid = VOS_TRUE;
    }
    /* 当参数个数为6时，LTE宏打开平台不支持LTE时才有效 */
    else if ((6 == pstPrefPlmnParam->ucAtParaIndex)
          && (VOS_FALSE == ulPlatformSuppLteFlg))
    {
        ulAtParaRatValid = VOS_TRUE;
    }
    else
    {
        ulAtParaRatValid = VOS_FALSE;
    }
#else
    /* 当LTE宏关闭，参数个数为6才认为有效 */
    if (6 == pstPrefPlmnParam->ucAtParaIndex)
    {
        ulAtParaRatValid = VOS_TRUE;
    }
    else
    {
        ulAtParaRatValid = VOS_FALSE;
    }
#endif

    if (VOS_FALSE == ulAtParaRatValid)
    {
        return VOS_FALSE;
    }

    if (1 == pstPrefPlmnParam->usGsmRat)
    {
        *pusRat = TAF_MMC_USIM_GSM_RAT;
    }

    if (1 == pstPrefPlmnParam->usGsmCompactRat)
    {
        *pusRat |= TAF_MMC_USIM_GSM_COMPACT_RAT;
    }

    if (1 == pstPrefPlmnParam->usUtrn)
    {
        *pusRat |= TAF_MMC_USIM_UTRN_RAT;
    }

#if(FEATURE_ON == FEATURE_LTE)
    if ((VOS_TRUE == pstPrefPlmnParam->usEutrn)
     && (VOS_TRUE == ulPlatformSuppLteFlg))
    {
        *pusRat |= TAF_MMC_USIM_E_UTRN_RAT;
    }
#endif

    return VOS_TRUE;
}


VOS_UINT16 MMA_CheckCpolPara(
    TAF_PH_SET_PREFPLMN_STRU           *pstPrefPlmnParam,
    TAF_PH_PREF_PLMN_OPERATE_STRU      *pstPrefPlmnOpt)
{
    VOS_UINT16                          usRet;

    usRet = MMA_SetCpolOperType(pstPrefPlmnParam,pstPrefPlmnOpt);

    if (VOS_TRUE != usRet)
    {
        return usRet;
    }

    if ((MN_PH_PrefPLMN_ADD_ONE == pstPrefPlmnOpt->enPrefPlmnOperType)
     || (MN_PH_PrefPLMN_MODIFY_ONE == pstPrefPlmnOpt->enPrefPlmnOperType))
    {
        /* 增加或修改时,将用户设置接入技术修改为USIM的格式 */
        usRet = MMA_CheckAccessExist(pstPrefPlmnParam,&pstPrefPlmnOpt->usRat);
        if (VOS_TRUE != usRet)
        {
            return usRet;
        }
    }
    else
    {
        if ((0 != pstPrefPlmnParam->usGsmRatLen)
         || (0 != pstPrefPlmnParam->usGsmCompactRatLen)
#if(FEATURE_ON == FEATURE_LTE)
         || (0 != pstPrefPlmnParam->usEutrnLen)
#endif
         || (0 != pstPrefPlmnParam->usUtrnLen))
        {
            return TAF_ERR_PARA_ERROR;
        }
    }

    return VOS_TRUE;
}
VOS_UINT32 MMA_TestSyscfg(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PH_SYSCFG_BAND_STR             *pstSysCfgBandStr = VOS_NULL_PTR;
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent    = VOS_NULL_PTR;
    MN_APP_REQ_MSG_STRU                *pRcvMsg          = VOS_NULL_PTR;
    VOS_UINT16                          ClientId;
    VOS_UINT8                           OpId;

    pRcvMsg         = (MN_APP_REQ_MSG_STRU*)pstMsg;
    ClientId        = pRcvMsg->clientId;
    OpId            = pRcvMsg->opId;

    pstSysCfgBandStr = (TAF_PH_SYSCFG_BAND_STR*)(pRcvMsg->aucContent);
    pstPhoneEvent    = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        return VOS_TRUE;
    }

    Taf_InitEventInfoOP(pstPhoneEvent);

    /*标别该事件为TAF_MSG_MMA_TEST_PREF_PLMN_CNF消息的回复消息,*/
    pstPhoneEvent->ClientId           = ClientId;
    pstPhoneEvent->OpId               = OpId;
    pstPhoneEvent->OP_SysCfgBandGroup = 1;
    pstPhoneEvent->PhoneEvent         = TAF_PH_EVT_TEST_SYSCFG_CNF;
    pstPhoneEvent->PhoneError         = TAF_ERR_NO_ERROR;

    Taf_GetSyscfgBandGroupStr(pstSysCfgBandStr);

    PS_MEM_CPY((VOS_VOID *)pstPhoneEvent->strSysCfgBandGroup,
               pstSysCfgBandStr->strSysCfgBandGroup,
               sizeof(TAF_PH_SYSCFG_BAND_STR));

    MN_PH_SendMsgToAt((VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return VOS_TRUE;

}
VOS_UINT32 MMA_TestPrefPlmn(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType;
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent = VOS_NULL_PTR;
    MN_APP_REQ_MSG_STRU                *pRcvMsg;
    VOS_UINT16                          ClientId;
    VOS_UINT8                           OpId;

    pRcvMsg         = (MN_APP_REQ_MSG_STRU*)pstMsg;
    ClientId        = pRcvMsg->clientId;
    OpId            = pRcvMsg->opId;

    enPrefPLMNType = pRcvMsg->aucContent[0];

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR != pstPhoneEvent)
    {
        Taf_InitEventInfoOP(pstPhoneEvent);

        /*标别该事件为TAF_MSG_MMA_TEST_PREF_PLMN_CNF消息的回复消息,*/
        pstPhoneEvent->ClientId           = ClientId;
        pstPhoneEvent->OpId               = OpId;
        pstPhoneEvent->OP_PrefPlmnList    = 1;
        pstPhoneEvent->PhoneEvent         = TAF_PH_EVT_TEST_PREF_PLMN_CNF;
        pstPhoneEvent->PhoneError         = TAF_ERR_NO_ERROR;

        MMA_QryPrefPlmnInfo(enPrefPLMNType,&(pstPhoneEvent->stPrefPlmnList));

        MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

        PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
    }

    return VOS_TRUE;
}
VOS_VOID    Mma_VerifyPhonePersSuccEvent(SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 enVerifySimlockRsltOrig)
{
    TAF_PHONE_EVENT_INFO_STRU *         pstPhoneEvent;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("Mma_VerifyPhonePersSuccEvent:ERROR:ALLOC MEMORY FAIL.");
        return;
    }
    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    /*调用TAF事件上报函数,上报PIN事件*/
    pstPhoneEvent->OP_Pin             = MMA_TRUE;
    pstPhoneEvent->PhoneEvent         = TAF_PH_EVT_OP_PIN_CNF;

    pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_OK;

    switch (enVerifySimlockRsltOrig)
    {
        case SC_VERIFY_SIMLOCK_RESULT_PH_NET_PIN:
            pstPhoneEvent->PinCnf.PinType = TAF_PHNET_PIN;
            pstPhoneEvent->PinCnf.CmdType = TAF_PIN_VERIFY;
           break;
        case SC_VERIFY_SIMLOCK_RESULT_PH_NET_PUK:
            pstPhoneEvent->PinCnf.PinType = TAF_PHNET_PUK;
            pstPhoneEvent->PinCnf.CmdType = TAF_PIN_UNBLOCK;
            break;
        case SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PIN:
            pstPhoneEvent->PinCnf.PinType = TAF_PHNETSUB_PIN;
            pstPhoneEvent->PinCnf.CmdType = TAF_PIN_VERIFY;
            break;
        case SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PUK:
            pstPhoneEvent->PinCnf.PinType = TAF_PHNETSUB_PUK;
            pstPhoneEvent->PinCnf.CmdType = TAF_PIN_UNBLOCK;
            break;
        case SC_VERIFY_SIMLOCK_RESULT_PH_SP_PIN:
            pstPhoneEvent->PinCnf.PinType = TAF_PHSP_PIN;
            pstPhoneEvent->PinCnf.CmdType = TAF_PIN_VERIFY;
            break;
        case SC_VERIFY_SIMLOCK_RESULT_PH_SP_PUK:
            pstPhoneEvent->PinCnf.PinType = TAF_PHSP_PUK;
            pstPhoneEvent->PinCnf.CmdType = TAF_PIN_UNBLOCK;
            break;
        default:
            /* 异常取值 */
            pstPhoneEvent->PinCnf.PinType = TAF_SIM_NON;
            pstPhoneEvent->PinCnf.CmdType = TAF_PIN_VERIFY;
            break;
    }

    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

}



VOS_VOID MMA_SetCPinForApModemWhenLocked(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    MN_PH_SET_CPIN_REQ_STRU            *pstSetCPinReq
)
{
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 enVerifySimlockRslt;
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 enVerifySimlockRsltOrig;
    SC_ERROR_CODE_ENUM_UINT32           enScResult;
    VOS_UINT32                          ulLoop;
    VOS_UINT8                           ucTi;
    MODEM_ID_ENUM_UINT16                enModemId;


    enVerifySimlockRslt     = SC_VERIFY_SIMLOCK_RESULT_BUTT;
    enVerifySimlockRsltOrig = SC_VERIFY_SIMLOCK_RESULT_BUTT;

    /* 将ASCII字符的密码("0"~"9"的ASCII字符)转换为数字密码(0~9的数字) */
    for (ulLoop = 0; ulLoop < pstSetCPinReq->usPara0Len; ulLoop++)
    {
        if( ('0' <= *(pstSetCPinReq->aucPara0 + ulLoop) )
         && ('9' >= *(pstSetCPinReq->aucPara0 + ulLoop) ))
        {
            *(pstSetCPinReq->aucPara0 + ulLoop) -= '0';
        }
        else
        {
            MMA_HandleEventError(usClientId,
                                 ucOpId,
                                 TAF_ERR_PARA_ERROR,
                                 TAF_PH_EVT_OP_PIN_CNF );
            return;
        }
    }

    /* 先获取到解锁前的锁卡状态，用户后续返回解锁/UNBLOCK 成功事件时，填写PinType使用 */
    enModemId  = VOS_GetModemIDFromPid(WUEPS_PID_MMA);
    enScResult = SC_PERS_VerifySimLock(&enVerifySimlockRsltOrig, enModemId);

    if (SC_ERROR_CODE_NO_ERROR != enScResult)
    {
        MMA_HandleEventError(usClientId,
                             ucOpId,
                             TAF_ERR_UNSPECIFIED_ERROR,
                             TAF_PH_EVT_OP_PIN_CNF );
        return;
    }

    /* 调用SC接口进行解锁或UNBLOCK */
    enScResult = SC_PERS_UnLock(pstSetCPinReq->aucPara0, (VOS_UINT8)pstSetCPinReq->usPara0Len, &enVerifySimlockRslt, enModemId);


    if (SC_ERROR_CODE_NO_ERROR != enScResult)
    {
        if ( SC_ERROR_CODE_UNLOCK_KEY_INCORRECT == enScResult)
        {
            MMA_HandleEventError(usClientId,
                                 ucOpId,
                                 TAF_ERR_SIM_INCORRECT_PASSWORD,
                                 TAF_PH_EVT_OP_PIN_CNF );
        }
        else
        {
            MMA_HandleEventError(usClientId,
                                 ucOpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 TAF_PH_EVT_OP_PIN_CNF );
        }
        return;
    }

    /*填入对PIN操作的全局变量供返回函数查询，存储CLIENTID*/
    /*记录当前clientId与OpId*/
    if (MMA_SUCCESS != MMA_TiAlloc(TAF_MMA, usClientId, ucOpId, TAF_PH_EVT_OP_PIN_CNF, &ucTi))
    {   /*Ti分配失败*/
        MMA_WARNINGLOG("MMA_SetCPinForApModemWhenLocked():WARNING:ALLOC TI FAILED!");
        /*错误处理*/
        MMA_HandleEventError(usClientId, ucOpId, TAF_ERR_TI_ALLOC_FAIL, TAF_PH_EVT_OP_PIN_CNF);
        return;
    }

    /* 向AT发送解锁或UNBLOCK成功事件 */
    Mma_VerifyPhonePersSuccEvent(enVerifySimlockRsltOrig);

    /* 如果解锁后处于ready态，则构造卡状态变化上报，并将锁卡状态设置为已经解锁 */
    if (SC_VERIFY_SIMLOCK_RESULT_READY == enVerifySimlockRslt)
    {
        /* 解锁卡后触发锁网锁卡状态变化指示消息 */
        TAF_MMA_SndSimlocakStatusChangeInd();

        g_stMmaMePersonalisationStatus.SimLockStatus = MMA_SIM_LOCK_IS_VERIFY;
    }

    return;
}



VOS_UINT32 MMA_SetCPin(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PH_PIN_DATA_STRU                stPinData;
    TAF_ME_PERSONALISATION_DATA_STRU    stMePersonalisationData;
    MN_PH_SET_CPIN_REQ_STRU            *pstSetCPinReq;
    VOS_UINT32                          i;
    MN_APP_REQ_MSG_STRU                *pRcvMsg;
    VOS_UINT16                          ClientId;
    VOS_UINT8                           OpId;

    pRcvMsg         = (MN_APP_REQ_MSG_STRU*)pstMsg;
    ClientId        = pRcvMsg->clientId;
    OpId            = pRcvMsg->opId;



    PS_MEM_SET(&stPinData, (VOS_CHAR)0xFF, sizeof(stPinData));

    pstSetCPinReq = (MN_PH_SET_CPIN_REQ_STRU*)pRcvMsg->aucContent;

    if (TAF_PH_ME_SIM_LOCK == MMA_GetMeLockStatus())
    {
        /* 若上层应用为AP-Modem，则走AP-Modem的处理分支，否则走MBB的处理分支 */
        if (SYSTEM_APP_ANDROID == g_usMmaAppConfigSupportType)
        {
            MMA_SetCPinForApModemWhenLocked(ClientId, OpId, pstSetCPinReq);
        }
        else
        {
            /* 初始化 */
            PS_MEM_SET(&stMePersonalisationData,0x00,sizeof(stMePersonalisationData));

            /* 设置<pin> */
            if (0 != pstSetCPinReq->usPara0Len)
            {
                PS_MEM_CPY((VOS_VOID*)stMePersonalisationData.aucOldPwd,
                           (VOS_VOID*)pstSetCPinReq->aucPara0,
                           pstSetCPinReq->usPara0Len);
            }

            /* 设置<newpin> */
            if (0 != pstSetCPinReq->usPara1Len)
            {
                PS_MEM_CPY((VOS_VOID*)stMePersonalisationData.aucNewPwd,
                           (VOS_VOID*)pstSetCPinReq->aucPara1,
                           pstSetCPinReq->usPara1Len);
            }

            /* 设置CmdType */
            stMePersonalisationData.MePersonalType  = TAF_SIM_PERSONALISATION;
            stMePersonalisationData.CmdType         = TAF_ME_PERSONALISATION_VERIFY;

            /* 执行命令操作 */
            MMA_MePersonalisationHandle(ClientId, OpId,stMePersonalisationData);
        }
    }
    else
    {
        /* 初始化 */
        SI_PB_MemSet(sizeof(stPinData), 0xFF,(VOS_UINT8 *)&stPinData);

        /* 设置<pin> */
        if(0 != pstSetCPinReq->usPara0Len)
        {
            if (TAF_PH_PINCODELENMAX < pstSetCPinReq->usPara0Len)
            {
                MMA_HandleEventError(ClientId,
                                     OpId,
                                     TAF_ERR_PARA_ERROR,
                                     TAF_PH_EVT_OP_PIN_CNF );

                return VOS_TRUE;
            }

            for(i=0; i<pstSetCPinReq->usPara0Len; i++)
            {
                if((pstSetCPinReq->aucPara0[i] > '9' )||(pstSetCPinReq->aucPara0[i] < '0'))
                {
                    MMA_HandleEventError(ClientId,
                                         OpId,
                                         TAF_ERR_PARA_ERROR,
                                         TAF_PH_EVT_OP_PIN_CNF );

                    return VOS_TRUE;
                }
            }

            PS_MEM_CPY((VOS_VOID*)stPinData.aucOldPin,(VOS_VOID*)pstSetCPinReq->aucPara0,pstSetCPinReq->usPara0Len);
        }

        /* 设置<newpin> */
        if (0 != pstSetCPinReq->usPara1Len)
        {
            for (i=0; i<pstSetCPinReq->usPara1Len; i++)
            {
                if ((pstSetCPinReq->aucPara1[i] > '9' )||(pstSetCPinReq->aucPara1[i] < '0'))
                {
                    MMA_HandleEventError(ClientId,
                                         OpId,
                                         TAF_ERR_PARA_ERROR,
                                         TAF_PH_EVT_OP_PIN_CNF );

                    return VOS_TRUE;
                }
            }

            PS_MEM_CPY((VOS_VOID*)stPinData.aucNewPin,(VOS_VOID*)pstSetCPinReq->aucPara1,pstSetCPinReq->usPara1Len);
        }

        /* 设置CmdType */
        if (0 == pstSetCPinReq->usPara1Len)
        {
            stPinData.PinType = TAF_SIM_PIN;
            stPinData.CmdType = TAF_PIN_VERIFY;
        }
        else
        {
            stPinData.PinType = TAF_SIM_PUK;
            stPinData.CmdType = TAF_PIN_UNBLOCK;
        }

        /* 执行命令操作 */
        MMA_PinHandle(ClientId, OpId,stPinData);
    }

    return VOS_TRUE;

}
VOS_UINT32 MMA_SetPrefPlmn(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PH_SET_PREFPLMN_STRU             *pstPrefPlmnParam;
    TAF_PH_PREF_PLMN_OPERATE_STRU         stPrefPlmnOpt;
    TAF_PHONE_EVENT_INFO_STRU            *pstPhoneEvent;
    VOS_UINT16                            usRet;
    MN_APP_REQ_MSG_STRU                *pRcvMsg;
    VOS_UINT16                          ClientId;
    VOS_UINT8                           OpId;

    pRcvMsg         = (MN_APP_REQ_MSG_STRU*)pstMsg;
    ClientId        = pRcvMsg->clientId;
    OpId            = pRcvMsg->opId;

    pstPrefPlmnParam = (TAF_PH_SET_PREFPLMN_STRU*)pRcvMsg->aucContent;

    usRet = MMA_CheckCpolPara(pstPrefPlmnParam,&stPrefPlmnOpt);
    if ( VOS_TRUE != usRet )
    {
        pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

        if (VOS_NULL_PTR != pstPhoneEvent)
        {
            Taf_InitEventInfoOP(pstPhoneEvent);

            pstPhoneEvent->PhoneEvent         = TAF_PH_EVT_ERR;
            pstPhoneEvent->ClientId           = ClientId;
            pstPhoneEvent->OpId               = OpId;
            pstPhoneEvent->PhoneError         = usRet;

            MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

            PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
        }
        return VOS_TRUE;
    }

    stPrefPlmnOpt.enPrefPLMNType = pstPrefPlmnParam->enPrefPlmnType;

    MMA_PrefPlmnHandle(ClientId, OpId, &stPrefPlmnOpt);

    return VOS_TRUE;

}

VOS_UINT32 MMA_GetDomainAttachState(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PHONE_EVENT_INFO_STRU         *pstPhoneEvent;
    MN_APP_REQ_MSG_STRU                *pRcvMsg;
    VOS_UINT16                          ClientId;
    VOS_UINT8                           OpId;

    pRcvMsg         = (MN_APP_REQ_MSG_STRU*)pstMsg;
    ClientId        = pRcvMsg->clientId;
    OpId            = pRcvMsg->opId;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                                              sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        return VOS_TRUE;
    }

    /* 获取CS 和 PS的注册状态 */
    if (TAF_SUCCESS == Taf_GetCurrentAttachStatus( &(pstPhoneEvent->stDomainAttachState.ucCsState),
                                                   &(pstPhoneEvent->stDomainAttachState.ucPsState)))
    {
        pstPhoneEvent->PhoneEvent           = TAF_PH_EVT_GET_CURRENT_ATTACH_STATUS_CNF;
        pstPhoneEvent->OP_DomainAttachState = 1;
    }
    else
    {
        pstPhoneEvent->PhoneEvent           = TAF_PH_EVT_ERR;
        pstPhoneEvent->PhoneError           = TAF_ERR_AT_ERROR;
    }

    Taf_InitEventInfoOP(pstPhoneEvent);
    pstPhoneEvent->ClientId                         = ClientId;
    pstPhoneEvent->OpId                             = OpId;

    pstPhoneEvent->stDomainAttachState.ucDomain     = pRcvMsg->aucContent[0];

    MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return VOS_TRUE;
}
VOS_UINT32 MMA_SetPrefPlmnType(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PHONE_EVENT_INFO_STRU       *pstPhoneEvent;
    MN_PH_PREF_PLMN_TYPE_ENUM_U8     enPrefPlmnType;
    VOS_UINT32                       ulRet;
    MN_APP_REQ_MSG_STRU                *pRcvMsg;
    VOS_UINT16                          ClientId;
    VOS_UINT8                           OpId;

    pRcvMsg         = (MN_APP_REQ_MSG_STRU*)pstMsg;
    ClientId        = pRcvMsg->clientId;
    OpId            = pRcvMsg->opId;

    enPrefPlmnType = (MN_PH_PREF_PLMN_TYPE_ENUM_U8)pRcvMsg->aucContent[0];
    pstPhoneEvent  = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        return VOS_TRUE;
    }

    Taf_InitEventInfoOP(pstPhoneEvent);

    pstPhoneEvent->ClientId        = ClientId;
    pstPhoneEvent->OpId            = OpId;

    ulRet = TAF_SetPrefPlmnType(enPrefPlmnType);
    if (TAF_PARA_OK != ulRet)
    {
        pstPhoneEvent->PhoneEvent     = TAF_PH_EVT_ERR;
        pstPhoneEvent->PhoneError     = TAF_ERR_CME_OPT_NOT_SUPPORTED;
    }
    else
    {
        pstPhoneEvent->ucPrefPlmnType = enPrefPlmnType;
        pstPhoneEvent->PhoneEvent     = TAF_PH_EVT_SET_PREF_PLMN_TYPE_CNF;
        pstPhoneEvent->PhoneError     = TAF_ERR_NO_ERROR;
    }

    MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return VOS_TRUE;
}

VOS_UINT32 MMA_DigitString2Hex(TAF_UINT8 *nptr,TAF_UINT16 usLen,TAF_UINT32 *pRtn)
{
    TAF_UINT32 c     = 0;         /* current Char */
    TAF_UINT32 total = 0;         /* current total */
    TAF_UINT8 Length = 0;         /* current Length */

    c = (TAF_UINT32)*nptr++;

    while(Length++ < usLen)
    {
        if( (c  >= '0') && (c  <= '9') )
        {
            c  = c  - '0';
        }
        else
        {
            return VOS_FALSE;
        }

        if(total > 0x0FFFFFFF)              /* 发生反转 */
        {
            return VOS_FALSE;
        }
        else
        {
            total = (total << 4) + c;              /* accumulate digit */
            c = (TAF_UINT32)(TAF_UINT8)*nptr++;    /* get next Char */
        }
    }

    *pRtn = total;   /* return result, negated if necessary */
    return VOS_TRUE;
}
VOS_UINT32 MMA_FormatPlmnStr2PlmnId(
    TAF_PLMN_ID_STRU                   *pPlmn,
    VOS_UINT32                          usFormat,
    VOS_UINT16                          usOprNameLen,
    VOS_CHAR                           *pucOprName
)
{
    TAF_PH_OPERATOR_NAME_FORMAT_OP_STRU AtOperName;

    /* 根据ucType，获取PLMN */
    switch(usFormat)
    {
    case AT_COPS_LONG_ALPH_TYPE:    /* long format alphanumeric */
        VOS_StrCpy((VOS_CHAR*)AtOperName.OperName.aucOperatorNameLong,
                   (VOS_CHAR*)pucOprName);
        AtOperName.ListOp = TAF_PH_OPER_NAME_LONGNAME2OTERS;

        if(MMA_SUCCESS == Taf_PhoneGetOperNameInfo(&AtOperName))
        {
            pPlmn->Mnc = AtOperName.OperName.PlmnId.Mnc;

            pPlmn->Mcc = AtOperName.OperName.PlmnId.Mcc;
        }
        else
        {
            return VOS_FALSE;
        }
        break;

    case AT_COPS_SHORT_ALPH_TYPE:   /* short format alphanumeric */
        VOS_StrCpy((VOS_CHAR*)AtOperName.OperName.aucOperatorNameShort,
                   (VOS_CHAR*)pucOprName);
        AtOperName.ListOp = TAF_PH_OPER_NAME_SHORTNAME2OTHERS;

        if(MMA_SUCCESS == Taf_PhoneGetOperNameInfo(&AtOperName))
        {
            pPlmn->Mnc = AtOperName.OperName.PlmnId.Mnc;

            pPlmn->Mcc = AtOperName.OperName.PlmnId.Mcc;
        }
        else
        {
            return VOS_FALSE;
        }
        break;

    default:  /* numeric */
        if(VOS_FALSE == MMA_DigitString2Hex((TAF_UINT8*)pucOprName,3,&pPlmn->Mcc))
        {
            return VOS_FALSE;
        }
        /* Mnc */
        if(VOS_FALSE == MMA_DigitString2Hex((TAF_UINT8*)&pucOprName[3],usOprNameLen-3,&pPlmn->Mnc))
        {
            return VOS_FALSE;
        }

        pPlmn->Mcc |= 0xFFFFF000;
        pPlmn->Mnc |= (0xFFFFFFFF << ((usOprNameLen-3)*4));

        break;
    }
    return VOS_TRUE;
}
VOS_UINT32 MMA_SetPlmnUserSel(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PLMN_USER_SEL_STRU               *pstPlmnUserSel;
    TAF_PLMN_ID_STRU                      stPlmn;
    VOS_UINT32                            ulRet;
    MN_APP_REQ_MSG_STRU                *pRcvMsg;
    VOS_UINT16                          ClientId;
    VOS_UINT8                           OpId;

    pRcvMsg         = (MN_APP_REQ_MSG_STRU*)pstMsg;
    ClientId        = pRcvMsg->clientId;
    OpId            = pRcvMsg->opId;


    /* 在CL模式时候，不支持AT进行选网操作,直接返回ERROR */
    if (VOS_TRUE == TAF_MMA_IsPowerOnCLInterWork())
    {
        MMA_HandleEventError(ClientId, OpId, TAF_ERR_ERROR, TAF_PH_EVT_PLMN_SEL_CNF);

        return VOS_TRUE;
    }

    pstPlmnUserSel = (TAF_PLMN_USER_SEL_STRU*)pRcvMsg->aucContent;

    /* 转化用户输入PLMN字符为PLMN ID */
    ulRet = MMA_FormatPlmnStr2PlmnId(&stPlmn,
                                      pstPlmnUserSel->ucFormatType,
                                      pstPlmnUserSel->usOperNameLen,
                                      pstPlmnUserSel->ucOperName);
    if( VOS_TRUE == ulRet )
    {
        MMA_PlmnId2NasStyle(&stPlmn);
        /* 通过原有的TAF_PH_EVT_PLMN_SEL_CNF消息把指定搜结果发送给AT */
        MMA_PhonePlmnUserSel(ClientId, OpId, stPlmn, pstPlmnUserSel->ucAccessMode, TAF_PLMN_RESELETION_MANUAL);
    }
    else
    {
        /*错误处理*/
        MMA_HandleEventError(ClientId, OpId, TAF_ERR_PARA_ERROR, TAF_PH_EVT_PLMN_SEL_CNF);
    }

    return VOS_TRUE;
}


 VOS_UINT8 NAS_MMA_TranslateNtwkName2UCS2(
    VOS_UINT8                   *pucSrcNtwkName,
    VOS_CHAR                    *pucDstNtwkName,
    VOS_UINT8                    ucDstLen
)
 {
    VOS_UINT8                           ucCodScheme;
    VOS_UINT8                           uc7BitCharNum;
    VOS_UINT8                          *pucTmpNtwkName;
    VOS_UINT8                          *pTmpDstName;
    VOS_UINT8                           ucTmpDecodeLen;
    VOS_UINT8                           ucDecodeLen;
    VOS_UINT8                           i;

    VOS_UINT32                          ulRslt;

    ucDecodeLen = 0;
    /* pucSrcNtwkName中的编码参考24.008 MM INFORMATION
       第一个字节是长度,第二个字节是编码类型,后面的是编码后的网络名称 */
    if (*pucSrcNtwkName == 0)
    {
        return ucDecodeLen;
    }

    ucCodScheme     = ( 0x70 & pucSrcNtwkName[1] ) >> 4;

    uc7BitCharNum   = (VOS_UINT8)(((pucSrcNtwkName[0] - 1) * 8) / 7);
    pucTmpNtwkName    = &pucSrcNtwkName[2];

    ucTmpDecodeLen = uc7BitCharNum;
    if (0 != (((pucSrcNtwkName[0] -1) * 8) % 7))
    {
        ucTmpDecodeLen += 1;
    }

    pTmpDstName = PS_MEM_ALLOC(WUEPS_PID_MMC, ucTmpDecodeLen);
    if (VOS_NULL_PTR == pTmpDstName)
    {
        return ucDecodeLen;
    }

    /* 如果是7 BIT编码,转换成UCS-2 */
    if (0 == ucCodScheme)
    {
        ulRslt = TAF_STD_UnPack7Bit(pucTmpNtwkName, uc7BitCharNum, 0,(VOS_UINT8*)pTmpDstName);

        if (VOS_OK != ulRslt)
        {
            MMA_WARNINGLOG("TAF_MMA_TranslateNtwkName2Str: TAF_STD_UnPack7Bit Err.");
        }

        /* 解码后的数据转换成UCS-2 */
        for (i = 0; i < ucTmpDecodeLen; i++)
        {
            if ((i * 2) < ucDstLen)
            {
                *(pucDstNtwkName+i*2) = 0;
                *(pucDstNtwkName+(i*2 + 1)) = (VOS_CHAR)(pTmpDstName[i]);
            }
        }
        ucDecodeLen = (VOS_UINT8)((i - 1)*2);
    }
    else if (1 == ucCodScheme)
    {
        ucDecodeLen = *pucSrcNtwkName - 1;
        if (ucDecodeLen > ucDstLen)
        {
            PS_MEM_CPY(pucDstNtwkName, pucTmpNtwkName, ucDstLen);
            ucDecodeLen = ucDstLen;
        }
        else
        {
            PS_MEM_CPY(pucDstNtwkName, pucTmpNtwkName, ucDecodeLen);
        }
    }
    else
    {
    }

    PS_MEM_FREE(WUEPS_PID_MMC, pTmpDstName);

    return ucDecodeLen;
 }


VOS_VOID TAF_MMA_QryMmPlmnInfo(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_MM_INFO_PLMN_NAME_STRU      stMmPlmnInfo;
    TAF_PLMN_ID_STRU                   *pstPlmnId;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus(); 

    /*获取当前PLMN中所带的运营商信息*/
    PS_MEM_SET(&stMmPlmnInfo, 0, sizeof(TAF_MMA_MM_INFO_PLMN_NAME_STRU));

    if ( (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enCsServiceStatus)
      || (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enPsServiceStatus) )
    {
        pstPlmnId = (TAF_PLMN_ID_STRU *)TAF_SDC_GetCurrCampPlmnId(); 

        if ((pstPlmnId->Mcc == gstMmaValue.stOperatorNameInfo.stOperatorPlmnId.ulMcc)
        &&  (pstPlmnId->Mnc == gstMmaValue.stOperatorNameInfo.stOperatorPlmnId.ulMnc))
        {
            stMmPlmnInfo.ucLongNameLen  = NAS_MMA_TranslateNtwkName2UCS2(gstMmaValue.stOperatorNameInfo.aucOperatorNameLong,
                                                            (VOS_CHAR *)(stMmPlmnInfo.aucLongName),
                                                             NAS_MML_MAX_OPER_LONG_NAME_LEN);
            
            stMmPlmnInfo.ucShortNameLen = NAS_MMA_TranslateNtwkName2UCS2(gstMmaValue.stOperatorNameInfo.aucOperatorNameShort,
                                                            (VOS_CHAR *)(stMmPlmnInfo.aucShortName),
                                                            NAS_MML_MAX_OPER_SHORT_NAME_LEN);
        }
    }

    /*使用数据上报函数将数据传至AT*/
    MMA_ParaQueryReport(usClientId, ucOpId, TAF_MM_PLMN_INFO_QRY_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stMmPlmnInfo);


}


VOS_UINT32 MMA_SetMtPowerDown(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PHONE_EVENT_INFO_STRU  *pstPhoneEvent;
    MN_APP_REQ_MSG_STRU                *pRcvMsg;
    VOS_UINT16                          ClientId;
    VOS_UINT8                           OpId;

    pRcvMsg         = (MN_APP_REQ_MSG_STRU*)pstMsg;
    ClientId        = pRcvMsg->clientId;
    OpId            = pRcvMsg->opId;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        return VOS_TRUE;
    }

#if defined(INSTANCE_1)
    /* RF(G/W) OFF,delay n ms */
    DRV_RF_LDODOWN(RF_ID1);
#else
    /* RF(G/W) OFF,delay n ms */
    DRV_RF_LDODOWN(RF_ID0);
#endif


    /* USIM OFF,delay n ms */
    DRV_USIMMSCI_DEACT();
    /* BBP(G/W) OFF,不需要显示关闭 */

    Taf_InitEventInfoOP(pstPhoneEvent);

    /*标别该事件为MT 下电的回复消息,*/
    pstPhoneEvent->PhoneEvent         = TAF_PH_EVT_MT_POWER_DOWN_CNF;
    pstPhoneEvent->ClientId           = ClientId;
    pstPhoneEvent->OpId               = OpId;
    pstPhoneEvent->PhoneError         = TAF_ERR_NO_ERROR;

    MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return VOS_TRUE;
}
VOS_UINT32 MMA_PhoneProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent= VOS_NULL_PTR;
    VOS_UINT8                           ucError = TAF_ERR_NO_ERROR;
    VOS_UINT8                          *pucTmp  = VOS_NULL_PTR;
    VOS_UINT32                          ulTmpAddr;
    TAF_AVAIL_PLMN_LIST_TYPE            PlmnLstType;
    TAF_PLMN_RESEL_MODE_TYPE            ReSelMode;

    /*增加电话处理过程变量*/
    TAF_PH_PIN_DATA_STRU                stPinData;
    TAF_ME_PERSONALISATION_DATA_STRU    stMePersonalData;
    VOS_UINT16                          usMsgType;
    VOS_UINT16                          ClientId;
    VOS_UINT8                           OpId;
    MN_APP_REQ_MSG_STRU                *pstRcvMsg= VOS_NULL_PTR;

    pstRcvMsg = (MN_APP_REQ_MSG_STRU *)pstMsg;

    usMsgType   = pstRcvMsg->usMsgName;
    ClientId    = pstRcvMsg->clientId;
    OpId        = pstRcvMsg->opId;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MMA_PhoneProc():ERROR:ALLOC MEMORY FAIL!");

        return VOS_TRUE;
    }

    pucTmp = &(pstRcvMsg->aucContent[0]);

    switch(usMsgType)
    {
        case TAF_MSG_MMA_PLMN_LIST:
            PlmnLstType = *pucTmp++;
            if (TAF_PLMN_LIST == PlmnLstType)
            {
                MMA_PhonePlmnList(ClientId, OpId);
            }
            else
            {
                ucError = TAF_ERR_PARA_ERROR;
                MMA_HandleEventError(ClientId,OpId,ucError,TAF_PH_EVT_ERR);
                PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

                return VOS_TRUE;
            }
            break;

        case TAF_MSG_MMA_PLMN_LIST_ABORT_REQ:

            /* 调用处理函数 */
            TAF_MMA_RcvUserAbortPlmnList(OpId);
            break;

        case TAF_MSG_MMA_PLMN_RESEL:
            ReSelMode = *pucTmp++;
            MMA_PhonePlmnResel(ClientId, OpId, ReSelMode);
            break;
        case TAF_MSG_MMA_OP_PIN_REQ:
            ulTmpAddr = (VOS_UINT32)pucTmp;
            MMA_MEM_CPY((VOS_UINT8 *)&stPinData, (VOS_UINT8 *)ulTmpAddr, sizeof(TAF_PH_PIN_DATA_STRU));
            MMA_PinHandle(ClientId, OpId,stPinData);
            break;
        case TAF_MSG_MMA_ME_PERSONAL_REQ:
            ulTmpAddr = (VOS_UINT32)pucTmp;
            MMA_MEM_CPY((VOS_UINT8 *)&stMePersonalData, (VOS_UINT8 *)ulTmpAddr, sizeof(TAF_ME_PERSONALISATION_DATA_STRU));
            MMA_MePersonalisationHandle(ClientId, OpId,stMePersonalData);
            break;
        case TAF_MSG_MMA_PARA_QUERY:
            MMA_QueryProc(ClientId, OpId, pucTmp);
            break;
        case TAF_MSG_MMA_ATTACH :
            MMA_PhoneAttach(ClientId, OpId, (TAF_PH_ATTACH_TYPE)(*pucTmp));
            break;


        case TAF_MSG_MMA_INFO_REPORT:
            MMA_InitAtMsgReq(ClientId, OpId, *((TAF_START_INFO_IND_STRU *)pucTmp));
            break;
        default:

            ucError = TAF_ERR_CMD_TYPE_ERROR;
            break;
    }
    if (TAF_ERR_NO_ERROR != ucError)
    {
        Taf_InitEventInfoOP(pstPhoneEvent);
        pstPhoneEvent->OP_PhoneError      = 1;
        pstPhoneEvent->PhoneEvent         = TAF_PH_EVT_ERR;
        pstPhoneEvent->ClientId           = ClientId;
        pstPhoneEvent->OpId               = OpId;
        pstPhoneEvent->PhoneError         = ucError;

        /* 调用电话管理上报函数 */
        MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    }

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return VOS_TRUE;
}


VOS_VOID MN_MMA_SyscfgWaitDetachCnfTimerExpired(VOS_VOID)
{
    MMC_MMA_DETACH_CNF_STRU             stDetachCnf;
    MMA_SYS_CFG_SET_STRU               *pstSysCfgSet = VOS_NULL_PTR;

    pstSysCfgSet = &gstMmaValue.stSysCfgSet;

    /* 未收到detach cnf消息，认为detach成功，构造假的detach cnf消息，调用Sta_DetachCnf 进行统一处理*/
    stDetachCnf.ulOpID       = pstSysCfgSet->ucOpId;


    if (MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH == (pstSysCfgSet->usSetFlag & MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH))
    {
        stDetachCnf.enCnDomainId    = MMA_MMC_SRVDOMAIN_CS;
        stDetachCnf.enServiceStatus = MMA_MMC_SERVICE_STATUS_NO_SERVICE;

        Sta_DetachCnf (1, (struct MsgCB *)&stDetachCnf);
    }
    if (MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH == (pstSysCfgSet->usSetFlag & MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH))
    {
        stDetachCnf.enCnDomainId    = MMA_MMC_SRVDOMAIN_PS;
        stDetachCnf.enServiceStatus = MMA_MMC_SERVICE_STATUS_NO_SERVICE;

        Sta_DetachCnf (1, (struct MsgCB *)&stDetachCnf);
    }

}
VOS_VOID TAF_MMA_PnnListMemProtectTimerExpired(VOS_VOID)
{
    /* 超时消息有效性检查 */
    if ( MMA_TIMER_RUN != g_stPnnListMemProtectTimer.ucTimerStatus )
    {
        MMA_WARNINGLOG("TAF_MMA_PnnListWaitAtCnfTimerExpired():WARNING:The timer is not running!");
        return;
    }

    /*设置停止定时器标志*/
    g_stPnnListMemProtectTimer.ucTimerStatus = MMA_TIMER_STOP;

    /* 将gstMmaPnnListInfo恢复为初始化状态 */
    TAF_MMA_ClearcPnnListQryCtrInfo();
}
VOS_VOID TAF_MMA_RcvDelayPowerDownTimerExpired(VOS_VOID)
{
    /* 调用API接口进行下电关机 */
    DRV_POWER_OFF();
}


VOS_UINT32 MMA_TimeExpired(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    REL_TIMER_MSG                      *pMsg;

    pMsg = (REL_TIMER_MSG*)pstMsg;

    switch(pMsg->ulPara)
    {
        case TAF_USIM_OPPIN:
            MMA_OpPinExpired();
            break;

        case TAF_USIM_SPN:
            MMA_ReadSpnFileExpired();
            break;
        case MMA_INTERNAL_TIMER_ID :
            MMA_ExpiredInternalTimer();
            break;
        case MMA_TIMER_FOR_PC_REPLAY :
            /* 输出回放消息 */
#ifndef __PS_WIN32_RECUR__

            /* 已与PC工具连接, 且PC工具侧使能了NAS PC回放消息的发送 */
            if ( (VOS_TRUE == g_ucMmaOmConnectFlg)
              && (VOS_TRUE == g_ucMmaOmPcRecurEnableFlg) )
            {
                NAS_MMA_SndOutsideContextData();
                Api_AppMmLayerContextSndoutForPcReplay();
            }

#endif
            break;
        case TI_MN_PH_RESET:
            MN_PH_ResetTimerOutProc();
            break;
        case TI_MN_PH_PIN_OPER:
            MN_PH_PinOperTimerOutProc();
            break;
        case TI_MN_PH_CPHS_SIM_ONS:
            MMA_ReadCPHSOperNameStrFileExpired();
            break;

        case TI_MN_MMA_SYSCFG_WAIT_DETACH_CNF:
            MN_MMA_SyscfgWaitDetachCnfTimerExpired();
            break;

        case TI_TAF_MMA_PNN_LIST_MEM_PROTECT_TIMER:
            TAF_MMA_PnnListMemProtectTimerExpired();
            break;

        case TI_TAF_MMA_DELAY_POWER_DOWN:
            TAF_MMA_RcvDelayPowerDownTimerExpired();
            break;


        case TI_TAF_MMA_PERIOD_TRYING_NETWORK_SELECTION_MENU:
            TAF_MMA_NetworkSelectionMenuCtrlTimerExpired();
            break;

        case TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING:
            TAF_MMA_RatBalancingCtrlTimerExpired();
            break;

        case TI_TAF_MMA_NET_SCAN_TIMER:
            TAF_MMA_NetScanTimerExpired();
            break;

        case TI_TAF_MMA_ABORT_NET_SCAN_TIMER:
            TAF_MMA_AbortNetScanTimerExpired();
            break;

        default:
            Sta_TimeoutProc(pMsg);
            break;
    }
    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_IsEventNeedRpt(
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
)
{
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    ulResult                = VOS_TRUE;

    switch (pEvent->PhoneEvent)
    {
        /* ACINFO上报 */
        case TAF_MMA_EVT_AC_INFO_CHANGE_IND:

            ulResult = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                                TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                                TAF_SDC_RPT_CMD_ACINFO);

            break;

        /* 服务状态上报 */
        case TAF_PH_EVT_SERVICE_STATUS_IND:

            ulResult = TAF_MMA_IsServStatusChangeNeedRpt();

            break;

        /* 注册状态上报 */
        case TAF_PH_EVT_SYSTEM_INFO_IND:

            ulResult = TAF_MMA_IsRegStatusChangeNeedRpt(pEvent);

            break;

        /* RSSI/CERSSI上报 */
        case TAF_PH_EVT_RSSI_CHANGE_IND:

            ulResult = TAF_MMA_IsRssiChangeNeedRpt();

            break;

        /* TIME/CTZV上报 */
        case TAF_PH_EVT_MM_INFO_IND:

            ulResult = TAF_MMA_IsTimeChangeNeedRpt();

            break;

        /* 模式变化上报 */
        case TAF_PH_EVT_MODE_CHANGE_IND:

            ulResult = TAF_MMA_IsModeChangeNeedRpt();

            break;

        /* PLMN变化上报 */
        case TAF_PH_EVT_PLMN_CHANGE_IND:

            ulResult = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                                TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                                TAF_SDC_RPT_CMD_PLMN);

            break;

        /* 卡状态上报 */
        case TAF_PH_EVT_USIM_INFO_IND:

            ulResult = TAF_MMA_IsSimStaChangeNeedRpt();

            break;
        /* 增加AT^REJINFO的分支 */
        case TAF_MMA_EVT_REG_REJ_INFO_IND:

            ulResult = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                                TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                                TAF_SDC_RPT_CMD_REJINFO);

            break;

         case TAF_MMA_EVT_PLMN_SELECTION_INFO_IND:

            ulResult = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                                TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                                TAF_SDC_RPT_CMD_PLMNSELEINFO);

            break;

        default:
            ulResult = VOS_TRUE;
    }

    return ulResult;
}
VOS_VOID  Taf_PhoneEvent (TAF_PHONE_EVENT_INFO_STRU  *pEvent)
{
    /* BEGIN: Modified by liuyang id:48197, 2006/3/16   PN:A32D02382*/
    VOS_UINT8 ucTi;
    /* END:   Modified by liuyang id:48197, 2006/3/16 */
    VOS_UINT16 ClientId;
    VOS_UINT8 AppOpId;
    TAF_PHONE_EVENT PhoneEvent;

    TAF_SDC_CURC_RPT_CTRL_STRU          *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU   *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(pEvent->aucCurcRptCfg, pstCurcRptCtrl->aucRptCfg, TAF_MMA_RPT_CFG_MAX_SIZE);
    PS_MEM_CPY(pEvent->aucUnsolicitedRptCfg, pstUnsolicitedRptCtrl->aucRptCfg, TAF_MMA_RPT_CFG_MAX_SIZE);

    ucTi = pEvent->OpId;
    switch(pEvent->PhoneEvent)
    {
        case TAF_PH_EVT_SERVICE_STATUS_IND:
        /*新增的注册状态、信号质量、电池电量属于广播消息，*/
        case TAF_PH_EVT_PLMN_LIST_IND:
        case TAF_PH_EVT_SYSTEM_INFO_IND:
        case TAF_PH_EVT_POWER_STA_IND:
        case TAF_PH_EVT_RSSI_CHANGE_IND:
        case TAF_PH_EVT_MM_INFO_IND:
        case TAF_PH_EVT_MODE_CHANGE_IND:
        case MN_PH_EVT_SIMLOCKED_IND:
        case MN_PH_EVT_ROAMING_IND:
        case TAF_PH_EVT_PLMN_CHANGE_IND:
        case TAF_PH_EVT_NSM_STATUS_IND:
        case TAF_MMA_EVT_AC_INFO_CHANGE_IND:
            AppOpId = MMA_OP_ID_INTERNAL;
            ClientId = MMA_CLIENTID_BROADCAST;
            break;

        case TAF_PH_EVT_PLMN_LIST_CNF:
            /*APP发起的过程最大Ti是1*/
            if (ucTi <= TAF_MAX_STATUS_TI)
            {
                /*通过Ti获取ClientId,CallId*/
                if (TAF_SUCCESS != MMA_GetIdByTi(TAF_MMA, ucTi, &ClientId, &AppOpId, &PhoneEvent))
                {
                    MMA_WARNINGLOG("Taf_PhoneEvent():WARNING:Invoke Taf_GetIdByTi failed");
                    return;
                }
            }
            else
            {   /*是STATUS主动发起的事件*/
                AppOpId = MMA_OP_ID_INTERNAL;
                /*ClientId置为广播值:高字节的低4bit类型是MUX_PORT_BUTT*/
                ClientId = MMA_CLIENTID_BROADCAST;
            }
            break;
         case TAF_PH_EVT_OP_PINREMAIN_CNF :
            ucTi = MMA_PH_PIN_TI;
            if (TAF_SUCCESS != MMA_GetIdByTi(TAF_MMA, ucTi, &ClientId, &AppOpId,&PhoneEvent))
            {
                MMA_WARNINGLOG("Taf_PhoneEvent():WARNING:Invoke Taf_GetIdByTi failed.");
                return;
            }
            break;

        /*
        不需要对数据处理，只需将下层发上来的信息，通过Ti找到ClientiD、Opid
          直接传给上层对应的借口即可
        */
        case TAF_PH_EVT_ATTACH_CNF :
        case TAF_PH_EVT_PS_ATTACH_CNF :
        case TAF_PH_EVT_CS_ATTACH_CNF :
        case TAF_PH_EVT_PS_DETACH_CNF :
        case TAF_PH_EVT_CS_DETACH_CNF :
        case TAF_PH_EVT_DETACH_CNF :
            /*APP发起的过程最大Ti是1*/
            if (ucTi <= TAF_MAX_STATUS_TI)
            {
                /*通过Ti获取ClientId,CallId*/
                if (TAF_SUCCESS != MMA_GetIdByTi(TAF_MMA, ucTi, &ClientId, &AppOpId, &PhoneEvent))
                {
                    MMA_WARNINGLOG("Taf_PhoneEvent():WARNING:Invoke Taf_GetIdByTi failed.");
                    return;
                }
                /*填入ATTACH、DETACH事件通知AT此次是什么操作*/
                pEvent->PhoneEvent = PhoneEvent;
            }
            else
            {
                /*status单独发起的DETTACH过程不上报给上层*/
                MMA_INFOLOG1("Taf_PhoneEvent():INFO:INTERNAL PROC Event=", pEvent->PhoneEvent);
                return;
            }

            break;

        /*PIN操作的结构已经填充完成，考虑到PIN的某些操作,更改密码等会与其他事件
        并发，不使用PHONE EVENT事件结构体内包含了ClientId和OpId*/
        case TAF_PH_EVT_OP_PIN_CNF :
            ucTi = MMA_PH_PIN_TI;
            if (TAF_SUCCESS != MMA_GetIdByTi(TAF_MMA, ucTi, &ClientId, &AppOpId,&PhoneEvent))
            {
                MMA_WARNINGLOG("Taf_PhoneEvent():WARNING:Invoke Taf_GetIdByTi failed.");
                return;
            }
            break;

        /*TAF_PH_EVT_STOP_CNF, TAF_PH_EVT_START_CNF事件
        会走此分支, 此类事件没有具体参数*/
        default:
            if (ucTi > TAF_MAX_STATUS_TI)
            {   /*不是APP主动发起，直接返回*/
                return;
            }
            /*通过Ti获取ClientId,CallId*/
            if (TAF_SUCCESS != MMA_GetIdByTi(TAF_MMA, ucTi, &ClientId, &AppOpId,&PhoneEvent))
            {
                MMA_WARNINGLOG("Taf_PhoneEvent():WARNING:Invoke Taf_GetIdByTi failed.");
                return;
            }
            break;
    }

    pEvent->ClientId  = ClientId;
    pEvent->OpId      = AppOpId;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(pEvent->ClientId,(VOS_UINT8*)pEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    return;
}

/* MMA_DefPhClassType函数不再使用，删除 */


VOS_VOID MMA_DefQuickStartStatus(VOS_UINT16 ClientId, VOS_UINT8 OpId,
                                 VOS_UINT32 ulSetValue)
{
    NAS_NVIM_FOLLOWON_OPENSPEED_FLAG_STRU   stQuickStartFlg;
    VOS_UINT32  ulResult;

    if (0 == ulSetValue)
    {
        stQuickStartFlg.ulQuickStartSta = MMA_QUICK_START_DISABLE;
    }
    else
    {
        stQuickStartFlg.ulQuickStartSta = MMA_QUICK_START_ENABLE;

    }
    ulResult = MMA_WriteValue2Nvim(en_NV_Item_FollowOn_OpenSpeed_Flag, (VOS_VOID *)(&stQuickStartFlg), sizeof(NAS_NVIM_FOLLOWON_OPENSPEED_FLAG_STRU));

    if (MMA_SUCCESS !=ulResult)
    {
        Taf_ParaSetResult(ClientId, OpId, TAF_PARA_WRITE_NVIM_ERROR, TAF_PH_QUICK_START_PARA, VOS_NULL_PTR);
    }
    else
    {
        gstMmaValue.ulQuickStartFlg = stQuickStartFlg.ulQuickStartSta;
        Taf_ParaSetResult(ClientId, OpId, TAF_PARA_OK, TAF_PH_QUICK_START_PARA, VOS_NULL_PTR);
    }
    return;
}

VOS_VOID MMA_DefPhAutoAttachStatus(VOS_UINT16 ClientId, VOS_UINT8 OpId,
                                 VOS_UINT32 ulSetValue)
{
    VOS_UINT32                          ulData;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucAutoAttachFlg;
    NAS_NVIM_AUTOATTACH_STRU            stAutoattachFlag;
    VOS_UINT32                          ulLength;

    ulLength    = 0;
    ulResult    = 0;
    PS_MEM_SET(&stAutoattachFlag, 0x00, sizeof(NAS_NVIM_AUTOATTACH_STRU));

    NV_GetLength(en_NV_Item_Autoattach, &ulLength);
    if (NV_OK == NV_Read(en_NV_Item_Autoattach,
                          (VOS_VOID*)(&stAutoattachFlag),
                          ulLength))
    {
        MMA_INFOLOG1("MMA_DefPhAutoAttachStatus:Read result from NV", (VOS_INT32)stAutoattachFlag.usAutoattachFlag);
        if (1 == stAutoattachFlag.usAutoattachFlag)
        {
            ucAutoAttachFlg = MMA_AUTO_ATTACH_ENABLE;
        }
        else
        {
            ucAutoAttachFlg = MMA_AUTO_ATTACH_DISABLE;
        }
    }
    else
    {
        MMA_WARNINGLOG("MMA_DefPhAutoAttachStatus:WARNING: Read NV Failure!");
        ucAutoAttachFlg = MMA_AUTO_ATTACH_ENABLE;
    }

    if (0 == ulSetValue)
    {
        ulData = MMA_AUTO_ATTACH_DISABLE;
    }
    else if (1 == ulSetValue)
    {
        ulData = MMA_AUTO_ATTACH_ENABLE;
    }
    else
    {
        MMA_WARNINGLOG("MMA_DefPhAutoAttachStatus:WARNING: ulSetValue error");
        Taf_ParaSetResult(ClientId, OpId, TAF_PARA_SET_ERROR, TAF_PH_AUTO_ATTACH_PARA, VOS_NULL_PTR);
        return;
    }

    if ( ucAutoAttachFlg == ulData )
    {

        /* 返回成功，否则AT口会被占据 */
        Taf_ParaSetResult(ClientId, OpId, TAF_PARA_OK, TAF_PH_AUTO_ATTACH_PARA, VOS_NULL_PTR);

        /* NV项值没有改变，则直接返回 */
        return;
    }

    /* 值不一样，需要写NV */

    stAutoattachFlag.usAutoattachFlag = (VOS_UINT16)ulData;
    ulResult = MMA_WriteValue2Nvim(en_NV_Item_Autoattach, (VOS_VOID *)(&stAutoattachFlag), sizeof(VOS_UINT16));

    if (MMA_SUCCESS != ulResult)
    {
        MMA_WARNINGLOG("MMA_DefPhAutoAttachStatus:WARNING: Write NV Failure!");
        Taf_ParaSetResult(ClientId, OpId, TAF_PARA_WRITE_NVIM_ERROR, TAF_PH_AUTO_ATTACH_PARA, VOS_NULL_PTR);
    }
    else
    {
        Taf_ParaSetResult(ClientId, OpId, TAF_PARA_OK, TAF_PH_AUTO_ATTACH_PARA, VOS_NULL_PTR);
        if ((MMA_AUTO_ATTACH_DISABLE == ucAutoAttachFlg)
            &&(MMA_AUTO_ATTACH_ENABLE == ulData))
        {
            if ( (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != TAF_SDC_GetPsServiceStatus())
              && ( (STA_FSM_NULL     != g_StatusContext.ulFsmState)
                && (STA_FSM_STOP     != g_StatusContext.ulFsmState) )
              && (TAF_PH_MS_CLASS_CC != g_StatusContext.ucModeService)  )
            {
                Sta_AttachByModeService((VOS_UINT32)OpId);
            }
        }
    }
    return;
}
VOS_VOID MMA_FplmnQurey(VOS_UINT16        ClientId,
                               VOS_UINT8         OpId)
{
    TAF_PH_FPLMN_OPERATE_STRU stFplmnOperateforMmc;
    VOS_UINT8                 ucSimStatus;
    /* 获取卡状态 */
    MMA_PhoneGetUsimStatus(&ucSimStatus);
    /* 卡不存在则返回错误 */
    if (MMA_USIM_PRESENT != ucSimStatus)
    {
        MMA_WARNINGLOG("MMA_FplmnHandle():SIM/USIM Not Present!");
        Taf_ParaSetResult(ClientId, OpId, TAF_PARA_NO_USIM_ERROR, TAF_PH_FPLMN_PARA, VOS_NULL_PTR);
        return;
    }
    /* 设置命令类型为查询类型 */
    stFplmnOperateforMmc.ulCmdType = TAF_PH_FPLMN_QUREY;
    /* FPLMN公共处理 */
    Api_MmaFplmnHandleFromMmc(ClientId,OpId,stFplmnOperateforMmc);
}


VOS_VOID TAF_MMA_SndAtSubPnnList(
    VOS_UINT32                   ulSndNum
)
{
    TAF_PH_USIM_PNN_CNF_STRU           *pstPNN = VOS_NULL_PTR;
    TAF_PH_USIM_PNN_CNF_STRU           *pstPnnData = VOS_NULL_PTR;
    VOS_UINT16                          usCurrIndex;
    VOS_UINT16                          usEfLen;  /* 每条PNN的长度 */
    VOS_UINT16                          i;

    i           = 0;
    usCurrIndex = gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.usCurrIndex;
    usEfLen     = gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.usEfLen;
    pstPnnData  = (TAF_PH_USIM_PNN_CNF_STRU*)gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.pucPnnData;

    pstPNN = (TAF_PH_USIM_PNN_CNF_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA,
              sizeof(TAF_PH_USIM_PNN_CNF_STRU)+(sizeof(TAF_PH_USIM_PNN_RECORD)*ulSndNum));

    if(VOS_NULL_PTR == pstPNN)
    {
        MMA_ReportParaQueryError(gstMmaValue.stPNN.ClientId, gstMmaValue.stPNN.OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_PNN_PARA);

        /* 申请内存失败的时候将保存USIM内容的内存释放掉，将全局变量恢复成初始状态 */
        TAF_MMA_ClearcPnnListQryCtrInfo();
        /* 停止保护定时器 */
        TAF_MMA_PnnListMemProtectTimerProc(TAF_MMA_PROC_PNN_PROTECT_TIMER_STOP);
        return;
    }

    MMA_MEM_SET(pstPNN, 0, sizeof(TAF_PH_USIM_PNN_CNF_STRU)+(sizeof(TAF_PH_USIM_PNN_RECORD)*ulSndNum));

    pstPNN->TotalRecordNum            = ulSndNum;
    pstPNN->RecordLen                 = gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.usEfLen;
    pstPNN->usPnnCurrIndex            = gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.usCurrIndex;
    pstPNN->Icctype                   = gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.Icctype;


    for( i = 0; i< ulSndNum ; i++)
    {
        VOS_MemCpy(pstPNN->PNNRecord[i].PNN, &pstPnnData->PNNRecord[i+usCurrIndex], usEfLen);
    }

    MMA_ParaQueryReport(gstMmaValue.stPNN.ClientId, gstMmaValue.stPNN.OpId,
                        TAF_PH_PNN_PARA, TAF_ERR_NO_ERROR, pstPNN);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPNN);

}
VOS_VOID TAF_MMA_ClearcPnnListQryCtrInfo(VOS_VOID)
{
    /* 将全局变量恢复成初始化状态 */
    if (VOS_NULL_PTR != gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.pucPnnData)
    {
        PS_MEM_FREE(WUEPS_PID_MMA, gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.pucPnnData);
    }
    PS_MEM_SET(&gstMmaValue.stPNN.stPnnListInfo, 0x00, sizeof(gstMmaValue.stPNN.stPnnListInfo));
}
VOS_UINT32 TAF_MMA_PnnListMemProtectTimerProc(
    TAF_MMA_PROC_PNN_PROTECT_TIMER_ENUM_UINT8 enProcType
)
{
    VOS_UINT32                          ulRet;

    ulRet = VOS_TRUE;

    /* 开启定时器 */
    if (TAF_MMA_PROC_PNN_PROTECT_TIMER_START == enProcType)
    {
        ulRet = NAS_StartRelTimer(&g_stPnnListMemProtectTimer.MmaHTimer,
                                  WUEPS_PID_MMA,
                                  TI_TAF_MMA_PNN_LIST_MEM_PROTECT_TIMER_LEN,
                                  TAF_MMA,
                                  TI_TAF_MMA_PNN_LIST_MEM_PROTECT_TIMER,
                                  VOS_RELTIMER_NOLOOP );

        if( VOS_OK !=  ulRet)
        {
            MMA_WARNINGLOG("TAF_MMA_AtQryPnnListProtectTimerProc():ERROR:Start TI_TAF_MMA_PNN_LIST_MEM_PROTECT_TIMER Timer Error!");
        }
        else
        {
            g_stPnnListMemProtectTimer.ucTimerStatus = MMA_TIMER_RUN;
        }
    }
    /*关闭定时器*/
    else if (TAF_MMA_PROC_PNN_PROTECT_TIMER_STOP == enProcType)
    {
        if ( MMA_TIMER_RUN == g_stPnnListMemProtectTimer.ucTimerStatus )
        {
            ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                     TI_TAF_MMA_PNN_LIST_MEM_PROTECT_TIMER,
                                     &(g_stPnnListMemProtectTimer.MmaHTimer));
            if (VOS_OK != ulRet)
            {
                MMA_WARNINGLOG("TAF_MMA_AtQryPnnListProtectTimerProc():WARNING:NAS_StopRelTimer failed");
            }

            /*设置停止定时器标志*/
            g_stPnnListMemProtectTimer.ucTimerStatus = MMA_TIMER_STOP;
        }
    }
    else
    {
    }

    return ulRet;
}
VOS_VOID TAF_MMA_AtQryPnnListProc(VOS_VOID)
{
    VOS_UINT32                          ulSndNum; /* 本次发送的Pnn数目 */
    VOS_UINT16                          usUnSentNum;
    VOS_UINT8                           ucIsThereUnSendPnnLeft;

    ulSndNum               = 0;
    usUnSentNum            = 0;
    ucIsThereUnSendPnnLeft = VOS_TRUE;

    /* 参数检查，如果当前从卡中读取到的PNN内容为空，则给AT回复一个空列表 */
    if ((VOS_NULL_PTR == gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.pucPnnData)
                 ||(0 == gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.usSumPnnNumInSim)
                 ||(0 == gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.usEfLen))
    {
        MMA_WARNINGLOG("TAF_MMA_AtQryPnnListProc():The PNN LIST is empty!");

        ulSndNum               = 0;
        TAF_MMA_SndAtSubPnnList(ulSndNum);
        TAF_MMA_ClearcPnnListQryCtrInfo();
        return;
    }

    /* 如果要求读取的index大于等于PNN的总数目，认为是最后一次上报，上报一个空列表，并停止保护定时器 */
    if (gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.usCurrIndex >= gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.usSumPnnNumInSim)
    {
        MMA_WARNINGLOG("TAF_MMA_AtQryPnnListProc():There is no unsent pnn in the list!");

        ulSndNum               = 0;
        TAF_MMA_SndAtSubPnnList(ulSndNum);
        /* 将全局变量恢复成初始化状态 */
        TAF_MMA_ClearcPnnListQryCtrInfo();

        /* 停止定时器 */
        TAF_MMA_PnnListMemProtectTimerProc(TAF_MMA_PROC_PNN_PROTECT_TIMER_STOP);
        return;
    }

    /* 如果PNN LIST总数目减去AT下发的INDEX大于等于AT要求读取的数目，则上报AT要求读取的数目 */
    usUnSentNum = gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.usSumPnnNumInSim - gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.usCurrIndex;

    if (usUnSentNum >= gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.usReadNum)
    {
        ulSndNum = gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.usReadNum;
        ucIsThereUnSendPnnLeft = VOS_TRUE;
    }
    else
    {
        ulSndNum               = usUnSentNum;
        ucIsThereUnSendPnnLeft = VOS_FALSE;
    }

    TAF_MMA_SndAtSubPnnList(ulSndNum);

    /* 如果是首次上报并且后面需要继续上报，则开启保护定时器 */
    if ((0 == gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.usCurrIndex)
      &&(VOS_TRUE == ucIsThereUnSendPnnLeft))
    {
        /* 启动等待AT新请求的15s保护定时器，如果定时器超时则主动释放掉内存 */
        TAF_MMA_PnnListMemProtectTimerProc(TAF_MMA_PROC_PNN_PROTECT_TIMER_START);
    }

    /* 如果没有需要继续上报的记录，则将全局变量重新初始化，并且不是首次上报的话，就停止保护定时器 */
    if (VOS_FALSE == ucIsThereUnSendPnnLeft)
    {
        /* 只报一次的情况下不启动定时器不需要停止，停止保护定时器 */
        if (0 != gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.usCurrIndex)
        {
            TAF_MMA_PnnListMemProtectTimerProc(TAF_MMA_PROC_PNN_PROTECT_TIMER_STOP);
        }

        /* 将全局变量恢复成初始化状态 */
        TAF_MMA_ClearcPnnListQryCtrInfo();
    }
}
VOS_UINT8 TAF_MMA_IsNeedSndGetUsimPnnFileReq(
    VOS_UINT16        ClientId,
    VOS_UINT8         OpId,
    TAF_PH_QRY_USIM_INFO_STRU          *pstInfo
)
{
    VOS_UINT8                           ucRet;

    /* 如果AT查询的是PNN列表，则判断一下是否是第一次AT查询，如果是则需要读取卡文件，如果不是的话则不需要读卡文件 */
    if ((TAF_PH_PNN_FILE == pstInfo->enEfId) && (0 == pstInfo->ulRecNum))
    {
        gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.usCurrIndex = pstInfo->stPnnQryIndex.usPnnCurrIndex;
        gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.usReadNum   = pstInfo->stPnnQryIndex.usPnnNum;
        gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.Icctype     = pstInfo->Icctype;

        if (pstInfo->stPnnQryIndex.usPnnCurrIndex > 0)
        {
            gstMmaValue.stPNN.ClientId    = ClientId;
            gstMmaValue.stPNN.OpId        = OpId;

            /* 使用缓存的信息进行上报，不再读取卡文件 */
            ucRet = VOS_FALSE;
        }
        /* 如果是首次读卡，则把全局变量置位，收到卡的回复之后，根据全局变量进行分次发送 */
        else
        {
            gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.ucAtPnnWaitCardCnfFlg = VOS_TRUE;
            ucRet = VOS_TRUE;
        }
    }
    /* 如果查询的是单个PNN记录，则需要读取卡文件 */
    else
    {
        ucRet = VOS_TRUE;
    }

    return ucRet;
}



VOS_VOID MMA_QryUsimInfo(
    VOS_UINT16        ClientId,
    VOS_UINT8         OpId,
    TAF_PH_QRY_USIM_INFO_STRU           *pstInfo
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucSimStatus;
    VOS_UINT8                           ucRecordNum;
    VOS_UINT16                          usEfId;
    USIMM_APP_TYPE_ENUM_UINT32          enAppType;
    VOS_UINT32                          ulFileExist;
    TAF_PARA_TYPE                       enRptQryType;

    USIMM_GET_FILE_INFO_STRU            stGetFileInfo;


    if(MMA_TIMER_STOP != gstMmaInternalTimer.stTimer.ucTimerStatus)
    {
        MMA_WARNINGLOG("MMA_QryUsimInfo():WARNING:USIM is not Ready!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 TAF_PH_PNN_PARA);
        return;
    }

    /*判断卡状态*/
    ulRet = MMA_PhoneGetUsimStatus(&ucSimStatus );
    if ( (ulRet           != MMA_SUCCESS )
       ||(MMA_USIM_ABSENT == ucSimStatus)
       )
    {
        MMA_WARNINGLOG("MMA_QryUsimInfo():WARNING:USIM is Error or Absent!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_USIM_SIM_CARD_NOTEXIST,
                                 TAF_PH_PNN_PARA);
        return;
    }

    if (TAF_PH_PNN_FILE == pstInfo->enEfId)
    {
        usEfId = MMA_USIM_PNN_FILE_INDEX;

        ulFileExist = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_PLMN_NTWRK_NAME);

        enRptQryType = TAF_PH_PNN_PARA;
    }
    else
    {
       usEfId = MMA_USIM_OPL_FILE_INDEX;

       ulFileExist = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_OPLMN_LIST);

       enRptQryType = TAF_PH_OPL_PARA;
    }

    if (PS_USIM_SERVICE_NOT_AVAILIABLE == ulFileExist)
    {
        if (VOS_TRUE == MMA_IsNeedToDisplayCPHSOperNameStr())
        {
            /*使用CPHS文件的 SIM卡 PNN上报其6f14的值，Opl返回OK,与后台配合以满足CPHS的显示要求*/
            if (TAF_PH_PNN_FILE == pstInfo->enEfId)
            {
                MMA_ReportPNNParaFromCPHSOperNameStr(ClientId, OpId);
            }
            else
            {
                MMA_ReportOPLParaForCPHSDisplay(ClientId, OpId);
            }
        }
        else
        {
            MMA_WARNINGLOG("MMA_QryUsimInfo():WARNING:file not exist!");
            MMA_ReportParaQueryError(ClientId, OpId,
                                     TAF_ERR_FILE_NOT_EXIST,
                                     enRptQryType);
        }
        return;
    }

    if (TAF_PH_ICC_SIM == pstInfo->Icctype)
    {
        enAppType = USIMM_GSM_APP;
    }
    else
    {
        enAppType = USIMM_UMTS_APP;
    }

    if (TAF_PH_INVALID_USIM_REC == pstInfo->ulRecNum)
    {
        /* 获取记录列表 */
        ucRecordNum  = 0;
        f_bGetRecRange = VOS_TRUE;
        f_stRecInfo.enNextPhase = MMA_GET_REC_II;
        f_stRecInfo.enAppType = enAppType;
    }
    else
    {
        ucRecordNum = (VOS_UINT8)(pstInfo->ulRecNum);
    }

    /* 如果是查询PNN列表，需要进行判断，如果是首次请求，则读取卡文件，否则根据缓存中卡信息上报AT，保证只读取一次卡文件 */
    if (VOS_FALSE == TAF_MMA_IsNeedSndGetUsimPnnFileReq(ClientId, OpId, pstInfo))
    {
        /* 使用缓存的信息进行上报，不再读取卡文件 */
        TAF_MMA_AtQryPnnListProc();
        return;
    }

    TAF_COMM_BUILD_USIM_GET_FILE_INFO( &stGetFileInfo, enAppType, usEfId, ucRecordNum);

    if( USIMM_API_SUCCESS != NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA, 0, &stGetFileInfo))
    {
        MMA_WARNINGLOG("MMA_PnnQuery():WARNING:get PNN File Error!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 enRptQryType);
        return;
    }

    /*启动定时器*/
    if(MMA_SUCCESS != MMA_StartInternalTimer(1,
                                             MMA_GET_USIM_PNN,
                                             ClientId,
                                             OpId))
    {
        MMA_WARNINGLOG("MMA_PnnQuery():WARNING:Unknow Error!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 enRptQryType);
        return;
    }
    if (TAF_PH_PNN_FILE == pstInfo->enEfId)
    {
        gstMmaValue.stPNN.ClientId = ClientId;
        gstMmaValue.stPNN.OpId     = OpId;
    }
    else
    {
        gstMmaValue.stOPL.ClientId = ClientId;
        gstMmaValue.stOPL.OpId     = OpId;
    }
}


VOS_VOID MMA_CPnnQuery(VOS_UINT16        ClientId,
                              VOS_UINT8         OpId,
                              TAF_PH_ICC_TYPE   *pIccType)
{
    VOS_UINT32                      ulRet;
    VOS_UINT8                       ucSimStatus;
    USIMM_APP_TYPE_ENUM_UINT32      enAppType;
    VOS_UINT32                      ulPnnExistFlg;
    VOS_UINT32                      ulOplExistFlg;

    USIMM_GET_FILE_INFO_STRU stGetFileInfo;

    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    if(MMA_TIMER_STOP != gstMmaInternalTimer.stTimer.ucTimerStatus)
    {
        MMA_WARNINGLOG("MMA_PnnQuery():WARNING:USIM is not Ready!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 TAF_PH_CPNN_PARA);
        return;
    }

    /*判断卡状态*/
    ulRet = MMA_PhoneGetUsimStatus(&ucSimStatus );
    if ( (ulRet           != MMA_SUCCESS )
       ||(MMA_USIM_ABSENT == ucSimStatus)
       )
    {
        MMA_WARNINGLOG("MMA_PnnQuery():WARNING:USIM is Error or Absent!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_USIM_SIM_CARD_NOTEXIST,
                                 TAF_PH_CPNN_PARA);
        return;
    }

    if ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != enCsServiceStatus)
     && (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != enPsServiceStatus) )
    {
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 TAF_PH_CPNN_PARA);
        return;
    }
    else
    {
        ulPnnExistFlg = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_PLMN_NTWRK_NAME);
        ulOplExistFlg = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_OPLMN_LIST);

        if ( (PS_USIM_SERVICE_NOT_AVAILIABLE == ulPnnExistFlg)
            || (PS_USIM_SERVICE_NOT_AVAILIABLE == ulOplExistFlg) )
        {
            MMA_ReportParaQueryError(ClientId, OpId,
                                     TAF_ERR_UNSPECIFIED_ERROR,
                                     TAF_PH_CPNN_PARA);
            return;
        }

        if (TAF_PH_ICC_USIM == *pIccType)
        {
            enAppType = USIMM_UMTS_APP;
        }
        else
        {
            enAppType = USIMM_GSM_APP;
        }

        TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                        enAppType,
                                        MMA_USIM_PNN_FILE_INDEX,
                                        0);

        if( USIMM_API_SUCCESS != NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA,
                                                 0,
                                                 &stGetFileInfo))
        {
            MMA_WARNINGLOG("MMA_PnnQuery():WARNING:get PNN File Error!");
            MMA_ReportParaQueryError(ClientId, OpId,
                                     TAF_ERR_UNSPECIFIED_ERROR,
                                     TAF_PH_CPNN_PARA);
            return;
        }
        else
        {
            gstMmaValue.stCPNN.ucWaitPnnOPLMask |= MMA_CPNN_WAIT_PNN_FLAG;
        }

        TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo, enAppType, MMA_USIM_OPL_FILE_INDEX, 0);

        if( USIMM_API_SUCCESS != NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA,
                                                 0,
                                                 &stGetFileInfo))
        {
            MMA_WARNINGLOG("MMA_PnnQuery():WARNING:get OPL File Error!");
            MMA_ReportParaQueryError(ClientId, OpId,
                                     TAF_ERR_UNSPECIFIED_ERROR,
                                     TAF_PH_CPNN_PARA);

            gstMmaValue.stCPNN.ucWaitPnnOPLMask = MMA_CPNN_WAIT_NULL;
            return;
        }
        else
        {
            gstMmaValue.stCPNN.ucWaitPnnOPLMask |= MMA_CPNN_WAIT_OPL_FLAG;
        }
    }

    /*启动定时器*/
    if(MMA_SUCCESS != MMA_StartInternalTimer(1,
                                             MMA_GET_USIM_CPNN,
                                             ClientId,
                                             OpId))
    {
        MMA_WARNINGLOG("MMA_CPnnQuery():WARNING:Unknow Error!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 TAF_PH_CPNN_PARA);

        gstMmaValue.stCPNN.ucWaitPnnOPLMask = MMA_CPNN_WAIT_NULL;
        return;
    }

    gstMmaValue.stCPNN.ClientId = ClientId;
    gstMmaValue.stCPNN.OpId     = OpId;
    gstMmaValue.stCPNN.ulPNNGetRslt = VOS_FALSE;
    return;

}

VOS_UINT32 MMA_GetPlmnIdxInOplList(TAF_SDC_PLMN_ID_STRU stPlmn)
{
    TAF_UINT32                   i ,j,ulIdx;
    TAF_PH_USIM_OPL_CNF_STRU      *pstOPL;
    TAF_UINT32                   ulOplRcdNum;
    TAF_CHAR                     strPlmn[7];

    PS_MEM_SET(strPlmn,0,7);

    MMA_PlmnId2Bcd((TAF_PLMN_ID_STRU *)&stPlmn);
    stPlmn.ulMcc &= 0x0fff;
    stPlmn.ulMnc &= 0x0fff;
    if( 0x0F == ((0x0f00 & stPlmn.ulMnc) >> 8))
    {
        stPlmn.ulMnc &= 0x00ff;
        VOS_sprintf(strPlmn,"%03X%02X",stPlmn.ulMcc,stPlmn.ulMnc);
    }
    else
    {
        VOS_sprintf(strPlmn,"%03X%03X",stPlmn.ulMcc,stPlmn.ulMnc);
    }


    pstOPL = gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf;
    ulOplRcdNum = pstOPL->TotalRecordNum;
    ulIdx = ulOplRcdNum;

    for ( j = 0; j < ulOplRcdNum; j++ )
    {
        for ( i =0; i < pstOPL->OPLRecord[j].PlmnLen; i++ )
        {
            if ((strPlmn[i]-'0') != pstOPL->OPLRecord[j].PLMN[i])
            {
                break;
            }
        }

        if ( i == pstOPL->OPLRecord[j].PlmnLen)
        {
            ulIdx = j;
            break;
        }
    }
    return ulIdx;
}



VOS_VOID MMA_CPnnReport()
{
    TAF_PH_USIM_PNN_CNF_STRU    *pstPNN;
    TAF_UINT8                    ucRoamFlag = TAF_PH_NO_ROAM;
    TAF_UINT32                   ulOplIdx = 0;
    TAF_UINT32                   ulPlmnIdxInPNN = 0;
    TAF_UINT32                   ulOplRcdNum;

    pstPNN   = (TAF_PH_USIM_PNN_CNF_STRU *)VOS_MemAlloc(WUEPS_PID_MMA, DYNAMIC_MEM_PT,
                                                        sizeof(TAF_PH_USIM_PNN_CNF_STRU));
    if(VOS_NULL_PTR == pstPNN)
    {
        MMA_ReportParaQueryError(gstMmaValue.stCPNN.ClientId, gstMmaValue.stCPNN.OpId,
                            TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_CPNN_PARA);
    }
    else
    {
        PS_MEM_SET(pstPNN, 0, sizeof(TAF_PH_USIM_PNN_CNF_STRU));
        pstPNN->RecordLen = gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf->RecordLen;
        pstPNN->TotalRecordNum = 0;

        ucRoamFlag  = MMA_IsRoam(TAF_SDC_GetCurrCampPlmnId());

        if ( 0 != gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf->TotalRecordNum )
        {
            if (TAF_PH_NO_ROAM == ucRoamFlag)
            {
                pstPNN->TotalRecordNum = 1;
                PS_MEM_CPY(pstPNN->PNNRecord[0].PNN,
                            gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf->PNNRecord[0].PNN, pstPNN->RecordLen);

            }
            else if (0 != gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->TotalRecordNum)
            {
                ulOplIdx = MMA_GetPlmnIdxInOplList(*(TAF_SDC_GetCurrCampPlmnId()));

                ulOplRcdNum = gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->TotalRecordNum;

                if (ulOplIdx < ulOplRcdNum)
                {
                    ulPlmnIdxInPNN = gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[ulOplIdx].PNNIndex ;
                    if ( ulPlmnIdxInPNN > 0)
                    {
                        ulPlmnIdxInPNN -= 1;
                    }
                    if ( ulPlmnIdxInPNN < gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf->TotalRecordNum )
                    {
                        pstPNN->TotalRecordNum = 1;
                        PS_MEM_CPY(pstPNN->PNNRecord[0].PNN,
                                    gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf->PNNRecord[ulPlmnIdxInPNN].PNN, pstPNN->RecordLen);
                    }

                }

            }
            else
            {
            }

        }

        if ( ( 0 != pstPNN->TotalRecordNum ) || ( TAF_PH_NO_ROAM == ucRoamFlag ) )
        {
            MMA_ParaQueryReport(gstMmaValue.stCPNN.ClientId, gstMmaValue.stCPNN.OpId,
                            TAF_PH_CPNN_PARA, TAF_ERR_NO_ERROR, pstPNN);
        }
        else
        {
            MMA_ReportParaQueryError(gstMmaValue.stCPNN.ClientId, gstMmaValue.stCPNN.OpId,
                                TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_CPNN_PARA);
        }
    }
    PS_MEM_FREE(WUEPS_PID_MMA, pstPNN);
    PS_MEM_FREE(WUEPS_PID_MMA, gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf);
    PS_MEM_FREE(WUEPS_PID_MMA, gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf);
    gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf = VOS_NULL_PTR;
    gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf = VOS_NULL_PTR;
    gstMmaValue.stCPNN.ulPNNGetRslt = VOS_FALSE;


}


VOS_VOID Mma_LoadDefault(VOS_VOID)
{
    NAS_NVIM_MS_CLASS_STRU              stMsClass;
    NAS_NVIM_SELPLMN_MODE_STRU          stPlmnSelMode;
    MMA_UE_SUPPORT_FREQ_BAND_STRU       stSetBand;
    VOS_UINT32                          ulLength;
    VOS_UINT32                          ulAutoSwitchOnFlg;

    /*set default value:+CGCLASS*/
    PS_MEM_SET(&stMsClass, 0x00, sizeof(NAS_NVIM_MS_CLASS_STRU));
    stMsClass.ucMsClass = TAF_PH_MS_CLASS_A;
    ulLength    = 0;

    gstMmaValue.stSetMsClass.NewMsClassType = stMsClass.ucMsClass;
    NV_GetLength(en_NV_Item_MMA_MsClass, &ulLength);

    if (NV_OK != NV_Write(en_NV_Item_MMA_MsClass, &stMsClass, ulLength))
    {
        MMA_WARNINGLOG("Mma_LoadDefault():WARNING:Write NV:MsClass FAIL!");
    }

    stPlmnSelMode.usSelPlmnMode = MMA_MMC_PLMN_SEL_MODE_AUTO;
    NV_GetLength(en_NV_Item_SelPlmn_Mode, &ulLength);

    if (NV_OK != NV_Write (en_NV_Item_SelPlmn_Mode, &stPlmnSelMode, ulLength))
    {
        MMA_WARNINGLOG("Mma_LoadDefault():WARNING:Write NV:SelPlmn_Mode, FAIL!");
    }

    PS_MEM_SET(stSetBand.aucUeSupportGsmBand,(VOS_CHAR)0xff,NVIM_MAX_FDD_FREQ_BANDS_NUM);

    stSetBand.unGsmBand.BitBand.BandGsm850 = 1;
    stSetBand.unGsmBand.BitBand.BandGsmP900 = 1;
    stSetBand.unGsmBand.BitBand.BandGsmR900 = 1;
    stSetBand.unGsmBand.BitBand.BandGsmE900 = 1;
    stSetBand.unGsmBand.BitBand.BandGsm1800 = 1;
    stSetBand.unGsmBand.BitBand.BandGsm1900 = 1;

    stSetBand.unWcdmaBand.BitBand.BandWCDMA_I_2100 = 1;
    stSetBand.unWcdmaBand.BitBand.BandWCDMA_II_1900 = 1;
    stSetBand.unWcdmaBand.BitBand.BandWCDMA_V_850 = 1;

    stSetBand.aucUeSupportGsmBand[0]=3;
    stSetBand.aucUeSupportGsmBand[1]=4;
    stSetBand.aucUeSupportGsmBand[2]=5;
    stSetBand.aucUeSupportGsmBand[3]=6;
    stSetBand.aucUeSupportGsmBand[4]=7;
    stSetBand.aucUeSupportGsmBand[5]=8;

    PS_MEM_SET(stSetBand.aucUeSupportWcdmaBand,(VOS_CHAR)0xff,NVIM_MAX_FDD_FREQ_BANDS_NUM);
    stSetBand.aucUeSupportWcdmaBand[0]=1;
    stSetBand.aucUeSupportWcdmaBand[1]=2;
    stSetBand.aucUeSupportWcdmaBand[2]=5;


    /* 更新频段NV */
    if (NV_OK != NV_Write(en_NV_Item_Support_Freqbands,
                          &(stSetBand),
                          sizeof(MMA_UE_SUPPORT_FREQ_BAND_STRU)))
    {
        MMA_WARNINGLOG("Mma_LoadDefault():WARNING:NVIM_Write en_NV_Item_Support_Freqbands failed");
    }

    ulAutoSwitchOnFlg = MMA_RF_AUTO_TEST_DISABLE;
    NV_GetLength(en_NV_Item_RF_Auto_Test_Flag, &ulLength);

    if (NV_OK != NV_Write(en_NV_Item_RF_Auto_Test_Flag,
            (VOS_VOID*)(&ulAutoSwitchOnFlg),
            ulLength))
    {
        MMA_WARNINGLOG("Mma_LoadDefault():WARNING:NVIM_Write en_NV_Item_Support_Freqbands failed");
    }

    TAF_MMA_SetAutoSwitchOnFlg(ulAutoSwitchOnFlg);
}


VOS_UINT32 MMA_LoadDefaultVaule(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstRcvMsg;

    pstRcvMsg = (MN_APP_REQ_MSG_STRU *)pstMsg;

    /*MMA Load Default*/
    Mma_LoadDefault();

    /*Aps load default*/
    Aps_LoadDefault();


    /*上报结果*/
    Taf_ParaSetResult(pstRcvMsg->clientId, pstRcvMsg->opId, TAF_PARA_OK, TAF_PH_LOAD_DEFAULT, VOS_NULL_PTR);

    return VOS_TRUE;
}
VOS_VOID MMA_GetQuickStartStatus(VOS_UINT16 ClientId,VOS_UINT8 OpId)
{
    VOS_UINT8                           ucPara;

    if (MMA_QUICK_START_ENABLE == gstMmaValue.ulQuickStartFlg)
    {
        ucPara = 1;
    }
    else
    {
        ucPara = 0;
    }

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_QUICK_START_PARA,TAF_ERR_NO_ERROR,(VOS_VOID*)&ucPara);
}

VOS_VOID MMA_GetPhAutoAttachStatus(VOS_UINT16 ClientId,VOS_UINT8 OpId)
{
    VOS_UINT8                           ucPara;
    NAS_NVIM_AUTOATTACH_STRU            stAutoattachFlag;
    VOS_UINT32                          ulLength;

    ulLength        = 0;
    PS_MEM_SET(&stAutoattachFlag, 0x00, sizeof(NAS_NVIM_AUTOATTACH_STRU));
    NV_GetLength(en_NV_Item_Autoattach, &ulLength);

    if ( NV_OK != NV_Read(en_NV_Item_Autoattach,
                           (VOS_VOID*)(&stAutoattachFlag),
                           ulLength))
    {
        MMA_WARNINGLOG("MMA_GetPhAutoAttachStatus:Read Auto Attach Status Failed!");
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_AUTO_ATTACH_PARA, TAF_ERR_NO_ERROR, (VOS_VOID*)&ucPara);
    }
    else
    {
        MMA_INFOLOG1("MMA_GetPhAutoAttachStatus:Read result from NV",(VOS_INT32)(stAutoattachFlag.usAutoattachFlag));
        if (0 == stAutoattachFlag.usAutoattachFlag)
        {
            ucPara = MMA_AUTO_ATTACH_DISABLE;
        }
        else
        {
            ucPara = MMA_AUTO_ATTACH_ENABLE;
        }
        /*使用数据上报函数将数据传至TAFm*/
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_AUTO_ATTACH_PARA, TAF_ERR_NO_ERROR, (VOS_VOID*)&ucPara);
    }

    return;
}
VOS_VOID MMA_HSQuery(VOS_UINT16 ClientId, VOS_UINT8 OpId)
{
    VOS_UINT8               aucBuf[9];
    VOS_UINT8              *pPara = VOS_NULL_PTR;
    VOS_UINT32              ulId;
    VOS_UINT8               ucPhoneMode;

    ucPhoneMode = TAF_SDC_GetCurPhoneMode();

    ulId      = MMA_HS_ID;
    PS_MEM_CPY(aucBuf, &ulId, sizeof(VOS_UINT32));                              /*id*/

    aucBuf[4] = MMA_HS_PROTOCOL_APP;                                            /*protocol*/

    if ((TAF_PH_MODE_FULL == ucPhoneMode)                    /*UE CUR CFUN MODE*/
     || (TAF_PH_MODE_FT == ucPhoneMode))
    {
        aucBuf[5] = MMA_HS_ONLINE;
    }
    else if (TAF_PH_MODE_RFOFF == ucPhoneMode)
    {
        aucBuf[5] = MMA_HS_OFFLINE;
    }
    else
    {
        aucBuf[5] = MMA_HS_LPM;
    }

    aucBuf[6] = MMA_HS_PRODUCT_CLASS_GW;                                        /*product_class*/
    aucBuf[7] = MMA_HS_PRODUCT_ID;                                              /*product id*/
    aucBuf[8] = 0x0;                                                            /*software_id*/
    pPara = (VOS_UINT8 *)aucBuf;

    /*report the query result by calling call back function*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_HS_PARA, TAF_ERR_NO_ERROR, pPara);

    return;
}

/*<==A32D06630*/

VOS_UINT32 MMA_WriteValue2Nvim(const NV_ID_ENUM_U16 enNVItemType,
                               VOS_VOID *pData, VOS_UINT16 usDataLen)
{
    VOS_UINT8* pNvTemp;
    VOS_UINT8* pDataTemp;
    VOS_UINT16 i;

    i = 0;
    pDataTemp = (VOS_UINT8*)pData;

    pNvTemp = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_MMA, usDataLen);

    if (VOS_NULL_PTR == pNvTemp)
    {
        MMA_ERRORLOG("MMA_WriteValue2Nvim:ERROR:ALLOC MEMORY FAIL.");
        return MMA_ERROR;
    }

    PS_MEM_SET(pNvTemp, 0, usDataLen);

    if ( NV_OK == NV_Read(enNVItemType, pNvTemp, usDataLen) )
    {
        for ( i = 0 ; i < usDataLen ; i++ )
        {
            if ( *(pDataTemp + i) != *(pNvTemp + i))
            {
                break;
            }
        }
    }

    MMC_MEM_FREE(VOS_MEMPOOL_INDEX_NAS, pNvTemp);

    if ( i == usDataLen )
    {
        MMA_INFOLOG("MMA_WriteValue2Nvim():The content to write is same as NV's");
        return MMA_SUCCESS;
    }

    if (NV_OK !=  NV_Write(enNVItemType, pData, usDataLen))
    {
         MMA_WARNINGLOG("MMA_WriteValue2Nvim():WARNING:Invoke NVIM Write function falied");
         return MMA_ERROR;
    }

    return MMA_SUCCESS;
}



/*****************************************************************************
 函 数 名  : MMA_SyncMsClassType
 功能描述  : 将设置完成的手机类型写入NVIM中,
             并将当前值更新为新设入值,同时清
             除设置标志
 输入参数  : VOS_UINT16        ClientId,
             VOS_UINT8               OpId,
             TAF_PH_MS_CLASS_TYPE MsClass
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年9月7日
    作    者   : liuyang
    修改内容   : 新生成函数

  2.日    期   : 2006年3月8日
    作    者   : liuyang id:48197
    修改内容   : 问题单号：A32D02424，增加上报后全局变量清理
*****************************************************************************/
VOS_VOID MMA_SyncMsClassType(VOS_UINT16        ClientId,
                             VOS_UINT8               OpId,
                             TAF_PH_MS_CLASS_TYPE MsClass)
{
    /*
    1.MMA收到TAFM的模式设置请求，按照其中的接入模式参数更新MMA的全局变量
    2.如果TAFM设置值与MMA原值不相同，则MMA将新值存储到NVIM中
    3.通过Taf_ParaSetResult接口向TAFM上报设置成功
   */

    /*判断更改参数是否成功*/
    if (MsClass != gstMmaValue.stSetMsClass.NewMsClassType)
    {
        /*不成功，向上上报设置参数错误*/
        Taf_ParaSetResult(ClientId, OpId, TAF_PARA_UNSPECIFIED_ERROR, TAF_PH_MS_CLASS_PARA, VOS_NULL_PTR);
        /* BEGIN: Added by liuyang id:48197, 2006/3/8   PN:A32D02424*/
        /*更新全局变量的值*/
        MMA_UpdateMsClassTypeGolbalValue(MsClass);
        /* END:   Added by liuyang id:48197, 2006/3/8 */
        return;
    }

    /*更新全局变量的值*/
    MMA_UpdateMsClassTypeGolbalValue(MsClass);

    /*写入NVIM，还未获得分配的item，临时使用定义*/
    if (MMA_SUCCESS != MMA_WriteValue2Nvim(en_NV_Item_MMA_MsClass,
                                           &(MsClass),
                                           sizeof(TAF_PH_MS_CLASS_TYPE)))
    {
        TAF_PRINT(TAF_TAFM, ERROR_PRINT, "MMA_SyncMsClassType:NV_Write runs failed");

        Taf_ParaSetResult(ClientId, OpId, TAF_PARA_WRITE_NVIM_ERROR, TAF_PH_MS_CLASS_PARA, VOS_NULL_PTR);
        return;
    }
    /*成功，向上上报参数设置成功*/
    Taf_ParaSetResult(ClientId, OpId, TAF_PARA_OK, TAF_PH_MS_CLASS_PARA, VOS_NULL_PTR);
    return;
}

/*****************************************************************************
 函 数 名  : MMA_SyncMsClassTypeGolbalValue
 功能描述  : 更新全局变量gstMmaValue中有关手机类型的参数
 输入参数  : TAF_PH_MS_CLASS_TYPE MsClass
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年9月24日
    作    者   : liuyang
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MMA_UpdateMsClassTypeGolbalValue(TAF_PH_MS_CLASS_TYPE MsClass)
{
    /*更改设置参数标志位，同步全局参数*/
    gstMmaValue.stSetMsClass.enSetMsClassTypeflag           = MMA_SET_MSCLASSTYPE_NO;
    gstMmaValue.pg_StatusContext->ucModeService             = MsClass;
    gstMmaValue.stSetMsClass.MsClassType                    = MsClass;
    gstMmaValue.stSetMsClass.NewMsClassType                 = MsClass;
    gstMmaValue.stSetMsClass.enSetMsClassTypeDeAttFlg       = MMA_SET_CLASSTYPE_DE_ATTACH_ENABLE;
    return;
}
VOS_VOID MMA_PhoneAttach(VOS_UINT16 ClientId,
                         VOS_UINT8 OpId,
                         TAF_PH_ATTACH_TYPE AttachType)
{
    VOS_UINT8                   ucTi;
    VOS_UINT32                  ulRet;
    TAF_PHONE_EVENT             PhoneEvt;


    /* 如果是C+L模式 */
     if (VOS_TRUE == TAF_MMA_IsPowerOnCLInterWork())
     {
        /*此时是AT触发了attach，则回复失败,进行保护 */
        MMA_WARNINGLOG("MMA_PhoneAttach():WARNING:NOW, It is cL mode now!");
        MMA_HandleEventError(ClientId, OpId, TAF_ERR_PHONE_ATTACH_FORBIDDEN, TAF_PH_EVT_ATTACH_CNF);
        return;
     }

    if ( MMA_SET_MSCLASSTYPE_YES == gstMmaValue.stSetMsClass.enSetMsClassTypeflag)
    {
        if ( MMA_SET_CLASSTYPE_ATTACH_ENABLE != gstMmaValue.stSetMsClass.enSetMsClassTypeDeAttFlg)
        {
            MMA_WARNINGLOG("MMA_PhoneAttach():WARNING:NOW, It is Setting the MsClassType!");
            MMA_HandleEventError(ClientId, OpId, TAF_ERR_PHONE_ATTACH_FORBIDDEN, TAF_PH_EVT_ATTACH_CNF);
            return;
        }
    }

    switch ( AttachType)
    {
        case TAF_PH_PS_OPERATE :
            PhoneEvt = TAF_PH_EVT_PS_ATTACH_CNF;
            break;
        case TAF_PH_CS_OPERATE :
            PhoneEvt = TAF_PH_EVT_CS_ATTACH_CNF;
            break;
        case TAF_PH_PS_CS_OPERATE :
            PhoneEvt = TAF_PH_EVT_ATTACH_CNF;
            break;

        default:
            MMA_WARNINGLOG("MMA_PhoneAttach():WARNING:AttachType Unknow!");
            MMA_HandleEventError(ClientId, OpId, TAF_ERR_PARA_ERROR, TAF_PH_EVT_ATTACH_CNF);
            return;
    }

    /*获取Ti*/
    ulRet = MMA_TiAlloc(TAF_MMA, ClientId, OpId, PhoneEvt, &ucTi);
    if (MMA_FAILURE == ulRet )
    {
        MMA_WARNINGLOG("MMA_PhoneAttach():WARNING:MMA_TiAlloc failed");
        MMA_HandleEventError(ClientId, OpId, TAF_ERR_TI_ALLOC_FAIL, PhoneEvt);

        MN_MMA_SetMsClassTypeDeAttFlg();

        return;
    }

    /*调用STATUS模块，启动过程*/
    ulRet = Sta_Attach (ucTi, AttachType, TAF_MMA_ATTACH_REASON_INITIAL);
    if ( STA_ERROR == ulRet )
    {
        MMA_WARNINGLOG("MMA_PhoneAttach():WARNING:Sta_Attach failed");
        /*==>A32D10733
         * Error has been reported to TAF inside Sta_Attach. Here, no need to report again.
         */

        /*MMA_HandleEventError(ClientId, OpId, TAF_ERR_PHONE_ATTACH_FORBIDDEN, PhoneEvt);*/
    }

    MN_MMA_SetMsClassTypeDeAttFlg();

    return;
}
VOS_VOID MMA_PhoneDetach(VOS_UINT16         ClientId,
                         VOS_UINT8                OpId,
                         TAF_PH_DETACH_TYPE    DetachType,
                         TAF_MMA_DETACH_CAUSE_ENUM_UINT8     enDetachCause)
{
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    VOS_UINT8                           ucCtxIndex;
    VOS_UINT8       ucTi;
    VOS_UINT32      ulRet;
    TAF_PHONE_EVENT PhoneEvt;
    VOS_UINT8       ucDetachType;

    if (VOS_TRUE != TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_DETACH_REQ, &ucCtxIndex))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "MMA_PhoneDetach ERROR! NOT FIND DETACH REQ");

        return;
    }

    pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();

    /* BEGIN: Added by liuyang, 2005/10/26 */
    if ( MMA_SET_MSCLASSTYPE_YES == gstMmaValue.stSetMsClass.enSetMsClassTypeflag)
    {
        if ( MMA_SET_CLASSTYPE_DETACH_ENABLE != gstMmaValue.stSetMsClass.enSetMsClassTypeDeAttFlg)
        {
            MMA_WARNINGLOG("MMA_PhoneDetach():WARNING:NOW, It is Setting the MsClassType!");


            /*清除当前Ti全局变量中对应的记录*/
            MMA_CleanTiVar(ClientId, OpId);
            TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                                 TAF_MMA_APP_OPER_RESULT_FAILURE,
                                 TAF_ERR_PHONE_DETACH_FORBIDDEN);
            TAF_MMA_ClearOperCtx(ucCtxIndex);

            return;
        }
    }

    /* 启动PS域和CS域DETACH过程 */
    switch ( DetachType)
    {
        case TAF_PH_PS_OPERATE :
            ucDetachType    = MMA_PS_DETACH;
            PhoneEvt        = TAF_PH_EVT_PS_DETACH_CNF;
            break;
        case TAF_PH_CS_OPERATE :
            ucDetachType    = MMA_CS_DETACH;
            PhoneEvt        = TAF_PH_EVT_CS_DETACH_CNF;
            break;
        case TAF_PH_PS_CS_OPERATE:
            ucDetachType    = MMA_PS_CS_DETACH;
            PhoneEvt        = TAF_PH_EVT_DETACH_CNF;
            break;

        default:
            PhoneEvt        = TAF_PH_EVT_DETACH_CNF;
            MMA_WARNINGLOG("MMA_PhoneDetach():WARNING:DetachType Error!");


            /*清除当前Ti全局变量中对应的记录*/
            MMA_CleanTiVar(ClientId, OpId);
            TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                                     TAF_MMA_APP_OPER_RESULT_FAILURE,
                                     TAF_ERR_PHONE_MSG_UNMATCH);

            TAF_MMA_ClearOperCtx(ucCtxIndex);

            return;
    }

    /* 启动PS域和CS域DETACH过程 */
    /*获取Ti*/
    ulRet = MMA_TiAlloc(TAF_MMA, ClientId, OpId, PhoneEvt, &ucTi);
    if ( MMA_FAILURE == ulRet )
    {
        MMA_WARNINGLOG("MMA_PhoneDetach():WARNING:MMA_TiAlloc failed!");


        /*清除当前Ti全局变量中对应的记录*/
        MMA_CleanTiVar(ClientId, OpId);
        TAF_MMA_SndDetachCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                             TAF_MMA_APP_OPER_RESULT_FAILURE,
                             TAF_ERR_TI_ALLOC_FAIL);

        TAF_MMA_ClearOperCtx(ucCtxIndex);

        MN_MMA_SetMsClassTypeDeAttFlg();

        return;
    }

    /*调用STATUS启动DETACH过程*/
    ulRet = Sta_Detach (ucTi, ucDetachType, enDetachCause);
    if ( STA_ERROR == ulRet )
    {
        MMA_WARNINGLOG("MMA_PhoneDetach():WARNING:invoke Sta_Detach() return error!");
        /*==>A32D10733
         * Error has been reported to TAF inside Sta_Attach. Here, no need to report again.
         */
        /*MMA_HandleEventError(ClientId, OpId, TAF_ERR_PHONE_DETACH_FORBIDDEN, PhoneEvt);*/
    }

    MN_MMA_SetMsClassTypeDeAttFlg();

    return;
}
VOS_VOID MMA_ImsiToImsiStr(VOS_CHAR  *pcImsi)
{
    VOS_UINT32                          i;

    VOS_UINT8                          *pucImsi = VOS_NULL_PTR;

    pucImsi = TAF_SDC_GetSimImsi();

    for (i = 1; i <9; i++)
    {
        if (1 == i)
        {
            *pcImsi++ = ((pucImsi[i] >> 4) & 0x0f) + 0x30;
        }
        else
        {
            *pcImsi++ = (pucImsi[i] & 0x0f) + 0x30;
            *pcImsi++ = ((pucImsi[i] >> 4) & 0x0f) + 0x30;
        }
    }

}
VOS_UINT32 MMA_GetCardLockCfgData(VOS_VOID)
{
    TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU          stSimLockPlmnInfo;
    TAF_CUSTOM_CARDLOCK_STATUS_STRU             stCardLockStatus;
    TAF_CUSTOM_SIM_LOCK_MAX_TIMES_STRU          stSimLockMaxTimes;
    VOS_UINT32                                  ulResult;


    PS_MEM_SET(&stSimLockPlmnInfo, 0x00, sizeof(stSimLockPlmnInfo));
    PS_MEM_SET(&stCardLockStatus,  0x00, sizeof(stCardLockStatus));
    PS_MEM_SET(&stSimLockMaxTimes, 0x00, sizeof(stSimLockMaxTimes));

    ulResult = NV_Read(en_NV_Item_CardlockStatus,
                       &stCardLockStatus,
                       (VOS_UINT32)(sizeof(stCardLockStatus)));
    if (NV_OK != ulResult)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_WARNING, "MMA_GetCardLockCfgData:WARNING: Not read sim lock status");
        return VOS_ERR;
    }
    g_MmaNewValue.stCardLockStatus = stCardLockStatus;

    ulResult = NV_Read(en_NV_Item_CustomizeSimLockMaxTimes,
                       &stSimLockMaxTimes,
                       sizeof(stSimLockMaxTimes));
    if (NV_OK != ulResult)
    {
        g_MmaNewValue.stSimLockMaxTimes.ulStatus = VOS_FALSE;
        g_MmaNewValue.stSimLockMaxTimes.ulLockMaxTimes = TAF_PH_CARDLOCK_DEFAULT_MAXTIME;
    }
    else
    {
        g_MmaNewValue.stSimLockMaxTimes = stSimLockMaxTimes;
    }

    ulResult = NV_Read(en_NV_Item_CustomizeSimLockPlmnInfo,
                       &stSimLockPlmnInfo,
                       sizeof(stSimLockPlmnInfo));
    if (NV_OK != ulResult)
    {
        g_MmaNewValue.stSimLockPlmnInfo.ulStatus = VOS_FALSE;
        PS_MEM_SET(g_MmaNewValue.stSimLockPlmnInfo.astSimLockPlmnRange, (VOS_CHAR)0xaa,
                       sizeof(g_MmaNewValue.stSimLockPlmnInfo.astSimLockPlmnRange));
    }
    else
    {
        g_MmaNewValue.stSimLockPlmnInfo = stSimLockPlmnInfo;
    }

    return VOS_OK;
}

VOS_UINT32 Taf_GetSimLockStatus(VOS_BOOL *pbActiveFlg)
{

    TAF_CUSTOM_CARDLOCK_STATUS_STRU             stCardLockStatus;
    VOS_UINT32                                  ulResult;


    PS_MEM_SET(&stCardLockStatus, 0x00, sizeof(stCardLockStatus));

    ulResult = NV_Read(en_NV_Item_CardlockStatus,
                       &stCardLockStatus,
                       (VOS_UINT32)(sizeof(stCardLockStatus)));

    if (NV_OK != ulResult)
    {
        MN_WARN_LOG("Taf_GetSimLockStatus:WARNING:  read sim lock status fail");
        return VOS_ERR;
    }

    if ((VOS_TRUE == MMA_IsSimLockPlmnInfoValid())
     && (NV_ITEM_ACTIVE == stCardLockStatus.ulStatus)
     && (TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE != stCardLockStatus.enCardlockStatus))
    {
        *pbActiveFlg = VOS_TRUE;
    }
    else
    {
        *pbActiveFlg = VOS_FALSE;

    }

    return VOS_OK;
}

VOS_UINT32 MMA_IsImsiIllegal(VOS_VOID)
{
    VOS_UINT32                          i,j;
    VOS_UINT32                          ulRslt = VOS_FALSE;
    VOS_UINT8                           aucTmpImsi[20];
    VOS_INT                             CmpRslt1,CmpRslt2;

    VOS_UINT8                          *pucImsi = VOS_NULL_PTR;

    pucImsi = TAF_SDC_GetSimImsi();


    if (VOS_FALSE == g_MmaNewValue.stSimLockPlmnInfo.ulStatus)
    {
        return VOS_FALSE;
    }

    /* 判定当前的IMSI是否和NV项的匹配 */
    for ( i = 0; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++ )
    {
        if (0xAA == g_MmaNewValue.stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin[0])
        {
            continue;
        }

        VOS_MemSet(aucTmpImsi, 0x00, 20);
        for (j=0; j < 8; j++)
        {
            if (j < 7)
            {
                aucTmpImsi[j] = (VOS_UINT8)((pucImsi[j + 1] & 0xf0) + ((pucImsi[j + 2]) & 0x0f));
            }
            else
            {
                aucTmpImsi[j] = (VOS_UINT8)((pucImsi[j + 1] & 0xf0) + 0x0f);
            }

            if ((0x0f == (g_MmaNewValue.stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin[j] & 0x0f))
             && (0xff != g_MmaNewValue.stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin[j]))
            {
                aucTmpImsi[j] = (VOS_UINT8)((aucTmpImsi[j] & 0xf0) + 0x0f);
                j++;
                break;
            }


            if (0xff == g_MmaNewValue.stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin[j])
            {
                break;
            }

        }

        CmpRslt1 = VOS_MemCmp(aucTmpImsi,
                              g_MmaNewValue.stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin,
                              j);

        CmpRslt2 = VOS_MemCmp(aucTmpImsi,
                              g_MmaNewValue.stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeEnd,
                              j);

        if ((CmpRslt1>=0) && (CmpRslt2<=0))
        {
            ulRslt = VOS_FALSE;
            break;
        }
        else
        {
            ulRslt = VOS_TRUE;
        }
    }

    return ulRslt;
}
VOS_UINT32 MMA_IsEygptVdfImsiIllegal()
{
    VOS_UINT32                          i;
    VOS_CHAR                            acTmpImsi[16] = {0};
    VOS_CHAR                            *pacEygptVdf[]={"50503", "60202", "26202",
                                                        "20205", "21670", "27201",
                                                        "22210", "20404", "53001",
                                                        "26801", "21401", "23415",
                                                        "27801", "22601", "27602",
                                                        "54201", "23003", "20601",
                                                        "65501", "20810", "23201",
                                                        "24008", "21910", "23801",
                                                        "27403", "24405", "29340",
                                                        "41902", "42602", "28001",
                                                        "24802", "27001", "45406",
                                                        "52503", "28401", "23211",
                                                        "24421"};

    MMA_ImsiToImsiStr(acTmpImsi);
    for (i = 0; i < 37; i++)
    {
        if (0 == VOS_MemCmp(acTmpImsi, pacEygptVdf[i], 5))
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 MMA_IsNorwayNetcomImsiIllegal()
{
    VOS_CHAR                            acTmpImsi[16] = {0};
    VOS_CHAR                            *pcNorwayNetcomImsi = "24202";

    /*根据NETCOM的需求IMSI range: 24202xy ,Where x = 0-9 and y = 0-3,或者xy = 59,86我们只要判断
    IMSI的网络号和国家号为24202,并且y的范围为0-3就可以了,因为
    x的范围是0-9,即任意范围,所以不用判断.
    */
    MMA_ImsiToImsiStr(acTmpImsi);

    if (0 == VOS_MemCmp(acTmpImsi, pcNorwayNetcomImsi, 5))
    {
        if (((acTmpImsi[5] == 0x35) && (acTmpImsi[6] == 0x39) )
         || ((acTmpImsi[5] == 0x38) && (acTmpImsi[6] == 0x36) ) )
        {
            return VOS_FALSE;
        }

        if ((acTmpImsi[6] >= 0x30) && (acTmpImsi[6] <= 0x33))
        {
            return VOS_FALSE;
        }

    }

    return VOS_TRUE;

}


VOS_UINT32 MMA_IsMexicoTelcelIllegal()
{
    VOS_CHAR                            acTmpImsi[16] = {0};
    VOS_CHAR                            *pcMexicoTelcelImsi="334020";

    /* 墨西哥telce PLMN ID 334020 */
    MMA_ImsiToImsiStr(acTmpImsi);
    if (0 == VOS_MemCmp(acTmpImsi, pcMexicoTelcelImsi, 6))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32 MMA_IsDominicaTelcelIllegal()
{
    VOS_CHAR                            acTmpImsi[16] = {0};
    VOS_CHAR                            *pcDominicaTelcelImsi1="37002";
    VOS_CHAR                            *pcDominicaTelcelImsi2="42502";
    VOS_CHAR                            *pcDominicaTelcelImsi3="338070";
    VOS_INT32                           iImsiCmpRslt1,iImsiCmpRslt2,iImsiCmpRslt3;

    /*dominica permanet SIM Lock 37002, 42502, 338070*/
    MMA_ImsiToImsiStr(acTmpImsi);
    iImsiCmpRslt1 = VOS_MemCmp(acTmpImsi, pcDominicaTelcelImsi1, 5);
    iImsiCmpRslt2 = VOS_MemCmp(acTmpImsi, pcDominicaTelcelImsi2, 5);
    iImsiCmpRslt3 = VOS_MemCmp(acTmpImsi, pcDominicaTelcelImsi3, 6);
    if ((0 == iImsiCmpRslt1)
      ||(0 == iImsiCmpRslt2)
      ||(0 == iImsiCmpRslt3))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 Mma_VerifyPhonePersonalization(VOS_VOID)
{
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 enVerifySimlockResult;
    SC_ERROR_CODE_ENUM_UINT32           enScResult;
    VOS_UINT32                          ulResult;
    MODEM_ID_ENUM_UINT16                enModemId;

    enModemId  = VOS_GetModemIDFromPid(WUEPS_PID_MMA);
    enScResult = SC_PERS_VerifySimLock(&enVerifySimlockResult, enModemId);

    /* 调用SC接口进行锁卡校验，若函数执行失败，则按照SIM卡被锁定处理 */
    if (SC_ERROR_CODE_NO_ERROR != enScResult)
    {
        return VOS_TRUE;
    }

    /* 根据校验结果返回SIM卡是否被锁 */
    switch (enVerifySimlockResult)
    {
        case SC_VERIFY_SIMLOCK_RESULT_READY:
            ulResult = VOS_FALSE;
            break;

        case SC_VERIFY_SIMLOCK_RESULT_PH_NET_PIN:
        case SC_VERIFY_SIMLOCK_RESULT_PH_NET_PUK:
        case SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PIN:
        case SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PUK:
        case SC_VERIFY_SIMLOCK_RESULT_PH_SP_PIN:
        case SC_VERIFY_SIMLOCK_RESULT_PH_SP_PUK:
            ulResult = VOS_TRUE;
            break;

        default:
            /* 无效值，按照SIM卡被锁进行处理 */
            ulResult = VOS_TRUE;
            break;
    }

    return ulResult;
}
VOS_BOOL Mma_IsImsiLocked(VOS_VOID)
{
    VOS_UINT32                          ulResult;
    MMA_CUSTOM_CARDLOCK_OPERATOR_STRU   stCardLockOp;
    MMA_CUSTOM_CARDLOCK_PERM_STRU       stCardPerm;
    VOS_CHAR                            acTmpImsi[16] = {0};
    VOS_CHAR                            *pcLabImsi1="00101";
    VOS_CHAR                            *pcLabImsi2="00102";
    VOS_CHAR                            *pcLabImsi3="00201";
    VOS_INT32                           iImsiCmpRslt1,iImsiCmpRslt2,iImsiCmpRslt3;
    VOS_UINT32                          ulChkRslt;
    VOS_UINT32                          ulLength;

    ulLength        = 0;
    stCardLockOp.ucStatus           = VOS_FALSE;
    stCardLockOp.enCardLockOperator = MMA_CUSTOM_CARDLOCK_BUTT;

    stCardPerm.ucStatus             = VOS_FALSE;
    stCardPerm.ucCardLockPerm       = VOS_FALSE;

    /*当IMSI的值为00101,00102,00201时，不进行锁卡判定*/
    VOS_MemSet(acTmpImsi, 16, 0);
    MMA_ImsiToImsiStr(acTmpImsi);

    iImsiCmpRslt1 = VOS_MemCmp(acTmpImsi, pcLabImsi1, 5);
    iImsiCmpRslt2 = VOS_MemCmp(acTmpImsi, pcLabImsi2, 5);
    iImsiCmpRslt3 = VOS_MemCmp(acTmpImsi, pcLabImsi3, 5);

    if ((0 == iImsiCmpRslt1)
      ||(0 == iImsiCmpRslt2)
      ||(0 == iImsiCmpRslt3))
    {
        return VOS_FALSE;
    }

    /*
    根据产品线的要求，如果CardLock这个NV项读取失败，则认为应该是卡被锁定，
    剩余次数为0(这种情况很少出现)
    */
    if (VOS_OK!= MMA_GetCardLockCfgData())
    {
        g_MmaNewValue.stCardLockStatus.ulStatus = VOS_TRUE;
        g_MmaNewValue.stCardLockStatus.enCardlockStatus = TAF_OPERATOR_LOCK_LOCKED;
        g_MmaNewValue.stCardLockStatus.ulRemainUnlockTimes = 0;
        return VOS_TRUE;
    }


    if (VOS_TRUE != g_MmaNewValue.stCardLockStatus.ulStatus)
    {
        return VOS_FALSE;
    }

    NV_GetLength(en_NV_Item_HUAWEI_CARDLOCK_OPERATOR_EXT, &ulLength);

    ulResult = NV_Read(en_NV_Item_HUAWEI_CARDLOCK_OPERATOR_EXT,
                       &stCardLockOp,
                       ulLength);
    if (NV_OK != ulResult)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_WARNING, "Mma_IsImsiLocked:WARNING: Read en_NV_Item_HUAWEI_CARDLOCK_PERM_EXT Err");
        return VOS_FALSE;
    }

    if (VOS_TRUE == stCardLockOp.ucStatus)
    {
        switch (stCardLockOp.enCardLockOperator)
        {
            case MMA_CUSTOM_CARDLOCK_EGYPT_VDF:
                ulChkRslt = MMA_IsEygptVdfImsiIllegal();
                break;

            case MMA_CUSTOM_CARDLOCK_NORWAY_NETCOM:
                ulChkRslt = MMA_IsNorwayNetcomImsiIllegal();
                break;

            case MMA_CUSTOM_CARDLOCK_MEXICO_TELCEL:
                ulChkRslt = MMA_IsMexicoTelcelIllegal();
                break;

            case MMA_CUSTOM_CARDLOCK_DOMINICA_TELCEL:
                ulChkRslt = MMA_IsDominicaTelcelIllegal();
                break;

            default:
                ulChkRslt = MMA_IsImsiIllegal();
                break;
        }

        if (VOS_TRUE == ulChkRslt)
        {
            /*如果永久锁卡选项激活，则将卡置为永久锁定状态*/
            NV_GetLength(en_NV_Item_HUAWEI_CARDLOCK_PERM_EXT, &ulLength);

            ulResult = NV_Read(en_NV_Item_HUAWEI_CARDLOCK_PERM_EXT,
                               &stCardPerm,
                               ulLength);
            if ((NV_OK == ulResult)
              &&(VOS_TRUE == stCardPerm.ucStatus)
              &&(VOS_TRUE == stCardPerm.ucCardLockPerm))
            {
                g_MmaNewValue.stCardLockStatus.enCardlockStatus = TAF_OPERATOR_LOCK_LOCKED;
                ulResult = NV_Write(en_NV_Item_CardlockStatus,
                                    &g_MmaNewValue.stCardLockStatus,
                                    sizeof(g_MmaNewValue.stCardLockStatus));
                if (NV_OK != ulResult)
                {
                     MMA_ERRORLOG("Mma_IsImsiLocked:ERROR:write en_NV_Item_Me_Personalisation_Active_Flag Fail.");
                     return VOS_TRUE;
                }

                ulResult = NV_SpecialNvIdBackup(en_NV_Item_CardlockStatus,
                                                &g_MmaNewValue.stCardLockStatus,
                                                sizeof(g_MmaNewValue.stCardLockStatus));

                if (NV_OK != ulResult)
                {
                    MMA_ERRORLOG("Mma_IsImsiLocked:ERROR:write special en_NV_Item_CardlockStatus Fail.");
                    return VOS_TRUE;
                }

            }
            else
            {
                /*如果之前已经解锁了，根据cardlock的要求，不再要求校验*/
                /*如果用户在任意一次输入正确之后，密码将会被永久性清除，
                用户可以任意使用该数据卡，即使更换了另一张非指定卡，
                这时也不再需要重新输入密码。*/
                if (TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE == g_MmaNewValue.stCardLockStatus.enCardlockStatus )
                {
                    return VOS_FALSE;
                }
            }

        }
    }
    else
    {
        /*如果之前已经解锁了，根据cardlock的要求，不再要求校验*/
        /*如果用户在任意一次输入正确之后，密码将会被永久性清除，
        用户可以任意使用该数据卡，即使更换了另一张非指定卡，
        这时也不再需要重新输入密码。*/
        if (TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE == g_MmaNewValue.stCardLockStatus.enCardlockStatus )
        {
            return VOS_FALSE;
        }
        ulChkRslt = MMA_IsImsiIllegal();
    }

    return ulChkRslt;


}

/*<==A32D06630*/

VOS_UINT32 MMA_CheckMePersonalisationStatus()
{
    VOS_UINT32 ulResult = VOS_FALSE;

    if (MMA_QUICK_START_ENABLE == gstMmaValue.ulQuickStartFlg)
    {
        MMA_INFOLOG("MMA_CheckMePersonalisationStatus Info:Usim status is ROMSIM!");
        return VOS_FALSE;
    }
    if (g_stMmaMePersonalisationStatus.SimLockStatus == MMA_SIM_IS_UNLOCK)
    {
        /* 判断是哪种上层应用，从而根据形态分别进行AP-Modem和非AP-Modem的锁网锁卡校验 */
        if (SYSTEM_APP_ANDROID == g_usMmaAppConfigSupportType)
        {
            /* AP-Modem模式下，获取当前是否锁卡 */
            ulResult = Mma_VerifyPhonePersonalization();
        }
        else
        {
            ulResult = Mma_IsImsiLocked();
        }

        if (VOS_FALSE == ulResult)
        {
            g_stMmaMePersonalisationStatus.SimLockStatus = MMA_SIM_LOCK_IS_VERIFY;
            MMA_INFOLOG("MMA_CheckMePersonalisationStatus Info:Usim is not locked!");
        }
        else
        {
            g_stMmaMePersonalisationStatus.SimLockStatus = (VOS_UINT8)ulResult;
            MMA_INFOLOG("MMA_CheckMePersonalisationStatus Info:Usim is locked!");
        }

    }
    else if (g_stMmaMePersonalisationStatus.SimLockStatus == MMA_SIM_IS_LOCK)
    {
        MMA_INFOLOG("MMA_CheckMePersonalisationStatus Info:Usim is locked!");
        return VOS_TRUE;
    }
    else
    {

    }

    return ulResult;

}




VOS_VOID MMA_PhoneStopEventReport ( VOS_UINT16 usClientID, VOS_UINT32 ulOpID, VOS_UINT32 ulEventType )
{
    TAF_PHONE_EVENT_INFO_STRU   *pEvent;

    pEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pEvent)
    {
        MMA_ERRORLOG("MMA_PhoneStopEventReport:ERROR:ALLOC MEMORY ERROR!");
        return;
    }
    PS_MEM_SET(pEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));
    /* 清除结构变量标志 */
    pEvent->OP_CsServiceStatus    = 0;
    pEvent->OP_PhoneError         = 0;
    pEvent->OP_PlmnList           = 0;
    pEvent->OP_PlmnReselMode      = 0;
    pEvent->OP_PsServiceStatus    = 0;
    pEvent->OP_RegPlmn            = 0;
    pEvent->OP_Spare              = 0;
    /*增加上报事件选项初始化*/
    pEvent->OP_CurRegSta          = 0;
    pEvent->OP_Rssi               = 0;
    pEvent->OP_BatteryPower       = 0;
    pEvent->OP_Pin                = 0;

    /* 填写上报事件参数 */
    /* 必填参数 */
    pEvent->ClientId = usClientID;
    pEvent->OpId = (VOS_UINT8)ulOpID;         /*写入ulOpId*/
    pEvent->PhoneEvent = (TAF_PHONE_EVENT)ulEventType;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(pEvent->ClientId,(VOS_UINT8*)pEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    PS_MEM_FREE(WUEPS_PID_MMA, pEvent);

    return;
}





VOS_VOID  MMA_PhonePlmnList (VOS_UINT16               ClientId,
                             VOS_UINT8                      OpId)
{
    VOS_UINT8 ucTi;
    VOS_UINT8 ucError = TAF_ERR_NO_ERROR;

    ucError = MMA_CheckUsimStatusForPlmnSel();
    if(TAF_ERR_NO_ERROR != ucError)
    {
        MMA_WARNINGLOG("MMA_PhonePlmnList():WARNING:UsimStatus Error!");
        /*错误处理*/
        MMA_HandleEventError(ClientId, OpId, ucError, TAF_PH_EVT_ERR);
        return;
    }



    /* 如果是C+L模式，此时是AT触发了plmn list搜网 ，则回复失败,进行保护  */
    if (VOS_TRUE == TAF_MMA_IsPowerOnCLInterWork())
    {
        MMA_HandleEventError(ClientId, OpId, TAF_ERR_ERROR, TAF_PH_EVT_ERR);

        return;
    }


    /*1、获取TI
      2、调用Status程序启动可用PLMN搜索*/
    if (MMA_SUCCESS != MMA_TiAlloc(TAF_MMA, ClientId, OpId, TAF_PH_EVT_PLMN_LIST_CNF, &ucTi))
    {   /*Ti分配失败*/
        ucError = TAF_ERR_TI_ALLOC_FAIL;
        MMA_WARNINGLOG("MMA_PhonePlmnList():WARNING:ALLOC TI FAILED!");
    }
    else
    {
        Sta_PlmnList(ucTi);
        return;
    }

    /*错误处理*/
    MMA_HandleEventError(ClientId, OpId, ucError, TAF_PH_EVT_ERR);
    return;
}


/*****************************************************************************
 函 数 名  : MMA_PlmnListAbort
 功能描述  : 停止PLMN搜索，通过操作结果回调函数上报返回结果
 输入参数  : VOS_UINT16    ClientId  客户端程序调用id
             VOS_UINT8    OpId             操作ID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005-08-25
    作    者   : liuyang
    修改内容   : V100R001版本已经实现具体过程，当前只是将TAFM分配TI的功能合并入其中

*****************************************************************************/
VOS_VOID MMA_PlmnListAbort( VOS_UINT16    ClientId, VOS_UINT8    OpId )
{
    VOS_UINT8 ucTi;
    VOS_UINT8 ucError = TAF_ERR_NO_ERROR;

    /*1、获取TI
      2、调用Status程序中断可用PLMN搜索*/
    if (MMA_SUCCESS != MMA_TiAlloc(TAF_MMA, ClientId, OpId, TAF_PH_EVT_PLMN_LIST_ABORT_CNF, &ucTi))
    {   /*Ti分配失败*/
        ucError = TAF_ERR_TI_ALLOC_FAIL;
    }
    else
    {
        Sta_PlmnListAbort(ucTi);
    }

    /*错误处理*/
    MMA_HandleEventError(ClientId, OpId, ucError, TAF_PH_EVT_PLMN_LIST_ABORT_CNF);
    return;
}


VOS_VOID MMA_PhonePlmnResel (VOS_UINT16                 ClientId,
                             VOS_UINT8                        OpId,
                             TAF_PLMN_RESEL_MODE_TYPE      ReselType)
{
    VOS_UINT8 ucTi;
    VOS_UINT8 ucError = TAF_ERR_NO_ERROR;

    ucError = MMA_CheckUsimStatusForPlmnSel();
    if(TAF_ERR_NO_ERROR != ucError)
    {
        MMA_WARNINGLOG("MMA_PhonePlmnResel():WARNING:UsimStatus Error!");
        /*错误处理*/
        MMA_HandleEventError(ClientId, OpId, ucError, TAF_PH_EVT_PLMN_RESEL_CNF);
        return;
    }


    /* 在CL模式时候，不支持AT进行选网操作,直接返回ERROR */
    if (VOS_TRUE == TAF_MMA_IsPowerOnCLInterWork())
    {
        MMA_HandleEventError(ClientId, OpId, TAF_ERR_ERROR, TAF_PH_EVT_PLMN_RESEL_CNF);

        return;
    }

    /*1、获取TI
      2、判断PLMN搜索采用类型
      3、调用相应的STATUS模块函数*/
    if (MMA_SUCCESS != MMA_TiAlloc(TAF_MMA, ClientId, OpId, TAF_PH_EVT_PLMN_RESEL_CNF, &ucTi))
    {   /*Ti分配失败*/
        ucError = TAF_ERR_TI_ALLOC_FAIL;
        MMA_WARNINGLOG("MMA_PhonePlmnResel():WARNING:ALLOC TI FAILED!");

    }
    else
    {
         /*手动选择PLMN*/
         if (  TAF_PLMN_RESELETION_MANUAL == ReselType )
         {
             Sta_PlmnReselectManual(ucTi);
         }
         /*自动选择PLMN*/
         else if ( TAF_PLMN_RESELETION_AUTO == ReselType)
         {
             Sta_PlmnReselectAuto(ucTi);
         }
         else
         {
             MMA_WARNINGLOG("MMA_PhonePlmnResel():WARNING:Error ReselType!");
             ucError = TAF_ERR_PARA_ERROR;
         }
    }

    /*错误处理*/
    MMA_HandleEventError(ClientId, OpId, ucError, TAF_PH_EVT_PLMN_RESEL_CNF);
    return;
}



VOS_VOID MMA_PhonePlmnUserSel (
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    TAF_PLMN_ID_STRU                    Plmn,
    TAF_PH_RAT_TYPE_ENUM_UINT8          enPhRat,
    TAF_PLMN_RESEL_MODE_TYPE            ReselMode
)
{
    VOS_UINT8                           ucTi;
    VOS_UINT8                           ucError;
    TAF_PH_RAT_TYPE_ENUM_UINT8          enPrioSelRat;

    ucError            = TAF_ERR_NO_ERROR;

    ucError = MMA_CheckUsimStatusForPlmnSel();
    if (TAF_ERR_NO_ERROR != ucError)
    {
        MMA_WARNINGLOG("MMA_PhonePlmnUserSel():WARNING:UsimStatus Error!");

        /*错误处理*/
        MMA_HandleEventError(ClientId, OpId, ucError, TAF_PH_EVT_PLMN_SEL_CNF);

        return;
    }

    /*增加ACCESSMODE选择*/
    if ((enPhRat > TAF_PH_RAT_BUTT)
     || (ReselMode > TAF_PLMN_RESELETION_MANUAL))
    {
        ucError = TAF_ERR_PARA_ERROR;
        MMA_WARNINGLOG("MMA_PhonePlmnUserSel():WARNING:ACCESS & RESELETION MODE UNKNOW!");

        /*错误处理*/
        MMA_HandleEventError(ClientId, OpId, ucError, TAF_PH_EVT_PLMN_SEL_CNF);

        return;
    }

    enPrioSelRat = MN_MMA_GetUserPlmnSelPrioRat(enPhRat, Plmn);

    /*1、获取TI
      2、调用Status程序启动可用PLMN搜索*/
    if (MMA_SUCCESS != MMA_TiAlloc(TAF_MMA, ClientId, OpId, TAF_PH_EVT_PLMN_SEL_CNF, &ucTi))
    {
        /*Ti分配失败*/
        ucError = TAF_ERR_TI_ALLOC_FAIL;
        MMA_WARNINGLOG("MMA_PhonePlmnUserSel():WARNING:ALLOC TI FAILED!");
    }
    else
    {
        Sta_PlmnSelect(ucTi, Plmn, enPrioSelRat, ReselMode);
    }

    /*错误处理*/
    MMA_HandleEventError(ClientId, OpId, ucError, TAF_PH_EVT_PLMN_SEL_CNF);

    return;
}



VOS_VOID MMA_HandleEventError(VOS_UINT16   ClientId,
                              VOS_UINT8          OpId,
                              VOS_UINT8       ucError,
                              TAF_PHONE_EVENT PhEvent)
{
    TAF_PHONE_EVENT_INFO_STRU *pstPhoneEvent;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MMA_HandleEventError:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    if (TAF_ERR_NO_ERROR != ucError)
    {
        /*初始化所有事件选项*/
        MMA_InitEventInfoOP(pstPhoneEvent);
        /*将错误上报事件置1*/
        pstPhoneEvent->OP_PhoneError = MMA_TRUE;

        pstPhoneEvent->PhoneEvent    = PhEvent;
        pstPhoneEvent->ClientId      = ClientId;
        pstPhoneEvent->OpId          = OpId;
        pstPhoneEvent->PhoneError    = ucError;

        /*清除当前Ti全局变量中对应的记录*/
        MMA_CleanTiVar(ClientId, OpId);

        /* 调用电话管理上报函数 */
        MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    }
    PS_MEM_FREE(WUEPS_PID_MMA,pstPhoneEvent);
    return;
}


VOS_VOID MMA_PhoneGetMfrId(VOS_UINT16    ClientId, VOS_UINT8   OpId)
{
    /* VOS_UINT8  *pucBuf; */
    VOS_UINT8   ucParalen;
    VOS_UINT8   *pPara = VOS_NULL_PTR;

    /*获取当前厂商代码实际长度*/
    ucParalen = (VOS_UINT8)VOS_StrLen((VOS_CHAR *)gstMmaValue.stMeInfo.FmrId.aucMfrId) + 1;
    if ( ucParalen > TAF_MAX_MFR_ID_LEN)
    {
        gstMmaValue.stMeInfo.FmrId.aucMfrId[TAF_MAX_MFR_ID_LEN] = '\0';
        /*增加警告打印*/
        MMA_WARNINGLOG("MMA_PhoneGetMfrId():WARNING:MfrId Unterminated!");
    }
    pPara = gstMmaValue.stMeInfo.FmrId.aucMfrId;

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_MFR_ID_PARA,TAF_ERR_NO_ERROR, pPara);

    return;
}




VOS_VOID MMA_PhoneGetModelId(VOS_UINT16           ClientId,
                             VOS_UINT8                  OpId)
{
    /*VOS_UINT8  *pucBuf;  */
    VOS_UINT8   ucParalen;
    /*VOS_UINT8   ucBufLen;  */
    VOS_UINT8   *pPara = VOS_NULL_PTR;

    /*获取当前模块ID实际长度*/
    ucParalen = (VOS_UINT8)VOS_StrLen((VOS_CHAR *)(gstMmaValue.stMeInfo.ModelId.aucModelId)) + 1;
    if ( ucParalen > TAF_MAX_MODEL_ID_LEN)
    {
        gstMmaValue.stMeInfo.ModelId.aucModelId[TAF_MAX_MODEL_ID_LEN] = '\0';
        MMA_WARNINGLOG("MMA_PhoneGetModelId():WARNING:ModelId Unterminated!");
    }

    pPara = gstMmaValue.stMeInfo.ModelId.aucModelId;

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_MODEL_ID_PARA,TAF_ERR_NO_ERROR, pPara);

    return;
}



VOS_VOID MMA_PhoneGetRevisionId(VOS_UINT16           ClientId,
                                VOS_UINT8                  OpId)
{
    /*VOS_UINT8   *pucBuf;  */
    VOS_UINT8    ucParalen;
    VOS_UINT8   *pPara = VOS_NULL_PTR;

    /*获取当前软件版本实际长度*/
    ucParalen = (VOS_UINT8)VOS_StrLen((VOS_CHAR *)(gstMmaValue.stMeInfo.SoftVersion.aucRevisionId)) + 1;
    if ( ucParalen > TAF_MAX_REVISION_ID_LEN)
    {
        gstMmaValue.stMeInfo.SoftVersion.aucRevisionId[TAF_MAX_REVISION_ID_LEN] = '\0';
        /*增加警告打印*/
        MMA_WARNINGLOG("MMA_PhoneGetRevisionId():WARNING:RevisionId Unterminated!");
    }

    pPara = gstMmaValue.stMeInfo.SoftVersion.aucRevisionId;

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_REVISION_ID_PARA, TAF_ERR_NO_ERROR,pPara);
    return;
}



VOS_VOID MMA_PhoneGetImsi(VOS_UINT16           ClientId,
                          VOS_UINT8                  OpId)
{
    TAF_UINT16                          usErrorCode;
    TAF_PH_IMSI_STRU                    stImsiTmp;
    VOS_UINT8                          *pPara = VOS_NULL_PTR;

    VOS_UINT8                          *pucImsi = VOS_NULL_PTR;

    pucImsi = TAF_SDC_GetSimImsi();

    /*查询USIM状态*/
    if ( MMA_ERROR == MMA_PhoneUsimReady(ClientId, OpId, TAF_PH_IMSI_ID_PARA) )
    {
        MMA_WARNINGLOG("MMA_PhoneGetImsi():WARNING:NO USIM");
        return;
    }

    switch ( MMA_CheckPin1SatusSimple() )
    {
        case MMA_ENABLE_PIN1_NONEED:
        case MMA_NONEED_PIN1:
            usErrorCode = TAF_ERR_NO_ERROR;
            break;
        case MMA_NEED_PIN1:
            usErrorCode = TAF_ERR_NEED_PIN1;
            break;
        case MMA_NEED_PUK1:
            usErrorCode = TAF_ERR_NEED_PUK1;
            break;
        case MMA_PUK1_REMAINS_ZERO:
            usErrorCode = TAF_ERR_USIM_SIM_INVALIDATION;
            break;
        case MMA_PIN1_STATUS_FAIL:
            usErrorCode = TAF_ERR_UNSPECIFIED_ERROR;
            break;
        default:
            usErrorCode = TAF_ERR_UNSPECIFIED_ERROR;
            break;
    }
    if ( TAF_ERR_NO_ERROR != usErrorCode )
    {
        /*错误处理*/
        MMA_ReportParaQueryError(ClientId, OpId,
                                 usErrorCode,
                                 TAF_PH_IMSI_ID_PARA);
        return;
    }

    if (USIMM_API_SUCCESS != NAS_USIMMAPI_GetCardIMSI(pucImsi))
    {
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_ERROR,
                                 TAF_PH_IMSI_ID_PARA);
        return;
    }


    if (pucImsi[0]>8)
    {
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_ERROR,
                                 TAF_PH_IMSI_ID_PARA);
        return;
    }
    MMA_ImsiBcd2Ascii(  (VOS_UINT8)(pucImsi[0] * 2),
                      &pucImsi[1],
                        stImsiTmp.aucImsi
                     );


    pPara = (VOS_UINT8 *)stImsiTmp.aucImsi;
    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_IMSI_ID_PARA, TAF_ERR_NO_ERROR,pPara);
    return;
}
VOS_UINT32 MMA_PhoneUsimReady( VOS_UINT16           ClientId,
                               VOS_UINT8                  OpId,
                               TAF_PARA_TYPE     QueryType )
{
    VOS_UINT8               ucUsimStatus;


    ucUsimStatus = USIMM_CARD_SERVIC_ABSENT;

    /*获取当前USIM状态*/
    if  ( MMA_SUCCESS != MMA_GetUsimStatusFromUsim(&ucUsimStatus, VOS_NULL_PTR))
    {
        MMA_WARNINGLOG("MMA_PhoneUsimReady():WORNING:Invoke Taf_UsimmGetStatus fail.");
        /*CLIENTID为无效值时,其余都有有效*/
        if ( MMA_CLIENTID_NONE != ClientId )
        {
            MMA_ReportParaQueryError(ClientId, OpId,
                                     TAF_ERR_SIM_BUSY,
                                     QueryType);
        }
        return MMA_ERROR;
    }

    /*当调用USIMAPI的值不为0,则表示有错误*/
    if ( USIMM_CARD_SERVIC_ABSENT == ucUsimStatus )
    {
        /*打印消息*/
        MMA_NORMAILLOG("MMA_PhoneUsimReady():NORMAL:USIM status is Not Ready.");
        /*生成错误数据包,*/
        /*CLIENTID为无效值时,其余都有有效*/
        if ( MMA_CLIENTID_NONE != ClientId )
        {
            MMA_ReportParaQueryError(ClientId, OpId,
                                     TAF_ERR_USIM_SIM_CARD_NOTEXIST,
                                     QueryType);
        }
        /*返回获取数据无效*/
        return MMA_ERROR;
    }

    return MMA_SUCCESS;
}


VOS_VOID  MMA_PhoneGetRegStatus(VOS_UINT16      ClientId,
                                VOS_UINT8             OpId)
{
    TAF_PH_REG_STATE_STRU      stRegStatusTmp;

    /*获取当前最新的注册信息*/
    MMA_UpdateRegInfo(&stRegStatusTmp);

    /*使用数据上报函数将注册状态数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_CS_REG_STATE_PARA, TAF_ERR_NO_ERROR,(VOS_VOID*)&stRegStatusTmp);

    return;
}



VOS_VOID MMA_PhoneGetGmmRegStatus( VOS_UINT16    ClientId, VOS_UINT8      OpId )
{
    TAF_PH_REG_STATE_STRU     stRegStatusTmp;

    /*获取当前最新的注册信息*/
    MMA_UpdateRegInfo(&stRegStatusTmp);


    /*使用数据上报函数将注册状态数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_PS_REG_STATE_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stRegStatusTmp);

    return;
}

#if(FEATURE_ON == FEATURE_LTE)

VOS_VOID NAS_MMA_GetEpsRegStatus(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_PH_REG_STATE_STRU               stRegStatusTmp;

    /*获取当前最新的注册信息*/
    MMA_UpdateRegInfo(&stRegStatusTmp);

    /*使用数据上报函数将注册状态数据传至TAFm*/
    MMA_ParaQueryReport(usClientId, ucOpId, TAF_PH_EPS_REG_STATE_PARA,
                        TAF_ERR_NO_ERROR, (VOS_VOID*)&stRegStatusTmp);

    return;


}

#endif



VOS_VOID NAS_MMA_JudgeServiceStatus(TAF_PHONE_EVENT_INFO_STRU *pstPhoneEvent)
{
    TAF_SDC_REPORT_SRVSTA_ENUM_UINT8    enServiceStatus;

    enServiceStatus = TAF_SDC_GetServiceStatus();

    if (TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE != enServiceStatus)
    {
        if (TAF_SDC_REPORT_SRVSTA_NO_SERVICE != enServiceStatus)
        {
            if ((TAF_PH_REG_REGISTERED_HOME_NETWORK == pstPhoneEvent->CurRegSta.RegState)
             || (TAF_PH_REG_REGISTERED_ROAM == pstPhoneEvent->CurRegSta.RegState))
            {
                pstPhoneEvent->CurRegSta.RegState   = TAF_PH_REG_NOT_REGISTERED_SEARCHING;
                pstPhoneEvent->CurRegSta.OP_CellId  = MMA_FALSE;
            }

            if ((TAF_PH_REG_REGISTERED_HOME_NETWORK == pstPhoneEvent->CurRegSta.ucPsRegState)
             || (TAF_PH_REG_REGISTERED_ROAM == pstPhoneEvent->CurRegSta.ucPsRegState))
            {
                pstPhoneEvent->CurRegSta.ucPsRegState = TAF_PH_REG_NOT_REGISTERED_SEARCHING;
                pstPhoneEvent->CurRegSta.OP_CellId  = MMA_FALSE;
            }
        }
        else
        {
            pstPhoneEvent->CurRegSta.RegState = TAF_PH_REG_NOT_REGISTERED_NOT_SEARCH;
            pstPhoneEvent->CurRegSta.ucPsRegState = TAF_PH_REG_NOT_REGISTERED_NOT_SEARCH;
            pstPhoneEvent->CurRegSta.OP_CellId  = MMA_FALSE;
        }
    }

    return;

}



VOS_VOID TAF_MMA_SndStkLocationInfoInd (VOS_VOID)
{
    NAS_STK_LOCATION_INFO_IND_STRU     *pstMsg        = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;
    TAF_SDC_PLMN_ID_STRU               *pstTempPlmnID = VOS_NULL_PTR;

    /* 如果支持SVLTE但是该modem不支持CS域，则直接退出，不向STK发送该消息 */
    if (VOS_TRUE == TAF_SDC_GetSvlteSupportFlag())
    {
       if (VOS_TRUE != TAF_MMA_IsCsServDomainAvail(gstMmaValue.stSetMsClass.MsClassType))
       {
           return;
       }
    }

    /* 申请内存 */
    ulLength = sizeof(NAS_STK_LOCATION_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg   = (NAS_STK_LOCATION_INFO_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA, ulLength);
    if (VOS_NULL_PTR == pstMsg)
    {
        MN_ERR_LOG("TAF_MMA_SndStkLocationInfoInd: Alloc Msg");
        return;
    }

    PS_MEM_SET(&(pstMsg->stSysInfo), 0, sizeof(NAS_STK_SYS_INFO_STRU));

    PS_MEM_SET(&(pstMsg->stServiceStatus), 0, sizeof(NAS_STK_SERVICE_STATUS_INFO_STRU));

    /* 填写新消息内容 */
    pstMsg->ulMsgName            = ID_NAS_STK_LOCATION_INFO_IND;

    pstMsg->ulReceiverPid        = NAS_GetOamReceivePid(MAPS_STK_PID);

    pstMsg->stSysInfo.enRat      = TAF_MMA_ConvertMmaRatToStkRat(TAF_SDC_GetSysMode());

    pstMsg->stSysInfo.ulCellId   = TAF_SDC_GetCurrCampCellId();

    pstMsg->stSysInfo.usLac      = TAF_SDC_GetCurrCampLac();

    pstTempPlmnID                = TAF_SDC_GetCurrCampPlmnId();
    pstMsg->stSysInfo.stPlmn.Mcc = pstTempPlmnID->ulMcc;
    pstMsg->stSysInfo.stPlmn.Mnc = pstTempPlmnID->ulMnc;

    pstMsg->stSysInfo.enUtranMode = NAS_STK_UTRAN_MODE_BUTT;
    if (TAF_PH_RAT_WCDMA == pstMsg->stSysInfo.enRat)
    {
        pstMsg->stSysInfo.enUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();
    }

    pstMsg->stServiceStatus.enCsServiceStatus = TAF_SDC_GetCsServiceStatus();

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG( WUEPS_PID_MMA, pstMsg))
    {
        MN_ERR_LOG("TAF_MMA_SndStkLocationInfoInd: PS_SEND_MSG ERROR");
    }

    return;

}

#if (FEATURE_MULTI_MODEM == FEATURE_ON)


VOS_VOID TAF_MMA_SndMtcEplmnInfoInd(
    MMC_MMA_EPLMN_INFO_IND_STRU        *pstEplmnInfoIndMsg
)
{
    MMA_MTC_EPLMN_INFO_IND_STRU        *pstMmaSndMtcEplmnInfoInd = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;

    pstMmaSndMtcEplmnInfoInd = (MMA_MTC_EPLMN_INFO_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                              WUEPS_PID_MMA,
                                              sizeof(MMA_MTC_EPLMN_INFO_IND_STRU));

    if( VOS_NULL_PTR == pstMmaSndMtcEplmnInfoInd )
    {
        MN_ERR_LOG("TAF_MMA_SndMtcEplmnInfoInd: Alloc Msg Failed!");
        return;
    }

    PS_MEM_SET(((VOS_UINT8 *)pstMmaSndMtcEplmnInfoInd + VOS_MSG_HEAD_LENGTH),
               0x00,
               (sizeof(MMA_MTC_EPLMN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH));

    pstMmaSndMtcEplmnInfoInd->stMsgHeader.ulReceiverPid   = UEPS_PID_MTC;
    pstMmaSndMtcEplmnInfoInd->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstMmaSndMtcEplmnInfoInd->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMmaSndMtcEplmnInfoInd->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMmaSndMtcEplmnInfoInd->stMsgHeader.ulMsgName       = ID_MMA_MTC_EPLMN_INFO_IND;

    pstMmaSndMtcEplmnInfoInd->stEplmnInfo.ucEquPlmnNum    = pstEplmnInfoIndMsg->stEplmnInfo.ucEquPlmnNum;
    PS_MEM_CPY(pstMmaSndMtcEplmnInfoInd->stEplmnInfo.astEquPlmnAddr, pstEplmnInfoIndMsg->stEplmnInfo.astEquPlmnAddr,
               sizeof(pstMmaSndMtcEplmnInfoInd->stEplmnInfo.astEquPlmnAddr));

    ulResult = PS_SEND_MSG(WUEPS_PID_MMA, pstMmaSndMtcEplmnInfoInd);

    if( VOS_OK != ulResult )
    {
        MN_ERR_LOG("TAF_MMA_SndMtcEplmnInfoInd: Send Msg Failed!");
    }

    return;
}


#endif


VOS_UINT32 TAF_MMA_IsCellChange(MMC_MMA_SYS_INFO_IND_STRU *pstCurrSysInfo)
{
    TAF_SDC_PLMN_ID_STRU               *pstOldPlmnId = VOS_NULL_PTR;
    VOS_UINT16                          stOldLac;
    VOS_UINT8                           stOldRac;
    VOS_UINT32                          stOldCellId;    

    pstOldPlmnId = TAF_SDC_GetCurrCampPlmnId();
    stOldLac     = TAF_SDC_GetCurrCampLac();
    stOldRac     = TAF_SDC_GetCurrCampRac();
    stOldCellId  = TAF_SDC_GetCurrCampCellId();

    if ((pstOldPlmnId->ulMcc != pstCurrSysInfo->stPlmnId.ulMcc)
     || (pstOldPlmnId->ulMnc != pstCurrSysInfo->stPlmnId.ulMnc)
     || (stOldLac            != pstCurrSysInfo->usLac)
     || (stOldRac            != pstCurrSysInfo->ucRac)
     || (stOldCellId         != pstCurrSysInfo->stCellId.aulCellId[0]))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_VOID TAF_MMA_ProcMmcSysInfoInNormalService(
    VOS_UINT32                          ulCellChangeFlg
)
{
    if ((VOS_TRUE == NAS_MMA_CellIdChangeForStkLocSta())
     || (TAF_REPORT_SRVSTA_NORMAL_SERVICE != g_stMmsStkLocStaSysInfo.enServiceStatus))
    {
        NAS_MMA_ReportLociStatus();
    }

    if ((TAF_SDC_SYS_MODE_LTE != TAF_SDC_GetSysMode())
     && (VOS_TRUE                 == ulCellChangeFlg))
    {
        if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == TAF_SDC_GetPsServiceStatus())
        {
            TAF_MMA_ReportRegStatus(TAF_SDC_GetPsRegStatus(), MMA_MMC_SRVDOMAIN_PS);
        }

        if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == TAF_SDC_GetCsServiceStatus())
        {
             TAF_MMA_ReportRegStatus(TAF_SDC_GetCsRegStatus(), MMA_MMC_SRVDOMAIN_CS);
        }
    }

    return;
}

VOS_UINT32 TAF_MMA_RcvMmcSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulCellChangeFlg;    
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8                       enCsServiceStauts;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8                       enPsServiceStatus;
    NAS_MML_MISCELLANEOUS_CFG_INFO_STRU                    *pstMiscellaneousCfgInfo = VOS_NULL_PTR;
    MMC_MMA_SYS_INFO_IND_STRU                              *pstMmcSysInfo           = VOS_NULL_PTR;

#if ( (FEATURE_ON == FEATURE_IMS) || (FEATURE_ON == FEATURE_CL_INTERWORK) )


    TAF_SDC_CAMP_PLMN_INFO_STRU                             stOldCampPlmnInfo;
    VOS_UINT32                                              ulIsClInterWorkFlg;
    TAF_SDC_CAMP_PLMN_INFO_STRU                            *pstCurCampPlmnInfo     = VOS_NULL_PTR;

    ulIsClInterWorkFlg = TAF_MMA_IsPowerOnCLInterWork();

    pstCurCampPlmnInfo = TAF_SDC_GetCampPlmnInfo();

    PS_MEM_CPY(&stOldCampPlmnInfo, pstCurCampPlmnInfo, sizeof(TAF_SDC_CAMP_PLMN_INFO_STRU));
#endif

    pstMmcSysInfo           = (MMC_MMA_SYS_INFO_IND_STRU*)pstMsg;
    pstMiscellaneousCfgInfo = NAS_MML_GetMiscellaneousCfgInfo();

    enCsServiceStauts  = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus  = TAF_SDC_GetPsServiceStatus();

    TAF_MMA_PlmnIdReport((TAF_PLMN_ID_STRU*)&(pstMmcSysInfo->stPlmnId));

    /* 检查是否需要上报^MODE */
    Mma_ModeChgReport((MMA_MMC_NET_RAT_TYPE_ENUM_UINT8)pstMmcSysInfo->enCurNetWork, pstMmcSysInfo->ucSysSubMode);

    /* 只有小区ID改变时，才能更新RAT类型。此类型是用来SYSINFO查询时使用 */
    TAF_SDC_SetSysMode(pstMmcSysInfo->enCurNetWork);
    TAF_SDC_SetSysSubMode(pstMmcSysInfo->ucSysSubMode);

    TAF_SDC_SetRoamFlag(pstMmcSysInfo->ucRoamFlag);

    ulCellChangeFlg = TAF_MMA_IsCellChange(pstMmcSysInfo);

    TAF_SDC_SetCsAttachAllowFlg(pstMmcSysInfo->ucCsAttachAllowFlag);
    TAF_SDC_SetPsAttachAllowFlg(pstMmcSysInfo->ucPsAttachAllowFlag);

    /*更新当前全局变量中的PLMN*/
    TAF_SDC_SetCurrCampPlmnId((TAF_SDC_PLMN_ID_STRU *)&pstMmcSysInfo->stPlmnId);
    TAF_SDC_SetCurrCampLac(pstMmcSysInfo->usLac);
    TAF_SDC_SetCurrCampRac(pstMmcSysInfo->ucRac);
    TAF_SDC_SetCurrCampCellId(pstMmcSysInfo->stCellId.aulCellId[0]);

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    TAF_MMA_SndMtcSysInfo(pstMmcSysInfo->ucIsFobbiddenPlmnFlag,
                          pstMmcSysInfo->ucPsSupportFlg);
#endif

#if (FEATURE_ON == FEATURE_CL_INTERWORK)

    /* 驻留时判断CGI信息发生改变时通知CMMCA模块 */
    if ( (VOS_TRUE == TAF_MMA_IsCGIInfoChanged(&stOldCampPlmnInfo, TAF_SDC_GetCampPlmnInfo()))
      && (VOS_TRUE == ulIsClInterWorkFlg) )
    {
        TAF_MMA_SndCmmcaSysInfoInd(pstMmcSysInfo->stPlmnId.ulMcc,
                          pstMmcSysInfo->stPlmnId.ulMnc,
                          pstMmcSysInfo->usArfcn,
                          pstMmcSysInfo->enPrioClass);
    }
#endif

    if ( SYSTEM_APP_WEBUI == pstMiscellaneousCfgInfo->usAppConfigSupportFlg)
    {
        /* 判断是否需要对应用的漫游状态进行更新 */
        MN_PH_AppRoamStatusRpt();
    }



    if ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enPsServiceStatus)
     || (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enCsServiceStauts))
    {
        TAF_MMA_ProcMmcSysInfoInNormalService(ulCellChangeFlg);
    }

    TAF_MMA_SndStkLocationInfoInd();

    /* 当前在L模，则通知AT模块当前的接入技术 */
    if (NAS_MML_NET_RAT_TYPE_LTE == pstMmcSysInfo->enCurNetWork)
    {
        /*在L下不使用该全局变量记录接入禁止情况，清空成非接入禁止
        回到GU下时如果接入禁止发生改变MMC会重新上报*/
        TAF_MMA_InitCellAcInfo();
    }


#if (FEATURE_ON == FEATURE_IMS)
    if (VOS_FALSE == TAF_SDC_GetImsSupportFlag())
    {
        return VOS_TRUE;
    }

    /* 接入技术变化时通知SPM,触发SPM处理缓存 */
    if (TAF_SDC_GetSysMode() != stOldCampPlmnInfo.enSysMode)
    {
        TAF_MMA_SndSpmRatChangeNotify();
    }

    /* 驻留时判断CGI信息发生改变时通知ISMA模块,触发IMSA模块通知IMS协议栈 */
    if (VOS_TRUE == TAF_MMA_IsCGIInfoChanged(&stOldCampPlmnInfo, TAF_SDC_GetCampPlmnInfo()))
    {
        TAF_MMA_SndImsaCampInfoChangeInd();
    }
#endif

    return VOS_TRUE;
}





VOS_VOID MMA_GetRssiLevelFromValue( VOS_INT32      lRssiValue, VOS_UINT8    *pucRssiLevel )
{
    if (lRssiValue >= -51)
    {
        *pucRssiLevel = 31;
    }
    else if (lRssiValue <= MMA_RSSI_LOWER_LIMIT)
    {
        *pucRssiLevel = TAF_PH_RSSIZERO;
    }
    else
    {
        *pucRssiLevel = (VOS_UINT8)(((VOS_UINT32)(lRssiValue - MMA_RSSI_LOWER_LIMIT )) >> 1);
    }

    return;
}



VOS_VOID MMA_InitEventInfoOP(TAF_PHONE_EVENT_INFO_STRU *pstEvent)
{
    /*填充所有选项为0*/
    pstEvent->OP_BatteryPower    = MMA_FALSE;
    pstEvent->OP_CsServiceStatus = MMA_FALSE;
    pstEvent->OP_CurRegSta       = MMA_FALSE;
    pstEvent->OP_PhoneError      = MMA_FALSE;
    pstEvent->OP_PlmnList        = MMA_FALSE;
    pstEvent->OP_PlmnReselMode   = MMA_FALSE;
    pstEvent->OP_PsServiceStatus = MMA_FALSE;
    pstEvent->OP_RegPlmn         = MMA_FALSE;
    pstEvent->OP_Rssi            = MMA_FALSE;
    pstEvent->OP_OperMode        = MMA_FALSE;
    pstEvent->OP_Pin             = MMA_FALSE;
    pstEvent->OP_Spare           = MMA_FALSE;
    pstEvent->OP_PinRemain       = MMA_FALSE;
    pstEvent->OP_Mode            = MMA_FALSE;
    pstEvent->OP_Srvst           = MMA_FALSE;
    pstEvent->OP_UsimRestrictAccess = MMA_FALSE;
    pstEvent->OP_UsimPNN         = MMA_FALSE;
    pstEvent->OP_UsimOPL         = MMA_FALSE;
    pstEvent->Op_CellAcInfo      = MMA_FALSE;
    pstEvent->OP_PsInitRslt     = MMA_FALSE;
    return;
}
/*****************************************************************************
 函 数 名  : MMA_PhonePinRemainTimeReport
 功能描述  :
 输入参数  :
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年9月30日
    作    者   : liuyang
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MMA_PhonePinRemainTimeReport(const TAF_PH_PIN_TIMECNF_STRU * pstPinCnf)
{
    TAF_PHONE_EVENT_INFO_STRU *pstPhoneEvent;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MMA_PhonePinRemainTimeReport:ERROR:ALLOC MEMORY FAIL.");
        return;
    }
    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));
    /*参数有效性判断*/
    if ( VOS_NULL_PTR == pstPinCnf)
    {
        MMA_WARNINGLOG("MMA_PinDataReport():WARNING:pPinCnf is NULL PTR!.");
        MMA_HandleEventError(gstMmaValue.stOpPinData.ClientId,
                             gstMmaValue.stOpPinData.OpId,
                             TAF_ERR_NULL_PTR,
                             TAF_PH_EVT_OP_PINREMAIN_CNF);
        PS_MEM_FREE(WUEPS_PID_MMA,pstPhoneEvent);
        return;
    }

    MMA_InitEventInfoOP(pstPhoneEvent);

    pstPhoneEvent->OP_PinRemain    = MMA_TRUE;
    pstPhoneEvent->PinRemainCnf    = *pstPinCnf;
    pstPhoneEvent->PhoneEvent      = TAF_PH_EVT_OP_PINREMAIN_CNF;
    Taf_PhoneEvent(pstPhoneEvent);
    PS_MEM_FREE(WUEPS_PID_MMA,pstPhoneEvent);
}
VOS_VOID MMA_UpdateRegInfo( TAF_PH_REG_STATE_STRU *pstCurRegInf )
{
    TAF_PLMN_ID_STRU                   *pstPlmnId = VOS_NULL_PTR;
    TAF_SDC_REG_STATUS_ENUM_UINT8       enCsRegStatus;
    TAF_SDC_REG_STATUS_ENUM_UINT8       enPsRegStatus;


    /*获取MM层注册状态*/
    enCsRegStatus = TAF_SDC_GetCsRegStatus();

    pstCurRegInf->RegState   = (TAF_PH_REG_STATE_TYPE)enCsRegStatus;

    /*获取PS注册状态*/
    enPsRegStatus = TAF_SDC_GetPsRegStatus();

    pstCurRegInf->ucPsRegState  = (TAF_PH_REG_STATE_TYPE)enPsRegStatus;

    /*获取当前PLMN*/
    pstPlmnId          = (TAF_PLMN_ID_STRU *)TAF_SDC_GetCurrCampPlmnId();
    pstCurRegInf->Plmn = *pstPlmnId;

    /*获取当前CI*/
    pstCurRegInf->CellId.aulCellId[0] = TAF_SDC_GetCurrCampCellId();
    pstCurRegInf->CellId.ucCellNum = 1;

    /*获取当前LAC*/
    pstCurRegInf->usLac  = TAF_SDC_GetCurrCampLac();

    /*获取当前RAC*/
    pstCurRegInf->ucRac     = TAF_SDC_GetCurrCampRac();


    /* 将MMC上报的系统子模式转为+CREG/+CGREG命令参数[ACT]中协议规定的接入技术值 */
    NAS_MMA_TransferSysSubMode2ActForAtCmdCreg(TAF_SDC_GetSysSubMode(), &(pstCurRegInf->ucAct));

    return;
}
VOS_VOID MMA_PhoneGetBatteryPower(VOS_UINT16     ClientId,
                                  VOS_UINT8            OpId)
{
    /*VOS_UINT8                 *pucBuf;  */
    /*VOS_UINT8                  ucParalen;  */
    TAF_PH_BATTERYPOWER_STRU   stBatteryStatus;
    /*VOS_UINT8                  aucBuf[128];  */

    /*pucBuf                    = aucBuf;  */

    TAF_PMU_STATE_STRU pmuState;

    PS_MEM_SET(&pmuState,0x00,sizeof(pmuState));

    DRV_GET_PMU_STATE(&pmuState);

    /************************************************************/
    /*从电源模块中获取当前所要信息*/
    if(1 == pmuState.bat_out)
    {
        stBatteryStatus.BatteryPowerStatus = TAF_PH_NOBATTERYFIXED;
    }
    else if(1 == pmuState.chg_timer_overflow)
    {
        stBatteryStatus.BatteryPowerStatus = TAF_PH_POWERSOURCEERROR;
    }
    else if(1 == pmuState.chargerstate)
    {
        stBatteryStatus.BatteryPowerStatus = TAF_PH_BATTERYNOSUPPLY;
    }
    else
    {
        stBatteryStatus.BatteryPowerStatus = TAF_PH_BATTERYCHARGING;
    }

    stBatteryStatus.BatteryRemains      = (VOS_UINT8)((pmuState.bal_Volt * 100) / 900);
    /************************************************************/

    /*使用数据上报函数将注册状态数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_BATTERY_POWER_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stBatteryStatus);

    return;
}


VOS_UINT32 TAF_MMA_GetPidFromClientId(
    MN_CLIENT_ID_T                      usClientId
)
{
    MN_CLIENT_ID_TYPE_UINT16            enClientIdType;

    enClientIdType = MN_GetClientIdType(usClientId);

    if (MN_CLIENT_ID_TYPE_AT == enClientIdType)
    {
        return WUEPS_PID_AT;
    }
    else if (CMMCA_CLIENT_ID == usClientId)
    {
        return WUEPS_PID_CMMCA;
    }
    else
    {
        return MN_GetPidFromClientId(usClientId);
    }
}


VOS_VOID MMA_PhModeReport(VOS_UINT16           ClientId,
                                   VOS_UINT8             OpId,
                                   TAF_PH_OP_MODE_CNF_STRU   stPhMode,
                                   TAF_PH_ERR_CODE       usErrorCode)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent = VOS_NULL_PTR;
    TAF_MMA_CTRL_STRU                   stCtrlInfo;
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt;
    VOS_UINT32                          ulRcvPid;

    enRslt = TAF_MMA_APP_OPER_RESULT_SUCCESS;

    if ( TAF_ERR_NO_ERROR != usErrorCode )
    {
        enRslt = TAF_MMA_APP_OPER_RESULT_FAILURE;
    }

    /* 初始化事件上报 */

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MMA_PhModeReport:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    /* 操作模式项置 */
    pstPhoneEvent->OP_OperMode = 1;
    pstPhoneEvent->PhoneEvent = TAF_PH_EVT_OPER_MODE_IND;

    /* 如果有错误填写错误码 */
    if ( TAF_ERR_NO_ERROR != usErrorCode )
    {
        pstPhoneEvent->OP_PhoneError = 1;
        pstPhoneEvent->PhoneError = usErrorCode;
        MMA_INFOLOG1("MMA_PhModeReport():INFO:Report Error code:",usErrorCode);
    }


    /* 填写上报事件OperMode结构*/
    pstPhoneEvent->OperMode = stPhMode;
    pstPhoneEvent->OpId     = OpId;

    if ( ( TAF_PH_CMD_SET == stPhMode.CmdType )
       &&( TAF_ERR_NO_ERROR == usErrorCode ) )
    {
        /* 设置成功事件广播 */
        pstPhoneEvent->ClientId = MMA_CLIENTID_BROADCAST;
        MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));
    }

    /* 设置结果上报 */
    if ( TAF_PH_CMD_SET == stPhMode.CmdType )
    {
        /* 当前为CMMCA模式，则上报给CMMCA模块 */
        ulRcvPid = TAF_MMA_GetPidFromClientId(ClientId);
        if ((WUEPS_PID_AT == ulRcvPid)
         || (WUEPS_PID_CMMCA == ulRcvPid))
        {
            TAF_MMA_BuildMmaCtrlInfo(ulRcvPid, ClientId, OpId, &stCtrlInfo);
            TAF_MMA_SndPhoneModeSetCnf(&stCtrlInfo,
                                       stPhMode.PhMode,
                                       enRslt,
                                       usErrorCode);
        }
        else if (VOS_PID_BUTT != ulRcvPid)
        {
            pstPhoneEvent->ClientId   = ClientId;
            pstPhoneEvent->PhoneEvent = TAF_PH_EVT_OPER_MODE_CNF;

            MN_PH_SendMsg(ClientId, (VOS_UINT8*)pstPhoneEvent, sizeof(TAF_PHONE_EVENT_INFO_STRU));
        }
        else
        {
        }

        PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

        return;

    }

    /* 查询事件上报调用客户端 */
    pstPhoneEvent->PhoneEvent = TAF_PH_EVT_OPER_MODE_CNF;
    pstPhoneEvent->ClientId   = ClientId;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
}

/* 函数MMA_PhModeQuery MMA_PhModeHandle MMA_PhModeSet移动位置 */



VOS_VOID MMA_ChangeSrv2Class(TAF_PH_SERVICE_DOMAIN    ucSrvDomain,
                                        TAF_PH_MS_CLASS_TYPE    *pucMsClass)
{
    switch(ucSrvDomain)
    {
        case TAF_PH_SERVICE_CS_PS:
            *pucMsClass = TAF_PH_MS_CLASS_A;
            break;
        case TAF_PH_SERVICE_PS:
            *pucMsClass = TAF_PH_MS_CLASS_CG;
            break;
        case TAF_PH_SERVICE_ANY:
        case TAF_PH_SERVICE_CS:
            *pucMsClass = TAF_PH_MS_CLASS_CC;
            break;
        default:
            *pucMsClass = TAF_PH_MS_CLASS_NULL;
            break;
    }
    return;
}

VOS_VOID MMA_ChangeClass2Srv(TAF_PH_SERVICE_DOMAIN     *pucSrvDomain,
                                        TAF_PH_MS_CLASS_TYPE      ucMsClass)
{
    switch(ucMsClass)
    {
        case TAF_PH_MS_CLASS_A:
        case TAF_PH_MS_CLASS_B:
            *pucSrvDomain = TAF_PH_SERVICE_CS_PS;
            break;
        case TAF_PH_MS_CLASS_CG:
            *pucSrvDomain = TAF_PH_SERVICE_PS;
            break;
        case TAF_PH_MS_CLASS_CC:
            *pucSrvDomain = TAF_PH_SERVICE_CS;
            break;
        case TAF_PH_MS_CLASS_ANY:
            *pucSrvDomain = TAF_PH_SERVICE_ANY;
            break;

        default:
            *pucSrvDomain = TAF_PH_SERVICE_NULL;
            break;
    }
    return;
}

VOS_VOID MMA_PhSysCfgSetSrvDomainCheck(TAF_PH_SERVICE_DOMAIN   ucSrvDomain,
                                       MMA_SYS_CFG_SET_FLG     *pusSetFlg,
                                       TAF_PH_ATTACH_TYPE      *pucAttachType,
                                       TAF_PH_DETACH_TYPE      *pucDetachType)
{
    TAF_PH_MS_CLASS_TYPE                    ucNewMsClass;
    TAF_PH_MS_CLASS_TYPE                    ucCurMsClass;
    MMA_CLASS_CHANGE_INDEX_ENUM             enClassChnageIndex;

    MMA_ChangeSrv2Class(ucSrvDomain, &ucNewMsClass);

    ucCurMsClass = gstMmaValue.stSetMsClass.MsClassType;

    gstMmaValue.stSetMsClass.NewMsClassType = ucNewMsClass;

    /*根据当前手机类型判断需要ATTACH还是DETACH过程*/
    enClassChnageIndex = (MMA_CLASS_CHANGE_INDEX_ENUM)(((ucCurMsClass << 4) & 0xf0)
                                                       | (ucNewMsClass & 0x0f));

    /* 服务域变化时，记录一下是否需要通知STK/MSG CS域的变化情况 */
    switch (enClassChnageIndex)
    {
        /*NULL->A*/
    case MMA_NULL2A:
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_ATTACH;
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_ATTACH;
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_ON;
        *pucAttachType = TAF_PH_PS_CS_OPERATE;
        *pucDetachType = TAF_PH_ATTACH_DETACH_NULL;
        break;

        /*CG->A*/
    case MMA_CG2A:

        /*NULL->CC*/
    case MMA_NULL2CC:
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_ATTACH;
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_ON;
        *pucAttachType = TAF_PH_CS_OPERATE;
        *pucDetachType = TAF_PH_ATTACH_DETACH_NULL;
        break;

        /*NULL->CG*/
    case MMA_NULL2CG:

        /*CC->A*/
    case MMA_CC2A:
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_ATTACH;
        *pucAttachType = TAF_PH_PS_OPERATE;
        *pucDetachType = TAF_PH_ATTACH_DETACH_NULL;
        break;

        /*只有DETACH情况的类型切换*/
        /*CC->NULL*/
    case MMA_CC2NULL:

        /*A->CG*/
    case MMA_A2CG:
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH;
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_OFF;
        *pucAttachType = TAF_PH_ATTACH_DETACH_NULL;
        *pucDetachType = TAF_PH_CS_OPERATE;
        break;

        /*CG->NULL*/
    case MMA_CG2NULL:

        /*A->CC*/
    case MMA_A2CC:
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH;
        *pucAttachType = TAF_PH_ATTACH_DETACH_NULL;
        *pucDetachType = TAF_PH_PS_OPERATE;
        break;

        /*A->NULL*/
    case MMA_A2NULL:
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH;
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH;
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_OFF;
        *pucAttachType = TAF_PH_ATTACH_DETACH_NULL;
        *pucDetachType = TAF_PH_PS_CS_OPERATE;
        break;

        /*CG->CC*/
    case MMA_CG2CC:
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH;
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_ATTACH;
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_ON;
        *pucAttachType = TAF_PH_CS_OPERATE;
        *pucDetachType = TAF_PH_PS_OPERATE;
        break;

        /*CC->CG*/
    case MMA_CC2CG:
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH;
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_ATTACH;
        *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_OFF;
        *pucAttachType = TAF_PH_PS_OPERATE;
        *pucDetachType = TAF_PH_CS_OPERATE;
        break;
    default:
        *pucAttachType = TAF_PH_ATTACH_DETACH_NULL;
        *pucDetachType = TAF_PH_ATTACH_DETACH_NULL;
        break;
    }
}
VOS_UINT32 MN_MMA_IsUserSettedRatOrderChanged(
    TAF_PH_RAT_ORDER_STRU              *pstUserSettedRatOrder,
    TAF_PH_RAT_ORDER_STRU              *pstOrigUserSettedRatOrder
)
{
    VOS_UINT32                          i;

    /* 个数不等 */
    if (pstUserSettedRatOrder->ucRatOrderNum != pstOrigUserSettedRatOrder->ucRatOrderNum)
    {
        return VOS_TRUE;
    }

    /* 比较优先级技术是否完全相等 */
    for (i = 0; i < pstUserSettedRatOrder->ucRatOrderNum; i++)
    {
        if (pstUserSettedRatOrder->aenRatOrder[i] != pstOrigUserSettedRatOrder->aenRatOrder[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;

}



VOS_UINT32 MN_MMA_IsUserSettedLBandChanged(
    TAF_USER_SET_PREF_BAND64            *pstLBand,
    TAF_USER_SET_PREF_BAND64            *pstOrigLBand

)
{
    /* 若LTE的频段指示为非TAF_PH_BAND_NO_CHANGE */
    if ( (TAF_PH_BAND_NO_CHANGE != pstLBand->ulBandLow)
      || (0                     != pstLBand->ulBandHigh) )
    {
        if ( (pstLBand->ulBandLow != pstOrigLBand->ulBandLow)
          || (pstLBand->ulBandHigh != pstOrigLBand->ulBandHigh) )
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}
VOS_UINT32 MN_MMA_IsGUDualMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
)
{
    VOS_UINT8                           isWModeExist;
    VOS_UINT8                           isGModeExist;
    VOS_UINT32                          i;

    isWModeExist = VOS_FALSE;
    isGModeExist = VOS_FALSE;

    for (i = 0; i < pstRatOrder->ucRatOrderNum; i++)
    {
        if (TAF_PH_RAT_GSM == pstRatOrder->aenRatOrder[i])
        {
            isGModeExist = VOS_TRUE;
        }
        else if(TAF_PH_RAT_WCDMA == pstRatOrder->aenRatOrder[i])
        {
            isWModeExist = VOS_TRUE;
        }
        else
        {
        }
    }

    if ((VOS_TRUE == isGModeExist) && (VOS_TRUE == isWModeExist))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 MN_MMA_IsGOnlyMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
)
{
    if ((1 == pstRatOrder->ucRatOrderNum)
     && (TAF_PH_RAT_GSM == pstRatOrder->aenRatOrder[0]))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 MN_MMA_IsWOnlyMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
)
{
    if ((1 == pstRatOrder->ucRatOrderNum)
     && (TAF_PH_RAT_WCDMA == pstRatOrder->aenRatOrder[0]))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 MN_MMA_IsLOnlyMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
)
{
    if ((1 == pstRatOrder->ucRatOrderNum)
     && (TAF_PH_RAT_LTE == pstRatOrder->aenRatOrder[0]))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}

#endif


VOS_VOID MN_MMA_Convert64BitBandTo32Bit(
    TAF_USER_SET_PREF_BAND64           *pstBand,
    VOS_UINT32                         *pulBand
)
{
    /* 只有WCDMA900和WCDMA1700两个频段超过32位需要进行转换,
       转换原则:WCDMA900从0x2000000000000转换为0x00020000
                WCDMA1700从0x4000000000000转换为0x00040000
       原GU低于32位的频段不存在0x00020000和0x00040000的值 */
    if ((0 != pstBand->ulBandHigh)
     || ( TAF_PH_BAND_ANY != pstBand->ulBandLow))
    {
        *pulBand = pstBand->ulBandHigh |
                        (pstBand->ulBandLow & ~TAF_PH_BAND_NEED_CHANGE_TO_64BIT);
    }
    else
    {
        *pulBand = pstBand->ulBandLow;
    }

    return;
}



VOS_UINT32  TAF_MMA_IsSysCfgSetValid(
    TAF_MMA_SYS_CFG_PARA_STRU           *pstSysCfgPara
)
{
    VOS_UINT32                          ulPlatformSuppFlg;
    VOS_UINT32                          i;

    /* 同时满足以下两个条件时，才认为用户设置的接入技术列表有效
        1)用户设置的接入技术在平台支持的接入技术能力范围内
        2)用户设置的频段有效 */

    /* 接入技术设置没了 */
    if (0 == pstSysCfgPara->stRatOrder.ucRatOrderNum)
    {
        return VOS_FALSE;
    }

    /* 判断用户设置的接入技术是否是平台支持的接入技术 */
    for (i = 0; i < pstSysCfgPara->stRatOrder.ucRatOrderNum; i++)
    {
        ulPlatformSuppFlg = TAF_MMA_IsPlatformSupportSpecUserRat(pstSysCfgPara->stRatOrder.aenRatOrder[i]);

        /* 如果不支持，需要删除，如果支持，继续检查下一个 */
        if (VOS_FALSE == ulPlatformSuppFlg)
        {
            return VOS_FALSE;
        }
    }

    /* 判断用户设置的频段是否有效 */
    if (VOS_FALSE == MN_MMA_IsUserSettedBandValid(pstSysCfgPara))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}



VOS_UINT32 MN_MMA_IsUserSettedGuBandChanged_SysCfg(
    TAF_USER_SET_PREF_BAND64            *pstGuBand,
    MMA_USER_BAND_SET_UN                *pstOrigGuBand

)
{
    MMA_USER_BAND_SET_UN                uGuBand;

    if ((TAF_PH_BAND_NO_CHANGE != pstGuBand->ulBandLow)
     || (0                     != pstGuBand->ulBandHigh))
    {
        MN_MMA_Convert64BitBandTo32Bit(pstGuBand, &uGuBand.ulPrefBand);
        MN_MMA_ConvertGUFrequencyBand(&uGuBand.ulPrefBand);

        if (uGuBand.ulPrefBand != pstOrigGuBand->ulPrefBand)
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_VOID MN_MMA_ConvertGUFrequencyBand(
    VOS_UINT32                         *pulPrefBand
)
{
    MN_MMA_SYSCFG_USER_SET_BAND_STRU   *pstUserSettedBand   = VOS_NULL_PTR;
    VOS_UINT32                          ulUserSettedPrefBand;

    pstUserSettedBand    = MN_MMA_GetSyscfgUserSetBandAddr();
    ulUserSettedPrefBand = *pulPrefBand;

    switch ( ulUserSettedPrefBand )
    {
        /* 用户设置的GU频段为不改变 */
        case TAF_PH_BAND_NO_CHANGE:
            *pulPrefBand     = pstUserSettedBand->uUserSetGuBand.ulPrefBand;
            break;

        /* 用户设置的GU频段为任何频段 */
        case TAF_PH_BAND_ANY:
            *pulPrefBand     &= gstMmaBandCapa.ulAllUeBand;
            break;

        default:
            *pulPrefBand     = ulUserSettedPrefBand;
            break;
    }

    /* 过滤无效频段 */
    *pulPrefBand &= gstMmaBandCapa.ulAllUeBand;

    return;
}

#if (FEATURE_ON == FEATURE_LTE)
VOS_VOID MN_MMA_ConvertLteFrequencyBand(
    TAF_USER_SET_PREF_BAND64           *pstLBand
)
{
    MN_MMA_SYSCFG_USER_SET_BAND_STRU   *pstUserSettedBand   = VOS_NULL_PTR;
    TAF_USER_SET_PREF_BAND64           *pstUeSupportLteBand = VOS_NULL_PTR;
    TAF_USER_SET_PREF_BAND64            stLBand;

    stLBand            = *pstLBand;
    pstUserSettedBand  = MN_MMA_GetSyscfgUserSetBandAddr();

    /* 用户设置的LTE频段为任何频段 */
    if ((MN_MMA_LTE_HIGH_BAND_ANY == stLBand.ulBandHigh)
     && (MN_MMA_LTE_LOW_BAND_ANY == stLBand.ulBandLow))
    {
        *pstLBand = gstMmaBandCapa.stUeSupportLteBand;
    }

    /* 用户设置的LTE频段为不改变 */
    else if((TAF_PH_BAND_NO_CHANGE == stLBand.ulBandLow)
         && (0 == stLBand.ulBandHigh))
    {
        *pstLBand = pstUserSettedBand->stUserSetLteBand;
    }
    else
    {
        *pstLBand = stLBand;
    }

    /* 过滤无效频段 */
    pstUeSupportLteBand   = MN_MMA_GetUeSupportLteBandAddr();
    pstLBand->ulBandLow  &= pstUeSupportLteBand->ulBandLow;
    pstLBand->ulBandHigh &= pstUeSupportLteBand->ulBandHigh;

    return;
}
#endif

VOS_UINT32  MN_MMA_IsUserSettedBandValid(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgPara
)
{
    MMA_USER_BAND_SET_UN                uGuBand;
#if(FEATURE_ON == FEATURE_LTE)
    TAF_USER_SET_PREF_BAND64           *pstUeSupportLteBand = VOS_NULL_PTR;
#endif

    /* 将GU频段 pstSysCfgPara->stGuBand 64位转换为32位的 */
    MN_MMA_Convert64BitBandTo32Bit(&pstSysCfgPara->stGuBand, &uGuBand.ulPrefBand);

    MN_MMA_ConvertGUFrequencyBand(&uGuBand.ulPrefBand);

#if (FEATURE_ON == FEATURE_LTE)
    /* 获取用户设置的LBand */
    MN_MMA_ConvertLteFrequencyBand(&pstSysCfgPara->stLBand);
#endif

    /* 单模或多模时,用户设置的GU和L的频段必须要与当前UE支持的GU和L的频段有重叠频段
       否则禁止用户设置频段下去;当UE支持的GU或L的频段无效时,不检查用户设置的频段 */
    if ((0 == (uGuBand.ulPrefBand & gstMmaBandCapa.ulUeGSptBand))
     && (0 != gstMmaBandCapa.ulUeGSptBand))
    {
        return VOS_FALSE;
    }

    if ((0 == (uGuBand.ulPrefBand & gstMmaBandCapa.ulUeWSptBand))
     && (0 != gstMmaBandCapa.ulUeWSptBand))
    {
        return VOS_FALSE;
    }

#if (FEATURE_ON == FEATURE_LTE)
    pstUeSupportLteBand = MN_MMA_GetUeSupportLteBandAddr();

    if ((0 == (pstSysCfgPara->stLBand.ulBandLow & pstUeSupportLteBand->ulBandLow))
     && (0 == (pstSysCfgPara->stLBand.ulBandHigh & pstUeSupportLteBand->ulBandHigh))
     && ((0 != pstUeSupportLteBand->ulBandLow)
      || (0 != pstUeSupportLteBand->ulBandHigh)))
    {
        return VOS_FALSE;
    }
#endif

    return VOS_TRUE;
}





VOS_VOID MN_MMA_SetSyscfg_PowerOffStatus(VOS_VOID)
{
    /* 更新NV */
    MMA_PhSysCfgWriteNvim();

    /* 上报设置结果 */

    TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_NO_ERROR);

    gstMmaValue.stSysCfgSet.usSetFlag = MMA_SYS_CFG_NONE_SET;

    return;

}



VOS_VOID MN_MMA_SetSyscfg_EnableStatus(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfg,
    TAF_MMA_SYSCFG_USER_ENUM_UINT8      enSyscfgUser
)
{
    VOS_UINT32                          ulNeedSendMmcSysCfgFlag;
    TAF_PH_MS_CLASS_TYPE                ucClassType;
    VOS_UINT16                          usSetFlg;
    VOS_UINT8                           ucAttachType;
    VOS_UINT8                           ucDetachType;

    MMA_MMC_ATTACH_TYPE_ENUM_UINT32     enMmcAttachType;
    MMA_MMC_DETACH_TYPE_ENUM_UINT32     enMmcDetachType;
    VOS_UINT32                          ulRet;

    TAF_CS_SERVICE_ENUM_UINT32          enCsState;

    ulNeedSendMmcSysCfgFlag = VOS_FALSE;
    usSetFlg                = gstMmaValue.stSysCfgSet.usSetFlag;
    ucAttachType            = gstMmaValue.stSysCfgSet.ucAttachType;
    ucDetachType            = gstMmaValue.stSysCfgSet.ucDetachType;

    /* 漫游设置,接入模式设置,频带设置参数有发生改变 */
    if (MMA_SYS_CFG_NONE_SET != (usSetFlg & MMA_SYS_CFG_SND_TO_MMC))
    {
        ulNeedSendMmcSysCfgFlag = VOS_TRUE;
    }

    /* 服务域参数设置有效 */
    if (MMA_SYS_CFG_SRV_DOMAIN_SET == (usSetFlg & MMA_SYS_CFG_SRV_DOMAIN_SET))
    {
        /* 服务域设置需要DETACH,先进行DETACH,如果不需要通知MMC SYSCFG参数,
           且无需detach后再attach的则可以立刻上报设置结果 */
        if ((MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH == (usSetFlg & MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH))
         || (MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH == (usSetFlg & MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH)))
        {
            if (VOS_OK == TAF_MMA_ConvertDetachTypeToMmc(ucDetachType, &enMmcDetachType))
            {
                /* 启动等待detach cnf回复的20s保护定时器 */
                ulRet = NAS_StartRelTimer(&g_stSyscfgWaitDetachCnfTimer.MmaHTimer,
                                          WUEPS_PID_MMA,
                                          TI_MN_MMA_SYSCFG_WAIT_DETACH_CNF_TIMER_LEN,
                                          TAF_MMA,
                                          TI_MN_MMA_SYSCFG_WAIT_DETACH_CNF,
                                          VOS_RELTIMER_NOLOOP );

                if( VOS_OK !=  ulRet)
                {
                    MN_ERR_LOG("MN_MMA_SetSyscfg_EnableStatus():ERROR:Start TI_MN_MMA_SYSCFG_WAIT_DETACH_CNF Timer Error!");
                }
                else
                {
                    g_stSyscfgWaitDetachCnfTimer.ucTimerStatus = MMA_TIMER_RUN;
                }

                TAF_MMA_SndMmcDetachReq(STA_OPID_INSIDE, enMmcDetachType, TAF_MMA_DETACH_CAUSE_NORMAL);
            }

        }
        else
        {

            /* 如果需要和MMC进行交互设置syscfg参数,attach放在设置结束后再做 */
            if ((VOS_OK == TAF_MMA_ConvertAttachTypeToMmc(gstMmaValue.stSysCfgSet.ucAttachType, &enMmcAttachType))
             && (VOS_FALSE == ulNeedSendMmcSysCfgFlag))
            {
                /*ATTACH消息需要先发送MODE CHANGE消息*/
                MMA_ChangeSrv2Class(gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucSrvDomain, &ucClassType);
                TAF_MMA_SndMmcModeChangeReq((MMA_MMC_MS_MODE_ENUM_UINT32)ucClassType);

                enCsState = TAF_MMA_GetCsStateFromSyscfgSetFlag(usSetFlg);
                if (TAF_CS_SERVICE_BUTT != enCsState)
                {
                    TAF_MMA_SndStkCsServiceChangeNotify(enCsState);
                    TAF_MMA_SndMsgCsServiceChangeNotify(enCsState);
                }

                /* 清除标识:置标识时没有判断是不是支持SVLTE，所以不支持SVLTE时，也需要统一清除 */
                TAF_MMA_ClearSyscfgCsServiceChangeFlag(usSetFlg);

                TAF_MMA_SndMmcAttachReq(STA_OPID_INSIDE, enMmcAttachType, TAF_MMA_ATTACH_REASON_INITIAL);
            }
        }
    }

    /* 需要通知MMC SYSCFG参数 */
    if (VOS_TRUE == ulNeedSendMmcSysCfgFlag)
    {
        /* 没有DETACH则直接通知，否则等待DETACH结果 */
        if (TAF_PH_ATTACH_DETACH_NULL == ucDetachType)
        {
            TAF_MMA_SndMmcSysCfgReq(pstSysCfg, usSetFlg);
            TAF_MMA_SetSyscfgUser(enSyscfgUser);
        }

        /* 记录当前状态 */
        g_StatusContext.ulPreFsmState = g_StatusContext.ulFsmState;

        /* 更新当前状态 */
        g_StatusContext.ulFsmState = STA_FSM_SYS_CFG_SET;
    }
    else
    {
        if (TAF_MMA_SYSCFG_USER_AT == enSyscfgUser)
        {
            /* 更新NV */
            MMA_PhSysCfgWriteNvim();
        }

        /* detach类型为空时,需要立刻向AT回SYSCFG结果,否则,等待DETACH结果
           或者20s保护定时器超时后再会结果 */
        if (TAF_PH_ATTACH_DETACH_NULL == ucDetachType) /*只需发送Detach 或Attach 消息*/
        {
            if (TAF_MMA_SYSCFG_USER_AT == enSyscfgUser)
            {
                /*上报设置结果*/

                TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_NO_ERROR);
            }

            if (TAF_PH_ATTACH_DETACH_NULL == ucAttachType) /*只需发送Detach 消息*/
            {
                MMA_ChangeSrv2Class(gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucSrvDomain, &ucClassType);

                TAF_MMA_SndMmcModeChangeReq((MMA_MMC_MS_MODE_ENUM_UINT32)ucClassType);

                enCsState = TAF_MMA_GetCsStateFromSyscfgSetFlag(usSetFlg);
                if (TAF_CS_SERVICE_BUTT != enCsState)
                {
                    TAF_MMA_SndStkCsServiceChangeNotify(enCsState);
                    TAF_MMA_SndMsgCsServiceChangeNotify(enCsState);
                }

                /* 清除标识:置标识时没有判断是不是支持SVLTE，所以不支持SVLTE时，也需要统一清除 */
                TAF_MMA_ClearSyscfgCsServiceChangeFlag(usSetFlg);


            }
            gstMmaValue.stSysCfgSet.usSetFlag = MMA_SYS_CFG_NONE_SET;
        }
        else  /* 不需发送Detach */
        {
            /*记录当前状态*/
            g_StatusContext.ulPreFsmState = g_StatusContext.ulFsmState;

            /*更新当前状态*/
            g_StatusContext.ulFsmState = STA_FSM_SYS_CFG_SET;
        }

    }

    return;
}
VOS_VOID  TAF_MMA_WritePrioAccessModeToNvim( VOS_VOID )
{
    NAS_MMA_NVIM_ACCESS_MODE_STRU        stAccessMode;
    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU *pstLastSyscfgSet    = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    pstLastSyscfgSet     = MN_MMA_GetLastSyscfgSetAddr();
    ulLength             = 0;

    NV_GetLength(en_NV_Item_MMA_AccessMode, &ulLength);
    if (ulLength > sizeof(NAS_MMA_NVIM_ACCESS_MODE_STRU))
    {
        return;
    }

    if (NV_OK != NV_Read(en_NV_Item_MMA_AccessMode ,
                         &stAccessMode,
                         ulLength))

    {
        MN_WARN_LOG("TAF_MMA_WritePrioAccessModeToNvim:Read:NV_Read runs failed");
        return ;
    }

    stAccessMode.aucAccessMode[1] = pstLastSyscfgSet->enPrioRat;
    if (MMA_SUCCESS != MMA_WriteValue2Nvim(en_NV_Item_MMA_AccessMode,
                                           &stAccessMode,
                                           (VOS_UINT16)ulLength))
    {
        MN_ERR_LOG("TAF_MMA_WritePrioAccessModeToNvim():WARNING:NVIM_Write en_NV_Item_MMA_AccessMode failed");
    }
}

#if (FEATURE_ON == FEATURE_LTE)


VOS_UINT32  MN_MMA_NeedDetachAttach_CampOnGUMode(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgPara
)
{
    VOS_UINT32                          ulPrefBand;
    VOS_UINT32                          ulNeedAttachDetachFlag;
    VOS_UINT32                          ulNewRatLteExistFlag;
    VOS_UINT32                          ulOldRatLteExistFlag;
    TAF_USER_SET_PREF_BAND64           *pstOldLteBand = VOS_NULL_PTR;

    TAF_SDC_SYS_MODE_ENUM_UINT8         enCurRatType;

    enCurRatType = TAF_SDC_GetSysMode();

    ulPrefBand             = 0;
    ulNeedAttachDetachFlag = VOS_FALSE;

    /* 将GU频段 pstSysCfgPara->stGuBand 64位转换为32位的 */
    MN_MMA_Convert64BitBandTo32Bit(&pstSysCfgPara->stGuBand, &ulPrefBand);
    MN_MMA_ConvertGUFrequencyBand(&ulPrefBand);

    /* 获取用户设置的LBand */
    MN_MMA_ConvertLteFrequencyBand(&pstSysCfgPara->stLBand);

    if ((TAF_SDC_SYS_MODE_WCDMA == enCurRatType)
     || (TAF_SDC_SYS_MODE_GSM == enCurRatType))
    {
        /* 当前驻留GU模，新设置的模式为L only */
        if ((VOS_TRUE == MN_MMA_IsLOnlyMode(&(pstSysCfgPara->stRatOrder)))
         || (0 == ulPrefBand))
        {
            ulNeedAttachDetachFlag = VOS_TRUE;

            return ulNeedAttachDetachFlag;
        }


        /* 当前驻留GU模，模式从支持L到没有L的场景,需要做Detach/Attach */

        pstOldLteBand = MN_MMA_GetUserSetLteBandAddr();

        if ((VOS_TRUE == MN_MMA_IsSpecRatSupported(TAF_PH_RAT_LTE))
         && ((0 != pstOldLteBand->ulBandHigh)
         ||  (0 != pstOldLteBand->ulBandLow)))
        {
            ulOldRatLteExistFlag = VOS_TRUE;
        }
        else
        {
            ulOldRatLteExistFlag = VOS_FALSE;
        }

        if (VOS_TRUE ==  MN_MMA_IsSpecRatInRatList(TAF_PH_RAT_LTE, &(pstSysCfgPara->stRatOrder))
         && ((0 != pstSysCfgPara->stLBand.ulBandHigh)
          || (0 != pstSysCfgPara->stLBand.ulBandLow)))
        {
            ulNewRatLteExistFlag = VOS_TRUE;
        }
        else
        {
            ulNewRatLteExistFlag = VOS_FALSE;
        }

        if ((VOS_TRUE == ulOldRatLteExistFlag)
         && (VOS_FALSE == ulNewRatLteExistFlag))
        {
            ulNeedAttachDetachFlag = VOS_TRUE;
        }

    }

    return ulNeedAttachDetachFlag;
}


VOS_UINT32  MN_MMA_NeedDetachAttach_CampOnLMode(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgPara
)
{
    VOS_UINT32                          ulDetachAttachFlag;
    VOS_UINT32                          ulOldRatOnlyLteFlag;
    VOS_UINT32                          ulNewRatOnlyLteFlag;
    VOS_UINT32                          ulPrefBand;
    MN_MMA_SYSCFG_USER_SET_BAND_STRU   *pstOldUserSetBand = VOS_NULL_PTR;

    TAF_SDC_SYS_MODE_ENUM_UINT8          enCurRatType;

    enCurRatType  = TAF_SDC_GetSysMode();

    ulDetachAttachFlag  = VOS_FALSE;

    /* 将GU频段 pstSysCfgPara->stGuBand 64位转换为32位的 */
    MN_MMA_Convert64BitBandTo32Bit(&pstSysCfgPara->stGuBand, &ulPrefBand);
    MN_MMA_ConvertGUFrequencyBand(&ulPrefBand);

    /* 获取用户设置的LBand */
    MN_MMA_ConvertLteFrequencyBand(&pstSysCfgPara->stLBand);

    pstOldUserSetBand = MN_MMA_GetSyscfgUserSetBandAddr();

    if (TAF_SDC_SYS_MODE_LTE == enCurRatType)
    {
        /* 目标模式设置为GU时，需要发起Detach/Attach */
        if ((VOS_FALSE == MN_MMA_IsSpecRatInRatList(TAF_PH_RAT_LTE, &(pstSysCfgPara->stRatOrder)))
         || ((0 == pstSysCfgPara->stLBand.ulBandHigh)
          && (0 == pstSysCfgPara->stLBand.ulBandLow)))
        {
            ulDetachAttachFlag = VOS_TRUE;

            return ulDetachAttachFlag;
        }


        /* 从Lonly到非L only时，或者从非Lonly到L only时,需要发起Detach/Attach */

        /* 老的设置是否LTE only */
        if ((VOS_TRUE == MN_MMA_IsLOnlyMode(MN_MMA_GetRatPrioListAddr()))
         || (0 == pstOldUserSetBand->uUserSetGuBand.ulPrefBand))
        {
            ulOldRatOnlyLteFlag = VOS_TRUE;
        }
        else
        {
            ulOldRatOnlyLteFlag = VOS_FALSE;
        }

        /* 新的设置是否LTE only */
        if ((VOS_TRUE == MN_MMA_IsLOnlyMode(&(pstSysCfgPara->stRatOrder)))
         || (0 == ulPrefBand))
        {
            ulNewRatOnlyLteFlag = VOS_TRUE;
        }
        else
        {
            ulNewRatOnlyLteFlag = VOS_FALSE;
        }

        if (ulOldRatOnlyLteFlag != ulNewRatOnlyLteFlag)
        {
            ulDetachAttachFlag = VOS_TRUE;
        }


    }

    return ulDetachAttachFlag;
}

VOS_VOID  MN_MMA_UpdateRatTrigedAttachPara(
    TAF_PH_MS_CLASS_TYPE                ucNewMsClassType,
    MMA_SYS_CFG_SET_FLG                *pusSetFlg,
    TAF_PH_ATTACH_TYPE                 *pucAttachType
)
{
    /* Attach操作与当前的服务域类型相同:
     PS Only时,触发PS attach,CS_PS时,触发 CS_PS Attach,CS only触发CS Attach */
    VOS_UINT8                          ucCsAttachAllow;
    VOS_UINT8                          ucPsAttachAllow;
    VOS_UINT8                          ucSndPsAttach;
    VOS_UINT8                          ucSndCsAttach;

    ucSndPsAttach       = VOS_TRUE;
    ucSndCsAttach       = VOS_TRUE;
    ucCsAttachAllow     = TAF_SDC_GetCsAttachAllowFlg();
    ucPsAttachAllow     = TAF_SDC_GetPsAttachAllowFlg();

    /* 如果本次syscfg需要PS attach, 当前的手机模式肯定是支持PS域的
       1.如果syscfg之前的手机模式是支持PS域的，并且ps不允许注册，那么可能是at^cgcatt=0,1导致的ps detach,不需要给MMC发attach req
       2.如果syscfg之前的手机模式是支持PS域的，并且PS允许注册，需要给MMC发attach req
       3.如果syscfg之前的手机模式是不支持PS域的，现在设置为支持，需要给MMC发attach req */
    /* CS域处理相同 */

    /* 之前的模式支持PS域，但PS不允许注册，不需要发ps attach */
    if ( (VOS_TRUE  == TAF_MMA_IsPsServDomainAvail(gstMmaValue.stSetMsClass.MsClassType))
      && (VOS_FALSE == ucPsAttachAllow) )
    {
        ucSndPsAttach   = VOS_FALSE;
    }

    /* 之前的模式支持CS域，但CS不允许注册，不需要发cs attach */
    if ( (VOS_TRUE  == TAF_MMA_IsCsServDomainAvail(gstMmaValue.stSetMsClass.MsClassType))
      && (VOS_FALSE == ucCsAttachAllow) )
    {
        ucSndCsAttach   = VOS_FALSE;
    }


    if (TAF_PH_MS_CLASS_A == ucNewMsClassType)
    {
        /* 如果CS、PS都不需要attach,直接返回 */
        if ( (VOS_FALSE == ucSndCsAttach)
          && (VOS_FALSE == ucSndPsAttach) )
        {
            return;
        }

        /* CS、PS域中只有一个需要attach时，修改相应的pusSetFlg和pucAttachType */
        if (ucSndCsAttach != ucSndPsAttach)
        {
            if (VOS_TRUE == ucSndCsAttach)
            {
                *pusSetFlg     |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_ATTACH;
                *pucAttachType  = TAF_PH_CS_OPERATE;
            }
            else
            {
                *pusSetFlg     |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_ATTACH;
                *pucAttachType  = TAF_PH_PS_OPERATE;
            }
        }
        else
        {
            /* 两个域都需要attach */
            *pusSetFlg     |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_ATTACH;
            *pusSetFlg     |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_ATTACH;
            *pucAttachType  = TAF_PH_PS_CS_OPERATE;
        }
    }
    else if (TAF_PH_MS_CLASS_CG == ucNewMsClassType)
    {
        /* 不需要ps attach时不设标志 */
        if (VOS_FALSE == ucSndPsAttach)
        {
            return;
        }
        
        *pusSetFlg     |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_ATTACH;
        *pucAttachType  = TAF_PH_PS_OPERATE;
    }
    else
    {
        /* 不需要cs attach时不设标志 */
        if (VOS_FALSE == ucSndCsAttach)
        {
            return;
        }
        
        *pusSetFlg     |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_ATTACH;
        *pucAttachType  = TAF_PH_CS_OPERATE;
    }

    return;

}




VOS_VOID  MN_MMA_UpdateRatTrigedSrvDomianPara(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgPara,
    MMA_SYS_CFG_SET_FLG                *pusSetFlg,
    TAF_PH_ATTACH_TYPE                 *pucAttachType,
    TAF_PH_DETACH_TYPE                 *pucDetachType
)
{
    MMA_SYS_CFG_SET_FLG                 usDetachSetFlg;
    TAF_PH_DETACH_TYPE                  ucRatTrigedDetachType;

    /* 1)   RAT和Band没有变化时，直接返回 */
    if ((!(MMA_BIT_ISVALID(*pusSetFlg, MMA_SYS_CFG_MODE_SET)))
       && (!(MMA_BIT_ISVALID(*pusSetFlg, MMA_SYS_CFG_BAND_SET))))
    {
        return;
    }

    usDetachSetFlg        = MMA_SYS_CFG_NONE_SET;
    ucRatTrigedDetachType = TAF_PH_ATTACH_DETACH_NULL;


    /* 2)  当前驻留在GU模，设置为L模Only或只有L的频段时，
        根据当前的SrvDomain触发Detach操作
        (CS Only，PS Only或CS_PS时,都触发PS Detach)*/
    if (VOS_TRUE == MN_MMA_NeedDetachAttach_CampOnGUMode(pstSysCfgPara))
    {
        ucRatTrigedDetachType = TAF_PH_PS_OPERATE;
        usDetachSetFlg       |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH;
    }

    /* 3)  当前驻留在L模，设置为GU模或频段只有GU的频段时，
        根据当前的SrvDomain触发Detach操作
        (PS Only触发PS Detach ,CS_PS时,触发CS_PS Detach) */
    if (VOS_TRUE == MN_MMA_NeedDetachAttach_CampOnLMode(pstSysCfgPara))
    {
        if (TAF_PH_MS_CLASS_A == gstMmaValue.stSetMsClass.MsClassType)
        {
            ucRatTrigedDetachType = TAF_PH_PS_CS_OPERATE;
            usDetachSetFlg       |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH;
            usDetachSetFlg       |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH;
        }
        else
        {
            ucRatTrigedDetachType = TAF_PH_PS_OPERATE;
            usDetachSetFlg       |= MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH;
        }

    }

    /* RAT变化不触发Detach时也不会触发Attach，直接返回 */
    if (MMA_SYS_CFG_NONE_SET == usDetachSetFlg)
    {
        return;
    }

    /* 更新当前的Detach设置变量 */
    *pusSetFlg     |= MMA_SYS_CFG_SRV_DOMAIN_SET;
    *pusSetFlg     |= usDetachSetFlg;
    *pucDetachType |= ucRatTrigedDetachType;

    /* 更新当前的Attach设置变量 */
    MN_MMA_UpdateRatTrigedAttachPara(gstMmaValue.stSetMsClass.NewMsClassType,
                                     pusSetFlg, pucAttachType);

}
#endif






VOS_UINT32 MMA_PhSysCfgSetCmdCheck(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfg,
    MMA_SYS_CFG_SET_FLG                *pusSetFlg,
    TAF_PH_ATTACH_TYPE                 *pucAttachType,
    TAF_PH_DETACH_TYPE                 *pucDetachType
)
{
    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU *pstLastSyscfgSet    = VOS_NULL_PTR;
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList      = VOS_NULL_PTR;
    VOS_UINT32                          ulRatOrderChangedFlg;
    VOS_UINT32                          ulGuBandChangedFlg;
    TAF_PH_RAT_ORDER_STRU               stRatOrder;
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT32                          ulLBandChangedFlg;
#endif
    NAS_NVIM_ROAM_CFG_INFO_STRU         stRoamFeature;
    VOS_UINT8                           ucRoamParaChange;
    VOS_UINT32                          i;
    VOS_UINT8                           ucWriteAccessPrio;

   *pusSetFlg            = MMA_SYS_CFG_NONE_SET;
   *pucAttachType        = TAF_PH_ATTACH_DETACH_NULL;
   *pucDetachType        = TAF_PH_ATTACH_DETACH_NULL;
    pstLastSyscfgSet     = MN_MMA_GetLastSyscfgSetAddr();
    ulRatOrderChangedFlg = VOS_FALSE;
    ulGuBandChangedFlg   = VOS_FALSE;

#if (FEATURE_ON == FEATURE_LTE)
    ulLBandChangedFlg    = VOS_FALSE;
#endif

    pstRatPrioList = MN_MMA_GetRatPrioListAddr();
    if (STA_FSM_NULL == g_StatusContext.ulFsmState)
    {

        PS_MEM_SET(&stRatOrder, 0x00, sizeof(stRatOrder));

        TAF_MMA_ReadNvimRatPrioList(&stRatOrder);

        PS_MEM_CPY(pstRatPrioList, &stRatOrder, sizeof(TAF_PH_RAT_ORDER_STRU));
    }

    ulRatOrderChangedFlg = MN_MMA_IsUserSettedRatOrderChanged(&pstSysCfg->stRatOrder,
                                                      pstRatPrioList);

    /* 接入技术或接入优先级发生了改变 */
    if (VOS_TRUE == ulRatOrderChangedFlg)
    {
        *pusSetFlg |= MMA_SYS_CFG_MODE_SET;
    }

    /* GU频段参数发生了改变 */
    ulGuBandChangedFlg = MN_MMA_IsUserSettedGuBandChanged_SysCfg(&pstSysCfg->stGuBand,
                                            &pstLastSyscfgSet->stUserSetBand.uUserSetGuBand);

    if (VOS_TRUE == ulGuBandChangedFlg)
    {
        *pusSetFlg |= MMA_SYS_CFG_BAND_SET;
    }



    /* 漫游参数发生了改变 */
    ucRoamParaChange = MMA_FALSE;


    PS_MEM_SET(&stRoamFeature, 0x00, sizeof(stRoamFeature));

    if (NV_OK != NV_Read(en_NV_Item_Roam_Capa, &stRoamFeature, 2))
    {
        MN_ERR_LOG("MMA_PhSysCfgSetCmdCheck Read en_NV_Item_Roam_Capa Nvim Error");
        return TAF_PARA_WRITE_NVIM_ERROR;
    }

    if (MMA_MMC_ROAM_FEATURE_OFF == stRoamFeature.ucRoamFeatureFlg)
    {
        if ((pstLastSyscfgSet->ucRoam != pstSysCfg->ucRoam)
         && (2 != pstSysCfg->ucRoam))
        {
            ucRoamParaChange = MMA_TRUE;
        }
    }
    else
    {
        if ((pstLastSyscfgSet->ucRoam != pstSysCfg->ucRoam)
         && (TAF_PH_ROAM_UNCHANGE != pstSysCfg->ucRoam))
        {
            ucRoamParaChange = MMA_TRUE;
        }
    }

    if (MMA_TRUE == ucRoamParaChange)
    {
        *pusSetFlg |= MMA_SYS_CFG_ROAM_SET;
    }


    /* if 服务域发生了改变 */
    if (TAF_PH_SERVICE_NOCHANGE != pstSysCfg->ucSrvDomain)
    {
        if (pstSysCfg->ucSrvDomain != pstLastSyscfgSet->ucSrvDomain)
        {
            *pusSetFlg |= MMA_SYS_CFG_SRV_DOMAIN_SET;

            /*检查服务域设置引发的动作*/
            MMA_PhSysCfgSetSrvDomainCheck(pstSysCfg->ucSrvDomain, pusSetFlg, pucAttachType, pucDetachType);
        }
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* L频段参数发生改变 */
    ulLBandChangedFlg = MN_MMA_IsUserSettedLBandChanged(&pstSysCfg->stLBand,
                                                   &pstLastSyscfgSet->stUserSetBand.stUserSetLteBand);
    if (VOS_TRUE == ulLBandChangedFlg)
    {
        *pusSetFlg |= MMA_SYS_CFG_BAND_SET;
    }

#endif

    /* 不支持L模时需要写入 */
    ucWriteAccessPrio = VOS_TRUE;
    for ( i = 0 ; i < gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder.ucRatOrderNum; i++ )
    {
        if ( TAF_PH_RAT_LTE == gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder.aenRatOrder[i])
        {
            ucWriteAccessPrio = VOS_FALSE;
        }
    }

    if ( (VOS_TRUE == ucWriteAccessPrio)
      && ( pstSysCfg->enUserPrio != pstLastSyscfgSet->enPrioRat)
      && ( TAF_MMA_USER_SET_PRIO_NOCHANGE != pstSysCfg->enUserPrio))
    {
        /* 不设置的情况需要将接入优先级写入NVIM中 */
        if ( MMA_SYS_CFG_NONE_SET == *pusSetFlg )
        {
            pstLastSyscfgSet->enPrioRat = pstSysCfg->enUserPrio;
            TAF_MMA_WritePrioAccessModeToNvim();
        }
    }


    /* 保存SYSCFG设置参数 */
    PS_MEM_CPY(&(gstMmaValue.stSysCfgSet.stSysCfgSetPara), pstSysCfg, sizeof(TAF_MMA_SYS_CFG_PARA_STRU));


#if (FEATURE_ON == FEATURE_LTE)

    MN_MMA_UpdateRatTrigedSrvDomianPara(pstSysCfg, pusSetFlg,
                                        pucAttachType, pucDetachType);
#endif
    /* 记录设置标志 */
    gstMmaValue.stSysCfgSet.usSetFlag    = *pusSetFlg;
    gstMmaValue.stSysCfgSet.ucAttachType = *pucAttachType;
    gstMmaValue.stSysCfgSet.ucDetachType = *pucDetachType;

    return VOS_OK;
}




VOS_VOID MMA_PhSysCfgSet(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfg
)
{
    MMA_SYS_CFG_SET_FLG                 usSetFlg;
    TAF_PH_ATTACH_TYPE                  ucAttachType;
    TAF_PH_DETACH_TYPE                  ucDetachType;
    VOS_UINT32                          ulRst;
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT8                           ucCardType;
    VOS_UINT8                           ucRatBalancingEnableFlg;

    ucRatBalancingEnableFlg = TAF_MMA_GetRatBalancingEnableFlg();

    ucCardType = USIMM_CARD_NOCARD;
#endif

    usSetFlg     = MMA_SYS_CFG_NONE_SET;
    ucAttachType = TAF_PH_ATTACH_DETACH_NULL;
    ucDetachType = TAF_PH_ATTACH_DETACH_NULL;
    ulRst        = VOS_TRUE;

    ulRst = TAF_MMA_IsSysCfgSetValid(pstSysCfg);
    if (VOS_TRUE != ulRst)
    {
        MN_ERR_LOG("MMA_PhSysCfgSet:PARA ERROR!");

        TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_PARA_ERROR);

        return;
    }

    ulRst = MMA_IsGsmForbidden();

    /* 如果UE不支持GSM，当设置成G单模时，返回失败;当设置成多模时，给MMC发送请求时校正去掉G模 */
    if (MMA_TRUE == ulRst)
    {
        if (VOS_TRUE == MN_MMA_IsGOnlyMode(&pstSysCfg->stRatOrder))
        {
            MN_ERR_LOG("MMA_PhSysCfgSet:GSM ONLY & FORBIDDEN!");

            TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_PARA_ERROR);

            return;
        }
    }

#if (FEATURE_ON == FEATURE_LTE)

    /* 如果是SIM卡，当设置成L单模时，返回失败；当设置成多模时，给MMC发送请求时校正去掉L模 */
    if (VOS_TRUE == MN_MMA_IsLOnlyMode(&pstSysCfg->stRatOrder))
    {
        NAS_USIMMAPI_GetCardType(VOS_NULL_PTR, &ucCardType) ;

        if ( (USIMM_CARD_SIM == ucCardType)
          && (VOS_TRUE       == ucRatBalancingEnableFlg) )
        {
            MMA_ERRORLOG("MMA_PhSysCfgSet: L ONLY & SIM CARD!");

            TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_PARA_ERROR);

            return;
        }
    }
#endif

    /* 获取参数 */
    MMA_PhSysCfgSetCmdCheck(pstSysCfg, &usSetFlg, &ucAttachType, &ucDetachType);

    /* 无需设置直接上报成功 */
    if (MMA_SYS_CFG_NONE_SET == usSetFlg)
    {
        TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_NO_ERROR);

        MN_NORM_LOG("MMA_PhSysCfgSet:NORMAL:SYSCFG NONE SET!");
    }
    else
    {
        /* 记录ClientId,OpId，用于处理结果的上报 */
        gstMmaValue.stSysCfgSet.usClientId = ClientId;
        gstMmaValue.stSysCfgSet.ucOpId     = OpId;

        switch (g_StatusContext.ulFsmState)
        {
            case STA_FSM_NULL:
            case STA_FSM_STOP:
                MN_MMA_SetSyscfg_PowerOffStatus();
                break;

            case STA_FSM_RESTART:
                /* 内部重启过程,MMA_PhSysCfgSetCmdCheck已缓存Syscfg,待收到正常开机时处理 */
                break;

            case STA_FSM_PLMN_RESEL:
                TAF_MMA_RcvAppSyscfg_PlmnResel(pstSysCfg);
                break;

            case STA_FSM_ENABLE:
                MN_MMA_SetSyscfg_EnableStatus(pstSysCfg, TAF_MMA_SYSCFG_USER_AT);
                break;

            case STA_FSM_SYS_CFG_SET:
                TAF_MMA_RcvAppSyscfg_SysCfgSet(ClientId, OpId);
                break;

            default:
                MN_WARN_LOG("MMA_PhSysCfgSet:WARNING: error status");
                break;
        }

    }

    return;
}




VOS_UINT32  MMA_SwitchUserSetBand2UESupportBand
( MMA_USER_BAND_SET_UN           uUserSetBand,
  MMA_UE_SUPPORT_FREQ_BAND_STRU *pstUeBand)
{

    MMA_USER_PREF_BAND_STRU            *pstUserBitBand;
    MMA_GSM_PREF_BAND_STRU             *pstGsmBitBand;
    MMA_WCDMA_PREF_BAND_STRU           *pstWcdmaBitBand;
    MN_MMA_SYSCFG_USER_SET_BAND_STRU   *pstUserSetBand = VOS_NULL_PTR;

    pstUserSetBand = MN_MMA_GetSyscfgUserSetBandAddr();

    pstUserBitBand = &(uUserSetBand.BitBand);
    pstGsmBitBand = &(pstUeBand->unGsmBand.BitBand);
    pstWcdmaBitBand = &(pstUeBand->unWcdmaBand.BitBand);

    PS_MEM_SET(pstGsmBitBand,0,sizeof(MMA_GSM_PREF_BAND_STRU));
    PS_MEM_SET(pstWcdmaBitBand,0,sizeof(MMA_WCDMA_PREF_BAND_STRU));

    if (TAF_PH_BAND_NO_CHANGE == uUserSetBand.ulPrefBand)
    {
        pstUeBand->unGsmBand.ulBand   = pstUserSetBand->uUserSetUeFormatGuBand.unGsmBand.ulBand;
        pstUeBand->unWcdmaBand.ulBand = pstUserSetBand->uUserSetUeFormatGuBand.unWcdmaBand.ulBand;

        MMA_NORMAILLOG("MMA_SwitchUserSetBand2UESupportBand:INFO:BAND NO CHANGE.");
        return MMA_SUCCESS;
    }

    if (TAF_PH_BAND_ANY == uUserSetBand.ulPrefBand)
    {
        uUserSetBand.ulPrefBand = gstMmaBandCapa.ulAllUeBand;
    }

    /* 过滤无效频段 */
    uUserSetBand.ulPrefBand  &= (gstMmaBandCapa.ulAllUeBand);

    pstGsmBitBand->BandGsm850 = pstUserBitBand->BandGsm850;
    pstGsmBitBand->BandGsm1800 = pstUserBitBand->BandGsmDcs1800;
    pstGsmBitBand->BandGsmE900 = pstUserBitBand->BandGsmEgsm900;
    pstGsmBitBand->BandGsm1900 = pstUserBitBand->BandGsmPcs1900;
    pstGsmBitBand->BandGsmP900 = pstUserBitBand->BandGsmPgsm900;
    pstGsmBitBand->BandGsmR900 = pstUserBitBand->BandGsmRgsm900;
    pstGsmBitBand->BandGsm850 = pstUserBitBand->BandGsm850;

    pstWcdmaBitBand->BandWCDMA_I_2100 = pstUserBitBand->BandWcdma_I_Imt2k1;
    pstWcdmaBitBand->BandWCDMA_II_1900 = pstUserBitBand->BandWcdma_II_PCS_1900;
    pstWcdmaBitBand->BandWCDMA_III_1800 = pstUserBitBand->BandWcdma_III_1800;
    pstWcdmaBitBand->BandWCDMA_IV_1700 = pstUserBitBand->BandWcdma_IV_1700;
    pstWcdmaBitBand->BandWCDMA_V_850 = pstUserBitBand->BandWcdma_V_850;
    pstWcdmaBitBand->BandWCDMA_VI_800 = pstUserBitBand->BandWcdma_VI_800;
    pstWcdmaBitBand->BandWCDMA_VII_2600 = pstUserBitBand->BandWcdma_VII_2600;
    pstWcdmaBitBand->BandWCDMA_VIII_900 = pstUserBitBand->BandWcdma_VIII_900;
    pstWcdmaBitBand->BandWCDMA_IX_J1700 = pstUserBitBand->BandWcdma_IX_1700;

    pstWcdmaBitBand->BandWCDMA_XIX_850 = pstUserBitBand->BandWCDMA_XIX_850;
    pstWcdmaBitBand->BandWCDMA_XI_1500  = pstUserBitBand->BandWcdma_XI_1500;

    MMA_INFOLOG1("MMA_SwitchUserSetBand2UESupportBand:INFO:GSM band:",
                 (VOS_INT32)pstUeBand->unGsmBand.ulBand);
    MMA_INFOLOG1("MMA_SwitchUserSetBand2UESupportBand:INFO:WCDMA band:",
                 (VOS_INT32)pstUeBand->unWcdmaBand.ulBand);

    return MMA_SUCCESS;
}
VOS_UINT32  MMA_SwitchUESupportBand2UserSetBand
(   MMA_UE_SUPPORT_FREQ_BAND_STRU stUeBand,
    MMA_USER_BAND_SET_UN *        puUserSetBand)
{
    MMA_USER_PREF_BAND_STRU            *pstUserBitBand;
    MMA_GSM_PREF_BAND_STRU             *pstGsmBitBand;
    MMA_WCDMA_PREF_BAND_STRU           *pstWcdmaBitBand;

    PS_MEM_SET(puUserSetBand, 0, sizeof(MMA_USER_BAND_SET_UN));
    pstUserBitBand                      = &(puUserSetBand->BitBand);
    pstGsmBitBand                       = &(stUeBand.unGsmBand.BitBand);
    pstWcdmaBitBand                     = &(stUeBand.unWcdmaBand.BitBand);

    pstUserBitBand->BandGsmDcs1800        = pstGsmBitBand->BandGsm1800;
    pstUserBitBand->BandGsmPcs1900        = pstGsmBitBand->BandGsm1900;
    pstUserBitBand->BandGsm850            = pstGsmBitBand->BandGsm850;
    pstUserBitBand->BandGsmEgsm900        = pstGsmBitBand->BandGsmE900;
    pstUserBitBand->BandGsmPgsm900        = pstGsmBitBand->BandGsmP900;
    pstUserBitBand->BandGsmRgsm900        = pstGsmBitBand->BandGsmR900;
    pstUserBitBand->BandWcdma_I_Imt2k1    = pstWcdmaBitBand->BandWCDMA_I_2100;
    pstUserBitBand->BandWcdma_II_PCS_1900 = pstWcdmaBitBand->BandWCDMA_II_1900;
    pstUserBitBand->BandWcdma_III_1800    = pstWcdmaBitBand->BandWCDMA_III_1800;
    pstUserBitBand->BandWcdma_IV_1700     = pstWcdmaBitBand->BandWCDMA_IV_1700;
    pstUserBitBand->BandWcdma_V_850       = pstWcdmaBitBand->BandWCDMA_V_850;
    pstUserBitBand->BandWcdma_VI_800      = pstWcdmaBitBand->BandWCDMA_VI_800;
    pstUserBitBand->BandWcdma_VII_2600    = pstWcdmaBitBand->BandWCDMA_VII_2600;
    pstUserBitBand->BandWcdma_VIII_900    = pstWcdmaBitBand->BandWCDMA_VIII_900;
    pstUserBitBand->BandWcdma_IX_1700     = pstWcdmaBitBand->BandWCDMA_IX_J1700;
    pstUserBitBand->BandWcdma_XI_1500     = pstWcdmaBitBand->BandWCDMA_XI_1500;

    pstUserBitBand->BandWCDMA_XIX_850     = pstWcdmaBitBand->BandWCDMA_XIX_850;
    MMA_INFOLOG1("MMA_SwitchUESupportBand2UserSetBand:INFO:band:",
                 (VOS_INT32)puUserSetBand->ulPrefBand);
    return MMA_SUCCESS;
}



VOS_UINT32 MMA_PhSysCfgWriteNvim(VOS_VOID)
{
    MMA_UE_SUPPORT_FREQ_BAND_STRU       stSetBand;
    TAF_PH_MS_CLASS_TYPE                MsClass;
    NAS_NVIM_ROAM_CFG_INFO_STRU         stRoamFeature;
    VOS_UINT8                           ucRoamParaChange;

    TAF_PH_NVIM_RAT_ORDER_STRU          stRatPrioList;

    TAF_PH_PREF_BAND                    ulBand;
    VOS_UINT32                          ulRatOrderChangedFlg;
    MMA_USER_BAND_SET_UN                uUserSetBand;
    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU *pstLastSyscfgSet = VOS_NULL_PTR;
    NAS_MMA_NVIM_ACCESS_MODE_STRU        stAccessMode;
    VOS_UINT32                          i;
    VOS_UINT32                          ulLength;

#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT32                          ulLBandChangedFlg;
    ulLBandChangedFlg    = VOS_TRUE;
#endif

    ulLength = 0;
    pstLastSyscfgSet     = MN_MMA_GetLastSyscfgSetAddr();
    ucRoamParaChange     = MMA_FALSE;
    ulRatOrderChangedFlg = VOS_TRUE;
#if (FEATURE_ON == FEATURE_LTE)
    ulLBandChangedFlg    = VOS_TRUE;
#endif

    ulRatOrderChangedFlg = MN_MMA_IsUserSettedRatOrderChanged(&gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder,
                                                                     MN_MMA_GetRatPrioListAddr());
    if (VOS_TRUE == ulRatOrderChangedFlg)
    {
        PS_MEM_CPY(&stRatPrioList, &(gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder), sizeof(TAF_PH_NVIM_RAT_ORDER_STRU));

        /* 更新接入技术,接入优先级NV */
        if (MMA_SUCCESS != MMA_WriteValue2Nvim(en_NV_Item_RAT_PRIO_LIST,
                                               &stRatPrioList,
                                               sizeof(stRatPrioList)))
        {
            MN_ERR_LOG("MMA_PhSysCfgWriteNvim():WARNING:NVIM_Write en_NV_Item_RAT_PRIO_LIST failed");
        }

        /* 更新全局变量 */
        PS_MEM_CPY(&(pstLastSyscfgSet->stRatPrioList), &stRatPrioList, sizeof(TAF_PH_NVIM_RAT_ORDER_STRU));

        TAF_SDC_SetMsPrioRatList((TAF_SDC_RAT_PRIO_STRU *)&(pstLastSyscfgSet->stRatPrioList));
    }

#if (FEATURE_ON == FEATURE_LTE)
    ulLBandChangedFlg = MN_MMA_IsUserSettedLBandChanged(&gstMmaValue.stSysCfgSet.stSysCfgSetPara.stLBand,
                                                               &pstLastSyscfgSet->stUserSetBand.stUserSetLteBand);
    if (VOS_TRUE == ulLBandChangedFlg)
    {

        /* 更新用户设置的L频段的NV */
        if (MMA_SUCCESS != MMA_WriteValue2Nvim(en_NV_Item_USER_SET_LTEBANDS,
                                               &(gstMmaValue.stSysCfgSet.stSysCfgSetPara.stLBand),
                                               sizeof(TAF_USER_SET_PREF_BAND64)))
        {
            MN_ERR_LOG("MMA_PhSysCfgWriteNvim():WARNING:NVIM_Write en_NV_Item_USER_SET_LTEBANDS failed");
        }

        /* 更新全局变量 */
        pstLastSyscfgSet->stUserSetBand.stUserSetLteBand = gstMmaValue.stSysCfgSet.stSysCfgSetPara.stLBand;
    }

#endif

    MN_MMA_Convert64BitBandTo32Bit(&gstMmaValue.stSysCfgSet.stSysCfgSetPara.stGuBand, &ulBand);
    if (TAF_PH_BAND_NO_CHANGE != ulBand)
    {
        MN_MMA_ConvertGUFrequencyBand(&ulBand);
        uUserSetBand.ulPrefBand = ulBand;
        MMA_SwitchUserSetBand2UESupportBand(uUserSetBand, &stSetBand);

        /* 更新频段全局参数 */
        pstLastSyscfgSet->stUserSetBand.uUserSetGuBand.ulPrefBand                 = uUserSetBand.ulPrefBand ;
        pstLastSyscfgSet->stUserSetBand.uUserSetUeFormatGuBand.unGsmBand.ulBand   = stSetBand.unGsmBand.ulBand;
        pstLastSyscfgSet->stUserSetBand.uUserSetUeFormatGuBand.unWcdmaBand.ulBand = stSetBand.unWcdmaBand.ulBand;
        pstLastSyscfgSet->stUserSetBand.stOrigUserSetGuBand                       = gstMmaValue.stSysCfgSet.stSysCfgSetPara.stGuBand;

        /* 更新用户设置频段的NV */
        if (MMA_SUCCESS != MMA_WriteValue2Nvim(en_NV_Item_User_Set_Freqbands,
                                               &(gstMmaValue.stSysCfgSet.stSysCfgSetPara.stGuBand),
                                               sizeof(gstMmaValue.stSysCfgSet.stSysCfgSetPara.stGuBand)))
        {
            MN_ERR_LOG("MMA_PhSysCfgWriteNvim():WARNING:NVIM_Write en_NV_Item_User_Set_Freqbands failed");
        }

        /* 更新频段NV */
        if (MMA_SUCCESS != MMA_WriteValue2Nvim(en_NV_Item_Support_Freqbands,
                                               &(stSetBand),
                                               sizeof(stSetBand.unGsmBand)+sizeof(stSetBand.unWcdmaBand)))
        {
            MN_ERR_LOG("MMA_PhSysCfgWriteNvim():WARNING:NVIM_Write en_NV_Item_Support_Freqbands failed");
        }
    }


    PS_MEM_SET(&stRoamFeature, 0x00, sizeof(NAS_NVIM_ROAM_CFG_INFO_STRU));

    if (NV_OK != NV_Read(en_NV_Item_Roam_Capa, &stRoamFeature, 2))
    {
        MN_ERR_LOG("MMA_PhSysCfgWriteNvim Read en_NV_Item_Roam_Capa Nvim Error");
    }

    if (MMA_MMC_ROAM_FEATURE_OFF == stRoamFeature.ucRoamFeatureFlg)
    {
        if ((pstLastSyscfgSet->ucRoam != gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucRoam)
         && (2 != gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucRoam))
        {
            ucRoamParaChange = MMA_TRUE;
        }
    }
    else
    {
        if ((pstLastSyscfgSet->ucRoam != gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucRoam)
         && (TAF_PH_ROAM_UNCHANGE != gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucRoam))
        {
            ucRoamParaChange = MMA_TRUE;
        }
    }
    if (MMA_TRUE == ucRoamParaChange)
    {
        /* 更新漫游支持项全局参数 */
        pstLastSyscfgSet->ucRoam = gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucRoam;

        MN_INFO_LOG1("MMA_PhSysCfgWriteNvim: gstMmaValue.ucRoam", pstLastSyscfgSet->ucRoam);

        stRoamFeature.ucRoamCapability = pstLastSyscfgSet->ucRoam;
        /* 更新漫游支持项NV */
        if (MMA_SUCCESS != MMA_WriteValue2Nvim(en_NV_Item_Roam_Capa,
                                               &stRoamFeature,
                                               2))
        {
            MN_WARN_LOG("MMA_PhSysCfgWriteNvim():WARNING:NVIM_Write en_NV_Item_Support_Freqbands failed");
        }
        MN_INFO_LOG1("MMA_PhSysCfgWriteNvim: usRoamCapa", pstLastSyscfgSet->ucRoam);
    }

    if (TAF_PH_SERVICE_NOCHANGE != gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucSrvDomain)
    {
        /* 更新服务域全局参数 */
        pstLastSyscfgSet->ucSrvDomain = gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucSrvDomain;
        MMA_ChangeSrv2Class(gstMmaValue.stSysCfgSet.stSysCfgSetPara.ucSrvDomain, &MsClass);
        gstMmaValue.stSetMsClass.NewMsClassType = MsClass;
        gstMmaValue.stSetMsClass.MsClassType = MsClass;
        gstMmaValue.pg_StatusContext->ucModeService = MsClass;

        /* 更新服务域NV */
        if (MMA_SUCCESS != MMA_WriteValue2Nvim(en_NV_Item_MMA_MsClass,
                                               &(pstLastSyscfgSet->ucSrvDomain),
                                                sizeof(TAF_PH_MS_CLASS_TYPE)))
        {
            MN_WARN_LOG("MMA_PhSysCfgWriteNvim:WARNING:NV_Write runs failed");
        }

    }

    /* 不支持L模时需要写入 */
    for ( i = 0 ; i < gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder.ucRatOrderNum; i++ )
    {
        if ( TAF_PH_RAT_LTE == gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder.aenRatOrder[i])
        {
            return MMA_SUCCESS;
        }
    }


    PS_MEM_SET(&stAccessMode, 0x00, sizeof(NAS_MMA_NVIM_ACCESS_MODE_STRU));

    NV_GetLength(en_NV_Item_MMA_AccessMode, &ulLength);
    if (ulLength > sizeof(NAS_MMA_NVIM_ACCESS_MODE_STRU))
    {
        return MMA_SUCCESS;
    }

    if (NV_OK != NV_Read(en_NV_Item_MMA_AccessMode ,
                         &stAccessMode,
                         ulLength))

    {
        MN_WARN_LOG("MMA_PhSysCfgWriteNvim:Read:NV_Read runs failed");
        return MMA_SUCCESS;
    }

    /* NVIM中第二个字节表示接入优先级 */
    if ( ( ( (stAccessMode.aucAccessMode)[1] != gstMmaValue.stSysCfgSet.stSysCfgSetPara.enUserPrio)
         && ( TAF_MMA_USER_SET_PRIO_NOCHANGE != gstMmaValue.stSysCfgSet.stSysCfgSetPara.enUserPrio))
      || ( VOS_TRUE == ulRatOrderChangedFlg ))
    {
        /* 需要更新NVIM项 */
        if ( ( 1 == gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder.ucRatOrderNum)
          && ( TAF_PH_RAT_GSM == gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder.aenRatOrder[0]))
        {
            (stAccessMode.aucAccessMode)[0] = TAF_PH_USER_SET_RAT_GSM;
        }
        else if ( ( 1 == gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder.ucRatOrderNum)
               && ( TAF_PH_RAT_WCDMA == gstMmaValue.stSysCfgSet.stSysCfgSetPara.stRatOrder.aenRatOrder[0]))
        {
            (stAccessMode.aucAccessMode)[0] = TAF_PH_USER_SET_RAT_WCDMA;
        }
        else
        {
            (stAccessMode.aucAccessMode)[0] = TAF_PH_USER_SET_RAT_WCDMA_GSM;
        }

        if ( TAF_MMA_USER_SET_PRIO_NOCHANGE != gstMmaValue.stSysCfgSet.stSysCfgSetPara.enUserPrio)
        {
            pstLastSyscfgSet->enPrioRat = gstMmaValue.stSysCfgSet.stSysCfgSetPara.enUserPrio;
            (stAccessMode.aucAccessMode)[1] = gstMmaValue.stSysCfgSet.stSysCfgSetPara.enUserPrio;
        }

        if (MMA_SUCCESS != MMA_WriteValue2Nvim(en_NV_Item_MMA_AccessMode,
                                               &stAccessMode,
                                               (VOS_UINT16)ulLength))
        {
            MN_ERR_LOG("MMA_PhSysCfgWriteNvim():WARNING:NVIM_Write en_NV_Item_MMA_AccessMode failed");
        }


    }


    return MMA_SUCCESS;
}


VOS_UINT32  MMA_IsGsmForbidden()
{
    VOS_UINT32   ulGsmFordidenFlg = MMA_FALSE;
    VOS_UINT32   ulEfustGsmAccess = 1;
    VOS_UINT32   ulEfustGsmSecContext = 1;

    if (MMA_QUICK_START_ENABLE == gstMmaValue.ulQuickStartFlg)
    {
        return MMA_FALSE;
    }

    /* 删除对USIM卡禁止GSM接入的判断 */

    /* 判断G模支持的频段，如果UE不支持G模，认为G模被禁止 */
    if (0 == gstMmaBandCapa.ulUeGSptBand)
    {
        ulGsmFordidenFlg = MMA_TRUE;
    }

    MMA_INFOLOG1("MMA_IsGsmForbidden: ucForbidReg2GNetWork = ",(VOS_INT32)gstMmaValue.stEfustServiceCfg.ucForbidReg2GNetWork);
    MMA_INFOLOG1("MMA_IsGsmForbidden: ulEfustGsmAccess = ",(VOS_INT32)ulEfustGsmAccess);
    MMA_INFOLOG1("MMA_IsGsmForbidden: ulEfustGsmSecContext = ",(VOS_INT32)ulEfustGsmSecContext);

    return ulGsmFordidenFlg;
}


/* MMA_SndMmcSysCfgReq函数移至MmaSndMmc.c中 */

VOS_UINT32 TAF_MMA_ConvertDetachTypeToMmc(
    VOS_UINT32                          ulDetachType,
    MMA_MMC_DETACH_TYPE_ENUM_UINT32    *pulMmcDetachType
)
{
    switch(ulDetachType)
    {
        case TAF_PH_PS_OPERATE:
            *pulMmcDetachType = MMA_MMC_DETACH_TYPE_GPRS;
            break;

        case TAF_PH_CS_OPERATE:
            *pulMmcDetachType = MMA_MMC_DETACH_TYPE_IMSI;
            break;

        case TAF_PH_PS_CS_OPERATE:
            *pulMmcDetachType = MMA_MMC_DETACH_TYPE_GPRS_IMSI;
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_ConvertAttachTypeToMmc(
    VOS_UINT32                          ulAttachType,
    MMA_MMC_ATTACH_TYPE_ENUM_UINT32    *pulMmcAttachType
)
{
    switch(ulAttachType)
    {
        case TAF_PH_PS_OPERATE:
            *pulMmcAttachType = MMA_MMC_ATTACH_TYPE_GPRS;
            break;

        case TAF_PH_CS_OPERATE:
            *pulMmcAttachType = MMA_MMC_ATTACH_TYPE_IMSI;
            break;

        case TAF_PH_PS_CS_OPERATE:
            *pulMmcAttachType = MMA_MMC_ATTACH_TYPE_GPRS_IMSI;
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}



VOS_UINT32 TAF_MMA_ConvertPlmnPriorityClass(
    MMC_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8                  enPrioClass,
    TAF_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8                 *penDestPrioClass
)
{
    switch(enPrioClass)
    {
        case MMC_MMA_PLMN_PRIORITY_HOME:
            *penDestPrioClass = TAF_MMA_PLMN_PRIORITY_HOME;
            break;

        case MMC_MMA_PLMN_PRIORITY_PREF:
            *penDestPrioClass = TAF_MMA_PLMN_PRIORITY_PREF;
            break;

        case MMC_MMA_PLMN_PRIORITY_ANY:
            *penDestPrioClass = TAF_MMA_PLMN_PRIORITY_ANY;
            break;

        case MMC_MMA_PLMN_PRIORITY_BUTT:
            *penDestPrioClass = TAF_MMA_PLMN_PRIORITY_BUTT;
            break;

        default:
            NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_ConvertPlmnPriorityClass ERROR!!!");
            *penDestPrioClass = TAF_MMA_PLMN_PRIORITY_BUTT;

            return VOS_FALSE;
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_MMA_ConvertCardType(
    USIMM_CARD_TYPE_ENUM_UINT32                             enCardType,
    TAF_MMA_USIMM_CARD_TYPE_ENUM_UINT32                    *penDestCardType
)
{
    switch(enCardType)
    {
        case USIMM_CARD_SIM:
            *penDestCardType = TAF_MMA_USIMM_CARD_SIM;
            break;

        case USIMM_CARD_USIM:
            *penDestCardType = TAF_MMA_USIMM_CARD_USIM;
            break;

        case USIMM_CARD_ROM_SIM:
            *penDestCardType = TAF_MMA_USIMM_CARD_ROM_SIM;
            break;

        case USIMM_CARD_ISIM:
            *penDestCardType = TAF_MMA_USIMM_CARD_ISIM;
            break;

        case USIMM_CARD_NOCARD:
            *penDestCardType = TAF_MMA_USIMM_CARD_NOCARD;
            break;

        default:
            *penDestCardType = TAF_MMA_USIMM_CARD_TYPE_BUTT;
            break;
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_MMA_ConvertCardStatus(
    USIMM_CARD_SERVIC_ENUM_UINT32       enCardStatus,
    TAF_MMA_CARD_STATUS_ENUM_UINT8     *penDestCardStatus
)
{
    switch(enCardStatus)
    {
        case USIMM_CARD_SERVIC_AVAILABLE:
            *penDestCardStatus = TAF_MMA_CARD_STATUS_AVAILABLE;
            break;

        default:
            *penDestCardStatus = TAF_MMA_CARD_STATUS_NOT_AVAILABLE;
            break;
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_MMA_ConvertServiceStatus(
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8                       enServiceStatus,
    TAF_PHONE_SERVICE_STATUS                               *penDestServiceStatus
)
{
    switch(enServiceStatus)
    {
        case TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE:
            *penDestServiceStatus = TAF_REPORT_SRVSTA_NORMAL_SERVICE;
            break;

        case TAF_SDC_SERVICE_STATUS_LIMITED_SERVICE:
            *penDestServiceStatus = TAF_REPORT_SRVSTA_LIMITED_SERVICE;
            break;

        case TAF_SDC_SERVICE_STATUS_LIMITED_SERVICE_REGION:
            *penDestServiceStatus = TAF_REPORT_SRVSTA_REGIONAL_LIMITED_SERVICE;
            break;

        case TAF_SDC_SERVICE_STATUS_NO_IMSI:
            *penDestServiceStatus = TAF_REPORT_SRVSTA_LIMITED_SERVICE;
            break;

        case TAF_SDC_SERVICE_STATUS_NO_SERVICE:
            *penDestServiceStatus = TAF_REPORT_SRVSTA_NO_SERVICE;
            break;

        case TAF_SDC_SERVICE_STATUS_NO_CHANGE:
            *penDestServiceStatus = TAF_REPORT_SRVSTA_LIMITED_SERVICE;
            break;

        case TAF_SDC_SERVICE_STATUS_DEEP_SLEEP:
            *penDestServiceStatus = TAF_REPORT_SRVSTA_DEEP_SLEEP;
            break;

        default:
            *penDestServiceStatus = TAF_REPORT_SRVSTA_NO_SERVICE;

            return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID MMA_PhSysCfgQuery(VOS_UINT16 ClientId,
                           VOS_UINT8  OpId)
{
    /*直接上报当前参数*/
    MMA_PhSysCfgReport(ClientId, OpId, TAF_PH_CMD_QUERY, TAF_ERR_NO_ERROR);
    return;
}



#if (FEATURE_ON == FEATURE_LTE)
VOS_VOID MN_MMA_GetSupportedUserLteBand(
    TAF_USER_SET_PREF_BAND64           *pstLBand
)
{
    TAF_USER_SET_PREF_BAND64           *pstUserSetLteBand   = VOS_NULL_PTR;
    TAF_USER_SET_PREF_BAND64           *pstUeSupportLteBand = VOS_NULL_PTR;

    /* 取得当前用户设置的LTE的频段 */
    pstUserSetLteBand   = MN_MMA_GetUserSetLteBandAddr();

    /* 取得当前产品支持的LTE的频段 */
    pstUeSupportLteBand = MN_MMA_GetUeSupportLteBandAddr();

    /* 获取当前产品支持的用户设置的LTE频段信息 */
    pstLBand->ulBandLow  = (pstUserSetLteBand->ulBandLow) & (pstUeSupportLteBand->ulBandLow);
    pstLBand->ulBandHigh = (pstUserSetLteBand->ulBandHigh) & (pstUeSupportLteBand->ulBandHigh);

}

#endif
VOS_VOID TAF_MMA_ReportSysCfgSetCnf(
    TAF_PH_ERR_CODE                     usErrorCode
)
{
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    VOS_UINT8                           ucCtxIndex;
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt;

    pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();
    enRslt          = TAF_MMA_APP_OPER_RESULT_SUCCESS;

    if ( TAF_ERR_NO_ERROR != usErrorCode )
    {
        enRslt = TAF_MMA_APP_OPER_RESULT_FAILURE;
    }

    if (VOS_TRUE == TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_SYS_CFG_REQ, &ucCtxIndex))
    {
        /* 进行SYSCFG的设置回复 */
        TAF_MMA_SndSysCfgSetCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                                enRslt,
                                usErrorCode);

        /* SYSCFG设置完成后，清除CTX中内容  */
        TAF_MMA_ClearOperCtx(ucCtxIndex);
    }
    else
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_ReportSysCfgSetCnf:ERROR");
    }

    return;

}


VOS_VOID MMA_PhSysCfgReport(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    TAF_PH_CMD_TYPE                     ucCmdType,
    TAF_PH_ERR_CODE                     usErrorCode
)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstEvent;
    NAS_NVIM_ROAM_CFG_INFO_STRU         stRoamFeature;
    VOS_UINT32                          ulLength;

    NAS_MMA_NVIM_ACCESS_MODE_STRU        stAccessMode;
    NAS_NVIM_MS_CLASS_STRU          stMsClass;
    TAF_PH_MS_CLASS_TYPE                ucMsClassResult;

    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU *pstLastSyscfgSet = VOS_NULL_PTR;
    ulLength        = 0;

    pstLastSyscfgSet = MN_MMA_GetLastSyscfgSetAddr();

    pstEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstEvent)
    {
        MMA_ERRORLOG("MMA_PhSysCfgReport:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    /* 初始化事件上报 */
    PS_MEM_SET(pstEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    /* 操作模式项置 */
    pstEvent->OP_SysCfg = 1;
    pstEvent->PhoneEvent = TAF_PH_EVT_SYS_CFG_CNF;

    /* 如果有错误填写错误码 */
    if (TAF_ERR_NO_ERROR != usErrorCode)
    {
        pstEvent->OP_PhoneError = 1;
        pstEvent->PhoneError = usErrorCode;
        MMA_WARNINGLOG1("MMA_PhModeReport():WARNING:Report Error code:", usErrorCode);
    }


    if (TAF_PH_CMD_QUERY == ucCmdType)
    {
        /* 获取当前的接入模式优先级，如果关机，从NV:en_NV_Item_RAT_PRIO_LIST中读取 */
        if (STA_FSM_NULL == g_StatusContext.ulFsmState)
        {
            TAF_MMA_ReadNvimRatPrioList(&(pstLastSyscfgSet->stRatPrioList));
            PS_MEM_CPY(&(gstMmaValue.pg_StatusContext->stRatPrioList),
                       &(pstLastSyscfgSet->stRatPrioList), sizeof(TAF_PH_RAT_ORDER_STRU));

            NV_GetLength(en_NV_Item_MMA_AccessMode, &ulLength);

            if (NV_OK != NV_Read(en_NV_Item_MMA_AccessMode ,
                     &stAccessMode,
                     ulLength))

            {
                (stAccessMode.aucAccessMode)[1] = TAF_PLMN_PRIO_AUTO;
            }
            pstLastSyscfgSet->enPrioRat = (stAccessMode.aucAccessMode)[1] ;
        }

        pstEvent->stSysCfg.stRatOrder = pstLastSyscfgSet->stRatPrioList;

        /* 获取当前用户设置的GU频段信息 */
        pstEvent->stSysCfg.stGuBand = pstLastSyscfgSet->stUserSetBand.stOrigUserSetGuBand;

        /* 获取当前用户设置的漫游信息 */
        pstEvent->stSysCfg.ucRoam = pstLastSyscfgSet->ucRoam;

        /* 获取当前用户设置的接入优先级 */
        pstEvent->stSysCfg.enUserPrio = pstLastSyscfgSet->enPrioRat;

        if (STA_FSM_NULL == g_StatusContext.ulFsmState)
        {

            PS_MEM_SET(&stRoamFeature, 0x00, sizeof(stRoamFeature));

            NV_GetLength(en_NV_Item_Roam_Capa, &ulLength);
            if (NV_OK == NV_Read(en_NV_Item_Roam_Capa,
                                 &stRoamFeature,
                                 ulLength))
            {
                pstEvent->stSysCfg.ucRoam = stRoamFeature.ucRoamCapability;
                MMA_INFOLOG1("MMA_PhSysCfgReport: usRoamCapa", stRoamFeature.ucRoamCapability);
            }
            else
            {
                MMA_WARNINGLOG("MMA_PhSysCfgReport():WARNING:Read from NVIM en_NV_Item_Roam_Capa Fail");
                pstEvent->stSysCfg.ucRoam = TAF_PH_ROAM_NATIONAL_ON_INTERNATIONAL_OFF;
            }
        }

        /* 获取当前用户设置的服务域信息 */
        pstEvent->stSysCfg.ucSrvDomain = pstLastSyscfgSet->ucSrvDomain;

        if (STA_FSM_NULL == g_StatusContext.ulFsmState)
        {

            PS_MEM_SET(&stMsClass, 0x00, sizeof(NAS_NVIM_MS_CLASS_STRU));


            /* 明确在关机状态获取服务域信息 */
            NV_GetLength(en_NV_Item_MMA_MsClass, &ulLength);
            if (NV_OK != NV_Read(en_NV_Item_MMA_MsClass ,
                                 &stMsClass,
                                 ulLength))
            {
                MMA_WARNINGLOG("MMA_PhSysCfgReport():WARNING:Read from NVIM MSClass Fail");
                ucMsClassResult = TAF_PH_MS_CLASS_A;
            }
            else
            {
                ucMsClassResult = stMsClass.ucMsClass;
            }

            MMA_ChangeClass2Srv(&pstEvent->stSysCfg.ucSrvDomain, ucMsClassResult);
        }

        /* 获取当前产品支持的用户设置的LTE频段信息 */
#if (FEATURE_ON == FEATURE_LTE)
        MN_MMA_GetSupportedUserLteBand(&pstEvent->stSysCfg.stLBand);
#endif
    }

    /* 设置失败或查询事件上报调用客户端 */
    pstEvent->ClientId = ClientId;
    pstEvent->OpId = OpId;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(pstEvent->ClientId,(VOS_UINT8*)pstEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    PS_MEM_FREE(WUEPS_PID_MMA, pstEvent);

    return;
}

/* MMA_PhSysCfgHandle */

#if ( FEATURE_ON == FEATURE_LTE )
 VOS_UINT32 TAF_MMA_SendL4aMsg(
    VOS_UINT32                          MsgId,
    VOS_VOID                           *pData,
    VOS_UINT32                          uLen
)
{
    VOS_UINT32                          ulRet       = VOS_OK;
    MsgBlock                           *pMsgBlock   = NULL;

    pMsgBlock = (MsgBlock*)PS_ALLOC_MSG(WUEPS_PID_MMA, (uLen - VOS_MSG_HEAD_LENGTH));
    if(NULL == pMsgBlock)
    {
        return VOS_ERR;
    }

    VOS_MemCpy((VOS_UINT8*)pMsgBlock, (VOS_UINT8*)pData, uLen);

    /* 封装VOS消息 */
    pMsgBlock->ulReceiverPid = MSP_L4_L4A_PID;
    pMsgBlock->ulSenderPid = WUEPS_PID_MMA;

    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pMsgBlock);
    if(VOS_OK != ulRet)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR,
                "TAF_MMA_SendL4aMsg:ERROR: Fail to send message!");
    }

    return ulRet;
}
VOS_UINT32 TAF_MMA_SendL4aCsqInfoReq(
    VOS_UINT16                      ucClientId,
    VOS_INT8                        ucOpId
)
{
    VOS_UINT32 ulRet                = VOS_OK;

    /* L4A */
    L4A_CSQ_INFO_REQ_STRU stCsqReq;

    PS_MEM_SET(&stCsqReq, 0, sizeof(L4A_CSQ_INFO_REQ_STRU));

    /* 赋值 */
    stCsqReq.stCtrl.ulClientId      = ucClientId;
    stCsqReq.stCtrl.ulOpId          = 0;
    stCsqReq.stCtrl.ulPid           = WUEPS_PID_AT;
    stCsqReq.enMsgId                = ID_MSG_L4A_CSQ_INFO_REQ;

    /* 发送消息到L4A */
    ulRet = TAF_MMA_SendL4aMsg(ID_MSG_L4A_CSQ_INFO_REQ, (VOS_VOID*)(&stCsqReq), sizeof(stCsqReq));

    return ulRet;
}



VOS_UINT32 TAF_MMA_SendL4aCerssiInfoReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucNdb,
    VOS_UINT8                           ucMinRptTimerInterval
)
{
    VOS_UINT32                          ulRet;
    L4A_CSQ_INFO_REQ_STRU               stCsqReq;

    ulRet = VOS_OK;

    PS_MEM_SET(&stCsqReq, 0x0, sizeof(stCsqReq));

    stCsqReq.stCtrl.ulClientId      = usClientId;
    stCsqReq.stCtrl.ulOpId          = 0;
    stCsqReq.stCtrl.ulPid           = WUEPS_PID_AT;
    stCsqReq.ucnDb                  = ucNdb;
    stCsqReq.ucMinRptTimerInterval  = ucMinRptTimerInterval;
    stCsqReq.enMsgId                = ID_MSG_L4A_CERSSI_REQ;

    ulRet = TAF_MMA_SendL4aMsg(ID_MSG_L4A_CERSSI_REQ, (VOS_VOID*)(&stCsqReq), sizeof(stCsqReq));

    return ulRet;
}
VOS_UINT32 TAF_MMA_SendL4aAnqueryInfoReq(
    VOS_UINT16                      ucClientId,
    VOS_INT8                        ucOpId
)
{
    VOS_UINT32 ulRet                = VOS_OK;

    /* L4A */
    L4A_CSQ_INFO_REQ_STRU stCsqReq;

    PS_MEM_SET(&stCsqReq, 0, sizeof(L4A_CSQ_INFO_REQ_STRU));

    /* 赋值 */
    stCsqReq.stCtrl.ulClientId      = ucClientId;
    stCsqReq.stCtrl.ulOpId          = 0;
    stCsqReq.stCtrl.ulPid           = WUEPS_PID_AT;
    stCsqReq.enMsgId                = ID_MSG_L4A_ANQUERY_INFO_REQ;

    /* 发送消息到L4A */

    ulRet = TAF_MMA_SendL4aMsg(ID_MSG_L4A_ANQUERY_INFO_REQ, (VOS_VOID*)(&stCsqReq), sizeof(stCsqReq));

    return ulRet;
}
#endif


VOS_VOID MMA_PhoneGetRssi (
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId
)
{
    VOS_UINT32                           ulI;
    TAF_PH_RSSI_STRU                     stMmaRssiTmp;

    VOS_UINT32                          i;

    PS_MEM_SET(&stMmaRssiTmp, 0, sizeof(stMmaRssiTmp));


#if ( FEATURE_ON == FEATURE_LTE )
    if (TAF_SDC_SYS_MODE_LTE == TAF_SDC_GetSysMode())
    {
        /*调用L模接口，由该接口转发消息给L4A获取小区信号，再由L4A回复消息给AT。*/
        if (VOS_OK != TAF_MMA_SendL4aCsqInfoReq(ClientId,(VOS_INT8)OpId) )
        {
            MMA_ParaQueryReport(ClientId, OpId, TAF_PH_RSSI_VALUE_PARA,TAF_ERR_AT_ERROR, VOS_NULL_PTR);
        }

        return ;
    }
#endif

    /*获取当前RSSI的值,调用API*/
    gstMmaValue.stRssiValue.ucRssiNum = gstMmaValue.stCerssiValue.ucRssiNum;

    for (i = 0; i < gstMmaValue.stCerssiValue.ucRssiNum; i++)
    {
        if (TAF_SDC_SYS_MODE_GSM == TAF_SDC_GetSysMode())
        {
            gstMmaValue.stRssiValue.astRssi[i].lRssiValue = gstMmaValue.stCerssiValue.aRssi[i].u.stGCellSignInfo.sRssiValue;
        }
        else
        {
            gstMmaValue.stRssiValue.astRssi[i].lRssiValue = gstMmaValue.stCerssiValue.aRssi[i].u.stWCellSignInfo.sRscpValue;
        }

        gstMmaValue.stRssiValue.astRssi[i].ucChannalQual  = gstMmaValue.stCerssiValue.aRssi[i].ucChannalQual;
        gstMmaValue.stRssiValue.astRssi[i].ucRssiValue4AT = 0;
    }


    /*获取RSSI信息*/
    /* usParalen = 0; */

    /*准备数据*/
    for ( ulI = 0; ulI < gstMmaValue.stRssiValue.ucRssiNum; ulI++ )
    {
        MMA_GetRssiLevelFromValue(gstMmaValue.stRssiValue.astRssi[ulI].lRssiValue,
                                  &(stMmaRssiTmp.aRssi[ulI].ucRssiValue));
        stMmaRssiTmp.aRssi[ulI].ucChannalQual = gstMmaValue.stRssiValue.astRssi[ulI].ucChannalQual;
    }

    /*填入当前上报小区信号强度的小区个数*/
    stMmaRssiTmp.ucRssiNum = gstMmaValue.stRssiValue.ucRssiNum;

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_RSSI_VALUE_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stMmaRssiTmp);

    return;
}
VOS_VOID MMA_GetPhCellRscp (VOS_UINT16          ClientId,
                           VOS_UINT8                 OpId)
{
    TAF_PH_CELL_RSSI_STRU               stMmaRssiTmp;
    VOS_UINT8                           ucRscpLevelTmp;

    stMmaRssiTmp.CellNum    = gstMmaValue.stCerssiValue.ucRssiNum;
    stMmaRssiTmp.CellDlFreq = gstMmaValue.stCerssiValue.usCellDlFreq;

    if (TAF_SDC_SYS_MODE_GSM == TAF_SDC_GetSysMode())
    {
        MMA_GetRssiLevelFromValue(gstMmaValue.stCerssiValue.aRssi[0].u.stGCellSignInfo.sRssiValue,
                                      &ucRscpLevelTmp);
        stMmaRssiTmp.CellRscp[0].CellRSCP = ucRscpLevelTmp;
    }
    else
    {
        MMA_GetRssiLevelFromValue(gstMmaValue.stCerssiValue.aRssi[0].u.stWCellSignInfo.sRscpValue,
                                      &ucRscpLevelTmp);
        stMmaRssiTmp.CellRscp[0].CellRSCP = ucRscpLevelTmp;
    }

    stMmaRssiTmp.CellRscp[0].CellId = TAF_SDC_GetCurrCampCellId();


    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_CELL_RSCP_VALUE_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stMmaRssiTmp);

    return;
}
VOS_VOID MMA_GetPhUeRfPowerFreq(VOS_UINT16          ClientId,
                            VOS_UINT8           OpId)
{
    TAF_PH_UE_RFPOWER_FREQ_STRU         stUeRfPowerFreq;

    stUeRfPowerFreq.CellUlFreq = gstMmaValue.stCerssiValue.usCellUlFreq;
    stUeRfPowerFreq.UeRfPower  = gstMmaValue.stCerssiValue.sUeRfPower;


    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_UE_RFPOWER_FREQ_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stUeRfPowerFreq);

    return;
}
VOS_UINT32 MMA_PhoneRssiInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent;
    VOS_UINT32                          ulI;
    MMC_MMA_RSSI_IND_STRU              *pMsg;

    pMsg = (MMC_MMA_RSSI_IND_STRU*)pstMsg;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MMA_PhoneRssiInd:ERROR:ALLOC MEMORY FAIL.");
        return VOS_TRUE;
    }
    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));
    /*填充事件*/
    MMA_InitEventInfoOP(pstPhoneEvent);
    /* BEGIN: Modified by liuyang id:48197, 2006/3/16   PN:A32D02382*/
    pstPhoneEvent->OP_Rssi                       = MMA_TRUE;
    /* END:   Modified by liuyang id:48197, 2006/3/16 */

    pstPhoneEvent->RssiValue.ucRssiNum         = pMsg->ucRssiNum;

    gstMmaValue.stCerssiValue.ucRssiNum        = pMsg->ucRssiNum;

    pstPhoneEvent->RssiValue.enRatType          = TAF_SDC_GetSysMode();
    pstPhoneEvent->RssiValue.ucCurrentUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();

    for ( ulI = 0; ulI < pstPhoneEvent->RssiValue.ucRssiNum; ulI++ )
    {
        pstPhoneEvent->RssiValue.aRssi[ulI].ucChannalQual = pMsg->astRssi[ulI].ucBer;

        gstMmaValue.stCerssiValue.aRssi[ulI].ucChannalQual = pMsg->astRssi[ulI].ucBer;

        /* RSCP/ECIO/RSSI存在如下的对应关系：RSCP=ECIO+RSSI*/
        gstMmaValue.stCerssiValue.sUeRfPower   = pMsg->sUeRfPower;
        gstMmaValue.stCerssiValue.usCellDlFreq = pMsg->usCellDlFreq;
        gstMmaValue.stCerssiValue.usCellUlFreq = pMsg->usCellUlFreq;

        if (TAF_SDC_SYS_MODE_GSM == TAF_SDC_GetSysMode())
        {
            pstPhoneEvent->RssiValue.aRssi[ulI].u.stGCellSignInfo.sRssiValue = pMsg->astRssi[ulI].sRssiValue;
            MMA_GetRssiLevelFromValue(pMsg->astRssi[ulI].sRssiValue,
                                     &(pstPhoneEvent->RssiValue.aRssi[ulI].ucRssiValue));

            gstMmaValue.stCerssiValue.aRssi[ulI].u.stGCellSignInfo.sRssiValue = pMsg->astRssi[ulI].sRssiValue;
        }
        else
        {
            pstPhoneEvent->RssiValue.aRssi[ulI].u.stWCellSignInfo.sRscpValue = pMsg->astRssi[ulI].sRscpValue;
            pstPhoneEvent->RssiValue.aRssi[ulI].u.stWCellSignInfo.sEcioValue = pMsg->astRssi[ulI].sRscpValue - pMsg->astRssi[ulI].sRssiValue;

            gstMmaValue.stCerssiValue.aRssi[ulI].u.stWCellSignInfo.sRscpValue = pMsg->astRssi[ulI].sRscpValue;
            gstMmaValue.stCerssiValue.aRssi[ulI].u.stWCellSignInfo.sEcioValue = pMsg->astRssi[ulI].sRscpValue - pMsg->astRssi[ulI].sRssiValue;

            MMA_GetRssiLevelFromValue(pMsg->astRssi[ulI].sRscpValue,
                                 &(pstPhoneEvent->RssiValue.aRssi[ulI].ucRssiValue));
        }

        gstMmaValue.stCerssiValue.aRssi[ulI].ucRssiValue = pstPhoneEvent->RssiValue.aRssi[ulI].ucRssiValue;
    }

    pstPhoneEvent->PhoneEvent                    = TAF_PH_EVT_RSSI_CHANGE_IND;
    /*上报事件*/
    if (MMA_MMC_SCELL_MEAS_TYPE_RXLEV == (pMsg->unMeasReportType.ucMeasReportType & MMA_MMC_SCELL_MEAS_TYPE_RXLEV))
    {
        Taf_PhoneEvent(pstPhoneEvent);
    }

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return VOS_TRUE;
}


VOS_UINT32 MMA_ConvertPhonePinType(TAF_PH_PIN_TYPE *penPinType)
{
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 enSimLockResult;
    VOS_UINT32                          ulResult;
    MODEM_ID_ENUM_UINT16                enModemId;

    enModemId  = VOS_GetModemIDFromPid(WUEPS_PID_MMA);
    /* 进行锁卡校验，若校验函数返回失败，则按照当前锁在PH_NET_PIN来进行异常处理 */
    if (SC_ERROR_CODE_NO_ERROR != SC_PERS_VerifySimLock(&enSimLockResult, enModemId))
    {
        *penPinType = TAF_PHNET_PIN;
        return VOS_OK;
    }

    ulResult = VOS_OK;
    switch (enSimLockResult)
    {
        case SC_VERIFY_SIMLOCK_RESULT_PH_NET_PIN:
            *penPinType = TAF_PHNET_PIN;
            break;
        case SC_VERIFY_SIMLOCK_RESULT_PH_NET_PUK:
            *penPinType = TAF_PHNET_PUK;
            break;
        case SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PIN:
            *penPinType = TAF_PHNETSUB_PIN;
            break;
        case SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PUK:
            *penPinType = TAF_PHNETSUB_PUK;
            break;
        case SC_VERIFY_SIMLOCK_RESULT_PH_SP_PIN:
            *penPinType = TAF_PHSP_PIN;
            break;
        case SC_VERIFY_SIMLOCK_RESULT_PH_SP_PUK:
            *penPinType = TAF_PHSP_PUK;
            break;
        case SC_VERIFY_SIMLOCK_RESULT_READY:
            /* SIM卡未被锁定，则返回锁网锁卡校验通过 */
            break;
        default:
            /* 异常，则返回锁卡校验失败 */
            ulResult = VOS_ERR;
            break;
    }

    return ulResult;
}
VOS_VOID MMA_PinQueryHandle(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_PH_PIN_CMD_TYPE                 ucPinCmdType,
    TAF_PH_PIN_CNF_STRU                *pstPinCnf
)
{
    VOS_UINT32                          ulResult;
    TAF_PH_ME_LOCK_STATUS               ucLockStatus;

    /*若是查询PIN状态，走此分支，通过调用usim提供的查询API返回*/
    pstPinCnf->OpPinResult = (VOS_UINT8)NAS_USIMMAPI_GetPinStatus(&(pstPinCnf->PinType),
                                                            &(pstPinCnf->QueryResult.UsimmEnableFlg),
                                                            &(pstPinCnf->QueryResult.UsimmNeedType),
                                                            &(pstPinCnf->ucRemain));
    NAS_USIMMAPI_GetPinRemainTime((USIMM_PIN_REMAIN_STRU *)(&(pstPinCnf->RemainTime)));


    ucLockStatus = MMA_GetMeLockStatus();

    /* 若上层应用为AP-Modem，且SIM卡状态为READY(非TAF_SIM_PIN或TAF_SIM_PUK)，且为AT^CPIN或AT+CPIN查询，则需增加当前锁卡状态的判断逻辑 */
    if ( (SYSTEM_APP_ANDROID == g_usMmaAppConfigSupportType)
      && (TAF_PIN_QUERY == ucPinCmdType)
      && (TAF_PH_ME_SIM_LOCK == ucLockStatus) )
    {
        /* 若从USIMM获取的卡状态为PIN或PUK状态，则直接返回，不进行锁卡校验 */
        if ((TAF_SIM_PIN != pstPinCnf->PinType) && (TAF_SIM_PUK != pstPinCnf->PinType))
        {
            ulResult = MMA_ConvertPhonePinType(&(pstPinCnf->PinType));
            /* 出错，+CPIN命令返回ERROR */
            if(VOS_OK != ulResult)
            {
                MMA_HandleEventError(usClientId, ucOpId, TAF_ERR_AT_ERROR, TAF_PH_EVT_OP_PIN_CNF);
                return;
            }
        }
    }

    /*查询API直接返回需要的值，调用上报函数上报*/
    MMA_PinDataReport( pstPinCnf );

    return;
}
VOS_VOID MMA_PinHandle (VOS_UINT16 ClientId, VOS_UINT8 OpId,
                        TAF_PH_PIN_DATA_STRU stPinData)
{
    TAF_PH_PIN_CNF_STRU stPinTmp;
    TAF_PH_PIN_TIMECNF_STRU stPinRemainTime;
    VOS_UINT8           ucTi;
    VOS_UINT32          ulRet;
    VOS_UINT8           ucError = TAF_ERR_NO_ERROR;
    VOS_UINT32          ulResult;
    VOS_UINT8           ucSimType;    
    
    TAF_SDC_USIM_STATUS_ENUM_UINT8  enUsimStatus;

    enUsimStatus = TAF_SDC_GetSimStatus();

    stPinTmp.CmdType  = stPinData.CmdType;
    stPinTmp.PinType  = stPinData.PinType;
    stPinTmp.ucRemain = MMA_PIN_REMAINS_UNINIT;

    /*增加PIN码有效性判断*/
    
    if (TAF_SDC_USIM_STATUS_NO_PRESENT == enUsimStatus)
    {
        MMA_WARNINGLOG("MMA_PinHandle():WARNING:sim busy!");
        MMA_HandleEventError(ClientId,
                             OpId,
                             TAF_ERR_USIM_SIM_CARD_NOTEXIST,
                             TAF_PH_EVT_OP_PIN_CNF );
        return;
    }
    
    if ( (TAF_SDC_USIM_STATUS_READY_TIMEROUT == enUsimStatus)
      || (TAF_SDC_USIM_STATUS_BUTT == enUsimStatus))
    {
        MMA_WARNINGLOG("MMA_PinHandle():WARNING:sim busy!");
        MMA_HandleEventError(ClientId,
                             OpId,
                             TAF_ERR_SIM_BUSY,
                             TAF_PH_EVT_OP_PIN_CNF );
        return;
    }    
    
    
    if ( MMA_SUCCESS != Mma_CheckPin(stPinData))
    {
        MMA_WARNINGLOG("MMA_PinHandle():WARNING:INPUT PIN is Error!");
        MMA_HandleEventError(ClientId,
                             OpId,
                             TAF_ERR_PARA_ERROR,
                             TAF_PH_EVT_OP_PIN_CNF );
        return;
    }

    /*获取当前USIM状态*/
    if ( MMA_SUCCESS != MMA_PhoneUsimReady(MMA_CLIENTID_NONE, MMA_OP_ID_INTERNAL, TAF_TELE_PARA_BUTT))
    {
        /*打印消息*/
        MMA_WARNINGLOG("MMA_PinHandle():WARNING: usim NOT ready!");
        /*生成错误数据包*/
        /*返回获取数据无效*/
        /*返回操作失败*/
        if ((TAF_PIN_VERIFY == stPinTmp.CmdType)
         || (TAF_PIN_UNBLOCK == stPinTmp.CmdType))
        {
            MMA_HandleEventError(ClientId,
                                 OpId,
                                 TAF_ERR_CMD_TYPE_ERROR,
                                 TAF_PH_EVT_OP_PIN_CNF );
        }
        else
        {
            MMA_HandleEventError(ClientId,
                                 OpId,
                                 TAF_ERR_SIM_FAIL,
                                 TAF_PH_EVT_OP_PIN_CNF );
        }
        return;
    }

    if ( TAF_PIN_DISABLE == stPinTmp.CmdType )
    {
        ulRet = MMA_PhoneGetUsimType( &ucSimType );
        if ( (MMA_SUCCESS       == ulRet  )
           &&(MMA_USIM_TYPE_SIM == ucSimType)
           )
        {
            /*检查SST文件判断是否可以DISABLE*/
            ulResult = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_CHV1_DISABLE);

            if (MMA_TRUE != ulResult)
            {
                /*打印消息*/
                MMA_WARNINGLOG("MMA_PinHandle():WARNING: GET SST FAIL, OR Service not allowed!");
                ucError = TAF_ERR_UNSPECIFIED_ERROR;
            }
        }
        else if ( MMA_SUCCESS       != ulRet )
        {
            /*打印消息*/
            MMA_WARNINGLOG("MMA_PinHandle():WARNING: Get uicc Tpye Fail!");
            ucError = TAF_ERR_UNSPECIFIED_ERROR;
        }
        else
        {
            MMA_NORMAILLOG("MMA_PinHandle():NORMAL: Usim return no error!");
        }
        if ( TAF_ERR_NO_ERROR != ucError )
        {
            MMA_HandleEventError(ClientId,
                                 OpId,
                                 ucError,
                                 TAF_PH_EVT_OP_PIN_CNF );
            return;
        }
    }

    /*填入对PIN操作的全局变量供返回函数查询，存储CLIENTID*/
    /*记录当前clientId与OpId*/
    if (MMA_SUCCESS != MMA_TiAlloc(TAF_MMA, ClientId, OpId, TAF_PH_EVT_OP_PIN_CNF, &ucTi))
    {   /*Ti分配失败*/
        MMA_WARNINGLOG("MMA_PinHandle():WARNING:ALLOC TI FAILED!");
        /*错误处理*/
        MMA_HandleEventError(ClientId, OpId, TAF_ERR_TI_ALLOC_FAIL, TAF_PH_EVT_OP_PIN_CNF);
        return;
    }
    gstMmaValue.stOpPinData.ClientId        = ClientId;
    gstMmaValue.stOpPinData.OpId            = OpId;
    gstMmaValue.stOpPinData.stMmaPinData    = stPinData;

    if ((TAF_PIN_QUERY == stPinData.CmdType) || (TAF_PIN2_QUERY == stPinData.CmdType))
    {
        MMA_PinQueryHandle(ClientId, OpId, stPinData.CmdType, &stPinTmp);
    }
    else if(TAF_PIN_RemainTime == stPinData.CmdType)
    {
        stPinRemainTime.CmdType = TAF_PIN_RemainTime;

        stPinRemainTime.PinType = USIMM_PIN;

        stPinRemainTime.OpPinResult =  (VOS_UINT8)NAS_USIMMAPI_GetPinStatus(&(stPinRemainTime.PinType),
                                                       &(stPinRemainTime.QueryResult.UsimmEnableFlg),
                                                       &(stPinRemainTime.QueryResult.UsimmNeedType),
                                                       &(stPinRemainTime.ucPIN1Remain));    /* Read PIN1 remain time */

        stPinRemainTime.PinType = USIMM_PUK;

        stPinRemainTime.OpPinResult =  (VOS_UINT8)NAS_USIMMAPI_GetPinStatus(&(stPinRemainTime.PinType),
                                                       &(stPinRemainTime.QueryResult.UsimmEnableFlg),
                                                       &(stPinRemainTime.QueryResult.UsimmNeedType),
                                                       &(stPinRemainTime.ucPUK1Remain));    /* Read PUK1 remain time */

        stPinRemainTime.PinType = USIMM_PIN2;

        stPinRemainTime.OpPinResult =  (VOS_UINT8)NAS_USIMMAPI_GetPinStatus(&(stPinRemainTime.PinType),
                                                       &(stPinRemainTime.QueryResult.UsimmEnableFlg),
                                                       &(stPinRemainTime.QueryResult.UsimmNeedType),
                                                       &(stPinRemainTime.ucPIN2Remain));    /* Read PUK1 remain time */

        stPinRemainTime.PinType = USIMM_PUK2;

        stPinRemainTime.OpPinResult =  (VOS_UINT8)NAS_USIMMAPI_GetPinStatus(&(stPinRemainTime.PinType),
                                                       &(stPinRemainTime.QueryResult.UsimmEnableFlg),
                                                       &(stPinRemainTime.QueryResult.UsimmNeedType),
                                                       &(stPinRemainTime.ucPUK2Remain));    /* Read PUK1 remain time */

        MMA_PhonePinRemainTimeReport( &stPinRemainTime );
    }
    else
    {
        /*对USIM操作实现*/
        /*启动定时器*/
        if( VOS_OK != NAS_StartRelTimer( &(g_MmaSimTimer[0].MmaHTimer),
                                         WUEPS_PID_MMA,
                                         MMA_PIN_HANDLE_TIME,
                                         TAF_MMA,
                                         TAF_USIM_OPPIN,
                                         VOS_RELTIMER_NOLOOP ) )
        {
            MMA_ERRORLOG("MMA_PinHandle():ERROR:Start Timer Error!");
            /*返回操作失败*/
            /*错误处理*/
            MMA_HandleEventError(ClientId, OpId, TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_EVT_OP_PIN_CNF);
            g_MmaSimTimer[0].ucTimerStatus     = MMA_TIMER_ERROR;
            return;
        }
        g_MmaSimTimer[0].ucTimerStatus         = MMA_TIMER_RUN;

        /*查询系统当前需要验证的PIN码类型,输入参数除了PIN类型以外其它参数无意义*/
        NAS_USIMMAPI_GetPinStatus(&(stPinData.PinType),
                              &stPinTmp.QueryResult.UsimmEnableFlg,
                              &stPinTmp.QueryResult.UsimmNeedType,
                              &stPinTmp.ucRemain);
        /*调用USIM提供的操作，对PIN进行操作*/
        /* BEGIN: Modified by liuyang id:48197, 2006/3/16   PN:A32D02412*/

        if(USIMM_API_SUCCESS != NAS_USIMMAPI_PinReq(WUEPS_PID_MMA,
                                            stPinData.CmdType,
                                            stPinData.PinType,
                                            stPinData.aucOldPin,
                                            stPinData.aucNewPin))

        {
            MMA_WARNINGLOG("MMA_PinHandle():WARNING:Invoke USIMM Function Fail!");
            /*停止定时器*/

            if (VOS_OK != NAS_StopRelTimer(WUEPS_PID_MMA, TAF_USIM_OPPIN, &(g_MmaSimTimer[0].MmaHTimer)))
            {
                MMA_WARNINGLOG("MMA_PinHandle():WARNING:NAS_StopRelTimer failed!");
            }

            /*设置停止定时器标志*/
            g_MmaSimTimer[0].ucTimerStatus    = MMA_TIMER_STOP;
            /*上报错误*/
            MMA_HandleEventError(ClientId, OpId, TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_EVT_OP_PIN_CNF);
            MMA_WARNINGLOG("MMA_PinHandle():WARNING:invoke USIM Api Fail!");

            return;
        }

    }
    return;
}

VOS_UINT32 MMA_VerifyOperatorLockPwd(VOS_UINT8 *pucPwd)
{
    VOS_UINT8   aucTmpImei[TAF_PH_IMEI_LEN];

    PS_MEM_CPY(aucTmpImei, g_aucMmaImei, TAF_PH_IMEI_LEN-1);

    aucTmpImei[TAF_PH_IMEI_LEN -1] = 0;


    if (VOS_FALSE == DRV_CARDLOCK_MD5_VERIFY((char *)pucPwd, (char *)aucTmpImei))
    {
        return VOS_ERR;
    }

    return VOS_OK;


}

VOS_VOID MMA_MePersonalCnfReport(VOS_UINT16                        ClientId,
                                VOS_UINT8                         OpId,
                                TAF_PH_ME_PERSONLISATION_CNF_STRU stMePersonalCnf)
{
    TAF_PHONE_EVENT_INFO_STRU   *pstPhoneEvent;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MMA_MePersonalCnfReport:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    pstPhoneEvent->ClientId = ClientId;
    pstPhoneEvent->OpId = OpId;
    pstPhoneEvent->PhoneEvent = TAF_PH_EVT_ME_PERSONALISATION_CNF;

    pstPhoneEvent->MePersonalisation = stMePersonalCnf;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

}
VOS_UINT32 MMA_CheckMePersonalisationPwd(TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData)
{
    VOS_UINT8       aucPwd[TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX +1];
    VOS_UINT32      ulRslt;
    VOS_UINT16      usDataLen = TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX;


    PS_MEM_SET(aucPwd, 0x00, sizeof(aucPwd));

    switch(stMePersonalData.MePersonalType)
    {
        case TAF_SIM_PERSONALISATION:
            ulRslt = NV_Read(en_NV_Item_Sim_Personalisation_Pwd,
                             aucPwd,
                             usDataLen);

            if (NV_OK != ulRslt)
            {
                return TAF_PH_ME_PERSONALISATION_ERROR;
            }
            PS_MEM_CPY(g_MmaNewValue.aucPwd, aucPwd, TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX);

            ulRslt = (VOS_UINT32)VOS_MemCmp(stMePersonalData.aucOldPwd,
                                aucPwd,
                                TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX);
            if (VOS_OK != ulRslt)
            {
                return TAF_PH_ME_PERSONALISATION_WRONG_PWD;
            }
            break;
        case TAF_OPERATOR_PERSONALISATION:
            if (VOS_OK != MMA_VerifyOperatorLockPwd(stMePersonalData.aucOldPwd))
            {
                return TAF_PH_ME_PERSONALISATION_WRONG_PWD;
            }
            break;
        default:
            return TAF_PH_ME_PERSONALISATION_WRONG_PARA;
    }

    return TAF_PH_ME_PERSONALISATION_OK;
}




VOS_VOID MMA_MePersonlisationSet(VOS_UINT16 ClientId,
                                VOS_UINT8 OpId,
                                TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData)
{
    TAF_PH_ME_PERSONLISATION_CNF_STRU stMePersonalCnf;
    VOS_UINT32          ulResult;
    TAF_UINT8           aucSimPersonalisationStr[TAF_MAX_IMSI_LEN + 1];


    PS_MEM_SET(&stMePersonalCnf, 0x00, sizeof(stMePersonalCnf));

    stMePersonalCnf.CmdType  = stMePersonalData.CmdType;
    stMePersonalCnf.MePersonalisatonType  = stMePersonalData.MePersonalType;

    switch(stMePersonalData.MePersonalType)
    {
        case TAF_SIM_PERSONALISATION:
            /*检查密码是否正确*/
            ulResult = MMA_CheckMePersonalisationPwd(stMePersonalData);


            /*密码正确，则设置当前的IMSI String*/
            if (TAF_PH_ME_PERSONALISATION_OK == ulResult)
            {
                aucSimPersonalisationStr[0] = (VOS_UINT8)stMePersonalData.unMePersonalisationSet.SimPersionalisationStr.DataLen;
                VOS_MemCpy(aucSimPersonalisationStr + 1,
                           stMePersonalData.unMePersonalisationSet.SimPersionalisationStr.aucSimPersonalisationStr,
                           aucSimPersonalisationStr[0]);

                ulResult = NV_Write(en_NV_Item_Sim_Personalisation_Imsi_Str,
                                    aucSimPersonalisationStr,
                                    aucSimPersonalisationStr[0] + 1);

                if (NV_OK != ulResult)
                {
                    ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
                }
            }
            break;

        default:
            ulResult = TAF_PH_ME_PERSONALISATION_WRONG_PARA;
            break;
    }

    stMePersonalCnf.OpRslt = (VOS_UINT8)ulResult;

    MMA_MePersonalCnfReport(ClientId, OpId, stMePersonalCnf);

}

VOS_VOID TAF_UsimmStatusCardUnlocked()
{
    VOS_UINT8                           ucUsimStatus;
    VOS_UINT8                           ucUsimType;
    PS_USIM_STATUS_IND_STRU * pUsimMsg = VOS_NULL_PTR;


    ucUsimStatus = USIMM_CARD_SERVIC_BUTT;
    ucUsimType   = USIMM_CARD_NOCARD;

    MMA_GetUsimStatusFromUsim(&ucUsimStatus,&ucUsimType);

    if (USIMM_CARD_SERVIC_AVAILABLE != ucUsimStatus)
    {
        return;
    }
    pUsimMsg = (PS_USIM_STATUS_IND_STRU *)PS_ALLOC_MSG(MAPS_PIH_PID, sizeof(PS_USIM_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        return;
    }

    MMA_MEM_SET(pUsimMsg,0x00,sizeof(PS_USIM_STATUS_IND_STRU));

    pUsimMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;

    pUsimMsg->ulSenderPid     = MAPS_PIH_PID;

    pUsimMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pUsimMsg->ulReceiverPid   = WUEPS_PID_MMA;
    pUsimMsg->ucIMSILen       = USIMM_EF_IMSI_LEN;
    pUsimMsg->enCardStatus    = ucUsimStatus;
    pUsimMsg->enCardType      = ucUsimType;
    pUsimMsg->ulMsgName       = PS_USIM_GET_STATUS_IND;
    if (USIMM_API_SUCCESS != NAS_USIMMAPI_GetCardIMSI(pUsimMsg->aucIMSI))
    {
        NAS_WARNING_LOG(MAPS_PIH_PID, "TAF_UsimmStatusCardUnlocked():WARNING:NAS_USIMMAPI_GetCardIMSI FIAL");
    }

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_USIM, pUsimMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_USIM, "TAF_UsimmStatusCardUnlocked():WARNING:SEND MSG FIAL");
    }

    return;
}


VOS_UINT32   MMA_OperatorPersonlisationVerify(
TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucRemainTimes;
    VOS_UINT8                           ucLockStatus;

    TAF_CUSTOM_CARDLOCK_STATUS_STRU     stCardLockStatus;
    TAF_CUSTOM_SIM_LOCK_MAX_TIMES_STRU  stSimLockMaxTimes;


    PS_MEM_SET(&stSimLockMaxTimes, 0x00, sizeof(stSimLockMaxTimes));
    PS_MEM_SET(&stCardLockStatus,  0x00, sizeof(stCardLockStatus));

    /* 获取当前锁卡状态和剩余次数 */
    ulResult = NV_Read(en_NV_Item_CardlockStatus,
                       &stCardLockStatus,
                       sizeof(stCardLockStatus));
    if (NV_OK != ulResult)
    {
        MMA_ERRORLOG("MMA_OperatorPersonlisationVerify:ERROR:Read en_NV_Item_CardlockStatus Fail.");
        return TAF_PH_ME_PERSONALISATION_OP_NOT_ALLOW;
    }
    g_MmaNewValue.stCardLockStatus = stCardLockStatus;

    ulResult = NV_Read(en_NV_Item_CustomizeSimLockMaxTimes,
                      &stSimLockMaxTimes,
                      sizeof(stSimLockMaxTimes));
    if (NV_OK != ulResult)
    {
        g_MmaNewValue.stSimLockMaxTimes.ulStatus = VOS_FALSE;
        g_MmaNewValue.stSimLockMaxTimes.ulLockMaxTimes = TAF_PH_CARDLOCK_DEFAULT_MAXTIME;
    }
    else
    {
        if (VOS_FALSE == stSimLockMaxTimes.ulStatus)
        {
            g_MmaNewValue.stSimLockMaxTimes.ulLockMaxTimes = TAF_PH_CARDLOCK_DEFAULT_MAXTIME;
        }
        else
        {
            g_MmaNewValue.stSimLockMaxTimes = stSimLockMaxTimes;
        }
    }

    if (VOS_FALSE == stCardLockStatus.ulStatus)
    {
        MMA_ERRORLOG("MMA_OperatorPersonlisationVerify:ERROR:Status no correct.");
        return TAF_PH_ME_PERSONALISATION_OP_NOT_ALLOW;
    }

    ucLockStatus = (VOS_UINT8)stCardLockStatus.enCardlockStatus;
    ucRemainTimes  = (VOS_UINT8)stCardLockStatus.ulRemainUnlockTimes;

    /* 已经处于永久锁定状态不允许校验  */

    if (( ( TAF_OPERATOR_UNLOCK_TIMES_MIN == ucRemainTimes )
           &&( TAF_OPERATOR_LOCK_LOCKED == ucLockStatus))
          ||(TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE == ucLockStatus) )
    {
        ulResult = TAF_PH_ME_PERSONALISATION_OP_NOT_ALLOW;
        MMA_ERRORLOG("MMA_OperatorPersonlisationVerify:card locked already.");
    }
    else
    {
        /* 检查密码是否正确 */
        ulResult = MMA_CheckMePersonalisationPwd(stMePersonalData);

        /* 密码正确 */
        if (TAF_PH_ME_PERSONALISATION_OK == ulResult)
        {
            MMA_INFOLOG("MMA_OperatorPersonlisationVerify:INFO:MMA_CheckMePersonalisationPwd Return OK.");
            /* g_stMmaMePersonalisationStatus 数据更新后移 */



            /* 一旦校验通过，将永久解除锁卡状态  */
            stCardLockStatus.enCardlockStatus = TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE;
            stCardLockStatus.ulRemainUnlockTimes = stSimLockMaxTimes.ulLockMaxTimes;

            ulResult = NV_Write(en_NV_Item_CardlockStatus,
                                &stCardLockStatus,
                                sizeof(stCardLockStatus));
            if (NV_OK != ulResult)
            {
                MMA_ERRORLOG("MMA_OperatorPersonlisationVerify:ERROR:write en_NV_Item_CardlockStatus Fail.");
                return TAF_PH_ME_PERSONALISATION_ERROR;
            }

            /*nv写成功时才对其备份*/
            ulResult = NV_SpecialNvIdBackup(en_NV_Item_CardlockStatus,
                                           &stCardLockStatus,
                                           sizeof(stCardLockStatus));
            if (NV_OK != ulResult)
            {
                MMA_WARNINGLOG("MMA_OperatorPersonlisationVerify: WARNING:write Special en_NV_Item_CardlockStatus Fail.");
                return TAF_PH_ME_PERSONALISATION_ERROR;
            }

            /* 解锁卡后触发锁网锁卡状态变化指示消息 */
            TAF_MMA_SndSimlocakStatusChangeInd();

            /* 解除锁卡状态 */
            g_stMmaMePersonalisationStatus.SimLockStatus
                                 = MMA_SIM_LOCK_IS_VERIFY;

            g_stMmaMePersonalisationStatus.stCardLockInfo.RemainTimes
                                 = stSimLockMaxTimes.ulLockMaxTimes;

            ulResult = TAF_PH_ME_PERSONALISATION_OK;
        }
        else if (TAF_PH_ME_PERSONALISATION_WRONG_PWD == ulResult)
        {
            MMA_WARNINGLOG("MMA_OperatorPersonlisationVerify:WARNING:MMA_CheckMePersonalisationPwd Return error.");
            stCardLockStatus.ulRemainUnlockTimes--;
            g_stMmaMePersonalisationStatus.stCardLockInfo.RemainTimes
                                    = ucRemainTimes;

            if (TAF_OPERATOR_UNLOCK_TIMES_MIN == stCardLockStatus.ulRemainUnlockTimes)
            {
                stCardLockStatus.enCardlockStatus = TAF_OPERATOR_LOCK_LOCKED;
            }

            /* 更新RemainTimes和锁定状态到NV中 */
            ulResult = NV_Write(en_NV_Item_CardlockStatus,
                         &stCardLockStatus,
                         sizeof(stCardLockStatus));
            if (NV_WRITE_SECURE_FAIL == ulResult)
            {
                /* 如果解锁失败，恢复原来的值 */
                g_stMmaMePersonalisationStatus.stCardLockInfo.RemainTimes++;
            }



            if (NV_OK == ulResult)
            {
                /*nv写成功时才对其备份*/

                ulResult = NV_SpecialNvIdBackup(en_NV_Item_CardlockStatus,
                                               &stCardLockStatus,
                                               sizeof(stCardLockStatus));
                if (NV_OK != ulResult)
                {
                    MMA_WARNINGLOG("MMA_OperatorPersonlisationVerify: WARNING:write en_NV_Item_CardlockStatus Fail.");
                }
            }


            ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
        }
        else
        {
            ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
        }
    }

    return ulResult;
}


VOS_VOID MMA_MePersonlisationVerify(VOS_UINT16   ClientId,
                                   VOS_UINT8    OpId,
                                   TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData)
{
    TAF_PH_ME_PERSONLISATION_CNF_STRU   stMePersonalCnf;
    VOS_UINT32                          ulResult;


    PS_MEM_SET(&stMePersonalCnf, 0x00, sizeof(stMePersonalCnf));

    stMePersonalCnf.CmdType  = stMePersonalData.CmdType;
    stMePersonalCnf.MePersonalisatonType  = stMePersonalData.MePersonalType;

    ulResult = MMA_GetMeLockStatus();

    if (TAF_PH_ME_SIM_LOCK == ulResult)
    {
        switch(stMePersonalData.MePersonalType)
        {
            case TAF_SIM_PERSONALISATION:
                /* 检查密码是否正确 */
                ulResult = MMA_CheckMePersonalisationPwd(stMePersonalData);
                /* 密码正确，则设置当前的IMSI String */
                if (TAF_PH_ME_PERSONALISATION_OK == ulResult)
                {
                    /* 解除锁卡状态 */
                    g_stMmaMePersonalisationStatus.SimLockStatus = MMA_SIM_LOCK_IS_VERIFY;
                }
                break;
            case TAF_OPERATOR_PERSONALISATION:
                ulResult = MMA_OperatorPersonlisationVerify( stMePersonalData );
                break;
            default:
                ulResult = TAF_PH_ME_PERSONALISATION_WRONG_PARA;
                break;
        }
    }
    else if (TAF_PH_ME_NO_SIM == ulResult)
    {
        if ( TAF_OPERATOR_PERSONALISATION == stMePersonalData.MePersonalType )
        {
            ulResult = MMA_OperatorPersonlisationVerify( stMePersonalData );
        }
        else
        {
            ulResult = TAF_PH_ME_PERSONALISATION_NO_SIM;
        }
    }
    else
    {
        ulResult = TAF_PH_ME_PERSONALISATION_OP_NOT_ALLOW;
    }
    stMePersonalCnf.OpRslt = (VOS_UINT8)ulResult;

    MMA_MePersonalCnfReport(ClientId, OpId, stMePersonalCnf);

}
VOS_VOID MMA_MePersonlisationPwdChange(VOS_UINT16   ClientId,
                                      VOS_UINT8    OpId,
                                      TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData)
{
    TAF_PH_ME_PERSONLISATION_CNF_STRU stMePersonalCnf;
    VOS_UINT32          ulResult;


    PS_MEM_SET(&stMePersonalCnf, 0x00, sizeof(stMePersonalCnf));


    stMePersonalCnf.CmdType  = stMePersonalData.CmdType;
    stMePersonalCnf.MePersonalisatonType  = stMePersonalData.MePersonalType;

    switch(stMePersonalData.MePersonalType)
    {
        case TAF_SIM_PERSONALISATION:
            /*检查密码是否正确*/
            ulResult = MMA_CheckMePersonalisationPwd(stMePersonalData);


            /*密码正确，则设置新的密码*/
            if (TAF_PH_ME_PERSONALISATION_OK == ulResult)
            {
                ulResult = NV_Write(en_NV_Item_Sim_Personalisation_Pwd,
                                    stMePersonalData.aucNewPwd,
                                    TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX);

                if (NV_OK != ulResult)
                {
                    ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
                }
            }
            break;

        default:
            ulResult = TAF_PH_ME_PERSONALISATION_WRONG_PARA;
            break;
    }

    stMePersonalCnf.OpRslt = (VOS_UINT8)ulResult;;

    MMA_MePersonalCnfReport(ClientId, OpId, stMePersonalCnf);

}


VOS_VOID MMA_MePersonlisationActive(VOS_UINT16   ClientId,
                                   VOS_UINT8    OpId,
                                   TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData)
{
    TAF_PH_ME_PERSONLISATION_CNF_STRU   stMePersonalCnf;
    VOS_UINT32                          ulResult;
    NAS_MMA_ME_PERSONAL_ACT_FLAG_STRU   stMePersonActFlag;
    VOS_UINT16                          usDataLen = TAF_MAX_IMSI_LEN + 1;
    VOS_UINT32                          ulNvLength;
    TAF_UINT8                           aucSimPersonalisationStr[TAF_MAX_IMSI_LEN + 1];

    PS_MEM_SET(&stMePersonalCnf,         0x00, sizeof(stMePersonalCnf));
    PS_MEM_SET(aucSimPersonalisationStr, 0x00, sizeof(aucSimPersonalisationStr));
    stMePersonActFlag.usMePersonalActFlag = TAF_ME_PERSONALISATION_ACTIVE_STATUS;
    ulNvLength = 0;

    stMePersonalCnf.CmdType  = stMePersonalData.CmdType;
    stMePersonalCnf.MePersonalisatonType  = stMePersonalData.MePersonalType;

    switch(stMePersonalData.MePersonalType)
    {
        case TAF_SIM_PERSONALISATION:
            /*检查密码是否正确*/
            ulResult = MMA_CheckMePersonalisationPwd(stMePersonalData);
            if (TAF_PH_ME_PERSONALISATION_OK != ulResult)
            {
                break;
            }

            /*检查当前是否有设定的字串，字串为0时，不能激活*/
            ulResult = NV_Read(en_NV_Item_Sim_Personalisation_Imsi_Str,
                               aucSimPersonalisationStr,
                               usDataLen);
            if (NV_OK != ulResult)
            {
                ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
                break;
            }
            else
            {
                if (0 == aucSimPersonalisationStr[0])
                {
                    ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
                    break;
                }
            }

            /*设置激活状态*/
            NV_GetLength(en_NV_Item_Me_Personalisation_Active_Flag, &ulNvLength);
            ulResult = NV_Write(en_NV_Item_Me_Personalisation_Active_Flag,
                                &stMePersonActFlag,
                                ulNvLength);
            if (NV_OK != ulResult)
            {
                ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
            }
            break;

        default:
            ulResult = TAF_PH_ME_PERSONALISATION_WRONG_PARA;
            break;
    }

    stMePersonalCnf.OpRslt = (VOS_UINT8)ulResult;

    MMA_MePersonalCnfReport(ClientId, OpId, stMePersonalCnf);
}
VOS_VOID MMA_MePersonlisationDeactive(VOS_UINT16   ClientId,
                                     VOS_UINT8    OpId,
                                     TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData)
{
    TAF_PH_ME_PERSONLISATION_CNF_STRU   stMePersonalCnf;
    VOS_UINT32                          ulResult;
    NAS_MMA_ME_PERSONAL_ACT_FLAG_STRU   stMePersonActFlag;

    stMePersonActFlag.usMePersonalActFlag = TAF_ME_PERSONALISATION_DEACTIVE_STATUS;


    PS_MEM_SET(&stMePersonalCnf, 0x00, sizeof(stMePersonalCnf));

    stMePersonalCnf.CmdType  = stMePersonalData.CmdType;
    stMePersonalCnf.MePersonalisatonType  = stMePersonalData.MePersonalType;

    switch(stMePersonalData.MePersonalType)
    {
        case TAF_SIM_PERSONALISATION:
            /*检查密码是否正确*/
            ulResult = MMA_CheckMePersonalisationPwd(stMePersonalData);

            /*密码正确，去激活锁卡功能*/
            if (TAF_PH_ME_PERSONALISATION_OK == ulResult)
            {
                ulResult = NV_Write(en_NV_Item_Me_Personalisation_Active_Flag,
                                    &stMePersonActFlag,
                                    2);
                if (NV_OK != ulResult)
                {
                    ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
                }
            }
            break;

        default:
            ulResult = TAF_PH_ME_PERSONALISATION_WRONG_PARA;
            break;
    }

    stMePersonalCnf.OpRslt = (VOS_UINT8)ulResult;;

    MMA_MePersonalCnfReport(ClientId, OpId, stMePersonalCnf);
}


VOS_VOID MMA_MePersonlisationRetrieve(VOS_UINT16   ClientId,
                                     VOS_UINT8    OpId,
                                     TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData)
{
    TAF_PH_ME_PERSONLISATION_CNF_STRU   stMePersonalCnf;
    VOS_UINT32                          ulResult = TAF_PH_ME_PERSONALISATION_OK;
    VOS_UINT16                          usDataLen = TAF_MAX_IMSI_LEN + 1;
    TAF_UINT8                           aucSimPersonalisationStr[TAF_MAX_IMSI_LEN + 1];


    PS_MEM_SET(&stMePersonalCnf,         0x00, sizeof(stMePersonalCnf));
    PS_MEM_SET(aucSimPersonalisationStr, 0x00, sizeof(aucSimPersonalisationStr));

    stMePersonalCnf.CmdType  = stMePersonalData.CmdType;
    stMePersonalCnf.MePersonalisatonType  = stMePersonalData.MePersonalType;

    switch(stMePersonalData.MePersonalType)
    {
        case TAF_SIM_PERSONALISATION:
            /*检查密码是否正确*/
            ulResult = MMA_CheckMePersonalisationPwd(stMePersonalData);

            /*密码正确，读取锁卡的字串*/
            if (TAF_PH_ME_PERSONALISATION_OK == ulResult)
            {
                ulResult = NV_Read(en_NV_Item_Sim_Personalisation_Imsi_Str,
                                   aucSimPersonalisationStr,
                                   usDataLen);
                if (NV_OK != ulResult)
                {
                    if (ulResult == NV_ID_NOT_EXIST)
                    {
                        stMePersonalCnf.unReportContent.SimPersionalisationStr.DataLen = 0;
                        ulResult = TAF_PH_ME_PERSONALISATION_OK;
                    }
                    else
                    {
                        ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
                    }
                }
                else
                {
                    stMePersonalCnf.unReportContent.SimPersionalisationStr.DataLen = aucSimPersonalisationStr[0];
                    VOS_MemCpy(stMePersonalCnf.unReportContent.SimPersionalisationStr.aucSimPersonalisationStr,
                               aucSimPersonalisationStr + 1,
                               aucSimPersonalisationStr[0]
                               );
                }
            }
            break;

        default:
            ulResult = TAF_PH_ME_PERSONALISATION_WRONG_PARA;
            break;
    }

    stMePersonalCnf.OpRslt = (VOS_UINT8)ulResult;;

    MMA_MePersonalCnfReport(ClientId, OpId, stMePersonalCnf);

}


VOS_BOOL MMA_IsSimLockPlmnInfoValid(VOS_VOID)
{
    TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU  stSimLockPlmnInfo;
    VOS_UINT32                          i;


    PS_MEM_SET(&stSimLockPlmnInfo, 0x00, sizeof(stSimLockPlmnInfo));

    if (NV_OK != NV_Read(en_NV_Item_CustomizeSimLockPlmnInfo,
                       &stSimLockPlmnInfo,
                       sizeof(stSimLockPlmnInfo)))
    {
        return VOS_FALSE;
    }

    if (VOS_FALSE == stSimLockPlmnInfo.ulStatus)
    {
        return VOS_FALSE;
    }

    for ( i = 0; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++ )
    {
        if (0xAA == stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin[0])
        {
            continue;
        }

        break;
    }

    /* 如果每一项的PLMN都不是有效的，那么返回没有有效的PLMN信息 */
    if (TAF_MAX_SIM_LOCK_RANGE_NUM == i)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32 MMA_GetCardLockInfo(TAF_OPERATOR_LOCK_INFO_STRU *pstOperatorLockInfo)
{

    VOS_UINT32                          ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
    TAF_CUSTOM_CARDLOCK_STATUS_STRU     stCardLockStatus;
    TAF_OPERATOR_LOCK_INFO_STRU         *pstCardLockInfo;
    VOS_UINT8                           ucSimStatus;


    PS_MEM_SET(&stCardLockStatus, 0x00, sizeof(stCardLockStatus));

    if (VOS_NULL_PTR == pstOperatorLockInfo)
    {
        return ulResult;
    }

    ulResult = TAF_PH_ME_PERSONALISATION_OK;
    pstCardLockInfo = &(g_stMmaMePersonalisationStatus.stCardLockInfo);

    if (NV_OK == NV_Read(en_NV_Item_CardlockStatus,
                            &stCardLockStatus,sizeof(stCardLockStatus)))
    {

        if (VOS_TRUE != stCardLockStatus.ulStatus)
        {
            /*更新运营商锁状态*/
            pstCardLockInfo->OperatorLockStatus = TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE;
        }
        else
        {
            /*更新运营商锁状态*/
            pstCardLockInfo->OperatorLockStatus= stCardLockStatus.enCardlockStatus;

            if (MMA_SIM_LOCK_IS_VERIFY == g_stMmaMePersonalisationStatus.SimLockStatus)
            {
                pstCardLockInfo->OperatorLockStatus = TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE;
            }
        }

        /* 更新剩余次数 */
        if (TAF_OPERATOR_LOCK_LOCKED == pstCardLockInfo->OperatorLockStatus)
        {
            pstCardLockInfo->RemainTimes = 0;
        }
        else if (TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE == pstCardLockInfo->OperatorLockStatus)
        {
            pstCardLockInfo->RemainTimes = g_MmaNewValue.stSimLockMaxTimes.ulLockMaxTimes;
        }
        else
        {
            pstCardLockInfo->RemainTimes = stCardLockStatus.ulRemainUnlockTimes;
        }


    }
    else
    {
        pstCardLockInfo->OperatorLockStatus = TAF_OPERATOR_LOCK_LOCKED;
        pstCardLockInfo->RemainTimes = 0;
    }

    /* 如果当前无卡，并且NV项en_NV_Item_CustomizeSimLockPlmnInfo中的
       PLMN无效的时候 */
    MMA_PhoneGetUsimStatus(&ucSimStatus);
    if ((VOS_FALSE == MMA_IsSimLockPlmnInfoValid())
      &&(MMA_USIM_ABSENT == ucSimStatus))
    {
        pstCardLockInfo->OperatorLockStatus = TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE;
    }

    /*目前其他的数据卡在使用^cardlock这个命令时，上报的
      operator没有使用，所以在这里，把长度置0*/
    pstCardLockInfo->OperatorLen = 0;

    g_MmaNewValue.stCardLockStatus = stCardLockStatus;

    PS_MEM_CPY(pstOperatorLockInfo,
                   pstCardLockInfo,
                       sizeof(TAF_OPERATOR_LOCK_INFO_STRU));

    return ulResult;

}




VOS_VOID MMA_MePersonlisationQuery(VOS_UINT16   ClientId,
                                  VOS_UINT8    OpId,
                                  TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData)
{
    TAF_PH_ME_PERSONLISATION_CNF_STRU   stMePersonalCnf;
    VOS_UINT32                          ulResult;
    NAS_MMA_ME_PERSONAL_ACT_FLAG_STRU   stMePersonActFlag;
    VOS_UINT16                          usDataLen = 2;
    VOS_UINT8                           ucSimStatus;

    VOS_UINT8                          *pucImsi = VOS_NULL_PTR;

    pucImsi = TAF_SDC_GetSimImsi();


    stMePersonActFlag.usMePersonalActFlag = VOS_FALSE;

    PS_MEM_SET(&stMePersonalCnf,0,sizeof(TAF_PH_ME_PERSONLISATION_CNF_STRU));
    stMePersonalCnf.CmdType  = stMePersonalData.CmdType;
    stMePersonalCnf.MePersonalisatonType  = stMePersonalData.MePersonalType;

    MMA_PhoneGetUsimStatus(&ucSimStatus);

    if (MMA_USIM_PRESENT != ucSimStatus)
    {
        if (MMA_USIM_ABSENT == ucSimStatus)
        {
            ulResult = MMA_GetCardLockInfo(&(stMePersonalCnf.unReportContent.OperatorLockInfo));
        }
        else
        {
            ulResult = TAF_PH_ME_PERSONALISATION_NO_SIM;
        }
    }
    else if (MMA_QUICK_START_ENABLE == gstMmaValue.ulQuickStartFlg)
    {
        ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
    }
    else if (MMA_NONEED_PIN1 != MMA_CheckPin1SatusSimple())
    {
        ulResult = MMA_GetCardLockInfo(&(stMePersonalCnf.unReportContent.OperatorLockInfo));
    }
    else
    {
        if (USIMM_API_SUCCESS == NAS_USIMMAPI_GetCardIMSI(pucImsi))
        {
            MMA_CheckMePersonalisationStatus();

            switch(stMePersonalData.MePersonalType)
            {
                /* this case is  used for clck query  */
                case TAF_SIM_PERSONALISATION:
                    ulResult = NV_Read(en_NV_Item_Me_Personalisation_Active_Flag,
                                       &stMePersonActFlag,
                                       usDataLen);
                    if (NV_OK != ulResult)
                    {
                        ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
                        break;
                    }
                    g_MmaNewValue.usSimMePersonalisationActFlag = stMePersonActFlag.usMePersonalActFlag;

                    ulResult = TAF_PH_ME_PERSONALISATION_OK;
                    stMePersonalCnf.ActiveStatus = (VOS_UINT8)(stMePersonActFlag.usMePersonalActFlag);
                    break;
                case TAF_OPERATOR_PERSONALISATION:
                    ulResult = MMA_GetCardLockInfo(&(stMePersonalCnf.unReportContent.OperatorLockInfo));
                    break;
                default:
                    ulResult = TAF_PH_ME_PERSONALISATION_WRONG_PARA;
                    break;
            }
        }
        else
        {
            ulResult = TAF_PH_ME_PERSONALISATION_ERROR;
        }
    }
    stMePersonalCnf.OpRslt              = (VOS_UINT8)ulResult;
    MMA_MePersonalCnfReport(ClientId, OpId, stMePersonalCnf);
}
VOS_VOID MMA_MePersonalisationHandle(VOS_UINT16 ClientId,
                                     VOS_UINT8 OpId,
                                     TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData)
{


    switch (stMePersonalData.CmdType)
    {
        case  TAF_ME_PERSONALISATION_SET:
            MMA_MePersonlisationSet(ClientId, OpId, stMePersonalData);
            break;

        case TAF_ME_PERSONALISATION_VERIFY:
            MMA_MePersonlisationVerify(ClientId, OpId, stMePersonalData);
            break;

        case TAF_ME_PERSONALISATION_PWD_CHANGE:
            MMA_MePersonlisationPwdChange(ClientId, OpId, stMePersonalData);
            break;

        case TAF_ME_PERSONALISATION_ACTIVE:
            MMA_MePersonlisationActive(ClientId, OpId, stMePersonalData);
            break;

        case TAF_ME_PERSONALISATION_DEACTIVE:
            MMA_MePersonlisationDeactive(ClientId, OpId, stMePersonalData);
            break;

        case TAF_ME_PERSONALISATION_RETRIEVE:
            MMA_MePersonlisationRetrieve(ClientId, OpId, stMePersonalData);
            break;

        case TAF_ME_PERSONALISATION_QUERY:
            MMA_MePersonlisationQuery(ClientId, OpId, stMePersonalData);
            break;

        default:
            break;
    }


    return;
}


VOS_UINT8 MMA_GetMeLockStatus(VOS_VOID)
{
    VOS_UINT8                          ucSimStatus;
    VOS_UINT8                           ucSdcSimStatus;

    VOS_UINT8                          *pucImsi = VOS_NULL_PTR;

    pucImsi = TAF_SDC_GetSimImsi();

    MMA_PhoneGetUsimStatus(&ucSimStatus);
    if (MMA_USIM_PRESENT != ucSimStatus)
    {
        return TAF_PH_ME_NO_SIM;
    }

    ucSdcSimStatus = TAF_SDC_GetSimStatus();

    if ((TAF_SDC_USIM_STATUS_SIM_PIN == ucSdcSimStatus)
     || (TAF_SDC_USIM_STATUS_SIM_PUK == ucSdcSimStatus))
    {
        return TAF_PH_ME_NO_SIM;
    }

    /* 如果当前没有获取到IMSI,重新获取一次 */
    if (pucImsi[0] > 8)
    {
        if (USIMM_API_SUCCESS == NAS_USIMMAPI_GetCardIMSI(pucImsi))
        {
            /* 检查是否锁卡 */
            MMA_CheckMePersonalisationStatus();
        }
        else
        {
            return TAF_PH_ME_NO_SIM;
        }
    }

    if (g_stMmaMePersonalisationStatus.SimLockStatus == 1)
    {
        return TAF_PH_ME_SIM_LOCK;
    }
    return TAF_PH_ME_SIM_UNLOCK;
}

/*****************************************************************************
 函 数 名  : MMA_OpPinExpired
 功能描述  : PIN 超时处理函数，返回PIN 超时结果给上层AT
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年9月1日
    作    者   : liuyang
    修改内容   : 新生成函数

  2.日    期   : 2006年3月8日
    作    者   : liuyang id:48197
    修改内容   : 问题单号：A32D02371，使用宏替代了原有的常数

  3.日    期   : 2006年3月20日
    作    者   : liuyang id:48197
    修改内容   : 问题单号：A32D02602
*****************************************************************************/
VOS_VOID MMA_OpPinExpired(VOS_VOID)
{
    /*超时消息有效性判断*/
    if ( MMA_TIMER_RUN != g_MmaSimTimer[0].ucTimerStatus  )
    {
        MMA_WARNINGLOG("MMA_OpPinExpired():WARNING:OP PIN TIMER is not running!");
        return;
    }
    MMA_WARNINGLOG("MMA_OpPinExpired():WARNING:OP PIN TIMER OUT!");
    /*设置停止定时器标志*/
    g_MmaSimTimer[0].ucTimerStatus           = MMA_TIMER_STOP;
    /*清除使用标志*/
    gastMmaTiTab[MMA_PH_PIN_TI].ucUsed       = MMA_TI_UNUSED;
    /*错误处理*/
    MMA_HandleEventError(gastMmaTiTab[MMA_PH_PIN_TI].ClientId, gastMmaTiTab[MMA_PH_PIN_TI].Id, TAF_ERR_TIME_OUT, TAF_PH_EVT_OP_PIN_CNF);
    return;
}

/* 删除 MMA_SendPINOk函数 ，该函数没有地方调用 */

VOS_UINT32 MN_PH_PinOper(
    const TAF_PH_PIN_CNF_STRU           *pstPinInfo
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           aucMmaImsi[9];
    VOS_UINT32                          ulPin1;
    VOS_UINT8                           ucSdcSimStatus;

    ucSdcSimStatus  = TAF_SDC_GetSimStatus();

    /* 获取当前是否能获取到IMSI,能获取到则不用启动定时器 */
    ulPin1 = MMA_Pin1Noneed();

    if ((MMA_TRUE == MMA_OpPin1Success(pstPinInfo))
     && (MMA_TRUE == ulPin1)
     && ((TAF_SDC_USIM_STATUS_SIM_PIN == ucSdcSimStatus)
      || (TAF_SDC_USIM_STATUS_SIM_PUK == ucSdcSimStatus)))
    {

        /* 关机状态下解PIN,不能把MMA状态置为开机状态 */

        /*A32D11538==>需要输入PIN码时,MMA模块初始化时认为有卡*/


        if (USIMM_API_SUCCESS == NAS_USIMMAPI_GetCardIMSI(aucMmaImsi))
        {
            MMA_INFOLOG("MN_PH_PinOper():Info :USIMM_GetCardIMSI OK!");
            return VOS_OK;
        }

        /* PIN OK后USIM需要初始化，需要延时以保证读取IMSI正常 */
        g_stPhPhResetTimer.ulTimerId = TI_MN_PH_PIN_OPER;

        ulRet = NAS_StartRelTimer( &g_stPhPhResetTimer.MmaHTimer,
                                     WUEPS_PID_MMA,
                                     TI_MN_PH_PIN_OPER_LENGTH,
                                     TAF_MMA,
                                     TI_MN_PH_PIN_OPER,
                                     VOS_RELTIMER_NOLOOP );

        if( VOS_OK !=  ulRet)
        {
            MMA_ERRORLOG("MN_PH_PinOper():ERROR:Start TI_MN_PH_PIN_OPER Timer Error!");
        }
        else
        {
            g_stPhPhResetTimer.ucTimerStatus = MMA_TIMER_RUN;
        }
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID MMA_PinDataReport( const TAF_PH_PIN_CNF_STRU *pstPinCnf)
{
    TAF_PHONE_EVENT_INFO_STRU           *pstPhoneEvent;
    VOS_UINT32                          ulRet;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MMA_InitAtMsgReq:ERROR:ALLOC MEMORY FAIL.");
        return;
    }
    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));
    /*参数有效性判断*/
    if ( VOS_NULL_PTR == pstPinCnf)
    {
        MMA_WARNINGLOG("MMA_PinDataReport():WARNING:pPinCnf is NULL PTR!.");
        MMA_HandleEventError(gstMmaValue.stOpPinData.ClientId,
                             gstMmaValue.stOpPinData.OpId,
                             TAF_ERR_NULL_PTR,
                             TAF_PH_EVT_OP_PIN_CNF);
        PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
        return;
    }
    MMA_InitEventInfoOP(pstPhoneEvent);
    /*调用TAF事件上报函数,上报PIN事件*/
    pstPhoneEvent->OP_Pin                    = MMA_TRUE;
    pstPhoneEvent->PinCnf                    = *pstPinCnf;
    pstPhoneEvent->PhoneEvent                = TAF_PH_EVT_OP_PIN_CNF;

    /*非查询状态下，要停止定时器*/
    if ( (TAF_PIN_QUERY != pstPinCnf->CmdType)
     && (TAF_PIN2_QUERY != pstPinCnf->CmdType) )
    {
        /*停止定时器*/
        if ( MMA_TIMER_RUN == g_MmaSimTimer[0].ucTimerStatus )
        {
            if (VOS_OK != NAS_StopRelTimer(WUEPS_PID_MMA, TAF_USIM_OPPIN, &(g_MmaSimTimer[0].MmaHTimer)))
            {
                MMA_WARNINGLOG("MMA_PinDataReport():WARNING:NAS_StopRelTimer failed!");
            }

            /*设置停止定时器标志*/
            g_MmaSimTimer[0].ucTimerStatus    = MMA_TIMER_STOP;
        }
        switch ( pstPinCnf->OpPinResult )
        {
            case USIMM_SIM_PIN_REQUIRED:
                pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_NEED_PIN1;
                break;
            case USIMM_SIM_PIN2_REQUIRED:
                pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_NEED_PIN2;
                break;
            case USIMM_SIM_PUK_REQUIRED:
                pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_NEED_PUK1;
                break;
            case USIMM_SIM_PUK2_REQUIRED:
                pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_NEED_PUK2;
                break;
            case USIMM_INCORRECT_PASSWORD:
                if ((TAF_SIM_PUK == pstPhoneEvent->PinCnf.PinType) && (0 == pstPhoneEvent->PinCnf.RemainTime.ucPuk1RemainTime))
                {
                    pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_SIM_FAIL;
                }
                else
                {
                    pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_INCORRECT_PASSWORD;
                }
                break;
            case USIMM_OPERATION_NOT_ALLOW:
                pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_OPERATION_NOT_ALLOW;
                break;
            case USIMM_SIM_FAILURE:
                pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_SIM_FAIL;
                break;
            case USIMM_API_SUCCESS:
                pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_OK;
                break;
            default:
                MMA_WARNINGLOG1("MMA_PinDataReport():WARNING:WRONG PIN RESULT is", pstPhoneEvent->PinCnf.OpPinResult);
                pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_PIN_ERROR;
                break;
        }
    }

    ulRet = VOS_OK;

    if ( ((TAF_PIN_QUERY != pstPinCnf->CmdType) && (TAF_PIN2_QUERY != pstPinCnf->CmdType))
      && (TAF_PH_OP_PIN_OK == pstPhoneEvent->PinCnf.OpPinResult))
    {
        ulRet = MN_PH_PinOper(pstPinCnf);
    }

    /*获取当前是否能上报,如果需要启动定时器则需等定时器超时后才能上报 */
    if ( VOS_OK == ulRet)
    {
        Taf_PhoneEvent(pstPhoneEvent);
    }
    else
    {
        PS_MEM_CPY(&g_stPhPinCnf,&pstPhoneEvent->PinCnf,sizeof(g_stPhPinCnf));
    }

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return;
}
VOS_VOID MMA_PhoneGetNetworkNameFromUsim (VOS_UINT16   ClientId,
                                          VOS_UINT8          OpId)
{
    TAF_PH_USIM_SPN_STRU      stSpnInfo;

    if (TAF_PH_USIM_SPN == gstMmaValue.stUsimInfo.UsimSpnInfo.SpnType)
    {
        stSpnInfo = gstMmaValue.stUsimInfo.UsimSpnInfo;
    }
    else if (TAF_PH_GSM_SPN == gstMmaValue.stUsimInfo.SimSpnInfo.SpnType)
    {
        stSpnInfo = gstMmaValue.stUsimInfo.SimSpnInfo;
    }
    else
     {
       MMA_WARNINGLOG("MMA_PhoneGetNetworkNameFromUsim():WARNING:SPN FILE NO READY");
       MMA_ReportParaQueryError(ClientId, OpId,
                                TAF_ERR_UNSPECIFIED_ERROR,
                                TAF_PH_NETWORKNAMEFROMUSIM_PARA);
       return;
    }

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_NETWORKNAMEFROMUSIM_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stSpnInfo);

    return;
}
VOS_VOID MMA_PhoneGetNetworkName(VOS_UINT16    ClientId,
                                 VOS_UINT8           OpId)
{
    TAF_PH_NETWORKNAME_STRU              stOperatorNameTmp;
    TAF_PH_OPERATOR_NAME_FORMAT_OP_STRU  stOperName;
    TAF_PLMN_ID_STRU                    *pstPlmnId = VOS_NULL_PTR;
    TAF_PLMN_ID_STRU                     stPlmnId;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8    enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8    enPsServiceStatus;
    MMA_MMC_OPERATOR_NAME_INFO_STRU      stMmcName;

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();
    pstPlmnId         = &stPlmnId;

    PS_MEM_SET(&stOperName,0,sizeof(stOperName));
    PS_MEM_SET(&stOperatorNameTmp,0,sizeof(stOperatorNameTmp));
    PS_MEM_SET(&stMmcName, 0, sizeof(stMmcName));

    /* 该字段无用，at会根据自己记录的全局变量上报format */
    stOperatorNameTmp.NameFormat        = TAF_PH_NETWORKNAME_ALL;
    stOperatorNameTmp.RaMode            = TAF_SDC_GetSysMode();


    /* mmc对不同状态的plmn Id判断不准确，使用随SYS_INFO上报的PLMN ID, 加上当前服务状态的判定 */
    if ( (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enCsServiceStatus)
      || (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enPsServiceStatus) )
    {
        pstPlmnId = (TAF_PLMN_ID_STRU *)TAF_SDC_GetCurrCampPlmnId();
    }
    else
    {
        pstPlmnId->Mcc = 0;
        pstPlmnId->Mnc = 0;
    }

    stOperatorNameTmp.Name.PlmnId  = *pstPlmnId;

    /* 如果当前注册的PLMN与储存的信息一致，使用储存信息 */
    if ((pstPlmnId->Mcc == gstMmaValue.stOperatorNameInfo.stOperatorPlmnId.ulMcc)
     && (pstPlmnId->Mnc == gstMmaValue.stOperatorNameInfo.stOperatorPlmnId.ulMnc))
    {
        TAF_MMA_TranslateNtwkName2Str((VOS_UINT8 *)(gstMmaValue.stOperatorNameInfo.aucOperatorNameLong), 
                                      (VOS_CHAR *)stMmcName.aucOperatorNameLong, 
                                      TAF_PH_OPER_NAME_LONG);
        TAF_MMA_TranslateNtwkName2Str((VOS_UINT8 *)(gstMmaValue.stOperatorNameInfo.aucOperatorNameShort), 
                                      (VOS_CHAR *)stMmcName.aucOperatorNameShort, 
                                      TAF_PH_OPER_NAME_SHORT);
        
        PS_MEM_CPY(stOperatorNameTmp.Name.aucOperatorNameLong,
             stMmcName.aucOperatorNameLong, sizeof(stOperatorNameTmp.Name.aucOperatorNameLong));
        
        PS_MEM_CPY(stOperatorNameTmp.Name.aucOperatorNameShort,
              stMmcName.aucOperatorNameShort, sizeof(stOperatorNameTmp.Name.aucOperatorNameShort));
    }

    stOperatorNameTmp.PlmnSelMode  = TAF_MMA_GetPlmnSelectionMode();

    /*当前结构中没有附带有运营商名字的信息,只有PLMN,需要在MMA中调用增加*/
    stOperName.ListOp                   = TAF_PH_OPER_NAME_PLMN2ALPHANUMERICNAME;
    stOperName.OperName                 = stOperatorNameTmp.Name;


    MMA_INFOLOG1("Taf_PhoneGetOperNameInfo():stOperName.OperName.PlmnId.Mcc =",(VOS_INT32)stOperName.OperName.PlmnId.Mcc);
    MMA_INFOLOG1("Taf_PhoneGetOperNameInfo():stOperName.OperName.PlmnId.Mnc =",(VOS_INT32)stOperName.OperName.PlmnId.Mnc);
    MMA_PlmnId2Bcd(&(stOperName.OperName.PlmnId));
    /* BEGIN: Added by liuyang id:48197, 2006/2/24   PN:A32D02186*/
    if (MMA_FAILURE == Taf_PhoneGetOperNameInfo( &stOperName ))
    {
        stOperName.OperName.aucOperatorNameLong[0]  = '\0';
        stOperName.OperName.aucOperatorNameShort[0] = '\0';
    }
    /* END:   Added by liuyang id:48197, 2006/2/24 */
    MMA_MEM_CPY(&(stOperatorNameTmp.Name), &(stOperName.OperName), sizeof(TAF_PH_OPERATOR_NAME_STRU));
    /*使用数据上报函数将注册状态数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_NETWORKNAME_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stOperatorNameTmp);
    return;
}

/* 删除MMA_PhoneGetCellRoam */

TAF_UINT32 Taf_PhoneGetNetworkNameForListPlmn(TAF_PH_OPERATOR_NAME_FORMAT_OP_STRU *pstOperName)
{
    VOS_UINT32                ulRet = MMA_FAILURE;

    pstOperName->OperName.aucOperatorNameLong[0]  = '\0';
    pstOperName->OperName.aucOperatorNameShort[0] = '\0';

    if ((0 == pstOperName->OperName.PlmnId.Mcc) && (0 == pstOperName->OperName.PlmnId.Mnc))
    {
        return ulRet;
    }

    /* 从运营商列表中查询PLMNID对应的名称 */
    ulRet = MMA_PhoneFindNtwkNameByPlmnId(&(pstOperName->OperName));

    return ulRet;
}
VOS_VOID MMA_PhoneGetSrvStatus(VOS_UINT16 ClientId, VOS_UINT8 OpId)
{

    /*VOS_UINT8                *pucBuf;  */
    /*VOS_UINT8                 ucParalen;  */
    TAF_PH_SRV_STA_STRU       stSrvTmp;

    /*服务状态装入缓冲区*/
    stSrvTmp.CsSrvSta = (TAF_PHONE_SERVICE_STATUS)(TAF_SDC_GetCsServiceStatus());
    stSrvTmp.PsSrvSta = (TAF_PHONE_SERVICE_STATUS)(TAF_SDC_GetPsServiceStatus());

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_SRV_STATUS_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stSrvTmp);

    return;
}

/* 删除MMA_PhoneGetRoamSta */

VOS_VOID MMA_PhoneGetDomain(VOS_UINT16  ClientId,  VOS_UINT8  OpId)
{
    TAF_PH_DOMAIN_FLAG       PhDomain;

    /*从status中获取当前域状态*/
    PhDomain = gstMmaValue.pg_StatusContext->ucDomainFlag;

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_DOMAIN_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&PhDomain);
    return;
}


VOS_UINT32 NAS_MMA_MntnTraceGetUsimStatus(
    VOS_UINT8                           ucUsimStatus,
    VOS_UINT8                           ucMeLockStatus
)
{
    NAS_MMA_MNTN_GET_USIM_STATUS_STRU   *pstSendMsg;
    NAS_MMA_USIM_STATUS_INFO_STRT       *pstUsimStatusInfo;

    pstSendMsg = (NAS_MMA_MNTN_GET_USIM_STATUS_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA, sizeof(NAS_MMA_MNTN_GET_USIM_STATUS_STRU));

    if ( VOS_NULL_PTR == pstSendMsg )
    {
        MMA_ERRORLOG("NAS_MMA_MntnTraceGetUsimStatus:ERROR: Alloc Memory Fail.");
        return MMA_ERROR;
    }

    pstUsimStatusInfo = &pstSendMsg->stStatusInfo;

    pstUsimStatusInfo->ulUsimHotInFlg           = g_ucUsimHotInFlag;
    pstUsimStatusInfo->ulUsimHotOutFlg          = g_ucUsimHotOutFlag;

    pstUsimStatusInfo->ulUsimHotInCount         = g_ulUsimHotInCount;
    pstUsimStatusInfo->ulUsimHotOutCount        = g_ulUsimHotOutCount;

    pstUsimStatusInfo->ucUsimStatusFromUsim     = ucUsimStatus;

    pstUsimStatusInfo->ucMeLockStatus           = ucMeLockStatus;

    pstUsimStatusInfo->ucMmaGlobalUsimStatus    = TAF_SDC_GetSimStatus();


    pstSendMsg->ulReceiverPid                   = WUEPS_PID_MMA;
    pstSendMsg->ulSenderPid                     = WUEPS_PID_MMA;
    pstSendMsg->ulSenderCpuId                   = VOS_LOCAL_CPUID;
    pstSendMsg->ulReceiverCpuId                 = VOS_LOCAL_CPUID;
    pstSendMsg->ulLength                        = sizeof(NAS_MMA_USIM_STATUS_INFO_STRT) + 4;
    pstSendMsg->ulMsgID                         = ID_NAS_MMA_MNTN_GET_USIM_STATUS;

    OM_TraceMsgHook(pstSendMsg);
    PS_FREE_MSG(WUEPS_PID_MMA, pstSendMsg);

    return MMA_SUCCESS;
}


VOS_UINT8 MMA_GetUsimStatus(
    VOS_UINT32                          ulUsimStaChg
)
{
    VOS_BOOL                            bCsSimStaus;
    VOS_BOOL                            bPsSimStaus;
    VOS_UINT8                           ucMeLockStatus;
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enSdcSimStatus;
    VOS_UINT8                           ucCurPhMode;

    /* 快速开机时返回ROMSIM */
    if (MMA_QUICK_START_ENABLE == gstMmaValue.ulQuickStartFlg)
    {
        return TAF_PH_USIM_ROMSIM;
    }

    enSdcSimStatus = TAF_SDC_GetSimStatus();
    ucMeLockStatus = MMA_GetMeLockStatus();

    /* 如果当前需要校验PIN码或卡处于锁定状态或卡不可用，则卡状态为无效状态 */
    if ((VOS_TRUE                    == g_ucUsimHotInFlag)
     || (TAF_SDC_USIM_STATUS_SIM_PIN == enSdcSimStatus)
     || (TAF_SDC_USIM_STATUS_SIM_PUK == enSdcSimStatus)
     || (TAF_SDC_USIM_STATUS_UNVALID == enSdcSimStatus)
     || (MMA_SIM_IS_LOCK             == ucMeLockStatus)
     || (TAF_SDC_USIM_STATUS_READY_TIMEROUT == enSdcSimStatus))
    {
        return TAF_PH_USIM_STATUS_UNAVAIL;
    }

    if (TAF_SDC_USIM_STATUS_NO_PRESENT == enSdcSimStatus)
    {
        return TAF_PH_USIM_NON;
    }

    ucCurPhMode = TAF_SDC_GetCurPhoneMode();

    /* 关机状态或开机过程中或卡状态发生变化使用SDC中的卡状态上报 */
    if ((TAF_MMA_FSM_PHONE_MODE == TAF_MMA_GetCurrFsmId())
     || (TAF_PH_MODE_FULL       != ucCurPhMode)
     || (VOS_TRUE               == ulUsimStaChg))
    {
        return enSdcSimStatus;
    }

    /* 其他情况需要从MMC获取CS/PS的卡是否无效信息,目前SDC未维护,
       后续维护后可以直接从SDC中获取 */
    NAS_MMC_GetCsPsSimStatus(&bCsSimStaus, &bPsSimStaus);

    if ((VOS_FALSE == bCsSimStaus)
     && (VOS_FALSE == bPsSimStaus))
    {
        return TAF_PH_USIM_STATUS_CSPS_UNVAIL;
    }

    if (VOS_FALSE == bCsSimStaus)
    {
        return TAF_PH_USIM_STATUS_CS_UNVAIL;
    }

    if (VOS_FALSE == bPsSimStaus)
    {
        return TAF_PH_USIM_STATUS_PS_UNVAIL;
    }

    return TAF_PH_USIM_STATUS_AVAIL;
}


VOS_VOID  MN_PH_ConvertSysSubModeToAtFormat_FDD(
    TAF_SYS_SUBMODE_ENUM_UINT8          enSrcSysSubMode,
    MN_PH_SUB_SYS_MODE_EX_ENUM_U8      *penDestSysSubMode
)
{
    switch (enSrcSysSubMode)
    {
        case TAF_SYS_SUBMODE_NONE:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_NONE_RAT;
            break;

        case TAF_SYS_SUBMODE_GSM:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_GSM_RAT;
            break;

        case TAF_SYS_SUBMODE_GPRS:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_GPRS_RAT;
            break;

        case TAF_SYS_SUBMODE_EDGE:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_EDGE_RAT;
            break;

        case TAF_SYS_SUBMODE_WCDMA:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_WCDMA_RAT;
            break;

        case TAF_SYS_SUBMODE_HSDPA:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_HSDPA_RAT;
            break;

        case TAF_SYS_SUBMODE_HSUPA:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_HSUPA_RAT;
            break;

        case TAF_SYS_SUBMODE_HSDPA_HSUPA:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_HSPA_RAT;
            break;

        case TAF_SYS_SUBMODE_HSPA_PLUS:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_HSPA_PLUS_RAT;
            break;

        case TAF_SYS_SUBMODE_DC_HSPA_PLUS:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_DCHSPA_PLUS_RAT;
            break;

        case TAF_SYS_SUBMODE_DC_MIMO:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_DCHSPA_PLUS_RAT;
            break;

        case TAF_SYS_SUBMODE_LTE:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_LTE_RAT;
            break;

        default:

            /* 异常打印 */
            MN_INFO_LOG("MN_PH_ConvertSysSubModeToAtFormat: invalid sub sys mode!");
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_BUTT_RAT;
            break;
    }

   return;
}
VOS_VOID  MN_PH_ConvertSysSubModeToAtFormat_TDD(
    TAF_SYS_SUBMODE_ENUM_UINT8          enSrcSysSubMode,
    MN_PH_SUB_SYS_MODE_EX_ENUM_U8      *penDestSysSubMode
)
{
    switch (enSrcSysSubMode)
    {
        case TAF_SYS_SUBMODE_NONE:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_NONE_RAT;
            break;

        case TAF_SYS_SUBMODE_GSM:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_GSM_RAT;
            break;

        case TAF_SYS_SUBMODE_GPRS:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_GPRS_RAT;
            break;

        case TAF_SYS_SUBMODE_EDGE:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_EDGE_RAT;
            break;

        case TAF_SYS_SUBMODE_HSDPA:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_TD_HSDPA_RAT;
            break;

        case TAF_SYS_SUBMODE_HSUPA:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_TD_HSUPA_RAT;
            break;

        case TAF_SYS_SUBMODE_HSDPA_HSUPA:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_TD_HSPA_RAT;
            break;

        case TAF_SYS_SUBMODE_TD_SCDMA:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_TDCDMA_RAT;
            break;

        case TAF_SYS_SUBMODE_HSPA_PLUS:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_TD_HSPA_PLUS_RAT;
            break;

        case TAF_SYS_SUBMODE_DC_HSPA_PLUS:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_DCHSPA_PLUS_RAT;
            break;

        case TAF_SYS_SUBMODE_DC_MIMO:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_DCHSPA_PLUS_RAT;
            break;

        case TAF_SYS_SUBMODE_LTE:
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_LTE_RAT;
            break;

        default:

            /* 异常打印 */
            MN_INFO_LOG("MN_PH_ConvertSysSubModeToAtFormat: invalid sub sys mode!");
            *penDestSysSubMode = MN_PH_SUB_SYS_MODE_EX_BUTT_RAT;
            break;
    }

   return;
}





VOS_VOID  MN_PH_ConvertSysSubModeToAtFormat(
    TAF_SYS_SUBMODE_ENUM_UINT8          enSrcSysSubMode,
    MN_PH_SUB_SYS_MODE_EX_ENUM_U8      *penDestSysSubMode
)
{
    if (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
    {
        MN_PH_ConvertSysSubModeToAtFormat_FDD(enSrcSysSubMode, penDestSysSubMode);
    }
    else
    {
        MN_PH_ConvertSysSubModeToAtFormat_TDD(enSrcSysSubMode, penDestSysSubMode);
    }

   return;
}



VOS_VOID  MN_PH_GetSysInfoRoamStatus(
    TAF_PH_SYSINFO_STRU                 *pstSysInfo
)
{
    TAF_PH_ROAM_STA                     enRoamSta;

    enRoamSta = MMA_IsRoam(TAF_SDC_GetCurrCampPlmnId());

    /* 成功注册到HOME PLMN时显示非漫游，成功注册到漫游网络时显示漫游，限制服务时显示未知 */
    if (TAF_PH_NO_ROAM == enRoamSta)
    {
        pstSysInfo->ucRoamStatus = TAF_PH_INFO_NONE_ROMAING;
    }
    else if (TAF_PH_ROAMING == enRoamSta)
    {
        pstSysInfo->ucRoamStatus = TAF_PH_INFO_ROMAING;
    }
    else
    {
        pstSysInfo->ucRoamStatus = TAF_PH_INFO_ROAM_UNKNOW;
    }
}
VOS_VOID  MN_PH_GetSysInfoSrvStatus(
    TAF_PH_SYSINFO_STRU                 *pstSysInfo
)
{
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enSimStatus;

    enSimStatus = TAF_SDC_GetSimStatus();

    if ( (VOS_TRUE           == gstMmaValue.pg_StatusContext->ulTimMaxFlg)
      && (SYSTEM_APP_ANDROID != g_usMmaAppConfigSupportType) )
    {

        pstSysInfo->ucSrvStatus = TAF_PH_INFO_DEEP_SLEEP;
        pstSysInfo->ucSrvDomain = TAF_PH_INFO_NO_DOMAIN;
    }

    if ((TAF_SDC_USIM_STATUS_NO_PRESENT == enSimStatus)
     || (TAF_SDC_USIM_STATUS_UNVALID == enSimStatus)
     || (TAF_SDC_USIM_STATUS_READY_TIMEROUT == enSimStatus))
    {
        pstSysInfo->ucSrvDomain = TAF_PH_INFO_NO_DOMAIN;
    }

    /* 上电不开机时查询sysinfo或sysinfoex,初始化值为BUTT,AT手册中没有这个值,做一下转换 */
    if (TAF_SDC_SERVICE_DOMAIN_BUTT == pstSysInfo->ucSrvDomain)
    {
        pstSysInfo->ucSrvDomain = TAF_PH_INFO_NO_DOMAIN;
    }

    if (TAF_SDC_REPORT_SRVSTA_BUTT == pstSysInfo->ucSrvStatus)
    {
        pstSysInfo->ucSrvStatus = TAF_PH_INFO_NO_SERV;
    }

}
VOS_VOID  MN_PH_GetSysInfoSysModeEx(
    TAF_PH_SYSINFO_STRU                 *pstSysInfo
)
{
    TAF_SYS_SUBMODE_ENUM_UINT8          enSubMode;

    switch(TAF_SDC_GetSysMode())
    {
        case TAF_SDC_SYS_MODE_GSM:
            pstSysInfo->ucSysMode = MN_PH_SYS_MODE_EX_GSM_RAT;
            enSubMode = TAF_SDC_GetSysSubMode();
            break;

        case TAF_SDC_SYS_MODE_WCDMA:
            if (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
            {
                pstSysInfo->ucSysMode = MN_PH_SYS_MODE_EX_WCDMA_RAT;

                /*W下，^SYSINFO查询的系统子模式是数传态下系统子模式 */
                enSubMode = Sta_GetSysSubMode_W(gstMmaValue.pg_StatusContext->ucDataTranStatus,
                                                            gstMmaValue.pg_StatusContext->ucDataTranMode);
            }
            else
            {
                pstSysInfo->ucSysMode = MN_PH_SYS_MODE_EX_TDCDMA_RAT;

                /*TD下，^SYSINFO查询的系统子模式是数传态下系统子模式 */
                enSubMode = Sta_GetSysSubMode_Td(gstMmaValue.pg_StatusContext->ucDataTranStatus,
                                                            gstMmaValue.pg_StatusContext->ucDataTranMode);
            }
            break;

        case TAF_SDC_SYS_MODE_LTE:
            pstSysInfo->ucSysMode = MN_PH_SYS_MODE_EX_LTE_RAT;

            enSubMode  = TAF_SDC_GetSysSubMode();

            break;

        default:
            pstSysInfo->ucSysMode    = MN_PH_SYS_MODE_EX_NONE_RAT;

            enSubMode                = TAF_SYS_SUBMODE_NONE;
            break;
    }


    /* 将系统子模式进行转换 */
    MN_PH_ConvertSysSubModeToAtFormat(enSubMode, &(pstSysInfo->ucSysSubMode));
}
VOS_VOID MMA_PhoneGetSysInfo(VOS_UINT16     ClientId,
                           VOS_UINT8      OpId)
{
    TAF_PH_SYSINFO_STRU    sys_info;
    VOS_UINT32             ulPinStatus;

    /* 获取当前漫游特性 */
    MN_PH_GetSysInfoRoamStatus(&sys_info);

    sys_info.ucSrvStatus = TAF_SDC_GetServiceStatus();
    sys_info.ucSrvDomain = TAF_SDC_GetServiceDomain();


    /* 获取当前Service Status信息 */
    MN_PH_GetSysInfoSrvStatus(&sys_info);


    /*如果当前需要PIN码、需要PUK码或者PUK码block，MMA给TAF上报的服务域应该为0(无服务)*/
    ulPinStatus = MMA_CheckPin1SatusSimple();

    MMA_INFOLOG1("Pin status ulRet=", (long)ulPinStatus);

    /* 需要PIN码时漫游状态显示为未知 */
    if (MMA_NONEED_PIN1 != ulPinStatus)
    {
        sys_info.ucSrvDomain = TAF_PH_INFO_NO_DOMAIN;
        sys_info.ucRoamStatus = TAF_PH_INFO_ROAM_UNKNOW;
    }

    /*获取当前网络接入技术*/
    MMA_INFOLOG1("g_stTafSdcCtx.stNetworkInfo.stCampPlmnInfo.enSysMode=", TAF_SDC_GetSysMode());
    MMA_INFOLOG1("gstMmaValue.pg_StatusContext->ucDataTranStatus=", gstMmaValue.pg_StatusContext->ucDataTranMode);

    switch(TAF_SDC_GetSysMode())
    {
        case TAF_SDC_SYS_MODE_GSM:
            sys_info.ucSysMode = TAF_PH_INFO_GSM_RAT;
            sys_info.ucSysSubMode = TAF_SDC_GetSysSubMode();
            break;
        case TAF_SDC_SYS_MODE_WCDMA:
            if (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
            {
                sys_info.ucSysMode = TAF_PH_INFO_WCDMA_RAT;

                /* W下，^SYSINFO查询的系统子模式是数传态下系统子模式 */
                sys_info.ucSysSubMode = Sta_GetSysSubMode_W(gstMmaValue.pg_StatusContext->ucDataTranStatus,
                                                            gstMmaValue.pg_StatusContext->ucDataTranMode);
            }
            else
            {
                sys_info.ucSysMode = TAF_PH_INFO_TD_SCDMA_RAT;

                /* TD下，^SYSINFO查询的系统子模式是数传态下系统子模式 */
                sys_info.ucSysSubMode = Sta_GetSysSubMode_Td(gstMmaValue.pg_StatusContext->ucDataTranStatus,
                                                             gstMmaValue.pg_StatusContext->ucDataTranMode);
            }
            break;

#if(FEATURE_ON == FEATURE_LTE)
        case TAF_SDC_SYS_MODE_LTE:
            /*  SYSINFO是GU使用的命令，在L下时，按W显示 */
            sys_info.ucSysMode    = TAF_PH_INFO_WCDMA_RAT;
            sys_info.ucSysSubMode = TAF_SYS_SUBMODE_WCDMA;

            break;
#endif

         default:
            sys_info.ucSysMode = TAF_PH_INFO_NONE_RAT;
            sys_info.ucSysSubMode = TAF_SYS_SUBMODE_NONE;
            break;
    }


    if ( MMA_SIM_IS_LOCK == MMA_GetMeLockStatus() )
    {
        sys_info.ucSimLockStatus = VOS_TRUE;
    }
    else
    {
        sys_info.ucSimLockStatus = VOS_FALSE;
    }

    if (MMA_PUK1_REMAINS_ZERO == ulPinStatus)
    {
        sys_info.ucSimStatus = TAF_PH_USIM_STATUS_UNAVAIL;
    }
    else
    {
        sys_info.ucSimStatus = MMA_GetUsimStatus(VOS_FALSE);
    }

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_SYSINFO_VALUE_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&sys_info);

    return;

}
VOS_VOID MN_PH_GetSysInfoEx(
    VOS_UINT16                         usClientId,
    VOS_UINT8                          ucOpId
)
{
    TAF_PH_SYSINFO_STRU    sys_info;
    VOS_UINT32             ulPinStatus;

    /* 获取当前漫游特性 */
    MN_PH_GetSysInfoRoamStatus(&sys_info);

    sys_info.ucSrvStatus = TAF_SDC_GetServiceStatus();
    sys_info.ucSrvDomain = TAF_SDC_GetServiceDomain();

    /* 获取当前Service Status信息 */
    MN_PH_GetSysInfoSrvStatus(&sys_info);


    /*如果当前需要PIN码、需要PUK码或者PUK码block，MMA给TAF上报的服务域应该为0(无服务)*/
    ulPinStatus = MMA_CheckPin1SatusSimple();

    MMA_INFOLOG1("Pin status ulRet=", (long)ulPinStatus);

    /* 需要PIN码时漫游状态显示为未知 */
    if (MMA_NONEED_PIN1 != ulPinStatus)
    {
        sys_info.ucSrvDomain = TAF_PH_INFO_NO_DOMAIN;
        sys_info.ucRoamStatus = TAF_PH_INFO_ROAM_UNKNOW;
    }

    /*获取当前网络接入技术*/
    MMA_INFOLOG1("g_stTafSdcCtx.stNetworkInfo.stCampPlmnInfo.enSysMode=", TAF_SDC_GetSysMode());
    MMA_INFOLOG1("gstMmaValue.pg_StatusContext->ucDataTranStatus=", gstMmaValue.pg_StatusContext->ucDataTranMode);

    MN_PH_GetSysInfoSysModeEx(&sys_info);

    if ( MMA_SIM_IS_LOCK == MMA_GetMeLockStatus() )
    {
        sys_info.ucSimLockStatus = VOS_TRUE;
    }
    else
    {
        sys_info.ucSimLockStatus = VOS_FALSE;
    }

    if (MMA_PUK1_REMAINS_ZERO == ulPinStatus)
    {
        sys_info.ucSimStatus = TAF_PH_USIM_STATUS_UNAVAIL;
    }
    else
    {
        sys_info.ucSimStatus = MMA_GetUsimStatus(VOS_FALSE);
    }

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(usClientId, ucOpId, TAF_PH_SYSINFO_EX_VALUE_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&sys_info);

    return;
}

VOS_VOID MN_PH_AppRoamStatusRpt( VOS_VOID )
{
    TAF_PH_SYSINFO_STRU                 sys_info;
    VOS_INT32                           lInvalidRssi;

    VOS_INT32                           lRssiValue;
    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode;

    sys_info.ucSrvStatus = TAF_SDC_GetServiceStatus();
    sys_info.ucSrvDomain = TAF_SDC_GetServiceDomain();
    enSysMode            = TAF_SDC_GetSysMode();

    if (TAF_SDC_SYS_MODE_GSM == enSysMode)
    {
        lInvalidRssi = MMC_MMA_UTRA_RSSI_UNVALID;
    }
    else
    {
        lInvalidRssi = MMC_MMA_UTRA_RSCP_UNVALID;
    }

    if (TAF_SDC_SYS_MODE_GSM == enSysMode)
    {
        lRssiValue = gstMmaValue.stCerssiValue.aRssi[0].u.stGCellSignInfo.sRssiValue;
    }
    else
    {
        lRssiValue = gstMmaValue.stCerssiValue.aRssi[0].u.stWCellSignInfo.sRscpValue;
    }

    if(lInvalidRssi >= lRssiValue)
    {
        sys_info.ucSrvStatus = TAF_REPORT_SRVSTA_NO_SERVICE;
        sys_info.ucSrvDomain = TAF_PH_INFO_NO_DOMAIN;
    }


    if (VOS_TRUE == gstMmaValue.pg_StatusContext->ulTimMaxFlg )
    {
        sys_info.ucSrvStatus = TAF_PH_INFO_DEEP_SLEEP;
        sys_info.ucSrvDomain = TAF_PH_INFO_NO_DOMAIN;
    }

    if ( TAF_PH_INFO_NOMRL_SERV != sys_info.ucSrvStatus )
    {
        /* 服务无效 不做漫游判断 */
        MN_INFO_LOG("MN_PH_AppRoamStatusRpt: service is not normal services");
        return;
    }

    if ( TAF_PH_ROAMING == MMA_IsRoam(TAF_SDC_GetCurrCampPlmnId()) )
    {
        sys_info.ucRoamStatus = TAF_PH_INFO_ROMAING;
    }
    else
    {
        sys_info.ucRoamStatus = TAF_PH_INFO_NONE_ROMAING;
    }

    if ( sys_info.ucRoamStatus != g_ucMnPhRoamStatus )
    {
        g_ucMnPhRoamStatus = sys_info.ucRoamStatus;
        MN_PH_RoamingRpt( g_ucMnPhRoamStatus );
    }

}


VOS_VOID MN_PH_GetRssilvForCsqlvlExt (
    VOS_INT32                           lRssiValue,
    VOS_UINT8                           ucCurRaForSysInfo,
    MN_PH_CSQLVLEXT_RSSILV_ENUM_UINT8  *pucRssilv
)
{
    VOS_UINT32                          ulAbsRssiValue;
    VOS_UINT32                          aulGsmRssi[MN_PH_CSQLVLEXT_RSSILV_BUTT] = {105, 90, 84, 82, 80, 76};
    VOS_UINT32                          aulWcdmaRscp[MN_PH_CSQLVLEXT_RSSILV_BUTT] = {105, 102, 96, 94, 92, 88};
    VOS_UINT32                         *pulRssiConvertRule;

    /* 取绝对值 */
    ulAbsRssiValue = (VOS_UINT32)(-lRssiValue);

    if (TAF_PH_RA_GSM == ucCurRaForSysInfo)
    {
        pulRssiConvertRule = aulGsmRssi;
    }
    else
    {
        pulRssiConvertRule = aulWcdmaRscp;
    }

    /* 根据SRS中描述的信号强度对应关系获取到信号强度格数 */
    if (ulAbsRssiValue <= pulRssiConvertRule[MN_PH_CSQLVLEXT_RSSILV_5])
    {
        *pucRssilv = MN_PH_CSQLVLEXT_RSSILV_VALUE_99; /* 5格 */
    }
    else if (ulAbsRssiValue <= pulRssiConvertRule[MN_PH_CSQLVLEXT_RSSILV_4])
    {
        *pucRssilv = MN_PH_CSQLVLEXT_RSSILV_VALUE_80; /* 4格 */
    }
    else if (ulAbsRssiValue <= pulRssiConvertRule[MN_PH_CSQLVLEXT_RSSILV_3])
    {
        *pucRssilv = MN_PH_CSQLVLEXT_RSSILV_VALUE_60; /* 3格 */
    }
    else if (ulAbsRssiValue <= pulRssiConvertRule[MN_PH_CSQLVLEXT_RSSILV_2])
    {
        *pucRssilv = MN_PH_CSQLVLEXT_RSSILV_VALUE_40; /* 2格 */
    }
    else if (ulAbsRssiValue <= pulRssiConvertRule[MN_PH_CSQLVLEXT_RSSILV_1])
    {
        *pucRssilv = MN_PH_CSQLVLEXT_RSSILV_VALUE_20; /* 1格 */
    }
    else
    {
        *pucRssilv = MN_PH_CSQLVLEXT_RSSILV_VALUE_0;  /* 0格 */
    }

    return;
}


VOS_VOID MN_PH_GetCsqLvlExtPara(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
)
{
    MN_PH_CSQLVLEXT_PARA_STRU           stCsqLvlExtPara;

    VOS_UINT32                          i;

    /* 根据信号强度获取到CSQLVLEXT命令的信号格数，CSQLVLEXT仅关心INDEX为0的RSSI值 */
    if (TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE != TAF_SDC_GetServiceStatus())
    {
        stCsqLvlExtPara.enRssilv = MN_PH_CSQLVLEXT_RSSILV_VALUE_0;
    }
    else
    {
        /*获取当前RSSI的值,调用API*/
        gstMmaValue.stRssiValue.ucRssiNum = gstMmaValue.stCerssiValue.ucRssiNum;

        for (i = 0; i < gstMmaValue.stCerssiValue.ucRssiNum; i++)
        {
            if (TAF_SDC_SYS_MODE_GSM == TAF_SDC_GetSysMode())
            {
                gstMmaValue.stRssiValue.astRssi[i].lRssiValue = gstMmaValue.stCerssiValue.aRssi[i].u.stGCellSignInfo.sRssiValue;
            }
            else
            {
                gstMmaValue.stRssiValue.astRssi[i].lRssiValue = gstMmaValue.stCerssiValue.aRssi[i].u.stWCellSignInfo.sRscpValue;
            }

            gstMmaValue.stRssiValue.astRssi[i].ucChannalQual  = gstMmaValue.stCerssiValue.aRssi[i].ucChannalQual;
            gstMmaValue.stRssiValue.astRssi[i].ucRssiValue4AT = 0;
        }


        /* 当前支持一个服务小区所以信号强度用数组中第一个数据表示 */
        MN_PH_GetRssilvForCsqlvlExt(gstMmaValue.stRssiValue.astRssi[0].lRssiValue,
                                    TAF_SDC_GetSysMode(),
                                    &stCsqLvlExtPara.enRssilv);

    }

    /* 当前版本不支持CSQLVLEXT命令的比特误码率百分比查询，固定返回99 */
    stCsqLvlExtPara.enBer = MN_PH_CSQLVLEXT_BER_VALUE_99;

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_CSQLVLEXT_VALUE_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stCsqLvlExtPara);

    return;

}

VOS_VOID MMA_GetPhIccType(VOS_UINT16 ClientId,VOS_UINT8 OpId)

{
    TAF_PH_UICC_TYPE stIcc_Type;
    VOS_UINT8 ucSimType;
    VOS_UINT8 usPinType;


    if(TAF_PH_USIM_ROMSIM == MMA_GetUsimStatus(VOS_FALSE))
    {
        stIcc_Type.MmaIccType = TAF_PH_ICC_UNKNOW;
    }
    else
    {
        MMA_PhoneGetUsimType(&ucSimType);

        if(MMA_USIM_TYPE_USIM == ucSimType)
        {
            stIcc_Type.MmaIccType = TAF_PH_ICC_USIM;
        }
        else if(MMA_USIM_TYPE_SIM == ucSimType)
        {
            stIcc_Type.MmaIccType = TAF_PH_ICC_SIM;
        }
        else
        {
        }
    }

    usPinType = USIMM_PIN;

    NAS_USIMMAPI_GetPinStatus(&usPinType, &stIcc_Type.MmaIccPinStatus, &stIcc_Type.MmaIccPinVerifyStatus, &stIcc_Type.ucRemainTimes);

    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_ICC_TYPE_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)(&stIcc_Type));
    return;
}



VOS_VOID MMA_GetPhIccStatus(VOS_UINT16 ClientId,VOS_UINT8 OpId)
{
    TAF_USIM_STATUS                     ICC_STATE;
    ICC_STATE                           = MMA_GetUsimStatus(VOS_FALSE);
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_ICC_STATUS_PARA,TAF_ERR_NO_ERROR, &ICC_STATE);
    return;
}



VOS_VOID TAF_MMA_SndAtIccStatusInd(
    VOS_UINT8                           ucSimStatus,
    VOS_UINT8                           ucSimLockStatus
)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent = VOS_NULL_PTR;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("TAF_MMA_SndAtIccStatusInd:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    /* 初始化电话上报事件 */
    MMA_InitEventInfoOP(pstPhoneEvent);

    /* 填写事件类型为卡状态信息指示:TAF_PH_EVT_USIM_INFO_IND*/
    pstPhoneEvent->PhoneEvent             = TAF_PH_EVT_USIM_INFO_IND;

    /* 填写事件客户端类型为广播事件 */
    pstPhoneEvent->ClientId               = MMA_CLIENTID_BROADCAST;
    pstPhoneEvent->OpId                   = MMA_OP_ID_INTERNAL;

    /* 填写上报的卡状态和锁定状态 */
    pstPhoneEvent->SimStatus              = ucSimStatus;
    pstPhoneEvent->MeLockStatus           = ucSimLockStatus;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
    return;
}



VOS_VOID MMA_QueryProc(VOS_UINT16 ClientId,
                       VOS_UINT8        OpId,
                       VOS_UINT8     *pMsg)
{
    TAF_PARA_TYPE ParaQuery;

    ParaQuery = (TAF_PARA_TYPE)(*pMsg);

    if(  (ParaQuery > (TAF_PH_PRODUCT_NAME_PARA + 1))
       ||(ParaQuery < TAF_PH_MFR_ID_PARA)
      )
    {
          MMA_WARNINGLOG("MMA_QueryProc():WARNING:Receive Error ParaQuery");
          MMA_ReportParaQueryError(ClientId, OpId,
                                   TAF_ERR_CAPABILITY_ERROR,
                                   ParaQuery);
          return;
    }

    /*根据查询类型值，分发调用函数*/
    switch ( ParaQuery)
    {
        /* 获取CSQLVLEXT命令的信号参数 */
        case TAF_PH_CSQLVLEXT_VALUE_PARA:
            MN_PH_GetCsqLvlExtPara(ClientId, OpId);
            break;
        case TAF_PH_CSQLVL_VALUE_PARA:
            MN_PH_GetCsqLvlPara(ClientId, OpId);
            break;
        /*厂商信息查询*/
        case TAF_PH_MFR_ID_PARA:
            MMA_PhoneGetMfrId(ClientId, OpId);
            break;

        /*硬件版本信息查询*/
        case TAF_PH_MODEL_ID_PARA :
            MMA_PhoneGetModelId(ClientId, OpId);
            break;

        /*软件版本信息查询*/
        case  TAF_PH_REVISION_ID_PARA:
            MMA_PhoneGetRevisionId(ClientId, OpId);
            break;

        /*获取IMSI*/
        case TAF_PH_IMSI_ID_PARA:
            MMA_PhoneGetImsi(ClientId, OpId);
            break;

        /*获取当前注册状态*/
        case TAF_PH_CS_REG_STATE_PARA:
            MMA_PhoneGetRegStatus(ClientId, OpId);
            break;

        /*获取RSSI值*/
        case  TAF_PH_RSSI_VALUE_PARA:
            MMA_PhoneGetRssi(ClientId, OpId);
            break;

        /*==>A32D06630*/
        case TAF_PH_CELL_RSCP_VALUE_PARA:   /*Get cell RCSP & Cell Info*/
            MMA_GetPhCellRscp(ClientId, OpId);
            break;

        case TAF_PH_UE_RFPOWER_FREQ_PARA:   /*Get UE's RF power and uplink frequency*/
            MMA_GetPhUeRfPowerFreq(ClientId, OpId);
            break;
        /*<==A32D06630*/

        /*获取当前电源状态*/
        case  TAF_PH_BATTERY_POWER_PARA:
            MMA_PhoneGetBatteryPower(ClientId, OpId);
            break;

        /*获取当前网络运营商名字（COPS）*/
        case TAF_PH_NETWORKNAME_PARA:
            MMA_PhoneGetNetworkName(ClientId, OpId);
            break;

        /* 删除MMA_PhoneGetCellRoam */

        /*获取网络运营商名（SPN）*/
        case TAF_PH_NETWORKNAMEFROMUSIM_PARA:
            MMA_PhoneGetNetworkNameFromUsim(ClientId, OpId);
            break;

        /*获取手机当前服务状态*/
        case TAF_PH_SRV_STATUS_PARA :
            MMA_PhoneGetSrvStatus(ClientId, OpId);
            break;

        /* 删除MMA_PhoneGetRoamSta */

        /*获取域状态*/
        case TAF_PH_DOMAIN_PARA :
            MMA_PhoneGetDomain(ClientId,OpId);
            break;

        /* case TAF_PH_WHOLE_DATA */

        /*获取手机产品名称*/
        case TAF_PH_PRODUCT_NAME_PARA:
            MMA_PhoneGetProductName(ClientId, OpId);
            break;

        /* case TAF_PH_IDENTIFICATION_INFO_PARA */


        /*对应CGREG命令*/
        case TAF_PH_PS_REG_STATE_PARA:
            MMA_PhoneGetGmmRegStatus(ClientId, OpId);
            break;

#if(FEATURE_ON == FEATURE_LTE)
        case TAF_PH_EPS_REG_STATE_PARA:
            NAS_MMA_GetEpsRegStatus(ClientId, OpId);
            break;

#endif
        case TAF_PH_ICC_ID:
            MMA_PhoneGetIccId(ClientId, OpId);
            break;
        case TAF_PH_SYSINFO_VALUE_PARA :
            MMA_PhoneGetSysInfo(ClientId, OpId);
            break;

        case TAF_PH_SYSINFO_EX_VALUE_PARA:
            MN_PH_GetSysInfoEx(ClientId, OpId);
            break;

        case TAF_PH_ICC_TYPE_PARA:  /*Get the UICC type*/
            MMA_GetPhIccType(ClientId, OpId);
            break;
        case TAF_PH_ICC_STATUS_PARA:    /*Get the UICC state*/
            MMA_GetPhIccStatus(ClientId, OpId);
            break;
        case TAF_PH_QUICK_START_PARA:
            MMA_GetQuickStartStatus(ClientId, OpId);
            break;
        case TAF_PH_AUTO_ATTACH_PARA:
            MMA_GetPhAutoAttachStatus(ClientId, OpId);
            break;
        /* 禁止列表查询 */
        case TAF_PH_FPLMN_PARA:
            MMA_FplmnQurey(ClientId, OpId);
            break;
        case TAF_PH_HS_PARA:
            MMA_HSQuery(ClientId, OpId);
            break;

        case TAF_PH_REGISTER_TIME_VALUE_PARA:
            MMA_RegisterTimeQuery(ClientId, OpId);
            break;
        case TAF_PH_ANQUERY_VALUE_PARA:
            MMA_AnQueryProc(ClientId, OpId);
            break;
        case TAF_PH_HOMEPLMN_VALUE_PARA:
            MMA_HomePlmnProc(ClientId, OpId);
            break;
        case TAF_PH_CSNR_VALUE_PARA:
            MMA_CsnrParaProc(ClientId, OpId);
            break;
        case TAF_PH_SPN_VALUE_PARA:
            MMA_SpnParaProc(ClientId, OpId);
            break;
        case TAF_PH_SIMLOCK_VALUE_PARA:
            TAF_MMA_RcvSimLockQry(ClientId, OpId);
            break;

        case TAF_MM_PLMN_INFO_QRY_PARA:
            TAF_MMA_QryMmPlmnInfo(ClientId, OpId);
            break;

        case TAF_PH_PLMN_QRY_PARA:
            TAF_MMA_QryPlmnPara(ClientId, OpId);
            break;


        case TAF_PH_USER_SRV_STATE_QRY_PARA:
            TAF_MMA_QryUserSrvStatePara(ClientId, OpId);
            break;

        default:
            MMA_WARNINGLOG("MMA_QueryProc():WARNING:No funcation fit the case");
            break;
    }
    return;
}
VOS_VOID MMA_PhoneGetProductName( VOS_UINT16     ClientId, VOS_UINT8    OpId )
{
    VOS_UINT8     ucParalen;
    VOS_VOID      *pPara;

    /*发送数据准备*/
    /*获取域状态信息长度*/
    ucParalen = (VOS_UINT8)VOS_StrLen((VOS_CHAR *)(gstMmaValue.stMeInfo.ProductName.aucProductName)) + 1;
    if ( ucParalen > TAF_PH_PRODUCT_NAME_LEN)
    {
        gstMmaValue.stMeInfo.ProductName.aucProductName[TAF_PH_PRODUCT_NAME_LEN] = '\0';
        /*增加一个warning打印*/
        MMA_WARNINGLOG("MMA_PhoneGetProductName():WARNING:Length of Product is too Long!");
    }

    pPara = (VOS_VOID*)gstMmaValue.stMeInfo.ProductName.aucProductName;
    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_PRODUCT_NAME_PARA,TAF_ERR_NO_ERROR, pPara);

    return;
}

/*MMA_PhoneGetIdentificationInfo()
  MMA_PhoneGetRevisionIdentification()
  MMA_PhoneGetWholeInfo()
  MMA_FillWholeInfo()*/

TAF_PH_ROAM_STA MMA_IsRoam(TAF_SDC_PLMN_ID_STRU *pstCurPlmnId)
{
    TAF_SDC_REG_STATUS_ENUM_UINT8       enCsRegStatus;
    TAF_SDC_REG_STATUS_ENUM_UINT8       enPsRegStatus;

    enCsRegStatus  = TAF_SDC_GetCsRegStatus();
    enPsRegStatus  = TAF_SDC_GetPsRegStatus();

    if ((TAF_SDC_REG_REGISTERED_HOME_NETWORK == enCsRegStatus)
     || (TAF_SDC_REG_REGISTERED_HOME_NETWORK == enPsRegStatus))
    {
        return TAF_PH_NO_ROAM;
    }
    /* 成功注册到漫游网络上时返回TAF_PH_ROAMING，限制服务时返回TAF_PH_ROAM_UNKNOW */
    if ((TAF_SDC_REG_REGISTERED_ROAM == enCsRegStatus)
     || (TAF_SDC_REG_REGISTERED_ROAM == enPsRegStatus))
    {
        return TAF_PH_ROAMING;
    }
    else
    {
        return TAF_PH_ROAM_UNKNOW;
    }

}
VOS_VOID MMA_ReportParaQueryError( VOS_UINT16       ClientId,
                                   VOS_UINT8              OpId,
                                   VOS_UINT16           usErrorCode,
                                   TAF_PARA_TYPE ParaType)
{
    if (TAF_ERR_NO_ERROR != usErrorCode)
    {
        MMA_WARNINGLOG1("MMA_ReportParaQueryError():WARNING:Para query falied, Error Code = %d", usErrorCode);

        /*调用APP参数查询失败上报函数*/
        MN_QRYPARA_SendMsg(ClientId, OpId, ParaType,usErrorCode, TAF_NULL_PTR);

    }
    return;
}
VOS_VOID*  MMA_ParaQueryReport(VOS_UINT16            ClientId,
                              VOS_UINT8                   OpId,
                              TAF_PARA_TYPE      QueryType,
                              VOS_UINT16         usErrorCode,
                              VOS_VOID                 *pucPara)
{
    /*调用APP查询参数上报函数*/
    MN_QRYPARA_SendMsg(ClientId, OpId, QueryType,usErrorCode, pucPara);

    return pucPara;
}
VOS_UINT32  MMA_TiAlloc(VOS_UINT8        ucModule,
                        VOS_UINT16    ClientId,
                        VOS_UINT8           Id,
                        TAF_PHONE_EVENT  PhoneEvt,
                        VOS_UINT8       *pucTi)
{

    if (VOS_NULL_PTR == pucTi)
    {
        MMA_WARNINGLOG("MMA_TiAlloc():WARNING:pucTi is NULL PTR!");
        return MMA_FAILURE;
    }
    switch (ucModule)
    {
        case TAF_MMA:
            if ( TAF_PH_EVT_OP_PIN_CNF == PhoneEvt )
            {
                if ( MMA_TI_USED == gastMmaTiTab[MMA_PH_PIN_TI].ucUsed)
                {
                    return MMA_FAILURE;
                }
                gastMmaTiTab[MMA_PH_PIN_TI].ClientId          = ClientId;
                gastMmaTiTab[MMA_PH_PIN_TI].Id                = Id;
                gastMmaTiTab[MMA_PH_PIN_TI].PhoneEvent        = PhoneEvt;
                /* BEGIN: Modified by liuyang id:48197, 2006/3/8   PN:A32D02371*/
                gastMmaTiTab[MMA_PH_PIN_TI].ucUsed            = MMA_TI_USED;
                /* END:   Modified by liuyang id:48197, 2006/3/8 */
                return TAF_SUCCESS;
            }
            switch (gstMmaValue.pg_StatusContext->ulFsmState)
            {
                case STA_FSM_NULL:
                case STA_FSM_ENABLE:
                case STA_FSM_RESTART:
                case STA_FSM_STOP :
                    /*TI分配成功*/
                    gastMmaTiTab[0].ClientId          = ClientId;
                    gastMmaTiTab[0].Id                = Id;
                    gastMmaTiTab[0].PhoneEvent        = PhoneEvt;
                    gastMmaTiTab[0].ucUsed            = MMA_TI_USED;
                    *pucTi = 0;
                    return MMA_SUCCESS;

                case STA_FSM_DE_ATTACH:
                case STA_FSM_PLMN_LIST:
                case STA_FSM_PLMN_RESEL:
                case STA_FSM_PLMN_SEL:
                case STA_FSM_SYS_CFG_SET:
                    /*TI分配成功*/
                    gastMmaTiTab[1].ClientId          = ClientId;
                    gastMmaTiTab[1].Id                = Id;
                    gastMmaTiTab[1].PhoneEvent        = PhoneEvt;
                    gastMmaTiTab[1].ucUsed            = MMA_TI_USED;
                    *pucTi = 1;
                    return MMA_SUCCESS;
                default:
                    MMA_WARNINGLOG("MMA_TiAlloc():WARNING:Error in Status FSM!");
                    return MMA_FAILURE;
            }
       default:
            /*增加打印信息*/
            MMA_WARNINGLOG("MMA_TiAlloc():WARNING:Error input Module Name!");
            return MMA_FAILURE;
    }
}




/**********************************************************
 Function:     MMA_GetIdByTi
 Description:  从TI记录表中用TI获取关联的ClientId和Id
 Calls:
 Data Accessed:
 Data Updated:
 Input:        ucModule－子模块类型：TAF_STATUS,
               Ti－TI值
 Output:       pClientId－APP、AT标识
               pId－ APP、AT操作标识
 Return:       MMA_SUCCESS－成功
               MMA_FAILURE－失败
 Others:

  1.日    期   : 2006年3月8日
    作    者   : liuyang id:48197
    修改内容   : 问题单号：A32D02371，使用宏定义替换原有的常数
**********************************************************/
VOS_UINT32  MMA_GetIdByTi (VOS_UINT8         ucModule,
                           VOS_UINT8         ucTi,
                           VOS_UINT16    *pClientId,
                           VOS_UINT8           *pId,
                           TAF_PHONE_EVENT  *PhoneEvt)
{
    /*参数判别有效性*/
    if ( (VOS_NULL_PTR == pClientId) || (VOS_NULL_PTR == pId) )
    {
        MMA_WARNINGLOG("MMA_GetIdByTi():WARNING:pClientId & pId are NULL PTR!!");
        return MMA_FAILURE;
    }
    switch (ucModule)
    {
        case TAF_MMA:
            /*Ti=2专门用于PIN操作*/
            if ( MMA_PH_PIN_TI == ucTi )
            {
                *pClientId = gastMmaTiTab[MMA_PH_PIN_TI].ClientId;
                *pId       = gastMmaTiTab[MMA_PH_PIN_TI].Id;
                *PhoneEvt  = gastMmaTiTab[MMA_PH_PIN_TI].PhoneEvent;
                /* BEGIN: Modified by liuyang id:48197, 2006/3/8   PN:A32D02371*/
                /*清除使用标志*/
                /*gastMmaTiTab[MMA_PH_PIN_TI].ucUsed   = 0;
                gastMmaTiTab[MMA_PH_PIN_TI].ClientId = 0;
                gastMmaTiTab[MMA_PH_PIN_TI].Id       = 0;*/
                gastMmaTiTab[MMA_PH_PIN_TI].ucUsed   = MMA_TI_UNUSED;
                gastMmaTiTab[MMA_PH_PIN_TI].ClientId = MMA_CLIENTID_NONE;
                gastMmaTiTab[MMA_PH_PIN_TI].Id       = MMA_OP_ID_INTERNAL;
                /* END:   Modified by liuyang id:48197, 2006/3/8 */
                return MMA_SUCCESS;
            }
            if (ucTi > TAF_MAX_STATUS_TI)
            {   /*TI有效范围是0-1*/
                MMA_WARNINGLOG("MMA_GetIdByTi():WARNNING:TI is out of MAX");
                return MMA_FAILURE;
            }
            /* 不需要判断g_TafPhoneTiTab[ucTi].ucUsed,因为分配时没有使用*/
            *pClientId = gastMmaTiTab[ucTi].ClientId;
            *pId       = gastMmaTiTab[ucTi].Id;
            *PhoneEvt  = gastMmaTiTab[ucTi].PhoneEvent;
            /* BEGIN: Modified by liuyang id:48197, 2006/3/8   PN:A32D02371*/
            /*清除使用标志*/
            /*gastMmaTiTab[ucTi].ucUsed   = 0;
            gastMmaTiTab[ucTi].ClientId = 0;
            gastMmaTiTab[ucTi].Id       = 0;*/
            gastMmaTiTab[ucTi].ucUsed   = MMA_TI_UNUSED;
            gastMmaTiTab[ucTi].ClientId = MMA_CLIENTID_NONE;
            gastMmaTiTab[ucTi].Id       = MMA_OP_ID_INTERNAL;
            /* END:   Modified by liuyang id:48197, 2006/3/8 */
            return MMA_SUCCESS;
        default:
            /*需增加打印信息*/
            MMA_WARNINGLOG("MMA_GetIdByTi():WARNING:Error input Module Name!");
            return MMA_FAILURE;
    }
}

/* MMA_InitUsimInfo删除 */
/* MMA_UpdateBandNvRefForbBand放到TafMmaProcNvim文件中 */
/* MMA_UpdateForbBandStatusToNV放到TafMmaProcNvim文件中 */

/* TAF_MMA_InitOrigMeInfo移动位置 */




VOS_VOID MN_MMA_SetDefaultRatPrioList(
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList
)
{
    VOS_UINT8                           i;
    VOS_UINT32                          ulGsmForbiddenFlg;

    i = 0;

#if (FEATURE_ON == FEATURE_LTE)
    /* 平台接入技术支持LTE时才能设置支持LTE */
    if (VOS_TRUE == TAF_SDC_IsPlatformSupportLte())
    {
        pstRatPrioList->aenRatOrder[i] = TAF_PH_RAT_LTE;
        i ++;
    }
#endif

    /* 平台接入技术支持UTRAN时才能设置支持WCDMA */
    if (VOS_TRUE == TAF_SDC_IsPlatformSupportUtran())
    {
        pstRatPrioList->aenRatOrder[i] = TAF_PH_RAT_WCDMA;
        i ++;
    }

    /* 平台接入技术支持GSM而且硬件支持GSM频段时才能设置支持GSM */
    ulGsmForbiddenFlg = MMA_IsGsmForbidden();
    if ((VOS_TRUE == TAF_SDC_IsPlatformSupportGsm())
     && (MMA_FALSE == ulGsmForbiddenFlg))
    {
        pstRatPrioList->aenRatOrder[i] = TAF_PH_RAT_GSM;
        i ++;
    }

    pstRatPrioList->ucRatOrderNum  = i;

    /* 其它赋值为BUTT */
    for (; i < TAF_PH_MAX_GUL_RAT_NUM; i ++)
    {
        pstRatPrioList->aenRatOrder[i] = TAF_PH_RAT_BUTT;
    }

    return;
}


VOS_VOID TAF_MMA_ReadNvimRatPrioList(
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          k;
    TAF_PH_NVIM_RAT_ORDER_STRU          stRatPrioList;
    VOS_UINT32                          ulLength;

    ulLength        = 0;
    NV_GetLength(en_NV_Item_RAT_PRIO_LIST, &ulLength);
    if (ulLength > sizeof(TAF_PH_NVIM_RAT_ORDER_STRU))
    {
        return;
    }

    PS_MEM_SET(&stRatPrioList, 0X0, ulLength);

    /* 从NV中获取接入技术优先级 */
    if (NV_OK != NV_Read (en_NV_Item_RAT_PRIO_LIST, &stRatPrioList,
                         ulLength))
    {
        MN_MMA_SetDefaultRatPrioList(pstRatPrioList);

        return;
    }

    PS_MEM_CPY(pstRatPrioList, &stRatPrioList, ulLength);

    /* 检查NV数据有效性 */
    for ( i = 0 ; i < pstRatPrioList->ucRatOrderNum; i++ )
    {
        /* 判断是否存在无效的接入技术 */
        if (pstRatPrioList->aenRatOrder[i] >= TAF_PH_RAT_BUTT)
        {
            MN_MMA_SetDefaultRatPrioList(pstRatPrioList);

            return;
        }

        /* 判断是否存在重复的接入技术 */
        for (k = i + 1 ; k < pstRatPrioList->ucRatOrderNum; k++ )
        {
            if (pstRatPrioList->aenRatOrder[i] == pstRatPrioList->aenRatOrder[k])
            {
                MN_MMA_SetDefaultRatPrioList(pstRatPrioList);

                return;
            }
        }
    }

    /* 删除平台不支持的接入技术 */
    TAF_MMA_DelPlatformUnsupportedRat(pstRatPrioList);

    /* 如果所有接入技术平台都不支持，使用平台默认值 */
    if (0 == pstRatPrioList->ucRatOrderNum)
    {
        MN_MMA_SetDefaultRatPrioList(pstRatPrioList);
    }
    else
    {
        /* 其它赋值为BUTT */
        for (i = pstRatPrioList->ucRatOrderNum; i < TAF_PH_MAX_GUL_RAT_NUM; i ++)
        {
            pstRatPrioList->aenRatOrder[i] = TAF_PH_RAT_BUTT;
        }
    }

    return;
}



/* 删除 MMA_InitMeInfo */


VOS_VOID MMA_RdWt_StordedClientOpID(MMA_STORED_CLIENT_OPID_OP_ENUM  enOpType)
{
    if(MMA_STORED_CLIENT_OPID_OP_WRITE == enOpType)
    {
        gstMmaStoredClientOPID.ausClientID[0] = gstMmaValue.stOpPinData.ClientId;
        gstMmaStoredClientOPID.aucOPID[0]     = gstMmaValue.stOpPinData.OpId;

        gstMmaStoredClientOPID.ausClientID[1] = gstMmaValue.stSysCfgSet.usClientId;
        gstMmaStoredClientOPID.aucOPID[1]     = gstMmaValue.stSysCfgSet.ucOpId;

        gstMmaStoredClientOPID.ausClientID[2] = gstMmaValue.stGetIccId.ClientId;
        gstMmaStoredClientOPID.aucOPID[2]     = gstMmaValue.stGetIccId.OpId;

        gstMmaStoredClientOPID.ausClientID[3] = gstMmaValue.stPNN.ClientId;
        gstMmaStoredClientOPID.aucOPID[3]     = gstMmaValue.stPNN.OpId;

        gstMmaStoredClientOPID.ausClientID[4] = gstMmaValue.stOPL.ClientId;
        gstMmaStoredClientOPID.aucOPID[4]     = gstMmaValue.stOPL.OpId;

        gstMmaStoredClientOPID.ausClientID[5] = gstMmaValue.stCPNN.ClientId;
        gstMmaStoredClientOPID.aucOPID[5]     = gstMmaValue.stCPNN.OpId;
    }
    else if(MMA_STORED_CLIENT_OPID_OP_READ == enOpType)
    {
         gstMmaValue.stOpPinData.ClientId = gstMmaStoredClientOPID.ausClientID[0];
         gstMmaValue.stOpPinData.OpId     = gstMmaStoredClientOPID.aucOPID[0];

         gstMmaValue.stSysCfgSet.usClientId = gstMmaStoredClientOPID.ausClientID[1];
         gstMmaValue.stSysCfgSet.ucOpId     = gstMmaStoredClientOPID.aucOPID[1];

         gstMmaValue.stGetIccId.ClientId  = gstMmaStoredClientOPID.ausClientID[2];
         gstMmaValue.stGetIccId.OpId      = gstMmaStoredClientOPID.aucOPID[2];

         gstMmaValue.stPNN.ClientId       = gstMmaStoredClientOPID.ausClientID[3];
         gstMmaValue.stPNN.OpId           = gstMmaStoredClientOPID.aucOPID[3];

         gstMmaValue.stOPL.ClientId       = gstMmaStoredClientOPID.ausClientID[4];
         gstMmaValue.stOPL.OpId           = gstMmaStoredClientOPID.aucOPID[4];

         gstMmaValue.stCPNN.ClientId       = gstMmaStoredClientOPID.ausClientID[5];
         gstMmaValue.stCPNN.OpId           = gstMmaStoredClientOPID.aucOPID[5];
    }
    else
    {
    }

}

/* TAF_MMA_InitRssiValue移动位置 */
/* MMA_InitGobalValue删除 */



VOS_VOID    MMA_InitCm(VOS_VOID)
{

#ifndef __PS_WIN32_RECUR__
    TAF_MMA_SndPowerIndMsg();
#else
    Aps_PowerOff();
#endif

    NAS_RabmPowerOff();

    Sm_PowerOff();

    MN_CALL_PowerOff();

    MN_MSG_PowerOff();

    NAS_CC_PowerOff();

    SMS_Poweroff();

    SSA_PowerOff();

    Tc_TaskInit();

}

/* 删除MMA_Init1 */

VOS_VOID  MMA_ReadSimCPHSOperNameStr(VOS_VOID)
{
    VOS_UINT8    ucSimType;
    VOS_UINT32   ulRet;

    USIMM_GET_FILE_INFO_STRU stGetFileInfo;

    /*UE 不支持CPHS,不进行读取 */
    if (NV_ITEM_DEACTIVE == g_usMmaUeSupportCPHSFlg)
    {
        return;
    }

    MMA_PhoneGetUsimType(&ucSimType);

    /*只有SIM卡才可能使用CPHS文件*/
    if (MMA_USIM_TYPE_SIM != ucSimType)
    {
        return;
    }
    TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                    USIMM_GSM_APP,
                                    TAF_CPHS_SIM_ONS_EFID,
                                    0);

    ulRet = NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA, 0, &stGetFileInfo);

    if (USIMM_API_SUCCESS != ulRet)
    {
        MMA_ERRORLOG("MMA_ReadSimCPHSOperNameStr():ERROR:Get File Req failed");
    }

    /*从CPHS类型的SIM卡中获取OperNameString文件,文件ID:0x6F14*/
    /*启动定时器*/
    if( VOS_OK != NAS_StartRelTimer( &g_MmaSimTimer[MN_PH_SIM_TIMER_ENUM_CPHS_ONS].MmaHTimer,
                                      WUEPS_PID_MMA,
                                      MMA_GET_FILE_TIME,
                                      TAF_MMA,
                                      TI_MN_PH_CPHS_SIM_ONS,
                                      VOS_RELTIMER_NOLOOP )
                                      )
    {
        MMA_ERRORLOG("MMA_ReadSimCPHSOperNameStr():ERROR:VOS_StartRelTimer runs failed!");
        g_MmaSimTimer[MN_PH_SIM_TIMER_ENUM_CPHS_ONS].ucTimerStatus = MMA_TIMER_ERROR;
    }
    else
    {
        g_MmaSimTimer[MN_PH_SIM_TIMER_ENUM_CPHS_ONS].ucTimerStatus = MMA_TIMER_RUN;
    }

}




VOS_BOOL   MMA_IsNeedToDisplayCPHSOperNameStr(VOS_VOID)
{
    VOS_BOOL                            bRptFlg;

    bRptFlg = VOS_FALSE;

    /* CPHS文件有效，且SPN无效时，上报^PNN,^OPL */
    if (VOS_TRUE == gstMmaValue.stUsimInfo.stSimCPHSOperName.bCPHSOperNameValid)
    {
        if ((TAF_PH_SPN_TYPE_UNKNOW == gstMmaValue.stUsimInfo.SimSpnInfo.SpnType)
          || (0xff == gstMmaValue.stUsimInfo.SimSpnInfo.aucSpnName[0]))
        {
            bRptFlg = VOS_TRUE;
        }
    }

    return bRptFlg;
}
VOS_VOID   MMA_ReportOPLParaForCPHSDisplay(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId
)
{
    TAF_PH_USIM_OPL_CNF_STRU            *pstOPL;

    pstOPL   = (TAF_PH_USIM_OPL_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                            sizeof(TAF_PH_USIM_OPL_CNF_STRU));

    if(VOS_NULL_PTR == pstOPL)
    {
        MMA_ReportParaQueryError(ClientId, OpId,
                            TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_OPL_PARA);
        return;
    }

    MMA_MEM_SET(pstOPL, 0, sizeof(TAF_PH_USIM_OPL_CNF_STRU));

    MMA_ParaQueryReport(ClientId, OpId,
                        TAF_PH_OPL_PARA, TAF_ERR_NO_ERROR, pstOPL);

    PS_MEM_FREE(WUEPS_PID_MMA, pstOPL);

    return;

}


VOS_UINT32   MMA_CovertCPHSOperNameStrToPNNFormat(
    TAF_PH_USIM_PNN_RECORD             *pstPNNRcd
)
{
    VOS_UINT8                          *pCPHSOperNameStr;
    VOS_UINT8                           ucSpareBitNumInLastOctet;
    VOS_UINT8                           ucOctet3InNetworkName;
    VOS_UINT32                          ulSrcStrLen;
    VOS_UINT32                          ulDestStrLen;

    if ((VOS_NULL_PTR == pstPNNRcd)
     || (VOS_FALSE == gstMmaValue.stUsimInfo.stSimCPHSOperName.bCPHSOperNameValid))
    {
        return MMA_ERROR;
    }

    pCPHSOperNameStr = gstMmaValue.stUsimInfo.stSimCPHSOperName.aucCPHSOperNameStr;

    PS_MEM_SET(pstPNNRcd->PNN,0x00,sizeof(TAF_PH_USIM_PNN_RECORD));

    for ( ulSrcStrLen = 0 ; ulSrcStrLen < TAF_PH_OPER_NAME_LONG ;  ulSrcStrLen++ )
    {
         if ( 0xFF == pCPHSOperNameStr[ulSrcStrLen] )
         {
            break;
         }
    }
    /* 将 bit 8为0的Gsm 7bit转换为压缩的 7bit编码 */
    ulDestStrLen = (VOS_UINT32)(((ulSrcStrLen * 7) + 7) / 8); /* 压缩后的字节个数 */

    if (ulDestStrLen <= (sizeof(pstPNNRcd->PNN)-3))
    {
        /* 从第4个 字节开始为实际的 Text String*/
        if (VOS_OK != TAF_STD_Pack7Bit(pCPHSOperNameStr,
                                                ulSrcStrLen,
                                                0,
                                                (pstPNNRcd->PNN + 3),
                                                &ulDestStrLen))
        {
            MMA_ERRORLOG("MMA_CovertCPHSOperNameStrToPNNFormat():Error:TAF_STD_Pack7Bit failed!");
            return MMA_ERROR;
        }
    }
    else
    {
        return MMA_ERROR;
    }
    /* octet3: ext 1,coding scheme:Gsm 7bit, Add CI:0,Number of spare bits in last octet */
    ucSpareBitNumInLastOctet = (VOS_UINT8)((ulDestStrLen * 8) - (ulSrcStrLen * 7));
    ucOctet3InNetworkName = (VOS_UINT8)(0x80 | ucSpareBitNumInLastOctet);

    /*将压缩后的值，按TLV格式 填充到 pstPNNRcd中*/
    pstPNNRcd->PNN[0] = FULL_NAME_IEI;
    pstPNNRcd->PNN[1] = (VOS_UINT8)(ulDestStrLen + 1); /* Length of (octet3 + text string) */
    pstPNNRcd->PNN[2] = ucOctet3InNetworkName;

    return MMA_SUCCESS;

}


VOS_VOID   MMA_ReportPNNParaFromCPHSOperNameStr(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId
)
{
    TAF_PH_USIM_PNN_CNF_STRU            *pstPNN;

    pstPNN   = (TAF_PH_USIM_PNN_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                               sizeof(TAF_PH_USIM_PNN_CNF_STRU));

    if(VOS_NULL_PTR == pstPNN)
    {
        MMA_ReportParaQueryError(ClientId, OpId,TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_PNN_PARA);
        return;
    }

    MMA_MEM_SET(pstPNN, 0, sizeof(TAF_PH_USIM_PNN_CNF_STRU));

    if (MMA_SUCCESS == MMA_CovertCPHSOperNameStrToPNNFormat(&(pstPNN->PNNRecord[0])))
    {
        pstPNN->TotalRecordNum = 1;
        pstPNN->RecordLen      = VOS_StrLen((VOS_CHAR*)pstPNN->PNNRecord[0].PNN);/* 此处不需要 +1 */
        MMA_ParaQueryReport(ClientId, OpId,
                                    TAF_PH_PNN_PARA, TAF_ERR_NO_ERROR, pstPNN);
    }
    else
    {
        MMA_ReportParaQueryError(ClientId, OpId,TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_PNN_PARA);
    }

    PS_MEM_FREE(WUEPS_PID_MMA, pstPNN);
    return;

}
VOS_VOID   MMA_ReadCPHSOperNameStrFileCnf(PS_USIM_GET_FILE_CNF_STRU *pUsimTafMsg)
{
    TAF_PH_SIM_CPHS_OPER_NAME_STRU      *pstONS;

    pstONS = &gstMmaValue.stUsimInfo.stSimCPHSOperName;

    if ((MMA_SUCCESS == pUsimTafMsg->ulResult)
     && (0xff != pUsimTafMsg->aucEf[0]))
    {
        pstONS ->bCPHSOperNameValid  = VOS_TRUE;

        if (pUsimTafMsg->usEfLen > TAF_PH_OPER_NAME_LONG)
        {
            MMA_MEM_CPY(pstONS->aucCPHSOperNameStr, (VOS_UINT8*)(VOS_UINT32)(pUsimTafMsg->aucEf), TAF_PH_OPER_NAME_LONG);
        }
        else
        {
            MMA_MEM_CPY(pstONS->aucCPHSOperNameStr, pUsimTafMsg->aucEf, pUsimTafMsg->usEfLen);
        }

        pstONS->aucCPHSOperNameStr[TAF_PH_OPER_NAME_LONG] = '\0';
    }



}


VOS_VOID MMA_ReadCPHSOperNameStrFileExpired(VOS_VOID)
{
    g_MmaSimTimer[MN_PH_SIM_TIMER_ENUM_CPHS_ONS].ucTimerStatus = MMA_TIMER_STOP;
    MMA_INFOLOG("MMA_ReadCPHSOperNameStrFileExpired");
}

/*****************************************************************************
 函 数 名  : MMA_ReadSpnFileExpired
 功能描述  : 读取spn文件超时处理函数
 输入参数  : VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年9月15日
    作    者   : liuyang
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MMA_ReadSpnFileExpired(VOS_VOID)
{
    MMA_INFOLOG("MMA_ReadSpnFileExpired");
}



VOS_VOID   MMA_ReadSpnFileCnf(TAF_PH_USIM_SPN_CNF_STRU *pMsg,TAF_PH_SPN_TYPE enUpdateType)
{
    TAF_UINT8    uFirstByte;
    TAF_PH_USIM_SPN_STRU            *pstSpnInfo;

    if (TAF_PH_SPN_TYPE_UNKNOW == enUpdateType)
    {
        return;
    }
    uFirstByte = pMsg->aucSpnName[0];
    if (TAF_PH_USIM_SPN == enUpdateType)
    {
        pstSpnInfo = &gstMmaValue.stUsimInfo.UsimSpnInfo;
        gstMmaValue.stUsimInfo.UsimSpnInfo.SpnType = TAF_PH_USIM_SPN;
    }
    else
    {
        pstSpnInfo = &gstMmaValue.stUsimInfo.SimSpnInfo;
        gstMmaValue.stUsimInfo.SimSpnInfo.SpnType = TAF_PH_GSM_SPN;
    }

    /* 目前使用 b1b2 。 */
    pstSpnInfo->DispRplmnMode = pMsg->ucDispRplmnMode & 0x03;

    if ( uFirstByte == 0xff )
    {   /* no SPN available, set SPN invalid. */
        pstSpnInfo->SpnCoding = TAF_PH_RAW_MODE;

        PS_MEM_SET(pstSpnInfo->aucSpnName, (VOS_CHAR)0xFF, TAF_PH_SPN_NAME_MAXLEN);
        pstSpnInfo->aucSpnName[TAF_PH_SPN_NAME_MAXLEN] = '\0';

        return;
    }

    PS_MEM_CPY(pstSpnInfo->aucSpnName,pMsg->aucSpnName,TAF_PH_SPN_NAME_MAXLEN);
    pstSpnInfo->aucSpnName[TAF_PH_SPN_NAME_MAXLEN] = '\0';

   /* 判断是否为UCS2编码，ref: ETSI TS102.221 annex A */
    if ( (uFirstByte == 0x80) || (uFirstByte == 0x81) || (uFirstByte == 0x82) )
    {
        pstSpnInfo->SpnCoding = TAF_PH_RAW_MODE;
    }
    else
    {
        pstSpnInfo->SpnCoding = TAF_PH_GSM_7BIT_DEFAULT;
    }

    return;
}
VOS_UINT32 Taf_PhoneGetOperNameInfo( TAF_PH_OPERATOR_NAME_FORMAT_OP_STRU *pstOperName )
{
    TAF_PH_OPERATOR_NAME_STRU stOperNameTmp;
    VOS_UINT32                ulRet = MMA_FAILURE;
    TAF_UCHAR                 aucSpnName[TAF_PH_OPER_NAME_LONG];
    TAF_PLMN_ID_STRU          nasStylePlmn;

    /*参数有效性判断*/
    if ( VOS_NULL_PTR == pstOperName )
    {
        MMA_WARNINGLOG("Taf_PhoneGetOperNameInfo():WARNING:NULL PTR");
        return ulRet;
    }

    stOperNameTmp = pstOperName->OperName;
    MMA_INFOLOG1("Taf_PhoneGetOperNameInfo():stOperNameTmp.Mcc =",(VOS_INT32)stOperNameTmp.PlmnId.Mcc);
    MMA_INFOLOG1("Taf_PhoneGetOperNameInfo():stOperNameTmp.Mnc =",(VOS_INT32)stOperNameTmp.PlmnId.Mnc);

    PS_MEM_SET(aucSpnName,0x00,TAF_PH_OPER_NAME_LONG);

    /*利用已提供的信息,获取存在NVIM中剩余的运营商姓名*/
    switch ( pstOperName->ListOp )
    {
        /*通过PLMN获得长名字和短名字*/
        case TAF_PH_OPER_NAME_PLMN2ALPHANUMERICNAME :
            stOperNameTmp = pstOperName->OperName;

            /* 从NV项中没有查询到PLMN ID对应的运营商名称时*/
            /* 当前mm/Gmm info值是否非空 */
            if (('\0' != stOperNameTmp.aucOperatorNameLong[0])&&('\0' != stOperNameTmp.aucOperatorNameShort[0]))
            {
                ulRet = MMA_SUCCESS;
                return ulRet;
            }

            Taf_GetSpnName(aucSpnName,TAF_PH_OPER_NAME_LONG);
            /* 是否显示SPN*/
            nasStylePlmn.Mcc  = stOperNameTmp.PlmnId.Mcc;
            nasStylePlmn.Mnc  = stOperNameTmp.PlmnId.Mnc;
            MMA_PlmnId2NasStyle(&nasStylePlmn);

            if((TAF_PH_NO_ROAM == MMA_IsRoam((TAF_SDC_PLMN_ID_STRU *)&nasStylePlmn))
            && (MMA_FALSE == gstMmaValue.ucCopsNotDisplaySpnFlg)
            && ('\0' != aucSpnName[0]))
            {
                if( '\0' == stOperNameTmp.aucOperatorNameLong[0])
                {
                    PS_MEM_CPY(stOperNameTmp.aucOperatorNameLong,aucSpnName,TAF_PH_OPER_NAME_LONG);
                }
                if( '\0' == stOperNameTmp.aucOperatorNameShort[0])
                {
                    PS_MEM_CPY(stOperNameTmp.aucOperatorNameShort,aucSpnName,TAF_PH_OPER_NAME_SHORT);
                }
                ulRet = MMA_SUCCESS;
                pstOperName->OperName = stOperNameTmp;
            }
            else
            {
                /* 从NV项中查询PLMN ID对应的运营商名称*/
                ulRet = TAF_MMA_ReadAlphanumericNameByPlmnIdNvim(&stOperNameTmp);
                if(MMA_FAILURE == ulRet)
                {
                    /* 从运营商列表中查询PLMNID对应的名称 */
                    ulRet = MMA_PhoneFindNtwkNameByPlmnId(&stOperNameTmp);
                }
                if( '\0' == pstOperName->OperName.aucOperatorNameLong[0])
                {
                    PS_MEM_CPY(pstOperName->OperName.aucOperatorNameLong,stOperNameTmp.aucOperatorNameLong,TAF_PH_OPER_NAME_LONG);
                }
                if( '\0' == pstOperName->OperName.aucOperatorNameShort[0])
                {
                    PS_MEM_CPY(pstOperName->OperName.aucOperatorNameShort,stOperNameTmp.aucOperatorNameShort,TAF_PH_OPER_NAME_SHORT);
                }

                if (( '\0' != pstOperName->OperName.aucOperatorNameLong[0])
                 || ( '\0' != pstOperName->OperName.aucOperatorNameShort[0]))
                {
                    ulRet = MMA_SUCCESS;
                }
            }

            break;

        /*通过短名字获取对应的长名字和PLMN*/
        case TAF_PH_OPER_NAME_SHORTNAME2OTHERS :
            ulRet = MMA_PhoneGetOperInfoByShortNameFromOperTbl(&stOperNameTmp);
            if (MMA_SUCCESS == ulRet)
            {
                pstOperName->OperName = stOperNameTmp;
            }
            break;

        /*通过长名字获取对应的短名字和PLMN*/
        case TAF_PH_OPER_NAME_LONGNAME2OTERS :
            ulRet = MMA_PhoneGetOperInfoByLongNameFromOperTbl(&stOperNameTmp);
            if (MMA_SUCCESS == ulRet)
            {
                pstOperName->OperName = stOperNameTmp;
            }
            break;

        default:
            MMA_WARNINGLOG("Taf_PhoneGetOperNameInfo():WARNING:error ListOp");
            break;
    }
    return ulRet;
}



VOS_UINT32 MMA_PhoneFindNtwkNameByPlmnId( TAF_PH_OPERATOR_NAME_STRU   *pstOperName)
{

    VOS_UINT16                          usTotalNtwkNum;
    VOS_UINT16                          i = 0;
    VOS_BOOL                            bFound = VOS_FALSE;
    VOS_BOOL                            bContinueSearch = VOS_TRUE;
    TAF_PH_OPERATOR_NAME_TBL_STRU       *pstNetworkNameTbl = VOS_NULL_PTR;

    TAF_PLMN_ID_STRU tmpPlmnId = pstOperName->PlmnId;

    MMA_INFOLOG1("orig MMA_PhoneFindNtwkNameByPlmnId():tmpPlmnId.Mcc =",(VOS_INT32)tmpPlmnId.Mcc);
    MMA_INFOLOG1("orig MMA_PhoneFindNtwkNameByPlmnId():tmpPlmnId.Mnc =",(VOS_INT32)tmpPlmnId.Mnc);

    tmpPlmnId.Mcc &= 0x0fff;
    tmpPlmnId.Mnc &= 0x0fff;

    if( 0x0F == ((0x0f00 & tmpPlmnId.Mnc) >> 8))
    {
        tmpPlmnId.Mnc &= 0x00ff;
    }
    MMA_INFOLOG1("MMA_PhoneFindNtwkNameByPlmnId():tmpPlmnId.Mcc =",(VOS_INT32)tmpPlmnId.Mcc);
    MMA_INFOLOG1("MMA_PhoneFindNtwkNameByPlmnId():tmpPlmnId.Mnc =",(VOS_INT32)tmpPlmnId.Mnc);

    usTotalNtwkNum      = TAF_MMA_GetNetWorkNameTblSize();
    pstNetworkNameTbl   = TAF_MMA_GetNetworkNameTblAddr();


    /* 如果当前NV配置不显示本地的全球运行商列表信息,则直接返回 */
    if ( MMA_TRUE == TAF_MMA_GetNotDisplayLocalNetworkNameFlg() )
    {
        pstOperName->aucOperatorNameLong[0]  = '\0';
        pstOperName->aucOperatorNameShort[0] = '\0';

        return MMA_FAILURE;
    }

    while(( VOS_TRUE == bContinueSearch )&&(i < usTotalNtwkNum))
    {
        if(tmpPlmnId.Mcc == pstNetworkNameTbl->PlmnId.Mcc)
        {
            if(tmpPlmnId.Mnc == pstNetworkNameTbl->PlmnId.Mnc)
            {
                bFound = VOS_TRUE;
                bContinueSearch = VOS_FALSE;

                VOS_StrNCpy(pstOperName->aucOperatorNameLong,
                        pstNetworkNameTbl->pStrOperatorNameLong,TAF_PH_OPER_NAME_LONG);
                VOS_StrNCpy(pstOperName->aucOperatorNameShort,
                        pstNetworkNameTbl->pStrOperatorNameShort,TAF_PH_OPER_NAME_SHORT);
            }
            else if (tmpPlmnId.Mnc < pstNetworkNameTbl->PlmnId.Mnc)
            {
                bContinueSearch = VOS_FALSE;
            }
            else
            {
            }
        }
        else if(tmpPlmnId.Mcc < pstNetworkNameTbl->PlmnId.Mcc)
        {
            bContinueSearch = VOS_FALSE;
        }
        else
        {
        }

        pstNetworkNameTbl ++;

        ++ i;

    }

    if(VOS_FALSE == bFound)
    {
        pstOperName->aucOperatorNameLong[0]  = '\0';
        pstOperName->aucOperatorNameShort[0] = '\0';
        return MMA_FAILURE;
    }

    return MMA_SUCCESS;
}

/* 获取长短名的NV未使用，相应的函数删除 */

VOS_BOOL NAS_MMA_PlmnInCurUserSrchList(TAF_PLMN_ID_STRU *pstPlmnId)
{
    VOS_UINT32                          i;
    TAF_PLMN_ID_STRU                    stPlmnId;

    for (i = 0; i < g_StatusContext.PlmnList.ucPlmnNum; i++)
    {
        stPlmnId = g_StatusContext.PlmnList.Plmn[i];
        MMA_PlmnId2Bcd(&stPlmnId);

        stPlmnId.Mcc &= 0x0fff;
        stPlmnId.Mnc &= 0x0fff;

        if (0x0F == ((0x0f00 & stPlmnId.Mnc) >> 8))
        {
            stPlmnId.Mnc &= 0x00ff;
        }

        if ((stPlmnId.Mcc == pstPlmnId->Mcc)
         && (stPlmnId.Mnc == pstPlmnId->Mnc))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 MMA_PhoneGetOperInfoByLongNameFromOperTbl( TAF_PH_OPERATOR_NAME_STRU   *pstOperName)
{
    VOS_UINT16                          usTotalNtwkNum;
    VOS_UINT16                          i;
    VOS_BOOL                            bFlag;
    TAF_PH_OPERATOR_NAME_STRU           stDestPhOperName;
    TAF_PH_OPERATOR_NAME_TBL_STRU      *pstNetworkNameTbl = VOS_NULL_PTR;

    bFlag = VOS_FALSE;
    usTotalNtwkNum      = TAF_MMA_GetNetWorkNameTblSize();
    pstNetworkNameTbl   = TAF_MMA_GetNetworkNameTblAddr();

    PS_MEM_SET(&stDestPhOperName, 0, sizeof(stDestPhOperName));


    /* 如果当前NV配置不显示本地的全球运行商列表信息,则直接返回 */
    if ( MMA_TRUE == TAF_MMA_GetNotDisplayLocalNetworkNameFlg() )
    {
        pstOperName->aucOperatorNameShort[0]    = '\0';
        pstOperName->PlmnId.Mcc                 = 0;
        pstOperName->PlmnId.Mnc                 = 0;

        return MMA_FAILURE;
    }

    for (i = 0; i < usTotalNtwkNum; i++ )
    {
        if ( !VOS_StrNiCmp( (VOS_CHAR *)(pstOperName->aucOperatorNameLong),
                          (VOS_CHAR *)(pstNetworkNameTbl->pStrOperatorNameLong),
                          TAF_PH_OPER_NAME_LONG) )
        {
            /* the current long operator name is equal to the specified one
             * copy PLMN ID & short operator name into output parameter and return back
             */
            pstOperName->PlmnId.Mcc = pstNetworkNameTbl->PlmnId.Mcc;
            pstOperName->PlmnId.Mnc = pstNetworkNameTbl->PlmnId.Mnc;

            VOS_StrNCpy(pstOperName->aucOperatorNameShort,
                    pstNetworkNameTbl->pStrOperatorNameShort,TAF_PH_OPER_NAME_SHORT);
            MMA_NORMAILLOG("the specified PLMN found in gastNetworkNameTbl!");


            if (VOS_FALSE == bFlag)
            {
                stDestPhOperName = *pstOperName;
                bFlag            = VOS_TRUE;
            }

            if (VOS_TRUE == NAS_MMA_PlmnInCurUserSrchList(&pstOperName->PlmnId))
            {
                stDestPhOperName = *pstOperName;
                break;
            }

        }

        pstNetworkNameTbl ++;
    }

    if (VOS_FALSE != bFlag)
    {
        VOS_StrNCpy(pstOperName->aucOperatorNameShort,
                stDestPhOperName.aucOperatorNameShort,TAF_PH_OPER_NAME_SHORT);
        pstOperName->PlmnId = stDestPhOperName.PlmnId;

        if ( pstOperName->PlmnId.Mnc < 0x100)
        {
            pstOperName->PlmnId.Mnc <<= 4;
            pstOperName->PlmnId.Mnc |= 0xF;
        }

        return MMA_SUCCESS;
    }
    else
    {
        MMA_NORMAILLOG("the specified PLMN not found in gastNetworkNameTbl!");
        pstOperName->aucOperatorNameShort[0]    = '\0';
        pstOperName->PlmnId.Mcc                 = 0;
        pstOperName->PlmnId.Mnc                 = 0;
        return MMA_FAILURE;
    }

}
VOS_UINT32 MMA_PhoneGetOperInfoByShortNameFromOperTbl( TAF_PH_OPERATOR_NAME_STRU   *pstOperName)
{
    VOS_UINT16                          usTotalNtwkNum;
    VOS_UINT16                          i ;
    VOS_BOOL                            bFlag;
    TAF_PH_OPERATOR_NAME_STRU           stDestPhOperName;
    TAF_PH_OPERATOR_NAME_TBL_STRU      *pstNetworkNameTbl = VOS_NULL_PTR;

    bFlag = VOS_FALSE;

    usTotalNtwkNum      = TAF_MMA_GetNetWorkNameTblSize();
    pstNetworkNameTbl   = TAF_MMA_GetNetworkNameTblAddr();

    PS_MEM_SET(&stDestPhOperName, 0, sizeof(stDestPhOperName));



    /* 如果当前NV配置不显示本地的全球运行商列表信息,则直接返回 */
    if ( MMA_TRUE == TAF_MMA_GetNotDisplayLocalNetworkNameFlg() )
    {
        pstOperName->aucOperatorNameLong[0]    = '\0';
        pstOperName->PlmnId.Mcc                 = 0;
        pstOperName->PlmnId.Mnc                 = 0;

        return MMA_FAILURE;
    }

    for (i = 0; i < usTotalNtwkNum; i++ )
    {
        if ( !VOS_StrNiCmp( (VOS_CHAR *)(pstOperName->aucOperatorNameShort),
                          (VOS_CHAR *)(pstNetworkNameTbl->pStrOperatorNameShort),
                          TAF_PH_OPER_NAME_SHORT) )
        {
            /* the current long operator name is equal to the specified one
             * copy PLMN ID & short operator name into output parameter and return back
             */
            pstOperName->PlmnId.Mcc = pstNetworkNameTbl->PlmnId.Mcc;
            pstOperName->PlmnId.Mnc = pstNetworkNameTbl->PlmnId.Mnc;

            VOS_StrNCpy(pstOperName->aucOperatorNameLong,
                    pstNetworkNameTbl->pStrOperatorNameLong,TAF_PH_OPER_NAME_LONG);
            MMA_NORMAILLOG("the specified PLMN found in gastNetworkNameTbl!");


            if (VOS_FALSE == bFlag)
            {
                stDestPhOperName = *pstOperName;
                bFlag            = VOS_TRUE;
            }

            if (VOS_TRUE == NAS_MMA_PlmnInCurUserSrchList(&pstOperName->PlmnId))
            {
                stDestPhOperName = *pstOperName;
                break;
            }
        }

        pstNetworkNameTbl ++;

    }

    if (VOS_FALSE != bFlag)
    {

        VOS_StrNCpy(pstOperName->aucOperatorNameLong,
                stDestPhOperName.aucOperatorNameLong,TAF_PH_OPER_NAME_LONG);

        pstOperName->PlmnId              = stDestPhOperName.PlmnId;

        if ( pstOperName->PlmnId.Mnc < 0x100)
        {
            pstOperName->PlmnId.Mnc <<= 4;
            pstOperName->PlmnId.Mnc |= 0xF;
        }

        return MMA_SUCCESS;
    }
    else
    {
        MMA_NORMAILLOG("the specified PLMN not found in gastNetworkNameTbl!");
        pstOperName->aucOperatorNameLong[0]    = '\0';
        pstOperName->PlmnId.Mcc                 = 0;
        pstOperName->PlmnId.Mnc                 = 0;
        return MMA_FAILURE;
    }
}
VOS_UINT32 MMA_PhoneGetAllOperNumByLongNameFromOperTbl( TAF_PH_OPERATOR_NAME_STRU   *pstOperName,
                                                                                 VOS_UINT8 *pucMaxMncLen)
{
    VOS_UINT16                          usTotalNtwkNum;
    VOS_UINT32                          ulTotalOperNum = 0;
    VOS_UINT16                          i = 0;
    VOS_UINT8                           ucCurMncLen;
    TAF_PH_OPERATOR_NAME_TBL_STRU      *pstNetworkNameTbl = VOS_NULL_PTR;

    usTotalNtwkNum      = TAF_MMA_GetNetWorkNameTblSize();
    pstNetworkNameTbl   = TAF_MMA_GetNetworkNameTblAddr();

    *pucMaxMncLen = 0;


    /* 如果当前NV配置不显示本地的全球运行商列表信息,则直接返回 */
    if ( MMA_TRUE == TAF_MMA_GetNotDisplayLocalNetworkNameFlg() )
    {
        return ulTotalOperNum;
    }

    for (i = 0; i < usTotalNtwkNum; i++ )
    {
        if ( !VOS_StrNiCmp( (VOS_CHAR *)(pstOperName->aucOperatorNameLong),
                          (VOS_CHAR *)(pstNetworkNameTbl->pStrOperatorNameLong),
                          TAF_PH_OPER_NAME_LONG) )
        {
            /* 累加OPER个数 */
            ulTotalOperNum++;

            /* the current long operator name is equal to the specified one
             * copy PLMN ID & short operator name into output parameter and return back
             */
            if ( pstNetworkNameTbl->PlmnId.Mnc < 0x100)
            {
                ucCurMncLen = 5;
            }
            else
            {
                ucCurMncLen = 6;
            }

            if (*pucMaxMncLen < ucCurMncLen)
            {
                *pucMaxMncLen = ucCurMncLen;
            }

        }

        pstNetworkNameTbl ++;
    }

    return ulTotalOperNum;

}
VOS_UINT32 MMA_PhoneGetAllOperNumByShortNameFromOperTbl( TAF_PH_OPERATOR_NAME_STRU   *pstOperName,
                                                                                 VOS_UINT8 *pucMaxMncLen)
{
    VOS_UINT16                          usTotalNtwkNum;
    VOS_UINT32                          ulTotalOperNum = 0;
    VOS_UINT16                          i = 0;
    VOS_UINT8                           ucCurMncLen;
    TAF_PH_OPERATOR_NAME_TBL_STRU      *pstNetworkNameTbl = VOS_NULL_PTR;

    usTotalNtwkNum      = TAF_MMA_GetNetWorkNameTblSize();
    pstNetworkNameTbl   = TAF_MMA_GetNetworkNameTblAddr();

    *pucMaxMncLen = 0;


    /* 如果当前NV配置不显示本地的全球运行商列表信息,则直接返回 */
    if ( MMA_TRUE == TAF_MMA_GetNotDisplayLocalNetworkNameFlg() )
    {
        return ulTotalOperNum;
    }

    for (i = 0; i < usTotalNtwkNum; i++ )
    {
        if ( !VOS_StrNiCmp( (VOS_CHAR *)(pstOperName->aucOperatorNameShort),
                          (VOS_CHAR *)(pstNetworkNameTbl->pStrOperatorNameShort),
                          TAF_PH_OPER_NAME_SHORT) )
        {
            /* 累加OPER个数 */
            ulTotalOperNum++;

            /* the current long operator name is equal to the specified one
             * copy PLMN ID & short operator name into output parameter and return back
             */
            if ( pstNetworkNameTbl->PlmnId.Mnc < 0x100)
            {
                ucCurMncLen = 5;
            }
            else
            {
                ucCurMncLen = 6;
            }

            if (*pucMaxMncLen < ucCurMncLen)
            {
                *pucMaxMncLen = ucCurMncLen;
            }

        }

        pstNetworkNameTbl ++;
    }

    return ulTotalOperNum;

}
VOS_UINT32 MMA_PhoneGetAllOperInfoByLongNameFromOperTbl( TAF_PH_OPERATOR_NAME_STRU   *pstOrigOperName,
                                                                                TAF_PH_OPERATOR_NAME_STRU   *pstOperName)
{
    VOS_UINT16                          usTotalNtwkNum;
    VOS_UINT16                          i = 0;
    TAF_PH_OPERATOR_NAME_STRU          *pstOperResult = pstOperName;
    TAF_PH_OPERATOR_NAME_TBL_STRU      *pstNetworkNameTbl = VOS_NULL_PTR;

    usTotalNtwkNum      = TAF_MMA_GetNetWorkNameTblSize();
    pstNetworkNameTbl   = TAF_MMA_GetNetworkNameTblAddr();


    /* 如果当前NV配置不显示本地的全球运行商列表信息,则直接返回 */
    if ( MMA_TRUE == TAF_MMA_GetNotDisplayLocalNetworkNameFlg() )
    {
        return MMA_SUCCESS;
    }

    for (i = 0; i < usTotalNtwkNum; i++ )
    {
        if ( !VOS_StrNiCmp( (VOS_CHAR *)(pstOrigOperName->aucOperatorNameLong),
                          (VOS_CHAR *)(pstNetworkNameTbl->pStrOperatorNameLong),
                          TAF_PH_OPER_NAME_LONG) )
        {
            /* the current long operator name is equal to the specified one
             * copy PLMN ID & short operator name into output parameter and return back
             */
            pstOperResult->PlmnId.Mcc = pstNetworkNameTbl->PlmnId.Mcc;
            pstOperResult->PlmnId.Mnc = pstNetworkNameTbl->PlmnId.Mnc;

            VOS_StrNCpy(pstOperResult->aucOperatorNameShort,
                        pstNetworkNameTbl->pStrOperatorNameShort,
                        TAF_PH_OPER_NAME_SHORT);

            VOS_StrNCpy(pstOperResult->aucOperatorNameLong,
                        pstNetworkNameTbl->pStrOperatorNameLong,
                        TAF_PH_OPER_NAME_LONG);

            pstOperResult++;
        }

        pstNetworkNameTbl ++;
    }

    return MMA_SUCCESS;
}
VOS_UINT32 MMA_PhoneGetAllOperInfoByShortNameFromOperTbl( TAF_PH_OPERATOR_NAME_STRU   *pstOrigOperName,
                                                                                TAF_PH_OPERATOR_NAME_STRU   *pstOperName)
{
    VOS_UINT16                          usTotalNtwkNum;
    VOS_UINT16                          i = 0;
    TAF_PH_OPERATOR_NAME_STRU          *pstOperResult = pstOperName;
    TAF_PH_OPERATOR_NAME_TBL_STRU      *pstNetworkNameTbl = VOS_NULL_PTR;

    usTotalNtwkNum      = TAF_MMA_GetNetWorkNameTblSize();
    pstNetworkNameTbl   = TAF_MMA_GetNetworkNameTblAddr();


    /* 如果当前NV配置不显示本地的全球运行商列表信息,则直接返回 */
    if ( MMA_TRUE == TAF_MMA_GetNotDisplayLocalNetworkNameFlg() )
    {
        return MMA_SUCCESS;
    }

    for (i = 0; i < usTotalNtwkNum; i++ )
    {
        if ( !VOS_StrNiCmp( (VOS_CHAR *)(pstOrigOperName->aucOperatorNameShort),
                          (VOS_CHAR *)(pstNetworkNameTbl->pStrOperatorNameShort),
                          TAF_PH_OPER_NAME_SHORT) )
        {
            /* the current short operator name is equal to the specified one
             * copy PLMN ID & long operator name into output parameter and return back
             */
            pstOperResult->PlmnId.Mcc = pstNetworkNameTbl->PlmnId.Mcc;
            pstOperResult->PlmnId.Mnc = pstNetworkNameTbl->PlmnId.Mnc;

            VOS_StrNCpy(pstOperResult->aucOperatorNameShort,
                        pstNetworkNameTbl->pStrOperatorNameShort,
                        TAF_PH_OPER_NAME_SHORT);

            VOS_StrNCpy(pstOperResult->aucOperatorNameLong,
                        pstNetworkNameTbl->pStrOperatorNameLong,
                        TAF_PH_OPER_NAME_LONG);

            pstOperResult++;
        }

        pstNetworkNameTbl ++;

    }

    return MMA_SUCCESS;
}




VOS_UINT32 MMA_InitAtMsgReq( VOS_UINT16   ClientId,
                             VOS_UINT8          OpId,
                             TAF_START_INFO_IND_STRU   stAtMsgReq)
{
    VOS_UINT32                   ulRet = MMA_ERROR;
    TAF_PHONE_EVENT_INFO_STRU   *pstPhoneEvent;
    VOS_UINT32                   ulRptCmdStatus;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    /* 更新CERSSI对应的SDC全局变量 */
    if (0 == stAtMsgReq.ucSignThreshold)
    {
        ulRptCmdStatus = VOS_FALSE;
    }
    else
    {
        ulRptCmdStatus = VOS_TRUE;
    }

    TAF_SDC_UpdateRptCmdStatus(TAF_SDC_RPT_CMD_CERSSI, ulRptCmdStatus);

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MMA_InitAtMsgReq:ERROR:ALLOC MEMORY FAIL.");
        return ulRet;
    }

    if ( (stAtMsgReq.ucActionType > TAF_START_EVENT_INFO_STOP)
       ||(stAtMsgReq.ucRrcMsgType > ( TAF_EVENT_INFO_CELL_BLER
                                     |TAF_EVENT_INFO_CELL_ID
                                     |TAF_EVENT_INFO_CELL_SIGN
                                    )
         )
       )
    {
        MMA_HandleEventError(ClientId, OpId, TAF_ERR_PARA_ERROR, TAF_PH_EVT_SETUP_SYSTEM_INFO_RSP);
        MMA_WARNINGLOG("MMA_InitAtMsgReq():WARNING:Error Para");
        PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
        return MMA_ERROR;
    }
    else
    {
        ulRet = TAF_MMA_SndMmcSignalReportReq(stAtMsgReq.ucActionType, stAtMsgReq.ucRrcMsgType,
                          stAtMsgReq.ucSignThreshold, stAtMsgReq.ucMinRptTimerInterval);

        /* 通知L模^cerssi设置 */
#if ( FEATURE_ON == FEATURE_LTE )
        TAF_MMA_SendL4aCerssiInfoReq(ClientId, OpId, stAtMsgReq.ucSignThreshold,stAtMsgReq.ucMinRptTimerInterval);
#endif
    }

    /*上报设置结果*/
    if ( MMA_SUCCESS == ulRet)
    {
        pstPhoneEvent->ClientId              = ClientId;
        pstPhoneEvent->OpId                  = OpId;
        pstPhoneEvent->OP_PhoneError         = TAF_PARA_OK;
        pstPhoneEvent->PhoneEvent            = TAF_PH_EVT_SETUP_SYSTEM_INFO_RSP;
        Taf_InitEventInfoOP(pstPhoneEvent);

        /* 调用电话管理上报函数 */
        MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));
    }
    else
    {
        MMA_HandleEventError(ClientId, OpId, TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_EVT_SETUP_SYSTEM_INFO_RSP);
    }
    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);


    return ulRet;
}

/*****************************************************************************
 函 数 名  : MMA_CleanTiVar
 功能描述  : 清除gastMmaTiTab全局变量对应的值，默认值为0，该函数没有返回值，
             操作成功的有效性由调用者保证
 输入参数  : VOS_UINT16            ClientId
             VOS_UINT8                   OpId
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年12月8日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

  2.日    期   : 2006年3月8日
    作    者   : liuyang id:48197
    修改内容   : 问题单号：A32D02371，使用宏替换原有的数字
*****************************************************************************/
VOS_VOID MMA_CleanTiVar(VOS_UINT16            ClientId,
                        VOS_UINT8                   OpId     )
{
    VOS_UINT32 ulI = 0;

    for ( ulI = 0; ulI < MMA_MAX_TI ; ulI++ )
    {
        if ( (TAF_USED == gastMmaTiTab[ulI].ucUsed  )
           &&(ClientId == gastMmaTiTab[ulI].ClientId)
           &&(OpId     == gastMmaTiTab[ulI].Id      )
           )
        {
            /*清除当前使用标志*/
            gastMmaTiTab[ulI].ucUsed    = MMA_TI_UNUSED;
            gastMmaTiTab[ulI].ClientId  = MMA_CLIENTID_NONE;
            gastMmaTiTab[ulI].Id        = MMA_OP_ID_INTERNAL;
            break;
        }
    }
    return;
}


/*****************************************************************************
 函 数 名  : MMA_BcdNumToAsciiNum
 功能描述  : BCD码转位ASCII码，代码来自SSA
 输入参数  : VOS_UINT8 *pucAsciiNum
             VOS_UINT8 *pucBcdNum
             VOS_UINT8 ucBcdNumLen
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年12月9日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MMA_BcdNumToAsciiNum(VOS_UINT8 *pucAsciiNum, VOS_UINT8 *pucBcdNum, VOS_UINT8 ucBcdNumLen)
{
    VOS_UINT8       ucTmp;
    VOS_UINT8       ucFirstNumber;
    VOS_UINT8       ucSecondNumber;

    for (ucTmp = 0; ucTmp < ucBcdNumLen; ucTmp++)
    {
        ucFirstNumber = (VOS_UINT8)((*(pucBcdNum + ucTmp)) & 0x0f);             /*low four bits*/
        ucSecondNumber = (VOS_UINT8)(((*(pucBcdNum + ucTmp)) >> 4) & 0x0f);   /*high four bits*/
        if(ucFirstNumber <= 9)
        {
            *pucAsciiNum = ucFirstNumber + 0x30;
            pucAsciiNum++;
        }
        else if(0xa == ucFirstNumber)            /* the key is '*' */
        {
            *pucAsciiNum = 0x2a;
            pucAsciiNum++;
        }
        else if(0xb == ucFirstNumber)            /* the key is '#' */
        {
            *pucAsciiNum = 0x23;
            pucAsciiNum++;
        }
        else                                     /* the key is 'a','b'or 'c' */
        {
            *pucAsciiNum = ucFirstNumber + 0x57;
            pucAsciiNum++;
        }

        /* the following proceed the second number */
        if(0xf == ucSecondNumber)
        {
            break;
        }
        if(ucSecondNumber <= 9)
        {
            *pucAsciiNum = ucSecondNumber + 0x30;
            pucAsciiNum++;
        }
        else if(0xa == ucSecondNumber)            /* the key is '*' */
        {
            *pucAsciiNum = 0x2a;
            pucAsciiNum++;
        }
        else if(0xb == ucSecondNumber)            /* the key is '#' */
        {
            *pucAsciiNum = 0x23;
            pucAsciiNum++;
        }
        else                                     /* the key is 'a','b'or 'c' */
        {
            *pucAsciiNum = ucSecondNumber + 0x57;
            pucAsciiNum++;
        }
    }
    *pucAsciiNum = '\0';
    return;
}
VOS_UINT32 MMA_ImsiBcd2Ascii(VOS_UINT8 ucLen, VOS_UINT8 *pucNum, VOS_UINT8 *pucAsciiStr)
{
    VOS_UINT32 ulI = 0;
    VOS_UINT8   ucTmp;
    /*有效性判断*/
    if ( (VOS_NULL_PTR == pucNum)
       ||(VOS_NULL_PTR == pucAsciiStr)
       ||(           0 == ucLen )
       )
    {
        MMA_WARNINGLOG("MMA_ImsiBcd2Ascii():WARNING:NULL PTR");
        return MMA_FAILURE;
    }

    /*国际移动用户识别码*/
    /*清空存放字符串的缓冲区*/
    for ( ulI = 0; ulI < (VOS_UINT32)ucLen; ulI++ )
    {
        pucAsciiStr[ulI] = 0;
    }
    ucTmp = (*pucNum >> 4) & 0x0f;
    /*取出第一位*/
    MMA_BcdNumToAsciiNum(pucAsciiStr, &ucTmp, 1);
    /*进行Bcd到Ascii的转换*/
    MMA_BcdNumToAsciiNum(pucAsciiStr + 1, pucNum + 1, (ucLen - 1)/2);
    return MMA_SUCCESS;
}

/*****************************************************************************
 函 数 名  : MMA_ImeiBcd2Ascii
 功能描述  : 转化IMEI编码为字符串形式
 输入参数  : VOS_UINT8 ucLen   --BCD码长度
             VOS_UINT8 *pucNum --BCD码存放的位置
             VOS_UINT8 *pucAsciiStr  --字符串写入位置
 输出参数  : 无
 返 回 值  : VOS_UINT32 --传入为空指针或长度为0时，返回错误
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年12月9日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 MMA_ImeiBcd2Ascii(VOS_UINT8 ucLen, VOS_UINT8 *pucNum, VOS_UINT8 *pucAsciiStr)
{
    VOS_UINT32 ulI = 0;
    if ( (VOS_NULL_PTR == pucNum)
       ||(VOS_NULL_PTR == pucAsciiStr)
       ||(           0 == ucLen )
       )
    {
        MMA_WARNINGLOG("MMA_ImsiBcd2Ascii():WARNING:NULL PTR");
        return MMA_FAILURE;
    }
    /*国际移动设备识别码*/
    for ( ulI = 0; ulI < ucLen ; ulI++ )
    {
        *(pucAsciiStr + ulI) = *(pucNum + ulI) + 0x30;
    }
    pucAsciiStr[ulI] = '\0';
    return MMA_SUCCESS;
}
VOS_UINT32 MMA_ImeiAsciiToBcd(VOS_UINT8 *pucAsciiNum, VOS_UINT8 ucAsciiNumLen, VOS_UINT8 *pucBcdNum)
{
    VOS_UINT32 ulI = 0;
    if ( (VOS_NULL_PTR == pucAsciiNum)
       ||(VOS_NULL_PTR == pucBcdNum)
       ||(           0 == ucAsciiNumLen )
       )
    {
        MMA_WARNINGLOG("MMA_ImeiAsciiToBcd():WARNING:NULL PTR");
        return MMA_FAILURE;
    }
    /*国际移动设备识别码*/
    for ( ulI = 0; ulI < ucAsciiNumLen ; ulI++ )
    {
        *(pucBcdNum + ulI) = *(pucAsciiNum + ulI) - 0x30;
    }

    return MMA_SUCCESS;
}
VOS_UINT32 Taf_GetCurrentAttachStatus( TAF_PH_CS_STATUS    *pCsResult,
                                       TAF_PH_PS_STATUS    *pPsResult )
{
    extern STATUS_CONTEXT_STRU g_StatusContext;

    /*只有当当前服务状态为正常服务或限制服务状态才被认为成功附着，其余状态都
      为未附着*/
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8    enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8    enPsServiceStatus;

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    if  ( TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enCsServiceStatus)
    {
        *pCsResult = TAF_PH_ATTACHED;
    }
    else
    {
        *pCsResult = TAF_PH_DETACHED;
    }
    if  (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == enPsServiceStatus)
    {
        *pPsResult = TAF_PH_ATTACHED;
    }
    else
    {
        *pPsResult = TAF_PH_DETACHED;
    }


    return TAF_SUCCESS;
}



/*****************************************************************************
 函 数 名  : MMA_PlmnId2Bcd
 功能描述  : 将原PLMNid转化为BCD格式记录
             例如:MCC为460,MNC为01时,
             pstPlmnId->Mcc = 0x000604      pstPlmnId->Mcc = 0x00000460
                                        ---->
             pstPlmnId->Mnc = 0x000100      pstPlmnId->Mnc = 0x00000001
 输入参数  : TAF_PLMN_ID_STRU *pstPlmnId
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年12月20日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

  2.日    期   : 2006年2月23日
    作    者   : liuyang id:48197
    修改内容   : 根据与Wcdma的接口修改传入的PLMN顺序,问题单号:A32D02119
*****************************************************************************/
VOS_VOID MMA_PlmnId2Bcd(TAF_PLMN_ID_STRU *pstPlmnId)
{
    VOS_UINT8   ucI;
    VOS_UINT32  ulTmp;
    VOS_UINT8   aucTmp[4];

    ulTmp           = pstPlmnId->Mcc;
    pstPlmnId->Mcc  = 0;
    for ( ucI = 0 ; ucI < 3 ; ucI++ )
    {
        aucTmp[ucI]     = ulTmp & 0x0f;
        ulTmp         >>=  8;
    }
    pstPlmnId->Mcc = ((VOS_UINT32)aucTmp[0] << 8)
                    |((VOS_UINT32)aucTmp[1] << 4)
                    | aucTmp[2];

    ulTmp           = pstPlmnId->Mnc;
    pstPlmnId->Mnc  = 0;
    for ( ucI = 0 ; ucI < 3 ; ucI++ )
    {
        aucTmp[ucI]     = ulTmp & 0x0f;
        ulTmp         >>=  8;
    }

    /* BEGIN: Modified by liuyang id:48197, 2006/2/23   PN:A32D02119*/
    /*pstPlmnId->Mnc = ((VOS_UINT32)aucTmp[0] << 8)
                    |((VOS_UINT32)aucTmp[1] << 4)
                    | aucTmp[2];*/
    if(0xf == aucTmp[2])
    {
        pstPlmnId->Mnc = ((VOS_UINT32)aucTmp[0] << 4)
                        |((VOS_UINT32)aucTmp[1] )
                        |0xf00;
    }
    else
    {
        pstPlmnId->Mnc = ((VOS_UINT32)aucTmp[0] << 8)
                    |((VOS_UINT32)aucTmp[1] << 4)
                    | aucTmp[2];
    }
    /* END:   Modified by liuyang id:48197, 2006/2/23 */
    return;
}


/*****************************************************************************
 函 数 名  : MMA_PlmnId2NasStyle
 功能描述  : 将原PLMNid转化为NAS内部使用格式记录
             例如:MCC为460,MNC为01时,
             pstPlmnId->Mcc = 0x000604      pstPlmnId->Mcc = 0x00000460
                                        <----
             pstPlmnId->Mnc = 0x000100      pstPlmnId->Mnc = 0x00000001
 输入参数  : TAF_PLMN_ID_STRU *pstPlmnId
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2005年12月20日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

  2.日    期   : 2006年2月23日
    作    者   : liuyang id:48197
    修改内容   : 根据与Wcdma的接口修改传入的PLMN顺序,问题单号:A32D02119

*****************************************************************************/
VOS_VOID MMA_PlmnId2NasStyle(TAF_PLMN_ID_STRU *pstPlmnId)
{
    VOS_UINT8   ucI;
    VOS_UINT8   aucTmp[4];
    VOS_UINT32  ulTmp;

    ulTmp           = pstPlmnId->Mcc;
    pstPlmnId->Mcc  = 0;
    for ( ucI = 0 ; ucI < 3 ; ucI++ )
    {
        aucTmp[ucI]     = ulTmp & 0x0f;
        ulTmp         >>=  4;
    }
    pstPlmnId->Mcc  =  aucTmp[2]
                     |((VOS_UINT32)aucTmp[1] << 8)
                     |((VOS_UINT32)aucTmp[0] << 16);

    ulTmp           = pstPlmnId->Mnc;
    pstPlmnId->Mnc  = 0;
    for ( ucI = 0 ; ucI < 3 ; ucI++ )
    {
        aucTmp[ucI]     = ulTmp & 0x0f;
        ulTmp         >>=  4;
    }
    /* BEGIN: Modified by liuyang id:48197, 2006/2/23   PN:A32D02119*/
    /*pstPlmnId->Mnc =   aucTmp[2]
                     |((VOS_UINT32)aucTmp[1] << 8)
                     |((VOS_UINT32)aucTmp[0] << 16);*/
    if(0xf == aucTmp[2])
    {
        pstPlmnId->Mnc =  aucTmp[1]
                         |((VOS_UINT32)aucTmp[0] << 8)
                         |0x0f0000;
    }
    else
    {
        pstPlmnId->Mnc =   aucTmp[2]
                     |((VOS_UINT32)aucTmp[1] << 8)
                     |((VOS_UINT32)aucTmp[0] << 16);
    }
    /* END:   Modified by liuyang id:48197, 2006/2/23 */
    return;
}
VOS_VOID MMA_ChangeUsimStatus(VOS_UINT32  ulUsimSta)
{


    switch ( ulUsimSta )
    {
        case USIMM_CARD_SERVIC_ABSENT :
             TAF_SDC_SetSimStatus(TAF_SDC_USIM_STATUS_NO_PRESENT);

             return;

        case USIMM_CARD_SERVIC_UNAVAILABLE :
             TAF_SDC_SetSimStatus(TAF_SDC_USIM_STATUS_UNVALID);
             return;

        case USIMM_CARD_SERVIC_SIM_PIN :
             TAF_SDC_SetSimStatus(TAF_SDC_USIM_STATUS_SIM_PIN);
             return;

        case USIMM_CARD_SERVIC_SIM_PUK :
             TAF_SDC_SetSimStatus(TAF_SDC_USIM_STATUS_SIM_PUK);
             return;

        case USIMM_CARD_SERVIC_NET_LCOK :
             TAF_SDC_SetSimStatus(TAF_SDC_USIM_STATUS_NET_LCOK);
             return;

        case USIMM_CARD_SERVIC_IMSI_LCOK :
             TAF_SDC_SetSimStatus(TAF_SDC_USIM_STATUS_IMSI_LCOK);
             return;

        case USIMM_CARD_SERVIC_AVAILABLE :
             TAF_SDC_SetSimStatus(TAF_SDC_USIM_STATUS_VALID);
             return;

        /* 异常处理 */
        default :
            NAS_WARNING_LOG(WUEPS_PID_MMA, "MMA_ChangeUsimStatus: USIM STATUS INVALID");

            /* 其他情况下认为卡状态有效 */
            TAF_SDC_SetSimStatus(TAF_SDC_USIM_STATUS_VALID);
            return ;
    }


}


VOS_UINT32 TAF_MMA_IsUsimStatusChanged(
    VOS_UINT32                          ulUsimSta
)
{
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enSimStatus;

    VOS_UINT32                          ulUsimChangeFlg;

    enSimStatus     = TAF_SDC_GetSimStatus();

    /* 默认USIM状态没有改变 */
    ulUsimChangeFlg = VOS_FALSE;

    switch ( ulUsimSta )
    {
        case USIMM_CARD_SERVIC_ABSENT :
        case USIMM_CARD_SERVIC_UNAVAILABLE :
            ulUsimChangeFlg = TAF_MMA_IsUsimStatusChange_UsimmCardServiceAbsent(enSimStatus);
            break;

        case USIMM_CARD_SERVIC_AVAILABLE :
            ulUsimChangeFlg = TAF_MMA_IsUsimStatusChange_UsimmCardServiceAvailable(enSimStatus);
            break;

        case USIMM_CARD_SERVIC_SIM_PIN :
        case USIMM_CARD_SERVIC_SIM_PUK :
        case USIMM_CARD_SERVIC_NET_LCOK :
        case USIMM_CARD_SERVIC_IMSI_LCOK :
            ulUsimChangeFlg = TAF_MMA_IsUsimStatusChange_UsimmCardServiceSimPin(enSimStatus);
            break;

        default:
            NAS_WARNING_LOG(WUEPS_PID_MMA, "MMA_IsUsimStatusChange:WARNING INVALID SIM STATUS!");
            break;
    }


    return ulUsimChangeFlg;
}


VOS_UINT32 TAF_MMA_IsImsiChanged(
    PS_USIM_STATUS_IND_STRU            *pstUsimMsg
)
{
    VOS_UINT8                          *pucSdcImsi = VOS_NULL_PTR;

    if (USIMM_CARD_SERVIC_AVAILABLE != pstUsimMsg->enCardStatus)
    {
        return VOS_FALSE;
    }

    pucSdcImsi = TAF_SDC_GetLastSimImsi();
	
    if (0 != VOS_MemCmp(pucSdcImsi, pstUsimMsg->aucIMSI, pstUsimMsg->ucIMSILen))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_VOID TAF_MMA_ChangeUsimType(VOS_UINT32  ulCardType)
{
    VOS_UINT8                           ucSimType;

    switch (ulCardType)
    {
        case USIMM_CARD_SIM:

            ucSimType = TAF_SDC_SIM_TYPE_SIM;
            break;

        case USIMM_CARD_USIM:

            ucSimType = TAF_SDC_SIM_TYPE_USIM;
            break;

        case USIMM_CARD_ROM_SIM:
            ucSimType = TAF_SDC_SIM_TYPE_ROM_SIM;
            break;

        case USIMM_CARD_NOCARD:
        case USIMM_CARD_ISIM:
        default:

            ucSimType = TAF_SDC_SIM_TYPE_BUTT;
            break;
    }

    TAF_SDC_SetSimType(ucSimType);
    return;
}





VOS_VOID MMA_ForwardOmMaintainInfoInd(
    VOS_UINT32                          ulReceiverPid,
    VOS_UINT32                          ulMsgName,
    VOS_UINT8                           ucOmConnectFlg,
    VOS_UINT8                           ucOmPcRecurEnableFlg
)
{
    MMCM_OM_MAINTAIN_INFO_IND_STRU     *pstSndMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    /* 消息发送内存申请 */
    pstSndMsg = (MMCM_OM_MAINTAIN_INFO_IND_STRU*)PS_ALLOC_MSG(
                                           WUEPS_PID_MMA,
                                           sizeof(MMCM_OM_MAINTAIN_INFO_IND_STRU)- VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstSndMsg )
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR,
               "MMA_ForwardOmMaintainInfoInd:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    pstSndMsg->ulSenderCpuId        = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid          = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId      = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid        = ulReceiverPid;
    pstSndMsg->ulLength             = sizeof(MMCM_OM_MAINTAIN_INFO_IND_STRU)- VOS_MSG_HEAD_LENGTH;

    pstSndMsg->ulMsgName            = ulMsgName;

    pstSndMsg->ucOmConnectFlg       = ucOmConnectFlg;
    pstSndMsg->ucOmPcRecurEnableFlg = ucOmPcRecurEnableFlg;
    pstSndMsg->aucRsv2[0]           = 0;
    pstSndMsg->aucRsv2[1]           = 0;


    /* 消息发送 */
    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);
    if( VOS_OK != ulRst)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR,
                "MMA_ForwardOmMaintainInfoInd:ERROR: Fail to send message!");
        return;
    }

    return;
}


VOS_VOID MMA_SndAtOmMaintainInfoInd(
    VOS_UINT8                           ucOmConnectFlg,
    VOS_UINT8                           ucOmPcRecurEnableFlg
)
{
    ATMMA_OM_MAINTAIN_INFO_IND_STRU    *pstSndMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;


    /* 消息发送内存申请 */
    pstSndMsg = (ATMMA_OM_MAINTAIN_INFO_IND_STRU*)PS_ALLOC_MSG(
                                           WUEPS_PID_MMA,
                                           sizeof(ATMMA_OM_MAINTAIN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstSndMsg )
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR,
               "MMA_SndAtOmMaintainInfoInd:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    pstSndMsg->ulSenderCpuId        = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid          = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId      = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid        = WUEPS_PID_AT;
    pstSndMsg->ulLength             = sizeof(ATMMA_OM_MAINTAIN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    pstSndMsg->ulMsgName            = AT_MMA_OM_MAINTAIN_INFO_IND;

    pstSndMsg->ucOmConnectFlg       = ucOmConnectFlg;
    pstSndMsg->ucOmPcRecurEnableFlg = ucOmPcRecurEnableFlg;
    pstSndMsg->aucRsv2[0]           = 0;
    pstSndMsg->aucRsv2[1]           = 0;

    /* 消息发送 */
    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);
    if( VOS_OK != ulRst)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR,
                "MMA_SndAtOmMaintainInfoInd:ERROR: Fail to send message!");
        return;
    }

    return;
}



VOS_UINT32 MMA_PhoneGetUsimStatus( VOS_UINT8 *pucSimStatus )
{
    VOS_UINT8               ucUsimStatus;

    /*获取当前USIM状态*/
    /*当调用USIMAPI的值不为0,则表示有错误*/


    ucUsimStatus = USIMM_CARD_SERVIC_ABSENT;

    if( MMA_SUCCESS != MMA_GetUsimStatusFromUsim(&ucUsimStatus, VOS_NULL_PTR) )
    {
        MMA_WARNINGLOG("MMA_PhoneGetUsimStatus():WORNING:Invoke Taf_UsimmGetStatus fail.");
        *pucSimStatus = MMA_USIM_NOREADY;
        return MMA_ERROR;
    }
    /*获取当前USIM状态*/
    if(USIMM_CARD_SERVIC_ABSENT == ucUsimStatus)
    {
        *pucSimStatus = MMA_USIM_ABSENT;

        MMA_NORMAILLOG("MMA_PhoneGetUsimStatus():USIM is not present!");
    }
    else
    {
        *pucSimStatus = MMA_USIM_PRESENT;

        if (USIMM_API_SUCCESS != NAS_USIMMAPI_GetCardIMSI(gstMmaValue.stUsimInfo.UsimStatus.aucImsi))
        {
            MMA_NORMAILLOG("MMA_PhoneGetUsimStatus():USIMM_GetCardIMSI failed!");
        }
        MMA_NORMAILLOG("MMA_PhoneGetUsimStatus():USIM is present!");

    }

    return MMA_SUCCESS;
}



/* BEGIN: Added by liuyang id:48197, 2006/2/28   PN:A32D01672*/

VOS_UINT32 MMA_CheckPin1SatusSimple(VOS_VOID)
{
    TAF_PH_PIN_CNF_STRU  stPinTmp;

    stPinTmp.PinType     = TAF_SIM_PIN;
    /*查询是否需要PIN1码*/
    stPinTmp.OpPinResult =  (VOS_UINT8)NAS_USIMMAPI_GetPinStatus(&(stPinTmp.PinType),
                                                   &(stPinTmp.QueryResult.UsimmEnableFlg),
                                                   &(stPinTmp.QueryResult.UsimmNeedType),
                                                   &(stPinTmp.ucRemain));

    if( stPinTmp.OpPinResult != TAF_PH_OP_PIN_OK )
    {
        MMA_WARNINGLOG("MMA_CheckPin1SatusSimple():WARNING:QUERY PIN Status Fail!");
        return MMA_PIN1_STATUS_FAIL;
    }
    else
    {
        if ( USIMM_PIN_NEED == stPinTmp.QueryResult.UsimmNeedType )
        {
            MMA_NORMAILLOG("MMA_CheckPin1SatusSimple():NORMAL:USIM ENABLE,RETURN NEED!");
            return MMA_NEED_PIN1;
        }
        else if( USIMM_PUK_NEED == stPinTmp.QueryResult.UsimmNeedType )
        {
            MMA_NORMAILLOG("MMA_CheckPin1SatusSimple():NORMAL:USIM ENABLE,RETURN NEED!");
            return MMA_NEED_PUK1;
        }
        else if( USIMM_PUK_BLOCK == stPinTmp.QueryResult.UsimmNeedType )
        {
            MMA_NORMAILLOG("MMA_CheckPin1SatusSimple():NORMAL:USIM ENABLE,RETURN NEED!");
            return MMA_PUK1_REMAINS_ZERO;
        }
        else
        {
            MMA_NORMAILLOG("MMA_CheckPin1SatusSimple():NORMAL:USIM ENABLE,RETURN NO NEED!");
            return MMA_NONEED_PIN1;
        }
    }
}
/* END:   Added by liuyang id:48197, 2006/2/28 */

/* BEGIN: Added by liuyang id:48197, 2006/3/2   PN:A32D02310*/
/*****************************************************************************
 函 数 名  : MMA_StartInternalTimer
 功能描述  : 启动内部定时器用于延时
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年3月2日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

  2.日    期   : 2006年3月15日
    作    者   : liuyang id:48197
    修改内容   : 增加启动定时器状态判断
*****************************************************************************/
VOS_UINT32 MMA_StartInternalTimer(VOS_UINT32    ulTimes,
                                  VOS_UINT32    ulOpType,
                                  VOS_UINT16 ClientId,
                                  VOS_UINT8        OpId)
{
    VOS_UINT32 ulRet;

    if ( ulTimes < 1)
    {
        MMA_WARNINGLOG("MMA_StartInternalTimer():WARNING:TIMES IS ZERO, NO DELAY");
        return MMA_ERROR;
    }
    if ( MMA_INTERNAL_OP_NONE  == ulOpType)
    {
        MMA_WARNINGLOG("MMA_StartInternalTimer():WARNING:OP TYPE NONE, NO DELAY");
        return MMA_ERROR;
    }
    /* BEGIN: Added by liuyang id:48197, 2006/3/15   PN:A32D02500*/
    if( gstMmaInternalTimer.stTimer.ucTimerStatus != MMA_TIMER_STOP)
    {
        MMA_WARNINGLOG("MMA_StartInternalTimer():WARNING:TIMER HAS ALREADY START");
        return MMA_ERROR;
    }
    /* END:   Added by liuyang id:48197, 2006/3/15 */
    gstMmaInternalTimer.ulTimes                       = ulTimes;
    gstMmaInternalTimer.ulOpType                      = ulOpType;
    gstMmaInternalTimer.ClientId                      = ClientId;
    gstMmaInternalTimer.OpId                          = OpId;
    /*启动定时器*/
    ulRet =  NAS_StartRelTimer(  &(gstMmaInternalTimer.stTimer.MmaHTimer),
                                 WUEPS_PID_MMA,
                                 MMA_INTERNAL_TIME,
                                 TAF_MMA,
                                 MMA_INTERNAL_TIMER_ID,
                                 VOS_RELTIMER_NOLOOP );
    if( ulRet != VOS_OK )
    {
        MMA_ERRORLOG("MMA_StartInternalTimer():ERROR:Start Timer Error!");
        /*返回操作失败*/
        gstMmaInternalTimer.stTimer.ucTimerStatus     = MMA_TIMER_ERROR;
        return MMA_ERROR;
    }
    gstMmaInternalTimer.stTimer.ucTimerStatus         = MMA_TIMER_RUN;

    return MMA_SUCCESS;
}


VOS_VOID MMA_ExpiredInternalTimer(VOS_VOID)
{

    if ( MMA_TIMER_RUN != gstMmaInternalTimer.stTimer.ucTimerStatus )
    {
        MMA_WARNINGLOG("MMA_ExpiredInternalTimer():WARNING:It's not wait for this Timer!");
        return;
    }
    /*设置定时器状态标志*/
    gstMmaInternalTimer.stTimer.ucTimerStatus = MMA_TIMER_STOP;
    switch ( gstMmaInternalTimer.ulOpType )
    {
        case MMA_INTERNAL_OP_CLASS_CHANGE :
            /*切换过程中启动定时器延时,此时应该启动ATTACH过程*/
            /*判断最终要设定的手机类型*/
            if ( TAF_PH_MS_CLASS_CC ==
                  gstMmaValue.stSetMsClass.NewMsClassType)
            {
               /*NULL -> CC*/
               /* BEGIN: Added by liuyang, 2005/10/26 */
               gstMmaValue.stSetMsClass.enSetMsClassTypeDeAttFlg = MMA_SET_CLASSTYPE_ATTACH_ENABLE;
               MMA_PhoneAttach(gstMmaInternalTimer.ClientId, gstMmaInternalTimer.OpId, TAF_PH_CS_OPERATE);
               /*Sta_Attach(opId, TAF_CS_ATTACH);  */
               /* END:   Added by liuyang, 2005/10/26 */
            }
            else if(TAF_PH_MS_CLASS_CG ==
                   gstMmaValue.stSetMsClass.NewMsClassType)
            {
                /*NULL -> CG*/
               gstMmaValue.stSetMsClass.enSetMsClassTypeDeAttFlg = MMA_SET_CLASSTYPE_ATTACH_ENABLE;
               MMA_PhoneAttach(gstMmaInternalTimer.ClientId, gstMmaInternalTimer.OpId, TAF_PH_PS_OPERATE);
            }
            else
            {
               /*打印错误消息*/
               MMA_WARNINGLOG("MMA_ExpiredInternalTimer():WARNING: Error MS class attach type  input");
               /*去掉控制内部过程标志变量*/
               gstMmaValue.stSetMsClass.enSetMsClassTypeDeAttFlg = MMA_SET_CLASSTYPE_DE_ATTACH_ENABLE;
            }

            break;


        case MMA_SET_PREF_PLMN_REQ :
            /*调用超时处理函数*/
            MMA_SetPrefPlmnExpired();
            break;
        case MMA_GET_USIM_ICC_ID:
            if ( gstMmaInternalTimer.ulTimes < MMA_WAIT_GET_USIM_ICC_ID_MAX_TIMES )
            {
                if (  MMA_SUCCESS == MMA_StartInternalTimer(gstMmaInternalTimer.ulTimes+1,
                                                            MMA_GET_USIM_ICC_ID,
                                                            gstMmaInternalTimer.ClientId,
                                                            gstMmaInternalTimer.OpId))
                {
                    MMA_NORMAILLOG("MMA_ExpiredInternalTimer():NORMAL:Continue to get ICC ID Ready");
                    return;
                }
                else
                {
                    MMA_WARNINGLOG("MMA_ExpiredInternalTimer():WARNING:START INTERNAL TIMER FAIL for ICC ID Ready");
                    /*调用超时处理函数*/
                    MMA_GetUsimIccIdExpired();
                }
            }
            else
            {
                /*调用超时处理函数*/
                MMA_GetUsimIccIdExpired();
            }
            break;

        case MMA_GET_USIM_PNN:
            if ( gstMmaInternalTimer.ulTimes < MMA_WAIT_GET_USIM_FILE_MAX_TIMES )
            {
                if (  MMA_SUCCESS == MMA_StartInternalTimer(gstMmaInternalTimer.ulTimes+1,
                                                            MMA_GET_USIM_PNN,
                                                            gstMmaInternalTimer.ClientId,
                                                            gstMmaInternalTimer.OpId))
                {
                    MMA_NORMAILLOG("MMA_ExpiredInternalTimer():NORMAL:Continue to get PNN Ready");
                    return;
                }
                else
                {
                    MMA_WARNINGLOG("MMA_ExpiredInternalTimer():WARNING:START INTERNAL TIMER FAIL for PNN Ready");
                    /*调用超时处理函数*/
                    MMA_ReportParaQueryError(gstMmaInternalTimer.ClientId,
                             gstMmaInternalTimer.OpId,
                             TAF_ERR_UNSPECIFIED_ERROR,
                             TAF_PH_PNN_PARA);
                }
            }
            else
            {
                /*调用超时处理函数*/
                MMA_ReportParaQueryError(gstMmaInternalTimer.ClientId,
                             gstMmaInternalTimer.OpId,
                             TAF_ERR_UNSPECIFIED_ERROR,
                             TAF_PH_PNN_PARA);
            }
            break;

        case MMA_GET_USIM_OPL:
            if ( gstMmaInternalTimer.ulTimes < MMA_WAIT_GET_USIM_FILE_MAX_TIMES )
            {
                if (  MMA_SUCCESS == MMA_StartInternalTimer(gstMmaInternalTimer.ulTimes+1,
                                                            MMA_GET_USIM_OPL,
                                                            gstMmaInternalTimer.ClientId,
                                                            gstMmaInternalTimer.OpId))
                {
                    MMA_NORMAILLOG("MMA_ExpiredInternalTimer():NORMAL:Continue to get OPL Ready");
                    return;
                }
                else
                {
                    MMA_WARNINGLOG("MMA_ExpiredInternalTimer():WARNING:START INTERNAL TIMER FAIL for PNN Ready");
                    /*调用超时处理函数*/
                    MMA_ReportParaQueryError(gstMmaInternalTimer.ClientId,
                             gstMmaInternalTimer.OpId,
                             TAF_ERR_UNSPECIFIED_ERROR,
                             TAF_PH_OPL_PARA);
                }
            }
            else
            {
                    /*调用超时处理函数*/
                    MMA_ReportParaQueryError(gstMmaInternalTimer.ClientId,
                             gstMmaInternalTimer.OpId,
                             TAF_ERR_UNSPECIFIED_ERROR,
                             TAF_PH_OPL_PARA);
            }
            break;
        case MMA_GET_USIM_CPNN:
            if ( ( gstMmaInternalTimer.ulTimes < MMA_WAIT_GET_USIM_FILE_MAX_TIMES )
                && ( MMA_CPNN_WAIT_NULL != gstMmaValue.stCPNN.ucWaitPnnOPLMask) )
            {
                if (MMA_CPNN_WAIT_PNN_FLAG == (gstMmaValue.stCPNN.ucWaitPnnOPLMask & MMA_CPNN_WAIT_PNN_FLAG))
                {
                    if (  MMA_SUCCESS == MMA_StartInternalTimer(gstMmaInternalTimer.ulTimes+1,
                                                                MMA_GET_USIM_CPNN,
                                                                gstMmaInternalTimer.ClientId,
                                                                gstMmaInternalTimer.OpId))
                    {
                        MMA_NORMAILLOG("MMA_ExpiredInternalTimer():NORMAL:Continue to get PNN Ready");
                        return;
                    }
                    else
                    {
                        MMA_WARNINGLOG("MMA_ExpiredInternalTimer():WARNING:START INTERNAL TIMER FAIL for PNN Ready");
                        /*调用超时处理函数*/
                        MMA_ReportParaQueryError(gstMmaInternalTimer.ClientId,
                                 gstMmaInternalTimer.OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 TAF_PH_CPNN_PARA);
                        gstMmaValue.stCPNN.ucWaitPnnOPLMask = MMA_CPNN_WAIT_NULL;
                    }
                }
                else if (MMA_CPNN_WAIT_OPL_FLAG == (gstMmaValue.stCPNN.ucWaitPnnOPLMask & MMA_CPNN_WAIT_OPL_FLAG))
                {
                    if (  MMA_SUCCESS == MMA_StartInternalTimer(gstMmaInternalTimer.ulTimes+1,
                                                                MMA_GET_USIM_CPNN,
                                                                gstMmaInternalTimer.ClientId,
                                                                gstMmaInternalTimer.OpId))
                    {
                        MMA_NORMAILLOG("MMA_ExpiredInternalTimer():NORMAL:Continue to get OPL Ready");
                        return;
                    }
                    else
                    {
                        MMA_WARNINGLOG("MMA_ExpiredInternalTimer():WARNING:START INTERNAL TIMER FAIL for PNN Ready");
                        /*调用超时处理函数*/
                        MMA_ReportParaQueryError(gstMmaInternalTimer.ClientId,
                                 gstMmaInternalTimer.OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 TAF_PH_CPNN_PARA);
                        gstMmaValue.stCPNN.ulPNNGetRslt = VOS_FALSE;
                        gstMmaValue.stCPNN.ucWaitPnnOPLMask = MMA_CPNN_WAIT_NULL;
                        if (VOS_NULL_PTR != gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf )
                        {
                            PS_MEM_FREE(WUEPS_PID_MMA, gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf);
                            gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf = VOS_NULL_PTR;
                        }
                    }
                }
                else
                {
                }
            }
            else
            {
                    /*调用超时处理函数*/
                MMA_ReportParaQueryError(gstMmaInternalTimer.ClientId,
                         gstMmaInternalTimer.OpId,
                         TAF_ERR_UNSPECIFIED_ERROR,
                         TAF_PH_CPNN_PARA);
                gstMmaValue.stCPNN.ulPNNGetRslt = VOS_FALSE;
                gstMmaValue.stCPNN.ucWaitPnnOPLMask = MMA_CPNN_WAIT_NULL;
                if (VOS_NULL_PTR != gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf )
                {
                    PS_MEM_FREE(WUEPS_PID_MMA, gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf);
                    gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf = VOS_NULL_PTR;
                }

                if (VOS_NULL_PTR != gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf )
                {
                    PS_MEM_FREE(WUEPS_PID_MMA, gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf);
                    gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf = VOS_NULL_PTR;
                }
            }
            break;

        case MMA_GET_USIM_MNC_LENGTH_OP:
            MMA_WARNINGLOG("MMA_ExpiredInternalTimer():WARNING:START INTERNAL TIMER FAIL for HPLMN MNC LEN");
            MMA_ParaQueryReport(gstMmaValue.stGetHplmn.usClientId,
                                gstMmaValue.stGetHplmn.ucOpId,
                                TAF_PH_HOMEPLMN_VALUE_PARA,
                                TAF_ERR_ERROR, VOS_NULL_PTR);
            gstMmaValue.stGetHplmn.ucHplmnQryFlag = VOS_FALSE;
            break;

        default:
            MMA_WARNINGLOG("MMA_ExpiredInternalTimer:WARNING:NONE Op Type fit the Timer");
            break;
    }
    /*清除当前内部定时器全局标志*/
    TAF_MMA_InitInternalTimer();

    return;
}

/* END:   Added by liuyang id:48197, 2006/3/2 */

/* BEGIN: Added by liuyang id:48197, 2006/3/8   PN:A32D02383*/

VOS_UINT32 Mma_CheckPin(TAF_PH_PIN_DATA_STRU stPinData)
{
    VOS_UINT32                          ulTemp;
    if ( MMA_SUCCESS != Mma_CheckPinCmdType(stPinData.CmdType))
    {
        MMA_WARNINGLOG("Mma_CheckPin():WARNING:PIN cmd Type error!");
        return MMA_ERROR;
    }
    if ( MMA_SUCCESS != Mma_CheckPinTypes(stPinData.PinType))
    {
        MMA_WARNINGLOG("Mma_CheckPin():WARNING:PIN TYPES ERROR!");
        return MMA_ERROR;
    }
    /*只有CHANGE与UNBLOCK的 时候才会使用NEWPIN，其余命令只需使用OLDPIN，
    查询命令时不关心PIN*/
    switch ( stPinData.CmdType )
    {
        /* BEGIN: Added by liuyang id:48197, 2006/3/16   PN:A32D02412*/
        case TAF_PIN_UNBLOCK:
        /* END:   Added by liuyang id:48197, 2006/3/16 */
        case TAF_PIN_CHANGE:
            ulTemp = Mma_CheckPinAvail(stPinData.aucOldPin);
            if ( (MMA_SUCCESS != Mma_CheckPinAvail(stPinData.aucNewPin))
               ||(MMA_SUCCESS != ulTemp)
               )
            {
                MMA_WARNINGLOG("Mma_CheckPin():WARNING:PIN is Error");
                return MMA_ERROR;
            }
            return MMA_SUCCESS;
        case TAF_PIN_VERIFY:
        case TAF_PIN_DISABLE:
        case TAF_PIN_ENABLE:
        /* BEGIN: Deleted by liuyang id:48197, 2006/3/16   PN:A32D02412*/
        /*case TAF_PIN_UNBLOCK:*/
        /* END:   Deleted by liuyang id:48197, 2006/3/16 */
            return Mma_CheckPinAvail(stPinData.aucOldPin);
        case TAF_PIN_QUERY:
        case TAF_PIN_RemainTime:
        case TAF_PIN2_QUERY:
            return MMA_SUCCESS;
        default:
            MMA_WARNINGLOG("Mma_CheckPin():WARNING:PIN cmd Type error in switch");
            return MMA_ERROR;
    }
}

/*****************************************************************************
 函 数 名  : Mma_CheckPinTypes
 功能描述  : 校验PIN类型
 输入参数  : TAF_PH_PIN_TYPE PinTypes
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年3月8日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 Mma_CheckPinTypes(TAF_PH_PIN_TYPE PinTypes)
{
    switch ( PinTypes)
    {
        case TAF_SIM_PIN:
        case TAF_SIM_PUK:
        case TAF_SIM_PIN2:
        case TAF_SIM_PUK2:
        case TAF_SIM_NON:
            return MMA_SUCCESS;
        default:
            MMA_WARNINGLOG("Mma_CheckPinTypes():WARNING:UNSUPPORT Pin Types");
            return MMA_ERROR;
    }
}


VOS_UINT32 Mma_CheckPinCmdType(TAF_PH_PIN_CMD_TYPE PinCmdType)
{
    VOS_UINT32 ulRet;

    switch ( PinCmdType)
    {
        case TAF_PIN_VERIFY:
        case TAF_PIN_CHANGE:
        case TAF_PIN_DISABLE:
        case TAF_PIN_ENABLE:
        case TAF_PIN_UNBLOCK:
        case TAF_PIN_QUERY:
        case TAF_PIN_RemainTime:
        case TAF_PIN2_QUERY:
            ulRet = MMA_SUCCESS;
            break;
        default:
            MMA_WARNINGLOG("Mma_CheckPinCmdType():WARNING:UNKNOW PIN CMD TYPE");
            ulRet = MMA_ERROR;
            break;
    }
    return ulRet;
}

/*****************************************************************************
 函 数 名  : Mma_CheckPinLength
 功能描述  : Pin长度判断
 输入参数  : VOS_UINT8 *pucPin
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年3月8日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 Mma_CheckPinLength(VOS_UINT8 *pucPin)
{
    if ( VOS_NULL_PTR == pucPin)
    {
        MMA_WARNINGLOG("Mma_CheckPinLength():WARNING:PIN is NULL PTR");
        return MMA_ERROR;
    }
/*
    ulTemp = VOS_StrLen((VOS_INT8 *)pucPin);
    if ( (ulTemp > MMA_PIN_LEN_MAX)
       ||(ulTemp < MMA_PIN_LEN_MIN)
       )
    {
        MMA_WARNINGLOG("Mma_CheckPinLength():WARNING:PIN LENGTH is Error");
        return MMA_ERROR;
    }
 */
    return MMA_SUCCESS;
}

/* BEGIN: Added by liuyang id:48197, 2006/3/8   PN:A32D02407*/
/*****************************************************************************
 函 数 名  : Mma_CheckPinValue
 功能描述  : 检查PIN值有效性
 输入参数  : VOS_UINT8 *pucPin
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年3月8日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 Mma_CheckPinValue(VOS_UINT8 *pucPin)
{
    if ( VOS_NULL_PTR == pucPin)
    {
        MMA_WARNINGLOG("Mma_CheckPinValue():WARNING:PIN is NULL PTR");
        return MMA_ERROR;
    }
/*
    for ( ulI = 0; ulI < VOS_StrLen((VOS_INT8 *)pucPin) ; ulI++ )
    {
        if ( (pucPin[ulI] >'9' )
           ||(pucPin[ulI] <'0')
           )
        {
            MMA_WARNINGLOG("Mma_CheckPinValue():WARNING:PIN Value is Error");
            return MMA_ERROR;
        }
    }
*/
    return MMA_SUCCESS;
}

/*****************************************************************************
 函 数 名  : Mma_CheckPinAvail
 功能描述  : 检查PIN有效性
 输入参数  : VOS_UINT8 *pucPin
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年3月8日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 Mma_CheckPinAvail(VOS_UINT8 *pucPin)
{
    if ( MMA_SUCCESS != Mma_CheckPinLength(pucPin) )
    {
        MMA_WARNINGLOG("Mma_CheckPinAvail():WARNING:PIN Length is Error");
        return MMA_ERROR;
    }
    else
    {
        if ( MMA_SUCCESS != Mma_CheckPinValue(pucPin))
        {
            MMA_WARNINGLOG("Mma_CheckPinAvail():WARNING:PIN Value is Error");
            return MMA_ERROR;
        }
    }
    return MMA_SUCCESS;
}

/* MMA_InitTi位置移动 */

/* NAS_MMA_RestoreContextData函数位置移动 */


VOS_UINT32 NAS_MMA_SndOutsideContextData()
{
    NAS_MMA_SDT_MSG_ST                      *pSndMsgCB     = VOS_NULL_PTR;
    NAS_MMA_OUTSIDE_RUNNING_CONTEXT_ST      *pstOutsideCtx;

    VOS_UINT8                               *pucImsi = VOS_NULL_PTR;

    pucImsi = TAF_SDC_GetSimImsi();

    pSndMsgCB = (NAS_MMA_SDT_MSG_ST *)PS_ALLOC_MSG(WUEPS_PID_MMA, sizeof(NAS_MMA_SDT_MSG_ST));

    if ( VOS_NULL_PTR == pSndMsgCB )
    {
        MMA_ERRORLOG("NAS_MMA_SndOutsideContextData:ERROR: Alloc Memory Fail.");
        return MMA_ERROR;
    }

    pstOutsideCtx = &pSndMsgCB->stOutsideCtx;

    VOS_StrNCpy(pstOutsideCtx->acVersionBuffer, PROTOCOL_STACK_VERSION, VER_MAX_LENGTH);

    pstOutsideCtx->ucSimType = g_MmaNewValue.ucSimType;

    PS_MEM_CPY(pstOutsideCtx->pc_g_aucMmaImsi, pucImsi, 9);

    PS_MEM_CPY(pstOutsideCtx->pc_g_aucMmaImei, g_aucMmaImei, TAF_PH_IMEI_LEN-1);


    PS_MEM_CPY(&pstOutsideCtx->pc_g_StatusContext, &g_StatusContext, sizeof(STATUS_CONTEXT_STRU));

    PS_MEM_CPY(&pstOutsideCtx->pc_gstMmaValue, &gstMmaValue, sizeof(TAF_MMA_GLOBAL_VALUE_ST));
    PS_MEM_CPY(&pstOutsideCtx->pc_g_stMmaMePersonalisationStatus, &g_stMmaMePersonalisationStatus, sizeof(MMA_ME_PERSONALISATION_STATUS_STRU));
    PS_MEM_CPY(pstOutsideCtx->pc_gastMmaTiTab, gastMmaTiTab, MMA_MAX_TI*sizeof(MMA_TI_TABLE));

    PS_MEM_CPY(&pstOutsideCtx->pc_gstMmaInternalTimer, &gstMmaInternalTimer, sizeof(MMA_INTERNAL_TIMER_ST));

    PS_MEM_CPY(pstOutsideCtx->pc_g_MmaSimTimer, g_MmaSimTimer, TAF_SIM_TIMER_NUM*sizeof(MMA_TIMER_ST));

    pSndMsgCB->ulReceiverPid = WUEPS_PID_MMA;
    pSndMsgCB->ulSenderPid   = WUEPS_PID_MMA;
    pSndMsgCB->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pSndMsgCB->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSndMsgCB->ulLength      = sizeof(NAS_MMA_OUTSIDE_RUNNING_CONTEXT_ST) + 4;
    pSndMsgCB->usMsgID       = EVT_NAS_MMA_OUTSIDE_RUNNING_CONTEXT_FOR_PC_REPLAY;

    OM_TraceMsgHook(pSndMsgCB);
    PS_FREE_MSG(WUEPS_PID_MMA, pSndMsgCB);

    return MMA_SUCCESS;
}

/* NAS_MMA_RestoreFixedContextData函数位置移动 */

VOS_UINT32 NAS_MMA_SndOutsideFixedContextData()
{
    NAS_MMA_FIXED_CONTEXT_MSG_ST                   *pSndMsgCB     = VOS_NULL_PTR;
    NAS_MMA_FIXED_PART_CONTEXT_ST                  *pstOutsideCtx;
    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU             *pstLastSyscfgSet = VOS_NULL_PTR;
    TAF_PH_RAT_ORDER_STRU                          *pstAllRatOrder      = VOS_NULL_PTR;
    MN_MMA_CPAM_RAT_PRIO_ENUM_UINT8                 enCpamPlmnPrio;
    MN_MMA_CPAM_RAT_TYPE_ENUM_UINT8                 enAccessMode;

    VOS_UINT8                          *pucImsi = VOS_NULL_PTR;

    pstLastSyscfgSet = MN_MMA_GetLastSyscfgSetAddr();

    pucImsi          = TAF_SDC_GetSimImsi();

    pSndMsgCB = (NAS_MMA_FIXED_CONTEXT_MSG_ST *)PS_ALLOC_MSG(WUEPS_PID_MMA, sizeof(NAS_MMA_FIXED_CONTEXT_MSG_ST));

    if ( VOS_NULL_PTR == pSndMsgCB )
    {
        MMA_ERRORLOG("NAS_MMA_SndOutsideFixedContextData:ERROR: Alloc Memory Fail.");
        return MMA_ERROR;
    }

    pstOutsideCtx = &pSndMsgCB->stFixedOutsideCtx;

    /* version 0 part variable */
    PS_MEM_CPY(pstOutsideCtx->aucMmaImsi, pucImsi, 9);

    pstOutsideCtx->aucSimType = g_MmaNewValue.ucSimType;

    pstOutsideCtx->ulPrefBand = pstLastSyscfgSet->stUserSetBand.uUserSetGuBand.ulPrefBand;
    pstOutsideCtx->Band = pstLastSyscfgSet->stUserSetBand.uUserSetUeFormatGuBand;
    pstOutsideCtx->ulQuickStartFlg = gstMmaValue.ulQuickStartFlg;

    pstOutsideCtx->stEfustServiceCfg = gstMmaValue.stEfustServiceCfg;

    PS_MEM_CPY(pstOutsideCtx->aucImeiBcd, g_MmaNewValue.aucImeiBcd, TAF_PH_IMEI_LEN+1);
    pstAllRatOrder = MN_MMA_GetRatPrioListAddr();

    MN_MMA_GetCpamGURatPrio(&enCpamPlmnPrio, pstAllRatOrder);
    MN_MMA_GetCpamGUAccessRat(&enAccessMode, pstAllRatOrder);

    /* 根据gstMmaValue.stRatPrioList给pstOutsideCtx->AccessMode和pstOutsideCtx->PlmnPrio赋值 */
    pstOutsideCtx->enAccessMode   = enAccessMode;
    pstOutsideCtx->PlmnPrio       = enCpamPlmnPrio + TAF_PLMN_PRIO_DIST_BTWN_CPAM_SYSCFG;

    pstOutsideCtx->MsClassType = gstMmaValue.stSetMsClass.MsClassType;
    pstOutsideCtx->ucRoam = pstLastSyscfgSet->ucRoam;
    pstOutsideCtx->ucRoamFeatureStatus = gstMmaValue.ucRoamFeatureStatus;

    pstOutsideCtx->stMeInfo = gstMmaValue.stMeInfo;

    pstOutsideCtx->stSimLockPlmnInfo = g_MmaNewValue.stSimLockPlmnInfo;
    pstOutsideCtx->stCardLockStatus = g_MmaNewValue.stCardLockStatus;
    pstOutsideCtx->stSimLockMaxTimes = g_MmaNewValue.stSimLockMaxTimes;

    PS_MEM_CPY(pstOutsideCtx->aucPwd, g_MmaNewValue.aucPwd, TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX+1);
    pstOutsideCtx->usSimMePersonalisationActFlag = g_MmaNewValue.usSimMePersonalisationActFlag;
    PS_MEM_CPY(pstOutsideCtx->aucSimPersonalisationStr, g_MmaNewValue.aucSimPersonalisationStr, TAF_MAX_IMSI_LEN+1);

    /* version 1 part variable */
    pstOutsideCtx->gstMmaBandCapa = gstMmaBandCapa;

    pstOutsideCtx->ulVersion = 1;

    pSndMsgCB->ulReceiverPid = WUEPS_PID_MMA;
    pSndMsgCB->ulSenderPid   = WUEPS_PID_MMA;
    pSndMsgCB->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pSndMsgCB->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSndMsgCB->ulLength      = sizeof(NAS_MMA_FIXED_PART_CONTEXT_ST) + 4;
    pSndMsgCB->usMsgID       = EVT_NAS_MMA_FIXED_PART_CONTEXT;

    OM_TraceMsgHook(pSndMsgCB);
    PS_FREE_MSG(WUEPS_PID_MMA, pSndMsgCB);

    return MMA_SUCCESS;
}



/*****************************************************************************
 函 数 名  : MMA_OpPinSuccess
 功能描述  : 操作PIN是否成功
 输入参数  : TAF_PH_PIN_CNF_STRU *pstPinCnf
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年3月14日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数
  2.日    期   : 2007年02月09日
    作    者   : luojian id:60022475
    修改内容   : 根据问题单号A32D08488
*****************************************************************************/
VOS_UINT32 MMA_OpPin1Success(const TAF_PH_PIN_CNF_STRU *pstPinCnf)
{


    if ( (TAF_SIM_PIN != pstPinCnf->PinType) && (TAF_SIM_PUK != pstPinCnf->PinType) )
    {
        return MMA_FALSE;
    }

    if ( TAF_PH_OP_PIN_OK != pstPinCnf->OpPinResult )
    {
       return MMA_FALSE;
    }
    return MMA_TRUE;
}

/*****************************************************************************
 函 数 名  : MMA_Pin1Noneed
 功能描述  : 判断是否需要PIN1
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年3月14日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 MMA_Pin1Noneed()
{
    VOS_UINT32 ulRet;
    /*检查PIN状态*/
    ulRet = MMA_CheckPin1SatusSimple();
    /*PIN使能*/
    if ( MMA_NONEED_PIN1 != ulRet )
    {
        MMA_NORMAILLOG("MMA_Pin1Noneed():NORMAL:NEED PIN");
        return MMA_FALSE;
    }
    else
    {
        MMA_NORMAILLOG("MMA_Pin1Noneed():NORMAL:NO NEED PIN");
        return MMA_TRUE;
    }
}

/*****************************************************************************
 函 数 名  : MMA_CheckUsimStatus
 功能描述  : 检查USIM状态是否启动,PIN状态是否可用
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年3月15日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 MMA_CheckUsimStatus()
{
    VOS_UINT8   ucUsimStatus;
    VOS_UINT32  ulRet;

    ulRet = MMA_PhoneGetUsimStatus(&ucUsimStatus);
    if ( (MMA_SUCCESS != ulRet )
       ||((  (ucUsimStatus != MMA_USIM_ABSENT)
              &&(ucUsimStatus != MMA_USIM_PRESENT)
             )
         )
       )
    {
        /*USIM初始化未完成，启动定时器等待*/
        /*MMA_StartInitialTimer();*/
        MMA_WARNINGLOG("MMA_CheckUsimStatus():WARNING:USIM IS NOT READY");
        return MMA_ERROR;
    }
    else
    {
        if ( MMA_USIM_PRESENT == ucUsimStatus)
        {
            /*PIN使能*/
            ulRet = MMA_CheckPin1SatusSimple();
            if (MMA_PIN1_STATUS_FAIL == ulRet)
            {
                MMA_NORMAILLOG("MMA_CheckUsimStatus():NORMAL:PIN1 STATUS NOT READY");
                return MMA_ERROR;
            }
            else if ( MMA_NEED_PIN1 == ulRet )
            {
                MMA_NORMAILLOG("MMA_CheckUsimStatus():NORMAL:PIN1 NEED");
                return MMA_NORMAL;
            }
            else
            {
                MMA_NORMAILLOG("MMA_CheckUsimStatus():NORMAL:PIN1 NO NEED");
            }
        }
        return MMA_SUCCESS;
    }
}
TAF_PH_MS_CLASS_TYPE MMA_GetCurMsClass()
{
    TAF_PH_MS_CLASS_TYPE                Result;
    VOS_UINT32                          ulDataLen;
    NAS_NVIM_MS_CLASS_STRU              stMsClass;

    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode;

    enSysMode = TAF_SDC_GetSysMode();
    ulDataLen = 0;

    if (  (TAF_PH_MS_CLASS_A == gstMmaValue.pg_StatusContext->ucModeService)
        &&(TAF_SDC_SYS_MODE_GSM     == enSysMode)
       )
    {
        return TAF_PH_MS_CLASS_B;
    }

    if (TAF_PH_MS_CLASS_NULL == gstMmaValue.pg_StatusContext->ucModeService )
    {

        PS_MEM_SET(&stMsClass, 0x00, sizeof(NAS_NVIM_MS_CLASS_STRU));

        NV_GetLength(en_NV_Item_MMA_MsClass, &ulDataLen);

        if (NV_OK != NV_Read(en_NV_Item_MMA_MsClass ,
                             &stMsClass,
                             ulDataLen))
        {
            MMA_WARNINGLOG("MMA_GetCurMsClass():WARNING:Read from NVIM MSClass Fail");
            Result = TAF_PH_MS_CLASS_A;/*TAF_PH_MS_CLASS_CC TAF_PH_MS_CLASS_CG*/
        }
        else
        {
            Result = stMsClass.ucMsClass;
        }
    }
    else
    {
        Result = gstMmaValue.pg_StatusContext->ucModeService;
    }
    return Result;
}

/* 删除MMA_TranslateIntIntoBitInRegStates */


/*****************************************************************************
 函 数 名  : MMA_ClearCurRegOp
 功能描述  : 清空注册状态结构中所有的OP项
 输入参数  : TAF_PH_REG_STATE_STRU  *pstCurRegSta
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年4月13日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MMA_ClearCurRegOp(TAF_PH_REG_STATE_STRU  *pstCurRegSta)
{
    pstCurRegSta->OP_CsRegState = MMA_FALSE;
    pstCurRegSta->OP_PsRegState = MMA_FALSE;
    pstCurRegSta->OP_ucAct      = MMA_FALSE;
    pstCurRegSta->OP_Lac        = MMA_FALSE;
    pstCurRegSta->OP_Plmn       = MMA_FALSE;
    pstCurRegSta->OP_Rac        = MMA_FALSE;
    pstCurRegSta->OP_CellId     = MMA_FALSE;
    pstCurRegSta->OP_Spare      = MMA_FALSE;
    return;
}




VOS_UINT32 MMA_PhoneGetUsimType( VOS_UINT8 *pucSimType )
{
    VOS_UINT8               ucUsimType;


    ucUsimType = USIMM_CARD_NOCARD;

#ifndef __PS_WIN32_RECUR__
    /*获取当前USIM状态*/
    /*当调用USIMAPI的值不为0,则表示有错误*/
    if  ( MMA_SUCCESS != MMA_GetUsimStatusFromUsim(VOS_NULL_PTR, &ucUsimType) )
    {
        *pucSimType = USIMM_CARD_NOCARD;

        MMA_WARNINGLOG("MMA_PhoneGetUsimType():WORNING:Invoke Taf_UsimmGetStatus fail.");
        return MMA_ERROR;
    }
    if ( USIMM_CARD_SIM == ucUsimType )
    {
        *pucSimType = MMA_USIM_TYPE_SIM;
    }
    else
    {
        *pucSimType = MMA_USIM_TYPE_USIM;
    }

    g_MmaNewValue.ucSimType = *pucSimType;

#else
    *pucSimType = g_MmaNewValue.ucSimType;

#endif

    return MMA_SUCCESS;
}



VOS_UINT32 MMA_StopInternalTimer()
{
    VOS_UINT32 ulRet;
    if ( gstMmaInternalTimer.stTimer.ucTimerStatus != MMA_TIMER_RUN )
    {
        MMA_WARNINGLOG("MMA_StopInternalTimer():WARNING:INTERNAL TIMER IS NOT RUNNING");
    }


    ulRet = NAS_StopRelTimer(WUEPS_PID_MMA, MMA_INTERNAL_TIMER_ID,
                            &(gstMmaInternalTimer.stTimer.MmaHTimer));


    if ( VOS_OK != ulRet )
    {
        MMA_WARNINGLOG("MMA_StopInternalTimer():WARNING:Nas_StopRelTimer failed");
    }
    TAF_MMA_InitInternalTimer();
    return ulRet;
}






VOS_VOID MMA_TurnOffForFlightMode( VOS_VOID )
{
    /*Make DSP into Halt mode, and close DSP PLL, BBP PLL, power off BBP, RF */

    OM_SetDrxTimerWakeSrc(VOS_GetModemIDFromPid(WUEPS_PID_MMA));

    TAF_MMA_SndDrxTimerInfo(VOS_TRUE);
    return;
}
VOS_VOID MMA_TurnOnForFlightMode()
{
    /* 禁止底层睡眠 */
    /* V9R1/V3R3上NAS不再投PS票，删除LTE特性宏控制 */
    TAF_DRVAPI_PWRCTRL_SLEEPVOTE_LOCK(PWRCTRL_SLEEP_NAS);

    if (VOS_TRUE == TAF_MMA_GetMmaLogInfoFlag())
    {
        vos_printf("\n*MMA_TurnOnForFlightMode:PWRCTRL_SLEEP_NAS,TAF_DRVAPI_PWRCTRL_SLEEPVOTE_LOCK\n");
    }
    return;
}
VOS_BOOL MN_PH_IsUsingPlmnSel(VOS_VOID)
{
    VOS_BOOL                            bPlmnSelFlg;
    VOS_UINT8                           ucSimType;

    bPlmnSelFlg = VOS_FALSE;
    ucSimType   = MMA_USIM_TYPE_USIM;

    /*User controlled PLMN Selector with Access Technology*/
    if (PS_USIM_SERVICE_NOT_AVAILIABLE
                    == NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_UPLMN_SEL_WACT))
    {
        MMA_PhoneGetUsimType(&ucSimType);

        if (MMA_USIM_TYPE_SIM == ucSimType)
        {
            if (PS_USIM_SERVICE_AVAILIABLE
                        == NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_PLMN_SELECTOR))

            {
                bPlmnSelFlg = VOS_TRUE;
            }
        }

    }

    return bPlmnSelFlg;

}
VOS_VOID MMA_PhoneGetUPlmn(
    TAF_USER_PLMN_LIST_STRU             *pstUPlmnInfo
)
{
    VOS_UINT32 ulI = 0;
    /*属遗留问题，需要确认上报的参数*/
    MMC_MMA_SHARE_PARA_ST   *pstMmcPara;

    pstMmcPara = (MMC_MMA_SHARE_PARA_ST *)PS_MEM_ALLOC(WUEPS_PID_MMA ,
                                            sizeof(MMC_MMA_SHARE_PARA_ST));

    if (VOS_NULL_PTR == pstMmcPara)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_WARNING, "MMA_PhoneGetUPlmn: Alloc memory fail.");
        return;
    }

    if (VOS_TRUE == MN_PH_IsUsingPlmnSel())
    {
        pstMmcPara->enMmaParaType = EN_MMC_PLMNSEL_INFO;
    }
    else
    {
        pstMmcPara->enMmaParaType = EN_MMC_UPLMN_INFO;
    }

    Api_MmaGetParaFromMmc(pstMmcPara);

    pstUPlmnInfo->usPlmnNum = pstMmcPara->u.stUPlmnInfo.usCurPlmnNum;
    for ( ulI = 0; ulI < pstMmcPara->u.stUPlmnInfo.usCurPlmnNum ; ulI++ )
    {
        pstUPlmnInfo->Plmn[ulI].Mcc = pstMmcPara->u.stUPlmnInfo.astPlmnInfo[ulI].stPlmn.ulMcc;
        pstUPlmnInfo->Plmn[ulI].Mnc = pstMmcPara->u.stUPlmnInfo.astPlmnInfo[ulI].stPlmn.ulMnc;
        MMA_PlmnId2Bcd(&pstUPlmnInfo->Plmn[ulI]);
        pstUPlmnInfo->PlmnRat[ulI] = pstMmcPara->u.stUPlmnInfo.astPlmnInfo[ulI].usRaMode;
    }
    MMA_FREE_MEM(pstMmcPara);

    return;
}
VOS_VOID MMA_PhoneGetOPlmn(
    TAF_USER_PLMN_LIST_STRU             *pstOPlmnInfo
)
{
    VOS_UINT32 ulI = 0;
    /*属遗留问题，需要确认上报的参数*/
    MMC_MMA_SHARE_PARA_ST   *pstMmcPara;

    pstMmcPara = (MMC_MMA_SHARE_PARA_ST *)PS_MEM_ALLOC(WUEPS_PID_MMA ,
                                            sizeof(MMC_MMA_SHARE_PARA_ST));

    if (VOS_NULL_PTR == pstMmcPara)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_WARNING, "MMA_PhoneGetUPlmn: Alloc memory fail.");
        return;
    }

    pstMmcPara->enMmaParaType = EN_MMC_OPLMN_INFO;

    Api_MmaGetParaFromMmc(pstMmcPara);

    pstOPlmnInfo->usPlmnNum = pstMmcPara->u.stOPlmnInfo.usCurPlmnNum;
    for ( ulI = 0; ulI < pstMmcPara->u.stOPlmnInfo.usCurPlmnNum ; ulI++ )
    {
        pstOPlmnInfo->Plmn[ulI].Mcc = pstMmcPara->u.stOPlmnInfo.astPlmnInfo[ulI].stPlmn.ulMcc;
        pstOPlmnInfo->Plmn[ulI].Mnc = pstMmcPara->u.stOPlmnInfo.astPlmnInfo[ulI].stPlmn.ulMnc;
        MMA_PlmnId2Bcd(&(pstOPlmnInfo->Plmn[ulI]));
        pstOPlmnInfo->PlmnRat[ulI] = pstMmcPara->u.stOPlmnInfo.astPlmnInfo[ulI].usRaMode;

    }
    MMA_FREE_MEM(pstMmcPara);

    /*使用数据上报函数将数据传至TAFm*/
}


VOS_VOID MMA_PhoneGetHPlmn(
    TAF_USER_PLMN_LIST_STRU             *pstHPlmnInfo
)
{
    VOS_UINT32 ulI = 0;
    /*属遗留问题，需要确认上报的参数*/
    MMC_MMA_SHARE_PARA_ST   *pstMmcPara;

    pstMmcPara = (MMC_MMA_SHARE_PARA_ST *)PS_MEM_ALLOC(WUEPS_PID_MMA ,
                                            sizeof(MMC_MMA_SHARE_PARA_ST));

    if (VOS_NULL_PTR == pstMmcPara)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_WARNING, "MMA_PhoneGetUPlmn: Alloc memory fail.");
        return;
    }

    pstMmcPara->enMmaParaType = EN_MMC_HPLMN_INFO;

    Api_MmaGetParaFromMmc(pstMmcPara);

    pstHPlmnInfo->usPlmnNum = pstMmcPara->u.stHPlmnInfo.usCurPlmnNum;
    for ( ulI = 0; ulI < pstMmcPara->u.stHPlmnInfo.usCurPlmnNum ; ulI++ )
    {
        pstHPlmnInfo->Plmn[ulI].Mcc = pstMmcPara->u.stHPlmnInfo.astPlmnInfo[ulI].stPlmn.ulMcc;
        pstHPlmnInfo->Plmn[ulI].Mnc = pstMmcPara->u.stHPlmnInfo.astPlmnInfo[ulI].stPlmn.ulMnc;
        MMA_PlmnId2Bcd(&pstHPlmnInfo->Plmn[ulI]);
        pstHPlmnInfo->PlmnRat[ulI] = pstMmcPara->u.stHPlmnInfo.astPlmnInfo[ulI].usRaMode;
    }
    MMA_FREE_MEM(pstMmcPara);

}


/*****************************************************************************
 函 数 名  : MMA_SetUplmnReqExpired
 功能描述  : 设置UPLMN超时处理函数
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年8月2日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MMA_SetPrefPlmnExpired()
{
    /*上报当前更改频段操作失败*/
    MMA_WARNINGLOG("MMA_SetPrefPlmnExpired():WARNING:Set Pref PLMN Expired, Fail");
    Taf_ParaSetResult(gstMmaInternalTimer.ClientId,
                      gstMmaInternalTimer.OpId,
                      TAF_PARA_UNSPECIFIED_ERROR,
                      TAF_PH_SET_PERF_PLMN_PARA,
                      VOS_NULL_PTR);
    return;
}


VOS_VOID MMA_UpdatePlmnInfo(
    VOS_UINT16                          usEfId
)
{
    MMC_MMA_SHARE_PARA_ST           *pstMmcPara;
    VOS_UINT16                       usIndex;

    pstMmcPara = (MMC_MMA_SHARE_PARA_ST *)PS_MEM_ALLOC(WUEPS_PID_MMA ,
                                            sizeof(MMC_MMA_SHARE_PARA_ST));

    if (VOS_NULL_PTR == pstMmcPara)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_WARNING, "MMA_DelPrefPlmnInfo: Alloc memory fail.");
        return ;
    }

    if (EFPLMNsel == usEfId )
    {
        pstMmcPara->enMmaParaType = EN_MMC_PLMNSEL_INFO;
    }
    else if (EFPLMNwAcT == usEfId)
    {
        pstMmcPara->enMmaParaType = EN_MMC_UPLMN_INFO;
    }
    else if (EFOPLMNwAcT == usEfId)
    {
        pstMmcPara->enMmaParaType = EN_MMC_OPLMN_INFO;
    }
    else
    {
        pstMmcPara->enMmaParaType = EN_MMC_HPLMN_INFO;
    }
    usIndex = f_stMmaPlmnInfo.usIndex;

    if (usIndex >= MN_MMA_MAX_UPLMN_NUM)
    {
        PS_MEM_FREE(WUEPS_PID_MMA, pstMmcPara);
        return ;
    }
    Api_MmaGetParaFromMmc(pstMmcPara);
    if ( (EFPLMNsel == usEfId )
      || (EFPLMNwAcT == usEfId))
    {
        pstMmcPara->u.stUPlmnInfo.astPlmnInfo[usIndex].stPlmn.ulMcc = f_stMmaPlmnInfo.stPlmnId.Mcc;
        pstMmcPara->u.stUPlmnInfo.astPlmnInfo[usIndex].stPlmn.ulMnc = f_stMmaPlmnInfo.stPlmnId.Mnc;
        MMA_PlmnId2NasStyle((TAF_PLMN_ID_STRU *)&(pstMmcPara->u.stUPlmnInfo.astPlmnInfo[usIndex].stPlmn));
        pstMmcPara->u.stUPlmnInfo.astPlmnInfo[usIndex].usRaMode = f_stMmaPlmnInfo.PlmnRat;


        /* MMA通知MMC当前UPLMN需要进行更新  */
        TAF_MMA_SndMmcUpdateUplmnNtf();
    }
    else if (EFOPLMNwAcT == usEfId)
    {
        pstMmcPara->u.stOPlmnInfo.astPlmnInfo[usIndex].stPlmn.ulMcc = f_stMmaPlmnInfo.stPlmnId.Mcc;
        pstMmcPara->u.stOPlmnInfo.astPlmnInfo[usIndex].stPlmn.ulMnc = f_stMmaPlmnInfo.stPlmnId.Mnc;
        MMA_PlmnId2NasStyle((TAF_PLMN_ID_STRU *)&(pstMmcPara->u.stUPlmnInfo.astPlmnInfo[usIndex].stPlmn));
        pstMmcPara->u.stUPlmnInfo.astPlmnInfo[usIndex].usRaMode = f_stMmaPlmnInfo.PlmnRat;

    }
    else
    {
        pstMmcPara->u.stHPlmnInfo.astPlmnInfo[usIndex].stPlmn.ulMcc = f_stMmaPlmnInfo.stPlmnId.Mcc;
        pstMmcPara->u.stHPlmnInfo.astPlmnInfo[usIndex].stPlmn.ulMnc = f_stMmaPlmnInfo.stPlmnId.Mnc;
        MMA_PlmnId2NasStyle((TAF_PLMN_ID_STRU *)&(pstMmcPara->u.stUPlmnInfo.astPlmnInfo[usIndex].stPlmn));
        pstMmcPara->u.stUPlmnInfo.astPlmnInfo[usIndex].usRaMode = f_stMmaPlmnInfo.PlmnRat;

    }


    if (EN_MMC_UPLMN_INFO == pstMmcPara->enMmaParaType)
    {
        pstMmcPara->enMmaParaType = EN_MMC_SET_UPLMN_INFO;
    }
    else if (EN_MMC_PLMNSEL_INFO == pstMmcPara->enMmaParaType)
    {
        pstMmcPara->enMmaParaType = EN_MMC_SET_PLMNSEL_INFO;
    }
    else if (EN_MMC_SET_OPLMN_INFO == pstMmcPara->enMmaParaType)
    {
        pstMmcPara->enMmaParaType = EN_MMC_OPLMN_INFO;
    }
    else
    {
        pstMmcPara->enMmaParaType = EN_MMC_HPLMN_INFO;
    }

    Api_MmaSetParaFromMmc(pstMmcPara);

    PS_MEM_FREE(WUEPS_PID_MMA, pstMmcPara);

}


VOS_VOID MMA_SetPrefPlmnCnf(PS_USIM_SET_FILE_CNF_STRU *pstMsg)
{
    VOS_UINT16 ClientId = gstMmaInternalTimer.ClientId;
    VOS_UINT8  OpId = gstMmaInternalTimer.OpId;

    VOS_UINT32                          ulRet;
    /*停止定时器*/
    MMA_StopInternalTimer();

    /*判断消息有效性*/
    if ( 0 != pstMsg->ulResult)
    {
        MMA_WARNINGLOG("MMA_SetUplmnCnf():WARNING:Set UPLMN File Error!");
        ulRet = pstMsg->ulErrorCode;
    }
    else
    {
        ulRet = TAF_PARA_OK;
        /* 更新内存中的变量 */
        MMA_UpdatePlmnInfo(pstMsg->usEfId);
    }

    /*向用户报告信息*/
    Taf_ParaSetResult(ClientId,
                      OpId,
                      (VOS_UINT8)ulRet,
                      TAF_PH_SET_PERF_PLMN_PARA,
                      VOS_NULL_PTR);
    return;
}


VOS_VOID MN_MMA_FillUPlmnFile(
    VOS_UINT16                  usUpdateFile,
    VOS_UINT8                  *pucEfFile,
    TAF_USER_PLMN_LIST_STRU    *pstUPlmnInfo
)
{
    VOS_UINT8  ucRecordLen;
    VOS_UINT16 i;
    VOS_UINT16 usRatMode;

    if (EFPLMNsel == usUpdateFile)
    {
        ucRecordLen = 3;
    }
    else
    {
        ucRecordLen = 5;
    }

    for (i = 0; i < pstUPlmnInfo->usPlmnNum; i++)
    {
        pucEfFile[i*ucRecordLen]  = (VOS_UINT8)(0x0f & (pstUPlmnInfo->Plmn[i].Mcc >> 4));
        pucEfFile[i*ucRecordLen] <<= 4;
        pucEfFile[i*ucRecordLen] |= (VOS_UINT8)(0x0f & (pstUPlmnInfo->Plmn[i].Mcc >> 8));

        pucEfFile[(i*ucRecordLen)+1]  = (VOS_UINT8)(0x0f & pstUPlmnInfo->Plmn[i].Mcc);
        if (0xF0 == (0xf0 & (pstUPlmnInfo->Plmn[i].Mnc >>4)))
        {
            pucEfFile[(i*ucRecordLen)+1] |= (VOS_UINT8)(0xf0 & (pstUPlmnInfo->Plmn[i].Mnc >>4));

            pucEfFile[(i*ucRecordLen)+2]  = (VOS_UINT8)(0x0f & (pstUPlmnInfo->Plmn[i].Mnc));
            pucEfFile[(i*ucRecordLen)+2] <<= 4;
            pucEfFile[(i*ucRecordLen)+2] |= (VOS_UINT8)(0x0f & (pstUPlmnInfo->Plmn[i].Mnc >>4));
        }
        else
        {
            pucEfFile[(i*ucRecordLen)+1] |= (VOS_UINT8)(0xf0 & (pstUPlmnInfo->Plmn[i].Mnc <<4));

            pucEfFile[(i*ucRecordLen)+2]  = (VOS_UINT8)(0x0f & (pstUPlmnInfo->Plmn[i].Mnc >>4));
            pucEfFile[(i*ucRecordLen)+2] <<= 4;
            pucEfFile[(i*ucRecordLen)+2] |= (VOS_UINT8)(0x0f & (pstUPlmnInfo->Plmn[i].Mnc >>8));
        }

        if (5 == ucRecordLen)
        {
            usRatMode = pstUPlmnInfo->PlmnRat[i];
            pucEfFile[(i*ucRecordLen)+3] = (VOS_UINT8)(usRatMode >> 8);
            pucEfFile[(i*ucRecordLen)+4] = (VOS_UINT8)(usRatMode & 0xFF);
        }
    }

    return;
}


VOS_UINT32 MMA_UpdateUsimPrePlmn(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    TAF_USER_PLMN_LIST_STRU            *pstUPlmnInfo,
    VOS_UINT16                          usUpdateFile
)
{
    VOS_UINT8   *pucEfFile;
    VOS_UINT16  usEfLen;
    USIMM_SET_FILE_INFO_STRU            stSetFileInfo;

    /*启动定时器*/
    if(MMA_SUCCESS != MMA_StartInternalTimer(1,
                                             MMA_SET_PREF_PLMN_REQ,
                                             ClientId,
                                             OpId))
    {
        MMA_WARNINGLOG("MMA_UpdateUsimPrePlmn():WARNING:Unknow Error!");
        return MMA_FAILURE;
    }

    if ( (EFPLMNwAcT == usUpdateFile)
      || (EFOPLMNwAcT == usUpdateFile)
      || (EFHPLMNwAcT == usUpdateFile))
    {
        usEfLen = (VOS_UINT16)(5*(pstUPlmnInfo->usPlmnNum));
    }
    else
    {   /*PlmnSel*/
        usEfLen = (VOS_UINT16)(3*(pstUPlmnInfo->usPlmnNum));
    }

    pucEfFile = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_MMA, usEfLen);
    if ( VOS_NULL_PTR ==  pucEfFile)
    {
        MMA_WARNINGLOG("MMA_UpdateUsimPrePlmn():WARNING:mem alloc Error!");

        /*停止定时器*/
        MMA_StopInternalTimer();
        return MMA_FAILURE;
    }

    MMA_MEM_SET(pucEfFile, 0, usEfLen);

    MN_MMA_FillUPlmnFile(usUpdateFile, pucEfFile, pstUPlmnInfo);

    TAF_COMM_BUILD_USIM_SET_FILE_INFO(&stSetFileInfo,
                                    USIMM_UNLIMIT_APP,
                                    usUpdateFile,
                                    0,
                                    usEfLen,
                                    pucEfFile);

    if( USIMM_API_SUCCESS != NAS_USIMMAPI_SetFileReq(WUEPS_PID_MMA, 0, &stSetFileInfo))
    {
        MMA_WARNINGLOG("MMA_UpdateUsimPrePlmn():WARNING:Set File Error!");
        /*停止定时器*/
        MMA_StopInternalTimer();
        /* 释放内存 */
        MMA_FREE_MEM(pucEfFile);
        return MMA_FAILURE;
    }

    MMA_FREE_MEM(pucEfFile);
    return MMA_SUCCESS;
}



/*****************************************************************************
 函 数 名  : MMA_GetProductionVersion
 功能描述  : 得到产品信息
 输入参数  : 保存信息的地址
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年1月13日
    作    者   : 孙少华
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID MMA_GetProductionVersion(VOS_CHAR *pcDest)
{
    VOS_StrNCpy(pcDest, (VOS_CHAR *)gstMmaValue.stMeInfo.SoftVersion.aucRevisionId,
        TAF_MAX_REVISION_ID_LEN+1);
}


VOS_VOID Taf_GetProductionID(VOS_CHAR *pcDest)
{
    VOS_StrNCpy(pcDest, (VOS_CHAR *)gstMmaValue.stMeInfo.ModelId.aucModelId,
        TAF_MAX_MODEL_ID_LEN+1);
}

/*****************************************************************************
 函 数 名  : MMA_WriteVersion
 功能描述  : 将PS版本信息注册到底软
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年1月13日
    作    者   : 孙少华
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID MMA_WriteVersion(VOS_VOID)
{
    VOS_UINT32                 ulRslt;
    VOS_CHAR                   acVersionBuffer[VER_MAX_LENGTH]  = {0};

    VOS_StrNCpy(acVersionBuffer, PROTOCOL_STACK_VERSION, VER_MAX_LENGTH);

    ulRslt = (VOS_UINT32)DRV_MEM_VERCTRL((signed char *)acVersionBuffer,
                    VER_MAX_LENGTH, VER_PS, VERIONWRITEMODE); /* 写入协议栈版本信息 */
    if( VOS_OK != ulRslt )
    {
        MMA_WARNINGLOG("MMA_WriteVersion():WARNING:Write PS Version Info fail.");
    }
}


/* 函数WuepsMmaPidInit移动位置 */


/* 函数Mma_TimeOutProc删除 */


/* 删除Mma_MsgProc函数，移到新文件中 */

/* 删除 MMA_UsimAccessCnfMsgProc接口函数，因为 PS_USIM_GENERIC_ACCESS_CNF 不会上报 MMA 模块处理 */

VOS_UINT32 MMA_USIMMaxRecordNumCnfProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucCardType;
    PS_USIM_EFMAX_CNF_STRU             *pMsg;

    pMsg = (PS_USIM_EFMAX_CNF_STRU*)pstMsg;

    MMA_StopInternalTimer();
    MMA_PhoneGetUsimType(&ucCardType);

    if(MMA_USIM_PNN_FILE_INDEX == pMsg->usEfId)
    {
        TAF_PH_USIM_PNN_CNF_STRU stPNN;

        if(MMA_SUCCESS != pMsg->ulResult)
        {
            MMA_ReportParaQueryError(gstMmaValue.stPNN.ClientId, gstMmaValue.stPNN.OpId,\
                                TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_PNN_PARA);
        }
        else
        {
            stPNN.RecordLen         = pMsg->ucRecordLen;
            stPNN.TotalRecordNum    = pMsg->ucRecordNum;
            stPNN.CardType          = ucCardType;

            MMA_ParaQueryReport(gstMmaValue.stPNN.ClientId, gstMmaValue.stPNN.OpId,\
                                TAF_PH_PNN_PARA, TAF_ERR_NO_ERROR, (VOS_VOID*)(&stPNN));
        }
    }
    else if(MMA_USIM_OPL_FILE_INDEX == pMsg->usEfId)
    {
        TAF_PH_USIM_OPL_CNF_STRU stOPL;

        if(MMA_SUCCESS != pMsg->ulResult)
        {
            MMA_ReportParaQueryError(gstMmaValue.stOPL.ClientId, gstMmaValue.stOPL.OpId,\
                                TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_OPL_PARA);
        }
        else
        {
            stOPL.RecordLen         = pMsg->ucRecordLen;
            stOPL.TotalRecordNum    = pMsg->ucRecordNum;
            stOPL.CardType          = ucCardType;

            MMA_ParaQueryReport(gstMmaValue.stOPL.ClientId, gstMmaValue.stOPL.OpId,\
                                TAF_PH_OPL_PARA, TAF_ERR_NO_ERROR, (VOS_VOID*)(&stOPL));
        }
    }
    else
    {
        MMA_ERRORLOG("MMA_UsimMsg:EF ID is unknown.\n");
    }

    return VOS_TRUE;
}



VOS_UINT32 MMA_UsimPINOperateMsgProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    PS_USIM_PIN_CNF_STRU               *pstUsimPinMsg;
    TAF_PH_PIN_CNF_STRU                 stPinTmp;

    pstUsimPinMsg = (PS_USIM_PIN_CNF_STRU*)pstMsg;

    stPinTmp.CmdType      = (VOS_UINT8)pstUsimPinMsg->enCmdType;
    stPinTmp.OpPinResult  = (VOS_UINT8)pstUsimPinMsg->ulResult;
    stPinTmp.PinType      = (VOS_UINT8)pstUsimPinMsg->enPinType;
    NAS_USIMMAPI_GetPinRemainTime((USIMM_PIN_REMAIN_STRU *)(&(stPinTmp.RemainTime)));
    MMA_PinDataReport((const TAF_PH_PIN_CNF_STRU *)&(stPinTmp));

    return VOS_TRUE;
}



VOS_VOID MMA_UsimGetPNNFileRspForCPNN(PS_USIM_GET_FILE_CNF_STRU  * pUsimTafMsg)
{
    VOS_UINT32                  ulTotalNum;
    VOS_UINT32                  i;
    VOS_UINT8                   ucEfLen;

    if ( MMA_SUCCESS != pUsimTafMsg->ulResult )
    {
        MMA_StopInternalTimer();

        MMA_ReportParaQueryError(gstMmaValue.stCPNN.ClientId, gstMmaValue.stCPNN.OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_CPNN_PARA);
        return;
    }

    ulTotalNum = (VOS_UINT32)pUsimTafMsg->ucTotalNum;
    ucEfLen    = (VOS_UINT8)pUsimTafMsg->usEfLen;

    gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf
                = (TAF_PH_USIM_PNN_CNF_STRU *)VOS_MemAlloc(WUEPS_PID_MMA, DYNAMIC_MEM_PT,
                    sizeof(TAF_PH_USIM_PNN_CNF_STRU)+(sizeof(TAF_PH_USIM_PNN_RECORD)*ulTotalNum));

    if ( VOS_NULL_PTR == gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf )
    {
        MMA_ReportParaQueryError(gstMmaValue.stPNN.ClientId, gstMmaValue.stPNN.OpId,
                            TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_CPNN_PARA);

        MMA_StopInternalTimer();

        return;
    }

    PS_MEM_SET(gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf, 0,sizeof(TAF_PH_USIM_PNN_CNF_STRU)+(sizeof(TAF_PH_USIM_PNN_RECORD)*ulTotalNum));

    gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf->TotalRecordNum = ulTotalNum;
    gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf->RecordLen      = ucEfLen;
    for ( i = 0; i < ulTotalNum ; i++)
    {
        PS_MEM_CPY(gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf->PNNRecord[i].PNN, pUsimTafMsg->aucEf+(ucEfLen*i), ucEfLen);
    }

    gstMmaValue.stCPNN.ulPNNGetRslt = VOS_TRUE;

}


VOS_VOID MMA_UsimGetOPLFileRspForCPNN(PS_USIM_GET_FILE_CNF_STRU *pUsimTafMsg)
{
    VOS_UINT32                  ulTotalNum;
    VOS_UINT32                  i;
    VOS_UINT8                   ucEfLen;
    VOS_UINT8                   *pucContent;

    if ( VOS_FALSE == gstMmaValue.stCPNN.ulPNNGetRslt )
    {
        return;
    }

    MMA_StopInternalTimer();

    if ( MMA_SUCCESS != pUsimTafMsg->ulResult )
    {
        MMA_ReportParaQueryError(gstMmaValue.stCPNN.ClientId, gstMmaValue.stCPNN.OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_CPNN_PARA);
        PS_MEM_FREE(WUEPS_PID_MMA, gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf);

        gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf = VOS_NULL_PTR;

        gstMmaValue.stCPNN.ulPNNGetRslt = VOS_FALSE;
        return;

    }

    ulTotalNum = pUsimTafMsg->ucTotalNum;
    ucEfLen    = (VOS_UINT8)pUsimTafMsg->usEfLen;
    pucContent = pUsimTafMsg->aucEf;

    gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf
           = (TAF_PH_USIM_OPL_CNF_STRU *)VOS_MemAlloc(WUEPS_PID_MMA, DYNAMIC_MEM_PT,
                sizeof(TAF_PH_USIM_OPL_CNF_STRU)+(sizeof(TAF_PH_USIM_OPL_RECORD)*ulTotalNum));

    if ( VOS_NULL_PTR == gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf )
    {
        MMA_ReportParaQueryError(gstMmaValue.stCPNN.ClientId, gstMmaValue.stCPNN.OpId,
                            TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_CPNN_PARA);

        PS_MEM_FREE(WUEPS_PID_MMA, gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf);

        gstMmaValue.stCPNN.stCpnnCnf.pstPnnCnf = VOS_NULL_PTR;

        gstMmaValue.stCPNN.ulPNNGetRslt = VOS_FALSE;

        return;
    }
    VOS_MemSet(gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf, 0, sizeof(TAF_PH_USIM_OPL_CNF_STRU)+(sizeof(TAF_PH_USIM_OPL_RECORD)*ulTotalNum));

    gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->RecordLen      = ucEfLen;
    gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->TotalRecordNum = ulTotalNum;

    for(i = 0 ; i < ulTotalNum ; i++ )
    {
        /*重新组合MCC和MNC*/
        gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PLMN[0]  =  (pucContent[0]&0x0F);
        gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PLMN[1]  =  ((pucContent[0]&0xF0)>>4);
        gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PLMN[2]  =  (pucContent[1]&0x0F);
        gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PLMN[3]  =  (pucContent[2]&0x0F);
        gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PLMN[4]  =  ((pucContent[2]&0xF0)>>4);
        gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PLMN[5]  =  ((pucContent[1]&0xF0)>>4);
        gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PlmnLen  =  6;
        gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].LACLow   =  (*(pucContent+3)<<8)+(*(pucContent+4));
        gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].LACHigh  =  (*(pucContent+5)<<8)+(*(pucContent+6));
        gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PNNIndex =  pucContent[7];

        if((0xF >= gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PLMN[5])
          &&(0xA <= gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PLMN[5]))
        {
            gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PlmnLen = 5;
            gstMmaValue.stCPNN.stCpnnCnf.pstOplCnf->OPLRecord[i].PLMN[5] = 0xF;
        }
        pucContent += ucEfLen;
    }

    MMA_CPnnReport();

}
VOS_VOID MMA_ProcRecRange(PS_USIM_GET_FILE_CNF_STRU *pUsimTafMsg)
{
    USIMM_APP_TYPE_ENUM_UINT32          ulNextType;
    VOS_UINT16                          ClientId;
    VOS_UINT8                           OpId;
    TAF_PARA_TYPE                       RptType;
    static TAF_PH_QRY_USIM_RANGE_INFO_STRU         f_stQryRangeInfo;
    USIMM_GET_FILE_INFO_STRU                                stGetFileInfo;

    if (MMA_GET_REC_II == f_stRecInfo.enNextPhase)
    {
        PS_MEM_SET(&f_stQryRangeInfo,0,sizeof(f_stQryRangeInfo));
    }

    if (USIMM_UMTS_APP == f_stRecInfo.enAppType)
    {
        if (MMA_SUCCESS == pUsimTafMsg->ulResult)
        {
            f_stQryRangeInfo.stUsimInfo.bFileExist = VOS_TRUE;
            f_stQryRangeInfo.stUsimInfo.Icctype = TAF_PH_ICC_USIM;
            f_stQryRangeInfo.stUsimInfo.ulTotalRecNum = pUsimTafMsg->ucTotalNum;
            f_stQryRangeInfo.stUsimInfo.ulRecordLen = pUsimTafMsg->usEfLen;
        }
        else
        {
            f_stQryRangeInfo.stUsimInfo.bFileExist = VOS_FALSE;
        }
        ulNextType = USIMM_GSM_APP;
    }
    else
    {
        if (MMA_SUCCESS == pUsimTafMsg->ulResult)
        {
            f_stQryRangeInfo.stSimInfo.bFileExist = VOS_TRUE;
            f_stQryRangeInfo.stSimInfo.Icctype = TAF_PH_ICC_SIM;
            f_stQryRangeInfo.stSimInfo.ulTotalRecNum = pUsimTafMsg->ucTotalNum;
            f_stQryRangeInfo.stSimInfo.ulRecordLen = pUsimTafMsg->usEfLen;
        }
        else
        {
            f_stQryRangeInfo.stSimInfo.bFileExist = VOS_FALSE;
        }

        ulNextType = USIMM_UMTS_APP;
    }

    if (MMA_USIM_PNN_FILE_INDEX == pUsimTafMsg->usEfId)
    {
        ClientId = gstMmaValue.stPNN.ClientId;
        OpId = gstMmaValue.stPNN.OpId;
        RptType = TAF_PH_PNN_RANGE_PARA;
    }
    else
    {
        ClientId = gstMmaValue.stOPL.ClientId;
        OpId = gstMmaValue.stOPL.OpId;
        RptType = TAF_PH_OPL_RANGE_PARA;
    }


    if (MMA_GET_REC_II == f_stRecInfo.enNextPhase)
    {
        f_stRecInfo.enNextPhase = MMA_GET_REC_END;
        f_stRecInfo.enAppType   = ulNextType;

        TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                        ulNextType,
                                        pUsimTafMsg->usEfId,
                                        0);

        if( USIMM_API_SUCCESS != NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA, 0, &stGetFileInfo))

        {
            MMA_WARNINGLOG("MMA_ProcRecRange():WARNING:get File Error!");
            MMA_ParaQueryReport(ClientId,OpId,RptType,TAF_ERR_NO_ERROR,&f_stQryRangeInfo);
            return;
        }

        /*启动定时器*/
        if(MMA_SUCCESS != MMA_StartInternalTimer(1,pUsimTafMsg->usEfId,ClientId,OpId))
        {
            MMA_WARNINGLOG("MMA_ProcRecRange():WARNING:Unknow Error!");
            MMA_ParaQueryReport(ClientId,OpId,RptType,TAF_ERR_NO_ERROR,&f_stQryRangeInfo);
            return;
        }
    }
    else
    {
        /*需要上报 */
        MMA_ParaQueryReport(ClientId,OpId,RptType,TAF_ERR_NO_ERROR,&f_stQryRangeInfo);
        f_bGetRecRange = VOS_FALSE;
        PS_MEM_SET(&f_stQryRangeInfo,0x00,sizeof(f_stQryRangeInfo));
    }
}


VOS_VOID MMA_UsimGetSimSpnInfo()
{
    VOS_UINT32                          ulRet;

    USIMM_GET_FILE_INFO_STRU            stGetFileInfo;

    if (TAF_PH_USIM_SPN == f_SpnCurrenReadType)
    {
        f_SpnCurrenReadType = TAF_PH_GSM_SPN;

        TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                        USIMM_GSM_APP,
                                        TAF_USIM_SPN_EFID,
                                        0);

        ulRet = NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA, 0, &stGetFileInfo);


        if (USIMM_API_SUCCESS != ulRet)
        {
            MMA_ERRORLOG("MMA_UsimGetSimSpnInfo():ERROR:Get File Req failed");
            f_SpnCurrenReadType = TAF_PH_SPN_TYPE_UNKNOW;
            return ;
        }
        /*启动定时器*/
        if( VOS_OK != NAS_StartRelTimer( &g_MmaSimTimer[1].MmaHTimer,
                                          WUEPS_PID_MMA,
                                          MMA_GET_FILE_TIME,
                                          TAF_MMA,
                                          TAF_USIM_SPN,
                                          VOS_RELTIMER_NOLOOP )
                                          )
        {
            MMA_ERRORLOG("MMA_UsimGetSimSpnInfo():ERROR:VOS_StartRelTimer runs failed!");
            g_MmaSimTimer[1].ucTimerStatus = MMA_TIMER_ERROR;
        }
        else
        {
            g_MmaSimTimer[1].ucTimerStatus = MMA_TIMER_RUN;
        }
    }
    else
    {
        f_SpnCurrenReadType = TAF_PH_SPN_TYPE_UNKNOW;
    }

}
VOS_VOID TAF_MMA_UsimGetFileRspPnnMsgProc(PS_USIM_GET_FILE_CNF_STRU *pUsimTafMsg)
{
    VOS_UINT32                  ulTotalNum;
    VOS_UINT8                   ucEfLen;
    TAF_PH_USIM_PNN_CNF_STRU   *pstPNN = VOS_NULL_PTR;
    VOS_UINT32                  i;

    ulTotalNum = 0;
    ucEfLen    = 0;
    i          = 0;

    MMA_StopInternalTimer();
    if (VOS_TRUE == f_bGetRecRange)
    {
        MMA_ProcRecRange(pUsimTafMsg);
        return;
    }

    /*如果读取文件失败*/
    if(MMA_SUCCESS != pUsimTafMsg->ulResult)
    {
       MMA_ReportParaQueryError(gstMmaValue.stPNN.ClientId, gstMmaValue.stPNN.OpId,
                                TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_PNN_PARA);
    }
    else
    {
        ulTotalNum = pUsimTafMsg->ucTotalNum;
        ucEfLen    = (VOS_UINT8)pUsimTafMsg->usEfLen;


        pstPNN = (TAF_PH_USIM_PNN_CNF_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA,
                  sizeof(TAF_PH_USIM_PNN_CNF_STRU)+(sizeof(TAF_PH_USIM_PNN_RECORD)*ulTotalNum));

        if(VOS_NULL_PTR == pstPNN)
        {
            MMA_ReportParaQueryError(gstMmaValue.stPNN.ClientId, gstMmaValue.stPNN.OpId,
                                TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_PNN_PARA);
            return;
        }

        MMA_MEM_SET(pstPNN, 0, sizeof(TAF_PH_USIM_PNN_CNF_STRU)+(sizeof(TAF_PH_USIM_PNN_RECORD)*ulTotalNum));

        pstPNN->TotalRecordNum = ulTotalNum;
        pstPNN->RecordLen      = ucEfLen;
        for( i = 0; i< ulTotalNum ; i++)
        {
            VOS_MemCpy(pstPNN->PNNRecord[i].PNN, pUsimTafMsg->aucEf+(ucEfLen*i), ucEfLen);
        }

        /* 如果是要求读取PNN列表，则按照分段发送的形式进行发送 */
        if (VOS_TRUE == gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.ucAtPnnWaitCardCnfFlg)
        {
            gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.pucPnnData            = (VOS_UINT8*)pstPNN;
            gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.usEfLen               = ucEfLen;
            gstMmaValue.stPNN.stPnnListInfo.stPnnListDataInfo.usSumPnnNumInSim      = (VOS_UINT16)ulTotalNum;
            /* 卡回复了AT^PNN的查询后就将该标示置为false，
                防止卡的refresh消息更新已经在分段上报的PNN列表 */
            gstMmaValue.stPNN.stPnnListInfo.stPnnListCtrInfo.ucAtPnnWaitCardCnfFlg  = VOS_FALSE;
            TAF_MMA_AtQryPnnListProc();
        }
        else
        {
            MMA_ParaQueryReport(gstMmaValue.stPNN.ClientId, gstMmaValue.stPNN.OpId,
                            TAF_PH_PNN_PARA, TAF_ERR_NO_ERROR, pstPNN);

            PS_MEM_FREE(WUEPS_PID_MMA, pstPNN);
        }
     }
}



VOS_UINT32 MMA_UsimGetFileRspMsgProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                  ulTotalNum;
    VOS_UINT32                  ulRet;
    VOS_UINT32                  i;
    VOS_UINT8                   ucEfLen;
    VOS_UINT8                   *pucContent;
    TAF_PH_USIM_SPN_CNF_STRU    stSpnTmp;
    TAF_PH_USIM_OPL_CNF_STRU    *pstOPL;
    VOS_UINT8                   ucCardType;

    PS_USIM_GET_FILE_CNF_STRU          *pUsimTafMsg;

    pUsimTafMsg = (PS_USIM_GET_FILE_CNF_STRU*)pstMsg;

    switch ( pUsimTafMsg->usEfId )
    {
        case TAF_USIM_SPN_EFID :


            ulRet = NAS_StopRelTimer(WUEPS_PID_MMA, TAF_USIM_SPN,
                                     &g_MmaSimTimer[1].MmaHTimer);


            if ( VOS_OK != ulRet )
            {
                MMA_ERRORLOG ("MMA_UsimMsgProc():ERROR:STOP SPN TIMER ERROR!");
            }
            g_MmaSimTimer[1].ucTimerStatus = MMA_TIMER_STOP;
            if (MMA_SUCCESS == pUsimTafMsg->ulResult)
            {
                if (pUsimTafMsg->usEfLen > sizeof(TAF_PH_USIM_SPN_CNF_STRU))
                {
                    MMA_MEM_CPY(&stSpnTmp, (VOS_UINT8*)(VOS_UINT32)(pUsimTafMsg->aucEf), sizeof(TAF_PH_USIM_SPN_CNF_STRU));
                }
                else
                {
                    MMA_MEM_CPY(&stSpnTmp, pUsimTafMsg->aucEf, pUsimTafMsg->usEfLen);
                }

                MMA_ReadSpnFileCnf((TAF_PH_USIM_SPN_CNF_STRU *)&stSpnTmp,f_SpnCurrenReadType);
            }
            MMA_UsimGetSimSpnInfo();
            break;
         case TAF_CPHS_SIM_ONS_EFID :

            ulRet = NAS_StopRelTimer(WUEPS_PID_MMA, TI_MN_PH_CPHS_SIM_ONS,
                                     &g_MmaSimTimer[MN_PH_SIM_TIMER_ENUM_CPHS_ONS].MmaHTimer);

            g_MmaSimTimer[MN_PH_SIM_TIMER_ENUM_CPHS_ONS].ucTimerStatus = MMA_TIMER_STOP;
            MMA_ReadCPHSOperNameStrFileCnf(pUsimTafMsg);
            break;
        case MMA_USIM_ICC_ID_INDEX :
            MMA_MEM_SET(&(gstMmaValue.stGetIccId.stIccId), 0, sizeof(TAF_PH_ICC_ID_STRU));
            MMA_MEM_CPY(gstMmaValue.stGetIccId.stIccId.aucIccId,
                        pUsimTafMsg->aucEf,
                        pUsimTafMsg->usEfLen);
            gstMmaValue.stGetIccId.stIccId.ucLen = (VOS_UINT8)pUsimTafMsg->usEfLen;
            MMA_GetUsimIccIdFileInd();
            break;
        case MMA_USIM_IMSI_FILE_ID:

            ulRet = MMA_CheckPin1SatusSimple();

            if ( MMA_NONEED_PIN1 == ulRet )
            {
                if (pUsimTafMsg->usEfLen< 10 )
                {
                    MMA_MEM_CPY(TAF_SDC_GetSimImsi(), pUsimTafMsg->aucEf, pUsimTafMsg->usEfLen);

                    /* 检查是否锁卡 */
                    MMA_CheckMePersonalisationStatus();
                }
            }
            break;
        case MMA_USIM_PNN_FILE_INDEX:

            if ( MMA_CPNN_WAIT_PNN_FLAG == (gstMmaValue.stCPNN.ucWaitPnnOPLMask & MMA_CPNN_WAIT_PNN_FLAG ))
            {
                MMA_UsimGetPNNFileRspForCPNN(pUsimTafMsg);
                gstMmaValue.stCPNN.ucWaitPnnOPLMask &=  ~MMA_CPNN_WAIT_PNN_FLAG;
            }
            else
            {
                /* 将对PNN LIST的处理封装成函数 */
                TAF_MMA_UsimGetFileRspPnnMsgProc(pUsimTafMsg);
            }
            break;
        case MMA_USIM_OPL_FILE_INDEX:
            if ( MMA_CPNN_WAIT_OPL_FLAG == (gstMmaValue.stCPNN.ucWaitPnnOPLMask & MMA_CPNN_WAIT_OPL_FLAG ) )
            {
                MMA_UsimGetOPLFileRspForCPNN(pUsimTafMsg);
                gstMmaValue.stCPNN.ucWaitPnnOPLMask &=  ~MMA_CPNN_WAIT_OPL_FLAG;
            }
            else
            {
                MMA_StopInternalTimer();
                if (VOS_TRUE == f_bGetRecRange)
                {
                    MMA_ProcRecRange(pUsimTafMsg);

                    return VOS_TRUE;
                }
                /*如果读取文件失败*/
                if(MMA_SUCCESS != pUsimTafMsg->ulResult )
                {
                   MMA_ReportParaQueryError(gstMmaValue.stOPL.ClientId, gstMmaValue.stOPL.OpId,
                                            TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_OPL_PARA);
                }
                else
                {
                    ulTotalNum = pUsimTafMsg->ucTotalNum;
                    ucEfLen    = (VOS_UINT8)pUsimTafMsg->usEfLen;
                    pucContent = pUsimTafMsg->aucEf;

                    pstOPL   = (TAF_PH_USIM_OPL_CNF_STRU *)VOS_MemAlloc(WUEPS_PID_MMA, DYNAMIC_MEM_PT,
                                sizeof(TAF_PH_USIM_OPL_CNF_STRU)+(sizeof(TAF_PH_USIM_OPL_RECORD)*ulTotalNum));

                    if(VOS_NULL_PTR == pstOPL)
                    {
                        MMA_ReportParaQueryError(gstMmaValue.stOPL.ClientId, gstMmaValue.stOPL.OpId,
                                            TAF_ERR_UNSPECIFIED_ERROR, TAF_PH_OPL_PARA);

                        return VOS_TRUE;
                    }

                    MMA_MEM_SET(pstOPL, 0, sizeof(TAF_PH_USIM_OPL_CNF_STRU)+(sizeof(TAF_PH_USIM_OPL_RECORD)*ulTotalNum));

                    pstOPL->TotalRecordNum = ulTotalNum;
                    pstOPL->RecordLen      = ucEfLen;
                    if (VOS_TRUE == f_bGetRecRange)
                    {
                        f_bGetRecRange = VOS_FALSE;
                        MMA_PhoneGetUsimType(&ucCardType);
                        pstOPL->CardType          = ucCardType;
                        pstOPL->OPLRecord[0].PlmnLen = 0;
                    }
                    else
                    {
                        for(i = 0 ; i < ulTotalNum ; i++ )
                        {
                            /*重新组合MCC和MNC*/
                            pstOPL->OPLRecord[i].PLMN[0]  =  (pucContent[0]&0x0F);
                            pstOPL->OPLRecord[i].PLMN[1]  =  ((pucContent[0]&0xF0)>>4);
                            pstOPL->OPLRecord[i].PLMN[2]  =  (pucContent[1]&0x0F);
                            pstOPL->OPLRecord[i].PLMN[3]  =  (pucContent[2]&0x0F);
                            pstOPL->OPLRecord[i].PLMN[4]  =  ((pucContent[2]&0xF0)>>4);
                            pstOPL->OPLRecord[i].PLMN[5]  =  ((pucContent[1]&0xF0)>>4);
                            pstOPL->OPLRecord[i].PlmnLen  =  6;
                            pstOPL->OPLRecord[i].LACLow   =  (*(pucContent+3)<<8)+(*(pucContent+4));
                            pstOPL->OPLRecord[i].LACHigh  =  (*(pucContent+5)<<8)+(*(pucContent+6));
                            pstOPL->OPLRecord[i].PNNIndex =  pucContent[7];
                            if((0xF >= pstOPL->OPLRecord[i].PLMN[5])
                              &&(0xA <= pstOPL->OPLRecord[i].PLMN[5]))
                            {
                                pstOPL->OPLRecord[i].PlmnLen = 5;
                                pstOPL->OPLRecord[i].PLMN[5] = 0xF;
                            }

                            pucContent += ucEfLen;
                        }
                    }
                    MMA_ParaQueryReport(gstMmaValue.stOPL.ClientId, gstMmaValue.stOPL.OpId,
                                    TAF_PH_OPL_PARA, TAF_ERR_NO_ERROR, pstOPL);

                    PS_MEM_FREE(WUEPS_PID_MMA, pstOPL);
                }
            }

            break;

        case MMA_MNC_LENGTH_FILE_ID :
            if (VOS_TRUE == gstMmaValue.stGetHplmn.ucHplmnQryFlag)
            {
                TAF_MMA_GetUsimHplmnMncLenFileInd(pUsimTafMsg);
                gstMmaValue.stGetHplmn.ucHplmnQryFlag = VOS_FALSE;
            }
            break;

        case TAF_MMA_CUSTOMER_SERVICE_PROFILE_FILE_ID:
            TAF_MMA_RcvCustomerServiceProfileFile(pUsimTafMsg->ulResult,
                                                  pUsimTafMsg->usEfLen,
                                                  pUsimTafMsg->aucEf);
            break;

        default:
            MMA_WARNINGLOG("MMA_UsimGetFileRspMsgProc():WARNING:UNKNOW EFID RECEIVE!");
            break;
    }

    return VOS_TRUE;
}



VOS_UINT32 MMA_UsimSetFileCnfMsgProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    PS_USIM_SET_FILE_CNF_STRU          *pUsimSetFileCnf;

    pUsimSetFileCnf = (PS_USIM_SET_FILE_CNF_STRU*)pstMsg;

    if ((EFPLMNsel == pUsimSetFileCnf->usEfId)
     || (EFPLMNwAcT == pUsimSetFileCnf->usEfId)
     || (EFOPLMNwAcT == pUsimSetFileCnf->usEfId)
     || (EFHPLMNwAcT== pUsimSetFileCnf->usEfId))
    {
        MMA_NORMAILLOG("MMA_UsimMsgProc():NORMAL:set File preferred PLMN list Finished!");
        MMA_SetPrefPlmnCnf(pUsimSetFileCnf);
    }
    else
    {
        MMA_WARNINGLOG1("MMA_UsimMsgProc():WARNING:Rev RSP Efid Wrong,Efid = !",pUsimSetFileCnf->usEfId);
    }

    return VOS_TRUE;
}



VOS_VOID TAF_MMA_UsimRefreshIndAllFileChangedMsgProc(PS_USIM_REFRESH_IND_STRU *pUsimTafMsg)
{
    /* 目前只更新SPN的文件。 */
    VOS_UINT32   ulSpnExistFlag = 0;
    VOS_UINT8    ucSimType;
    VOS_UINT32   ulRet;

    USIMM_GET_FILE_INFO_STRU            stGetFileInfo;
    /*从USIM中获取SPN文件,文件ID:0x6F46*/
    ulSpnExistFlag = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_SPN);


   if ( PS_USIM_SERVICE_AVAILIABLE == ulSpnExistFlag )
   {
       MMA_PhoneGetUsimType(&ucSimType);
       if (MMA_USIM_TYPE_USIM == ucSimType)
       {
           TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                USIMM_UMTS_APP,
                                TAF_USIM_SPN_EFID,
                                0);

            ulRet = NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA, 0, &stGetFileInfo);

            if (USIMM_API_SUCCESS != ulRet)
            {
                MMA_ERRORLOG("MMA_UsimRefreshIndMsgProc():ERROR:Get File Req failed");
            }
            f_SpnCurrenReadType = TAF_PH_USIM_SPN;
       }
       else
       {
          TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                USIMM_GSM_APP,
                                TAF_USIM_SPN_EFID,
                                0);

            ulRet = NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA, 0, &stGetFileInfo);

            if (USIMM_API_SUCCESS != ulRet)
            {
                MMA_ERRORLOG("MMA_UsimRefreshIndMsgProc():ERROR:Get File Req failed");
            }

            f_SpnCurrenReadType = TAF_PH_GSM_SPN;
       }

       /*启动定时器*/
       if( VOS_OK != NAS_StartRelTimer( &g_MmaSimTimer[1].MmaHTimer,
                                         WUEPS_PID_MMA,
                                         MMA_GET_FILE_TIME,
                                         TAF_MMA,
                                         TAF_USIM_SPN,
                                         VOS_RELTIMER_NOLOOP )
                                         )
       {
           MMA_ERRORLOG("MMA_UsimRefreshIndMsgProc():ERROR:VOS_StartRelTimer runs failed!");
           g_MmaSimTimer[1].ucTimerStatus = MMA_TIMER_ERROR;
       }
       else
       {
           g_MmaSimTimer[1].ucTimerStatus = MMA_TIMER_RUN;
       }

   }
   else
   {
       gstMmaValue.stUsimInfo.UsimSpnInfo.SpnType     = TAF_PH_SPN_TYPE_UNKNOW;
       gstMmaValue.stUsimInfo.SimSpnInfo.SpnType      = TAF_PH_SPN_TYPE_UNKNOW;
   }

}
VOS_VOID MMA_USIMRefreshFileProc(VOS_UINT16 usEfId, VOS_UINT16 usEfLen, VOS_UINT8 *pucEf)
{
    PS_USIM_GET_FILE_CNF_STRU           *pUsimTafMsg;

    switch (usEfId)
    {
        case TAF_CPHS_SIM_ONS_EFID:
            pUsimTafMsg = (PS_USIM_GET_FILE_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, (VOS_UINT32)(sizeof(PS_USIM_GET_FILE_CNF_STRU) - 4 + usEfLen));

            if (VOS_NULL_PTR == pUsimTafMsg)
            {
                MMA_ERRORLOG("MMA_USIMReadRefreshFileProc():ERROR:MALLOC FAIL");
                return;
            }

            PS_MEM_SET(pUsimTafMsg, 0, (VOS_UINT32)(sizeof(PS_USIM_GET_FILE_CNF_STRU) - 4 + usEfLen));

           pUsimTafMsg->ulResult = MMA_SUCCESS;
           pUsimTafMsg->usEfLen  = usEfLen;
           PS_MEM_CPY(pUsimTafMsg->aucEf, pucEf, usEfLen);

           MMA_ReadCPHSOperNameStrFileCnf(pUsimTafMsg);

           PS_MEM_FREE(WUEPS_PID_MMA, pUsimTafMsg);
           break;
        case MMA_USIM_ICC_ID_INDEX:
            MMA_MEM_SET(&(gstMmaValue.stGetIccId.stIccId), 0, sizeof(TAF_PH_ICC_ID_STRU));
            MMA_MEM_CPY(gstMmaValue.stGetIccId.stIccId.aucIccId,
                        pucEf,
                        usEfLen);
            gstMmaValue.stGetIccId.stIccId.ucLen = (VOS_UINT8)usEfLen;
            MMA_GetUsimIccIdFileInd();
            break;
        case MMA_USIM_IMSI_FILE_ID:
            MMA_WARNINGLOG("MMA_USIMReadRefreshFileProc():WARNING:Refresh File Id IMSI");
            break;
        case MMA_USIM_PNN_FILE_INDEX:
            MMA_INFOLOG("MMA_USIMReadRefreshFileProc():INFO:Refresh File PNN");
            break;
        case MMA_USIM_OPL_FILE_INDEX:
            MMA_INFOLOG("MMA_USIMReadRefreshFileProc():INFO:Refresh File OPL");
            break;

        case TAF_MMA_CUSTOMER_SERVICE_PROFILE_FILE_ID:
            TAF_MMA_RcvCustomerServiceProfileFile(VOS_OK, usEfLen, pucEf);
            break;

        case TAF_MMA_RAT_MODE_FILE_ID:
            TAF_MMA_RcvRatModeFileRefresh(usEfLen, pucEf);
            break;

        default:
            MMA_WARNINGLOG("MMA_USIMReadRefreshFileProc():WARNING:UNKNOW EFID RECEIVE!");
            break;
    }

}


VOS_VOID TAF_MMA_UsimRefreshIndFileListChangedMsgProc(PS_USIM_REFRESH_IND_STRU *pUsimTafMsg)
{
    VOS_UINT16                          usEfId;
    VOS_UINT32                          ulEfLen;
    VOS_UINT8                          *pucEf;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          i;
    VOS_UINT8                           ucSimType;
    USIMM_GET_FILE_INFO_STRU            stGetFileInfo;

    pucEf = VOS_NULL_PTR;

    usEfId    = 0;
    ulEfLen   = 0;
    ucSimType = MMA_USIM_TYPE_USIM;

    /*依次读取上报的refresh文件*/
    for (i = 0; i < pUsimTafMsg->usEfNum; i++)
    {
        usEfId = pUsimTafMsg->astEfId[i].usFileId;

        if(TAF_USIM_SPN_EFID == usEfId)
        {
            MMA_PhoneGetUsimType(&ucSimType);

            if (MMA_USIM_TYPE_USIM == ucSimType)
            {
                TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                                USIMM_UMTS_APP,
                                                TAF_USIM_SPN_EFID,
                                                0);

                ulResult = NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA, 0, &stGetFileInfo);
            }
            else
            {
                TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                                USIMM_GSM_APP,
                                                TAF_USIM_SPN_EFID,
                                                0);

                ulResult = NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA, 0, &stGetFileInfo);
            }

            if (USIMM_API_SUCCESS != ulResult)
            {
                /*读当前文件失败，读取下一个文件*/
                MMA_ERRORLOG("MMA_UsimRefreshFileIndMsgProc():ERROR:READ FILE FAIL!");
            }
        }
        else
        {
            ulResult = NAS_USIMMAPI_GetCachedFile(usEfId, &ulEfLen, &pucEf, pUsimTafMsg->astEfId[i].enAppType);

            if (USIMM_API_SUCCESS != ulResult)
            {
                /*读当前文件失败，读取下一个文件*/
                MMA_ERRORLOG("MMA_UsimRefreshFileIndMsgProc():ERROR:READ FILE FAIL!");
                continue;
            }

            /* 增加可维可测 */
            TAF_MMA_SndOmGetCacheFile(usEfId, ulEfLen, pucEf);


            /*读取文件成功后，MMA内部做相应处理*/
            MMA_USIMRefreshFileProc(usEfId, (VOS_UINT16)ulEfLen, pucEf);
        }
    }

    return;
}


VOS_UINT32 MMA_ProcHotInOutUsimStatusInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucCardStatus;
    VOS_UINT8                           ucSimLockStatus;
    PS_HOT_IN_OUT_USIM_STATUS_IND_STRU *pstUsimMsg = VOS_NULL_PTR;

    pstUsimMsg = (PS_HOT_IN_OUT_USIM_STATUS_IND_STRU*)pstMsg;

    /* 获取锁卡状态 */
    if ( MMA_SIM_IS_LOCK == MMA_GetMeLockStatus() )
    {
        ucSimLockStatus                 = VOS_TRUE;
    }
    else
    {
        ucSimLockStatus                 = VOS_FALSE;
    }

    ucCardStatus                        = USIMM_CARD_SERVIC_BUTT;

    /* 热拨出 */
    if (SIM_CARD_OUT == pstUsimMsg->ulCardInOutStatus)
    {
        /* USIM卡热拔出卡状态上报255 */
        TAF_MMA_SndAtIccStatusInd(TAF_PH_USIM_NON, ucSimLockStatus);

        /* 将USIM热拔出的标识置为TRUE */
        g_ucUsimHotOutFlag = VOS_TRUE;

        g_ucUsimHotInFlag  = VOS_FALSE;

        /* 收到热拔出事件，计数加1 */
        g_ulUsimHotOutCount++;
    }
    /* 热插入 */
    else if (SIM_CARD_IN == pstUsimMsg->ulCardInOutStatus)
    {
        g_ucUsimHotOutFlag = VOS_FALSE;

        (VOS_VOID)NAS_USIMMAPI_GetCardType(&ucCardStatus, VOS_NULL_PTR);

        /* 只有在卡被拔出的情况，收到卡插入才有效 */
        if (USIMM_CARD_SERVIC_ABSENT == ucCardStatus)
        {
            TAF_MMA_SndAtIccStatusInd(TAF_PH_USIM_STATUS_UNAVAIL, ucSimLockStatus);

            /* 将USIM热插入的标识置为TRUE */
            g_ucUsimHotInFlag  = VOS_TRUE;
        }

        /* 收到热插入事件，计数加1 */
        g_ulUsimHotInCount++;
    }
    else
    {
        /* for pclint */
    }

    return VOS_TRUE;
}
VOS_UINT32 MMA_UsimRestrictedAccessCnfMsgProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PHONE_EVENT_INFO_STRU   *pstPhoneEvent;

    PS_USIM_RESTRICTED_ACCESS_CNF_STRU *pMsg;

    pMsg = (PS_USIM_RESTRICTED_ACCESS_CNF_STRU*)pstMsg;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MMA_UsimRestrictedAccessCnfMsgProc:ERROR:ALLOC MEMORY FAIL.");

        return VOS_TRUE;
    }
    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if ((MMA_SUCCESS == pMsg->ulResult)
     && (USIMM_SW_SENDCMD_ERROR != pMsg->ulErrorCode))
    {

        pstPhoneEvent->PhoneEvent = TAF_PH_EVT_RESTRICTED_ACCESS_CNF;
        pstPhoneEvent->OpId = 0;
        pstPhoneEvent->ClientId = (VOS_UINT16)pMsg->ulSendPara;
        pstPhoneEvent->OP_UsimRestrictAccess = 1;
        pstPhoneEvent->RestrictedAccess.ucLen = (VOS_UINT8)pMsg->usLen;
        pstPhoneEvent->RestrictedAccess.ucSW1 = pMsg->ucSW1;
        pstPhoneEvent->RestrictedAccess.ucSW2 = pMsg->ucSW2;
        PS_MEM_CPY(pstPhoneEvent->RestrictedAccess.aucContent,
                  pMsg->aucContent,
                  pMsg->usLen);
    }
    else
    {
        pstPhoneEvent->PhoneEvent    = TAF_PH_EVT_ERR;
        pstPhoneEvent->ClientId      = (VOS_UINT16)pMsg->ulSendPara;
        pstPhoneEvent->OP_PhoneError = 1;
        pstPhoneEvent->PhoneError    = TAF_ERR_SIM_FAIL;
    }

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return VOS_TRUE;
}
VOS_UINT32 MMA_UsimRestrictedAccessCommandProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usClientId;
    VOS_UINT8                           ucOpId;
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent   = VOS_NULL_PTR;
    TAF_SIM_RESTRIC_ACCESS_STRU        *pstCommand      = VOS_NULL_PTR;
    MN_APP_REQ_MSG_STRU                *pstRcvMsg       = VOS_NULL_PTR;

    pstRcvMsg = (MN_APP_REQ_MSG_STRU *)pstMsg;

    pstCommand  = (TAF_SIM_RESTRIC_ACCESS_STRU *)pstRcvMsg->aucContent;
    usClientId  = pstRcvMsg->clientId;
    ucOpId      = pstRcvMsg->opId;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MMA_UsimRestrictedAccessCommandProc:ERROR:ALLOC MEMORY FAIL.");
        return VOS_TRUE;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    ulRslt = USIMM_RestrictedAccessReq( WUEPS_PID_MMA,
                                        usClientId,
                                        pstCommand);

    if (TAF_ERR_NO_ERROR != ulRslt)
    {

        pstPhoneEvent->OP_PhoneError      = 1;
        pstPhoneEvent->PhoneEvent         = TAF_PH_EVT_ERR;
        pstPhoneEvent->ClientId           = usClientId;
        pstPhoneEvent->OpId               = ucOpId;
        pstPhoneEvent->PhoneError         = TAF_ERR_SIM_BUSY;

        /* 调用电话管理上报函数 */
        MN_PH_SendMsg(pstPhoneEvent->ClientId,(VOS_UINT8*)pstPhoneEvent,sizeof(TAF_PHONE_EVENT_INFO_STRU));

    }

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return VOS_TRUE;

}

/* MMA_TafMsgProc 函数删除 */

/* END:   Added by liuyang id:48197, 2006/8/9   PN:优化MMA*/

VOS_VOID  MMA_PhoneGetIccId(VOS_UINT16           ClientId,
                            VOS_UINT8            OpId)
{
    VOS_UINT32 ulRet;
    VOS_UINT8 ucSimStatus;
    USIMM_GET_FILE_INFO_STRU                    stGetFileInfo;
    if(MMA_TIMER_STOP != gstMmaInternalTimer.stTimer.ucTimerStatus)
    {
        MMA_WARNINGLOG("MMA_PhoneGetIccId():WARNING:USIM is not Ready!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 TAF_PH_ICC_ID);
        return;
    }

    /*判断卡状态*/
    ulRet = MMA_PhoneGetUsimStatus(&ucSimStatus );
    if ( (ulRet           != MMA_SUCCESS )
       ||(MMA_USIM_ABSENT == ucSimStatus)
       )
    {
        MMA_WARNINGLOG("MMA_PhoneGetIccId():WARNING:USIM is Error or Absent!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_USIM_SIM_CARD_NOTEXIST,
                                 TAF_PH_ICC_ID);
        return;
    }

    /*启动定时器*/
    if(MMA_SUCCESS != MMA_StartInternalTimer(1,
                                             MMA_GET_USIM_ICC_ID,
                                             ClientId,
                                             OpId))
    {
        MMA_WARNINGLOG("MMA_PhoneGetIccId():WARNING:Unknow Error!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 TAF_PH_ICC_ID);
        return;
    }
    /*读卡*/
    TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                    USIMM_UNLIMIT_APP,
                                    MMA_USIM_ICC_ID_INDEX,
                                    0);

    if( USIMM_API_SUCCESS != NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA,
                                              0,
                                              &stGetFileInfo))
    {
        MMA_WARNINGLOG("MMA_PhoneGetIccId():WARNING:get File Error!");
        MMA_ReportParaQueryError(ClientId, OpId,
                                 TAF_ERR_UNSPECIFIED_ERROR,
                                 TAF_PH_ICC_ID);

        /*停止定时器*/
        MMA_StopInternalTimer();
        return;
    }
    gstMmaValue.stGetIccId.ClientId = ClientId;
    gstMmaValue.stGetIccId.OpId     = OpId;
    return;
}



VOS_VOID MMA_GetUsimIccIdFileInd(VOS_VOID)
{
    MMA_NORMAILLOG("MMA_GetUsimIccIdFileInd():NORMAL:GET ICC ID SUCCESSFUL");
    MMA_ParaQueryReport(gstMmaValue.stGetIccId.ClientId,
                        gstMmaValue.stGetIccId.OpId,
                        TAF_PH_ICC_ID,
                        TAF_ERR_NO_ERROR,
                        (VOS_VOID*)(&gstMmaValue.stGetIccId.stIccId));

    /*停止定时器*/
    MMA_StopInternalTimer();

    return;
}

/*****************************************************************************
 函 数 名  : MMA_GetUsimIccIdExpired
 功能描述  : 等待USIM上报结果超时
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年8月23日
    作    者   : liuyang id:48197
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MMA_GetUsimIccIdExpired(VOS_VOID)
{
    /*获取ICCID操作失败*/
    MMA_WARNINGLOG("MMA_GetUsimIccIdExpired():WARNING:GET USIM ICC ID, Fail");

    MMA_ReportParaQueryError(gstMmaInternalTimer.ClientId,
                             gstMmaInternalTimer.OpId,
                             TAF_ERR_UNSPECIFIED_ERROR,
                             TAF_PH_ICC_ID);

    return;
}




VOS_VOID  Taf_GetSpnName(VOS_UINT8*  aucSpnName,VOS_UINT32 ulMaxLen)
{
    TAF_UINT32              ulDatalen = 0;
    TAF_PH_USIM_SPN_STRU    *pstSpn = VOS_NULL_PTR;

    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode;

    enSysMode = TAF_SDC_GetSysMode();

    PS_MEM_SET(aucSpnName,0x00,ulMaxLen);
    if ((TAF_SDC_SYS_MODE_WCDMA == enSysMode)
     || (TAF_SDC_SYS_MODE_LTE == enSysMode))
    {
        if ( ( TAF_PH_USIM_SPN == gstMmaValue.stUsimInfo.UsimSpnInfo.SpnType )
            && ( 0 == ( (gstMmaValue.stUsimInfo.UsimSpnInfo.DispRplmnMode) & 0x02 )))
        {
            pstSpn = &gstMmaValue.stUsimInfo.UsimSpnInfo;
        }
    }
    else if ( TAF_SDC_SYS_MODE_GSM == enSysMode )
    {

        if  ( ( TAF_PH_GSM_SPN == gstMmaValue.stUsimInfo.SimSpnInfo.SpnType )
        && ( 0 == ( (gstMmaValue.stUsimInfo.SimSpnInfo.DispRplmnMode) & 0x02 )))
        {
            pstSpn = &gstMmaValue.stUsimInfo.SimSpnInfo;
        }

    }
    else
    {

    }


    if  ( (VOS_NULL_PTR == pstSpn) || (0xFF == pstSpn->aucSpnName[0]) )
    {
        return ;
    }

    if ( pstSpn->SpnCoding == TAF_PH_GSM_7BIT_DEFAULT )
    {

        for ( ulDatalen = 0 ; ulDatalen < TAF_PH_SPN_NAME_MAXLEN ;  ulDatalen++ )
        {
             if ( 0xFF == pstSpn->aucSpnName[ulDatalen] )
             {
                break;
             }
        }
        PS_MEM_CPY(aucSpnName,pstSpn->aucSpnName,ulDatalen);
    }
    else
    {
        ulDatalen = TAF_PH_SPN_NAME_MAXLEN;

        At_HexAlpha2AsciiString(ulMaxLen,(TAF_INT8 *)aucSpnName,(TAF_UINT8 *) aucSpnName,
                                            pstSpn->aucSpnName, (TAF_UINT16)ulDatalen);
    }
    return ;
}
TAF_UINT32  Taf_DefGetSPN(TAF_UINT8 ucSpnType, TAF_PH_USIM_SPN_STRU   *pstAtSPNRslt)
{
    if (ucSpnType > 1 )
    {
        return TAF_FAILURE;
    }
    /* 0 表示SIM */
    if (1 == ucSpnType)
    {
        PS_MEM_CPY(pstAtSPNRslt, &gstMmaValue.stUsimInfo.UsimSpnInfo, sizeof(TAF_PH_USIM_SPN_STRU));
    }
    else
    {
        PS_MEM_CPY(pstAtSPNRslt, &gstMmaValue.stUsimInfo.SimSpnInfo, sizeof(TAF_PH_USIM_SPN_STRU));
    }
    return TAF_SUCCESS;
}
VOS_BOOL Taf_IsWBandSupported(VOS_UINT16 usBandNo)
{
    VOS_UINT16    usBandMask;

    /*目前只支持W只能设置支持到Band11*/
    if ((TAF_DEVICE_AT_MAX_W_BAND_NUM < usBandNo)||(0 == usBandNo))
    {
        return VOS_FALSE;
    }
    /* W Rf support
    -------------------------------------------------------------------------------
          bit8       bit7      bit6     bit5    bit4     bit3      bit2     bit1
                                                         bit11     bit10    bit9
    -------------------------------------------------------------------------------
    WCDMA   900(VIII)  2600(VII) 800(VI)  850(V)  1700(IV) 1800(III) 1900(II) 2100(I) oct1
            spare      spare     spare    spare   spare    spare     spare   J1700(IX)oct2
   */
    usBandMask = (VOS_UINT16)(0x0001 << (usBandNo-1));

    if (usBandMask & gstMmaBandCapa.unWRFSptBand.ulBand)
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }

}

TAF_UINT32  TAF_SetPrefPlmnType(
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPlmnType
)
{
    VOS_UINT32                          ulRet;

    if (enPrefPlmnType > MN_PH_PREF_PLMN_HPLMN)
    {
        return TAF_PARA_TYPE_NOT_SUPPORT;
    }

    if (MN_PH_PREF_PLMN_UPLMN == enPrefPlmnType)
    {
        ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_UPLMN_SEL_WACT);

        if (PS_USIM_SERVICE_NOT_AVAILIABLE == ulRet)
        {
            ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_PLMN_SELECTOR);
        }
    }
    else if (MN_PH_PREF_PLMN_OPLMN == enPrefPlmnType)
    {
        ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_OPLMN_SEL_WACT);
    }
    else
    {
        ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_HPLMN_WACT);
    }


    if (PS_USIM_SERVICE_NOT_AVAILIABLE == ulRet)
    {
        return TAF_PARA_TYPE_NOT_SUPPORT;
    }

    return TAF_PARA_OK;
}



TAF_UINT32  Taf_SyscfgBand2Str(TAF_PH_PREF_BAND band,TAF_CHAR* strBand)
{

    VOS_UINT32 i,ulLen;
    VOS_UINT32 ulBandStrLen = 0;
    TAF_CHAR* strSep = "/";

    ulBandStrLen = 0;
    ulLen = sizeof(gastMmaSyscfgBandStr)/sizeof(gastMmaSyscfgBandStr[0]);

    MMA_INFOLOG1("Taf_SyscfgBand2Str:band = \r\n", (VOS_INT)band);

    for (i = 0; i < ulLen; i++)
    {
        if(0 != (band & gastMmaSyscfgBandStr[i].BandMask))
        {
            VOS_StrCpy(strBand + ulBandStrLen, gastMmaSyscfgBandStr[i].BandStr);

            ulBandStrLen += VOS_StrLen(gastMmaSyscfgBandStr[i].BandStr);

            VOS_StrCpy(strBand + ulBandStrLen, strSep);

            ulBandStrLen += VOS_StrLen(strSep);

            if(ulBandStrLen >= (TAF_PH_SYSCFG_GROUP_BAND_LEN/2))
            {
                MMA_ERRORLOG("Taf_SyscfgBand2Str(): ERROR :strBand too long ");
                break;
            }
        }
    }

    if (ulBandStrLen > 0)
    {
        strBand[ulBandStrLen-1] = '\0';
    }

    return TAF_SUCCESS;
}



VOS_VOID  Taf_GetSyscfgBandGroupStr(TAF_PH_SYSCFG_BAND_STR* pstSysCfgBandStr)
{
    /*
    获取支持的频段项 gstMmaBandCapa.ulAllUeBand
    与Group1相与得到Band1,与Group2相与得到Band2,非空时再根据组合项来显示对应的Str
    */

    TAF_PH_PREF_BAND bandGroup1,bandGroup2,bandAll,bandNeedTransferTo64Bit;
    TAF_CHAR*  BandStr1;
    TAF_CHAR*  BandStr2;

    TAF_CHAR* strSysCfgBandGroup = pstSysCfgBandStr->strSysCfgBandGroup;

    BandStr1 = (TAF_CHAR*)PS_MEM_ALLOC(WUEPS_PID_MMA, (TAF_PH_SYSCFG_GROUP_BAND_LEN/2));

    if (VOS_NULL_PTR == BandStr1)
    {
        return;
    }

    BandStr2 = (TAF_CHAR*)PS_MEM_ALLOC(WUEPS_PID_MMA, (TAF_PH_SYSCFG_GROUP_BAND_LEN/2));

    if (VOS_NULL_PTR == BandStr2)
    {
        PS_MEM_FREE(WUEPS_PID_MMA,BandStr1);
        return;
    }

    PS_MEM_SET(BandStr1,0,(TAF_PH_SYSCFG_GROUP_BAND_LEN/2));
    PS_MEM_SET(BandStr2,0,(TAF_PH_SYSCFG_GROUP_BAND_LEN/2));


    bandAll = TAF_PH_BAND_ANY;
    bandGroup1 = gstMmaBandCapa.ulAllUeBand & TAF_PH_BAND_GROUP1_FOR_SYSCFG;
    bandGroup2 = gstMmaBandCapa.ulAllUeBand & TAF_PH_BAND_GROUP2_FOR_SYSCFG;
    bandNeedTransferTo64Bit = bandGroup1 & TAF_PH_BAND_NEED_CHANGE_TO_64BIT;


    if ((0 == bandGroup1)&&(0 == bandGroup2))
    {
        VOS_sprintf(strSysCfgBandGroup, "((%x,\"All bands\"))",bandAll);
    }
    else if ((0 != bandGroup1) && (0 != bandGroup2))
    {
        Taf_SyscfgBand2Str(bandGroup1,BandStr1);
        Taf_SyscfgBand2Str(bandGroup2,BandStr2);
        if (0 != bandNeedTransferTo64Bit )
        {
            VOS_sprintf(strSysCfgBandGroup, "((%x%08x,\"%s\"),(%x,\"%s\"),(%x,\"All bands\"))",
                        bandNeedTransferTo64Bit,(bandGroup1 & ~bandNeedTransferTo64Bit),BandStr1,bandGroup2,BandStr2,bandAll);
        }
        else
        {
            VOS_sprintf(strSysCfgBandGroup, "((%x,\"%s\"),(%x,\"%s\"),(%x,\"All bands\"))",
                        bandGroup1,BandStr1,bandGroup2,BandStr2,bandAll);
        }
    }
    else if  (0 != bandGroup1)
    {
        Taf_SyscfgBand2Str(bandGroup1,BandStr1);
        if (0 != bandNeedTransferTo64Bit )
        {
            VOS_sprintf(strSysCfgBandGroup, "((%x%08x,\"%s\"),(%x,\"All bands\"))",
                                     bandNeedTransferTo64Bit,(bandGroup1 & ~bandNeedTransferTo64Bit),BandStr1,bandAll);
        }
        else
        {
            VOS_sprintf(strSysCfgBandGroup, "((%x,\"%s\"),(%x,\"All bands\"))",
                                     bandGroup1 ,BandStr1,bandAll);
        }

    }
    else
    {
        Taf_SyscfgBand2Str(bandGroup2,BandStr2);
        VOS_sprintf(strSysCfgBandGroup, "((%x,\"%s\"),(%x,\"All bands\"))",
                                          bandGroup2,BandStr2,bandAll);

    }

    PS_MEM_FREE(WUEPS_PID_MMA,BandStr1);
    PS_MEM_FREE(WUEPS_PID_MMA,BandStr2);

    return;
}


VOS_VOID  MN_MMA_GetSyscfgExLteBandGroupStr(
    TAF_PH_SYSCFG_BAND_STR             *pstSysCfgBandStr
)
{
    TAF_CHAR                           *pchSysCfgBandGroup = VOS_NULL_PTR;
    TAF_PH_PREF_BAND                    stLteAllLowband;
    TAF_PH_PREF_BAND                    stLteAllHighband;

    pchSysCfgBandGroup                  = pstSysCfgBandStr->strSysCfgBandGroup;
    stLteAllHighband                    = MN_MMA_LTE_HIGH_BAND_ANY;
    stLteAllLowband                     = MN_MMA_LTE_LOW_BAND_ANY;

    /* (7fffffffffffffff,"All Bands") */
    VOS_sprintf(pchSysCfgBandGroup, "((%x%x,\"All bands\"))", stLteAllHighband, stLteAllLowband);

    return;
}
VOS_UINT32  TAF_TimeCmp(
    VOS_UINT32                          ulStartTick,
    VOS_UINT32                          ulEndTick,
    VOS_UINT32                          ulCmpSec
)
{
    VOS_UINT32                          ulCostSec;

    /* 时间戳是由0开始递增，所以当前的时间戳应该大于上次。另在时间戳
       到0xFFFFFFFF时会重置到0，所以else分支处理这种情况 */
    if (ulEndTick >= ulStartTick)
    {
        ulCostSec = (ulEndTick - ulStartTick)/(32*1024);
    }
    else
    {
        ulCostSec = (0xFFFFFFFF - ulStartTick + ulEndTick)/(32*1024);
    }

    if ( ulCostSec <= ulCmpSec )
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }

}
TAF_UINT32  Taf_QueryCSNR(TAF_INT16 *psCpichRscp,TAF_INT16 *psCpichEcNo)
{
    TAF_UINT32                          ulRslt;
    TAF_SDC_SYS_MODE_ENUM_UINT8         enRadioMode;        /*手机接入模式查询*/
    static VOS_UINT32                   ulFirstInvalidTick = 0;
    VOS_UINT32                          ulTmpTick;

    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enCurrentUtranMode;

    if (TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE == TAF_SDC_GetServiceStatus())
    {
        /*获取当前网络模式*/
        enRadioMode  = TAF_SDC_GetSysMode();

        if (TAF_SDC_SYS_MODE_WCDMA == enRadioMode)
        {/* WCDMA */

            /* 取得当前的UTRAN模式 */
            enCurrentUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();

            if ( NAS_UTRANCTRL_UTRAN_MODE_FDD == enCurrentUtranMode )
            {
                ulRslt = WAS_MNTN_QueryCSNR(psCpichRscp,psCpichEcNo);
            }
            else
            {
                *psCpichRscp = RRC_NAS_MEASURE_INVALID_VALUE;
                *psCpichEcNo = MMA_RSCP_MIN;
                ulRslt = TAF_FAILURE;
            }

            /* 针对用户发起列表搜后，UE在回RPLMN过程中查询RSCP，此时WAS由于正在搜网，
               返回了一个无效值，导致后台显示无信号，特做如下特殊处理:
               如果UE处于正常服务状态，但WAS返回的信号为无效值，两秒内再查询均使用上次
               的有效值。超过两秒仍然返回无效值，则认为可能搜网异常，返回最小值 */

            if (RRC_NAS_MEASURE_INVALID_VALUE == *psCpichRscp)
            {
                ulTmpTick = OM_GetSlice();

                if ( 0 == ulFirstInvalidTick )
                {
                    /* 记录刚开始无效信号的 tick, 返回保存的信号值 */
                    ulFirstInvalidTick = ulTmpTick;
                    *psCpichRscp = g_sLastRscp;
                    *psCpichEcNo = g_sLastEcNo;
                }
                else if( TAF_TimeCmp(ulFirstInvalidTick, ulTmpTick, MMA_INVALID_RSCP_SEC_MAX ) )
                {
                    /* 2 秒内出现无效信号, 返回保存的信号值 */
                    *psCpichRscp = g_sLastRscp;
                    *psCpichEcNo = g_sLastEcNo;
                }
                else
                {
                    /* 超过2 秒, 返回最小值 */
                    *psCpichRscp = MMA_RSCP_MIN;
                    *psCpichEcNo = MMA_ECNO_MIN;
                }
            }
            else
            {
                /* 正常值则保存 */
                g_sLastRscp = *psCpichRscp;
                g_sLastEcNo = *psCpichEcNo;
                ulFirstInvalidTick = 0;
            }
        }
        else
        {/* GSM */
            *psCpichRscp = MMA_RSCP_MIN;
            *psCpichEcNo = MMA_ECNO_MIN;

            ulRslt = TAF_SUCCESS;
        }
    }
    else
    {   /* 非正常服务状态 */
        *psCpichRscp = MMA_RSCP_MIN;
        *psCpichEcNo = MMA_ECNO_MIN;

        ulRslt = TAF_SUCCESS;

        /* 无服务的时候初始化其为最小值 */
        g_sLastRscp = MMA_RSCP_MIN;
        g_sLastEcNo = MMA_ECNO_MIN;
    }



    if(*psCpichRscp > MMA_RSCP_MAX)
    {
        *psCpichRscp = MMA_RSCP_MAX;
    }

    if(*psCpichRscp < MMA_RSCP_MIN)
    {
        *psCpichRscp = MMA_RSCP_MIN;
    }

    if(*psCpichEcNo > MMA_ECNO_MAX)
    {
        *psCpichEcNo = MMA_ECNO_MAX;
    }

    if(*psCpichEcNo < MMA_ECNO_MIN)
    {
        *psCpichEcNo = MMA_ECNO_MIN;
    }

    return ulRslt;
}
VOS_VOID MN_PH_GetCsqLvlPara(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
)
{
    MN_PH_CSQLVL_PARA_STRU              stCsqLvlPara;
    VOS_INT16                           sCpichRscp;
    VOS_INT16                           sCpichEcNo;
    VOS_UINT16                          ausCsqLvl[AT_CSQLVL_MAX_NUM]={105,102,97};

    sCpichRscp = RRC_NAS_MEASURE_INVALID_VALUE;
    sCpichEcNo = MMA_RSCP_MIN;

    /* 获取 Rscp 值 */
    if (TAF_SUCCESS != Taf_QueryCSNR(&sCpichRscp, &sCpichEcNo))
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_WARNING,
               "MN_PH_GetCsqLvlPara: Taf_QueryCSNR fail.");
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_CSQLVL_VALUE_PARA,TAF_ERR_GET_CSQLVL_FAIL, VOS_NULL_PTR);
        return;
    }

    /* 取绝对值 */
    stCsqLvlPara.usRscp = (VOS_UINT16)(-sCpichRscp);

    /* 按规则转换为相应的格数 */
    if ( stCsqLvlPara.usRscp <= ausCsqLvl[2] )
    {
        stCsqLvlPara.usLevel = AT_CSQLVL_LEVEL_3;
    }
    else if ( stCsqLvlPara.usRscp <= ausCsqLvl[1] )
    {
        stCsqLvlPara.usLevel = AT_CSQLVL_LEVEL_2;
    }
    else if ( stCsqLvlPara.usRscp <= ausCsqLvl[0] )
    {
        stCsqLvlPara.usLevel = AT_CSQLVL_LEVEL_1;
    }
    else
    {
        stCsqLvlPara.usLevel = AT_CSQLVL_LEVEL_0;
    }

    /*使用数据上报函数将数据传至TAFm*/
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_CSQLVL_VALUE_PARA,TAF_ERR_NO_ERROR, (VOS_VOID*)&stCsqLvlPara);

    return;

}

TAF_UINT32  Taf_DefGetCardMode(TAF_UINT8 *pucCardMode)
{

    VOS_UINT8    ucUsimActiveType;
    VOS_UINT8    ucUsimStatus;


    ucUsimStatus     = MMA_USIM_ABSENT;
    ucUsimActiveType = TAF_PH_SPN_TYPE_UNKNOW;

    MMA_GetUsimStatusFromUsim(&ucUsimStatus,  &ucUsimActiveType);

    if ( MMA_USIM_ABSENT != ucUsimStatus )
    {
        if (ucUsimActiveType == TAF_PH_GSM_SPN)
        {
            *pucCardMode     = 1;
        }
        else if (ucUsimActiveType == TAF_PH_USIM_SPN)
        {
            *pucCardMode     = 2;
        }
        else
        {
            *pucCardMode     = 0;
        }
    }
    else
    {
        *pucCardMode     = 0;
    }

    return TAF_SUCCESS;
}


TAF_UINT32 TAF_GetUsimStatus(TAF_VOID)
{
    return MMA_CheckUsimStatusForPlmnSel();
}



VOS_UINT32 MMA_GetUsimStatusFromUsim(VOS_UINT8 *pucCardStatus,
                                                   VOS_UINT8 *pucCardType)
{
    VOS_UINT32               ulResult;
    VOS_UINT8                aucImsi[9] = {0};

    ulResult = NAS_USIMMAPI_GetCardType(pucCardStatus, pucCardType) ;

    if ((VOS_NULL_PTR == pucCardStatus)
     || (USIMM_CARD_SERVIC_AVAILABLE != *pucCardStatus))
    {
        return ulResult;
    }


    /* 跟靳妮确认，如果当前卡状态为USIMM_CARD_SERVIC_AVAILABLE，则一定读完了IMSI卡文件 */
    if (USIMM_API_SUCCESS != NAS_USIMMAPI_GetCardIMSI(aucImsi))
    {
        return ulResult;
    }

    /* 如果IMSI长度非法，则按照无卡处理，注TAF_SDC_MAX_IMSI_LEN长度是包含长度信息的 */
    if (aucImsi[0] >= TAF_SDC_MAX_IMSI_LEN)
    {
        *pucCardStatus = USIMM_CARD_SERVIC_ABSENT;
    }

    return ulResult;
}


VOS_UINT8 MMA_CheckUsimStatusForPlmnSel()
{
    VOS_UINT8                           ucSimStatus = MMA_USIM_ABSENT;
    VOS_UINT8                           ucError    = TAF_ERR_NO_ERROR;

    MMA_PhoneGetUsimStatus(&ucSimStatus);
    if ( MMA_USIM_PRESENT == ucSimStatus )
    {
        /*需要输入PIN码*/
        switch ( MMA_CheckPin1SatusSimple() )
        {
            case MMA_ENABLE_PIN1_NONEED:
            case MMA_NONEED_PIN1:
                ucError = TAF_ERR_NO_ERROR;
                break;
            case MMA_NEED_PIN1:
                ucError = TAF_ERR_NEED_PIN1;
                break;
            case MMA_NEED_PUK1:
                ucError = TAF_ERR_NEED_PUK1;
                break;
            case MMA_PIN1_STATUS_FAIL:
                ucError = TAF_ERR_SIM_FAIL;
                break;
            default:
                ucError = TAF_ERR_UNSPECIFIED_ERROR;
                MMA_WARNINGLOG("MMA_CheckUsimStatusForPlmnSel():WARNING:MEET MYSTERIOUS THING!");
                break;
        }
    }
    else
    {
        ucError = TAF_ERR_USIM_SIM_CARD_NOTEXIST;
    }
    return ucError;
}
VOS_UINT8 MMA_GetCurrentPhoneMode(VOS_VOID)
{
    return TAF_SDC_GetCurPhoneMode();
}

VOS_UINT32 MMA_ClearEplmnFromNV(VOS_VOID)
{
    NVIM_EQUIVALENT_PLMN_LIST_STRU *pEquivPlmn;
    VOS_UINT16    usTemLen;

    usTemLen = sizeof (NVIM_EQUIVALENT_PLMN_LIST_STRU);
    pEquivPlmn = (NVIM_EQUIVALENT_PLMN_LIST_STRU *)MMC_MEM_ALLOC(
                            VOS_MEMPOOL_INDEX_NAS,
                            usTemLen,
                            WUEPS_MEM_NO_WAIT);                             /* 申请内存                                 */
    if (VOS_NULL_PTR == pEquivPlmn)
    {
        PS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_PRINT_ERROR, "MMC_ClearEplmn:ERROR: Memory Alloc Error");
        return MMA_ERROR;                                                             /* 返回                                     */
    }


    if (NV_OK == NV_Read(en_NV_Item_EquivalentPlmn, pEquivPlmn, usTemLen))
    {
        if ( 0 == pEquivPlmn->ucCount )
        {
            MMC_MEM_FREE(VOS_MEMPOOL_INDEX_NAS, pEquivPlmn);
            MMA_INFOLOG("MMA_ClearEplmnFromNV():The content to write is same as NV's");

            return MMA_SUCCESS;
        }
    }


    /* 初始化EPLMN的个数 */
    PS_MEM_SET(pEquivPlmn, 0, usTemLen);

    if (NV_OK != NV_Write(en_NV_Item_EquivalentPlmn,
                          pEquivPlmn,
                          sizeof(NVIM_EQUIVALENT_PLMN_LIST_STRU)))
    {
        PS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_PRINT_WARNING, "MMC_ClearEplmn:WARNING: en_NV_Item_EquivalentPlmn Error");
    }
    MMC_MEM_FREE(VOS_MEMPOOL_INDEX_NAS, pEquivPlmn);

    return MMA_SUCCESS;
}
TAF_UINT32  Taf_GetSysInfoForSTK(TAF_MMA_LOCATION_STATUS_EVENT_INFO_STRU *pstSysInfo)
{
    pstSysInfo->stPlmn = *(TAF_SDC_GetCurrCampPlmnId());

    pstSysInfo->ulCellId            = TAF_SDC_GetCurrCampCellId();

    pstSysInfo->enServiceStatus     = TAF_MMA_GetSrvTypeForStk();
    pstSysInfo->enSysMode = TAF_SDC_GetSysMode();
    pstSysInfo->usLac     = TAF_SDC_GetCurrCampLac();


    return TAF_SUCCESS;
}




TAF_BOOL TAF_IsNormalSrvStatus(VOS_VOID)
{
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enCsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;

    enCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    if ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != enCsServiceStatus )
     && (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != enPsServiceStatus) )
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

#if (VOS_WIN32 == VOS_OS_VER)
/*****************************************************************************
 函 数 名  : NAS_MMA_SndNVData
 功能描述  : 将ps相关的nv发送给OM，以便回放使用
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年1月6日
    作    者   : 赵鹏 59430
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID NAS_MMA_SndNVData(VOS_VOID)
{
    /* Absolute Path for Operating NV file */
    extern const VOS_CHAR g_aucNvFolderPath[]; /* NV Folder path */

    const VOS_CHAR aucNvimCtrlFile[]   = "NvimCtrl.bin";
    const VOS_CHAR aucNvimAuthFile[]   = "NvimAuth.bin";
    const VOS_CHAR aucSYSNVTableFile[] = "SYSNVTable.bin";
    const VOS_CHAR aucPSNVTableFile[]  = "PSNVTable.bin";
    const VOS_CHAR aucRFNVTableFile[]  = "RFNVTable.bin";
    const VOS_CHAR aucCODECNVTableFile[]  = "CODECNVTable.bin";
    const VOS_CHAR aucCFDNVTableFile[]  = "CFDNVTable.bin";

    NAS_MMA_SndNVFileData(g_aucNvFolderPath , aucNvimCtrlFile);
    NAS_MMA_SndNVFileData(g_aucNvFolderPath , aucNvimAuthFile);
    NAS_MMA_SndNVFileData(g_aucNvFolderPath , aucSYSNVTableFile);
    NAS_MMA_SndNVFileData(g_aucNvFolderPath , aucPSNVTableFile);
    NAS_MMA_SndNVFileData(g_aucNvFolderPath , aucRFNVTableFile);
    NAS_MMA_SndNVFileData(g_aucNvFolderPath , aucCODECNVTableFile);
    NAS_MMA_SndNVFileData(g_aucNvFolderPath , aucCFDNVTableFile);

    return;
}
/*****************************************************************************
 函 数 名  : NAS_MMA_SndNVFileData
 功能描述  : 将指定NV法送给OM
 输入参数  : aucFileName NV文件名称
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年1月6日
    作    者   : 赵鹏 59430
    修改内容   : 新生成函数
*****************************************************************************/

VOS_UINT32 NAS_MMA_SndNVFileData(const VOS_CHAR * aucNVFolder ,const VOS_CHAR * aucFileName)
{
    VOS_CHAR                                   *paucNvFilePath = VOS_NULL_PTR;
    VOS_UINT32                                  ulNvFilePathLen;
    FILE *                                      fp = VOS_NULL_PTR;
    VOS_UINT32                                  ulFileSize;
    VOS_UINT32                                  ulReadSize;
    VOS_UINT32                                  ulReadCount;
    VOS_UINT32                                  ulReadTotaldSize;
    VOS_UINT32                                  ulPacketLen;
    NAS_MMA_NV_MSG_ST                          *pNvMsgCB     = VOS_NULL_PTR;



#if (VOS_OS_VER == VOS_VXWORKS)
    ulNvFilePathLen = VOS_StrLen((VOS_CHAR *)aucNVFolder) + VOS_StrLen("/") + VOS_StrLen((VOS_CHAR *)aucFileName);
    paucNvFilePath = (VOS_CHAR *)PS_MEM_ALLOC(WUEPS_PID_MMA, ulNvFilePathLen + 1);

    if ( VOS_NULL_PTR == paucNvFilePath )
    {
        MMA_ERRORLOG("NAS_MMA_SndNVData:ERROR: Alloc Memory Fail.");
        return MMA_ERROR;
    }

    VOS_sprintf(paucNvFilePath , "%s/%s" , aucNVFolder , aucFileName);
#elif( (VOS_WIN32 == VOS_OS_VER) ||  (defined __PS_WIN32_RECUR__) )
    ulNvFilePathLen = VOS_StrLen((VOS_CHAR *)aucNVFolder) + VOS_StrLen((VOS_CHAR *)aucFileName);
    paucNvFilePath = (VOS_CHAR *)PS_MEM_ALLOC(WUEPS_PID_MMA, ulNvFilePathLen + 1);

    if ( VOS_NULL_PTR == paucNvFilePath )
    {
        MMA_ERRORLOG("NAS_MMA_SndNVData:ERROR: Alloc Memory Fail.");
        return MMA_ERROR;
    }

    VOS_sprintf(paucNvFilePath , "%s" , aucFileName);
#endif


    /*以只读的方式打开文件*/
    fp = NV_File_Open((VOS_CHAR*)paucNvFilePath, NV_FILE_OPEN_MODE_R);

    /*如果读取NV file 长度失败*/
    if ( VOS_NULL_PTR == fp )
    {
        PS_MEM_FREE(WUEPS_PID_MMA, paucNvFilePath);
        return MMA_ERROR;
    }

    /*获取文件长度*/
    NV_File_Seek(fp, 0, NV_FILE_SEEK_END);
    ulFileSize = (VOS_UINT32)NV_File_Tell(fp);

    /* 临时处理，由于PS NV文件超过0xFFFF,OM发送数据的的长度类型未UNSIGNED SHORT类型，所以无法发送 */
    if (ulFileSize > 0xe000 )
    {
        ulPacketLen = 0xe000;
    }
    else
    {
        ulPacketLen = ulFileSize;
    }

    /* 指向文件开头 */
    NV_File_Seek(fp, 0, NV_FILE_SEEK_SET);

    /*读取NV file 长度成功*/
    pNvMsgCB = (NAS_MMA_NV_MSG_ST *)PS_ALLOC_MSG(WUEPS_PID_MMA, sizeof(NAS_MMA_NV_MSG_ST) + ulPacketLen);
    if ( VOS_NULL_PTR == pNvMsgCB )
    {
        MMA_ERRORLOG("NAS_MMA_SndNVData:ERROR: Alloc Memory Fail.");

        NV_File_Close(fp);

        PS_MEM_FREE(WUEPS_PID_MMA, paucNvFilePath);
        paucNvFilePath = VOS_NULL_PTR;

        return MMA_ERROR;
    }

    /*构建消息头部*/
    pNvMsgCB->ulReceiverPid   = WUEPS_PID_MMA;
    pNvMsgCB->ulSenderPid     = WUEPS_PID_MMA;
    pNvMsgCB->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pNvMsgCB->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pNvMsgCB->ulLength        = sizeof(NAS_MMA_NV_CONTEXT_ST) + 4 + ulPacketLen;
    pNvMsgCB->usMsgID         = EVT_NAS_MMA_NV_CONTEXT;

    /*NV文件名*/
    VOS_StrCpy((VOS_CHAR *)pNvMsgCB->stNVCtxt.auNvFileName , (VOS_CHAR *)aucFileName);

    /*文件长度*/
    pNvMsgCB->stNVCtxt.ulNvFileLen = ulFileSize;
    pNvMsgCB->stNVCtxt.ulPacketLen = ulPacketLen;

    /*读取文件内容*/
    ulReadCount = 0;
    ulReadSize = 0;
    ulReadTotaldSize = 0;
    do
    {
        if( ( ulPacketLen - ulReadTotaldSize ) >= 4096)
        {
            ulReadSize = 4096;

        }
        else
        {
            ulReadSize = ulPacketLen - ulReadTotaldSize ;

        }

        ulReadCount = (VOS_UINT32)NV_File_Read(
                              (VOS_CHAR*)(pNvMsgCB->stNVCtxt.auNvFileData + ulReadTotaldSize),
                              ulReadSize ,
                              1,
                              fp);

        if (ulReadSize != ulReadCount)
        {
            MMA_ERRORLOG("NAS_MMA_SndNVData:ERROR: NV_File_Read Fail.");
            break;
        }

        ulReadTotaldSize += ulReadSize;

    }while(ulReadTotaldSize < ulPacketLen);

    /*如果读文件成功*/
    if(ulReadTotaldSize == ulPacketLen)
    {
        /*发送消息*/
        OM_TraceMsgHook(pNvMsgCB);
    }

    if ( ulFileSize > ulPacketLen)
    {
        /* 临时处理，文件太大，OM发送不了，OM数据长度类型为UNSIGNED SHORT，所以该处拆分两个报发送 */
        ulPacketLen = ulFileSize - 0xe000;
        /*构建消息头部*/
        pNvMsgCB->ulReceiverPid   = WUEPS_PID_MMA;
        pNvMsgCB->ulSenderPid     = WUEPS_PID_MMA;
        pNvMsgCB->ulSenderCpuId   = VOS_LOCAL_CPUID;
        pNvMsgCB->ulReceiverCpuId = VOS_LOCAL_CPUID;
        pNvMsgCB->ulLength        = sizeof(NAS_MMA_NV_CONTEXT_ST) + 4 + ulPacketLen;
        pNvMsgCB->usMsgID         = EVT_NAS_MMA_NV_CONTEXT;

        /*NV文件名*/
        VOS_StrCpy((VOS_CHAR *)pNvMsgCB->stNVCtxt.auNvFileName , (VOS_CHAR *)aucFileName);

        /*文件长度*/
        pNvMsgCB->stNVCtxt.ulNvFileLen = ulFileSize;
        pNvMsgCB->stNVCtxt.ulPacketLen = ulPacketLen;

        /*读取文件内容*/
        ulReadCount = 0;
        ulReadSize = 0;
        ulReadTotaldSize = 0;
        do
        {
            if( ( ulPacketLen - ulReadTotaldSize ) >= 4096)
            {
                ulReadSize = 4096;
            }
            else
            {
                ulReadSize = ulPacketLen - ulReadTotaldSize ;
            }

            ulReadCount = (VOS_UINT32)NV_File_Read(
                                    (VOS_CHAR*)(pNvMsgCB->stNVCtxt.auNvFileData + ulReadTotaldSize),
                                                ulReadSize ,
                                                1,
                                                fp);

            if (ulReadSize != ulReadCount)
            {
                MMA_ERRORLOG("NAS_MMA_SndNVData:ERROR: NV_File_Read Fail.");
                break;
            }

            ulReadTotaldSize += ulReadSize;

        }while(ulReadTotaldSize < ulPacketLen);

        /*如果读文件成功*/
        if(ulReadTotaldSize == ulPacketLen)
        {
            /*发送消息*/
            OM_TraceMsgHook(pNvMsgCB);
        }
    }

    /*关闭文件*/
    NV_File_Close(fp);

    /*释放内存*/
    PS_MEM_FREE(WUEPS_PID_MMA, paucNvFilePath);
    paucNvFilePath = VOS_NULL_PTR;
    PS_FREE_MSG(WUEPS_PID_MMA, pNvMsgCB);
    pNvMsgCB = VOS_NULL_PTR;

    return MMA_SUCCESS;
}

#endif





VOS_VOID MN_PH_ResetTimerOutProc( VOS_VOID  )
{
    VOS_UINT32                          ulRslt;

    ulRslt = NAS_StopRelTimer(WUEPS_PID_MMA, TI_MN_PH_RESET, &g_stPhPhResetTimer.MmaHTimer);
    if (VOS_OK != ulRslt)
    {
        MMA_ERRORLOG("MN_PH_ResetTimerOutProc:ERROR:NAS_StopRelTimer failed.");
    }

    VOS_FlowReboot();
}


VOS_VOID MN_PH_PinOperTimerOutProc( VOS_VOID )
{
    TAF_PHONE_EVENT_INFO_STRU           *pstPhoneEvent;
    VOS_UINT32                          ulRslt;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                                              sizeof(TAF_PHONE_EVENT_INFO_STRU));
    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("MN_PH_PinOperTimerOutProc:ERROR:ALLOC MEMORY FAIL.");
        return;
    }
    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    ulRslt = NAS_StopRelTimer(WUEPS_PID_MMA, TI_MN_PH_PIN_OPER, &g_stPhPinOperTimer.MmaHTimer);
    if (VOS_OK != ulRslt)
    {
        MMA_ERRORLOG("MN_PH_PinOperTimerOutProc:ERROR:NAS_StopRelTimer FAIL.");
    }
    g_stPhPhResetTimer.ucTimerStatus = MMA_TIMER_STOP;

    MMA_InitEventInfoOP(pstPhoneEvent);

    /*调用TAF事件上报函数,上报PIN事件*/
    pstPhoneEvent->OP_Pin                    = MMA_TRUE;
    pstPhoneEvent->PhoneEvent                = TAF_PH_EVT_OP_PIN_CNF;
    PS_MEM_CPY(&pstPhoneEvent->PinCnf,&g_stPhPinCnf,sizeof(g_stPhPinCnf));
    pstPhoneEvent->PinCnf.OpPinResult = TAF_PH_OP_PIN_OK;

    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    /* MMA_SendPINOk(); */
}
NAS_OM_REGISTER_STATE_ENUM_UINT8 NAS_GetRegisterState(VOS_VOID)
{
    NAS_OM_REGISTER_STATE_ENUM_UINT8    ucRegisterState;

    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode;

    enSysMode = TAF_SDC_GetSysMode();

    ucRegisterState = NAS_OM_REGISTER_STATE_BUTT;

    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        ucRegisterState = NAS_OM_REGISTER_STATE_POWEROFF;
    }
    else
    {
        if (TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE == TAF_SDC_GetServiceStatus())
        {
            if (TAF_SDC_SYS_MODE_GSM == enSysMode)
            {
                ucRegisterState = NAS_OM_REGISTER_STATE_REGISTER_2G;
            }
            else if (TAF_SDC_SYS_MODE_WCDMA == enSysMode)
            {
                ucRegisterState = NAS_OM_REGISTER_STATE_REGISTER_3G;
            }
            else if (TAF_SDC_SYS_MODE_LTE == enSysMode)
            {
                ucRegisterState = NAS_OM_REGISTER_STATE_REGISTER_4G;
            }
            else
            {
                ucRegisterState = NAS_OM_REGISTER_STATE_NOT_REGISTER;
            }
        }
        else
        {
            ucRegisterState = NAS_OM_REGISTER_STATE_NOT_REGISTER;
        }
    }

    return ucRegisterState;
}


NAS_OM_SERVICE_TYPE_ENUM_UINT8   NAS_GetServiceType(VOS_VOID)
{
    NAS_OM_SERVICE_TYPE_ENUM_UINT8  ucServiceType;

    TAF_SDC_SYS_MODE_ENUM_UINT8     enSysMode;

    enSysMode = TAF_SDC_GetSysMode();

    ucServiceType = NAS_OM_SERVICE_TYPE_BUTT;

    if (TAF_SDC_SYS_MODE_GSM == enSysMode)
    {
        if (TAF_APS_USER_CONN_EXIST == TAF_APS_GetUserConnStatus())
        {
            /* 当前在G下 */
            ucServiceType = NAS_OM_SERVICE_TYPE_GSM;
        }
        else
        {
            ucServiceType = NAS_OM_SERVICE_TYPE_NO_SERVICE;
        }
    }
    else if (TAF_SDC_SYS_MODE_WCDMA == enSysMode)
    {
        if (TAF_APS_USER_CONN_EXIST == TAF_APS_GetUserConnStatus())
        {
            /* 当前在W下 */
            switch (g_StatusContext.ucDataTranMode)
            {
                case STA_DATA_TRANSFER_R99_ONLY:
                    ucServiceType = NAS_OM_SERVICE_TYPE_WCDMA;
                    break;
                case STA_DATA_TRANSFER_HSDPA:
                case STA_DATA_TRANSFER_HSUPA:
                case STA_DATA_TRANSFER_HSDPA_AND_HSUPA:
                case STA_DATA_TRANSFER_HSPA_PLUS:
                case STA_DATA_TRANSFER_DC_HSPA_PLUS:
                case STA_DATA_TRANSFER_DC_MIMO:
                    ucServiceType = NAS_OM_SERVICE_TYPE_HSPA;
                    break;
                default:
                    ucServiceType = NAS_OM_SERVICE_TYPE_WCDMA;
                    break;
            }
        }
        else
        {
            ucServiceType = NAS_OM_SERVICE_TYPE_NO_SERVICE;
        }
    }
    else if (TAF_SDC_SYS_MODE_LTE == enSysMode)
    {
        if ( STA_DATA_TRANSFER_LTE_DATA == g_StatusContext.ucDataTranMode )
        {
            ucServiceType = NAS_OM_SERVICE_TYPE_LTE;
        }
        else
        {
            ucServiceType = NAS_OM_SERVICE_TYPE_NO_SERVICE;
        }
    }
    else
    {
        ucServiceType = NAS_OM_SERVICE_TYPE_NO_SERVICE;
    }



    return ucServiceType;
}
VOS_BOOL NAS_MMA_GetServiceDomainSet(VOS_VOID)
{
    if (MMA_SYS_CFG_NONE_SET == gstMmaValue.stSysCfgSet.usSetFlag)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}



VOS_VOID NAS_MMA_TransferSysSubMode2ActForAtCmdCreg(
    VOS_UINT8                           ucSysSubMode,
    TAF_PH_ACCESS_TECH_ENUM_UINT8       *pucAct
)
{
    switch (ucSysSubMode)
    {
        case TAF_SYS_SUBMODE_GSM:
            *pucAct = TAF_PH_ACCESS_TECH_GSM;
            break;
        case TAF_SYS_SUBMODE_GPRS:
            *pucAct = TAF_PH_ACCESS_TECH_GSM;
            break;
        case TAF_SYS_SUBMODE_EDGE:
            *pucAct = TAF_PH_ACCESS_TECH_EGPRS;
            break;
        case TAF_SYS_SUBMODE_WCDMA:
            *pucAct = TAF_PH_ACCESS_TECH_UTRAN;
            break;
        case TAF_SYS_SUBMODE_HSDPA:
            *pucAct = TAF_PH_ACCESS_TECH_HSDPA;
            break;
        case TAF_SYS_SUBMODE_HSUPA:
            *pucAct = TAF_PH_ACCESS_TECH_HSUPA;
            break;
        case TAF_SYS_SUBMODE_HSDPA_HSUPA:
            *pucAct = TAF_PH_ACCESS_TECH_HSDPA_HSUPA;
            break;

        /* 27007协议10.1.20:
          <AcT>: a numeric parameter that indicates the access technology of the serving cell
          0   GSM
          1   GSM Compact
          2   UTRAN
          3   GSM w/EGPRS (see NOTE 2)
          4   UTRAN w/HSDPA (see NOTE 3)
          5   UTRAN w/HSUPA (see NOTE 3)
          6   UTRAN w/HSDPA and HSUPA (see NOTE 3)
          7   E-UTRAN
          NOTE 2: 3GPP TS 44.060 [71] specifies the System Information messages which give the information about whether the serving cell supports EGPRS.
          NOTE 3: 3GPP TS 25.331 [74] specifies the System Information blocks which give the information about whether the serving cell supports HSDPA or HSUPA.
          协议未对HSPA+做明确说明，目前按6UTRAN w/HSDPA and HSUPA上报
        */

        case TAF_SYS_SUBMODE_HSPA_PLUS:
        case TAF_SYS_SUBMODE_DC_HSPA_PLUS:
        case TAF_SYS_SUBMODE_DC_MIMO:
            *pucAct = TAF_PH_ACCESS_TECH_HSDPA_HSUPA;
            break;

        case TAF_SYS_SUBMODE_LTE:
            *pucAct = TAF_PH_ACCESS_TECH_E_UTRAN;
            break;

        default:
            *pucAct = TAF_PH_ACCESS_TECH_BUTT;
            break;
    }

    return;
}


VOS_BOOL NAS_MMA_CellIdChangeForStkLocSta()
{
    TAF_SDC_PLMN_ID_STRU               *pstPlmnId;
    VOS_UINT32                          ulCellId;
    VOS_UINT16                          usLac;

    pstPlmnId = TAF_SDC_GetCurrCampPlmnId();
    usLac     = TAF_SDC_GetCurrCampLac();
    ulCellId  = TAF_SDC_GetCurrCampCellId();

    if ((g_stMmsStkLocStaSysInfo.stPlmn.ulMcc != pstPlmnId->ulMcc)
     || (g_stMmsStkLocStaSysInfo.stPlmn.ulMnc != pstPlmnId->ulMnc)
     || (g_stMmsStkLocStaSysInfo.usLac != usLac)
     || (g_stMmsStkLocStaSysInfo.ulCellId != ulCellId))
    {
       return VOS_TRUE;
    }

    return VOS_FALSE;
}



NAS_STK_SERVICE_STATUS_ENUM_UINT8 NAS_MMA_ConvertSdcServStaToStkServSta(
    TAF_SDC_REPORT_SRVSTA_ENUM_UINT8    enSdcSrvSta
)
{
    switch (enSdcSrvSta)
    {
        case TAF_SDC_REPORT_SRVSTA_NO_SERVICE:
             return NAS_STK_SERVICE_STATUS_NO_SERVICE;

        case TAF_SDC_REPORT_SRVSTA_LIMITED_SERVICE:
             return NAS_STK_SERVICE_STATUS_LIMITED_SERVICE;

        case TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE:
             return NAS_STK_SERVICE_STATUS_NORMAL_SERVICE;

        case TAF_SDC_REPORT_SRVSTA_REGIONAL_LIMITED_SERVICE:
             return NAS_STK_SERVICE_STATUS_LIMITED_SERVICE_REGION;

        case TAF_SDC_REPORT_SRVSTA_DEEP_SLEEP:
             return NAS_STK_SERVICE_STATUS_DEEP_SLEEP;

        default:
            MN_ERR_LOG("NAS_MMA_ConvertSdcServStaToStkServSta: The input is invalid");
            return NAS_STK_SERVICE_STATUS_BUTT;
    }

}




VOS_VOID NAS_MMA_SndStkLociStatusEvent(VOS_VOID)
{
    NAS_STK_LOC_STATUS_EVENT_STRU      *pstMsg        = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;
    TAF_SDC_PLMN_ID_STRU               *pstTempPlmnID = VOS_NULL_PTR;

    /* 申请内存 */
    ulLength = sizeof(NAS_STK_LOC_STATUS_EVENT_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg   = (NAS_STK_LOC_STATUS_EVENT_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA, ulLength);
    if (VOS_NULL_PTR == pstMsg)
    {
        MN_ERR_LOG("TAF_MMA_UpdateStkLociStatusEvent: Alloc Msg");
        return;
    }

    PS_MEM_SET(&(pstMsg->stLocStatusEvent), 0, sizeof(NAS_STK_LOC_STATUS_EVENT_INFO_STRU));

    /* 填写新消息内容 */
    pstMsg->ulMsgId                            = ID_NAS_STK_LOC_STATUS_EVENT;

    pstMsg->ulReceiverPid                      = NAS_GetOamReceivePid(MAPS_STK_PID);

    pstTempPlmnID                              = TAF_SDC_GetCurrCampPlmnId();
    pstMsg->stLocStatusEvent.stPlmn.Mcc        = pstTempPlmnID->ulMcc;
    pstMsg->stLocStatusEvent.stPlmn.Mnc        = pstTempPlmnID->ulMnc;

    pstMsg->stLocStatusEvent.enRat             = TAF_SDC_GetSysMode();

    pstMsg->stLocStatusEvent.enServiceStatus   = NAS_MMA_ConvertSdcServStaToStkServSta(TAF_SDC_GetServiceStatus());

    pstMsg->stLocStatusEvent.ulCellId          = TAF_SDC_GetCurrCampCellId();

    pstMsg->stLocStatusEvent.usLac             = TAF_SDC_GetCurrCampLac();

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG( WUEPS_PID_MMA, pstMsg))
    {
        MN_ERR_LOG("TAF_MMA_UpdateStkLociStatusEvent: PS_SEND_MSG ERROR");
    }

    return;

}



VOS_VOID NAS_MMA_ReportLociStatus(VOS_VOID)
{
    /* 由于SVLTE下两个Modem都更新没有必要，和移动确认过，只需要CS 所在Modem更新即可。
       MMA在调用前判断下，SVLTE形态下，如果没有CS域能力，就不调用。*/
    if (VOS_TRUE == TAF_MMA_IsNeedSndStkEvt())
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_LOCATION_STATUS_EVENT, VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);
#ifndef __PS_WIN32_RECUR__

        NAS_MMA_SndStkLociStatusEvent();

        Taf_GetSysInfoForSTK(&g_stMmsStkLocStaSysInfo);

        gstMmaValue.pg_StatusContext->ucIsReportedLocStatus = VOS_TRUE;
#endif
    }

    return;
}
VOS_UINT32 MN_PH_QryApHplmn(
    VOS_UINT32                          *pulMcc,
    VOS_UINT32                          *pulMnc
)
{
    VOS_UINT8                          *pucImsi = VOS_NULL_PTR;

    pucImsi = TAF_SDC_GetSimImsi();

    if (MMA_SIM_IS_LOCK == g_stMmaMePersonalisationStatus.SimLockStatus)
    {
        MN_WARN_LOG("MN_PH_QryApHplmn:ERROR:SIM LOCKED.");
        return VOS_ERR;
    }

    /* 获取MMC的HPlmn */
    if (USIMM_API_SUCCESS != NAS_USIMMAPI_GetCardIMSI(pucImsi))
    {
        MN_ERR_LOG("MN_PH_QryApHplmn:ERROR:GET IMSI FAIL.");
        return VOS_ERR;
    }

    *pulMcc = 0;
    *pulMnc = 0;

    *pulMcc |= (pucImsi[1] & 0xF0) >> 4;
    *pulMcc |= (pucImsi[2] & 0x0F) << 8;
    *pulMcc |= (pucImsi[2] & 0xF0) << 12;
    *pulMnc |= (pucImsi[3] & 0x0F);
    *pulMnc |= (pucImsi[3] & 0xF0) << 4;
    *pulMnc |= (pucImsi[4] & 0x0F) << 16;


    return VOS_OK;
}
VOS_UINT32 MN_PH_QryAnQuery(
    VOS_INT16                           *psCpichRscp,
    VOS_INT16                           *psCpichEcNo,
    VOS_UINT8                           *pucRssi,
    VOS_UINT32                          *pulCellId
)
{
    VOS_UINT32                          ulResult;

    VOS_INT32                           lRssiValue;

    /* 调用Taf_QueryCSNR来获取rscp, ecio的值 */
    ulResult =  Taf_QueryCSNR(psCpichRscp,psCpichEcNo);
    if(TAF_SUCCESS != ulResult)
    {
        return VOS_ERR;
    }

    if (TAF_SDC_SYS_MODE_GSM == TAF_SDC_GetSysMode())
    {
        lRssiValue = gstMmaValue.stCerssiValue.aRssi[0].u.stGCellSignInfo.sRssiValue;
    }
    else
    {
        lRssiValue = gstMmaValue.stCerssiValue.aRssi[0].u.stWCellSignInfo.sRscpValue;
    }

    MMA_GetRssiLevelFromValue(lRssiValue, pucRssi);

    /* 无服务状态时，将RSSI置为不可测99 */
    if ( TAF_REPORT_SRVSTA_NO_SERVICE == TAF_SDC_GetServiceStatus() )
    {
        *pucRssi = MMA_RSSI_INVALID;
    }

    /* 调用函数Api_MmaGetParaFromMmc获取mmc中的当前驻留的CellId的值 */
    *pulCellId  = TAF_SDC_GetCurrCampCellId();

    /* 上报数据转换:将 Rscp、Ecio显示为非负值，若Rscp、Ecio为-145，-32，或者rssi为99，则转换为0 */
    if ( ((MMA_RSCP_MIN == (*psCpichRscp)) && (MMA_ECNO_MIN == (*psCpichEcNo)))
      || (MMA_RSSI_INVALID == (*pucRssi)) )
    {
        (*psCpichRscp) = 0;
        (*psCpichEcNo) = 0;
        (*pulCellId)   = 0xFFFFFFFF;
    }


    return VOS_OK;
}


VOS_VOID NAS_MMA_OutputUsimInfo(NAS_MNTN_USIM_INFO_STRU *pstUsimInfo)
{
    VOS_UINT32                          ulRet;
    NAS_MML_PLMN_ID_STRU                stRplmn;

    VOS_UINT8                          *pucImsi = VOS_NULL_PTR;

    pucImsi = TAF_SDC_GetSimImsi();

    stRplmn.ulMcc = MMA_INVALID_MCC;
    stRplmn.ulMnc = MMA_INVALID_MNC;

    ulRet = NAS_USIMMAPI_GetCardType(&pstUsimInfo->ucCardStatus,
                              &pstUsimInfo->ucCardType);

    if (USIMM_API_SUCCESS != ulRet)
    {
        MN_WARN_LOG("NAS_MMA_OutputUsimInfo: Fail to get (U)SIM information.");
        return;
    }

    /* 快速开机或无卡无其他信息需要获取，直接返回 */
    if ((USIMM_CARD_ROM_SIM       == pstUsimInfo->ucCardType)
     || (USIMM_CARD_SERVIC_ABSENT == pstUsimInfo->ucCardStatus))
    {
        return;
    }

    /* 获取卡的PIN码启动状态
    #define TAF_ERR_NO_ERROR                   (0)  成功
    #define TAF_ERR_NEED_PIN1                 (18)  需要输入PIN1码
    #define TAF_ERR_NEED_PUK1                 (19)  需要输入PUK1
    #define TAF_ERR_SIM_FAIL                  (24)  SIM卡操作失败
    #define TAF_ERR_UNSPECIFIED_ERROR         (50)  其他错误类型
    */
    pstUsimInfo->ucPinStatus = MMA_CheckUsimStatusForPlmnSel();

    if (TAF_ERR_NO_ERROR == pstUsimInfo->ucPinStatus)
    {
        PS_MEM_CPY(pstUsimInfo->aucImsi, pucImsi, NAS_MNTN_IMSI_LENGTH_IN_USIM);

        if (VOS_TRUE != NAS_MML_GetGURPlmn(&stRplmn))
        {
            MN_WARN_LOG("NAS_MMA_OutputUsimInfo(): Warning:NAS_MML_GetGURPlmn failed.");
        }

        pstUsimInfo->stRplmn.ulMcc = stRplmn.ulMcc;
        pstUsimInfo->stRplmn.ulMnc = stRplmn.ulMnc;
    }

    return;
}


VOS_VOID NAS_MMA_OutputPlmnSrchBand(VOS_UINT32 *pulBand)
{
    /* BAND设置：gstMmaValue.uUserSetBand */
    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU  *pstLastSyscfgSet = VOS_NULL_PTR;

    pstLastSyscfgSet = MN_MMA_GetLastSyscfgSetAddr();
    *pulBand = pstLastSyscfgSet->stUserSetBand.uUserSetGuBand.ulPrefBand;
    return;
}
VOS_VOID MN_PH_SimLockedRpt( VOS_VOID  )
{
    TAF_PHONE_EVENT_INFO_STRU           *pstPhoneEvent;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                        sizeof(TAF_PHONE_EVENT_INFO_STRU));
    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MN_ERR_LOG("MN_PH_PinOperTimerOutProc:ERROR:ALLOC MEMORY FAIL.");
        return ;
    }
    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    MMA_InitEventInfoOP(pstPhoneEvent);

    /*调用TAF事件上报函数,上报锁卡事件*/
    pstPhoneEvent->PhoneEvent                = MN_PH_EVT_SIMLOCKED_IND;
    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

}

VOS_UINT8 APP_MN_PH_AppQryPhMode (VOS_VOID)
{
    return TAF_SDC_GetCurPhoneMode();
}
VOS_VOID MN_PH_RoamingRpt( VOS_UINT8 ucTmpRoamStatus )
{
    TAF_PHONE_EVENT_INFO_STRU           *pstPhoneEvent;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                        sizeof(TAF_PHONE_EVENT_INFO_STRU));
    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MN_ERR_LOG("MN_PH_RoamingRpt:ERROR:ALLOC MEMORY FAIL.");
        return ;
    }
    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    MMA_InitEventInfoOP(pstPhoneEvent);

    /*调用TAF事件上报函数,上报锁卡事件*/
    pstPhoneEvent->PhoneEvent                = MN_PH_EVT_ROAMING_IND;
    pstPhoneEvent->ucRoamStatus              = ucTmpRoamStatus;
    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

}
VOS_UINT8 NAS_MMA_GetPhoneMode(VOS_VOID)
{
    return TAF_SDC_GetCurPhoneMode();
}


VOS_UINT8 MN_MMA_GetServiceDomain(VOS_VOID)
{
    return gstMmaValue.stLastSyscfgSet.ucSrvDomain;
}


/* MMA_SndTafMmcPlmnSrchReq() 移至MmaSndMmc.c中*/

VOS_UINT8 MN_MMA_GetRoamingBrokerFlg(VOS_VOID)
{
    return gstMmaValue.ucRoamBrokerSwitch;
}


VOS_UINT32 MN_MMA_IsSpecRatSupported(
    TAF_PH_RAT_TYPE_ENUM_UINT8          enSpecRat
)
{
    VOS_UINT32                          i;
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList = VOS_NULL_PTR;

    pstRatPrioList     = MN_MMA_GetRatPrioListAddr();

    /* 如果GSM禁止接入,则认为不支持GSM */
    if ((MMA_TRUE == MMA_IsGsmForbidden())
     && (TAF_PH_RAT_GSM == enSpecRat))
    {
        return VOS_FALSE;
    }

    /* 指定的接入技术是否在当前用户设置的RAT列表中 */
    for (i = 0 ; i < pstRatPrioList->ucRatOrderNum; i++)
    {
        if ((enSpecRat == pstRatPrioList->aenRatOrder[i])
         && (enSpecRat < TAF_PH_RAT_BUTT))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

VOS_UINT32 MN_MMA_IsSpecRatInRatList(
    TAF_PH_RAT_TYPE_ENUM_UINT8          enSpecRat,
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList
)
{
    VOS_UINT32                          i;

    /* 指定的接入技术是否在pstRatPrioList中 */
    for (i = 0 ; i < pstRatPrioList->ucRatOrderNum; i++)
    {
        if ((enSpecRat == pstRatPrioList->aenRatOrder[i])
         && (enSpecRat < TAF_PH_RAT_BUTT))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}
TAF_PH_RAT_TYPE_ENUM_UINT8 MN_MMA_GetUserPlmnSelPrioRat(
    TAF_PH_RAT_TYPE_ENUM_UINT8          enPhRat,
    TAF_PLMN_ID_STRU                    stPlmn
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          j;
    TAF_PH_RAT_ORDER_STRU               stRatPrioList;
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList = VOS_NULL_PTR;

    pstRatPrioList     = MN_MMA_GetRatPrioListAddr();

    /* 如果用户指定的接入技术当前支持,则优先搜索用户指定的接入技术 */
    if (VOS_TRUE == MN_MMA_IsSpecRatSupported(enPhRat))
    {
        return enPhRat;
    }

    stRatPrioList.ucRatOrderNum = 0x0;

    /* 从MMA上报的PLMN列表中查找指定的网络,如果找到，将对应的接入技术保存 */
    for (i = 0; i < g_StatusContext.PlmnList.ucPlmnNum; i++)
    {
        /* 比较用户指定的网络与列表中保存的网络,由于用户是直接从上报的列表中选择的网络,因此可以直接比较 */
        if ((stPlmn.Mcc == g_StatusContext.PlmnList.Plmn[i].Mcc)
         && (stPlmn.Mnc == g_StatusContext.PlmnList.Plmn[i].Mnc))
        {
            stRatPrioList.aenRatOrder[stRatPrioList.ucRatOrderNum] =
                g_StatusContext.PlmnList.PlmnInfo[i].RaMode;

            stRatPrioList.ucRatOrderNum++;
        }

        /* 保护越界 */
        if (stRatPrioList.ucRatOrderNum >= TAF_PH_MAX_GUL_RAT_NUM)
        {
            break;
        }
    }

    /* 用户指定的接入技术当前不支持,如果列表中包含用户指定的网络,则返回存在接入技术中最高优先级的接入技术 */
    for (i = 0; i < pstRatPrioList->ucRatOrderNum; i++)
    {
        for (j = 0; j < stRatPrioList.ucRatOrderNum; j++)
        {
            if (pstRatPrioList->aenRatOrder[i] == stRatPrioList.aenRatOrder[j])
            {
                return pstRatPrioList->aenRatOrder[i];
            }
        }
    }

    if ((MMA_TRUE == MMA_IsGsmForbidden())
     && (TAF_PH_RAT_GSM == pstRatPrioList->aenRatOrder[0]))
    {
        /* 用户指定的接入技术当前不支持,而且列表中也没有用户指定的网络,
           最高优先级为GSM而且当前GSM接入禁止,此时不可能为GSM单模(SYSCFG会保证),
           返回次高优先级接入技术 */
        return pstRatPrioList->aenRatOrder[1];
    }

    /* 用户指定的接入技术当前不支持,而且列表中也没有用户指定的网络,
       返回用户设置的最优先接入技术 */
    return pstRatPrioList->aenRatOrder[0];
}


VOS_VOID MMA_RegisterTimeQuery(VOS_UINT16 ClientId, VOS_UINT8 OpId)
{
    VOS_UINT32                          ulCostTime;

    ulCostTime = MN_PH_GetRegCostTime();

    /* 上报注册时间 */
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_REGISTER_TIME_VALUE_PARA, TAF_ERR_NO_ERROR, &ulCostTime);

    return;
}
VOS_VOID MMA_AnQueryProc(VOS_UINT16 ClientId, VOS_UINT8 OpId)
{
    MN_MMA_ANQUERY_PARA_STRU            stAnqueryPara;
    VOS_UINT32                          ulResult;

    /*调用函数MN_PH_QryAnQuery获取rscp, ecio,rssi,cellid */
    ulResult = MN_PH_QryAnQuery(&stAnqueryPara.sCpichRscp,
                                &stAnqueryPara.sCpichEcNo,
                                &stAnqueryPara.ucRssi,
                                &stAnqueryPara.ulCellId);


#if ( FEATURE_ON == FEATURE_LTE )
    if (TAF_SDC_SYS_MODE_LTE == TAF_SDC_GetSysMode())
    {
        /* 调用L模接口，由该接口转发消息给L4A获取小区信号，再由L4A回复消息给AT。 */
        if (VOS_OK != TAF_MMA_SendL4aAnqueryInfoReq(ClientId,(VOS_INT8)OpId) )
        {
            MMA_ParaQueryReport(ClientId, OpId, TAF_PH_ANQUERY_VALUE_PARA,TAF_ERR_AT_ERROR, VOS_NULL_PTR);
        }

        return ;
    }
#endif

    /* 回复AT^ANQUERY查询命令 */
    if (VOS_OK != ulResult)
    {
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_ANQUERY_VALUE_PARA, TAF_ERR_ERROR, VOS_NULL_PTR);
    }
    else
    {
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_ANQUERY_VALUE_PARA, TAF_ERR_NO_ERROR, &stAnqueryPara);
    }

    return;
}
VOS_VOID MMA_HomePlmnProc(VOS_UINT16 ClientId, VOS_UINT8 OpId)
{
    VOS_UINT32 ulRet;
    VOS_UINT8 ucSimStatus;
    USIMM_GET_FILE_INFO_STRU            stGetFileInfo;

    if(MMA_TIMER_STOP != gstMmaInternalTimer.stTimer.ucTimerStatus)
    {
        MMA_WARNINGLOG("MMA_HomePlmnProc():WARNING:USIM is not Ready!");
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_HOMEPLMN_VALUE_PARA, TAF_ERR_ERROR, VOS_NULL_PTR);
        return;
    }

    /*判断卡状态*/
    ulRet = MMA_PhoneGetUsimStatus(&ucSimStatus );
    if ( (ulRet != MMA_SUCCESS )
       ||(MMA_USIM_ABSENT == ucSimStatus)
       )
    {
        MMA_WARNINGLOG("MMA_HomePlmnProc():WARNING:USIM is Error or Absent!");
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_HOMEPLMN_VALUE_PARA, TAF_ERR_ERROR, VOS_NULL_PTR);
        return;
    }

    /*启动定时器*/
    if(MMA_SUCCESS != MMA_StartInternalTimer(1,
                                             MMA_GET_USIM_MNC_LENGTH_OP,
                                             ClientId,
                                             OpId))
    {
        MMA_WARNINGLOG("MMA_HomePlmnProc():WARNING:Unknow Error!");
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_HOMEPLMN_VALUE_PARA, TAF_ERR_ERROR, VOS_NULL_PTR);
        return;
    }
    /*读卡*/
    TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                    USIMM_UNLIMIT_APP,
                                    MMA_MNC_LENGTH_FILE_ID,
                                    0);

    if( USIMM_API_SUCCESS != NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA,
                                              0,
                                              &stGetFileInfo))
    {
        MMA_WARNINGLOG("MMA_HomePlmnProc():WARNING:get File Error!");
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_HOMEPLMN_VALUE_PARA, TAF_ERR_ERROR, VOS_NULL_PTR);

        /*停止定时器*/
        MMA_StopInternalTimer();
        return;
    }

    gstMmaValue.stGetHplmn.usClientId = ClientId;
    gstMmaValue.stGetHplmn.ucOpId     = OpId;
    gstMmaValue.stGetHplmn.ucHplmnQryFlag = VOS_TRUE;


    return;
}
VOS_VOID MMA_CsnrParaProc(VOS_UINT16 ClientId, VOS_UINT8 OpId)
{
    MN_MMA_CSNR_PARA_STRU               stCsnrPara;
    VOS_UINT32                          ulResult;

    /* 调用 Taf_QueryCSNR 获取信号值 */
    ulResult = Taf_QueryCSNR(&stCsnrPara.sCpichRscp, &stCsnrPara.sCpichEcNo);

    /* 上报AT^CSNR参数查询结果事件 */
    if (VOS_OK != ulResult)
    {
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_CSNR_VALUE_PARA, TAF_ERR_ERROR, VOS_NULL_PTR);
    }
    else
    {
        MMA_ParaQueryReport(ClientId, OpId, TAF_PH_CSNR_VALUE_PARA, TAF_ERR_NO_ERROR, &stCsnrPara);
    }

    return;
}


TAF_UINT32 MMA_String2Hex(VOS_UINT8 *nptr,VOS_UINT16 usLen,VOS_UINT32 *pRtn)
{
    VOS_UINT32 c     = 0;         /* current Char */
    VOS_UINT32 total = 0;         /* current total */
    VOS_UINT8 Length = 0;         /* current Length */

    c = (VOS_UINT32)*nptr++;

    while(Length++ < usLen)
    {
        if( (c  >= '0') && (c  <= '9') )
        {
            c  = c  - '0';
        }
        else if( (c  >= 'a') && (c  <= 'f') )
        {
            c  = (c  - 'a') + 10;
        }
        else if( (c  >= 'A') && (c  <= 'F') )
        {
            c  = (c  - 'A') + 10;
        }
        else
        {
            return VOS_ERR;
        }

        if(total > 0x0FFFFFFF)              /* 发生反转 */
        {
            return VOS_ERR;
        }
        else
        {
            total = (total << 4) + c;              /* accumulate digit */
            c = (TAF_UINT32)(TAF_UINT8)*nptr++;    /* get next Char */
        }
    }

    *pRtn = total;   /* return result, negated if necessary */
    return VOS_OK;
}
VOS_VOID MMA_CrpnQuery_GetPlmnNum(
    TAF_PH_OPERATOR_NAME_STRU          *pstOrigOperName,
    MN_MMA_CRPN_QRY_REQ_STRU           *pstMnMmaCrpnQry,
    VOS_UINT8                          *pucMaxMncLen,
    VOS_UINT32                         *pulTotalNum
)
{
    VOS_UINT8                           ucMaxMncLen;
    VOS_UINT32                          ulTotalNum;

    if (0 == pstMnMmaCrpnQry->ucPlmnType)
    {
        PS_MEM_CPY(pstOrigOperName->aucOperatorNameLong, pstMnMmaCrpnQry->aucPlmnName, pstMnMmaCrpnQry->usPlmnNameLen);
        pstOrigOperName->aucOperatorNameLong[pstMnMmaCrpnQry->usPlmnNameLen] = 0;
        ulTotalNum = MMA_PhoneGetAllOperNumByLongNameFromOperTbl(pstOrigOperName, &ucMaxMncLen);
    }
    else if(1 == pstMnMmaCrpnQry->ucPlmnType)
    {
        PS_MEM_CPY(pstOrigOperName->aucOperatorNameShort, pstMnMmaCrpnQry->aucPlmnName, pstMnMmaCrpnQry->usPlmnNameLen);
        pstOrigOperName->aucOperatorNameShort[pstMnMmaCrpnQry->usPlmnNameLen] = 0;
        ulTotalNum = MMA_PhoneGetAllOperNumByShortNameFromOperTbl(pstOrigOperName, &ucMaxMncLen);
    }
    else
    {
        /*一个PLMN ID只对应于一个PLMN INFO*/
        ulTotalNum = 1;
        ucMaxMncLen = (VOS_UINT8)pstMnMmaCrpnQry->usPlmnNameLen;
    }

    *pucMaxMncLen = ucMaxMncLen;
    *pulTotalNum  = ulTotalNum;

    return;
}
VOS_VOID MMA_CrpnQuery(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    MN_MMA_CRPN_QRY_REQ_STRU           *pstMnMmaCrpnQry
)
{
    TAF_PH_OPERATOR_NAME_STRU           stOrigOperName;
    VOS_UINT8                           ucMaxMncLen;
    VOS_UINT32                          ulTotalNum;
    MN_MMA_CRPN_QRY_CNF_STRU           *pstMnMmaCrpnQryCnf;
    TAF_PH_OPERATOR_NAME_STRU          *pstOperName;
    VOS_UINT32                          i;

    /* 变量初始化 */
    pstMnMmaCrpnQryCnf = VOS_NULL_PTR;
    pstOperName        = VOS_NULL_PTR;
    PS_MEM_SET(&stOrigOperName, 0, sizeof(TAF_PH_OPERATOR_NAME_STRU));

    /* 获取符合条件的PLMN总个数 */
    MMA_CrpnQuery_GetPlmnNum(&stOrigOperName, pstMnMmaCrpnQry, &ucMaxMncLen, &ulTotalNum);

    /* 检查是否有网络，并且MNC是否合法 */
    if ( (0 == ulTotalNum)
      || ((5 != ucMaxMncLen) && (6 != ucMaxMncLen)))
    {
        MMA_PhCrpnQueryRsp(ClientId, OpId, TAF_ERR_PB_WRONG_INDEX, VOS_NULL_PTR);

        return;
    }

    /* 分配PLMN INFO的存储空间 */
    pstOperName = (TAF_PH_OPERATOR_NAME_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                                            sizeof(TAF_PH_OPERATOR_NAME_STRU) * ulTotalNum);

    if (VOS_NULL_PTR == pstOperName)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "MMA_CrpnQuery: Alloc mem fail!");
        return;
    }

    /* 获取PLMN INFO */
    if (0 == pstMnMmaCrpnQry->ucPlmnType)
    {
        MMA_PhoneGetAllOperInfoByLongNameFromOperTbl(&stOrigOperName, pstOperName);

    }
    else if(1 == pstMnMmaCrpnQry->ucPlmnType)
    {
        MMA_PhoneGetAllOperInfoByShortNameFromOperTbl(&stOrigOperName, pstOperName);

    }
    else
    {
        /* Mcc */
        if(VOS_ERR == MMA_String2Hex(pstMnMmaCrpnQry->aucPlmnName,
                                     3,
                                     &pstOperName->PlmnId.Mcc))
        {
            /* 释放动态内存 */
            PS_MEM_FREE(WUEPS_PID_MMA, pstOperName);

            MMA_PhCrpnQueryRsp(ClientId, OpId, TAF_ERR_PARA_ERROR, VOS_NULL_PTR);
            return;
        }
        /* Mnc */
        if(VOS_ERR == MMA_String2Hex(&pstMnMmaCrpnQry->aucPlmnName[3],
                                      pstMnMmaCrpnQry->usPlmnNameLen - 3,
                                     &pstOperName->PlmnId.Mnc))
        {
            /* 释放动态内存 */
            PS_MEM_FREE(WUEPS_PID_MMA, pstOperName);

            MMA_PhCrpnQueryRsp(ClientId, OpId, TAF_ERR_PARA_ERROR, VOS_NULL_PTR);
            return;
        }

        pstOperName->PlmnId.Mcc |= 0xFFFFF000;
        pstOperName->PlmnId.Mnc |= (0xFFFFFFFF << ((pstMnMmaCrpnQry->usPlmnNameLen-3)*4));

        if (MMA_SUCCESS != MMA_PhoneFindNtwkNameByPlmnId(pstOperName))
        {
            /* 释放动态内存 */
            PS_MEM_FREE(WUEPS_PID_MMA, pstOperName);

            MMA_PhCrpnQueryRsp(ClientId, OpId, TAF_ERR_PB_WRONG_INDEX, VOS_NULL_PTR);
            return;
        }
    }


    /* 申请消息内存 */
    pstMnMmaCrpnQryCnf = (MN_MMA_CRPN_QRY_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                                            sizeof(MN_MMA_CRPN_QRY_CNF_STRU));
    if (VOS_NULL_PTR == pstMnMmaCrpnQryCnf)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "MMA_CrpnQuery: Alloc mem fail!");

        /* 释放动态内存 */
        PS_MEM_FREE(WUEPS_PID_MMA, pstOperName);
        return;
    }
    PS_MEM_SET(pstMnMmaCrpnQryCnf, 0, sizeof(MN_MMA_CRPN_QRY_CNF_STRU));

    /* 发送消息赋值 */
    pstMnMmaCrpnQryCnf->ucMaxMncLen = ucMaxMncLen;
    pstMnMmaCrpnQryCnf->ucTotalNum  = (VOS_UINT8)ulTotalNum;
    PS_MEM_CPY(&pstMnMmaCrpnQryCnf->stMnMmaCrpnQry, pstMnMmaCrpnQry, sizeof(MN_MMA_CRPN_QRY_REQ_STRU));

    if (ulTotalNum > TAF_PH_CRPN_PLMN_MAX_NUM)
    {
        ulTotalNum = TAF_PH_CRPN_PLMN_MAX_NUM;
    }

    for (i = 0; i < ulTotalNum; i++)
    {
        PS_MEM_CPY(&pstMnMmaCrpnQryCnf->stOperNameList[i], (pstOperName + i), sizeof(TAF_PH_OPERATOR_NAME_STRU));
    }

    MMA_PhCrpnQueryRsp(ClientId, OpId, TAF_ERR_NO_ERROR, pstMnMmaCrpnQryCnf);

    /* 释放动态内存 */
    PS_MEM_FREE(WUEPS_PID_MMA, pstOperName);
    PS_MEM_FREE(WUEPS_PID_MMA, pstMnMmaCrpnQryCnf);

    return;
}
VOS_VOID MMA_SpnParaProc(VOS_UINT16 ClientId, VOS_UINT8 OpId)
{
    MN_MMA_SPN_PARA_STRU                stMnMmaSpnInfo;

    /* SIM SPN */
    PS_MEM_CPY(&stMnMmaSpnInfo.stSimSpnInfo, &gstMmaValue.stUsimInfo.SimSpnInfo, sizeof(TAF_PH_USIM_SPN_STRU));

    /* USIM SPN */
    PS_MEM_CPY(&stMnMmaSpnInfo.stUsimSpnInfo, &gstMmaValue.stUsimInfo.UsimSpnInfo, sizeof(TAF_PH_USIM_SPN_STRU));

    /* 上报AT^SPN参数查询结果事件 */
    MMA_ParaQueryReport(ClientId, OpId, TAF_PH_SPN_VALUE_PARA, TAF_ERR_NO_ERROR, &stMnMmaSpnInfo);

    return;
}
VOS_VOID MMA_CmmTestCmdProc(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    MM_TEST_AT_CMD_STRU                *pStAtTestCmd
)
{
    MN_MM_TEST_CMD_RSP_MSG_STRU        *pstMnMmTestCmdRspMsg;
    VOS_UINT32                          ulRst;

    /* 申请消息内存 */
    pstMnMmTestCmdRspMsg = (MN_MM_TEST_CMD_RSP_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(MN_MM_TEST_CMD_RSP_MSG_STRU));

    if( VOS_NULL_PTR == pstMnMmTestCmdRspMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "MMA_CmmTestCmdProc:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    pstMnMmTestCmdRspMsg->ulResult = Mm_AtTest(pStAtTestCmd, &pstMnMmTestCmdRspMsg->stAtCmdRslt);

    /* 填写消息 */
    pstMnMmTestCmdRspMsg->ulReceiverPid          = WUEPS_PID_AT;
    pstMnMmTestCmdRspMsg->ulMsgName              = AT_MMA_CMM_SET_CMD_RSP;
    pstMnMmTestCmdRspMsg->stAtAppCtrl.usClientId = MN_GetRealClientId(ClientId, WUEPS_PID_MMA);
    pstMnMmTestCmdRspMsg->stAtAppCtrl.ucOpId     = OpId;

    /* 发送消息 */
    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstMnMmTestCmdRspMsg);
    if( VOS_OK != ulRst)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "MMA_CmmTestCmdProc:ERROR: Send Msg Error!");
    }

    return;
}
VOS_INT32* MN_MMA_GetBandInfo(VOS_VOID)
{
    return (VOS_INT32*)&gstMmaBandCapa;
}


VOS_VOID MMA_PhCrpnQueryRsp(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    VOS_UINT32                          ulResult,
    MN_MMA_CRPN_QRY_CNF_STRU           *pStMnMmaCrpnQryCnf
)
{
    MN_MMA_CRPN_QRY_CNF_MSG_STRU       *pstMnMmaCrpnQryCnfMsg;
    VOS_UINT32                          ulRst;
    MN_MMA_CRPN_QRY_CNF_STRU           *pData;

    pData = pStMnMmaCrpnQryCnf;

    /* 申请消息内存 */
    pstMnMmaCrpnQryCnfMsg = (MN_MMA_CRPN_QRY_CNF_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(MN_MMA_CRPN_QRY_CNF_MSG_STRU));

    if( VOS_NULL_PTR == pstMnMmaCrpnQryCnfMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "MMA_PhCrpnQueryRsp:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    /* 初始化消息数据 */
    PS_MEM_SET(&pstMnMmaCrpnQryCnfMsg->stMnMmaCrpnQryCnf, 0, sizeof(MN_MMA_CRPN_QRY_CNF_STRU));

    /* 填写消息 */
    pstMnMmaCrpnQryCnfMsg->ulReceiverPid          = WUEPS_PID_AT;
    pstMnMmaCrpnQryCnfMsg->ulMsgName              = AT_MMA_CRPN_QUERY_RSP;
    pstMnMmaCrpnQryCnfMsg->stAtAppCtrl.usClientId = MN_GetRealClientId(ClientId, WUEPS_PID_MMA);
    pstMnMmaCrpnQryCnfMsg->stAtAppCtrl.ucOpId     = OpId;
    pstMnMmaCrpnQryCnfMsg->ulResult               = ulResult;

    if (VOS_NULL_PTR != pData)
    {
        PS_MEM_CPY(&pstMnMmaCrpnQryCnfMsg->stMnMmaCrpnQryCnf, pData, sizeof(MN_MMA_CRPN_QRY_CNF_STRU));
    }

    /* 发送消息 */
    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstMnMmaCrpnQryCnfMsg);
    if( VOS_OK != ulRst)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "MMA_PhCrpnQueryRsp:ERROR: Send Msg Error!");
    }

    return;
}



VOS_VOID MMA_PhUsimStatusInd(
    PS_USIM_STATUS_IND_STRU           *pStPsUsimStatusInd
)
{
    AT_MMA_USIM_STATUS_IND_STRU        *pstAtMmaUsimStatusIndMsg;
    VOS_UINT32                          ulRst;

    /* 申请消息内存 */
    pstAtMmaUsimStatusIndMsg = (AT_MMA_USIM_STATUS_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(AT_MMA_USIM_STATUS_IND_STRU));

    if( VOS_NULL_PTR == pstAtMmaUsimStatusIndMsg )
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR, "MMA_PhUsimStatusInd:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    /* 填写消息 */
    pstAtMmaUsimStatusIndMsg->ulReceiverPid          = WUEPS_PID_AT;
    pstAtMmaUsimStatusIndMsg->ulMsgName              = AT_MMA_USIM_STATUS_IND;

    /* 此处要求USIM上报准确的卡状态，这里直接透传到AT */
    pstAtMmaUsimStatusIndMsg->enCardStatus           = pStPsUsimStatusInd->enCardStatus;

    pstAtMmaUsimStatusIndMsg->enCardType             = pStPsUsimStatusInd->enCardType;

    pstAtMmaUsimStatusIndMsg->ucIMSILen              = pStPsUsimStatusInd->ucIMSILen;
    PS_MEM_CPY(pstAtMmaUsimStatusIndMsg->aucIMSI, pStPsUsimStatusInd->aucIMSI, NAS_MAX_IMSI_LENGTH);

    /* 发送消息 */
    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstAtMmaUsimStatusIndMsg);
    if( VOS_OK != ulRst)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR, "MMA_PhUsimStatusInd:ERROR: Send Msg Error!");
    }

    return;
}


VOS_VOID Taf_InitEventInfoOP(TAF_PHONE_EVENT_INFO_STRU *pstEvent)
{

    /*填充所有选项为0*/
    pstEvent->OP_RegPlmn            = 0;
    pstEvent->OP_CsServiceStatus    = 0;
    pstEvent->OP_PsServiceStatus    = 0;
    pstEvent->OP_PlmnList           = 0;
    pstEvent->OP_PlmnReselMode      = 0;
    pstEvent->OP_PhoneError         = 0;
    pstEvent->OP_CurRegSta          = 0;
    pstEvent->OP_Rssi               = 0;
    pstEvent->OP_BatteryPower       = 0;
    pstEvent->OP_Pin                = 0;
    pstEvent->OP_UsimAccessData     = 0;
    pstEvent->OP_PinRemain          = 0;
    pstEvent->OP_MeLockStatus       = 0;
    pstEvent->OP_MePersonalisation  = 0;
    pstEvent->OP_OperMode           = 0;
    pstEvent->OP_SpnInfo            = 0;
    pstEvent->OP_Mode               = 0;
    pstEvent->OP_Srvst              = 0;
    pstEvent->OP_UsimRestrictAccess = 0;
    pstEvent->OP_UsimPNN            = 0;
    pstEvent->OP_SysCfg             = 0;
    pstEvent->OP_UsimOPL            = 0;
    pstEvent->OP_Spare              = 0;

    return;
}
VOS_VOID MMA_PrefPlmnHandle(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    TAF_PH_PREF_PLMN_OPERATE_STRU       *pstPrefPlmnOperate
)
{
    VOS_UINT32                          ulRet;
    VOS_BOOL                            bFinish = VOS_TRUE;

    if (MN_PH_PrefPLMN_DEL_ONE == pstPrefPlmnOperate->enPrefPlmnOperType)
    {

        ulRet = MMA_DelPrefPlmnInfo(ClientId,
                                    OpId,
                                    pstPrefPlmnOperate->enPrefPLMNType,
                                    pstPrefPlmnOperate->ulIndex,
                                    &bFinish);
        if (VOS_FALSE == bFinish)
        {
            return;
        }

        Taf_ParaSetResult(ClientId,
                          OpId,
                          (VOS_UINT8)ulRet,
                          TAF_PH_SET_PERF_PLMN_PARA,
                          VOS_NULL_PTR);
    }
    else if (MN_PH_PrefPLMN_MODIFY_ONE == pstPrefPlmnOperate->enPrefPlmnOperType)
    {
        ulRet = MN_MMA_ModPrefPlmnInfo(ClientId,
                                       OpId,
                                       pstPrefPlmnOperate->enPrefPLMNType,
                                       pstPrefPlmnOperate->ulIndex,
                                       pstPrefPlmnOperate->usRat,
                                       &(pstPrefPlmnOperate->stPlmn));
       if (TAF_PARA_OK != ulRet)
       {
           Taf_ParaSetResult(ClientId,
                             OpId,
                             (VOS_UINT8)ulRet,
                             TAF_PH_SET_PERF_PLMN_PARA,
                             VOS_NULL_PTR);
       }
    }
    else
    {
        ulRet = MMA_AddPrefPlmnInfo(ClientId,
                                    OpId,
                                    pstPrefPlmnOperate->enPrefPLMNType,
                                    pstPrefPlmnOperate->usRat,
                                    &(pstPrefPlmnOperate->stPlmn));
       if (TAF_PARA_OK != ulRet)
       {
           Taf_ParaSetResult(ClientId,
                             OpId,
                             (VOS_UINT8)ulRet,
                             TAF_PH_SET_PERF_PLMN_PARA,
                             VOS_NULL_PTR);
       }
    }
}


VOS_UINT16 TAF_MMA_GetSpecificPlmnTblSize(
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType
)
{
    VOS_UINT16                                              usCurPlmnNum;
    NAS_MML_SIM_OPERPLMN_INFO_STRU                         *pstOperPlmnList = VOS_NULL_PTR;
    NAS_MML_SIM_USERPLMN_INFO_STRU                         *pstUserPlmnList = VOS_NULL_PTR;
    NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU                   *pstHplmnWithRat = VOS_NULL_PTR;

    usCurPlmnNum = 0;
    switch(enPrefPLMNType)
    {
        case MN_PH_PREF_PLMN_UPLMN:
            pstUserPlmnList = NAS_MML_GetSimUserPlmnList();
            usCurPlmnNum    = pstUserPlmnList->ucUserPlmnNum;
            break;
        case MN_PH_PREF_PLMN_OPLMN:
            pstOperPlmnList = NAS_MML_GetSimOperPlmnList();
            usCurPlmnNum    = pstOperPlmnList->usOperPlmnNum;
            break;
        case MN_PH_PREF_PLMN_HPLMN:
            pstHplmnWithRat = NAS_MML_GetSimHplmnWithRatList();
            usCurPlmnNum    = pstHplmnWithRat->ucHPlmnWithRatNum;
            break;
        default:
            break;
    }

    return usCurPlmnNum;
}


VOS_VOID TAF_MMA_RcvTafMsgCpolQueryReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CPOL_INFO_QUERY_REQ_STRU   *pstCpolInfo
)
{
    TAF_MMA_CPOL_INFO_QUERY_CNF_STRU   *pstCpolQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulPlmnNum;
    VOS_UINT32                          ulRemainPlmnNum;
    VOS_UINT32                          ulTotal;

    /*
        设置分组结束标志和分组上报运营商个数: 待发送的运营商数少于一条消息中
        允许发送的运营商数，则设置结束标志并发送所有待发送运营商信息；
    */
    ulTotal             = TAF_MMA_GetSpecificPlmnTblSize(pstCpolInfo->enPrefPLMNType);

    if (pstCpolInfo->ulFromIndex > ulTotal)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL_PTR, PS_PRINT_WARNING,
                "TAF_MMA_RcvTafMsgCpolQueryReq: abnormal request!");
        ulPlmnNum = 0;
    }
    else
    {
        ulRemainPlmnNum = ulTotal - (pstCpolInfo->ulFromIndex);

        if (ulRemainPlmnNum < pstCpolInfo->ulPlmnNum)
        {
            ulPlmnNum   = ulRemainPlmnNum;
        }
        else
        {
            ulPlmnNum   = pstCpolInfo->ulPlmnNum;
        }
    }

    /* 以TAF_PH_COPN_PLMN_MAX_NUM个运营商为一组分组上报运营商信息 */
    pstCpolQryCnf  = (TAF_MMA_CPOL_INFO_QUERY_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                     WUEPS_PID_MMA,
                                     sizeof(TAF_MMA_CPOL_INFO_QUERY_CNF_STRU));

    if (VOS_NULL_PTR == pstCpolQryCnf)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL_PTR, PS_PRINT_ERROR,
                "TAF_MMA_RcvTafMsgCpolQueryReq: Memory Alloc Error for pMsg!");
        return;
    }

    /* 初始化消息 */
    PS_MEM_SET(&pstCpolQryCnf->stPlmnName,
                0x00,
               sizeof(pstCpolQryCnf->stPlmnName));

    /* 填写消息头部信息 */
    pstCpolQryCnf->ulReceiverPid          = WUEPS_PID_AT;
    pstCpolQryCnf->ulMsgName              = AT_MMA_CPOL_INFO_QUERY_CNF;
    pstCpolQryCnf->stAtAppCtrl.usClientId = usClientId;
    pstCpolQryCnf->stAtAppCtrl.ucOpId     = ucOpId;
    pstCpolQryCnf->ulFromIndex            = pstCpolInfo->ulFromIndex;
    pstCpolQryCnf->ulValidPlmnNum         = pstCpolInfo->ulValidPlmnNum;

    /*
    从ulFromIndex开始将PLMN信息填写到pstCpolQryCnf->stPlmnName结构数组，
    PLMN个数为pstCpolQryCnf->ulPlmnNum
    */
    pstCpolQryCnf->stPlmnName.bPlmnSelFlg = MN_PH_IsUsingPlmnSel();

    MMA_QryPrefPlmnName(pstCpolInfo->enPrefPLMNType,
                        pstCpolInfo->ulFromIndex,
                        ulPlmnNum,
                        &pstCpolQryCnf->stPlmnName);

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMA, pstCpolQryCnf))
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL_PTR, PS_PRINT_ERROR,
                "TAF_MMA_RcvTafMsgCpolQueryReq: Send Msg Error!");
    }

    return;

}


VOS_BOOL TAF_MMA_IsSimLockPlmnInfoValid(VOS_VOID)
{
    TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU  stSimLockPlmnInfo;
    VOS_UINT32                          i;

    PS_MEM_SET(&stSimLockPlmnInfo, 0x00, sizeof(stSimLockPlmnInfo));

    if (NV_OK != NV_Read(en_NV_Item_CustomizeSimLockPlmnInfo,
                       &stSimLockPlmnInfo,
                       sizeof(stSimLockPlmnInfo)))
    {
        return VOS_FALSE;
    }

    if (VOS_FALSE == stSimLockPlmnInfo.ulStatus)
    {
        return VOS_FALSE;
    }

    for ( i = 0; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++ )
    {
        if (0xAA == stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin[0])
        {
            continue;
        }

        break;
    }

    /* 如果每一项的PLMN都不是有效的，那么返回没有有效的PLMN信息 */
    if (TAF_MAX_SIM_LOCK_RANGE_NUM == i)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_VOID TAF_MMA_RcvSimLockQry(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId
)
{
    TAF_CUSTOM_CARDLOCK_STATUS_STRU     stCardLockStatus;
    VOS_UINT32                          ulRet;
    AT_MMA_SIMLOCK_STATUS_STRU         *pstEvent;


    PS_MEM_SET(&stCardLockStatus, 0x00, sizeof(stCardLockStatus));

    pstEvent = (AT_MMA_SIMLOCK_STATUS_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(AT_MMA_SIMLOCK_STATUS_STRU));
    if (VOS_NULL_PTR == pstEvent)
    {
        MN_WARN_LOG("TAF_MMA_RcvSimLockQry: read sim lock status fail");
        return;
    }

    pstEvent->ulReceiverPid          = WUEPS_PID_AT;
    pstEvent->ulMsgName              = AT_MMA_SIMLOCK_STAUS_QUERY_CNF;
    pstEvent->stAtAppCtrl.usClientId = MN_GetRealClientId(ClientId, WUEPS_PID_MMA);;
    pstEvent->stAtAppCtrl.ucOpId     = OpId;

    ulRet = NV_Read(en_NV_Item_CardlockStatus,
                    &stCardLockStatus,
                    (VOS_UINT32)(sizeof(stCardLockStatus)));
    if (NV_OK != ulRet)
    {
        pstEvent->bSimlockEnableFlg = VOS_TRUE;
    }
    else
    {
        if ((VOS_TRUE == TAF_MMA_IsSimLockPlmnInfoValid())
         && (NV_ITEM_ACTIVE == stCardLockStatus.ulStatus)
         && (TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE != stCardLockStatus.enCardlockStatus))
        {
            pstEvent->bSimlockEnableFlg = VOS_TRUE;
        }
        else
        {
            pstEvent->bSimlockEnableFlg = VOS_FALSE;
        }
    }

    /* 发送消息 */
    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pstEvent);
    if( VOS_OK != ulRet)
    {
        MMA_WARNINGLOG("TAF_MMA_RcvSimLockQry: Send Msg Error");
    }

    return;
}
VOS_VOID TAF_MMA_UpdateSimlockStatus(VOS_BOOL bUnlockOK)
{

    VOS_UINT32                          ulResult;
    TAF_CUSTOM_CARDLOCK_STATUS_STRU     stCardLockStatus;
    TAF_CUSTOM_SIM_LOCK_MAX_TIMES_STRU  stSimLockMaxTimes;


    stSimLockMaxTimes.ulStatus       = NV_ITEM_DEACTIVE;
    stSimLockMaxTimes.ulLockMaxTimes = TAF_PH_CARDLOCK_DEFAULT_MAXTIME;

    /* 获取当前最大解锁次数 */
    ulResult = NV_Read(en_NV_Item_CustomizeSimLockMaxTimes,
                      &stSimLockMaxTimes,
                      sizeof(stSimLockMaxTimes));
    if (NV_OK != ulResult)
    {
        MMA_WARNINGLOG("TAF_MMA_UpdateSimlockStatus: Fail to read en_NV_Item_CustomizeSimLockMaxTimes.");
        stSimLockMaxTimes.ulLockMaxTimes = TAF_PH_CARDLOCK_DEFAULT_MAXTIME;
    }
    else
    {
        if (NV_ITEM_DEACTIVE == stSimLockMaxTimes.ulStatus)
        {
            stSimLockMaxTimes.ulLockMaxTimes = TAF_PH_CARDLOCK_DEFAULT_MAXTIME;
        }
    }

    /* 获取NV项en_NV_Item_CardlockStatus中的锁卡数据 */
    if (NV_OK != NV_Read(en_NV_Item_CardlockStatus, &stCardLockStatus, sizeof(stCardLockStatus)))
    {
        MMA_WARNINGLOG("TAF_MMA_UpdateSimlockStatus: Not read sim lock status");
        stCardLockStatus.ulRemainUnlockTimes = 0;
    }

    if (VOS_TRUE == bUnlockOK)
    {
        /* Simlock校验通过，将永久解除锁卡状态  */
        stCardLockStatus.ulStatus            = NV_ITEM_ACTIVE;
        stCardLockStatus.enCardlockStatus    = TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE;
        stCardLockStatus.ulRemainUnlockTimes = stSimLockMaxTimes.ulLockMaxTimes;
    }
    else
    {
        stCardLockStatus.ulStatus            = NV_ITEM_ACTIVE;
        stCardLockStatus.enCardlockStatus    = TAF_OPERATOR_LOCK_LOCKED;
    }

    ulResult = NV_Write(en_NV_Item_CardlockStatus,
                        &stCardLockStatus,
                        sizeof(stCardLockStatus));
    if (NV_OK != ulResult)
    {
        MMA_WARNINGLOG("TAF_MMA_UpdateSimlockStatus: write en_NV_Item_CardlockStatus Fail.");
        return;
    }

    ulResult = NV_SpecialNvIdBackup(en_NV_Item_CardlockStatus,
                                    &stCardLockStatus,
                                    sizeof(stCardLockStatus));
    if (NV_OK != ulResult)
    {
        MMA_WARNINGLOG("TAF_MMA_UpdateSimlockStatus:ERROR:write special en_NV_Item_CardlockStatus Fail.");
        return;
    }


    return;

}



VOS_UINT32 MMA_SaveWCipherInfo(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_UMTS_CIPHER_INFO_IND_STRU  *pstCipherInfoInd;

    pstCipherInfoInd = (MMC_MMA_UMTS_CIPHER_INFO_IND_STRU*)pstMsg;

    /* 将MMC发来的Cipher信息保存到全局变量 */
    g_stMmaCipherInfo.enCipherAlgorForCSDomain = pstCipherInfoInd->ucCipherAlgorForCSDomain;
    g_stMmaCipherInfo.enCipherAlgorForPSDomain = pstCipherInfoInd->ucCipherAlgorForPSDomain;

    return VOS_TRUE;
}
VOS_UINT32 MMA_SaveGCipherInfo(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_GPRS_CIPHER_INFO_IND_STRU  *pstCipherInfoInd;
    pstCipherInfoInd = (MMC_MMA_GPRS_CIPHER_INFO_IND_STRU*)pstMsg;

    /* 将MMC发来的Cipher信息保存到全局变量 */
    g_stMmaCipherInfo.ucGprsCipher             = pstCipherInfoInd->ucGprsCipher;
    g_stMmaCipherInfo.ucGprsCipherAlg          = pstCipherInfoInd->ucGprsCipherAlg;

    return VOS_TRUE;
}
VOS_VOID MMA_QryCipherInfo(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    MN_MMA_CIPHER_QRY_REQ_STRU         *pstInfo
)
{
    MN_MMA_CIPHER_QRY_CNF_STRU         *pstCipherQryCnf;
    VOS_UINT8                           ucCipherInfo;
    TAF_SDC_SYS_MODE_ENUM_UINT8         enRadioMode;
    VOS_UINT32                          ulRst;

    /* 初始化参数 */
    ucCipherInfo    = MMA_UMTS_UEA0;
    pstCipherQryCnf = VOS_NULL_PTR;
    ulRst           = VOS_NULL;

    /*获取当前接入技术, 调用API*/
    enRadioMode = TAF_SDC_GetSysMode();

    if (TAF_SDC_SYS_MODE_GSM == enRadioMode)
    {
        /* 获取G模下的PS域加密信息 */
        if (VOS_TRUE == g_stMmaCipherInfo.ucGprsCipher)
        {
            ucCipherInfo = g_stMmaCipherInfo.ucGprsCipherAlg;
        }
        else
        {
            ucCipherInfo = MMA_UMTS_UEA0;
        }
    }
    else
    {
        /* 获取W模下的PS域加密信息 */
        switch(g_stMmaCipherInfo.enCipherAlgorForPSDomain)
        {
            case MMA_CIPHER_ALGOR_UEA0:
                ucCipherInfo = MMA_UMTS_UEA0;
                break;
            case MMA_CIPHER_ALGOR_UEA1:
                ucCipherInfo = MMA_UMTS_UEA1;
                break;
            case MMA_CIPHER_ALGOR_UEA2:
                ucCipherInfo = MMA_UMTS_UEA2;
                break;
            default:
                ucCipherInfo = MMA_UMTS_UEA0;
                break;
        }
    }

    pstCipherQryCnf = (MN_MMA_CIPHER_QRY_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                     WUEPS_PID_MMA,
                                                     sizeof(MN_MMA_CIPHER_QRY_CNF_STRU));

    if( VOS_NULL_PTR == pstCipherQryCnf )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "MMA_QryCipherInfo:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR*)pstCipherQryCnf + VOS_MSG_HEAD_LENGTH,
           0x00,
           sizeof(MN_MMA_CIPHER_QRY_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填写消息 */
    pstCipherQryCnf->ulReceiverPid          = WUEPS_PID_AT;
    pstCipherQryCnf->ulMsgName              = AT_MMA_CIPHER_INFO_QUERY_CNF;
    pstCipherQryCnf->stAtAppCtrl.usClientId = MN_GetRealClientId(usClientId, WUEPS_PID_MMA);
    pstCipherQryCnf->stAtAppCtrl.ucOpId     = ucOpId;
    pstCipherQryCnf->ucCipherInfo           = ucCipherInfo;
    pstCipherQryCnf->ulResult               = VOS_OK;

    /* 发送消息 */
    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstCipherQryCnf);
    if( VOS_OK != ulRst)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR, "MMA_QryCipherInfo:ERROR: Send Msg Error!");
    }

    return;
}
VOS_UINT32 MN_PH_QryLocinfo (
    VOS_UINT32                         *pulMcc,
    VOS_UINT32                         *pulMnc,
    VOS_UINT16                         *pusLac,
    VOS_UINT8                          *pucRac,
    VOS_UINT32                         *pulCellId
)
{
    TAF_SDC_PLMN_ID_STRU                *pstPlmnId = VOS_NULL_PTR;

    if ( TAF_REPORT_SRVSTA_NO_SERVICE == TAF_SDC_GetServiceStatus() )
    {
        /* 无服务状态时的处理*/
        (*pulMcc)       = 0;
        (*pulMnc)       = 0;
        (*pusLac)       = 0;
        (*pucRac)       = 0;
        (*pulCellId)    = 0;

        return VOS_OK;
    }

    /* 获取当前网络的PLMN、LAC、RAC、CellId值 */
    pstPlmnId            = TAF_SDC_GetCurrCampPlmnId();
    *pulMcc              = pstPlmnId->ulMcc;
    *pulMnc              = pstPlmnId->ulMnc;
    *pusLac              = TAF_SDC_GetCurrCampLac();
    *pucRac              = TAF_SDC_GetCurrCampRac();
    *pulCellId           = TAF_SDC_GetCurrCampCellId();


    return VOS_OK;
}


VOS_VOID MMA_QryLocInfo(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    MN_MMA_LOCATION_INFO_REQ_STRU      *pstInfo
)
{
    MN_MMA_LOCATION_INFO_CNF_STRU      *pstLocInfoCnf;
    VOS_UINT32                          ulRst;

    /* 初始化参数 */
    pstLocInfoCnf   = VOS_NULL_PTR;
    ulRst           = VOS_NULL;

    pstLocInfoCnf   = (MN_MMA_LOCATION_INFO_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                     WUEPS_PID_MMA,
                                                     sizeof(MN_MMA_LOCATION_INFO_CNF_STRU));

    if( VOS_NULL_PTR == pstLocInfoCnf )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "MMA_QryLocInfo:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR*)pstLocInfoCnf + VOS_MSG_HEAD_LENGTH,
           0x00,
           sizeof(MN_MMA_LOCATION_INFO_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填写消息 */
    pstLocInfoCnf->ulReceiverPid          = WUEPS_PID_AT;
    pstLocInfoCnf->ulMsgName              = AT_MMA_LOCATION_INFO_QUERY_CNF;
    pstLocInfoCnf->stAtAppCtrl.usClientId = MN_GetRealClientId(usClientId, WUEPS_PID_MMA);
    pstLocInfoCnf->stAtAppCtrl.ucOpId     = ucOpId;

    /*调用MN_PH_QryLocinfo查询各参数*/
    pstLocInfoCnf->ulResult = MN_PH_QryLocinfo (&pstLocInfoCnf->ulMcc,
                                                &pstLocInfoCnf->ulMnc,
                                                &pstLocInfoCnf->usLac,
                                                &pstLocInfoCnf->ucRac,
                                                &pstLocInfoCnf->ulCellid);
    /* 发送消息 */
    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstLocInfoCnf);
    if( VOS_OK != ulRst)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR, "MMA_QryLocInfo:ERROR: Send Msg Error!");
    }

    return;
}
VOS_VOID TAF_MMA_GetUsimHplmnMncLenFileInd(PS_USIM_GET_FILE_CNF_STRU *pUsimTafMsg)
{
    TAF_HPLMN_WITH_MNC_LEN_STRU         stHplmnWithMncLen;
    VOS_UINT32                          ulResult;

    /* 卡长度无效 */
    if ( pUsimTafMsg->usEfLen <= 3 )
    {
        stHplmnWithMncLen.ucHplmnMncLen = MN_MMA_MNC_LENGTH_THREE_BYTES_IN_IMSI;
    }
    else
    {
        /* 获取MNC长度 */
        if (( MN_MMA_MNC_LENGTH_TWO_BYTES_IN_IMSI == (pUsimTafMsg->aucEf[MN_MMA_MNC_LENGTH_INDICATOR_POSITION-1] & MN_MMA_OCTET_LOW_FOUR_BITS) )
         || ( MN_MMA_MNC_LENGTH_THREE_BYTES_IN_IMSI == (pUsimTafMsg->aucEf[MN_MMA_MNC_LENGTH_INDICATOR_POSITION-1] & MN_MMA_OCTET_LOW_FOUR_BITS) ))
        {
            stHplmnWithMncLen.ucHplmnMncLen = pUsimTafMsg->aucEf[MN_MMA_MNC_LENGTH_INDICATOR_POSITION-1] & MN_MMA_OCTET_LOW_FOUR_BITS;
        }
        else
        {
            stHplmnWithMncLen.ucHplmnMncLen = MN_MMA_MNC_LENGTH_THREE_BYTES_IN_IMSI;
        }
    }

    /* 获取HPLMN */
    ulResult = MN_PH_QryApHplmn(&stHplmnWithMncLen.stHplmn.Mcc, &stHplmnWithMncLen.stHplmn.Mnc);

    /* 上报AT^APHPLMN参数查询结果事件 */
    if (VOS_OK != ulResult)
    {
        MMA_ParaQueryReport(gstMmaValue.stGetHplmn.usClientId,
                            gstMmaValue.stGetHplmn.ucOpId,
                            TAF_PH_HOMEPLMN_VALUE_PARA,
                            TAF_ERR_ERROR,
                            VOS_NULL_PTR);
    }
    else
    {
        MMA_ParaQueryReport(gstMmaValue.stGetHplmn.usClientId,
                            gstMmaValue.stGetHplmn.ucOpId,
                            TAF_PH_HOMEPLMN_VALUE_PARA,
                            TAF_ERR_NO_ERROR,
                            (VOS_VOID*)(&stHplmnWithMncLen));
    }

    /*停止定时器*/
    MMA_StopInternalTimer();

    return;
}
VOS_UINT32 TAF_MMA_RcvOamConnectStatusInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 设置SDT连接状态为 已连接 */
    g_ucMmaOmConnectFlg = VOS_TRUE;

    /* INIT完成 且 已使能则发送NAS回放消息 */
    if( (VOS_TRUE       == g_ucMmaOmConnectFlg)
     && (VOS_TRUE       == g_ucMmaOmPcRecurEnableFlg) )
    {
        NAS_MMA_SndOutsideContextData();
        Api_AppMmLayerContextSndoutForPcReplay();
    }

    /* 转发SDT的连接状态及SDT设置的PC回放使能信息到其它组件 */
    MMA_ForwardOmMaintainInfoInd(WUEPS_PID_TAF,
                                 MMCM_OM_MAINTAIN_INFO_IND,
                                 g_ucMmaOmConnectFlg,
                                 g_ucMmaOmPcRecurEnableFlg);

    MMA_ForwardOmMaintainInfoInd(WUEPS_PID_SMS,
                                 MMCM_OM_MAINTAIN_INFO_IND,
                                 g_ucMmaOmConnectFlg,
                                 g_ucMmaOmPcRecurEnableFlg);

    MMA_SndAtOmMaintainInfoInd(g_ucMmaOmConnectFlg,
                               g_ucMmaOmPcRecurEnableFlg);

    /* 转发SDT的连接状态及SDT设置的PC回放使能信息到MMC */
    TAF_MMA_SndMmcOmMaintainInfoInd(g_ucMmaOmConnectFlg, g_ucMmaOmPcRecurEnableFlg);

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_RcvOamTraceCfgReq(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_OAM_TRACE_CFG_IND_STRU         *pstTraceCfgInd;
    VOS_UINT8                           ucPcRecurEnableFlg;

    pstTraceCfgInd = (TAF_OAM_TRACE_CFG_IND_STRU *)(pstMsg);

    if (TAF_OAM_PC_RECUR_CFG_ENABLE == pstTraceCfgInd->stTraceCfg.enPcRecurCfgFlg)
    {
        ucPcRecurEnableFlg = VOS_TRUE;
    }
    else
    {
        ucPcRecurEnableFlg = VOS_FALSE;
    }

    /* 保存是否使能发送NAS的PC回放消息的标志 */
    g_ucMmaOmPcRecurEnableFlg = ucPcRecurEnableFlg;

    MMA_ForwardOmMaintainInfoInd(WUEPS_PID_TAF,
                                 MMCM_OM_MAINTAIN_INFO_IND,
                                 g_ucMmaOmConnectFlg,
                                 g_ucMmaOmPcRecurEnableFlg);

    MMA_ForwardOmMaintainInfoInd(WUEPS_PID_SMS,
                                 MMCM_OM_MAINTAIN_INFO_IND,
                                 g_ucMmaOmConnectFlg,
                                 g_ucMmaOmPcRecurEnableFlg);

    MMA_SndAtOmMaintainInfoInd(g_ucMmaOmConnectFlg,
                               g_ucMmaOmPcRecurEnableFlg);

    /* 转发PC回放开关信息到 MMC */
    TAF_MMA_SndMmcOmMaintainInfoInd(g_ucMmaOmConnectFlg, ucPcRecurEnableFlg);

    return VOS_TRUE;
}




VOS_UINT32 TAF_MMA_ReadCustomerServiceProfileFile_SwitchOn(VOS_VOID)
{
    USIMM_GET_FILE_INFO_STRU            stGetFileInfo;

    /* 网络选择菜单控制功能未使能 */
    if (VOS_TRUE != TAF_MMA_GetNetworkSelectionMenuEnableFlg())
    {
        return VOS_ERR;
    }

    /* 该文件受PIN码保护，只有卡状态为AVAILBALE时才读取，否则直接返回VOS_ERR*/
    if (MMA_NONEED_PIN1 != MMA_CheckPin1SatusSimple())
    {
        return VOS_ERR;
    }

    /* 读6F15文件，该文件类型为transparent，所以记录号为0 */
    TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                                    USIMM_UNLIMIT_APP,
                                    TAF_MMA_CUSTOMER_SERVICE_PROFILE_FILE_ID,
                                    0);

    if (USIMM_API_SUCCESS == NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA,
                                              0,
                                              &stGetFileInfo))
    {
        return VOS_OK;
    }
    else
    {
        return VOS_ERR;
    }
}
VOS_VOID TAF_MMA_TransferRatMode(
    TAF_MMA_RAT_MODE_ENUM_U8            enRatMode,
    TAF_PH_RAT_ORDER_STRU              *pstRatModeOrder
)
{
    switch(enRatMode)
    {
        case TAF_MMA_RAT_MODE_ENUM_DAUL_MODE:
            pstRatModeOrder->ucRatOrderNum  = TAF_PH_MAX_GU_RAT_NUM;
            pstRatModeOrder->aenRatOrder[0] = TAF_PH_RAT_WCDMA;
            pstRatModeOrder->aenRatOrder[1] = TAF_PH_RAT_GSM;
            pstRatModeOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            break;

        case TAF_MMA_RAT_MODE_ENUM_2G_ONLY:
            pstRatModeOrder->ucRatOrderNum  = 1;
            pstRatModeOrder->aenRatOrder[0] = TAF_PH_RAT_GSM;
            pstRatModeOrder->aenRatOrder[1] = TAF_PH_RAT_BUTT;
            pstRatModeOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            break;

        case TAF_MMA_RAT_MODE_ENUM_3G_ONLY:
            pstRatModeOrder->ucRatOrderNum  = 1;
            pstRatModeOrder->aenRatOrder[0] = TAF_PH_RAT_WCDMA;
            pstRatModeOrder->aenRatOrder[1] = TAF_PH_RAT_BUTT;
            pstRatModeOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case TAF_MMA_RAT_MODE_ENUM_TRI_MODE:
            pstRatModeOrder->ucRatOrderNum  = TAF_PH_MAX_GUL_RAT_NUM;
            pstRatModeOrder->aenRatOrder[0] = TAF_PH_RAT_LTE;
            pstRatModeOrder->aenRatOrder[1] = TAF_PH_RAT_WCDMA;
            pstRatModeOrder->aenRatOrder[2] = TAF_PH_RAT_GSM;
            break;

        case TAF_MMA_RAT_MODE_ENUM_LTE_ONLY:
            pstRatModeOrder->ucRatOrderNum  = 1;
            pstRatModeOrder->aenRatOrder[0] = TAF_PH_RAT_LTE;
            pstRatModeOrder->aenRatOrder[1] = TAF_PH_RAT_BUTT;
            pstRatModeOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;

            break;
#endif

        default:
#if (FEATURE_ON == FEATURE_LTE)
            pstRatModeOrder->ucRatOrderNum  = TAF_PH_MAX_GUL_RAT_NUM;
            pstRatModeOrder->aenRatOrder[0] = TAF_PH_RAT_LTE;
            pstRatModeOrder->aenRatOrder[1] = TAF_PH_RAT_WCDMA;
            pstRatModeOrder->aenRatOrder[2] = TAF_PH_RAT_GSM;
#else
            pstRatModeOrder->ucRatOrderNum  = TAF_PH_MAX_GU_RAT_NUM;
            pstRatModeOrder->aenRatOrder[0] = TAF_PH_RAT_WCDMA;
            pstRatModeOrder->aenRatOrder[1] = TAF_PH_RAT_GSM;
            pstRatModeOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
#endif
            break;
    }

    /* 删除平台不支持的接入技术 */
    TAF_MMA_DelPlatformUnsupportedRat(pstRatModeOrder);

    /* 如果所有接入技术平台都不支持，使用平台默认值 */
    if (0 == pstRatModeOrder->ucRatOrderNum)
    {
        MN_MMA_SetDefaultRatPrioList(pstRatModeOrder);
    }

    return;
}

VOS_UINT32 TAF_MMA_ReadRatModeFile(VOS_VOID)
{
    VOS_UINT32                          ulEfLen;
    VOS_UINT8                          *pucEf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucSimType;
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList = VOS_NULL_PTR;


    ulEfLen   = 0;
    ucSimType = USIMM_CARD_NOCARD;

    pstRatPrioList = MN_MMA_GetRatPrioListAddr();

    /* 接入技术平衡控制功能未使能 */
    if (VOS_TRUE != TAF_MMA_GetRatBalancingEnableFlg())
    {
        return VOS_ERR;
    }

    NAS_USIMMAPI_GetCardType(VOS_NULL_PTR, &ucSimType);

    if (USIMM_CARD_USIM == ucSimType)
    {
        ulResult = NAS_USIMMAPI_GetCachedFile(TAF_MMA_RAT_MODE_FILE_ID, &ulEfLen, &pucEf, USIMM_ATT_APP);

        /* 读文件失败 */
        if (USIMM_API_SUCCESS != ulResult)
        {
            /* 使用默认值 */
            MN_MMA_SetDefaultRatPrioList(pstRatPrioList);
        }
        /* 读文件成功 */
        else
        {
            TAF_MMA_SndOmGetCacheFile(TAF_MMA_RAT_MODE_FILE_ID, ulEfLen, pucEf);

            if (ulEfLen < TAF_MMA_RAT_MODE_FILE_LEN)
            {
                /* 使用默认值 */
                MN_MMA_SetDefaultRatPrioList(pstRatPrioList);
            }
            else
            {
                TAF_MMA_TransferRatMode(pucEf[0], pstRatPrioList);
            }
        }
    }
    else
    {
        /* 非USIM卡，直接设置成DUAL MODE*/
        TAF_MMA_TransferRatMode(TAF_MMA_RAT_MODE_ENUM_DAUL_MODE, pstRatPrioList);
    }

    /* 更新gstMmaValue.pg_StatusContext.stRatPrioList全局变量 */
    PS_MEM_CPY(&gstMmaValue.pg_StatusContext->stRatPrioList, pstRatPrioList, sizeof(TAF_PH_RAT_ORDER_STRU));

    return VOS_OK;
}
VOS_VOID TAF_MMA_SetPlmnSelectionMode(VOS_UINT8 ucPlmnSelMode)
{
    g_StatusContext.ucPlmnSelMode = ucPlmnSelMode;
}


VOS_UINT8 TAF_MMA_GetPlmnSelectionMode(VOS_VOID)
{
    return g_StatusContext.ucPlmnSelMode;
}


VOS_VOID TAF_MMA_SetNetworkSelectionMenuEnableFlg(VOS_UINT8 ucEnableFlg)
{
    g_StatusContext.stNetSelMenuCtx.ucNetSelMenuFlg = ucEnableFlg;
}


VOS_UINT8 TAF_MMA_GetNetworkSelectionMenuEnableFlg(VOS_VOID)
{
    return g_StatusContext.stNetSelMenuCtx.ucNetSelMenuFlg;
}


VOS_VOID TAF_MMA_SetPlmnMode_Refresh(TAF_MMA_PLMN_MODE_ENUM_UINT8 enPlmnMode)
{
    g_StatusContext.stNetSelMenuCtx.enPlmnMode = enPlmnMode;
}


TAF_MMA_PLMN_MODE_ENUM_UINT8 TAF_MMA_GetPlmnMode_Refresh(VOS_VOID)
{
    return g_StatusContext.stNetSelMenuCtx.enPlmnMode;
}


VOS_VOID TAF_MMA_SetAutoPlmnSelUser(TAF_MMA_AUTO_PLMN_SEL_USER_ENUM_UINT8 enAutoPlmnSelUser)
{
    g_StatusContext.stNetSelMenuCtx.enAutoPlmnSelUser = enAutoPlmnSelUser;
}


TAF_MMA_AUTO_PLMN_SEL_USER_ENUM_UINT8 TAF_MMA_GetAutoPlmnSelUser(VOS_VOID)
{
    return g_StatusContext.stNetSelMenuCtx.enAutoPlmnSelUser;
}


VOS_UINT8 TAF_MMA_GetTiTryingPeriodNetSelMenuStatus(VOS_VOID)
{
    return g_StatusContext.stNetSelMenuCtx.stTiPeriodTryingNetSelMenu.ucTimerStatus;
}


VOS_VOID TAF_MMA_SetTiTryingPeriodNetSelMenuStatus(VOS_UINT8 ucStatus)
{
    g_StatusContext.stNetSelMenuCtx.stTiPeriodTryingNetSelMenu.ucTimerStatus = ucStatus;
}


VOS_VOID TAF_MMA_SetRatBalancingEnableFlg(VOS_UINT8 ucEnableFlg)
{
    g_StatusContext.stRatBalancingCtx.ucRatBalancingFlg = ucEnableFlg;
}


VOS_UINT8 TAF_MMA_GetRatBalancingEnableFlg(VOS_VOID)
{
    return g_StatusContext.stRatBalancingCtx.ucRatBalancingFlg;
}


VOS_VOID TAF_MMA_SetRatMode_Refresh(TAF_MMA_RAT_MODE_ENUM_U8 enRatMode)
{
    g_StatusContext.stRatBalancingCtx.enRatMode = enRatMode;
}


TAF_MMA_RAT_MODE_ENUM_U8 TAF_MMA_GetRatMode_Refresh(VOS_VOID)
{
    return g_StatusContext.stRatBalancingCtx.enRatMode;
}


VOS_VOID TAF_MMA_SetSyscfgUser(TAF_MMA_SYSCFG_USER_ENUM_UINT8 enSyscfgUser)
{
    g_StatusContext.stRatBalancingCtx.enSyscfgUser = enSyscfgUser;
}


TAF_MMA_SYSCFG_USER_ENUM_UINT8 TAF_MMA_GetSyscfgUser(VOS_VOID)
{
    return g_StatusContext.stRatBalancingCtx.enSyscfgUser;
}


VOS_UINT8 TAF_MMA_GetTiTryingPeriodRatBalancingStatus(VOS_VOID)
{
    return g_StatusContext.stRatBalancingCtx.stTiPeriodTryingRatBalancing.ucTimerStatus;
}


VOS_VOID TAF_MMA_SetTiTryingPeriodRatBalancingStatus(VOS_UINT8 ucStatus)
{
    g_StatusContext.stRatBalancingCtx.stTiPeriodTryingRatBalancing.ucTimerStatus = ucStatus;
}


VOS_UINT32 TAF_MMA_GetPlmnModeOfCustomerServiceProfileFile(
    VOS_UINT16                          usEfLen,
    VOS_UINT8                          *pucEf,
    TAF_MMA_PLMN_MODE_ENUM_UINT8       *penPlmnMode
)
{
    VOS_UINT16                          usIndex;
    VOS_UINT16                          usIndexMax;

    /* 6F15文件格式
    |Byte
    |---|---|---|---|--------|-------|--------|
    |1  |2  |3  |4  |.. .. ..| X-1   | X      |
    |---|---|---|---|--------|-------|--------|
    |A1 |B1 |A2 |B2 |.. .. ..| A(X/2)| B(X/2) |
    |---|---|---|---|--------|-------|--------|

    Byte A   Byte B
    <hex>    <----------------------------------------binary--------------------------------------->

    |--------|-----------|------|-----|--------|-------|------|------|----------|----------|
    |Service |           |      |     |        |       |      |      |          |          |
    |Group   |           |      |     |        |       |      |      |          |          |
    |Code    | Service   | Bit8 | Bit7| Bit6   | Bit5  | Bit4 | Bit3 | Bit2     | Bit1     |
    |--------|-----------|------|-----|--------|-------|------|------|----------|----------|
    |C0      | ValueAdded| PLMN | VPS | SM/MO  | SM/MO | Fax  | Data | Reserved | Language |
    |        | Services  | MODE |     | PAGING | EMAIL |      |      |          |          |
    |--------|-----------|------|-----|--------|-------|------|------|----------|----------|

    A*和B*为一个组合，A*表示service group，B*的每个bit表示一个service
    plmn mode位: A*为C0，B*的Bit8
    */

    usIndexMax = usEfLen;

    /* 文件长度不为偶数，即最后一个GROUP只有A*没有B*，所以不需要匹配最后一个A* */
    if (0 != (usEfLen % TAF_MMA_GROUP_LEN_IN_CUSTOMER_SERVICE_PROFILE_FILE))
    {
        usIndexMax = usEfLen - 1;
        MMA_WARNINGLOG("TAF_MMA_GetPlmnModeOfCustomerServiceProfileFile: file content is invalid");
    }

    /* 查找PLMN MODE位 */
    for (usIndex = 0; usIndex < usIndexMax; usIndex = (usIndex + TAF_MMA_GROUP_LEN_IN_CUSTOMER_SERVICE_PROFILE_FILE))
    {
        if (TAF_MMA_SERVICE_GROUP_CODE_IN_CUSTOMER_SERVICE_PROFILE_FILE == pucEf[usIndex])
        {
            /* Bit8表示PLMN MODE*/
            *penPlmnMode
                = (pucEf[usIndex + 1] & TAF_MMA_PLMN_MODE_IN_SERVICE_GROUP_CODE) >> TAF_MMA_PLMN_MODE_OFFSET_IN_SERVICE_GROUP_CODE;

            break;
        }
    }

    /* PLMN MODE位不存在，返回VOS_ERR */
    if (usIndex >= usIndexMax)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_RcvCustomerServiceProfileFile_PreProc(
    VOS_UINT16                          usEfLen,
    VOS_UINT8                          *pucEf
)
{
    TAF_MMA_PLMN_MODE_ENUM_UINT8        enPlmnMode;

    /* 网络选择菜单控制功能未使能，返回VOS_TRUE */
    if (VOS_FALSE == TAF_MMA_GetNetworkSelectionMenuEnableFlg())
    {
        MMA_WARNINGLOG("TAF_MMA_RcvCustomerServiceProfileFile_PreProc: NV is deactive");
        return VOS_TRUE;
    }

    /* 未获取PLMN MODE位，返回VOS_TRUE */
    if (VOS_OK != TAF_MMA_GetPlmnModeOfCustomerServiceProfileFile(usEfLen, pucEf, &enPlmnMode))
    {
        MMA_ERRORLOG("TAF_MMA_RcvCustomerServiceProfileFile_PreProc: No Plmn Mode");
        return VOS_TRUE;
    }

    /* 调用TAF_MMA_ReportPlmnModeEvent上报TAF_PH_EVT_NSM_STATUS_IND事件 */
    TAF_MMA_ReportPlmnModeEvent(enPlmnMode);

    /* 调用TAF_MMA_SetPlmnMode_Refresh 保存PLMN MODE */
    TAF_MMA_SetPlmnMode_Refresh(enPlmnMode);

    /* PLMN MODE 位1，返回VOS_TRUE */
    if (TAF_MMA_PLMN_MODE_ENUM_ENABLE == enPlmnMode)
    {
        return VOS_TRUE;
    }

    /* 当前已经是自动搜网，直接返回 VOS_TRUE*/
    if (MMA_MMC_PLMN_SEL_MODE_AUTO== TAF_MMA_GetPlmnSelectionMode())
    {
        return VOS_TRUE;
    }

    /* TI_TAF_MMA_PERIOD_TRYING_NETWORK_SELECTION_MENU定时器正在运行，直接返回VOS_TRUE */
    if (MMA_TIMER_RUN == TAF_MMA_GetTiTryingPeriodNetSelMenuStatus())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_VOID TAF_MMA_ProcCustomerServiceProfileFile(VOS_VOID)
{
    VOS_UINT32                          ulResult;

    if ((STA_FSM_STOP == g_StatusContext.ulFsmState)
     || (STA_FSM_NULL == g_StatusContext.ulFsmState))
    {
        /* 关机或NULL状态不处理 */
        MMA_NORMAILLOG("TAF_MMA_ProcCustomerServiceProfileFile(): Start Timer Fail");
    }
    else if (STA_FSM_ENABLE == g_StatusContext.ulFsmState)
    {
        /* 发起自动搜网请求 */
        TAF_MMA_SndAutoPlmnReselect_Refresh();

        /* 记录为Refresh触发的自动搜网 */
        TAF_MMA_SetAutoPlmnSelUser(TAF_MMA_AUTO_PLMN_SEL_USER_USIM_REFRESH);
    }
    else
    {
        /* 启动TI_TAF_MMA_PERIOD_TRYING_NETWORK_SELECTION_MENU定时器 */
        ulResult = NAS_StartRelTimer(&(g_StatusContext.stNetSelMenuCtx.stTiPeriodTryingNetSelMenu.MmaHTimer),
                                     WUEPS_PID_MMA,
                                     TI_TAF_MMA_PERIOD_TRYING_NETWORK_SELECTION_MENU_LEN,
                                     TAF_MMA,
                                     TI_TAF_MMA_PERIOD_TRYING_NETWORK_SELECTION_MENU,
                                     VOS_RELTIMER_NOLOOP );

        if( VOS_OK !=  ulResult)
        {
            MMA_ERRORLOG("TAF_MMA_ProcCustomerServiceProfileFile(): Start Timer Fail");
        }
        else
        {
            TAF_MMA_SetTiTryingPeriodNetSelMenuStatus(MMA_TIMER_RUN);
        }
    }

    return;
}


VOS_VOID TAF_MMA_RcvCustomerServiceProfileFile(
    VOS_UINT32                          ulRslt,
    VOS_UINT16                          usEfLen,
    VOS_UINT8                          *pucEf
)
{
    if (VOS_OK != ulRslt)
    {
        return;
    }

    /* 预处理结束，不需要进一步处理 */
    if (VOS_TRUE == TAF_MMA_RcvCustomerServiceProfileFile_PreProc(usEfLen, pucEf))
    {
        return;
    }

    /* 处理6F15文件 */
    TAF_MMA_ProcCustomerServiceProfileFile();

    return;
}


VOS_VOID TAF_MMA_NetworkSelectionMenuCtrlTimerExpired(VOS_VOID)
{
    /* 更新定时器状态 */
    TAF_MMA_SetTiTryingPeriodNetSelMenuStatus(MMA_TIMER_STOP);

    /* 如果在定时器运行过程中，又触发了REFRESH，需要判断最后一次REFRESH是否需要发起自动搜网 */
    if (TAF_MMA_PLMN_MODE_ENUM_DISABLE != TAF_MMA_GetPlmnMode_Refresh())
    {
        return;
    }

    /* 当前已经是自动搜网 ，返回 */
    if (MMA_MMC_PLMN_SEL_MODE_AUTO == TAF_MMA_GetPlmnSelectionMode())
    {
        return;
    }

    /* 处理6F15文件 */
    TAF_MMA_ProcCustomerServiceProfileFile();

    return;
}



VOS_UINT32 TAF_MMA_ProcRatMode_EnableStatus(TAF_MMA_RAT_MODE_ENUM_U8 enRatMode)
{
    TAF_PH_RAT_ORDER_STRU               stCurRatModeOrder;

    TAF_MMA_SYS_CFG_PARA_STRU           stSysCfg;

    MMA_SYS_CFG_SET_FLG                 usSetFlg;
    TAF_PH_ATTACH_TYPE                  ucAttachType;
    TAF_PH_DETACH_TYPE                  ucDetachType;
    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU *pLastSyscfgSet = VOS_NULL_PTR;
#if(FEATURE_ON == FEATURE_LTE)
    TAF_USER_SET_PREF_BAND64           *pstUeSupportLteBand = VOS_NULL_PTR;
#endif

    /* 将4F36文件的PLMN MODE值转换成MMA的接入技术列表 */
    TAF_MMA_TransferRatMode(enRatMode, &stCurRatModeOrder);

    /* 从MN_MMA_GetRatPrioListAddr中获取接入技术之外的参数 */
    pLastSyscfgSet     = MN_MMA_GetLastSyscfgSetAddr();


    stSysCfg.ucRoam      = pLastSyscfgSet->ucRoam;
    stSysCfg.ucSrvDomain = pLastSyscfgSet->ucSrvDomain;
    stSysCfg.stGuBand    = pLastSyscfgSet->stUserSetBand.stOrigUserSetGuBand;
#if(FEATURE_ON == FEATURE_LTE)
    stSysCfg.stLBand     = pLastSyscfgSet->stUserSetBand.stUserSetLteBand;
#endif

    stSysCfg.stGuBand.ulBandLow  &= gstMmaBandCapa.ulAllUeBand;
#if(FEATURE_ON == FEATURE_LTE)
    pstUeSupportLteBand = MN_MMA_GetUeSupportLteBandAddr();
    stSysCfg.stLBand.ulBandHigh &= pstUeSupportLteBand->ulBandHigh;
    stSysCfg.stLBand.ulBandLow  &= pstUeSupportLteBand->ulBandLow;
#endif

    if (VOS_TRUE == MN_MMA_IsSpecRatSupported(TAF_PH_RAT_LTE))
    {
        stSysCfg.enUserPrio = TAF_MMA_USER_SET_PRIO_NOCHANGE;
    }
    else
    {
        stSysCfg.enUserPrio = TAF_MMA_USER_SET_PRIO_AUTO;
    }

    /* 根据refresh的接入技术更新接入技术 */
    stSysCfg.stRatOrder  = stCurRatModeOrder;

    /* 调用MMA_PhSysCfgSetCmdCheck获取参数 */
    MMA_PhSysCfgSetCmdCheck(&stSysCfg, &usSetFlg, &ucAttachType, &ucDetachType);

    /* 调用MN_MMA_SetSyscfg_EnableStatus发送SYSCFG请求  */
    MN_MMA_SetSyscfg_EnableStatus(&stSysCfg, TAF_MMA_SYSCFG_USER_USIM_REFRESH);

    return VOS_OK;
}
VOS_UINT32 TAF_MMA_ProcRatMode(TAF_MMA_RAT_MODE_ENUM_U8 enRatMode)
{
    VOS_UINT32                          ulResult;

    if ((STA_FSM_STOP == g_StatusContext.ulFsmState)
     || (STA_FSM_NULL == g_StatusContext.ulFsmState))
    {
        /* 关机或NULL状态不处理 */
        MMA_NORMAILLOG("TAF_MMA_RcvRatModeFileRefresh(): Start Timer Fail");
    }
    else if (STA_FSM_ENABLE == g_StatusContext.ulFsmState)
    {
        /* 发起SYSCFG请求 */
        TAF_MMA_ProcRatMode_EnableStatus(enRatMode);
    }
    else
    {
        /* 启动TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING定时器 */
        ulResult = NAS_StartRelTimer(&(g_StatusContext.stRatBalancingCtx.stTiPeriodTryingRatBalancing.MmaHTimer),
                                     WUEPS_PID_MMA,
                                     TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING_LEN,
                                     TAF_MMA,
                                     TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING,
                                     VOS_RELTIMER_NOLOOP );

        if (VOS_OK != ulResult)
        {
            MMA_ERRORLOG("TAF_MMA_RcvRatModeFileRefresh(): Start Timer Fail");
        }
        else
        {
            TAF_MMA_SetTiTryingPeriodRatBalancingStatus(MMA_TIMER_RUN);
        }
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MMA_CheckRatModePara(
    TAF_PH_RAT_ORDER_STRU              *pstCurRatModeOrder
)
{
    VOS_UINT8                           ucCardType;
    VOS_UINT32                          ulGForbbidenFlag;


    ucCardType = USIMM_CARD_NOCARD;

    ulGForbbidenFlag = MN_MMA_IsGOnlyMode(pstCurRatModeOrder);

    NAS_USIMMAPI_GetCardType(VOS_NULL_PTR, &ucCardType) ;

#if (FEATURE_ON == FEATURE_LTE)

    /* 如果是SIM卡，当设置成L单模时不处理 */
    if (VOS_TRUE == MN_MMA_IsLOnlyMode(pstCurRatModeOrder))
    {
        if (USIMM_CARD_SIM == ucCardType)
        {
            MMA_ERRORLOG("TAF_MMA_CheckRatModePara_PreProc: L ONLY & SIM CARD!");
            return VOS_TRUE;
        }
    }
#endif

    /* 如果UE不支持GSM，当设置成G单模时，返回失败; */
    if (MMA_TRUE == MMA_IsGsmForbidden())
    {
        if (VOS_TRUE == ulGForbbidenFlag)
        {
            MMA_ERRORLOG("TAF_MMA_CheckRatModePara_PreProc: G ONLY & FORBIDDEN!");
            return VOS_TRUE;
        }
    }

    /* SIM卡并且G模禁止，当前接入技术为GL或者LG，返回失败 */
    if ((USIMM_CARD_SIM == ucCardType)
     && (VOS_TRUE == ulGForbbidenFlag)
     && ((TAF_PH_MAX_GUL_RAT_NUM - 1) == pstCurRatModeOrder->ucRatOrderNum))
    {
        if ((TAF_PH_RAT_LTE == pstCurRatModeOrder->aenRatOrder[0])
         && (TAF_PH_RAT_GSM == pstCurRatModeOrder->aenRatOrder[1]))
        {
            return VOS_TRUE;
        }

        if ((TAF_PH_RAT_GSM == pstCurRatModeOrder->aenRatOrder[0])
         && (TAF_PH_RAT_LTE == pstCurRatModeOrder->aenRatOrder[1]))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_MMA_RcvRatModeFileRefresh_PreProc(
    VOS_UINT16                          usEfLen,
    VOS_UINT8                          *pucEf
)
{
    TAF_PH_RAT_ORDER_STRU               stCurRatModeOrder;
    VOS_UINT32                          ulRatOrderChangedFlg;
    TAF_PH_RAT_ORDER_STRU              *pstLastRatPrioList = VOS_NULL_PTR;


    /* 如果当期为CL 模式，则直接返回 */
    if (VOS_TRUE == TAF_MMA_IsPowerOnCLInterWork())
    {
        return VOS_TRUE;
    }

    /* 接入技术平衡控制功能未使能，直接返回 */
    if (VOS_FALSE == TAF_MMA_GetRatBalancingEnableFlg())
    {
        return VOS_TRUE;
    }

    /* 该文件内容只有一个byte */
    if (usEfLen < TAF_MMA_RAT_MODE_FILE_LEN)
    {
        return VOS_TRUE;
    }

    /* 将4F36文件的PLMN MODE值转换成MMA的接入技术列表 */
    TAF_MMA_TransferRatMode(pucEf[0], &stCurRatModeOrder);

    /* 接入技术检查*/
    if (VOS_TRUE == TAF_MMA_CheckRatModePara(&stCurRatModeOrder))
    {
        return VOS_TRUE;
    }

    /* 保存refresh的接入技术 */
    TAF_MMA_SetRatMode_Refresh(pucEf[0]);

    /* TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING定时器正在运行，返回 */
    if (MMA_TIMER_RUN == TAF_MMA_GetTiTryingPeriodRatBalancingStatus())
    {
        return VOS_TRUE;
    }

    pstLastRatPrioList = MN_MMA_GetRatPrioListAddr();

    /* 与MN_MMA_GetRatPrioListAddr中的接入技术比较，不改变，直接返回*/
    ulRatOrderChangedFlg = MN_MMA_IsUserSettedRatOrderChanged(&stCurRatModeOrder,
                                                              pstLastRatPrioList);

    /* 接入技术不改变 */
    if (VOS_FALSE == ulRatOrderChangedFlg)
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_VOID TAF_MMA_RcvRatModeFileRefresh(
    VOS_UINT16                          usEfLen,
    VOS_UINT8                          *pucEf
)
{
    /* 预处理结束，不需要进一步处理 */
    if (VOS_TRUE == TAF_MMA_RcvRatModeFileRefresh_PreProc(usEfLen, pucEf))
    {
        return;
    }

    /* 处理4F36文件 */
    TAF_MMA_ProcRatMode(pucEf[0]);

    return;
}


VOS_VOID TAF_MMA_RatBalancingCtrlTimerExpired(VOS_VOID)
{
    TAF_PH_RAT_ORDER_STRU              *pstLastRatPrioList = VOS_NULL_PTR;
    TAF_PH_RAT_ORDER_STRU               stCurRatModeOrder;
    VOS_UINT32                          ulRatOrderChangedFlg;

    /* 更新定时器状态 */
    TAF_MMA_SetTiTryingPeriodRatBalancingStatus(MMA_TIMER_STOP);

    pstLastRatPrioList = MN_MMA_GetRatPrioListAddr();

    /* 将4F36文件的PLMN MODE值转换成MMA的接入技术列表 */
    TAF_MMA_TransferRatMode(TAF_MMA_GetRatMode_Refresh(), &stCurRatModeOrder);

    /* 与MN_MMA_GetRatPrioListAddr中的接入技术比较，不改变，直接返回*/
    ulRatOrderChangedFlg = MN_MMA_IsUserSettedRatOrderChanged(&stCurRatModeOrder,
                                                              pstLastRatPrioList);

    /* 参数改变，需要处理4F36文件 */
    if (VOS_TRUE == ulRatOrderChangedFlg)
    {
        TAF_MMA_ProcRatMode(TAF_MMA_GetRatMode_Refresh());
    }

    return;
}



VOS_UINT32 TAF_MMA_RcvAppSyscfg_PlmnResel(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfg
)
{
    /* 如果是6F15触发的自动搜网，下发SYSCFG；
       如果是AT触发的，由AT保证串行处理，此处不需处理 */
    if (TAF_MMA_AUTO_PLMN_SEL_USER_USIM_REFRESH == TAF_MMA_GetAutoPlmnSelUser())
    {
        MN_MMA_SetSyscfg_EnableStatus(pstSysCfg, TAF_MMA_SYSCFG_USER_AT);
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_RcvAppSyscfg_SysCfgSet(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    /* 如果4f36文件refresh触发的syscfg过程中，AT下发SYSCFG直接回复ERROR */
    if (TAF_MMA_SYSCFG_USER_USIM_REFRESH == TAF_MMA_GetSyscfgUser())
    {
        TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_ERROR);

        MMA_WARNINGLOG("TAF_MMA_RcvAppSyscfg_SysCfg:NORMAL:SYSCFG NONE SET!");
    }

    return VOS_OK;
}
VOS_VOID TAF_MMA_PlmnIdReport(
    TAF_PLMN_ID_STRU                   *pstPlmn
)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent = VOS_NULL_PTR;

    TAF_SDC_PLMN_ID_STRU               *pstPlmnId = VOS_NULL_PTR;

    pstPlmnId = TAF_SDC_GetCurrCampPlmnId();

    /* 如果主动上报控制NV没有打开，则不上报 */
    if (VOS_FALSE == gstMmaValue.ucReportPlmnFlg)
    {
        return;
    }

    /* plmn没有改变，不上报 */
    if ((pstPlmn->Mcc == pstPlmnId->ulMcc)
     && (pstPlmn->Mnc == pstPlmnId->ulMnc))
    {
        return;
    }

    /* 申请变量 */
    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));
    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("TAF_MMA_PlmnIdReport:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    /* 初始化变量 */
    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    /* 填充上报事件 */
    pstPhoneEvent->PhoneEvent    = TAF_PH_EVT_PLMN_CHANGE_IND;
    pstPhoneEvent->OP_CurPlmn    = MMA_TRUE;
    pstPhoneEvent->stCurPlmn.Mcc = pstPlmn->Mcc;
    pstPhoneEvent->stCurPlmn.Mnc = pstPlmn->Mnc;

    MMA_PlmnId2Bcd(&(pstPhoneEvent->stCurPlmn));

    /* 事件上报 */
    Taf_PhoneEvent(pstPhoneEvent);

    /* 内存释放 */
    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return;
}
VOS_VOID  TAF_MMA_SndOmGetCacheFile(
    VOS_UINT32                          ulFileId,
    VOS_UINT32                          ulFileLen,
    VOS_UINT8                          *pucFileContent
)
{
    TAF_MMA_GET_CACHE_FILE_STRU        *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgLen;

    ulMsgLen = sizeof(TAF_MMA_GET_CACHE_FILE_STRU) + ulFileLen;

    pstMsg = (TAF_MMA_GET_CACHE_FILE_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA, ulMsgLen);

    if (VOS_NULL_PTR == pstMsg)
    {
        MMA_ERRORLOG("TAF_MMA_SndOmMmcTimerStatus:ERROR:Alloc Mem Fail.");
        return;
    }

    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength        = ulMsgLen - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName       = ulFileId;

    pstMsg->ulFileLen                   = ulFileLen;

    PS_MEM_CPY(pstMsg->aucFileContent, pucFileContent, ulFileLen);

    OM_TraceMsgHook(pstMsg);

    PS_MEM_FREE(WUEPS_PID_MMA, pstMsg);

}


VOS_VOID  TAF_MMA_SndOmLocationStatus(VOS_VOID)
{
    MSG_HEADER_STRU                    *pstMsg = VOS_NULL_PTR;

    pstMsg = (MSG_HEADER_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(MSG_HEADER_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        MMA_ERRORLOG("TAF_MMA_SndOmLocationStatus:ERROR:Alloc Mem Fail.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMA;
    pstMsg->ulReceiverPid   = WUEPS_PID_MMA;
    pstMsg->ulLength        = 4;
    pstMsg->ulMsgName       = 0x9999;

    OM_TraceMsgHook(pstMsg);

    PS_MEM_FREE(WUEPS_PID_MMA, pstMsg);

}


VOS_VOID  TAF_MMA_SndDrxTimerInfo( VOS_UINT8 ucDrxTimerStatus )
{
    TAF_MMA_DRX_TIMER_STAUTS_STRU        *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgLen;

    ulMsgLen = sizeof(TAF_MMA_DRX_TIMER_STAUTS_STRU)  ;

    pstMsg = (TAF_MMA_DRX_TIMER_STAUTS_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA, ulMsgLen);

    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "TAF_MMA_SndDrxTimerInfo:ERROR:Alloc Mem Fail.");
        return;
    }

    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength        = ulMsgLen - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName       = 0x8888;
    PS_MEM_SET(pstMsg->aucReserve,0x00,sizeof(pstMsg->aucReserve));
    pstMsg->ucDrxTimerStatus            = ucDrxTimerStatus;

    OM_TraceMsgHook(pstMsg);

    PS_MEM_FREE(WUEPS_PID_MMA, pstMsg);

}


VOS_VOID  TAF_MMA_LogReadWriteNvInfo(
    VOS_UINT32                          ulNvId,
    VOS_UINT32                          ulRslt,
    VOS_UINT8                           *pucNVInfo,
    VOS_UINT32                          ulNvInfoLen
)
{
    TAF_MMA_LOG_READ_WRITE_NV_INFO_STRU            *pstMsg = VOS_NULL_PTR;

    pstMsg = (TAF_MMA_LOG_READ_WRITE_NV_INFO_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                         sizeof(TAF_MMA_LOG_READ_WRITE_NV_INFO_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_LogReadWriteNvInfo:ERROR:Alloc Mem Fail.");
        return;
    }

    PS_MEM_SET(pstMsg, 0x00, sizeof(TAF_MMA_LOG_READ_WRITE_NV_INFO_STRU));

    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength        = sizeof(TAF_MMA_LOG_READ_WRITE_NV_INFO_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName       = TAF_MMA_LOG_NV_READ_WRITE_RSLT_MSG;
    pstMsg->ulNvId                      = ulNvId;
    pstMsg->ulRslt                      = ulRslt;

    if (ulNvInfoLen > 0)
    {
        if (ulNvInfoLen < TAF_MMA_MAX_NV_INFO_LEN)
        {
            PS_MEM_CPY(pstMsg->aucNvInfo, pucNVInfo, ulNvInfoLen);
        }
        else
        {
            PS_MEM_CPY(pstMsg->aucNvInfo, pucNVInfo, TAF_MMA_MAX_NV_INFO_LEN);
        }
    }

    OM_TraceMsgHook(pstMsg);

    PS_MEM_FREE(WUEPS_PID_MMA, pstMsg);

    return;
}


VOS_UINT32 TAF_MMA_IsPlatformSupportSpecUserRat(
    TAF_PH_RAT_TYPE_ENUM_UINT8          enUserRat
)
{
    VOS_UINT32                          ulPlatformSuppFlg;
    VOS_UINT32                          ulGsmForbiddenFlg;

    ulPlatformSuppFlg = VOS_TRUE;

    switch (enUserRat)

    {
        case TAF_PH_RAT_GSM:
            /* 对于GSM接入技术，还需要判断GSM硬件频段是否支持 */
            ulGsmForbiddenFlg = MMA_IsGsmForbidden();

            if ((VOS_FALSE == TAF_SDC_IsPlatformSupportGsm())
             || (MMA_TRUE == ulGsmForbiddenFlg))
            {
                ulPlatformSuppFlg = VOS_FALSE;
            }
            break;

        case TAF_PH_RAT_WCDMA:
            if (VOS_FALSE == TAF_SDC_IsPlatformSupportUtran())
            {
                ulPlatformSuppFlg = VOS_FALSE;
            }
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case TAF_PH_RAT_LTE:
            if (VOS_FALSE == TAF_SDC_IsPlatformSupportLte())
            {
                ulPlatformSuppFlg = VOS_FALSE;
            }
            break;
#endif

        default :
            MMA_WARNINGLOG("TAF_MMA_IsPlatformSupportSpecUserRat:WARNING:invalid rat.");

            /* 无效接入技术默认不支持 */
            ulPlatformSuppFlg = VOS_FALSE;
            break;
    }

    return ulPlatformSuppFlg;
}


VOS_VOID TAF_MMA_DelPlatformUnsupportedRat(
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulPlatformSuppFlg;

    i = 0;

    if (pstRatPrioList->ucRatOrderNum > TAF_PH_MAX_GUL_RAT_NUM)
    {
        pstRatPrioList->ucRatOrderNum = TAF_PH_MAX_GUL_RAT_NUM;
    }

    /* 将pstRatPrioList中支持接入技术进行遍历，检查平台接入技术能力是否支持 */
    while (i < pstRatPrioList->ucRatOrderNum)
    {
        ulPlatformSuppFlg = TAF_MMA_IsPlatformSupportSpecUserRat(pstRatPrioList->aenRatOrder[i]);

        /* 如果不支持，需要删除，如果支持，继续检查下一个 */
        if (VOS_FALSE == ulPlatformSuppFlg)
        {
            pstRatPrioList->aenRatOrder[i] = TAF_PH_RAT_BUTT;

            PS_MEM_MOVE(&(pstRatPrioList->aenRatOrder[i]),
                        &(pstRatPrioList->aenRatOrder[i+1]),
                        (TAF_PH_MAX_GUL_RAT_NUM-i-1));

            pstRatPrioList->ucRatOrderNum --;
        }
        else
        {
            i ++;
        }
    }

    return;
}


VOS_VOID TAF_MMA_RcvTafMsgAcInfoQueryReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_AC_INFO_QUERY_REQ_STRU     *pstAcInfoQuery
)
{
    TAF_MMA_AC_INFO_QUERY_CNF_STRU     *pstAcInfoQryCnf = VOS_NULL_PTR;
    TAF_SDC_SYS_MODE_ENUM_UINT8         enRadioMode;
    TAF_SDC_ACCESS_RESTRICTION_STRU    *pstCsAcRestriction = VOS_NULL_PTR;
    TAF_SDC_ACCESS_RESTRICTION_STRU    *pstPsAcRestriction = VOS_NULL_PTR;

    pstCsAcRestriction = TAF_SDC_GetCsAcRestriction();
    pstPsAcRestriction = TAF_SDC_GetPsAcRestriction();
    pstAcInfoQryCnf    = (TAF_MMA_AC_INFO_QUERY_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                             WUEPS_PID_MMA,
                                                             sizeof(TAF_MMA_AC_INFO_QUERY_CNF_STRU));

    if (VOS_NULL_PTR == pstAcInfoQryCnf)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_RcvTafMsgAcInfoQueryReq:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR*)pstAcInfoQryCnf + VOS_MSG_HEAD_LENGTH,
               0x00,
               sizeof(TAF_MMA_AC_INFO_QUERY_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填写消息 */
    pstAcInfoQryCnf->ulReceiverPid          = WUEPS_PID_AT;
    pstAcInfoQryCnf->ulMsgName              = AT_MMA_AC_INFO_QUERY_CNF;
    pstAcInfoQryCnf->stAtAppCtrl.usClientId = MN_GetRealClientId(usClientId, WUEPS_PID_MMA);
    pstAcInfoQryCnf->stAtAppCtrl.ucOpId     = ucOpId;

    /*获取当前接入技术, 调用API*/
    enRadioMode = TAF_SDC_GetSysMode();

    if (TAF_SDC_SYS_MODE_LTE == enRadioMode)
    {
        /* 返回失败 */
        pstAcInfoQryCnf->ulRslt = TAF_ERR_ERROR;
    }
    else
    {
        pstAcInfoQryCnf->ulRslt = TAF_ERR_NO_ERROR;

        pstAcInfoQryCnf->stCellCsAcInfo.ucSrvDomain           = TAF_PH_SERVICE_CS;
        pstAcInfoQryCnf->stCellCsAcInfo.enCellAcType          = pstCsAcRestriction->enCellAcType;
        pstAcInfoQryCnf->stCellCsAcInfo.ucRestrictPagingRsp   = pstCsAcRestriction->ucRestrictPagingRsp;
        pstAcInfoQryCnf->stCellCsAcInfo.ucRestrictRegister    = pstCsAcRestriction->ucRestrictRegister;

        pstAcInfoQryCnf->stCellPsAcInfo.ucSrvDomain           = TAF_PH_SERVICE_PS;
        pstAcInfoQryCnf->stCellPsAcInfo.enCellAcType          = pstPsAcRestriction->enCellAcType;
        pstAcInfoQryCnf->stCellPsAcInfo.ucRestrictPagingRsp   = pstPsAcRestriction->ucRestrictPagingRsp;
        pstAcInfoQryCnf->stCellPsAcInfo.ucRestrictRegister    = pstPsAcRestriction->ucRestrictRegister;
    }


    /* 发送消息 */
    if( VOS_OK != PS_SEND_MSG(WUEPS_PID_MMA, pstAcInfoQryCnf))
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR, "TAF_MMA_RcvTafMsgAcInfoQueryReq:ERROR: Send Msg Error!");
    }

    return;
}
VOS_VOID TAF_MMA_RcvTafMsgCerssiInfoQueryReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CERSSI_INFO_QUERY_REQ_STRU *pstCerssiInfoQuery
)
{
    TAF_MMA_CERSSI_INFO_QUERY_CNF_STRU *pstCerssiInfoQryCnf = VOS_NULL_PTR;

    pstCerssiInfoQryCnf  = (TAF_MMA_CERSSI_INFO_QUERY_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                         WUEPS_PID_MMA,
                                                         sizeof(TAF_MMA_CERSSI_INFO_QUERY_CNF_STRU));

    if (VOS_NULL_PTR == pstCerssiInfoQryCnf)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_RcvTafMsgCerssiInfoQueryReq:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR*)pstCerssiInfoQryCnf + VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CERSSI_INFO_QUERY_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填写消息 */
    pstCerssiInfoQryCnf->ulReceiverPid          = WUEPS_PID_AT;
    pstCerssiInfoQryCnf->ulMsgName              = AT_MMA_CERSSI_INFO_QUERY_CNF;
    pstCerssiInfoQryCnf->stAtAppCtrl.usClientId = usClientId;
    pstCerssiInfoQryCnf->stAtAppCtrl.ucOpId     = ucOpId;

    pstCerssiInfoQryCnf->stCerssi.ucCurrentUtranMode    = NAS_UTRANCTRL_GetCurrUtranMode();
    pstCerssiInfoQryCnf->ucRslt                         = VOS_OK;
    pstCerssiInfoQryCnf->stCerssi.ucRssiNum             = gstMmaValue.stCerssiValue.ucRssiNum;
    pstCerssiInfoQryCnf->stCerssi.enRatType             = TAF_SDC_GetSysMode();

    PS_MEM_CPY(pstCerssiInfoQryCnf->stCerssi.aRssi, gstMmaValue.stCerssiValue.aRssi, sizeof(pstCerssiInfoQryCnf->stCerssi.aRssi));

    /* 发送消息 */
    if( VOS_OK != PS_SEND_MSG(WUEPS_PID_MMA, pstCerssiInfoQryCnf))
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_ERROR, "TAF_MMA_RcvTafMsgCerssiInfoQueryReq:ERROR: Send Msg Error!");
    }

    return;
}



VOS_VOID TAF_MMA_FillCopnOperNameList(
    VOS_UINT32                          ulFromIndex,
    VOS_UINT16                          usPlmnNum,
    TAF_MMA_COPN_INFO_QUERY_CNF_STRU   *pstCopnQryCnf
)
{
    VOS_UINT32                          ulLoop;
    TAF_PH_OPERATOR_NAME_STRU          *pstOperNameList   = VOS_NULL_PTR;
    TAF_PH_OPERATOR_NAME_TBL_STRU      *pstNetworkNameTbl = VOS_NULL_PTR;

    /*
       填写gastNetworkNameTbl中从ulFromIndex开始的usPlmnNum运营商信息到
       +COPN结果上报消息的运营商名称列表
    */
    pstCopnQryCnf->usPlmnNum      = usPlmnNum;

    pstNetworkNameTbl             = TAF_MMA_GetNetworkNameTblAddr();
    pstNetworkNameTbl            +=  ulFromIndex;

    pstOperNameList               = (TAF_PH_OPERATOR_NAME_STRU *)pstCopnQryCnf->aucContent;

    for (ulLoop = 0; ulLoop < usPlmnNum; ulLoop++)
    {
        /*
        将运营商的信息拷贝到+COPN上报消息中: 数字类型的PLMN，运营商长名
        为提高接口的可扩展性，运营商短名也在消息中发送
        */
        pstOperNameList->PlmnId.Mcc = pstNetworkNameTbl->PlmnId.Mcc;
        pstOperNameList->PlmnId.Mnc = pstNetworkNameTbl->PlmnId.Mnc;

        /* 运营商的MNC长度不为3时，将第三位设置为F */
        if (TAF_MMA_PLMN_MNC_TWO_BYTES == pstNetworkNameTbl->ucMncLength)
        {
            pstOperNameList->PlmnId.Mnc |= TAF_MMA_PLMN_MNC_DIGIT3_MASK;
        }

        VOS_StrNCpy(pstOperNameList->aucOperatorNameShort,
                   pstNetworkNameTbl->pStrOperatorNameShort,
                   TAF_PH_OPER_NAME_SHORT);

        VOS_StrNCpy(pstOperNameList->aucOperatorNameLong,
                   pstNetworkNameTbl->pStrOperatorNameLong,
                   TAF_PH_OPER_NAME_LONG);

        pstNetworkNameTbl ++;
        pstOperNameList ++;
    }

    return;
}
VOS_VOID TAF_MMA_RcvTafMsgCopnQueryReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_COPN_INFO_QUERY_REQ_STRU   *pstCopnInfo
)
{
    TAF_MMA_COPN_INFO_QUERY_CNF_STRU   *pstCopnQryCnf = VOS_NULL_PTR;
    VOS_UINT16                          usPlmnNum;
    VOS_UINT16                          usRemainPlmnNum;
    VOS_UINT16                          usTotal;
    VOS_UINT32                          ulMsgSize;

    /*
        设置分组结束标志和分组上报运营商个数: 待发送的运营商数少于一条消息中
        允许发送的运营商数，则设置结束标志并发送所有待发送运营商信息；
    */
    usTotal             = TAF_MMA_GetNetWorkNameTblSize();

    if (pstCopnInfo->usFromIndex > usTotal)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL_PTR, PS_PRINT_WARNING,
                "TAF_MMA_RcvTafMsgCopnQueryReq: abnormal request!");
        usPlmnNum = 0;
    }
    else
    {
        usRemainPlmnNum = usTotal - (pstCopnInfo->usFromIndex);

        if (usRemainPlmnNum < pstCopnInfo->usPlmnNum)
        {
            usPlmnNum   = usRemainPlmnNum;
        }
        else
        {
            usPlmnNum   = pstCopnInfo->usPlmnNum;
        }
    }

    if (0 < usPlmnNum)
    {
        ulMsgSize  = sizeof(TAF_MMA_COPN_INFO_QUERY_CNF_STRU) - sizeof(pstCopnQryCnf->aucContent);
        ulMsgSize += usPlmnNum * sizeof(TAF_PH_OPERATOR_NAME_STRU);
    }
    else
    {
        ulMsgSize  = sizeof(TAF_MMA_COPN_INFO_QUERY_CNF_STRU);
    }

    /* 以TAF_PH_COPN_PLMN_MAX_NUM个运营商为一组分组上报运营商信息 */
    pstCopnQryCnf  = (TAF_MMA_COPN_INFO_QUERY_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                         WUEPS_PID_MMA,
                                                         ulMsgSize);

    if (VOS_NULL_PTR == pstCopnQryCnf)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL_PTR, PS_PRINT_ERROR,
                "TAF_MMA_RcvTafMsgCopnQueryReq: Memory Alloc Error for pMsg!");
        return;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR *)pstCopnQryCnf + VOS_MSG_HEAD_LENGTH,
                0x00,
                (ulMsgSize - VOS_MSG_HEAD_LENGTH));

    /* 填写消息头部信息 */
    pstCopnQryCnf->ulReceiverPid          = WUEPS_PID_AT;
    pstCopnQryCnf->ulMsgName              = AT_MMA_COPN_INFO_QUERY_CNF;
    pstCopnQryCnf->stAtAppCtrl.usClientId = usClientId;
    pstCopnQryCnf->stAtAppCtrl.ucOpId     = ucOpId;

    /*
    从ulFromIndex开始将gastNetworkNameTbl中运营商信息填写到
    pstCopnQryCnf->astOperNameList结构数组，
    运营商的个数为pstCopnQryCnf->usPlmnNum
    */
    TAF_MMA_FillCopnOperNameList(pstCopnInfo->usFromIndex, usPlmnNum, pstCopnQryCnf);

    pstCopnQryCnf->usFromIndex    = pstCopnInfo->usFromIndex;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMA, pstCopnQryCnf))
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL_PTR, PS_PRINT_ERROR,
                "TAF_MMA_RcvTafMsgCopnQueryReq: Send Msg Error!");
    }

    return;

}
VOS_VOID TAF_MMA_SaveCellAcInfo(
    MMC_MMA_AC_INFO_CHANGE_IND_STRU    *pstAcInfoInd
)
{
    TAF_SDC_ACCESS_RESTRICTION_STRU     stAcRestriction;

    PS_MEM_SET(&stAcRestriction, 0, sizeof(stAcRestriction));

    stAcRestriction.enCellAcType              = pstAcInfoInd->enCellAcType;
    stAcRestriction.ucRestrictPagingRsp       = pstAcInfoInd->ucRestrictPagingRsp;
    stAcRestriction.ucRestrictRegister        = pstAcInfoInd->ucRestrictRegister;

    if (MMA_MMC_SRVDOMAIN_CS == pstAcInfoInd->enCnDomainId)
    {
        TAF_SDC_SetCsAcRestriction(&stAcRestriction);
    }
    else if (MMA_MMC_SRVDOMAIN_PS == pstAcInfoInd->enCnDomainId)
    {
        TAF_SDC_SetPsAcRestriction(&stAcRestriction);
    }
    else
    {
        MMA_ERRORLOG("TAF_MMA_SaveCellAcInfo:ERROR:Unexpected src domain.");
    }


    return;
}

/* TAF_MMA_InitCellAcInfo移动位置 */

VOS_UINT32 TAF_MMA_RcvMmcAcInfoChangeInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 上报给AT */
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent = VOS_NULL_PTR;
    MMC_MMA_AC_INFO_CHANGE_IND_STRU    *pstAcInfoChangeInd;

    pstAcInfoChangeInd = (MMC_MMA_AC_INFO_CHANGE_IND_STRU*)pstMsg;


    /* 存储当前小区的接入禁止信息 */
    TAF_MMA_SaveCellAcInfo(pstAcInfoChangeInd);

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("TAF_MMA_RcvMmcAcInfoChangeInd:ERROR:ALLOC MEMORY FAIL.");

        return VOS_TRUE;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    /*填充事件*/
    MMA_InitEventInfoOP(pstPhoneEvent);

    pstPhoneEvent->Op_CellAcInfo                    = MMA_TRUE;
    pstPhoneEvent->stCellAcInfo.enCellAcType        = pstAcInfoChangeInd->enCellAcType;
    pstPhoneEvent->stCellAcInfo.ucRestrictPagingRsp = pstAcInfoChangeInd->ucRestrictPagingRsp;
    pstPhoneEvent->stCellAcInfo.ucRestrictRegister  = pstAcInfoChangeInd->ucRestrictRegister;

    pstPhoneEvent->PhoneEvent                       = TAF_MMA_EVT_AC_INFO_CHANGE_IND;

    /* MMC的服务域格式与ACINFO命令上报的格式值不完全对应,需要进行转换 */
    if (MMA_MMC_SRVDOMAIN_CS == pstAcInfoChangeInd->enCnDomainId )
    {
        pstPhoneEvent->stCellAcInfo.ucSrvDomain  = TAF_PH_SERVICE_CS;
    }
    else if (MMA_MMC_SRVDOMAIN_PS == pstAcInfoChangeInd->enCnDomainId )
    {
        pstPhoneEvent->stCellAcInfo.ucSrvDomain  = TAF_PH_SERVICE_PS;
    }
    else
    {
        MMA_ERRORLOG("TAF_MMA_RcvMmcAcInfoChangeInd:ERROR:Unexpected srv domain.");
    }

    /*上报事件*/
    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_IsModeChangeNeedRpt(VOS_VOID)
{
    TAF_SDC_CURC_RPT_CTRL_STRU             *pstCurcRptCtrl          = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU      *pstUnsolicitedRptCtrl   = VOS_NULL_PTR;
    VOS_UINT32                              ulChkCurcRptFlg;
    VOS_UINT32                              ulChkUnsolicitedRptFlg;

    pstCurcRptCtrl              = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl       = TAF_SDC_GetUnsolicitedRptCtrl();

    ulChkCurcRptFlg             = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                            TAF_SDC_RPT_CMD_MODE);

    ulChkUnsolicitedRptFlg      = TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_MODE);

    /* CURC和^mode都设置为主动上报打开时，才需要上报 */
    if ( (VOS_FALSE == ulChkCurcRptFlg)
     ||  (VOS_FALSE == ulChkUnsolicitedRptFlg) )
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_IsServStatusChangeNeedRpt(VOS_VOID)
{
    TAF_SDC_CURC_RPT_CTRL_STRU             *pstCurcRptCtrl          = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU      *pstUnsolicitedRptCtrl   = VOS_NULL_PTR;
    VOS_UINT32                              ulChkCurcRptFlg;
    VOS_UINT32                              ulChkUnsolicitedRptFlg;

    pstCurcRptCtrl                  = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl           = TAF_SDC_GetUnsolicitedRptCtrl();

    ulChkCurcRptFlg                 = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                            TAF_SDC_RPT_CMD_SRVST);

    ulChkUnsolicitedRptFlg          = TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_SRVST);

    /* CURC和^srvst都设置为主动上报打开时，才需要上报 */
    if ( (VOS_FALSE == ulChkCurcRptFlg)
     ||  (VOS_FALSE == ulChkUnsolicitedRptFlg) )
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_IsRssiChangeNeedRpt(VOS_VOID)
{
    VOS_UINT32                              ulChkRssiCurcFlg;
    VOS_UINT32                              ulChkRssiUnsolicitedFlg;
    VOS_UINT32                              ulChkCerssiCurcFlg;
    VOS_UINT32                              ulChkCerssiUnsolicitedFlg;
    TAF_SDC_CURC_RPT_CTRL_STRU             *pstCurcRptCtrl          = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU      *pstUnsolicitedRptCtrl   = VOS_NULL_PTR;

    pstCurcRptCtrl              = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl       = TAF_SDC_GetUnsolicitedRptCtrl();

    ulChkRssiCurcFlg            = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                            TAF_SDC_RPT_CMD_RSSI);

    ulChkRssiUnsolicitedFlg     = TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_RSSI);

    ulChkCerssiCurcFlg          = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                            TAF_SDC_RPT_CMD_CERSSI);

    ulChkCerssiUnsolicitedFlg   = TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_CERSSI);

    /* RSSI需要上报 */
    if ( (VOS_TRUE == ulChkRssiCurcFlg)
      && (VOS_TRUE == ulChkRssiUnsolicitedFlg) )
    {
        return VOS_TRUE;
    }

    /* CERSSI需要上报 */
    if ( (VOS_TRUE == ulChkCerssiCurcFlg)
      && (VOS_TRUE == ulChkCerssiUnsolicitedFlg) )
    {
        return VOS_TRUE;
    }


    return VOS_FALSE;
}


VOS_UINT32 TAF_MMA_IsTimeChangeNeedRpt(VOS_VOID)
{
    VOS_UINT32                              ulChkTimeCurcFlg;
    VOS_UINT32                              ulChkTimeUnsolicitedFlg;
    VOS_UINT32                              ulChkCtzvCurcFlg;
    VOS_UINT32                              ulChkCtzvUnsolicitedFlg;
    TAF_SDC_CURC_RPT_CTRL_STRU             *pstCurcRptCtrl          = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU      *pstUnsolicitedRptCtrl   = VOS_NULL_PTR;

    pstCurcRptCtrl              = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl       = TAF_SDC_GetUnsolicitedRptCtrl();

    ulChkTimeCurcFlg            = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                            TAF_SDC_RPT_CMD_TIME);

    ulChkTimeUnsolicitedFlg     = TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_TIME);

    ulChkCtzvCurcFlg            = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                            TAF_SDC_RPT_CMD_CTZV);

    ulChkCtzvUnsolicitedFlg     = TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_CTZV);

    /* TIME需要上报 */
    if ( (VOS_TRUE == ulChkTimeCurcFlg)
      && (VOS_TRUE == ulChkTimeUnsolicitedFlg) )
    {
        return VOS_TRUE;
    }

    /* CTZV需要上报 */
    if ( (VOS_TRUE == ulChkCtzvCurcFlg)
      && (VOS_TRUE == ulChkCtzvUnsolicitedFlg) )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_MMA_IsSimStaChangeNeedRpt(VOS_VOID)
{
    TAF_SDC_CURC_RPT_CTRL_STRU             *pstCurcRptCtrl          = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU      *pstUnsolicitedRptCtrl   = VOS_NULL_PTR;
    VOS_UINT32                              ulChkCurcRptFlg;
    VOS_UINT32                              ulChkUnsolicitedRptFlg;

    pstCurcRptCtrl                  = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl           = TAF_SDC_GetUnsolicitedRptCtrl();

    ulChkCurcRptFlg                 = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                            TAF_SDC_RPT_CMD_SIMST);

    ulChkUnsolicitedRptFlg          = TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_SIMST);

    /* simst需要上报 */
    if ( (VOS_TRUE == ulChkCurcRptFlg)
      && (VOS_TRUE == ulChkUnsolicitedRptFlg) )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 TAF_MMA_IsRegStatusChangeNeedRpt(
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
)
{
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU      *pstUnsolicitedRptCtrl   = VOS_NULL_PTR;

    pstUnsolicitedRptCtrl           = TAF_SDC_GetUnsolicitedRptCtrl();

    if (MMA_FALSE == pEvent->OP_CurRegSta)
    {
        return VOS_FALSE;
    }

    /* creg 需要上报 */
    if ( (VOS_TRUE == TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_CREG))
      && (VOS_TRUE == pEvent->CurRegSta.OP_CsRegState) )
    {
        return VOS_TRUE;
    }

    /* cgreg 需要上报 */
    if ( (VOS_TRUE == TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_CGREG))
      && (VOS_TRUE == pEvent->CurRegSta.OP_PsRegState) )
    {
        return VOS_TRUE;
    }

    /* cereg 需要上报 */
    if ( (VOS_TRUE == TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_CEREG))
      && (VOS_TRUE == pEvent->CurRegSta.OP_PsRegState) )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
MMA_MMC_ALLOWED_REG_DOMAIN_ENUM_UINT8 TAF_MMA_GetRegDomain(VOS_VOID)
{
    MMA_MMC_ALLOWED_REG_DOMAIN_ENUM_UINT8                   enAllowedRegDomain;
    VOS_UINT8                                               ucAutoAttachFlg;
    NAS_NVIM_AUTOATTACH_STRU                            stAutoattachFlag;
    VOS_UINT32                                              ulLength;

    ulLength = 0;
    PS_MEM_SET(&stAutoattachFlag, 0x00, sizeof(NAS_NVIM_AUTOATTACH_STRU));

    NV_GetLength(en_NV_Item_Autoattach, &ulLength);
    if (NV_OK == NV_Read(en_NV_Item_Autoattach,
                         (VOS_VOID*)(&stAutoattachFlag),
                         ulLength))
    {
        if (NV_ITEM_ACTIVE == stAutoattachFlag.usAutoattachFlag)
        {
            ucAutoAttachFlg = MMA_AUTO_ATTACH_ENABLE;
        }
        else
        {
            ucAutoAttachFlg = MMA_AUTO_ATTACH_DISABLE;
        }
    }
    else
    {
        ucAutoAttachFlg = MMA_AUTO_ATTACH_ENABLE;
    }

    switch (g_StatusContext.ucModeService)
    {
        case TAF_PH_MS_CLASS_A:
            enAllowedRegDomain = MMA_MMC_ALLOWED_REG_DOMAIN_CS_PS;

            if (MMA_AUTO_ATTACH_DISABLE == ucAutoAttachFlg)
            {
                 enAllowedRegDomain = MMA_MMC_ALLOWED_REG_DOMAIN_CS;
            }
            break;

        case TAF_PH_MS_CLASS_CG:
            enAllowedRegDomain = MMA_MMC_ALLOWED_REG_DOMAIN_PS;

            if (MMA_AUTO_ATTACH_DISABLE == ucAutoAttachFlg)
            {
                enAllowedRegDomain = MMA_MMC_ALLOWED_REG_DOMAIN_NULL;
            }
            break;

        case TAF_PH_MS_CLASS_CC:
            enAllowedRegDomain = MMA_MMC_ALLOWED_REG_DOMAIN_CS;
            break;

        case TAF_PH_MS_CLASS_NULL:
            enAllowedRegDomain = MMA_MMC_ALLOWED_REG_DOMAIN_NULL;
            break;

        default:
            enAllowedRegDomain = MMA_MMC_ALLOWED_REG_DOMAIN_NULL;
            break;
    }

    return enAllowedRegDomain;
}


 VOS_UINT32 TAF_MMA_HexAlpha2AsciiString(
    VOS_UINT32                      ulDestLength,
    VOS_UINT8                      *pucDst,
    VOS_UINT8                      *pucSrc,
    VOS_UINT16                      usSrcLen)
 {
    VOS_UINT16                     usLen;
    VOS_UINT16                     usChkLen;
    VOS_UINT8                     *pWrite = VOS_NULL_PTR;
    VOS_UINT8                     *pRead = VOS_NULL_PTR;
    VOS_UINT8                      ucHigh;
    VOS_UINT8                      ucLow;

    usLen       = 0;
    usChkLen    = 0;
    pWrite      = pucDst;
    pRead       = pucSrc;
    ucHigh      = 0;
    ucLow       = 0;

    if((2 * usSrcLen)>= ulDestLength)
    {
        PS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_PRINT_WARNING, "MM_HexAlpha2AsciiString too long");
        return 0;
    }

    if(0 != usSrcLen)
    {
        /* 扫完整个字串 */
        while( usChkLen++ < usSrcLen )
        {
            ucHigh = 0x0F & (*pRead >> 4);
            ucLow = 0x0F & *pRead;

            usLen += 2;    /* 记录长度 */

            if(0x09 >= ucHigh)   /* 0-9 */
            {
                *pWrite++ = ucHigh + 0x30;
            }
            else if(0x0A <= ucHigh)    /* A-F */
            {
                *pWrite++ = ucHigh + 0x37;
            }
            else
            {
            }

            if(0x09 >= ucLow)   /* 0-9 */
            {
                *pWrite++ = ucLow + 0x30;
            }
            else if(0x0A <= ucLow)    /* A-F */
            {
                *pWrite++ = ucLow + 0x37;
            }
            else
            {
            }

             /* 下一个字符 */
            pRead++;
        }

    }

    return usLen;
 }
VOS_VOID TAF_MMA_DefaultAlphaToAscii(
    VOS_UINT8                          *pucDefAlpha,
    VOS_UINT8                           ucDefAlphaLen,
    VOS_UINT8                          *pucAsciiChar
)
{
    VOS_UINT8                           ucLoop;
    VOS_UINT8                           ucPos;
    VOS_UINT16                          usDecode;

    for (ucLoop = 0; ucLoop < ucDefAlphaLen; ucLoop++)
    {
        ucPos           = 0x7f & pucDefAlpha[ucLoop];
        usDecode        = f_ausMnAsciiSfxDefAlpha[ucPos];
        /* 参考标杆:在单字节不能表示时,使用?进行代替 */
        if (usDecode > 0x00ff)
        {
            *pucAsciiChar = '?';
        }
        else
        {
            *pucAsciiChar = (VOS_UINT8)usDecode;
        }

        pucAsciiChar++;
    }

    return;
}
VOS_VOID TAF_MMA_TranslateNtwkName2Str(
    VOS_UINT8                       *pucIeNtwkName,
    VOS_CHAR                        *pucNtwkName,
    VOS_UINT32                       ulLen
)
{
    VOS_UINT8                       ucIndex;
    VOS_UINT8                       ucIeLen;
    VOS_UINT8                       ucCodScheme;
    VOS_UINT8                       uc7BitCharNum;
    VOS_UINT8                      *pTmpNtwkName;
    VOS_UINT8                      *pucDecodeNwName;
    VOS_UINT8                       ucSpareBit;

    VOS_UINT32                      ulRslt;

    ucIndex         = 0;

    ucIeLen         = pucIeNtwkName[ucIndex++];
    ucCodScheme     = ( 0x70 & pucIeNtwkName[ucIndex] ) >> 4;
    /* 填充BIT */
    ucSpareBit      = pucIeNtwkName[ucIndex] & 0x07;
    pTmpNtwkName    = &pucIeNtwkName[ucIndex+1];

    if (0 == ucIeLen)
    {
        return;
    }

    if (ulLen == 0)
    {
        return;
    }

    pucDecodeNwName = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_MMA, ulLen);
    if (VOS_NULL_PTR == pucDecodeNwName)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_WARNING, "TAF_MMA_TranslateNtwkName2Str: Alloc memory fail.");
        return;
    }

    if (0 == ucCodScheme)
    {
        uc7BitCharNum   = (VOS_UINT8)(((ucIeLen -1) * 8) / 7);
        if (uc7BitCharNum < ulLen)
        {
            ulRslt = TAF_STD_UnPack7Bit(pTmpNtwkName, uc7BitCharNum, 0, pucDecodeNwName);

            if (VOS_OK != ulRslt)
            {
                MMA_WARNINGLOG("TAF_MMA_TranslateNtwkName2Str: TAF_STD_UnPack7Bit Err.");
            }

            /* 有7个填充BIT,最后一个字节解码后的数据无效，需要把数据长度减1 */
            if ((ucSpareBit == 7) && (uc7BitCharNum > 0))
            {
                uc7BitCharNum -= 1;
            }
            TAF_MMA_DefaultAlphaToAscii(pucDecodeNwName, uc7BitCharNum, (VOS_UINT8 *)pucNtwkName);
        }
    }
    else if (1 == ucCodScheme)
    {
        TAF_MMA_HexAlpha2AsciiString(ulLen, (VOS_UINT8 *)pucNtwkName,(VOS_UINT8 *)pTmpNtwkName, (TAF_UINT16)ucIeLen-1);
    }
    else
    {
    }

    MMA_FREE_MEM(pucDecodeNwName);
}
VOS_VOID TAF_MMA_QryPlmnPara(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_PLMN_ID_STRU                    stPlmnId;

    TAF_SDC_PLMN_ID_STRU               *pstCurrCampPlmnId = VOS_NULL_PTR;

    pstCurrCampPlmnId = TAF_SDC_GetCurrCampPlmnId();

    stPlmnId.Mcc = pstCurrCampPlmnId->ulMcc;
    stPlmnId.Mnc = pstCurrCampPlmnId->ulMnc;

    MMA_PlmnId2Bcd(&stPlmnId);

    /*使用数据上报函数将数据传至AT*/
    MMA_ParaQueryReport(MN_GetRealClientId(usClientId, WUEPS_PID_MMA),
                        ucOpId,
                        TAF_PH_PLMN_QRY_PARA,
                        TAF_ERR_NO_ERROR,
                        (VOS_VOID*)&stPlmnId);

    return;
}



VOS_VOID TAF_MMA_QryUserSrvStatePara(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_USER_SRV_STATE_STRU             stUserSrvState;

    VOS_UINT8                           ucCsServiceConnStatusFlag;

    ucCsServiceConnStatusFlag      = TAF_SDC_GetCsServiceConnStatusFlag();
    stUserSrvState.ulCsSrvExistFlg = VOS_FALSE;

    /* 获取当前是否有CS业务 */
    if ((VOS_TRUE == TAF_SDC_GetCsSrvExistFlg())
     || (VOS_TRUE == ucCsServiceConnStatusFlag))
    {
        stUserSrvState.ulCsSrvExistFlg = VOS_TRUE;
    }

    /* 目前只提供CS业务是否存在的查询，PS业务的查询不提供，固定填成0 */
    stUserSrvState.ulPsSrvExistFlg = VOS_FALSE;

    /*使用数据上报函数将数据传至AT*/
    MMA_ParaQueryReport(MN_GetRealClientId(usClientId, WUEPS_PID_MMA),
                        ucOpId,
                        TAF_PH_USER_SRV_STATE_QRY_PARA,
                        TAF_ERR_NO_ERROR,
                        (VOS_VOID*)&stUserSrvState);

    return;

}



VOS_VOID TAF_MMA_RcvTafMsgSimInsertInd(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    TAF_MMA_SIM_INSERT_IND_STRU        *pstSimInsertState
)
{
    TAF_MMA_SIM_INSERT_RSP_STRU        *pstSimInsertRsp;

    pstSimInsertRsp  = (TAF_MMA_SIM_INSERT_RSP_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                         WUEPS_PID_MMA,
                                                         sizeof(TAF_MMA_SIM_INSERT_RSP_STRU));

    if (VOS_NULL_PTR == pstSimInsertRsp)
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL_PTR, PS_PRINT_ERROR,
                "TAF_MMA_RcvTafMsgSimInsertInd: Memory Alloc Error for pMsg!");
        return;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR *)pstSimInsertRsp + VOS_MSG_HEAD_LENGTH,
                0x00,
                (sizeof(TAF_MMA_SIM_INSERT_RSP_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息头部信息 */
    pstSimInsertRsp->ulReceiverPid          = WUEPS_PID_AT;
    pstSimInsertRsp->ulMsgName              = AT_MMA_SIM_INSERT_RSP;
    pstSimInsertRsp->stAtAppCtrl.usClientId = ClientId;
    pstSimInsertRsp->stAtAppCtrl.ucOpId     = OpId;

    if (TAF_SIM_INSERT_STATE_REMOVE == pstSimInsertState->enSimInsertState)
    {
        if (USIMM_API_SUCCESS == USIMM_DeactiveCardReq(WUEPS_PID_MMA))
        {
            pstSimInsertRsp->ulResult = VOS_OK;
        }
        else
        {
            pstSimInsertRsp->ulResult = VOS_ERR;
        }
    }
    else
    {
        pstSimInsertRsp->ulResult = VOS_OK;
    }

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMA, pstSimInsertRsp))
    {
        PS_LOG(WUEPS_PID_MMA, VOS_NULL_PTR, PS_PRINT_ERROR,
                "TAF_MMA_RcvTafMsgSimInsertInd: Send Msg Error!");
    }

    return;
}
VOS_VOID TAF_MMA_ReadSpnFile(VOS_VOID)
{
    VOS_UINT32                          ulSpnExistFlag;
    USIMM_GET_FILE_INFO_STRU            stGetFileInfo;
    VOS_UINT8                           ucSimType;
    VOS_UINT32                          ulRet;

    /* 从USIM中获取SPN文件,文件ID:0x6F46 */
    ulSpnExistFlag = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_SPN);

    if (PS_USIM_SERVICE_AVAILIABLE == ulSpnExistFlag)
    {
        MMA_PhoneGetUsimType(&ucSimType);

        if (MMA_USIM_TYPE_USIM == ucSimType)
        {
            TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                        USIMM_UMTS_APP,
                        TAF_USIM_SPN_EFID,
                        0);

            ulRet = NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA, 0, &stGetFileInfo);

            if (USIMM_API_SUCCESS != ulRet)
            {
                MMA_ERRORLOG("TAF_MMA_ReadSpnFile():ERROR:Get File Req failed");
            }
            f_SpnCurrenReadType = TAF_PH_USIM_SPN;
        }
        else
        {
            TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo,
                            USIMM_GSM_APP,
                            TAF_USIM_SPN_EFID,
                            0);

            ulRet = NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMA, 0, &stGetFileInfo);

            if (USIMM_API_SUCCESS != ulRet)
            {
                MMA_ERRORLOG("TAF_MMA_ReadSpnFile():ERROR:Get File Req failed");
            }
            f_SpnCurrenReadType = TAF_PH_GSM_SPN;
        }

        /*启动定时器*/
        if (VOS_OK != NAS_StartRelTimer(&g_MmaSimTimer[1].MmaHTimer,
                                         WUEPS_PID_MMA,
                                         MMA_GET_FILE_TIME,
                                         TAF_MMA,
                                         TAF_USIM_SPN,
                                         VOS_RELTIMER_NOLOOP))
        {
            MMA_ERRORLOG("TAF_MMA_ReadSpnFile():ERROR:VOS_StartRelTimer runs failed!");
            g_MmaSimTimer[1].ucTimerStatus = MMA_TIMER_ERROR;
        }
        else
        {
            g_MmaSimTimer[1].ucTimerStatus = MMA_TIMER_RUN;
        }
    }
    else
    {
        gstMmaValue.stUsimInfo.UsimSpnInfo.SpnType     = TAF_PH_SPN_TYPE_UNKNOW;
        gstMmaValue.stUsimInfo.SimSpnInfo.SpnType     = TAF_PH_SPN_TYPE_UNKNOW;
    }
}


VOS_UINT32 TAF_MMA_IsUsimStatusChange_UsimmCardServiceAbsent(
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enPreSimStatus
)
{
    switch ( enPreSimStatus )
    {
        /* 当前SDC中卡状态是有效的，则不认为需要重启 */
        case TAF_SDC_USIM_STATUS_NO_PRESENT :
        case TAF_SDC_USIM_STATUS_SIM_PIN :
        case TAF_SDC_USIM_STATUS_SIM_PUK :
        case TAF_SDC_USIM_STATUS_NET_LCOK :
        case TAF_SDC_USIM_STATUS_IMSI_LCOK :
        case TAF_SDC_USIM_STATUS_READY_TIMEROUT:
            return VOS_FALSE;

        /* 否则都需要重启 */
        default:
            return VOS_TRUE;
    }

}


VOS_UINT32 TAF_MMA_IsUsimStatusChange_UsimmCardServiceAvailable(
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enPreSimStatus
)
{
    switch ( enPreSimStatus )
    {
        /* 当前SDC中卡状态是有效的，则不认为需要重启 */
        case TAF_SDC_USIM_STATUS_VALID :
        case TAF_SDC_USIM_STATUS_CS_UNVALID :
        case TAF_SDC_USIM_STATUS_PS_UNVALID :
        case TAF_SDC_USIM_STATUS_CS_PS_UNVALID :
        case TAF_SDC_USIM_STATUS_ROMSIM :

            return VOS_FALSE;

        /* 否则都需要重启 */
        default:
            return VOS_TRUE;
    }
}


VOS_UINT32 TAF_MMA_IsUsimStatusChange_UsimmCardServiceSimPin(
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enPreSimStatus
)
{
    switch ( enPreSimStatus )
    {
        /* 当前SDC中卡状态是有效的则认为改变，需要重启 */
        case TAF_SDC_USIM_STATUS_VALID :
        case TAF_SDC_USIM_STATUS_CS_UNVALID :
        case TAF_SDC_USIM_STATUS_PS_UNVALID :
        case TAF_SDC_USIM_STATUS_CS_PS_UNVALID :
        case TAF_SDC_USIM_STATUS_ROMSIM :

        case TAF_SDC_USIM_STATUS_BUTT:

            return VOS_TRUE;

        /* 否则都不需要重启 */
        default:
            return VOS_FALSE;
    }

}
#if (FEATURE_MULTI_MODEM == FEATURE_ON)

VOS_VOID TAF_MMA_SndMtcPowerStateInd(MTC_MODEM_POWER_STATE_ENUM_UINT8 enPowerState)
{
    VOS_UINT16                          ulLenth;
    MMA_MTC_POWER_STATE_IND_STRU       *pstPowerState = VOS_NULL_PTR;

    ulLenth = sizeof(MMA_MTC_POWER_STATE_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 分配消息空间 */
    pstPowerState  = (MMA_MTC_POWER_STATE_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA, ulLenth);
    if (VOS_NULL_PTR == pstPowerState)
    {
        MN_ERR_LOG("TAF_MMA_SndMtcPowerStateInd: Alloc Msg Failed!");
        return;
    }

    /* 清消息空间 */
    PS_MEM_SET((VOS_UINT8*)pstPowerState + VOS_MSG_HEAD_LENGTH, 0, ulLenth);

    /* 填充消息 */
    pstPowerState->stMsgHeader.ulReceiverPid      = UEPS_PID_MTC;
    pstPowerState->stMsgHeader.ulMsgName          = ID_MMA_MTC_POWER_STATE_IND;
    pstPowerState->enPowerState                   = enPowerState;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMA, pstPowerState))
    {
        MN_ERR_LOG("TAF_MMA_SndMtcPowerStateInd: Snd Msg Failed!");
    }

    return;

}


VOS_VOID TAF_MMA_SndMtcRatModeInd(
    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode,
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enUtranMode
)
{
    MMA_MTC_RAT_MODE_IND_STRU          *pstRatMode = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;

    pstRatMode = (MMA_MTC_RAT_MODE_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                 WUEPS_PID_MMA,
                                                 sizeof(MMA_MTC_RAT_MODE_IND_STRU));
    if( VOS_NULL_PTR == pstRatMode )
    {
        MN_ERR_LOG("TAF_MMA_SndMtcRatModeInd: Alloc Msg Failed!");
        return;
    }

    PS_MEM_SET(((VOS_UINT8 *)pstRatMode + VOS_MSG_HEAD_LENGTH),
               0x00,
               (sizeof(MMA_MTC_RAT_MODE_IND_STRU) - VOS_MSG_HEAD_LENGTH));

    pstRatMode->stMsgHeader.ulReceiverPid   = UEPS_PID_MTC;
    pstRatMode->stMsgHeader.ulMsgName       = ID_MMA_MTC_RAT_MODE_IND;

    switch (enSysMode)
    {
        case TAF_SDC_SYS_MODE_GSM:
            pstRatMode->enRatMode = MTC_RATMODE_GSM;
            break;
        case TAF_SDC_SYS_MODE_WCDMA:
            if (NAS_UTRANCTRL_UTRAN_MODE_FDD == enUtranMode)
            {
                pstRatMode->enRatMode = MTC_RATMODE_WCDMA;
            }
            else
            {
                pstRatMode->enRatMode = MTC_RATMODE_TDS;
            }
            break;
        case TAF_SDC_SYS_MODE_LTE:
            pstRatMode->enRatMode = MTC_RATMODE_LTE;
            break;
        default:
            pstRatMode->enRatMode = MTC_RATMODE_BUTT;
            break;
    }

    ulResult = PS_SEND_MSG(WUEPS_PID_MMA, pstRatMode);
    if( VOS_OK != ulResult )
    {
        MN_ERR_LOG("TAF_MMA_SndMtcRatModeInd: Send Msg Failed!");
    }

    return;
}

VOS_VOID TAF_MMA_SndMtcRegStatusInd(VOS_UINT8 ucIsUsimValidFlag)
{
    VOS_UINT16                          ulLenth;
    MMA_MTC_REG_STATUS_IND_STRU        *pstMsg = VOS_NULL_PTR;

    ulLenth = sizeof(MMA_MTC_REG_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 分配消息空间 */
    pstMsg  = (MMA_MTC_REG_STATUS_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA, ulLenth);
    if (VOS_NULL_PTR == pstMsg)
    {
        MN_ERR_LOG("TAF_MMA_SndMtcRegStatusInd: Alloc Msg Failed!");
        return;
    }

    /* 清消息空间 */
    PS_MEM_SET((VOS_UINT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0, ulLenth);

    /* 填充消息 */
    pstMsg->stMsgHeader.ulReceiverPid    = UEPS_PID_MTC;
    pstMsg->stMsgHeader.ulMsgName        = ID_MMA_MTC_REG_STATUS_IND;
    pstMsg->stMsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength         = sizeof(MMA_MTC_REG_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ucIsUsimValidFlg             = ucIsUsimValidFlag;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMA, pstMsg))
    {
        MN_ERR_LOG("TAF_MMA_SndMtcRegStatusInd: Snd Msg Failed!");
    }

    return;
}


#endif


#if (VOS_WIN32 == VOS_OS_VER)

VOS_VOID TAF_MMA_SetSimLockStatus(VOS_UINT8 ucSimLockStatus)
{
    g_stMmaMePersonalisationStatus.SimLockStatus = ucSimLockStatus;

}
#endif

VOS_VOID TAF_MMA_RcvMsgEOPlmnSetReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_SET_EOPLMN_LIST_STRU       *pstEOPlmnSetPara
)
{
    /* 保存上下文*/
    gstMmaValue.stEOPlmnSet.usClientId = usClientId;
    gstMmaValue.stEOPlmnSet.ucOpId     = ucOpId;

    /* 给MMC模块发送ID_MMA_MMC_EOPLMN_SET_REQ */
    TAF_MMA_SndMmcEOPlmnSetReq(pstEOPlmnSetPara);

    return;
}


VOS_UINT32 TAF_MMA_RcvMmcEOPlmnSetCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PHONE_EVENT_EOPLMN_SET_CNF_STRU          *pstEOPlmnSetCnf = VOS_NULL_PTR;
    MMC_MMA_EOPLMN_SET_CNF_STRU                  *pstMmcMsg       = VOS_NULL_PTR;

    pstMmcMsg   = (MMC_MMA_EOPLMN_SET_CNF_STRU *)pstMsg;

    pstEOPlmnSetCnf = (TAF_PHONE_EVENT_EOPLMN_SET_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                                              sizeof(TAF_PHONE_EVENT_EOPLMN_SET_CNF_STRU));

    if (VOS_NULL_PTR == pstEOPlmnSetCnf)
    {
        MN_ERR_LOG("TAF_MMA_RcvMmcEOPlmnSetCnf: Alloc Msg Failed!");
        return VOS_TRUE;
    }

    PS_MEM_SET(pstEOPlmnSetCnf, 0x00, sizeof(TAF_PHONE_EVENT_EOPLMN_SET_CNF_STRU));

    pstEOPlmnSetCnf->ulResult    = pstMmcMsg->ulResult;
    pstEOPlmnSetCnf->ClientId    = gstMmaValue.stEOPlmnSet.usClientId;
    pstEOPlmnSetCnf->OpId        = gstMmaValue.stEOPlmnSet.ucOpId;

    /* 填充消息头 */
    pstEOPlmnSetCnf->PhoneEvent  = TAF_MMA_EVT_USER_CFG_OPLMN_SET_CNF;

    MN_PH_SendMsg( pstEOPlmnSetCnf->ClientId,
                   (VOS_UINT8*)pstEOPlmnSetCnf,
                   sizeof(TAF_PHONE_EVENT_EOPLMN_SET_CNF_STRU) );

    PS_MEM_FREE(WUEPS_PID_MMA, pstEOPlmnSetCnf);

    return VOS_TRUE;
}
VOS_VOID TAF_MMA_RcvMsgEOPlmnQueryReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    VOS_UINT32                                              ulLen;
    TAF_PHONE_EVENT_EOPLMN_QRY_CNF_STRU                    *pstEOPlmnQryCnf = VOS_NULL_PTR;
    NAS_MMC_NVIM_USER_CFG_OPLMN_EXTEND_STRU                *pstNvimCfgOPlmnInfo = VOS_NULL_PTR;

    pstNvimCfgOPlmnInfo = (NAS_MMC_NVIM_USER_CFG_OPLMN_EXTEND_STRU*)PS_MEM_ALLOC(
                                                      WUEPS_PID_MMA,
                                                      sizeof(NAS_MMC_NVIM_USER_CFG_OPLMN_EXTEND_STRU));
    if (VOS_NULL_PTR == pstNvimCfgOPlmnInfo)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_RcvMsgEOPlmnQueryReq():ERROR: Memory Alloc Error");
        return;
    }
    PS_MEM_SET(pstNvimCfgOPlmnInfo, 0x00, sizeof(NAS_MMC_NVIM_USER_CFG_OPLMN_EXTEND_STRU));


    pstEOPlmnQryCnf = (TAF_PHONE_EVENT_EOPLMN_QRY_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                                              sizeof(TAF_PHONE_EVENT_EOPLMN_QRY_CNF_STRU));

    if (VOS_NULL_PTR == pstEOPlmnQryCnf)
    {
        MN_ERR_LOG("TAF_MMA_RcvMsgEOPlmnQueryReq: Alloc Msg Failed!");
        PS_MEM_FREE(WUEPS_PID_MMA, pstNvimCfgOPlmnInfo);
        return;
    }

    PS_MEM_SET(pstEOPlmnQryCnf, 0x00, sizeof(TAF_PHONE_EVENT_EOPLMN_QRY_CNF_STRU));

    /* 读NV项en_NV_Item_USER_CFG_OPLMN_EXTEND_LIST，失败，直接返回 */
    NV_GetLength(en_NV_Item_USER_CFG_OPLMN_EXTEND_LIST, &ulLen);

    if (NV_OK != NV_Read(en_NV_Item_USER_CFG_OPLMN_EXTEND_LIST,
                         pstNvimCfgOPlmnInfo, ulLen))
    {
        MN_ERR_LOG("NAS_MMC_ReadUserCfgOPlmnInfoNvim():WARNING: read en_NV_Item_USER_CFG_OPLMN_EXTEND_LIST Error");
        /* 填充消息 */
        pstEOPlmnQryCnf->ulResult = TAF_ERR_ERROR;
    }
    else
    {
        /* 填充消息 */
        pstEOPlmnQryCnf->ulResult = TAF_ERR_NO_ERROR;
    }

    /* 填充消息头 */
    pstEOPlmnQryCnf->PhoneEvent = TAF_MMA_EVT_USER_CFG_OPLMN_QRY_CNF;
    pstEOPlmnQryCnf->OpId       = ucOpId;
    pstEOPlmnQryCnf->ClientId   = usClientId;

    PS_MEM_CPY(pstEOPlmnQryCnf->aucVersion,
                    pstNvimCfgOPlmnInfo->aucVersion,
                    NAS_MMC_NVIM_MAX_USER_OPLMN_VERSION_LEN * sizeof(VOS_UINT8));

    pstEOPlmnQryCnf->usOPlmnNum = pstNvimCfgOPlmnInfo->usOplmnListNum;

    PS_MEM_CPY(pstEOPlmnQryCnf->aucOPlmnList,
                    pstNvimCfgOPlmnInfo->aucOPlmnList,
                    NAS_MMC_NVIM_MAX_USER_CFG_OPLMN_DATA_EXTEND_LEN * sizeof(VOS_UINT8));

    MN_PH_SendMsg( pstEOPlmnQryCnf->ClientId,
                   (VOS_UINT8*)pstEOPlmnQryCnf,
                   sizeof(TAF_PHONE_EVENT_EOPLMN_QRY_CNF_STRU) );

    PS_MEM_FREE(WUEPS_PID_MMA, pstNvimCfgOPlmnInfo);
    PS_MEM_FREE(WUEPS_PID_MMA, pstEOPlmnQryCnf);
    return;
}




VOS_VOID  TAF_DRVAPI_PWRCTRL_SLEEPVOTE_UNLOCK(
    PWC_CLIENT_ID_E                     enPwcClientId
)
{
   VOS_UINT32                           ulRslt;
   PWC_CLIENT_ID_E                      enClientId;

#if defined(INSTANCE_1)

    switch (enPwcClientId)
    {
        case PWRCTRL_SLEEP_NAS:
            enClientId = PWRCTRL_SLEEP_NAS_1;
            break;

        case PWRCTRL_SLEEP_FTM:
            enClientId = PWRCTRL_SLEEP_FTM_1;
            break;

        default:
            enClientId = enPwcClientId;
            break;
    }
#else
    enClientId = enPwcClientId;
#endif

    if (VOS_TRUE == TAF_MMA_GetMmaLogInfoFlag())
    {
        vos_printf("\n*TAF_DRVAPI_PWRCTRL_SLEEPVOTE_UNLOCK:enClientId = %d\n", enClientId);
    }

    ulRslt = DRV_PWRCTRL_SLEEPVOTE_UNLOCK(enClientId);

    TAF_MMA_LogDrvApiPwrCtrlSleepVoteUnlock(ulRslt, enClientId);
}




VOS_VOID  TAF_DRVAPI_PWRCTRL_SLEEPVOTE_LOCK(
    PWC_CLIENT_ID_E                     enPwcClientId
)
{
   VOS_UINT32                           ulRslt;
   PWC_CLIENT_ID_E                      enClientId;

#if defined(INSTANCE_1)

    switch (enPwcClientId)
    {
        case PWRCTRL_SLEEP_NAS:
            enClientId = PWRCTRL_SLEEP_NAS_1;
            break;

        case PWRCTRL_SLEEP_FTM:
            enClientId = PWRCTRL_SLEEP_FTM_1;
            break;

        default:
            enClientId = enPwcClientId;
            break;
    }
#else
    enClientId = enPwcClientId;
#endif

    if (VOS_TRUE == TAF_MMA_GetMmaLogInfoFlag())
    {
        vos_printf("\n*TAF_DRVAPI_PWRCTRL_SLEEPVOTE_LOCK:enClientId = %d\n", enClientId);
    }

    ulRslt = DRV_PWRCTRL_SLEEPVOTE_LOCK(enClientId);
    
    
    TAF_MMA_LogDrvApiPwrCtrlSleepVoteLock(ulRslt, enClientId);
}


VOS_VOID TAF_MMA_UpdateSimRegStatus(VOS_VOID)
{
    TAF_SDC_USIM_STATUS_ENUM_UINT8      ucSimStatus;

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    VOS_UINT32                          ulOldUsimValidStatus;
    VOS_UINT32                          ulNewUsimValidStatus;

    ulOldUsimValidStatus = TAF_SDC_IsUsimStausValid();
#endif

    ucSimStatus = TAF_SDC_GetSimStatus();

    if ((TAF_SDC_USIM_STATUS_VALID  == ucSimStatus)
     || (TAF_SDC_USIM_STATUS_ROMSIM == ucSimStatus))
    {
        TAF_SDC_SetSimCsRegStatus(VOS_TRUE);
        TAF_SDC_SetSimPsRegStatus(VOS_TRUE);
    }
    else
    {
        TAF_SDC_SetSimCsRegStatus(VOS_FALSE);
        TAF_SDC_SetSimPsRegStatus(VOS_FALSE);
    }

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    ulNewUsimValidStatus = TAF_SDC_IsUsimStausValid();

    /* 如果cs ps卡状态发生改变需要通知mtc */
    if (ulOldUsimValidStatus != ulNewUsimValidStatus)
    {
        TAF_MMA_SndMtcRegStatusInd((VOS_UINT8)ulNewUsimValidStatus);
    }
#endif

    return;
}
VOS_UINT32 TAF_MMA_StartNetScanProtectTimer(VOS_VOID)
{
    VOS_UINT32                          ulRet;

    ulRet = VOS_OK;

    /* 定时器不在运行，则开启定时器 */
    if (MMA_TIMER_RUN != g_stNetScanProtectTimer.ucTimerStatus)
    {
        ulRet = NAS_StartRelTimer(&g_stNetScanProtectTimer.MmaHTimer,
                                  WUEPS_PID_MMA,
                                  TI_TAF_MMA_NET_SCAN_TIMER_LEN,
                                  TAF_MMA,
                                  TI_TAF_MMA_NET_SCAN_TIMER,
                                  VOS_RELTIMER_NOLOOP );

        if( VOS_OK !=  ulRet)
        {
            MMA_WARNINGLOG("TAF_MMA_NetScanProtectTimerProc():ERROR:Start TI_TAF_MMA_PNN_LIST_MEM_PROTECT_TIMER Timer Error!");
        }
        else
        {
            g_stNetScanProtectTimer.ucTimerStatus = MMA_TIMER_RUN;
        }
    }

    return ulRet;
}
VOS_UINT32 TAF_MMA_StopNetScanProtectTimer(VOS_VOID)
{
    VOS_UINT32                          ulRet;

    ulRet                               = VOS_OK;

    /* 定时器不处于停止状态，则停止定时器 */
    if ( MMA_TIMER_RUN == g_stNetScanProtectTimer.ucTimerStatus )
    {
        ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                 TI_TAF_MMA_NET_SCAN_TIMER,
                                 &(g_stNetScanProtectTimer.MmaHTimer));
        if (VOS_OK != ulRet)
        {
            MMA_WARNINGLOG("TAF_MMA_NetScanProtectTimerProc():WARNING:NAS_StopRelTimer failed");
        }

        /*设置停止定时器标志*/
        g_stNetScanProtectTimer.ucTimerStatus = MMA_TIMER_STOP;
    }

    return ulRet;
}


VOS_UINT32 TAF_MMA_StartAbortNetScanProtectTimer(VOS_VOID)
{
    VOS_UINT32                          ulRet;

    ulRet = VOS_OK;

    /* 开启定时器 */
    if (MMA_TIMER_RUN != g_stAbortNetScanProtectTimer.ucTimerStatus)
    {
        ulRet = NAS_StartRelTimer(&g_stAbortNetScanProtectTimer.MmaHTimer,
                                  WUEPS_PID_MMA,
                                  TI_TAF_MMA_ABORT_NET_SCAN_TIMER_LEN,
                                  TAF_MMA,
                                  TI_TAF_MMA_ABORT_NET_SCAN_TIMER,
                                  VOS_RELTIMER_NOLOOP );

        if( VOS_OK !=  ulRet)
        {
            MMA_WARNINGLOG("TAF_MMA_AbortNetScanProtectTimerProc():ERROR:Start TI_TAF_MMA_ABORT_NET_SCAN_TIMER Timer Error!");
        }
        else
        {
            g_stAbortNetScanProtectTimer.ucTimerStatus = MMA_TIMER_RUN;
        }
    }

    return ulRet;
}
VOS_UINT32 TAF_MMA_StopAbortNetScanProtectTimer(VOS_VOID)
{
    VOS_UINT32                          ulRet;

    ulRet = VOS_OK;

    /* 如果该定时器处于运行状态，则停止 */
    if ( MMA_TIMER_RUN == g_stAbortNetScanProtectTimer.ucTimerStatus )
    {
        ulRet = NAS_StopRelTimer(WUEPS_PID_MMA,
                                 TI_TAF_MMA_ABORT_NET_SCAN_TIMER,
                                 &(g_stAbortNetScanProtectTimer.MmaHTimer));
        if (VOS_OK != ulRet)
        {
            MMA_WARNINGLOG("TAF_MMA_AbortNetScanProtectTimerProc():WARNING:NAS_StopRelTimer failed");
        }

        /*设置停止定时器标志*/
        g_stAbortNetScanProtectTimer.ucTimerStatus = MMA_TIMER_STOP;
    }

    return ulRet;
}



VOS_VOID TAF_MMA_NetScanTimerExpired(VOS_VOID)
{
    TAF_MMA_StopNetScanProtectTimer();

    /* 给MMC发送停止扫描请求 */
    TAF_MMA_SndMmcAbortNetScanMsgReq();

    TAF_MMA_StartAbortNetScanProtectTimer();

    return;
}
VOS_VOID TAF_MMA_AbortNetScanTimerExpired(VOS_VOID)
{
    TAF_MMA_NET_SCAN_CNF_STRU           stNetScanCnf;

    PS_MEM_SET(&stNetScanCnf, 0x00, sizeof(TAF_MMA_NET_SCAN_CNF_STRU));

    TAF_MMA_StopAbortNetScanProtectTimer();

    /* 填充消息头 */
    stNetScanCnf.PhoneEvent             = TAF_MMA_EVT_NET_SCAN_CNF;
    stNetScanCnf.usClientId             = gstMmaValue.stNetScan.usClientId;
    stNetScanCnf.ucOpId                 = gstMmaValue.stNetScan.ucOpId;

    stNetScanCnf.enResult               = TAF_MMA_NET_SCAN_RESULT_FAILURE;
    stNetScanCnf.enCause                = TAF_MMA_NET_SCAN_CAUSE_TIMER_EXPIRED;

    /* 给AT模块回复定时器超时异常 */
    MN_PH_SendMsg( stNetScanCnf.usClientId,
                   (VOS_UINT8*)&stNetScanCnf,
                   sizeof(TAF_MMA_NET_SCAN_CNF_STRU) );

    return;
}




VOS_VOID TAF_MMA_RcvTafMsgNetScanReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_NET_SCAN_REQ_STRU          *pstNetScanSetPara
)
{
    TAF_MMA_NET_SCAN_CNF_STRU           stNetScanCnf;

    PS_MEM_SET(&stNetScanCnf, 0x00, sizeof(TAF_MMA_NET_SCAN_CNF_STRU));

    if ( STA_FSM_ENABLE != g_StatusContext.ulFsmState )
    {
        stNetScanCnf.PhoneEvent = TAF_MMA_EVT_NET_SCAN_CNF;
        stNetScanCnf.usClientId = usClientId;
        stNetScanCnf.ucOpId     = ucOpId;
        stNetScanCnf.enResult   = TAF_MMA_NET_SCAN_RESULT_FAILURE;
        stNetScanCnf.enCause    = TAF_MMA_NET_SCAN_CAUSE_MMA_STATE_DISABLE;

        /* 通过电话管理上报失败 */
        MN_PH_SendMsg( stNetScanCnf.usClientId,
                       (VOS_UINT8*)&stNetScanCnf,
                       sizeof(TAF_MMA_NET_SCAN_CNF_STRU) );

        return;
    }

    /* 保存^NETSCAN下发的ClientId和OpId */
    gstMmaValue.stNetScan.usClientId = usClientId;
    gstMmaValue.stNetScan.ucOpId     = ucOpId;

    /* 给MMC模块发送NETSCAN REQ */
    TAF_MMA_SndMmcNetScanMsgReq(pstNetScanSetPara);

    /* 启动STA_TIMER_NET_SCAN 108s定时器，如果超时未收到MMC的回复则给MMC发送ABORT NET SCAN REQ */
    TAF_MMA_StartNetScanProtectTimer();

    return;
}


VOS_VOID TAF_MMA_RcvTafMsgAbortNetScanReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    if ( MMA_TIMER_RUN == g_stNetScanProtectTimer.ucTimerStatus )
    {
        TAF_MMA_StopNetScanProtectTimer();

        /* 保存^NETSCAN下发的ClientId和OpId */
        gstMmaValue.stNetScan.usClientId = usClientId;
        gstMmaValue.stNetScan.ucOpId     = ucOpId;

        /* 给MMC模块发送打断NETSCAN REQ */
        TAF_MMA_SndMmcAbortNetScanMsgReq();

        /* 启动TI_TAF_MMA_ABORT_NET_SCAN_TIMER 10s定时器 */
        TAF_MMA_StartAbortNetScanProtectTimer();
    }

    /* MMA保护定时器超时后，给MMC发送ABORT请求后，用户此时再下发打断请求，
       则忽略用户下发的打断请求 */

    return;
}



#if (FEATURE_MULTI_MODEM == FEATURE_ON)

VOS_VOID TAF_MMA_SndMtcCurrCampPlmnInfoInd(
    MMA_MTC_CURR_CAMP_PLMN_INFO_IND_STRU                   *pstPlmnInfo
)
{
    MMA_MTC_CURR_CAMP_PLMN_INFO_IND_STRU                   *pstCurrCampPlmnInfo = VOS_NULL_PTR;
    VOS_UINT32                                              ulResult;

    pstCurrCampPlmnInfo = (MMA_MTC_CURR_CAMP_PLMN_INFO_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                              WUEPS_PID_MMA,
                                              sizeof(MMA_MTC_CURR_CAMP_PLMN_INFO_IND_STRU));

    if( VOS_NULL_PTR == pstCurrCampPlmnInfo )
    {
        MN_ERR_LOG("TAF_MMA_SndMtcCurrCampPlmnInfoInd: Alloc Msg Failed!");
        return;
    }

    PS_MEM_SET(((VOS_UINT8 *)pstCurrCampPlmnInfo + VOS_MSG_HEAD_LENGTH),
               0x00,
               (sizeof(MMA_MTC_CURR_CAMP_PLMN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH));

    pstCurrCampPlmnInfo->stMsgHeader.ulReceiverPid   = UEPS_PID_MTC;
    pstCurrCampPlmnInfo->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstCurrCampPlmnInfo->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstCurrCampPlmnInfo->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstCurrCampPlmnInfo->stMsgHeader.ulMsgName       = ID_MMA_MTC_CURR_CAMP_PLMN_INFO_IND;
    pstCurrCampPlmnInfo->enRatMode                   = pstPlmnInfo->enRatMode;
    pstCurrCampPlmnInfo->ucIsForbiddenPlmnFlag       = pstPlmnInfo->ucIsForbiddenPlmnFlag;
    pstCurrCampPlmnInfo->ucPsAttachAllowFlag         = pstPlmnInfo->ucPsAttachAllowFlag;
    pstCurrCampPlmnInfo->stPlmnId.ulMcc              = pstPlmnInfo->stPlmnId.ulMcc;
    pstCurrCampPlmnInfo->stPlmnId.ulMnc              = pstPlmnInfo->stPlmnId.ulMnc;
    pstCurrCampPlmnInfo->ucPsSupportFlg              = pstPlmnInfo->ucPsSupportFlg;

    ulResult = PS_SEND_MSG(WUEPS_PID_MMA, pstCurrCampPlmnInfo);

    if( VOS_OK != ulResult )
    {
        MN_ERR_LOG("TAF_MMA_SndMtcCurrCampPlmnInfoInd: Send Msg Failed!");
    }

    return;
}


VOS_VOID TAF_MMA_SndMtcSysInfo(
    VOS_UINT8                           ucIsFobbiddenPlmnFlag,
    VOS_UINT8                           ucPsSupportFlg
)
{
    MMA_MTC_CURR_CAMP_PLMN_INFO_IND_STRU   stSndMtcPlmnInfo;
    TAF_SDC_PLMN_ID_STRU                  *pstCurrCampPlmnId = VOS_NULL_PTR;

    pstCurrCampPlmnId = TAF_SDC_GetCurrCampPlmnId();

    /* 给MTC模块上报当前接入模式 */
    TAF_MMA_SndMtcRatModeInd(TAF_SDC_GetSysMode(), NAS_UTRANCTRL_GetCurrUtranMode());

    /* 通知mtc 当前驻留网络接入技术和是否禁止信息 */
    PS_MEM_SET(&stSndMtcPlmnInfo, 0, sizeof(stSndMtcPlmnInfo));
    TAF_MMA_ConvertRatModeToMtc(TAF_SDC_GetSysMode(), NAS_UTRANCTRL_GetCurrUtranMode(),
                                &stSndMtcPlmnInfo.enRatMode);

    stSndMtcPlmnInfo.ucIsForbiddenPlmnFlag  = ucIsFobbiddenPlmnFlag;
    stSndMtcPlmnInfo.ucPsAttachAllowFlag    = TAF_SDC_GetPsAttachAllowFlg();
    stSndMtcPlmnInfo.stPlmnId.ulMcc         = pstCurrCampPlmnId->ulMcc;
    stSndMtcPlmnInfo.stPlmnId.ulMnc         = pstCurrCampPlmnId->ulMnc;
    stSndMtcPlmnInfo.ucPsSupportFlg         = ucPsSupportFlg;
    TAF_MMA_SndMtcCurrCampPlmnInfoInd(&stSndMtcPlmnInfo);

    return;
}


VOS_VOID TAF_MMA_ConvertRatModeToMtc(
    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode,
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enUtranMode,
    MTC_RATMODE_ENUM_UINT8             *pucRatMode
)
{
    switch (enSysMode)
    {
        case TAF_SDC_SYS_MODE_GSM:
            *pucRatMode = MTC_RATMODE_GSM;
            break;

        case TAF_SDC_SYS_MODE_WCDMA:
            if (NAS_UTRANCTRL_UTRAN_MODE_FDD == enUtranMode)
            {
                *pucRatMode = MTC_RATMODE_WCDMA;
            }
            else
            {
                *pucRatMode = MTC_RATMODE_TDS;
            }
            break;

        case TAF_SDC_SYS_MODE_LTE:
            *pucRatMode = MTC_RATMODE_LTE;
            break;
        default:
            *pucRatMode = MTC_RATMODE_BUTT;
            break;
    }

    return;
}

#endif



VOS_VOID TAF_MMA_BuildMmaCtrlInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CTRL_STRU                  *pstCtrlInfo
)
{
    pstCtrlInfo->ulModuleId = ulModuleId;
    pstCtrlInfo->usClientId = usClientId;
    pstCtrlInfo->ucOpId     = ucOpId;

    return;
}


VOS_UINT32 TAF_MMA_IsPowerOnCLInterWork(VOS_VOID)
{
    /* 当前NV配置为非CL互操作模式 */
    if (VOS_FALSE == TAF_SDC_IsConfigCLInterWork())
    {
        return VOS_FALSE;
    }

    /* 模式设置状态机在运行,则返回VOS_TRUE */
    if (TAF_MMA_FSM_PHONE_MODE == TAF_MMA_GetCurrFsmId())
    {
        return VOS_TRUE;
    }

    /* 模式状态机不在运行且当前不为开机模式，则返回VOS_FALSE */
    if (TAF_SDC_PHONE_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        return VOS_FALSE;
    }

    /* 当前为开机模式，则返回VOS_TRUE */
    return VOS_TRUE;
}


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif


