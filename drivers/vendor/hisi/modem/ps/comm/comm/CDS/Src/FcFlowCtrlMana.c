


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "CDS.h"
#include "QosFcCommon.h"
#include "OmApi.h"
#if (CDS_FEATURE_ON == CDS_FEATURE_LTE)
#include "LPsNvInterface.h"
#endif
#include "FcFlowCtrlMana.h"
#include "TtfNvInterface.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_FC_FLOWCTRL_MANA_C
/*lint +e767*/




/*****************************************************************************
  2 外部函数声明
*****************************************************************************/

/******************************************************************************
   3 私有定义
******************************************************************************/

/******************************************************************************
   4 全局变量定义
******************************************************************************/
#if (CDS_FEATURE_ON == CDS_FEATURE_LTE)
FLOWCTRL_LTE_CONFIG_STRU        g_stFcLteConfig = \
                                {{4,0,{0,0},\
                                   {50000,40000,35000,30000,\
                                   25000,20000,15000,10000,\
                                   5000,2000,0,0}, 200},\
                                   {5,20,10,2000,10,5,60,98,90,0}};
#endif

FLOWCTRL_CDS_CONFIG_STRU        g_stFcCdsConfig = {0, {80,50,20,95,100,200,300,400}, {50,80}};

FC_LTE_CPU_CONTROL_STRU         g_stFcLteCpuCtrl;

FC_LTE_TEMPERATURE_CONTROL_STRU g_stFcLteTemperatureCtrl;

FC_CDS_DL_CONTROL_STRU          g_stFcCdsDlCtrl;

/* CPU流控BSR乘数 */
VOS_DOUBLE                      g_dBsrCpuModu = 1.0;
VOS_DOUBLE                      g_dBsrCpuLimit = 0.02;

/* HARQ丢包率 */
VOS_UINT32  g_ulHarqKeepRate = 100;

VOS_UINT32  g_ulUlFcThroughputStat = 0;
VOS_UINT32  g_ulUlDowngradeFlag = PS_FALSE ;
VOS_UINT32  g_ulUlThroughputMax = FC_UL_ENTRY_THROUGHPUT_INIT;

FC_CL_STATS_INFO_STRU    g_stFcCLStats = {0};

#if (CDS_FEATURE_ON == CDS_FEATURE_LTE)
extern VOS_UINT32 LUP_GetCurrentTime(VOS_VOID);
/* extern VOS_VOID   LUP_RelDataReq(VOS_VOID); */
#endif

/*extern VOS_VOID BSP_PWRCTRL_GetCcpuLoadCB(PWRCTRLFUNCPTR pFunc);*/

/******************************************************************************
   5 函数实现
******************************************************************************/
#if (CDS_FEATURE_ON == CDS_FEATURE_LTE)

VOS_VOID Fc_LteConfig2Default(VOS_VOID)
{
    g_stFcLteConfig.stFcCpuConfig.usFirstDelay = 5;
    g_stFcLteConfig.stFcCpuConfig.usUlDownRate = 20;
    g_stFcLteConfig.stFcCpuConfig.usUlUpRate = 10;
    g_stFcLteConfig.stFcCpuConfig.usUlMinThr = 2000;
    g_stFcLteConfig.stFcCpuConfig.usHqDownRate = 10;
    g_stFcLteConfig.stFcCpuConfig.usHqUpRate = 5;
    g_stFcLteConfig.stFcCpuConfig.usHqMaxDiscardRate = 60;
    g_stFcLteConfig.stFcCpuConfig.usDowngradeThres = 98;
    g_stFcLteConfig.stFcCpuConfig.usUpgradeThres = 90;

    g_stFcLteConfig.stFcTemperatureConfig.ucInitialPos = 4;
    g_stFcLteConfig.stFcTemperatureConfig.ucZeroSupport = 0;
    g_stFcLteConfig.stFcTemperatureConfig.ulMinBsrThr = 200;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[0] = 50000;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[1] = 40000;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[2] = 35000;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[3] = 30000;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[4] = 25000;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[5] = 20000;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[6] = 15000;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[7] = 10000;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[8] = 5000;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[9] = 2000;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[10] = 0;
    g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[11] = 0;

    return;
}


VOS_UINT32 FC_LteConfigcheck(VOS_VOID)
{
    VOS_UINT32 i = 0;

    if(g_stFcLteConfig.stFcCpuConfig.usUlDownRate >= 100)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS, "FC_LteConfigcheck Error config stFcCpuConfig.usUlDownRate over 100:", g_stFcLteConfig.stFcCpuConfig.usUlDownRate);
        return PS_FAIL;
    }
    if(g_stFcLteConfig.stFcCpuConfig.usUlUpRate >= 100)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS, "FC_LteConfigcheck Error config stFcCpuConfig.usUlUpRate over 100:", g_stFcLteConfig.stFcCpuConfig.usUlUpRate);
        return PS_FAIL;
    }
    if(g_stFcLteConfig.stFcCpuConfig.usHqDownRate >= 100)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS, "FC_LteConfigcheck Error config stFcCpuConfig.usHqDownRate over 100:", g_stFcLteConfig.stFcCpuConfig.usHqDownRate);
        return PS_FAIL;
    }
    if(g_stFcLteConfig.stFcCpuConfig.usHqUpRate >= 100)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS, "FC_LteConfigcheck Error config stFcCpuConfig.usHqUpRate over 100:", g_stFcLteConfig.stFcCpuConfig.usHqUpRate);
        return PS_FAIL;
    }
    if(g_stFcLteConfig.stFcCpuConfig.usDowngradeThres >= 100)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS, "FC_LteConfigcheck Error config stFcCpuConfig.usDowngradeThres over 100:", g_stFcLteConfig.stFcCpuConfig.usDowngradeThres);
        return PS_FAIL;
    }
    if(g_stFcLteConfig.stFcCpuConfig.usUpgradeThres >= g_stFcLteConfig.stFcCpuConfig.usDowngradeThres)
    {
        CDS_ERROR_LOG2(UEPS_PID_CDS, "FC_LteConfigcheck Error config stFcCpuConfig.usUpgradeThres greater than stFcCpuConfig.usDowngradeThres:",
                                    g_stFcLteConfig.stFcCpuConfig.usUpgradeThres,
                                    g_stFcLteConfig.stFcCpuConfig.usDowngradeThres);
        return PS_FAIL;
    }

    if(g_stFcLteConfig.stFcTemperatureConfig.ucInitialPos >= FC_UL_THROUGHPUT_THRES_CNT_NV)
    {
        CDS_ERROR_LOG2(UEPS_PID_CDS, "FC_LteConfigcheck Error config stFcTemperatureConfig.ucInitialPos beyond :", g_stFcLteConfig.stFcTemperatureConfig.ucInitialPos, FC_UL_THROUGHPUT_THRES_CNT_NV);
        return PS_FAIL;
    }
    if(g_stFcLteConfig.stFcTemperatureConfig.ucZeroSupport > 1)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS, "FC_LteConfigcheck Error config stFcTemperatureConfig.ucZeroSupport over 1:", g_stFcLteConfig.stFcTemperatureConfig.ucZeroSupport);
        return PS_FAIL;
    }

    for(i = 0; i < FC_UL_THROUGHPUT_THRES_CNT_NV-1; i++)
    {
        if(0 == g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[i])
        {
            break;
        }

        if(g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[i] < g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[i+1])
        {
            CDS_ERROR_LOG(UEPS_PID_CDS, "FC_LteConfigcheck: g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps must be descending order!");
            return PS_FAIL;
        }
    }

    return PS_SUCC;
}


VOS_VOID Fc_KbpsToBsr(VOS_VOID)
{
    VOS_UINT32                  i;

    g_stFcLteCpuCtrl.ulUlMinBsr = g_stFcLteConfig.stFcCpuConfig.usUlMinThr*g_stFcLteTemperatureCtrl.ulBsrTimerLen/8;
    g_dBsrCpuLimit = g_stFcLteCpuCtrl.ulUlMinBsr / 150000.0;

    for(i = 0; i < g_stFcLteTemperatureCtrl.ucBsrThresCnt; i++)
    {
        g_stFcLteTemperatureCtrl.ulTemperSteps[i] = g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[i]*g_stFcLteTemperatureCtrl.ulBsrTimerLen/8;
    }
}


VOS_VOID Fc_BsrTimerSetting( VOS_UINT32 ulBsrTimer )
{
    if(g_stFcLteTemperatureCtrl.ulBsrTimerLen != ulBsrTimer)
    {
        g_stFcLteTemperatureCtrl.ulBsrTimerLen = ulBsrTimer;
        /* BSR周期改变，重新计算 */
        Fc_KbpsToBsr();

    }
}


#endif
VOS_VOID Fc_CdsConfig2Default(VOS_VOID)
{
    g_stFcCdsConfig.ulFcEnableMask = 0;

    g_stFcCdsConfig.stQosFcConfig.ulPktCntLimit = 80;
    g_stFcCdsConfig.stQosFcConfig.ulTimerLen = 50;

    g_stFcCdsConfig.stQosFcConfig.ulRandomDiscardRate = 20;
    g_stFcCdsConfig.stQosFcConfig.ulDiscardRate = 95;

    g_stFcCdsConfig.stQosFcConfig.ulWarningThres = 100;
    g_stFcCdsConfig.stQosFcConfig.ulDiscardThres = 200;
    g_stFcCdsConfig.stQosFcConfig.ulRandomDiscardThres = 300;
    g_stFcCdsConfig.stQosFcConfig.ulRestoreThres = 400;

    g_stFcCdsConfig.stFcCdsDlConfig.ulDiscardThres = 50;
    g_stFcCdsConfig.stFcCdsDlConfig.ulDiscardRate = 80;
    return;
}


VOS_UINT32 FC_CdsConfigcheck(VOS_VOID)
{
    if(g_stFcCdsConfig.stFcCdsDlConfig.ulDiscardRate >= 100)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS, "FC_CdsConfigcheck Error config stFcCdsDlConfig.ulDiscardRate over 100:", g_stFcCdsConfig.stFcCdsDlConfig.ulDiscardRate);
        return PS_FAIL;
    }
    if(g_stFcCdsConfig.stQosFcConfig.ulDiscardRate >= 100)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS, "FC_CdsConfigcheck Error config stQosFcConfig.ulDiscardRate over 100:", g_stFcCdsConfig.stQosFcConfig.ulDiscardRate);
        return PS_FAIL;
    }
    if(g_stFcCdsConfig.stQosFcConfig.ulRandomDiscardRate >= g_stFcCdsConfig.stQosFcConfig.ulDiscardRate)
    {
        CDS_ERROR_LOG2(UEPS_PID_CDS, "FC_CdsConfigcheck Error config stQosFcConfig.ulRandomDiscardRate greater than stQosFcConfig.ulDiscardRate:", g_stFcCdsConfig.stQosFcConfig.ulRandomDiscardRate, g_stFcCdsConfig.stQosFcConfig.ulDiscardRate);
        return PS_FAIL;
    }
    if(g_stFcCdsConfig.stQosFcConfig.ulWarningThres >= g_stFcCdsConfig.stQosFcConfig.ulDiscardThres)
    {
        CDS_ERROR_LOG2(UEPS_PID_CDS, "FC_CdsConfigcheck Error config stQosFcConfig.ulWarningThres greater than stQosFcConfig.ulDiscardThres:", g_stFcCdsConfig.stQosFcConfig.ulWarningThres, g_stFcCdsConfig.stQosFcConfig.ulDiscardThres);
        return PS_FAIL;
    }

    if(g_stFcCdsConfig.stQosFcConfig.ulDiscardThres >= g_stFcCdsConfig.stQosFcConfig.ulRandomDiscardThres)
    {
        CDS_ERROR_LOG2(UEPS_PID_CDS, "FC_CdsConfigcheck Error config stQosFcConfig.ulDiscardThres greater than stQosFcConfig.ulRandomDiscardThres:", g_stFcCdsConfig.stQosFcConfig.ulDiscardThres, g_stFcCdsConfig.stQosFcConfig.ulRandomDiscardThres);
        return PS_FAIL;
    }
    if(g_stFcCdsConfig.stQosFcConfig.ulRandomDiscardThres >= g_stFcCdsConfig.stQosFcConfig.ulRestoreThres)
    {
        CDS_ERROR_LOG2(UEPS_PID_CDS, "FC_CdsConfigcheck Error config stQosFcConfig.ulRandomDiscardThres greater than stQosFcConfig.ulRestoreThres:", g_stFcCdsConfig.stQosFcConfig.ulRandomDiscardThres, g_stFcCdsConfig.stQosFcConfig.ulRestoreThres);
        return PS_FAIL;
    }

    return PS_SUCC;
}


VOS_VOID Fc_LteStatusInit(VOS_VOID)
{
    /* 复位流控状态 */
    g_dBsrCpuModu = 1.0;
    g_ulHarqKeepRate = 100;

    g_ulUlDowngradeFlag = PS_FALSE;
    g_ulUlThroughputMax = FC_UL_ENTRY_THROUGHPUT_INIT;
    g_stFcLteTemperatureCtrl.ucCurrPos = FC_UL_THROUGHPUT_THRES_CNT;
}
VOS_VOID Fc_LteInit(VOS_VOID)
{
    VOS_UINT32                  ulReturnCode;

#if (CDS_FEATURE_ON == CDS_FEATURE_LTE)
    VOS_UINT32                  i;

    /* 从NV读取流控配置信息 */
    /*lint -e718*/
    /*lint -e746*/
    /*lint -e732*/
    /*lint -e830*/
    ulReturnCode = LPs_NvimItem_Read(EN_NV_ID_FLOWCTRL_CONFIG,\
                                    (VOS_VOID *)(&g_stFcLteConfig),\
                                    sizeof(FLOWCTRL_LTE_CONFIG_STRU));
    /*lint +e830*/
    /*lint +e732*/
    /*lint +e746*/
    /*lint +e718*/
    if (PS_SUCC != ulReturnCode)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS, "Fc_LteInit,LPs_NvimItem_Read FLOWCTRL_LTE_CONFIG fail:", ulReturnCode);
        /* 初始化为默认值 */
        Fc_LteConfig2Default();
    }

    if(PS_SUCC != FC_LteConfigcheck())
    {
        /* 初始化为默认值 */
        Fc_LteConfig2Default();
    }

    g_stFcLteCpuCtrl.ulFirstDelay = g_stFcLteConfig.stFcCpuConfig.usFirstDelay;
    g_stFcLteCpuCtrl.ulUlDownRate = g_stFcLteConfig.stFcCpuConfig.usUlDownRate;
    g_stFcLteCpuCtrl.ulUlUpRate = g_stFcLteConfig.stFcCpuConfig.usUlUpRate;
    g_stFcLteCpuCtrl.ulHqDownRate = g_stFcLteConfig.stFcCpuConfig.usHqDownRate;
    g_stFcLteCpuCtrl.ulHqUpRate = g_stFcLteConfig.stFcCpuConfig.usHqUpRate;
    g_stFcLteCpuCtrl.ulHqMaxDiscardRate = g_stFcLteConfig.stFcCpuConfig.usHqMaxDiscardRate;
    g_stFcLteCpuCtrl.ulDowngradeThres = g_stFcLteConfig.stFcCpuConfig.usDowngradeThres;
    g_stFcLteCpuCtrl.ulUpgradeThres = g_stFcLteConfig.stFcCpuConfig.usUpgradeThres;

    g_stFcLteTemperatureCtrl.ucInitialPos = g_stFcLteConfig.stFcTemperatureConfig.ucInitialPos;
    g_stFcLteTemperatureCtrl.ulBsrTimerLen = 10;    /* 默认BSR周期10ms */
    g_stFcLteTemperatureCtrl.ucAvailCnt = 0;
    g_stFcLteTemperatureCtrl.ucBsrThresCnt = 0;
    g_stFcLteTemperatureCtrl.ucCurrPos = FC_UL_THROUGHPUT_THRES_CNT;

    for(i = 0; i < FC_UL_THROUGHPUT_THRES_CNT_NV; i++)
    {
        if(0 == g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[i])
        {
            break;
        }

        if(g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[i] < g_stFcLteConfig.stFcTemperatureConfig.ulMinBsrThr)
        {
            g_stFcLteTemperatureCtrl.ucAvailCnt++;
            /* 周期折算，NV设置为kbps，入口流控基于100ms字节数 */
            g_stFcLteTemperatureCtrl.ulTemperSteps[i] = g_stFcLteConfig.stFcTemperatureConfig.ulTemperSteps[i]*100 / 8;
        }
        else
        {
            g_stFcLteTemperatureCtrl.ucBsrThresCnt++;
        }
    }
    /* 入口流控紧随BSR流控 */
    g_stFcLteTemperatureCtrl.ucAvailCnt += g_stFcLteTemperatureCtrl.ucBsrThresCnt;

    for(; i < FC_UL_THROUGHPUT_THRES_CNT; i++)
    {
        g_stFcLteTemperatureCtrl.ulTemperSteps[i] = 0;
    }

    if(g_stFcLteConfig.stFcTemperatureConfig.ucZeroSupport > 0)
    {
        g_stFcLteTemperatureCtrl.ucAvailCnt++;
    }

    g_ulUlDowngradeFlag = PS_FALSE;

    Fc_KbpsToBsr();
#endif


    /* 从GU NV读取流控配置信息 */
    /*lint -e718*/
    /*lint -e746*/
    /*lint -e732*/
    ulReturnCode = NV_ReadEx(MODEM_ID_0,EN_NV_ITEM_CDS_FC_CONFIG,\
                                    (&g_stFcCdsConfig),\
                                    sizeof(FLOWCTRL_CDS_CONFIG_STRU));
    /*lint +e732*/
    /*lint +e746*/
    /*lint +e718*/

    if (PS_SUCC != ulReturnCode)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS, "Fc_LteInit,NV_Read FLOWCTRL_CDS_CONFIG fail:", ulReturnCode);
        /* 初始化为默认值 */
        Fc_CdsConfig2Default();
    }

    if(PS_SUCC != FC_CdsConfigcheck())
    {
        /* 初始化为默认值 */
        Fc_CdsConfig2Default();
    }

    /* 基于NV填充控制结构 */
    if(0 == (FC_CDS_DL_ENABLED_MASK & g_stFcCdsConfig.ulFcEnableMask))
    {
        g_stFcCdsDlCtrl.bDlCdsFcEnable = VOS_FALSE;
    }
    else
    {
        g_stFcCdsDlCtrl.bDlCdsFcEnable = VOS_TRUE;
    }

    g_stFcCdsDlCtrl.ulDiscardThres = g_stFcCdsConfig.stFcCdsDlConfig.ulDiscardThres;
    g_stFcCdsDlCtrl.ulDiscardRate = g_stFcCdsConfig.stFcCdsDlConfig.ulDiscardRate;

    return;
}


unsigned int NULL_CPUFcProcess( unsigned int arg )
{
    if(arg > g_stFcLteCpuCtrl.ulDowngradeThres)
    {
        CDS_WARNING_LOG1(UEPS_PID_CDS,"NULL_CPUFcProcess : NULL CPU overload:", arg);
    }

    return 0;
}


SPY_DATA_DOWNGRADE_RESULT_ENUM_UINT32 NULL_DownGradeUlProcess (VOS_VOID)
{
    CDS_WARNING_LOG(UEPS_PID_CDS,"NULL_DownGradeUlProcess : NULL Speed Downgrade");
    /* 开机高温场景 */
    return SPY_DATA_DOWNGRADE_CONTINUE;
}
SPY_DATA_UPGRADE_RESULT_ENUM_UINT32 NULL_UpGradeUlProcess (VOS_VOID)
{
    CDS_WARNING_LOG(UEPS_PID_CDS,"NULL_UpGradeUlProcess : NULL Speed Upgrade");

    return SPY_DATA_UPGRADE_FINISH;
}


VOS_VOID NULL_RecoverUlGradeProcess (VOS_VOID)
{
    CDS_WARNING_LOG(UEPS_PID_CDS, "NULL_RecoverUlGradeProcess : NULL Speed Recover");

    return;
}


VOS_VOID FC_HarqDown (VOS_VOID)
{
    if(g_stFcLteCpuCtrl.ulHqMaxDiscardRate > (100 - g_ulHarqKeepRate))
    {
        FC_CL_HQ_DOWN_STAT(1);
        g_ulHarqKeepRate = g_ulHarqKeepRate * (100 - g_stFcLteCpuCtrl.ulHqDownRate) / 100;
        if(g_stFcLteCpuCtrl.ulHqMaxDiscardRate < (100 - g_ulHarqKeepRate))
        {
            FC_CL_HQ_TO_DOWN_STAT(1);
            g_ulHarqKeepRate = 100 - g_stFcLteCpuCtrl.ulHqMaxDiscardRate;
            CDS_WARNING_LOG(UEPS_PID_CDS,"FC_HarqDown : HARQ DOWNGRADE finish");
        }
        CDS_WARNING_LOG1(UEPS_PID_CDS,"FC_HarqDown : HARQ DiscardRate:", (100 - g_ulHarqKeepRate));

        /* 写给DSP */
    }

    return;
}
VOS_VOID FC_HarqUp (VOS_VOID)
{
    FC_CL_HQ_UP_STAT(1);

    g_ulHarqKeepRate = g_ulHarqKeepRate * (100 + g_stFcLteCpuCtrl.ulHqUpRate) / 100;
    CDS_WARNING_LOG1(UEPS_PID_CDS,"FC_HarqUp : HARQ DiscardRate:", (100 - g_ulHarqKeepRate));

    if(g_ulHarqKeepRate > 100)
    {
        g_ulHarqKeepRate = 100;
        CDS_WARNING_LOG(UEPS_PID_CDS,"FC_HarqUp : HARQ UPGRADE finish");
        FC_CL_HQ_TO_NORMAL_STAT(1);
    }

    /* 写给DSP */

    return;
}
unsigned int FC_LCPU_Process( unsigned int arg )
{
    static VOS_UINT32 ulFirstDelay = 0;

    if(arg > g_stFcLteCpuCtrl.ulDowngradeThres)
    {
        /* 0.9999浮点逼近 1 */
        if(0.9999 <  g_dBsrCpuModu)
        {
            /* 延迟进入流控状态 */
            ulFirstDelay++;
            if(g_stFcLteCpuCtrl.ulFirstDelay <= ulFirstDelay)
            {
                FC_CL_CPU_DOWN_STAT(3);
                FC_CL_CPU_TO_DOWN_STAT(1);
                /*  首次快降 */
                g_dBsrCpuModu = (100.0 - g_stFcLteCpuCtrl.ulUlDownRate) * g_dBsrCpuModu / 100.0;
                g_dBsrCpuModu = (100.0 - g_stFcLteCpuCtrl.ulUlDownRate) * g_dBsrCpuModu / 100.0;
                g_dBsrCpuModu = (100.0 - g_stFcLteCpuCtrl.ulUlDownRate) * g_dBsrCpuModu / 100.0;
                ulFirstDelay = 0;
            }
            CDS_WARNING_LOG2(UEPS_PID_CDS, "FC_LCPU_Process : Modulus: CPU: ", (VOS_UINT32)(g_dBsrCpuModu * 10000.0), arg);
        }
        else
        {
            if(g_dBsrCpuLimit < g_dBsrCpuModu)
            {
                FC_CL_CPU_DOWN_STAT(1);
                g_dBsrCpuModu = (100.0 - g_stFcLteCpuCtrl.ulUlDownRate) * g_dBsrCpuModu / 100.0;
                CDS_WARNING_LOG2(UEPS_PID_CDS, "FC_LCPU_Process : Modulus: CPU: ", (VOS_UINT32)(g_dBsrCpuModu * 10000.0), arg);
            }
            else if(g_stFcLteCpuCtrl.ulHqMaxDiscardRate > (100 - g_ulHarqKeepRate))
            {
                /* HARQ降速 */
                FC_HarqDown();
            }
            /* modify by jiqiang 2014.03.19 pclint begin */
            else
            {
            }
            /* modify by jiqiang 2014.03.19 pclint end */
        }
    }
    else if(arg < g_stFcLteCpuCtrl.ulUpgradeThres)
    {
        ulFirstDelay = 0;
        if(g_ulHarqKeepRate < 100)
        {
            /* HARQ升速 */
            FC_HarqUp();
        }
        else if(0.9999 >  g_dBsrCpuModu)
        {
            FC_CL_CPU_UP_STAT(1);
            g_dBsrCpuModu = (100.0 + g_stFcLteCpuCtrl.ulUlUpRate) * g_dBsrCpuModu / 100.0;

            if(1 <  g_dBsrCpuModu)
            {
                g_dBsrCpuModu = 1.0;
                FC_CL_CPU_TO_NORMAL_STAT(1);
            }
            CDS_WARNING_LOG1(UEPS_PID_CDS,"FC_LCPU_Process : Modulus: ", (VOS_UINT32)(g_dBsrCpuModu * 10000.0));
        }
        /* modify by jiqiang 2014.03.19 pclint begin */
        else
        {
            ;
        }
        /* modify by jiqiang 2014.03.19 pclint end */
     }
    else
    {
        ulFirstDelay = 0;
        /* 不处理 */
    }

    return 0;
}


SPY_DATA_DOWNGRADE_RESULT_ENUM_UINT32 FC_LDownUlGrade_Process (VOS_VOID)
{
    if(g_stFcLteTemperatureCtrl.ucCurrPos >=  FC_UL_THROUGHPUT_THRES_CNT)
    {
        /*  首次快降 */
        g_stFcLteTemperatureCtrl.ucCurrPos = g_stFcLteTemperatureCtrl.ucInitialPos;

        FC_CL_TEMPERATURE_DOWN_STAT(g_stFcLteTemperatureCtrl.ucInitialPos+1);
    }
    else
    {
        g_stFcLteTemperatureCtrl.ucCurrPos++;

        FC_CL_TEMPERATURE_DOWN_STAT(1);
    }

    if(g_stFcLteTemperatureCtrl.ucCurrPos >= g_stFcLteTemperatureCtrl.ucBsrThresCnt)
    {
        /* 是否启动了入口流控 */
        if(g_stFcLteTemperatureCtrl.ucAvailCnt > g_stFcLteTemperatureCtrl.ucBsrThresCnt)
        {
            g_ulUlDowngradeFlag = PS_TRUE;
        }
        if(g_stFcLteTemperatureCtrl.ucCurrPos >= g_stFcLteTemperatureCtrl.ucAvailCnt)
        {
            /* 校正到最后一个有效位置 */
            g_stFcLteTemperatureCtrl.ucCurrPos = g_stFcLteTemperatureCtrl.ucAvailCnt - 1;
        }
        if(PS_TRUE == g_ulUlDowngradeFlag)
        {
            CDS_WARNING_LOG2(UEPS_PID_CDS,"FC_LDownUlGrade_Process: Downgrade Entry : ->:", g_ulUlThroughputMax, g_stFcLteTemperatureCtrl.ulTemperSteps[g_stFcLteTemperatureCtrl.ucCurrPos]);
            g_ulUlThroughputMax = g_stFcLteTemperatureCtrl.ulTemperSteps[g_stFcLteTemperatureCtrl.ucCurrPos];
        }
    }

    if(g_stFcLteTemperatureCtrl.ucCurrPos >= g_stFcLteTemperatureCtrl.ucAvailCnt -1)
    {
        FC_CL_TEMPERATURE_TO_LIMIT_STAT(1);

        CDS_WARNING_LOG1(UEPS_PID_CDS,"FC_LDownUlGrade_Process: Finish : ThresPos:", g_stFcLteTemperatureCtrl.ucCurrPos);
        return SPY_DATA_DOWNGRADE_FINISH;
    }

    CDS_WARNING_LOG1(UEPS_PID_CDS, "FC_LDownUlGrade_Process : ThresPos: ", g_stFcLteTemperatureCtrl.ucCurrPos);

    return SPY_DATA_DOWNGRADE_CONTINUE;
}
SPY_DATA_UPGRADE_RESULT_ENUM_UINT32 FC_LUpUlGrade_Process (VOS_VOID)
{
    FC_CL_TEMPERATURE_UP_STAT(1);

    if(PS_TRUE == g_ulUlDowngradeFlag)
    {
        if(g_stFcLteTemperatureCtrl.ucBsrThresCnt < g_stFcLteTemperatureCtrl.ucCurrPos)
        {
            CDS_WARNING_LOG2(UEPS_PID_CDS,"FC_LUpUlGrade_Process: Upgrade Entry : ->:", g_ulUlThroughputMax, g_stFcLteTemperatureCtrl.ulTemperSteps[g_stFcLteTemperatureCtrl.ucCurrPos-1]);
            g_ulUlThroughputMax = g_stFcLteTemperatureCtrl.ulTemperSteps[g_stFcLteTemperatureCtrl.ucCurrPos-1];
        }
        else
        {   /* 退出入口流控 */
            g_ulUlDowngradeFlag = PS_FALSE;
            g_ulUlThroughputMax = FC_UL_ENTRY_THROUGHPUT_INIT;
            /*g_stFcLteTemperatureCtrl.ucCurrPos = g_stFcLteTemperatureCtrl.ucBsrThresCnt-1;*/
        }
    }

    if(PS_FALSE == g_ulUlDowngradeFlag)
    {
        if((g_stFcLteTemperatureCtrl.ucCurrPos == 0)
            || (g_stFcLteTemperatureCtrl.ucCurrPos >= FC_UL_THROUGHPUT_THRES_CNT))
        {
            g_stFcLteTemperatureCtrl.ucCurrPos = FC_UL_THROUGHPUT_THRES_CNT;
            CDS_WARNING_LOG(UEPS_PID_CDS, "FC_LUpUlGrade_Process: Finish !");

            FC_CL_TEMPERATURE_TO_NORMAL_STAT(1);

            return SPY_DATA_UPGRADE_FINISH;
        }
    }

    g_stFcLteTemperatureCtrl.ucCurrPos--;

    CDS_WARNING_LOG1(UEPS_PID_CDS, "FC_LUpUlGrade_Process : ThresPos: ", g_stFcLteTemperatureCtrl.ucCurrPos);

    return SPY_DATA_UPGRADE_CONTINUE;
}


VOS_VOID FC_LRecoverUlGrade_Process (VOS_VOID)
{
    FC_CL_TEMPERATURE_RECOVER_STAT(1);

    g_ulUlDowngradeFlag = PS_FALSE;
    g_ulUlThroughputMax = FC_UL_ENTRY_THROUGHPUT_INIT;
    g_stFcLteTemperatureCtrl.ucCurrPos = FC_UL_THROUGHPUT_THRES_CNT;

    CDS_WARNING_LOG1(UEPS_PID_CDS, "FC_LRecoverUlGrade_Process : ThresPos: ", g_stFcLteTemperatureCtrl.ucCurrPos);
    return;
}

#if (CDS_FEATURE_ON == CDS_FEATURE_LTE)

VOS_UINT32  FC_UlDowngradeInput(VOS_VOID)
{
    static VOS_UINT32   ulIPStatTick = 0;
    VOS_UINT32          ulTTI;

    if(PS_FALSE == g_ulUlDowngradeFlag)
    {
        return PS_FALSE;
    }

    ulTTI = LUP_GetCurrentTime()/FC_UL_DOWNGRADE_GAP;

    if((ulTTI != ulIPStatTick))
    {
        ulIPStatTick = ulTTI;
        g_ulUlFcThroughputStat = 1;
    }

    if(g_ulUlFcThroughputStat > g_ulUlThroughputMax)
    {
    	return PS_TRUE;
    }

    return PS_FALSE;
}
#endif


VOS_VOID Fc_SwitchToGu(VOS_VOID)
{
    FC_CL_TO_GU_STAT(1);
#if (CDS_FEATURE_ON == CDS_FEATURE_LTE)
    /*释放上行SDU*/
    LUP_RelDataReq();
#endif
    /* 新进入GU，恢复温保流控状态 */
    FC_RecoverUlGradeProcess();

    BSP_PWRCTRL_GetCcpuLoadCB(FC_CPU_Process);
    Spy_DownGradeRegister(FC_DownUlGradeProcess);
    Spy_UpGradeRegister(FC_UpUlGradeProcess);
    Spy_RestoreRegister(FC_RecoverUlGradeProcess);
    return;
}


VOS_VOID Fc_SwitchToLte(VOS_VOID)
{
    /* 新进入LTE，恢复流控状态 */
    Fc_LteStatusInit();

    FC_CL_TO_LTE_STAT(1);

    BSP_PWRCTRL_GetCcpuLoadCB(FC_LCPU_Process);
    Spy_DownGradeRegister(FC_LDownUlGrade_Process);
    Spy_UpGradeRegister(FC_LUpUlGrade_Process);
    Spy_RestoreRegister(FC_LRecoverUlGrade_Process);
    return;
}
VOS_VOID Fc_SwitchToNull(VOS_VOID)
{
    FC_CL_TO_NULL_STAT(1);

#if (CDS_FEATURE_ON == CDS_FEATURE_LTE)
    /*释放上行SDU*/
    LUP_RelDataReq();
#endif
    Fc_LteStatusInit();

    BSP_PWRCTRL_GetCcpuLoadCB(NULL_CPUFcProcess);
    Spy_DownGradeRegister(NULL_DownGradeUlProcess);
    Spy_UpGradeRegister(NULL_UpGradeUlProcess);
    Spy_RestoreRegister(NULL_RecoverUlGradeProcess);
    return;
}
VOS_UINT32 g_BsrCheck = 0;
VOS_UINT32 Fc_BufferReportDiscount( VOS_UINT32 ulBufferSize )
{
    VOS_UINT8 ulTmpCurrPos;
 
    if(PS_FALSE == g_ulUlDowngradeFlag)
    {
        ulTmpCurrPos = g_stFcLteTemperatureCtrl.ucCurrPos;        
    }
    else
    {
        if (g_stFcLteTemperatureCtrl.ucBsrThresCnt > 0)
        {
            ulTmpCurrPos = g_stFcLteTemperatureCtrl.ucBsrThresCnt - 1;
        }
        else
        {
            ulTmpCurrPos = FC_UL_THROUGHPUT_THRES_CNT;
        }
    }

    if(ulTmpCurrPos < FC_UL_THROUGHPUT_THRES_CNT)
    {
        if(ulBufferSize > g_stFcLteTemperatureCtrl.ulTemperSteps[ulTmpCurrPos])
        {
            ulBufferSize = g_stFcLteTemperatureCtrl.ulTemperSteps[ulTmpCurrPos];
        }
    }

    /* CPU或温控减速 */
    /* 0.9999浮点逼近 1 */
    if((g_dBsrCpuModu < 0.9999) && (g_stFcLteCpuCtrl.ulUlMinBsr < ulBufferSize))
    {
        ulBufferSize = (VOS_UINT32)(ulBufferSize * g_dBsrCpuModu);
        if(g_stFcLteCpuCtrl.ulUlMinBsr > ulBufferSize)
        {
            FC_CL_BSR_BELOW_MIN_STAT(1);
            ulBufferSize = g_stFcLteCpuCtrl.ulUlMinBsr;
        }
    }
    if(g_BsrCheck)
    {
        if(977 == PS_RAND(3000))
        {
            vos_printf("BSR value:  %d\r\n", ulBufferSize);
        }
    }

    return ulBufferSize;
}


VOS_BOOL Fc_CdsDlPktDiscard(VOS_UINT8 ucRabId)
{
    VOS_UINT32 ulRandValue;

    /* 规避PC-LINT问题，后面扩展下行流控时将用到 */
    (VOS_VOID)ucRabId;
    /*lint -e960*/
    if((g_stFcCdsDlCtrl.bDlCdsFcEnable)
        && (g_stFcCdsDlCtrl.ulDiscardThres > TTF_GetDlIpFreeMemCnt()))
    /*lint +e960*/
    {
        /* 丢包 */
        ulRandValue = PS_RAND(101);
        if(ulRandValue > g_stFcCdsDlCtrl.ulDiscardRate)
        {
            return VOS_FALSE;
        }

        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_VOID Fc_ShowLtePara(VOS_VOID)
{
    vos_printf("LTE流控参数 :\r\n");
    vos_printf("\r\n");
    vos_printf("下行CDS流控是否使能   %s\r\n", g_stFcCdsDlCtrl.bDlCdsFcEnable?"Yes":"No");
    vos_printf("下行CDS丢包门限       %d\r\n", g_stFcCdsDlCtrl.ulDiscardThres);
    vos_printf("下行CDS丢包率         %d\r\n", g_stFcCdsDlCtrl.ulDiscardRate);
    vos_printf("\r\n");

    vos_printf("CPU过载流控延迟进入   %d\r\n", g_stFcLteCpuCtrl.ulFirstDelay);
    vos_printf("CPU升速步长百分比     %d\r\n", g_stFcLteCpuCtrl.ulUlUpRate);
    vos_printf("CPU降速步长百分比     %d\r\n", g_stFcLteCpuCtrl.ulUlDownRate);
    vos_printf("CPU降速下限 BSR       %d\r\n", g_stFcLteCpuCtrl.ulUlMinBsr);
    vos_printf("HARQ升速步长百分比    %d\r\n", g_stFcLteCpuCtrl.ulHqUpRate);
    vos_printf("HARQ降速步长百分比    %d\r\n", g_stFcLteCpuCtrl.ulHqDownRate);
    vos_printf("HARQ丢包降速上限      %d\r\n", g_stFcLteCpuCtrl.ulHqMaxDiscardRate);
    vos_printf("CPU降速阈值           %d\r\n", g_stFcLteCpuCtrl.ulDowngradeThres);
    vos_printf("CPU升速阈值           %d\r\n", g_stFcLteCpuCtrl.ulUpgradeThres);
    vos_printf("\r\n");

    vos_printf("降速起始位置          %d\r\n", g_stFcLteTemperatureCtrl.ucInitialPos);
    vos_printf("温保总流控阶数          %d\r\n", g_stFcLteTemperatureCtrl.ucAvailCnt);
    vos_printf("BSR降速阶数           %d\r\n", g_stFcLteTemperatureCtrl.ucBsrThresCnt);
    vos_printf("温保入口控制参数      %d %d %d %d\r\n", g_stFcLteTemperatureCtrl.ulTemperSteps[0],
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[1],
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[2],
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[3]);
    vos_printf("                      %d %d %d %d\r\n",
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[4],
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[5],
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[6],
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[7]);
    vos_printf("                      %d %d %d %d\r\n",
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[8],
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[9],
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[10],
                                                          g_stFcLteTemperatureCtrl.ulTemperSteps[11]);
    vos_printf("                      %d\r\n", g_stFcLteTemperatureCtrl.ulTemperSteps[12]);

    vos_printf("BSR周期               %d\r\n", g_stFcLteTemperatureCtrl.ulBsrTimerLen);

    vos_printf("\r\n");
    return;
}


VOS_VOID Fc_ShowLteStats(VOS_VOID)
{
    vos_printf("C核LTE流控统计 :\r\n");
    vos_printf("\r\n");
    vos_printf("切为LTE模次数               %d\r\n", g_stFcCLStats.ulToLteNum);
    vos_printf("切为GU模次数                %d\r\n", g_stFcCLStats.ulToGUNum);
    vos_printf("进入NULL模式次数            %d\r\n", g_stFcCLStats.ulToNullNum);
    vos_printf("进入LOOPBACK次数            %d\r\n", g_stFcCLStats.ulToLBNum);
    vos_printf("LTE温控降速次数             %d\r\n", g_stFcCLStats.ulTemperatureDownNum);
    vos_printf("LTE温控升速次数             %d\r\n", g_stFcCLStats.ulTemperatureUpNum);
    vos_printf("LTE温控直接恢复次数         %d\r\n", g_stFcCLStats.ulTemperatureRecoverNum);
    vos_printf("LTE温控降速到下限次数       %d\r\n", g_stFcCLStats.ulTemperatureToLimitNum);
    vos_printf("LTE温控恢复到正常次数       %d\r\n", g_stFcCLStats.ulTemperatureToNormalNum);
    vos_printf("LTE CPU过载降速次数         %d\r\n", g_stFcCLStats.ulCpuDownNum);
    vos_printf("LTE CPU恢复升速次数         %d\r\n", g_stFcCLStats.ulCpuUpNum);
    vos_printf("LTE CPU进入降速状态数       %d\r\n", g_stFcCLStats.ulCpuToDownNum);
    vos_printf("LTE CPU恢复到正常次数       %d\r\n", g_stFcCLStats.ulCpuToNormalNum);
    vos_printf("LTE HARQ过载降速次数        %d\r\n", g_stFcCLStats.ulHqDownNum);
    vos_printf("LTE HARQ恢复升速次数        %d\r\n", g_stFcCLStats.ulHqUpNum);
    vos_printf("LTE HARQ降到底次数          %d\r\n", g_stFcCLStats.ulHqToDownNum);
    vos_printf("LTE HARQ恢复到正常次数      %d\r\n", g_stFcCLStats.ulHqToNormalNum);
    vos_printf("LTE上报缓存低于下限         %d\r\n", g_stFcCLStats.ulBelowMinBsr);

    vos_printf("\r\n");

    return;
}


VOS_VOID Fc_ShowLteResult(VOS_VOID)
{
    vos_printf("C核LTE流控结果 :\r\n");
    vos_printf("\r\n");
    vos_printf("CPU乘数  (*10000)      %05d\r\n", (VOS_UINT32)(g_dBsrCpuModu * 10000));
    vos_printf("HARQ丢包率             %d\r\n", (100 - g_ulHarqKeepRate));
    vos_printf("温保流控当前位置       %d\r\n", g_stFcLteTemperatureCtrl.ucCurrPos);
    vos_printf("温保入口控制状态       %d\r\n", g_ulUlDowngradeFlag);

    vos_printf("\r\n");

    return;
}


VOS_VOID Fc_ResetLteStats(VOS_VOID)
{
    PS_MEM_SET(&g_stFcCLStats, 0, sizeof(FC_CL_STATS_INFO_STRU));
    return;

}


VOS_VOID Fc_LteHelp(VOS_VOID)
{
    vos_printf("LTE流控软调命令 :\r\n");
    vos_printf("\r\n");
    vos_printf("Fc_ShowLtePara       :显示LTE流控参数\r\n");
    vos_printf("Fc_ShowLteStats      :显示LTE流控统计量\r\n");
    vos_printf("Fc_ResetLteStats     :复位LTE流控统计量\r\n");
    vos_printf("Fc_ShowLteResult     :显示C核LTE流控结果\r\n");

    vos_printf("QosFc_ShowPara       :显示QoS流控参数\r\n");
    vos_printf("QosFc_ShowStats      :显示QoS流控统计量\r\n");
    vos_printf("QosFc_ResetStats     :复位QoS流控统计量\r\n");
    vos_printf("QosFc_ShowState      :显示QoS流控状态\r\n");
    vos_printf("QosFc_ShowRabState   :显示QoS流控承载状态\r\n");
    vos_printf("QosFc_ShowEntity     :显示Qos流控实体状态\r\n");
    vos_printf("QosFc_ShowChannel    :显示Qos流控通道状态\r\n");

    vos_printf("\r\n");
    return;

}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



