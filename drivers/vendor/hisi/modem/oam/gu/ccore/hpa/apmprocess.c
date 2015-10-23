/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: APMProcess.c                                                    */
/*                                                                           */
/*    Author: Jiang KaiBo                                                    */
/*            Xu Cheng                                                       */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-02                                                             */
/*                                                                           */
/* Description: process data of APM mailbox                                  */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "hpacomm.h"
#include "omprivate.h"
#include "rfa.h"
#include "apminterface.h"
#include "apmprocess.h"
#include "WatchDog.h"
#include "PhyNvInterface.h"
#include "gbbp_interface.h"
#include "phyoaminterface.h"
#include "bbp_comm_interface.h"
#include "FileSysInterface.h"

#ifdef __LDF_FUNCTION__
#include "HifiDumpInterface.h"
#include "WasOmInterface.h"
#endif

#include "sleepflow.h"

#if (RAT_MODE != RAT_GU)
#include "product_config.h"
#include "psregrpt.h"
#endif

/* Macro of log */
#define    THIS_FILE_ID        PS_FILE_ID_APM_PROCESS_C


/*lint -e614 */
/*lint -e40 */

/*lint -e40 */
/*lint -e614 */

#define     APM_WAITING_LOOP_TIMES      (50)

/* 保存PHY的激活状态 */
VOS_UINT32                      g_aulShpaActiveState[MODEM_ID_BUTT][VOS_RATMODE_LTE];

/* Record current RAT mdoe. 2G or 3G */
VOS_UINT32                      g_aulShpaCurSysId[MODEM_ID_BUTT];

/* A semaphor. Get it when succeed to load DSP. */
VOS_UINT32                      g_aulShpaLoadPhySemaphor[MODEM_ID_BUTT];

/* A semaphor. Get it when succeed to Active DSP. */
VOS_UINT32                      g_aulShpaActivePhySemaphor[MODEM_ID_BUTT];

/* A semaphor. Get it when save data to file. */
VOS_UINT32                      g_ulLdfSavingSemaphor;

/* DSP Config info Data */
DSP_CONFIG_CTRL_STRU            g_astDspConfigCtrl[MODEM_ID_BUTT];

/* Read from Nv ID en_NV_Item_UMTS_BAND */
VOS_UINT32                      g_ulUeWcdmaBands = 0;

/*系统使用的地址*/
PLATFORM_SYSADDR_STRU           g_stSysAddr;

/*lint -e40 */
#if (FEATURE_ON == FEATURE_VIRTUAL_BAND)
#define APM_INIT_NV_CODE() \
APM_RF_NV_INFO_STRU astPhyOamNvInfo[] =  {\
    {g_ausCommonNvIdReadList,   sizeof(g_ausCommonNvIdReadList)/sizeof(VOS_UINT16),}, /* Common */\
    {g_ausGsmComNvIdReadList,   sizeof(g_ausGsmComNvIdReadList)/sizeof(VOS_UINT16),}, /* Gsm common */\
    {g_ausGsmNvIdReadBand850,   sizeof(g_ausGsmNvIdReadBand850)/sizeof(VOS_UINT16),}, /* Gsm Band 850 */\
    {g_ausGsmNvIdReadBand900,   sizeof(g_ausGsmNvIdReadBand900)/sizeof(VOS_UINT16),}, /* Gsm Band 900 */\
    {g_ausGsmNvIdReadBand1800,  sizeof(g_ausGsmNvIdReadBand1800)/sizeof(VOS_UINT16),}, /* Gsm Band 1800 */\
    {g_ausGsmNvIdReadBand1900,  sizeof(g_ausGsmNvIdReadBand1900)/sizeof(VOS_UINT16),}, /* Gsm Band 1900 */\
    {g_ausGsmNvIdReadBandVirtual,sizeof(g_ausGsmNvIdReadBandVirtual)/sizeof(VOS_UINT16),}, /* Gsm jazz new band */\
    {g_ausWcdmaComNvIdReadList, sizeof(g_ausWcdmaComNvIdReadList)/sizeof(VOS_UINT16),}, /* Wcdma common */\
    {g_ausWcdmaNvIdReadBand1,   sizeof(g_ausWcdmaNvIdReadBand1)/sizeof(VOS_UINT16),}, /* Band 1 */\
    {g_ausWcdmaNvIdReadBand2,   sizeof(g_ausWcdmaNvIdReadBand2)/sizeof(VOS_UINT16),}, /* Band 2 */\
    {g_ausWcdmaNvIdReadBand3,   sizeof(g_ausWcdmaNvIdReadBand3)/sizeof(VOS_UINT16),}, /* Band 3 */\
    {g_ausWcdmaNvIdReadBand4,   sizeof(g_ausWcdmaNvIdReadBand4)/sizeof(VOS_UINT16),}, /* Band 4 */\
    {g_ausWcdmaNvIdReadBand5,   sizeof(g_ausWcdmaNvIdReadBand5)/sizeof(VOS_UINT16),}, /* Band 5 */\
    {g_ausWcdmaNvIdReadBand6,   sizeof(g_ausWcdmaNvIdReadBand6)/sizeof(VOS_UINT16),}, /* Band 6 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 7 */\
    {g_ausWcdmaNvIdReadBand8,   sizeof(g_ausWcdmaNvIdReadBand8)/sizeof(VOS_UINT16),}, /* Band 8 */\
    {g_ausWcdmaNvIdReadBand9,   sizeof(g_ausWcdmaNvIdReadBand9)/sizeof(VOS_UINT16),}, /* Band 9 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 10 */\
    {g_ausWcdmaNvIdReadBand11,   sizeof(g_ausWcdmaNvIdReadBand11)/sizeof(VOS_UINT16),}, /* Band 11 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 12 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 13 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 14 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 15 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 16 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 17 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 18 */\
    {g_ausWcdmaNvIdReadBand19,  sizeof(g_ausWcdmaNvIdReadBand19)/sizeof(VOS_UINT16),}, /* Band 19 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 20 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 21 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 22 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 23 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 24 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 25 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 26 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 27 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 28 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 29 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 30 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 31 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 32 */\
};
#else
#define APM_INIT_NV_CODE() \
APM_RF_NV_INFO_STRU astPhyOamNvInfo[] =  {\
    {g_ausCommonNvIdReadList,   sizeof(g_ausCommonNvIdReadList)/sizeof(VOS_UINT16),}, /* Common */\
    {g_ausGsmComNvIdReadList,   sizeof(g_ausGsmComNvIdReadList)/sizeof(VOS_UINT16),}, /* Gsm common */\
    {g_ausGsmNvIdReadBand850,   sizeof(g_ausGsmNvIdReadBand850)/sizeof(VOS_UINT16),}, /* Gsm Band 850 */\
    {g_ausGsmNvIdReadBand900,   sizeof(g_ausGsmNvIdReadBand900)/sizeof(VOS_UINT16),}, /* Gsm Band 900 */\
    {g_ausGsmNvIdReadBand1800,  sizeof(g_ausGsmNvIdReadBand1800)/sizeof(VOS_UINT16),}, /* Gsm Band 1800 */\
    {g_ausGsmNvIdReadBand1900,  sizeof(g_ausGsmNvIdReadBand1900)/sizeof(VOS_UINT16),}, /* Gsm Band 1900 */\
    {g_ausWcdmaComNvIdReadList, sizeof(g_ausWcdmaComNvIdReadList)/sizeof(VOS_UINT16),}, /* Wcdma common */\
    {g_ausWcdmaNvIdReadBand1,   sizeof(g_ausWcdmaNvIdReadBand1)/sizeof(VOS_UINT16),}, /* Band 1 */\
    {g_ausWcdmaNvIdReadBand2,   sizeof(g_ausWcdmaNvIdReadBand2)/sizeof(VOS_UINT16),}, /* Band 2 */\
    {g_ausWcdmaNvIdReadBand3,   sizeof(g_ausWcdmaNvIdReadBand3)/sizeof(VOS_UINT16),}, /* Band 3 */\
    {g_ausWcdmaNvIdReadBand4,   sizeof(g_ausWcdmaNvIdReadBand4)/sizeof(VOS_UINT16),}, /* Band 4 */\
    {g_ausWcdmaNvIdReadBand5,   sizeof(g_ausWcdmaNvIdReadBand5)/sizeof(VOS_UINT16),}, /* Band 5 */\
    {g_ausWcdmaNvIdReadBand6,   sizeof(g_ausWcdmaNvIdReadBand6)/sizeof(VOS_UINT16),}, /* Band 6 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 7 */\
    {g_ausWcdmaNvIdReadBand8,   sizeof(g_ausWcdmaNvIdReadBand8)/sizeof(VOS_UINT16),}, /* Band 8 */\
    {g_ausWcdmaNvIdReadBand9,   sizeof(g_ausWcdmaNvIdReadBand9)/sizeof(VOS_UINT16),}, /* Band 9 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 10 */\
    {g_ausWcdmaNvIdReadBand11,   sizeof(g_ausWcdmaNvIdReadBand11)/sizeof(VOS_UINT16),}, /* Band 11 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 12 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 13 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 14 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 15 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 16 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 17 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 18 */\
    {g_ausWcdmaNvIdReadBand19,  sizeof(g_ausWcdmaNvIdReadBand19)/sizeof(VOS_UINT16),}, /* Band 19 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 20 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 21 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 22 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 23 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 24 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 25 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 26 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 27 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 28 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 29 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 30 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 31 */\
    {VOS_NULL_PTR,      0,                                           }, /* Band 32 */\
};
#endif
/*lint +e40 */

/*lint -e40 */

#if (FEATURE_ON == FEATURE_VIRTUAL_BAND)
#define APM_INITGDSP1_NV_CODE() \
APM_RF_NV_INFO_STRU astGPhy1OamNvInfo[] =  {\
    {g_ausCommonNvIdReadList,   sizeof(g_ausCommonNvIdReadList)/sizeof(VOS_UINT16),}, /* Common */\
    {g_ausGsmComNvIdReadList,   sizeof(g_ausGsmComNvIdReadList)/sizeof(VOS_UINT16),}, /* Gsm common */\
    {g_ausGsmNvIdReadBand850,   sizeof(g_ausGsmNvIdReadBand850)/sizeof(VOS_UINT16),}, /* Gsm Band 850 */\
    {g_ausGsmNvIdReadBand900,   sizeof(g_ausGsmNvIdReadBand900)/sizeof(VOS_UINT16),}, /* Gsm Band 900 */\
    {g_ausGsmNvIdReadBand1800,  sizeof(g_ausGsmNvIdReadBand1800)/sizeof(VOS_UINT16),}, /* Gsm Band 1800 */\
    {g_ausGsmNvIdReadBand1900,  sizeof(g_ausGsmNvIdReadBand1900)/sizeof(VOS_UINT16),}, /* Gsm Band 1900 */\
    {g_ausGsmNvIdReadBandVirtual,sizeof(g_ausGsmNvIdReadBandVirtual)/sizeof(VOS_UINT16),}, /* Gsm jazz new band */\
};
#else
#define APM_INITGDSP1_NV_CODE() \
APM_RF_NV_INFO_STRU astGPhy1OamNvInfo[] =  {\
    {g_ausCommonNvIdReadList,   sizeof(g_ausCommonNvIdReadList)/sizeof(VOS_UINT16),}, /* Common */\
    {g_ausGsmComNvIdReadList,   sizeof(g_ausGsmComNvIdReadList)/sizeof(VOS_UINT16),}, /* Gsm common */\
    {g_ausGsmNvIdReadBand850,   sizeof(g_ausGsmNvIdReadBand850)/sizeof(VOS_UINT16),}, /* Gsm Band 850 */\
    {g_ausGsmNvIdReadBand900,   sizeof(g_ausGsmNvIdReadBand900)/sizeof(VOS_UINT16),}, /* Gsm Band 900 */\
    {g_ausGsmNvIdReadBand1800,  sizeof(g_ausGsmNvIdReadBand1800)/sizeof(VOS_UINT16),}, /* Gsm Band 1800 */\
    {g_ausGsmNvIdReadBand1900,  sizeof(g_ausGsmNvIdReadBand1900)/sizeof(VOS_UINT16),}, /* Gsm Band 1900 */\
};
#endif

/*lint +e40 */

#ifdef  __LDF_FUNCTION__

#if 0

VOS_VOID SHPA_Ldf_Check(VOS_VOID)
{
    return;
}
#endif

/*****************************************************************************
 Function   : SHPA_Ldf_Hifi_Saving
 Description: Save Load HIFI Fail reason
 Input      :
 Return     : void
 Other      :
 *****************************************************************************/
VOS_UINT32 SHPA_Ldf_Hifi_Saving(VOS_VOID)
{
    return VOS_OK;
}
#endif

/*****************************************************************************
 Function   : SHPA_GetRateType
 Description: Get Current Modem RAT Type
 Input      : enModemID -- Modem ID
 Return     : Rat Type
 Other      : it is a API. PS can call it.
 *****************************************************************************/
VOS_UINT32 SHPA_GetRateType(MODEM_ID_ENUM_UINT16 enModemID)
{
    if (MODEM_ID_BUTT <= enModemID)
    {
        return VOS_RATMODE_BUTT;
    }

    return g_aulShpaCurSysId[enModemID];
}


VOS_UINT32 GHPA_GetRealFN(MODEM_ID_ENUM_UINT16 enModemID)
{
    VOS_UINT16  usFnLow  = 0;
    VOS_UINT16  usFnHigh = 0;
    VOS_UINT32  ulFnReal = 0;
    VOS_UINT32  ulRegValue = 0;

    if (MODEM_ID_BUTT <= enModemID)
    {
        return ulFnReal;
    }

    ulRegValue = HPA_Read32Reg(g_aulGBBPRegAddr[GTC_FN][enModemID]);

    /* 从帧号地址中读取帧号的低位和高位 */
    usFnLow  = ((VOS_UINT16)ulRegValue) & 0x07FF;           /*低11比特有效*/
    usFnHigh = ((VOS_UINT16)(ulRegValue >> 16)) & 0x07FF;   /*低11比特有效*/

    /* 根据公式计算帧号 */
    ulFnReal = usFnLow + (usFnHigh * 26 * 51);

    return ulFnReal;
}

/*****************************************************************************
 Function   : SHPA_ReadWTxPower
 Description: 由于Was不想读取寄存器，因此移到此处读取上行发射功率寄存器
 Input      : 无
 Return     : 寄存器值
 Other      : 戴明扬要求此函数在OM提供
 *****************************************************************************/
VOS_UINT32 SHPA_ReadWTxPower( VOS_VOID)
{
#ifndef FEATURE_DSP2ARM
    /*lint -e40 */
    return HPA_Read32Reg(W_BBP_SLOT_0_UL_TX_POWER_REG);
    /*lint +e40 */
#endif
    return 0;
}


/*****************************************************************************
 Function   :APM_GetNvSpareLen
 Description:Get DSP Nv Data Length
 Input      :None

 Return     :VOS_NULL_LONG/Total NV Data Lenght
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_GetNvSpareLen(VOS_UINT32                 ulWBandNum,
                                    VOS_UINT32                  ulWOneBandLen)
{
    VOS_UINT32  ulWBandSpareLen;

    if ( ulWBandNum > OAM_PHY_W_MAX_BAND_COUNT)
    {
        LogPrint(" APM_GetNvSpareLen:Get DSP NV Len Err.\r\n");
    }

    ulWBandSpareLen = ( OAM_PHY_W_MAX_BAND_COUNT - ulWBandNum ) * ulWOneBandLen;

    return ulWBandSpareLen;
}

/*****************************************************************************
 Function   :APM_GetNvTotalLength
 Description:Get DSP Nv Data Length
 Input      :None

 Return     :VOS_NULL_LONG/Total NV Data Lenght
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_GetNvTotalLength()
{
    VOS_UINT32                  i;
    VOS_UINT32                  j;
    VOS_UINT32                  ulShiftBits;
    VOS_UINT32                  ulLength = 0;
    VOS_UINT32                  ulNvLen;
    VOS_UINT32                  ulTempNum;
    VOS_UINT32                  ulArrayNum;
    VOS_UINT16                 *pusArray;
    VOS_UINT32                  ulBands;
    APM_RF_NV_INFO_STRU        *pstApmNvInfo;
    VOS_UINT32                  ulWBandNum;
    VOS_UINT32                  ulWOneBandLen;

    /*lint -e40 */
    APM_INIT_NV_CODE()
    /*lint +e40 */

    ulArrayNum      = sizeof(astPhyOamNvInfo)/sizeof(APM_RF_NV_INFO_STRU);
    pstApmNvInfo    = astPhyOamNvInfo;

    LogPrint1("APM_GetNvTotalLength: ulArrayNum: %d.\n",(VOS_INT32)ulArrayNum);


    /* All except W Bands. */
    for (i = 0 ; i < (ulArrayNum - RFA_UE_SUPPORT_MAX_BAND_NUM); i++)
    {
        ulTempNum = pstApmNvInfo[i].ulNum;
        pusArray  = pstApmNvInfo[i].pusArray;

        for(j=0; j<ulTempNum; j++)
        {
            if(NV_OK != NV_GetLength(pusArray[j], &ulNvLen))
            {
                g_astDspConfigCtrl[MODEM_ID_0].ulGetNvLenErrId = pusArray[j];

                return VOS_NULL_LONG;
            }

            ulLength += ulNvLen;
        }

        LogPrint1("APM_GetNvTotalLength: ulArrayNum: %d.\n",(VOS_INT32)ulLength);
    }

    ulWBandNum      = 0;
    ulWOneBandLen   = 0;

    /* W Bands */
    for (; i < ulArrayNum; i++)
    {
        ulShiftBits = i - (ulArrayNum - RFA_UE_SUPPORT_MAX_BAND_NUM);
        ulBands = g_ulUeWcdmaBands >> ulShiftBits;

        if ( VOS_NULL == ( ulBands & 0x1) )
        {
            continue;
        }

        if ( VOS_NULL_PTR == pstApmNvInfo[i].pusArray)
        {
            g_astDspConfigCtrl[MODEM_ID_0].ulGetNvLenErrId = en_NV_Item_WG_RF_MAIN_BAND;

            return VOS_NULL_LONG;
        }

        ulWBandNum++;

        ulTempNum = pstApmNvInfo[i].ulNum;
        pusArray  = pstApmNvInfo[i].pusArray;

        for(j=0; j<ulTempNum; j++)
        {
            if(NV_OK != NV_GetLength(pusArray[j], &ulNvLen))
            {
                g_astDspConfigCtrl[MODEM_ID_0].ulGetNvLenErrId = pusArray[j];

                return VOS_NULL_LONG;
            }

            ulLength += ulNvLen;

            ulWOneBandLen += ulNvLen;
        }

        LogPrint1("APM_GetNvTotalLength: ulArrayNum: %d.\n",(VOS_INT32)ulLength);
    }

    LogPrint2("APM_GetNvTotalLength: ulLength: %d, ulWBandNum :%d.\n",
            (VOS_INT32)ulLength, (VOS_INT32)ulWBandNum);

    /* 除0保护 */
    if ( 0 == ulWBandNum )
    {
        LogPrint2("APM_GetNvTotalLength: warning Func Id11111:  Id: ulLength: %d, ulWBandNum :%d.\n",
                (VOS_INT32)ulLength, (VOS_INT32)ulWBandNum);

        return ulLength;
    }

    /* 把剩余的长度补充进去，便于加上保护位，由DSP去判断 */
    ulLength += APM_GetNvSpareLen( ulWBandNum, ulWOneBandLen/ulWBandNum);

    LogPrint2("APM_GetNvTotalLength: ulLength: %d, ulWBandNum :%d.\n",
            (VOS_INT32)ulLength, (VOS_INT32)ulWBandNum);

    return ulLength;
}

/*****************************************************************************
 Function   :APM_GetNvTotalData
 Description:Get Dsp Nv Data
 Input      :pucData:Storm the Nv Data
 Return     :VOS_ERR/VOS_OK
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_GetNvTotalData(VOS_UINT8 *pucData)
{
    VOS_UINT32                  i;
    VOS_UINT32                  j;
    VOS_UINT32                  ulShiftBits;
    VOS_UINT32                  ulNvLen;
    VOS_UINT32                  ulTempNum;
    VOS_UINT32                  ulArrayNum;
    VOS_UINT16                 *pusArray;
    VOS_UINT32                  ulBands;
    APM_RF_NV_INFO_STRU        *pstApmNvInfo;

    /*lint -e40 */
    APM_INIT_NV_CODE()
    /*lint +e40 */

    ulArrayNum = sizeof(astPhyOamNvInfo)/sizeof(APM_RF_NV_INFO_STRU);
    pstApmNvInfo = astPhyOamNvInfo;

    /* All except W Bands. */
    for (i = 0 ; i < (ulArrayNum - RFA_UE_SUPPORT_MAX_BAND_NUM); i++)
    {
        ulTempNum = pstApmNvInfo[i].ulNum;
        pusArray  = pstApmNvInfo[i].pusArray;

        for(j=0; j<ulTempNum; j++)
        {
            if(NV_OK != NV_GetLength(pusArray[j], &ulNvLen))
            {
                return VOS_ERR;
            }

            if(NV_OK != NV_ReadEx(MODEM_ID_0,pusArray[j], pucData, ulNvLen))
            {
                g_astDspConfigCtrl[MODEM_ID_0].ulGetNvErrId = pusArray[j];

                return VOS_ERR;
            }

            pucData += ulNvLen;
        }
    }

    /* W Bands */
    for (; i < ulArrayNum; i++)
    {
        ulShiftBits = i - (ulArrayNum - RFA_UE_SUPPORT_MAX_BAND_NUM);
        ulBands = g_ulUeWcdmaBands >> ulShiftBits;

        if ( VOS_NULL == ( ulBands & 0x1) )
        {
            continue;
        }

        ulTempNum = pstApmNvInfo[i].ulNum;
        pusArray = pstApmNvInfo[i].pusArray;

        for(j=0; j<ulTempNum; j++)
        {
            if(NV_OK != NV_GetLength(pusArray[j], &ulNvLen))
            {
                return VOS_ERR;
            }

            if(NV_OK != NV_ReadEx(MODEM_ID_0,pusArray[j], pucData, ulNvLen))
            {
                g_astDspConfigCtrl[MODEM_ID_0].ulGetNvErrId = pusArray[j];

                return VOS_ERR;
            }

            pucData += ulNvLen;
        }
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   :APM_ConfigDspNvInit
 Description:APM init DSP NV data
 Input      :ulCtrlTagValue:Download Control Tag
 Return     :VOS_ERR/VOS_OK
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_GetDspNvData(VOS_VOID)
{
    VOS_UINT32                          ulNvWordlength;

    g_astDspConfigCtrl[MODEM_ID_0].ulGetNvLenSlice = OM_GetSlice();              /*记录当前操作时间*/

    g_astDspConfigCtrl[MODEM_ID_0].ulDspNvTotalLen = APM_GetNvTotalLength();  /*记录写入数据总长度*/

    if ( VOS_NULL_LONG == g_astDspConfigCtrl[MODEM_ID_0].ulDspNvTotalLen )
    {
        g_astDspConfigCtrl[MODEM_ID_0].enErrorCode = DSP_NVREAD_LENERR;          /*记录当前的错误值*/

        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_GetDspNvData:Get DSP NV Len Err.");

        return VOS_ERR;
    }

    /*当前获取的NV数据总长度如果是奇数，认为是NV数据错误*/
    if ( 0 != (g_astDspConfigCtrl[MODEM_ID_0].ulDspNvTotalLen % 2) )
    {
        g_astDspConfigCtrl[MODEM_ID_0].enErrorCode = DSP_NVREAD_TOTALLENERR;      /*记录当前的错误值*/

        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_GetDspNvData:Get DSP NV Total Len is Error.");

        return VOS_ERR;
    }

    /*申请全局变量把数据保存下来，在解复位dSp的时候写入, 多申请4个自己用于保存末尾标记*/
    g_astDspConfigCtrl[MODEM_ID_0].pusNVData =
                            (VOS_UINT16*)VOS_MemAlloc(UEPS_PID_APM, STATIC_MEM_PT,
                                                g_astDspConfigCtrl[MODEM_ID_0].ulDspNvTotalLen+(4*sizeof(VOS_UINT32)));

    if(VOS_NULL_PTR == g_astDspConfigCtrl[MODEM_ID_0].pusNVData)
    {
        g_astDspConfigCtrl[MODEM_ID_0].enErrorCode = DSP_NVREAD_ALLOCMEMERR;

        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_GetDspNvData:Alloc Memory is Failed.");

        return VOS_ERR;
    }

    if(VOS_OK != APM_GetNvTotalData((VOS_UINT8 *)g_astDspConfigCtrl[MODEM_ID_0].pusNVData))
    {
        g_astDspConfigCtrl[MODEM_ID_0].enErrorCode = DSP_NVREAD_READNVERR;

        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_GetDspNvData:Read NV Data is Failed.");

        return VOS_ERR;         /*异常退出后会重启不需要释放内存*/
    }

    ulNvWordlength = g_astDspConfigCtrl[MODEM_ID_0].ulDspNvTotalLen/2;

    g_astDspConfigCtrl[MODEM_ID_0].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_1;
    g_astDspConfigCtrl[MODEM_ID_0].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_2;
    g_astDspConfigCtrl[MODEM_ID_0].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_3;
    g_astDspConfigCtrl[MODEM_ID_0].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_4;
    g_astDspConfigCtrl[MODEM_ID_0].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_5;
    g_astDspConfigCtrl[MODEM_ID_0].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_6;
    g_astDspConfigCtrl[MODEM_ID_0].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_7;
    g_astDspConfigCtrl[MODEM_ID_0].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_8;

    return VOS_OK;
}

/*****************************************************************************
 Function   :APM_ConfigDspNv
 Description:APM Download the NV data To Dsp Memory
 Input      :ulCtrlTagValue:Download Control Tag
 Return     :VOS_ERR/VOS_OK
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_ConfigDspNv(VOS_BOOL bNeedReReadNV)
{
    /*判断之前启动是否发生错误*/
    if((DSP_NVREAD_NOERROR != g_astDspConfigCtrl[MODEM_ID_0].enErrorCode)
        ||(VOS_NULL_PTR == g_astDspConfigCtrl[MODEM_ID_0].ulDspNVConfigAddr))
    {
        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_ConfigDspNv:Init NV Data is Failed.");

        return VOS_ERR;
    }

    if(VOS_TRUE == bNeedReReadNV)
    {
        /*重新读取全部NV项内容，由于NV项长度不变，因此不需要重新分配内存*/
        if(VOS_OK != APM_GetNvTotalData((VOS_UINT8 *)g_astDspConfigCtrl[MODEM_ID_0].pusNVData))
        {
            g_astDspConfigCtrl[MODEM_ID_0].enErrorCode = DSP_NVREAD_READNVERR;

            PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_ConfigDspNv:Read NV Data is Failed.");

            return VOS_ERR;
        }
    }

    /*写入配置信息,多写16字节保护字*/
    HPA_PutDspMem((VOS_CHAR *)g_astDspConfigCtrl[MODEM_ID_0].pusNVData,
                    (VOS_CHAR *)g_astDspConfigCtrl[MODEM_ID_0].ulDspNVConfigAddr,
                    (VOS_INT)(g_astDspConfigCtrl[MODEM_ID_0].ulDspNvTotalLen+(4*sizeof(VOS_UINT32))));

    return VOS_OK;
}

#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
/*****************************************************************************
 Function   :APM_GetGDsp1NvTotalLength
 Description:APM init GDSP 1 NV data
 Input      :ulCtrlTagValue:Download Control Tag
 Return     :VOS_ERR/VOS_OK
 Other      :
 *****************************************************************************/

VOS_UINT32 APM_GetGDsp1NvTotalLength(VOS_VOID)
{
    VOS_UINT32              i;
    VOS_UINT32              j;
    VOS_UINT32              ulNvLen;
    VOS_UINT32              ulLength = 0;
    VOS_UINT32              ulArrayNum;
    APM_RF_NV_INFO_STRU     *pstApmNvInfo;

    /*lint -e40 */
    APM_INITGDSP1_NV_CODE()
    /*lint +e40 */

    ulArrayNum      = sizeof(astGPhy1OamNvInfo)/sizeof(APM_RF_NV_INFO_STRU);
    pstApmNvInfo    = astGPhy1OamNvInfo;

    LogPrint1("APM_GetNvTotalLength: ulArrayNum: %d.\n",(VOS_INT32)ulArrayNum);

    for(i=0; i<ulArrayNum; i++)
    {
        for(j=0; j<pstApmNvInfo[i].ulNum; j++)
        {
            if(NV_OK != NV_GetLength(pstApmNvInfo[i].pusArray[j], &ulNvLen))
            {
                g_astDspConfigCtrl[MODEM_ID_1].ulGetNvLenErrId = pstApmNvInfo[i].pusArray[j];

                return VOS_NULL_LONG;
            }

            ulLength += ulNvLen;
        }
    }

    return ulLength;
}

/*****************************************************************************
 Function   :APM_GetGDsp1NvTotalData
 Description:APM init GDSP 1 NV data
 Input      :ulCtrlTagValue:Download Control Tag
 Return     :VOS_ERR/VOS_OK
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_GetGDsp1NvTotalData(VOS_UINT8 *pucData)
{
    VOS_UINT32              i;
    VOS_UINT32              j;
    VOS_UINT32              ulNvLen;
    VOS_UINT8               *pucAddr;
    VOS_UINT32              ulArrayNum;
    APM_RF_NV_INFO_STRU     *pstApmNvInfo;

    /*lint -e40 */
    APM_INITGDSP1_NV_CODE()
    /*lint +e40 */

    g_astDspConfigCtrl[MODEM_ID_1].ulGetNvLenSlice = OM_GetSlice();              /*记录当前操作时间*/

    ulArrayNum      = sizeof(astGPhy1OamNvInfo)/sizeof(APM_RF_NV_INFO_STRU);
    pstApmNvInfo    = astGPhy1OamNvInfo;

    LogPrint1("APM_GetNvTotalLength: ulArrayNum: %d.\n",(VOS_INT32)ulArrayNum);

    pucAddr = pucData;

    for(i=0; i<ulArrayNum; i++)
    {
        for(j=0; j<pstApmNvInfo[i].ulNum; j++)
        {
            if(NV_OK != NV_GetLength(pstApmNvInfo[i].pusArray[j], &ulNvLen))
            {
                return VOS_ERR;
            }

            if(NV_OK != NV_ReadEx(MODEM_ID_1, pstApmNvInfo[i].pusArray[j], pucAddr, ulNvLen))
            {
                g_astDspConfigCtrl[MODEM_ID_1].ulGetNvErrId = pstApmNvInfo[i].pusArray[j];

                return VOS_ERR;
            }

            pucAddr += ulNvLen;
        }
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   :APM_GetGDsp1NvData
 Description:APM init G DSP 1 NV data
 Input      :ulCtrlTagValue:Download Control Tag
 Return     :VOS_ERR/VOS_OK
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_GetGDsp1NvData(VOS_VOID)
{
    VOS_UINT32                          ulNvWordlength;

    g_astDspConfigCtrl[MODEM_ID_1].ulGetNvLenSlice = OM_GetSlice();              /*记录当前操作时间*/

    g_astDspConfigCtrl[MODEM_ID_1].ulDspNvTotalLen = APM_GetGDsp1NvTotalLength();  /*记录写入数据总长度*/

    if ( VOS_NULL_LONG == g_astDspConfigCtrl[MODEM_ID_1].ulDspNvTotalLen )
    {
        g_astDspConfigCtrl[MODEM_ID_1].enErrorCode = DSP_NVREAD_LENERR;          /*记录当前的错误值*/

        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_GetGDsp1NvData:Get DSP NV Len Err.");

        return VOS_ERR;
    }

    /*当前获取的NV数据总长度如果是奇数，认为是NV数据错误*/
    if ( 0 != (g_astDspConfigCtrl[MODEM_ID_1].ulDspNvTotalLen % 2) )
    {
        g_astDspConfigCtrl[MODEM_ID_1].enErrorCode = DSP_NVREAD_TOTALLENERR;      /*记录当前的错误值*/

        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_GetGDsp1NvData:Get DSP NV Len Err.");

        return VOS_ERR;
    }

    /*申请全局变量把数据保存下来，在解复位dSp的时候写入, 多申请16个byte用于保存末尾标记*/
    g_astDspConfigCtrl[MODEM_ID_1].pusNVData =
                            (VOS_UINT16*)VOS_MemAlloc(UEPS_PID_APM, STATIC_MEM_PT,
                                                g_astDspConfigCtrl[MODEM_ID_1].ulDspNvTotalLen+(4*sizeof(VOS_UINT32)));

    if(VOS_NULL_PTR == g_astDspConfigCtrl[MODEM_ID_1].pusNVData)
    {
        g_astDspConfigCtrl[MODEM_ID_1].enErrorCode = DSP_NVREAD_ALLOCMEMERR;

        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_GetGDsp1NvData:Alloc Memory is Failed.");

        return VOS_ERR;
    }

    if(VOS_OK != APM_GetGDsp1NvTotalData((VOS_UINT8*)g_astDspConfigCtrl[MODEM_ID_1].pusNVData))
    {
        g_astDspConfigCtrl[MODEM_ID_1].enErrorCode = DSP_NVREAD_READNVERR;

        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_GetGDsp1NvData:Read NV Data is Failed.");

        return VOS_ERR;         /*异常退出后会重启不需要释放内存*/
    }

    ulNvWordlength = g_astDspConfigCtrl[MODEM_ID_1].ulDspNvTotalLen/2;

    g_astDspConfigCtrl[MODEM_ID_1].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_1;
    g_astDspConfigCtrl[MODEM_ID_1].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_2;
    g_astDspConfigCtrl[MODEM_ID_1].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_3;
    g_astDspConfigCtrl[MODEM_ID_1].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_4;
    g_astDspConfigCtrl[MODEM_ID_1].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_5;
    g_astDspConfigCtrl[MODEM_ID_1].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_6;
    g_astDspConfigCtrl[MODEM_ID_1].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_7;
    g_astDspConfigCtrl[MODEM_ID_1].pusNVData[ulNvWordlength++] = COMM_EXT_DATA_MEM_PROTECT_BIT_8;

    return VOS_OK;
}

/*****************************************************************************
 Function   :APM_ConfigDspNv
 Description:APM Download the NV data To Dsp Memory
 Input      :ulCtrlTagValue:Download Control Tag
 Return     :VOS_ERR/VOS_OK
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_ConfigGDsp1Nv(VOS_BOOL bNeedReReadNV)
{
    /*判断之前启动是否发生错误*/
    if((DSP_NVREAD_NOERROR != g_astDspConfigCtrl[MODEM_ID_1].enErrorCode)
        ||(VOS_NULL_PTR == g_astDspConfigCtrl[MODEM_ID_1].ulDspNVConfigAddr))
    {
        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_ConfigGDsp1Nv:Init NV Data is Failed.");

        return VOS_ERR;     /*异常退出后会重启不需要释放内存*/
    }

    if(VOS_TRUE == bNeedReReadNV)
    {
        /*重新读取全部NV项内容，由于NV项长度不变，因此不需要重新分配内存*/
        if(VOS_OK != APM_GetGDsp1NvTotalData((VOS_UINT8*)g_astDspConfigCtrl[MODEM_ID_1].pusNVData))
        {
            g_astDspConfigCtrl[MODEM_ID_1].enErrorCode = DSP_NVREAD_READNVERR;

            PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," APM_ConfigGDsp1Nv:Read NV Data is Failed.");

            return VOS_ERR;
        }
    }

    /*写入配置信息,多写16字节保护字*/
    HPA_PutDspMem((VOS_CHAR *)g_astDspConfigCtrl[MODEM_ID_1].pusNVData,
                    (VOS_CHAR *)g_astDspConfigCtrl[MODEM_ID_1].ulDspNVConfigAddr,
                    (VOS_INT)(g_astDspConfigCtrl[MODEM_ID_1].ulDspNvTotalLen+(4*sizeof(VOS_UINT32))));

    return VOS_OK;
}
#else
/*****************************************************************************
 Function   :APM_GetGDsp1NvData
 Description:APM init G DSP 1 NV data
 Input      :ulCtrlTagValue:Download Control Tag
 Return     :VOS_ERR/VOS_OK
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_GetGDsp1NvData(VOS_VOID)
{
    return VOS_OK;
}

/*****************************************************************************
 Function   :APM_ConfigDspNv
 Description:APM Download the NV data To Dsp Memory
 Input      :ulCtrlTagValue:Download Control Tag
 Return     :VOS_ERR/VOS_OK
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_ConfigGDsp1Nv(VOS_BOOL bNeedReReadNV)
{
    return VOS_ERR;
}

#endif


VOS_VOID APM_ReportEvent(VOS_UINT ulEventData)
{
    PS_OM_EVENT_IND_STRU stEventInd;

    stEventInd.ulLength   = sizeof(PS_OM_EVENT_IND_STRU) - sizeof(stEventInd.ulLength);

    stEventInd.usEventId  = 0;

    stEventInd.ulModuleId = UEPS_PID_APM;

    *((VOS_UINT*)stEventInd.aucData) = ulEventData;

    OM_Event(&stEventInd);

    return;
}


VOS_UINT32 SHPA_ActiveDSP(VOS_RATMODE_ENUM_UINT32 enRatMode, MODEM_ID_ENUM_UINT16 enModemId)
{
    UPHY_OAM_ACTIVE_UPHY_REQ_STRU *pstMsg;

    /* Add for L Only version */
    if (BOARD_TYPE_LTE_ONLY == BSP_OM_GetBoardType())
    {
        return VOS_OK;
    }

    if((MODEM_ID_BUTT <= enModemId)||(VOS_RATMODE_LTE <= enRatMode))
    {
        PS_LOG(UEPS_PID_APM, 0, PS_PRINT_ERROR,"SHPA_ActiveDSP: Para Err");
        return VOS_ERR;
    }

    /*lint -e662 -e661*/

    /* 已经激活PHY，则不用再激活 */
    if (VOS_TRUE == g_aulShpaActiveState[enModemId][enRatMode])
    {
        LogPrint1("SHPA_ActiveDSP: Modem %d Has Actived !\r\n", enModemId);

        return VOS_OK;
    }

    LogPrint1("SHPA_ActiveDSP: Modem %d Active Start !\r\n", enModemId);

    g_astDspConfigCtrl[enModemId].ulDSPActiveSlice = OM_GetSlice();

    if (MODEM_ID_1 == enModemId)
    {
#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
        I1_SLEEP_VoteLock(enRatMode);

        I1_SLEEP_PowerUp(enRatMode);
#endif
    }
    else
    {
        SLEEP_VoteLock(enRatMode);

        SLEEP_PowerUp(enRatMode);
    }

    /* 接收DSP的消息的PID是SHPA*/
    pstMsg = (UPHY_OAM_ACTIVE_UPHY_REQ_STRU*)VOS_AllocMsg(UEPS_PID_SHPA, sizeof(UPHY_OAM_ACTIVE_UPHY_REQ_STRU)- VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pstMsg)  /*此处失败会复位，因此不需要记录错误*/
    {
        PS_LOG(UEPS_PID_APM, 0, PS_PRINT_ERROR,"SHPA_ActiveDSP: VOS_AllocMsg Err");
        return VOS_ERR;
    }

    if(MODEM_ID_1 == enModemId)
    {
        pstMsg->ulReceiverPid   = I1_DSP_PID_APM;
    }
    else
    {
        pstMsg->ulReceiverPid   = I0_DSP_PID_APM;
    }

    pstMsg->enMsgId         = ID_OAM_UPHY_ACTIVE_UPHY_REQ;
    pstMsg->enModem         = enModemId;
    pstMsg->enRatTpye       = enRatMode;

    if(VOS_ERR == VOS_SendMsg(UEPS_PID_SHPA, pstMsg))
    {
        PS_LOG(UEPS_PID_APM, 0, PS_PRINT_ERROR,"SHPA_ActiveDSP: VOS_SendMsg Err");
        return VOS_ERR;
    }

    g_astDspConfigCtrl[enModemId].enActiveState = APM_SENDED_MSG;

    g_astDspConfigCtrl[enModemId].ulActiveSmPResult
                = VOS_SmP(g_aulShpaActivePhySemaphor[enModemId], SHPA_TMR_LOAD_PHY_INTERVAL);

    if( VOS_OK != g_astDspConfigCtrl[enModemId].ulActiveSmPResult)
    {
        PS_LOG1(UEPS_PID_APM, 0, PS_PRINT_ERROR,"SHPA_ActiveDSP: Modem %d g_ulShpaActivePhySemaphor Timeout!\r\n", enModemId);

        g_astDspConfigCtrl[enModemId].enErrorCode = DSP_ACTIVE_TIME_OUT;

        VOS_ProtectionReboot(OAM_PROTECTION_DSP_Init, VOS_FILE_ID, __LINE__, (VOS_CHAR *)(g_astDspConfigCtrl), sizeof(g_astDspConfigCtrl));

        return VOS_ERR;
    }

    if(UPHY_OAM_RSLT_SUCC != g_astDspConfigCtrl[enModemId].ulActiveResult)
    {
        PS_LOG1(UEPS_PID_APM, 0, PS_PRINT_ERROR,"SHPA_ActiveDSP: Modem %d Active FAIL!\r\n", enModemId);

        VOS_ProtectionReboot(OAM_PROTECTION_DSP_Init, VOS_FILE_ID, __LINE__, (VOS_CHAR *)(g_astDspConfigCtrl), sizeof(g_astDspConfigCtrl));

        return VOS_ERR;
    }

    /* 已经激活 */
    g_aulShpaActiveState[enModemId][enRatMode] = VOS_TRUE;

    /*lint +e662 +e661*/

#if (RAT_MODE != RAT_GU)
#if(FEATURE_ON == FEATURE_TEMP_MULTI_MODE_LP)
    /* 写上电保护标志位 */
/*lint -e413 -e10 -e40*/
    HPA_Write32Reg(DSP_TEMP_BASE_ADDR+PHY_ARM_POWER_INIT_OFFSET, 0);
/*lint +e413 +e10 +e40*/
#endif
#endif

    LogPrint1("SHPA_ActiveDSP: Modem %d Active end !\r\n", enModemId);

    return VOS_OK;
}

/*****************************************************************************
 Function   :APM_DSPBufferHook
 Description:Dsp Buffer Read and Send to SDT
 Input      :ulLength:Read the Data length
            :ulDataAddr:Read the Memory Addr
 Return     :VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID APM_DSPBufferHook(VOS_UINT32 ulLength,VOS_UINT32 ulDataAddr)
{
    PS_APM_OM_TRANS_IND_STRU *pstTransMsg;

    pstTransMsg = (PS_APM_OM_TRANS_IND_STRU *)VOS_AllocMsg(UEPS_PID_APM, (sizeof(PS_APM_OM_TRANS_IND_STRU) - VOS_MSG_HEAD_LENGTH) + ulLength);

    if (VOS_NULL_PTR == pstTransMsg)
    {
        return;
    }

    pstTransMsg->ulReceiverPid  = WUEPS_PID_OM;
    pstTransMsg->usTransPrimId  = 0x5001;               /*固定填写0x5001*/
    pstTransMsg->ucFuncType     = 4;                    /*对于透明消息，功能类型固定填写4*/
    pstTransMsg->usAppLength    = (VOS_UINT16)(12 + ulLength);        /*14为透明消息结构体usAppLength之后的长度*/
    pstTransMsg->ulTimeStamp    = OM_GetSlice();
    pstTransMsg->usPrimId       = 0xF081;
    pstTransMsg->usToolsId      = 0;

    if(0 != ulLength)        /*当前存在回复文件内容*/
    {
        HPA_GetDspMem((char *)ulDataAddr,(char *)&pstTransMsg->aucData[0],(int)ulLength);
    }

    (VOS_VOID)VOS_SendMsg(pstTransMsg->ulSenderPid, pstTransMsg);

    return;
}


VOS_UINT16 SHPA_LoadPhy( VOS_RATMODE_ENUM_UINT32              enMode,
                                MODEM_ID_ENUM_UINT16                enModemId,
                                UPHY_OAM_BUSINESS_TYPE_ENUM_UINT16  enBusiness )
{
    PHY_OAM_LOAD_PHY_REQ_STRU           *pstLoadReq;

    if((MODEM_ID_BUTT <= enModemId)||(VOS_RATMODE_LTE <= enMode)||(UPHY_OAM_BUSINESS_TYPE_BUTT <= enBusiness))
    {
        LogPrint3("SHPA_LoadPhy: Para Error! Modem id is %d Modem is %d Type is %d\r\n",
                                                    (VOS_INT)enModemId, (VOS_INT)enMode, (VOS_INT)enBusiness);
        return VOS_FALSE;
    }

    LogPrint1("SHPA_LoadPhy: Modme %d Start !\r\n", enModemId);

    g_astDspConfigCtrl[enModemId].ulDSPLoadSlice = OM_GetSlice();

    pstLoadReq = (PHY_OAM_LOAD_PHY_REQ_STRU *)VOS_AllocMsg(UEPS_PID_SHPA, sizeof(PHY_OAM_LOAD_PHY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstLoadReq)
    {
        LogPrint("SHPA_LoadPhy: VOS_AllocMsg Fail!\r\n");
        return VOS_FALSE;
    }

    /*Construct the LOAD_PHY_REQ*/
    pstLoadReq->enMsgId         = ID_OAM_PHY_LOAD_PHY_REQ;
    pstLoadReq->enRatMode       = enMode;
    pstLoadReq->enModem         = enModemId;
    pstLoadReq->enBusinessType  = enBusiness;

    if(VOS_RATMODE_GSM == enMode)
    {
        if(MODEM_ID_1 == enModemId)
        {
            pstLoadReq->ulReceiverPid   = I1_DSP_PID_GPHY;
        }
        else
        {
            pstLoadReq->ulReceiverPid   = I0_DSP_PID_GPHY;
        }
    }
    else
    {
        pstLoadReq->ulReceiverPid   = DSP_PID_WPHY;
    }

    if (VOS_OK != VOS_SendMsg(UEPS_PID_SHPA, pstLoadReq))
    {
        LogPrint("SHPA_LoadPhy:Waring: VOS_SendMsg Error!\r\n");
        return VOS_FALSE;
    }

    /*lint -e662 -e661*/
    g_astDspConfigCtrl[enModemId].enLoadState = APM_SENDED_MSG;

    g_astDspConfigCtrl[enModemId].ulLoadSmPResult
                    = VOS_SmP(g_aulShpaLoadPhySemaphor[enModemId],SHPA_TMR_LOAD_PHY_INTERVAL);

    if( VOS_OK !=  g_astDspConfigCtrl[enModemId].ulLoadSmPResult)
    {
        LogPrint1("SHPA_LoadPhy:Waring: Modem %d g_aulShpaLoadPhySemaphor Timeout!\r\n", enModemId);

        g_astDspConfigCtrl[enModemId].enErrorCode = CPHY_DSP_LOAD_TIMEOUT;

        APM_ReportEvent(g_astDspConfigCtrl[enModemId].enErrorCode);

        VOS_ProtectionReboot(OAM_PROTECTION_LOAD_DSP, THIS_FILE_ID, __LINE__, VOS_NULL_PTR, 0);

        return VOS_FALSE;
    }

    VOS_TaskDelay(APM_LDF_LOAD_PHY_DELAY_TIME);

    APM_ReportEvent(g_astDspConfigCtrl[enModemId].ulLoadResult);

    if(UPHY_OAM_RSLT_SUCC != g_astDspConfigCtrl[enModemId].ulLoadResult)
    {
        PS_LOG1( UEPS_PID_APM, 0, PS_PRINT_WARNING, "SHPA_LoadPhy:Modem %d Load DSP Failed, Reboot it!", enModemId);

        LogPrint1("Load DSP Failed, g_ulShpaLoadPhyStatusis %d\r\n",(VOS_INT)g_astDspConfigCtrl[enModemId].ulLoadResult);

        VOS_ProtectionReboot(OAM_PROTECTION_LOAD_DSP, THIS_FILE_ID, __LINE__, VOS_NULL_PTR, 0);

        return VOS_FALSE;
    }
    else
    {
        g_aulShpaCurSysId[enModemId] = enMode;

        if(UPHY_OAM_BUSINESS_TYPE_NORMAL == enBusiness)
        {
            OM_DspTypeInd((VOS_UINT8)enModemId,enMode, OM_APP_QUERY_DSP_CNF);
        }
    }
    /*lint +e662 +e661*/

    LogPrint1("SHPA_LoadPhy: Modme %d Load OK !\r\n", enModemId);

    return VOS_TRUE;
}

/*****************************************************************************
 Function   :APM_InitSystemAddr
 Description:Read the Dsp Nv Data
 Input      :VOS_VOID
 Return     :VOS_ERR/VOS_OK
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_InitSystemAddr(VOS_VOID)
{
    BSP_AXI_SECT_INFO       stAXIInfo;

    VOS_MemSet(&g_stSysAddr, 0, sizeof(g_stSysAddr));

    g_stSysAddr.ulAHBBaseAddr       = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_AHB);

    if(0 == g_stSysAddr.ulAHBBaseAddr)
    {
        LogPrint("APM_InitSystemAddr: BSP_IP_TYPE_AHB Addr Get Error!\r\n");
        return VOS_ERR;
    }

    g_stSysAddr.ulWBBPBaseAddr      = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_WBBP);

    if(0 == g_stSysAddr.ulWBBPBaseAddr)
    {
        LogPrint("APM_InitSystemAddr: BSP_IP_TYPE_WBBP Addr Get Error!\r\n");
        return VOS_ERR;
    }

    g_stSysAddr.ulWBBPDRXBaseAddr   = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_WBBP_DRX);

    if(0 == g_stSysAddr.ulWBBPDRXBaseAddr)
    {
        LogPrint("APM_InitSystemAddr: BSP_IP_TYPE_WBBP_DRX Addr Get Error!\r\n");
        return VOS_ERR;
    }

    g_stSysAddr.ulGBBPBaseAddr      = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_GBBP);

    if(0 == g_stSysAddr.ulGBBPBaseAddr)
    {
        LogPrint("APM_InitSystemAddr: BSP_IP_TYPE_GBBP Addr Get Error!\r\n");
        return VOS_ERR;
    }

    g_stSysAddr.ulGBBPDRXBaseAddr   = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_GBBP_DRX);

    if(0 == g_stSysAddr.ulGBBPDRXBaseAddr)
    {
        LogPrint("APM_InitSystemAddr: BSP_IP_TYPE_GBBP_DRX Addr Get Error!\r\n");
        return VOS_ERR;
    }

    g_stSysAddr.ulGBBP1BaseAddr      = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_GBBP1);

    g_stSysAddr.ulGBBP1DRXBaseAddr   = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_GBBP1_DRX);

    /*只有双卡版本才对于此地址判断，单卡版本和V3R3不判断数据使用底软的返回值*/
#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )
    if(0 == g_stSysAddr.ulGBBP1BaseAddr)
    {
        LogPrint("APM_InitSystemAddr: BSP_IP_TYPE_GBBP1 Addr Get Error!\r\n");
        return VOS_ERR;
    }

    if(0 == g_stSysAddr.ulGBBP1DRXBaseAddr)
    {
        LogPrint("APM_InitSystemAddr: BSP_IP_TYPE_GBBP1_DRX Addr Get Error!\r\n");
        return VOS_ERR;
    }
#endif

    g_stSysAddr.ulSYSCTRLBaseAddr   = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_SYSCTRL);

    if(0 == g_stSysAddr.ulSYSCTRLBaseAddr)
    {
        LogPrint("APM_InitSystemAddr: BSP_IP_TYPE_SYSCTRL Addr Get Error!\r\n");
        return VOS_ERR;
    }

    if(VOS_OK != DRV_GET_FIX_AXI_ADDR(BSP_AXI_SECT_TYPE_HIFI, &stAXIInfo))
    {
        LogPrint("APM_InitSystemAddr: BSP_AXI_SECT_TYPE_HIFI Addr Get Error!\r\n");
        return VOS_ERR;
    }

    g_stSysAddr.ulHIFIAXIBaseAddr   = stAXIInfo.ulSectVirtAddr; /*上层用虚地址*/

    if(VOS_OK != DRV_GET_FIX_AXI_ADDR(BSP_AXI_SECT_TYPE_TEMPERATURE, &stAXIInfo))
    {
        LogPrint("APM_InitSystemAddr: BSP_AXI_SECT_TYPE_TEMPERATURE Addr Get Error!\r\n");
        return VOS_ERR;
    }

    g_stSysAddr.ulDSPTempBaseAddr   = stAXIInfo.ulSectVirtAddr; /*上层用虚地址*/

#ifdef FEATURE_DSP2ARM
    UCOM_SetTemparatureAddr(g_stSysAddr.ulDSPTempBaseAddr);
#endif

#ifdef FEATURE_DSP2ARM
    /*完成DSP共享信息的更新*/
    if ( VOS_OK != DRV_BSP_UPDATE_DSP_SHAREINFO(UCOM_GetDspShareMemAddr()) )
    {
        PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR," SHPA_AcitiveDSP:Write DSP Config Info Failed.");

        VOS_ProtectionReboot(OAM_PROTECTION_DSP_SHARE, VOS_FILE_ID, __LINE__, (VOS_CHAR *)(g_astDspConfigCtrl), sizeof(g_astDspConfigCtrl));

        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : HPA_DspBootInfoReq
 功能描述  : Handle the DSP Load NV Data Req Msg
 输入参数  : PHY_OAM_MSG_STRU *pstRcvMsg
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID HPA_DspBootInfoReq(PHY_OAM_MSG_STRU *pstMsg)
{
    UPHY_OAM_BOOT_INFO_REQ_STRU  *pstDspMsg;
    UPHY_OAM_BOOT_INFO_IND_STRU  *pstCnfMsg;

    pstDspMsg = (UPHY_OAM_BOOT_INFO_REQ_STRU *)pstMsg;

    if(MODEM_ID_BUTT <= pstDspMsg->enModem)
    {
        PS_LOG(UEPS_PID_SHPA, 0, PS_PRINT_ERROR,"HPA_DspLoadNVReq: the Msg Modem ID is Error");

        VOS_ProtectionReboot(OAM_PROTECTION_DSP_Init, VOS_FILE_ID, __LINE__, (VOS_CHAR *)(&pstDspMsg), sizeof(UPHY_OAM_BOOT_INFO_REQ_STRU));

        return;
    }

#ifdef FEATURE_DSP2ARM
    if(pstDspMsg->ulSenderPid > VOS_CPU_ID_0_PID_BUTT)
    {
        PS_LOG(UEPS_PID_SHPA, 0, PS_PRINT_ERROR,"HPA_DspLoadNVReq: the Msg Send PID is Error");

        VOS_ProtectionReboot(OAM_PROTECTION_DSP_Init, VOS_FILE_ID, __LINE__, (VOS_CHAR *)(&pstDspMsg), sizeof(UPHY_OAM_LOAD_NV_REQ_STRU));

        return;
    }
#endif

    pstCnfMsg = (UPHY_OAM_BOOT_INFO_IND_STRU*)VOS_AllocMsg(UEPS_PID_SHPA, sizeof(UPHY_OAM_BOOT_INFO_IND_STRU)- VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pstCnfMsg)   /* 申请失败会复位 */
    {
        return;
    }

    pstCnfMsg->ulReceiverPid    = pstDspMsg->ulSenderPid;
    pstCnfMsg->enModem          = pstDspMsg->enModem;
    pstCnfMsg->enRatMode        = pstDspMsg->enRatMode;
    pstCnfMsg->enMsgId          = ID_OAM_UPHY_BOOT_INFO_IND;

    pstCnfMsg->ulUmtsBandBitMap = g_ulUeWcdmaBands;

    /*配置内存相关地址，DSP如果不用需要忽略*/
    /*配置CQI表的地址,还在使用*/
    if(VOS_OK != RFA_GetFixMemInfo(BSP_DDR_SECT_TYPE_CQI, RFA_CQI_TOTAL_LENGTH, &pstCnfMsg->ulCQIAddr))
    {
        VOS_ProtectionReboot(OAM_PROTECTION_DSP_Init, VOS_FILE_ID, __LINE__, (VOS_CHAR *)(&pstCnfMsg), sizeof(UPHY_OAM_BOOT_INFO_IND_STRU));
        return;
    }

    /*配置APT表的地址*/
    if(VOS_OK != RFA_GetFixMemInfo(BSP_DDR_SECT_TYPE_APT, RFA_APT_TOTAL_LENGTH, &pstCnfMsg->ulAPTAddr))
    {
        PS_LOG(UEPS_PID_SHPA, 0, PS_PRINT_ERROR,"HPA_DspLoadNVReq: Get the BSP_DDR_SECT_TYPE_APT Addr is Error");
        pstCnfMsg->ulAPTAddr = VOS_NULL;
    }

    /*配置ET表的地址*/
    if(VOS_OK != RFA_GetFixMemInfo(BSP_DDR_SECT_TYPE_ET, RFA_ET_TOTAL_LENGTH, &pstCnfMsg->ulETAddr))
    {
        PS_LOG(UEPS_PID_SHPA, 0, PS_PRINT_ERROR,"HPA_DspLoadNVReq: Get the BSP_DDR_SECT_TYPE_ET Addr is Error");
        pstCnfMsg->ulETAddr = VOS_NULL;
    }

    (VOS_VOID)VOS_SendMsg(UEPS_PID_SHPA, pstCnfMsg);

    return;
}

/*****************************************************************************
 函 数 名  : HPA_DspLoadNVReq
 功能描述  : Handle the DSP Load NV Data Req Msg
 输入参数  : PHY_OAM_MSG_STRU *pstRcvMsg
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID HPA_DspLoadNVReq(PHY_OAM_MSG_STRU *pstMsg)
{
    UPHY_OAM_LOAD_NV_REQ_STRU    *pstDspMsg;
    UPHY_OAM_LOAD_NV_CNF_STRU    *pstCnfMsg;
    VOS_UINT32                  ulResult;

    pstDspMsg = (UPHY_OAM_LOAD_NV_REQ_STRU *)pstMsg;

    if(MODEM_ID_BUTT <= pstDspMsg->enModem)
    {
        PS_LOG(UEPS_PID_SHPA, 0, PS_PRINT_ERROR,"HPA_DspLoadNVReq: the Msg Modem ID is Error");

        VOS_ProtectionReboot(OAM_PROTECTION_DSP_Init, VOS_FILE_ID, __LINE__, (VOS_CHAR *)(&pstDspMsg), sizeof(UPHY_OAM_LOAD_NV_REQ_STRU));

        return;
    }

#ifdef FEATURE_DSP2ARM
    if(pstDspMsg->ulSenderPid > VOS_CPU_ID_0_PID_BUTT)
    {
        PS_LOG(UEPS_PID_SHPA, 0, PS_PRINT_ERROR,"HPA_DspLoadNVReq: the Msg Send PID is Error");

        VOS_ProtectionReboot(OAM_PROTECTION_DSP_Init, VOS_FILE_ID, __LINE__, (VOS_CHAR *)(&pstDspMsg), sizeof(UPHY_OAM_LOAD_NV_REQ_STRU));

        return;
    }
#endif

    pstCnfMsg = (UPHY_OAM_LOAD_NV_CNF_STRU*)VOS_AllocMsg(UEPS_PID_SHPA, sizeof(UPHY_OAM_LOAD_NV_CNF_STRU)- VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pstCnfMsg)   /* 申请失败会复位 */
    {
        return;
    }

    pstCnfMsg->ulReceiverPid= pstDspMsg->ulSenderPid;
    pstCnfMsg->enModem      = pstDspMsg->enModem;
    pstCnfMsg->enRatMode    = pstDspMsg->enRatMode;
    pstCnfMsg->enMsgId      = ID_OAM_UPHY_LOAD_NV_CNF;

    if(DSP_NVREAD_NOERROR != g_astDspConfigCtrl[pstDspMsg->enModem].enErrorCode)
    {
        ulResult = VOS_ERR;
    }
    else
    {
        g_astDspConfigCtrl[pstDspMsg->enModem].ulDspNVConfigAddr = pstDspMsg->uwNvAddr;

        /* 目前按照Modem进行配置 */
        if(MODEM_ID_1 == pstDspMsg->enModem)
        {
            ulResult = APM_ConfigGDsp1Nv(VOS_FALSE);
        }
        else
        {
            ulResult = APM_ConfigDspNv(VOS_FALSE);  /*不需要重新读取NV,初始化的时候已经完成读取*/
        }
    }

    if(VOS_OK != ulResult)
    {
        pstCnfMsg->enRslt = UPHY_OAM_RSLT_FAIL;
    }
    else
    {
        pstCnfMsg->enRslt = UPHY_OAM_RSLT_SUCC;
    }

    (VOS_VOID)VOS_SendMsg(UEPS_PID_SHPA, pstCnfMsg);

    return;
}

/*****************************************************************************
 函 数 名  : HPA_DspAcitveCnf
 功能描述  : Handle the DSP Active Cnf Msg
 输入参数  : PHY_OAM_MSG_STRU *pstRcvMsg
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID HPA_DspAcitveCnf(PHY_OAM_MSG_STRU *pstMsg)
{
    UPHY_OAM_ACTIVE_UPHY_CNF_STRU   *pstDspMsg;

    pstDspMsg = (UPHY_OAM_ACTIVE_UPHY_CNF_STRU *)pstMsg;

    if(MODEM_ID_BUTT <= pstDspMsg->enModem)
    {
        PS_LOG(UEPS_PID_SHPA, 0, PS_PRINT_ERROR,"HPA_DspAcitveCnf: the Msg Modem ID is Error");

        VOS_ProtectionReboot(OAM_PROTECTION_DSP_Init, VOS_FILE_ID, __LINE__, (VOS_CHAR *)(&pstDspMsg), sizeof(UPHY_OAM_ACTIVE_UPHY_CNF_STRU));

        return;
    }

    g_astDspConfigCtrl[pstDspMsg->enModem].ulDSPActCnfSlice = OM_GetSlice();

    g_astDspConfigCtrl[pstDspMsg->enModem].enActiveState = APM_RECEIVED_MSG;

    g_astDspConfigCtrl[pstDspMsg->enModem].ulActiveResult= pstDspMsg->enRslt;

    VOS_SmV(g_aulShpaActivePhySemaphor[pstDspMsg->enModem]);

    return;
}


/*****************************************************************************
 函 数 名  : HPA_DspLoadCnf
 功能描述  : Handle the DSP Load Cnf Msg
 输入参数  : PHY_OAM_MSG_STRU *pstRcvMsg
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID HPA_DspLoadCnf(PHY_OAM_MSG_STRU *pstMsg)
{
    PHY_OAM_LOAD_PHY_CNF_STRU   *pstDspMsg;

    pstDspMsg = (PHY_OAM_LOAD_PHY_CNF_STRU *)pstMsg;

    if(MODEM_ID_BUTT <= pstDspMsg->enModem)
    {
        PS_LOG(UEPS_PID_SHPA, 0, PS_PRINT_ERROR,"HPA_DspLoadCnf: the Msg Modem ID is Error");

        VOS_ProtectionReboot(OAM_PROTECTION_DSP_Init, VOS_FILE_ID, __LINE__, (VOS_CHAR *)(&pstDspMsg), sizeof(PHY_OAM_LOAD_PHY_CNF_STRU));

        return;
    }

    g_astDspConfigCtrl[pstDspMsg->enModem].ulDSPLoadCnfSlice = OM_GetSlice();

    g_astDspConfigCtrl[pstDspMsg->enModem].enLoadState   = APM_RECEIVED_MSG;

    g_astDspConfigCtrl[pstDspMsg->enModem].ulLoadResult = pstDspMsg->enRslt;

    VOS_SmV(g_aulShpaLoadPhySemaphor[pstDspMsg->enModem]);

    return;
}


VOS_VOID HPA_ApmMsgPIDProc(MsgBlock *pstRcvMsg)
{
#ifdef FEATURE_DSP2ARM
    PHY_OAM_MSG_STRU              *pstMsg;

    pstMsg = (PHY_OAM_MSG_STRU *)pstRcvMsg;

    switch(pstMsg->usMsgId)
    {
        case ID_PHY_OAM_LOAD_PHY_CNF:
            HPA_DspLoadCnf(pstMsg);
            break;

        case ID_UPHY_OAM_ACTIVE_UPHY_CNF:
            HPA_DspAcitveCnf(pstMsg);
            break;

        case ID_UPHY_OAM_LOAD_NV_REQ:
            HPA_DspLoadNVReq(pstMsg);
            break;

        case ID_UPHY_OAM_BOOT_INFO_REQ:
            HPA_DspBootInfoReq(pstMsg);
            break;

        default:
            break;
    }

#endif
    return;
}

/*****************************************************************************
 Function   : APM_InitAPMGlobal
 Description: Init one memory control block
 Input      : VOS_MemCtrlBlock -- address of control block
            : ulBlockLength -- length
            : ulTotalBlockNumber -- number
            : ulAddress -- the start of address
 Return     : void
 Other      :
 *****************************************************************************/

VOS_VOID APM_InitAPMGlobal(VOS_VOID)
{
    VOS_UINT32  i;

    for(i=0; i<MODEM_ID_BUTT; i++)
    {
        g_aulShpaCurSysId[i] = VOS_RATMODE_BUTT;
    }

    VOS_MemSet(g_astDspConfigCtrl, 0, sizeof(g_astDspConfigCtrl));

    VOS_MemSet(g_aulShpaActiveState, 0, sizeof(g_aulShpaActiveState));

    return;
}

/*****************************************************************************
 Function   : APM_PIDInit
 Description: Init one memory control block
 Input      : VOS_MemCtrlBlock -- address of control block
            : ulBlockLength -- length
            : ulTotalBlockNumber -- number
            : ulAddress -- the start of address
 Return     : void
 Other      :
 *****************************************************************************/
VOS_UINT32 APM_PIDInit( enum VOS_INIT_PHASE_DEFINE ip )
{
    VOS_UINT32 ulWcdmaBands;
    VOS_UINT32          i;

    switch ( ip )
    {
        case VOS_IP_INITIAL:
            APM_InitAPMGlobal();

#ifndef FEATURE_DSP2ARM
            /*lint -e413 -e10 -e40*/
            HPA_Write32Reg(ZSP_SLEEP_TYPE_ADDR, ZSP_SLEEP_INIT | ZSP_SLEEP_NORMAL);
            /*lint +e413 +e10 +e40*/
#endif

            if(NV_OK != NV_ReadPart(en_NV_Item_WG_RF_MAIN_BAND,
                                    0, &ulWcdmaBands, sizeof(VOS_UINT32)))
            {
                return VOS_ERR;
            }

            /*需要保证下发给物理层的频段值不为空*/
            ulWcdmaBands = (RFA_CPHY_SUPPORT_BANDS & ulWcdmaBands);

            if(0 == ulWcdmaBands)
            {
                return VOS_ERR;
            }

            g_ulUeWcdmaBands = ulWcdmaBands;

            for(i=0; i<MODEM_ID_BUTT; i++)
            {
                if ( VOS_OK != VOS_SmCCreate( "sem4", 0, VOS_SEMA4_FIFO,
                                            &g_aulShpaLoadPhySemaphor[i]) )
                {
                    PS_LOG(  UEPS_PID_APM, 0, PS_PRINT_WARNING, "APM_PIDInit: Create Semaphore<sem4> Error!!!");
                    return VOS_ERR;
                }

                if ( VOS_OK != VOS_SmCCreate( "sem5", 0, VOS_SEMA4_FIFO,
                                            &g_aulShpaActivePhySemaphor[i]) )
                {
                    PS_LOG(  UEPS_PID_APM, 0, PS_PRINT_WARNING, "APM_PIDInit: Create Semaphore<sem5> Error!!!");
                    return VOS_ERR;
                }
            }

            if ( VOS_OK != VOS_SmCCreate( "Ldf", 1, VOS_SEMA4_FIFO,
                                            &g_ulLdfSavingSemaphor) )
            {
                PS_LOG(  UEPS_PID_APM, 0, PS_PRINT_WARNING, "APM_PIDInit: Create Semaphore<Ldf> Error!!!");
                return VOS_ERR;
            }

            break;

        case VOS_IP_LOAD_DATA:
            if ( VOS_OK != RFA_CQITableProc() )
            {
                PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR, "APM_PIDInit: Load CQI Error!!!");

                return VOS_ERR;
            }

            if ( VOS_OK != APM_GetDspNvData() )
            {
                PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR, "APM_PIDInit: DSP Init Error!!!");

                return VOS_ERR;
            }

            if ( VOS_OK != APM_GetGDsp1NvData())
            {
                PS_LOG( UEPS_PID_APM, 0, PS_PRINT_ERROR, "APM_PIDInit: G DSP 1 Init Error!!!");

                return VOS_ERR;
            }
            break;

        default:
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   : HPA_ShowSystemAddr
 Description: Show memory Addr Inof
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_ShowSystemAddr(VOS_VOID)
{
    vos_printf("\r\nThe AHBBaseAddr     is 0x%x",g_stSysAddr.ulAHBBaseAddr    );
    vos_printf("\r\nThe WBBPBaseAddr    is 0x%x",g_stSysAddr.ulWBBPBaseAddr   );
    vos_printf("\r\nThe WBBPDRXBaseAddr is 0x%x",g_stSysAddr.ulWBBPDRXBaseAddr);
    vos_printf("\r\nThe GBBPBaseAddr    is 0x%x",g_stSysAddr.ulGBBPBaseAddr   );
    vos_printf("\r\nThe GBBPDRXBaseAddr is 0x%x",g_stSysAddr.ulGBBPDRXBaseAddr);
    vos_printf("\r\nThe GBBP1BaseAddr   is 0x%x",g_stSysAddr.ulGBBP1BaseAddr   );
    vos_printf("\r\nThe GBBP1DRXBaseAddris 0x%x",g_stSysAddr.ulGBBP1DRXBaseAddr);
    vos_printf("\r\nThe SYSCTRLBaseAddr is 0x%x",g_stSysAddr.ulSYSCTRLBaseAddr);
    vos_printf("\r\nThe HIFIAXIBaseAddr is 0x%x",g_stSysAddr.ulHIFIAXIBaseAddr);
    vos_printf("\r\nThe DSPTempBaseAddr is 0x%x",g_stSysAddr.ulDSPTempBaseAddr);

    return ;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



